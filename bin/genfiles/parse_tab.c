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
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){union Cyc_YYSTYPE _T0;struct _fat_ptr _T1;struct _RegionHandle*_T2;void*_T3;struct Cyc_Yystacktype*_T4;struct Cyc_Yystacktype*_T5;struct _RegionHandle*_T6;unsigned _T7;unsigned _T8;unsigned _T9;struct _fat_ptr _TA;int _TB;char*_TC;short*_TD;int _TE;int _TF;int _T10;int _T11;struct _fat_ptr _T12;int _T13;int _T14;struct Cyc_Yystack_overflow_exn_struct*_T15;struct Cyc_Yystack_overflow_exn_struct*_T16;struct _fat_ptr _T17;int _T18;short*_T19;struct _RegionHandle*_T1A;unsigned _T1B;int _T1C;unsigned _T1D;int _T1E;unsigned _T1F;unsigned _T20;struct _fat_ptr _T21;unsigned char*_T22;short*_T23;unsigned _T24;int _T25;unsigned _T26;struct _fat_ptr _T27;int _T28;struct Cyc_Yystacktype*_T29;struct _RegionHandle*_T2A;unsigned _T2B;int _T2C;unsigned _T2D;int _T2E;unsigned _T2F;unsigned _T30;struct _fat_ptr _T31;unsigned _T32;int _T33;char*_T34;struct Cyc_Yystacktype*_T35;unsigned _T36;struct _fat_ptr _T37;char*_T38;struct Cyc_Yystacktype*_T39;short*_T3A;int _T3B;char*_T3C;short*_T3D;short _T3E;int _T3F;int _T40;int _T41;int _T42;struct Cyc_Lexing_lexbuf*_T43;union Cyc_YYSTYPE*_T44;union Cyc_YYSTYPE*_T45;struct Cyc_Yyltype*_T46;struct Cyc_Yyltype*_T47;int _T48;short*_T49;int _T4A;short _T4B;int _T4C;short*_T4D;int _T4E;short _T4F;int _T50;int _T51;short*_T52;int _T53;short _T54;int _T55;int _T56;struct _fat_ptr _T57;int _T58;char*_T59;struct Cyc_Yystacktype*_T5A;struct Cyc_Yystacktype _T5B;short*_T5C;int _T5D;char*_T5E;short*_T5F;short _T60;short*_T61;int _T62;char*_T63;short*_T64;short _T65;struct _fat_ptr _T66;int _T67;int _T68;int _T69;struct _fat_ptr _T6A;char*_T6B;char*_T6C;struct Cyc_Yystacktype*_T6D;struct Cyc_Yystacktype _T6E;int _T6F;int _T70;struct Cyc_Yystacktype*_T71;union Cyc_YYSTYPE*_T72;union Cyc_YYSTYPE*_T73;struct Cyc_Yystacktype*_T74;struct Cyc_Yystacktype _T75;struct Cyc_Yystacktype*_T76;union Cyc_YYSTYPE*_T77;union Cyc_YYSTYPE*_T78;struct Cyc_Yystacktype*_T79;union Cyc_YYSTYPE*_T7A;union Cyc_YYSTYPE*_T7B;struct Cyc_List_List*_T7C;struct Cyc_Yystacktype*_T7D;union Cyc_YYSTYPE*_T7E;union Cyc_YYSTYPE*_T7F;struct Cyc_List_List*_T80;struct Cyc_List_List*_T81;struct Cyc_List_List*_T82;struct Cyc_Absyn_Decl*_T83;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T84;struct Cyc_Yystacktype*_T85;union Cyc_YYSTYPE*_T86;union Cyc_YYSTYPE*_T87;struct Cyc_Yystacktype*_T88;union Cyc_YYSTYPE*_T89;union Cyc_YYSTYPE*_T8A;struct Cyc_Yystacktype*_T8B;struct Cyc_Yystacktype _T8C;struct Cyc_Yyltype _T8D;unsigned _T8E;struct Cyc_List_List*_T8F;struct Cyc_Absyn_Decl*_T90;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T91;struct Cyc_Yystacktype*_T92;union Cyc_YYSTYPE*_T93;union Cyc_YYSTYPE*_T94;struct Cyc_Yystacktype*_T95;union Cyc_YYSTYPE*_T96;union Cyc_YYSTYPE*_T97;struct Cyc_Yystacktype*_T98;struct Cyc_Yystacktype _T99;struct Cyc_Yyltype _T9A;unsigned _T9B;struct Cyc_Yystacktype*_T9C;union Cyc_YYSTYPE*_T9D;union Cyc_YYSTYPE*_T9E;struct Cyc_List_List*_T9F;struct Cyc_Absyn_Decl*_TA0;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_TA1;struct _fat_ptr*_TA2;struct Cyc_Yystacktype*_TA3;union Cyc_YYSTYPE*_TA4;union Cyc_YYSTYPE*_TA5;struct Cyc_Yystacktype*_TA6;union Cyc_YYSTYPE*_TA7;union Cyc_YYSTYPE*_TA8;struct Cyc_Yystacktype*_TA9;struct Cyc_Yystacktype _TAA;struct Cyc_Yyltype _TAB;unsigned _TAC;struct Cyc_List_List*_TAD;struct Cyc_Absyn_Decl*_TAE;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_TAF;struct _fat_ptr*_TB0;struct Cyc_Yystacktype*_TB1;union Cyc_YYSTYPE*_TB2;union Cyc_YYSTYPE*_TB3;struct Cyc_Yystacktype*_TB4;union Cyc_YYSTYPE*_TB5;union Cyc_YYSTYPE*_TB6;struct Cyc_Yystacktype*_TB7;struct Cyc_Yystacktype _TB8;struct Cyc_Yyltype _TB9;unsigned _TBA;struct Cyc_Yystacktype*_TBB;union Cyc_YYSTYPE*_TBC;union Cyc_YYSTYPE*_TBD;struct Cyc_Yystacktype*_TBE;union Cyc_YYSTYPE*_TBF;union Cyc_YYSTYPE*_TC0;struct Cyc_List_List*_TC1;struct Cyc_Yystacktype*_TC2;union Cyc_YYSTYPE*_TC3;union Cyc_YYSTYPE*_TC4;struct Cyc_List_List*_TC5;struct Cyc_List_List*_TC6;struct Cyc_Yystacktype*_TC7;union Cyc_YYSTYPE*_TC8;union Cyc_YYSTYPE*_TC9;struct Cyc_Yystacktype*_TCA;union Cyc_YYSTYPE*_TCB;union Cyc_YYSTYPE*_TCC;struct Cyc_Yystacktype*_TCD;union Cyc_YYSTYPE*_TCE;union Cyc_YYSTYPE*_TCF;struct Cyc_Yystacktype*_TD0;union Cyc_YYSTYPE*_TD1;union Cyc_YYSTYPE*_TD2;struct Cyc_Yystacktype*_TD3;struct Cyc_Yystacktype _TD4;struct Cyc_Yyltype _TD5;unsigned _TD6;unsigned _TD7;struct _fat_ptr _TD8;struct _fat_ptr _TD9;struct Cyc_List_List*_TDA;unsigned _TDB;unsigned _TDC;int _TDD;struct Cyc_Yystacktype*_TDE;struct Cyc_Yystacktype _TDF;struct Cyc_Yyltype _TE0;unsigned _TE1;int _TE2;struct Cyc_Yystacktype*_TE3;struct Cyc_Yystacktype _TE4;struct Cyc_Yyltype _TE5;unsigned _TE6;unsigned _TE7;struct _fat_ptr _TE8;struct _fat_ptr _TE9;struct Cyc_List_List*_TEA;struct Cyc_Absyn_Decl*_TEB;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_TEC;struct Cyc_Yystacktype*_TED;union Cyc_YYSTYPE*_TEE;union Cyc_YYSTYPE*_TEF;struct _tuple10*_TF0;struct Cyc_Yystacktype*_TF1;struct Cyc_Yystacktype _TF2;struct Cyc_Yyltype _TF3;unsigned _TF4;struct Cyc_Yystacktype*_TF5;union Cyc_YYSTYPE*_TF6;union Cyc_YYSTYPE*_TF7;struct Cyc_List_List*_TF8;struct Cyc_Absyn_Decl*_TF9;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_TFA;struct Cyc_Yystacktype*_TFB;union Cyc_YYSTYPE*_TFC;union Cyc_YYSTYPE*_TFD;struct Cyc_Yystacktype*_TFE;struct Cyc_Yystacktype _TFF;struct Cyc_Yyltype _T100;unsigned _T101;struct Cyc_Yystacktype*_T102;union Cyc_YYSTYPE*_T103;union Cyc_YYSTYPE*_T104;struct Cyc_List_List*_T105;struct Cyc_Absyn_Decl*_T106;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T107;struct Cyc_Yystacktype*_T108;union Cyc_YYSTYPE*_T109;union Cyc_YYSTYPE*_T10A;struct _tuple10*_T10B;struct Cyc_Yystacktype*_T10C;struct Cyc_Yystacktype _T10D;struct Cyc_Yyltype _T10E;unsigned _T10F;struct Cyc_Yystacktype*_T110;union Cyc_YYSTYPE*_T111;union Cyc_YYSTYPE*_T112;struct Cyc_List_List*_T113;struct Cyc_Absyn_Decl*_T114;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T115;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T116;struct Cyc_Yystacktype*_T117;struct Cyc_Yystacktype _T118;struct Cyc_Yyltype _T119;unsigned _T11A;struct Cyc_Yystacktype*_T11B;union Cyc_YYSTYPE*_T11C;union Cyc_YYSTYPE*_T11D;struct Cyc_List_List*_T11E;struct Cyc_Absyn_Decl*_T11F;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T120;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T121;struct Cyc_Yystacktype*_T122;struct Cyc_Yystacktype _T123;struct Cyc_Yyltype _T124;unsigned _T125;struct Cyc_Yystacktype*_T126;union Cyc_YYSTYPE*_T127;union Cyc_YYSTYPE*_T128;struct Cyc_List_List*_T129;struct Cyc_Absyn_Decl*_T12A;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T12B;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T12C;struct Cyc_Yystacktype*_T12D;struct Cyc_Yystacktype _T12E;struct Cyc_Yyltype _T12F;unsigned _T130;struct Cyc_Yystacktype*_T131;union Cyc_YYSTYPE*_T132;union Cyc_YYSTYPE*_T133;struct Cyc_List_List*_T134;struct Cyc_Absyn_Decl*_T135;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T136;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T137;struct Cyc_Yystacktype*_T138;struct Cyc_Yystacktype _T139;struct Cyc_Yyltype _T13A;unsigned _T13B;struct Cyc_Yystacktype*_T13C;union Cyc_YYSTYPE*_T13D;union Cyc_YYSTYPE*_T13E;struct Cyc_Yystacktype*_T13F;union Cyc_YYSTYPE*_T140;union Cyc_YYSTYPE*_T141;struct _fat_ptr _T142;struct _fat_ptr _T143;int _T144;struct Cyc_Yystacktype*_T145;union Cyc_YYSTYPE*_T146;union Cyc_YYSTYPE*_T147;struct _fat_ptr _T148;struct _fat_ptr _T149;int _T14A;struct Cyc_Yystacktype*_T14B;struct Cyc_Yystacktype _T14C;struct Cyc_Yyltype _T14D;unsigned _T14E;unsigned _T14F;struct _fat_ptr _T150;struct _fat_ptr _T151;struct Cyc_Yystacktype*_T152;struct Cyc_Yystacktype _T153;struct Cyc_List_List*_T154;struct Cyc_Yystacktype*_T155;union Cyc_YYSTYPE*_T156;union Cyc_YYSTYPE*_T157;struct Cyc_List_List*_T158;struct Cyc_Yystacktype*_T159;union Cyc_YYSTYPE*_T15A;union Cyc_YYSTYPE*_T15B;struct Cyc_List_List*_T15C;struct Cyc_Yystacktype*_T15D;union Cyc_YYSTYPE*_T15E;union Cyc_YYSTYPE*_T15F;struct Cyc_Yystacktype*_T160;union Cyc_YYSTYPE*_T161;union Cyc_YYSTYPE*_T162;struct _tuple30*_T163;struct Cyc_Yystacktype*_T164;struct Cyc_Yystacktype _T165;struct _tuple30*_T166;struct Cyc_Yystacktype*_T167;union Cyc_YYSTYPE*_T168;union Cyc_YYSTYPE*_T169;struct _tuple30*_T16A;struct _tuple30*_T16B;struct Cyc_Yystacktype*_T16C;struct Cyc_Yystacktype _T16D;struct Cyc_Yyltype _T16E;unsigned _T16F;struct Cyc_List_List*_T170;struct _tuple33*_T171;struct Cyc_Yystacktype*_T172;struct Cyc_Yystacktype _T173;struct Cyc_Yyltype _T174;unsigned _T175;struct Cyc_Yystacktype*_T176;union Cyc_YYSTYPE*_T177;union Cyc_YYSTYPE*_T178;struct Cyc_List_List*_T179;struct _tuple33*_T17A;struct Cyc_Yystacktype*_T17B;struct Cyc_Yystacktype _T17C;struct Cyc_Yyltype _T17D;unsigned _T17E;struct Cyc_Yystacktype*_T17F;union Cyc_YYSTYPE*_T180;union Cyc_YYSTYPE*_T181;struct Cyc_Yystacktype*_T182;union Cyc_YYSTYPE*_T183;union Cyc_YYSTYPE*_T184;struct Cyc_Yystacktype*_T185;struct Cyc_Yystacktype _T186;struct Cyc_List_List*_T187;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T188;struct Cyc_Yystacktype*_T189;union Cyc_YYSTYPE*_T18A;union Cyc_YYSTYPE*_T18B;void*_T18C;struct Cyc_Yystacktype*_T18D;struct Cyc_Yystacktype _T18E;struct Cyc_Yyltype _T18F;unsigned _T190;unsigned _T191;struct Cyc_Yystacktype*_T192;struct Cyc_Yystacktype _T193;struct _RegionHandle*_T194;struct Cyc_Yystacktype*_T195;union Cyc_YYSTYPE*_T196;union Cyc_YYSTYPE*_T197;struct Cyc_Parse_Declarator _T198;struct Cyc_Yystacktype*_T199;union Cyc_YYSTYPE*_T19A;union Cyc_YYSTYPE*_T19B;struct Cyc_Absyn_Stmt*_T19C;struct Cyc_Yystacktype*_T19D;struct Cyc_Yystacktype _T19E;struct Cyc_Yyltype _T19F;unsigned _T1A0;unsigned _T1A1;struct Cyc_Absyn_Fndecl*_T1A2;struct Cyc_Yystacktype*_T1A3;union Cyc_YYSTYPE*_T1A4;union Cyc_YYSTYPE*_T1A5;struct _RegionHandle*_T1A6;struct Cyc_Parse_Declaration_spec*_T1A7;struct Cyc_Parse_Declaration_spec*_T1A8;struct Cyc_Yystacktype*_T1A9;union Cyc_YYSTYPE*_T1AA;union Cyc_YYSTYPE*_T1AB;struct Cyc_Parse_Declarator _T1AC;struct Cyc_Yystacktype*_T1AD;union Cyc_YYSTYPE*_T1AE;union Cyc_YYSTYPE*_T1AF;struct Cyc_Absyn_Stmt*_T1B0;struct Cyc_Yystacktype*_T1B1;struct Cyc_Yystacktype _T1B2;struct Cyc_Yyltype _T1B3;unsigned _T1B4;unsigned _T1B5;struct Cyc_Absyn_Fndecl*_T1B6;struct _RegionHandle*_T1B7;struct Cyc_Yystacktype*_T1B8;union Cyc_YYSTYPE*_T1B9;union Cyc_YYSTYPE*_T1BA;struct Cyc_Parse_Declarator _T1BB;struct Cyc_Yystacktype*_T1BC;union Cyc_YYSTYPE*_T1BD;union Cyc_YYSTYPE*_T1BE;struct Cyc_List_List*_T1BF;struct Cyc_Yystacktype*_T1C0;union Cyc_YYSTYPE*_T1C1;union Cyc_YYSTYPE*_T1C2;struct Cyc_Absyn_Stmt*_T1C3;struct Cyc_Yystacktype*_T1C4;struct Cyc_Yystacktype _T1C5;struct Cyc_Yyltype _T1C6;unsigned _T1C7;unsigned _T1C8;struct Cyc_Absyn_Fndecl*_T1C9;struct Cyc_Yystacktype*_T1CA;union Cyc_YYSTYPE*_T1CB;union Cyc_YYSTYPE*_T1CC;struct _RegionHandle*_T1CD;struct Cyc_Parse_Declaration_spec*_T1CE;struct Cyc_Parse_Declaration_spec*_T1CF;struct Cyc_Yystacktype*_T1D0;union Cyc_YYSTYPE*_T1D1;union Cyc_YYSTYPE*_T1D2;struct Cyc_Parse_Declarator _T1D3;struct Cyc_Yystacktype*_T1D4;union Cyc_YYSTYPE*_T1D5;union Cyc_YYSTYPE*_T1D6;struct Cyc_List_List*_T1D7;struct Cyc_Yystacktype*_T1D8;union Cyc_YYSTYPE*_T1D9;union Cyc_YYSTYPE*_T1DA;struct Cyc_Absyn_Stmt*_T1DB;struct Cyc_Yystacktype*_T1DC;struct Cyc_Yystacktype _T1DD;struct Cyc_Yyltype _T1DE;unsigned _T1DF;unsigned _T1E0;struct Cyc_Absyn_Fndecl*_T1E1;struct Cyc_Yystacktype*_T1E2;union Cyc_YYSTYPE*_T1E3;union Cyc_YYSTYPE*_T1E4;struct _RegionHandle*_T1E5;struct Cyc_Parse_Declaration_spec*_T1E6;struct Cyc_Parse_Declaration_spec*_T1E7;struct Cyc_Yystacktype*_T1E8;union Cyc_YYSTYPE*_T1E9;union Cyc_YYSTYPE*_T1EA;struct Cyc_Parse_Declarator _T1EB;struct Cyc_Yystacktype*_T1EC;union Cyc_YYSTYPE*_T1ED;union Cyc_YYSTYPE*_T1EE;struct Cyc_Absyn_Stmt*_T1EF;struct Cyc_Yystacktype*_T1F0;struct Cyc_Yystacktype _T1F1;struct Cyc_Yyltype _T1F2;unsigned _T1F3;unsigned _T1F4;struct Cyc_Absyn_Fndecl*_T1F5;struct Cyc_Yystacktype*_T1F6;union Cyc_YYSTYPE*_T1F7;union Cyc_YYSTYPE*_T1F8;struct _RegionHandle*_T1F9;struct Cyc_Parse_Declaration_spec*_T1FA;struct Cyc_Parse_Declaration_spec*_T1FB;struct Cyc_Yystacktype*_T1FC;union Cyc_YYSTYPE*_T1FD;union Cyc_YYSTYPE*_T1FE;struct Cyc_Parse_Declarator _T1FF;struct Cyc_Yystacktype*_T200;union Cyc_YYSTYPE*_T201;union Cyc_YYSTYPE*_T202;struct Cyc_List_List*_T203;struct Cyc_Yystacktype*_T204;union Cyc_YYSTYPE*_T205;union Cyc_YYSTYPE*_T206;struct Cyc_Absyn_Stmt*_T207;struct Cyc_Yystacktype*_T208;struct Cyc_Yystacktype _T209;struct Cyc_Yyltype _T20A;unsigned _T20B;unsigned _T20C;struct Cyc_Absyn_Fndecl*_T20D;struct Cyc_Yystacktype*_T20E;union Cyc_YYSTYPE*_T20F;union Cyc_YYSTYPE*_T210;struct _tuple0*_T211;struct Cyc_Yystacktype*_T212;struct Cyc_Yystacktype _T213;struct _fat_ptr*_T214;struct Cyc_Yystacktype*_T215;union Cyc_YYSTYPE*_T216;union Cyc_YYSTYPE*_T217;struct Cyc_Yystacktype*_T218;struct Cyc_Yystacktype _T219;struct Cyc_Yystacktype*_T21A;union Cyc_YYSTYPE*_T21B;union Cyc_YYSTYPE*_T21C;struct Cyc_Parse_Declaration_spec _T21D;struct Cyc_Yystacktype*_T21E;struct Cyc_Yystacktype _T21F;struct Cyc_Yyltype _T220;unsigned _T221;unsigned _T222;struct Cyc_Yystacktype*_T223;struct Cyc_Yystacktype _T224;struct Cyc_Yyltype _T225;unsigned _T226;unsigned _T227;struct Cyc_List_List*_T228;struct Cyc_Yystacktype*_T229;union Cyc_YYSTYPE*_T22A;union Cyc_YYSTYPE*_T22B;struct Cyc_Parse_Declaration_spec _T22C;struct _tuple11*(*_T22D)(struct _tuple11*);struct Cyc_Yystacktype*_T22E;union Cyc_YYSTYPE*_T22F;union Cyc_YYSTYPE*_T230;struct _tuple11*_T231;struct _tuple11*_T232;struct Cyc_Yystacktype*_T233;struct Cyc_Yystacktype _T234;struct Cyc_Yyltype _T235;unsigned _T236;unsigned _T237;struct Cyc_Yystacktype*_T238;struct Cyc_Yystacktype _T239;struct Cyc_Yyltype _T23A;unsigned _T23B;unsigned _T23C;struct Cyc_List_List*_T23D;struct Cyc_List_List*_T23E;struct Cyc_Yystacktype*_T23F;union Cyc_YYSTYPE*_T240;union Cyc_YYSTYPE*_T241;struct Cyc_Absyn_Pat*_T242;struct Cyc_Yystacktype*_T243;union Cyc_YYSTYPE*_T244;union Cyc_YYSTYPE*_T245;struct Cyc_Absyn_Exp*_T246;struct Cyc_Yystacktype*_T247;struct Cyc_Yystacktype _T248;struct Cyc_Yyltype _T249;unsigned _T24A;unsigned _T24B;struct Cyc_Yystacktype*_T24C;union Cyc_YYSTYPE*_T24D;union Cyc_YYSTYPE*_T24E;struct _tuple0*_T24F;struct _tuple0*_T250;struct Cyc_List_List*_T251;void*_T252;struct Cyc_List_List*_T253;struct _tuple0*_T254;void*_T255;struct Cyc_List_List*_T256;struct Cyc_List_List*_T257;struct Cyc_List_List*_T258;struct Cyc_Yystacktype*_T259;struct Cyc_Yystacktype _T25A;struct Cyc_Yyltype _T25B;unsigned _T25C;unsigned _T25D;struct Cyc_Yystacktype*_T25E;union Cyc_YYSTYPE*_T25F;union Cyc_YYSTYPE*_T260;struct _fat_ptr _T261;struct Cyc_Yystacktype*_T262;struct Cyc_Yystacktype _T263;struct Cyc_Yyltype _T264;unsigned _T265;unsigned _T266;struct Cyc_Absyn_Tvar*_T267;struct _fat_ptr*_T268;struct Cyc_Yystacktype*_T269;union Cyc_YYSTYPE*_T26A;union Cyc_YYSTYPE*_T26B;struct Cyc_Absyn_Tvar*_T26C;struct Cyc_Absyn_Tvar*_T26D;struct Cyc_Absyn_Kind*_T26E;struct Cyc_Absyn_Kind*_T26F;struct Cyc_Absyn_Tvar*_T270;struct Cyc_Yystacktype*_T271;struct Cyc_Yystacktype _T272;struct Cyc_Yyltype _T273;unsigned _T274;unsigned _T275;struct _tuple0*_T276;struct _fat_ptr*_T277;struct Cyc_Yystacktype*_T278;union Cyc_YYSTYPE*_T279;union Cyc_YYSTYPE*_T27A;void*_T27B;struct Cyc_List_List*_T27C;struct Cyc_Absyn_Tvar*_T27D;struct Cyc_Absyn_Vardecl*_T27E;struct Cyc_Yystacktype*_T27F;struct Cyc_Yystacktype _T280;struct Cyc_Yyltype _T281;unsigned _T282;unsigned _T283;struct _fat_ptr _T284;struct Cyc_String_pa_PrintArg_struct _T285;struct Cyc_Yystacktype*_T286;union Cyc_YYSTYPE*_T287;union Cyc_YYSTYPE*_T288;struct _fat_ptr _T289;struct _fat_ptr _T28A;struct Cyc_Yystacktype*_T28B;union Cyc_YYSTYPE*_T28C;union Cyc_YYSTYPE*_T28D;struct _fat_ptr _T28E;struct Cyc_Yystacktype*_T28F;struct Cyc_Yystacktype _T290;struct Cyc_Yyltype _T291;unsigned _T292;unsigned _T293;struct Cyc_Absyn_Tvar*_T294;struct _fat_ptr*_T295;struct Cyc_Absyn_Tvar*_T296;struct Cyc_Absyn_Tvar*_T297;struct Cyc_Absyn_Kind*_T298;struct Cyc_Absyn_Kind*_T299;struct Cyc_Absyn_Tvar*_T29A;struct Cyc_Yystacktype*_T29B;struct Cyc_Yystacktype _T29C;struct Cyc_Yyltype _T29D;unsigned _T29E;unsigned _T29F;struct _tuple0*_T2A0;struct _fat_ptr*_T2A1;struct Cyc_Yystacktype*_T2A2;union Cyc_YYSTYPE*_T2A3;union Cyc_YYSTYPE*_T2A4;void*_T2A5;struct Cyc_List_List*_T2A6;struct Cyc_Absyn_Tvar*_T2A7;struct Cyc_Absyn_Vardecl*_T2A8;struct Cyc_Yystacktype*_T2A9;union Cyc_YYSTYPE*_T2AA;union Cyc_YYSTYPE*_T2AB;struct Cyc_Absyn_Exp*_T2AC;struct Cyc_Yystacktype*_T2AD;struct Cyc_Yystacktype _T2AE;struct Cyc_Yyltype _T2AF;unsigned _T2B0;unsigned _T2B1;struct Cyc_Yystacktype*_T2B2;union Cyc_YYSTYPE*_T2B3;union Cyc_YYSTYPE*_T2B4;struct _fat_ptr _T2B5;struct _fat_ptr _T2B6;int _T2B7;struct Cyc_Yystacktype*_T2B8;struct Cyc_Yystacktype _T2B9;struct Cyc_Yyltype _T2BA;unsigned _T2BB;unsigned _T2BC;struct _fat_ptr _T2BD;struct _fat_ptr _T2BE;struct Cyc_Yystacktype*_T2BF;union Cyc_YYSTYPE*_T2C0;union Cyc_YYSTYPE*_T2C1;struct Cyc_Absyn_Exp*_T2C2;struct Cyc_Yystacktype*_T2C3;struct Cyc_Yystacktype _T2C4;struct Cyc_Yystacktype*_T2C5;union Cyc_YYSTYPE*_T2C6;union Cyc_YYSTYPE*_T2C7;struct Cyc_List_List*_T2C8;struct Cyc_Yystacktype*_T2C9;union Cyc_YYSTYPE*_T2CA;union Cyc_YYSTYPE*_T2CB;struct Cyc_List_List*_T2CC;struct Cyc_List_List*_T2CD;struct Cyc_Parse_Declaration_spec _T2CE;struct Cyc_Yystacktype*_T2CF;union Cyc_YYSTYPE*_T2D0;union Cyc_YYSTYPE*_T2D1;struct Cyc_Yystacktype*_T2D2;struct Cyc_Yystacktype _T2D3;struct Cyc_Yyltype _T2D4;unsigned _T2D5;unsigned _T2D6;struct Cyc_Yystacktype*_T2D7;union Cyc_YYSTYPE*_T2D8;union Cyc_YYSTYPE*_T2D9;struct Cyc_Parse_Declaration_spec _T2DA;enum Cyc_Parse_Storage_class _T2DB;int _T2DC;struct Cyc_Yystacktype*_T2DD;struct Cyc_Yystacktype _T2DE;struct Cyc_Yyltype _T2DF;unsigned _T2E0;unsigned _T2E1;struct _fat_ptr _T2E2;struct _fat_ptr _T2E3;struct Cyc_Parse_Declaration_spec _T2E4;struct Cyc_Yystacktype*_T2E5;union Cyc_YYSTYPE*_T2E6;union Cyc_YYSTYPE*_T2E7;struct Cyc_Parse_Declaration_spec _T2E8;struct Cyc_Parse_Declaration_spec _T2E9;struct Cyc_Parse_Declaration_spec _T2EA;struct Cyc_Parse_Declaration_spec _T2EB;struct Cyc_Yystacktype*_T2EC;struct Cyc_Yystacktype _T2ED;struct Cyc_Yyltype _T2EE;unsigned _T2EF;unsigned _T2F0;struct _fat_ptr _T2F1;struct _fat_ptr _T2F2;struct Cyc_Yystacktype*_T2F3;struct Cyc_Yystacktype _T2F4;struct Cyc_Parse_Declaration_spec _T2F5;struct Cyc_Yystacktype*_T2F6;struct Cyc_Yystacktype _T2F7;struct Cyc_Yyltype _T2F8;unsigned _T2F9;unsigned _T2FA;struct Cyc_Yystacktype*_T2FB;union Cyc_YYSTYPE*_T2FC;union Cyc_YYSTYPE*_T2FD;struct Cyc_Yystacktype*_T2FE;union Cyc_YYSTYPE*_T2FF;union Cyc_YYSTYPE*_T300;struct Cyc_Parse_Declaration_spec _T301;struct Cyc_Parse_Declaration_spec _T302;struct Cyc_Parse_Declaration_spec _T303;struct Cyc_Yystacktype*_T304;struct Cyc_Yystacktype _T305;struct Cyc_Yyltype _T306;unsigned _T307;unsigned _T308;struct Cyc_Parse_Declaration_spec _T309;struct Cyc_Parse_Type_specifier _T30A;struct Cyc_Yystacktype*_T30B;union Cyc_YYSTYPE*_T30C;union Cyc_YYSTYPE*_T30D;struct Cyc_Parse_Type_specifier _T30E;struct Cyc_Parse_Declaration_spec _T30F;struct Cyc_Parse_Declaration_spec _T310;struct Cyc_Parse_Declaration_spec _T311;struct Cyc_Yystacktype*_T312;union Cyc_YYSTYPE*_T313;union Cyc_YYSTYPE*_T314;struct Cyc_Yystacktype*_T315;union Cyc_YYSTYPE*_T316;union Cyc_YYSTYPE*_T317;struct Cyc_Parse_Declaration_spec _T318;struct Cyc_Parse_Declaration_spec _T319;struct Cyc_Yystacktype*_T31A;union Cyc_YYSTYPE*_T31B;union Cyc_YYSTYPE*_T31C;struct Cyc_Absyn_Tqual _T31D;struct Cyc_Parse_Declaration_spec _T31E;struct Cyc_Absyn_Tqual _T31F;struct Cyc_Parse_Declaration_spec _T320;struct Cyc_Parse_Declaration_spec _T321;struct Cyc_Parse_Declaration_spec _T322;struct Cyc_Parse_Declaration_spec _T323;struct Cyc_Yystacktype*_T324;struct Cyc_Yystacktype _T325;struct Cyc_Yyltype _T326;unsigned _T327;unsigned _T328;struct Cyc_Yystacktype*_T329;union Cyc_YYSTYPE*_T32A;union Cyc_YYSTYPE*_T32B;struct Cyc_Parse_Declaration_spec _T32C;struct Cyc_Parse_Declaration_spec _T32D;struct Cyc_Parse_Declaration_spec _T32E;struct Cyc_Parse_Declaration_spec _T32F;struct Cyc_Parse_Declaration_spec _T330;struct Cyc_Parse_Declaration_spec _T331;struct Cyc_Yystacktype*_T332;struct Cyc_Yystacktype _T333;struct Cyc_Yyltype _T334;unsigned _T335;unsigned _T336;struct Cyc_Yystacktype*_T337;union Cyc_YYSTYPE*_T338;union Cyc_YYSTYPE*_T339;struct Cyc_Yystacktype*_T33A;union Cyc_YYSTYPE*_T33B;union Cyc_YYSTYPE*_T33C;struct Cyc_Parse_Declaration_spec _T33D;struct Cyc_Parse_Declaration_spec _T33E;struct Cyc_Parse_Declaration_spec _T33F;struct Cyc_Parse_Declaration_spec _T340;struct Cyc_Parse_Declaration_spec _T341;struct Cyc_Yystacktype*_T342;union Cyc_YYSTYPE*_T343;union Cyc_YYSTYPE*_T344;struct Cyc_List_List*_T345;struct Cyc_Parse_Declaration_spec _T346;struct Cyc_List_List*_T347;struct Cyc_Yystacktype*_T348;union Cyc_YYSTYPE*_T349;union Cyc_YYSTYPE*_T34A;struct _fat_ptr _T34B;struct _fat_ptr _T34C;int _T34D;struct Cyc_Yystacktype*_T34E;struct Cyc_Yystacktype _T34F;struct Cyc_Yyltype _T350;unsigned _T351;unsigned _T352;struct _fat_ptr _T353;struct _fat_ptr _T354;struct Cyc_Yystacktype*_T355;struct Cyc_Yystacktype _T356;struct Cyc_Yystacktype*_T357;struct Cyc_Yystacktype _T358;struct Cyc_List_List*_T359;struct Cyc_Yystacktype*_T35A;union Cyc_YYSTYPE*_T35B;union Cyc_YYSTYPE*_T35C;struct Cyc_List_List*_T35D;struct Cyc_Yystacktype*_T35E;union Cyc_YYSTYPE*_T35F;union Cyc_YYSTYPE*_T360;struct Cyc_Yystacktype*_T361;union Cyc_YYSTYPE*_T362;union Cyc_YYSTYPE*_T363;struct Cyc_Yystacktype*_T364;struct Cyc_Yystacktype _T365;struct Cyc_Yyltype _T366;unsigned _T367;unsigned _T368;struct Cyc_Yystacktype*_T369;union Cyc_YYSTYPE*_T36A;union Cyc_YYSTYPE*_T36B;struct _fat_ptr _T36C;void*_T36D;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T36E;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T36F;void*_T370;struct Cyc_Yystacktype*_T371;struct Cyc_Yystacktype _T372;struct Cyc_Yyltype _T373;unsigned _T374;unsigned _T375;struct Cyc_Yystacktype*_T376;union Cyc_YYSTYPE*_T377;union Cyc_YYSTYPE*_T378;struct _fat_ptr _T379;struct Cyc_Yystacktype*_T37A;struct Cyc_Yystacktype _T37B;struct Cyc_Yyltype _T37C;unsigned _T37D;unsigned _T37E;struct Cyc_Yystacktype*_T37F;union Cyc_YYSTYPE*_T380;union Cyc_YYSTYPE*_T381;struct Cyc_Absyn_Exp*_T382;void*_T383;struct Cyc_Yystacktype*_T384;struct Cyc_Yystacktype _T385;struct Cyc_Yyltype _T386;unsigned _T387;unsigned _T388;struct Cyc_Yystacktype*_T389;struct Cyc_Yystacktype _T38A;struct Cyc_Yyltype _T38B;unsigned _T38C;unsigned _T38D;struct Cyc_Yystacktype*_T38E;union Cyc_YYSTYPE*_T38F;union Cyc_YYSTYPE*_T390;struct _fat_ptr _T391;struct Cyc_Yystacktype*_T392;union Cyc_YYSTYPE*_T393;union Cyc_YYSTYPE*_T394;struct _fat_ptr _T395;struct Cyc_Yystacktype*_T396;struct Cyc_Yystacktype _T397;struct Cyc_Yyltype _T398;unsigned _T399;unsigned _T39A;struct Cyc_Yystacktype*_T39B;union Cyc_YYSTYPE*_T39C;union Cyc_YYSTYPE*_T39D;union Cyc_Absyn_Cnst _T39E;unsigned _T39F;struct Cyc_Yystacktype*_T3A0;struct Cyc_Yystacktype _T3A1;struct Cyc_Yyltype _T3A2;unsigned _T3A3;unsigned _T3A4;struct Cyc_Yystacktype*_T3A5;union Cyc_YYSTYPE*_T3A6;union Cyc_YYSTYPE*_T3A7;union Cyc_Absyn_Cnst _T3A8;unsigned _T3A9;void*_T3AA;struct Cyc_Yystacktype*_T3AB;struct Cyc_Yystacktype _T3AC;struct Cyc_Yystacktype*_T3AD;union Cyc_YYSTYPE*_T3AE;union Cyc_YYSTYPE*_T3AF;struct _tuple0*_T3B0;struct Cyc_Yystacktype*_T3B1;union Cyc_YYSTYPE*_T3B2;union Cyc_YYSTYPE*_T3B3;struct Cyc_List_List*_T3B4;void*_T3B5;struct Cyc_Yystacktype*_T3B6;struct Cyc_Yystacktype _T3B7;struct Cyc_Yyltype _T3B8;unsigned _T3B9;unsigned _T3BA;struct Cyc_Parse_Type_specifier _T3BB;void*_T3BC;struct Cyc_Yystacktype*_T3BD;struct Cyc_Yystacktype _T3BE;struct Cyc_Yyltype _T3BF;unsigned _T3C0;unsigned _T3C1;struct Cyc_Parse_Type_specifier _T3C2;void*_T3C3;struct Cyc_Yystacktype*_T3C4;struct Cyc_Yystacktype _T3C5;struct Cyc_Yyltype _T3C6;unsigned _T3C7;unsigned _T3C8;struct Cyc_Parse_Type_specifier _T3C9;struct Cyc_Yystacktype*_T3CA;struct Cyc_Yystacktype _T3CB;struct Cyc_Yyltype _T3CC;unsigned _T3CD;unsigned _T3CE;struct Cyc_Parse_Type_specifier _T3CF;void*_T3D0;struct Cyc_Yystacktype*_T3D1;struct Cyc_Yystacktype _T3D2;struct Cyc_Yyltype _T3D3;unsigned _T3D4;unsigned _T3D5;struct Cyc_Parse_Type_specifier _T3D6;struct Cyc_Yystacktype*_T3D7;struct Cyc_Yystacktype _T3D8;struct Cyc_Yyltype _T3D9;unsigned _T3DA;unsigned _T3DB;struct Cyc_Parse_Type_specifier _T3DC;void*_T3DD;struct Cyc_Yystacktype*_T3DE;struct Cyc_Yystacktype _T3DF;struct Cyc_Yyltype _T3E0;unsigned _T3E1;unsigned _T3E2;struct Cyc_Parse_Type_specifier _T3E3;void*_T3E4;struct Cyc_Yystacktype*_T3E5;struct Cyc_Yystacktype _T3E6;struct Cyc_Yyltype _T3E7;unsigned _T3E8;unsigned _T3E9;struct Cyc_Parse_Type_specifier _T3EA;struct Cyc_Yystacktype*_T3EB;struct Cyc_Yystacktype _T3EC;struct Cyc_Yyltype _T3ED;unsigned _T3EE;unsigned _T3EF;struct Cyc_Parse_Type_specifier _T3F0;struct Cyc_Yystacktype*_T3F1;struct Cyc_Yystacktype _T3F2;struct Cyc_Yyltype _T3F3;unsigned _T3F4;unsigned _T3F5;struct Cyc_Parse_Type_specifier _T3F6;struct Cyc_Yystacktype*_T3F7;struct Cyc_Yystacktype _T3F8;struct Cyc_Yyltype _T3F9;unsigned _T3FA;unsigned _T3FB;struct Cyc_Parse_Type_specifier _T3FC;struct Cyc_Yystacktype*_T3FD;struct Cyc_Yystacktype _T3FE;struct Cyc_Yystacktype*_T3FF;struct Cyc_Yystacktype _T400;struct Cyc_Yystacktype*_T401;union Cyc_YYSTYPE*_T402;union Cyc_YYSTYPE*_T403;struct Cyc_Absyn_Exp*_T404;void*_T405;struct Cyc_Yystacktype*_T406;struct Cyc_Yystacktype _T407;struct Cyc_Yyltype _T408;unsigned _T409;unsigned _T40A;struct Cyc_Parse_Type_specifier _T40B;struct _fat_ptr _T40C;struct Cyc_Absyn_Kind*_T40D;struct Cyc_Absyn_Kind*_T40E;void*_T40F;struct Cyc_Yystacktype*_T410;struct Cyc_Yystacktype _T411;struct Cyc_Yyltype _T412;unsigned _T413;unsigned _T414;struct Cyc_Parse_Type_specifier _T415;struct Cyc_Yystacktype*_T416;struct Cyc_Yystacktype _T417;struct Cyc_Yystacktype*_T418;union Cyc_YYSTYPE*_T419;union Cyc_YYSTYPE*_T41A;void*_T41B;struct Cyc_Yystacktype*_T41C;struct Cyc_Yystacktype _T41D;struct Cyc_Yyltype _T41E;unsigned _T41F;unsigned _T420;struct Cyc_Parse_Type_specifier _T421;void*_T422;struct Cyc_Yystacktype*_T423;struct Cyc_Yystacktype _T424;struct Cyc_Yyltype _T425;unsigned _T426;unsigned _T427;struct Cyc_Parse_Type_specifier _T428;struct Cyc_Yystacktype*_T429;union Cyc_YYSTYPE*_T42A;union Cyc_YYSTYPE*_T42B;struct Cyc_Absyn_Kind*_T42C;struct Cyc_Core_Opt*_T42D;void*_T42E;struct Cyc_Yystacktype*_T42F;struct Cyc_Yystacktype _T430;struct Cyc_Yyltype _T431;unsigned _T432;unsigned _T433;struct Cyc_Parse_Type_specifier _T434;struct Cyc_List_List*(*_T435)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T436)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T437)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T438;struct Cyc_Yystacktype _T439;struct Cyc_Yyltype _T43A;unsigned _T43B;unsigned _T43C;struct Cyc_Yystacktype*_T43D;union Cyc_YYSTYPE*_T43E;union Cyc_YYSTYPE*_T43F;struct Cyc_List_List*_T440;struct Cyc_List_List*_T441;struct Cyc_List_List*_T442;void*_T443;struct Cyc_Yystacktype*_T444;struct Cyc_Yystacktype _T445;struct Cyc_Yyltype _T446;unsigned _T447;unsigned _T448;struct Cyc_Parse_Type_specifier _T449;struct Cyc_Yystacktype*_T44A;union Cyc_YYSTYPE*_T44B;union Cyc_YYSTYPE*_T44C;void*_T44D;void*_T44E;struct Cyc_Yystacktype*_T44F;struct Cyc_Yystacktype _T450;struct Cyc_Yyltype _T451;unsigned _T452;unsigned _T453;struct Cyc_Parse_Type_specifier _T454;struct Cyc_Core_Opt*_T455;struct Cyc_Core_Opt*_T456;void*_T457;void*_T458;struct Cyc_Yystacktype*_T459;struct Cyc_Yystacktype _T45A;struct Cyc_Yyltype _T45B;unsigned _T45C;unsigned _T45D;struct Cyc_Parse_Type_specifier _T45E;struct Cyc_Yystacktype*_T45F;union Cyc_YYSTYPE*_T460;union Cyc_YYSTYPE*_T461;void*_T462;void*_T463;struct Cyc_Yystacktype*_T464;struct Cyc_Yystacktype _T465;struct Cyc_Yyltype _T466;unsigned _T467;unsigned _T468;struct Cyc_Parse_Type_specifier _T469;struct Cyc_Yystacktype*_T46A;union Cyc_YYSTYPE*_T46B;union Cyc_YYSTYPE*_T46C;void*_T46D;void*_T46E;struct Cyc_Yystacktype*_T46F;struct Cyc_Yystacktype _T470;struct Cyc_Yyltype _T471;unsigned _T472;unsigned _T473;struct Cyc_Parse_Type_specifier _T474;struct Cyc_Core_Opt*_T475;struct Cyc_Core_Opt*_T476;void*_T477;void*_T478;struct Cyc_Yystacktype*_T479;struct Cyc_Yystacktype _T47A;struct Cyc_Yyltype _T47B;unsigned _T47C;unsigned _T47D;struct Cyc_Parse_Type_specifier _T47E;struct Cyc_Yystacktype*_T47F;union Cyc_YYSTYPE*_T480;union Cyc_YYSTYPE*_T481;struct Cyc_Absyn_Exp*_T482;void*_T483;struct Cyc_Yystacktype*_T484;struct Cyc_Yystacktype _T485;struct Cyc_Yyltype _T486;unsigned _T487;unsigned _T488;struct Cyc_Parse_Type_specifier _T489;struct Cyc_Yystacktype*_T48A;union Cyc_YYSTYPE*_T48B;union Cyc_YYSTYPE*_T48C;struct _fat_ptr _T48D;struct Cyc_Yystacktype*_T48E;struct Cyc_Yystacktype _T48F;struct Cyc_Yyltype _T490;unsigned _T491;unsigned _T492;struct Cyc_Absyn_Kind*_T493;unsigned _T494;int _T495;struct Cyc_Yystacktype*_T496;struct Cyc_Yystacktype _T497;struct Cyc_Yyltype _T498;unsigned _T499;struct Cyc_Absyn_Tqual _T49A;struct Cyc_Absyn_Tqual _T49B;struct Cyc_Absyn_Tqual _T49C;struct Cyc_Absyn_TypeDecl*_T49D;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T49E;struct Cyc_Absyn_Enumdecl*_T49F;struct Cyc_Yystacktype*_T4A0;union Cyc_YYSTYPE*_T4A1;union Cyc_YYSTYPE*_T4A2;struct Cyc_Core_Opt*_T4A3;struct Cyc_Yystacktype*_T4A4;union Cyc_YYSTYPE*_T4A5;union Cyc_YYSTYPE*_T4A6;struct Cyc_Absyn_TypeDecl*_T4A7;struct Cyc_Yystacktype*_T4A8;struct Cyc_Yystacktype _T4A9;struct Cyc_Yyltype _T4AA;unsigned _T4AB;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T4AC;void*_T4AD;struct Cyc_Yystacktype*_T4AE;struct Cyc_Yystacktype _T4AF;struct Cyc_Yyltype _T4B0;unsigned _T4B1;unsigned _T4B2;struct Cyc_Parse_Type_specifier _T4B3;struct Cyc_Yystacktype*_T4B4;union Cyc_YYSTYPE*_T4B5;union Cyc_YYSTYPE*_T4B6;struct _tuple0*_T4B7;void*_T4B8;struct Cyc_Yystacktype*_T4B9;struct Cyc_Yystacktype _T4BA;struct Cyc_Yyltype _T4BB;unsigned _T4BC;unsigned _T4BD;struct Cyc_Parse_Type_specifier _T4BE;struct Cyc_Yystacktype*_T4BF;union Cyc_YYSTYPE*_T4C0;union Cyc_YYSTYPE*_T4C1;struct Cyc_List_List*_T4C2;void*_T4C3;struct Cyc_Yystacktype*_T4C4;struct Cyc_Yystacktype _T4C5;struct Cyc_Yyltype _T4C6;unsigned _T4C7;unsigned _T4C8;struct Cyc_Parse_Type_specifier _T4C9;struct Cyc_Absyn_Enumfield*_T4CA;struct Cyc_Yystacktype*_T4CB;union Cyc_YYSTYPE*_T4CC;union Cyc_YYSTYPE*_T4CD;struct Cyc_Yystacktype*_T4CE;struct Cyc_Yystacktype _T4CF;struct Cyc_Yyltype _T4D0;unsigned _T4D1;struct Cyc_Absyn_Enumfield*_T4D2;struct Cyc_Yystacktype*_T4D3;union Cyc_YYSTYPE*_T4D4;union Cyc_YYSTYPE*_T4D5;struct Cyc_Yystacktype*_T4D6;union Cyc_YYSTYPE*_T4D7;union Cyc_YYSTYPE*_T4D8;struct Cyc_Yystacktype*_T4D9;struct Cyc_Yystacktype _T4DA;struct Cyc_Yyltype _T4DB;unsigned _T4DC;struct Cyc_List_List*_T4DD;struct Cyc_Yystacktype*_T4DE;union Cyc_YYSTYPE*_T4DF;union Cyc_YYSTYPE*_T4E0;struct Cyc_List_List*_T4E1;struct Cyc_Yystacktype*_T4E2;union Cyc_YYSTYPE*_T4E3;union Cyc_YYSTYPE*_T4E4;struct Cyc_List_List*_T4E5;struct Cyc_Yystacktype*_T4E6;union Cyc_YYSTYPE*_T4E7;union Cyc_YYSTYPE*_T4E8;struct Cyc_Yystacktype*_T4E9;union Cyc_YYSTYPE*_T4EA;union Cyc_YYSTYPE*_T4EB;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T4EC;struct Cyc_Yystacktype*_T4ED;union Cyc_YYSTYPE*_T4EE;union Cyc_YYSTYPE*_T4EF;struct Cyc_Yystacktype*_T4F0;union Cyc_YYSTYPE*_T4F1;union Cyc_YYSTYPE*_T4F2;void*_T4F3;struct Cyc_Yystacktype*_T4F4;struct Cyc_Yystacktype _T4F5;struct Cyc_Yyltype _T4F6;unsigned _T4F7;unsigned _T4F8;struct Cyc_Parse_Type_specifier _T4F9;struct Cyc_List_List*(*_T4FA)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T4FB)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T4FC;struct Cyc_Yystacktype _T4FD;struct Cyc_Yyltype _T4FE;unsigned _T4FF;unsigned _T500;struct Cyc_Yystacktype*_T501;union Cyc_YYSTYPE*_T502;union Cyc_YYSTYPE*_T503;struct Cyc_List_List*_T504;struct Cyc_List_List*(*_T505)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T506)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T507;struct Cyc_Yystacktype _T508;struct Cyc_Yyltype _T509;unsigned _T50A;unsigned _T50B;struct Cyc_Yystacktype*_T50C;union Cyc_YYSTYPE*_T50D;union Cyc_YYSTYPE*_T50E;struct Cyc_List_List*_T50F;struct Cyc_Yystacktype*_T510;union Cyc_YYSTYPE*_T511;union Cyc_YYSTYPE*_T512;struct _tuple28 _T513;struct _tuple28*_T514;unsigned _T515;struct _tuple28*_T516;struct _tuple28 _T517;struct Cyc_Yystacktype*_T518;union Cyc_YYSTYPE*_T519;union Cyc_YYSTYPE*_T51A;struct _tuple25 _T51B;enum Cyc_Absyn_AggrKind _T51C;struct Cyc_Yystacktype*_T51D;union Cyc_YYSTYPE*_T51E;union Cyc_YYSTYPE*_T51F;struct _tuple0*_T520;struct Cyc_List_List*_T521;struct Cyc_List_List*_T522;struct Cyc_List_List*_T523;struct Cyc_List_List*_T524;struct Cyc_Yystacktype*_T525;union Cyc_YYSTYPE*_T526;union Cyc_YYSTYPE*_T527;struct Cyc_List_List*_T528;struct Cyc_Yystacktype*_T529;union Cyc_YYSTYPE*_T52A;union Cyc_YYSTYPE*_T52B;struct _tuple25 _T52C;int _T52D;struct Cyc_Absyn_AggrdeclImpl*_T52E;struct Cyc_Yystacktype*_T52F;struct Cyc_Yystacktype _T530;struct Cyc_Yyltype _T531;unsigned _T532;unsigned _T533;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T534;void*_T535;struct Cyc_Yystacktype*_T536;struct Cyc_Yystacktype _T537;struct Cyc_Yyltype _T538;unsigned _T539;unsigned _T53A;struct Cyc_Parse_Type_specifier _T53B;struct Cyc_Yystacktype*_T53C;union Cyc_YYSTYPE*_T53D;union Cyc_YYSTYPE*_T53E;struct _tuple25 _T53F;enum Cyc_Absyn_AggrKind _T540;struct Cyc_Yystacktype*_T541;union Cyc_YYSTYPE*_T542;union Cyc_YYSTYPE*_T543;struct _tuple0*_T544;struct Cyc_Core_Opt*_T545;struct Cyc_Yystacktype*_T546;union Cyc_YYSTYPE*_T547;union Cyc_YYSTYPE*_T548;struct _tuple25 _T549;int _T54A;struct Cyc_Core_Opt*_T54B;union Cyc_Absyn_AggrInfo _T54C;struct Cyc_Yystacktype*_T54D;union Cyc_YYSTYPE*_T54E;union Cyc_YYSTYPE*_T54F;struct Cyc_List_List*_T550;void*_T551;struct Cyc_Yystacktype*_T552;struct Cyc_Yystacktype _T553;struct Cyc_Yyltype _T554;unsigned _T555;unsigned _T556;struct Cyc_Parse_Type_specifier _T557;struct _tuple25 _T558;struct Cyc_Yystacktype*_T559;union Cyc_YYSTYPE*_T55A;union Cyc_YYSTYPE*_T55B;struct _tuple25 _T55C;struct Cyc_Yystacktype*_T55D;union Cyc_YYSTYPE*_T55E;union Cyc_YYSTYPE*_T55F;struct Cyc_Yystacktype*_T560;union Cyc_YYSTYPE*_T561;union Cyc_YYSTYPE*_T562;struct Cyc_List_List*_T563;struct Cyc_List_List*_T564;struct Cyc_Yystacktype*_T565;union Cyc_YYSTYPE*_T566;union Cyc_YYSTYPE*_T567;struct Cyc_List_List*_T568;void*_T569;struct Cyc_List_List*_T56A;struct Cyc_List_List*_T56B;struct Cyc_List_List*_T56C;void(*_T56D)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*);void(*_T56E)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T56F;struct Cyc_List_List*_T570;struct Cyc_List_List*_T571;struct Cyc_Yystacktype*_T572;union Cyc_YYSTYPE*_T573;union Cyc_YYSTYPE*_T574;struct Cyc_List_List*_T575;struct Cyc_Yystacktype*_T576;union Cyc_YYSTYPE*_T577;union Cyc_YYSTYPE*_T578;struct Cyc_Yystacktype*_T579;union Cyc_YYSTYPE*_T57A;union Cyc_YYSTYPE*_T57B;struct _tuple11*_T57C;struct _RegionHandle*_T57D;struct Cyc_Yystacktype*_T57E;union Cyc_YYSTYPE*_T57F;union Cyc_YYSTYPE*_T580;struct _tuple11*_T581;struct _RegionHandle*_T582;struct Cyc_Yystacktype*_T583;union Cyc_YYSTYPE*_T584;union Cyc_YYSTYPE*_T585;struct Cyc_Yystacktype*_T586;union Cyc_YYSTYPE*_T587;union Cyc_YYSTYPE*_T588;struct _tuple12 _T589;struct Cyc_Yystacktype*_T58A;union Cyc_YYSTYPE*_T58B;union Cyc_YYSTYPE*_T58C;struct _tuple12 _T58D;struct Cyc_Yystacktype*_T58E;union Cyc_YYSTYPE*_T58F;union Cyc_YYSTYPE*_T590;struct Cyc_Yystacktype*_T591;union Cyc_YYSTYPE*_T592;union Cyc_YYSTYPE*_T593;void*_T594;struct Cyc_Yystacktype*_T595;struct Cyc_Yystacktype _T596;struct Cyc_Yyltype _T597;unsigned _T598;unsigned _T599;struct _tuple12 _T59A;struct Cyc_Yystacktype*_T59B;union Cyc_YYSTYPE*_T59C;union Cyc_YYSTYPE*_T59D;struct Cyc_Yystacktype*_T59E;union Cyc_YYSTYPE*_T59F;union Cyc_YYSTYPE*_T5A0;struct _tuple12 _T5A1;struct Cyc_Yystacktype*_T5A2;union Cyc_YYSTYPE*_T5A3;union Cyc_YYSTYPE*_T5A4;struct Cyc_Yystacktype*_T5A5;union Cyc_YYSTYPE*_T5A6;union Cyc_YYSTYPE*_T5A7;struct Cyc_Yystacktype*_T5A8;union Cyc_YYSTYPE*_T5A9;union Cyc_YYSTYPE*_T5AA;void*_T5AB;struct Cyc_Yystacktype*_T5AC;struct Cyc_Yystacktype _T5AD;struct Cyc_Yyltype _T5AE;unsigned _T5AF;unsigned _T5B0;struct Cyc_Yystacktype*_T5B1;union Cyc_YYSTYPE*_T5B2;union Cyc_YYSTYPE*_T5B3;struct Cyc_Absyn_Tqual _T5B4;unsigned _T5B5;struct Cyc_Yystacktype*_T5B6;struct Cyc_Yystacktype _T5B7;struct Cyc_Yyltype _T5B8;unsigned _T5B9;struct Cyc_Yystacktype*_T5BA;union Cyc_YYSTYPE*_T5BB;union Cyc_YYSTYPE*_T5BC;struct Cyc_List_List*_T5BD;void*_T5BE;struct _tuple13*_T5BF;struct _RegionHandle*_T5C0;struct Cyc_List_List*_T5C1;struct _RegionHandle*_T5C2;struct _tuple16*_T5C3;struct _RegionHandle*_T5C4;struct Cyc_List_List*_T5C5;struct Cyc_Parse_Type_specifier _T5C6;struct Cyc_Yystacktype*_T5C7;struct Cyc_Yystacktype _T5C8;struct Cyc_Yyltype _T5C9;unsigned _T5CA;unsigned _T5CB;struct _RegionHandle*_T5CC;struct _RegionHandle*_T5CD;struct Cyc_List_List*_T5CE;struct Cyc_List_List*_T5CF;struct Cyc_List_List*(*_T5D0)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T5D1)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T5D2;struct Cyc_Yystacktype _T5D3;struct Cyc_Yyltype _T5D4;unsigned _T5D5;unsigned _T5D6;struct Cyc_List_List*_T5D7;struct Cyc_List_List*_T5D8;struct _tuple26 _T5D9;struct Cyc_Yystacktype*_T5DA;struct Cyc_Yystacktype _T5DB;struct Cyc_Yyltype _T5DC;unsigned _T5DD;unsigned _T5DE;struct Cyc_Yystacktype*_T5DF;union Cyc_YYSTYPE*_T5E0;union Cyc_YYSTYPE*_T5E1;struct Cyc_Yystacktype*_T5E2;union Cyc_YYSTYPE*_T5E3;union Cyc_YYSTYPE*_T5E4;struct _tuple26 _T5E5;struct _tuple26 _T5E6;struct Cyc_Yystacktype*_T5E7;struct Cyc_Yystacktype _T5E8;struct Cyc_Yyltype _T5E9;unsigned _T5EA;unsigned _T5EB;struct Cyc_Yystacktype*_T5EC;union Cyc_YYSTYPE*_T5ED;union Cyc_YYSTYPE*_T5EE;struct Cyc_Parse_Type_specifier _T5EF;struct _tuple26 _T5F0;struct Cyc_Parse_Type_specifier _T5F1;struct _tuple26 _T5F2;struct _tuple26 _T5F3;struct Cyc_Yystacktype*_T5F4;union Cyc_YYSTYPE*_T5F5;union Cyc_YYSTYPE*_T5F6;struct Cyc_Yystacktype*_T5F7;union Cyc_YYSTYPE*_T5F8;union Cyc_YYSTYPE*_T5F9;struct _tuple26 _T5FA;struct Cyc_Yystacktype*_T5FB;union Cyc_YYSTYPE*_T5FC;union Cyc_YYSTYPE*_T5FD;struct Cyc_Absyn_Tqual _T5FE;struct _tuple26 _T5FF;struct Cyc_Absyn_Tqual _T600;struct _tuple26 _T601;struct _tuple26 _T602;struct _tuple26 _T603;struct Cyc_Yystacktype*_T604;struct Cyc_Yystacktype _T605;struct Cyc_Yyltype _T606;unsigned _T607;unsigned _T608;struct Cyc_Yystacktype*_T609;union Cyc_YYSTYPE*_T60A;union Cyc_YYSTYPE*_T60B;struct Cyc_Yystacktype*_T60C;union Cyc_YYSTYPE*_T60D;union Cyc_YYSTYPE*_T60E;struct _tuple26 _T60F;struct _tuple26 _T610;struct _tuple26 _T611;struct Cyc_Yystacktype*_T612;union Cyc_YYSTYPE*_T613;union Cyc_YYSTYPE*_T614;struct Cyc_List_List*_T615;struct _tuple26 _T616;struct Cyc_List_List*_T617;struct _tuple26 _T618;struct Cyc_Yystacktype*_T619;struct Cyc_Yystacktype _T61A;struct Cyc_Yyltype _T61B;unsigned _T61C;unsigned _T61D;struct Cyc_Yystacktype*_T61E;union Cyc_YYSTYPE*_T61F;union Cyc_YYSTYPE*_T620;struct Cyc_Yystacktype*_T621;union Cyc_YYSTYPE*_T622;union Cyc_YYSTYPE*_T623;struct _tuple26 _T624;struct _tuple26 _T625;struct Cyc_Yystacktype*_T626;struct Cyc_Yystacktype _T627;struct Cyc_Yyltype _T628;unsigned _T629;unsigned _T62A;struct Cyc_Yystacktype*_T62B;union Cyc_YYSTYPE*_T62C;union Cyc_YYSTYPE*_T62D;struct Cyc_Parse_Type_specifier _T62E;struct _tuple26 _T62F;struct Cyc_Parse_Type_specifier _T630;struct _tuple26 _T631;struct _tuple26 _T632;struct Cyc_Yystacktype*_T633;union Cyc_YYSTYPE*_T634;union Cyc_YYSTYPE*_T635;struct Cyc_Yystacktype*_T636;union Cyc_YYSTYPE*_T637;union Cyc_YYSTYPE*_T638;struct _tuple26 _T639;struct Cyc_Yystacktype*_T63A;union Cyc_YYSTYPE*_T63B;union Cyc_YYSTYPE*_T63C;struct Cyc_Absyn_Tqual _T63D;struct _tuple26 _T63E;struct Cyc_Absyn_Tqual _T63F;struct _tuple26 _T640;struct _tuple26 _T641;struct _tuple26 _T642;struct Cyc_Yystacktype*_T643;struct Cyc_Yystacktype _T644;struct Cyc_Yyltype _T645;unsigned _T646;unsigned _T647;struct Cyc_Yystacktype*_T648;union Cyc_YYSTYPE*_T649;union Cyc_YYSTYPE*_T64A;struct Cyc_Yystacktype*_T64B;union Cyc_YYSTYPE*_T64C;union Cyc_YYSTYPE*_T64D;struct _tuple26 _T64E;struct _tuple26 _T64F;struct _tuple26 _T650;struct Cyc_Yystacktype*_T651;union Cyc_YYSTYPE*_T652;union Cyc_YYSTYPE*_T653;struct Cyc_List_List*_T654;struct _tuple26 _T655;struct Cyc_List_List*_T656;struct Cyc_List_List*_T657;struct _RegionHandle*_T658;struct Cyc_Yystacktype*_T659;union Cyc_YYSTYPE*_T65A;union Cyc_YYSTYPE*_T65B;struct Cyc_List_List*_T65C;struct _RegionHandle*_T65D;struct Cyc_Yystacktype*_T65E;union Cyc_YYSTYPE*_T65F;union Cyc_YYSTYPE*_T660;struct Cyc_Yystacktype*_T661;union Cyc_YYSTYPE*_T662;union Cyc_YYSTYPE*_T663;struct _tuple12*_T664;struct _RegionHandle*_T665;struct Cyc_Yystacktype*_T666;union Cyc_YYSTYPE*_T667;union Cyc_YYSTYPE*_T668;struct Cyc_Yystacktype*_T669;union Cyc_YYSTYPE*_T66A;union Cyc_YYSTYPE*_T66B;struct _tuple12*_T66C;struct _RegionHandle*_T66D;struct _tuple0*_T66E;struct _fat_ptr*_T66F;struct Cyc_Yystacktype*_T670;union Cyc_YYSTYPE*_T671;union Cyc_YYSTYPE*_T672;struct _tuple12*_T673;struct _RegionHandle*_T674;struct _tuple0*_T675;struct _fat_ptr*_T676;struct _tuple12*_T677;struct _RegionHandle*_T678;struct Cyc_Yystacktype*_T679;union Cyc_YYSTYPE*_T67A;union Cyc_YYSTYPE*_T67B;struct Cyc_Yystacktype*_T67C;union Cyc_YYSTYPE*_T67D;union Cyc_YYSTYPE*_T67E;struct Cyc_Yystacktype*_T67F;union Cyc_YYSTYPE*_T680;union Cyc_YYSTYPE*_T681;struct Cyc_Absyn_Exp*_T682;struct Cyc_Yystacktype*_T683;union Cyc_YYSTYPE*_T684;union Cyc_YYSTYPE*_T685;struct Cyc_List_List*(*_T686)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T687)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T688;struct Cyc_Yystacktype _T689;struct Cyc_Yyltype _T68A;unsigned _T68B;unsigned _T68C;struct Cyc_Yystacktype*_T68D;union Cyc_YYSTYPE*_T68E;union Cyc_YYSTYPE*_T68F;struct Cyc_List_List*_T690;struct Cyc_Yystacktype*_T691;union Cyc_YYSTYPE*_T692;union Cyc_YYSTYPE*_T693;struct _tuple0*_T694;struct Cyc_List_List*_T695;struct Cyc_Core_Opt*_T696;struct Cyc_Yystacktype*_T697;union Cyc_YYSTYPE*_T698;union Cyc_YYSTYPE*_T699;int _T69A;struct Cyc_Yystacktype*_T69B;struct Cyc_Yystacktype _T69C;struct Cyc_Yyltype _T69D;unsigned _T69E;unsigned _T69F;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T6A0;void*_T6A1;struct Cyc_Yystacktype*_T6A2;struct Cyc_Yystacktype _T6A3;struct Cyc_Yyltype _T6A4;unsigned _T6A5;unsigned _T6A6;struct Cyc_Parse_Type_specifier _T6A7;struct Cyc_Yystacktype*_T6A8;union Cyc_YYSTYPE*_T6A9;union Cyc_YYSTYPE*_T6AA;struct Cyc_Absyn_UnknownDatatypeInfo _T6AB;struct Cyc_Yystacktype*_T6AC;union Cyc_YYSTYPE*_T6AD;union Cyc_YYSTYPE*_T6AE;union Cyc_Absyn_DatatypeInfo _T6AF;struct Cyc_Yystacktype*_T6B0;union Cyc_YYSTYPE*_T6B1;union Cyc_YYSTYPE*_T6B2;struct Cyc_List_List*_T6B3;void*_T6B4;struct Cyc_Yystacktype*_T6B5;struct Cyc_Yystacktype _T6B6;struct Cyc_Yyltype _T6B7;unsigned _T6B8;unsigned _T6B9;struct Cyc_Parse_Type_specifier _T6BA;struct Cyc_Yystacktype*_T6BB;union Cyc_YYSTYPE*_T6BC;union Cyc_YYSTYPE*_T6BD;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T6BE;struct Cyc_Yystacktype*_T6BF;union Cyc_YYSTYPE*_T6C0;union Cyc_YYSTYPE*_T6C1;struct Cyc_Yystacktype*_T6C2;union Cyc_YYSTYPE*_T6C3;union Cyc_YYSTYPE*_T6C4;union Cyc_Absyn_DatatypeFieldInfo _T6C5;struct Cyc_Yystacktype*_T6C6;union Cyc_YYSTYPE*_T6C7;union Cyc_YYSTYPE*_T6C8;struct Cyc_List_List*_T6C9;void*_T6CA;struct Cyc_Yystacktype*_T6CB;struct Cyc_Yystacktype _T6CC;struct Cyc_Yyltype _T6CD;unsigned _T6CE;unsigned _T6CF;struct Cyc_Parse_Type_specifier _T6D0;struct Cyc_List_List*_T6D1;struct Cyc_Yystacktype*_T6D2;union Cyc_YYSTYPE*_T6D3;union Cyc_YYSTYPE*_T6D4;struct Cyc_List_List*_T6D5;struct Cyc_Yystacktype*_T6D6;union Cyc_YYSTYPE*_T6D7;union Cyc_YYSTYPE*_T6D8;struct Cyc_List_List*_T6D9;struct Cyc_Yystacktype*_T6DA;union Cyc_YYSTYPE*_T6DB;union Cyc_YYSTYPE*_T6DC;struct Cyc_Yystacktype*_T6DD;union Cyc_YYSTYPE*_T6DE;union Cyc_YYSTYPE*_T6DF;struct Cyc_List_List*_T6E0;struct Cyc_Yystacktype*_T6E1;union Cyc_YYSTYPE*_T6E2;union Cyc_YYSTYPE*_T6E3;struct Cyc_Yystacktype*_T6E4;union Cyc_YYSTYPE*_T6E5;union Cyc_YYSTYPE*_T6E6;struct Cyc_Absyn_Datatypefield*_T6E7;struct Cyc_Yystacktype*_T6E8;union Cyc_YYSTYPE*_T6E9;union Cyc_YYSTYPE*_T6EA;struct Cyc_Yystacktype*_T6EB;struct Cyc_Yystacktype _T6EC;struct Cyc_Yyltype _T6ED;unsigned _T6EE;struct Cyc_Yystacktype*_T6EF;union Cyc_YYSTYPE*_T6F0;union Cyc_YYSTYPE*_T6F1;struct Cyc_List_List*(*_T6F2)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T6F3)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T6F4)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T6F5;struct Cyc_Yystacktype _T6F6;struct Cyc_Yyltype _T6F7;unsigned _T6F8;unsigned _T6F9;struct Cyc_Yystacktype*_T6FA;union Cyc_YYSTYPE*_T6FB;union Cyc_YYSTYPE*_T6FC;struct Cyc_List_List*_T6FD;struct Cyc_List_List*_T6FE;struct Cyc_Absyn_Datatypefield*_T6FF;struct Cyc_Yystacktype*_T700;union Cyc_YYSTYPE*_T701;union Cyc_YYSTYPE*_T702;struct Cyc_Yystacktype*_T703;struct Cyc_Yystacktype _T704;struct Cyc_Yyltype _T705;unsigned _T706;struct Cyc_Yystacktype*_T707;union Cyc_YYSTYPE*_T708;union Cyc_YYSTYPE*_T709;struct Cyc_Yystacktype*_T70A;struct Cyc_Yystacktype _T70B;struct Cyc_Yystacktype*_T70C;union Cyc_YYSTYPE*_T70D;union Cyc_YYSTYPE*_T70E;struct Cyc_Parse_Declarator _T70F;struct Cyc_Parse_Declarator _T710;struct Cyc_Parse_Declarator _T711;struct Cyc_Yystacktype*_T712;union Cyc_YYSTYPE*_T713;union Cyc_YYSTYPE*_T714;struct Cyc_List_List*_T715;struct Cyc_Parse_Declarator _T716;struct Cyc_List_List*_T717;struct Cyc_Yystacktype*_T718;struct Cyc_Yystacktype _T719;struct Cyc_Yystacktype*_T71A;union Cyc_YYSTYPE*_T71B;union Cyc_YYSTYPE*_T71C;struct Cyc_Parse_Declarator _T71D;struct Cyc_Parse_Declarator _T71E;struct Cyc_Parse_Declarator _T71F;struct Cyc_Yystacktype*_T720;union Cyc_YYSTYPE*_T721;union Cyc_YYSTYPE*_T722;struct Cyc_List_List*_T723;struct Cyc_Parse_Declarator _T724;struct Cyc_List_List*_T725;struct Cyc_Parse_Declarator _T726;struct Cyc_Yystacktype*_T727;union Cyc_YYSTYPE*_T728;union Cyc_YYSTYPE*_T729;struct Cyc_Yystacktype*_T72A;struct Cyc_Yystacktype _T72B;struct Cyc_Yyltype _T72C;unsigned _T72D;struct Cyc_Yystacktype*_T72E;struct Cyc_Yystacktype _T72F;struct Cyc_Yystacktype*_T730;union Cyc_YYSTYPE*_T731;union Cyc_YYSTYPE*_T732;struct Cyc_List_List*_T733;struct _RegionHandle*_T734;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T735;struct _RegionHandle*_T736;struct Cyc_Yystacktype*_T737;struct Cyc_Yystacktype _T738;struct Cyc_Yyltype _T739;unsigned _T73A;struct Cyc_Yystacktype*_T73B;union Cyc_YYSTYPE*_T73C;union Cyc_YYSTYPE*_T73D;struct Cyc_Parse_Declarator _T73E;struct Cyc_Yystacktype*_T73F;struct Cyc_Yystacktype _T740;struct Cyc_Parse_Declarator _T741;struct Cyc_Yystacktype*_T742;union Cyc_YYSTYPE*_T743;union Cyc_YYSTYPE*_T744;struct Cyc_Parse_Declarator _T745;struct Cyc_Yystacktype*_T746;union Cyc_YYSTYPE*_T747;union Cyc_YYSTYPE*_T748;struct Cyc_Parse_Declarator _T749;struct Cyc_List_List*_T74A;struct _RegionHandle*_T74B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T74C;struct _RegionHandle*_T74D;struct Cyc_Yystacktype*_T74E;union Cyc_YYSTYPE*_T74F;union Cyc_YYSTYPE*_T750;struct Cyc_Yystacktype*_T751;struct Cyc_Yystacktype _T752;struct Cyc_Yyltype _T753;unsigned _T754;struct Cyc_Yystacktype*_T755;union Cyc_YYSTYPE*_T756;union Cyc_YYSTYPE*_T757;struct Cyc_Parse_Declarator _T758;struct Cyc_Parse_Declarator _T759;struct Cyc_Yystacktype*_T75A;union Cyc_YYSTYPE*_T75B;union Cyc_YYSTYPE*_T75C;struct Cyc_Parse_Declarator _T75D;struct Cyc_Yystacktype*_T75E;union Cyc_YYSTYPE*_T75F;union Cyc_YYSTYPE*_T760;struct Cyc_Parse_Declarator _T761;struct Cyc_List_List*_T762;struct _RegionHandle*_T763;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T764;struct _RegionHandle*_T765;struct Cyc_Yystacktype*_T766;union Cyc_YYSTYPE*_T767;union Cyc_YYSTYPE*_T768;struct Cyc_Yystacktype*_T769;union Cyc_YYSTYPE*_T76A;union Cyc_YYSTYPE*_T76B;struct Cyc_Yystacktype*_T76C;struct Cyc_Yystacktype _T76D;struct Cyc_Yyltype _T76E;unsigned _T76F;struct Cyc_Yystacktype*_T770;union Cyc_YYSTYPE*_T771;union Cyc_YYSTYPE*_T772;struct Cyc_Parse_Declarator _T773;struct Cyc_Yystacktype*_T774;union Cyc_YYSTYPE*_T775;union Cyc_YYSTYPE*_T776;struct Cyc_Yystacktype*_T777;union Cyc_YYSTYPE*_T778;union Cyc_YYSTYPE*_T779;struct Cyc_Parse_Declarator _T77A;struct Cyc_Yystacktype*_T77B;union Cyc_YYSTYPE*_T77C;union Cyc_YYSTYPE*_T77D;struct Cyc_Parse_Declarator _T77E;struct Cyc_Yystacktype*_T77F;union Cyc_YYSTYPE*_T780;union Cyc_YYSTYPE*_T781;struct Cyc_Parse_Declarator _T782;struct Cyc_List_List*_T783;struct _RegionHandle*_T784;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T785;struct _RegionHandle*_T786;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T787;struct _RegionHandle*_T788;struct Cyc_Yystacktype*_T789;union Cyc_YYSTYPE*_T78A;union Cyc_YYSTYPE*_T78B;struct Cyc_Parse_Declarator _T78C;struct Cyc_Parse_Declarator _T78D;struct Cyc_Yystacktype*_T78E;union Cyc_YYSTYPE*_T78F;union Cyc_YYSTYPE*_T790;struct Cyc_Parse_Declarator _T791;struct Cyc_Yystacktype*_T792;union Cyc_YYSTYPE*_T793;union Cyc_YYSTYPE*_T794;struct Cyc_Parse_Declarator _T795;struct Cyc_List_List*_T796;struct _RegionHandle*_T797;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T798;struct _RegionHandle*_T799;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T79A;struct _RegionHandle*_T79B;struct Cyc_Yystacktype*_T79C;union Cyc_YYSTYPE*_T79D;union Cyc_YYSTYPE*_T79E;struct Cyc_Yystacktype*_T79F;struct Cyc_Yystacktype _T7A0;struct Cyc_Yyltype _T7A1;unsigned _T7A2;struct Cyc_Yystacktype*_T7A3;union Cyc_YYSTYPE*_T7A4;union Cyc_YYSTYPE*_T7A5;struct Cyc_Parse_Declarator _T7A6;struct Cyc_List_List*(*_T7A7)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T7A8)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T7A9;struct Cyc_Yystacktype _T7AA;struct Cyc_Yyltype _T7AB;unsigned _T7AC;unsigned _T7AD;struct Cyc_Yystacktype*_T7AE;union Cyc_YYSTYPE*_T7AF;union Cyc_YYSTYPE*_T7B0;struct Cyc_List_List*_T7B1;struct Cyc_List_List*_T7B2;struct Cyc_Parse_Declarator _T7B3;struct Cyc_Yystacktype*_T7B4;union Cyc_YYSTYPE*_T7B5;union Cyc_YYSTYPE*_T7B6;struct Cyc_Parse_Declarator _T7B7;struct Cyc_Yystacktype*_T7B8;union Cyc_YYSTYPE*_T7B9;union Cyc_YYSTYPE*_T7BA;struct Cyc_Parse_Declarator _T7BB;struct Cyc_List_List*_T7BC;struct _RegionHandle*_T7BD;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T7BE;struct _RegionHandle*_T7BF;struct Cyc_Yystacktype*_T7C0;struct Cyc_Yystacktype _T7C1;struct Cyc_Yyltype _T7C2;unsigned _T7C3;struct Cyc_Yystacktype*_T7C4;union Cyc_YYSTYPE*_T7C5;union Cyc_YYSTYPE*_T7C6;struct Cyc_Parse_Declarator _T7C7;struct Cyc_Parse_Declarator _T7C8;struct Cyc_Yystacktype*_T7C9;union Cyc_YYSTYPE*_T7CA;union Cyc_YYSTYPE*_T7CB;struct Cyc_Parse_Declarator _T7CC;struct Cyc_Yystacktype*_T7CD;union Cyc_YYSTYPE*_T7CE;union Cyc_YYSTYPE*_T7CF;struct Cyc_Parse_Declarator _T7D0;struct Cyc_List_List*_T7D1;struct _RegionHandle*_T7D2;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T7D3;struct _RegionHandle*_T7D4;struct Cyc_Yystacktype*_T7D5;struct Cyc_Yystacktype _T7D6;struct Cyc_Yyltype _T7D7;unsigned _T7D8;struct Cyc_Yystacktype*_T7D9;union Cyc_YYSTYPE*_T7DA;union Cyc_YYSTYPE*_T7DB;struct Cyc_Yystacktype*_T7DC;union Cyc_YYSTYPE*_T7DD;union Cyc_YYSTYPE*_T7DE;struct Cyc_Parse_Declarator _T7DF;struct Cyc_Parse_Declarator _T7E0;struct Cyc_Yystacktype*_T7E1;union Cyc_YYSTYPE*_T7E2;union Cyc_YYSTYPE*_T7E3;struct Cyc_Yystacktype*_T7E4;struct Cyc_Yystacktype _T7E5;struct Cyc_Yyltype _T7E6;unsigned _T7E7;struct Cyc_Parse_Declarator _T7E8;struct Cyc_Yystacktype*_T7E9;union Cyc_YYSTYPE*_T7EA;union Cyc_YYSTYPE*_T7EB;struct Cyc_Yystacktype*_T7EC;struct Cyc_Yystacktype _T7ED;struct Cyc_Yyltype _T7EE;unsigned _T7EF;struct Cyc_Yystacktype*_T7F0;struct Cyc_Yystacktype _T7F1;struct Cyc_Yystacktype*_T7F2;union Cyc_YYSTYPE*_T7F3;union Cyc_YYSTYPE*_T7F4;struct Cyc_List_List*_T7F5;struct _RegionHandle*_T7F6;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T7F7;struct _RegionHandle*_T7F8;struct Cyc_Yystacktype*_T7F9;struct Cyc_Yystacktype _T7FA;struct Cyc_Yyltype _T7FB;unsigned _T7FC;struct Cyc_Yystacktype*_T7FD;union Cyc_YYSTYPE*_T7FE;union Cyc_YYSTYPE*_T7FF;struct Cyc_Parse_Declarator _T800;struct Cyc_Yystacktype*_T801;struct Cyc_Yystacktype _T802;struct Cyc_Yystacktype*_T803;union Cyc_YYSTYPE*_T804;union Cyc_YYSTYPE*_T805;struct Cyc_Parse_Declarator _T806;struct Cyc_Parse_Declarator _T807;struct Cyc_Parse_Declarator _T808;struct Cyc_List_List*_T809;struct _RegionHandle*_T80A;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T80B;struct _RegionHandle*_T80C;struct Cyc_Yystacktype*_T80D;union Cyc_YYSTYPE*_T80E;union Cyc_YYSTYPE*_T80F;struct Cyc_Yystacktype*_T810;struct Cyc_Yystacktype _T811;struct Cyc_Yyltype _T812;unsigned _T813;struct Cyc_Parse_Declarator _T814;struct Cyc_Yystacktype*_T815;union Cyc_YYSTYPE*_T816;union Cyc_YYSTYPE*_T817;struct Cyc_Parse_Declarator _T818;struct Cyc_Parse_Declarator _T819;struct Cyc_Parse_Declarator _T81A;struct Cyc_List_List*_T81B;struct _RegionHandle*_T81C;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T81D;struct _RegionHandle*_T81E;struct Cyc_Yystacktype*_T81F;union Cyc_YYSTYPE*_T820;union Cyc_YYSTYPE*_T821;struct Cyc_Yystacktype*_T822;union Cyc_YYSTYPE*_T823;union Cyc_YYSTYPE*_T824;struct Cyc_Yystacktype*_T825;struct Cyc_Yystacktype _T826;struct Cyc_Yyltype _T827;unsigned _T828;struct Cyc_Parse_Declarator _T829;struct Cyc_Yystacktype*_T82A;union Cyc_YYSTYPE*_T82B;union Cyc_YYSTYPE*_T82C;struct Cyc_Yystacktype*_T82D;union Cyc_YYSTYPE*_T82E;union Cyc_YYSTYPE*_T82F;struct Cyc_Yystacktype*_T830;union Cyc_YYSTYPE*_T831;union Cyc_YYSTYPE*_T832;struct Cyc_Parse_Declarator _T833;struct Cyc_Parse_Declarator _T834;struct Cyc_Parse_Declarator _T835;struct Cyc_List_List*_T836;struct _RegionHandle*_T837;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T838;struct _RegionHandle*_T839;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T83A;struct _RegionHandle*_T83B;struct Cyc_Parse_Declarator _T83C;struct Cyc_Yystacktype*_T83D;union Cyc_YYSTYPE*_T83E;union Cyc_YYSTYPE*_T83F;struct Cyc_Parse_Declarator _T840;struct Cyc_Parse_Declarator _T841;struct Cyc_Parse_Declarator _T842;struct Cyc_List_List*_T843;struct _RegionHandle*_T844;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T845;struct _RegionHandle*_T846;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T847;struct _RegionHandle*_T848;struct Cyc_Yystacktype*_T849;union Cyc_YYSTYPE*_T84A;union Cyc_YYSTYPE*_T84B;struct Cyc_Yystacktype*_T84C;struct Cyc_Yystacktype _T84D;struct Cyc_Yyltype _T84E;unsigned _T84F;struct Cyc_Parse_Declarator _T850;struct Cyc_List_List*(*_T851)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T852)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T853;struct Cyc_Yystacktype _T854;struct Cyc_Yyltype _T855;unsigned _T856;unsigned _T857;struct Cyc_Yystacktype*_T858;union Cyc_YYSTYPE*_T859;union Cyc_YYSTYPE*_T85A;struct Cyc_List_List*_T85B;struct Cyc_List_List*_T85C;struct Cyc_Yystacktype*_T85D;union Cyc_YYSTYPE*_T85E;union Cyc_YYSTYPE*_T85F;struct Cyc_Parse_Declarator _T860;struct Cyc_Parse_Declarator _T861;struct Cyc_Parse_Declarator _T862;struct Cyc_List_List*_T863;struct _RegionHandle*_T864;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T865;struct _RegionHandle*_T866;struct Cyc_Yystacktype*_T867;struct Cyc_Yystacktype _T868;struct Cyc_Yyltype _T869;unsigned _T86A;struct Cyc_Parse_Declarator _T86B;struct Cyc_Yystacktype*_T86C;union Cyc_YYSTYPE*_T86D;union Cyc_YYSTYPE*_T86E;struct Cyc_Parse_Declarator _T86F;struct Cyc_Parse_Declarator _T870;struct Cyc_Parse_Declarator _T871;struct Cyc_List_List*_T872;struct _RegionHandle*_T873;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T874;struct _RegionHandle*_T875;struct Cyc_Yystacktype*_T876;struct Cyc_Yystacktype _T877;struct Cyc_Yyltype _T878;unsigned _T879;struct Cyc_Yystacktype*_T87A;union Cyc_YYSTYPE*_T87B;union Cyc_YYSTYPE*_T87C;struct Cyc_Parse_Declarator _T87D;struct Cyc_Yystacktype*_T87E;struct Cyc_Yystacktype _T87F;struct Cyc_Yystacktype*_T880;union Cyc_YYSTYPE*_T881;union Cyc_YYSTYPE*_T882;struct Cyc_List_List*_T883;struct Cyc_Yystacktype*_T884;union Cyc_YYSTYPE*_T885;union Cyc_YYSTYPE*_T886;struct Cyc_List_List*_T887;struct Cyc_List_List*_T888;struct Cyc_Yystacktype*_T889;union Cyc_YYSTYPE*_T88A;union Cyc_YYSTYPE*_T88B;struct Cyc_List_List*_T88C;struct Cyc_List_List*_T88D;struct _RegionHandle*_T88E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T88F;struct _RegionHandle*_T890;struct Cyc_Yystacktype*_T891;struct Cyc_Yystacktype _T892;struct Cyc_Yyltype _T893;unsigned _T894;struct Cyc_Yystacktype*_T895;union Cyc_YYSTYPE*_T896;union Cyc_YYSTYPE*_T897;struct Cyc_Yystacktype*_T898;union Cyc_YYSTYPE*_T899;union Cyc_YYSTYPE*_T89A;struct _tuple22*_T89B;int _T89C;struct Cyc_Absyn_PtrLoc*_T89D;struct Cyc_Yystacktype*_T89E;struct Cyc_Yystacktype _T89F;struct Cyc_Yyltype _T8A0;unsigned _T8A1;struct Cyc_Yystacktype*_T8A2;struct Cyc_Yystacktype _T8A3;struct Cyc_Yyltype _T8A4;unsigned _T8A5;struct _RegionHandle*_T8A6;struct Cyc_Absyn_PtrLoc*_T8A7;void*_T8A8;void*_T8A9;struct Cyc_Yystacktype*_T8AA;union Cyc_YYSTYPE*_T8AB;union Cyc_YYSTYPE*_T8AC;void*_T8AD;struct Cyc_Yystacktype*_T8AE;union Cyc_YYSTYPE*_T8AF;union Cyc_YYSTYPE*_T8B0;struct Cyc_List_List*_T8B1;struct Cyc_Yystacktype*_T8B2;union Cyc_YYSTYPE*_T8B3;union Cyc_YYSTYPE*_T8B4;struct Cyc_Absyn_Tqual _T8B5;struct Cyc_List_List*_T8B6;struct _RegionHandle*_T8B7;struct Cyc_List_List*_T8B8;struct _RegionHandle*_T8B9;struct Cyc_Yystacktype*_T8BA;union Cyc_YYSTYPE*_T8BB;union Cyc_YYSTYPE*_T8BC;struct Cyc_Yystacktype*_T8BD;union Cyc_YYSTYPE*_T8BE;union Cyc_YYSTYPE*_T8BF;struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T8C0;struct _RegionHandle*_T8C1;struct Cyc_Yystacktype*_T8C2;union Cyc_YYSTYPE*_T8C3;union Cyc_YYSTYPE*_T8C4;void*_T8C5;struct Cyc_Yystacktype*_T8C6;struct Cyc_Yystacktype _T8C7;struct Cyc_Yyltype _T8C8;unsigned _T8C9;unsigned _T8CA;struct _fat_ptr _T8CB;struct _fat_ptr _T8CC;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T8CD;struct _RegionHandle*_T8CE;struct Cyc_List_List*_T8CF;struct Cyc_Yystacktype*_T8D0;union Cyc_YYSTYPE*_T8D1;union Cyc_YYSTYPE*_T8D2;void*_T8D3;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T8D4;struct _RegionHandle*_T8D5;struct Cyc_Yystacktype*_T8D6;union Cyc_YYSTYPE*_T8D7;union Cyc_YYSTYPE*_T8D8;void*_T8D9;struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T8DA;struct _RegionHandle*_T8DB;void*_T8DC;struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T8DD;struct _RegionHandle*_T8DE;void*_T8DF;struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T8E0;struct _RegionHandle*_T8E1;void*_T8E2;struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T8E3;struct _RegionHandle*_T8E4;void*_T8E5;struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T8E6;struct _RegionHandle*_T8E7;void*_T8E8;struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T8E9;struct _RegionHandle*_T8EA;void*_T8EB;struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T8EC;struct _RegionHandle*_T8ED;void*_T8EE;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T8EF;struct _RegionHandle*_T8F0;struct Cyc_Yystacktype*_T8F1;union Cyc_YYSTYPE*_T8F2;union Cyc_YYSTYPE*_T8F3;void*_T8F4;struct Cyc_Yystacktype*_T8F5;struct Cyc_Yystacktype _T8F6;struct Cyc_Yyltype _T8F7;unsigned _T8F8;unsigned _T8F9;struct Cyc_Yystacktype*_T8FA;union Cyc_YYSTYPE*_T8FB;union Cyc_YYSTYPE*_T8FC;struct _fat_ptr _T8FD;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T8FE;struct _RegionHandle*_T8FF;void*_T900;struct Cyc_Yystacktype*_T901;union Cyc_YYSTYPE*_T902;union Cyc_YYSTYPE*_T903;void*_T904;struct Cyc_Yystacktype*_T905;union Cyc_YYSTYPE*_T906;union Cyc_YYSTYPE*_T907;void*_T908;void*_T909;void*_T90A;struct Cyc_Yystacktype*_T90B;union Cyc_YYSTYPE*_T90C;union Cyc_YYSTYPE*_T90D;void*_T90E;void*_T90F;void*_T910;void*_T911;struct Cyc_Yystacktype*_T912;struct Cyc_Yystacktype _T913;struct Cyc_Yyltype _T914;unsigned _T915;unsigned _T916;struct Cyc_Yystacktype*_T917;union Cyc_YYSTYPE*_T918;union Cyc_YYSTYPE*_T919;struct _fat_ptr _T91A;void*_T91B;struct _tuple22*_T91C;struct Cyc_Yystacktype*_T91D;struct Cyc_Yystacktype _T91E;struct Cyc_Yyltype _T91F;unsigned _T920;int _T921;struct Cyc_Yystacktype*_T922;struct Cyc_Yystacktype _T923;struct Cyc_Yyltype _T924;unsigned _T925;unsigned _T926;struct _fat_ptr _T927;struct Cyc_Yystacktype*_T928;union Cyc_YYSTYPE*_T929;union Cyc_YYSTYPE*_T92A;void*_T92B;struct _tuple22*_T92C;struct Cyc_Yystacktype*_T92D;struct Cyc_Yystacktype _T92E;struct Cyc_Yyltype _T92F;unsigned _T930;struct Cyc_Yystacktype*_T931;struct Cyc_Yystacktype _T932;struct Cyc_Yyltype _T933;unsigned _T934;unsigned _T935;struct _fat_ptr _T936;struct Cyc_Yystacktype*_T937;union Cyc_YYSTYPE*_T938;union Cyc_YYSTYPE*_T939;void*_T93A;struct _tuple22*_T93B;int _T93C;int _T93D;struct _tuple22*_T93E;struct Cyc_Yystacktype*_T93F;struct Cyc_Yystacktype _T940;struct Cyc_Yyltype _T941;unsigned _T942;struct Cyc_Yystacktype*_T943;struct Cyc_Yystacktype _T944;struct Cyc_Yyltype _T945;unsigned _T946;unsigned _T947;struct _fat_ptr _T948;struct Cyc_Core_Opt*_T949;struct Cyc_Core_Opt*_T94A;void*_T94B;struct _tuple22*_T94C;struct Cyc_Yystacktype*_T94D;struct Cyc_Yystacktype _T94E;struct Cyc_Yyltype _T94F;unsigned _T950;void*_T951;int _T952;struct Cyc_Core_Opt*_T953;struct Cyc_Core_Opt*_T954;struct Cyc_Yystacktype*_T955;union Cyc_YYSTYPE*_T956;union Cyc_YYSTYPE*_T957;struct Cyc_Absyn_Exp*_T958;void*_T959;int _T95A;int(*_T95B)(unsigned,struct _fat_ptr);struct _fat_ptr _T95C;struct Cyc_Yystacktype*_T95D;union Cyc_YYSTYPE*_T95E;union Cyc_YYSTYPE*_T95F;struct _fat_ptr _T960;void*_T961;void*_T962;struct Cyc_List_List*_T963;struct Cyc_Yystacktype*_T964;union Cyc_YYSTYPE*_T965;union Cyc_YYSTYPE*_T966;struct Cyc_List_List*_T967;struct Cyc_Yystacktype*_T968;union Cyc_YYSTYPE*_T969;union Cyc_YYSTYPE*_T96A;struct Cyc_Yystacktype*_T96B;union Cyc_YYSTYPE*_T96C;union Cyc_YYSTYPE*_T96D;struct Cyc_Core_Opt*_T96E;struct Cyc_Core_Opt*_T96F;void*_T970;struct Cyc_Yystacktype*_T971;union Cyc_YYSTYPE*_T972;union Cyc_YYSTYPE*_T973;int _T974;struct Cyc_List_List*_T975;void*_T976;struct Cyc_Yystacktype*_T977;union Cyc_YYSTYPE*_T978;union Cyc_YYSTYPE*_T979;struct Cyc_List_List*_T97A;void*_T97B;struct Cyc_Core_Opt*_T97C;struct Cyc_Core_Opt*_T97D;void*_T97E;struct _fat_ptr _T97F;int _T980;char*_T981;struct Cyc_Yystacktype*_T982;struct Cyc_Yystacktype _T983;struct Cyc_Yyltype _T984;unsigned _T985;unsigned _T986;struct Cyc_Absyn_Tqual _T987;struct Cyc_Yystacktype*_T988;union Cyc_YYSTYPE*_T989;union Cyc_YYSTYPE*_T98A;struct Cyc_Absyn_Tqual _T98B;struct Cyc_Yystacktype*_T98C;union Cyc_YYSTYPE*_T98D;union Cyc_YYSTYPE*_T98E;struct Cyc_Absyn_Tqual _T98F;struct Cyc_Absyn_Tqual _T990;struct Cyc_Yystacktype*_T991;union Cyc_YYSTYPE*_T992;union Cyc_YYSTYPE*_T993;struct _tuple28 _T994;struct _tuple28*_T995;unsigned _T996;struct _tuple28*_T997;struct _tuple28 _T998;struct _tuple27*_T999;struct Cyc_Yystacktype*_T99A;union Cyc_YYSTYPE*_T99B;union Cyc_YYSTYPE*_T99C;struct Cyc_Yystacktype*_T99D;union Cyc_YYSTYPE*_T99E;union Cyc_YYSTYPE*_T99F;struct _tuple28 _T9A0;struct _tuple28*_T9A1;unsigned _T9A2;struct _tuple28*_T9A3;struct _tuple28 _T9A4;struct _tuple27*_T9A5;struct Cyc_Yystacktype*_T9A6;union Cyc_YYSTYPE*_T9A7;union Cyc_YYSTYPE*_T9A8;struct Cyc_List_List*_T9A9;struct Cyc_Yystacktype*_T9AA;union Cyc_YYSTYPE*_T9AB;union Cyc_YYSTYPE*_T9AC;struct Cyc_Yystacktype*_T9AD;union Cyc_YYSTYPE*_T9AE;union Cyc_YYSTYPE*_T9AF;struct _tuple28 _T9B0;struct _tuple28*_T9B1;unsigned _T9B2;struct _tuple28*_T9B3;struct _tuple28 _T9B4;struct _tuple27*_T9B5;struct Cyc_Yystacktype*_T9B6;union Cyc_YYSTYPE*_T9B7;union Cyc_YYSTYPE*_T9B8;struct Cyc_List_List*_T9B9;struct Cyc_Yystacktype*_T9BA;union Cyc_YYSTYPE*_T9BB;union Cyc_YYSTYPE*_T9BC;struct Cyc_Yystacktype*_T9BD;union Cyc_YYSTYPE*_T9BE;union Cyc_YYSTYPE*_T9BF;struct Cyc_Absyn_VarargInfo*_T9C0;struct Cyc_Absyn_VarargInfo*_T9C1;struct Cyc_Absyn_VarargInfo*_T9C2;struct Cyc_Absyn_VarargInfo*_T9C3;struct Cyc_Yystacktype*_T9C4;union Cyc_YYSTYPE*_T9C5;union Cyc_YYSTYPE*_T9C6;struct Cyc_Yystacktype*_T9C7;union Cyc_YYSTYPE*_T9C8;union Cyc_YYSTYPE*_T9C9;struct _tuple28 _T9CA;struct _tuple28*_T9CB;unsigned _T9CC;struct _tuple28*_T9CD;struct _tuple28 _T9CE;struct _tuple27*_T9CF;struct Cyc_Yystacktype*_T9D0;union Cyc_YYSTYPE*_T9D1;union Cyc_YYSTYPE*_T9D2;struct Cyc_Yystacktype*_T9D3;union Cyc_YYSTYPE*_T9D4;union Cyc_YYSTYPE*_T9D5;struct Cyc_Absyn_VarargInfo*_T9D6;struct Cyc_Absyn_VarargInfo*_T9D7;struct Cyc_Absyn_VarargInfo*_T9D8;struct Cyc_Absyn_VarargInfo*_T9D9;struct Cyc_Yystacktype*_T9DA;union Cyc_YYSTYPE*_T9DB;union Cyc_YYSTYPE*_T9DC;struct Cyc_Yystacktype*_T9DD;union Cyc_YYSTYPE*_T9DE;union Cyc_YYSTYPE*_T9DF;struct _tuple28 _T9E0;struct _tuple28*_T9E1;unsigned _T9E2;struct _tuple28*_T9E3;struct _tuple28 _T9E4;struct _tuple27*_T9E5;struct Cyc_Yystacktype*_T9E6;union Cyc_YYSTYPE*_T9E7;union Cyc_YYSTYPE*_T9E8;struct Cyc_List_List*_T9E9;struct Cyc_Yystacktype*_T9EA;union Cyc_YYSTYPE*_T9EB;union Cyc_YYSTYPE*_T9EC;struct Cyc_Yystacktype*_T9ED;struct Cyc_Yystacktype _T9EE;struct Cyc_Yyltype _T9EF;unsigned _T9F0;unsigned _T9F1;struct Cyc_Yystacktype*_T9F2;union Cyc_YYSTYPE*_T9F3;union Cyc_YYSTYPE*_T9F4;struct _fat_ptr _T9F5;void*_T9F6;struct Cyc_Yystacktype*_T9F7;union Cyc_YYSTYPE*_T9F8;union Cyc_YYSTYPE*_T9F9;struct _fat_ptr _T9FA;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T9FB;void*_T9FC;struct Cyc_Yystacktype*_T9FD;union Cyc_YYSTYPE*_T9FE;union Cyc_YYSTYPE*_T9FF;void*_TA00;struct Cyc_Yystacktype*_TA01;struct Cyc_Yystacktype _TA02;struct Cyc_Yyltype _TA03;unsigned _TA04;unsigned _TA05;void*_TA06;struct Cyc_Yystacktype*_TA07;union Cyc_YYSTYPE*_TA08;union Cyc_YYSTYPE*_TA09;struct _fat_ptr _TA0A;struct Cyc_Yystacktype*_TA0B;union Cyc_YYSTYPE*_TA0C;union Cyc_YYSTYPE*_TA0D;struct Cyc_Absyn_Kind*_TA0E;void*_TA0F;struct Cyc_Yystacktype*_TA10;union Cyc_YYSTYPE*_TA11;union Cyc_YYSTYPE*_TA12;void*_TA13;struct Cyc_Yystacktype*_TA14;struct Cyc_Yystacktype _TA15;struct Cyc_Yyltype _TA16;unsigned _TA17;unsigned _TA18;void*_TA19;struct Cyc_Yystacktype*_TA1A;union Cyc_YYSTYPE*_TA1B;union Cyc_YYSTYPE*_TA1C;struct Cyc_List_List*_TA1D;void*_TA1E;struct Cyc_Yystacktype*_TA1F;struct Cyc_Yystacktype _TA20;struct _tuple28*_TA21;struct Cyc_List_List*_TA22;struct Cyc_Yystacktype*_TA23;union Cyc_YYSTYPE*_TA24;union Cyc_YYSTYPE*_TA25;struct _fat_ptr _TA26;struct _tuple28*_TA27;struct Cyc_List_List*_TA28;struct Cyc_Yystacktype*_TA29;union Cyc_YYSTYPE*_TA2A;union Cyc_YYSTYPE*_TA2B;struct _fat_ptr _TA2C;struct Cyc_Yystacktype*_TA2D;union Cyc_YYSTYPE*_TA2E;union Cyc_YYSTYPE*_TA2F;struct _tuple28*_TA30;struct _tuple28*_TA31;struct Cyc_List_List*_TA32;struct Cyc_Yystacktype*_TA33;union Cyc_YYSTYPE*_TA34;union Cyc_YYSTYPE*_TA35;struct Cyc_Yystacktype*_TA36;union Cyc_YYSTYPE*_TA37;union Cyc_YYSTYPE*_TA38;struct _tuple28*_TA39;struct _tuple28*_TA3A;struct Cyc_List_List*_TA3B;struct Cyc_Yystacktype*_TA3C;union Cyc_YYSTYPE*_TA3D;union Cyc_YYSTYPE*_TA3E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA3F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA40;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA41;struct Cyc_Absyn_Kind*_TA42;struct Cyc_Yystacktype*_TA43;union Cyc_YYSTYPE*_TA44;union Cyc_YYSTYPE*_TA45;struct _fat_ptr _TA46;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA47;void*_TA48;struct Cyc_Yystacktype*_TA49;struct Cyc_Yystacktype _TA4A;struct Cyc_Yyltype _TA4B;unsigned _TA4C;unsigned _TA4D;struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_TA4E;struct Cyc_Yystacktype*_TA4F;union Cyc_YYSTYPE*_TA50;union Cyc_YYSTYPE*_TA51;struct Cyc_List_List*_TA52;void*_TA53;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_TA54;struct Cyc_Yystacktype*_TA55;union Cyc_YYSTYPE*_TA56;union Cyc_YYSTYPE*_TA57;struct Cyc_List_List*_TA58;struct Cyc_Yystacktype*_TA59;union Cyc_YYSTYPE*_TA5A;union Cyc_YYSTYPE*_TA5B;struct Cyc_List_List*_TA5C;void*_TA5D;struct Cyc_Yystacktype*_TA5E;struct Cyc_Yystacktype _TA5F;struct Cyc_Yyltype _TA60;unsigned _TA61;unsigned _TA62;struct Cyc_Yystacktype*_TA63;union Cyc_YYSTYPE*_TA64;union Cyc_YYSTYPE*_TA65;struct _fat_ptr _TA66;struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_TA67;struct Cyc_Yystacktype*_TA68;union Cyc_YYSTYPE*_TA69;union Cyc_YYSTYPE*_TA6A;struct Cyc_List_List*_TA6B;void*_TA6C;struct _tuple29*_TA6D;struct Cyc_Yystacktype*_TA6E;union Cyc_YYSTYPE*_TA6F;union Cyc_YYSTYPE*_TA70;struct Cyc_Yystacktype*_TA71;union Cyc_YYSTYPE*_TA72;union Cyc_YYSTYPE*_TA73;struct Cyc_Yystacktype*_TA74;struct Cyc_Yystacktype _TA75;struct Cyc_Yyltype _TA76;unsigned _TA77;unsigned _TA78;struct Cyc_Yystacktype*_TA79;union Cyc_YYSTYPE*_TA7A;union Cyc_YYSTYPE*_TA7B;struct _fat_ptr _TA7C;void*_TA7D;struct Cyc_Yystacktype*_TA7E;union Cyc_YYSTYPE*_TA7F;union Cyc_YYSTYPE*_TA80;void*_TA81;struct Cyc_Yystacktype*_TA82;union Cyc_YYSTYPE*_TA83;union Cyc_YYSTYPE*_TA84;void*_TA85;void*_TA86;void*_TA87;void*_TA88;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA89;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA8A;struct Cyc_Absyn_Kind*_TA8B;struct Cyc_Yystacktype*_TA8C;union Cyc_YYSTYPE*_TA8D;union Cyc_YYSTYPE*_TA8E;struct _fat_ptr _TA8F;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA90;void*_TA91;struct Cyc_Yystacktype*_TA92;struct Cyc_Yystacktype _TA93;struct Cyc_Yyltype _TA94;unsigned _TA95;unsigned _TA96;void*_TA97;struct Cyc_Yystacktype*_TA98;union Cyc_YYSTYPE*_TA99;union Cyc_YYSTYPE*_TA9A;void*_TA9B;void*_TA9C;struct Cyc_Yystacktype*_TA9D;union Cyc_YYSTYPE*_TA9E;union Cyc_YYSTYPE*_TA9F;struct _fat_ptr _TAA0;struct _fat_ptr _TAA1;int _TAA2;struct Cyc_Warn_String_Warn_Warg_struct _TAA3;struct Cyc_Yystacktype*_TAA4;struct Cyc_Yystacktype _TAA5;struct Cyc_Yyltype _TAA6;unsigned _TAA7;unsigned _TAA8;struct _fat_ptr _TAA9;struct Cyc_Yystacktype*_TAAA;struct Cyc_Yystacktype _TAAB;struct Cyc_Yystacktype*_TAAC;union Cyc_YYSTYPE*_TAAD;union Cyc_YYSTYPE*_TAAE;struct Cyc_List_List*_TAAF;struct Cyc_Yystacktype*_TAB0;union Cyc_YYSTYPE*_TAB1;union Cyc_YYSTYPE*_TAB2;struct Cyc_List_List*_TAB3;struct Cyc_List_List*_TAB4;struct Cyc_Yystacktype*_TAB5;struct Cyc_Yystacktype _TAB6;struct Cyc_List_List*_TAB7;struct Cyc_Yystacktype*_TAB8;union Cyc_YYSTYPE*_TAB9;union Cyc_YYSTYPE*_TABA;void*_TABB;struct Cyc_Yystacktype*_TABC;union Cyc_YYSTYPE*_TABD;union Cyc_YYSTYPE*_TABE;void*_TABF;struct Cyc_Absyn_Kind*_TAC0;struct Cyc_Absyn_Kind*_TAC1;struct Cyc_List_List*_TAC2;struct Cyc_Yystacktype*_TAC3;union Cyc_YYSTYPE*_TAC4;union Cyc_YYSTYPE*_TAC5;struct Cyc_List_List*_TAC6;struct Cyc_Yystacktype*_TAC7;union Cyc_YYSTYPE*_TAC8;union Cyc_YYSTYPE*_TAC9;struct _tuple8*_TACA;struct Cyc_Yystacktype*_TACB;struct Cyc_Yystacktype _TACC;struct Cyc_Yyltype _TACD;unsigned _TACE;unsigned _TACF;struct Cyc_List_List*_TAD0;struct Cyc_Yystacktype*_TAD1;union Cyc_YYSTYPE*_TAD2;union Cyc_YYSTYPE*_TAD3;struct _tuple8*_TAD4;struct Cyc_Yystacktype*_TAD5;struct Cyc_Yystacktype _TAD6;struct Cyc_Yyltype _TAD7;unsigned _TAD8;unsigned _TAD9;struct Cyc_Yystacktype*_TADA;union Cyc_YYSTYPE*_TADB;union Cyc_YYSTYPE*_TADC;struct Cyc_List_List*_TADD;struct Cyc_Yystacktype*_TADE;union Cyc_YYSTYPE*_TADF;union Cyc_YYSTYPE*_TAE0;struct Cyc_List_List*_TAE1;struct Cyc_Yystacktype*_TAE2;union Cyc_YYSTYPE*_TAE3;union Cyc_YYSTYPE*_TAE4;struct Cyc_Yystacktype*_TAE5;union Cyc_YYSTYPE*_TAE6;union Cyc_YYSTYPE*_TAE7;struct Cyc_Yystacktype*_TAE8;union Cyc_YYSTYPE*_TAE9;union Cyc_YYSTYPE*_TAEA;struct Cyc_Absyn_Tqual _TAEB;unsigned _TAEC;struct Cyc_Yystacktype*_TAED;struct Cyc_Yystacktype _TAEE;struct Cyc_Yyltype _TAEF;unsigned _TAF0;struct Cyc_Yystacktype*_TAF1;union Cyc_YYSTYPE*_TAF2;union Cyc_YYSTYPE*_TAF3;struct Cyc_Parse_Type_specifier _TAF4;struct Cyc_Yystacktype*_TAF5;struct Cyc_Yystacktype _TAF6;struct Cyc_Yyltype _TAF7;unsigned _TAF8;unsigned _TAF9;struct Cyc_Warn_String_Warn_Warg_struct _TAFA;struct Cyc_Yystacktype*_TAFB;struct Cyc_Yystacktype _TAFC;struct Cyc_Yyltype _TAFD;unsigned _TAFE;unsigned _TAFF;struct _fat_ptr _TB00;int _TB01;struct Cyc_Warn_String_Warn_Warg_struct _TB02;struct Cyc_Yystacktype*_TB03;struct Cyc_Yystacktype _TB04;struct Cyc_Yyltype _TB05;unsigned _TB06;unsigned _TB07;struct _fat_ptr _TB08;struct _tuple0*_TB09;struct _tuple0 _TB0A;struct Cyc_Warn_String_Warn_Warg_struct _TB0B;struct Cyc_Yystacktype*_TB0C;struct Cyc_Yystacktype _TB0D;struct Cyc_Yyltype _TB0E;unsigned _TB0F;unsigned _TB10;struct _fat_ptr _TB11;struct _tuple8*_TB12;struct Cyc_Yystacktype*_TB13;union Cyc_YYSTYPE*_TB14;union Cyc_YYSTYPE*_TB15;struct Cyc_Absyn_Tqual _TB16;unsigned _TB17;struct Cyc_Yystacktype*_TB18;struct Cyc_Yystacktype _TB19;struct Cyc_Yyltype _TB1A;unsigned _TB1B;struct Cyc_Parse_Type_specifier _TB1C;struct Cyc_Yystacktype*_TB1D;struct Cyc_Yystacktype _TB1E;struct Cyc_Yyltype _TB1F;unsigned _TB20;unsigned _TB21;struct Cyc_Warn_String_Warn_Warg_struct _TB22;struct Cyc_Yystacktype*_TB23;struct Cyc_Yystacktype _TB24;struct Cyc_Yyltype _TB25;unsigned _TB26;unsigned _TB27;struct _fat_ptr _TB28;struct _tuple8*_TB29;struct Cyc_Yystacktype*_TB2A;union Cyc_YYSTYPE*_TB2B;union Cyc_YYSTYPE*_TB2C;struct Cyc_Absyn_Tqual _TB2D;unsigned _TB2E;struct Cyc_Yystacktype*_TB2F;struct Cyc_Yystacktype _TB30;struct Cyc_Yyltype _TB31;unsigned _TB32;struct Cyc_Parse_Type_specifier _TB33;struct Cyc_Yystacktype*_TB34;struct Cyc_Yystacktype _TB35;struct Cyc_Yyltype _TB36;unsigned _TB37;unsigned _TB38;struct Cyc_Yystacktype*_TB39;union Cyc_YYSTYPE*_TB3A;union Cyc_YYSTYPE*_TB3B;struct Cyc_Parse_Abstractdeclarator _TB3C;struct Cyc_Warn_String_Warn_Warg_struct _TB3D;struct Cyc_Yystacktype*_TB3E;struct Cyc_Yystacktype _TB3F;struct Cyc_Yyltype _TB40;unsigned _TB41;unsigned _TB42;struct _fat_ptr _TB43;struct Cyc_Warn_String_Warn_Warg_struct _TB44;struct Cyc_Yystacktype*_TB45;struct Cyc_Yystacktype _TB46;struct Cyc_Yyltype _TB47;unsigned _TB48;unsigned _TB49;struct _fat_ptr _TB4A;struct _tuple8*_TB4B;struct Cyc_Yystacktype*_TB4C;union Cyc_YYSTYPE*_TB4D;union Cyc_YYSTYPE*_TB4E;struct Cyc_List_List*_TB4F;struct Cyc_List_List*_TB50;struct Cyc_List_List*_TB51;struct _fat_ptr*_TB52;struct Cyc_Yystacktype*_TB53;union Cyc_YYSTYPE*_TB54;union Cyc_YYSTYPE*_TB55;struct Cyc_List_List*_TB56;struct _fat_ptr*_TB57;struct Cyc_Yystacktype*_TB58;union Cyc_YYSTYPE*_TB59;union Cyc_YYSTYPE*_TB5A;struct Cyc_Yystacktype*_TB5B;union Cyc_YYSTYPE*_TB5C;union Cyc_YYSTYPE*_TB5D;struct Cyc_Yystacktype*_TB5E;struct Cyc_Yystacktype _TB5F;struct Cyc_Yystacktype*_TB60;struct Cyc_Yystacktype _TB61;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB62;void*_TB63;struct Cyc_Yystacktype*_TB64;struct Cyc_Yystacktype _TB65;struct Cyc_Yyltype _TB66;unsigned _TB67;unsigned _TB68;struct Cyc_Absyn_Exp*_TB69;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB6A;struct Cyc_Yystacktype*_TB6B;union Cyc_YYSTYPE*_TB6C;union Cyc_YYSTYPE*_TB6D;struct Cyc_List_List*_TB6E;void*_TB6F;struct Cyc_Yystacktype*_TB70;struct Cyc_Yystacktype _TB71;struct Cyc_Yyltype _TB72;unsigned _TB73;unsigned _TB74;struct Cyc_Absyn_Exp*_TB75;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB76;struct Cyc_Yystacktype*_TB77;union Cyc_YYSTYPE*_TB78;union Cyc_YYSTYPE*_TB79;struct Cyc_List_List*_TB7A;void*_TB7B;struct Cyc_Yystacktype*_TB7C;struct Cyc_Yystacktype _TB7D;struct Cyc_Yyltype _TB7E;unsigned _TB7F;unsigned _TB80;struct Cyc_Absyn_Exp*_TB81;struct Cyc_Yystacktype*_TB82;struct Cyc_Yystacktype _TB83;struct Cyc_Yyltype _TB84;unsigned _TB85;unsigned _TB86;struct _tuple0*_TB87;struct _fat_ptr*_TB88;struct Cyc_Yystacktype*_TB89;union Cyc_YYSTYPE*_TB8A;union Cyc_YYSTYPE*_TB8B;void*_TB8C;struct Cyc_Yystacktype*_TB8D;struct Cyc_Yystacktype _TB8E;struct Cyc_Yyltype _TB8F;unsigned _TB90;unsigned _TB91;struct Cyc_Absyn_Exp*_TB92;struct Cyc_Absyn_Vardecl*_TB93;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TB94;struct Cyc_Yystacktype*_TB95;union Cyc_YYSTYPE*_TB96;union Cyc_YYSTYPE*_TB97;struct Cyc_Yystacktype*_TB98;union Cyc_YYSTYPE*_TB99;union Cyc_YYSTYPE*_TB9A;void*_TB9B;struct Cyc_Yystacktype*_TB9C;struct Cyc_Yystacktype _TB9D;struct Cyc_Yyltype _TB9E;unsigned _TB9F;unsigned _TBA0;struct Cyc_Absyn_Exp*_TBA1;struct Cyc_Yystacktype*_TBA2;union Cyc_YYSTYPE*_TBA3;union Cyc_YYSTYPE*_TBA4;struct _tuple8*_TBA5;struct Cyc_Yystacktype*_TBA6;struct Cyc_Yystacktype _TBA7;struct Cyc_Yyltype _TBA8;unsigned _TBA9;unsigned _TBAA;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_TBAB;struct Cyc_Yystacktype*_TBAC;union Cyc_YYSTYPE*_TBAD;union Cyc_YYSTYPE*_TBAE;void*_TBAF;struct Cyc_Yystacktype*_TBB0;struct Cyc_Yystacktype _TBB1;struct Cyc_Yyltype _TBB2;unsigned _TBB3;unsigned _TBB4;struct Cyc_Absyn_Exp*_TBB5;struct Cyc_List_List*_TBB6;struct _tuple34*_TBB7;struct Cyc_Yystacktype*_TBB8;union Cyc_YYSTYPE*_TBB9;union Cyc_YYSTYPE*_TBBA;struct Cyc_List_List*_TBBB;struct _tuple34*_TBBC;struct Cyc_Yystacktype*_TBBD;union Cyc_YYSTYPE*_TBBE;union Cyc_YYSTYPE*_TBBF;struct Cyc_Yystacktype*_TBC0;union Cyc_YYSTYPE*_TBC1;union Cyc_YYSTYPE*_TBC2;struct Cyc_List_List*_TBC3;struct _tuple34*_TBC4;struct Cyc_Yystacktype*_TBC5;union Cyc_YYSTYPE*_TBC6;union Cyc_YYSTYPE*_TBC7;struct Cyc_Yystacktype*_TBC8;union Cyc_YYSTYPE*_TBC9;union Cyc_YYSTYPE*_TBCA;struct Cyc_List_List*_TBCB;struct _tuple34*_TBCC;struct Cyc_Yystacktype*_TBCD;union Cyc_YYSTYPE*_TBCE;union Cyc_YYSTYPE*_TBCF;struct Cyc_Yystacktype*_TBD0;union Cyc_YYSTYPE*_TBD1;union Cyc_YYSTYPE*_TBD2;struct Cyc_Yystacktype*_TBD3;union Cyc_YYSTYPE*_TBD4;union Cyc_YYSTYPE*_TBD5;struct Cyc_Yystacktype*_TBD6;union Cyc_YYSTYPE*_TBD7;union Cyc_YYSTYPE*_TBD8;struct Cyc_List_List*_TBD9;struct Cyc_List_List*_TBDA;struct Cyc_List_List*_TBDB;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBDC;struct _fat_ptr*_TBDD;struct Cyc_Yystacktype*_TBDE;union Cyc_YYSTYPE*_TBDF;union Cyc_YYSTYPE*_TBE0;struct Cyc_List_List*_TBE1;struct Cyc_Yystacktype*_TBE2;union Cyc_YYSTYPE*_TBE3;union Cyc_YYSTYPE*_TBE4;struct Cyc_List_List*_TBE5;struct Cyc_Yystacktype*_TBE6;union Cyc_YYSTYPE*_TBE7;union Cyc_YYSTYPE*_TBE8;struct Cyc_Yystacktype*_TBE9;union Cyc_YYSTYPE*_TBEA;union Cyc_YYSTYPE*_TBEB;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_TBEC;struct Cyc_Yystacktype*_TBED;union Cyc_YYSTYPE*_TBEE;union Cyc_YYSTYPE*_TBEF;void*_TBF0;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBF1;struct _fat_ptr*_TBF2;struct Cyc_Yystacktype*_TBF3;union Cyc_YYSTYPE*_TBF4;union Cyc_YYSTYPE*_TBF5;void*_TBF6;struct Cyc_Yystacktype*_TBF7;union Cyc_YYSTYPE*_TBF8;union Cyc_YYSTYPE*_TBF9;struct Cyc_Parse_Type_specifier _TBFA;struct Cyc_Yystacktype*_TBFB;struct Cyc_Yystacktype _TBFC;struct Cyc_Yyltype _TBFD;unsigned _TBFE;unsigned _TBFF;struct Cyc_Yystacktype*_TC00;struct Cyc_Yystacktype _TC01;struct Cyc_Yyltype _TC02;unsigned _TC03;unsigned _TC04;struct _fat_ptr _TC05;struct _fat_ptr _TC06;struct _tuple8*_TC07;struct Cyc_Yystacktype*_TC08;union Cyc_YYSTYPE*_TC09;union Cyc_YYSTYPE*_TC0A;struct Cyc_Parse_Type_specifier _TC0B;struct Cyc_Yystacktype*_TC0C;struct Cyc_Yystacktype _TC0D;struct Cyc_Yyltype _TC0E;unsigned _TC0F;unsigned _TC10;struct Cyc_Yystacktype*_TC11;union Cyc_YYSTYPE*_TC12;union Cyc_YYSTYPE*_TC13;struct Cyc_Parse_Abstractdeclarator _TC14;struct _tuple14 _TC15;struct Cyc_List_List*_TC16;struct Cyc_Yystacktype*_TC17;struct Cyc_Yystacktype _TC18;struct Cyc_Yyltype _TC19;unsigned _TC1A;unsigned _TC1B;struct _fat_ptr _TC1C;struct _fat_ptr _TC1D;struct _tuple14 _TC1E;struct Cyc_List_List*_TC1F;struct Cyc_Yystacktype*_TC20;struct Cyc_Yystacktype _TC21;struct Cyc_Yyltype _TC22;unsigned _TC23;unsigned _TC24;struct _fat_ptr _TC25;struct _fat_ptr _TC26;struct _tuple8*_TC27;struct _tuple14 _TC28;struct _tuple14 _TC29;struct Cyc_Yystacktype*_TC2A;union Cyc_YYSTYPE*_TC2B;union Cyc_YYSTYPE*_TC2C;struct _tuple8*_TC2D;struct Cyc_Yystacktype*_TC2E;struct Cyc_Yystacktype _TC2F;struct Cyc_Yyltype _TC30;unsigned _TC31;unsigned _TC32;void*_TC33;void*_TC34;struct Cyc_Yystacktype*_TC35;union Cyc_YYSTYPE*_TC36;union Cyc_YYSTYPE*_TC37;struct Cyc_List_List*_TC38;void*_TC39;struct Cyc_Yystacktype*_TC3A;union Cyc_YYSTYPE*_TC3B;union Cyc_YYSTYPE*_TC3C;void*_TC3D;void*_TC3E;struct Cyc_List_List*_TC3F;struct Cyc_Yystacktype*_TC40;union Cyc_YYSTYPE*_TC41;union Cyc_YYSTYPE*_TC42;struct Cyc_Yystacktype*_TC43;union Cyc_YYSTYPE*_TC44;union Cyc_YYSTYPE*_TC45;void*_TC46;struct Cyc_Yystacktype*_TC47;union Cyc_YYSTYPE*_TC48;union Cyc_YYSTYPE*_TC49;void*_TC4A;struct Cyc_List_List*_TC4B;struct Cyc_Yystacktype*_TC4C;union Cyc_YYSTYPE*_TC4D;union Cyc_YYSTYPE*_TC4E;struct Cyc_List_List*_TC4F;struct Cyc_Yystacktype*_TC50;union Cyc_YYSTYPE*_TC51;union Cyc_YYSTYPE*_TC52;struct Cyc_Yystacktype*_TC53;union Cyc_YYSTYPE*_TC54;union Cyc_YYSTYPE*_TC55;struct Cyc_Parse_Abstractdeclarator _TC56;struct Cyc_Yystacktype*_TC57;union Cyc_YYSTYPE*_TC58;union Cyc_YYSTYPE*_TC59;struct Cyc_Yystacktype*_TC5A;struct Cyc_Yystacktype _TC5B;struct Cyc_Parse_Abstractdeclarator _TC5C;struct Cyc_Yystacktype*_TC5D;union Cyc_YYSTYPE*_TC5E;union Cyc_YYSTYPE*_TC5F;struct Cyc_List_List*_TC60;struct Cyc_Yystacktype*_TC61;union Cyc_YYSTYPE*_TC62;union Cyc_YYSTYPE*_TC63;struct Cyc_Parse_Abstractdeclarator _TC64;struct Cyc_List_List*_TC65;struct Cyc_Yystacktype*_TC66;struct Cyc_Yystacktype _TC67;struct Cyc_Parse_Abstractdeclarator _TC68;struct Cyc_List_List*_TC69;struct _RegionHandle*_TC6A;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TC6B;struct _RegionHandle*_TC6C;struct Cyc_Yystacktype*_TC6D;union Cyc_YYSTYPE*_TC6E;union Cyc_YYSTYPE*_TC6F;struct Cyc_Yystacktype*_TC70;struct Cyc_Yystacktype _TC71;struct Cyc_Yyltype _TC72;unsigned _TC73;struct Cyc_Parse_Abstractdeclarator _TC74;struct Cyc_List_List*_TC75;struct _RegionHandle*_TC76;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TC77;struct _RegionHandle*_TC78;struct Cyc_Yystacktype*_TC79;union Cyc_YYSTYPE*_TC7A;union Cyc_YYSTYPE*_TC7B;struct Cyc_Yystacktype*_TC7C;struct Cyc_Yystacktype _TC7D;struct Cyc_Yyltype _TC7E;unsigned _TC7F;struct Cyc_Yystacktype*_TC80;union Cyc_YYSTYPE*_TC81;union Cyc_YYSTYPE*_TC82;struct Cyc_Parse_Abstractdeclarator _TC83;struct Cyc_Parse_Abstractdeclarator _TC84;struct Cyc_List_List*_TC85;struct _RegionHandle*_TC86;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TC87;struct _RegionHandle*_TC88;struct Cyc_Yystacktype*_TC89;union Cyc_YYSTYPE*_TC8A;union Cyc_YYSTYPE*_TC8B;struct Cyc_Yystacktype*_TC8C;union Cyc_YYSTYPE*_TC8D;union Cyc_YYSTYPE*_TC8E;struct Cyc_Yystacktype*_TC8F;struct Cyc_Yystacktype _TC90;struct Cyc_Yyltype _TC91;unsigned _TC92;struct Cyc_Parse_Abstractdeclarator _TC93;struct Cyc_List_List*_TC94;struct _RegionHandle*_TC95;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TC96;struct _RegionHandle*_TC97;struct Cyc_Yystacktype*_TC98;union Cyc_YYSTYPE*_TC99;union Cyc_YYSTYPE*_TC9A;struct Cyc_Yystacktype*_TC9B;union Cyc_YYSTYPE*_TC9C;union Cyc_YYSTYPE*_TC9D;struct Cyc_Yystacktype*_TC9E;struct Cyc_Yystacktype _TC9F;struct Cyc_Yyltype _TCA0;unsigned _TCA1;struct Cyc_Yystacktype*_TCA2;union Cyc_YYSTYPE*_TCA3;union Cyc_YYSTYPE*_TCA4;struct Cyc_Parse_Abstractdeclarator _TCA5;struct Cyc_Yystacktype*_TCA6;union Cyc_YYSTYPE*_TCA7;union Cyc_YYSTYPE*_TCA8;struct Cyc_Yystacktype*_TCA9;union Cyc_YYSTYPE*_TCAA;union Cyc_YYSTYPE*_TCAB;struct Cyc_Parse_Abstractdeclarator _TCAC;struct Cyc_List_List*_TCAD;struct _RegionHandle*_TCAE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCAF;struct _RegionHandle*_TCB0;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCB1;struct _RegionHandle*_TCB2;struct Cyc_Yystacktype*_TCB3;union Cyc_YYSTYPE*_TCB4;union Cyc_YYSTYPE*_TCB5;struct Cyc_Yystacktype*_TCB6;union Cyc_YYSTYPE*_TCB7;union Cyc_YYSTYPE*_TCB8;struct Cyc_Parse_Abstractdeclarator _TCB9;struct Cyc_List_List*_TCBA;struct _RegionHandle*_TCBB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCBC;struct _RegionHandle*_TCBD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCBE;struct _RegionHandle*_TCBF;struct Cyc_Yystacktype*_TCC0;union Cyc_YYSTYPE*_TCC1;union Cyc_YYSTYPE*_TCC2;struct Cyc_Parse_Abstractdeclarator _TCC3;struct Cyc_List_List*(*_TCC4)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_TCC5)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_TCC6;struct Cyc_Yystacktype _TCC7;struct Cyc_Yyltype _TCC8;unsigned _TCC9;unsigned _TCCA;struct Cyc_Yystacktype*_TCCB;union Cyc_YYSTYPE*_TCCC;union Cyc_YYSTYPE*_TCCD;struct Cyc_List_List*_TCCE;struct Cyc_List_List*_TCCF;struct Cyc_Parse_Abstractdeclarator _TCD0;struct Cyc_List_List*_TCD1;struct _RegionHandle*_TCD2;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_TCD3;struct _RegionHandle*_TCD4;struct Cyc_Yystacktype*_TCD5;struct Cyc_Yystacktype _TCD6;struct Cyc_Yyltype _TCD7;unsigned _TCD8;struct Cyc_Yystacktype*_TCD9;union Cyc_YYSTYPE*_TCDA;union Cyc_YYSTYPE*_TCDB;struct Cyc_Parse_Abstractdeclarator _TCDC;struct Cyc_Parse_Abstractdeclarator _TCDD;struct Cyc_List_List*_TCDE;struct _RegionHandle*_TCDF;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_TCE0;struct _RegionHandle*_TCE1;struct Cyc_Yystacktype*_TCE2;struct Cyc_Yystacktype _TCE3;struct Cyc_Yyltype _TCE4;unsigned _TCE5;struct Cyc_Yystacktype*_TCE6;union Cyc_YYSTYPE*_TCE7;union Cyc_YYSTYPE*_TCE8;struct Cyc_Yystacktype*_TCE9;union Cyc_YYSTYPE*_TCEA;union Cyc_YYSTYPE*_TCEB;struct Cyc_Parse_Abstractdeclarator _TCEC;struct _tuple21 _TCED;struct Cyc_Yystacktype*_TCEE;union Cyc_YYSTYPE*_TCEF;union Cyc_YYSTYPE*_TCF0;struct _tuple21 _TCF1;struct Cyc_Yystacktype*_TCF2;union Cyc_YYSTYPE*_TCF3;union Cyc_YYSTYPE*_TCF4;struct _tuple21 _TCF5;struct Cyc_Yystacktype*_TCF6;union Cyc_YYSTYPE*_TCF7;union Cyc_YYSTYPE*_TCF8;struct _tuple21 _TCF9;struct Cyc_Yystacktype*_TCFA;union Cyc_YYSTYPE*_TCFB;union Cyc_YYSTYPE*_TCFC;struct _tuple21 _TCFD;struct Cyc_Yystacktype*_TCFE;union Cyc_YYSTYPE*_TCFF;union Cyc_YYSTYPE*_TD00;struct Cyc_Yystacktype*_TD01;union Cyc_YYSTYPE*_TD02;union Cyc_YYSTYPE*_TD03;struct _tuple21 _TD04;struct _tuple21 _TD05;struct _tuple21 _TD06;struct Cyc_Yystacktype*_TD07;union Cyc_YYSTYPE*_TD08;union Cyc_YYSTYPE*_TD09;struct Cyc_Yystacktype*_TD0A;union Cyc_YYSTYPE*_TD0B;union Cyc_YYSTYPE*_TD0C;struct _tuple21 _TD0D;struct _tuple21 _TD0E;struct _tuple21 _TD0F;struct Cyc_Yystacktype*_TD10;union Cyc_YYSTYPE*_TD11;union Cyc_YYSTYPE*_TD12;struct Cyc_Yystacktype*_TD13;union Cyc_YYSTYPE*_TD14;union Cyc_YYSTYPE*_TD15;struct _tuple21 _TD16;struct _tuple21 _TD17;struct _tuple21 _TD18;struct Cyc_Yystacktype*_TD19;union Cyc_YYSTYPE*_TD1A;union Cyc_YYSTYPE*_TD1B;struct Cyc_Yystacktype*_TD1C;union Cyc_YYSTYPE*_TD1D;union Cyc_YYSTYPE*_TD1E;struct _tuple21 _TD1F;struct _tuple21 _TD20;struct _tuple21 _TD21;struct Cyc_Yystacktype*_TD22;struct Cyc_Yystacktype _TD23;struct Cyc_Yystacktype*_TD24;struct Cyc_Yystacktype _TD25;struct Cyc_Yystacktype*_TD26;struct Cyc_Yystacktype _TD27;struct Cyc_Yystacktype*_TD28;struct Cyc_Yystacktype _TD29;struct Cyc_Yystacktype*_TD2A;struct Cyc_Yystacktype _TD2B;struct Cyc_Yystacktype*_TD2C;struct Cyc_Yystacktype _TD2D;struct Cyc_Yystacktype*_TD2E;struct Cyc_Yystacktype _TD2F;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD30;struct _fat_ptr*_TD31;struct Cyc_Yystacktype*_TD32;union Cyc_YYSTYPE*_TD33;union Cyc_YYSTYPE*_TD34;struct Cyc_Yystacktype*_TD35;union Cyc_YYSTYPE*_TD36;union Cyc_YYSTYPE*_TD37;void*_TD38;struct Cyc_Yystacktype*_TD39;struct Cyc_Yystacktype _TD3A;struct Cyc_Yyltype _TD3B;unsigned _TD3C;unsigned _TD3D;struct Cyc_Absyn_Stmt*_TD3E;struct Cyc_Yystacktype*_TD3F;struct Cyc_Yystacktype _TD40;struct Cyc_Yyltype _TD41;unsigned _TD42;unsigned _TD43;struct Cyc_Absyn_Stmt*_TD44;struct Cyc_Yystacktype*_TD45;union Cyc_YYSTYPE*_TD46;union Cyc_YYSTYPE*_TD47;struct Cyc_Absyn_Exp*_TD48;struct Cyc_Yystacktype*_TD49;struct Cyc_Yystacktype _TD4A;struct Cyc_Yyltype _TD4B;unsigned _TD4C;unsigned _TD4D;struct Cyc_Absyn_Stmt*_TD4E;struct Cyc_Yystacktype*_TD4F;struct Cyc_Yystacktype _TD50;struct Cyc_Yyltype _TD51;unsigned _TD52;unsigned _TD53;struct Cyc_Absyn_Stmt*_TD54;struct Cyc_Yystacktype*_TD55;struct Cyc_Yystacktype _TD56;struct Cyc_Yystacktype*_TD57;struct Cyc_Yystacktype _TD58;struct Cyc_Yyltype _TD59;unsigned _TD5A;unsigned _TD5B;struct Cyc_Absyn_Stmt*_TD5C;struct Cyc_Yystacktype*_TD5D;struct Cyc_Yystacktype _TD5E;struct Cyc_Yystacktype*_TD5F;union Cyc_YYSTYPE*_TD60;union Cyc_YYSTYPE*_TD61;struct Cyc_List_List*_TD62;struct Cyc_Yystacktype*_TD63;struct Cyc_Yystacktype _TD64;struct Cyc_Yyltype _TD65;unsigned _TD66;unsigned _TD67;struct Cyc_Absyn_Stmt*_TD68;struct Cyc_Absyn_Stmt*_TD69;struct Cyc_Yystacktype*_TD6A;union Cyc_YYSTYPE*_TD6B;union Cyc_YYSTYPE*_TD6C;struct Cyc_List_List*_TD6D;struct Cyc_Yystacktype*_TD6E;union Cyc_YYSTYPE*_TD6F;union Cyc_YYSTYPE*_TD70;struct Cyc_Absyn_Stmt*_TD71;struct Cyc_Absyn_Stmt*_TD72;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD73;struct _fat_ptr*_TD74;struct Cyc_Yystacktype*_TD75;union Cyc_YYSTYPE*_TD76;union Cyc_YYSTYPE*_TD77;struct Cyc_Yystacktype*_TD78;union Cyc_YYSTYPE*_TD79;union Cyc_YYSTYPE*_TD7A;struct Cyc_List_List*_TD7B;struct Cyc_Absyn_Stmt*_TD7C;void*_TD7D;struct Cyc_Yystacktype*_TD7E;struct Cyc_Yystacktype _TD7F;struct Cyc_Yyltype _TD80;unsigned _TD81;unsigned _TD82;struct Cyc_Absyn_Stmt*_TD83;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD84;struct _fat_ptr*_TD85;struct Cyc_Yystacktype*_TD86;union Cyc_YYSTYPE*_TD87;union Cyc_YYSTYPE*_TD88;struct Cyc_Yystacktype*_TD89;union Cyc_YYSTYPE*_TD8A;union Cyc_YYSTYPE*_TD8B;struct Cyc_List_List*_TD8C;struct Cyc_Yystacktype*_TD8D;union Cyc_YYSTYPE*_TD8E;union Cyc_YYSTYPE*_TD8F;struct Cyc_Absyn_Stmt*_TD90;void*_TD91;struct Cyc_Yystacktype*_TD92;struct Cyc_Yystacktype _TD93;struct Cyc_Yyltype _TD94;unsigned _TD95;unsigned _TD96;struct Cyc_Absyn_Stmt*_TD97;struct Cyc_Yystacktype*_TD98;struct Cyc_Yystacktype _TD99;struct Cyc_Yystacktype*_TD9A;union Cyc_YYSTYPE*_TD9B;union Cyc_YYSTYPE*_TD9C;struct Cyc_Absyn_Stmt*_TD9D;struct Cyc_Yystacktype*_TD9E;union Cyc_YYSTYPE*_TD9F;union Cyc_YYSTYPE*_TDA0;struct Cyc_Absyn_Stmt*_TDA1;struct Cyc_Yystacktype*_TDA2;struct Cyc_Yystacktype _TDA3;struct Cyc_Yyltype _TDA4;unsigned _TDA5;unsigned _TDA6;struct Cyc_Absyn_Stmt*_TDA7;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDA8;struct Cyc_Yystacktype*_TDA9;union Cyc_YYSTYPE*_TDAA;union Cyc_YYSTYPE*_TDAB;void*_TDAC;struct Cyc_Yystacktype*_TDAD;struct Cyc_Yystacktype _TDAE;struct Cyc_Yyltype _TDAF;unsigned _TDB0;unsigned _TDB1;struct Cyc_Absyn_Decl*_TDB2;struct Cyc_Absyn_Stmt*_TDB3;struct Cyc_Absyn_Stmt*_TDB4;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDB5;struct Cyc_Yystacktype*_TDB6;union Cyc_YYSTYPE*_TDB7;union Cyc_YYSTYPE*_TDB8;void*_TDB9;struct Cyc_Yystacktype*_TDBA;struct Cyc_Yystacktype _TDBB;struct Cyc_Yyltype _TDBC;unsigned _TDBD;unsigned _TDBE;struct Cyc_Absyn_Decl*_TDBF;struct Cyc_Yystacktype*_TDC0;union Cyc_YYSTYPE*_TDC1;union Cyc_YYSTYPE*_TDC2;struct Cyc_Absyn_Stmt*_TDC3;struct Cyc_Absyn_Stmt*_TDC4;struct Cyc_Yystacktype*_TDC5;union Cyc_YYSTYPE*_TDC6;union Cyc_YYSTYPE*_TDC7;struct Cyc_Absyn_Exp*_TDC8;struct Cyc_Yystacktype*_TDC9;union Cyc_YYSTYPE*_TDCA;union Cyc_YYSTYPE*_TDCB;struct Cyc_Absyn_Stmt*_TDCC;struct Cyc_Absyn_Stmt*_TDCD;struct Cyc_Yystacktype*_TDCE;struct Cyc_Yystacktype _TDCF;struct Cyc_Yyltype _TDD0;unsigned _TDD1;unsigned _TDD2;struct Cyc_Absyn_Stmt*_TDD3;struct Cyc_Yystacktype*_TDD4;union Cyc_YYSTYPE*_TDD5;union Cyc_YYSTYPE*_TDD6;struct Cyc_Absyn_Exp*_TDD7;struct Cyc_Yystacktype*_TDD8;union Cyc_YYSTYPE*_TDD9;union Cyc_YYSTYPE*_TDDA;struct Cyc_Absyn_Stmt*_TDDB;struct Cyc_Yystacktype*_TDDC;union Cyc_YYSTYPE*_TDDD;union Cyc_YYSTYPE*_TDDE;struct Cyc_Absyn_Stmt*_TDDF;struct Cyc_Yystacktype*_TDE0;struct Cyc_Yystacktype _TDE1;struct Cyc_Yyltype _TDE2;unsigned _TDE3;unsigned _TDE4;struct Cyc_Absyn_Stmt*_TDE5;struct Cyc_Yystacktype*_TDE6;union Cyc_YYSTYPE*_TDE7;union Cyc_YYSTYPE*_TDE8;struct Cyc_Absyn_Exp*_TDE9;struct Cyc_Yystacktype*_TDEA;union Cyc_YYSTYPE*_TDEB;union Cyc_YYSTYPE*_TDEC;struct Cyc_List_List*_TDED;struct Cyc_Yystacktype*_TDEE;struct Cyc_Yystacktype _TDEF;struct Cyc_Yyltype _TDF0;unsigned _TDF1;unsigned _TDF2;struct Cyc_Absyn_Stmt*_TDF3;struct Cyc_Yystacktype*_TDF4;union Cyc_YYSTYPE*_TDF5;union Cyc_YYSTYPE*_TDF6;struct _tuple0*_TDF7;struct Cyc_Yystacktype*_TDF8;struct Cyc_Yystacktype _TDF9;struct Cyc_Yyltype _TDFA;unsigned _TDFB;unsigned _TDFC;struct Cyc_Absyn_Exp*_TDFD;struct Cyc_Yystacktype*_TDFE;union Cyc_YYSTYPE*_TDFF;union Cyc_YYSTYPE*_TE00;struct Cyc_List_List*_TE01;struct Cyc_Yystacktype*_TE02;struct Cyc_Yystacktype _TE03;struct Cyc_Yyltype _TE04;unsigned _TE05;unsigned _TE06;struct Cyc_Absyn_Stmt*_TE07;struct Cyc_Yystacktype*_TE08;union Cyc_YYSTYPE*_TE09;union Cyc_YYSTYPE*_TE0A;struct Cyc_List_List*_TE0B;struct Cyc_Yystacktype*_TE0C;struct Cyc_Yystacktype _TE0D;struct Cyc_Yyltype _TE0E;unsigned _TE0F;unsigned _TE10;struct Cyc_Absyn_Exp*_TE11;struct Cyc_Yystacktype*_TE12;union Cyc_YYSTYPE*_TE13;union Cyc_YYSTYPE*_TE14;struct Cyc_List_List*_TE15;struct Cyc_Yystacktype*_TE16;struct Cyc_Yystacktype _TE17;struct Cyc_Yyltype _TE18;unsigned _TE19;unsigned _TE1A;struct Cyc_Absyn_Stmt*_TE1B;struct Cyc_Yystacktype*_TE1C;union Cyc_YYSTYPE*_TE1D;union Cyc_YYSTYPE*_TE1E;struct Cyc_Absyn_Stmt*_TE1F;struct Cyc_Yystacktype*_TE20;union Cyc_YYSTYPE*_TE21;union Cyc_YYSTYPE*_TE22;struct Cyc_List_List*_TE23;struct Cyc_Yystacktype*_TE24;struct Cyc_Yystacktype _TE25;struct Cyc_Yyltype _TE26;unsigned _TE27;unsigned _TE28;struct Cyc_Absyn_Stmt*_TE29;struct Cyc_List_List*_TE2A;struct Cyc_Absyn_Switch_clause*_TE2B;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE2C;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE2D;void*_TE2E;struct Cyc_Yystacktype*_TE2F;struct Cyc_Yystacktype _TE30;struct Cyc_Yyltype _TE31;unsigned _TE32;unsigned _TE33;struct Cyc_Yystacktype*_TE34;union Cyc_YYSTYPE*_TE35;union Cyc_YYSTYPE*_TE36;struct Cyc_Yystacktype*_TE37;struct Cyc_Yystacktype _TE38;struct Cyc_Yyltype _TE39;unsigned _TE3A;struct Cyc_Yystacktype*_TE3B;union Cyc_YYSTYPE*_TE3C;union Cyc_YYSTYPE*_TE3D;struct Cyc_List_List*_TE3E;struct Cyc_Absyn_Switch_clause*_TE3F;struct Cyc_Yystacktype*_TE40;union Cyc_YYSTYPE*_TE41;union Cyc_YYSTYPE*_TE42;struct Cyc_Yystacktype*_TE43;struct Cyc_Yystacktype _TE44;struct Cyc_Yyltype _TE45;unsigned _TE46;unsigned _TE47;struct Cyc_Yystacktype*_TE48;struct Cyc_Yystacktype _TE49;struct Cyc_Yyltype _TE4A;unsigned _TE4B;struct Cyc_Yystacktype*_TE4C;union Cyc_YYSTYPE*_TE4D;union Cyc_YYSTYPE*_TE4E;struct Cyc_List_List*_TE4F;struct Cyc_Absyn_Switch_clause*_TE50;struct Cyc_Yystacktype*_TE51;union Cyc_YYSTYPE*_TE52;union Cyc_YYSTYPE*_TE53;struct Cyc_Yystacktype*_TE54;union Cyc_YYSTYPE*_TE55;union Cyc_YYSTYPE*_TE56;struct Cyc_Yystacktype*_TE57;struct Cyc_Yystacktype _TE58;struct Cyc_Yyltype _TE59;unsigned _TE5A;struct Cyc_Yystacktype*_TE5B;union Cyc_YYSTYPE*_TE5C;union Cyc_YYSTYPE*_TE5D;struct Cyc_List_List*_TE5E;struct Cyc_Absyn_Switch_clause*_TE5F;struct Cyc_Yystacktype*_TE60;union Cyc_YYSTYPE*_TE61;union Cyc_YYSTYPE*_TE62;struct Cyc_Yystacktype*_TE63;union Cyc_YYSTYPE*_TE64;union Cyc_YYSTYPE*_TE65;struct Cyc_Yystacktype*_TE66;struct Cyc_Yystacktype _TE67;struct Cyc_Yyltype _TE68;unsigned _TE69;unsigned _TE6A;struct Cyc_Yystacktype*_TE6B;struct Cyc_Yystacktype _TE6C;struct Cyc_Yyltype _TE6D;unsigned _TE6E;struct Cyc_Yystacktype*_TE6F;union Cyc_YYSTYPE*_TE70;union Cyc_YYSTYPE*_TE71;struct Cyc_List_List*_TE72;struct Cyc_Absyn_Switch_clause*_TE73;struct Cyc_Yystacktype*_TE74;union Cyc_YYSTYPE*_TE75;union Cyc_YYSTYPE*_TE76;struct Cyc_Yystacktype*_TE77;union Cyc_YYSTYPE*_TE78;union Cyc_YYSTYPE*_TE79;struct Cyc_Yystacktype*_TE7A;union Cyc_YYSTYPE*_TE7B;union Cyc_YYSTYPE*_TE7C;struct Cyc_Yystacktype*_TE7D;struct Cyc_Yystacktype _TE7E;struct Cyc_Yyltype _TE7F;unsigned _TE80;struct Cyc_Yystacktype*_TE81;union Cyc_YYSTYPE*_TE82;union Cyc_YYSTYPE*_TE83;struct Cyc_Yystacktype*_TE84;union Cyc_YYSTYPE*_TE85;union Cyc_YYSTYPE*_TE86;struct Cyc_Absyn_Exp*_TE87;struct Cyc_Yystacktype*_TE88;union Cyc_YYSTYPE*_TE89;union Cyc_YYSTYPE*_TE8A;struct Cyc_Absyn_Stmt*_TE8B;struct Cyc_Yystacktype*_TE8C;struct Cyc_Yystacktype _TE8D;struct Cyc_Yyltype _TE8E;unsigned _TE8F;unsigned _TE90;struct Cyc_Absyn_Stmt*_TE91;struct Cyc_Yystacktype*_TE92;union Cyc_YYSTYPE*_TE93;union Cyc_YYSTYPE*_TE94;struct Cyc_Absyn_Stmt*_TE95;struct Cyc_Yystacktype*_TE96;union Cyc_YYSTYPE*_TE97;union Cyc_YYSTYPE*_TE98;struct Cyc_Absyn_Exp*_TE99;struct Cyc_Yystacktype*_TE9A;struct Cyc_Yystacktype _TE9B;struct Cyc_Yyltype _TE9C;unsigned _TE9D;unsigned _TE9E;struct Cyc_Absyn_Stmt*_TE9F;struct Cyc_Yystacktype*_TEA0;union Cyc_YYSTYPE*_TEA1;union Cyc_YYSTYPE*_TEA2;struct Cyc_Absyn_Exp*_TEA3;struct Cyc_Yystacktype*_TEA4;union Cyc_YYSTYPE*_TEA5;union Cyc_YYSTYPE*_TEA6;struct Cyc_Absyn_Exp*_TEA7;struct Cyc_Yystacktype*_TEA8;union Cyc_YYSTYPE*_TEA9;union Cyc_YYSTYPE*_TEAA;struct Cyc_Absyn_Exp*_TEAB;struct Cyc_Yystacktype*_TEAC;union Cyc_YYSTYPE*_TEAD;union Cyc_YYSTYPE*_TEAE;struct Cyc_Absyn_Stmt*_TEAF;struct Cyc_Yystacktype*_TEB0;struct Cyc_Yystacktype _TEB1;struct Cyc_Yyltype _TEB2;unsigned _TEB3;unsigned _TEB4;struct Cyc_Absyn_Stmt*_TEB5;struct Cyc_Absyn_Exp*_TEB6;struct Cyc_Yystacktype*_TEB7;union Cyc_YYSTYPE*_TEB8;union Cyc_YYSTYPE*_TEB9;struct Cyc_Absyn_Exp*_TEBA;struct Cyc_Yystacktype*_TEBB;union Cyc_YYSTYPE*_TEBC;union Cyc_YYSTYPE*_TEBD;struct Cyc_Absyn_Exp*_TEBE;struct Cyc_Yystacktype*_TEBF;union Cyc_YYSTYPE*_TEC0;union Cyc_YYSTYPE*_TEC1;struct Cyc_Absyn_Stmt*_TEC2;struct Cyc_Yystacktype*_TEC3;struct Cyc_Yystacktype _TEC4;struct Cyc_Yyltype _TEC5;unsigned _TEC6;unsigned _TEC7;struct Cyc_Yystacktype*_TEC8;union Cyc_YYSTYPE*_TEC9;union Cyc_YYSTYPE*_TECA;struct Cyc_List_List*_TECB;struct Cyc_Absyn_Stmt*_TECC;struct Cyc_Absyn_Stmt*_TECD;struct Cyc_Absyn_Exp*_TECE;struct Cyc_Yystacktype*_TECF;struct Cyc_Yystacktype _TED0;struct _fat_ptr*_TED1;struct Cyc_Yystacktype*_TED2;union Cyc_YYSTYPE*_TED3;union Cyc_YYSTYPE*_TED4;struct Cyc_Yystacktype*_TED5;struct Cyc_Yystacktype _TED6;struct Cyc_Yyltype _TED7;unsigned _TED8;unsigned _TED9;struct Cyc_Absyn_Stmt*_TEDA;struct Cyc_Yystacktype*_TEDB;struct Cyc_Yystacktype _TEDC;struct Cyc_Yyltype _TEDD;unsigned _TEDE;unsigned _TEDF;struct Cyc_Absyn_Stmt*_TEE0;struct Cyc_Yystacktype*_TEE1;struct Cyc_Yystacktype _TEE2;struct Cyc_Yyltype _TEE3;unsigned _TEE4;unsigned _TEE5;struct Cyc_Absyn_Stmt*_TEE6;struct Cyc_Yystacktype*_TEE7;struct Cyc_Yystacktype _TEE8;struct Cyc_Yyltype _TEE9;unsigned _TEEA;unsigned _TEEB;struct Cyc_Absyn_Stmt*_TEEC;struct Cyc_Yystacktype*_TEED;union Cyc_YYSTYPE*_TEEE;union Cyc_YYSTYPE*_TEEF;struct Cyc_Absyn_Exp*_TEF0;struct Cyc_Yystacktype*_TEF1;struct Cyc_Yystacktype _TEF2;struct Cyc_Yyltype _TEF3;unsigned _TEF4;unsigned _TEF5;struct Cyc_Absyn_Stmt*_TEF6;struct Cyc_Yystacktype*_TEF7;struct Cyc_Yystacktype _TEF8;struct Cyc_Yyltype _TEF9;unsigned _TEFA;unsigned _TEFB;struct Cyc_Absyn_Stmt*_TEFC;struct Cyc_Yystacktype*_TEFD;struct Cyc_Yystacktype _TEFE;struct Cyc_Yyltype _TEFF;unsigned _TF00;unsigned _TF01;struct Cyc_Absyn_Stmt*_TF02;struct Cyc_Yystacktype*_TF03;union Cyc_YYSTYPE*_TF04;union Cyc_YYSTYPE*_TF05;struct Cyc_List_List*_TF06;struct Cyc_Yystacktype*_TF07;struct Cyc_Yystacktype _TF08;struct Cyc_Yyltype _TF09;unsigned _TF0A;unsigned _TF0B;struct Cyc_Absyn_Stmt*_TF0C;struct Cyc_Yystacktype*_TF0D;struct Cyc_Yystacktype _TF0E;struct Cyc_Yystacktype*_TF0F;struct Cyc_Yystacktype _TF10;struct Cyc_Yystacktype*_TF11;union Cyc_YYSTYPE*_TF12;union Cyc_YYSTYPE*_TF13;struct Cyc_Absyn_Pat*_TF14;struct Cyc_Absyn_Exp*_TF15;struct Cyc_Yystacktype*_TF16;union Cyc_YYSTYPE*_TF17;union Cyc_YYSTYPE*_TF18;struct Cyc_Absyn_Exp*_TF19;struct Cyc_Yystacktype*_TF1A;union Cyc_YYSTYPE*_TF1B;union Cyc_YYSTYPE*_TF1C;struct Cyc_Absyn_Exp*_TF1D;struct Cyc_Yystacktype*_TF1E;struct Cyc_Yystacktype _TF1F;struct Cyc_Yyltype _TF20;unsigned _TF21;unsigned _TF22;struct Cyc_Absyn_Exp*_TF23;struct Cyc_Absyn_Pat*_TF24;struct Cyc_Yystacktype*_TF25;struct Cyc_Yystacktype _TF26;struct Cyc_Yystacktype*_TF27;union Cyc_YYSTYPE*_TF28;union Cyc_YYSTYPE*_TF29;struct Cyc_Absyn_Pat*_TF2A;struct Cyc_Absyn_Exp*_TF2B;struct Cyc_Yystacktype*_TF2C;union Cyc_YYSTYPE*_TF2D;union Cyc_YYSTYPE*_TF2E;struct Cyc_Absyn_Exp*_TF2F;struct Cyc_Yystacktype*_TF30;struct Cyc_Yystacktype _TF31;struct Cyc_Yyltype _TF32;unsigned _TF33;unsigned _TF34;struct Cyc_Absyn_Exp*_TF35;struct Cyc_Absyn_Pat*_TF36;struct Cyc_Yystacktype*_TF37;struct Cyc_Yystacktype _TF38;struct Cyc_Yystacktype*_TF39;union Cyc_YYSTYPE*_TF3A;union Cyc_YYSTYPE*_TF3B;struct Cyc_Absyn_Pat*_TF3C;struct Cyc_Absyn_Exp*_TF3D;struct Cyc_Yystacktype*_TF3E;union Cyc_YYSTYPE*_TF3F;union Cyc_YYSTYPE*_TF40;struct Cyc_Absyn_Exp*_TF41;struct Cyc_Yystacktype*_TF42;struct Cyc_Yystacktype _TF43;struct Cyc_Yyltype _TF44;unsigned _TF45;unsigned _TF46;struct Cyc_Absyn_Exp*_TF47;struct Cyc_Absyn_Pat*_TF48;struct Cyc_Yystacktype*_TF49;struct Cyc_Yystacktype _TF4A;struct Cyc_Yystacktype*_TF4B;union Cyc_YYSTYPE*_TF4C;union Cyc_YYSTYPE*_TF4D;struct Cyc_Absyn_Pat*_TF4E;struct Cyc_Absyn_Exp*_TF4F;struct Cyc_Yystacktype*_TF50;union Cyc_YYSTYPE*_TF51;union Cyc_YYSTYPE*_TF52;struct Cyc_Absyn_Exp*_TF53;struct Cyc_Yystacktype*_TF54;struct Cyc_Yystacktype _TF55;struct Cyc_Yyltype _TF56;unsigned _TF57;unsigned _TF58;struct Cyc_Absyn_Exp*_TF59;struct Cyc_Absyn_Pat*_TF5A;struct Cyc_Yystacktype*_TF5B;struct Cyc_Yystacktype _TF5C;struct Cyc_Yystacktype*_TF5D;union Cyc_YYSTYPE*_TF5E;union Cyc_YYSTYPE*_TF5F;struct Cyc_Absyn_Pat*_TF60;struct Cyc_Absyn_Exp*_TF61;struct Cyc_Yystacktype*_TF62;union Cyc_YYSTYPE*_TF63;union Cyc_YYSTYPE*_TF64;struct Cyc_Absyn_Exp*_TF65;struct Cyc_Yystacktype*_TF66;struct Cyc_Yystacktype _TF67;struct Cyc_Yyltype _TF68;unsigned _TF69;unsigned _TF6A;struct Cyc_Absyn_Exp*_TF6B;struct Cyc_Absyn_Pat*_TF6C;struct Cyc_Yystacktype*_TF6D;struct Cyc_Yystacktype _TF6E;struct Cyc_Yystacktype*_TF6F;union Cyc_YYSTYPE*_TF70;union Cyc_YYSTYPE*_TF71;struct Cyc_Absyn_Pat*_TF72;struct Cyc_Absyn_Exp*_TF73;struct Cyc_Yystacktype*_TF74;union Cyc_YYSTYPE*_TF75;union Cyc_YYSTYPE*_TF76;struct Cyc_Absyn_Exp*_TF77;struct Cyc_Yystacktype*_TF78;struct Cyc_Yystacktype _TF79;struct Cyc_Yyltype _TF7A;unsigned _TF7B;unsigned _TF7C;struct Cyc_Absyn_Exp*_TF7D;struct Cyc_Absyn_Pat*_TF7E;struct Cyc_Yystacktype*_TF7F;struct Cyc_Yystacktype _TF80;struct Cyc_Yystacktype*_TF81;union Cyc_YYSTYPE*_TF82;union Cyc_YYSTYPE*_TF83;struct Cyc_Absyn_Exp*(*_TF84)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TF85;union Cyc_YYSTYPE*_TF86;union Cyc_YYSTYPE*_TF87;struct Cyc_Absyn_Pat*_TF88;struct Cyc_Absyn_Exp*_TF89;struct Cyc_Yystacktype*_TF8A;union Cyc_YYSTYPE*_TF8B;union Cyc_YYSTYPE*_TF8C;struct Cyc_Absyn_Exp*_TF8D;struct Cyc_Yystacktype*_TF8E;struct Cyc_Yystacktype _TF8F;struct Cyc_Yyltype _TF90;unsigned _TF91;unsigned _TF92;struct Cyc_Absyn_Exp*_TF93;struct Cyc_Absyn_Pat*_TF94;struct Cyc_Yystacktype*_TF95;struct Cyc_Yystacktype _TF96;struct Cyc_Yystacktype*_TF97;union Cyc_YYSTYPE*_TF98;union Cyc_YYSTYPE*_TF99;struct Cyc_Absyn_Exp*(*_TF9A)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TF9B;union Cyc_YYSTYPE*_TF9C;union Cyc_YYSTYPE*_TF9D;struct Cyc_Absyn_Pat*_TF9E;struct Cyc_Absyn_Exp*_TF9F;struct Cyc_Yystacktype*_TFA0;union Cyc_YYSTYPE*_TFA1;union Cyc_YYSTYPE*_TFA2;struct Cyc_Absyn_Exp*_TFA3;struct Cyc_Yystacktype*_TFA4;struct Cyc_Yystacktype _TFA5;struct Cyc_Yyltype _TFA6;unsigned _TFA7;unsigned _TFA8;struct Cyc_Absyn_Exp*_TFA9;struct Cyc_Absyn_Pat*_TFAA;struct Cyc_Yystacktype*_TFAB;struct Cyc_Yystacktype _TFAC;struct Cyc_Yystacktype*_TFAD;union Cyc_YYSTYPE*_TFAE;union Cyc_YYSTYPE*_TFAF;struct Cyc_Absyn_Pat*_TFB0;struct Cyc_Absyn_Exp*_TFB1;struct Cyc_Yystacktype*_TFB2;union Cyc_YYSTYPE*_TFB3;union Cyc_YYSTYPE*_TFB4;struct Cyc_Absyn_Exp*_TFB5;struct Cyc_Yystacktype*_TFB6;struct Cyc_Yystacktype _TFB7;struct Cyc_Yyltype _TFB8;unsigned _TFB9;unsigned _TFBA;struct Cyc_Absyn_Exp*_TFBB;struct Cyc_Absyn_Pat*_TFBC;struct Cyc_Yystacktype*_TFBD;union Cyc_YYSTYPE*_TFBE;union Cyc_YYSTYPE*_TFBF;struct Cyc_Absyn_Pat*_TFC0;struct Cyc_Absyn_Exp*_TFC1;struct Cyc_Yystacktype*_TFC2;union Cyc_YYSTYPE*_TFC3;union Cyc_YYSTYPE*_TFC4;struct Cyc_Absyn_Exp*_TFC5;struct Cyc_Yystacktype*_TFC6;struct Cyc_Yystacktype _TFC7;struct Cyc_Yyltype _TFC8;unsigned _TFC9;unsigned _TFCA;struct Cyc_Absyn_Exp*_TFCB;struct Cyc_Absyn_Pat*_TFCC;struct Cyc_Yystacktype*_TFCD;struct Cyc_Yystacktype _TFCE;struct Cyc_Yystacktype*_TFCF;union Cyc_YYSTYPE*_TFD0;union Cyc_YYSTYPE*_TFD1;enum Cyc_Absyn_Primop _TFD2;struct Cyc_Yystacktype*_TFD3;union Cyc_YYSTYPE*_TFD4;union Cyc_YYSTYPE*_TFD5;struct Cyc_Absyn_Pat*_TFD6;struct Cyc_Absyn_Exp*_TFD7;struct Cyc_Yystacktype*_TFD8;union Cyc_YYSTYPE*_TFD9;union Cyc_YYSTYPE*_TFDA;struct Cyc_Absyn_Exp*_TFDB;struct Cyc_Yystacktype*_TFDC;struct Cyc_Yystacktype _TFDD;struct Cyc_Yyltype _TFDE;unsigned _TFDF;unsigned _TFE0;struct Cyc_Absyn_Exp*_TFE1;struct Cyc_Absyn_Pat*_TFE2;struct Cyc_Yystacktype*_TFE3;struct Cyc_Yystacktype _TFE4;struct Cyc_Yystacktype*_TFE5;union Cyc_YYSTYPE*_TFE6;union Cyc_YYSTYPE*_TFE7;enum Cyc_Absyn_Primop _TFE8;struct Cyc_Yystacktype*_TFE9;union Cyc_YYSTYPE*_TFEA;union Cyc_YYSTYPE*_TFEB;struct Cyc_Absyn_Pat*_TFEC;struct Cyc_Absyn_Exp*_TFED;struct Cyc_Yystacktype*_TFEE;union Cyc_YYSTYPE*_TFEF;union Cyc_YYSTYPE*_TFF0;struct Cyc_Absyn_Exp*_TFF1;struct Cyc_Yystacktype*_TFF2;struct Cyc_Yystacktype _TFF3;struct Cyc_Yyltype _TFF4;unsigned _TFF5;unsigned _TFF6;struct Cyc_Absyn_Exp*_TFF7;struct Cyc_Absyn_Pat*_TFF8;struct Cyc_Yystacktype*_TFF9;struct Cyc_Yystacktype _TFFA;struct Cyc_Yystacktype*_TFFB;union Cyc_YYSTYPE*_TFFC;union Cyc_YYSTYPE*_TFFD;struct _tuple8*_TFFE;struct Cyc_Yystacktype*_TFFF;struct Cyc_Yystacktype _T1000;struct Cyc_Yyltype _T1001;unsigned _T1002;unsigned _T1003;void*_T1004;struct Cyc_Yystacktype*_T1005;union Cyc_YYSTYPE*_T1006;union Cyc_YYSTYPE*_T1007;struct Cyc_Absyn_Exp*_T1008;struct Cyc_Yystacktype*_T1009;struct Cyc_Yystacktype _T100A;struct Cyc_Yyltype _T100B;unsigned _T100C;unsigned _T100D;struct Cyc_Absyn_Exp*_T100E;struct Cyc_Absyn_Pat*_T100F;struct Cyc_Yystacktype*_T1010;struct Cyc_Yystacktype _T1011;struct Cyc_Yystacktype*_T1012;union Cyc_YYSTYPE*_T1013;union Cyc_YYSTYPE*_T1014;enum Cyc_Absyn_Primop _T1015;struct Cyc_Yystacktype*_T1016;union Cyc_YYSTYPE*_T1017;union Cyc_YYSTYPE*_T1018;struct Cyc_Absyn_Exp*_T1019;struct Cyc_Yystacktype*_T101A;struct Cyc_Yystacktype _T101B;struct Cyc_Yyltype _T101C;unsigned _T101D;unsigned _T101E;struct Cyc_Absyn_Exp*_T101F;struct Cyc_Absyn_Pat*_T1020;struct Cyc_Yystacktype*_T1021;union Cyc_YYSTYPE*_T1022;union Cyc_YYSTYPE*_T1023;struct _tuple8*_T1024;struct Cyc_Yystacktype*_T1025;struct Cyc_Yystacktype _T1026;struct Cyc_Yyltype _T1027;unsigned _T1028;unsigned _T1029;void*_T102A;struct Cyc_Yystacktype*_T102B;struct Cyc_Yystacktype _T102C;struct Cyc_Yyltype _T102D;unsigned _T102E;unsigned _T102F;struct Cyc_Absyn_Exp*_T1030;struct Cyc_Absyn_Pat*_T1031;struct Cyc_Yystacktype*_T1032;union Cyc_YYSTYPE*_T1033;union Cyc_YYSTYPE*_T1034;struct Cyc_Absyn_Exp*_T1035;struct Cyc_Yystacktype*_T1036;struct Cyc_Yystacktype _T1037;struct Cyc_Yyltype _T1038;unsigned _T1039;unsigned _T103A;struct Cyc_Absyn_Exp*_T103B;struct Cyc_Absyn_Pat*_T103C;struct Cyc_Yystacktype*_T103D;union Cyc_YYSTYPE*_T103E;union Cyc_YYSTYPE*_T103F;struct _tuple8*_T1040;struct _tuple8 _T1041;void*_T1042;struct Cyc_Yystacktype*_T1043;union Cyc_YYSTYPE*_T1044;union Cyc_YYSTYPE*_T1045;struct Cyc_List_List*_T1046;struct Cyc_List_List*_T1047;struct Cyc_Yystacktype*_T1048;struct Cyc_Yystacktype _T1049;struct Cyc_Yyltype _T104A;unsigned _T104B;unsigned _T104C;struct Cyc_Absyn_Exp*_T104D;struct Cyc_Absyn_Pat*_T104E;struct Cyc_Yystacktype*_T104F;struct Cyc_Yystacktype _T1050;struct Cyc_Yystacktype*_T1051;struct Cyc_Yystacktype _T1052;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1053;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1054;void*_T1055;struct Cyc_Yystacktype*_T1056;struct Cyc_Yystacktype _T1057;struct Cyc_Yyltype _T1058;unsigned _T1059;unsigned _T105A;struct Cyc_Absyn_Pat*_T105B;struct Cyc_Yystacktype*_T105C;union Cyc_YYSTYPE*_T105D;union Cyc_YYSTYPE*_T105E;struct Cyc_Absyn_Exp*_T105F;struct Cyc_Absyn_Pat*_T1060;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T1061;struct Cyc_Yystacktype*_T1062;union Cyc_YYSTYPE*_T1063;union Cyc_YYSTYPE*_T1064;void*_T1065;struct Cyc_Yystacktype*_T1066;struct Cyc_Yystacktype _T1067;struct Cyc_Yyltype _T1068;unsigned _T1069;unsigned _T106A;struct Cyc_Absyn_Pat*_T106B;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T106C;struct Cyc_Yystacktype*_T106D;union Cyc_YYSTYPE*_T106E;union Cyc_YYSTYPE*_T106F;void*_T1070;struct Cyc_Yystacktype*_T1071;struct Cyc_Yystacktype _T1072;struct Cyc_Yyltype _T1073;unsigned _T1074;unsigned _T1075;struct Cyc_Absyn_Pat*_T1076;struct Cyc_Yystacktype*_T1077;union Cyc_YYSTYPE*_T1078;union Cyc_YYSTYPE*_T1079;struct Cyc_Absyn_Exp*_T107A;int*_T107B;int _T107C;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T107D;union Cyc_Absyn_Cnst _T107E;struct _union_Cnst_LongLong_c _T107F;unsigned _T1080;union Cyc_Absyn_Cnst _T1081;struct _union_Cnst_Char_c _T1082;struct _tuple3 _T1083;union Cyc_Absyn_Cnst _T1084;struct _union_Cnst_Char_c _T1085;struct _tuple3 _T1086;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T1087;void*_T1088;struct Cyc_Absyn_Exp*_T1089;unsigned _T108A;struct Cyc_Absyn_Pat*_T108B;union Cyc_Absyn_Cnst _T108C;struct _union_Cnst_Short_c _T108D;struct _tuple4 _T108E;union Cyc_Absyn_Cnst _T108F;struct _union_Cnst_Short_c _T1090;struct _tuple4 _T1091;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T1092;short _T1093;void*_T1094;struct Cyc_Absyn_Exp*_T1095;unsigned _T1096;struct Cyc_Absyn_Pat*_T1097;union Cyc_Absyn_Cnst _T1098;struct _union_Cnst_Int_c _T1099;struct _tuple5 _T109A;union Cyc_Absyn_Cnst _T109B;struct _union_Cnst_Int_c _T109C;struct _tuple5 _T109D;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T109E;void*_T109F;struct Cyc_Absyn_Exp*_T10A0;unsigned _T10A1;struct Cyc_Absyn_Pat*_T10A2;union Cyc_Absyn_Cnst _T10A3;struct _union_Cnst_Float_c _T10A4;struct _tuple7 _T10A5;union Cyc_Absyn_Cnst _T10A6;struct _union_Cnst_Float_c _T10A7;struct _tuple7 _T10A8;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T10A9;void*_T10AA;struct Cyc_Absyn_Exp*_T10AB;unsigned _T10AC;struct Cyc_Absyn_Pat*_T10AD;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10AE;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10AF;void*_T10B0;struct Cyc_Absyn_Exp*_T10B1;unsigned _T10B2;struct Cyc_Absyn_Pat*_T10B3;struct Cyc_Yystacktype*_T10B4;struct Cyc_Yystacktype _T10B5;struct Cyc_Yyltype _T10B6;unsigned _T10B7;unsigned _T10B8;struct _fat_ptr _T10B9;struct _fat_ptr _T10BA;struct Cyc_Yystacktype*_T10BB;struct Cyc_Yystacktype _T10BC;struct Cyc_Yyltype _T10BD;unsigned _T10BE;unsigned _T10BF;struct _fat_ptr _T10C0;struct _fat_ptr _T10C1;struct Cyc_Yystacktype*_T10C2;struct Cyc_Yystacktype _T10C3;struct Cyc_Yyltype _T10C4;unsigned _T10C5;unsigned _T10C6;struct _fat_ptr _T10C7;struct _fat_ptr _T10C8;struct Cyc_Yystacktype*_T10C9;union Cyc_YYSTYPE*_T10CA;union Cyc_YYSTYPE*_T10CB;struct _fat_ptr _T10CC;struct _fat_ptr _T10CD;int _T10CE;struct Cyc_Yystacktype*_T10CF;struct Cyc_Yystacktype _T10D0;struct Cyc_Yyltype _T10D1;unsigned _T10D2;unsigned _T10D3;struct _fat_ptr _T10D4;struct _fat_ptr _T10D5;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T10D6;struct Cyc_Yystacktype*_T10D7;struct Cyc_Yystacktype _T10D8;struct Cyc_Yyltype _T10D9;unsigned _T10DA;unsigned _T10DB;struct _tuple0*_T10DC;struct _fat_ptr*_T10DD;struct Cyc_Yystacktype*_T10DE;union Cyc_YYSTYPE*_T10DF;union Cyc_YYSTYPE*_T10E0;void*_T10E1;struct Cyc_Yystacktype*_T10E2;union Cyc_YYSTYPE*_T10E3;union Cyc_YYSTYPE*_T10E4;void*_T10E5;struct Cyc_Yystacktype*_T10E6;struct Cyc_Yystacktype _T10E7;struct Cyc_Yyltype _T10E8;unsigned _T10E9;unsigned _T10EA;struct Cyc_Absyn_Pat*_T10EB;struct Cyc_Yystacktype*_T10EC;union Cyc_YYSTYPE*_T10ED;union Cyc_YYSTYPE*_T10EE;struct _fat_ptr _T10EF;struct _fat_ptr _T10F0;int _T10F1;struct Cyc_Yystacktype*_T10F2;struct Cyc_Yystacktype _T10F3;struct Cyc_Yyltype _T10F4;unsigned _T10F5;unsigned _T10F6;struct _fat_ptr _T10F7;struct _fat_ptr _T10F8;struct Cyc_Yystacktype*_T10F9;struct Cyc_Yystacktype _T10FA;struct Cyc_Yyltype _T10FB;unsigned _T10FC;struct Cyc_Yystacktype*_T10FD;union Cyc_YYSTYPE*_T10FE;union Cyc_YYSTYPE*_T10FF;struct _fat_ptr _T1100;unsigned _T1101;struct Cyc_Absyn_Tvar*_T1102;struct _fat_ptr*_T1103;struct Cyc_Yystacktype*_T1104;union Cyc_YYSTYPE*_T1105;union Cyc_YYSTYPE*_T1106;struct Cyc_Absyn_Tvar*_T1107;struct Cyc_Absyn_Tvar*_T1108;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1109;struct Cyc_Absyn_Kind*_T110A;struct Cyc_Absyn_Tvar*_T110B;struct Cyc_Yystacktype*_T110C;struct Cyc_Yystacktype _T110D;struct Cyc_Yyltype _T110E;unsigned _T110F;unsigned _T1110;struct _tuple0*_T1111;struct _fat_ptr*_T1112;struct Cyc_Yystacktype*_T1113;union Cyc_YYSTYPE*_T1114;union Cyc_YYSTYPE*_T1115;struct Cyc_Yystacktype*_T1116;union Cyc_YYSTYPE*_T1117;union Cyc_YYSTYPE*_T1118;struct _tuple8*_T1119;struct Cyc_Yystacktype*_T111A;struct Cyc_Yystacktype _T111B;struct Cyc_Yyltype _T111C;unsigned _T111D;unsigned _T111E;void*_T111F;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1120;void*_T1121;unsigned _T1122;struct Cyc_Absyn_Pat*_T1123;struct Cyc_Yystacktype*_T1124;union Cyc_YYSTYPE*_T1125;union Cyc_YYSTYPE*_T1126;struct _fat_ptr _T1127;struct _fat_ptr _T1128;int _T1129;struct Cyc_Yystacktype*_T112A;struct Cyc_Yystacktype _T112B;struct Cyc_Yyltype _T112C;unsigned _T112D;unsigned _T112E;struct _fat_ptr _T112F;struct _fat_ptr _T1130;struct Cyc_Yystacktype*_T1131;struct Cyc_Yystacktype _T1132;struct Cyc_Yyltype _T1133;unsigned _T1134;struct Cyc_Yystacktype*_T1135;union Cyc_YYSTYPE*_T1136;union Cyc_YYSTYPE*_T1137;struct _fat_ptr _T1138;unsigned _T1139;struct Cyc_Absyn_Tvar*_T113A;struct _fat_ptr*_T113B;struct Cyc_Yystacktype*_T113C;union Cyc_YYSTYPE*_T113D;union Cyc_YYSTYPE*_T113E;struct Cyc_Absyn_Tvar*_T113F;struct Cyc_Absyn_Tvar*_T1140;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1141;struct Cyc_Absyn_Kind*_T1142;struct Cyc_Absyn_Tvar*_T1143;struct Cyc_Yystacktype*_T1144;struct Cyc_Yystacktype _T1145;struct Cyc_Yyltype _T1146;unsigned _T1147;unsigned _T1148;struct _tuple0*_T1149;struct _fat_ptr*_T114A;struct Cyc_Yystacktype*_T114B;union Cyc_YYSTYPE*_T114C;union Cyc_YYSTYPE*_T114D;struct Cyc_Yystacktype*_T114E;union Cyc_YYSTYPE*_T114F;union Cyc_YYSTYPE*_T1150;struct _tuple8*_T1151;struct Cyc_Yystacktype*_T1152;struct Cyc_Yystacktype _T1153;struct Cyc_Yyltype _T1154;unsigned _T1155;unsigned _T1156;void*_T1157;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1158;void*_T1159;unsigned _T115A;struct Cyc_Absyn_Pat*_T115B;struct Cyc_Yystacktype*_T115C;union Cyc_YYSTYPE*_T115D;union Cyc_YYSTYPE*_T115E;struct _tuple23*_T115F;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1160;void*_T1161;struct Cyc_Yystacktype*_T1162;struct Cyc_Yystacktype _T1163;struct Cyc_Yyltype _T1164;unsigned _T1165;unsigned _T1166;struct Cyc_Absyn_Pat*_T1167;struct Cyc_Yystacktype*_T1168;union Cyc_YYSTYPE*_T1169;union Cyc_YYSTYPE*_T116A;struct _tuple23*_T116B;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T116C;struct Cyc_Yystacktype*_T116D;union Cyc_YYSTYPE*_T116E;union Cyc_YYSTYPE*_T116F;void*_T1170;struct Cyc_Yystacktype*_T1171;struct Cyc_Yystacktype _T1172;struct Cyc_Yyltype _T1173;unsigned _T1174;unsigned _T1175;struct Cyc_Absyn_Pat*_T1176;struct Cyc_Yystacktype*_T1177;union Cyc_YYSTYPE*_T1178;union Cyc_YYSTYPE*_T1179;struct _tuple23*_T117A;struct Cyc_List_List*(*_T117B)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T117C)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T117D;struct Cyc_Yystacktype _T117E;struct Cyc_Yyltype _T117F;unsigned _T1180;unsigned _T1181;struct Cyc_Yystacktype*_T1182;union Cyc_YYSTYPE*_T1183;union Cyc_YYSTYPE*_T1184;struct Cyc_List_List*_T1185;struct Cyc_Yystacktype*_T1186;union Cyc_YYSTYPE*_T1187;union Cyc_YYSTYPE*_T1188;struct _tuple0*_T1189;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T118A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T118B;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T118C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T118D;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T118E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T118F;void*_T1190;struct Cyc_Yystacktype*_T1191;struct Cyc_Yystacktype _T1192;struct Cyc_Yyltype _T1193;unsigned _T1194;unsigned _T1195;struct Cyc_Absyn_Pat*_T1196;struct Cyc_Yystacktype*_T1197;union Cyc_YYSTYPE*_T1198;union Cyc_YYSTYPE*_T1199;struct _tuple23*_T119A;struct Cyc_List_List*(*_T119B)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T119C)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T119D;struct Cyc_Yystacktype _T119E;struct Cyc_Yyltype _T119F;unsigned _T11A0;unsigned _T11A1;struct Cyc_Yystacktype*_T11A2;union Cyc_YYSTYPE*_T11A3;union Cyc_YYSTYPE*_T11A4;struct Cyc_List_List*_T11A5;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T11A6;void*_T11A7;struct Cyc_Yystacktype*_T11A8;struct Cyc_Yystacktype _T11A9;struct Cyc_Yyltype _T11AA;unsigned _T11AB;unsigned _T11AC;struct Cyc_Absyn_Pat*_T11AD;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11AE;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11AF;struct Cyc_Yystacktype*_T11B0;union Cyc_YYSTYPE*_T11B1;union Cyc_YYSTYPE*_T11B2;void*_T11B3;struct Cyc_Yystacktype*_T11B4;struct Cyc_Yystacktype _T11B5;struct Cyc_Yyltype _T11B6;unsigned _T11B7;unsigned _T11B8;void*_T11B9;struct Cyc_Yystacktype*_T11BA;struct Cyc_Yystacktype _T11BB;struct Cyc_Yyltype _T11BC;unsigned _T11BD;unsigned _T11BE;struct Cyc_Absyn_Pat*_T11BF;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T11C0;struct Cyc_Yystacktype*_T11C1;struct Cyc_Yystacktype _T11C2;struct Cyc_Yyltype _T11C3;unsigned _T11C4;unsigned _T11C5;struct _tuple0*_T11C6;struct _fat_ptr*_T11C7;struct Cyc_Yystacktype*_T11C8;union Cyc_YYSTYPE*_T11C9;union Cyc_YYSTYPE*_T11CA;void*_T11CB;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11CC;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11CD;void*_T11CE;struct Cyc_Yystacktype*_T11CF;struct Cyc_Yystacktype _T11D0;struct Cyc_Yyltype _T11D1;unsigned _T11D2;unsigned _T11D3;void*_T11D4;struct Cyc_Yystacktype*_T11D5;struct Cyc_Yystacktype _T11D6;struct Cyc_Yyltype _T11D7;unsigned _T11D8;unsigned _T11D9;struct Cyc_Absyn_Pat*_T11DA;struct Cyc_Yystacktype*_T11DB;union Cyc_YYSTYPE*_T11DC;union Cyc_YYSTYPE*_T11DD;struct _fat_ptr _T11DE;struct _fat_ptr _T11DF;int _T11E0;struct Cyc_Yystacktype*_T11E1;struct Cyc_Yystacktype _T11E2;struct Cyc_Yyltype _T11E3;unsigned _T11E4;unsigned _T11E5;struct _fat_ptr _T11E6;struct _fat_ptr _T11E7;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T11E8;struct Cyc_Yystacktype*_T11E9;struct Cyc_Yystacktype _T11EA;struct Cyc_Yyltype _T11EB;unsigned _T11EC;unsigned _T11ED;struct _tuple0*_T11EE;struct _fat_ptr*_T11EF;struct Cyc_Yystacktype*_T11F0;union Cyc_YYSTYPE*_T11F1;union Cyc_YYSTYPE*_T11F2;void*_T11F3;struct Cyc_Yystacktype*_T11F4;union Cyc_YYSTYPE*_T11F5;union Cyc_YYSTYPE*_T11F6;void*_T11F7;struct Cyc_Yystacktype*_T11F8;struct Cyc_Yystacktype _T11F9;struct Cyc_Yyltype _T11FA;unsigned _T11FB;unsigned _T11FC;struct Cyc_Absyn_Pat*_T11FD;struct Cyc_Yystacktype*_T11FE;union Cyc_YYSTYPE*_T11FF;union Cyc_YYSTYPE*_T1200;struct _fat_ptr _T1201;struct Cyc_Absyn_Kind*_T1202;struct Cyc_Absyn_Kind*_T1203;void*_T1204;struct Cyc_Yystacktype*_T1205;struct Cyc_Yystacktype _T1206;struct Cyc_Yyltype _T1207;unsigned _T1208;unsigned _T1209;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T120A;struct Cyc_Yystacktype*_T120B;struct Cyc_Yystacktype _T120C;struct Cyc_Yyltype _T120D;unsigned _T120E;unsigned _T120F;void*_T1210;struct Cyc_Yystacktype*_T1211;struct Cyc_Yystacktype _T1212;struct Cyc_Yyltype _T1213;unsigned _T1214;unsigned _T1215;struct _tuple0*_T1216;struct _fat_ptr*_T1217;struct Cyc_Yystacktype*_T1218;union Cyc_YYSTYPE*_T1219;union Cyc_YYSTYPE*_T121A;void*_T121B;void*_T121C;struct Cyc_Yystacktype*_T121D;struct Cyc_Yystacktype _T121E;struct Cyc_Yyltype _T121F;unsigned _T1220;unsigned _T1221;struct Cyc_Absyn_Pat*_T1222;struct Cyc_Absyn_Kind*_T1223;struct Cyc_Absyn_Kind*_T1224;void*_T1225;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1226;struct Cyc_Yystacktype*_T1227;struct Cyc_Yystacktype _T1228;struct Cyc_Yyltype _T1229;unsigned _T122A;unsigned _T122B;struct _tuple0*_T122C;struct _fat_ptr*_T122D;struct Cyc_Yystacktype*_T122E;union Cyc_YYSTYPE*_T122F;union Cyc_YYSTYPE*_T1230;void*_T1231;void*_T1232;void*_T1233;struct Cyc_Yystacktype*_T1234;struct Cyc_Yystacktype _T1235;struct Cyc_Yyltype _T1236;unsigned _T1237;unsigned _T1238;struct Cyc_Absyn_Pat*_T1239;struct _tuple23*_T123A;struct Cyc_Yystacktype*_T123B;union Cyc_YYSTYPE*_T123C;union Cyc_YYSTYPE*_T123D;struct Cyc_List_List*_T123E;struct _tuple23*_T123F;struct Cyc_Yystacktype*_T1240;union Cyc_YYSTYPE*_T1241;union Cyc_YYSTYPE*_T1242;struct Cyc_List_List*_T1243;struct _tuple23*_T1244;struct Cyc_List_List*_T1245;struct Cyc_Yystacktype*_T1246;union Cyc_YYSTYPE*_T1247;union Cyc_YYSTYPE*_T1248;struct Cyc_List_List*_T1249;struct Cyc_Yystacktype*_T124A;union Cyc_YYSTYPE*_T124B;union Cyc_YYSTYPE*_T124C;struct Cyc_Yystacktype*_T124D;union Cyc_YYSTYPE*_T124E;union Cyc_YYSTYPE*_T124F;struct _tuple24*_T1250;struct Cyc_Yystacktype*_T1251;union Cyc_YYSTYPE*_T1252;union Cyc_YYSTYPE*_T1253;struct _tuple24*_T1254;struct Cyc_Yystacktype*_T1255;union Cyc_YYSTYPE*_T1256;union Cyc_YYSTYPE*_T1257;struct Cyc_Yystacktype*_T1258;union Cyc_YYSTYPE*_T1259;union Cyc_YYSTYPE*_T125A;struct _tuple23*_T125B;struct Cyc_Yystacktype*_T125C;union Cyc_YYSTYPE*_T125D;union Cyc_YYSTYPE*_T125E;struct Cyc_List_List*_T125F;struct _tuple23*_T1260;struct Cyc_Yystacktype*_T1261;union Cyc_YYSTYPE*_T1262;union Cyc_YYSTYPE*_T1263;struct Cyc_List_List*_T1264;struct _tuple23*_T1265;struct Cyc_List_List*_T1266;struct Cyc_Yystacktype*_T1267;union Cyc_YYSTYPE*_T1268;union Cyc_YYSTYPE*_T1269;struct Cyc_List_List*_T126A;struct Cyc_Yystacktype*_T126B;union Cyc_YYSTYPE*_T126C;union Cyc_YYSTYPE*_T126D;struct Cyc_Yystacktype*_T126E;union Cyc_YYSTYPE*_T126F;union Cyc_YYSTYPE*_T1270;struct Cyc_Yystacktype*_T1271;struct Cyc_Yystacktype _T1272;struct Cyc_Yystacktype*_T1273;union Cyc_YYSTYPE*_T1274;union Cyc_YYSTYPE*_T1275;struct Cyc_Absyn_Exp*_T1276;struct Cyc_Yystacktype*_T1277;union Cyc_YYSTYPE*_T1278;union Cyc_YYSTYPE*_T1279;struct Cyc_Absyn_Exp*_T127A;struct Cyc_Yystacktype*_T127B;struct Cyc_Yystacktype _T127C;struct Cyc_Yyltype _T127D;unsigned _T127E;unsigned _T127F;struct Cyc_Absyn_Exp*_T1280;struct Cyc_Yystacktype*_T1281;struct Cyc_Yystacktype _T1282;struct Cyc_Yystacktype*_T1283;union Cyc_YYSTYPE*_T1284;union Cyc_YYSTYPE*_T1285;struct Cyc_Absyn_Exp*_T1286;struct Cyc_Yystacktype*_T1287;union Cyc_YYSTYPE*_T1288;union Cyc_YYSTYPE*_T1289;struct Cyc_Core_Opt*_T128A;struct Cyc_Yystacktype*_T128B;union Cyc_YYSTYPE*_T128C;union Cyc_YYSTYPE*_T128D;struct Cyc_Absyn_Exp*_T128E;struct Cyc_Yystacktype*_T128F;struct Cyc_Yystacktype _T1290;struct Cyc_Yyltype _T1291;unsigned _T1292;unsigned _T1293;struct Cyc_Absyn_Exp*_T1294;struct Cyc_Yystacktype*_T1295;union Cyc_YYSTYPE*_T1296;union Cyc_YYSTYPE*_T1297;struct Cyc_Absyn_Exp*_T1298;struct Cyc_Yystacktype*_T1299;union Cyc_YYSTYPE*_T129A;union Cyc_YYSTYPE*_T129B;struct Cyc_Absyn_Exp*_T129C;struct Cyc_Yystacktype*_T129D;struct Cyc_Yystacktype _T129E;struct Cyc_Yyltype _T129F;unsigned _T12A0;unsigned _T12A1;struct Cyc_Absyn_Exp*_T12A2;struct Cyc_Core_Opt*_T12A3;struct Cyc_Core_Opt*_T12A4;struct Cyc_Core_Opt*_T12A5;struct Cyc_Core_Opt*_T12A6;struct Cyc_Core_Opt*_T12A7;struct Cyc_Core_Opt*_T12A8;struct Cyc_Core_Opt*_T12A9;struct Cyc_Core_Opt*_T12AA;struct Cyc_Core_Opt*_T12AB;struct Cyc_Core_Opt*_T12AC;struct Cyc_Yystacktype*_T12AD;struct Cyc_Yystacktype _T12AE;struct Cyc_Yystacktype*_T12AF;union Cyc_YYSTYPE*_T12B0;union Cyc_YYSTYPE*_T12B1;struct Cyc_Absyn_Exp*_T12B2;struct Cyc_Yystacktype*_T12B3;union Cyc_YYSTYPE*_T12B4;union Cyc_YYSTYPE*_T12B5;struct Cyc_Absyn_Exp*_T12B6;struct Cyc_Yystacktype*_T12B7;union Cyc_YYSTYPE*_T12B8;union Cyc_YYSTYPE*_T12B9;struct Cyc_Absyn_Exp*_T12BA;struct Cyc_Yystacktype*_T12BB;struct Cyc_Yystacktype _T12BC;struct Cyc_Yyltype _T12BD;unsigned _T12BE;unsigned _T12BF;struct Cyc_Absyn_Exp*_T12C0;struct Cyc_Yystacktype*_T12C1;union Cyc_YYSTYPE*_T12C2;union Cyc_YYSTYPE*_T12C3;struct Cyc_Absyn_Exp*_T12C4;struct Cyc_Yystacktype*_T12C5;struct Cyc_Yystacktype _T12C6;struct Cyc_Yyltype _T12C7;unsigned _T12C8;unsigned _T12C9;struct Cyc_Absyn_Exp*_T12CA;struct Cyc_Yystacktype*_T12CB;union Cyc_YYSTYPE*_T12CC;union Cyc_YYSTYPE*_T12CD;struct Cyc_Absyn_Exp*_T12CE;struct Cyc_Yystacktype*_T12CF;struct Cyc_Yystacktype _T12D0;struct Cyc_Yyltype _T12D1;unsigned _T12D2;unsigned _T12D3;struct Cyc_Absyn_Exp*_T12D4;struct Cyc_Yystacktype*_T12D5;union Cyc_YYSTYPE*_T12D6;union Cyc_YYSTYPE*_T12D7;struct Cyc_Absyn_Exp*_T12D8;struct Cyc_Yystacktype*_T12D9;struct Cyc_Yystacktype _T12DA;struct Cyc_Yyltype _T12DB;unsigned _T12DC;unsigned _T12DD;struct Cyc_Absyn_Exp*_T12DE;struct Cyc_Yystacktype*_T12DF;union Cyc_YYSTYPE*_T12E0;union Cyc_YYSTYPE*_T12E1;struct Cyc_Absyn_Exp*_T12E2;struct Cyc_Yystacktype*_T12E3;union Cyc_YYSTYPE*_T12E4;union Cyc_YYSTYPE*_T12E5;struct Cyc_Absyn_Exp*_T12E6;struct Cyc_Yystacktype*_T12E7;struct Cyc_Yystacktype _T12E8;struct Cyc_Yyltype _T12E9;unsigned _T12EA;unsigned _T12EB;struct Cyc_Absyn_Exp*_T12EC;struct Cyc_Yystacktype*_T12ED;union Cyc_YYSTYPE*_T12EE;union Cyc_YYSTYPE*_T12EF;struct Cyc_Absyn_Exp*_T12F0;struct Cyc_Yystacktype*_T12F1;union Cyc_YYSTYPE*_T12F2;union Cyc_YYSTYPE*_T12F3;struct Cyc_Absyn_Exp*_T12F4;struct Cyc_Yystacktype*_T12F5;struct Cyc_Yystacktype _T12F6;struct Cyc_Yyltype _T12F7;unsigned _T12F8;unsigned _T12F9;struct Cyc_Absyn_Exp*_T12FA;struct Cyc_Yystacktype*_T12FB;union Cyc_YYSTYPE*_T12FC;union Cyc_YYSTYPE*_T12FD;struct Cyc_Absyn_Exp*_T12FE;struct Cyc_Absyn_Exp*_T12FF;struct Cyc_Yystacktype*_T1300;union Cyc_YYSTYPE*_T1301;union Cyc_YYSTYPE*_T1302;struct Cyc_Absyn_Exp*_T1303;struct Cyc_Absyn_Exp*_T1304;struct Cyc_Yystacktype*_T1305;struct Cyc_Yystacktype _T1306;struct Cyc_Yyltype _T1307;unsigned _T1308;unsigned _T1309;struct Cyc_Absyn_Exp*_T130A;struct Cyc_Yystacktype*_T130B;union Cyc_YYSTYPE*_T130C;union Cyc_YYSTYPE*_T130D;struct Cyc_Absyn_Exp*_T130E;struct Cyc_Absyn_Exp*_T130F;struct Cyc_Yystacktype*_T1310;union Cyc_YYSTYPE*_T1311;union Cyc_YYSTYPE*_T1312;struct Cyc_Absyn_Exp*_T1313;struct Cyc_Absyn_Exp*_T1314;struct Cyc_Yystacktype*_T1315;struct Cyc_Yystacktype _T1316;struct Cyc_Yyltype _T1317;unsigned _T1318;unsigned _T1319;struct Cyc_Absyn_Exp*_T131A;struct Cyc_Yystacktype*_T131B;struct Cyc_Yystacktype _T131C;struct Cyc_Yystacktype*_T131D;struct Cyc_Yystacktype _T131E;struct Cyc_Yystacktype*_T131F;union Cyc_YYSTYPE*_T1320;union Cyc_YYSTYPE*_T1321;struct Cyc_Absyn_Exp*_T1322;struct Cyc_Yystacktype*_T1323;union Cyc_YYSTYPE*_T1324;union Cyc_YYSTYPE*_T1325;struct Cyc_Absyn_Exp*_T1326;struct Cyc_Yystacktype*_T1327;struct Cyc_Yystacktype _T1328;struct Cyc_Yyltype _T1329;unsigned _T132A;unsigned _T132B;struct Cyc_Absyn_Exp*_T132C;struct Cyc_Yystacktype*_T132D;struct Cyc_Yystacktype _T132E;struct Cyc_Yystacktype*_T132F;union Cyc_YYSTYPE*_T1330;union Cyc_YYSTYPE*_T1331;struct Cyc_Absyn_Exp*_T1332;struct Cyc_Yystacktype*_T1333;union Cyc_YYSTYPE*_T1334;union Cyc_YYSTYPE*_T1335;struct Cyc_Absyn_Exp*_T1336;struct Cyc_Yystacktype*_T1337;struct Cyc_Yystacktype _T1338;struct Cyc_Yyltype _T1339;unsigned _T133A;unsigned _T133B;struct Cyc_Absyn_Exp*_T133C;struct Cyc_Yystacktype*_T133D;struct Cyc_Yystacktype _T133E;struct Cyc_Yystacktype*_T133F;union Cyc_YYSTYPE*_T1340;union Cyc_YYSTYPE*_T1341;struct Cyc_Absyn_Exp*_T1342;struct Cyc_Yystacktype*_T1343;union Cyc_YYSTYPE*_T1344;union Cyc_YYSTYPE*_T1345;struct Cyc_Absyn_Exp*_T1346;struct Cyc_Yystacktype*_T1347;struct Cyc_Yystacktype _T1348;struct Cyc_Yyltype _T1349;unsigned _T134A;unsigned _T134B;struct Cyc_Absyn_Exp*_T134C;struct Cyc_Yystacktype*_T134D;struct Cyc_Yystacktype _T134E;struct Cyc_Yystacktype*_T134F;union Cyc_YYSTYPE*_T1350;union Cyc_YYSTYPE*_T1351;struct Cyc_Absyn_Exp*_T1352;struct Cyc_Yystacktype*_T1353;union Cyc_YYSTYPE*_T1354;union Cyc_YYSTYPE*_T1355;struct Cyc_Absyn_Exp*_T1356;struct Cyc_Yystacktype*_T1357;struct Cyc_Yystacktype _T1358;struct Cyc_Yyltype _T1359;unsigned _T135A;unsigned _T135B;struct Cyc_Absyn_Exp*_T135C;struct Cyc_Yystacktype*_T135D;struct Cyc_Yystacktype _T135E;struct Cyc_Yystacktype*_T135F;union Cyc_YYSTYPE*_T1360;union Cyc_YYSTYPE*_T1361;struct Cyc_Absyn_Exp*_T1362;struct Cyc_Yystacktype*_T1363;union Cyc_YYSTYPE*_T1364;union Cyc_YYSTYPE*_T1365;struct Cyc_Absyn_Exp*_T1366;struct Cyc_Yystacktype*_T1367;struct Cyc_Yystacktype _T1368;struct Cyc_Yyltype _T1369;unsigned _T136A;unsigned _T136B;struct Cyc_Absyn_Exp*_T136C;struct Cyc_Yystacktype*_T136D;struct Cyc_Yystacktype _T136E;struct Cyc_Yystacktype*_T136F;union Cyc_YYSTYPE*_T1370;union Cyc_YYSTYPE*_T1371;struct Cyc_Absyn_Exp*(*_T1372)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T1373;union Cyc_YYSTYPE*_T1374;union Cyc_YYSTYPE*_T1375;struct Cyc_Absyn_Exp*_T1376;struct Cyc_Yystacktype*_T1377;union Cyc_YYSTYPE*_T1378;union Cyc_YYSTYPE*_T1379;struct Cyc_Absyn_Exp*_T137A;struct Cyc_Yystacktype*_T137B;struct Cyc_Yystacktype _T137C;struct Cyc_Yyltype _T137D;unsigned _T137E;unsigned _T137F;struct Cyc_Absyn_Exp*_T1380;struct Cyc_Yystacktype*_T1381;struct Cyc_Yystacktype _T1382;struct Cyc_Yystacktype*_T1383;union Cyc_YYSTYPE*_T1384;union Cyc_YYSTYPE*_T1385;struct Cyc_Absyn_Exp*(*_T1386)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T1387;union Cyc_YYSTYPE*_T1388;union Cyc_YYSTYPE*_T1389;struct Cyc_Absyn_Exp*_T138A;struct Cyc_Yystacktype*_T138B;union Cyc_YYSTYPE*_T138C;union Cyc_YYSTYPE*_T138D;struct Cyc_Absyn_Exp*_T138E;struct Cyc_Yystacktype*_T138F;struct Cyc_Yystacktype _T1390;struct Cyc_Yyltype _T1391;unsigned _T1392;unsigned _T1393;struct Cyc_Absyn_Exp*_T1394;struct Cyc_Yystacktype*_T1395;struct Cyc_Yystacktype _T1396;struct Cyc_Yystacktype*_T1397;union Cyc_YYSTYPE*_T1398;union Cyc_YYSTYPE*_T1399;struct Cyc_Absyn_Exp*_T139A;struct Cyc_Yystacktype*_T139B;union Cyc_YYSTYPE*_T139C;union Cyc_YYSTYPE*_T139D;struct Cyc_Absyn_Exp*_T139E;struct Cyc_Yystacktype*_T139F;struct Cyc_Yystacktype _T13A0;struct Cyc_Yyltype _T13A1;unsigned _T13A2;unsigned _T13A3;struct Cyc_Absyn_Exp*_T13A4;struct Cyc_Yystacktype*_T13A5;union Cyc_YYSTYPE*_T13A6;union Cyc_YYSTYPE*_T13A7;struct Cyc_Absyn_Exp*_T13A8;struct Cyc_Yystacktype*_T13A9;union Cyc_YYSTYPE*_T13AA;union Cyc_YYSTYPE*_T13AB;struct Cyc_Absyn_Exp*_T13AC;struct Cyc_Yystacktype*_T13AD;struct Cyc_Yystacktype _T13AE;struct Cyc_Yyltype _T13AF;unsigned _T13B0;unsigned _T13B1;struct Cyc_Absyn_Exp*_T13B2;struct Cyc_Yystacktype*_T13B3;struct Cyc_Yystacktype _T13B4;struct Cyc_Yystacktype*_T13B5;union Cyc_YYSTYPE*_T13B6;union Cyc_YYSTYPE*_T13B7;enum Cyc_Absyn_Primop _T13B8;struct Cyc_Yystacktype*_T13B9;union Cyc_YYSTYPE*_T13BA;union Cyc_YYSTYPE*_T13BB;struct Cyc_Absyn_Exp*_T13BC;struct Cyc_Yystacktype*_T13BD;union Cyc_YYSTYPE*_T13BE;union Cyc_YYSTYPE*_T13BF;struct Cyc_Absyn_Exp*_T13C0;struct Cyc_Yystacktype*_T13C1;struct Cyc_Yystacktype _T13C2;struct Cyc_Yyltype _T13C3;unsigned _T13C4;unsigned _T13C5;struct Cyc_Absyn_Exp*_T13C6;struct Cyc_Yystacktype*_T13C7;struct Cyc_Yystacktype _T13C8;struct Cyc_Yystacktype*_T13C9;union Cyc_YYSTYPE*_T13CA;union Cyc_YYSTYPE*_T13CB;enum Cyc_Absyn_Primop _T13CC;struct Cyc_Yystacktype*_T13CD;union Cyc_YYSTYPE*_T13CE;union Cyc_YYSTYPE*_T13CF;struct Cyc_Absyn_Exp*_T13D0;struct Cyc_Yystacktype*_T13D1;union Cyc_YYSTYPE*_T13D2;union Cyc_YYSTYPE*_T13D3;struct Cyc_Absyn_Exp*_T13D4;struct Cyc_Yystacktype*_T13D5;struct Cyc_Yystacktype _T13D6;struct Cyc_Yyltype _T13D7;unsigned _T13D8;unsigned _T13D9;struct Cyc_Absyn_Exp*_T13DA;struct Cyc_Absyn_Exp*(*_T13DB)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DC)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DD)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DE)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DF)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13E0)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T13E1;struct Cyc_Yystacktype _T13E2;struct Cyc_Yystacktype*_T13E3;union Cyc_YYSTYPE*_T13E4;union Cyc_YYSTYPE*_T13E5;struct _tuple8*_T13E6;struct Cyc_Yystacktype*_T13E7;struct Cyc_Yystacktype _T13E8;struct Cyc_Yyltype _T13E9;unsigned _T13EA;unsigned _T13EB;void*_T13EC;struct Cyc_Yystacktype*_T13ED;union Cyc_YYSTYPE*_T13EE;union Cyc_YYSTYPE*_T13EF;struct Cyc_Absyn_Exp*_T13F0;struct Cyc_Yystacktype*_T13F1;struct Cyc_Yystacktype _T13F2;struct Cyc_Yyltype _T13F3;unsigned _T13F4;unsigned _T13F5;struct Cyc_Absyn_Exp*_T13F6;struct Cyc_Yystacktype*_T13F7;struct Cyc_Yystacktype _T13F8;struct Cyc_Yystacktype*_T13F9;union Cyc_YYSTYPE*_T13FA;union Cyc_YYSTYPE*_T13FB;struct Cyc_Absyn_Exp*_T13FC;struct Cyc_Yystacktype*_T13FD;struct Cyc_Yystacktype _T13FE;struct Cyc_Yyltype _T13FF;unsigned _T1400;unsigned _T1401;struct Cyc_Absyn_Exp*_T1402;struct Cyc_Yystacktype*_T1403;union Cyc_YYSTYPE*_T1404;union Cyc_YYSTYPE*_T1405;struct Cyc_Absyn_Exp*_T1406;struct Cyc_Yystacktype*_T1407;struct Cyc_Yystacktype _T1408;struct Cyc_Yyltype _T1409;unsigned _T140A;unsigned _T140B;struct Cyc_Absyn_Exp*_T140C;struct Cyc_Yystacktype*_T140D;union Cyc_YYSTYPE*_T140E;union Cyc_YYSTYPE*_T140F;struct Cyc_Absyn_Exp*_T1410;struct Cyc_Yystacktype*_T1411;struct Cyc_Yystacktype _T1412;struct Cyc_Yyltype _T1413;unsigned _T1414;unsigned _T1415;struct Cyc_Absyn_Exp*_T1416;struct Cyc_Yystacktype*_T1417;union Cyc_YYSTYPE*_T1418;union Cyc_YYSTYPE*_T1419;struct Cyc_Absyn_Exp*_T141A;struct Cyc_Yystacktype*_T141B;struct Cyc_Yystacktype _T141C;struct Cyc_Yyltype _T141D;unsigned _T141E;unsigned _T141F;struct Cyc_Absyn_Exp*_T1420;struct Cyc_Yystacktype*_T1421;union Cyc_YYSTYPE*_T1422;union Cyc_YYSTYPE*_T1423;enum Cyc_Absyn_Primop _T1424;struct Cyc_Yystacktype*_T1425;union Cyc_YYSTYPE*_T1426;union Cyc_YYSTYPE*_T1427;struct Cyc_Absyn_Exp*_T1428;struct Cyc_Yystacktype*_T1429;struct Cyc_Yystacktype _T142A;struct Cyc_Yyltype _T142B;unsigned _T142C;unsigned _T142D;struct Cyc_Absyn_Exp*_T142E;struct Cyc_Yystacktype*_T142F;union Cyc_YYSTYPE*_T1430;union Cyc_YYSTYPE*_T1431;struct _tuple8*_T1432;struct Cyc_Yystacktype*_T1433;struct Cyc_Yystacktype _T1434;struct Cyc_Yyltype _T1435;unsigned _T1436;unsigned _T1437;void*_T1438;struct Cyc_Yystacktype*_T1439;struct Cyc_Yystacktype _T143A;struct Cyc_Yyltype _T143B;unsigned _T143C;unsigned _T143D;struct Cyc_Absyn_Exp*_T143E;struct Cyc_Yystacktype*_T143F;union Cyc_YYSTYPE*_T1440;union Cyc_YYSTYPE*_T1441;struct Cyc_Absyn_Exp*_T1442;struct Cyc_Yystacktype*_T1443;struct Cyc_Yystacktype _T1444;struct Cyc_Yyltype _T1445;unsigned _T1446;unsigned _T1447;struct Cyc_Absyn_Exp*_T1448;struct Cyc_Yystacktype*_T1449;union Cyc_YYSTYPE*_T144A;union Cyc_YYSTYPE*_T144B;struct _tuple8*_T144C;struct Cyc_Yystacktype*_T144D;struct Cyc_Yystacktype _T144E;struct Cyc_Yyltype _T144F;unsigned _T1450;unsigned _T1451;void*_T1452;struct Cyc_Yystacktype*_T1453;union Cyc_YYSTYPE*_T1454;union Cyc_YYSTYPE*_T1455;struct Cyc_List_List*_T1456;struct Cyc_List_List*_T1457;struct Cyc_Yystacktype*_T1458;struct Cyc_Yystacktype _T1459;struct Cyc_Yyltype _T145A;unsigned _T145B;unsigned _T145C;struct Cyc_Absyn_Exp*_T145D;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T145E;struct Cyc_Yystacktype*_T145F;union Cyc_YYSTYPE*_T1460;union Cyc_YYSTYPE*_T1461;void*_T1462;struct Cyc_Yystacktype*_T1463;struct Cyc_Yystacktype _T1464;struct Cyc_Yyltype _T1465;unsigned _T1466;unsigned _T1467;struct Cyc_Absyn_Exp*_T1468;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1469;struct Cyc_Yystacktype*_T146A;union Cyc_YYSTYPE*_T146B;union Cyc_YYSTYPE*_T146C;struct Cyc_Yystacktype*_T146D;union Cyc_YYSTYPE*_T146E;union Cyc_YYSTYPE*_T146F;struct Cyc_Yystacktype*_T1470;union Cyc_YYSTYPE*_T1471;union Cyc_YYSTYPE*_T1472;void*_T1473;struct Cyc_Yystacktype*_T1474;struct Cyc_Yystacktype _T1475;struct Cyc_Yyltype _T1476;unsigned _T1477;unsigned _T1478;struct Cyc_Absyn_Exp*_T1479;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T147A;struct Cyc_Yystacktype*_T147B;union Cyc_YYSTYPE*_T147C;union Cyc_YYSTYPE*_T147D;struct Cyc_Yystacktype*_T147E;union Cyc_YYSTYPE*_T147F;union Cyc_YYSTYPE*_T1480;void*_T1481;struct Cyc_Yystacktype*_T1482;struct Cyc_Yystacktype _T1483;struct Cyc_Yyltype _T1484;unsigned _T1485;unsigned _T1486;struct Cyc_Absyn_Exp*_T1487;struct Cyc_Yystacktype*_T1488;union Cyc_YYSTYPE*_T1489;union Cyc_YYSTYPE*_T148A;struct Cyc_Absyn_Exp*_T148B;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T148C;struct Cyc_Yystacktype*_T148D;union Cyc_YYSTYPE*_T148E;union Cyc_YYSTYPE*_T148F;void*_T1490;struct Cyc_Yystacktype*_T1491;struct Cyc_Yystacktype _T1492;struct Cyc_Yyltype _T1493;unsigned _T1494;unsigned _T1495;struct Cyc_Absyn_Exp*_T1496;struct Cyc_Yystacktype*_T1497;union Cyc_YYSTYPE*_T1498;union Cyc_YYSTYPE*_T1499;struct Cyc_Absyn_Exp*_T149A;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T149B;struct Cyc_Yystacktype*_T149C;union Cyc_YYSTYPE*_T149D;union Cyc_YYSTYPE*_T149E;void*_T149F;struct Cyc_Yystacktype*_T14A0;struct Cyc_Yystacktype _T14A1;struct Cyc_Yyltype _T14A2;unsigned _T14A3;unsigned _T14A4;struct Cyc_Absyn_Exp*_T14A5;struct Cyc_Yystacktype*_T14A6;union Cyc_YYSTYPE*_T14A7;union Cyc_YYSTYPE*_T14A8;struct _tuple8*_T14A9;struct Cyc_Yystacktype*_T14AA;struct Cyc_Yystacktype _T14AB;struct Cyc_Yyltype _T14AC;unsigned _T14AD;unsigned _T14AE;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14AF;void**_T14B0;struct Cyc_Yystacktype*_T14B1;union Cyc_YYSTYPE*_T14B2;union Cyc_YYSTYPE*_T14B3;void*_T14B4;struct Cyc_Yystacktype*_T14B5;struct Cyc_Yystacktype _T14B6;struct Cyc_Yyltype _T14B7;unsigned _T14B8;unsigned _T14B9;struct Cyc_Absyn_Exp*_T14BA;struct Cyc_Yystacktype*_T14BB;union Cyc_YYSTYPE*_T14BC;union Cyc_YYSTYPE*_T14BD;struct _tuple8*_T14BE;struct Cyc_Yystacktype*_T14BF;struct Cyc_Yystacktype _T14C0;struct Cyc_Yyltype _T14C1;unsigned _T14C2;unsigned _T14C3;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14C4;struct Cyc_Yystacktype*_T14C5;union Cyc_YYSTYPE*_T14C6;union Cyc_YYSTYPE*_T14C7;struct Cyc_Yystacktype*_T14C8;union Cyc_YYSTYPE*_T14C9;union Cyc_YYSTYPE*_T14CA;void**_T14CB;struct Cyc_Yystacktype*_T14CC;union Cyc_YYSTYPE*_T14CD;union Cyc_YYSTYPE*_T14CE;void*_T14CF;struct Cyc_Yystacktype*_T14D0;struct Cyc_Yystacktype _T14D1;struct Cyc_Yyltype _T14D2;unsigned _T14D3;unsigned _T14D4;struct Cyc_Absyn_Exp*_T14D5;struct Cyc_Yystacktype*_T14D6;union Cyc_YYSTYPE*_T14D7;union Cyc_YYSTYPE*_T14D8;struct _tuple8*_T14D9;struct Cyc_Yystacktype*_T14DA;struct Cyc_Yystacktype _T14DB;struct Cyc_Yyltype _T14DC;unsigned _T14DD;unsigned _T14DE;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14DF;struct Cyc_Yystacktype*_T14E0;union Cyc_YYSTYPE*_T14E1;union Cyc_YYSTYPE*_T14E2;void**_T14E3;struct Cyc_Yystacktype*_T14E4;union Cyc_YYSTYPE*_T14E5;union Cyc_YYSTYPE*_T14E6;void*_T14E7;struct Cyc_Yystacktype*_T14E8;struct Cyc_Yystacktype _T14E9;struct Cyc_Yyltype _T14EA;unsigned _T14EB;unsigned _T14EC;struct Cyc_Absyn_Exp*_T14ED;struct Cyc_List_List*_T14EE;struct Cyc_Yystacktype*_T14EF;union Cyc_YYSTYPE*_T14F0;union Cyc_YYSTYPE*_T14F1;struct _fat_ptr _T14F2;struct Cyc_Yystacktype*_T14F3;struct Cyc_Yystacktype _T14F4;struct Cyc_Yyltype _T14F5;unsigned _T14F6;unsigned _T14F7;struct Cyc_Absyn_Exp*_T14F8;struct Cyc_Yystacktype*_T14F9;union Cyc_YYSTYPE*_T14FA;union Cyc_YYSTYPE*_T14FB;struct Cyc_Absyn_Exp*_T14FC;struct Cyc_Yystacktype*_T14FD;struct Cyc_Yystacktype _T14FE;struct Cyc_Yyltype _T14FF;unsigned _T1500;unsigned _T1501;struct Cyc_Absyn_Exp*_T1502;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1503;struct Cyc_Yystacktype*_T1504;union Cyc_YYSTYPE*_T1505;union Cyc_YYSTYPE*_T1506;struct _fat_ptr*_T1507;struct Cyc_Yystacktype*_T1508;union Cyc_YYSTYPE*_T1509;union Cyc_YYSTYPE*_T150A;void*_T150B;struct Cyc_Yystacktype*_T150C;struct Cyc_Yystacktype _T150D;struct Cyc_Yyltype _T150E;unsigned _T150F;unsigned _T1510;struct Cyc_Absyn_Exp*_T1511;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1512;struct Cyc_Yystacktype*_T1513;union Cyc_YYSTYPE*_T1514;union Cyc_YYSTYPE*_T1515;struct Cyc_Absyn_Exp*_T1516;struct Cyc_Yystacktype*_T1517;struct Cyc_Yystacktype _T1518;struct Cyc_Yyltype _T1519;unsigned _T151A;unsigned _T151B;struct _fat_ptr*_T151C;struct Cyc_Yystacktype*_T151D;union Cyc_YYSTYPE*_T151E;union Cyc_YYSTYPE*_T151F;void*_T1520;struct Cyc_Yystacktype*_T1521;struct Cyc_Yystacktype _T1522;struct Cyc_Yyltype _T1523;unsigned _T1524;unsigned _T1525;struct Cyc_Absyn_Exp*_T1526;struct Cyc_Yystacktype*_T1527;union Cyc_YYSTYPE*_T1528;union Cyc_YYSTYPE*_T1529;struct _tuple8*_T152A;struct Cyc_Yystacktype*_T152B;struct Cyc_Yystacktype _T152C;struct Cyc_Yyltype _T152D;unsigned _T152E;unsigned _T152F;void*_T1530;struct Cyc_Yystacktype*_T1531;struct Cyc_Yystacktype _T1532;struct Cyc_Yyltype _T1533;unsigned _T1534;unsigned _T1535;struct Cyc_Absyn_Exp*_T1536;struct Cyc_Yystacktype*_T1537;union Cyc_YYSTYPE*_T1538;union Cyc_YYSTYPE*_T1539;void*_T153A;struct Cyc_Yystacktype*_T153B;struct Cyc_Yystacktype _T153C;struct Cyc_Yyltype _T153D;unsigned _T153E;unsigned _T153F;struct Cyc_Absyn_Exp*_T1540;struct Cyc_Yystacktype*_T1541;union Cyc_YYSTYPE*_T1542;union Cyc_YYSTYPE*_T1543;struct Cyc_Absyn_Exp*_T1544;struct Cyc_Yystacktype*_T1545;struct Cyc_Yystacktype _T1546;struct Cyc_Yyltype _T1547;unsigned _T1548;unsigned _T1549;struct Cyc_Absyn_Exp*_T154A;struct Cyc_Yystacktype*_T154B;union Cyc_YYSTYPE*_T154C;union Cyc_YYSTYPE*_T154D;struct Cyc_Absyn_Exp*_T154E;struct Cyc_Yystacktype*_T154F;struct Cyc_Yystacktype _T1550;struct Cyc_Yyltype _T1551;unsigned _T1552;unsigned _T1553;struct Cyc_Absyn_Exp*_T1554;struct Cyc_Yystacktype*_T1555;union Cyc_YYSTYPE*_T1556;union Cyc_YYSTYPE*_T1557;struct Cyc_Absyn_Exp*_T1558;struct Cyc_Yystacktype*_T1559;struct Cyc_Yystacktype _T155A;struct Cyc_Yyltype _T155B;unsigned _T155C;unsigned _T155D;struct Cyc_Absyn_Exp*_T155E;struct Cyc_Yystacktype*_T155F;union Cyc_YYSTYPE*_T1560;union Cyc_YYSTYPE*_T1561;struct Cyc_Absyn_Exp*_T1562;struct Cyc_Yystacktype*_T1563;struct Cyc_Yystacktype _T1564;struct Cyc_Yyltype _T1565;unsigned _T1566;unsigned _T1567;struct Cyc_Absyn_Exp*_T1568;struct Cyc_Yystacktype*_T1569;union Cyc_YYSTYPE*_T156A;union Cyc_YYSTYPE*_T156B;struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T156C;struct Cyc_Yystacktype*_T156D;union Cyc_YYSTYPE*_T156E;union Cyc_YYSTYPE*_T156F;struct Cyc_Yystacktype*_T1570;union Cyc_YYSTYPE*_T1571;union Cyc_YYSTYPE*_T1572;void*_T1573;struct _tuple31*_T1574;struct Cyc_Yystacktype*_T1575;union Cyc_YYSTYPE*_T1576;union Cyc_YYSTYPE*_T1577;struct _tuple31*_T1578;struct Cyc_Yystacktype*_T1579;union Cyc_YYSTYPE*_T157A;union Cyc_YYSTYPE*_T157B;struct _tuple31*_T157C;struct Cyc_Yystacktype*_T157D;union Cyc_YYSTYPE*_T157E;union Cyc_YYSTYPE*_T157F;struct Cyc_List_List*_T1580;struct Cyc_List_List*_T1581;struct Cyc_Yystacktype*_T1582;union Cyc_YYSTYPE*_T1583;union Cyc_YYSTYPE*_T1584;struct Cyc_List_List*_T1585;struct Cyc_Yystacktype*_T1586;union Cyc_YYSTYPE*_T1587;union Cyc_YYSTYPE*_T1588;struct Cyc_Yystacktype*_T1589;union Cyc_YYSTYPE*_T158A;union Cyc_YYSTYPE*_T158B;struct _tuple28*_T158C;struct _tuple28*_T158D;struct Cyc_Yystacktype*_T158E;union Cyc_YYSTYPE*_T158F;union Cyc_YYSTYPE*_T1590;struct _tuple28*_T1591;struct Cyc_Yystacktype*_T1592;union Cyc_YYSTYPE*_T1593;union Cyc_YYSTYPE*_T1594;struct Cyc_List_List*_T1595;struct Cyc_Yystacktype*_T1596;union Cyc_YYSTYPE*_T1597;union Cyc_YYSTYPE*_T1598;struct Cyc_List_List*_T1599;struct Cyc_Yystacktype*_T159A;union Cyc_YYSTYPE*_T159B;union Cyc_YYSTYPE*_T159C;struct Cyc_List_List*_T159D;struct Cyc_Yystacktype*_T159E;union Cyc_YYSTYPE*_T159F;union Cyc_YYSTYPE*_T15A0;struct Cyc_Yystacktype*_T15A1;union Cyc_YYSTYPE*_T15A2;union Cyc_YYSTYPE*_T15A3;struct _tuple32*_T15A4;struct Cyc_Yystacktype*_T15A5;union Cyc_YYSTYPE*_T15A6;union Cyc_YYSTYPE*_T15A7;struct Cyc_Yystacktype*_T15A8;union Cyc_YYSTYPE*_T15A9;union Cyc_YYSTYPE*_T15AA;struct Cyc_Yystacktype*_T15AB;union Cyc_YYSTYPE*_T15AC;union Cyc_YYSTYPE*_T15AD;struct Cyc_List_List*_T15AE;struct Cyc_List_List*_T15AF;struct Cyc_List_List*_T15B0;struct _fat_ptr*_T15B1;struct Cyc_Yystacktype*_T15B2;union Cyc_YYSTYPE*_T15B3;union Cyc_YYSTYPE*_T15B4;struct Cyc_List_List*_T15B5;struct _fat_ptr*_T15B6;struct Cyc_Yystacktype*_T15B7;union Cyc_YYSTYPE*_T15B8;union Cyc_YYSTYPE*_T15B9;struct Cyc_Yystacktype*_T15BA;union Cyc_YYSTYPE*_T15BB;union Cyc_YYSTYPE*_T15BC;struct Cyc_Yystacktype*_T15BD;struct Cyc_Yystacktype _T15BE;struct Cyc_Yystacktype*_T15BF;union Cyc_YYSTYPE*_T15C0;union Cyc_YYSTYPE*_T15C1;struct Cyc_Absyn_Exp*_T15C2;struct Cyc_Yystacktype*_T15C3;union Cyc_YYSTYPE*_T15C4;union Cyc_YYSTYPE*_T15C5;struct Cyc_Absyn_Exp*_T15C6;struct Cyc_Yystacktype*_T15C7;struct Cyc_Yystacktype _T15C8;struct Cyc_Yyltype _T15C9;unsigned _T15CA;unsigned _T15CB;struct Cyc_Absyn_Exp*_T15CC;struct Cyc_Yystacktype*_T15CD;union Cyc_YYSTYPE*_T15CE;union Cyc_YYSTYPE*_T15CF;struct Cyc_Absyn_Exp*_T15D0;struct Cyc_Yystacktype*_T15D1;struct Cyc_Yystacktype _T15D2;struct Cyc_Yyltype _T15D3;unsigned _T15D4;unsigned _T15D5;struct Cyc_Absyn_Exp*_T15D6;struct Cyc_Yystacktype*_T15D7;union Cyc_YYSTYPE*_T15D8;union Cyc_YYSTYPE*_T15D9;struct Cyc_Absyn_Exp*_T15DA;struct Cyc_Yystacktype*_T15DB;union Cyc_YYSTYPE*_T15DC;union Cyc_YYSTYPE*_T15DD;struct Cyc_List_List*_T15DE;struct Cyc_Yystacktype*_T15DF;struct Cyc_Yystacktype _T15E0;struct Cyc_Yyltype _T15E1;unsigned _T15E2;unsigned _T15E3;struct Cyc_Absyn_Exp*_T15E4;struct Cyc_Yystacktype*_T15E5;union Cyc_YYSTYPE*_T15E6;union Cyc_YYSTYPE*_T15E7;struct Cyc_Absyn_Exp*_T15E8;struct _fat_ptr*_T15E9;struct Cyc_Yystacktype*_T15EA;union Cyc_YYSTYPE*_T15EB;union Cyc_YYSTYPE*_T15EC;struct Cyc_Yystacktype*_T15ED;struct Cyc_Yystacktype _T15EE;struct Cyc_Yyltype _T15EF;unsigned _T15F0;unsigned _T15F1;struct Cyc_Absyn_Exp*_T15F2;struct Cyc_Yystacktype*_T15F3;union Cyc_YYSTYPE*_T15F4;union Cyc_YYSTYPE*_T15F5;struct Cyc_Absyn_Exp*_T15F6;struct _fat_ptr*_T15F7;struct Cyc_Yystacktype*_T15F8;union Cyc_YYSTYPE*_T15F9;union Cyc_YYSTYPE*_T15FA;struct Cyc_Yystacktype*_T15FB;struct Cyc_Yystacktype _T15FC;struct Cyc_Yyltype _T15FD;unsigned _T15FE;unsigned _T15FF;struct Cyc_Absyn_Exp*_T1600;struct Cyc_Yystacktype*_T1601;union Cyc_YYSTYPE*_T1602;union Cyc_YYSTYPE*_T1603;struct Cyc_Absyn_Exp*_T1604;struct Cyc_Yystacktype*_T1605;struct Cyc_Yystacktype _T1606;struct Cyc_Yyltype _T1607;unsigned _T1608;unsigned _T1609;struct Cyc_Absyn_Exp*_T160A;struct Cyc_Yystacktype*_T160B;union Cyc_YYSTYPE*_T160C;union Cyc_YYSTYPE*_T160D;struct Cyc_Absyn_Exp*_T160E;struct Cyc_Yystacktype*_T160F;struct Cyc_Yystacktype _T1610;struct Cyc_Yyltype _T1611;unsigned _T1612;unsigned _T1613;struct Cyc_Absyn_Exp*_T1614;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1615;struct Cyc_Yystacktype*_T1616;union Cyc_YYSTYPE*_T1617;union Cyc_YYSTYPE*_T1618;void*_T1619;struct Cyc_Yystacktype*_T161A;struct Cyc_Yystacktype _T161B;struct Cyc_Yyltype _T161C;unsigned _T161D;unsigned _T161E;struct Cyc_Absyn_Exp*_T161F;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1620;struct Cyc_Yystacktype*_T1621;union Cyc_YYSTYPE*_T1622;union Cyc_YYSTYPE*_T1623;struct Cyc_Yystacktype*_T1624;union Cyc_YYSTYPE*_T1625;union Cyc_YYSTYPE*_T1626;struct Cyc_List_List*_T1627;void*_T1628;struct Cyc_Yystacktype*_T1629;struct Cyc_Yystacktype _T162A;struct Cyc_Yyltype _T162B;unsigned _T162C;unsigned _T162D;struct Cyc_Absyn_Exp*_T162E;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T162F;struct Cyc_Yystacktype*_T1630;union Cyc_YYSTYPE*_T1631;union Cyc_YYSTYPE*_T1632;struct Cyc_Yystacktype*_T1633;union Cyc_YYSTYPE*_T1634;union Cyc_YYSTYPE*_T1635;struct Cyc_List_List*_T1636;void*_T1637;struct Cyc_Yystacktype*_T1638;struct Cyc_Yystacktype _T1639;struct Cyc_Yyltype _T163A;unsigned _T163B;unsigned _T163C;struct Cyc_Absyn_Exp*_T163D;struct Cyc_List_List*_T163E;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T163F;struct _fat_ptr*_T1640;struct Cyc_Yystacktype*_T1641;union Cyc_YYSTYPE*_T1642;union Cyc_YYSTYPE*_T1643;struct Cyc_Yystacktype*_T1644;struct Cyc_Yystacktype _T1645;struct Cyc_Yyltype _T1646;unsigned _T1647;unsigned _T1648;struct Cyc_Yystacktype*_T1649;union Cyc_YYSTYPE*_T164A;union Cyc_YYSTYPE*_T164B;union Cyc_Absyn_Cnst _T164C;unsigned _T164D;int _T164E;struct Cyc_List_List*_T164F;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1650;struct Cyc_List_List*_T1651;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1652;struct _fat_ptr*_T1653;struct Cyc_Yystacktype*_T1654;union Cyc_YYSTYPE*_T1655;union Cyc_YYSTYPE*_T1656;struct Cyc_Yystacktype*_T1657;union Cyc_YYSTYPE*_T1658;union Cyc_YYSTYPE*_T1659;struct Cyc_Yystacktype*_T165A;struct Cyc_Yystacktype _T165B;struct Cyc_Yyltype _T165C;unsigned _T165D;unsigned _T165E;struct Cyc_Yystacktype*_T165F;union Cyc_YYSTYPE*_T1660;union Cyc_YYSTYPE*_T1661;union Cyc_Absyn_Cnst _T1662;unsigned _T1663;int _T1664;struct Cyc_List_List*_T1665;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1666;struct Cyc_Yystacktype*_T1667;union Cyc_YYSTYPE*_T1668;union Cyc_YYSTYPE*_T1669;struct Cyc_Yystacktype*_T166A;union Cyc_YYSTYPE*_T166B;union Cyc_YYSTYPE*_T166C;struct _tuple0*_T166D;struct Cyc_Yystacktype*_T166E;struct Cyc_Yystacktype _T166F;struct Cyc_Yyltype _T1670;unsigned _T1671;unsigned _T1672;struct Cyc_Absyn_Exp*_T1673;struct Cyc_Yystacktype*_T1674;union Cyc_YYSTYPE*_T1675;union Cyc_YYSTYPE*_T1676;struct _fat_ptr _T1677;struct Cyc_Yystacktype*_T1678;struct Cyc_Yystacktype _T1679;struct Cyc_Yyltype _T167A;unsigned _T167B;unsigned _T167C;struct Cyc_Absyn_Exp*_T167D;struct Cyc_Yystacktype*_T167E;struct Cyc_Yystacktype _T167F;struct Cyc_Yystacktype*_T1680;union Cyc_YYSTYPE*_T1681;union Cyc_YYSTYPE*_T1682;struct _fat_ptr _T1683;struct Cyc_Yystacktype*_T1684;struct Cyc_Yystacktype _T1685;struct Cyc_Yyltype _T1686;unsigned _T1687;unsigned _T1688;struct Cyc_Absyn_Exp*_T1689;struct Cyc_Yystacktype*_T168A;union Cyc_YYSTYPE*_T168B;union Cyc_YYSTYPE*_T168C;struct _fat_ptr _T168D;struct Cyc_Yystacktype*_T168E;struct Cyc_Yystacktype _T168F;struct Cyc_Yyltype _T1690;unsigned _T1691;unsigned _T1692;struct Cyc_Absyn_Exp*_T1693;struct Cyc_Yystacktype*_T1694;struct Cyc_Yystacktype _T1695;struct Cyc_Yystacktype*_T1696;union Cyc_YYSTYPE*_T1697;union Cyc_YYSTYPE*_T1698;struct Cyc_Absyn_Exp*_T1699;struct Cyc_Yystacktype*_T169A;struct Cyc_Yystacktype _T169B;struct Cyc_Yyltype _T169C;unsigned _T169D;unsigned _T169E;struct Cyc_Absyn_Exp*_T169F;struct Cyc_Yystacktype*_T16A0;union Cyc_YYSTYPE*_T16A1;union Cyc_YYSTYPE*_T16A2;struct Cyc_Absyn_Exp*_T16A3;struct Cyc_Yystacktype*_T16A4;union Cyc_YYSTYPE*_T16A5;union Cyc_YYSTYPE*_T16A6;struct Cyc_List_List*_T16A7;struct Cyc_List_List*_T16A8;struct Cyc_Yystacktype*_T16A9;struct Cyc_Yystacktype _T16AA;struct Cyc_Yyltype _T16AB;unsigned _T16AC;unsigned _T16AD;struct Cyc_Absyn_Exp*_T16AE;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T16AF;struct Cyc_Yystacktype*_T16B0;union Cyc_YYSTYPE*_T16B1;union Cyc_YYSTYPE*_T16B2;struct Cyc_Yystacktype*_T16B3;union Cyc_YYSTYPE*_T16B4;union Cyc_YYSTYPE*_T16B5;struct Cyc_Yystacktype*_T16B6;union Cyc_YYSTYPE*_T16B7;union Cyc_YYSTYPE*_T16B8;struct Cyc_List_List*_T16B9;void*_T16BA;struct Cyc_Yystacktype*_T16BB;struct Cyc_Yystacktype _T16BC;struct Cyc_Yyltype _T16BD;unsigned _T16BE;unsigned _T16BF;struct Cyc_Absyn_Exp*_T16C0;struct Cyc_Yystacktype*_T16C1;union Cyc_YYSTYPE*_T16C2;union Cyc_YYSTYPE*_T16C3;struct Cyc_List_List*_T16C4;struct Cyc_Yystacktype*_T16C5;struct Cyc_Yystacktype _T16C6;struct Cyc_Yyltype _T16C7;unsigned _T16C8;unsigned _T16C9;struct Cyc_Absyn_Exp*_T16CA;struct Cyc_Yystacktype*_T16CB;union Cyc_YYSTYPE*_T16CC;union Cyc_YYSTYPE*_T16CD;struct Cyc_Absyn_Stmt*_T16CE;struct Cyc_Yystacktype*_T16CF;struct Cyc_Yystacktype _T16D0;struct Cyc_Yyltype _T16D1;unsigned _T16D2;unsigned _T16D3;struct Cyc_Absyn_Exp*_T16D4;struct Cyc_Yystacktype*_T16D5;union Cyc_YYSTYPE*_T16D6;union Cyc_YYSTYPE*_T16D7;struct Cyc_List_List*_T16D8;struct Cyc_List_List*_T16D9;struct Cyc_List_List*_T16DA;struct Cyc_Yystacktype*_T16DB;union Cyc_YYSTYPE*_T16DC;union Cyc_YYSTYPE*_T16DD;struct Cyc_List_List*_T16DE;struct Cyc_Yystacktype*_T16DF;union Cyc_YYSTYPE*_T16E0;union Cyc_YYSTYPE*_T16E1;struct Cyc_Yystacktype*_T16E2;union Cyc_YYSTYPE*_T16E3;union Cyc_YYSTYPE*_T16E4;struct Cyc_Yystacktype*_T16E5;union Cyc_YYSTYPE*_T16E6;union Cyc_YYSTYPE*_T16E7;union Cyc_Absyn_Cnst _T16E8;struct Cyc_Yystacktype*_T16E9;struct Cyc_Yystacktype _T16EA;struct Cyc_Yyltype _T16EB;unsigned _T16EC;unsigned _T16ED;struct Cyc_Absyn_Exp*_T16EE;struct Cyc_Yystacktype*_T16EF;union Cyc_YYSTYPE*_T16F0;union Cyc_YYSTYPE*_T16F1;char _T16F2;struct Cyc_Yystacktype*_T16F3;struct Cyc_Yystacktype _T16F4;struct Cyc_Yyltype _T16F5;unsigned _T16F6;unsigned _T16F7;struct Cyc_Absyn_Exp*_T16F8;struct Cyc_Yystacktype*_T16F9;union Cyc_YYSTYPE*_T16FA;union Cyc_YYSTYPE*_T16FB;struct _fat_ptr _T16FC;struct Cyc_Yystacktype*_T16FD;struct Cyc_Yystacktype _T16FE;struct Cyc_Yyltype _T16FF;unsigned _T1700;unsigned _T1701;struct Cyc_Absyn_Exp*_T1702;struct Cyc_Yystacktype*_T1703;struct Cyc_Yystacktype _T1704;struct Cyc_Yyltype _T1705;unsigned _T1706;unsigned _T1707;struct Cyc_Absyn_Exp*_T1708;struct Cyc_Yystacktype*_T1709;union Cyc_YYSTYPE*_T170A;union Cyc_YYSTYPE*_T170B;unsigned long _T170C;struct _fat_ptr _T170D;unsigned char*_T170E;const char*_T170F;int _T1710;char _T1711;int _T1712;char _T1713;int _T1714;char _T1715;int _T1716;char _T1717;int _T1718;struct _fat_ptr _T1719;int _T171A;struct Cyc_Yystacktype*_T171B;struct Cyc_Yystacktype _T171C;struct Cyc_Yyltype _T171D;unsigned _T171E;unsigned _T171F;struct Cyc_Absyn_Exp*_T1720;struct _tuple0*_T1721;struct _fat_ptr*_T1722;struct Cyc_Yystacktype*_T1723;union Cyc_YYSTYPE*_T1724;union Cyc_YYSTYPE*_T1725;struct Cyc_Yystacktype*_T1726;struct Cyc_Yystacktype _T1727;struct _tuple0*_T1728;struct _fat_ptr*_T1729;struct Cyc_Yystacktype*_T172A;union Cyc_YYSTYPE*_T172B;union Cyc_YYSTYPE*_T172C;struct Cyc_Yystacktype*_T172D;struct Cyc_Yystacktype _T172E;struct Cyc_Yystacktype*_T172F;struct Cyc_Yystacktype _T1730;struct Cyc_Yystacktype*_T1731;struct Cyc_Yystacktype _T1732;struct Cyc_Yystacktype*_T1733;struct Cyc_Yystacktype _T1734;struct Cyc_Yystacktype*_T1735;struct Cyc_Yystacktype _T1736;struct Cyc_Lexing_lexbuf*_T1737;struct Cyc_List_List*_T1738;struct _tuple35*_T1739;struct Cyc_Yystacktype*_T173A;union Cyc_YYSTYPE*_T173B;union Cyc_YYSTYPE*_T173C;struct _fat_ptr _T173D;struct Cyc_Yystacktype*_T173E;union Cyc_YYSTYPE*_T173F;union Cyc_YYSTYPE*_T1740;struct _fat_ptr _T1741;struct _fat_ptr _T1742;int _T1743;int _T1744;struct Cyc_Yystacktype*_T1745;union Cyc_YYSTYPE*_T1746;union Cyc_YYSTYPE*_T1747;struct _fat_ptr _T1748;void*_T1749;struct Cyc_Yystacktype*_T174A;union Cyc_YYSTYPE*_T174B;union Cyc_YYSTYPE*_T174C;struct Cyc_Yystacktype*_T174D;union Cyc_YYSTYPE*_T174E;union Cyc_YYSTYPE*_T174F;struct Cyc_Yystacktype*_T1750;union Cyc_YYSTYPE*_T1751;union Cyc_YYSTYPE*_T1752;struct Cyc_List_List*_T1753;struct Cyc_List_List*_T1754;struct Cyc_Yystacktype*_T1755;union Cyc_YYSTYPE*_T1756;union Cyc_YYSTYPE*_T1757;struct Cyc_List_List*_T1758;struct Cyc_Yystacktype*_T1759;union Cyc_YYSTYPE*_T175A;union Cyc_YYSTYPE*_T175B;struct Cyc_Yystacktype*_T175C;union Cyc_YYSTYPE*_T175D;union Cyc_YYSTYPE*_T175E;struct Cyc_Yystacktype*_T175F;union Cyc_YYSTYPE*_T1760;union Cyc_YYSTYPE*_T1761;struct _fat_ptr _T1762;void*_T1763;struct Cyc_Yystacktype*_T1764;struct Cyc_Yystacktype _T1765;struct Cyc_Yyltype _T1766;unsigned _T1767;unsigned _T1768;struct Cyc_Yystacktype*_T1769;union Cyc_YYSTYPE*_T176A;union Cyc_YYSTYPE*_T176B;struct _fat_ptr _T176C;void*_T176D;struct Cyc_Yystacktype*_T176E;union Cyc_YYSTYPE*_T176F;union Cyc_YYSTYPE*_T1770;struct _fat_ptr _T1771;struct Cyc_Yystacktype*_T1772;union Cyc_YYSTYPE*_T1773;union Cyc_YYSTYPE*_T1774;void*_T1775;void*_T1776;void*_T1777;struct Cyc_Yystacktype*_T1778;union Cyc_YYSTYPE*_T1779;union Cyc_YYSTYPE*_T177A;struct _fat_ptr _T177B;struct Cyc_Yystacktype*_T177C;union Cyc_YYSTYPE*_T177D;union Cyc_YYSTYPE*_T177E;enum Cyc_Parse_ConstraintOps _T177F;struct Cyc_Yystacktype*_T1780;union Cyc_YYSTYPE*_T1781;union Cyc_YYSTYPE*_T1782;void*_T1783;struct Cyc_Yystacktype*_T1784;union Cyc_YYSTYPE*_T1785;union Cyc_YYSTYPE*_T1786;void*_T1787;void*_T1788;void*_T1789;struct Cyc_Yystacktype*_T178A;union Cyc_YYSTYPE*_T178B;union Cyc_YYSTYPE*_T178C;struct _fat_ptr _T178D;struct Cyc_Yystacktype*_T178E;union Cyc_YYSTYPE*_T178F;union Cyc_YYSTYPE*_T1790;void*_T1791;struct Cyc_Yystacktype*_T1792;union Cyc_YYSTYPE*_T1793;union Cyc_YYSTYPE*_T1794;void*_T1795;void*_T1796;void*_T1797;struct Cyc_Yystacktype*_T1798;union Cyc_YYSTYPE*_T1799;union Cyc_YYSTYPE*_T179A;struct _fat_ptr _T179B;struct Cyc_Yystacktype*_T179C;union Cyc_YYSTYPE*_T179D;union Cyc_YYSTYPE*_T179E;void*_T179F;struct Cyc_Yystacktype*_T17A0;union Cyc_YYSTYPE*_T17A1;union Cyc_YYSTYPE*_T17A2;void*_T17A3;void*_T17A4;void*_T17A5;struct Cyc_Yystacktype*_T17A6;union Cyc_YYSTYPE*_T17A7;union Cyc_YYSTYPE*_T17A8;struct _fat_ptr _T17A9;struct Cyc_Yystacktype*_T17AA;union Cyc_YYSTYPE*_T17AB;union Cyc_YYSTYPE*_T17AC;void*_T17AD;struct Cyc_Yystacktype*_T17AE;union Cyc_YYSTYPE*_T17AF;union Cyc_YYSTYPE*_T17B0;void*_T17B1;void*_T17B2;void*_T17B3;struct Cyc_Yystacktype*_T17B4;union Cyc_YYSTYPE*_T17B5;union Cyc_YYSTYPE*_T17B6;struct _fat_ptr _T17B7;struct Cyc_Yystacktype*_T17B8;union Cyc_YYSTYPE*_T17B9;union Cyc_YYSTYPE*_T17BA;void*_T17BB;struct Cyc_Yystacktype*_T17BC;union Cyc_YYSTYPE*_T17BD;union Cyc_YYSTYPE*_T17BE;void*_T17BF;void*_T17C0;void*_T17C1;struct Cyc_Yystacktype*_T17C2;union Cyc_YYSTYPE*_T17C3;union Cyc_YYSTYPE*_T17C4;struct _fat_ptr _T17C5;struct Cyc_Yystacktype*_T17C6;union Cyc_YYSTYPE*_T17C7;union Cyc_YYSTYPE*_T17C8;enum Cyc_Parse_ConstraintOps _T17C9;struct Cyc_Yystacktype*_T17CA;union Cyc_YYSTYPE*_T17CB;union Cyc_YYSTYPE*_T17CC;void*_T17CD;struct Cyc_Yystacktype*_T17CE;union Cyc_YYSTYPE*_T17CF;union Cyc_YYSTYPE*_T17D0;void*_T17D1;void*_T17D2;void*_T17D3;int _T17D4;int _T17D5;struct _fat_ptr _T17D6;int _T17D7;char*_T17D8;struct Cyc_Yystacktype*_T17D9;struct _fat_ptr _T17DA;int _T17DB;struct _fat_ptr _T17DC;char*_T17DD;char*_T17DE;struct Cyc_Yystacktype*_T17DF;struct Cyc_Yyltype _T17E0;struct Cyc_Yystacktype*_T17E1;struct Cyc_Yyltype _T17E2;struct Cyc_Yystacktype*_T17E3;struct Cyc_Yystacktype*_T17E4;struct Cyc_Yystacktype _T17E5;struct Cyc_Yyltype _T17E6;struct Cyc_Yystacktype*_T17E7;struct Cyc_Yystacktype*_T17E8;struct Cyc_Yystacktype _T17E9;struct Cyc_Yyltype _T17EA;struct _fat_ptr _T17EB;unsigned char*_T17EC;struct Cyc_Yystacktype*_T17ED;int _T17EE;struct _fat_ptr _T17EF;int _T17F0;int _T17F1;char*_T17F2;struct Cyc_Yystacktype*_T17F3;struct Cyc_Yystacktype _T17F4;struct Cyc_Yyltype _T17F5;struct _fat_ptr _T17F6;unsigned char*_T17F7;struct Cyc_Yystacktype*_T17F8;int _T17F9;struct _fat_ptr _T17FA;unsigned char*_T17FB;struct Cyc_Yystacktype*_T17FC;int _T17FD;int _T17FE;struct Cyc_Yystacktype _T17FF;struct Cyc_Yyltype _T1800;short*_T1801;int _T1802;char*_T1803;short*_T1804;short _T1805;short*_T1806;int _T1807;char*_T1808;short*_T1809;short _T180A;int _T180B;struct _fat_ptr _T180C;int _T180D;char*_T180E;short*_T180F;short _T1810;int _T1811;short*_T1812;int _T1813;short _T1814;int _T1815;struct _fat_ptr _T1816;unsigned char*_T1817;short*_T1818;int _T1819;short _T181A;int _T181B;short*_T181C;int _T181D;short _T181E;short*_T181F;int _T1820;short _T1821;short*_T1822;int _T1823;char*_T1824;short*_T1825;short _T1826;int _T1827;int _T1828;int _T1829;int _T182A;unsigned _T182B;unsigned _T182C;short*_T182D;int _T182E;char*_T182F;short*_T1830;short _T1831;int _T1832;int _T1833;struct _fat_ptr*_T1834;int _T1835;char*_T1836;struct _fat_ptr*_T1837;struct _fat_ptr _T1838;unsigned long _T1839;unsigned long _T183A;struct _fat_ptr _T183B;int _T183C;unsigned _T183D;char*_T183E;struct _RegionHandle*_T183F;unsigned _T1840;int _T1841;unsigned _T1842;struct _fat_ptr _T1843;struct _fat_ptr _T1844;int _T1845;int _T1846;unsigned _T1847;unsigned _T1848;short*_T1849;int _T184A;char*_T184B;short*_T184C;short _T184D;int _T184E;int _T184F;struct _fat_ptr _T1850;struct _fat_ptr _T1851;struct _fat_ptr _T1852;struct _fat_ptr _T1853;struct _fat_ptr _T1854;struct _fat_ptr*_T1855;int _T1856;char*_T1857;struct _fat_ptr*_T1858;struct _fat_ptr _T1859;struct _fat_ptr _T185A;struct _fat_ptr _T185B;struct _fat_ptr _T185C;int _T185D;int _T185E;struct _fat_ptr _T185F;int _T1860;char*_T1861;short*_T1862;short _T1863;short*_T1864;int _T1865;char*_T1866;short*_T1867;short _T1868;int _T1869;int _T186A;short*_T186B;int _T186C;short _T186D;int _T186E;short*_T186F;int _T1870;short _T1871;int _T1872;int _T1873;struct _fat_ptr _T1874;int _T1875;char*_T1876;struct Cyc_Yystacktype*_T1877;struct Cyc_Yystacktype _T1878;struct _RegionHandle _T1879=_new_region(0U,"yyregion");struct _RegionHandle*yyregion=& _T1879;_push_region(yyregion);{
# 149
int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;{union Cyc_YYSTYPE _T187A;_T187A.YYINITIALSVAL.tag=1U;
_T187A.YYINITIALSVAL.val=0;_T0=_T187A;}{union Cyc_YYSTYPE yylval=_T0;
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;{unsigned _T187A=200U;_T2=yyregion;_T3=_region_calloc(_T2,0U,sizeof(short),_T187A);_T1=_tag_fat(_T3,sizeof(short),_T187A);}{
# 164
struct _fat_ptr yyss=_T1;
# 166
int yyvsp_offset;{unsigned _T187A=200U;_T6=yyregion;_T7=_check_times(_T187A,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T187B=_region_malloc(_T6,0U,_T7);{unsigned _T187C=200U;unsigned i;i=0;_TL250: if(i < _T187C)goto _TL24E;else{goto _TL24F;}_TL24E: _T8=i;
# 169
_T187B[_T8].v=yylval;_T9=i;_T187B[_T9].l=Cyc_yynewloc();i=i + 1;goto _TL250;_TL24F:;}_T5=(struct Cyc_Yystacktype*)_T187B;}_T4=_T5;}{
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
yystacksize=10000;goto _TL256;_TL255: _TL256: _T18=yystacksize;{unsigned _T187A=(unsigned)_T18;_T1A=yyregion;_T1B=_check_times(_T187A,sizeof(short));{short*_T187B=_region_malloc(_T1A,0U,_T1B);_T1C=yystacksize;{unsigned _T187C=(unsigned)_T1C;unsigned i;i=0;_TL25A: if(i < _T187C)goto _TL258;else{goto _TL259;}_TL258: _T1D=i;_T1E=yyssp_offset;_T1F=(unsigned)_T1E;
# 220
if(_T1D > _T1F)goto _TL25B;_T20=i;_T21=yyss;_T22=_T21.curr;_T23=(short*)_T22;_T24=i;_T25=(int)_T24;_T187B[_T20]=_T23[_T25];goto _TL25C;_TL25B: _T26=i;_T187B[_T26]=0;_TL25C: i=i + 1;goto _TL25A;_TL259:;}_T19=(short*)_T187B;}_T17=
# 219
_tag_fat(_T19,sizeof(short),_T187A);}{struct _fat_ptr yyss1=_T17;_T28=yystacksize;{unsigned _T187A=(unsigned)_T28;_T2A=yyregion;_T2B=_check_times(_T187A,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T187B=_region_malloc(_T2A,0U,_T2B);_T2C=yystacksize;{unsigned _T187C=(unsigned)_T2C;unsigned i;i=0;_TL260: if(i < _T187C)goto _TL25E;else{goto _TL25F;}_TL25E: _T2D=i;_T2E=yyssp_offset;_T2F=(unsigned)_T2E;
# 224
if(_T2D > _T2F)goto _TL261;_T30=i;_T31=yyvs;_T32=i;_T33=(int)_T32;_T34=_check_fat_subscript(_T31,sizeof(struct Cyc_Yystacktype),_T33);_T35=(struct Cyc_Yystacktype*)_T34;_T187B[_T30]=*_T35;goto _TL262;_TL261: _T36=i;_T37=yyvs;_T38=_check_fat_subscript(_T37,sizeof(struct Cyc_Yystacktype),0);_T39=(struct Cyc_Yystacktype*)_T38;_T187B[_T36]=*_T39;_TL262: i=i + 1;goto _TL260;_TL25F:;}_T29=(struct Cyc_Yystacktype*)_T187B;}_T27=
# 223
_tag_fat(_T29,sizeof(struct Cyc_Yystacktype),_T187A);}{
# 222
struct _fat_ptr yyvs1=_T27;
# 230
yyss=yyss1;
yyvs=yyvs1;}}goto _TL252;_TL251: _TL252: goto yybackup;
# 242
yybackup: _T3A=Cyc_yypact;_T3B=yystate;_T3C=_check_known_subscript_notnull(_T3A,1262U,sizeof(short),_T3B);_T3D=(short*)_T3C;_T3E=*_T3D;
# 254 "cycbison.simple"
yyn=(int)_T3E;_T3F=yyn;_T40=- 32768;
if(_T3F!=_T40)goto _TL263;goto yydefault;_TL263: _T41=yychar;_T42=- 2;
# 261
if(_T41!=_T42)goto _TL265;_T43=yylex_buf;_T44=& yylval;_T45=(union Cyc_YYSTYPE*)_T44;_T46=& yylloc;_T47=(struct Cyc_Yyltype*)_T46;
# 267
yychar=Cyc_yylex(_T43,_T45,_T47);goto _TL266;_TL265: _TL266:
# 271
 if(yychar > 0)goto _TL267;
# 273
yychar1=0;
yychar=0;goto _TL268;
# 282
_TL267: if(yychar <= 0)goto _TL269;if(yychar > 400)goto _TL269;_T49=Cyc_yytranslate;_T4A=yychar;_T4B=_T49[_T4A];_T48=(int)_T4B;goto _TL26A;_TL269: _T48=363;_TL26A: yychar1=_T48;_TL268: _T4C=yychar1;
# 299 "cycbison.simple"
yyn=yyn + _T4C;
if(yyn < 0)goto _TL26D;else{goto _TL26F;}_TL26F: if(yyn > 8241)goto _TL26D;else{goto _TL26E;}_TL26E: _T4D=Cyc_yycheck;_T4E=yyn;_T4F=_T4D[_T4E];_T50=(int)_T4F;_T51=yychar1;if(_T50!=_T51)goto _TL26D;else{goto _TL26B;}_TL26D: goto yydefault;_TL26B: _T52=Cyc_yytable;_T53=yyn;_T54=_T52[_T53];
# 302
yyn=(int)_T54;
# 309
if(yyn >= 0)goto _TL270;_T55=yyn;_T56=- 32768;
# 311
if(_T55!=_T56)goto _TL272;goto yyerrlab;_TL272:
 yyn=- yyn;goto yyreduce;
# 315
_TL270: if(yyn!=0)goto _TL274;goto yyerrlab;_TL274:
# 317
 if(yyn!=1261)goto _TL276;{int _T187A=0;_npop_handler(0);return _T187A;}_TL276:
# 328 "cycbison.simple"
 if(yychar==0)goto _TL278;
yychar=- 2;goto _TL279;_TL278: _TL279: _T57=yyvs;
# 332
yyvsp_offset=yyvsp_offset + 1;_T58=yyvsp_offset;_T59=_check_fat_subscript(_T57,sizeof(struct Cyc_Yystacktype),_T58);_T5A=(struct Cyc_Yystacktype*)_T59;{struct Cyc_Yystacktype _T187A;_T187A.v=yylval;_T187A.l=yylloc;_T5B=_T187A;}*_T5A=_T5B;
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
 yyval=Cyc_YY74(1);_T71=yyyvsp;_T72=& _T71[0].v;_T73=(union Cyc_YYSTYPE*)_T72;
Cyc_Parse_constraint_graph=Cyc_yyget_YY71(_T73);goto _LL0;case 3: _T74=yyyvsp;_T75=_T74[0];
# 1333 "parse.y"
yyval=_T75.v;_T76=yyyvsp;_T77=& _T76[0].v;_T78=(union Cyc_YYSTYPE*)_T77;
Cyc_Parse_parse_result=Cyc_yyget_YY16(_T78);goto _LL0;case 4: _T79=yyyvsp;_T7A=& _T79[0].v;_T7B=(union Cyc_YYSTYPE*)_T7A;_T7C=
# 1339 "parse.y"
Cyc_yyget_YY16(_T7B);_T7D=yyyvsp;_T7E=& _T7D[1].v;_T7F=(union Cyc_YYSTYPE*)_T7E;_T80=Cyc_yyget_YY16(_T7F);_T81=Cyc_List_imp_append(_T7C,_T80);yyval=Cyc_YY16(_T81);goto _LL0;case 5:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T187C=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T187C->tag=10;_T85=yyyvsp;_T86=& _T85[0].v;_T87=(union Cyc_YYSTYPE*)_T86;
# 1343 "parse.y"
_T187C->f1=Cyc_yyget_QualId_tok(_T87);_T88=yyyvsp;_T89=& _T88[2].v;_T8A=(union Cyc_YYSTYPE*)_T89;_T187C->f2=Cyc_yyget_YY16(_T8A);_T84=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T187C;}_T187B->r=(void*)_T84;_T8B=yyyvsp;_T8C=_T8B[0];_T8D=_T8C.l;_T8E=_T8D.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_T8E);_T83=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_T83;_T187A->tl=0;_T82=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T82);
Cyc_Lex_leave_using();goto _LL0;case 6:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T187C=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T187C->tag=10;_T92=yyyvsp;_T93=& _T92[0].v;_T94=(union Cyc_YYSTYPE*)_T93;
# 1347 "parse.y"
_T187C->f1=Cyc_yyget_QualId_tok(_T94);_T95=yyyvsp;_T96=& _T95[2].v;_T97=(union Cyc_YYSTYPE*)_T96;_T187C->f2=Cyc_yyget_YY16(_T97);_T91=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T187C;}_T187B->r=(void*)_T91;_T98=yyyvsp;_T99=_T98[0];_T9A=_T99.l;_T9B=_T9A.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_T9B);_T90=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_T90;_T9C=yyyvsp;_T9D=& _T9C[4].v;_T9E=(union Cyc_YYSTYPE*)_T9D;_T187A->tl=Cyc_yyget_YY16(_T9E);_T8F=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T8F);goto _LL0;case 7:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T187C=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T187C->tag=9;{struct _fat_ptr*_T187D=_cycalloc(sizeof(struct _fat_ptr));_TA3=yyyvsp;_TA4=& _TA3[0].v;_TA5=(union Cyc_YYSTYPE*)_TA4;
# 1350
*_T187D=Cyc_yyget_String_tok(_TA5);_TA2=(struct _fat_ptr*)_T187D;}_T187C->f1=_TA2;_TA6=yyyvsp;_TA7=& _TA6[2].v;_TA8=(union Cyc_YYSTYPE*)_TA7;_T187C->f2=Cyc_yyget_YY16(_TA8);_TA1=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T187C;}_T187B->r=(void*)_TA1;_TA9=yyyvsp;_TAA=_TA9[0];_TAB=_TAA.l;_TAC=_TAB.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_TAC);_TA0=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_TA0;_T187A->tl=0;_T9F=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T9F);
Cyc_Lex_leave_namespace();goto _LL0;case 8:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T187C=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T187C->tag=9;{struct _fat_ptr*_T187D=_cycalloc(sizeof(struct _fat_ptr));_TB1=yyyvsp;_TB2=& _TB1[0].v;_TB3=(union Cyc_YYSTYPE*)_TB2;
# 1354 "parse.y"
*_T187D=Cyc_yyget_String_tok(_TB3);_TB0=(struct _fat_ptr*)_T187D;}_T187C->f1=_TB0;_TB4=yyyvsp;_TB5=& _TB4[2].v;_TB6=(union Cyc_YYSTYPE*)_TB5;_T187C->f2=Cyc_yyget_YY16(_TB6);_TAF=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T187C;}_T187B->r=(void*)_TAF;_TB7=yyyvsp;_TB8=_TB7[0];_TB9=_TB8.l;_TBA=_TB9.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_TBA);_TAE=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_TAE;_TBB=yyyvsp;_TBC=& _TBB[4].v;_TBD=(union Cyc_YYSTYPE*)_TBC;_T187A->tl=Cyc_yyget_YY16(_TBD);_TAD=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_TAD);goto _LL0;case 9: _TBE=yyyvsp;_TBF=& _TBE[2].v;_TC0=(union Cyc_YYSTYPE*)_TBF;_TC1=
# 1356 "parse.y"
Cyc_yyget_YY16(_TC0);_TC2=yyyvsp;_TC3=& _TC2[4].v;_TC4=(union Cyc_YYSTYPE*)_TC3;_TC5=Cyc_yyget_YY16(_TC4);_TC6=Cyc_List_append(_TC1,_TC5);yyval=Cyc_YY16(_TC6);goto _LL0;case 10: _TC7=yyyvsp;_TC8=& _TC7[0].v;_TC9=(union Cyc_YYSTYPE*)_TC8;{
# 1358 "parse.y"
int is_c_include=Cyc_yyget_YY32(_TC9);_TCA=yyyvsp;_TCB=& _TCA[4].v;_TCC=(union Cyc_YYSTYPE*)_TCB;{
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(_TCC);_TCD=yyyvsp;_TCE=& _TCD[5].v;_TCF=(union Cyc_YYSTYPE*)_TCE;{
struct _tuple30*_T187A=Cyc_yyget_YY56(_TCF);unsigned _T187B;struct Cyc_List_List*_T187C;{struct _tuple30 _T187D=*_T187A;_T187C=_T187D.f0;_T187B=_T187D.f1;}{struct Cyc_List_List*exs=_T187C;unsigned wc=_T187B;_TD0=yyyvsp;_TD1=& _TD0[6].v;_TD2=(union Cyc_YYSTYPE*)_TD1;{
struct Cyc_List_List*hides=Cyc_yyget_YY57(_TD2);
if(exs==0)goto _TL281;if(hides==0)goto _TL281;_TD3=yyyvsp;_TD4=_TD3[0];_TD5=_TD4.l;_TD6=_TD5.first_line;_TD7=
Cyc_Position_loc_to_seg(_TD6);_TD8=_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U);_TD9=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TD7,_TD8,_TD9);goto _TL282;_TL281: _TL282: _TDA=hides;_TDB=(unsigned)_TDA;
if(!_TDB)goto _TL283;_TDC=wc;_TDD=(int)_TDC;if(_TDD)goto _TL283;else{goto _TL285;}
_TL285: _TDE=yyyvsp;_TDF=_TDE[6];_TE0=_TDF.l;_TE1=_TE0.first_line;wc=Cyc_Position_loc_to_seg(_TE1);goto _TL284;_TL283: _TL284: _TE2=is_c_include;
if(_TE2)goto _TL286;else{goto _TL288;}
_TL288: if(exs!=0)goto _TL28B;else{goto _TL28C;}_TL28C: if(cycdecls!=0)goto _TL28B;else{goto _TL289;}
_TL28B: _TE3=yyyvsp;_TE4=_TE3[0];_TE5=_TE4.l;_TE6=_TE5.first_line;_TE7=Cyc_Position_loc_to_seg(_TE6);_TE8=_tag_fat("expecting \"C include\"",sizeof(char),22U);_TE9=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TE7,_TE8,_TE9);{struct Cyc_List_List*_T187D=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187E=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T187F=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T187F->tag=12;_TED=yyyvsp;_TEE=& _TED[2].v;_TEF=(union Cyc_YYSTYPE*)_TEE;
_T187F->f1=Cyc_yyget_YY16(_TEF);_T187F->f2=cycdecls;_T187F->f3=exs;{struct _tuple10*_T1880=_cycalloc(sizeof(struct _tuple10));_T1880->f0=wc;_T1880->f1=hides;_TF0=(struct _tuple10*)_T1880;}_T187F->f4=_TF0;_TEC=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T187F;}_T187E->r=(void*)_TEC;_TF1=yyyvsp;_TF2=_TF1[0];_TF3=_TF2.l;_TF4=_TF3.first_line;_T187E->loc=Cyc_Position_loc_to_seg(_TF4);_TEB=(struct Cyc_Absyn_Decl*)_T187E;}_T187D->hd=_TEB;_TF5=yyyvsp;_TF6=& _TF5[7].v;_TF7=(union Cyc_YYSTYPE*)_TF6;_T187D->tl=Cyc_yyget_YY16(_TF7);_TEA=(struct Cyc_List_List*)_T187D;}yyval=Cyc_YY16(_TEA);goto _TL28A;
# 1372
_TL289:{struct Cyc_List_List*_T187D=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187E=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T187F=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_T187F->tag=11;_TFB=yyyvsp;_TFC=& _TFB[2].v;_TFD=(union Cyc_YYSTYPE*)_TFC;_T187F->f1=Cyc_yyget_YY16(_TFD);_TFA=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T187F;}_T187E->r=(void*)_TFA;_TFE=yyyvsp;_TFF=_TFE[0];_T100=_TFF.l;_T101=_T100.first_line;_T187E->loc=Cyc_Position_loc_to_seg(_T101);_TF9=(struct Cyc_Absyn_Decl*)_T187E;}_T187D->hd=_TF9;_T102=yyyvsp;_T103=& _T102[7].v;_T104=(union Cyc_YYSTYPE*)_T103;_T187D->tl=Cyc_yyget_YY16(_T104);_TF8=(struct Cyc_List_List*)_T187D;}yyval=Cyc_YY16(_TF8);_TL28A: goto _TL287;
# 1375
_TL286:{struct Cyc_List_List*_T187D=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187E=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T187F=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T187F->tag=12;_T108=yyyvsp;_T109=& _T108[2].v;_T10A=(union Cyc_YYSTYPE*)_T109;_T187F->f1=Cyc_yyget_YY16(_T10A);_T187F->f2=cycdecls;_T187F->f3=exs;{struct _tuple10*_T1880=_cycalloc(sizeof(struct _tuple10));_T1880->f0=wc;_T1880->f1=hides;_T10B=(struct _tuple10*)_T1880;}_T187F->f4=_T10B;_T107=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T187F;}_T187E->r=(void*)_T107;_T10C=yyyvsp;_T10D=_T10C[0];_T10E=_T10D.l;_T10F=_T10E.first_line;_T187E->loc=Cyc_Position_loc_to_seg(_T10F);_T106=(struct Cyc_Absyn_Decl*)_T187E;}_T187D->hd=_T106;_T110=yyyvsp;_T111=& _T110[7].v;_T112=(union Cyc_YYSTYPE*)_T111;_T187D->tl=Cyc_yyget_YY16(_T112);_T105=(struct Cyc_List_List*)_T187D;}yyval=Cyc_YY16(_T105);_TL287: goto _LL0;}}}}}case 11:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T115=& Cyc_Absyn_Porton_d_val;_T116=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_T115;
# 1378 "parse.y"
_T187B->r=(void*)_T116;_T117=yyyvsp;_T118=_T117[0];_T119=_T118.l;_T11A=_T119.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_T11A);_T114=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_T114;_T11B=yyyvsp;_T11C=& _T11B[2].v;_T11D=(union Cyc_YYSTYPE*)_T11C;_T187A->tl=Cyc_yyget_YY16(_T11D);_T113=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T113);goto _LL0;case 12:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T120=& Cyc_Absyn_Portoff_d_val;_T121=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_T120;
# 1380 "parse.y"
_T187B->r=(void*)_T121;_T122=yyyvsp;_T123=_T122[0];_T124=_T123.l;_T125=_T124.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_T125);_T11F=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_T11F;_T126=yyyvsp;_T127=& _T126[2].v;_T128=(union Cyc_YYSTYPE*)_T127;_T187A->tl=Cyc_yyget_YY16(_T128);_T11E=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T11E);goto _LL0;case 13:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T12B=& Cyc_Absyn_Tempeston_d_val;_T12C=(struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*)_T12B;
# 1382 "parse.y"
_T187B->r=(void*)_T12C;_T12D=yyyvsp;_T12E=_T12D[0];_T12F=_T12E.l;_T130=_T12F.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_T130);_T12A=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_T12A;_T131=yyyvsp;_T132=& _T131[2].v;_T133=(union Cyc_YYSTYPE*)_T132;_T187A->tl=Cyc_yyget_YY16(_T133);_T129=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T129);goto _LL0;case 14:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T136=& Cyc_Absyn_Tempestoff_d_val;_T137=(struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*)_T136;
# 1384 "parse.y"
_T187B->r=(void*)_T137;_T138=yyyvsp;_T139=_T138[0];_T13A=_T139.l;_T13B=_T13A.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_T13B);_T135=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_T135;_T13C=yyyvsp;_T13D=& _T13C[2].v;_T13E=(union Cyc_YYSTYPE*)_T13D;_T187A->tl=Cyc_yyget_YY16(_T13E);_T134=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T134);goto _LL0;case 15:
# 1385 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 16:
# 1389 "parse.y"
 Cyc_Parse_parsing_tempest=1;goto _LL0;case 17:
# 1392
 Cyc_Parse_parsing_tempest=0;goto _LL0;case 18:
# 1397 "parse.y"
 Cyc_Lex_enter_extern_c();_T13F=yyyvsp;_T140=& _T13F[1].v;_T141=(union Cyc_YYSTYPE*)_T140;_T142=
Cyc_yyget_String_tok(_T141);_T143=_tag_fat("C",sizeof(char),2U);_T144=Cyc_strcmp(_T142,_T143);if(_T144!=0)goto _TL28D;
yyval=Cyc_YY32(0);goto _TL28E;
_TL28D: _T145=yyyvsp;_T146=& _T145[1].v;_T147=(union Cyc_YYSTYPE*)_T146;_T148=Cyc_yyget_String_tok(_T147);_T149=_tag_fat("C include",sizeof(char),10U);_T14A=Cyc_strcmp(_T148,_T149);if(_T14A!=0)goto _TL28F;
yyval=Cyc_YY32(1);goto _TL290;
# 1403
_TL28F: _T14B=yyyvsp;_T14C=_T14B[0];_T14D=_T14C.l;_T14E=_T14D.first_line;_T14F=Cyc_Position_loc_to_seg(_T14E);_T150=_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U);_T151=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T14F,_T150,_T151);
yyval=Cyc_YY32(1);_TL290: _TL28E: goto _LL0;case 19:
# 1410 "parse.y"
 Cyc_Lex_leave_extern_c();goto _LL0;case 20:
# 1414 "parse.y"
 yyval=Cyc_YY57(0);goto _LL0;case 21: _T152=yyyvsp;_T153=_T152[2];
# 1415 "parse.y"
yyval=_T153.v;goto _LL0;case 22:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T155=yyyvsp;_T156=& _T155[0].v;_T157=(union Cyc_YYSTYPE*)_T156;
# 1419 "parse.y"
_T187A->hd=Cyc_yyget_QualId_tok(_T157);_T187A->tl=0;_T154=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY57(_T154);goto _LL0;case 23:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T159=yyyvsp;_T15A=& _T159[0].v;_T15B=(union Cyc_YYSTYPE*)_T15A;
# 1420 "parse.y"
_T187A->hd=Cyc_yyget_QualId_tok(_T15B);_T187A->tl=0;_T158=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY57(_T158);goto _LL0;case 24:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T15D=yyyvsp;_T15E=& _T15D[0].v;_T15F=(union Cyc_YYSTYPE*)_T15E;
# 1421 "parse.y"
_T187A->hd=Cyc_yyget_QualId_tok(_T15F);_T160=yyyvsp;_T161=& _T160[2].v;_T162=(union Cyc_YYSTYPE*)_T161;_T187A->tl=Cyc_yyget_YY57(_T162);_T15C=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY57(_T15C);goto _LL0;case 25:{struct _tuple30*_T187A=_cycalloc(sizeof(struct _tuple30));
# 1425 "parse.y"
_T187A->f0=0;_T187A->f1=0U;_T163=(struct _tuple30*)_T187A;}yyval=Cyc_YY56(_T163);goto _LL0;case 26: _T164=yyyvsp;_T165=_T164[0];
# 1426 "parse.y"
yyval=_T165.v;goto _LL0;case 27:{struct _tuple30*_T187A=_cycalloc(sizeof(struct _tuple30));_T167=yyyvsp;_T168=& _T167[2].v;_T169=(union Cyc_YYSTYPE*)_T168;
# 1430 "parse.y"
_T187A->f0=Cyc_yyget_YY55(_T169);_T187A->f1=0U;_T166=(struct _tuple30*)_T187A;}yyval=Cyc_YY56(_T166);goto _LL0;case 28:{struct _tuple30*_T187A=_cycalloc(sizeof(struct _tuple30));
# 1431 "parse.y"
_T187A->f0=0;_T187A->f1=0U;_T16A=(struct _tuple30*)_T187A;}yyval=Cyc_YY56(_T16A);goto _LL0;case 29:{struct _tuple30*_T187A=_cycalloc(sizeof(struct _tuple30));
# 1432 "parse.y"
_T187A->f0=0;_T16C=yyyvsp;_T16D=_T16C[0];_T16E=_T16D.l;_T16F=_T16E.first_line;_T187A->f1=Cyc_Position_loc_to_seg(_T16F);_T16B=(struct _tuple30*)_T187A;}yyval=Cyc_YY56(_T16B);goto _LL0;case 30:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T187B=_cycalloc(sizeof(struct _tuple33));_T172=yyyvsp;_T173=_T172[0];_T174=_T173.l;_T175=_T174.first_line;
# 1437 "parse.y"
_T187B->f0=Cyc_Position_loc_to_seg(_T175);_T176=yyyvsp;_T177=& _T176[0].v;_T178=(union Cyc_YYSTYPE*)_T177;_T187B->f1=Cyc_yyget_QualId_tok(_T178);_T187B->f2=0;_T171=(struct _tuple33*)_T187B;}_T187A->hd=_T171;_T187A->tl=0;_T170=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY55(_T170);goto _LL0;case 31:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T187B=_cycalloc(sizeof(struct _tuple33));_T17B=yyyvsp;_T17C=_T17B[0];_T17D=_T17C.l;_T17E=_T17D.first_line;
# 1439 "parse.y"
_T187B->f0=Cyc_Position_loc_to_seg(_T17E);_T17F=yyyvsp;_T180=& _T17F[0].v;_T181=(union Cyc_YYSTYPE*)_T180;_T187B->f1=Cyc_yyget_QualId_tok(_T181);_T187B->f2=0;_T17A=(struct _tuple33*)_T187B;}_T187A->hd=_T17A;_T182=yyyvsp;_T183=& _T182[2].v;_T184=(union Cyc_YYSTYPE*)_T183;_T187A->tl=Cyc_yyget_YY55(_T184);_T179=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY55(_T179);goto _LL0;case 32:
# 1443 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 33: _T185=yyyvsp;_T186=_T185[2];
# 1444 "parse.y"
yyval=_T186.v;goto _LL0;case 34:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T187B->tag=1;_T189=yyyvsp;_T18A=& _T189[0].v;_T18B=(union Cyc_YYSTYPE*)_T18A;
# 1448 "parse.y"
_T187B->f1=Cyc_yyget_YY15(_T18B);_T188=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T187B;}_T18C=(void*)_T188;_T18D=yyyvsp;_T18E=_T18D[0];_T18F=_T18E.l;_T190=_T18F.first_line;_T191=Cyc_Position_loc_to_seg(_T190);_T187A->hd=Cyc_Absyn_new_decl(_T18C,_T191);_T187A->tl=0;_T187=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T187);goto _LL0;case 35: _T192=yyyvsp;_T193=_T192[0];
# 1449 "parse.y"
yyval=_T193.v;goto _LL0;case 36:
# 1450 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 39: _T194=yyr;_T195=yyyvsp;_T196=& _T195[0].v;_T197=(union Cyc_YYSTYPE*)_T196;_T198=
# 1484 "parse.y"
Cyc_yyget_YY28(_T197);_T199=yyyvsp;_T19A=& _T199[1].v;_T19B=(union Cyc_YYSTYPE*)_T19A;_T19C=Cyc_yyget_Stmt_tok(_T19B);_T19D=yyyvsp;_T19E=_T19D[0];_T19F=_T19E.l;_T1A0=_T19F.first_line;_T1A1=Cyc_Position_loc_to_seg(_T1A0);_T1A2=Cyc_Parse_make_function(_T194,0,_T198,0,_T19C,_T1A1);yyval=Cyc_YY15(_T1A2);goto _LL0;case 40: _T1A3=yyyvsp;_T1A4=& _T1A3[0].v;_T1A5=(union Cyc_YYSTYPE*)_T1A4;{
# 1486 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1A5);_T1A6=yyr;_T1A7=& d;_T1A8=(struct Cyc_Parse_Declaration_spec*)_T1A7;_T1A9=yyyvsp;_T1AA=& _T1A9[1].v;_T1AB=(union Cyc_YYSTYPE*)_T1AA;_T1AC=
Cyc_yyget_YY28(_T1AB);_T1AD=yyyvsp;_T1AE=& _T1AD[2].v;_T1AF=(union Cyc_YYSTYPE*)_T1AE;_T1B0=Cyc_yyget_Stmt_tok(_T1AF);_T1B1=yyyvsp;_T1B2=_T1B1[0];_T1B3=_T1B2.l;_T1B4=_T1B3.first_line;_T1B5=Cyc_Position_loc_to_seg(_T1B4);_T1B6=Cyc_Parse_make_function(_T1A6,_T1A8,_T1AC,0,_T1B0,_T1B5);yyval=Cyc_YY15(_T1B6);goto _LL0;}case 41: _T1B7=yyr;_T1B8=yyyvsp;_T1B9=& _T1B8[0].v;_T1BA=(union Cyc_YYSTYPE*)_T1B9;_T1BB=
# 1498 "parse.y"
Cyc_yyget_YY28(_T1BA);_T1BC=yyyvsp;_T1BD=& _T1BC[1].v;_T1BE=(union Cyc_YYSTYPE*)_T1BD;_T1BF=Cyc_yyget_YY16(_T1BE);_T1C0=yyyvsp;_T1C1=& _T1C0[2].v;_T1C2=(union Cyc_YYSTYPE*)_T1C1;_T1C3=Cyc_yyget_Stmt_tok(_T1C2);_T1C4=yyyvsp;_T1C5=_T1C4[0];_T1C6=_T1C5.l;_T1C7=_T1C6.first_line;_T1C8=Cyc_Position_loc_to_seg(_T1C7);_T1C9=Cyc_Parse_make_function(_T1B7,0,_T1BB,_T1BF,_T1C3,_T1C8);yyval=Cyc_YY15(_T1C9);goto _LL0;case 42: _T1CA=yyyvsp;_T1CB=& _T1CA[0].v;_T1CC=(union Cyc_YYSTYPE*)_T1CB;{
# 1500 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1CC);_T1CD=yyr;_T1CE=& d;_T1CF=(struct Cyc_Parse_Declaration_spec*)_T1CE;_T1D0=yyyvsp;_T1D1=& _T1D0[1].v;_T1D2=(union Cyc_YYSTYPE*)_T1D1;_T1D3=
Cyc_yyget_YY28(_T1D2);_T1D4=yyyvsp;_T1D5=& _T1D4[2].v;_T1D6=(union Cyc_YYSTYPE*)_T1D5;_T1D7=Cyc_yyget_YY16(_T1D6);_T1D8=yyyvsp;_T1D9=& _T1D8[3].v;_T1DA=(union Cyc_YYSTYPE*)_T1D9;_T1DB=Cyc_yyget_Stmt_tok(_T1DA);_T1DC=yyyvsp;_T1DD=_T1DC[0];_T1DE=_T1DD.l;_T1DF=_T1DE.first_line;_T1E0=Cyc_Position_loc_to_seg(_T1DF);_T1E1=Cyc_Parse_make_function(_T1CD,_T1CF,_T1D3,_T1D7,_T1DB,_T1E0);yyval=Cyc_YY15(_T1E1);goto _LL0;}case 43: _T1E2=yyyvsp;_T1E3=& _T1E2[0].v;_T1E4=(union Cyc_YYSTYPE*)_T1E3;{
# 1508 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1E4);_T1E5=yyr;_T1E6=& d;_T1E7=(struct Cyc_Parse_Declaration_spec*)_T1E6;_T1E8=yyyvsp;_T1E9=& _T1E8[1].v;_T1EA=(union Cyc_YYSTYPE*)_T1E9;_T1EB=
Cyc_yyget_YY28(_T1EA);_T1EC=yyyvsp;_T1ED=& _T1EC[2].v;_T1EE=(union Cyc_YYSTYPE*)_T1ED;_T1EF=Cyc_yyget_Stmt_tok(_T1EE);_T1F0=yyyvsp;_T1F1=_T1F0[0];_T1F2=_T1F1.l;_T1F3=_T1F2.first_line;_T1F4=Cyc_Position_loc_to_seg(_T1F3);_T1F5=Cyc_Parse_make_function(_T1E5,_T1E7,_T1EB,0,_T1EF,_T1F4);yyval=Cyc_YY15(_T1F5);goto _LL0;}case 44: _T1F6=yyyvsp;_T1F7=& _T1F6[0].v;_T1F8=(union Cyc_YYSTYPE*)_T1F7;{
# 1511 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1F8);_T1F9=yyr;_T1FA=& d;_T1FB=(struct Cyc_Parse_Declaration_spec*)_T1FA;_T1FC=yyyvsp;_T1FD=& _T1FC[1].v;_T1FE=(union Cyc_YYSTYPE*)_T1FD;_T1FF=
Cyc_yyget_YY28(_T1FE);_T200=yyyvsp;_T201=& _T200[2].v;_T202=(union Cyc_YYSTYPE*)_T201;_T203=Cyc_yyget_YY16(_T202);_T204=yyyvsp;_T205=& _T204[3].v;_T206=(union Cyc_YYSTYPE*)_T205;_T207=Cyc_yyget_Stmt_tok(_T206);_T208=yyyvsp;_T209=_T208[0];_T20A=_T209.l;_T20B=_T20A.first_line;_T20C=Cyc_Position_loc_to_seg(_T20B);_T20D=Cyc_Parse_make_function(_T1F9,_T1FB,_T1FF,_T203,_T207,_T20C);yyval=Cyc_YY15(_T20D);goto _LL0;}case 45: _T20E=yyyvsp;_T20F=& _T20E[1].v;_T210=(union Cyc_YYSTYPE*)_T20F;_T211=
# 1516 "parse.y"
Cyc_yyget_QualId_tok(_T210);Cyc_Lex_enter_using(_T211);_T212=yyyvsp;_T213=_T212[1];yyval=_T213.v;goto _LL0;case 46:
# 1519
 Cyc_Lex_leave_using();goto _LL0;case 47:{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));_T215=yyyvsp;_T216=& _T215[1].v;_T217=(union Cyc_YYSTYPE*)_T216;
# 1522
*_T187A=Cyc_yyget_String_tok(_T217);_T214=(struct _fat_ptr*)_T187A;}Cyc_Lex_enter_namespace(_T214);_T218=yyyvsp;_T219=_T218[1];yyval=_T219.v;goto _LL0;case 48:
# 1525
 Cyc_Lex_leave_namespace();goto _LL0;case 49:
# 1528
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + 1;goto _LL0;case 50:
# 1531
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + -1;goto _LL0;case 51: _T21A=yyyvsp;_T21B=& _T21A[0].v;_T21C=(union Cyc_YYSTYPE*)_T21B;_T21D=
# 1536 "parse.y"
Cyc_yyget_YY17(_T21C);_T21E=yyyvsp;_T21F=_T21E[0];_T220=_T21F.l;_T221=_T220.first_line;_T222=Cyc_Position_loc_to_seg(_T221);_T223=yyyvsp;_T224=_T223[0];_T225=_T224.l;_T226=_T225.first_line;_T227=Cyc_Position_loc_to_seg(_T226);_T228=Cyc_Parse_make_declarations(_T21D,0,_T222,_T227);yyval=Cyc_YY16(_T228);goto _LL0;case 52: _T229=yyyvsp;_T22A=& _T229[0].v;_T22B=(union Cyc_YYSTYPE*)_T22A;_T22C=
# 1538 "parse.y"
Cyc_yyget_YY17(_T22B);{struct _tuple11*(*_T187A)(struct _tuple11*)=(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;_T22D=_T187A;}_T22E=yyyvsp;_T22F=& _T22E[1].v;_T230=(union Cyc_YYSTYPE*)_T22F;_T231=Cyc_yyget_YY19(_T230);_T232=_T22D(_T231);_T233=yyyvsp;_T234=_T233[0];_T235=_T234.l;_T236=_T235.first_line;_T237=Cyc_Position_loc_to_seg(_T236);_T238=yyyvsp;_T239=_T238[0];_T23A=_T239.l;_T23B=_T23A.first_line;_T23C=Cyc_Position_loc_to_seg(_T23B);_T23D=Cyc_Parse_make_declarations(_T22C,_T232,_T237,_T23C);yyval=Cyc_YY16(_T23D);goto _LL0;case 53:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T23F=yyyvsp;_T240=& _T23F[1].v;_T241=(union Cyc_YYSTYPE*)_T240;_T242=
# 1541
Cyc_yyget_YY9(_T241);_T243=yyyvsp;_T244=& _T243[3].v;_T245=(union Cyc_YYSTYPE*)_T244;_T246=Cyc_yyget_Exp_tok(_T245);_T247=yyyvsp;_T248=_T247[0];_T249=_T248.l;_T24A=_T249.first_line;_T24B=Cyc_Position_loc_to_seg(_T24A);_T187A->hd=Cyc_Absyn_let_decl(_T242,_T246,_T24B);_T187A->tl=0;_T23E=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T23E);goto _LL0;case 54:  {
# 1543 "parse.y"
struct Cyc_List_List*vds=0;_T24C=yyyvsp;_T24D=& _T24C[1].v;_T24E=(union Cyc_YYSTYPE*)_T24D;{
struct Cyc_List_List*ids=Cyc_yyget_YY37(_T24E);_TL294: if(ids!=0)goto _TL292;else{goto _TL293;}
_TL292:{struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0));_T24F=qv;_T24F->f0=Cyc_Absyn_Rel_n(0);_T250=qv;_T251=ids;_T252=_T251->hd;_T250->f1=(struct _fat_ptr*)_T252;{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T254=qv;_T255=
Cyc_Absyn_wildtyp(0);_T187A->hd=Cyc_Absyn_new_vardecl(0U,_T254,_T255,0,0);_T187A->tl=vds;_T253=(struct Cyc_List_List*)_T187A;}vds=_T253;}_T256=ids;
# 1544
ids=_T256->tl;goto _TL294;_TL293:;}{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T258=
# 1548
Cyc_List_imp_rev(vds);_T259=yyyvsp;_T25A=_T259[0];_T25B=_T25A.l;_T25C=_T25B.first_line;_T25D=Cyc_Position_loc_to_seg(_T25C);_T187A->hd=Cyc_Absyn_letv_decl(_T258,_T25D);_T187A->tl=0;_T257=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T257);goto _LL0;}case 55: _T25E=yyyvsp;_T25F=& _T25E[2].v;_T260=(union Cyc_YYSTYPE*)_T25F;_T261=
# 1553 "parse.y"
Cyc_yyget_String_tok(_T260);_T262=yyyvsp;_T263=_T262[2];_T264=_T263.l;_T265=_T264.first_line;_T266=Cyc_Position_loc_to_seg(_T265);Cyc_Parse_tvar_ok(_T261,_T266);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T267=tv;{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));_T269=yyyvsp;_T26A=& _T269[2].v;_T26B=(union Cyc_YYSTYPE*)_T26A;*_T187A=Cyc_yyget_String_tok(_T26B);_T268=(struct _fat_ptr*)_T187A;}_T267->name=_T268;_T26C=tv;_T26C->identity=- 1;_T26D=tv;_T26E=& Cyc_Kinds_ek;_T26F=(struct Cyc_Absyn_Kind*)_T26E;_T26D->kind=Cyc_Kinds_kind_to_bound(_T26F);_T270=tv;_T270->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T271=yyyvsp;_T272=_T271[4];_T273=_T272.l;_T274=_T273.first_line;_T275=
Cyc_Position_loc_to_seg(_T274);{struct _tuple0*_T187A=_cycalloc(sizeof(struct _tuple0));_T187A->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T278=yyyvsp;_T279=& _T278[4].v;_T27A=(union Cyc_YYSTYPE*)_T279;*_T187B=Cyc_yyget_String_tok(_T27A);_T277=(struct _fat_ptr*)_T187B;}_T187A->f1=_T277;_T276=(struct _tuple0*)_T187A;}_T27B=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T275,_T276,_T27B,0,0);{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T27D=tv;_T27E=vd;_T27F=yyyvsp;_T280=_T27F[0];_T281=_T280.l;_T282=_T281.first_line;_T283=
Cyc_Position_loc_to_seg(_T282);_T187A->hd=Cyc_Absyn_region_decl(_T27D,_T27E,0,_T283);_T187A->tl=0;_T27C=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T27C);goto _LL0;}}}case 56:{struct Cyc_String_pa_PrintArg_struct _T187A;_T187A.tag=0;_T286=yyyvsp;_T287=& _T286[1].v;_T288=(union Cyc_YYSTYPE*)_T287;
# 1561
_T187A.f1=Cyc_yyget_String_tok(_T288);_T285=_T187A;}{struct Cyc_String_pa_PrintArg_struct _T187A=_T285;void*_T187B[1];_T187B[0]=& _T187A;_T289=_tag_fat("`%s",sizeof(char),4U);_T28A=_tag_fat(_T187B,sizeof(void*),1);_T284=Cyc_aprintf(_T289,_T28A);}{struct _fat_ptr tvstring=_T284;_T28B=yyyvsp;_T28C=& _T28B[1].v;_T28D=(union Cyc_YYSTYPE*)_T28C;_T28E=
Cyc_yyget_String_tok(_T28D);_T28F=yyyvsp;_T290=_T28F[1];_T291=_T290.l;_T292=_T291.first_line;_T293=Cyc_Position_loc_to_seg(_T292);Cyc_Parse_tvar_ok(_T28E,_T293);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T294=tv;{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));*_T187A=tvstring;_T295=(struct _fat_ptr*)_T187A;}_T294->name=_T295;_T296=tv;_T296->identity=- 1;_T297=tv;_T298=& Cyc_Kinds_ek;_T299=(struct Cyc_Absyn_Kind*)_T298;_T297->kind=Cyc_Kinds_kind_to_bound(_T299);_T29A=tv;_T29A->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T29B=yyyvsp;_T29C=_T29B[1];_T29D=_T29C.l;_T29E=_T29D.first_line;_T29F=
Cyc_Position_loc_to_seg(_T29E);{struct _tuple0*_T187A=_cycalloc(sizeof(struct _tuple0));_T187A->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T2A2=yyyvsp;_T2A3=& _T2A2[1].v;_T2A4=(union Cyc_YYSTYPE*)_T2A3;*_T187B=Cyc_yyget_String_tok(_T2A4);_T2A1=(struct _fat_ptr*)_T187B;}_T187A->f1=_T2A1;_T2A0=(struct _tuple0*)_T187A;}_T2A5=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T29F,_T2A0,_T2A5,0,0);{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T2A7=tv;_T2A8=vd;_T2A9=yyyvsp;_T2AA=& _T2A9[2].v;_T2AB=(union Cyc_YYSTYPE*)_T2AA;_T2AC=
Cyc_yyget_YY61(_T2AB);_T2AD=yyyvsp;_T2AE=_T2AD[0];_T2AF=_T2AE.l;_T2B0=_T2AF.first_line;_T2B1=Cyc_Position_loc_to_seg(_T2B0);_T187A->hd=Cyc_Absyn_region_decl(_T2A7,_T2A8,_T2AC,_T2B1);_T187A->tl=0;_T2A6=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T2A6);goto _LL0;}}}}case 57:
# 1571 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 58: _T2B2=yyyvsp;_T2B3=& _T2B2[1].v;_T2B4=(union Cyc_YYSTYPE*)_T2B3;_T2B5=
# 1573 "parse.y"
Cyc_yyget_String_tok(_T2B4);_T2B6=_tag_fat("open",sizeof(char),5U);_T2B7=Cyc_strcmp(_T2B5,_T2B6);if(_T2B7==0)goto _TL295;_T2B8=yyyvsp;_T2B9=_T2B8[3];_T2BA=_T2B9.l;_T2BB=_T2BA.first_line;_T2BC=Cyc_Position_loc_to_seg(_T2BB);_T2BD=_tag_fat("expecting `open'",sizeof(char),17U);_T2BE=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2BC,_T2BD,_T2BE);goto _TL296;_TL295: _TL296: _T2BF=yyyvsp;_T2C0=& _T2BF[3].v;_T2C1=(union Cyc_YYSTYPE*)_T2C0;_T2C2=
Cyc_yyget_Exp_tok(_T2C1);yyval=Cyc_YY61(_T2C2);goto _LL0;case 59: _T2C3=yyyvsp;_T2C4=_T2C3[0];
# 1577
yyval=_T2C4.v;goto _LL0;case 60: _T2C5=yyyvsp;_T2C6=& _T2C5[0].v;_T2C7=(union Cyc_YYSTYPE*)_T2C6;_T2C8=
# 1578 "parse.y"
Cyc_yyget_YY16(_T2C7);_T2C9=yyyvsp;_T2CA=& _T2C9[1].v;_T2CB=(union Cyc_YYSTYPE*)_T2CA;_T2CC=Cyc_yyget_YY16(_T2CB);_T2CD=Cyc_List_imp_append(_T2C8,_T2CC);yyval=Cyc_YY16(_T2CD);goto _LL0;case 61:{struct Cyc_Parse_Declaration_spec _T187A;_T2CF=yyyvsp;_T2D0=& _T2CF[0].v;_T2D1=(union Cyc_YYSTYPE*)_T2D0;
# 1584 "parse.y"
_T187A.sc=Cyc_yyget_YY20(_T2D1);_T2D2=yyyvsp;_T2D3=_T2D2[0];_T2D4=_T2D3.l;_T2D5=_T2D4.first_line;_T2D6=Cyc_Position_loc_to_seg(_T2D5);_T187A.tq=Cyc_Absyn_empty_tqual(_T2D6);
_T187A.type_specs=Cyc_Parse_empty_spec(0U);_T187A.is_inline=0;_T187A.attributes=0;_T2CE=_T187A;}
# 1584
yyval=Cyc_YY17(_T2CE);goto _LL0;case 62: _T2D7=yyyvsp;_T2D8=& _T2D7[1].v;_T2D9=(union Cyc_YYSTYPE*)_T2D8;{
# 1587 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T2D9);_T2DA=two;_T2DB=_T2DA.sc;_T2DC=(int)_T2DB;
if(_T2DC==7)goto _TL297;_T2DD=yyyvsp;_T2DE=_T2DD[0];_T2DF=_T2DE.l;_T2E0=_T2DF.first_line;_T2E1=
Cyc_Position_loc_to_seg(_T2E0);_T2E2=
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U);_T2E3=_tag_fat(0U,sizeof(void*),0);
# 1589
Cyc_Warn_warn(_T2E1,_T2E2,_T2E3);goto _TL298;_TL297: _TL298:{struct Cyc_Parse_Declaration_spec _T187A;_T2E5=yyyvsp;_T2E6=& _T2E5[0].v;_T2E7=(union Cyc_YYSTYPE*)_T2E6;
# 1591
_T187A.sc=Cyc_yyget_YY20(_T2E7);_T2E8=two;_T187A.tq=_T2E8.tq;_T2E9=two;_T187A.type_specs=_T2E9.type_specs;_T2EA=two;
_T187A.is_inline=_T2EA.is_inline;_T2EB=two;_T187A.attributes=_T2EB.attributes;_T2E4=_T187A;}
# 1591
yyval=Cyc_YY17(_T2E4);goto _LL0;}case 63: _T2EC=yyyvsp;_T2ED=_T2EC[0];_T2EE=_T2ED.l;_T2EF=_T2EE.first_line;_T2F0=
# 1595 "parse.y"
Cyc_Position_loc_to_seg(_T2EF);_T2F1=_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U);_T2F2=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T2F0,_T2F1,_T2F2);_T2F3=yyyvsp;_T2F4=_T2F3[1];
yyval=_T2F4.v;goto _LL0;case 64:{struct Cyc_Parse_Declaration_spec _T187A;
# 1599 "parse.y"
_T187A.sc=7U;_T2F6=yyyvsp;_T2F7=_T2F6[0];_T2F8=_T2F7.l;_T2F9=_T2F8.first_line;_T2FA=Cyc_Position_loc_to_seg(_T2F9);_T187A.tq=Cyc_Absyn_empty_tqual(_T2FA);_T2FB=yyyvsp;_T2FC=& _T2FB[0].v;_T2FD=(union Cyc_YYSTYPE*)_T2FC;
_T187A.type_specs=Cyc_yyget_YY21(_T2FD);_T187A.is_inline=0;_T187A.attributes=0;_T2F5=_T187A;}
# 1599
yyval=Cyc_YY17(_T2F5);goto _LL0;case 65: _T2FE=yyyvsp;_T2FF=& _T2FE[1].v;_T300=(union Cyc_YYSTYPE*)_T2FF;{
# 1602 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T300);{struct Cyc_Parse_Declaration_spec _T187A;_T302=two;
_T187A.sc=_T302.sc;_T303=two;_T187A.tq=_T303.tq;_T304=yyyvsp;_T305=_T304[0];_T306=_T305.l;_T307=_T306.first_line;_T308=
Cyc_Position_loc_to_seg(_T307);_T309=two;_T30A=_T309.type_specs;_T30B=yyyvsp;_T30C=& _T30B[0].v;_T30D=(union Cyc_YYSTYPE*)_T30C;_T30E=Cyc_yyget_YY21(_T30D);_T187A.type_specs=Cyc_Parse_combine_specifiers(_T308,_T30A,_T30E);_T30F=two;
_T187A.is_inline=_T30F.is_inline;_T310=two;_T187A.attributes=_T310.attributes;_T301=_T187A;}
# 1603
yyval=Cyc_YY17(_T301);goto _LL0;}case 66:{struct Cyc_Parse_Declaration_spec _T187A;
# 1608 "parse.y"
_T187A.sc=7U;_T312=yyyvsp;_T313=& _T312[0].v;_T314=(union Cyc_YYSTYPE*)_T313;_T187A.tq=Cyc_yyget_YY24(_T314);_T187A.type_specs=Cyc_Parse_empty_spec(0U);_T187A.is_inline=0;_T187A.attributes=0;_T311=_T187A;}yyval=Cyc_YY17(_T311);goto _LL0;case 67: _T315=yyyvsp;_T316=& _T315[1].v;_T317=(union Cyc_YYSTYPE*)_T316;{
# 1610 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T317);{struct Cyc_Parse_Declaration_spec _T187A;_T319=two;
_T187A.sc=_T319.sc;_T31A=yyyvsp;_T31B=& _T31A[0].v;_T31C=(union Cyc_YYSTYPE*)_T31B;_T31D=Cyc_yyget_YY24(_T31C);_T31E=two;_T31F=_T31E.tq;_T187A.tq=Cyc_Absyn_combine_tqual(_T31D,_T31F);_T320=two;
_T187A.type_specs=_T320.type_specs;_T321=two;_T187A.is_inline=_T321.is_inline;_T322=two;_T187A.attributes=_T322.attributes;_T318=_T187A;}
# 1611
yyval=Cyc_YY17(_T318);goto _LL0;}case 68:{struct Cyc_Parse_Declaration_spec _T187A;
# 1615 "parse.y"
_T187A.sc=7U;_T324=yyyvsp;_T325=_T324[0];_T326=_T325.l;_T327=_T326.first_line;_T328=Cyc_Position_loc_to_seg(_T327);_T187A.tq=Cyc_Absyn_empty_tqual(_T328);
_T187A.type_specs=Cyc_Parse_empty_spec(0U);_T187A.is_inline=1;_T187A.attributes=0;_T323=_T187A;}
# 1615
yyval=Cyc_YY17(_T323);goto _LL0;case 69: _T329=yyyvsp;_T32A=& _T329[1].v;_T32B=(union Cyc_YYSTYPE*)_T32A;{
# 1618 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T32B);{struct Cyc_Parse_Declaration_spec _T187A;_T32D=two;
_T187A.sc=_T32D.sc;_T32E=two;_T187A.tq=_T32E.tq;_T32F=two;_T187A.type_specs=_T32F.type_specs;_T187A.is_inline=1;_T330=two;_T187A.attributes=_T330.attributes;_T32C=_T187A;}yyval=Cyc_YY17(_T32C);goto _LL0;}case 70:{struct Cyc_Parse_Declaration_spec _T187A;
# 1622 "parse.y"
_T187A.sc=7U;_T332=yyyvsp;_T333=_T332[0];_T334=_T333.l;_T335=_T334.first_line;_T336=Cyc_Position_loc_to_seg(_T335);_T187A.tq=Cyc_Absyn_empty_tqual(_T336);
_T187A.type_specs=Cyc_Parse_empty_spec(0U);_T187A.is_inline=0;_T337=yyyvsp;_T338=& _T337[0].v;_T339=(union Cyc_YYSTYPE*)_T338;_T187A.attributes=Cyc_yyget_YY46(_T339);_T331=_T187A;}
# 1622
yyval=Cyc_YY17(_T331);goto _LL0;case 71: _T33A=yyyvsp;_T33B=& _T33A[1].v;_T33C=(union Cyc_YYSTYPE*)_T33B;{
# 1625 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T33C);{struct Cyc_Parse_Declaration_spec _T187A;_T33E=two;
_T187A.sc=_T33E.sc;_T33F=two;_T187A.tq=_T33F.tq;_T340=two;
_T187A.type_specs=_T340.type_specs;_T341=two;_T187A.is_inline=_T341.is_inline;_T342=yyyvsp;_T343=& _T342[0].v;_T344=(union Cyc_YYSTYPE*)_T343;_T345=
Cyc_yyget_YY46(_T344);_T346=two;_T347=_T346.attributes;_T187A.attributes=Cyc_List_imp_append(_T345,_T347);_T33D=_T187A;}
# 1626
yyval=Cyc_YY17(_T33D);goto _LL0;}case 72:
# 1632 "parse.y"
 yyval=Cyc_YY20(4U);goto _LL0;case 73:
# 1633 "parse.y"
 yyval=Cyc_YY20(5U);goto _LL0;case 74:
# 1634 "parse.y"
 yyval=Cyc_YY20(3U);goto _LL0;case 75:
# 1635 "parse.y"
 yyval=Cyc_YY20(1U);goto _LL0;case 76: _T348=yyyvsp;_T349=& _T348[1].v;_T34A=(union Cyc_YYSTYPE*)_T349;_T34B=
# 1637 "parse.y"
Cyc_yyget_String_tok(_T34A);_T34C=_tag_fat("C",sizeof(char),2U);_T34D=Cyc_strcmp(_T34B,_T34C);if(_T34D==0)goto _TL299;_T34E=yyyvsp;_T34F=_T34E[0];_T350=_T34F.l;_T351=_T350.first_line;_T352=
Cyc_Position_loc_to_seg(_T351);_T353=_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U);_T354=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T352,_T353,_T354);goto _TL29A;_TL299: _TL29A:
 yyval=Cyc_YY20(2U);goto _LL0;case 77:
# 1641 "parse.y"
 yyval=Cyc_YY20(0U);goto _LL0;case 78:
# 1643 "parse.y"
 yyval=Cyc_YY20(6U);goto _LL0;case 79:
# 1648 "parse.y"
 yyval=Cyc_YY46(0);goto _LL0;case 80: _T355=yyyvsp;_T356=_T355[0];
# 1649 "parse.y"
yyval=_T356.v;goto _LL0;case 81: _T357=yyyvsp;_T358=_T357[3];
# 1653 "parse.y"
yyval=_T358.v;goto _LL0;case 82:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T35A=yyyvsp;_T35B=& _T35A[0].v;_T35C=(union Cyc_YYSTYPE*)_T35B;
# 1656
_T187A->hd=Cyc_yyget_YY47(_T35C);_T187A->tl=0;_T359=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY46(_T359);goto _LL0;case 83:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T35E=yyyvsp;_T35F=& _T35E[0].v;_T360=(union Cyc_YYSTYPE*)_T35F;
# 1657 "parse.y"
_T187A->hd=Cyc_yyget_YY47(_T360);_T361=yyyvsp;_T362=& _T361[2].v;_T363=(union Cyc_YYSTYPE*)_T362;_T187A->tl=Cyc_yyget_YY46(_T363);_T35D=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY46(_T35D);goto _LL0;case 84: _T364=yyyvsp;_T365=_T364[0];_T366=_T365.l;_T367=_T366.first_line;_T368=
# 1660
Cyc_Position_loc_to_seg(_T367);_T369=yyyvsp;_T36A=& _T369[0].v;_T36B=(union Cyc_YYSTYPE*)_T36A;_T36C=Cyc_yyget_String_tok(_T36B);_T36D=Cyc_Atts_parse_nullary_att(_T368,_T36C);yyval=Cyc_YY47(_T36D);goto _LL0;case 85: _T36E=& Cyc_Atts_Const_att_val;_T36F=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_T36E;_T370=(void*)_T36F;
# 1661 "parse.y"
yyval=Cyc_YY47(_T370);goto _LL0;case 86: _T371=yyyvsp;_T372=_T371[0];_T373=_T372.l;_T374=_T373.first_line;_T375=
# 1663 "parse.y"
Cyc_Position_loc_to_seg(_T374);_T376=yyyvsp;_T377=& _T376[0].v;_T378=(union Cyc_YYSTYPE*)_T377;_T379=Cyc_yyget_String_tok(_T378);_T37A=yyyvsp;_T37B=_T37A[2];_T37C=_T37B.l;_T37D=_T37C.first_line;_T37E=Cyc_Position_loc_to_seg(_T37D);_T37F=yyyvsp;_T380=& _T37F[2].v;_T381=(union Cyc_YYSTYPE*)_T380;_T382=Cyc_yyget_Exp_tok(_T381);_T383=Cyc_Atts_parse_unary_att(_T375,_T379,_T37E,_T382);yyval=Cyc_YY47(_T383);goto _LL0;case 87: _T384=yyyvsp;_T385=_T384[0];_T386=_T385.l;_T387=_T386.first_line;_T388=
# 1665 "parse.y"
Cyc_Position_loc_to_seg(_T387);_T389=yyyvsp;_T38A=_T389[2];_T38B=_T38A.l;_T38C=_T38B.first_line;_T38D=Cyc_Position_loc_to_seg(_T38C);_T38E=yyyvsp;_T38F=& _T38E[0].v;_T390=(union Cyc_YYSTYPE*)_T38F;_T391=Cyc_yyget_String_tok(_T390);_T392=yyyvsp;_T393=& _T392[2].v;_T394=(union Cyc_YYSTYPE*)_T393;_T395=Cyc_yyget_String_tok(_T394);_T396=yyyvsp;_T397=_T396[4];_T398=_T397.l;_T399=_T398.first_line;_T39A=
Cyc_Position_loc_to_seg(_T399);_T39B=yyyvsp;_T39C=& _T39B[4].v;_T39D=(union Cyc_YYSTYPE*)_T39C;_T39E=Cyc_yyget_Int_tok(_T39D);_T39F=Cyc_Parse_cnst2uint(_T39A,_T39E);_T3A0=yyyvsp;_T3A1=_T3A0[6];_T3A2=_T3A1.l;_T3A3=_T3A2.first_line;_T3A4=
Cyc_Position_loc_to_seg(_T3A3);_T3A5=yyyvsp;_T3A6=& _T3A5[6].v;_T3A7=(union Cyc_YYSTYPE*)_T3A6;_T3A8=Cyc_yyget_Int_tok(_T3A7);_T3A9=Cyc_Parse_cnst2uint(_T3A4,_T3A8);_T3AA=
# 1665
Cyc_Atts_parse_format_att(_T388,_T38D,_T391,_T395,_T39F,_T3A9);yyval=Cyc_YY47(_T3AA);goto _LL0;case 88: _T3AB=yyyvsp;_T3AC=_T3AB[0];
# 1677 "parse.y"
yyval=_T3AC.v;goto _LL0;case 89: _T3AD=yyyvsp;_T3AE=& _T3AD[0].v;_T3AF=(union Cyc_YYSTYPE*)_T3AE;_T3B0=
# 1679 "parse.y"
Cyc_yyget_QualId_tok(_T3AF);_T3B1=yyyvsp;_T3B2=& _T3B1[1].v;_T3B3=(union Cyc_YYSTYPE*)_T3B2;_T3B4=Cyc_yyget_YY41(_T3B3);_T3B5=Cyc_Absyn_typedef_type(_T3B0,_T3B4,0,0);_T3B6=yyyvsp;_T3B7=_T3B6[0];_T3B8=_T3B7.l;_T3B9=_T3B8.first_line;_T3BA=Cyc_Position_loc_to_seg(_T3B9);_T3BB=Cyc_Parse_type_spec(_T3B5,_T3BA);yyval=Cyc_YY21(_T3BB);goto _LL0;case 90: _T3BC=Cyc_Absyn_void_type;_T3BD=yyyvsp;_T3BE=_T3BD[0];_T3BF=_T3BE.l;_T3C0=_T3BF.first_line;_T3C1=
# 1683 "parse.y"
Cyc_Position_loc_to_seg(_T3C0);_T3C2=Cyc_Parse_type_spec(_T3BC,_T3C1);yyval=Cyc_YY21(_T3C2);goto _LL0;case 91: _T3C3=Cyc_Absyn_char_type;_T3C4=yyyvsp;_T3C5=_T3C4[0];_T3C6=_T3C5.l;_T3C7=_T3C6.first_line;_T3C8=
# 1684 "parse.y"
Cyc_Position_loc_to_seg(_T3C7);_T3C9=Cyc_Parse_type_spec(_T3C3,_T3C8);yyval=Cyc_YY21(_T3C9);goto _LL0;case 92: _T3CA=yyyvsp;_T3CB=_T3CA[0];_T3CC=_T3CB.l;_T3CD=_T3CC.first_line;_T3CE=
# 1685 "parse.y"
Cyc_Position_loc_to_seg(_T3CD);_T3CF=Cyc_Parse_short_spec(_T3CE);yyval=Cyc_YY21(_T3CF);goto _LL0;case 93: _T3D0=Cyc_Absyn_sint_type;_T3D1=yyyvsp;_T3D2=_T3D1[0];_T3D3=_T3D2.l;_T3D4=_T3D3.first_line;_T3D5=
# 1686 "parse.y"
Cyc_Position_loc_to_seg(_T3D4);_T3D6=Cyc_Parse_type_spec(_T3D0,_T3D5);yyval=Cyc_YY21(_T3D6);goto _LL0;case 94: _T3D7=yyyvsp;_T3D8=_T3D7[0];_T3D9=_T3D8.l;_T3DA=_T3D9.first_line;_T3DB=
# 1687 "parse.y"
Cyc_Position_loc_to_seg(_T3DA);_T3DC=Cyc_Parse_long_spec(_T3DB);yyval=Cyc_YY21(_T3DC);goto _LL0;case 95: _T3DD=Cyc_Absyn_float_type;_T3DE=yyyvsp;_T3DF=_T3DE[0];_T3E0=_T3DF.l;_T3E1=_T3E0.first_line;_T3E2=
# 1688 "parse.y"
Cyc_Position_loc_to_seg(_T3E1);_T3E3=Cyc_Parse_type_spec(_T3DD,_T3E2);yyval=Cyc_YY21(_T3E3);goto _LL0;case 96: _T3E4=Cyc_Absyn_double_type;_T3E5=yyyvsp;_T3E6=_T3E5[0];_T3E7=_T3E6.l;_T3E8=_T3E7.first_line;_T3E9=
# 1689 "parse.y"
Cyc_Position_loc_to_seg(_T3E8);_T3EA=Cyc_Parse_type_spec(_T3E4,_T3E9);yyval=Cyc_YY21(_T3EA);goto _LL0;case 97: _T3EB=yyyvsp;_T3EC=_T3EB[0];_T3ED=_T3EC.l;_T3EE=_T3ED.first_line;_T3EF=
# 1690 "parse.y"
Cyc_Position_loc_to_seg(_T3EE);_T3F0=Cyc_Parse_signed_spec(_T3EF);yyval=Cyc_YY21(_T3F0);goto _LL0;case 98: _T3F1=yyyvsp;_T3F2=_T3F1[0];_T3F3=_T3F2.l;_T3F4=_T3F3.first_line;_T3F5=
# 1691 "parse.y"
Cyc_Position_loc_to_seg(_T3F4);_T3F6=Cyc_Parse_unsigned_spec(_T3F5);yyval=Cyc_YY21(_T3F6);goto _LL0;case 99: _T3F7=yyyvsp;_T3F8=_T3F7[0];_T3F9=_T3F8.l;_T3FA=_T3F9.first_line;_T3FB=
# 1692 "parse.y"
Cyc_Position_loc_to_seg(_T3FA);_T3FC=Cyc_Parse_complex_spec(_T3FB);yyval=Cyc_YY21(_T3FC);goto _LL0;case 100: _T3FD=yyyvsp;_T3FE=_T3FD[0];
# 1693 "parse.y"
yyval=_T3FE.v;goto _LL0;case 101: _T3FF=yyyvsp;_T400=_T3FF[0];
# 1694 "parse.y"
yyval=_T400.v;goto _LL0;case 102: _T401=yyyvsp;_T402=& _T401[2].v;_T403=(union Cyc_YYSTYPE*)_T402;_T404=
# 1696 "parse.y"
Cyc_yyget_Exp_tok(_T403);_T405=Cyc_Absyn_typeof_type(_T404);_T406=yyyvsp;_T407=_T406[0];_T408=_T407.l;_T409=_T408.first_line;_T40A=Cyc_Position_loc_to_seg(_T409);_T40B=Cyc_Parse_type_spec(_T405,_T40A);yyval=Cyc_YY21(_T40B);goto _LL0;case 103: _T40C=
# 1698 "parse.y"
_tag_fat("__builtin_va_list",sizeof(char),18U);_T40D=& Cyc_Kinds_bk;_T40E=(struct Cyc_Absyn_Kind*)_T40D;_T40F=Cyc_Absyn_builtin_type(_T40C,_T40E);_T410=yyyvsp;_T411=_T410[0];_T412=_T411.l;_T413=_T412.first_line;_T414=Cyc_Position_loc_to_seg(_T413);_T415=Cyc_Parse_type_spec(_T40F,_T414);yyval=Cyc_YY21(_T415);goto _LL0;case 104: _T416=yyyvsp;_T417=_T416[0];
# 1700 "parse.y"
yyval=_T417.v;goto _LL0;case 105: _T418=yyyvsp;_T419=& _T418[0].v;_T41A=(union Cyc_YYSTYPE*)_T419;_T41B=
# 1702 "parse.y"
Cyc_yyget_YY45(_T41A);_T41C=yyyvsp;_T41D=_T41C[0];_T41E=_T41D.l;_T41F=_T41E.first_line;_T420=Cyc_Position_loc_to_seg(_T41F);_T421=Cyc_Parse_type_spec(_T41B,_T420);yyval=Cyc_YY21(_T421);goto _LL0;case 106: _T422=
# 1704 "parse.y"
Cyc_Absyn_new_evar(0,0);_T423=yyyvsp;_T424=_T423[0];_T425=_T424.l;_T426=_T425.first_line;_T427=Cyc_Position_loc_to_seg(_T426);_T428=Cyc_Parse_type_spec(_T422,_T427);yyval=Cyc_YY21(_T428);goto _LL0;case 107: _T429=yyyvsp;_T42A=& _T429[2].v;_T42B=(union Cyc_YYSTYPE*)_T42A;_T42C=
# 1706 "parse.y"
Cyc_yyget_YY44(_T42B);_T42D=Cyc_Kinds_kind_to_opt(_T42C);_T42E=Cyc_Absyn_new_evar(_T42D,0);_T42F=yyyvsp;_T430=_T42F[0];_T431=_T430.l;_T432=_T431.first_line;_T433=Cyc_Position_loc_to_seg(_T432);_T434=Cyc_Parse_type_spec(_T42E,_T433);yyval=Cyc_YY21(_T434);goto _LL0;case 108: _T436=Cyc_List_map_c;{
# 1708 "parse.y"
struct Cyc_List_List*(*_T187A)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T436;_T435=_T187A;}{struct _tuple19*(*_T187A)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T437=_T187A;}_T438=yyyvsp;_T439=_T438[2];_T43A=_T439.l;_T43B=_T43A.first_line;_T43C=Cyc_Position_loc_to_seg(_T43B);_T43D=yyyvsp;_T43E=& _T43D[2].v;_T43F=(union Cyc_YYSTYPE*)_T43E;_T440=Cyc_yyget_YY39(_T43F);_T441=Cyc_List_imp_rev(_T440);_T442=_T435(_T437,_T43C,_T441);_T443=Cyc_Absyn_tuple_type(_T442);_T444=yyyvsp;_T445=_T444[0];_T446=_T445.l;_T447=_T446.first_line;_T448=
Cyc_Position_loc_to_seg(_T447);_T449=
# 1708
Cyc_Parse_type_spec(_T443,_T448);yyval=Cyc_YY21(_T449);goto _LL0;case 109: _T44A=yyyvsp;_T44B=& _T44A[2].v;_T44C=(union Cyc_YYSTYPE*)_T44B;_T44D=
# 1711 "parse.y"
Cyc_yyget_YY45(_T44C);_T44E=Cyc_Absyn_rgn_handle_type(_T44D);_T44F=yyyvsp;_T450=_T44F[0];_T451=_T450.l;_T452=_T451.first_line;_T453=Cyc_Position_loc_to_seg(_T452);_T454=Cyc_Parse_type_spec(_T44E,_T453);yyval=Cyc_YY21(_T454);goto _LL0;case 110: _T455=& Cyc_Kinds_eko;_T456=(struct Cyc_Core_Opt*)_T455;_T457=
# 1713 "parse.y"
Cyc_Absyn_new_evar(_T456,0);_T458=Cyc_Absyn_rgn_handle_type(_T457);_T459=yyyvsp;_T45A=_T459[0];_T45B=_T45A.l;_T45C=_T45B.first_line;_T45D=Cyc_Position_loc_to_seg(_T45C);_T45E=Cyc_Parse_type_spec(_T458,_T45D);yyval=Cyc_YY21(_T45E);goto _LL0;case 111: _T45F=yyyvsp;_T460=& _T45F[2].v;_T461=(union Cyc_YYSTYPE*)_T460;_T462=
# 1715 "parse.y"
Cyc_yyget_YY58(_T461);_T463=Cyc_Absyn_aqual_handle_type(_T462);_T464=yyyvsp;_T465=_T464[0];_T466=_T465.l;_T467=_T466.first_line;_T468=Cyc_Position_loc_to_seg(_T467);_T469=Cyc_Parse_type_spec(_T463,_T468);yyval=Cyc_YY21(_T469);goto _LL0;case 112: _T46A=yyyvsp;_T46B=& _T46A[2].v;_T46C=(union Cyc_YYSTYPE*)_T46B;_T46D=
# 1717 "parse.y"
Cyc_yyget_YY45(_T46C);_T46E=Cyc_Absyn_tag_type(_T46D);_T46F=yyyvsp;_T470=_T46F[0];_T471=_T470.l;_T472=_T471.first_line;_T473=Cyc_Position_loc_to_seg(_T472);_T474=Cyc_Parse_type_spec(_T46E,_T473);yyval=Cyc_YY21(_T474);goto _LL0;case 113: _T475=& Cyc_Kinds_iko;_T476=(struct Cyc_Core_Opt*)_T475;_T477=
# 1719 "parse.y"
Cyc_Absyn_new_evar(_T476,0);_T478=Cyc_Absyn_tag_type(_T477);_T479=yyyvsp;_T47A=_T479[0];_T47B=_T47A.l;_T47C=_T47B.first_line;_T47D=Cyc_Position_loc_to_seg(_T47C);_T47E=Cyc_Parse_type_spec(_T478,_T47D);yyval=Cyc_YY21(_T47E);goto _LL0;case 114: _T47F=yyyvsp;_T480=& _T47F[2].v;_T481=(union Cyc_YYSTYPE*)_T480;_T482=
# 1721 "parse.y"
Cyc_yyget_Exp_tok(_T481);_T483=Cyc_Absyn_valueof_type(_T482);_T484=yyyvsp;_T485=_T484[0];_T486=_T485.l;_T487=_T486.first_line;_T488=Cyc_Position_loc_to_seg(_T487);_T489=Cyc_Parse_type_spec(_T483,_T488);yyval=Cyc_YY21(_T489);goto _LL0;case 115: _T48A=yyyvsp;_T48B=& _T48A[0].v;_T48C=(union Cyc_YYSTYPE*)_T48B;_T48D=
# 1726 "parse.y"
Cyc_yyget_String_tok(_T48C);_T48E=yyyvsp;_T48F=_T48E[0];_T490=_T48F.l;_T491=_T490.first_line;_T492=Cyc_Position_loc_to_seg(_T491);_T493=Cyc_Kinds_id_to_kind(_T48D,_T492);yyval=Cyc_YY44(_T493);goto _LL0;case 116: _T495=Cyc_Flags_porting_c_code;
# 1730 "parse.y"
if(!_T495)goto _TL29B;_T496=yyyvsp;_T497=_T496[0];_T498=_T497.l;_T499=_T498.first_line;_T494=Cyc_Position_loc_to_seg(_T499);goto _TL29C;_TL29B: _T494=0U;_TL29C: {unsigned loc=_T494;{struct Cyc_Absyn_Tqual _T187A;
_T187A.print_const=1;_T187A.q_volatile=0;_T187A.q_restrict=0;_T187A.real_const=1;_T187A.loc=loc;_T49A=_T187A;}yyval=Cyc_YY24(_T49A);goto _LL0;}case 117:{struct Cyc_Absyn_Tqual _T187A;
# 1732 "parse.y"
_T187A.print_const=0;_T187A.q_volatile=1;_T187A.q_restrict=0;_T187A.real_const=0;_T187A.loc=0U;_T49B=_T187A;}yyval=Cyc_YY24(_T49B);goto _LL0;case 118:{struct Cyc_Absyn_Tqual _T187A;
# 1733 "parse.y"
_T187A.print_const=0;_T187A.q_volatile=0;_T187A.q_restrict=1;_T187A.real_const=0;_T187A.loc=0U;_T49C=_T187A;}yyval=Cyc_YY24(_T49C);goto _LL0;case 119:  {
# 1739 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl));_T49D=ed;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_T187A->tag=1;{struct Cyc_Absyn_Enumdecl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T187B->sc=2U;_T4A0=yyyvsp;_T4A1=& _T4A0[1].v;_T4A2=(union Cyc_YYSTYPE*)_T4A1;_T187B->name=Cyc_yyget_QualId_tok(_T4A2);{struct Cyc_Core_Opt*_T187C=_cycalloc(sizeof(struct Cyc_Core_Opt));_T4A4=yyyvsp;_T4A5=& _T4A4[3].v;_T4A6=(union Cyc_YYSTYPE*)_T4A5;_T187C->v=Cyc_yyget_YY49(_T4A6);_T4A3=(struct Cyc_Core_Opt*)_T187C;}_T187B->fields=_T4A3;_T49F=(struct Cyc_Absyn_Enumdecl*)_T187B;}_T187A->f1=_T49F;_T49E=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T187A;}_T49D->r=(void*)_T49E;_T4A7=ed;_T4A8=yyyvsp;_T4A9=_T4A8[0];_T4AA=_T4A9.l;_T4AB=_T4AA.first_line;
_T4A7->loc=Cyc_Position_loc_to_seg(_T4AB);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T187A->tag=10;
_T187A->f1=ed;_T187A->f2=0;_T4AC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T187A;}_T4AD=(void*)_T4AC;_T4AE=yyyvsp;_T4AF=_T4AE[0];_T4B0=_T4AF.l;_T4B1=_T4B0.first_line;_T4B2=Cyc_Position_loc_to_seg(_T4B1);_T4B3=Cyc_Parse_type_spec(_T4AD,_T4B2);yyval=Cyc_YY21(_T4B3);goto _LL0;}case 120: _T4B4=yyyvsp;_T4B5=& _T4B4[1].v;_T4B6=(union Cyc_YYSTYPE*)_T4B5;_T4B7=
# 1744 "parse.y"
Cyc_yyget_QualId_tok(_T4B6);_T4B8=Cyc_Absyn_enum_type(_T4B7,0);_T4B9=yyyvsp;_T4BA=_T4B9[0];_T4BB=_T4BA.l;_T4BC=_T4BB.first_line;_T4BD=Cyc_Position_loc_to_seg(_T4BC);_T4BE=Cyc_Parse_type_spec(_T4B8,_T4BD);yyval=Cyc_YY21(_T4BE);goto _LL0;case 121: _T4BF=yyyvsp;_T4C0=& _T4BF[2].v;_T4C1=(union Cyc_YYSTYPE*)_T4C0;_T4C2=
# 1746 "parse.y"
Cyc_yyget_YY49(_T4C1);_T4C3=Cyc_Absyn_anon_enum_type(_T4C2);_T4C4=yyyvsp;_T4C5=_T4C4[0];_T4C6=_T4C5.l;_T4C7=_T4C6.first_line;_T4C8=Cyc_Position_loc_to_seg(_T4C7);_T4C9=Cyc_Parse_type_spec(_T4C3,_T4C8);yyval=Cyc_YY21(_T4C9);goto _LL0;case 122:{struct Cyc_Absyn_Enumfield*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_T4CB=yyyvsp;_T4CC=& _T4CB[0].v;_T4CD=(union Cyc_YYSTYPE*)_T4CC;
# 1752 "parse.y"
_T187A->name=Cyc_yyget_QualId_tok(_T4CD);_T187A->tag=0;_T4CE=yyyvsp;_T4CF=_T4CE[0];_T4D0=_T4CF.l;_T4D1=_T4D0.first_line;_T187A->loc=Cyc_Position_loc_to_seg(_T4D1);_T4CA=(struct Cyc_Absyn_Enumfield*)_T187A;}yyval=Cyc_YY48(_T4CA);goto _LL0;case 123:{struct Cyc_Absyn_Enumfield*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_T4D3=yyyvsp;_T4D4=& _T4D3[0].v;_T4D5=(union Cyc_YYSTYPE*)_T4D4;
# 1754 "parse.y"
_T187A->name=Cyc_yyget_QualId_tok(_T4D5);_T4D6=yyyvsp;_T4D7=& _T4D6[2].v;_T4D8=(union Cyc_YYSTYPE*)_T4D7;_T187A->tag=Cyc_yyget_Exp_tok(_T4D8);_T4D9=yyyvsp;_T4DA=_T4D9[0];_T4DB=_T4DA.l;_T4DC=_T4DB.first_line;_T187A->loc=Cyc_Position_loc_to_seg(_T4DC);_T4D2=(struct Cyc_Absyn_Enumfield*)_T187A;}yyval=Cyc_YY48(_T4D2);goto _LL0;case 124:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T4DE=yyyvsp;_T4DF=& _T4DE[0].v;_T4E0=(union Cyc_YYSTYPE*)_T4DF;
# 1758 "parse.y"
_T187A->hd=Cyc_yyget_YY48(_T4E0);_T187A->tl=0;_T4DD=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY49(_T4DD);goto _LL0;case 125:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T4E2=yyyvsp;_T4E3=& _T4E2[0].v;_T4E4=(union Cyc_YYSTYPE*)_T4E3;
# 1759 "parse.y"
_T187A->hd=Cyc_yyget_YY48(_T4E4);_T187A->tl=0;_T4E1=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY49(_T4E1);goto _LL0;case 126:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T4E6=yyyvsp;_T4E7=& _T4E6[0].v;_T4E8=(union Cyc_YYSTYPE*)_T4E7;
# 1760 "parse.y"
_T187A->hd=Cyc_yyget_YY48(_T4E8);_T4E9=yyyvsp;_T4EA=& _T4E9[2].v;_T4EB=(union Cyc_YYSTYPE*)_T4EA;_T187A->tl=Cyc_yyget_YY49(_T4EB);_T4E5=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY49(_T4E5);goto _LL0;case 127:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_T187A->tag=7;_T4ED=yyyvsp;_T4EE=& _T4ED[0].v;_T4EF=(union Cyc_YYSTYPE*)_T4EE;
# 1766 "parse.y"
_T187A->f1=Cyc_yyget_YY22(_T4EF);_T187A->f2=0;_T4F0=yyyvsp;_T4F1=& _T4F0[2].v;_T4F2=(union Cyc_YYSTYPE*)_T4F1;_T187A->f3=Cyc_yyget_YY25(_T4F2);_T4EC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T187A;}_T4F3=(void*)_T4EC;_T4F4=yyyvsp;_T4F5=_T4F4[0];_T4F6=_T4F5.l;_T4F7=_T4F6.first_line;_T4F8=Cyc_Position_loc_to_seg(_T4F7);_T4F9=Cyc_Parse_type_spec(_T4F3,_T4F8);yyval=Cyc_YY21(_T4F9);goto _LL0;case 128: _T4FB=Cyc_List_map_c;{
# 1770 "parse.y"
struct Cyc_List_List*(*_T187A)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T4FB;_T4FA=_T187A;}_T4FC=yyyvsp;_T4FD=_T4FC[2];_T4FE=_T4FD.l;_T4FF=_T4FE.first_line;_T500=Cyc_Position_loc_to_seg(_T4FF);_T501=yyyvsp;_T502=& _T501[2].v;_T503=(union Cyc_YYSTYPE*)_T502;_T504=Cyc_yyget_YY41(_T503);{struct Cyc_List_List*ts=_T4FA(Cyc_Parse_typ2tvar,_T500,_T504);_T506=Cyc_List_map_c;{
struct Cyc_List_List*(*_T187A)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T506;_T505=_T187A;}_T507=yyyvsp;_T508=_T507[4];_T509=_T508.l;_T50A=_T509.first_line;_T50B=Cyc_Position_loc_to_seg(_T50A);_T50C=yyyvsp;_T50D=& _T50C[4].v;_T50E=(union Cyc_YYSTYPE*)_T50D;_T50F=Cyc_yyget_YY41(_T50E);{struct Cyc_List_List*exist_ts=_T505(Cyc_Parse_typ2tvar,_T50B,_T50F);_T510=yyyvsp;_T511=& _T510[5].v;_T512=(union Cyc_YYSTYPE*)_T511;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T512);_T514=ec_qb;_T515=(unsigned)_T514;
if(!_T515)goto _TL29D;_T516=ec_qb;_T513=*_T516;goto _TL29E;_TL29D:{struct _tuple28 _T187A;_T187A.f0=0;_T187A.f1=0;_T517=_T187A;}_T513=_T517;_TL29E: {struct _tuple28 _T187A=_T513;struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_List_List*ec=_T187C;struct Cyc_List_List*qb=_T187B;_T518=yyyvsp;_T519=& _T518[0].v;_T51A=(union Cyc_YYSTYPE*)_T519;_T51B=
Cyc_yyget_YY23(_T51A);_T51C=_T51B.f1;_T51D=yyyvsp;_T51E=& _T51D[1].v;_T51F=(union Cyc_YYSTYPE*)_T51E;_T520=Cyc_yyget_QualId_tok(_T51F);_T521=ts;_T522=exist_ts;_T523=ec;_T524=qb;_T525=yyyvsp;_T526=& _T525[6].v;_T527=(union Cyc_YYSTYPE*)_T526;_T528=
Cyc_yyget_YY25(_T527);_T529=yyyvsp;_T52A=& _T529[0].v;_T52B=(union Cyc_YYSTYPE*)_T52A;_T52C=Cyc_yyget_YY23(_T52B);_T52D=_T52C.f0;_T52E=Cyc_Absyn_aggrdecl_impl(_T522,_T523,_T524,_T528,_T52D);_T52F=yyyvsp;_T530=_T52F[0];_T531=_T530.l;_T532=_T531.first_line;_T533=
Cyc_Position_loc_to_seg(_T532);{
# 1774
struct Cyc_Absyn_TypeDecl*td=Cyc_Absyn_aggr_tdecl(_T51C,2U,_T520,_T521,_T52E,0,_T533);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T187D->tag=10;
# 1777
_T187D->f1=td;_T187D->f2=0;_T534=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T187D;}_T535=(void*)_T534;_T536=yyyvsp;_T537=_T536[0];_T538=_T537.l;_T539=_T538.first_line;_T53A=Cyc_Position_loc_to_seg(_T539);_T53B=Cyc_Parse_type_spec(_T535,_T53A);yyval=Cyc_YY21(_T53B);goto _LL0;}}}}}}case 129: _T53C=yyyvsp;_T53D=& _T53C[0].v;_T53E=(union Cyc_YYSTYPE*)_T53D;_T53F=
# 1780 "parse.y"
Cyc_yyget_YY23(_T53E);_T540=_T53F.f1;_T541=yyyvsp;_T542=& _T541[1].v;_T543=(union Cyc_YYSTYPE*)_T542;_T544=Cyc_yyget_QualId_tok(_T543);_T546=yyyvsp;_T547=& _T546[0].v;_T548=(union Cyc_YYSTYPE*)_T547;_T549=Cyc_yyget_YY23(_T548);_T54A=_T549.f0;if(!_T54A)goto _TL29F;{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));_T187A->v=(void*)1;_T54B=(struct Cyc_Core_Opt*)_T187A;}_T545=_T54B;goto _TL2A0;_TL29F: _T545=0;_TL2A0: _T54C=Cyc_Absyn_UnknownAggr(_T540,_T544,_T545);_T54D=yyyvsp;_T54E=& _T54D[2].v;_T54F=(union Cyc_YYSTYPE*)_T54E;_T550=
Cyc_yyget_YY41(_T54F);_T551=
# 1780
Cyc_Absyn_aggr_type(_T54C,_T550);_T552=yyyvsp;_T553=_T552[0];_T554=_T553.l;_T555=_T554.first_line;_T556=
Cyc_Position_loc_to_seg(_T555);_T557=
# 1780
Cyc_Parse_type_spec(_T551,_T556);yyval=Cyc_YY21(_T557);goto _LL0;case 130:{struct _tuple25 _T187A;
# 1785 "parse.y"
_T187A.f0=1;_T559=yyyvsp;_T55A=& _T559[1].v;_T55B=(union Cyc_YYSTYPE*)_T55A;_T187A.f1=Cyc_yyget_YY22(_T55B);_T558=_T187A;}yyval=Cyc_YY23(_T558);goto _LL0;case 131:{struct _tuple25 _T187A;
# 1786 "parse.y"
_T187A.f0=0;_T55D=yyyvsp;_T55E=& _T55D[0].v;_T55F=(union Cyc_YYSTYPE*)_T55E;_T187A.f1=Cyc_yyget_YY22(_T55F);_T55C=_T187A;}yyval=Cyc_YY23(_T55C);goto _LL0;case 132:
# 1789
 yyval=Cyc_YY22(0U);goto _LL0;case 133:
# 1790 "parse.y"
 yyval=Cyc_YY22(1U);goto _LL0;case 134:
# 1793
 yyval=Cyc_YY41(0);goto _LL0;case 135: _T560=yyyvsp;_T561=& _T560[1].v;_T562=(union Cyc_YYSTYPE*)_T561;_T563=
# 1794 "parse.y"
Cyc_yyget_YY41(_T562);_T564=Cyc_List_imp_rev(_T563);yyval=Cyc_YY41(_T564);goto _LL0;case 136:
# 1799 "parse.y"
 yyval=Cyc_YY25(0);goto _LL0;case 137:  {
# 1801 "parse.y"
struct Cyc_List_List*decls=0;_T565=yyyvsp;_T566=& _T565[0].v;_T567=(union Cyc_YYSTYPE*)_T566;{
struct Cyc_List_List*x=Cyc_yyget_YY26(_T567);_TL2A4: if(x!=0)goto _TL2A2;else{goto _TL2A3;}
_TL2A2: _T568=x;_T569=_T568->hd;_T56A=(struct Cyc_List_List*)_T569;_T56B=decls;decls=Cyc_List_imp_append(_T56A,_T56B);_T56C=x;
# 1802
x=_T56C->tl;goto _TL2A4;_TL2A3:;}{
# 1805
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags==0)goto _TL2A5;_T56E=Cyc_List_iter_c;{
void(*_T187A)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))_T56E;_T56D=_T187A;}_T56F=tags;_T570=decls;_T56D(Cyc_Parse_substitute_aggrfield_tags,_T56F,_T570);goto _TL2A6;_TL2A5: _TL2A6:
 yyval=Cyc_YY25(decls);goto _LL0;}}case 138:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T572=yyyvsp;_T573=& _T572[0].v;_T574=(union Cyc_YYSTYPE*)_T573;
# 1814 "parse.y"
_T187A->hd=Cyc_yyget_YY25(_T574);_T187A->tl=0;_T571=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY26(_T571);goto _LL0;case 139:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T576=yyyvsp;_T577=& _T576[1].v;_T578=(union Cyc_YYSTYPE*)_T577;
# 1815 "parse.y"
_T187A->hd=Cyc_yyget_YY25(_T578);_T579=yyyvsp;_T57A=& _T579[0].v;_T57B=(union Cyc_YYSTYPE*)_T57A;_T187A->tl=Cyc_yyget_YY26(_T57B);_T575=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY26(_T575);goto _LL0;case 140: _T57D=yyr;{struct _tuple11*_T187A=_region_malloc(_T57D,0U,sizeof(struct _tuple11));
# 1821 "parse.y"
_T187A->tl=0;_T57E=yyyvsp;_T57F=& _T57E[0].v;_T580=(union Cyc_YYSTYPE*)_T57F;_T187A->hd=Cyc_yyget_YY18(_T580);_T57C=(struct _tuple11*)_T187A;}yyval=Cyc_YY19(_T57C);goto _LL0;case 141: _T582=yyr;{struct _tuple11*_T187A=_region_malloc(_T582,0U,sizeof(struct _tuple11));_T583=yyyvsp;_T584=& _T583[0].v;_T585=(union Cyc_YYSTYPE*)_T584;
# 1823 "parse.y"
_T187A->tl=Cyc_yyget_YY19(_T585);_T586=yyyvsp;_T587=& _T586[2].v;_T588=(union Cyc_YYSTYPE*)_T587;_T187A->hd=Cyc_yyget_YY18(_T588);_T581=(struct _tuple11*)_T187A;}yyval=Cyc_YY19(_T581);goto _LL0;case 142:{struct _tuple12 _T187A;_T58A=yyyvsp;_T58B=& _T58A[0].v;_T58C=(union Cyc_YYSTYPE*)_T58B;
# 1827 "parse.y"
_T187A.f0=Cyc_yyget_YY28(_T58C);_T187A.f1=0;_T187A.f2=0;_T589=_T187A;}yyval=Cyc_YY18(_T589);goto _LL0;case 143:{struct _tuple12 _T187A;_T58E=yyyvsp;_T58F=& _T58E[0].v;_T590=(union Cyc_YYSTYPE*)_T58F;
# 1828 "parse.y"
_T187A.f0=Cyc_yyget_YY28(_T590);_T187A.f1=0;_T591=yyyvsp;_T592=& _T591[2].v;_T593=(union Cyc_YYSTYPE*)_T592;_T594=Cyc_yyget_YY63(_T593);_T595=yyyvsp;_T596=_T595[1];_T597=_T596.l;_T598=_T597.first_line;_T599=Cyc_Position_loc_to_seg(_T598);_T187A.f2=Cyc_Absyn_new_exp(_T594,_T599);_T58D=_T187A;}yyval=Cyc_YY18(_T58D);goto _LL0;case 144:{struct _tuple12 _T187A;_T59B=yyyvsp;_T59C=& _T59B[0].v;_T59D=(union Cyc_YYSTYPE*)_T59C;
# 1829 "parse.y"
_T187A.f0=Cyc_yyget_YY28(_T59D);_T59E=yyyvsp;_T59F=& _T59E[2].v;_T5A0=(union Cyc_YYSTYPE*)_T59F;_T187A.f1=Cyc_yyget_Exp_tok(_T5A0);_T187A.f2=0;_T59A=_T187A;}yyval=Cyc_YY18(_T59A);goto _LL0;case 145:{struct _tuple12 _T187A;_T5A2=yyyvsp;_T5A3=& _T5A2[0].v;_T5A4=(union Cyc_YYSTYPE*)_T5A3;
# 1830 "parse.y"
_T187A.f0=Cyc_yyget_YY28(_T5A4);_T5A5=yyyvsp;_T5A6=& _T5A5[4].v;_T5A7=(union Cyc_YYSTYPE*)_T5A6;_T187A.f1=Cyc_yyget_Exp_tok(_T5A7);_T5A8=yyyvsp;_T5A9=& _T5A8[2].v;_T5AA=(union Cyc_YYSTYPE*)_T5A9;_T5AB=Cyc_yyget_YY63(_T5AA);_T5AC=yyyvsp;_T5AD=_T5AC[1];_T5AE=_T5AD.l;_T5AF=_T5AE.first_line;_T5B0=Cyc_Position_loc_to_seg(_T5AF);_T187A.f2=Cyc_Absyn_new_exp(_T5AB,_T5B0);_T5A1=_T187A;}yyval=Cyc_YY18(_T5A1);goto _LL0;case 146:  {struct _RegionHandle _T187A=_new_region(0U,"temp");struct _RegionHandle*temp=& _T187A;_push_region(temp);_T5B1=yyyvsp;_T5B2=& _T5B1[0].v;_T5B3=(union Cyc_YYSTYPE*)_T5B2;{
# 1836 "parse.y"
struct _tuple26 _T187B=Cyc_yyget_YY36(_T5B3);struct Cyc_List_List*_T187C;struct Cyc_Parse_Type_specifier _T187D;struct Cyc_Absyn_Tqual _T187E;_T187E=_T187B.f0;_T187D=_T187B.f1;_T187C=_T187B.f2;{struct Cyc_Absyn_Tqual tq=_T187E;struct Cyc_Parse_Type_specifier tspecs=_T187D;struct Cyc_List_List*atts=_T187C;_T5B4=tq;_T5B5=_T5B4.loc;
if(_T5B5!=0U)goto _TL2A7;_T5B6=yyyvsp;_T5B7=_T5B6[0];_T5B8=_T5B7.l;_T5B9=_T5B8.first_line;tq.loc=Cyc_Position_loc_to_seg(_T5B9);goto _TL2A8;_TL2A7: _TL2A8: {
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;_T5BA=yyyvsp;_T5BB=& _T5BA[1].v;_T5BC=(union Cyc_YYSTYPE*)_T5BB;{
struct Cyc_List_List*x=Cyc_yyget_YY30(_T5BC);_TL2AC: if(x!=0)goto _TL2AA;else{goto _TL2AB;}
_TL2AA: _T5BD=x;_T5BE=_T5BD->hd;{struct _tuple12*_T187F=(struct _tuple12*)_T5BE;struct Cyc_Absyn_Exp*_T1880;struct Cyc_Absyn_Exp*_T1881;struct Cyc_Parse_Declarator _T1882;{struct _tuple12 _T1883=*_T187F;_T1882=_T1883.f0;_T1881=_T1883.f1;_T1880=_T1883.f2;}{struct Cyc_Parse_Declarator d=_T1882;struct Cyc_Absyn_Exp*wd=_T1881;struct Cyc_Absyn_Exp*wh=_T1880;_T5C0=temp;{struct _tuple13*_T1883=_region_malloc(_T5C0,0U,sizeof(struct _tuple13));
_T1883->tl=decls;_T1883->hd=d;_T5BF=(struct _tuple13*)_T1883;}decls=_T5BF;_T5C2=temp;{struct Cyc_List_List*_T1883=_region_malloc(_T5C2,0U,sizeof(struct Cyc_List_List));_T5C4=temp;{struct _tuple16*_T1884=_region_malloc(_T5C4,0U,sizeof(struct _tuple16));
_T1884->f0=wd;_T1884->f1=wh;_T5C3=(struct _tuple16*)_T1884;}_T1883->hd=_T5C3;_T1883->tl=widths_and_reqs;_T5C1=(struct Cyc_List_List*)_T1883;}widths_and_reqs=_T5C1;}}_T5C5=x;
# 1840
x=_T5C5->tl;goto _TL2AC;_TL2AB:;}_T5C6=tspecs;_T5C7=yyyvsp;_T5C8=_T5C7[0];_T5C9=_T5C8.l;_T5CA=_T5C9.first_line;_T5CB=
# 1845
Cyc_Position_loc_to_seg(_T5CA);{void*t=Cyc_Parse_speclist2typ(_T5C6,_T5CB);_T5CC=temp;_T5CD=temp;_T5CE=
# 1847
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);_T5CF=widths_and_reqs;{
# 1846
struct Cyc_List_List*info=Cyc_List_rzip(_T5CC,_T5CD,_T5CE,_T5CF);_T5D1=Cyc_List_map_c;{
# 1848
struct Cyc_List_List*(*_T187F)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))_T5D1;_T5D0=_T187F;}_T5D2=yyyvsp;_T5D3=_T5D2[0];_T5D4=_T5D3.l;_T5D5=_T5D4.first_line;_T5D6=Cyc_Position_loc_to_seg(_T5D5);_T5D7=info;_T5D8=_T5D0(Cyc_Parse_make_aggr_field,_T5D6,_T5D7);yyval=Cyc_YY25(_T5D8);_npop_handler(0);goto _LL0;}}}}}_pop_region();}case 147:{struct _tuple26 _T187A;_T5DA=yyyvsp;_T5DB=_T5DA[0];_T5DC=_T5DB.l;_T5DD=_T5DC.first_line;_T5DE=
# 1857 "parse.y"
Cyc_Position_loc_to_seg(_T5DD);_T187A.f0=Cyc_Absyn_empty_tqual(_T5DE);_T5DF=yyyvsp;_T5E0=& _T5DF[0].v;_T5E1=(union Cyc_YYSTYPE*)_T5E0;_T187A.f1=Cyc_yyget_YY21(_T5E1);_T187A.f2=0;_T5D9=_T187A;}yyval=Cyc_YY36(_T5D9);goto _LL0;case 148: _T5E2=yyyvsp;_T5E3=& _T5E2[1].v;_T5E4=(union Cyc_YYSTYPE*)_T5E3;{
# 1859 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T5E4);{struct _tuple26 _T187A;_T5E6=two;_T187A.f0=_T5E6.f0;_T5E7=yyyvsp;_T5E8=_T5E7[0];_T5E9=_T5E8.l;_T5EA=_T5E9.first_line;_T5EB=Cyc_Position_loc_to_seg(_T5EA);_T5EC=yyyvsp;_T5ED=& _T5EC[0].v;_T5EE=(union Cyc_YYSTYPE*)_T5ED;_T5EF=Cyc_yyget_YY21(_T5EE);_T5F0=two;_T5F1=_T5F0.f1;_T187A.f1=Cyc_Parse_combine_specifiers(_T5EB,_T5EF,_T5F1);_T5F2=two;_T187A.f2=_T5F2.f2;_T5E5=_T187A;}yyval=Cyc_YY36(_T5E5);goto _LL0;}case 149:{struct _tuple26 _T187A;_T5F4=yyyvsp;_T5F5=& _T5F4[0].v;_T5F6=(union Cyc_YYSTYPE*)_T5F5;
# 1861 "parse.y"
_T187A.f0=Cyc_yyget_YY24(_T5F6);_T187A.f1=Cyc_Parse_empty_spec(0U);_T187A.f2=0;_T5F3=_T187A;}yyval=Cyc_YY36(_T5F3);goto _LL0;case 150: _T5F7=yyyvsp;_T5F8=& _T5F7[1].v;_T5F9=(union Cyc_YYSTYPE*)_T5F8;{
# 1863 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T5F9);{struct _tuple26 _T187A;_T5FB=yyyvsp;_T5FC=& _T5FB[0].v;_T5FD=(union Cyc_YYSTYPE*)_T5FC;_T5FE=Cyc_yyget_YY24(_T5FD);_T5FF=two;_T600=_T5FF.f0;_T187A.f0=Cyc_Absyn_combine_tqual(_T5FE,_T600);_T601=two;_T187A.f1=_T601.f1;_T602=two;_T187A.f2=_T602.f2;_T5FA=_T187A;}yyval=Cyc_YY36(_T5FA);goto _LL0;}case 151:{struct _tuple26 _T187A;_T604=yyyvsp;_T605=_T604[0];_T606=_T605.l;_T607=_T606.first_line;_T608=
# 1865 "parse.y"
Cyc_Position_loc_to_seg(_T607);_T187A.f0=Cyc_Absyn_empty_tqual(_T608);_T187A.f1=Cyc_Parse_empty_spec(0U);_T609=yyyvsp;_T60A=& _T609[0].v;_T60B=(union Cyc_YYSTYPE*)_T60A;_T187A.f2=Cyc_yyget_YY46(_T60B);_T603=_T187A;}yyval=Cyc_YY36(_T603);goto _LL0;case 152: _T60C=yyyvsp;_T60D=& _T60C[1].v;_T60E=(union Cyc_YYSTYPE*)_T60D;{
# 1867 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T60E);{struct _tuple26 _T187A;_T610=two;_T187A.f0=_T610.f0;_T611=two;_T187A.f1=_T611.f1;_T612=yyyvsp;_T613=& _T612[0].v;_T614=(union Cyc_YYSTYPE*)_T613;_T615=Cyc_yyget_YY46(_T614);_T616=two;_T617=_T616.f2;_T187A.f2=Cyc_List_append(_T615,_T617);_T60F=_T187A;}yyval=Cyc_YY36(_T60F);goto _LL0;}case 153:{struct _tuple26 _T187A;_T619=yyyvsp;_T61A=_T619[0];_T61B=_T61A.l;_T61C=_T61B.first_line;_T61D=
# 1873 "parse.y"
Cyc_Position_loc_to_seg(_T61C);_T187A.f0=Cyc_Absyn_empty_tqual(_T61D);_T61E=yyyvsp;_T61F=& _T61E[0].v;_T620=(union Cyc_YYSTYPE*)_T61F;_T187A.f1=Cyc_yyget_YY21(_T620);_T187A.f2=0;_T618=_T187A;}yyval=Cyc_YY36(_T618);goto _LL0;case 154: _T621=yyyvsp;_T622=& _T621[1].v;_T623=(union Cyc_YYSTYPE*)_T622;{
# 1875 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T623);{struct _tuple26 _T187A;_T625=two;_T187A.f0=_T625.f0;_T626=yyyvsp;_T627=_T626[0];_T628=_T627.l;_T629=_T628.first_line;_T62A=Cyc_Position_loc_to_seg(_T629);_T62B=yyyvsp;_T62C=& _T62B[0].v;_T62D=(union Cyc_YYSTYPE*)_T62C;_T62E=Cyc_yyget_YY21(_T62D);_T62F=two;_T630=_T62F.f1;_T187A.f1=Cyc_Parse_combine_specifiers(_T62A,_T62E,_T630);_T631=two;_T187A.f2=_T631.f2;_T624=_T187A;}yyval=Cyc_YY36(_T624);goto _LL0;}case 155:{struct _tuple26 _T187A;_T633=yyyvsp;_T634=& _T633[0].v;_T635=(union Cyc_YYSTYPE*)_T634;
# 1877 "parse.y"
_T187A.f0=Cyc_yyget_YY24(_T635);_T187A.f1=Cyc_Parse_empty_spec(0U);_T187A.f2=0;_T632=_T187A;}yyval=Cyc_YY36(_T632);goto _LL0;case 156: _T636=yyyvsp;_T637=& _T636[1].v;_T638=(union Cyc_YYSTYPE*)_T637;{
# 1879 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T638);{struct _tuple26 _T187A;_T63A=yyyvsp;_T63B=& _T63A[0].v;_T63C=(union Cyc_YYSTYPE*)_T63B;_T63D=Cyc_yyget_YY24(_T63C);_T63E=two;_T63F=_T63E.f0;_T187A.f0=Cyc_Absyn_combine_tqual(_T63D,_T63F);_T640=two;_T187A.f1=_T640.f1;_T641=two;_T187A.f2=_T641.f2;_T639=_T187A;}yyval=Cyc_YY36(_T639);goto _LL0;}case 157:{struct _tuple26 _T187A;_T643=yyyvsp;_T644=_T643[0];_T645=_T644.l;_T646=_T645.first_line;_T647=
# 1881 "parse.y"
Cyc_Position_loc_to_seg(_T646);_T187A.f0=Cyc_Absyn_empty_tqual(_T647);_T187A.f1=Cyc_Parse_empty_spec(0U);_T648=yyyvsp;_T649=& _T648[0].v;_T64A=(union Cyc_YYSTYPE*)_T649;_T187A.f2=Cyc_yyget_YY46(_T64A);_T642=_T187A;}yyval=Cyc_YY36(_T642);goto _LL0;case 158: _T64B=yyyvsp;_T64C=& _T64B[1].v;_T64D=(union Cyc_YYSTYPE*)_T64C;{
# 1883 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T64D);{struct _tuple26 _T187A;_T64F=two;_T187A.f0=_T64F.f0;_T650=two;_T187A.f1=_T650.f1;_T651=yyyvsp;_T652=& _T651[0].v;_T653=(union Cyc_YYSTYPE*)_T652;_T654=Cyc_yyget_YY46(_T653);_T655=two;_T656=_T655.f2;_T187A.f2=Cyc_List_append(_T654,_T656);_T64E=_T187A;}yyval=Cyc_YY36(_T64E);goto _LL0;}case 159: _T658=yyr;{struct Cyc_List_List*_T187A=_region_malloc(_T658,0U,sizeof(struct Cyc_List_List));_T659=yyyvsp;_T65A=& _T659[0].v;_T65B=(union Cyc_YYSTYPE*)_T65A;
# 1889 "parse.y"
_T187A->hd=Cyc_yyget_YY29(_T65B);_T187A->tl=0;_T657=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY30(_T657);goto _LL0;case 160: _T65D=yyr;{struct Cyc_List_List*_T187A=_region_malloc(_T65D,0U,sizeof(struct Cyc_List_List));_T65E=yyyvsp;_T65F=& _T65E[2].v;_T660=(union Cyc_YYSTYPE*)_T65F;
# 1891 "parse.y"
_T187A->hd=Cyc_yyget_YY29(_T660);_T661=yyyvsp;_T662=& _T661[0].v;_T663=(union Cyc_YYSTYPE*)_T662;_T187A->tl=Cyc_yyget_YY30(_T663);_T65C=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY30(_T65C);goto _LL0;case 161: _T665=yyr;{struct _tuple12*_T187A=_region_malloc(_T665,0U,sizeof(struct _tuple12));_T666=yyyvsp;_T667=& _T666[0].v;_T668=(union Cyc_YYSTYPE*)_T667;
# 1896 "parse.y"
_T187A->f0=Cyc_yyget_YY28(_T668);_T187A->f1=0;_T669=yyyvsp;_T66A=& _T669[1].v;_T66B=(union Cyc_YYSTYPE*)_T66A;_T187A->f2=Cyc_yyget_YY61(_T66B);_T664=(struct _tuple12*)_T187A;}yyval=Cyc_YY29(_T664);goto _LL0;case 162: _T66D=yyr;{struct _tuple12*_T187A=_region_malloc(_T66D,0U,sizeof(struct _tuple12));{struct _tuple0*_T187B=_cycalloc(sizeof(struct _tuple0));
# 1900 "parse.y"
_T187B->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));*_T187C=_tag_fat("",sizeof(char),1U);_T66F=(struct _fat_ptr*)_T187C;}_T187B->f1=_T66F;_T66E=(struct _tuple0*)_T187B;}_T187A->f0.id=_T66E;_T187A->f0.varloc=0U;_T187A->f0.tms=0;_T670=yyyvsp;_T671=& _T670[1].v;_T672=(union Cyc_YYSTYPE*)_T671;_T187A->f1=Cyc_yyget_Exp_tok(_T672);_T187A->f2=0;_T66C=(struct _tuple12*)_T187A;}yyval=Cyc_YY29(_T66C);goto _LL0;case 163: _T674=yyr;{struct _tuple12*_T187A=_region_malloc(_T674,0U,sizeof(struct _tuple12));{struct _tuple0*_T187B=_cycalloc(sizeof(struct _tuple0));
# 1905 "parse.y"
_T187B->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));*_T187C=_tag_fat("",sizeof(char),1U);_T676=(struct _fat_ptr*)_T187C;}_T187B->f1=_T676;_T675=(struct _tuple0*)_T187B;}_T187A->f0.id=_T675;_T187A->f0.varloc=0U;_T187A->f0.tms=0;_T187A->f1=0;_T187A->f2=0;_T673=(struct _tuple12*)_T187A;}yyval=Cyc_YY29(_T673);goto _LL0;case 164: _T678=yyr;{struct _tuple12*_T187A=_region_malloc(_T678,0U,sizeof(struct _tuple12));_T679=yyyvsp;_T67A=& _T679[0].v;_T67B=(union Cyc_YYSTYPE*)_T67A;
# 1908 "parse.y"
_T187A->f0=Cyc_yyget_YY28(_T67B);_T67C=yyyvsp;_T67D=& _T67C[2].v;_T67E=(union Cyc_YYSTYPE*)_T67D;_T187A->f1=Cyc_yyget_Exp_tok(_T67E);_T187A->f2=0;_T677=(struct _tuple12*)_T187A;}yyval=Cyc_YY29(_T677);goto _LL0;case 165: _T67F=yyyvsp;_T680=& _T67F[2].v;_T681=(union Cyc_YYSTYPE*)_T680;_T682=
# 1912 "parse.y"
Cyc_yyget_Exp_tok(_T681);yyval=Cyc_YY61(_T682);goto _LL0;case 166:
# 1913 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 167: _T683=yyyvsp;_T684=& _T683[0].v;_T685=(union Cyc_YYSTYPE*)_T684;{
# 1919 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T685);_T687=Cyc_List_map_c;{
struct Cyc_List_List*(*_T187A)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T687;_T686=_T187A;}_T688=yyyvsp;_T689=_T688[2];_T68A=_T689.l;_T68B=_T68A.first_line;_T68C=Cyc_Position_loc_to_seg(_T68B);_T68D=yyyvsp;_T68E=& _T68D[2].v;_T68F=(union Cyc_YYSTYPE*)_T68E;_T690=Cyc_yyget_YY41(_T68F);{struct Cyc_List_List*ts=_T686(Cyc_Parse_typ2tvar,_T68C,_T690);_T691=yyyvsp;_T692=& _T691[1].v;_T693=(union Cyc_YYSTYPE*)_T692;_T694=
Cyc_yyget_QualId_tok(_T693);_T695=ts;{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));_T697=yyyvsp;_T698=& _T697[4].v;_T699=(union Cyc_YYSTYPE*)_T698;_T187A->v=Cyc_yyget_YY35(_T699);_T696=(struct Cyc_Core_Opt*)_T187A;}_T69A=is_extensible;_T69B=yyyvsp;_T69C=_T69B[0];_T69D=_T69C.l;_T69E=_T69D.first_line;_T69F=
Cyc_Position_loc_to_seg(_T69E);{
# 1921
struct Cyc_Absyn_TypeDecl*dd=Cyc_Absyn_datatype_tdecl(2U,_T694,_T695,_T696,_T69A,_T69F);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T187A->tag=10;
# 1923
_T187A->f1=dd;_T187A->f2=0;_T6A0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T187A;}_T6A1=(void*)_T6A0;_T6A2=yyyvsp;_T6A3=_T6A2[0];_T6A4=_T6A3.l;_T6A5=_T6A4.first_line;_T6A6=Cyc_Position_loc_to_seg(_T6A5);_T6A7=Cyc_Parse_type_spec(_T6A1,_T6A6);yyval=Cyc_YY21(_T6A7);goto _LL0;}}}case 168: _T6A8=yyyvsp;_T6A9=& _T6A8[0].v;_T6AA=(union Cyc_YYSTYPE*)_T6A9;{
# 1926 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T6AA);{struct Cyc_Absyn_UnknownDatatypeInfo _T187A;_T6AC=yyyvsp;_T6AD=& _T6AC[1].v;_T6AE=(union Cyc_YYSTYPE*)_T6AD;
_T187A.name=Cyc_yyget_QualId_tok(_T6AE);_T187A.is_extensible=is_extensible;_T6AB=_T187A;}_T6AF=Cyc_Absyn_UnknownDatatype(_T6AB);_T6B0=yyyvsp;_T6B1=& _T6B0[2].v;_T6B2=(union Cyc_YYSTYPE*)_T6B1;_T6B3=Cyc_yyget_YY41(_T6B2);_T6B4=Cyc_Absyn_datatype_type(_T6AF,_T6B3);_T6B5=yyyvsp;_T6B6=_T6B5[0];_T6B7=_T6B6.l;_T6B8=_T6B7.first_line;_T6B9=Cyc_Position_loc_to_seg(_T6B8);_T6BA=Cyc_Parse_type_spec(_T6B4,_T6B9);yyval=Cyc_YY21(_T6BA);goto _LL0;}case 169: _T6BB=yyyvsp;_T6BC=& _T6BB[0].v;_T6BD=(union Cyc_YYSTYPE*)_T6BC;{
# 1930 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T6BD);{struct Cyc_Absyn_UnknownDatatypeFieldInfo _T187A;_T6BF=yyyvsp;_T6C0=& _T6BF[1].v;_T6C1=(union Cyc_YYSTYPE*)_T6C0;
_T187A.datatype_name=Cyc_yyget_QualId_tok(_T6C1);_T6C2=yyyvsp;_T6C3=& _T6C2[3].v;_T6C4=(union Cyc_YYSTYPE*)_T6C3;_T187A.field_name=Cyc_yyget_QualId_tok(_T6C4);_T187A.is_extensible=is_extensible;_T6BE=_T187A;}_T6C5=Cyc_Absyn_UnknownDatatypefield(_T6BE);_T6C6=yyyvsp;_T6C7=& _T6C6[4].v;_T6C8=(union Cyc_YYSTYPE*)_T6C7;_T6C9=Cyc_yyget_YY41(_T6C8);_T6CA=Cyc_Absyn_datatype_field_type(_T6C5,_T6C9);_T6CB=yyyvsp;_T6CC=_T6CB[0];_T6CD=_T6CC.l;_T6CE=_T6CD.first_line;_T6CF=Cyc_Position_loc_to_seg(_T6CE);_T6D0=Cyc_Parse_type_spec(_T6CA,_T6CF);yyval=Cyc_YY21(_T6D0);goto _LL0;}case 170:
# 1936 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 171:
# 1937 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 172:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T6D2=yyyvsp;_T6D3=& _T6D2[0].v;_T6D4=(union Cyc_YYSTYPE*)_T6D3;
# 1941 "parse.y"
_T187A->hd=Cyc_yyget_YY34(_T6D4);_T187A->tl=0;_T6D1=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY35(_T6D1);goto _LL0;case 173:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T6D6=yyyvsp;_T6D7=& _T6D6[0].v;_T6D8=(union Cyc_YYSTYPE*)_T6D7;
# 1942 "parse.y"
_T187A->hd=Cyc_yyget_YY34(_T6D8);_T187A->tl=0;_T6D5=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY35(_T6D5);goto _LL0;case 174:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T6DA=yyyvsp;_T6DB=& _T6DA[0].v;_T6DC=(union Cyc_YYSTYPE*)_T6DB;
# 1943 "parse.y"
_T187A->hd=Cyc_yyget_YY34(_T6DC);_T6DD=yyyvsp;_T6DE=& _T6DD[2].v;_T6DF=(union Cyc_YYSTYPE*)_T6DE;_T187A->tl=Cyc_yyget_YY35(_T6DF);_T6D9=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY35(_T6D9);goto _LL0;case 175:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T6E1=yyyvsp;_T6E2=& _T6E1[0].v;_T6E3=(union Cyc_YYSTYPE*)_T6E2;
# 1944 "parse.y"
_T187A->hd=Cyc_yyget_YY34(_T6E3);_T6E4=yyyvsp;_T6E5=& _T6E4[2].v;_T6E6=(union Cyc_YYSTYPE*)_T6E5;_T187A->tl=Cyc_yyget_YY35(_T6E6);_T6E0=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY35(_T6E0);goto _LL0;case 176:
# 1948 "parse.y"
 yyval=Cyc_YY33(2U);goto _LL0;case 177:
# 1949 "parse.y"
 yyval=Cyc_YY33(3U);goto _LL0;case 178:
# 1950 "parse.y"
 yyval=Cyc_YY33(0U);goto _LL0;case 179:{struct Cyc_Absyn_Datatypefield*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T6E8=yyyvsp;_T6E9=& _T6E8[1].v;_T6EA=(union Cyc_YYSTYPE*)_T6E9;
# 1954 "parse.y"
_T187A->name=Cyc_yyget_QualId_tok(_T6EA);_T187A->typs=0;_T6EB=yyyvsp;_T6EC=_T6EB[0];_T6ED=_T6EC.l;_T6EE=_T6ED.first_line;_T187A->loc=Cyc_Position_loc_to_seg(_T6EE);_T6EF=yyyvsp;_T6F0=& _T6EF[0].v;_T6F1=(union Cyc_YYSTYPE*)_T6F0;_T187A->sc=Cyc_yyget_YY33(_T6F1);_T6E7=(struct Cyc_Absyn_Datatypefield*)_T187A;}yyval=Cyc_YY34(_T6E7);goto _LL0;case 180: _T6F3=Cyc_List_map_c;{
# 1956 "parse.y"
struct Cyc_List_List*(*_T187A)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T6F3;_T6F2=_T187A;}{struct _tuple19*(*_T187A)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T6F4=_T187A;}_T6F5=yyyvsp;_T6F6=_T6F5[3];_T6F7=_T6F6.l;_T6F8=_T6F7.first_line;_T6F9=Cyc_Position_loc_to_seg(_T6F8);_T6FA=yyyvsp;_T6FB=& _T6FA[3].v;_T6FC=(union Cyc_YYSTYPE*)_T6FB;_T6FD=Cyc_yyget_YY39(_T6FC);_T6FE=Cyc_List_imp_rev(_T6FD);{struct Cyc_List_List*typs=_T6F2(_T6F4,_T6F9,_T6FE);{struct Cyc_Absyn_Datatypefield*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T700=yyyvsp;_T701=& _T700[1].v;_T702=(union Cyc_YYSTYPE*)_T701;
_T187A->name=Cyc_yyget_QualId_tok(_T702);_T187A->typs=typs;_T703=yyyvsp;_T704=_T703[0];_T705=_T704.l;_T706=_T705.first_line;_T187A->loc=Cyc_Position_loc_to_seg(_T706);_T707=yyyvsp;_T708=& _T707[0].v;_T709=(union Cyc_YYSTYPE*)_T708;_T187A->sc=Cyc_yyget_YY33(_T709);_T6FF=(struct Cyc_Absyn_Datatypefield*)_T187A;}yyval=Cyc_YY34(_T6FF);goto _LL0;}case 181: _T70A=yyyvsp;_T70B=_T70A[0];
# 1962 "parse.y"
yyval=_T70B.v;goto _LL0;case 182: _T70C=yyyvsp;_T70D=& _T70C[1].v;_T70E=(union Cyc_YYSTYPE*)_T70D;{
# 1964 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T70E);{struct Cyc_Parse_Declarator _T187A;_T710=two;
_T187A.id=_T710.id;_T711=two;_T187A.varloc=_T711.varloc;_T712=yyyvsp;_T713=& _T712[0].v;_T714=(union Cyc_YYSTYPE*)_T713;_T715=Cyc_yyget_YY27(_T714);_T716=two;_T717=_T716.tms;_T187A.tms=Cyc_List_imp_append(_T715,_T717);_T70F=_T187A;}yyval=Cyc_YY28(_T70F);goto _LL0;}case 183: _T718=yyyvsp;_T719=_T718[0];
# 1971 "parse.y"
yyval=_T719.v;goto _LL0;case 184: _T71A=yyyvsp;_T71B=& _T71A[1].v;_T71C=(union Cyc_YYSTYPE*)_T71B;{
# 1973 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T71C);{struct Cyc_Parse_Declarator _T187A;_T71E=two;
_T187A.id=_T71E.id;_T71F=two;_T187A.varloc=_T71F.varloc;_T720=yyyvsp;_T721=& _T720[0].v;_T722=(union Cyc_YYSTYPE*)_T721;_T723=Cyc_yyget_YY27(_T722);_T724=two;_T725=_T724.tms;_T187A.tms=Cyc_List_imp_append(_T723,_T725);_T71D=_T187A;}yyval=Cyc_YY28(_T71D);goto _LL0;}case 185:{struct Cyc_Parse_Declarator _T187A;_T727=yyyvsp;_T728=& _T727[0].v;_T729=(union Cyc_YYSTYPE*)_T728;
# 1978 "parse.y"
_T187A.id=Cyc_yyget_QualId_tok(_T729);_T72A=yyyvsp;_T72B=_T72A[0];_T72C=_T72B.l;_T72D=_T72C.first_line;_T187A.varloc=Cyc_Position_loc_to_seg(_T72D);_T187A.tms=0;_T726=_T187A;}yyval=Cyc_YY28(_T726);goto _LL0;case 186: _T72E=yyyvsp;_T72F=_T72E[1];
# 1979 "parse.y"
yyval=_T72F.v;goto _LL0;case 187: _T730=yyyvsp;_T731=& _T730[2].v;_T732=(union Cyc_YYSTYPE*)_T731;{
# 1983 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T732);_T734=yyr;{struct Cyc_List_List*_T187A=_region_malloc(_T734,0U,sizeof(struct Cyc_List_List));_T736=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T187B=_region_malloc(_T736,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T187B->tag=5;_T737=yyyvsp;_T738=_T737[1];_T739=_T738.l;_T73A=_T739.first_line;
_T187B->f1=Cyc_Position_loc_to_seg(_T73A);_T73B=yyyvsp;_T73C=& _T73B[1].v;_T73D=(union Cyc_YYSTYPE*)_T73C;_T187B->f2=Cyc_yyget_YY46(_T73D);_T735=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T187B;}_T187A->hd=(void*)_T735;_T73E=d;_T187A->tl=_T73E.tms;_T733=(struct Cyc_List_List*)_T187A;}d.tms=_T733;_T73F=yyyvsp;_T740=_T73F[2];
yyval=_T740.v;goto _LL0;}case 188:{struct Cyc_Parse_Declarator _T187A;_T742=yyyvsp;_T743=& _T742[0].v;_T744=(union Cyc_YYSTYPE*)_T743;_T745=
# 1988 "parse.y"
Cyc_yyget_YY28(_T744);_T187A.id=_T745.id;_T746=yyyvsp;_T747=& _T746[0].v;_T748=(union Cyc_YYSTYPE*)_T747;_T749=Cyc_yyget_YY28(_T748);_T187A.varloc=_T749.varloc;_T74B=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T74B,0U,sizeof(struct Cyc_List_List));_T74D=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T74D,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T187C->tag=0;_T74E=yyyvsp;_T74F=& _T74E[3].v;_T750=(union Cyc_YYSTYPE*)_T74F;_T187C->f1=Cyc_yyget_YY54(_T750);_T751=yyyvsp;_T752=_T751[3];_T753=_T752.l;_T754=_T753.first_line;_T187C->f2=Cyc_Position_loc_to_seg(_T754);_T74C=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T74C;_T755=yyyvsp;_T756=& _T755[0].v;_T757=(union Cyc_YYSTYPE*)_T756;_T758=Cyc_yyget_YY28(_T757);_T187B->tl=_T758.tms;_T74A=(struct Cyc_List_List*)_T187B;}_T187A.tms=_T74A;_T741=_T187A;}yyval=Cyc_YY28(_T741);goto _LL0;case 189:{struct Cyc_Parse_Declarator _T187A;_T75A=yyyvsp;_T75B=& _T75A[0].v;_T75C=(union Cyc_YYSTYPE*)_T75B;_T75D=
# 1990 "parse.y"
Cyc_yyget_YY28(_T75C);_T187A.id=_T75D.id;_T75E=yyyvsp;_T75F=& _T75E[0].v;_T760=(union Cyc_YYSTYPE*)_T75F;_T761=Cyc_yyget_YY28(_T760);_T187A.varloc=_T761.varloc;_T763=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T763,0U,sizeof(struct Cyc_List_List));_T765=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T765,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T187C->tag=1;_T766=yyyvsp;_T767=& _T766[2].v;_T768=(union Cyc_YYSTYPE*)_T767;
_T187C->f1=Cyc_yyget_Exp_tok(_T768);_T769=yyyvsp;_T76A=& _T769[4].v;_T76B=(union Cyc_YYSTYPE*)_T76A;_T187C->f2=Cyc_yyget_YY54(_T76B);_T76C=yyyvsp;_T76D=_T76C[4];_T76E=_T76D.l;_T76F=_T76E.first_line;_T187C->f3=Cyc_Position_loc_to_seg(_T76F);_T764=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T764;_T770=yyyvsp;_T771=& _T770[0].v;_T772=(union Cyc_YYSTYPE*)_T771;_T773=Cyc_yyget_YY28(_T772);_T187B->tl=_T773.tms;_T762=(struct Cyc_List_List*)_T187B;}_T187A.tms=_T762;_T759=_T187A;}
# 1990
yyval=Cyc_YY28(_T759);goto _LL0;case 190: _T774=yyyvsp;_T775=& _T774[2].v;_T776=(union Cyc_YYSTYPE*)_T775;{
# 1993 "parse.y"
struct _tuple27*_T187A=Cyc_yyget_YY40(_T776);struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;void*_T187D;struct Cyc_Absyn_VarargInfo*_T187E;int _T187F;struct Cyc_List_List*_T1880;{struct _tuple27 _T1881=*_T187A;_T1880=_T1881.f0;_T187F=_T1881.f1;_T187E=_T1881.f2;_T187D=_T1881.f3;_T187C=_T1881.f4;_T187B=_T1881.f5;}{struct Cyc_List_List*lis=_T1880;int b=_T187F;struct Cyc_Absyn_VarargInfo*c=_T187E;void*eff=_T187D;struct Cyc_List_List*ec=_T187C;struct Cyc_List_List*qb=_T187B;_T777=yyyvsp;_T778=& _T777[4].v;_T779=(union Cyc_YYSTYPE*)_T778;{
struct _tuple21 _T1881=Cyc_yyget_YY62(_T779);struct Cyc_Absyn_Exp*_T1882;struct Cyc_Absyn_Exp*_T1883;struct Cyc_Absyn_Exp*_T1884;struct Cyc_Absyn_Exp*_T1885;_T1885=_T1881.f0;_T1884=_T1881.f1;_T1883=_T1881.f2;_T1882=_T1881.f3;{struct Cyc_Absyn_Exp*chk=_T1885;struct Cyc_Absyn_Exp*req=_T1884;struct Cyc_Absyn_Exp*ens=_T1883;struct Cyc_Absyn_Exp*thrws=_T1882;{struct Cyc_Parse_Declarator _T1886;_T77B=yyyvsp;_T77C=& _T77B[0].v;_T77D=(union Cyc_YYSTYPE*)_T77C;_T77E=
Cyc_yyget_YY28(_T77D);_T1886.id=_T77E.id;_T77F=yyyvsp;_T780=& _T77F[0].v;_T781=(union Cyc_YYSTYPE*)_T780;_T782=Cyc_yyget_YY28(_T781);_T1886.varloc=_T782.varloc;_T784=yyr;{struct Cyc_List_List*_T1887=_region_malloc(_T784,0U,sizeof(struct Cyc_List_List));_T786=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1888=_region_malloc(_T786,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1888->tag=3;_T788=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1889=_region_malloc(_T788,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1889->tag=1;_T1889->f1=lis;_T1889->f2=b;_T1889->f3=c;_T1889->f4=eff;_T1889->f5=ec;_T1889->f6=qb;_T1889->f7=chk;_T1889->f8=req;_T1889->f9=ens;_T1889->f10=thrws;_T787=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1889;}_T1888->f1=(void*)_T787;_T785=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1888;}_T1887->hd=(void*)_T785;_T789=yyyvsp;_T78A=& _T789[0].v;_T78B=(union Cyc_YYSTYPE*)_T78A;_T78C=Cyc_yyget_YY28(_T78B);_T1887->tl=_T78C.tms;_T783=(struct Cyc_List_List*)_T1887;}_T1886.tms=_T783;_T77A=_T1886;}yyval=Cyc_YY28(_T77A);goto _LL0;}}}}case 191:{struct Cyc_Parse_Declarator _T187A;_T78E=yyyvsp;_T78F=& _T78E[0].v;_T790=(union Cyc_YYSTYPE*)_T78F;_T791=
# 1998 "parse.y"
Cyc_yyget_YY28(_T790);_T187A.id=_T791.id;_T792=yyyvsp;_T793=& _T792[0].v;_T794=(union Cyc_YYSTYPE*)_T793;_T795=Cyc_yyget_YY28(_T794);_T187A.varloc=_T795.varloc;_T797=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T797,0U,sizeof(struct Cyc_List_List));_T799=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T799,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T187C->tag=3;_T79B=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T187D=_region_malloc(_T79B,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T187D->tag=0;_T79C=yyyvsp;_T79D=& _T79C[2].v;_T79E=(union Cyc_YYSTYPE*)_T79D;_T187D->f1=Cyc_yyget_YY37(_T79E);_T79F=yyyvsp;_T7A0=_T79F[0];_T7A1=_T7A0.l;_T7A2=_T7A1.first_line;_T187D->f2=Cyc_Position_loc_to_seg(_T7A2);_T79A=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T187D;}_T187C->f1=(void*)_T79A;_T798=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T798;_T7A3=yyyvsp;_T7A4=& _T7A3[0].v;_T7A5=(union Cyc_YYSTYPE*)_T7A4;_T7A6=Cyc_yyget_YY28(_T7A5);_T187B->tl=_T7A6.tms;_T796=(struct Cyc_List_List*)_T187B;}_T187A.tms=_T796;_T78D=_T187A;}yyval=Cyc_YY28(_T78D);goto _LL0;case 192: _T7A8=Cyc_List_map_c;{
# 2001
struct Cyc_List_List*(*_T187A)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T7A8;_T7A7=_T187A;}_T7A9=yyyvsp;_T7AA=_T7A9[1];_T7AB=_T7AA.l;_T7AC=_T7AB.first_line;_T7AD=Cyc_Position_loc_to_seg(_T7AC);_T7AE=yyyvsp;_T7AF=& _T7AE[2].v;_T7B0=(union Cyc_YYSTYPE*)_T7AF;_T7B1=Cyc_yyget_YY41(_T7B0);_T7B2=Cyc_List_imp_rev(_T7B1);{struct Cyc_List_List*ts=_T7A7(Cyc_Parse_typ2tvar,_T7AD,_T7B2);{struct Cyc_Parse_Declarator _T187A;_T7B4=yyyvsp;_T7B5=& _T7B4[0].v;_T7B6=(union Cyc_YYSTYPE*)_T7B5;_T7B7=
Cyc_yyget_YY28(_T7B6);_T187A.id=_T7B7.id;_T7B8=yyyvsp;_T7B9=& _T7B8[0].v;_T7BA=(union Cyc_YYSTYPE*)_T7B9;_T7BB=Cyc_yyget_YY28(_T7BA);_T187A.varloc=_T7BB.varloc;_T7BD=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T7BD,0U,sizeof(struct Cyc_List_List));_T7BF=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T7BF,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T187C->tag=4;_T187C->f1=ts;_T7C0=yyyvsp;_T7C1=_T7C0[0];_T7C2=_T7C1.l;_T7C3=_T7C2.first_line;_T187C->f2=Cyc_Position_loc_to_seg(_T7C3);_T187C->f3=0;_T7BE=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T7BE;_T7C4=yyyvsp;_T7C5=& _T7C4[0].v;_T7C6=(union Cyc_YYSTYPE*)_T7C5;_T7C7=Cyc_yyget_YY28(_T7C6);_T187B->tl=_T7C7.tms;_T7BC=(struct Cyc_List_List*)_T187B;}_T187A.tms=_T7BC;_T7B3=_T187A;}yyval=Cyc_YY28(_T7B3);goto _LL0;}case 193:{struct Cyc_Parse_Declarator _T187A;_T7C9=yyyvsp;_T7CA=& _T7C9[0].v;_T7CB=(union Cyc_YYSTYPE*)_T7CA;_T7CC=
# 2005 "parse.y"
Cyc_yyget_YY28(_T7CB);_T187A.id=_T7CC.id;_T7CD=yyyvsp;_T7CE=& _T7CD[0].v;_T7CF=(union Cyc_YYSTYPE*)_T7CE;_T7D0=Cyc_yyget_YY28(_T7CF);_T187A.varloc=_T7D0.varloc;_T7D2=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T7D2,0U,sizeof(struct Cyc_List_List));_T7D4=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T7D4,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T187C->tag=5;_T7D5=yyyvsp;_T7D6=_T7D5[1];_T7D7=_T7D6.l;_T7D8=_T7D7.first_line;_T187C->f1=Cyc_Position_loc_to_seg(_T7D8);_T7D9=yyyvsp;_T7DA=& _T7D9[1].v;_T7DB=(union Cyc_YYSTYPE*)_T7DA;_T187C->f2=Cyc_yyget_YY46(_T7DB);_T7D3=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T7D3;_T7DC=yyyvsp;_T7DD=& _T7DC[0].v;_T7DE=(union Cyc_YYSTYPE*)_T7DD;_T7DF=
Cyc_yyget_YY28(_T7DE);_T187B->tl=_T7DF.tms;_T7D1=(struct Cyc_List_List*)_T187B;}
# 2005
_T187A.tms=_T7D1;_T7C8=_T187A;}yyval=Cyc_YY28(_T7C8);goto _LL0;case 194:{struct Cyc_Parse_Declarator _T187A;_T7E1=yyyvsp;_T7E2=& _T7E1[0].v;_T7E3=(union Cyc_YYSTYPE*)_T7E2;
# 2012 "parse.y"
_T187A.id=Cyc_yyget_QualId_tok(_T7E3);_T7E4=yyyvsp;_T7E5=_T7E4[0];_T7E6=_T7E5.l;_T7E7=_T7E6.first_line;_T187A.varloc=Cyc_Position_loc_to_seg(_T7E7);_T187A.tms=0;_T7E0=_T187A;}yyval=Cyc_YY28(_T7E0);goto _LL0;case 195:{struct Cyc_Parse_Declarator _T187A;_T7E9=yyyvsp;_T7EA=& _T7E9[0].v;_T7EB=(union Cyc_YYSTYPE*)_T7EA;
# 2013 "parse.y"
_T187A.id=Cyc_yyget_QualId_tok(_T7EB);_T7EC=yyyvsp;_T7ED=_T7EC[0];_T7EE=_T7ED.l;_T7EF=_T7EE.first_line;_T187A.varloc=Cyc_Position_loc_to_seg(_T7EF);_T187A.tms=0;_T7E8=_T187A;}yyval=Cyc_YY28(_T7E8);goto _LL0;case 196: _T7F0=yyyvsp;_T7F1=_T7F0[1];
# 2014 "parse.y"
yyval=_T7F1.v;goto _LL0;case 197: _T7F2=yyyvsp;_T7F3=& _T7F2[2].v;_T7F4=(union Cyc_YYSTYPE*)_T7F3;{
# 2018 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T7F4);_T7F6=yyr;{struct Cyc_List_List*_T187A=_region_malloc(_T7F6,0U,sizeof(struct Cyc_List_List));_T7F8=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T187B=_region_malloc(_T7F8,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T187B->tag=5;_T7F9=yyyvsp;_T7FA=_T7F9[1];_T7FB=_T7FA.l;_T7FC=_T7FB.first_line;
_T187B->f1=Cyc_Position_loc_to_seg(_T7FC);_T7FD=yyyvsp;_T7FE=& _T7FD[1].v;_T7FF=(union Cyc_YYSTYPE*)_T7FE;_T187B->f2=Cyc_yyget_YY46(_T7FF);_T7F7=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T187B;}_T187A->hd=(void*)_T7F7;_T800=d;_T187A->tl=_T800.tms;_T7F5=(struct Cyc_List_List*)_T187A;}d.tms=_T7F5;_T801=yyyvsp;_T802=_T801[2];
yyval=_T802.v;goto _LL0;}case 198: _T803=yyyvsp;_T804=& _T803[0].v;_T805=(union Cyc_YYSTYPE*)_T804;{
# 2023 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T805);{struct Cyc_Parse_Declarator _T187A;_T807=one;
_T187A.id=_T807.id;_T808=one;_T187A.varloc=_T808.varloc;_T80A=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T80A,0U,sizeof(struct Cyc_List_List));_T80C=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T80C,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T187C->tag=0;_T80D=yyyvsp;_T80E=& _T80D[3].v;_T80F=(union Cyc_YYSTYPE*)_T80E;
_T187C->f1=Cyc_yyget_YY54(_T80F);_T810=yyyvsp;_T811=_T810[3];_T812=_T811.l;_T813=_T812.first_line;_T187C->f2=Cyc_Position_loc_to_seg(_T813);_T80B=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T80B;_T814=one;_T187B->tl=_T814.tms;_T809=(struct Cyc_List_List*)_T187B;}_T187A.tms=_T809;_T806=_T187A;}
# 2024
yyval=Cyc_YY28(_T806);goto _LL0;}case 199: _T815=yyyvsp;_T816=& _T815[0].v;_T817=(union Cyc_YYSTYPE*)_T816;{
# 2027 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T817);{struct Cyc_Parse_Declarator _T187A;_T819=one;
_T187A.id=_T819.id;_T81A=one;_T187A.varloc=_T81A.varloc;_T81C=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T81C,0U,sizeof(struct Cyc_List_List));_T81E=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T81E,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T187C->tag=1;_T81F=yyyvsp;_T820=& _T81F[2].v;_T821=(union Cyc_YYSTYPE*)_T820;
_T187C->f1=Cyc_yyget_Exp_tok(_T821);_T822=yyyvsp;_T823=& _T822[4].v;_T824=(union Cyc_YYSTYPE*)_T823;_T187C->f2=Cyc_yyget_YY54(_T824);_T825=yyyvsp;_T826=_T825[4];_T827=_T826.l;_T828=_T827.first_line;_T187C->f3=Cyc_Position_loc_to_seg(_T828);_T81D=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T81D;_T829=one;
_T187B->tl=_T829.tms;_T81B=(struct Cyc_List_List*)_T187B;}
# 2029
_T187A.tms=_T81B;_T818=_T187A;}
# 2028
yyval=Cyc_YY28(_T818);goto _LL0;}case 200: _T82A=yyyvsp;_T82B=& _T82A[2].v;_T82C=(union Cyc_YYSTYPE*)_T82B;{
# 2032 "parse.y"
struct _tuple27*_T187A=Cyc_yyget_YY40(_T82C);struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;void*_T187D;struct Cyc_Absyn_VarargInfo*_T187E;int _T187F;struct Cyc_List_List*_T1880;{struct _tuple27 _T1881=*_T187A;_T1880=_T1881.f0;_T187F=_T1881.f1;_T187E=_T1881.f2;_T187D=_T1881.f3;_T187C=_T1881.f4;_T187B=_T1881.f5;}{struct Cyc_List_List*lis=_T1880;int b=_T187F;struct Cyc_Absyn_VarargInfo*c=_T187E;void*eff=_T187D;struct Cyc_List_List*ec=_T187C;struct Cyc_List_List*qb=_T187B;_T82D=yyyvsp;_T82E=& _T82D[4].v;_T82F=(union Cyc_YYSTYPE*)_T82E;{
struct _tuple21 _T1881=Cyc_yyget_YY62(_T82F);struct Cyc_Absyn_Exp*_T1882;struct Cyc_Absyn_Exp*_T1883;struct Cyc_Absyn_Exp*_T1884;struct Cyc_Absyn_Exp*_T1885;_T1885=_T1881.f0;_T1884=_T1881.f1;_T1883=_T1881.f2;_T1882=_T1881.f3;{struct Cyc_Absyn_Exp*chk=_T1885;struct Cyc_Absyn_Exp*req=_T1884;struct Cyc_Absyn_Exp*ens=_T1883;struct Cyc_Absyn_Exp*thrws=_T1882;_T830=yyyvsp;_T831=& _T830[0].v;_T832=(union Cyc_YYSTYPE*)_T831;{
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T832);{struct Cyc_Parse_Declarator _T1886;_T834=one;
_T1886.id=_T834.id;_T835=one;_T1886.varloc=_T835.varloc;_T837=yyr;{struct Cyc_List_List*_T1887=_region_malloc(_T837,0U,sizeof(struct Cyc_List_List));_T839=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1888=_region_malloc(_T839,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1888->tag=3;_T83B=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1889=_region_malloc(_T83B,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1889->tag=1;_T1889->f1=lis;_T1889->f2=b;_T1889->f3=c;_T1889->f4=eff;_T1889->f5=ec;_T1889->f6=qb;_T1889->f7=chk;_T1889->f8=req;_T1889->f9=ens;_T1889->f10=thrws;_T83A=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1889;}_T1888->f1=(void*)_T83A;_T838=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1888;}_T1887->hd=(void*)_T838;_T83C=one;_T1887->tl=_T83C.tms;_T836=(struct Cyc_List_List*)_T1887;}_T1886.tms=_T836;_T833=_T1886;}yyval=Cyc_YY28(_T833);goto _LL0;}}}}}case 201: _T83D=yyyvsp;_T83E=& _T83D[0].v;_T83F=(union Cyc_YYSTYPE*)_T83E;{
# 2038 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T83F);{struct Cyc_Parse_Declarator _T187A;_T841=one;
_T187A.id=_T841.id;_T842=one;_T187A.varloc=_T842.varloc;_T844=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T844,0U,sizeof(struct Cyc_List_List));_T846=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T846,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T187C->tag=3;_T848=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T187D=_region_malloc(_T848,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T187D->tag=0;_T849=yyyvsp;_T84A=& _T849[2].v;_T84B=(union Cyc_YYSTYPE*)_T84A;_T187D->f1=Cyc_yyget_YY37(_T84B);_T84C=yyyvsp;_T84D=_T84C[0];_T84E=_T84D.l;_T84F=_T84E.first_line;_T187D->f2=Cyc_Position_loc_to_seg(_T84F);_T847=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T187D;}_T187C->f1=(void*)_T847;_T845=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T845;_T850=one;_T187B->tl=_T850.tms;_T843=(struct Cyc_List_List*)_T187B;}_T187A.tms=_T843;_T840=_T187A;}yyval=Cyc_YY28(_T840);goto _LL0;}case 202: _T852=Cyc_List_map_c;{
# 2042
struct Cyc_List_List*(*_T187A)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T852;_T851=_T187A;}_T853=yyyvsp;_T854=_T853[1];_T855=_T854.l;_T856=_T855.first_line;_T857=Cyc_Position_loc_to_seg(_T856);_T858=yyyvsp;_T859=& _T858[2].v;_T85A=(union Cyc_YYSTYPE*)_T859;_T85B=Cyc_yyget_YY41(_T85A);_T85C=Cyc_List_imp_rev(_T85B);{struct Cyc_List_List*ts=_T851(Cyc_Parse_typ2tvar,_T857,_T85C);_T85D=yyyvsp;_T85E=& _T85D[0].v;_T85F=(union Cyc_YYSTYPE*)_T85E;{
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T85F);{struct Cyc_Parse_Declarator _T187A;_T861=one;
_T187A.id=_T861.id;_T862=one;_T187A.varloc=_T862.varloc;_T864=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T864,0U,sizeof(struct Cyc_List_List));_T866=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T866,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T187C->tag=4;_T187C->f1=ts;_T867=yyyvsp;_T868=_T867[0];_T869=_T868.l;_T86A=_T869.first_line;_T187C->f2=Cyc_Position_loc_to_seg(_T86A);_T187C->f3=0;_T865=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T865;_T86B=one;_T187B->tl=_T86B.tms;_T863=(struct Cyc_List_List*)_T187B;}_T187A.tms=_T863;_T860=_T187A;}yyval=Cyc_YY28(_T860);goto _LL0;}}case 203: _T86C=yyyvsp;_T86D=& _T86C[0].v;_T86E=(union Cyc_YYSTYPE*)_T86D;{
# 2046 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T86E);{struct Cyc_Parse_Declarator _T187A;_T870=one;
_T187A.id=_T870.id;_T871=one;_T187A.varloc=_T871.varloc;_T873=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T873,0U,sizeof(struct Cyc_List_List));_T875=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_T875,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T187C->tag=5;_T876=yyyvsp;_T877=_T876[1];_T878=_T877.l;_T879=_T878.first_line;_T187C->f1=Cyc_Position_loc_to_seg(_T879);_T87A=yyyvsp;_T87B=& _T87A[1].v;_T87C=(union Cyc_YYSTYPE*)_T87B;_T187C->f2=Cyc_yyget_YY46(_T87C);_T874=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_T874;_T87D=one;_T187B->tl=_T87D.tms;_T872=(struct Cyc_List_List*)_T187B;}_T187A.tms=_T872;_T86F=_T187A;}yyval=Cyc_YY28(_T86F);goto _LL0;}case 204: _T87E=yyyvsp;_T87F=_T87E[0];
# 2052 "parse.y"
yyval=_T87F.v;goto _LL0;case 205: _T880=yyyvsp;_T881=& _T880[0].v;_T882=(union Cyc_YYSTYPE*)_T881;_T883=
# 2053 "parse.y"
Cyc_yyget_YY27(_T882);_T884=yyyvsp;_T885=& _T884[1].v;_T886=(union Cyc_YYSTYPE*)_T885;_T887=Cyc_yyget_YY27(_T886);_T888=Cyc_List_imp_append(_T883,_T887);yyval=Cyc_YY27(_T888);goto _LL0;case 206:  {
# 2057 "parse.y"
struct Cyc_List_List*ans=0;_T889=yyyvsp;_T88A=& _T889[3].v;_T88B=(union Cyc_YYSTYPE*)_T88A;_T88C=
Cyc_yyget_YY46(_T88B);if(_T88C==0)goto _TL2AD;_T88E=yyr;{struct Cyc_List_List*_T187A=_region_malloc(_T88E,0U,sizeof(struct Cyc_List_List));_T890=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T187B=_region_malloc(_T890,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T187B->tag=5;_T891=yyyvsp;_T892=_T891[3];_T893=_T892.l;_T894=_T893.first_line;
_T187B->f1=Cyc_Position_loc_to_seg(_T894);_T895=yyyvsp;_T896=& _T895[3].v;_T897=(union Cyc_YYSTYPE*)_T896;_T187B->f2=Cyc_yyget_YY46(_T897);_T88F=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T187B;}_T187A->hd=(void*)_T88F;_T187A->tl=ans;_T88D=(struct Cyc_List_List*)_T187A;}ans=_T88D;goto _TL2AE;_TL2AD: _TL2AE: {
# 2061
struct Cyc_Absyn_PtrLoc*ptrloc=0;_T898=yyyvsp;_T899=& _T898[0].v;_T89A=(union Cyc_YYSTYPE*)_T899;_T89B=
Cyc_yyget_YY1(_T89A);{struct _tuple22 _T187A=*_T89B;void*_T187B;void*_T187C;unsigned _T187D;_T187D=_T187A.f0;_T187C=_T187A.f1;_T187B=_T187A.f2;{unsigned ploc=_T187D;void*nullable=_T187C;void*bound=_T187B;_T89C=Cyc_Flags_porting_c_code;
if(!_T89C)goto _TL2AF;{struct Cyc_Absyn_PtrLoc*_T187E=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));
_T187E->ptr_loc=ploc;_T89E=yyyvsp;_T89F=_T89E[2];_T8A0=_T89F.l;_T8A1=_T8A0.first_line;_T187E->rgn_loc=Cyc_Position_loc_to_seg(_T8A1);_T8A2=yyyvsp;_T8A3=_T8A2[1];_T8A4=_T8A3.l;_T8A5=_T8A4.first_line;_T187E->zt_loc=Cyc_Position_loc_to_seg(_T8A5);_T89D=(struct Cyc_Absyn_PtrLoc*)_T187E;}ptrloc=_T89D;goto _TL2B0;_TL2AF: _TL2B0: _T8A6=yyr;_T8A7=ptrloc;_T8A8=nullable;_T8A9=bound;_T8AA=yyyvsp;_T8AB=& _T8AA[2].v;_T8AC=(union Cyc_YYSTYPE*)_T8AB;_T8AD=
Cyc_yyget_YY45(_T8AC);_T8AE=yyyvsp;_T8AF=& _T8AE[1].v;_T8B0=(union Cyc_YYSTYPE*)_T8AF;_T8B1=Cyc_yyget_YY60(_T8B0);_T8B2=yyyvsp;_T8B3=& _T8B2[4].v;_T8B4=(union Cyc_YYSTYPE*)_T8B3;_T8B5=Cyc_yyget_YY24(_T8B4);{void*mod=Cyc_Parse_make_pointer_mod(_T8A6,_T8A7,_T8A8,_T8A9,_T8AD,_T8B1,_T8B5);_T8B7=yyr;{struct Cyc_List_List*_T187E=_region_malloc(_T8B7,0U,sizeof(struct Cyc_List_List));
_T187E->hd=mod;_T187E->tl=ans;_T8B6=(struct Cyc_List_List*)_T187E;}ans=_T8B6;
yyval=Cyc_YY27(ans);goto _LL0;}}}}}case 207:
# 2071
 yyval=Cyc_YY60(0);goto _LL0;case 208: _T8B9=yyr;{struct Cyc_List_List*_T187A=_region_malloc(_T8B9,0U,sizeof(struct Cyc_List_List));_T8BA=yyyvsp;_T8BB=& _T8BA[0].v;_T8BC=(union Cyc_YYSTYPE*)_T8BB;
# 2072 "parse.y"
_T187A->hd=Cyc_yyget_YY59(_T8BC);_T8BD=yyyvsp;_T8BE=& _T8BD[1].v;_T8BF=(union Cyc_YYSTYPE*)_T8BE;_T187A->tl=Cyc_yyget_YY60(_T8BF);_T8B8=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY60(_T8B8);goto _LL0;case 209: _T8C1=yyr;{struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8C1,0U,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_T187A->tag=0;_T8C2=yyyvsp;_T8C3=& _T8C2[2].v;_T8C4=(union Cyc_YYSTYPE*)_T8C3;
# 2077 "parse.y"
_T187A->f1=Cyc_yyget_Exp_tok(_T8C4);_T8C0=(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8C5=(void*)_T8C0;yyval=Cyc_YY59(_T8C5);goto _LL0;case 210: _T8C6=yyyvsp;_T8C7=_T8C6[0];_T8C8=_T8C7.l;_T8C9=_T8C8.first_line;_T8CA=
# 2079 "parse.y"
Cyc_Position_loc_to_seg(_T8C9);_T8CB=_tag_fat("@region qualifiers are deprecated; use @effect instead",sizeof(char),55U);_T8CC=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T8CA,_T8CB,_T8CC);_T8CE=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8CE,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T187A->tag=2;{struct Cyc_List_List*_T187B=_cycalloc(sizeof(struct Cyc_List_List));_T8D0=yyyvsp;_T8D1=& _T8D0[2].v;_T8D2=(union Cyc_YYSTYPE*)_T8D1;
_T187B->hd=Cyc_yyget_YY45(_T8D2);_T187B->tl=0;_T8CF=(struct Cyc_List_List*)_T187B;}_T187A->f1=_T8CF;_T8CD=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8D3=(void*)_T8CD;yyval=Cyc_YY59(_T8D3);goto _LL0;case 211: _T8D5=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8D5,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T187A->tag=2;_T8D6=yyyvsp;_T8D7=& _T8D6[2].v;_T8D8=(union Cyc_YYSTYPE*)_T8D7;
# 2081 "parse.y"
_T187A->f1=Cyc_yyget_YY41(_T8D8);_T8D4=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8D9=(void*)_T8D4;yyval=Cyc_YY59(_T8D9);goto _LL0;case 212: _T8DB=yyr;{struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8DB,0U,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));
# 2082 "parse.y"
_T187A->tag=3;_T8DA=(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8DC=(void*)_T8DA;yyval=Cyc_YY59(_T8DC);goto _LL0;case 213: _T8DE=yyr;{struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8DE,0U,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));
# 2083 "parse.y"
_T187A->tag=4;_T8DD=(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8DF=(void*)_T8DD;yyval=Cyc_YY59(_T8DF);goto _LL0;case 214: _T8E1=yyr;{struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8E1,0U,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));
# 2084 "parse.y"
_T187A->tag=7;_T8E0=(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8E2=(void*)_T8E0;yyval=Cyc_YY59(_T8E2);goto _LL0;case 215: _T8E4=yyr;{struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8E4,0U,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2085 "parse.y"
_T187A->tag=5;_T8E3=(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8E5=(void*)_T8E3;yyval=Cyc_YY59(_T8E5);goto _LL0;case 216: _T8E7=yyr;{struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8E7,0U,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2086 "parse.y"
_T187A->tag=6;_T8E6=(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8E8=(void*)_T8E6;yyval=Cyc_YY59(_T8E8);goto _LL0;case 217: _T8EA=yyr;{struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8EA,0U,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));
# 2087 "parse.y"
_T187A->tag=8;_T8E9=(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8EB=(void*)_T8E9;yyval=Cyc_YY59(_T8EB);goto _LL0;case 218: _T8ED=yyr;{struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8ED,0U,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));
# 2088 "parse.y"
_T187A->tag=9;_T8EC=(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8EE=(void*)_T8EC;yyval=Cyc_YY59(_T8EE);goto _LL0;case 219: _T8F0=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8F0,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T187A->tag=10;_T8F1=yyyvsp;_T8F2=& _T8F1[2].v;_T8F3=(union Cyc_YYSTYPE*)_T8F2;
# 2089 "parse.y"
_T187A->f1=Cyc_yyget_YY58(_T8F3);_T8EF=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T8F4=(void*)_T8EF;yyval=Cyc_YY59(_T8F4);goto _LL0;case 220: _T8F5=yyyvsp;_T8F6=_T8F5[0];_T8F7=_T8F6.l;_T8F8=_T8F7.first_line;_T8F9=
# 2090 "parse.y"
Cyc_Position_loc_to_seg(_T8F8);_T8FA=yyyvsp;_T8FB=& _T8FA[0].v;_T8FC=(union Cyc_YYSTYPE*)_T8FB;_T8FD=Cyc_yyget_String_tok(_T8FC);{void*aq=Cyc_Parse_id2aqual(_T8F9,_T8FD);_T8FF=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T187A=_region_malloc(_T8FF,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T187A->tag=10;_T187A->f1=aq;_T8FE=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T187A;}_T900=(void*)_T8FE;yyval=Cyc_YY59(_T900);goto _LL0;}case 221: _T901=yyyvsp;_T902=& _T901[0].v;_T903=(union Cyc_YYSTYPE*)_T902;_T904=
# 2094 "parse.y"
Cyc_yyget_YY58(_T903);yyval=Cyc_YY58(_T904);goto _LL0;case 222: _T905=yyyvsp;_T906=& _T905[0].v;_T907=(union Cyc_YYSTYPE*)_T906;_T908=
# 2100 "parse.y"
Cyc_yyget_YY45(_T907);_T909=Cyc_Absyn_al_qual_type;_T90A=Cyc_Absyn_aqual_var_type(_T908,_T909);yyval=Cyc_YY58(_T90A);goto _LL0;case 223: _T90B=yyyvsp;_T90C=& _T90B[2].v;_T90D=(union Cyc_YYSTYPE*)_T90C;_T90E=
# 2104 "parse.y"
Cyc_yyget_YY45(_T90D);_T90F=Cyc_Absyn_aqualsof_type(_T90E);_T910=Cyc_Absyn_al_qual_type;_T911=Cyc_Absyn_aqual_var_type(_T90F,_T910);yyval=Cyc_YY58(_T911);goto _LL0;case 224: goto _LL0;case 225: _T912=yyyvsp;_T913=_T912[0];_T914=_T913.l;_T915=_T914.first_line;_T916=
# 2110 "parse.y"
Cyc_Position_loc_to_seg(_T915);_T917=yyyvsp;_T918=& _T917[0].v;_T919=(union Cyc_YYSTYPE*)_T918;_T91A=Cyc_yyget_String_tok(_T919);_T91B=Cyc_Parse_id2aqual(_T916,_T91A);yyval=Cyc_YY58(_T91B);goto _LL0;case 226:{struct _tuple22*_T187A=_cycalloc(sizeof(struct _tuple22));_T91D=yyyvsp;_T91E=_T91D[0];_T91F=_T91E.l;_T920=_T91F.first_line;
# 2116 "parse.y"
_T187A->f0=Cyc_Position_loc_to_seg(_T920);_T187A->f1=Cyc_Absyn_true_type;_T921=Cyc_Parse_parsing_tempest;if(!_T921)goto _TL2B1;_T187A->f2=Cyc_Absyn_fat_bound_type;goto _TL2B2;_TL2B1: _T922=yyyvsp;_T923=_T922[0];_T924=_T923.l;_T925=_T924.first_line;_T926=Cyc_Position_loc_to_seg(_T925);_T927=Cyc_Position_string_of_segment(_T926);_T928=yyyvsp;_T929=& _T928[1].v;_T92A=(union Cyc_YYSTYPE*)_T929;_T92B=Cyc_yyget_YY2(_T92A);_T187A->f2=Cyc_Parse_assign_cvar_pos(_T927,0,_T92B);_TL2B2: _T91C=(struct _tuple22*)_T187A;}yyval=Cyc_YY1(_T91C);goto _LL0;case 227:{struct _tuple22*_T187A=_cycalloc(sizeof(struct _tuple22));_T92D=yyyvsp;_T92E=_T92D[0];_T92F=_T92E.l;_T930=_T92F.first_line;
# 2117 "parse.y"
_T187A->f0=Cyc_Position_loc_to_seg(_T930);_T187A->f1=Cyc_Absyn_false_type;_T931=yyyvsp;_T932=_T931[0];_T933=_T932.l;_T934=_T933.first_line;_T935=Cyc_Position_loc_to_seg(_T934);_T936=Cyc_Position_string_of_segment(_T935);_T937=yyyvsp;_T938=& _T937[1].v;_T939=(union Cyc_YYSTYPE*)_T938;_T93A=Cyc_yyget_YY2(_T939);_T187A->f2=Cyc_Parse_assign_cvar_pos(_T936,0,_T93A);_T92C=(struct _tuple22*)_T187A;}yyval=Cyc_YY1(_T92C);goto _LL0;case 228: _T93C=Cyc_Flags_override_fat;
# 2118 "parse.y"
if(!_T93C)goto _TL2B3;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2B3;_T93D=Cyc_Flags_interproc;if(!_T93D)goto _TL2B3;{struct _tuple22*_T187A=_cycalloc(sizeof(struct _tuple22));_T93F=yyyvsp;_T940=_T93F[0];_T941=_T940.l;_T942=_T941.first_line;
_T187A->f0=Cyc_Position_loc_to_seg(_T942);_T187A->f1=Cyc_Absyn_true_type;_T943=yyyvsp;_T944=_T943[0];_T945=_T944.l;_T946=_T945.first_line;_T947=Cyc_Position_loc_to_seg(_T946);_T948=Cyc_Position_string_of_segment(_T947);_T949=& Cyc_Kinds_ptrbko;_T94A=(struct Cyc_Core_Opt*)_T949;_T94B=Cyc_Absyn_cvar_type(_T94A);_T187A->f2=Cyc_Parse_assign_cvar_pos(_T948,1,_T94B);_T93E=(struct _tuple22*)_T187A;}_T93B=_T93E;goto _TL2B4;_TL2B3:{struct _tuple22*_T187A=_cycalloc(sizeof(struct _tuple22));_T94D=yyyvsp;_T94E=_T94D[0];_T94F=_T94E.l;_T950=_T94F.first_line;
_T187A->f0=Cyc_Position_loc_to_seg(_T950);_T187A->f1=Cyc_Absyn_true_type;_T187A->f2=Cyc_Absyn_fat_bound_type;_T94C=(struct _tuple22*)_T187A;}_T93B=_T94C;_TL2B4:
# 2118
 yyval=Cyc_YY1(_T93B);goto _LL0;case 229: _T952=Cyc_Flags_interproc;
# 2123
if(!_T952)goto _TL2B5;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2B5;_T953=& Cyc_Kinds_ptrbko;_T954=(struct Cyc_Core_Opt*)_T953;_T951=Cyc_Absyn_cvar_type(_T954);goto _TL2B6;_TL2B5: _T951=Cyc_Absyn_bounds_one();_TL2B6: yyval=Cyc_YY2(_T951);goto _LL0;case 230: _T955=yyyvsp;_T956=& _T955[1].v;_T957=(union Cyc_YYSTYPE*)_T956;_T958=
# 2124 "parse.y"
Cyc_yyget_Exp_tok(_T957);_T959=Cyc_Absyn_thin_bounds_exp(_T958);yyval=Cyc_YY2(_T959);goto _LL0;case 231: _T95A=Cyc_Flags_resolve;
# 2127 "parse.y"
if(_T95A)goto _TL2B7;else{goto _TL2B9;}
_TL2B9:{int(*_T187A)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T95B=_T187A;}_T95C=_tag_fat("Type variable not permitted in pointer bound",sizeof(char),45U);_T95B(0U,_T95C);goto _TL2B8;_TL2B7: _TL2B8: _T95D=yyyvsp;_T95E=& _T95D[1].v;_T95F=(union Cyc_YYSTYPE*)_T95E;_T960=
Cyc_yyget_String_tok(_T95F);_T961=Cyc_Parse_typevar2cvar(_T960);yyval=Cyc_YY2(_T961);goto _LL0;case 232: _T962=
# 2134 "parse.y"
Cyc_Tcutil_any_bool(0);yyval=Cyc_YY54(_T962);goto _LL0;case 233:
# 2135 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_true_type);goto _LL0;case 234:
# 2136 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_false_type);goto _LL0;case 235:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T964=yyyvsp;_T965=& _T964[0].v;_T966=(union Cyc_YYSTYPE*)_T965;
# 2140 "parse.y"
_T187A->hd=Cyc_yyget_YY45(_T966);_T187A->tl=0;_T963=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY41(_T963);goto _LL0;case 236:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T968=yyyvsp;_T969=& _T968[0].v;_T96A=(union Cyc_YYSTYPE*)_T969;
# 2141 "parse.y"
_T187A->hd=Cyc_yyget_YY45(_T96A);_T96B=yyyvsp;_T96C=& _T96B[2].v;_T96D=(union Cyc_YYSTYPE*)_T96C;_T187A->tl=Cyc_yyget_YY41(_T96D);_T967=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY41(_T967);goto _LL0;case 237: _T96E=& Cyc_Kinds_eko;_T96F=(struct Cyc_Core_Opt*)_T96E;_T970=
# 2145 "parse.y"
Cyc_Absyn_new_evar(_T96F,0);yyval=Cyc_YY45(_T970);goto _LL0;case 238: _T971=yyyvsp;_T972=& _T971[0].v;_T973=(union Cyc_YYSTYPE*)_T972;{
# 2146 "parse.y"
struct Cyc_List_List*es=Cyc_yyget_YY41(_T973);_T974=Cyc_List_length(es);if(_T974!=1)goto _TL2BA;_T975=_check_null(es);_T976=_T975->hd;yyval=Cyc_YY45(_T976);goto _TL2BB;_TL2BA: _T977=yyyvsp;_T978=& _T977[0].v;_T979=(union Cyc_YYSTYPE*)_T978;_T97A=Cyc_yyget_YY41(_T979);_T97B=Cyc_Absyn_join_eff(_T97A);yyval=Cyc_YY45(_T97B);_TL2BB: goto _LL0;}case 239: _T97C=& Cyc_Kinds_eko;_T97D=(struct Cyc_Core_Opt*)_T97C;_T97E=
# 2147 "parse.y"
Cyc_Absyn_new_evar(_T97D,0);yyval=Cyc_YY45(_T97E);goto _LL0;case 240: _T97F=yyvs;_T980=yyvsp_offset + 1;_T981=_check_fat_subscript(_T97F,sizeof(struct Cyc_Yystacktype),_T980);_T982=(struct Cyc_Yystacktype*)_T981;_T983=*_T982;_T984=_T983.l;_T985=_T984.first_line;_T986=
# 2158 "parse.y"
Cyc_Position_loc_to_seg(_T985);_T987=Cyc_Absyn_empty_tqual(_T986);yyval=Cyc_YY24(_T987);goto _LL0;case 241: _T988=yyyvsp;_T989=& _T988[0].v;_T98A=(union Cyc_YYSTYPE*)_T989;_T98B=
# 2159 "parse.y"
Cyc_yyget_YY24(_T98A);_T98C=yyyvsp;_T98D=& _T98C[1].v;_T98E=(union Cyc_YYSTYPE*)_T98D;_T98F=Cyc_yyget_YY24(_T98E);_T990=Cyc_Absyn_combine_tqual(_T98B,_T98F);yyval=Cyc_YY24(_T990);goto _LL0;case 242: _T991=yyyvsp;_T992=& _T991[1].v;_T993=(union Cyc_YYSTYPE*)_T992;{
# 2164 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T993);_T995=ec_qb;_T996=(unsigned)_T995;
if(!_T996)goto _TL2BC;_T997=ec_qb;_T994=*_T997;goto _TL2BD;_TL2BC:{struct _tuple28 _T187A;_T187A.f0=0;_T187A.f1=0;_T998=_T187A;}_T994=_T998;_TL2BD: {struct _tuple28 _T187A=_T994;struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_List_List*ec=_T187C;struct Cyc_List_List*qb=_T187B;{struct _tuple27*_T187D=_cycalloc(sizeof(struct _tuple27));
_T187D->f0=0;_T187D->f1=0;_T187D->f2=0;_T99A=yyyvsp;_T99B=& _T99A[0].v;_T99C=(union Cyc_YYSTYPE*)_T99B;_T187D->f3=Cyc_yyget_YY50(_T99C);_T187D->f4=ec;_T187D->f5=qb;_T999=(struct _tuple27*)_T187D;}yyval=Cyc_YY40(_T999);goto _LL0;}}}case 243: _T99D=yyyvsp;_T99E=& _T99D[2].v;_T99F=(union Cyc_YYSTYPE*)_T99E;{
# 2168 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T99F);_T9A1=ec_qb;_T9A2=(unsigned)_T9A1;
if(!_T9A2)goto _TL2BE;_T9A3=ec_qb;_T9A0=*_T9A3;goto _TL2BF;_TL2BE:{struct _tuple28 _T187A;_T187A.f0=0;_T187A.f1=0;_T9A4=_T187A;}_T9A0=_T9A4;_TL2BF: {struct _tuple28 _T187A=_T9A0;struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_List_List*ec=_T187C;struct Cyc_List_List*qb=_T187B;{struct _tuple27*_T187D=_cycalloc(sizeof(struct _tuple27));_T9A6=yyyvsp;_T9A7=& _T9A6[0].v;_T9A8=(union Cyc_YYSTYPE*)_T9A7;_T9A9=
Cyc_yyget_YY39(_T9A8);_T187D->f0=Cyc_List_imp_rev(_T9A9);_T187D->f1=0;_T187D->f2=0;_T9AA=yyyvsp;_T9AB=& _T9AA[1].v;_T9AC=(union Cyc_YYSTYPE*)_T9AB;_T187D->f3=Cyc_yyget_YY50(_T9AC);_T187D->f4=ec;_T187D->f5=qb;_T9A5=(struct _tuple27*)_T187D;}yyval=Cyc_YY40(_T9A5);goto _LL0;}}}case 244: _T9AD=yyyvsp;_T9AE=& _T9AD[4].v;_T9AF=(union Cyc_YYSTYPE*)_T9AE;{
# 2172 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9AF);_T9B1=ec_qb;_T9B2=(unsigned)_T9B1;
if(!_T9B2)goto _TL2C0;_T9B3=ec_qb;_T9B0=*_T9B3;goto _TL2C1;_TL2C0:{struct _tuple28 _T187A;_T187A.f0=0;_T187A.f1=0;_T9B4=_T187A;}_T9B0=_T9B4;_TL2C1: {struct _tuple28 _T187A=_T9B0;struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_List_List*ec=_T187C;struct Cyc_List_List*qb=_T187B;{struct _tuple27*_T187D=_cycalloc(sizeof(struct _tuple27));_T9B6=yyyvsp;_T9B7=& _T9B6[0].v;_T9B8=(union Cyc_YYSTYPE*)_T9B7;_T9B9=
Cyc_yyget_YY39(_T9B8);_T187D->f0=Cyc_List_imp_rev(_T9B9);_T187D->f1=1;_T187D->f2=0;_T9BA=yyyvsp;_T9BB=& _T9BA[3].v;_T9BC=(union Cyc_YYSTYPE*)_T9BB;_T187D->f3=Cyc_yyget_YY50(_T9BC);_T187D->f4=ec;_T187D->f5=qb;_T9B5=(struct _tuple27*)_T187D;}yyval=Cyc_YY40(_T9B5);goto _LL0;}}}case 245: _T9BD=yyyvsp;_T9BE=& _T9BD[2].v;_T9BF=(union Cyc_YYSTYPE*)_T9BE;{
# 2177
struct _tuple8*_T187A=Cyc_yyget_YY38(_T9BF);void*_T187B;struct Cyc_Absyn_Tqual _T187C;struct _fat_ptr*_T187D;{struct _tuple8 _T187E=*_T187A;_T187D=_T187E.f0;_T187C=_T187E.f1;_T187B=_T187E.f2;}{struct _fat_ptr*n=_T187D;struct Cyc_Absyn_Tqual tq=_T187C;void*t=_T187B;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_T9C0=v;_T9C0->name=n;_T9C1=v;_T9C1->tq=tq;_T9C2=v;_T9C2->type=t;_T9C3=v;_T9C4=yyyvsp;_T9C5=& _T9C4[1].v;_T9C6=(union Cyc_YYSTYPE*)_T9C5;_T9C3->inject=Cyc_yyget_YY32(_T9C6);_T9C7=yyyvsp;_T9C8=& _T9C7[4].v;_T9C9=(union Cyc_YYSTYPE*)_T9C8;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9C9);_T9CB=ec_qb;_T9CC=(unsigned)_T9CB;
if(!_T9CC)goto _TL2C2;_T9CD=ec_qb;_T9CA=*_T9CD;goto _TL2C3;_TL2C2:{struct _tuple28 _T187E;_T187E.f0=0;_T187E.f1=0;_T9CE=_T187E;}_T9CA=_T9CE;_TL2C3: {struct _tuple28 _T187E=_T9CA;struct Cyc_List_List*_T187F;struct Cyc_List_List*_T1880;_T1880=_T187E.f0;_T187F=_T187E.f1;{struct Cyc_List_List*ec=_T1880;struct Cyc_List_List*qb=_T187F;{struct _tuple27*_T1881=_cycalloc(sizeof(struct _tuple27));
_T1881->f0=0;_T1881->f1=0;_T1881->f2=v;_T9D0=yyyvsp;_T9D1=& _T9D0[3].v;_T9D2=(union Cyc_YYSTYPE*)_T9D1;_T1881->f3=Cyc_yyget_YY50(_T9D2);_T1881->f4=ec;_T1881->f5=qb;_T9CF=(struct _tuple27*)_T1881;}yyval=Cyc_YY40(_T9CF);goto _LL0;}}}}}case 246: _T9D3=yyyvsp;_T9D4=& _T9D3[4].v;_T9D5=(union Cyc_YYSTYPE*)_T9D4;{
# 2185
struct _tuple8*_T187A=Cyc_yyget_YY38(_T9D5);void*_T187B;struct Cyc_Absyn_Tqual _T187C;struct _fat_ptr*_T187D;{struct _tuple8 _T187E=*_T187A;_T187D=_T187E.f0;_T187C=_T187E.f1;_T187B=_T187E.f2;}{struct _fat_ptr*n=_T187D;struct Cyc_Absyn_Tqual tq=_T187C;void*t=_T187B;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_T9D6=v;_T9D6->name=n;_T9D7=v;_T9D7->tq=tq;_T9D8=v;_T9D8->type=t;_T9D9=v;_T9DA=yyyvsp;_T9DB=& _T9DA[3].v;_T9DC=(union Cyc_YYSTYPE*)_T9DB;_T9D9->inject=Cyc_yyget_YY32(_T9DC);_T9DD=yyyvsp;_T9DE=& _T9DD[6].v;_T9DF=(union Cyc_YYSTYPE*)_T9DE;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9DF);_T9E1=ec_qb;_T9E2=(unsigned)_T9E1;
if(!_T9E2)goto _TL2C4;_T9E3=ec_qb;_T9E0=*_T9E3;goto _TL2C5;_TL2C4:{struct _tuple28 _T187E;_T187E.f0=0;_T187E.f1=0;_T9E4=_T187E;}_T9E0=_T9E4;_TL2C5: {struct _tuple28 _T187E=_T9E0;struct Cyc_List_List*_T187F;struct Cyc_List_List*_T1880;_T1880=_T187E.f0;_T187F=_T187E.f1;{struct Cyc_List_List*ec=_T1880;struct Cyc_List_List*qb=_T187F;{struct _tuple27*_T1881=_cycalloc(sizeof(struct _tuple27));_T9E6=yyyvsp;_T9E7=& _T9E6[0].v;_T9E8=(union Cyc_YYSTYPE*)_T9E7;_T9E9=
Cyc_yyget_YY39(_T9E8);_T1881->f0=Cyc_List_imp_rev(_T9E9);_T1881->f1=0;_T1881->f2=v;_T9EA=yyyvsp;_T9EB=& _T9EA[5].v;_T9EC=(union Cyc_YYSTYPE*)_T9EB;_T1881->f3=Cyc_yyget_YY50(_T9EC);_T1881->f4=ec;_T1881->f5=qb;_T9E5=(struct _tuple27*)_T1881;}yyval=Cyc_YY40(_T9E5);goto _LL0;}}}}}case 247:
# 2195 "parse.y"
 yyval=Cyc_YY50(0);goto _LL0;case 248: _T9ED=yyyvsp;_T9EE=_T9ED[0];_T9EF=_T9EE.l;_T9F0=_T9EF.first_line;_T9F1=
# 2196 "parse.y"
Cyc_Position_loc_to_seg(_T9F0);_T9F2=yyyvsp;_T9F3=& _T9F2[0].v;_T9F4=(union Cyc_YYSTYPE*)_T9F3;_T9F5=Cyc_yyget_String_tok(_T9F4);_T9F6=Cyc_Parse_id2aqual(_T9F1,_T9F5);yyval=Cyc_YY50(_T9F6);goto _LL0;case 249: _T9F7=yyyvsp;_T9F8=& _T9F7[0].v;_T9F9=(union Cyc_YYSTYPE*)_T9F8;_T9FA=
# 2199
Cyc_yyget_String_tok(_T9F9);{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_T187A->tag=1;_T187A->f1=0;_T9FB=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T187A;}_T9FC=(void*)_T9FB;_T9FD=yyyvsp;_T9FE=& _T9FD[1].v;_T9FF=(union Cyc_YYSTYPE*)_T9FE;_TA00=Cyc_yyget_YY50(_T9FF);_TA01=yyyvsp;_TA02=_TA01[0];_TA03=_TA02.l;_TA04=_TA03.first_line;_TA05=Cyc_Position_loc_to_seg(_TA04);_TA06=Cyc_Parse_id2type(_T9FA,_T9FC,_TA00,_TA05);yyval=Cyc_YY45(_TA06);goto _LL0;case 250: _TA07=yyyvsp;_TA08=& _TA07[0].v;_TA09=(union Cyc_YYSTYPE*)_TA08;_TA0A=
# 2200 "parse.y"
Cyc_yyget_String_tok(_TA09);_TA0B=yyyvsp;_TA0C=& _TA0B[2].v;_TA0D=(union Cyc_YYSTYPE*)_TA0C;_TA0E=Cyc_yyget_YY44(_TA0D);_TA0F=Cyc_Kinds_kind_to_bound(_TA0E);_TA10=yyyvsp;_TA11=& _TA10[3].v;_TA12=(union Cyc_YYSTYPE*)_TA11;_TA13=Cyc_yyget_YY50(_TA12);_TA14=yyyvsp;_TA15=_TA14[0];_TA16=_TA15.l;_TA17=_TA16.first_line;_TA18=Cyc_Position_loc_to_seg(_TA17);_TA19=Cyc_Parse_id2type(_TA0A,_TA0F,_TA13,_TA18);yyval=Cyc_YY45(_TA19);goto _LL0;case 251:
# 2203
 yyval=Cyc_YY50(0);goto _LL0;case 252: _TA1A=yyyvsp;_TA1B=& _TA1A[1].v;_TA1C=(union Cyc_YYSTYPE*)_TA1B;_TA1D=
# 2204 "parse.y"
Cyc_yyget_YY41(_TA1C);_TA1E=Cyc_Absyn_join_eff(_TA1D);yyval=Cyc_YY50(_TA1E);goto _LL0;case 253:
# 2208 "parse.y"
 yyval=Cyc_YY51(0);goto _LL0;case 254: _TA1F=yyyvsp;_TA20=_TA1F[1];
# 2209 "parse.y"
yyval=_TA20.v;goto _LL0;case 255:{struct _tuple28*_T187A=_cycalloc(sizeof(struct _tuple28));{void*_T187B[1];_TA23=yyyvsp;_TA24=& _TA23[0].v;_TA25=(union Cyc_YYSTYPE*)_TA24;_T187B[0]=
# 2214 "parse.y"
Cyc_yyget_YY52(_TA25);_TA26=_tag_fat(_T187B,sizeof(void*),1);_TA22=Cyc_List_list(_TA26);}_T187A->f0=_TA22;_T187A->f1=0;_TA21=(struct _tuple28*)_T187A;}yyval=Cyc_YY51(_TA21);goto _LL0;case 256:{struct _tuple28*_T187A=_cycalloc(sizeof(struct _tuple28));
# 2216 "parse.y"
_T187A->f0=0;{struct _tuple29*_T187B[1];_TA29=yyyvsp;_TA2A=& _TA29[0].v;_TA2B=(union Cyc_YYSTYPE*)_TA2A;_T187B[0]=Cyc_yyget_YY53(_TA2B);_TA2C=_tag_fat(_T187B,sizeof(struct _tuple29*),1);_TA28=Cyc_List_list(_TA2C);}_T187A->f1=_TA28;_TA27=(struct _tuple28*)_T187A;}yyval=Cyc_YY51(_TA27);goto _LL0;case 257: _TA2D=yyyvsp;_TA2E=& _TA2D[2].v;_TA2F=(union Cyc_YYSTYPE*)_TA2E;{
# 2219 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(_TA2F);_TA30=
_check_null(rest);{struct _tuple28 _T187A=*_TA30;struct Cyc_List_List*_T187B;_T187B=_T187A.f0;{struct Cyc_List_List*rpo=_T187B;_TA31=rest;{struct Cyc_List_List*_T187C=_cycalloc(sizeof(struct Cyc_List_List));_TA33=yyyvsp;_TA34=& _TA33[0].v;_TA35=(union Cyc_YYSTYPE*)_TA34;
_T187C->hd=Cyc_yyget_YY52(_TA35);_T187C->tl=rpo;_TA32=(struct Cyc_List_List*)_T187C;}(*_TA31).f0=_TA32;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 258: _TA36=yyyvsp;_TA37=& _TA36[2].v;_TA38=(union Cyc_YYSTYPE*)_TA37;{
# 2226 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(_TA38);_TA39=
_check_null(rest);{struct _tuple28 _T187A=*_TA39;struct Cyc_List_List*_T187B;_T187B=_T187A.f1;{struct Cyc_List_List*qb=_T187B;_TA3A=rest;{struct Cyc_List_List*_T187C=_cycalloc(sizeof(struct Cyc_List_List));_TA3C=yyyvsp;_TA3D=& _TA3C[0].v;_TA3E=(union Cyc_YYSTYPE*)_TA3D;
_T187C->hd=Cyc_yyget_YY53(_TA3E);_T187C->tl=qb;_TA3B=(struct Cyc_List_List*)_T187C;}(*_TA3A).f1=_TA3B;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 259:  {
# 2236 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_TA3F=kb;_TA3F->tag=2;_TA40=kb;_TA40->f1=0;_TA41=kb;_TA42=& Cyc_Kinds_ek;_TA41->f2=(struct Cyc_Absyn_Kind*)_TA42;_TA43=yyyvsp;_TA44=& _TA43[2].v;_TA45=(union Cyc_YYSTYPE*)_TA44;_TA46=
Cyc_yyget_String_tok(_TA45);_TA47=kb;_TA48=(void*)_TA47;_TA49=yyyvsp;_TA4A=_TA49[2];_TA4B=_TA4A.l;_TA4C=_TA4B.first_line;_TA4D=Cyc_Position_loc_to_seg(_TA4C);{void*t=Cyc_Parse_id2type(_TA46,_TA48,0,_TA4D);{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct));_T187A->tag=2;_TA4F=yyyvsp;_TA50=& _TA4F[0].v;_TA51=(union Cyc_YYSTYPE*)_TA50;_TA52=
Cyc_yyget_YY41(_TA51);_T187A->f1=Cyc_Parse_effect_from_atomic(_TA52);_T187A->f2=t;_TA4E=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T187A;}_TA53=(void*)_TA4E;yyval=Cyc_YY52(_TA53);goto _LL0;}}case 260:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_T187A->tag=1;_TA55=yyyvsp;_TA56=& _TA55[0].v;_TA57=(union Cyc_YYSTYPE*)_TA56;_TA58=
# 2242 "parse.y"
Cyc_yyget_YY41(_TA57);_T187A->f1=Cyc_Parse_effect_from_atomic(_TA58);_TA59=yyyvsp;_TA5A=& _TA59[2].v;_TA5B=(union Cyc_YYSTYPE*)_TA5A;_TA5C=
Cyc_yyget_YY41(_TA5B);_T187A->f2=Cyc_Parse_effect_from_atomic(_TA5C);_TA54=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T187A;}_TA5D=(void*)_TA54;
# 2242
yyval=Cyc_YY52(_TA5D);goto _LL0;case 261: _TA5E=yyyvsp;_TA5F=_TA5E[0];_TA60=_TA5F.l;_TA61=_TA60.first_line;_TA62=
# 2247 "parse.y"
Cyc_Position_loc_to_seg(_TA61);_TA63=yyyvsp;_TA64=& _TA63[0].v;_TA65=(union Cyc_YYSTYPE*)_TA64;_TA66=Cyc_yyget_String_tok(_TA65);Cyc_Parse_check_single_constraint(_TA62,_TA66);{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct));_T187A->tag=0;_TA68=yyyvsp;_TA69=& _TA68[2].v;_TA6A=(union Cyc_YYSTYPE*)_TA69;_TA6B=
Cyc_yyget_YY41(_TA6A);_T187A->f1=Cyc_Parse_effect_from_atomic(_TA6B);_TA67=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T187A;}_TA6C=(void*)_TA67;yyval=Cyc_YY52(_TA6C);goto _LL0;case 262:{struct _tuple29*_T187A=_cycalloc(sizeof(struct _tuple29));_TA6E=yyyvsp;_TA6F=& _TA6E[2].v;_TA70=(union Cyc_YYSTYPE*)_TA6F;
# 2255 "parse.y"
_T187A->f0=Cyc_yyget_YY45(_TA70);_TA71=yyyvsp;_TA72=& _TA71[0].v;_TA73=(union Cyc_YYSTYPE*)_TA72;_T187A->f1=Cyc_yyget_YY45(_TA73);_TA6D=(struct _tuple29*)_T187A;}yyval=Cyc_YY53(_TA6D);goto _LL0;case 263:
# 2260 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_al_qual_type);goto _LL0;case 264:
# 2261 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_un_qual_type);goto _LL0;case 265:
# 2262 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rc_qual_type);goto _LL0;case 266:
# 2263 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rtd_qual_type);goto _LL0;case 267: _TA74=yyyvsp;_TA75=_TA74[0];_TA76=_TA75.l;_TA77=_TA76.first_line;_TA78=
# 2264 "parse.y"
Cyc_Position_loc_to_seg(_TA77);_TA79=yyyvsp;_TA7A=& _TA79[0].v;_TA7B=(union Cyc_YYSTYPE*)_TA7A;_TA7C=Cyc_yyget_String_tok(_TA7B);_TA7D=Cyc_Parse_id2aqual(_TA78,_TA7C);yyval=Cyc_YY58(_TA7D);goto _LL0;case 268: _TA7E=yyyvsp;_TA7F=& _TA7E[0].v;_TA80=(union Cyc_YYSTYPE*)_TA7F;_TA81=
# 2269 "parse.y"
Cyc_yyget_YY58(_TA80);yyval=Cyc_YY45(_TA81);goto _LL0;case 269: _TA82=yyyvsp;_TA83=& _TA82[2].v;_TA84=(union Cyc_YYSTYPE*)_TA83;_TA85=
# 2272 "parse.y"
Cyc_yyget_YY45(_TA84);_TA86=Cyc_Absyn_aqualsof_type(_TA85);_TA87=Cyc_Absyn_al_qual_type;_TA88=Cyc_Absyn_aqual_var_type(_TA86,_TA87);yyval=Cyc_YY45(_TA88);goto _LL0;case 270:  {
# 2279 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_TA89=kb;_TA89->tag=0;_TA8A=kb;_TA8B=& Cyc_Kinds_aqk;_TA8A->f1=(struct Cyc_Absyn_Kind*)_TA8B;_TA8C=yyyvsp;_TA8D=& _TA8C[0].v;_TA8E=(union Cyc_YYSTYPE*)_TA8D;_TA8F=
Cyc_yyget_String_tok(_TA8E);_TA90=kb;_TA91=(void*)_TA90;_TA92=yyyvsp;_TA93=_TA92[0];_TA94=_TA93.l;_TA95=_TA94.first_line;_TA96=Cyc_Position_loc_to_seg(_TA95);_TA97=Cyc_Parse_id2type(_TA8F,_TA91,0,_TA96);yyval=Cyc_YY45(_TA97);goto _LL0;}case 271: _TA98=yyyvsp;_TA99=& _TA98[2].v;_TA9A=(union Cyc_YYSTYPE*)_TA99;_TA9B=
# 2284 "parse.y"
Cyc_yyget_YY45(_TA9A);_TA9C=Cyc_Absyn_aqualsof_type(_TA9B);yyval=Cyc_YY45(_TA9C);goto _LL0;case 272:
# 2313 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 273: _TA9D=yyyvsp;_TA9E=& _TA9D[0].v;_TA9F=(union Cyc_YYSTYPE*)_TA9E;_TAA0=
# 2315 "parse.y"
Cyc_yyget_String_tok(_TA9F);_TAA1=_tag_fat("inject",sizeof(char),7U);_TAA2=Cyc_zstrcmp(_TAA0,_TAA1);if(_TAA2==0)goto _TL2C6;{struct Cyc_Warn_String_Warn_Warg_struct _T187A;_T187A.tag=0;
_T187A.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_TAA3=_T187A;}{struct Cyc_Warn_String_Warn_Warg_struct _T187A=_TAA3;void*_T187B[1];_T187B[0]=& _T187A;_TAA4=yyyvsp;_TAA5=_TAA4[0];_TAA6=_TAA5.l;_TAA7=_TAA6.first_line;_TAA8=Cyc_Position_loc_to_seg(_TAA7);_TAA9=_tag_fat(_T187B,sizeof(void*),1);Cyc_Warn_err2(_TAA8,_TAA9);}goto _TL2C7;_TL2C6: _TL2C7:
 yyval=Cyc_YY32(1);goto _LL0;case 274: _TAAA=yyyvsp;_TAAB=_TAAA[0];
# 2322 "parse.y"
yyval=_TAAB.v;goto _LL0;case 275: _TAAC=yyyvsp;_TAAD=& _TAAC[0].v;_TAAE=(union Cyc_YYSTYPE*)_TAAD;_TAAF=
# 2323 "parse.y"
Cyc_yyget_YY41(_TAAE);_TAB0=yyyvsp;_TAB1=& _TAB0[2].v;_TAB2=(union Cyc_YYSTYPE*)_TAB1;_TAB3=Cyc_yyget_YY41(_TAB2);_TAB4=Cyc_List_imp_append(_TAAF,_TAB3);yyval=Cyc_YY41(_TAB4);goto _LL0;case 276:
# 2327 "parse.y"
 yyval=Cyc_YY41(0);goto _LL0;case 277: _TAB5=yyyvsp;_TAB6=_TAB5[1];
# 2328 "parse.y"
yyval=_TAB6.v;goto _LL0;case 278:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TAB8=yyyvsp;_TAB9=& _TAB8[2].v;_TABA=(union Cyc_YYSTYPE*)_TAB9;_TABB=
# 2329 "parse.y"
Cyc_yyget_YY45(_TABA);_T187A->hd=Cyc_Absyn_regionsof_eff(_TABB);_T187A->tl=0;_TAB7=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY41(_TAB7);goto _LL0;case 279: _TABC=yyyvsp;_TABD=& _TABC[0].v;_TABE=(union Cyc_YYSTYPE*)_TABD;_TABF=
# 2331 "parse.y"
Cyc_yyget_YY45(_TABE);_TAC0=& Cyc_Kinds_ek;_TAC1=(struct Cyc_Absyn_Kind*)_TAC0;Cyc_Parse_set_vartyp_kind(_TABF,_TAC1,0);{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TAC3=yyyvsp;_TAC4=& _TAC3[0].v;_TAC5=(union Cyc_YYSTYPE*)_TAC4;
_T187A->hd=Cyc_yyget_YY45(_TAC5);_T187A->tl=0;_TAC2=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY41(_TAC2);goto _LL0;case 280:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TAC7=yyyvsp;_TAC8=& _TAC7[0].v;_TAC9=(union Cyc_YYSTYPE*)_TAC8;_TACA=
# 2339 "parse.y"
Cyc_yyget_YY38(_TAC9);_TACB=yyyvsp;_TACC=_TACB[0];_TACD=_TACC.l;_TACE=_TACD.first_line;_TACF=Cyc_Position_loc_to_seg(_TACE);_T187A->hd=Cyc_Parse_type_name_to_type(_TACA,_TACF);_T187A->tl=0;_TAC6=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY41(_TAC6);goto _LL0;case 281:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TAD1=yyyvsp;_TAD2=& _TAD1[0].v;_TAD3=(union Cyc_YYSTYPE*)_TAD2;_TAD4=
# 2341 "parse.y"
Cyc_yyget_YY38(_TAD3);_TAD5=yyyvsp;_TAD6=_TAD5[0];_TAD7=_TAD6.l;_TAD8=_TAD7.first_line;_TAD9=Cyc_Position_loc_to_seg(_TAD8);_T187A->hd=Cyc_Parse_type_name_to_type(_TAD4,_TAD9);_TADA=yyyvsp;_TADB=& _TADA[2].v;_TADC=(union Cyc_YYSTYPE*)_TADB;_T187A->tl=Cyc_yyget_YY41(_TADC);_TAD0=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY41(_TAD0);goto _LL0;case 282:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TADE=yyyvsp;_TADF=& _TADE[0].v;_TAE0=(union Cyc_YYSTYPE*)_TADF;
# 2346 "parse.y"
_T187A->hd=Cyc_yyget_YY38(_TAE0);_T187A->tl=0;_TADD=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY39(_TADD);goto _LL0;case 283:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TAE2=yyyvsp;_TAE3=& _TAE2[2].v;_TAE4=(union Cyc_YYSTYPE*)_TAE3;
# 2347 "parse.y"
_T187A->hd=Cyc_yyget_YY38(_TAE4);_TAE5=yyyvsp;_TAE6=& _TAE5[0].v;_TAE7=(union Cyc_YYSTYPE*)_TAE6;_T187A->tl=Cyc_yyget_YY39(_TAE7);_TAE1=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY39(_TAE1);goto _LL0;case 284: _TAE8=yyyvsp;_TAE9=& _TAE8[0].v;_TAEA=(union Cyc_YYSTYPE*)_TAE9;{
# 2353 "parse.y"
struct _tuple26 _T187A=Cyc_yyget_YY36(_TAEA);struct Cyc_List_List*_T187B;struct Cyc_Parse_Type_specifier _T187C;struct Cyc_Absyn_Tqual _T187D;_T187D=_T187A.f0;_T187C=_T187A.f1;_T187B=_T187A.f2;{struct Cyc_Absyn_Tqual tq=_T187D;struct Cyc_Parse_Type_specifier tspecs=_T187C;struct Cyc_List_List*atts=_T187B;_TAEB=tq;_TAEC=_TAEB.loc;
if(_TAEC!=0U)goto _TL2C8;_TAED=yyyvsp;_TAEE=_TAED[0];_TAEF=_TAEE.l;_TAF0=_TAEF.first_line;tq.loc=Cyc_Position_loc_to_seg(_TAF0);goto _TL2C9;_TL2C8: _TL2C9: _TAF1=yyyvsp;_TAF2=& _TAF1[1].v;_TAF3=(union Cyc_YYSTYPE*)_TAF2;{
struct Cyc_Parse_Declarator _T187E=Cyc_yyget_YY28(_TAF3);struct Cyc_List_List*_T187F;unsigned _T1880;struct _tuple0*_T1881;_T1881=_T187E.id;_T1880=_T187E.varloc;_T187F=_T187E.tms;{struct _tuple0*qv=_T1881;unsigned varloc=_T1880;struct Cyc_List_List*tms=_T187F;_TAF4=tspecs;_TAF5=yyyvsp;_TAF6=_TAF5[0];_TAF7=_TAF6.l;_TAF8=_TAF7.first_line;_TAF9=
Cyc_Position_loc_to_seg(_TAF8);{void*t=Cyc_Parse_speclist2typ(_TAF4,_TAF9);
struct _tuple14 _T1882=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T1883;struct Cyc_List_List*_T1884;void*_T1885;struct Cyc_Absyn_Tqual _T1886;_T1886=_T1882.f0;_T1885=_T1882.f1;_T1884=_T1882.f2;_T1883=_T1882.f3;{struct Cyc_Absyn_Tqual tq2=_T1886;void*t2=_T1885;struct Cyc_List_List*tvs=_T1884;struct Cyc_List_List*atts2=_T1883;
if(tvs==0)goto _TL2CA;{struct Cyc_Warn_String_Warn_Warg_struct _T1887;_T1887.tag=0;
_T1887.f1=_tag_fat("parameter with bad type params",sizeof(char),31U);_TAFA=_T1887;}{struct Cyc_Warn_String_Warn_Warg_struct _T1887=_TAFA;void*_T1888[1];_T1888[0]=& _T1887;_TAFB=yyyvsp;_TAFC=_TAFB[1];_TAFD=_TAFC.l;_TAFE=_TAFD.first_line;_TAFF=Cyc_Position_loc_to_seg(_TAFE);_TB00=_tag_fat(_T1888,sizeof(void*),1);Cyc_Warn_err2(_TAFF,_TB00);}goto _TL2CB;_TL2CA: _TL2CB: _TB01=
Cyc_Absyn_is_qvar_qualified(qv);if(!_TB01)goto _TL2CC;{struct Cyc_Warn_String_Warn_Warg_struct _T1887;_T1887.tag=0;
_T1887.f1=_tag_fat("parameter cannot be qualified with a namespace",sizeof(char),47U);_TB02=_T1887;}{struct Cyc_Warn_String_Warn_Warg_struct _T1887=_TB02;void*_T1888[1];_T1888[0]=& _T1887;_TB03=yyyvsp;_TB04=_TB03[0];_TB05=_TB04.l;_TB06=_TB05.first_line;_TB07=Cyc_Position_loc_to_seg(_TB06);_TB08=_tag_fat(_T1888,sizeof(void*),1);Cyc_Warn_err2(_TB07,_TB08);}goto _TL2CD;_TL2CC: _TL2CD: _TB09=qv;_TB0A=*_TB09;{
struct _fat_ptr*idopt=_TB0A.f1;
if(atts2==0)goto _TL2CE;{struct Cyc_Warn_String_Warn_Warg_struct _T1887;_T1887.tag=0;
_T1887.f1=_tag_fat("extra attributes on parameter, ignoring",sizeof(char),40U);_TB0B=_T1887;}{struct Cyc_Warn_String_Warn_Warg_struct _T1887=_TB0B;void*_T1888[1];_T1888[0]=& _T1887;_TB0C=yyyvsp;_TB0D=_TB0C[0];_TB0E=_TB0D.l;_TB0F=_TB0E.first_line;_TB10=Cyc_Position_loc_to_seg(_TB0F);_TB11=_tag_fat(_T1888,sizeof(void*),1);Cyc_Warn_warn2(_TB10,_TB11);}goto _TL2CF;_TL2CE: _TL2CF:{struct _tuple8*_T1887=_cycalloc(sizeof(struct _tuple8));
_T1887->f0=idopt;_T1887->f1=tq2;_T1887->f2=t2;_TB12=(struct _tuple8*)_T1887;}yyval=Cyc_YY38(_TB12);goto _LL0;}}}}}}}case 285: _TB13=yyyvsp;_TB14=& _TB13[0].v;_TB15=(union Cyc_YYSTYPE*)_TB14;{
# 2368 "parse.y"
struct _tuple26 _T187A=Cyc_yyget_YY36(_TB15);struct Cyc_List_List*_T187B;struct Cyc_Parse_Type_specifier _T187C;struct Cyc_Absyn_Tqual _T187D;_T187D=_T187A.f0;_T187C=_T187A.f1;_T187B=_T187A.f2;{struct Cyc_Absyn_Tqual tq=_T187D;struct Cyc_Parse_Type_specifier tspecs=_T187C;struct Cyc_List_List*atts=_T187B;_TB16=tq;_TB17=_TB16.loc;
if(_TB17!=0U)goto _TL2D0;_TB18=yyyvsp;_TB19=_TB18[0];_TB1A=_TB19.l;_TB1B=_TB1A.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB1B);goto _TL2D1;_TL2D0: _TL2D1: _TB1C=tspecs;_TB1D=yyyvsp;_TB1E=_TB1D[0];_TB1F=_TB1E.l;_TB20=_TB1F.first_line;_TB21=
Cyc_Position_loc_to_seg(_TB20);{void*t=Cyc_Parse_speclist2typ(_TB1C,_TB21);
if(atts==0)goto _TL2D2;{struct Cyc_Warn_String_Warn_Warg_struct _T187E;_T187E.tag=0;
_T187E.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB22=_T187E;}{struct Cyc_Warn_String_Warn_Warg_struct _T187E=_TB22;void*_T187F[1];_T187F[0]=& _T187E;_TB23=yyyvsp;_TB24=_TB23[0];_TB25=_TB24.l;_TB26=_TB25.first_line;_TB27=Cyc_Position_loc_to_seg(_TB26);_TB28=_tag_fat(_T187F,sizeof(void*),1);Cyc_Warn_warn2(_TB27,_TB28);}goto _TL2D3;_TL2D2: _TL2D3:{struct _tuple8*_T187E=_cycalloc(sizeof(struct _tuple8));
_T187E->f0=0;_T187E->f1=tq;_T187E->f2=t;_TB29=(struct _tuple8*)_T187E;}yyval=Cyc_YY38(_TB29);goto _LL0;}}}case 286: _TB2A=yyyvsp;_TB2B=& _TB2A[0].v;_TB2C=(union Cyc_YYSTYPE*)_TB2B;{
# 2376 "parse.y"
struct _tuple26 _T187A=Cyc_yyget_YY36(_TB2C);struct Cyc_List_List*_T187B;struct Cyc_Parse_Type_specifier _T187C;struct Cyc_Absyn_Tqual _T187D;_T187D=_T187A.f0;_T187C=_T187A.f1;_T187B=_T187A.f2;{struct Cyc_Absyn_Tqual tq=_T187D;struct Cyc_Parse_Type_specifier tspecs=_T187C;struct Cyc_List_List*atts=_T187B;_TB2D=tq;_TB2E=_TB2D.loc;
if(_TB2E!=0U)goto _TL2D4;_TB2F=yyyvsp;_TB30=_TB2F[0];_TB31=_TB30.l;_TB32=_TB31.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB32);goto _TL2D5;_TL2D4: _TL2D5: _TB33=tspecs;_TB34=yyyvsp;_TB35=_TB34[0];_TB36=_TB35.l;_TB37=_TB36.first_line;_TB38=
Cyc_Position_loc_to_seg(_TB37);{void*t=Cyc_Parse_speclist2typ(_TB33,_TB38);_TB39=yyyvsp;_TB3A=& _TB39[1].v;_TB3B=(union Cyc_YYSTYPE*)_TB3A;_TB3C=
Cyc_yyget_YY31(_TB3B);{struct Cyc_List_List*tms=_TB3C.tms;
struct _tuple14 _T187E=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T187F;struct Cyc_List_List*_T1880;void*_T1881;struct Cyc_Absyn_Tqual _T1882;_T1882=_T187E.f0;_T1881=_T187E.f1;_T1880=_T187E.f2;_T187F=_T187E.f3;{struct Cyc_Absyn_Tqual tq2=_T1882;void*t2=_T1881;struct Cyc_List_List*tvs=_T1880;struct Cyc_List_List*atts2=_T187F;
if(tvs==0)goto _TL2D6;{struct Cyc_Warn_String_Warn_Warg_struct _T1883;_T1883.tag=0;
# 2383
_T1883.f1=_tag_fat("bad type parameters on formal argument, ignoring",sizeof(char),49U);_TB3D=_T1883;}{struct Cyc_Warn_String_Warn_Warg_struct _T1883=_TB3D;void*_T1884[1];_T1884[0]=& _T1883;_TB3E=yyyvsp;_TB3F=_TB3E[0];_TB40=_TB3F.l;_TB41=_TB40.first_line;_TB42=
# 2382
Cyc_Position_loc_to_seg(_TB41);_TB43=_tag_fat(_T1884,sizeof(void*),1);Cyc_Warn_warn2(_TB42,_TB43);}goto _TL2D7;_TL2D6: _TL2D7:
# 2384
 if(atts2==0)goto _TL2D8;{struct Cyc_Warn_String_Warn_Warg_struct _T1883;_T1883.tag=0;
_T1883.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB44=_T1883;}{struct Cyc_Warn_String_Warn_Warg_struct _T1883=_TB44;void*_T1884[1];_T1884[0]=& _T1883;_TB45=yyyvsp;_TB46=_TB45[0];_TB47=_TB46.l;_TB48=_TB47.first_line;_TB49=Cyc_Position_loc_to_seg(_TB48);_TB4A=_tag_fat(_T1884,sizeof(void*),1);Cyc_Warn_warn2(_TB49,_TB4A);}goto _TL2D9;_TL2D8: _TL2D9:{struct _tuple8*_T1883=_cycalloc(sizeof(struct _tuple8));
_T1883->f0=0;_T1883->f1=tq2;_T1883->f2=t2;_TB4B=(struct _tuple8*)_T1883;}yyval=Cyc_YY38(_TB4B);goto _LL0;}}}}}case 287: _TB4C=yyyvsp;_TB4D=& _TB4C[0].v;_TB4E=(union Cyc_YYSTYPE*)_TB4D;_TB4F=
# 2391 "parse.y"
Cyc_yyget_YY37(_TB4E);_TB50=Cyc_List_imp_rev(_TB4F);yyval=Cyc_YY37(_TB50);goto _LL0;case 288:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_TB53=yyyvsp;_TB54=& _TB53[0].v;_TB55=(union Cyc_YYSTYPE*)_TB54;
# 2394
*_T187B=Cyc_yyget_String_tok(_TB55);_TB52=(struct _fat_ptr*)_T187B;}_T187A->hd=_TB52;_T187A->tl=0;_TB51=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY37(_TB51);goto _LL0;case 289:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_TB58=yyyvsp;_TB59=& _TB58[2].v;_TB5A=(union Cyc_YYSTYPE*)_TB59;
# 2395 "parse.y"
*_T187B=Cyc_yyget_String_tok(_TB5A);_TB57=(struct _fat_ptr*)_T187B;}_T187A->hd=_TB57;_TB5B=yyyvsp;_TB5C=& _TB5B[0].v;_TB5D=(union Cyc_YYSTYPE*)_TB5C;_T187A->tl=Cyc_yyget_YY37(_TB5D);_TB56=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY37(_TB56);goto _LL0;case 290: _TB5E=yyyvsp;_TB5F=_TB5E[0];
# 2399 "parse.y"
yyval=_TB5F.v;goto _LL0;case 291: _TB60=yyyvsp;_TB61=_TB60[0];
# 2400 "parse.y"
yyval=_TB61.v;goto _LL0;case 292:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T187A->tag=35;
# 2405 "parse.y"
_T187A->f1=0;_T187A->f2=0;_TB62=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T187A;}_TB63=(void*)_TB62;_TB64=yyyvsp;_TB65=_TB64[0];_TB66=_TB65.l;_TB67=_TB66.first_line;_TB68=Cyc_Position_loc_to_seg(_TB67);_TB69=Cyc_Absyn_new_exp(_TB63,_TB68);yyval=Cyc_Exp_tok(_TB69);goto _LL0;case 293:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T187A->tag=35;
# 2407 "parse.y"
_T187A->f1=0;_TB6B=yyyvsp;_TB6C=& _TB6B[1].v;_TB6D=(union Cyc_YYSTYPE*)_TB6C;_TB6E=Cyc_yyget_YY5(_TB6D);_T187A->f2=Cyc_List_imp_rev(_TB6E);_TB6A=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T187A;}_TB6F=(void*)_TB6A;_TB70=yyyvsp;_TB71=_TB70[0];_TB72=_TB71.l;_TB73=_TB72.first_line;_TB74=Cyc_Position_loc_to_seg(_TB73);_TB75=Cyc_Absyn_new_exp(_TB6F,_TB74);yyval=Cyc_Exp_tok(_TB75);goto _LL0;case 294:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T187A->tag=35;
# 2409 "parse.y"
_T187A->f1=0;_TB77=yyyvsp;_TB78=& _TB77[1].v;_TB79=(union Cyc_YYSTYPE*)_TB78;_TB7A=Cyc_yyget_YY5(_TB79);_T187A->f2=Cyc_List_imp_rev(_TB7A);_TB76=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T187A;}_TB7B=(void*)_TB76;_TB7C=yyyvsp;_TB7D=_TB7C[0];_TB7E=_TB7D.l;_TB7F=_TB7E.first_line;_TB80=Cyc_Position_loc_to_seg(_TB7F);_TB81=Cyc_Absyn_new_exp(_TB7B,_TB80);yyval=Cyc_Exp_tok(_TB81);goto _LL0;case 295: _TB82=yyyvsp;_TB83=_TB82[2];_TB84=_TB83.l;_TB85=_TB84.first_line;_TB86=
# 2411 "parse.y"
Cyc_Position_loc_to_seg(_TB85);{struct _tuple0*_T187A=_cycalloc(sizeof(struct _tuple0));_T187A->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_TB89=yyyvsp;_TB8A=& _TB89[2].v;_TB8B=(union Cyc_YYSTYPE*)_TB8A;*_T187B=Cyc_yyget_String_tok(_TB8B);_TB88=(struct _fat_ptr*)_T187B;}_T187A->f1=_TB88;_TB87=(struct _tuple0*)_T187A;}_TB8C=Cyc_Absyn_uint_type;_TB8D=yyyvsp;_TB8E=_TB8D[2];_TB8F=_TB8E.l;_TB90=_TB8F.first_line;_TB91=
Cyc_Position_loc_to_seg(_TB90);_TB92=Cyc_Absyn_uint_exp(0U,_TB91);{
# 2411
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_TB86,_TB87,_TB8C,_TB92,0);_TB93=vd;
# 2414
_TB93->tq.real_const=1;{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_T187A->tag=26;
_T187A->f1=vd;_TB95=yyyvsp;_TB96=& _TB95[4].v;_TB97=(union Cyc_YYSTYPE*)_TB96;_T187A->f2=Cyc_yyget_Exp_tok(_TB97);_TB98=yyyvsp;_TB99=& _TB98[6].v;_TB9A=(union Cyc_YYSTYPE*)_TB99;_T187A->f3=Cyc_yyget_Exp_tok(_TB9A);_T187A->f4=0;_TB94=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T187A;}_TB9B=(void*)_TB94;_TB9C=yyyvsp;_TB9D=_TB9C[0];_TB9E=_TB9D.l;_TB9F=_TB9E.first_line;_TBA0=Cyc_Position_loc_to_seg(_TB9F);_TBA1=Cyc_Absyn_new_exp(_TB9B,_TBA0);yyval=Cyc_Exp_tok(_TBA1);goto _LL0;}case 296: _TBA2=yyyvsp;_TBA3=& _TBA2[6].v;_TBA4=(union Cyc_YYSTYPE*)_TBA3;_TBA5=
# 2419 "parse.y"
Cyc_yyget_YY38(_TBA4);_TBA6=yyyvsp;_TBA7=_TBA6[6];_TBA8=_TBA7.l;_TBA9=_TBA8.first_line;_TBAA=Cyc_Position_loc_to_seg(_TBA9);{void*t=Cyc_Parse_type_name_to_type(_TBA5,_TBAA);{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_T187A->tag=27;_TBAC=yyyvsp;_TBAD=& _TBAC[4].v;_TBAE=(union Cyc_YYSTYPE*)_TBAD;
_T187A->f1=Cyc_yyget_Exp_tok(_TBAE);_T187A->f2=t;_T187A->f3=0;_TBAB=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T187A;}_TBAF=(void*)_TBAB;_TBB0=yyyvsp;_TBB1=_TBB0[0];_TBB2=_TBB1.l;_TBB3=_TBB2.first_line;_TBB4=Cyc_Position_loc_to_seg(_TBB3);_TBB5=Cyc_Absyn_new_exp(_TBAF,_TBB4);yyval=Cyc_Exp_tok(_TBB5);goto _LL0;}case 297:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T187B=_cycalloc(sizeof(struct _tuple34));
# 2427 "parse.y"
_T187B->f0=0;_TBB8=yyyvsp;_TBB9=& _TBB8[0].v;_TBBA=(union Cyc_YYSTYPE*)_TBB9;_T187B->f1=Cyc_yyget_Exp_tok(_TBBA);_TBB7=(struct _tuple34*)_T187B;}_T187A->hd=_TBB7;_T187A->tl=0;_TBB6=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY5(_TBB6);goto _LL0;case 298:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T187B=_cycalloc(sizeof(struct _tuple34));_TBBD=yyyvsp;_TBBE=& _TBBD[0].v;_TBBF=(union Cyc_YYSTYPE*)_TBBE;
# 2429 "parse.y"
_T187B->f0=Cyc_yyget_YY42(_TBBF);_TBC0=yyyvsp;_TBC1=& _TBC0[1].v;_TBC2=(union Cyc_YYSTYPE*)_TBC1;_T187B->f1=Cyc_yyget_Exp_tok(_TBC2);_TBBC=(struct _tuple34*)_T187B;}_T187A->hd=_TBBC;_T187A->tl=0;_TBBB=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY5(_TBBB);goto _LL0;case 299:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T187B=_cycalloc(sizeof(struct _tuple34));
# 2431 "parse.y"
_T187B->f0=0;_TBC5=yyyvsp;_TBC6=& _TBC5[2].v;_TBC7=(union Cyc_YYSTYPE*)_TBC6;_T187B->f1=Cyc_yyget_Exp_tok(_TBC7);_TBC4=(struct _tuple34*)_T187B;}_T187A->hd=_TBC4;_TBC8=yyyvsp;_TBC9=& _TBC8[0].v;_TBCA=(union Cyc_YYSTYPE*)_TBC9;_T187A->tl=Cyc_yyget_YY5(_TBCA);_TBC3=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY5(_TBC3);goto _LL0;case 300:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T187B=_cycalloc(sizeof(struct _tuple34));_TBCD=yyyvsp;_TBCE=& _TBCD[2].v;_TBCF=(union Cyc_YYSTYPE*)_TBCE;
# 2433 "parse.y"
_T187B->f0=Cyc_yyget_YY42(_TBCF);_TBD0=yyyvsp;_TBD1=& _TBD0[3].v;_TBD2=(union Cyc_YYSTYPE*)_TBD1;_T187B->f1=Cyc_yyget_Exp_tok(_TBD2);_TBCC=(struct _tuple34*)_T187B;}_T187A->hd=_TBCC;_TBD3=yyyvsp;_TBD4=& _TBD3[0].v;_TBD5=(union Cyc_YYSTYPE*)_TBD4;_T187A->tl=Cyc_yyget_YY5(_TBD5);_TBCB=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY5(_TBCB);goto _LL0;case 301: _TBD6=yyyvsp;_TBD7=& _TBD6[0].v;_TBD8=(union Cyc_YYSTYPE*)_TBD7;_TBD9=
# 2437 "parse.y"
Cyc_yyget_YY42(_TBD8);_TBDA=Cyc_List_imp_rev(_TBD9);yyval=Cyc_YY42(_TBDA);goto _LL0;case 302:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T187B->tag=1;{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));_TBDE=yyyvsp;_TBDF=& _TBDE[0].v;_TBE0=(union Cyc_YYSTYPE*)_TBDF;
# 2438 "parse.y"
*_T187C=Cyc_yyget_String_tok(_TBE0);_TBDD=(struct _fat_ptr*)_T187C;}_T187B->f1=_TBDD;_TBDC=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T187B;}_T187A->hd=(void*)_TBDC;_T187A->tl=0;_TBDB=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY42(_TBDB);goto _LL0;case 303:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TBE2=yyyvsp;_TBE3=& _TBE2[0].v;_TBE4=(union Cyc_YYSTYPE*)_TBE3;
# 2443 "parse.y"
_T187A->hd=Cyc_yyget_YY43(_TBE4);_T187A->tl=0;_TBE1=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY42(_TBE1);goto _LL0;case 304:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TBE6=yyyvsp;_TBE7=& _TBE6[1].v;_TBE8=(union Cyc_YYSTYPE*)_TBE7;
# 2444 "parse.y"
_T187A->hd=Cyc_yyget_YY43(_TBE8);_TBE9=yyyvsp;_TBEA=& _TBE9[0].v;_TBEB=(union Cyc_YYSTYPE*)_TBEA;_T187A->tl=Cyc_yyget_YY42(_TBEB);_TBE5=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY42(_TBE5);goto _LL0;case 305:{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_T187A->tag=0;_TBED=yyyvsp;_TBEE=& _TBED[1].v;_TBEF=(union Cyc_YYSTYPE*)_TBEE;
# 2448 "parse.y"
_T187A->f1=Cyc_yyget_Exp_tok(_TBEF);_TBEC=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_T187A;}_TBF0=(void*)_TBEC;yyval=Cyc_YY43(_TBF0);goto _LL0;case 306:{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T187A->tag=1;{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_TBF3=yyyvsp;_TBF4=& _TBF3[1].v;_TBF5=(union Cyc_YYSTYPE*)_TBF4;
# 2449 "parse.y"
*_T187B=Cyc_yyget_String_tok(_TBF5);_TBF2=(struct _fat_ptr*)_T187B;}_T187A->f1=_TBF2;_TBF1=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T187A;}_TBF6=(void*)_TBF1;yyval=Cyc_YY43(_TBF6);goto _LL0;case 307: _TBF7=yyyvsp;_TBF8=& _TBF7[0].v;_TBF9=(union Cyc_YYSTYPE*)_TBF8;{
# 2454 "parse.y"
struct _tuple26 _T187A=Cyc_yyget_YY36(_TBF9);struct Cyc_List_List*_T187B;struct Cyc_Parse_Type_specifier _T187C;struct Cyc_Absyn_Tqual _T187D;_T187D=_T187A.f0;_T187C=_T187A.f1;_T187B=_T187A.f2;{struct Cyc_Absyn_Tqual tq=_T187D;struct Cyc_Parse_Type_specifier tss=_T187C;struct Cyc_List_List*atts=_T187B;_TBFA=tss;_TBFB=yyyvsp;_TBFC=_TBFB[0];_TBFD=_TBFC.l;_TBFE=_TBFD.first_line;_TBFF=
Cyc_Position_loc_to_seg(_TBFE);{void*t=Cyc_Parse_speclist2typ(_TBFA,_TBFF);
if(atts==0)goto _TL2DA;_TC00=yyyvsp;_TC01=_TC00[0];_TC02=_TC01.l;_TC03=_TC02.first_line;_TC04=
Cyc_Position_loc_to_seg(_TC03);_TC05=_tag_fat("ignoring attributes in type",sizeof(char),28U);_TC06=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC04,_TC05,_TC06);goto _TL2DB;_TL2DA: _TL2DB:{struct _tuple8*_T187E=_cycalloc(sizeof(struct _tuple8));
_T187E->f0=0;_T187E->f1=tq;_T187E->f2=t;_TC07=(struct _tuple8*)_T187E;}yyval=Cyc_YY38(_TC07);goto _LL0;}}}case 308: _TC08=yyyvsp;_TC09=& _TC08[0].v;_TC0A=(union Cyc_YYSTYPE*)_TC09;{
# 2461 "parse.y"
struct _tuple26 _T187A=Cyc_yyget_YY36(_TC0A);struct Cyc_List_List*_T187B;struct Cyc_Parse_Type_specifier _T187C;struct Cyc_Absyn_Tqual _T187D;_T187D=_T187A.f0;_T187C=_T187A.f1;_T187B=_T187A.f2;{struct Cyc_Absyn_Tqual tq=_T187D;struct Cyc_Parse_Type_specifier tss=_T187C;struct Cyc_List_List*atts=_T187B;_TC0B=tss;_TC0C=yyyvsp;_TC0D=_TC0C[0];_TC0E=_TC0D.l;_TC0F=_TC0E.first_line;_TC10=
Cyc_Position_loc_to_seg(_TC0F);{void*t=Cyc_Parse_speclist2typ(_TC0B,_TC10);_TC11=yyyvsp;_TC12=& _TC11[1].v;_TC13=(union Cyc_YYSTYPE*)_TC12;_TC14=
Cyc_yyget_YY31(_TC13);{struct Cyc_List_List*tms=_TC14.tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);_TC15=t_info;_TC16=_TC15.f2;
if(_TC16==0)goto _TL2DC;_TC17=yyyvsp;_TC18=_TC17[1];_TC19=_TC18.l;_TC1A=_TC19.first_line;_TC1B=
Cyc_Position_loc_to_seg(_TC1A);_TC1C=_tag_fat("bad type params, ignoring",sizeof(char),26U);_TC1D=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC1B,_TC1C,_TC1D);goto _TL2DD;_TL2DC: _TL2DD: _TC1E=t_info;_TC1F=_TC1E.f3;
if(_TC1F==0)goto _TL2DE;_TC20=yyyvsp;_TC21=_TC20[1];_TC22=_TC21.l;_TC23=_TC22.first_line;_TC24=
Cyc_Position_loc_to_seg(_TC23);_TC25=_tag_fat("bad specifiers, ignoring",sizeof(char),25U);_TC26=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC24,_TC25,_TC26);goto _TL2DF;_TL2DE: _TL2DF:{struct _tuple8*_T187E=_cycalloc(sizeof(struct _tuple8));
_T187E->f0=0;_TC28=t_info;_T187E->f1=_TC28.f0;_TC29=t_info;_T187E->f2=_TC29.f1;_TC27=(struct _tuple8*)_T187E;}yyval=Cyc_YY38(_TC27);goto _LL0;}}}}case 309: _TC2A=yyyvsp;_TC2B=& _TC2A[0].v;_TC2C=(union Cyc_YYSTYPE*)_TC2B;_TC2D=
# 2474 "parse.y"
Cyc_yyget_YY38(_TC2C);_TC2E=yyyvsp;_TC2F=_TC2E[0];_TC30=_TC2F.l;_TC31=_TC30.first_line;_TC32=Cyc_Position_loc_to_seg(_TC31);_TC33=Cyc_Parse_type_name_to_type(_TC2D,_TC32);yyval=Cyc_YY45(_TC33);goto _LL0;case 310: _TC34=
# 2475 "parse.y"
Cyc_Absyn_join_eff(0);yyval=Cyc_YY45(_TC34);goto _LL0;case 311: _TC35=yyyvsp;_TC36=& _TC35[1].v;_TC37=(union Cyc_YYSTYPE*)_TC36;_TC38=
# 2476 "parse.y"
Cyc_yyget_YY41(_TC37);_TC39=Cyc_Absyn_join_eff(_TC38);yyval=Cyc_YY45(_TC39);goto _LL0;case 312: _TC3A=yyyvsp;_TC3B=& _TC3A[2].v;_TC3C=(union Cyc_YYSTYPE*)_TC3B;_TC3D=
# 2477 "parse.y"
Cyc_yyget_YY45(_TC3C);_TC3E=Cyc_Absyn_regionsof_eff(_TC3D);yyval=Cyc_YY45(_TC3E);goto _LL0;case 313:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TC40=yyyvsp;_TC41=& _TC40[0].v;_TC42=(union Cyc_YYSTYPE*)_TC41;
# 2478 "parse.y"
_T187A->hd=Cyc_yyget_YY45(_TC42);_TC43=yyyvsp;_TC44=& _TC43[2].v;_TC45=(union Cyc_YYSTYPE*)_TC44;_T187A->tl=Cyc_yyget_YY41(_TC45);_TC3F=(struct Cyc_List_List*)_T187A;}_TC46=Cyc_Absyn_join_eff(_TC3F);yyval=Cyc_YY45(_TC46);goto _LL0;case 314: _TC47=yyyvsp;_TC48=& _TC47[0].v;_TC49=(union Cyc_YYSTYPE*)_TC48;_TC4A=
# 2479 "parse.y"
Cyc_yyget_YY45(_TC49);yyval=Cyc_YY45(_TC4A);goto _LL0;case 315:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TC4C=yyyvsp;_TC4D=& _TC4C[0].v;_TC4E=(union Cyc_YYSTYPE*)_TC4D;
# 2485 "parse.y"
_T187A->hd=Cyc_yyget_YY45(_TC4E);_T187A->tl=0;_TC4B=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY41(_TC4B);goto _LL0;case 316:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_TC50=yyyvsp;_TC51=& _TC50[2].v;_TC52=(union Cyc_YYSTYPE*)_TC51;
# 2486 "parse.y"
_T187A->hd=Cyc_yyget_YY45(_TC52);_TC53=yyyvsp;_TC54=& _TC53[0].v;_TC55=(union Cyc_YYSTYPE*)_TC54;_T187A->tl=Cyc_yyget_YY41(_TC55);_TC4F=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY41(_TC4F);goto _LL0;case 317:{struct Cyc_Parse_Abstractdeclarator _T187A;_TC57=yyyvsp;_TC58=& _TC57[0].v;_TC59=(union Cyc_YYSTYPE*)_TC58;
# 2490 "parse.y"
_T187A.tms=Cyc_yyget_YY27(_TC59);_TC56=_T187A;}yyval=Cyc_YY31(_TC56);goto _LL0;case 318: _TC5A=yyyvsp;_TC5B=_TC5A[0];
# 2491 "parse.y"
yyval=_TC5B.v;goto _LL0;case 319:{struct Cyc_Parse_Abstractdeclarator _T187A;_TC5D=yyyvsp;_TC5E=& _TC5D[0].v;_TC5F=(union Cyc_YYSTYPE*)_TC5E;_TC60=
# 2493 "parse.y"
Cyc_yyget_YY27(_TC5F);_TC61=yyyvsp;_TC62=& _TC61[1].v;_TC63=(union Cyc_YYSTYPE*)_TC62;_TC64=Cyc_yyget_YY31(_TC63);_TC65=_TC64.tms;_T187A.tms=Cyc_List_imp_append(_TC60,_TC65);_TC5C=_T187A;}yyval=Cyc_YY31(_TC5C);goto _LL0;case 320: _TC66=yyyvsp;_TC67=_TC66[1];
# 2498 "parse.y"
yyval=_TC67.v;goto _LL0;case 321:{struct Cyc_Parse_Abstractdeclarator _T187A;_TC6A=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_TC6A,0U,sizeof(struct Cyc_List_List));_TC6C=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_TC6C,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T187C->tag=0;_TC6D=yyyvsp;_TC6E=& _TC6D[2].v;_TC6F=(union Cyc_YYSTYPE*)_TC6E;
# 2500 "parse.y"
_T187C->f1=Cyc_yyget_YY54(_TC6F);_TC70=yyyvsp;_TC71=_TC70[2];_TC72=_TC71.l;_TC73=_TC72.first_line;_T187C->f2=Cyc_Position_loc_to_seg(_TC73);_TC6B=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_TC6B;_T187B->tl=0;_TC69=(struct Cyc_List_List*)_T187B;}_T187A.tms=_TC69;_TC68=_T187A;}yyval=Cyc_YY31(_TC68);goto _LL0;case 322:{struct Cyc_Parse_Abstractdeclarator _T187A;_TC76=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_TC76,0U,sizeof(struct Cyc_List_List));_TC78=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_TC78,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T187C->tag=0;_TC79=yyyvsp;_TC7A=& _TC79[3].v;_TC7B=(union Cyc_YYSTYPE*)_TC7A;
# 2502 "parse.y"
_T187C->f1=Cyc_yyget_YY54(_TC7B);_TC7C=yyyvsp;_TC7D=_TC7C[3];_TC7E=_TC7D.l;_TC7F=_TC7E.first_line;_T187C->f2=Cyc_Position_loc_to_seg(_TC7F);_TC77=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_TC77;_TC80=yyyvsp;_TC81=& _TC80[0].v;_TC82=(union Cyc_YYSTYPE*)_TC81;_TC83=Cyc_yyget_YY31(_TC82);_T187B->tl=_TC83.tms;_TC75=(struct Cyc_List_List*)_T187B;}_T187A.tms=_TC75;_TC74=_T187A;}yyval=Cyc_YY31(_TC74);goto _LL0;case 323:{struct Cyc_Parse_Abstractdeclarator _T187A;_TC86=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_TC86,0U,sizeof(struct Cyc_List_List));_TC88=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_TC88,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T187C->tag=1;_TC89=yyyvsp;_TC8A=& _TC89[1].v;_TC8B=(union Cyc_YYSTYPE*)_TC8A;
# 2504 "parse.y"
_T187C->f1=Cyc_yyget_Exp_tok(_TC8B);_TC8C=yyyvsp;_TC8D=& _TC8C[3].v;_TC8E=(union Cyc_YYSTYPE*)_TC8D;_T187C->f2=Cyc_yyget_YY54(_TC8E);_TC8F=yyyvsp;_TC90=_TC8F[3];_TC91=_TC90.l;_TC92=_TC91.first_line;_T187C->f3=Cyc_Position_loc_to_seg(_TC92);_TC87=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_TC87;_T187B->tl=0;_TC85=(struct Cyc_List_List*)_T187B;}_T187A.tms=_TC85;_TC84=_T187A;}yyval=Cyc_YY31(_TC84);goto _LL0;case 324:{struct Cyc_Parse_Abstractdeclarator _T187A;_TC95=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_TC95,0U,sizeof(struct Cyc_List_List));_TC97=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_TC97,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T187C->tag=1;_TC98=yyyvsp;_TC99=& _TC98[2].v;_TC9A=(union Cyc_YYSTYPE*)_TC99;
# 2506 "parse.y"
_T187C->f1=Cyc_yyget_Exp_tok(_TC9A);_TC9B=yyyvsp;_TC9C=& _TC9B[4].v;_TC9D=(union Cyc_YYSTYPE*)_TC9C;_T187C->f2=Cyc_yyget_YY54(_TC9D);_TC9E=yyyvsp;_TC9F=_TC9E[4];_TCA0=_TC9F.l;_TCA1=_TCA0.first_line;_T187C->f3=Cyc_Position_loc_to_seg(_TCA1);_TC96=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_TC96;_TCA2=yyyvsp;_TCA3=& _TCA2[0].v;_TCA4=(union Cyc_YYSTYPE*)_TCA3;_TCA5=
Cyc_yyget_YY31(_TCA4);_T187B->tl=_TCA5.tms;_TC94=(struct Cyc_List_List*)_T187B;}
# 2506
_T187A.tms=_TC94;_TC93=_T187A;}yyval=Cyc_YY31(_TC93);goto _LL0;case 325: _TCA6=yyyvsp;_TCA7=& _TCA6[1].v;_TCA8=(union Cyc_YYSTYPE*)_TCA7;{
# 2510 "parse.y"
struct _tuple27*_T187A=Cyc_yyget_YY40(_TCA8);struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;void*_T187D;struct Cyc_Absyn_VarargInfo*_T187E;int _T187F;struct Cyc_List_List*_T1880;{struct _tuple27 _T1881=*_T187A;_T1880=_T1881.f0;_T187F=_T1881.f1;_T187E=_T1881.f2;_T187D=_T1881.f3;_T187C=_T1881.f4;_T187B=_T1881.f5;}{struct Cyc_List_List*lis=_T1880;int b=_T187F;struct Cyc_Absyn_VarargInfo*c=_T187E;void*eff=_T187D;struct Cyc_List_List*ec=_T187C;struct Cyc_List_List*qb=_T187B;_TCA9=yyyvsp;_TCAA=& _TCA9[3].v;_TCAB=(union Cyc_YYSTYPE*)_TCAA;{
struct _tuple21 _T1881=Cyc_yyget_YY62(_TCAB);struct Cyc_Absyn_Exp*_T1882;struct Cyc_Absyn_Exp*_T1883;struct Cyc_Absyn_Exp*_T1884;struct Cyc_Absyn_Exp*_T1885;_T1885=_T1881.f0;_T1884=_T1881.f1;_T1883=_T1881.f2;_T1882=_T1881.f3;{struct Cyc_Absyn_Exp*chk=_T1885;struct Cyc_Absyn_Exp*req=_T1884;struct Cyc_Absyn_Exp*ens=_T1883;struct Cyc_Absyn_Exp*thrws=_T1882;{struct Cyc_Parse_Abstractdeclarator _T1886;_TCAE=yyr;{struct Cyc_List_List*_T1887=_region_malloc(_TCAE,0U,sizeof(struct Cyc_List_List));_TCB0=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1888=_region_malloc(_TCB0,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1888->tag=3;_TCB2=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1889=_region_malloc(_TCB2,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1889->tag=1;
_T1889->f1=lis;_T1889->f2=b;_T1889->f3=c;_T1889->f4=eff;_T1889->f5=ec;_T1889->f6=qb;_T1889->f7=chk;_T1889->f8=req;_T1889->f9=ens;_T1889->f10=thrws;_TCB1=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1889;}_T1888->f1=(void*)_TCB1;_TCAF=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1888;}_T1887->hd=(void*)_TCAF;_T1887->tl=0;_TCAD=(struct Cyc_List_List*)_T1887;}_T1886.tms=_TCAD;_TCAC=_T1886;}yyval=Cyc_YY31(_TCAC);goto _LL0;}}}}case 326: _TCB3=yyyvsp;_TCB4=& _TCB3[2].v;_TCB5=(union Cyc_YYSTYPE*)_TCB4;{
# 2515 "parse.y"
struct _tuple27*_T187A=Cyc_yyget_YY40(_TCB5);struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;void*_T187D;struct Cyc_Absyn_VarargInfo*_T187E;int _T187F;struct Cyc_List_List*_T1880;{struct _tuple27 _T1881=*_T187A;_T1880=_T1881.f0;_T187F=_T1881.f1;_T187E=_T1881.f2;_T187D=_T1881.f3;_T187C=_T1881.f4;_T187B=_T1881.f5;}{struct Cyc_List_List*lis=_T1880;int b=_T187F;struct Cyc_Absyn_VarargInfo*c=_T187E;void*eff=_T187D;struct Cyc_List_List*ec=_T187C;struct Cyc_List_List*qb=_T187B;_TCB6=yyyvsp;_TCB7=& _TCB6[4].v;_TCB8=(union Cyc_YYSTYPE*)_TCB7;{
struct _tuple21 _T1881=Cyc_yyget_YY62(_TCB8);struct Cyc_Absyn_Exp*_T1882;struct Cyc_Absyn_Exp*_T1883;struct Cyc_Absyn_Exp*_T1884;struct Cyc_Absyn_Exp*_T1885;_T1885=_T1881.f0;_T1884=_T1881.f1;_T1883=_T1881.f2;_T1882=_T1881.f3;{struct Cyc_Absyn_Exp*chk=_T1885;struct Cyc_Absyn_Exp*req=_T1884;struct Cyc_Absyn_Exp*ens=_T1883;struct Cyc_Absyn_Exp*thrws=_T1882;{struct Cyc_Parse_Abstractdeclarator _T1886;_TCBB=yyr;{struct Cyc_List_List*_T1887=_region_malloc(_TCBB,0U,sizeof(struct Cyc_List_List));_TCBD=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1888=_region_malloc(_TCBD,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1888->tag=3;_TCBF=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1889=_region_malloc(_TCBF,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1889->tag=1;
_T1889->f1=lis;
_T1889->f2=b;_T1889->f3=c;_T1889->f4=eff;_T1889->f5=ec;_T1889->f6=qb;_T1889->f7=chk;_T1889->f8=req;_T1889->f9=ens;_T1889->f10=thrws;_TCBE=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1889;}
# 2517
_T1888->f1=(void*)_TCBE;_TCBC=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1888;}_T1887->hd=(void*)_TCBC;_TCC0=yyyvsp;_TCC1=& _TCC0[0].v;_TCC2=(union Cyc_YYSTYPE*)_TCC1;_TCC3=
Cyc_yyget_YY31(_TCC2);_T1887->tl=_TCC3.tms;_TCBA=(struct Cyc_List_List*)_T1887;}
# 2517
_T1886.tms=_TCBA;_TCB9=_T1886;}yyval=Cyc_YY31(_TCB9);goto _LL0;}}}}case 327: _TCC5=Cyc_List_map_c;{
# 2522
struct Cyc_List_List*(*_T187A)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_TCC5;_TCC4=_T187A;}_TCC6=yyyvsp;_TCC7=_TCC6[1];_TCC8=_TCC7.l;_TCC9=_TCC8.first_line;_TCCA=Cyc_Position_loc_to_seg(_TCC9);_TCCB=yyyvsp;_TCCC=& _TCCB[2].v;_TCCD=(union Cyc_YYSTYPE*)_TCCC;_TCCE=Cyc_yyget_YY41(_TCCD);_TCCF=Cyc_List_imp_rev(_TCCE);{struct Cyc_List_List*ts=_TCC4(Cyc_Parse_typ2tvar,_TCCA,_TCCF);{struct Cyc_Parse_Abstractdeclarator _T187A;_TCD2=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_TCD2,0U,sizeof(struct Cyc_List_List));_TCD4=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_TCD4,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T187C->tag=4;
_T187C->f1=ts;_TCD5=yyyvsp;_TCD6=_TCD5[1];_TCD7=_TCD6.l;_TCD8=_TCD7.first_line;_T187C->f2=Cyc_Position_loc_to_seg(_TCD8);_T187C->f3=0;_TCD3=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_TCD3;_TCD9=yyyvsp;_TCDA=& _TCD9[0].v;_TCDB=(union Cyc_YYSTYPE*)_TCDA;_TCDC=
Cyc_yyget_YY31(_TCDB);_T187B->tl=_TCDC.tms;_TCD1=(struct Cyc_List_List*)_T187B;}
# 2523
_T187A.tms=_TCD1;_TCD0=_T187A;}yyval=Cyc_YY31(_TCD0);goto _LL0;}case 328:{struct Cyc_Parse_Abstractdeclarator _T187A;_TCDF=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_TCDF,0U,sizeof(struct Cyc_List_List));_TCE1=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T187C=_region_malloc(_TCE1,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T187C->tag=5;_TCE2=yyyvsp;_TCE3=_TCE2[1];_TCE4=_TCE3.l;_TCE5=_TCE4.first_line;
# 2527 "parse.y"
_T187C->f1=Cyc_Position_loc_to_seg(_TCE5);_TCE6=yyyvsp;_TCE7=& _TCE6[1].v;_TCE8=(union Cyc_YYSTYPE*)_TCE7;_T187C->f2=Cyc_yyget_YY46(_TCE8);_TCE0=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T187C;}_T187B->hd=(void*)_TCE0;_TCE9=yyyvsp;_TCEA=& _TCE9[0].v;_TCEB=(union Cyc_YYSTYPE*)_TCEA;_TCEC=Cyc_yyget_YY31(_TCEB);_T187B->tl=_TCEC.tms;_TCDE=(struct Cyc_List_List*)_T187B;}_T187A.tms=_TCDE;_TCDD=_T187A;}yyval=Cyc_YY31(_TCDD);goto _LL0;case 329:{struct _tuple21 _T187A;_TCEE=yyyvsp;_TCEF=& _TCEE[2].v;_TCF0=(union Cyc_YYSTYPE*)_TCEF;
# 2533 "parse.y"
_T187A.f0=Cyc_yyget_Exp_tok(_TCF0);_T187A.f1=0;_T187A.f2=0;_T187A.f3=0;_TCED=_T187A;}yyval=Cyc_YY62(_TCED);goto _LL0;case 330:{struct _tuple21 _T187A;
# 2535 "parse.y"
_T187A.f0=0;_TCF2=yyyvsp;_TCF3=& _TCF2[2].v;_TCF4=(union Cyc_YYSTYPE*)_TCF3;_T187A.f1=Cyc_yyget_Exp_tok(_TCF4);_T187A.f2=0;_T187A.f3=0;_TCF1=_T187A;}yyval=Cyc_YY62(_TCF1);goto _LL0;case 331:{struct _tuple21 _T187A;
# 2537 "parse.y"
_T187A.f0=0;_T187A.f1=0;_TCF6=yyyvsp;_TCF7=& _TCF6[2].v;_TCF8=(union Cyc_YYSTYPE*)_TCF7;_T187A.f2=Cyc_yyget_Exp_tok(_TCF8);_T187A.f3=0;_TCF5=_T187A;}yyval=Cyc_YY62(_TCF5);goto _LL0;case 332:{struct _tuple21 _T187A;
# 2539 "parse.y"
_T187A.f0=0;_T187A.f1=0;_T187A.f2=0;_TCFA=yyyvsp;_TCFB=& _TCFA[2].v;_TCFC=(union Cyc_YYSTYPE*)_TCFB;_T187A.f3=Cyc_yyget_Exp_tok(_TCFC);_TCF9=_T187A;}yyval=Cyc_YY62(_TCF9);goto _LL0;case 333:{struct _tuple21 _T187A;_TCFE=yyyvsp;_TCFF=& _TCFE[2].v;_TD00=(union Cyc_YYSTYPE*)_TCFF;
# 2541 "parse.y"
_T187A.f0=Cyc_yyget_Exp_tok(_TD00);_T187A.f1=0;_T187A.f2=0;_T187A.f3=0;_TCFD=_T187A;}_TD01=yyyvsp;_TD02=& _TD01[4].v;_TD03=(union Cyc_YYSTYPE*)_TD02;_TD04=Cyc_yyget_YY62(_TD03);_TD05=Cyc_Parse_join_assns(_TCFD,_TD04);yyval=Cyc_YY62(_TD05);goto _LL0;case 334:{struct _tuple21 _T187A;
# 2543 "parse.y"
_T187A.f0=0;_TD07=yyyvsp;_TD08=& _TD07[2].v;_TD09=(union Cyc_YYSTYPE*)_TD08;_T187A.f1=Cyc_yyget_Exp_tok(_TD09);_T187A.f2=0;_T187A.f3=0;_TD06=_T187A;}_TD0A=yyyvsp;_TD0B=& _TD0A[4].v;_TD0C=(union Cyc_YYSTYPE*)_TD0B;_TD0D=Cyc_yyget_YY62(_TD0C);_TD0E=Cyc_Parse_join_assns(_TD06,_TD0D);yyval=Cyc_YY62(_TD0E);goto _LL0;case 335:{struct _tuple21 _T187A;
# 2545 "parse.y"
_T187A.f0=0;_T187A.f1=0;_TD10=yyyvsp;_TD11=& _TD10[2].v;_TD12=(union Cyc_YYSTYPE*)_TD11;_T187A.f2=Cyc_yyget_Exp_tok(_TD12);_T187A.f3=0;_TD0F=_T187A;}_TD13=yyyvsp;_TD14=& _TD13[4].v;_TD15=(union Cyc_YYSTYPE*)_TD14;_TD16=Cyc_yyget_YY62(_TD15);_TD17=Cyc_Parse_join_assns(_TD0F,_TD16);yyval=Cyc_YY62(_TD17);goto _LL0;case 336:{struct _tuple21 _T187A;
# 2547 "parse.y"
_T187A.f0=0;_T187A.f1=0;_T187A.f2=0;_TD19=yyyvsp;_TD1A=& _TD19[2].v;_TD1B=(union Cyc_YYSTYPE*)_TD1A;_T187A.f3=Cyc_yyget_Exp_tok(_TD1B);_TD18=_T187A;}_TD1C=yyyvsp;_TD1D=& _TD1C[4].v;_TD1E=(union Cyc_YYSTYPE*)_TD1D;_TD1F=Cyc_yyget_YY62(_TD1E);_TD20=Cyc_Parse_join_assns(_TD18,_TD1F);yyval=Cyc_YY62(_TD20);goto _LL0;case 337:{struct _tuple21 _T187A;
# 2551 "parse.y"
_T187A.f0=0;_T187A.f1=0;_T187A.f2=0;_T187A.f3=0;_TD21=_T187A;}yyval=Cyc_YY62(_TD21);goto _LL0;case 338: _TD22=yyyvsp;_TD23=_TD22[0];
# 2552 "parse.y"
yyval=_TD23.v;goto _LL0;case 339: _TD24=yyyvsp;_TD25=_TD24[0];
# 2557 "parse.y"
yyval=_TD25.v;goto _LL0;case 340: _TD26=yyyvsp;_TD27=_TD26[0];
# 2558 "parse.y"
yyval=_TD27.v;goto _LL0;case 341: _TD28=yyyvsp;_TD29=_TD28[0];
# 2559 "parse.y"
yyval=_TD29.v;goto _LL0;case 342: _TD2A=yyyvsp;_TD2B=_TD2A[0];
# 2560 "parse.y"
yyval=_TD2B.v;goto _LL0;case 343: _TD2C=yyyvsp;_TD2D=_TD2C[0];
# 2561 "parse.y"
yyval=_TD2D.v;goto _LL0;case 344: _TD2E=yyyvsp;_TD2F=_TD2E[0];
# 2562 "parse.y"
yyval=_TD2F.v;goto _LL0;case 345:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T187A->tag=13;{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_TD32=yyyvsp;_TD33=& _TD32[0].v;_TD34=(union Cyc_YYSTYPE*)_TD33;
# 2568 "parse.y"
*_T187B=Cyc_yyget_String_tok(_TD34);_TD31=(struct _fat_ptr*)_T187B;}_T187A->f1=_TD31;_TD35=yyyvsp;_TD36=& _TD35[2].v;_TD37=(union Cyc_YYSTYPE*)_TD36;_T187A->f2=Cyc_yyget_Stmt_tok(_TD37);_TD30=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T187A;}_TD38=(void*)_TD30;_TD39=yyyvsp;_TD3A=_TD39[0];_TD3B=_TD3A.l;_TD3C=_TD3B.first_line;_TD3D=Cyc_Position_loc_to_seg(_TD3C);_TD3E=Cyc_Absyn_new_stmt(_TD38,_TD3D);yyval=Cyc_Stmt_tok(_TD3E);goto _LL0;case 346: _TD3F=yyyvsp;_TD40=_TD3F[0];_TD41=_TD40.l;_TD42=_TD41.first_line;_TD43=
# 2572 "parse.y"
Cyc_Position_loc_to_seg(_TD42);_TD44=Cyc_Absyn_skip_stmt(_TD43);yyval=Cyc_Stmt_tok(_TD44);goto _LL0;case 347: _TD45=yyyvsp;_TD46=& _TD45[0].v;_TD47=(union Cyc_YYSTYPE*)_TD46;_TD48=
# 2573 "parse.y"
Cyc_yyget_Exp_tok(_TD47);_TD49=yyyvsp;_TD4A=_TD49[0];_TD4B=_TD4A.l;_TD4C=_TD4B.first_line;_TD4D=Cyc_Position_loc_to_seg(_TD4C);_TD4E=Cyc_Absyn_exp_stmt(_TD48,_TD4D);yyval=Cyc_Stmt_tok(_TD4E);goto _LL0;case 348:
# 2578 "parse.y"
 Cyc_Parse_inside_function_definition=1;goto _LL0;case 349:
# 2582 "parse.y"
 Cyc_Parse_inside_function_definition=0;goto _LL0;case 350: _TD4F=yyyvsp;_TD50=_TD4F[0];_TD51=_TD50.l;_TD52=_TD51.first_line;_TD53=
# 2586 "parse.y"
Cyc_Position_loc_to_seg(_TD52);_TD54=Cyc_Absyn_skip_stmt(_TD53);yyval=Cyc_Stmt_tok(_TD54);goto _LL0;case 351: _TD55=yyyvsp;_TD56=_TD55[1];
# 2587 "parse.y"
yyval=_TD56.v;goto _LL0;case 352: _TD57=yyyvsp;_TD58=_TD57[0];_TD59=_TD58.l;_TD5A=_TD59.first_line;_TD5B=
# 2592 "parse.y"
Cyc_Position_loc_to_seg(_TD5A);_TD5C=Cyc_Absyn_skip_stmt(_TD5B);yyval=Cyc_Stmt_tok(_TD5C);goto _LL0;case 353: _TD5D=yyyvsp;_TD5E=_TD5D[1];
# 2593 "parse.y"
yyval=_TD5E.v;goto _LL0;case 354: _TD5F=yyyvsp;_TD60=& _TD5F[0].v;_TD61=(union Cyc_YYSTYPE*)_TD60;_TD62=
# 2598 "parse.y"
Cyc_yyget_YY16(_TD61);_TD63=yyyvsp;_TD64=_TD63[0];_TD65=_TD64.l;_TD66=_TD65.first_line;_TD67=Cyc_Position_loc_to_seg(_TD66);_TD68=Cyc_Absyn_skip_stmt(_TD67);_TD69=Cyc_Parse_flatten_declarations(_TD62,_TD68);yyval=Cyc_Stmt_tok(_TD69);goto _LL0;case 355: _TD6A=yyyvsp;_TD6B=& _TD6A[0].v;_TD6C=(union Cyc_YYSTYPE*)_TD6B;_TD6D=
# 2599 "parse.y"
Cyc_yyget_YY16(_TD6C);_TD6E=yyyvsp;_TD6F=& _TD6E[1].v;_TD70=(union Cyc_YYSTYPE*)_TD6F;_TD71=Cyc_yyget_Stmt_tok(_TD70);_TD72=Cyc_Parse_flatten_declarations(_TD6D,_TD71);yyval=Cyc_Stmt_tok(_TD72);goto _LL0;case 356:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T187A->tag=13;{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_TD75=yyyvsp;_TD76=& _TD75[0].v;_TD77=(union Cyc_YYSTYPE*)_TD76;
# 2601 "parse.y"
*_T187B=Cyc_yyget_String_tok(_TD77);_TD74=(struct _fat_ptr*)_T187B;}_T187A->f1=_TD74;_TD78=yyyvsp;_TD79=& _TD78[2].v;_TD7A=(union Cyc_YYSTYPE*)_TD79;_TD7B=Cyc_yyget_YY16(_TD7A);_TD7C=Cyc_Absyn_skip_stmt(0U);_T187A->f2=Cyc_Parse_flatten_declarations(_TD7B,_TD7C);_TD73=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T187A;}_TD7D=(void*)_TD73;_TD7E=yyyvsp;_TD7F=_TD7E[0];_TD80=_TD7F.l;_TD81=_TD80.first_line;_TD82=Cyc_Position_loc_to_seg(_TD81);_TD83=Cyc_Absyn_new_stmt(_TD7D,_TD82);yyval=Cyc_Stmt_tok(_TD83);goto _LL0;case 357:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T187A->tag=13;{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_TD86=yyyvsp;_TD87=& _TD86[0].v;_TD88=(union Cyc_YYSTYPE*)_TD87;
# 2603 "parse.y"
*_T187B=Cyc_yyget_String_tok(_TD88);_TD85=(struct _fat_ptr*)_T187B;}_T187A->f1=_TD85;_TD89=yyyvsp;_TD8A=& _TD89[2].v;_TD8B=(union Cyc_YYSTYPE*)_TD8A;_TD8C=Cyc_yyget_YY16(_TD8B);_TD8D=yyyvsp;_TD8E=& _TD8D[3].v;_TD8F=(union Cyc_YYSTYPE*)_TD8E;_TD90=Cyc_yyget_Stmt_tok(_TD8F);_T187A->f2=Cyc_Parse_flatten_declarations(_TD8C,_TD90);_TD84=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T187A;}_TD91=(void*)_TD84;_TD92=yyyvsp;_TD93=_TD92[0];_TD94=_TD93.l;_TD95=_TD94.first_line;_TD96=Cyc_Position_loc_to_seg(_TD95);_TD97=Cyc_Absyn_new_stmt(_TD91,_TD96);yyval=Cyc_Stmt_tok(_TD97);goto _LL0;case 358: _TD98=yyyvsp;_TD99=_TD98[0];
# 2604 "parse.y"
yyval=_TD99.v;goto _LL0;case 359: _TD9A=yyyvsp;_TD9B=& _TD9A[0].v;_TD9C=(union Cyc_YYSTYPE*)_TD9B;_TD9D=
# 2605 "parse.y"
Cyc_yyget_Stmt_tok(_TD9C);_TD9E=yyyvsp;_TD9F=& _TD9E[1].v;_TDA0=(union Cyc_YYSTYPE*)_TD9F;_TDA1=Cyc_yyget_Stmt_tok(_TDA0);_TDA2=yyyvsp;_TDA3=_TDA2[0];_TDA4=_TDA3.l;_TDA5=_TDA4.first_line;_TDA6=Cyc_Position_loc_to_seg(_TDA5);_TDA7=Cyc_Absyn_seq_stmt(_TD9D,_TDA1,_TDA6);yyval=Cyc_Stmt_tok(_TDA7);goto _LL0;case 360:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T187A->tag=1;_TDA9=yyyvsp;_TDAA=& _TDA9[0].v;_TDAB=(union Cyc_YYSTYPE*)_TDAA;
# 2606 "parse.y"
_T187A->f1=Cyc_yyget_YY15(_TDAB);_TDA8=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T187A;}_TDAC=(void*)_TDA8;_TDAD=yyyvsp;_TDAE=_TDAD[0];_TDAF=_TDAE.l;_TDB0=_TDAF.first_line;_TDB1=Cyc_Position_loc_to_seg(_TDB0);_TDB2=Cyc_Absyn_new_decl(_TDAC,_TDB1);_TDB3=
Cyc_Absyn_skip_stmt(0U);_TDB4=
# 2606
Cyc_Parse_flatten_decl(_TDB2,_TDB3);yyval=Cyc_Stmt_tok(_TDB4);goto _LL0;case 361:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T187A->tag=1;_TDB6=yyyvsp;_TDB7=& _TDB6[0].v;_TDB8=(union Cyc_YYSTYPE*)_TDB7;
# 2609 "parse.y"
_T187A->f1=Cyc_yyget_YY15(_TDB8);_TDB5=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T187A;}_TDB9=(void*)_TDB5;_TDBA=yyyvsp;_TDBB=_TDBA[0];_TDBC=_TDBB.l;_TDBD=_TDBC.first_line;_TDBE=Cyc_Position_loc_to_seg(_TDBD);_TDBF=Cyc_Absyn_new_decl(_TDB9,_TDBE);_TDC0=yyyvsp;_TDC1=& _TDC0[1].v;_TDC2=(union Cyc_YYSTYPE*)_TDC1;_TDC3=Cyc_yyget_Stmt_tok(_TDC2);_TDC4=Cyc_Parse_flatten_decl(_TDBF,_TDC3);yyval=Cyc_Stmt_tok(_TDC4);goto _LL0;case 362: _TDC5=yyyvsp;_TDC6=& _TDC5[2].v;_TDC7=(union Cyc_YYSTYPE*)_TDC6;_TDC8=
# 2614 "parse.y"
Cyc_yyget_Exp_tok(_TDC7);_TDC9=yyyvsp;_TDCA=& _TDC9[4].v;_TDCB=(union Cyc_YYSTYPE*)_TDCA;_TDCC=Cyc_yyget_Stmt_tok(_TDCB);_TDCD=Cyc_Absyn_skip_stmt(0U);_TDCE=yyyvsp;_TDCF=_TDCE[0];_TDD0=_TDCF.l;_TDD1=_TDD0.first_line;_TDD2=Cyc_Position_loc_to_seg(_TDD1);_TDD3=Cyc_Absyn_ifthenelse_stmt(_TDC8,_TDCC,_TDCD,_TDD2);yyval=Cyc_Stmt_tok(_TDD3);goto _LL0;case 363: _TDD4=yyyvsp;_TDD5=& _TDD4[2].v;_TDD6=(union Cyc_YYSTYPE*)_TDD5;_TDD7=
# 2616 "parse.y"
Cyc_yyget_Exp_tok(_TDD6);_TDD8=yyyvsp;_TDD9=& _TDD8[4].v;_TDDA=(union Cyc_YYSTYPE*)_TDD9;_TDDB=Cyc_yyget_Stmt_tok(_TDDA);_TDDC=yyyvsp;_TDDD=& _TDDC[6].v;_TDDE=(union Cyc_YYSTYPE*)_TDDD;_TDDF=Cyc_yyget_Stmt_tok(_TDDE);_TDE0=yyyvsp;_TDE1=_TDE0[0];_TDE2=_TDE1.l;_TDE3=_TDE2.first_line;_TDE4=Cyc_Position_loc_to_seg(_TDE3);_TDE5=Cyc_Absyn_ifthenelse_stmt(_TDD7,_TDDB,_TDDF,_TDE4);yyval=Cyc_Stmt_tok(_TDE5);goto _LL0;case 364: _TDE6=yyyvsp;_TDE7=& _TDE6[2].v;_TDE8=(union Cyc_YYSTYPE*)_TDE7;_TDE9=
# 2622 "parse.y"
Cyc_yyget_Exp_tok(_TDE8);_TDEA=yyyvsp;_TDEB=& _TDEA[5].v;_TDEC=(union Cyc_YYSTYPE*)_TDEB;_TDED=Cyc_yyget_YY8(_TDEC);_TDEE=yyyvsp;_TDEF=_TDEE[0];_TDF0=_TDEF.l;_TDF1=_TDF0.first_line;_TDF2=Cyc_Position_loc_to_seg(_TDF1);_TDF3=Cyc_Absyn_switch_stmt(_TDE9,_TDED,_TDF2);yyval=Cyc_Stmt_tok(_TDF3);goto _LL0;case 365: _TDF4=yyyvsp;_TDF5=& _TDF4[1].v;_TDF6=(union Cyc_YYSTYPE*)_TDF5;_TDF7=
# 2625
Cyc_yyget_QualId_tok(_TDF6);_TDF8=yyyvsp;_TDF9=_TDF8[1];_TDFA=_TDF9.l;_TDFB=_TDFA.first_line;_TDFC=Cyc_Position_loc_to_seg(_TDFB);_TDFD=Cyc_Absyn_unknownid_exp(_TDF7,_TDFC);_TDFE=yyyvsp;_TDFF=& _TDFE[3].v;_TE00=(union Cyc_YYSTYPE*)_TDFF;_TE01=Cyc_yyget_YY8(_TE00);_TE02=yyyvsp;_TE03=_TE02[0];_TE04=_TE03.l;_TE05=_TE04.first_line;_TE06=Cyc_Position_loc_to_seg(_TE05);_TE07=Cyc_Absyn_switch_stmt(_TDFD,_TE01,_TE06);yyval=Cyc_Stmt_tok(_TE07);goto _LL0;case 366: _TE08=yyyvsp;_TE09=& _TE08[3].v;_TE0A=(union Cyc_YYSTYPE*)_TE09;_TE0B=
# 2628
Cyc_yyget_YY4(_TE0A);_TE0C=yyyvsp;_TE0D=_TE0C[1];_TE0E=_TE0D.l;_TE0F=_TE0E.first_line;_TE10=Cyc_Position_loc_to_seg(_TE0F);_TE11=Cyc_Absyn_tuple_exp(_TE0B,_TE10);_TE12=yyyvsp;_TE13=& _TE12[6].v;_TE14=(union Cyc_YYSTYPE*)_TE13;_TE15=Cyc_yyget_YY8(_TE14);_TE16=yyyvsp;_TE17=_TE16[0];_TE18=_TE17.l;_TE19=_TE18.first_line;_TE1A=Cyc_Position_loc_to_seg(_TE19);_TE1B=Cyc_Absyn_switch_stmt(_TE11,_TE15,_TE1A);yyval=Cyc_Stmt_tok(_TE1B);goto _LL0;case 367: _TE1C=yyyvsp;_TE1D=& _TE1C[1].v;_TE1E=(union Cyc_YYSTYPE*)_TE1D;_TE1F=
# 2632 "parse.y"
Cyc_yyget_Stmt_tok(_TE1E);_TE20=yyyvsp;_TE21=& _TE20[4].v;_TE22=(union Cyc_YYSTYPE*)_TE21;_TE23=Cyc_yyget_YY8(_TE22);_TE24=yyyvsp;_TE25=_TE24[0];_TE26=_TE25.l;_TE27=_TE26.first_line;_TE28=Cyc_Position_loc_to_seg(_TE27);_TE29=Cyc_Absyn_trycatch_stmt(_TE1F,_TE23,_TE28);yyval=Cyc_Stmt_tok(_TE29);goto _LL0;case 368:
# 2646 "parse.y"
 yyval=Cyc_YY8(0);goto _LL0;case 369:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE2C=& Cyc_Absyn_Wild_p_val;_TE2D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_TE2C;_TE2E=(void*)_TE2D;_TE2F=yyyvsp;_TE30=_TE2F[0];_TE31=_TE30.l;_TE32=_TE31.first_line;_TE33=
# 2649 "parse.y"
Cyc_Position_loc_to_seg(_TE32);_T187B->pattern=Cyc_Absyn_new_pat(_TE2E,_TE33);_T187B->pat_vars=0;
_T187B->where_clause=0;_TE34=yyyvsp;_TE35=& _TE34[2].v;_TE36=(union Cyc_YYSTYPE*)_TE35;_T187B->body=Cyc_yyget_Stmt_tok(_TE36);_TE37=yyyvsp;_TE38=_TE37[0];_TE39=_TE38.l;_TE3A=_TE39.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_TE3A);_TE2B=(struct Cyc_Absyn_Switch_clause*)_T187B;}
# 2649
_T187A->hd=_TE2B;_TE3B=yyyvsp;_TE3C=& _TE3B[3].v;_TE3D=(union Cyc_YYSTYPE*)_TE3C;
_T187A->tl=Cyc_yyget_YY8(_TE3D);_TE2A=(struct Cyc_List_List*)_T187A;}
# 2649
yyval=Cyc_YY8(_TE2A);goto _LL0;case 370:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE40=yyyvsp;_TE41=& _TE40[1].v;_TE42=(union Cyc_YYSTYPE*)_TE41;
# 2652 "parse.y"
_T187B->pattern=Cyc_yyget_YY9(_TE42);_T187B->pat_vars=0;_T187B->where_clause=0;_TE43=yyyvsp;_TE44=_TE43[2];_TE45=_TE44.l;_TE46=_TE45.first_line;_TE47=
Cyc_Position_loc_to_seg(_TE46);_T187B->body=Cyc_Absyn_fallthru_stmt(0,_TE47);_TE48=yyyvsp;_TE49=_TE48[0];_TE4A=_TE49.l;_TE4B=_TE4A.first_line;
_T187B->loc=Cyc_Position_loc_to_seg(_TE4B);_TE3F=(struct Cyc_Absyn_Switch_clause*)_T187B;}
# 2652
_T187A->hd=_TE3F;_TE4C=yyyvsp;_TE4D=& _TE4C[3].v;_TE4E=(union Cyc_YYSTYPE*)_TE4D;
# 2654
_T187A->tl=Cyc_yyget_YY8(_TE4E);_TE3E=(struct Cyc_List_List*)_T187A;}
# 2652
yyval=Cyc_YY8(_TE3E);goto _LL0;case 371:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE51=yyyvsp;_TE52=& _TE51[1].v;_TE53=(union Cyc_YYSTYPE*)_TE52;
# 2656 "parse.y"
_T187B->pattern=Cyc_yyget_YY9(_TE53);_T187B->pat_vars=0;_T187B->where_clause=0;_TE54=yyyvsp;_TE55=& _TE54[3].v;_TE56=(union Cyc_YYSTYPE*)_TE55;_T187B->body=Cyc_yyget_Stmt_tok(_TE56);_TE57=yyyvsp;_TE58=_TE57[0];_TE59=_TE58.l;_TE5A=_TE59.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_TE5A);_TE50=(struct Cyc_Absyn_Switch_clause*)_T187B;}_T187A->hd=_TE50;_TE5B=yyyvsp;_TE5C=& _TE5B[4].v;_TE5D=(union Cyc_YYSTYPE*)_TE5C;_T187A->tl=Cyc_yyget_YY8(_TE5D);_TE4F=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY8(_TE4F);goto _LL0;case 372:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE60=yyyvsp;_TE61=& _TE60[1].v;_TE62=(union Cyc_YYSTYPE*)_TE61;
# 2658 "parse.y"
_T187B->pattern=Cyc_yyget_YY9(_TE62);_T187B->pat_vars=0;_TE63=yyyvsp;_TE64=& _TE63[3].v;_TE65=(union Cyc_YYSTYPE*)_TE64;_T187B->where_clause=Cyc_yyget_Exp_tok(_TE65);_TE66=yyyvsp;_TE67=_TE66[4];_TE68=_TE67.l;_TE69=_TE68.first_line;_TE6A=
Cyc_Position_loc_to_seg(_TE69);_T187B->body=Cyc_Absyn_fallthru_stmt(0,_TE6A);_TE6B=yyyvsp;_TE6C=_TE6B[0];_TE6D=_TE6C.l;_TE6E=_TE6D.first_line;
_T187B->loc=Cyc_Position_loc_to_seg(_TE6E);_TE5F=(struct Cyc_Absyn_Switch_clause*)_T187B;}
# 2658
_T187A->hd=_TE5F;_TE6F=yyyvsp;_TE70=& _TE6F[5].v;_TE71=(union Cyc_YYSTYPE*)_TE70;
# 2660
_T187A->tl=Cyc_yyget_YY8(_TE71);_TE5E=(struct Cyc_List_List*)_T187A;}
# 2658
yyval=Cyc_YY8(_TE5E);goto _LL0;case 373:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE74=yyyvsp;_TE75=& _TE74[1].v;_TE76=(union Cyc_YYSTYPE*)_TE75;
# 2662 "parse.y"
_T187B->pattern=Cyc_yyget_YY9(_TE76);_T187B->pat_vars=0;_TE77=yyyvsp;_TE78=& _TE77[3].v;_TE79=(union Cyc_YYSTYPE*)_TE78;_T187B->where_clause=Cyc_yyget_Exp_tok(_TE79);_TE7A=yyyvsp;_TE7B=& _TE7A[5].v;_TE7C=(union Cyc_YYSTYPE*)_TE7B;_T187B->body=Cyc_yyget_Stmt_tok(_TE7C);_TE7D=yyyvsp;_TE7E=_TE7D[0];_TE7F=_TE7E.l;_TE80=_TE7F.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_TE80);_TE73=(struct Cyc_Absyn_Switch_clause*)_T187B;}_T187A->hd=_TE73;_TE81=yyyvsp;_TE82=& _TE81[6].v;_TE83=(union Cyc_YYSTYPE*)_TE82;_T187A->tl=Cyc_yyget_YY8(_TE83);_TE72=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY8(_TE72);goto _LL0;case 374: _TE84=yyyvsp;_TE85=& _TE84[2].v;_TE86=(union Cyc_YYSTYPE*)_TE85;_TE87=
# 2669 "parse.y"
Cyc_yyget_Exp_tok(_TE86);_TE88=yyyvsp;_TE89=& _TE88[4].v;_TE8A=(union Cyc_YYSTYPE*)_TE89;_TE8B=Cyc_yyget_Stmt_tok(_TE8A);_TE8C=yyyvsp;_TE8D=_TE8C[0];_TE8E=_TE8D.l;_TE8F=_TE8E.first_line;_TE90=Cyc_Position_loc_to_seg(_TE8F);_TE91=Cyc_Absyn_while_stmt(_TE87,_TE8B,_TE90);yyval=Cyc_Stmt_tok(_TE91);goto _LL0;case 375: _TE92=yyyvsp;_TE93=& _TE92[1].v;_TE94=(union Cyc_YYSTYPE*)_TE93;_TE95=
# 2671 "parse.y"
Cyc_yyget_Stmt_tok(_TE94);_TE96=yyyvsp;_TE97=& _TE96[4].v;_TE98=(union Cyc_YYSTYPE*)_TE97;_TE99=Cyc_yyget_Exp_tok(_TE98);_TE9A=yyyvsp;_TE9B=_TE9A[0];_TE9C=_TE9B.l;_TE9D=_TE9C.first_line;_TE9E=Cyc_Position_loc_to_seg(_TE9D);_TE9F=Cyc_Absyn_do_stmt(_TE95,_TE99,_TE9E);yyval=Cyc_Stmt_tok(_TE9F);goto _LL0;case 376: _TEA0=yyyvsp;_TEA1=& _TEA0[2].v;_TEA2=(union Cyc_YYSTYPE*)_TEA1;_TEA3=
# 2673 "parse.y"
Cyc_yyget_Exp_tok(_TEA2);_TEA4=yyyvsp;_TEA5=& _TEA4[4].v;_TEA6=(union Cyc_YYSTYPE*)_TEA5;_TEA7=Cyc_yyget_Exp_tok(_TEA6);_TEA8=yyyvsp;_TEA9=& _TEA8[6].v;_TEAA=(union Cyc_YYSTYPE*)_TEA9;_TEAB=Cyc_yyget_Exp_tok(_TEAA);_TEAC=yyyvsp;_TEAD=& _TEAC[8].v;_TEAE=(union Cyc_YYSTYPE*)_TEAD;_TEAF=Cyc_yyget_Stmt_tok(_TEAE);_TEB0=yyyvsp;_TEB1=_TEB0[0];_TEB2=_TEB1.l;_TEB3=_TEB2.first_line;_TEB4=Cyc_Position_loc_to_seg(_TEB3);_TEB5=Cyc_Absyn_for_stmt(_TEA3,_TEA7,_TEAB,_TEAF,_TEB4);yyval=Cyc_Stmt_tok(_TEB5);goto _LL0;case 377: _TEB6=
# 2675 "parse.y"
Cyc_Absyn_false_exp(0U);_TEB7=yyyvsp;_TEB8=& _TEB7[3].v;_TEB9=(union Cyc_YYSTYPE*)_TEB8;_TEBA=Cyc_yyget_Exp_tok(_TEB9);_TEBB=yyyvsp;_TEBC=& _TEBB[5].v;_TEBD=(union Cyc_YYSTYPE*)_TEBC;_TEBE=Cyc_yyget_Exp_tok(_TEBD);_TEBF=yyyvsp;_TEC0=& _TEBF[7].v;_TEC1=(union Cyc_YYSTYPE*)_TEC0;_TEC2=Cyc_yyget_Stmt_tok(_TEC1);_TEC3=yyyvsp;_TEC4=_TEC3[0];_TEC5=_TEC4.l;_TEC6=_TEC5.first_line;_TEC7=Cyc_Position_loc_to_seg(_TEC6);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_for_stmt(_TEB6,_TEBA,_TEBE,_TEC2,_TEC7);_TEC8=yyyvsp;_TEC9=& _TEC8[2].v;_TECA=(union Cyc_YYSTYPE*)_TEC9;_TECB=
Cyc_yyget_YY16(_TECA);_TECC=s;_TECD=Cyc_Parse_flatten_declarations(_TECB,_TECC);yyval=Cyc_Stmt_tok(_TECD);goto _LL0;}case 378: _TECE=
# 2679
Cyc_Absyn_true_exp(0U);yyval=Cyc_Exp_tok(_TECE);goto _LL0;case 379: _TECF=yyyvsp;_TED0=_TECF[0];
# 2680 "parse.y"
yyval=_TED0.v;goto _LL0;case 380:{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));_TED2=yyyvsp;_TED3=& _TED2[1].v;_TED4=(union Cyc_YYSTYPE*)_TED3;
# 2683
*_T187A=Cyc_yyget_String_tok(_TED4);_TED1=(struct _fat_ptr*)_T187A;}_TED5=yyyvsp;_TED6=_TED5[0];_TED7=_TED6.l;_TED8=_TED7.first_line;_TED9=Cyc_Position_loc_to_seg(_TED8);_TEDA=Cyc_Absyn_goto_stmt(_TED1,_TED9);yyval=Cyc_Stmt_tok(_TEDA);goto _LL0;case 381: _TEDB=yyyvsp;_TEDC=_TEDB[0];_TEDD=_TEDC.l;_TEDE=_TEDD.first_line;_TEDF=
# 2684 "parse.y"
Cyc_Position_loc_to_seg(_TEDE);_TEE0=Cyc_Absyn_continue_stmt(_TEDF);yyval=Cyc_Stmt_tok(_TEE0);goto _LL0;case 382: _TEE1=yyyvsp;_TEE2=_TEE1[0];_TEE3=_TEE2.l;_TEE4=_TEE3.first_line;_TEE5=
# 2685 "parse.y"
Cyc_Position_loc_to_seg(_TEE4);_TEE6=Cyc_Absyn_break_stmt(_TEE5);yyval=Cyc_Stmt_tok(_TEE6);goto _LL0;case 383: _TEE7=yyyvsp;_TEE8=_TEE7[0];_TEE9=_TEE8.l;_TEEA=_TEE9.first_line;_TEEB=
# 2686 "parse.y"
Cyc_Position_loc_to_seg(_TEEA);_TEEC=Cyc_Absyn_return_stmt(0,_TEEB);yyval=Cyc_Stmt_tok(_TEEC);goto _LL0;case 384: _TEED=yyyvsp;_TEEE=& _TEED[1].v;_TEEF=(union Cyc_YYSTYPE*)_TEEE;_TEF0=
# 2687 "parse.y"
Cyc_yyget_Exp_tok(_TEEF);_TEF1=yyyvsp;_TEF2=_TEF1[0];_TEF3=_TEF2.l;_TEF4=_TEF3.first_line;_TEF5=Cyc_Position_loc_to_seg(_TEF4);_TEF6=Cyc_Absyn_return_stmt(_TEF0,_TEF5);yyval=Cyc_Stmt_tok(_TEF6);goto _LL0;case 385: _TEF7=yyyvsp;_TEF8=_TEF7[0];_TEF9=_TEF8.l;_TEFA=_TEF9.first_line;_TEFB=
# 2689 "parse.y"
Cyc_Position_loc_to_seg(_TEFA);_TEFC=Cyc_Absyn_fallthru_stmt(0,_TEFB);yyval=Cyc_Stmt_tok(_TEFC);goto _LL0;case 386: _TEFD=yyyvsp;_TEFE=_TEFD[0];_TEFF=_TEFE.l;_TF00=_TEFF.first_line;_TF01=
# 2690 "parse.y"
Cyc_Position_loc_to_seg(_TF00);_TF02=Cyc_Absyn_fallthru_stmt(0,_TF01);yyval=Cyc_Stmt_tok(_TF02);goto _LL0;case 387: _TF03=yyyvsp;_TF04=& _TF03[2].v;_TF05=(union Cyc_YYSTYPE*)_TF04;_TF06=
# 2692 "parse.y"
Cyc_yyget_YY4(_TF05);_TF07=yyyvsp;_TF08=_TF07[0];_TF09=_TF08.l;_TF0A=_TF09.first_line;_TF0B=Cyc_Position_loc_to_seg(_TF0A);_TF0C=Cyc_Absyn_fallthru_stmt(_TF06,_TF0B);yyval=Cyc_Stmt_tok(_TF0C);goto _LL0;case 388: _TF0D=yyyvsp;_TF0E=_TF0D[0];
# 2701 "parse.y"
yyval=_TF0E.v;goto _LL0;case 389: _TF0F=yyyvsp;_TF10=_TF0F[0];
# 2704
yyval=_TF10.v;goto _LL0;case 390: _TF11=yyyvsp;_TF12=& _TF11[0].v;_TF13=(union Cyc_YYSTYPE*)_TF12;_TF14=
# 2706 "parse.y"
Cyc_yyget_YY9(_TF13);_TF15=Cyc_Parse_pat2exp(_TF14);_TF16=yyyvsp;_TF17=& _TF16[2].v;_TF18=(union Cyc_YYSTYPE*)_TF17;_TF19=Cyc_yyget_Exp_tok(_TF18);_TF1A=yyyvsp;_TF1B=& _TF1A[4].v;_TF1C=(union Cyc_YYSTYPE*)_TF1B;_TF1D=Cyc_yyget_Exp_tok(_TF1C);_TF1E=yyyvsp;_TF1F=_TF1E[0];_TF20=_TF1F.l;_TF21=_TF20.first_line;_TF22=Cyc_Position_loc_to_seg(_TF21);_TF23=Cyc_Absyn_conditional_exp(_TF15,_TF19,_TF1D,_TF22);_TF24=Cyc_Absyn_exp_pat(_TF23);yyval=Cyc_YY9(_TF24);goto _LL0;case 391: _TF25=yyyvsp;_TF26=_TF25[0];
# 2709
yyval=_TF26.v;goto _LL0;case 392: _TF27=yyyvsp;_TF28=& _TF27[0].v;_TF29=(union Cyc_YYSTYPE*)_TF28;_TF2A=
# 2711 "parse.y"
Cyc_yyget_YY9(_TF29);_TF2B=Cyc_Parse_pat2exp(_TF2A);_TF2C=yyyvsp;_TF2D=& _TF2C[2].v;_TF2E=(union Cyc_YYSTYPE*)_TF2D;_TF2F=Cyc_yyget_Exp_tok(_TF2E);_TF30=yyyvsp;_TF31=_TF30[0];_TF32=_TF31.l;_TF33=_TF32.first_line;_TF34=Cyc_Position_loc_to_seg(_TF33);_TF35=Cyc_Absyn_or_exp(_TF2B,_TF2F,_TF34);_TF36=Cyc_Absyn_exp_pat(_TF35);yyval=Cyc_YY9(_TF36);goto _LL0;case 393: _TF37=yyyvsp;_TF38=_TF37[0];
# 2714
yyval=_TF38.v;goto _LL0;case 394: _TF39=yyyvsp;_TF3A=& _TF39[0].v;_TF3B=(union Cyc_YYSTYPE*)_TF3A;_TF3C=
# 2716 "parse.y"
Cyc_yyget_YY9(_TF3B);_TF3D=Cyc_Parse_pat2exp(_TF3C);_TF3E=yyyvsp;_TF3F=& _TF3E[2].v;_TF40=(union Cyc_YYSTYPE*)_TF3F;_TF41=Cyc_yyget_Exp_tok(_TF40);_TF42=yyyvsp;_TF43=_TF42[0];_TF44=_TF43.l;_TF45=_TF44.first_line;_TF46=Cyc_Position_loc_to_seg(_TF45);_TF47=Cyc_Absyn_and_exp(_TF3D,_TF41,_TF46);_TF48=Cyc_Absyn_exp_pat(_TF47);yyval=Cyc_YY9(_TF48);goto _LL0;case 395: _TF49=yyyvsp;_TF4A=_TF49[0];
# 2719
yyval=_TF4A.v;goto _LL0;case 396: _TF4B=yyyvsp;_TF4C=& _TF4B[0].v;_TF4D=(union Cyc_YYSTYPE*)_TF4C;_TF4E=
# 2721 "parse.y"
Cyc_yyget_YY9(_TF4D);_TF4F=Cyc_Parse_pat2exp(_TF4E);_TF50=yyyvsp;_TF51=& _TF50[2].v;_TF52=(union Cyc_YYSTYPE*)_TF51;_TF53=Cyc_yyget_Exp_tok(_TF52);_TF54=yyyvsp;_TF55=_TF54[0];_TF56=_TF55.l;_TF57=_TF56.first_line;_TF58=Cyc_Position_loc_to_seg(_TF57);_TF59=Cyc_Absyn_prim2_exp(14U,_TF4F,_TF53,_TF58);_TF5A=Cyc_Absyn_exp_pat(_TF59);yyval=Cyc_YY9(_TF5A);goto _LL0;case 397: _TF5B=yyyvsp;_TF5C=_TF5B[0];
# 2724
yyval=_TF5C.v;goto _LL0;case 398: _TF5D=yyyvsp;_TF5E=& _TF5D[0].v;_TF5F=(union Cyc_YYSTYPE*)_TF5E;_TF60=
# 2726 "parse.y"
Cyc_yyget_YY9(_TF5F);_TF61=Cyc_Parse_pat2exp(_TF60);_TF62=yyyvsp;_TF63=& _TF62[2].v;_TF64=(union Cyc_YYSTYPE*)_TF63;_TF65=Cyc_yyget_Exp_tok(_TF64);_TF66=yyyvsp;_TF67=_TF66[0];_TF68=_TF67.l;_TF69=_TF68.first_line;_TF6A=Cyc_Position_loc_to_seg(_TF69);_TF6B=Cyc_Absyn_prim2_exp(15U,_TF61,_TF65,_TF6A);_TF6C=Cyc_Absyn_exp_pat(_TF6B);yyval=Cyc_YY9(_TF6C);goto _LL0;case 399: _TF6D=yyyvsp;_TF6E=_TF6D[0];
# 2729
yyval=_TF6E.v;goto _LL0;case 400: _TF6F=yyyvsp;_TF70=& _TF6F[0].v;_TF71=(union Cyc_YYSTYPE*)_TF70;_TF72=
# 2731 "parse.y"
Cyc_yyget_YY9(_TF71);_TF73=Cyc_Parse_pat2exp(_TF72);_TF74=yyyvsp;_TF75=& _TF74[2].v;_TF76=(union Cyc_YYSTYPE*)_TF75;_TF77=Cyc_yyget_Exp_tok(_TF76);_TF78=yyyvsp;_TF79=_TF78[0];_TF7A=_TF79.l;_TF7B=_TF7A.first_line;_TF7C=Cyc_Position_loc_to_seg(_TF7B);_TF7D=Cyc_Absyn_prim2_exp(13U,_TF73,_TF77,_TF7C);_TF7E=Cyc_Absyn_exp_pat(_TF7D);yyval=Cyc_YY9(_TF7E);goto _LL0;case 401: _TF7F=yyyvsp;_TF80=_TF7F[0];
# 2734
yyval=_TF80.v;goto _LL0;case 402: _TF81=yyyvsp;_TF82=& _TF81[1].v;_TF83=(union Cyc_YYSTYPE*)_TF82;_TF84=
# 2736 "parse.y"
Cyc_yyget_YY69(_TF83);_TF85=yyyvsp;_TF86=& _TF85[0].v;_TF87=(union Cyc_YYSTYPE*)_TF86;_TF88=Cyc_yyget_YY9(_TF87);_TF89=Cyc_Parse_pat2exp(_TF88);_TF8A=yyyvsp;_TF8B=& _TF8A[2].v;_TF8C=(union Cyc_YYSTYPE*)_TF8B;_TF8D=Cyc_yyget_Exp_tok(_TF8C);_TF8E=yyyvsp;_TF8F=_TF8E[0];_TF90=_TF8F.l;_TF91=_TF90.first_line;_TF92=Cyc_Position_loc_to_seg(_TF91);_TF93=_TF84(_TF89,_TF8D,_TF92);_TF94=Cyc_Absyn_exp_pat(_TF93);yyval=Cyc_YY9(_TF94);goto _LL0;case 403: _TF95=yyyvsp;_TF96=_TF95[0];
# 2739
yyval=_TF96.v;goto _LL0;case 404: _TF97=yyyvsp;_TF98=& _TF97[1].v;_TF99=(union Cyc_YYSTYPE*)_TF98;_TF9A=
# 2741 "parse.y"
Cyc_yyget_YY69(_TF99);_TF9B=yyyvsp;_TF9C=& _TF9B[0].v;_TF9D=(union Cyc_YYSTYPE*)_TF9C;_TF9E=Cyc_yyget_YY9(_TF9D);_TF9F=Cyc_Parse_pat2exp(_TF9E);_TFA0=yyyvsp;_TFA1=& _TFA0[2].v;_TFA2=(union Cyc_YYSTYPE*)_TFA1;_TFA3=Cyc_yyget_Exp_tok(_TFA2);_TFA4=yyyvsp;_TFA5=_TFA4[0];_TFA6=_TFA5.l;_TFA7=_TFA6.first_line;_TFA8=Cyc_Position_loc_to_seg(_TFA7);_TFA9=_TF9A(_TF9F,_TFA3,_TFA8);_TFAA=Cyc_Absyn_exp_pat(_TFA9);yyval=Cyc_YY9(_TFAA);goto _LL0;case 405: _TFAB=yyyvsp;_TFAC=_TFAB[0];
# 2744
yyval=_TFAC.v;goto _LL0;case 406: _TFAD=yyyvsp;_TFAE=& _TFAD[0].v;_TFAF=(union Cyc_YYSTYPE*)_TFAE;_TFB0=
# 2746 "parse.y"
Cyc_yyget_YY9(_TFAF);_TFB1=Cyc_Parse_pat2exp(_TFB0);_TFB2=yyyvsp;_TFB3=& _TFB2[2].v;_TFB4=(union Cyc_YYSTYPE*)_TFB3;_TFB5=Cyc_yyget_Exp_tok(_TFB4);_TFB6=yyyvsp;_TFB7=_TFB6[0];_TFB8=_TFB7.l;_TFB9=_TFB8.first_line;_TFBA=Cyc_Position_loc_to_seg(_TFB9);_TFBB=Cyc_Absyn_prim2_exp(16U,_TFB1,_TFB5,_TFBA);_TFBC=Cyc_Absyn_exp_pat(_TFBB);yyval=Cyc_YY9(_TFBC);goto _LL0;case 407: _TFBD=yyyvsp;_TFBE=& _TFBD[0].v;_TFBF=(union Cyc_YYSTYPE*)_TFBE;_TFC0=
# 2748 "parse.y"
Cyc_yyget_YY9(_TFBF);_TFC1=Cyc_Parse_pat2exp(_TFC0);_TFC2=yyyvsp;_TFC3=& _TFC2[2].v;_TFC4=(union Cyc_YYSTYPE*)_TFC3;_TFC5=Cyc_yyget_Exp_tok(_TFC4);_TFC6=yyyvsp;_TFC7=_TFC6[0];_TFC8=_TFC7.l;_TFC9=_TFC8.first_line;_TFCA=Cyc_Position_loc_to_seg(_TFC9);_TFCB=Cyc_Absyn_prim2_exp(17U,_TFC1,_TFC5,_TFCA);_TFCC=Cyc_Absyn_exp_pat(_TFCB);yyval=Cyc_YY9(_TFCC);goto _LL0;case 408: _TFCD=yyyvsp;_TFCE=_TFCD[0];
# 2751
yyval=_TFCE.v;goto _LL0;case 409: _TFCF=yyyvsp;_TFD0=& _TFCF[1].v;_TFD1=(union Cyc_YYSTYPE*)_TFD0;_TFD2=
# 2753 "parse.y"
Cyc_yyget_YY6(_TFD1);_TFD3=yyyvsp;_TFD4=& _TFD3[0].v;_TFD5=(union Cyc_YYSTYPE*)_TFD4;_TFD6=Cyc_yyget_YY9(_TFD5);_TFD7=Cyc_Parse_pat2exp(_TFD6);_TFD8=yyyvsp;_TFD9=& _TFD8[2].v;_TFDA=(union Cyc_YYSTYPE*)_TFD9;_TFDB=Cyc_yyget_Exp_tok(_TFDA);_TFDC=yyyvsp;_TFDD=_TFDC[0];_TFDE=_TFDD.l;_TFDF=_TFDE.first_line;_TFE0=Cyc_Position_loc_to_seg(_TFDF);_TFE1=Cyc_Absyn_prim2_exp(_TFD2,_TFD7,_TFDB,_TFE0);_TFE2=Cyc_Absyn_exp_pat(_TFE1);yyval=Cyc_YY9(_TFE2);goto _LL0;case 410: _TFE3=yyyvsp;_TFE4=_TFE3[0];
# 2756
yyval=_TFE4.v;goto _LL0;case 411: _TFE5=yyyvsp;_TFE6=& _TFE5[1].v;_TFE7=(union Cyc_YYSTYPE*)_TFE6;_TFE8=
# 2758 "parse.y"
Cyc_yyget_YY6(_TFE7);_TFE9=yyyvsp;_TFEA=& _TFE9[0].v;_TFEB=(union Cyc_YYSTYPE*)_TFEA;_TFEC=Cyc_yyget_YY9(_TFEB);_TFED=Cyc_Parse_pat2exp(_TFEC);_TFEE=yyyvsp;_TFEF=& _TFEE[2].v;_TFF0=(union Cyc_YYSTYPE*)_TFEF;_TFF1=Cyc_yyget_Exp_tok(_TFF0);_TFF2=yyyvsp;_TFF3=_TFF2[0];_TFF4=_TFF3.l;_TFF5=_TFF4.first_line;_TFF6=Cyc_Position_loc_to_seg(_TFF5);_TFF7=Cyc_Absyn_prim2_exp(_TFE8,_TFED,_TFF1,_TFF6);_TFF8=Cyc_Absyn_exp_pat(_TFF7);yyval=Cyc_YY9(_TFF8);goto _LL0;case 412: _TFF9=yyyvsp;_TFFA=_TFF9[0];
# 2761
yyval=_TFFA.v;goto _LL0;case 413: _TFFB=yyyvsp;_TFFC=& _TFFB[1].v;_TFFD=(union Cyc_YYSTYPE*)_TFFC;_TFFE=
# 2763 "parse.y"
Cyc_yyget_YY38(_TFFD);_TFFF=yyyvsp;_T1000=_TFFF[1];_T1001=_T1000.l;_T1002=_T1001.first_line;_T1003=Cyc_Position_loc_to_seg(_T1002);{void*t=Cyc_Parse_type_name_to_type(_TFFE,_T1003);_T1004=t;_T1005=yyyvsp;_T1006=& _T1005[3].v;_T1007=(union Cyc_YYSTYPE*)_T1006;_T1008=
Cyc_yyget_Exp_tok(_T1007);_T1009=yyyvsp;_T100A=_T1009[0];_T100B=_T100A.l;_T100C=_T100B.first_line;_T100D=Cyc_Position_loc_to_seg(_T100C);_T100E=Cyc_Absyn_cast_exp(_T1004,_T1008,1,0U,_T100D);_T100F=Cyc_Absyn_exp_pat(_T100E);yyval=Cyc_YY9(_T100F);goto _LL0;}case 414: _T1010=yyyvsp;_T1011=_T1010[0];
# 2769 "parse.y"
yyval=_T1011.v;goto _LL0;case 415: _T1012=yyyvsp;_T1013=& _T1012[0].v;_T1014=(union Cyc_YYSTYPE*)_T1013;_T1015=
# 2773 "parse.y"
Cyc_yyget_YY6(_T1014);_T1016=yyyvsp;_T1017=& _T1016[1].v;_T1018=(union Cyc_YYSTYPE*)_T1017;_T1019=Cyc_yyget_Exp_tok(_T1018);_T101A=yyyvsp;_T101B=_T101A[0];_T101C=_T101B.l;_T101D=_T101C.first_line;_T101E=Cyc_Position_loc_to_seg(_T101D);_T101F=Cyc_Absyn_prim1_exp(_T1015,_T1019,_T101E);_T1020=Cyc_Absyn_exp_pat(_T101F);yyval=Cyc_YY9(_T1020);goto _LL0;case 416: _T1021=yyyvsp;_T1022=& _T1021[2].v;_T1023=(union Cyc_YYSTYPE*)_T1022;_T1024=
# 2775 "parse.y"
Cyc_yyget_YY38(_T1023);_T1025=yyyvsp;_T1026=_T1025[2];_T1027=_T1026.l;_T1028=_T1027.first_line;_T1029=Cyc_Position_loc_to_seg(_T1028);{void*t=Cyc_Parse_type_name_to_type(_T1024,_T1029);_T102A=t;_T102B=yyyvsp;_T102C=_T102B[0];_T102D=_T102C.l;_T102E=_T102D.first_line;_T102F=
Cyc_Position_loc_to_seg(_T102E);_T1030=Cyc_Absyn_sizeoftype_exp(_T102A,_T102F);_T1031=Cyc_Absyn_exp_pat(_T1030);yyval=Cyc_YY9(_T1031);goto _LL0;}case 417: _T1032=yyyvsp;_T1033=& _T1032[1].v;_T1034=(union Cyc_YYSTYPE*)_T1033;_T1035=
# 2778 "parse.y"
Cyc_yyget_Exp_tok(_T1034);_T1036=yyyvsp;_T1037=_T1036[0];_T1038=_T1037.l;_T1039=_T1038.first_line;_T103A=Cyc_Position_loc_to_seg(_T1039);_T103B=Cyc_Absyn_sizeofexp_exp(_T1035,_T103A);_T103C=Cyc_Absyn_exp_pat(_T103B);yyval=Cyc_YY9(_T103C);goto _LL0;case 418: _T103D=yyyvsp;_T103E=& _T103D[2].v;_T103F=(union Cyc_YYSTYPE*)_T103E;_T1040=
# 2780 "parse.y"
Cyc_yyget_YY38(_T103F);_T1041=*_T1040;_T1042=_T1041.f2;_T1043=yyyvsp;_T1044=& _T1043[4].v;_T1045=(union Cyc_YYSTYPE*)_T1044;_T1046=Cyc_yyget_YY3(_T1045);_T1047=Cyc_List_imp_rev(_T1046);_T1048=yyyvsp;_T1049=_T1048[0];_T104A=_T1049.l;_T104B=_T104A.first_line;_T104C=Cyc_Position_loc_to_seg(_T104B);_T104D=Cyc_Absyn_offsetof_exp(_T1042,_T1047,_T104C);_T104E=Cyc_Absyn_exp_pat(_T104D);yyval=Cyc_YY9(_T104E);goto _LL0;case 419: _T104F=yyyvsp;_T1050=_T104F[0];
# 2785 "parse.y"
yyval=_T1050.v;goto _LL0;case 420: _T1051=yyyvsp;_T1052=_T1051[0];
# 2793 "parse.y"
yyval=_T1052.v;goto _LL0;case 421: _T1053=& Cyc_Absyn_Wild_p_val;_T1054=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T1053;_T1055=(void*)_T1054;_T1056=yyyvsp;_T1057=_T1056[0];_T1058=_T1057.l;_T1059=_T1058.first_line;_T105A=
# 2797 "parse.y"
Cyc_Position_loc_to_seg(_T1059);_T105B=Cyc_Absyn_new_pat(_T1055,_T105A);yyval=Cyc_YY9(_T105B);goto _LL0;case 422: _T105C=yyyvsp;_T105D=& _T105C[1].v;_T105E=(union Cyc_YYSTYPE*)_T105D;_T105F=
# 2798 "parse.y"
Cyc_yyget_Exp_tok(_T105E);_T1060=Cyc_Absyn_exp_pat(_T105F);yyval=Cyc_YY9(_T1060);goto _LL0;case 423:{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_T187A->tag=14;_T1062=yyyvsp;_T1063=& _T1062[0].v;_T1064=(union Cyc_YYSTYPE*)_T1063;
# 2799 "parse.y"
_T187A->f1=Cyc_yyget_QualId_tok(_T1064);_T1061=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T187A;}_T1065=(void*)_T1061;_T1066=yyyvsp;_T1067=_T1066[0];_T1068=_T1067.l;_T1069=_T1068.first_line;_T106A=Cyc_Position_loc_to_seg(_T1069);_T106B=Cyc_Absyn_new_pat(_T1065,_T106A);yyval=Cyc_YY9(_T106B);goto _LL0;case 424:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T187A->tag=5;_T106D=yyyvsp;_T106E=& _T106D[1].v;_T106F=(union Cyc_YYSTYPE*)_T106E;
# 2800 "parse.y"
_T187A->f1=Cyc_yyget_YY9(_T106F);_T106C=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T187A;}_T1070=(void*)_T106C;_T1071=yyyvsp;_T1072=_T1071[0];_T1073=_T1072.l;_T1074=_T1073.first_line;_T1075=Cyc_Position_loc_to_seg(_T1074);_T1076=Cyc_Absyn_new_pat(_T1070,_T1075);yyval=Cyc_YY9(_T1076);goto _LL0;case 425: _T1077=yyyvsp;_T1078=& _T1077[0].v;_T1079=(union Cyc_YYSTYPE*)_T1078;{
# 2802 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(_T1079);_T107A=e;{
void*_T187A=_T107A->r;struct _fat_ptr _T187B;int _T187C;short _T187D;char _T187E;enum Cyc_Absyn_Sign _T187F;_T107B=(int*)_T187A;_T107C=*_T107B;if(_T107C!=0)goto _TL2E0;_T107D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T187A;_T107E=_T107D->f1;_T107F=_T107E.LongLong_c;_T1080=_T107F.tag;switch(_T1080){case 2:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1880=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T187A;_T1081=_T1880->f1;_T1082=_T1081.Char_c;_T1083=_T1082.val;_T187F=_T1083.f0;_T1084=_T1880->f1;_T1085=_T1084.Char_c;_T1086=_T1085.val;_T187E=_T1086.f1;}{enum Cyc_Absyn_Sign s=_T187F;char i=_T187E;{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T1880=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_T1880->tag=10;
# 2805
_T1880->f1=i;_T1087=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T1880;}_T1088=(void*)_T1087;_T1089=e;_T108A=_T1089->loc;_T108B=Cyc_Absyn_new_pat(_T1088,_T108A);yyval=Cyc_YY9(_T108B);goto _LL537;}case 4:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1880=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T187A;_T108C=_T1880->f1;_T108D=_T108C.Short_c;_T108E=_T108D.val;_T187F=_T108E.f0;_T108F=_T1880->f1;_T1090=_T108F.Short_c;_T1091=_T1090.val;_T187D=_T1091.f1;}{enum Cyc_Absyn_Sign s=_T187F;short i=_T187D;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T1880=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T1880->tag=9;
# 2807
_T1880->f1=s;_T1093=i;_T1880->f2=(int)_T1093;_T1092=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T1880;}_T1094=(void*)_T1092;_T1095=e;_T1096=_T1095->loc;_T1097=Cyc_Absyn_new_pat(_T1094,_T1096);yyval=Cyc_YY9(_T1097);goto _LL537;}case 5:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1880=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T187A;_T1098=_T1880->f1;_T1099=_T1098.Int_c;_T109A=_T1099.val;_T187F=_T109A.f0;_T109B=_T1880->f1;_T109C=_T109B.Int_c;_T109D=_T109C.val;_T187C=_T109D.f1;}{enum Cyc_Absyn_Sign s=_T187F;int i=_T187C;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T1880=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T1880->tag=9;
# 2809
_T1880->f1=s;_T1880->f2=i;_T109E=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T1880;}_T109F=(void*)_T109E;_T10A0=e;_T10A1=_T10A0->loc;_T10A2=Cyc_Absyn_new_pat(_T109F,_T10A1);yyval=Cyc_YY9(_T10A2);goto _LL537;}case 7:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1880=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T187A;_T10A3=_T1880->f1;_T10A4=_T10A3.Float_c;_T10A5=_T10A4.val;_T187B=_T10A5.f0;_T10A6=_T1880->f1;_T10A7=_T10A6.Float_c;_T10A8=_T10A7.val;_T187C=_T10A8.f1;}{struct _fat_ptr s=_T187B;int i=_T187C;{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T1880=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_T1880->tag=11;
# 2811
_T1880->f1=s;_T1880->f2=i;_T10A9=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T1880;}_T10AA=(void*)_T10A9;_T10AB=e;_T10AC=_T10AB->loc;_T10AD=Cyc_Absyn_new_pat(_T10AA,_T10AC);yyval=Cyc_YY9(_T10AD);goto _LL537;}case 1: _T10AE=& Cyc_Absyn_Null_p_val;_T10AF=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_T10AE;_T10B0=(void*)_T10AF;_T10B1=e;_T10B2=_T10B1->loc;_T10B3=
# 2813
Cyc_Absyn_new_pat(_T10B0,_T10B2);yyval=Cyc_YY9(_T10B3);goto _LL537;case 8: goto _LL545;case 9: _LL545: _T10B4=yyyvsp;_T10B5=_T10B4[0];_T10B6=_T10B5.l;_T10B7=_T10B6.first_line;_T10B8=
# 2816
Cyc_Position_loc_to_seg(_T10B7);_T10B9=_tag_fat("strings cannot occur within patterns",sizeof(char),37U);_T10BA=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10B8,_T10B9,_T10BA);goto _LL537;case 6: _T10BB=yyyvsp;_T10BC=_T10BB[0];_T10BD=_T10BC.l;_T10BE=_T10BD.first_line;_T10BF=
# 2818
Cyc_Position_loc_to_seg(_T10BE);_T10C0=_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U);_T10C1=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10BF,_T10C0,_T10C1);goto _LL537;default: goto _LL548;}goto _TL2E1;_TL2E0: _LL548: _T10C2=yyyvsp;_T10C3=_T10C2[0];_T10C4=_T10C3.l;_T10C5=_T10C4.first_line;_T10C6=
# 2820
Cyc_Position_loc_to_seg(_T10C5);_T10C7=_tag_fat("bad constant in case",sizeof(char),21U);_T10C8=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10C6,_T10C7,_T10C8);_TL2E1: _LL537:;}goto _LL0;}case 426: _T10C9=yyyvsp;_T10CA=& _T10C9[1].v;_T10CB=(union Cyc_YYSTYPE*)_T10CA;_T10CC=
# 2824 "parse.y"
Cyc_yyget_String_tok(_T10CB);_T10CD=_tag_fat("as",sizeof(char),3U);_T10CE=Cyc_strcmp(_T10CC,_T10CD);if(_T10CE==0)goto _TL2E3;_T10CF=yyyvsp;_T10D0=_T10CF[1];_T10D1=_T10D0.l;_T10D2=_T10D1.first_line;_T10D3=
Cyc_Position_loc_to_seg(_T10D2);_T10D4=_tag_fat("expecting `as'",sizeof(char),15U);_T10D5=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10D3,_T10D4,_T10D5);goto _TL2E4;_TL2E3: _TL2E4:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T187A->tag=1;_T10D7=yyyvsp;_T10D8=_T10D7[0];_T10D9=_T10D8.l;_T10DA=_T10D9.first_line;_T10DB=
Cyc_Position_loc_to_seg(_T10DA);{struct _tuple0*_T187B=_cycalloc(sizeof(struct _tuple0));_T187B->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));_T10DE=yyyvsp;_T10DF=& _T10DE[0].v;_T10E0=(union Cyc_YYSTYPE*)_T10DF;*_T187C=Cyc_yyget_String_tok(_T10E0);_T10DD=(struct _fat_ptr*)_T187C;}_T187B->f1=_T10DD;_T10DC=(struct _tuple0*)_T187B;}_T10E1=Cyc_Absyn_void_type;_T187A->f1=Cyc_Absyn_new_vardecl(_T10DB,_T10DC,_T10E1,0,0);_T10E2=yyyvsp;_T10E3=& _T10E2[2].v;_T10E4=(union Cyc_YYSTYPE*)_T10E3;
_T187A->f2=Cyc_yyget_YY9(_T10E4);_T10D6=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T187A;}_T10E5=(void*)_T10D6;_T10E6=yyyvsp;_T10E7=_T10E6[0];_T10E8=_T10E7.l;_T10E9=_T10E8.first_line;_T10EA=Cyc_Position_loc_to_seg(_T10E9);_T10EB=
# 2826
Cyc_Absyn_new_pat(_T10E5,_T10EA);yyval=Cyc_YY9(_T10EB);goto _LL0;case 427: _T10EC=yyyvsp;_T10ED=& _T10EC[0].v;_T10EE=(union Cyc_YYSTYPE*)_T10ED;_T10EF=
# 2830 "parse.y"
Cyc_yyget_String_tok(_T10EE);_T10F0=_tag_fat("alias",sizeof(char),6U);_T10F1=Cyc_strcmp(_T10EF,_T10F0);if(_T10F1==0)goto _TL2E5;_T10F2=yyyvsp;_T10F3=_T10F2[1];_T10F4=_T10F3.l;_T10F5=_T10F4.first_line;_T10F6=
Cyc_Position_loc_to_seg(_T10F5);_T10F7=_tag_fat("expecting `alias'",sizeof(char),18U);_T10F8=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10F6,_T10F7,_T10F8);goto _TL2E6;_TL2E5: _TL2E6: _T10F9=yyyvsp;_T10FA=_T10F9[0];_T10FB=_T10FA.l;_T10FC=_T10FB.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T10FC);_T10FD=yyyvsp;_T10FE=& _T10FD[2].v;_T10FF=(union Cyc_YYSTYPE*)_T10FE;_T1100=
Cyc_yyget_String_tok(_T10FF);_T1101=location;Cyc_Parse_tvar_ok(_T1100,_T1101);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T1102=tv;{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));_T1104=yyyvsp;_T1105=& _T1104[2].v;_T1106=(union Cyc_YYSTYPE*)_T1105;*_T187A=Cyc_yyget_String_tok(_T1106);_T1103=(struct _fat_ptr*)_T187A;}_T1102->name=_T1103;_T1107=tv;_T1107->identity=- 1;_T1108=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T187A->tag=0;_T110A=& Cyc_Kinds_ek;_T187A->f1=(struct Cyc_Absyn_Kind*)_T110A;_T1109=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T187A;}_T1108->kind=(void*)_T1109;_T110B=tv;_T110B->aquals_bound=0;_T110C=yyyvsp;_T110D=_T110C[0];_T110E=_T110D.l;_T110F=_T110E.first_line;_T1110=
Cyc_Position_loc_to_seg(_T110F);{struct _tuple0*_T187A=_cycalloc(sizeof(struct _tuple0));_T187A->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T1113=yyyvsp;_T1114=& _T1113[5].v;_T1115=(union Cyc_YYSTYPE*)_T1114;*_T187B=Cyc_yyget_String_tok(_T1115);_T1112=(struct _fat_ptr*)_T187B;}_T187A->f1=_T1112;_T1111=(struct _tuple0*)_T187A;}_T1116=yyyvsp;_T1117=& _T1116[4].v;_T1118=(union Cyc_YYSTYPE*)_T1117;_T1119=
Cyc_yyget_YY38(_T1118);_T111A=yyyvsp;_T111B=_T111A[4];_T111C=_T111B.l;_T111D=_T111C.first_line;_T111E=Cyc_Position_loc_to_seg(_T111D);_T111F=Cyc_Parse_type_name_to_type(_T1119,_T111E);{
# 2835
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T1110,_T1111,_T111F,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T187A->tag=2;
# 2837
_T187A->f1=tv;_T187A->f2=vd;_T1120=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T187A;}_T1121=(void*)_T1120;_T1122=location;_T1123=Cyc_Absyn_new_pat(_T1121,_T1122);yyval=Cyc_YY9(_T1123);goto _LL0;}}}case 428: _T1124=yyyvsp;_T1125=& _T1124[0].v;_T1126=(union Cyc_YYSTYPE*)_T1125;_T1127=
# 2840 "parse.y"
Cyc_yyget_String_tok(_T1126);_T1128=_tag_fat("alias",sizeof(char),6U);_T1129=Cyc_strcmp(_T1127,_T1128);if(_T1129==0)goto _TL2E7;_T112A=yyyvsp;_T112B=_T112A[1];_T112C=_T112B.l;_T112D=_T112C.first_line;_T112E=
Cyc_Position_loc_to_seg(_T112D);_T112F=_tag_fat("expecting `alias'",sizeof(char),18U);_T1130=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T112E,_T112F,_T1130);goto _TL2E8;_TL2E7: _TL2E8: _T1131=yyyvsp;_T1132=_T1131[0];_T1133=_T1132.l;_T1134=_T1133.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T1134);_T1135=yyyvsp;_T1136=& _T1135[2].v;_T1137=(union Cyc_YYSTYPE*)_T1136;_T1138=
Cyc_yyget_String_tok(_T1137);_T1139=location;Cyc_Parse_tvar_ok(_T1138,_T1139);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T113A=tv;{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));_T113C=yyyvsp;_T113D=& _T113C[2].v;_T113E=(union Cyc_YYSTYPE*)_T113D;*_T187A=Cyc_yyget_String_tok(_T113E);_T113B=(struct _fat_ptr*)_T187A;}_T113A->name=_T113B;_T113F=tv;_T113F->identity=- 1;_T1140=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T187A->tag=0;_T1142=& Cyc_Kinds_ek;_T187A->f1=(struct Cyc_Absyn_Kind*)_T1142;_T1141=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T187A;}_T1140->kind=(void*)_T1141;_T1143=tv;_T1143->aquals_bound=0;_T1144=yyyvsp;_T1145=_T1144[0];_T1146=_T1145.l;_T1147=_T1146.first_line;_T1148=
Cyc_Position_loc_to_seg(_T1147);{struct _tuple0*_T187A=_cycalloc(sizeof(struct _tuple0));_T187A->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T114B=yyyvsp;_T114C=& _T114B[5].v;_T114D=(union Cyc_YYSTYPE*)_T114C;*_T187B=Cyc_yyget_String_tok(_T114D);_T114A=(struct _fat_ptr*)_T187B;}_T187A->f1=_T114A;_T1149=(struct _tuple0*)_T187A;}_T114E=yyyvsp;_T114F=& _T114E[4].v;_T1150=(union Cyc_YYSTYPE*)_T114F;_T1151=
Cyc_yyget_YY38(_T1150);_T1152=yyyvsp;_T1153=_T1152[4];_T1154=_T1153.l;_T1155=_T1154.first_line;_T1156=Cyc_Position_loc_to_seg(_T1155);_T1157=Cyc_Parse_type_name_to_type(_T1151,_T1156);{
# 2845
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T1148,_T1149,_T1157,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T187A->tag=2;
# 2847
_T187A->f1=tv;_T187A->f2=vd;_T1158=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T187A;}_T1159=(void*)_T1158;_T115A=location;_T115B=Cyc_Absyn_new_pat(_T1159,_T115A);yyval=Cyc_YY9(_T115B);goto _LL0;}}}case 429: _T115C=yyyvsp;_T115D=& _T115C[2].v;_T115E=(union Cyc_YYSTYPE*)_T115D;_T115F=
# 2850 "parse.y"
Cyc_yyget_YY14(_T115E);{struct _tuple23 _T187A=*_T115F;int _T187B;struct Cyc_List_List*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_List_List*fps=_T187C;int dots=_T187B;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T187D->tag=6;
_T187D->f1=0;_T187D->f2=1;_T187D->f3=0;_T187D->f4=fps;_T187D->f5=dots;_T1160=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T187D;}_T1161=(void*)_T1160;_T1162=yyyvsp;_T1163=_T1162[0];_T1164=_T1163.l;_T1165=_T1164.first_line;_T1166=Cyc_Position_loc_to_seg(_T1165);_T1167=Cyc_Absyn_new_pat(_T1161,_T1166);yyval=Cyc_YY9(_T1167);goto _LL0;}}case 430: _T1168=yyyvsp;_T1169=& _T1168[2].v;_T116A=(union Cyc_YYSTYPE*)_T1169;_T116B=
# 2854 "parse.y"
Cyc_yyget_YY10(_T116A);{struct _tuple23 _T187A=*_T116B;int _T187B;struct Cyc_List_List*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_List_List*ps=_T187C;int dots=_T187B;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_T187D->tag=15;_T116D=yyyvsp;_T116E=& _T116D[0].v;_T116F=(union Cyc_YYSTYPE*)_T116E;
_T187D->f1=Cyc_yyget_QualId_tok(_T116F);_T187D->f2=ps;_T187D->f3=dots;_T116C=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T187D;}_T1170=(void*)_T116C;_T1171=yyyvsp;_T1172=_T1171[0];_T1173=_T1172.l;_T1174=_T1173.first_line;_T1175=Cyc_Position_loc_to_seg(_T1174);_T1176=Cyc_Absyn_new_pat(_T1170,_T1175);yyval=Cyc_YY9(_T1176);goto _LL0;}}case 431: _T1177=yyyvsp;_T1178=& _T1177[3].v;_T1179=(union Cyc_YYSTYPE*)_T1178;_T117A=
# 2858 "parse.y"
Cyc_yyget_YY14(_T1179);{struct _tuple23 _T187A=*_T117A;int _T187B;struct Cyc_List_List*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_List_List*fps=_T187C;int dots=_T187B;_T117C=Cyc_List_map_c;{
struct Cyc_List_List*(*_T187D)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T117C;_T117B=_T187D;}_T117D=yyyvsp;_T117E=_T117D[2];_T117F=_T117E.l;_T1180=_T117F.first_line;_T1181=Cyc_Position_loc_to_seg(_T1180);_T1182=yyyvsp;_T1183=& _T1182[2].v;_T1184=(union Cyc_YYSTYPE*)_T1183;_T1185=Cyc_yyget_YY41(_T1184);{struct Cyc_List_List*exist_ts=_T117B(Cyc_Parse_typ2tvar,_T1181,_T1185);_T1186=yyyvsp;_T1187=& _T1186[0].v;_T1188=(union Cyc_YYSTYPE*)_T1187;_T1189=
Cyc_yyget_QualId_tok(_T1188);{union Cyc_Absyn_AggrInfo ai=Cyc_Absyn_UnknownAggr(0U,_T1189,0);
struct Cyc_Absyn_AppType_Absyn_Type_struct*typ;typ=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_T118A=typ;_T118A->tag=0;_T118B=typ;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T187D->tag=24;_T187D->f1=ai;_T118C=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T187D;}_T118B->f1=(void*)_T118C;_T118D=typ;_T118D->f2=0;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T187D->tag=6;_T118F=typ;
_T187D->f1=(void*)_T118F;_T187D->f2=0;_T187D->f3=exist_ts;_T187D->f4=fps;_T187D->f5=dots;_T118E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T187D;}_T1190=(void*)_T118E;_T1191=yyyvsp;_T1192=_T1191[0];_T1193=_T1192.l;_T1194=_T1193.first_line;_T1195=Cyc_Position_loc_to_seg(_T1194);_T1196=Cyc_Absyn_new_pat(_T1190,_T1195);yyval=Cyc_YY9(_T1196);goto _LL0;}}}}case 432: _T1197=yyyvsp;_T1198=& _T1197[2].v;_T1199=(union Cyc_YYSTYPE*)_T1198;_T119A=
# 2865 "parse.y"
Cyc_yyget_YY14(_T1199);{struct _tuple23 _T187A=*_T119A;int _T187B;struct Cyc_List_List*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_List_List*fps=_T187C;int dots=_T187B;_T119C=Cyc_List_map_c;{
struct Cyc_List_List*(*_T187D)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T119C;_T119B=_T187D;}_T119D=yyyvsp;_T119E=_T119D[1];_T119F=_T119E.l;_T11A0=_T119F.first_line;_T11A1=Cyc_Position_loc_to_seg(_T11A0);_T11A2=yyyvsp;_T11A3=& _T11A2[1].v;_T11A4=(union Cyc_YYSTYPE*)_T11A3;_T11A5=Cyc_yyget_YY41(_T11A4);{struct Cyc_List_List*exist_ts=_T119B(Cyc_Parse_typ2tvar,_T11A1,_T11A5);{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T187D->tag=6;
_T187D->f1=0;_T187D->f2=0;_T187D->f3=exist_ts;_T187D->f4=fps;_T187D->f5=dots;_T11A6=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T187D;}_T11A7=(void*)_T11A6;_T11A8=yyyvsp;_T11A9=_T11A8[0];_T11AA=_T11A9.l;_T11AB=_T11AA.first_line;_T11AC=Cyc_Position_loc_to_seg(_T11AB);_T11AD=Cyc_Absyn_new_pat(_T11A7,_T11AC);yyval=Cyc_YY9(_T11AD);goto _LL0;}}}case 433:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T187A->tag=5;{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T187B->tag=5;_T11B0=yyyvsp;_T11B1=& _T11B0[1].v;_T11B2=(union Cyc_YYSTYPE*)_T11B1;
# 2870 "parse.y"
_T187B->f1=Cyc_yyget_YY9(_T11B2);_T11AF=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T187B;}_T11B3=(void*)_T11AF;_T11B4=yyyvsp;_T11B5=_T11B4[0];_T11B6=_T11B5.l;_T11B7=_T11B6.first_line;_T11B8=Cyc_Position_loc_to_seg(_T11B7);_T187A->f1=Cyc_Absyn_new_pat(_T11B3,_T11B8);_T11AE=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T187A;}_T11B9=(void*)_T11AE;_T11BA=yyyvsp;_T11BB=_T11BA[0];_T11BC=_T11BB.l;_T11BD=_T11BC.first_line;_T11BE=Cyc_Position_loc_to_seg(_T11BD);_T11BF=Cyc_Absyn_new_pat(_T11B9,_T11BE);yyval=Cyc_YY9(_T11BF);goto _LL0;case 434:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T187A->tag=3;_T11C1=yyyvsp;_T11C2=_T11C1[0];_T11C3=_T11C2.l;_T11C4=_T11C3.first_line;_T11C5=
# 2872 "parse.y"
Cyc_Position_loc_to_seg(_T11C4);{struct _tuple0*_T187B=_cycalloc(sizeof(struct _tuple0));_T187B->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));_T11C8=yyyvsp;_T11C9=& _T11C8[1].v;_T11CA=(union Cyc_YYSTYPE*)_T11C9;*_T187C=Cyc_yyget_String_tok(_T11CA);_T11C7=(struct _fat_ptr*)_T187C;}_T187B->f1=_T11C7;_T11C6=(struct _tuple0*)_T187B;}_T11CB=Cyc_Absyn_void_type;_T187A->f1=Cyc_Absyn_new_vardecl(_T11C5,_T11C6,_T11CB,0,0);_T11CC=& Cyc_Absyn_Wild_p_val;_T11CD=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T11CC;_T11CE=(void*)_T11CD;_T11CF=yyyvsp;_T11D0=_T11CF[1];_T11D1=_T11D0.l;_T11D2=_T11D1.first_line;_T11D3=
# 2874
Cyc_Position_loc_to_seg(_T11D2);_T187A->f2=Cyc_Absyn_new_pat(_T11CE,_T11D3);_T11C0=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T187A;}_T11D4=(void*)_T11C0;_T11D5=yyyvsp;_T11D6=_T11D5[0];_T11D7=_T11D6.l;_T11D8=_T11D7.first_line;_T11D9=
Cyc_Position_loc_to_seg(_T11D8);_T11DA=
# 2872
Cyc_Absyn_new_pat(_T11D4,_T11D9);yyval=Cyc_YY9(_T11DA);goto _LL0;case 435: _T11DB=yyyvsp;_T11DC=& _T11DB[2].v;_T11DD=(union Cyc_YYSTYPE*)_T11DC;_T11DE=
# 2877 "parse.y"
Cyc_yyget_String_tok(_T11DD);_T11DF=_tag_fat("as",sizeof(char),3U);_T11E0=Cyc_strcmp(_T11DE,_T11DF);if(_T11E0==0)goto _TL2E9;_T11E1=yyyvsp;_T11E2=_T11E1[2];_T11E3=_T11E2.l;_T11E4=_T11E3.first_line;_T11E5=
Cyc_Position_loc_to_seg(_T11E4);_T11E6=_tag_fat("expecting `as'",sizeof(char),15U);_T11E7=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T11E5,_T11E6,_T11E7);goto _TL2EA;_TL2E9: _TL2EA:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T187A->tag=3;_T11E9=yyyvsp;_T11EA=_T11E9[0];_T11EB=_T11EA.l;_T11EC=_T11EB.first_line;_T11ED=
Cyc_Position_loc_to_seg(_T11EC);{struct _tuple0*_T187B=_cycalloc(sizeof(struct _tuple0));_T187B->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));_T11F0=yyyvsp;_T11F1=& _T11F0[1].v;_T11F2=(union Cyc_YYSTYPE*)_T11F1;*_T187C=Cyc_yyget_String_tok(_T11F2);_T11EF=(struct _fat_ptr*)_T187C;}_T187B->f1=_T11EF;_T11EE=(struct _tuple0*)_T187B;}_T11F3=Cyc_Absyn_void_type;_T187A->f1=Cyc_Absyn_new_vardecl(_T11ED,_T11EE,_T11F3,0,0);_T11F4=yyyvsp;_T11F5=& _T11F4[3].v;_T11F6=(union Cyc_YYSTYPE*)_T11F5;
# 2881
_T187A->f2=Cyc_yyget_YY9(_T11F6);_T11E8=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T187A;}_T11F7=(void*)_T11E8;_T11F8=yyyvsp;_T11F9=_T11F8[0];_T11FA=_T11F9.l;_T11FB=_T11FA.first_line;_T11FC=Cyc_Position_loc_to_seg(_T11FB);_T11FD=
# 2879
Cyc_Absyn_new_pat(_T11F7,_T11FC);yyval=Cyc_YY9(_T11FD);goto _LL0;case 436: _T11FE=yyyvsp;_T11FF=& _T11FE[2].v;_T1200=(union Cyc_YYSTYPE*)_T11FF;_T1201=
# 2884 "parse.y"
Cyc_yyget_String_tok(_T1200);_T1202=& Cyc_Kinds_ik;_T1203=(struct Cyc_Absyn_Kind*)_T1202;_T1204=Cyc_Kinds_kind_to_bound(_T1203);_T1205=yyyvsp;_T1206=_T1205[2];_T1207=_T1206.l;_T1208=_T1207.first_line;_T1209=Cyc_Position_loc_to_seg(_T1208);{void*tag=Cyc_Parse_id2type(_T1201,_T1204,0,_T1209);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T187A->tag=4;_T120B=yyyvsp;_T120C=_T120B[2];_T120D=_T120C.l;_T120E=_T120D.first_line;_T120F=
Cyc_Position_loc_to_seg(_T120E);_T1210=tag;_T187A->f1=Cyc_Parse_typ2tvar(_T120F,_T1210);_T1211=yyyvsp;_T1212=_T1211[0];_T1213=_T1212.l;_T1214=_T1213.first_line;_T1215=
Cyc_Position_loc_to_seg(_T1214);{struct _tuple0*_T187B=_cycalloc(sizeof(struct _tuple0));_T187B->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));_T1218=yyyvsp;_T1219=& _T1218[0].v;_T121A=(union Cyc_YYSTYPE*)_T1219;*_T187C=Cyc_yyget_String_tok(_T121A);_T1217=(struct _fat_ptr*)_T187C;}_T187B->f1=_T1217;_T1216=(struct _tuple0*)_T187B;}_T121B=
Cyc_Absyn_tag_type(tag);
# 2886
_T187A->f2=Cyc_Absyn_new_vardecl(_T1215,_T1216,_T121B,0,0);_T120A=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T187A;}_T121C=(void*)_T120A;_T121D=yyyvsp;_T121E=_T121D[0];_T121F=_T121E.l;_T1220=_T121F.first_line;_T1221=
# 2888
Cyc_Position_loc_to_seg(_T1220);_T1222=
# 2885
Cyc_Absyn_new_pat(_T121C,_T1221);yyval=Cyc_YY9(_T1222);goto _LL0;}case 437: _T1223=& Cyc_Kinds_ik;_T1224=(struct Cyc_Absyn_Kind*)_T1223;_T1225=
# 2890 "parse.y"
Cyc_Kinds_kind_to_bound(_T1224);{struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(_T1225);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T187A->tag=4;
_T187A->f1=tv;_T1227=yyyvsp;_T1228=_T1227[0];_T1229=_T1228.l;_T122A=_T1229.first_line;_T122B=
Cyc_Position_loc_to_seg(_T122A);{struct _tuple0*_T187B=_cycalloc(sizeof(struct _tuple0));_T187B->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));_T122E=yyyvsp;_T122F=& _T122E[0].v;_T1230=(union Cyc_YYSTYPE*)_T122F;*_T187C=Cyc_yyget_String_tok(_T1230);_T122D=(struct _fat_ptr*)_T187C;}_T187B->f1=_T122D;_T122C=(struct _tuple0*)_T187B;}_T1231=
Cyc_Absyn_var_type(tv);_T1232=Cyc_Absyn_tag_type(_T1231);
# 2892
_T187A->f2=Cyc_Absyn_new_vardecl(_T122B,_T122C,_T1232,0,0);_T1226=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T187A;}_T1233=(void*)_T1226;_T1234=yyyvsp;_T1235=_T1234[0];_T1236=_T1235.l;_T1237=_T1236.first_line;_T1238=
# 2894
Cyc_Position_loc_to_seg(_T1237);_T1239=
# 2891
Cyc_Absyn_new_pat(_T1233,_T1238);yyval=Cyc_YY9(_T1239);goto _LL0;}case 438:{struct _tuple23*_T187A=_cycalloc(sizeof(struct _tuple23));_T123B=yyyvsp;_T123C=& _T123B[0].v;_T123D=(union Cyc_YYSTYPE*)_T123C;_T123E=
# 2898 "parse.y"
Cyc_yyget_YY11(_T123D);_T187A->f0=Cyc_List_rev(_T123E);_T187A->f1=0;_T123A=(struct _tuple23*)_T187A;}yyval=Cyc_YY10(_T123A);goto _LL0;case 439:{struct _tuple23*_T187A=_cycalloc(sizeof(struct _tuple23));_T1240=yyyvsp;_T1241=& _T1240[0].v;_T1242=(union Cyc_YYSTYPE*)_T1241;_T1243=
# 2899 "parse.y"
Cyc_yyget_YY11(_T1242);_T187A->f0=Cyc_List_rev(_T1243);_T187A->f1=1;_T123F=(struct _tuple23*)_T187A;}yyval=Cyc_YY10(_T123F);goto _LL0;case 440:{struct _tuple23*_T187A=_cycalloc(sizeof(struct _tuple23));
# 2900 "parse.y"
_T187A->f0=0;_T187A->f1=1;_T1244=(struct _tuple23*)_T187A;}yyval=Cyc_YY10(_T1244);goto _LL0;case 441:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T1246=yyyvsp;_T1247=& _T1246[0].v;_T1248=(union Cyc_YYSTYPE*)_T1247;
# 2903
_T187A->hd=Cyc_yyget_YY9(_T1248);_T187A->tl=0;_T1245=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY11(_T1245);goto _LL0;case 442:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T124A=yyyvsp;_T124B=& _T124A[2].v;_T124C=(union Cyc_YYSTYPE*)_T124B;
# 2904 "parse.y"
_T187A->hd=Cyc_yyget_YY9(_T124C);_T124D=yyyvsp;_T124E=& _T124D[0].v;_T124F=(union Cyc_YYSTYPE*)_T124E;_T187A->tl=Cyc_yyget_YY11(_T124F);_T1249=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY11(_T1249);goto _LL0;case 443:{struct _tuple24*_T187A=_cycalloc(sizeof(struct _tuple24));
# 2907
_T187A->f0=0;_T1251=yyyvsp;_T1252=& _T1251[0].v;_T1253=(union Cyc_YYSTYPE*)_T1252;_T187A->f1=Cyc_yyget_YY9(_T1253);_T1250=(struct _tuple24*)_T187A;}yyval=Cyc_YY12(_T1250);goto _LL0;case 444:{struct _tuple24*_T187A=_cycalloc(sizeof(struct _tuple24));_T1255=yyyvsp;_T1256=& _T1255[0].v;_T1257=(union Cyc_YYSTYPE*)_T1256;
# 2908 "parse.y"
_T187A->f0=Cyc_yyget_YY42(_T1257);_T1258=yyyvsp;_T1259=& _T1258[1].v;_T125A=(union Cyc_YYSTYPE*)_T1259;_T187A->f1=Cyc_yyget_YY9(_T125A);_T1254=(struct _tuple24*)_T187A;}yyval=Cyc_YY12(_T1254);goto _LL0;case 445:{struct _tuple23*_T187A=_cycalloc(sizeof(struct _tuple23));_T125C=yyyvsp;_T125D=& _T125C[0].v;_T125E=(union Cyc_YYSTYPE*)_T125D;_T125F=
# 2911
Cyc_yyget_YY13(_T125E);_T187A->f0=Cyc_List_rev(_T125F);_T187A->f1=0;_T125B=(struct _tuple23*)_T187A;}yyval=Cyc_YY14(_T125B);goto _LL0;case 446:{struct _tuple23*_T187A=_cycalloc(sizeof(struct _tuple23));_T1261=yyyvsp;_T1262=& _T1261[0].v;_T1263=(union Cyc_YYSTYPE*)_T1262;_T1264=
# 2912 "parse.y"
Cyc_yyget_YY13(_T1263);_T187A->f0=Cyc_List_rev(_T1264);_T187A->f1=1;_T1260=(struct _tuple23*)_T187A;}yyval=Cyc_YY14(_T1260);goto _LL0;case 447:{struct _tuple23*_T187A=_cycalloc(sizeof(struct _tuple23));
# 2913 "parse.y"
_T187A->f0=0;_T187A->f1=1;_T1265=(struct _tuple23*)_T187A;}yyval=Cyc_YY14(_T1265);goto _LL0;case 448:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T1267=yyyvsp;_T1268=& _T1267[0].v;_T1269=(union Cyc_YYSTYPE*)_T1268;
# 2916
_T187A->hd=Cyc_yyget_YY12(_T1269);_T187A->tl=0;_T1266=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY13(_T1266);goto _LL0;case 449:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T126B=yyyvsp;_T126C=& _T126B[2].v;_T126D=(union Cyc_YYSTYPE*)_T126C;
# 2917 "parse.y"
_T187A->hd=Cyc_yyget_YY12(_T126D);_T126E=yyyvsp;_T126F=& _T126E[0].v;_T1270=(union Cyc_YYSTYPE*)_T126F;_T187A->tl=Cyc_yyget_YY13(_T1270);_T126A=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY13(_T126A);goto _LL0;case 450: _T1271=yyyvsp;_T1272=_T1271[0];
# 2922 "parse.y"
yyval=_T1272.v;goto _LL0;case 451: _T1273=yyyvsp;_T1274=& _T1273[0].v;_T1275=(union Cyc_YYSTYPE*)_T1274;_T1276=
# 2923 "parse.y"
Cyc_yyget_Exp_tok(_T1275);_T1277=yyyvsp;_T1278=& _T1277[2].v;_T1279=(union Cyc_YYSTYPE*)_T1278;_T127A=Cyc_yyget_Exp_tok(_T1279);_T127B=yyyvsp;_T127C=_T127B[0];_T127D=_T127C.l;_T127E=_T127D.first_line;_T127F=Cyc_Position_loc_to_seg(_T127E);_T1280=Cyc_Absyn_seq_exp(_T1276,_T127A,_T127F);yyval=Cyc_Exp_tok(_T1280);goto _LL0;case 452: _T1281=yyyvsp;_T1282=_T1281[0];
# 2927 "parse.y"
yyval=_T1282.v;goto _LL0;case 453: _T1283=yyyvsp;_T1284=& _T1283[0].v;_T1285=(union Cyc_YYSTYPE*)_T1284;_T1286=
# 2929 "parse.y"
Cyc_yyget_Exp_tok(_T1285);_T1287=yyyvsp;_T1288=& _T1287[1].v;_T1289=(union Cyc_YYSTYPE*)_T1288;_T128A=Cyc_yyget_YY7(_T1289);_T128B=yyyvsp;_T128C=& _T128B[2].v;_T128D=(union Cyc_YYSTYPE*)_T128C;_T128E=Cyc_yyget_Exp_tok(_T128D);_T128F=yyyvsp;_T1290=_T128F[0];_T1291=_T1290.l;_T1292=_T1291.first_line;_T1293=Cyc_Position_loc_to_seg(_T1292);_T1294=Cyc_Absyn_assignop_exp(_T1286,_T128A,_T128E,_T1293);yyval=Cyc_Exp_tok(_T1294);goto _LL0;case 454: _T1295=yyyvsp;_T1296=& _T1295[0].v;_T1297=(union Cyc_YYSTYPE*)_T1296;_T1298=
# 2931 "parse.y"
Cyc_yyget_Exp_tok(_T1297);_T1299=yyyvsp;_T129A=& _T1299[2].v;_T129B=(union Cyc_YYSTYPE*)_T129A;_T129C=Cyc_yyget_Exp_tok(_T129B);_T129D=yyyvsp;_T129E=_T129D[0];_T129F=_T129E.l;_T12A0=_T129F.first_line;_T12A1=Cyc_Position_loc_to_seg(_T12A0);_T12A2=Cyc_Absyn_swap_exp(_T1298,_T129C,_T12A1);yyval=Cyc_Exp_tok(_T12A2);goto _LL0;case 455:
# 2935 "parse.y"
 yyval=Cyc_YY7(0);goto _LL0;case 456:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2936 "parse.y"
_T187A->v=(void*)1U;_T12A3=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12A3);goto _LL0;case 457:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2937 "parse.y"
_T187A->v=(void*)3U;_T12A4=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12A4);goto _LL0;case 458:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2938 "parse.y"
_T187A->v=(void*)4U;_T12A5=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12A5);goto _LL0;case 459:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2939 "parse.y"
_T187A->v=(void*)0U;_T12A6=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12A6);goto _LL0;case 460:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2940 "parse.y"
_T187A->v=(void*)2U;_T12A7=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12A7);goto _LL0;case 461:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2941 "parse.y"
_T187A->v=(void*)16U;_T12A8=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12A8);goto _LL0;case 462:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2942 "parse.y"
_T187A->v=(void*)17U;_T12A9=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12A9);goto _LL0;case 463:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2943 "parse.y"
_T187A->v=(void*)13U;_T12AA=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12AA);goto _LL0;case 464:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2944 "parse.y"
_T187A->v=(void*)15U;_T12AB=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12AB);goto _LL0;case 465:{struct Cyc_Core_Opt*_T187A=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2945 "parse.y"
_T187A->v=(void*)14U;_T12AC=(struct Cyc_Core_Opt*)_T187A;}yyval=Cyc_YY7(_T12AC);goto _LL0;case 466: _T12AD=yyyvsp;_T12AE=_T12AD[0];
# 2949 "parse.y"
yyval=_T12AE.v;goto _LL0;case 467: _T12AF=yyyvsp;_T12B0=& _T12AF[0].v;_T12B1=(union Cyc_YYSTYPE*)_T12B0;_T12B2=
# 2951 "parse.y"
Cyc_yyget_Exp_tok(_T12B1);_T12B3=yyyvsp;_T12B4=& _T12B3[2].v;_T12B5=(union Cyc_YYSTYPE*)_T12B4;_T12B6=Cyc_yyget_Exp_tok(_T12B5);_T12B7=yyyvsp;_T12B8=& _T12B7[4].v;_T12B9=(union Cyc_YYSTYPE*)_T12B8;_T12BA=Cyc_yyget_Exp_tok(_T12B9);_T12BB=yyyvsp;_T12BC=_T12BB[0];_T12BD=_T12BC.l;_T12BE=_T12BD.first_line;_T12BF=Cyc_Position_loc_to_seg(_T12BE);_T12C0=Cyc_Absyn_conditional_exp(_T12B2,_T12B6,_T12BA,_T12BF);yyval=Cyc_Exp_tok(_T12C0);goto _LL0;case 468: _T12C1=yyyvsp;_T12C2=& _T12C1[1].v;_T12C3=(union Cyc_YYSTYPE*)_T12C2;_T12C4=
# 2953 "parse.y"
Cyc_yyget_Exp_tok(_T12C3);_T12C5=yyyvsp;_T12C6=_T12C5[0];_T12C7=_T12C6.l;_T12C8=_T12C7.first_line;_T12C9=Cyc_Position_loc_to_seg(_T12C8);_T12CA=Cyc_Absyn_throw_exp(_T12C4,_T12C9);yyval=Cyc_Exp_tok(_T12CA);goto _LL0;case 469: _T12CB=yyyvsp;_T12CC=& _T12CB[1].v;_T12CD=(union Cyc_YYSTYPE*)_T12CC;_T12CE=
# 2955 "parse.y"
Cyc_yyget_Exp_tok(_T12CD);_T12CF=yyyvsp;_T12D0=_T12CF[0];_T12D1=_T12D0.l;_T12D2=_T12D1.first_line;_T12D3=Cyc_Position_loc_to_seg(_T12D2);_T12D4=Cyc_Absyn_New_exp(0,_T12CE,0,_T12D3);yyval=Cyc_Exp_tok(_T12D4);goto _LL0;case 470: _T12D5=yyyvsp;_T12D6=& _T12D5[1].v;_T12D7=(union Cyc_YYSTYPE*)_T12D6;_T12D8=
# 2956 "parse.y"
Cyc_yyget_Exp_tok(_T12D7);_T12D9=yyyvsp;_T12DA=_T12D9[0];_T12DB=_T12DA.l;_T12DC=_T12DB.first_line;_T12DD=Cyc_Position_loc_to_seg(_T12DC);_T12DE=Cyc_Absyn_New_exp(0,_T12D8,0,_T12DD);yyval=Cyc_Exp_tok(_T12DE);goto _LL0;case 471: _T12DF=yyyvsp;_T12E0=& _T12DF[4].v;_T12E1=(union Cyc_YYSTYPE*)_T12E0;_T12E2=
# 2957 "parse.y"
Cyc_yyget_Exp_tok(_T12E1);_T12E3=yyyvsp;_T12E4=& _T12E3[2].v;_T12E5=(union Cyc_YYSTYPE*)_T12E4;_T12E6=Cyc_yyget_Exp_tok(_T12E5);_T12E7=yyyvsp;_T12E8=_T12E7[0];_T12E9=_T12E8.l;_T12EA=_T12E9.first_line;_T12EB=Cyc_Position_loc_to_seg(_T12EA);_T12EC=Cyc_Absyn_New_exp(0,_T12E2,_T12E6,_T12EB);yyval=Cyc_Exp_tok(_T12EC);goto _LL0;case 472: _T12ED=yyyvsp;_T12EE=& _T12ED[4].v;_T12EF=(union Cyc_YYSTYPE*)_T12EE;_T12F0=
# 2958 "parse.y"
Cyc_yyget_Exp_tok(_T12EF);_T12F1=yyyvsp;_T12F2=& _T12F1[2].v;_T12F3=(union Cyc_YYSTYPE*)_T12F2;_T12F4=Cyc_yyget_Exp_tok(_T12F3);_T12F5=yyyvsp;_T12F6=_T12F5[0];_T12F7=_T12F6.l;_T12F8=_T12F7.first_line;_T12F9=Cyc_Position_loc_to_seg(_T12F8);_T12FA=Cyc_Absyn_New_exp(0,_T12F0,_T12F4,_T12F9);yyval=Cyc_Exp_tok(_T12FA);goto _LL0;case 473: _T12FB=yyyvsp;_T12FC=& _T12FB[2].v;_T12FD=(union Cyc_YYSTYPE*)_T12FC;_T12FE=
# 2959 "parse.y"
Cyc_yyget_Exp_tok(_T12FD);{struct _tuple16 _T187A=Cyc_Parse_split_seq(_T12FE);struct Cyc_Absyn_Exp*_T187B;struct Cyc_Absyn_Exp*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_Absyn_Exp*rgn=_T187C;struct Cyc_Absyn_Exp*qual=_T187B;_T12FF=rgn;_T1300=yyyvsp;_T1301=& _T1300[4].v;_T1302=(union Cyc_YYSTYPE*)_T1301;_T1303=Cyc_yyget_Exp_tok(_T1302);_T1304=qual;_T1305=yyyvsp;_T1306=_T1305[0];_T1307=_T1306.l;_T1308=_T1307.first_line;_T1309=Cyc_Position_loc_to_seg(_T1308);_T130A=Cyc_Absyn_New_exp(_T12FF,_T1303,_T1304,_T1309);yyval=Cyc_Exp_tok(_T130A);goto _LL0;}}case 474: _T130B=yyyvsp;_T130C=& _T130B[2].v;_T130D=(union Cyc_YYSTYPE*)_T130C;_T130E=
# 2960 "parse.y"
Cyc_yyget_Exp_tok(_T130D);{struct _tuple16 _T187A=Cyc_Parse_split_seq(_T130E);struct Cyc_Absyn_Exp*_T187B;struct Cyc_Absyn_Exp*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_Absyn_Exp*rgn=_T187C;struct Cyc_Absyn_Exp*qual=_T187B;_T130F=rgn;_T1310=yyyvsp;_T1311=& _T1310[4].v;_T1312=(union Cyc_YYSTYPE*)_T1311;_T1313=Cyc_yyget_Exp_tok(_T1312);_T1314=qual;_T1315=yyyvsp;_T1316=_T1315[0];_T1317=_T1316.l;_T1318=_T1317.first_line;_T1319=Cyc_Position_loc_to_seg(_T1318);_T131A=Cyc_Absyn_New_exp(_T130F,_T1313,_T1314,_T1319);yyval=Cyc_Exp_tok(_T131A);goto _LL0;}}case 475: _T131B=yyyvsp;_T131C=_T131B[0];
# 2972 "parse.y"
yyval=_T131C.v;goto _LL0;case 476: _T131D=yyyvsp;_T131E=_T131D[0];
# 2975
yyval=_T131E.v;goto _LL0;case 477: _T131F=yyyvsp;_T1320=& _T131F[0].v;_T1321=(union Cyc_YYSTYPE*)_T1320;_T1322=
# 2977 "parse.y"
Cyc_yyget_Exp_tok(_T1321);_T1323=yyyvsp;_T1324=& _T1323[2].v;_T1325=(union Cyc_YYSTYPE*)_T1324;_T1326=Cyc_yyget_Exp_tok(_T1325);_T1327=yyyvsp;_T1328=_T1327[0];_T1329=_T1328.l;_T132A=_T1329.first_line;_T132B=Cyc_Position_loc_to_seg(_T132A);_T132C=Cyc_Absyn_or_exp(_T1322,_T1326,_T132B);yyval=Cyc_Exp_tok(_T132C);goto _LL0;case 478: _T132D=yyyvsp;_T132E=_T132D[0];
# 2980
yyval=_T132E.v;goto _LL0;case 479: _T132F=yyyvsp;_T1330=& _T132F[0].v;_T1331=(union Cyc_YYSTYPE*)_T1330;_T1332=
# 2982 "parse.y"
Cyc_yyget_Exp_tok(_T1331);_T1333=yyyvsp;_T1334=& _T1333[2].v;_T1335=(union Cyc_YYSTYPE*)_T1334;_T1336=Cyc_yyget_Exp_tok(_T1335);_T1337=yyyvsp;_T1338=_T1337[0];_T1339=_T1338.l;_T133A=_T1339.first_line;_T133B=Cyc_Position_loc_to_seg(_T133A);_T133C=Cyc_Absyn_and_exp(_T1332,_T1336,_T133B);yyval=Cyc_Exp_tok(_T133C);goto _LL0;case 480: _T133D=yyyvsp;_T133E=_T133D[0];
# 2985
yyval=_T133E.v;goto _LL0;case 481: _T133F=yyyvsp;_T1340=& _T133F[0].v;_T1341=(union Cyc_YYSTYPE*)_T1340;_T1342=
# 2987 "parse.y"
Cyc_yyget_Exp_tok(_T1341);_T1343=yyyvsp;_T1344=& _T1343[2].v;_T1345=(union Cyc_YYSTYPE*)_T1344;_T1346=Cyc_yyget_Exp_tok(_T1345);_T1347=yyyvsp;_T1348=_T1347[0];_T1349=_T1348.l;_T134A=_T1349.first_line;_T134B=Cyc_Position_loc_to_seg(_T134A);_T134C=Cyc_Absyn_prim2_exp(14U,_T1342,_T1346,_T134B);yyval=Cyc_Exp_tok(_T134C);goto _LL0;case 482: _T134D=yyyvsp;_T134E=_T134D[0];
# 2990
yyval=_T134E.v;goto _LL0;case 483: _T134F=yyyvsp;_T1350=& _T134F[0].v;_T1351=(union Cyc_YYSTYPE*)_T1350;_T1352=
# 2992 "parse.y"
Cyc_yyget_Exp_tok(_T1351);_T1353=yyyvsp;_T1354=& _T1353[2].v;_T1355=(union Cyc_YYSTYPE*)_T1354;_T1356=Cyc_yyget_Exp_tok(_T1355);_T1357=yyyvsp;_T1358=_T1357[0];_T1359=_T1358.l;_T135A=_T1359.first_line;_T135B=Cyc_Position_loc_to_seg(_T135A);_T135C=Cyc_Absyn_prim2_exp(15U,_T1352,_T1356,_T135B);yyval=Cyc_Exp_tok(_T135C);goto _LL0;case 484: _T135D=yyyvsp;_T135E=_T135D[0];
# 2995
yyval=_T135E.v;goto _LL0;case 485: _T135F=yyyvsp;_T1360=& _T135F[0].v;_T1361=(union Cyc_YYSTYPE*)_T1360;_T1362=
# 2997 "parse.y"
Cyc_yyget_Exp_tok(_T1361);_T1363=yyyvsp;_T1364=& _T1363[2].v;_T1365=(union Cyc_YYSTYPE*)_T1364;_T1366=Cyc_yyget_Exp_tok(_T1365);_T1367=yyyvsp;_T1368=_T1367[0];_T1369=_T1368.l;_T136A=_T1369.first_line;_T136B=Cyc_Position_loc_to_seg(_T136A);_T136C=Cyc_Absyn_prim2_exp(13U,_T1362,_T1366,_T136B);yyval=Cyc_Exp_tok(_T136C);goto _LL0;case 486: _T136D=yyyvsp;_T136E=_T136D[0];
# 3000
yyval=_T136E.v;goto _LL0;case 487: _T136F=yyyvsp;_T1370=& _T136F[1].v;_T1371=(union Cyc_YYSTYPE*)_T1370;_T1372=
# 3002 "parse.y"
Cyc_yyget_YY69(_T1371);_T1373=yyyvsp;_T1374=& _T1373[0].v;_T1375=(union Cyc_YYSTYPE*)_T1374;_T1376=Cyc_yyget_Exp_tok(_T1375);_T1377=yyyvsp;_T1378=& _T1377[2].v;_T1379=(union Cyc_YYSTYPE*)_T1378;_T137A=Cyc_yyget_Exp_tok(_T1379);_T137B=yyyvsp;_T137C=_T137B[0];_T137D=_T137C.l;_T137E=_T137D.first_line;_T137F=Cyc_Position_loc_to_seg(_T137E);_T1380=_T1372(_T1376,_T137A,_T137F);yyval=Cyc_Exp_tok(_T1380);goto _LL0;case 488: _T1381=yyyvsp;_T1382=_T1381[0];
# 3005
yyval=_T1382.v;goto _LL0;case 489: _T1383=yyyvsp;_T1384=& _T1383[1].v;_T1385=(union Cyc_YYSTYPE*)_T1384;_T1386=
# 3007 "parse.y"
Cyc_yyget_YY69(_T1385);_T1387=yyyvsp;_T1388=& _T1387[0].v;_T1389=(union Cyc_YYSTYPE*)_T1388;_T138A=Cyc_yyget_Exp_tok(_T1389);_T138B=yyyvsp;_T138C=& _T138B[2].v;_T138D=(union Cyc_YYSTYPE*)_T138C;_T138E=Cyc_yyget_Exp_tok(_T138D);_T138F=yyyvsp;_T1390=_T138F[0];_T1391=_T1390.l;_T1392=_T1391.first_line;_T1393=Cyc_Position_loc_to_seg(_T1392);_T1394=_T1386(_T138A,_T138E,_T1393);yyval=Cyc_Exp_tok(_T1394);goto _LL0;case 490: _T1395=yyyvsp;_T1396=_T1395[0];
# 3010
yyval=_T1396.v;goto _LL0;case 491: _T1397=yyyvsp;_T1398=& _T1397[0].v;_T1399=(union Cyc_YYSTYPE*)_T1398;_T139A=
# 3012 "parse.y"
Cyc_yyget_Exp_tok(_T1399);_T139B=yyyvsp;_T139C=& _T139B[2].v;_T139D=(union Cyc_YYSTYPE*)_T139C;_T139E=Cyc_yyget_Exp_tok(_T139D);_T139F=yyyvsp;_T13A0=_T139F[0];_T13A1=_T13A0.l;_T13A2=_T13A1.first_line;_T13A3=Cyc_Position_loc_to_seg(_T13A2);_T13A4=Cyc_Absyn_prim2_exp(16U,_T139A,_T139E,_T13A3);yyval=Cyc_Exp_tok(_T13A4);goto _LL0;case 492: _T13A5=yyyvsp;_T13A6=& _T13A5[0].v;_T13A7=(union Cyc_YYSTYPE*)_T13A6;_T13A8=
# 3014 "parse.y"
Cyc_yyget_Exp_tok(_T13A7);_T13A9=yyyvsp;_T13AA=& _T13A9[2].v;_T13AB=(union Cyc_YYSTYPE*)_T13AA;_T13AC=Cyc_yyget_Exp_tok(_T13AB);_T13AD=yyyvsp;_T13AE=_T13AD[0];_T13AF=_T13AE.l;_T13B0=_T13AF.first_line;_T13B1=Cyc_Position_loc_to_seg(_T13B0);_T13B2=Cyc_Absyn_prim2_exp(17U,_T13A8,_T13AC,_T13B1);yyval=Cyc_Exp_tok(_T13B2);goto _LL0;case 493: _T13B3=yyyvsp;_T13B4=_T13B3[0];
# 3017
yyval=_T13B4.v;goto _LL0;case 494: _T13B5=yyyvsp;_T13B6=& _T13B5[1].v;_T13B7=(union Cyc_YYSTYPE*)_T13B6;_T13B8=
# 3019 "parse.y"
Cyc_yyget_YY6(_T13B7);_T13B9=yyyvsp;_T13BA=& _T13B9[0].v;_T13BB=(union Cyc_YYSTYPE*)_T13BA;_T13BC=Cyc_yyget_Exp_tok(_T13BB);_T13BD=yyyvsp;_T13BE=& _T13BD[2].v;_T13BF=(union Cyc_YYSTYPE*)_T13BE;_T13C0=Cyc_yyget_Exp_tok(_T13BF);_T13C1=yyyvsp;_T13C2=_T13C1[0];_T13C3=_T13C2.l;_T13C4=_T13C3.first_line;_T13C5=Cyc_Position_loc_to_seg(_T13C4);_T13C6=Cyc_Absyn_prim2_exp(_T13B8,_T13BC,_T13C0,_T13C5);yyval=Cyc_Exp_tok(_T13C6);goto _LL0;case 495: _T13C7=yyyvsp;_T13C8=_T13C7[0];
# 3022
yyval=_T13C8.v;goto _LL0;case 496: _T13C9=yyyvsp;_T13CA=& _T13C9[1].v;_T13CB=(union Cyc_YYSTYPE*)_T13CA;_T13CC=
# 3024 "parse.y"
Cyc_yyget_YY6(_T13CB);_T13CD=yyyvsp;_T13CE=& _T13CD[0].v;_T13CF=(union Cyc_YYSTYPE*)_T13CE;_T13D0=Cyc_yyget_Exp_tok(_T13CF);_T13D1=yyyvsp;_T13D2=& _T13D1[2].v;_T13D3=(union Cyc_YYSTYPE*)_T13D2;_T13D4=Cyc_yyget_Exp_tok(_T13D3);_T13D5=yyyvsp;_T13D6=_T13D5[0];_T13D7=_T13D6.l;_T13D8=_T13D7.first_line;_T13D9=Cyc_Position_loc_to_seg(_T13D8);_T13DA=Cyc_Absyn_prim2_exp(_T13CC,_T13D0,_T13D4,_T13D9);yyval=Cyc_Exp_tok(_T13DA);goto _LL0;case 497: _T13DB=Cyc_Absyn_eq_exp;
# 3027
yyval=Cyc_YY69(_T13DB);goto _LL0;case 498: _T13DC=Cyc_Absyn_neq_exp;
# 3028 "parse.y"
yyval=Cyc_YY69(_T13DC);goto _LL0;case 499: _T13DD=Cyc_Absyn_lt_exp;
# 3031
yyval=Cyc_YY69(_T13DD);goto _LL0;case 500: _T13DE=Cyc_Absyn_gt_exp;
# 3032 "parse.y"
yyval=Cyc_YY69(_T13DE);goto _LL0;case 501: _T13DF=Cyc_Absyn_lte_exp;
# 3033 "parse.y"
yyval=Cyc_YY69(_T13DF);goto _LL0;case 502: _T13E0=Cyc_Absyn_gte_exp;
# 3034 "parse.y"
yyval=Cyc_YY69(_T13E0);goto _LL0;case 503:
# 3037
 yyval=Cyc_YY6(0U);goto _LL0;case 504:
# 3038 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 505:
# 3041
 yyval=Cyc_YY6(1U);goto _LL0;case 506:
# 3042 "parse.y"
 yyval=Cyc_YY6(3U);goto _LL0;case 507:
# 3043 "parse.y"
 yyval=Cyc_YY6(4U);goto _LL0;case 508: _T13E1=yyyvsp;_T13E2=_T13E1[0];
# 3047 "parse.y"
yyval=_T13E2.v;goto _LL0;case 509: _T13E3=yyyvsp;_T13E4=& _T13E3[1].v;_T13E5=(union Cyc_YYSTYPE*)_T13E4;_T13E6=
# 3049 "parse.y"
Cyc_yyget_YY38(_T13E5);_T13E7=yyyvsp;_T13E8=_T13E7[1];_T13E9=_T13E8.l;_T13EA=_T13E9.first_line;_T13EB=Cyc_Position_loc_to_seg(_T13EA);{void*t=Cyc_Parse_type_name_to_type(_T13E6,_T13EB);_T13EC=t;_T13ED=yyyvsp;_T13EE=& _T13ED[3].v;_T13EF=(union Cyc_YYSTYPE*)_T13EE;_T13F0=
Cyc_yyget_Exp_tok(_T13EF);_T13F1=yyyvsp;_T13F2=_T13F1[0];_T13F3=_T13F2.l;_T13F4=_T13F3.first_line;_T13F5=Cyc_Position_loc_to_seg(_T13F4);_T13F6=Cyc_Absyn_cast_exp(_T13EC,_T13F0,1,0U,_T13F5);yyval=Cyc_Exp_tok(_T13F6);goto _LL0;}case 510: _T13F7=yyyvsp;_T13F8=_T13F7[0];
# 3054 "parse.y"
yyval=_T13F8.v;goto _LL0;case 511: _T13F9=yyyvsp;_T13FA=& _T13F9[1].v;_T13FB=(union Cyc_YYSTYPE*)_T13FA;_T13FC=
# 3055 "parse.y"
Cyc_yyget_Exp_tok(_T13FB);_T13FD=yyyvsp;_T13FE=_T13FD[0];_T13FF=_T13FE.l;_T1400=_T13FF.first_line;_T1401=Cyc_Position_loc_to_seg(_T1400);_T1402=Cyc_Absyn_increment_exp(_T13FC,0U,_T1401);yyval=Cyc_Exp_tok(_T1402);goto _LL0;case 512: _T1403=yyyvsp;_T1404=& _T1403[1].v;_T1405=(union Cyc_YYSTYPE*)_T1404;_T1406=
# 3056 "parse.y"
Cyc_yyget_Exp_tok(_T1405);_T1407=yyyvsp;_T1408=_T1407[0];_T1409=_T1408.l;_T140A=_T1409.first_line;_T140B=Cyc_Position_loc_to_seg(_T140A);_T140C=Cyc_Absyn_increment_exp(_T1406,2U,_T140B);yyval=Cyc_Exp_tok(_T140C);goto _LL0;case 513: _T140D=yyyvsp;_T140E=& _T140D[1].v;_T140F=(union Cyc_YYSTYPE*)_T140E;_T1410=
# 3057 "parse.y"
Cyc_yyget_Exp_tok(_T140F);_T1411=yyyvsp;_T1412=_T1411[0];_T1413=_T1412.l;_T1414=_T1413.first_line;_T1415=Cyc_Position_loc_to_seg(_T1414);_T1416=Cyc_Absyn_address_exp(_T1410,_T1415);yyval=Cyc_Exp_tok(_T1416);goto _LL0;case 514: _T1417=yyyvsp;_T1418=& _T1417[1].v;_T1419=(union Cyc_YYSTYPE*)_T1418;_T141A=
# 3058 "parse.y"
Cyc_yyget_Exp_tok(_T1419);_T141B=yyyvsp;_T141C=_T141B[0];_T141D=_T141C.l;_T141E=_T141D.first_line;_T141F=Cyc_Position_loc_to_seg(_T141E);_T1420=Cyc_Absyn_deref_exp(_T141A,_T141F);yyval=Cyc_Exp_tok(_T1420);goto _LL0;case 515: _T1421=yyyvsp;_T1422=& _T1421[0].v;_T1423=(union Cyc_YYSTYPE*)_T1422;_T1424=
# 3059 "parse.y"
Cyc_yyget_YY6(_T1423);_T1425=yyyvsp;_T1426=& _T1425[1].v;_T1427=(union Cyc_YYSTYPE*)_T1426;_T1428=Cyc_yyget_Exp_tok(_T1427);_T1429=yyyvsp;_T142A=_T1429[0];_T142B=_T142A.l;_T142C=_T142B.first_line;_T142D=Cyc_Position_loc_to_seg(_T142C);_T142E=Cyc_Absyn_prim1_exp(_T1424,_T1428,_T142D);yyval=Cyc_Exp_tok(_T142E);goto _LL0;case 516: _T142F=yyyvsp;_T1430=& _T142F[2].v;_T1431=(union Cyc_YYSTYPE*)_T1430;_T1432=
# 3061 "parse.y"
Cyc_yyget_YY38(_T1431);_T1433=yyyvsp;_T1434=_T1433[2];_T1435=_T1434.l;_T1436=_T1435.first_line;_T1437=Cyc_Position_loc_to_seg(_T1436);{void*t=Cyc_Parse_type_name_to_type(_T1432,_T1437);_T1438=t;_T1439=yyyvsp;_T143A=_T1439[0];_T143B=_T143A.l;_T143C=_T143B.first_line;_T143D=
Cyc_Position_loc_to_seg(_T143C);_T143E=Cyc_Absyn_sizeoftype_exp(_T1438,_T143D);yyval=Cyc_Exp_tok(_T143E);goto _LL0;}case 517: _T143F=yyyvsp;_T1440=& _T143F[1].v;_T1441=(union Cyc_YYSTYPE*)_T1440;_T1442=
# 3064 "parse.y"
Cyc_yyget_Exp_tok(_T1441);_T1443=yyyvsp;_T1444=_T1443[0];_T1445=_T1444.l;_T1446=_T1445.first_line;_T1447=Cyc_Position_loc_to_seg(_T1446);_T1448=Cyc_Absyn_sizeofexp_exp(_T1442,_T1447);yyval=Cyc_Exp_tok(_T1448);goto _LL0;case 518: _T1449=yyyvsp;_T144A=& _T1449[2].v;_T144B=(union Cyc_YYSTYPE*)_T144A;_T144C=
# 3066 "parse.y"
Cyc_yyget_YY38(_T144B);_T144D=yyyvsp;_T144E=_T144D[2];_T144F=_T144E.l;_T1450=_T144F.first_line;_T1451=Cyc_Position_loc_to_seg(_T1450);{void*t=Cyc_Parse_type_name_to_type(_T144C,_T1451);_T1452=t;_T1453=yyyvsp;_T1454=& _T1453[4].v;_T1455=(union Cyc_YYSTYPE*)_T1454;_T1456=
Cyc_yyget_YY3(_T1455);_T1457=Cyc_List_imp_rev(_T1456);_T1458=yyyvsp;_T1459=_T1458[0];_T145A=_T1459.l;_T145B=_T145A.first_line;_T145C=Cyc_Position_loc_to_seg(_T145B);_T145D=Cyc_Absyn_offsetof_exp(_T1452,_T1457,_T145C);yyval=Cyc_Exp_tok(_T145D);goto _LL0;}case 519:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187A->tag=33;
# 3071
_T187A->f1.mknd=0U;_T187A->f1.rgn=0;_T187A->f1.aqual=0;_T187A->f1.elt_type=0;_T145F=yyyvsp;_T1460=& _T145F[2].v;_T1461=(union Cyc_YYSTYPE*)_T1460;_T187A->f1.num_elts=Cyc_yyget_Exp_tok(_T1461);_T187A->f1.fat_result=0;_T187A->f1.inline_call=0;_T145E=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187A;}_T1462=(void*)_T145E;_T1463=yyyvsp;_T1464=_T1463[0];_T1465=_T1464.l;_T1466=_T1465.first_line;_T1467=
Cyc_Position_loc_to_seg(_T1466);_T1468=
# 3071
Cyc_Absyn_new_exp(_T1462,_T1467);yyval=Cyc_Exp_tok(_T1468);goto _LL0;case 520:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187A->tag=33;
# 3074 "parse.y"
_T187A->f1.mknd=0U;_T146A=yyyvsp;_T146B=& _T146A[2].v;_T146C=(union Cyc_YYSTYPE*)_T146B;_T187A->f1.rgn=Cyc_yyget_Exp_tok(_T146C);_T146D=yyyvsp;_T146E=& _T146D[4].v;_T146F=(union Cyc_YYSTYPE*)_T146E;_T187A->f1.aqual=Cyc_yyget_Exp_tok(_T146F);_T187A->f1.elt_type=0;_T1470=yyyvsp;_T1471=& _T1470[6].v;_T1472=(union Cyc_YYSTYPE*)_T1471;_T187A->f1.num_elts=Cyc_yyget_Exp_tok(_T1472);_T187A->f1.fat_result=0;_T187A->f1.inline_call=0;_T1469=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187A;}_T1473=(void*)_T1469;_T1474=yyyvsp;_T1475=_T1474[0];_T1476=_T1475.l;_T1477=_T1476.first_line;_T1478=
Cyc_Position_loc_to_seg(_T1477);_T1479=
# 3074
Cyc_Absyn_new_exp(_T1473,_T1478);yyval=Cyc_Exp_tok(_T1479);goto _LL0;case 521:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187A->tag=33;
# 3077 "parse.y"
_T187A->f1.mknd=0U;_T147B=yyyvsp;_T147C=& _T147B[2].v;_T147D=(union Cyc_YYSTYPE*)_T147C;_T187A->f1.rgn=Cyc_yyget_Exp_tok(_T147D);_T187A->f1.aqual=0;_T187A->f1.elt_type=0;_T147E=yyyvsp;_T147F=& _T147E[4].v;_T1480=(union Cyc_YYSTYPE*)_T147F;_T187A->f1.num_elts=Cyc_yyget_Exp_tok(_T1480);_T187A->f1.fat_result=0;_T187A->f1.inline_call=0;_T147A=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187A;}_T1481=(void*)_T147A;_T1482=yyyvsp;_T1483=_T1482[0];_T1484=_T1483.l;_T1485=_T1484.first_line;_T1486=
Cyc_Position_loc_to_seg(_T1485);_T1487=
# 3077
Cyc_Absyn_new_exp(_T1481,_T1486);yyval=Cyc_Exp_tok(_T1487);goto _LL0;case 522: _T1488=yyyvsp;_T1489=& _T1488[2].v;_T148A=(union Cyc_YYSTYPE*)_T1489;_T148B=
# 3080 "parse.y"
Cyc_yyget_Exp_tok(_T148A);{struct _tuple16 _T187A=Cyc_Parse_split_seq(_T148B);struct Cyc_Absyn_Exp*_T187B;struct Cyc_Absyn_Exp*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_Absyn_Exp*rgn=_T187C;struct Cyc_Absyn_Exp*qual=_T187B;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187D->tag=33;
_T187D->f1.mknd=2U;_T187D->f1.rgn=rgn;_T187D->f1.aqual=qual;_T187D->f1.elt_type=0;_T148D=yyyvsp;_T148E=& _T148D[4].v;_T148F=(union Cyc_YYSTYPE*)_T148E;_T187D->f1.num_elts=Cyc_yyget_Exp_tok(_T148F);_T187D->f1.fat_result=0;_T187D->f1.inline_call=0;_T148C=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187D;}_T1490=(void*)_T148C;_T1491=yyyvsp;_T1492=_T1491[0];_T1493=_T1492.l;_T1494=_T1493.first_line;_T1495=
Cyc_Position_loc_to_seg(_T1494);_T1496=
# 3081
Cyc_Absyn_new_exp(_T1490,_T1495);yyval=Cyc_Exp_tok(_T1496);goto _LL0;}}case 523: _T1497=yyyvsp;_T1498=& _T1497[2].v;_T1499=(union Cyc_YYSTYPE*)_T1498;_T149A=
# 3084 "parse.y"
Cyc_yyget_Exp_tok(_T1499);{struct _tuple16 _T187A=Cyc_Parse_split_seq(_T149A);struct Cyc_Absyn_Exp*_T187B;struct Cyc_Absyn_Exp*_T187C;_T187C=_T187A.f0;_T187B=_T187A.f1;{struct Cyc_Absyn_Exp*rgn=_T187C;struct Cyc_Absyn_Exp*qual=_T187B;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187D->tag=33;
_T187D->f1.mknd=0U;_T187D->f1.rgn=rgn;_T187D->f1.aqual=qual;_T187D->f1.elt_type=0;_T149C=yyyvsp;_T149D=& _T149C[4].v;_T149E=(union Cyc_YYSTYPE*)_T149D;_T187D->f1.num_elts=Cyc_yyget_Exp_tok(_T149E);_T187D->f1.fat_result=0;_T187D->f1.inline_call=1;_T149B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187D;}_T149F=(void*)_T149B;_T14A0=yyyvsp;_T14A1=_T14A0[0];_T14A2=_T14A1.l;_T14A3=_T14A2.first_line;_T14A4=
Cyc_Position_loc_to_seg(_T14A3);_T14A5=
# 3085
Cyc_Absyn_new_exp(_T149F,_T14A4);yyval=Cyc_Exp_tok(_T14A5);goto _LL0;}}case 524: _T14A6=yyyvsp;_T14A7=& _T14A6[6].v;_T14A8=(union Cyc_YYSTYPE*)_T14A7;_T14A9=
# 3088 "parse.y"
Cyc_yyget_YY38(_T14A8);_T14AA=yyyvsp;_T14AB=_T14AA[6];_T14AC=_T14AB.l;_T14AD=_T14AC.first_line;_T14AE=Cyc_Position_loc_to_seg(_T14AD);{void*t=Cyc_Parse_type_name_to_type(_T14A9,_T14AE);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187A->tag=33;
_T187A->f1.mknd=1U;_T187A->f1.rgn=0;_T187A->f1.aqual=0;{void**_T187B=_cycalloc(sizeof(void*));*_T187B=t;_T14B0=(void**)_T187B;}_T187A->f1.elt_type=_T14B0;_T14B1=yyyvsp;_T14B2=& _T14B1[2].v;_T14B3=(union Cyc_YYSTYPE*)_T14B2;_T187A->f1.num_elts=Cyc_yyget_Exp_tok(_T14B3);_T187A->f1.fat_result=0;_T187A->f1.inline_call=0;_T14AF=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187A;}_T14B4=(void*)_T14AF;_T14B5=yyyvsp;_T14B6=_T14B5[0];_T14B7=_T14B6.l;_T14B8=_T14B7.first_line;_T14B9=
Cyc_Position_loc_to_seg(_T14B8);_T14BA=
# 3089
Cyc_Absyn_new_exp(_T14B4,_T14B9);yyval=Cyc_Exp_tok(_T14BA);goto _LL0;}case 525: _T14BB=yyyvsp;_T14BC=& _T14BB[10].v;_T14BD=(union Cyc_YYSTYPE*)_T14BC;_T14BE=
# 3092 "parse.y"
Cyc_yyget_YY38(_T14BD);_T14BF=yyyvsp;_T14C0=_T14BF[10];_T14C1=_T14C0.l;_T14C2=_T14C1.first_line;_T14C3=Cyc_Position_loc_to_seg(_T14C2);{void*t=Cyc_Parse_type_name_to_type(_T14BE,_T14C3);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187A->tag=33;
_T187A->f1.mknd=1U;_T14C5=yyyvsp;_T14C6=& _T14C5[2].v;_T14C7=(union Cyc_YYSTYPE*)_T14C6;_T187A->f1.rgn=Cyc_yyget_Exp_tok(_T14C7);_T14C8=yyyvsp;_T14C9=& _T14C8[4].v;_T14CA=(union Cyc_YYSTYPE*)_T14C9;_T187A->f1.aqual=Cyc_yyget_Exp_tok(_T14CA);{void**_T187B=_cycalloc(sizeof(void*));*_T187B=t;_T14CB=(void**)_T187B;}_T187A->f1.elt_type=_T14CB;_T14CC=yyyvsp;_T14CD=& _T14CC[6].v;_T14CE=(union Cyc_YYSTYPE*)_T14CD;_T187A->f1.num_elts=Cyc_yyget_Exp_tok(_T14CE);_T187A->f1.fat_result=0;_T187A->f1.inline_call=0;_T14C4=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187A;}_T14CF=(void*)_T14C4;_T14D0=yyyvsp;_T14D1=_T14D0[0];_T14D2=_T14D1.l;_T14D3=_T14D2.first_line;_T14D4=
Cyc_Position_loc_to_seg(_T14D3);_T14D5=
# 3093
Cyc_Absyn_new_exp(_T14CF,_T14D4);yyval=Cyc_Exp_tok(_T14D5);goto _LL0;}case 526: _T14D6=yyyvsp;_T14D7=& _T14D6[8].v;_T14D8=(union Cyc_YYSTYPE*)_T14D7;_T14D9=
# 3097
Cyc_yyget_YY38(_T14D8);_T14DA=yyyvsp;_T14DB=_T14DA[8];_T14DC=_T14DB.l;_T14DD=_T14DC.first_line;_T14DE=Cyc_Position_loc_to_seg(_T14DD);{void*t=Cyc_Parse_type_name_to_type(_T14D9,_T14DE);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187A->tag=33;
_T187A->f1.mknd=1U;_T14E0=yyyvsp;_T14E1=& _T14E0[2].v;_T14E2=(union Cyc_YYSTYPE*)_T14E1;_T187A->f1.rgn=Cyc_yyget_Exp_tok(_T14E2);_T187A->f1.aqual=0;{void**_T187B=_cycalloc(sizeof(void*));*_T187B=t;_T14E3=(void**)_T187B;}_T187A->f1.elt_type=_T14E3;_T14E4=yyyvsp;_T14E5=& _T14E4[4].v;_T14E6=(union Cyc_YYSTYPE*)_T14E5;_T187A->f1.num_elts=Cyc_yyget_Exp_tok(_T14E6);_T187A->f1.fat_result=0;_T187A->f1.inline_call=0;_T14DF=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187A;}_T14E7=(void*)_T14DF;_T14E8=yyyvsp;_T14E9=_T14E8[0];_T14EA=_T14E9.l;_T14EB=_T14EA.first_line;_T14EC=
Cyc_Position_loc_to_seg(_T14EB);_T14ED=
# 3098
Cyc_Absyn_new_exp(_T14E7,_T14EC);yyval=Cyc_Exp_tok(_T14ED);goto _LL0;}case 527:{struct Cyc_Absyn_Exp*_T187A[1];_T14EF=yyyvsp;_T14F0=& _T14EF[2].v;_T14F1=(union Cyc_YYSTYPE*)_T14F0;_T187A[0]=
# 3101 "parse.y"
Cyc_yyget_Exp_tok(_T14F1);_T14F2=_tag_fat(_T187A,sizeof(struct Cyc_Absyn_Exp*),1);_T14EE=Cyc_List_list(_T14F2);}_T14F3=yyyvsp;_T14F4=_T14F3[0];_T14F5=_T14F4.l;_T14F6=_T14F5.first_line;_T14F7=Cyc_Position_loc_to_seg(_T14F6);_T14F8=Cyc_Absyn_primop_exp(18U,_T14EE,_T14F7);yyval=Cyc_Exp_tok(_T14F8);goto _LL0;case 528: _T14F9=yyyvsp;_T14FA=& _T14F9[2].v;_T14FB=(union Cyc_YYSTYPE*)_T14FA;_T14FC=
# 3103 "parse.y"
Cyc_yyget_Exp_tok(_T14FB);_T14FD=yyyvsp;_T14FE=_T14FD[0];_T14FF=_T14FE.l;_T1500=_T14FF.first_line;_T1501=Cyc_Position_loc_to_seg(_T1500);_T1502=Cyc_Absyn_tagof_exp(_T14FC,_T1501);yyval=Cyc_Exp_tok(_T1502);goto _LL0;case 529:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T187A->tag=37;_T1504=yyyvsp;_T1505=& _T1504[2].v;_T1506=(union Cyc_YYSTYPE*)_T1505;
# 3105 "parse.y"
_T187A->f1=Cyc_yyget_Exp_tok(_T1506);{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T1508=yyyvsp;_T1509=& _T1508[4].v;_T150A=(union Cyc_YYSTYPE*)_T1509;*_T187B=Cyc_yyget_String_tok(_T150A);_T1507=(struct _fat_ptr*)_T187B;}_T187A->f2=_T1507;_T1503=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T187A;}_T150B=(void*)_T1503;_T150C=yyyvsp;_T150D=_T150C[0];_T150E=_T150D.l;_T150F=_T150E.first_line;_T1510=Cyc_Position_loc_to_seg(_T150F);_T1511=Cyc_Absyn_new_exp(_T150B,_T1510);yyval=Cyc_Exp_tok(_T1511);goto _LL0;case 530:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T187A->tag=37;_T1513=yyyvsp;_T1514=& _T1513[2].v;_T1515=(union Cyc_YYSTYPE*)_T1514;_T1516=
# 3107 "parse.y"
Cyc_yyget_Exp_tok(_T1515);_T1517=yyyvsp;_T1518=_T1517[2];_T1519=_T1518.l;_T151A=_T1519.first_line;_T151B=Cyc_Position_loc_to_seg(_T151A);_T187A->f1=Cyc_Absyn_deref_exp(_T1516,_T151B);{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T151D=yyyvsp;_T151E=& _T151D[4].v;_T151F=(union Cyc_YYSTYPE*)_T151E;*_T187B=Cyc_yyget_String_tok(_T151F);_T151C=(struct _fat_ptr*)_T187B;}_T187A->f2=_T151C;_T1512=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T187A;}_T1520=(void*)_T1512;_T1521=yyyvsp;_T1522=_T1521[0];_T1523=_T1522.l;_T1524=_T1523.first_line;_T1525=Cyc_Position_loc_to_seg(_T1524);_T1526=Cyc_Absyn_new_exp(_T1520,_T1525);yyval=Cyc_Exp_tok(_T1526);goto _LL0;case 531: _T1527=yyyvsp;_T1528=& _T1527[2].v;_T1529=(union Cyc_YYSTYPE*)_T1528;_T152A=
# 3109 "parse.y"
Cyc_yyget_YY38(_T1529);_T152B=yyyvsp;_T152C=_T152B[2];_T152D=_T152C.l;_T152E=_T152D.first_line;_T152F=Cyc_Position_loc_to_seg(_T152E);{void*t=Cyc_Parse_type_name_to_type(_T152A,_T152F);_T1530=t;_T1531=yyyvsp;_T1532=_T1531[0];_T1533=_T1532.l;_T1534=_T1533.first_line;_T1535=
Cyc_Position_loc_to_seg(_T1534);_T1536=Cyc_Absyn_valueof_exp(_T1530,_T1535);yyval=Cyc_Exp_tok(_T1536);goto _LL0;}case 532: _T1537=yyyvsp;_T1538=& _T1537[1].v;_T1539=(union Cyc_YYSTYPE*)_T1538;_T153A=
# 3111 "parse.y"
Cyc_yyget_YY63(_T1539);_T153B=yyyvsp;_T153C=_T153B[0];_T153D=_T153C.l;_T153E=_T153D.first_line;_T153F=Cyc_Position_loc_to_seg(_T153E);_T1540=Cyc_Absyn_new_exp(_T153A,_T153F);yyval=Cyc_Exp_tok(_T1540);goto _LL0;case 533: _T1541=yyyvsp;_T1542=& _T1541[1].v;_T1543=(union Cyc_YYSTYPE*)_T1542;_T1544=
# 3112 "parse.y"
Cyc_yyget_Exp_tok(_T1543);_T1545=yyyvsp;_T1546=_T1545[0];_T1547=_T1546.l;_T1548=_T1547.first_line;_T1549=Cyc_Position_loc_to_seg(_T1548);_T154A=Cyc_Absyn_extension_exp(_T1544,_T1549);yyval=Cyc_Exp_tok(_T154A);goto _LL0;case 534: _T154B=yyyvsp;_T154C=& _T154B[2].v;_T154D=(union Cyc_YYSTYPE*)_T154C;_T154E=
# 3113 "parse.y"
Cyc_yyget_Exp_tok(_T154D);_T154F=yyyvsp;_T1550=_T154F[0];_T1551=_T1550.l;_T1552=_T1551.first_line;_T1553=Cyc_Position_loc_to_seg(_T1552);_T1554=Cyc_Absyn_assert_exp(_T154E,0,_T1553);yyval=Cyc_Exp_tok(_T1554);goto _LL0;case 535: _T1555=yyyvsp;_T1556=& _T1555[2].v;_T1557=(union Cyc_YYSTYPE*)_T1556;_T1558=
# 3114 "parse.y"
Cyc_yyget_Exp_tok(_T1557);_T1559=yyyvsp;_T155A=_T1559[0];_T155B=_T155A.l;_T155C=_T155B.first_line;_T155D=Cyc_Position_loc_to_seg(_T155C);_T155E=Cyc_Absyn_assert_exp(_T1558,1,_T155D);yyval=Cyc_Exp_tok(_T155E);goto _LL0;case 536: _T155F=yyyvsp;_T1560=& _T155F[2].v;_T1561=(union Cyc_YYSTYPE*)_T1560;_T1562=
# 3115 "parse.y"
Cyc_yyget_Exp_tok(_T1561);_T1563=yyyvsp;_T1564=_T1563[0];_T1565=_T1564.l;_T1566=_T1565.first_line;_T1567=Cyc_Position_loc_to_seg(_T1566);_T1568=Cyc_Absyn_assert_false_exp(_T1562,_T1567);yyval=Cyc_Exp_tok(_T1568);goto _LL0;case 537:
# 3119 "parse.y"
 yyval=Cyc_YY6(12U);goto _LL0;case 538:
# 3120 "parse.y"
 yyval=Cyc_YY6(11U);goto _LL0;case 539:
# 3121 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 540:
# 3122 "parse.y"
 yyval=Cyc_YY6(0U);goto _LL0;case 541: _T1569=yyyvsp;_T156A=& _T1569[3].v;_T156B=(union Cyc_YYSTYPE*)_T156A;{
# 3127 "parse.y"
struct _tuple31*_T187A=Cyc_yyget_YY64(_T156B);struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;struct Cyc_List_List*_T187D;{struct _tuple31 _T187E=*_T187A;_T187D=_T187E.f0;_T187C=_T187E.f1;_T187B=_T187E.f2;}{struct Cyc_List_List*outlist=_T187D;struct Cyc_List_List*inlist=_T187C;struct Cyc_List_List*clobbers=_T187B;{struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T187E=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_T187E->tag=39;_T156D=yyyvsp;_T156E=& _T156D[0].v;_T156F=(union Cyc_YYSTYPE*)_T156E;
_T187E->f1=Cyc_yyget_YY32(_T156F);_T1570=yyyvsp;_T1571=& _T1570[2].v;_T1572=(union Cyc_YYSTYPE*)_T1571;_T187E->f2=Cyc_yyget_String_tok(_T1572);_T187E->f3=outlist;_T187E->f4=inlist;_T187E->f5=clobbers;_T156C=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_T187E;}_T1573=(void*)_T156C;yyval=Cyc_YY63(_T1573);goto _LL0;}}case 542:
# 3132 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 543:
# 3133 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 544:{struct _tuple31*_T187A=_cycalloc(sizeof(struct _tuple31));
# 3137 "parse.y"
_T187A->f0=0;_T187A->f1=0;_T187A->f2=0;_T1574=(struct _tuple31*)_T187A;}yyval=Cyc_YY64(_T1574);goto _LL0;case 545: _T1575=yyyvsp;_T1576=& _T1575[1].v;_T1577=(union Cyc_YYSTYPE*)_T1576;{
# 3139 "parse.y"
struct _tuple28*_T187A=Cyc_yyget_YY65(_T1577);struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;{struct _tuple28 _T187D=*_T187A;_T187C=_T187D.f0;_T187B=_T187D.f1;}{struct Cyc_List_List*inlist=_T187C;struct Cyc_List_List*clobbers=_T187B;{struct _tuple31*_T187D=_cycalloc(sizeof(struct _tuple31));
_T187D->f0=0;_T187D->f1=inlist;_T187D->f2=clobbers;_T1578=(struct _tuple31*)_T187D;}yyval=Cyc_YY64(_T1578);goto _LL0;}}case 546: _T1579=yyyvsp;_T157A=& _T1579[2].v;_T157B=(union Cyc_YYSTYPE*)_T157A;{
# 3142 "parse.y"
struct _tuple28*_T187A=Cyc_yyget_YY65(_T157B);struct Cyc_List_List*_T187B;struct Cyc_List_List*_T187C;{struct _tuple28 _T187D=*_T187A;_T187C=_T187D.f0;_T187B=_T187D.f1;}{struct Cyc_List_List*inlist=_T187C;struct Cyc_List_List*clobbers=_T187B;{struct _tuple31*_T187D=_cycalloc(sizeof(struct _tuple31));_T157D=yyyvsp;_T157E=& _T157D[1].v;_T157F=(union Cyc_YYSTYPE*)_T157E;_T1580=
Cyc_yyget_YY67(_T157F);_T187D->f0=Cyc_List_imp_rev(_T1580);_T187D->f1=inlist;_T187D->f2=clobbers;_T157C=(struct _tuple31*)_T187D;}yyval=Cyc_YY64(_T157C);goto _LL0;}}case 547:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T1582=yyyvsp;_T1583=& _T1582[0].v;_T1584=(union Cyc_YYSTYPE*)_T1583;
# 3146
_T187A->hd=Cyc_yyget_YY68(_T1584);_T187A->tl=0;_T1581=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY67(_T1581);goto _LL0;case 548:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T1586=yyyvsp;_T1587=& _T1586[2].v;_T1588=(union Cyc_YYSTYPE*)_T1587;
# 3147 "parse.y"
_T187A->hd=Cyc_yyget_YY68(_T1588);_T1589=yyyvsp;_T158A=& _T1589[0].v;_T158B=(union Cyc_YYSTYPE*)_T158A;_T187A->tl=Cyc_yyget_YY67(_T158B);_T1585=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY67(_T1585);goto _LL0;case 549:{struct _tuple28*_T187A=_cycalloc(sizeof(struct _tuple28));
# 3151 "parse.y"
_T187A->f0=0;_T187A->f1=0;_T158C=(struct _tuple28*)_T187A;}yyval=Cyc_YY65(_T158C);goto _LL0;case 550:{struct _tuple28*_T187A=_cycalloc(sizeof(struct _tuple28));
# 3152 "parse.y"
_T187A->f0=0;_T158E=yyyvsp;_T158F=& _T158E[1].v;_T1590=(union Cyc_YYSTYPE*)_T158F;_T187A->f1=Cyc_yyget_YY66(_T1590);_T158D=(struct _tuple28*)_T187A;}yyval=Cyc_YY65(_T158D);goto _LL0;case 551:{struct _tuple28*_T187A=_cycalloc(sizeof(struct _tuple28));_T1592=yyyvsp;_T1593=& _T1592[1].v;_T1594=(union Cyc_YYSTYPE*)_T1593;_T1595=
# 3153 "parse.y"
Cyc_yyget_YY67(_T1594);_T187A->f0=Cyc_List_imp_rev(_T1595);_T1596=yyyvsp;_T1597=& _T1596[2].v;_T1598=(union Cyc_YYSTYPE*)_T1597;_T187A->f1=Cyc_yyget_YY66(_T1598);_T1591=(struct _tuple28*)_T187A;}yyval=Cyc_YY65(_T1591);goto _LL0;case 552:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T159A=yyyvsp;_T159B=& _T159A[0].v;_T159C=(union Cyc_YYSTYPE*)_T159B;
# 3156
_T187A->hd=Cyc_yyget_YY68(_T159C);_T187A->tl=0;_T1599=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY67(_T1599);goto _LL0;case 553:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T159E=yyyvsp;_T159F=& _T159E[2].v;_T15A0=(union Cyc_YYSTYPE*)_T159F;
# 3157 "parse.y"
_T187A->hd=Cyc_yyget_YY68(_T15A0);_T15A1=yyyvsp;_T15A2=& _T15A1[0].v;_T15A3=(union Cyc_YYSTYPE*)_T15A2;_T187A->tl=Cyc_yyget_YY67(_T15A3);_T159D=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY67(_T159D);goto _LL0;case 554:{struct _tuple32*_T187A=_cycalloc(sizeof(struct _tuple32));_T15A5=yyyvsp;_T15A6=& _T15A5[0].v;_T15A7=(union Cyc_YYSTYPE*)_T15A6;
# 3161 "parse.y"
_T187A->f0=Cyc_yyget_String_tok(_T15A7);_T15A8=yyyvsp;_T15A9=& _T15A8[2].v;_T15AA=(union Cyc_YYSTYPE*)_T15A9;_T187A->f1=Cyc_yyget_Exp_tok(_T15AA);_T15A4=(struct _tuple32*)_T187A;}yyval=Cyc_YY68(_T15A4);goto _LL0;case 555:
# 3166 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 556:
# 3167 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 557: _T15AB=yyyvsp;_T15AC=& _T15AB[1].v;_T15AD=(union Cyc_YYSTYPE*)_T15AC;_T15AE=
# 3168 "parse.y"
Cyc_yyget_YY66(_T15AD);_T15AF=Cyc_List_imp_rev(_T15AE);yyval=Cyc_YY66(_T15AF);goto _LL0;case 558:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T15B2=yyyvsp;_T15B3=& _T15B2[0].v;_T15B4=(union Cyc_YYSTYPE*)_T15B3;
# 3171
*_T187B=Cyc_yyget_String_tok(_T15B4);_T15B1=(struct _fat_ptr*)_T187B;}_T187A->hd=_T15B1;_T187A->tl=0;_T15B0=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY66(_T15B0);goto _LL0;case 559:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T15B7=yyyvsp;_T15B8=& _T15B7[2].v;_T15B9=(union Cyc_YYSTYPE*)_T15B8;
# 3172 "parse.y"
*_T187B=Cyc_yyget_String_tok(_T15B9);_T15B6=(struct _fat_ptr*)_T187B;}_T187A->hd=_T15B6;_T15BA=yyyvsp;_T15BB=& _T15BA[0].v;_T15BC=(union Cyc_YYSTYPE*)_T15BB;_T187A->tl=Cyc_yyget_YY66(_T15BC);_T15B5=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY66(_T15B5);goto _LL0;case 560: _T15BD=yyyvsp;_T15BE=_T15BD[0];
# 3177 "parse.y"
yyval=_T15BE.v;goto _LL0;case 561: _T15BF=yyyvsp;_T15C0=& _T15BF[0].v;_T15C1=(union Cyc_YYSTYPE*)_T15C0;_T15C2=
# 3179 "parse.y"
Cyc_yyget_Exp_tok(_T15C1);_T15C3=yyyvsp;_T15C4=& _T15C3[2].v;_T15C5=(union Cyc_YYSTYPE*)_T15C4;_T15C6=Cyc_yyget_Exp_tok(_T15C5);_T15C7=yyyvsp;_T15C8=_T15C7[0];_T15C9=_T15C8.l;_T15CA=_T15C9.first_line;_T15CB=Cyc_Position_loc_to_seg(_T15CA);_T15CC=Cyc_Absyn_subscript_exp(_T15C2,_T15C6,_T15CB);yyval=Cyc_Exp_tok(_T15CC);goto _LL0;case 562: _T15CD=yyyvsp;_T15CE=& _T15CD[0].v;_T15CF=(union Cyc_YYSTYPE*)_T15CE;_T15D0=
# 3181 "parse.y"
Cyc_yyget_Exp_tok(_T15CF);_T15D1=yyyvsp;_T15D2=_T15D1[0];_T15D3=_T15D2.l;_T15D4=_T15D3.first_line;_T15D5=Cyc_Position_loc_to_seg(_T15D4);_T15D6=Cyc_Absyn_unknowncall_exp(_T15D0,0,_T15D5);yyval=Cyc_Exp_tok(_T15D6);goto _LL0;case 563: _T15D7=yyyvsp;_T15D8=& _T15D7[0].v;_T15D9=(union Cyc_YYSTYPE*)_T15D8;_T15DA=
# 3183 "parse.y"
Cyc_yyget_Exp_tok(_T15D9);_T15DB=yyyvsp;_T15DC=& _T15DB[2].v;_T15DD=(union Cyc_YYSTYPE*)_T15DC;_T15DE=Cyc_yyget_YY4(_T15DD);_T15DF=yyyvsp;_T15E0=_T15DF[0];_T15E1=_T15E0.l;_T15E2=_T15E1.first_line;_T15E3=Cyc_Position_loc_to_seg(_T15E2);_T15E4=Cyc_Absyn_unknowncall_exp(_T15DA,_T15DE,_T15E3);yyval=Cyc_Exp_tok(_T15E4);goto _LL0;case 564: _T15E5=yyyvsp;_T15E6=& _T15E5[0].v;_T15E7=(union Cyc_YYSTYPE*)_T15E6;_T15E8=
# 3185 "parse.y"
Cyc_yyget_Exp_tok(_T15E7);{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));_T15EA=yyyvsp;_T15EB=& _T15EA[2].v;_T15EC=(union Cyc_YYSTYPE*)_T15EB;*_T187A=Cyc_yyget_String_tok(_T15EC);_T15E9=(struct _fat_ptr*)_T187A;}_T15ED=yyyvsp;_T15EE=_T15ED[0];_T15EF=_T15EE.l;_T15F0=_T15EF.first_line;_T15F1=Cyc_Position_loc_to_seg(_T15F0);_T15F2=Cyc_Absyn_aggrmember_exp(_T15E8,_T15E9,_T15F1);yyval=Cyc_Exp_tok(_T15F2);goto _LL0;case 565: _T15F3=yyyvsp;_T15F4=& _T15F3[0].v;_T15F5=(union Cyc_YYSTYPE*)_T15F4;_T15F6=
# 3187 "parse.y"
Cyc_yyget_Exp_tok(_T15F5);{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));_T15F8=yyyvsp;_T15F9=& _T15F8[2].v;_T15FA=(union Cyc_YYSTYPE*)_T15F9;*_T187A=Cyc_yyget_String_tok(_T15FA);_T15F7=(struct _fat_ptr*)_T187A;}_T15FB=yyyvsp;_T15FC=_T15FB[0];_T15FD=_T15FC.l;_T15FE=_T15FD.first_line;_T15FF=Cyc_Position_loc_to_seg(_T15FE);_T1600=Cyc_Absyn_aggrarrow_exp(_T15F6,_T15F7,_T15FF);yyval=Cyc_Exp_tok(_T1600);goto _LL0;case 566: _T1601=yyyvsp;_T1602=& _T1601[0].v;_T1603=(union Cyc_YYSTYPE*)_T1602;_T1604=
# 3189 "parse.y"
Cyc_yyget_Exp_tok(_T1603);_T1605=yyyvsp;_T1606=_T1605[0];_T1607=_T1606.l;_T1608=_T1607.first_line;_T1609=Cyc_Position_loc_to_seg(_T1608);_T160A=Cyc_Absyn_increment_exp(_T1604,1U,_T1609);yyval=Cyc_Exp_tok(_T160A);goto _LL0;case 567: _T160B=yyyvsp;_T160C=& _T160B[0].v;_T160D=(union Cyc_YYSTYPE*)_T160C;_T160E=
# 3191 "parse.y"
Cyc_yyget_Exp_tok(_T160D);_T160F=yyyvsp;_T1610=_T160F[0];_T1611=_T1610.l;_T1612=_T1611.first_line;_T1613=Cyc_Position_loc_to_seg(_T1612);_T1614=Cyc_Absyn_increment_exp(_T160E,3U,_T1613);yyval=Cyc_Exp_tok(_T1614);goto _LL0;case 568:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T187A->tag=24;_T1616=yyyvsp;_T1617=& _T1616[1].v;_T1618=(union Cyc_YYSTYPE*)_T1617;
# 3193 "parse.y"
_T187A->f1=Cyc_yyget_YY38(_T1618);_T187A->f2=0;_T1615=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T187A;}_T1619=(void*)_T1615;_T161A=yyyvsp;_T161B=_T161A[0];_T161C=_T161B.l;_T161D=_T161C.first_line;_T161E=Cyc_Position_loc_to_seg(_T161D);_T161F=Cyc_Absyn_new_exp(_T1619,_T161E);yyval=Cyc_Exp_tok(_T161F);goto _LL0;case 569:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T187A->tag=24;_T1621=yyyvsp;_T1622=& _T1621[1].v;_T1623=(union Cyc_YYSTYPE*)_T1622;
# 3195 "parse.y"
_T187A->f1=Cyc_yyget_YY38(_T1623);_T1624=yyyvsp;_T1625=& _T1624[4].v;_T1626=(union Cyc_YYSTYPE*)_T1625;_T1627=Cyc_yyget_YY5(_T1626);_T187A->f2=Cyc_List_imp_rev(_T1627);_T1620=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T187A;}_T1628=(void*)_T1620;_T1629=yyyvsp;_T162A=_T1629[0];_T162B=_T162A.l;_T162C=_T162B.first_line;_T162D=Cyc_Position_loc_to_seg(_T162C);_T162E=Cyc_Absyn_new_exp(_T1628,_T162D);yyval=Cyc_Exp_tok(_T162E);goto _LL0;case 570:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T187A->tag=24;_T1630=yyyvsp;_T1631=& _T1630[1].v;_T1632=(union Cyc_YYSTYPE*)_T1631;
# 3197 "parse.y"
_T187A->f1=Cyc_yyget_YY38(_T1632);_T1633=yyyvsp;_T1634=& _T1633[4].v;_T1635=(union Cyc_YYSTYPE*)_T1634;_T1636=Cyc_yyget_YY5(_T1635);_T187A->f2=Cyc_List_imp_rev(_T1636);_T162F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T187A;}_T1637=(void*)_T162F;_T1638=yyyvsp;_T1639=_T1638[0];_T163A=_T1639.l;_T163B=_T163A.first_line;_T163C=Cyc_Position_loc_to_seg(_T163B);_T163D=Cyc_Absyn_new_exp(_T1637,_T163C);yyval=Cyc_Exp_tok(_T163D);goto _LL0;case 571:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T187B->tag=0;{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));_T1641=yyyvsp;_T1642=& _T1641[0].v;_T1643=(union Cyc_YYSTYPE*)_T1642;
# 3202 "parse.y"
*_T187C=Cyc_yyget_String_tok(_T1643);_T1640=(struct _fat_ptr*)_T187C;}_T187B->f1=_T1640;_T163F=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T187B;}_T187A->hd=(void*)_T163F;_T187A->tl=0;_T163E=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY3(_T163E);goto _LL0;case 572: _T1644=yyyvsp;_T1645=_T1644[0];_T1646=_T1645.l;_T1647=_T1646.first_line;_T1648=
# 3205
Cyc_Position_loc_to_seg(_T1647);_T1649=yyyvsp;_T164A=& _T1649[0].v;_T164B=(union Cyc_YYSTYPE*)_T164A;_T164C=Cyc_yyget_Int_tok(_T164B);{unsigned i=Cyc_Parse_cnst2uint(_T1648,_T164C);_T164D=i;_T164E=(int)_T164D;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T187A=Cyc_Absyn_tuple_field_designator(_T164E);struct _fat_ptr*_T187B;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T187C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T187A;_T187B=_T187C->f1;}{struct _fat_ptr*f=_T187B;{struct Cyc_List_List*_T187C=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T187D->tag=0;
_T187D->f1=f;_T1650=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T187D;}_T187C->hd=(void*)_T1650;_T187C->tl=0;_T164F=(struct Cyc_List_List*)_T187C;}yyval=Cyc_YY3(_T164F);goto _LL0;}}}case 573:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T187B->tag=0;{struct _fat_ptr*_T187C=_cycalloc(sizeof(struct _fat_ptr));_T1654=yyyvsp;_T1655=& _T1654[2].v;_T1656=(union Cyc_YYSTYPE*)_T1655;
# 3209 "parse.y"
*_T187C=Cyc_yyget_String_tok(_T1656);_T1653=(struct _fat_ptr*)_T187C;}_T187B->f1=_T1653;_T1652=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T187B;}_T187A->hd=(void*)_T1652;_T1657=yyyvsp;_T1658=& _T1657[0].v;_T1659=(union Cyc_YYSTYPE*)_T1658;_T187A->tl=Cyc_yyget_YY3(_T1659);_T1651=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY3(_T1651);goto _LL0;case 574: _T165A=yyyvsp;_T165B=_T165A[2];_T165C=_T165B.l;_T165D=_T165C.first_line;_T165E=
# 3212
Cyc_Position_loc_to_seg(_T165D);_T165F=yyyvsp;_T1660=& _T165F[2].v;_T1661=(union Cyc_YYSTYPE*)_T1660;_T1662=Cyc_yyget_Int_tok(_T1661);{unsigned i=Cyc_Parse_cnst2uint(_T165E,_T1662);_T1663=i;_T1664=(int)_T1663;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T187A=Cyc_Absyn_tuple_field_designator(_T1664);struct _fat_ptr*_T187B;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T187C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T187A;_T187B=_T187C->f1;}{struct _fat_ptr*f=_T187B;{struct Cyc_List_List*_T187C=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T187D->tag=0;
_T187D->f1=f;_T1666=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T187D;}_T187C->hd=(void*)_T1666;_T1667=yyyvsp;_T1668=& _T1667[0].v;_T1669=(union Cyc_YYSTYPE*)_T1668;_T187C->tl=Cyc_yyget_YY3(_T1669);_T1665=(struct Cyc_List_List*)_T187C;}yyval=Cyc_YY3(_T1665);goto _LL0;}}}case 575: _T166A=yyyvsp;_T166B=& _T166A[0].v;_T166C=(union Cyc_YYSTYPE*)_T166B;_T166D=
# 3219 "parse.y"
Cyc_yyget_QualId_tok(_T166C);_T166E=yyyvsp;_T166F=_T166E[0];_T1670=_T166F.l;_T1671=_T1670.first_line;_T1672=Cyc_Position_loc_to_seg(_T1671);_T1673=Cyc_Absyn_unknownid_exp(_T166D,_T1672);yyval=Cyc_Exp_tok(_T1673);goto _LL0;case 576: _T1674=yyyvsp;_T1675=& _T1674[2].v;_T1676=(union Cyc_YYSTYPE*)_T1675;_T1677=
# 3220 "parse.y"
Cyc_yyget_String_tok(_T1676);_T1678=yyyvsp;_T1679=_T1678[0];_T167A=_T1679.l;_T167B=_T167A.first_line;_T167C=Cyc_Position_loc_to_seg(_T167B);_T167D=Cyc_Absyn_pragma_exp(_T1677,_T167C);yyval=Cyc_Exp_tok(_T167D);goto _LL0;case 577: _T167E=yyyvsp;_T167F=_T167E[0];
# 3221 "parse.y"
yyval=_T167F.v;goto _LL0;case 578: _T1680=yyyvsp;_T1681=& _T1680[0].v;_T1682=(union Cyc_YYSTYPE*)_T1681;_T1683=
# 3222 "parse.y"
Cyc_yyget_String_tok(_T1682);_T1684=yyyvsp;_T1685=_T1684[0];_T1686=_T1685.l;_T1687=_T1686.first_line;_T1688=Cyc_Position_loc_to_seg(_T1687);_T1689=Cyc_Absyn_string_exp(_T1683,_T1688);yyval=Cyc_Exp_tok(_T1689);goto _LL0;case 579: _T168A=yyyvsp;_T168B=& _T168A[0].v;_T168C=(union Cyc_YYSTYPE*)_T168B;_T168D=
# 3223 "parse.y"
Cyc_yyget_String_tok(_T168C);_T168E=yyyvsp;_T168F=_T168E[0];_T1690=_T168F.l;_T1691=_T1690.first_line;_T1692=Cyc_Position_loc_to_seg(_T1691);_T1693=Cyc_Absyn_wstring_exp(_T168D,_T1692);yyval=Cyc_Exp_tok(_T1693);goto _LL0;case 580: _T1694=yyyvsp;_T1695=_T1694[1];
# 3224 "parse.y"
yyval=_T1695.v;goto _LL0;case 581: _T1696=yyyvsp;_T1697=& _T1696[0].v;_T1698=(union Cyc_YYSTYPE*)_T1697;_T1699=
# 3228 "parse.y"
Cyc_yyget_Exp_tok(_T1698);_T169A=yyyvsp;_T169B=_T169A[0];_T169C=_T169B.l;_T169D=_T169C.first_line;_T169E=Cyc_Position_loc_to_seg(_T169D);_T169F=Cyc_Absyn_noinstantiate_exp(_T1699,_T169E);yyval=Cyc_Exp_tok(_T169F);goto _LL0;case 582: _T16A0=yyyvsp;_T16A1=& _T16A0[0].v;_T16A2=(union Cyc_YYSTYPE*)_T16A1;_T16A3=
# 3231
Cyc_yyget_Exp_tok(_T16A2);_T16A4=yyyvsp;_T16A5=& _T16A4[3].v;_T16A6=(union Cyc_YYSTYPE*)_T16A5;_T16A7=Cyc_yyget_YY41(_T16A6);_T16A8=Cyc_List_imp_rev(_T16A7);_T16A9=yyyvsp;_T16AA=_T16A9[0];_T16AB=_T16AA.l;_T16AC=_T16AB.first_line;_T16AD=Cyc_Position_loc_to_seg(_T16AC);_T16AE=Cyc_Absyn_instantiate_exp(_T16A3,_T16A8,_T16AD);yyval=Cyc_Exp_tok(_T16AE);goto _LL0;case 583:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T187A->tag=28;_T16B0=yyyvsp;_T16B1=& _T16B0[0].v;_T16B2=(union Cyc_YYSTYPE*)_T16B1;
# 3234
_T187A->f1=Cyc_yyget_QualId_tok(_T16B2);_T16B3=yyyvsp;_T16B4=& _T16B3[2].v;_T16B5=(union Cyc_YYSTYPE*)_T16B4;_T187A->f2=Cyc_yyget_YY41(_T16B5);_T16B6=yyyvsp;_T16B7=& _T16B6[3].v;_T16B8=(union Cyc_YYSTYPE*)_T16B7;_T16B9=Cyc_yyget_YY5(_T16B8);_T187A->f3=Cyc_List_imp_rev(_T16B9);_T187A->f4=0;_T16AF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T187A;}_T16BA=(void*)_T16AF;_T16BB=yyyvsp;_T16BC=_T16BB[0];_T16BD=_T16BC.l;_T16BE=_T16BD.first_line;_T16BF=Cyc_Position_loc_to_seg(_T16BE);_T16C0=Cyc_Absyn_new_exp(_T16BA,_T16BF);yyval=Cyc_Exp_tok(_T16C0);goto _LL0;case 584: _T16C1=yyyvsp;_T16C2=& _T16C1[2].v;_T16C3=(union Cyc_YYSTYPE*)_T16C2;_T16C4=
# 3236 "parse.y"
Cyc_yyget_YY4(_T16C3);_T16C5=yyyvsp;_T16C6=_T16C5[0];_T16C7=_T16C6.l;_T16C8=_T16C7.first_line;_T16C9=Cyc_Position_loc_to_seg(_T16C8);_T16CA=Cyc_Absyn_tuple_exp(_T16C4,_T16C9);yyval=Cyc_Exp_tok(_T16CA);goto _LL0;case 585: _T16CB=yyyvsp;_T16CC=& _T16CB[2].v;_T16CD=(union Cyc_YYSTYPE*)_T16CC;_T16CE=
# 3238 "parse.y"
Cyc_yyget_Stmt_tok(_T16CD);_T16CF=yyyvsp;_T16D0=_T16CF[0];_T16D1=_T16D0.l;_T16D2=_T16D1.first_line;_T16D3=Cyc_Position_loc_to_seg(_T16D2);_T16D4=Cyc_Absyn_stmt_exp(_T16CE,_T16D3);yyval=Cyc_Exp_tok(_T16D4);goto _LL0;case 586: _T16D5=yyyvsp;_T16D6=& _T16D5[0].v;_T16D7=(union Cyc_YYSTYPE*)_T16D6;_T16D8=
# 3242 "parse.y"
Cyc_yyget_YY4(_T16D7);_T16D9=Cyc_List_imp_rev(_T16D8);yyval=Cyc_YY4(_T16D9);goto _LL0;case 587:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T16DB=yyyvsp;_T16DC=& _T16DB[0].v;_T16DD=(union Cyc_YYSTYPE*)_T16DC;
# 3247 "parse.y"
_T187A->hd=Cyc_yyget_Exp_tok(_T16DD);_T187A->tl=0;_T16DA=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY4(_T16DA);goto _LL0;case 588:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T16DF=yyyvsp;_T16E0=& _T16DF[2].v;_T16E1=(union Cyc_YYSTYPE*)_T16E0;
# 3249 "parse.y"
_T187A->hd=Cyc_yyget_Exp_tok(_T16E1);_T16E2=yyyvsp;_T16E3=& _T16E2[0].v;_T16E4=(union Cyc_YYSTYPE*)_T16E3;_T187A->tl=Cyc_yyget_YY4(_T16E4);_T16DE=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY4(_T16DE);goto _LL0;case 589: _T16E5=yyyvsp;_T16E6=& _T16E5[0].v;_T16E7=(union Cyc_YYSTYPE*)_T16E6;_T16E8=
# 3255 "parse.y"
Cyc_yyget_Int_tok(_T16E7);_T16E9=yyyvsp;_T16EA=_T16E9[0];_T16EB=_T16EA.l;_T16EC=_T16EB.first_line;_T16ED=Cyc_Position_loc_to_seg(_T16EC);_T16EE=Cyc_Absyn_const_exp(_T16E8,_T16ED);yyval=Cyc_Exp_tok(_T16EE);goto _LL0;case 590: _T16EF=yyyvsp;_T16F0=& _T16EF[0].v;_T16F1=(union Cyc_YYSTYPE*)_T16F0;_T16F2=
# 3256 "parse.y"
Cyc_yyget_Char_tok(_T16F1);_T16F3=yyyvsp;_T16F4=_T16F3[0];_T16F5=_T16F4.l;_T16F6=_T16F5.first_line;_T16F7=Cyc_Position_loc_to_seg(_T16F6);_T16F8=Cyc_Absyn_char_exp(_T16F2,_T16F7);yyval=Cyc_Exp_tok(_T16F8);goto _LL0;case 591: _T16F9=yyyvsp;_T16FA=& _T16F9[0].v;_T16FB=(union Cyc_YYSTYPE*)_T16FA;_T16FC=
# 3257 "parse.y"
Cyc_yyget_String_tok(_T16FB);_T16FD=yyyvsp;_T16FE=_T16FD[0];_T16FF=_T16FE.l;_T1700=_T16FF.first_line;_T1701=Cyc_Position_loc_to_seg(_T1700);_T1702=Cyc_Absyn_wchar_exp(_T16FC,_T1701);yyval=Cyc_Exp_tok(_T1702);goto _LL0;case 592: _T1703=yyyvsp;_T1704=_T1703[0];_T1705=_T1704.l;_T1706=_T1705.first_line;_T1707=
# 3259 "parse.y"
Cyc_Position_loc_to_seg(_T1706);_T1708=Cyc_Absyn_null_exp(_T1707);yyval=Cyc_Exp_tok(_T1708);goto _LL0;case 593: _T1709=yyyvsp;_T170A=& _T1709[0].v;_T170B=(union Cyc_YYSTYPE*)_T170A;{
# 3261 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(_T170B);_T170C=
Cyc_strlen(f);{int l=(int)_T170C;
int i=1;
if(l <= 0)goto _TL2EB;_T170D=f;_T170E=_T170D.curr;_T170F=(const char*)_T170E;_T1710=l - 1;{
char c=_T170F[_T1710];_T1711=c;_T1712=(int)_T1711;
if(_T1712==102)goto _TL2EF;else{goto _TL2F0;}_TL2F0: _T1713=c;_T1714=(int)_T1713;if(_T1714==70)goto _TL2EF;else{goto _TL2ED;}_TL2EF: i=0;goto _TL2EE;
_TL2ED: _T1715=c;_T1716=(int)_T1715;if(_T1716==108)goto _TL2F3;else{goto _TL2F4;}_TL2F4: _T1717=c;_T1718=(int)_T1717;if(_T1718==76)goto _TL2F3;else{goto _TL2F1;}_TL2F3: i=2;goto _TL2F2;_TL2F1: _TL2F2: _TL2EE:;}goto _TL2EC;_TL2EB: _TL2EC: _T1719=f;_T171A=i;_T171B=yyyvsp;_T171C=_T171B[0];_T171D=_T171C.l;_T171E=_T171D.first_line;_T171F=
# 3269
Cyc_Position_loc_to_seg(_T171E);_T1720=Cyc_Absyn_float_exp(_T1719,_T171A,_T171F);yyval=Cyc_Exp_tok(_T1720);goto _LL0;}}case 594:{struct _tuple0*_T187A=_cycalloc(sizeof(struct _tuple0));
# 3274 "parse.y"
_T187A->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T1723=yyyvsp;_T1724=& _T1723[0].v;_T1725=(union Cyc_YYSTYPE*)_T1724;*_T187B=Cyc_yyget_String_tok(_T1725);_T1722=(struct _fat_ptr*)_T187B;}_T187A->f1=_T1722;_T1721=(struct _tuple0*)_T187A;}yyval=Cyc_QualId_tok(_T1721);goto _LL0;case 595: _T1726=yyyvsp;_T1727=_T1726[0];
# 3275 "parse.y"
yyval=_T1727.v;goto _LL0;case 596:{struct _tuple0*_T187A=_cycalloc(sizeof(struct _tuple0));
# 3278
_T187A->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T187B=_cycalloc(sizeof(struct _fat_ptr));_T172A=yyyvsp;_T172B=& _T172A[0].v;_T172C=(union Cyc_YYSTYPE*)_T172B;*_T187B=Cyc_yyget_String_tok(_T172C);_T1729=(struct _fat_ptr*)_T187B;}_T187A->f1=_T1729;_T1728=(struct _tuple0*)_T187A;}yyval=Cyc_QualId_tok(_T1728);goto _LL0;case 597: _T172D=yyyvsp;_T172E=_T172D[0];
# 3279 "parse.y"
yyval=_T172E.v;goto _LL0;case 598: _T172F=yyyvsp;_T1730=_T172F[0];
# 3284 "parse.y"
yyval=_T1730.v;goto _LL0;case 599: _T1731=yyyvsp;_T1732=_T1731[0];
# 3285 "parse.y"
yyval=_T1732.v;goto _LL0;case 600: _T1733=yyyvsp;_T1734=_T1733[0];
# 3288
yyval=_T1734.v;goto _LL0;case 601: _T1735=yyyvsp;_T1736=_T1735[0];
# 3289 "parse.y"
yyval=_T1736.v;goto _LL0;case 602: goto _LL0;case 603: _T1737=yylex_buf;
# 3294 "parse.y"
_T1737->lex_curr_pos=_T1737->lex_curr_pos - 1;goto _LL0;case 604:
# 3298 "parse.y"
 yyval=Cyc_YY71(0);goto _LL0;case 605:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple35*_T187B=_cycalloc(sizeof(struct _tuple35));_T173A=yyyvsp;_T173B=& _T173A[1].v;_T173C=(union Cyc_YYSTYPE*)_T173B;_T173D=
# 3300 "parse.y"
Cyc_yyget_String_tok(_T173C);_T173E=yyyvsp;_T173F=& _T173E[3].v;_T1740=(union Cyc_YYSTYPE*)_T173F;_T1741=Cyc_yyget_String_tok(_T1740);_T1742=_tag_fat("true",sizeof(char),5U);_T1743=Cyc_strcmp(_T1741,_T1742);_T1744=_T1743==0;_T1745=yyyvsp;_T1746=& _T1745[0].v;_T1747=(union Cyc_YYSTYPE*)_T1746;_T1748=Cyc_yyget_String_tok(_T1747);_T1749=Cyc_Parse_typevar2cvar(_T1748);_T187B->f0=Cyc_Parse_assign_cvar_pos(_T173D,_T1744,_T1749);_T174A=yyyvsp;_T174B=& _T174A[5].v;_T174C=(union Cyc_YYSTYPE*)_T174B;_T187B->f1=Cyc_yyget_YY72(_T174C);_T1739=(struct _tuple35*)_T187B;}_T187A->hd=_T1739;_T174D=yyyvsp;_T174E=& _T174D[7].v;_T174F=(union Cyc_YYSTYPE*)_T174E;_T187A->tl=Cyc_yyget_YY71(_T174F);_T1738=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY71(_T1738);goto _LL0;case 606:
# 3304 "parse.y"
 yyval=Cyc_YY72(0);goto _LL0;case 607: _T1750=yyyvsp;_T1751=& _T1750[0].v;_T1752=(union Cyc_YYSTYPE*)_T1751;_T1753=
# 3305 "parse.y"
Cyc_yyget_YY72(_T1752);yyval=Cyc_YY72(_T1753);goto _LL0;case 608:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T1755=yyyvsp;_T1756=& _T1755[0].v;_T1757=(union Cyc_YYSTYPE*)_T1756;
# 3309 "parse.y"
_T187A->hd=Cyc_yyget_YY73(_T1757);_T187A->tl=0;_T1754=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY72(_T1754);goto _LL0;case 609:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));_T1759=yyyvsp;_T175A=& _T1759[0].v;_T175B=(union Cyc_YYSTYPE*)_T175A;
# 3310 "parse.y"
_T187A->hd=Cyc_yyget_YY73(_T175B);_T175C=yyyvsp;_T175D=& _T175C[2].v;_T175E=(union Cyc_YYSTYPE*)_T175D;_T187A->tl=Cyc_yyget_YY72(_T175E);_T1758=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY72(_T1758);goto _LL0;case 610: _T175F=yyyvsp;_T1760=& _T175F[0].v;_T1761=(union Cyc_YYSTYPE*)_T1760;_T1762=
# 3314 "parse.y"
Cyc_yyget_String_tok(_T1761);_T1763=Cyc_Parse_typevar2cvar(_T1762);yyval=Cyc_YY45(_T1763);goto _LL0;case 611: _T1764=yyyvsp;_T1765=_T1764[0];_T1766=_T1765.l;_T1767=_T1766.first_line;_T1768=
# 3315 "parse.y"
Cyc_Position_loc_to_seg(_T1767);_T1769=yyyvsp;_T176A=& _T1769[0].v;_T176B=(union Cyc_YYSTYPE*)_T176A;_T176C=Cyc_yyget_String_tok(_T176B);_T176D=Cyc_Parse_str2type(_T1768,_T176C);yyval=Cyc_YY45(_T176D);goto _LL0;case 612: _T176E=yyyvsp;_T176F=& _T176E[0].v;_T1770=(union Cyc_YYSTYPE*)_T176F;_T1771=
# 3319 "parse.y"
Cyc_yyget_String_tok(_T1770);_T1772=yyyvsp;_T1773=& _T1772[3].v;_T1774=(union Cyc_YYSTYPE*)_T1773;_T1775=Cyc_yyget_YY73(_T1774);_T1776=Cyc_BansheeIf_check_constraint(_T1775);_T1777=Cyc_BansheeIf_add_location(_T1771,_T1776);yyval=Cyc_YY73(_T1777);goto _LL0;case 613: _T1778=yyyvsp;_T1779=& _T1778[0].v;_T177A=(union Cyc_YYSTYPE*)_T1779;_T177B=
# 3320 "parse.y"
Cyc_yyget_String_tok(_T177A);_T177C=yyyvsp;_T177D=& _T177C[3].v;_T177E=(union Cyc_YYSTYPE*)_T177D;_T177F=Cyc_yyget_YY70(_T177E);_T1780=yyyvsp;_T1781=& _T1780[5].v;_T1782=(union Cyc_YYSTYPE*)_T1781;_T1783=Cyc_yyget_YY45(_T1782);_T1784=yyyvsp;_T1785=& _T1784[7].v;_T1786=(union Cyc_YYSTYPE*)_T1785;_T1787=Cyc_yyget_YY45(_T1786);_T1788=Cyc_Parse_comparison_constraint(_T177F,_T1783,_T1787);_T1789=Cyc_BansheeIf_add_location(_T177B,_T1788);yyval=Cyc_YY73(_T1789);goto _LL0;case 614: _T178A=yyyvsp;_T178B=& _T178A[0].v;_T178C=(union Cyc_YYSTYPE*)_T178B;_T178D=
# 3321 "parse.y"
Cyc_yyget_String_tok(_T178C);_T178E=yyyvsp;_T178F=& _T178E[3].v;_T1790=(union Cyc_YYSTYPE*)_T178F;_T1791=Cyc_yyget_YY45(_T1790);_T1792=yyyvsp;_T1793=& _T1792[5].v;_T1794=(union Cyc_YYSTYPE*)_T1793;_T1795=Cyc_yyget_YY45(_T1794);_T1796=Cyc_BansheeIf_cond_equality_constraint(_T1791,_T1795);_T1797=Cyc_BansheeIf_add_location(_T178D,_T1796);yyval=Cyc_YY73(_T1797);goto _LL0;case 615: _T1798=yyyvsp;_T1799=& _T1798[0].v;_T179A=(union Cyc_YYSTYPE*)_T1799;_T179B=
# 3322 "parse.y"
Cyc_yyget_String_tok(_T179A);_T179C=yyyvsp;_T179D=& _T179C[3].v;_T179E=(union Cyc_YYSTYPE*)_T179D;_T179F=Cyc_yyget_YY45(_T179E);_T17A0=yyyvsp;_T17A1=& _T17A0[5].v;_T17A2=(union Cyc_YYSTYPE*)_T17A1;_T17A3=Cyc_yyget_YY45(_T17A2);_T17A4=Cyc_BansheeIf_equality_constraint(_T179F,_T17A3);_T17A5=Cyc_BansheeIf_add_location(_T179B,_T17A4);yyval=Cyc_YY73(_T17A5);goto _LL0;case 616: _T17A6=yyyvsp;_T17A7=& _T17A6[0].v;_T17A8=(union Cyc_YYSTYPE*)_T17A7;_T17A9=
# 3323 "parse.y"
Cyc_yyget_String_tok(_T17A8);_T17AA=yyyvsp;_T17AB=& _T17AA[3].v;_T17AC=(union Cyc_YYSTYPE*)_T17AB;_T17AD=Cyc_yyget_YY45(_T17AC);_T17AE=yyyvsp;_T17AF=& _T17AE[5].v;_T17B0=(union Cyc_YYSTYPE*)_T17AF;_T17B1=Cyc_yyget_YY45(_T17B0);_T17B2=Cyc_BansheeIf_inclusion_constraint(_T17AD,_T17B1);_T17B3=Cyc_BansheeIf_add_location(_T17A9,_T17B2);yyval=Cyc_YY73(_T17B3);goto _LL0;case 617: _T17B4=yyyvsp;_T17B5=& _T17B4[0].v;_T17B6=(union Cyc_YYSTYPE*)_T17B5;_T17B7=
# 3324 "parse.y"
Cyc_yyget_String_tok(_T17B6);_T17B8=yyyvsp;_T17B9=& _T17B8[3].v;_T17BA=(union Cyc_YYSTYPE*)_T17B9;_T17BB=Cyc_yyget_YY73(_T17BA);_T17BC=yyyvsp;_T17BD=& _T17BC[5].v;_T17BE=(union Cyc_YYSTYPE*)_T17BD;_T17BF=Cyc_yyget_YY73(_T17BE);_T17C0=Cyc_BansheeIf_implication_constraint(_T17BB,_T17BF);_T17C1=Cyc_BansheeIf_add_location(_T17B7,_T17C0);yyval=Cyc_YY73(_T17C1);goto _LL0;case 618: _T17C2=yyyvsp;_T17C3=& _T17C2[0].v;_T17C4=(union Cyc_YYSTYPE*)_T17C3;_T17C5=
# 3325 "parse.y"
Cyc_yyget_String_tok(_T17C4);_T17C6=yyyvsp;_T17C7=& _T17C6[3].v;_T17C8=(union Cyc_YYSTYPE*)_T17C7;_T17C9=Cyc_yyget_YY70(_T17C8);_T17CA=yyyvsp;_T17CB=& _T17CA[5].v;_T17CC=(union Cyc_YYSTYPE*)_T17CB;_T17CD=Cyc_yyget_YY73(_T17CC);_T17CE=yyyvsp;_T17CF=& _T17CE[7].v;_T17D0=(union Cyc_YYSTYPE*)_T17CF;_T17D1=Cyc_yyget_YY73(_T17D0);_T17D2=Cyc_Parse_composite_constraint(_T17C9,_T17CD,_T17D1);_T17D3=Cyc_BansheeIf_add_location(_T17C5,_T17D2);yyval=Cyc_YY73(_T17D3);goto _LL0;case 619:
# 3328
 yyval=Cyc_YY70(0U);goto _LL0;case 620:
# 3329 "parse.y"
 yyval=Cyc_YY70(1U);goto _LL0;case 621:
# 3330 "parse.y"
 yyval=Cyc_YY70(2U);goto _LL0;case 622:
# 3331 "parse.y"
 yyval=Cyc_YY70(3U);goto _LL0;case 623:
# 3332 "parse.y"
 yyval=Cyc_YY70(4U);goto _LL0;default: goto _LL0;}_LL0: _T17D4=yylen;
# 375 "cycbison.simple"
yyvsp_offset=yyvsp_offset - _T17D4;_T17D5=yylen;
yyssp_offset=yyssp_offset - _T17D5;_T17D6=yyvs;
# 389 "cycbison.simple"
yyvsp_offset=yyvsp_offset + 1;_T17D7=yyvsp_offset;_T17D8=_check_fat_subscript(_T17D6,sizeof(struct Cyc_Yystacktype),_T17D7);_T17D9=(struct Cyc_Yystacktype*)_T17D8;(*_T17D9).v=yyval;
# 392
if(yylen!=0)goto _TL2F5;_T17DA=yyvs;_T17DB=yyvsp_offset - 1;_T17DC=
_fat_ptr_plus(_T17DA,sizeof(struct Cyc_Yystacktype),_T17DB);_T17DD=_untag_fat_ptr_check_bound(_T17DC,sizeof(struct Cyc_Yystacktype),2U);_T17DE=_check_null(_T17DD);{struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_T17DE;_T17DF=p;_T17E0=yylloc;
_T17DF[1].l.first_line=_T17E0.first_line;_T17E1=p;_T17E2=yylloc;
_T17E1[1].l.first_column=_T17E2.first_column;_T17E3=p;_T17E4=p;_T17E5=_T17E4[0];_T17E6=_T17E5.l;
_T17E3[1].l.last_line=_T17E6.last_line;_T17E7=p;_T17E8=p;_T17E9=_T17E8[0];_T17EA=_T17E9.l;
_T17E7[1].l.last_column=_T17EA.last_column;}goto _TL2F6;
# 399
_TL2F5: _T17EB=yyvs;_T17EC=_T17EB.curr;_T17ED=(struct Cyc_Yystacktype*)_T17EC;_T17EE=yyvsp_offset;_T17EF=yyvs;_T17F0=yyvsp_offset + yylen;_T17F1=_T17F0 - 1;_T17F2=_check_fat_subscript(_T17EF,sizeof(struct Cyc_Yystacktype),_T17F1);_T17F3=(struct Cyc_Yystacktype*)_T17F2;_T17F4=*_T17F3;_T17F5=_T17F4.l;_T17ED[_T17EE].l.last_line=_T17F5.last_line;_T17F6=yyvs;_T17F7=_T17F6.curr;_T17F8=(struct Cyc_Yystacktype*)_T17F7;_T17F9=yyvsp_offset;_T17FA=yyvs;_T17FB=_T17FA.curr;_T17FC=(struct Cyc_Yystacktype*)_T17FB;_T17FD=yyvsp_offset + yylen;_T17FE=_T17FD - 1;_T17FF=_T17FC[_T17FE];_T1800=_T17FF.l;
_T17F8[_T17F9].l.last_column=_T1800.last_column;_TL2F6: _T1801=Cyc_yyr1;_T1802=yyn;_T1803=_check_known_subscript_notnull(_T1801,624U,sizeof(short),_T1802);_T1804=(short*)_T1803;_T1805=*_T1804;
# 409
yyn=(int)_T1805;_T1806=Cyc_yypgoto;_T1807=yyn - 175;_T1808=_check_known_subscript_notnull(_T1806,188U,sizeof(short),_T1807);_T1809=(short*)_T1808;_T180A=*_T1809;_T180B=(int)_T180A;_T180C=yyss;_T180D=yyssp_offset;_T180E=_check_fat_subscript(_T180C,sizeof(short),_T180D);_T180F=(short*)_T180E;_T1810=*_T180F;_T1811=(int)_T1810;
# 411
yystate=_T180B + _T1811;
if(yystate < 0)goto _TL2F7;if(yystate > 8241)goto _TL2F7;_T1812=Cyc_yycheck;_T1813=yystate;_T1814=_T1812[_T1813];_T1815=(int)_T1814;_T1816=yyss;_T1817=_T1816.curr;_T1818=(short*)_T1817;_T1819=yyssp_offset;_T181A=_T1818[_T1819];_T181B=(int)_T181A;if(_T1815!=_T181B)goto _TL2F7;_T181C=Cyc_yytable;_T181D=yystate;_T181E=_T181C[_T181D];
yystate=(int)_T181E;goto _TL2F8;
# 415
_TL2F7: _T181F=Cyc_yydefgoto;_T1820=yyn - 175;_T1821=_T181F[_T1820];yystate=(int)_T1821;_TL2F8: goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus!=0)goto _TL2F9;
# 424
yynerrs=yynerrs + 1;_T1822=Cyc_yypact;_T1823=yystate;_T1824=_check_known_subscript_notnull(_T1822,1262U,sizeof(short),_T1823);_T1825=(short*)_T1824;_T1826=*_T1825;
# 427
yyn=(int)_T1826;_T1827=yyn;_T1828=- 32768;
# 429
if(_T1827 <= _T1828)goto _TL2FB;if(yyn >= 8241)goto _TL2FB;{
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
if(yyn >= 0)goto _TL2FD;_T1829=- yyn;goto _TL2FE;_TL2FD: _T1829=0;_TL2FE: x=_T1829;
_TL302: _T182A=x;_T182B=(unsigned)_T182A;_T182C=363U / sizeof(char*);
# 437
if(_T182B < _T182C)goto _TL300;else{goto _TL301;}
# 439
_TL300: _T182D=Cyc_yycheck;_T182E=x + yyn;_T182F=_check_known_subscript_notnull(_T182D,8242U,sizeof(short),_T182E);_T1830=(short*)_T182F;_T1831=*_T1830;_T1832=(int)_T1831;_T1833=x;if(_T1832!=_T1833)goto _TL303;_T1834=Cyc_yytname;_T1835=x;_T1836=_check_known_subscript_notnull(_T1834,363U,sizeof(struct _fat_ptr),_T1835);_T1837=(struct _fat_ptr*)_T1836;_T1838=*_T1837;_T1839=
Cyc_strlen(_T1838);_T183A=_T1839 + 15U;sze=sze + _T183A;count=count + 1;goto _TL304;_TL303: _TL304:
# 438
 x=x + 1;goto _TL302;_TL301: _T183C=sze + 15;_T183D=(unsigned)_T183C;{unsigned _T187A=_T183D + 1U;_T183F=yyregion;_T1840=_check_times(_T187A,sizeof(char));{char*_T187B=_region_malloc(_T183F,0U,_T1840);_T1841=sze + 15;{unsigned _T187C=(unsigned)_T1841;unsigned i;i=0;_TL308: if(i < _T187C)goto _TL306;else{goto _TL307;}_TL306: _T1842=i;
# 441
_T187B[_T1842]='\000';i=i + 1;goto _TL308;_TL307: _T187B[_T187C]=0;}_T183E=(char*)_T187B;}_T183B=_tag_fat(_T183E,sizeof(char),_T187A);}msg=_T183B;_T1843=msg;_T1844=
_tag_fat("parse error",sizeof(char),12U);Cyc_strcpy(_T1843,_T1844);
# 444
if(count >= 5)goto _TL309;
# 446
count=0;
if(yyn >= 0)goto _TL30B;_T1845=- yyn;goto _TL30C;_TL30B: _T1845=0;_TL30C: x=_T1845;
_TL310: _T1846=x;_T1847=(unsigned)_T1846;_T1848=363U / sizeof(char*);
# 447
if(_T1847 < _T1848)goto _TL30E;else{goto _TL30F;}
# 449
_TL30E: _T1849=Cyc_yycheck;_T184A=x + yyn;_T184B=_check_known_subscript_notnull(_T1849,8242U,sizeof(short),_T184A);_T184C=(short*)_T184B;_T184D=*_T184C;_T184E=(int)_T184D;_T184F=x;if(_T184E!=_T184F)goto _TL311;_T1850=msg;
# 451
if(count!=0)goto _TL313;_T1852=
_tag_fat(", expecting `",sizeof(char),14U);_T1851=_T1852;goto _TL314;_TL313: _T1853=
_tag_fat(" or `",sizeof(char),6U);_T1851=_T1853;_TL314:
# 451
 Cyc_strcat(_T1850,_T1851);_T1854=msg;_T1855=Cyc_yytname;_T1856=x;_T1857=_check_known_subscript_notnull(_T1855,363U,sizeof(struct _fat_ptr),_T1856);_T1858=(struct _fat_ptr*)_T1857;_T1859=*_T1858;
# 454
Cyc_strcat(_T1854,_T1859);_T185A=msg;_T185B=
_tag_fat("'",sizeof(char),2U);Cyc_strcat(_T185A,_T185B);
count=count + 1;goto _TL312;_TL311: _TL312:
# 448
 x=x + 1;goto _TL310;_TL30F: goto _TL30A;_TL309: _TL30A:
# 459
 Cyc_yyerror(msg,yystate,yychar);}goto _TL2FC;
# 463
_TL2FB: _T185C=_tag_fat("parse error",sizeof(char),12U);_T185D=yystate;_T185E=yychar;Cyc_yyerror(_T185C,_T185D,_T185E);_TL2FC: goto _TL2FA;_TL2F9: _TL2FA: goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus!=3)goto _TL315;
# 474
if(yychar!=0)goto _TL317;{int _T187A=1;_npop_handler(0);return _T187A;}_TL317:
# 483
 yychar=- 2;goto _TL316;_TL315: _TL316:
# 489
 yyerrstatus=3;goto yyerrhandle;
# 493
yyerrdefault:
# 503 "cycbison.simple"
 yyerrpop:
# 505
 if(yyssp_offset!=0)goto _TL319;{int _T187A=1;_npop_handler(0);return _T187A;}_TL319:
 yyvsp_offset=yyvsp_offset + -1;_T185F=yyss;
yyssp_offset=yyssp_offset + -1;_T1860=yyssp_offset;_T1861=_check_fat_subscript(_T185F,sizeof(short),_T1860);_T1862=(short*)_T1861;_T1863=*_T1862;yystate=(int)_T1863;
# 521 "cycbison.simple"
yyerrhandle: _T1864=Cyc_yypact;_T1865=yystate;_T1866=_check_known_subscript_notnull(_T1864,1262U,sizeof(short),_T1865);_T1867=(short*)_T1866;_T1868=*_T1867;
yyn=(int)_T1868;_T1869=yyn;_T186A=- 32768;
if(_T1869!=_T186A)goto _TL31B;goto yyerrdefault;_TL31B:
# 525
 yyn=yyn + 1;
if(yyn < 0)goto _TL31F;else{goto _TL321;}_TL321: if(yyn > 8241)goto _TL31F;else{goto _TL320;}_TL320: _T186B=Cyc_yycheck;_T186C=yyn;_T186D=_T186B[_T186C];_T186E=(int)_T186D;if(_T186E!=1)goto _TL31F;else{goto _TL31D;}_TL31F: goto yyerrdefault;_TL31D: _T186F=Cyc_yytable;_T1870=yyn;_T1871=_T186F[_T1870];
# 528
yyn=(int)_T1871;
if(yyn >= 0)goto _TL322;_T1872=yyn;_T1873=- 32768;
# 531
if(_T1872!=_T1873)goto _TL324;goto yyerrpop;_TL324:
 yyn=- yyn;goto yyreduce;
# 535
_TL322: if(yyn!=0)goto _TL326;goto yyerrpop;_TL326:
# 537
 if(yyn!=1261)goto _TL328;{int _T187A=0;_npop_handler(0);return _T187A;}_TL328: _T1874=yyvs;
# 546
yyvsp_offset=yyvsp_offset + 1;_T1875=yyvsp_offset;_T1876=_check_fat_subscript(_T1874,sizeof(struct Cyc_Yystacktype),_T1875);_T1877=(struct Cyc_Yystacktype*)_T1876;{struct Cyc_Yystacktype _T187A;_T187A.v=yylval;_T187A.l=yylloc;_T1878=_T187A;}*_T1877=_T1878;goto yynewstate;}}}}_pop_region();}
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

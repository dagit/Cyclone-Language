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
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 166
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 186 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 317
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 330
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 337
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;struct Cyc_Absyn_Exp*f9;struct Cyc_Absyn_Exp*f10;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 529 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 536
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 554
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 568
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 736 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 904
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 930
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 938
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 944
void*Cyc_Absyn_compress(void*);
# 957
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 959
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 962
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 964
extern void*Cyc_Absyn_sint_type;
# 966
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 969
extern void*Cyc_Absyn_complex_type(void*);
# 971
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_shorthand_type;extern void*Cyc_Absyn_refcnt_rgn_shorthand_type;
# 973
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 977
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 979
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_tuple_type(struct Cyc_List_List*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 998
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 1010
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 1014
void*Cyc_Absyn_bounds_one (void);
void*Cyc_Absyn_cvar_type(struct Cyc_Core_Opt*);
void*Cyc_Absyn_cvar_type_name(struct Cyc_Core_Opt*,struct _fat_ptr);
# 1020
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 1040
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1043
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1047
void*Cyc_Absyn_aqualsof_type(void*);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1078
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tagof_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1089
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1099
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1101
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1108
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1110
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
# 1124
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1128
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_false_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1139
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1144
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1155
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1158
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1161
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1165
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1170
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1172
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1181
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1188
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1191
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1196
void*Cyc_Absyn_function_type(struct Cyc_List_List*,void*,struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
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
# 43 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 216 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 247
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 257
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 281
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
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x==0)return x;{
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second!=0){
{struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 115
1U;}
# 120
return first;}}char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};
# 176
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier,unsigned);struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 181
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
static struct Cyc_List_List*Cyc_Parse_constraint_graph=0;
static int Cyc_Parse_inside_function_definition=0;
static int Cyc_Parse_inside_noinference_block=0;
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr msg){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=msg;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Parse_Exit_exn_struct));_Tmp0->tag=Cyc_Parse_Exit;_Tmp0;}));}
# 190
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 192
void*_Tmp0;struct Cyc_Absyn_Tqual _Tmp1;_Tmp1=tqt->f1;_Tmp0=tqt->f2;{struct Cyc_Absyn_Tqual tq=_Tmp1;void*t=_Tmp0;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc!=0U)loc=tq.loc;
Cyc_Warn_warn(loc,_tag_fat("qualifier on type is ignored",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}
# 197
return t;}}
# 200
static void*Cyc_Parse_make_pointer_mod(struct _RegionHandle*r,struct Cyc_Absyn_PtrLoc*loc,void*nullable,void*bound,void*eff,struct Cyc_List_List*pqs,struct Cyc_Absyn_Tqual tqs){
# 205
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*autoreleased=Cyc_Tcutil_any_bool(0);
void*aqual=0;
for(1;pqs!=0;pqs=pqs->tl){
void*_Tmp0=(void*)pqs->hd;void*_Tmp1;switch(*((int*)_Tmp0)){case 5:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 6:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 7:
 autoreleased=Cyc_Absyn_true_type;goto _LL0;case 9:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 8:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 4:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 3:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0: _Tmp1=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}case 10: _Tmp1=(void*)((struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{void*aq=_Tmp1;
aqual=aq;goto _LL0;}case 2: _Tmp1=((struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{struct Cyc_List_List*ts=_Tmp1;
eff=Cyc_Absyn_join_eff(ts);goto _LL0;}default: _Tmp1=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{void*t=_Tmp1;
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("Found region pointer qual",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}}_LL0:;}
# 222
return(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_Tmp0->tag=2,_Tmp0->f1.eff=eff,_Tmp0->f1.nullable=nullable,_Tmp0->f1.bounds=bound,_Tmp0->f1.zero_term=zeroterm,_Tmp0->f1.ptrloc=loc,_Tmp0->f1.autoreleased=autoreleased,
# 224
aqual!=0?_Tmp0->f1.aqual=aqual:({void*_Tmp1=({void*_Tmp2=Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,0);Cyc_Absyn_aqual_var_type(_Tmp2,Cyc_Absyn_rtd_qual_type);});_Tmp0->f1.aqual=_Tmp1;}),_Tmp0->f2=tqs;_Tmp0;});}
# 229
static void Cyc_Parse_check_single_constraint(unsigned loc,struct _fat_ptr id){
if(Cyc_zstrcmp(id,_tag_fat("single",sizeof(char),7U)))
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat(" is not a valid effect constraint",sizeof(char),34U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err(loc,id,_tag_fat(_Tmp1,sizeof(void*),1));});}
# 233
static void*Cyc_Parse_effect_from_atomic(struct Cyc_List_List*effs){
if(Cyc_List_length(effs)==1)
return(void*)_check_null(effs)->hd;else{
# 237
return Cyc_Absyn_join_eff(effs);}}
# 240
static struct _tuple0*Cyc_Parse_gensym_enum (void){
static int enum_counter=0;
struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);_Tmp0->f0=_Tmp1;}),({
struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)enum_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("__anonymous_enum_%d__",sizeof(char),22U),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f1=_Tmp1;});return _Tmp0;}struct _tuple15{unsigned f0;struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple17{struct _tuple15*f0;struct _tuple16*f1;};
# 246
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple17*field_info){
# 251
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;unsigned _Tmp7;_Tmp7=field_info->f0->f0;_Tmp6=field_info->f0->f1;_Tmp5=field_info->f0->f2;_Tmp4=field_info->f0->f3;_Tmp3=field_info->f0->f4;_Tmp2=field_info->f0->f5;_Tmp1=field_info->f1->f0;_Tmp0=field_info->f1->f1;{unsigned varloc=_Tmp7;struct _tuple0*qid=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*t=_Tmp4;struct Cyc_List_List*tvs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;struct Cyc_Absyn_Exp*widthopt=_Tmp1;struct Cyc_Absyn_Exp*reqopt=_Tmp0;
if(tvs!=0)
Cyc_Warn_err(loc,_tag_fat("bad type params in struct field",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_Absyn_is_qvar_qualified(qid))
Cyc_Warn_err(loc,_tag_fat("struct or union field cannot be qualified with a namespace",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_Absyn_Aggrfield*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp8->name=(*qid).f1,_Tmp8->tq=tq,_Tmp8->type=t,_Tmp8->width=widthopt,_Tmp8->attributes=atts,_Tmp8->requires_clause=reqopt;return _Tmp8;}}}
# 261
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _Tmp0;_Tmp0.Signed_spec=0,_Tmp0.Unsigned_spec=0,_Tmp0.Short_spec=0,_Tmp0.Long_spec=0,_Tmp0.Long_Long_spec=0,_Tmp0.Complex_spec=0,_Tmp0.Valid_type_spec=0,_Tmp0.Type_spec=Cyc_Absyn_sint_type,_Tmp0.loc=loc;return _Tmp0;}
# 273
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Signed_spec=1;
return s;}
# 284
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Unsigned_spec=1;
return s;}
# 289
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Short_spec=1;
return s;}
# 294
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Long_spec=1;
return s;}
# 299
static struct Cyc_Parse_Type_specifier Cyc_Parse_complex_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Complex_spec=1;
return s;}
# 306
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 308
if(Cyc_Tcutil_is_array_type(t)){
void*_Tmp0=t;void*_Tmp1=argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_eko,0): Cyc_Absyn_heap_rgn_type;return Cyc_Tcutil_promote_array(_Tmp0,_Tmp1,Cyc_Absyn_al_qual_type,0);}else{return t;}}struct _tuple18{struct _fat_ptr*f0;void*f1;};
# 321 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
struct _tuple8*_Tmp0=(struct _tuple8*)x->hd;struct _fat_ptr _Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)((struct _tuple8*)_Tmp0)->f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2)->tl==0){_Tmp3=_Tmp0->f0;_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f2)->f2->hd;if((struct _fat_ptr*)_Tmp3!=0){struct _fat_ptr*v=_Tmp3;void*i=_Tmp2;
# 326
{void*_Tmp4;if(*((int*)i)==1){_Tmp4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)i)->f2;{void**z=(void**)_Tmp4;
# 330
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp5=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=*v;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));});*nm=_Tmp5;});
({void*_Tmp5=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp6->name=nm,_Tmp6->identity=-1,({void*_Tmp7=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp8->tag=0,_Tmp8->f1=& Cyc_Kinds_ik;_Tmp8;});_Tmp6->kind=_Tmp7;}),_Tmp6->aquals_bound=0;_Tmp6;}));*z=_Tmp5;});
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 335
res=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp5=({struct _tuple18*_Tmp6=_cycalloc(sizeof(struct _tuple18));_Tmp6->f0=v,_Tmp6->f1=i;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=res;_Tmp4;});goto _LL0;}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2)->hd)==1){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2)->tl==0){_Tmp1=*_Tmp0->f0;_Tmp3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f2)->f2->hd)->f2;{struct _fat_ptr v=_Tmp1;void**z=(void**)_Tmp3;
# 339
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=v;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp6,sizeof(void*),1));});*nm=_Tmp4;});
({void*_Tmp4=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp5->name=nm,_Tmp5->identity=-1,({void*_Tmp6=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp7->tag=0,_Tmp7->f1=& Cyc_Kinds_ek;_Tmp7;});_Tmp5->kind=_Tmp6;}),_Tmp5->aquals_bound=0;_Tmp5;}));*z=_Tmp4;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{_LL5:
 goto _LL0;}}_LL0:;}
# 344
return res;}
# 348
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
void*_Tmp0=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->tl==0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*i=_Tmp1;
# 353
res=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp3=({struct _tuple18*_Tmp4=_cycalloc(sizeof(struct _tuple18));_Tmp4->f0=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_Tmp4->f1=i;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=res;_Tmp2;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 356
return res;}
# 360
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)==0){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f0.Rel_n.tag==1){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f0.Rel_n.val==0){_Tmp1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1->f1;{struct _fat_ptr*y=_Tmp1;
# 363
{struct Cyc_List_List*ts=tags;for(0;ts!=0;ts=ts->tl){
struct _tuple18*_Tmp2=(struct _tuple18*)ts->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct _fat_ptr*x=_Tmp4;void*i=_Tmp3;
if(Cyc_strptrcmp(x,y)==0){
void*_Tmp5=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_Tmp6->tag=38,({void*_Tmp7=Cyc_Tcutil_copy_type(i);_Tmp6->f1=_Tmp7;});_Tmp6;});return Cyc_Absyn_new_exp(_Tmp5,e->loc);}}}}
# 368
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 371
return e;}
# 376
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;switch(*((int*)t)){case 5: _Tmp9=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;_Tmp6=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zero_term;_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zt_loc;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;struct Cyc_Absyn_Exp*nelts=_Tmp7;void*zt=_Tmp6;unsigned ztloc=_Tmp5;
# 379
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts!=0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*et2=Cyc_Parse_substitute_tags(tags,et);
if(nelts!=nelts2 || et!=et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 4: _Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.eff;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.nullable;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.zero_term;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.ptrloc;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.autoreleased;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.aqual;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;void*r=_Tmp7;void*n=_Tmp6;void*b=_Tmp4;void*zt=_Tmp3;struct Cyc_Absyn_PtrLoc*pl=_Tmp2;void*rel=_Tmp1;void*aq=_Tmp0;
# 387
void*et2=Cyc_Parse_substitute_tags(tags,et);
void*b2=Cyc_Parse_substitute_tags(tags,b);
if(et2!=et || b2!=b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _TmpA;_TmpA.elt_type=et2,_TmpA.elt_tq=tq,_TmpA.ptr_atts.eff=r,_TmpA.ptr_atts.nullable=n,_TmpA.ptr_atts.bounds=b2,_TmpA.ptr_atts.zero_term=zt,_TmpA.ptr_atts.ptrloc=pl,_TmpA.ptr_atts.autoreleased=rel,_TmpA.ptr_atts.aqual=aq;_TmpA;}));
goto _LL0;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==13){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2)->tl==0){_Tmp9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2->hd;{void*t=_Tmp9;
# 393
void*t2=Cyc_Parse_substitute_tags(tags,t);
if(t!=t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9: _Tmp9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 397
struct Cyc_Absyn_Exp*e2=Cyc_Parse_substitute_tags_exp(tags,e);
if(e2!=e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9:
 goto _LL0;}_LL0:;}
# 402
return t;}
# 407
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_Tmp0=Cyc_Parse_substitute_tags(tags,x->type);x->type=_Tmp0;});}struct _tuple19{struct Cyc_Absyn_Tqual f0;void*f1;};struct _tuple20{void*f0;struct Cyc_Absyn_Tqual f1;void*f2;};
# 414
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple20*t){
struct _tuple19*_Tmp0=_cycalloc(sizeof(struct _tuple19));_Tmp0->f0=(*t).f1,_Tmp0->f1=(*t).f2;return _Tmp0;}
# 418
static int Cyc_Parse_is_typeparam(void*tm){
if(*((int*)tm)==4)
return 1;else{
return 0;};}
# 427
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k,void*aliashint,unsigned loc){
if(Cyc_zstrcmp(s,_tag_fat("`H",sizeof(char),3U))==0)
return Cyc_Absyn_heap_rgn_type;
# 431
if(Cyc_zstrcmp(s,_tag_fat("`U",sizeof(char),3U))==0)
return Cyc_Absyn_unique_rgn_shorthand_type;
if(Cyc_zstrcmp(s,_tag_fat("`RC",sizeof(char),4U))==0)
return Cyc_Absyn_refcnt_rgn_shorthand_type;
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
return Cyc_CurRgn_curr_rgn_type();
aliashint=Cyc_Kinds_consistent_aliashint(loc,k,aliashint);
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->name=_Tmp1;}),_Tmp0->identity=-1,_Tmp0->kind=k,_Tmp0->aquals_bound=aliashint;_Tmp0;}));}
# 441
static void*Cyc_Parse_id2aqual(unsigned loc,struct _fat_ptr s){
if(Cyc_strlen(s)==2U){
char _Tmp0=((const char*)s.curr)[1];switch((int)_Tmp0){case 65:
 return Cyc_Absyn_al_qual_type;case 85:
 return Cyc_Absyn_un_qual_type;case 84:
 return Cyc_Absyn_rtd_qual_type;default:
 goto _LL0;}_LL0:;}else{
# 450
if(Cyc_strlen(s)==3U){
if((int)((const char*)s.curr)[1]==82 &&(int)((const char*)s.curr)[2]==67)
return Cyc_Absyn_rc_qual_type;}}
# 454
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad aqual bound ",sizeof(char),17U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=s;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Absyn_al_qual_type;}
# 458
static struct Cyc_List_List*Cyc_Parse_insert_aqual(struct _RegionHandle*yy,struct Cyc_List_List*qlist,void*aq,unsigned loc){
{struct Cyc_List_List*l=qlist;for(0;l!=0;l=l->tl){
void*_Tmp0=(void*)l->hd;if(*((int*)_Tmp0)==10){
# 462
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Multiple alias qualifiers",sizeof(char),26U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});
return qlist;}else{
# 465
goto _LL0;}_LL0:;}}{
# 468
struct Cyc_List_List*_Tmp0=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));({void*_Tmp1=(void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yy,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=10,_Tmp2->f1=aq;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=qlist;return _Tmp0;}}
# 471
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(Cyc_zstrcmp(s,_tag_fat("`H",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of heap region",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`U",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of unique region",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`RC",sizeof(char),4U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of refcounted region",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of \"current\" region",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));}
# 486
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_Tmp0;if(*((int*)t)==2){_Tmp0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*pr=_Tmp0;
return pr;}}else{
({int(*_Tmp1)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp1;})(loc,_tag_fat("expecting a list of type variables, not types",sizeof(char),46U));};}
# 494
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=(void**)&((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1->kind;{void**cptr=(void**)_Tmp1;
# 497
void*_Tmp2=Cyc_Kinds_compress_kb(*cptr);if(*((int*)_Tmp2)==1){
# 499
({void*_Tmp3=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp4->tag=2,_Tmp4->f1=0,_Tmp4->f2=k;_Tmp4;}): Cyc_Kinds_kind_to_bound(k);*cptr=_Tmp3;});return;}else{
return;};}}else{
# 502
return;};}
# 507
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 515
if(tms==0)return 0;{
# 517
void*_Tmp0=(void*)tms->hd;void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp1;
# 520
if(tms->tl==0 ||
 Cyc_Parse_is_typeparam((void*)tms->tl->hd)&& _check_null(tms->tl)->tl==0){
# 523
void*_Tmp2;if(*((int*)args)==1){
# 525
Cyc_Warn_warn(loc,
_tag_fat("function declaration with both new- and old-style parameter declarations; ignoring old-style",sizeof(char),93U),_tag_fat(0U,sizeof(void*),0));
return tms;}else{_Tmp2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f1;{struct Cyc_List_List*ids=_Tmp2;
# 529
if(({int _Tmp3=Cyc_List_length(ids);_Tmp3!=Cyc_List_length(tds);}))
({int(*_Tmp3)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp3;})(loc,
_tag_fat("wrong number of parameter declarations in old-style function declaration",sizeof(char),73U));{
# 533
struct Cyc_List_List*rev_new_params=0;
for(1;ids!=0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2!=0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*x=(struct Cyc_Absyn_Decl*)tds2->hd;
void*_Tmp3=x->r;void*_Tmp4;if(*((int*)_Tmp3)==0){_Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 540
if(Cyc_zstrptrcmp((*vd->name).f1,(struct _fat_ptr*)ids->hd)!=0)
continue;
if(vd->initializer!=0)
({int(*_Tmp5)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp5;})(x->loc,_tag_fat("initializer found in parameter declaration",sizeof(char),43U));
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({int(*_Tmp5)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp5;})(x->loc,_tag_fat("namespaces forbidden in parameter declarations",sizeof(char),47U));
rev_new_params=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp6=({struct _tuple8*_Tmp7=_cycalloc(sizeof(struct _tuple8));_Tmp7->f0=(*vd->name).f1,_Tmp7->f1=vd->tq,_Tmp7->f2=vd->type;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=rev_new_params;_Tmp5;});
# 548
goto L;}}else{
({int(*_Tmp5)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp5;})(x->loc,_tag_fat("nonvariable declaration in parameter type",sizeof(char),42U));};}
# 552
L: if(tds2==0)
({int(*_Tmp3)(unsigned,struct _fat_ptr)=({int(*_Tmp4)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp4;});unsigned _Tmp4=loc;_Tmp3(_Tmp4,Cyc_strconcat(*((struct _fat_ptr*)ids->hd),_tag_fat(" is not given a type",sizeof(char),21U)));});}{
# 555
struct Cyc_List_List*_Tmp3=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));
({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yy,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yy,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp7->tag=1,({struct Cyc_List_List*_Tmp8=Cyc_List_imp_rev(rev_new_params);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=0,_Tmp7->f3=0,_Tmp7->f4=0,_Tmp7->f5=0,_Tmp7->f6=0,_Tmp7->f7=0,_Tmp7->f8=0,_Tmp7->f9=0,_Tmp7->f10=0;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;return _Tmp3;}}}};}
# 562
goto _LL4;}}else{_LL4: {
struct Cyc_List_List*_Tmp2=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp3=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_Tmp2->tl=_Tmp3;});return _Tmp2;}};}}
# 569
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 573
if(tds!=0)
d=({struct Cyc_Parse_Declarator _Tmp0;_Tmp0.id=d.id,_Tmp0.varloc=d.varloc,({struct Cyc_List_List*_Tmp1=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_Tmp0.tms=_Tmp1;});_Tmp0;});{
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 581
if(dso!=0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 587
enum Cyc_Parse_Storage_class _Tmp0=dso->sc;switch((int)_Tmp0){case Cyc_Parse_None_sc:
 goto _LL0;case Cyc_Parse_Extern_sc:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default:
 Cyc_Warn_err(loc,_tag_fat("bad storage class on function",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}}{
# 594
void*t=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f0;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct Cyc_Absyn_Tqual fn_tqual=_Tmp4;void*fn_type=_Tmp3;struct Cyc_List_List*x=_Tmp2;struct Cyc_List_List*out_atts=_Tmp1;
# 599
if(x!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));{
# 602
struct Cyc_Absyn_FnInfo _Tmp5;if(*((int*)fn_type)==6){_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fn_type)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp5;
# 604
{struct Cyc_List_List*args2=i.args;for(0;args2!=0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f0==0){
Cyc_Warn_err(loc,_tag_fat("missing argument variable in function prototype",sizeof(char),48U),_tag_fat(0U,sizeof(void*),0));
({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=_tag_fat("?",sizeof(char),2U);_Tmp7;});(*((struct _tuple8*)args2->hd)).f0=_Tmp6;});}}}
# 615
({struct Cyc_List_List*_Tmp6=Cyc_List_append(i.attributes,out_atts);i.attributes=_Tmp6;});{
struct Cyc_Absyn_Fndecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_Tmp6->sc=sc,_Tmp6->is_inline=is_inline,_Tmp6->name=d.id,_Tmp6->body=body,_Tmp6->i=i,_Tmp6->cached_type=0,_Tmp6->param_vardecls=0,_Tmp6->fn_vardecl=0,_Tmp6->orig_scope=sc,_Tmp6->escapes=0;return _Tmp6;}}}else{
# 619
({int(*_Tmp6)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp6;})(loc,_tag_fat("declarator is not a function prototype",sizeof(char),39U));};}}}}}static char _TmpG0[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 623
static struct _fat_ptr Cyc_Parse_msg1={_TmpG0,_TmpG0,_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 625
static struct _fat_ptr Cyc_Parse_msg2={_TmpG1,_TmpG1,_TmpG1 + 84U};
# 632
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 635
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
# 660
return s1;}
# 666
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 674
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
# 687
if(!seen_type){
if(!seen_sign && !seen_size){
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_double_type);
Cyc_Warn_warn(loc,_tag_fat("missing type within specifier",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));}
# 693
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_int_type(sgn,sz));else{
return Cyc_Absyn_int_type(sgn,sz);}}
# 697
{enum Cyc_Absyn_Size_of _Tmp0;enum Cyc_Absyn_Sign _Tmp1;if(*((int*)t)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 1: _Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Sign sgn2=_Tmp1;enum Cyc_Absyn_Size_of sz2=_Tmp0;
# 699
if(seen_sign &&(int)sgn2!=(int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 703
if(seen_size &&(int)sz2!=(int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;}case 2:
# 709
 if(seen_size)
t=Cyc_Absyn_long_double_type;
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;default: goto _LL5;}else{_LL5:
# 715
 if(seen_sign)
Cyc_Warn_err(loc,_tag_fat("sign specification on non-integral type",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
if(seen_size)
Cyc_Warn_err(loc,_tag_fat("size qualifier on non-integral type",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}_LL0:;}
# 721
return t;}
# 724
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 728
if(ds==0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*q=d.id;
unsigned varloc=d.varloc;
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f0;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct Cyc_Absyn_Tqual tq2=_Tmp4;void*new_typ=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
# 735
struct Cyc_List_List*tl=ds->tl==0?0:({struct _RegionHandle*_Tmp5=r;struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=Cyc_Tcutil_copy_type(t);struct _tuple13*_Tmp8=ds->tl;Cyc_Parse_apply_tmss(_Tmp5,_Tmp6,_Tmp7,_Tmp8,shared_atts);});
struct Cyc_List_List*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_region_malloc(r,0U,sizeof(struct _tuple15));_Tmp7->f0=varloc,_Tmp7->f1=q,_Tmp7->f2=tq2,_Tmp7->f3=new_typ,_Tmp7->f4=tvs,_Tmp7->f5=atts;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=tl;return _Tmp5;}}}
# 739
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 742
if(tms==0){struct _tuple14 _Tmp0;_Tmp0.f0=tq,_Tmp0.f1=t,_Tmp0.f2=0,_Tmp0.f3=atts;return _Tmp0;}{
void*_Tmp0=(void*)tms->hd;struct Cyc_Absyn_Tqual _Tmp1;struct Cyc_Absyn_PtrAtts _Tmp2;void*_Tmp3;unsigned _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: _Tmp5=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*zeroterm=_Tmp5;unsigned ztloc=_Tmp4;
# 745
struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 745
struct Cyc_List_List*_Tmp8=atts;return Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);}case 1: _Tmp5=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp5;void*zeroterm=_Tmp3;unsigned ztloc=_Tmp4;
# 748
struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 748
struct Cyc_List_List*_Tmp8=atts;return Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);}case 3: _Tmp5=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp5;
# 751
unsigned _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;int _TmpF;void*_Tmp10;if(*((int*)args)==1){_Tmp10=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f1;_TmpF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f2;_TmpE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f3;_TmpD=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f4;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f5;_TmpB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f6;_TmpA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f7;_Tmp9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f8;_Tmp8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f9;_Tmp7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f10;{struct Cyc_List_List*args2=_Tmp10;int c_vararg=_TmpF;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpE;void*eff=_TmpD;struct Cyc_List_List*effc=_TmpC;struct Cyc_List_List*qb=_TmpB;struct Cyc_Absyn_Exp*chks=_TmpA;struct Cyc_Absyn_Exp*req=_Tmp9;struct Cyc_Absyn_Exp*ens=_Tmp8;struct Cyc_Absyn_Exp*thrw=_Tmp7;
# 753
struct Cyc_List_List*typvars=0;
# 755
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as!=0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_Tmp11=_cycalloc(sizeof(struct Cyc_List_List));_Tmp11->hd=(void*)as->hd,_Tmp11->tl=fn_atts;_Tmp11;});else{
# 760
new_atts=({struct Cyc_List_List*_Tmp11=_cycalloc(sizeof(struct Cyc_List_List));_Tmp11->hd=(void*)as->hd,_Tmp11->tl=new_atts;_Tmp11;});}}}
# 762
if(tms->tl!=0){
void*_Tmp11=(void*)tms->tl->hd;void*_Tmp12;if(*((int*)_Tmp11)==4){_Tmp12=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp11)->f1;{struct Cyc_List_List*ts=_Tmp12;
# 765
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{
goto _LL12;}_LL12:;}
# 771
if(((((!c_vararg && cyc_vararg==0)&& args2!=0)&& args2->tl==0)&&(*((struct _tuple8*)args2->hd)).f0==0)&&(*((struct _tuple8*)args2->hd)).f2==Cyc_Absyn_void_type)
# 776
args2=0;{
# 781
struct Cyc_List_List*new_requires=0;
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_Tmp11=(struct _tuple8*)a->hd;void*_Tmp12;struct Cyc_Absyn_Tqual _Tmp13;void*_Tmp14;_Tmp14=_Tmp11->f0;_Tmp13=_Tmp11->f1;_Tmp12=(void**)& _Tmp11->f2;{struct _fat_ptr*vopt=_Tmp14;struct Cyc_Absyn_Tqual tq=_Tmp13;void**t=(void**)_Tmp12;
void*_Tmp15=*t;unsigned _Tmp16;void*_Tmp17;void*_Tmp18;struct Cyc_Absyn_Tqual _Tmp19;void*_Tmp1A;if(*((int*)_Tmp15)==5){_Tmp1A=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp15)->f1.elt_type;_Tmp19=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp15)->f1.tq;_Tmp18=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp15)->f1.num_elts;_Tmp17=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp15)->f1.zero_term;_Tmp16=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp15)->f1.zt_loc;{void*et=_Tmp1A;struct Cyc_Absyn_Tqual tq=_Tmp19;struct Cyc_Absyn_Exp*neltsopt=_Tmp18;void*zt=_Tmp17;unsigned ztloc=_Tmp16;
# 786
if(neltsopt!=0 && vopt!=0){
struct _tuple0*v;v=_cycalloc(sizeof(struct _tuple0)),v->f0.Loc_n.tag=4U,v->f0.Loc_n.val=0,v->f1=vopt;{
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,({struct Cyc_Absyn_Exp*_Tmp1B[1];({struct Cyc_Absyn_Exp*_Tmp1C=Cyc_Absyn_var_exp(v,0U);_Tmp1B[0]=_Tmp1C;});Cyc_List_list(_tag_fat(_Tmp1B,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);
new_requires=({struct Cyc_List_List*_Tmp1B=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1B->hd=new_req,_Tmp1B->tl=new_requires;_Tmp1B;});}}
# 793
goto _LL1A;}}else{
goto _LL1A;}_LL1A:;}}}
# 797
if(new_requires!=0){
struct Cyc_Absyn_Exp*r;
if(req!=0)
r=req;else{
# 802
r=(struct Cyc_Absyn_Exp*)new_requires->hd;
new_requires=new_requires->tl;}
# 805
for(1;new_requires!=0;new_requires=new_requires->tl){
r=Cyc_Absyn_and_exp(r,(struct Cyc_Absyn_Exp*)new_requires->hd,0U);}
req=r;}{
# 811
struct Cyc_List_List*tags=Cyc_Parse_get_arg_tags(args2);
# 813
if(tags!=0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 818
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_Tmp11=(struct _tuple8*)a->hd;void*_Tmp12;struct Cyc_Absyn_Tqual _Tmp13;void*_Tmp14;_Tmp14=_Tmp11->f0;_Tmp13=_Tmp11->f1;_Tmp12=(void**)& _Tmp11->f2;{struct _fat_ptr*vopt=_Tmp14;struct Cyc_Absyn_Tqual tq=_Tmp13;void**t=(void**)_Tmp12;
if(tags!=0)
({void*_Tmp15=Cyc_Parse_substitute_tags(tags,*t);*t=_Tmp15;});
({void*_Tmp15=Cyc_Parse_array2ptr(*t,1);*t=_Tmp15;});}}}{
# 830
struct Cyc_Absyn_Tqual _Tmp11=Cyc_Absyn_empty_tqual(tq.loc);void*_Tmp12=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,effc,qb,fn_atts,chks,req,ens,thrw);
# 830
struct Cyc_List_List*_Tmp13=new_atts;return Cyc_Parse_apply_tms(_Tmp11,_Tmp12,_Tmp13,
# 835
_check_null(tms)->tl);}}}}}else{_Tmp6=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f2;{unsigned loc=_Tmp6;
# 837
({int(*_Tmp11)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp11;})(loc,_tag_fat("function declaration without parameter types",sizeof(char),45U));}};}case 4: _Tmp5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;unsigned loc=_Tmp4;
# 844
if(tms->tl==0){
struct _tuple14 _Tmp6;_Tmp6.f0=tq,_Tmp6.f1=t,_Tmp6.f2=ts,_Tmp6.f3=atts;return _Tmp6;}
# 849
({int(*_Tmp6)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp6;})(loc,
_tag_fat("type parameters must appear before function arguments in declarator",sizeof(char),68U));}case 2: _Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_PtrAtts ptratts=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 852
struct Cyc_Absyn_Tqual _Tmp6=tq2;void*_Tmp7=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp8;_Tmp8.elt_type=t,_Tmp8.elt_tq=tq,_Tmp8.ptr_atts=ptratts;_Tmp8;}));struct Cyc_List_List*_Tmp8=atts;return Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);}default: _Tmp4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{unsigned loc=_Tmp4;struct Cyc_List_List*atts2=_Tmp5;
# 857
struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=t;struct Cyc_List_List*_Tmp8=Cyc_List_append(atts,atts2);return Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);}};}}
# 863
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 872
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple15*t){
void*_Tmp0;void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;unsigned _Tmp5;_Tmp5=t->f0;_Tmp4=t->f1;_Tmp3=t->f2;_Tmp2=t->f3;_Tmp1=t->f4;_Tmp0=t->f5;{unsigned varloc=_Tmp5;struct _tuple0*x=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*typ=_Tmp2;struct Cyc_List_List*tvs=_Tmp1;struct Cyc_List_List*atts=_Tmp0;
# 875
Cyc_Lex_register_typedef(x);{
# 877
struct Cyc_Core_Opt*kind;
void*type;
{void*_Tmp6;if(*((int*)typ)==1){_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)typ)->f1;{struct Cyc_Core_Opt*kopt=_Tmp6;
# 881
type=0;
kind=kopt==0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{
kind=0;type=typ;goto _LL3;}_LL3:;}{
# 886
void*_Tmp6=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_Tmp7->tag=8,({struct Cyc_Absyn_Typedefdecl*_Tmp8=({struct Cyc_Absyn_Typedefdecl*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));_Tmp9->name=x,_Tmp9->tvs=tvs,_Tmp9->kind=kind,_Tmp9->defn=type,_Tmp9->atts=atts,_Tmp9->tq=tq,_Tmp9->extern_c=0;_Tmp9;});_Tmp7->f1=_Tmp8;});_Tmp7;});return Cyc_Absyn_new_decl(_Tmp6,loc);}}}}
# 893
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_Tmp1->tag=12,_Tmp1->f1=d,_Tmp1->f2=s;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,d->loc);}
# 897
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return({struct Cyc_Absyn_Stmt*(*_Tmp0)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right;_Tmp0;})(Cyc_Parse_flatten_decl,ds,s);}
# 901
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es,struct Cyc_List_List**rs){
# 905
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
struct Cyc_List_List*renames=0;
for(1;ds!=0;ds=ds->tl){
struct _tuple12 _Tmp0=ds->hd;void*_Tmp1;void*_Tmp2;struct Cyc_Parse_Declarator _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_Parse_Declarator d=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;struct Cyc_Absyn_Exp*rename=_Tmp1;
declarators=({struct _tuple13*_Tmp4=_region_malloc(r,0U,sizeof(struct _tuple13));_Tmp4->tl=declarators,_Tmp4->hd=d;_Tmp4;});
exprs=({struct Cyc_List_List*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp4->hd=e,_Tmp4->tl=exprs;_Tmp4;});
renames=({struct Cyc_List_List*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp4->hd=rename,_Tmp4->tl=renames;_Tmp4;});}}
# 914
({struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(exprs);*es=_Tmp0;});
({struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(renames);*rs=_Tmp0;});
({struct _tuple13*_Tmp0=({struct _tuple13*(*_Tmp1)(struct _tuple13*)=(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;_Tmp1;})(declarators);*decls=_Tmp0;});}
# 924
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 927
struct _RegionHandle _Tmp0=_new_region(0U,"mkrgn");struct _RegionHandle*mkrgn=& _Tmp0;_push_region(mkrgn);
{void*_Tmp1;struct Cyc_Parse_Type_specifier _Tmp2;struct Cyc_Absyn_Tqual _Tmp3;_Tmp3=ds.tq;_Tmp2=ds.type_specs;_Tmp1=ds.attributes;{struct Cyc_Absyn_Tqual tq=_Tmp3;struct Cyc_Parse_Type_specifier tss=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
if(tq.loc==0U)tq.loc=tqual_loc;
if(ds.is_inline)
Cyc_Warn_warn(loc,_tag_fat("inline qualifier on non-function definition",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));{
# 933
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
# 949
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
struct Cyc_List_List*renames=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs,& renames);{
# 954
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es!=0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd!=0){
exps_empty=0;
break;}}}{
# 962
void*base_type=Cyc_Parse_collapse_type_specifiers(tss,loc);
if(declarators==0){
# 966
int _Tmp4;void*_Tmp5;enum Cyc_Absyn_AggrKind _Tmp6;void*_Tmp7;switch(*((int*)base_type)){case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1)->r)){case 0: _Tmp7=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
# 968
({struct Cyc_List_List*_Tmp8=Cyc_List_append(ad->attributes,atts);ad->attributes=_Tmp8;});
ad->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 1: _Tmp7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp7;
# 972
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on enum not supported",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
ed->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpB->tag=7,_TmpB->f1=ed;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: _Tmp7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp7;
# 976
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on datatypes not supported",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));
dd->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=dd;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)){case 24: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.tag==1){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f0;_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f1;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp6;struct _tuple0*n=_Tmp7;struct Cyc_List_List*ts=_Tmp5;
# 980
struct Cyc_List_List*ts2=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),ad->kind=k,ad->sc=s,ad->name=n,ad->tvs=ts2,ad->impl=0,ad->attributes=0,ad->expected_mem_kind=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on type declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{goto _LL25;}case 22: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.tag==2){_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl**tudp=_Tmp7;
# 985
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=*tudp;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.name;_Tmp4=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.is_extensible;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{struct _tuple0*n=_Tmp7;int isx=_Tmp4;struct Cyc_List_List*ts=_Tmp5;
# 988
struct Cyc_List_List*ts2=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Decl*tud=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),_Tmp8->hd=tud,_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 19: _Tmp7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct _tuple0*n=_Tmp7;
# 993
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,ed->name=n,ed->fields=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 20: _Tmp7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct Cyc_List_List*fs=_Tmp7;
# 999
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,({struct _tuple0*_Tmp8=Cyc_Parse_gensym_enum();ed->name=_Tmp8;}),({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=fs;_Tmp9;});ed->fields=_Tmp8;});
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: goto _LL25;}default: _LL25:
 Cyc_Warn_err(loc,_tag_fat("missing declarator",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));{struct Cyc_List_List*_Tmp8=0;_npop_handler(0);return _Tmp8;}};}{
# 1006
struct Cyc_List_List*fields=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);
if(istypedef){
# 1011
if(!exps_empty)
Cyc_Warn_err(loc,_tag_fat("initializer in typedef declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*decls=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp4;})(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_Tmp4=decls;_npop_handler(0);return _Tmp4;}}{
# 1017
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*ds=fields;for(0;ds!=0;(
# 1020
ds=ds->tl,exprs=_check_null(exprs)->tl,renames=_check_null(renames)->tl)){
struct _tuple15*_Tmp4=(struct _tuple15*)ds->hd;void*_Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;unsigned _TmpA;_TmpA=_Tmp4->f0;_Tmp9=_Tmp4->f1;_Tmp8=_Tmp4->f2;_Tmp7=_Tmp4->f3;_Tmp6=_Tmp4->f4;_Tmp5=_Tmp4->f5;{unsigned varloc=_TmpA;struct _tuple0*x=_Tmp9;struct Cyc_Absyn_Tqual tq2=_Tmp8;void*t2=_Tmp7;struct Cyc_List_List*tvs2=_Tmp6;struct Cyc_List_List*atts2=_Tmp5;
if(tvs2!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
if(exprs==0)
({int(*_TmpB)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TmpB;})(loc,_tag_fat("unexpected NULL in parse!",sizeof(char),26U));
if(renames==0)
({int(*_TmpB)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TmpB;})(loc,_tag_fat("unexpected NULL in parse!",sizeof(char),26U));{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)exprs->hd,(struct Cyc_Absyn_Exp*)renames->hd);
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({struct Cyc_Absyn_Decl*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpE=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpF->tag=0,_TmpF->f1=vd;_TmpF;});_TmpD->r=_TmpE;}),_TmpD->loc=loc;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=decls;_TmpB;});}}}}{
# 1034
struct Cyc_List_List*_Tmp4=Cyc_List_imp_rev(decls);_npop_handler(0);return _Tmp4;}}}}}}}}}
# 928
;_pop_region();}
# 1038
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
long long _Tmp0;char _Tmp1;int _Tmp2;switch(x.LongLong_c.tag){case 5: _Tmp2=x.Int_c.val.f1;{int i=_Tmp2;
return(unsigned)i;}case 2: _Tmp1=x.Char_c.val.f1;{char c=_Tmp1;
return(unsigned)c;}case 6: _Tmp0=x.LongLong_c.val.f1;{long long x=_Tmp0;
# 1043
unsigned long long y=(unsigned long long)x;
if(y > 4294967295U)
Cyc_Warn_err(loc,_tag_fat("integer constant too large",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
return(unsigned)x;}default:
# 1048
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Absynpp_cnst2string(x);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err(loc,_tag_fat("expected integer constant but found %s",sizeof(char),39U),_tag_fat(_Tmp4,sizeof(void*),1));});
return 0U;};}
# 1054
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;int _Tmp4;enum Cyc_Absyn_Sign _Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 14: _Tmp6=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple0*x=_Tmp6;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 1058
struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_unknownid_exp(vd->name,p->loc);return Cyc_Absyn_deref_exp(_Tmp7,p->loc);}}else{goto _LL13;}case 5: _Tmp6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp6;
struct Cyc_Absyn_Exp*_Tmp7=Cyc_Parse_pat2exp(p2);return Cyc_Absyn_address_exp(_Tmp7,p->loc);}case 8:
 return Cyc_Absyn_null_exp(p->loc);case 9: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign s=_Tmp5;int i=_Tmp4;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 10: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp3;
return Cyc_Absyn_char_exp(c,p->loc);}case 11: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp4;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 15: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3==0){_Tmp6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _tuple0*x=_Tmp6;struct Cyc_List_List*ps=_Tmp1;
# 1065
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=({struct Cyc_List_List*(*_Tmp7)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;_Tmp7;})(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 16: _Tmp6=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
return e;}default: _LL13:
# 1070
 Cyc_Warn_err(p->loc,_tag_fat("cannot mix patterns and expressions in case",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return Cyc_Absyn_null_exp(p->loc);};}
# 1074
static struct _tuple16 Cyc_Parse_split_seq(struct Cyc_Absyn_Exp*maybe_seq){
void*_Tmp0=maybe_seq->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==9){_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1077
struct _tuple16 _Tmp3;_Tmp3.f0=e1,_Tmp3.f1=e2;return _Tmp3;}}else{
# 1079
struct _tuple16 _Tmp3;_Tmp3.f0=maybe_seq,_Tmp3.f1=0;return _Tmp3;};}
# 1082
static struct Cyc_Absyn_Exp*Cyc_Parse_join_assn(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
if(e1!=0 && e2!=0)return Cyc_Absyn_and_exp(e1,e2,0U);else{
if(e1!=0)return e1;else{
return e2;}}}struct _tuple21{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
# 1087
static struct _tuple21 Cyc_Parse_join_assns(struct _tuple21 a1,struct _tuple21 a2){
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=a1.f0;_Tmp2=a1.f1;_Tmp1=a1.f2;_Tmp0=a1.f3;{struct Cyc_Absyn_Exp*c1=_Tmp3;struct Cyc_Absyn_Exp*r1=_Tmp2;struct Cyc_Absyn_Exp*e1=_Tmp1;struct Cyc_Absyn_Exp*t1=_Tmp0;
void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;_Tmp7=a2.f0;_Tmp6=a2.f1;_Tmp5=a2.f2;_Tmp4=a2.f3;{struct Cyc_Absyn_Exp*c2=_Tmp7;struct Cyc_Absyn_Exp*r2=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Exp*t2=_Tmp4;
struct Cyc_Absyn_Exp*c=Cyc_Parse_join_assn(c1,c2);
struct Cyc_Absyn_Exp*r=Cyc_Parse_join_assn(r1,r2);
struct Cyc_Absyn_Exp*e=Cyc_Parse_join_assn(e1,e2);
struct Cyc_Absyn_Exp*t=Cyc_Parse_join_assn(t1,t2);
struct _tuple21 _Tmp8;_Tmp8.f0=c,_Tmp8.f1=r,_Tmp8.f2=e,_Tmp8.f3=t;return _Tmp8;}}}
# 1097
static void*Cyc_Parse_assign_cvar_pos(struct _fat_ptr posstr,int ovfat,void*cv){
void*_Tmp0;void*_Tmp1;if(*((int*)cv)==3){_Tmp1=(const char**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f6;_Tmp0=(int*)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f7;{const char**pos=(const char**)_Tmp1;int*ov=(int*)_Tmp0;
# 1100
({const char*_Tmp2=(const char*)_untag_fat_ptr_check_bound(posstr,sizeof(char),1U);*pos=_Tmp2;});
*ov=ovfat;
return cv;}}else{
# 1104
return cv;};}
# 1108
static void*Cyc_Parse_typevar2cvar(struct _fat_ptr s){
# 1111
static struct Cyc_Hashtable_Table*cvmap=0;
if(!((unsigned)cvmap))
cvmap=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;_Tmp0;})(101,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);{
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{void*_Tmp2=({void*(*_Tmp3)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=({void*(*_Tmp4)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(void*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;_Tmp4;});struct Cyc_Hashtable_Table*_Tmp4=cvmap;_Tmp3(_Tmp4,({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=s;_Tmp5;}));});_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 1118
struct _fat_ptr kind=Cyc_strchr(s,'_');
struct _fat_ptr name=Cyc_strchr(_fat_ptr_plus(kind,sizeof(char),1),'_');
_fat_ptr_inplace_plus(& name,sizeof(char),1);
if(!Cyc_strncmp(kind,_tag_fat("_PTRBND",sizeof(char),8U),7U)){
void*t=Cyc_Absyn_cvar_type_name(& Cyc_Kinds_ptrbko,name);
({void(*_Tmp4)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*)=({void(*_Tmp5)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*))Cyc_Hashtable_insert;_Tmp5;});struct Cyc_Hashtable_Table*_Tmp5=_check_null(cvmap);struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=s;_Tmp7;});_Tmp4(_Tmp5,_Tmp6,t);});
return t;}else{
# 1127
({int(*_Tmp4)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp4;})(0U,_tag_fat("Constraint variable unknown kind",sizeof(char),33U));}}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}}
# 1133
static void*Cyc_Parse_str2type(unsigned loc,struct _fat_ptr s){
if(!Cyc_strcmp(s,_tag_fat("@fat",sizeof(char),5U)))
return _check_null(Cyc_Tcutil_ptrbnd_cvar_equivalent(Cyc_Absyn_fat_bound_type));else{
# 1137
if(!Cyc_strcmp(s,_tag_fat("@thin @numelts{valueof_t(1U)}",sizeof(char),30U)))
return _check_null(Cyc_Tcutil_ptrbnd_cvar_equivalent(Cyc_Absyn_bounds_one()));}
# 1140
({int(*_Tmp0)(unsigned,struct _fat_ptr)=({int(*_Tmp1)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp1;});unsigned _Tmp1=loc;_Tmp0(_Tmp1,({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=s;_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("Unknown type constant:: %s",sizeof(char),27U),_tag_fat(_Tmp3,sizeof(void*),1));}));});}
# 1143
static void*Cyc_Parse_composite_constraint(enum Cyc_Parse_ConstraintOps op,void*t1,void*t2){
switch((int)op){case Cyc_Parse_C_AND_OP:
 return Cyc_BansheeIf_and_constraint(t1,_check_null(t2));case Cyc_Parse_C_OR_OP:
 return Cyc_BansheeIf_or_constraint(t1,_check_null(t2));case Cyc_Parse_C_NOT_OP:
 return Cyc_BansheeIf_not_constraint(t1);default:
# 1149
({int(*_Tmp0)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp0;})(0U,_tag_fat("Unexpected operator for composite constraint",sizeof(char),45U));};}
# 1153
static void*Cyc_Parse_comparison_constraint(enum Cyc_Parse_ConstraintOps op,void*t1,void*t2){
switch((int)op){case Cyc_Parse_C_EQ_OP:
 return Cyc_BansheeIf_cmpeq_constraint(t1,t2);case Cyc_Parse_C_INCL_OP:
 return Cyc_BansheeIf_inclusion_constraint(t1,t2);default:
# 1158
({int(*_Tmp0)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp0;})(0U,_tag_fat("Unexpected operator for composite constraint",sizeof(char),45U));};}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f0;void*f1;void*f2;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f0;int f1;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f0;enum Cyc_Absyn_AggrKind f1;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _union_YYSTYPE_YY29{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f0;struct Cyc_Parse_Type_specifier f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY36{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f0;int f1;struct Cyc_Absyn_VarargInfo*f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _tuple28{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct _union_YYSTYPE_YY51{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _tuple29{void*f0;void*f1;};struct _union_YYSTYPE_YY53{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY54{int tag;void*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _tuple30{struct Cyc_List_List*f0;unsigned f1;};struct _union_YYSTYPE_YY56{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _union_YYSTYPE_YY59{int tag;void*val;};struct _union_YYSTYPE_YY60{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY62{int tag;struct _tuple21 val;};struct _union_YYSTYPE_YY63{int tag;void*val;};struct _tuple31{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY64{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY65{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY67{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};struct _union_YYSTYPE_YY68{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY69{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YY70{int tag;enum Cyc_Parse_ConstraintOps val;};struct _union_YYSTYPE_YY71{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY72{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY73{int tag;void*val;};struct _union_YYSTYPE_YY74{int tag;int val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YY67 YY67;struct _union_YYSTYPE_YY68 YY68;struct _union_YYSTYPE_YY69 YY69;struct _union_YYSTYPE_YY70 YY70;struct _union_YYSTYPE_YY71 YY71;struct _union_YYSTYPE_YY72 YY72;struct _union_YYSTYPE_YY73 YY73;struct _union_YYSTYPE_YY74 YY74;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1251
static void Cyc_yythrowfail(struct _fat_ptr s){
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=s;_Tmp0;}));}static char _TmpG2[7U]="cnst_t";
# 1214 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2,_TmpG2,_TmpG2 + 7U};
union Cyc_Absyn_Cnst _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Int_tok.tag==1){_Tmp0=yy1->Int_tok.val;{union Cyc_Absyn_Cnst yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1221
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.Int_tok.tag=1U,_Tmp0.Int_tok.val=yy1;return _Tmp0;}static char _TmpG3[5U]="char";
# 1215 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3,_TmpG3,_TmpG3 + 5U};
char _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Char_tok.tag==2){_Tmp0=yy1->Char_tok.val;{char yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1222
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.Char_tok.tag=2U,_Tmp0.Char_tok.val=yy1;return _Tmp0;}static char _TmpG4[13U]="string_t<`H>";
# 1216 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4,_TmpG4,_TmpG4 + 13U};
struct _fat_ptr _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->String_tok.tag==3){_Tmp0=yy1->String_tok.val;{struct _fat_ptr yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1223
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.String_tok.tag=3U,_Tmp0.String_tok.val=yy1;return _Tmp0;}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1219 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG5,_TmpG5,_TmpG5 + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY1.tag==7){_Tmp0=yy1->YY1.val;{struct _tuple22*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1226
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY1.tag=7U,_Tmp0.YY1.val=yy1;return _Tmp0;}static char _TmpG6[11U]="ptrbound_t";
# 1220 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG6,_TmpG6,_TmpG6 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY2.tag==8){_Tmp0=yy1->YY2.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1227
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY2.tag=8U,_Tmp0.YY2.val=yy1;return _Tmp0;}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1221 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG7,_TmpG7,_TmpG7 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY3.tag==9){_Tmp0=yy1->YY3.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1228
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY3.tag=9U,_Tmp0.YY3.val=yy1;return _Tmp0;}static char _TmpG8[6U]="exp_t";
# 1222 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG8,_TmpG8,_TmpG8 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Exp_tok.tag==5){_Tmp0=yy1->Exp_tok.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1229
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.Exp_tok.tag=5U,_Tmp0.Exp_tok.val=yy1;return _Tmp0;}static char _TmpG9[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG9,_TmpG9,_TmpG9 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY4.tag==10){_Tmp0=yy1->YY4.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1237
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY4.tag=10U,_Tmp0.YY4.val=yy1;return _Tmp0;}static char _TmpGA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1231 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGA,_TmpGA,_TmpGA + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY5.tag==11){_Tmp0=yy1->YY5.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1238
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY5.tag=11U,_Tmp0.YY5.val=yy1;return _Tmp0;}static char _TmpGB[9U]="primop_t";
# 1232 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGB,_TmpGB,_TmpGB + 9U};
enum Cyc_Absyn_Primop _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY6.tag==12){_Tmp0=yy1->YY6.val;{enum Cyc_Absyn_Primop yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1239
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY6.tag=12U,_Tmp0.YY6.val=yy1;return _Tmp0;}static char _TmpGC[19U]="opt_t<primop_t,`H>";
# 1233 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGC,_TmpGC,_TmpGC + 19U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY7.tag==13){_Tmp0=yy1->YY7.val;{struct Cyc_Core_Opt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1240
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY7.tag=13U,_Tmp0.YY7.val=yy1;return _Tmp0;}static char _TmpGD[7U]="qvar_t";
# 1234 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGD,_TmpGD,_TmpGD + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->QualId_tok.tag==4){_Tmp0=yy1->QualId_tok.val;{struct _tuple0*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1241
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.QualId_tok.tag=4U,_Tmp0.QualId_tok.val=yy1;return _Tmp0;}static char _TmpGE[7U]="stmt_t";
# 1237 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGE,_TmpGE,_TmpGE + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Stmt_tok.tag==6){_Tmp0=yy1->Stmt_tok.val;{struct Cyc_Absyn_Stmt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1244
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.Stmt_tok.tag=6U,_Tmp0.Stmt_tok.val=yy1;return _Tmp0;}static char _TmpGF[27U]="list_t<switch_clause_t,`H>";
# 1240 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGF,_TmpGF,_TmpGF + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY8.tag==14){_Tmp0=yy1->YY8.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1247
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY8.tag=14U,_Tmp0.YY8.val=yy1;return _Tmp0;}static char _TmpG10[6U]="pat_t";
# 1241 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG10,_TmpG10,_TmpG10 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY9.tag==15){_Tmp0=yy1->YY9.val;{struct Cyc_Absyn_Pat*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1248
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY9.tag=15U,_Tmp0.YY9.val=yy1;return _Tmp0;}static char _TmpG11[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1246 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG11,_TmpG11,_TmpG11 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY10.tag==16){_Tmp0=yy1->YY10.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1253
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY10.tag=16U,_Tmp0.YY10.val=yy1;return _Tmp0;}static char _TmpG12[17U]="list_t<pat_t,`H>";
# 1247 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG12,_TmpG12,_TmpG12 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY11.tag==17){_Tmp0=yy1->YY11.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1254
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY11.tag=17U,_Tmp0.YY11.val=yy1;return _Tmp0;}static char _TmpG13[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1248 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG13,_TmpG13,_TmpG13 + 36U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY12.tag==18){_Tmp0=yy1->YY12.val;{struct _tuple24*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1255
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY12.tag=18U,_Tmp0.YY12.val=yy1;return _Tmp0;}static char _TmpG14[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1249 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG14,_TmpG14,_TmpG14 + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY13.tag==19){_Tmp0=yy1->YY13.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1256
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY13.tag=19U,_Tmp0.YY13.val=yy1;return _Tmp0;}static char _TmpG15[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1250 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG15,_TmpG15,_TmpG15 + 58U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY14.tag==20){_Tmp0=yy1->YY14.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1257
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY14.tag=20U,_Tmp0.YY14.val=yy1;return _Tmp0;}static char _TmpG16[9U]="fndecl_t";
# 1251 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG16,_TmpG16,_TmpG16 + 9U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY15.tag==21){_Tmp0=yy1->YY15.val;{struct Cyc_Absyn_Fndecl*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1258
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY15.tag=21U,_Tmp0.YY15.val=yy1;return _Tmp0;}static char _TmpG17[18U]="list_t<decl_t,`H>";
# 1252 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG17,_TmpG17,_TmpG17 + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY16.tag==22){_Tmp0=yy1->YY16.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1259
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY16.tag=22U,_Tmp0.YY16.val=yy1;return _Tmp0;}static char _TmpG18[12U]="decl_spec_t";
# 1255 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG18,_TmpG18,_TmpG18 + 12U};
struct Cyc_Parse_Declaration_spec _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY17.tag==23){_Tmp0=yy1->YY17.val;{struct Cyc_Parse_Declaration_spec yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1262
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY17.tag=23U,_Tmp0.YY17.val=yy1;return _Tmp0;}static char _TmpG19[41U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)";
# 1256 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG19,_TmpG19,_TmpG19 + 41U};
struct _tuple12 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY18.tag==24){_Tmp0=yy1->YY18.val;{struct _tuple12 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1263
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY18.tag=24U,_Tmp0.YY18.val=yy1;return _Tmp0;}static char _TmpG1A[23U]="declarator_list_t<`yy>";
# 1257 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1A,_TmpG1A,_TmpG1A + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY19.tag==25){_Tmp0=yy1->YY19.val;{struct _tuple11*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1264
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY19.tag=25U,_Tmp0.YY19.val=yy1;return _Tmp0;}static char _TmpG1B[16U]="storage_class_t";
# 1258 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1B,_TmpG1B,_TmpG1B + 16U};
enum Cyc_Parse_Storage_class _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY20.tag==26){_Tmp0=yy1->YY20.val;{enum Cyc_Parse_Storage_class yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1265
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY20.tag=26U,_Tmp0.YY20.val=yy1;return _Tmp0;}static char _TmpG1C[17U]="type_specifier_t";
# 1259 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1C,_TmpG1C,_TmpG1C + 17U};
struct Cyc_Parse_Type_specifier _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY21.tag==27){_Tmp0=yy1->YY21.val;{struct Cyc_Parse_Type_specifier yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1266
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY21.tag=27U,_Tmp0.YY21.val=yy1;return _Tmp0;}static char _TmpG1D[12U]="aggr_kind_t";
# 1261 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1D,_TmpG1D,_TmpG1D + 12U};
enum Cyc_Absyn_AggrKind _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY22.tag==28){_Tmp0=yy1->YY22.val;{enum Cyc_Absyn_AggrKind yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1268
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY22.tag=28U,_Tmp0.YY22.val=yy1;return _Tmp0;}static char _TmpG1E[20U]="$(bool,aggr_kind_t)";
# 1262 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1E,_TmpG1E,_TmpG1E + 20U};
struct _tuple25 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY23.tag==29){_Tmp0=yy1->YY23.val;{struct _tuple25 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1269
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY23.tag=29U,_Tmp0.YY23.val=yy1;return _Tmp0;}static char _TmpG1F[8U]="tqual_t";
# 1263 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1F,_TmpG1F,_TmpG1F + 8U};
struct Cyc_Absyn_Tqual _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY24.tag==30){_Tmp0=yy1->YY24.val;{struct Cyc_Absyn_Tqual yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1270
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY24.tag=30U,_Tmp0.YY24.val=yy1;return _Tmp0;}static char _TmpG20[23U]="list_t<aggrfield_t,`H>";
# 1264 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG20,_TmpG20,_TmpG20 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY25.tag==31){_Tmp0=yy1->YY25.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1271
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY25.tag=31U,_Tmp0.YY25.val=yy1;return _Tmp0;}static char _TmpG21[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1265 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG21,_TmpG21,_TmpG21 + 34U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY26.tag==32){_Tmp0=yy1->YY26.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1272
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY26.tag=32U,_Tmp0.YY26.val=yy1;return _Tmp0;}static char _TmpG22[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1266 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG22,_TmpG22,_TmpG22 + 33U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY27.tag==33){_Tmp0=yy1->YY27.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1273
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY27.tag=33U,_Tmp0.YY27.val=yy1;return _Tmp0;}static char _TmpG23[18U]="declarator_t<`yy>";
# 1267 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG23,_TmpG23,_TmpG23 + 18U};
struct Cyc_Parse_Declarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY28.tag==34){_Tmp0=yy1->YY28.val;{struct Cyc_Parse_Declarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1274
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY28.tag=34U,_Tmp0.YY28.val=yy1;return _Tmp0;}static char _TmpG24[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1268 "parse.y"
static struct _tuple12*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG24,_TmpG24,_TmpG24 + 45U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY29.tag==35){_Tmp0=yy1->YY29.val;{struct _tuple12*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1275
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple12*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY29.tag=35U,_Tmp0.YY29.val=yy1;return _Tmp0;}static char _TmpG25[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1269 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG25,_TmpG25,_TmpG25 + 57U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY30.tag==36){_Tmp0=yy1->YY30.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1276
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY30.tag=36U,_Tmp0.YY30.val=yy1;return _Tmp0;}static char _TmpG26[26U]="abstractdeclarator_t<`yy>";
# 1270 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG26,_TmpG26,_TmpG26 + 26U};
struct Cyc_Parse_Abstractdeclarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY31.tag==37){_Tmp0=yy1->YY31.val;{struct Cyc_Parse_Abstractdeclarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1277
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY31.tag=37U,_Tmp0.YY31.val=yy1;return _Tmp0;}static char _TmpG27[5U]="bool";
# 1271 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG27,_TmpG27,_TmpG27 + 5U};
int _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY32.tag==38){_Tmp0=yy1->YY32.val;{int yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1278
static union Cyc_YYSTYPE Cyc_YY32(int yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY32.tag=38U,_Tmp0.YY32.val=yy1;return _Tmp0;}static char _TmpG28[8U]="scope_t";
# 1272 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG28,_TmpG28,_TmpG28 + 8U};
enum Cyc_Absyn_Scope _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY33.tag==39){_Tmp0=yy1->YY33.val;{enum Cyc_Absyn_Scope yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1279
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY33.tag=39U,_Tmp0.YY33.val=yy1;return _Tmp0;}static char _TmpG29[16U]="datatypefield_t";
# 1273 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG29,_TmpG29,_TmpG29 + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY34.tag==40){_Tmp0=yy1->YY34.val;{struct Cyc_Absyn_Datatypefield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1280
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY34.tag=40U,_Tmp0.YY34.val=yy1;return _Tmp0;}static char _TmpG2A[27U]="list_t<datatypefield_t,`H>";
# 1274 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2A,_TmpG2A,_TmpG2A + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY35.tag==41){_Tmp0=yy1->YY35.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1281
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY35.tag=41U,_Tmp0.YY35.val=yy1;return _Tmp0;}static char _TmpG2B[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1275 "parse.y"
static struct _tuple26 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2B,_TmpG2B,_TmpG2B + 41U};
struct _tuple26 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY36.tag==42){_Tmp0=yy1->YY36.val;{struct _tuple26 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1282
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple26 yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY36.tag=42U,_Tmp0.YY36.val=yy1;return _Tmp0;}static char _TmpG2C[17U]="list_t<var_t,`H>";
# 1276 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2C,_TmpG2C,_TmpG2C + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY37.tag==43){_Tmp0=yy1->YY37.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1283
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY37.tag=43U,_Tmp0.YY37.val=yy1;return _Tmp0;}static char _TmpG2D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1277 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2D,_TmpG2D,_TmpG2D + 31U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY38.tag==44){_Tmp0=yy1->YY38.val;{struct _tuple8*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1284
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY38.tag=44U,_Tmp0.YY38.val=yy1;return _Tmp0;}static char _TmpG2E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1278 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2E,_TmpG2E,_TmpG2E + 42U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY39.tag==45){_Tmp0=yy1->YY39.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1285
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY39.tag=45U,_Tmp0.YY39.val=yy1;return _Tmp0;}static char _TmpG2F[139U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1279 "parse.y"
static struct _tuple27*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2F,_TmpG2F,_TmpG2F + 139U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY40.tag==46){_Tmp0=yy1->YY40.val;{struct _tuple27*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1286
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple27*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY40.tag=46U,_Tmp0.YY40.val=yy1;return _Tmp0;}static char _TmpG30[8U]="types_t";
# 1280 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG30,_TmpG30,_TmpG30 + 8U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY41.tag==47){_Tmp0=yy1->YY41.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1287
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY41.tag=47U,_Tmp0.YY41.val=yy1;return _Tmp0;}static char _TmpG31[24U]="list_t<designator_t,`H>";
# 1282 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG31,_TmpG31,_TmpG31 + 24U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY42.tag==48){_Tmp0=yy1->YY42.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1289
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY42.tag=48U,_Tmp0.YY42.val=yy1;return _Tmp0;}static char _TmpG32[13U]="designator_t";
# 1283 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG32,_TmpG32,_TmpG32 + 13U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY43.tag==49){_Tmp0=yy1->YY43.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1290
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY43.tag=49U,_Tmp0.YY43.val=yy1;return _Tmp0;}static char _TmpG33[7U]="kind_t";
# 1284 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG33,_TmpG33,_TmpG33 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY44.tag==50){_Tmp0=yy1->YY44.val;{struct Cyc_Absyn_Kind*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1291
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY44.tag=50U,_Tmp0.YY44.val=yy1;return _Tmp0;}static char _TmpG34[7U]="type_t";
# 1285 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG34,_TmpG34,_TmpG34 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY45.tag==51){_Tmp0=yy1->YY45.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1292
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY45.tag=51U,_Tmp0.YY45.val=yy1;return _Tmp0;}static char _TmpG35[23U]="list_t<attribute_t,`H>";
# 1286 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG35,_TmpG35,_TmpG35 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY46.tag==52){_Tmp0=yy1->YY46.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1293
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY46.tag=52U,_Tmp0.YY46.val=yy1;return _Tmp0;}static char _TmpG36[12U]="attribute_t";
# 1287 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG36,_TmpG36,_TmpG36 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY47.tag==53){_Tmp0=yy1->YY47.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1294
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY47.tag=53U,_Tmp0.YY47.val=yy1;return _Tmp0;}static char _TmpG37[12U]="enumfield_t";
# 1288 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG37,_TmpG37,_TmpG37 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY48.tag==54){_Tmp0=yy1->YY48.val;{struct Cyc_Absyn_Enumfield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1295
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY48.tag=54U,_Tmp0.YY48.val=yy1;return _Tmp0;}static char _TmpG38[23U]="list_t<enumfield_t,`H>";
# 1289 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG38,_TmpG38,_TmpG38 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY49.tag==55){_Tmp0=yy1->YY49.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1296
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY49.tag=55U,_Tmp0.YY49.val=yy1;return _Tmp0;}static char _TmpG39[11U]="type_opt_t";
# 1290 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG39,_TmpG39,_TmpG39 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY50.tag==56){_Tmp0=yy1->YY50.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1297
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY50.tag=56U,_Tmp0.YY50.val=yy1;return _Tmp0;}static char _TmpG3A[61U]="$(list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)*`H";
# 1292 "parse.y"
static struct _tuple28*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3A,_TmpG3A,_TmpG3A + 61U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY51.tag==57){_Tmp0=yy1->YY51.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1299
static union Cyc_YYSTYPE Cyc_YY51(struct _tuple28*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY51.tag=57U,_Tmp0.YY51.val=yy1;return _Tmp0;}static char _TmpG3B[12U]="effconstr_t";
# 1293 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3B,_TmpG3B,_TmpG3B + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY52.tag==58){_Tmp0=yy1->YY52.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1300
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY52.tag=58U,_Tmp0.YY52.val=yy1;return _Tmp0;}static char _TmpG3C[21U]="$(type_t, type_t)@`H";
# 1294 "parse.y"
static struct _tuple29*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3C,_TmpG3C,_TmpG3C + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY53.tag==59){_Tmp0=yy1->YY53.val;{struct _tuple29*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1301
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple29*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY53.tag=59U,_Tmp0.YY53.val=yy1;return _Tmp0;}static char _TmpG3D[11U]="booltype_t";
# 1295 "parse.y"
static void*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3D,_TmpG3D,_TmpG3D + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY54.tag==60){_Tmp0=yy1->YY54.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1302
static union Cyc_YYSTYPE Cyc_YY54(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY54.tag=60U,_Tmp0.YY54.val=yy1;return _Tmp0;}static char _TmpG3E[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1296 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3E,_TmpG3E,_TmpG3E + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY55.tag==61){_Tmp0=yy1->YY55.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1303
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY55.tag=61U,_Tmp0.YY55.val=yy1;return _Tmp0;}static char _TmpG3F[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1297 "parse.y"
static struct _tuple30*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3F,_TmpG3F,_TmpG3F + 48U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY56.tag==62){_Tmp0=yy1->YY56.val;{struct _tuple30*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1304
static union Cyc_YYSTYPE Cyc_YY56(struct _tuple30*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY56.tag=62U,_Tmp0.YY56.val=yy1;return _Tmp0;}static char _TmpG40[18U]="list_t<qvar_t,`H>";
# 1298 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG40,_TmpG40,_TmpG40 + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY57.tag==63){_Tmp0=yy1->YY57.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1305
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY57.tag=63U,_Tmp0.YY57.val=yy1;return _Tmp0;}static char _TmpG41[12U]="aqualtype_t";
# 1299 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG41,_TmpG41,_TmpG41 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY58.tag==64){_Tmp0=yy1->YY58.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1306
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY58.tag=64U,_Tmp0.YY58.val=yy1;return _Tmp0;}static char _TmpG42[20U]="pointer_qual_t<`yy>";
# 1300 "parse.y"
static void*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG42,_TmpG42,_TmpG42 + 20U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY59.tag==65){_Tmp0=yy1->YY59.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1307
static union Cyc_YYSTYPE Cyc_YY59(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY59.tag=65U,_Tmp0.YY59.val=yy1;return _Tmp0;}static char _TmpG43[21U]="pointer_quals_t<`yy>";
# 1301 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG43,_TmpG43,_TmpG43 + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY60.tag==66){_Tmp0=yy1->YY60.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1308
static union Cyc_YYSTYPE Cyc_YY60(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY60.tag=66U,_Tmp0.YY60.val=yy1;return _Tmp0;}static char _TmpG44[10U]="exp_opt_t";
# 1302 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG44,_TmpG44,_TmpG44 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY61.tag==67){_Tmp0=yy1->YY61.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1309
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY61.tag=67U,_Tmp0.YY61.val=yy1;return _Tmp0;}static char _TmpG45[43U]="$(exp_opt_t,exp_opt_t,exp_opt_t,exp_opt_t)";
# 1303 "parse.y"
static struct _tuple21 Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG45,_TmpG45,_TmpG45 + 43U};
struct _tuple21 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY62.tag==68){_Tmp0=yy1->YY62.val;{struct _tuple21 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1310
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple21 yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY62.tag=68U,_Tmp0.YY62.val=yy1;return _Tmp0;}static char _TmpG46[10U]="raw_exp_t";
# 1305 "parse.y"
static void*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG46,_TmpG46,_TmpG46 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY63.tag==69){_Tmp0=yy1->YY63.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1312
static union Cyc_YYSTYPE Cyc_YY63(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY63.tag=69U,_Tmp0.YY63.val=yy1;return _Tmp0;}static char _TmpG47[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1306 "parse.y"
static struct _tuple31*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG47,_TmpG47,_TmpG47 + 112U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY64.tag==70){_Tmp0=yy1->YY64.val;{struct _tuple31*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1313
static union Cyc_YYSTYPE Cyc_YY64(struct _tuple31*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY64.tag=70U,_Tmp0.YY64.val=yy1;return _Tmp0;}static char _TmpG48[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1307 "parse.y"
static struct _tuple28*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG48,_TmpG48,_TmpG48 + 73U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY65.tag==71){_Tmp0=yy1->YY65.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1314
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple28*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY65.tag=71U,_Tmp0.YY65.val=yy1;return _Tmp0;}static char _TmpG49[28U]="list_t<string_t<`H>@`H, `H>";
# 1308 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG49,_TmpG49,_TmpG49 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY66.tag==72){_Tmp0=yy1->YY66.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1315
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY66.tag=72U,_Tmp0.YY66.val=yy1;return _Tmp0;}static char _TmpG4A[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1309 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY67(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4A,_TmpG4A,_TmpG4A + 38U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY67.tag==73){_Tmp0=yy1->YY67.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1316
static union Cyc_YYSTYPE Cyc_YY67(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY67.tag=73U,_Tmp0.YY67.val=yy1;return _Tmp0;}static char _TmpG4B[26U]="$(string_t<`H>, exp_t)@`H";
# 1310 "parse.y"
static struct _tuple32*Cyc_yyget_YY68(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4B,_TmpG4B,_TmpG4B + 26U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY68.tag==74){_Tmp0=yy1->YY68.val;{struct _tuple32*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1317
static union Cyc_YYSTYPE Cyc_YY68(struct _tuple32*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY68.tag=74U,_Tmp0.YY68.val=yy1;return _Tmp0;}static char _TmpG4C[16U]="exp_maker_fun_t";
# 1311 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY69(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_TmpG4C,_TmpG4C,_TmpG4C + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY69.tag==75){_Tmp0=yy1->YY69.val;{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1318
static union Cyc_YYSTYPE Cyc_YY69(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY69.tag=75U,_Tmp0.YY69.val=yy1;return _Tmp0;}static char _TmpG4D[19U]="enum ConstraintOps";
# 1313 "parse.y"
static enum Cyc_Parse_ConstraintOps Cyc_yyget_YY70(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4D,_TmpG4D,_TmpG4D + 19U};
enum Cyc_Parse_ConstraintOps _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY70.tag==76){_Tmp0=yy1->YY70.val;{enum Cyc_Parse_ConstraintOps yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1320
static union Cyc_YYSTYPE Cyc_YY70(enum Cyc_Parse_ConstraintOps yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY70.tag=76U,_Tmp0.YY70.val=yy1;return _Tmp0;}static char _TmpG4E[60U]="list_t<$(type_t, list_t<BansheeIf::constraint_t,`H>)@`H,`H>";
# 1314 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY71(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4E,_TmpG4E,_TmpG4E + 60U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY71.tag==77){_Tmp0=yy1->YY71.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1321
static union Cyc_YYSTYPE Cyc_YY71(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY71.tag=77U,_Tmp0.YY71.val=yy1;return _Tmp0;}static char _TmpG4F[35U]="list_t<BansheeIf::constraint_t,`H>";
# 1315 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY72(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4F,_TmpG4F,_TmpG4F + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY72.tag==78){_Tmp0=yy1->YY72.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1322
static union Cyc_YYSTYPE Cyc_YY72(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY72.tag=78U,_Tmp0.YY72.val=yy1;return _Tmp0;}static char _TmpG50[24U]="BansheeIf::constraint_t";
# 1316 "parse.y"
static void*Cyc_yyget_YY73(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG50,_TmpG50,_TmpG50 + 24U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY73.tag==79){_Tmp0=yy1->YY73.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1323
static union Cyc_YYSTYPE Cyc_YY73(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY73.tag=79U,_Tmp0.YY73.val=yy1;return _Tmp0;}static char _TmpG51[4U]="int";
# 1318 "parse.y"
static int Cyc_yyget_YY74(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG51,_TmpG51,_TmpG51 + 4U};
int _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY74.tag==80){_Tmp0=yy1->YY74.val;{int yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1325
static union Cyc_YYSTYPE Cyc_YY74(int yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY74.tag=80U,_Tmp0.YY74.val=yy1;return _Tmp0;}struct Cyc_Yyltype{int timestamp;unsigned first_line;unsigned first_column;unsigned last_line;unsigned last_column;};
# 1341
struct Cyc_Yyltype Cyc_yynewloc (void){
struct Cyc_Yyltype _Tmp0;_Tmp0.timestamp=0,_Tmp0.first_line=0U,_Tmp0.first_column=0U,_Tmp0.last_line=0U,_Tmp0.last_column=0U;return _Tmp0;}
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
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _Tmp0=_new_region(0U,"yyregion");struct _RegionHandle*yyregion=& _Tmp0;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _Tmp1;_Tmp1.YYINITIALSVAL.tag=81U,_Tmp1.YYINITIALSVAL.val=0;_Tmp1;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _Tmp1=200U;_tag_fat(_region_calloc(yyregion,0U,sizeof(short),_Tmp1),sizeof(short),_Tmp1);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _Tmp1=200U;({struct Cyc_Yystacktype*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(struct Cyc_Yystacktype)));});({{unsigned _Tmp3=200U;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i].v=yylval,({struct Cyc_Yyltype _Tmp4=Cyc_yynewloc();_Tmp2[i].l=_Tmp4;});}}0;});_Tmp2;});}),sizeof(struct Cyc_Yystacktype),200U);
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
if(yyssp_offset >= (yystacksize - 1)- 14){
# 212
if(yystacksize >= 10000){
Cyc_yyerror(_tag_fat("parser stack overflow",sizeof(char),22U),yystate,yychar);
_throw(& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _Tmp1=(unsigned)yystacksize;_tag_fat(({short*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(short)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=((short*)yyss.curr)[(int)i]:(_Tmp2[i]=0);}}0;});_Tmp2;}),sizeof(short),_Tmp1);});
# 222
struct _fat_ptr yyvs1=({unsigned _Tmp1=(unsigned)yystacksize;
_tag_fat(({struct Cyc_Yystacktype*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(struct Cyc_Yystacktype)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_Tmp2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_Tmp2;}),sizeof(struct Cyc_Yystacktype),_Tmp1);});
# 230
yyss=yyss1;
yyvs=yyvs1;}}
# 240
goto yybackup;
# 242
yybackup:
# 254 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1262U,sizeof(short),yystate));
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
yychar1=yychar > 0 && yychar <= 400?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,401U,sizeof(short),yychar)): 363;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 8241)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8242U,sizeof(short),yyn))!=yychar1)goto yydefault;
# 302
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yytable,8242U,sizeof(short),yyn));
# 309
if(yyn < 0){
# 311
if(yyn==-32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 315
if(yyn==0)goto yyerrlab;}
# 317
if(yyn==1261){
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefact,1262U,sizeof(short),yystate));
if(yyn==0)goto yyerrlab;
# 351
yyreduce:
# 353
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,624U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr_check_bound(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),14U));
if(yylen > 0)
yyval=yyyvsp[0].v;
# 370 "cycbison.simple"
switch((int)yyn){case 1:
# 1323 "parse.y"
 yyval=Cyc_YY74(0);
goto _LL0;case 2:
# 1326 "parse.y"
 yyval=Cyc_YY74(1);
Cyc_Parse_constraint_graph=Cyc_yyget_YY71(& yyyvsp[0].v);
# 1329
goto _LL0;case 3:
# 1333 "parse.y"
 yyval=yyyvsp[0].v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(& yyyvsp[0].v);
# 1336
goto _LL0;case 4:
# 1339 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 5:
# 1343 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_using();
# 1346
goto _LL0;case 6:
# 1347 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 7:
# 1350
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_namespace();
# 1353
goto _LL0;case 8:
# 1354 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 9:
# 1356 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_List_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[4].v));}));
goto _LL0;case 10:  {
# 1358 "parse.y"
int is_c_include=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(& yyyvsp[4].v);
struct _tuple30*_Tmp1=Cyc_yyget_YY56(& yyyvsp[5].v);unsigned _Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*exs=_Tmp3;unsigned wc=_Tmp2;
struct Cyc_List_List*hides=Cyc_yyget_YY57(& yyyvsp[6].v);
if(exs!=0 && hides!=0)
({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp5=_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U);Cyc_Warn_err(_Tmp4,_Tmp5,_tag_fat(0U,sizeof(void*),0));});
if((unsigned)hides && !((int)wc))
wc=Cyc_Position_loc_to_seg(yyyvsp[6].l.first_line);
if(!is_c_include){
if(exs!=0 || cycdecls!=0){
({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp5=_tag_fat("expecting \"C include\"",sizeof(char),22U);Cyc_Warn_err(_Tmp4,_Tmp5,_tag_fat(0U,sizeof(void*),0));});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f0=wc,_TmpA->f1=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),({unsigned _Tmp7=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp6->loc=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}else{
# 1372
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_Tmp8->tag=11,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),({unsigned _Tmp7=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp6->loc=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}}else{
# 1375
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f0=wc,_TmpA->f1=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),({unsigned _Tmp7=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp6->loc=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}
# 1377
goto _LL0;}}case 11:
# 1378 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Porton_d_val,({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 12:
# 1380 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Portoff_d_val,({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 13:
# 1382 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempeston_d_val,({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 14:
# 1384 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempestoff_d_val,({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 15:
# 1385 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 16:
# 1389 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 17:
# 1392
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 18:
# 1397 "parse.y"
 Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})==0)
yyval=Cyc_YY32(0);else{
if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C include",sizeof(char),10U));})==0)
yyval=Cyc_YY32(1);else{
# 1403
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp2=_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U);Cyc_Warn_err(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});
yyval=Cyc_YY32(1);}}
# 1407
goto _LL0;case 19:
# 1410 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 20:
# 1414 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 21:
# 1415 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 22:
# 1419 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 23:
# 1420 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 24:
# 1421 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY57(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 25:
# 1425 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f0=0,_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 26:
# 1426 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 27:
# 1430 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY55(& yyyvsp[2].v);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 28:
# 1431 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f0=0,_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 29:
# 1432 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f0=0,({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 30:
# 1437 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->f0=_Tmp4;}),({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 31:
# 1439 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->f0=_Tmp4;}),({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY55(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 32:
# 1443 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 33:
# 1444 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 34:
# 1448 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp4->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp5=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_new_decl(_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 35:
# 1449 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 36:
# 1450 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 39:
# 1484 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,0,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 40:  {
# 1486 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 41:
# 1498 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,_Tmp3,_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 42:  {
# 1500 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 43:  {
# 1508 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 44:  {
# 1511 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 45:
# 1516 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(& yyyvsp[1].v));yyval=yyyvsp[1].v;
goto _LL0;case 46:
# 1519
 Cyc_Lex_leave_using();
goto _LL0;case 47:
# 1522
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp1=_Tmp2;});_Tmp1;}));yyval=yyyvsp[1].v;
goto _LL0;case 48:
# 1525
 Cyc_Lex_leave_namespace();
goto _LL0;case 49:
# 1528
 ++ Cyc_Parse_inside_noinference_block;
goto _LL0;case 50:
# 1531
 -- Cyc_Parse_inside_noinference_block;
goto _LL0;case 51:
# 1536 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Parse_make_declarations(_Tmp1,0,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 52:
# 1538 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);struct _tuple11*_Tmp2=({struct _tuple11*(*_Tmp3)(struct _tuple11*)=({struct _tuple11*(*_Tmp4)(struct _tuple11*)=(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;_Tmp4;});_Tmp3(Cyc_yyget_YY19(& yyyvsp[1].v));});unsigned _Tmp3=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Parse_make_declarations(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 53:
# 1541
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_let_decl(_Tmp3,_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 54:  {
# 1543 "parse.y"
struct Cyc_List_List*vds=0;
{struct Cyc_List_List*ids=Cyc_yyget_YY37(& yyyvsp[1].v);for(0;ids!=0;ids=ids->tl){
struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0)),({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);qv->f0=_Tmp1;}),qv->f1=(struct _fat_ptr*)ids->hd;
vds=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Vardecl*_Tmp2=({struct _tuple0*_Tmp3=qv;Cyc_Absyn_new_vardecl(0U,_Tmp3,Cyc_Absyn_wildtyp(0),0,0);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=vds;_Tmp1;});}}
# 1548
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(vds);Cyc_Absyn_letv_decl(_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1550
goto _LL0;}case 55:
# 1553 "parse.y"
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line));});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ek);tv->kind=_Tmp1;}),tv->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[4].l.first_line);struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0,0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Tvar*_Tmp3=tv;struct Cyc_Absyn_Vardecl*_Tmp4=vd;Cyc_Absyn_region_decl(_Tmp3,_Tmp4,0,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1559
goto _LL0;}}case 56:  {
# 1561
struct _fat_ptr tvstring=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[1].v);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp2,sizeof(void*),1));});
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_Parse_tvar_ok(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line));});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=tvstring;_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ek);tv->kind=_Tmp1;}),tv->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0,0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Tvar*_Tmp3=tv;struct Cyc_Absyn_Vardecl*_Tmp4=vd;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_YY61(& yyyvsp[2].v);Cyc_Absyn_region_decl(_Tmp3,_Tmp4,_Tmp5,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1568
goto _LL0;}}}case 57:
# 1571 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 58:
# 1573 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("open",sizeof(char),5U));})!=0)({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[3].l.first_line);struct _fat_ptr _Tmp2=_tag_fat("expecting `open'",sizeof(char),17U);Cyc_Warn_err(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});
yyval=Cyc_YY61(Cyc_yyget_Exp_tok(& yyyvsp[3].v));
goto _LL0;case 59:
# 1577
 yyval=yyyvsp[0].v;
goto _LL0;case 60:
# 1578 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 61:
# 1584 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 62:  {
# 1587 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
if((int)two.sc!=7)
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp2=
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U);
# 1589
Cyc_Warn_warn(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});
# 1591
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1594
goto _LL0;}case 63:
# 1595 "parse.y"
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp2=_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U);Cyc_Warn_warn(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});
yyval=yyyvsp[1].v;
# 1598
goto _LL0;case 64:
# 1599 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 65:  {
# 1602 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,({
struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct Cyc_Parse_Type_specifier _Tmp4=two.type_specs;Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,Cyc_yyget_YY21(& yyyvsp[0].v));});_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1607
goto _LL0;}case 66:
# 1608 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.tq=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 67:  {
# 1610 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.tq);});_Tmp1.tq=_Tmp2;}),_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1614
goto _LL0;}case 68:
# 1615 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=1,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 69:  {
# 1618 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=1,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1621
goto _LL0;}case 70:
# 1622 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;case 71:  {
# 1625 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,({
# 1628
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.attributes);});_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;}case 72:
# 1632 "parse.y"
 yyval=Cyc_YY20(4U);
goto _LL0;case 73:
# 1633 "parse.y"
 yyval=Cyc_YY20(5U);
goto _LL0;case 74:
# 1634 "parse.y"
 yyval=Cyc_YY20(3U);
goto _LL0;case 75:
# 1635 "parse.y"
 yyval=Cyc_YY20(1U);
goto _LL0;case 76:
# 1637 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})!=0)
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp2=_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U);Cyc_Warn_err(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});
yyval=Cyc_YY20(2U);
# 1641
goto _LL0;case 77:
# 1641 "parse.y"
 yyval=Cyc_YY20(0U);
goto _LL0;case 78:
# 1643 "parse.y"
 yyval=Cyc_YY20(6U);
goto _LL0;case 79:
# 1648 "parse.y"
 yyval=Cyc_YY46(0);
goto _LL0;case 80:
# 1649 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 81:
# 1653 "parse.y"
 yyval=yyyvsp[3].v;
goto _LL0;case 82:
# 1656
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 83:
# 1657 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 84:
# 1660
 yyval=Cyc_YY47(({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Atts_parse_nullary_att(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 85:
# 1661 "parse.y"
 yyval=Cyc_YY47((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 86:
# 1663 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);unsigned _Tmp3=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);Cyc_Atts_parse_unary_att(_Tmp1,_Tmp2,_Tmp3,Cyc_yyget_Exp_tok(& yyyvsp[2].v));}));
goto _LL0;case 87:
# 1665 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[0].v);struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);unsigned _Tmp5=({
unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[4].l.first_line);Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[4].v));});
# 1665
Cyc_Atts_parse_format_att(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,({
# 1667
unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[6].l.first_line);Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[6].v));}));}));
goto _LL0;case 88:
# 1677 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 89:
# 1679 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_typedef_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[1].v),0,0);});Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 90:
# 1683 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_void_type;Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 91:
# 1684 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_char_type;Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 92:
# 1685 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 93:
# 1686 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_sint_type;Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 94:
# 1687 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 95:
# 1688 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_float_type;Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 96:
# 1689 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_double_type;Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 97:
# 1690 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 98:
# 1691 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 99:
# 1692 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_complex_spec(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 100:
# 1693 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 101:
# 1694 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 102:
# 1696 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 103:
# 1698 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_builtin_type(_tag_fat("__builtin_va_list",sizeof(char),18U),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 104:
# 1700 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 105:
# 1702 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_yyget_YY45(& yyyvsp[0].v);Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 106:
# 1704 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 107:
# 1706 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY44(& yyyvsp[2].v)),0);Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 108:
# 1708 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tuple_type(({struct Cyc_List_List*(*_Tmp2)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp3)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp3;});struct _tuple19*(*_Tmp3)(unsigned,struct _tuple8*)=({struct _tuple19*(*_Tmp4)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_Tmp4;});unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);_Tmp2(_Tmp3,_Tmp4,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[2].v)));}));Cyc_Parse_type_spec(_Tmp1,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 109:
# 1711 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 110:
# 1713 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_eko,0));Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 111:
# 1715 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_aqual_handle_type(Cyc_yyget_YY58(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 112:
# 1717 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 113:
# 1719 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 114:
# 1721 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 115:
# 1726 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Kinds_id_to_kind(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 116:  {
# 1730 "parse.y"
unsigned loc=Cyc_Flags_porting_c_code?Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line): 0U;
yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=1,_Tmp1.q_volatile=0,_Tmp1.q_restrict=0,_Tmp1.real_const=1,_Tmp1.loc=loc;_Tmp1;}));
goto _LL0;}case 117:
# 1732 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=1,_Tmp1.q_restrict=0,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 118:
# 1733 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=0,_Tmp1.q_restrict=1,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 119:  {
# 1739 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl)),({void*_Tmp1=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Enumdecl*_Tmp3=({struct Cyc_Absyn_Enumdecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_Tmp4->sc=2U,({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp4->name=_Tmp5;}),({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY49(& yyyvsp[3].v);_Tmp6->v=_Tmp7;});_Tmp6;});_Tmp4->fields=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});ed->r=_Tmp1;}),({
unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);ed->loc=_Tmp1;});
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=10,_Tmp2->f1=ed,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 1743
goto _LL0;}case 120:
# 1744 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(& yyyvsp[1].v),0);Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 121:
# 1746 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY49(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 122:
# 1752 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),_Tmp1->tag=0,({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1->loc=_Tmp2;});_Tmp1;}));
goto _LL0;case 123:
# 1754 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->tag=_Tmp2;}),({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1->loc=_Tmp2;});_Tmp1;}));
goto _LL0;case 124:
# 1758 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 125:
# 1759 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 126:
# 1760 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY49(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 127:
# 1766 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp2->tag=7,({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0,({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY25(& yyyvsp[2].v);_Tmp2->f3=_Tmp3;});_Tmp2;});Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 128:  {
# 1770 "parse.y"
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[4].l.first_line);_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_yyget_YY41(& yyyvsp[4].v));});
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[5].v);
struct _tuple28 _Tmp1=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct Cyc_Absyn_TypeDecl*td=({enum Cyc_Absyn_AggrKind _Tmp4=Cyc_yyget_YY23(& yyyvsp[0].v).f1;struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp6=ts;struct Cyc_Absyn_AggrdeclImpl*_Tmp7=({
struct Cyc_List_List*_Tmp8=exist_ts;struct Cyc_List_List*_Tmp9=ec;struct Cyc_List_List*_TmpA=qb;struct Cyc_List_List*_TmpB=Cyc_yyget_YY25(& yyyvsp[6].v);Cyc_Absyn_aggrdecl_impl(_Tmp8,_Tmp9,_TmpA,_TmpB,Cyc_yyget_YY23(& yyyvsp[0].v).f0);});
# 1774
Cyc_Absyn_aggr_tdecl(_Tmp4,2U,_Tmp5,_Tmp6,_Tmp7,0,
# 1776
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});
yyval=Cyc_YY21(({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp5->tag=10,_Tmp5->f1=td,_Tmp5->f2=0;_Tmp5;});Cyc_Parse_type_spec(_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 1779
goto _LL0;}}case 129:
# 1780 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_AggrInfo _Tmp2=({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY23(& yyyvsp[0].v).f1;struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_UnknownAggr(_Tmp3,_Tmp4,Cyc_yyget_YY23(& yyyvsp[0].v).f0?({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=(void*)1;_Tmp5;}): 0);});Cyc_Absyn_aggr_type(_Tmp2,
Cyc_yyget_YY41(& yyyvsp[2].v));});
# 1780
Cyc_Parse_type_spec(_Tmp1,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 130:
# 1785 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp1;_Tmp1.f0=1,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[1].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 131:
# 1786 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp1;_Tmp1.f0=0,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 132:
# 1789
 yyval=Cyc_YY22(0U);
goto _LL0;case 133:
# 1790 "parse.y"
 yyval=Cyc_YY22(1U);
goto _LL0;case 134:
# 1793
 yyval=Cyc_YY41(0);
goto _LL0;case 135:
# 1794 "parse.y"
 yyval=Cyc_YY41(Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 136:
# 1799 "parse.y"
 yyval=Cyc_YY25(0);
goto _LL0;case 137:  {
# 1801 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*x=Cyc_yyget_YY26(& yyyvsp[0].v);for(0;x!=0;x=x->tl){
decls=Cyc_List_imp_append((struct Cyc_List_List*)x->hd,decls);}}{
# 1805
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags!=0)
({void(*_Tmp1)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;_Tmp1;})(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY25(decls);
# 1810
goto _LL0;}}case 138:
# 1814 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 139:
# 1815 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY26(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 140:
# 1821 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple11));_Tmp1->tl=0,({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 141:
# 1823 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple11));({struct _tuple11*_Tmp2=Cyc_yyget_YY19(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;}),({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 142:
# 1827 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0,_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 143:
# 1828 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0,({struct Cyc_Absyn_Exp*_Tmp2=({void*_Tmp3=Cyc_yyget_YY63(& yyyvsp[2].v);Cyc_Absyn_new_exp(_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line));});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 144:
# 1829 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 145:
# 1830 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=({void*_Tmp3=Cyc_yyget_YY63(& yyyvsp[2].v);Cyc_Absyn_new_exp(_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line));});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 146:  {
# 1835 "parse.y"
struct _RegionHandle _Tmp1=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp1;_push_region(temp);
{struct _tuple26 _Tmp2=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f0;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tspecs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
if(tq.loc==0U)({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);tq.loc=_Tmp6;});{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*x=Cyc_yyget_YY30(& yyyvsp[1].v);for(0;x!=0;x=x->tl){
struct _tuple12*_Tmp6=(struct _tuple12*)x->hd;void*_Tmp7;void*_Tmp8;struct Cyc_Parse_Declarator _Tmp9;_Tmp9=_Tmp6->f0;_Tmp8=_Tmp6->f1;_Tmp7=_Tmp6->f2;{struct Cyc_Parse_Declarator d=_Tmp9;struct Cyc_Absyn_Exp*wd=_Tmp8;struct Cyc_Absyn_Exp*wh=_Tmp7;
decls=({struct _tuple13*_TmpA=_region_malloc(temp,0U,sizeof(struct _tuple13));_TmpA->tl=decls,_TmpA->hd=d;_TmpA;});
widths_and_reqs=({struct Cyc_List_List*_TmpA=_region_malloc(temp,0U,sizeof(struct Cyc_List_List));({struct _tuple16*_TmpB=({struct _tuple16*_TmpC=_region_malloc(temp,0U,sizeof(struct _tuple16));_TmpC->f0=wd,_TmpC->f1=wh;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=widths_and_reqs;_TmpA;});}}}{
# 1845
void*t=({struct Cyc_Parse_Type_specifier _Tmp6=tspecs;Cyc_Parse_speclist2typ(_Tmp6,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});
struct Cyc_List_List*info=({struct _RegionHandle*_Tmp6=temp;struct _RegionHandle*_Tmp7=temp;struct Cyc_List_List*_Tmp8=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1846
Cyc_List_rzip(_Tmp6,_Tmp7,_Tmp8,widths_and_reqs);});
# 1848
yyval=Cyc_YY25(({struct Cyc_List_List*(*_Tmp6)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp7)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp7;});unsigned _Tmp7=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp6(Cyc_Parse_make_aggr_field,_Tmp7,info);}));
# 1850
_npop_handler(0);goto _LL0;}}}}
# 1836
;_pop_region();}case 147:
# 1857 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 148:  {
# 1859 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 149:
# 1861 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 150:  {
# 1863 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f0);});_Tmp1.f0=_Tmp2;}),_Tmp1.f1=two.f1,_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 151:
# 1865 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 152:  {
# 1867 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,_Tmp1.f1=two.f1,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f2);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;}case 153:
# 1873 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 154:  {
# 1875 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 155:
# 1877 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 156:  {
# 1879 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f0);});_Tmp1.f0=_Tmp2;}),_Tmp1.f1=two.f1,_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 157:
# 1881 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 158:  {
# 1883 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,_Tmp1.f1=two.f1,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f2);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;}case 159:
# 1889 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({struct _tuple12*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 160:
# 1891 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({struct _tuple12*_Tmp2=Cyc_yyget_YY29(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY30(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 161:
# 1896 "parse.y"
 yyval=Cyc_YY29(({struct _tuple12*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple12));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_YY61(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 162:
# 1900 "parse.y"
 yyval=Cyc_YY29(({struct _tuple12*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple12));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f0=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->f0.id=_Tmp2;}),_Tmp1->f0.varloc=0U,_Tmp1->f0.tms=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
# 1902
goto _LL0;case 163:
# 1905 "parse.y"
 yyval=Cyc_YY29(({struct _tuple12*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple12));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f0=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->f0.id=_Tmp2;}),_Tmp1->f0.varloc=0U,_Tmp1->f0.tms=0,_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
# 1907
goto _LL0;case 164:
# 1908 "parse.y"
 yyval=Cyc_YY29(({struct _tuple12*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple12));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 165:
# 1912 "parse.y"
 yyval=Cyc_YY61(Cyc_yyget_Exp_tok(& yyyvsp[2].v));
goto _LL0;case 166:
# 1913 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 167:  {
# 1919 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_Absyn_TypeDecl*dd=({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=ts;struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY35(& yyyvsp[4].v);_Tmp4->v=_Tmp5;});_Tmp4;});int _Tmp4=is_extensible;Cyc_Absyn_datatype_tdecl(2U,_Tmp1,_Tmp2,_Tmp3,_Tmp4,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=10,_Tmp2->f1=dd,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 1925
goto _LL0;}case 168:  {
# 1926 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeInfo _Tmp2=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 1929
goto _LL0;}case 169:  {
# 1930 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeFieldInfo _Tmp2=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.datatype_name=_Tmp4;}),({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[3].v);_Tmp3.field_name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_field_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[4].v));});Cyc_Parse_type_spec(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 1933
goto _LL0;}case 170:
# 1936 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 171:
# 1937 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 172:
# 1941 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 173:
# 1942 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 174:
# 1943 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 175:
# 1944 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 176:
# 1948 "parse.y"
 yyval=Cyc_YY33(2U);
goto _LL0;case 177:
# 1949 "parse.y"
 yyval=Cyc_YY33(3U);
goto _LL0;case 178:
# 1950 "parse.y"
 yyval=Cyc_YY33(0U);
goto _LL0;case 179:
# 1954 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=0,({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1->loc=_Tmp2;}),({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;case 180:  {
# 1956 "parse.y"
struct Cyc_List_List*typs=({struct Cyc_List_List*(*_Tmp1)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});struct _tuple19*(*_Tmp2)(unsigned,struct _tuple8*)=({struct _tuple19*(*_Tmp3)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_Tmp3;});unsigned _Tmp3=Cyc_Position_loc_to_seg(yyyvsp[3].l.first_line);_Tmp1(_Tmp2,_Tmp3,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[3].v)));});
yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=typs,({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1->loc=_Tmp2;}),({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;}case 181:
# 1962 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 182:  {
# 1964 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 183:
# 1971 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 184:  {
# 1973 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 185:
# 1978 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1.varloc=_Tmp2;}),_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 186:
# 1979 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 187:  {
# 1983 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,({unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp4->f1=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1987
goto _LL0;}case 188:
# 1988 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[3].l.first_line);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 189:
# 1990 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[4].l.first_line);_Tmp5->f3=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 190:  {
# 1993 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY62(& yyyvsp[4].v);void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;_TmpC=_Tmp8.f0;_TmpB=_Tmp8.f1;_TmpA=_Tmp8.f2;_Tmp9=_Tmp8.f3;{struct Cyc_Absyn_Exp*chk=_TmpC;struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _TmpD;({struct _tuple0*_TmpE=Cyc_yyget_YY28(& yyyvsp[0].v).id;_TmpD.id=_TmpE;}),({unsigned _TmpE=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_TmpD.varloc=_TmpE;}),({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp10=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp11=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp11->tag=3,({void*_Tmp12=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp13=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp13->tag=1,_Tmp13->f1=lis,_Tmp13->f2=b,_Tmp13->f3=c,_Tmp13->f4=eff,_Tmp13->f5=ec,_Tmp13->f6=qb,_Tmp13->f7=chk,_Tmp13->f8=req,_Tmp13->f9=ens,_Tmp13->f10=thrws;_Tmp13;});_Tmp11->f1=_Tmp12;});_Tmp11;});_TmpF->hd=_Tmp10;}),({struct Cyc_List_List*_Tmp10=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_TmpF->tl=_Tmp10;});_TmpF;});_TmpD.tms=_TmpE;});_TmpD;}));
# 1997
goto _LL0;}}}case 191:
# 1998 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),({unsigned _Tmp8=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp7->f2=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 192:  {
# 2001
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2004
goto _LL0;}case 193:
# 2005 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2005
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2008
goto _LL0;case 194:
# 2012 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1.varloc=_Tmp2;}),_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 195:
# 2013 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1.varloc=_Tmp2;}),_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 196:
# 2014 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 197:  {
# 2018 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,({unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp4->f1=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 2022
goto _LL0;}case 198:  {
# 2023 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[3].l.first_line);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 199:  {
# 2027 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[4].l.first_line);_Tmp5->f3=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2031
goto _LL0;}case 200:  {
# 2032 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY62(& yyyvsp[4].v);void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;_TmpC=_Tmp8.f0;_TmpB=_Tmp8.f1;_TmpA=_Tmp8.f2;_Tmp9=_Tmp8.f3;{struct Cyc_Absyn_Exp*chk=_TmpC;struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _TmpD;_TmpD.id=one.id,_TmpD.varloc=one.varloc,({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp10=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp11=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp11->tag=3,({void*_Tmp12=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp13=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp13->tag=1,_Tmp13->f1=lis,_Tmp13->f2=b,_Tmp13->f3=c,_Tmp13->f4=eff,_Tmp13->f5=ec,_Tmp13->f6=qb,_Tmp13->f7=chk,_Tmp13->f8=req,_Tmp13->f9=ens,_Tmp13->f10=thrws;_Tmp13;});_Tmp11->f1=_Tmp12;});_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=one.tms;_TmpF;});_TmpD.tms=_TmpE;});_TmpD;}));
# 2037
goto _LL0;}}}case 201:  {
# 2038 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),({unsigned _Tmp8=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp7->f2=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 202:  {
# 2042
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 203:  {
# 2046 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 204:
# 2052 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 205:
# 2053 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY27(& yyyvsp[1].v));}));
goto _LL0;case 206:  {
# 2057 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(& yyyvsp[3].v)!=0)
ans=({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp3->tag=5,({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[3].l.first_line);_Tmp3->f1=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY46(& yyyvsp[3].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=ans;_Tmp1;});{
# 2061
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _Tmp1=*Cyc_yyget_YY1(& yyyvsp[0].v);void*_Tmp2;void*_Tmp3;unsigned _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{unsigned ploc=_Tmp4;void*nullable=_Tmp3;void*bound=_Tmp2;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));_Tmp5->ptr_loc=ploc,({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);_Tmp5->rgn_loc=_Tmp6;}),({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp5->zt_loc=_Tmp6;});_Tmp5;});{
void*mod=({struct _RegionHandle*_Tmp5=yyr;struct Cyc_Absyn_PtrLoc*_Tmp6=ptrloc;void*_Tmp7=nullable;void*_Tmp8=bound;void*_Tmp9=Cyc_yyget_YY45(& yyyvsp[2].v);struct Cyc_List_List*_TmpA=Cyc_yyget_YY60(& yyyvsp[1].v);Cyc_Parse_make_pointer_mod(_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_yyget_YY24(& yyyvsp[4].v));});
ans=({struct Cyc_List_List*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));_Tmp5->hd=mod,_Tmp5->tl=ans;_Tmp5;});
yyval=Cyc_YY27(ans);
# 2069
goto _LL0;}}}}case 207:
# 2071
 yyval=Cyc_YY60(0);
goto _LL0;case 208:
# 2072 "parse.y"
 yyval=Cyc_YY60(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY59(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY60(& yyyvsp[1].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 209:
# 2077 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 210:
# 2079 "parse.y"
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp2=_tag_fat("@region qualifiers are deprecated; use @effect instead",sizeof(char),55U);Cyc_Warn_warn(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});
yyval=Cyc_YY59((void*)({struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=2,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 211:
# 2081 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=2,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 212:
# 2082 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=3;_Tmp1;}));
goto _LL0;case 213:
# 2083 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=4;_Tmp1;}));
goto _LL0;case 214:
# 2084 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=7;_Tmp1;}));
goto _LL0;case 215:
# 2085 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=5;_Tmp1;}));
goto _LL0;case 216:
# 2086 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=6;_Tmp1;}));
goto _LL0;case 217:
# 2087 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=8;_Tmp1;}));
goto _LL0;case 218:
# 2088 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=9;_Tmp1;}));
goto _LL0;case 219:
# 2089 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=10,({void*_Tmp2=Cyc_yyget_YY58(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 220:  {
# 2090 "parse.y"
void*aq=({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));});yyval=Cyc_YY59((void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=10,_Tmp1->f1=aq;_Tmp1;}));
goto _LL0;}case 221:
# 2094 "parse.y"
 yyval=Cyc_YY58(Cyc_yyget_YY58(& yyyvsp[0].v));
goto _LL0;case 222:
# 2100 "parse.y"
 yyval=Cyc_YY58(({void*_Tmp1=Cyc_yyget_YY45(& yyyvsp[0].v);Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 2102
goto _LL0;case 223:
# 2104 "parse.y"
 yyval=Cyc_YY58(({void*_Tmp1=Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 2106
goto _LL0;case 224:
# 2110 "parse.y"
 goto _LL0;case 225:
# 2110 "parse.y"
 yyval=Cyc_YY58(({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 226:
# 2116 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_Tmp1->f2=Cyc_Absyn_fat_bound_type:({void*_Tmp2=({struct _fat_ptr _Tmp3=Cyc_Position_string_of_segment(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));Cyc_Parse_assign_cvar_pos(_Tmp3,0,Cyc_yyget_YY2(& yyyvsp[1].v));});_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 227:
# 2117 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=Cyc_Absyn_false_type,({void*_Tmp2=({struct _fat_ptr _Tmp3=Cyc_Position_string_of_segment(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));Cyc_Parse_assign_cvar_pos(_Tmp3,0,Cyc_yyget_YY2(& yyyvsp[1].v));});_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 228:
# 2118 "parse.y"
 yyval=Cyc_YY1((Cyc_Flags_override_fat && Cyc_Parse_inside_noinference_block==0)&& Cyc_Flags_interproc?({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));
({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=Cyc_Absyn_true_type,({void*_Tmp2=({struct _fat_ptr _Tmp3=Cyc_Position_string_of_segment(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));Cyc_Parse_assign_cvar_pos(_Tmp3,1,Cyc_Absyn_cvar_type(& Cyc_Kinds_ptrbko));});_Tmp1->f2=_Tmp2;});_Tmp1;}):({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));
({unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=Cyc_Absyn_true_type,_Tmp1->f2=Cyc_Absyn_fat_bound_type;_Tmp1;}));
goto _LL0;case 229:
# 2123
 yyval=Cyc_YY2(Cyc_Flags_interproc && Cyc_Parse_inside_noinference_block==0?Cyc_Absyn_cvar_type(& Cyc_Kinds_ptrbko): Cyc_Absyn_bounds_one());
goto _LL0;case 230:
# 2124 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 231:
# 2127 "parse.y"
 if(!Cyc_Flags_resolve)
({int(*_Tmp1)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp1;})(0U,_tag_fat("Type variable not permitted in pointer bound",sizeof(char),45U));
yyval=Cyc_YY2(Cyc_Parse_typevar2cvar(Cyc_yyget_String_tok(& yyyvsp[1].v)));
# 2131
goto _LL0;case 232:
# 2134 "parse.y"
 yyval=Cyc_YY54(Cyc_Tcutil_any_bool(0));
goto _LL0;case 233:
# 2135 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_true_type);
goto _LL0;case 234:
# 2136 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_false_type);
goto _LL0;case 235:
# 2140 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 236:
# 2141 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 237:
# 2145 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_eko,0));
goto _LL0;case 238:  {
# 2146 "parse.y"
struct Cyc_List_List*es=Cyc_yyget_YY41(& yyyvsp[0].v);if(Cyc_List_length(es)==1)yyval=Cyc_YY45((void*)_check_null(es)->hd);else{yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[0].v)));}
goto _LL0;}case 239:
# 2147 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_eko,0));
goto _LL0;case 240:
# 2158 "parse.y"
 yyval=Cyc_YY24(Cyc_Absyn_empty_tqual(Cyc_Position_loc_to_seg((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l.first_line)));
goto _LL0;case 241:
# 2159 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp1,Cyc_yyget_YY24(& yyyvsp[1].v));}));
goto _LL0;case 242:  {
# 2164 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[1].v);
struct _tuple28 _Tmp1=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));_Tmp4->f0=0,_Tmp4->f1=0,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[0].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=ec,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 243:  {
# 2168 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[2].v);
struct _tuple28 _Tmp1=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=0,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[1].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=ec,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 244:  {
# 2172 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[4].v);
struct _tuple28 _Tmp1=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=1,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=ec,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 245:  {
# 2177
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[1].v);v->inject=_Tmp5;});{
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[4].v);
struct _tuple28 _Tmp5=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp6;_Tmp6.f0=0,_Tmp6.f1=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*ec=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY40(({struct _tuple27*_Tmp8=_cycalloc(sizeof(struct _tuple27));_Tmp8->f0=0,_Tmp8->f1=0,_Tmp8->f2=v,({void*_Tmp9=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp8->f3=_Tmp9;}),_Tmp8->f4=ec,_Tmp8->f5=qb;_Tmp8;}));
# 2183
goto _LL0;}}}}case 246:  {
# 2185
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[4].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[3].v);v->inject=_Tmp5;});{
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[6].v);
struct _tuple28 _Tmp5=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp6;_Tmp6.f0=0,_Tmp6.f1=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*ec=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY40(({struct _tuple27*_Tmp8=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp9=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp8->f0=_Tmp9;}),_Tmp8->f1=0,_Tmp8->f2=v,({void*_Tmp9=Cyc_yyget_YY50(& yyyvsp[5].v);_Tmp8->f3=_Tmp9;}),_Tmp8->f4=ec,_Tmp8->f5=qb;_Tmp8;}));
# 2191
goto _LL0;}}}}case 247:
# 2195 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 248:
# 2196 "parse.y"
 yyval=Cyc_YY50(({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 249:
# 2199
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp3->tag=1,_Tmp3->f1=0;_Tmp3;});void*_Tmp3=Cyc_yyget_YY50(& yyyvsp[1].v);Cyc_Parse_id2type(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 250:
# 2200 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=Cyc_Kinds_kind_to_bound(Cyc_yyget_YY44(& yyyvsp[2].v));void*_Tmp3=Cyc_yyget_YY50(& yyyvsp[3].v);Cyc_Parse_id2type(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 251:
# 2203
 yyval=Cyc_YY50(0);
goto _LL0;case 252:
# 2204 "parse.y"
 yyval=Cyc_YY50(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 253:
# 2208 "parse.y"
 yyval=Cyc_YY51(0);
goto _LL0;case 254:
# 2209 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 255:
# 2214 "parse.y"
 yyval=Cyc_YY51(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=({void*_Tmp3[1];({void*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 256:
# 2216 "parse.y"
 yyval=Cyc_YY51(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f0=0,({struct Cyc_List_List*_Tmp2=({struct _tuple29*_Tmp3[1];({struct _tuple29*_Tmp4=Cyc_yyget_YY53(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(struct _tuple29*),1));});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 257:  {
# 2219 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(& yyyvsp[2].v);
struct _tuple28 _Tmp1=*_check_null(rest);void*_Tmp2;_Tmp2=_Tmp1.f0;{struct Cyc_List_List*rpo=_Tmp2;
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp5=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp4->hd=_Tmp5;}),_Tmp4->tl=rpo;_Tmp4;});(*rest).f0=_Tmp3;});
yyval=Cyc_YY51(rest);
# 2224
goto _LL0;}}case 258:  {
# 2226 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(& yyyvsp[2].v);
struct _tuple28 _Tmp1=*_check_null(rest);void*_Tmp2;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*qb=_Tmp2;
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp5=Cyc_yyget_YY53(& yyyvsp[0].v);_Tmp4->hd=_Tmp5;}),_Tmp4->tl=qb;_Tmp4;});(*rest).f1=_Tmp3;});
yyval=Cyc_YY51(rest);
# 2231
goto _LL0;}}case 259:  {
# 2236 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct)),kb->tag=2,kb->f1=0,kb->f2=& Cyc_Kinds_ek;{
void*t=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);void*_Tmp2=(void*)kb;Cyc_Parse_id2type(_Tmp1,_Tmp2,0,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line));});
yyval=Cyc_YY52((void*)({struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct));_Tmp1->tag=2,({void*_Tmp2=Cyc_Parse_effect_from_atomic(Cyc_yyget_YY41(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=t;_Tmp1;}));
# 2240
goto _LL0;}}case 260:
# 2242 "parse.y"
 yyval=Cyc_YY52((void*)({struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_Tmp1->tag=1,({void*_Tmp2=Cyc_Parse_effect_from_atomic(Cyc_yyget_YY41(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),({
void*_Tmp2=Cyc_Parse_effect_from_atomic(Cyc_yyget_YY41(& yyyvsp[2].v));_Tmp1->f2=_Tmp2;});_Tmp1;}));
# 2245
goto _LL0;case 261:
# 2247 "parse.y"
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Parse_check_single_constraint(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));});
yyval=Cyc_YY52((void*)({struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct));_Tmp1->tag=0,({void*_Tmp2=Cyc_Parse_effect_from_atomic(Cyc_yyget_YY41(& yyyvsp[2].v));_Tmp1->f1=_Tmp2;});_Tmp1;}));
# 2250
goto _LL0;case 262:
# 2255 "parse.y"
 yyval=Cyc_YY53(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->f0=_Tmp2;}),({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
# 2257
goto _LL0;case 263:
# 2260 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_al_qual_type);
goto _LL0;case 264:
# 2261 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_un_qual_type);
goto _LL0;case 265:
# 2262 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rc_qual_type);
goto _LL0;case 266:
# 2263 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rtd_qual_type);
goto _LL0;case 267:
# 2264 "parse.y"
 yyval=Cyc_YY58(({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 268:
# 2269 "parse.y"
 yyval=Cyc_YY45(Cyc_yyget_YY58(& yyyvsp[0].v));
goto _LL0;case 269:
# 2272 "parse.y"
 yyval=Cyc_YY45(({void*_Tmp1=Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 2274
goto _LL0;case 270:  {
# 2279 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct)),kb->tag=0,kb->f1=& Cyc_Kinds_aqk;
yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=(void*)kb;Cyc_Parse_id2type(_Tmp1,_Tmp2,0,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 2282
goto _LL0;}case 271:
# 2284 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v)));
# 2286
goto _LL0;case 272:
# 2313 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 273:
# 2315 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_zstrcmp(_Tmp1,_tag_fat("inject",sizeof(char),7U));})!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;({unsigned _Tmp3=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Warn_err2(_Tmp3,_tag_fat(_Tmp2,sizeof(void*),1));});});
yyval=Cyc_YY32(1);
# 2319
goto _LL0;case 274:
# 2322 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 275:
# 2323 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY41(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY41(& yyyvsp[2].v));}));
goto _LL0;case 276:
# 2327 "parse.y"
 yyval=Cyc_YY41(0);
goto _LL0;case 277:
# 2328 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 278:
# 2329 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 279:
# 2331 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(& yyyvsp[0].v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 2334
goto _LL0;case 280:
# 2339 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 281:
# 2341 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 282:
# 2346 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 283:
# 2347 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY39(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 284:  {
# 2353 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)({unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);tq.loc=_Tmp5;});{
struct Cyc_Parse_Declarator _Tmp5=Cyc_yyget_YY28(& yyyvsp[1].v);void*_Tmp6;unsigned _Tmp7;void*_Tmp8;_Tmp8=_Tmp5.id;_Tmp7=_Tmp5.varloc;_Tmp6=_Tmp5.tms;{struct _tuple0*qv=_Tmp8;unsigned varloc=_Tmp7;struct Cyc_List_List*tms=_Tmp6;
void*t=({struct Cyc_Parse_Type_specifier _Tmp9=tspecs;Cyc_Parse_speclist2typ(_Tmp9,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});
struct _tuple14 _Tmp9=Cyc_Parse_apply_tms(tq,t,atts,tms);void*_TmpA;void*_TmpB;void*_TmpC;struct Cyc_Absyn_Tqual _TmpD;_TmpD=_Tmp9.f0;_TmpC=_Tmp9.f1;_TmpB=_Tmp9.f2;_TmpA=_Tmp9.f3;{struct Cyc_Absyn_Tqual tq2=_TmpD;void*t2=_TmpC;struct Cyc_List_List*tvs=_TmpB;struct Cyc_List_List*atts2=_TmpA;
if(tvs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("parameter with bad type params",sizeof(char),31U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;({unsigned _Tmp10=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);Cyc_Warn_err2(_Tmp10,_tag_fat(_TmpF,sizeof(void*),1));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("parameter cannot be qualified with a namespace",sizeof(char),47U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;({unsigned _Tmp10=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Warn_err2(_Tmp10,_tag_fat(_TmpF,sizeof(void*),1));});});{
struct _fat_ptr*idopt=(*qv).f1;
if(atts2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("extra attributes on parameter, ignoring",sizeof(char),40U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;({unsigned _Tmp10=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Warn_warn2(_Tmp10,_tag_fat(_TmpF,sizeof(void*),1));});});
yyval=Cyc_YY38(({struct _tuple8*_TmpE=_cycalloc(sizeof(struct _tuple8));_TmpE->f0=idopt,_TmpE->f1=tq2,_TmpE->f2=t2;_TmpE;}));
# 2367
goto _LL0;}}}}}}case 285:  {
# 2368 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)({unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);tq.loc=_Tmp5;});{
void*t=({struct Cyc_Parse_Type_specifier _Tmp5=tspecs;Cyc_Parse_speclist2typ(_Tmp5,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});
if(atts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({unsigned _Tmp7=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Warn_warn2(_Tmp7,_tag_fat(_Tmp6,sizeof(void*),1));});});
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=tq,_Tmp5->f2=t;_Tmp5;}));
# 2375
goto _LL0;}}}case 286:  {
# 2376 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)({unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);tq.loc=_Tmp5;});{
void*t=({struct Cyc_Parse_Type_specifier _Tmp5=tspecs;Cyc_Parse_speclist2typ(_Tmp5,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});
struct Cyc_List_List*tms=Cyc_yyget_YY31(& yyyvsp[1].v).tms;
struct _tuple14 _Tmp5=Cyc_Parse_apply_tms(tq,t,atts,tms);void*_Tmp6;void*_Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;_Tmp9=_Tmp5.f0;_Tmp8=_Tmp5.f1;_Tmp7=_Tmp5.f2;_Tmp6=_Tmp5.f3;{struct Cyc_Absyn_Tqual tq2=_Tmp9;void*t2=_Tmp8;struct Cyc_List_List*tvs=_Tmp7;struct Cyc_List_List*atts2=_Tmp6;
if(tvs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=
_tag_fat("bad type parameters on formal argument, ignoring",sizeof(char),49U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({unsigned _TmpC=
# 2382
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Warn_warn2(_TmpC,_tag_fat(_TmpB,sizeof(void*),1));});});
# 2384
if(atts2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({unsigned _TmpC=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Warn_warn2(_TmpC,_tag_fat(_TmpB,sizeof(void*),1));});});
yyval=Cyc_YY38(({struct _tuple8*_TmpA=_cycalloc(sizeof(struct _tuple8));_TmpA->f0=0,_TmpA->f1=tq2,_TmpA->f2=t2;_TmpA;}));
# 2388
goto _LL0;}}}}case 287:
# 2391 "parse.y"
 yyval=Cyc_YY37(Cyc_List_imp_rev(Cyc_yyget_YY37(& yyyvsp[0].v)));
goto _LL0;case 288:
# 2394
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 289:
# 2395 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY37(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 290:
# 2399 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 291:
# 2400 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 292:
# 2405 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 293:
# 2407 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 294:
# 2409 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 295:  {
# 2411 "parse.y"
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});void*_Tmp3=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,_Tmp3,
Cyc_Absyn_uint_exp(0U,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line)),0);});
# 2414
vd->tq.real_const=1;
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_Tmp2->tag=26,_Tmp2->f1=vd,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 2417
goto _LL0;}case 296:  {
# 2419 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[6].l.first_line));});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_Tmp2->tag=27,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=t,_Tmp2->f3=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 2422
goto _LL0;}case 297:
# 2427 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f0=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 298:
# 2429 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp3->f0=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 299:
# 2431 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f0=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 300:
# 2433 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[2].v);_Tmp3->f0=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 301:
# 2437 "parse.y"
 yyval=Cyc_YY42(Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[0].v)));
goto _LL0;case 302:
# 2438 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp3->tag=1,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 303:
# 2443 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 304:
# 2444 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 305:
# 2448 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 306:
# 2449 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp1->tag=1,({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 307:  {
# 2454 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=({struct Cyc_Parse_Type_specifier _Tmp5=tss;Cyc_Parse_speclist2typ(_Tmp5,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});
if(atts!=0)
({unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp6=_tag_fat("ignoring attributes in type",sizeof(char),28U);Cyc_Warn_warn(_Tmp5,_Tmp6,_tag_fat(0U,sizeof(void*),0));});
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=tq,_Tmp5->f2=t;_Tmp5;}));
# 2460
goto _LL0;}}case 308:  {
# 2461 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=({struct Cyc_Parse_Type_specifier _Tmp5=tss;Cyc_Parse_speclist2typ(_Tmp5,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});
struct Cyc_List_List*tms=Cyc_yyget_YY31(& yyyvsp[1].v).tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);
if(t_info.f2!=0)
({unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);struct _fat_ptr _Tmp6=_tag_fat("bad type params, ignoring",sizeof(char),26U);Cyc_Warn_warn(_Tmp5,_Tmp6,_tag_fat(0U,sizeof(void*),0));});
if(t_info.f3!=0)
({unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);struct _fat_ptr _Tmp6=_tag_fat("bad specifiers, ignoring",sizeof(char),25U);Cyc_Warn_warn(_Tmp5,_Tmp6,_tag_fat(0U,sizeof(void*),0));});
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=t_info.f0,_Tmp5->f2=t_info.f1;_Tmp5;}));
# 2471
goto _LL0;}}case 309:
# 2474 "parse.y"
 yyval=Cyc_YY45(({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 310:
# 2475 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(0));
goto _LL0;case 311:
# 2476 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 312:
# 2477 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v)));
goto _LL0;case 313:
# 2478 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;})));
goto _LL0;case 314:
# 2479 "parse.y"
 yyval=Cyc_YY45(Cyc_yyget_YY45(& yyyvsp[0].v));
goto _LL0;case 315:
# 2485 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 316:
# 2486 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 317:
# 2490 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY27(& yyyvsp[0].v);_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 318:
# 2491 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 319:
# 2493 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,Cyc_yyget_YY31(& yyyvsp[1].v).tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 320:
# 2498 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 321:
# 2500 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 322:
# 2502 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[3].l.first_line);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 323:
# 2504 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f2=_Tmp6;}),({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[3].l.first_line);_Tmp5->f3=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 324:
# 2506 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[4].l.first_line);_Tmp5->f3=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2506
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2509
goto _LL0;case 325:  {
# 2510 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY62(& yyyvsp[3].v);void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;_TmpC=_Tmp8.f0;_TmpB=_Tmp8.f1;_TmpA=_Tmp8.f2;_Tmp9=_Tmp8.f3;{struct Cyc_Absyn_Exp*chk=_TmpC;struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _TmpD;({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp10=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp11=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp11->tag=3,({void*_Tmp12=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp13=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp13->tag=1,_Tmp13->f1=lis,_Tmp13->f2=b,_Tmp13->f3=c,_Tmp13->f4=eff,_Tmp13->f5=ec,_Tmp13->f6=qb,_Tmp13->f7=chk,_Tmp13->f8=req,_Tmp13->f9=ens,_Tmp13->f10=thrws;_Tmp13;});_Tmp11->f1=_Tmp12;});_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=0;_TmpF;});_TmpD.tms=_TmpE;});_TmpD;}));
# 2514
goto _LL0;}}}case 326:  {
# 2515 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY62(& yyyvsp[4].v);void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;_TmpC=_Tmp8.f0;_TmpB=_Tmp8.f1;_TmpA=_Tmp8.f2;_Tmp9=_Tmp8.f3;{struct Cyc_Absyn_Exp*chk=_TmpC;struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _TmpD;({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp10=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp11=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp11->tag=3,({void*_Tmp12=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp13=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp13->tag=1,_Tmp13->f1=lis,_Tmp13->f2=b,_Tmp13->f3=c,_Tmp13->f4=eff,_Tmp13->f5=ec,_Tmp13->f6=qb,_Tmp13->f7=chk,_Tmp13->f8=req,_Tmp13->f9=ens,_Tmp13->f10=thrws;_Tmp13;});_Tmp11->f1=_Tmp12;});_Tmp11;});_TmpF->hd=_Tmp10;}),({
struct Cyc_List_List*_Tmp10=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_TmpF->tl=_Tmp10;});_TmpF;});
# 2517
_TmpD.tms=_TmpE;});_TmpD;}));
# 2520
goto _LL0;}}}case 327:  {
# 2522
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2523
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2526
goto _LL0;}case 328:
# 2527 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,({unsigned _Tmp6=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2529
goto _LL0;case 329:
# 2533 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1;({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0,_Tmp1.f2=0,_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 330:
# 2535 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1;_Tmp1.f0=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0,_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 331:
# 2537 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1;_Tmp1.f0=0,_Tmp1.f1=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f2=_Tmp2;}),_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 332:
# 2539 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1;_Tmp1.f0=0,_Tmp1.f1=0,_Tmp1.f2=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 333:
# 2541 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f0=_Tmp3;}),_Tmp2.f1=0,_Tmp2.f2=0,_Tmp2.f3=0;_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY62(& yyyvsp[4].v));}));
goto _LL0;case 334:
# 2543 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;_Tmp2.f0=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f1=_Tmp3;}),_Tmp2.f2=0,_Tmp2.f3=0;_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY62(& yyyvsp[4].v));}));
goto _LL0;case 335:
# 2545 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f2=_Tmp3;}),_Tmp2.f3=0;_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY62(& yyyvsp[4].v));}));
goto _LL0;case 336:
# 2547 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0,_Tmp2.f2=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f3=_Tmp3;});_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY62(& yyyvsp[4].v));}));
goto _LL0;case 337:
# 2551 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1;_Tmp1.f0=0,_Tmp1.f1=0,_Tmp1.f2=0,_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 338:
# 2552 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 339:
# 2557 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 340:
# 2558 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 341:
# 2559 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 342:
# 2560 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 343:
# 2561 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 344:
# 2562 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 345:
# 2568 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 346:
# 2572 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 347:
# 2573 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_exp_stmt(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 348:
# 2578 "parse.y"
 Cyc_Parse_inside_function_definition=1;
goto _LL0;case 349:
# 2582 "parse.y"
 Cyc_Parse_inside_function_definition=0;
goto _LL0;case 350:
# 2586 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 351:
# 2587 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 352:
# 2592 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 353:
# 2593 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 354:
# 2598 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_Absyn_skip_stmt(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));}));
goto _LL0;case 355:
# 2599 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 356:
# 2601 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_Absyn_skip_stmt(0U));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 357:
# 2603 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_yyget_Stmt_tok(& yyyvsp[3].v));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 358:
# 2604 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 359:
# 2605 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Absyn_seq_stmt(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 360:
# 2606 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});Cyc_Parse_flatten_decl(_Tmp1,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 361:
# 2609 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});Cyc_Parse_flatten_decl(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 362:
# 2614 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 363:
# 2616 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[6].v);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 364:
# 2622 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 365:
# 2625
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_unknownid_exp(_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line));});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 366:
# 2628
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[3].v);Cyc_Absyn_tuple_exp(_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line));});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 367:
# 2632 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);Cyc_Absyn_trycatch_stmt(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 368:
# 2646 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 369:
# 2649 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp3->body=_Tmp4;}),({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});
# 2649
_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 370:
# 2652 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line));_Tmp3->body=_Tmp4;}),({
unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});
# 2652
_Tmp1->hd=_Tmp2;}),({
# 2654
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 371:
# 2656 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);_Tmp3->body=_Tmp4;}),({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 372:
# 2658 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_loc_to_seg(yyyvsp[4].l.first_line));_Tmp3->body=_Tmp4;}),({
unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});
# 2658
_Tmp1->hd=_Tmp2;}),({
# 2660
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 373:
# 2662 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[5].v);_Tmp3->body=_Tmp4;}),({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);_Tmp3->loc=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 374:
# 2669 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);Cyc_Absyn_while_stmt(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 375:
# 2671 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_do_stmt(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 376:
# 2673 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[8].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 377:  {
# 2675 "parse.y"
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[5].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[7].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp1,s);}));
goto _LL0;}case 378:
# 2679
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 379:
# 2680 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 380:
# 2683
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp2=_Tmp3;});_Tmp2;});Cyc_Absyn_goto_stmt(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 381:
# 2684 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 382:
# 2685 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 383:
# 2686 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 384:
# 2687 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_return_stmt(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 385:
# 2689 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 386:
# 2690 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 387:
# 2692 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_fallthru_stmt(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 388:
# 2701 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 389:
# 2704
 yyval=yyyvsp[0].v;
goto _LL0;case 390:
# 2706 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 391:
# 2709
 yyval=yyyvsp[0].v;
goto _LL0;case 392:
# 2711 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 393:
# 2714
 yyval=yyyvsp[0].v;
goto _LL0;case 394:
# 2716 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 395:
# 2719
 yyval=yyyvsp[0].v;
goto _LL0;case 396:
# 2721 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 397:
# 2724
 yyval=yyyvsp[0].v;
goto _LL0;case 398:
# 2726 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 399:
# 2729
 yyval=yyyvsp[0].v;
goto _LL0;case 400:
# 2731 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 401:
# 2734
 yyval=yyyvsp[0].v;
goto _LL0;case 402:
# 2736 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY69(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 403:
# 2739
 yyval=yyyvsp[0].v;
goto _LL0;case 404:
# 2741 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY69(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 405:
# 2744
 yyval=yyyvsp[0].v;
goto _LL0;case 406:
# 2746 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 407:
# 2748 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 408:
# 2751
 yyval=yyyvsp[0].v;
goto _LL0;case 409:
# 2753 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 410:
# 2756
 yyval=yyyvsp[0].v;
goto _LL0;case 411:
# 2758 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 412:
# 2761
 yyval=yyyvsp[0].v;
goto _LL0;case 413:  {
# 2763 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line));});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
# 2766
goto _LL0;}case 414:
# 2769 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 415:
# 2773 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 416:  {
# 2775 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line));});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=t;Cyc_Absyn_sizeoftype_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;}case 417:
# 2778 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 418:
# 2780 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=(*Cyc_yyget_YY38(& yyyvsp[2].v)).f2;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));})));
goto _LL0;case 419:
# 2785 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 420:
# 2793 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 421:
# 2797 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 422:
# 2798 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 423:
# 2799 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_Tmp2->tag=14,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 424:
# 2800 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=5,({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 425:  {
# 2802 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(& yyyvsp[0].v);
{void*_Tmp1=e->r;struct _fat_ptr _Tmp2;int _Tmp3;short _Tmp4;char _Tmp5;enum Cyc_Absyn_Sign _Tmp6;if(*((int*)_Tmp1)==0)switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.LongLong_c.tag){case 2: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f0;_Tmp5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;char i=_Tmp5;
# 2805
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_Tmp8->tag=10,_Tmp8->f1=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL537;}case 4: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f0;_Tmp4=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;short i=_Tmp4;
# 2807
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=9,_Tmp8->f1=s,_Tmp8->f2=(int)i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL537;}case 5: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f0;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;int i=_Tmp3;
# 2809
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=9,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL537;}case 7: _Tmp2=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f0;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f1;{struct _fat_ptr s=_Tmp2;int i=_Tmp3;
# 2811
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_Tmp8->tag=11,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL537;}case 1:
# 2813
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL537;case 8:
 goto _LL545;case 9: _LL545:
# 2816
({unsigned _Tmp7=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp8=_tag_fat("strings cannot occur within patterns",sizeof(char),37U);Cyc_Warn_err(_Tmp7,_Tmp8,_tag_fat(0U,sizeof(void*),0));});goto _LL537;case 6:
# 2818
({unsigned _Tmp7=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp8=_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U);Cyc_Warn_err(_Tmp7,_Tmp8,_tag_fat(0U,sizeof(void*),0));});goto _LL537;default: goto _LL548;}else{_LL548:
# 2820
({unsigned _Tmp7=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _fat_ptr _Tmp8=_tag_fat("bad constant in case",sizeof(char),21U);Cyc_Warn_err(_Tmp7,_Tmp8,_tag_fat(0U,sizeof(void*),0));});}_LL537:;}
# 2823
goto _LL0;}case 426:
# 2824 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);struct _fat_ptr _Tmp2=_tag_fat("expecting `as'",sizeof(char),15U);Cyc_Warn_err(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0,0);});_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2826
Cyc_Absyn_new_pat(_Tmp1,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 2829
goto _LL0;case 427:
# 2830 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);struct _fat_ptr _Tmp2=_tag_fat("expecting `alias'",sizeof(char),18U);Cyc_Warn_err(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});{
unsigned location=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_ek;_Tmp2;});tv->kind=_Tmp1;}),tv->aquals_bound=0;{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[4].l.first_line));}),0,0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2839
goto _LL0;}}}case 428:
# 2840 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);struct _fat_ptr _Tmp2=_tag_fat("expecting `alias'",sizeof(char),18U);Cyc_Warn_err(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});{
unsigned location=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_ek;_Tmp2;});tv->kind=_Tmp1;}),tv->aquals_bound=0;{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[4].l.first_line));}),0,0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2849
goto _LL0;}}}case 429:  {
# 2850 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=0,_Tmp5->f2=1,_Tmp5->f3=0,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 2853
goto _LL0;}}case 430:  {
# 2854 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY10(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_Tmp5->tag=15,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=ps,_Tmp5->f3=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 2857
goto _LL0;}}case 431:  {
# 2858 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[3].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp5;});unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);_Tmp4(Cyc_Parse_typ2tvar,_Tmp5,Cyc_yyget_YY41(& yyyvsp[2].v));});
union Cyc_Absyn_AggrInfo ai=Cyc_Absyn_UnknownAggr(0U,Cyc_yyget_QualId_tok(& yyyvsp[0].v),0);
struct Cyc_Absyn_AppType_Absyn_Type_struct*typ;typ=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct)),typ->tag=0,({void*_Tmp4=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_Tmp5->tag=24,_Tmp5->f1=ai;_Tmp5;});typ->f1=_Tmp4;}),typ->f2=0;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=(void*)typ,_Tmp5->f2=0,_Tmp5->f3=exist_ts,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 2864
goto _LL0;}}case 432:  {
# 2865 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp5;});unsigned _Tmp5=Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line);_Tmp4(Cyc_Parse_typ2tvar,_Tmp5,Cyc_yyget_YY41(& yyyvsp[1].v));});
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=0,_Tmp5->f2=0,_Tmp5->f3=exist_ts,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 2869
goto _LL0;}}case 433:
# 2870 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=5,({struct Cyc_Absyn_Pat*_Tmp3=({void*_Tmp4=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp5->tag=5,({struct Cyc_Absyn_Pat*_Tmp6=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;});_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));});_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 434:
# 2872 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0,0);});_Tmp2->f1=_Tmp3;}),({
# 2874
struct Cyc_Absyn_Pat*_Tmp3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line));_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2872
Cyc_Absyn_new_pat(_Tmp1,
# 2875
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 435:
# 2877 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);struct _fat_ptr _Tmp2=_tag_fat("expecting `as'",sizeof(char),15U);Cyc_Warn_err(_Tmp1,_Tmp2,_tag_fat(0U,sizeof(void*),0));});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0,0);});_Tmp2->f1=_Tmp3;}),({
# 2881
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[3].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2879
Cyc_Absyn_new_pat(_Tmp1,
# 2881
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 2883
goto _LL0;case 436:  {
# 2884 "parse.y"
void*tag=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);void*_Tmp2=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik);Cyc_Parse_id2type(_Tmp1,_Tmp2,0,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line));});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,({struct Cyc_Absyn_Tvar*_Tmp3=({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);Cyc_Parse_typ2tvar(_Tmp4,tag);});_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(tag),0,0);});
# 2886
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2885
Cyc_Absyn_new_pat(_Tmp1,
# 2888
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 437:  {
# 2890 "parse.y"
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,_Tmp2->f1=tv,({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0,0);});
# 2892
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2891
Cyc_Absyn_new_pat(_Tmp1,
# 2894
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 438:
# 2898 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 439:
# 2899 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 440:
# 2900 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f0=0,_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 441:
# 2903
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 442:
# 2904 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY11(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 443:
# 2907
 yyval=Cyc_YY12(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));_Tmp1->f0=0,({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 444:
# 2908 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 445:
# 2911
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 446:
# 2912 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 447:
# 2913 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f0=0,_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 448:
# 2916
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp2=Cyc_yyget_YY12(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 449:
# 2917 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp2=Cyc_yyget_YY12(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY13(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 450:
# 2922 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 451:
# 2923 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_seq_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 452:
# 2927 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 453:
# 2929 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Core_Opt*_Tmp2=Cyc_yyget_YY7(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assignop_exp(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 454:
# 2931 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_swap_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 455:
# 2935 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 456:
# 2936 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)1U;_Tmp1;}));
goto _LL0;case 457:
# 2937 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)3U;_Tmp1;}));
goto _LL0;case 458:
# 2938 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)4U;_Tmp1;}));
goto _LL0;case 459:
# 2939 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)0U;_Tmp1;}));
goto _LL0;case 460:
# 2940 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)2U;_Tmp1;}));
goto _LL0;case 461:
# 2941 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)16U;_Tmp1;}));
goto _LL0;case 462:
# 2942 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)17U;_Tmp1;}));
goto _LL0;case 463:
# 2943 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)13U;_Tmp1;}));
goto _LL0;case 464:
# 2944 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)15U;_Tmp1;}));
goto _LL0;case 465:
# 2945 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)14U;_Tmp1;}));
goto _LL0;case 466:
# 2949 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 467:
# 2951 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 468:
# 2953 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_throw_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 469:
# 2955 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,0,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 470:
# 2956 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,0,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 471:
# 2957 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 472:
# 2958 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 473:  {
# 2959 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp4=rgn;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp6=qual;Cyc_Absyn_New_exp(_Tmp4,_Tmp5,_Tmp6,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}}case 474:  {
# 2960 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp4=rgn;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp6=qual;Cyc_Absyn_New_exp(_Tmp4,_Tmp5,_Tmp6,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}}case 475:
# 2972 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 476:
# 2975
 yyval=yyyvsp[0].v;
goto _LL0;case 477:
# 2977 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 478:
# 2980
 yyval=yyyvsp[0].v;
goto _LL0;case 479:
# 2982 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 480:
# 2985
 yyval=yyyvsp[0].v;
goto _LL0;case 481:
# 2987 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 482:
# 2990
 yyval=yyyvsp[0].v;
goto _LL0;case 483:
# 2992 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 484:
# 2995
 yyval=yyyvsp[0].v;
goto _LL0;case 485:
# 2997 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 486:
# 3000
 yyval=yyyvsp[0].v;
goto _LL0;case 487:
# 3002 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY69(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 488:
# 3005
 yyval=yyyvsp[0].v;
goto _LL0;case 489:
# 3007 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY69(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 490:
# 3010
 yyval=yyyvsp[0].v;
goto _LL0;case 491:
# 3012 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 492:
# 3014 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 493:
# 3017
 yyval=yyyvsp[0].v;
goto _LL0;case 494:
# 3019 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 495:
# 3022
 yyval=yyyvsp[0].v;
goto _LL0;case 496:
# 3024 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 497:
# 3027
 yyval=Cyc_YY69(Cyc_Absyn_eq_exp);
goto _LL0;case 498:
# 3028 "parse.y"
 yyval=Cyc_YY69(Cyc_Absyn_neq_exp);
goto _LL0;case 499:
# 3031
 yyval=Cyc_YY69(Cyc_Absyn_lt_exp);
goto _LL0;case 500:
# 3032 "parse.y"
 yyval=Cyc_YY69(Cyc_Absyn_gt_exp);
goto _LL0;case 501:
# 3033 "parse.y"
 yyval=Cyc_YY69(Cyc_Absyn_lte_exp);
goto _LL0;case 502:
# 3034 "parse.y"
 yyval=Cyc_YY69(Cyc_Absyn_gte_exp);
goto _LL0;case 503:
# 3037
 yyval=Cyc_YY6(0U);
goto _LL0;case 504:
# 3038 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 505:
# 3041
 yyval=Cyc_YY6(1U);
goto _LL0;case 506:
# 3042 "parse.y"
 yyval=Cyc_YY6(3U);
goto _LL0;case 507:
# 3043 "parse.y"
 yyval=Cyc_YY6(4U);
goto _LL0;case 508:
# 3047 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 509:  {
# 3049 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[1].l.first_line));});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 510:
# 3054 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 511:
# 3055 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,0U,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 512:
# 3056 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,2U,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 513:
# 3057 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_address_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 514:
# 3058 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_deref_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 515:
# 3059 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 516:  {
# 3061 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line));});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;Cyc_Absyn_sizeoftype_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 3064
goto _LL0;}case 517:
# 3064 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 518:  {
# 3066 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line));});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 3069
goto _LL0;}case 519:
# 3071
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,_Tmp2->f1.rgn=0,_Tmp2->f1.aqual=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 520:
# 3074 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.aqual=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 521:
# 3077 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.aqual=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 522:  {
# 3080 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;
yyval=Cyc_Exp_tok(({void*_Tmp4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp5->tag=33,_Tmp5->f1.mknd=2U,_Tmp5->f1.rgn=rgn,_Tmp5->f1.aqual=qual,_Tmp5->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp5->f1.num_elts=_Tmp6;}),_Tmp5->f1.fat_result=0,_Tmp5->f1.inline_call=0;_Tmp5;});Cyc_Absyn_new_exp(_Tmp4,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}}case 523:  {
# 3084 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;
yyval=Cyc_Exp_tok(({void*_Tmp4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp5->tag=33,_Tmp5->f1.mknd=0U,_Tmp5->f1.rgn=rgn,_Tmp5->f1.aqual=qual,_Tmp5->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp5->f1.num_elts=_Tmp6;}),_Tmp5->f1.fat_result=0,_Tmp5->f1.inline_call=1;_Tmp5;});Cyc_Absyn_new_exp(_Tmp4,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}}case 524:  {
# 3088 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[6].l.first_line));});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,_Tmp2->f1.rgn=0,_Tmp2->f1.aqual=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 525:  {
# 3092 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[10].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[10].l.first_line));});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.aqual=_Tmp3;}),({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 526:  {
# 3097
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[8].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[8].l.first_line));});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.aqual=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,
Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 527:
# 3101 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=({struct Cyc_Absyn_Exp*_Tmp2[1];({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2[0]=_Tmp3;});Cyc_List_list(_tag_fat(_Tmp2,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(18U,_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 528:
# 3103 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_tagof_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 529:
# 3105 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=37,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 530:
# 3107 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=37,({struct Cyc_Absyn_Exp*_Tmp3=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_deref_exp(_Tmp4,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line));});_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 531:  {
# 3109 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line));});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;Cyc_Absyn_valueof_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;}case 532:
# 3111 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=Cyc_yyget_YY63(& yyyvsp[1].v);Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 533:
# 3112 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_extension_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 534:
# 3113 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_exp(_Tmp1,0,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 535:
# 3114 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_exp(_Tmp1,1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 536:
# 3115 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_false_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 537:
# 3119 "parse.y"
 yyval=Cyc_YY6(12U);
goto _LL0;case 538:
# 3120 "parse.y"
 yyval=Cyc_YY6(11U);
goto _LL0;case 539:
# 3121 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 540:
# 3122 "parse.y"
 yyval=Cyc_YY6(0U);
goto _LL0;case 541:  {
# 3127 "parse.y"
struct _tuple31*_Tmp1=Cyc_yyget_YY64(& yyyvsp[3].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*outlist=_Tmp4;struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY63((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_Tmp5->tag=39,({int _Tmp6=Cyc_yyget_YY32(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=outlist,_Tmp5->f4=inlist,_Tmp5->f5=clobbers;_Tmp5;}));
goto _LL0;}}case 542:
# 3132 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 543:
# 3133 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 544:
# 3137 "parse.y"
 yyval=Cyc_YY64(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));_Tmp1->f0=0,_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 545:  {
# 3139 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY65(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY64(({struct _tuple31*_Tmp4=_cycalloc(sizeof(struct _tuple31));_Tmp4->f0=0,_Tmp4->f1=inlist,_Tmp4->f2=clobbers;_Tmp4;}));
goto _LL0;}}case 546:  {
# 3142 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY65(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY64(({struct _tuple31*_Tmp4=_cycalloc(sizeof(struct _tuple31));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY67(& yyyvsp[1].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=inlist,_Tmp4->f2=clobbers;_Tmp4;}));
goto _LL0;}}case 547:
# 3146
 yyval=Cyc_YY67(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY68(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 548:
# 3147 "parse.y"
 yyval=Cyc_YY67(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY68(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 549:
# 3151 "parse.y"
 yyval=Cyc_YY65(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f0=0,_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 550:
# 3152 "parse.y"
 yyval=Cyc_YY65(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f0=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 551:
# 3153 "parse.y"
 yyval=Cyc_YY65(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY67(& yyyvsp[1].v));_Tmp1->f0=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 552:
# 3156
 yyval=Cyc_YY67(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY68(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 553:
# 3157 "parse.y"
 yyval=Cyc_YY67(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY68(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 554:
# 3161 "parse.y"
 yyval=Cyc_YY68(({struct _tuple32*_Tmp1=_cycalloc(sizeof(struct _tuple32));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 555:
# 3166 "parse.y"
 yyval=Cyc_YY66(0);
goto _LL0;case 556:
# 3167 "parse.y"
 yyval=Cyc_YY66(0);
goto _LL0;case 557:
# 3168 "parse.y"
 yyval=Cyc_YY66(Cyc_List_imp_rev(Cyc_yyget_YY66(& yyyvsp[1].v)));
goto _LL0;case 558:
# 3171
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 559:
# 3172 "parse.y"
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 560:
# 3177 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 561:
# 3179 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_subscript_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 562:
# 3181 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_unknowncall_exp(_Tmp1,0,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 563:
# 3183 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_unknowncall_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 564:
# 3185 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrmember_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 565:
# 3187 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrarrow_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 566:
# 3189 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,1U,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 567:
# 3191 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,3U,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 568:
# 3193 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 569:
# 3195 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 570:
# 3197 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 571:
# 3202 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 572:  {
# 3205
unsigned i=({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Parse_cnst2uint(_Tmp1,Cyc_yyget_Int_tok(& yyyvsp[0].v));});
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=Cyc_Absyn_tuple_field_designator((int)i);void*_Tmp2;_Tmp2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp1)->f1;{struct _fat_ptr*f=_Tmp2;
yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp5->tag=0,_Tmp5->f1=f;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;}));
goto _LL0;}}case 573:
# 3209 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 574:  {
# 3212
unsigned i=({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[2].l.first_line);Cyc_Parse_cnst2uint(_Tmp1,Cyc_yyget_Int_tok(& yyyvsp[2].v));});
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=Cyc_Absyn_tuple_field_designator((int)i);void*_Tmp2;_Tmp2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp1)->f1;{struct _fat_ptr*f=_Tmp2;
yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp5->tag=0,_Tmp5->f1=f;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp3->tl=_Tmp4;});_Tmp3;}));
goto _LL0;}}case 575:
# 3219 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_unknownid_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 576:
# 3220 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Absyn_pragma_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 577:
# 3221 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 578:
# 3222 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_string_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 579:
# 3223 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wstring_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 580:
# 3224 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 581:
# 3228 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_noinstantiate_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 582:
# 3231
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[3].v));Cyc_Absyn_instantiate_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 583:
# 3234
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp2->tag=28,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[3].v));_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 584:
# 3236 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_tuple_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 585:
# 3238 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Absyn_stmt_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 586:
# 3242 "parse.y"
 yyval=Cyc_YY4(Cyc_List_imp_rev(Cyc_yyget_YY4(& yyyvsp[0].v)));
goto _LL0;case 587:
# 3247 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 588:
# 3249 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 589:
# 3255 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _Tmp1=Cyc_yyget_Int_tok(& yyyvsp[0].v);Cyc_Absyn_const_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 590:
# 3256 "parse.y"
 yyval=Cyc_Exp_tok(({char _Tmp1=Cyc_yyget_Char_tok(& yyyvsp[0].v);Cyc_Absyn_char_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 591:
# 3257 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wchar_exp(_Tmp1,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
goto _LL0;case 592:
# 3259 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp(Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line)));
goto _LL0;case 593:  {
# 3261 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(& yyyvsp[0].v);
int l=(int)Cyc_strlen(f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c==102 ||(int)c==70)i=0;else{
if((int)c==108 ||(int)c==76)i=2;}}
# 3269
yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=f;int _Tmp2=i;Cyc_Absyn_float_exp(_Tmp1,_Tmp2,Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line));}));
# 3271
goto _LL0;}case 594:
# 3274 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f0=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 595:
# 3275 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 596:
# 3278
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f0=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 597:
# 3279 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 598:
# 3284 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 599:
# 3285 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 600:
# 3288
 yyval=yyyvsp[0].v;
goto _LL0;case 601:
# 3289 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 602:
# 3294 "parse.y"
 goto _LL0;case 603:
# 3294 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;case 604:
# 3298 "parse.y"
 yyval=Cyc_YY71(0);
goto _LL0;case 605:
# 3300 "parse.y"
 yyval=Cyc_YY71(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp2=({struct _tuple35*_Tmp3=_cycalloc(sizeof(struct _tuple35));({void*_Tmp4=({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[1].v);int _Tmp6=({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(& yyyvsp[3].v);Cyc_strcmp(_Tmp7,_tag_fat("true",sizeof(char),5U));})==0;Cyc_Parse_assign_cvar_pos(_Tmp5,_Tmp6,Cyc_Parse_typevar2cvar(Cyc_yyget_String_tok(& yyyvsp[0].v)));});_Tmp3->f0=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY72(& yyyvsp[5].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY71(& yyyvsp[7].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 606:
# 3304 "parse.y"
 yyval=Cyc_YY72(0);
goto _LL0;case 607:
# 3305 "parse.y"
 yyval=Cyc_YY72(Cyc_yyget_YY72(& yyyvsp[0].v));
goto _LL0;case 608:
# 3309 "parse.y"
 yyval=Cyc_YY72(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY73(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 609:
# 3310 "parse.y"
 yyval=Cyc_YY72(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY73(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY72(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 610:
# 3314 "parse.y"
 yyval=Cyc_YY45(Cyc_Parse_typevar2cvar(Cyc_yyget_String_tok(& yyyvsp[0].v)));
goto _LL0;case 611:
# 3315 "parse.y"
 yyval=Cyc_YY45(({unsigned _Tmp1=Cyc_Position_loc_to_seg(yyyvsp[0].l.first_line);Cyc_Parse_str2type(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 612:
# 3319 "parse.y"
 yyval=Cyc_YY73(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_BansheeIf_add_location(_Tmp1,Cyc_BansheeIf_check_constraint(Cyc_yyget_YY73(& yyyvsp[3].v)));}));
goto _LL0;case 613:
# 3320 "parse.y"
 yyval=Cyc_YY73(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_BansheeIf_add_location(_Tmp1,({enum Cyc_Parse_ConstraintOps _Tmp2=Cyc_yyget_YY70(& yyyvsp[3].v);void*_Tmp3=Cyc_yyget_YY45(& yyyvsp[5].v);Cyc_Parse_comparison_constraint(_Tmp2,_Tmp3,Cyc_yyget_YY45(& yyyvsp[7].v));}));}));
goto _LL0;case 614:
# 3321 "parse.y"
 yyval=Cyc_YY73(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_BansheeIf_add_location(_Tmp1,({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[3].v);Cyc_BansheeIf_cond_equality_constraint(_Tmp2,Cyc_yyget_YY45(& yyyvsp[5].v));}));}));
goto _LL0;case 615:
# 3322 "parse.y"
 yyval=Cyc_YY73(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_BansheeIf_add_location(_Tmp1,({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[3].v);Cyc_BansheeIf_equality_constraint(_Tmp2,Cyc_yyget_YY45(& yyyvsp[5].v));}));}));
goto _LL0;case 616:
# 3323 "parse.y"
 yyval=Cyc_YY73(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_BansheeIf_add_location(_Tmp1,({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[3].v);Cyc_BansheeIf_inclusion_constraint(_Tmp2,Cyc_yyget_YY45(& yyyvsp[5].v));}));}));
goto _LL0;case 617:
# 3324 "parse.y"
 yyval=Cyc_YY73(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_BansheeIf_add_location(_Tmp1,({void*_Tmp2=Cyc_yyget_YY73(& yyyvsp[3].v);Cyc_BansheeIf_implication_constraint(_Tmp2,Cyc_yyget_YY73(& yyyvsp[5].v));}));}));
goto _LL0;case 618:
# 3325 "parse.y"
 yyval=Cyc_YY73(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_BansheeIf_add_location(_Tmp1,({enum Cyc_Parse_ConstraintOps _Tmp2=Cyc_yyget_YY70(& yyyvsp[3].v);void*_Tmp3=Cyc_yyget_YY73(& yyyvsp[5].v);Cyc_Parse_composite_constraint(_Tmp2,_Tmp3,Cyc_yyget_YY73(& yyyvsp[7].v));}));}));
goto _LL0;case 619:
# 3328
 yyval=Cyc_YY70(0U);
goto _LL0;case 620:
# 3329 "parse.y"
 yyval=Cyc_YY70(1U);
goto _LL0;case 621:
# 3330 "parse.y"
 yyval=Cyc_YY70(2U);
goto _LL0;case 622:
# 3331 "parse.y"
 yyval=Cyc_YY70(3U);
goto _LL0;case 623:
# 3332 "parse.y"
 yyval=Cyc_YY70(4U);
goto _LL0;default:
# 3336
 goto _LL0;}_LL0:;
# 375 "cycbison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 389 "cycbison.simple"
(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))).v=yyval;
# 392
if(yylen==0){
struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr_check_bound(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset - 1),sizeof(struct Cyc_Yystacktype),2U));
p[1].l.first_line=yylloc.first_line;
p[1].l.first_column=yylloc.first_column;
p[1].l.last_line=p[0].l.last_line;
p[1].l.last_column=p[0].l.last_column;}else{
# 399
((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset].l.last_line=(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l.last_line;
((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset].l.last_column=((struct Cyc_Yystacktype*)yyvs.curr)[(yyvsp_offset + yylen)- 1].l.last_column;}
# 409
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr1,624U,sizeof(short),yyn));
# 411
yystate=({int _Tmp1=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,188U,sizeof(short),yyn - 175));_Tmp1 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 8241)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8242U,sizeof(short),yystate))==(int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,188U,sizeof(short),yyn - 175));}
# 417
goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus==0){
# 424
++ yynerrs;
# 427
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1262U,sizeof(short),yystate));
# 429
if(yyn > -32768 && yyn < 8241){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 363U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8242U,sizeof(short),x + yyn))==x)
({unsigned long _Tmp1=Cyc_strlen(*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,363U,sizeof(struct _fat_ptr),x)))+ 15U;sze +=_Tmp1;}),count ++;}
msg=({unsigned _Tmp1=(unsigned)(sze + 15)+ 1U;_tag_fat(({char*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(char)));});({{unsigned _Tmp3=(unsigned)(sze + 15);unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='\000';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;}),sizeof(char),_Tmp1);});
Cyc_strcpy(msg,_tag_fat("parse error",sizeof(char),12U));
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 363U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8242U,sizeof(short),x + yyn))==x){
# 451
Cyc_strcat(msg,count==0?
_tag_fat(", expecting `",sizeof(char),14U):
 _tag_fat(" or `",sizeof(char),6U));
Cyc_strcat(msg,*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,363U,sizeof(struct _fat_ptr),x)));
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1262U,sizeof(short),yystate));
if(yyn==-32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 8241)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8242U,sizeof(short),yyn))!=1)goto yyerrdefault;
# 528
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 531
if(yyn==-32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 535
if(yyn==0)goto yyerrpop;}
# 537
if(yyn==1261){
int _Tmp1=0;_npop_handler(0);return _Tmp1;}
# 546
({struct Cyc_Yystacktype _Tmp1=({struct Cyc_Yystacktype _Tmp2;_Tmp2.v=yylval,_Tmp2.l=yylloc;_Tmp2;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_Tmp1;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3336 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
void*_Tmp0;struct _fat_ptr _Tmp1;char _Tmp2;union Cyc_Absyn_Cnst _Tmp3;switch(v.Stmt_tok.tag){case 1: _Tmp3=v.Int_tok.val;{union Cyc_Absyn_Cnst c=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_cnst2string(c);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 2: _Tmp2=v.Char_tok.val;{char c=_Tmp2;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)c);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%c",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 3: _Tmp1=v.String_tok.val;{struct _fat_ptr s=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("\"%s\"",sizeof(char),5U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 4: _Tmp0=v.QualId_tok.val;{struct _tuple0*q=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_qvar2string(q);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 5: _Tmp0=v.Exp_tok.val;{struct Cyc_Absyn_Exp*e=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 6: _Tmp0=v.Stmt_tok.val;{struct Cyc_Absyn_Stmt*s=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_stmt2string(s);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}default:
 Cyc_fprintf(Cyc_stderr,_tag_fat("?",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 3348
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return _tag_fat("end-of-file",sizeof(char),12U);
if(token==387)
return Cyc_Lex_token_string;
if(token==396)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 400?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,401U,sizeof(short),token)): 363;
if((unsigned)z < 363U)
return*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,363U,sizeof(struct _fat_ptr),z));
return _tag_fat(0,0,0);}}
# 3362
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _Tmp0=_new_region(0U,"yyr");struct _RegionHandle*yyr=& _Tmp0;_push_region(yyr);
({struct _RegionHandle*_Tmp1=yyr;Cyc_yyparse(_Tmp1,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_Tmp1=Cyc_Parse_parse_result;_npop_handler(0);return _Tmp1;}
# 3365
;_pop_region();}}
# 3369
struct Cyc_List_List*Cyc_Parse_parse_constraint_file(struct Cyc___cycFILE*f){
Cyc_Parse_constraint_graph=0;{
struct _RegionHandle _Tmp0=_new_region(0U,"yyr");struct _RegionHandle*yyr=& _Tmp0;_push_region(yyr);
({struct _RegionHandle*_Tmp1=yyr;Cyc_yyparse(_Tmp1,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_Tmp1=Cyc_Parse_constraint_graph;_npop_handler(0);return _Tmp1;}
# 3372
;_pop_region();}}

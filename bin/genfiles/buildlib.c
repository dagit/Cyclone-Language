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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 321 "core.h"
void Cyc_Core_rethrow(void*);
# 38 "cycboot.h"
extern int Cyc_open(const char*,int,struct _fat_ptr);struct Cyc___cycFILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 104
extern int Cyc_fputc(int,struct Cyc___cycFILE*);
# 106
extern int Cyc_fputs(const char*,struct Cyc___cycFILE*);
# 224 "cycboot.h"
extern int Cyc_vfprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 300 "cycboot.h"
extern int isspace(int);
# 310
extern int toupper(int);
# 318
extern int system(const char*);
extern void exit(int);
# 331
extern int mkdir(const char*,unsigned short);
# 334
extern int close(int);
extern int chdir(const char*);
extern struct _fat_ptr Cyc_getcwd(struct _fat_ptr,unsigned long);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 78 "lexing.h"
extern struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 82
extern struct _fat_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
extern char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
extern int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple0{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 65
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 77
extern struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 84
extern struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 87
extern struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*,void*);
# 96
extern int Cyc_Set_cardinality(struct Cyc_Set_Set*);
# 99
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*);
# 102
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 139
extern void*Cyc_Set_choose(struct Cyc_Set_Set*);
# 143
extern struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*,struct Cyc_Set_Set*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
extern int Cyc_strncmp(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 105 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 110
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 86
extern int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*);
# 30 "filename.h"
extern struct _fat_ptr Cyc_Filename_concat(struct _fat_ptr,struct _fat_ptr);
# 34
extern struct _fat_ptr Cyc_Filename_chop_extension(struct _fat_ptr);
# 40
extern struct _fat_ptr Cyc_Filename_dirname(struct _fat_ptr);
# 43
extern struct _fat_ptr Cyc_Filename_basename(struct _fat_ptr);struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
# 66 "arg.h"
extern void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 69
extern int Cyc_Arg_current;
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);struct Cyc_Buffer_t;
# 50 "buffer.h"
extern struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned);
# 58
extern struct _fat_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 81
extern void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 92 "buffer.h"
extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _fat_ptr);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 526 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 533
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 551
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Subset_coercion =3U,Cyc_Absyn_Other_coercion =4U};
# 566
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 1166 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1224
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);struct _tuple12{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;};
# 1230
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1238
struct _tuple1*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 27 "warn.h"
extern void Cyc_Warn_reset(struct _fat_ptr);
extern int Cyc_Warn_print_warnings;
# 25 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 68 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 29 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*,int,struct Cyc_List_List*);
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);
# 30 "specsfile.h"
extern void Cyc_Specsfile_set_target_arch(struct _fat_ptr);
# 32
extern void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr);
# 34
extern struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr);
# 36
extern struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*,struct _fat_ptr);
# 39
extern struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 44
extern struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr);
# 78 "buildlib.cyl"
extern void Cyc_Lex_lex_init(int);static char _TmpG0[4U]="gcc";
# 88
static struct _fat_ptr Cyc_cyclone_cc={(unsigned char*)_TmpG0,(unsigned char*)_TmpG0,(unsigned char*)_TmpG0 + 4U};static char _TmpG1[1U]="";
static struct _fat_ptr Cyc_target_cflags={(unsigned char*)_TmpG1,(unsigned char*)_TmpG1,(unsigned char*)_TmpG1 + 1U};
# 91
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
# 94
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 98
int Cyc_log(struct _fat_ptr fmt,struct _fat_ptr ap){struct Cyc___cycFILE*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;struct Cyc___cycFILE*_T3;int _T4;
# 101
if(Cyc_log_file!=0)goto _TL0;_T0=Cyc_stderr;_T1=
_tag_fat("Internal error: log file is NULL\n",sizeof(char),34U);_T2=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T0,_T1,_T2);
exit(1);goto _TL1;_TL0: _TL1: {
# 105
int x=Cyc_vfprintf(Cyc_log_file,fmt,ap);_T3=
_check_null(Cyc_log_file);Cyc_fflush(_T3);_T4=x;
return _T4;}}
# 110
static struct _fat_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _fat_ptr*sptr){struct Cyc_Set_Set**_T0;struct Cyc_Set_Set*(*_T1)(struct Cyc_Set_Set*,struct _fat_ptr*);struct Cyc_Set_Set*(*_T2)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_T3;struct Cyc_Set_Set*_T4;struct _fat_ptr*_T5;{struct Cyc_Set_Set**_T6=_cycalloc(sizeof(struct Cyc_Set_Set*));_T2=Cyc_Set_insert;{
struct Cyc_Set_Set*(*_T7)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T2;_T1=_T7;}_T3=_check_null(Cyc_current_targets);_T4=*_T3;_T5=sptr;*_T6=_T1(_T4,_T5);_T0=(struct Cyc_Set_Set**)_T6;}Cyc_current_targets=_T0;}
# 116
static struct Cyc_Set_Set**Cyc_headers_to_do=0;struct _tuple13{struct _fat_ptr*f0;struct Cyc_Set_Set*f1;};
# 120
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 126
struct Cyc___cycFILE*Cyc_slurp_out=0;
# 128
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
# 130
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple14{struct _fat_ptr f0;struct _fat_ptr*f1;};
# 134
struct _tuple14*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _fat_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple15{struct _fat_ptr f0;struct _fat_ptr f1;};struct _tuple16{struct _fat_ptr*f0;struct _fat_ptr*f1;};struct _tuple17{struct _fat_ptr f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;};
# 150
struct _tuple17*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _fat_ptr Cyc_current_headerfile={(void*)0,(void*)0,(void*)(0 + 0)};
struct Cyc_List_List*Cyc_snarfed_symbols=0;
struct Cyc_List_List*Cyc_current_symbols=0;
struct Cyc_List_List*Cyc_current_user_defs=0;
struct Cyc_List_List*Cyc_current_cstubs=0;
struct Cyc_List_List*Cyc_current_cycstubs=0;
struct Cyc_List_List*Cyc_current_hstubs=0;
struct Cyc_List_List*Cyc_current_omit_symbols=0;
struct Cyc_List_List*Cyc_current_cpp=0;
struct Cyc_Buffer_t*Cyc_specbuf=0;
struct _fat_ptr Cyc_current_symbol={(void*)0,(void*)0,(void*)(0 + 0)};
int Cyc_rename_current_symbol=0;
int Cyc_braces_to_match=0;
int Cyc_parens_to_match=0;
# 171
int Cyc_numdef=0;
# 173
static struct Cyc_List_List*Cyc_cppargs=0;static char _TmpG2[14U]="BUILDLIB_sym_";
# 175
struct _fat_ptr Cyc_user_prefix={(unsigned char*)_TmpG2,(unsigned char*)_TmpG2,(unsigned char*)_TmpG2 + 14U};
static struct _fat_ptr*Cyc_add_user_prefix(struct _fat_ptr*symbol){struct _fat_ptr _T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct _fat_ptr*_T4;_T0=Cyc_user_prefix;_T1=symbol;_T2=*_T1;_T3=
Cyc_strconcat(_T0,_T2);{struct _fat_ptr s=_T3;{struct _fat_ptr*_T5=_cycalloc(sizeof(struct _fat_ptr));
*_T5=s;_T4=(struct _fat_ptr*)_T5;}return _T4;}}
# 180
static struct _fat_ptr Cyc_remove_user_prefix(struct _fat_ptr symbol){int _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct Cyc___cycFILE*_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;unsigned _T7;int _T8;unsigned long _T9;unsigned _TA;unsigned long _TB;struct _fat_ptr _TC;struct _fat_ptr _TD;
unsigned prefix_len=Cyc_strlen(Cyc_user_prefix);_T0=
Cyc_strncmp(symbol,Cyc_user_prefix,prefix_len);if(_T0==0)goto _TL2;{struct Cyc_String_pa_PrintArg_struct _TE;_TE.tag=0;
_TE.f1=symbol;_T1=_TE;}{struct Cyc_String_pa_PrintArg_struct _TE=_T1;void*_TF[1];_TF[0]=& _TE;_T2=Cyc_stderr;_T3=_tag_fat("Internal error: bad user type name %s\n",sizeof(char),39U);_T4=_tag_fat(_TF,sizeof(void*),1);Cyc_fprintf(_T2,_T3,_T4);}_T5=symbol;
return _T5;_TL2: _T6=symbol;_T7=prefix_len;_T8=(int)_T7;_T9=
# 186
Cyc_strlen(symbol);_TA=prefix_len;_TB=_T9 - _TA;_TC=Cyc_substring(_T6,_T8,_TB);_TD=_TC;return _TD;}
# 189
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Aggrdecl*_T3;struct _tuple1*_T4;struct _fat_ptr*_T5;struct Cyc_Absyn_Aggrdecl*_T6;struct _tuple1*_T7;struct _tuple1 _T8;struct _fat_ptr*_T9;struct _fat_ptr _TA;struct Cyc_Absyn_Enumdecl*_TB;struct _tuple1*_TC;struct _fat_ptr*_TD;struct Cyc_Absyn_Enumdecl*_TE;struct _tuple1*_TF;struct _tuple1 _T10;struct _fat_ptr*_T11;struct _fat_ptr _T12;struct Cyc_Absyn_Typedefdecl*_T13;struct _tuple1*_T14;struct _fat_ptr*_T15;struct Cyc_Absyn_Typedefdecl*_T16;struct _tuple1*_T17;struct _tuple1 _T18;struct _fat_ptr*_T19;struct _fat_ptr _T1A;struct Cyc___cycFILE*_T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;_T0=d;{
void*_T1E=_T0->r;struct Cyc_Absyn_Typedefdecl*_T1F;struct Cyc_Absyn_Enumdecl*_T20;struct Cyc_Absyn_Aggrdecl*_T21;_T1=(int*)_T1E;_T2=*_T1;switch(_T2){case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T22=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T1E;_T21=_T22->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T21;_T3=ad;_T4=_T3->name;{struct _fat_ptr*_T22=_cycalloc(sizeof(struct _fat_ptr));_T6=ad;_T7=_T6->name;_T8=*_T7;_T9=_T8.f1;_TA=*_T9;
# 192
*_T22=Cyc_remove_user_prefix(_TA);_T5=(struct _fat_ptr*)_T22;}(*_T4).f1=_T5;goto _LL0;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T22=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T1E;_T20=_T22->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T20;_TB=ed;_TC=_TB->name;{struct _fat_ptr*_T22=_cycalloc(sizeof(struct _fat_ptr));_TE=ed;_TF=_TE->name;_T10=*_TF;_T11=_T10.f1;_T12=*_T11;
# 194
*_T22=Cyc_remove_user_prefix(_T12);_TD=(struct _fat_ptr*)_T22;}(*_TC).f1=_TD;goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T22=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T1E;_T1F=_T22->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T1F;_T13=td;_T14=_T13->name;{struct _fat_ptr*_T22=_cycalloc(sizeof(struct _fat_ptr));_T16=td;_T17=_T16->name;_T18=*_T17;_T19=_T18.f1;_T1A=*_T19;
# 196
*_T22=Cyc_remove_user_prefix(_T1A);_T15=(struct _fat_ptr*)_T22;}(*_T14).f1=_T15;goto _LL0;}default: _T1B=Cyc_stderr;_T1C=
# 198
_tag_fat("Error in .cys file: bad user-defined type definition\n",sizeof(char),54U);_T1D=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T1B,_T1C,_T1D);
exit(1);}_LL0:;}}
# 205
const int Cyc_lex_base[495U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1250,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,386,112,104,105,123,126,397,112,134,190,399,1365,1480,414,205,213,233,219,224,224,245,525,1595,- 9,1710,654,- 10,233,253,684,1825,- 7,1940,720,722,- 11,432,370,379,2018,2095,2172,2253,434,465,381,2328,253,253,253,251,247,257,0,13,4,2409,5,462,2417,2482,475,- 4,- 3,49,467,6,2443,7,491,2505,2543,499,- 28,1162,1167,273,264,267,271,281,284,290,291,- 25,584,271,276,287,280,291,313,310,313,309,320,326,333,335,- 23,361,391,394,413,420,405,- 27,423,451,448,458,465,467,- 19,447,463,455,467,478,473,475,491,492,504,500,500,522,524,- 21,544,543,536,560,556,573,558,580,583,571,575,572,575,- 26,576,581,595,603,591,593,609,616,617,4,6,21,618,619,606,606,618,618,625,627,24,631,632,24,20,639,642,53,697,698,700,649,648,644,634,636,650,654,658,640,656,658,718,719,- 16,738,660,651,662,660,678,666,663,694,711,695,710,729,725,747,727,736,744,739,741,757,758,814,815,- 18,747,748,742,752,753,767,769,1,825,825,826,827,769,769,776,777,2,833,779,780,1078,814,815,816,817,780,778,820,826,827,882,884,912,- 15,870,871,926,927,928,875,883,938,939,940,- 12,887,888,943,944,945,892,893,948,949,950,- 13,880,889,935,938,941,923,940,968,984,985,1041,1042,- 17,1277,987,1015,1012,1011,1010,1006,1037,1041,1043,1041,1054,1370,1046,1047,1045,1058,1372,1396,1061,1063,1054,1055,1053,1066,1487,1058,1060,1058,1071,1597,- 7,- 8,8,1259,2575,9,1339,2599,2637,1563,1279,- 49,1253,- 2,1112,- 4,1113,1235,1481,1117,1234,1134,1676,1120,2664,2707,1124,1166,1132,1213,2777,1137,1170,- 36,- 42,- 37,2852,1168,- 40,1206,- 45,- 39,- 48,2927,2956,1695,1193,1204,1791,2966,2996,1810,1906,3029,3060,3098,1229,1319,3168,3206,1339,1349,1341,1353,1407,1418,- 6,- 34,1224,3138,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1240,3246,1244,1245,1925,1258,1269,1302,1303,1304,1305,1338,1345,1357,3319,3403,- 22,2472,1359,- 20,- 24,- 41,- 38,- 35,1490,3485,1367,3568,1359,15,1301,1302,1305,1303,1301,1322,1427};
const int Cyc_lex_backtrk[495U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,27,27,27,27,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,23,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,19,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,43,- 1,42,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[495U]={- 1,- 1,- 1,392,381,141,23,99,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,104,- 1,- 1,- 1,- 1,- 1,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,133,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,290,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,458,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3825U]={0,0,0,0,0,0,0,0,0,0,22,19,28,487,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,289,298,488,221,22,- 1,- 1,22,- 1,- 1,48,222,46,241,22,485,485,485,485,485,485,485,485,485,485,31,104,22,223,115,42,232,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,31,235,236,239,485,133,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,483,483,483,483,483,483,483,483,483,483,122,20,75,22,69,57,58,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,59,60,61,62,483,63,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,64,65,130,393,394,393,393,394,131,22,66,67,68,70,71,132,34,34,34,34,34,34,34,34,72,73,393,395,396,74,76,397,398,399,48,48,400,401,48,402,403,404,405,406,406,406,406,406,406,406,406,406,407,77,408,409,410,48,19,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,19,- 1,- 1,412,411,78,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,382,413,102,383,142,142,24,24,142,134,123,105,105,82,95,105,83,85,28,86,24,29,84,25,384,87,88,142,89,22,26,26,21,21,105,96,97,143,116,117,118,119,120,121,26,35,35,35,35,35,35,35,35,153,146,147,30,30,30,30,30,30,30,30,105,105,148,149,105,150,151,152,211,113,113,113,113,113,197,113,68,68,385,191,68,182,175,105,135,124,144,74,74,79,79,74,113,79,113,168,161,162,27,68,163,31,164,21,81,81,145,165,81,166,74,167,79,114,114,114,114,114,114,114,114,114,114,- 1,32,- 1,- 1,81,- 1,22,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,- 1,169,- 1,- 1,114,- 1,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,170,171,28,129,129,129,129,129,129,129,129,172,173,19,174,31,35,35,35,35,35,35,35,35,176,106,106,90,90,106,35,90,140,140,140,140,140,140,140,140,35,35,35,35,35,35,35,35,106,177,90,178,179,180,91,181,183,184,185,21,21,21,107,108,107,107,107,107,107,107,107,107,107,107,21,186,187,188,189,190,36,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,192,193,194,195,107,196,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,50,50,91,198,50,199,200,33,33,33,33,33,33,33,33,33,33,93,93,201,202,93,203,50,33,33,33,33,33,33,204,205,206,207,208,154,155,156,51,157,93,209,- 1,158,- 1,210,212,98,98,213,52,98,159,160,214,215,33,33,33,33,33,33,216,217,218,219,220,224,225,226,98,227,228,229,230,- 1,231,- 1,43,43,233,234,43,101,101,102,102,101,237,102,53,238,240,49,242,54,55,256,243,245,246,43,56,244,247,248,101,249,102,250,251,252,253,254,255,337,293,44,44,44,44,44,44,44,44,44,44,281,261,262,263,94,264,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,265,99,266,267,44,268,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,257,269,45,270,271,36,272,103,273,258,259,274,275,276,277,278,260,279,280,282,283,284,285,286,287,37,288,290,291,292,91,294,295,296,297,299,300,301,38,39,39,39,39,39,39,39,39,39,39,331,326,320,315,307,308,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,309,310,311,312,39,313,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,314,22,33,33,33,33,33,33,33,33,33,33,316,317,318,319,103,321,31,33,33,33,33,33,33,322,323,324,325,327,328,329,330,94,332,333,334,335,336,338,41,41,41,41,41,41,41,41,41,41,339,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,340,341,342,343,41,344,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,345,41,41,41,41,41,41,41,41,41,41,346,347,- 1,348,349,363,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,358,354,355,356,41,357,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,302,99,46,359,303,360,361,362,364,365,366,367,376,304,371,305,372,373,374,375,377,46,378,379,380,368,368,210,481,368,350,350,457,452,350,448,423,47,47,47,47,47,47,47,47,47,47,152,368,453,454,306,416,350,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,420,174,421,419,47,414,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,22,46,46,369,482,46,482,482,351,422,391,391,451,370,391,479,141,418,352,455,36,478,36,353,46,477,181,482,350,350,391,391,350,391,391,21,415,456,480,196,47,47,47,47,47,47,47,47,47,47,280,350,36,391,36,21,35,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,103,325,336,49,47,35,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,22,79,79,351,314,79,362,362,367,367,362,255,367,352,390,390,390,390,390,390,390,390,35,349,79,474,21,21,489,362,490,367,368,368,491,492,368,19,493,31,80,80,80,80,80,80,80,80,80,80,494,31,31,99,35,368,99,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,31,31,99,21,80,99,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,31,81,81,- 1,447,81,447,369,375,375,0,482,375,482,482,0,0,370,0,0,0,0,0,0,81,0,0,0,0,0,0,375,- 1,0,482,0,447,0,447,28,80,80,80,80,80,80,80,80,80,80,0,0,0,0,0,0,0,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,0,0,459,0,80,0,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,35,90,90,380,380,90,0,380,31,31,31,31,31,31,31,31,0,0,0,0,0,0,0,0,90,0,380,0,0,0,0,0,0,0,35,0,0,0,0,0,92,92,92,92,92,92,92,92,92,92,0,0,0,0,0,0,0,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,0,0,0,0,92,0,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,91,93,93,0,449,93,450,450,450,450,450,450,450,450,450,450,0,0,0,- 1,429,0,429,0,93,430,430,430,430,430,430,430,430,430,430,0,0,0,0,0,92,92,92,92,92,92,92,92,92,92,0,0,0,0,0,0,0,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,0,0,0,0,92,0,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,94,98,98,0,0,98,430,430,430,430,430,430,430,430,430,430,0,0,0,0,433,0,433,0,98,434,434,434,434,434,434,434,434,434,434,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,0,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,0,0,0,0,100,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,99,101,101,0,0,101,434,434,434,434,434,434,434,434,434,434,474,0,0,0,0,0,0,0,101,475,475,475,475,475,475,475,475,0,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,0,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,0,0,0,0,100,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,36,109,109,109,109,109,109,109,109,109,109,109,109,22,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,109,109,109,109,109,109,109,109,109,109,22,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,22,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,0,0,- 1,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,0,0,110,110,110,110,110,110,110,110,110,110,110,110,112,0,0,0,0,0,0,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,0,0,0,0,110,0,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,114,114,114,114,114,114,114,114,114,114,0,0,0,0,0,0,0,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,28,0,0,125,114,0,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,0,0,28,0,0,136,126,126,126,126,126,126,126,126,128,128,128,128,128,128,128,128,128,128,0,0,0,0,0,0,0,128,128,128,128,128,128,0,0,0,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,31,0,0,- 1,0,474,0,0,128,128,128,128,128,128,476,476,476,476,476,476,476,476,0,127,128,128,128,128,128,128,128,128,128,128,31,0,0,0,0,0,0,128,128,128,128,128,128,139,139,139,139,139,139,139,139,139,139,138,0,0,0,0,0,0,139,139,139,139,139,139,0,0,0,128,128,128,128,128,128,19,0,0,386,0,0,139,139,139,139,139,139,139,139,139,139,0,139,139,139,139,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,0,0,387,387,387,387,387,387,387,387,0,0,0,0,0,0,0,0,0,139,139,139,139,139,139,0,389,389,389,389,389,389,389,389,389,389,0,0,0,0,0,0,0,389,389,389,389,389,389,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,389,389,389,389,389,389,389,389,389,389,388,389,389,389,389,389,389,389,389,389,389,389,389,0,0,424,0,435,435,435,435,435,435,435,435,436,436,0,0,0,0,0,0,0,0,0,0,0,426,389,389,389,389,389,389,437,0,0,0,0,0,0,0,0,438,0,0,439,424,0,425,425,425,425,425,425,425,425,425,425,426,0,0,0,0,0,0,437,0,0,0,426,0,0,0,0,438,0,427,439,0,0,0,0,0,0,0,428,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,426,0,0,0,0,0,0,427,0,0,0,0,0,0,0,0,428,417,417,417,417,417,417,417,417,417,417,0,0,0,0,0,0,0,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,0,0,0,0,417,0,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,0,0,0,0,0,0,0,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,0,0,0,0,417,0,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,431,431,431,431,431,431,431,431,431,431,0,0,0,0,0,0,0,0,0,0,0,432,94,0,0,0,0,424,94,425,425,425,425,425,425,425,425,425,425,430,430,430,430,430,430,430,430,430,430,0,426,0,0,432,94,0,0,427,0,0,94,91,0,0,0,0,428,91,0,431,431,431,431,431,431,431,431,431,431,0,0,0,426,0,0,0,0,0,0,427,432,94,0,91,0,0,0,94,428,91,0,0,434,434,434,434,434,434,434,434,434,434,0,0,0,0,0,0,0,0,0,0,432,94,94,0,0,0,0,94,94,424,0,435,435,435,435,435,435,435,435,436,436,0,0,0,0,0,0,0,0,0,0,0,426,0,94,0,0,0,0,445,94,0,0,0,0,0,0,424,446,436,436,436,436,436,436,436,436,436,436,0,0,0,0,0,426,0,0,0,0,0,426,445,0,0,0,0,0,443,0,0,446,0,0,0,0,0,444,0,0,450,450,450,450,450,450,450,450,450,450,0,0,0,426,0,0,0,0,0,0,443,432,94,0,0,0,0,0,94,444,440,440,440,440,440,440,440,440,440,440,0,0,0,0,0,0,0,440,440,440,440,440,440,432,94,0,0,0,0,0,94,0,0,0,0,0,0,0,440,440,440,440,440,440,440,440,440,440,0,440,440,440,440,440,440,440,440,440,440,440,440,0,0,0,460,0,441,0,0,461,0,0,0,0,0,442,0,0,462,462,462,462,462,462,462,462,0,440,440,440,440,440,440,463,0,0,0,0,441,0,0,0,0,0,0,0,0,442,0,0,0,0,0,0,0,0,0,0,0,0,0,0,464,0,0,0,0,465,466,0,0,0,467,0,0,0,0,0,0,0,468,0,0,0,469,0,470,0,471,0,472,473,473,473,473,473,473,473,473,473,473,0,0,0,0,0,0,0,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,0,0,0,0,0,0,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,167,0,0,0,0,0,0,0,0,473,473,473,473,473,473,473,473,473,473,0,0,0,0,0,0,0,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,0,0,0,0,0,0,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,19,0,0,484,0,0,0,483,483,483,483,483,483,483,483,483,483,0,0,0,0,0,0,0,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,0,0,0,0,483,0,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,486,0,0,0,0,0,0,0,485,485,485,485,485,485,485,485,485,485,0,0,0,0,0,0,0,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,0,0,0,0,485,0,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3825U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,123,125,134,136,383,386,43,43,46,46,43,487,46,- 1,- 1,- 1,- 1,- 1,121,288,297,0,220,10,12,42,10,12,42,43,220,46,221,20,1,1,1,1,1,1,1,1,1,1,38,51,122,222,10,38,231,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,132,234,235,238,1,132,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,69,70,7,30,30,30,30,30,30,30,30,71,72,3,3,3,73,75,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,77,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,82,4,5,5,6,8,5,6,8,50,50,53,83,50,53,84,27,85,17,27,53,17,4,86,87,5,88,5,6,8,9,18,50,95,96,5,115,116,117,118,119,120,17,34,34,34,34,34,34,34,34,144,145,146,27,27,27,27,27,27,27,27,105,105,147,148,105,149,150,151,154,106,106,113,113,106,155,113,68,68,4,156,68,157,158,105,6,8,5,74,74,78,78,74,106,78,113,159,160,161,17,68,162,27,163,7,81,81,5,164,81,165,74,166,78,11,11,11,11,11,11,11,11,11,11,104,27,111,104,81,111,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,112,168,133,112,11,133,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,169,170,68,126,126,126,126,126,126,126,126,171,172,74,173,78,129,129,129,129,129,129,129,129,175,13,13,89,89,13,81,89,137,137,137,137,137,137,137,137,140,140,140,140,140,140,140,140,13,176,89,177,178,179,4,180,182,183,184,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,185,186,187,188,189,190,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,191,192,193,194,13,195,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,89,197,14,198,199,32,32,32,32,32,32,32,32,32,32,93,93,200,201,93,202,14,32,32,32,32,32,32,203,204,205,206,207,153,153,153,14,153,93,208,104,153,111,209,211,97,97,212,14,97,153,153,213,214,32,32,32,32,32,32,215,216,217,218,219,223,224,225,97,226,227,228,229,112,230,133,15,15,232,233,15,101,101,102,102,101,236,102,14,237,239,240,241,14,14,243,242,244,245,15,14,242,246,247,101,248,102,249,250,251,252,253,254,257,258,15,15,15,15,15,15,15,15,15,15,259,260,261,262,93,263,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,264,97,265,266,15,267,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,256,268,15,269,270,101,271,102,272,256,256,273,274,275,276,277,256,278,279,281,282,283,284,285,286,16,287,289,290,291,292,293,294,295,296,298,299,300,16,16,16,16,16,16,16,16,16,16,16,302,303,304,305,306,307,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,308,309,310,311,16,312,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,313,16,33,33,33,33,33,33,33,33,33,33,315,316,317,318,319,320,15,33,33,33,33,33,33,321,322,323,324,326,327,328,329,330,331,332,333,334,335,337,39,39,39,39,39,39,39,39,39,39,338,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,339,340,341,342,39,343,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,344,41,41,41,41,41,41,41,41,41,41,345,346,290,347,348,351,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,352,353,354,355,41,356,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,301,357,44,358,301,359,360,361,363,364,365,366,369,301,370,301,371,372,373,374,376,44,377,378,379,142,142,395,397,142,143,143,400,402,143,404,407,44,44,44,44,44,44,44,44,44,44,409,142,402,402,301,412,143,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,408,408,408,418,44,413,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,142,393,47,393,393,143,420,384,384,449,142,384,398,410,410,143,401,427,458,428,143,47,460,461,393,350,350,391,391,350,384,391,384,413,401,398,463,47,47,47,47,47,47,47,47,47,47,464,350,427,391,428,391,437,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,465,466,467,468,47,437,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,79,79,350,469,79,362,362,367,367,362,470,367,350,387,387,387,387,387,387,387,387,438,471,79,476,484,486,488,362,489,367,368,368,490,491,368,362,492,367,79,79,79,79,79,79,79,79,79,79,493,441,442,443,438,368,444,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,441,442,443,494,79,444,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,80,80,399,445,80,446,368,375,375,- 1,482,375,482,482,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,- 1,- 1,- 1,375,399,- 1,482,- 1,445,- 1,446,375,80,80,80,80,80,80,80,80,80,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,- 1,- 1,399,- 1,80,- 1,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,90,90,380,380,90,- 1,380,390,390,390,390,390,390,390,390,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,380,- 1,- 1,- 1,- 1,- 1,- 1,- 1,380,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,92,92,- 1,403,92,403,403,403,403,403,403,403,403,403,403,- 1,- 1,- 1,399,426,- 1,426,- 1,92,426,426,426,426,426,426,426,426,426,426,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,98,98,- 1,- 1,98,429,429,429,429,429,429,429,429,429,429,- 1,- 1,- 1,- 1,432,- 1,432,- 1,98,432,432,432,432,432,432,432,432,432,432,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,100,100,- 1,- 1,100,433,433,433,433,433,433,433,433,433,433,462,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,462,462,462,462,462,462,462,462,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,- 1,- 1,110,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,114,114,114,114,114,114,114,114,114,114,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,124,- 1,- 1,124,114,- 1,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,- 1,- 1,135,- 1,- 1,135,124,124,124,124,124,124,124,124,127,127,127,127,127,127,127,127,127,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,- 1,- 1,- 1,135,135,135,135,135,135,135,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,110,- 1,475,- 1,- 1,127,127,127,127,127,127,475,475,475,475,475,475,475,475,- 1,124,128,128,128,128,128,128,128,128,128,128,135,- 1,- 1,- 1,- 1,- 1,- 1,128,128,128,128,128,128,138,138,138,138,138,138,138,138,138,138,135,- 1,- 1,- 1,- 1,- 1,- 1,138,138,138,138,138,138,- 1,- 1,- 1,128,128,128,128,128,128,385,- 1,- 1,385,- 1,- 1,139,139,139,139,139,139,139,139,139,139,- 1,138,138,138,138,138,138,139,139,139,139,139,139,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,385,385,385,385,385,385,385,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,139,139,139,139,139,- 1,388,388,388,388,388,388,388,388,388,388,- 1,- 1,- 1,- 1,- 1,- 1,- 1,388,388,388,388,388,388,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,389,389,389,389,389,389,389,389,389,389,385,388,388,388,388,388,388,389,389,389,389,389,389,- 1,- 1,405,- 1,405,405,405,405,405,405,405,405,405,405,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,389,389,389,389,389,389,405,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,- 1,- 1,405,406,- 1,406,406,406,406,406,406,406,406,406,406,405,- 1,- 1,- 1,- 1,- 1,- 1,405,- 1,- 1,- 1,406,- 1,- 1,- 1,- 1,405,- 1,406,405,- 1,- 1,- 1,- 1,- 1,- 1,- 1,406,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,406,- 1,- 1,- 1,- 1,- 1,- 1,406,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,406,411,411,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,- 1,- 1,- 1,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,411,- 1,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,411,417,417,417,417,417,417,417,417,417,417,- 1,- 1,- 1,- 1,- 1,- 1,- 1,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,- 1,- 1,- 1,- 1,417,- 1,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,417,424,424,424,424,424,424,424,424,424,424,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,424,424,- 1,- 1,- 1,- 1,425,424,425,425,425,425,425,425,425,425,425,425,430,430,430,430,430,430,430,430,430,430,- 1,425,- 1,- 1,424,424,- 1,- 1,425,- 1,- 1,424,430,- 1,- 1,- 1,- 1,425,430,- 1,431,431,431,431,431,431,431,431,431,431,- 1,- 1,- 1,425,- 1,- 1,- 1,- 1,- 1,- 1,425,431,431,- 1,430,- 1,- 1,- 1,431,425,430,- 1,- 1,434,434,434,434,434,434,434,434,434,434,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,431,431,434,- 1,- 1,- 1,- 1,431,434,435,- 1,435,435,435,435,435,435,435,435,435,435,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,435,- 1,434,- 1,- 1,- 1,- 1,435,434,- 1,- 1,- 1,- 1,- 1,- 1,436,435,436,436,436,436,436,436,436,436,436,436,- 1,- 1,- 1,- 1,- 1,435,- 1,- 1,- 1,- 1,- 1,436,435,- 1,- 1,- 1,- 1,- 1,436,- 1,- 1,435,- 1,- 1,- 1,- 1,- 1,436,- 1,- 1,450,450,450,450,450,450,450,450,450,450,- 1,- 1,- 1,436,- 1,- 1,- 1,- 1,- 1,- 1,436,450,450,- 1,- 1,- 1,- 1,- 1,450,436,439,439,439,439,439,439,439,439,439,439,- 1,- 1,- 1,- 1,- 1,- 1,- 1,439,439,439,439,439,439,450,450,- 1,- 1,- 1,- 1,- 1,450,- 1,- 1,- 1,- 1,- 1,- 1,- 1,440,440,440,440,440,440,440,440,440,440,- 1,439,439,439,439,439,439,440,440,440,440,440,440,- 1,- 1,- 1,459,- 1,440,- 1,- 1,459,- 1,- 1,- 1,- 1,- 1,440,- 1,- 1,459,459,459,459,459,459,459,459,- 1,440,440,440,440,440,440,459,- 1,- 1,- 1,- 1,440,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,440,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,459,- 1,- 1,- 1,- 1,459,459,- 1,- 1,- 1,459,- 1,- 1,- 1,- 1,- 1,- 1,- 1,459,- 1,- 1,- 1,459,- 1,459,- 1,459,- 1,459,472,472,472,472,472,472,472,472,472,472,- 1,- 1,- 1,- 1,- 1,- 1,- 1,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,- 1,- 1,- 1,- 1,- 1,- 1,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,473,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,473,473,473,473,473,473,473,473,473,473,- 1,- 1,- 1,- 1,- 1,- 1,- 1,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,- 1,- 1,- 1,- 1,- 1,- 1,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,473,483,- 1,- 1,483,- 1,- 1,- 1,483,483,483,483,483,483,483,483,483,483,- 1,- 1,- 1,- 1,- 1,- 1,- 1,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,- 1,- 1,- 1,- 1,483,- 1,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,483,485,- 1,- 1,- 1,- 1,- 1,- 1,- 1,485,485,485,485,485,485,485,485,485,485,- 1,- 1,- 1,- 1,- 1,- 1,- 1,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,- 1,- 1,- 1,- 1,485,- 1,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,485,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){struct Cyc_Lexing_lexbuf*_T0;struct Cyc_Lexing_lexbuf*_T1;struct Cyc_Lexing_lexbuf*_T2;struct Cyc_Lexing_lexbuf*_T3;int _T4;const int*_T5;int _T6;const char*_T7;const int*_T8;int _T9;int _TA;const int*_TB;int _TC;struct Cyc_Lexing_lexbuf*_TD;struct Cyc_Lexing_lexbuf*_TE;struct Cyc_Lexing_lexbuf*_TF;struct Cyc_Lexing_lexbuf*_T10;int _T11;struct Cyc_Lexing_lexbuf*_T12;int _T13;struct Cyc_Lexing_lexbuf*_T14;int _T15;int _T16;int _T17;struct Cyc_Lexing_lexbuf*_T18;struct _fat_ptr _T19;struct Cyc_Lexing_lexbuf*_T1A;int _T1B;int _T1C;unsigned char*_T1D;char*_T1E;char _T1F;int _T20;int _T21;const int*_T22;int _T23;const char*_T24;const int*_T25;int _T26;int _T27;const int*_T28;int _T29;const int*_T2A;int _T2B;struct Cyc_Lexing_lexbuf*_T2C;struct Cyc_Lexing_lexbuf*_T2D;struct Cyc_Lexing_lexbuf*_T2E;int _T2F;int _T30;struct Cyc_Lexing_Error_exn_struct*_T31;void*_T32;struct Cyc_Lexing_lexbuf*_T33;int _T34;struct Cyc_Lexing_lexbuf*_T35;
# 212
int state;int base;int backtrk;
int c;
state=start_state;
# 216
if(state < 0)goto _TL5;_T0=lbuf;_T1=lbuf;_T2=lbuf;
_T1->lex_start_pos=_T2->lex_curr_pos;_T0->lex_last_pos=_T1->lex_start_pos;_T3=lbuf;
_T3->lex_last_action=- 1;goto _TL6;
# 220
_TL5: _T4=- state;state=_T4 - 1;_TL6:
# 222
 _TL7: if(1)goto _TL8;else{goto _TL9;}
_TL8: _T5=Cyc_lex_base;_T6=state;_T7=_check_known_subscript_notnull(_T5,495U,sizeof(int),_T6);_T8=(const int*)_T7;base=*_T8;
if(base >= 0)goto _TLA;_T9=- base;_TA=_T9 - 1;return _TA;_TLA: _TB=Cyc_lex_backtrk;_TC=state;
backtrk=_TB[_TC];
if(backtrk < 0)goto _TLC;_TD=lbuf;_TE=lbuf;
_TD->lex_last_pos=_TE->lex_curr_pos;_TF=lbuf;
_TF->lex_last_action=backtrk;goto _TLD;_TLC: _TLD: _T10=lbuf;_T11=_T10->lex_curr_pos;_T12=lbuf;_T13=_T12->lex_buffer_len;
# 230
if(_T11 < _T13)goto _TLE;_T14=lbuf;_T15=_T14->lex_eof_reached;
if(_T15)goto _TL10;else{goto _TL12;}
_TL12: _T16=- state;_T17=_T16 - 1;return _T17;
# 234
_TL10: c=256;goto _TLF;
# 236
_TLE: _T18=lbuf;_T19=_T18->lex_buffer;_T1A=lbuf;_T1B=_T1A->lex_curr_pos;_T1A->lex_curr_pos=_T1B + 1;_T1C=_T1B;_T1D=_check_fat_subscript(_T19,sizeof(char),_T1C);_T1E=(char*)_T1D;_T1F=*_T1E;c=(int)_T1F;_T20=c;_T21=- 1;
if(_T20!=_T21)goto _TL13;c=256;goto _TL14;
_TL13: if(c >= 0)goto _TL15;c=256 + c;goto _TL16;_TL15: _TL16: _TL14: _TLF: _T22=Cyc_lex_check;_T23=base + c;_T24=_check_known_subscript_notnull(_T22,3825U,sizeof(int),_T23);_T25=(const int*)_T24;_T26=*_T25;_T27=state;
# 240
if(_T26!=_T27)goto _TL17;_T28=Cyc_lex_trans;_T29=base + c;
state=_T28[_T29];goto _TL18;
# 243
_TL17: _T2A=Cyc_lex_default;_T2B=state;state=_T2A[_T2B];_TL18:
 if(state >= 0)goto _TL19;_T2C=lbuf;_T2D=lbuf;
_T2C->lex_curr_pos=_T2D->lex_last_pos;_T2E=lbuf;_T2F=_T2E->lex_last_action;_T30=- 1;
if(_T2F!=_T30)goto _TL1B;{struct Cyc_Lexing_Error_exn_struct*_T36=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T36->tag=Cyc_Lexing_Error;
_T36->f1=_tag_fat("empty token",sizeof(char),12U);_T31=(struct Cyc_Lexing_Error_exn_struct*)_T36;}_T32=(void*)_T31;_throw(_T32);goto _TL1C;
# 249
_TL1B: _T33=lbuf;_T34=_T33->lex_last_action;return _T34;_TL1C: goto _TL1A;
# 252
_TL19: if(c!=256)goto _TL1D;_T35=lbuf;_T35->lex_eof_reached=0;goto _TL1E;_TL1D: _TL1E: _TL1A: goto _TL7;_TL9:;}
# 256
struct _tuple13*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct Cyc_Set_Set**_T2;struct Cyc_Set_Set*(*_T3)(struct Cyc_Set_Set*,struct _fat_ptr*);struct Cyc_Set_Set*(*_T4)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_T5;struct Cyc_Set_Set*_T6;struct Cyc_List_List*_T7;void*_T8;struct _fat_ptr*_T9;struct Cyc_List_List*_TA;struct _tuple13*_TB;struct Cyc_Set_Set**_TC;struct _tuple13*_TD;struct Cyc_Lexing_lexbuf*_TE;void(*_TF)(struct Cyc_Lexing_lexbuf*);struct _tuple13*_T10;struct Cyc_Lexing_Error_exn_struct*_T11;void*_T12;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:
# 214 "buildlib.cyl"
 Cyc_macroname(lexbuf);
_TL23: if(Cyc_current_args!=0)goto _TL21;else{goto _TL22;}
_TL21:{struct Cyc_Set_Set**_T13=_cycalloc(sizeof(struct Cyc_Set_Set*));_T4=Cyc_Set_delete;{struct Cyc_Set_Set*(*_T14)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T4;_T3=_T14;}_T5=_check_null(Cyc_current_targets);_T6=*_T5;_T7=Cyc_current_args;_T8=_T7->hd;_T9=(struct _fat_ptr*)_T8;*_T13=_T3(_T6,_T9);_T2=(struct Cyc_Set_Set**)_T13;}Cyc_current_targets=_T2;_TA=
# 215
_check_null(Cyc_current_args);Cyc_current_args=_TA->tl;goto _TL23;_TL22:{struct _tuple13*_T13=_cycalloc(sizeof(struct _tuple13));
# 219
_T13->f0=_check_null(Cyc_current_source);_TC=_check_null(Cyc_current_targets);_T13->f1=*_TC;_TB=(struct _tuple13*)_T13;}return _TB;case 1: _TD=
# 222 "buildlib.cyl"
Cyc_line(lexbuf);return _TD;case 2:
# 224
 return 0;default: _TE=lexbuf;_TF=_TE->refill_buff;
_TF(lexbuf);_T10=
Cyc_line_rec(lexbuf,lexstate);return _T10;}{struct Cyc_Lexing_Error_exn_struct*_T13=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T13->tag=Cyc_Lexing_Error;
# 228
_T13->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T11=(struct Cyc_Lexing_Error_exn_struct*)_T13;}_T12=(void*)_T11;_throw(_T12);}
# 230
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){struct _tuple13*_T0;_T0=Cyc_line_rec(lexbuf,0);return _T0;}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;int _T4;int _T5;int _T6;int _T7;unsigned long _T8;struct _fat_ptr _T9;struct Cyc_Set_Set**_TA;struct Cyc_Set_Set*(*_TB)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Set_Set*(*_TC)(int(*)(void*,void*));int(*_TD)(struct _fat_ptr*,struct _fat_ptr*);struct _fat_ptr*_TE;struct _fat_ptr _TF;int _T10;int _T11;int _T12;int _T13;unsigned long _T14;struct _fat_ptr _T15;struct Cyc_Set_Set**_T16;struct Cyc_Set_Set*(*_T17)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Set_Set*(*_T18)(int(*)(void*,void*));int(*_T19)(struct _fat_ptr*,struct _fat_ptr*);struct _fat_ptr*_T1A;struct _fat_ptr _T1B;struct Cyc_Set_Set**_T1C;struct Cyc_Set_Set*(*_T1D)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Set_Set*(*_T1E)(int(*)(void*,void*));int(*_T1F)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Lexing_lexbuf*_T20;void(*_T21)(struct Cyc_Lexing_lexbuf*);int _T22;struct Cyc_Lexing_Error_exn_struct*_T23;void*_T24;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:{struct _fat_ptr*_T25=_cycalloc(sizeof(struct _fat_ptr));_T3=
# 228 "buildlib.cyl"
Cyc_Lexing_lexeme(lexbuf);_T4=
Cyc_Lexing_lexeme_end(lexbuf);_T5=Cyc_Lexing_lexeme_start(lexbuf);_T6=_T4 - _T5;_T7=_T6 - 2;_T8=(unsigned long)_T7;_T9=
# 228
Cyc_substring(_T3,0,_T8);*_T25=_T9;_T2=(struct _fat_ptr*)_T25;}Cyc_current_source=_T2;
# 230
Cyc_current_args=0;{struct Cyc_Set_Set**_T25=_cycalloc(sizeof(struct Cyc_Set_Set*));_TC=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T26)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_TC;_TB=_T26;}_TD=Cyc_strptrcmp;*_T25=_TB(_TD);_TA=(struct Cyc_Set_Set**)_T25;}Cyc_current_targets=_TA;
Cyc_token(lexbuf);
return 0;case 1:{struct _fat_ptr*_T25=_cycalloc(sizeof(struct _fat_ptr));_TF=
# 236
Cyc_Lexing_lexeme(lexbuf);_T10=
Cyc_Lexing_lexeme_end(lexbuf);_T11=Cyc_Lexing_lexeme_start(lexbuf);_T12=_T10 - _T11;_T13=_T12 - 1;_T14=(unsigned long)_T13;_T15=
# 236
Cyc_substring(_TF,0,_T14);*_T25=_T15;_TE=(struct _fat_ptr*)_T25;}Cyc_current_source=_TE;
# 238
Cyc_current_args=0;{struct Cyc_Set_Set**_T25=_cycalloc(sizeof(struct Cyc_Set_Set*));_T18=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T26)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T18;_T17=_T26;}_T19=Cyc_strptrcmp;*_T25=_T17(_T19);_T16=(struct Cyc_Set_Set**)_T25;}Cyc_current_targets=_T16;
Cyc_args(lexbuf);
return 0;case 2:{struct _fat_ptr*_T25=_cycalloc(sizeof(struct _fat_ptr));_T1B=
# 244
Cyc_Lexing_lexeme(lexbuf);*_T25=_T1B;_T1A=(struct _fat_ptr*)_T25;}Cyc_current_source=_T1A;
Cyc_current_args=0;{struct Cyc_Set_Set**_T25=_cycalloc(sizeof(struct Cyc_Set_Set*));_T1E=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T26)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T1E;_T1D=_T26;}_T1F=Cyc_strptrcmp;*_T25=_T1D(_T1F);_T1C=(struct Cyc_Set_Set**)_T25;}Cyc_current_targets=_T1C;
Cyc_token(lexbuf);
return 0;default: _T20=lexbuf;_T21=_T20->refill_buff;
# 250
_T21(lexbuf);_T22=
Cyc_macroname_rec(lexbuf,lexstate);return _T22;}{struct Cyc_Lexing_Error_exn_struct*_T25=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T25->tag=Cyc_Lexing_Error;
# 253
_T25->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T23=(struct Cyc_Lexing_Error_exn_struct*)_T25;}_T24=(void*)_T23;_throw(_T24);}
# 255
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_macroname_rec(lexbuf,1);return _T0;}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;int _T4;int _T5;int _T6;int _T7;unsigned long _T8;struct _fat_ptr _T9;struct Cyc_List_List*_TA;int _TB;struct _fat_ptr*_TC;struct _fat_ptr _TD;int _TE;int _TF;int _T10;int _T11;unsigned long _T12;struct _fat_ptr _T13;struct Cyc_List_List*_T14;int _T15;struct _fat_ptr*_T16;struct _fat_ptr _T17;int _T18;int _T19;int _T1A;int _T1B;unsigned long _T1C;struct _fat_ptr _T1D;struct Cyc_List_List*_T1E;int _T1F;struct Cyc_Lexing_lexbuf*_T20;void(*_T21)(struct Cyc_Lexing_lexbuf*);int _T22;struct Cyc_Lexing_Error_exn_struct*_T23;void*_T24;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:  {
# 253 "buildlib.cyl"
struct _fat_ptr*a;a=_cycalloc(sizeof(struct _fat_ptr));_T2=a;_T3=Cyc_Lexing_lexeme(lexbuf);_T4=
Cyc_Lexing_lexeme_end(lexbuf);_T5=Cyc_Lexing_lexeme_start(lexbuf);_T6=_T4 - _T5;_T7=_T6 - 2;_T8=(unsigned long)_T7;_T9=
# 253
Cyc_substring(_T3,0,_T8);*_T2=_T9;{struct Cyc_List_List*_T25=_cycalloc(sizeof(struct Cyc_List_List));
# 255
_T25->hd=a;_T25->tl=Cyc_current_args;_TA=(struct Cyc_List_List*)_T25;}Cyc_current_args=_TA;_TB=
Cyc_args(lexbuf);return _TB;}case 1:  {
# 259
struct _fat_ptr*a;a=_cycalloc(sizeof(struct _fat_ptr));_TC=a;_TD=Cyc_Lexing_lexeme(lexbuf);_TE=
Cyc_Lexing_lexeme_end(lexbuf);_TF=Cyc_Lexing_lexeme_start(lexbuf);_T10=_TE - _TF;_T11=_T10 - 1;_T12=(unsigned long)_T11;_T13=
# 259
Cyc_substring(_TD,0,_T12);*_TC=_T13;{struct Cyc_List_List*_T25=_cycalloc(sizeof(struct Cyc_List_List));
# 261
_T25->hd=a;_T25->tl=Cyc_current_args;_T14=(struct Cyc_List_List*)_T25;}Cyc_current_args=_T14;_T15=
Cyc_args(lexbuf);return _T15;}case 2:  {
# 265
struct _fat_ptr*a;a=_cycalloc(sizeof(struct _fat_ptr));_T16=a;_T17=Cyc_Lexing_lexeme(lexbuf);_T18=
Cyc_Lexing_lexeme_end(lexbuf);_T19=Cyc_Lexing_lexeme_start(lexbuf);_T1A=_T18 - _T19;_T1B=_T1A - 1;_T1C=(unsigned long)_T1B;_T1D=
# 265
Cyc_substring(_T17,0,_T1C);*_T16=_T1D;{struct Cyc_List_List*_T25=_cycalloc(sizeof(struct Cyc_List_List));
# 267
_T25->hd=a;_T25->tl=Cyc_current_args;_T1E=(struct Cyc_List_List*)_T25;}Cyc_current_args=_T1E;_T1F=
Cyc_token(lexbuf);return _T1F;}default: _T20=lexbuf;_T21=_T20->refill_buff;
# 270
_T21(lexbuf);_T22=
Cyc_args_rec(lexbuf,lexstate);return _T22;}{struct Cyc_Lexing_Error_exn_struct*_T25=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T25->tag=Cyc_Lexing_Error;
# 273
_T25->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T23=(struct Cyc_Lexing_Error_exn_struct*)_T25;}_T24=(void*)_T23;_throw(_T24);}
# 275
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_args_rec(lexbuf,2);return _T0;}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;int _T4;int _T5;int _T6;int _T7;int _T8;int _T9;int _TA;int _TB;int _TC;int _TD;int _TE;int _TF;int _T10;int _T11;int _T12;int _T13;int _T14;int _T15;int _T16;int _T17;int _T18;int _T19;int _T1A;int _T1B;int _T1C;int _T1D;int _T1E;int _T1F;int _T20;int _T21;int _T22;int _T23;int _T24;int _T25;int _T26;int _T27;int _T28;int _T29;int _T2A;int _T2B;int _T2C;int _T2D;int _T2E;int _T2F;int _T30;int _T31;int _T32;int _T33;struct Cyc_Lexing_lexbuf*_T34;void(*_T35)(struct Cyc_Lexing_lexbuf*);int _T36;struct Cyc_Lexing_Error_exn_struct*_T37;void*_T38;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));_T3=
# 274 "buildlib.cyl"
Cyc_Lexing_lexeme(lexbuf);*_T39=_T3;_T2=(struct _fat_ptr*)_T39;}Cyc_add_target(_T2);_T4=Cyc_token(lexbuf);return _T4;case 1:
# 277 "buildlib.cyl"
 return 0;case 2: _T5=
# 280 "buildlib.cyl"
Cyc_token(lexbuf);return _T5;case 3:
# 283 "buildlib.cyl"
 Cyc_string(lexbuf);_T6=Cyc_token(lexbuf);return _T6;case 4: _T7=
# 286 "buildlib.cyl"
Cyc_token(lexbuf);return _T7;case 5: _T8=
# 288
Cyc_token(lexbuf);return _T8;case 6: _T9=
# 290
Cyc_token(lexbuf);return _T9;case 7: _TA=
# 292
Cyc_token(lexbuf);return _TA;case 8: _TB=
# 295 "buildlib.cyl"
Cyc_token(lexbuf);return _TB;case 9: _TC=
# 298 "buildlib.cyl"
Cyc_token(lexbuf);return _TC;case 10: _TD=
# 301 "buildlib.cyl"
Cyc_token(lexbuf);return _TD;case 11: _TE=
# 303
Cyc_token(lexbuf);return _TE;case 12: _TF=
# 305
Cyc_token(lexbuf);return _TF;case 13: _T10=
# 307
Cyc_token(lexbuf);return _T10;case 14: _T11=
# 309
Cyc_token(lexbuf);return _T11;case 15: _T12=
# 311
Cyc_token(lexbuf);return _T12;case 16: _T13=
# 313
Cyc_token(lexbuf);return _T13;case 17: _T14=
# 315
Cyc_token(lexbuf);return _T14;case 18: _T15=
# 317
Cyc_token(lexbuf);return _T15;case 19: _T16=
# 319
Cyc_token(lexbuf);return _T16;case 20: _T17=
# 321
Cyc_token(lexbuf);return _T17;case 21: _T18=
# 323
Cyc_token(lexbuf);return _T18;case 22: _T19=
# 325
Cyc_token(lexbuf);return _T19;case 23: _T1A=
# 327
Cyc_token(lexbuf);return _T1A;case 24: _T1B=
# 330 "buildlib.cyl"
Cyc_token(lexbuf);return _T1B;case 25: _T1C=
# 332
Cyc_token(lexbuf);return _T1C;case 26: _T1D=
# 334
Cyc_token(lexbuf);return _T1D;case 27: _T1E=
# 336
Cyc_token(lexbuf);return _T1E;case 28: _T1F=
# 338
Cyc_token(lexbuf);return _T1F;case 29: _T20=
# 340
Cyc_token(lexbuf);return _T20;case 30: _T21=
# 342
Cyc_token(lexbuf);return _T21;case 31: _T22=
# 344
Cyc_token(lexbuf);return _T22;case 32: _T23=
# 346
Cyc_token(lexbuf);return _T23;case 33: _T24=
# 348
Cyc_token(lexbuf);return _T24;case 34: _T25=
# 350
Cyc_token(lexbuf);return _T25;case 35: _T26=
# 352
Cyc_token(lexbuf);return _T26;case 36: _T27=
# 354
Cyc_token(lexbuf);return _T27;case 37: _T28=
# 356
Cyc_token(lexbuf);return _T28;case 38: _T29=
# 358
Cyc_token(lexbuf);return _T29;case 39: _T2A=
# 360
Cyc_token(lexbuf);return _T2A;case 40: _T2B=
# 362
Cyc_token(lexbuf);return _T2B;case 41: _T2C=
# 364
Cyc_token(lexbuf);return _T2C;case 42: _T2D=
# 366
Cyc_token(lexbuf);return _T2D;case 43: _T2E=
# 368
Cyc_token(lexbuf);return _T2E;case 44: _T2F=
# 370
Cyc_token(lexbuf);return _T2F;case 45: _T30=
# 372
Cyc_token(lexbuf);return _T30;case 46: _T31=
# 374
Cyc_token(lexbuf);return _T31;case 47: _T32=
# 376
Cyc_token(lexbuf);return _T32;case 48: _T33=
# 379 "buildlib.cyl"
Cyc_token(lexbuf);return _T33;default: _T34=lexbuf;_T35=_T34->refill_buff;
_T35(lexbuf);_T36=
Cyc_token_rec(lexbuf,lexstate);return _T36;}{struct Cyc_Lexing_Error_exn_struct*_T39=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T39->tag=Cyc_Lexing_Error;
# 383
_T39->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T37=(struct Cyc_Lexing_Error_exn_struct*)_T39;}_T38=(void*)_T37;_throw(_T38);}
# 385
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_token_rec(lexbuf,3);return _T0;}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;int _T2;int _T3;int _T4;int _T5;int _T6;int _T7;int _T8;struct Cyc_Lexing_lexbuf*_T9;void(*_TA)(struct Cyc_Lexing_lexbuf*);int _TB;struct Cyc_Lexing_Error_exn_struct*_TC;void*_TD;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0: _T2=
# 384 "buildlib.cyl"
Cyc_string(lexbuf);return _T2;case 1:
# 385 "buildlib.cyl"
 return 0;case 2: _T3=
# 386 "buildlib.cyl"
Cyc_string(lexbuf);return _T3;case 3: _T4=
# 387 "buildlib.cyl"
Cyc_string(lexbuf);return _T4;case 4: _T5=
# 390 "buildlib.cyl"
Cyc_string(lexbuf);return _T5;case 5: _T6=
# 393 "buildlib.cyl"
Cyc_string(lexbuf);return _T6;case 6: _T7=
# 395
Cyc_string(lexbuf);return _T7;case 7:
# 396 "buildlib.cyl"
 return 0;case 8:
# 397 "buildlib.cyl"
 return 0;case 9: _T8=
# 398 "buildlib.cyl"
Cyc_string(lexbuf);return _T8;default: _T9=lexbuf;_TA=_T9->refill_buff;
_TA(lexbuf);_TB=
Cyc_string_rec(lexbuf,lexstate);return _TB;}{struct Cyc_Lexing_Error_exn_struct*_TE=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_TE->tag=Cyc_Lexing_Error;
# 402
_TE->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_TC=(struct Cyc_Lexing_Error_exn_struct*)_TE;}_TD=(void*)_TC;_throw(_TD);}
# 404
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_string_rec(lexbuf,4);return _T0;}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct Cyc___cycFILE*_T2;int _T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc___cycFILE*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc___cycFILE*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc___cycFILE*_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc___cycFILE*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct _fat_ptr _T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct _fat_ptr _T22;struct _fat_ptr _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct _fat_ptr _T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc___cycFILE*_T29;struct Cyc___cycFILE*_T2A;struct Cyc___cycFILE*_T2B;struct Cyc___cycFILE*_T2C;struct Cyc___cycFILE*_T2D;struct Cyc___cycFILE*_T2E;struct Cyc___cycFILE*_T2F;struct Cyc___cycFILE*_T30;char _T31;int _T32;struct Cyc___cycFILE*_T33;struct Cyc_Lexing_lexbuf*_T34;void(*_T35)(struct Cyc_Lexing_lexbuf*);int _T36;struct Cyc_Lexing_Error_exn_struct*_T37;void*_T38;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:
# 407 "buildlib.cyl"
 return 0;case 1: _T2=
# 409
_check_null(Cyc_slurp_out);Cyc_fputc(34,_T2);
_TL29: _T3=Cyc_slurp_string(lexbuf);if(_T3)goto _TL2A;else{goto _TL2B;}_TL2A: goto _TL29;_TL2B:
 return 1;case 2: _T4=
# 416 "buildlib.cyl"
_check_null(Cyc_slurp_out);Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",_T4);_T5=
_tag_fat("Warning: declaration of malloc sidestepped\n",sizeof(char),44U);_T6=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T5,_T6);
return 1;case 3: _T7=
# 422 "buildlib.cyl"
_check_null(Cyc_slurp_out);Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",_T7);_T8=
_tag_fat("Warning: declaration of malloc sidestepped\n",sizeof(char),44U);_T9=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T8,_T9);
return 1;case 4: _TA=
# 428 "buildlib.cyl"
_check_null(Cyc_slurp_out);Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",_TA);_TB=
_tag_fat("Warning: declaration of calloc sidestepped\n",sizeof(char),44U);_TC=_tag_fat(0U,sizeof(void*),0);Cyc_log(_TB,_TC);
return 1;case 5: _TD=
# 434 "buildlib.cyl"
_check_null(Cyc_slurp_out);Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",_TD);_TE=
_tag_fat("Warning: declaration of calloc sidestepped\n",sizeof(char),44U);_TF=_tag_fat(0U,sizeof(void*),0);Cyc_log(_TE,_TF);
return 1;case 6: _T10=
# 438
_check_null(Cyc_slurp_out);Cyc_fputs("__region",_T10);_T11=
_tag_fat("Warning: use of region sidestepped\n",sizeof(char),36U);_T12=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T11,_T12);
return 1;case 7: _T13=
# 442
_tag_fat("Warning: use of __extension__ deleted\n",sizeof(char),39U);_T14=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T13,_T14);
return 1;case 8: _T15=
# 446 "buildlib.cyl"
_tag_fat("Warning: use of nonnull attribute deleted\n",sizeof(char),43U);_T16=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T15,_T16);
return 1;case 9: _T17=
# 451 "buildlib.cyl"
_tag_fat("Warning: use of mode HI deleted\n",sizeof(char),33U);_T18=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T17,_T18);
return 1;case 10: _T19=
# 454
_tag_fat("Warning: use of mode SI deleted\n",sizeof(char),33U);_T1A=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T19,_T1A);
return 1;case 11: _T1B=
# 457
_tag_fat("Warning: use of mode QI deleted\n",sizeof(char),33U);_T1C=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T1B,_T1C);
return 1;case 12: _T1D=
# 460
_tag_fat("Warning: use of mode DI deleted\n",sizeof(char),33U);_T1E=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T1D,_T1E);
return 1;case 13: _T1F=
# 463
_tag_fat("Warning: use of mode DI deleted\n",sizeof(char),33U);_T20=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T1F,_T20);
return 1;case 14: _T21=
# 466
_tag_fat("Warning: use of mode word deleted\n",sizeof(char),35U);_T22=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T21,_T22);
return 1;case 15: _T23=
# 469
_tag_fat("Warning: use of __attribute__ ((deprecated)) deleted\n",sizeof(char),54U);_T24=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T23,_T24);
return 1;case 16: _T25=
# 472
_tag_fat("Warning: use of __attribute__ ((__deprecated__)) deleted\n",sizeof(char),58U);_T26=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T25,_T26);
return 1;case 17: _T27=
# 475
_tag_fat("Warning: use of __attribute__ ((__transparent_union__)) deleted\n",sizeof(char),65U);_T28=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T27,_T28);
return 1;case 18: _T29=
# 478
_check_null(Cyc_slurp_out);Cyc_fputs("inline",_T29);return 1;case 19: _T2A=
# 480
_check_null(Cyc_slurp_out);Cyc_fputs("inline",_T2A);return 1;case 20: _T2B=
# 482
_check_null(Cyc_slurp_out);Cyc_fputs("const",_T2B);return 1;case 21: _T2C=
# 484
_check_null(Cyc_slurp_out);Cyc_fputs("const",_T2C);return 1;case 22: _T2D=
# 486
_check_null(Cyc_slurp_out);Cyc_fputs("signed",_T2D);return 1;case 23: _T2E=
# 488
_check_null(Cyc_slurp_out);Cyc_fputs("signed",_T2E);return 1;case 24: _T2F=
# 490
_check_null(Cyc_slurp_out);Cyc_fputs("signed",_T2F);return 1;case 25: _T30=
# 495 "buildlib.cyl"
_check_null(Cyc_slurp_out);Cyc_fputs("int",_T30);return 1;case 26:
# 497
 return 1;case 27: _T31=
# 499
Cyc_Lexing_lexeme_char(lexbuf,0);_T32=(int)_T31;_T33=_check_null(Cyc_slurp_out);Cyc_fputc(_T32,_T33);return 1;default: _T34=lexbuf;_T35=_T34->refill_buff;
_T35(lexbuf);_T36=
Cyc_slurp_rec(lexbuf,lexstate);return _T36;}{struct Cyc_Lexing_Error_exn_struct*_T39=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T39->tag=Cyc_Lexing_Error;
# 503
_T39->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T37=(struct Cyc_Lexing_Error_exn_struct*)_T39;}_T38=(void*)_T37;_throw(_T38);}
# 505
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_slurp_rec(lexbuf,5);return _T0;}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct Cyc___cycFILE*_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct Cyc_String_pa_PrintArg_struct _T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_String_pa_PrintArg_struct _T9;struct Cyc___cycFILE*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc___cycFILE*_TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc_String_pa_PrintArg_struct _T11;struct Cyc___cycFILE*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;struct Cyc_String_pa_PrintArg_struct _T15;struct Cyc___cycFILE*_T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct Cyc_String_pa_PrintArg_struct _T19;struct Cyc___cycFILE*_T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct Cyc___cycFILE*_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct Cyc_Lexing_lexbuf*_T21;void(*_T22)(struct Cyc_Lexing_lexbuf*);int _T23;struct Cyc_Lexing_Error_exn_struct*_T24;void*_T25;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:
# 503 "buildlib.cyl"
 return 0;case 1: _T2=
# 505
_check_null(Cyc_slurp_out);Cyc_fputc(34,_T2);return 0;case 2: _T3=
# 507
_tag_fat("Warning: unclosed string\n",sizeof(char),26U);_T4=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T3,_T4);{struct Cyc_String_pa_PrintArg_struct _T26;_T26.tag=0;
_T26.f1=Cyc_Lexing_lexeme(lexbuf);_T5=_T26;}{struct Cyc_String_pa_PrintArg_struct _T26=_T5;void*_T27[1];_T27[0]=& _T26;_T6=_check_null(Cyc_slurp_out);_T7=_tag_fat("%s",sizeof(char),3U);_T8=_tag_fat(_T27,sizeof(void*),1);Cyc_fprintf(_T6,_T7,_T8);}return 1;case 3:{struct Cyc_String_pa_PrintArg_struct _T26;_T26.tag=0;
# 510
_T26.f1=Cyc_Lexing_lexeme(lexbuf);_T9=_T26;}{struct Cyc_String_pa_PrintArg_struct _T26=_T9;void*_T27[1];_T27[0]=& _T26;_TA=_check_null(Cyc_slurp_out);_TB=_tag_fat("%s",sizeof(char),3U);_TC=_tag_fat(_T27,sizeof(void*),1);Cyc_fprintf(_TA,_TB,_TC);}return 1;case 4:{struct Cyc_String_pa_PrintArg_struct _T26;_T26.tag=0;
# 512
_T26.f1=Cyc_Lexing_lexeme(lexbuf);_TD=_T26;}{struct Cyc_String_pa_PrintArg_struct _T26=_TD;void*_T27[1];_T27[0]=& _T26;_TE=_check_null(Cyc_slurp_out);_TF=_tag_fat("%s",sizeof(char),3U);_T10=_tag_fat(_T27,sizeof(void*),1);Cyc_fprintf(_TE,_TF,_T10);}return 1;case 5:{struct Cyc_String_pa_PrintArg_struct _T26;_T26.tag=0;
# 514
_T26.f1=Cyc_Lexing_lexeme(lexbuf);_T11=_T26;}{struct Cyc_String_pa_PrintArg_struct _T26=_T11;void*_T27[1];_T27[0]=& _T26;_T12=_check_null(Cyc_slurp_out);_T13=_tag_fat("%s",sizeof(char),3U);_T14=_tag_fat(_T27,sizeof(void*),1);Cyc_fprintf(_T12,_T13,_T14);}return 1;case 6:{struct Cyc_String_pa_PrintArg_struct _T26;_T26.tag=0;
# 516
_T26.f1=Cyc_Lexing_lexeme(lexbuf);_T15=_T26;}{struct Cyc_String_pa_PrintArg_struct _T26=_T15;void*_T27[1];_T27[0]=& _T26;_T16=_check_null(Cyc_slurp_out);_T17=_tag_fat("%s",sizeof(char),3U);_T18=_tag_fat(_T27,sizeof(void*),1);Cyc_fprintf(_T16,_T17,_T18);}return 1;case 7:{struct Cyc_String_pa_PrintArg_struct _T26;_T26.tag=0;
# 518
_T26.f1=Cyc_Lexing_lexeme(lexbuf);_T19=_T26;}{struct Cyc_String_pa_PrintArg_struct _T26=_T19;void*_T27[1];_T27[0]=& _T26;_T1A=_check_null(Cyc_slurp_out);_T1B=_tag_fat("%s",sizeof(char),3U);_T1C=_tag_fat(_T27,sizeof(void*),1);Cyc_fprintf(_T1A,_T1B,_T1C);}return 1;case 8:{struct Cyc_String_pa_PrintArg_struct _T26;_T26.tag=0;
# 520
_T26.f1=Cyc_Lexing_lexeme(lexbuf);_T1D=_T26;}{struct Cyc_String_pa_PrintArg_struct _T26=_T1D;void*_T27[1];_T27[0]=& _T26;_T1E=_check_null(Cyc_slurp_out);_T1F=_tag_fat("%s",sizeof(char),3U);_T20=_tag_fat(_T27,sizeof(void*),1);Cyc_fprintf(_T1E,_T1F,_T20);}return 1;default: _T21=lexbuf;_T22=_T21->refill_buff;
_T22(lexbuf);_T23=
Cyc_slurp_string_rec(lexbuf,lexstate);return _T23;}{struct Cyc_Lexing_Error_exn_struct*_T26=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T26->tag=Cyc_Lexing_Error;
# 524
_T26->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T24=(struct Cyc_Lexing_Error_exn_struct*)_T26;}_T25=(void*)_T24;_throw(_T25);}
# 526
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_slurp_string_rec(lexbuf,6);return _T0;}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;int _T2;int _T3;struct Cyc_Lexing_lexbuf*_T4;void(*_T5)(struct Cyc_Lexing_lexbuf*);int _T6;struct Cyc_Lexing_Error_exn_struct*_T7;void*_T8;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:
# 530 "buildlib.cyl"
 return 0;case 1:
# 532
 if(Cyc_parens_to_match!=1)goto _TL2E;return 0;_TL2E:
 Cyc_parens_to_match=Cyc_parens_to_match + -1;
return 1;case 2:
# 536
 Cyc_parens_to_match=Cyc_parens_to_match + 1;
return 1;case 3:
# 539
 _TL30: _T2=Cyc_asm_string(lexbuf);if(_T2)goto _TL31;else{goto _TL32;}_TL31: goto _TL30;_TL32:
 return 1;case 4:
# 542
 _TL33: _T3=Cyc_asm_comment(lexbuf);if(_T3)goto _TL34;else{goto _TL35;}_TL34: goto _TL33;_TL35:
 return 1;case 5:
# 545
 return 1;case 6:
# 547
 return 1;default: _T4=lexbuf;_T5=_T4->refill_buff;
_T5(lexbuf);_T6=
Cyc_asmtok_rec(lexbuf,lexstate);return _T6;}{struct Cyc_Lexing_Error_exn_struct*_T9=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T9->tag=Cyc_Lexing_Error;
# 551
_T9->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T7=(struct Cyc_Lexing_Error_exn_struct*)_T9;}_T8=(void*)_T7;_throw(_T8);}
# 553
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_asmtok_rec(lexbuf,7);return _T0;}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_Lexing_lexbuf*_T6;void(*_T7)(struct Cyc_Lexing_lexbuf*);int _T8;struct Cyc_Lexing_Error_exn_struct*_T9;void*_TA;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0: _T2=
# 551 "buildlib.cyl"
_tag_fat("Warning: unclosed string\n",sizeof(char),26U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T2,_T3);return 0;case 1:
# 553
 return 0;case 2: _T4=
# 555
_tag_fat("Warning: unclosed string\n",sizeof(char),26U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T4,_T5);return 1;case 3:
# 557
 return 1;case 4:
# 559
 return 1;case 5:
# 561
 return 1;case 6:
# 563
 return 1;case 7:
# 565
 return 1;case 8:
# 567
 return 1;default: _T6=lexbuf;_T7=_T6->refill_buff;
_T7(lexbuf);_T8=
Cyc_asm_string_rec(lexbuf,lexstate);return _T8;}{struct Cyc_Lexing_Error_exn_struct*_TB=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_TB->tag=Cyc_Lexing_Error;
# 571
_TB->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T9=(struct Cyc_Lexing_Error_exn_struct*)_TB;}_TA=(void*)_T9;_throw(_TA);}
# 573
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_asm_string_rec(lexbuf,8);return _T0;}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_Lexing_lexbuf*_T4;void(*_T5)(struct Cyc_Lexing_lexbuf*);int _T6;struct Cyc_Lexing_Error_exn_struct*_T7;void*_T8;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0: _T2=
# 571 "buildlib.cyl"
_tag_fat("Warning: unclosed comment\n",sizeof(char),27U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T2,_T3);return 0;case 1:
# 573
 return 0;case 2:
# 575
 return 1;default: _T4=lexbuf;_T5=_T4->refill_buff;
_T5(lexbuf);_T6=
Cyc_asm_comment_rec(lexbuf,lexstate);return _T6;}{struct Cyc_Lexing_Error_exn_struct*_T9=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T9->tag=Cyc_Lexing_Error;
# 579
_T9->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T7=(struct Cyc_Lexing_Error_exn_struct*)_T9;}_T8=(void*)_T7;_throw(_T8);}
# 581
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_asm_comment_rec(lexbuf,9);return _T0;}
struct _tuple14*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _tuple14*_T2;struct _tuple14*_T3;struct Cyc_Lexing_lexbuf*_T4;void(*_T5)(struct Cyc_Lexing_lexbuf*);struct _tuple14*_T6;struct Cyc_Lexing_Error_exn_struct*_T7;void*_T8;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:
# 583 "buildlib.cyl"
 Cyc_current_line=_tag_fat("#define ",sizeof(char),9U);
Cyc_suck_macroname(lexbuf);{struct _tuple14*_T9=_cycalloc(sizeof(struct _tuple14));
_T9->f0=Cyc_current_line;_T9->f1=_check_null(Cyc_current_source);_T2=(struct _tuple14*)_T9;}return _T2;case 1: _T3=
# 587
Cyc_suck_line(lexbuf);return _T3;case 2:
# 589
 return 0;default: _T4=lexbuf;_T5=_T4->refill_buff;
_T5(lexbuf);_T6=
Cyc_suck_line_rec(lexbuf,lexstate);return _T6;}{struct Cyc_Lexing_Error_exn_struct*_T9=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T9->tag=Cyc_Lexing_Error;
# 593
_T9->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T7=(struct Cyc_Lexing_Error_exn_struct*)_T9;}_T8=(void*)_T7;_throw(_T8);}
# 595
struct _tuple14*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){struct _tuple14*_T0;_T0=Cyc_suck_line_rec(lexbuf,10);return _T0;}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;int _T5;struct Cyc_Lexing_lexbuf*_T6;void(*_T7)(struct Cyc_Lexing_lexbuf*);int _T8;struct Cyc_Lexing_Error_exn_struct*_T9;void*_TA;
lexstate=Cyc_lex_engine(lexstate,lexbuf);if(lexstate!=0)goto _TL39;{struct _fat_ptr*_TB=_cycalloc(sizeof(struct _fat_ptr));_T1=
# 593 "buildlib.cyl"
Cyc_Lexing_lexeme(lexbuf);*_TB=_T1;_T0=(struct _fat_ptr*)_TB;}Cyc_current_source=_T0;_T2=Cyc_current_line;_T3=Cyc_current_source;_T4=*_T3;
Cyc_current_line=Cyc_strconcat(_T2,_T4);_T5=
Cyc_suck_restofline(lexbuf);return _T5;_TL39: _T6=lexbuf;_T7=_T6->refill_buff;
# 597
_T7(lexbuf);_T8=
Cyc_suck_macroname_rec(lexbuf,lexstate);return _T8;{struct Cyc_Lexing_Error_exn_struct*_TB=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_TB->tag=Cyc_Lexing_Error;
# 600
_TB->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T9=(struct Cyc_Lexing_Error_exn_struct*)_TB;}_TA=(void*)_T9;_throw(_TA);}
# 602
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_suck_macroname_rec(lexbuf,11);return _T0;}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){struct _fat_ptr _T0;struct _fat_ptr _T1;struct Cyc_Lexing_lexbuf*_T2;void(*_T3)(struct Cyc_Lexing_lexbuf*);int _T4;struct Cyc_Lexing_Error_exn_struct*_T5;void*_T6;
lexstate=Cyc_lex_engine(lexstate,lexbuf);if(lexstate!=0)goto _TL3B;_T0=Cyc_current_line;_T1=
# 600 "buildlib.cyl"
Cyc_Lexing_lexeme(lexbuf);Cyc_current_line=Cyc_strconcat(_T0,_T1);return 0;_TL3B: _T2=lexbuf;_T3=_T2->refill_buff;
_T3(lexbuf);_T4=
Cyc_suck_restofline_rec(lexbuf,lexstate);return _T4;{struct Cyc_Lexing_Error_exn_struct*_T7=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T7->tag=Cyc_Lexing_Error;
# 604
_T7->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T5=(struct Cyc_Lexing_Error_exn_struct*)_T7;}_T6=(void*)_T5;_throw(_T6);}
# 606
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_suck_restofline_rec(lexbuf,12);return _T0;}
struct _tuple17*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _tuple17*_T2;struct _fat_ptr _T3;int _T4;int _T5;int _T6;int _T7;unsigned long _T8;struct _fat_ptr _T9;int _TA;struct _tuple17*_TB;struct _tuple17*_TC;struct Cyc_Int_pa_PrintArg_struct _TD;char _TE;int _TF;struct Cyc___cycFILE*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;struct Cyc_Lexing_lexbuf*_T13;void(*_T14)(struct Cyc_Lexing_lexbuf*);struct _tuple17*_T15;struct Cyc_Lexing_Error_exn_struct*_T16;void*_T17;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0: _T2=
# 607 "buildlib.cyl"
Cyc_spec(lexbuf);return _T2;case 1: _T3=
# 610
Cyc_Lexing_lexeme(lexbuf);_T4=
Cyc_Lexing_lexeme_end(lexbuf);_T5=Cyc_Lexing_lexeme_start(lexbuf);_T6=_T4 - _T5;_T7=_T6 - 1;_T8=(unsigned long)_T7;_T9=
# 610
Cyc_substring(_T3,0,_T8);
# 609
Cyc_current_headerfile=_T9;
# 612
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
Cyc_current_cpp=0;
_TL3E: _TA=Cyc_commands(lexbuf);if(_TA)goto _TL3F;else{goto _TL40;}_TL3F: goto _TL3E;_TL40:
 Cyc_current_hstubs=Cyc_List_imp_rev(Cyc_current_hstubs);
Cyc_current_cstubs=Cyc_List_imp_rev(Cyc_current_cstubs);
Cyc_current_cycstubs=Cyc_List_imp_rev(Cyc_current_cycstubs);
Cyc_current_cpp=Cyc_List_imp_rev(Cyc_current_cpp);{struct _tuple17*_T18=_cycalloc(sizeof(struct _tuple17));
_T18->f0=Cyc_current_headerfile;
_T18->f1=Cyc_current_symbols;
_T18->f2=Cyc_current_user_defs;
_T18->f3=Cyc_current_omit_symbols;
_T18->f4=Cyc_current_hstubs;
_T18->f5=Cyc_current_cstubs;
_T18->f6=Cyc_current_cycstubs;
_T18->f7=Cyc_current_cpp;_TB=(struct _tuple17*)_T18;}
# 624
return _TB;case 2: _TC=
# 634
Cyc_spec(lexbuf);return _TC;case 3:
# 636
 return 0;case 4:{struct Cyc_Int_pa_PrintArg_struct _T18;_T18.tag=1;_TE=
# 640
Cyc_Lexing_lexeme_char(lexbuf,0);_TF=(int)_TE;_T18.f1=(unsigned long)_TF;_TD=_T18;}{struct Cyc_Int_pa_PrintArg_struct _T18=_TD;void*_T19[1];_T19[0]=& _T18;_T10=Cyc_stderr;_T11=
# 639
_tag_fat("Error in .cys file: expected header file name, found '%c' instead\n",sizeof(char),67U);_T12=_tag_fat(_T19,sizeof(void*),1);Cyc_fprintf(_T10,_T11,_T12);}
# 641
return 0;default: _T13=lexbuf;_T14=_T13->refill_buff;
_T14(lexbuf);_T15=
Cyc_spec_rec(lexbuf,lexstate);return _T15;}{struct Cyc_Lexing_Error_exn_struct*_T18=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T18->tag=Cyc_Lexing_Error;
# 645
_T18->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T16=(struct Cyc_Lexing_Error_exn_struct*)_T18;}_T17=(void*)_T16;_throw(_T17);}
# 647
struct _tuple17*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){struct _tuple17*_T0;_T0=Cyc_spec_rec(lexbuf,13);return _T0;}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;int _T2;int _T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;int _T7;struct _tuple15*_T8;struct _fat_ptr _T9;struct _tuple15*_TA;struct Cyc_Buffer_t*_TB;struct _fat_ptr _TC;struct Cyc_List_List*_TD;struct _fat_ptr*_TE;struct _fat_ptr _TF;unsigned long _T10;int _T11;struct _fat_ptr _T12;unsigned char*_T13;char*_T14;char _T15;int _T16;int _T17;struct _fat_ptr*_T18;struct _fat_ptr _T19;unsigned char*_T1A;char*_T1B;char _T1C;int _T1D;int _T1E;struct _fat_ptr*_T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;unsigned char*_T22;struct _fat_ptr _T23;unsigned char*_T24;int _T25;int _T26;unsigned long _T27;int _T28;struct _tuple15*_T29;struct _fat_ptr _T2A;struct _tuple15*_T2B;struct Cyc_Buffer_t*_T2C;struct _fat_ptr _T2D;struct Cyc_List_List*_T2E;int _T2F;struct _tuple15*_T30;struct _fat_ptr _T31;struct _tuple15*_T32;struct Cyc_Buffer_t*_T33;struct _fat_ptr _T34;struct Cyc_List_List*_T35;struct _fat_ptr*_T36;struct _fat_ptr _T37;unsigned long _T38;int _T39;struct _fat_ptr _T3A;unsigned char*_T3B;char*_T3C;char _T3D;int _T3E;int _T3F;struct _fat_ptr*_T40;struct _fat_ptr _T41;unsigned char*_T42;char*_T43;char _T44;int _T45;int _T46;struct _fat_ptr*_T47;struct _fat_ptr _T48;struct _fat_ptr _T49;unsigned char*_T4A;struct _fat_ptr _T4B;unsigned char*_T4C;int _T4D;int _T4E;unsigned long _T4F;int _T50;struct _tuple15*_T51;struct _fat_ptr _T52;struct _tuple15*_T53;struct Cyc_Buffer_t*_T54;struct _fat_ptr _T55;struct Cyc_List_List*_T56;int _T57;struct _tuple15*_T58;struct _fat_ptr _T59;struct _tuple15*_T5A;struct Cyc_Buffer_t*_T5B;struct _fat_ptr _T5C;struct Cyc_List_List*_T5D;struct _fat_ptr*_T5E;struct _fat_ptr _T5F;unsigned long _T60;int _T61;struct _fat_ptr _T62;unsigned char*_T63;char*_T64;char _T65;int _T66;int _T67;struct _fat_ptr*_T68;struct _fat_ptr _T69;unsigned char*_T6A;char*_T6B;char _T6C;int _T6D;int _T6E;struct _fat_ptr*_T6F;struct _fat_ptr _T70;struct _fat_ptr _T71;unsigned char*_T72;struct _fat_ptr _T73;unsigned char*_T74;int _T75;int _T76;unsigned long _T77;int _T78;struct _tuple15*_T79;struct _fat_ptr _T7A;struct _tuple15*_T7B;struct Cyc_Buffer_t*_T7C;struct _fat_ptr _T7D;struct Cyc_List_List*_T7E;int _T7F;struct _fat_ptr*_T80;struct Cyc_Buffer_t*_T81;struct _fat_ptr _T82;struct Cyc_List_List*_T83;struct Cyc_Int_pa_PrintArg_struct _T84;char _T85;int _T86;struct Cyc___cycFILE*_T87;struct _fat_ptr _T88;struct _fat_ptr _T89;struct Cyc_Lexing_lexbuf*_T8A;void(*_T8B)(struct Cyc_Lexing_lexbuf*);int _T8C;struct Cyc_Lexing_Error_exn_struct*_T8D;void*_T8E;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:
# 645 "buildlib.cyl"
 return 0;case 1:
# 647
 return 0;case 2:
# 649
 Cyc_snarfed_symbols=0;
_TL42: _T2=Cyc_snarfsymbols(lexbuf);if(_T2)goto _TL43;else{goto _TL44;}_TL43: goto _TL42;_TL44:
 Cyc_current_symbols=Cyc_List_append(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3:
# 654
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*tmp_user_defs=Cyc_current_user_defs;
_TL45: _T3=Cyc_snarfsymbols(lexbuf);if(_T3)goto _TL46;else{goto _TL47;}_TL46: goto _TL45;_TL47:
 if(tmp_user_defs==Cyc_current_user_defs)goto _TL48;_T4=Cyc_stderr;_T5=
# 659
_tag_fat("Error in .cys file: got optional definition in omitsymbols\n",sizeof(char),60U);_T6=_tag_fat(0U,sizeof(void*),0);
# 658
Cyc_fprintf(_T4,_T5,_T6);
# 660
return 0;_TL48:
# 662
 Cyc_current_omit_symbols=Cyc_List_append(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;}case 4:
# 665
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
_TL4A: _T7=Cyc_block(lexbuf);if(_T7)goto _TL4B;else{goto _TL4C;}_TL4B: goto _TL4A;_TL4C: {
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15));_T8=x;_T9=_tag_fat(0,0,0);_T8->f0=_T9;_TA=x;_TB=
_check_null(Cyc_specbuf);_TC=Cyc_Buffer_contents(_TB);_TA->f1=_TC;{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));
_T8F->hd=x;_T8F->tl=Cyc_current_hstubs;_TD=(struct Cyc_List_List*)_T8F;}Cyc_current_hstubs=_TD;
return 1;}case 5:  {
# 673
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);_TE=& s;_TF=
_tag_fat("hstub",sizeof(char),6U);_T10=Cyc_strlen(_TF);_T11=(int)_T10;_fat_ptr_inplace_plus(_TE,sizeof(char),_T11);
_TL4D: _T12=s;_T13=_check_fat_subscript(_T12,sizeof(char),0U);_T14=(char*)_T13;_T15=*_T14;_T16=(int)_T15;_T17=isspace(_T16);if(_T17)goto _TL4E;else{goto _TL4F;}_TL4E: _T18=& s;_fat_ptr_inplace_plus(_T18,sizeof(char),1);goto _TL4D;_TL4F: {
struct _fat_ptr t=s;
_TL50: _T19=t;_T1A=_check_fat_subscript(_T19,sizeof(char),0U);_T1B=(char*)_T1A;_T1C=*_T1B;_T1D=(int)_T1C;_T1E=isspace(_T1D);if(_T1E)goto _TL52;else{goto _TL51;}_TL51: _T1F=& t;_fat_ptr_inplace_plus(_T1F,sizeof(char),1);goto _TL50;_TL52: _T20=s;_T21=t;_T22=_T21.curr;_T23=s;_T24=_T23.curr;_T25=_T22 - _T24;_T26=_T25 / sizeof(char);_T27=(unsigned long)_T26;{
struct _fat_ptr symbol=Cyc_substring(_T20,0,_T27);
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
_TL53: _T28=Cyc_block(lexbuf);if(_T28)goto _TL54;else{goto _TL55;}_TL54: goto _TL53;_TL55: {
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15));_T29=x;_T2A=symbol;_T29->f0=_T2A;_T2B=x;_T2C=
_check_null(Cyc_specbuf);_T2D=Cyc_Buffer_contents(_T2C);_T2B->f1=_T2D;{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));
_T8F->hd=x;_T8F->tl=Cyc_current_hstubs;_T2E=(struct Cyc_List_List*)_T8F;}Cyc_current_hstubs=_T2E;
return 1;}}}}case 6:
# 687
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
_TL56: _T2F=Cyc_block(lexbuf);if(_T2F)goto _TL57;else{goto _TL58;}_TL57: goto _TL56;_TL58: {
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15));_T30=x;_T31=_tag_fat(0,0,0);_T30->f0=_T31;_T32=x;_T33=
_check_null(Cyc_specbuf);_T34=Cyc_Buffer_contents(_T33);_T32->f1=_T34;{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));
_T8F->hd=x;_T8F->tl=Cyc_current_cstubs;_T35=(struct Cyc_List_List*)_T8F;}Cyc_current_cstubs=_T35;
return 1;}case 7:  {
# 695
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);_T36=& s;_T37=
_tag_fat("cstub",sizeof(char),6U);_T38=Cyc_strlen(_T37);_T39=(int)_T38;_fat_ptr_inplace_plus(_T36,sizeof(char),_T39);
_TL59: _T3A=s;_T3B=_check_fat_subscript(_T3A,sizeof(char),0U);_T3C=(char*)_T3B;_T3D=*_T3C;_T3E=(int)_T3D;_T3F=isspace(_T3E);if(_T3F)goto _TL5A;else{goto _TL5B;}_TL5A: _T40=& s;_fat_ptr_inplace_plus(_T40,sizeof(char),1);goto _TL59;_TL5B: {
struct _fat_ptr t=s;
_TL5C: _T41=t;_T42=_check_fat_subscript(_T41,sizeof(char),0U);_T43=(char*)_T42;_T44=*_T43;_T45=(int)_T44;_T46=isspace(_T45);if(_T46)goto _TL5E;else{goto _TL5D;}_TL5D: _T47=& t;_fat_ptr_inplace_plus(_T47,sizeof(char),1);goto _TL5C;_TL5E: _T48=s;_T49=t;_T4A=_T49.curr;_T4B=s;_T4C=_T4B.curr;_T4D=_T4A - _T4C;_T4E=_T4D / sizeof(char);_T4F=(unsigned long)_T4E;{
struct _fat_ptr symbol=Cyc_substring(_T48,0,_T4F);
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
_TL5F: _T50=Cyc_block(lexbuf);if(_T50)goto _TL60;else{goto _TL61;}_TL60: goto _TL5F;_TL61: {
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15));_T51=x;_T52=symbol;_T51->f0=_T52;_T53=x;_T54=
_check_null(Cyc_specbuf);_T55=Cyc_Buffer_contents(_T54);_T53->f1=_T55;{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));
_T8F->hd=x;_T8F->tl=Cyc_current_cstubs;_T56=(struct Cyc_List_List*)_T8F;}Cyc_current_cstubs=_T56;
return 1;}}}}case 8:
# 709
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
_TL62: _T57=Cyc_block(lexbuf);if(_T57)goto _TL63;else{goto _TL64;}_TL63: goto _TL62;_TL64: {
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15));_T58=x;_T59=_tag_fat(0,0,0);_T58->f0=_T59;_T5A=x;_T5B=
_check_null(Cyc_specbuf);_T5C=Cyc_Buffer_contents(_T5B);_T5A->f1=_T5C;{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));
_T8F->hd=x;_T8F->tl=Cyc_current_cycstubs;_T5D=(struct Cyc_List_List*)_T8F;}Cyc_current_cycstubs=_T5D;
return 1;}case 9:  {
# 717
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);_T5E=& s;_T5F=
_tag_fat("cycstub",sizeof(char),8U);_T60=Cyc_strlen(_T5F);_T61=(int)_T60;_fat_ptr_inplace_plus(_T5E,sizeof(char),_T61);
_TL65: _T62=s;_T63=_check_fat_subscript(_T62,sizeof(char),0U);_T64=(char*)_T63;_T65=*_T64;_T66=(int)_T65;_T67=isspace(_T66);if(_T67)goto _TL66;else{goto _TL67;}_TL66: _T68=& s;_fat_ptr_inplace_plus(_T68,sizeof(char),1);goto _TL65;_TL67: {
struct _fat_ptr t=s;
_TL68: _T69=t;_T6A=_check_fat_subscript(_T69,sizeof(char),0U);_T6B=(char*)_T6A;_T6C=*_T6B;_T6D=(int)_T6C;_T6E=isspace(_T6D);if(_T6E)goto _TL6A;else{goto _TL69;}_TL69: _T6F=& t;_fat_ptr_inplace_plus(_T6F,sizeof(char),1);goto _TL68;_TL6A: _T70=s;_T71=t;_T72=_T71.curr;_T73=s;_T74=_T73.curr;_T75=_T72 - _T74;_T76=_T75 / sizeof(char);_T77=(unsigned long)_T76;{
struct _fat_ptr symbol=Cyc_substring(_T70,0,_T77);
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
_TL6B: _T78=Cyc_block(lexbuf);if(_T78)goto _TL6C;else{goto _TL6D;}_TL6C: goto _TL6B;_TL6D: {
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15));_T79=x;_T7A=symbol;_T79->f0=_T7A;_T7B=x;_T7C=
_check_null(Cyc_specbuf);_T7D=Cyc_Buffer_contents(_T7C);_T7B->f1=_T7D;{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));
_T8F->hd=x;_T8F->tl=Cyc_current_cycstubs;_T7E=(struct Cyc_List_List*)_T8F;}Cyc_current_cycstubs=_T7E;
return 1;}}}}case 10:
# 731
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
_TL6E: _T7F=Cyc_block(lexbuf);if(_T7F)goto _TL6F;else{goto _TL70;}_TL6F: goto _TL6E;_TL70: {
struct _fat_ptr*x;x=_cycalloc(sizeof(struct _fat_ptr));_T80=x;_T81=_check_null(Cyc_specbuf);_T82=Cyc_Buffer_contents(_T81);*_T80=_T82;{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));
_T8F->hd=x;_T8F->tl=Cyc_current_cpp;_T83=(struct Cyc_List_List*)_T8F;}Cyc_current_cpp=_T83;
return 1;}case 11:
# 738
 return 1;case 12:
# 740
 return 1;case 13:{struct Cyc_Int_pa_PrintArg_struct _T8F;_T8F.tag=1;_T85=
# 744
Cyc_Lexing_lexeme_char(lexbuf,0);_T86=(int)_T85;_T8F.f1=(unsigned long)_T86;_T84=_T8F;}{struct Cyc_Int_pa_PrintArg_struct _T8F=_T84;void*_T90[1];_T90[0]=& _T8F;_T87=Cyc_stderr;_T88=
# 743
_tag_fat("Error in .cys file: expected command, found '%c' instead\n",sizeof(char),58U);_T89=_tag_fat(_T90,sizeof(void*),1);Cyc_fprintf(_T87,_T88,_T89);}
# 745
return 0;default: _T8A=lexbuf;_T8B=_T8A->refill_buff;
_T8B(lexbuf);_T8C=
Cyc_commands_rec(lexbuf,lexstate);return _T8C;}{struct Cyc_Lexing_Error_exn_struct*_T8F=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T8F->tag=Cyc_Lexing_Error;
# 749
_T8F->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T8D=(struct Cyc_Lexing_Error_exn_struct*)_T8F;}_T8E=(void*)_T8D;_throw(_T8E);}
# 751
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_commands_rec(lexbuf,14);return _T0;}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct Cyc_List_List*_T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;unsigned char*_T6;char*_T7;char _T8;int _T9;int _TA;struct _fat_ptr*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;unsigned char*_TE;struct _fat_ptr _TF;unsigned char*_T10;int _T11;int _T12;unsigned long _T13;int _T14;struct _tuple16*_T15;struct _fat_ptr*_T16;struct _tuple16*_T17;struct _fat_ptr*_T18;struct Cyc_Buffer_t*_T19;struct _fat_ptr _T1A;struct Cyc_List_List*_T1B;struct _fat_ptr*_T1C;struct _fat_ptr _T1D;struct Cyc_List_List*_T1E;struct Cyc___cycFILE*_T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct Cyc_Int_pa_PrintArg_struct _T22;char _T23;int _T24;struct Cyc___cycFILE*_T25;struct _fat_ptr _T26;struct _fat_ptr _T27;struct Cyc_Lexing_lexbuf*_T28;void(*_T29)(struct Cyc_Lexing_lexbuf*);int _T2A;struct Cyc_Lexing_Error_exn_struct*_T2B;void*_T2C;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:{struct Cyc_List_List*_T2D=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T2E=_cycalloc(sizeof(struct _fat_ptr));_T4=
# 755 "buildlib.cyl"
Cyc_Lexing_lexeme(lexbuf);*_T2E=_T4;_T3=(struct _fat_ptr*)_T2E;}_T2D->hd=_T3;_T2D->tl=Cyc_snarfed_symbols;_T2=(struct Cyc_List_List*)_T2D;}Cyc_snarfed_symbols=_T2;
return 1;case 1:  {
# 758
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);
struct _fat_ptr t=s;
_TL72: _T5=t;_T6=_check_fat_subscript(_T5,sizeof(char),0U);_T7=(char*)_T6;_T8=*_T7;_T9=(int)_T8;_TA=isspace(_T9);if(_TA)goto _TL74;else{goto _TL73;}_TL73: _TB=& t;_fat_ptr_inplace_plus(_TB,sizeof(char),1);goto _TL72;_TL74: _TC=s;_TD=t;_TE=_TD.curr;_TF=s;_T10=_TF.curr;_T11=_TE - _T10;_T12=_T11 / sizeof(char);_T13=(unsigned long)_T12;
Cyc_current_symbol=Cyc_substring(_TC,0,_T13);
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
_TL75: _T14=Cyc_block(lexbuf);if(_T14)goto _TL76;else{goto _TL77;}_TL76: goto _TL75;_TL77:
# 767
 Cyc_rename_current_symbol=0;{
struct _tuple16*user_def;user_def=_cycalloc(sizeof(struct _tuple16));_T15=user_def;{struct _fat_ptr*_T2D=_cycalloc(sizeof(struct _fat_ptr));*_T2D=Cyc_current_symbol;_T16=(struct _fat_ptr*)_T2D;}_T15->f0=_T16;_T17=user_def;{struct _fat_ptr*_T2D=_cycalloc(sizeof(struct _fat_ptr));_T19=
_check_null(Cyc_specbuf);_T1A=Cyc_Buffer_contents(_T19);*_T2D=_T1A;_T18=(struct _fat_ptr*)_T2D;}_T17->f1=_T18;{struct Cyc_List_List*_T2D=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T2E=_cycalloc(sizeof(struct _fat_ptr));_T1D=Cyc_current_symbol;
*_T2E=_T1D;_T1C=(struct _fat_ptr*)_T2E;}_T2D->hd=_T1C;_T2D->tl=Cyc_snarfed_symbols;_T1B=(struct Cyc_List_List*)_T2D;}Cyc_snarfed_symbols=_T1B;{struct Cyc_List_List*_T2D=_cycalloc(sizeof(struct Cyc_List_List));
_T2D->hd=user_def;_T2D->tl=Cyc_current_user_defs;_T1E=(struct Cyc_List_List*)_T2D;}Cyc_current_user_defs=_T1E;
return 1;}}case 2:
# 774
 return 1;case 3:
# 776
 return 0;case 4: _T1F=Cyc_stderr;_T20=
# 779
_tag_fat("Error in .cys file: unexpected end-of-file\n",sizeof(char),44U);_T21=_tag_fat(0U,sizeof(void*),0);
# 778
Cyc_fprintf(_T1F,_T20,_T21);
# 780
return 0;case 5:{struct Cyc_Int_pa_PrintArg_struct _T2D;_T2D.tag=1;_T23=
# 784
Cyc_Lexing_lexeme_char(lexbuf,0);_T24=(int)_T23;_T2D.f1=(unsigned long)_T24;_T22=_T2D;}{struct Cyc_Int_pa_PrintArg_struct _T2D=_T22;void*_T2E[1];_T2E[0]=& _T2D;_T25=Cyc_stderr;_T26=
# 783
_tag_fat("Error in .cys file: expected symbol, found '%c' instead\n",sizeof(char),57U);_T27=_tag_fat(_T2E,sizeof(void*),1);Cyc_fprintf(_T25,_T26,_T27);}
# 785
return 0;default: _T28=lexbuf;_T29=_T28->refill_buff;
_T29(lexbuf);_T2A=
Cyc_snarfsymbols_rec(lexbuf,lexstate);return _T2A;}{struct Cyc_Lexing_Error_exn_struct*_T2D=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T2D->tag=Cyc_Lexing_Error;
# 789
_T2D->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T2B=(struct Cyc_Lexing_Error_exn_struct*)_T2D;}_T2C=(void*)_T2B;_throw(_T2C);}
# 791
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_snarfsymbols_rec(lexbuf,15);return _T0;}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_Buffer_t*_T4;struct Cyc_Buffer_t*_T5;struct Cyc_Buffer_t*_T6;int _T7;struct Cyc_Buffer_t*_T8;struct _fat_ptr _T9;int _TA;struct Cyc_Buffer_t*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;int _TE;int _TF;struct Cyc_Buffer_t*_T10;struct _fat_ptr*_T11;struct _fat_ptr*_T12;struct _fat_ptr _T13;struct Cyc_Buffer_t*_T14;struct _fat_ptr _T15;struct Cyc_Buffer_t*_T16;char _T17;struct Cyc_Lexing_lexbuf*_T18;void(*_T19)(struct Cyc_Lexing_lexbuf*);int _T1A;struct Cyc_Lexing_Error_exn_struct*_T1B;void*_T1C;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0: _T2=
# 795 "buildlib.cyl"
_tag_fat("Warning: unclosed brace\n",sizeof(char),25U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T2,_T3);return 0;case 1:
# 797
 if(Cyc_braces_to_match!=1)goto _TL79;return 0;_TL79:
 Cyc_braces_to_match=Cyc_braces_to_match + -1;_T4=
_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_T4,'}');
return 1;case 2:
# 802
 Cyc_braces_to_match=Cyc_braces_to_match + 1;_T5=
_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_T5,'{');
return 1;case 3: _T6=
# 806
_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_T6,'"');
_TL7B: _T7=Cyc_block_string(lexbuf);if(_T7)goto _TL7C;else{goto _TL7D;}_TL7C: goto _TL7B;_TL7D:
 return 1;case 4: _T8=
# 810
_check_null(Cyc_specbuf);_T9=_tag_fat("/*",sizeof(char),3U);Cyc_Buffer_add_string(_T8,_T9);
_TL7E: _TA=Cyc_block_comment(lexbuf);if(_TA)goto _TL7F;else{goto _TL80;}_TL7F: goto _TL7E;_TL80:
 return 1;case 5: _TB=
# 814
_check_null(Cyc_specbuf);_TC=Cyc_Lexing_lexeme(lexbuf);Cyc_Buffer_add_string(_TB,_TC);
return 1;case 6: _TD=
# 817
Cyc_Lexing_lexeme(lexbuf);{struct _fat_ptr symbol=_TD;_TE=Cyc_rename_current_symbol;
if(!_TE)goto _TL81;_TF=Cyc_strcmp(symbol,Cyc_current_symbol);if(_TF)goto _TL81;else{goto _TL83;}
_TL83: _T10=_check_null(Cyc_specbuf);{struct _fat_ptr*_T1D=_cycalloc(sizeof(struct _fat_ptr));*_T1D=symbol;_T11=(struct _fat_ptr*)_T1D;}_T12=Cyc_add_user_prefix(_T11);_T13=*_T12;Cyc_Buffer_add_string(_T10,_T13);goto _TL82;
# 821
_TL81: _T14=_check_null(Cyc_specbuf);_T15=symbol;Cyc_Buffer_add_string(_T14,_T15);_TL82:
 return 1;}case 7: _T16=
# 824
_check_null(Cyc_specbuf);_T17=Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_Buffer_add_char(_T16,_T17);
return 1;default: _T18=lexbuf;_T19=_T18->refill_buff;
_T19(lexbuf);_T1A=
Cyc_block_rec(lexbuf,lexstate);return _T1A;}{struct Cyc_Lexing_Error_exn_struct*_T1D=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T1D->tag=Cyc_Lexing_Error;
# 829
_T1D->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T1B=(struct Cyc_Lexing_Error_exn_struct*)_T1D;}_T1C=(void*)_T1B;_throw(_T1C);}
# 831
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_block_rec(lexbuf,16);return _T0;}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_Buffer_t*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_Buffer_t*_T7;struct _fat_ptr _T8;struct Cyc_Buffer_t*_T9;struct _fat_ptr _TA;struct Cyc_Buffer_t*_TB;struct _fat_ptr _TC;struct Cyc_Buffer_t*_TD;struct _fat_ptr _TE;struct Cyc_Buffer_t*_TF;struct _fat_ptr _T10;struct Cyc_Buffer_t*_T11;struct _fat_ptr _T12;struct Cyc_Buffer_t*_T13;struct _fat_ptr _T14;struct Cyc_Lexing_lexbuf*_T15;void(*_T16)(struct Cyc_Lexing_lexbuf*);int _T17;struct Cyc_Lexing_Error_exn_struct*_T18;void*_T19;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0: _T2=
# 829 "buildlib.cyl"
_tag_fat("Warning: unclosed string\n",sizeof(char),26U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T2,_T3);return 0;case 1: _T4=
# 831
_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_T4,'"');return 0;case 2: _T5=
# 833
_tag_fat("Warning: unclosed string\n",sizeof(char),26U);_T6=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T5,_T6);_T7=
_check_null(Cyc_specbuf);_T8=Cyc_Lexing_lexeme(lexbuf);Cyc_Buffer_add_string(_T7,_T8);
return 1;case 3: _T9=
# 837
_check_null(Cyc_specbuf);_TA=Cyc_Lexing_lexeme(lexbuf);Cyc_Buffer_add_string(_T9,_TA);
return 1;case 4: _TB=
# 840
_check_null(Cyc_specbuf);_TC=Cyc_Lexing_lexeme(lexbuf);Cyc_Buffer_add_string(_TB,_TC);
return 1;case 5: _TD=
# 843
_check_null(Cyc_specbuf);_TE=Cyc_Lexing_lexeme(lexbuf);Cyc_Buffer_add_string(_TD,_TE);
return 1;case 6: _TF=
# 846
_check_null(Cyc_specbuf);_T10=Cyc_Lexing_lexeme(lexbuf);Cyc_Buffer_add_string(_TF,_T10);
return 1;case 7: _T11=
# 849
_check_null(Cyc_specbuf);_T12=Cyc_Lexing_lexeme(lexbuf);Cyc_Buffer_add_string(_T11,_T12);
return 1;case 8: _T13=
# 852
_check_null(Cyc_specbuf);_T14=Cyc_Lexing_lexeme(lexbuf);Cyc_Buffer_add_string(_T13,_T14);
return 1;default: _T15=lexbuf;_T16=_T15->refill_buff;
_T16(lexbuf);_T17=
Cyc_block_string_rec(lexbuf,lexstate);return _T17;}{struct Cyc_Lexing_Error_exn_struct*_T1A=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T1A->tag=Cyc_Lexing_Error;
# 857
_T1A->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T18=(struct Cyc_Lexing_Error_exn_struct*)_T1A;}_T19=(void*)_T18;_throw(_T19);}
# 859
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_block_string_rec(lexbuf,17);return _T0;}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_Buffer_t*_T4;struct _fat_ptr _T5;struct Cyc_Buffer_t*_T6;struct _fat_ptr _T7;struct Cyc_Lexing_lexbuf*_T8;void(*_T9)(struct Cyc_Lexing_lexbuf*);int _TA;struct Cyc_Lexing_Error_exn_struct*_TB;void*_TC;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0: _T2=
# 857 "buildlib.cyl"
_tag_fat("Warning: unclosed comment\n",sizeof(char),27U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T2,_T3);return 0;case 1: _T4=
# 859
_check_null(Cyc_specbuf);_T5=_tag_fat("*/",sizeof(char),3U);Cyc_Buffer_add_string(_T4,_T5);return 0;case 2: _T6=
# 861
_check_null(Cyc_specbuf);_T7=Cyc_Lexing_lexeme(lexbuf);Cyc_Buffer_add_string(_T6,_T7);
return 1;default: _T8=lexbuf;_T9=_T8->refill_buff;
_T9(lexbuf);_TA=
Cyc_block_comment_rec(lexbuf,lexstate);return _TA;}{struct Cyc_Lexing_Error_exn_struct*_TD=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_TD->tag=Cyc_Lexing_Error;
# 866
_TD->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_TB=(struct Cyc_Lexing_Error_exn_struct*)_TD;}_TC=(void*)_TB;_throw(_TC);}
# 868
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){int _T0;_T0=Cyc_block_comment_rec(lexbuf,18);return _T0;}
# 870 "buildlib.cyl"
void Cyc_scan_type(void*,struct Cyc_Hashtable_Table*);struct _tuple18{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;void*_T3;struct _tuple1*_T4;struct _tuple1 _T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Hashtable_Table*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_Hashtable_Table*_TE;struct Cyc_List_List*_TF;void*_T10;struct Cyc_Absyn_MallocInfo _T11;struct Cyc_Absyn_MallocInfo _T12;struct Cyc_Absyn_MallocInfo _T13;struct Cyc_Absyn_MallocInfo _T14;struct Cyc_Absyn_MallocInfo _T15;void**_T16;void*_T17;struct Cyc_Hashtable_Table*_T18;void*_T19;void*_T1A;void*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;void*_T1E;struct Cyc_List_List*_T1F;struct Cyc___cycFILE*_T20;struct _fat_ptr _T21;struct _fat_ptr _T22;struct Cyc___cycFILE*_T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc___cycFILE*_T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc___cycFILE*_T29;struct _fat_ptr _T2A;struct _fat_ptr _T2B;struct Cyc___cycFILE*_T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;struct Cyc___cycFILE*_T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc___cycFILE*_T32;struct _fat_ptr _T33;struct _fat_ptr _T34;struct Cyc___cycFILE*_T35;struct _fat_ptr _T36;struct _fat_ptr _T37;struct Cyc___cycFILE*_T38;struct _fat_ptr _T39;struct _fat_ptr _T3A;struct Cyc___cycFILE*_T3B;struct _fat_ptr _T3C;struct _fat_ptr _T3D;struct Cyc___cycFILE*_T3E;struct _fat_ptr _T3F;struct _fat_ptr _T40;struct Cyc___cycFILE*_T41;struct _fat_ptr _T42;struct _fat_ptr _T43;struct Cyc___cycFILE*_T44;struct _fat_ptr _T45;struct _fat_ptr _T46;struct Cyc___cycFILE*_T47;struct _fat_ptr _T48;struct _fat_ptr _T49;struct Cyc___cycFILE*_T4A;struct _fat_ptr _T4B;struct _fat_ptr _T4C;struct Cyc___cycFILE*_T4D;struct _fat_ptr _T4E;struct _fat_ptr _T4F;struct Cyc___cycFILE*_T50;struct _fat_ptr _T51;struct _fat_ptr _T52;struct Cyc___cycFILE*_T53;struct _fat_ptr _T54;struct _fat_ptr _T55;_T0=
_check_null(e);{void*_T56=_T0->r;struct _fat_ptr*_T57;void**_T58;enum Cyc_Absyn_MallocKind _T59;struct Cyc_List_List*_T5A;struct Cyc_Absyn_Exp*_T5B;struct Cyc_Absyn_Exp*_T5C;void*_T5D;_T1=(int*)_T56;_T2=*_T1;switch(_T2){case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T56;_T3=_T5E->f1;_T5D=(void*)_T3;}{void*b=_T5D;_T4=
# 874
Cyc_Absyn_binding2qvar(b);_T5=*_T4;{struct _fat_ptr*v=_T5.f1;
Cyc_add_target(v);
return;}}case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f2;}{struct Cyc_List_List*x=_T5D;
# 878
_TL8A: if(x!=0)goto _TL88;else{goto _TL89;}
_TL88: _T6=x;_T7=_T6->hd;_T8=(struct Cyc_Absyn_Exp*)_T7;_T9=dep;Cyc_scan_exp(_T8,_T9);_TA=x;
# 878
x=_TA->tl;goto _TL8A;_TL89:
# 881
 return;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;_T5C=_T5E->f2;}{struct Cyc_Absyn_Exp*e1=_T5D;struct Cyc_Absyn_Exp*e2=_T5C;_T5D=e1;_T5C=e2;goto _LL8;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;_T5C=_T5E->f2;}_LL8: {struct Cyc_Absyn_Exp*e1=_T5D;struct Cyc_Absyn_Exp*e2=_T5C;_T5D=e1;_T5C=e2;goto _LLA;}case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;_T5C=_T5E->f3;}_LLA: {struct Cyc_Absyn_Exp*e1=_T5D;struct Cyc_Absyn_Exp*e2=_T5C;
# 887
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;}{struct Cyc_Absyn_Exp*e1=_T5D;_T5D=e1;goto _LLE;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;}_LLE: {struct Cyc_Absyn_Exp*e1=_T5D;_T5D=e1;goto _LL10;}case 18:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;}_LL10: {struct Cyc_Absyn_Exp*e1=_T5D;_T5D=e1;goto _LL12;}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;}_LL12: {struct Cyc_Absyn_Exp*e1=_T5D;_T5D=e1;goto _LL14;}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;}_LL14: {struct Cyc_Absyn_Exp*e1=_T5D;
# 898
Cyc_scan_exp(e1,dep);
return;}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;_T5C=_T5E->f2;_T5B=_T5E->f3;}{struct Cyc_Absyn_Exp*e1=_T5D;struct Cyc_Absyn_Exp*e2=_T5C;struct Cyc_Absyn_Exp*e3=_T5B;
# 901
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
Cyc_scan_exp(e3,dep);
return;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;_T5C=_T5E->f2;}{struct Cyc_Absyn_Exp*e1=_T5D;struct Cyc_Absyn_Exp*e2=_T5C;_T5D=e1;_T5C=e2;goto _LL1A;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;_T5C=_T5E->f2;}_LL1A: {struct Cyc_Absyn_Exp*e1=_T5D;struct Cyc_Absyn_Exp*e2=_T5C;
# 907
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;_T5A=_T5E->f2;}{struct Cyc_Absyn_Exp*e1=_T5D;struct Cyc_List_List*x=_T5A;
# 911
Cyc_scan_exp(e1,dep);
_TL8E: if(x!=0)goto _TL8C;else{goto _TL8D;}
_TL8C: _TB=x;_TC=_TB->hd;_TD=(struct Cyc_Absyn_Exp*)_TC;_TE=dep;Cyc_scan_exp(_TD,_TE);_TF=x;
# 912
x=_TF->tl;goto _TL8E;_TL8D:
# 915
 return;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T56;_T10=_T5E->f1;_T5D=(void*)_T10;_T5C=_T5E->f2;}{void*t1=_T5D;struct Cyc_Absyn_Exp*e1=_T5C;
# 917
Cyc_scan_type(t1,dep);
Cyc_scan_exp(e1,dep);
return;}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T56;_T11=_T5E->f1;_T59=_T11.mknd;_T12=_T5E->f1;_T5D=_T12.rgn;_T13=_T5E->f1;_T5C=_T13.aqual;_T14=_T5E->f1;_T58=_T14.elt_type;_T15=_T5E->f1;_T5B=_T15.num_elts;}{enum Cyc_Absyn_MallocKind mknd=_T59;struct Cyc_Absyn_Exp*ropt=_T5D;struct Cyc_Absyn_Exp*aqopt=_T5C;void**topt=_T58;struct Cyc_Absyn_Exp*e=_T5B;
# 921
if(ropt==0)goto _TL8F;Cyc_scan_exp(ropt,dep);goto _TL90;_TL8F: _TL90:
 if(aqopt==0)goto _TL91;Cyc_scan_exp(aqopt,dep);goto _TL92;_TL91: _TL92:
 if(topt==0)goto _TL93;_T16=topt;_T17=*_T16;_T18=dep;Cyc_scan_type(_T17,_T18);goto _TL94;_TL93: _TL94:
 Cyc_scan_exp(e,dep);
return;}case 37:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;}{struct Cyc_Absyn_Exp*e=_T5D;
# 927
Cyc_scan_exp(e,dep);return;}case 38:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_T56;_T19=_T5E->f1;_T5D=(void*)_T19;}{void*t1=_T5D;_T5D=t1;goto _LL26;}case 17:{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T56;_T1A=_T5E->f1;_T5D=(void*)_T1A;}_LL26: {void*t1=_T5D;
# 930
Cyc_scan_type(t1,dep);
return;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;_T57=_T5E->f2;}{struct Cyc_Absyn_Exp*e1=_T5D;struct _fat_ptr*fn=_T57;_T5D=e1;_T57=fn;goto _LL2A;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T56;_T5D=_T5E->f1;_T57=_T5E->f2;}_LL2A: {struct Cyc_Absyn_Exp*e1=_T5D;struct _fat_ptr*fn=_T57;
# 935
Cyc_scan_exp(e1,dep);
Cyc_add_target(fn);
return;}case 19:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T56;_T1B=_T5E->f1;_T5D=(void*)_T1B;_T5A=_T5E->f2;}{void*t1=_T5D;struct Cyc_List_List*f=_T5A;
# 939
Cyc_scan_type(t1,dep);_T1C=
# 941
_check_null(f);{void*_T5E=_T1C->hd;struct _fat_ptr*_T5F;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T60=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T5E;_T5F=_T60->f1;}{struct _fat_ptr*fn=_T5F;
Cyc_add_target(fn);goto _LL57;}_LL57:;}
# 944
return;}case 0:
# 946
 return;case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T5E=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T56;_T5A=_T5E->f2;}{struct Cyc_List_List*x=_T5A;
# 948
_TL98: if(x!=0)goto _TL96;else{goto _TL97;}
_TL96: _T1D=x;_T1E=_T1D->hd;{struct _tuple18*_T5E=(struct _tuple18*)_T1E;struct Cyc_Absyn_Exp*_T5F;{struct _tuple18 _T60=*_T5E;_T5F=_T60.f1;}{struct Cyc_Absyn_Exp*e1=_T5F;
Cyc_scan_exp(e1,dep);}}_T1F=x;
# 948
x=_T1F->tl;goto _TL98;_TL97:
# 952
 return;}case 39:
 return;case 2: _T20=Cyc_stderr;_T21=
# 955
_tag_fat("Error: unexpected Pragma_e\n",sizeof(char),28U);_T22=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T20,_T21,_T22);
exit(1);return;case 34: _T23=Cyc_stderr;_T24=
# 958
_tag_fat("Error: unexpected Swap_e\n",sizeof(char),26U);_T25=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T23,_T24,_T25);
exit(1);return;case 36: _T26=Cyc_stderr;_T27=
# 961
_tag_fat("Error: unexpected Stmt_e\n",sizeof(char),26U);_T28=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T26,_T27,_T28);
exit(1);return;case 41: _T29=Cyc_stderr;_T2A=
# 964
_tag_fat("Error: unexpected Assert_e\n",sizeof(char),28U);_T2B=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T29,_T2A,_T2B);
exit(1);return;case 42: _T2C=Cyc_stderr;_T2D=
# 967
_tag_fat("Error: unexpected Assert_false_e\n",sizeof(char),34U);_T2E=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2C,_T2D,_T2E);
exit(1);return;case 11: _T2F=Cyc_stderr;_T30=
# 970
_tag_fat("Error: unexpected Throw_e\n",sizeof(char),27U);_T31=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2F,_T30,_T31);
exit(1);return;case 12: _T32=Cyc_stderr;_T33=
# 973
_tag_fat("Error: unexpected NoInstantiate_e\n",sizeof(char),35U);_T34=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T32,_T33,_T34);
exit(1);return;case 13: _T35=Cyc_stderr;_T36=
# 976
_tag_fat("Error: unexpected Instantiate_e\n",sizeof(char),33U);_T37=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T35,_T36,_T37);
exit(1);return;case 16: _T38=Cyc_stderr;_T39=
# 979
_tag_fat("Error: unexpected New_e\n",sizeof(char),25U);_T3A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T38,_T39,_T3A);
exit(1);return;case 24: _T3B=Cyc_stderr;_T3C=
# 982
_tag_fat("Error: unexpected CompoundLit_e\n",sizeof(char),33U);_T3D=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T3B,_T3C,_T3D);
exit(1);return;case 25: _T3E=Cyc_stderr;_T3F=
# 985
_tag_fat("Error: unexpected Array_e\n",sizeof(char),27U);_T40=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T3E,_T3F,_T40);
exit(1);return;case 26: _T41=Cyc_stderr;_T42=
# 988
_tag_fat("Error: unexpected Comprehension_e\n",sizeof(char),35U);_T43=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T41,_T42,_T43);
exit(1);return;case 27: _T44=Cyc_stderr;_T45=
# 991
_tag_fat("Error: unexpected ComprehensionNoinit_e\n",sizeof(char),41U);_T46=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T44,_T45,_T46);
exit(1);return;case 28: _T47=Cyc_stderr;_T48=
# 994
_tag_fat("Error: unexpected Aggregate_e\n",sizeof(char),31U);_T49=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T47,_T48,_T49);
exit(1);return;case 29: _T4A=Cyc_stderr;_T4B=
# 997
_tag_fat("Error: unexpected AnonStruct_e\n",sizeof(char),32U);_T4C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4A,_T4B,_T4C);
exit(1);return;case 30: _T4D=Cyc_stderr;_T4E=
# 1000
_tag_fat("Error: unexpected Datatype_e\n",sizeof(char),30U);_T4F=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4D,_T4E,_T4F);
exit(1);return;case 31: _T50=Cyc_stderr;_T51=
# 1003
_tag_fat("Error: unexpected Enum_e\n",sizeof(char),26U);_T52=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T50,_T51,_T52);
exit(1);return;default: _T53=Cyc_stderr;_T54=
# 1006
_tag_fat("Error: unexpected AnonEnum_e\n",sizeof(char),30U);_T55=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T53,_T54,_T55);
exit(1);return;};}}
# 1011
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){struct Cyc_Absyn_Exp*_T0;unsigned _T1;_T0=eo;_T1=(unsigned)_T0;
if(!_T1)goto _TL99;Cyc_scan_exp(eo,dep);goto _TL9A;_TL99: _TL9A:
 return;}
# 1016
void Cyc_scan_decl(struct Cyc_Absyn_Decl*,struct Cyc_Hashtable_Table*);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;struct _tuple1*_T9;void*_TA;struct _tuple1*_TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc___cycFILE*_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;void*_T10;struct Cyc_Absyn_PtrInfo _T11;void*_T12;struct Cyc_Hashtable_Table*_T13;void*_T14;struct Cyc_Absyn_ArrayInfo _T15;struct Cyc_Absyn_ArrayInfo _T16;struct Cyc_Absyn_ArrayInfo _T17;void*_T18;void*_T19;struct Cyc_Absyn_FnInfo _T1A;void*_T1B;struct Cyc_Hashtable_Table*_T1C;struct Cyc_Absyn_FnInfo _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_List_List*_T20;struct Cyc_Absyn_FnInfo _T21;struct Cyc_Absyn_VarargInfo*_T22;struct Cyc_Absyn_FnInfo _T23;struct Cyc_Absyn_VarargInfo*_T24;void*_T25;struct Cyc_Hashtable_Table*_T26;void*_T27;struct Cyc_List_List*_T28;void*_T29;struct Cyc_Absyn_Aggrfield*_T2A;void*_T2B;struct Cyc_Hashtable_Table*_T2C;struct Cyc_List_List*_T2D;void*_T2E;struct Cyc_Absyn_Aggrfield*_T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_Hashtable_Table*_T31;struct Cyc_List_List*_T32;void*_T33;struct _tuple1*_T34;void*_T35;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T36;struct Cyc_Absyn_TypeDecl*_T37;struct Cyc_Absyn_TypeDecl*_T38;void*_T39;int*_T3A;unsigned _T3B;void*_T3C;struct Cyc_Absyn_TypeDecl*_T3D;void*_T3E;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T3F;void*_T40;struct Cyc_Absyn_Decl*_T41;struct Cyc_Hashtable_Table*_T42;struct Cyc_Absyn_Aggrdecl*_T43;void*_T44;struct Cyc_Absyn_TypeDecl*_T45;void*_T46;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T47;void*_T48;struct Cyc_Absyn_Decl*_T49;struct Cyc_Hashtable_Table*_T4A;struct Cyc_Absyn_Enumdecl*_T4B;void*_T4C;struct Cyc_Absyn_TypeDecl*_T4D;void*_T4E;struct Cyc___cycFILE*_T4F;struct _fat_ptr _T50;struct _fat_ptr _T51;struct Cyc___cycFILE*_T52;struct _fat_ptr _T53;struct _fat_ptr _T54;struct Cyc___cycFILE*_T55;struct _fat_ptr _T56;struct _fat_ptr _T57;struct Cyc___cycFILE*_T58;struct _fat_ptr _T59;struct _fat_ptr _T5A;struct Cyc___cycFILE*_T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;struct Cyc___cycFILE*_T5E;struct _fat_ptr _T5F;struct _fat_ptr _T60;struct Cyc_Absyn_Datatypedecl*_T61;struct Cyc_Absyn_Enumdecl*_T62;struct Cyc_Absyn_Aggrdecl*_T63;struct _fat_ptr*_T64;struct Cyc_List_List*_T65;struct Cyc_Absyn_FnInfo _T66;struct Cyc_Absyn_Exp*_T67;struct Cyc_Absyn_PtrInfo _T68;void*_T69;void*_T6A;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T4=_T6B->f1;_T6A=(void*)_T4;_T69=_T6B->f2;}{void*c=_T6A;struct Cyc_List_List*ts=_T69;struct _fat_ptr*_T6B;union Cyc_Absyn_AggrInfo _T6C;_T5=c;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 0: goto _LL23;case 1: _LL23: goto _LL25;case 21: _LL25: goto _LL27;case 2: _LL27: goto _LL29;case 3: _LL29: goto _LL2B;case 20: _LL2B:
# 1027
 return;case 24: _T8=c;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T6D=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T8;_T6C=_T6D->f1;}{union Cyc_Absyn_AggrInfo info=_T6C;
# 1029
struct _tuple12 _T6D=Cyc_Absyn_aggr_kinded_name(info);struct _fat_ptr*_T6E;_T9=_T6D.f1;{struct _tuple1 _T6F=*_T9;_T6E=_T6F.f1;}{struct _fat_ptr*v=_T6E;_T6B=v;goto _LL2F;}}case 19: _TA=c;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T6D=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_TA;_TB=_T6D->f1;{struct _tuple1 _T6E=*_TB;_T6B=_T6E.f1;}}_LL2F: {struct _fat_ptr*v=_T6B;
# 1031
Cyc_add_target(v);return;}case 22: goto _LL33;case 23: _LL33: goto _LL35;case 4: _LL35: goto _LL37;case 6: _LL37: goto _LL39;case 7: _LL39: goto _LL3B;case 8: _LL3B: goto _LL3D;case 9: _LL3D: goto _LL3F;case 10: _LL3F: goto _LL41;case 5: _LL41: goto _LL43;case 11: _LL43: goto _LL45;case 12: _LL45: goto _LL47;case 13: _LL47: goto _LL49;case 14: _LL49: goto _LL4B;case 15: _LL4B: goto _LL4D;case 16: _LL4D: goto _LL4F;case 18: _LL4F: goto _LL51;default: _LL51:{struct Cyc_String_pa_PrintArg_struct _T6D;_T6D.tag=0;
# 1041
_T6D.f1=Cyc_Absynpp_typ2string(t);_TC=_T6D;}{struct Cyc_String_pa_PrintArg_struct _T6D=_TC;void*_T6E[1];_T6E[0]=& _T6D;_TD=Cyc_stderr;_TE=_tag_fat("Error: unexpected %s\n",sizeof(char),22U);_TF=_tag_fat(_T6E,sizeof(void*),1);Cyc_fprintf(_TD,_TE,_TF);}
exit(1);return;};}case 4: _T10=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T10;_T68=_T6B->f1;}{struct Cyc_Absyn_PtrInfo x=_T68;_T11=x;_T12=_T11.elt_type;_T13=dep;
# 1046
Cyc_scan_type(_T12,_T13);
return;}case 5: _T14=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T14;_T15=_T6B->f1;_T6A=_T15.elt_type;_T16=_T6B->f1;_T67=_T16.num_elts;_T17=_T6B->f1;_T69=_T17.zero_term;}{void*t=_T6A;struct Cyc_Absyn_Exp*sz=_T67;void*zt=_T69;
# 1049
Cyc_scan_type(t,dep);
Cyc_scan_exp_opt(sz,dep);
return;}case 11: _T18=t;{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_T18;_T67=_T6B->f1;}{struct Cyc_Absyn_Exp*e=_T67;
# 1053
Cyc_scan_exp(e,dep);
return;}case 6: _T19=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T19;_T66=_T6B->f1;}{struct Cyc_Absyn_FnInfo x=_T66;_T1A=x;_T1B=_T1A.ret_type;_T1C=dep;
# 1056
Cyc_scan_type(_T1B,_T1C);_T1D=x;{
struct Cyc_List_List*a=_T1D.args;_TLA0: if(a!=0)goto _TL9E;else{goto _TL9F;}
_TL9E: _T1E=a;_T1F=_T1E->hd;{struct _tuple9*_T6B=(struct _tuple9*)_T1F;void*_T6C;{struct _tuple9 _T6D=*_T6B;_T6C=_T6D.f2;}{void*argt=_T6C;
Cyc_scan_type(argt,dep);}}_T20=a;
# 1057
a=_T20->tl;goto _TLA0;_TL9F:;}_T21=x;_T22=_T21.cyc_varargs;
# 1061
if(_T22==0)goto _TLA1;_T23=x;_T24=_T23.cyc_varargs;_T25=_T24->type;_T26=dep;
Cyc_scan_type(_T25,_T26);goto _TLA2;_TLA1: _TLA2:
 return;}case 7: _T27=t;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T27;_T65=_T6B->f3;}{struct Cyc_List_List*sfs=_T65;
# 1065
_TLA6: if(sfs!=0)goto _TLA4;else{goto _TLA5;}
_TLA4: _T28=sfs;_T29=_T28->hd;_T2A=(struct Cyc_Absyn_Aggrfield*)_T29;_T2B=_T2A->type;_T2C=dep;Cyc_scan_type(_T2B,_T2C);_T2D=sfs;_T2E=_T2D->hd;_T2F=(struct Cyc_Absyn_Aggrfield*)_T2E;_T30=_T2F->width;_T31=dep;
Cyc_scan_exp_opt(_T30,_T31);_T32=sfs;
# 1065
sfs=_T32->tl;goto _TLA6;_TLA5:
# 1069
 return;}case 8: _T33=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T33;_T34=_T6B->f1;{struct _tuple1 _T6C=*_T34;_T64=_T6C.f1;}}{struct _fat_ptr*v=_T64;
# 1071
Cyc_add_target(v);
return;}case 10: _T35=t;_T36=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T35;_T37=_T36->f1;_T38=(struct Cyc_Absyn_TypeDecl*)_T37;_T39=_T38->r;_T3A=(int*)_T39;_T3B=*_T3A;switch(_T3B){case 0: _T3C=t;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T3C;_T3D=_T6B->f1;{struct Cyc_Absyn_TypeDecl _T6C=*_T3D;_T3E=_T6C.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_T6D=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T3E;_T63=_T6D->f1;}}}{struct Cyc_Absyn_Aggrdecl*x=_T63;{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T6B=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T6B->tag=5;
# 1075
_T6B->f1=x;_T3F=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T6B;}_T40=(void*)_T3F;_T41=Cyc_Absyn_new_decl(_T40,0U);_T42=dep;Cyc_scan_decl(_T41,_T42);_T43=x;{
struct _tuple1*_T6B=_T43->name;struct _fat_ptr*_T6C;{struct _tuple1 _T6D=*_T6B;_T6C=_T6D.f1;}{struct _fat_ptr*n=_T6C;
Cyc_add_target(n);
return;}}}case 1: _T44=t;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T44;_T45=_T6B->f1;{struct Cyc_Absyn_TypeDecl _T6C=*_T45;_T46=_T6C.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T6D=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T46;_T62=_T6D->f1;}}}{struct Cyc_Absyn_Enumdecl*x=_T62;{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T6B=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_T6B->tag=7;
# 1081
_T6B->f1=x;_T47=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T6B;}_T48=(void*)_T47;_T49=Cyc_Absyn_new_decl(_T48,0U);_T4A=dep;Cyc_scan_decl(_T49,_T4A);_T4B=x;{
struct _tuple1*_T6B=_T4B->name;struct _fat_ptr*_T6C;{struct _tuple1 _T6D=*_T6B;_T6C=_T6D.f1;}{struct _fat_ptr*n=_T6C;
Cyc_add_target(n);
return;}}}default: _T4C=t;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T4C;_T4D=_T6B->f1;{struct Cyc_Absyn_TypeDecl _T6C=*_T4D;_T4E=_T6C.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_T6D=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T4E;_T61=_T6D->f1;}}}{struct Cyc_Absyn_Datatypedecl*dd=_T61;_T4F=Cyc_stderr;_T50=
# 1087
_tag_fat("Error: unexpected Datatype declaration\n",sizeof(char),40U);_T51=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4F,_T50,_T51);
exit(1);return;}};case 12: _T52=Cyc_stderr;_T53=
# 1090
_tag_fat("Error: unexpected @subset type\n",sizeof(char),32U);_T54=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T52,_T53,_T54);
exit(1);return;case 3: _T55=Cyc_stderr;_T56=
# 1093
_tag_fat("Error: unexpected Cvar\n",sizeof(char),24U);_T57=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T55,_T56,_T57);
exit(1);return;case 1: _T58=Cyc_stderr;_T59=
# 1096
_tag_fat("Error: unexpected Evar\n",sizeof(char),24U);_T5A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T58,_T59,_T5A);
exit(1);return;case 2: _T5B=Cyc_stderr;_T5C=
# 1099
_tag_fat("Error: unexpected VarType\n",sizeof(char),27U);_T5D=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T5B,_T5C,_T5D);
exit(1);return;default: _T5E=Cyc_stderr;_T5F=
# 1102
_tag_fat("Error: unexpected valueof_t\n",sizeof(char),29U);_T60=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T5E,_T5F,_T60);
exit(1);return;};}
# 1107
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){struct Cyc_Set_Set**_T0;struct Cyc_Set_Set*(*_T1)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Set_Set*(*_T2)(int(*)(void*,void*));int(*_T3)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct Cyc_Absyn_Vardecl*_T7;struct Cyc_Absyn_Vardecl*_T8;void*_T9;struct Cyc_Hashtable_Table*_TA;struct Cyc_Absyn_Vardecl*_TB;struct Cyc_Absyn_Exp*_TC;struct Cyc_Hashtable_Table*_TD;struct Cyc_Absyn_Fndecl*_TE;struct Cyc_Absyn_Fndecl*_TF;struct Cyc_Absyn_FnInfo _T10;void*_T11;struct Cyc_Hashtable_Table*_T12;struct Cyc_Absyn_Fndecl*_T13;struct Cyc_Absyn_FnInfo _T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_List_List*_T17;struct Cyc_Absyn_Fndecl*_T18;struct Cyc_Absyn_FnInfo _T19;struct Cyc_Absyn_VarargInfo*_T1A;struct Cyc_Absyn_Fndecl*_T1B;struct Cyc_Absyn_FnInfo _T1C;struct Cyc_Absyn_VarargInfo*_T1D;void*_T1E;struct Cyc_Hashtable_Table*_T1F;struct Cyc_Absyn_Fndecl*_T20;int _T21;struct Cyc_String_pa_PrintArg_struct _T22;struct _fat_ptr*_T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc_Absyn_Aggrdecl*_T26;struct Cyc_Absyn_Aggrdecl*_T27;struct Cyc_Absyn_AggrdeclImpl*_T28;unsigned _T29;struct Cyc_Absyn_Aggrdecl*_T2A;struct Cyc_Absyn_AggrdeclImpl*_T2B;struct Cyc_List_List*_T2C;void*_T2D;struct Cyc_Absyn_Aggrfield*_T2E;void*_T2F;struct Cyc_Hashtable_Table*_T30;struct Cyc_Absyn_Aggrfield*_T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_Hashtable_Table*_T33;struct Cyc_List_List*_T34;struct Cyc_Absyn_Aggrdecl*_T35;struct Cyc_Absyn_AggrdeclImpl*_T36;struct Cyc_Absyn_AggrdeclImpl*_T37;struct Cyc_List_List*_T38;struct Cyc_Absyn_Enumdecl*_T39;struct Cyc_Absyn_Enumdecl*_T3A;struct Cyc_Core_Opt*_T3B;unsigned _T3C;struct Cyc_Absyn_Enumdecl*_T3D;struct Cyc_Core_Opt*_T3E;void*_T3F;struct Cyc_List_List*_T40;void*_T41;struct Cyc_Absyn_Enumfield*_T42;struct Cyc_Absyn_Exp*_T43;struct Cyc_Hashtable_Table*_T44;struct Cyc_List_List*_T45;struct Cyc_Absyn_Enumdecl*_T46;struct Cyc_Core_Opt*_T47;struct Cyc_Core_Opt*_T48;void*_T49;struct Cyc_List_List*_T4A;struct Cyc_Absyn_Typedefdecl*_T4B;struct Cyc_Absyn_Typedefdecl*_T4C;void*_T4D;unsigned _T4E;struct Cyc_Absyn_Typedefdecl*_T4F;void*_T50;struct Cyc_Hashtable_Table*_T51;struct Cyc___cycFILE*_T52;struct _fat_ptr _T53;struct _fat_ptr _T54;struct Cyc___cycFILE*_T55;struct _fat_ptr _T56;struct _fat_ptr _T57;struct Cyc___cycFILE*_T58;struct _fat_ptr _T59;struct _fat_ptr _T5A;struct Cyc___cycFILE*_T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;struct Cyc___cycFILE*_T5E;struct _fat_ptr _T5F;struct _fat_ptr _T60;struct Cyc___cycFILE*_T61;struct _fat_ptr _T62;struct _fat_ptr _T63;struct Cyc___cycFILE*_T64;struct _fat_ptr _T65;struct _fat_ptr _T66;struct Cyc___cycFILE*_T67;struct _fat_ptr _T68;struct _fat_ptr _T69;struct Cyc___cycFILE*_T6A;struct _fat_ptr _T6B;struct _fat_ptr _T6C;struct Cyc___cycFILE*_T6D;struct _fat_ptr _T6E;struct _fat_ptr _T6F;struct Cyc___cycFILE*_T70;struct _fat_ptr _T71;struct _fat_ptr _T72;struct Cyc___cycFILE*_T73;struct _fat_ptr _T74;struct _fat_ptr _T75;struct _handler_cons*_T76;int _T77;struct Cyc_Set_Set*(*_T78)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_T79)(struct Cyc_Hashtable_Table*,void*);void*_T7A;struct Cyc_Core_Not_found_exn_struct*_T7B;char*_T7C;char*_T7D;struct Cyc_Set_Set*(*_T7E)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Set_Set*(*_T7F)(int(*)(void*,void*));int(*_T80)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Set_Set**_T81;struct Cyc_Set_Set*_T82;struct Cyc_Set_Set*_T83;void(*_T84)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*);void(*_T85)(struct Cyc_Hashtable_Table*,void*,void*);
struct Cyc_Set_Set**saved_targets=Cyc_current_targets;
struct _fat_ptr*saved_source=Cyc_current_source;{struct Cyc_Set_Set**_T86=_cycalloc(sizeof(struct Cyc_Set_Set*));_T2=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T87)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T2;_T1=_T87;}_T3=Cyc_strptrcmp;*_T86=_T1(_T3);_T0=(struct Cyc_Set_Set**)_T86;}Cyc_current_targets=_T0;_T4=d;{
void*_T86=_T4->r;struct Cyc_Absyn_Typedefdecl*_T87;struct Cyc_Absyn_Enumdecl*_T88;struct Cyc_Absyn_Aggrdecl*_T89;struct Cyc_Absyn_Fndecl*_T8A;struct Cyc_Absyn_Vardecl*_T8B;_T5=(int*)_T86;_T6=*_T5;switch(_T6){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T8C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T86;_T8B=_T8C->f1;}{struct Cyc_Absyn_Vardecl*x=_T8B;_T7=x;{
# 1113
struct _tuple1*_T8C=_T7->name;struct _fat_ptr*_T8D;{struct _tuple1 _T8E=*_T8C;_T8D=_T8E.f1;}{struct _fat_ptr*v=_T8D;
Cyc_current_source=v;_T8=x;_T9=_T8->type;_TA=dep;
Cyc_scan_type(_T9,_TA);_TB=x;_TC=_TB->initializer;_TD=dep;
Cyc_scan_exp_opt(_TC,_TD);goto _LL0;}}}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T8C=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T86;_T8A=_T8C->f1;}{struct Cyc_Absyn_Fndecl*x=_T8A;_TE=x;{
# 1119
struct _tuple1*_T8C=_TE->name;struct _fat_ptr*_T8D;{struct _tuple1 _T8E=*_T8C;_T8D=_T8E.f1;}{struct _fat_ptr*v=_T8D;
Cyc_current_source=v;_TF=x;_T10=_TF->i;_T11=_T10.ret_type;_T12=dep;
Cyc_scan_type(_T11,_T12);_T13=x;_T14=_T13->i;{
struct Cyc_List_List*a=_T14.args;_TLAC: if(a!=0)goto _TLAA;else{goto _TLAB;}
_TLAA: _T15=a;_T16=_T15->hd;{struct _tuple9*_T8E=(struct _tuple9*)_T16;void*_T8F;{struct _tuple9 _T90=*_T8E;_T8F=_T90.f2;}{void*t1=_T8F;
Cyc_scan_type(t1,dep);}}_T17=a;
# 1122
a=_T17->tl;goto _TLAC;_TLAB:;}_T18=x;_T19=_T18->i;_T1A=_T19.cyc_varargs;
# 1126
if(_T1A==0)goto _TLAD;_T1B=x;_T1C=_T1B->i;_T1D=_T1C.cyc_varargs;_T1E=_T1D->type;_T1F=dep;
Cyc_scan_type(_T1E,_T1F);goto _TLAE;_TLAD: _TLAE: _T20=x;_T21=_T20->is_inline;
if(!_T21)goto _TLAF;{struct Cyc_String_pa_PrintArg_struct _T8E;_T8E.tag=0;_T23=v;
_T8E.f1=*_T23;_T22=_T8E;}{struct Cyc_String_pa_PrintArg_struct _T8E=_T22;void*_T8F[1];_T8F[0]=& _T8E;_T24=_tag_fat("Warning: ignoring inline function %s\n",sizeof(char),38U);_T25=_tag_fat(_T8F,sizeof(void*),1);Cyc_log(_T24,_T25);}goto _TLB0;_TLAF: _TLB0: goto _LL0;}}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T8C=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T86;_T89=_T8C->f1;}{struct Cyc_Absyn_Aggrdecl*x=_T89;_T26=x;{
# 1132
struct _tuple1*_T8C=_T26->name;struct _fat_ptr*_T8D;{struct _tuple1 _T8E=*_T8C;_T8D=_T8E.f1;}{struct _fat_ptr*v=_T8D;
Cyc_current_source=v;_T27=x;_T28=_T27->impl;_T29=(unsigned)_T28;
if(!_T29)goto _TLB1;_T2A=x;_T2B=_T2A->impl;{
struct Cyc_List_List*fs=_T2B->fields;_TLB6: if(fs!=0)goto _TLB4;else{goto _TLB5;}
_TLB4: _T2C=fs;_T2D=_T2C->hd;{struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)_T2D;_T2E=f;_T2F=_T2E->type;_T30=dep;
Cyc_scan_type(_T2F,_T30);_T31=f;_T32=_T31->width;_T33=dep;
Cyc_scan_exp_opt(_T32,_T33);}_T34=fs;
# 1135
fs=_T34->tl;goto _TLB6;_TLB5:;}_T35=x;_T36=_T35->impl;_T37=
# 1142
_check_null(_T36);{struct Cyc_List_List*fs=_T37->fields;_TLBA: if(fs!=0)goto _TLB8;else{goto _TLB9;}_TLB8: _T38=fs;fs=_T38->tl;goto _TLBA;_TLB9:;}goto _TLB2;_TLB1: _TLB2: goto _LL0;}}}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T8C=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T86;_T88=_T8C->f1;}{struct Cyc_Absyn_Enumdecl*x=_T88;_T39=x;{
# 1148
struct _tuple1*_T8C=_T39->name;struct _fat_ptr*_T8D;{struct _tuple1 _T8E=*_T8C;_T8D=_T8E.f1;}{struct _fat_ptr*v=_T8D;
Cyc_current_source=v;_T3A=x;_T3B=_T3A->fields;_T3C=(unsigned)_T3B;
if(!_T3C)goto _TLBB;_T3D=x;_T3E=_T3D->fields;_T3F=_T3E->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T3F;_TLC0: if(fs!=0)goto _TLBE;else{goto _TLBF;}
_TLBE: _T40=fs;_T41=_T40->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_T41;_T42=f;_T43=_T42->tag;_T44=dep;
Cyc_scan_exp_opt(_T43,_T44);}_T45=fs;
# 1151
fs=_T45->tl;goto _TLC0;_TLBF:;}_T46=x;_T47=_T46->fields;_T48=
# 1157
_check_null(_T47);_T49=_T48->v;{struct Cyc_List_List*fs=(struct Cyc_List_List*)_T49;_TLC4: if(fs!=0)goto _TLC2;else{goto _TLC3;}_TLC2: _T4A=fs;fs=_T4A->tl;goto _TLC4;_TLC3:;}goto _TLBC;_TLBB: _TLBC: goto _LL0;}}}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T8C=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T86;_T87=_T8C->f1;}{struct Cyc_Absyn_Typedefdecl*x=_T87;_T4B=x;{
# 1163
struct _tuple1*_T8C=_T4B->name;struct _fat_ptr*_T8D;{struct _tuple1 _T8E=*_T8C;_T8D=_T8E.f1;}{struct _fat_ptr*v=_T8D;
Cyc_current_source=v;_T4C=x;_T4D=_T4C->defn;_T4E=(unsigned)_T4D;
if(!_T4E)goto _TLC5;_T4F=x;_T50=_T4F->defn;_T51=dep;
Cyc_scan_type(_T50,_T51);goto _TLC6;_TLC5: _TLC6: goto _LL0;}}}case 4: _T52=Cyc_stderr;_T53=
# 1169
_tag_fat("Error: unexpected region declaration",sizeof(char),37U);_T54=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T52,_T53,_T54);
exit(1);case 13: _T55=Cyc_stderr;_T56=
# 1172
_tag_fat("Error: unexpected __cyclone_port_on__",sizeof(char),38U);_T57=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T55,_T56,_T57);
exit(1);case 14: _T58=Cyc_stderr;_T59=
# 1175
_tag_fat("Error: unexpected __cyclone_port_off__",sizeof(char),39U);_T5A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T58,_T59,_T5A);
exit(1);case 15: _T5B=Cyc_stderr;_T5C=
# 1178
_tag_fat("Error: unexpected __tempest_on__",sizeof(char),33U);_T5D=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T5B,_T5C,_T5D);
exit(1);case 16: _T5E=Cyc_stderr;_T5F=
# 1181
_tag_fat("Error: unexpected __tempest_off__",sizeof(char),34U);_T60=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T5E,_T5F,_T60);
exit(1);case 2: _T61=Cyc_stderr;_T62=
# 1184
_tag_fat("Error: unexpected let declaration\n",sizeof(char),35U);_T63=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T61,_T62,_T63);
exit(1);case 6: _T64=Cyc_stderr;_T65=
# 1187
_tag_fat("Error: unexpected datatype declaration\n",sizeof(char),40U);_T66=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T64,_T65,_T66);
exit(1);case 3: _T67=Cyc_stderr;_T68=
# 1190
_tag_fat("Error: unexpected let declaration\n",sizeof(char),35U);_T69=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T67,_T68,_T69);
exit(1);case 9: _T6A=Cyc_stderr;_T6B=
# 1193
_tag_fat("Error: unexpected namespace declaration\n",sizeof(char),41U);_T6C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T6A,_T6B,_T6C);
exit(1);case 10: _T6D=Cyc_stderr;_T6E=
# 1196
_tag_fat("Error: unexpected using declaration\n",sizeof(char),37U);_T6F=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T6D,_T6E,_T6F);
exit(1);case 11: _T70=Cyc_stderr;_T71=
# 1199
_tag_fat("Error: unexpected extern \"C\" declaration\n",sizeof(char),42U);_T72=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T70,_T71,_T72);
exit(1);default: _T73=Cyc_stderr;_T74=
# 1202
_tag_fat("Error: unexpected extern \"C include\" declaration\n",sizeof(char),50U);_T75=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T73,_T74,_T75);
exit(1);}_LL0:;}{
# 1210
struct Cyc_Set_Set*old;
struct _fat_ptr*name=_check_null(Cyc_current_source);{struct _handler_cons _T86;_T76=& _T86;_push_handler(_T76);{int _T87=0;_T77=setjmp(_T86.handler);if(!_T77)goto _TLC7;_T87=1;goto _TLC8;_TLC7: _TLC8: if(_T87)goto _TLC9;else{goto _TLCB;}_TLCB: _T79=Cyc_Hashtable_lookup;{
# 1213
struct Cyc_Set_Set*(*_T88)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_T79;_T78=_T88;}old=_T78(dep,name);_pop_handler();goto _TLCA;_TLC9: _T7A=Cyc_Core_get_exn_thrown();{void*_T88=(void*)_T7A;void*_T89;_T7B=(struct Cyc_Core_Not_found_exn_struct*)_T88;_T7C=_T7B->tag;_T7D=Cyc_Core_Not_found;if(_T7C!=_T7D)goto _TLCC;_T7F=Cyc_Set_empty;{
# 1215
struct Cyc_Set_Set*(*_T8A)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T7F;_T7E=_T8A;}_T80=Cyc_strptrcmp;old=_T7E(_T80);goto _LL35;_TLCC: _T89=_T88;{void*exn=_T89;_rethrow(exn);}_LL35:;}_TLCA:;}}_T81=
# 1217
_check_null(Cyc_current_targets);_T82=*_T81;_T83=old;{struct Cyc_Set_Set*targets=Cyc_Set_union_two(_T82,_T83);_T85=Cyc_Hashtable_insert;{
void(*_T86)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))_T85;_T84=_T86;}_T84(dep,name,targets);
# 1220
Cyc_current_targets=saved_targets;
Cyc_current_source=saved_source;}}}
# 1224
struct Cyc_Hashtable_Table*Cyc_new_deps (void){struct Cyc_Hashtable_Table*(*_T0)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T1)(int,int(*)(void*,void*),int(*)(void*));int(*_T2)(struct _fat_ptr*,struct _fat_ptr*);int(*_T3)(struct _fat_ptr*);struct Cyc_Hashtable_Table*_T4;_T1=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T5)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T1;_T0=_T5;}_T2=Cyc_strptrcmp;_T3=Cyc_Hashtable_hash_stringptr;_T4=_T0(107,_T2,_T3);return _T4;}
# 1228
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _fat_ptr*x){struct _handler_cons*_T0;int _T1;struct Cyc_Set_Set*(*_T2)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_T3)(struct Cyc_Hashtable_Table*,void*);void*_T4;struct Cyc_Core_Not_found_exn_struct*_T5;char*_T6;char*_T7;struct Cyc_Set_Set*(*_T8)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Set_Set*(*_T9)(int(*)(void*,void*));int(*_TA)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Set_Set*_TB;struct _handler_cons _TC;_T0=& _TC;_push_handler(_T0);{int _TD=0;_T1=setjmp(_TC.handler);if(!_T1)goto _TLCE;_TD=1;goto _TLCF;_TLCE: _TLCF: if(_TD)goto _TLD0;else{goto _TLD2;}_TLD2: _T3=Cyc_Hashtable_lookup;{
struct Cyc_Set_Set*(*_TE)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_T3;_T2=_TE;}{struct Cyc_Set_Set*_TE=_T2(t,x);_npop_handler(0);return _TE;}_pop_handler();goto _TLD1;_TLD0: _T4=Cyc_Core_get_exn_thrown();{void*_TE=(void*)_T4;void*_TF;_T5=(struct Cyc_Core_Not_found_exn_struct*)_TE;_T6=_T5->tag;_T7=Cyc_Core_Not_found;if(_T6!=_T7)goto _TLD3;_T9=Cyc_Set_empty;{
# 1231
struct Cyc_Set_Set*(*_T10)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T9;_T8=_T10;}_TA=Cyc_strptrcmp;_TB=_T8(_TA);return _TB;_TLD3: _TF=_TE;{void*exn=_TF;_rethrow(exn);};}_TLD1:;}}
# 1235
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*(*_T0)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Set_Set*(*_T1)(int(*)(void*,void*));int(*_T2)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Set_Set*(*_T3)(struct Cyc_Set_Set*,struct _fat_ptr*);struct Cyc_Set_Set*(*_T4)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T5;struct Cyc_List_List*_T6;void*_T7;struct _fat_ptr*_T8;struct Cyc_List_List*_T9;struct _fat_ptr*_TA;int _TB;int(*_TC)(struct Cyc_Iter_Iter,struct _fat_ptr**);int(*_TD)(struct Cyc_Iter_Iter,void*);struct Cyc_Iter_Iter _TE;struct _fat_ptr**_TF;int _T10;struct Cyc_Set_Set*_T11;struct Cyc_Set_Set*_T12;struct Cyc_Set_Set*_T13;_T1=Cyc_Set_empty;{
# 1245 "buildlib.cyl"
struct Cyc_Set_Set*(*_T14)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T1;_T0=_T14;}_T2=Cyc_strptrcmp;{struct Cyc_Set_Set*emptyset=_T0(_T2);
struct Cyc_Set_Set*curr;
curr=emptyset;_TLD8: if(init!=0)goto _TLD6;else{goto _TLD7;}
_TLD6: _T4=Cyc_Set_insert;{struct Cyc_Set_Set*(*_T14)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T4;_T3=_T14;}_T5=curr;_T6=init;_T7=_T6->hd;_T8=(struct _fat_ptr*)_T7;curr=_T3(_T5,_T8);_T9=init;
# 1247
init=_T9->tl;goto _TLD8;_TLD7: {
# 1250
struct Cyc_Set_Set*delta=curr;
# 1252
struct _fat_ptr*sptr;sptr=_cycalloc(sizeof(struct _fat_ptr));_TA=sptr;*_TA=_tag_fat("",sizeof(char),1U);
_TLD9: _TB=Cyc_Set_cardinality(delta);if(_TB > 0)goto _TLDA;else{goto _TLDB;}
_TLDA:{struct Cyc_Set_Set*next=emptyset;
struct Cyc_Iter_Iter iter=Cyc_Set_make_iter(Cyc_Core_heap_region,delta);
_TLDC: _TD=Cyc_Iter_next;{int(*_T14)(struct Cyc_Iter_Iter,struct _fat_ptr**)=(int(*)(struct Cyc_Iter_Iter,struct _fat_ptr**))_TD;_TC=_T14;}_TE=iter;_TF=& sptr;_T10=_TC(_TE,_TF);if(_T10)goto _TLDD;else{goto _TLDE;}
_TLDD: _T11=next;_T12=Cyc_find(t,sptr);next=Cyc_Set_union_two(_T11,_T12);goto _TLDC;_TLDE:
 delta=Cyc_Set_diff(next,curr);
curr=Cyc_Set_union_two(curr,delta);}goto _TLD9;_TLDB: _T13=curr;
# 1261
return _T13;}}}
# 1264
enum Cyc_buildlib_mode{Cyc_NORMAL =0U,Cyc_GATHER =1U,Cyc_GATHERSCRIPT =2U,Cyc_FINISH =3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering (void){int _T0;enum Cyc_buildlib_mode _T1;int _T2;enum Cyc_buildlib_mode _T3;int _T4;_T1=Cyc_mode;_T2=(int)_T1;
if(_T2!=1)goto _TLDF;_T0=1;goto _TLE0;_TLDF: _T3=Cyc_mode;_T4=(int)_T3;_T0=_T4==2;_TLE0: return _T0;}
# 1270
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _fat_ptr fmt,struct _fat_ptr ap){struct Cyc___cycFILE*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;int _T3;
# 1274
if(Cyc_script_file!=0)goto _TLE1;_T0=Cyc_stderr;_T1=
_tag_fat("Internal error: script file is NULL\n",sizeof(char),37U);_T2=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T0,_T1,_T2);
exit(1);goto _TLE2;_TLE1: _TLE2: _T3=
# 1278
Cyc_vfprintf(Cyc_script_file,fmt,ap);return _T3;}
# 1281
int Cyc_force_directory(struct _fat_ptr d){enum Cyc_buildlib_mode _T0;int _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_String_pa_PrintArg_struct _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;struct _fat_ptr _T9;int _TA;int _TB;struct _fat_ptr _TC;unsigned char*_TD;unsigned char*_TE;const char*_TF;int _T10;int _T11;struct Cyc_String_pa_PrintArg_struct _T12;struct Cyc___cycFILE*_T13;struct _fat_ptr _T14;struct _fat_ptr _T15;_T0=Cyc_mode;_T1=(int)_T0;
if(_T1!=2)goto _TLE3;{struct Cyc_String_pa_PrintArg_struct _T16;_T16.tag=0;
_T16.f1=d;_T2=_T16;}{struct Cyc_String_pa_PrintArg_struct _T16=_T2;{struct Cyc_String_pa_PrintArg_struct _T17;_T17.tag=0;_T17.f1=d;_T3=_T17;}{struct Cyc_String_pa_PrintArg_struct _T17=_T3;void*_T18[2];_T18[0]=& _T16;_T18[1]=& _T17;_T4=_tag_fat("if ! test -e %s; then mkdir %s; fi\n",sizeof(char),36U);_T5=_tag_fat(_T18,sizeof(void*),2);Cyc_prscript(_T4,_T5);}}goto _TLE4;
# 1288
_TLE3: _T6=d;_T7=_untag_fat_ptr_check_bound(_T6,sizeof(char),1U);_T8=(const char*)_T7;_T9=_tag_fat(0U,sizeof(unsigned short),0);{int fd=Cyc_open(_T8,0,_T9);_TA=fd;_TB=- 1;
if(_TA!=_TB)goto _TLE5;_TC=d;_TD=_untag_fat_ptr(_TC,sizeof(char),1U);_TE=_check_null(_TD);_TF=(const char*)_TE;_T10=
mkdir(_TF,448);_T11=- 1;if(_T10!=_T11)goto _TLE7;{struct Cyc_String_pa_PrintArg_struct _T16;_T16.tag=0;
_T16.f1=d;_T12=_T16;}{struct Cyc_String_pa_PrintArg_struct _T16=_T12;void*_T17[1];_T17[0]=& _T16;_T13=Cyc_stderr;_T14=_tag_fat("Error: could not create directory %s\n",sizeof(char),38U);_T15=_tag_fat(_T17,sizeof(void*),1);Cyc_fprintf(_T13,_T14,_T15);}
return 1;_TLE7: goto _TLE6;
# 1295
_TLE5: close(fd);_TLE6:;}_TLE4:
# 1297
 return 0;}
# 1300
int Cyc_force_directory_prefixes(struct _fat_ptr file){unsigned long _T0;struct Cyc_List_List*_T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;struct Cyc_List_List*_T4;void*_T5;struct _fat_ptr*_T6;struct _fat_ptr _T7;int _T8;struct Cyc_List_List*_T9;
# 1304
struct _fat_ptr curr=Cyc_strdup(file);
# 1306
struct Cyc_List_List*x=0;
_TLE9: if(1)goto _TLEA;else{goto _TLEB;}
_TLEA: curr=Cyc_Filename_dirname(curr);_T0=
Cyc_strlen(curr);if(_T0!=0U)goto _TLEC;goto _TLEB;_TLEC:{struct Cyc_List_List*_TA=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TB=_cycalloc(sizeof(struct _fat_ptr));_T3=curr;
*_TB=_T3;_T2=(struct _fat_ptr*)_TB;}_TA->hd=_T2;_TA->tl=x;_T1=(struct Cyc_List_List*)_TA;}x=_T1;goto _TLE9;_TLEB:
# 1313
 _TLF1: if(x!=0)goto _TLEF;else{goto _TLF0;}
_TLEF: _T4=x;_T5=_T4->hd;_T6=(struct _fat_ptr*)_T5;_T7=*_T6;_T8=Cyc_force_directory(_T7);if(!_T8)goto _TLF2;return 1;_TLF2: _T9=x;
# 1313
x=_T9->tl;goto _TLF1;_TLF0:
# 1316
 return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _fat_ptr f1;};
# 1323
static int Cyc_is_other_special(char c){char _T0;int _T1;_T0=c;_T1=(int)_T0;switch(_T1){case 92: goto _LL4;case 34: _LL4: goto _LL6;case 59: _LL6: goto _LL8;case 38: _LL8: goto _LLA;case 40: _LLA: goto _LLC;case 41: _LLC: goto _LLE;case 124: _LLE: goto _LL10;case 94: _LL10: goto _LL12;case 60: _LL12: goto _LL14;case 62: _LL14: goto _LL16;case 10: _LL16: goto _LL18;case 9: _LL18:
# 1339
 return 1;default:
 return 0;};}
# 1344
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;unsigned char*_T4;const char*_T5;const char*_T6;int _T7;char _T8;int _T9;int _TA;struct _fat_ptr _TB;struct Cyc_List_List*_TC;struct _fat_ptr*_TD;struct _fat_ptr*_TE;struct _fat_ptr _TF;struct _fat_ptr*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;unsigned long _T13;int _T14;unsigned long _T15;unsigned long _T16;int _T17;unsigned long _T18;struct _fat_ptr _T19;unsigned long _T1A;char*_T1B;unsigned _T1C;unsigned _T1D;int _T1E;unsigned long _T1F;unsigned long _T20;struct _fat_ptr _T21;unsigned char*_T22;const char*_T23;const char*_T24;int _T25;char _T26;int _T27;int _T28;struct _fat_ptr _T29;int _T2A;int _T2B;unsigned char*_T2C;char*_T2D;unsigned _T2E;unsigned char*_T2F;char*_T30;struct _fat_ptr _T31;int _T32;int _T33;unsigned char*_T34;char*_T35;unsigned _T36;unsigned char*_T37;char*_T38;struct _fat_ptr _T39;
unsigned long len=Cyc_strlen(s);
# 1348
int single_quotes=0;
int other_special=0;{
int i=0;_TLF8: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 < _T2)goto _TLF6;else{goto _TLF7;}
_TLF6: _T3=s;_T4=_T3.curr;_T5=(const char*)_T4;_T6=_check_null(_T5);_T7=i;{char c=_T6[_T7];_T8=c;_T9=(int)_T8;
if(_T9!=39)goto _TLF9;single_quotes=single_quotes + 1;goto _TLFA;
_TLF9: _TA=Cyc_is_other_special(c);if(!_TA)goto _TLFB;other_special=other_special + 1;goto _TLFC;_TLFB: _TLFC: _TLFA:;}
# 1350
i=i + 1;goto _TLF8;_TLF7:;}
# 1357
if(single_quotes!=0)goto _TLFD;if(other_special!=0)goto _TLFD;_TB=s;
return _TB;_TLFD:
# 1361
 if(single_quotes!=0)goto _TLFF;{struct _fat_ptr*_T3A[3];{struct _fat_ptr*_T3B=_cycalloc(sizeof(struct _fat_ptr));
*_T3B=_tag_fat("'",sizeof(char),2U);_TD=(struct _fat_ptr*)_T3B;}_T3A[0]=_TD;{struct _fat_ptr*_T3B=_cycalloc(sizeof(struct _fat_ptr));_TF=s;*_T3B=_TF;_TE=(struct _fat_ptr*)_T3B;}_T3A[1]=_TE;{struct _fat_ptr*_T3B=_cycalloc(sizeof(struct _fat_ptr));*_T3B=_tag_fat("'",sizeof(char),2U);_T10=(struct _fat_ptr*)_T3B;}_T3A[2]=_T10;_T11=_tag_fat(_T3A,sizeof(struct _fat_ptr*),3);_TC=Cyc_List_list(_T11);}_T12=Cyc_strconcat_l(_TC);return _T12;_TLFF: _T13=len;_T14=single_quotes;_T15=(unsigned long)_T14;_T16=_T13 + _T15;_T17=other_special;_T18=(unsigned long)_T17;{
# 1365
unsigned long len2=_T16 + _T18;_T1A=len2 + 1U;{unsigned _T3A=_T1A + 1U;_T1C=_check_times(_T3A,sizeof(char));{char*_T3B=_cycalloc_atomic(_T1C);{unsigned _T3C=_T3A;unsigned i;i=0;_TL104: if(i < _T3C)goto _TL102;else{goto _TL103;}_TL102: _T1D=i;
_T3B[_T1D]='\000';i=i + 1;goto _TL104;_TL103: _T3B[_T3C]=0;}_T1B=(char*)_T3B;}_T19=_tag_fat(_T1B,sizeof(char),_T3A);}{struct _fat_ptr s2=_T19;
int i=0;
int j=0;
_TL108: _T1E=i;_T1F=(unsigned long)_T1E;_T20=len;if(_T1F < _T20)goto _TL106;else{goto _TL107;}
_TL106: _T21=s;_T22=_T21.curr;_T23=(const char*)_T22;_T24=_check_null(_T23);_T25=i;{char c=_T24[_T25];_T26=c;_T27=(int)_T26;
if(_T27==39)goto _TL10B;else{goto _TL10C;}_TL10C: _T28=Cyc_is_other_special(c);if(_T28)goto _TL10B;else{goto _TL109;}
_TL10B: _T29=s2;_T2A=j;j=_T2A + 1;_T2B=_T2A;{struct _fat_ptr _T3A=_fat_ptr_plus(_T29,sizeof(char),_T2B);_T2C=_check_fat_subscript(_T3A,sizeof(char),0U);_T2D=(char*)_T2C;{char _T3B=*_T2D;char _T3C='\\';_T2E=_get_fat_size(_T3A,sizeof(char));if(_T2E!=1U)goto _TL10D;if(_T3B!=0)goto _TL10D;if(_T3C==0)goto _TL10D;_throw_arraybounds();goto _TL10E;_TL10D: _TL10E: _T2F=_T3A.curr;_T30=(char*)_T2F;*_T30=_T3C;}}goto _TL10A;_TL109: _TL10A: _T31=s2;_T32=j;
j=_T32 + 1;_T33=_T32;{struct _fat_ptr _T3A=_fat_ptr_plus(_T31,sizeof(char),_T33);_T34=_check_fat_subscript(_T3A,sizeof(char),0U);_T35=(char*)_T34;{char _T3B=*_T35;char _T3C=c;_T36=_get_fat_size(_T3A,sizeof(char));if(_T36!=1U)goto _TL10F;if(_T3B!=0)goto _TL10F;if(_T3C==0)goto _TL10F;_throw_arraybounds();goto _TL110;_TL10F: _TL110: _T37=_T3A.curr;_T38=(char*)_T37;*_T38=_T3C;}}}
# 1369
i=i + 1;goto _TL108;_TL107: _T39=s2;
# 1375
return _T39;}}}
# 1377
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){struct _fat_ptr*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;{struct _fat_ptr*_T3=_cycalloc(sizeof(struct _fat_ptr));_T1=sp;_T2=*_T1;
*_T3=Cyc_sh_escape_string(_T2);_T0=(struct _fat_ptr*)_T3;}return _T0;}
# 1382
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){struct Cyc_Set_Set**_T0;unsigned _T1;int(*_T2)(struct Cyc_Set_Set*,struct _fat_ptr*);int(*_T3)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_T4;struct Cyc_Set_Set*_T5;struct _fat_ptr*_T6;struct _fat_ptr _T7;void*_T8;void*_T9;unsigned _TA;int _TB;struct Cyc_String_pa_PrintArg_struct _TC;struct _fat_ptr _TD;void*_TE;void*_TF;unsigned _T10;struct Cyc___cycFILE*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;int _T14;struct Cyc_String_pa_PrintArg_struct _T15;struct _fat_ptr _T16;void*_T17;void*_T18;unsigned _T19;struct _fat_ptr _T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;void*_T1D;void*_T1E;unsigned _T1F;struct _fat_ptr _T20;void*_T21;void*_T22;unsigned _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct _fat_ptr _T26;unsigned char*_T27;unsigned char*_T28;struct _fat_ptr _T29;struct _fat_ptr _T2A;unsigned _T2B;struct _fat_ptr _T2C;struct Cyc_String_pa_PrintArg_struct _T2D;struct _fat_ptr _T2E;struct _fat_ptr _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc_String_pa_PrintArg_struct _T32;struct _fat_ptr _T33;struct _fat_ptr _T34;unsigned char*_T35;unsigned char*_T36;struct _fat_ptr _T37;struct _fat_ptr _T38;unsigned _T39;struct _fat_ptr _T3A;struct Cyc_String_pa_PrintArg_struct _T3B;struct _fat_ptr _T3C;struct _fat_ptr _T3D;struct _fat_ptr _T3E;struct _fat_ptr _T3F;struct Cyc_String_pa_PrintArg_struct _T40;struct _fat_ptr _T41;struct _fat_ptr _T42;unsigned char*_T43;unsigned char*_T44;struct _fat_ptr _T45;struct _fat_ptr _T46;unsigned _T47;struct _fat_ptr _T48;struct Cyc_String_pa_PrintArg_struct _T49;struct _fat_ptr _T4A;struct _fat_ptr _T4B;struct _fat_ptr _T4C;struct _fat_ptr _T4D;struct Cyc_String_pa_PrintArg_struct _T4E;struct _fat_ptr _T4F;struct _fat_ptr _T50;unsigned char*_T51;unsigned char*_T52;struct _handler_cons*_T53;int _T54;struct _fat_ptr _T55;void*_T56;void*_T57;unsigned _T58;int _T59;enum Cyc_buildlib_mode _T5A;int _T5B;enum Cyc_buildlib_mode _T5C;int _T5D;struct Cyc_String_pa_PrintArg_struct _T5E;struct _fat_ptr _T5F;void*_T60;void*_T61;unsigned _T62;struct _fat_ptr _T63;struct _fat_ptr _T64;struct Cyc_String_pa_PrintArg_struct _T65;struct Cyc_List_List*_T66;void*_T67;struct _fat_ptr*_T68;struct _fat_ptr _T69;struct _fat_ptr _T6A;struct Cyc_List_List*_T6B;struct Cyc_String_pa_PrintArg_struct _T6C;struct _fat_ptr _T6D;void*_T6E;void*_T6F;unsigned _T70;struct _fat_ptr _T71;struct _fat_ptr _T72;struct _fat_ptr _T73;struct _fat_ptr _T74;struct Cyc_String_pa_PrintArg_struct _T75;struct Cyc_String_pa_PrintArg_struct _T76;struct _fat_ptr _T77;void*_T78;void*_T79;unsigned _T7A;struct Cyc_String_pa_PrintArg_struct _T7B;struct _fat_ptr _T7C;void*_T7D;void*_T7E;unsigned _T7F;struct _fat_ptr _T80;struct _fat_ptr _T81;struct Cyc_String_pa_PrintArg_struct _T82;struct Cyc_String_pa_PrintArg_struct _T83;struct _fat_ptr _T84;void*_T85;void*_T86;unsigned _T87;struct Cyc_String_pa_PrintArg_struct _T88;struct _fat_ptr _T89;void*_T8A;void*_T8B;unsigned _T8C;struct _fat_ptr _T8D;struct _fat_ptr _T8E;struct Cyc_String_pa_PrintArg_struct _T8F;struct _fat_ptr _T90;void*_T91;void*_T92;unsigned _T93;struct _fat_ptr _T94;struct _fat_ptr _T95;struct Cyc___cycFILE*_T96;unsigned _T97;struct Cyc_String_pa_PrintArg_struct _T98;struct _fat_ptr _T99;void*_T9A;void*_T9B;unsigned _T9C;struct Cyc___cycFILE*_T9D;struct _fat_ptr _T9E;struct _fat_ptr _T9F;int _TA0;struct Cyc_String_pa_PrintArg_struct _TA1;struct _fat_ptr _TA2;void*_TA3;void*_TA4;unsigned _TA5;struct Cyc___cycFILE*_TA6;struct _fat_ptr _TA7;struct _fat_ptr _TA8;struct Cyc_List_List*_TA9;void*_TAA;struct _fat_ptr*_TAB;struct _fat_ptr _TAC;unsigned char*_TAD;unsigned char*_TAE;const char*_TAF;struct Cyc___cycFILE*_TB0;struct Cyc_List_List*_TB1;struct Cyc_String_pa_PrintArg_struct _TB2;struct _fat_ptr _TB3;void*_TB4;void*_TB5;unsigned _TB6;struct Cyc___cycFILE*_TB7;struct _fat_ptr _TB8;struct _fat_ptr _TB9;struct Cyc_List_List*_TBA;struct _fat_ptr*_TBB;struct _fat_ptr _TBC;struct Cyc_List_List*(*_TBD)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*);struct Cyc_List_List*(*_TBE)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_TBF;struct _fat_ptr _TC0;struct _fat_ptr _TC1;struct Cyc_String_pa_PrintArg_struct _TC2;struct Cyc_String_pa_PrintArg_struct _TC3;struct Cyc_String_pa_PrintArg_struct _TC4;struct Cyc_String_pa_PrintArg_struct _TC5;struct _fat_ptr _TC6;void*_TC7;void*_TC8;unsigned _TC9;struct Cyc_String_pa_PrintArg_struct _TCA;struct _fat_ptr _TCB;void*_TCC;void*_TCD;unsigned _TCE;struct Cyc_String_pa_PrintArg_struct _TCF;int _TD0;struct _fat_ptr _TD1;struct _fat_ptr _TD2;unsigned char*_TD3;unsigned char*_TD4;int _TD5;struct Cyc_String_pa_PrintArg_struct _TD6;struct _fat_ptr _TD7;void*_TD8;void*_TD9;unsigned _TDA;struct Cyc___cycFILE*_TDB;struct _fat_ptr _TDC;struct _fat_ptr _TDD;int _TDE;struct _fat_ptr _TDF;struct Cyc_String_pa_PrintArg_struct _TE0;struct Cyc_String_pa_PrintArg_struct _TE1;struct Cyc_String_pa_PrintArg_struct _TE2;struct Cyc_String_pa_PrintArg_struct _TE3;struct _fat_ptr _TE4;void*_TE5;void*_TE6;unsigned _TE7;struct Cyc_String_pa_PrintArg_struct _TE8;struct _fat_ptr _TE9;void*_TEA;void*_TEB;unsigned _TEC;struct Cyc_String_pa_PrintArg_struct _TED;int _TEE;struct _fat_ptr _TEF;struct _fat_ptr _TF0;unsigned char*_TF1;unsigned char*_TF2;int _TF3;struct Cyc_String_pa_PrintArg_struct _TF4;struct _fat_ptr _TF5;void*_TF6;void*_TF7;unsigned _TF8;struct Cyc___cycFILE*_TF9;struct _fat_ptr _TFA;struct _fat_ptr _TFB;int _TFC;struct Cyc___cycFILE*_TFD;unsigned _TFE;struct Cyc_NO_SUPPORT_exn_struct*_TFF;struct _fat_ptr _T100;struct Cyc_String_pa_PrintArg_struct _T101;struct _fat_ptr _T102;void*_T103;void*_T104;unsigned _T105;struct _fat_ptr _T106;struct _fat_ptr _T107;void*_T108;int _T109;struct Cyc___cycFILE*_T10A;struct _fat_ptr _T10B;struct _fat_ptr _T10C;struct _tuple13*_T10D;void(*_T10E)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*);void(*_T10F)(struct Cyc_Hashtable_Table*,void*,void*);int _T110;struct Cyc___cycFILE*_T111;struct _fat_ptr _T112;struct _fat_ptr _T113;struct Cyc___cycFILE*_T114;unsigned _T115;struct Cyc_NO_SUPPORT_exn_struct*_T116;struct _fat_ptr _T117;struct Cyc_String_pa_PrintArg_struct _T118;struct _fat_ptr _T119;void*_T11A;void*_T11B;unsigned _T11C;struct _fat_ptr _T11D;struct _fat_ptr _T11E;void*_T11F;int _T120;struct Cyc___cycFILE*_T121;struct _fat_ptr _T122;struct _fat_ptr _T123;struct Cyc___cycFILE*_T124;unsigned _T125;int _T126;int _T127;struct Cyc___cycFILE*_T128;struct _fat_ptr _T129;struct _fat_ptr _T12A;struct Cyc_List_List*_T12B;void*_T12C;struct Cyc_String_pa_PrintArg_struct _T12D;struct _fat_ptr*_T12E;struct Cyc___cycFILE*_T12F;struct _fat_ptr _T130;struct _fat_ptr _T131;struct Cyc_List_List*_T132;struct Cyc___cycFILE*_T133;enum Cyc_buildlib_mode _T134;int _T135;struct Cyc___cycFILE*_T136;unsigned _T137;int _T138;struct Cyc___cycFILE*_T139;struct _fat_ptr _T13A;struct _fat_ptr _T13B;struct _fat_ptr _T13C;void*_T13D;void*_T13E;unsigned _T13F;struct _handler_cons*_T140;int _T141;int _T142;struct Cyc___cycFILE*_T143;struct _fat_ptr _T144;struct _fat_ptr _T145;void*_T146;int _T147;struct Cyc___cycFILE*_T148;struct _fat_ptr _T149;struct _fat_ptr _T14A;struct Cyc_List_List*_T14B;void*_T14C;struct Cyc_Absyn_Decl*_T14D;struct Cyc_Hashtable_Table*_T14E;struct Cyc_List_List*_T14F;struct Cyc_List_List*(*_T150)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*);struct Cyc_List_List*(*_T151)(void*(*)(void*),struct Cyc_List_List*);struct _tuple0 _T152;struct Cyc_List_List*_T153;struct Cyc_List_List*_T154;struct Cyc_Hashtable_Table*_T155;struct Cyc_Set_Set*(*_T156)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Set_Set*(*_T157)(int(*)(void*,void*));int(*_T158)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T159;void*_T15A;struct Cyc_Absyn_Decl*_T15B;int*_T15C;unsigned _T15D;struct Cyc_Absyn_Vardecl*_T15E;struct Cyc_Set_Set*(*_T15F)(struct Cyc_Set_Set*,struct _fat_ptr*);struct Cyc_Set_Set*(*_T160)(struct Cyc_Set_Set*,void*);int(*_T161)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);int(*_T162)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T163)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T164;struct _fat_ptr*_T165;int _T166;struct Cyc_Absyn_Fndecl*_T167;struct Cyc_Set_Set*(*_T168)(struct Cyc_Set_Set*,struct _fat_ptr*);struct Cyc_Set_Set*(*_T169)(struct Cyc_Set_Set*,void*);int(*_T16A)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);int(*_T16B)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T16C)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T16D;struct _fat_ptr*_T16E;int _T16F;struct Cyc_Absyn_Aggrdecl*_T170;struct Cyc_Absyn_Enumdecl*_T171;int(*_T172)(struct Cyc_Set_Set*,struct _fat_ptr*);int(*_T173)(struct Cyc_Set_Set*,void*);int _T174;struct Cyc_List_List*_T175;struct Cyc_Absyn_Enumdecl*_T176;struct Cyc_Core_Opt*_T177;unsigned _T178;struct Cyc_Absyn_Enumdecl*_T179;struct Cyc_Core_Opt*_T17A;void*_T17B;struct Cyc_List_List*_T17C;void*_T17D;struct Cyc_Absyn_Enumfield*_T17E;int(*_T17F)(struct Cyc_Set_Set*,struct _fat_ptr*);int(*_T180)(struct Cyc_Set_Set*,void*);int _T181;struct Cyc_List_List*_T182;struct Cyc_List_List*_T183;struct Cyc_Absyn_Typedefdecl*_T184;int(*_T185)(struct Cyc_Set_Set*,struct _fat_ptr*);int(*_T186)(struct Cyc_Set_Set*,void*);int _T187;struct _fat_ptr*_T188;struct _fat_ptr _T189;struct _fat_ptr _T18A;unsigned long _T18B;int _T18C;struct Cyc_List_List*_T18D;struct Cyc_List_List*_T18E;struct Cyc_List_List*_T18F;int _T190;struct Cyc___cycFILE*_T191;unsigned _T192;struct _fat_ptr _T193;struct Cyc_String_pa_PrintArg_struct _T194;struct _fat_ptr _T195;void*_T196;void*_T197;unsigned _T198;struct _fat_ptr _T199;struct _fat_ptr _T19A;int _T19B;unsigned _T19C;struct _fat_ptr _T19D;unsigned _T19E;struct _fat_ptr _T19F;unsigned char*_T1A0;char*_T1A1;char*_T1A2;int _T1A3;char _T1A4;int _T1A5;struct _fat_ptr _T1A6;unsigned char*_T1A7;char*_T1A8;int _T1A9;char _T1AA;int _T1AB;struct _fat_ptr _T1AC;int _T1AD;unsigned char*_T1AE;char*_T1AF;unsigned _T1B0;unsigned char*_T1B1;char*_T1B2;struct _fat_ptr _T1B3;unsigned char*_T1B4;char*_T1B5;int _T1B6;char _T1B7;int _T1B8;struct _fat_ptr _T1B9;unsigned char*_T1BA;char*_T1BB;int _T1BC;char _T1BD;int _T1BE;struct _fat_ptr _T1BF;int _T1C0;unsigned char*_T1C1;char*_T1C2;struct _fat_ptr _T1C3;unsigned char*_T1C4;char*_T1C5;int _T1C6;char _T1C7;int _T1C8;int _T1C9;unsigned _T1CA;unsigned char*_T1CB;char*_T1CC;int _T1CD;struct Cyc___cycFILE*_T1CE;struct _fat_ptr _T1CF;struct _fat_ptr _T1D0;struct Cyc_String_pa_PrintArg_struct _T1D1;struct Cyc_String_pa_PrintArg_struct _T1D2;struct Cyc___cycFILE*_T1D3;struct _fat_ptr _T1D4;struct _fat_ptr _T1D5;struct Cyc_List_List*_T1D6;void*_T1D7;struct Cyc_Absyn_Decl*_T1D8;int*_T1D9;unsigned _T1DA;struct Cyc_Absyn_Vardecl*_T1DB;struct Cyc_Absyn_Fndecl*_T1DC;int _T1DD;struct Cyc_Absyn_Fndecl*_T1DE;struct Cyc_Absyn_Aggrdecl*_T1DF;struct Cyc_Absyn_Enumdecl*_T1E0;struct Cyc_Absyn_Typedefdecl*_T1E1;struct _fat_ptr*_T1E2;unsigned _T1E3;int _T1E4;int(*_T1E5)(struct Cyc_Set_Set*,struct _fat_ptr*);int(*_T1E6)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T1E7;struct _fat_ptr*_T1E8;int _T1E9;struct Cyc_Core_Impossible_exn_struct*_T1EA;void*_T1EB;struct Cyc_Absyn_Decl*_T1EC;int*_T1ED;unsigned _T1EE;struct Cyc_NO_SUPPORT_exn_struct*_T1EF;void*_T1F0;struct Cyc_List_List*_T1F1;struct Cyc_List_List*_T1F2;struct Cyc_List_List*_T1F3;struct Cyc_List_List*_T1F4;struct _handler_cons*_T1F5;int _T1F6;struct Cyc_Tcenv_Tenv*_T1F7;struct Cyc_List_List*_T1F8;void*_T1F9;struct Cyc_NO_SUPPORT_exn_struct*_T1FA;void*_T1FB;struct _tuple0 _T1FC;struct Cyc_List_List*_T1FD;void*_T1FE;struct Cyc_List_List*_T1FF;void*_T200;struct _fat_ptr*_T201;unsigned _T202;struct Cyc_Absynpp_Params*_T203;struct Cyc_Absynpp_Params*_T204;struct _fat_ptr*_T205;unsigned _T206;struct _fat_ptr _T207;struct Cyc_String_pa_PrintArg_struct _T208;struct _fat_ptr*_T209;struct _fat_ptr _T20A;struct _fat_ptr _T20B;struct Cyc_String_pa_PrintArg_struct _T20C;struct Cyc___cycFILE*_T20D;struct _fat_ptr _T20E;struct _fat_ptr _T20F;struct Cyc_String_pa_PrintArg_struct _T210;struct Cyc___cycFILE*_T211;struct _fat_ptr _T212;struct _fat_ptr _T213;struct Cyc_List_List*_T214;struct _fat_ptr _T215;struct Cyc___cycFILE*_T216;struct Cyc___cycFILE*_T217;struct _fat_ptr _T218;struct _fat_ptr _T219;struct Cyc_List_List*_T21A;struct _fat_ptr _T21B;struct Cyc___cycFILE*_T21C;struct Cyc_List_List*_T21D;struct Cyc_List_List*_T21E;struct Cyc___cycFILE*_T21F;unsigned _T220;struct Cyc_NO_SUPPORT_exn_struct*_T221;struct _fat_ptr _T222;struct Cyc_String_pa_PrintArg_struct _T223;struct _fat_ptr _T224;void*_T225;void*_T226;unsigned _T227;struct _fat_ptr _T228;struct _fat_ptr _T229;void*_T22A;struct _tuple14*_T22B;int(*_T22C)(struct Cyc_Set_Set*,struct _fat_ptr*);int(*_T22D)(struct Cyc_Set_Set*,void*);int _T22E;struct Cyc_String_pa_PrintArg_struct _T22F;struct _fat_ptr*_T230;struct Cyc___cycFILE*_T231;struct _fat_ptr _T232;struct _fat_ptr _T233;struct Cyc_String_pa_PrintArg_struct _T234;struct Cyc___cycFILE*_T235;struct _fat_ptr _T236;struct _fat_ptr _T237;struct Cyc___cycFILE*_T238;struct _fat_ptr _T239;struct _fat_ptr _T23A;enum Cyc_buildlib_mode _T23B;int _T23C;struct Cyc_List_List*_T23D;void*_T23E;struct _fat_ptr _T23F;unsigned char*_T240;char*_T241;struct _fat_ptr _T242;unsigned char*_T243;char*_T244;struct _fat_ptr _T245;unsigned char*_T246;const char*_T247;struct Cyc___cycFILE*_T248;int(*_T249)(struct Cyc_Set_Set*,struct _fat_ptr*);int(*_T24A)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T24B;struct _fat_ptr*_T24C;int _T24D;struct _fat_ptr _T24E;unsigned char*_T24F;const char*_T250;struct Cyc___cycFILE*_T251;struct Cyc_String_pa_PrintArg_struct _T252;struct _fat_ptr _T253;struct _fat_ptr _T254;int _T255;struct _fat_ptr*(*_T256)(struct Cyc_Set_Set*);void*(*_T257)(struct Cyc_Set_Set*);struct Cyc_Set_Set*(*_T258)(struct Cyc_Set_Set*,struct _fat_ptr*);struct Cyc_Set_Set*(*_T259)(struct Cyc_Set_Set*,void*);struct Cyc_String_pa_PrintArg_struct _T25A;struct _fat_ptr*_T25B;struct _fat_ptr _T25C;struct _fat_ptr _T25D;struct _fat_ptr _T25E;struct _fat_ptr _T25F;struct Cyc_String_pa_PrintArg_struct _T260;struct _fat_ptr _T261;struct _fat_ptr _T262;struct Cyc_List_List*_T263;struct Cyc___cycFILE*_T264;struct _fat_ptr _T265;struct _fat_ptr _T266;int _T267;struct Cyc___cycFILE*_T268;struct _fat_ptr _T269;struct _fat_ptr _T26A;int _T26B;struct Cyc_List_List*_T26C;void*_T26D;struct _fat_ptr _T26E;unsigned char*_T26F;char*_T270;struct _fat_ptr _T271;unsigned char*_T272;char*_T273;int(*_T274)(struct Cyc_Set_Set*,struct _fat_ptr*);int(*_T275)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T276;struct _fat_ptr*_T277;int _T278;struct _fat_ptr _T279;unsigned char*_T27A;const char*_T27B;struct Cyc___cycFILE*_T27C;struct Cyc_List_List*_T27D;struct Cyc_String_pa_PrintArg_struct _T27E;struct _fat_ptr _T27F;void*_T280;void*_T281;unsigned _T282;struct Cyc___cycFILE*_T283;struct _fat_ptr _T284;struct _fat_ptr _T285;struct Cyc_List_List*_T286;void*_T287;struct _fat_ptr _T288;unsigned char*_T289;char*_T28A;struct _fat_ptr _T28B;unsigned char*_T28C;char*_T28D;int(*_T28E)(struct Cyc_Set_Set*,struct _fat_ptr*);int(*_T28F)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T290;struct _fat_ptr*_T291;int _T292;struct _fat_ptr _T293;unsigned char*_T294;const char*_T295;struct Cyc___cycFILE*_T296;struct Cyc_List_List*_T297;struct Cyc___cycFILE*_T298;struct _fat_ptr _T299;struct _fat_ptr _T29A;void*_T29B;struct Cyc_Core_Impossible_exn_struct*_T29C;char*_T29D;char*_T29E;struct Cyc_String_pa_PrintArg_struct _T29F;struct _fat_ptr _T2A0;struct _fat_ptr _T2A1;struct Cyc_Dict_Absent_exn_struct*_T2A2;char*_T2A3;char*_T2A4;struct _fat_ptr _T2A5;struct _fat_ptr _T2A6;struct Cyc_Core_Failure_exn_struct*_T2A7;char*_T2A8;char*_T2A9;struct Cyc_String_pa_PrintArg_struct _T2AA;struct _fat_ptr _T2AB;struct _fat_ptr _T2AC;struct Cyc_Core_Invalid_argument_exn_struct*_T2AD;char*_T2AE;char*_T2AF;struct Cyc_String_pa_PrintArg_struct _T2B0;struct _fat_ptr _T2B1;struct _fat_ptr _T2B2;struct Cyc_Core_Not_found_exn_struct*_T2B3;char*_T2B4;char*_T2B5;struct _fat_ptr _T2B6;struct _fat_ptr _T2B7;struct Cyc_NO_SUPPORT_exn_struct*_T2B8;char*_T2B9;char*_T2BA;struct Cyc_String_pa_PrintArg_struct _T2BB;struct _fat_ptr _T2BC;struct _fat_ptr _T2BD;struct Cyc_Lexing_Error_exn_struct*_T2BE;char*_T2BF;char*_T2C0;struct Cyc_String_pa_PrintArg_struct _T2C1;struct _fat_ptr _T2C2;struct _fat_ptr _T2C3;struct _fat_ptr _T2C4;struct _fat_ptr _T2C5;struct Cyc___cycFILE*_T2C6;unsigned _T2C7;struct Cyc_String_pa_PrintArg_struct _T2C8;struct _fat_ptr _T2C9;void*_T2CA;void*_T2CB;unsigned _T2CC;struct Cyc___cycFILE*_T2CD;struct _fat_ptr _T2CE;struct _fat_ptr _T2CF;struct Cyc_String_pa_PrintArg_struct _T2D0;struct _fat_ptr _T2D1;void*_T2D2;void*_T2D3;unsigned _T2D4;struct Cyc___cycFILE*_T2D5;struct _fat_ptr _T2D6;struct _fat_ptr _T2D7;struct _fat_ptr _T2D8;struct _fat_ptr _T2D9;
# 1390
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;_T0=Cyc_headers_to_do;_T1=(unsigned)_T0;
# 1395
if(!_T1)goto _TL111;_T3=Cyc_Set_member;{int(*_T2DA)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T3;_T2=_T2DA;}_T4=Cyc_headers_to_do;_T5=*_T4;{struct _fat_ptr*_T2DA=_cycalloc(sizeof(struct _fat_ptr));{const char*_T2DB=filename;_T8=(void*)_T2DB;_T9=(void*)_T2DB;_TA=_get_zero_arr_size_char(_T9,1U);_T7=_tag_fat(_T8,sizeof(char),_TA);}*_T2DA=_T7;_T6=(struct _fat_ptr*)_T2DA;}_TB=_T2(_T5,_T6);if(_TB)goto _TL111;else{goto _TL113;}
_TL113: return 0;_TL111:{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;{const char*_T2DB=filename;_TE=(void*)_T2DB;_TF=(void*)_T2DB;_T10=_get_zero_arr_size_char(_TF,1U);_TD=_tag_fat(_TE,sizeof(char),_T10);}
# 1399
_T2DA.f1=_TD;_TC=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_TC;void*_T2DB[1];_T2DB[0]=& _T2DA;_T11=Cyc_stderr;_T12=_tag_fat("********************************* %s...\n",sizeof(char),41U);_T13=_tag_fat(_T2DB,sizeof(void*),1);Cyc_fprintf(_T11,_T12,_T13);}_T14=
# 1401
Cyc_gathering();if(_T14)goto _TL114;else{goto _TL116;}_TL116:{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;{const char*_T2DB=filename;_T17=(void*)_T2DB;_T18=(void*)_T2DB;_T19=_get_zero_arr_size_char(_T18,1U);_T16=_tag_fat(_T17,sizeof(char),_T19);}_T2DA.f1=_T16;_T15=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_T15;void*_T2DB[1];_T2DB[0]=& _T2DA;_T1A=_tag_fat("\n%s:\n",sizeof(char),6U);_T1B=_tag_fat(_T2DB,sizeof(void*),1);Cyc_log(_T1A,_T1B);}goto _TL115;_TL114: _TL115:{const char*_T2DA=filename;_T1D=(void*)_T2DA;_T1E=(void*)_T2DA;_T1F=_get_zero_arr_size_char(_T1E,1U);_T1C=_tag_fat(_T1D,sizeof(char),_T1F);}{
# 1413 "buildlib.cyl"
struct _fat_ptr basename=Cyc_Filename_basename(_T1C);{const char*_T2DA=filename;_T21=(void*)_T2DA;_T22=(void*)_T2DA;_T23=_get_zero_arr_size_char(_T22,1U);_T20=_tag_fat(_T21,sizeof(char),_T23);}{
struct _fat_ptr dirname=Cyc_Filename_dirname(_T20);
struct _fat_ptr choppedname=Cyc_Filename_chop_extension(basename);_T24=choppedname;_T25=
_tag_fat(".iA",sizeof(char),4U);_T26=Cyc_strconcat(_T24,_T25);_T27=_untag_fat_ptr_check_bound(_T26,sizeof(char),1U);_T28=_check_null(_T27);{const char*cppinfile=(const char*)_T28;_T2A=dirname;_T2B=
# 1419
_get_fat_size(_T2A,sizeof(char));if(_T2B!=0U)goto _TL117;{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;
_T2DA.f1=choppedname;_T2D=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_T2D;void*_T2DB[1];_T2DB[0]=& _T2DA;_T2E=_tag_fat("%s.iB",sizeof(char),6U);_T2F=_tag_fat(_T2DB,sizeof(void*),1);_T2C=Cyc_aprintf(_T2E,_T2F);}_T29=_T2C;goto _TL118;_TL117: _T30=dirname;{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;
_T2DA.f1=choppedname;_T32=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_T32;void*_T2DB[1];_T2DB[0]=& _T2DA;_T33=_tag_fat("%s.iB",sizeof(char),6U);_T34=_tag_fat(_T2DB,sizeof(void*),1);_T31=Cyc_aprintf(_T33,_T34);}_T29=Cyc_Filename_concat(_T30,_T31);_TL118: _T35=_untag_fat_ptr_check_bound(_T29,sizeof(char),1U);_T36=_check_null(_T35);{
# 1418
const char*macrosfile=(const char*)_T36;_T38=dirname;_T39=
# 1424
_get_fat_size(_T38,sizeof(char));if(_T39!=0U)goto _TL119;{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;
_T2DA.f1=choppedname;_T3B=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_T3B;void*_T2DB[1];_T2DB[0]=& _T2DA;_T3C=_tag_fat("%s.iC",sizeof(char),6U);_T3D=_tag_fat(_T2DB,sizeof(void*),1);_T3A=Cyc_aprintf(_T3C,_T3D);}_T37=_T3A;goto _TL11A;_TL119: _T3E=dirname;{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;
_T2DA.f1=choppedname;_T40=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_T40;void*_T2DB[1];_T2DB[0]=& _T2DA;_T41=_tag_fat("%s.iC",sizeof(char),6U);_T42=_tag_fat(_T2DB,sizeof(void*),1);_T3F=Cyc_aprintf(_T41,_T42);}_T37=Cyc_Filename_concat(_T3E,_T3F);_TL11A: _T43=_untag_fat_ptr_check_bound(_T37,sizeof(char),1U);_T44=_check_null(_T43);{
# 1423
const char*declsfile=(const char*)_T44;_T46=dirname;_T47=
# 1429
_get_fat_size(_T46,sizeof(char));if(_T47!=0U)goto _TL11B;{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;
_T2DA.f1=choppedname;_T49=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_T49;void*_T2DB[1];_T2DB[0]=& _T2DA;_T4A=_tag_fat("%s.iD",sizeof(char),6U);_T4B=_tag_fat(_T2DB,sizeof(void*),1);_T48=Cyc_aprintf(_T4A,_T4B);}_T45=_T48;goto _TL11C;_TL11B: _T4C=dirname;{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;
_T2DA.f1=choppedname;_T4E=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_T4E;void*_T2DB[1];_T2DB[0]=& _T2DA;_T4F=_tag_fat("%s.iD",sizeof(char),6U);_T50=_tag_fat(_T2DB,sizeof(void*),1);_T4D=Cyc_aprintf(_T4F,_T50);}_T45=Cyc_Filename_concat(_T4C,_T4D);_TL11C: _T51=_untag_fat_ptr_check_bound(_T45,sizeof(char),1U);_T52=_check_null(_T51);{
# 1428
const char*filtereddeclsfile=(const char*)_T52;{struct _handler_cons _T2DA;_T53=& _T2DA;_push_handler(_T53);{int _T2DB=0;_T54=setjmp(_T2DA.handler);if(!_T54)goto _TL11D;_T2DB=1;goto _TL11E;_TL11D: _TL11E: if(_T2DB)goto _TL11F;else{goto _TL121;}_TL121:{const char*_T2DC=filename;_T56=(void*)_T2DC;_T57=(void*)_T2DC;_T58=_get_zero_arr_size_char(_T57,1U);_T55=_tag_fat(_T56,sizeof(char),_T58);}_T59=
# 1436
Cyc_force_directory_prefixes(_T55);if(!_T59)goto _TL122;{int _T2DC=1;_npop_handler(0);return _T2DC;}_TL122: _T5A=Cyc_mode;_T5B=(int)_T5A;
# 1441
if(_T5B==3)goto _TL124;_T5C=Cyc_mode;_T5D=(int)_T5C;
if(_T5D!=2)goto _TL126;{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{const char*_T2DD=cppinfile;_T60=(void*)_T2DD;_T61=(void*)_T2DD;_T62=_get_zero_arr_size_char(_T61,1U);_T5F=_tag_fat(_T60,sizeof(char),_T62);}
_T2DC.f1=_T5F;_T5E=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T5E;void*_T2DD[1];_T2DD[0]=& _T2DC;_T63=_tag_fat("cat >%s <<XXX\n",sizeof(char),15U);_T64=_tag_fat(_T2DD,sizeof(void*),1);Cyc_prscript(_T63,_T64);}{
struct Cyc_List_List*l=cpp_insert;_TL12B: if(l!=0)goto _TL129;else{goto _TL12A;}
_TL129:{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;_T66=l;_T67=_T66->hd;_T68=(struct _fat_ptr*)_T67;_T2DC.f1=*_T68;_T65=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T65;void*_T2DD[1];_T2DD[0]=& _T2DC;_T69=_tag_fat("%s",sizeof(char),3U);_T6A=_tag_fat(_T2DD,sizeof(void*),1);Cyc_prscript(_T69,_T6A);}_T6B=l;
# 1444
l=_T6B->tl;goto _TL12B;_TL12A:;}{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{const char*_T2DD=filename;_T6E=(void*)_T2DD;_T6F=(void*)_T2DD;_T70=_get_zero_arr_size_char(_T6F,1U);_T6D=_tag_fat(_T6E,sizeof(char),_T70);}
# 1446
_T2DC.f1=_T6D;_T6C=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T6C;void*_T2DD[1];_T2DD[0]=& _T2DC;_T71=_tag_fat("#include <%s>\n",sizeof(char),15U);_T72=_tag_fat(_T2DD,sizeof(void*),1);Cyc_prscript(_T71,_T72);}_T73=
_tag_fat("XXX\n",sizeof(char),5U);_T74=_tag_fat(0U,sizeof(void*),0);Cyc_prscript(_T73,_T74);{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;
_T2DC.f1=Cyc_target_cflags;_T75=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T75;{struct Cyc_String_pa_PrintArg_struct _T2DD;_T2DD.tag=0;{const char*_T2DE=macrosfile;_T78=(void*)_T2DE;_T79=(void*)_T2DE;_T7A=_get_zero_arr_size_char(_T79,1U);_T77=_tag_fat(_T78,sizeof(char),_T7A);}_T2DD.f1=_T77;_T76=_T2DD;}{struct Cyc_String_pa_PrintArg_struct _T2DD=_T76;{struct Cyc_String_pa_PrintArg_struct _T2DE;_T2DE.tag=0;{const char*_T2DF=cppinfile;_T7D=(void*)_T2DF;_T7E=(void*)_T2DF;_T7F=_get_zero_arr_size_char(_T7E,1U);_T7C=_tag_fat(_T7D,sizeof(char),_T7F);}_T2DE.f1=_T7C;_T7B=_T2DE;}{struct Cyc_String_pa_PrintArg_struct _T2DE=_T7B;void*_T2DF[3];_T2DF[0]=& _T2DC;_T2DF[1]=& _T2DD;_T2DF[2]=& _T2DE;_T80=_tag_fat("$GCC %s -E -dM -o %s -x c %s && \\\n",sizeof(char),35U);_T81=_tag_fat(_T2DF,sizeof(void*),3);Cyc_prscript(_T80,_T81);}}}{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;
_T2DC.f1=Cyc_target_cflags;_T82=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T82;{struct Cyc_String_pa_PrintArg_struct _T2DD;_T2DD.tag=0;{const char*_T2DE=declsfile;_T85=(void*)_T2DE;_T86=(void*)_T2DE;_T87=_get_zero_arr_size_char(_T86,1U);_T84=_tag_fat(_T85,sizeof(char),_T87);}_T2DD.f1=_T84;_T83=_T2DD;}{struct Cyc_String_pa_PrintArg_struct _T2DD=_T83;{struct Cyc_String_pa_PrintArg_struct _T2DE;_T2DE.tag=0;{const char*_T2DF=cppinfile;_T8A=(void*)_T2DF;_T8B=(void*)_T2DF;_T8C=_get_zero_arr_size_char(_T8B,1U);_T89=_tag_fat(_T8A,sizeof(char),_T8C);}_T2DE.f1=_T89;_T88=_T2DE;}{struct Cyc_String_pa_PrintArg_struct _T2DE=_T88;void*_T2DF[3];_T2DF[0]=& _T2DC;_T2DF[1]=& _T2DD;_T2DF[2]=& _T2DE;_T8D=_tag_fat("$GCC %s -E     -o %s -x c %s;\n",sizeof(char),31U);_T8E=_tag_fat(_T2DF,sizeof(void*),3);Cyc_prscript(_T8D,_T8E);}}}{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{const char*_T2DD=cppinfile;_T91=(void*)_T2DD;_T92=(void*)_T2DD;_T93=_get_zero_arr_size_char(_T92,1U);_T90=_tag_fat(_T91,sizeof(char),_T93);}
_T2DC.f1=_T90;_T8F=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T8F;void*_T2DD[1];_T2DD[0]=& _T2DC;_T94=_tag_fat("rm %s\n",sizeof(char),7U);_T95=_tag_fat(_T2DD,sizeof(void*),1);Cyc_prscript(_T94,_T95);}goto _TL127;
# 1453
_TL126: maybe=Cyc_fopen(cppinfile,"w");_T96=maybe;_T97=(unsigned)_T96;
if(_T97)goto _TL12C;else{goto _TL12E;}
_TL12E:{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{const char*_T2DD=cppinfile;_T9A=(void*)_T2DD;_T9B=(void*)_T2DD;_T9C=_get_zero_arr_size_char(_T9B,1U);_T99=_tag_fat(_T9A,sizeof(char),_T9C);}_T2DC.f1=_T99;_T98=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T98;void*_T2DD[1];_T2DD[0]=& _T2DC;_T9D=Cyc_stderr;_T9E=_tag_fat("Error: could not create file %s\n",sizeof(char),33U);_T9F=_tag_fat(_T2DD,sizeof(void*),1);Cyc_fprintf(_T9D,_T9E,_T9F);}{int _T2DC=1;_npop_handler(0);return _T2DC;}_TL12C: _TA0=Cyc_verbose;
# 1458
if(!_TA0)goto _TL12F;{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{const char*_T2DD=cppinfile;_TA3=(void*)_T2DD;_TA4=(void*)_T2DD;_TA5=_get_zero_arr_size_char(_TA4,1U);_TA2=_tag_fat(_TA3,sizeof(char),_TA5);}
_T2DC.f1=_TA2;_TA1=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_TA1;void*_T2DD[1];_T2DD[0]=& _T2DC;_TA6=Cyc_stderr;_TA7=_tag_fat("Creating %s\n",sizeof(char),13U);_TA8=_tag_fat(_T2DD,sizeof(void*),1);Cyc_fprintf(_TA6,_TA7,_TA8);}goto _TL130;_TL12F: _TL130:
 out_file=maybe;{
struct Cyc_List_List*l=cpp_insert;_TL134: if(l!=0)goto _TL132;else{goto _TL133;}
_TL132: _TA9=l;_TAA=_TA9->hd;_TAB=(struct _fat_ptr*)_TAA;_TAC=*_TAB;_TAD=_untag_fat_ptr_check_bound(_TAC,sizeof(char),1U);_TAE=_check_null(_TAD);_TAF=(const char*)_TAE;_TB0=out_file;Cyc_fputs(_TAF,_TB0);_TB1=l;
# 1461
l=_TB1->tl;goto _TL134;_TL133:;}{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{const char*_T2DD=filename;_TB4=(void*)_T2DD;_TB5=(void*)_T2DD;_TB6=_get_zero_arr_size_char(_TB5,1U);_TB3=_tag_fat(_TB4,sizeof(char),_TB6);}
# 1464
_T2DC.f1=_TB3;_TB2=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_TB2;void*_T2DD[1];_T2DD[0]=& _T2DC;_TB7=out_file;_TB8=_tag_fat("#include <%s>\n",sizeof(char),15U);_TB9=_tag_fat(_T2DD,sizeof(void*),1);Cyc_fprintf(_TB7,_TB8,_TB9);}
Cyc_fclose(out_file);{struct Cyc_List_List*_T2DC=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T2DD=_cycalloc(sizeof(struct _fat_ptr));_TBC=
# 1467
_tag_fat("",sizeof(char),1U);*_T2DD=_TBC;_TBB=(struct _fat_ptr*)_T2DD;}_T2DC->hd=_TBB;_TBE=Cyc_List_map;{
struct Cyc_List_List*(*_T2DD)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))_TBE;_TBD=_T2DD;}_TBF=Cyc_List_rev(Cyc_cppargs);_T2DC->tl=_TBD(Cyc_sh_escape_stringptr,_TBF);_TBA=(struct Cyc_List_List*)_T2DC;}_TC0=
_tag_fat(" ",sizeof(char),2U);{
# 1466
struct _fat_ptr cppargs_string=
Cyc_str_sepstr(_TBA,_TC0);{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;
# 1472
_T2DC.f1=Cyc_cyclone_cc;_TC2=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_TC2;{struct Cyc_String_pa_PrintArg_struct _T2DD;_T2DD.tag=0;_T2DD.f1=Cyc_target_cflags;_TC3=_T2DD;}{struct Cyc_String_pa_PrintArg_struct _T2DD=_TC3;{struct Cyc_String_pa_PrintArg_struct _T2DE;_T2DE.tag=0;
_T2DE.f1=cppargs_string;_TC4=_T2DE;}{struct Cyc_String_pa_PrintArg_struct _T2DE=_TC4;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;{const char*_T2E0=macrosfile;_TC7=(void*)_T2E0;_TC8=(void*)_T2E0;_TC9=_get_zero_arr_size_char(_TC8,1U);_TC6=_tag_fat(_TC7,sizeof(char),_TC9);}_T2DF.f1=_TC6;_TC5=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_TC5;{struct Cyc_String_pa_PrintArg_struct _T2E0;_T2E0.tag=0;{const char*_T2E1=cppinfile;_TCC=(void*)_T2E1;_TCD=(void*)_T2E1;_TCE=_get_zero_arr_size_char(_TCD,1U);_TCB=_tag_fat(_TCC,sizeof(char),_TCE);}_T2E0.f1=_TCB;_TCA=_T2E0;}{struct Cyc_String_pa_PrintArg_struct _T2E0=_TCA;{struct Cyc_String_pa_PrintArg_struct _T2E1;_T2E1.tag=0;_TD0=Cyc_verbose;
if(!_TD0)goto _TL135;_T2E1.f1=_tag_fat("",sizeof(char),1U);goto _TL136;_TL135: _T2E1.f1=_tag_fat("-w",sizeof(char),3U);_TL136: _TCF=_T2E1;}{struct Cyc_String_pa_PrintArg_struct _T2E1=_TCF;void*_T2E2[6];_T2E2[0]=& _T2DC;_T2E2[1]=& _T2DD;_T2E2[2]=& _T2DE;_T2E2[3]=& _T2DF;_T2E2[4]=& _T2E0;_T2E2[5]=& _T2E1;_TD1=
# 1471
_tag_fat("%s %s %s -E -dM -o %s -x c %s %s",sizeof(char),33U);_TD2=_tag_fat(_T2E2,sizeof(void*),6);_TC1=Cyc_aprintf(_TD1,_TD2);}}}}}}_TD3=_untag_fat_ptr_check_bound(_TC1,sizeof(char),1U);_TD4=_check_null(_TD3);{char*cmd=(char*)_TD4;_TD5=Cyc_verbose;
# 1475
if(!_TD5)goto _TL137;{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{char*_T2DD=cmd;_TD8=(void*)_T2DD;_TD9=(void*)_T2DD;_TDA=_get_zero_arr_size_char(_TD9,1U);_TD7=_tag_fat(_TD8,sizeof(char),_TDA);}
_T2DC.f1=_TD7;_TD6=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_TD6;void*_T2DD[1];_T2DD[0]=& _T2DC;_TDB=Cyc_stderr;_TDC=_tag_fat("%s\n",sizeof(char),4U);_TDD=_tag_fat(_T2DD,sizeof(void*),1);Cyc_fprintf(_TDB,_TDC,_TDD);}goto _TL138;_TL137: _TL138: _TDE=
system(cmd);if(_TDE)goto _TL139;else{goto _TL13B;}
# 1480
_TL13B:{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;
_T2DC.f1=Cyc_cyclone_cc;_TE0=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_TE0;{struct Cyc_String_pa_PrintArg_struct _T2DD;_T2DD.tag=0;_T2DD.f1=Cyc_target_cflags;_TE1=_T2DD;}{struct Cyc_String_pa_PrintArg_struct _T2DD=_TE1;{struct Cyc_String_pa_PrintArg_struct _T2DE;_T2DE.tag=0;
_T2DE.f1=cppargs_string;_TE2=_T2DE;}{struct Cyc_String_pa_PrintArg_struct _T2DE=_TE2;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;{const char*_T2E0=declsfile;_TE5=(void*)_T2E0;_TE6=(void*)_T2E0;_TE7=_get_zero_arr_size_char(_TE6,1U);_TE4=_tag_fat(_TE5,sizeof(char),_TE7);}_T2DF.f1=_TE4;_TE3=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_TE3;{struct Cyc_String_pa_PrintArg_struct _T2E0;_T2E0.tag=0;{const char*_T2E1=cppinfile;_TEA=(void*)_T2E1;_TEB=(void*)_T2E1;_TEC=_get_zero_arr_size_char(_TEB,1U);_TE9=_tag_fat(_TEA,sizeof(char),_TEC);}_T2E0.f1=_TE9;_TE8=_T2E0;}{struct Cyc_String_pa_PrintArg_struct _T2E0=_TE8;{struct Cyc_String_pa_PrintArg_struct _T2E1;_T2E1.tag=0;_TEE=Cyc_verbose;
if(!_TEE)goto _TL13C;_T2E1.f1=_tag_fat("",sizeof(char),1U);goto _TL13D;_TL13C: _T2E1.f1=_tag_fat("-w",sizeof(char),3U);_TL13D: _TED=_T2E1;}{struct Cyc_String_pa_PrintArg_struct _T2E1=_TED;void*_T2E2[6];_T2E2[0]=& _T2DC;_T2E2[1]=& _T2DD;_T2E2[2]=& _T2DE;_T2E2[3]=& _T2DF;_T2E2[4]=& _T2E0;_T2E2[5]=& _T2E1;_TEF=
# 1480
_tag_fat("%s %s %s -E -o %s -x c %s %s",sizeof(char),29U);_TF0=_tag_fat(_T2E2,sizeof(void*),6);_TDF=Cyc_aprintf(_TEF,_TF0);}}}}}}_TF1=_untag_fat_ptr_check_bound(_TDF,sizeof(char),1U);_TF2=_check_null(_TF1);cmd=(char*)_TF2;_TF3=Cyc_verbose;
# 1484
if(!_TF3)goto _TL13E;{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{char*_T2DD=cmd;_TF6=(void*)_T2DD;_TF7=(void*)_T2DD;_TF8=_get_zero_arr_size_char(_TF7,1U);_TF5=_tag_fat(_TF6,sizeof(char),_TF8);}
_T2DC.f1=_TF5;_TF4=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_TF4;void*_T2DD[1];_T2DD[0]=& _T2DC;_TF9=Cyc_stderr;_TFA=_tag_fat("%s\n",sizeof(char),4U);_TFB=_tag_fat(_T2DD,sizeof(void*),1);Cyc_fprintf(_TF9,_TFA,_TFB);}goto _TL13F;_TL13E: _TL13F:
 system(cmd);goto _TL13A;_TL139: _TL13A:;}}_TL127: goto _TL125;_TL124: _TL125: _TFC=
# 1491
Cyc_gathering();if(!_TFC)goto _TL140;{int _T2DC=0;_npop_handler(0);return _T2DC;}_TL140:{
# 1494
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(macrosfile,"r");_TFD=maybe;_TFE=(unsigned)_TFD;
if(_TFE)goto _TL142;else{goto _TL144;}_TL144:{struct Cyc_NO_SUPPORT_exn_struct*_T2DC=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_T2DC->tag=Cyc_NO_SUPPORT;{struct Cyc_String_pa_PrintArg_struct _T2DD;_T2DD.tag=0;{const char*_T2DE=macrosfile;_T103=(void*)_T2DE;_T104=(void*)_T2DE;_T105=_get_zero_arr_size_char(_T104,1U);_T102=_tag_fat(_T103,sizeof(char),_T105);}
_T2DD.f1=_T102;_T101=_T2DD;}{struct Cyc_String_pa_PrintArg_struct _T2DD=_T101;void*_T2DE[1];_T2DE[0]=& _T2DD;_T106=
# 1496
_tag_fat("can't open macrosfile %s",sizeof(char),25U);_T107=_tag_fat(_T2DE,sizeof(void*),1);_T100=Cyc_aprintf(_T106,_T107);}_T2DC->f1=_T100;_TFF=(struct Cyc_NO_SUPPORT_exn_struct*)_T2DC;}_T108=(void*)_TFF;_throw(_T108);goto _TL143;_TL142: _TL143: _T109=Cyc_verbose;
# 1499
if(!_T109)goto _TL145;_T10A=Cyc_stderr;_T10B=_tag_fat("Getting macros...",sizeof(char),18U);_T10C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T10A,_T10B,_T10C);goto _TL146;_TL145: _TL146:
 in_file=maybe;{
struct Cyc_Lexing_lexbuf*l=Cyc_Lexing_from_file(in_file);
struct _tuple13*entry;
_TL147: entry=Cyc_line(l);_T10D=entry;if(_T10D!=0)goto _TL148;else{goto _TL149;}
_TL148:{struct _tuple13*_T2DC=entry;struct Cyc_Set_Set*_T2DD;struct _fat_ptr*_T2DE;{struct _tuple13 _T2DF=*_T2DC;_T2DE=_T2DF.f0;_T2DD=_T2DF.f1;}{struct _fat_ptr*name=_T2DE;struct Cyc_Set_Set*uses=_T2DD;_T10F=Cyc_Hashtable_insert;{
void(*_T2DF)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))_T10F;_T10E=_T2DF;}_T10E(t,name,uses);}}goto _TL147;_TL149:
# 1509
 Cyc_fclose(in_file);_T110=Cyc_verbose;
if(!_T110)goto _TL14A;_T111=Cyc_stderr;_T112=_tag_fat("done.\n",sizeof(char),7U);_T113=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T111,_T112,_T113);goto _TL14B;_TL14A: _TL14B:
# 1513
 maybe=Cyc_fopen(declsfile,"r");_T114=maybe;_T115=(unsigned)_T114;
if(_T115)goto _TL14C;else{goto _TL14E;}_TL14E:{struct Cyc_NO_SUPPORT_exn_struct*_T2DC=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_T2DC->tag=Cyc_NO_SUPPORT;{struct Cyc_String_pa_PrintArg_struct _T2DD;_T2DD.tag=0;{const char*_T2DE=declsfile;_T11A=(void*)_T2DE;_T11B=(void*)_T2DE;_T11C=_get_zero_arr_size_char(_T11B,1U);_T119=_tag_fat(_T11A,sizeof(char),_T11C);}
_T2DD.f1=_T119;_T118=_T2DD;}{struct Cyc_String_pa_PrintArg_struct _T2DD=_T118;void*_T2DE[1];_T2DE[0]=& _T2DD;_T11D=
# 1514
_tag_fat("can't open declsfile %s",sizeof(char),24U);_T11E=_tag_fat(_T2DE,sizeof(void*),1);_T117=Cyc_aprintf(_T11D,_T11E);}_T2DC->f1=_T117;_T116=(struct Cyc_NO_SUPPORT_exn_struct*)_T2DC;}_T11F=(void*)_T116;_throw(_T11F);goto _TL14D;_TL14C: _TL14D: _T120=Cyc_verbose;
# 1517
if(!_T120)goto _TL14F;_T121=Cyc_stderr;_T122=_tag_fat("Extracting declarations...",sizeof(char),27U);_T123=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T121,_T122,_T123);goto _TL150;_TL14F: _TL150:
 in_file=maybe;
l=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(filtereddeclsfile,"w");_T124=Cyc_slurp_out;_T125=(unsigned)_T124;
if(_T125)goto _TL151;else{goto _TL153;}_TL153:{int _T2DC=1;_npop_handler(0);return _T2DC;}_TL151:
 _TL154: _T126=Cyc_slurp(l);if(_T126)goto _TL155;else{goto _TL156;}_TL155: goto _TL154;_TL156: _T127=Cyc_verbose;
if(!_T127)goto _TL157;_T128=Cyc_stderr;_T129=_tag_fat("done.\n",sizeof(char),7U);_T12A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T128,_T129,_T12A);goto _TL158;_TL157: _TL158: {
# 1525
struct Cyc_List_List*x=user_defs;
_TL159: if(x!=0)goto _TL15A;else{goto _TL15B;}
_TL15A: _T12B=x;_T12C=_T12B->hd;{struct _tuple16*_T2DC=(struct _tuple16*)_T12C;struct _fat_ptr*_T2DD;{struct _tuple16 _T2DE=*_T2DC;_T2DD=_T2DE.f1;}{struct _fat_ptr*s=_T2DD;{struct Cyc_String_pa_PrintArg_struct _T2DE;_T2DE.tag=0;_T12E=s;
_T2DE.f1=*_T12E;_T12D=_T2DE;}{struct Cyc_String_pa_PrintArg_struct _T2DE=_T12D;void*_T2DF[1];_T2DF[0]=& _T2DE;_T12F=_check_null(Cyc_slurp_out);_T130=_tag_fat("%s",sizeof(char),3U);_T131=_tag_fat(_T2DF,sizeof(void*),1);Cyc_fprintf(_T12F,_T130,_T131);}_T132=x;
x=_T132->tl;}}goto _TL159;_TL15B:
# 1531
 Cyc_fclose(in_file);_T133=
_check_null(Cyc_slurp_out);Cyc_fclose(_T133);_T134=Cyc_mode;_T135=(int)_T134;
if(_T135==3)goto _TL15C;goto _TL15D;_TL15C: _TL15D:
# 1537
 maybe=Cyc_fopen(filtereddeclsfile,"r");_T136=maybe;_T137=(unsigned)_T136;
if(_T137)goto _TL15E;else{goto _TL160;}_TL160:{int _T2DC=1;_npop_handler(0);return _T2DC;}_TL15E: _T138=Cyc_verbose;
if(!_T138)goto _TL161;_T139=Cyc_stderr;_T13A=_tag_fat("Parsing declarations...",sizeof(char),24U);_T13B=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T139,_T13A,_T13B);goto _TL162;_TL161: _TL162:
 in_file=maybe;{const char*_T2DC=filtereddeclsfile;_T13D=(void*)_T2DC;_T13E=(void*)_T2DC;_T13F=_get_zero_arr_size_char(_T13E,1U);_T13C=_tag_fat(_T13D,sizeof(char),_T13F);}
Cyc_Warn_reset(_T13C);
Cyc_Lex_lex_init(0);
Cyc_Warn_print_warnings=Cyc_verbose;{
struct Cyc_List_List*decls=0;{struct _handler_cons _T2DC;_T140=& _T2DC;_push_handler(_T140);{int _T2DD=0;_T141=setjmp(_T2DC.handler);if(!_T141)goto _TL163;_T2DD=1;goto _TL164;_TL163: _TL164: if(_T2DD)goto _TL165;else{goto _TL167;}_TL167:
# 1546
 decls=Cyc_Parse_parse_file(in_file);
Cyc_Warn_print_warnings=1;
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);_T142=Cyc_verbose;
if(!_T142)goto _TL168;_T143=Cyc_stderr;_T144=_tag_fat("done.\n",sizeof(char),7U);_T145=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T143,_T144,_T145);goto _TL169;_TL168: _TL169: _pop_handler();goto _TL166;_TL165: _T146=Cyc_Core_get_exn_thrown();{void*_T2DE=(void*)_T146;void*_T2DF;_T2DF=_T2DE;{void*x=_T2DF;
# 1554
Cyc_Warn_print_warnings=1;
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);_T147=Cyc_verbose;
if(!_T147)goto _TL16A;_T148=Cyc_stderr;_T149=_tag_fat("exception thrown.\n",sizeof(char),19U);_T14A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T148,_T149,_T14A);goto _TL16B;_TL16A: _TL16B:
 Cyc_Core_rethrow(x);goto _LL6;}_LL6:;}_TL166:;}}{
# 1562
struct Cyc_List_List*d=decls;_TL16F: if(d!=0)goto _TL16D;else{goto _TL16E;}
_TL16D: _T14B=d;_T14C=_T14B->hd;_T14D=(struct Cyc_Absyn_Decl*)_T14C;_T14E=t;Cyc_scan_decl(_T14D,_T14E);_T14F=d;
# 1562
d=_T14F->tl;goto _TL16F;_TL16E:;}_T151=Cyc_List_map;{
# 1566
struct Cyc_List_List*(*_T2DC)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))_T151;_T150=_T2DC;}_T152=Cyc_List_split(user_defs);_T153=_T152.f0;{struct Cyc_List_List*user_symbols=_T150(Cyc_add_user_prefix,_T153);_T154=
Cyc_List_append(start_symbols,user_symbols);_T155=t;{struct Cyc_Set_Set*reachable_set=Cyc_reachable(_T154,_T155);
# 1570
struct Cyc_List_List*reachable_decls=0;
struct Cyc_List_List*user_decls=0;_T157=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T2DC)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T157;_T156=_T2DC;}_T158=Cyc_strptrcmp;{struct Cyc_Set_Set*defined_symbols=_T156(_T158);{
struct Cyc_List_List*d=decls;_TL173: if(d!=0)goto _TL171;else{goto _TL172;}
_TL171: _T159=d;_T15A=_T159->hd;{struct Cyc_Absyn_Decl*decl=(struct Cyc_Absyn_Decl*)_T15A;
struct _fat_ptr*name;_T15B=decl;{
void*_T2DC=_T15B->r;struct Cyc_Absyn_Typedefdecl*_T2DD;struct Cyc_Absyn_Enumdecl*_T2DE;struct Cyc_Absyn_Aggrdecl*_T2DF;struct Cyc_Absyn_Fndecl*_T2E0;struct Cyc_Absyn_Vardecl*_T2E1;_T15C=(int*)_T2DC;_T15D=*_T15C;switch(_T15D){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2DC;_T2E1=_T2E2->f1;}{struct Cyc_Absyn_Vardecl*x=_T2E1;_T15E=x;{
# 1578
struct _tuple1*_T2E2=_T15E->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;_T160=Cyc_Set_insert;{
struct Cyc_Set_Set*(*_T2E4)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T160;_T15F=_T2E4;}defined_symbols=_T15F(defined_symbols,v);_T162=Cyc_List_mem;{
int(*_T2E4)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T162;_T161=_T2E4;}_T163=Cyc_strptrcmp;_T164=omit_symbols;_T165=v;_T166=_T161(_T163,_T164,_T165);if(!_T166)goto _TL175;name=0;goto _TL176;
_TL175: name=v;_TL176: goto _LL9;}}}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T2DC;_T2E0=_T2E2->f1;}{struct Cyc_Absyn_Fndecl*x=_T2E0;_T167=x;{
# 1584
struct _tuple1*_T2E2=_T167->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;_T169=Cyc_Set_insert;{
struct Cyc_Set_Set*(*_T2E4)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T169;_T168=_T2E4;}defined_symbols=_T168(defined_symbols,v);_T16B=Cyc_List_mem;{
int(*_T2E4)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T16B;_T16A=_T2E4;}_T16C=Cyc_strptrcmp;_T16D=omit_symbols;_T16E=v;_T16F=_T16A(_T16C,_T16D,_T16E);if(!_T16F)goto _TL177;name=0;goto _TL178;
_TL177: name=v;_TL178: goto _LL9;}}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T2DC;_T2DF=_T2E2->f1;}{struct Cyc_Absyn_Aggrdecl*x=_T2DF;_T170=x;{
# 1590
struct _tuple1*_T2E2=_T170->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;
name=v;goto _LL9;}}}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T2DC;_T2DE=_T2E2->f1;}{struct Cyc_Absyn_Enumdecl*x=_T2DE;_T171=x;{
# 1594
struct _tuple1*_T2E2=_T171->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;
name=v;
# 1598
if(name==0)goto _TL179;_T173=Cyc_Set_member;{int(*_T2E4)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T173;_T172=_T2E4;}_T174=_T172(reachable_set,name);if(!_T174)goto _TL179;{struct Cyc_List_List*_T2E4=_cycalloc(sizeof(struct Cyc_List_List));
_T2E4->hd=decl;_T2E4->tl=reachable_decls;_T175=(struct Cyc_List_List*)_T2E4;}reachable_decls=_T175;goto _TL17A;
# 1601
_TL179: _T176=x;_T177=_T176->fields;_T178=(unsigned)_T177;if(!_T178)goto _TL17B;_T179=x;_T17A=_T179->fields;_T17B=_T17A->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T17B;_TL180: if(fs!=0)goto _TL17E;else{goto _TL17F;}
_TL17E: _T17C=fs;_T17D=_T17C->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_T17D;_T17E=f;{
struct _tuple1*_T2E4=_T17E->name;struct _fat_ptr*_T2E5;{struct _tuple1 _T2E6=*_T2E4;_T2E5=_T2E6.f1;}{struct _fat_ptr*v=_T2E5;_T180=Cyc_Set_member;{
int(*_T2E6)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T180;_T17F=_T2E6;}_T181=_T17F(reachable_set,v);if(!_T181)goto _TL181;{struct Cyc_List_List*_T2E6=_cycalloc(sizeof(struct Cyc_List_List));
_T2E6->hd=decl;_T2E6->tl=reachable_decls;_T182=(struct Cyc_List_List*)_T2E6;}reachable_decls=_T182;goto _TL17F;_TL181:;}}}_T183=fs;
# 1602
fs=_T183->tl;goto _TL180;_TL17F:;}goto _TL17C;_TL17B: _TL17C: _TL17A:
# 1611
 name=0;goto _LL9;}}}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T2DC;_T2DD=_T2E2->f1;}{struct Cyc_Absyn_Typedefdecl*x=_T2DD;_T184=x;{
# 1614
struct _tuple1*_T2E2=_T184->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;
name=v;goto _LL9;}}}case 13: goto _LL17;case 14: _LL17: goto _LL19;case 15: _LL19: goto _LL1B;case 16: _LL1B: goto _LL1D;case 2: _LL1D: goto _LL1F;case 6: _LL1F: goto _LL21;case 3: _LL21: goto _LL23;case 9: _LL23: goto _LL25;case 10: _LL25: goto _LL27;case 11: _LL27: goto _LL29;case 12: _LL29: goto _LL2B;default: _LL2B:
# 1629
 name=0;goto _LL9;}_LL9:;}
# 1633
if(name==0)goto _TL183;_T186=Cyc_Set_member;{int(*_T2DC)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T186;_T185=_T2DC;}_T187=_T185(reachable_set,name);if(!_T187)goto _TL183;_T188=name;_T189=*_T188;_T18A=Cyc_user_prefix;_T18B=
Cyc_strlen(Cyc_user_prefix);_T18C=Cyc_strncmp(_T189,_T18A,_T18B);if(_T18C==0)goto _TL185;{struct Cyc_List_List*_T2DC=_cycalloc(sizeof(struct Cyc_List_List));
_T2DC->hd=decl;_T2DC->tl=reachable_decls;_T18D=(struct Cyc_List_List*)_T2DC;}reachable_decls=_T18D;goto _TL186;
# 1638
_TL185: Cyc_rename_decl(decl);{struct Cyc_List_List*_T2DC=_cycalloc(sizeof(struct Cyc_List_List));
_T2DC->hd=decl;_T2DC->tl=user_decls;_T18E=(struct Cyc_List_List*)_T2DC;}user_decls=_T18E;_TL186: goto _TL184;_TL183: _TL184:;}_T18F=d;
# 1573
d=_T18F->tl;goto _TL173;_TL172:;}_T190=Cyc_do_setjmp;
# 1645
if(_T190)goto _TL187;else{goto _TL189;}
_TL189: maybe=Cyc_fopen(filename,"w");_T191=maybe;_T192=(unsigned)_T191;
if(_T192)goto _TL18A;else{goto _TL18C;}_TL18C:{int _T2DC=1;_npop_handler(0);return _T2DC;}_TL18A:
 out_file=maybe;goto _TL188;
_TL187: out_file=Cyc_stdout;_TL188:{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{const char*_T2DD=filename;_T196=(void*)_T2DD;_T197=(void*)_T2DD;_T198=_get_zero_arr_size_char(_T197,1U);_T195=_tag_fat(_T196,sizeof(char),_T198);}
_T2DC.f1=_T195;_T194=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T194;void*_T2DD[1];_T2DD[0]=& _T2DC;_T199=_tag_fat("_%s_",sizeof(char),5U);_T19A=_tag_fat(_T2DD,sizeof(void*),1);_T193=Cyc_aprintf(_T199,_T19A);}{struct _fat_ptr ifdefmacro=_T193;{
int j=0;_TL190: _T19B=j;_T19C=(unsigned)_T19B;_T19D=ifdefmacro;_T19E=_get_fat_size(_T19D,sizeof(char));if(_T19C < _T19E)goto _TL18E;else{goto _TL18F;}
_TL18E: _T19F=ifdefmacro;_T1A0=_T19F.curr;_T1A1=(char*)_T1A0;_T1A2=_check_null(_T1A1);_T1A3=j;_T1A4=_T1A2[_T1A3];_T1A5=(int)_T1A4;if(_T1A5==46)goto _TL193;else{goto _TL194;}_TL194: _T1A6=ifdefmacro;_T1A7=_T1A6.curr;_T1A8=(char*)_T1A7;_T1A9=j;_T1AA=_T1A8[_T1A9];_T1AB=(int)_T1AA;if(_T1AB==47)goto _TL193;else{goto _TL191;}
_TL193: _T1AC=ifdefmacro;_T1AD=j;{struct _fat_ptr _T2DC=_fat_ptr_plus(_T1AC,sizeof(char),_T1AD);_T1AE=_T2DC.curr;_T1AF=(char*)_T1AE;{char _T2DD=*_T1AF;char _T2DE='_';_T1B0=_get_fat_size(_T2DC,sizeof(char));if(_T1B0!=1U)goto _TL195;if(_T2DD!=0)goto _TL195;if(_T2DE==0)goto _TL195;_throw_arraybounds();goto _TL196;_TL195: _TL196: _T1B1=_T2DC.curr;_T1B2=(char*)_T1B1;*_T1B2=_T2DE;}}goto _TL192;
_TL191: _T1B3=ifdefmacro;_T1B4=_T1B3.curr;_T1B5=(char*)_T1B4;_T1B6=j;_T1B7=_T1B5[_T1B6];_T1B8=(int)_T1B7;if(_T1B8==95)goto _TL197;_T1B9=ifdefmacro;_T1BA=_T1B9.curr;_T1BB=(char*)_T1BA;_T1BC=j;_T1BD=_T1BB[_T1BC];_T1BE=(int)_T1BD;if(_T1BE==47)goto _TL197;_T1BF=ifdefmacro;_T1C0=j;{struct _fat_ptr _T2DC=_fat_ptr_plus(_T1BF,sizeof(char),_T1C0);_T1C1=_T2DC.curr;_T1C2=(char*)_T1C1;{char _T2DD=*_T1C2;_T1C3=ifdefmacro;_T1C4=_T1C3.curr;_T1C5=(char*)_T1C4;_T1C6=j;_T1C7=_T1C5[_T1C6];_T1C8=(int)_T1C7;_T1C9=
toupper(_T1C8);{char _T2DE=(char)_T1C9;_T1CA=_get_fat_size(_T2DC,sizeof(char));if(_T1CA!=1U)goto _TL199;if(_T2DD!=0)goto _TL199;if(_T2DE==0)goto _TL199;_throw_arraybounds();goto _TL19A;_TL199: _TL19A: _T1CB=_T2DC.curr;_T1CC=(char*)_T1CB;*_T1CC=_T2DE;}}}goto _TL198;_TL197: _TL198: _TL192:
# 1651
 j=j + 1;goto _TL190;_TL18F:;}_T1CD=Cyc_do_setjmp;
# 1657
if(_T1CD)goto _TL19B;else{goto _TL19D;}
_TL19D: _T1CE=out_file;_T1CF=_tag_fat("__noinference__{\n",sizeof(char),18U);_T1D0=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T1CE,_T1CF,_T1D0);goto _TL19C;_TL19B: _TL19C:{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;
# 1662
_T2DC.f1=ifdefmacro;_T1D1=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T1D1;{struct Cyc_String_pa_PrintArg_struct _T2DD;_T2DD.tag=0;
_T2DD.f1=ifdefmacro;_T1D2=_T2DD;}{struct Cyc_String_pa_PrintArg_struct _T2DD=_T1D2;void*_T2DE[2];_T2DE[0]=& _T2DC;_T2DE[1]=& _T2DD;_T1D3=out_file;_T1D4=
# 1661
_tag_fat("#ifndef %s\n#define %s\n",sizeof(char),23U);_T1D5=_tag_fat(_T2DE,sizeof(void*),2);Cyc_fprintf(_T1D3,_T1D4,_T1D5);}}{
# 1666
struct Cyc_List_List*print_decls=0;
struct Cyc_List_List*names=0;{
struct Cyc_List_List*d=reachable_decls;_TL1A1: if(d!=0)goto _TL19F;else{goto _TL1A0;}
_TL19F: _T1D6=d;_T1D7=_T1D6->hd;{struct Cyc_Absyn_Decl*decl=(struct Cyc_Absyn_Decl*)_T1D7;
int anon_enum=0;
struct _fat_ptr*name;_T1D8=decl;{
void*_T2DC=_T1D8->r;struct Cyc_Absyn_Typedefdecl*_T2DD;struct Cyc_Absyn_Enumdecl*_T2DE;struct Cyc_Absyn_Aggrdecl*_T2DF;struct Cyc_Absyn_Fndecl*_T2E0;struct Cyc_Absyn_Vardecl*_T2E1;_T1D9=(int*)_T2DC;_T1DA=*_T1D9;switch(_T1DA){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2DC;_T2E1=_T2E2->f1;}{struct Cyc_Absyn_Vardecl*x=_T2E1;_T1DB=x;{
# 1674
struct _tuple1*_T2E2=_T1DB->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;
name=v;goto _LL3E;}}}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T2DC;_T2E0=_T2E2->f1;}{struct Cyc_Absyn_Fndecl*x=_T2E0;_T1DC=x;_T1DD=_T1DC->is_inline;
# 1678
if(!_T1DD)goto _TL1A3;name=0;goto _LL3E;_TL1A3: _T1DE=x;{
struct _tuple1*_T2E2=_T1DE->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;
name=v;goto _LL3E;}}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T2DC;_T2DF=_T2E2->f1;}{struct Cyc_Absyn_Aggrdecl*x=_T2DF;_T1DF=x;{
# 1683
struct _tuple1*_T2E2=_T1DF->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;
name=v;goto _LL3E;}}}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T2DC;_T2DE=_T2E2->f1;}{struct Cyc_Absyn_Enumdecl*x=_T2DE;_T1E0=x;{
# 1687
struct _tuple1*_T2E2=_T1E0->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;
name=v;goto _LL3E;}}}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T2E2=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T2DC;_T2DD=_T2E2->f1;}{struct Cyc_Absyn_Typedefdecl*x=_T2DD;_T1E1=x;{
# 1691
struct _tuple1*_T2E2=_T1E1->name;struct _fat_ptr*_T2E3;{struct _tuple1 _T2E4=*_T2E2;_T2E3=_T2E4.f1;}{struct _fat_ptr*v=_T2E3;
name=v;goto _LL3E;}}}case 4: goto _LL4C;case 13: _LL4C: goto _LL4E;case 14: _LL4E: goto _LL50;case 15: _LL50: goto _LL52;case 16: _LL52: goto _LL54;case 2: _LL54: goto _LL56;case 6: _LL56: goto _LL58;case 3: _LL58: goto _LL5A;case 9: _LL5A: goto _LL5C;case 10: _LL5C: goto _LL5E;case 11: _LL5E: goto _LL60;default: _LL60:
# 1706
 name=0;goto _LL3E;}_LL3E:;}_T1E2=name;_T1E3=(unsigned)_T1E2;
# 1709
if(_T1E3)goto _TL1A5;else{goto _TL1A7;}_TL1A7: _T1E4=anon_enum;if(_T1E4)goto _TL1A5;else{goto _TL1A8;}_TL1A8: goto _TL19E;_TL1A5: _T1E6=Cyc_Set_member;{
# 1714
int(*_T2DC)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T1E6;_T1E5=_T2DC;}_T1E7=reachable_set;_T1E8=Cyc_add_user_prefix(name);_T1E9=_T1E5(_T1E7,_T1E8);if(!_T1E9)goto _TL1A9;{
struct Cyc_Absyn_Decl*user_decl=Cyc_Absyn_lookup_decl(user_decls,name);
if(user_decl!=0)goto _TL1AB;{struct Cyc_Core_Impossible_exn_struct*_T2DC=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T2DC->tag=Cyc_Core_Impossible;
_T2DC->f1=_tag_fat("Internal Error: bad user-def name",sizeof(char),34U);_T1EA=(struct Cyc_Core_Impossible_exn_struct*)_T2DC;}_T1EB=(void*)_T1EA;_throw(_T1EB);goto _TL1AC;
# 1720
_TL1AB: _T1EC=user_decl;{void*_T2DC=_T1EC->r;_T1ED=(int*)_T2DC;_T1EE=*_T1ED;switch(_T1EE){case 0: goto _LL74;case 1: _LL74:{struct Cyc_NO_SUPPORT_exn_struct*_T2DD=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_T2DD->tag=Cyc_NO_SUPPORT;
# 1723
_T2DD->f1=_tag_fat("user defintions for function or variable decls",sizeof(char),47U);_T1EF=(struct Cyc_NO_SUPPORT_exn_struct*)_T2DD;}_T1F0=(void*)_T1EF;_throw(_T1F0);default: goto _LL70;}_LL70:;}_TL1AC:{struct Cyc_List_List*_T2DC=_cycalloc(sizeof(struct Cyc_List_List));
# 1729
_T2DC->hd=decl;_T2DC->tl=print_decls;_T1F1=(struct Cyc_List_List*)_T2DC;}print_decls=_T1F1;}goto _TL1AA;
# 1732
_TL1A9:{struct Cyc_List_List*_T2DC=_cycalloc(sizeof(struct Cyc_List_List));_T2DC->hd=decl;_T2DC->tl=print_decls;_T1F2=(struct Cyc_List_List*)_T2DC;}print_decls=_T1F2;_TL1AA:{struct Cyc_List_List*_T2DC=_cycalloc(sizeof(struct Cyc_List_List));
_T2DC->hd=name;_T2DC->tl=names;_T1F3=(struct Cyc_List_List*)_T2DC;}names=_T1F3;}_TL19E: _T1F4=d;
# 1668
d=_T1F4->tl;goto _TL1A1;_TL1A0:;}{struct _handler_cons _T2DC;_T1F5=& _T2DC;_push_handler(_T1F5);{int _T2DD=0;_T1F6=setjmp(_T2DC.handler);if(!_T1F6)goto _TL1AE;_T2DD=1;goto _TL1AF;_TL1AE: _TL1AF: if(_T2DD)goto _TL1B0;else{goto _TL1B2;}_TL1B2:
# 1738
 Cyc_Binding_resolve_all(print_decls);_T1F7=
Cyc_Tcenv_tc_init();_T1F8=print_decls;Cyc_Tc_tc(_T1F7,1,_T1F8);_pop_handler();goto _TL1B1;_TL1B0: _T1F9=Cyc_Core_get_exn_thrown();{void*_T2DE=(void*)_T1F9;{struct Cyc_NO_SUPPORT_exn_struct*_T2DF=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_T2DF->tag=Cyc_NO_SUPPORT;
# 1741
_T2DF->f1=_tag_fat("can't typecheck acquired declarations",sizeof(char),38U);_T1FA=(struct Cyc_NO_SUPPORT_exn_struct*)_T2DF;}_T1FB=(void*)_T1FA;_throw(_T1FB);;}_TL1B1:;}}{struct _tuple0 _T2DC;
# 1745
_T2DC.f0=print_decls;_T2DC.f1=names;_T1FC=_T2DC;}{struct _tuple0 _T2DC=_T1FC;struct Cyc_List_List*_T2DD;struct Cyc_List_List*_T2DE;_T2DE=_T2DC.f0;_T2DD=_T2DC.f1;{struct Cyc_List_List*d=_T2DE;struct Cyc_List_List*n=_T2DD;
_TL1B6:
# 1745
 if(d!=0)goto _TL1B7;else{goto _TL1B5;}_TL1B7: if(n!=0)goto _TL1B4;else{goto _TL1B5;}
# 1747
_TL1B4: _T1FD=d;_T1FE=_T1FD->hd;{struct Cyc_Absyn_Decl*decl=(struct Cyc_Absyn_Decl*)_T1FE;_T1FF=n;_T200=_T1FF->hd;{
struct _fat_ptr*name=(struct _fat_ptr*)_T200;
int anon_enum=0;_T201=name;_T202=(unsigned)_T201;
if(_T202)goto _TL1B8;else{goto _TL1BA;}
_TL1BA: anon_enum=1;goto _TL1B9;_TL1B8: _TL1B9: _T203=& Cyc_Absynpp_cyc_params_r;_T204=(struct Cyc_Absynpp_Params*)_T203;
# 1754
Cyc_Absynpp_set_params(_T204);_T205=name;_T206=(unsigned)_T205;
if(!_T206)goto _TL1BB;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;_T209=name;
_T2DF.f1=*_T209;_T208=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T208;void*_T2E0[1];_T2E0[0]=& _T2DF;_T20A=_tag_fat("_%s_def_",sizeof(char),9U);_T20B=_tag_fat(_T2E0,sizeof(void*),1);_T207=Cyc_aprintf(_T20A,_T20B);}ifdefmacro=_T207;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;
_T2DF.f1=ifdefmacro;_T20C=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T20C;void*_T2E0[1];_T2E0[0]=& _T2DF;_T20D=out_file;_T20E=_tag_fat("#ifndef %s\n",sizeof(char),12U);_T20F=_tag_fat(_T2E0,sizeof(void*),1);Cyc_fprintf(_T20D,_T20E,_T20F);}{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;
_T2DF.f1=ifdefmacro;_T210=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T210;void*_T2E0[1];_T2E0[0]=& _T2DF;_T211=out_file;_T212=_tag_fat("#define %s\n",sizeof(char),12U);_T213=_tag_fat(_T2E0,sizeof(void*),1);Cyc_fprintf(_T211,_T212,_T213);}{struct Cyc_Absyn_Decl*_T2DF[1];_T2DF[0]=decl;_T215=_tag_fat(_T2DF,sizeof(struct Cyc_Absyn_Decl*),1);_T214=Cyc_List_list(_T215);}_T216=out_file;
# 1760
Cyc_Absynpp_decllist2file(_T214,_T216);_T217=out_file;_T218=
_tag_fat("#endif\n",sizeof(char),8U);_T219=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T217,_T218,_T219);goto _TL1BC;
# 1765
_TL1BB:{struct Cyc_Absyn_Decl*_T2DF[1];_T2DF[0]=decl;_T21B=_tag_fat(_T2DF,sizeof(struct Cyc_Absyn_Decl*),1);_T21A=Cyc_List_list(_T21B);}_T21C=out_file;Cyc_Absynpp_decllist2file(_T21A,_T21C);_TL1BC:;}}_T21D=d;
# 1746
d=_T21D->tl;_T21E=n;n=_T21E->tl;goto _TL1B6;_TL1B5:;}}
# 1770
maybe=Cyc_fopen(macrosfile,"r");_T21F=maybe;_T220=(unsigned)_T21F;
if(_T220)goto _TL1BD;else{goto _TL1BF;}_TL1BF:{struct Cyc_NO_SUPPORT_exn_struct*_T2DC=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_T2DC->tag=Cyc_NO_SUPPORT;{struct Cyc_String_pa_PrintArg_struct _T2DD;_T2DD.tag=0;{const char*_T2DE=macrosfile;_T225=(void*)_T2DE;_T226=(void*)_T2DE;_T227=_get_zero_arr_size_char(_T226,1U);_T224=_tag_fat(_T225,sizeof(char),_T227);}
_T2DD.f1=_T224;_T223=_T2DD;}{struct Cyc_String_pa_PrintArg_struct _T2DD=_T223;void*_T2DE[1];_T2DE[0]=& _T2DD;_T228=
# 1771
_tag_fat("can't open macrosfile %s",sizeof(char),25U);_T229=_tag_fat(_T2DE,sizeof(void*),1);_T222=Cyc_aprintf(_T228,_T229);}_T2DC->f1=_T222;_T221=(struct Cyc_NO_SUPPORT_exn_struct*)_T2DC;}_T22A=(void*)_T221;_throw(_T22A);goto _TL1BE;_TL1BD: _TL1BE:
# 1773
 in_file=maybe;
l=Cyc_Lexing_from_file(in_file);{
struct _tuple14*entry2;
_TL1C0: entry2=Cyc_suck_line(l);_T22B=entry2;if(_T22B!=0)goto _TL1C1;else{goto _TL1C2;}
_TL1C1:{struct _tuple14*_T2DC=entry2;struct _fat_ptr*_T2DD;struct _fat_ptr _T2DE;{struct _tuple14 _T2DF=*_T2DC;_T2DE=_T2DF.f0;_T2DD=_T2DF.f1;}{struct _fat_ptr line=_T2DE;struct _fat_ptr*name=_T2DD;_T22D=Cyc_Set_member;{
int(*_T2DF)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T22D;_T22C=_T2DF;}_T22E=_T22C(reachable_set,name);if(!_T22E)goto _TL1C3;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;_T230=name;
_T2DF.f1=*_T230;_T22F=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T22F;void*_T2E0[1];_T2E0[0]=& _T2DF;_T231=out_file;_T232=_tag_fat("#ifndef %s\n",sizeof(char),12U);_T233=_tag_fat(_T2E0,sizeof(void*),1);Cyc_fprintf(_T231,_T232,_T233);}{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;
_T2DF.f1=line;_T234=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T234;void*_T2E0[1];_T2E0[0]=& _T2DF;_T235=out_file;_T236=_tag_fat("%s\n",sizeof(char),4U);_T237=_tag_fat(_T2E0,sizeof(void*),1);Cyc_fprintf(_T235,_T236,_T237);}_T238=out_file;_T239=
_tag_fat("#endif\n",sizeof(char),8U);_T23A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T238,_T239,_T23A);goto _TL1C4;_TL1C3: _TL1C4:;}}goto _TL1C0;_TL1C2:
# 1784
 Cyc_fclose(in_file);_T23B=Cyc_mode;_T23C=(int)_T23B;
if(_T23C==3)goto _TL1C5;goto _TL1C6;_TL1C5: _TL1C6:
# 1787
 if(hstubs==0)goto _TL1C7;{
struct Cyc_List_List*x=hstubs;_TL1CC: if(x!=0)goto _TL1CA;else{goto _TL1CB;}
_TL1CA: _T23D=x;_T23E=_T23D->hd;{struct _tuple15*_T2DC=(struct _tuple15*)_T23E;struct _fat_ptr _T2DD;struct _fat_ptr _T2DE;{struct _tuple15 _T2DF=*_T2DC;_T2DE=_T2DF.f0;_T2DD=_T2DF.f1;}{struct _fat_ptr symbol=_T2DE;struct _fat_ptr text=_T2DD;_T23F=text;_T240=_T23F.curr;_T241=(char*)_T240;
if(_T241==0)goto _TL1CD;_T242=symbol;_T243=_T242.curr;_T244=(char*)_T243;
if(_T244!=0)goto _TL1CF;_T245=text;_T246=_untag_fat_ptr_check_bound(_T245,sizeof(char),1U);_T247=(const char*)_T246;_T248=out_file;
# 1793
Cyc_fputs(_T247,_T248);goto _TL1D0;
_TL1CF: _T24A=Cyc_Set_member;{int(*_T2DF)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T24A;_T249=_T2DF;}_T24B=defined_symbols;{struct _fat_ptr*_T2DF=_cycalloc(sizeof(struct _fat_ptr));*_T2DF=symbol;_T24C=(struct _fat_ptr*)_T2DF;}_T24D=_T249(_T24B,_T24C);if(!_T24D)goto _TL1D1;_T24E=text;_T24F=_untag_fat_ptr_check_bound(_T24E,sizeof(char),1U);_T250=(const char*)_T24F;_T251=out_file;
Cyc_fputs(_T250,_T251);goto _TL1D2;
# 1797
_TL1D1:{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;_T2DF.f1=symbol;_T252=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T252;void*_T2E0[1];_T2E0[0]=& _T2DF;_T253=_tag_fat("%s is not supported on this platform\n",sizeof(char),38U);_T254=_tag_fat(_T2E0,sizeof(void*),1);Cyc_log(_T253,_T254);}{
struct Cyc_Set_Set*x=defined_symbols;_TL1D6: _T255=Cyc_Set_is_empty(x);if(_T255)goto _TL1D5;else{goto _TL1D4;}
_TL1D4: _T257=Cyc_Set_choose;{struct _fat_ptr*(*_T2DF)(struct Cyc_Set_Set*)=(struct _fat_ptr*(*)(struct Cyc_Set_Set*))_T257;_T256=_T2DF;}{struct _fat_ptr*y=_T256(x);_T259=Cyc_Set_delete;{struct Cyc_Set_Set*(*_T2DF)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T259;_T258=_T2DF;}x=_T258(x,y);{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;_T25B=y;
_T2DF.f1=*_T25B;_T25A=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T25A;void*_T2E0[1];_T2E0[0]=& _T2DF;_T25C=_tag_fat("+%s",sizeof(char),4U);_T25D=_tag_fat(_T2E0,sizeof(void*),1);Cyc_log(_T25C,_T25D);}_T25E=
_tag_fat("\n",sizeof(char),2U);_T25F=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T25E,_T25F);}goto _TL1D6;_TL1D5:;}_TL1D2: _TL1D0: goto _TL1CE;
# 1806
_TL1CD:{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;
_T2DF.f1=symbol;_T260=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T260;void*_T2E0[1];_T2E0[0]=& _T2DF;_T261=
# 1806
_tag_fat("Null text for %s, will not be supported on this platform\n",sizeof(char),58U);_T262=_tag_fat(_T2E0,sizeof(void*),1);Cyc_log(_T261,_T262);}_TL1CE:;}}_T263=x;
# 1788
x=_T263->tl;goto _TL1CC;_TL1CB:;}goto _TL1C8;_TL1C7: _TL1C8: _T264=out_file;_T265=
# 1810
_tag_fat("#endif\n",sizeof(char),8U);_T266=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T264,_T265,_T266);_T267=Cyc_do_setjmp;
if(_T267)goto _TL1D7;else{goto _TL1D9;}
_TL1D9: _T268=out_file;_T269=_tag_fat("}\n",sizeof(char),3U);_T26A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T268,_T269,_T26A);goto _TL1D8;_TL1D7: _TL1D8: _T26B=Cyc_do_setjmp;
if(!_T26B)goto _TL1DA;{int _T2DC=0;_npop_handler(0);return _T2DC;}
_TL1DA: Cyc_fclose(out_file);
# 1817
if(cstubs==0)goto _TL1DC;
out_file=_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*x=cstubs;_TL1E1: if(x!=0)goto _TL1DF;else{goto _TL1E0;}
_TL1DF: _T26C=x;_T26D=_T26C->hd;{struct _tuple15*_T2DC=(struct _tuple15*)_T26D;struct _fat_ptr _T2DD;struct _fat_ptr _T2DE;{struct _tuple15 _T2DF=*_T2DC;_T2DE=_T2DF.f0;_T2DD=_T2DF.f1;}{struct _fat_ptr symbol=_T2DE;struct _fat_ptr text=_T2DD;_T26E=text;_T26F=_T26E.curr;_T270=(char*)_T26F;
if(_T270==0)goto _TL1E2;_T271=symbol;_T272=_T271.curr;_T273=(char*)_T272;if(_T273==0)goto _TL1E4;else{goto _TL1E5;}_TL1E5: _T275=Cyc_Set_member;{
int(*_T2DF)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T275;_T274=_T2DF;}_T276=defined_symbols;{struct _fat_ptr*_T2DF=_cycalloc(sizeof(struct _fat_ptr));*_T2DF=symbol;_T277=(struct _fat_ptr*)_T2DF;}_T278=_T274(_T276,_T277);
# 1821
if(_T278)goto _TL1E4;else{goto _TL1E2;}
# 1823
_TL1E4: _T279=text;_T27A=_untag_fat_ptr_check_bound(_T279,sizeof(char),1U);_T27B=(const char*)_T27A;_T27C=out_file;Cyc_fputs(_T27B,_T27C);goto _TL1E3;_TL1E2: _TL1E3:;}}_T27D=x;
# 1819
x=_T27D->tl;goto _TL1E1;_TL1E0:;}goto _TL1DD;_TL1DC: _TL1DD:
# 1828
 out_file=_check_null(Cyc_cycstubs_file);
if(cycstubs==0)goto _TL1E6;{struct Cyc_String_pa_PrintArg_struct _T2DC;_T2DC.tag=0;{const char*_T2DD=filename;_T280=(void*)_T2DD;_T281=(void*)_T2DD;_T282=_get_zero_arr_size_char(_T281,1U);_T27F=_tag_fat(_T280,sizeof(char),_T282);}
# 1833
_T2DC.f1=_T27F;_T27E=_T2DC;}{struct Cyc_String_pa_PrintArg_struct _T2DC=_T27E;void*_T2DD[1];_T2DD[0]=& _T2DC;_T283=out_file;_T284=_tag_fat("#include <%s>\n\n",sizeof(char),16U);_T285=_tag_fat(_T2DD,sizeof(void*),1);Cyc_fprintf(_T283,_T284,_T285);}
out_file=_check_null(Cyc_cycstubs_file);{
struct Cyc_List_List*x=cycstubs;_TL1EB: if(x!=0)goto _TL1E9;else{goto _TL1EA;}
_TL1E9: _T286=x;_T287=_T286->hd;{struct _tuple15*_T2DC=(struct _tuple15*)_T287;struct _fat_ptr _T2DD;struct _fat_ptr _T2DE;{struct _tuple15 _T2DF=*_T2DC;_T2DE=_T2DF.f0;_T2DD=_T2DF.f1;}{struct _fat_ptr symbol=_T2DE;struct _fat_ptr text=_T2DD;_T288=text;_T289=_T288.curr;_T28A=(char*)_T289;
if(_T28A==0)goto _TL1EC;_T28B=symbol;_T28C=_T28B.curr;_T28D=(char*)_T28C;if(_T28D==0)goto _TL1EE;else{goto _TL1EF;}_TL1EF: _T28F=Cyc_Set_member;{
int(*_T2DF)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T28F;_T28E=_T2DF;}_T290=defined_symbols;{struct _fat_ptr*_T2DF=_cycalloc(sizeof(struct _fat_ptr));*_T2DF=symbol;_T291=(struct _fat_ptr*)_T2DF;}_T292=_T28E(_T290,_T291);
# 1837
if(_T292)goto _TL1EE;else{goto _TL1EC;}
# 1839
_TL1EE: _T293=text;_T294=_untag_fat_ptr_check_bound(_T293,sizeof(char),1U);_T295=(const char*)_T294;_T296=out_file;Cyc_fputs(_T295,_T296);goto _TL1ED;_TL1EC: _TL1ED:;}}_T297=x;
# 1835
x=_T297->tl;goto _TL1EB;_TL1EA:;}_T298=out_file;_T299=
# 1841
_tag_fat("\n",sizeof(char),2U);_T29A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T298,_T299,_T29A);goto _TL1E7;_TL1E6: _TL1E7: {int _T2DC=0;_npop_handler(0);return _T2DC;}}}}}}}}}}}_pop_handler();goto _TL120;_TL11F: _T29B=Cyc_Core_get_exn_thrown();{void*_T2DC=(void*)_T29B;void*_T2DD;struct _fat_ptr _T2DE;_T29C=(struct Cyc_Core_Impossible_exn_struct*)_T2DC;_T29D=_T29C->tag;_T29E=Cyc_Core_Impossible;if(_T29D!=_T29E)goto _TL1F0;{struct Cyc_Core_Impossible_exn_struct*_T2DF=(struct Cyc_Core_Impossible_exn_struct*)_T2DC;_T2DE=_T2DF->f1;}{struct _fat_ptr s=_T2DE;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;
# 1848
_T2DF.f1=s;_T29F=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T29F;void*_T2E0[1];_T2E0[0]=& _T2DF;_T2A0=_tag_fat("Got Core::Impossible(%s)\n",sizeof(char),26U);_T2A1=_tag_fat(_T2E0,sizeof(void*),1);Cyc_log(_T2A0,_T2A1);}goto _LL89;}_TL1F0: _T2A2=(struct Cyc_Dict_Absent_exn_struct*)_T2DC;_T2A3=_T2A2->tag;_T2A4=Cyc_Dict_Absent;if(_T2A3!=_T2A4)goto _TL1F2;_T2A5=
# 1850
_tag_fat("Got Dict::Absent\n",sizeof(char),18U);_T2A6=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T2A5,_T2A6);goto _LL89;_TL1F2: _T2A7=(struct Cyc_Core_Failure_exn_struct*)_T2DC;_T2A8=_T2A7->tag;_T2A9=Cyc_Core_Failure;if(_T2A8!=_T2A9)goto _TL1F4;{struct Cyc_Core_Failure_exn_struct*_T2DF=(struct Cyc_Core_Failure_exn_struct*)_T2DC;_T2DE=_T2DF->f1;}{struct _fat_ptr s=_T2DE;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;
# 1852
_T2DF.f1=s;_T2AA=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T2AA;void*_T2E0[1];_T2E0[0]=& _T2DF;_T2AB=_tag_fat("Got Core::Failure(%s)\n",sizeof(char),23U);_T2AC=_tag_fat(_T2E0,sizeof(void*),1);Cyc_log(_T2AB,_T2AC);}goto _LL89;}_TL1F4: _T2AD=(struct Cyc_Core_Invalid_argument_exn_struct*)_T2DC;_T2AE=_T2AD->tag;_T2AF=Cyc_Core_Invalid_argument;if(_T2AE!=_T2AF)goto _TL1F6;{struct Cyc_Core_Invalid_argument_exn_struct*_T2DF=(struct Cyc_Core_Invalid_argument_exn_struct*)_T2DC;_T2DE=_T2DF->f1;}{struct _fat_ptr s=_T2DE;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;
# 1854
_T2DF.f1=s;_T2B0=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T2B0;void*_T2E0[1];_T2E0[0]=& _T2DF;_T2B1=_tag_fat("Got Invalid_argument(%s)\n",sizeof(char),26U);_T2B2=_tag_fat(_T2E0,sizeof(void*),1);Cyc_log(_T2B1,_T2B2);}goto _LL89;}_TL1F6: _T2B3=(struct Cyc_Core_Not_found_exn_struct*)_T2DC;_T2B4=_T2B3->tag;_T2B5=Cyc_Core_Not_found;if(_T2B4!=_T2B5)goto _TL1F8;_T2B6=
# 1856
_tag_fat("Got Not_found\n",sizeof(char),15U);_T2B7=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T2B6,_T2B7);goto _LL89;_TL1F8: _T2B8=(struct Cyc_NO_SUPPORT_exn_struct*)_T2DC;_T2B9=_T2B8->tag;_T2BA=Cyc_NO_SUPPORT;if(_T2B9!=_T2BA)goto _TL1FA;{struct Cyc_NO_SUPPORT_exn_struct*_T2DF=(struct Cyc_NO_SUPPORT_exn_struct*)_T2DC;_T2DE=_T2DF->f1;}{struct _fat_ptr s=_T2DE;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;
# 1858
_T2DF.f1=s;_T2BB=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T2BB;void*_T2E0[1];_T2E0[0]=& _T2DF;_T2BC=_tag_fat("No support because %s\n",sizeof(char),23U);_T2BD=_tag_fat(_T2E0,sizeof(void*),1);Cyc_log(_T2BC,_T2BD);}goto _LL89;}_TL1FA: _T2BE=(struct Cyc_Lexing_Error_exn_struct*)_T2DC;_T2BF=_T2BE->tag;_T2C0=Cyc_Lexing_Error;if(_T2BF!=_T2C0)goto _TL1FC;{struct Cyc_Lexing_Error_exn_struct*_T2DF=(struct Cyc_Lexing_Error_exn_struct*)_T2DC;_T2DE=_T2DF->f1;}{struct _fat_ptr s=_T2DE;{struct Cyc_String_pa_PrintArg_struct _T2DF;_T2DF.tag=0;
# 1860
_T2DF.f1=s;_T2C1=_T2DF;}{struct Cyc_String_pa_PrintArg_struct _T2DF=_T2C1;void*_T2E0[1];_T2E0[0]=& _T2DF;_T2C2=_tag_fat("Got a lexing error %s\n",sizeof(char),23U);_T2C3=_tag_fat(_T2E0,sizeof(void*),1);Cyc_log(_T2C2,_T2C3);}goto _LL89;}_TL1FC: _T2DD=_T2DC;{void*x=_T2DD;_T2C4=
# 1862
_tag_fat("Got unknown exception\n",sizeof(char),23U);_T2C5=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T2C4,_T2C5);
Cyc_Core_rethrow(x);}_LL89:;}_TL120:;}}
# 1868
maybe=Cyc_fopen(filename,"w");_T2C6=maybe;_T2C7=(unsigned)_T2C6;
if(_T2C7)goto _TL1FE;else{goto _TL200;}
_TL200:{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;{const char*_T2DB=filename;_T2CA=(void*)_T2DB;_T2CB=(void*)_T2DB;_T2CC=_get_zero_arr_size_char(_T2CB,1U);_T2C9=_tag_fat(_T2CA,sizeof(char),_T2CC);}_T2DA.f1=_T2C9;_T2C8=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_T2C8;void*_T2DB[1];_T2DB[0]=& _T2DA;_T2CD=Cyc_stderr;_T2CE=_tag_fat("Error: could not create file %s\n",sizeof(char),33U);_T2CF=_tag_fat(_T2DB,sizeof(void*),1);Cyc_fprintf(_T2CD,_T2CE,_T2CF);}
return 1;_TL1FE:
# 1873
 out_file=maybe;{struct Cyc_String_pa_PrintArg_struct _T2DA;_T2DA.tag=0;{const char*_T2DB=filename;_T2D2=(void*)_T2DB;_T2D3=(void*)_T2DB;_T2D4=_get_zero_arr_size_char(_T2D3,1U);_T2D1=_tag_fat(_T2D2,sizeof(char),_T2D4);}
# 1876
_T2DA.f1=_T2D1;_T2D0=_T2DA;}{struct Cyc_String_pa_PrintArg_struct _T2DA=_T2D0;void*_T2DB[1];_T2DB[0]=& _T2DA;_T2D5=out_file;_T2D6=
# 1875
_tag_fat("#error -- %s is not supported on this platform\n",sizeof(char),48U);_T2D7=_tag_fat(_T2DB,sizeof(void*),1);Cyc_fprintf(_T2D5,_T2D6,_T2D7);}
# 1877
Cyc_fclose(out_file);_T2D8=
# 1880
_tag_fat("Not supported on this platform\n",sizeof(char),32U);_T2D9=_tag_fat(0U,sizeof(void*),0);Cyc_log(_T2D8,_T2D9);
# 1887
return 0;}}}}}}}
# 1891
int Cyc_process_specfile(const char*file,const char*dir){int _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct _fat_ptr _T2;void*_T3;void*_T4;unsigned _T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc___cycFILE*_T9;unsigned _TA;struct Cyc_String_pa_PrintArg_struct _TB;struct _fat_ptr _TC;void*_TD;void*_TE;unsigned _TF;struct Cyc___cycFILE*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;char*_T13;char*_T14;unsigned _T15;unsigned _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;unsigned _T19;enum Cyc_buildlib_mode _T1A;int _T1B;int _T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct _fat_ptr _T1E;void*_T1F;void*_T20;unsigned _T21;struct Cyc___cycFILE*_T22;struct _fat_ptr _T23;struct _fat_ptr _T24;enum Cyc_buildlib_mode _T25;int _T26;struct _fat_ptr _T27;struct Cyc_String_pa_PrintArg_struct _T28;struct Cyc_String_pa_PrintArg_struct _T29;struct _fat_ptr _T2A;struct _fat_ptr _T2B;int _T2C;struct Cyc_String_pa_PrintArg_struct _T2D;struct Cyc___cycFILE*_T2E;struct _fat_ptr _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;unsigned char*_T32;unsigned char*_T33;const char*_T34;struct _tuple17*_T35;struct _fat_ptr _T36;unsigned char*_T37;unsigned char*_T38;const char*_T39;struct Cyc_List_List*_T3A;struct Cyc_List_List*_T3B;struct Cyc_List_List*_T3C;struct Cyc_List_List*_T3D;struct Cyc_List_List*_T3E;struct Cyc_List_List*_T3F;struct Cyc_List_List*_T40;int _T41;enum Cyc_buildlib_mode _T42;int _T43;struct _fat_ptr _T44;unsigned char*_T45;unsigned char*_T46;char*_T47;int _T48;struct Cyc_String_pa_PrintArg_struct _T49;struct Cyc___cycFILE*_T4A;struct _fat_ptr _T4B;struct _fat_ptr _T4C;_T0=Cyc_verbose;
if(!_T0)goto _TL201;{struct Cyc_String_pa_PrintArg_struct _T4D;_T4D.tag=0;{const char*_T4E=file;_T3=(void*)_T4E;_T4=(void*)_T4E;_T5=_get_zero_arr_size_char(_T4,1U);_T2=_tag_fat(_T3,sizeof(char),_T5);}
_T4D.f1=_T2;_T1=_T4D;}{struct Cyc_String_pa_PrintArg_struct _T4D=_T1;void*_T4E[1];_T4E[0]=& _T4D;_T6=Cyc_stderr;_T7=_tag_fat("Processing %s\n",sizeof(char),15U);_T8=_tag_fat(_T4E,sizeof(void*),1);Cyc_fprintf(_T6,_T7,_T8);}goto _TL202;_TL201: _TL202: {
struct Cyc___cycFILE*maybe=Cyc_fopen(file,"r");_T9=maybe;_TA=(unsigned)_T9;
if(_TA)goto _TL203;else{goto _TL205;}
_TL205:{struct Cyc_String_pa_PrintArg_struct _T4D;_T4D.tag=0;{const char*_T4E=file;_TD=(void*)_T4E;_TE=(void*)_T4E;_TF=_get_zero_arr_size_char(_TE,1U);_TC=_tag_fat(_TD,sizeof(char),_TF);}_T4D.f1=_TC;_TB=_T4D;}{struct Cyc_String_pa_PrintArg_struct _T4D=_TB;void*_T4E[1];_T4E[0]=& _T4D;_T10=Cyc_stderr;_T11=_tag_fat("Error: could not open %s\n",sizeof(char),26U);_T12=_tag_fat(_T4E,sizeof(void*),1);Cyc_fprintf(_T10,_T11,_T12);}
return 1;_TL203: {
# 1899
struct Cyc___cycFILE*in_file=maybe;{unsigned _T4D=1024U + 1U;_T15=_check_times(_T4D,sizeof(char));{char*_T4E=_cycalloc_atomic(_T15);{unsigned _T4F=_T4D;unsigned i;i=0;_TL209: if(i < _T4F)goto _TL207;else{goto _TL208;}_TL207: _T16=i;
# 1903
_T4E[_T16]='\000';i=i + 1;goto _TL209;_TL208: _T4E[_T4F]=0;}_T14=(char*)_T4E;}_T13=_T14;}{struct _fat_ptr buf=_tag_fat(_T13,sizeof(char),1025U);_T17=buf;_T18=buf;_T19=
_get_fat_size(_T18,sizeof(char));{struct _fat_ptr cwd=Cyc_getcwd(_T17,_T19);_T1A=Cyc_mode;_T1B=(int)_T1A;
if(_T1B==2)goto _TL20A;_T1C=
chdir(dir);if(!_T1C)goto _TL20C;{struct Cyc_String_pa_PrintArg_struct _T4D;_T4D.tag=0;{const char*_T4E=dir;_T1F=(void*)_T4E;_T20=(void*)_T4E;_T21=_get_zero_arr_size_char(_T20,1U);_T1E=_tag_fat(_T1F,sizeof(char),_T21);}
_T4D.f1=_T1E;_T1D=_T4D;}{struct Cyc_String_pa_PrintArg_struct _T4D=_T1D;void*_T4E[1];_T4E[0]=& _T4D;_T22=Cyc_stderr;_T23=_tag_fat("Error: can't change directory to %s\n",sizeof(char),37U);_T24=_tag_fat(_T4E,sizeof(void*),1);Cyc_fprintf(_T22,_T23,_T24);}
return 1;_TL20C: goto _TL20B;_TL20A: _TL20B: _T25=Cyc_mode;_T26=(int)_T25;
# 1911
if(_T26!=1)goto _TL20E;{struct Cyc_String_pa_PrintArg_struct _T4D;_T4D.tag=0;
# 1914
_T4D.f1=Cyc_cyclone_cc;_T28=_T4D;}{struct Cyc_String_pa_PrintArg_struct _T4D=_T28;{struct Cyc_String_pa_PrintArg_struct _T4E;_T4E.tag=0;_T4E.f1=Cyc_target_cflags;_T29=_T4E;}{struct Cyc_String_pa_PrintArg_struct _T4E=_T29;void*_T4F[2];_T4F[0]=& _T4D;_T4F[1]=& _T4E;_T2A=
# 1913
_tag_fat("echo | %s %s -E -dM - -o INITMACROS.h\n",sizeof(char),39U);_T2B=_tag_fat(_T4F,sizeof(void*),2);_T27=Cyc_aprintf(_T2A,_T2B);}}{struct _fat_ptr cmd=_T27;_T2C=Cyc_verbose;
# 1915
if(!_T2C)goto _TL210;{struct Cyc_String_pa_PrintArg_struct _T4D;_T4D.tag=0;
_T4D.f1=cmd;_T2D=_T4D;}{struct Cyc_String_pa_PrintArg_struct _T4D=_T2D;void*_T4E[1];_T4E[0]=& _T4D;_T2E=Cyc_stderr;_T2F=_tag_fat("%s\n",sizeof(char),4U);_T30=_tag_fat(_T4E,sizeof(void*),1);Cyc_fprintf(_T2E,_T2F,_T30);}goto _TL211;_TL210: _TL211: _T31=cmd;_T32=_untag_fat_ptr(_T31,sizeof(char),1U);_T33=_check_null(_T32);_T34=(const char*)_T33;
system(_T34);}goto _TL20F;_TL20E: _TL20F: {
# 1920
struct Cyc_Lexing_lexbuf*l=Cyc_Lexing_from_file(in_file);
struct _tuple17*entry;
_TL212: entry=Cyc_spec(l);_T35=entry;if(_T35!=0)goto _TL213;else{goto _TL214;}
_TL213:{struct _tuple17*_T4D=entry;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;struct Cyc_List_List*_T50;struct Cyc_List_List*_T51;struct Cyc_List_List*_T52;struct Cyc_List_List*_T53;struct Cyc_List_List*_T54;struct _fat_ptr _T55;{struct _tuple17 _T56=*_T4D;_T55=_T56.f0;_T54=_T56.f1;_T53=_T56.f2;_T52=_T56.f3;_T51=_T56.f4;_T50=_T56.f5;_T4F=_T56.f6;_T4E=_T56.f7;}{struct _fat_ptr headerfile=_T55;struct Cyc_List_List*start_symbols=_T54;struct Cyc_List_List*user_defs=_T53;struct Cyc_List_List*omit_symbols=_T52;struct Cyc_List_List*hstubs=_T51;struct Cyc_List_List*cstubs=_T50;struct Cyc_List_List*cycstubs=_T4F;struct Cyc_List_List*cpp_insert=_T4E;_T36=headerfile;_T37=_untag_fat_ptr(_T36,sizeof(char),1U);_T38=_check_null(_T37);_T39=(const char*)_T38;_T3A=start_symbols;_T3B=user_defs;_T3C=omit_symbols;_T3D=hstubs;_T3E=cstubs;_T3F=cycstubs;_T40=cpp_insert;_T41=
# 1925
Cyc_process_file(_T39,_T3A,_T3B,_T3C,_T3D,_T3E,_T3F,_T40);if(!_T41)goto _TL215;
# 1927
return 1;_TL215:;}}goto _TL212;_TL214:
# 1929
 Cyc_fclose(in_file);_T42=Cyc_mode;_T43=(int)_T42;
# 1931
if(_T43==2)goto _TL217;_T44=cwd;_T45=_untag_fat_ptr(_T44,sizeof(char),1U);_T46=_check_null(_T45);_T47=(char*)_T46;_T48=
chdir(_T47);if(!_T48)goto _TL219;{struct Cyc_String_pa_PrintArg_struct _T4D;_T4D.tag=0;
_T4D.f1=cwd;_T49=_T4D;}{struct Cyc_String_pa_PrintArg_struct _T4D=_T49;void*_T4E[1];_T4E[0]=& _T4D;_T4A=Cyc_stderr;_T4B=_tag_fat("Error: could not change directory to %s\n",sizeof(char),41U);_T4C=_tag_fat(_T4E,sizeof(void*),1);Cyc_fprintf(_T4A,_T4B,_T4C);}
return 1;_TL219: goto _TL218;_TL217: _TL218:
# 1937
 return 0;}}}}}}
# 1941
int Cyc_process_setjmp(const char*dir){char*_T0;char*_T1;unsigned _T2;unsigned _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;unsigned _T6;int _T7;struct Cyc_String_pa_PrintArg_struct _T8;struct _fat_ptr _T9;void*_TA;void*_TB;unsigned _TC;struct Cyc___cycFILE*_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_List_List*_T10;struct _fat_ptr*_T11;struct _fat_ptr _T12;struct Cyc_List_List*_T13;struct _tuple15*_T14;struct _fat_ptr _T15;int _T16;struct _fat_ptr _T17;unsigned char*_T18;unsigned char*_T19;char*_T1A;int _T1B;struct Cyc_String_pa_PrintArg_struct _T1C;struct Cyc___cycFILE*_T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;{unsigned _T20=1024U + 1U;_T2=_check_times(_T20,sizeof(char));{char*_T21=_cycalloc_atomic(_T2);{unsigned _T22=_T20;unsigned i;i=0;_TL21E: if(i < _T22)goto _TL21C;else{goto _TL21D;}_TL21C: _T3=i;
# 1944
_T21[_T3]='\000';i=i + 1;goto _TL21E;_TL21D: _T21[_T22]=0;}_T1=(char*)_T21;}_T0=_T1;}{struct _fat_ptr buf=_tag_fat(_T0,sizeof(char),1025U);_T4=buf;_T5=buf;_T6=
_get_fat_size(_T5,sizeof(char));{struct _fat_ptr cwd=Cyc_getcwd(_T4,_T6);_T7=
chdir(dir);if(!_T7)goto _TL21F;{struct Cyc_String_pa_PrintArg_struct _T20;_T20.tag=0;{const char*_T21=dir;_TA=(void*)_T21;_TB=(void*)_T21;_TC=_get_zero_arr_size_char(_TB,1U);_T9=_tag_fat(_TA,sizeof(char),_TC);}
_T20.f1=_T9;_T8=_T20;}{struct Cyc_String_pa_PrintArg_struct _T20=_T8;void*_T21[1];_T21[0]=& _T20;_TD=Cyc_stderr;_TE=_tag_fat("Error: can't change directory to %s\n",sizeof(char),37U);_TF=_tag_fat(_T21,sizeof(void*),1);Cyc_fprintf(_TD,_TE,_TF);}
return 1;_TL21F:{struct _fat_ptr*_T20[1];{struct _fat_ptr*_T21=_cycalloc(sizeof(struct _fat_ptr));
# 1950
*_T21=_tag_fat("jmp_buf",sizeof(char),8U);_T11=(struct _fat_ptr*)_T21;}_T20[0]=_T11;_T12=_tag_fat(_T20,sizeof(struct _fat_ptr*),1);_T10=Cyc_List_list(_T12);}{struct _tuple15*_T20[1];{struct _tuple15*_T21=_cycalloc(sizeof(struct _tuple15));
_T21->f0=_tag_fat("setjmp",sizeof(char),7U);_T21->f1=_tag_fat("extern int setjmp(jmp_buf);\n",sizeof(char),29U);_T14=(struct _tuple15*)_T21;}_T20[0]=_T14;_T15=_tag_fat(_T20,sizeof(struct _tuple15*),1);_T13=Cyc_List_list(_T15);}_T16=
# 1950
Cyc_process_file("setjmp.h",_T10,0,0,_T13,0,0,0);if(!_T16)goto _TL221;
# 1953
return 1;_TL221: _T17=cwd;_T18=_untag_fat_ptr(_T17,sizeof(char),1U);_T19=_check_null(_T18);_T1A=(char*)_T19;_T1B=
chdir(_T1A);if(!_T1B)goto _TL223;{struct Cyc_String_pa_PrintArg_struct _T20;_T20.tag=0;
_T20.f1=cwd;_T1C=_T20;}{struct Cyc_String_pa_PrintArg_struct _T20=_T1C;void*_T21[1];_T21[0]=& _T20;_T1D=Cyc_stderr;_T1E=_tag_fat("Error: could not change directory to %s\n",sizeof(char),41U);_T1F=_tag_fat(_T21,sizeof(void*),1);Cyc_fprintf(_T1D,_T1E,_T1F);}
return 1;_TL223:
# 1958
 return 0;}}}static char _TmpG3[13U]="BUILDLIB.OUT";
# 1962
static struct _fat_ptr Cyc_output_dir={(unsigned char*)_TmpG3,(unsigned char*)_TmpG3,(unsigned char*)_TmpG3 + 13U};
static void Cyc_set_output_dir(struct _fat_ptr s){
Cyc_output_dir=s;}
# 1966
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _fat_ptr s){struct Cyc_List_List*_T0;struct _fat_ptr _T1;unsigned char*_T2;unsigned char*_T3;{struct Cyc_List_List*_T4=_cycalloc(sizeof(struct Cyc_List_List));_T1=s;_T2=_untag_fat_ptr_check_bound(_T1,sizeof(char),1U);_T3=_check_null(_T2);
_T4->hd=(const char*)_T3;_T4->tl=Cyc_spec_files;_T0=(struct Cyc_List_List*)_T4;}Cyc_spec_files=_T0;}
# 1970
static int Cyc_no_other(struct _fat_ptr s){return 0;}
static void Cyc_set_GATHER (void){
Cyc_mode=1U;}
# 1974
static void Cyc_set_GATHERSCRIPT (void){
Cyc_mode=2U;}
# 1977
static void Cyc_set_FINISH (void){
Cyc_mode=3U;}
# 1980
static void Cyc_add_cpparg(struct _fat_ptr s){struct Cyc_List_List*_T0;struct _fat_ptr*_T1;{struct Cyc_List_List*_T2=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T3=_cycalloc(sizeof(struct _fat_ptr));
*_T3=s;_T1=(struct _fat_ptr*)_T3;}_T2->hd=_T1;_T2->tl=Cyc_cppargs;_T0=(struct Cyc_List_List*)_T2;}Cyc_cppargs=_T0;}
# 1983
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _fat_ptr s){struct Cyc_String_pa_PrintArg_struct _T0;struct Cyc___cycFILE*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;{struct Cyc_String_pa_PrintArg_struct _T4;_T4.tag=0;
_T4.f1=s;_T0=_T4;}{struct Cyc_String_pa_PrintArg_struct _T4=_T0;void*_T5[1];_T5[0]=& _T4;_T1=Cyc_stderr;_T2=_tag_fat("Unsupported option %s\n",sizeof(char),23U);_T3=_tag_fat(_T5,sizeof(void*),1);Cyc_fprintf(_T1,_T2,_T3);}
Cyc_badparse=1;}
# 1988
static void Cyc_set_header(struct _fat_ptr s){struct Cyc_Set_Set**_T0;unsigned _T1;struct Cyc_Set_Set**_T2;struct Cyc_Set_Set*(*_T3)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Set_Set*(*_T4)(int(*)(void*,void*));int(*_T5)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Set_Set**_T6;struct Cyc_Set_Set*(*_T7)(struct Cyc_Set_Set*,struct _fat_ptr*);struct Cyc_Set_Set*(*_T8)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_T9;struct Cyc_Set_Set*_TA;struct _fat_ptr*_TB;_T0=Cyc_headers_to_do;_T1=(unsigned)_T0;
if(_T1)goto _TL225;else{goto _TL227;}
_TL227:{struct Cyc_Set_Set**_TC=_cycalloc(sizeof(struct Cyc_Set_Set*));_T4=Cyc_Set_empty;{struct Cyc_Set_Set*(*_TD)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T4;_T3=_TD;}_T5=Cyc_strptrcmp;*_TC=_T3(_T5);_T2=(struct Cyc_Set_Set**)_TC;}Cyc_headers_to_do=_T2;goto _TL226;_TL225: _TL226: _T6=
_check_null(Cyc_headers_to_do);_T8=Cyc_Set_insert;{struct Cyc_Set_Set*(*_TC)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))_T8;_T7=_TC;}_T9=Cyc_headers_to_do;_TA=*_T9;{struct _fat_ptr*_TC=_cycalloc(sizeof(struct _fat_ptr));*_TC=s;_TB=(struct _fat_ptr*)_TC;}*_T6=_T7(_TA,_TB);}
# 1997
extern void GC_blacklist_warn_clear (void);struct _tuple19{struct _fat_ptr f0;int f1;struct _fat_ptr f2;void*f3;struct _fat_ptr f4;};
int Cyc_main(int argc,struct _fat_ptr argv){struct Cyc_List_List*_T0;struct _tuple19*_T1;struct Cyc_Arg_String_spec_Arg_Spec_struct*_T2;struct _tuple19*_T3;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_T4;struct _tuple19*_T5;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_T6;struct _tuple19*_T7;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_T8;struct _tuple19*_T9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_TA;struct _tuple19*_TB;struct Cyc_Arg_String_spec_Arg_Spec_struct*_TC;struct _tuple19*_TD;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_TE;struct _tuple19*_TF;struct Cyc_Arg_String_spec_Arg_Spec_struct*_T10;struct _tuple19*_T11;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_T12;struct _tuple19*_T13;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_T14;struct _fat_ptr _T15;struct Cyc_List_List*_T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct Cyc_List_List*_T19;struct _fat_ptr _T1A;struct _fat_ptr _T1B;int _T1C;int _T1D;int _T1E;int _T1F;enum Cyc_buildlib_mode _T20;int _T21;int _T22;enum Cyc_buildlib_mode _T23;int _T24;struct Cyc_List_List*_T25;struct _fat_ptr _T26;struct _fat_ptr _T27;int _T28;struct Cyc_String_pa_PrintArg_struct _T29;struct Cyc___cycFILE*_T2A;struct _fat_ptr _T2B;struct _fat_ptr _T2C;struct Cyc_List_List*_T2D;struct _fat_ptr _T2E;int _T2F;struct Cyc_String_pa_PrintArg_struct _T30;struct Cyc___cycFILE*_T31;struct _fat_ptr _T32;struct _fat_ptr _T33;struct Cyc_List_List*_T34;struct _fat_ptr _T35;struct _fat_ptr _T36;unsigned char*_T37;unsigned _T38;int _T39;struct Cyc_String_pa_PrintArg_struct _T3A;struct Cyc___cycFILE*_T3B;struct _fat_ptr _T3C;struct _fat_ptr _T3D;enum Cyc_buildlib_mode _T3E;int _T3F;int _T40;struct Cyc___cycFILE*_T41;struct _fat_ptr _T42;struct _fat_ptr _T43;struct Cyc___cycFILE*_T44;unsigned _T45;struct Cyc___cycFILE*_T46;struct _fat_ptr _T47;struct _fat_ptr _T48;struct _fat_ptr _T49;struct _fat_ptr _T4A;struct _fat_ptr _T4B;struct _fat_ptr _T4C;int _T4D;int _T4E;struct Cyc_String_pa_PrintArg_struct _T4F;struct Cyc___cycFILE*_T50;struct _fat_ptr _T51;struct _fat_ptr _T52;int _T53;struct Cyc_String_pa_PrintArg_struct _T54;struct Cyc___cycFILE*_T55;struct _fat_ptr _T56;struct _fat_ptr _T57;enum Cyc_buildlib_mode _T58;int _T59;struct Cyc_String_pa_PrintArg_struct _T5A;struct _fat_ptr _T5B;struct _fat_ptr _T5C;struct Cyc_String_pa_PrintArg_struct _T5D;struct _fat_ptr _T5E;struct _fat_ptr _T5F;int _T60;struct _fat_ptr _T61;struct _fat_ptr _T62;struct _fat_ptr _T63;unsigned char*_T64;unsigned char*_T65;const char*_T66;struct Cyc___cycFILE*_T67;unsigned _T68;struct Cyc_String_pa_PrintArg_struct _T69;struct Cyc___cycFILE*_T6A;struct _fat_ptr _T6B;struct _fat_ptr _T6C;int _T6D;struct _fat_ptr _T6E;struct _fat_ptr _T6F;struct _fat_ptr _T70;unsigned char*_T71;unsigned char*_T72;const char*_T73;struct Cyc___cycFILE*_T74;unsigned _T75;struct Cyc_String_pa_PrintArg_struct _T76;struct Cyc___cycFILE*_T77;struct _fat_ptr _T78;struct _fat_ptr _T79;struct _fat_ptr _T7A;struct _fat_ptr _T7B;struct _fat_ptr _T7C;unsigned char*_T7D;unsigned char*_T7E;const char*_T7F;struct Cyc___cycFILE*_T80;unsigned _T81;struct Cyc_String_pa_PrintArg_struct _T82;struct Cyc___cycFILE*_T83;struct _fat_ptr _T84;struct _fat_ptr _T85;struct Cyc___cycFILE*_T86;struct _fat_ptr _T87;struct _fat_ptr _T88;struct _fat_ptr _T89;unsigned char*_T8A;unsigned char*_T8B;int _T8C;int _T8D;struct Cyc_List_List*_T8E;void*_T8F;const char*_T90;const char*_T91;int _T92;struct Cyc___cycFILE*_T93;struct _fat_ptr _T94;struct _fat_ptr _T95;struct Cyc_List_List*_T96;enum Cyc_buildlib_mode _T97;int _T98;struct Cyc___cycFILE*_T99;int _T9A;struct Cyc___cycFILE*_T9B;int _T9C;struct Cyc___cycFILE*_T9D;struct Cyc___cycFILE*_T9E;
GC_blacklist_warn_clear();{struct _tuple19*_T9F[10];{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2002
_TA0->f0=_tag_fat("-d",sizeof(char),3U);_TA0->f1=0;_TA0->f2=_tag_fat(" <file>",sizeof(char),8U);{struct Cyc_Arg_String_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_TA1->tag=5;
_TA1->f1=Cyc_set_output_dir;_T2=(struct Cyc_Arg_String_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_T2;
_TA0->f4=_tag_fat("Set the output directory to <file>",sizeof(char),35U);_T1=(struct _tuple19*)_TA0;}
# 2002
_T9F[0]=_T1;{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2005
_TA0->f0=_tag_fat("-gather",sizeof(char),8U);_TA0->f1=0;_TA0->f2=_tag_fat("",sizeof(char),1U);{struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_Arg_Spec_struct));_TA1->tag=0;
_TA1->f1=Cyc_set_GATHER;_T4=(struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_T4;
_TA0->f4=_tag_fat("Gather C library info but don't produce Cyclone headers",sizeof(char),56U);_T3=(struct _tuple19*)_TA0;}
# 2005
_T9F[1]=_T3;{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2008
_TA0->f0=_tag_fat("-gatherscript",sizeof(char),14U);_TA0->f1=0;_TA0->f2=_tag_fat("",sizeof(char),1U);{struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_Arg_Spec_struct));_TA1->tag=0;
_TA1->f1=Cyc_set_GATHERSCRIPT;_T6=(struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_T6;
_TA0->f4=_tag_fat("Produce a script to gather C library info",sizeof(char),42U);_T5=(struct _tuple19*)_TA0;}
# 2008
_T9F[2]=_T5;{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2011
_TA0->f0=_tag_fat("-finish",sizeof(char),8U);_TA0->f1=0;_TA0->f2=_tag_fat("",sizeof(char),1U);{struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_Arg_Spec_struct));_TA1->tag=0;
_TA1->f1=Cyc_set_FINISH;_T8=(struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_T8;
_TA0->f4=_tag_fat("Produce Cyclone headers from pre-gathered C library info",sizeof(char),57U);_T7=(struct _tuple19*)_TA0;}
# 2011
_T9F[3]=_T7;{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2014
_TA0->f0=_tag_fat("-setjmp",sizeof(char),8U);_TA0->f1=0;_TA0->f2=_tag_fat("",sizeof(char),1U);{struct Cyc_Arg_Set_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_Arg_Spec_struct));_TA1->tag=3;
_TA1->f1=& Cyc_do_setjmp;_TA=(struct Cyc_Arg_Set_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_TA;
# 2019
_TA0->f4=_tag_fat("Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",sizeof(char),186U);_T9=(struct _tuple19*)_TA0;}
# 2014
_T9F[4]=_T9;{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2020
_TA0->f0=_tag_fat("-b",sizeof(char),3U);_TA0->f1=0;_TA0->f2=_tag_fat(" <machine>",sizeof(char),11U);{struct Cyc_Arg_String_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_TA1->tag=5;
_TA1->f1=Cyc_Specsfile_set_target_arch;_TC=(struct Cyc_Arg_String_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_TC;
_TA0->f4=_tag_fat("Set the target machine for compilation to <machine>",sizeof(char),52U);_TB=(struct _tuple19*)_TA0;}
# 2020
_T9F[5]=_TB;{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2023
_TA0->f0=_tag_fat("-B",sizeof(char),3U);_TA0->f1=1;_TA0->f2=_tag_fat("<file>",sizeof(char),7U);{struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_TA1->tag=1;
_TA1->f1=Cyc_Specsfile_add_cyclone_exec_path;_TE=(struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_TE;
_TA0->f4=_tag_fat("Add to the list of directories to search for compiler files",sizeof(char),60U);_TD=(struct _tuple19*)_TA0;}
# 2023
_T9F[6]=_TD;{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2026
_TA0->f0=_tag_fat("-h",sizeof(char),3U);_TA0->f1=0;_TA0->f2=_tag_fat(" <header>",sizeof(char),10U);{struct Cyc_Arg_String_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_TA1->tag=5;
_TA1->f1=Cyc_set_header;_T10=(struct Cyc_Arg_String_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_T10;
_TA0->f4=_tag_fat("Produce this header, and other -h headers only",sizeof(char),47U);_TF=(struct _tuple19*)_TA0;}
# 2026
_T9F[7]=_TF;{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2029
_TA0->f0=_tag_fat("-v",sizeof(char),3U);_TA0->f1=0;_TA0->f2=_tag_fat("",sizeof(char),1U);{struct Cyc_Arg_Set_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_Arg_Spec_struct));_TA1->tag=3;
_TA1->f1=& Cyc_verbose;_T12=(struct Cyc_Arg_Set_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_T12;
_TA0->f4=_tag_fat("Verbose operation",sizeof(char),18U);_T11=(struct _tuple19*)_TA0;}
# 2029
_T9F[8]=_T11;{struct _tuple19*_TA0=_cycalloc(sizeof(struct _tuple19));
# 2032
_TA0->f0=_tag_fat("-",sizeof(char),2U);_TA0->f1=1;_TA0->f2=_tag_fat("",sizeof(char),1U);{struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_TA1=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_TA1->tag=1;
_TA1->f1=Cyc_add_cpparg;_T14=(struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_TA1;}_TA0->f3=(void*)_T14;
_TA0->f4=_tag_fat("",sizeof(char),1U);_T13=(struct _tuple19*)_TA0;}
# 2032
_T9F[9]=_T13;_T15=_tag_fat(_T9F,sizeof(struct _tuple19*),10);_T0=Cyc_List_list(_T15);}{
# 2001
struct Cyc_List_List*options=_T0;_T16=options;_T17=
# 2037
_tag_fat("Options:",sizeof(char),9U);_T18=argv;{struct _fat_ptr otherargs=Cyc_Specsfile_parse_b(_T16,Cyc_add_spec_file,Cyc_no_other,_T17,_T18);
# 2039
Cyc_Arg_current=0;_T19=options;_T1A=
_tag_fat("Options:",sizeof(char),9U);_T1B=otherargs;Cyc_Arg_parse(_T19,Cyc_add_spec_file,Cyc_no_other,_T1A,_T1B);_T1C=Cyc_badparse;
if(_T1C)goto _TL22A;else{goto _TL22E;}_TL22E: _T1D=Cyc_do_setjmp;if(_T1D)goto _TL22D;else{goto _TL22F;}_TL22F: if(Cyc_spec_files==0)goto _TL22A;else{goto _TL22D;}_TL22D: _T1E=Cyc_do_setjmp;if(_T1E)goto _TL230;else{goto _TL22C;}_TL230: if(Cyc_spec_files!=0)goto _TL22A;else{goto _TL22C;}_TL22C: _T1F=Cyc_do_setjmp;if(_T1F)goto _TL231;else{goto _TL22B;}_TL231: _T20=Cyc_mode;_T21=(int)_T20;if(_T21==1)goto _TL22A;else{goto _TL22B;}_TL22B: _T22=Cyc_do_setjmp;if(_T22)goto _TL232;else{goto _TL228;}_TL232: _T23=Cyc_mode;_T24=(int)_T23;if(_T24==2)goto _TL22A;else{goto _TL228;}
# 2046
_TL22A: _T25=options;_T26=
# 2048
_tag_fat("Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",sizeof(char),59U);
# 2046
Cyc_Arg_usage(_T25,_T26);
# 2049
return 1;_TL228: _T27=
# 2055
_tag_fat("cycspecs",sizeof(char),9U);{struct _fat_ptr specs_file=Cyc_Specsfile_find_in_arch_path(_T27);_T28=Cyc_verbose;
if(!_T28)goto _TL233;{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;_T9F.f1=specs_file;_T29=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T29;void*_TA0[1];_TA0[0]=& _T9F;_T2A=Cyc_stderr;_T2B=_tag_fat("Reading from specs file %s\n",sizeof(char),28U);_T2C=_tag_fat(_TA0,sizeof(void*),1);Cyc_fprintf(_T2A,_T2B,_T2C);}goto _TL234;_TL233: _TL234: {
struct Cyc_List_List*specs=Cyc_Specsfile_read_specs(specs_file);_T2D=specs;_T2E=
_tag_fat("cyclone_target_cflags",sizeof(char),22U);Cyc_target_cflags=Cyc_Specsfile_get_spec(_T2D,_T2E);_T2F=Cyc_verbose;
if(!_T2F)goto _TL235;{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;_T9F.f1=Cyc_target_cflags;_T30=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T30;void*_TA0[1];_TA0[0]=& _T9F;_T31=Cyc_stderr;_T32=_tag_fat("Target cflags are %s\n",sizeof(char),22U);_T33=_tag_fat(_TA0,sizeof(void*),1);Cyc_fprintf(_T31,_T32,_T33);}goto _TL236;_TL235: _TL236: _T34=specs;_T35=
_tag_fat("cyclone_cc",sizeof(char),11U);Cyc_cyclone_cc=Cyc_Specsfile_get_spec(_T34,_T35);_T36=Cyc_cyclone_cc;_T37=_T36.curr;_T38=(unsigned)_T37;
if(_T38)goto _TL237;else{goto _TL239;}_TL239: Cyc_cyclone_cc=_tag_fat("gcc",sizeof(char),4U);goto _TL238;_TL237: _TL238: _T39=Cyc_verbose;
if(!_T39)goto _TL23A;{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;_T9F.f1=Cyc_cyclone_cc;_T3A=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T3A;void*_TA0[1];_TA0[0]=& _T9F;_T3B=Cyc_stderr;_T3C=_tag_fat("C compiler is %s\n",sizeof(char),18U);_T3D=_tag_fat(_TA0,sizeof(void*),1);Cyc_fprintf(_T3B,_T3C,_T3D);}goto _TL23B;_TL23A: _TL23B: _T3E=Cyc_mode;_T3F=(int)_T3E;
# 2064
if(_T3F!=2)goto _TL23C;_T40=Cyc_verbose;
if(!_T40)goto _TL23E;_T41=Cyc_stderr;_T42=
_tag_fat("Creating BUILDLIB.sh\n",sizeof(char),22U);_T43=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T41,_T42,_T43);goto _TL23F;_TL23E: _TL23F:
 Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");_T44=Cyc_script_file;_T45=(unsigned)_T44;
if(_T45)goto _TL240;else{goto _TL242;}
_TL242: _T46=Cyc_stderr;_T47=_tag_fat("Could not create file BUILDLIB.sh\n",sizeof(char),35U);_T48=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T46,_T47,_T48);
exit(1);goto _TL241;_TL240: _TL241: _T49=
# 2072
_tag_fat("#!/bin/sh\n",sizeof(char),11U);_T4A=_tag_fat(0U,sizeof(void*),0);Cyc_prscript(_T49,_T4A);_T4B=
_tag_fat("GCC=\"gcc\"\n",sizeof(char),11U);_T4C=_tag_fat(0U,sizeof(void*),0);Cyc_prscript(_T4B,_T4C);goto _TL23D;_TL23C: _TL23D: _T4D=
# 2077
Cyc_force_directory_prefixes(Cyc_output_dir);if(_T4D)goto _TL245;else{goto _TL246;}_TL246: _T4E=Cyc_force_directory(Cyc_output_dir);if(_T4E)goto _TL245;else{goto _TL243;}
_TL245:{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;_T9F.f1=Cyc_output_dir;_T4F=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T4F;void*_TA0[1];_TA0[0]=& _T9F;_T50=Cyc_stderr;_T51=_tag_fat("Error: could not create directory %s\n",sizeof(char),38U);_T52=_tag_fat(_TA0,sizeof(void*),1);Cyc_fprintf(_T50,_T51,_T52);}
return 1;_TL243: _T53=Cyc_verbose;
# 2081
if(!_T53)goto _TL247;{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;
_T9F.f1=Cyc_output_dir;_T54=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T54;void*_TA0[1];_TA0[0]=& _T9F;_T55=Cyc_stderr;_T56=_tag_fat("Output directory is %s\n",sizeof(char),24U);_T57=_tag_fat(_TA0,sizeof(void*),1);Cyc_fprintf(_T55,_T56,_T57);}goto _TL248;_TL247: _TL248: _T58=Cyc_mode;_T59=(int)_T58;
# 2084
if(_T59!=2)goto _TL249;{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;
_T9F.f1=Cyc_output_dir;_T5A=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T5A;void*_TA0[1];_TA0[0]=& _T9F;_T5B=_tag_fat("cd %s\n",sizeof(char),7U);_T5C=_tag_fat(_TA0,sizeof(void*),1);Cyc_prscript(_T5B,_T5C);}{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;
# 2087
_T9F.f1=Cyc_target_cflags;_T5D=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T5D;void*_TA0[1];_TA0[0]=& _T9F;_T5E=
# 2086
_tag_fat("echo | $GCC %s -E -dM - -o INITMACROS.h\n",sizeof(char),41U);_T5F=_tag_fat(_TA0,sizeof(void*),1);Cyc_prscript(_T5E,_T5F);}goto _TL24A;_TL249: _TL24A: _T60=
# 2090
Cyc_gathering();if(_T60)goto _TL24B;else{goto _TL24D;}
# 2093
_TL24D: _T61=Cyc_output_dir;_T62=_tag_fat("BUILDLIB.LOG",sizeof(char),13U);_T63=Cyc_Filename_concat(_T61,_T62);_T64=_untag_fat_ptr_check_bound(_T63,sizeof(char),1U);_T65=_check_null(_T64);_T66=(const char*)_T65;Cyc_log_file=Cyc_fopen(_T66,"w");_T67=Cyc_log_file;_T68=(unsigned)_T67;
if(_T68)goto _TL24E;else{goto _TL250;}
_TL250:{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;_T9F.f1=Cyc_output_dir;_T69=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T69;void*_TA0[1];_TA0[0]=& _T9F;_T6A=Cyc_stderr;_T6B=_tag_fat("Error: could not create log file in directory %s\n",sizeof(char),50U);_T6C=_tag_fat(_TA0,sizeof(void*),1);Cyc_fprintf(_T6A,_T6B,_T6C);}
return 1;_TL24E: _T6D=Cyc_do_setjmp;
# 2099
if(_T6D)goto _TL251;else{goto _TL253;}
# 2101
_TL253: _T6E=Cyc_output_dir;_T6F=_tag_fat("cstubs.c",sizeof(char),9U);_T70=Cyc_Filename_concat(_T6E,_T6F);_T71=_untag_fat_ptr_check_bound(_T70,sizeof(char),1U);_T72=_check_null(_T71);_T73=(const char*)_T72;Cyc_cstubs_file=Cyc_fopen(_T73,"w");_T74=Cyc_cstubs_file;_T75=(unsigned)_T74;
if(_T75)goto _TL254;else{goto _TL256;}
_TL256:{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;_T9F.f1=Cyc_output_dir;_T76=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T76;void*_TA0[1];_TA0[0]=& _T9F;_T77=Cyc_stderr;_T78=_tag_fat("Error: could not create cstubs.c in directory %s\n",sizeof(char),50U);_T79=_tag_fat(_TA0,sizeof(void*),1);Cyc_fprintf(_T77,_T78,_T79);}
return 1;_TL254: _T7A=Cyc_output_dir;_T7B=
# 2108
_tag_fat("cycstubs.cyc",sizeof(char),13U);_T7C=Cyc_Filename_concat(_T7A,_T7B);_T7D=_untag_fat_ptr_check_bound(_T7C,sizeof(char),1U);_T7E=_check_null(_T7D);_T7F=(const char*)_T7E;Cyc_cycstubs_file=Cyc_fopen(_T7F,"w");_T80=Cyc_cycstubs_file;_T81=(unsigned)_T80;
if(_T81)goto _TL257;else{goto _TL259;}
_TL259:{struct Cyc_String_pa_PrintArg_struct _T9F;_T9F.tag=0;
# 2112
_T9F.f1=Cyc_output_dir;_T82=_T9F;}{struct Cyc_String_pa_PrintArg_struct _T9F=_T82;void*_TA0[1];_TA0[0]=& _T9F;_T83=Cyc_stderr;_T84=
# 2111
_tag_fat("Error: could not create cycstubs.c in directory %s\n",sizeof(char),52U);_T85=_tag_fat(_TA0,sizeof(void*),1);Cyc_fprintf(_T83,_T84,_T85);}
# 2113
return 1;_TL257: _T86=Cyc_cycstubs_file;_T87=
# 2117
_tag_fat("#include <core.h>\nusing Core;\n\n",sizeof(char),32U);_T88=_tag_fat(0U,sizeof(void*),0);
# 2115
Cyc_fprintf(_T86,_T87,_T88);goto _TL252;_TL251: _TL252: goto _TL24C;_TL24B: _TL24C: _T89=Cyc_output_dir;_T8A=_untag_fat_ptr_check_bound(_T89,sizeof(char),1U);_T8B=_check_null(_T8A);{
# 2122
const char*outdir=(const char*)_T8B;_T8C=Cyc_do_setjmp;
if(!_T8C)goto _TL25A;_T8D=Cyc_process_setjmp(outdir);if(!_T8D)goto _TL25A;
return 1;
# 2128
_TL25A: _TL25F: if(Cyc_spec_files!=0)goto _TL25D;else{goto _TL25E;}
_TL25D: _T8E=Cyc_spec_files;_T8F=_T8E->hd;_T90=(const char*)_T8F;_T91=outdir;_T92=Cyc_process_specfile(_T90,_T91);if(!_T92)goto _TL260;_T93=Cyc_stderr;_T94=
_tag_fat("FATAL ERROR -- QUIT!\n",sizeof(char),22U);_T95=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T93,_T94,_T95);
exit(1);goto _TL261;_TL260: _TL261: _T96=Cyc_spec_files;
# 2128
Cyc_spec_files=_T96->tl;goto _TL25F;_TL25E: _T97=Cyc_mode;_T98=(int)_T97;
# 2136
if(_T98!=2)goto _TL262;_T99=
_check_null(Cyc_script_file);Cyc_fclose(_T99);goto _TL263;
# 2139
_TL262: _T9A=Cyc_gathering();if(_T9A)goto _TL264;else{goto _TL266;}
_TL266: _T9B=_check_null(Cyc_log_file);Cyc_fclose(_T9B);_T9C=Cyc_do_setjmp;
if(_T9C)goto _TL267;else{goto _TL269;}
_TL269: _T9D=_check_null(Cyc_cstubs_file);Cyc_fclose(_T9D);_T9E=
_check_null(Cyc_cycstubs_file);Cyc_fclose(_T9E);goto _TL268;_TL267: _TL268: goto _TL265;_TL264: _TL265: _TL263:
# 2147
 return 0;}}}}}}

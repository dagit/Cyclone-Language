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
extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _fat_ptr);
# 27 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 168
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 529 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 536
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 554
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 568
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 1165 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1223
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);struct _tuple12{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;};
# 1229
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1237
struct _tuple1*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
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
# 77 "buildlib.cyl"
extern void Cyc_Lex_lex_init(int);static char _TmpG0[4U]="gcc";
# 87
static struct _fat_ptr Cyc_cyclone_cc={_TmpG0,_TmpG0,_TmpG0 + 4U};static char _TmpG1[1U]="";
static struct _fat_ptr Cyc_target_cflags={_TmpG1,_TmpG1,_TmpG1 + 1U};
# 90
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
# 93
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 97
int Cyc_log(struct _fat_ptr fmt,struct _fat_ptr ap){
# 100
if(Cyc_log_file==0){
Cyc_fprintf(Cyc_stderr,_tag_fat("Internal error: log file is NULL\n",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
exit(1);}{
# 104
int x=Cyc_vfprintf(Cyc_log_file,fmt,ap);
Cyc_fflush(_check_null(Cyc_log_file));
return x;}}
# 109
static struct _fat_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _fat_ptr*sptr){
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;_Tmp2;})(*_check_null(Cyc_current_targets),sptr);*_Tmp0=_Tmp1;});_Tmp0;});}
# 115
static struct Cyc_Set_Set**Cyc_headers_to_do=0;struct _tuple13{struct _fat_ptr*f0;struct Cyc_Set_Set*f1;};
# 119
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 125
struct Cyc___cycFILE*Cyc_slurp_out=0;
# 127
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
# 129
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple14{struct _fat_ptr f0;struct _fat_ptr*f1;};
# 133
struct _tuple14*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _fat_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple15{struct _fat_ptr f0;struct _fat_ptr f1;};struct _tuple16{struct _fat_ptr*f0;struct _fat_ptr*f1;};struct _tuple17{struct _fat_ptr f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;};
# 149
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
# 170
int Cyc_numdef=0;
# 172
static struct Cyc_List_List*Cyc_cppargs=0;static char _TmpG2[14U]="BUILDLIB_sym_";
# 174
struct _fat_ptr Cyc_user_prefix={_TmpG2,_TmpG2,_TmpG2 + 14U};
static struct _fat_ptr*Cyc_add_user_prefix(struct _fat_ptr*symbol){
struct _fat_ptr s=Cyc_strconcat(Cyc_user_prefix,*symbol);
struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));*_Tmp0=s;return _Tmp0;}
# 179
static struct _fat_ptr Cyc_remove_user_prefix(struct _fat_ptr symbol){
unsigned prefix_len=Cyc_strlen(Cyc_user_prefix);
if(Cyc_strncmp(symbol,Cyc_user_prefix,prefix_len)!=0){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=symbol;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Internal error: bad user type name %s\n",sizeof(char),39U),_tag_fat(_Tmp1,sizeof(void*),1));});
return symbol;}
# 185
return({struct _fat_ptr _Tmp0=symbol;int _Tmp1=(int)prefix_len;Cyc_substring(_Tmp0,_Tmp1,({unsigned long _Tmp2=Cyc_strlen(symbol);_Tmp2 - prefix_len;}));});}
# 188
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 191
({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_remove_user_prefix(*(*ad->name).f1);*_Tmp3=_Tmp4;});_Tmp3;});(*ad->name).f1=_Tmp2;});goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
# 193
({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_remove_user_prefix(*(*ed->name).f1);*_Tmp3=_Tmp4;});_Tmp3;});(*ed->name).f1=_Tmp2;});goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 195
({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_remove_user_prefix(*(*td->name).f1);*_Tmp3=_Tmp4;});_Tmp3;});(*td->name).f1=_Tmp2;});goto _LL0;}default:
# 197
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error in .cys file: bad user-defined type definition\n",sizeof(char),54U),_tag_fat(0U,sizeof(void*),0));
exit(1);}_LL0:;}
# 204
const int Cyc_lex_base[444U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 23,1161,1166,273,752,253,253,270,262,276,286,284,289,283,297,300,- 20,286,290,296,306,315,299,- 22,308,314,311,324,331,333,- 16,313,329,366,395,407,402,404,420,423,409,406,412,436,447,- 18,438,437,431,444,440,456,434,456,459,448,453,444,444,- 21,446,450,460,484,466,468,484,491,492,4,6,21,519,520,507,506,519,548,556,557,24,570,571,24,20,573,590,53,646,648,651,597,600,589,589,590,607,608,602,612,614,631,632,1,689,697,689,690,632,632,639,640,2,697,644,645,1078,670,671,672,673,646,644,659,665,666,721,732,733,- 15,680,681,737,765,767,714,716,797,798,811,- 12,759,760,815,817,818,765,766,821,823,824,- 13,1275,771,772,769,768,767,762,764,767,779,777,790,1254,782,783,781,821,1368,1280,824,825,815,817,854,867,1480,859,860,858,871,1485,- 7,- 8,8,1373,2570,9,930,2594,2632,1103,1389,- 49,1260,- 2,925,- 4,926,1111,1365,927,1104,1119,1451,928,2659,2702,932,976,930,1018,2772,931,1053,- 36,- 42,- 37,2847,- 28,942,- 40,- 25,978,- 27,- 45,- 39,- 48,2922,2951,1561,996,1007,1577,2961,2991,1676,1692,3024,3055,3093,1077,1087,3163,3201,1111,1123,1144,1200,1192,1202,- 6,- 34,995,3133,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1029,3241,1043,1045,1793,1073,1078,1121,1127,1129,1130,1134,1139,1143,3314,3398,- 17,1810,1187,- 19,- 24,- 41,- 38,- 35,- 26,1504,3480,1195,3563,1225,15,1168,1172,1172,1174,1172,1182,1255};
const int Cyc_lex_backtrk[444U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,18,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[444U]={- 1,- 1,- 1,337,326,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,248,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,406,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3820U]={0,0,0,0,0,0,0,0,0,0,22,19,28,436,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,247,256,437,213,22,- 1,- 1,22,- 1,- 1,48,214,46,233,22,434,434,434,434,434,434,434,434,434,434,31,106,22,215,117,42,224,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,31,227,228,231,434,135,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,432,432,432,432,432,432,432,432,432,432,124,20,77,22,70,57,58,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,59,60,61,62,432,63,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,64,65,132,338,339,338,338,339,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,338,340,341,75,78,342,343,344,48,48,345,346,48,347,348,349,350,351,351,351,351,351,351,351,351,351,352,79,353,354,355,48,19,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,19,- 1,- 1,357,356,80,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,327,358,104,328,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,329,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,203,189,30,30,30,30,30,30,30,30,69,69,183,174,69,76,76,167,160,76,155,81,81,156,157,81,69,69,330,158,69,159,161,69,137,126,146,162,76,76,76,163,164,76,81,165,166,168,27,69,169,31,170,21,83,83,171,172,83,173,175,176,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,177,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,178,92,179,180,181,182,100,100,36,184,100,185,19,107,107,115,115,107,186,115,92,187,108,108,95,95,108,35,95,100,115,115,188,190,115,191,107,192,115,193,194,195,196,197,198,108,199,95,200,201,202,94,204,115,205,206,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,207,208,209,210,211,212,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,216,217,218,219,109,220,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,221,50,222,223,33,33,33,33,33,33,33,33,33,33,103,103,225,226,103,229,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,232,- 1,49,- 1,234,235,104,104,236,52,104,237,238,251,239,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,240,241,242,243,- 1,244,- 1,43,43,245,246,43,248,250,94,252,253,254,255,53,257,249,258,259,54,55,289,284,278,273,43,56,142,142,142,142,142,142,142,142,265,266,267,268,269,270,44,44,44,44,44,44,44,44,44,44,271,272,274,275,36,276,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,277,105,105,279,44,280,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,281,282,45,35,35,35,35,35,35,35,35,148,149,150,283,151,285,286,287,152,288,96,290,291,292,37,293,294,153,154,308,303,299,300,301,302,102,304,38,39,39,39,39,39,39,39,39,39,39,305,306,307,309,310,311,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,312,321,316,317,39,318,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,- 1,22,33,33,33,33,33,33,33,33,33,33,319,320,322,323,324,325,31,33,33,33,33,33,33,335,335,335,335,335,335,335,335,430,429,405,396,371,366,361,41,41,41,41,41,41,41,41,41,41,365,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,367,368,369,370,41,399,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,426,41,41,41,41,41,41,41,41,41,41,363,364,36,159,36,425,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,202,36,359,36,41,188,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,260,403,46,427,261,31,31,31,31,31,31,31,31,262,105,263,35,35,400,404,283,46,294,49,313,313,428,272,313,295,295,360,173,295,401,402,422,47,47,47,47,47,47,47,47,47,47,313,35,35,31,264,295,31,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,166,21,31,102,47,31,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,314,102,46,307,307,296,21,307,438,431,315,431,431,439,440,297,102,395,395,441,298,46,442,443,295,295,307,21,295,313,313,0,431,313,19,0,0,47,47,47,47,47,47,47,47,47,47,295,102,395,395,0,313,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,0,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,296,- 1,81,312,312,314,0,312,336,336,297,0,336,0,0,315,0,0,0,0,0,81,0,0,336,336,312,0,336,0,- 1,336,0,21,31,0,0,82,82,82,82,82,82,82,82,82,82,336,0,21,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,407,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,320,320,83,0,320,325,325,0,397,325,398,398,398,398,398,398,398,398,398,398,0,83,0,320,431,0,431,431,325,0,0,28,0,0,0,0,35,82,82,82,82,82,82,82,82,82,82,431,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,0,377,92,377,0,0,378,378,378,378,378,378,378,378,378,378,0,0,- 1,0,0,92,378,378,378,378,378,378,378,378,378,378,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,381,95,381,0,0,382,382,382,382,382,382,382,382,382,382,0,0,0,0,0,95,382,382,382,382,382,382,382,382,382,382,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,166,0,100,0,0,0,0,0,0,423,423,423,423,423,423,423,423,166,0,0,0,100,0,0,0,0,424,424,424,424,424,424,424,424,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,331,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,0,0,0,0,0,0,0,0,332,332,332,332,332,332,332,332,0,0,0,0,0,0,0,0,0,141,141,141,141,141,141,0,334,334,334,334,334,334,334,334,334,334,0,0,0,0,0,0,0,334,334,334,334,334,334,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,334,334,334,334,334,334,334,334,334,334,333,334,334,334,334,334,334,334,334,334,334,334,334,0,0,372,0,383,383,383,383,383,383,383,383,384,384,0,0,0,0,0,0,0,0,0,0,0,374,334,334,334,334,334,334,385,0,0,0,0,0,0,0,0,386,0,0,387,372,0,373,373,373,373,373,373,373,373,373,373,374,0,0,0,0,0,0,385,0,0,0,374,0,0,0,0,386,0,375,387,0,0,0,0,0,0,0,376,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,374,0,0,0,0,0,0,375,0,0,0,0,0,0,0,0,376,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,0,0,0,0,362,0,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,0,0,0,0,362,0,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,379,379,379,379,379,379,379,379,379,379,0,0,0,0,0,0,0,0,0,0,0,380,96,0,0,0,0,372,96,373,373,373,373,373,373,373,373,373,373,378,378,378,378,378,378,378,378,378,378,0,374,0,0,380,96,0,0,375,0,0,96,94,0,0,0,0,376,94,0,379,379,379,379,379,379,379,379,379,379,0,0,0,374,0,0,0,0,0,0,375,380,96,0,94,0,0,0,96,376,94,0,0,382,382,382,382,382,382,382,382,382,382,0,0,0,0,0,0,0,0,0,0,380,96,96,0,0,0,0,96,96,372,0,383,383,383,383,383,383,383,383,384,384,0,0,0,0,0,0,0,0,0,0,0,374,0,96,0,0,0,0,393,96,0,0,0,0,0,0,372,394,384,384,384,384,384,384,384,384,384,384,0,0,0,0,0,374,0,0,0,0,0,374,393,0,0,0,0,0,391,0,0,394,0,0,0,0,0,392,0,0,398,398,398,398,398,398,398,398,398,398,0,0,0,374,0,0,0,0,0,0,391,380,96,0,0,0,0,0,96,392,388,388,388,388,388,388,388,388,388,388,0,0,0,0,0,0,0,388,388,388,388,388,388,380,96,0,0,0,0,0,96,0,0,0,0,0,0,0,388,388,388,388,388,388,388,388,388,388,0,388,388,388,388,388,388,388,388,388,388,388,388,0,0,0,408,0,389,0,0,409,0,0,0,0,0,390,0,0,410,410,410,410,410,410,410,410,0,388,388,388,388,388,388,411,0,0,0,0,389,0,0,0,0,0,0,0,0,390,0,0,0,0,0,0,0,0,0,0,0,0,0,0,412,0,0,0,0,413,414,0,0,0,415,0,0,0,0,0,0,0,416,0,0,0,417,0,418,0,419,0,420,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,143,0,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,19,0,0,433,0,0,0,432,432,432,432,432,432,432,432,432,432,0,0,0,0,0,0,0,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,0,0,0,0,432,0,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,435,0,0,0,0,0,0,0,434,434,434,434,434,434,434,434,434,434,0,0,0,0,0,0,0,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,0,0,0,0,434,0,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3820U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,328,331,43,43,46,46,43,436,46,- 1,- 1,- 1,- 1,- 1,123,246,255,0,212,10,12,42,10,12,42,43,212,46,213,20,1,1,1,1,1,1,1,1,1,1,38,51,124,214,10,38,223,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,226,227,230,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,149,27,27,27,27,27,27,27,27,68,68,150,151,68,75,75,152,153,75,154,80,80,155,156,80,69,69,4,157,69,158,160,68,6,8,5,161,75,76,76,162,163,76,80,164,165,167,17,69,168,27,169,7,83,83,170,171,83,172,174,175,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,176,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,177,91,178,179,180,181,99,99,182,183,99,184,76,107,107,108,108,107,185,108,91,186,13,13,95,95,13,83,95,99,115,115,187,189,115,190,107,191,108,192,193,194,195,196,197,13,198,95,199,200,201,4,203,115,204,205,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,206,207,208,209,210,211,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,215,216,217,218,13,219,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,220,14,221,222,32,32,32,32,32,32,32,32,32,32,103,103,224,225,103,228,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,231,106,232,113,233,234,104,104,235,14,104,236,236,237,238,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,239,240,241,242,114,243,135,15,15,244,245,15,247,249,250,251,252,253,254,14,256,248,257,258,14,14,260,261,262,263,15,14,139,139,139,139,139,139,139,139,264,265,266,267,268,269,15,15,15,15,15,15,15,15,15,15,270,271,273,274,103,275,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,276,104,277,278,15,279,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,280,281,15,142,142,142,142,142,142,142,142,147,147,147,282,147,284,285,286,147,287,288,289,290,291,16,292,293,147,147,296,297,298,299,300,301,302,303,16,16,16,16,16,16,16,16,16,16,16,304,305,306,308,309,310,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,311,314,315,316,16,317,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,248,16,33,33,33,33,33,33,33,33,33,33,318,319,321,322,323,324,15,33,33,33,33,33,33,332,332,332,332,332,332,332,332,340,342,345,349,352,354,357,39,39,39,39,39,39,39,39,39,39,364,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,353,353,353,367,39,397,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,406,41,41,41,41,41,41,41,41,41,41,355,355,375,408,376,409,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,411,375,358,376,41,412,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,259,346,44,343,259,335,335,335,335,335,335,335,335,259,413,259,385,386,347,346,414,44,415,416,144,144,343,417,144,145,145,358,418,145,347,347,419,44,44,44,44,44,44,44,44,44,44,144,385,386,389,259,145,390,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,424,433,389,391,44,390,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,391,47,307,307,145,435,307,437,338,144,338,338,438,439,145,392,393,394,440,145,47,441,442,295,295,307,443,295,313,313,- 1,338,313,307,- 1,- 1,47,47,47,47,47,47,47,47,47,47,295,392,393,394,- 1,313,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,- 1,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,295,344,81,312,312,313,- 1,312,329,329,295,- 1,329,- 1,- 1,313,- 1,- 1,- 1,- 1,- 1,81,- 1,- 1,336,336,312,- 1,336,- 1,344,329,- 1,329,312,- 1,- 1,81,81,81,81,81,81,81,81,81,81,336,- 1,336,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,344,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,320,320,82,- 1,320,325,325,- 1,348,325,348,348,348,348,348,348,348,348,348,348,- 1,82,- 1,320,431,- 1,431,431,325,- 1,- 1,320,- 1,- 1,- 1,- 1,325,82,82,82,82,82,82,82,82,82,82,431,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,- 1,374,92,374,- 1,- 1,374,374,374,374,374,374,374,374,374,374,- 1,- 1,344,- 1,- 1,92,377,377,377,377,377,377,377,377,377,377,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,380,93,380,- 1,- 1,380,380,380,380,380,380,380,380,380,380,- 1,- 1,- 1,- 1,- 1,93,381,381,381,381,381,381,381,381,381,381,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,410,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,410,410,410,410,410,410,410,410,423,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,423,423,423,423,423,423,423,423,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,330,- 1,- 1,330,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,330,330,330,330,330,330,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,141,141,141,141,141,141,- 1,333,333,333,333,333,333,333,333,333,333,- 1,- 1,- 1,- 1,- 1,- 1,- 1,333,333,333,333,333,333,- 1,- 1,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,334,334,334,334,334,334,334,334,334,334,330,333,333,333,333,333,333,334,334,334,334,334,334,- 1,- 1,350,- 1,350,350,350,350,350,350,350,350,350,350,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,350,334,334,334,334,334,334,350,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,350,- 1,- 1,350,351,- 1,351,351,351,351,351,351,351,351,351,351,350,- 1,- 1,- 1,- 1,- 1,- 1,350,- 1,- 1,- 1,351,- 1,- 1,- 1,- 1,350,- 1,351,350,- 1,- 1,- 1,- 1,- 1,- 1,- 1,351,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,351,- 1,- 1,- 1,- 1,- 1,- 1,351,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,351,356,356,356,356,356,356,356,356,356,356,- 1,- 1,- 1,- 1,- 1,- 1,- 1,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,- 1,- 1,- 1,- 1,356,- 1,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,356,362,362,362,362,362,362,362,362,362,362,- 1,- 1,- 1,- 1,- 1,- 1,- 1,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,- 1,- 1,- 1,- 1,362,- 1,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,362,372,372,372,372,372,372,372,372,372,372,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,372,372,- 1,- 1,- 1,- 1,373,372,373,373,373,373,373,373,373,373,373,373,378,378,378,378,378,378,378,378,378,378,- 1,373,- 1,- 1,372,372,- 1,- 1,373,- 1,- 1,372,378,- 1,- 1,- 1,- 1,373,378,- 1,379,379,379,379,379,379,379,379,379,379,- 1,- 1,- 1,373,- 1,- 1,- 1,- 1,- 1,- 1,373,379,379,- 1,378,- 1,- 1,- 1,379,373,378,- 1,- 1,382,382,382,382,382,382,382,382,382,382,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,379,379,382,- 1,- 1,- 1,- 1,379,382,383,- 1,383,383,383,383,383,383,383,383,383,383,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,383,- 1,382,- 1,- 1,- 1,- 1,383,382,- 1,- 1,- 1,- 1,- 1,- 1,384,383,384,384,384,384,384,384,384,384,384,384,- 1,- 1,- 1,- 1,- 1,383,- 1,- 1,- 1,- 1,- 1,384,383,- 1,- 1,- 1,- 1,- 1,384,- 1,- 1,383,- 1,- 1,- 1,- 1,- 1,384,- 1,- 1,398,398,398,398,398,398,398,398,398,398,- 1,- 1,- 1,384,- 1,- 1,- 1,- 1,- 1,- 1,384,398,398,- 1,- 1,- 1,- 1,- 1,398,384,387,387,387,387,387,387,387,387,387,387,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,387,387,387,387,387,398,398,- 1,- 1,- 1,- 1,- 1,398,- 1,- 1,- 1,- 1,- 1,- 1,- 1,388,388,388,388,388,388,388,388,388,388,- 1,387,387,387,387,387,387,388,388,388,388,388,388,- 1,- 1,- 1,407,- 1,388,- 1,- 1,407,- 1,- 1,- 1,- 1,- 1,388,- 1,- 1,407,407,407,407,407,407,407,407,- 1,388,388,388,388,388,388,407,- 1,- 1,- 1,- 1,388,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,388,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,- 1,- 1,- 1,- 1,407,407,- 1,- 1,- 1,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,- 1,- 1,- 1,407,- 1,407,- 1,407,- 1,407,420,420,420,420,420,420,420,420,420,420,- 1,- 1,- 1,- 1,- 1,- 1,- 1,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,- 1,- 1,- 1,- 1,- 1,- 1,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,432,- 1,- 1,432,- 1,- 1,- 1,432,432,432,432,432,432,432,432,432,432,- 1,- 1,- 1,- 1,- 1,- 1,- 1,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,- 1,- 1,- 1,- 1,432,- 1,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,434,- 1,- 1,- 1,- 1,- 1,- 1,- 1,434,434,434,434,434,434,434,434,434,434,- 1,- 1,- 1,- 1,- 1,- 1,- 1,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,- 1,- 1,- 1,- 1,434,- 1,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 211
int state;int base;int backtrk;
int c;
state=start_state;
# 215
if(state >= 0){
({int _Tmp0=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_Tmp0;});
lbuf->lex_last_action=-1;}else{
# 219
state=(- state)- 1;}
# 221
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_lex_base,444U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=*((const int*)_check_known_subscript_notnull(Cyc_lex_backtrk,444U,sizeof(int),state));
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 229
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 233
c=256;}}else{
# 235
c=(int)*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c==-1)c=256;else{
if(c < 0)c=256 + c;}}
# 239
if(*((const int*)_check_known_subscript_notnull(Cyc_lex_check,3820U,sizeof(int),base + c))==state)
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_trans,3820U,sizeof(int),base + c));else{
# 242
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_default,444U,sizeof(int),state));}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action==-1)
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("empty token",sizeof(char),12U);_Tmp0;}));else{
# 248
return lbuf->lex_last_action;}}else{
# 251
if(c==256)lbuf->lex_eof_reached=0;}
# 222
1U;}}
# 255
struct _tuple13*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 213 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(1;Cyc_current_args!=0;Cyc_current_args=_check_null(Cyc_current_args)->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_delete;_Tmp2;})(*_check_null(Cyc_current_targets),(struct _fat_ptr*)Cyc_current_args->hd);*_Tmp0=_Tmp1;});_Tmp0;});}{
# 218
struct _tuple13*_Tmp0=_cycalloc(sizeof(struct _tuple13));_Tmp0->f0=_check_null(Cyc_current_source),_Tmp0->f1=*_check_null(Cyc_current_targets);return _Tmp0;}case 1:
# 221 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2:
# 223
 return 0;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);};
# 227
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 229
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 227 "buildlib.cyl"
 Cyc_current_source=({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=(struct _fat_ptr)({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp2,0,(unsigned long)(({
int _Tmp3=Cyc_Lexing_lexeme_end(lexbuf);_Tmp3 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 227
*_Tmp0=_Tmp1;});_Tmp0;});
# 229
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp2;})(Cyc_strptrcmp);*_Tmp0=_Tmp1;});_Tmp0;});
Cyc_token(lexbuf);
return 0;case 1:
# 235
 Cyc_current_source=({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=(struct _fat_ptr)({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp2,0,(unsigned long)(({
int _Tmp3=Cyc_Lexing_lexeme_end(lexbuf);_Tmp3 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 235
*_Tmp0=_Tmp1;});_Tmp0;});
# 237
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp2;})(Cyc_strptrcmp);*_Tmp0=_Tmp1;});_Tmp0;});
Cyc_args(lexbuf);
return 0;case 2:
# 243
 Cyc_current_source=({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_Tmp0=_Tmp1;});_Tmp0;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp2;})(Cyc_strptrcmp);*_Tmp0=_Tmp1;});_Tmp0;});
Cyc_token(lexbuf);
return 0;default:
# 249
 lexbuf->refill_buff(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);};
# 252
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 254
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:  {
# 252 "buildlib.cyl"
struct _fat_ptr*a;a=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp0=(struct _fat_ptr)({struct _fat_ptr _Tmp1=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp1,0,(unsigned long)(({
int _Tmp2=Cyc_Lexing_lexeme_end(lexbuf);_Tmp2 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 252
*a=_Tmp0;});
# 254
Cyc_current_args=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=a,_Tmp0->tl=Cyc_current_args;_Tmp0;});
return Cyc_args(lexbuf);}case 1:  {
# 258
struct _fat_ptr*a;a=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp0=(struct _fat_ptr)({struct _fat_ptr _Tmp1=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp1,0,(unsigned long)(({
int _Tmp2=Cyc_Lexing_lexeme_end(lexbuf);_Tmp2 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 258
*a=_Tmp0;});
# 260
Cyc_current_args=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=a,_Tmp0->tl=Cyc_current_args;_Tmp0;});
return Cyc_args(lexbuf);}case 2:  {
# 264
struct _fat_ptr*a;a=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp0=(struct _fat_ptr)({struct _fat_ptr _Tmp1=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp1,0,(unsigned long)(({
int _Tmp2=Cyc_Lexing_lexeme_end(lexbuf);_Tmp2 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 264
*a=_Tmp0;});
# 266
Cyc_current_args=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=a,_Tmp0->tl=Cyc_current_args;_Tmp0;});
return Cyc_token(lexbuf);}default:
# 269
 lexbuf->refill_buff(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);};
# 272
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 274
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 273 "buildlib.cyl"
 Cyc_add_target(({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_Tmp0=_Tmp1;});_Tmp0;}));return Cyc_token(lexbuf);case 1:
# 276 "buildlib.cyl"
 return 0;case 2:
# 279 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3:
# 282 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4:
# 285 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5:
# 287
 return Cyc_token(lexbuf);case 6:
# 289
 return Cyc_token(lexbuf);case 7:
# 291
 return Cyc_token(lexbuf);case 8:
# 294 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9:
# 297 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10:
# 300 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11:
# 302
 return Cyc_token(lexbuf);case 12:
# 304
 return Cyc_token(lexbuf);case 13:
# 306
 return Cyc_token(lexbuf);case 14:
# 308
 return Cyc_token(lexbuf);case 15:
# 310
 return Cyc_token(lexbuf);case 16:
# 312
 return Cyc_token(lexbuf);case 17:
# 314
 return Cyc_token(lexbuf);case 18:
# 316
 return Cyc_token(lexbuf);case 19:
# 318
 return Cyc_token(lexbuf);case 20:
# 320
 return Cyc_token(lexbuf);case 21:
# 322
 return Cyc_token(lexbuf);case 22:
# 324
 return Cyc_token(lexbuf);case 23:
# 326
 return Cyc_token(lexbuf);case 24:
# 329 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25:
# 331
 return Cyc_token(lexbuf);case 26:
# 333
 return Cyc_token(lexbuf);case 27:
# 335
 return Cyc_token(lexbuf);case 28:
# 337
 return Cyc_token(lexbuf);case 29:
# 339
 return Cyc_token(lexbuf);case 30:
# 341
 return Cyc_token(lexbuf);case 31:
# 343
 return Cyc_token(lexbuf);case 32:
# 345
 return Cyc_token(lexbuf);case 33:
# 347
 return Cyc_token(lexbuf);case 34:
# 349
 return Cyc_token(lexbuf);case 35:
# 351
 return Cyc_token(lexbuf);case 36:
# 353
 return Cyc_token(lexbuf);case 37:
# 355
 return Cyc_token(lexbuf);case 38:
# 357
 return Cyc_token(lexbuf);case 39:
# 359
 return Cyc_token(lexbuf);case 40:
# 361
 return Cyc_token(lexbuf);case 41:
# 363
 return Cyc_token(lexbuf);case 42:
# 365
 return Cyc_token(lexbuf);case 43:
# 367
 return Cyc_token(lexbuf);case 44:
# 369
 return Cyc_token(lexbuf);case 45:
# 371
 return Cyc_token(lexbuf);case 46:
# 373
 return Cyc_token(lexbuf);case 47:
# 375
 return Cyc_token(lexbuf);case 48:
# 378 "buildlib.cyl"
 return Cyc_token(lexbuf);default:
 lexbuf->refill_buff(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);};
# 382
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 384
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 383 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1:
# 384 "buildlib.cyl"
 return 0;case 2:
# 385 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3:
# 386 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4:
# 389 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5:
# 392 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6:
# 394
 return Cyc_string(lexbuf);case 7:
# 395 "buildlib.cyl"
 return 0;case 8:
# 396 "buildlib.cyl"
 return 0;case 9:
# 397 "buildlib.cyl"
 return Cyc_string(lexbuf);default:
 lexbuf->refill_buff(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);};
# 401
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 403
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 406 "buildlib.cyl"
 return 0;case 1:
# 408
 Cyc_fputc(34,_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){1U;}
return 1;case 2:
# 415 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",_check_null(Cyc_slurp_out));
Cyc_log(_tag_fat("Warning: declaration of malloc sidestepped\n",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return 1;case 3:
# 421 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",_check_null(Cyc_slurp_out));
Cyc_log(_tag_fat("Warning: declaration of malloc sidestepped\n",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return 1;case 4:
# 427 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",_check_null(Cyc_slurp_out));
Cyc_log(_tag_fat("Warning: declaration of calloc sidestepped\n",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return 1;case 5:
# 433 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",_check_null(Cyc_slurp_out));
Cyc_log(_tag_fat("Warning: declaration of calloc sidestepped\n",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return 1;case 6:
# 437
 Cyc_fputs("__region",_check_null(Cyc_slurp_out));
Cyc_log(_tag_fat("Warning: use of region sidestepped\n",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
return 1;case 7:
# 441
 Cyc_log(_tag_fat("Warning: use of __extension__ deleted\n",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));
return 1;case 8:
# 445 "buildlib.cyl"
 Cyc_log(_tag_fat("Warning: use of nonnull attribute deleted\n",sizeof(char),43U),_tag_fat(0U,sizeof(void*),0));
return 1;case 9:
# 450 "buildlib.cyl"
 Cyc_log(_tag_fat("Warning: use of mode HI deleted\n",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
return 1;case 10:
# 453
 Cyc_log(_tag_fat("Warning: use of mode SI deleted\n",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
return 1;case 11:
# 456
 Cyc_log(_tag_fat("Warning: use of mode QI deleted\n",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
return 1;case 12:
# 459
 Cyc_log(_tag_fat("Warning: use of mode DI deleted\n",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
return 1;case 13:
# 462
 Cyc_log(_tag_fat("Warning: use of mode DI deleted\n",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
return 1;case 14:
# 465
 Cyc_log(_tag_fat("Warning: use of mode word deleted\n",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
return 1;case 15:
# 468
 Cyc_fputs("inline",_check_null(Cyc_slurp_out));return 1;case 16:
# 470
 Cyc_fputs("inline",_check_null(Cyc_slurp_out));return 1;case 17:
# 472
 Cyc_fputs("const",_check_null(Cyc_slurp_out));return 1;case 18:
# 474
 Cyc_fputs("const",_check_null(Cyc_slurp_out));return 1;case 19:
# 476
 Cyc_fputs("signed",_check_null(Cyc_slurp_out));return 1;case 20:
# 481 "buildlib.cyl"
 Cyc_fputs("int",_check_null(Cyc_slurp_out));return 1;case 21:
# 483
 return 1;case 22:
# 485
({int _Tmp0=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_Tmp0,_check_null(Cyc_slurp_out));});return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);};
# 489
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 491
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 489 "buildlib.cyl"
 return 0;case 1:
# 491
 Cyc_fputc(34,_check_null(Cyc_slurp_out));return 0;case 2:
# 493
 Cyc_log(_tag_fat("Warning: unclosed string\n",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(_check_null(Cyc_slurp_out),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});return 1;case 3:
# 496
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(_check_null(Cyc_slurp_out),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});return 1;case 4:
# 498
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(_check_null(Cyc_slurp_out),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});return 1;case 5:
# 500
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(_check_null(Cyc_slurp_out),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});return 1;case 6:
# 502
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(_check_null(Cyc_slurp_out),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});return 1;case 7:
# 504
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(_check_null(Cyc_slurp_out),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});return 1;case 8:
# 506
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(_check_null(Cyc_slurp_out),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);};
# 510
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 512
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 516 "buildlib.cyl"
 return 0;case 1:
# 518
 if(Cyc_parens_to_match==1)return 0;
-- Cyc_parens_to_match;
return 1;case 2:
# 522
 ++ Cyc_parens_to_match;
return 1;case 3:
# 525
 while(Cyc_asm_string(lexbuf)){1U;}
return 1;case 4:
# 528
 while(Cyc_asm_comment(lexbuf)){1U;}
return 1;case 5:
# 531
 return 1;case 6:
# 533
 return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);};
# 537
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 539
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 537 "buildlib.cyl"
 Cyc_log(_tag_fat("Warning: unclosed string\n",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 539
 return 0;case 2:
# 541
 Cyc_log(_tag_fat("Warning: unclosed string\n",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));return 1;case 3:
# 543
 return 1;case 4:
# 545
 return 1;case 5:
# 547
 return 1;case 6:
# 549
 return 1;case 7:
# 551
 return 1;case 8:
# 553
 return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);};
# 557
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 559
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 557 "buildlib.cyl"
 Cyc_log(_tag_fat("Warning: unclosed comment\n",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 559
 return 0;case 2:
# 561
 return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);};
# 565
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 567
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple14*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 569 "buildlib.cyl"
 Cyc_current_line=_tag_fat("#define ",sizeof(char),9U);
Cyc_suck_macroname(lexbuf);{
struct _tuple14*_Tmp0=_cycalloc(sizeof(struct _tuple14));_Tmp0->f0=Cyc_current_line,_Tmp0->f1=_check_null(Cyc_current_source);return _Tmp0;}case 1:
# 573
 return Cyc_suck_line(lexbuf);case 2:
# 575
 return 0;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);};
# 579
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 581
struct _tuple14*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
if(lexstate==0){
# 579 "buildlib.cyl"
Cyc_current_source=({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_Tmp0=_Tmp1;});_Tmp0;});
Cyc_current_line=Cyc_strconcat(Cyc_current_line,*Cyc_current_source);
return Cyc_suck_restofline(lexbuf);}else{
# 583
lexbuf->refill_buff(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);};
# 586
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 588
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
if(lexstate==0){
# 586 "buildlib.cyl"
Cyc_current_line=({struct _fat_ptr _Tmp0=Cyc_current_line;Cyc_strconcat(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});return 0;}else{
lexbuf->refill_buff(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);};
# 590
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 592
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple17*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 593 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1:
# 595
 Cyc_current_headerfile=({
struct _fat_ptr _Tmp0=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp0,0,(unsigned long)(({
int _Tmp1=Cyc_Lexing_lexeme_end(lexbuf);_Tmp1 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
Cyc_current_cpp=0;
while(Cyc_commands(lexbuf)){1U;}
Cyc_current_hstubs=Cyc_List_imp_rev(Cyc_current_hstubs);
Cyc_current_cstubs=Cyc_List_imp_rev(Cyc_current_cstubs);
Cyc_current_cycstubs=Cyc_List_imp_rev(Cyc_current_cycstubs);
Cyc_current_cpp=Cyc_List_imp_rev(Cyc_current_cpp);{
struct _tuple17*_Tmp0=_cycalloc(sizeof(struct _tuple17));_Tmp0->f0=Cyc_current_headerfile,_Tmp0->f1=Cyc_current_symbols,_Tmp0->f2=Cyc_current_user_defs,_Tmp0->f3=Cyc_current_omit_symbols,_Tmp0->f4=Cyc_current_hstubs,_Tmp0->f5=Cyc_current_cstubs,_Tmp0->f6=Cyc_current_cycstubs,_Tmp0->f7=Cyc_current_cpp;return _Tmp0;}case 2:
# 620
 return Cyc_spec(lexbuf);case 3:
# 622
 return 0;case 4:
# 624
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,({
# 626
unsigned long _Tmp2=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error in .cys file: expected header file name, found '%c' instead\n",sizeof(char),67U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 0;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);};
# 631
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 633
struct _tuple17*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 631 "buildlib.cyl"
 return 0;case 1:
# 633
 return 0;case 2:
# 635
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){1U;}
Cyc_current_symbols=Cyc_List_append(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3:
# 640
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*tmp_user_defs=Cyc_current_user_defs;
while(Cyc_snarfsymbols(lexbuf)){1U;}
if(tmp_user_defs!=Cyc_current_user_defs){
Cyc_fprintf(Cyc_stderr,
_tag_fat("Error in .cys file: got optional definition in omitsymbols\n",sizeof(char),60U),_tag_fat(0U,sizeof(void*),0));
return 0;}
# 648
Cyc_current_omit_symbols=Cyc_List_append(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;}case 4:
# 651
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){1U;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f0=_tag_fat(0,0,0),({
struct _fat_ptr _Tmp0=(struct _fat_ptr)Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f1=_Tmp0;});
Cyc_current_hstubs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x,_Tmp0->tl=Cyc_current_hstubs;_Tmp0;});
return 1;}case 5:  {
# 659
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(_tag_fat("hstub",sizeof(char),6U)));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);1U;}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);1U;}{
struct _fat_ptr symbol=Cyc_substring(s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){1U;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f0=symbol,({
struct _fat_ptr _Tmp0=(struct _fat_ptr)Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f1=_Tmp0;});
Cyc_current_hstubs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x,_Tmp0->tl=Cyc_current_hstubs;_Tmp0;});
return 1;}}}}case 6:
# 673
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){1U;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f0=_tag_fat(0,0,0),({
struct _fat_ptr _Tmp0=(struct _fat_ptr)Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f1=_Tmp0;});
Cyc_current_cstubs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x,_Tmp0->tl=Cyc_current_cstubs;_Tmp0;});
return 1;}case 7:  {
# 681
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(_tag_fat("cstub",sizeof(char),6U)));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);1U;}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);1U;}{
struct _fat_ptr symbol=Cyc_substring(s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){1U;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f0=symbol,({
struct _fat_ptr _Tmp0=(struct _fat_ptr)Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f1=_Tmp0;});
Cyc_current_cstubs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x,_Tmp0->tl=Cyc_current_cstubs;_Tmp0;});
return 1;}}}}case 8:
# 695
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){1U;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f0=_tag_fat(0,0,0),({
struct _fat_ptr _Tmp0=(struct _fat_ptr)Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f1=_Tmp0;});
Cyc_current_cycstubs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x,_Tmp0->tl=Cyc_current_cycstubs;_Tmp0;});
return 1;}case 9:  {
# 703
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(_tag_fat("cycstub",sizeof(char),8U)));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);1U;}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);1U;}{
struct _fat_ptr symbol=Cyc_substring(s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){1U;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f0=symbol,({
struct _fat_ptr _Tmp0=(struct _fat_ptr)Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f1=_Tmp0;});
Cyc_current_cycstubs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x,_Tmp0->tl=Cyc_current_cycstubs;_Tmp0;});
return 1;}}}}case 10:
# 717
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){1U;}{
struct _fat_ptr*x;x=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp0=(struct _fat_ptr)Cyc_Buffer_contents(_check_null(Cyc_specbuf));*x=_Tmp0;});
Cyc_current_cpp=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x,_Tmp0->tl=Cyc_current_cpp;_Tmp0;});
return 1;}case 11:
# 724
 return 1;case 12:
# 726
 return 1;case 13:
# 728
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,({
# 730
unsigned long _Tmp2=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error in .cys file: expected command, found '%c' instead\n",sizeof(char),58U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 0;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);};
# 735
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 737
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 741 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_snarfed_symbols;_Tmp0;});
return 1;case 1:  {
# 744
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);1U;}
Cyc_current_symbol=Cyc_substring(s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){1U;}
# 753
Cyc_rename_current_symbol=0;{
struct _tuple16*user_def;user_def=_cycalloc(sizeof(struct _tuple16)),({struct _fat_ptr*_Tmp0=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));*_Tmp1=Cyc_current_symbol;_Tmp1;});user_def->f0=_Tmp0;}),({
struct _fat_ptr*_Tmp0=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_Buffer_contents(_check_null(Cyc_specbuf));*_Tmp1=_Tmp2;});_Tmp1;});user_def->f1=_Tmp0;});
Cyc_snarfed_symbols=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=(struct _fat_ptr)Cyc_current_symbol;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_snarfed_symbols;_Tmp0;});
Cyc_current_user_defs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=user_def,_Tmp0->tl=Cyc_current_user_defs;_Tmp0;});
return 1;}}case 2:
# 760
 return 1;case 3:
# 762
 return 0;case 4:
# 764
 Cyc_fprintf(Cyc_stderr,
_tag_fat("Error in .cys file: unexpected end-of-file\n",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return 0;case 5:
# 768
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,({
# 770
unsigned long _Tmp2=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error in .cys file: expected symbol, found '%c' instead\n",sizeof(char),57U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 0;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);};
# 775
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 777
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 781 "buildlib.cyl"
 Cyc_log(_tag_fat("Warning: unclosed brace\n",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 783
 if(Cyc_braces_to_match==1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char(_check_null(Cyc_specbuf),'}');
return 1;case 2:
# 788
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char(_check_null(Cyc_specbuf),'{');
return 1;case 3:
# 792
 Cyc_Buffer_add_char(_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){1U;}
return 1;case 4:
# 796
 Cyc_Buffer_add_string(_check_null(Cyc_specbuf),_tag_fat("/*",sizeof(char),3U));
while(Cyc_block_comment(lexbuf)){1U;}
return 1;case 5:
# 800
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6:  {
# 803
struct _fat_ptr symbol=Cyc_Lexing_lexeme(lexbuf);
if(Cyc_rename_current_symbol && !Cyc_strcmp(symbol,Cyc_current_symbol))
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,*Cyc_add_user_prefix(({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));*_Tmp1=symbol;_Tmp1;})));});else{
# 807
Cyc_Buffer_add_string(_check_null(Cyc_specbuf),symbol);}
return 1;}case 7:
# 810
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_Tmp0,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);};
# 815
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 817
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 815 "buildlib.cyl"
 Cyc_log(_tag_fat("Warning: unclosed string\n",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 817
 Cyc_Buffer_add_char(_check_null(Cyc_specbuf),'"');return 0;case 2:
# 819
 Cyc_log(_tag_fat("Warning: unclosed string\n",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3:
# 823
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4:
# 826
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5:
# 829
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6:
# 832
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7:
# 835
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8:
# 838
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});
return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);};
# 843
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 845
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 843 "buildlib.cyl"
 Cyc_log(_tag_fat("Warning: unclosed comment\n",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 845
 Cyc_Buffer_add_string(_check_null(Cyc_specbuf),_tag_fat("*/",sizeof(char),3U));return 0;case 2:
# 847
({struct Cyc_Buffer_t*_Tmp0=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp0,Cyc_Lexing_lexeme(lexbuf));});
return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);};
# 852
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 854
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 856 "buildlib.cyl"
void Cyc_scan_type(void*,struct Cyc_Hashtable_Table*);struct _tuple18{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_Tmp0=_check_null(e)->r;void*_Tmp1;enum Cyc_Absyn_MallocKind _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 1: _Tmp5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp5;
# 860
struct _fat_ptr*v=(*Cyc_Absyn_binding2qvar(b)).f1;
Cyc_add_target(v);
return;}case 3: _Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*x=_Tmp5;
# 864
for(1;x!=0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 867
return;}case 23: _Tmp5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 869
_Tmp5=e1;_Tmp4=e2;goto _LL8;}case 9: _Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 871
_Tmp5=e1;_Tmp4=e2;goto _LLA;}case 4: _Tmp5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 873
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 40: _Tmp5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LLE;}case 20: _Tmp5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLE: {struct Cyc_Absyn_Exp*e1=_Tmp5;
# 878
_Tmp5=e1;goto _LL10;}case 18: _Tmp5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_Tmp5;
# 880
_Tmp5=e1;goto _LL12;}case 15: _Tmp5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_Tmp5;
# 882
_Tmp5=e1;goto _LL14;}case 5: _Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp5;
# 884
Cyc_scan_exp(e1,dep);
return;}case 6: _Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;
# 887
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
Cyc_scan_exp(e3,dep);
return;}case 7: _Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL1A;}case 8: _Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 893
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 10: _Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_List_List*x=_Tmp4;
# 897
Cyc_scan_exp(e1,dep);
for(1;x!=0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 901
return;}case 14: _Tmp5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t1=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp4;
# 903
Cyc_scan_type(t1,dep);
Cyc_scan_exp(e1,dep);
return;}case 33: _Tmp2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.mknd;_Tmp5=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp4=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp3=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;{enum Cyc_Absyn_MallocKind mknd=_Tmp2;struct Cyc_Absyn_Exp*ropt=_Tmp5;struct Cyc_Absyn_Exp*aqopt=_Tmp4;void**topt=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp1;
# 907
if(ropt!=0)Cyc_scan_exp(ropt,dep);
if(aqopt!=0)Cyc_scan_exp(aqopt,dep);
if(topt!=0)Cyc_scan_type(*topt,dep);
Cyc_scan_exp(e,dep);
return;}case 37: _Tmp5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
# 913
Cyc_scan_exp(e,dep);return;}case 38: _Tmp5=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t1=_Tmp5;
_Tmp5=t1;goto _LL26;}case 17: _Tmp5=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL26: {void*t1=_Tmp5;
# 916
Cyc_scan_type(t1,dep);
return;}case 21: _Tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct _fat_ptr*fn=_Tmp4;
# 919
_Tmp5=e1;_Tmp4=fn;goto _LL2A;}case 22: _Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct _fat_ptr*fn=_Tmp4;
# 921
Cyc_scan_exp(e1,dep);
Cyc_add_target(fn);
return;}case 19: _Tmp5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t1=_Tmp5;struct Cyc_List_List*f=_Tmp4;
# 925
Cyc_scan_type(t1,dep);
# 927
{void*_Tmp6=(void*)_check_null(f)->hd;void*_Tmp7;_Tmp7=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_Tmp6)->f1;{struct _fat_ptr*fn=_Tmp7;
Cyc_add_target(fn);goto _LL57;}_LL57:;}
# 930
return;}case 0:
# 932
 return;case 35: _Tmp5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*x=_Tmp5;
# 934
for(1;x!=0;x=x->tl){
struct _tuple18*_Tmp6=(struct _tuple18*)x->hd;void*_Tmp7;_Tmp7=_Tmp6->f1;{struct Cyc_Absyn_Exp*e1=_Tmp7;
Cyc_scan_exp(e1,dep);}}
# 938
return;}case 39:
 return;case 2:
# 941
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Pragma_e\n",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 34:
# 944
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Swap_e\n",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 36:
# 947
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Stmt_e\n",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 41:
# 950
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Assert_e\n",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 42:
# 953
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Assert_false_e\n",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 11:
# 956
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Throw_e\n",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 12:
# 959
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected NoInstantiate_e\n",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 13:
# 962
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Instantiate_e\n",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 16:
# 965
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected New_e\n",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 24:
# 968
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected CompoundLit_e\n",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 25:
# 971
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Array_e\n",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 26:
# 974
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Comprehension_e\n",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 27:
# 977
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected ComprehensionNoinit_e\n",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 28:
# 980
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Aggregate_e\n",sizeof(char),31U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 29:
# 983
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected AnonStruct_e\n",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 30:
# 986
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Datatype_e\n",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 31:
# 989
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Enum_e\n",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;default:
# 992
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected AnonEnum_e\n",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;};}
# 997
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned)eo)Cyc_scan_exp(eo,dep);
return;}
# 1002
void Cyc_scan_decl(struct Cyc_Absyn_Decl*,struct Cyc_Hashtable_Table*);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
struct Cyc_Absyn_FnInfo _Tmp0;void*_Tmp1;struct Cyc_Absyn_PtrInfo _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)t)){case 0: _Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{void*c=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 1006
void*_Tmp5;union Cyc_Absyn_AggrInfo _Tmp6;switch(*((int*)c)){case 0:
 goto _LL21;case 1: _LL21:
 goto _LL23;case 21: _LL23:
 goto _LL25;case 2: _LL25:
 goto _LL27;case 3: _LL27:
 goto _LL29;case 20: _LL29:
# 1013
 return;case 24: _Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp6;
# 1015
struct _tuple12 _Tmp7=Cyc_Absyn_aggr_kinded_name(info);void*_Tmp8;_Tmp8=_Tmp7.f1->f1;{struct _fat_ptr*v=_Tmp8;
_Tmp5=v;goto _LL2D;}}case 19: _Tmp5=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)c)->f1->f1;_LL2D: {struct _fat_ptr*v=_Tmp5;
Cyc_add_target(v);return;}case 22:
 goto _LL31;case 23: _LL31: goto _LL33;case 4: _LL33:
 goto _LL35;case 6: _LL35: goto _LL37;case 7: _LL37:
 goto _LL39;case 8: _LL39: goto _LL3B;case 9: _LL3B:
 goto _LL3D;case 10: _LL3D:
 goto _LL3F;case 5: _LL3F: goto _LL41;case 11: _LL41:
 goto _LL43;case 12: _LL43: goto _LL45;case 13: _LL45:
 goto _LL47;case 14: _LL47: goto _LL49;case 15: _LL49:
 goto _LL4B;case 16: _LL4B: goto _LL4D;case 18: _LL4D:
 goto _LL4F;default: _LL4F:
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_typ2string(t);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected %s\n",sizeof(char),22U),_tag_fat(_Tmp8,sizeof(void*),1));});
exit(1);return;};}case 4: _Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_PtrInfo x=_Tmp2;
# 1032
Cyc_scan_type(x.elt_type,dep);
return;}case 5: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zero_term;{void*t=_Tmp4;struct Cyc_Absyn_Exp*sz=_Tmp3;void*zt=_Tmp1;
# 1035
Cyc_scan_type(t,dep);
Cyc_scan_exp_opt(sz,dep);
return;}case 11: _Tmp4=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
# 1039
Cyc_scan_exp(e,dep);
return;}case 6: _Tmp0=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo x=_Tmp0;
# 1042
Cyc_scan_type(x.ret_type,dep);
{struct Cyc_List_List*a=x.args;for(0;a!=0;a=a->tl){
struct _tuple9*_Tmp5=(struct _tuple9*)a->hd;void*_Tmp6;_Tmp6=_Tmp5->f2;{void*argt=_Tmp6;
Cyc_scan_type(argt,dep);}}}
# 1047
if(x.cyc_varargs!=0)
Cyc_scan_type(x.cyc_varargs->type,dep);
return;}case 7: _Tmp4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f3;{struct Cyc_List_List*sfs=_Tmp4;
# 1051
for(1;sfs!=0;sfs=sfs->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->width,dep);}
# 1055
return;}case 8: _Tmp4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1->f1;{struct _fat_ptr*v=_Tmp4;
# 1057
Cyc_add_target(v);
return;}case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1)->r)){case 0: _Tmp4=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*x=_Tmp4;
# 1061
({struct Cyc_Absyn_Decl*_Tmp5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp6->tag=5,_Tmp6->f1=x;_Tmp6;}),0U);Cyc_scan_decl(_Tmp5,dep);});{
struct _tuple1*_Tmp5=x->name;void*_Tmp6;_Tmp6=_Tmp5->f1;{struct _fat_ptr*n=_Tmp6;
Cyc_add_target(n);
return;}}}case 1: _Tmp4=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*x=_Tmp4;
# 1067
({struct Cyc_Absyn_Decl*_Tmp5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_Tmp6->tag=7,_Tmp6->f1=x;_Tmp6;}),0U);Cyc_scan_decl(_Tmp5,dep);});{
struct _tuple1*_Tmp5=x->name;void*_Tmp6;_Tmp6=_Tmp5->f1;{struct _fat_ptr*n=_Tmp6;
Cyc_add_target(n);
return;}}}default: _Tmp4=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp4;
# 1073
Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Datatype declaration\n",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;}}case 3:
# 1076
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Cvar\n",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 1:
# 1079
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected Evar\n",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 2:
# 1082
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected VarType\n",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;default:
# 1085
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected valueof_t\n",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));
exit(1);return;};}
# 1090
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**saved_targets=Cyc_current_targets;
struct _fat_ptr*saved_source=Cyc_current_source;
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp2;})(Cyc_strptrcmp);*_Tmp0=_Tmp1;});_Tmp0;});
{void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*x=_Tmp1;
# 1096
struct _tuple1*_Tmp2=x->name;void*_Tmp3;_Tmp3=_Tmp2->f1;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
Cyc_scan_type(x->type,dep);
Cyc_scan_exp_opt(x->initializer,dep);
goto _LL0;}}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*x=_Tmp1;
# 1102
struct _tuple1*_Tmp2=x->name;void*_Tmp3;_Tmp3=_Tmp2->f1;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
Cyc_scan_type(x->i.ret_type,dep);
{struct Cyc_List_List*a=x->i.args;for(0;a!=0;a=a->tl){
struct _tuple9*_Tmp4=(struct _tuple9*)a->hd;void*_Tmp5;_Tmp5=_Tmp4->f2;{void*t1=_Tmp5;
Cyc_scan_type(t1,dep);}}}
# 1109
if(x->i.cyc_varargs!=0)
Cyc_scan_type(x->i.cyc_varargs->type,dep);
if(x->is_inline)
Cyc_fprintf(Cyc_stderr,_tag_fat("Warning: ignoring inline function\n",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*x=_Tmp1;
# 1115
struct _tuple1*_Tmp2=x->name;void*_Tmp3;_Tmp3=_Tmp2->f1;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
if((unsigned)x->impl){
{struct Cyc_List_List*fs=x->impl->fields;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)fs->hd;
Cyc_scan_type(f->type,dep);
Cyc_scan_exp_opt(f->width,dep);}}{
# 1125
struct Cyc_List_List*fs=_check_null(x->impl)->fields;for(0;fs!=0;fs=fs->tl){;}}}
# 1129
goto _LL0;}}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*x=_Tmp1;
# 1131
struct _tuple1*_Tmp2=x->name;void*_Tmp3;_Tmp3=_Tmp2->f1;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
if((unsigned)x->fields){
{struct Cyc_List_List*fs=(struct Cyc_List_List*)x->fields->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_scan_exp_opt(f->tag,dep);}}{
# 1140
struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(x->fields)->v;for(0;fs!=0;fs=fs->tl){;}}}
# 1144
goto _LL0;}}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*x=_Tmp1;
# 1146
struct _tuple1*_Tmp2=x->name;void*_Tmp3;_Tmp3=_Tmp2->f1;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
if((unsigned)x->defn)
Cyc_scan_type(x->defn,dep);
goto _LL0;}}case 4:
# 1152
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected region declaration",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 13:
# 1155
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected __cyclone_port_on__",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 14:
# 1158
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected __cyclone_port_off__",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 15:
# 1161
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected __tempest_on__",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 16:
# 1164
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected __tempest_off__",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 2:
# 1167
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected let declaration\n",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 6:
# 1170
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected datatype declaration\n",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 3:
# 1173
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected let declaration\n",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 9:
# 1176
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected namespace declaration\n",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 10:
# 1179
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected using declaration\n",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));
exit(1);case 11:
# 1182
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected extern \"C\" declaration\n",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));
exit(1);default:
# 1185
 Cyc_fprintf(Cyc_stderr,_tag_fat("Error: unexpected extern \"C include\" declaration\n",sizeof(char),50U),_tag_fat(0U,sizeof(void*),0));
exit(1);}_LL0:;}{
# 1193
struct Cyc_Set_Set*old;
struct _fat_ptr*name=_check_null(Cyc_current_source);
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
old=({struct Cyc_Set_Set*(*_Tmp2)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;_Tmp2;})(dep,name);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 1198
old=({struct Cyc_Set_Set*(*_Tmp4)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp4;})(Cyc_strptrcmp);goto _LL35;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL35:;}}}{
# 1200
struct Cyc_Set_Set*targets=Cyc_Set_union_two(*_check_null(Cyc_current_targets),old);
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))Cyc_Hashtable_insert;_Tmp0;})(dep,name,targets);
# 1203
Cyc_current_targets=saved_targets;
Cyc_current_source=saved_source;}}}
# 1207
struct Cyc_Hashtable_Table*Cyc_new_deps (void){
return({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;_Tmp0;})(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1211
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _fat_ptr*x){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){{struct Cyc_Set_Set*_Tmp2=({struct Cyc_Set_Set*(*_Tmp3)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;_Tmp3;})(t,x);_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found)
# 1214
return({struct Cyc_Set_Set*(*_Tmp4)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp4;})(Cyc_strptrcmp);else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 1218
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1228 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=({struct Cyc_Set_Set*(*_Tmp0)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp0;})(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init!=0;init=init->tl){
curr=({struct Cyc_Set_Set*(*_Tmp0)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;_Tmp0;})(curr,(struct _fat_ptr*)init->hd);}{
# 1233
struct Cyc_Set_Set*delta=curr;
# 1235
struct _fat_ptr*sptr;sptr=_cycalloc(sizeof(struct _fat_ptr)),*sptr=_tag_fat("",sizeof(char),1U);
while(Cyc_Set_cardinality(delta)> 0){
{struct Cyc_Set_Set*next=emptyset;
struct Cyc_Iter_Iter iter=Cyc_Set_make_iter(Cyc_Core_heap_region,delta);
while(({int(*_Tmp0)(struct Cyc_Iter_Iter,struct _fat_ptr**)=(int(*)(struct Cyc_Iter_Iter,struct _fat_ptr**))Cyc_Iter_next;_Tmp0;})(iter,& sptr)){
next=({struct Cyc_Set_Set*_Tmp0=next;Cyc_Set_union_two(_Tmp0,Cyc_find(t,sptr));});1U;}
delta=Cyc_Set_diff(next,curr);
curr=Cyc_Set_union_two(curr,delta);}
# 1237
1U;}
# 1244
return curr;}}
# 1247
enum Cyc_buildlib_mode{Cyc_NORMAL =0U,Cyc_GATHER =1U,Cyc_GATHERSCRIPT =2U,Cyc_FINISH =3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering (void){
return(int)Cyc_mode==1 ||(int)Cyc_mode==2;}
# 1253
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _fat_ptr fmt,struct _fat_ptr ap){
# 1257
if(Cyc_script_file==0){
Cyc_fprintf(Cyc_stderr,_tag_fat("Internal error: script file is NULL\n",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));
exit(1);}
# 1261
return Cyc_vfprintf(Cyc_script_file,fmt,ap);}
# 1264
int Cyc_force_directory(struct _fat_ptr d){
if((int)Cyc_mode==2)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=d;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=d;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_prscript(_tag_fat("if ! test -e %s; then mkdir %s; fi\n",sizeof(char),36U),_tag_fat(_Tmp2,sizeof(void*),2));});else{
# 1271
int fd=({const char*_Tmp0=(const char*)_untag_fat_ptr_check_bound(d,sizeof(char),1U);Cyc_open(_Tmp0,0,_tag_fat(0U,sizeof(unsigned short),0));});
if(fd==-1){
if(mkdir((const char*)_check_null(_untag_fat_ptr(d,sizeof(char),1U)),448)==-1){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=d;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not create directory %s\n",sizeof(char),38U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}}else{
# 1278
close(fd);}}
# 1280
return 0;}
# 1283
int Cyc_force_directory_prefixes(struct _fat_ptr file){
# 1287
struct _fat_ptr curr=Cyc_strdup(file);
# 1289
struct Cyc_List_List*x=0;
while(1){
curr=Cyc_Filename_dirname(curr);
if(Cyc_strlen(curr)==0U)break;
x=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=(struct _fat_ptr)curr;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=x;_Tmp0;});
# 1291
1U;}
# 1296
for(1;x!=0;x=x->tl){
if(Cyc_force_directory(*((struct _fat_ptr*)x->hd)))return 1;}
# 1299
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _fat_ptr f1;};
# 1306
static int Cyc_is_other_special(char c){
switch((int)c){case 92:
 goto _LL4;case 34: _LL4:
 goto _LL6;case 59: _LL6:
 goto _LL8;case 38: _LL8:
 goto _LLA;case 40: _LLA:
 goto _LLC;case 41: _LLC:
 goto _LLE;case 124: _LLE:
 goto _LL10;case 94: _LL10:
 goto _LL12;case 60: _LL12:
 goto _LL14;case 62: _LL14:
 goto _LL16;case 10: _LL16:
# 1321
 goto _LL18;case 9: _LL18:
 return 1;default:
 return 0;};}
# 1327
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long len=Cyc_strlen(s);
# 1331
int single_quotes=0;
int other_special=0;
{int i=0;for(0;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c==39)++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 1340
if(single_quotes==0 && other_special==0)
return s;
# 1344
if(single_quotes==0)
return Cyc_strconcat_l(({struct _fat_ptr*_Tmp0[3];({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("'",sizeof(char),2U);_Tmp2;});_Tmp0[0]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=(struct _fat_ptr)s;_Tmp2;});_Tmp0[1]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("'",sizeof(char),2U);_Tmp2;});_Tmp0[2]=_Tmp1;});Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _fat_ptr*),3));}));{
# 1348
unsigned long len2=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;
struct _fat_ptr s2=({unsigned _Tmp0=(len2 + 1U)+ 1U;_tag_fat(({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=len2 + 1U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='\000';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;}),sizeof(char),_Tmp0);});
int i=0;
int j=0;
for(1;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c==39 || Cyc_is_other_special(c))
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\\';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 1358
return s2;}}
# 1360
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=Cyc_sh_escape_string(*sp);*_Tmp0=_Tmp1;});return _Tmp0;}
# 1365
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){
# 1373
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1378
if((unsigned)Cyc_headers_to_do && !({int(*_Tmp0)(struct Cyc_Set_Set*,struct _fat_ptr*)=({int(*_Tmp1)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp1;});struct Cyc_Set_Set*_Tmp1=*Cyc_headers_to_do;_Tmp0(_Tmp1,({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=({const char*_Tmp3=filename;_tag_fat((void*)_Tmp3,sizeof(char),_get_zero_arr_size_char((void*)_Tmp3,1U));});_Tmp2;}));}))
return 0;
# 1381
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("********************************* %s...\n",sizeof(char),41U),_tag_fat(_Tmp1,sizeof(void*),1));});
# 1384
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_log(_tag_fat("\n%s:\n",sizeof(char),6U),_tag_fat(_Tmp1,sizeof(void*),1));});{
# 1396 "buildlib.cyl"
struct _fat_ptr basename=Cyc_Filename_basename(({const char*_Tmp0=filename;_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}));
struct _fat_ptr dirname=Cyc_Filename_dirname(({const char*_Tmp0=filename;_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}));
struct _fat_ptr choppedname=Cyc_Filename_chop_extension(basename);
const char*cppinfile=(const char*)_check_null(_untag_fat_ptr_check_bound(Cyc_strconcat(choppedname,_tag_fat(".iA",sizeof(char),4U)),sizeof(char),1U));
# 1401
const char*macrosfile=(const char*)_check_null(_untag_fat_ptr_check_bound(_get_fat_size(dirname,sizeof(char))==0U?({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=choppedname;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("%s.iB",sizeof(char),6U),_tag_fat(_Tmp1,sizeof(void*),1));}):({struct _fat_ptr _Tmp0=dirname;Cyc_Filename_concat(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=choppedname;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("%s.iB",sizeof(char),6U),_tag_fat(_Tmp2,sizeof(void*),1));}));}),sizeof(char),1U));
# 1406
const char*declsfile=(const char*)_check_null(_untag_fat_ptr_check_bound(_get_fat_size(dirname,sizeof(char))==0U?({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=choppedname;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("%s.iC",sizeof(char),6U),_tag_fat(_Tmp1,sizeof(void*),1));}):({struct _fat_ptr _Tmp0=dirname;Cyc_Filename_concat(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=choppedname;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("%s.iC",sizeof(char),6U),_tag_fat(_Tmp2,sizeof(void*),1));}));}),sizeof(char),1U));
# 1411
const char*filtereddeclsfile=(const char*)_check_null(_untag_fat_ptr_check_bound(_get_fat_size(dirname,sizeof(char))==0U?({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=choppedname;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("%s.iD",sizeof(char),6U),_tag_fat(_Tmp1,sizeof(void*),1));}):({struct _fat_ptr _Tmp0=dirname;Cyc_Filename_concat(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=choppedname;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("%s.iD",sizeof(char),6U),_tag_fat(_Tmp2,sizeof(void*),1));}));}),sizeof(char),1U));
# 1416
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
# 1419
if(Cyc_force_directory_prefixes(({const char*_Tmp2=filename;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));}))){
int _Tmp2=1;_npop_handler(0);return _Tmp2;}
# 1424
if((int)Cyc_mode!=3){
if((int)Cyc_mode==2){
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=cppinfile;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_prscript(_tag_fat("cat >%s <<XXX\n",sizeof(char),15U),_tag_fat(_Tmp3,sizeof(void*),1));});
{struct Cyc_List_List*l=cpp_insert;for(0;l!=0;l=l->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=*((struct _fat_ptr*)l->hd);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_prscript(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp3,sizeof(void*),1));});}}
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=filename;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_prscript(_tag_fat("#include <%s>\n",sizeof(char),15U),_tag_fat(_Tmp3,sizeof(void*),1));});
Cyc_prscript(_tag_fat("XXX\n",sizeof(char),5U),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=Cyc_target_cflags;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5=macrosfile;_tag_fat((void*)_Tmp5,sizeof(char),_get_zero_arr_size_char((void*)_Tmp5,1U));});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=cppinfile;_tag_fat((void*)_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[3];_Tmp5[0]=& _Tmp2,_Tmp5[1]=& _Tmp3,_Tmp5[2]=& _Tmp4;Cyc_prscript(_tag_fat("$GCC %s -E -dM -o %s -x c %s && \\\n",sizeof(char),35U),_tag_fat(_Tmp5,sizeof(void*),3));});
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=Cyc_target_cflags;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5=declsfile;_tag_fat((void*)_Tmp5,sizeof(char),_get_zero_arr_size_char((void*)_Tmp5,1U));});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=cppinfile;_tag_fat((void*)_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[3];_Tmp5[0]=& _Tmp2,_Tmp5[1]=& _Tmp3,_Tmp5[2]=& _Tmp4;Cyc_prscript(_tag_fat("$GCC %s -E     -o %s -x c %s;\n",sizeof(char),31U),_tag_fat(_Tmp5,sizeof(void*),3));});
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=cppinfile;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_prscript(_tag_fat("rm %s\n",sizeof(char),7U),_tag_fat(_Tmp3,sizeof(void*),1));});}else{
# 1436
maybe=Cyc_fopen(cppinfile,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=cppinfile;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not create file %s\n",sizeof(char),33U),_tag_fat(_Tmp3,sizeof(void*),1));});{
int _Tmp2=1;_npop_handler(0);return _Tmp2;}}
# 1441
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=cppinfile;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat("Creating %s\n",sizeof(char),13U),_tag_fat(_Tmp3,sizeof(void*),1));});
out_file=maybe;
{struct Cyc_List_List*l=cpp_insert;for(0;l!=0;l=l->tl){
Cyc_fputs((const char*)_check_null(_untag_fat_ptr_check_bound(*((struct _fat_ptr*)l->hd),sizeof(char),1U)),out_file);}}
# 1447
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=filename;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(out_file,_tag_fat("#include <%s>\n",sizeof(char),15U),_tag_fat(_Tmp3,sizeof(void*),1));});
Cyc_fclose(out_file);{
struct _fat_ptr cppargs_string=({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=(struct _fat_ptr)_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*(*_Tmp5)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp6)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;_Tmp6;});_Tmp5(Cyc_sh_escape_stringptr,Cyc_List_rev(Cyc_cppargs));});_Tmp3->tl=_Tmp4;});_Tmp3;});
# 1450
Cyc_str_sepstr(_Tmp2,
# 1452
_tag_fat(" ",sizeof(char),2U));});
char*cmd=(char*)_check_null(_untag_fat_ptr_check_bound(({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=Cyc_cyclone_cc;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=Cyc_target_cflags;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=cppargs_string;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7=macrosfile;_tag_fat((void*)_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=cppinfile;_tag_fat((void*)_Tmp8,sizeof(char),_get_zero_arr_size_char((void*)_Tmp8,1U));});_Tmp7;});void*_Tmp7[5];_Tmp7[0]=& _Tmp2,_Tmp7[1]=& _Tmp3,_Tmp7[2]=& _Tmp4,_Tmp7[3]=& _Tmp5,_Tmp7[4]=& _Tmp6;Cyc_aprintf(_tag_fat("%s %s %s -E -dM -o %s -x c %s",sizeof(char),30U),_tag_fat(_Tmp7,sizeof(void*),5));}),sizeof(char),1U));
# 1456
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({char*_Tmp4=cmd;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat("%s\n",sizeof(char),4U),_tag_fat(_Tmp3,sizeof(void*),1));});
if(!system(cmd)){
# 1461
cmd=(char*)_check_null(_untag_fat_ptr_check_bound(({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=Cyc_cyclone_cc;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=Cyc_target_cflags;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=cppargs_string;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7=declsfile;_tag_fat((void*)_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=cppinfile;_tag_fat((void*)_Tmp8,sizeof(char),_get_zero_arr_size_char((void*)_Tmp8,1U));});_Tmp7;});void*_Tmp7[5];_Tmp7[0]=& _Tmp2,_Tmp7[1]=& _Tmp3,_Tmp7[2]=& _Tmp4,_Tmp7[3]=& _Tmp5,_Tmp7[4]=& _Tmp6;Cyc_aprintf(_tag_fat("%s %s %s -E -o %s -x c %s",sizeof(char),26U),_tag_fat(_Tmp7,sizeof(void*),5));}),sizeof(char),1U));
# 1464
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({char*_Tmp4=cmd;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat("%s\n",sizeof(char),4U),_tag_fat(_Tmp3,sizeof(void*),1));});
system(cmd);}}}}
# 1471
if(Cyc_gathering()){int _Tmp2=0;_npop_handler(0);return _Tmp2;}{
# 1474
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp2->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=macrosfile;_tag_fat((void*)_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("can't open macrosfile %s",sizeof(char),25U),_tag_fat(_Tmp5,sizeof(void*),1));});_Tmp2->f1=_Tmp3;});_Tmp2;}));
# 1479
if(Cyc_verbose)Cyc_fprintf(Cyc_stderr,_tag_fat("Getting macros...",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));
in_file=maybe;{
struct Cyc_Lexing_lexbuf*l=Cyc_Lexing_from_file(in_file);
struct _tuple13*entry;
while((entry=Cyc_line(l))!=0){
{struct _tuple13*_Tmp2=entry;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct _fat_ptr*name=_Tmp4;struct Cyc_Set_Set*uses=_Tmp3;
({void(*_Tmp5)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))Cyc_Hashtable_insert;_Tmp5;})(t,name,uses);}}
# 1484
1U;}
# 1489
Cyc_fclose(in_file);
if(Cyc_verbose)Cyc_fprintf(Cyc_stderr,_tag_fat("done.\n",sizeof(char),7U),_tag_fat(0U,sizeof(void*),0));
# 1493
maybe=Cyc_fopen(declsfile,"r");
if(!((unsigned)maybe))_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp2->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=declsfile;_tag_fat((void*)_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("can't open declsfile %s",sizeof(char),24U),_tag_fat(_Tmp5,sizeof(void*),1));});_Tmp2->f1=_Tmp3;});_Tmp2;}));
# 1497
if(Cyc_verbose)Cyc_fprintf(Cyc_stderr,_tag_fat("Extracting declarations...",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(filtereddeclsfile,"w");
if(!((unsigned)Cyc_slurp_out)){int _Tmp2=1;_npop_handler(0);return _Tmp2;}
while(Cyc_slurp(l)){1U;}
if(Cyc_verbose)Cyc_fprintf(Cyc_stderr,_tag_fat("done.\n",sizeof(char),7U),_tag_fat(0U,sizeof(void*),0));{
# 1505
struct Cyc_List_List*x=user_defs;
while(x!=0){
{struct _tuple16*_Tmp2=(struct _tuple16*)x->hd;void*_Tmp3;_Tmp3=_Tmp2->f1;{struct _fat_ptr*s=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(_check_null(Cyc_slurp_out),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});
x=x->tl;}}
# 1507
1U;}
# 1511
Cyc_fclose(in_file);
Cyc_fclose(_check_null(Cyc_slurp_out));
if((int)Cyc_mode!=3)
;
# 1517
maybe=Cyc_fopen(filtereddeclsfile,"r");
if(!((unsigned)maybe)){int _Tmp2=1;_npop_handler(0);return _Tmp2;}
if(Cyc_verbose)Cyc_fprintf(Cyc_stderr,_tag_fat("Parsing declarations...",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));
in_file=maybe;
Cyc_Position_reset_position(({const char*_Tmp2=filtereddeclsfile;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*decls=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
if(Cyc_verbose)Cyc_fprintf(Cyc_stderr,_tag_fat("done.\n",sizeof(char),7U),_tag_fat(0U,sizeof(void*),0));
# 1529
{struct Cyc_List_List*d=decls;for(0;d!=0;d=d->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)d->hd,t);}}{
# 1533
struct Cyc_List_List*user_symbols=({struct Cyc_List_List*(*_Tmp2)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp3)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;});_Tmp2(Cyc_add_user_prefix,Cyc_List_split(user_defs).f0);});
struct Cyc_Set_Set*reachable_set=({struct Cyc_List_List*_Tmp2=Cyc_List_append(start_symbols,user_symbols);Cyc_reachable(_Tmp2,t);});
# 1537
struct Cyc_List_List*reachable_decls=0;
struct Cyc_List_List*user_decls=0;
struct Cyc_Set_Set*defined_symbols=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp2;})(Cyc_strptrcmp);
{struct Cyc_List_List*d=decls;for(0;d!=0;d=d->tl){
struct Cyc_Absyn_Decl*decl=(struct Cyc_Absyn_Decl*)d->hd;
struct _fat_ptr*name;
{void*_Tmp2=decl->r;void*_Tmp3;switch(*((int*)_Tmp2)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*x=_Tmp3;
# 1545
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
defined_symbols=({struct Cyc_Set_Set*(*_Tmp6)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;_Tmp6;})(defined_symbols,v);
if(({int(*_Tmp6)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;_Tmp6;})(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 1: _Tmp3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Fndecl*x=_Tmp3;
# 1551
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
defined_symbols=({struct Cyc_Set_Set*(*_Tmp6)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;_Tmp6;})(defined_symbols,v);
if(({int(*_Tmp6)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;_Tmp6;})(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 5: _Tmp3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Aggrdecl*x=_Tmp3;
# 1557
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL6;}}case 7: _Tmp3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Enumdecl*x=_Tmp3;
# 1561
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
name=v;
# 1565
if(name!=0 &&({int(*_Tmp6)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp6;})(reachable_set,name))
reachable_decls=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=decl,_Tmp6->tl=reachable_decls;_Tmp6;});else{
# 1568
if((unsigned)x->fields){
struct Cyc_List_List*fs=(struct Cyc_List_List*)x->fields->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
struct _tuple1*_Tmp6=f->name;void*_Tmp7;_Tmp7=_Tmp6->f1;{struct _fat_ptr*v=_Tmp7;
if(({int(*_Tmp8)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp8;})(reachable_set,v)){
reachable_decls=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=decl,_Tmp8->tl=reachable_decls;_Tmp8;});
break;}}}}}
# 1578
name=0;
goto _LL6;}}case 8: _Tmp3=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Typedefdecl*x=_Tmp3;
# 1581
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL6;}}case 13:
 goto _LL14;case 14: _LL14:
 goto _LL16;case 15: _LL16:
 goto _LL18;case 16: _LL18:
 goto _LL1A;case 2: _LL1A:
 goto _LL1C;case 6: _LL1C:
 goto _LL1E;case 3: _LL1E:
 goto _LL20;case 9: _LL20:
 goto _LL22;case 10: _LL22:
 goto _LL24;case 11: _LL24:
 goto _LL26;case 12: _LL26:
 goto _LL28;default: _LL28:
# 1596
 name=0;
goto _LL6;}_LL6:;}
# 1600
if(name!=0 &&({int(*_Tmp2)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp2;})(reachable_set,name)){
if(({struct _fat_ptr _Tmp2=*name;struct _fat_ptr _Tmp3=Cyc_user_prefix;Cyc_strncmp(_Tmp2,_Tmp3,Cyc_strlen(Cyc_user_prefix));})!=0)
reachable_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=decl,_Tmp2->tl=reachable_decls;_Tmp2;});else{
# 1605
Cyc_rename_decl(decl);
user_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=decl,_Tmp2->tl=user_decls;_Tmp2;});}}}}
# 1612
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){int _Tmp2=1;_npop_handler(0);return _Tmp2;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _fat_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=filename;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("_%s_",sizeof(char),5U),_tag_fat(_Tmp3,sizeof(void*),1));});
{int j=0;for(0;(unsigned)j < _get_fat_size(ifdefmacro,sizeof(char));++ j){
if((int)((char*)ifdefmacro.curr)[j]==46 ||(int)((char*)ifdefmacro.curr)[j]==47)
({struct _fat_ptr _Tmp2=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='_';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});else{
if((int)((char*)ifdefmacro.curr)[j]!=95 &&(int)((char*)ifdefmacro.curr)[j]!=47)
({struct _fat_ptr _Tmp2=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)toupper((int)((char*)ifdefmacro.curr)[j]);if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});}}}
# 1624
if(!Cyc_do_setjmp)
Cyc_fprintf(out_file,_tag_fat("__noinference__{\n",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=ifdefmacro;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=ifdefmacro;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_fprintf(out_file,_tag_fat("#ifndef %s\n#define %s\n",sizeof(char),23U),_tag_fat(_Tmp4,sizeof(void*),2));});{
# 1633
struct Cyc_List_List*print_decls=0;
struct Cyc_List_List*names=0;
{struct Cyc_List_List*d=reachable_decls;for(0;d!=0;d=d->tl){
struct Cyc_Absyn_Decl*decl=(struct Cyc_Absyn_Decl*)d->hd;
int anon_enum=0;
struct _fat_ptr*name;
{void*_Tmp2=decl->r;void*_Tmp3;switch(*((int*)_Tmp2)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*x=_Tmp3;
# 1641
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL3B;}}case 1: _Tmp3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Fndecl*x=_Tmp3;
# 1645
if(x->is_inline){name=0;goto _LL3B;}{
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL3B;}}}case 5: _Tmp3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Aggrdecl*x=_Tmp3;
# 1650
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL3B;}}case 7: _Tmp3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Enumdecl*x=_Tmp3;
# 1654
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL3B;}}case 8: _Tmp3=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Typedefdecl*x=_Tmp3;
# 1658
struct _tuple1*_Tmp4=x->name;void*_Tmp5;_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL3B;}}case 4:
 goto _LL49;case 13: _LL49:
 goto _LL4B;case 14: _LL4B:
 goto _LL4D;case 15: _LL4D:
 goto _LL4F;case 16: _LL4F:
 goto _LL51;case 2: _LL51:
 goto _LL53;case 6: _LL53:
 goto _LL55;case 3: _LL55:
 goto _LL57;case 9: _LL57:
 goto _LL59;case 10: _LL59:
 goto _LL5B;case 11: _LL5B:
 goto _LL5D;default: _LL5D:
# 1673
 name=0;
goto _LL3B;}_LL3B:;}
# 1676
if(!((unsigned)name)&& !anon_enum)continue;
# 1681
if(({int(*_Tmp2)(struct Cyc_Set_Set*,struct _fat_ptr*)=({int(*_Tmp3)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp3;});struct Cyc_Set_Set*_Tmp3=reachable_set;_Tmp2(_Tmp3,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*user_decl=Cyc_Absyn_lookup_decl(user_decls,name);
if(user_decl==0)
_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_Tmp2->tag=Cyc_Core_Impossible,_Tmp2->f1=_tag_fat("Internal Error: bad user-def name",sizeof(char),34U);_Tmp2;}));else{
# 1687
void*_Tmp2=user_decl->r;switch(*((int*)_Tmp2)){case 0:
 goto _LL71;case 1: _LL71:
# 1690
 _throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp3->tag=Cyc_NO_SUPPORT,_Tmp3->f1=_tag_fat("user defintions for function or variable decls",sizeof(char),47U);_Tmp3;}));default:
 goto _LL6D;}_LL6D:;}
# 1696
print_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=decl,_Tmp2->tl=print_decls;_Tmp2;});}else{
# 1699
print_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=decl,_Tmp2->tl=print_decls;_Tmp2;});}
names=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=name,_Tmp2->tl=names;_Tmp2;});}}
# 1704
{struct _handler_cons _Tmp2;_push_handler(& _Tmp2);{int _Tmp3=0;if(setjmp(_Tmp2.handler))_Tmp3=1;if(!_Tmp3){
Cyc_Binding_resolve_all(print_decls);
({struct Cyc_Tcenv_Tenv*_Tmp4=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_Tmp4,1,print_decls);});
# 1705
;_pop_handler();}else{void*_Tmp4=(void*)Cyc_Core_get_exn_thrown();
# 1708
_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp5->tag=Cyc_NO_SUPPORT,_Tmp5->f1=_tag_fat("can't typecheck acquired declarations",sizeof(char),38U);_Tmp5;}));;}}}
# 1712
{struct _tuple0 _Tmp2=({struct _tuple0 _Tmp3;_Tmp3.f0=print_decls,_Tmp3.f1=names;_Tmp3;});void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct Cyc_List_List*d=_Tmp4;struct Cyc_List_List*n=_Tmp3;for(0;
d!=0 && n!=0;(d=d->tl,n=n->tl)){
struct Cyc_Absyn_Decl*decl=(struct Cyc_Absyn_Decl*)d->hd;
struct _fat_ptr*name=(struct _fat_ptr*)n->hd;
int anon_enum=0;
if(!((unsigned)name))
anon_enum=1;
# 1721
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned)name){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*name;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("_%s_def_",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=ifdefmacro;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(out_file,_tag_fat("#ifndef %s\n",sizeof(char),12U),_tag_fat(_Tmp6,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=ifdefmacro;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(out_file,_tag_fat("#define %s\n",sizeof(char),12U),_tag_fat(_Tmp6,sizeof(void*),1));});
# 1727
({struct Cyc_List_List*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6[1];_Tmp6[0]=decl;Cyc_List_list(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Decl*),1));});Cyc_Absynpp_decllist2file(_Tmp5,out_file);});
Cyc_fprintf(out_file,_tag_fat("#endif\n",sizeof(char),8U),_tag_fat(0U,sizeof(void*),0));}else{
# 1732
({struct Cyc_List_List*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6[1];_Tmp6[0]=decl;Cyc_List_list(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Decl*),1));});Cyc_Absynpp_decllist2file(_Tmp5,out_file);});}}}}
# 1737
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp2->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=macrosfile;_tag_fat((void*)_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("can't open macrosfile %s",sizeof(char),25U),_tag_fat(_Tmp5,sizeof(void*),1));});_Tmp2->f1=_Tmp3;});_Tmp2;}));
# 1740
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);{
struct _tuple14*entry2;
while((entry2=Cyc_suck_line(l))!=0){
{struct _tuple14*_Tmp2=entry2;void*_Tmp3;struct _fat_ptr _Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct _fat_ptr line=_Tmp4;struct _fat_ptr*name=_Tmp3;
if(({int(*_Tmp5)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp5;})(reachable_set,name)){
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*name;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(out_file,_tag_fat("#ifndef %s\n",sizeof(char),12U),_tag_fat(_Tmp6,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=line;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(out_file,_tag_fat("%s\n",sizeof(char),4U),_tag_fat(_Tmp6,sizeof(void*),1));});
Cyc_fprintf(out_file,_tag_fat("#endif\n",sizeof(char),8U),_tag_fat(0U,sizeof(void*),0));}}}
# 1744
1U;}
# 1751
Cyc_fclose(in_file);
if((int)Cyc_mode!=3);
# 1754
if(hstubs!=0){
struct Cyc_List_List*x=hstubs;for(0;x!=0;x=x->tl){
struct _tuple15*_Tmp2=(struct _tuple15*)x->hd;struct _fat_ptr _Tmp3;struct _fat_ptr _Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct _fat_ptr symbol=_Tmp4;struct _fat_ptr text=_Tmp3;
if((char*)text.curr!=0){
if((char*)symbol.curr==0)
# 1760
({const char*_Tmp5=(const char*)_untag_fat_ptr_check_bound(text,sizeof(char),1U);Cyc_fputs(_Tmp5,out_file);});else{
if(({int(*_Tmp5)(struct Cyc_Set_Set*,struct _fat_ptr*)=({int(*_Tmp6)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp6;});struct Cyc_Set_Set*_Tmp6=defined_symbols;_Tmp5(_Tmp6,({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=symbol;_Tmp7;}));}))
({const char*_Tmp5=(const char*)_untag_fat_ptr_check_bound(text,sizeof(char),1U);Cyc_fputs(_Tmp5,out_file);});else{
# 1764
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=symbol;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_log(_tag_fat("%s is not supported on this platform\n",sizeof(char),38U),_tag_fat(_Tmp6,sizeof(void*),1));});{
struct Cyc_Set_Set*x=defined_symbols;for(0;!Cyc_Set_is_empty(x);1){
struct _fat_ptr*y=({struct _fat_ptr*(*_Tmp5)(struct Cyc_Set_Set*)=(struct _fat_ptr*(*)(struct Cyc_Set_Set*))Cyc_Set_choose;_Tmp5;})(x);x=({struct Cyc_Set_Set*(*_Tmp5)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_delete;_Tmp5;})(x,y);
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*y;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_log(_tag_fat("+%s",sizeof(char),4U),_tag_fat(_Tmp6,sizeof(void*),1));});
Cyc_log(_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}}}}}else{
# 1773
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=symbol;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_log(_tag_fat("Null text for %s, will not be supported on this platform\n",sizeof(char),58U),_tag_fat(_Tmp6,sizeof(void*),1));});}}}}
# 1777
Cyc_fprintf(out_file,_tag_fat("#endif\n",sizeof(char),8U),_tag_fat(0U,sizeof(void*),0));
if(!Cyc_do_setjmp)
Cyc_fprintf(out_file,_tag_fat("}\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_do_setjmp){int _Tmp2=0;_npop_handler(0);return _Tmp2;}else{
Cyc_fclose(out_file);}
# 1784
if(cstubs!=0){
out_file=_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*x=cstubs;for(0;x!=0;x=x->tl){
struct _tuple15*_Tmp2=(struct _tuple15*)x->hd;struct _fat_ptr _Tmp3;struct _fat_ptr _Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct _fat_ptr symbol=_Tmp4;struct _fat_ptr text=_Tmp3;
if((char*)text.curr!=0 &&(
(char*)symbol.curr==0 ||({int(*_Tmp5)(struct Cyc_Set_Set*,struct _fat_ptr*)=({int(*_Tmp6)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp6;});struct Cyc_Set_Set*_Tmp6=defined_symbols;_Tmp5(_Tmp6,({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=symbol;_Tmp7;}));})))
({const char*_Tmp5=(const char*)_untag_fat_ptr_check_bound(text,sizeof(char),1U);Cyc_fputs(_Tmp5,out_file);});}}}}
# 1795
out_file=_check_null(Cyc_cycstubs_file);
if(cycstubs!=0){
# 1800
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=filename;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(out_file,_tag_fat("#include <%s>\n\n",sizeof(char),16U),_tag_fat(_Tmp3,sizeof(void*),1));});
out_file=_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*x=cycstubs;for(0;x!=0;x=x->tl){
struct _tuple15*_Tmp2=(struct _tuple15*)x->hd;struct _fat_ptr _Tmp3;struct _fat_ptr _Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct _fat_ptr symbol=_Tmp4;struct _fat_ptr text=_Tmp3;
if((char*)text.curr!=0 &&(
(char*)symbol.curr==0 ||({int(*_Tmp5)(struct Cyc_Set_Set*,struct _fat_ptr*)=({int(*_Tmp6)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp6;});struct Cyc_Set_Set*_Tmp6=defined_symbols;_Tmp5(_Tmp6,({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=symbol;_Tmp7;}));})))
({const char*_Tmp5=(const char*)_untag_fat_ptr_check_bound(text,sizeof(char),1U);Cyc_fputs(_Tmp5,out_file);});}}}
# 1808
Cyc_fprintf(out_file,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}{
# 1811
int _Tmp2=0;_npop_handler(0);return _Tmp2;}}}}}}}}}
# 1419
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;struct _fat_ptr _Tmp4;if(((struct Cyc_Core_Impossible_exn_struct*)_Tmp2)->tag==Cyc_Core_Impossible){_Tmp4=((struct Cyc_Core_Impossible_exn_struct*)_Tmp2)->f1;{struct _fat_ptr s=_Tmp4;
# 1815
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,_tag_fat("Got Core::Impossible(%s)\n",sizeof(char),26U),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL86;}}else{if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 1817
Cyc_fprintf(Cyc_stderr,_tag_fat("Got Dict::Absent\n",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));goto _LL86;}else{if(((struct Cyc_Core_Failure_exn_struct*)_Tmp2)->tag==Cyc_Core_Failure){_Tmp4=((struct Cyc_Core_Failure_exn_struct*)_Tmp2)->f1;{struct _fat_ptr s=_Tmp4;
# 1819
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,_tag_fat("Got Core::Failure(%s)\n",sizeof(char),23U),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL86;}}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_Tmp2)->tag==Cyc_Core_Invalid_argument){_Tmp4=((struct Cyc_Core_Invalid_argument_exn_struct*)_Tmp2)->f1;{struct _fat_ptr s=_Tmp4;
# 1821
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,_tag_fat("Got Invalid_argument(%s)\n",sizeof(char),26U),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL86;}}else{if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 1823
Cyc_fprintf(Cyc_stderr,_tag_fat("Got Not_found\n",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));goto _LL86;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_Tmp2)->tag==Cyc_NO_SUPPORT){_Tmp4=((struct Cyc_NO_SUPPORT_exn_struct*)_Tmp2)->f1;{struct _fat_ptr s=_Tmp4;
# 1825
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,_tag_fat("No support because %s\n",sizeof(char),23U),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL86;}}else{if(((struct Cyc_Lexing_Error_exn_struct*)_Tmp2)->tag==Cyc_Lexing_Error){_Tmp4=((struct Cyc_Lexing_Error_exn_struct*)_Tmp2)->f1;{struct _fat_ptr s=_Tmp4;
# 1827
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,_tag_fat("Got a lexing error %s\n",sizeof(char),23U),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL86;}}else{_Tmp3=_Tmp2;{void*x=_Tmp3;
# 1829
Cyc_fprintf(Cyc_stderr,_tag_fat("Got unknown exception\n",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));
Cyc_Core_rethrow(x);}}}}}}}}_LL86:;}}}
# 1835
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not create file %s\n",sizeof(char),33U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 1840
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(out_file,_tag_fat("#error -- %s is not supported on this platform\n",sizeof(char),48U),_tag_fat(_Tmp1,sizeof(void*),1));});
# 1844
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Warning: %s will not be supported on this platform\n",sizeof(char),52U),_tag_fat(_Tmp1,sizeof(void*),1));});
# 1847
Cyc_log(_tag_fat("Not supported on this platform\n",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
# 1854
return 0;}}
# 1858
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=file;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Processing %s\n",sizeof(char),15U),_tag_fat(_Tmp1,sizeof(void*),1));});{
struct Cyc___cycFILE*maybe=Cyc_fopen(file,"r");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=file;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not open %s\n",sizeof(char),26U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}{
# 1866
struct Cyc___cycFILE*in_file=maybe;
# 1870
struct _fat_ptr buf=_tag_fat(({unsigned _Tmp0=1024U + 1U;({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=1024U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='\000';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;});}),sizeof(char),1025U);
struct _fat_ptr cwd=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));
if((int)Cyc_mode!=2){
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=dir;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: can't change directory to %s\n",sizeof(char),37U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}}
# 1878
if((int)Cyc_mode==1){
# 1880
struct _fat_ptr cmd=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_cyclone_cc;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=Cyc_target_cflags;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("echo | %s %s -E -dM - -o INITMACROS.h\n",sizeof(char),39U),_tag_fat(_Tmp2,sizeof(void*),2));});
# 1882
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cmd;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s\n",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});
system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)));}{
# 1887
struct Cyc_Lexing_lexbuf*l=Cyc_Lexing_from_file(in_file);
struct _tuple17*entry;
while((entry=Cyc_spec(l))!=0){
{struct _tuple17*_Tmp0=entry;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;struct _fat_ptr _Tmp8;_Tmp8=_Tmp0->f0;_Tmp7=_Tmp0->f1;_Tmp6=_Tmp0->f2;_Tmp5=_Tmp0->f3;_Tmp4=_Tmp0->f4;_Tmp3=_Tmp0->f5;_Tmp2=_Tmp0->f6;_Tmp1=_Tmp0->f7;{struct _fat_ptr headerfile=_Tmp8;struct Cyc_List_List*start_symbols=_Tmp7;struct Cyc_List_List*user_defs=_Tmp6;struct Cyc_List_List*omit_symbols=_Tmp5;struct Cyc_List_List*hstubs=_Tmp4;struct Cyc_List_List*cstubs=_Tmp3;struct Cyc_List_List*cycstubs=_Tmp2;struct Cyc_List_List*cpp_insert=_Tmp1;
# 1892
if(Cyc_process_file((const char*)_check_null(_untag_fat_ptr(headerfile,sizeof(char),1U)),start_symbols,user_defs,omit_symbols,hstubs,cstubs,cycstubs,cpp_insert))
# 1894
return 1;}}
# 1890
1U;}
# 1896
Cyc_fclose(in_file);
# 1898
if((int)Cyc_mode!=2){
if(chdir((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U)))){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cwd;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not change directory to %s\n",sizeof(char),41U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}}
# 1904
return 0;}}}}
# 1908
int Cyc_process_setjmp(const char*dir){
# 1911
struct _fat_ptr buf=_tag_fat(({unsigned _Tmp0=1024U + 1U;({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=1024U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='\000';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;});}),sizeof(char),1025U);
struct _fat_ptr cwd=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=dir;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: can't change directory to %s\n",sizeof(char),37U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 1917
if(({struct Cyc_List_List*_Tmp0=({struct _fat_ptr*_Tmp1[1];({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=_tag_fat("jmp_buf",sizeof(char),8U);_Tmp3;});_Tmp1[0]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(struct _fat_ptr*),1));});Cyc_process_file("setjmp.h",_Tmp0,0,0,({struct _tuple15*_Tmp1[1];({struct _tuple15*_Tmp2=({struct _tuple15*_Tmp3=_cycalloc(sizeof(struct _tuple15));
_Tmp3->f0=_tag_fat("setjmp",sizeof(char),7U),_Tmp3->f1=_tag_fat("extern int setjmp(jmp_buf);\n",sizeof(char),29U);_Tmp3;});_Tmp1[0]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(struct _tuple15*),1));}),0,0,0);}))
# 1920
return 1;
if(chdir((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U)))){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cwd;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not change directory to %s\n",sizeof(char),41U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 1925
return 0;}static char _TmpG3[13U]="BUILDLIB.OUT";
# 1929
static struct _fat_ptr Cyc_output_dir={_TmpG3,_TmpG3,_TmpG3 + 13U};
static void Cyc_set_output_dir(struct _fat_ptr s){
Cyc_output_dir=s;}
# 1933
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _fat_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(const char*)_check_null(_untag_fat_ptr_check_bound(s,sizeof(char),1U)),_Tmp0->tl=Cyc_spec_files;_Tmp0;});}
# 1937
static int Cyc_no_other(struct _fat_ptr s){return 0;}
static void Cyc_set_GATHER (void){
Cyc_mode=1U;}
# 1941
static void Cyc_set_GATHERSCRIPT (void){
Cyc_mode=2U;}
# 1944
static void Cyc_set_FINISH (void){
Cyc_mode=3U;}
# 1947
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cppargs;_Tmp0;});}
# 1950
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _fat_ptr s){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Unsupported option %s\n",sizeof(char),23U),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_badparse=1;}
# 1955
static void Cyc_set_header(struct _fat_ptr s){
if(!((unsigned)Cyc_headers_to_do))
Cyc_headers_to_do=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp2;})(Cyc_strptrcmp);*_Tmp0=_Tmp1;});_Tmp0;});
({struct Cyc_Set_Set*_Tmp0=({struct Cyc_Set_Set*(*_Tmp1)(struct Cyc_Set_Set*,struct _fat_ptr*)=({struct Cyc_Set_Set*(*_Tmp2)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;_Tmp2;});struct Cyc_Set_Set*_Tmp2=*Cyc_headers_to_do;_Tmp1(_Tmp2,({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=s;_Tmp3;}));});*_check_null(Cyc_headers_to_do)=_Tmp0;});}
# 1964
extern void GC_blacklist_warn_clear (void);struct _tuple19{struct _fat_ptr f0;int f1;struct _fat_ptr f2;void*f3;struct _fat_ptr f4;};
int Cyc_main(int argc,struct _fat_ptr argv){
GC_blacklist_warn_clear();{
# 1968
struct Cyc_List_List*options=({struct _tuple19*_Tmp0[10];({
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-d",sizeof(char),3U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat(" <file>",sizeof(char),8U),({
void*_Tmp3=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_Tmp4->tag=5,_Tmp4->f1=Cyc_set_output_dir;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Set the output directory to <file>",sizeof(char),35U);_Tmp2;});
# 1969
_Tmp0[0]=_Tmp1;}),({
# 1972
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-gather",sizeof(char),8U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat("",sizeof(char),1U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_Arg_Spec_struct));_Tmp4->tag=0,_Tmp4->f1=Cyc_set_GATHER;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Gather C library info but don't produce Cyclone headers",sizeof(char),56U);_Tmp2;});
# 1972
_Tmp0[1]=_Tmp1;}),({
# 1975
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-gatherscript",sizeof(char),14U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat("",sizeof(char),1U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_Arg_Spec_struct));_Tmp4->tag=0,_Tmp4->f1=Cyc_set_GATHERSCRIPT;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Produce a script to gather C library info",sizeof(char),42U);_Tmp2;});
# 1975
_Tmp0[2]=_Tmp1;}),({
# 1978
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-finish",sizeof(char),8U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat("",sizeof(char),1U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_Arg_Spec_struct));_Tmp4->tag=0,_Tmp4->f1=Cyc_set_FINISH;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Produce Cyclone headers from pre-gathered C library info",sizeof(char),57U);_Tmp2;});
# 1978
_Tmp0[3]=_Tmp1;}),({
# 1981
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-setjmp",sizeof(char),8U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat("",sizeof(char),1U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_Arg_Spec_struct));_Tmp4->tag=3,_Tmp4->f1=& Cyc_do_setjmp;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
# 1986
_tag_fat("Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",sizeof(char),186U);_Tmp2;});
# 1981
_Tmp0[4]=_Tmp1;}),({
# 1987
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-b",sizeof(char),3U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat(" <machine>",sizeof(char),11U),({
void*_Tmp3=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_Tmp4->tag=5,_Tmp4->f1=Cyc_Specsfile_set_target_arch;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Set the target machine for compilation to <machine>",sizeof(char),52U);_Tmp2;});
# 1987
_Tmp0[5]=_Tmp1;}),({
# 1990
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-B",sizeof(char),3U),_Tmp2->f1=1,_Tmp2->f2=_tag_fat("<file>",sizeof(char),7U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_Tmp4->tag=1,_Tmp4->f1=Cyc_Specsfile_add_cyclone_exec_path;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Add to the list of directories to search for compiler files",sizeof(char),60U);_Tmp2;});
# 1990
_Tmp0[6]=_Tmp1;}),({
# 1993
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-h",sizeof(char),3U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat(" <header>",sizeof(char),10U),({
void*_Tmp3=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_Tmp4->tag=5,_Tmp4->f1=Cyc_set_header;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Produce this header, and other -h headers only",sizeof(char),47U);_Tmp2;});
# 1993
_Tmp0[7]=_Tmp1;}),({
# 1996
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-v",sizeof(char),3U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat("",sizeof(char),1U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_Arg_Spec_struct));_Tmp4->tag=3,_Tmp4->f1=& Cyc_verbose;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Verbose operation",sizeof(char),18U);_Tmp2;});
# 1996
_Tmp0[8]=_Tmp1;}),({
# 1999
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=_tag_fat("-",sizeof(char),2U),_Tmp2->f1=1,_Tmp2->f2=_tag_fat("",sizeof(char),1U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_Tmp4->tag=1,_Tmp4->f1=Cyc_add_cpparg;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("",sizeof(char),1U);_Tmp2;});
# 1999
_Tmp0[9]=_Tmp1;});Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _tuple19*),10));});
# 2004
struct _fat_ptr otherargs=Cyc_Specsfile_parse_b(options,Cyc_add_spec_file,Cyc_no_other,_tag_fat("Options:",sizeof(char),9U),argv);
# 2006
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_spec_file,Cyc_no_other,_tag_fat("Options:",sizeof(char),9U),otherargs);
if((((Cyc_badparse ||
 !Cyc_do_setjmp && Cyc_spec_files==0)||
 Cyc_do_setjmp && Cyc_spec_files!=0)||
 Cyc_do_setjmp &&(int)Cyc_mode==1)||
 Cyc_do_setjmp &&(int)Cyc_mode==2){
Cyc_Arg_usage(options,
# 2015
_tag_fat("Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",sizeof(char),59U));
return 1;}{
# 2022
struct _fat_ptr specs_file=Cyc_Specsfile_find_in_arch_path(_tag_fat("cycspecs",sizeof(char),9U));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=specs_file;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Reading from specs file %s\n",sizeof(char),28U),_tag_fat(_Tmp1,sizeof(void*),1));});{
struct Cyc_List_List*specs=Cyc_Specsfile_read_specs(specs_file);
Cyc_target_cflags=Cyc_Specsfile_get_spec(specs,_tag_fat("cyclone_target_cflags",sizeof(char),22U));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_target_cflags;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Target cflags are %s\n",sizeof(char),22U),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_cyclone_cc=Cyc_Specsfile_get_spec(specs,_tag_fat("cyclone_cc",sizeof(char),11U));
if(!((unsigned)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=_tag_fat("gcc",sizeof(char),4U);
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_cyclone_cc;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("C compiler is %s\n",sizeof(char),18U),_tag_fat(_Tmp1,sizeof(void*),1));});
# 2031
if((int)Cyc_mode==2){
if(Cyc_verbose)
Cyc_fprintf(Cyc_stderr,_tag_fat("Creating BUILDLIB.sh\n",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned)Cyc_script_file)){
Cyc_fprintf(Cyc_stderr,_tag_fat("Could not create file BUILDLIB.sh\n",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
exit(1);}
# 2039
Cyc_prscript(_tag_fat("#!/bin/sh\n",sizeof(char),11U),_tag_fat(0U,sizeof(void*),0));
Cyc_prscript(_tag_fat("GCC=\"gcc\"\n",sizeof(char),11U),_tag_fat(0U,sizeof(void*),0));}
# 2044
if(Cyc_force_directory_prefixes(Cyc_output_dir)|| Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not create directory %s\n",sizeof(char),38U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 2048
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Output directory is %s\n",sizeof(char),24U),_tag_fat(_Tmp1,sizeof(void*),1));});
# 2051
if((int)Cyc_mode==2){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_prscript(_tag_fat("cd %s\n",sizeof(char),7U),_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_target_cflags;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_prscript(_tag_fat("echo | $GCC %s -E -dM - -o INITMACROS.h\n",sizeof(char),41U),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 2057
if(!Cyc_gathering()){
# 2060
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr_check_bound(Cyc_Filename_concat(Cyc_output_dir,_tag_fat("BUILDLIB.LOG",sizeof(char),13U)),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not create log file in directory %s\n",sizeof(char),50U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 2066
if(!Cyc_do_setjmp){
# 2068
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr_check_bound(Cyc_Filename_concat(Cyc_output_dir,_tag_fat("cstubs.c",sizeof(char),9U)),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not create cstubs.c in directory %s\n",sizeof(char),50U),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 2075
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr_check_bound(Cyc_Filename_concat(Cyc_output_dir,_tag_fat("cycstubs.cyc",sizeof(char),13U)),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Error: could not create cycstubs.c in directory %s\n",sizeof(char),52U),_tag_fat(_Tmp1,sizeof(void*),1));});
# 2080
return 1;}
# 2082
Cyc_fprintf(Cyc_cycstubs_file,
# 2084
_tag_fat("#include <core.h>\nusing Core;\n\n",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));}}{
# 2089
const char*outdir=(const char*)_check_null(_untag_fat_ptr_check_bound(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp && Cyc_process_setjmp(outdir))
return 1;else{
# 2095
for(1;Cyc_spec_files!=0;Cyc_spec_files=Cyc_spec_files->tl){
if(Cyc_process_specfile((const char*)Cyc_spec_files->hd,outdir)){
Cyc_fprintf(Cyc_stderr,_tag_fat("FATAL ERROR -- QUIT!\n",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));
exit(1);}}}
# 2103
if((int)Cyc_mode==2)
Cyc_fclose(_check_null(Cyc_script_file));else{
# 2106
if(!Cyc_gathering()){
Cyc_fclose(_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose(_check_null(Cyc_cstubs_file));
Cyc_fclose(_check_null(Cyc_cycstubs_file));}}}
# 2114
return 0;}}}}}

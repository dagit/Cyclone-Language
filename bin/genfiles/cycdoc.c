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

# 95 "core.h"
 struct _fat_ptr Cyc_Core_new_string(unsigned);struct _tuple0{void*f0;void*f1;};
# 113
void*Cyc_Core_fst(struct _tuple0*);
# 115
void*Cyc_Core_snd(struct _tuple0*);struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
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
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 165
extern int puts(const char*);
# 167
extern int remove(const char*);
# 272 "cycboot.h"
extern void Cyc_file_close(struct Cyc___cycFILE*);
# 318
extern int system(const char*);
extern void exit(int);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 78 "lexing.h"
extern struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 84
extern int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 110 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_Lineno_Pos{struct _fat_ptr logical_file;struct _fat_ptr line;int line_no;int col;};
# 32 "lineno.h"
void Cyc_Lineno_poss_of_abss(struct _fat_ptr,struct Cyc_List_List*);
# 34 "filename.h"
extern struct _fat_ptr Cyc_Filename_chop_extension(struct _fat_ptr);struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};
# 66 "arg.h"
extern void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 25 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Doc;
# 53
extern struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*,int);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 52 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 54
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*);
# 27 "warn.h"
extern void Cyc_Warn_reset(struct _fat_ptr);
# 30 "cycdoc.cyl"
extern void Cyc_Lex_lex_init(int);struct Cyc_MatchDecl_Comment_struct{int tag;struct _fat_ptr f1;};struct Cyc_Standalone_Comment_struct{int tag;struct _fat_ptr f1;};struct _tuple12{int f0;void*f1;};
# 45
struct _tuple12*Cyc_token(struct Cyc_Lexing_lexbuf*);
# 48
const int Cyc_lex_base[15U]={0,- 4,0,- 3,1,2,3,0,4,6,7,- 1,8,9,- 2};
const int Cyc_lex_backtrk[15U]={- 1,- 1,3,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[15U]={1,0,- 1,0,- 1,- 1,6,- 1,8,8,8,0,6,6,0};
const int Cyc_lex_trans[266U]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,6,0,0,0,0,0,0,0,4,5,7,12,9,2,10,10,13,13,0,11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};
const int Cyc_lex_check[266U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,4,5,6,8,0,9,10,12,13,- 1,9,10,12,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,6,8,- 1,9,10,12,13};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){struct Cyc_Lexing_lexbuf*_T0;struct Cyc_Lexing_lexbuf*_T1;struct Cyc_Lexing_lexbuf*_T2;struct Cyc_Lexing_lexbuf*_T3;int _T4;const int*_T5;int _T6;const char*_T7;const int*_T8;int _T9;int _TA;const int*_TB;int _TC;const char*_TD;const int*_TE;struct Cyc_Lexing_lexbuf*_TF;struct Cyc_Lexing_lexbuf*_T10;struct Cyc_Lexing_lexbuf*_T11;struct Cyc_Lexing_lexbuf*_T12;int _T13;struct Cyc_Lexing_lexbuf*_T14;int _T15;struct Cyc_Lexing_lexbuf*_T16;int _T17;int _T18;int _T19;struct Cyc_Lexing_lexbuf*_T1A;struct _fat_ptr _T1B;struct Cyc_Lexing_lexbuf*_T1C;int _T1D;int _T1E;char*_T1F;char*_T20;char _T21;const int*_T22;int _T23;const char*_T24;const int*_T25;int _T26;int _T27;const int*_T28;int _T29;const char*_T2A;const int*_T2B;const int*_T2C;int _T2D;const char*_T2E;const int*_T2F;struct Cyc_Lexing_lexbuf*_T30;struct Cyc_Lexing_lexbuf*_T31;struct Cyc_Lexing_lexbuf*_T32;int _T33;struct Cyc_Lexing_Error_exn_struct*_T34;void*_T35;struct Cyc_Lexing_lexbuf*_T36;int _T37;struct Cyc_Lexing_lexbuf*_T38;
# 55
int state;int base;int backtrk;
int c;
state=start_state;
# 59
if(state < 0)goto _TL0;_T0=lbuf;_T1=lbuf;_T2=lbuf;
_T1->lex_start_pos=_T2->lex_curr_pos;_T0->lex_last_pos=_T1->lex_start_pos;_T3=lbuf;
_T3->lex_last_action=-1;goto _TL1;
# 63
_TL0: _T4=- state;state=_T4 - 1;_TL1:
# 65
 _TL2: if(1)goto _TL3;else{goto _TL4;}
_TL3: _T5=Cyc_lex_base;_T6=state;_T7=_check_known_subscript_notnull(_T5,15U,sizeof(int),_T6);_T8=(const int*)_T7;base=*_T8;
if(base >= 0)goto _TL5;_T9=- base;_TA=_T9 - 1;return _TA;_TL5: _TB=Cyc_lex_backtrk;_TC=state;_TD=_check_known_subscript_notnull(_TB,15U,sizeof(int),_TC);_TE=(const int*)_TD;
backtrk=*_TE;
if(backtrk < 0)goto _TL7;_TF=lbuf;_T10=lbuf;
_TF->lex_last_pos=_T10->lex_curr_pos;_T11=lbuf;
_T11->lex_last_action=backtrk;goto _TL8;_TL7: _TL8: _T12=lbuf;_T13=_T12->lex_curr_pos;_T14=lbuf;_T15=_T14->lex_buffer_len;
# 73
if(_T13 < _T15)goto _TL9;_T16=lbuf;_T17=_T16->lex_eof_reached;
if(_T17)goto _TLB;else{goto _TLD;}
_TLD: _T18=- state;_T19=_T18 - 1;return _T19;
# 77
_TLB: c=256;goto _TLA;
# 79
_TL9: _T1A=lbuf;_T1B=_T1A->lex_buffer;_T1C=lbuf;_T1D=_T1C->lex_curr_pos;_T1C->lex_curr_pos=_T1D + 1;_T1E=_T1D;_T1F=_check_fat_subscript(_T1B,sizeof(char),_T1E);_T20=(char*)_T1F;_T21=*_T20;c=(int)_T21;
if(c!=-1)goto _TLE;c=256;goto _TLF;
_TLE: if(c >= 0)goto _TL10;c=256 + c;goto _TL11;_TL10: _TL11: _TLF: _TLA: _T22=Cyc_lex_check;_T23=base + c;_T24=_check_known_subscript_notnull(_T22,266U,sizeof(int),_T23);_T25=(const int*)_T24;_T26=*_T25;_T27=state;
# 83
if(_T26!=_T27)goto _TL12;_T28=Cyc_lex_trans;_T29=base + c;_T2A=_check_known_subscript_notnull(_T28,266U,sizeof(int),_T29);_T2B=(const int*)_T2A;
state=*_T2B;goto _TL13;
# 86
_TL12: _T2C=Cyc_lex_default;_T2D=state;_T2E=_check_known_subscript_notnull(_T2C,15U,sizeof(int),_T2D);_T2F=(const int*)_T2E;state=*_T2F;_TL13:
 if(state >= 0)goto _TL14;_T30=lbuf;_T31=lbuf;
_T30->lex_curr_pos=_T31->lex_last_pos;_T32=lbuf;_T33=_T32->lex_last_action;
if(_T33!=-1)goto _TL16;{struct Cyc_Lexing_Error_exn_struct*_T39=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T39->tag=Cyc_Lexing_Error;
_T39->f1=_tag_fat("empty token",sizeof(char),12U);_T34=(struct Cyc_Lexing_Error_exn_struct*)_T39;}_T35=(void*)_T34;_throw(_T35);goto _TL17;
# 92
_TL16: _T36=lbuf;_T37=_T36->lex_last_action;return _T37;_TL17: goto _TL15;
# 95
_TL14: if(c!=256)goto _TL18;_T38=lbuf;_T38->lex_eof_reached=0;goto _TL19;_TL18: _TL19: _TL15: goto _TL2;_TL4:;}
# 99
struct _tuple12*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct Cyc_Lexing_lexbuf*_T2;int _T3;struct Cyc_Lexing_lexbuf*_T4;int _T5;struct Cyc_Lexing_lexbuf*_T6;int _T7;int _T8;struct _tuple12*_T9;struct Cyc_Standalone_Comment_struct*_TA;struct Cyc_Lexing_lexbuf*_TB;struct _fat_ptr _TC;int _TD;int _TE;unsigned long _TF;struct Cyc_Lexing_lexbuf*_T10;int _T11;struct Cyc_Lexing_lexbuf*_T12;int _T13;struct Cyc_Lexing_lexbuf*_T14;int _T15;int _T16;struct _tuple12*_T17;struct Cyc_MatchDecl_Comment_struct*_T18;struct Cyc_Lexing_lexbuf*_T19;struct _fat_ptr _T1A;int _T1B;int _T1C;unsigned long _T1D;struct _tuple12*_T1E;struct Cyc_Lexing_lexbuf*_T1F;void(*_T20)(struct Cyc_Lexing_lexbuf*);struct _tuple12*_T21;struct Cyc_Lexing_Error_exn_struct*_T22;void*_T23;
lexstate=Cyc_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0: _T2=lexbuf;_T3=_T2->lex_start_pos;{
# 52 "cycdoc.cyl"
int start=_T3 + 5;_T4=lexbuf;_T5=_T4->lex_curr_pos;_T6=lexbuf;_T7=_T6->lex_start_pos;_T8=_T5 - _T7;{
int len=_T8 - 7;{struct _tuple12*_T24=_cycalloc(sizeof(struct _tuple12));
_T24->f0=Cyc_Lexing_lexeme_start(lexbuf);{struct Cyc_Standalone_Comment_struct*_T25=_cycalloc(sizeof(struct Cyc_Standalone_Comment_struct));_T25->tag=1;_TB=lexbuf;_TC=_TB->lex_buffer;_TD=start;_TE=len;_TF=(unsigned long)_TE;
_T25->f1=Cyc_substring(_TC,_TD,_TF);_TA=(struct Cyc_Standalone_Comment_struct*)_T25;}_T24->f1=(void*)_TA;_T9=(struct _tuple12*)_T24;}
# 54
return _T9;}}case 1: _T10=lexbuf;_T11=_T10->lex_start_pos;{
# 57
int start=_T11 + 4;_T12=lexbuf;_T13=_T12->lex_curr_pos;_T14=lexbuf;_T15=_T14->lex_start_pos;_T16=_T13 - _T15;{
int len=_T16 - 6;{struct _tuple12*_T24=_cycalloc(sizeof(struct _tuple12));
_T24->f0=Cyc_Lexing_lexeme_start(lexbuf);{struct Cyc_MatchDecl_Comment_struct*_T25=_cycalloc(sizeof(struct Cyc_MatchDecl_Comment_struct));_T25->tag=0;_T19=lexbuf;_T1A=_T19->lex_buffer;_T1B=start;_T1C=len;_T1D=(unsigned long)_T1C;
_T25->f1=Cyc_substring(_T1A,_T1B,_T1D);_T18=(struct Cyc_MatchDecl_Comment_struct*)_T25;}_T24->f1=(void*)_T18;_T17=(struct _tuple12*)_T24;}
# 59
return _T17;}}case 2:
# 61 "cycdoc.cyl"
 return 0;case 3: _T1E=
# 62 "cycdoc.cyl"
Cyc_token(lexbuf);return _T1E;default: _T1F=lexbuf;_T20=_T1F->refill_buff;
_T20(lexbuf);_T21=
Cyc_token_rec(lexbuf,lexstate);return _T21;}{struct Cyc_Lexing_Error_exn_struct*_T24=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T24->tag=Cyc_Lexing_Error;
# 66
_T24->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T22=(struct Cyc_Lexing_Error_exn_struct*)_T24;}_T23=(void*)_T22;_throw(_T23);}
# 68
struct _tuple12*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){struct _tuple12*_T0;_T0=Cyc_token_rec(lexbuf,0);return _T0;}
# 66 "cycdoc.cyl"
static void Cyc_dump_begin (void){
# 68
puts("%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}");}
# 82
static void Cyc_dump_middle (void){
puts("\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}");}
# 89
static void Cyc_dump_end (void){
# 91
puts("%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}");}
# 100
static void Cyc_pr_comment(struct Cyc___cycFILE*outf,struct _fat_ptr s){unsigned long _T0;struct _fat_ptr _T1;int _T2;char*_T3;const char*_T4;char _T5;int _T6;char _T7;int _T8;struct Cyc___cycFILE*_T9;struct _fat_ptr _TA;int _TB;char*_TC;const char*_TD;char _TE;int _TF;char _T10;int _T11;char _T12;int _T13;struct Cyc___cycFILE*_T14;
int depth=0;_T0=
Cyc_strlen(s);{int len=(int)_T0;
int i=0;_TL1E: if(i < len)goto _TL1C;else{goto _TL1D;}
_TL1C: _T1=s;_T2=i;_T3=_check_fat_subscript(_T1,sizeof(char),_T2);_T4=(const char*)_T3;{char c=*_T4;_T5=c;_T6=(int)_T5;
if(_T6==91)goto _TL1F;_T7=c;_T8=(int)_T7;_T9=outf;Cyc_fputc(_T8,_T9);goto _TL1B;_TL1F:
 Cyc_fputs("\\texttt{",outf);
i=i + 1;
depth=depth + 1;
_TL24: if(i < len)goto _TL22;else{goto _TL23;}
_TL22: _TA=s;_TB=i;_TC=_check_fat_subscript(_TA,sizeof(char),_TB);_TD=(const char*)_TC;{char c=*_TD;_TE=c;_TF=(int)_TE;
if(_TF!=93)goto _TL25;
depth=depth + -1;
if(depth!=0)goto _TL27;
Cyc_fputc(125,outf);goto _TL23;_TL27: goto _TL26;
# 118
_TL25: _T10=c;_T11=(int)_T10;if(_T11!=91)goto _TL29;depth=depth + 1;goto _TL2A;_TL29: _TL2A: _TL26: _T12=c;_T13=(int)_T12;_T14=outf;
Cyc_fputc(_T13,_T14);}
# 109
i=i + 1;goto _TL24;_TL23:;}_TL1B:
# 103
 i=i + 1;goto _TL1E;_TL1D:;}}
# 125
static int Cyc_width=50;
static void Cyc_set_width(int w){
Cyc_width=w;}
# 129
static struct Cyc_List_List*Cyc_cycdoc_files=0;
static void Cyc_add_other(struct _fat_ptr s){struct Cyc_List_List*_T0;struct _fat_ptr*_T1;{struct Cyc_List_List*_T2=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T3=_cycalloc(sizeof(struct _fat_ptr));
*_T3=s;_T1=(struct _fat_ptr*)_T3;}_T2->hd=_T1;_T2->tl=Cyc_cycdoc_files;_T0=(struct Cyc_List_List*)_T2;}Cyc_cycdoc_files=_T0;}
# 133
static int Cyc_no_other(struct _fat_ptr s){
return 0;}
# 136
static struct Cyc_List_List*Cyc_cycargs=0;
static void Cyc_add_cycarg(struct _fat_ptr s){struct Cyc_List_List*_T0;struct _fat_ptr*_T1;{struct Cyc_List_List*_T2=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T3=_cycalloc(sizeof(struct _fat_ptr));
*_T3=s;_T1=(struct _fat_ptr*)_T3;}_T2->hd=_T1;_T2->tl=Cyc_cycargs;_T0=(struct Cyc_List_List*)_T2;}Cyc_cycargs=_T0;}static char _TmpG0[8U]="cyclone";
# 140
static struct _fat_ptr Cyc_cyclone_file={_TmpG0,_TmpG0,_TmpG0 + 8U};
static void Cyc_set_cyclone_file(struct _fat_ptr s){
Cyc_cyclone_file=s;}
# 146
static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _fat_ptr comment){struct Cyc_String_pa_PrintArg_struct _T0;struct Cyc_PP_Doc*_T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;
Cyc_dump_begin();{struct Cyc_String_pa_PrintArg_struct _T7;_T7.tag=0;_T1=
# 152
Cyc_Absynpp_decl2doc(d);_T7.f1=Cyc_PP_string_of_doc(_T1,50);_T0=_T7;}{struct Cyc_String_pa_PrintArg_struct _T7=_T0;void*_T8[1];_T2=_T8 + 0;*_T2=& _T7;_T3=_tag_fat("%s",sizeof(char),3U);_T4=_tag_fat(_T8,sizeof(void*),1);Cyc_printf(_T3,_T4);}
Cyc_dump_middle();
Cyc_pr_comment(Cyc_stdout,comment);_T5=
_tag_fat("\n",sizeof(char),2U);_T6=_tag_fat(0U,sizeof(void*),0);Cyc_printf(_T5,_T6);
Cyc_dump_end();}
# 160
static int Cyc_is_other_special(char c){char _T0;int _T1;_T0=c;_T1=(int)_T0;switch(_T1){case 92: goto _LL4;case 34: _LL4: goto _LL6;case 59: _LL6: goto _LL8;case 38: _LL8: goto _LLA;case 40: _LLA: goto _LLC;case 41: _LLC: goto _LLE;case 124: _LLE: goto _LL10;case 94: _LL10: goto _LL12;case 60: _LL12: goto _LL14;case 62: _LL14: goto _LL16;case 32: _LL16: goto _LL18;case 10: _LL18: goto _LL1A;case 9: _LL1A:
# 174
 return 1;default:
 return 0;};}
# 179
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;unsigned char*_T4;const char*_T5;int _T6;char _T7;int _T8;int _T9;struct _fat_ptr _TA;struct Cyc_List_List*_TB;struct _fat_ptr**_TC;struct _fat_ptr*_TD;struct _fat_ptr**_TE;struct _fat_ptr*_TF;struct _fat_ptr _T10;struct _fat_ptr**_T11;struct _fat_ptr*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;unsigned long _T15;int _T16;unsigned long _T17;unsigned long _T18;int _T19;unsigned long _T1A;struct _fat_ptr _T1B;unsigned long _T1C;char*_T1D;unsigned _T1E;char*_T1F;unsigned _T20;char*_T21;int _T22;unsigned long _T23;unsigned long _T24;struct _fat_ptr _T25;unsigned char*_T26;const char*_T27;int _T28;char _T29;int _T2A;int _T2B;struct _fat_ptr _T2C;int _T2D;int _T2E;char*_T2F;char*_T30;unsigned _T31;unsigned char*_T32;char*_T33;struct _fat_ptr _T34;int _T35;int _T36;char*_T37;char*_T38;unsigned _T39;unsigned char*_T3A;char*_T3B;struct _fat_ptr _T3C;
unsigned long len=Cyc_strlen(s);
# 183
int single_quotes=0;
int other_special=0;{
int i=0;_TL2F: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 < _T2)goto _TL2D;else{goto _TL2E;}
_TL2D: _T3=s;_T4=_T3.curr;_T5=(const char*)_T4;_T6=i;{char c=_T5[_T6];_T7=c;_T8=(int)_T7;
if(_T8!=39)goto _TL30;single_quotes=single_quotes + 1;goto _TL31;
_TL30: _T9=Cyc_is_other_special(c);if(!_T9)goto _TL32;other_special=other_special + 1;goto _TL33;_TL32: _TL33: _TL31:;}
# 185
i=i + 1;goto _TL2F;_TL2E:;}
# 192
if(single_quotes!=0)goto _TL34;if(other_special!=0)goto _TL34;_TA=s;
return _TA;_TL34:
# 196
 if(single_quotes!=0)goto _TL36;{struct _fat_ptr*_T3D[3];_TC=_T3D + 0;{struct _fat_ptr*_T3E=_cycalloc(sizeof(struct _fat_ptr));
*_T3E=_tag_fat("'",sizeof(char),2U);_TD=(struct _fat_ptr*)_T3E;}*_TC=_TD;_TE=_T3D + 1;{struct _fat_ptr*_T3E=_cycalloc(sizeof(struct _fat_ptr));_T10=s;*_T3E=_T10;_TF=(struct _fat_ptr*)_T3E;}*_TE=_TF;_T11=_T3D + 2;{struct _fat_ptr*_T3E=_cycalloc(sizeof(struct _fat_ptr));*_T3E=_tag_fat("'",sizeof(char),2U);_T12=(struct _fat_ptr*)_T3E;}*_T11=_T12;_T13=_tag_fat(_T3D,sizeof(struct _fat_ptr*),3);_TB=Cyc_List_list(_T13);}_T14=Cyc_strconcat_l(_TB);return _T14;_TL36: _T15=len;_T16=single_quotes;_T17=(unsigned long)_T16;_T18=_T15 + _T17;_T19=other_special;_T1A=(unsigned long)_T19;{
# 200
unsigned long len2=_T18 + _T1A;_T1C=len2 + 1U;{unsigned _T3D=_T1C + 1U;_T1E=_check_times(_T3D,sizeof(char));{char*_T3E=_cycalloc_atomic(_T1E);{unsigned _T3F=len2 + 1U;unsigned i;i=0;_TL3B: if(i < _T3F)goto _TL39;else{goto _TL3A;}_TL39: _T20=i;_T1F=_T3E + _T20;
*_T1F='\000';i=i + 1;goto _TL3B;_TL3A: _T21=_T3E + _T3F;*_T21=0;}_T1D=(char*)_T3E;}_T1B=_tag_fat(_T1D,sizeof(char),_T3D);}{struct _fat_ptr s2=_T1B;
int i=0;
int j=0;
_TL3F: _T22=i;_T23=(unsigned long)_T22;_T24=len;if(_T23 < _T24)goto _TL3D;else{goto _TL3E;}
_TL3D: _T25=s;_T26=_T25.curr;_T27=(const char*)_T26;_T28=i;{char c=_T27[_T28];_T29=c;_T2A=(int)_T29;
if(_T2A==39)goto _TL42;else{goto _TL43;}_TL43: _T2B=Cyc_is_other_special(c);if(_T2B)goto _TL42;else{goto _TL40;}
_TL42: _T2C=s2;_T2D=j;j=_T2D + 1;_T2E=_T2D;{struct _fat_ptr _T3D=_fat_ptr_plus(_T2C,sizeof(char),_T2E);_T2F=_check_fat_subscript(_T3D,sizeof(char),0U);_T30=(char*)_T2F;{char _T3E=*_T30;char _T3F='\\';_T31=_get_fat_size(_T3D,sizeof(char));if(_T31!=1U)goto _TL44;if(_T3E!=0)goto _TL44;if(_T3F==0)goto _TL44;_throw_arraybounds();goto _TL45;_TL44: _TL45: _T32=_T3D.curr;_T33=(char*)_T32;*_T33=_T3F;}}goto _TL41;_TL40: _TL41: _T34=s2;_T35=j;
j=_T35 + 1;_T36=_T35;{struct _fat_ptr _T3D=_fat_ptr_plus(_T34,sizeof(char),_T36);_T37=_check_fat_subscript(_T3D,sizeof(char),0U);_T38=(char*)_T37;{char _T3E=*_T38;char _T3F=c;_T39=_get_fat_size(_T3D,sizeof(char));if(_T39!=1U)goto _TL46;if(_T3E!=0)goto _TL46;if(_T3F==0)goto _TL46;_throw_arraybounds();goto _TL47;_TL46: _TL47: _T3A=_T3D.curr;_T3B=(char*)_T3A;*_T3B=_T3F;}}}
# 204
i=i + 1;goto _TL3F;_TL3E: _T3C=s2;
# 210
return _T3C;}}}
# 212
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){struct _fat_ptr*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;{struct _fat_ptr*_T3=_cycalloc(sizeof(struct _fat_ptr));_T1=sp;_T2=*_T1;
*_T3=Cyc_sh_escape_string(_T2);_T0=(struct _fat_ptr*)_T3;}return _T0;}
# 220
static struct Cyc_Lineno_Pos*Cyc_new_pos (void){struct Cyc_Lineno_Pos*_T0;{struct Cyc_Lineno_Pos*_T1=_cycalloc(sizeof(struct Cyc_Lineno_Pos));
_T1->logical_file=_tag_fat("",sizeof(char),1U);_T1->line=Cyc_Core_new_string(0U);_T1->line_no=0;_T1->col=0;_T0=(struct Cyc_Lineno_Pos*)_T1;}return _T0;}struct _tuple13{int f0;struct Cyc_Lineno_Pos*f1;};
# 223
static struct _tuple13*Cyc_start2pos(int x){struct _tuple13*_T0;{struct _tuple13*_T1=_cycalloc(sizeof(struct _tuple13));
_T1->f0=x;_T1->f1=Cyc_new_pos();_T0=(struct _tuple13*)_T1;}return _T0;}
# 228
static int Cyc_decl2start(struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;unsigned _T1;int _T2;_T0=d;_T1=_T0->loc;_T2=(int)_T1;
return _T2;}struct _tuple14{struct Cyc_Lineno_Pos*f0;void*f1;};
# 233
static struct Cyc_List_List*Cyc_this_file(struct _fat_ptr file,struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;struct _tuple14*_T2;struct _tuple14 _T3;struct Cyc_Lineno_Pos*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;int _T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;
# 235
struct Cyc_List_List*result=0;
_TL4B: if(x!=0)goto _TL49;else{goto _TL4A;}
_TL49: _T0=x;_T1=_T0->hd;_T2=(struct _tuple14*)_T1;_T3=*_T2;_T4=_T3.f0;_T5=_T4->logical_file;_T6=file;_T7=Cyc_strcmp(_T5,_T6);if(_T7!=0)goto _TL4C;{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));_T9=x;_TA=_T9->hd;
_TD->hd=(struct _tuple14*)_TA;_TD->tl=result;_T8=(struct Cyc_List_List*)_TD;}result=_T8;goto _TL4D;_TL4C: _TL4D: _TB=x;
# 236
x=_TB->tl;goto _TL4B;_TL4A:
# 240
 result=Cyc_List_imp_rev(result);_TC=result;
return _TC;}
# 245
static int Cyc_lineno(struct Cyc_Lineno_Pos*p){struct Cyc_Lineno_Pos*_T0;int _T1;_T0=p;_T1=_T0->line_no;
return _T1;}
# 249
static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*decls){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;
struct Cyc_List_List*result=0;
_TL4E: if(decls!=0)goto _TL4F;else{goto _TL50;}
_TL4F: _T0=decls;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{void*_TD=_T2->r;struct Cyc_List_List*_TE;_T3=(int*)_TD;_T4=*_T3;switch(_T4){case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_TF=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_TD;_TE=_TF->f2;}{struct Cyc_List_List*tdl=_TE;_TE=tdl;goto _LL4;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_TF=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_TD;_TE=_TF->f2;}_LL4: {struct Cyc_List_List*tdl=_TE;_TE=tdl;goto _LL6;}case 11:{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_TF=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_TD;_TE=_TF->f1;}_LL6: {struct Cyc_List_List*tdl=_TE;_TE=tdl;goto _LL8;}case 12:{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_TF=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_TD;_TE=_TF->f1;}_LL8: {struct Cyc_List_List*tdl=_TE;_T5=tdl;_T6=decls;_T7=_T6->tl;
# 257
decls=Cyc_List_append(_T5,_T7);goto _LL0;}case 5: goto _LLC;case 0: _LLC: goto _LLE;case 6: _LLE: goto _LL10;case 7: _LL10: goto _LL12;case 8: _LL12: goto _LL14;case 1: _LL14: goto _LL16;case 2: _LL16: goto _LL18;case 3: _LL18: goto _LL1A;case 13: _LL1A: goto _LL1C;case 14: _LL1C: goto _LL1E;case 15: _LL1E: goto _LL20;case 16: _LL20: goto _LL22;default: _LL22:{struct Cyc_List_List*_TF=_cycalloc(sizeof(struct Cyc_List_List));_T9=decls;_TA=_T9->hd;
# 272
_TF->hd=(struct Cyc_Absyn_Decl*)_TA;_TF->tl=result;_T8=(struct Cyc_List_List*)_TF;}result=_T8;_TB=decls;
decls=_TB->tl;goto _LL0;}_LL0:;}goto _TL4E;_TL50: _TC=
# 277
Cyc_List_imp_rev(result);return _TC;}struct _tuple15{int f0;struct Cyc_Absyn_Decl*f1;};struct _tuple16{struct Cyc_Lineno_Pos*f0;struct Cyc_Absyn_Decl*f1;};
# 281
static void Cyc_process_file(struct _fat_ptr filename){struct _fat_ptr _T0;struct _fat_ptr _T1;struct _fat_ptr _T2;char*_T3;char*_T4;struct Cyc_List_List*_T5;struct _fat_ptr*_T6;struct _fat_ptr _T7;struct Cyc_List_List*(*_T8)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*);struct Cyc_List_List*(*_T9)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc_String_pa_PrintArg_struct _TE;struct Cyc_String_pa_PrintArg_struct _TF;struct _fat_ptr _T10;void*_T11;void*_T12;unsigned _T13;struct Cyc_String_pa_PrintArg_struct _T14;void**_T15;void**_T16;void**_T17;void**_T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;char*_T1B;char*_T1C;int _T1D;struct Cyc___cycFILE*_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;void*_T22;void*_T23;unsigned _T24;struct Cyc___cycFILE*_T25;struct Cyc_List_List*(*_T26)(struct _tuple13*(*)(int),struct Cyc_List_List*);struct Cyc_List_List*(*_T27)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*(*_T28)(int(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*);struct Cyc_List_List*(*_T29)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T2A;struct Cyc_List_List*_T2B;struct _fat_ptr _T2C;void*_T2D;void*_T2E;unsigned _T2F;struct Cyc_List_List*_T30;struct Cyc_List_List*(*_T31)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*);struct Cyc_List_List*(*_T32)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Lineno_Pos*(*_T33)(struct _tuple13*);void*(*_T34)(struct _tuple0*);struct Cyc_List_List*_T35;struct Cyc_List_List*_T36;struct Cyc_List_List*_T37;struct Cyc_List_List*(*_T38)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*);struct Cyc_List_List*(*_T39)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*(*_T3A)(struct Cyc_Lineno_Pos*(*)(struct _tuple16*),struct Cyc_List_List*);struct Cyc_List_List*(*_T3B)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Lineno_Pos*(*_T3C)(struct _tuple16*);void*(*_T3D)(struct _tuple0*);struct Cyc_List_List*_T3E;struct Cyc_List_List*_T3F;struct Cyc_List_List*_T40;struct Cyc_List_List*(*_T41)(struct Cyc_Absyn_Decl*(*)(struct _tuple16*),struct Cyc_List_List*);struct Cyc_List_List*(*_T42)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Decl*(*_T43)(struct _tuple16*);void*(*_T44)(struct _tuple0*);struct Cyc_List_List*_T45;struct Cyc_List_List*_T46;struct _fat_ptr _T47;char*_T48;char*_T49;const char*_T4A;struct Cyc___cycFILE*_T4B;struct _tuple12*_T4C;struct _tuple12*_T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;int _T50;struct Cyc_List_List*(*_T51)(struct _tuple13*(*)(int),struct Cyc_List_List*);struct Cyc_List_List*(*_T52)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T53;struct Cyc_List_List*(*_T54)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*);struct Cyc_List_List*(*_T55)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*(*_T56)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*);struct Cyc_List_List*(*_T57)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Lineno_Pos*(*_T58)(struct _tuple13*);void*(*_T59)(struct _tuple0*);struct Cyc_List_List*_T5A;struct Cyc_List_List*_T5B;struct Cyc_List_List*_T5C;struct Cyc_List_List*_T5D;struct Cyc_List_List*_T5E;void*_T5F;struct Cyc_List_List*_T60;void*_T61;void*_T62;int*_T63;int _T64;void*_T65;struct _fat_ptr _T66;struct _fat_ptr _T67;struct Cyc_List_List*_T68;void*_T69;struct Cyc_List_List*_T6A;struct Cyc_List_List*_T6B;struct Cyc_List_List*_T6C;struct Cyc_List_List*_T6D;struct Cyc_List_List*_T6E;void*_T6F;struct Cyc_List_List*_T70;struct Cyc_List_List*_T71;struct Cyc_List_List*_T72;
struct _fat_ptr basename=Cyc_Filename_chop_extension(filename);_T0=basename;_T1=
# 284
_tag_fat(".cyp",sizeof(char),5U);_T2=Cyc_strconcat(_T0,_T1);_T3=_untag_fat_ptr_check_bound(_T2,sizeof(char),1U);_T4=_check_null(_T3);{const char*preprocfile=(const char*)_T4;{struct Cyc_List_List*_T73=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T74=_cycalloc(sizeof(struct _fat_ptr));_T7=
# 286
_tag_fat("",sizeof(char),1U);*_T74=_T7;_T6=(struct _fat_ptr*)_T74;}_T73->hd=_T6;_T9=Cyc_List_map;{
struct Cyc_List_List*(*_T74)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))_T9;_T8=_T74;}_TA=Cyc_List_rev(Cyc_cycargs);_T73->tl=_T8(Cyc_sh_escape_stringptr,_TA);_T5=(struct Cyc_List_List*)_T73;}_TB=
_tag_fat(" ",sizeof(char),2U);{
# 285
struct _fat_ptr cycargs_string=
Cyc_str_sepstr(_T5,_TB);{struct Cyc_String_pa_PrintArg_struct _T73;_T73.tag=0;
# 290
_T73.f1=Cyc_cyclone_file;_TD=_T73;}{struct Cyc_String_pa_PrintArg_struct _T73=_TD;{struct Cyc_String_pa_PrintArg_struct _T74;_T74.tag=0;
_T74.f1=cycargs_string;_TE=_T74;}{struct Cyc_String_pa_PrintArg_struct _T74=_TE;{struct Cyc_String_pa_PrintArg_struct _T75;_T75.tag=0;{const char*_T76=preprocfile;_T11=(void*)_T76;_T12=(void*)_T76;_T13=_get_zero_arr_size_char(_T12,1U);_T10=_tag_fat(_T11,sizeof(char),_T13);}
_T75.f1=Cyc_sh_escape_string(_T10);_TF=_T75;}{struct Cyc_String_pa_PrintArg_struct _T75=_TF;{struct Cyc_String_pa_PrintArg_struct _T76;_T76.tag=0;
_T76.f1=Cyc_sh_escape_string(filename);_T14=_T76;}{struct Cyc_String_pa_PrintArg_struct _T76=_T14;void*_T77[4];_T15=_T77 + 0;*_T15=& _T73;_T16=_T77 + 1;*_T16=& _T74;_T17=_T77 + 2;*_T17=& _T75;_T18=_T77 + 3;*_T18=& _T76;_T19=
# 289
_tag_fat("%s %s -E -o %s -x cyc %s",sizeof(char),25U);_T1A=_tag_fat(_T77,sizeof(void*),4);_TC=Cyc_aprintf(_T19,_T1A);}}}}_T1B=_untag_fat_ptr_check_bound(_TC,sizeof(char),1U);_T1C=_check_null(_T1B);{const char*cmd=(const char*)_T1C;_T1D=
# 294
system(cmd);if(_T1D==0)goto _TL52;_T1E=Cyc_stderr;_T1F=
_tag_fat("\nError: preprocessing\n",sizeof(char),23U);_T20=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T1E,_T1F,_T20);
return;_TL52:{const char*_T73=preprocfile;_T22=(void*)_T73;_T23=(void*)_T73;_T24=_get_zero_arr_size_char(_T23,1U);_T21=_tag_fat(_T22,sizeof(char),_T24);}
# 299
Cyc_Warn_reset(_T21);_T25=
Cyc_fopen(preprocfile,"r");{struct Cyc___cycFILE*in_file=_check_null(_T25);
Cyc_Lex_lex_init(1);{
struct Cyc_List_List*decls=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(1);
Cyc_file_close(in_file);
decls=Cyc_flatten_decls(decls);_T27=Cyc_List_map;{
# 307
struct Cyc_List_List*(*_T73)(struct _tuple13*(*)(int),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple13*(*)(int),struct Cyc_List_List*))_T27;_T26=_T73;}_T29=Cyc_List_map;{struct Cyc_List_List*(*_T73)(int(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*))_T29;_T28=_T73;}_T2A=decls;_T2B=_T28(Cyc_decl2start,_T2A);{struct Cyc_List_List*poss=_T26(Cyc_start2pos,_T2B);{const char*_T73=preprocfile;_T2D=(void*)_T73;_T2E=(void*)_T73;_T2F=_get_zero_arr_size_char(_T2E,1U);_T2C=_tag_fat(_T2D,sizeof(char),_T2F);}_T30=poss;
Cyc_Lineno_poss_of_abss(_T2C,_T30);
remove(preprocfile);_T32=Cyc_List_map;{
struct Cyc_List_List*(*_T73)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*))_T32;_T31=_T73;}_T34=Cyc_Core_snd;{struct Cyc_Lineno_Pos*(*_T73)(struct _tuple13*)=(struct Cyc_Lineno_Pos*(*)(struct _tuple13*))_T34;_T33=_T73;}_T35=poss;_T36=_T31(_T33,_T35);_T37=decls;{struct Cyc_List_List*pos_decls=Cyc_List_zip(_T36,_T37);
# 312
pos_decls=Cyc_this_file(filename,pos_decls);_T39=Cyc_List_map;{
struct Cyc_List_List*(*_T73)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*))_T39;_T38=_T73;}_T3B=Cyc_List_map;{struct Cyc_List_List*(*_T73)(struct Cyc_Lineno_Pos*(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*)(struct _tuple16*),struct Cyc_List_List*))_T3B;_T3A=_T73;}_T3D=Cyc_Core_fst;{struct Cyc_Lineno_Pos*(*_T73)(struct _tuple16*)=(struct Cyc_Lineno_Pos*(*)(struct _tuple16*))_T3D;_T3C=_T73;}_T3E=pos_decls;_T3F=_T3A(_T3C,_T3E);_T40=_T38(Cyc_lineno,_T3F);_T42=Cyc_List_map;{
struct Cyc_List_List*(*_T73)(struct Cyc_Absyn_Decl*(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(struct _tuple16*),struct Cyc_List_List*))_T42;_T41=_T73;}_T44=Cyc_Core_snd;{struct Cyc_Absyn_Decl*(*_T73)(struct _tuple16*)=(struct Cyc_Absyn_Decl*(*)(struct _tuple16*))_T44;_T43=_T73;}_T45=pos_decls;_T46=_T41(_T43,_T45);{
# 313
struct Cyc_List_List*lineno_decls=Cyc_List_zip(_T40,_T46);_T47=filename;_T48=_untag_fat_ptr_check_bound(_T47,sizeof(char),1U);_T49=_check_null(_T48);_T4A=(const char*)_T49;_T4B=
# 317
Cyc_fopen(_T4A,"r");{struct Cyc___cycFILE*f=_check_null(_T4B);
struct Cyc_Lexing_lexbuf*lb=Cyc_Lexing_from_file(f);
struct Cyc_List_List*comments=0;
struct Cyc_List_List*indices=0;
struct _tuple12*tok;
_TL54: tok=Cyc_token(lb);_T4C=tok;if(_T4C!=0)goto _TL55;else{goto _TL56;}
_TL55: _T4D=tok;{struct _tuple12 _T73=*_T4D;void*_T74;int _T75;_T75=_T73.f0;_T74=_T73.f1;{int index=_T75;void*comment=_T74;{struct Cyc_List_List*_T76=_cycalloc(sizeof(struct Cyc_List_List));
_T76->hd=comment;_T76->tl=comments;_T4E=(struct Cyc_List_List*)_T76;}comments=_T4E;{struct Cyc_List_List*_T76=_cycalloc(sizeof(struct Cyc_List_List));_T50=index;
_T76->hd=(void*)_T50;_T76->tl=indices;_T4F=(struct Cyc_List_List*)_T76;}indices=_T4F;}}goto _TL54;_TL56:
# 327
 Cyc_fclose(f);
comments=Cyc_List_imp_rev(comments);
indices=Cyc_List_imp_rev(indices);_T52=Cyc_List_map;{
# 331
struct Cyc_List_List*(*_T73)(struct _tuple13*(*)(int),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple13*(*)(int),struct Cyc_List_List*))_T52;_T51=_T73;}_T53=indices;{struct Cyc_List_List*poss=_T51(Cyc_start2pos,_T53);
Cyc_Lineno_poss_of_abss(filename,poss);_T55=Cyc_List_map;{
struct Cyc_List_List*(*_T73)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*))_T55;_T54=_T73;}_T57=Cyc_List_map;{struct Cyc_List_List*(*_T73)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*))_T57;_T56=_T73;}_T59=Cyc_Core_snd;{struct Cyc_Lineno_Pos*(*_T73)(struct _tuple13*)=(struct Cyc_Lineno_Pos*(*)(struct _tuple13*))_T59;_T58=_T73;}_T5A=poss;_T5B=_T56(_T58,_T5A);_T5C=_T54(Cyc_lineno,_T5B);_T5D=comments;{struct Cyc_List_List*lineno_comments=Cyc_List_zip(_T5C,_T5D);
# 338
_TL57: if(lineno_decls!=0)goto _TL5A;else{goto _TL59;}_TL5A: if(lineno_comments!=0)goto _TL58;else{goto _TL59;}
_TL58: _T5E=lineno_decls;_T5F=_T5E->hd;{struct _tuple15*_T73=(struct _tuple15*)_T5F;struct Cyc_Absyn_Decl*_T74;int _T75;{struct _tuple15 _T76=*_T73;_T75=_T76.f0;_T74=_T76.f1;}{int dline=_T75;struct Cyc_Absyn_Decl*decl=_T74;_T60=lineno_comments;_T61=_T60->hd;{
struct _tuple12*_T76=(struct _tuple12*)_T61;void*_T77;int _T78;{struct _tuple12 _T79=*_T76;_T78=_T79.f0;_T77=_T79.f1;}{int cline=_T78;void*comment=_T77;struct _fat_ptr _T79;_T62=comment;_T63=(int*)_T62;_T64=*_T63;if(_T64!=1)goto _TL5B;_T65=comment;{struct Cyc_Standalone_Comment_struct*_T7A=(struct Cyc_Standalone_Comment_struct*)_T65;_T79=_T7A->f1;}{struct _fat_ptr cmt=_T79;
# 343
Cyc_pr_comment(Cyc_stdout,cmt);_T66=
_tag_fat("\n",sizeof(char),2U);_T67=_tag_fat(0U,sizeof(void*),0);Cyc_printf(_T66,_T67);_T68=lineno_comments;
lineno_comments=_T68->tl;goto _LL9;}_TL5B: _T69=comment;{struct Cyc_MatchDecl_Comment_struct*_T7A=(struct Cyc_MatchDecl_Comment_struct*)_T69;_T79=_T7A->f1;}{struct _fat_ptr cmt=_T79;
# 348
if(cline >= dline)goto _TL5D;_T6A=lineno_comments;
# 350
lineno_comments=_T6A->tl;goto _TL57;_TL5D: _T6B=lineno_decls;_T6C=_T6B->tl;
# 353
if(_T6C==0)goto _TL5F;_T6D=lineno_decls;_T6E=_T6D->tl;_T6F=_T6E->hd;{
struct _tuple15*_T7A=(struct _tuple15*)_T6F;int _T7B;{struct _tuple15 _T7C=*_T7A;_T7B=_T7C.f0;}{int dline2=_T7B;
if(dline2 >= cline)goto _TL61;_T70=lineno_decls;
# 357
lineno_decls=_T70->tl;goto _TL57;_TL61:;}}goto _TL60;_TL5F: _TL60:
# 363
 Cyc_dumpdecl(decl,cmt);
Cyc_fflush(Cyc_stdout);_T71=lineno_decls;
lineno_decls=_T71->tl;_T72=lineno_comments;
lineno_comments=_T72->tl;goto _LL9;}_LL9:;}}}}goto _TL57;_TL59:;}}}}}}}}}}}}
# 373
extern void GC_blacklist_warn_clear (void);struct _tuple17{struct _fat_ptr f0;int f1;struct _fat_ptr f2;void*f3;struct _fat_ptr f4;};
# 375
int Cyc_main(int argc,struct _fat_ptr argv){struct Cyc_List_List*_T0;struct _tuple17**_T1;struct _tuple17*_T2;struct Cyc_Arg_String_spec_Arg_Spec_struct*_T3;struct _tuple17**_T4;struct _tuple17*_T5;struct Cyc_Arg_Int_spec_Arg_Spec_struct*_T6;struct _tuple17**_T7;struct _tuple17*_T8;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_T9;struct _tuple17**_TA;struct _tuple17*_TB;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_TC;struct _tuple17**_TD;struct _tuple17*_TE;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_TF;struct _fat_ptr _T10;struct Cyc_List_List*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_List_List*_T14;struct _fat_ptr _T15;struct Cyc_Absynpp_Params*_T16;struct Cyc_Absynpp_Params*_T17;struct Cyc_List_List*_T18;void*_T19;struct _fat_ptr*_T1A;struct _fat_ptr _T1B;struct Cyc_List_List*_T1C;
# 380
GC_blacklist_warn_clear();{struct _tuple17*_T1D[5];_T1=_T1D + 0;{struct _tuple17*_T1E=_cycalloc(sizeof(struct _tuple17));
# 383
_T1E->f0=_tag_fat("-cyclone",sizeof(char),9U);_T1E->f1=0;_T1E->f2=_tag_fat(" <file>",sizeof(char),8U);{struct Cyc_Arg_String_spec_Arg_Spec_struct*_T1F=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_T1F->tag=5;
_T1F->f1=Cyc_set_cyclone_file;_T3=(struct Cyc_Arg_String_spec_Arg_Spec_struct*)_T1F;}_T1E->f3=(void*)_T3;
_T1E->f4=_tag_fat("Use <file> as the cyclone compiler",sizeof(char),35U);_T2=(struct _tuple17*)_T1E;}
# 383
*_T1=_T2;_T4=_T1D + 1;{struct _tuple17*_T1E=_cycalloc(sizeof(struct _tuple17));
# 386
_T1E->f0=_tag_fat("-w",sizeof(char),3U);_T1E->f1=0;_T1E->f2=_tag_fat(" <width>",sizeof(char),9U);{struct Cyc_Arg_Int_spec_Arg_Spec_struct*_T1F=_cycalloc(sizeof(struct Cyc_Arg_Int_spec_Arg_Spec_struct));_T1F->tag=6;
_T1F->f1=Cyc_set_width;_T6=(struct Cyc_Arg_Int_spec_Arg_Spec_struct*)_T1F;}_T1E->f3=(void*)_T6;
_T1E->f4=_tag_fat("Use <width> as the max width for printing declarations",sizeof(char),55U);_T5=(struct _tuple17*)_T1E;}
# 386
*_T4=_T5;_T7=_T1D + 2;{struct _tuple17*_T1E=_cycalloc(sizeof(struct _tuple17));
# 389
_T1E->f0=_tag_fat("-D",sizeof(char),3U);_T1E->f1=1;_T1E->f2=_tag_fat("<name>[=<value>]",sizeof(char),17U);{struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_T1F=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_T1F->tag=1;
_T1F->f1=Cyc_add_cycarg;_T9=(struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_T1F;}_T1E->f3=(void*)_T9;
_T1E->f4=_tag_fat("Pass definition to preprocessor",sizeof(char),32U);_T8=(struct _tuple17*)_T1E;}
# 389
*_T7=_T8;_TA=_T1D + 3;{struct _tuple17*_T1E=_cycalloc(sizeof(struct _tuple17));
# 392
_T1E->f0=_tag_fat("-I",sizeof(char),3U);_T1E->f1=1;_T1E->f2=_tag_fat("<dir>",sizeof(char),6U);{struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_T1F=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_T1F->tag=1;
_T1F->f1=Cyc_add_cycarg;_TC=(struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_T1F;}_T1E->f3=(void*)_TC;
_T1E->f4=_tag_fat("Add to the list of directories to search for include files",sizeof(char),59U);_TB=(struct _tuple17*)_T1E;}
# 392
*_TA=_TB;_TD=_T1D + 4;{struct _tuple17*_T1E=_cycalloc(sizeof(struct _tuple17));
# 395
_T1E->f0=_tag_fat("-B",sizeof(char),3U);_T1E->f1=1;_T1E->f2=_tag_fat("<file>",sizeof(char),7U);{struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_T1F=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_T1F->tag=1;
_T1F->f1=Cyc_add_cycarg;_TF=(struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_T1F;}_T1E->f3=(void*)_TF;
_T1E->f4=_tag_fat("Add to the list of directories to search for compiler files",sizeof(char),60U);_TE=(struct _tuple17*)_T1E;}
# 395
*_TD=_TE;_T10=_tag_fat(_T1D,sizeof(struct _tuple17*),5);_T0=Cyc_List_list(_T10);}{
# 382
struct Cyc_List_List*options=_T0;_T11=options;_T12=
# 400
_tag_fat("Options:",sizeof(char),9U);_T13=argv;Cyc_Arg_parse(_T11,Cyc_add_other,Cyc_no_other,_T12,_T13);
# 402
if(Cyc_cycdoc_files!=0)goto _TL63;_T14=options;_T15=
# 405
_tag_fat("Usage: cycdoc [options] file1 file2 ...\nOptions:",sizeof(char),49U);
# 403
Cyc_Arg_usage(_T14,_T15);
# 406
exit(1);goto _TL64;_TL63: _TL64:
# 411
 Cyc_PP_tex_output=1;_T16=& Cyc_Absynpp_tc_params_r;_T17=(struct Cyc_Absynpp_Params*)_T16;
# 413
Cyc_Absynpp_set_params(_T17);
Cyc_Absynpp_print_for_cycdoc=1;{
# 416
struct Cyc_List_List*l=Cyc_List_rev(Cyc_cycdoc_files);_TL68: if(l!=0)goto _TL66;else{goto _TL67;}
_TL66: _T18=l;_T19=_T18->hd;_T1A=(struct _fat_ptr*)_T19;_T1B=*_T1A;Cyc_process_file(_T1B);_T1C=l;
# 416
l=_T1C->tl;goto _TL68;_TL67:;}
# 419
return 0;}}

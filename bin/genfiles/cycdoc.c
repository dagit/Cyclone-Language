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
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

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
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 27 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 24 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Doc;
# 53
extern struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*,int);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 51 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 53
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*);
# 29 "cycdoc.cyl"
extern void Cyc_Lex_lex_init(int);struct Cyc_MatchDecl_Comment_struct{int tag;struct _fat_ptr f1;};struct Cyc_Standalone_Comment_struct{int tag;struct _fat_ptr f1;};struct _tuple12{int f0;void*f1;};
# 44
struct _tuple12*Cyc_token(struct Cyc_Lexing_lexbuf*);
# 47
const int Cyc_lex_base[15U]={0,- 4,0,- 3,1,2,3,0,4,6,7,- 1,8,9,- 2};
const int Cyc_lex_backtrk[15U]={- 1,- 1,3,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[15U]={1,0,- 1,0,- 1,- 1,6,- 1,8,8,8,0,6,6,0};
const int Cyc_lex_trans[266U]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,6,0,0,0,0,0,0,0,4,5,7,12,9,2,10,10,13,13,0,11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};
const int Cyc_lex_check[266U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,4,5,6,8,0,9,10,12,13,- 1,9,10,12,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,6,8,- 1,9,10,12,13};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 54
int state;int base;int backtrk;
int c;
state=start_state;
# 58
if(state >= 0){
({int _Tmp0=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_Tmp0;});
lbuf->lex_last_action=-1;}else{
# 62
state=(- state)- 1;}
# 64
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_lex_base,15U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=*((const int*)_check_known_subscript_notnull(Cyc_lex_backtrk,15U,sizeof(int),state));
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 72
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 76
c=256;}}else{
# 78
c=(int)*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c==-1)c=256;else{
if(c < 0)c=256 + c;}}
# 82
if(*((const int*)_check_known_subscript_notnull(Cyc_lex_check,266U,sizeof(int),base + c))==state)
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_trans,266U,sizeof(int),base + c));else{
# 85
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_default,15U,sizeof(int),state));}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action==-1)
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("empty token",sizeof(char),12U);_Tmp0;}));else{
# 91
return lbuf->lex_last_action;}}else{
# 94
if(c==256)lbuf->lex_eof_reached=0;}
# 65
1U;}}
# 98
struct _tuple12*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:  {
# 51 "cycdoc.cyl"
int start=lexbuf->lex_start_pos + 5;
int len=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 7;
struct _tuple12*_Tmp0=_cycalloc(sizeof(struct _tuple12));({int _Tmp1=Cyc_Lexing_lexeme_start(lexbuf);_Tmp0->f0=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_Standalone_Comment_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Standalone_Comment_struct));_Tmp2->tag=1,({struct _fat_ptr _Tmp3=Cyc_substring((struct _fat_ptr)lexbuf->lex_buffer,start,(unsigned long)len);_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->f1=_Tmp1;});return _Tmp0;}case 1:  {
# 56
int start=lexbuf->lex_start_pos + 4;
int len=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 6;
struct _tuple12*_Tmp0=_cycalloc(sizeof(struct _tuple12));({int _Tmp1=Cyc_Lexing_lexeme_start(lexbuf);_Tmp0->f0=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_MatchDecl_Comment_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_MatchDecl_Comment_struct));_Tmp2->tag=0,({struct _fat_ptr _Tmp3=Cyc_substring((struct _fat_ptr)lexbuf->lex_buffer,start,(unsigned long)len);_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->f1=_Tmp1;});return _Tmp0;}case 2:
# 60 "cycdoc.cyl"
 return 0;case 3:
# 61 "cycdoc.cyl"
 return Cyc_token(lexbuf);default:
 lexbuf->refill_buff(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);};
# 65
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 67
struct _tuple12*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,0);}
# 65 "cycdoc.cyl"
static void Cyc_dump_begin (void){
# 67
puts("%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}");}
# 81
static void Cyc_dump_middle (void){
puts("\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}");}
# 88
static void Cyc_dump_end (void){
# 90
puts("%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}");}
# 99
static void Cyc_pr_comment(struct Cyc___cycFILE*outf,struct _fat_ptr s){
int depth=0;
int len=(int)Cyc_strlen(s);
int i=0;for(0;i < len;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c!=91){Cyc_fputc((int)c,outf);continue;}
Cyc_fputs("\\texttt{",outf);
++ i;
++ depth;
for(1;i < len;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c==93){
-- depth;
if(depth==0){
Cyc_fputc(125,outf);
break;}}else{
# 117
if((int)c==91)++ depth;}
Cyc_fputc((int)c,outf);}}}
# 124
static int Cyc_width=50;
static void Cyc_set_width(int w){
Cyc_width=w;}
# 128
static struct Cyc_List_List*Cyc_cycdoc_files=0;
static void Cyc_add_other(struct _fat_ptr s){
Cyc_cycdoc_files=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cycdoc_files;_Tmp0;});}
# 132
static int Cyc_no_other(struct _fat_ptr s){
return 0;}
# 135
static struct Cyc_List_List*Cyc_cycargs=0;
static void Cyc_add_cycarg(struct _fat_ptr s){
Cyc_cycargs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cycargs;_Tmp0;});}static char _TmpG0[8U]="cyclone";
# 139
static struct _fat_ptr Cyc_cyclone_file={_TmpG0,_TmpG0,_TmpG0 + 8U};
static void Cyc_set_cyclone_file(struct _fat_ptr s){
Cyc_cyclone_file=s;}
# 145
static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _fat_ptr comment){
Cyc_dump_begin();
# 151
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_PP_string_of_doc(Cyc_Absynpp_decl2doc(d),50);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_dump_middle();
Cyc_pr_comment(Cyc_stdout,comment);
Cyc_printf(_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
Cyc_dump_end();}
# 159
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
 goto _LL16;case 32: _LL16:
 goto _LL18;case 10: _LL18:
 goto _LL1A;case 9: _LL1A:
 return 1;default:
 return 0;};}
# 178
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long len=Cyc_strlen(s);
# 182
int single_quotes=0;
int other_special=0;
{int i=0;for(0;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c==39)++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 191
if(single_quotes==0 && other_special==0)
return s;
# 195
if(single_quotes==0)
return Cyc_strconcat_l(({struct _fat_ptr*_Tmp0[3];({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("'",sizeof(char),2U);_Tmp2;});_Tmp0[0]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=(struct _fat_ptr)s;_Tmp2;});_Tmp0[1]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("'",sizeof(char),2U);_Tmp2;});_Tmp0[2]=_Tmp1;});Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _fat_ptr*),3));}));{
# 199
unsigned long len2=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;
struct _fat_ptr s2=({unsigned _Tmp0=(len2 + 1U)+ 1U;_tag_fat(({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=len2 + 1U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='\000';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;}),sizeof(char),_Tmp0);});
int i=0;
int j=0;
for(1;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c==39 || Cyc_is_other_special(c))
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\\';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 209
return s2;}}
# 211
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=Cyc_sh_escape_string(*sp);*_Tmp0=_Tmp1;});return _Tmp0;}
# 219
static struct Cyc_Lineno_Pos*Cyc_new_pos (void){
struct Cyc_Lineno_Pos*_Tmp0=_cycalloc(sizeof(struct Cyc_Lineno_Pos));_Tmp0->logical_file=_tag_fat("",sizeof(char),1U),({struct _fat_ptr _Tmp1=Cyc_Core_new_string(0U);_Tmp0->line=_Tmp1;}),_Tmp0->line_no=0,_Tmp0->col=0;return _Tmp0;}struct _tuple13{int f0;struct Cyc_Lineno_Pos*f1;};
# 222
static struct _tuple13*Cyc_start2pos(int x){
struct _tuple13*_Tmp0=_cycalloc(sizeof(struct _tuple13));_Tmp0->f0=x,({struct Cyc_Lineno_Pos*_Tmp1=Cyc_new_pos();_Tmp0->f1=_Tmp1;});return _Tmp0;}
# 227
static int Cyc_decl2start(struct Cyc_Absyn_Decl*d){
return(int)d->loc;}struct _tuple14{struct Cyc_Lineno_Pos*f0;void*f1;};
# 232
static struct Cyc_List_List*Cyc_this_file(struct _fat_ptr file,struct Cyc_List_List*x){
# 234
struct Cyc_List_List*result=0;
for(1;x!=0;x=x->tl){
if(Cyc_strcmp((*((struct _tuple14*)x->hd)).f0->logical_file,file)==0)
result=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(struct _tuple14*)x->hd,_Tmp0->tl=result;_Tmp0;});}
# 239
result=Cyc_List_imp_rev(result);
return result;}
# 244
static int Cyc_lineno(struct Cyc_Lineno_Pos*p){
return p->line_no;}
# 248
static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*decls){
struct Cyc_List_List*result=0;
while(decls!=0){
{void*_Tmp0=((struct Cyc_Absyn_Decl*)decls->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 9: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*tdl=_Tmp1;
_Tmp1=tdl;goto _LL4;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL4: {struct Cyc_List_List*tdl=_Tmp1;
_Tmp1=tdl;goto _LL6;}case 11: _Tmp1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_LL6: {struct Cyc_List_List*tdl=_Tmp1;
_Tmp1=tdl;goto _LL8;}case 12: _Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_LL8: {struct Cyc_List_List*tdl=_Tmp1;
# 256
decls=Cyc_List_append(tdl,decls->tl);
goto _LL0;}case 5:
 goto _LLC;case 0: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 8: _LL12:
 goto _LL14;case 1: _LL14:
 goto _LL16;case 2: _LL16:
 goto _LL18;case 3: _LL18:
 goto _LL1A;case 13: _LL1A:
 goto _LL1C;case 14: _LL1C:
 goto _LL1E;case 15: _LL1E:
 goto _LL20;case 16: _LL20:
 goto _LL22;default: _LL22:
# 271
 result=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(struct Cyc_Absyn_Decl*)decls->hd,_Tmp2->tl=result;_Tmp2;});
decls=decls->tl;
goto _LL0;}_LL0:;}
# 251
1U;}
# 276
return Cyc_List_imp_rev(result);}struct _tuple15{int f0;struct Cyc_Absyn_Decl*f1;};struct _tuple16{struct Cyc_Lineno_Pos*f0;struct Cyc_Absyn_Decl*f1;};
# 280
static void Cyc_process_file(struct _fat_ptr filename){
struct _fat_ptr basename=Cyc_Filename_chop_extension(filename);
# 283
const char*preprocfile=(const char*)_untag_fat_ptr_check_bound(Cyc_strconcat(basename,_tag_fat(".cyp",sizeof(char),5U)),sizeof(char),1U);
struct _fat_ptr cycargs_string=({
struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=(struct _fat_ptr)_tag_fat("",sizeof(char),1U);_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*(*_Tmp3)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp4)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;_Tmp4;});_Tmp3(Cyc_sh_escape_stringptr,Cyc_List_rev(Cyc_cycargs));});_Tmp1->tl=_Tmp2;});_Tmp1;});
# 285
Cyc_str_sepstr(_Tmp0,
# 287
_tag_fat(" ",sizeof(char),2U));});
const char*cmd=(const char*)_untag_fat_ptr_check_bound(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)Cyc_cyclone_file;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)cycargs_string;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=(struct _fat_ptr)Cyc_sh_escape_string(({const char*_Tmp5=preprocfile;_tag_fat((void*)_Tmp5,sizeof(char),_get_zero_arr_size_char((void*)_Tmp5,1U));}));_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_sh_escape_string(filename);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_aprintf(_tag_fat("%s %s -E -o %s -x cyc %s",sizeof(char),25U),_tag_fat(_Tmp4,sizeof(void*),4));}),sizeof(char),1U);
# 293
if(system(cmd)!=0){
Cyc_fprintf(Cyc_stderr,_tag_fat("\nError: preprocessing\n",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));
return;}
# 298
Cyc_Position_reset_position(({const char*_Tmp0=preprocfile;_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}));{
struct Cyc___cycFILE*in_file=Cyc_fopen(preprocfile,"r");
Cyc_Lex_lex_init(1);{
struct Cyc_List_List*decls=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(1);
Cyc_file_close(in_file);
decls=Cyc_flatten_decls(decls);{
# 306
struct Cyc_List_List*poss=({struct Cyc_List_List*(*_Tmp0)(struct _tuple13*(*)(int),struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp1)(struct _tuple13*(*)(int),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple13*(*)(int),struct Cyc_List_List*))Cyc_List_map;_Tmp1;});_Tmp0(Cyc_start2pos,({struct Cyc_List_List*(*_Tmp1)(int(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*))Cyc_List_map;_Tmp1;})(Cyc_decl2start,decls));});
Cyc_Lineno_poss_of_abss(({const char*_Tmp0=preprocfile;_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}),poss);
remove(preprocfile);{
struct Cyc_List_List*pos_decls=({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*))Cyc_List_map;_Tmp1;})(({struct Cyc_Lineno_Pos*(*_Tmp1)(struct _tuple13*)=(struct Cyc_Lineno_Pos*(*)(struct _tuple13*))Cyc_Core_snd;_Tmp1;}),poss);Cyc_List_zip(_Tmp0,decls);});
# 311
pos_decls=Cyc_this_file(filename,pos_decls);{
struct Cyc_List_List*lineno_decls=({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*(*_Tmp1)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*))Cyc_List_map;_Tmp2;});_Tmp1(Cyc_lineno,({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Lineno_Pos*(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;_Tmp2;})(({struct Cyc_Lineno_Pos*(*_Tmp2)(struct _tuple16*)=(struct Cyc_Lineno_Pos*(*)(struct _tuple16*))Cyc_Core_fst;_Tmp2;}),pos_decls));});Cyc_List_zip(_Tmp0,
({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Decl*(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;_Tmp1;})(({struct Cyc_Absyn_Decl*(*_Tmp1)(struct _tuple16*)=(struct Cyc_Absyn_Decl*(*)(struct _tuple16*))Cyc_Core_snd;_Tmp1;}),pos_decls));});
# 316
struct Cyc___cycFILE*f=Cyc_fopen((const char*)_untag_fat_ptr_check_bound(filename,sizeof(char),1U),"r");
struct Cyc_Lexing_lexbuf*lb=Cyc_Lexing_from_file(f);
struct Cyc_List_List*comments=0;
struct Cyc_List_List*indices=0;
struct _tuple12*tok;
while((tok=Cyc_token(lb))!=0){
{struct _tuple12 _Tmp0=*tok;void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int index=_Tmp2;void*comment=_Tmp1;
comments=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=comment,_Tmp3->tl=comments;_Tmp3;});
indices=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=(void*)index,_Tmp3->tl=indices;_Tmp3;});}}
# 322
1U;}
# 326
Cyc_fclose(f);
comments=Cyc_List_imp_rev(comments);
indices=Cyc_List_imp_rev(indices);{
# 330
struct Cyc_List_List*poss=({struct Cyc_List_List*(*_Tmp0)(struct _tuple13*(*)(int),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple13*(*)(int),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(Cyc_start2pos,indices);
Cyc_Lineno_poss_of_abss(filename,poss);{
struct Cyc_List_List*lineno_comments=({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*(*_Tmp1)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*))Cyc_List_map;_Tmp2;});_Tmp1(Cyc_lineno,({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*))Cyc_List_map;_Tmp2;})(({struct Cyc_Lineno_Pos*(*_Tmp2)(struct _tuple13*)=(struct Cyc_Lineno_Pos*(*)(struct _tuple13*))Cyc_Core_snd;_Tmp2;}),poss));});Cyc_List_zip(_Tmp0,comments);});
# 337
while(lineno_decls!=0 && lineno_comments!=0){
{struct _tuple15*_Tmp0=(struct _tuple15*)lineno_decls->hd;void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{int dline=_Tmp2;struct Cyc_Absyn_Decl*decl=_Tmp1;
struct _tuple12*_Tmp3=(struct _tuple12*)lineno_comments->hd;void*_Tmp4;int _Tmp5;_Tmp5=_Tmp3->f0;_Tmp4=_Tmp3->f1;{int cline=_Tmp5;void*comment=_Tmp4;
struct _fat_ptr _Tmp6;if(*((int*)comment)==1){_Tmp6=((struct Cyc_Standalone_Comment_struct*)comment)->f1;{struct _fat_ptr cmt=_Tmp6;
# 342
Cyc_pr_comment(Cyc_stdout,cmt);
Cyc_printf(_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
lineno_comments=lineno_comments->tl;
goto _LL9;}}else{_Tmp6=((struct Cyc_MatchDecl_Comment_struct*)comment)->f1;{struct _fat_ptr cmt=_Tmp6;
# 347
if(cline < dline){
# 349
lineno_comments=lineno_comments->tl;
continue;}
# 352
if(lineno_decls->tl!=0){
struct _tuple15*_Tmp7=(struct _tuple15*)lineno_decls->tl->hd;int _Tmp8;_Tmp8=_Tmp7->f0;{int dline2=_Tmp8;
if(dline2 < cline){
# 356
lineno_decls=lineno_decls->tl;
continue;}}}
# 362
Cyc_dumpdecl(decl,cmt);
Cyc_fflush(Cyc_stdout);
lineno_decls=lineno_decls->tl;
lineno_comments=lineno_comments->tl;
goto _LL9;}}_LL9:;}}}
# 338
1U;}}}}}}}}}
# 372
extern void GC_blacklist_warn_clear (void);struct _tuple17{struct _fat_ptr f0;int f1;struct _fat_ptr f2;void*f3;struct _fat_ptr f4;};
# 374
int Cyc_main(int argc,struct _fat_ptr argv){
# 379
GC_blacklist_warn_clear();{
# 381
struct Cyc_List_List*options=({struct _tuple17*_Tmp0[5];({
struct _tuple17*_Tmp1=({struct _tuple17*_Tmp2=_cycalloc(sizeof(struct _tuple17));_Tmp2->f0=_tag_fat("-cyclone",sizeof(char),9U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat(" <file>",sizeof(char),8U),({
void*_Tmp3=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_Tmp4->tag=5,_Tmp4->f1=Cyc_set_cyclone_file;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Use <file> as the cyclone compiler",sizeof(char),35U);_Tmp2;});
# 382
_Tmp0[0]=_Tmp1;}),({
# 385
struct _tuple17*_Tmp1=({struct _tuple17*_Tmp2=_cycalloc(sizeof(struct _tuple17));_Tmp2->f0=_tag_fat("-w",sizeof(char),3U),_Tmp2->f1=0,_Tmp2->f2=_tag_fat(" <width>",sizeof(char),9U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Int_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Int_spec_Arg_Spec_struct));_Tmp4->tag=6,_Tmp4->f1=Cyc_set_width;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Use <width> as the max width for printing declarations",sizeof(char),55U);_Tmp2;});
# 385
_Tmp0[1]=_Tmp1;}),({
# 388
struct _tuple17*_Tmp1=({struct _tuple17*_Tmp2=_cycalloc(sizeof(struct _tuple17));_Tmp2->f0=_tag_fat("-D",sizeof(char),3U),_Tmp2->f1=1,_Tmp2->f2=_tag_fat("<name>[=<value>]",sizeof(char),17U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_Tmp4->tag=1,_Tmp4->f1=Cyc_add_cycarg;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Pass definition to preprocessor",sizeof(char),32U);_Tmp2;});
# 388
_Tmp0[2]=_Tmp1;}),({
# 391
struct _tuple17*_Tmp1=({struct _tuple17*_Tmp2=_cycalloc(sizeof(struct _tuple17));_Tmp2->f0=_tag_fat("-I",sizeof(char),3U),_Tmp2->f1=1,_Tmp2->f2=_tag_fat("<dir>",sizeof(char),6U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_Tmp4->tag=1,_Tmp4->f1=Cyc_add_cycarg;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Add to the list of directories to search for include files",sizeof(char),59U);_Tmp2;});
# 391
_Tmp0[3]=_Tmp1;}),({
# 394
struct _tuple17*_Tmp1=({struct _tuple17*_Tmp2=_cycalloc(sizeof(struct _tuple17));_Tmp2->f0=_tag_fat("-B",sizeof(char),3U),_Tmp2->f1=1,_Tmp2->f2=_tag_fat("<file>",sizeof(char),7U),({
void*_Tmp3=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_Tmp4->tag=1,_Tmp4->f1=Cyc_add_cycarg;_Tmp4;});_Tmp2->f3=_Tmp3;}),_Tmp2->f4=
_tag_fat("Add to the list of directories to search for compiler files",sizeof(char),60U);_Tmp2;});
# 394
_Tmp0[4]=_Tmp1;});Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _tuple17*),5));});
# 399
Cyc_Arg_parse(options,Cyc_add_other,Cyc_no_other,_tag_fat("Options:",sizeof(char),9U),argv);
# 401
if(Cyc_cycdoc_files==0){
Cyc_Arg_usage(options,
# 404
_tag_fat("Usage: cycdoc [options] file1 file2 ...\nOptions:",sizeof(char),49U));
exit(1);}
# 410
Cyc_PP_tex_output=1;
# 412
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Absynpp_print_for_cycdoc=1;
# 415
{struct Cyc_List_List*l=Cyc_List_rev(Cyc_cycdoc_files);for(0;l!=0;l=l->tl){
Cyc_process_file(*((struct _fat_ptr*)l->hd));}}
# 418
return 0;}}

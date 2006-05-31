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
 extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};
# 321 "core.h"
void Cyc_Core_rethrow(void*);
# 325
const char*Cyc_Core_get_exn_name(void*);
# 327
const char*Cyc_Core_get_exn_filename (void);
# 334
int Cyc_Core_get_exn_lineno (void);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};
# 66 "arg.h"
extern void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 69
extern int Cyc_Arg_current;
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 84
extern int Cyc_feof(struct Cyc___cycFILE*);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 108
extern unsigned long Cyc_fread(struct _fat_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 140 "cycboot.h"
extern unsigned long Cyc_fwrite(struct _fat_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 167
extern int remove(const char*);
# 271 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_file_open(struct _fat_ptr,struct _fat_ptr);
extern void Cyc_file_close(struct Cyc___cycFILE*);
# 313
extern char*getenv(const char*);
# 318
extern int system(const char*);
extern void exit(int);struct Cyc_timeval{long tv_sec;long tv_usec;};
# 328
extern int gettimeofday(struct Cyc_timeval*,void*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 51
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
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);
# 121
extern struct _fat_ptr Cyc_strchr(struct _fat_ptr,char);
# 30 "filename.h"
extern struct _fat_ptr Cyc_Filename_concat(struct _fat_ptr,struct _fat_ptr);
# 34
extern struct _fat_ptr Cyc_Filename_chop_extension(struct _fat_ptr);
# 40
extern struct _fat_ptr Cyc_Filename_dirname(struct _fat_ptr);
# 43
extern struct _fat_ptr Cyc_Filename_basename(struct _fat_ptr);
# 46
extern int Cyc_Filename_check_suffix(struct _fat_ptr,struct _fat_ptr);
# 48 "position.h"
extern int Cyc_Position_use_gcc_style_location;
# 13 "bansheeif.h"
int Cyc_BansheeIf_banshee_init (void);
# 29
int Cyc_BansheeIf_discharge_implications (void);
# 25 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 26 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*,struct Cyc___cycFILE*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 68 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 38 "currgn.h"
void Cyc_CurRgn_adjust_all(struct Cyc_List_List*);
# 29 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*,int,struct Cyc_List_List*);
# 33
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 35 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*,struct Cyc_List_List*);
# 48
void Cyc_Toc_finish (void);
# 26 "remove_aggregates.h"
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*);
# 25 "remove_labels.h"
struct Cyc_List_List*Cyc_RemoveLabels_remove_unused_labels(struct Cyc_List_List*);
# 29 "rename_temps.h"
struct Cyc_List_List*Cyc_RenameTemps_rename(struct Cyc_List_List*);
# 29 "remove_casts.h"
struct Cyc_List_List*Cyc_RemoveCasts_remove_casts(struct Cyc_List_List*);
# 27 "toseqc.h"
extern struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*);
# 29 "low_treeshake.h"
struct Cyc_List_List*Cyc_LowTreeShake_shake(struct Cyc_List_List*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*);
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*);
# 40 "insert_checks.h"
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*,int);struct Cyc_Interface_I;
# 36 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_empty (void);
# 45 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final (void);
# 49
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple11{struct _fat_ptr f0;struct _fat_ptr f1;};
# 57
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*,struct Cyc_Interface_I*,struct _tuple11*);
# 72 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*)(void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 27 "warn.h"
extern void Cyc_Warn_reset(struct _fat_ptr);
# 31
extern int Cyc_Warn_max_errors;
# 33
extern int Cyc_Warn_error_p (void);
# 37
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 39
void Cyc_Warn_flush_warnings (void);
# 26 "flags.h"
extern int Cyc_Flags_verbose;
# 30
extern int Cyc_Flags_porting_c_code;
# 34
extern int Cyc_Flags_no_regions;
# 36
extern int Cyc_Flags_no_reaps;
# 38
extern int Cyc_Flags_infstats;
extern int Cyc_Flags_override_fat;
extern int Cyc_Flags_interproc;
extern int Cyc_Flags_resolve;
extern int Cyc_Flags_no_merge;
extern int Cyc_Flags_tags;
# 46
extern int Cyc_Flags_no_register;
# 49
extern int Cyc_Flags_warn_override;
# 59
extern int Cyc_Flags_warn_alias_coerce;
# 63
extern int Cyc_Flags_warn_region_coerce;
# 66
extern int Cyc_Flags_warn_lose_unique;
# 70
extern int Cyc_Flags_warn_bounds_checks;
# 74
extern int Cyc_Flags_warn_all_null_deref;
# 77
extern int Cyc_Flags_warn_assert;
# 81
extern int Cyc_Flags_noexpand_r;
# 86
extern int Cyc_Flags_allpaths;
# 92
extern int Cyc_Flags_print_unprovable;
# 98
extern int Cyc_Flags_tovc_r;
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 103
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 113 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_TempRename =12U,Cyc_Flags_CastRemoval =13U,Cyc_Flags_EvalOrder =14U,Cyc_Flags_CCompiler =15U,Cyc_Flags_AllPasses =16U};
# 132
extern enum Cyc_Flags_Cyclone_Passes Cyc_Flags_stop_after_pass;
# 134
void Cyc_Flags_set_cpponly (void);
void Cyc_Flags_set_parseonly (void);
void Cyc_Flags_set_crgnonly (void);
void Cyc_Flags_set_tconly (void);
void Cyc_Flags_set_cfonly (void);
void Cyc_Flags_set_toconly (void);
# 145
extern int Cyc_Flags_compile_for_boot;
# 149
extern int Cyc_Flags_print_parser_state_and_token;
# 153
extern unsigned Cyc_Flags_max_vc_summary;
# 157
extern unsigned Cyc_Flags_max_vc_paths;
# 162
extern unsigned Cyc_Flags_max_vc_term_size;
# 164
void Cyc_Flags_set_all_warnings (void);
# 29 "specsfile.h"
extern struct _fat_ptr Cyc_Specsfile_target_arch;
extern void Cyc_Specsfile_set_target_arch(struct _fat_ptr);
extern struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
extern void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr);
# 34
extern struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr);
extern struct _fat_ptr Cyc_Specsfile_split_specs(struct _fat_ptr);
extern struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*,struct _fat_ptr);
# 38
extern struct _fat_ptr Cyc_Specsfile_def_lib_path;
extern struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 44
extern struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr);
extern struct _fat_ptr Cyc_Specsfile_find_in_exec_path(struct _fat_ptr);
# 38 "cifc.h"
void Cyc_Cifc_set_inst_tvar (void);
# 9 "interproc.h"
void Cyc_Interproc_set_inference_prefix(struct _fat_ptr);
void Cyc_Interproc_dump_tags(struct _fat_ptr,struct Cyc_List_List*);
void Cyc_Interproc_close_tags_file (void);
struct Cyc_List_List*Cyc_Interproc_resolve_all(struct _fat_ptr,struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Interproc_binding_cvar_resolution(struct Cyc_List_List*);
void Cyc_Interproc_emit_decls(struct _fat_ptr,struct Cyc_List_List*);
void Cyc_Interproc_inference_statistics (void);
# 31 "lower.h"
extern struct Cyc_List_List*Cyc_Lower_lower(struct Cyc_List_List*);
# 51 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 54
void Cyc_Lex_pos_init (void);
void Cyc_Lex_lex_init(int);
# 62
static int Cyc_pp_r=0;
static int Cyc_noshake_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_save_temps_r=0;
static int Cyc_save_c_r=0;
static int Cyc_nogc_r=0;
static int Cyc_pa_r=0;
static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;
static int Cyc_generate_line_directives_r=1;
static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;
static int Cyc_print_all_kinds_r=0;
static int Cyc_print_all_effects_r=0;
static int Cyc_nocyc_setjmp_r=0;
static int Cyc_generate_interface_r=0;
static int Cyc_toseqc_r=1;
static int Cyc_pthread_r=0;
static int Cyc_vcgen_r=1;
static int Cyc_nolower_r=0;
# 89
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _fat_ptr s){struct Cyc_List_List*_T0;struct _fat_ptr*_T1;{struct Cyc_List_List*_T2=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T3=_cycalloc(sizeof(struct _fat_ptr));
*_T3=s;_T1=(struct _fat_ptr*)_T3;}_T2->hd=_T1;_T2->tl=Cyc_ccargs;_T0=(struct Cyc_List_List*)_T2;}Cyc_ccargs=_T0;}
# 97
void Cyc_set_c_compiler(struct _fat_ptr s){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;int _T6;struct _fat_ptr _T7;_T0=s;_T1=
_tag_fat("vc",sizeof(char),3U);_T2=Cyc_strcmp(_T0,_T1);if(_T2!=0)goto _TL0;
Cyc_Flags_c_compiler=1U;_T3=
_tag_fat("-DVC_C",sizeof(char),7U);Cyc_add_ccarg(_T3);goto _TL1;
# 102
_TL0: _T4=s;_T5=_tag_fat("gcc",sizeof(char),4U);_T6=Cyc_strcmp(_T4,_T5);if(_T6!=0)goto _TL2;
Cyc_Flags_c_compiler=0U;_T7=
_tag_fat("-DGCC_C",sizeof(char),8U);Cyc_add_ccarg(_T7);goto _TL3;_TL2: _TL3: _TL1:;}
# 109
void Cyc_set_port_c_code (void){
Cyc_Flags_porting_c_code=1;
Cyc_Warn_max_errors=65535;
Cyc_save_c_r=1;
Cyc_Flags_set_parseonly();}
# 116
static struct _fat_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _fat_ptr s){struct _fat_ptr*_T0;{struct _fat_ptr*_T1=_cycalloc(sizeof(struct _fat_ptr));
*_T1=s;_T0=(struct _fat_ptr*)_T1;}Cyc_output_file=_T0;}
# 121
static struct _fat_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _fat_ptr s){
Cyc_specified_interface=s;}
# 131
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Cversion;static char _TmpG0[1U]="";
# 136
static struct _fat_ptr Cyc_cpp={_TmpG0,_TmpG0,_TmpG0 + 1U};
static void Cyc_set_cpp(struct _fat_ptr s){
Cyc_cpp=s;}
# 141
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _fat_ptr s){struct Cyc_List_List*_T0;struct _fat_ptr*_T1;{struct Cyc_List_List*_T2=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T3=_cycalloc(sizeof(struct _fat_ptr));
*_T3=s;_T1=(struct _fat_ptr*)_T3;}_T2->hd=_T1;_T2->tl=Cyc_cppargs;_T0=(struct Cyc_List_List*)_T2;}Cyc_cppargs=_T0;}
# 147
static void Cyc_add_cpp_and_ccarg(struct _fat_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 152
static void Cyc_add_iprefix(struct _fat_ptr s){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;{struct Cyc_String_pa_PrintArg_struct _T5;_T5.tag=0;
_T5.f1=s;_T1=_T5;}{struct Cyc_String_pa_PrintArg_struct _T5=_T1;void*_T6[1];_T2=_T6 + 0;*_T2=& _T5;_T3=_tag_fat("-iprefix %s",sizeof(char),12U);_T4=_tag_fat(_T6,sizeof(void*),1);_T0=Cyc_aprintf(_T3,_T4);}Cyc_add_cpparg(_T0);}
# 155
static void Cyc_add_iwithprefix(struct _fat_ptr s){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;{struct Cyc_String_pa_PrintArg_struct _T5;_T5.tag=0;
_T5.f1=s;_T1=_T5;}{struct Cyc_String_pa_PrintArg_struct _T5=_T1;void*_T6[1];_T2=_T6 + 0;*_T2=& _T5;_T3=_tag_fat("-iwithprefix %s",sizeof(char),16U);_T4=_tag_fat(_T6,sizeof(void*),1);_T0=Cyc_aprintf(_T3,_T4);}Cyc_add_cpparg(_T0);}
# 158
static void Cyc_add_iwithprefixbefore(struct _fat_ptr s){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;{struct Cyc_String_pa_PrintArg_struct _T5;_T5.tag=0;
_T5.f1=s;_T1=_T5;}{struct Cyc_String_pa_PrintArg_struct _T5=_T1;void*_T6[1];_T2=_T6 + 0;*_T2=& _T5;_T3=_tag_fat("-iwithprefixbefore %s",sizeof(char),22U);_T4=_tag_fat(_T6,sizeof(void*),1);_T0=Cyc_aprintf(_T3,_T4);}Cyc_add_cpparg(_T0);}
# 161
static void Cyc_add_isystem(struct _fat_ptr s){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;{struct Cyc_String_pa_PrintArg_struct _T5;_T5.tag=0;
_T5.f1=s;_T1=_T5;}{struct Cyc_String_pa_PrintArg_struct _T5=_T1;void*_T6[1];_T2=_T6 + 0;*_T2=& _T5;_T3=_tag_fat("-isystem %s",sizeof(char),12U);_T4=_tag_fat(_T6,sizeof(void*),1);_T0=Cyc_aprintf(_T3,_T4);}Cyc_add_cpparg(_T0);}
# 164
static void Cyc_add_idirafter(struct _fat_ptr s){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;{struct Cyc_String_pa_PrintArg_struct _T5;_T5.tag=0;
_T5.f1=s;_T1=_T5;}{struct Cyc_String_pa_PrintArg_struct _T5=_T1;void*_T6[1];_T2=_T6 + 0;*_T2=& _T5;_T3=_tag_fat("-idirafter %s",sizeof(char),14U);_T4=_tag_fat(_T6,sizeof(void*),1);_T0=Cyc_aprintf(_T3,_T4);}Cyc_add_cpparg(_T0);}
# 168
static void Cyc_set_many_errors (void){
Cyc_Warn_max_errors=65535;}static char _TmpG1[20U]="_see_cycspecs_file_";
# 172
static struct _fat_ptr Cyc_def_inc_path={_TmpG1,_TmpG1,_TmpG1 + 20U};
# 175
static void Cyc_print_version (void){struct Cyc_String_pa_PrintArg_struct _T0;struct _fat_ptr _T1;void*_T2;void*_T3;unsigned _T4;void**_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_String_pa_PrintArg_struct _T8;struct _fat_ptr _T9;void*_TA;void*_TB;unsigned _TC;void**_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_String_pa_PrintArg_struct _T10;struct _fat_ptr _T11;void*_T12;void*_T13;unsigned _T14;void**_T15;struct _fat_ptr _T16;struct _fat_ptr _T17;struct Cyc_String_pa_PrintArg_struct _T18;void**_T19;struct _fat_ptr _T1A;struct _fat_ptr _T1B;{struct Cyc_String_pa_PrintArg_struct _T1C;_T1C.tag=0;{char*_T1D=Cversion;_T2=(void*)_T1D;_T3=(void*)_T1D;_T4=_get_zero_arr_size_char(_T3,1U);_T1=_tag_fat(_T2,sizeof(char),_T4);}
_T1C.f1=_T1;_T0=_T1C;}{struct Cyc_String_pa_PrintArg_struct _T1C=_T0;void*_T1D[1];_T5=_T1D + 0;*_T5=& _T1C;_T6=_tag_fat("The Cyclone compiler, version %s\n",sizeof(char),34U);_T7=_tag_fat(_T1D,sizeof(void*),1);Cyc_printf(_T6,_T7);}{struct Cyc_String_pa_PrintArg_struct _T1C;_T1C.tag=0;{char*_T1D=Carch;_TA=(void*)_T1D;_TB=(void*)_T1D;_TC=_get_zero_arr_size_char(_TB,1U);_T9=_tag_fat(_TA,sizeof(char),_TC);}
_T1C.f1=_T9;_T8=_T1C;}{struct Cyc_String_pa_PrintArg_struct _T1C=_T8;void*_T1D[1];_TD=_T1D + 0;*_TD=& _T1C;_TE=_tag_fat("Compiled for architecture: %s\n",sizeof(char),31U);_TF=_tag_fat(_T1D,sizeof(void*),1);Cyc_printf(_TE,_TF);}{struct Cyc_String_pa_PrintArg_struct _T1C;_T1C.tag=0;{char*_T1D=Cdef_lib_path;_T12=(void*)_T1D;_T13=(void*)_T1D;_T14=_get_zero_arr_size_char(_T13,1U);_T11=_tag_fat(_T12,sizeof(char),_T14);}
_T1C.f1=_T11;_T10=_T1C;}{struct Cyc_String_pa_PrintArg_struct _T1C=_T10;void*_T1D[1];_T15=_T1D + 0;*_T15=& _T1C;_T16=_tag_fat("Standard library directory: %s\n",sizeof(char),32U);_T17=_tag_fat(_T1D,sizeof(void*),1);Cyc_printf(_T16,_T17);}{struct Cyc_String_pa_PrintArg_struct _T1C;_T1C.tag=0;
# 180
_T1C.f1=Cyc_def_inc_path;_T18=_T1C;}{struct Cyc_String_pa_PrintArg_struct _T1C=_T18;void*_T1D[1];_T19=_T1D + 0;*_T19=& _T1C;_T1A=_tag_fat("Standard include directory: %s\n",sizeof(char),32U);_T1B=_tag_fat(_T1D,sizeof(void*),1);Cyc_printf(_T1A,_T1B);}
exit(0);}
# 184
static int Cyc_is_cyclone_sourcefile(struct _fat_ptr s){struct _fat_ptr _T0;unsigned long _T1;int _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;int _T5;int _T6;
unsigned long len=Cyc_strlen(s);
if(len > 4U)goto _TL4;return 0;_TL4: _T0=s;_T1=len - 4U;_T2=(int)_T1;_T3=
_fat_ptr_plus(_T0,sizeof(char),_T2);_T4=_tag_fat(".cyc",sizeof(char),5U);_T5=Cyc_strcmp(_T3,_T4);_T6=_T5==0;return _T6;}
# 200 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _fat_ptr s){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;struct _fat_ptr _T3;int _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;int _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;struct Cyc_List_List*_TB;struct _fat_ptr*_TC;_T0=s;_T1=
_tag_fat("-lxml",sizeof(char),6U);_T2=Cyc_strcmp(_T0,_T1);if(_T2!=0)goto _TL6;_T4=Cyc_pa_r;
if(!_T4)goto _TL8;_T3=_tag_fat("-lxml_a",sizeof(char),8U);goto _TL9;_TL8: _T3=s;_TL9: Cyc_add_ccarg(_T3);goto _TL7;
_TL6: _T5=s;_T6=_tag_fat("-lcyc",sizeof(char),6U);_T7=Cyc_strncmp(_T5,_T6,5U);if(_T7!=0)goto _TLA;
Cyc_add_ccarg(s);goto _TLB;
# 207
_TLA: _T8=s;_T9=_tag_fat("-lpthread",sizeof(char),10U);_TA=Cyc_strncmp(_T8,_T9,9U);if(_TA!=0)goto _TLC;
Cyc_pthread_r=1;goto _TLD;_TLC: _TLD:{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TE=_cycalloc(sizeof(struct _fat_ptr));
*_TE=s;_TC=(struct _fat_ptr*)_TE;}_TD->hd=_TC;_TD->tl=Cyc_libargs;_TB=(struct Cyc_List_List*)_TD;}Cyc_libargs=_TB;_TLB: _TL7:;}
# 213
static void Cyc_add_marg(struct _fat_ptr s){
Cyc_add_ccarg(s);}
# 217
static void Cyc_set_save_temps (void){struct _fat_ptr _T0;
Cyc_save_temps_r=1;_T0=
_tag_fat("-save-temps",sizeof(char),12U);Cyc_add_ccarg(_T0);}
# 222
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies (void){struct _fat_ptr _T0;
Cyc_Flags_stop_after_pass=0U;
Cyc_produce_dependencies=1;_T0=
_tag_fat("-M",sizeof(char),3U);Cyc_add_cpparg(_T0);}
# 229
static struct _fat_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _fat_ptr s){struct _fat_ptr*_T0;{struct _fat_ptr*_T1=_cycalloc(sizeof(struct _fat_ptr));
*_T1=s;_T0=(struct _fat_ptr*)_T1;}Cyc_dependencies_target=_T0;}
# 234
static void Cyc_set_stop_after_objectfile (void){struct _fat_ptr _T0;
Cyc_stop_after_objectfile_r=1;_T0=
_tag_fat("-c",sizeof(char),3U);Cyc_add_ccarg(_T0);}
# 239
static void Cyc_set_nocppprecomp (void){struct _fat_ptr _T0;_T0=
_tag_fat("-no-cpp-precomp",sizeof(char),16U);Cyc_add_cpp_and_ccarg(_T0);}
# 243
static void Cyc_clear_lineno (void){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 247
static void Cyc_set_elim_se (void){
Cyc_elim_se_r=1;}
# 250
static void Cyc_set_tovc (void){struct _fat_ptr _T0;
Cyc_Flags_tovc_r=1;_T0=
_tag_fat("-DCYC_ANSI_OUTPUT",sizeof(char),18U);Cyc_add_ccarg(_T0);
Cyc_set_elim_se();}
# 255
static void Cyc_set_notoseqc (void){
Cyc_toseqc_r=0;}
# 258
static void Cyc_set_noboundschecks (void){struct _fat_ptr _T0;_T0=
_tag_fat("-DNO_CYC_BOUNDS_CHECKS",sizeof(char),23U);Cyc_add_ccarg(_T0);}
# 261
static void Cyc_set_nonullchecks (void){struct _fat_ptr _T0;_T0=
_tag_fat("-DNO_CYC_NULL_CHECKS",sizeof(char),21U);Cyc_add_ccarg(_T0);}
# 264
static void Cyc_set_nochecks (void){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 270
static void Cyc_set_nocyc (void){struct _fat_ptr _T0;
Cyc_add_cyc_namespace_r=0;_T0=
_tag_fat("-DNO_CYC_PREFIX",sizeof(char),16U);Cyc_add_ccarg(_T0);}
# 275
static void Cyc_set_pa (void){struct _fat_ptr _T0;struct _fat_ptr _T1;
Cyc_pa_r=1;_T0=
_tag_fat("-DCYC_REGION_PROFILE",sizeof(char),21U);Cyc_add_ccarg(_T0);_T1=
_tag_fat("-DCYC_REGION_PROFILE",sizeof(char),21U);Cyc_add_cpparg(_T1);}
# 281
static void Cyc_set_noreaps (void){struct _fat_ptr _T0;
# 284
Cyc_Flags_no_reaps=1;_T0=
_tag_fat("-D_CYC_INTERNAL_DISABLE_REAPS",sizeof(char),30U);Cyc_add_cpparg(_T0);}
# 288
static int Cyc_binit_done=0;
# 290
static void Cyc_set_resolve (void){int _T0;int _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;_T0=Cyc_binit_done;
if(_T0)goto _TLE;else{goto _TL10;}
_TL10: Cyc_binit_done=1;_T1=
Cyc_BansheeIf_banshee_init();if(!_T1)goto _TL11;
Cyc_Flags_resolve=1;goto _TL12;
# 297
_TL11: _T2=_tag_fat("Interprocedural inference is not supported by this build of Cyclone; ignoring --inf option",sizeof(char),91U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(0U,_T2,_T3);_TL12: goto _TLF;_TLE: _TLF:;}
# 302
static void Cyc_set_interproc (void){int _T0;int _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;_T0=Cyc_binit_done;
if(_T0)goto _TL13;else{goto _TL15;}
_TL15: Cyc_binit_done=1;_T1=
Cyc_BansheeIf_banshee_init();if(!_T1)goto _TL16;
Cyc_Flags_interproc=1;goto _TL17;
# 309
_TL16: _T2=_tag_fat("Interprocedural inference is not supported by this build of Cyclone; ignoring --inf option",sizeof(char),91U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(0U,_T2,_T3);_TL17: goto _TL14;_TL13: _TL14:;}
# 313
static void Cyc_set_ovfat (void){int _T0;int _T1;int _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;_T0=Cyc_Flags_interproc;
if(!_T0)goto _TL18;_T1=Cyc_Flags_resolve;if(_T1)goto _TL18;else{goto _TL1A;}
_TL1A: Cyc_Flags_override_fat=1;goto _TL19;_TL18: _TL19: _T2=Cyc_Flags_resolve;
if(!_T2)goto _TL1B;_T3=
_tag_fat("Incompatible options --ovfat and --resolve",sizeof(char),43U);_T4=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(0U,_T3,_T4);goto _TL1C;_TL1B: _TL1C:;}
# 319
static void Cyc_set_infstats (void){struct _fat_ptr _T0;struct _fat_ptr _T1;
Cyc_set_interproc();
Cyc_Flags_infstats=1;_T0=
_tag_fat("--infstats causes all other options to be ignored",sizeof(char),50U);_T1=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(0U,_T0,_T1);}
# 325
static void Cyc_set_nomerge (void){int _T0;int _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;int _T4;_T0=Cyc_binit_done;
if(_T0)goto _TL1D;else{goto _TL1F;}
_TL1F: Cyc_binit_done=1;_T1=
Cyc_BansheeIf_banshee_init();if(!_T1)goto _TL20;
Cyc_Flags_interproc=1;
Cyc_Flags_no_merge=1;goto _TL21;
# 333
_TL20: _T2=_tag_fat("Interprocedural inference is not supported by this build of Cyclone; ignoring --nomerge option",sizeof(char),95U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(0U,_T2,_T3);_TL21: goto _TL1E;
# 336
_TL1D: _T4=Cyc_Flags_interproc;if(!_T4)goto _TL22;
Cyc_Flags_no_merge=1;
Cyc_Flags_stop_after_pass=6U;goto _TL23;_TL22: _TL23: _TL1E:;}
# 342
static void Cyc_set_tags (void){
Cyc_Flags_tags=1;
Cyc_Flags_stop_after_pass=1U;}
# 347
static void Cyc_set_pg (void){struct _fat_ptr _T0;
Cyc_pg_r=1;_T0=
_tag_fat("-pg",sizeof(char),4U);Cyc_add_ccarg(_T0);}
# 352
static void Cyc_set_stop_after_asmfile (void){struct _fat_ptr _T0;
Cyc_stop_after_asmfile_r=1;_T0=
_tag_fat("-S",sizeof(char),3U);Cyc_add_ccarg(_T0);}
# 358
enum Cyc_inputtype{Cyc_DEFAULTINPUT =0U,Cyc_CYCLONEFILE =1U};
# 362
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _fat_ptr s){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;int _T5;struct Cyc_String_pa_PrintArg_struct _T6;void**_T7;struct Cyc___cycFILE*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;_T0=s;_T1=
_tag_fat("cyc",sizeof(char),4U);_T2=Cyc_strcmp(_T0,_T1);if(_T2!=0)goto _TL24;
Cyc_intype=1U;goto _TL25;
_TL24: _T3=s;_T4=_tag_fat("none",sizeof(char),5U);_T5=Cyc_strcmp(_T3,_T4);if(_T5!=0)goto _TL26;
Cyc_intype=0U;goto _TL27;
# 369
_TL26:{struct Cyc_String_pa_PrintArg_struct _TB;_TB.tag=0;_TB.f1=s;_T6=_TB;}{struct Cyc_String_pa_PrintArg_struct _TB=_T6;void*_TC[1];_T7=_TC + 0;*_T7=& _TB;_T8=Cyc_stderr;_T9=_tag_fat("Input type '%s' not supported\n",sizeof(char),31U);_TA=_tag_fat(_TC,sizeof(void*),1);Cyc_fprintf(_T8,_T9,_TA);}_TL27: _TL25:;}
# 372
struct _fat_ptr Cyc_make_base_filename(struct _fat_ptr s,struct _fat_ptr*output_file){struct _fat_ptr _T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;unsigned long _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;
if(output_file!=0)goto _TL28;_T0=s;goto _TL29;_TL28: _T1=output_file;_T0=*_T1;_TL29: {struct _fat_ptr outdir=Cyc_Filename_dirname(_T0);_T2=
Cyc_Filename_basename(s);{struct _fat_ptr base=Cyc_Filename_chop_extension(_T2);_T4=
# 378
Cyc_strlen(outdir);if(_T4 <= 0U)goto _TL2A;_T3=Cyc_Filename_concat(outdir,base);goto _TL2B;_TL2A: _T3=base;_TL2B: {struct _fat_ptr basename=_T3;_T5=basename;_T6=_T5;
return _T6;}}}}
# 384
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 386
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _fat_ptr s){enum Cyc_inputtype _T0;int _T1;int _T2;struct Cyc_List_List*_T3;struct _fat_ptr*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;
# 389
if(Cyc_gcc_optarg <= 0)goto _TL2C;
Cyc_add_ccarg(s);
Cyc_gcc_optarg=Cyc_gcc_optarg + -1;goto _TL2D;
_TL2C: _T0=Cyc_intype;_T1=(int)_T0;if(_T1==1)goto _TL30;else{goto _TL31;}_TL31: _T2=Cyc_is_cyclone_sourcefile(s);if(_T2)goto _TL30;else{goto _TL2E;}
# 394
_TL30:{struct Cyc_List_List*_T7=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T8=_cycalloc(sizeof(struct _fat_ptr));*_T8=s;_T4=(struct _fat_ptr*)_T8;}_T7->hd=_T4;_T7->tl=Cyc_cyclone_files;_T3=(struct Cyc_List_List*)_T7;}Cyc_cyclone_files=_T3;{
# 400
struct _fat_ptr basename=Cyc_make_base_filename(s,0);_T5=basename;_T6=
_tag_fat(".c",sizeof(char),3U);{struct _fat_ptr cfile=Cyc_strconcat(_T5,_T6);
Cyc_add_ccarg(cfile);}}goto _TL2F;
# 404
_TL2E: Cyc_add_ccarg(s);_TL2F: _TL2D:;}
# 409
static int Cyc_assume_gcc_flag=1;struct _tuple12{struct _fat_ptr flag;int optargs;};static char _TmpG2[3U]="-s";static char _TmpG3[3U]="-O";static char _TmpG4[4U]="-O0";static char _TmpG5[4U]="-O2";static char _TmpG6[4U]="-O3";static char _TmpG7[21U]="-fomit-frame-pointer";static char _TmpG8[13U]="-fno-builtin";static char _TmpG9[3U]="-g";static char _TmpGA[3U]="-p";static char _TmpGB[8U]="-static";
static int Cyc_add_other_flag(struct _fat_ptr s){int _T0;int _T1;unsigned _T2;struct _tuple12*_T3;int _T4;char*_T5;struct _tuple12*_T6;struct _tuple12 _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;struct _tuple12*_TB;int _TC;struct _tuple12 _TD;
static struct _tuple12 known_gcc_flags[10U]={{.flag={_TmpG2,_TmpG2,_TmpG2 + 3U},.optargs=0},{.flag={_TmpG3,_TmpG3,_TmpG3 + 3U},.optargs=0},{.flag={_TmpG4,_TmpG4,_TmpG4 + 4U},.optargs=0},{.flag={_TmpG5,_TmpG5,_TmpG5 + 4U},.optargs=0},{.flag={_TmpG6,_TmpG6,_TmpG6 + 4U},.optargs=0},{.flag={_TmpG7,_TmpG7,_TmpG7 + 21U},.optargs=0},{.flag={_TmpG8,_TmpG8,_TmpG8 + 13U},.optargs=0},{.flag={_TmpG9,_TmpG9,_TmpG9 + 3U},.optargs=0},{.flag={_TmpGA,_TmpGA,_TmpGA + 3U},.optargs=0},{.flag={_TmpGB,_TmpGB,_TmpGB + 8U},.optargs=0}};_T0=Cyc_assume_gcc_flag;
# 423
if(!_T0)goto _TL32;
Cyc_add_ccarg(s);
return 1;
# 427
_TL32:{int i=0;_TL37: _T1=i;_T2=(unsigned)_T1;if(_T2 < 10U)goto _TL35;else{goto _TL36;}
_TL35: _T3=known_gcc_flags;_T4=i;_T5=_check_known_subscript_notnull(_T3,10U,sizeof(struct _tuple12),_T4);_T6=(struct _tuple12*)_T5;_T7=*_T6;_T8=_T7.flag;_T9=s;_TA=Cyc_strcmp(_T8,_T9);if(_TA)goto _TL38;else{goto _TL3A;}
_TL3A: Cyc_add_ccarg(s);_TB=known_gcc_flags;_TC=i;_TD=_TB[_TC];
Cyc_gcc_optarg=_TD.optargs;goto _TL36;_TL38:
# 427
 i=i + 1;goto _TL37;_TL36:;}
# 433
return 0;}
# 437
static void Cyc_noassume_gcc_flag (void){
Cyc_assume_gcc_flag=0;}
# 441
static void Cyc_remove_file(struct _fat_ptr s){int _T0;struct _fat_ptr _T1;char*_T2;char*_T3;const char*_T4;_T0=Cyc_save_temps_r;
if(_T0)goto _TL3B;else{goto _TL3D;}
_TL3D: _T1=s;_T2=_untag_fat_ptr_check_bound(_T1,sizeof(char),1U);_T3=_check_null(_T2);_T4=(const char*)_T3;remove(_T4);goto _TL3C;_TL3B: _TL3C:;}
# 448
int Cyc_compile_failure=0;
# 450
struct Cyc___cycFILE*Cyc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){struct _handler_cons*_T0;int*_T1;int _T2;void*_T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc_String_pa_PrintArg_struct _T5;void**_T6;void**_T7;struct Cyc___cycFILE*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct _handler_cons _TB;_T0=& _TB;_push_handler(_T0);{int _TC=0;_T1=_TB.handler;_T2=setjmp(_T1);if(!_T2)goto _TL3E;_TC=1;goto _TL3F;_TL3E: _TL3F: if(_TC)goto _TL40;else{goto _TL42;}_TL42:{struct Cyc___cycFILE*_TD=
Cyc_file_open(filename,mode);_npop_handler(0);return _TD;}_pop_handler();goto _TL41;_TL40: _T3=Cyc_Core_get_exn_thrown();{void*_TD=(void*)_T3;
# 453
Cyc_compile_failure=1;{struct Cyc_String_pa_PrintArg_struct _TE;_TE.tag=0;
_TE.f1=msg_part;_T4=_TE;}{struct Cyc_String_pa_PrintArg_struct _TE=_T4;{struct Cyc_String_pa_PrintArg_struct _TF;_TF.tag=0;_TF.f1=filename;_T5=_TF;}{struct Cyc_String_pa_PrintArg_struct _TF=_T5;void*_T10[2];_T6=_T10 + 0;*_T6=& _TE;_T7=_T10 + 1;*_T7=& _TF;_T8=Cyc_stderr;_T9=_tag_fat("\nError: couldn't open %s %s\n",sizeof(char),29U);_TA=_tag_fat(_T10,sizeof(void*),2);Cyc_fprintf(_T8,_T9,_TA);}}
Cyc_fflush(Cyc_stderr);
return 0;;}_TL41:;}}struct _tuple13{void*f0;const char*f1;int f2;};
# 460
struct _tuple13*Cyc_uncaught_exn=0;
# 462
extern void CYCALLOCPROFILE_mark(const char*);
# 464
void*Cyc_do_stage(struct _fat_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void*onfail){struct _fat_ptr _T0;char*_T1;const char*_T2;int _T3;struct Cyc_timeval*_T4;struct Cyc_timeval*_T5;struct _handler_cons*_T6;int*_T7;int _T8;void*_T9;int _TA;struct Cyc_String_pa_PrintArg_struct _TB;void**_TC;struct Cyc___cycFILE*_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct _tuple13*_T10;int _T11;int _T12;struct Cyc___cycFILE*_T13;struct _fat_ptr _T14;struct _fat_ptr _T15;int _T16;struct Cyc_String_pa_PrintArg_struct _T17;void**_T18;struct Cyc___cycFILE*_T19;struct _fat_ptr _T1A;struct _fat_ptr _T1B;struct Cyc_timeval*_T1C;struct Cyc_timeval*_T1D;struct Cyc_timeval _T1E;long _T1F;struct Cyc_timeval _T20;long _T21;struct Cyc_timeval _T22;long _T23;struct Cyc_Int_pa_PrintArg_struct _T24;struct Cyc_timeval _T25;long _T26;struct Cyc_Int_pa_PrintArg_struct _T27;struct Cyc_timeval _T28;long _T29;void**_T2A;void**_T2B;struct Cyc___cycFILE*_T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;void*_T2F;_T0=stage_name;_T1=_untag_fat_ptr_check_bound(_T0,sizeof(char),1U);_T2=(const char*)_T1;
# 466
CYCALLOCPROFILE_mark(_T2);{
struct Cyc_timeval start;struct Cyc_timeval end;_T3=Cyc_Flags_verbose;
if(!_T3)goto _TL43;_T4=& start;_T5=(struct Cyc_timeval*)_T4;
gettimeofday(_T5,0);goto _TL44;_TL43: _TL44: {
# 471
void*ans;{struct _handler_cons _T30;_T6=& _T30;_push_handler(_T6);{int _T31=0;_T7=_T30.handler;_T8=setjmp(_T7);if(!_T8)goto _TL45;_T31=1;goto _TL46;_TL45: _TL46: if(_T31)goto _TL47;else{goto _TL49;}_TL49:
 ans=f(env,tds);_pop_handler();goto _TL48;_TL47: _T9=Cyc_Core_get_exn_thrown();{void*_T32=(void*)_T9;void*_T33;_T33=_T32;{void*x=_T33;
# 474
ans=onfail;_TA=
Cyc_Warn_error_p();if(!_TA)goto _TL4A;goto _LL0;_TL4A:
# 477
 Cyc_compile_failure=1;{struct Cyc_String_pa_PrintArg_struct _T34;_T34.tag=0;
_T34.f1=stage_name;_TB=_T34;}{struct Cyc_String_pa_PrintArg_struct _T34=_TB;void*_T35[1];_TC=_T35 + 0;*_TC=& _T34;_TD=Cyc_stderr;_TE=_tag_fat("COMPILER STAGE %s (unexpected exception) \n",sizeof(char),43U);_TF=_tag_fat(_T35,sizeof(void*),1);Cyc_fprintf(_TD,_TE,_TF);}{struct _tuple13*_T34=_cycalloc(sizeof(struct _tuple13));
_T34->f0=x;_T34->f1=Cyc_Core_get_exn_filename();_T34->f2=Cyc_Core_get_exn_lineno();_T10=(struct _tuple13*)_T34;}Cyc_uncaught_exn=_T10;goto _LL0;}_LL0:;}_TL48:;}}_T11=
# 482
Cyc_Warn_error_p();if(!_T11)goto _TL4C;
Cyc_compile_failure=1;goto _TL4D;_TL4C: _TL4D: _T12=Cyc_compile_failure;
if(!_T12)goto _TL4E;_T13=Cyc_stderr;_T14=
_tag_fat("\nCOMPILATION FAILED!\n",sizeof(char),22U);_T15=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T13,_T14,_T15);
Cyc_fflush(Cyc_stderr);goto _TL4F;
_TL4E: _T16=Cyc_Flags_verbose;if(!_T16)goto _TL50;{struct Cyc_String_pa_PrintArg_struct _T30;_T30.tag=0;
_T30.f1=stage_name;_T17=_T30;}{struct Cyc_String_pa_PrintArg_struct _T30=_T17;void*_T31[1];_T18=_T31 + 0;*_T18=& _T30;_T19=Cyc_stderr;_T1A=_tag_fat("%s completed ",sizeof(char),14U);_T1B=_tag_fat(_T31,sizeof(void*),1);Cyc_fprintf(_T19,_T1A,_T1B);}_T1C=& end;_T1D=(struct Cyc_timeval*)_T1C;
gettimeofday(_T1D,0);_T1E=start;_T1F=_T1E.tv_sec;
end.tv_sec=end.tv_sec - _T1F;_T20=start;_T21=_T20.tv_usec;
end.tv_usec=end.tv_usec - _T21;_T22=end;_T23=_T22.tv_usec;
if(_T23 >= 0)goto _TL52;
end.tv_usec=end.tv_usec + 1000000;
end.tv_sec=end.tv_sec + -1;goto _TL53;_TL52: _TL53:{struct Cyc_Int_pa_PrintArg_struct _T30;_T30.tag=1;_T25=end;_T26=_T25.tv_sec;
# 496
_T30.f1=(unsigned long)_T26;_T24=_T30;}{struct Cyc_Int_pa_PrintArg_struct _T30=_T24;{struct Cyc_Int_pa_PrintArg_struct _T31;_T31.tag=1;_T28=end;_T29=_T28.tv_usec;_T31.f1=(unsigned long)_T29;_T27=_T31;}{struct Cyc_Int_pa_PrintArg_struct _T31=_T27;void*_T32[2];_T2A=_T32 + 0;*_T2A=& _T30;_T2B=_T32 + 1;*_T2B=& _T31;_T2C=Cyc_stderr;_T2D=_tag_fat("(%d.%06d secs)\n",sizeof(char),16U);_T2E=_tag_fat(_T32,sizeof(void*),2);Cyc_fprintf(_T2C,_T2D,_T2E);}}
Cyc_fflush(Cyc_stderr);goto _TL51;_TL50: _TL51: _TL4F: _T2F=ans;
# 499
return _T2F;}}}
# 502
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){struct _handler_cons*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Parse_Exit_exn_struct*_T4;char*_T5;char*_T6;struct Cyc_List_List*_T7;
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;{struct _handler_cons _T8;_T0=& _T8;_push_handler(_T0);{int _T9=0;_T1=_T8.handler;_T2=setjmp(_T1);if(!_T2)goto _TL54;_T9=1;goto _TL55;_TL54: _TL55: if(_T9)goto _TL56;else{goto _TL58;}_TL58:
# 507
 ans=Cyc_Parse_parse_file(f);
Cyc_file_close(f);_pop_handler();goto _TL57;_TL56: _T3=Cyc_Core_get_exn_thrown();{void*_TA=(void*)_T3;void*_TB;_T4=(struct Cyc_Parse_Exit_exn_struct*)_TA;_T5=_T4->tag;_T6=Cyc_Parse_Exit;if(_T5!=_T6)goto _TL59;
# 510
Cyc_file_close(f);Cyc_compile_failure=1;goto _LL0;_TL59: _TB=_TA;{void*e=_TB;
Cyc_file_close(f);Cyc_Core_rethrow(e);}_LL0:;}_TL57:;}}
# 513
Cyc_Lex_lex_init(1);_T7=ans;
return _T7;}}
# 517
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}
# 522
struct Cyc_List_List*Cyc_do_interproc_binding(struct _fat_ptr*filename,struct Cyc_List_List*tds){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;_T0=filename;_T1=*_T0;_T2=tds;_T3=
Cyc_Interproc_resolve_all(_T1,_T2);return _T3;}
# 526
struct Cyc_List_List*Cyc_do_interproc_resolution(int ig,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;_T0=
Cyc_Interproc_binding_cvar_resolution(tds);return _T0;}
# 530
int Cyc_do_adjust(int ignore,struct Cyc_List_List*tds){
Cyc_CurRgn_adjust_all(tds);
return 1;}
# 535
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){int _T0;struct Cyc_List_List*_T1;
# 537
Cyc_Tc_tc(te,1,tds);_T0=Cyc_noshake_r;
if(_T0)goto _TL5B;else{goto _TL5D;}
_TL5D: tds=Cyc_Tc_treeshake(te,tds);goto _TL5C;_TL5B: _TL5C: _T1=tds;
return _T1;}
# 543
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){struct Cyc_JumpAnalysis_Jump_Anal_Result*_T0;_T0=
# 545
Cyc_JumpAnalysis_jump_analysis(tds);return _T0;}
# 548
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;
# 550
Cyc_NewControlFlow_cf_check(tables,tds);_T0=tds;
return _T0;}
# 554
int Cyc_do_insert_checks(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
Cyc_InsertChecks_insert_checks(tds,tables,Cyc_vcgen_r);
return 1;}struct _tuple14{struct Cyc_Tcenv_Tenv*f0;struct Cyc___cycFILE*f1;struct Cyc___cycFILE*f2;};
# 561
struct Cyc_List_List*Cyc_do_interface(struct _tuple14*params,struct Cyc_List_List*tds){struct _tuple14*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Genv*_T2;struct Cyc_List_List*_T3;struct Cyc_Interface_I*_T4;struct Cyc_Interface_I*_T5;struct _tuple11*_T6;int _T7;struct Cyc_List_List*_T8;struct Cyc___cycFILE*_T9;struct Cyc___cycFILE*_TA;struct Cyc_Tcenv_Tenv*_TB;_T0=params;{struct _tuple14 _TC=*_T0;_TB=_TC.f0;_TA=_TC.f1;_T9=_TC.f2;}{struct Cyc_Tcenv_Tenv*te=_TB;struct Cyc___cycFILE*fi=_TA;struct Cyc___cycFILE*fo=_T9;_T1=te;_T2=_T1->ae;_T3=tds;{
# 564
struct Cyc_Interface_I*i1=Cyc_Interface_extract(_T2,_T3);
if(fi!=0)goto _TL5E;
Cyc_Interface_save(i1,fo);goto _TL5F;
# 568
_TL5E:{struct Cyc_Interface_I*i0=Cyc_Interface_parse(fi);_T4=i0;_T5=i1;{struct _tuple11*_TC=_cycalloc(sizeof(struct _tuple11));
_TC->f0=_tag_fat("written interface",sizeof(char),18U);_TC->f1=_tag_fat("maximal interface",sizeof(char),18U);_T6=(struct _tuple11*)_TC;}_T7=Cyc_Interface_is_subinterface(_T4,_T5,_T6);if(_T7)goto _TL60;else{goto _TL62;}
_TL62: Cyc_compile_failure=1;goto _TL61;
# 572
_TL60: Cyc_Interface_save(i0,fo);_TL61:;}_TL5F: _T8=tds;
# 574
return _T8;}}}
# 577
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;_T0=
# 580
Cyc_Toc_toc(pops,tds);return _T0;}
# 582
struct Cyc_List_List*Cyc_do_removeaggrs(int ignore,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;_T0=
Cyc_RemoveAggrs_remove_aggrs(tds);return _T0;}
# 585
struct Cyc_List_List*Cyc_do_removelabs(int ignore,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;_T0=
Cyc_RemoveLabels_remove_unused_labels(tds);return _T0;}
# 588
struct Cyc_List_List*Cyc_do_temprename(int ignore,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;_T0=
Cyc_RenameTemps_rename(tds);return _T0;}
# 591
struct Cyc_List_List*Cyc_do_castremove(int ignore,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;_T0=
Cyc_RemoveCasts_remove_casts(tds);return _T0;}
# 594
struct Cyc_List_List*Cyc_do_lower(int ignore,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;_T0=
Cyc_Lower_lower(tds);return _T0;}
# 598
struct Cyc_List_List*Cyc_do_toseqc(int ignore,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;_T0=
Cyc_Toseqc_toseqc(tds);return _T0;}
# 601
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;
Cyc_Tovc_elim_array_initializers=Cyc_Flags_tovc_r;_T0=
Cyc_Tovc_tovc(tds);return _T0;}
# 605
struct Cyc_List_List*Cyc_do_lowtreeshake(int ignore,struct Cyc_List_List*tds){int _T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;_T0=Cyc_noshake_r;
if(_T0)goto _TL63;else{goto _TL65;}
_TL65: _T1=Cyc_LowTreeShake_shake(tds);return _T1;
_TL63: _T2=tds;return _T2;}
# 611
static struct _fat_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 613
static struct _fat_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 615
static void Cyc_set_cyc_include(struct _fat_ptr f){
Cyc_cyc_include=f;}
# 620
int Cyc_copy_internal_file(struct _fat_ptr file,struct Cyc___cycFILE*out_file){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct Cyc___cycFILE*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct _fat_ptr _T8;char*_T9;unsigned _TA;char*_TB;struct _fat_ptr _TC;struct Cyc___cycFILE*_TD;int _TE;struct Cyc_String_pa_PrintArg_struct _TF;void**_T10;struct Cyc___cycFILE*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;char*_T14;struct _fat_ptr _T15;unsigned long _T16;struct Cyc___cycFILE*_T17;struct Cyc_String_pa_PrintArg_struct _T18;void**_T19;struct Cyc___cycFILE*_T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;_T0=file;_T1=_T0.curr;_T2=(char*)_T1;
# 623
if(_T2!=0)goto _TL66;_T3=Cyc_stderr;_T4=
_tag_fat("Internal error: copy_internal_file called with NULL\n",sizeof(char),53U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T3,_T4,_T5);
return 1;_TL66: _T6=file;_T7=
# 627
_tag_fat("r",sizeof(char),2U);_T8=_tag_fat("internal compiler file",sizeof(char),23U);{struct Cyc___cycFILE*file_f=Cyc_try_file_open(_T6,_T7,_T8);
if(file_f!=0)goto _TL68;return 1;_TL68: {
unsigned long n_read=1024U;
unsigned long n_written;
char buf[1024U];{unsigned _T1D=1024U;unsigned i;i=0;_TL6D: if(i < _T1D)goto _TL6B;else{goto _TL6C;}_TL6B: _TA=i;_T9=buf + _TA;*_T9='\000';i=i + 1;goto _TL6D;_TL6C:;}
_TL6E: if(n_read==1024U)goto _TL6F;else{goto _TL70;}
_TL6F: _TB=buf;_TC=_tag_fat(_TB,sizeof(char),1024U);_TD=file_f;n_read=Cyc_fread(_TC,1U,1024U,_TD);
if(n_read==1024U)goto _TL71;_TE=Cyc_feof(file_f);if(_TE)goto _TL71;else{goto _TL73;}
_TL73: Cyc_compile_failure=1;{struct Cyc_String_pa_PrintArg_struct _T1D;_T1D.tag=0;
_T1D.f1=file;_TF=_T1D;}{struct Cyc_String_pa_PrintArg_struct _T1D=_TF;void*_T1E[1];_T10=_T1E + 0;*_T10=& _T1D;_T11=Cyc_stderr;_T12=_tag_fat("\nError: problem copying %s\n",sizeof(char),28U);_T13=_tag_fat(_T1E,sizeof(void*),1);Cyc_fprintf(_T11,_T12,_T13);}
Cyc_fflush(Cyc_stderr);
return 1;_TL71: _T14=buf;_T15=
# 640
_tag_fat(_T14,sizeof(char),1024U);_T16=n_read;_T17=out_file;n_written=Cyc_fwrite(_T15,1U,_T16,_T17);
if(n_read==n_written)goto _TL74;
Cyc_compile_failure=1;{struct Cyc_String_pa_PrintArg_struct _T1D;_T1D.tag=0;
_T1D.f1=file;_T18=_T1D;}{struct Cyc_String_pa_PrintArg_struct _T1D=_T18;void*_T1E[1];_T19=_T1E + 0;*_T19=& _T1D;_T1A=Cyc_stderr;_T1B=_tag_fat("\nError: problem copying %s\n",sizeof(char),28U);_T1C=_tag_fat(_T1E,sizeof(void*),1);Cyc_fprintf(_T1A,_T1B,_T1C);}
Cyc_fflush(Cyc_stderr);
return 1;_TL74: goto _TL6E;_TL70:
# 648
 Cyc_fclose(file_f);
return 0;}}}
# 654
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles (void){int _T0;struct Cyc_List_List*_T1;void*_T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;struct Cyc_List_List*_T5;_T0=Cyc_save_c_r;
if(_T0)goto _TL76;else{goto _TL78;}
_TL78: _TL7C: if(Cyc_cfiles!=0)goto _TL7A;else{goto _TL7B;}
_TL7A: _T1=Cyc_cfiles;_T2=_T1->hd;_T3=(struct _fat_ptr*)_T2;_T4=*_T3;Cyc_remove_file(_T4);_T5=
# 657
_check_null(Cyc_cfiles);Cyc_cfiles=_T5->tl;goto _TL7C;_TL7B: goto _TL77;_TL76: _TL77:;}
# 660
static void Cyc_find_fail(struct _fat_ptr file){struct Cyc_Core_Failure_exn_struct*_T0;struct _fat_ptr _T1;struct Cyc_String_pa_PrintArg_struct _T2;void**_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;void*_T6;
Cyc_compile_failure=1;
Cyc_remove_cfiles();{struct Cyc_Core_Failure_exn_struct*_T7=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_T7->tag=Cyc_Core_Failure;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;
_T8.f1=file;_T2=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T2;void*_T9[1];_T3=_T9 + 0;*_T3=& _T8;_T4=_tag_fat("Error: can't find internal compiler file %s\n",sizeof(char),45U);_T5=_tag_fat(_T9,sizeof(void*),1);_T1=Cyc_aprintf(_T4,_T5);}_T7->f1=_T1;_T0=(struct Cyc_Core_Failure_exn_struct*)_T7;}_T6=(void*)_T0;_throw(_T6);}
# 665
static struct _fat_ptr Cyc_find_in_arch_path(struct _fat_ptr s){struct _fat_ptr _T0;char*_T1;unsigned _T2;struct _fat_ptr _T3;
struct _fat_ptr r=Cyc_Specsfile_find_in_arch_path(s);_T0=r;_T1=_T0.curr;_T2=(unsigned)_T1;
if(_T2)goto _TL7D;else{goto _TL7F;}_TL7F: Cyc_find_fail(s);goto _TL7E;_TL7D: _TL7E: _T3=r;
return _T3;}
# 670
static struct _fat_ptr Cyc_find_in_exec_path(struct _fat_ptr s){struct _fat_ptr _T0;char*_T1;unsigned _T2;struct _fat_ptr _T3;
struct _fat_ptr r=Cyc_Specsfile_find_in_exec_path(s);_T0=r;_T1=_T0.curr;_T2=(unsigned)_T1;
if(_T2)goto _TL80;else{goto _TL82;}_TL82: Cyc_find_fail(s);goto _TL81;_TL80: _TL81: _T3=r;
return _T3;}struct _tuple15{struct Cyc___cycFILE*f0;struct _fat_ptr f1;};
# 676
struct Cyc_List_List*Cyc_do_print(struct _tuple15*env,struct Cyc_List_List*tds){struct _tuple15*_T0;struct Cyc_Absynpp_Params _T1;enum Cyc_Flags_Cyclone_Passes _T2;int _T3;enum Cyc_Flags_Cyclone_Passes _T4;int _T5;int _T6;int _T7;struct Cyc___cycFILE*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;int _TB;struct Cyc_List_List*_TC;int _TD;struct Cyc_List_List*_TE;int _TF;struct Cyc_Absynpp_Params*_T10;struct Cyc_Absynpp_Params*_T11;struct Cyc_Absynpp_Params*_T12;struct Cyc_Absynpp_Params*_T13;struct Cyc_List_List*_T14;struct _fat_ptr _T15;struct Cyc___cycFILE*_T16;_T0=env;{struct _tuple15 _T17=*_T0;_T16=_T17.f0;_T15=_T17.f1;}{struct Cyc___cycFILE*out_file=_T16;struct _fat_ptr cfile=_T15;_T2=Cyc_Flags_stop_after_pass;_T3=(int)_T2;
# 679
if(_T3 >= 9)goto _TL83;_T1=Cyc_Absynpp_cyc_params_r;goto _TL84;_TL83: _T1=Cyc_Absynpp_c_params_r;_TL84: {struct Cyc_Absynpp_Params params_r=_T1;
params_r.expand_typedefs=!Cyc_Flags_noexpand_r;
params_r.to_VC=Cyc_Flags_tovc_r;
params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;
params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;
params_r.print_all_effects=Cyc_print_all_effects_r;_T4=Cyc_Flags_stop_after_pass;_T5=(int)_T4;
# 690
if(_T5 <= 6)goto _TL85;_T6=Cyc_nocyc_setjmp_r;
if(_T6)goto _TL87;else{goto _TL89;}
_TL89: _T7=Cyc_Flags_compile_for_boot;if(!_T7)goto _TL8A;_T8=out_file;_T9=
_tag_fat("#include <setjmp.h>\n",sizeof(char),21U);_TA=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T8,_T9,_TA);goto _TL8B;
_TL8A: _TB=Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file);if(!_TB)goto _TL8C;_TC=tds;return _TC;_TL8C: _TL8B: goto _TL88;_TL87: _TL88: _TD=
# 696
Cyc_copy_internal_file(Cyc_cyc_include,out_file);if(!_TD)goto _TL8E;_TE=tds;return _TE;_TL8E: goto _TL86;_TL85: _TL86: _TF=Cyc_pp_r;
# 698
if(!_TF)goto _TL90;_T10=& params_r;_T11=(struct Cyc_Absynpp_Params*)_T10;
Cyc_Absynpp_set_params(_T11);
Cyc_Absynpp_decllist2file(tds,out_file);goto _TL91;
# 702
_TL90: _T12=& params_r;_T13=(struct Cyc_Absynpp_Params*)_T12;Cyc_Absyndump_set_params(_T13);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);_TL91:
# 705
 Cyc_fflush(out_file);_T14=tds;
return _T14;}}}
# 710
static struct Cyc_List_List*Cyc_split_by_char(struct _fat_ptr s,char c){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct _fat_ptr _T3;unsigned char*_T4;char*_T5;struct Cyc_List_List*_T6;struct _fat_ptr*_T7;struct Cyc_List_List*_T8;struct _fat_ptr*_T9;struct _fat_ptr _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;unsigned char*_TD;struct _fat_ptr _TE;unsigned char*_TF;int _T10;int _T11;unsigned long _T12;struct _fat_ptr _T13;struct _fat_ptr _T14;struct _fat_ptr _T15;unsigned char*_T16;struct _fat_ptr _T17;unsigned char*_T18;int _T19;int _T1A;unsigned long _T1B;struct _fat_ptr _T1C;struct Cyc_List_List*_T1D;_T0=s;_T1=_T0.curr;_T2=(char*)_T1;
if(_T2!=0)goto _TL92;return 0;_TL92: {
struct Cyc_List_List*result=0;
unsigned long len=Cyc_strlen(s);
# 715
_TL94: if(len > 0U)goto _TL95;else{goto _TL96;}
_TL95:{struct _fat_ptr end=Cyc_strchr(s,c);_T3=end;_T4=_T3.curr;_T5=(char*)_T4;
if(_T5!=0)goto _TL97;{struct Cyc_List_List*_T1E=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1F=_cycalloc(sizeof(struct _fat_ptr));
*_T1F=s;_T7=(struct _fat_ptr*)_T1F;}_T1E->hd=_T7;_T1E->tl=result;_T6=(struct Cyc_List_List*)_T1E;}result=_T6;goto _TL96;
# 721
_TL97:{struct Cyc_List_List*_T1E=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1F=_cycalloc(sizeof(struct _fat_ptr));_TA=s;_TB=end;_TC=_TB;_TD=_TC.curr;_TE=s;_TF=_TE.curr;_T10=_TD - _TF;_T11=_T10 / sizeof(char);_T12=(unsigned long)_T11;_T13=Cyc_substring(_TA,0,_T12);*_T1F=_T13;_T9=(struct _fat_ptr*)_T1F;}_T1E->hd=_T9;_T1E->tl=result;_T8=(struct Cyc_List_List*)_T1E;}result=_T8;_T14=end;_T15=_T14;_T16=_T15.curr;_T17=s;_T18=_T17.curr;_T19=_T16 - _T18;_T1A=_T19 / sizeof(char);_T1B=(unsigned long)_T1A;
len=len - _T1B;_T1C=end;
s=_fat_ptr_plus(_T1C,sizeof(char),1);;}goto _TL94;_TL96: _T1D=
# 726
Cyc_List_imp_rev(result);return _T1D;}}
# 731
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _fat_ptr subdir){struct Cyc_List_List*_T0;struct _fat_ptr*_T1;struct Cyc_List_List*_T2;void*_T3;struct _fat_ptr*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;
# 733
struct Cyc_List_List*l=0;
_TL9C: if(dirs!=0)goto _TL9A;else{goto _TL9B;}
_TL9A:{struct Cyc_List_List*_TA=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TB=_cycalloc(sizeof(struct _fat_ptr));_T2=dirs;_T3=_T2->hd;_T4=(struct _fat_ptr*)_T3;_T5=*_T4;_T6=subdir;_T7=Cyc_Filename_concat(_T5,_T6);*_TB=_T7;_T1=(struct _fat_ptr*)_TB;}_TA->hd=_T1;_TA->tl=l;_T0=(struct Cyc_List_List*)_TA;}l=_T0;_T8=dirs;
# 734
dirs=_T8->tl;goto _TL9C;_TL9B:
# 736
 l=Cyc_List_imp_rev(l);_T9=l;
return _T9;}
# 741
static int Cyc_is_other_special(char c){char _T0;int _T1;_T0=c;_T1=(int)_T0;switch(_T1){case 92: goto _LL4;case 34: _LL4: goto _LL6;case 59: _LL6: goto _LL8;case 38: _LL8: goto _LLA;case 40: _LLA: goto _LLC;case 41: _LLC: goto _LLE;case 124: _LLE: goto _LL10;case 94: _LL10: goto _LL12;case 60: _LL12: goto _LL14;case 62: _LL14: goto _LL16;case 10: _LL16: goto _LL18;case 9: _LL18:
# 757
 return 1;default:
 return 0;};}
# 763
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;unsigned char*_T4;const char*_T5;int _T6;char _T7;int _T8;int _T9;struct _fat_ptr _TA;struct Cyc_List_List*_TB;struct _fat_ptr**_TC;struct _fat_ptr*_TD;struct _fat_ptr**_TE;struct _fat_ptr*_TF;struct _fat_ptr _T10;struct _fat_ptr**_T11;struct _fat_ptr*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;unsigned long _T15;int _T16;unsigned long _T17;unsigned long _T18;int _T19;unsigned long _T1A;struct _fat_ptr _T1B;unsigned long _T1C;char*_T1D;unsigned _T1E;char*_T1F;unsigned _T20;char*_T21;int _T22;unsigned long _T23;unsigned long _T24;struct _fat_ptr _T25;unsigned char*_T26;const char*_T27;int _T28;char _T29;int _T2A;int _T2B;struct _fat_ptr _T2C;int _T2D;int _T2E;char*_T2F;char*_T30;unsigned _T31;unsigned char*_T32;char*_T33;struct _fat_ptr _T34;int _T35;int _T36;char*_T37;char*_T38;unsigned _T39;unsigned char*_T3A;char*_T3B;struct _fat_ptr _T3C;
unsigned long len=Cyc_strlen(s);
# 767
int single_quotes=0;
int other_special=0;{
int i=0;_TLA1: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 < _T2)goto _TL9F;else{goto _TLA0;}
_TL9F: _T3=s;_T4=_T3.curr;_T5=(const char*)_T4;_T6=i;{char c=_T5[_T6];_T7=c;_T8=(int)_T7;
if(_T8!=39)goto _TLA2;single_quotes=single_quotes + 1;goto _TLA3;
_TLA2: _T9=Cyc_is_other_special(c);if(!_T9)goto _TLA4;other_special=other_special + 1;goto _TLA5;_TLA4: _TLA5: _TLA3:;}
# 769
i=i + 1;goto _TLA1;_TLA0:;}
# 776
if(single_quotes!=0)goto _TLA6;if(other_special!=0)goto _TLA6;_TA=s;
return _TA;_TLA6:
# 780
 if(single_quotes!=0)goto _TLA8;{struct _fat_ptr*_T3D[3];_TC=_T3D + 0;{struct _fat_ptr*_T3E=_cycalloc(sizeof(struct _fat_ptr));
*_T3E=_tag_fat("'",sizeof(char),2U);_TD=(struct _fat_ptr*)_T3E;}*_TC=_TD;_TE=_T3D + 1;{struct _fat_ptr*_T3E=_cycalloc(sizeof(struct _fat_ptr));_T10=s;*_T3E=_T10;_TF=(struct _fat_ptr*)_T3E;}*_TE=_TF;_T11=_T3D + 2;{struct _fat_ptr*_T3E=_cycalloc(sizeof(struct _fat_ptr));*_T3E=_tag_fat("'",sizeof(char),2U);_T12=(struct _fat_ptr*)_T3E;}*_T11=_T12;_T13=_tag_fat(_T3D,sizeof(struct _fat_ptr*),3);_TB=Cyc_List_list(_T13);}_T14=Cyc_strconcat_l(_TB);return _T14;_TLA8: _T15=len;_T16=single_quotes;_T17=(unsigned long)_T16;_T18=_T15 + _T17;_T19=other_special;_T1A=(unsigned long)_T19;{
# 784
unsigned long len2=_T18 + _T1A;_T1C=len2 + 1U;{unsigned _T3D=_T1C + 1U;_T1E=_check_times(_T3D,sizeof(char));{char*_T3E=_cycalloc_atomic(_T1E);{unsigned _T3F=len2 + 1U;unsigned i;i=0;_TLAD: if(i < _T3F)goto _TLAB;else{goto _TLAC;}_TLAB: _T20=i;_T1F=_T3E + _T20;
*_T1F='\000';i=i + 1;goto _TLAD;_TLAC: _T21=_T3E + _T3F;*_T21=0;}_T1D=(char*)_T3E;}_T1B=_tag_fat(_T1D,sizeof(char),_T3D);}{struct _fat_ptr s2=_T1B;
int i=0;
int j=0;
_TLB1: _T22=i;_T23=(unsigned long)_T22;_T24=len;if(_T23 < _T24)goto _TLAF;else{goto _TLB0;}
_TLAF: _T25=s;_T26=_T25.curr;_T27=(const char*)_T26;_T28=i;{char c=_T27[_T28];_T29=c;_T2A=(int)_T29;
if(_T2A==39)goto _TLB4;else{goto _TLB5;}_TLB5: _T2B=Cyc_is_other_special(c);if(_T2B)goto _TLB4;else{goto _TLB2;}
_TLB4: _T2C=s2;_T2D=j;j=_T2D + 1;_T2E=_T2D;{struct _fat_ptr _T3D=_fat_ptr_plus(_T2C,sizeof(char),_T2E);_T2F=_check_fat_subscript(_T3D,sizeof(char),0U);_T30=(char*)_T2F;{char _T3E=*_T30;char _T3F='\\';_T31=_get_fat_size(_T3D,sizeof(char));if(_T31!=1U)goto _TLB6;if(_T3E!=0)goto _TLB6;if(_T3F==0)goto _TLB6;_throw_arraybounds();goto _TLB7;_TLB6: _TLB7: _T32=_T3D.curr;_T33=(char*)_T32;*_T33=_T3F;}}goto _TLB3;_TLB2: _TLB3: _T34=s2;_T35=j;
j=_T35 + 1;_T36=_T35;{struct _fat_ptr _T3D=_fat_ptr_plus(_T34,sizeof(char),_T36);_T37=_check_fat_subscript(_T3D,sizeof(char),0U);_T38=(char*)_T37;{char _T3E=*_T38;char _T3F=c;_T39=_get_fat_size(_T3D,sizeof(char));if(_T39!=1U)goto _TLB8;if(_T3E!=0)goto _TLB8;if(_T3F==0)goto _TLB8;_throw_arraybounds();goto _TLB9;_TLB8: _TLB9: _T3A=_T3D.curr;_T3B=(char*)_T3A;*_T3B=_T3F;}}}
# 788
i=i + 1;goto _TLB1;_TLB0: _T3C=s2;
# 794
return _T3C;}}}
# 796
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){struct _fat_ptr*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;{struct _fat_ptr*_T3=_cycalloc(sizeof(struct _fat_ptr));_T1=sp;_T2=*_T1;
*_T3=Cyc_sh_escape_string(_T2);_T0=(struct _fat_ptr*)_T3;}return _T0;}
# 799
static struct _fat_ptr Cyc_get_cppargs_string (void){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct Cyc_List_List*_T3;struct _fat_ptr*_T4;struct _fat_ptr _T5;struct Cyc_List_List*(*_T6)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*);struct Cyc_List_List*(*_T7)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;
static struct _fat_ptr cppargs_string={(void*)0,(void*)0,(void*)(0 + 0)};_T0=cppargs_string;_T1=_T0.curr;_T2=(char*)_T1;
if(_T2!=0)goto _TLBA;{struct Cyc_List_List*_TB=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TC=_cycalloc(sizeof(struct _fat_ptr));_T5=
# 803
_tag_fat("",sizeof(char),1U);*_TC=_T5;_T4=(struct _fat_ptr*)_TC;}_TB->hd=_T4;_T7=Cyc_List_map;{
struct Cyc_List_List*(*_TC)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))_T7;_T6=_TC;}_T8=Cyc_List_rev(Cyc_cppargs);_TB->tl=_T6(Cyc_sh_escape_stringptr,_T8);_T3=(struct Cyc_List_List*)_TB;}_T9=
_tag_fat(" ",sizeof(char),2U);
# 802
cppargs_string=
Cyc_str_sepstr(_T3,_T9);goto _TLBB;_TLBA: _TLBB: _TA=cppargs_string;
# 806
return _TA;}
# 809
struct Cyc_List_List*Cyc_Cyclone_parse_file(struct _fat_ptr filename){struct _fat_ptr _T0;struct _fat_ptr _T1;struct _fat_ptr*_T2;int _T3;struct Cyc_String_pa_PrintArg_struct _T4;void**_T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;int _TC;struct Cyc___cycFILE*_TD;unsigned _TE;struct Cyc_List_List*_TF;struct _fat_ptr _T10;unsigned long _T11;struct Cyc_List_List*_T12;struct _fat_ptr*_T13;struct _fat_ptr _T14;void*_T15;void*_T16;unsigned _T17;struct Cyc_List_List*_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct _fat_ptr*_T1B;struct _fat_ptr _T1C;struct Cyc_List_List*(*_T1D)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1E)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T1F;struct _fat_ptr _T20;enum Cyc_Flags_Cyclone_Passes _T21;int _T22;struct _fat_ptr _T23;struct _fat_ptr _T24;struct Cyc_String_pa_PrintArg_struct _T25;struct _fat_ptr*_T26;void**_T27;struct _fat_ptr _T28;struct _fat_ptr _T29;struct _fat_ptr _T2A;struct Cyc_String_pa_PrintArg_struct _T2B;void**_T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;int _T2F;struct _fat_ptr _T30;struct Cyc_String_pa_PrintArg_struct _T31;struct _fat_ptr*_T32;void**_T33;struct _fat_ptr _T34;struct _fat_ptr _T35;struct _fat_ptr _T36;struct Cyc_String_pa_PrintArg_struct _T37;struct Cyc_String_pa_PrintArg_struct _T38;struct Cyc_String_pa_PrintArg_struct _T39;struct Cyc_String_pa_PrintArg_struct _T3A;struct Cyc_String_pa_PrintArg_struct _T3B;struct Cyc_String_pa_PrintArg_struct _T3C;void**_T3D;void**_T3E;void**_T3F;void**_T40;void**_T41;void**_T42;struct _fat_ptr _T43;struct _fat_ptr _T44;int _T45;struct Cyc_String_pa_PrintArg_struct _T46;void**_T47;struct Cyc___cycFILE*_T48;struct _fat_ptr _T49;struct _fat_ptr _T4A;struct _fat_ptr _T4B;char*_T4C;char*_T4D;const char*_T4E;int _T4F;struct Cyc___cycFILE*_T50;struct _fat_ptr _T51;struct _fat_ptr _T52;enum Cyc_Flags_Cyclone_Passes _T53;int _T54;enum Cyc_Flags_Cyclone_Passes _T55;int _T56;struct _fat_ptr _T57;struct _fat_ptr _T58;struct _fat_ptr _T59;int _T5A;enum Cyc_Flags_Cyclone_Passes _T5B;int _T5C;struct Cyc_List_List*(*_T5D)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*,struct Cyc_List_List*);struct _fat_ptr _T5E;struct Cyc_List_List*_T5F;struct Cyc___cycFILE*_T60;struct Cyc_List_List*_T61;struct Cyc_List_List*_T62;
struct _fat_ptr basename=Cyc_make_base_filename(filename,Cyc_output_file);_T0=basename;_T1=
_tag_fat(".cyp",sizeof(char),5U);{struct _fat_ptr preprocfile=Cyc_strconcat(_T0,_T1);
# 815
struct _fat_ptr*preprocfileptr;preprocfileptr=_cycalloc(sizeof(struct _fat_ptr));_T2=preprocfileptr;*_T2=preprocfile;_T3=Cyc_Flags_verbose;
# 817
if(!_T3)goto _TLBC;{struct Cyc_String_pa_PrintArg_struct _T63;_T63.tag=0;_T63.f1=filename;_T4=_T63;}{struct Cyc_String_pa_PrintArg_struct _T63=_T4;void*_T64[1];_T5=_T64 + 0;*_T5=& _T63;_T6=Cyc_stderr;_T7=_tag_fat("Compiling %s\n",sizeof(char),14U);_T8=_tag_fat(_T64,sizeof(void*),1);Cyc_fprintf(_T6,_T7,_T8);}goto _TLBD;_TLBC: _TLBD: _T9=filename;_TA=
# 820
_tag_fat("r",sizeof(char),2U);_TB=_tag_fat("input file",sizeof(char),11U);{struct Cyc___cycFILE*f0=Cyc_try_file_open(_T9,_TA,_TB);_TC=Cyc_compile_failure;
if(_TC)goto _TLC0;else{goto _TLC1;}_TLC1: _TD=f0;_TE=(unsigned)_TD;if(_TE)goto _TLBE;else{goto _TLC0;}
_TLC0: return 0;_TLBE:
 Cyc_fclose(f0);{
# 828
struct _fat_ptr cppargs_string=Cyc_get_cppargs_string();
# 836
struct Cyc_List_List*stdinc_dirs=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_TF=stdinc_dirs;_T10=
_tag_fat("include",sizeof(char),8U);stdinc_dirs=Cyc_add_subdir(_TF,_T10);_T11=
Cyc_strlen(Cyc_def_inc_path);if(_T11 <= 0U)goto _TLC2;{struct Cyc_List_List*_T63=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T64=_cycalloc(sizeof(struct _fat_ptr));
*_T64=Cyc_def_inc_path;_T13=(struct _fat_ptr*)_T64;}_T63->hd=_T13;_T63->tl=stdinc_dirs;_T12=(struct Cyc_List_List*)_T63;}stdinc_dirs=_T12;goto _TLC3;_TLC2: _TLC3: {
char*cyclone_include_path=getenv("CYCLONE_INCLUDE_PATH");
if(cyclone_include_path==0)goto _TLC4;{char*_T63=cyclone_include_path;_T15=(void*)_T63;_T16=(void*)_T63;_T17=_get_zero_arr_size_char(_T16,1U);_T14=_tag_fat(_T15,sizeof(char),_T17);}_T18=
Cyc_split_by_char(_T14,':');_T19=stdinc_dirs;stdinc_dirs=Cyc_List_append(_T18,_T19);goto _TLC5;_TLC4: _TLC5:{struct Cyc_List_List*_T63=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T64=_cycalloc(sizeof(struct _fat_ptr));_T1C=
# 844
_tag_fat("",sizeof(char),1U);*_T64=_T1C;_T1B=(struct _fat_ptr*)_T64;}_T63->hd=_T1B;_T1E=Cyc_List_map;{
struct Cyc_List_List*(*_T64)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))_T1E;_T1D=_T64;}_T1F=stdinc_dirs;_T63->tl=_T1D(Cyc_sh_escape_stringptr,_T1F);_T1A=(struct Cyc_List_List*)_T63;}_T20=
_tag_fat(" -I",sizeof(char),4U);{
# 843
struct _fat_ptr stdinc_string=
Cyc_str_sepstr(_T1A,_T20);
# 847
struct _fat_ptr ofile_string;_T21=Cyc_Flags_stop_after_pass;_T22=(int)_T21;
if(_T22 > 0)goto _TLC6;
if(Cyc_output_file!=0)goto _TLC8;_T23=_tag_fat("",sizeof(char),1U);goto _TLC9;_TLC8:{struct Cyc_String_pa_PrintArg_struct _T63;_T63.tag=0;_T26=Cyc_output_file;_T63.f1=*_T26;_T25=_T63;}{struct Cyc_String_pa_PrintArg_struct _T63=_T25;void*_T64[1];_T27=_T64 + 0;*_T27=& _T63;_T28=_tag_fat(" > %s",sizeof(char),6U);_T29=_tag_fat(_T64,sizeof(void*),1);_T24=Cyc_aprintf(_T28,_T29);}_T23=_T24;_TLC9: ofile_string=_T23;goto _TLC7;
# 851
_TLC6:{struct Cyc_String_pa_PrintArg_struct _T63;_T63.tag=0;_T63.f1=Cyc_sh_escape_string(preprocfile);_T2B=_T63;}{struct Cyc_String_pa_PrintArg_struct _T63=_T2B;void*_T64[1];_T2C=_T64 + 0;*_T2C=& _T63;_T2D=_tag_fat(" > %s",sizeof(char),6U);_T2E=_tag_fat(_T64,sizeof(void*),1);_T2A=Cyc_aprintf(_T2D,_T2E);}ofile_string=_T2A;_TLC7: {
# 853
struct _fat_ptr fixup_string;_T2F=Cyc_produce_dependencies;
if(!_T2F)goto _TLCA;
# 857
if(Cyc_dependencies_target!=0)goto _TLCC;
# 861
fixup_string=_tag_fat(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",sizeof(char),35U);goto _TLCD;
# 866
_TLCC:{struct Cyc_String_pa_PrintArg_struct _T63;_T63.tag=0;_T32=Cyc_dependencies_target;
_T63.f1=*_T32;_T31=_T63;}{struct Cyc_String_pa_PrintArg_struct _T63=_T31;void*_T64[1];_T33=_T64 + 0;*_T33=& _T63;_T34=
# 866
_tag_fat(" | sed 's/^.*\\.cyc\\.o:/%s:/'",sizeof(char),29U);_T35=_tag_fat(_T64,sizeof(void*),1);_T30=Cyc_aprintf(_T34,_T35);}fixup_string=_T30;_TLCD: goto _TLCB;
# 869
_TLCA: fixup_string=_tag_fat("",sizeof(char),1U);_TLCB:{struct Cyc_String_pa_PrintArg_struct _T63;_T63.tag=0;
# 872
_T63.f1=Cyc_cpp;_T37=_T63;}{struct Cyc_String_pa_PrintArg_struct _T63=_T37;{struct Cyc_String_pa_PrintArg_struct _T64;_T64.tag=0;_T64.f1=cppargs_string;_T38=_T64;}{struct Cyc_String_pa_PrintArg_struct _T64=_T38;{struct Cyc_String_pa_PrintArg_struct _T65;_T65.tag=0;_T65.f1=stdinc_string;_T39=_T65;}{struct Cyc_String_pa_PrintArg_struct _T65=_T39;{struct Cyc_String_pa_PrintArg_struct _T66;_T66.tag=0;
_T66.f1=Cyc_sh_escape_string(filename);_T3A=_T66;}{struct Cyc_String_pa_PrintArg_struct _T66=_T3A;{struct Cyc_String_pa_PrintArg_struct _T67;_T67.tag=0;_T67.f1=fixup_string;_T3B=_T67;}{struct Cyc_String_pa_PrintArg_struct _T67=_T3B;{struct Cyc_String_pa_PrintArg_struct _T68;_T68.tag=0;_T68.f1=ofile_string;_T3C=_T68;}{struct Cyc_String_pa_PrintArg_struct _T68=_T3C;void*_T69[6];_T3D=_T69 + 0;*_T3D=& _T63;_T3E=_T69 + 1;*_T3E=& _T64;_T3F=_T69 + 2;*_T3F=& _T65;_T40=_T69 + 3;*_T40=& _T66;_T41=_T69 + 4;*_T41=& _T67;_T42=_T69 + 5;*_T42=& _T68;_T43=
# 871
_tag_fat("%s %s%s %s%s%s",sizeof(char),15U);_T44=_tag_fat(_T69,sizeof(void*),6);_T36=Cyc_aprintf(_T43,_T44);}}}}}}{struct _fat_ptr cmd=_T36;_T45=Cyc_Flags_verbose;
# 875
if(!_T45)goto _TLCE;{struct Cyc_String_pa_PrintArg_struct _T63;_T63.tag=0;_T63.f1=cmd;_T46=_T63;}{struct Cyc_String_pa_PrintArg_struct _T63=_T46;void*_T64[1];_T47=_T64 + 0;*_T47=& _T63;_T48=Cyc_stderr;_T49=_tag_fat("%s\n",sizeof(char),4U);_T4A=_tag_fat(_T64,sizeof(void*),1);Cyc_fprintf(_T48,_T49,_T4A);}goto _TLCF;_TLCE: _TLCF: _T4B=cmd;_T4C=_untag_fat_ptr_check_bound(_T4B,sizeof(char),1U);_T4D=_check_null(_T4C);_T4E=(const char*)_T4D;_T4F=
system(_T4E);if(_T4F==0)goto _TLD0;
Cyc_compile_failure=1;_T50=Cyc_stderr;_T51=
_tag_fat("\nError: preprocessing\n",sizeof(char),23U);_T52=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T50,_T51,_T52);_T53=Cyc_Flags_stop_after_pass;_T54=(int)_T53;
if(_T54 <= 0)goto _TLD2;Cyc_remove_file(preprocfile);goto _TLD3;_TLD2: _TLD3:
 return 0;_TLD0: _T55=Cyc_Flags_stop_after_pass;_T56=(int)_T55;
# 882
if(_T56 > 0)goto _TLD4;return 0;_TLD4:
# 885
 Cyc_Warn_reset(preprocfile);_T57=preprocfile;_T58=
_tag_fat("r",sizeof(char),2U);_T59=_tag_fat("file",sizeof(char),5U);{struct Cyc___cycFILE*in_file=Cyc_try_file_open(_T57,_T58,_T59);
# 888
struct Cyc_List_List*tds=0;_T5A=Cyc_compile_failure;
# 891
if(_T5A)goto _TLD6;else{goto _TLD8;}_TLD8: _T5B=Cyc_Flags_stop_after_pass;_T5C=(int)_T5B;if(_T5C < 1)goto _TLD6;{
struct Cyc_List_List*(*_T63)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*,struct Cyc_List_List*))Cyc_do_stage;_T5D=_T63;}_T5E=_tag_fat("parsing",sizeof(char),8U);_T5F=tds;_T60=_check_null(in_file);_T61=tds;tds=_T5D(_T5E,_T5F,Cyc_do_parse,_T60,_T61);goto _TLD7;_TLD6: _TLD7: _T62=tds;
return _T62;}}}}}}}}}
# 896
static void Cyc_process_file(struct _fat_ptr filename){struct _fat_ptr _T0;struct _fat_ptr _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;unsigned char*_T4;char*_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;int _TC;int _TD;int _TE;int _TF;int _T10;enum Cyc_Flags_Cyclone_Passes _T11;int _T12;int _T13;struct Cyc_List_List*(*_T14)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _fat_ptr*,struct Cyc_List_List*),struct _fat_ptr*,struct Cyc_List_List*);struct _fat_ptr _T15;struct Cyc_List_List*_T16;struct _fat_ptr*_T17;struct _fat_ptr*_T18;int _T19;struct Cyc_List_List*(*_T1A)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*);struct _fat_ptr _T1B;struct Cyc_List_List*_T1C;int(*_T1D)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int);struct _fat_ptr _T1E;struct Cyc_List_List*_T1F;int _T20;enum Cyc_Flags_Cyclone_Passes _T21;int _T22;int(*_T23)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int);struct _fat_ptr _T24;struct Cyc_List_List*_T25;int _T26;enum Cyc_Flags_Cyclone_Passes _T27;int _T28;struct Cyc_List_List*(*_T29)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _fat_ptr _T2A;struct Cyc_List_List*_T2B;struct Cyc_Tcenv_Tenv*_T2C;struct Cyc_List_List*_T2D;int _T2E;enum Cyc_Flags_Cyclone_Passes _T2F;int _T30;struct Cyc_JumpAnalysis_Jump_Anal_Result*(*_T31)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(int,struct Cyc_List_List*),int,struct Cyc_JumpAnalysis_Jump_Anal_Result*);struct _fat_ptr _T32;struct Cyc_List_List*_T33;int _T34;enum Cyc_Flags_Cyclone_Passes _T35;int _T36;struct Cyc_List_List*(*_T37)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*);struct _fat_ptr _T38;struct Cyc_List_List*_T39;struct Cyc_JumpAnalysis_Jump_Anal_Result*_T3A;struct Cyc_List_List*_T3B;int _T3C;enum Cyc_Flags_Cyclone_Passes _T3D;int _T3E;int(*_T3F)(struct _fat_ptr,struct Cyc_List_List*,int(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,int);struct _fat_ptr _T40;struct Cyc_List_List*_T41;struct Cyc_JumpAnalysis_Jump_Anal_Result*_T42;int _T43;int _T44;enum Cyc_Flags_Cyclone_Passes _T45;int _T46;struct _fat_ptr _T47;struct _fat_ptr _T48;char*_T49;char*_T4A;void*_T4B;void*_T4C;unsigned _T4D;struct _fat_ptr _T4E;struct _fat_ptr _T4F;struct Cyc_Absynpp_Params*_T50;struct Cyc_Absynpp_Params*_T51;struct Cyc_Absynpp_Params*_T52;struct Cyc_Absynpp_Params*_T53;int _T54;int _T55;struct _fat_ptr _T56;char*_T57;const char*_T58;struct _fat_ptr _T59;struct _fat_ptr _T5A;struct _fat_ptr _T5B;struct _tuple14 _T5C;struct Cyc_List_List*(*_T5D)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple14*,struct Cyc_List_List*),struct _tuple14*,struct Cyc_List_List*);struct _fat_ptr _T5E;struct Cyc_List_List*_T5F;struct _tuple14*_T60;struct Cyc_List_List*_T61;int _T62;int _T63;int _T64;struct Cyc___cycFILE*_T65;struct _fat_ptr _T66;struct _fat_ptr _T67;int _T68;enum Cyc_Flags_Cyclone_Passes _T69;int _T6A;struct Cyc_List_List*(*_T6B)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*,struct Cyc_List_List*);struct _fat_ptr _T6C;struct Cyc_List_List*_T6D;struct Cyc_JumpAnalysis_Jump_Anal_Result*_T6E;struct Cyc_Hashtable_Table*_T6F;struct Cyc_List_List*_T70;int _T71;enum Cyc_Flags_Cyclone_Passes _T72;int _T73;struct Cyc_List_List*(*_T74)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*);struct _fat_ptr _T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;int _T78;int _T79;struct Cyc_List_List*(*_T7A)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*);struct _fat_ptr _T7B;struct Cyc_List_List*_T7C;struct Cyc_List_List*_T7D;int _T7E;enum Cyc_Flags_Cyclone_Passes _T7F;int _T80;struct Cyc_List_List*(*_T81)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*);struct _fat_ptr _T82;struct Cyc_List_List*_T83;struct Cyc_List_List*_T84;int _T85;enum Cyc_Flags_Cyclone_Passes _T86;int _T87;int _T88;enum Cyc_Flags_Cyclone_Passes _T89;int _T8A;struct Cyc_List_List*(*_T8B)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*);struct _fat_ptr _T8C;struct Cyc_List_List*_T8D;struct Cyc_List_List*_T8E;int _T8F;enum Cyc_Flags_Cyclone_Passes _T90;int _T91;struct Cyc_List_List*(*_T92)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*);struct _fat_ptr _T93;struct Cyc_List_List*_T94;struct Cyc_List_List*_T95;int _T96;enum Cyc_Flags_Cyclone_Passes _T97;int _T98;struct Cyc_List_List*(*_T99)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*);struct _fat_ptr _T9A;struct Cyc_List_List*_T9B;struct Cyc_List_List*_T9C;int _T9D;int _T9E;int _T9F;int _TA0;enum Cyc_Flags_Cyclone_Passes _TA1;int _TA2;struct Cyc_String_pa_PrintArg_struct _TA3;struct _fat_ptr _TA4;void*_TA5;void*_TA6;unsigned _TA7;struct Cyc_String_pa_PrintArg_struct _TA8;struct _fat_ptr _TA9;void*_TAA;void*_TAB;unsigned _TAC;struct Cyc_Int_pa_PrintArg_struct _TAD;int _TAE;void**_TAF;void**_TB0;void**_TB1;struct Cyc___cycFILE*_TB2;struct _fat_ptr _TB3;struct _fat_ptr _TB4;int _TB5;int _TB6;enum Cyc_Flags_Cyclone_Passes _TB7;int _TB8;enum Cyc_Flags_Cyclone_Passes _TB9;int _TBA;struct _fat_ptr*_TBB;struct _fat_ptr _TBC;struct _fat_ptr _TBD;struct _fat_ptr _TBE;enum Cyc_Flags_Cyclone_Passes _TBF;int _TC0;struct _fat_ptr*_TC1;struct _fat_ptr _TC2;struct _fat_ptr _TC3;struct _fat_ptr _TC4;struct _fat_ptr _TC5;struct _fat_ptr _TC6;struct _fat_ptr _TC7;int _TC8;struct Cyc___cycFILE*_TC9;unsigned _TCA;int _TCB;struct _tuple15*_TCC;struct _tuple15*_TCD;struct Cyc_List_List*(*_TCE)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple15*,struct Cyc_List_List*),struct _tuple15*,struct Cyc_List_List*);struct _fat_ptr _TCF;struct Cyc_List_List*_TD0;struct _tuple15*_TD1;struct Cyc_List_List*_TD2;struct Cyc_List_List*_TD3;struct _fat_ptr*_TD4;
struct _fat_ptr basename=Cyc_make_base_filename(filename,Cyc_output_file);_T0=basename;_T1=
_tag_fat(".cyp",sizeof(char),5U);{struct _fat_ptr preprocfile=Cyc_strconcat(_T0,_T1);_T3=Cyc_specified_interface;_T4=_T3.curr;_T5=(char*)_T4;
if(_T5==0)goto _TLD9;_T2=Cyc_specified_interface;goto _TLDA;_TLD9: _T6=basename;_T7=
_tag_fat(".cyci",sizeof(char),6U);_T2=Cyc_strconcat(_T6,_T7);_TLDA: {
# 899
struct _fat_ptr interfacefile=_T2;_T8=basename;_T9=
# 901
_tag_fat(".cycio",sizeof(char),7U);{struct _fat_ptr interfaceobjfile=Cyc_strconcat(_T8,_T9);_TA=
Cyc_make_base_filename(filename,0);_TB=_tag_fat(".c",sizeof(char),3U);{struct _fat_ptr cfile=Cyc_strconcat(_TA,_TB);
# 904
struct Cyc_List_List*tds=Cyc_Cyclone_parse_file(filename);_TC=Cyc_compile_failure;
# 906
if(_TC)goto _TLDB;else{goto _TLDD;}_TLDD: _TD=Cyc_Flags_tags;if(!_TD)goto _TLDB;
Cyc_Interproc_dump_tags(filename,tds);
Cyc_remove_file(preprocfile);
return;_TLDB: _TE=Cyc_compile_failure;
# 913
if(_TE)goto _TLDE;else{goto _TLE0;}_TLE0: _TF=Cyc_Flags_porting_c_code;if(!_TF)goto _TLDE;
Cyc_Port_port(tds);
Cyc_remove_file(preprocfile);
return;_TLDE: _T10=Cyc_compile_failure;
# 921
if(_T10)goto _TLE1;else{goto _TLE3;}_TLE3: _T11=Cyc_Flags_stop_after_pass;_T12=(int)_T11;if(_T12 < 2)goto _TLE1;_T13=Cyc_Flags_interproc;
if(!_T13)goto _TLE4;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _fat_ptr*,struct Cyc_List_List*),struct _fat_ptr*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _fat_ptr*,struct Cyc_List_List*),struct _fat_ptr*,struct Cyc_List_List*))Cyc_do_stage;_T14=_TD5;}_T15=_tag_fat("interproc_binding",sizeof(char),18U);_T16=tds;_T17=& filename;_T18=(struct _fat_ptr*)_T17;tds=_T14(_T15,_T16,Cyc_do_interproc_binding,_T18,0);goto _TLE5;
# 925
_TLE4: _T19=Cyc_Flags_resolve;if(!_T19)goto _TLE6;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;_T1A=_TD5;}_T1B=_tag_fat("interproc_resolution",sizeof(char),21U);_T1C=tds;tds=_T1A(_T1B,_T1C,Cyc_do_interproc_resolution,1,0);goto _TLE7;
# 929
_TLE6:{int(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int)=(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage;_T1D=_TD5;}_T1E=_tag_fat("binding",sizeof(char),8U);_T1F=tds;_T1D(_T1E,_T1F,Cyc_do_binding,1,1);_TLE7: _TLE5: goto _TLE2;_TLE1: _TLE2: {
# 933
struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables=0;
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_tc_init();_T20=Cyc_compile_failure;
# 937
if(_T20)goto _TLE8;else{goto _TLEA;}_TLEA: _T21=Cyc_Flags_stop_after_pass;_T22=(int)_T21;if(_T22 < 3)goto _TLE8;{
int(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int)=(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage;_T23=_TD5;}_T24=_tag_fat("current region resolution",sizeof(char),26U);_T25=tds;_T23(_T24,_T25,Cyc_do_adjust,1,1);goto _TLE9;_TLE8: _TLE9: _T26=Cyc_compile_failure;
# 941
if(_T26)goto _TLEB;else{goto _TLED;}_TLED: _T27=Cyc_Flags_stop_after_pass;_T28=(int)_T27;if(_T28 < 4)goto _TLEB;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*))Cyc_do_stage;_T29=_TD5;}_T2A=_tag_fat("type checking",sizeof(char),14U);_T2B=tds;_T2C=te;_T2D=tds;tds=_T29(_T2A,_T2B,Cyc_do_typecheck,_T2C,_T2D);goto _TLEC;_TLEB: _TLEC: _T2E=Cyc_compile_failure;
# 945
if(_T2E)goto _TLEE;else{goto _TLF0;}_TLF0: _T2F=Cyc_Flags_stop_after_pass;_T30=(int)_T2F;if(_T30 < 5)goto _TLEE;{
struct Cyc_JumpAnalysis_Jump_Anal_Result*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(int,struct Cyc_List_List*),int,struct Cyc_JumpAnalysis_Jump_Anal_Result*)=(struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(int,struct Cyc_List_List*),int,struct Cyc_JumpAnalysis_Jump_Anal_Result*))Cyc_do_stage;_T31=_TD5;}_T32=_tag_fat("jump checking",sizeof(char),14U);_T33=tds;jump_tables=_T31(_T32,_T33,Cyc_do_jumpanalysis,1,0);goto _TLEF;_TLEE: _TLEF: _T34=Cyc_compile_failure;
# 949
if(_T34)goto _TLF1;else{goto _TLF3;}_TLF3: _T35=Cyc_Flags_stop_after_pass;_T36=(int)_T35;if(_T36 < 6)goto _TLF1;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*))Cyc_do_stage;_T37=_TD5;}_T38=_tag_fat("control-flow checking",sizeof(char),22U);_T39=tds;_T3A=_check_null(jump_tables);_T3B=tds;tds=_T37(_T38,_T39,Cyc_do_cfcheck,_T3A,_T3B);goto _TLF2;_TLF1: _TLF2: _T3C=Cyc_compile_failure;
# 953
if(_T3C)goto _TLF4;else{goto _TLF6;}_TLF6: _T3D=Cyc_Flags_stop_after_pass;_T3E=(int)_T3D;if(_T3E < 8)goto _TLF4;{
int(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,int(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,int)=(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,int))Cyc_do_stage;_T3F=_TD5;}_T40=_tag_fat("check insertion",sizeof(char),16U);_T41=tds;_T42=_check_null(jump_tables);_T3F(_T40,_T41,Cyc_do_insert_checks,_T42,1);goto _TLF5;_TLF4: _TLF5: _T43=Cyc_compile_failure;
# 957
if(_T43)goto _TLF7;else{goto _TLF9;}_TLF9: _T44=Cyc_generate_interface_r;if(!_T44)goto _TLF7;_T45=Cyc_Flags_stop_after_pass;_T46=(int)_T45;if(_T46 < 9)goto _TLF7;_T48=interfacefile;_T49=_untag_fat_ptr_check_bound(_T48,sizeof(char),1U);_T4A=_check_null(_T49);{const char*_TD5=(const char*)_T4A;_T4B=(void*)_TD5;_T4C=(void*)_TD5;_T4D=_get_zero_arr_size_char(_T4C,1U);_T47=_tag_fat(_T4B,sizeof(char),_T4D);}_T4E=
# 959
_tag_fat("w",sizeof(char),2U);_T4F=_tag_fat("interface file",sizeof(char),15U);{struct Cyc___cycFILE*inter_file=Cyc_try_file_open(_T47,_T4E,_T4F);
if(inter_file!=0)goto _TLFA;
Cyc_compile_failure=1;goto _TLFB;
# 963
_TLFA: _T50=& Cyc_Absynpp_cyci_params_r;_T51=(struct Cyc_Absynpp_Params*)_T50;Cyc_Absyndump_set_params(_T51);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);_T52=& Cyc_Absynpp_tc_params_r;_T53=(struct Cyc_Absynpp_Params*)_T52;
Cyc_Absynpp_set_params(_T53);_TLFB: _T54=Cyc_compile_failure;
# 970
if(_T54)goto _TLFC;else{goto _TLFE;}_TLFE: _T55=Cyc_ic_r;if(!_T55)goto _TLFC;_T56=interfacefile;_T57=_untag_fat_ptr(_T56,sizeof(char),1U);_T58=(const char*)_T57;{
struct Cyc___cycFILE*inter_file=Cyc_fopen(_T58,"r");_T59=interfaceobjfile;_T5A=
_tag_fat("w",sizeof(char),2U);_T5B=_tag_fat("interface object file",sizeof(char),22U);{struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open(_T59,_T5A,_T5B);
if(inter_objfile!=0)goto _TLFF;
Cyc_compile_failure=1;goto _TL100;
# 976
_TLFF: Cyc_Warn_reset(interfacefile);{struct _tuple14 _TD5;
_TD5.f0=te;_TD5.f1=inter_file;_TD5.f2=inter_objfile;_T5C=_TD5;}{struct _tuple14 int_env=_T5C;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple14*,struct Cyc_List_List*),struct _tuple14*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple14*,struct Cyc_List_List*),struct _tuple14*,struct Cyc_List_List*))Cyc_do_stage;_T5D=_TD5;}_T5E=_tag_fat("interface checking",sizeof(char),19U);_T5F=tds;_T60=& int_env;_T61=tds;tds=_T5D(_T5E,_T5F,Cyc_do_interface,_T60,_T61);
if(inter_file==0)goto _TL101;
Cyc_file_close(inter_file);goto _TL102;_TL101: _TL102:
 Cyc_file_close(inter_objfile);}_TL100:;}}goto _TLFD;_TLFC: _TLFD:;}goto _TLF8;_TLF7: _TLF8: _T62=Cyc_Flags_interproc;
# 985
if(_T62)goto _TL105;else{goto _TL106;}_TL106: _T63=Cyc_Flags_resolve;if(_T63)goto _TL105;else{goto _TL103;}
_TL105: Cyc_BansheeIf_discharge_implications();_T64=
Cyc_Warn_error_p();if(!_T64)goto _TL107;
Cyc_compile_failure=1;_T65=Cyc_stderr;_T66=
_tag_fat("\nCOMPILATION FAILED!\n",sizeof(char),22U);_T67=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T65,_T66,_T67);
Cyc_fflush(Cyc_stderr);goto _TL108;_TL107: _TL108: goto _TL104;_TL103: _TL104: _T68=Cyc_compile_failure;
# 995
if(_T68)goto _TL109;else{goto _TL10B;}_TL10B: _T69=Cyc_Flags_stop_after_pass;_T6A=(int)_T69;if(_T6A < 9)goto _TL109;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*,struct Cyc_List_List*))Cyc_do_stage;_T6B=_TD5;}_T6C=_tag_fat("translation to C",sizeof(char),17U);_T6D=tds;_T6E=_check_null(jump_tables);_T6F=_T6E->pop_tables;_T70=tds;tds=_T6B(_T6C,_T6D,Cyc_do_translate,_T6F,_T70);goto _TL10A;_TL109: _TL10A: _T71=Cyc_compile_failure;
# 999
if(_T71)goto _TL10C;else{goto _TL10E;}_TL10E: _T72=Cyc_Flags_stop_after_pass;_T73=(int)_T72;if(_T73 < 10)goto _TL10C;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;_T74=_TD5;}_T75=_tag_fat("aggregate removal",sizeof(char),18U);_T76=tds;_T77=tds;tds=_T74(_T75,_T76,Cyc_do_removeaggrs,1,_T77);goto _TL10D;_TL10C: _TL10D: _T78=Cyc_compile_failure;
# 1003
if(_T78)goto _TL10F;else{goto _TL111;}_TL111: _T79=Cyc_nolower_r;if(_T79)goto _TL10F;else{goto _TL112;}
_TL112:{struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;_T7A=_TD5;}_T7B=_tag_fat("lowering C code",sizeof(char),16U);_T7C=tds;_T7D=tds;tds=_T7A(_T7B,_T7C,Cyc_do_lower,1,_T7D);goto _TL110;_TL10F: _TL110: _T7E=Cyc_compile_failure;
# 1007
if(_T7E)goto _TL113;else{goto _TL115;}_TL115: _T7F=Cyc_Flags_stop_after_pass;_T80=(int)_T7F;if(_T80 < 11)goto _TL113;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;_T81=_TD5;}_T82=_tag_fat("unused label removal",sizeof(char),21U);_T83=tds;_T84=tds;tds=_T81(_T82,_T83,Cyc_do_removelabs,1,_T84);goto _TL114;_TL113: _TL114: _T85=Cyc_compile_failure;
# 1013
if(_T85)goto _TL116;else{goto _TL118;}_TL118: _T86=Cyc_Flags_stop_after_pass;_T87=(int)_T86;if(_T87 < 9)goto _TL116;
Cyc_Toc_finish();goto _TL117;_TL116: _TL117: _T88=Cyc_compile_failure;
# 1016
if(_T88)goto _TL119;else{goto _TL11B;}_TL11B: _T89=Cyc_Flags_stop_after_pass;_T8A=(int)_T89;if(_T8A < 14)goto _TL119;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;_T8B=_TD5;}_T8C=_tag_fat("low treeshaking",sizeof(char),16U);_T8D=tds;_T8E=tds;tds=_T8B(_T8C,_T8D,Cyc_do_lowtreeshake,1,_T8E);goto _TL11A;_TL119: _TL11A: _T8F=Cyc_compile_failure;
# 1019
if(_T8F)goto _TL11C;else{goto _TL11E;}_TL11E: _T90=Cyc_Flags_stop_after_pass;_T91=(int)_T90;if(_T91 < 12)goto _TL11C;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;_T92=_TD5;}_T93=_tag_fat("renaming temporaries",sizeof(char),21U);_T94=tds;_T95=tds;tds=_T92(_T93,_T94,Cyc_do_temprename,1,_T95);goto _TL11D;_TL11C: _TL11D: _T96=Cyc_compile_failure;
# 1022
if(_T96)goto _TL11F;else{goto _TL121;}_TL121: _T97=Cyc_Flags_stop_after_pass;_T98=(int)_T97;if(_T98 < 13)goto _TL11F;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;_T99=_TD5;}_T9A=_tag_fat("removing casts",sizeof(char),15U);_T9B=tds;_T9C=tds;tds=_T99(_T9A,_T9B,Cyc_do_castremove,1,_T9C);goto _TL120;_TL11F: _TL120: _T9D=Cyc_compile_failure;
# 1028
if(_T9D)goto _TL122;else{goto _TL124;}_TL124: _T9E=Cyc_Flags_interproc;if(!_T9E)goto _TL122;_T9F=Cyc_Flags_no_merge;if(!_T9F)goto _TL122;
# 1031
Cyc_Interproc_emit_decls(filename,tds);goto _TL123;_TL122: _TL123: _TA0=Cyc_compile_failure;
# 1033
if(_TA0)goto _TL125;else{goto _TL127;}
_TL127: Cyc_Warn_flush_warnings();goto _TL126;_TL125: _TL126: _TA1=Cyc_Flags_stop_after_pass;_TA2=(int)_TA1;
if(_TA2 <= 0)goto _TL128;
Cyc_remove_file(preprocfile);goto _TL129;_TL128: _TL129:
 if(Cyc_uncaught_exn==0)goto _TL12A;{
struct _tuple13*_TD5=Cyc_uncaught_exn;int _TD6;const char*_TD7;void*_TD8;{struct _tuple13 _TD9=*_TD5;_TD8=_TD9.f0;_TD7=_TD9.f1;_TD6=_TD9.f2;}{void*e=_TD8;const char*fn=_TD7;int line=_TD6;{struct Cyc_String_pa_PrintArg_struct _TD9;_TD9.tag=0;{const char*_TDA=
# 1040
Cyc_Core_get_exn_name(e);_TA5=(void*)_TDA;_TA6=(void*)_TDA;_TA7=_get_zero_arr_size_char(_TA6,1U);_TA4=_tag_fat(_TA5,sizeof(char),_TA7);}_TD9.f1=_TA4;_TA3=_TD9;}{struct Cyc_String_pa_PrintArg_struct _TD9=_TA3;{struct Cyc_String_pa_PrintArg_struct _TDA;_TDA.tag=0;{const char*_TDB=fn;_TAA=(void*)_TDB;_TAB=(void*)_TDB;_TAC=_get_zero_arr_size_char(_TAB,1U);_TA9=_tag_fat(_TAA,sizeof(char),_TAC);}_TDA.f1=_TA9;_TA8=_TDA;}{struct Cyc_String_pa_PrintArg_struct _TDA=_TA8;{struct Cyc_Int_pa_PrintArg_struct _TDB;_TDB.tag=1;_TAE=line;_TDB.f1=(unsigned long)_TAE;_TAD=_TDB;}{struct Cyc_Int_pa_PrintArg_struct _TDB=_TAD;void*_TDC[3];_TAF=_TDC + 0;*_TAF=& _TD9;_TB0=_TDC + 1;*_TB0=& _TDA;_TB1=_TDC + 2;*_TB1=& _TDB;_TB2=Cyc_stderr;_TB3=
# 1039
_tag_fat("uncaught exception: %s from %s, line %d\n",sizeof(char),41U);_TB4=_tag_fat(_TDC,sizeof(void*),3);Cyc_fprintf(_TB2,_TB3,_TB4);}}}
# 1041
Cyc_fflush(Cyc_stderr);
return;}}_TL12A: _TB5=Cyc_Flags_interproc;
# 1045
if(!_TB5)goto _TL12C;_TB6=Cyc_Flags_resolve;if(_TB6)goto _TL12C;else{goto _TL12E;}
_TL12E: return;_TL12C: {
# 1048
struct Cyc___cycFILE*out_file;_TB7=Cyc_Flags_stop_after_pass;_TB8=(int)_TB7;
if(_TB8 > 0)goto _TL12F;
return;_TL12F: _TB9=Cyc_Flags_stop_after_pass;_TBA=(int)_TB9;
# 1052
if(_TBA > 6)goto _TL131;
if(Cyc_output_file==0)goto _TL133;_TBB=Cyc_output_file;_TBC=*_TBB;_TBD=
_tag_fat("w",sizeof(char),2U);_TBE=_tag_fat("output file",sizeof(char),12U);out_file=Cyc_try_file_open(_TBC,_TBD,_TBE);goto _TL134;
# 1056
_TL133: out_file=Cyc_stdout;_TL134: goto _TL132;
_TL131: _TBF=Cyc_Flags_stop_after_pass;_TC0=(int)_TBF;if(_TC0 >= 15)goto _TL135;if(Cyc_output_file==0)goto _TL135;_TC1=Cyc_output_file;_TC2=*_TC1;_TC3=
_tag_fat("w",sizeof(char),2U);_TC4=_tag_fat("output file",sizeof(char),12U);out_file=Cyc_try_file_open(_TC2,_TC3,_TC4);goto _TL136;
# 1060
_TL135: _TC5=cfile;_TC6=_tag_fat("w",sizeof(char),2U);_TC7=_tag_fat("output file",sizeof(char),12U);out_file=Cyc_try_file_open(_TC5,_TC6,_TC7);_TL136: _TL132: _TC8=Cyc_compile_failure;
# 1062
if(_TC8)goto _TL139;else{goto _TL13A;}_TL13A: _TC9=out_file;_TCA=(unsigned)_TC9;if(_TCA)goto _TL137;else{goto _TL139;}_TL139: return;_TL137: _TCB=Cyc_noprint_r;
# 1064
if(_TCB)goto _TL13B;else{goto _TL13D;}
_TL13D:{struct _tuple15*env;env=_cycalloc(sizeof(struct _tuple15));_TCC=env;_TCC->f0=out_file;_TCD=env;_TCD->f1=cfile;{
struct Cyc_List_List*(*_TD5)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple15*,struct Cyc_List_List*),struct _tuple15*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple15*,struct Cyc_List_List*),struct _tuple15*,struct Cyc_List_List*))Cyc_do_stage;_TCE=_TD5;}_TCF=_tag_fat("printing",sizeof(char),9U);_TD0=tds;_TD1=env;_TD2=tds;tds=_TCE(_TCF,_TD0,Cyc_do_print,_TD1,_TD2);
Cyc_file_close(out_file);{struct Cyc_List_List*_TD5=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TD6=_cycalloc(sizeof(struct _fat_ptr));
*_TD6=Cyc_strdup(cfile);_TD4=(struct _fat_ptr*)_TD6;}_TD5->hd=_TD4;_TD5->tl=Cyc_cfiles;_TD3=(struct Cyc_List_List*)_TD5;}Cyc_cfiles=_TD3;}goto _TL13C;_TL13B: _TL13C:;}}}}}}}static char _TmpGC[8U]="<final>";
# 1072
static struct _fat_ptr Cyc_final_str={_TmpGC,_TmpGC,_TmpGC + 8U};
static struct _fat_ptr*Cyc_final_strptr=(struct _fat_ptr*)& Cyc_final_str;
# 1075
static struct Cyc_Interface_I*Cyc_read_cycio(struct _fat_ptr*n){struct Cyc_Interface_I*_T0;struct _handler_cons*_T1;int*_T2;int _T3;struct _fat_ptr*_T4;struct _fat_ptr _T5;void*_T6;struct Cyc_Core_Invalid_argument_exn_struct*_T7;char*_T8;char*_T9;struct _fat_ptr*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Interface_I*_T10;
if(n!=Cyc_final_strptr)goto _TL13E;_T0=
Cyc_Interface_final();return _T0;_TL13E: {
struct _fat_ptr basename;{struct _handler_cons _T11;_T1=& _T11;_push_handler(_T1);{int _T12=0;_T2=_T11.handler;_T3=setjmp(_T2);if(!_T3)goto _TL140;_T12=1;goto _TL141;_TL140: _TL141: if(_T12)goto _TL142;else{goto _TL144;}_TL144: _T4=n;_T5=*_T4;
basename=Cyc_Filename_chop_extension(_T5);_pop_handler();goto _TL143;_TL142: _T6=Cyc_Core_get_exn_thrown();{void*_T13=(void*)_T6;void*_T14;_T7=(struct Cyc_Core_Invalid_argument_exn_struct*)_T13;_T8=_T7->tag;_T9=Cyc_Core_Invalid_argument;if(_T8!=_T9)goto _TL145;_TA=n;
basename=*_TA;goto _LL0;_TL145: _T14=_T13;{void*exn=_T14;_rethrow(exn);}_LL0:;}_TL143:;}}_TB=basename;_TC=
# 1082
_tag_fat(".cycio",sizeof(char),7U);{struct _fat_ptr nf=Cyc_strconcat(_TB,_TC);_TD=nf;_TE=
_tag_fat("rb",sizeof(char),3U);_TF=_tag_fat("interface object file",sizeof(char),22U);{struct Cyc___cycFILE*f=Cyc_try_file_open(_TD,_TE,_TF);
if(f!=0)goto _TL147;
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);goto _TL148;_TL147: _TL148:
# 1089
 Cyc_Warn_reset(nf);{
struct Cyc_Interface_I*i=Cyc_Interface_load(f);
Cyc_file_close(f);_T10=i;
return _T10;}}}}}
# 1095
static int Cyc_is_cfile(struct _fat_ptr*n){struct _fat_ptr*_T0;struct _fat_ptr _T1;char*_T2;const char*_T3;char _T4;int _T5;int _T6;_T0=n;_T1=*_T0;_T2=_check_fat_subscript(_T1,sizeof(char),0);_T3=(const char*)_T2;_T4=*_T3;_T5=(int)_T4;_T6=_T5!=45;
return _T6;}
# 1099
static void Cyc_set_max_vc_summary(int n){int _T0;_T0=n;
Cyc_Flags_max_vc_summary=(unsigned)_T0;}
# 1102
static void Cyc_set_max_vc_paths(int n){int _T0;_T0=n;
Cyc_Flags_max_vc_paths=(unsigned)_T0;}
# 1105
static void Cyc_set_max_vc_term_size(int n){int _T0;_T0=n;
Cyc_Flags_max_vc_term_size=(unsigned)_T0;}
# 1109
extern void GC_blacklist_warn_clear (void);struct _tuple16{struct _fat_ptr f0;int f1;struct _fat_ptr f2;void*f3;struct _fat_ptr f4;};
# 1119
void Cyc_print_options (void);
# 1127
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3,& Cyc_Flags_verbose};static char _TmpGD[3U]="-v";static char _TmpGE[1U]="";static char _TmpGF[35U]="print compilation stages verbosely";struct _tuple16 Cyc_opt1_tuple={.f0={_TmpGD,_TmpGD,_TmpGD + 3U},.f1=0,.f2={_TmpGE,_TmpGE,_TmpGE + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt1_spec),.f4={_TmpGF,_TmpGF,_TmpGF + 35U}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 1129
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0,Cyc_print_version};static char _TmpG10[10U]="--version";static char _TmpG11[1U]="";static char _TmpG12[35U]="Print version information and exit";struct _tuple16 Cyc_opt2_tuple={.f0={_TmpG10,_TmpG10,_TmpG10 + 10U},.f1=0,.f2={_TmpG11,_TmpG11,_TmpG11 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt2_spec),.f4={_TmpG12,_TmpG12,_TmpG12 + 35U}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 1132
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5,Cyc_set_output_file};static char _TmpG13[3U]="-o";static char _TmpG14[8U]=" <file>";static char _TmpG15[35U]="Set the output file name to <file>";struct _tuple16 Cyc_opt3_tuple={.f0={_TmpG13,_TmpG13,_TmpG13 + 3U},.f1=0,.f2={_TmpG14,_TmpG14,_TmpG14 + 8U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt3_spec),.f4={_TmpG15,_TmpG15,_TmpG15 + 35U}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 1135
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1,Cyc_add_cpparg};static char _TmpG16[3U]="-D";static char _TmpG17[17U]="<name>[=<value>]";static char _TmpG18[32U]="Pass definition to preprocessor";struct _tuple16 Cyc_opt4_tuple={.f0={_TmpG16,_TmpG16,_TmpG16 + 3U},.f1=1,.f2={_TmpG17,_TmpG17,_TmpG17 + 17U},.f3=(void*)((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)& Cyc_opt4_spec),.f4={_TmpG18,_TmpG18,_TmpG18 + 32U}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 1138
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1,Cyc_Specsfile_add_cyclone_exec_path};static char _TmpG19[3U]="-B";static char _TmpG1A[7U]="<file>";static char _TmpG1B[60U]="Add to the list of directories to search for compiler files";struct _tuple16 Cyc_opt5_tuple={.f0={_TmpG19,_TmpG19,_TmpG19 + 3U},.f1=1,.f2={_TmpG1A,_TmpG1A,_TmpG1A + 7U},.f3=(void*)((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)& Cyc_opt5_spec),.f4={_TmpG1B,_TmpG1B,_TmpG1B + 60U}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 1141
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1,Cyc_add_cpparg};static char _TmpG1C[3U]="-I";static char _TmpG1D[6U]="<dir>";static char _TmpG1E[59U]="Add to the list of directories to search for include files";struct _tuple16 Cyc_opt6_tuple={.f0={_TmpG1C,_TmpG1C,_TmpG1C + 3U},.f1=1,.f2={_TmpG1D,_TmpG1D,_TmpG1D + 6U},.f3=(void*)((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)& Cyc_opt6_spec),.f4={_TmpG1E,_TmpG1E,_TmpG1E + 59U}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 1144
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1,Cyc_add_ccarg};static char _TmpG1F[3U]="-L";static char _TmpG20[6U]="<dir>";static char _TmpG21[38U]="Add to the list of directories for -l";struct _tuple16 Cyc_opt7_tuple={.f0={_TmpG1F,_TmpG1F,_TmpG1F + 3U},.f1=1,.f2={_TmpG20,_TmpG20,_TmpG20 + 6U},.f3=(void*)((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)& Cyc_opt7_spec),.f4={_TmpG21,_TmpG21,_TmpG21 + 38U}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 1147
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1,Cyc_add_libarg};static char _TmpG22[3U]="-l";static char _TmpG23[10U]="<libname>";static char _TmpG24[13U]="Library file";struct _tuple16 Cyc_opt8_tuple={.f0={_TmpG22,_TmpG22,_TmpG22 + 3U},.f1=1,.f2={_TmpG23,_TmpG23,_TmpG23 + 10U},.f3=(void*)((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)& Cyc_opt8_spec),.f4={_TmpG24,_TmpG24,_TmpG24 + 13U}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 1150
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1,Cyc_add_marg};static char _TmpG25[3U]="-m";static char _TmpG26[9U]="<option>";static char _TmpG27[52U]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple16 Cyc_opt9_tuple={.f0={_TmpG25,_TmpG25,_TmpG25 + 3U},.f1=1,.f2={_TmpG26,_TmpG26,_TmpG26 + 9U},.f3=(void*)((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)& Cyc_opt9_spec),.f4={_TmpG27,_TmpG27,_TmpG27 + 52U}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 1153
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0,Cyc_set_stop_after_objectfile};static char _TmpG28[3U]="-c";static char _TmpG29[1U]="";static char _TmpG2A[61U]="Produce an object file instead of an executable; do not link";struct _tuple16 Cyc_opt10_tuple={.f0={_TmpG28,_TmpG28,_TmpG28 + 3U},.f1=0,.f2={_TmpG29,_TmpG29,_TmpG29 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt10_spec),.f4={_TmpG2A,_TmpG2A,_TmpG2A + 61U}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 1156
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5,Cyc_set_inputtype};static char _TmpG2B[3U]="-x";static char _TmpG2C[12U]=" <language>";static char _TmpG2D[49U]="Specify <language> for the following input files";struct _tuple16 Cyc_opt11_tuple={.f0={_TmpG2B,_TmpG2B,_TmpG2B + 3U},.f1=0,.f2={_TmpG2C,_TmpG2C,_TmpG2C + 12U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt11_spec),.f4={_TmpG2D,_TmpG2D,_TmpG2D + 49U}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 1159
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0,Cyc_set_pa};static char _TmpG2E[4U]="-pa";static char _TmpG2F[1U]="";static char _TmpG30[33U]="Compile for profiling with aprof";struct _tuple16 Cyc_opt12_tuple={.f0={_TmpG2E,_TmpG2E,_TmpG2E + 4U},.f1=0,.f2={_TmpG2F,_TmpG2F,_TmpG2F + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt12_spec),.f4={_TmpG30,_TmpG30,_TmpG30 + 33U}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 1162
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0,Cyc_set_stop_after_asmfile};static char _TmpG31[3U]="-S";static char _TmpG32[1U]="";static char _TmpG33[35U]="Stop after producing assembly code";struct _tuple16 Cyc_opt13_tuple={.f0={_TmpG31,_TmpG31,_TmpG31 + 3U},.f1=0,.f2={_TmpG32,_TmpG32,_TmpG32 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt13_spec),.f4={_TmpG33,_TmpG33,_TmpG33 + 35U}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 1165
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0,Cyc_set_produce_dependencies};static char _TmpG34[3U]="-M";static char _TmpG35[1U]="";static char _TmpG36[21U]="Produce dependencies";struct _tuple16 Cyc_opt14_tuple={.f0={_TmpG34,_TmpG34,_TmpG34 + 3U},.f1=0,.f2={_TmpG35,_TmpG35,_TmpG35 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt14_spec),.f4={_TmpG36,_TmpG36,_TmpG36 + 21U}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1168
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1,Cyc_add_cpparg};static char _TmpG37[4U]="-MG";static char _TmpG38[1U]="";static char _TmpG39[68U]="When producing dependencies assume that missing files are generated";struct _tuple16 Cyc_opt15_tuple={.f0={_TmpG37,_TmpG37,_TmpG37 + 4U},.f1=0,.f2={_TmpG38,_TmpG38,_TmpG38 + 1U},.f3=(void*)((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)& Cyc_opt15_spec),.f4={_TmpG39,_TmpG39,_TmpG39 + 68U}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1172
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5,Cyc_set_dependencies_target};static char _TmpG3A[4U]="-MT";static char _TmpG3B[10U]=" <target>";static char _TmpG3C[29U]="Give target for dependencies";struct _tuple16 Cyc_opt16_tuple={.f0={_TmpG3A,_TmpG3A,_TmpG3A + 4U},.f1=0,.f2={_TmpG3B,_TmpG3B,_TmpG3B + 10U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt16_spec),.f4={_TmpG3C,_TmpG3C,_TmpG3C + 29U}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1175
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5,Cyc_Specsfile_set_target_arch};static char _TmpG3D[3U]="-b";static char _TmpG3E[10U]="<machine>";static char _TmpG3F[19U]="Set target machine";struct _tuple16 Cyc_opt17_tuple={.f0={_TmpG3D,_TmpG3D,_TmpG3D + 3U},.f1=0,.f2={_TmpG3E,_TmpG3E,_TmpG3E + 10U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt17_spec),.f4={_TmpG3F,_TmpG3F,_TmpG3F + 19U}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1178
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3,& Cyc_Flags_warn_lose_unique};static char _TmpG40[14U]="-Wlose-unique";static char _TmpG41[1U]="";static char _TmpG42[49U]="Try to warn when a unique pointer might get lost";struct _tuple16 Cyc_opt18_tuple={.f0={_TmpG40,_TmpG40,_TmpG40 + 14U},.f1=0,.f2={_TmpG41,_TmpG41,_TmpG41 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt18_spec),.f4={_TmpG42,_TmpG42,_TmpG42 + 49U}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1181
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3,& Cyc_Flags_warn_override};static char _TmpG43[11U]="-Woverride";static char _TmpG44[1U]="";static char _TmpG45[58U]="Warn when a local variable overrides an existing variable";struct _tuple16 Cyc_opt19_tuple={.f0={_TmpG43,_TmpG43,_TmpG43 + 11U},.f1=0,.f2={_TmpG44,_TmpG44,_TmpG44 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt19_spec),.f4={_TmpG45,_TmpG45,_TmpG45 + 58U}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1184
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0,Cyc_Flags_set_all_warnings};static char _TmpG46[6U]="-Wall";static char _TmpG47[1U]="";static char _TmpG48[21U]="Turn on all warnings";struct _tuple16 Cyc_opt20_tuple={.f0={_TmpG46,_TmpG46,_TmpG46 + 6U},.f1=0,.f2={_TmpG47,_TmpG47,_TmpG47 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt20_spec),.f4={_TmpG48,_TmpG48,_TmpG48 + 21U}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1187
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt21_spec={0,Cyc_Flags_set_cpponly};static char _TmpG49[3U]="-E";static char _TmpG4A[1U]="";static char _TmpG4B[25U]="Stop after preprocessing";struct _tuple16 Cyc_opt21_tuple={.f0={_TmpG49,_TmpG49,_TmpG49 + 3U},.f1=0,.f2={_TmpG4A,_TmpG4A,_TmpG4A + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt21_spec),.f4={_TmpG4B,_TmpG4B,_TmpG4B + 25U}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1190
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt22_spec={0,Cyc_Flags_set_parseonly};static char _TmpG4C[17U]="-stopafter-parse";static char _TmpG4D[1U]="";static char _TmpG4E[19U]="Stop after parsing";struct _tuple16 Cyc_opt22_tuple={.f0={_TmpG4C,_TmpG4C,_TmpG4C + 17U},.f1=0,.f2={_TmpG4D,_TmpG4D,_TmpG4D + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt22_spec),.f4={_TmpG4E,_TmpG4E,_TmpG4E + 19U}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1193
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt23_spec={0,Cyc_Flags_set_tconly};static char _TmpG4F[14U]="-stopafter-tc";static char _TmpG50[1U]="";static char _TmpG51[25U]="Stop after type checking";struct _tuple16 Cyc_opt23_tuple={.f0={_TmpG4F,_TmpG4F,_TmpG4F + 14U},.f1=0,.f2={_TmpG50,_TmpG50,_TmpG50 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt23_spec),.f4={_TmpG51,_TmpG51,_TmpG51 + 25U}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1196
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3,& Cyc_noprint_r};static char _TmpG52[9U]="-noprint";static char _TmpG53[1U]="";static char _TmpG54[42U]="Do not print output (when stopping early)";struct _tuple16 Cyc_opt24_tuple={.f0={_TmpG52,_TmpG52,_TmpG52 + 9U},.f1=0,.f2={_TmpG53,_TmpG53,_TmpG53 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt24_spec),.f4={_TmpG54,_TmpG54,_TmpG54 + 42U}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1199
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt25_spec={0,Cyc_Flags_set_cfonly};static char _TmpG55[14U]="-stopafter-cf";static char _TmpG56[1U]="";static char _TmpG57[33U]="Stop after control-flow checking";struct _tuple16 Cyc_opt25_tuple={.f0={_TmpG55,_TmpG55,_TmpG55 + 14U},.f1=0,.f2={_TmpG56,_TmpG56,_TmpG56 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt25_spec),.f4={_TmpG57,_TmpG57,_TmpG57 + 33U}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1202
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt26_spec={0,Cyc_Flags_set_toconly};static char _TmpG58[15U]="-stopafter-toc";static char _TmpG59[1U]="";static char _TmpG5A[28U]="Stop after translation to C";struct _tuple16 Cyc_opt26_tuple={.f0={_TmpG58,_TmpG58,_TmpG58 + 15U},.f1=0,.f2={_TmpG59,_TmpG59,_TmpG59 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt26_spec),.f4={_TmpG5A,_TmpG5A,_TmpG5A + 28U}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1205
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3,& Cyc_ic_r};static char _TmpG5B[4U]="-ic";static char _TmpG5C[1U]="";static char _TmpG5D[26U]="Activate the link-checker";struct _tuple16 Cyc_opt27_tuple={.f0={_TmpG5B,_TmpG5B,_TmpG5B + 4U},.f1=0,.f2={_TmpG5C,_TmpG5C,_TmpG5C + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt27_spec),.f4={_TmpG5D,_TmpG5D,_TmpG5D + 26U}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1208
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3,& Cyc_pp_r};static char _TmpG5E[4U]="-pp";static char _TmpG5F[1U]="";static char _TmpG60[47U]="Pretty print the C code that Cyclone generates";struct _tuple16 Cyc_opt28_tuple={.f0={_TmpG5E,_TmpG5E,_TmpG5E + 4U},.f1=0,.f2={_TmpG5F,_TmpG5F,_TmpG5F + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt28_spec),.f4={_TmpG60,_TmpG60,_TmpG60 + 47U}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1211
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4,& Cyc_pp_r};static char _TmpG61[4U]="-up";static char _TmpG62[1U]="";static char _TmpG63[55U]="Ugly print the C code that Cyclone generates (default)";struct _tuple16 Cyc_opt29_tuple={.f0={_TmpG61,_TmpG61,_TmpG61 + 4U},.f1=0,.f2={_TmpG62,_TmpG62,_TmpG62 + 1U},.f3=(void*)((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)& Cyc_opt29_spec),.f4={_TmpG63,_TmpG63,_TmpG63 + 55U}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1214
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3,& Cyc_elim_se_r};static char _TmpG64[28U]="-elim-statement-expressions";static char _TmpG65[1U]="";static char _TmpG66[66U]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple16 Cyc_opt30_tuple={.f0={_TmpG64,_TmpG64,_TmpG64 + 28U},.f1=0,.f2={_TmpG65,_TmpG65,_TmpG65 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt30_spec),.f4={_TmpG66,_TmpG66,_TmpG66 + 66U}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1218
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4,& Cyc_elim_se_r};static char _TmpG67[31U]="-no-elim-statement-expressions";static char _TmpG68[1U]="";static char _TmpG69[47U]="Do not avoid statement expressions in C output";struct _tuple16 Cyc_opt31_tuple={.f0={_TmpG67,_TmpG67,_TmpG67 + 31U},.f1=0,.f2={_TmpG68,_TmpG68,_TmpG68 + 1U},.f3=(void*)((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)& Cyc_opt31_spec),.f4={_TmpG69,_TmpG69,_TmpG69 + 47U}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1221
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0,Cyc_set_tovc};static char _TmpG6A[8U]="-un-gcc";static char _TmpG6B[1U]="";static char _TmpG6C[57U]="Avoid gcc extensions in C output (except for attributes)";struct _tuple16 Cyc_opt32_tuple={.f0={_TmpG6A,_TmpG6A,_TmpG6A + 8U},.f1=0,.f2={_TmpG6B,_TmpG6B,_TmpG6B + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt32_spec),.f4={_TmpG6C,_TmpG6C,_TmpG6C + 57U}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1224
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5,Cyc_set_c_compiler};static char _TmpG6D[8U]="-c-comp";static char _TmpG6E[12U]=" <compiler>";static char _TmpG6F[40U]="Produce C output for the given compiler";struct _tuple16 Cyc_opt33_tuple={.f0={_TmpG6D,_TmpG6D,_TmpG6D + 8U},.f1=0,.f2={_TmpG6E,_TmpG6E,_TmpG6E + 12U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt33_spec),.f4={_TmpG6F,_TmpG6F,_TmpG6F + 40U}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1227
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0,Cyc_set_save_temps};static char _TmpG70[12U]="-save-temps";static char _TmpG71[1U]="";static char _TmpG72[29U]="Don't delete temporary files";struct _tuple16 Cyc_opt34_tuple={.f0={_TmpG70,_TmpG70,_TmpG70 + 12U},.f1=0,.f2={_TmpG71,_TmpG71,_TmpG71 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt34_spec),.f4={_TmpG72,_TmpG72,_TmpG72 + 29U}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1230
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3,& Cyc_save_c_r};static char _TmpG73[8U]="-save-c";static char _TmpG74[1U]="";static char _TmpG75[31U]="Don't delete temporary C files";struct _tuple16 Cyc_opt35_tuple={.f0={_TmpG73,_TmpG73,_TmpG73 + 8U},.f1=0,.f2={_TmpG74,_TmpG74,_TmpG74 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt35_spec),.f4={_TmpG75,_TmpG75,_TmpG75 + 31U}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1233
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0,Cyc_clear_lineno};static char _TmpG76[11U]="--nolineno";static char _TmpG77[1U]="";static char _TmpG78[63U]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple16 Cyc_opt36_tuple={.f0={_TmpG76,_TmpG76,_TmpG76 + 11U},.f1=0,.f2={_TmpG77,_TmpG77,_TmpG77 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt36_spec),.f4={_TmpG78,_TmpG78,_TmpG78 + 63U}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1236
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0,Cyc_set_nochecks};static char _TmpG79[11U]="--nochecks";static char _TmpG7A[1U]="";static char _TmpG7B[27U]="Disable bounds/null checks";struct _tuple16 Cyc_opt37_tuple={.f0={_TmpG79,_TmpG79,_TmpG79 + 11U},.f1=0,.f2={_TmpG7A,_TmpG7A,_TmpG7A + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt37_spec),.f4={_TmpG7B,_TmpG7B,_TmpG7B + 27U}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1239
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0,Cyc_set_nonullchecks};static char _TmpG7C[15U]="--nonullchecks";static char _TmpG7D[1U]="";static char _TmpG7E[20U]="Disable null checks";struct _tuple16 Cyc_opt38_tuple={.f0={_TmpG7C,_TmpG7C,_TmpG7C + 15U},.f1=0,.f2={_TmpG7D,_TmpG7D,_TmpG7D + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt38_spec),.f4={_TmpG7E,_TmpG7E,_TmpG7E + 20U}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1242
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0,Cyc_set_noboundschecks};static char _TmpG7F[17U]="--noboundschecks";static char _TmpG80[1U]="";static char _TmpG81[22U]="Disable bounds checks";struct _tuple16 Cyc_opt39_tuple={.f0={_TmpG7F,_TmpG7F,_TmpG7F + 17U},.f1=0,.f2={_TmpG80,_TmpG80,_TmpG80 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt39_spec),.f4={_TmpG81,_TmpG81,_TmpG81 + 22U}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1245
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5,Cyc_set_cpp};static char _TmpG82[9U]="-use-cpp";static char _TmpG83[7U]="<path>";static char _TmpG84[35U]="Indicate which preprocessor to use";struct _tuple16 Cyc_opt41_tuple={.f0={_TmpG82,_TmpG82,_TmpG82 + 9U},.f1=0,.f2={_TmpG83,_TmpG83,_TmpG83 + 7U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt41_spec),.f4={_TmpG84,_TmpG84,_TmpG84 + 35U}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1248
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0,Cyc_set_nocppprecomp};static char _TmpG85[16U]="-no-cpp-precomp";static char _TmpG86[1U]="";static char _TmpG87[40U]="Don't do smart preprocessing (mac only)";struct _tuple16 Cyc_opt42_tuple={.f0={_TmpG85,_TmpG85,_TmpG85 + 16U},.f1=0,.f2={_TmpG86,_TmpG86,_TmpG86 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt42_spec),.f4={_TmpG87,_TmpG87,_TmpG87 + 40U}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1251
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0,Cyc_set_nocyc};static char _TmpG88[7U]="-nocyc";static char _TmpG89[1U]="";static char _TmpG8A[33U]="Don't add implicit namespace Cyc";struct _tuple16 Cyc_opt43_tuple={.f0={_TmpG88,_TmpG88,_TmpG88 + 7U},.f1=0,.f2={_TmpG89,_TmpG89,_TmpG89 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt43_spec),.f4={_TmpG8A,_TmpG8A,_TmpG8A + 33U}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1254
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3,& Cyc_nogc_r};static char _TmpG8B[6U]="-nogc";static char _TmpG8C[1U]="";static char _TmpG8D[36U]="Don't link in the garbage collector";struct _tuple16 Cyc_opt44_tuple={.f0={_TmpG8B,_TmpG8B,_TmpG8B + 6U},.f1=0,.f2={_TmpG8C,_TmpG8C,_TmpG8C + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt44_spec),.f4={_TmpG8D,_TmpG8D,_TmpG8D + 36U}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1257
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3,& Cyc_noshake_r};static char _TmpG8E[16U]="-noremoveunused";static char _TmpG8F[1U]="";static char _TmpG90[49U]="Don't remove externed variables that aren't used";struct _tuple16 Cyc_opt45_tuple={.f0={_TmpG8E,_TmpG8E,_TmpG8E + 16U},.f1=0,.f2={_TmpG8F,_TmpG8F,_TmpG8F + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt45_spec),.f4={_TmpG90,_TmpG90,_TmpG90 + 49U}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1260
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3,& Cyc_Flags_noexpand_r};static char _TmpG91[18U]="-noexpandtypedefs";static char _TmpG92[1U]="";static char _TmpG93[41U]="Don't expand typedefs in pretty printing";struct _tuple16 Cyc_opt46_tuple={.f0={_TmpG91,_TmpG91,_TmpG91 + 18U},.f1=0,.f2={_TmpG92,_TmpG92,_TmpG92 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt46_spec),.f4={_TmpG93,_TmpG93,_TmpG93 + 41U}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1263
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3,& Cyc_print_all_tvars_r};static char _TmpG94[15U]="-printalltvars";static char _TmpG95[1U]="";static char _TmpG96[57U]="Print all type variables (even implicit default effects)";struct _tuple16 Cyc_opt47_tuple={.f0={_TmpG94,_TmpG94,_TmpG94 + 15U},.f1=0,.f2={_TmpG95,_TmpG95,_TmpG95 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt47_spec),.f4={_TmpG96,_TmpG96,_TmpG96 + 57U}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1266
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3,& Cyc_print_all_kinds_r};static char _TmpG97[15U]="-printallkinds";static char _TmpG98[1U]="";static char _TmpG99[37U]="Always print kinds of type variables";struct _tuple16 Cyc_opt48_tuple={.f0={_TmpG97,_TmpG97,_TmpG97 + 15U},.f1=0,.f2={_TmpG98,_TmpG98,_TmpG98 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt48_spec),.f4={_TmpG99,_TmpG99,_TmpG99 + 37U}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1269
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3,& Cyc_print_full_evars_r};static char _TmpG9A[16U]="-printfullevars";static char _TmpG9B[1U]="";static char _TmpG9C[50U]="Print full information for evars (type debugging)";struct _tuple16 Cyc_opt49_tuple={.f0={_TmpG9A,_TmpG9A,_TmpG9A + 16U},.f1=0,.f2={_TmpG9B,_TmpG9B,_TmpG9B + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt49_spec),.f4={_TmpG9C,_TmpG9C,_TmpG9C + 50U}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1272
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3,& Cyc_print_all_effects_r};static char _TmpG9D[17U]="-printalleffects";static char _TmpG9E[1U]="";static char _TmpG9F[45U]="Print effects for functions (type debugging)";struct _tuple16 Cyc_opt50_tuple={.f0={_TmpG9D,_TmpG9D,_TmpG9D + 17U},.f1=0,.f2={_TmpG9E,_TmpG9E,_TmpG9E + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt50_spec),.f4={_TmpG9F,_TmpG9F,_TmpG9F + 45U}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1275
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3,& Cyc_nocyc_setjmp_r};static char _TmpGA0[14U]="-nocyc_setjmp";static char _TmpGA1[1U]="";static char _TmpGA2[46U]="Do not use compiler special file cyc_setjmp.h";struct _tuple16 Cyc_opt51_tuple={.f0={_TmpGA0,_TmpGA0,_TmpGA0 + 14U},.f1=0,.f2={_TmpGA1,_TmpGA1,_TmpGA1 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt51_spec),.f4={_TmpGA2,_TmpGA2,_TmpGA2 + 46U}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1278
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3,& Cyc_Flags_compile_for_boot};static char _TmpGA3[18U]="-compile-for-boot";static char _TmpGA4[1U]="";static char _TmpGA5[71U]="Compile using the special boot library instead of the standard library";struct _tuple16 Cyc_opt52_tuple={.f0={_TmpGA3,_TmpGA3,_TmpGA3 + 18U},.f1=0,.f2={_TmpGA4,_TmpGA4,_TmpGA4 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt52_spec),.f4={_TmpGA5,_TmpGA5,_TmpGA5 + 71U}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1281
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5,Cyc_set_cyc_include};static char _TmpGA6[4U]="-CI";static char _TmpGA7[8U]=" <file>";static char _TmpGA8[31U]="Set cyc_include.h to be <file>";struct _tuple16 Cyc_opt53_tuple={.f0={_TmpGA6,_TmpGA6,_TmpGA6 + 4U},.f1=0,.f2={_TmpGA7,_TmpGA7,_TmpGA7 + 8U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt53_spec),.f4={_TmpGA8,_TmpGA8,_TmpGA8 + 31U}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1284
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3,& Cyc_Flags_warn_bounds_checks};static char _TmpGA9[18U]="-warnboundschecks";static char _TmpGAA[1U]="";static char _TmpGAB[44U]="Warn when bounds checks can't be eliminated";struct _tuple16 Cyc_opt54_tuple={.f0={_TmpGA9,_TmpGA9,_TmpGA9 + 18U},.f1=0,.f2={_TmpGAA,_TmpGAA,_TmpGAA + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt54_spec),.f4={_TmpGAB,_TmpGAB,_TmpGAB + 44U}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1287
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3,& Cyc_Flags_warn_all_null_deref};static char _TmpGAC[16U]="-warnnullchecks";static char _TmpGAD[1U]="";static char _TmpGAE[45U]="Warn when any null check can't be eliminated";struct _tuple16 Cyc_opt55_tuple={.f0={_TmpGAC,_TmpGAC,_TmpGAC + 16U},.f1=0,.f2={_TmpGAD,_TmpGAD,_TmpGAD + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt55_spec),.f4={_TmpGAE,_TmpGAE,_TmpGAE + 45U}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1290
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3,& Cyc_Flags_warn_alias_coerce};static char _TmpGAF[19U]="-warnaliascoercion";static char _TmpGB0[1U]="";static char _TmpGB1[41U]="Warn when any alias coercion is inserted";struct _tuple16 Cyc_opt56_tuple={.f0={_TmpGAF,_TmpGAF,_TmpGAF + 19U},.f1=0,.f2={_TmpGB0,_TmpGB0,_TmpGB0 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt56_spec),.f4={_TmpGB1,_TmpGB1,_TmpGB1 + 41U}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1293
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3,& Cyc_Flags_warn_region_coerce};static char _TmpGB2[18U]="-warnregioncoerce";static char _TmpGB3[1U]="";static char _TmpGB4[42U]="Warn when any region coercion is inserted";struct _tuple16 Cyc_opt57_tuple={.f0={_TmpGB2,_TmpGB2,_TmpGB2 + 18U},.f1=0,.f2={_TmpGB3,_TmpGB3,_TmpGB3 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt57_spec),.f4={_TmpGB4,_TmpGB4,_TmpGB4 + 42U}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1296
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3,& Cyc_Flags_no_register};static char _TmpGB5[12U]="-noregister";static char _TmpGB6[1U]="";static char _TmpGB7[39U]="Treat register storage class as public";struct _tuple16 Cyc_opt58_tuple={.f0={_TmpGB5,_TmpGB5,_TmpGB5 + 12U},.f1=0,.f2={_TmpGB6,_TmpGB6,_TmpGB6 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt58_spec),.f4={_TmpGB7,_TmpGB7,_TmpGB7 + 39U}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1299
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4,& Cyc_Position_use_gcc_style_location};static char _TmpGB8[18U]="-detailedlocation";static char _TmpGB9[1U]="";static char _TmpGBA[58U]="Try to give more detailed location information for errors";struct _tuple16 Cyc_opt59_tuple={.f0={_TmpGB8,_TmpGB8,_TmpGB8 + 18U},.f1=0,.f2={_TmpGB9,_TmpGB9,_TmpGB9 + 1U},.f3=(void*)((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)& Cyc_opt59_spec),.f4={_TmpGBA,_TmpGBA,_TmpGBA + 58U}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1302
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0,Cyc_set_port_c_code};static char _TmpGBB[6U]="-port";static char _TmpGBC[1U]="";static char _TmpGBD[38U]="Suggest how to port C code to Cyclone";struct _tuple16 Cyc_opt60_tuple={.f0={_TmpGBB,_TmpGBB,_TmpGBB + 6U},.f1=0,.f2={_TmpGBC,_TmpGBC,_TmpGBC + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt60_spec),.f4={_TmpGBD,_TmpGBD,_TmpGBD + 38U}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1305
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3,& Cyc_Flags_no_regions};static char _TmpGBE[11U]="-noregions";static char _TmpGBF[1U]="";static char _TmpGC0[39U]="Generate code that doesn't use regions";struct _tuple16 Cyc_opt61_tuple={.f0={_TmpGBE,_TmpGBE,_TmpGBE + 11U},.f1=0,.f2={_TmpGBF,_TmpGBF,_TmpGBF + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt61_spec),.f4={_TmpGC0,_TmpGC0,_TmpGC0 + 39U}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1308
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5,Cyc_add_iprefix};static char _TmpGC1[9U]="-iprefix";static char _TmpGC2[9U]="<prefix>";static char _TmpGC3[67U]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple16 Cyc_opt62_tuple={.f0={_TmpGC1,_TmpGC1,_TmpGC1 + 9U},.f1=0,.f2={_TmpGC2,_TmpGC2,_TmpGC2 + 9U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt62_spec),.f4={_TmpGC3,_TmpGC3,_TmpGC3 + 67U}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1311
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5,Cyc_add_iwithprefix};static char _TmpGC4[13U]="-iwithprefix";static char _TmpGC5[6U]="<dir>";static char _TmpGC6[47U]="Add <prefix>/<dir> to the second include path.";struct _tuple16 Cyc_opt63_tuple={.f0={_TmpGC4,_TmpGC4,_TmpGC4 + 13U},.f1=0,.f2={_TmpGC5,_TmpGC5,_TmpGC5 + 6U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt63_spec),.f4={_TmpGC6,_TmpGC6,_TmpGC6 + 47U}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1314
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5,Cyc_add_iwithprefixbefore};static char _TmpGC7[19U]="-iwithprefixbefore";static char _TmpGC8[6U]="<dir>";static char _TmpGC9[45U]="Add <prefix>/<dir> to the main include path.";struct _tuple16 Cyc_opt64_tuple={.f0={_TmpGC7,_TmpGC7,_TmpGC7 + 19U},.f1=0,.f2={_TmpGC8,_TmpGC8,_TmpGC8 + 6U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt64_spec),.f4={_TmpGC9,_TmpGC9,_TmpGC9 + 45U}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1317
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5,Cyc_add_isystem};static char _TmpGCA[9U]="-isystem";static char _TmpGCB[6U]="<dir>";static char _TmpGCC[90U]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple16 Cyc_opt65_tuple={.f0={_TmpGCA,_TmpGCA,_TmpGCA + 9U},.f1=0,.f2={_TmpGCB,_TmpGCB,_TmpGCB + 6U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt65_spec),.f4={_TmpGCC,_TmpGCC,_TmpGCC + 90U}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1320
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5,Cyc_add_idirafter};static char _TmpGCD[11U]="-idirafter";static char _TmpGCE[6U]="<dir>";static char _TmpGCF[46U]="Add the directory to the second include path.";struct _tuple16 Cyc_opt66_tuple={.f0={_TmpGCD,_TmpGCD,_TmpGCD + 11U},.f1=0,.f2={_TmpGCE,_TmpGCE,_TmpGCE + 6U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt66_spec),.f4={_TmpGCF,_TmpGCF,_TmpGCF + 46U}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1323
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3,& Cyc_generate_interface_r};static char _TmpGD0[15U]="--geninterface";static char _TmpGD1[1U]="";static char _TmpGD2[25U]="Generate interface files";struct _tuple16 Cyc_opt67_tuple={.f0={_TmpGD0,_TmpGD0,_TmpGD0 + 15U},.f1=0,.f2={_TmpGD1,_TmpGD1,_TmpGD1 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt67_spec),.f4={_TmpGD2,_TmpGD2,_TmpGD2 + 25U}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1326
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5,Cyc_set_specified_interface};static char _TmpGD3[12U]="--interface";static char _TmpGD4[8U]=" <file>";static char _TmpGD5[37U]="Set the interface file to be <file>.";struct _tuple16 Cyc_opt68_tuple={.f0={_TmpGD3,_TmpGD3,_TmpGD3 + 12U},.f1=0,.f2={_TmpGD4,_TmpGD4,_TmpGD4 + 8U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt68_spec),.f4={_TmpGD5,_TmpGD5,_TmpGD5 + 37U}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1329
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0,Cyc_set_many_errors};static char _TmpGD6[13U]="--manyerrors";static char _TmpGD7[1U]="";static char _TmpGD8[36U]="don't stop after only a few errors.";struct _tuple16 Cyc_opt69_tuple={.f0={_TmpGD6,_TmpGD6,_TmpGD6 + 13U},.f1=0,.f2={_TmpGD7,_TmpGD7,_TmpGD7 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt69_spec),.f4={_TmpGD8,_TmpGD8,_TmpGD8 + 36U}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1332
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3,& Cyc_Flags_print_parser_state_and_token};static char _TmpGD9[13U]="--parsestate";static char _TmpGDA[1U]="";static char _TmpGDB[50U]="print the parse state and token on a syntax error";struct _tuple16 Cyc_opt70_tuple={.f0={_TmpGD9,_TmpGD9,_TmpGD9 + 13U},.f1=0,.f2={_TmpGDA,_TmpGDA,_TmpGDA + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt70_spec),.f4={_TmpGDB,_TmpGDB,_TmpGDB + 50U}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1335
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0,Cyc_noassume_gcc_flag};static char _TmpGDC[22U]="-known-gcc-flags-only";static char _TmpGDD[1U]="";static char _TmpGDE[57U]="do not assume that unknown flags should be passed to gcc";struct _tuple16 Cyc_opt71_tuple={.f0={_TmpGDC,_TmpGDC,_TmpGDC + 22U},.f1=0,.f2={_TmpGDD,_TmpGDD,_TmpGDD + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt71_spec),.f4={_TmpGDE,_TmpGDE,_TmpGDE + 57U}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1338
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0,Cyc_print_options};static char _TmpGDF[6U]="-help";static char _TmpGE0[1U]="";static char _TmpGE1[32U]="print out the available options";struct _tuple16 Cyc_opt72_tuple={.f0={_TmpGDF,_TmpGDF,_TmpGDF + 6U},.f1=0,.f2={_TmpGE0,_TmpGE0,_TmpGE0 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt72_spec),.f4={_TmpGE1,_TmpGE1,_TmpGE1 + 32U}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1341
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0,Cyc_print_options};static char _TmpGE2[7U]="-usage";static char _TmpGE3[1U]="";static char _TmpGE4[32U]="print out the available options";struct _tuple16 Cyc_opt73_tuple={.f0={_TmpGE2,_TmpGE2,_TmpGE2 + 7U},.f1=0,.f2={_TmpGE3,_TmpGE3,_TmpGE3 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt73_spec),.f4={_TmpGE4,_TmpGE4,_TmpGE4 + 32U}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1344
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt74_spec={0,Cyc_set_notoseqc};static char _TmpGE5[10U]="-no-seq-c";static char _TmpGE6[1U]="";static char _TmpGE7[71U]="Do not force left-to-right evaluation order of generated code (unsafe)";struct _tuple16 Cyc_opt74_tuple={.f0={_TmpGE5,_TmpGE5,_TmpGE5 + 10U},.f1=0,.f2={_TmpGE6,_TmpGE6,_TmpGE6 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt74_spec),.f4={_TmpGE7,_TmpGE7,_TmpGE7 + 71U}};struct Cyc_List_List Cyc_opt74={(void*)& Cyc_opt74_tuple,0};
# 1347
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt75_spec={0,Cyc_set_pg};static char _TmpGE8[4U]="-pg";static char _TmpGE9[1U]="";static char _TmpGEA[33U]="Compile for profiling with gprof";struct _tuple16 Cyc_opt75_tuple={.f0={_TmpGE8,_TmpGE8,_TmpGE8 + 4U},.f1=0,.f2={_TmpGE9,_TmpGE9,_TmpGE9 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt75_spec),.f4={_TmpGEA,_TmpGEA,_TmpGEA + 33U}};struct Cyc_List_List Cyc_opt75={(void*)& Cyc_opt75_tuple,0};
# 1350
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt76_spec={4,& Cyc_vcgen_r};static char _TmpGEB[10U]="--novcgen";static char _TmpGEC[1U]="";static char _TmpGED[39U]="do not compute verification-conditions";struct _tuple16 Cyc_opt76_tuple={.f0={_TmpGEB,_TmpGEB,_TmpGEB + 10U},.f1=0,.f2={_TmpGEC,_TmpGEC,_TmpGEC + 1U},.f3=(void*)((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)& Cyc_opt76_spec),.f4={_TmpGED,_TmpGED,_TmpGED + 39U}};struct Cyc_List_List Cyc_opt76={(void*)& Cyc_opt76_tuple,0};
# 1353
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt77_spec={0,Cyc_Cifc_set_inst_tvar};static char _TmpGEE[17U]="--cifc-inst-tvar";static char _TmpGEF[1U]="";static char _TmpGF0[59U]="instantiate type variables instead of introducing new ones";struct _tuple16 Cyc_opt77_tuple={.f0={_TmpGEE,_TmpGEE,_TmpGEE + 17U},.f1=0,.f2={_TmpGEF,_TmpGEF,_TmpGEF + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt77_spec),.f4={_TmpGF0,_TmpGF0,_TmpGF0 + 59U}};struct Cyc_List_List Cyc_opt77={(void*)& Cyc_opt77_tuple,0};
# 1356
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt78_spec={0,Cyc_Flags_set_crgnonly};static char _TmpGF1[16U]="-stopafter-crgn";static char _TmpGF2[1U]="";static char _TmpGF3[19U]="Stop after currrgn";struct _tuple16 Cyc_opt78_tuple={.f0={_TmpGF1,_TmpGF1,_TmpGF1 + 16U},.f1=0,.f2={_TmpGF2,_TmpGF2,_TmpGF2 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt78_spec),.f4={_TmpGF3,_TmpGF3,_TmpGF3 + 19U}};struct Cyc_List_List Cyc_opt78={(void*)& Cyc_opt78_tuple,0};
# 1359
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt79_spec={3,& Cyc_Flags_allpaths};static char _TmpGF4[11U]="--allpaths";static char _TmpGF5[1U]="";static char _TmpGF6[62U]="analyze all forward paths in verification-condition generator";struct _tuple16 Cyc_opt79_tuple={.f0={_TmpGF4,_TmpGF4,_TmpGF4 + 11U},.f1=0,.f2={_TmpGF5,_TmpGF5,_TmpGF5 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt79_spec),.f4={_TmpGF6,_TmpGF6,_TmpGF6 + 62U}};struct Cyc_List_List Cyc_opt79={(void*)& Cyc_opt79_tuple,0};
# 1361
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt80_spec={4,& Cyc_Flags_allpaths};static char _TmpGF7[13U]="--mergepaths";static char _TmpGF8[1U]="";static char _TmpGF9[71U]="merge paths at join-points within the verification-condition generator";struct _tuple16 Cyc_opt80_tuple={.f0={_TmpGF7,_TmpGF7,_TmpGF7 + 13U},.f1=0,.f2={_TmpGF8,_TmpGF8,_TmpGF8 + 1U},.f3=(void*)((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)& Cyc_opt80_spec),.f4={_TmpGF9,_TmpGF9,_TmpGF9 + 71U}};struct Cyc_List_List Cyc_opt80={(void*)& Cyc_opt80_tuple,0};
# 1363
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt81_spec={0,Cyc_set_noreaps};static char _TmpGFA[9U]="-noreaps";static char _TmpGFB[1U]="";static char _TmpGFC[37U]="Generate code that doesn't use reaps";struct _tuple16 Cyc_opt81_tuple={.f0={_TmpGFA,_TmpGFA,_TmpGFA + 9U},.f1=0,.f2={_TmpGFB,_TmpGFB,_TmpGFB + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt81_spec),.f4={_TmpGFC,_TmpGFC,_TmpGFC + 37U}};struct Cyc_List_List Cyc_opt81={(void*)& Cyc_opt81_tuple,0};
# 1366
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt82_spec={3,& Cyc_Flags_print_unprovable};static char _TmpGFD[13U]="--unprovable";static char _TmpGFE[1U]="";static char _TmpGFF[38U]="print assertions that were unprovable";struct _tuple16 Cyc_opt82_tuple={.f0={_TmpGFD,_TmpGFD,_TmpGFD + 13U},.f1=0,.f2={_TmpGFE,_TmpGFE,_TmpGFE + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt82_spec),.f4={_TmpGFF,_TmpGFF,_TmpGFF + 38U}};struct Cyc_List_List Cyc_opt82={(void*)& Cyc_opt82_tuple,0};
# 1369
struct Cyc_Arg_Int_spec_Arg_Spec_struct Cyc_opt83_spec={6,Cyc_set_max_vc_summary};static char _TmpG100[17U]="--max_vc_summary";static char _TmpG101[1U]="";static char _TmpG102[62U]="controls the maximum size of post-conditions inlined by vcgen";struct _tuple16 Cyc_opt83_tuple={.f0={_TmpG100,_TmpG100,_TmpG100 + 17U},.f1=0,.f2={_TmpG101,_TmpG101,_TmpG101 + 1U},.f3=(void*)((struct Cyc_Arg_Int_spec_Arg_Spec_struct*)& Cyc_opt83_spec),.f4={_TmpG102,_TmpG102,_TmpG102 + 62U}};struct Cyc_List_List Cyc_opt83={(void*)& Cyc_opt83_tuple,0};
# 1371
struct Cyc_Arg_Int_spec_Arg_Spec_struct Cyc_opt84_spec={6,Cyc_set_max_vc_paths};static char _TmpG103[15U]="--max_vc_paths";static char _TmpG104[1U]="";static char _TmpG105[67U]="controls the maximum number of paths explored by constraint solver";struct _tuple16 Cyc_opt84_tuple={.f0={_TmpG103,_TmpG103,_TmpG103 + 15U},.f1=0,.f2={_TmpG104,_TmpG104,_TmpG104 + 1U},.f3=(void*)((struct Cyc_Arg_Int_spec_Arg_Spec_struct*)& Cyc_opt84_spec),.f4={_TmpG105,_TmpG105,_TmpG105 + 67U}};struct Cyc_List_List Cyc_opt84={(void*)& Cyc_opt84_tuple,0};
# 1373
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt85_spec={0,Cyc_set_interproc};static char _TmpG106[6U]="--inf";static char _TmpG107[1U]="";static char _TmpG108[35U]="Use interprocedural type inference";struct _tuple16 Cyc_opt85_tuple={.f0={_TmpG106,_TmpG106,_TmpG106 + 6U},.f1=0,.f2={_TmpG107,_TmpG107,_TmpG107 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt85_spec),.f4={_TmpG108,_TmpG108,_TmpG108 + 35U}};struct Cyc_List_List Cyc_opt85={(void*)& Cyc_opt85_tuple,0};
# 1376
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt86_spec={0,Cyc_set_nomerge};static char _TmpG109[10U]="--nomerge";static char _TmpG10A[1U]="";static char _TmpG10B[37U]="Do not merge multiple files into one";struct _tuple16 Cyc_opt86_tuple={.f0={_TmpG109,_TmpG109,_TmpG109 + 10U},.f1=0,.f2={_TmpG10A,_TmpG10A,_TmpG10A + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt86_spec),.f4={_TmpG10B,_TmpG10B,_TmpG10B + 37U}};struct Cyc_List_List Cyc_opt86={(void*)& Cyc_opt86_tuple,0};
# 1379
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt87_spec={0,Cyc_set_resolve};static char _TmpG10C[10U]="--resolve";static char _TmpG10D[1U]="";static char _TmpG10E[29U]="Resolve constraint variables";struct _tuple16 Cyc_opt87_tuple={.f0={_TmpG10C,_TmpG10C,_TmpG10C + 10U},.f1=0,.f2={_TmpG10D,_TmpG10D,_TmpG10D + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt87_spec),.f4={_TmpG10E,_TmpG10E,_TmpG10E + 29U}};struct Cyc_List_List Cyc_opt87={(void*)& Cyc_opt87_tuple,0};
# 1382
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt88_spec={0,Cyc_set_ovfat};static char _TmpG10F[8U]="--ovfat";static char _TmpG110[1U]="";static char _TmpG111[65U]="Override '?' annotations in the source with constraint variables";struct _tuple16 Cyc_opt88_tuple={.f0={_TmpG10F,_TmpG10F,_TmpG10F + 8U},.f1=0,.f2={_TmpG110,_TmpG110,_TmpG110 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt88_spec),.f4={_TmpG111,_TmpG111,_TmpG111 + 65U}};struct Cyc_List_List Cyc_opt88={(void*)& Cyc_opt88_tuple,0};
# 1385
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt89_spec={0,Cyc_set_infstats};static char _TmpG112[11U]="--infstats";static char _TmpG113[1U]="";static char _TmpG114[71U]="Dump statistics for the result of inference -- reads <pfx>/.cych/cvars";struct _tuple16 Cyc_opt89_tuple={.f0={_TmpG112,_TmpG112,_TmpG112 + 11U},.f1=0,.f2={_TmpG113,_TmpG113,_TmpG113 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt89_spec),.f4={_TmpG114,_TmpG114,_TmpG114 + 71U}};struct Cyc_List_List Cyc_opt89={(void*)& Cyc_opt89_tuple,0};
# 1388
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt90_spec={0,Cyc_set_tags};static char _TmpG115[7U]="--tags";static char _TmpG116[1U]="";static char _TmpG117[65U]="Generate a TAGS file for use with interprocedural type inference";struct _tuple16 Cyc_opt90_tuple={.f0={_TmpG115,_TmpG115,_TmpG115 + 7U},.f1=0,.f2={_TmpG116,_TmpG116,_TmpG116 + 1U},.f3=(void*)((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)& Cyc_opt90_spec),.f4={_TmpG117,_TmpG117,_TmpG117 + 65U}};struct Cyc_List_List Cyc_opt90={(void*)& Cyc_opt90_tuple,0};
# 1391
struct Cyc_Arg_Int_spec_Arg_Spec_struct Cyc_opt91_spec={6,Cyc_set_max_vc_term_size};static char _TmpG118[19U]="--max_vc_term_size";static char _TmpG119[1U]="";static char _TmpG11A[71U]="controls the maximum size of terms considered by the constraint solver";struct _tuple16 Cyc_opt91_tuple={.f0={_TmpG118,_TmpG118,_TmpG118 + 19U},.f1=0,.f2={_TmpG119,_TmpG119,_TmpG119 + 1U},.f3=(void*)((struct Cyc_Arg_Int_spec_Arg_Spec_struct*)& Cyc_opt91_spec),.f4={_TmpG11A,_TmpG11A,_TmpG11A + 71U}};struct Cyc_List_List Cyc_opt91={(void*)& Cyc_opt91_tuple,0};
# 1393
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt92_spec={3,& Cyc_Flags_warn_assert};static char _TmpG11B[13U]="--warnassert";static char _TmpG11C[1U]="";static char _TmpG11D[43U]="Warn when an assert might fail at run-time";struct _tuple16 Cyc_opt92_tuple={.f0={_TmpG11B,_TmpG11B,_TmpG11B + 13U},.f1=0,.f2={_TmpG11C,_TmpG11C,_TmpG11C + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt92_spec),.f4={_TmpG11D,_TmpG11D,_TmpG11D + 43U}};struct Cyc_List_List Cyc_opt92={(void*)& Cyc_opt92_tuple,0};
# 1396
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt93_spec={5,Cyc_Interproc_set_inference_prefix};static char _TmpG11E[8U]="-infpfx";static char _TmpG11F[7U]=" <dir>";static char _TmpG120[72U]="Give a directory within which inference temporary files will be stored.";struct _tuple16 Cyc_opt93_tuple={.f0={_TmpG11E,_TmpG11E,_TmpG11E + 8U},.f1=0,.f2={_TmpG11F,_TmpG11F,_TmpG11F + 7U},.f3=(void*)((struct Cyc_Arg_String_spec_Arg_Spec_struct*)& Cyc_opt93_spec),.f4={_TmpG120,_TmpG120,_TmpG120 + 72U}};struct Cyc_List_List Cyc_opt93={(void*)& Cyc_opt93_tuple,0};
# 1399
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt94_spec={3,& Cyc_nolower_r};static char _TmpG121[11U]="--no-lower";static char _TmpG122[1U]="";static char _TmpG123[25U]="Do not lower the C code.";struct _tuple16 Cyc_opt94_tuple={.f0={_TmpG121,_TmpG121,_TmpG121 + 11U},.f1=0,.f2={_TmpG122,_TmpG122,_TmpG122 + 1U},.f3=(void*)((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)& Cyc_opt94_spec),.f4={_TmpG123,_TmpG123,_TmpG123 + 25U}};struct Cyc_List_List Cyc_opt94={(void*)& Cyc_opt94_tuple,0};
# 1403
struct Cyc_List_List*Cyc_global_options[93U]={(struct Cyc_List_List*)& Cyc_opt1,(struct Cyc_List_List*)& Cyc_opt2,(struct Cyc_List_List*)& Cyc_opt3,(struct Cyc_List_List*)& Cyc_opt4,(struct Cyc_List_List*)& Cyc_opt5,(struct Cyc_List_List*)& Cyc_opt6,(struct Cyc_List_List*)& Cyc_opt7,(struct Cyc_List_List*)& Cyc_opt8,(struct Cyc_List_List*)& Cyc_opt9,(struct Cyc_List_List*)& Cyc_opt10,(struct Cyc_List_List*)& Cyc_opt11,(struct Cyc_List_List*)& Cyc_opt12,(struct Cyc_List_List*)& Cyc_opt13,(struct Cyc_List_List*)& Cyc_opt14,(struct Cyc_List_List*)& Cyc_opt15,(struct Cyc_List_List*)& Cyc_opt16,(struct Cyc_List_List*)& Cyc_opt17,(struct Cyc_List_List*)& Cyc_opt18,(struct Cyc_List_List*)& Cyc_opt19,(struct Cyc_List_List*)& Cyc_opt20,(struct Cyc_List_List*)& Cyc_opt21,(struct Cyc_List_List*)& Cyc_opt22,(struct Cyc_List_List*)& Cyc_opt23,(struct Cyc_List_List*)& Cyc_opt24,(struct Cyc_List_List*)& Cyc_opt25,(struct Cyc_List_List*)& Cyc_opt26,(struct Cyc_List_List*)& Cyc_opt27,(struct Cyc_List_List*)& Cyc_opt28,(struct Cyc_List_List*)& Cyc_opt29,(struct Cyc_List_List*)& Cyc_opt30,(struct Cyc_List_List*)& Cyc_opt31,(struct Cyc_List_List*)& Cyc_opt32,(struct Cyc_List_List*)& Cyc_opt33,(struct Cyc_List_List*)& Cyc_opt34,(struct Cyc_List_List*)& Cyc_opt35,(struct Cyc_List_List*)& Cyc_opt36,(struct Cyc_List_List*)& Cyc_opt37,(struct Cyc_List_List*)& Cyc_opt38,(struct Cyc_List_List*)& Cyc_opt39,(struct Cyc_List_List*)& Cyc_opt41,(struct Cyc_List_List*)& Cyc_opt42,(struct Cyc_List_List*)& Cyc_opt43,(struct Cyc_List_List*)& Cyc_opt44,(struct Cyc_List_List*)& Cyc_opt45,(struct Cyc_List_List*)& Cyc_opt46,(struct Cyc_List_List*)& Cyc_opt47,(struct Cyc_List_List*)& Cyc_opt48,(struct Cyc_List_List*)& Cyc_opt49,(struct Cyc_List_List*)& Cyc_opt50,(struct Cyc_List_List*)& Cyc_opt51,(struct Cyc_List_List*)& Cyc_opt52,(struct Cyc_List_List*)& Cyc_opt53,(struct Cyc_List_List*)& Cyc_opt54,(struct Cyc_List_List*)& Cyc_opt55,(struct Cyc_List_List*)& Cyc_opt56,(struct Cyc_List_List*)& Cyc_opt57,(struct Cyc_List_List*)& Cyc_opt58,(struct Cyc_List_List*)& Cyc_opt59,(struct Cyc_List_List*)& Cyc_opt60,(struct Cyc_List_List*)& Cyc_opt61,(struct Cyc_List_List*)& Cyc_opt62,(struct Cyc_List_List*)& Cyc_opt63,(struct Cyc_List_List*)& Cyc_opt64,(struct Cyc_List_List*)& Cyc_opt65,(struct Cyc_List_List*)& Cyc_opt66,(struct Cyc_List_List*)& Cyc_opt67,(struct Cyc_List_List*)& Cyc_opt68,(struct Cyc_List_List*)& Cyc_opt69,(struct Cyc_List_List*)& Cyc_opt70,(struct Cyc_List_List*)& Cyc_opt71,(struct Cyc_List_List*)& Cyc_opt72,(struct Cyc_List_List*)& Cyc_opt73,(struct Cyc_List_List*)& Cyc_opt74,(struct Cyc_List_List*)& Cyc_opt75,(struct Cyc_List_List*)& Cyc_opt76,(struct Cyc_List_List*)& Cyc_opt77,(struct Cyc_List_List*)& Cyc_opt78,(struct Cyc_List_List*)& Cyc_opt79,(struct Cyc_List_List*)& Cyc_opt80,(struct Cyc_List_List*)& Cyc_opt81,(struct Cyc_List_List*)& Cyc_opt82,(struct Cyc_List_List*)& Cyc_opt83,(struct Cyc_List_List*)& Cyc_opt84,(struct Cyc_List_List*)& Cyc_opt85,(struct Cyc_List_List*)& Cyc_opt86,(struct Cyc_List_List*)& Cyc_opt87,(struct Cyc_List_List*)& Cyc_opt88,(struct Cyc_List_List*)& Cyc_opt89,(struct Cyc_List_List*)& Cyc_opt90,(struct Cyc_List_List*)& Cyc_opt91,(struct Cyc_List_List*)& Cyc_opt92,(struct Cyc_List_List*)& Cyc_opt93,(struct Cyc_List_List*)& Cyc_opt94};
# 1416
void Cyc_print_options (void){struct Cyc_List_List**_T0;struct Cyc_List_List*_T1;struct _fat_ptr _T2;_T0=Cyc_global_options;_T1=_T0[0];_T2=
_tag_fat("<program.cyc>",sizeof(char),14U);Cyc_Arg_usage(_T1,_T2);}
# 1421
int Cyc_main(int argc,struct _fat_ptr argv){int _T0;unsigned _T1;struct Cyc_List_List**_T2;int _T3;char*_T4;struct Cyc_List_List**_T5;struct Cyc_List_List*_T6;struct Cyc_List_List**_T7;int _T8;char*_T9;struct Cyc_List_List**_TA;struct Cyc_List_List**_TB;struct Cyc_List_List*_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;int _T10;struct Cyc_String_pa_PrintArg_struct _T11;void**_T12;struct Cyc___cycFILE*_T13;struct _fat_ptr _T14;struct _fat_ptr _T15;struct Cyc_List_List*_T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct _fat_ptr _T19;unsigned char*_T1A;struct _fat_ptr*_T1B;struct Cyc_List_List*_T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;struct Cyc_List_List*_T1F;struct _fat_ptr _T20;struct Cyc_List_List*_T21;struct _fat_ptr _T22;struct _fat_ptr _T23;char*_T24;unsigned _T25;struct Cyc_List_List*_T26;struct _fat_ptr _T27;struct Cyc_List_List*_T28;struct _fat_ptr _T29;struct _fat_ptr _T2A;int _T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;int _T2E;const char*_T2F;int _T30;struct _fat_ptr _T31;struct Cyc_String_pa_PrintArg_struct _T32;struct Cyc_String_pa_PrintArg_struct _T33;struct Cyc_String_pa_PrintArg_struct _T34;struct _fat_ptr _T35;void*_T36;void*_T37;unsigned _T38;struct Cyc_String_pa_PrintArg_struct _T39;void**_T3A;void**_T3B;void**_T3C;void**_T3D;struct _fat_ptr _T3E;struct _fat_ptr _T3F;struct Cyc___cycFILE*_T40;struct _fat_ptr _T41;struct _fat_ptr _T42;enum Cyc_Flags_Cyclone_Passes _T43;int _T44;int _T45;struct _fat_ptr _T46;enum Cyc_Flags_Cyclone_Passes _T47;int _T48;unsigned long _T49;struct _fat_ptr _T4A;struct _handler_cons*_T4B;int*_T4C;int _T4D;struct Cyc_List_List*_T4E;void*_T4F;struct _fat_ptr*_T50;struct _fat_ptr _T51;int _T52;struct Cyc_List_List*_T53;void*_T54;int _T55;int _T56;enum Cyc_Flags_Cyclone_Passes _T57;int _T58;struct _fat_ptr _T59;char*_T5A;unsigned _T5B;struct Cyc_List_List*_T5C;unsigned _T5D;struct Cyc_List_List*_T5E;struct _fat_ptr*_T5F;struct _fat_ptr _T60;struct _fat_ptr _T61;struct _fat_ptr _T62;struct _fat_ptr _T63;struct _fat_ptr _T64;struct Cyc_List_List*(*_T65)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*);struct Cyc_List_List*(*_T66)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T67;struct Cyc_List_List*_T68;struct _fat_ptr _T69;struct Cyc_List_List*_T6A;struct _fat_ptr*_T6B;struct Cyc_List_List*(*_T6C)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*);struct Cyc_List_List*(*_T6D)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T6E;struct _fat_ptr _T6F;int _T70;int _T71;int _T72;struct _fat_ptr*_T73;struct _fat_ptr _T74;struct _fat_ptr _T75;int _T76;int _T77;struct _fat_ptr*_T78;struct _fat_ptr _T79;struct _fat_ptr _T7A;int _T7B;struct _fat_ptr _T7C;int _T7D;int _T7E;int _T7F;int _T80;int _T81;int _T82;struct _fat_ptr _T83;int _T84;int _T85;struct _fat_ptr _T86;struct Cyc_String_pa_PrintArg_struct _T87;struct Cyc_String_pa_PrintArg_struct _T88;struct Cyc_String_pa_PrintArg_struct _T89;void**_T8A;void**_T8B;void**_T8C;struct _fat_ptr _T8D;struct _fat_ptr _T8E;struct _fat_ptr _T8F;struct Cyc_String_pa_PrintArg_struct _T90;struct Cyc_String_pa_PrintArg_struct _T91;struct Cyc_String_pa_PrintArg_struct _T92;void**_T93;void**_T94;void**_T95;struct _fat_ptr _T96;struct _fat_ptr _T97;int _T98;struct _handler_cons*_T99;int*_T9A;int _T9B;struct Cyc_List_List*(*_T9C)(int(*)(struct _fat_ptr*),struct Cyc_List_List*);struct Cyc_List_List*(*_T9D)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T9E;struct Cyc_List_List*(*_T9F)(int(*)(struct _fat_ptr*),struct Cyc_List_List*);struct Cyc_List_List*(*_TA0)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_TA1;struct Cyc_List_List*_TA2;struct Cyc_List_List*_TA3;int _TA4;struct Cyc_List_List*_TA5;struct Cyc_Interface_I*(*_TA6)(struct Cyc_Interface_I*(*)(struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Interface_I*(*_TA7)(struct Cyc_Interface_I*(*)(void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*_TA8;struct Cyc_List_List*_TA9;struct Cyc___cycFILE*_TAA;struct _fat_ptr _TAB;struct _fat_ptr _TAC;int _TAD;struct _fat_ptr _TAE;struct Cyc_String_pa_PrintArg_struct _TAF;struct _fat_ptr*_TB0;struct _fat_ptr _TB1;void**_TB2;struct _fat_ptr _TB3;struct _fat_ptr _TB4;struct _fat_ptr _TB5;struct _fat_ptr _TB6;struct _fat_ptr _TB7;struct Cyc_Interface_I*_TB8;struct Cyc_Interface_I*_TB9;struct _tuple11*_TBA;int _TBB;struct Cyc___cycFILE*_TBC;struct _fat_ptr _TBD;struct _fat_ptr _TBE;void*_TBF;struct Cyc_String_pa_PrintArg_struct _TC0;struct _fat_ptr _TC1;void*_TC2;void*_TC3;unsigned _TC4;struct Cyc_String_pa_PrintArg_struct _TC5;struct _fat_ptr _TC6;void*_TC7;void*_TC8;unsigned _TC9;struct Cyc_Int_pa_PrintArg_struct _TCA;int _TCB;void**_TCC;void**_TCD;void**_TCE;struct Cyc___cycFILE*_TCF;struct _fat_ptr _TD0;struct _fat_ptr _TD1;struct _fat_ptr _TD2;struct Cyc_String_pa_PrintArg_struct _TD3;struct _fat_ptr*_TD4;struct _fat_ptr _TD5;void**_TD6;struct _fat_ptr _TD7;struct _fat_ptr _TD8;struct _fat_ptr _TD9;struct Cyc_String_pa_PrintArg_struct _TDA;struct Cyc_String_pa_PrintArg_struct _TDB;struct Cyc_String_pa_PrintArg_struct _TDC;struct Cyc_String_pa_PrintArg_struct _TDD;struct Cyc_String_pa_PrintArg_struct _TDE;struct Cyc_String_pa_PrintArg_struct _TDF;void**_TE0;void**_TE1;void**_TE2;void**_TE3;void**_TE4;void**_TE5;struct _fat_ptr _TE6;struct _fat_ptr _TE7;int _TE8;struct Cyc_String_pa_PrintArg_struct _TE9;void**_TEA;struct Cyc___cycFILE*_TEB;struct _fat_ptr _TEC;struct _fat_ptr _TED;struct _fat_ptr _TEE;char*_TEF;char*_TF0;const char*_TF1;int _TF2;struct Cyc___cycFILE*_TF3;struct _fat_ptr _TF4;struct _fat_ptr _TF5;int _TF6;int _TF7;
# 1424
GC_blacklist_warn_clear();{
# 1427
struct _fat_ptr optstring=_tag_fat("Options:",sizeof(char),9U);{
# 1429
int i=1;_TL14C: _T0=i;_T1=(unsigned)_T0;if(_T1 < 93U)goto _TL14A;else{goto _TL14B;}
_TL14A: _T2=Cyc_global_options;_T3=i - 1;_T4=_check_known_subscript_notnull(_T2,93U,sizeof(struct Cyc_List_List*),_T3);_T5=(struct Cyc_List_List**)_T4;_T6=*_T5;_T7=Cyc_global_options;_T8=i;_T9=_check_known_subscript_notnull(_T7,93U,sizeof(struct Cyc_List_List*),_T8);_TA=(struct Cyc_List_List**)_T9;_T6->tl=*_TA;
# 1429
i=i + 1;goto _TL14C;_TL14B:;}_TB=Cyc_global_options;{
# 1431
struct Cyc_List_List*options=_TB[0];_TC=options;_TD=optstring;_TE=argv;{
# 1433
struct _fat_ptr otherargs=Cyc_Specsfile_parse_b(_TC,Cyc_add_other,Cyc_add_other_flag,_TD,_TE);_TF=
# 1439
_tag_fat("cycspecs",sizeof(char),9U);{struct _fat_ptr specs_file=Cyc_find_in_arch_path(_TF);_T10=Cyc_Flags_verbose;
if(!_T10)goto _TL14D;{struct Cyc_String_pa_PrintArg_struct _TF8;_TF8.tag=0;_TF8.f1=specs_file;_T11=_TF8;}{struct Cyc_String_pa_PrintArg_struct _TF8=_T11;void*_TF9[1];_T12=_TF9 + 0;*_T12=& _TF8;_T13=Cyc_stderr;_T14=_tag_fat("Reading from specs file %s\n",sizeof(char),28U);_T15=_tag_fat(_TF9,sizeof(void*),1);Cyc_fprintf(_T13,_T14,_T15);}goto _TL14E;_TL14D: _TL14E: {
struct Cyc_List_List*specs=Cyc_Specsfile_read_specs(specs_file);_T16=specs;_T17=
# 1443
_tag_fat("cyclone",sizeof(char),8U);_T18=Cyc_Specsfile_get_spec(_T16,_T17);{struct _fat_ptr args_from_specs_file=Cyc_Specsfile_split_specs(_T18);_T19=args_from_specs_file;_T1A=_T19.curr;_T1B=(struct _fat_ptr*)_T1A;
if(_T1B==0)goto _TL14F;
Cyc_Arg_current=0;_T1C=options;_T1D=optstring;_T1E=args_from_specs_file;
Cyc_Arg_parse(_T1C,Cyc_add_other,Cyc_add_other_flag,_T1D,_T1E);goto _TL150;_TL14F: _TL150: _T1F=specs;_T20=
# 1448
_tag_fat("cyclone_target_cflags",sizeof(char),22U);{struct _fat_ptr target_cflags=Cyc_Specsfile_get_spec(_T1F,_T20);_T21=specs;_T22=
_tag_fat("cyclone_cc",sizeof(char),11U);{struct _fat_ptr cyclone_cc=Cyc_Specsfile_get_spec(_T21,_T22);_T23=cyclone_cc;_T24=_T23.curr;_T25=(unsigned)_T24;
if(_T25)goto _TL151;else{goto _TL153;}_TL153: cyclone_cc=_tag_fat("gcc",sizeof(char),4U);goto _TL152;_TL151: _TL152: _T26=specs;_T27=
_tag_fat("cyclone_inc_path",sizeof(char),17U);Cyc_def_inc_path=Cyc_Specsfile_get_spec(_T26,_T27);
# 1454
Cyc_Arg_current=0;_T28=options;_T29=optstring;_T2A=otherargs;
Cyc_Arg_parse(_T28,Cyc_add_other,Cyc_add_other_flag,_T29,_T2A);_T2B=Cyc_Flags_infstats;
if(!_T2B)goto _TL154;
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();
Cyc_Interproc_inference_statistics();
return 0;_TL154: _T2C=Cyc_cpp;_T2D=
# 1462
_tag_fat("",sizeof(char),1U);_T2E=Cyc_strcmp(_T2C,_T2D);if(_T2E!=0)goto _TL156;_T30=Cyc_produce_dependencies;
# 1478 "cyclone.cyc"
if(!_T30)goto _TL158;_T2F="";goto _TL159;_TL158: _T2F=" -E";_TL159:{const char*dash_E=_T2F;{struct Cyc_String_pa_PrintArg_struct _TF8;_TF8.tag=0;
# 1480
_TF8.f1=cyclone_cc;_T32=_TF8;}{struct Cyc_String_pa_PrintArg_struct _TF8=_T32;{struct Cyc_String_pa_PrintArg_struct _TF9;_TF9.tag=0;_TF9.f1=target_cflags;_T33=_TF9;}{struct Cyc_String_pa_PrintArg_struct _TF9=_T33;{struct Cyc_String_pa_PrintArg_struct _TFA;_TFA.tag=0;{const char*_TFB=dash_E;_T36=(void*)_TFB;_T37=(void*)_TFB;_T38=_get_zero_arr_size_char(_T37,1U);_T35=_tag_fat(_T36,sizeof(char),_T38);}_TFA.f1=_T35;_T34=_TFA;}{struct Cyc_String_pa_PrintArg_struct _TFA=_T34;{struct Cyc_String_pa_PrintArg_struct _TFB;_TFB.tag=0;_TFB.f1=specs_file;_T39=_TFB;}{struct Cyc_String_pa_PrintArg_struct _TFB=_T39;void*_TFC[4];_T3A=_TFC + 0;*_T3A=& _TF8;_T3B=_TFC + 1;*_T3B=& _TF9;_T3C=_TFC + 2;*_T3C=& _TFA;_T3D=_TFC + 3;*_T3D=& _TFB;_T3E=
# 1479
_tag_fat("%s %s -w -x c%s -specs %s",sizeof(char),26U);_T3F=_tag_fat(_TFC,sizeof(void*),4);_T31=Cyc_aprintf(_T3E,_T3F);}}}}Cyc_set_cpp(_T31);}goto _TL157;_TL156: _TL157:
# 1482
 if(Cyc_cyclone_files!=0)goto _TL15A;if(Cyc_ccargs!=0)goto _TL15A;_T40=Cyc_stderr;_T41=
_tag_fat("missing file\n",sizeof(char),14U);_T42=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T40,_T41,_T42);
exit(1);goto _TL15B;_TL15A: _TL15B: _T43=Cyc_Flags_stop_after_pass;_T44=(int)_T43;
# 1491
if(_T44 <= 0)goto _TL15C;_T45=Cyc_nocyc_setjmp_r;if(_T45)goto _TL15C;else{goto _TL15E;}
_TL15E: _T46=_tag_fat("cyc_setjmp.h",sizeof(char),13U);Cyc_cyc_setjmp=Cyc_find_in_arch_path(_T46);goto _TL15D;_TL15C: _TL15D: _T47=Cyc_Flags_stop_after_pass;_T48=(int)_T47;
if(_T48 <= 0)goto _TL15F;_T49=Cyc_strlen(Cyc_cyc_include);if(_T49!=0U)goto _TL15F;_T4A=
_tag_fat("cyc_include.h",sizeof(char),14U);Cyc_cyc_include=Cyc_find_in_exec_path(_T4A);goto _TL160;_TL15F: _TL160:{struct _handler_cons _TF8;_T4B=& _TF8;_push_handler(_T4B);{int _TF9=0;_T4C=_TF8.handler;_T4D=setjmp(_T4C);if(!_T4D)goto _TL161;_TF9=1;goto _TL162;_TL161: _TL162: if(_TF9)goto _TL163;else{goto _TL165;}_TL165:{
# 1498
struct Cyc_List_List*l=Cyc_List_rev(Cyc_cyclone_files);_TL169: if(l!=0)goto _TL167;else{goto _TL168;}
_TL167: _T4E=l;_T4F=_T4E->hd;_T50=(struct _fat_ptr*)_T4F;_T51=*_T50;Cyc_process_file(_T51);_T52=Cyc_compile_failure;
if(!_T52)goto _TL16A;{int _TFA=1;_npop_handler(0);return _TFA;}_TL16A: _T53=l;
# 1498
l=_T53->tl;goto _TL169;_TL168:;}_pop_handler();goto _TL164;_TL163: _T54=Cyc_Core_get_exn_thrown();{void*_TFA=(void*)_T54;void*_TFB;_TFB=_TFA;{void*x=_TFB;_T55=Cyc_compile_failure;
# 1503
if(!_T55)goto _TL16C;return 1;_TL16C:
 Cyc_Core_rethrow(x);};}_TL164:;}}_T56=Cyc_Flags_tags;
# 1506
if(!_T56)goto _TL16E;
Cyc_Interproc_close_tags_file();goto _TL16F;_TL16E: _TL16F: _T57=Cyc_Flags_stop_after_pass;_T58=(int)_T57;
# 1510
if(_T58 >= 15)goto _TL170;return 0;_TL170:
# 1516
 if(Cyc_ccargs!=0)goto _TL172;return 0;_TL172: _T59=Cyc_Specsfile_target_arch;_T5A=_T59.curr;_T5B=(unsigned)_T5A;
# 1519
if(!_T5B)goto _TL174;_T5C=Cyc_Specsfile_cyclone_exec_path;_T5D=(unsigned)_T5C;if(!_T5D)goto _TL174;{struct Cyc_List_List*_TF8=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TF9=_cycalloc(sizeof(struct _fat_ptr));
*_TF9=_tag_fat("",sizeof(char),1U);_T5F=(struct _fat_ptr*)_TF9;}_TF8->hd=_T5F;
_TF8->tl=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_T5E=(struct Cyc_List_List*)_TF8;}_T60=
_tag_fat(" -L",sizeof(char),4U);_T61=
# 1520
Cyc_str_sepstr(_T5E,_T60);Cyc_add_ccarg(_T61);goto _TL175;_TL174: _TL175: _T62=
# 1523
_tag_fat("-L",sizeof(char),3U);_T63=Cyc_Specsfile_def_lib_path;_T64=Cyc_strconcat(_T62,_T63);Cyc_add_ccarg(_T64);
Cyc_ccargs=Cyc_List_rev(Cyc_ccargs);_T66=Cyc_List_map;{
struct Cyc_List_List*(*_TF8)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))_T66;_T65=_TF8;}_T67=Cyc_ccargs;_T68=_T65(Cyc_sh_escape_stringptr,_T67);_T69=_tag_fat(" ",sizeof(char),2U);{struct _fat_ptr ccargs_string=Cyc_str_sepstr(_T68,_T69);
Cyc_libargs=Cyc_List_rev(Cyc_libargs);{struct Cyc_List_List*_TF8=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TF9=_cycalloc(sizeof(struct _fat_ptr));
*_TF9=_tag_fat("",sizeof(char),1U);_T6B=(struct _fat_ptr*)_TF9;}_TF8->hd=_T6B;_T6D=Cyc_List_map;{
struct Cyc_List_List*(*_TF9)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))_T6D;_T6C=_TF9;}_T6E=Cyc_libargs;_TF8->tl=_T6C(Cyc_sh_escape_stringptr,_T6E);_T6A=(struct Cyc_List_List*)_TF8;}_T6F=_tag_fat(" ",sizeof(char),2U);{
# 1527
struct _fat_ptr libargs_string=Cyc_str_sepstr(_T6A,_T6F);
# 1530
struct Cyc_List_List*stdlib;
struct _fat_ptr stdlib_string;_T71=Cyc_stop_after_asmfile_r;
# 1533
if(_T71)goto _TL178;else{goto _TL17A;}_TL17A: _T72=Cyc_stop_after_objectfile_r;if(_T72)goto _TL178;else{goto _TL179;}_TL179: if(Cyc_output_file!=0)goto _TL17B;else{goto _TL176;}_TL17B: _T73=Cyc_output_file;_T74=*_T73;_T75=
# 1535
_tag_fat(".a",sizeof(char),3U);_T76=Cyc_Filename_check_suffix(_T74,_T75);
# 1533
if(_T76)goto _TL178;else{goto _TL176;}_TL178: _T70=1;goto _TL177;_TL176:
# 1536
 if(Cyc_output_file==0)goto _TL17C;_T78=Cyc_output_file;_T79=*_T78;_T7A=_tag_fat(".lib",sizeof(char),5U);_T77=Cyc_Filename_check_suffix(_T79,_T7A);goto _TL17D;_TL17C: _T77=0;_TL17D: _T70=_T77;_TL177: {
# 1532
int is_not_executable=_T70;_T7B=is_not_executable;
# 1537
if(!_T7B)goto _TL17E;
stdlib=0;_T7C=
_tag_fat("",sizeof(char),1U);stdlib_string=_T7C;goto _TL17F;
# 1542
_TL17E:{struct _fat_ptr libcyc_flag;
struct _fat_ptr nogc_filename;
struct _fat_ptr runtime_filename;
struct _fat_ptr gc_filename;_T7D=Cyc_pa_r;
if(!_T7D)goto _TL180;
libcyc_flag=_tag_fat("-lcyc_a",sizeof(char),8U);
nogc_filename=_tag_fat("nogc_a.a",sizeof(char),9U);
runtime_filename=_tag_fat("runtime_cyc_a.a",sizeof(char),16U);goto _TL181;
_TL180: _T7E=Cyc_nocheck_r;if(!_T7E)goto _TL182;
libcyc_flag=_tag_fat("-lcyc_nocheck",sizeof(char),14U);
nogc_filename=_tag_fat("nogc.a",sizeof(char),7U);
runtime_filename=_tag_fat("runtime_cyc.a",sizeof(char),14U);goto _TL183;
_TL182: _T7F=Cyc_pg_r;if(!_T7F)goto _TL184;
libcyc_flag=_tag_fat("-lcyc_pg",sizeof(char),9U);
runtime_filename=_tag_fat("runtime_cyc_pg.a",sizeof(char),17U);
nogc_filename=_tag_fat("nogc_pg.a",sizeof(char),10U);goto _TL185;
_TL184: _T80=Cyc_Flags_compile_for_boot;if(!_T80)goto _TL186;
# 1560
libcyc_flag=_tag_fat("-lcycboot",sizeof(char),10U);
nogc_filename=_tag_fat("nogc.a",sizeof(char),7U);
runtime_filename=_tag_fat("runtime_cyc.a",sizeof(char),14U);goto _TL187;
_TL186: _T81=Cyc_pthread_r;if(!_T81)goto _TL188;
libcyc_flag=_tag_fat("-lcyc",sizeof(char),6U);
nogc_filename=_tag_fat("nogc_pthread.a",sizeof(char),15U);
runtime_filename=_tag_fat("runtime_cyc_pthread.a",sizeof(char),22U);goto _TL189;
# 1568
_TL188: libcyc_flag=_tag_fat("-lcyc",sizeof(char),6U);
nogc_filename=_tag_fat("nogc.a",sizeof(char),7U);
runtime_filename=_tag_fat("runtime_cyc.a",sizeof(char),14U);_TL189: _TL187: _TL185: _TL183: _TL181: _T82=Cyc_pthread_r;
# 1572
if(_T82)goto _TL18A;else{goto _TL18C;}
_TL18C: gc_filename=_tag_fat("gc.a",sizeof(char),5U);goto _TL18B;
# 1575
_TL18A: gc_filename=_tag_fat("gc_pthread.a",sizeof(char),13U);_TL18B: _T84=Cyc_nogc_r;
# 1577
if(!_T84)goto _TL18D;_T83=Cyc_find_in_arch_path(nogc_filename);goto _TL18E;_TL18D: _T83=Cyc_find_in_arch_path(gc_filename);_TL18E: {struct _fat_ptr gc=_T83;
struct _fat_ptr runtime=Cyc_find_in_arch_path(runtime_filename);
# 1580
stdlib=0;_T85=Cyc_nogc_r;
if(!_T85)goto _TL18F;{struct Cyc_String_pa_PrintArg_struct _TF8;_TF8.tag=0;
_TF8.f1=libcyc_flag;_T87=_TF8;}{struct Cyc_String_pa_PrintArg_struct _TF8=_T87;{struct Cyc_String_pa_PrintArg_struct _TF9;_TF9.tag=0;_TF9.f1=runtime;_T88=_TF9;}{struct Cyc_String_pa_PrintArg_struct _TF9=_T88;{struct Cyc_String_pa_PrintArg_struct _TFA;_TFA.tag=0;_TFA.f1=gc;_T89=_TFA;}{struct Cyc_String_pa_PrintArg_struct _TFA=_T89;void*_TFB[3];_T8A=_TFB + 0;*_T8A=& _TF8;_T8B=_TFB + 1;*_T8B=& _TF9;_T8C=_TFB + 2;*_T8C=& _TFA;_T8D=_tag_fat(" %s %s %s ",sizeof(char),11U);_T8E=_tag_fat(_TFB,sizeof(void*),3);_T86=Cyc_aprintf(_T8D,_T8E);}}}stdlib_string=_T86;goto _TL190;
# 1584
_TL18F:{struct Cyc_String_pa_PrintArg_struct _TF8;_TF8.tag=0;_TF8.f1=libcyc_flag;_T90=_TF8;}{struct Cyc_String_pa_PrintArg_struct _TF8=_T90;{struct Cyc_String_pa_PrintArg_struct _TF9;_TF9.tag=0;_TF9.f1=runtime;_T91=_TF9;}{struct Cyc_String_pa_PrintArg_struct _TF9=_T91;{struct Cyc_String_pa_PrintArg_struct _TFA;_TFA.tag=0;_TFA.f1=gc;_T92=_TFA;}{struct Cyc_String_pa_PrintArg_struct _TFA=_T92;void*_TFB[3];_T93=_TFB + 0;*_T93=& _TF8;_T94=_TFB + 1;*_T94=& _TF9;_T95=_TFB + 2;*_T95=& _TFA;_T96=_tag_fat(" %s %s %s -DCYC_GC_PTHREAD_REDIRECTS ",sizeof(char),38U);_T97=_tag_fat(_TFB,sizeof(void*),3);_T8F=Cyc_aprintf(_T96,_T97);}}}stdlib_string=_T8F;_TL190:;}}_TL17F: _T98=Cyc_ic_r;
# 1587
if(!_T98)goto _TL191;{struct _handler_cons _TF8;_T99=& _TF8;_push_handler(_T99);{int _TF9=0;_T9A=_TF8.handler;_T9B=setjmp(_T9A);if(!_T9B)goto _TL193;_TF9=1;goto _TL194;_TL193: _TL194: if(_TF9)goto _TL195;else{goto _TL197;}_TL197: _T9D=Cyc_List_filter;{
struct Cyc_List_List*(*_TFA)(int(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))_T9D;_T9C=_TFA;}_T9E=Cyc_ccargs;Cyc_ccargs=_T9C(Cyc_is_cfile,_T9E);_TA0=Cyc_List_filter;{
struct Cyc_List_List*(*_TFA)(int(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))_TA0;_T9F=_TFA;}_TA1=Cyc_libargs;Cyc_libargs=_T9F(Cyc_is_cfile,_TA1);_TA2=stdlib;_TA3=
Cyc_List_append(Cyc_ccargs,Cyc_libargs);{struct Cyc_List_List*lf=Cyc_List_append(_TA2,_TA3);_TA4=is_not_executable;
if(_TA4)goto _TL198;else{goto _TL19A;}
_TL19A:{struct Cyc_List_List*_TFA=_cycalloc(sizeof(struct Cyc_List_List));_TFA->hd=Cyc_final_strptr;_TFA->tl=lf;_TA5=(struct Cyc_List_List*)_TFA;}lf=_TA5;goto _TL199;_TL198: _TL199: _TA7=Cyc_Interface_get_and_merge_list;{
# 1594
struct Cyc_Interface_I*(*_TFA)(struct Cyc_Interface_I*(*)(struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*)(struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))_TA7;_TA6=_TFA;}_TA8=lf;_TA9=lf;{struct Cyc_Interface_I*i=_TA6(Cyc_read_cycio,_TA8,_TA9);
if(i!=0)goto _TL19B;_TAA=Cyc_stderr;_TAB=
_tag_fat("Error: interfaces incompatible\n",sizeof(char),32U);_TAC=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TAA,_TAB,_TAC);
Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _TFA=1;_npop_handler(0);return _TFA;}_TL19B: _TAD=is_not_executable;
# 1601
if(!_TAD)goto _TL19D;
if(Cyc_output_file==0)goto _TL19F;{struct Cyc_String_pa_PrintArg_struct _TFA;_TFA.tag=0;_TB0=Cyc_output_file;_TB1=*_TB0;
_TFA.f1=Cyc_Filename_chop_extension(_TB1);_TAF=_TFA;}{struct Cyc_String_pa_PrintArg_struct _TFA=_TAF;void*_TFB[1];_TB2=_TFB + 0;*_TB2=& _TFA;_TB3=_tag_fat("%s.cycio",sizeof(char),9U);_TB4=_tag_fat(_TFB,sizeof(void*),1);_TAE=Cyc_aprintf(_TB3,_TB4);}{struct _fat_ptr output_file_io=_TAE;_TB5=output_file_io;_TB6=
_tag_fat("wb",sizeof(char),3U);_TB7=_tag_fat("interface object file",sizeof(char),22U);{struct Cyc___cycFILE*f=Cyc_try_file_open(_TB5,_TB6,_TB7);
if(f!=0)goto _TL1A1;
Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _TFA=1;_npop_handler(0);return _TFA;}_TL1A1:
# 1610
 Cyc_Interface_save(i,f);
Cyc_file_close(f);}}goto _TL1A0;_TL19F: _TL1A0: goto _TL19E;
# 1614
_TL19D: _TB8=Cyc_Interface_empty();_TB9=i;{struct _tuple11*_TFA=_cycalloc(sizeof(struct _tuple11));
_TFA->f0=_tag_fat("empty interface",sizeof(char),16U);_TFA->f1=_tag_fat("global interface",sizeof(char),17U);_TBA=(struct _tuple11*)_TFA;}_TBB=
# 1614
Cyc_Interface_is_subinterface(_TB8,_TB9,_TBA);if(_TBB)goto _TL1A3;else{goto _TL1A5;}
# 1616
_TL1A5: _TBC=Cyc_stderr;_TBD=_tag_fat("Error: some objects are still undefined\n",sizeof(char),41U);_TBE=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TBC,_TBD,_TBE);
Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _TFA=1;_npop_handler(0);return _TFA;}_TL1A3: _TL19E:;}}_pop_handler();goto _TL196;_TL195: _TBF=Cyc_Core_get_exn_thrown();{void*_TFA=(void*)_TBF;void*_TFB;_TFB=_TFA;{void*e=_TFB;{struct Cyc_String_pa_PrintArg_struct _TFC;_TFC.tag=0;{const char*_TFD=
# 1624
Cyc_Core_get_exn_name(e);_TC2=(void*)_TFD;_TC3=(void*)_TFD;_TC4=_get_zero_arr_size_char(_TC3,1U);_TC1=_tag_fat(_TC2,sizeof(char),_TC4);}_TFC.f1=_TC1;_TC0=_TFC;}{struct Cyc_String_pa_PrintArg_struct _TFC=_TC0;{struct Cyc_String_pa_PrintArg_struct _TFD;_TFD.tag=0;{const char*_TFE=Cyc_Core_get_exn_filename();_TC7=(void*)_TFE;_TC8=(void*)_TFE;_TC9=_get_zero_arr_size_char(_TC8,1U);_TC6=_tag_fat(_TC7,sizeof(char),_TC9);}_TFD.f1=_TC6;_TC5=_TFD;}{struct Cyc_String_pa_PrintArg_struct _TFD=_TC5;{struct Cyc_Int_pa_PrintArg_struct _TFE;_TFE.tag=1;_TCB=Cyc_Core_get_exn_lineno();_TFE.f1=(unsigned long)_TCB;_TCA=_TFE;}{struct Cyc_Int_pa_PrintArg_struct _TFE=_TCA;void*_TFF[3];_TCC=_TFF + 0;*_TCC=& _TFC;_TCD=_TFF + 1;*_TCD=& _TFD;_TCE=_TFF + 2;*_TCE=& _TFE;_TCF=Cyc_stderr;_TD0=_tag_fat("INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu",sizeof(char),131U);_TD1=_tag_fat(_TFF,sizeof(void*),3);Cyc_fprintf(_TCF,_TD0,_TD1);}}}
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;};}_TL196:;}}goto _TL192;_TL191: _TL192: {
# 1630
struct _fat_ptr outfile_str=_tag_fat("",sizeof(char),1U);
if(Cyc_output_file==0)goto _TL1A6;{struct Cyc_String_pa_PrintArg_struct _TF8;_TF8.tag=0;_TD4=Cyc_output_file;_TD5=*_TD4;
_TF8.f1=Cyc_sh_escape_string(_TD5);_TD3=_TF8;}{struct Cyc_String_pa_PrintArg_struct _TF8=_TD3;void*_TF9[1];_TD6=_TF9 + 0;*_TD6=& _TF8;_TD7=_tag_fat(" -o %s",sizeof(char),7U);_TD8=_tag_fat(_TF9,sizeof(void*),1);_TD2=Cyc_aprintf(_TD7,_TD8);}outfile_str=_TD2;goto _TL1A7;_TL1A6: _TL1A7:{struct Cyc_String_pa_PrintArg_struct _TF8;_TF8.tag=0;
# 1636
_TF8.f1=cyclone_cc;_TDA=_TF8;}{struct Cyc_String_pa_PrintArg_struct _TF8=_TDA;{struct Cyc_String_pa_PrintArg_struct _TF9;_TF9.tag=0;_TF9.f1=target_cflags;_TDB=_TF9;}{struct Cyc_String_pa_PrintArg_struct _TF9=_TDB;{struct Cyc_String_pa_PrintArg_struct _TFA;_TFA.tag=0;
_TFA.f1=ccargs_string;_TDC=_TFA;}{struct Cyc_String_pa_PrintArg_struct _TFA=_TDC;{struct Cyc_String_pa_PrintArg_struct _TFB;_TFB.tag=0;_TFB.f1=outfile_str;_TDD=_TFB;}{struct Cyc_String_pa_PrintArg_struct _TFB=_TDD;{struct Cyc_String_pa_PrintArg_struct _TFC;_TFC.tag=0;
_TFC.f1=stdlib_string;_TDE=_TFC;}{struct Cyc_String_pa_PrintArg_struct _TFC=_TDE;{struct Cyc_String_pa_PrintArg_struct _TFD;_TFD.tag=0;_TFD.f1=libargs_string;_TDF=_TFD;}{struct Cyc_String_pa_PrintArg_struct _TFD=_TDF;void*_TFE[6];_TE0=_TFE + 0;*_TE0=& _TF8;_TE1=_TFE + 1;*_TE1=& _TF9;_TE2=_TFE + 2;*_TE2=& _TFA;_TE3=_TFE + 3;*_TE3=& _TFB;_TE4=_TFE + 4;*_TE4=& _TFC;_TE5=_TFE + 5;*_TE5=& _TFD;_TE6=
# 1635
_tag_fat("%s %s %s%s %s%s",sizeof(char),16U);_TE7=_tag_fat(_TFE,sizeof(void*),6);_TD9=Cyc_aprintf(_TE6,_TE7);}}}}}}{
# 1634
struct _fat_ptr cccmd=_TD9;_TE8=Cyc_Flags_verbose;
# 1639
if(!_TE8)goto _TL1A8;{struct Cyc_String_pa_PrintArg_struct _TF8;_TF8.tag=0;_TF8.f1=cccmd;_TE9=_TF8;}{struct Cyc_String_pa_PrintArg_struct _TF8=_TE9;void*_TF9[1];_TEA=_TF9 + 0;*_TEA=& _TF8;_TEB=Cyc_stderr;_TEC=_tag_fat("%s\n",sizeof(char),4U);_TED=_tag_fat(_TF9,sizeof(void*),1);Cyc_fprintf(_TEB,_TEC,_TED);}Cyc_fflush(Cyc_stderr);goto _TL1A9;_TL1A8: _TL1A9: _TEE=cccmd;_TEF=_untag_fat_ptr_check_bound(_TEE,sizeof(char),1U);_TF0=_check_null(_TEF);_TF1=(const char*)_TF0;_TF2=
system(_TF1);if(_TF2==0)goto _TL1AA;_TF3=Cyc_stderr;_TF4=
_tag_fat("Error: C compiler failed\n",sizeof(char),26U);_TF5=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TF3,_TF4,_TF5);
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;_TL1AA:
# 1646
 Cyc_remove_cfiles();_TF7=Cyc_compile_failure;
# 1648
if(!_TF7)goto _TL1AC;_TF6=1;goto _TL1AD;_TL1AC: _TF6=0;_TL1AD: return _TF6;}}}}}}}}}}}}}}

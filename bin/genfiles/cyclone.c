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
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

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
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ({ \
  struct _fat_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char*)0) \
    _throw_arraybounds(); \
  _curr; })
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

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, unsigned orig_s) {  
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
  return _region_malloc(r,orig_s); 
}

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};
# 292 "core.h"
void Cyc_Core_rethrow(void*);
# 296
const char*Cyc_Core_get_exn_name(void*);
# 298
const char*Cyc_Core_get_exn_filename (void);
# 305
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
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
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
# 104 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 109
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);
# 120
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
# 27 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);
# 45
extern int Cyc_Position_use_gcc_style_location;
# 47
extern int Cyc_Position_max_errors;
# 49
extern int Cyc_Position_error_p (void);
# 24 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 38
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
# 27 "toseqc.h"
extern struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*);
# 29 "low_treeshake.h"
struct Cyc_List_List*Cyc_LowTreeShake_shake(struct Cyc_List_List*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*);
# 31 "warn.h"
void Cyc_Warn_flush_warnings (void);
# 40 "insert_checks.h"
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*,int);struct Cyc_Interface_I;
# 36 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_empty (void);
# 45 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final (void);
# 49
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple12{struct _fat_ptr f1;struct _fat_ptr f2;};
# 57
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*,struct Cyc_Interface_I*,struct _tuple12*);
# 72 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*)(void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 26 "flags.h"
extern int Cyc_Flags_porting_c_code;
# 30
extern int Cyc_Flags_no_regions;
# 34
extern int Cyc_Flags_no_register;
# 37
extern int Cyc_Flags_warn_override;
# 47
extern int Cyc_Flags_warn_alias_coerce;
# 51
extern int Cyc_Flags_warn_region_coerce;
# 54
extern int Cyc_Flags_warn_lose_unique;
# 58
extern int Cyc_Flags_warn_bounds_checks;
# 62
extern int Cyc_Flags_warn_all_null_deref;
# 66
extern int Cyc_Flags_noexpand_r;
# 71
extern int Cyc_Flags_allpaths;
# 80
extern int Cyc_Flags_tovc_r;
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 85
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 95 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_TempRename =12U,Cyc_Flags_EvalOrder =13U,Cyc_Flags_CCompiler =14U,Cyc_Flags_AllPasses =15U};
# 113
extern enum Cyc_Flags_Cyclone_Passes Cyc_Flags_stop_after_pass;
# 115
void Cyc_Flags_set_cpponly (void);
void Cyc_Flags_set_parseonly (void);
void Cyc_Flags_set_crgnonly (void);
void Cyc_Flags_set_tconly (void);
void Cyc_Flags_set_cfonly (void);
void Cyc_Flags_set_toconly (void);
# 126
extern int Cyc_Flags_compile_for_boot;
# 130
extern int Cyc_Flags_print_parser_state_and_token;
# 133
void Cyc_Flags_set_all_warnings (void);
# 29 "specsfile.h"
struct _fat_ptr Cyc_Specsfile_target_arch;
void Cyc_Specsfile_set_target_arch(struct _fat_ptr);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr);
# 34
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr);
struct _fat_ptr Cyc_Specsfile_split_specs(struct _fat_ptr);
struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*,struct _fat_ptr);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _fat_ptr Cyc_Specsfile_def_lib_path;
struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 44
struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr);
struct _fat_ptr Cyc_Specsfile_find_in_exec_path(struct _fat_ptr);
# 38 "cifc.h"
void Cyc_Cifc_set_inst_tvar (void);
# 46 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 49
void Cyc_Lex_pos_init (void);
void Cyc_Lex_lex_init(int);
# 57
static int Cyc_pp_r=0;
static int Cyc_noshake_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_v_r=0;
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
# 84
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _fat_ptr s){
Cyc_ccargs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_ccargs;_Tmp0;});}
# 92
void Cyc_set_c_compiler(struct _fat_ptr s){
if(Cyc_strcmp(s,({const char*_Tmp0="vc";_tag_fat(_Tmp0,sizeof(char),3U);}))==0){
Cyc_Flags_c_compiler=1U;
Cyc_add_ccarg(({const char*_Tmp0="-DVC_C";_tag_fat(_Tmp0,sizeof(char),7U);}));}else{
# 97
if(Cyc_strcmp(s,({const char*_Tmp0="gcc";_tag_fat(_Tmp0,sizeof(char),4U);}))==0){
Cyc_Flags_c_compiler=0U;
Cyc_add_ccarg(({const char*_Tmp0="-DGCC_C";_tag_fat(_Tmp0,sizeof(char),8U);}));}}}
# 104
void Cyc_set_port_c_code (void){
Cyc_Flags_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_Flags_set_parseonly();}
# 111
static struct _fat_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _fat_ptr s){
Cyc_output_file=({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));*_Tmp0=s;_Tmp0;});}
# 116
static struct _fat_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _fat_ptr s){
Cyc_specified_interface=s;}
# 126
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Cversion;static char _TmpG0[1U]="";
# 131
static struct _fat_ptr Cyc_cpp={_TmpG0,_TmpG0,_TmpG0 + 1U};
static void Cyc_set_cpp(struct _fat_ptr s){
Cyc_cpp=s;}
# 136
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cppargs;_Tmp0;});}
# 141
static void Cyc_add_cpp_and_ccarg(struct _fat_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 146
static void Cyc_add_iprefix(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2="-iprefix %s";_tag_fat(_Tmp2,sizeof(char),12U);}),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 149
static void Cyc_add_iwithprefix(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2="-iwithprefix %s";_tag_fat(_Tmp2,sizeof(char),16U);}),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 152
static void Cyc_add_iwithprefixbefore(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2="-iwithprefixbefore %s";_tag_fat(_Tmp2,sizeof(char),22U);}),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 155
static void Cyc_add_isystem(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2="-isystem %s";_tag_fat(_Tmp2,sizeof(char),12U);}),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 158
static void Cyc_add_idirafter(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2="-idirafter %s";_tag_fat(_Tmp2,sizeof(char),14U);}),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 162
static void Cyc_set_many_errors (void){
Cyc_Position_max_errors=65535;}static char _TmpG1[20U]="_see_cycspecs_file_";
# 166
static struct _fat_ptr Cyc_def_inc_path={_TmpG1,_TmpG1,_TmpG1 + 20U};
# 169
static void Cyc_print_version (void){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({char*_Tmp2=Cversion;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(({const char*_Tmp2="The Cyclone compiler, version %s\n";_tag_fat(_Tmp2,sizeof(char),34U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({char*_Tmp2=Carch;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(({const char*_Tmp2="Compiled for architecture: %s\n";_tag_fat(_Tmp2,sizeof(char),31U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({char*_Tmp2=Cdef_lib_path;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(({const char*_Tmp2="Standard library directory: %s\n";_tag_fat(_Tmp2,sizeof(char),32U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
# 174
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_def_inc_path;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(({const char*_Tmp2="Standard include directory: %s\n";_tag_fat(_Tmp2,sizeof(char),32U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
exit(0);}
# 178
static int Cyc_is_cyclone_sourcefile(struct _fat_ptr s){
unsigned long len=Cyc_strlen(s);
if(len <= 4U)return 0;
return({struct _fat_ptr _Tmp0=_fat_ptr_plus(s,sizeof(char),(int)(len - 4U));Cyc_strcmp(_Tmp0,({const char*_Tmp1=".cyc";_tag_fat(_Tmp1,sizeof(char),5U);}));})==0;}
# 194 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _fat_ptr s){
if(Cyc_strcmp(s,({const char*_Tmp0="-lxml";_tag_fat(_Tmp0,sizeof(char),6U);}))==0)
Cyc_add_ccarg(Cyc_pa_r?({const char*_Tmp0="-lxml_a";_tag_fat(_Tmp0,sizeof(char),8U);}): s);else{
if(Cyc_strncmp(s,({const char*_Tmp0="-lcyc";_tag_fat(_Tmp0,sizeof(char),6U);}),5U)==0)
Cyc_add_ccarg(s);else{
# 201
if(Cyc_strncmp(s,({const char*_Tmp0="-lpthread";_tag_fat(_Tmp0,sizeof(char),10U);}),9U)==0)
Cyc_pthread_r=1;
Cyc_libargs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_libargs;_Tmp0;});}}}
# 207
static void Cyc_add_marg(struct _fat_ptr s){
Cyc_add_ccarg(s);}
# 211
static void Cyc_set_save_temps (void){
Cyc_save_temps_r=1;
Cyc_add_ccarg(({const char*_Tmp0="-save-temps";_tag_fat(_Tmp0,sizeof(char),12U);}));}
# 216
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies (void){
Cyc_Flags_stop_after_pass=0U;
Cyc_produce_dependencies=1;
Cyc_add_cpparg(({const char*_Tmp0="-M";_tag_fat(_Tmp0,sizeof(char),3U);}));}
# 223
static struct _fat_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _fat_ptr s){
Cyc_dependencies_target=({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));*_Tmp0=s;_Tmp0;});}
# 228
static void Cyc_set_stop_after_objectfile (void){
Cyc_stop_after_objectfile_r=1;
Cyc_add_ccarg(({const char*_Tmp0="-c";_tag_fat(_Tmp0,sizeof(char),3U);}));}
# 233
static void Cyc_set_nocppprecomp (void){
Cyc_add_cpp_and_ccarg(({const char*_Tmp0="-no-cpp-precomp";_tag_fat(_Tmp0,sizeof(char),16U);}));}
# 237
static void Cyc_clear_lineno (void){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 241
static void Cyc_set_elim_se (void){
Cyc_elim_se_r=1;}
# 244
static void Cyc_set_tovc (void){
Cyc_Flags_tovc_r=1;
Cyc_add_ccarg(({const char*_Tmp0="-DCYC_ANSI_OUTPUT";_tag_fat(_Tmp0,sizeof(char),18U);}));
Cyc_set_elim_se();}
# 249
static void Cyc_set_notoseqc (void){
Cyc_toseqc_r=0;}
# 252
static void Cyc_set_noboundschecks (void){
Cyc_add_ccarg(({const char*_Tmp0="-DNO_CYC_BOUNDS_CHECKS";_tag_fat(_Tmp0,sizeof(char),23U);}));}
# 255
static void Cyc_set_nonullchecks (void){
Cyc_add_ccarg(({const char*_Tmp0="-DNO_CYC_NULL_CHECKS";_tag_fat(_Tmp0,sizeof(char),21U);}));}
# 258
static void Cyc_set_nochecks (void){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 264
static void Cyc_set_nocyc (void){
Cyc_add_cyc_namespace_r=0;
Cyc_add_ccarg(({const char*_Tmp0="-DNO_CYC_PREFIX";_tag_fat(_Tmp0,sizeof(char),16U);}));}
# 269
static void Cyc_set_pa (void){
Cyc_pa_r=1;
Cyc_add_ccarg(({const char*_Tmp0="-DCYC_REGION_PROFILE";_tag_fat(_Tmp0,sizeof(char),21U);}));
Cyc_add_cpparg(({const char*_Tmp0="-DCYC_REGION_PROFILE";_tag_fat(_Tmp0,sizeof(char),21U);}));}
# 275
static void Cyc_set_pg (void){
Cyc_pg_r=1;
Cyc_add_ccarg(({const char*_Tmp0="-pg";_tag_fat(_Tmp0,sizeof(char),4U);}));}
# 280
static void Cyc_set_stop_after_asmfile (void){
Cyc_stop_after_asmfile_r=1;
Cyc_add_ccarg(({const char*_Tmp0="-S";_tag_fat(_Tmp0,sizeof(char),3U);}));}
# 286
enum Cyc_inputtype{Cyc_DEFAULTINPUT =0U,Cyc_CYCLONEFILE =1U};
# 290
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _fat_ptr s){
if(Cyc_strcmp(s,({const char*_Tmp0="cyc";_tag_fat(_Tmp0,sizeof(char),4U);}))==0)
Cyc_intype=1U;else{
if(Cyc_strcmp(s,({const char*_Tmp0="none";_tag_fat(_Tmp0,sizeof(char),5U);}))==0)
Cyc_intype=0U;else{
# 297
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Input type '%s' not supported\n";_tag_fat(_Tmp2,sizeof(char),31U);}),_tag_fat(_Tmp1,sizeof(void*),1));});}}}
# 300
struct _fat_ptr Cyc_make_base_filename(struct _fat_ptr s,struct _fat_ptr*output_file){
struct _fat_ptr outdir=Cyc_Filename_dirname(output_file==0?s:*output_file);
struct _fat_ptr base=Cyc_Filename_chop_extension(Cyc_Filename_basename(s));
# 306
struct _fat_ptr basename=Cyc_strlen(outdir)> 0U?Cyc_Filename_concat(outdir,base): base;
return basename;}
# 312
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 314
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _fat_ptr s){
# 317
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
if((int)Cyc_intype==1 || Cyc_is_cyclone_sourcefile(s)){
# 322
Cyc_cyclone_files=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cyclone_files;_Tmp0;});{
# 328
struct _fat_ptr basename=Cyc_make_base_filename(s,0);
struct _fat_ptr cfile=Cyc_strconcat(basename,({const char*_Tmp0=".c";_tag_fat(_Tmp0,sizeof(char),3U);}));
Cyc_add_ccarg(cfile);}}else{
# 332
Cyc_add_ccarg(s);}}}
# 337
static int Cyc_assume_gcc_flag=1;struct _tuple13{struct _fat_ptr flag;int optargs;};static char _TmpG2[3U]="-s";static char _TmpG3[3U]="-O";static char _TmpG4[4U]="-O0";static char _TmpG5[4U]="-O2";static char _TmpG6[4U]="-O3";static char _TmpG7[21U]="-fomit-frame-pointer";static char _TmpG8[13U]="-fno-builtin";static char _TmpG9[3U]="-g";static char _TmpGA[3U]="-p";static char _TmpGB[8U]="-static";
static int Cyc_add_other_flag(struct _fat_ptr s){
static struct _tuple13 known_gcc_flags[10U]={{.flag={_TmpG2,_TmpG2,_TmpG2 + 3U},.optargs=0},{.flag={_TmpG3,_TmpG3,_TmpG3 + 3U},.optargs=0},{.flag={_TmpG4,_TmpG4,_TmpG4 + 4U},.optargs=0},{.flag={_TmpG5,_TmpG5,_TmpG5 + 4U},.optargs=0},{.flag={_TmpG6,_TmpG6,_TmpG6 + 4U},.optargs=0},{.flag={_TmpG7,_TmpG7,_TmpG7 + 21U},.optargs=0},{.flag={_TmpG8,_TmpG8,_TmpG8 + 13U},.optargs=0},{.flag={_TmpG9,_TmpG9,_TmpG9 + 3U},.optargs=0},{.flag={_TmpGA,_TmpGA,_TmpGA + 3U},.optargs=0},{.flag={_TmpGB,_TmpGB,_TmpGB + 8U},.optargs=0}};
# 351
if(Cyc_assume_gcc_flag){
Cyc_add_ccarg(s);
return 1;}else{
# 355
{int i=0;for(0;(unsigned)i < 10U;++ i){
if(!Cyc_strcmp(known_gcc_flags[i].flag,s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=(*((struct _tuple13*)_check_known_subscript_notnull(known_gcc_flags,10U,sizeof(struct _tuple13),i))).optargs;
break;}}}
# 361
return 0;}}
# 365
static void Cyc_noassume_gcc_flag (void){
Cyc_assume_gcc_flag=0;}
# 369
static void Cyc_remove_file(struct _fat_ptr s){
if(!Cyc_save_temps_r)
remove((const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)));}
# 376
int Cyc_compile_failure=0;
# 378
struct Cyc___cycFILE*Cyc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){{struct Cyc___cycFILE*_Tmp2=Cyc_file_open(filename,mode);_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();
# 381
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=msg_part;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=filename;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="\nError: couldn't open %s %s\n";_tag_fat(_Tmp6,sizeof(char),29U);}),_tag_fat(_Tmp5,sizeof(void*),2));});
Cyc_fflush(Cyc_stderr);
return 0;;}}}struct _tuple14{void*f1;const char*f2;int f3;};
# 388
struct _tuple14*Cyc_uncaught_exn=0;
# 390
extern void CYCALLOCPROFILE_mark(const char*);
# 392
void*Cyc_do_stage(struct _fat_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void*onfail){
# 394
CYCALLOCPROFILE_mark((const char*)_untag_fat_ptr(stage_name,sizeof(char),1U));{
struct Cyc_timeval start;struct Cyc_timeval end;
if(Cyc_v_r)
gettimeofday(& start,0);{
# 399
void*ans;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){ans=f(env,tds);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;_Tmp3=_Tmp2;{void*x=_Tmp3;
# 402
ans=onfail;
if(Cyc_Position_error_p())
goto _LL0;
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=stage_name;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="COMPILER STAGE %s (unexpected exception) \n";_tag_fat(_Tmp6,sizeof(char),43U);}),_tag_fat(_Tmp5,sizeof(void*),1));});
Cyc_uncaught_exn=({struct _tuple14*_Tmp4=_cycalloc(sizeof(struct _tuple14));_Tmp4->f1=x,({const char*_Tmp5=Cyc_Core_get_exn_filename();_Tmp4->f2=_Tmp5;}),({int _Tmp5=Cyc_Core_get_exn_lineno();_Tmp4->f3=_Tmp5;});_Tmp4;});
goto _LL0;}_LL0:;}}}
# 410
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="\nCOMPILATION FAILED!\n";_tag_fat(_Tmp0,sizeof(char),22U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_fflush(Cyc_stderr);}else{
if(Cyc_v_r){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=stage_name;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="%s completed ";_tag_fat(_Tmp2,sizeof(char),14U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
gettimeofday(& end,0);
end.tv_sec -=start.tv_sec;
end.tv_usec -=start.tv_usec;
if(end.tv_usec < 0){
end.tv_usec +=1000000;
-- end.tv_sec;}
# 424
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)end.tv_sec;_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)end.tv_usec;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="(%d.%06d secs)\n";_tag_fat(_Tmp3,sizeof(char),16U);}),_tag_fat(_Tmp2,sizeof(void*),2));});
Cyc_fflush(Cyc_stderr);}}
# 427
return ans;}}}
# 430
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
ans=Cyc_Parse_parse_file(f);
Cyc_file_close(f);
# 435
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Parse_Exit_exn_struct*)_Tmp2)->tag==Cyc_Parse_Exit){
# 438
Cyc_file_close(f);Cyc_compile_failure=1;goto _LL0;}else{_Tmp3=_Tmp2;{void*e=_Tmp3;
Cyc_file_close(f);Cyc_Core_rethrow(e);}}_LL0:;}}}
# 441
Cyc_Lex_lex_init(1);
return ans;}}
# 445
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}
# 450
int Cyc_do_adjust(int ignore,struct Cyc_List_List*tds){
Cyc_CurRgn_adjust_all(tds);
return 1;}
# 455
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){
# 457
Cyc_Tc_tc(te,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(te,tds);
return tds;}
# 463
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){
# 465
return Cyc_JumpAnalysis_jump_analysis(tds);}
# 468
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 470
Cyc_NewControlFlow_cf_check(tables,tds);
return tds;}
# 474
int Cyc_do_insert_checks(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
Cyc_InsertChecks_insert_checks(tds,tables,Cyc_vcgen_r);
return 1;}struct _tuple15{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 481
struct Cyc_List_List*Cyc_do_interface(struct _tuple15*params,struct Cyc_List_List*tds){
# 483
void*_Tmp0;void*_Tmp1;void*_Tmp2;_Tmp2=params->f1;_Tmp1=params->f2;_Tmp0=params->f3;{struct Cyc_Tcenv_Tenv*te=_Tmp2;struct Cyc___cycFILE*fi=_Tmp1;struct Cyc___cycFILE*fo=_Tmp0;
struct Cyc_Interface_I*i1=Cyc_Interface_extract(te->ae,tds);
if(fi==0)
Cyc_Interface_save(i1,fo);else{
# 488
struct Cyc_Interface_I*i0=Cyc_Interface_parse(fi);
if(!({struct Cyc_Interface_I*_Tmp3=i0;struct Cyc_Interface_I*_Tmp4=i1;Cyc_Interface_is_subinterface(_Tmp3,_Tmp4,({struct _tuple12*_Tmp5=_cycalloc(sizeof(struct _tuple12));_Tmp5->f1=({const char*_Tmp6="written interface";_tag_fat(_Tmp6,sizeof(char),18U);}),_Tmp5->f2=({const char*_Tmp6="maximal interface";_tag_fat(_Tmp6,sizeof(char),18U);});_Tmp5;}));}))
Cyc_compile_failure=1;else{
# 492
Cyc_Interface_save(i0,fo);}}
# 494
return tds;}}
# 497
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){
# 500
return Cyc_Toc_toc(pops,tds);}
# 502
struct Cyc_List_List*Cyc_do_removeaggrs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveAggrs_remove_aggrs(tds);}
# 505
struct Cyc_List_List*Cyc_do_removelabs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveLabels_remove_unused_labels(tds);}
# 508
struct Cyc_List_List*Cyc_do_temprename(int ignore,struct Cyc_List_List*tds){
return Cyc_RenameTemps_rename(tds);}
# 512
struct Cyc_List_List*Cyc_do_toseqc(int ignore,struct Cyc_List_List*tds){
return Cyc_Toseqc_toseqc(tds);}
# 515
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Flags_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 519
struct Cyc_List_List*Cyc_do_lowtreeshake(int ignore,struct Cyc_List_List*tds){
return Cyc_LowTreeShake_shake(tds);}
# 523
static struct _fat_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 525
static struct _fat_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 527
static void Cyc_set_cyc_include(struct _fat_ptr f){
Cyc_cyc_include=f;}
# 532
int Cyc_copy_internal_file(struct _fat_ptr file,struct Cyc___cycFILE*out_file){
# 535
if((char*)file.curr==(char*)_tag_fat(0,0,0).curr){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="Internal error: copy_internal_file called with NULL\n";_tag_fat(_Tmp0,sizeof(char),53U);}),_tag_fat(0U,sizeof(void*),0));
return 1;}{
# 539
struct Cyc___cycFILE*file_f=Cyc_try_file_open(file,({const char*_Tmp0="r";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="internal compiler file";_tag_fat(_Tmp0,sizeof(char),23U);}));
if(file_f==0)return 1;{
unsigned long n_read=1024U;
unsigned long n_written;
char buf[1024U];({{unsigned _Tmp0=1024U;unsigned i;for(i=0;i < _Tmp0;++ i){buf[i]='\000';}}0;});
while(n_read==1024U){
n_read=Cyc_fread(_tag_fat(buf,sizeof(char),1024U),1U,1024U,file_f);
if(n_read!=1024U && !Cyc_feof(file_f)){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=file;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="\nError: problem copying %s\n";_tag_fat(_Tmp2,sizeof(char),28U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_fflush(Cyc_stderr);
return 1;}
# 552
n_written=Cyc_fwrite(_tag_fat(buf,sizeof(char),1024U),1U,n_read,out_file);
if(n_read!=n_written){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=file;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="\nError: problem copying %s\n";_tag_fat(_Tmp2,sizeof(char),28U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_fflush(Cyc_stderr);
return 1;}}
# 560
Cyc_fclose(file_f);
return 0;}}}
# 566
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles (void){
if(!Cyc_save_c_r)
for(1;Cyc_cfiles!=0;Cyc_cfiles=_check_null(Cyc_cfiles)->tl){
Cyc_remove_file(*((struct _fat_ptr*)_check_null(Cyc_cfiles)->hd));}}
# 572
static void Cyc_find_fail(struct _fat_ptr file){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
(void*)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,({struct _fat_ptr _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=file;_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="Error: can't find internal compiler file %s\n";_tag_fat(_Tmp4,sizeof(char),45U);}),_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp0->f1=_Tmp1;});_Tmp0;}));}
# 577
static struct _fat_ptr Cyc_find_in_arch_path(struct _fat_ptr s){
struct _fat_ptr r=Cyc_Specsfile_find_in_arch_path(s);
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}
# 582
static struct _fat_ptr Cyc_find_in_exec_path(struct _fat_ptr s){
struct _fat_ptr r=Cyc_Specsfile_find_in_exec_path(s);
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}struct _tuple16{struct Cyc___cycFILE*f1;struct _fat_ptr f2;};
# 588
struct Cyc_List_List*Cyc_do_print(struct _tuple16*env,struct Cyc_List_List*tds){
# 590
struct _fat_ptr _Tmp0;void*_Tmp1;_Tmp1=env->f1;_Tmp0=env->f2;{struct Cyc___cycFILE*out_file=_Tmp1;struct _fat_ptr cfile=_Tmp0;
struct Cyc_Absynpp_Params params_r=(int)Cyc_Flags_stop_after_pass < 9?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
params_r.expand_typedefs=!Cyc_Flags_noexpand_r;
params_r.to_VC=Cyc_Flags_tovc_r;
params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;
params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;
params_r.print_all_effects=Cyc_print_all_effects_r;
# 602
if((int)Cyc_Flags_stop_after_pass > 6){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Flags_compile_for_boot)
Cyc_fprintf(out_file,({const char*_Tmp2="#include <setjmp.h>\n";_tag_fat(_Tmp2,sizeof(char),21U);}),_tag_fat(0U,sizeof(void*),0));else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 608
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 610
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& params_r);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 614
Cyc_Absyndump_set_params(& params_r);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 617
Cyc_fflush(out_file);
return tds;}}
# 622
static struct Cyc_List_List*Cyc_split_by_char(struct _fat_ptr s,char c){
if((char*)s.curr==(char*)_tag_fat(0,0,0).curr)return 0;{
struct Cyc_List_List*result=0;
unsigned long len=Cyc_strlen(s);
# 627
while(len > 0U){
struct _fat_ptr end=Cyc_strchr(s,c);
if((char*)end.curr==(char*)_tag_fat(0,0,0).curr){
result=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=result;_Tmp0;});
break;}else{
# 633
result=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_substring(s,0,(unsigned long)((end.curr - s.curr)/ sizeof(char)));*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=result;_Tmp0;});
len -=(unsigned long)((end.curr - s.curr)/ sizeof(char));
s=_fat_ptr_plus(end,sizeof(char),1);}}
# 638
return Cyc_List_imp_rev(result);}}
# 643
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _fat_ptr subdir){
# 645
struct Cyc_List_List*l=0;
for(1;dirs!=0;dirs=dirs->tl){
l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_Filename_concat(*((struct _fat_ptr*)dirs->hd),subdir);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=l;_Tmp0;});}
l=Cyc_List_imp_rev(l);
return l;}
# 653
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
# 668
 goto _LL18;case 9: _LL18:
 return 1;default:
 return 0;};}
# 675
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long len=Cyc_strlen(s);
# 679
int single_quotes=0;
int other_special=0;
{int i=0;for(0;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c==39)++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 688
if(single_quotes==0 && other_special==0)
return s;
# 692
if(single_quotes==0)
return Cyc_strconcat_l(({struct _fat_ptr*_Tmp0[3];({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=({const char*_Tmp3="'";_tag_fat(_Tmp3,sizeof(char),2U);});_Tmp2;});_Tmp0[0]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0[1]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=({const char*_Tmp3="'";_tag_fat(_Tmp3,sizeof(char),2U);});_Tmp2;});_Tmp0[2]=_Tmp1;});Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _fat_ptr*),3));}));{
# 696
unsigned long len2=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;
struct _fat_ptr s2=({unsigned _Tmp0=(len2 + 1U)+ 1U;_tag_fat(({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=len2 + 1U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='\000';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;}),sizeof(char),_Tmp0);});
int i=0;
int j=0;
for(1;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c==39 || Cyc_is_other_special(c))
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\\';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 706
return s2;}}
# 708
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=Cyc_sh_escape_string(*sp);*_Tmp0=_Tmp1;});_Tmp0;});}
# 712
static void Cyc_process_file(struct _fat_ptr filename){
# 714
struct _fat_ptr basename=Cyc_make_base_filename(filename,Cyc_output_file);
struct _fat_ptr preprocfile=Cyc_strconcat(basename,({const char*_Tmp0=".cyp";_tag_fat(_Tmp0,sizeof(char),5U);}));
struct _fat_ptr interfacefile=(char*)Cyc_specified_interface.curr!=(char*)_tag_fat(0,0,0).curr?Cyc_specified_interface:
 Cyc_strconcat(basename,({const char*_Tmp0=".cyci";_tag_fat(_Tmp0,sizeof(char),6U);}));
struct _fat_ptr interfaceobjfile=Cyc_strconcat(basename,({const char*_Tmp0=".cycio";_tag_fat(_Tmp0,sizeof(char),7U);}));
# 722
struct _fat_ptr cfile=({struct _fat_ptr _Tmp0=Cyc_make_base_filename(filename,0);Cyc_strconcat(_Tmp0,({const char*_Tmp1=".c";_tag_fat(_Tmp1,sizeof(char),3U);}));});
struct _fat_ptr*preprocfileptr;preprocfileptr=_cycalloc(sizeof(struct _fat_ptr)),*preprocfileptr=preprocfile;
# 725
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=filename;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Compiling %s\n";_tag_fat(_Tmp2,sizeof(char),14U);}),_tag_fat(_Tmp1,sizeof(void*),1));});{
# 728
struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,({const char*_Tmp0="r";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="input file";_tag_fat(_Tmp0,sizeof(char),11U);}));
if(Cyc_compile_failure || !((unsigned)f0))
return;
Cyc_fclose(f0);{
# 735
struct _fat_ptr cppargs_string=({
struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=({const char*_Tmp4="";_tag_fat(_Tmp4,sizeof(char),1U);});_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,Cyc_List_rev(Cyc_cppargs));_Tmp1->tl=_Tmp2;});_Tmp1;});
# 736
Cyc_str_sepstr(_Tmp0,({const char*_Tmp1=" ";_tag_fat(_Tmp1,sizeof(char),2U);}));});
# 745
struct Cyc_List_List*stdinc_dirs=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);
stdinc_dirs=Cyc_add_subdir(stdinc_dirs,({const char*_Tmp0="include";_tag_fat(_Tmp0,sizeof(char),8U);}));
if(Cyc_strlen(Cyc_def_inc_path)> 0U)
stdinc_dirs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=Cyc_def_inc_path;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=stdinc_dirs;_Tmp0;});{
char*cyclone_include_path=getenv("CYCLONE_INCLUDE_PATH");
if(cyclone_include_path!=0)
stdinc_dirs=({struct Cyc_List_List*_Tmp0=Cyc_split_by_char(({char*_Tmp1=cyclone_include_path;_tag_fat((void*)_Tmp1,sizeof(char),_get_zero_arr_size_char((void*)_Tmp1,1U));}),':');Cyc_List_append(_Tmp0,stdinc_dirs);});{
struct _fat_ptr stdinc_string=({
struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=({const char*_Tmp4="";_tag_fat(_Tmp4,sizeof(char),1U);});_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,stdinc_dirs);_Tmp1->tl=_Tmp2;});_Tmp1;});
# 753
Cyc_str_sepstr(_Tmp0,({const char*_Tmp1=" -I";_tag_fat(_Tmp1,sizeof(char),4U);}));});
# 756
struct _fat_ptr ofile_string;
if((int)Cyc_Flags_stop_after_pass <= 0)
ofile_string=Cyc_output_file==0?({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}):({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=*_check_null(Cyc_output_file);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2=" > %s";_tag_fat(_Tmp2,sizeof(char),6U);}),_tag_fat(_Tmp1,sizeof(void*),1));});else{
# 760
ofile_string=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_sh_escape_string(preprocfile);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2=" > %s";_tag_fat(_Tmp2,sizeof(char),6U);}),_tag_fat(_Tmp1,sizeof(void*),1));});}{
# 762
struct _fat_ptr fixup_string;
if(Cyc_produce_dependencies){
# 766
if(Cyc_dependencies_target==0)
# 770
fixup_string=({const char*_Tmp0=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_fat(_Tmp0,sizeof(char),35U);});else{
# 775
fixup_string=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=*
_check_null(Cyc_dependencies_target);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_fat(_Tmp2,sizeof(char),29U);}),_tag_fat(_Tmp1,sizeof(void*),1));});}}else{
# 778
fixup_string=({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);});}{
# 780
struct _fat_ptr cmd=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_cpp;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=cppargs_string;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=stdinc_string;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
# 782
struct _fat_ptr _Tmp5=Cyc_sh_escape_string(filename);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=fixup_string;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=ofile_string;_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="%s %s%s %s%s%s";_tag_fat(_Tmp7,sizeof(char),15U);}),_tag_fat(_Tmp6,sizeof(void*),6));});
# 784
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cmd;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="%s\n";_tag_fat(_Tmp2,sizeof(char),4U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
if(system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)))!=0){
Cyc_compile_failure=1;
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="\nError: preprocessing\n";_tag_fat(_Tmp0,sizeof(char),23U);}),_tag_fat(0U,sizeof(void*),0));
if((int)Cyc_Flags_stop_after_pass > 0)Cyc_remove_file(preprocfile);
return;}
# 791
if((int)Cyc_Flags_stop_after_pass <= 0)return;
# 794
Cyc_Position_reset_position(preprocfile);{
struct Cyc___cycFILE*in_file=Cyc_try_file_open(preprocfile,({const char*_Tmp0="r";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="file";_tag_fat(_Tmp0,sizeof(char),5U);}));
# 797
struct Cyc_List_List*tds=0;
struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables=0;
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_tc_init();
# 802
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 1)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="parsing";_tag_fat(_Tmp0,sizeof(char),8U);}),tds,Cyc_do_parse,_check_null(in_file),tds);
# 806
if(!Cyc_compile_failure && Cyc_Flags_porting_c_code){
Cyc_Port_port(tds);
Cyc_remove_file(preprocfile);
return;}
# 813
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 2)
({(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage;})(({const char*_Tmp0="binding";_tag_fat(_Tmp0,sizeof(char),8U);}),tds,Cyc_do_binding,1,1);
# 817
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 3)
({(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage;})(({const char*_Tmp0="current region resolution";_tag_fat(_Tmp0,sizeof(char),26U);}),tds,Cyc_do_adjust,1,1);
# 821
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 4)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="type checking";_tag_fat(_Tmp0,sizeof(char),14U);}),tds,Cyc_do_typecheck,te,tds);
# 825
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 5)
jump_tables=({(struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(int,struct Cyc_List_List*),int,struct Cyc_JumpAnalysis_Jump_Anal_Result*))Cyc_do_stage;})(({const char*_Tmp0="jump checking";_tag_fat(_Tmp0,sizeof(char),14U);}),tds,Cyc_do_jumpanalysis,1,0);
# 829
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 6)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="control-flow checking";_tag_fat(_Tmp0,sizeof(char),22U);}),tds,Cyc_do_cfcheck,_check_null(jump_tables),tds);
# 833
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 8)
({(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,int))Cyc_do_stage;})(({const char*_Tmp0="check insertion";_tag_fat(_Tmp0,sizeof(char),16U);}),tds,Cyc_do_insert_checks,_check_null(jump_tables),1);
# 837
if((!Cyc_compile_failure && Cyc_generate_interface_r)&&(int)Cyc_Flags_stop_after_pass >= 9){
# 839
struct Cyc___cycFILE*inter_file=Cyc_try_file_open(({const char*_Tmp0=(const char*)_check_null(_untag_fat_ptr(interfacefile,sizeof(char),1U));_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}),({const char*_Tmp0="w";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="interface file";_tag_fat(_Tmp0,sizeof(char),15U);}));
if(inter_file==0)
Cyc_compile_failure=1;else{
# 843
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 850
if(!Cyc_compile_failure && Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_fat_ptr(interfacefile,sizeof(char),1U),"r");
struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open(interfaceobjfile,({const char*_Tmp0="w";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="interface object file";_tag_fat(_Tmp0,sizeof(char),22U);}));
if(inter_objfile==0)
Cyc_compile_failure=1;else{
# 856
Cyc_Position_reset_position(interfacefile);{
struct _tuple15 int_env=({struct _tuple15 _Tmp0;_Tmp0.f1=te,_Tmp0.f2=inter_file,_Tmp0.f3=inter_objfile;_Tmp0;});
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple15*,struct Cyc_List_List*),struct _tuple15*,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="interface checking";_tag_fat(_Tmp0,sizeof(char),19U);}),tds,Cyc_do_interface,& int_env,tds);
if(inter_file!=0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);}}}}
# 867
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="translation to C";_tag_fat(_Tmp0,sizeof(char),17U);}),tds,Cyc_do_translate,_check_null(jump_tables)->pop_tables,tds);
# 871
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 10)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="aggregate removal";_tag_fat(_Tmp0,sizeof(char),18U);}),tds,Cyc_do_removeaggrs,1,tds);
# 874
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 11)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="unused label removal";_tag_fat(_Tmp0,sizeof(char),21U);}),tds,Cyc_do_removelabs,1,tds);
# 877
if((!Cyc_compile_failure && Cyc_toseqc_r)&&(int)Cyc_Flags_stop_after_pass >= 13)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="L-to-R evaluation order";_tag_fat(_Tmp0,sizeof(char),24U);}),tds,Cyc_do_toseqc,1,tds);
# 880
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
Cyc_Toc_finish();
# 883
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="low treehsaking";_tag_fat(_Tmp0,sizeof(char),16U);}),tds,Cyc_do_lowtreeshake,1,tds);
# 886
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 12)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="renaming temporaries";_tag_fat(_Tmp0,sizeof(char),21U);}),tds,Cyc_do_temprename,1,tds);
# 889
if(!Cyc_compile_failure &&(Cyc_Flags_tovc_r || Cyc_elim_se_r))
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="post-pass to VC";_tag_fat(_Tmp0,sizeof(char),16U);}),tds,Cyc_do_tovc,1,tds);
# 892
if(!Cyc_compile_failure)
Cyc_Warn_flush_warnings();
Cyc_remove_file(preprocfile);
if(Cyc_uncaught_exn!=0){
struct _tuple14*_Tmp0=_check_null(Cyc_uncaught_exn);int _Tmp1;const char*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{void*e=_Tmp3;const char*fn=_Tmp2;int line=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=({const char*_Tmp7=Cyc_Core_get_exn_name(e);_tag_fat((void*)_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7=fn;_tag_fat((void*)_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)line;_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,({const char*_Tmp8="uncaught exception: %s from %s, line %d\n";_tag_fat(_Tmp8,sizeof(char),41U);}),_tag_fat(_Tmp7,sizeof(void*),3));});
Cyc_fflush(Cyc_stderr);
return;}}{
# 904
struct Cyc___cycFILE*out_file;
if((int)Cyc_Flags_stop_after_pass <= 6){
if(Cyc_output_file!=0)
out_file=Cyc_try_file_open(*_check_null(Cyc_output_file),({const char*_Tmp0="w";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="output file";_tag_fat(_Tmp0,sizeof(char),12U);}));else{
# 909
out_file=Cyc_stdout;}}else{
if((int)Cyc_Flags_stop_after_pass < 14 && Cyc_output_file!=0)
out_file=Cyc_try_file_open(*_check_null(Cyc_output_file),({const char*_Tmp0="w";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="output file";_tag_fat(_Tmp0,sizeof(char),12U);}));else{
# 913
out_file=Cyc_try_file_open(cfile,({const char*_Tmp0="w";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="output file";_tag_fat(_Tmp0,sizeof(char),12U);}));}}
# 915
if(Cyc_compile_failure || !((unsigned)out_file))return;
# 917
if(!Cyc_noprint_r){
struct _tuple16*env;env=_cycalloc(sizeof(struct _tuple16)),env->f1=out_file,env->f2=cfile;
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple16*,struct Cyc_List_List*),struct _tuple16*,struct Cyc_List_List*))Cyc_do_stage;})(({const char*_Tmp0="printing";_tag_fat(_Tmp0,sizeof(char),9U);}),tds,Cyc_do_print,env,tds);
Cyc_file_close(out_file);
Cyc_cfiles=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_strdup(cfile);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cfiles;_Tmp0;});}}}}}}}}}}static char _TmpGC[8U]="<final>";
# 925
static struct _fat_ptr Cyc_final_str={_TmpGC,_TmpGC,_TmpGC + 8U};
static struct _fat_ptr*Cyc_final_strptr=& Cyc_final_str;
# 928
static struct Cyc_Interface_I*Cyc_read_cycio(struct _fat_ptr*n){
if(n==Cyc_final_strptr)
return Cyc_Interface_final();{
struct _fat_ptr basename;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){basename=Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_Tmp2)->tag==Cyc_Core_Invalid_argument){
basename=*n;goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;(void*)_rethrow(exn);}}_LL0:;}}}{
# 935
struct _fat_ptr nf=Cyc_strconcat(basename,({const char*_Tmp0=".cycio";_tag_fat(_Tmp0,sizeof(char),7U);}));
struct Cyc___cycFILE*f=Cyc_try_file_open(nf,({const char*_Tmp0="rb";_tag_fat(_Tmp0,sizeof(char),3U);}),({const char*_Tmp0="interface object file";_tag_fat(_Tmp0,sizeof(char),22U);}));
if(f==0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 942
Cyc_Position_reset_position(nf);{
struct Cyc_Interface_I*i=Cyc_Interface_load(f);
Cyc_file_close(f);
return i;}}}}
# 948
static int Cyc_is_cfile(struct _fat_ptr*n){
return(int)*((const char*)_check_fat_subscript(*n,sizeof(char),0))!=45;}
# 952
extern void GC_blacklist_warn_clear (void);struct _tuple17{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
# 962
void Cyc_print_options (void);
# 970
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3,& Cyc_v_r};static char _TmpGD[3U]="-v";static char _TmpGE[1U]="";static char _TmpGF[35U]="print compilation stages verbosely";struct _tuple17 Cyc_opt1_tuple={{_TmpGD,_TmpGD,_TmpGD + 3U},0,{_TmpGE,_TmpGE,_TmpGE + 1U},(void*)& Cyc_opt1_spec,{_TmpGF,_TmpGF,_TmpGF + 35U}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 972
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0,Cyc_print_version};static char _TmpG10[10U]="--version";static char _TmpG11[1U]="";static char _TmpG12[35U]="Print version information and exit";struct _tuple17 Cyc_opt2_tuple={{_TmpG10,_TmpG10,_TmpG10 + 10U},0,{_TmpG11,_TmpG11,_TmpG11 + 1U},(void*)& Cyc_opt2_spec,{_TmpG12,_TmpG12,_TmpG12 + 35U}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 975
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5,Cyc_set_output_file};static char _TmpG13[3U]="-o";static char _TmpG14[8U]=" <file>";static char _TmpG15[35U]="Set the output file name to <file>";struct _tuple17 Cyc_opt3_tuple={{_TmpG13,_TmpG13,_TmpG13 + 3U},0,{_TmpG14,_TmpG14,_TmpG14 + 8U},(void*)& Cyc_opt3_spec,{_TmpG15,_TmpG15,_TmpG15 + 35U}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 978
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1,Cyc_add_cpparg};static char _TmpG16[3U]="-D";static char _TmpG17[17U]="<name>[=<value>]";static char _TmpG18[32U]="Pass definition to preprocessor";struct _tuple17 Cyc_opt4_tuple={{_TmpG16,_TmpG16,_TmpG16 + 3U},1,{_TmpG17,_TmpG17,_TmpG17 + 17U},(void*)& Cyc_opt4_spec,{_TmpG18,_TmpG18,_TmpG18 + 32U}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 981
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1,Cyc_Specsfile_add_cyclone_exec_path};static char _TmpG19[3U]="-B";static char _TmpG1A[7U]="<file>";static char _TmpG1B[60U]="Add to the list of directories to search for compiler files";struct _tuple17 Cyc_opt5_tuple={{_TmpG19,_TmpG19,_TmpG19 + 3U},1,{_TmpG1A,_TmpG1A,_TmpG1A + 7U},(void*)& Cyc_opt5_spec,{_TmpG1B,_TmpG1B,_TmpG1B + 60U}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 984
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1,Cyc_add_cpparg};static char _TmpG1C[3U]="-I";static char _TmpG1D[6U]="<dir>";static char _TmpG1E[59U]="Add to the list of directories to search for include files";struct _tuple17 Cyc_opt6_tuple={{_TmpG1C,_TmpG1C,_TmpG1C + 3U},1,{_TmpG1D,_TmpG1D,_TmpG1D + 6U},(void*)& Cyc_opt6_spec,{_TmpG1E,_TmpG1E,_TmpG1E + 59U}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 987
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1,Cyc_add_ccarg};static char _TmpG1F[3U]="-L";static char _TmpG20[6U]="<dir>";static char _TmpG21[38U]="Add to the list of directories for -l";struct _tuple17 Cyc_opt7_tuple={{_TmpG1F,_TmpG1F,_TmpG1F + 3U},1,{_TmpG20,_TmpG20,_TmpG20 + 6U},(void*)& Cyc_opt7_spec,{_TmpG21,_TmpG21,_TmpG21 + 38U}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 990
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1,Cyc_add_libarg};static char _TmpG22[3U]="-l";static char _TmpG23[10U]="<libname>";static char _TmpG24[13U]="Library file";struct _tuple17 Cyc_opt8_tuple={{_TmpG22,_TmpG22,_TmpG22 + 3U},1,{_TmpG23,_TmpG23,_TmpG23 + 10U},(void*)& Cyc_opt8_spec,{_TmpG24,_TmpG24,_TmpG24 + 13U}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 993
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1,Cyc_add_marg};static char _TmpG25[3U]="-m";static char _TmpG26[9U]="<option>";static char _TmpG27[52U]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple17 Cyc_opt9_tuple={{_TmpG25,_TmpG25,_TmpG25 + 3U},1,{_TmpG26,_TmpG26,_TmpG26 + 9U},(void*)& Cyc_opt9_spec,{_TmpG27,_TmpG27,_TmpG27 + 52U}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 996
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0,Cyc_set_stop_after_objectfile};static char _TmpG28[3U]="-c";static char _TmpG29[1U]="";static char _TmpG2A[61U]="Produce an object file instead of an executable; do not link";struct _tuple17 Cyc_opt10_tuple={{_TmpG28,_TmpG28,_TmpG28 + 3U},0,{_TmpG29,_TmpG29,_TmpG29 + 1U},(void*)& Cyc_opt10_spec,{_TmpG2A,_TmpG2A,_TmpG2A + 61U}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 999
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5,Cyc_set_inputtype};static char _TmpG2B[3U]="-x";static char _TmpG2C[12U]=" <language>";static char _TmpG2D[49U]="Specify <language> for the following input files";struct _tuple17 Cyc_opt11_tuple={{_TmpG2B,_TmpG2B,_TmpG2B + 3U},0,{_TmpG2C,_TmpG2C,_TmpG2C + 12U},(void*)& Cyc_opt11_spec,{_TmpG2D,_TmpG2D,_TmpG2D + 49U}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 1002
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0,Cyc_set_pa};static char _TmpG2E[4U]="-pa";static char _TmpG2F[1U]="";static char _TmpG30[33U]="Compile for profiling with aprof";struct _tuple17 Cyc_opt12_tuple={{_TmpG2E,_TmpG2E,_TmpG2E + 4U},0,{_TmpG2F,_TmpG2F,_TmpG2F + 1U},(void*)& Cyc_opt12_spec,{_TmpG30,_TmpG30,_TmpG30 + 33U}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 1005
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0,Cyc_set_stop_after_asmfile};static char _TmpG31[3U]="-S";static char _TmpG32[1U]="";static char _TmpG33[35U]="Stop after producing assembly code";struct _tuple17 Cyc_opt13_tuple={{_TmpG31,_TmpG31,_TmpG31 + 3U},0,{_TmpG32,_TmpG32,_TmpG32 + 1U},(void*)& Cyc_opt13_spec,{_TmpG33,_TmpG33,_TmpG33 + 35U}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 1008
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0,Cyc_set_produce_dependencies};static char _TmpG34[3U]="-M";static char _TmpG35[1U]="";static char _TmpG36[21U]="Produce dependencies";struct _tuple17 Cyc_opt14_tuple={{_TmpG34,_TmpG34,_TmpG34 + 3U},0,{_TmpG35,_TmpG35,_TmpG35 + 1U},(void*)& Cyc_opt14_spec,{_TmpG36,_TmpG36,_TmpG36 + 21U}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1011
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1,Cyc_add_cpparg};static char _TmpG37[4U]="-MG";static char _TmpG38[1U]="";static char _TmpG39[68U]="When producing dependencies assume that missing files are generated";struct _tuple17 Cyc_opt15_tuple={{_TmpG37,_TmpG37,_TmpG37 + 4U},0,{_TmpG38,_TmpG38,_TmpG38 + 1U},(void*)& Cyc_opt15_spec,{_TmpG39,_TmpG39,_TmpG39 + 68U}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1015
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5,Cyc_set_dependencies_target};static char _TmpG3A[4U]="-MT";static char _TmpG3B[10U]=" <target>";static char _TmpG3C[29U]="Give target for dependencies";struct _tuple17 Cyc_opt16_tuple={{_TmpG3A,_TmpG3A,_TmpG3A + 4U},0,{_TmpG3B,_TmpG3B,_TmpG3B + 10U},(void*)& Cyc_opt16_spec,{_TmpG3C,_TmpG3C,_TmpG3C + 29U}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1018
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5,Cyc_Specsfile_set_target_arch};static char _TmpG3D[3U]="-b";static char _TmpG3E[10U]="<machine>";static char _TmpG3F[19U]="Set target machine";struct _tuple17 Cyc_opt17_tuple={{_TmpG3D,_TmpG3D,_TmpG3D + 3U},0,{_TmpG3E,_TmpG3E,_TmpG3E + 10U},(void*)& Cyc_opt17_spec,{_TmpG3F,_TmpG3F,_TmpG3F + 19U}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1021
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3,& Cyc_Flags_warn_lose_unique};static char _TmpG40[14U]="-Wlose-unique";static char _TmpG41[1U]="";static char _TmpG42[49U]="Try to warn when a unique pointer might get lost";struct _tuple17 Cyc_opt18_tuple={{_TmpG40,_TmpG40,_TmpG40 + 14U},0,{_TmpG41,_TmpG41,_TmpG41 + 1U},(void*)& Cyc_opt18_spec,{_TmpG42,_TmpG42,_TmpG42 + 49U}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1024
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3,& Cyc_Flags_warn_override};static char _TmpG43[11U]="-Woverride";static char _TmpG44[1U]="";static char _TmpG45[58U]="Warn when a local variable overrides an existing variable";struct _tuple17 Cyc_opt19_tuple={{_TmpG43,_TmpG43,_TmpG43 + 11U},0,{_TmpG44,_TmpG44,_TmpG44 + 1U},(void*)& Cyc_opt19_spec,{_TmpG45,_TmpG45,_TmpG45 + 58U}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1027
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0,Cyc_Flags_set_all_warnings};static char _TmpG46[6U]="-Wall";static char _TmpG47[1U]="";static char _TmpG48[21U]="Turn on all warnings";struct _tuple17 Cyc_opt20_tuple={{_TmpG46,_TmpG46,_TmpG46 + 6U},0,{_TmpG47,_TmpG47,_TmpG47 + 1U},(void*)& Cyc_opt20_spec,{_TmpG48,_TmpG48,_TmpG48 + 21U}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1030
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt21_spec={0,Cyc_Flags_set_cpponly};static char _TmpG49[3U]="-E";static char _TmpG4A[1U]="";static char _TmpG4B[25U]="Stop after preprocessing";struct _tuple17 Cyc_opt21_tuple={{_TmpG49,_TmpG49,_TmpG49 + 3U},0,{_TmpG4A,_TmpG4A,_TmpG4A + 1U},(void*)& Cyc_opt21_spec,{_TmpG4B,_TmpG4B,_TmpG4B + 25U}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1033
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt22_spec={0,Cyc_Flags_set_parseonly};static char _TmpG4C[17U]="-stopafter-parse";static char _TmpG4D[1U]="";static char _TmpG4E[19U]="Stop after parsing";struct _tuple17 Cyc_opt22_tuple={{_TmpG4C,_TmpG4C,_TmpG4C + 17U},0,{_TmpG4D,_TmpG4D,_TmpG4D + 1U},(void*)& Cyc_opt22_spec,{_TmpG4E,_TmpG4E,_TmpG4E + 19U}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1036
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt23_spec={0,Cyc_Flags_set_tconly};static char _TmpG4F[14U]="-stopafter-tc";static char _TmpG50[1U]="";static char _TmpG51[25U]="Stop after type checking";struct _tuple17 Cyc_opt23_tuple={{_TmpG4F,_TmpG4F,_TmpG4F + 14U},0,{_TmpG50,_TmpG50,_TmpG50 + 1U},(void*)& Cyc_opt23_spec,{_TmpG51,_TmpG51,_TmpG51 + 25U}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1039
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3,& Cyc_noprint_r};static char _TmpG52[9U]="-noprint";static char _TmpG53[1U]="";static char _TmpG54[42U]="Do not print output (when stopping early)";struct _tuple17 Cyc_opt24_tuple={{_TmpG52,_TmpG52,_TmpG52 + 9U},0,{_TmpG53,_TmpG53,_TmpG53 + 1U},(void*)& Cyc_opt24_spec,{_TmpG54,_TmpG54,_TmpG54 + 42U}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1042
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt25_spec={0,Cyc_Flags_set_cfonly};static char _TmpG55[14U]="-stopafter-cf";static char _TmpG56[1U]="";static char _TmpG57[33U]="Stop after control-flow checking";struct _tuple17 Cyc_opt25_tuple={{_TmpG55,_TmpG55,_TmpG55 + 14U},0,{_TmpG56,_TmpG56,_TmpG56 + 1U},(void*)& Cyc_opt25_spec,{_TmpG57,_TmpG57,_TmpG57 + 33U}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1045
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt26_spec={0,Cyc_Flags_set_toconly};static char _TmpG58[15U]="-stopafter-toc";static char _TmpG59[1U]="";static char _TmpG5A[28U]="Stop after translation to C";struct _tuple17 Cyc_opt26_tuple={{_TmpG58,_TmpG58,_TmpG58 + 15U},0,{_TmpG59,_TmpG59,_TmpG59 + 1U},(void*)& Cyc_opt26_spec,{_TmpG5A,_TmpG5A,_TmpG5A + 28U}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1048
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3,& Cyc_ic_r};static char _TmpG5B[4U]="-ic";static char _TmpG5C[1U]="";static char _TmpG5D[26U]="Activate the link-checker";struct _tuple17 Cyc_opt27_tuple={{_TmpG5B,_TmpG5B,_TmpG5B + 4U},0,{_TmpG5C,_TmpG5C,_TmpG5C + 1U},(void*)& Cyc_opt27_spec,{_TmpG5D,_TmpG5D,_TmpG5D + 26U}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1051
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3,& Cyc_pp_r};static char _TmpG5E[4U]="-pp";static char _TmpG5F[1U]="";static char _TmpG60[47U]="Pretty print the C code that Cyclone generates";struct _tuple17 Cyc_opt28_tuple={{_TmpG5E,_TmpG5E,_TmpG5E + 4U},0,{_TmpG5F,_TmpG5F,_TmpG5F + 1U},(void*)& Cyc_opt28_spec,{_TmpG60,_TmpG60,_TmpG60 + 47U}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1054
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4,& Cyc_pp_r};static char _TmpG61[4U]="-up";static char _TmpG62[1U]="";static char _TmpG63[55U]="Ugly print the C code that Cyclone generates (default)";struct _tuple17 Cyc_opt29_tuple={{_TmpG61,_TmpG61,_TmpG61 + 4U},0,{_TmpG62,_TmpG62,_TmpG62 + 1U},(void*)& Cyc_opt29_spec,{_TmpG63,_TmpG63,_TmpG63 + 55U}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1057
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3,& Cyc_elim_se_r};static char _TmpG64[28U]="-elim-statement-expressions";static char _TmpG65[1U]="";static char _TmpG66[66U]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple17 Cyc_opt30_tuple={{_TmpG64,_TmpG64,_TmpG64 + 28U},0,{_TmpG65,_TmpG65,_TmpG65 + 1U},(void*)& Cyc_opt30_spec,{_TmpG66,_TmpG66,_TmpG66 + 66U}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1061
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4,& Cyc_elim_se_r};static char _TmpG67[31U]="-no-elim-statement-expressions";static char _TmpG68[1U]="";static char _TmpG69[47U]="Do not avoid statement expressions in C output";struct _tuple17 Cyc_opt31_tuple={{_TmpG67,_TmpG67,_TmpG67 + 31U},0,{_TmpG68,_TmpG68,_TmpG68 + 1U},(void*)& Cyc_opt31_spec,{_TmpG69,_TmpG69,_TmpG69 + 47U}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1064
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0,Cyc_set_tovc};static char _TmpG6A[8U]="-un-gcc";static char _TmpG6B[1U]="";static char _TmpG6C[57U]="Avoid gcc extensions in C output (except for attributes)";struct _tuple17 Cyc_opt32_tuple={{_TmpG6A,_TmpG6A,_TmpG6A + 8U},0,{_TmpG6B,_TmpG6B,_TmpG6B + 1U},(void*)& Cyc_opt32_spec,{_TmpG6C,_TmpG6C,_TmpG6C + 57U}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1067
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5,Cyc_set_c_compiler};static char _TmpG6D[8U]="-c-comp";static char _TmpG6E[12U]=" <compiler>";static char _TmpG6F[40U]="Produce C output for the given compiler";struct _tuple17 Cyc_opt33_tuple={{_TmpG6D,_TmpG6D,_TmpG6D + 8U},0,{_TmpG6E,_TmpG6E,_TmpG6E + 12U},(void*)& Cyc_opt33_spec,{_TmpG6F,_TmpG6F,_TmpG6F + 40U}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1070
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0,Cyc_set_save_temps};static char _TmpG70[12U]="-save-temps";static char _TmpG71[1U]="";static char _TmpG72[29U]="Don't delete temporary files";struct _tuple17 Cyc_opt34_tuple={{_TmpG70,_TmpG70,_TmpG70 + 12U},0,{_TmpG71,_TmpG71,_TmpG71 + 1U},(void*)& Cyc_opt34_spec,{_TmpG72,_TmpG72,_TmpG72 + 29U}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1073
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3,& Cyc_save_c_r};static char _TmpG73[8U]="-save-c";static char _TmpG74[1U]="";static char _TmpG75[31U]="Don't delete temporary C files";struct _tuple17 Cyc_opt35_tuple={{_TmpG73,_TmpG73,_TmpG73 + 8U},0,{_TmpG74,_TmpG74,_TmpG74 + 1U},(void*)& Cyc_opt35_spec,{_TmpG75,_TmpG75,_TmpG75 + 31U}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1076
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0,Cyc_clear_lineno};static char _TmpG76[11U]="--nolineno";static char _TmpG77[1U]="";static char _TmpG78[63U]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple17 Cyc_opt36_tuple={{_TmpG76,_TmpG76,_TmpG76 + 11U},0,{_TmpG77,_TmpG77,_TmpG77 + 1U},(void*)& Cyc_opt36_spec,{_TmpG78,_TmpG78,_TmpG78 + 63U}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1079
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0,Cyc_set_nochecks};static char _TmpG79[11U]="--nochecks";static char _TmpG7A[1U]="";static char _TmpG7B[27U]="Disable bounds/null checks";struct _tuple17 Cyc_opt37_tuple={{_TmpG79,_TmpG79,_TmpG79 + 11U},0,{_TmpG7A,_TmpG7A,_TmpG7A + 1U},(void*)& Cyc_opt37_spec,{_TmpG7B,_TmpG7B,_TmpG7B + 27U}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1082
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0,Cyc_set_nonullchecks};static char _TmpG7C[15U]="--nonullchecks";static char _TmpG7D[1U]="";static char _TmpG7E[20U]="Disable null checks";struct _tuple17 Cyc_opt38_tuple={{_TmpG7C,_TmpG7C,_TmpG7C + 15U},0,{_TmpG7D,_TmpG7D,_TmpG7D + 1U},(void*)& Cyc_opt38_spec,{_TmpG7E,_TmpG7E,_TmpG7E + 20U}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1085
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0,Cyc_set_noboundschecks};static char _TmpG7F[17U]="--noboundschecks";static char _TmpG80[1U]="";static char _TmpG81[22U]="Disable bounds checks";struct _tuple17 Cyc_opt39_tuple={{_TmpG7F,_TmpG7F,_TmpG7F + 17U},0,{_TmpG80,_TmpG80,_TmpG80 + 1U},(void*)& Cyc_opt39_spec,{_TmpG81,_TmpG81,_TmpG81 + 22U}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1088
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5,Cyc_set_cpp};static char _TmpG82[9U]="-use-cpp";static char _TmpG83[7U]="<path>";static char _TmpG84[35U]="Indicate which preprocessor to use";struct _tuple17 Cyc_opt41_tuple={{_TmpG82,_TmpG82,_TmpG82 + 9U},0,{_TmpG83,_TmpG83,_TmpG83 + 7U},(void*)& Cyc_opt41_spec,{_TmpG84,_TmpG84,_TmpG84 + 35U}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1091
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0,Cyc_set_nocppprecomp};static char _TmpG85[16U]="-no-cpp-precomp";static char _TmpG86[1U]="";static char _TmpG87[40U]="Don't do smart preprocessing (mac only)";struct _tuple17 Cyc_opt42_tuple={{_TmpG85,_TmpG85,_TmpG85 + 16U},0,{_TmpG86,_TmpG86,_TmpG86 + 1U},(void*)& Cyc_opt42_spec,{_TmpG87,_TmpG87,_TmpG87 + 40U}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1094
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0,Cyc_set_nocyc};static char _TmpG88[7U]="-nocyc";static char _TmpG89[1U]="";static char _TmpG8A[33U]="Don't add implicit namespace Cyc";struct _tuple17 Cyc_opt43_tuple={{_TmpG88,_TmpG88,_TmpG88 + 7U},0,{_TmpG89,_TmpG89,_TmpG89 + 1U},(void*)& Cyc_opt43_spec,{_TmpG8A,_TmpG8A,_TmpG8A + 33U}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1097
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3,& Cyc_nogc_r};static char _TmpG8B[6U]="-nogc";static char _TmpG8C[1U]="";static char _TmpG8D[36U]="Don't link in the garbage collector";struct _tuple17 Cyc_opt44_tuple={{_TmpG8B,_TmpG8B,_TmpG8B + 6U},0,{_TmpG8C,_TmpG8C,_TmpG8C + 1U},(void*)& Cyc_opt44_spec,{_TmpG8D,_TmpG8D,_TmpG8D + 36U}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1100
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3,& Cyc_noshake_r};static char _TmpG8E[16U]="-noremoveunused";static char _TmpG8F[1U]="";static char _TmpG90[49U]="Don't remove externed variables that aren't used";struct _tuple17 Cyc_opt45_tuple={{_TmpG8E,_TmpG8E,_TmpG8E + 16U},0,{_TmpG8F,_TmpG8F,_TmpG8F + 1U},(void*)& Cyc_opt45_spec,{_TmpG90,_TmpG90,_TmpG90 + 49U}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1103
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3,& Cyc_Flags_noexpand_r};static char _TmpG91[18U]="-noexpandtypedefs";static char _TmpG92[1U]="";static char _TmpG93[41U]="Don't expand typedefs in pretty printing";struct _tuple17 Cyc_opt46_tuple={{_TmpG91,_TmpG91,_TmpG91 + 18U},0,{_TmpG92,_TmpG92,_TmpG92 + 1U},(void*)& Cyc_opt46_spec,{_TmpG93,_TmpG93,_TmpG93 + 41U}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1106
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3,& Cyc_print_all_tvars_r};static char _TmpG94[15U]="-printalltvars";static char _TmpG95[1U]="";static char _TmpG96[57U]="Print all type variables (even implicit default effects)";struct _tuple17 Cyc_opt47_tuple={{_TmpG94,_TmpG94,_TmpG94 + 15U},0,{_TmpG95,_TmpG95,_TmpG95 + 1U},(void*)& Cyc_opt47_spec,{_TmpG96,_TmpG96,_TmpG96 + 57U}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1109
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3,& Cyc_print_all_kinds_r};static char _TmpG97[15U]="-printallkinds";static char _TmpG98[1U]="";static char _TmpG99[37U]="Always print kinds of type variables";struct _tuple17 Cyc_opt48_tuple={{_TmpG97,_TmpG97,_TmpG97 + 15U},0,{_TmpG98,_TmpG98,_TmpG98 + 1U},(void*)& Cyc_opt48_spec,{_TmpG99,_TmpG99,_TmpG99 + 37U}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1112
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3,& Cyc_print_full_evars_r};static char _TmpG9A[16U]="-printfullevars";static char _TmpG9B[1U]="";static char _TmpG9C[50U]="Print full information for evars (type debugging)";struct _tuple17 Cyc_opt49_tuple={{_TmpG9A,_TmpG9A,_TmpG9A + 16U},0,{_TmpG9B,_TmpG9B,_TmpG9B + 1U},(void*)& Cyc_opt49_spec,{_TmpG9C,_TmpG9C,_TmpG9C + 50U}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1115
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3,& Cyc_print_all_effects_r};static char _TmpG9D[17U]="-printalleffects";static char _TmpG9E[1U]="";static char _TmpG9F[45U]="Print effects for functions (type debugging)";struct _tuple17 Cyc_opt50_tuple={{_TmpG9D,_TmpG9D,_TmpG9D + 17U},0,{_TmpG9E,_TmpG9E,_TmpG9E + 1U},(void*)& Cyc_opt50_spec,{_TmpG9F,_TmpG9F,_TmpG9F + 45U}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1118
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3,& Cyc_nocyc_setjmp_r};static char _TmpGA0[14U]="-nocyc_setjmp";static char _TmpGA1[1U]="";static char _TmpGA2[46U]="Do not use compiler special file cyc_setjmp.h";struct _tuple17 Cyc_opt51_tuple={{_TmpGA0,_TmpGA0,_TmpGA0 + 14U},0,{_TmpGA1,_TmpGA1,_TmpGA1 + 1U},(void*)& Cyc_opt51_spec,{_TmpGA2,_TmpGA2,_TmpGA2 + 46U}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1121
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3,& Cyc_Flags_compile_for_boot};static char _TmpGA3[18U]="-compile-for-boot";static char _TmpGA4[1U]="";static char _TmpGA5[71U]="Compile using the special boot library instead of the standard library";struct _tuple17 Cyc_opt52_tuple={{_TmpGA3,_TmpGA3,_TmpGA3 + 18U},0,{_TmpGA4,_TmpGA4,_TmpGA4 + 1U},(void*)& Cyc_opt52_spec,{_TmpGA5,_TmpGA5,_TmpGA5 + 71U}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1124
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5,Cyc_set_cyc_include};static char _TmpGA6[4U]="-CI";static char _TmpGA7[8U]=" <file>";static char _TmpGA8[31U]="Set cyc_include.h to be <file>";struct _tuple17 Cyc_opt53_tuple={{_TmpGA6,_TmpGA6,_TmpGA6 + 4U},0,{_TmpGA7,_TmpGA7,_TmpGA7 + 8U},(void*)& Cyc_opt53_spec,{_TmpGA8,_TmpGA8,_TmpGA8 + 31U}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1127
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3,& Cyc_Flags_warn_bounds_checks};static char _TmpGA9[18U]="-warnboundschecks";static char _TmpGAA[1U]="";static char _TmpGAB[44U]="Warn when bounds checks can't be eliminated";struct _tuple17 Cyc_opt54_tuple={{_TmpGA9,_TmpGA9,_TmpGA9 + 18U},0,{_TmpGAA,_TmpGAA,_TmpGAA + 1U},(void*)& Cyc_opt54_spec,{_TmpGAB,_TmpGAB,_TmpGAB + 44U}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1130
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3,& Cyc_Flags_warn_all_null_deref};static char _TmpGAC[16U]="-warnnullchecks";static char _TmpGAD[1U]="";static char _TmpGAE[45U]="Warn when any null check can't be eliminated";struct _tuple17 Cyc_opt55_tuple={{_TmpGAC,_TmpGAC,_TmpGAC + 16U},0,{_TmpGAD,_TmpGAD,_TmpGAD + 1U},(void*)& Cyc_opt55_spec,{_TmpGAE,_TmpGAE,_TmpGAE + 45U}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1133
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3,& Cyc_Flags_warn_alias_coerce};static char _TmpGAF[19U]="-warnaliascoercion";static char _TmpGB0[1U]="";static char _TmpGB1[41U]="Warn when any alias coercion is inserted";struct _tuple17 Cyc_opt56_tuple={{_TmpGAF,_TmpGAF,_TmpGAF + 19U},0,{_TmpGB0,_TmpGB0,_TmpGB0 + 1U},(void*)& Cyc_opt56_spec,{_TmpGB1,_TmpGB1,_TmpGB1 + 41U}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1136
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3,& Cyc_Flags_warn_region_coerce};static char _TmpGB2[18U]="-warnregioncoerce";static char _TmpGB3[1U]="";static char _TmpGB4[42U]="Warn when any region coercion is inserted";struct _tuple17 Cyc_opt57_tuple={{_TmpGB2,_TmpGB2,_TmpGB2 + 18U},0,{_TmpGB3,_TmpGB3,_TmpGB3 + 1U},(void*)& Cyc_opt57_spec,{_TmpGB4,_TmpGB4,_TmpGB4 + 42U}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1139
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3,& Cyc_Flags_no_register};static char _TmpGB5[12U]="-noregister";static char _TmpGB6[1U]="";static char _TmpGB7[39U]="Treat register storage class as public";struct _tuple17 Cyc_opt58_tuple={{_TmpGB5,_TmpGB5,_TmpGB5 + 12U},0,{_TmpGB6,_TmpGB6,_TmpGB6 + 1U},(void*)& Cyc_opt58_spec,{_TmpGB7,_TmpGB7,_TmpGB7 + 39U}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1142
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4,& Cyc_Position_use_gcc_style_location};static char _TmpGB8[18U]="-detailedlocation";static char _TmpGB9[1U]="";static char _TmpGBA[58U]="Try to give more detailed location information for errors";struct _tuple17 Cyc_opt59_tuple={{_TmpGB8,_TmpGB8,_TmpGB8 + 18U},0,{_TmpGB9,_TmpGB9,_TmpGB9 + 1U},(void*)& Cyc_opt59_spec,{_TmpGBA,_TmpGBA,_TmpGBA + 58U}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1145
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0,Cyc_set_port_c_code};static char _TmpGBB[6U]="-port";static char _TmpGBC[1U]="";static char _TmpGBD[38U]="Suggest how to port C code to Cyclone";struct _tuple17 Cyc_opt60_tuple={{_TmpGBB,_TmpGBB,_TmpGBB + 6U},0,{_TmpGBC,_TmpGBC,_TmpGBC + 1U},(void*)& Cyc_opt60_spec,{_TmpGBD,_TmpGBD,_TmpGBD + 38U}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1148
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3,& Cyc_Flags_no_regions};static char _TmpGBE[11U]="-noregions";static char _TmpGBF[1U]="";static char _TmpGC0[39U]="Generate code that doesn't use regions";struct _tuple17 Cyc_opt61_tuple={{_TmpGBE,_TmpGBE,_TmpGBE + 11U},0,{_TmpGBF,_TmpGBF,_TmpGBF + 1U},(void*)& Cyc_opt61_spec,{_TmpGC0,_TmpGC0,_TmpGC0 + 39U}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1151
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5,Cyc_add_iprefix};static char _TmpGC1[9U]="-iprefix";static char _TmpGC2[9U]="<prefix>";static char _TmpGC3[67U]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple17 Cyc_opt62_tuple={{_TmpGC1,_TmpGC1,_TmpGC1 + 9U},0,{_TmpGC2,_TmpGC2,_TmpGC2 + 9U},(void*)& Cyc_opt62_spec,{_TmpGC3,_TmpGC3,_TmpGC3 + 67U}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1154
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5,Cyc_add_iwithprefix};static char _TmpGC4[13U]="-iwithprefix";static char _TmpGC5[6U]="<dir>";static char _TmpGC6[47U]="Add <prefix>/<dir> to the second include path.";struct _tuple17 Cyc_opt63_tuple={{_TmpGC4,_TmpGC4,_TmpGC4 + 13U},0,{_TmpGC5,_TmpGC5,_TmpGC5 + 6U},(void*)& Cyc_opt63_spec,{_TmpGC6,_TmpGC6,_TmpGC6 + 47U}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1157
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5,Cyc_add_iwithprefixbefore};static char _TmpGC7[19U]="-iwithprefixbefore";static char _TmpGC8[6U]="<dir>";static char _TmpGC9[45U]="Add <prefix>/<dir> to the main include path.";struct _tuple17 Cyc_opt64_tuple={{_TmpGC7,_TmpGC7,_TmpGC7 + 19U},0,{_TmpGC8,_TmpGC8,_TmpGC8 + 6U},(void*)& Cyc_opt64_spec,{_TmpGC9,_TmpGC9,_TmpGC9 + 45U}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1160
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5,Cyc_add_isystem};static char _TmpGCA[9U]="-isystem";static char _TmpGCB[6U]="<dir>";static char _TmpGCC[90U]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple17 Cyc_opt65_tuple={{_TmpGCA,_TmpGCA,_TmpGCA + 9U},0,{_TmpGCB,_TmpGCB,_TmpGCB + 6U},(void*)& Cyc_opt65_spec,{_TmpGCC,_TmpGCC,_TmpGCC + 90U}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1163
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5,Cyc_add_idirafter};static char _TmpGCD[11U]="-idirafter";static char _TmpGCE[6U]="<dir>";static char _TmpGCF[46U]="Add the directory to the second include path.";struct _tuple17 Cyc_opt66_tuple={{_TmpGCD,_TmpGCD,_TmpGCD + 11U},0,{_TmpGCE,_TmpGCE,_TmpGCE + 6U},(void*)& Cyc_opt66_spec,{_TmpGCF,_TmpGCF,_TmpGCF + 46U}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1166
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3,& Cyc_generate_interface_r};static char _TmpGD0[15U]="--geninterface";static char _TmpGD1[1U]="";static char _TmpGD2[25U]="Generate interface files";struct _tuple17 Cyc_opt67_tuple={{_TmpGD0,_TmpGD0,_TmpGD0 + 15U},0,{_TmpGD1,_TmpGD1,_TmpGD1 + 1U},(void*)& Cyc_opt67_spec,{_TmpGD2,_TmpGD2,_TmpGD2 + 25U}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1169
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5,Cyc_set_specified_interface};static char _TmpGD3[12U]="--interface";static char _TmpGD4[8U]=" <file>";static char _TmpGD5[37U]="Set the interface file to be <file>.";struct _tuple17 Cyc_opt68_tuple={{_TmpGD3,_TmpGD3,_TmpGD3 + 12U},0,{_TmpGD4,_TmpGD4,_TmpGD4 + 8U},(void*)& Cyc_opt68_spec,{_TmpGD5,_TmpGD5,_TmpGD5 + 37U}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1172
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0,Cyc_set_many_errors};static char _TmpGD6[13U]="--manyerrors";static char _TmpGD7[1U]="";static char _TmpGD8[36U]="don't stop after only a few errors.";struct _tuple17 Cyc_opt69_tuple={{_TmpGD6,_TmpGD6,_TmpGD6 + 13U},0,{_TmpGD7,_TmpGD7,_TmpGD7 + 1U},(void*)& Cyc_opt69_spec,{_TmpGD8,_TmpGD8,_TmpGD8 + 36U}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1175
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3,& Cyc_Flags_print_parser_state_and_token};static char _TmpGD9[13U]="--parsestate";static char _TmpGDA[1U]="";static char _TmpGDB[50U]="print the parse state and token on a syntax error";struct _tuple17 Cyc_opt70_tuple={{_TmpGD9,_TmpGD9,_TmpGD9 + 13U},0,{_TmpGDA,_TmpGDA,_TmpGDA + 1U},(void*)& Cyc_opt70_spec,{_TmpGDB,_TmpGDB,_TmpGDB + 50U}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1178
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0,Cyc_noassume_gcc_flag};static char _TmpGDC[22U]="-known-gcc-flags-only";static char _TmpGDD[1U]="";static char _TmpGDE[57U]="do not assume that unknown flags should be passed to gcc";struct _tuple17 Cyc_opt71_tuple={{_TmpGDC,_TmpGDC,_TmpGDC + 22U},0,{_TmpGDD,_TmpGDD,_TmpGDD + 1U},(void*)& Cyc_opt71_spec,{_TmpGDE,_TmpGDE,_TmpGDE + 57U}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1181
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0,Cyc_print_options};static char _TmpGDF[6U]="-help";static char _TmpGE0[1U]="";static char _TmpGE1[32U]="print out the available options";struct _tuple17 Cyc_opt72_tuple={{_TmpGDF,_TmpGDF,_TmpGDF + 6U},0,{_TmpGE0,_TmpGE0,_TmpGE0 + 1U},(void*)& Cyc_opt72_spec,{_TmpGE1,_TmpGE1,_TmpGE1 + 32U}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1184
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0,Cyc_print_options};static char _TmpGE2[7U]="-usage";static char _TmpGE3[1U]="";static char _TmpGE4[32U]="print out the available options";struct _tuple17 Cyc_opt73_tuple={{_TmpGE2,_TmpGE2,_TmpGE2 + 7U},0,{_TmpGE3,_TmpGE3,_TmpGE3 + 1U},(void*)& Cyc_opt73_spec,{_TmpGE4,_TmpGE4,_TmpGE4 + 32U}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1187
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt74_spec={0,Cyc_set_notoseqc};static char _TmpGE5[10U]="-no-seq-c";static char _TmpGE6[1U]="";static char _TmpGE7[71U]="Do not force left-to-right evaluation order of generated code (unsafe)";struct _tuple17 Cyc_opt74_tuple={{_TmpGE5,_TmpGE5,_TmpGE5 + 10U},0,{_TmpGE6,_TmpGE6,_TmpGE6 + 1U},(void*)& Cyc_opt74_spec,{_TmpGE7,_TmpGE7,_TmpGE7 + 71U}};struct Cyc_List_List Cyc_opt74={(void*)& Cyc_opt74_tuple,0};
# 1190
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt75_spec={0,Cyc_set_pg};static char _TmpGE8[4U]="-pg";static char _TmpGE9[1U]="";static char _TmpGEA[33U]="Compile for profiling with gprof";struct _tuple17 Cyc_opt75_tuple={{_TmpGE8,_TmpGE8,_TmpGE8 + 4U},0,{_TmpGE9,_TmpGE9,_TmpGE9 + 1U},(void*)& Cyc_opt75_spec,{_TmpGEA,_TmpGEA,_TmpGEA + 33U}};struct Cyc_List_List Cyc_opt75={(void*)& Cyc_opt75_tuple,0};
# 1193
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt76_spec={4,& Cyc_vcgen_r};static char _TmpGEB[10U]="--novcgen";static char _TmpGEC[1U]="";static char _TmpGED[39U]="do not compute verification-conditions";struct _tuple17 Cyc_opt76_tuple={{_TmpGEB,_TmpGEB,_TmpGEB + 10U},0,{_TmpGEC,_TmpGEC,_TmpGEC + 1U},(void*)& Cyc_opt76_spec,{_TmpGED,_TmpGED,_TmpGED + 39U}};struct Cyc_List_List Cyc_opt76={(void*)& Cyc_opt76_tuple,0};
# 1196
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt77_spec={0,Cyc_Cifc_set_inst_tvar};static char _TmpGEE[17U]="--cifc-inst-tvar";static char _TmpGEF[1U]="";static char _TmpGF0[59U]="instantiate type variables instead of introducing new ones";struct _tuple17 Cyc_opt77_tuple={{_TmpGEE,_TmpGEE,_TmpGEE + 17U},0,{_TmpGEF,_TmpGEF,_TmpGEF + 1U},(void*)& Cyc_opt77_spec,{_TmpGF0,_TmpGF0,_TmpGF0 + 59U}};struct Cyc_List_List Cyc_opt77={(void*)& Cyc_opt77_tuple,0};
# 1199
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt78_spec={0,Cyc_Flags_set_crgnonly};static char _TmpGF1[16U]="-stopafter-crgn";static char _TmpGF2[1U]="";static char _TmpGF3[19U]="Stop after currrgn";struct _tuple17 Cyc_opt78_tuple={{_TmpGF1,_TmpGF1,_TmpGF1 + 16U},0,{_TmpGF2,_TmpGF2,_TmpGF2 + 1U},(void*)& Cyc_opt78_spec,{_TmpGF3,_TmpGF3,_TmpGF3 + 19U}};struct Cyc_List_List Cyc_opt78={(void*)& Cyc_opt78_tuple,0};
# 1202
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt79_spec={3,& Cyc_Flags_allpaths};static char _TmpGF4[11U]="--allpaths";static char _TmpGF5[1U]="";static char _TmpGF6[62U]="analyze all forward paths in verification-condition generator";struct _tuple17 Cyc_opt79_tuple={{_TmpGF4,_TmpGF4,_TmpGF4 + 11U},0,{_TmpGF5,_TmpGF5,_TmpGF5 + 1U},(void*)& Cyc_opt79_spec,{_TmpGF6,_TmpGF6,_TmpGF6 + 62U}};struct Cyc_List_List Cyc_opt79={(void*)& Cyc_opt79_tuple,0};
# 1205
struct Cyc_List_List*Cyc_global_options[78U]={& Cyc_opt1,& Cyc_opt2,& Cyc_opt3,& Cyc_opt4,& Cyc_opt5,& Cyc_opt6,& Cyc_opt7,& Cyc_opt8,& Cyc_opt9,& Cyc_opt10,& Cyc_opt11,& Cyc_opt12,& Cyc_opt13,& Cyc_opt14,& Cyc_opt15,& Cyc_opt16,& Cyc_opt17,& Cyc_opt18,& Cyc_opt19,& Cyc_opt20,& Cyc_opt21,& Cyc_opt22,& Cyc_opt23,& Cyc_opt24,& Cyc_opt25,& Cyc_opt26,& Cyc_opt27,& Cyc_opt28,& Cyc_opt29,& Cyc_opt30,& Cyc_opt31,& Cyc_opt32,& Cyc_opt33,& Cyc_opt34,& Cyc_opt35,& Cyc_opt36,& Cyc_opt37,& Cyc_opt38,& Cyc_opt39,& Cyc_opt41,& Cyc_opt42,& Cyc_opt43,& Cyc_opt44,& Cyc_opt45,& Cyc_opt46,& Cyc_opt47,& Cyc_opt48,& Cyc_opt49,& Cyc_opt50,& Cyc_opt51,& Cyc_opt52,& Cyc_opt53,& Cyc_opt54,& Cyc_opt55,& Cyc_opt56,& Cyc_opt57,& Cyc_opt58,& Cyc_opt59,& Cyc_opt60,& Cyc_opt61,& Cyc_opt62,& Cyc_opt63,& Cyc_opt64,& Cyc_opt65,& Cyc_opt66,& Cyc_opt67,& Cyc_opt68,& Cyc_opt69,& Cyc_opt70,& Cyc_opt71,& Cyc_opt72,& Cyc_opt73,& Cyc_opt74,& Cyc_opt75,& Cyc_opt76,& Cyc_opt77,& Cyc_opt78,& Cyc_opt79};
# 1219
void Cyc_print_options (void){
Cyc_Arg_usage(Cyc_global_options[0],({const char*_Tmp0="<program.cyc>";_tag_fat(_Tmp0,sizeof(char),14U);}));}
# 1224
int Cyc_main(int argc,struct _fat_ptr argv){
# 1227
GC_blacklist_warn_clear();{
# 1230
struct _fat_ptr optstring=({const char*_Tmp0="Options:";_tag_fat(_Tmp0,sizeof(char),9U);});
# 1232
{int i=1;for(0;(unsigned)i < 78U;++ i){
(*((struct Cyc_List_List**)_check_known_subscript_notnull(Cyc_global_options,78U,sizeof(struct Cyc_List_List*),i - 1)))->tl=Cyc_global_options[i];}}{
struct Cyc_List_List*options=Cyc_global_options[0];
# 1236
struct _fat_ptr otherargs=Cyc_Specsfile_parse_b(options,Cyc_add_other,Cyc_add_other_flag,optstring,argv);
# 1242
struct _fat_ptr specs_file=Cyc_find_in_arch_path(({const char*_Tmp0="cycspecs";_tag_fat(_Tmp0,sizeof(char),9U);}));
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=specs_file;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Reading from specs file %s\n";_tag_fat(_Tmp2,sizeof(char),28U);}),_tag_fat(_Tmp1,sizeof(void*),1));});{
struct Cyc_List_List*specs=Cyc_Specsfile_read_specs(specs_file);
# 1246
struct _fat_ptr args_from_specs_file=Cyc_Specsfile_split_specs(Cyc_Specsfile_get_spec(specs,({const char*_Tmp0="cyclone";_tag_fat(_Tmp0,sizeof(char),8U);})));
if((struct _fat_ptr*)args_from_specs_file.curr!=(struct _fat_ptr*)_tag_fat(0,0,0).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,args_from_specs_file);}{
# 1251
struct _fat_ptr target_cflags=Cyc_Specsfile_get_spec(specs,({const char*_Tmp0="cyclone_target_cflags";_tag_fat(_Tmp0,sizeof(char),22U);}));
struct _fat_ptr cyclone_cc=Cyc_Specsfile_get_spec(specs,({const char*_Tmp0="cyclone_cc";_tag_fat(_Tmp0,sizeof(char),11U);}));
if(!((unsigned)cyclone_cc.curr))cyclone_cc=({const char*_Tmp0="gcc";_tag_fat(_Tmp0,sizeof(char),4U);});
Cyc_def_inc_path=Cyc_Specsfile_get_spec(specs,({const char*_Tmp0="cyclone_inc_path";_tag_fat(_Tmp0,sizeof(char),17U);}));
# 1257
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,otherargs);
if(Cyc_strcmp(Cyc_cpp,({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}))==0){
# 1275 "cyclone.cyc"
const char*dash_E=Cyc_produce_dependencies?"":" -E";
Cyc_set_cpp(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cyclone_cc;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=target_cflags;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=dash_E;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=specs_file;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_aprintf(({const char*_Tmp5="%s %s -w -x c%s -specs %s";_tag_fat(_Tmp5,sizeof(char),26U);}),_tag_fat(_Tmp4,sizeof(void*),4));}));}
# 1279
if(Cyc_cyclone_files==0 && Cyc_ccargs==0){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="missing file\n";_tag_fat(_Tmp0,sizeof(char),14U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);}
# 1288
if((int)Cyc_Flags_stop_after_pass > 0 && !Cyc_nocyc_setjmp_r)
Cyc_cyc_setjmp=Cyc_find_in_arch_path(({const char*_Tmp0="cyc_setjmp.h";_tag_fat(_Tmp0,sizeof(char),13U);}));
if((int)Cyc_Flags_stop_after_pass > 0 && Cyc_strlen(Cyc_cyc_include)==0U)
Cyc_cyc_include=Cyc_find_in_exec_path(({const char*_Tmp0="cyc_include.h";_tag_fat(_Tmp0,sizeof(char),14U);}));
# 1294
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_List_List*l=Cyc_List_rev(Cyc_cyclone_files);for(0;l!=0;l=l->tl){
Cyc_process_file(*((struct _fat_ptr*)l->hd));
if(Cyc_compile_failure){int _Tmp2=1;_npop_handler(0);return _Tmp2;}}}
# 1295
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;_Tmp3=_Tmp2;{void*x=_Tmp3;
# 1300
if(Cyc_compile_failure)return 1;
Cyc_Core_rethrow(x);};}}}
# 1304
if((int)Cyc_Flags_stop_after_pass < 14)return 0;
# 1310
if(Cyc_ccargs==0)return 0;
# 1313
if((unsigned)Cyc_Specsfile_target_arch.curr &&(unsigned)Cyc_Specsfile_cyclone_exec_path)
Cyc_add_ccarg(({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=({const char*_Tmp4="";_tag_fat(_Tmp4,sizeof(char),1U);});_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_Tmp1->tl=_Tmp2;});_Tmp1;});
# 1314
Cyc_str_sepstr(_Tmp0,({const char*_Tmp1=" -L";_tag_fat(_Tmp1,sizeof(char),4U);}));}));
# 1317
Cyc_add_ccarg(Cyc_strconcat(({const char*_Tmp0="-L";_tag_fat(_Tmp0,sizeof(char),3U);}),Cyc_Specsfile_def_lib_path));
Cyc_ccargs=Cyc_List_rev(Cyc_ccargs);{
struct _fat_ptr ccargs_string=({struct Cyc_List_List*_Tmp0=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,Cyc_ccargs);Cyc_str_sepstr(_Tmp0,({const char*_Tmp1=" ";_tag_fat(_Tmp1,sizeof(char),2U);}));});
Cyc_libargs=Cyc_List_rev(Cyc_libargs);{
struct _fat_ptr libargs_string=({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=({const char*_Tmp4="";_tag_fat(_Tmp4,sizeof(char),1U);});_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,Cyc_libargs);_Tmp1->tl=_Tmp2;});_Tmp1;});
# 1321
Cyc_str_sepstr(_Tmp0,({const char*_Tmp1=" ";_tag_fat(_Tmp1,sizeof(char),2U);}));});
# 1324
struct Cyc_List_List*stdlib;
struct _fat_ptr stdlib_string;
int is_not_executable=
((Cyc_stop_after_asmfile_r || Cyc_stop_after_objectfile_r)||
# 1329
 Cyc_output_file!=0 && Cyc_Filename_check_suffix(*_check_null(Cyc_output_file),({const char*_Tmp0=".a";_tag_fat(_Tmp0,sizeof(char),3U);})))||
 Cyc_output_file!=0 && Cyc_Filename_check_suffix(*_check_null(Cyc_output_file),({const char*_Tmp0=".lib";_tag_fat(_Tmp0,sizeof(char),5U);}));
if(is_not_executable){
stdlib=0;
stdlib_string=({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);});}else{
# 1336
struct _fat_ptr libcyc_flag;
struct _fat_ptr nogc_filename;
struct _fat_ptr runtime_filename;
struct _fat_ptr gc_filename;
if(Cyc_pa_r){
libcyc_flag=({const char*_Tmp0="-lcyc_a";_tag_fat(_Tmp0,sizeof(char),8U);});
nogc_filename=({const char*_Tmp0="nogc_a.a";_tag_fat(_Tmp0,sizeof(char),9U);});
runtime_filename=({const char*_Tmp0="runtime_cyc_a.a";_tag_fat(_Tmp0,sizeof(char),16U);});}else{
if(Cyc_nocheck_r){
libcyc_flag=({const char*_Tmp0="-lcyc_nocheck";_tag_fat(_Tmp0,sizeof(char),14U);});
nogc_filename=({const char*_Tmp0="nogc.a";_tag_fat(_Tmp0,sizeof(char),7U);});
runtime_filename=({const char*_Tmp0="runtime_cyc.a";_tag_fat(_Tmp0,sizeof(char),14U);});}else{
if(Cyc_pg_r){
libcyc_flag=({const char*_Tmp0="-lcyc_pg";_tag_fat(_Tmp0,sizeof(char),9U);});
runtime_filename=({const char*_Tmp0="runtime_cyc_pg.a";_tag_fat(_Tmp0,sizeof(char),17U);});
nogc_filename=({const char*_Tmp0="nogc_pg.a";_tag_fat(_Tmp0,sizeof(char),10U);});}else{
if(Cyc_Flags_compile_for_boot){
# 1354
libcyc_flag=({const char*_Tmp0="-lcycboot";_tag_fat(_Tmp0,sizeof(char),10U);});
nogc_filename=({const char*_Tmp0="nogc.a";_tag_fat(_Tmp0,sizeof(char),7U);});
runtime_filename=({const char*_Tmp0="runtime_cyc.a";_tag_fat(_Tmp0,sizeof(char),14U);});}else{
if(Cyc_pthread_r){
libcyc_flag=({const char*_Tmp0="-lcyc";_tag_fat(_Tmp0,sizeof(char),6U);});
nogc_filename=({const char*_Tmp0="nogc_pthread.a";_tag_fat(_Tmp0,sizeof(char),15U);});
runtime_filename=({const char*_Tmp0="runtime_cyc_pthread.a";_tag_fat(_Tmp0,sizeof(char),22U);});}else{
# 1362
libcyc_flag=({const char*_Tmp0="-lcyc";_tag_fat(_Tmp0,sizeof(char),6U);});
nogc_filename=({const char*_Tmp0="nogc.a";_tag_fat(_Tmp0,sizeof(char),7U);});
runtime_filename=({const char*_Tmp0="runtime_cyc.a";_tag_fat(_Tmp0,sizeof(char),14U);});}}}}}
# 1366
if(!Cyc_pthread_r)
gc_filename=({const char*_Tmp0="gc.a";_tag_fat(_Tmp0,sizeof(char),5U);});else{
# 1369
gc_filename=({const char*_Tmp0="gc_pthread.a";_tag_fat(_Tmp0,sizeof(char),13U);});}{
# 1371
struct _fat_ptr gc=Cyc_nogc_r?Cyc_find_in_arch_path(nogc_filename): Cyc_find_in_arch_path(gc_filename);
struct _fat_ptr runtime=Cyc_find_in_arch_path(runtime_filename);
# 1374
stdlib=0;
if(Cyc_nogc_r)
stdlib_string=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=libcyc_flag;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=runtime;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=gc;_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4=" %s %s %s ";_tag_fat(_Tmp4,sizeof(char),11U);}),_tag_fat(_Tmp3,sizeof(void*),3));});else{
# 1378
stdlib_string=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=libcyc_flag;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=runtime;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=gc;_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4=" %s %s %s -DCYC_GC_PTHREAD_REDIRECTS ";_tag_fat(_Tmp4,sizeof(char),38U);}),_tag_fat(_Tmp3,sizeof(void*),3));});}}}
# 1381
if(Cyc_ic_r){struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
Cyc_ccargs=({(struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_filter;})(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=({(struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_filter;})(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*lf=({struct Cyc_List_List*_Tmp2=stdlib;Cyc_List_append(_Tmp2,Cyc_List_append(Cyc_ccargs,Cyc_libargs));});
if(!is_not_executable)
lf=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=Cyc_final_strptr,_Tmp2->tl=lf;_Tmp2;});{
# 1388
struct Cyc_Interface_I*i=({(struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*)(struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_Interface_get_and_merge_list;})(Cyc_read_cycio,lf,lf);
if(i==0){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: interfaces incompatible\n";_tag_fat(_Tmp2,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _Tmp2=1;_npop_handler(0);return _Tmp2;}}
# 1395
if(is_not_executable){
if(Cyc_output_file!=0){
struct _fat_ptr output_file_io=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Filename_chop_extension(*_check_null(Cyc_output_file));_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="%s.cycio";_tag_fat(_Tmp4,sizeof(char),9U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
struct Cyc___cycFILE*f=Cyc_try_file_open(output_file_io,({const char*_Tmp2="wb";_tag_fat(_Tmp2,sizeof(char),3U);}),({const char*_Tmp2="interface object file";_tag_fat(_Tmp2,sizeof(char),22U);}));
if(f==0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _Tmp2=1;_npop_handler(0);return _Tmp2;}}
# 1404
Cyc_Interface_save(i,f);
Cyc_file_close(f);}}else{
# 1408
if(!({struct Cyc_Interface_I*_Tmp2=Cyc_Interface_empty();struct Cyc_Interface_I*_Tmp3=i;Cyc_Interface_is_subinterface(_Tmp2,_Tmp3,({struct _tuple12*_Tmp4=_cycalloc(sizeof(struct _tuple12));
_Tmp4->f1=({const char*_Tmp5="empty interface";_tag_fat(_Tmp5,sizeof(char),16U);}),_Tmp4->f2=({const char*_Tmp5="global interface";_tag_fat(_Tmp5,sizeof(char),17U);});_Tmp4;}));})){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: some objects are still undefined\n";_tag_fat(_Tmp2,sizeof(char),41U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _Tmp2=1;_npop_handler(0);return _Tmp2;}}}}}
# 1382
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;_Tmp3=_Tmp2;{void*e=_Tmp3;
# 1418
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=({const char*_Tmp7=Cyc_Core_get_exn_name(e);_tag_fat((void*)_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=({const char*_Tmp8=Cyc_Core_get_exn_filename();_tag_fat((void*)_Tmp8,sizeof(char),_get_zero_arr_size_char((void*)_Tmp8,1U));});_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,({unsigned long _Tmp8=(unsigned long)Cyc_Core_get_exn_lineno();_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,({const char*_Tmp8="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu";_tag_fat(_Tmp8,sizeof(char),131U);}),_tag_fat(_Tmp7,sizeof(void*),3));});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;};}}}{
# 1424
struct _fat_ptr outfile_str=({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);});
if(Cyc_output_file!=0)
outfile_str=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_sh_escape_string(*_check_null(Cyc_output_file));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2=" -o %s";_tag_fat(_Tmp2,sizeof(char),7U);}),_tag_fat(_Tmp1,sizeof(void*),1));});{
# 1428
struct _fat_ptr cccmd=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cyclone_cc;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=target_cflags;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=ccargs_string;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=outfile_str;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=stdlib_string;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=libargs_string;_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="%s %s %s%s %s%s";_tag_fat(_Tmp7,sizeof(char),16U);}),_tag_fat(_Tmp6,sizeof(void*),6));});
# 1433
if(Cyc_v_r){({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cccmd;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="%s\n";_tag_fat(_Tmp2,sizeof(char),4U);}),_tag_fat(_Tmp1,sizeof(void*),1));});Cyc_fflush(Cyc_stderr);}
if(system((const char*)_check_null(_untag_fat_ptr(cccmd,sizeof(char),1U)))!=0){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="Error: C compiler failed\n";_tag_fat(_Tmp0,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}
# 1440
Cyc_remove_cfiles();
# 1442
return Cyc_compile_failure?1: 0;}}}}}}}}}

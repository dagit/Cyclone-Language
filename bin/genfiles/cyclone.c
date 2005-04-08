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
#define _check_fat_at_base(arr) (arr)
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
# 319 "core.h"
void Cyc_Core_rethrow(void*);
# 323
const char*Cyc_Core_get_exn_name(void*);
# 325
const char*Cyc_Core_get_exn_filename (void);
# 332
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
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 57
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
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple12{struct _fat_ptr f0;struct _fat_ptr f1;};
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
# 32
extern int Cyc_Flags_no_reaps;
# 36
extern int Cyc_Flags_no_register;
# 39
extern int Cyc_Flags_warn_override;
# 49
extern int Cyc_Flags_warn_alias_coerce;
# 53
extern int Cyc_Flags_warn_region_coerce;
# 56
extern int Cyc_Flags_warn_lose_unique;
# 60
extern int Cyc_Flags_warn_bounds_checks;
# 64
extern int Cyc_Flags_warn_all_null_deref;
# 68
extern int Cyc_Flags_noexpand_r;
# 73
extern int Cyc_Flags_allpaths;
# 82
extern int Cyc_Flags_tovc_r;
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 97 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_TempRename =12U,Cyc_Flags_CastRemoval =13U,Cyc_Flags_EvalOrder =14U,Cyc_Flags_CCompiler =15U,Cyc_Flags_AllPasses =16U};
# 116
extern enum Cyc_Flags_Cyclone_Passes Cyc_Flags_stop_after_pass;
# 118
void Cyc_Flags_set_cpponly (void);
void Cyc_Flags_set_parseonly (void);
void Cyc_Flags_set_crgnonly (void);
void Cyc_Flags_set_tconly (void);
void Cyc_Flags_set_cfonly (void);
void Cyc_Flags_set_toconly (void);
# 129
extern int Cyc_Flags_compile_for_boot;
# 133
extern int Cyc_Flags_print_parser_state_and_token;
# 136
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
# 47 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 50
void Cyc_Lex_pos_init (void);
void Cyc_Lex_lex_init(int);
# 58
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
# 85
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _fat_ptr s){
Cyc_ccargs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_ccargs;_Tmp0;});}
# 93
void Cyc_set_c_compiler(struct _fat_ptr s){
if(Cyc_strcmp(s,_tag_fat("vc",sizeof(char),3U))==0){
Cyc_Flags_c_compiler=1U;
Cyc_add_ccarg(_tag_fat("-DVC_C",sizeof(char),7U));}else{
# 98
if(Cyc_strcmp(s,_tag_fat("gcc",sizeof(char),4U))==0){
Cyc_Flags_c_compiler=0U;
Cyc_add_ccarg(_tag_fat("-DGCC_C",sizeof(char),8U));}}}
# 105
void Cyc_set_port_c_code (void){
Cyc_Flags_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_Flags_set_parseonly();}
# 112
static struct _fat_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _fat_ptr s){
Cyc_output_file=({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));*_Tmp0=s;_Tmp0;});}
# 117
static struct _fat_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _fat_ptr s){
Cyc_specified_interface=s;}
# 127
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Cversion;static char _TmpG0[1U]="";
# 132
static struct _fat_ptr Cyc_cpp={_TmpG0,_TmpG0,_TmpG0 + 1U};
static void Cyc_set_cpp(struct _fat_ptr s){
Cyc_cpp=s;}
# 137
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cppargs;_Tmp0;});}
# 142
static void Cyc_add_cpp_and_ccarg(struct _fat_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 147
static void Cyc_add_iprefix(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("-iprefix %s",sizeof(char),12U),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 150
static void Cyc_add_iwithprefix(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("-iwithprefix %s",sizeof(char),16U),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 153
static void Cyc_add_iwithprefixbefore(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("-iwithprefixbefore %s",sizeof(char),22U),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 156
static void Cyc_add_isystem(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("-isystem %s",sizeof(char),12U),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 159
static void Cyc_add_idirafter(struct _fat_ptr s){
Cyc_add_cpparg(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("-idirafter %s",sizeof(char),14U),_tag_fat(_Tmp1,sizeof(void*),1));}));}
# 163
static void Cyc_set_many_errors (void){
Cyc_Position_max_errors=65535;}static char _TmpG1[20U]="_see_cycspecs_file_";
# 167
static struct _fat_ptr Cyc_def_inc_path={_TmpG1,_TmpG1,_TmpG1 + 20U};
# 170
static void Cyc_print_version (void){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)({char*_Tmp2=Cversion;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(_tag_fat("The Cyclone compiler, version %s\n",sizeof(char),34U),_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)({char*_Tmp2=Carch;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(_tag_fat("Compiled for architecture: %s\n",sizeof(char),31U),_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)({char*_Tmp2=Cdef_lib_path;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(_tag_fat("Standard library directory: %s\n",sizeof(char),32U),_tag_fat(_Tmp1,sizeof(void*),1));});
# 175
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_def_inc_path);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(_tag_fat("Standard include directory: %s\n",sizeof(char),32U),_tag_fat(_Tmp1,sizeof(void*),1));});
exit(0);}
# 179
static int Cyc_is_cyclone_sourcefile(struct _fat_ptr s){
unsigned long len=Cyc_strlen(s);
if(len <= 4U)return 0;
return({struct _fat_ptr _Tmp0=(struct _fat_ptr)_fat_ptr_plus(s,sizeof(char),(int)(len - 4U));Cyc_strcmp(_Tmp0,_tag_fat(".cyc",sizeof(char),5U));})==0;}
# 195 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _fat_ptr s){
if(Cyc_strcmp(s,_tag_fat("-lxml",sizeof(char),6U))==0)
Cyc_add_ccarg(Cyc_pa_r?_tag_fat("-lxml_a",sizeof(char),8U): s);else{
if(Cyc_strncmp(s,_tag_fat("-lcyc",sizeof(char),6U),5U)==0)
Cyc_add_ccarg(s);else{
# 202
if(Cyc_strncmp(s,_tag_fat("-lpthread",sizeof(char),10U),9U)==0)
Cyc_pthread_r=1;
Cyc_libargs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_libargs;_Tmp0;});}}}
# 208
static void Cyc_add_marg(struct _fat_ptr s){
Cyc_add_ccarg(s);}
# 212
static void Cyc_set_save_temps (void){
Cyc_save_temps_r=1;
Cyc_add_ccarg(_tag_fat("-save-temps",sizeof(char),12U));}
# 217
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies (void){
Cyc_Flags_stop_after_pass=0U;
Cyc_produce_dependencies=1;
Cyc_add_cpparg(_tag_fat("-M",sizeof(char),3U));}
# 224
static struct _fat_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _fat_ptr s){
Cyc_dependencies_target=({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));*_Tmp0=s;_Tmp0;});}
# 229
static void Cyc_set_stop_after_objectfile (void){
Cyc_stop_after_objectfile_r=1;
Cyc_add_ccarg(_tag_fat("-c",sizeof(char),3U));}
# 234
static void Cyc_set_nocppprecomp (void){
Cyc_add_cpp_and_ccarg(_tag_fat("-no-cpp-precomp",sizeof(char),16U));}
# 238
static void Cyc_clear_lineno (void){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 242
static void Cyc_set_elim_se (void){
Cyc_elim_se_r=1;}
# 245
static void Cyc_set_tovc (void){
Cyc_Flags_tovc_r=1;
Cyc_add_ccarg(_tag_fat("-DCYC_ANSI_OUTPUT",sizeof(char),18U));
Cyc_set_elim_se();}
# 250
static void Cyc_set_notoseqc (void){
Cyc_toseqc_r=0;}
# 253
static void Cyc_set_noboundschecks (void){
Cyc_add_ccarg(_tag_fat("-DNO_CYC_BOUNDS_CHECKS",sizeof(char),23U));}
# 256
static void Cyc_set_nonullchecks (void){
Cyc_add_ccarg(_tag_fat("-DNO_CYC_NULL_CHECKS",sizeof(char),21U));}
# 259
static void Cyc_set_nochecks (void){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 265
static void Cyc_set_nocyc (void){
Cyc_add_cyc_namespace_r=0;
Cyc_add_ccarg(_tag_fat("-DNO_CYC_PREFIX",sizeof(char),16U));}
# 270
static void Cyc_set_pa (void){
Cyc_pa_r=1;
Cyc_add_ccarg(_tag_fat("-DCYC_REGION_PROFILE",sizeof(char),21U));
Cyc_add_cpparg(_tag_fat("-DCYC_REGION_PROFILE",sizeof(char),21U));}
# 276
static void Cyc_set_noreaps (void){
# 279
Cyc_Flags_no_reaps=1;
Cyc_add_cpparg(_tag_fat("-D_CYC_INTERNAL_DISABLE_REAPS",sizeof(char),30U));}
# 283
static void Cyc_set_pg (void){
Cyc_pg_r=1;
Cyc_add_ccarg(_tag_fat("-pg",sizeof(char),4U));}
# 288
static void Cyc_set_stop_after_asmfile (void){
Cyc_stop_after_asmfile_r=1;
Cyc_add_ccarg(_tag_fat("-S",sizeof(char),3U));}
# 294
enum Cyc_inputtype{Cyc_DEFAULTINPUT =0U,Cyc_CYCLONEFILE =1U};
# 298
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _fat_ptr s){
if(Cyc_strcmp(s,_tag_fat("cyc",sizeof(char),4U))==0)
Cyc_intype=1U;else{
if(Cyc_strcmp(s,_tag_fat("none",sizeof(char),5U))==0)
Cyc_intype=0U;else{
# 305
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Input type '%s' not supported\n",sizeof(char),31U),_tag_fat(_Tmp1,sizeof(void*),1));});}}}
# 308
struct _fat_ptr Cyc_make_base_filename(struct _fat_ptr s,struct _fat_ptr*output_file){
struct _fat_ptr outdir=Cyc_Filename_dirname(output_file==0?s:*output_file);
struct _fat_ptr base=Cyc_Filename_chop_extension(Cyc_Filename_basename(s));
# 314
struct _fat_ptr basename=Cyc_strlen(outdir)> 0U?Cyc_Filename_concat(outdir,base): base;
return basename;}
# 320
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 322
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _fat_ptr s){
# 325
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
if((int)Cyc_intype==1 || Cyc_is_cyclone_sourcefile(s)){
# 330
Cyc_cyclone_files=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cyclone_files;_Tmp0;});{
# 336
struct _fat_ptr basename=Cyc_make_base_filename(s,0);
struct _fat_ptr cfile=Cyc_strconcat(basename,_tag_fat(".c",sizeof(char),3U));
Cyc_add_ccarg(cfile);}}else{
# 340
Cyc_add_ccarg(s);}}}
# 345
static int Cyc_assume_gcc_flag=1;struct _tuple13{struct _fat_ptr flag;int optargs;};static char _TmpG2[3U]="-s";static char _TmpG3[3U]="-O";static char _TmpG4[4U]="-O0";static char _TmpG5[4U]="-O2";static char _TmpG6[4U]="-O3";static char _TmpG7[21U]="-fomit-frame-pointer";static char _TmpG8[13U]="-fno-builtin";static char _TmpG9[3U]="-g";static char _TmpGA[3U]="-p";static char _TmpGB[8U]="-static";
static int Cyc_add_other_flag(struct _fat_ptr s){
static struct _tuple13 known_gcc_flags[10U]={{.flag={_TmpG2,_TmpG2,_TmpG2 + 3U},.optargs=0},{.flag={_TmpG3,_TmpG3,_TmpG3 + 3U},.optargs=0},{.flag={_TmpG4,_TmpG4,_TmpG4 + 4U},.optargs=0},{.flag={_TmpG5,_TmpG5,_TmpG5 + 4U},.optargs=0},{.flag={_TmpG6,_TmpG6,_TmpG6 + 4U},.optargs=0},{.flag={_TmpG7,_TmpG7,_TmpG7 + 21U},.optargs=0},{.flag={_TmpG8,_TmpG8,_TmpG8 + 13U},.optargs=0},{.flag={_TmpG9,_TmpG9,_TmpG9 + 3U},.optargs=0},{.flag={_TmpGA,_TmpGA,_TmpGA + 3U},.optargs=0},{.flag={_TmpGB,_TmpGB,_TmpGB + 8U},.optargs=0}};
# 359
if(Cyc_assume_gcc_flag){
Cyc_add_ccarg(s);
return 1;}else{
# 363
{int i=0;for(0;(unsigned)i < 10U;++ i){
if(!Cyc_strcmp((*((struct _tuple13*)_check_known_subscript_notnull(known_gcc_flags,10U,sizeof(struct _tuple13),i))).flag,s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=known_gcc_flags[i].optargs;
break;}}}
# 369
return 0;}}
# 373
static void Cyc_noassume_gcc_flag (void){
Cyc_assume_gcc_flag=0;}
# 377
static void Cyc_remove_file(struct _fat_ptr s){
if(!Cyc_save_temps_r)
remove((const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)));}
# 384
int Cyc_compile_failure=0;
# 386
struct Cyc___cycFILE*Cyc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){{struct Cyc___cycFILE*_Tmp2=Cyc_file_open(filename,mode);_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();
# 389
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)msg_part);_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("\nError: couldn't open %s %s\n",sizeof(char),29U),_tag_fat(_Tmp5,sizeof(void*),2));});
Cyc_fflush(Cyc_stderr);
return 0;;}}}struct _tuple14{void*f0;const char*f1;int f2;};
# 396
struct _tuple14*Cyc_uncaught_exn=0;
# 398
extern void CYCALLOCPROFILE_mark(const char*);
# 400
void*Cyc_do_stage(struct _fat_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void*onfail){
# 402
CYCALLOCPROFILE_mark((const char*)_untag_fat_ptr(stage_name,sizeof(char),1U));{
struct Cyc_timeval start;struct Cyc_timeval end;
if(Cyc_v_r)
gettimeofday(& start,0);{
# 407
void*ans;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){ans=f(env,tds);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;_Tmp3=_Tmp2;{void*x=_Tmp3;
# 410
ans=onfail;
if(Cyc_Position_error_p())
goto _LL0;
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("COMPILER STAGE %s (unexpected exception) \n",sizeof(char),43U),_tag_fat(_Tmp5,sizeof(void*),1));});
Cyc_uncaught_exn=({struct _tuple14*_Tmp4=_cycalloc(sizeof(struct _tuple14));_Tmp4->f0=x,({const char*_Tmp5=Cyc_Core_get_exn_filename();_Tmp4->f1=_Tmp5;}),({int _Tmp5=Cyc_Core_get_exn_lineno();_Tmp4->f2=_Tmp5;});_Tmp4;});
goto _LL0;}_LL0:;}}}
# 418
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
Cyc_fprintf(Cyc_stderr,_tag_fat("\nCOMPILATION FAILED!\n",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));
Cyc_fflush(Cyc_stderr);}else{
if(Cyc_v_r){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s completed ",sizeof(char),14U),_tag_fat(_Tmp1,sizeof(void*),1));});
gettimeofday(& end,0);
end.tv_sec -=start.tv_sec;
end.tv_usec -=start.tv_usec;
if(end.tv_usec < 0){
end.tv_usec +=1000000;
-- end.tv_sec;}
# 432
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)end.tv_sec;_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)end.tv_usec;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("(%d.%06d secs)\n",sizeof(char),16U),_tag_fat(_Tmp2,sizeof(void*),2));});
Cyc_fflush(Cyc_stderr);}}
# 435
return ans;}}}
# 438
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
ans=Cyc_Parse_parse_file(f);
Cyc_file_close(f);
# 443
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Parse_Exit_exn_struct*)_Tmp2)->tag==Cyc_Parse_Exit){
# 446
Cyc_file_close(f);Cyc_compile_failure=1;goto _LL0;}else{_Tmp3=_Tmp2;{void*e=_Tmp3;
Cyc_file_close(f);Cyc_Core_rethrow(e);}}_LL0:;}}}
# 449
Cyc_Lex_lex_init(1);
return ans;}}
# 453
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}
# 458
int Cyc_do_adjust(int ignore,struct Cyc_List_List*tds){
Cyc_CurRgn_adjust_all(tds);
return 1;}
# 463
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){
# 465
Cyc_Tc_tc(te,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(te,tds);
return tds;}
# 471
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){
# 473
return Cyc_JumpAnalysis_jump_analysis(tds);}
# 476
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 478
Cyc_NewControlFlow_cf_check(tables,tds);
return tds;}
# 482
int Cyc_do_insert_checks(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
Cyc_InsertChecks_insert_checks(tds,tables,Cyc_vcgen_r);
return 1;}struct _tuple15{struct Cyc_Tcenv_Tenv*f0;struct Cyc___cycFILE*f1;struct Cyc___cycFILE*f2;};
# 489
struct Cyc_List_List*Cyc_do_interface(struct _tuple15*params,struct Cyc_List_List*tds){
# 491
void*_Tmp0;void*_Tmp1;void*_Tmp2;_Tmp2=params->f0;_Tmp1=params->f1;_Tmp0=params->f2;{struct Cyc_Tcenv_Tenv*te=_Tmp2;struct Cyc___cycFILE*fi=_Tmp1;struct Cyc___cycFILE*fo=_Tmp0;
struct Cyc_Interface_I*i1=Cyc_Interface_extract(te->ae,tds);
if(fi==0)
Cyc_Interface_save(i1,fo);else{
# 496
struct Cyc_Interface_I*i0=Cyc_Interface_parse(fi);
if(!({struct Cyc_Interface_I*_Tmp3=i0;struct Cyc_Interface_I*_Tmp4=i1;Cyc_Interface_is_subinterface(_Tmp3,_Tmp4,({struct _tuple12*_Tmp5=_cycalloc(sizeof(struct _tuple12));_Tmp5->f0=_tag_fat("written interface",sizeof(char),18U),_Tmp5->f1=_tag_fat("maximal interface",sizeof(char),18U);_Tmp5;}));}))
Cyc_compile_failure=1;else{
# 500
Cyc_Interface_save(i0,fo);}}
# 502
return tds;}}
# 505
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){
# 508
return Cyc_Toc_toc(pops,tds);}
# 510
struct Cyc_List_List*Cyc_do_removeaggrs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveAggrs_remove_aggrs(tds);}
# 513
struct Cyc_List_List*Cyc_do_removelabs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveLabels_remove_unused_labels(tds);}
# 516
struct Cyc_List_List*Cyc_do_temprename(int ignore,struct Cyc_List_List*tds){
return Cyc_RenameTemps_rename(tds);}
# 519
struct Cyc_List_List*Cyc_do_castremove(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveCasts_remove_casts(tds);}
# 523
struct Cyc_List_List*Cyc_do_toseqc(int ignore,struct Cyc_List_List*tds){
return Cyc_Toseqc_toseqc(tds);}
# 526
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Flags_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 530
struct Cyc_List_List*Cyc_do_lowtreeshake(int ignore,struct Cyc_List_List*tds){
return Cyc_LowTreeShake_shake(tds);}
# 534
static struct _fat_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 536
static struct _fat_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 538
static void Cyc_set_cyc_include(struct _fat_ptr f){
Cyc_cyc_include=f;}
# 543
int Cyc_copy_internal_file(struct _fat_ptr file,struct Cyc___cycFILE*out_file){
# 546
if((char*)file.curr==(char*)_tag_fat(0,0,0).curr){
Cyc_fprintf(Cyc_stderr,_tag_fat("Internal error: copy_internal_file called with NULL\n",sizeof(char),53U),_tag_fat(0U,sizeof(void*),0));
return 1;}{
# 550
struct Cyc___cycFILE*file_f=Cyc_try_file_open(file,_tag_fat("r",sizeof(char),2U),_tag_fat("internal compiler file",sizeof(char),23U));
if(file_f==0)return 1;{
unsigned long n_read=1024U;
unsigned long n_written;
char buf[1024U];({{unsigned _Tmp0=1024U;unsigned i;for(i=0;i < _Tmp0;++ i){buf[i]='\000';}}0;});
while(n_read==1024U){
n_read=Cyc_fread(_tag_fat(buf,sizeof(char),1024U),1U,1024U,file_f);
if(n_read!=1024U && !Cyc_feof(file_f)){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)file);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("\nError: problem copying %s\n",sizeof(char),28U),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_fflush(Cyc_stderr);
return 1;}
# 563
n_written=Cyc_fwrite(_tag_fat(buf,sizeof(char),1024U),1U,n_read,out_file);
if(n_read!=n_written){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)file);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("\nError: problem copying %s\n",sizeof(char),28U),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_fflush(Cyc_stderr);
return 1;}}
# 571
Cyc_fclose(file_f);
return 0;}}}
# 577
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles (void){
if(!Cyc_save_c_r)
for(1;Cyc_cfiles!=0;Cyc_cfiles=_check_null(Cyc_cfiles)->tl){
Cyc_remove_file(*((struct _fat_ptr*)Cyc_cfiles->hd));}}
# 583
static void Cyc_find_fail(struct _fat_ptr file){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,({struct _fat_ptr _Tmp1=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)((struct _fat_ptr)file);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("Error: can't find internal compiler file %s\n",sizeof(char),45U),_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp0->f1=_Tmp1;});_Tmp0;}));}
# 588
static struct _fat_ptr Cyc_find_in_arch_path(struct _fat_ptr s){
struct _fat_ptr r=Cyc_Specsfile_find_in_arch_path(s);
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}
# 593
static struct _fat_ptr Cyc_find_in_exec_path(struct _fat_ptr s){
struct _fat_ptr r=Cyc_Specsfile_find_in_exec_path(s);
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}struct _tuple16{struct Cyc___cycFILE*f0;struct _fat_ptr f1;};
# 599
struct Cyc_List_List*Cyc_do_print(struct _tuple16*env,struct Cyc_List_List*tds){
# 601
struct _fat_ptr _Tmp0;void*_Tmp1;_Tmp1=env->f0;_Tmp0=env->f1;{struct Cyc___cycFILE*out_file=_Tmp1;struct _fat_ptr cfile=_Tmp0;
struct Cyc_Absynpp_Params params_r=(int)Cyc_Flags_stop_after_pass < 9?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
params_r.expand_typedefs=!Cyc_Flags_noexpand_r;
params_r.to_VC=Cyc_Flags_tovc_r;
params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;
params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;
params_r.print_all_effects=Cyc_print_all_effects_r;
# 613
if((int)Cyc_Flags_stop_after_pass > 6){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Flags_compile_for_boot)
Cyc_fprintf(out_file,_tag_fat("#include <setjmp.h>\n",sizeof(char),21U),_tag_fat(0U,sizeof(void*),0));else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 619
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 621
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& params_r);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 625
Cyc_Absyndump_set_params(& params_r);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 628
Cyc_fflush(out_file);
return tds;}}
# 633
static struct Cyc_List_List*Cyc_split_by_char(struct _fat_ptr s,char c){
if((char*)s.curr==(char*)_tag_fat(0,0,0).curr)return 0;{
struct Cyc_List_List*result=0;
unsigned long len=Cyc_strlen(s);
# 638
while(len > 0U){
struct _fat_ptr end=Cyc_strchr(s,c);
if((char*)end.curr==(char*)_tag_fat(0,0,0).curr){
result=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=result;_Tmp0;});
break;}else{
# 644
result=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((((struct _fat_ptr)end).curr - s.curr)/ sizeof(char)));*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=result;_Tmp0;});
len -=(unsigned long)((end.curr - s.curr)/ sizeof(char));
s=_fat_ptr_plus(end,sizeof(char),1);}}
# 649
return Cyc_List_imp_rev(result);}}
# 654
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _fat_ptr subdir){
# 656
struct Cyc_List_List*l=0;
for(1;dirs!=0;dirs=dirs->tl){
l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_Filename_concat(*((struct _fat_ptr*)dirs->hd),subdir);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=l;_Tmp0;});}
l=Cyc_List_imp_rev(l);
return l;}
# 664
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
# 679
 goto _LL18;case 9: _LL18:
 return 1;default:
 return 0;};}
# 686
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long len=Cyc_strlen(s);
# 690
int single_quotes=0;
int other_special=0;
{int i=0;for(0;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c==39)++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 699
if(single_quotes==0 && other_special==0)
return s;
# 703
if(single_quotes==0)
return Cyc_strconcat_l(({struct _fat_ptr*_Tmp0[3];({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("'",sizeof(char),2U);_Tmp2;});_Tmp0[0]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=(struct _fat_ptr)s;_Tmp2;});_Tmp0[1]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("'",sizeof(char),2U);_Tmp2;});_Tmp0[2]=_Tmp1;});Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _fat_ptr*),3));}));{
# 707
unsigned long len2=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;
struct _fat_ptr s2=({unsigned _Tmp0=(len2 + 1U)+ 1U;_tag_fat(({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=len2 + 1U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='\000';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;}),sizeof(char),_Tmp0);});
int i=0;
int j=0;
for(1;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c==39 || Cyc_is_other_special(c))
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\\';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 717
return s2;}}
# 719
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=Cyc_sh_escape_string(*sp);*_Tmp0=_Tmp1;});_Tmp0;});}
# 723
static void Cyc_process_file(struct _fat_ptr filename){
# 725
struct _fat_ptr basename=Cyc_make_base_filename(filename,Cyc_output_file);
struct _fat_ptr preprocfile=Cyc_strconcat(basename,_tag_fat(".cyp",sizeof(char),5U));
struct _fat_ptr interfacefile=(char*)Cyc_specified_interface.curr!=(char*)_tag_fat(0,0,0).curr?Cyc_specified_interface:
 Cyc_strconcat(basename,_tag_fat(".cyci",sizeof(char),6U));
struct _fat_ptr interfaceobjfile=Cyc_strconcat(basename,_tag_fat(".cycio",sizeof(char),7U));
# 733
struct _fat_ptr cfile=({struct _fat_ptr _Tmp0=(struct _fat_ptr)Cyc_make_base_filename(filename,0);Cyc_strconcat(_Tmp0,_tag_fat(".c",sizeof(char),3U));});
struct _fat_ptr*preprocfileptr;preprocfileptr=_cycalloc(sizeof(struct _fat_ptr)),*preprocfileptr=preprocfile;
# 736
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Compiling %s\n",sizeof(char),14U),_tag_fat(_Tmp1,sizeof(void*),1));});{
# 739
struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,_tag_fat("r",sizeof(char),2U),_tag_fat("input file",sizeof(char),11U));
if(Cyc_compile_failure || !((unsigned)f0))
return;
Cyc_fclose(f0);{
# 747
struct _fat_ptr cppargs_string=({
struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=(struct _fat_ptr)_tag_fat("",sizeof(char),1U);_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,Cyc_List_rev(Cyc_cppargs));_Tmp1->tl=_Tmp2;});_Tmp1;});
# 748
Cyc_str_sepstr(_Tmp0,
# 750
_tag_fat(" ",sizeof(char),2U));});
# 757
struct Cyc_List_List*stdinc_dirs=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);
stdinc_dirs=Cyc_add_subdir(stdinc_dirs,_tag_fat("include",sizeof(char),8U));
if(Cyc_strlen(Cyc_def_inc_path)> 0U)
stdinc_dirs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=Cyc_def_inc_path;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=stdinc_dirs;_Tmp0;});{
char*cyclone_include_path=getenv("CYCLONE_INCLUDE_PATH");
if(cyclone_include_path!=0)
stdinc_dirs=({struct Cyc_List_List*_Tmp0=Cyc_split_by_char(({char*_Tmp1=cyclone_include_path;_tag_fat((void*)_Tmp1,sizeof(char),_get_zero_arr_size_char((void*)_Tmp1,1U));}),':');Cyc_List_append(_Tmp0,stdinc_dirs);});{
struct _fat_ptr stdinc_string=({
struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=(struct _fat_ptr)_tag_fat("",sizeof(char),1U);_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,stdinc_dirs);_Tmp1->tl=_Tmp2;});_Tmp1;});
# 765
Cyc_str_sepstr(_Tmp0,
# 767
_tag_fat(" -I",sizeof(char),4U));});
struct _fat_ptr ofile_string;
if((int)Cyc_Flags_stop_after_pass <= 0)
ofile_string=Cyc_output_file==0?_tag_fat("",sizeof(char),1U):({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)*Cyc_output_file);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat(" > %s",sizeof(char),6U),_tag_fat(_Tmp1,sizeof(void*),1));});else{
# 772
ofile_string=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string((struct _fat_ptr)preprocfile));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat(" > %s",sizeof(char),6U),_tag_fat(_Tmp1,sizeof(void*),1));});}{
# 774
struct _fat_ptr fixup_string;
if(Cyc_produce_dependencies){
# 778
if(Cyc_dependencies_target==0)
# 782
fixup_string=_tag_fat(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",sizeof(char),35U);else{
# 787
fixup_string=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)*Cyc_dependencies_target);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat(" | sed 's/^.*\\.cyc\\.o:/%s:/'",sizeof(char),29U),_tag_fat(_Tmp1,sizeof(void*),1));});}}else{
# 790
fixup_string=_tag_fat("",sizeof(char),1U);}{
# 792
struct _fat_ptr cmd=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cpp);_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)((struct _fat_ptr)stdinc_string);_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
# 794
struct _fat_ptr _Tmp5=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(filename));_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)fixup_string);_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)ofile_string);_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_aprintf(_tag_fat("%s %s%s %s%s%s",sizeof(char),15U),_tag_fat(_Tmp6,sizeof(void*),6));});
# 796
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s\n",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});
if(system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)))!=0){
Cyc_compile_failure=1;
Cyc_fprintf(Cyc_stderr,_tag_fat("\nError: preprocessing\n",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));
if((int)Cyc_Flags_stop_after_pass > 0)Cyc_remove_file(preprocfile);
return;}
# 803
if((int)Cyc_Flags_stop_after_pass <= 0)return;
# 806
Cyc_Position_reset_position(preprocfile);{
struct Cyc___cycFILE*in_file=Cyc_try_file_open(preprocfile,_tag_fat("r",sizeof(char),2U),_tag_fat("file",sizeof(char),5U));
# 809
struct Cyc_List_List*tds=0;
struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables=0;
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_tc_init();
# 814
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 1)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("parsing",sizeof(char),8U),tds,Cyc_do_parse,_check_null(in_file),tds);
# 818
if(!Cyc_compile_failure && Cyc_Flags_porting_c_code){
Cyc_Port_port(tds);
Cyc_remove_file(preprocfile);
return;}
# 825
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 2)
({(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage;})(_tag_fat("binding",sizeof(char),8U),tds,Cyc_do_binding,1,1);
# 829
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 3)
({(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage;})(_tag_fat("current region resolution",sizeof(char),26U),tds,Cyc_do_adjust,1,1);
# 833
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 4)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("type checking",sizeof(char),14U),tds,Cyc_do_typecheck,te,tds);
# 837
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 5)
jump_tables=({(struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(int,struct Cyc_List_List*),int,struct Cyc_JumpAnalysis_Jump_Anal_Result*))Cyc_do_stage;})(_tag_fat("jump checking",sizeof(char),14U),tds,Cyc_do_jumpanalysis,1,0);
# 841
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 6)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("control-flow checking",sizeof(char),22U),tds,Cyc_do_cfcheck,_check_null(jump_tables),tds);
# 845
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 8)
({(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,int))Cyc_do_stage;})(_tag_fat("check insertion",sizeof(char),16U),tds,Cyc_do_insert_checks,_check_null(jump_tables),1);
# 849
if((!Cyc_compile_failure && Cyc_generate_interface_r)&&(int)Cyc_Flags_stop_after_pass >= 9){
# 851
struct Cyc___cycFILE*inter_file=Cyc_try_file_open(({const char*_Tmp0=(const char*)_check_null(_untag_fat_ptr(interfacefile,sizeof(char),1U));_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}),_tag_fat("w",sizeof(char),2U),_tag_fat("interface file",sizeof(char),15U));
if(inter_file==0)
Cyc_compile_failure=1;else{
# 855
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 862
if(!Cyc_compile_failure && Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_fat_ptr(interfacefile,sizeof(char),1U),"r");
struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open(interfaceobjfile,_tag_fat("w",sizeof(char),2U),_tag_fat("interface object file",sizeof(char),22U));
if(inter_objfile==0)
Cyc_compile_failure=1;else{
# 868
Cyc_Position_reset_position(interfacefile);{
struct _tuple15 int_env=({struct _tuple15 _Tmp0;_Tmp0.f0=te,_Tmp0.f1=inter_file,_Tmp0.f2=inter_objfile;_Tmp0;});
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple15*,struct Cyc_List_List*),struct _tuple15*,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("interface checking",sizeof(char),19U),tds,Cyc_do_interface,& int_env,tds);
if(inter_file!=0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);}}}}
# 879
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("translation to C",sizeof(char),17U),tds,Cyc_do_translate,_check_null(jump_tables)->pop_tables,tds);
# 883
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 10)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("aggregate removal",sizeof(char),18U),tds,Cyc_do_removeaggrs,1,tds);
# 886
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 11)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("unused label removal",sizeof(char),21U),tds,Cyc_do_removelabs,1,tds);
# 889
if((!Cyc_compile_failure && Cyc_toseqc_r)&&(int)Cyc_Flags_stop_after_pass >= 14)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("L-to-R evaluation order",sizeof(char),24U),tds,Cyc_do_toseqc,1,tds);
# 892
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
Cyc_Toc_finish();
# 895
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("low treehsaking",sizeof(char),16U),tds,Cyc_do_lowtreeshake,1,tds);
# 898
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 12)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("renaming temporaries",sizeof(char),21U),tds,Cyc_do_temprename,1,tds);
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 13)
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("removing casts",sizeof(char),15U),tds,Cyc_do_castremove,1,tds);
# 903
if(!Cyc_compile_failure &&(Cyc_Flags_tovc_r || Cyc_elim_se_r))
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("post-pass to VC",sizeof(char),16U),tds,Cyc_do_tovc,1,tds);
# 906
if(!Cyc_compile_failure)
Cyc_Warn_flush_warnings();
Cyc_remove_file(preprocfile);
if(Cyc_uncaught_exn!=0){
struct _tuple14*_Tmp0=_check_null(Cyc_uncaught_exn);int _Tmp1;const char*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f0;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{void*e=_Tmp3;const char*fn=_Tmp2;int line=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=(struct _fat_ptr)({const char*_Tmp7=Cyc_Core_get_exn_name(e);_tag_fat((void*)_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)({const char*_Tmp7=fn;_tag_fat((void*)_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)line;_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("uncaught exception: %s from %s, line %d\n",sizeof(char),41U),_tag_fat(_Tmp7,sizeof(void*),3));});
Cyc_fflush(Cyc_stderr);
return;}}{
# 918
struct Cyc___cycFILE*out_file;
if((int)Cyc_Flags_stop_after_pass <= 6){
if(Cyc_output_file!=0)
out_file=Cyc_try_file_open(*Cyc_output_file,_tag_fat("w",sizeof(char),2U),_tag_fat("output file",sizeof(char),12U));else{
# 923
out_file=Cyc_stdout;}}else{
if((int)Cyc_Flags_stop_after_pass < 15 && Cyc_output_file!=0)
out_file=Cyc_try_file_open(*Cyc_output_file,_tag_fat("w",sizeof(char),2U),_tag_fat("output file",sizeof(char),12U));else{
# 927
out_file=Cyc_try_file_open(cfile,_tag_fat("w",sizeof(char),2U),_tag_fat("output file",sizeof(char),12U));}}
# 929
if(Cyc_compile_failure || !((unsigned)out_file))return;
# 931
if(!Cyc_noprint_r){
struct _tuple16*env;env=_cycalloc(sizeof(struct _tuple16)),env->f0=out_file,env->f1=cfile;
tds=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple16*,struct Cyc_List_List*),struct _tuple16*,struct Cyc_List_List*))Cyc_do_stage;})(_tag_fat("printing",sizeof(char),9U),tds,Cyc_do_print,env,tds);
Cyc_file_close(out_file);
Cyc_cfiles=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_strdup((struct _fat_ptr)cfile);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cfiles;_Tmp0;});}}}}}}}}}}static char _TmpGC[8U]="<final>";
# 939
static struct _fat_ptr Cyc_final_str={_TmpGC,_TmpGC,_TmpGC + 8U};
static struct _fat_ptr*Cyc_final_strptr=& Cyc_final_str;
# 942
static struct Cyc_Interface_I*Cyc_read_cycio(struct _fat_ptr*n){
if(n==Cyc_final_strptr)
return Cyc_Interface_final();{
struct _fat_ptr basename;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){basename=Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_Tmp2)->tag==Cyc_Core_Invalid_argument){
basename=*n;goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}{
# 949
struct _fat_ptr nf=Cyc_strconcat(basename,_tag_fat(".cycio",sizeof(char),7U));
struct Cyc___cycFILE*f=Cyc_try_file_open(nf,_tag_fat("rb",sizeof(char),3U),_tag_fat("interface object file",sizeof(char),22U));
if(f==0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 956
Cyc_Position_reset_position(nf);{
struct Cyc_Interface_I*i=Cyc_Interface_load(f);
Cyc_file_close(f);
return i;}}}}
# 962
static int Cyc_is_cfile(struct _fat_ptr*n){
return(int)*((const char*)_check_fat_subscript(*n,sizeof(char),0))!=45;}
# 966
extern void GC_blacklist_warn_clear (void);struct _tuple17{struct _fat_ptr f0;int f1;struct _fat_ptr f2;void*f3;struct _fat_ptr f4;};
# 976
void Cyc_print_options (void);
# 984
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3,& Cyc_v_r};static char _TmpGD[3U]="-v";static char _TmpGE[1U]="";static char _TmpGF[35U]="print compilation stages verbosely";struct _tuple17 Cyc_opt1_tuple={.f0={_TmpGD,_TmpGD,_TmpGD + 3U},.f1=0,.f2={_TmpGE,_TmpGE,_TmpGE + 1U},.f3=(void*)& Cyc_opt1_spec,.f4={_TmpGF,_TmpGF,_TmpGF + 35U}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 986
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0,Cyc_print_version};static char _TmpG10[10U]="--version";static char _TmpG11[1U]="";static char _TmpG12[35U]="Print version information and exit";struct _tuple17 Cyc_opt2_tuple={.f0={_TmpG10,_TmpG10,_TmpG10 + 10U},.f1=0,.f2={_TmpG11,_TmpG11,_TmpG11 + 1U},.f3=(void*)& Cyc_opt2_spec,.f4={_TmpG12,_TmpG12,_TmpG12 + 35U}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 989
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5,Cyc_set_output_file};static char _TmpG13[3U]="-o";static char _TmpG14[8U]=" <file>";static char _TmpG15[35U]="Set the output file name to <file>";struct _tuple17 Cyc_opt3_tuple={.f0={_TmpG13,_TmpG13,_TmpG13 + 3U},.f1=0,.f2={_TmpG14,_TmpG14,_TmpG14 + 8U},.f3=(void*)& Cyc_opt3_spec,.f4={_TmpG15,_TmpG15,_TmpG15 + 35U}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 992
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1,Cyc_add_cpparg};static char _TmpG16[3U]="-D";static char _TmpG17[17U]="<name>[=<value>]";static char _TmpG18[32U]="Pass definition to preprocessor";struct _tuple17 Cyc_opt4_tuple={.f0={_TmpG16,_TmpG16,_TmpG16 + 3U},.f1=1,.f2={_TmpG17,_TmpG17,_TmpG17 + 17U},.f3=(void*)& Cyc_opt4_spec,.f4={_TmpG18,_TmpG18,_TmpG18 + 32U}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 995
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1,Cyc_Specsfile_add_cyclone_exec_path};static char _TmpG19[3U]="-B";static char _TmpG1A[7U]="<file>";static char _TmpG1B[60U]="Add to the list of directories to search for compiler files";struct _tuple17 Cyc_opt5_tuple={.f0={_TmpG19,_TmpG19,_TmpG19 + 3U},.f1=1,.f2={_TmpG1A,_TmpG1A,_TmpG1A + 7U},.f3=(void*)& Cyc_opt5_spec,.f4={_TmpG1B,_TmpG1B,_TmpG1B + 60U}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 998
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1,Cyc_add_cpparg};static char _TmpG1C[3U]="-I";static char _TmpG1D[6U]="<dir>";static char _TmpG1E[59U]="Add to the list of directories to search for include files";struct _tuple17 Cyc_opt6_tuple={.f0={_TmpG1C,_TmpG1C,_TmpG1C + 3U},.f1=1,.f2={_TmpG1D,_TmpG1D,_TmpG1D + 6U},.f3=(void*)& Cyc_opt6_spec,.f4={_TmpG1E,_TmpG1E,_TmpG1E + 59U}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 1001
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1,Cyc_add_ccarg};static char _TmpG1F[3U]="-L";static char _TmpG20[6U]="<dir>";static char _TmpG21[38U]="Add to the list of directories for -l";struct _tuple17 Cyc_opt7_tuple={.f0={_TmpG1F,_TmpG1F,_TmpG1F + 3U},.f1=1,.f2={_TmpG20,_TmpG20,_TmpG20 + 6U},.f3=(void*)& Cyc_opt7_spec,.f4={_TmpG21,_TmpG21,_TmpG21 + 38U}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 1004
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1,Cyc_add_libarg};static char _TmpG22[3U]="-l";static char _TmpG23[10U]="<libname>";static char _TmpG24[13U]="Library file";struct _tuple17 Cyc_opt8_tuple={.f0={_TmpG22,_TmpG22,_TmpG22 + 3U},.f1=1,.f2={_TmpG23,_TmpG23,_TmpG23 + 10U},.f3=(void*)& Cyc_opt8_spec,.f4={_TmpG24,_TmpG24,_TmpG24 + 13U}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 1007
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1,Cyc_add_marg};static char _TmpG25[3U]="-m";static char _TmpG26[9U]="<option>";static char _TmpG27[52U]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple17 Cyc_opt9_tuple={.f0={_TmpG25,_TmpG25,_TmpG25 + 3U},.f1=1,.f2={_TmpG26,_TmpG26,_TmpG26 + 9U},.f3=(void*)& Cyc_opt9_spec,.f4={_TmpG27,_TmpG27,_TmpG27 + 52U}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 1010
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0,Cyc_set_stop_after_objectfile};static char _TmpG28[3U]="-c";static char _TmpG29[1U]="";static char _TmpG2A[61U]="Produce an object file instead of an executable; do not link";struct _tuple17 Cyc_opt10_tuple={.f0={_TmpG28,_TmpG28,_TmpG28 + 3U},.f1=0,.f2={_TmpG29,_TmpG29,_TmpG29 + 1U},.f3=(void*)& Cyc_opt10_spec,.f4={_TmpG2A,_TmpG2A,_TmpG2A + 61U}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 1013
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5,Cyc_set_inputtype};static char _TmpG2B[3U]="-x";static char _TmpG2C[12U]=" <language>";static char _TmpG2D[49U]="Specify <language> for the following input files";struct _tuple17 Cyc_opt11_tuple={.f0={_TmpG2B,_TmpG2B,_TmpG2B + 3U},.f1=0,.f2={_TmpG2C,_TmpG2C,_TmpG2C + 12U},.f3=(void*)& Cyc_opt11_spec,.f4={_TmpG2D,_TmpG2D,_TmpG2D + 49U}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 1016
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0,Cyc_set_pa};static char _TmpG2E[4U]="-pa";static char _TmpG2F[1U]="";static char _TmpG30[33U]="Compile for profiling with aprof";struct _tuple17 Cyc_opt12_tuple={.f0={_TmpG2E,_TmpG2E,_TmpG2E + 4U},.f1=0,.f2={_TmpG2F,_TmpG2F,_TmpG2F + 1U},.f3=(void*)& Cyc_opt12_spec,.f4={_TmpG30,_TmpG30,_TmpG30 + 33U}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 1019
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0,Cyc_set_stop_after_asmfile};static char _TmpG31[3U]="-S";static char _TmpG32[1U]="";static char _TmpG33[35U]="Stop after producing assembly code";struct _tuple17 Cyc_opt13_tuple={.f0={_TmpG31,_TmpG31,_TmpG31 + 3U},.f1=0,.f2={_TmpG32,_TmpG32,_TmpG32 + 1U},.f3=(void*)& Cyc_opt13_spec,.f4={_TmpG33,_TmpG33,_TmpG33 + 35U}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 1022
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0,Cyc_set_produce_dependencies};static char _TmpG34[3U]="-M";static char _TmpG35[1U]="";static char _TmpG36[21U]="Produce dependencies";struct _tuple17 Cyc_opt14_tuple={.f0={_TmpG34,_TmpG34,_TmpG34 + 3U},.f1=0,.f2={_TmpG35,_TmpG35,_TmpG35 + 1U},.f3=(void*)& Cyc_opt14_spec,.f4={_TmpG36,_TmpG36,_TmpG36 + 21U}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1025
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1,Cyc_add_cpparg};static char _TmpG37[4U]="-MG";static char _TmpG38[1U]="";static char _TmpG39[68U]="When producing dependencies assume that missing files are generated";struct _tuple17 Cyc_opt15_tuple={.f0={_TmpG37,_TmpG37,_TmpG37 + 4U},.f1=0,.f2={_TmpG38,_TmpG38,_TmpG38 + 1U},.f3=(void*)& Cyc_opt15_spec,.f4={_TmpG39,_TmpG39,_TmpG39 + 68U}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1029
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5,Cyc_set_dependencies_target};static char _TmpG3A[4U]="-MT";static char _TmpG3B[10U]=" <target>";static char _TmpG3C[29U]="Give target for dependencies";struct _tuple17 Cyc_opt16_tuple={.f0={_TmpG3A,_TmpG3A,_TmpG3A + 4U},.f1=0,.f2={_TmpG3B,_TmpG3B,_TmpG3B + 10U},.f3=(void*)& Cyc_opt16_spec,.f4={_TmpG3C,_TmpG3C,_TmpG3C + 29U}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1032
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5,Cyc_Specsfile_set_target_arch};static char _TmpG3D[3U]="-b";static char _TmpG3E[10U]="<machine>";static char _TmpG3F[19U]="Set target machine";struct _tuple17 Cyc_opt17_tuple={.f0={_TmpG3D,_TmpG3D,_TmpG3D + 3U},.f1=0,.f2={_TmpG3E,_TmpG3E,_TmpG3E + 10U},.f3=(void*)& Cyc_opt17_spec,.f4={_TmpG3F,_TmpG3F,_TmpG3F + 19U}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1035
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3,& Cyc_Flags_warn_lose_unique};static char _TmpG40[14U]="-Wlose-unique";static char _TmpG41[1U]="";static char _TmpG42[49U]="Try to warn when a unique pointer might get lost";struct _tuple17 Cyc_opt18_tuple={.f0={_TmpG40,_TmpG40,_TmpG40 + 14U},.f1=0,.f2={_TmpG41,_TmpG41,_TmpG41 + 1U},.f3=(void*)& Cyc_opt18_spec,.f4={_TmpG42,_TmpG42,_TmpG42 + 49U}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1038
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3,& Cyc_Flags_warn_override};static char _TmpG43[11U]="-Woverride";static char _TmpG44[1U]="";static char _TmpG45[58U]="Warn when a local variable overrides an existing variable";struct _tuple17 Cyc_opt19_tuple={.f0={_TmpG43,_TmpG43,_TmpG43 + 11U},.f1=0,.f2={_TmpG44,_TmpG44,_TmpG44 + 1U},.f3=(void*)& Cyc_opt19_spec,.f4={_TmpG45,_TmpG45,_TmpG45 + 58U}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1041
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0,Cyc_Flags_set_all_warnings};static char _TmpG46[6U]="-Wall";static char _TmpG47[1U]="";static char _TmpG48[21U]="Turn on all warnings";struct _tuple17 Cyc_opt20_tuple={.f0={_TmpG46,_TmpG46,_TmpG46 + 6U},.f1=0,.f2={_TmpG47,_TmpG47,_TmpG47 + 1U},.f3=(void*)& Cyc_opt20_spec,.f4={_TmpG48,_TmpG48,_TmpG48 + 21U}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1044
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt21_spec={0,Cyc_Flags_set_cpponly};static char _TmpG49[3U]="-E";static char _TmpG4A[1U]="";static char _TmpG4B[25U]="Stop after preprocessing";struct _tuple17 Cyc_opt21_tuple={.f0={_TmpG49,_TmpG49,_TmpG49 + 3U},.f1=0,.f2={_TmpG4A,_TmpG4A,_TmpG4A + 1U},.f3=(void*)& Cyc_opt21_spec,.f4={_TmpG4B,_TmpG4B,_TmpG4B + 25U}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1047
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt22_spec={0,Cyc_Flags_set_parseonly};static char _TmpG4C[17U]="-stopafter-parse";static char _TmpG4D[1U]="";static char _TmpG4E[19U]="Stop after parsing";struct _tuple17 Cyc_opt22_tuple={.f0={_TmpG4C,_TmpG4C,_TmpG4C + 17U},.f1=0,.f2={_TmpG4D,_TmpG4D,_TmpG4D + 1U},.f3=(void*)& Cyc_opt22_spec,.f4={_TmpG4E,_TmpG4E,_TmpG4E + 19U}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1050
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt23_spec={0,Cyc_Flags_set_tconly};static char _TmpG4F[14U]="-stopafter-tc";static char _TmpG50[1U]="";static char _TmpG51[25U]="Stop after type checking";struct _tuple17 Cyc_opt23_tuple={.f0={_TmpG4F,_TmpG4F,_TmpG4F + 14U},.f1=0,.f2={_TmpG50,_TmpG50,_TmpG50 + 1U},.f3=(void*)& Cyc_opt23_spec,.f4={_TmpG51,_TmpG51,_TmpG51 + 25U}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1053
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3,& Cyc_noprint_r};static char _TmpG52[9U]="-noprint";static char _TmpG53[1U]="";static char _TmpG54[42U]="Do not print output (when stopping early)";struct _tuple17 Cyc_opt24_tuple={.f0={_TmpG52,_TmpG52,_TmpG52 + 9U},.f1=0,.f2={_TmpG53,_TmpG53,_TmpG53 + 1U},.f3=(void*)& Cyc_opt24_spec,.f4={_TmpG54,_TmpG54,_TmpG54 + 42U}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1056
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt25_spec={0,Cyc_Flags_set_cfonly};static char _TmpG55[14U]="-stopafter-cf";static char _TmpG56[1U]="";static char _TmpG57[33U]="Stop after control-flow checking";struct _tuple17 Cyc_opt25_tuple={.f0={_TmpG55,_TmpG55,_TmpG55 + 14U},.f1=0,.f2={_TmpG56,_TmpG56,_TmpG56 + 1U},.f3=(void*)& Cyc_opt25_spec,.f4={_TmpG57,_TmpG57,_TmpG57 + 33U}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1059
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt26_spec={0,Cyc_Flags_set_toconly};static char _TmpG58[15U]="-stopafter-toc";static char _TmpG59[1U]="";static char _TmpG5A[28U]="Stop after translation to C";struct _tuple17 Cyc_opt26_tuple={.f0={_TmpG58,_TmpG58,_TmpG58 + 15U},.f1=0,.f2={_TmpG59,_TmpG59,_TmpG59 + 1U},.f3=(void*)& Cyc_opt26_spec,.f4={_TmpG5A,_TmpG5A,_TmpG5A + 28U}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1062
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3,& Cyc_ic_r};static char _TmpG5B[4U]="-ic";static char _TmpG5C[1U]="";static char _TmpG5D[26U]="Activate the link-checker";struct _tuple17 Cyc_opt27_tuple={.f0={_TmpG5B,_TmpG5B,_TmpG5B + 4U},.f1=0,.f2={_TmpG5C,_TmpG5C,_TmpG5C + 1U},.f3=(void*)& Cyc_opt27_spec,.f4={_TmpG5D,_TmpG5D,_TmpG5D + 26U}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1065
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3,& Cyc_pp_r};static char _TmpG5E[4U]="-pp";static char _TmpG5F[1U]="";static char _TmpG60[47U]="Pretty print the C code that Cyclone generates";struct _tuple17 Cyc_opt28_tuple={.f0={_TmpG5E,_TmpG5E,_TmpG5E + 4U},.f1=0,.f2={_TmpG5F,_TmpG5F,_TmpG5F + 1U},.f3=(void*)& Cyc_opt28_spec,.f4={_TmpG60,_TmpG60,_TmpG60 + 47U}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1068
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4,& Cyc_pp_r};static char _TmpG61[4U]="-up";static char _TmpG62[1U]="";static char _TmpG63[55U]="Ugly print the C code that Cyclone generates (default)";struct _tuple17 Cyc_opt29_tuple={.f0={_TmpG61,_TmpG61,_TmpG61 + 4U},.f1=0,.f2={_TmpG62,_TmpG62,_TmpG62 + 1U},.f3=(void*)& Cyc_opt29_spec,.f4={_TmpG63,_TmpG63,_TmpG63 + 55U}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1071
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3,& Cyc_elim_se_r};static char _TmpG64[28U]="-elim-statement-expressions";static char _TmpG65[1U]="";static char _TmpG66[66U]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple17 Cyc_opt30_tuple={.f0={_TmpG64,_TmpG64,_TmpG64 + 28U},.f1=0,.f2={_TmpG65,_TmpG65,_TmpG65 + 1U},.f3=(void*)& Cyc_opt30_spec,.f4={_TmpG66,_TmpG66,_TmpG66 + 66U}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1075
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4,& Cyc_elim_se_r};static char _TmpG67[31U]="-no-elim-statement-expressions";static char _TmpG68[1U]="";static char _TmpG69[47U]="Do not avoid statement expressions in C output";struct _tuple17 Cyc_opt31_tuple={.f0={_TmpG67,_TmpG67,_TmpG67 + 31U},.f1=0,.f2={_TmpG68,_TmpG68,_TmpG68 + 1U},.f3=(void*)& Cyc_opt31_spec,.f4={_TmpG69,_TmpG69,_TmpG69 + 47U}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1078
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0,Cyc_set_tovc};static char _TmpG6A[8U]="-un-gcc";static char _TmpG6B[1U]="";static char _TmpG6C[57U]="Avoid gcc extensions in C output (except for attributes)";struct _tuple17 Cyc_opt32_tuple={.f0={_TmpG6A,_TmpG6A,_TmpG6A + 8U},.f1=0,.f2={_TmpG6B,_TmpG6B,_TmpG6B + 1U},.f3=(void*)& Cyc_opt32_spec,.f4={_TmpG6C,_TmpG6C,_TmpG6C + 57U}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1081
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5,Cyc_set_c_compiler};static char _TmpG6D[8U]="-c-comp";static char _TmpG6E[12U]=" <compiler>";static char _TmpG6F[40U]="Produce C output for the given compiler";struct _tuple17 Cyc_opt33_tuple={.f0={_TmpG6D,_TmpG6D,_TmpG6D + 8U},.f1=0,.f2={_TmpG6E,_TmpG6E,_TmpG6E + 12U},.f3=(void*)& Cyc_opt33_spec,.f4={_TmpG6F,_TmpG6F,_TmpG6F + 40U}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1084
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0,Cyc_set_save_temps};static char _TmpG70[12U]="-save-temps";static char _TmpG71[1U]="";static char _TmpG72[29U]="Don't delete temporary files";struct _tuple17 Cyc_opt34_tuple={.f0={_TmpG70,_TmpG70,_TmpG70 + 12U},.f1=0,.f2={_TmpG71,_TmpG71,_TmpG71 + 1U},.f3=(void*)& Cyc_opt34_spec,.f4={_TmpG72,_TmpG72,_TmpG72 + 29U}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1087
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3,& Cyc_save_c_r};static char _TmpG73[8U]="-save-c";static char _TmpG74[1U]="";static char _TmpG75[31U]="Don't delete temporary C files";struct _tuple17 Cyc_opt35_tuple={.f0={_TmpG73,_TmpG73,_TmpG73 + 8U},.f1=0,.f2={_TmpG74,_TmpG74,_TmpG74 + 1U},.f3=(void*)& Cyc_opt35_spec,.f4={_TmpG75,_TmpG75,_TmpG75 + 31U}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1090
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0,Cyc_clear_lineno};static char _TmpG76[11U]="--nolineno";static char _TmpG77[1U]="";static char _TmpG78[63U]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple17 Cyc_opt36_tuple={.f0={_TmpG76,_TmpG76,_TmpG76 + 11U},.f1=0,.f2={_TmpG77,_TmpG77,_TmpG77 + 1U},.f3=(void*)& Cyc_opt36_spec,.f4={_TmpG78,_TmpG78,_TmpG78 + 63U}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1093
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0,Cyc_set_nochecks};static char _TmpG79[11U]="--nochecks";static char _TmpG7A[1U]="";static char _TmpG7B[27U]="Disable bounds/null checks";struct _tuple17 Cyc_opt37_tuple={.f0={_TmpG79,_TmpG79,_TmpG79 + 11U},.f1=0,.f2={_TmpG7A,_TmpG7A,_TmpG7A + 1U},.f3=(void*)& Cyc_opt37_spec,.f4={_TmpG7B,_TmpG7B,_TmpG7B + 27U}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1096
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0,Cyc_set_nonullchecks};static char _TmpG7C[15U]="--nonullchecks";static char _TmpG7D[1U]="";static char _TmpG7E[20U]="Disable null checks";struct _tuple17 Cyc_opt38_tuple={.f0={_TmpG7C,_TmpG7C,_TmpG7C + 15U},.f1=0,.f2={_TmpG7D,_TmpG7D,_TmpG7D + 1U},.f3=(void*)& Cyc_opt38_spec,.f4={_TmpG7E,_TmpG7E,_TmpG7E + 20U}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1099
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0,Cyc_set_noboundschecks};static char _TmpG7F[17U]="--noboundschecks";static char _TmpG80[1U]="";static char _TmpG81[22U]="Disable bounds checks";struct _tuple17 Cyc_opt39_tuple={.f0={_TmpG7F,_TmpG7F,_TmpG7F + 17U},.f1=0,.f2={_TmpG80,_TmpG80,_TmpG80 + 1U},.f3=(void*)& Cyc_opt39_spec,.f4={_TmpG81,_TmpG81,_TmpG81 + 22U}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1102
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5,Cyc_set_cpp};static char _TmpG82[9U]="-use-cpp";static char _TmpG83[7U]="<path>";static char _TmpG84[35U]="Indicate which preprocessor to use";struct _tuple17 Cyc_opt41_tuple={.f0={_TmpG82,_TmpG82,_TmpG82 + 9U},.f1=0,.f2={_TmpG83,_TmpG83,_TmpG83 + 7U},.f3=(void*)& Cyc_opt41_spec,.f4={_TmpG84,_TmpG84,_TmpG84 + 35U}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1105
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0,Cyc_set_nocppprecomp};static char _TmpG85[16U]="-no-cpp-precomp";static char _TmpG86[1U]="";static char _TmpG87[40U]="Don't do smart preprocessing (mac only)";struct _tuple17 Cyc_opt42_tuple={.f0={_TmpG85,_TmpG85,_TmpG85 + 16U},.f1=0,.f2={_TmpG86,_TmpG86,_TmpG86 + 1U},.f3=(void*)& Cyc_opt42_spec,.f4={_TmpG87,_TmpG87,_TmpG87 + 40U}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1108
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0,Cyc_set_nocyc};static char _TmpG88[7U]="-nocyc";static char _TmpG89[1U]="";static char _TmpG8A[33U]="Don't add implicit namespace Cyc";struct _tuple17 Cyc_opt43_tuple={.f0={_TmpG88,_TmpG88,_TmpG88 + 7U},.f1=0,.f2={_TmpG89,_TmpG89,_TmpG89 + 1U},.f3=(void*)& Cyc_opt43_spec,.f4={_TmpG8A,_TmpG8A,_TmpG8A + 33U}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1111
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3,& Cyc_nogc_r};static char _TmpG8B[6U]="-nogc";static char _TmpG8C[1U]="";static char _TmpG8D[36U]="Don't link in the garbage collector";struct _tuple17 Cyc_opt44_tuple={.f0={_TmpG8B,_TmpG8B,_TmpG8B + 6U},.f1=0,.f2={_TmpG8C,_TmpG8C,_TmpG8C + 1U},.f3=(void*)& Cyc_opt44_spec,.f4={_TmpG8D,_TmpG8D,_TmpG8D + 36U}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1114
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3,& Cyc_noshake_r};static char _TmpG8E[16U]="-noremoveunused";static char _TmpG8F[1U]="";static char _TmpG90[49U]="Don't remove externed variables that aren't used";struct _tuple17 Cyc_opt45_tuple={.f0={_TmpG8E,_TmpG8E,_TmpG8E + 16U},.f1=0,.f2={_TmpG8F,_TmpG8F,_TmpG8F + 1U},.f3=(void*)& Cyc_opt45_spec,.f4={_TmpG90,_TmpG90,_TmpG90 + 49U}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1117
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3,& Cyc_Flags_noexpand_r};static char _TmpG91[18U]="-noexpandtypedefs";static char _TmpG92[1U]="";static char _TmpG93[41U]="Don't expand typedefs in pretty printing";struct _tuple17 Cyc_opt46_tuple={.f0={_TmpG91,_TmpG91,_TmpG91 + 18U},.f1=0,.f2={_TmpG92,_TmpG92,_TmpG92 + 1U},.f3=(void*)& Cyc_opt46_spec,.f4={_TmpG93,_TmpG93,_TmpG93 + 41U}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1120
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3,& Cyc_print_all_tvars_r};static char _TmpG94[15U]="-printalltvars";static char _TmpG95[1U]="";static char _TmpG96[57U]="Print all type variables (even implicit default effects)";struct _tuple17 Cyc_opt47_tuple={.f0={_TmpG94,_TmpG94,_TmpG94 + 15U},.f1=0,.f2={_TmpG95,_TmpG95,_TmpG95 + 1U},.f3=(void*)& Cyc_opt47_spec,.f4={_TmpG96,_TmpG96,_TmpG96 + 57U}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1123
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3,& Cyc_print_all_kinds_r};static char _TmpG97[15U]="-printallkinds";static char _TmpG98[1U]="";static char _TmpG99[37U]="Always print kinds of type variables";struct _tuple17 Cyc_opt48_tuple={.f0={_TmpG97,_TmpG97,_TmpG97 + 15U},.f1=0,.f2={_TmpG98,_TmpG98,_TmpG98 + 1U},.f3=(void*)& Cyc_opt48_spec,.f4={_TmpG99,_TmpG99,_TmpG99 + 37U}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1126
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3,& Cyc_print_full_evars_r};static char _TmpG9A[16U]="-printfullevars";static char _TmpG9B[1U]="";static char _TmpG9C[50U]="Print full information for evars (type debugging)";struct _tuple17 Cyc_opt49_tuple={.f0={_TmpG9A,_TmpG9A,_TmpG9A + 16U},.f1=0,.f2={_TmpG9B,_TmpG9B,_TmpG9B + 1U},.f3=(void*)& Cyc_opt49_spec,.f4={_TmpG9C,_TmpG9C,_TmpG9C + 50U}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1129
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3,& Cyc_print_all_effects_r};static char _TmpG9D[17U]="-printalleffects";static char _TmpG9E[1U]="";static char _TmpG9F[45U]="Print effects for functions (type debugging)";struct _tuple17 Cyc_opt50_tuple={.f0={_TmpG9D,_TmpG9D,_TmpG9D + 17U},.f1=0,.f2={_TmpG9E,_TmpG9E,_TmpG9E + 1U},.f3=(void*)& Cyc_opt50_spec,.f4={_TmpG9F,_TmpG9F,_TmpG9F + 45U}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1132
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3,& Cyc_nocyc_setjmp_r};static char _TmpGA0[14U]="-nocyc_setjmp";static char _TmpGA1[1U]="";static char _TmpGA2[46U]="Do not use compiler special file cyc_setjmp.h";struct _tuple17 Cyc_opt51_tuple={.f0={_TmpGA0,_TmpGA0,_TmpGA0 + 14U},.f1=0,.f2={_TmpGA1,_TmpGA1,_TmpGA1 + 1U},.f3=(void*)& Cyc_opt51_spec,.f4={_TmpGA2,_TmpGA2,_TmpGA2 + 46U}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1135
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3,& Cyc_Flags_compile_for_boot};static char _TmpGA3[18U]="-compile-for-boot";static char _TmpGA4[1U]="";static char _TmpGA5[71U]="Compile using the special boot library instead of the standard library";struct _tuple17 Cyc_opt52_tuple={.f0={_TmpGA3,_TmpGA3,_TmpGA3 + 18U},.f1=0,.f2={_TmpGA4,_TmpGA4,_TmpGA4 + 1U},.f3=(void*)& Cyc_opt52_spec,.f4={_TmpGA5,_TmpGA5,_TmpGA5 + 71U}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1138
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5,Cyc_set_cyc_include};static char _TmpGA6[4U]="-CI";static char _TmpGA7[8U]=" <file>";static char _TmpGA8[31U]="Set cyc_include.h to be <file>";struct _tuple17 Cyc_opt53_tuple={.f0={_TmpGA6,_TmpGA6,_TmpGA6 + 4U},.f1=0,.f2={_TmpGA7,_TmpGA7,_TmpGA7 + 8U},.f3=(void*)& Cyc_opt53_spec,.f4={_TmpGA8,_TmpGA8,_TmpGA8 + 31U}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1141
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3,& Cyc_Flags_warn_bounds_checks};static char _TmpGA9[18U]="-warnboundschecks";static char _TmpGAA[1U]="";static char _TmpGAB[44U]="Warn when bounds checks can't be eliminated";struct _tuple17 Cyc_opt54_tuple={.f0={_TmpGA9,_TmpGA9,_TmpGA9 + 18U},.f1=0,.f2={_TmpGAA,_TmpGAA,_TmpGAA + 1U},.f3=(void*)& Cyc_opt54_spec,.f4={_TmpGAB,_TmpGAB,_TmpGAB + 44U}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1144
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3,& Cyc_Flags_warn_all_null_deref};static char _TmpGAC[16U]="-warnnullchecks";static char _TmpGAD[1U]="";static char _TmpGAE[45U]="Warn when any null check can't be eliminated";struct _tuple17 Cyc_opt55_tuple={.f0={_TmpGAC,_TmpGAC,_TmpGAC + 16U},.f1=0,.f2={_TmpGAD,_TmpGAD,_TmpGAD + 1U},.f3=(void*)& Cyc_opt55_spec,.f4={_TmpGAE,_TmpGAE,_TmpGAE + 45U}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1147
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3,& Cyc_Flags_warn_alias_coerce};static char _TmpGAF[19U]="-warnaliascoercion";static char _TmpGB0[1U]="";static char _TmpGB1[41U]="Warn when any alias coercion is inserted";struct _tuple17 Cyc_opt56_tuple={.f0={_TmpGAF,_TmpGAF,_TmpGAF + 19U},.f1=0,.f2={_TmpGB0,_TmpGB0,_TmpGB0 + 1U},.f3=(void*)& Cyc_opt56_spec,.f4={_TmpGB1,_TmpGB1,_TmpGB1 + 41U}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1150
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3,& Cyc_Flags_warn_region_coerce};static char _TmpGB2[18U]="-warnregioncoerce";static char _TmpGB3[1U]="";static char _TmpGB4[42U]="Warn when any region coercion is inserted";struct _tuple17 Cyc_opt57_tuple={.f0={_TmpGB2,_TmpGB2,_TmpGB2 + 18U},.f1=0,.f2={_TmpGB3,_TmpGB3,_TmpGB3 + 1U},.f3=(void*)& Cyc_opt57_spec,.f4={_TmpGB4,_TmpGB4,_TmpGB4 + 42U}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1153
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3,& Cyc_Flags_no_register};static char _TmpGB5[12U]="-noregister";static char _TmpGB6[1U]="";static char _TmpGB7[39U]="Treat register storage class as public";struct _tuple17 Cyc_opt58_tuple={.f0={_TmpGB5,_TmpGB5,_TmpGB5 + 12U},.f1=0,.f2={_TmpGB6,_TmpGB6,_TmpGB6 + 1U},.f3=(void*)& Cyc_opt58_spec,.f4={_TmpGB7,_TmpGB7,_TmpGB7 + 39U}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1156
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4,& Cyc_Position_use_gcc_style_location};static char _TmpGB8[18U]="-detailedlocation";static char _TmpGB9[1U]="";static char _TmpGBA[58U]="Try to give more detailed location information for errors";struct _tuple17 Cyc_opt59_tuple={.f0={_TmpGB8,_TmpGB8,_TmpGB8 + 18U},.f1=0,.f2={_TmpGB9,_TmpGB9,_TmpGB9 + 1U},.f3=(void*)& Cyc_opt59_spec,.f4={_TmpGBA,_TmpGBA,_TmpGBA + 58U}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1159
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0,Cyc_set_port_c_code};static char _TmpGBB[6U]="-port";static char _TmpGBC[1U]="";static char _TmpGBD[38U]="Suggest how to port C code to Cyclone";struct _tuple17 Cyc_opt60_tuple={.f0={_TmpGBB,_TmpGBB,_TmpGBB + 6U},.f1=0,.f2={_TmpGBC,_TmpGBC,_TmpGBC + 1U},.f3=(void*)& Cyc_opt60_spec,.f4={_TmpGBD,_TmpGBD,_TmpGBD + 38U}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1162
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3,& Cyc_Flags_no_regions};static char _TmpGBE[11U]="-noregions";static char _TmpGBF[1U]="";static char _TmpGC0[39U]="Generate code that doesn't use regions";struct _tuple17 Cyc_opt61_tuple={.f0={_TmpGBE,_TmpGBE,_TmpGBE + 11U},.f1=0,.f2={_TmpGBF,_TmpGBF,_TmpGBF + 1U},.f3=(void*)& Cyc_opt61_spec,.f4={_TmpGC0,_TmpGC0,_TmpGC0 + 39U}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1165
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5,Cyc_add_iprefix};static char _TmpGC1[9U]="-iprefix";static char _TmpGC2[9U]="<prefix>";static char _TmpGC3[67U]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple17 Cyc_opt62_tuple={.f0={_TmpGC1,_TmpGC1,_TmpGC1 + 9U},.f1=0,.f2={_TmpGC2,_TmpGC2,_TmpGC2 + 9U},.f3=(void*)& Cyc_opt62_spec,.f4={_TmpGC3,_TmpGC3,_TmpGC3 + 67U}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1168
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5,Cyc_add_iwithprefix};static char _TmpGC4[13U]="-iwithprefix";static char _TmpGC5[6U]="<dir>";static char _TmpGC6[47U]="Add <prefix>/<dir> to the second include path.";struct _tuple17 Cyc_opt63_tuple={.f0={_TmpGC4,_TmpGC4,_TmpGC4 + 13U},.f1=0,.f2={_TmpGC5,_TmpGC5,_TmpGC5 + 6U},.f3=(void*)& Cyc_opt63_spec,.f4={_TmpGC6,_TmpGC6,_TmpGC6 + 47U}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1171
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5,Cyc_add_iwithprefixbefore};static char _TmpGC7[19U]="-iwithprefixbefore";static char _TmpGC8[6U]="<dir>";static char _TmpGC9[45U]="Add <prefix>/<dir> to the main include path.";struct _tuple17 Cyc_opt64_tuple={.f0={_TmpGC7,_TmpGC7,_TmpGC7 + 19U},.f1=0,.f2={_TmpGC8,_TmpGC8,_TmpGC8 + 6U},.f3=(void*)& Cyc_opt64_spec,.f4={_TmpGC9,_TmpGC9,_TmpGC9 + 45U}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1174
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5,Cyc_add_isystem};static char _TmpGCA[9U]="-isystem";static char _TmpGCB[6U]="<dir>";static char _TmpGCC[90U]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple17 Cyc_opt65_tuple={.f0={_TmpGCA,_TmpGCA,_TmpGCA + 9U},.f1=0,.f2={_TmpGCB,_TmpGCB,_TmpGCB + 6U},.f3=(void*)& Cyc_opt65_spec,.f4={_TmpGCC,_TmpGCC,_TmpGCC + 90U}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1177
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5,Cyc_add_idirafter};static char _TmpGCD[11U]="-idirafter";static char _TmpGCE[6U]="<dir>";static char _TmpGCF[46U]="Add the directory to the second include path.";struct _tuple17 Cyc_opt66_tuple={.f0={_TmpGCD,_TmpGCD,_TmpGCD + 11U},.f1=0,.f2={_TmpGCE,_TmpGCE,_TmpGCE + 6U},.f3=(void*)& Cyc_opt66_spec,.f4={_TmpGCF,_TmpGCF,_TmpGCF + 46U}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1180
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3,& Cyc_generate_interface_r};static char _TmpGD0[15U]="--geninterface";static char _TmpGD1[1U]="";static char _TmpGD2[25U]="Generate interface files";struct _tuple17 Cyc_opt67_tuple={.f0={_TmpGD0,_TmpGD0,_TmpGD0 + 15U},.f1=0,.f2={_TmpGD1,_TmpGD1,_TmpGD1 + 1U},.f3=(void*)& Cyc_opt67_spec,.f4={_TmpGD2,_TmpGD2,_TmpGD2 + 25U}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1183
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5,Cyc_set_specified_interface};static char _TmpGD3[12U]="--interface";static char _TmpGD4[8U]=" <file>";static char _TmpGD5[37U]="Set the interface file to be <file>.";struct _tuple17 Cyc_opt68_tuple={.f0={_TmpGD3,_TmpGD3,_TmpGD3 + 12U},.f1=0,.f2={_TmpGD4,_TmpGD4,_TmpGD4 + 8U},.f3=(void*)& Cyc_opt68_spec,.f4={_TmpGD5,_TmpGD5,_TmpGD5 + 37U}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1186
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0,Cyc_set_many_errors};static char _TmpGD6[13U]="--manyerrors";static char _TmpGD7[1U]="";static char _TmpGD8[36U]="don't stop after only a few errors.";struct _tuple17 Cyc_opt69_tuple={.f0={_TmpGD6,_TmpGD6,_TmpGD6 + 13U},.f1=0,.f2={_TmpGD7,_TmpGD7,_TmpGD7 + 1U},.f3=(void*)& Cyc_opt69_spec,.f4={_TmpGD8,_TmpGD8,_TmpGD8 + 36U}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1189
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3,& Cyc_Flags_print_parser_state_and_token};static char _TmpGD9[13U]="--parsestate";static char _TmpGDA[1U]="";static char _TmpGDB[50U]="print the parse state and token on a syntax error";struct _tuple17 Cyc_opt70_tuple={.f0={_TmpGD9,_TmpGD9,_TmpGD9 + 13U},.f1=0,.f2={_TmpGDA,_TmpGDA,_TmpGDA + 1U},.f3=(void*)& Cyc_opt70_spec,.f4={_TmpGDB,_TmpGDB,_TmpGDB + 50U}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1192
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0,Cyc_noassume_gcc_flag};static char _TmpGDC[22U]="-known-gcc-flags-only";static char _TmpGDD[1U]="";static char _TmpGDE[57U]="do not assume that unknown flags should be passed to gcc";struct _tuple17 Cyc_opt71_tuple={.f0={_TmpGDC,_TmpGDC,_TmpGDC + 22U},.f1=0,.f2={_TmpGDD,_TmpGDD,_TmpGDD + 1U},.f3=(void*)& Cyc_opt71_spec,.f4={_TmpGDE,_TmpGDE,_TmpGDE + 57U}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1195
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0,Cyc_print_options};static char _TmpGDF[6U]="-help";static char _TmpGE0[1U]="";static char _TmpGE1[32U]="print out the available options";struct _tuple17 Cyc_opt72_tuple={.f0={_TmpGDF,_TmpGDF,_TmpGDF + 6U},.f1=0,.f2={_TmpGE0,_TmpGE0,_TmpGE0 + 1U},.f3=(void*)& Cyc_opt72_spec,.f4={_TmpGE1,_TmpGE1,_TmpGE1 + 32U}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1198
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0,Cyc_print_options};static char _TmpGE2[7U]="-usage";static char _TmpGE3[1U]="";static char _TmpGE4[32U]="print out the available options";struct _tuple17 Cyc_opt73_tuple={.f0={_TmpGE2,_TmpGE2,_TmpGE2 + 7U},.f1=0,.f2={_TmpGE3,_TmpGE3,_TmpGE3 + 1U},.f3=(void*)& Cyc_opt73_spec,.f4={_TmpGE4,_TmpGE4,_TmpGE4 + 32U}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1201
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt74_spec={0,Cyc_set_notoseqc};static char _TmpGE5[10U]="-no-seq-c";static char _TmpGE6[1U]="";static char _TmpGE7[71U]="Do not force left-to-right evaluation order of generated code (unsafe)";struct _tuple17 Cyc_opt74_tuple={.f0={_TmpGE5,_TmpGE5,_TmpGE5 + 10U},.f1=0,.f2={_TmpGE6,_TmpGE6,_TmpGE6 + 1U},.f3=(void*)& Cyc_opt74_spec,.f4={_TmpGE7,_TmpGE7,_TmpGE7 + 71U}};struct Cyc_List_List Cyc_opt74={(void*)& Cyc_opt74_tuple,0};
# 1204
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt75_spec={0,Cyc_set_pg};static char _TmpGE8[4U]="-pg";static char _TmpGE9[1U]="";static char _TmpGEA[33U]="Compile for profiling with gprof";struct _tuple17 Cyc_opt75_tuple={.f0={_TmpGE8,_TmpGE8,_TmpGE8 + 4U},.f1=0,.f2={_TmpGE9,_TmpGE9,_TmpGE9 + 1U},.f3=(void*)& Cyc_opt75_spec,.f4={_TmpGEA,_TmpGEA,_TmpGEA + 33U}};struct Cyc_List_List Cyc_opt75={(void*)& Cyc_opt75_tuple,0};
# 1207
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt76_spec={4,& Cyc_vcgen_r};static char _TmpGEB[10U]="--novcgen";static char _TmpGEC[1U]="";static char _TmpGED[39U]="do not compute verification-conditions";struct _tuple17 Cyc_opt76_tuple={.f0={_TmpGEB,_TmpGEB,_TmpGEB + 10U},.f1=0,.f2={_TmpGEC,_TmpGEC,_TmpGEC + 1U},.f3=(void*)& Cyc_opt76_spec,.f4={_TmpGED,_TmpGED,_TmpGED + 39U}};struct Cyc_List_List Cyc_opt76={(void*)& Cyc_opt76_tuple,0};
# 1210
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt77_spec={0,Cyc_Cifc_set_inst_tvar};static char _TmpGEE[17U]="--cifc-inst-tvar";static char _TmpGEF[1U]="";static char _TmpGF0[59U]="instantiate type variables instead of introducing new ones";struct _tuple17 Cyc_opt77_tuple={.f0={_TmpGEE,_TmpGEE,_TmpGEE + 17U},.f1=0,.f2={_TmpGEF,_TmpGEF,_TmpGEF + 1U},.f3=(void*)& Cyc_opt77_spec,.f4={_TmpGF0,_TmpGF0,_TmpGF0 + 59U}};struct Cyc_List_List Cyc_opt77={(void*)& Cyc_opt77_tuple,0};
# 1213
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt78_spec={0,Cyc_Flags_set_crgnonly};static char _TmpGF1[16U]="-stopafter-crgn";static char _TmpGF2[1U]="";static char _TmpGF3[19U]="Stop after currrgn";struct _tuple17 Cyc_opt78_tuple={.f0={_TmpGF1,_TmpGF1,_TmpGF1 + 16U},.f1=0,.f2={_TmpGF2,_TmpGF2,_TmpGF2 + 1U},.f3=(void*)& Cyc_opt78_spec,.f4={_TmpGF3,_TmpGF3,_TmpGF3 + 19U}};struct Cyc_List_List Cyc_opt78={(void*)& Cyc_opt78_tuple,0};
# 1216
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt79_spec={3,& Cyc_Flags_allpaths};static char _TmpGF4[11U]="--allpaths";static char _TmpGF5[1U]="";static char _TmpGF6[62U]="analyze all forward paths in verification-condition generator";struct _tuple17 Cyc_opt79_tuple={.f0={_TmpGF4,_TmpGF4,_TmpGF4 + 11U},.f1=0,.f2={_TmpGF5,_TmpGF5,_TmpGF5 + 1U},.f3=(void*)& Cyc_opt79_spec,.f4={_TmpGF6,_TmpGF6,_TmpGF6 + 62U}};struct Cyc_List_List Cyc_opt79={(void*)& Cyc_opt79_tuple,0};
# 1218
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt80_spec={0,Cyc_set_noreaps};static char _TmpGF7[9U]="-noreaps";static char _TmpGF8[1U]="";static char _TmpGF9[37U]="Generate code that doesn't use reaps";struct _tuple17 Cyc_opt80_tuple={.f0={_TmpGF7,_TmpGF7,_TmpGF7 + 9U},.f1=0,.f2={_TmpGF8,_TmpGF8,_TmpGF8 + 1U},.f3=(void*)& Cyc_opt80_spec,.f4={_TmpGF9,_TmpGF9,_TmpGF9 + 37U}};struct Cyc_List_List Cyc_opt80={(void*)& Cyc_opt80_tuple,0};
# 1222
struct Cyc_List_List*Cyc_global_options[79U]={& Cyc_opt1,& Cyc_opt2,& Cyc_opt3,& Cyc_opt4,& Cyc_opt5,& Cyc_opt6,& Cyc_opt7,& Cyc_opt8,& Cyc_opt9,& Cyc_opt10,& Cyc_opt11,& Cyc_opt12,& Cyc_opt13,& Cyc_opt14,& Cyc_opt15,& Cyc_opt16,& Cyc_opt17,& Cyc_opt18,& Cyc_opt19,& Cyc_opt20,& Cyc_opt21,& Cyc_opt22,& Cyc_opt23,& Cyc_opt24,& Cyc_opt25,& Cyc_opt26,& Cyc_opt27,& Cyc_opt28,& Cyc_opt29,& Cyc_opt30,& Cyc_opt31,& Cyc_opt32,& Cyc_opt33,& Cyc_opt34,& Cyc_opt35,& Cyc_opt36,& Cyc_opt37,& Cyc_opt38,& Cyc_opt39,& Cyc_opt41,& Cyc_opt42,& Cyc_opt43,& Cyc_opt44,& Cyc_opt45,& Cyc_opt46,& Cyc_opt47,& Cyc_opt48,& Cyc_opt49,& Cyc_opt50,& Cyc_opt51,& Cyc_opt52,& Cyc_opt53,& Cyc_opt54,& Cyc_opt55,& Cyc_opt56,& Cyc_opt57,& Cyc_opt58,& Cyc_opt59,& Cyc_opt60,& Cyc_opt61,& Cyc_opt62,& Cyc_opt63,& Cyc_opt64,& Cyc_opt65,& Cyc_opt66,& Cyc_opt67,& Cyc_opt68,& Cyc_opt69,& Cyc_opt70,& Cyc_opt71,& Cyc_opt72,& Cyc_opt73,& Cyc_opt74,& Cyc_opt75,& Cyc_opt76,& Cyc_opt77,& Cyc_opt78,& Cyc_opt79,& Cyc_opt80};
# 1237
void Cyc_print_options (void){
Cyc_Arg_usage(Cyc_global_options[0],_tag_fat("<program.cyc>",sizeof(char),14U));}
# 1242
int Cyc_main(int argc,struct _fat_ptr argv){
# 1245
GC_blacklist_warn_clear();{
# 1248
struct _fat_ptr optstring=_tag_fat("Options:",sizeof(char),9U);
# 1250
{int i=1;for(0;(unsigned)i < 79U;++ i){
({struct Cyc_List_List*_Tmp0=*((struct Cyc_List_List**)_check_known_subscript_notnull(Cyc_global_options,79U,sizeof(struct Cyc_List_List*),i));(*((struct Cyc_List_List**)_check_known_subscript_notnull(Cyc_global_options,79U,sizeof(struct Cyc_List_List*),i - 1)))->tl=_Tmp0;});}}{
struct Cyc_List_List*options=Cyc_global_options[0];
# 1254
struct _fat_ptr otherargs=Cyc_Specsfile_parse_b(options,Cyc_add_other,Cyc_add_other_flag,optstring,argv);
# 1260
struct _fat_ptr specs_file=Cyc_find_in_arch_path(_tag_fat("cycspecs",sizeof(char),9U));
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Reading from specs file %s\n",sizeof(char),28U),_tag_fat(_Tmp1,sizeof(void*),1));});{
struct Cyc_List_List*specs=Cyc_Specsfile_read_specs(specs_file);
# 1264
struct _fat_ptr args_from_specs_file=Cyc_Specsfile_split_specs(Cyc_Specsfile_get_spec(specs,_tag_fat("cyclone",sizeof(char),8U)));
if((struct _fat_ptr*)args_from_specs_file.curr!=(struct _fat_ptr*)_tag_fat(0,0,0).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,args_from_specs_file);}{
# 1269
struct _fat_ptr target_cflags=Cyc_Specsfile_get_spec(specs,_tag_fat("cyclone_target_cflags",sizeof(char),22U));
struct _fat_ptr cyclone_cc=Cyc_Specsfile_get_spec(specs,_tag_fat("cyclone_cc",sizeof(char),11U));
if(!((unsigned)cyclone_cc.curr))cyclone_cc=_tag_fat("gcc",sizeof(char),4U);
Cyc_def_inc_path=Cyc_Specsfile_get_spec(specs,_tag_fat("cyclone_inc_path",sizeof(char),17U));
# 1275
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,otherargs);
if(Cyc_strcmp(Cyc_cpp,_tag_fat("",sizeof(char),1U))==0){
# 1293 "cyclone.cyc"
const char*dash_E=Cyc_produce_dependencies?"":" -E";
Cyc_set_cpp(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)({const char*_Tmp4=dash_E;_tag_fat((void*)_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_aprintf(_tag_fat("%s %s -w -x c%s -specs %s",sizeof(char),26U),_tag_fat(_Tmp4,sizeof(void*),4));}));}
# 1297
if(Cyc_cyclone_files==0 && Cyc_ccargs==0){
Cyc_fprintf(Cyc_stderr,_tag_fat("missing file\n",sizeof(char),14U),_tag_fat(0U,sizeof(void*),0));
exit(1);}
# 1306
if((int)Cyc_Flags_stop_after_pass > 0 && !Cyc_nocyc_setjmp_r)
Cyc_cyc_setjmp=Cyc_find_in_arch_path(_tag_fat("cyc_setjmp.h",sizeof(char),13U));
if((int)Cyc_Flags_stop_after_pass > 0 && Cyc_strlen(Cyc_cyc_include)==0U)
Cyc_cyc_include=Cyc_find_in_exec_path(_tag_fat("cyc_include.h",sizeof(char),14U));
# 1312
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_List_List*l=Cyc_List_rev(Cyc_cyclone_files);for(0;l!=0;l=l->tl){
Cyc_process_file(*((struct _fat_ptr*)l->hd));
if(Cyc_compile_failure){int _Tmp2=1;_npop_handler(0);return _Tmp2;}}}
# 1313
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;_Tmp3=_Tmp2;{void*x=_Tmp3;
# 1318
if(Cyc_compile_failure)return 1;
Cyc_Core_rethrow(x);};}}}
# 1322
if((int)Cyc_Flags_stop_after_pass < 15)return 0;
# 1328
if(Cyc_ccargs==0)return 0;
# 1331
if((unsigned)Cyc_Specsfile_target_arch.curr &&(unsigned)Cyc_Specsfile_cyclone_exec_path)
Cyc_add_ccarg(({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=_tag_fat("",sizeof(char),1U);_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_Tmp1->tl=_Tmp2;});_Tmp1;});
# 1332
Cyc_str_sepstr(_Tmp0,
# 1334
_tag_fat(" -L",sizeof(char),4U));}));
Cyc_add_ccarg(Cyc_strconcat(_tag_fat("-L",sizeof(char),3U),Cyc_Specsfile_def_lib_path));
Cyc_ccargs=Cyc_List_rev(Cyc_ccargs);{
struct _fat_ptr ccargs_string=({struct Cyc_List_List*_Tmp0=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,Cyc_ccargs);Cyc_str_sepstr(_Tmp0,_tag_fat(" ",sizeof(char),2U));});
Cyc_libargs=Cyc_List_rev(Cyc_libargs);{
struct _fat_ptr libargs_string=({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=_tag_fat("",sizeof(char),1U);_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,Cyc_libargs);_Tmp1->tl=_Tmp2;});_Tmp1;});
# 1339
Cyc_str_sepstr(_Tmp0,
_tag_fat(" ",sizeof(char),2U));});
# 1342
struct Cyc_List_List*stdlib;
struct _fat_ptr stdlib_string;
int is_not_executable=
((Cyc_stop_after_asmfile_r || Cyc_stop_after_objectfile_r)||
# 1347
 Cyc_output_file!=0 && Cyc_Filename_check_suffix(*Cyc_output_file,_tag_fat(".a",sizeof(char),3U)))||
 Cyc_output_file!=0 && Cyc_Filename_check_suffix(*Cyc_output_file,_tag_fat(".lib",sizeof(char),5U));
if(is_not_executable){
stdlib=0;
stdlib_string=_tag_fat("",sizeof(char),1U);}else{
# 1354
struct _fat_ptr libcyc_flag;
struct _fat_ptr nogc_filename;
struct _fat_ptr runtime_filename;
struct _fat_ptr gc_filename;
if(Cyc_pa_r){
libcyc_flag=_tag_fat("-lcyc_a",sizeof(char),8U);
nogc_filename=_tag_fat("nogc_a.a",sizeof(char),9U);
runtime_filename=_tag_fat("runtime_cyc_a.a",sizeof(char),16U);}else{
if(Cyc_nocheck_r){
libcyc_flag=_tag_fat("-lcyc_nocheck",sizeof(char),14U);
nogc_filename=_tag_fat("nogc.a",sizeof(char),7U);
runtime_filename=_tag_fat("runtime_cyc.a",sizeof(char),14U);}else{
if(Cyc_pg_r){
libcyc_flag=_tag_fat("-lcyc_pg",sizeof(char),9U);
runtime_filename=_tag_fat("runtime_cyc_pg.a",sizeof(char),17U);
nogc_filename=_tag_fat("nogc_pg.a",sizeof(char),10U);}else{
if(Cyc_Flags_compile_for_boot){
# 1372
libcyc_flag=_tag_fat("-lcycboot",sizeof(char),10U);
nogc_filename=_tag_fat("nogc.a",sizeof(char),7U);
runtime_filename=_tag_fat("runtime_cyc.a",sizeof(char),14U);}else{
if(Cyc_pthread_r){
libcyc_flag=_tag_fat("-lcyc",sizeof(char),6U);
nogc_filename=_tag_fat("nogc_pthread.a",sizeof(char),15U);
runtime_filename=_tag_fat("runtime_cyc_pthread.a",sizeof(char),22U);}else{
# 1380
libcyc_flag=_tag_fat("-lcyc",sizeof(char),6U);
nogc_filename=_tag_fat("nogc.a",sizeof(char),7U);
runtime_filename=_tag_fat("runtime_cyc.a",sizeof(char),14U);}}}}}
# 1384
if(!Cyc_pthread_r)
gc_filename=_tag_fat("gc.a",sizeof(char),5U);else{
# 1387
gc_filename=_tag_fat("gc_pthread.a",sizeof(char),13U);}{
# 1389
struct _fat_ptr gc=Cyc_nogc_r?Cyc_find_in_arch_path(nogc_filename): Cyc_find_in_arch_path(gc_filename);
struct _fat_ptr runtime=Cyc_find_in_arch_path(runtime_filename);
# 1392
stdlib=0;
if(Cyc_nogc_r)
stdlib_string=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)libcyc_flag);_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)((struct _fat_ptr)runtime);_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)((struct _fat_ptr)gc);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_aprintf(_tag_fat(" %s %s %s ",sizeof(char),11U),_tag_fat(_Tmp3,sizeof(void*),3));});else{
# 1396
stdlib_string=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)libcyc_flag);_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)((struct _fat_ptr)runtime);_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)((struct _fat_ptr)gc);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_aprintf(_tag_fat(" %s %s %s -DCYC_GC_PTHREAD_REDIRECTS ",sizeof(char),38U),_tag_fat(_Tmp3,sizeof(void*),3));});}}}
# 1399
if(Cyc_ic_r){struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
Cyc_ccargs=({(struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_filter;})(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=({(struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_filter;})(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*lf=({struct Cyc_List_List*_Tmp2=stdlib;Cyc_List_append(_Tmp2,Cyc_List_append(Cyc_ccargs,Cyc_libargs));});
if(!is_not_executable)
lf=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=Cyc_final_strptr,_Tmp2->tl=lf;_Tmp2;});{
# 1406
struct Cyc_Interface_I*i=({(struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*)(struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_Interface_get_and_merge_list;})(Cyc_read_cycio,lf,lf);
if(i==0){
Cyc_fprintf(Cyc_stderr,_tag_fat("Error: interfaces incompatible\n",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _Tmp2=1;_npop_handler(0);return _Tmp2;}}
# 1413
if(is_not_executable){
if(Cyc_output_file!=0){
struct _fat_ptr output_file_io=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Filename_chop_extension(*_check_null(Cyc_output_file)));_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("%s.cycio",sizeof(char),9U),_tag_fat(_Tmp3,sizeof(void*),1));});
struct Cyc___cycFILE*f=Cyc_try_file_open(output_file_io,_tag_fat("wb",sizeof(char),3U),_tag_fat("interface object file",sizeof(char),22U));
if(f==0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _Tmp2=1;_npop_handler(0);return _Tmp2;}}
# 1422
Cyc_Interface_save(i,f);
Cyc_file_close(f);}}else{
# 1426
if(!({struct Cyc_Interface_I*_Tmp2=Cyc_Interface_empty();struct Cyc_Interface_I*_Tmp3=i;Cyc_Interface_is_subinterface(_Tmp2,_Tmp3,({struct _tuple12*_Tmp4=_cycalloc(sizeof(struct _tuple12));
_Tmp4->f0=_tag_fat("empty interface",sizeof(char),16U),_Tmp4->f1=_tag_fat("global interface",sizeof(char),17U);_Tmp4;}));})){
Cyc_fprintf(Cyc_stderr,_tag_fat("Error: some objects are still undefined\n",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _Tmp2=1;_npop_handler(0);return _Tmp2;}}}}}
# 1400
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;_Tmp3=_Tmp2;{void*e=_Tmp3;
# 1436
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)({const char*_Tmp7=Cyc_Core_get_exn_name(e);_tag_fat((void*)_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)({const char*_Tmp8=Cyc_Core_get_exn_filename();_tag_fat((void*)_Tmp8,sizeof(char),_get_zero_arr_size_char((void*)_Tmp8,1U));});_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,({unsigned long _Tmp8=(unsigned long)Cyc_Core_get_exn_lineno();_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu",sizeof(char),131U),_tag_fat(_Tmp7,sizeof(void*),3));});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;};}}}{
# 1442
struct _fat_ptr outfile_str=_tag_fat("",sizeof(char),1U);
if(Cyc_output_file!=0)
outfile_str=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(*Cyc_output_file));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat(" -o %s",sizeof(char),7U),_tag_fat(_Tmp1,sizeof(void*),1));});{
# 1446
struct _fat_ptr cccmd=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)((struct _fat_ptr)ccargs_string);_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)outfile_str);_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)stdlib_string);_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)libargs_string);_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_aprintf(_tag_fat("%s %s %s%s %s%s",sizeof(char),16U),_tag_fat(_Tmp6,sizeof(void*),6));});
# 1451
if(Cyc_v_r){({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)cccmd);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s\n",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});Cyc_fflush(Cyc_stderr);}
if(system((const char*)_check_null(_untag_fat_ptr(cccmd,sizeof(char),1U)))!=0){
Cyc_fprintf(Cyc_stderr,_tag_fat("Error: C compiler failed\n",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}
# 1458
Cyc_remove_cfiles();
# 1460
return Cyc_compile_failure?1: 0;}}}}}}}}}

#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file lib/runtime_*.c */
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
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Fat pointers */
struct _fat_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

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
void   _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void   _pop_dynregion();

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons *);
void _push_region(struct _RegionHandle *);
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

struct _xtunion_struct* Cyc_Core_get_exn_thrown();
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
#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#else
#define _INLINE inline
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_cks_null = (void*)(ptr); \
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
#define _zero_arr_plus_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_char_fn _zero_arr_plus_fn
#define _zero_arr_plus_short_fn _zero_arr_plus_fn
#define _zero_arr_plus_int_fn _zero_arr_plus_fn
#define _zero_arr_plus_float_fn _zero_arr_plus_fn
#define _zero_arr_plus_double_fn _zero_arr_plus_fn
#define _zero_arr_plus_longdouble_fn _zero_arr_plus_fn
#define _zero_arr_plus_voidstar_fn _zero_arr_plus_fn
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
short* _zero_arr_plus_short_fn(short*,unsigned,int,const char*,unsigned);
int* _zero_arr_plus_int_fn(int*,unsigned,int,const char*,unsigned);
float* _zero_arr_plus_float_fn(float*,unsigned,int,const char*,unsigned);
double* _zero_arr_plus_double_fn(double*,unsigned,int,const char*,unsigned);
long double* _zero_arr_plus_longdouble_fn(long double*,unsigned,int,const char*, unsigned);
void** _zero_arr_plus_voidstar_fn(void**,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
int _get_zero_arr_size_char(const char*,unsigned);
int _get_zero_arr_size_short(const short*,unsigned);
int _get_zero_arr_size_int(const int*,unsigned);
int _get_zero_arr_size_float(const float*,unsigned);
int _get_zero_arr_size_double(const double*,unsigned);
int _get_zero_arr_size_longdouble(const long double*,unsigned);
int _get_zero_arr_size_voidstar(const void**,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_short_fn(short**,int,const char*,unsigned);
int* _zero_arr_inplace_plus_int(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_longdouble_fn(long double**,int,const char*,unsigned);
void** _zero_arr_inplace_plus_voidstar_fn(void***,int,const char*,unsigned);
/* like the previous functions, but does post-addition (as in e++) */
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_post_short_fn(short**x,int,const char*,unsigned);
int* _zero_arr_inplace_plus_post_int_fn(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_post_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_post_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_post_longdouble_fn(long double**,int,const char *,unsigned);
void** _zero_arr_inplace_plus_post_voidstar_fn(void***,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_short(x,s,i) \
  (_zero_arr_plus_short_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_int(x,s,i) \
  (_zero_arr_plus_int_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_float(x,s,i) \
  (_zero_arr_plus_float_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_double(x,s,i) \
  (_zero_arr_plus_double_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_longdouble(x,s,i) \
  (_zero_arr_plus_longdouble_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_voidstar(x,s,i) \
  (_zero_arr_plus_voidstar_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

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
      _curr != (unsigned char *)0) \
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

/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);
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

static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
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

static _INLINE void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);
# 114
void*Cyc_Core_identity(void*);
# 116
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100 "cycboot.h"
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];extern char Cyc_List_Nth[4U];
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 54 "string.h"
extern int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 86
extern int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*);struct Cyc_Fn_Function{void*(*f)(void*,void*);void*env;};
# 46 "fn.h"
extern struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*)(void*,void*),void*);
# 49
extern struct Cyc_Fn_Function*Cyc_Fn_fp2fn(void*(*)(void*));
# 52
extern void*Cyc_Fn_apply(struct Cyc_Fn_Function*,void*);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc (void);
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc (void);
# 74
struct Cyc_PP_Doc*Cyc_PP_oline_doc (void);
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _fat_ptr);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _fat_ptr);
# 97
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*);
# 103
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*,struct Cyc_PP_Doc*);
# 105
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _fat_ptr,struct Cyc_List_List*);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _fat_ptr,struct Cyc_List_List*);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 40 "xarray.h"
extern int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 45
extern void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 48
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 66
extern void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 121
extern void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*);struct Cyc_PP_Empty_PP_Alist_struct{int tag;int f1;};struct Cyc_PP_Single_PP_Alist_struct{int tag;void*f1;};struct Cyc_PP_Append_PP_Alist_struct{int tag;void*f1;void*f2;};
# 51 "pp.cyc"
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_stringptr={0,0};
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_link={0,0};struct _tuple0{void*f1;void*f2;};
# 54
void*Cyc_PP_append(void*a1,void*a2){
struct _tuple0 _tmp0=({struct _tuple0 _tmp9C;_tmp9C.f1=a1,_tmp9C.f2=a2;_tmp9C;});struct _tuple0 _stmttmp0=_tmp0;struct _tuple0 _tmp1=_stmttmp0;if(*((int*)_tmp1.f1)== 0)
return a2;else{if(*((int*)_tmp1.f2)== 0)
return a1;else{
return(void*)({struct Cyc_PP_Append_PP_Alist_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=2,_tmp2->f1=a1,_tmp2->f2=a2;_tmp2;});}};}
# 62
struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct Cyc_List_List*l){
void*_tmp3=y;void*_tmp5;void*_tmp4;switch(*((int*)_tmp3)){case 0:
 return l;case 1: _tmp4=(void*)((struct Cyc_PP_Single_PP_Alist_struct*)_tmp3)->f1;{void*z=_tmp4;
return({struct Cyc_List_List*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->hd=z,_tmp6->tl=l;_tmp6;});}default: _tmp4=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp3)->f1;_tmp5=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp3)->f2;{void*a1=_tmp4;void*a2=_tmp5;
return({void*_tmp9E=a1;Cyc_PP_list_of_alist_f(_tmp9E,Cyc_PP_list_of_alist_f(a2,l));});}};}
# 69
struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){
return Cyc_PP_list_of_alist_f(x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*f;};
# 98
static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al){
struct Cyc_Xarray_Xarray*_tmp7=({(struct Cyc_Xarray_Xarray*(*)(int,void*))Cyc_Xarray_create;})(16,al);struct Cyc_Xarray_Xarray*xarr=_tmp7;
({(void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add;})(xarr,al);{
int last=0;
while(last >= 0){
void*_tmp8=({(void*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get;})(xarr,last);void*_stmttmp1=_tmp8;void*_tmp9=_stmttmp1;void*_tmpB;void*_tmpA;switch(*((int*)_tmp9)){case 0:
 -- last;goto _LL0;case 1: _tmpA=(struct _fat_ptr*)((struct Cyc_PP_Single_PP_Alist_struct*)_tmp9)->f1;{struct _fat_ptr*s=_tmpA;
-- last;({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp9D;_tmp9D.tag=0,_tmp9D.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp9D;});void*_tmpC[1];_tmpC[0]=& _tmpE;({struct Cyc___cycFILE*_tmpA0=f;struct _fat_ptr _tmp9F=({const char*_tmpD="%s";_tag_fat(_tmpD,sizeof(char),3U);});Cyc_fprintf(_tmpA0,_tmp9F,_tag_fat(_tmpC,sizeof(void*),1));});});goto _LL0;}default: _tmpA=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp9)->f1;_tmpB=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp9)->f2;{void*a1=_tmpA;void*a2=_tmpB;
# 107
({(void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set;})(xarr,last,a2);
if(({int _tmpA1=last;_tmpA1 == ({(int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length;})(xarr)- 1;}))
({(void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add;})(xarr,a1);else{
# 111
({(void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set;})(xarr,last + 1,a1);}
++ last;
goto _LL0;}}_LL0:;}
# 116
({(void(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_reuse;})(xarr);}}
# 120
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f){
struct Cyc_PP_Out*o=({struct Cyc_PP_Out*(*_tmpA3)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;});struct Cyc_Fn_Function*_tmpA2=d->f;_tmpA3(_tmpA2,({struct Cyc_PP_Ppstate*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->ci=0,_tmpF->cc=0,_tmpF->cl=1,_tmpF->pw=w,_tmpF->epw=w;_tmpF;}));});
Cyc_PP_dump_out(f,o->ppout);}
# 126
struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Out*o=({struct Cyc_PP_Out*(*_tmpA5)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;});struct Cyc_Fn_Function*_tmpA4=d->f;_tmpA5(_tmpA4,({struct Cyc_PP_Ppstate*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->ci=0,_tmp11->cc=0,_tmp11->cl=1,_tmp11->pw=w,_tmp11->epw=w;_tmp11;}));});
return(struct _fat_ptr)({struct Cyc_List_List*_tmpA6=({(struct Cyc_List_List*(*)(void*))Cyc_PP_list_of_alist;})(o->ppout);Cyc_str_sepstr(_tmpA6,({const char*_tmp10="";_tag_fat(_tmp10,sizeof(char),1U);}));});}struct _tuple1{struct _fat_ptr f1;struct Cyc_List_List*f2;};
# 133
struct _tuple1*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Out*o=({struct Cyc_PP_Out*(*_tmpA8)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;});struct Cyc_Fn_Function*_tmpA7=d->f;_tmpA8(_tmpA7,({struct Cyc_PP_Ppstate*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->ci=0,_tmp14->cc=0,_tmp14->cl=1,_tmp14->pw=w,_tmp14->epw=w;_tmp14;}));});
return({struct _tuple1*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _fat_ptr _tmpAB=(struct _fat_ptr)({
struct Cyc_List_List*_tmpAA=({(struct Cyc_List_List*(*)(void*))Cyc_PP_list_of_alist;})(o->ppout);Cyc_str_sepstr(_tmpAA,({const char*_tmp12="";_tag_fat(_tmp12,sizeof(char),1U);}));});
# 135
_tmp13->f1=_tmpAB;}),({
# 137
struct Cyc_List_List*_tmpA9=({(struct Cyc_List_List*(*)(void*))Cyc_PP_list_of_alist;})(o->links);_tmp13->f2=_tmpA9;});_tmp13;});}
# 140
static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;
# 142
int Cyc_PP_tex_output=0;
# 144
struct _fat_ptr Cyc_PP_nlblanks(int i){
if(Cyc_PP_bhashtbl == 0)
Cyc_PP_bhashtbl=({struct Cyc_Core_Opt*_tmp15=_cycalloc(sizeof(*_tmp15));({struct Cyc_Hashtable_Table*_tmpAC=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(int,int),int(*)(int)))Cyc_Hashtable_create;})(61,Cyc_Core_intcmp,({(int(*)(int))Cyc_Core_identity;}));_tmp15->v=_tmpAC;});_tmp15;});
if(i < 0)(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=Cyc_Core_Failure,({struct _fat_ptr _tmpAD=({const char*_tmp16="nlblanks";_tag_fat(_tmp16,sizeof(char),9U);});_tmp17->f1=_tmpAD;});_tmp17;}));{
struct _handler_cons _tmp18;_push_handler(& _tmp18);{int _tmp1A=0;if(setjmp(_tmp18.handler))_tmp1A=1;if(!_tmp1A){
{struct _fat_ptr _tmp1B=*({(struct _fat_ptr*(*)(struct Cyc_Hashtable_Table*,int))Cyc_Hashtable_lookup;})((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i);_npop_handler(0);return _tmp1B;};_pop_handler();}else{void*_tmp19=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C=_tmp19;void*_tmp1D;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp1C)->tag == Cyc_Core_Not_found){
# 152
if(!Cyc_PP_tex_output){
int _tmp1E=i / 8;int num_tabs=_tmp1E;
int _tmp1F=i % 8;int num_spaces=_tmp1F;
int _tmp20=(2 + num_tabs)+ num_spaces;int total=_tmp20;
struct _fat_ptr nlb=Cyc_Core_new_string((unsigned)total);
({struct _fat_ptr _tmp21=_fat_ptr_plus(nlb,sizeof(char),0);char _tmp22=*((char*)_check_fat_subscript(_tmp21,sizeof(char),0U));char _tmp23='\n';if(_get_fat_size(_tmp21,sizeof(char))== 1U &&(_tmp22 == 0 && _tmp23 != 0))_throw_arraybounds();*((char*)_tmp21.curr)=_tmp23;});
{int j=0;for(0;j < num_tabs;++ j){
({struct _fat_ptr _tmp24=_fat_ptr_plus(nlb,sizeof(char),j + 1);char _tmp25=*((char*)_check_fat_subscript(_tmp24,sizeof(char),0U));char _tmp26='\t';if(_get_fat_size(_tmp24,sizeof(char))== 1U &&(_tmp25 == 0 && _tmp26 != 0))_throw_arraybounds();*((char*)_tmp24.curr)=_tmp26;});}}
{int j=0;for(0;j < num_spaces;++ j){
({struct _fat_ptr _tmp27=_fat_ptr_plus(nlb,sizeof(char),(j + 1)+ num_tabs);char _tmp28=*((char*)_check_fat_subscript(_tmp27,sizeof(char),0U));char _tmp29=' ';if(_get_fat_size(_tmp27,sizeof(char))== 1U &&(_tmp28 == 0 && _tmp29 != 0))_throw_arraybounds();*((char*)_tmp27.curr)=_tmp29;});}}
({void(*_tmpB0)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*)=({(void(*)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmpAF=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v;int _tmpAE=i;_tmpB0(_tmpAF,_tmpAE,({unsigned _tmp2B=1;struct _fat_ptr*_tmp2A=_cycalloc(_check_times(_tmp2B,sizeof(struct _fat_ptr)));_tmp2A[0]=(struct _fat_ptr)nlb;_tmp2A;}));});
return(struct _fat_ptr)nlb;}else{
# 168
int _tmp2C=3 + i;int total=_tmp2C;
struct _fat_ptr nlb=Cyc_Core_new_string((unsigned)(total + 1));
({struct _fat_ptr _tmp2D=_fat_ptr_plus(nlb,sizeof(char),0);char _tmp2E=*((char*)_check_fat_subscript(_tmp2D,sizeof(char),0U));char _tmp2F='\\';if(_get_fat_size(_tmp2D,sizeof(char))== 1U &&(_tmp2E == 0 && _tmp2F != 0))_throw_arraybounds();*((char*)_tmp2D.curr)=_tmp2F;});
({struct _fat_ptr _tmp30=_fat_ptr_plus(nlb,sizeof(char),1);char _tmp31=*((char*)_check_fat_subscript(_tmp30,sizeof(char),0U));char _tmp32='\\';if(_get_fat_size(_tmp30,sizeof(char))== 1U &&(_tmp31 == 0 && _tmp32 != 0))_throw_arraybounds();*((char*)_tmp30.curr)=_tmp32;});
({struct _fat_ptr _tmp33=_fat_ptr_plus(nlb,sizeof(char),2);char _tmp34=*((char*)_check_fat_subscript(_tmp33,sizeof(char),0U));char _tmp35='\n';if(_get_fat_size(_tmp33,sizeof(char))== 1U &&(_tmp34 == 0 && _tmp35 != 0))_throw_arraybounds();*((char*)_tmp33.curr)=_tmp35;});
{int j=3;for(0;j < total;++ j){
({struct _fat_ptr _tmp36=_fat_ptr_plus(nlb,sizeof(char),j);char _tmp37=*((char*)_check_fat_subscript(_tmp36,sizeof(char),0U));char _tmp38='~';if(_get_fat_size(_tmp36,sizeof(char))== 1U &&(_tmp37 == 0 && _tmp38 != 0))_throw_arraybounds();*((char*)_tmp36.curr)=_tmp38;});}}
({void(*_tmpB3)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*)=({(void(*)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmpB2=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v;int _tmpB1=i;_tmpB3(_tmpB2,_tmpB1,({unsigned _tmp3A=1;struct _fat_ptr*_tmp39=_cycalloc(_check_times(_tmp3A,sizeof(struct _fat_ptr)));_tmp39[0]=(struct _fat_ptr)nlb;_tmp39;}));});
return(struct _fat_ptr)nlb;}}else{_tmp1D=_tmp1C;{void*exn=_tmp1D;(int)_rethrow(exn);}};}}}}
# 182
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;
# 184
int Cyc_PP_infinity=9999999;struct _tuple2{int f1;struct _fat_ptr f2;};
# 186
static struct Cyc_PP_Out*Cyc_PP_text_doc_f(struct _tuple2*clo,struct Cyc_PP_Ppstate*st){
struct _tuple2*_tmp3B=clo;struct _fat_ptr _tmp3D;int _tmp3C;_tmp3C=_tmp3B->f1;_tmp3D=_tmp3B->f2;{int slen=_tmp3C;struct _fat_ptr s=_tmp3D;
return({struct Cyc_PP_Out*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->newcc=st->cc + slen,_tmp41->newcl=st->cl,({
# 190
void*_tmpB5=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->tag=1,({struct _fat_ptr*_tmpB4=({unsigned _tmp3F=1;struct _fat_ptr*_tmp3E=_cycalloc(_check_times(_tmp3F,sizeof(struct _fat_ptr)));_tmp3E[0]=s;_tmp3E;});_tmp40->f1=_tmpB4;});_tmp40;});_tmp41->ppout=_tmpB5;}),_tmp41->links=(void*)& Cyc_PP_Empty_link;_tmp41;});}}
# 193
static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _fat_ptr s){
int slen=(int)Cyc_strlen((struct _fat_ptr)s);
return({struct Cyc_PP_Doc*_tmp43=_cycalloc(sizeof(*_tmp43));
_tmp43->mwo=slen,_tmp43->mw=Cyc_PP_infinity,({
# 198
struct Cyc_Fn_Function*_tmpB7=({struct Cyc_Fn_Function*(*_tmpB6)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*)=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*))Cyc_Fn_make_fn;});_tmpB6(Cyc_PP_text_doc_f,({struct _tuple2*_tmp42=_cycalloc(sizeof(*_tmp42));
# 200
_tmp42->f1=slen,_tmp42->f2=s;_tmp42;}));});
# 198
_tmp43->f=_tmpB7;});_tmp43;});}
# 202
struct Cyc_PP_Doc*Cyc_PP_text(struct _fat_ptr s){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl == 0){
t=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;})(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);
Cyc_PP_str_hashtbl=({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->v=t;_tmp44;});}else{
# 208
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl))->v;}{
# 210
struct _handler_cons _tmp45;_push_handler(& _tmp45);{int _tmp47=0;if(setjmp(_tmp45.handler))_tmp47=1;if(!_tmp47){
{struct Cyc_PP_Doc*_tmp49=({struct Cyc_PP_Doc*(*_tmpB9)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;});struct Cyc_Hashtable_Table*_tmpB8=t;_tmpB9(_tmpB8,({struct _fat_ptr*_tmp48=_cycalloc(sizeof(*_tmp48));*_tmp48=s;_tmp48;}));});_npop_handler(0);return _tmp49;};_pop_handler();}else{void*_tmp46=(void*)Cyc_Core_get_exn_thrown();void*_tmp4A=_tmp46;void*_tmp4B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp4A)->tag == Cyc_Core_Not_found){
# 214
struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);
({void(*_tmpBC)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*)=({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmpBB=t;struct _fat_ptr*_tmpBA=({struct _fat_ptr*_tmp4C=_cycalloc(sizeof(*_tmp4C));*_tmp4C=s;_tmp4C;});_tmpBC(_tmpBB,_tmpBA,d);});
return d;}else{_tmp4B=_tmp4A;{void*exn=_tmp4B;(int)_rethrow(exn);}};}}}}
# 219
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _fat_ptr*s){return Cyc_PP_text(*s);}
# 224
static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct _fat_ptr s,int slen){
return({struct Cyc_PP_Doc*_tmp4E=_cycalloc(sizeof(*_tmp4E));
_tmp4E->mwo=slen,_tmp4E->mw=Cyc_PP_infinity,({
# 228
struct Cyc_Fn_Function*_tmpBE=({struct Cyc_Fn_Function*(*_tmpBD)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*)=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*))Cyc_Fn_make_fn;});_tmpBD(Cyc_PP_text_doc_f,({struct _tuple2*_tmp4D=_cycalloc(sizeof(*_tmp4D));
# 230
_tmp4D->f1=slen,_tmp4D->f2=s;_tmp4D;}));});
# 228
_tmp4E->f=_tmpBE;});_tmp4E;});}
# 232
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _fat_ptr s,int slen){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl2 == 0){
t=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;})(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);
Cyc_PP_str_hashtbl2=({struct Cyc_Core_Opt*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->v=t;_tmp4F;});}else{
# 239
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl2))->v;}{
# 241
struct _handler_cons _tmp50;_push_handler(& _tmp50);{int _tmp52=0;if(setjmp(_tmp50.handler))_tmp52=1;if(!_tmp52){
{struct Cyc_PP_Doc*_tmp54=({struct Cyc_PP_Doc*(*_tmpC0)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;});struct Cyc_Hashtable_Table*_tmpBF=t;_tmpC0(_tmpBF,({struct _fat_ptr*_tmp53=_cycalloc(sizeof(*_tmp53));*_tmp53=s;_tmp53;}));});_npop_handler(0);return _tmp54;};_pop_handler();}else{void*_tmp51=(void*)Cyc_Core_get_exn_thrown();void*_tmp55=_tmp51;void*_tmp56;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp55)->tag == Cyc_Core_Not_found){
# 245
struct Cyc_PP_Doc*d=Cyc_PP_text_width_doc(s,slen);
({void(*_tmpC3)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*)=({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmpC2=t;struct _fat_ptr*_tmpC1=({struct _fat_ptr*_tmp57=_cycalloc(sizeof(*_tmp57));*_tmp57=s;_tmp57;});_tmpC3(_tmpC2,_tmpC1,d);});
return d;}else{_tmp56=_tmp55;{void*exn=_tmp56;(int)_rethrow(exn);}};}}}}
# 251
struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=0;
# 257
struct Cyc_PP_Doc*Cyc_PP_nil_doc (void){
if(Cyc_PP_nil_doc_opt == 0)
Cyc_PP_nil_doc_opt=({struct Cyc_Core_Opt*_tmp59=_cycalloc(sizeof(*_tmp59));({struct Cyc_PP_Doc*_tmpC4=Cyc_PP_text(({const char*_tmp58="";_tag_fat(_tmp58,sizeof(char),1U);}));_tmp59->v=_tmpC4;});_tmp59;});
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_nil_doc_opt))->v;}
# 264
struct Cyc_PP_Doc*Cyc_PP_blank_doc (void){
if(Cyc_PP_blank_doc_opt == 0)
Cyc_PP_blank_doc_opt=({struct Cyc_Core_Opt*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_PP_Doc*_tmpC5=Cyc_PP_text(({const char*_tmp5A="";_tag_fat(_tmp5A,sizeof(char),1U);}));_tmp5B->v=_tmpC5;});_tmp5B;});
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_blank_doc_opt))->v;}struct _tuple3{int f1;struct _fat_ptr f2;struct _fat_ptr f3;};struct _tuple4{int f1;int f2;int f3;struct _fat_ptr f4;};
# 270
static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple3*clo,struct Cyc_PP_Ppstate*st){
struct _tuple3*_tmp5C=clo;struct _fat_ptr _tmp5F;struct _fat_ptr _tmp5E;int _tmp5D;_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;_tmp5F=_tmp5C->f3;{int slen=_tmp5D;struct _fat_ptr shrt=_tmp5E;struct _fat_ptr full=_tmp5F;
return({struct Cyc_PP_Out*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->newcc=st->cc + slen,_tmp65->newcl=st->cl,({
# 274
void*_tmpC9=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=1,({struct _fat_ptr*_tmpC8=({unsigned _tmp61=1;struct _fat_ptr*_tmp60=_cycalloc(_check_times(_tmp61,sizeof(struct _fat_ptr)));_tmp60[0]=shrt;_tmp60;});_tmp62->f1=_tmpC8;});_tmp62;});_tmp65->ppout=_tmpC9;}),({
void*_tmpC7=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=1,({struct _tuple4*_tmpC6=({struct _tuple4*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->f1=st->cl,_tmp63->f2=st->cc,_tmp63->f3=slen,_tmp63->f4=full;_tmp63;});_tmp64->f1=_tmpC6;});_tmp64;});_tmp65->links=_tmpC7;});_tmp65;});}}
# 277
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _fat_ptr shrt,struct _fat_ptr full){
int slen=(int)Cyc_strlen((struct _fat_ptr)shrt);
return({struct Cyc_PP_Doc*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->mwo=slen,_tmp67->mw=Cyc_PP_infinity,({
# 281
struct Cyc_Fn_Function*_tmpCB=({struct Cyc_Fn_Function*(*_tmpCA)(struct Cyc_PP_Out*(*)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*)=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*))Cyc_Fn_make_fn;});_tmpCA(Cyc_PP_hyperlink_f,({struct _tuple3*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=slen,_tmp66->f2=shrt,_tmp66->f3=full;_tmp66;}));});_tmp67->f=_tmpCB;});_tmp67;});}
# 285
static struct Cyc_PP_Out*Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){
return({struct Cyc_PP_Out*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->newcc=st->ci,_tmp6B->newcl=st->cl + 1,({
# 288
void*_tmpCE=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=1,({struct _fat_ptr*_tmpCD=({unsigned _tmp69=1;struct _fat_ptr*_tmp68=_cycalloc(_check_times(_tmp69,sizeof(struct _fat_ptr)));({struct _fat_ptr _tmpCC=Cyc_PP_nlblanks(st->ci);_tmp68[0]=_tmpCC;});_tmp68;});_tmp6A->f1=_tmpCD;});_tmp6A;});_tmp6B->ppout=_tmpCE;}),_tmp6B->links=(void*)& Cyc_PP_Empty_link;_tmp6B;});}
# 291
struct Cyc_PP_Doc*Cyc_PP_line_doc (void){
if(Cyc_PP_line_doc_opt == 0)
Cyc_PP_line_doc_opt=({struct Cyc_Core_Opt*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct Cyc_PP_Doc*_tmpD0=({struct Cyc_PP_Doc*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->mwo=0,_tmp6C->mw=0,({struct Cyc_Fn_Function*_tmpCF=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn;})(Cyc_PP_line_f);_tmp6C->f=_tmpCF;});_tmp6C;});_tmp6D->v=_tmpD0;});_tmp6D;});
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple5{int f1;struct Cyc_PP_Doc*f2;};
# 297
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple5*clo,struct Cyc_PP_Ppstate*st){
struct _tuple5*_tmp6E=clo;void*_tmp70;int _tmp6F;_tmp6F=_tmp6E->f1;_tmp70=_tmp6E->f2;{int k=_tmp6F;struct Cyc_PP_Doc*d=_tmp70;
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp71=_cycalloc(sizeof(*_tmp71));
_tmp71->ci=st->ci + k,_tmp71->cc=st->cc,_tmp71->cl=st->cl,_tmp71->pw=st->pw,_tmp71->epw=st->epw;_tmp71;});
# 305
return({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st2);}}
# 307
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){
return({struct Cyc_PP_Doc*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->mwo=d->mwo,_tmp73->mw=d->mw,({
# 310
struct Cyc_Fn_Function*_tmpD2=({struct Cyc_Fn_Function*(*_tmpD1)(struct Cyc_PP_Out*(*)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*)=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*))Cyc_Fn_make_fn;});_tmpD1(Cyc_PP_nest_f,({struct _tuple5*_tmp72=_cycalloc(sizeof(*_tmp72));
# 312
_tmp72->f1=k,_tmp72->f2=d;_tmp72;}));});
# 310
_tmp73->f=_tmpD2;});_tmp73;});}
# 315
int Cyc_PP_min(int x,int y){
if(x <= y)return x;else{
return y;}}
# 320
int Cyc_PP_max(int x,int y){
if(x >= y)return x;else{
return y;}}struct _tuple6{struct Cyc_PP_Doc*f1;struct Cyc_PP_Doc*f2;};
# 325
static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct _tuple6*_tmp74=clo;void*_tmp76;void*_tmp75;_tmp75=_tmp74->f1;_tmp76=_tmp74->f2;{struct Cyc_PP_Doc*d1=_tmp75;struct Cyc_PP_Doc*d2=_tmp76;
int epw2=Cyc_PP_max(st->pw - d2->mw,st->epw - d1->mwo);
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->ci=st->ci,_tmp79->cc=st->cc,_tmp79->cl=st->cl,_tmp79->pw=st->pw,_tmp79->epw=epw2;_tmp79;});
struct Cyc_PP_Out*o1=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d1->f,st2);
struct Cyc_PP_Ppstate*st3=({struct Cyc_PP_Ppstate*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->ci=st->ci,_tmp78->cc=o1->newcc,_tmp78->cl=o1->newcl,_tmp78->pw=st->pw,_tmp78->epw=epw2;_tmp78;});
struct Cyc_PP_Out*o2=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d2->f,st3);
struct Cyc_PP_Out*o3=({struct Cyc_PP_Out*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->newcc=o2->newcc,_tmp77->newcl=o2->newcl,({
# 334
void*_tmpD4=({(void*(*)(void*,void*))Cyc_PP_append;})(o1->ppout,o2->ppout);_tmp77->ppout=_tmpD4;}),({
void*_tmpD3=({(void*(*)(void*,void*))Cyc_PP_append;})(o1->links,o2->links);_tmp77->links=_tmpD3;});_tmp77;});
return o3;}}
# 338
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_tmp7B=_cycalloc(sizeof(*_tmp7B));({int _tmpD8=Cyc_PP_min(d1->mw,d1->mwo + d2->mwo);_tmp7B->mwo=_tmpD8;}),({
int _tmpD7=Cyc_PP_min(d1->mw,d1->mwo + d2->mw);_tmp7B->mw=_tmpD7;}),({
struct Cyc_Fn_Function*_tmpD6=({struct Cyc_Fn_Function*(*_tmpD5)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*)=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*))Cyc_Fn_make_fn;});_tmpD5(Cyc_PP_concat_f,({struct _tuple6*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=d1,_tmp7A->f2=d2;_tmp7A;}));});_tmp7B->f=_tmpD6;});_tmp7B;});}
# 343
struct Cyc_PP_Doc*Cyc_PP_cat(struct _fat_ptr l){
struct Cyc_PP_Doc*_tmp7C=Cyc_PP_nil_doc();struct Cyc_PP_Doc*d=_tmp7C;
{int i=(int)(_get_fat_size(l,sizeof(struct Cyc_PP_Doc*))- 1U);for(0;i >= 0;-- i){
d=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_fat_subscript(l,sizeof(struct Cyc_PP_Doc*),i)),d);}}
# 348
return d;}
# 353
static struct Cyc_PP_Out*Cyc_PP_long_cats_f(struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
{struct Cyc_List_List*_tmp7D=ds0;struct Cyc_List_List*ds=_tmp7D;for(0;ds != 0;ds=ds->tl){
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)ds->hd;
struct Cyc_PP_Out*o=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st);
st=({struct Cyc_PP_Ppstate*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->ci=st->ci,_tmp7E->cc=o->newcc,_tmp7E->cl=o->newcl,_tmp7E->pw=st->pw,_tmp7E->epw=st->epw - d->mwo;_tmp7E;});
os=({struct Cyc_List_List*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->hd=o,_tmp7F->tl=os;_tmp7F;});}}{
# 361
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(1;os != 0;os=os->tl){
s=({(void*(*)(void*,void*))Cyc_PP_append;})(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=({(void*(*)(void*,void*))Cyc_PP_append;})(((struct Cyc_PP_Out*)os->hd)->links,links);}
# 369
return({struct Cyc_PP_Out*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->newcc=newcc,_tmp80->newcl=newcl,_tmp80->ppout=s,_tmp80->links=links;_tmp80;});}}
# 371
static struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist){
# 375
struct Cyc_List_List*orig=doclist;
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(doclist))->hd;
doclist=doclist->tl;{
int mw=d->mw;
int mwo=d->mw;
# 381
{struct Cyc_List_List*_tmp81=doclist;struct Cyc_List_List*ds=_tmp81;for(0;ds != 0;ds=ds->tl){
int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)ds->hd)->mwo);
int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)ds->hd)->mw);
mw=mw2;
mwo=mwo2;}}
# 387
return({struct Cyc_PP_Doc*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->mwo=mw,_tmp82->mw=mwo,({struct Cyc_Fn_Function*_tmpD9=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*))Cyc_Fn_make_fn;})(Cyc_PP_long_cats_f,orig);_tmp82->f=_tmpD9;});_tmp82;});}}
# 390
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist){
if(doclist == 0)return Cyc_PP_nil_doc();else{
if(doclist->tl == 0)return(struct Cyc_PP_Doc*)doclist->hd;else{
# 394
if(({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(doclist)> 30)return Cyc_PP_long_cats(doclist);else{
return({struct Cyc_PP_Doc*_tmpDA=(struct Cyc_PP_Doc*)doclist->hd;Cyc_PP_concat(_tmpDA,Cyc_PP_cats(doclist->tl));});}}}}
# 398
static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(struct _fat_ptr*docs_ptr,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
struct _fat_ptr docs=*docs_ptr;
int sz=(int)_get_fat_size(docs,sizeof(struct Cyc_PP_Doc*));
{int i=0;for(0;i < sz;++ i){
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_fat_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));
struct Cyc_PP_Out*o=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st);
st=({struct Cyc_PP_Ppstate*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->ci=st->ci,_tmp83->cc=o->newcc,_tmp83->cl=o->newcl,_tmp83->pw=st->pw,_tmp83->epw=st->epw - d->mwo;_tmp83;});
os=({struct Cyc_List_List*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->hd=o,_tmp84->tl=os;_tmp84;});}}{
# 408
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(1;os != 0;os=os->tl){
s=({(void*(*)(void*,void*))Cyc_PP_append;})(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=({(void*(*)(void*,void*))Cyc_PP_append;})(((struct Cyc_PP_Out*)os->hd)->links,links);}
# 416
return({struct Cyc_PP_Out*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->newcc=newcc,_tmp85->newcl=newcl,_tmp85->ppout=s,_tmp85->links=links;_tmp85;});}}
# 419
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _fat_ptr docs){
int sz=(int)_get_fat_size(docs,sizeof(struct Cyc_PP_Doc*));
if(sz == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=Cyc_Core_Failure,({struct _fat_ptr _tmpDB=({const char*_tmp86="cats_arr -- size zero array";_tag_fat(_tmp86,sizeof(char),28U);});_tmp87->f1=_tmpDB;});_tmp87;}));{
struct Cyc_PP_Doc*d=((struct Cyc_PP_Doc**)docs.curr)[0];
int mw=d->mw;
int mwo=d->mw;
{int i=1;for(0;i < sz;++ i){
int mw2=Cyc_PP_min(mw,mwo + (((struct Cyc_PP_Doc**)docs.curr)[i])->mwo);
int mwo2=Cyc_PP_min(mw,mwo + (((struct Cyc_PP_Doc**)docs.curr)[i])->mw);
mw=mw2;
mwo=mwo2;}}
# 432
return({struct Cyc_PP_Doc*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->mwo=mw,_tmp8A->mw=mwo,({struct Cyc_Fn_Function*_tmpDD=({struct Cyc_Fn_Function*(*_tmpDC)(struct Cyc_PP_Out*(*)(struct _fat_ptr*,struct Cyc_PP_Ppstate*),struct _fat_ptr*)=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _fat_ptr*,struct Cyc_PP_Ppstate*),struct _fat_ptr*))Cyc_Fn_make_fn;});_tmpDC(Cyc_PP_cats_arr_f,({unsigned _tmp89=1;struct _fat_ptr*_tmp88=_cycalloc(_check_times(_tmp89,sizeof(struct _fat_ptr)));_tmp88[0]=docs;_tmp88;}));});_tmp8A->f=_tmpDD;});_tmp8A;});}}
# 435
static struct Cyc_PP_Out*Cyc_PP_doc_union_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct _tuple6*_tmp8B=clo;void*_tmp8D;void*_tmp8C;_tmp8C=_tmp8B->f1;_tmp8D=_tmp8B->f2;{struct Cyc_PP_Doc*d=_tmp8C;struct Cyc_PP_Doc*d2=_tmp8D;
int dfits=st->cc + d->mwo <= st->epw || st->cc + d->mw <= st->pw;
# 439
if(dfits)return({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st);else{
return({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d2->f,st);}}}
# 442
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_tmp8F=_cycalloc(sizeof(*_tmp8F));({int _tmpE1=Cyc_PP_min(d->mwo,d2->mwo);_tmp8F->mwo=_tmpE1;}),({
int _tmpE0=Cyc_PP_min(d->mw,d2->mw);_tmp8F->mw=_tmpE0;}),({
struct Cyc_Fn_Function*_tmpDF=({struct Cyc_Fn_Function*(*_tmpDE)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*)=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*))Cyc_Fn_make_fn;});_tmpDE(Cyc_PP_doc_union_f,({struct _tuple6*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->f1=d,_tmp8E->f2=d2;_tmp8E;}));});_tmp8F->f=_tmpDF;});_tmp8F;});}
# 449
struct Cyc_PP_Doc*Cyc_PP_oline_doc (void){
return({struct Cyc_PP_Doc*_tmpE2=Cyc_PP_nil_doc();Cyc_PP_doc_union(_tmpE2,Cyc_PP_line_doc());});}
# 453
static struct Cyc_PP_Out*Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->ci=st->cc,_tmp90->cc=st->cc,_tmp90->cl=st->cl,_tmp90->pw=st->pw,_tmp90->epw=st->epw;_tmp90;});
return({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st2);}
# 457
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){
struct Cyc_PP_Doc*d2=({struct Cyc_PP_Doc*_tmp91=_cycalloc(sizeof(*_tmp91));
_tmp91->mwo=d->mwo,_tmp91->mw=d->mw,({
# 461
struct Cyc_Fn_Function*_tmpE3=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*))Cyc_Fn_make_fn;})(Cyc_PP_tab_f,d);_tmp91->f=_tmpE3;});_tmp91;});
return d2;}
# 467
static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
return({struct Cyc_PP_Doc*_tmp92[4];({struct Cyc_PP_Doc*_tmpE7=pp(l->hd);_tmp92[0]=_tmpE7;}),({struct Cyc_PP_Doc*_tmpE6=Cyc_PP_text(sep);_tmp92[1]=_tmpE6;}),({struct Cyc_PP_Doc*_tmpE5=Cyc_PP_oline_doc();_tmp92[2]=_tmpE5;}),({struct Cyc_PP_Doc*_tmpE4=Cyc_PP_ppseq_f(pp,sep,l->tl);_tmp92[3]=_tmpE4;});Cyc_PP_cat(_tag_fat(_tmp92,sizeof(struct Cyc_PP_Doc*),4));});}}}
# 473
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseq_f(pp,sep,l));}
# 477
struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _fat_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
# 481
struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);
struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*_tmp93=l;struct Cyc_List_List*x=_tmp93;
while(((struct Cyc_List_List*)_check_null(x))->tl != 0){
struct Cyc_List_List*_tmp94=x->tl;struct Cyc_List_List*temp=_tmp94;
({struct Cyc_List_List*_tmpE9=({struct Cyc_List_List*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->hd=sep2,({struct Cyc_List_List*_tmpE8=({struct Cyc_List_List*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->hd=oline,_tmp95->tl=temp;_tmp95;});_tmp96->tl=_tmpE8;});_tmp96;});x->tl=_tmpE9;});
x=temp;}
# 489
return Cyc_PP_cats(l);}}}
# 497
struct Cyc_PP_Doc*Cyc_PP_seq(struct _fat_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}
# 502
struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
return({struct Cyc_PP_Doc*_tmp97[4];({struct Cyc_PP_Doc*_tmpED=pp(l->hd);_tmp97[0]=_tmpED;}),({struct Cyc_PP_Doc*_tmpEC=Cyc_PP_text(sep);_tmp97[1]=_tmpEC;}),({struct Cyc_PP_Doc*_tmpEB=Cyc_PP_line_doc();_tmp97[2]=_tmpEB;}),({struct Cyc_PP_Doc*_tmpEA=Cyc_PP_ppseql_f(pp,sep,l->tl);_tmp97[3]=_tmpEA;});Cyc_PP_cat(_tag_fat(_tmp97,sizeof(struct Cyc_PP_Doc*),4));});}}}
# 507
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseql_f(pp,sep,l));}
# 511
static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _fat_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
return({struct Cyc_PP_Doc*_tmp98[4];_tmp98[0]=(struct Cyc_PP_Doc*)l->hd,({struct Cyc_PP_Doc*_tmpF0=Cyc_PP_text(sep);_tmp98[1]=_tmpF0;}),({struct Cyc_PP_Doc*_tmpEF=Cyc_PP_line_doc();_tmp98[2]=_tmpEF;}),({struct Cyc_PP_Doc*_tmpEE=Cyc_PP_seql_f(sep,l->tl);_tmp98[3]=_tmpEE;});Cyc_PP_cat(_tag_fat(_tmp98,sizeof(struct Cyc_PP_Doc*),4));});}}}
# 516
struct Cyc_PP_Doc*Cyc_PP_seql(struct _fat_ptr sep,struct Cyc_List_List*l0){
return Cyc_PP_tab(Cyc_PP_seql_f(sep,l0));}
# 521
struct Cyc_PP_Doc*Cyc_PP_group(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmp99[3];({struct Cyc_PP_Doc*_tmpF3=Cyc_PP_text(start);_tmp99[0]=_tmpF3;}),({
struct Cyc_PP_Doc*_tmpF2=Cyc_PP_seq(sep,ss);_tmp99[1]=_tmpF2;}),({
struct Cyc_PP_Doc*_tmpF1=Cyc_PP_text(stop);_tmp99[2]=_tmpF1;});Cyc_PP_cat(_tag_fat(_tmp99,sizeof(struct Cyc_PP_Doc*),3));});}
# 528
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*ss){
if(ss == 0)return Cyc_PP_nil_doc();else{
return({struct Cyc_PP_Doc*_tmp9A[3];({struct Cyc_PP_Doc*_tmpF6=Cyc_PP_text(start);_tmp9A[0]=_tmpF6;}),({
struct Cyc_PP_Doc*_tmpF5=Cyc_PP_seq(sep,ss);_tmp9A[1]=_tmpF5;}),({
struct Cyc_PP_Doc*_tmpF4=Cyc_PP_text(stop);_tmp9A[2]=_tmpF4;});Cyc_PP_cat(_tag_fat(_tmp9A,sizeof(struct Cyc_PP_Doc*),3));});}}
# 535
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmp9B[3];({struct Cyc_PP_Doc*_tmpF9=Cyc_PP_text(start);_tmp9B[0]=_tmpF9;}),({
struct Cyc_PP_Doc*_tmpF8=Cyc_PP_seql(sep,ss);_tmp9B[1]=_tmpF8;}),({
struct Cyc_PP_Doc*_tmpF7=Cyc_PP_text(stop);_tmp9B[2]=_tmpF7;});Cyc_PP_cat(_tag_fat(_tmp9B,sizeof(struct Cyc_PP_Doc*),3));});}

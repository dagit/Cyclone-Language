#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator  The corresponding definitions are in file lib/runtime_*.c */
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
void cyc_vfree(void *);
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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;
# 178
void Cyc_Core_ufree(void*);
# 191
struct _fat_ptr Cyc_Core_alias_refptr(struct _fat_ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 255
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*,const char*,int);
# 273 "core.h"
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 57
extern struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 251
extern int Cyc_List_forall(int(*)(void*),struct Cyc_List_List*);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*);
# 328
extern void*Cyc_List_assoc(struct Cyc_List_List*,void*);
# 371
extern struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 67
extern struct _fat_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 69
extern struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*,struct Cyc_Set_Set*,void*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 113
extern void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict,int(*)(void*,void*),void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 40 "xarray.h"
extern int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
extern int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
extern void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 176
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 181
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 304
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 504
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 669 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 843
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 850
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 857
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 860
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 864
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 866
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 870
void*Cyc_Absyn_compress(void*);
# 888
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 890
extern void*Cyc_Absyn_sint_type;
# 895
extern void*Cyc_Absyn_heap_rgn_type;
# 899
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 901
extern void*Cyc_Absyn_void_type;
# 918
void*Cyc_Absyn_exn_type (void);
# 926
extern void*Cyc_Absyn_fat_bound_type;
# 930
void*Cyc_Absyn_bounds_one (void);
# 938
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 942
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 944
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 946
void*Cyc_Absyn_strct(struct _fat_ptr*);
void*Cyc_Absyn_strctq(struct _tuple1*);
void*Cyc_Absyn_unionq_type(struct _tuple1*);
# 952
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 957
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 970
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 972
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 978
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 983
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 986
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1042
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1047
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1055
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1059
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1064
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1068
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1073
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
# 1108
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1113
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1123
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1125
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1127
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1136
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1142
int Cyc_Absyn_var_may_appear_exp(struct _tuple1*,struct Cyc_Absyn_Exp*);
# 46 "warn.h"
void*Cyc_Warn_impos_loc(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
void*Cyc_Warn_vimpos2(struct _fat_ptr);
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 30 "flags.h"
extern int Cyc_Flags_no_regions;
# 66
extern int Cyc_Flags_noexpand_r;
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 36 "tcutil.h"
int Cyc_Tcutil_is_void_type(void*);
# 41
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 50
int Cyc_Tcutil_is_fat_ptr(void*);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
# 55
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 76
int Cyc_Tcutil_is_fat_pointer_type_elt(void*,void**);
# 78
int Cyc_Tcutil_is_zero_pointer_type_elt(void*,void**);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 88
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 166
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 173
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**,int*,void**);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 39
extern struct Cyc_Absyn_Kind Cyc_Kinds_tbk;
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 84 "attributes.h"
struct Cyc_List_List*Cyc_Atts_atts2c(struct Cyc_List_List*);struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*);struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 35 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
# 37
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 41
struct _tuple1*Cyc_Toc_temp_var (void);
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_NewInfo[8U];struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;void*f2;};
# 47
void Cyc_Toc_init (void);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};char Cyc_Toc_NewInfo[8U]="NewInfo";
# 68 "toc.cyc"
static void*Cyc_Toc_unimp(struct _fat_ptr str){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=str;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_impos2(_tag_fat(_Tmp1,sizeof(void*),1));});}
# 71
static void*Cyc_Toc_toc_impos(struct _fat_ptr ap){
void*toc=(void*)({struct Cyc_Warn_String_Warn_Warg_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Warn_String_Warn_Warg_struct));_Tmp0->tag=0,_Tmp0->f1=({const char*_Tmp1="Toc: ";_tag_fat(_Tmp1,sizeof(char),6U);});_Tmp0;});
struct _fat_ptr args=({unsigned _Tmp0=_get_fat_size(ap,sizeof(void*))+ 1U;_tag_fat(({void**_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(void*)));({{unsigned _Tmp2=_get_fat_size(ap,sizeof(void*))+ 1U;unsigned i;for(i=0;i < _Tmp2;++ i){i == 0U?_Tmp1[i]=toc:(_Tmp1[i]=((void**)ap.curr)[(int)(i - 1U)]);}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});
Cyc_Warn_vimpos2(args);}
# 77
struct _fat_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 80
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 90
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 92
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return({(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})(*_check_null(Cyc_Toc_fn_pop_table),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 117
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
int i=Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp((*x).f2,(*y).f2);}
# 124
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_Tmp0=_region_malloc(d,sizeof(struct Cyc_Toc_TocState));
({struct Cyc_List_List**_Tmp1=({struct Cyc_List_List**_Tmp2=_region_malloc(d,sizeof(struct Cyc_List_List*));*_Tmp2=0;_Tmp2;});_Tmp0->tuple_types=_Tmp1;}),({
struct Cyc_List_List**_Tmp1=({struct Cyc_List_List**_Tmp2=_region_malloc(d,sizeof(struct Cyc_List_List*));*_Tmp2=0;_Tmp2;});_Tmp0->anon_aggr_types=_Tmp1;}),({
struct Cyc_Dict_Dict*_Tmp1=({struct Cyc_Dict_Dict*_Tmp2=_region_malloc(d,sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp3=({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty;})(d,Cyc_Absyn_qvar_cmp);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->aggrs_so_far=_Tmp1;}),({
struct Cyc_List_List**_Tmp1=({struct Cyc_List_List**_Tmp2=_region_malloc(d,sizeof(struct Cyc_List_List*));*_Tmp2=0;_Tmp2;});_Tmp0->abs_struct_types=_Tmp1;}),({
struct Cyc_Set_Set**_Tmp1=({struct Cyc_Set_Set**_Tmp2=_region_malloc(d,sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp3=({(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty;})(d,Cyc_Absyn_qvar_cmp);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->datatypes_so_far=_Tmp1;}),({
struct Cyc_Dict_Dict*_Tmp1=({struct Cyc_Dict_Dict*_Tmp2=_region_malloc(d,sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp3=({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty;})(d,Cyc_Absyn_qvar_cmp);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->xdatatypes_so_far=_Tmp1;}),({
struct Cyc_Dict_Dict*_Tmp1=({struct Cyc_Dict_Dict*_Tmp2=_region_malloc(d,sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp3=({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty;})(d,Cyc_Toc_qvar_tag_cmp);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->qvar_tags=_Tmp1;}),({
struct Cyc_Xarray_Xarray*_Tmp1=({(struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty;})(d);_Tmp0->temp_labels=_Tmp1;});_Tmp0;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 143
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 149
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 152
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_Tmp0=ts;struct Cyc_Toc_TocStateWrap*_Tmp1=Cyc_Toc_toc_state;ts=_Tmp1;Cyc_Toc_toc_state=_Tmp0;});{
struct Cyc_Toc_TocStateWrap _stmttmp0=*_check_null(ts);struct Cyc_Toc_TocStateWrap _Tmp0=_stmttmp0;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.dyn;_Tmp1=_Tmp0.state;{struct Cyc_Core_DynamicRegion*dyn=_Tmp2;struct Cyc_Toc_TocState*s=_Tmp1;
struct _fat_ptr dyn2=({(struct _fat_ptr(*)(struct _fat_ptr))Cyc_Core_alias_refptr;})(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _Tmp3=({struct Cyc_Toc_TocStateWrap _Tmp4;_Tmp4.dyn=dyn,_Tmp4.state=s;_Tmp4;});*ts=_Tmp3;});
({struct Cyc_Toc_TocStateWrap*_Tmp3=ts;struct Cyc_Toc_TocStateWrap*_Tmp4=Cyc_Toc_toc_state;ts=_Tmp4;Cyc_Toc_toc_state=_Tmp3;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 env=({struct _tuple15 _Tmp3;_Tmp3.f1=s,_Tmp3.f2=arg;_Tmp3;});
res=f(h,& env);}
# 160
;}
# 162
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 166
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 169
struct _tuple17 _stmttmp1=*env;struct _tuple17 _Tmp0=_stmttmp1;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f1;_Tmp2=(_Tmp0.f2)->f1;_Tmp1=(_Tmp0.f2)->f2;{struct Cyc_Toc_TocState*s=_Tmp3;struct _tuple1*q=_Tmp2;void*(*type_maker)(struct _tuple1*)=_Tmp1;
struct _tuple18**v=({(struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;})(*s->aggrs_so_far,q);
return v == 0?type_maker(q):(*(*v)).f2;}}
# 174
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _Tmp0;_Tmp0.f1=q,_Tmp0.f2=type_maker;_Tmp0;});
return({(void*(*)(struct _tuple16*,void*(*)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state;})(& env,Cyc_Toc_aggrdecl_type_body);}static char _TmpG0[5U]="curr";
# 187 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_TmpG0,_TmpG0,_TmpG0 + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _TmpG1[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_TmpG1,_TmpG1,_TmpG1 + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _TmpG2[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_TmpG2,_TmpG2,_TmpG2 + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _TmpG3[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_TmpG3,_TmpG3,_TmpG3 + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _TmpG4[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_TmpG4,_TmpG4,_TmpG4 + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _TmpG5[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_TmpG5,_TmpG5,_TmpG5 + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _TmpG6[7U]="_throw";
# 217 "toc.cyc"
static struct _fat_ptr Cyc_Toc__throw_str={_TmpG6,_TmpG6,_TmpG6 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _TmpG7[7U]="setjmp";
static struct _fat_ptr Cyc_Toc_setjmp_str={_TmpG7,_TmpG7,_TmpG7 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _TmpG8[14U]="_push_handler";
static struct _fat_ptr Cyc_Toc__push_handler_str={_TmpG8,_TmpG8,_TmpG8 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _TmpG9[13U]="_pop_handler";
static struct _fat_ptr Cyc_Toc__pop_handler_str={_TmpG9,_TmpG9,_TmpG9 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _TmpGA[12U]="_exn_thrown";
static struct _fat_ptr Cyc_Toc__exn_thrown_str={_TmpGA,_TmpGA,_TmpGA + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _TmpGB[14U]="_npop_handler";
static struct _fat_ptr Cyc_Toc__npop_handler_str={_TmpGB,_TmpGB,_TmpGB + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _TmpGC[12U]="_check_null";
static struct _fat_ptr Cyc_Toc__check_null_str={_TmpGC,_TmpGC,_TmpGC + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _TmpGD[28U]="_check_known_subscript_null";
static struct _fat_ptr Cyc_Toc__check_known_subscript_null_str={_TmpGD,_TmpGD,_TmpGD + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _TmpGE[31U]="_check_known_subscript_notnull";
static struct _fat_ptr Cyc_Toc__check_known_subscript_notnull_str={_TmpGE,_TmpGE,_TmpGE + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _TmpGF[21U]="_check_fat_subscript";
static struct _fat_ptr Cyc_Toc__check_fat_subscript_str={_TmpGF,_TmpGF,_TmpGF + 21U};static struct _tuple1 Cyc_Toc__check_fat_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_fat_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_subscript_bnd={0,& Cyc_Toc__check_fat_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_subscript_re={1,(void*)& Cyc_Toc__check_fat_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_subscript_ev={0,(void*)& Cyc_Toc__check_fat_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_subscript_e=& Cyc_Toc__check_fat_subscript_ev;static char _TmpG10[9U]="_fat_ptr";
static struct _fat_ptr Cyc_Toc__fat_ptr_str={_TmpG10,_TmpG10,_TmpG10 + 9U};static struct _tuple1 Cyc_Toc__fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_bnd={0,& Cyc_Toc__fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_re={1,(void*)& Cyc_Toc__fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_ev={0,(void*)& Cyc_Toc__fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_e=& Cyc_Toc__fat_ptr_ev;static char _TmpG11[9U]="_tag_fat";
static struct _fat_ptr Cyc_Toc__tag_fat_str={_TmpG11,_TmpG11,_TmpG11 + 9U};static struct _tuple1 Cyc_Toc__tag_fat_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_fat_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_fat_bnd={0,& Cyc_Toc__tag_fat_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_fat_re={1,(void*)& Cyc_Toc__tag_fat_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_fat_ev={0,(void*)& Cyc_Toc__tag_fat_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_fat_e=& Cyc_Toc__tag_fat_ev;static char _TmpG12[15U]="_untag_fat_ptr";
static struct _fat_ptr Cyc_Toc__untag_fat_ptr_str={_TmpG12,_TmpG12,_TmpG12 + 15U};static struct _tuple1 Cyc_Toc__untag_fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_fat_ptr_bnd={0,& Cyc_Toc__untag_fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_fat_ptr_re={1,(void*)& Cyc_Toc__untag_fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_fat_ptr_ev={0,(void*)& Cyc_Toc__untag_fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_fat_ptr_e=& Cyc_Toc__untag_fat_ptr_ev;static char _TmpG13[14U]="_get_fat_size";
static struct _fat_ptr Cyc_Toc__get_fat_size_str={_TmpG13,_TmpG13,_TmpG13 + 14U};static struct _tuple1 Cyc_Toc__get_fat_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_fat_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_fat_size_bnd={0,& Cyc_Toc__get_fat_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_fat_size_re={1,(void*)& Cyc_Toc__get_fat_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_fat_size_ev={0,(void*)& Cyc_Toc__get_fat_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_fat_size_e=& Cyc_Toc__get_fat_size_ev;static char _TmpG14[23U]="_fat_ptr_decrease_size";
static struct _fat_ptr Cyc_Toc__fat_ptr_decrease_size_str={_TmpG14,_TmpG14,_TmpG14 + 23U};static struct _tuple1 Cyc_Toc__fat_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_decrease_size_bnd={0,& Cyc_Toc__fat_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_decrease_size_re={1,(void*)& Cyc_Toc__fat_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__fat_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_decrease_size_e=& Cyc_Toc__fat_ptr_decrease_size_ev;static char _TmpG15[19U]="_get_zero_arr_size";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_str={_TmpG15,_TmpG15,_TmpG15 + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _TmpG16[24U]="_get_zero_arr_size_char";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_char_str={_TmpG16,_TmpG16,_TmpG16 + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _TmpG17[25U]="_get_zero_arr_size_short";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_short_str={_TmpG17,_TmpG17,_TmpG17 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _TmpG18[23U]="_get_zero_arr_size_int";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_int_str={_TmpG18,_TmpG18,_TmpG18 + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _TmpG19[25U]="_get_zero_arr_size_float";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_float_str={_TmpG19,_TmpG19,_TmpG19 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _TmpG1A[26U]="_get_zero_arr_size_double";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_double_str={_TmpG1A,_TmpG1A,_TmpG1A + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _TmpG1B[30U]="_get_zero_arr_size_longdouble";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_TmpG1B,_TmpG1B,_TmpG1B + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _TmpG1C[28U]="_get_zero_arr_size_voidstar";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_TmpG1C,_TmpG1C,_TmpG1C + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _TmpG1D[14U]="_fat_ptr_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_plus_str={_TmpG1D,_TmpG1D,_TmpG1D + 14U};static struct _tuple1 Cyc_Toc__fat_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_plus_bnd={0,& Cyc_Toc__fat_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_plus_re={1,(void*)& Cyc_Toc__fat_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_plus_e=& Cyc_Toc__fat_ptr_plus_ev;static char _TmpG1E[15U]="_zero_arr_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_str={_TmpG1E,_TmpG1E,_TmpG1E + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _TmpG1F[20U]="_zero_arr_plus_char";static struct _fat_ptr Cyc_Toc__zero_arr_plus_char_str={_TmpG1F,_TmpG1F,_TmpG1F + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _TmpG20[21U]="_zero_arr_plus_short";static struct _fat_ptr Cyc_Toc__zero_arr_plus_short_str={_TmpG20,_TmpG20,_TmpG20 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _TmpG21[19U]="_zero_arr_plus_int";static struct _fat_ptr Cyc_Toc__zero_arr_plus_int_str={_TmpG21,_TmpG21,_TmpG21 + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _TmpG22[21U]="_zero_arr_plus_float";static struct _fat_ptr Cyc_Toc__zero_arr_plus_float_str={_TmpG22,_TmpG22,_TmpG22 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _TmpG23[22U]="_zero_arr_plus_double";static struct _fat_ptr Cyc_Toc__zero_arr_plus_double_str={_TmpG23,_TmpG23,_TmpG23 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _TmpG24[26U]="_zero_arr_plus_longdouble";static struct _fat_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_TmpG24,_TmpG24,_TmpG24 + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _TmpG25[24U]="_zero_arr_plus_voidstar";static struct _fat_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_TmpG25,_TmpG25,_TmpG25 + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _TmpG26[22U]="_fat_ptr_inplace_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_str={_TmpG26,_TmpG26,_TmpG26 + 22U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_re={1,(void*)& Cyc_Toc__fat_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_e=& Cyc_Toc__fat_ptr_inplace_plus_ev;static char _TmpG27[23U]="_zero_arr_inplace_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_str={_TmpG27,_TmpG27,_TmpG27 + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _TmpG28[28U]="_zero_arr_inplace_plus_char";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_TmpG28,_TmpG28,_TmpG28 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _TmpG29[29U]="_zero_arr_inplace_plus_short";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_TmpG29,_TmpG29,_TmpG29 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _TmpG2A[27U]="_zero_arr_inplace_plus_int";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_TmpG2A,_TmpG2A,_TmpG2A + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _TmpG2B[29U]="_zero_arr_inplace_plus_float";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_TmpG2B,_TmpG2B,_TmpG2B + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _TmpG2C[30U]="_zero_arr_inplace_plus_double";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_TmpG2C,_TmpG2C,_TmpG2C + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _TmpG2D[34U]="_zero_arr_inplace_plus_longdouble";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_TmpG2D,_TmpG2D,_TmpG2D + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _TmpG2E[32U]="_zero_arr_inplace_plus_voidstar";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_TmpG2E,_TmpG2E,_TmpG2E + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _TmpG2F[27U]="_fat_ptr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_post_str={_TmpG2F,_TmpG2F,_TmpG2F + 27U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_post_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_post_re={1,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_post_e=& Cyc_Toc__fat_ptr_inplace_plus_post_ev;static char _TmpG30[28U]="_zero_arr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_TmpG30,_TmpG30,_TmpG30 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _TmpG31[33U]="_zero_arr_inplace_plus_post_char";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_TmpG31,_TmpG31,_TmpG31 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _TmpG32[34U]="_zero_arr_inplace_plus_post_short";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_TmpG32,_TmpG32,_TmpG32 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _TmpG33[32U]="_zero_arr_inplace_plus_post_int";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_TmpG33,_TmpG33,_TmpG33 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _TmpG34[34U]="_zero_arr_inplace_plus_post_float";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_TmpG34,_TmpG34,_TmpG34 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _TmpG35[35U]="_zero_arr_inplace_plus_post_double";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_TmpG35,_TmpG35,_TmpG35 + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _TmpG36[39U]="_zero_arr_inplace_plus_post_longdouble";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_TmpG36,_TmpG36,_TmpG36 + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _TmpG37[37U]="_zero_arr_inplace_plus_post_voidstar";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_TmpG37,_TmpG37,_TmpG37 + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _TmpG38[10U]="_cycalloc";
static struct _fat_ptr Cyc_Toc__cycalloc_str={_TmpG38,_TmpG38,_TmpG38 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _TmpG39[11U]="_cyccalloc";
static struct _fat_ptr Cyc_Toc__cyccalloc_str={_TmpG39,_TmpG39,_TmpG39 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _TmpG3A[17U]="_cycalloc_atomic";
static struct _fat_ptr Cyc_Toc__cycalloc_atomic_str={_TmpG3A,_TmpG3A,_TmpG3A + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _TmpG3B[18U]="_cyccalloc_atomic";
static struct _fat_ptr Cyc_Toc__cyccalloc_atomic_str={_TmpG3B,_TmpG3B,_TmpG3B + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _TmpG3C[15U]="_region_malloc";
static struct _fat_ptr Cyc_Toc__region_malloc_str={_TmpG3C,_TmpG3C,_TmpG3C + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _TmpG3D[15U]="_region_calloc";
static struct _fat_ptr Cyc_Toc__region_calloc_str={_TmpG3D,_TmpG3D,_TmpG3D + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _TmpG3E[16U]="_region_vmalloc";
static struct _fat_ptr Cyc_Toc__region_vmalloc_str={_TmpG3E,_TmpG3E,_TmpG3E + 16U};static struct _tuple1 Cyc_Toc__region_vmalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_vmalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_vmalloc_bnd={0,& Cyc_Toc__region_vmalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_vmalloc_re={1,(void*)& Cyc_Toc__region_vmalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_vmalloc_ev={0,(void*)& Cyc_Toc__region_vmalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_vmalloc_e=& Cyc_Toc__region_vmalloc_ev;static char _TmpG3F[20U]="_fast_region_malloc";
static struct _fat_ptr Cyc_Toc__fast_region_malloc_str={_TmpG3F,_TmpG3F,_TmpG3F + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;static char _TmpG40[13U]="_check_times";
static struct _fat_ptr Cyc_Toc__check_times_str={_TmpG40,_TmpG40,_TmpG40 + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _TmpG41[12U]="_new_region";
static struct _fat_ptr Cyc_Toc__new_region_str={_TmpG41,_TmpG41,_TmpG41 + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _TmpG42[13U]="_push_region";
static struct _fat_ptr Cyc_Toc__push_region_str={_TmpG42,_TmpG42,_TmpG42 + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _TmpG43[12U]="_pop_region";
static struct _fat_ptr Cyc_Toc__pop_region_str={_TmpG43,_TmpG43,_TmpG43 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _TmpG44[19U]="_throw_arraybounds";
static struct _fat_ptr Cyc_Toc__throw_arraybounds_str={_TmpG44,_TmpG44,_TmpG44 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _TmpG45[13U]="_throw_match";
static struct _fat_ptr Cyc_Toc__throw_match_str={_TmpG45,_TmpG45,_TmpG45 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _TmpG46[9U]="_rethrow";
static struct _fat_ptr Cyc_Toc__rethrow_str={_TmpG46,_TmpG46,_TmpG46 + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;
# 258
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression (void){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 261
if((unsigned)_get_exn_thrown_e)
return _check_null(_get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_Tmp0=_cycalloc(sizeof(struct _tuple1));((_Tmp0->f1).Abs_n).tag=2U,({struct Cyc_List_List*_Tmp1=({struct _fat_ptr*_Tmp2[1];({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));*_Tmp4=({const char*_Tmp5="Core";_tag_fat(_Tmp5,sizeof(char),5U);});_Tmp4;});_Tmp2[0]=_Tmp3;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp2,sizeof(struct _fat_ptr*),1));});((_Tmp0->f1).Abs_n).val=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=({const char*_Tmp3="get_exn_thrown";_tag_fat(_Tmp3,sizeof(char),15U);});_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct));_Tmp0->tag=0,_Tmp0->f1=qv;_Tmp0;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Exp));_Tmp0->topt=0,({void*_Tmp1=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct));_Tmp2->tag=1,_Tmp2->f1=bnd;_Tmp2;});_Tmp0->r=_Tmp1;}),_Tmp0->loc=0U,_Tmp0->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_Tmp0;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_Tmp0->tag=10,_Tmp0->f1=fnname,_Tmp0->f2=0,_Tmp0->f3=0,_Tmp0->f4=0;_Tmp0;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Exp));_Tmp0->topt=0,_Tmp0->r=fncall_re,_Tmp0->loc=0U,_Tmp0->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_Tmp0;});
return _check_null(_get_exn_thrown_e);}}
# 271
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 273
void*Cyc_Toc_void_star_type (void){
static void*t=0;
if(t == 0)
t=({void*_Tmp0=Cyc_Absyn_void_type;void*_Tmp1=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(0U);void*_Tmp3=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,Cyc_Absyn_false_type);});
return _check_null(t);}
# 279
static void*Cyc_Toc_fat_ptr_type (void){
static void*t=0;
if(t == 0)
t=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(0U,& Cyc_Toc__fat_ptr_pr,0),0);
return _check_null(t);}
# 285
static void*Cyc_Toc_rgn_type (void){
static void*r=0;
if(r == 0)
r=({void*_Tmp0=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_Tmp0,Cyc_Toc_mt_tq);});
return _check_null(r);}
# 292
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl (void){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 296
static struct _tuple19*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_Tmp0=_cycalloc(sizeof(struct _tuple19));({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp4->tag=1,_Tmp4->f1=name;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;});_Tmp0->f1=_Tmp1;}),_Tmp0->f2=e;_Tmp0;});}
# 300
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_Tmp0=f;Cyc_Absyn_fncall_exp(_Tmp0,({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_from_array;})(args),0U);});}
# 303
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_stmttmp2=e->r;void*_Tmp0=_stmttmp2;void*_Tmp1;if(*((int*)_Tmp0)== 14){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4 == Cyc_Absyn_No_coercion){_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp1;
return Cyc_Toc_cast_it(t,e);}}else{goto _LL3;}}else{_LL3:
 return Cyc_Absyn_cast_exp(t,e,0,1U,0U);};}
# 312
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct));_Tmp0->tag=14,_Tmp0->f1=t,_Tmp0->f2=e,_Tmp0->f3=0,_Tmp0->f4=1U;_Tmp0;});}
# 315
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct));_Tmp0->tag=20,_Tmp0->f1=e;_Tmp0;});}
# 318
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct));_Tmp0->tag=23,_Tmp0->f1=e1,_Tmp0->f2=e2;_Tmp0;});}
# 321
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct));_Tmp0->tag=37,_Tmp0->f1=s;_Tmp0;});}
# 324
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct));_Tmp0->tag=17,_Tmp0->f1=t;_Tmp0;});}
# 327
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_Tmp0->tag=10,_Tmp0->f1=e,({struct Cyc_List_List*_Tmp1=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_from_array;})(es);_Tmp0->f2=_Tmp1;}),_Tmp0->f3=0,_Tmp0->f4=1;_Tmp0;});}
# 330
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_Tmp0->tag=2,_Tmp0->f1=s1,_Tmp0->f2=s2;_Tmp0;});}
# 333
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct));_Tmp0->tag=6,_Tmp0->f1=e1,_Tmp0->f2=e2,_Tmp0->f3=e3;_Tmp0;});}
# 336
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct));_Tmp0->tag=21,_Tmp0->f1=e,_Tmp0->f2=n,_Tmp0->f3=0,_Tmp0->f4=0;_Tmp0;});}
# 339
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 342
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp0->tag=36,_Tmp0->f1=tdopt,_Tmp0->f2=ds;_Tmp0;});}
# 344
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct));_Tmp0->tag=8,_Tmp0->f1=v;_Tmp0;});}
# 348
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 352
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_stmttmp3=e->r;void*_Tmp0=_stmttmp3;void*_Tmp1;if(*((int*)_Tmp0)== 20){_Tmp1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{
return Cyc_Absyn_aggrmember_exp(e,f,loc);};}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 380 "toc.cyc"
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 386
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
void*_stmttmp4=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp4;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f2){case Cyc_Absyn_Char_sz:
 return fS->fchar;case Cyc_Absyn_Short_sz:
 return fS->fshort;case Cyc_Absyn_Int_sz:
 return fS->fint;default: goto _LLF;}case 2: switch((int)((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1){case 0:
 return fS->ffloat;case 1:
 return fS->fdouble;default:
 return fS->flongdouble;}default: goto _LLF;}case 3:
 return fS->fvoidstar;default: _LLF:
# 396
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="expression type ";_tag_fat(_Tmp3,sizeof(char),17U);});_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5=" (not int, float, double, or pointer)";_tag_fat(_Tmp5,sizeof(char),38U);});_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp4,sizeof(void*),3));});};}
# 399
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_stmttmp5=Cyc_Absyn_compress(_check_null(arr->topt));void*_Tmp0=_stmttmp5;void*_Tmp1;if(*((int*)_Tmp0)== 3){_Tmp1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;{void*et=_Tmp1;
return Cyc_Toc_getFunctionType(fS,et);}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="impossible type (not pointer)";_tag_fat(_Tmp4,sizeof(char),30U);});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp3,sizeof(void*),1));});};}
# 409
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_stmttmp6=e->r;void*_Tmp0=_stmttmp6;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Null_c).tag == 1)
return 1;else{goto _LLF;}case 14: _Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Toc_is_zero(e1);}case 24: _Tmp1=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp1;
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall;})(Cyc_Toc_is_zero,es);}case 26: _Tmp1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles=_Tmp1;
_Tmp1=dles;goto _LLA;}case 29: _Tmp1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LLA: {struct Cyc_List_List*dles=_Tmp1;
_Tmp1=dles;goto _LLC;}case 25: _Tmp1=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLC: {struct Cyc_List_List*dles=_Tmp1;
_Tmp1=dles;goto _LLE;}case 36: _Tmp1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLE: {struct Cyc_List_List*dles=_Tmp1;
# 418
for(1;dles != 0;dles=dles->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)dles->hd)).f2))return 0;}
return 1;}default: _LLF:
 return Cyc_Tcutil_is_zero(e);};}
# 426
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
struct _tuple1*_Tmp0=x;void*_Tmp1;union Cyc_Absyn_Nmspace _Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{union Cyc_Absyn_Nmspace ns=_Tmp2;struct _fat_ptr*v=_Tmp1;
union Cyc_Absyn_Nmspace _Tmp3=ns;void*_Tmp4;switch((_Tmp3.Abs_n).tag){case 4:
 _Tmp4=0;goto _LL7;case 1: _Tmp4=(_Tmp3.Rel_n).val;_LL7: {struct Cyc_List_List*vs=_Tmp4;
_Tmp4=vs;goto _LL9;}case 2: _Tmp4=(_Tmp3.Abs_n).val;_LL9: {struct Cyc_List_List*vs=_Tmp4;
_Tmp4=vs;goto _LLB;}default: _Tmp4=(_Tmp3.C_n).val;_LLB: {struct Cyc_List_List*vs=_Tmp4;
# 435
if(vs == 0)
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*s;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=*v;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="%s_%s_struct";_tag_fat(_Tmp8,sizeof(char),13U);}),_tag_fat(_Tmp7,sizeof(void*),2));});{
struct _RegionHandle _Tmp5=_new_region("r");struct _RegionHandle*r=& _Tmp5;_push_region(r);
{struct _fat_ptr _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=*s;_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_rstr_sepstr(r,vs,({const char*_TmpB="_";_tag_fat(_TmpB,sizeof(char),2U);}));_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=*v;_TmpA;});void*_TmpA[3];_TmpA[0]=& _Tmp7,_TmpA[1]=& _Tmp8,_TmpA[2]=& _Tmp9;Cyc_aprintf(({const char*_TmpB="%s_%s_%s_struct";_tag_fat(_TmpB,sizeof(char),16U);}),_tag_fat(_TmpA,sizeof(void*),3));});_npop_handler(0);return _Tmp6;};_pop_region();}}};}}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 448
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 451
struct _tuple20 _stmttmp7=*env;struct _tuple20 _Tmp0=_stmttmp7;void*_Tmp1;void*_Tmp2;_Tmp2=(_Tmp0.f1)->qvar_tags;_Tmp1=_Tmp0.f2;{struct Cyc_Dict_Dict*qvs=_Tmp2;struct _tuple14*pair=_Tmp1;
struct _tuple14 _stmttmp8=*pair;struct _tuple14 _Tmp3=_stmttmp8;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{struct _tuple1*fieldname=_Tmp5;struct _tuple1*dtname=_Tmp4;
struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){{struct _tuple1*_Tmp8=({(struct _tuple1*(*)(struct Cyc_Dict_Dict,int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*))Cyc_Dict_lookup_other;})(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0);return _Tmp8;};_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9=_Tmp8;void*_TmpA;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp9)->tag == Cyc_Dict_Absent){
# 455
struct _tuple14*new_pair=({struct _tuple14*_TmpB=_cycalloc(sizeof(struct _tuple14));_TmpB->f1=fieldname,_TmpB->f2=dtname;_TmpB;});
struct _tuple1*_TmpB=fieldname;void*_TmpC;union Cyc_Absyn_Nmspace _TmpD;_TmpD=_TmpB->f1;_TmpC=_TmpB->f2;{union Cyc_Absyn_Nmspace nmspace=_TmpD;struct _fat_ptr*fieldvar=_TmpC;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res=({struct _tuple1*_TmpE=_cycalloc(sizeof(struct _tuple1));_TmpE->f1=nmspace,({struct _fat_ptr*_TmpF=({struct _fat_ptr*_Tmp10=_cycalloc(sizeof(struct _fat_ptr));*_Tmp10=newvar;_Tmp10;});_TmpE->f2=_TmpF;});_TmpE;});
({struct Cyc_Dict_Dict _TmpE=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple14*,struct _tuple1*))Cyc_Dict_insert;})(*qvs,new_pair,res);*qvs=_TmpE;});
return res;}}else{_TmpA=_Tmp9;{void*exn=_TmpA;(int)_rethrow(exn);}};}}}}}
# 463
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _Tmp0;_Tmp0.f1=fieldname,_Tmp0.f2=dtname;_Tmp0;});
return({(struct _tuple1*(*)(struct _tuple14*,struct _tuple1*(*)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state;})(& env,Cyc_Toc_collapse_qvars_body);}
# 470
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_Tmp0->kind=0U,_Tmp0->sc=2U,_Tmp0->tvs=0,_Tmp0->attributes=0,_Tmp0->expected_mem_kind=0,({
# 473
struct _tuple1*_Tmp1=({struct _tuple1*_Tmp2=_cycalloc(sizeof(struct _tuple1));({union Cyc_Absyn_Nmspace _Tmp3=Cyc_Absyn_Rel_n(0);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=name;_Tmp2;});_Tmp0->name=_Tmp1;}),({
struct Cyc_Absyn_AggrdeclImpl*_Tmp1=({struct Cyc_Absyn_AggrdeclImpl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AggrdeclImpl));_Tmp2->exist_vars=0,_Tmp2->rgn_po=0,_Tmp2->fields=fs,_Tmp2->tagged=0;_Tmp2;});_Tmp0->impl=_Tmp1;});_Tmp0;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 479
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 482
struct _tuple21 _stmttmp9=*env;struct _tuple21 _Tmp0=_stmttmp9;void*_Tmp1;void*_Tmp2;_Tmp2=(_Tmp0.f1)->tuple_types;_Tmp1=_Tmp0.f2;{struct Cyc_List_List**tuple_types=_Tmp2;struct Cyc_List_List*tqs0=_Tmp1;
# 484
{struct Cyc_List_List*tts=*tuple_types;for(0;tts != 0;tts=tts->tl){
struct _tuple22*_stmttmpA=(struct _tuple22*)tts->hd;struct _tuple22*_Tmp3=_stmttmpA;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3->f1;_Tmp4=_Tmp3->f2;{void*x=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
struct Cyc_List_List*tqs=tqs0;
for(1;tqs != 0 && ts != 0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple23*)tqs->hd)).f2,(void*)ts->hd))
break;}
if(tqs == 0 && ts == 0)
return x;}}}{
# 495
struct _fat_ptr*xname=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="_tuple%d";_tag_fat(_Tmp7,sizeof(char),9U);}),_tag_fat(_Tmp6,sizeof(void*),1));});*_Tmp3=_Tmp4;});_Tmp3;});
struct Cyc_List_List*fs=0;
struct Cyc_List_List*ts=0;
{int i=1;for(0;tqs0 != 0;(tqs0=tqs0->tl,i ++)){
void*t=(*((struct _tuple23*)tqs0->hd)).f2;
fs=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_Tmp4=({struct Cyc_Absyn_Aggrfield*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));({struct _fat_ptr*_Tmp6=Cyc_Absyn_fieldname(i);_Tmp5->name=_Tmp6;}),_Tmp5->tq=Cyc_Toc_mt_tq,_Tmp5->type=t,_Tmp5->width=0,_Tmp5->attributes=0,_Tmp5->requires_clause=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=fs;_Tmp3;});
ts=({struct Cyc_List_List*_Tmp3=_region_malloc(d,sizeof(struct Cyc_List_List));_Tmp3->hd=t,_Tmp3->tl=ts;_Tmp3;});}}
# 503
fs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fs);
ts=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(ts);{
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Toc_make_c_struct_defn(xname,fs);
void*ans=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp3=sd;_Tmp3;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp5->tag=5,_Tmp5->f1=sd;_Tmp5;}),0U);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=Cyc_Toc_result_decls;_Tmp3;});
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(d,sizeof(struct Cyc_List_List));({struct _tuple22*_Tmp5=({struct _tuple22*_Tmp6=_region_malloc(d,sizeof(struct _tuple22));_Tmp6->f1=ans,_Tmp6->f2=ts;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=*tuple_types;_Tmp4;});*tuple_types=_Tmp3;});
return ans;}}}}
# 511
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return({(void*(*)(struct Cyc_List_List*,void*(*)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state;})(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 516
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 519
struct _tuple25*_Tmp0=env;void*_Tmp1;enum Cyc_Absyn_AggrKind _Tmp2;void*_Tmp3;_Tmp3=(_Tmp0->f1)->anon_aggr_types;_Tmp2=(_Tmp0->f2)->f1;_Tmp1=(_Tmp0->f2)->f2;{struct Cyc_List_List**anon_aggr_types=_Tmp3;enum Cyc_Absyn_AggrKind ak=_Tmp2;struct Cyc_List_List*fs=_Tmp1;
# 521
{struct Cyc_List_List*ts=*anon_aggr_types;for(0;ts != 0;ts=ts->tl){
struct _tuple26*_stmttmpB=(struct _tuple26*)ts->hd;struct _tuple26*_Tmp4=_stmttmpB;void*_Tmp5;enum Cyc_Absyn_AggrKind _Tmp6;void*_Tmp7;_Tmp7=_Tmp4->f1;_Tmp6=_Tmp4->f2;_Tmp5=_Tmp4->f3;{void*x=_Tmp7;enum Cyc_Absyn_AggrKind ak2=_Tmp6;struct Cyc_List_List*fs2=_Tmp5;
if((int)ak == (int)ak2 && !({(int(*)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 528
struct _fat_ptr*xname=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="_tuple%d";_tag_fat(_Tmp8,sizeof(char),9U);}),_tag_fat(_Tmp7,sizeof(void*),1));});*_Tmp4=_Tmp5;});_Tmp4;});
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Toc_make_c_struct_defn(xname,fs);
sd->kind=ak;{
void*ans=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp4=sd;_Tmp4;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp6->tag=5,_Tmp6->f1=sd;_Tmp6;}),0U);_Tmp4->hd=_Tmp5;}),_Tmp4->tl=Cyc_Toc_result_decls;_Tmp4;});
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_region_malloc(d,sizeof(struct Cyc_List_List));({struct _tuple26*_Tmp6=({struct _tuple26*_Tmp7=_region_malloc(d,sizeof(struct _tuple26));_Tmp7->f1=ans,_Tmp7->f2=ak,_Tmp7->f3=fs;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=*anon_aggr_types;_Tmp5;});*anon_aggr_types=_Tmp4;});
return ans;}}}}
# 536
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return({void*(*_Tmp0)(struct _tuple24*,void*(*)(struct _RegionHandle*,struct _tuple25*))=({(void*(*)(struct _tuple24*,void*(*)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state;});_Tmp0(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));_Tmp1->f1=ak,_Tmp1->f2=fs;_Tmp1;}),Cyc_Toc_add_anon_aggr_type_body);});}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 545
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 554
struct _tuple28 _stmttmpC=*env;struct _tuple28 _Tmp0=_stmttmpC;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=(_Tmp0.f1)->abs_struct_types;_Tmp4=(_Tmp0.f2)->f1;_Tmp3=(_Tmp0.f2)->f2;_Tmp2=(_Tmp0.f2)->f3;_Tmp1=(_Tmp0.f2)->f4;{struct Cyc_List_List**abs_struct_types=_Tmp5;struct _tuple1*struct_name=_Tmp4;struct Cyc_List_List*type_vars=_Tmp3;struct Cyc_List_List*type_args=_Tmp2;struct Cyc_List_List*fields=_Tmp1;
# 558
{struct Cyc_List_List*tts=*abs_struct_types;for(0;tts != 0;tts=tts->tl){
struct _tuple29*_stmttmpD=(struct _tuple29*)tts->hd;struct _tuple29*_Tmp6=_stmttmpD;void*_Tmp7;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp6->f1;_Tmp8=_Tmp6->f2;_Tmp7=_Tmp6->f3;{struct _tuple1*x=_Tmp9;struct Cyc_List_List*ts2=_Tmp8;void*t=_Tmp7;
if(Cyc_Absyn_qvar_cmp(x,struct_name)== 0 &&({
int _TmpA=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(type_args);_TmpA == ({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ts2);})){
int okay=1;
{struct Cyc_List_List*ts=type_args;for(0;ts != 0;(ts=ts->tl,ts2=ts2->tl)){
void*t=(void*)ts->hd;
void*t2=(void*)(_check_null(ts2))->hd;
{struct Cyc_Absyn_Kind*_stmttmpE=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_TmpA=_stmttmpE;switch((int)((struct Cyc_Absyn_Kind*)_TmpA)->kind){case Cyc_Absyn_EffKind:
 goto _LLA;case Cyc_Absyn_RgnKind: _LLA:
 continue;default:
# 571
 if(Cyc_Unify_unify(t,t2)||({void*_TmpB=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_TmpB,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 576
break;}}
# 578
if(okay)
return t;}}}}{
# 584
struct _fat_ptr*xname=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="_tuple%d";_tag_fat(_TmpA,sizeof(char),9U);}),_tag_fat(_Tmp9,sizeof(void*),1));});*_Tmp6=_Tmp7;});_Tmp6;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*fs=0;
# 588
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_region_malloc(d,sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp8=({struct _tuple29*_Tmp9=_region_malloc(d,sizeof(struct _tuple29));_Tmp9->f1=struct_name,_Tmp9->f2=type_args,_Tmp9->f3=x;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=*abs_struct_types;_Tmp7;});*abs_struct_types=_Tmp6;});{
# 591
struct _RegionHandle _Tmp6=_new_region("r");struct _RegionHandle*r=& _Tmp6;_push_region(r);
{struct Cyc_List_List*inst=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip;})(r,r,type_vars,type_args);
for(1;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 601
if((fields->tl == 0 &&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp8=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_Tmp9->tag=6,_Tmp9->f1=0;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=atts;_Tmp7;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 608
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_Tmp7->tag=4,({void*_Tmp8=Cyc_Toc_void_star_type();(_Tmp7->f1).elt_type=_Tmp8;}),(_Tmp7->f1).tq=Cyc_Toc_mt_tq,({
struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_uint_exp(0U,0U);(_Tmp7->f1).num_elts=_Tmp8;}),(_Tmp7->f1).zero_term=Cyc_Absyn_false_type,(_Tmp7->f1).zt_loc=0U;_Tmp7;});
fs=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_Tmp8=({struct Cyc_Absyn_Aggrfield*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp9->name=f->name,_Tmp9->tq=Cyc_Toc_mt_tq,_Tmp9->type=t,_Tmp9->width=f->width,_Tmp9->attributes=atts,_Tmp9->requires_clause=0;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=fs;_Tmp7;});}
# 613
fs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fs);{
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Toc_make_c_struct_defn(xname,fs);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp9->tag=5,_Tmp9->f1=sd;_Tmp9;}),0U);_Tmp7->hd=_Tmp8;}),_Tmp7->tl=Cyc_Toc_result_decls;_Tmp7;});{
void*_Tmp7=x;_npop_handler(0);return _Tmp7;}}}
# 592
;_pop_region();}}}}
# 619
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 623
struct _tuple27 env=({struct _tuple27 _Tmp0;_Tmp0.f1=struct_name,_Tmp0.f2=type_vars,_Tmp0.f3=type_args,_Tmp0.f4=fields;_Tmp0;});
return({(void*(*)(struct _tuple27*,void*(*)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state;})(& env,Cyc_Toc_add_struct_type_body);}
# 631
struct _tuple1*Cyc_Toc_temp_var (void){
return({struct _tuple1*_Tmp0=_cycalloc(sizeof(struct _tuple1));_Tmp0->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="_tmp%X";_tag_fat(_Tmp6,sizeof(char),7U);}),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});}struct _tuple30{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 634
struct _tuple30 Cyc_Toc_temp_var_and_exp (void){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple30 _Tmp0;_Tmp0.f1=v,({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_var_exp(v,0U);_Tmp0.f2=_Tmp1;});_Tmp0;});}struct _tuple31{struct Cyc_Toc_TocState*f1;int f2;};
# 641
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _stmttmpF=*env;struct _tuple31 _Tmp0=_stmttmpF;void*_Tmp1;_Tmp1=(_Tmp0.f1)->temp_labels;{struct Cyc_Xarray_Xarray*temp_labels=_Tmp1;
int i=Cyc_Toc_fresh_label_counter ++;
if(({int _Tmp2=i;_Tmp2 < ({(int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length;})(temp_labels);}))
return({(struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get;})(temp_labels,i);{
struct _fat_ptr*res=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="_LL%X";_tag_fat(_Tmp6,sizeof(char),6U);}),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});
if(({int _Tmp2=({(int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind;})(temp_labels,res);_Tmp2 != i;}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="fresh_label: add_ind returned bad index...";_tag_fat(_Tmp4,sizeof(char),43U);});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp3,sizeof(void*),1));});
return res;}}}
# 651
static struct _fat_ptr*Cyc_Toc_fresh_label (void){
return({(struct _fat_ptr*(*)(int,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state;})(0,Cyc_Toc_fresh_label_body);}
# 657
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
{struct Cyc_List_List*fs=(struct Cyc_List_List*)(_check_null(td->fields))->v;for(0;Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)(_check_null(fs))->hd)->name)!= 0;fs=fs->tl){
++ ans;}}
return Cyc_Absyn_signed_int_exp(ans,0U);}
# 667
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*);
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*);
# 674
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
return({struct _tuple9*_Tmp0=_cycalloc(sizeof(struct _tuple9));_Tmp0->f1=0,_Tmp0->f2=(*a).f2,({void*_Tmp1=Cyc_Toc_typ_to_c((*a).f3);_Tmp0->f3=_Tmp1;});_Tmp0;});}
# 693 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_stmttmp10=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp10;struct Cyc_Absyn_ArrayInfo _Tmp1;if(*((int*)_Tmp0)== 4){_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_ArrayInfo ai=_Tmp1;
return({void*_Tmp2=Cyc_Toc_typ_to_c_array(ai.elt_type);Cyc_Absyn_cstar_type(_Tmp2,ai.tq);});}}else{
return Cyc_Toc_typ_to_c(t);};}
# 700
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 702
struct Cyc_Absyn_Aggrfield*ans=({struct Cyc_Absyn_Aggrfield*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));*_Tmp0=*f;_Tmp0;});
ans->type=new_type;
ans->requires_clause=0;
ans->tq=Cyc_Toc_mt_tq;
return ans;}
# 709
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}
# 713
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_stmttmp11=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp11;void*_Tmp1;if(*((int*)_Tmp0)== 2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
return Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tbk);}}else{
return 0;};}
# 719
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_stmttmp12=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_Tmp0=_stmttmp12;if(((struct Cyc_Absyn_Kind*)_Tmp0)->kind == Cyc_Absyn_AnyKind)
return 1;else{
return 0;};}
# 725
static int Cyc_Toc_is_void_star(void*t){
void*_stmttmp13=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp13;void*_Tmp1;if(*((int*)_Tmp0)== 3){_Tmp1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;{void*t2=_Tmp1;
return Cyc_Tcutil_is_void_type(t2);}}else{
return 0;};}
# 731
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 734
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 738
void*Cyc_Toc_typ_to_c(void*t){
void*_Tmp0=t;enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;void*_Tmp3;int _Tmp4;unsigned _Tmp5;struct Cyc_Absyn_Tqual _Tmp6;union Cyc_Absyn_AggrInfo _Tmp7;void*_Tmp8;void*_Tmp9;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 0:
 return t;case 18:
# 749
 return Cyc_Absyn_void_type;case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1).KnownDatatypefield).tag == 2){_Tmp9=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1).KnownDatatypefield).val).f1;_Tmp8=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp9;struct Cyc_Absyn_Datatypefield*tuf=_Tmp8;
# 751
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{
# 753
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC="unresolved DatatypeFieldType";_tag_fat(_TmpC,sizeof(char),29U);});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpB,sizeof(void*),1));});}case 1:
# 761
 goto _LL12;case 2: _LL12:
 return t;case 20: _Tmp9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_Tmp8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp9;union Cyc_Absyn_AggrInfo info=_Tmp7;struct Cyc_List_List*ts=_Tmp8;
# 794
{union Cyc_Absyn_AggrInfo _TmpA=info;if((_TmpA.UnknownAggr).tag == 1)
# 796
return ts == 0?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_TmpB->tag=0,_TmpB->f1=c,_TmpB->f2=0;_TmpB;});else{
goto _LL35;}_LL35:;}{
# 799
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
# 801
if(ad->expected_mem_kind && ad->impl == 0)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpB;_TmpB.tag=6,_TmpB.f1=ad;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD=" was never defined.";_tag_fat(_TmpD,sizeof(char),20U);});_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_warn2(0U,_tag_fat(_TmpC,sizeof(void*),2));});
# 804
if((int)ad->kind == 1)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*fs=ad->impl == 0?0:(_check_null(ad->impl))->fields;
if(fs == 0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(1;(_check_null(fs))->tl != 0;fs=fs->tl){;}{
void*last_type=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpB;_TmpB.tag=6,_TmpB.f1=ad;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD=" ended up being abstract.";_tag_fat(_TmpD,sizeof(char),26U);});_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_warn2(0U,_tag_fat(_TmpC,sizeof(void*),2));});{
# 815
struct _RegionHandle _TmpA=_new_region("r");struct _RegionHandle*r=& _TmpA;_push_region(r);
{struct Cyc_List_List*inst=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip;})(r,r,ad->tvs,ts);
void*t=Cyc_Tcutil_rsubstitute(r,inst,last_type);
if(Cyc_Toc_is_abstract_type(t)){void*_TmpB=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0);return _TmpB;}{
void*_TmpB=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,(_check_null(ad->impl))->fields);_npop_handler(0);return _TmpB;}}
# 816
;_pop_region();}}
# 821
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 15: _Tmp9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{struct _tuple1*tdn=_Tmp9;
return t;}case 16: _Tmp9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{struct Cyc_List_List*fs=_Tmp9;
Cyc_Toc_enumfields_to_c(fs);return t;}case 4:
# 830
 return Cyc_Absyn_uint_type;case 3:
 return Cyc_Toc_rgn_type();case 17:
 return t;default:
# 835
 return Cyc_Toc_void_star_type();}case 1: _Tmp9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;{void**t2=_Tmp9;
# 742
if(*t2 != 0)
return Cyc_Toc_typ_to_c(_check_null(*t2));
return*t2=Cyc_Absyn_sint_type;}case 2: _Tmp9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp9;
# 746
if((int)(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk))->kind == 0)
return Cyc_Absyn_void_type;
return Cyc_Toc_void_star_type();}case 3: _Tmp9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_tq;_Tmp8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).bounds;{void*t2=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp6;void*bnds=_Tmp8;
# 757
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)== 0)
return Cyc_Toc_fat_ptr_type();
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}case 4: _Tmp9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).tq;_Tmp8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).num_elts;_Tmp5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zt_loc;{void*t2=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp8;unsigned ztl=_Tmp5;
# 764
return({void*_TmpA=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _TmpB=tq;struct Cyc_Absyn_Exp*_TmpC=e;void*_TmpD=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_TmpA,_TmpB,_TmpC,_TmpD,ztl);});}case 5: _Tmp6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ret_tqual;_Tmp9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ret_type;_Tmp8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).args;_Tmp4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).c_varargs;_Tmp3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).cyc_varargs;_Tmp2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).attributes;{struct Cyc_Absyn_Tqual tq2=_Tmp6;void*t2=_Tmp9;struct Cyc_List_List*args=_Tmp8;int c_vararg=_Tmp4;struct Cyc_Absyn_VarargInfo*cyc_vararg=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
# 768
struct Cyc_List_List*new_atts=Cyc_Atts_atts2c(atts);
struct Cyc_List_List*new_args=({(struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Toc_arg_to_c,args);
if(cyc_vararg != 0){
# 772
void*t=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));
struct _tuple9*vararg=({struct _tuple9*_TmpA=_cycalloc(sizeof(struct _tuple9));_TmpA->f1=cyc_vararg->name,_TmpA->f2=cyc_vararg->tq,_TmpA->f3=t;_TmpA;});
new_args=({struct Cyc_List_List*(*_TmpA)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_TmpB=new_args;_TmpA(_TmpB,({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));_TmpC->hd=vararg,_TmpC->tl=0;_TmpC;}));});}
# 776
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_TmpA->tag=5,(_TmpA->f1).tvars=0,(_TmpA->f1).effect=0,(_TmpA->f1).ret_tqual=tq2,({void*_TmpB=Cyc_Toc_typ_to_c(t2);(_TmpA->f1).ret_type=_TmpB;}),(_TmpA->f1).args=new_args,(_TmpA->f1).c_varargs=c_vararg,(_TmpA->f1).cyc_varargs=0,(_TmpA->f1).rgn_po=0,(_TmpA->f1).attributes=new_atts,(_TmpA->f1).requires_clause=0,(_TmpA->f1).requires_relns=0,(_TmpA->f1).ensures_clause=0,(_TmpA->f1).ensures_relns=0,(_TmpA->f1).return_value=0;_TmpA;});}case 6: _Tmp9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tqs=_Tmp9;
# 781
struct Cyc_List_List*tqs2=0;
for(1;tqs != 0;tqs=tqs->tl){
tqs2=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple23*_TmpB=({struct _tuple23*_TmpC=_cycalloc(sizeof(struct _tuple23));_TmpC->f1=(*((struct _tuple23*)tqs->hd)).f1,({void*_TmpD=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqs->hd)).f2);_TmpC->f2=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=tqs2;_TmpA;});}
return Cyc_Toc_add_tuple_type(({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(tqs2));}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp9;
# 788
struct Cyc_List_List*fs2=0;
for(1;fs != 0;fs=fs->tl){
fs2=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_TmpB=({struct Cyc_Absyn_Aggrfield*_TmpC=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_TmpC,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_TmpA->hd=_TmpB;}),_TmpA->tl=fs2;_TmpA;});}
return({enum Cyc_Absyn_AggrKind _TmpA=k;Cyc_Toc_add_anon_aggr_type(_TmpA,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fs2));});}case 8: _Tmp9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;_Tmp2=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f4;{struct _tuple1*tdn=_Tmp9;struct Cyc_List_List*ts=_Tmp8;struct Cyc_Absyn_Typedefdecl*td=_Tmp3;void*topt=_Tmp2;
# 827
if(topt == 0)
return ts == 0?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_Absyn_Type_struct));_TmpA->tag=8,_TmpA->f1=tdn,_TmpA->f2=0,_TmpA->f3=td,_TmpA->f4=0;_TmpA;});
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_Absyn_Type_struct));_TmpA->tag=8,_TmpA->f1=tdn,_TmpA->f2=0,_TmpA->f3=td,({void*_TmpB=Cyc_Toc_typ_to_c(topt);_TmpA->f4=_TmpB;});_TmpA;});}case 9: _Tmp9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 840
return t;}case 11: _Tmp9=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 845
Cyc_Toc_exptypes_to_c(e);
return e->topt == 0?t: Cyc_Toc_typ_to_c(_check_null(e->topt));}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)){case 0: _Tmp9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp9;
# 848
Cyc_Toc_aggrdecl_to_c(ad);
return Cyc_Toc_aggrdecl_type(ad->name,(int)ad->kind == 1?Cyc_Absyn_unionq_type: Cyc_Absyn_strctq);}case 1: _Tmp9=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp9;
# 851
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _Tmp9=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;_Tmp8=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp9;void**t=_Tmp8;
# 854
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*_check_null(t));}}};}
# 859
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_Tmp0=t;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;if(*((int*)_Tmp0)== 4){_Tmp2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).tq;{void*t2=_Tmp2;struct Cyc_Absyn_Tqual tq=_Tmp1;
# 862
return({void*_Tmp3=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_Tmp3,e);});}}else{
return Cyc_Toc_cast_it(t,e);};}
# 869
static int Cyc_Toc_atomic_type(void*t){
void*_stmttmp14=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp14;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 2:
 return 0;case 0: _Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp2;struct Cyc_List_List*ts=_Tmp1;
# 873
void*_Tmp3=c;void*_Tmp4;void*_Tmp5;union Cyc_Absyn_AggrInfo _Tmp6;switch(*((int*)_Tmp3)){case 0:
 goto _LL15;case 1: _LL15: goto _LL17;case 2: _LL17: goto _LL19;case 4: _LL19:
 goto _LL1B;case 15: _LL1B: goto _LL1D;case 16: _LL1D: return 1;case 18:
 goto _LL21;case 3: _LL21:
 goto _LL23;case 17: _LL23:
 return 0;case 20: _Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_Tmp3)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp6;
# 885
{union Cyc_Absyn_AggrInfo _Tmp7=info;if((_Tmp7.UnknownAggr).tag == 1)
return 0;else{
goto _LL2A;}_LL2A:;}{
# 889
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl == 0)
return 0;
{struct Cyc_List_List*fs=(_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_Tmp3)->f1).KnownDatatypefield).tag == 2){_Tmp5=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_Tmp3)->f1).KnownDatatypefield).val).f1;_Tmp4=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_Tmp3)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp5;struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;
# 896
{struct Cyc_List_List*tqs=tuf->typs;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}}
return 1;}}else{goto _LL28;}default: _LL28:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="atomic_typ: bad type ";_tag_fat(_Tmp9,sizeof(char),22U);});_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)t;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp9,sizeof(void*),2));});};}case 5:
# 901
 return 1;case 4: _Tmp2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;{void*t=_Tmp2;
return Cyc_Toc_atomic_type(t);}case 7: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*fs=_Tmp2;
# 904
for(1;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 6: _Tmp2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tqs=_Tmp2;
# 908
for(1;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}
return 1;}case 3:
 return 0;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="atomic_typ:  bad type ";_tag_fat(_Tmp5,sizeof(char),23U);});_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp5,sizeof(void*),2));});};}
# 917
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_stmttmp15=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp15;void*_Tmp1;union Cyc_Absyn_AggrInfo _Tmp2;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 20){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp2;
# 920
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="is_poly_field: type missing fields";_tag_fat(_Tmp5,sizeof(char),35U);});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp4,sizeof(void*),1));});
_Tmp1=(_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;_LL4: {struct Cyc_List_List*fs=_Tmp1;
# 925
struct Cyc_Absyn_Aggrfield*field=Cyc_Absyn_lookup_field(fs,f);
if(field == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="is_poly_field: bad field ";_tag_fat(_Tmp5,sizeof(char),26U);});_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*f;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp5,sizeof(void*),2));});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="is_poly_field: bad type ";_tag_fat(_Tmp5,sizeof(char),25U);});_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp5,sizeof(void*),2));});};}
# 936
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_stmttmp16=e->r;void*_Tmp0=_stmttmp16;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 21: _Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 939
return Cyc_Toc_is_poly_field(_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(_check_null(e->topt));}case 22: _Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 942
void*_stmttmp17=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp3=_stmttmp17;void*_Tmp4;if(*((int*)_Tmp3)== 3){_Tmp4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1).elt_type;{void*t=_Tmp4;
# 944
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar(_check_null(e->topt));}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="is_poly_project: bad type ";_tag_fat(_Tmp7,sizeof(char),27U);});_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)_check_null(e1->topt);_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp7,sizeof(void*),2));});};}default:
# 947
 return 0;};}
# 952
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_Tmp0[1];_Tmp0[0]=s;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),1));});}
# 957
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_Tmp0[2];_Tmp0[0]=s,_Tmp0[1]=n;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),2));});}
# 962
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_Tmp0[2];_Tmp0[0]=rgn,_Tmp0[1]=s;Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_malloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),2));});}
# 966
static struct Cyc_Absyn_Exp*Cyc_Toc_rvmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_Tmp0[2];_Tmp0[0]=rgn,_Tmp0[1]=s;Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_vmalloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),2));});}
# 970
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_Tmp0[2];_Tmp0[0]=rgn,_Tmp0[1]=s;Cyc_Toc_fncall_exp_dl(Cyc_Toc__fast_region_malloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),2));});}
# 974
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_Tmp0[3];_Tmp0[0]=rgn,_Tmp0[1]=s,_Tmp0[2]=n;Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_calloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),3));});}
# 978
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt (void){
return Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__throw_match_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U);}
# 983
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 991
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_stmttmp18=e->r;void*_Tmp0=_stmttmp18;if(*((int*)_Tmp0)== 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Wstring_c).tag){case 8:
 goto _LL4;case 9: _LL4: {
# 996
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp3->tag=0,_Tmp3->f1=vd;_Tmp3;}),0U);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=Cyc_Toc_result_decls;_Tmp1;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5:
# 1004
 xexp=({void*_Tmp1=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_Tmp1,e);});
# 1006
xplussz=({void*_Tmp1=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_Tmp1,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1009
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_Tmp0[3];({struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=0,_Tmp2->f2=xexp;_Tmp2;});_Tmp0[0]=_Tmp1;}),({
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=0,_Tmp2->f2=xexp;_Tmp2;});_Tmp0[1]=_Tmp1;}),({
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=0,_Tmp2->f2=xplussz;_Tmp2;});_Tmp0[2]=_Tmp1;});({
# 1009
(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp0,sizeof(struct _tuple19*),3));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1044 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_Tmp0=_region_malloc(r,sizeof(struct Cyc_Toc_Env));_Tmp0->break_lab=0,_Tmp0->continue_lab=0,_Tmp0->fallthru_info=0,_Tmp0->toplevel=1,({int*_Tmp1=({int*_Tmp2=_region_malloc(r,sizeof(int));*_Tmp2=0;_Tmp2;});_Tmp0->in_lhs=_Tmp1;}),_Tmp0->rgn=r;_Tmp0;});}
# 1048
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_Tmp0=nv;int _Tmp1;_Tmp1=_Tmp0->toplevel;{int t=_Tmp1;
return t;}}
# 1052
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_Tmp0=e;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0->break_lab;_Tmp4=_Tmp0->continue_lab;_Tmp3=_Tmp0->fallthru_info;_Tmp2=_Tmp0->toplevel;_Tmp1=_Tmp0->in_lhs;{struct _fat_ptr**b=_Tmp5;struct _fat_ptr**c=_Tmp4;struct Cyc_Toc_FallthruInfo*f=_Tmp3;int t=_Tmp2;int*lhs=_Tmp1;
return({struct Cyc_Toc_Env*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Toc_Env));_Tmp6->break_lab=b,_Tmp6->continue_lab=c,_Tmp6->fallthru_info=f,_Tmp6->toplevel=0,_Tmp6->in_lhs=lhs,_Tmp6->rgn=r;_Tmp6;});}}
# 1056
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_Tmp0=e;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0->break_lab;_Tmp4=_Tmp0->continue_lab;_Tmp3=_Tmp0->fallthru_info;_Tmp2=_Tmp0->toplevel;_Tmp1=_Tmp0->in_lhs;{struct _fat_ptr**b=_Tmp5;struct _fat_ptr**c=_Tmp4;struct Cyc_Toc_FallthruInfo*f=_Tmp3;int t=_Tmp2;int*lhs=_Tmp1;
return({struct Cyc_Toc_Env*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Toc_Env));_Tmp6->break_lab=b,_Tmp6->continue_lab=c,_Tmp6->fallthru_info=f,_Tmp6->toplevel=1,_Tmp6->in_lhs=lhs,_Tmp6->rgn=r;_Tmp6;});}}
# 1060
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_Tmp0=nv;void*_Tmp1;_Tmp1=_Tmp0->in_lhs;{int*b=_Tmp1;
return*b;}}
# 1064
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_Tmp0=e;void*_Tmp1;_Tmp1=_Tmp0->in_lhs;{int*lhs=_Tmp1;
*lhs=b;}}
# 1069
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_Tmp0=e;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0->break_lab;_Tmp4=_Tmp0->continue_lab;_Tmp3=_Tmp0->fallthru_info;_Tmp2=_Tmp0->toplevel;_Tmp1=_Tmp0->in_lhs;{struct _fat_ptr**b=_Tmp5;struct _fat_ptr**c=_Tmp4;struct Cyc_Toc_FallthruInfo*f=_Tmp3;int t=_Tmp2;int*lhs=_Tmp1;
return({struct Cyc_Toc_Env*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Toc_Env));_Tmp6->break_lab=b,_Tmp6->continue_lab=c,_Tmp6->fallthru_info=f,_Tmp6->toplevel=t,_Tmp6->in_lhs=lhs,_Tmp6->rgn=r;_Tmp6;});}}
# 1076
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_Tmp0=e;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0->break_lab;_Tmp4=_Tmp0->continue_lab;_Tmp3=_Tmp0->fallthru_info;_Tmp2=_Tmp0->toplevel;_Tmp1=_Tmp0->in_lhs;{struct _fat_ptr**b=_Tmp5;struct _fat_ptr**c=_Tmp4;struct Cyc_Toc_FallthruInfo*f=_Tmp3;int t=_Tmp2;int*lhs=_Tmp1;
return({struct Cyc_Toc_Env*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Toc_Env));_Tmp6->break_lab=0,_Tmp6->continue_lab=0,_Tmp6->fallthru_info=f,_Tmp6->toplevel=t,_Tmp6->in_lhs=lhs,_Tmp6->rgn=r;_Tmp6;});}}
# 1082
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1086
struct Cyc_Toc_Env*_Tmp0=e;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0->break_lab;_Tmp4=_Tmp0->continue_lab;_Tmp3=_Tmp0->fallthru_info;_Tmp2=_Tmp0->toplevel;_Tmp1=_Tmp0->in_lhs;{struct _fat_ptr**b=_Tmp5;struct _fat_ptr**c=_Tmp4;struct Cyc_Toc_FallthruInfo*f=_Tmp3;int t=_Tmp2;int*lhs=_Tmp1;
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Toc_FallthruInfo));
_Tmp6->label=fallthru_l,_Tmp6->binders=fallthru_binders;_Tmp6;});
return({struct Cyc_Toc_Env*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Toc_Env));({struct _fat_ptr**_Tmp7=({struct _fat_ptr**_Tmp8=_region_malloc(r,sizeof(struct _fat_ptr*));*_Tmp8=break_l;_Tmp8;});_Tmp6->break_lab=_Tmp7;}),_Tmp6->continue_lab=c,_Tmp6->fallthru_info=fi,_Tmp6->toplevel=t,_Tmp6->in_lhs=lhs,_Tmp6->rgn=r;_Tmp6;});}}
# 1091
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1093
struct Cyc_Toc_Env*_Tmp0=e;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0->break_lab;_Tmp4=_Tmp0->continue_lab;_Tmp3=_Tmp0->fallthru_info;_Tmp2=_Tmp0->toplevel;_Tmp1=_Tmp0->in_lhs;{struct _fat_ptr**b=_Tmp5;struct _fat_ptr**c=_Tmp4;struct Cyc_Toc_FallthruInfo*f=_Tmp3;int t=_Tmp2;int*lhs=_Tmp1;
return({struct Cyc_Toc_Env*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Toc_Env));({struct _fat_ptr**_Tmp7=({struct _fat_ptr**_Tmp8=_region_malloc(r,sizeof(struct _fat_ptr*));*_Tmp8=break_l;_Tmp8;});_Tmp6->break_lab=_Tmp7;}),_Tmp6->continue_lab=c,_Tmp6->fallthru_info=0,_Tmp6->toplevel=t,_Tmp6->in_lhs=lhs,_Tmp6->rgn=r;_Tmp6;});}}
# 1100
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1102
struct Cyc_Toc_Env*_Tmp0=e;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0->break_lab;_Tmp4=_Tmp0->continue_lab;_Tmp3=_Tmp0->fallthru_info;_Tmp2=_Tmp0->toplevel;_Tmp1=_Tmp0->in_lhs;{struct _fat_ptr**b=_Tmp5;struct _fat_ptr**c=_Tmp4;struct Cyc_Toc_FallthruInfo*f=_Tmp3;int t=_Tmp2;int*lhs=_Tmp1;
return({struct Cyc_Toc_Env*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Toc_Env));_Tmp6->break_lab=0,_Tmp6->continue_lab=c,({struct Cyc_Toc_FallthruInfo*_Tmp7=({struct Cyc_Toc_FallthruInfo*_Tmp8=_region_malloc(r,sizeof(struct Cyc_Toc_FallthruInfo));_Tmp8->label=next_l,_Tmp8->binders=0;_Tmp8;});_Tmp6->fallthru_info=_Tmp7;}),_Tmp6->toplevel=t,_Tmp6->in_lhs=lhs,_Tmp6->rgn=r;_Tmp6;});}}
# 1115 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Stmt*);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Pat*,void*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*);
# 1119
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Stmt*);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Fndecl*,int);struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1122
static void Cyc_Toc_asm_iolist_types_toc(struct Cyc_List_List*l){
for(1;l != 0;l=l->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple32*)l->hd)).f2);}}
# 1127
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_stmttmp19=e->annot;void*_Tmp0=_stmttmp19;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_InsertChecks_NoCheck)
return 0;else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_InsertChecks_NullOnly)
goto _LL6;else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_InsertChecks_NullAndFatBound){_LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_InsertChecks_NullAndThinBound){_LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_InsertChecks_FatBound)
goto _LLC;else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_InsertChecks_ThinBound){_LLC:
 return 0;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="Toc: do_null_check";_tag_fat(_Tmp3,sizeof(char),19U);});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_Tmp2,sizeof(void*),1));});}}}}}};}
# 1143
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1145
int ans;
int old_lhs=Cyc_Toc_in_lhs(nv);
void*old_typ=Cyc_Absyn_compress(_check_null(ptr->topt));
void*new_typ=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_Tmp0=old_typ;void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;if(*((int*)_Tmp0)== 3){_Tmp4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_tq;_Tmp2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).bounds;_Tmp1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).zero_term;{void*ta=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*b=_Tmp2;void*zt=_Tmp1;
# 1156
{void*_Tmp5=annot;void*_Tmp6;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_Tmp5)->tag == Cyc_InsertChecks_NoCheck){
# 1158
if(!((unsigned)({void*_Tmp7=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp7,b);}))){
# 1161
void*newt=({void*_Tmp7=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_Tmp7,tq);});
({void*_Tmp7=({void*_Tmp8=newt;Cyc_Toc_cast_it_r(_Tmp8,({struct Cyc_Absyn_Exp*_Tmp9=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_Tmp9,Cyc_Toc_curr_sp,0U);}));});ptr->r=_Tmp7;});
ptr->topt=newt;}
# 1165
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_Tmp5)->tag == Cyc_InsertChecks_NullOnly){
# 1168
if(!((unsigned)({void*_Tmp7=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp7,b);}))){
# 1170
void*newt=({void*_Tmp7=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_Tmp7,tq);});
({void*_Tmp7=({void*_Tmp8=newt;Cyc_Toc_cast_it_r(_Tmp8,({struct Cyc_Absyn_Exp*_Tmp9=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_Tmp9,Cyc_Toc_curr_sp,0U);}));});ptr->r=_Tmp7;});
ptr->topt=newt;
# 1177
if(index != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="subscript of ? with no bounds check but need null check";_tag_fat(_Tmp9,sizeof(char),56U);});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp8,sizeof(void*),1));});}
# 1181
({void*_Tmp7=({struct Cyc_Absyn_Exp*_Tmp8[1];({struct Cyc_Absyn_Exp*_Tmp9=Cyc_Absyn_new_exp(ptr->r,0U);_Tmp8[0]=_Tmp9;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_Tmp8,sizeof(struct Cyc_Absyn_Exp*),1));})->r;ptr->r=_Tmp7;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_Tmp5)->tag == Cyc_InsertChecks_NullAndFatBound)
goto _LLD;else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_Tmp5)->tag == Cyc_InsertChecks_FatBound){_LLD: {
# 1187
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_Tmp7=({void*_Tmp8=newt;Cyc_Toc_cast_it_r(_Tmp8,({struct Cyc_Absyn_Exp*_Tmp9[3];({
struct Cyc_Absyn_Exp*_TmpA=Cyc_Absyn_new_exp(ptr->r,0U);_Tmp9[0]=_TmpA;}),({
struct Cyc_Absyn_Exp*_TmpA=Cyc_Absyn_sizeoftype_exp(ta1,0U);_Tmp9[1]=_TmpA;}),_Tmp9[2]=ind;Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_fat_subscript_e,_tag_fat(_Tmp9,sizeof(struct Cyc_Absyn_Exp*),3));}));});
# 1190
ptr->r=_Tmp7;});
# 1194
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_Tmp5)->tag == Cyc_InsertChecks_NullAndThinBound){_Tmp6=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Exp*bd=_Tmp6;
# 1198
fn_e=Cyc_Toc__check_known_subscript_null_e;
_Tmp6=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_Tmp5)->tag == Cyc_InsertChecks_ThinBound){_Tmp6=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_Tmp5)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_Tmp6;
# 1201
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1206
struct _tuple13 _stmttmp1A=Cyc_Evexp_eval_const_uint_exp(bd);struct _tuple13 _Tmp7=_stmttmp1A;int _Tmp8;unsigned _Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{unsigned i=_Tmp9;int valid=_Tmp8;
if((!valid || i != 1U)|| !Cyc_Tcutil_is_zeroterm_pointer_type(_check_null(ptr->topt))){
# 1209
({void*_TmpA=({void*_TmpB=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_TmpB,({struct Cyc_Absyn_Exp*_TmpC[4];({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_new_exp(ptr->r,0U);_TmpC[0]=_TmpD;}),_TmpC[1]=bd,({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_sizeoftype_exp(ta1,0U);_TmpC[2]=_TmpD;}),_TmpC[3]=ind;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),4));}));});
# 1209
ptr->r=_TmpA;});
# 1212
ans=1;}else{
# 1215
if(Cyc_Toc_is_zero(bd)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1218
({void*_TmpA=({struct Cyc_Absyn_Exp*_TmpB[1];({struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_new_exp(ptr->r,0U);_TmpB[0]=_TmpC;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),1));})->r;ptr->r=_TmpA;});
ans=0;}else{
# 1223
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_TmpA=({void*_TmpB=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_TmpB,({struct Cyc_Absyn_Exp*_TmpC[3];({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_new_exp(ptr->r,0U);_TmpC[0]=_TmpD;}),_TmpC[1]=bd,_TmpC[2]=ind;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));}));});
# 1224
ptr->r=_TmpA;});
# 1226
ans=1;}}
# 1229
goto _LL5;}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="FIX: ptr_use_to_c, bad annotation";_tag_fat(_Tmp9,sizeof(char),34U);});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp8,sizeof(void*),1));});}}}}}}_LL5:;}
# 1232
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="ptr_use_to_c: non-pointer-type";_tag_fat(_Tmp7,sizeof(char),31U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp6,sizeof(void*),1));});};}}
# 1238
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="Missing type in primop ";_tag_fat(_Tmp2,sizeof(char),24U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp1,sizeof(void*),1));});
return _check_null(e->topt);}
# 1242
static struct _tuple23*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple23*_Tmp0=_cycalloc(sizeof(struct _tuple23));_Tmp0->f1=Cyc_Toc_mt_tq,({void*_Tmp1=Cyc_Toc_typ_to_c(_check_null(e->topt));_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 1247
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_stmttmp1B=e->r;void*_Tmp0=_stmttmp1B;int _Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 26: _Tmp2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles=_Tmp2;
# 1250
{struct Cyc_List_List*dles2=dles;for(0;dles2 != 0;dles2=dles2->tl){
if((*((struct _tuple19*)dles2->hd)).f1 != 0)
({(int(*)(struct _fat_ptr))Cyc_Toc_unimp;})(({const char*_Tmp3="array designators for abstract-field initialization";_tag_fat(_Tmp3,sizeof(char),52U);}));}}
_Tmp2=Cyc_Absyn_signed_int_exp(({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(dles),0U);_Tmp1=0;goto _LL4;}case 27: _Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_Tmp2;int zt=_Tmp1;
_Tmp2=bd;_Tmp1=zt;goto _LL6;}case 28: _Tmp2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_Tmp2;int zt=_Tmp1;
# 1256
bd=Cyc_Absyn_copy_exp(bd);
return zt?({struct Cyc_Absyn_Exp*_Tmp3=bd;Cyc_Absyn_add_exp(_Tmp3,Cyc_Absyn_uint_exp(1U,0U),0U);}): bd;}default:
 return 0;};}
# 1265
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1273
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_stmttmp1C=e->r;void*_Tmp0=_stmttmp1C;void*_Tmp1;if(*((int*)_Tmp0)== 29){_Tmp1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*dles2=_Tmp1;
dles=dles2;goto _LL0;}}else{
return 0;}_LL0:;}
# 1279
{void*_stmttmp1D=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp0=_stmttmp1D;void*_Tmp1;if(*((int*)_Tmp0)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1).KnownAggr).tag == 2){_Tmp1=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 1281
if(ad->impl == 0 ||(int)ad->kind == 1)
return 0;
field_types=(_check_null(ad->impl))->fields;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 1287
 return 0;}_LL5:;}
# 1289
if(field_types == 0)
return 0;
for(1;(_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*last_type_field=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(1;dles != 0;dles=dles->tl){
struct _tuple19*_stmttmp1E=(struct _tuple19*)dles->hd;struct _tuple19*_Tmp0=_stmttmp1E;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct Cyc_List_List*ds=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(ds);
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*nested_ans=Cyc_Toc_get_varsizeexp(e2);
if(nested_ans != 0)
return nested_ans;{
void*_stmttmp1F=Cyc_Absyn_compress(last_type_field->type);void*_Tmp3=_stmttmp1F;void*_Tmp4;void*_Tmp5;if(*((int*)_Tmp3)== 4){_Tmp5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp3)->f1).elt_type;_Tmp4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp3)->f1).num_elts;{void*elt_type=_Tmp5;struct Cyc_Absyn_Exp*type_bd=_Tmp4;
# 1304
if(type_bd == 0 || !Cyc_Toc_is_zero(type_bd))
return 0;
# 1310
return({struct Cyc_Absyn_Exp*_Tmp6=({struct Cyc_Absyn_Exp*_Tmp7[2];_Tmp7[0]=
_check_null(Cyc_Toc_array_length_exp(e2)),({
struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_Tmp7[1]=_Tmp8;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_times_e,_tag_fat(_Tmp7,sizeof(struct Cyc_Absyn_Exp*),2));});
# 1310
Cyc_Absyn_add_exp(_Tmp6,
# 1313
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}}else{
return 0;};}}}}
# 1318
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="get_varsizeexp: did not find last struct field";_tag_fat(_Tmp2,sizeof(char),47U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp1,sizeof(void*),1));});}}
# 1321
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
struct Cyc_List_List*fs=(_check_null(ad->impl))->fields;
{int i=1;for(0;fs != 0;(fs=fs->tl,++ i)){
if(Cyc_strcmp(*((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,*f)== 0)
return i;}}
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="get_member_offset ";_tag_fat(_Tmp2,sizeof(char),19U);});_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=*f;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=" failed";_tag_fat(_Tmp4,sizeof(char),8U);});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp3,sizeof(void*),3));});}struct _tuple33{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1330
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple33*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1333
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1336
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_Tmp0->tag=5,_Tmp0->f1=e1,_Tmp0->f2=incr;_Tmp0;}),0U);}
# 1340
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,void*),void*);
# 1349
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1351
void*_stmttmp20=e1->r;void*_Tmp0=_stmttmp20;int _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 37: _Tmp4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp4;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14: _Tmp4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21: _Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e=_Tmp4;struct _fat_ptr*fld=_Tmp3;int is_tagged=_Tmp2;int is_read=_Tmp1;
# 1356
e1->r=e->r;
({struct Cyc_Absyn_Exp*_Tmp5=e1;struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=fld,_Tmp7->tl=fs;_Tmp7;});struct Cyc_Absyn_Exp*(*_Tmp7)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_Tmp5,_Tmp6,_Tmp7,f_env);});
goto _LL0;}default:  {
# 1364
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1366
for(1;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_Tmp5=(f(e1_copy,f_env))->r;e1->r=_Tmp5;});
goto _LL0;}}_LL0:;}
# 1372
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1374
void*_stmttmp21=s->r;void*_Tmp0=_stmttmp21;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: _Tmp2=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12: _Tmp2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
# 1377
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2: _Tmp2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s2=_Tmp2;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="lvalue_assign_stmt: ";_tag_fat(_Tmp5,sizeof(char),21U);});_Tmp4;});struct Cyc_Warn_Stmt_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Stmt_Warn_Warg_struct _Tmp5;_Tmp5.tag=5,_Tmp5.f1=s;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Toc_toc_impos(_tag_fat(_Tmp5,sizeof(void*),2));});}_LL0:;}
# 1385
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*);
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_stmttmp22=e->r;void*_Tmp0=_stmttmp22;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 14: _Tmp2=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void**t=_Tmp2;struct Cyc_Absyn_Exp**e1=(struct Cyc_Absyn_Exp**)_Tmp1;
# 1389
({struct Cyc_Absyn_Exp*_Tmp3=Cyc_Toc_push_address_exp(*e1);*e1=_Tmp3;});
({void*_Tmp3=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_Tmp3;});
return e;}case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
return e1;}case 37: _Tmp2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp2;
# 1395
Cyc_Toc_push_address_stmt(s);return e;}default:
# 1397
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="can't take & of exp ";_tag_fat(_Tmp5,sizeof(char),21U);});_Tmp4;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp5;_Tmp5.tag=4,_Tmp5.f1=e;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp5,sizeof(void*),2));});};}
# 1401
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_stmttmp23=s->r;void*_Tmp0=_stmttmp23;void*_Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s2=_Tmp1;
_Tmp1=s2;goto _LL4;}case 12: _Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_Tmp1;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1: _Tmp1=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp**e=(struct Cyc_Absyn_Exp**)_Tmp1;
({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Toc_push_address_exp(*e);*e=_Tmp2;});goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="can't take & of stmt ";_tag_fat(_Tmp4,sizeof(char),22U);});_Tmp3;});struct Cyc_Warn_Stmt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Stmt_Warn_Warg_struct _Tmp4;_Tmp4.tag=5,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp4,sizeof(void*),2));});}_LL0:;}
# 1413
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1425
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type,Cyc_Absyn_false_type);
void*c_elt_type=Cyc_Toc_typ_to_c(elt_type);
void*c_fat_ptr_type=Cyc_Toc_typ_to_c(fat_ptr_type);
void*c_ptr_type=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*c_ptr_type_opt=({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=c_ptr_type;_Tmp0;});
struct Cyc_Absyn_Exp*xinit;
{void*_stmttmp24=e1->r;void*_Tmp0=_stmttmp24;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*ea=_Tmp2;
# 1433
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,3U,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1438
Cyc_Toc_exp_to_c(nv,ea);
xinit=ea;
goto _LL0;}case 23: _Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*ea=_Tmp2;struct Cyc_Absyn_Exp*eb=_Tmp1;
# 1442
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,3U,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1447
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_Tmp3[3];_Tmp3[0]=ea,({
struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_Tmp3[1]=_Tmp4;}),_Tmp3[2]=eb;Cyc_Toc_fncall_exp_dl(Cyc_Toc__fat_ptr_plus_e,_tag_fat(_Tmp3,sizeof(struct Cyc_Absyn_Exp*),3));});
goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="found bad lhs for zero-terminated pointer assignment";_tag_fat(_Tmp5,sizeof(char),53U);});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp4,sizeof(void*),1));});}_LL0:;}{
# 1453
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _Tmp0=_new_region("rgn2");struct _RegionHandle*rgn2=& _Tmp0;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*x_vd=({struct Cyc_Absyn_Vardecl*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_Tmp1->sc=2U,_Tmp1->name=x,_Tmp1->varloc=0U,_Tmp1->tq=Cyc_Toc_mt_tq,_Tmp1->type=c_fat_ptr_type,_Tmp1->initializer=xinit,_Tmp1->rgn=0,_Tmp1->attributes=0,_Tmp1->escapes=0,_Tmp1->is_proto=0;_Tmp1;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp1->tag=4,_Tmp1->f1=x_vd;_Tmp1;});
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_varb_exp((void*)x_bnd,0U);
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*deref_x=Cyc_Absyn_deref_exp(x_exp,0U);
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*y=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*y_vd=({struct Cyc_Absyn_Vardecl*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_Tmp1->sc=2U,_Tmp1->name=y,_Tmp1->varloc=0U,_Tmp1->tq=Cyc_Toc_mt_tq,_Tmp1->type=c_elt_type,_Tmp1->initializer=deref_x,_Tmp1->rgn=0,_Tmp1->attributes=0,_Tmp1->escapes=0,_Tmp1->is_proto=0;_Tmp1;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp1->tag=4,_Tmp1->f1=y_vd;_Tmp1;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*y_exp=Cyc_Absyn_varb_exp((void*)y_bnd,0U);
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _Tmp1=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_Tmp2=y_exp;Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1474
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*z=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*z_vd=({struct Cyc_Absyn_Vardecl*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_Tmp1->sc=2U,_Tmp1->name=z,_Tmp1->varloc=0U,_Tmp1->tq=Cyc_Toc_mt_tq,_Tmp1->type=c_elt_type,_Tmp1->initializer=z_init,_Tmp1->rgn=0,_Tmp1->attributes=0,_Tmp1->escapes=0,_Tmp1->is_proto=0;_Tmp1;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp1->tag=4,_Tmp1->f1=z_vd;_Tmp1;});
# 1480
struct Cyc_Absyn_Exp*y2_exp=Cyc_Absyn_varb_exp((void*)y_bnd,0U);y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*zero1_exp=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*comp1_exp=Cyc_Absyn_prim2_exp(5U,y2_exp,zero1_exp,0U);
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1487
struct Cyc_Absyn_Exp*z_exp=Cyc_Absyn_varb_exp((void*)z_bnd,0U);z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*zero2_exp=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*comp2_exp=Cyc_Absyn_prim2_exp(6U,z_exp,zero2_exp,0U);
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1494
struct Cyc_List_List*xsizeargs=({struct Cyc_Absyn_Exp*_Tmp1[2];({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_Tmp1[0]=_Tmp2;}),({
struct Cyc_Absyn_Exp*_Tmp2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_Tmp1[1]=_Tmp2;});({
# 1494
(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp1,sizeof(struct Cyc_Absyn_Exp*),2));});
# 1496
struct Cyc_Absyn_Exp*oneexp=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(5U,_Tmp1,oneexp,0U);});{
# 1501
struct Cyc_Absyn_Exp*comp_exp=({struct Cyc_Absyn_Exp*_Tmp1=xsize;Cyc_Absyn_and_exp(_Tmp1,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});
struct Cyc_Absyn_Stmt*thr_stmt=Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__throw_arraybounds_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U);
struct Cyc_Absyn_Exp*xcurr=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_Tmp1,Cyc_Toc_curr_sp,0U);});
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*deref_xcurr=Cyc_Absyn_deref_exp(xcurr,0U);
struct Cyc_Absyn_Exp*asn_exp=({struct Cyc_Absyn_Exp*_Tmp1=deref_xcurr;Cyc_Absyn_assign_exp(_Tmp1,Cyc_Absyn_var_exp(z,0U),0U);});
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(asn_exp,0U);
s=({struct Cyc_Absyn_Stmt*_Tmp1=({struct Cyc_Absyn_Exp*_Tmp2=comp_exp;struct Cyc_Absyn_Stmt*_Tmp3=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_Tmp2,_Tmp3,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_Tmp1,s,0U);});
s=({struct Cyc_Absyn_Decl*_Tmp1=({struct Cyc_Absyn_Decl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp4->tag=0,_Tmp4->f1=z_vd;_Tmp4;});_Tmp2->r=_Tmp3;}),_Tmp2->loc=0U;_Tmp2;});Cyc_Absyn_decl_stmt(_Tmp1,s,0U);});
s=({struct Cyc_Absyn_Decl*_Tmp1=({struct Cyc_Absyn_Decl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp4->tag=0,_Tmp4->f1=y_vd;_Tmp4;});_Tmp2->r=_Tmp3;}),_Tmp2->loc=0U;_Tmp2;});Cyc_Absyn_decl_stmt(_Tmp1,s,0U);});
s=({struct Cyc_Absyn_Decl*_Tmp1=({struct Cyc_Absyn_Decl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp4->tag=0,_Tmp4->f1=x_vd;_Tmp4;});_Tmp2->r=_Tmp3;}),_Tmp2->loc=0U;_Tmp2;});Cyc_Absyn_decl_stmt(_Tmp1,s,0U);});
({void*_Tmp1=Cyc_Toc_stmt_exp_r(s);e->r=_Tmp1;});}}}}}}}}}}
# 1455
;_pop_region();}}
# 1527 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1531
struct Cyc_Absyn_Aggrdecl*ad;
{void*_stmttmp25=Cyc_Absyn_compress(aggrtype);void*_Tmp0=_stmttmp25;union Cyc_Absyn_AggrInfo _Tmp1;if(*((int*)_Tmp0)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 20){_Tmp1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp1;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="expecting union but found ";_tag_fat(_Tmp4,sizeof(char),27U);});_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)aggrtype;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=" in check_tagged_union";_tag_fat(_Tmp6,sizeof(char),23U);});_Tmp5;});void*_Tmp5[3];_Tmp5[0]=& _Tmp2,_Tmp5[1]=& _Tmp3,_Tmp5[2]=& _Tmp4;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp5,sizeof(void*),3));});}_LL0:;}{
# 1537
struct _tuple30 _stmttmp26=Cyc_Toc_temp_var_and_exp();struct _tuple30 _Tmp0=_stmttmp26;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct _tuple1*temp=_Tmp2;struct Cyc_Absyn_Exp*temp_exp=_Tmp1;
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*temp_f_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);
struct Cyc_Absyn_Exp*temp_f_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*sres=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);
struct Cyc_Absyn_Stmt*ifs=({struct Cyc_Absyn_Exp*_Tmp3=test_exp;struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_Tmp3,_Tmp4,Cyc_Toc_skip_stmt_dl(),0U);});
void*e1_p_type=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*e1_f=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_Tmp3=temp;void*_Tmp4=e1_p_type;struct Cyc_Absyn_Exp*_Tmp5=e1_f;Cyc_Absyn_declare_stmt(_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});
return Cyc_Toc_stmt_exp_r(s);}else{
# 1550
struct Cyc_Absyn_Exp*temp_f_tag=({struct Cyc_Absyn_Exp*_Tmp3=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_Tmp3,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);
struct Cyc_Absyn_Exp*temp_f_val=({struct Cyc_Absyn_Exp*_Tmp3=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_Tmp3,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*sres=Cyc_Absyn_exp_stmt(temp_f_val,0U);
struct Cyc_Absyn_Stmt*ifs=({struct Cyc_Absyn_Exp*_Tmp3=test_exp;struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_Tmp3,_Tmp4,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_Tmp3=temp;void*_Tmp4=e1_c_type;struct Cyc_Absyn_Exp*_Tmp5=e1;Cyc_Absyn_declare_stmt(_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1560
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1563
void*_stmttmp27=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp27;union Cyc_Absyn_AggrInfo _Tmp1;if(*((int*)_Tmp0)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 20){_Tmp1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp1;
# 1565
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
({int _Tmp2=Cyc_Toc_get_member_offset(ad,f);*f_tag=_Tmp2;});{
# 1568
struct _fat_ptr str=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=*(*ad->name).f2;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=*f;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_aprintf(({const char*_Tmp5="_union_%s_%s";_tag_fat(_Tmp5,sizeof(char),13U);}),_tag_fat(_Tmp4,sizeof(void*),2));});
({void*_Tmp2=Cyc_Absyn_strct(({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=str;_Tmp3;}));*tagged_member_type=_Tmp2;});
if(clear_read)*_check_null(is_read)=0;
return(_check_null(ad->impl))->tagged;}}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 1579
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1582
void*_stmttmp28=e->r;void*_Tmp0=_stmttmp28;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 14: _Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp3;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="cast on lhs!";_tag_fat(_Tmp6,sizeof(char),13U);});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp5,sizeof(void*),1));});}case 21: _Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct _fat_ptr*f=_Tmp2;int*is_read=_Tmp1;
# 1585
return Cyc_Toc_is_tagged_union_project_impl(_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22: _Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct _fat_ptr*f=_Tmp2;int*is_read=(int*)_Tmp1;
# 1588
void*_stmttmp29=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp4=_stmttmp29;void*_Tmp5;if(*((int*)_Tmp4)== 3){_Tmp5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1).elt_type;{void*et=_Tmp5;
# 1590
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{
# 1592
return 0;};}default:
# 1594
 return 0;};}
# 1607 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1611
struct _tuple30 _stmttmp2A=Cyc_Toc_temp_var_and_exp();struct _tuple30 _Tmp0=_stmttmp2A;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct _tuple1*temp=_Tmp2;struct Cyc_Absyn_Exp*temp_exp=_Tmp1;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1620
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
s2=({struct Cyc_Absyn_Exp*_Tmp3=test_exp;struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_Tmp3,_Tmp4,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1623
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_Tmp3=temp;void*_Tmp4=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_Tmp5=
Cyc_Toc_push_address_exp(e1);
# 1623
Cyc_Absyn_declare_stmt(_Tmp3,_Tmp4,_Tmp5,
# 1625
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1630
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="exp_to_c: no type for ";_tag_fat(_Tmp2,sizeof(char),23U);});_Tmp1;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=e;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});{
void*old_typ=_check_null(e->topt);
# 1635
int did_inserted_checks=0;
{void*_stmttmp2B=e->annot;void*_Tmp0=_stmttmp2B;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_Absyn_EmptyAnnot)
goto _LL4;else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_Tmp0)->tag == Cyc_InsertChecks_NoCheck){_LL4:
 did_inserted_checks=1;goto _LL0;}else{
goto _LL0;}}_LL0:;}
# 1641
{void*_stmttmp2C=e->r;void*_Tmp0=_stmttmp2C;enum Cyc_Absyn_MallocKind _Tmp1;int _Tmp2;enum Cyc_Absyn_Coercion _Tmp3;void*_Tmp4;int _Tmp5;void*_Tmp6;void*_Tmp7;enum Cyc_Absyn_Incrementor _Tmp8;enum Cyc_Absyn_Primop _Tmp9;void*_TmpA;switch(*((int*)_Tmp0)){case 2:
# 1643
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Null_c).tag == 1){
# 1649
struct Cyc_Absyn_Exp*zero=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_TmpB=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero))->r;e->r=_TmpB;});else{
# 1654
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[3];_TmpC[0]=zero,_TmpC[1]=zero,_TmpC[2]=zero;Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_fat_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));});e->r=_TmpB;});}}else{
# 1656
e->r=(void*)& Cyc_Toc_zero_exp;}
goto _LL7;}else{
goto _LL7;}case 1:
 goto _LL7;case 42:
({void*_TmpB=(Cyc_Absyn_int_exp(0U,1,0U))->r;e->r=_TmpB;});goto _LL7;case 41: _TmpA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3: _Tmp9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp9;struct Cyc_List_List*es=_TmpA;
# 1664
struct Cyc_List_List*old_types=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Toc_get_cyc_type,es);
# 1666
({(void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Toc_exp_to_c,nv,es);
{enum Cyc_Absyn_Primop _TmpB=p;switch((int)_TmpB){case Cyc_Absyn_Numelts:  {
# 1669
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)(_check_null(es))->hd;
{void*_stmttmp2D=Cyc_Absyn_compress(_check_null(arg->topt));void*_TmpC=_stmttmp2D;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;if(*((int*)_TmpC)== 3){_Tmp10=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpC)->f1).elt_type;_TmpF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpC)->f1).ptr_atts).nullable;_TmpE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpC)->f1).ptr_atts).bounds;_TmpD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpC)->f1).ptr_atts).zero_term;{void*elt_type=_Tmp10;void*nbl=_TmpF;void*bound=_TmpE;void*zt=_TmpD;
# 1672
struct Cyc_Absyn_Exp*eopt=({void*_Tmp11=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp11,bound);});
if(eopt == 0)
# 1675
({void*_Tmp11=({struct Cyc_Absyn_Exp*_Tmp12[2];_Tmp12[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_Tmp13=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_Tmp12[1]=_Tmp13;});Cyc_Toc_fncall_exp_r(Cyc_Toc__get_fat_size_e,_tag_fat(_Tmp12,sizeof(struct Cyc_Absyn_Exp*),2));});
# 1675
e->r=_Tmp11;});else{
# 1677
if(Cyc_Tcutil_force_type2bool(0,zt)){
# 1679
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)es->hd);
({void*_Tmp11=({struct Cyc_Absyn_Exp*_Tmp12[2];_Tmp12[0]=(struct Cyc_Absyn_Exp*)es->hd,_Tmp12[1]=eopt;Cyc_Toc_fncall_exp_r(function_e,_tag_fat(_Tmp12,sizeof(struct Cyc_Absyn_Exp*),2));});e->r=_Tmp11;});}else{
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(eopt))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=({const char*_Tmp13="can't calculate numelts";_tag_fat(_Tmp13,sizeof(char),24U);});_Tmp12;});void*_Tmp12[1];_Tmp12[0]=& _Tmp11;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp12,sizeof(void*),1));});
# 1686
({void*_Tmp11=({struct Cyc_Absyn_Exp*_Tmp12=arg;struct Cyc_Absyn_Exp*_Tmp13=eopt;Cyc_Toc_conditional_exp_r(_Tmp12,_Tmp13,Cyc_Absyn_uint_exp(0U,0U));});e->r=_Tmp11;});}else{
# 1688
e->r=eopt->r;}}}
goto _LL77;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=({const char*_Tmp13="numelts primop applied to non-pointer ";_tag_fat(_Tmp13,sizeof(char),39U);});_Tmp12;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp13;_Tmp13.tag=2,_Tmp13.f1=(void*)
_check_null(arg->topt);_Tmp13;});void*_Tmp13[2];_Tmp13[0]=& _Tmp11,_Tmp13[1]=& _Tmp12;({
# 1690
(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp13,sizeof(void*),2));});}_LL77:;}
# 1693
goto _LL62;}case Cyc_Absyn_Plus:
# 1698
 if(Cyc_Toc_is_toplevel(nv))
({(int(*)(struct _fat_ptr))Cyc_Toc_unimp;})(({const char*_TmpC="can't do pointer arithmetic at top-level";_tag_fat(_TmpC,sizeof(char),41U);}));
{void*_stmttmp2E=Cyc_Absyn_compress((void*)(_check_null(old_types))->hd);void*_TmpC=_stmttmp2E;void*_TmpD;void*_TmpE;void*_TmpF;if(*((int*)_TmpC)== 3){_TmpF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpC)->f1).elt_type;_TmpE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpC)->f1).ptr_atts).bounds;_TmpD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpC)->f1).ptr_atts).zero_term;{void*elt_type=_TmpF;void*b=_TmpE;void*zt=_TmpD;
# 1702
struct Cyc_Absyn_Exp*eopt=({void*_Tmp10=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp10,b);});
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)(_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd;
if(eopt == 0)
({void*_Tmp10=({struct Cyc_Absyn_Exp*_Tmp11[3];_Tmp11[0]=e1,({
struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_Tmp11[1]=_Tmp12;}),_Tmp11[2]=e2;Cyc_Toc_fncall_exp_r(Cyc_Toc__fat_ptr_plus_e,_tag_fat(_Tmp11,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1706
e->r=_Tmp10;});else{
# 1708
if(Cyc_Tcutil_force_type2bool(0,zt))
({void*_Tmp10=({struct Cyc_Absyn_Exp*_Tmp11[3];_Tmp11[0]=e1,_Tmp11[1]=eopt,_Tmp11[2]=e2;({struct Cyc_Absyn_Exp*_Tmp12=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_Tmp12,_tag_fat(_Tmp11,sizeof(struct Cyc_Absyn_Exp*),3));});});e->r=_Tmp10;});}
goto _LL7C;}}else{
goto _LL7C;}_LL7C:;}
# 1713
goto _LL62;case Cyc_Absyn_Minus:  {
# 1718
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)(_check_null(old_types))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)(_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)(_check_null(old_types->tl))->hd)){
({void*_TmpC=({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_TmpD,Cyc_Toc_curr_sp);});e1->r=_TmpC;});
({void*_TmpC=({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_TmpD,Cyc_Toc_curr_sp);});e2->r=_TmpC;});
({void*_TmpC=({void*_TmpD=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_TmpD;});e1->topt=_TmpC;});
({void*_TmpC=({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_TmpD,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1726
e->r=_TmpC;});}else{
# 1729
({void*_TmpC=({struct Cyc_Absyn_Exp*_TmpD[3];_TmpD[0]=e1,({
struct Cyc_Absyn_Exp*_TmpE=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_TmpD[1]=_TmpE;}),({
struct Cyc_Absyn_Exp*_TmpE=Cyc_Absyn_prim1_exp(2U,e2,0U);_TmpD[2]=_TmpE;});Cyc_Toc_fncall_exp_r(Cyc_Toc__fat_ptr_plus_e,_tag_fat(_TmpD,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1729
e->r=_TmpC;});}}
# 1733
goto _LL62;}case Cyc_Absyn_Eq:
 goto _LL6C;case Cyc_Absyn_Neq: _LL6C: goto _LL6E;case Cyc_Absyn_Gt: _LL6E: goto _LL70;case Cyc_Absyn_Gte: _LL70: goto _LL72;case Cyc_Absyn_Lt: _LL72: goto _LL74;case Cyc_Absyn_Lte: _LL74: {
# 1736
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)(_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd;
void*t1=(void*)(_check_null(old_types))->hd;
void*t2=(void*)(_check_null(old_types->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*t=({void*_TmpC=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_TmpC,Cyc_Toc_mt_tq);});
({void*_TmpC=({void*_TmpD=t;Cyc_Toc_cast_it_r(_TmpD,({struct Cyc_Absyn_Exp*_TmpE=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_TmpE,Cyc_Toc_curr_sp,0U);}));});e1->r=_TmpC;});
e1->topt=t;}
# 1746
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*t=({void*_TmpC=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_TmpC,Cyc_Toc_mt_tq);});
({void*_TmpC=({void*_TmpD=t;Cyc_Toc_cast_it_r(_TmpD,({struct Cyc_Absyn_Exp*_TmpE=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_TmpE,Cyc_Toc_curr_sp,0U);}));});e2->r=_TmpC;});
e2->topt=t;}
# 1751
goto _LL62;}default:
 goto _LL62;}_LL62:;}
# 1754
goto _LL7;}case 5: _TmpA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_TmpA;enum Cyc_Absyn_Incrementor incr=_Tmp8;
# 1756
void*e2_cyc_typ=_check_null(e2->topt);
# 1765 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_TmpB="increment";_tag_fat(_TmpB,sizeof(char),10U);});
if((int)incr == 2 ||(int)incr == 3)incr_str=({const char*_TmpB="decrement";_tag_fat(_TmpB,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD="in-place ";_tag_fat(_TmpD,sizeof(char),10U);});_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=incr_str;_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF=" is not supported when ";_tag_fat(_TmpF,sizeof(char),24U);});_TmpE;});struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=({const char*_Tmp10="dereferencing a zero-terminated pointer";_tag_fat(_Tmp10,sizeof(char),40U);});_TmpF;});void*_TmpF[4];_TmpF[0]=& _TmpB,_TmpF[1]=& _TmpC,_TmpF[2]=& _TmpD,_TmpF[3]=& _TmpE;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_TmpF,sizeof(void*),4));});
# 1773
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _TmpB=incr;switch((int)_TmpB){case Cyc_Absyn_PreInc:
 op=0U;goto _LL81;case Cyc_Absyn_PreDec:
 op=2U;goto _LL81;default:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE="in-place post";_tag_fat(_TmpE,sizeof(char),14U);});_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=incr_str;_TmpE;});struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=({const char*_Tmp10=" is not supported on @tagged union members";_tag_fat(_Tmp10,sizeof(char),43U);});_TmpF;});void*_TmpF[3];_TmpF[0]=& _TmpC,_TmpF[1]=& _TmpD,_TmpF[2]=& _TmpE;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_TmpF,sizeof(void*),3));});}_LL81:;}
# 1783
({void*_TmpB=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct));_TmpC->tag=4,_TmpC->f1=e2,({struct Cyc_Core_Opt*_TmpD=({struct Cyc_Core_Opt*_TmpE=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpE->v=(void*)op;_TmpE;});_TmpC->f2=_TmpD;}),_TmpC->f3=one;_TmpC;});e->r=_TmpB;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1787
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1792
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
int change=1;
struct Cyc_Absyn_Exp*fn_e=((int)incr == 1 ||(int)incr == 3)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;
# 1797
if((int)incr == 2 ||(int)incr == 3)
change=-1;
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[3];({struct Cyc_Absyn_Exp*_TmpD=Cyc_Toc_push_address_exp(e2);_TmpC[0]=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_TmpC[1]=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_signed_int_exp(change,0U);_TmpC[2]=_TmpD;});Cyc_Toc_fncall_exp_r(fn_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1799
e->r=_TmpB;});}else{
# 1802
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1808
did_inserted_checks=1;
if((int)incr != 1){
struct _tuple1*x=Cyc_Toc_temp_var();
void*t=({void*_TmpB=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_TmpB,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*xexp=Cyc_Toc_push_address_exp(e2);
struct Cyc_Absyn_Exp*testexp=({struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_TmpB,
Cyc_Absyn_int_exp(2U,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_TmpC,
Cyc_Absyn_int_exp(2U,0,0U),0U);});
# 1816
Cyc_Absyn_and_exp(_TmpB,testexp,0U);});{
# 1818
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_TmpB=testexp;struct Cyc_Absyn_Stmt*_TmpC=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U,0U),0U);
# 1818
Cyc_Absyn_ifthenelse_stmt(_TmpB,_TmpC,
# 1820
Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__throw_arraybounds_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U),0U);});
s=({struct Cyc_Absyn_Stmt*_TmpB=s;Cyc_Absyn_seq_stmt(_TmpB,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_TmpB=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U))->r;e->r=_TmpB;});}}else{
# 1825
struct Cyc_Toc_functionSet*fnSet=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(fnSet,e2);
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[2];({struct Cyc_Absyn_Exp*_TmpD=Cyc_Toc_push_address_exp(e2);_TmpC[0]=_TmpD;}),({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_signed_int_exp(1,0U);_TmpC[1]=_TmpD;});Cyc_Toc_fncall_exp_r(fn_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),2));});e->r=_TmpB;});}}else{
# 1829
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
({(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor))Cyc_Toc_lvalue_assign;})(e2,0,Cyc_Toc_incr_lvalue,incr);
*e=*e2;}}}
# 1833
goto _LL7;}}case 4: _TmpA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Core_Opt*popt=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1852 "toc.cyc"
void*e1_old_typ=_check_null(e1->topt);
void*e2_old_typ=_check_null(e2->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(e1,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);
({void*_TmpB=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_TmpB;});
# 1863
goto _LL7;}{
# 1865
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
return;}{
# 1874
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1880
int done=0;
if(popt != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _stmttmp2F=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _TmpB=_stmttmp2F;switch((int)_TmpB){case Cyc_Absyn_Plus:
 change=e2;goto _LL88;case Cyc_Absyn_Minus:
 change=Cyc_Absyn_prim1_exp(2U,e2,0U);goto _LL88;default:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE="bad t ? pointer arithmetic";_tag_fat(_TmpE,sizeof(char),27U);});_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpD,sizeof(void*),1));});}_LL88:;}
# 1890
done=1;{
# 1892
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__fat_ptr_inplace_plus_e;
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[3];({struct Cyc_Absyn_Exp*_TmpD=Cyc_Toc_push_address_exp(e1);_TmpC[0]=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_TmpC[1]=_TmpD;}),_TmpC[2]=change;Cyc_Toc_fncall_exp_r(fn_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1893
e->r=_TmpB;});}}else{
# 1896
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1899
enum Cyc_Absyn_Primop _stmttmp30=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _TmpB=_stmttmp30;if(_TmpB == Cyc_Absyn_Plus){
# 1901
done=1;
({void*_TmpC=({struct Cyc_Absyn_Exp*_TmpD[2];_TmpD[0]=e1,_TmpD[1]=e2;({struct Cyc_Absyn_Exp*_TmpE=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_TmpE,_tag_fat(_TmpD,sizeof(struct Cyc_Absyn_Exp*),2));});});e->r=_TmpC;});
goto _LL8F;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE="bad zero-terminated pointer arithmetic";_tag_fat(_TmpE,sizeof(char),39U);});_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpD,sizeof(void*),1));});}_LL8F:;}}}
# 1907
if(!done){
# 1909
if(e1_poly)
({void*_TmpB=({void*_TmpC=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_TmpC,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_TmpB;});
# 1915
if(!Cyc_Absyn_is_lvalue(e1)){
({void(*_TmpB)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*)=({(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*))Cyc_Toc_lvalue_assign;});struct Cyc_Absyn_Exp*_TmpC=e1;_TmpB(_TmpC,0,Cyc_Toc_assignop_lvalue,({struct _tuple33*_TmpD=_cycalloc(sizeof(struct _tuple33));_TmpD->f1=popt,_TmpD->f2=e2;_TmpD;}));});
*e=*e1;}}
# 1920
goto _LL7;}}}}case 6: _TmpA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;struct Cyc_Absyn_Exp*e3=_Tmp6;
# 1922
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7: _TmpA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
_TmpA=e1;_Tmp7=e2;goto _LL1F;}case 8: _TmpA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1F: {struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
_TmpA=e1;_Tmp7=e2;goto _LL21;}case 9: _TmpA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 1929
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3 == 0){_TmpA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_List_List*es=_Tmp7;
# 1933
void*e1_typ=_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[1];({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_copy_exp(e1);_TmpC[0]=_TmpD;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),1));})->r;e1->r=_TmpB;});
({(void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_TmpA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3)->num_varargs;_Tmp6=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3)->injectors;_Tmp4=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3)->vai;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_List_List*es=_Tmp7;int num_varargs=_Tmp5;struct Cyc_List_List*injectors=_Tmp6;struct Cyc_Absyn_VarargInfo*vai=_Tmp4;
# 1949 "toc.cyc"
struct _RegionHandle _TmpB=_new_region("r");struct _RegionHandle*r=& _TmpB;_push_region(r);{
struct _tuple30 _stmttmp31=Cyc_Toc_temp_var_and_exp();struct _tuple30 _TmpC=_stmttmp31;void*_TmpD;void*_TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{struct _tuple1*argv=_TmpE;struct Cyc_Absyn_Exp*argvexp=_TmpD;
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_signed_int_exp(num_varargs,0U);
void*cva_type=Cyc_Toc_typ_to_c(vai->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 1956
int num_args=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(es);
int num_normargs=num_args - num_varargs;
# 1960
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,es=es->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)(_check_null(es))->hd);
new_args=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(struct Cyc_Absyn_Exp*)es->hd,_TmpF->tl=new_args;_TmpF;});}}
# 1965
new_args=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp10=({struct Cyc_Absyn_Exp*_Tmp11[3];
num_varargs == 0?({struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_uint_exp(0U,0U);_Tmp11[0]=_Tmp12;}):(_Tmp11[0]=argvexp),({
struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_Tmp11[1]=_Tmp12;}),_Tmp11[2]=num_varargs_exp;Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_Tmp11,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1965
_TmpF->hd=_Tmp10;}),_TmpF->tl=new_args;_TmpF;});
# 1970
new_args=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(new_args);{
# 1972
void*e1_typ=_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[1];({struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_copy_exp(e1);_Tmp10[0]=_Tmp11;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),1));})->r;e1->r=_TmpF;});
if(num_varargs == 0){
({void*_TmpF=(Cyc_Absyn_fncall_exp(e1,new_args,0U))->r;e->r=_TmpF;});
_npop_handler(0);goto _LL7;}{
# 1981
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 1984
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_stmttmp32=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_TmpF=_stmttmp32;void*_Tmp10;if(*((int*)_TmpF)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF)->f1)== 18){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF)->f1)->f1).KnownDatatype).tag == 2){_Tmp10=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF)->f1)->f1).KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl*x=_Tmp10;
tud=x;goto _LL97;}}else{goto _LL9A;}}else{goto _LL9A;}}else{_LL9A:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=({const char*_Tmp13="unknown datatype in vararg with inject";_tag_fat(_Tmp13,sizeof(char),39U);});_Tmp12;});void*_Tmp12[1];_Tmp12[0]=& _Tmp11;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp12,sizeof(void*),1));});}_LL97:;}{
# 1990
struct _fat_ptr vs=({unsigned _TmpF=(unsigned)num_varargs;_tag_fat(({struct _tuple1**_Tmp10=({struct _RegionHandle*_Tmp11=r;_region_malloc(_Tmp11,_check_times(_TmpF,sizeof(struct _tuple1*)));});({{unsigned _Tmp11=(unsigned)num_varargs;unsigned i;for(i=0;i < _Tmp11;++ i){({struct _tuple1*_Tmp12=Cyc_Toc_temp_var();_Tmp10[i]=_Tmp12;});}}0;});_Tmp10;}),sizeof(struct _tuple1*),_TmpF);});
# 1994
struct Cyc_List_List*array_args=0;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(((struct _tuple1**)vs.curr)[i],0U),0U);_TmpF->hd=_Tmp10;}),_TmpF->tl=array_args;_TmpF;});}}
s=({struct _tuple1*_TmpF=argv;void*_Tmp10=arr_type;struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_TmpF,_Tmp10,_Tmp11,s,0U);});
# 1999
es=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(es);
injectors=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(injectors);{
int i=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(es)- 1;
for(1;es != 0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=_check_null(arg->topt);
struct _tuple1*var=((struct _tuple1**)vs.curr)[i];
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_stmttmp33=(struct Cyc_Absyn_Datatypefield*)(_check_null(injectors))->hd;struct Cyc_Absyn_Datatypefield*_TmpF=_stmttmp33;void*_Tmp10;void*_Tmp11;_Tmp11=_TmpF->name;_Tmp10=_TmpF->typs;{struct _tuple1*qv=_Tmp11;struct Cyc_List_List*tqts=_Tmp10;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)(_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2013
struct _tuple1*tdn=Cyc_Toc_collapse_qvars(qv,tud->name);
struct Cyc_List_List*dles=({struct _tuple19*_Tmp12[2];({struct _tuple19*_Tmp13=({struct _fat_ptr*_Tmp14=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_Tmp14,Cyc_Toc_datatype_tag(tud,qv));});_Tmp12[0]=_Tmp13;}),({
struct _tuple19*_Tmp13=({struct _fat_ptr*_Tmp14=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_Tmp14,arg);});_Tmp12[1]=_Tmp13;});({
# 2014
(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp12,sizeof(struct _tuple19*),2));});
# 2016
s=({struct _tuple1*_Tmp12=var;void*_Tmp13=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_Tmp14=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp15->tag=29,_Tmp15->f1=tdn,_Tmp15->f2=0,_Tmp15->f3=dles,_Tmp15->f4=0;_Tmp15;}),0U);
# 2016
Cyc_Absyn_declare_stmt(_Tmp12,_Tmp13,_Tmp14,s,0U);});}}}}}}else{
# 2023
struct Cyc_List_List*array_args=0;
for(1;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(struct Cyc_Absyn_Exp*)es->hd,_TmpF->tl=array_args;_TmpF;});}{
# 2028
struct Cyc_Absyn_Exp*init=Cyc_Absyn_array_exp(({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(array_args),0U);
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}
# 2031
({void*_TmpF=Cyc_Toc_stmt_exp_r(s);e->r=_TmpF;});}}}}
# 2033
_npop_handler(0);goto _LL7;
# 1949
;_pop_region();}}case 11: _TmpA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;int b=_Tmp5;
# 2036
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_TmpC,({struct Cyc_Absyn_Exp*_TmpD[1];_TmpD[0]=e1;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_TmpD,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);})->r;e->r=_TmpB;});
goto _LL7;}}case 12: _TmpA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13: _TmpA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_List_List*ts=_Tmp7;
# 2042
Cyc_Toc_exp_to_c(nv,e1);
# 2051 "toc.cyc"
for(1;ts != 0;ts=ts->tl){
enum Cyc_Absyn_KindQual k=(Cyc_Tcutil_type_kind((void*)ts->hd))->kind;
if((int)k != 4 &&(int)k != 3){
{void*_stmttmp34=Cyc_Absyn_compress((void*)ts->hd);void*_TmpB=_stmttmp34;switch(*((int*)_TmpB)){case 2:
 goto _LLA3;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)== 18){_LLA3:
 continue;}else{goto _LLA4;}default: _LLA4: {
# 2058
void*tc=Cyc_Toc_typ_to_c(_check_null(e->topt));
({void*_TmpC=(Cyc_Toc_array_to_ptr_cast(tc,e1,0U))->r;e->r=_TmpC;});
# 2065
{void*_TmpC=tc;if(*((int*)_TmpC)== 3){if(*((int*)(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpC)->f1).elt_type)== 5){
# 2067
({void*_TmpD=Cyc_Toc_stmt_exp_r(({struct Cyc_Absyn_Exp*_TmpE=Cyc_Absyn_new_exp(e->r,e->loc);Cyc_Absyn_exp_stmt(_TmpE,e->loc);}));e->r=_TmpD;});
goto _LLA6;}else{goto _LLA9;}}else{_LLA9:
 goto _LLA6;}_LLA6:;}
# 2071
goto _LL9F;}}_LL9F:;}
# 2073
break;}}
# 2076
goto _LL7;}case 14: _TmpA=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void**t=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp7;int user_inserted=_Tmp5;enum Cyc_Absyn_Coercion coercion=_Tmp3;
# 2078
void*old_t2=_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2085
{struct _tuple34 _stmttmp35=({struct _tuple34 _TmpB;({void*_TmpC=Cyc_Absyn_compress(old_t2);_TmpB.f1=_TmpC;}),({void*_TmpC=Cyc_Absyn_compress(new_typ);_TmpB.f2=_TmpC;});_TmpB;});struct _tuple34 _TmpB=_stmttmp35;struct Cyc_Absyn_PtrInfo _TmpC;struct Cyc_Absyn_PtrInfo _TmpD;if(*((int*)_TmpB.f1)== 3)switch(*((int*)_TmpB.f2)){case 3: _TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB.f1)->f1;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB.f2)->f1;{struct Cyc_Absyn_PtrInfo p1=_TmpD;struct Cyc_Absyn_PtrInfo p2=_TmpC;
# 2087
struct Cyc_Absyn_Exp*b1=({void*_TmpE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpE,(p1.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*b2=({void*_TmpE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpE,(p2.ptr_atts).bounds);});
int zt1=Cyc_Tcutil_force_type2bool(0,(p1.ptr_atts).zero_term);
int zt2=Cyc_Tcutil_force_type2bool(0,(p2.ptr_atts).zero_term);
{struct _tuple35 _stmttmp36=({struct _tuple35 _TmpE;_TmpE.f1=b1,_TmpE.f2=b2;_TmpE;});struct _tuple35 _TmpE=_stmttmp36;if(_TmpE.f1 != 0){if(_TmpE.f2 != 0){
# 2094
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[1];_Tmp10[0]=e1;Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),1));})->r;e->r=_TmpF;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
*e=*e1;}
# 2103
goto _LLB2;}else{
# 2105
struct Cyc_Absyn_Exp*e2=_check_null(b1);
struct _tuple13 _stmttmp37=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _TmpF=_stmttmp37;int _Tmp10;unsigned _Tmp11;_Tmp11=_TmpF.f1;_Tmp10=_TmpF.f2;{unsigned i=_Tmp11;int valid=_Tmp10;
if(Cyc_Toc_is_toplevel(nv)){
# 2111
if((zt1 && !(p2.elt_tq).real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_Tmp12=e2;Cyc_Absyn_prim2_exp(2U,_Tmp12,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_Tmp12=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1))->r;e->r=_Tmp12;});}else{
if(zt1){
# 2120
struct _tuple30 _stmttmp38=Cyc_Toc_temp_var_and_exp();struct _tuple30 _Tmp12=_stmttmp38;void*_Tmp13;void*_Tmp14;_Tmp14=_Tmp12.f1;_Tmp13=_Tmp12.f2;{struct _tuple1*x=_Tmp14;struct Cyc_Absyn_Exp*x_exp=_Tmp13;
struct Cyc_Absyn_Exp*arg3;
# 2124
{void*_stmttmp39=e1->r;void*_Tmp15=_stmttmp39;void*_Tmp16;switch(*((int*)_Tmp15)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp15)->f1).Wstring_c).tag){case 8:
 arg3=e2;goto _LLC1;case 9:
 arg3=e2;goto _LLC1;default: goto _LLCA;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp15)->f1)){case 1: _Tmp16=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp15)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp16;
_Tmp16=vd;goto _LLC9;}case 4: _Tmp16=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp15)->f1)->f1;_LLC9: {struct Cyc_Absyn_Vardecl*vd=_Tmp16;
# 2129
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLCB;
arg3=e2;
goto _LLC1;}default: goto _LLCA;}default: _LLCA: _LLCB:
# 2136
 if(valid && i != 1U)
arg3=e2;else{
# 2139
arg3=({struct Cyc_Absyn_Exp*_Tmp17[2];({
struct Cyc_Absyn_Exp*_Tmp18=({void*_Tmp19=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_Tmp19,x_exp);});_Tmp17[0]=_Tmp18;}),_Tmp17[1]=e2;({struct Cyc_Absyn_Exp*_Tmp18=
# 2139
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,e1);Cyc_Toc_fncall_exp_dl(_Tmp18,_tag_fat(_Tmp17,sizeof(struct Cyc_Absyn_Exp*),2));});});}
# 2141
goto _LLC1;}_LLC1:;}
# 2145
if(!zt2 && !(p2.elt_tq).real_const)
arg3=({struct Cyc_Absyn_Exp*_Tmp15=arg3;Cyc_Absyn_prim2_exp(2U,_Tmp15,Cyc_Absyn_uint_exp(1U,0U),0U);});{
struct Cyc_Absyn_Exp*arg2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);
struct Cyc_Absyn_Exp*tg_exp=({struct Cyc_Absyn_Exp*_Tmp15[3];_Tmp15[0]=x_exp,_Tmp15[1]=arg2,_Tmp15[2]=arg3;Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_Tmp15,sizeof(struct Cyc_Absyn_Exp*),3));});
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(tg_exp,0U);
s=({struct _tuple1*_Tmp15=x;void*_Tmp16=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_Tmp17=e1;Cyc_Absyn_declare_stmt(_Tmp15,_Tmp16,_Tmp17,s,0U);});
({void*_Tmp15=Cyc_Toc_stmt_exp_r(s);e->r=_Tmp15;});}}}else{
# 2154
({void*_Tmp12=({struct Cyc_Absyn_Exp*_Tmp13[3];_Tmp13[0]=e1,({
struct Cyc_Absyn_Exp*_Tmp14=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_Tmp13[1]=_Tmp14;}),_Tmp13[2]=e2;Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_fat_e,_tag_fat(_Tmp13,sizeof(struct Cyc_Absyn_Exp*),3));});
# 2154
e->r=_Tmp12;});}}
# 2156
goto _LLB2;}}}else{if(_TmpE.f2 != 0){
# 2166 "toc.cyc"
struct Cyc_Absyn_Exp*new_e2=_check_null(b2);
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_TmpF=b2;Cyc_Absyn_add_exp(_TmpF,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2173
struct Cyc_Absyn_Exp*ptr_exp=({struct Cyc_Absyn_Exp*_TmpF[3];_TmpF[0]=e1,({
# 2175
struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_TmpF[1]=_Tmp10;}),_TmpF[2]=new_e2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__untag_fat_ptr_e,_tag_fat(_TmpF,sizeof(struct Cyc_Absyn_Exp*),3));});
# 2177
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[1];({struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_copy_exp(ptr_exp);_Tmp10[0]=_Tmp11;});Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),1));});ptr_exp->r=_TmpF;});
({void*_TmpF=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_TmpF;});
goto _LLB2;}}else{
# 2185
if((zt1 && !zt2)&& !(p2.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({(int(*)(struct _fat_ptr))Cyc_Toc_unimp;})(({const char*_TmpF="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_TmpF,sizeof(char),70U);}));
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[3];_Tmp10[0]=e1,({
struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_Tmp10[1]=_Tmp11;}),({
struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_uint_exp(1U,0U);_Tmp10[2]=_Tmp11;});Cyc_Toc_fncall_exp_r(Cyc_Toc__fat_ptr_decrease_size_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),3));});
# 2188
e->r=_TmpF;});}
# 2192
goto _LLB2;}}_LLB2:;}
# 2194
goto _LLAB;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB.f2)->f1)== 1){_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB.f1)->f1;{struct Cyc_Absyn_PtrInfo p1=_TmpD;
# 2196
{struct Cyc_Absyn_Exp*_stmttmp3A=({void*_TmpE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpE,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_TmpE=_stmttmp3A;if(_TmpE == 0){
# 2198
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_Tmp10,Cyc_Toc_curr_sp);});e1->r=_TmpF;});
e1->topt=new_typ_c;
goto _LLCC;}else{
goto _LLCC;}_LLCC:;}
# 2203
goto _LLAB;}}else{goto _LLB0;}default: goto _LLB0;}else{_LLB0:
# 2205
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
*e=*e1;
goto _LLAB;}_LLAB:;}
# 2209
goto _LL7;}case 15: _TmpA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
# 2212
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
({(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,int),int))Cyc_Toc_lvalue_assign;})(e1,0,Cyc_Toc_address_lvalue,1);
# 2218
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_cast_it_r(_TmpC,e1);});e->r=_TmpB;});}else{
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(_check_null(e1->topt))))
# 2221
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_cast_it_r(_TmpC,e1);});e->r=_TmpB;});}
goto _LL7;}case 16: _TmpA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*rgnopt=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp7;
# 2229
Cyc_Toc_exp_to_c(nv,e1);{
# 2231
void*elt_typ=Cyc_Toc_typ_to_c(_check_null(e1->topt));
{void*_stmttmp3B=Cyc_Absyn_compress(elt_typ);void*_TmpB=_stmttmp3B;void*_TmpC;if(*((int*)_TmpB)== 4){_TmpC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TmpB)->f1).elt_type;{void*t2=_TmpC;
elt_typ=Cyc_Toc_typ_to_c(t2);goto _LLD1;}}else{
goto _LLD1;}_LLD1:;}{
# 2236
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*lenvar=(unsigned)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*lenexp=(unsigned)array_len?Cyc_Absyn_var_exp(_check_null(lenvar),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_TmpB[2];_TmpB[0]=lenexp,({struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_TmpB[1]=_TmpC;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_times_e,_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),2));});else{
# 2243
mexp=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);}{
# 2246
struct Cyc_Absyn_Exp*vse=Cyc_Toc_get_varsizeexp(e1);
if(vse != 0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2250
if(rgnopt == 0 || Cyc_Flags_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2253
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=Cyc_Toc_rmalloc_exp(rgnopt,mexp);}
# 2256
({void*_TmpB=(void*)({struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct));_TmpC->tag=Cyc_Toc_NewInfo,_TmpC->f1=mexp,({void*_TmpD=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);_TmpC->f2=_TmpD;});_TmpC;});e->annot=_TmpB;});
if(Cyc_Tcutil_is_fat_ptr(old_typ))
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[3];({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_copy_exp(e);_TmpC[0]=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_TmpC[1]=_TmpD;}),
(unsigned)lenexp?_TmpC[2]=lenexp:({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_uint_exp(1U,0U);_TmpC[2]=_TmpD;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));})->r;
# 2258
e->r=_TmpB;});
# 2261
if(vse != 0)
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_TmpC,Cyc_Absyn_copy_exp(e));});e->r=_TmpB;});
if((unsigned)array_len){
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_TmpB=_check_null(lenvar);void*_TmpC=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_TmpD=array_len;Cyc_Absyn_declare_stmt(_TmpB,_TmpC,_TmpD,
Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U),0U);});
({void*_TmpB=Cyc_Toc_stmt_exp_r(s);e->r=_TmpB;});}
# 2268
goto _LL7;}}}}case 18: _TmpA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
# 2271
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17: _TmpA=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_TmpA;
({void*_TmpB=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct));_TmpC->tag=17,({void*_TmpD=Cyc_Toc_typ_to_c(t);_TmpC->f1=_TmpD;});_TmpC;});e->r=_TmpB;});goto _LL7;}case 19: _TmpA=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_TmpA;struct Cyc_List_List*fs=_Tmp7;
# 2274
void*t2=t;
struct Cyc_List_List*l=fs;
for(1;l != 0;l=l->tl){
void*_stmttmp3C=(void*)l->hd;void*_TmpB=_stmttmp3C;unsigned _TmpC;void*_TmpD;if(*((int*)_TmpB)== 0){_TmpD=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_TmpB)->f1;{struct _fat_ptr*n=_TmpD;
goto _LLD6;}}else{_TmpC=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_TmpB)->f1;{unsigned n=_TmpC;
# 2280
{void*_stmttmp3D=Cyc_Absyn_compress(t2);void*_TmpE=_stmttmp3D;void*_TmpF;union Cyc_Absyn_AggrInfo _Tmp10;switch(*((int*)_TmpE)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE)->f1)){case 20: _Tmp10=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp10;
# 2282
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=({const char*_Tmp13="struct fields must be known";_tag_fat(_Tmp13,sizeof(char),28U);});_Tmp12;});void*_Tmp12[1];_Tmp12[0]=& _Tmp11;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp12,sizeof(void*),1));});
_TmpF=(_check_null(ad->impl))->fields;goto _LLDF;}case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE)->f1)->f1).KnownDatatypefield).tag == 2){_TmpF=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypefield*tuf=_TmpF;
# 2296
if(n == 0U)
({void*_Tmp11=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp12=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp12->tag=0,_Tmp12->f1=Cyc_Toc_tag_sp;_Tmp12;}));l->hd=_Tmp11;});else{
# 2299
t2=(*({(struct _tuple23*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(tuf->typs,(int)(n - 1U))).f2;
({void*_Tmp11=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp12=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp12->tag=0,({struct _fat_ptr*_Tmp13=Cyc_Absyn_fieldname((int)n);_Tmp12->f1=_Tmp13;});_Tmp12;}));l->hd=_Tmp11;});}
# 2302
goto _LLDB;}}else{goto _LLE4;}default: goto _LLE4;}case 7: _TmpF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_TmpE)->f2;_LLDF: {struct Cyc_List_List*fields=_TmpF;
# 2287
struct Cyc_Absyn_Aggrfield*nth_field=({(struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(fields,(int)n);
({void*_Tmp11=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp12=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp12->tag=0,_Tmp12->f1=nth_field->name;_Tmp12;}));l->hd=_Tmp11;});
t2=nth_field->type;
goto _LLDB;}case 6: _TmpF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_TmpE)->f1;{struct Cyc_List_List*ts=_TmpF;
# 2292
({void*_Tmp11=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp12=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp12->tag=0,({struct _fat_ptr*_Tmp13=Cyc_Absyn_fieldname((int)(n + 1U));_Tmp12->f1=_Tmp13;});_Tmp12;}));l->hd=_Tmp11;});
t2=(*({(struct _tuple23*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(ts,(int)n)).f2;
goto _LLDB;}default: _LLE4:
# 2303
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=({const char*_Tmp13="impossible type for offsetof tuple index";_tag_fat(_Tmp13,sizeof(char),41U);});_Tmp12;});void*_Tmp12[1];_Tmp12[0]=& _Tmp11;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp12,sizeof(void*),1));});}_LLDB:;}
# 2305
goto _LLD6;}}_LLD6:;}
# 2308
({void*_TmpB=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct));_TmpC->tag=19,({void*_TmpD=Cyc_Toc_typ_to_c(t);_TmpC->f1=_TmpD;}),_TmpC->f2=fs;_TmpC;});e->r=_TmpB;});
goto _LL7;}case 21: _TmpA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*f=_Tmp7;int is_tagged=_Tmp5;int is_read=_Tmp2;
# 2311
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=e1;void*_TmpD=Cyc_Toc_typ_to_c(e1_cyc_type);void*_TmpE=e1_cyc_type;struct _fat_ptr*_TmpF=f;Cyc_Toc_check_tagged_union(_TmpC,_TmpD,_TmpE,_TmpF,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2315
e->r=_TmpB;});
# 2317
if(is_poly)
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_TmpC,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_TmpB;});
goto _LL7;}case 22: _TmpA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*f=_Tmp7;int is_tagged=_Tmp5;int is_read=_Tmp2;
# 2321
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=_check_null(e1->topt);
void*ta;
{void*_stmttmp3E=Cyc_Absyn_compress(e1typ);void*_TmpB=_stmttmp3E;struct Cyc_Absyn_PtrInfo _TmpC;if(*((int*)_TmpB)== 3){_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB)->f1;{struct Cyc_Absyn_PtrInfo p=_TmpC;
ta=p.elt_type;goto _LLE6;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF="get_ptr_typ: not a pointer!";_tag_fat(_TmpF,sizeof(char),28U);});_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LLE6:;}
# 2328
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=e1;void*_TmpD=Cyc_Toc_typ_to_c(e1typ);void*_TmpE=ta;struct _fat_ptr*_TmpF=f;Cyc_Toc_check_tagged_union(_TmpC,_TmpD,_TmpE,_TmpF,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_TmpB;});
if(is_poly && is_read)
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_TmpC,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_TmpB;});
goto _LL7;}case 20: _TmpA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
# 2336
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23: _TmpA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 2340
{void*_stmttmp3F=Cyc_Absyn_compress(_check_null(e1->topt));void*_TmpB=_stmttmp3F;void*_TmpC;if(*((int*)_TmpB)== 6){_TmpC=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_TmpB)->f1;{struct Cyc_List_List*ts=_TmpC;
# 2342
Cyc_Toc_exp_to_c(nv,e1);{
int old_lhs=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
struct _tuple13 _stmttmp40=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _TmpD=_stmttmp40;int _TmpE;unsigned _TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{unsigned i=_TmpF;int known=_TmpE;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11;_Tmp11.tag=0,_Tmp11.f1=({const char*_Tmp12="unknown tuple subscript in translation to C";_tag_fat(_Tmp12,sizeof(char),44U);});_Tmp11;});void*_Tmp11[1];_Tmp11[0]=& _Tmp10;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp11,sizeof(void*),1));});
({void*_Tmp10=({struct Cyc_Absyn_Exp*_Tmp11=e1;Cyc_Toc_aggrmember_exp_r(_Tmp11,Cyc_Absyn_fieldname((int)(i + 1U)));});e->r=_Tmp10;});
goto _LLEB;}}}}}else{
# 2352
did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_TmpD=Cyc_Toc_deref_exp_r(e1);e->r=_TmpD;});
goto _LLEB;}}_LLEB:;}
# 2358
goto _LL7;}case 24: _TmpA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_TmpA;
# 2360
if(!Cyc_Toc_is_toplevel(nv)){
# 2362
void*strct_typ=Cyc_Toc_add_tuple_type(({(struct Cyc_List_List*(*)(struct _tuple23*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Toc_tup_to_c,es));
void*_TmpB=strct_typ;union Cyc_Absyn_AggrInfo _TmpC;if(*((int*)_TmpB)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)== 20){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f2 == 0){_TmpC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)->f1;{union Cyc_Absyn_AggrInfo aggrinfo=_TmpC;
# 2365
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_TmpD=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpE=({struct _fat_ptr*_TmpF=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_TmpF,(struct Cyc_Absyn_Exp*)es->hd);});_TmpD->hd=_TmpE;}),_TmpD->tl=dles;_TmpD;});}}
# 2371
({void*_TmpD=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_TmpE->tag=29,_TmpE->f1=sd->name,_TmpE->f2=0,({struct Cyc_List_List*_TmpF=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(dles);_TmpE->f3=_TmpF;}),_TmpE->f4=sd;_TmpE;});e->r=_TmpD;});
e->topt=strct_typ;
goto _LLF3;}}else{goto _LLF6;}}else{goto _LLF6;}}else{_LLF6:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF="tuple type not an aggregate";_tag_fat(_TmpF,sizeof(char),28U);});_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LLF3:;}else{
# 2379
struct Cyc_List_List*dles=0;
for(1;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpC=({struct _tuple19*_TmpD=_cycalloc(sizeof(struct _tuple19));_TmpD->f1=0,_TmpD->f2=(struct Cyc_Absyn_Exp*)es->hd;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=dles;_TmpB;});}
# 2384
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(dles));e->r=_TmpB;});}
# 2386
goto _LL7;}case 26: _TmpA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles0=_TmpA;
# 2392
{struct Cyc_List_List*dles=dles0;for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_TmpB;});
goto _LL7;}case 27: _TmpA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;int iszeroterm=_Tmp5;
# 2400
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _stmttmp41=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple13 _TmpB=_stmttmp41;int _TmpC;unsigned _TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{unsigned sz=_TmpD;int known=_TmpC;
void*elt_typ=Cyc_Toc_typ_to_c(_check_null(e2->topt));
struct Cyc_List_List*es=0;
# 2407
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=({const char*_Tmp10="cannot determine value of constant";_tag_fat(_Tmp10,sizeof(char),35U);});_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_err2(e1->loc,_tag_fat(_TmpF,sizeof(void*),1));});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpF=({struct _tuple19*_Tmp10=_cycalloc(sizeof(struct _tuple19));_Tmp10->f1=0,_Tmp10->f2=e2;_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=es;_TmpE;});}}
# 2413
if(iszeroterm){
struct Cyc_Absyn_Exp*ezero=({void*_TmpE=elt_typ;Cyc_Toc_cast_it(_TmpE,Cyc_Absyn_uint_exp(0U,0U));});
es=({struct Cyc_List_List*(*_TmpE)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_TmpF=es;_TmpE(_TmpF,({struct Cyc_List_List*_Tmp10=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_Tmp11=({struct _tuple19*_Tmp12=_cycalloc(sizeof(struct _tuple19));_Tmp12->f1=0,_Tmp12->f2=ezero;_Tmp12;});_Tmp10->hd=_Tmp11;}),_Tmp10->tl=0;_Tmp10;}));});}}
# 2418
({void*_TmpE=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_TmpE;});}}
# 2420
goto _LL7;}case 28: _TmpA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;void*t1=_Tmp7;int iszeroterm=_Tmp5;
# 2427
if(Cyc_Toc_is_toplevel(nv))
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_TmpB;});else{
# 2430
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 30: _TmpA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*st=_TmpA;struct Cyc_List_List*dles=_Tmp7;
# 2434
{struct Cyc_List_List*dles2=dles;for(0;dles2 != 0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles2->hd)).f2);}}{
void*strct_typ=Cyc_Toc_typ_to_c(_check_null(e->topt));
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_stmttmp42=Cyc_Absyn_compress(strct_typ);void*_TmpB=_stmttmp42;union Cyc_Absyn_AggrInfo _TmpC;if(*((int*)_TmpB)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)== 20){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f2 == 0){_TmpC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)->f1;{union Cyc_Absyn_AggrInfo aggrinfo=_TmpC;
# 2441
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_TmpD=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_TmpE->tag=29,_TmpE->f1=sd->name,_TmpE->f2=0,_TmpE->f3=dles,_TmpE->f4=sd;_TmpE;});e->r=_TmpD;});
e->topt=strct_typ;
goto _LLFB;}}else{goto _LLFE;}}else{goto _LLFE;}}else{_LLFE:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF="anonStruct type not an aggregate";_tag_fat(_TmpF,sizeof(char),33U);});_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LLFB:;}else{
# 2448
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_TmpB;});}
goto _LL7;}}case 29: _TmpA=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct _tuple1**tdn=_TmpA;struct Cyc_List_List*exist_ts=_Tmp7;struct Cyc_List_List*dles=_Tmp6;struct Cyc_Absyn_Aggrdecl*sd=_Tmp4;
# 2454
if(sd == 0 || sd->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_TmpD,sizeof(char),58U);});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpC,sizeof(void*),1));});{
void*new_typ=Cyc_Toc_typ_to_c(old_typ);
{void*_stmttmp43=Cyc_Absyn_compress(new_typ);void*_TmpB=_stmttmp43;union Cyc_Absyn_AggrInfo _TmpC;if(*((int*)_TmpB)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)== 20){_TmpC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_TmpC;
({struct _tuple1*_TmpD=(Cyc_Absyn_aggr_kinded_name(info)).f2;*tdn=_TmpD;});goto _LL100;}}else{goto _LL103;}}else{_LL103:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_TmpF,sizeof(char),44U);});_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LL100:;}{
# 2465
struct Cyc_List_List*typ_fields=(_check_null(sd->impl))->fields;
if(typ_fields == 0)goto _LL7;
for(1;(_check_null(typ_fields))->tl != 0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*last_typ_field=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;
struct Cyc_List_List*fields=({(struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators;})(Cyc_Core_heap_region,e->loc,dles,sd->kind,(
_check_null(sd->impl))->fields);
if(!Cyc_Toc_is_toplevel(nv)){
void*_stmttmp44=Cyc_Absyn_compress(old_typ);void*_TmpB=_stmttmp44;void*_TmpC;if(*((int*)_TmpB)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)== 20){_TmpC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f2;{struct Cyc_List_List*param_ts=_TmpC;
# 2476
{struct Cyc_List_List*fields2=fields;for(0;fields2 != 0;fields2=fields2->tl){
struct _tuple36*_stmttmp45=(struct _tuple36*)fields2->hd;struct _tuple36*_TmpD=_stmttmp45;void*_TmpE;void*_TmpF;_TmpF=_TmpD->f1;_TmpE=_TmpD->f2;{struct Cyc_Absyn_Aggrfield*aggrfield=_TmpF;struct Cyc_Absyn_Exp*fieldexp=_TmpE;
void*old_field_typ=fieldexp->topt;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2481
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(_check_null(fieldexp->topt)))
({void*_Tmp10=({void*_Tmp11=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_Tmp11,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2483
fieldexp->r=_Tmp10;});
# 2486
if((_check_null(sd->impl))->tagged){
struct _fat_ptr*n=aggrfield->name;
struct Cyc_Absyn_Exp*tag_exp=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);
struct _tuple19*tag_dle=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);
struct _tuple19*val_dle=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);
struct _tuple1*s=({struct _tuple1*_Tmp10=_cycalloc(sizeof(struct _tuple1));({union Cyc_Absyn_Nmspace _Tmp11=Cyc_Absyn_Abs_n(0,1);_Tmp10->f1=_Tmp11;}),({
struct _fat_ptr*_Tmp11=({struct _fat_ptr*_Tmp12=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp13=({struct Cyc_String_pa_PrintArg_struct _Tmp14=({struct Cyc_String_pa_PrintArg_struct _Tmp15;_Tmp15.tag=0,_Tmp15.f1=*(*sd->name).f2;_Tmp15;});struct Cyc_String_pa_PrintArg_struct _Tmp15=({struct Cyc_String_pa_PrintArg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=*n;_Tmp16;});void*_Tmp16[2];_Tmp16[0]=& _Tmp14,_Tmp16[1]=& _Tmp15;Cyc_aprintf(({const char*_Tmp17="_union_%s_%s";_tag_fat(_Tmp17,sizeof(char),13U);}),_tag_fat(_Tmp16,sizeof(void*),2));});*_Tmp12=_Tmp13;});_Tmp12;});_Tmp10->f2=_Tmp11;});_Tmp10;});
# 2494
struct _tuple19*u_dle=({
struct _fat_ptr*_Tmp10=n;Cyc_Toc_make_field(_Tmp10,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp11->tag=29,_Tmp11->f1=s,_Tmp11->f2=0,({struct Cyc_List_List*_Tmp12=({struct _tuple19*_Tmp13[2];_Tmp13[0]=tag_dle,_Tmp13[1]=val_dle;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp13,sizeof(struct _tuple19*),2));});_Tmp11->f3=_Tmp12;}),_Tmp11->f4=0;_Tmp11;}),0U));});
# 2497
({void*_Tmp10=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp11->tag=29,_Tmp11->f1=*tdn,_Tmp11->f2=0,({struct Cyc_List_List*_Tmp12=({struct _tuple19*_Tmp13[1];_Tmp13[0]=u_dle;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp13,sizeof(struct _tuple19*),1));});_Tmp11->f3=_Tmp12;}),_Tmp11->f4=sd;_Tmp11;});e->r=_Tmp10;});}
# 2503
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field == aggrfield){
struct Cyc_List_List*inst=({struct Cyc_List_List*(*_Tmp10)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_Tmp11=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip;})((_check_null(sd->impl))->exist_vars,exist_ts);_Tmp10(_Tmp11,
({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip;})(sd->tvs,param_ts));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*fs=(_check_null(sd->impl))->fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*old_f=(struct Cyc_Absyn_Aggrfield*)fs->hd;
void*old_ftyp=Cyc_Tcutil_substitute(inst,old_f->type);
struct Cyc_Absyn_Aggrfield*new_f=({struct Cyc_Absyn_Aggrfield*_Tmp10=old_f;Cyc_Toc_aggrfield_to_c(_Tmp10,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
new_fields=({struct Cyc_List_List*_Tmp10=_cycalloc(sizeof(struct Cyc_List_List));_Tmp10->hd=new_f,_Tmp10->tl=new_fields;_Tmp10;});
# 2518
if(fs->tl == 0){
{void*_stmttmp46=Cyc_Absyn_compress(new_f->type);void*_Tmp10=_stmttmp46;struct Cyc_Absyn_ArrayInfo _Tmp11;if(*((int*)_Tmp10)== 4){_Tmp11=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp10)->f1;{struct Cyc_Absyn_ArrayInfo ai=_Tmp11;
# 2521
if(!Cyc_Evexp_c_can_eval(_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo ai2=ai;
({struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_Tmp12;});
({void*_Tmp12=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_Tmp13->tag=4,_Tmp13->f1=ai2;_Tmp13;});new_f->type=_Tmp12;});}
# 2526
goto _LL10D;}}else{
# 2530
if(fieldexp->topt == 0)
goto _LL10D;
{void*_stmttmp47=Cyc_Absyn_compress(_check_null(fieldexp->topt));void*_Tmp12=_stmttmp47;if(*((int*)_Tmp12)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp12)->f1)== 20){
# 2534
new_f->type=_check_null(fieldexp->topt);goto _LL112;}else{goto _LL115;}}else{_LL115:
 goto _LL112;}_LL112:;}
# 2537
goto _LL10D;}_LL10D:;}
# 2541
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_Tmp10=({struct Cyc_List_List*_Tmp11=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp12=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_Tmp13->tag=6,_Tmp13->f1=0;_Tmp13;});_Tmp11->hd=_Tmp12;}),_Tmp11->tl=new_f->attributes;_Tmp11;});new_f->attributes=_Tmp10;});}}}
# 2547
sd=({struct _fat_ptr*_Tmp10=({struct _fat_ptr*_Tmp11=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp12=({struct Cyc_Int_pa_PrintArg_struct _Tmp13=({struct Cyc_Int_pa_PrintArg_struct _Tmp14;_Tmp14.tag=1,_Tmp14.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_Tmp14;});void*_Tmp14[1];_Tmp14[0]=& _Tmp13;Cyc_aprintf(({const char*_Tmp15="_genStruct%d";_tag_fat(_Tmp15,sizeof(char),13U);}),_tag_fat(_Tmp14,sizeof(void*),1));});*_Tmp11=_Tmp12;});_Tmp11;});Cyc_Toc_make_c_struct_defn(_Tmp10,
# 2549
({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2553
({void*_Tmp10=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp11=sd;_Tmp11;})),0);e->topt=_Tmp10;});}}}}
# 2556
goto _LL105;}}else{goto _LL108;}}else{_LL108:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_TmpF,sizeof(char),39U);});_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LL105:;}else{
# 2565
if((_check_null(sd->impl))->tagged){
# 2567
struct _tuple36*_stmttmp48=(struct _tuple36*)(_check_null(fields))->hd;struct _tuple36*_TmpB=_stmttmp48;void*_TmpC;void*_TmpD;_TmpD=_TmpB->f1;_TmpC=_TmpB->f2;{struct Cyc_Absyn_Aggrfield*field=_TmpD;struct Cyc_Absyn_Exp*fieldexp=_TmpC;
void*fieldexp_type=_check_null(fieldexp->topt);
void*fieldtyp=field->type;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_TmpE=({
void*_TmpF=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_TmpF,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2573
fieldexp->r=_TmpE;});{
# 2576
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*newdles=({struct _tuple19*_TmpE[2];({struct _tuple19*_TmpF=({struct _tuple19*_Tmp10=_cycalloc(sizeof(struct _tuple19));_Tmp10->f1=0,_Tmp10->f2=field_tag_exp;_Tmp10;});_TmpE[0]=_TmpF;}),({struct _tuple19*_TmpF=({struct _tuple19*_Tmp10=_cycalloc(sizeof(struct _tuple19));_Tmp10->f1=0,_Tmp10->f2=fieldexp;_Tmp10;});_TmpE[1]=_TmpF;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_TmpE,sizeof(struct _tuple19*),2));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_TmpE=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_TmpF[1];({struct _tuple19*_Tmp10=Cyc_Toc_make_field(field->name,umem);_TmpF[0]=_Tmp10;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_TmpF,sizeof(struct _tuple19*),1));}));e->r=_TmpE;});}}}else{
# 2583
struct Cyc_List_List*newdles=0;
struct Cyc_List_List*sdfields=(_check_null(sd->impl))->fields;
for(1;sdfields != 0;sdfields=sdfields->tl){
struct Cyc_List_List*fields2=fields;for(0;fields2 != 0;fields2=fields2->tl){
if((*((struct _tuple36*)fields2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple36*_stmttmp49=(struct _tuple36*)fields2->hd;struct _tuple36*_TmpB=_stmttmp49;void*_TmpC;void*_TmpD;_TmpD=_TmpB->f1;_TmpC=_TmpB->f2;{struct Cyc_Absyn_Aggrfield*field=_TmpD;struct Cyc_Absyn_Exp*fieldexp=_TmpC;
void*fieldtyp=Cyc_Toc_typ_to_c(field->type);
void*fieldexp_typ=Cyc_Toc_typ_to_c(_check_null(fieldexp->topt));
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2593
if(!Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2595
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_TmpE=fieldtyp;Cyc_Toc_cast_it(_TmpE,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpF=({struct _tuple19*_Tmp10=_cycalloc(sizeof(struct _tuple19));_Tmp10->f1=0,_Tmp10->f2=fieldexp;_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=newdles;_TmpE;});
break;}}}}
# 2602
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(newdles));e->r=_TmpB;});}}
# 2605
goto _LL7;}}}}case 31: _TmpA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_TmpA;struct Cyc_Absyn_Datatypedecl*tud=_Tmp7;struct Cyc_Absyn_Datatypefield*tuf=_Tmp6;
# 2609
struct Cyc_List_List*tqts=tuf->typs;
{struct Cyc_List_List*es2=es;for(0;es2 != 0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*cur_e_typ=_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)(_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_TmpB=({void*_TmpC=field_typ;Cyc_Toc_cast_it(_TmpC,Cyc_Absyn_copy_exp(cur_e));})->r;cur_e->r=_TmpB;});}}{
# 2620
struct Cyc_Absyn_Exp*tag_exp=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2623
if(!Cyc_Toc_is_toplevel(nv)){
# 2625
struct Cyc_List_List*dles=0;
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpC=({struct _fat_ptr*_TmpD=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_TmpD,(struct Cyc_Absyn_Exp*)es->hd);});_TmpB->hd=_TmpC;}),_TmpB->tl=dles;_TmpB;});}}{
# 2631
struct _tuple19*tag_dle=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);
({void*_TmpB=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_TmpC->tag=29,({struct _tuple1*_TmpD=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_TmpC->f1=_TmpD;}),_TmpC->f2=0,({
struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));_TmpE->hd=tag_dle,({struct Cyc_List_List*_TmpF=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(dles);_TmpE->tl=_TmpF;});_TmpE;});_TmpC->f3=_TmpD;}),_TmpC->f4=0;_TmpC;});
# 2632
e->r=_TmpB;});}}else{
# 2637
struct Cyc_List_List*dles=0;
for(1;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpC=({struct _tuple19*_TmpD=_cycalloc(sizeof(struct _tuple19));_TmpD->f1=0,_TmpD->f2=(struct Cyc_Absyn_Exp*)es->hd;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=dles;_TmpB;});}
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpD=({struct _tuple19*_TmpE=_cycalloc(sizeof(struct _tuple19));_TmpE->f1=0,_TmpE->f2=tag_exp;_TmpE;});_TmpC->hd=_TmpD;}),({
struct Cyc_List_List*_TmpD=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(dles);_TmpC->tl=_TmpD;});_TmpC;}));
# 2640
e->r=_TmpB;});}
# 2643
goto _LL7;}}case 32:
# 2645
 goto _LL51;case 33: _LL51:
 goto _LL7;case 34: _Tmp1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).mknd;_TmpA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).rgn;_Tmp7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).elt_type;_Tmp6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).num_elts;_Tmp5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).fat_result;_Tmp2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).inline_call;{enum Cyc_Absyn_MallocKind mknd=_Tmp1;struct Cyc_Absyn_Exp*rgnopt=_TmpA;void**topt=_Tmp7;struct Cyc_Absyn_Exp*num_elts=_Tmp6;int is_fat=_Tmp5;int inline_call=_Tmp2;
# 2649
void*t_c=Cyc_Toc_typ_to_c(*_check_null(topt));
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;
if(is_fat){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2658
if((int)mknd == 1){
if(rgnopt != 0 && !Cyc_Flags_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_TmpB[3];_TmpB[0]=rgnopt,({
struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_sizeoftype_exp(t_c,0U);_TmpB[1]=_TmpC;}),_TmpB[2]=num_elts2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_calloc_e,_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),3));});}else{
# 2664
pexp=({void*_TmpB=*topt;struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_TmpB,_TmpC,num_elts2);});}}else{
# 2666
if(rgnopt != 0 && !Cyc_Flags_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);{
struct Cyc_Absyn_Exp*(*fn_e)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=(int)mknd == 2?Cyc_Toc_rvmalloc_exp:((inline_call?Cyc_Toc_rmalloc_inline_exp: Cyc_Toc_rmalloc_exp));
pexp=fn_e(rgnopt,num_elts2);}}else{
# 2671
pexp=Cyc_Toc_malloc_exp(*topt,num_elts2);}}
# 2673
if(is_fat){
struct Cyc_Absyn_Exp*elt_sz=(int)mknd == 1?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_TmpB[3];_TmpB[0]=pexp,_TmpB[1]=elt_sz,_TmpB[2]=num_elts2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),3));});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_TmpB=_check_null(x);void*_TmpC=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_TmpD=num_elts;Cyc_Absyn_declare_stmt(_TmpB,_TmpC,_TmpD,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_TmpB=Cyc_Toc_stmt_exp_r(s);e->r=_TmpB;});}else{
# 2680
e->r=pexp->r;}
goto _LL7;}}case 35: _TmpA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 2690
void*e1_old_typ=_check_null(e1->topt);
void*e2_old_typ=_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_TmpD,sizeof(char),57U);});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpC,sizeof(void*),1));});{
# 2698
unsigned loc=e->loc;
struct _tuple1*v1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*v1e=Cyc_Absyn_var_exp(v1,loc);v1e->topt=e1_old_typ;{
struct _tuple1*v2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*v2e=Cyc_Absyn_var_exp(v2,loc);v2e->topt=e2_old_typ;{
# 2704
struct Cyc_Absyn_Exp*s1e=({struct Cyc_Absyn_Exp*_TmpB=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_TmpC=v2e;Cyc_Absyn_assign_exp(_TmpB,_TmpC,loc);});s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_exp_stmt(s1e,loc);
struct Cyc_Absyn_Exp*s2e=({struct Cyc_Absyn_Exp*_TmpB=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_TmpC=v1e;Cyc_Absyn_assign_exp(_TmpB,_TmpC,loc);});s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_exp_stmt(s2e,loc);
# 2709
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_TmpB=v1;void*_TmpC=e1_old_typ;struct Cyc_Absyn_Exp*_TmpD=e1;struct Cyc_Absyn_Stmt*_TmpE=({
struct _tuple1*_TmpF=v2;void*_Tmp10=e2_old_typ;struct Cyc_Absyn_Exp*_Tmp11=e2;struct Cyc_Absyn_Stmt*_Tmp12=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2710
Cyc_Absyn_declare_stmt(_TmpF,_Tmp10,_Tmp11,_Tmp12,loc);});
# 2709
Cyc_Absyn_declare_stmt(_TmpB,_TmpC,_TmpD,_TmpE,loc);});
# 2712
Cyc_Toc_stmt_to_c(nv,s);
({void*_TmpB=Cyc_Toc_stmt_exp_r(s);e->r=_TmpB;});
goto _LL7;}}}}}}case 38: _TmpA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*f=_Tmp7;
# 2717
void*e1_typ=Cyc_Absyn_compress(_check_null(e1->topt));
Cyc_Toc_exp_to_c(nv,e1);
{void*_TmpB=e1_typ;void*_TmpC;if(*((int*)_TmpB)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)->f1).KnownAggr).tag == 2){_TmpC=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_TmpC;
# 2721
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
struct Cyc_Absyn_Exp*e1_f=Cyc_Toc_member_exp(e1,f,0U);
struct Cyc_Absyn_Exp*e1_f_tag=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);
({void*_TmpD=(Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U))->r;e->r=_TmpD;});
goto _LL11D;}}else{goto _LL120;}}else{goto _LL120;}}else{_LL120:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF="non-aggregate type in tagcheck";_tag_fat(_TmpF,sizeof(char),31U);});_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LL11D:;}
# 2728
goto _LL7;}case 37: _TmpA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_TmpA;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 36:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD="UnresolvedMem";_tag_fat(_TmpD,sizeof(char),14U);});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpC,sizeof(void*),1));});case 25:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD="compoundlit";_tag_fat(_TmpD,sizeof(char),12U);});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpC,sizeof(void*),1));});case 39:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD="valueof(-)";_tag_fat(_TmpD,sizeof(char),11U);});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpC,sizeof(void*),1));});default: _TmpA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp7=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_List_List*o=_TmpA;struct Cyc_List_List*i=_Tmp7;
# 2734
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL7;}}_LL7:;}
# 2738
if(!did_inserted_checks)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="Toc did not examine an inserted check: ";_tag_fat(_Tmp2,sizeof(char),40U);});_Tmp1;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=e;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});}}struct _tuple37{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2767 "toc.cyc"
static struct _tuple37*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2769
return({struct _tuple37*_Tmp0=_region_malloc(r,sizeof(struct _tuple37));_Tmp0->f1=0,({struct _fat_ptr*_Tmp1=Cyc_Toc_fresh_label();_Tmp0->f2=_Tmp1;}),({struct _fat_ptr*_Tmp1=Cyc_Toc_fresh_label();_Tmp0->f3=_Tmp1;}),_Tmp0->f4=sc;_Tmp0;});}
# 2775
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(1;ps != 0;ps=(_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*p=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 2781
if((int)(((p->orig_pat).pattern).tag == 1)){
void*t=_check_null(({union Cyc_Tcpat_PatOrWhere _Tmp0=p->orig_pat;if((_Tmp0.pattern).tag != 1)_throw_match();(_Tmp0.pattern).val;})->topt);
void*t2=Cyc_Absyn_compress(Cyc_Toc_typ_to_c_array(t));
void*_Tmp0=t2;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 0:
# 2786
 goto _LL4;case 20: _LL4:
 goto _LL6;default: goto _LL7;}case 7: _LL6:
 goto _LL0;default: _LL7:
 v=({void*_Tmp1=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_Tmp1,v);});goto _LL0;}_LL0:;}{
# 2792
void*_stmttmp4A=p->access;void*_Tmp0=_stmttmp4A;void*_Tmp1;int _Tmp2;unsigned _Tmp3;switch(*((int*)_Tmp0)){case 0:
# 2796
 goto _LL9;case 1:
# 2801
 if(ps->tl != 0){
void*_stmttmp4B=((struct Cyc_Tcpat_PathNode*)(_check_null(ps->tl))->hd)->access;void*_Tmp4=_stmttmp4B;unsigned _Tmp5;void*_Tmp6;void*_Tmp7;if(*((int*)_Tmp4)== 3){_Tmp7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp4)->f1;_Tmp6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp4)->f2;_Tmp5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp4)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp7;struct Cyc_Absyn_Datatypefield*tuf=_Tmp6;unsigned i=_Tmp5;
# 2804
ps=ps->tl;
v=({void*_Tmp8=({void*_Tmp9=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_Tmp9,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_Tmp8,v);});
v=({struct Cyc_Absyn_Exp*_Tmp8=v;Cyc_Absyn_aggrarrow_exp(_Tmp8,Cyc_Absyn_fieldname((int)(i + 1U)),0U);});
continue;}}else{
goto _LL14;}_LL14:;}
# 2810
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3: _Tmp3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0)->f3;{unsigned i=_Tmp3;
_Tmp3=i;goto _LL11;}case 2: _Tmp3=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_Tmp0)->f1;_LL11: {unsigned i=_Tmp3;
v=({struct Cyc_Absyn_Exp*_Tmp4=v;Cyc_Toc_member_exp(_Tmp4,Cyc_Absyn_fieldname((int)(i + 1U)),0U);});goto _LL9;}default: _Tmp2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0)->f2;{int tagged=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 2815
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2821
return v;}
# 2826
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_Tmp0=t;unsigned _Tmp1;int _Tmp2;struct _fat_ptr _Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: _Tmp5=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
return e == 0?v: e;}case 1:
 return({struct Cyc_Absyn_Exp*_Tmp6=v;Cyc_Absyn_eq_exp(_Tmp6,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2:
 return({struct Cyc_Absyn_Exp*_Tmp6=v;Cyc_Absyn_neq_exp(_Tmp6,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3: _Tmp5=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp5;struct Cyc_Absyn_Enumfield*ef=_Tmp4;
return({struct Cyc_Absyn_Exp*_Tmp6=v;Cyc_Absyn_eq_exp(_Tmp6,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_Tmp7->tag=32,_Tmp7->f1=ed,_Tmp7->f2=ef;_Tmp7;}),0U),0U);});}case 4: _Tmp5=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_Tmp0)->f2;{void*t=_Tmp5;struct Cyc_Absyn_Enumfield*ef=_Tmp4;
return({struct Cyc_Absyn_Exp*_Tmp6=v;Cyc_Absyn_eq_exp(_Tmp6,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_Tmp7->tag=33,_Tmp7->f1=t,_Tmp7->f2=ef;_Tmp7;}),0U),0U);});}case 5: _Tmp3=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_Tmp0)->f2;{struct _fat_ptr s=_Tmp3;int i=_Tmp2;
return({struct Cyc_Absyn_Exp*_Tmp6=v;Cyc_Absyn_eq_exp(_Tmp6,Cyc_Absyn_float_exp(s,i,0U),0U);});}case 6: _Tmp1=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_Tmp0)->f1;{unsigned i=_Tmp1;
return({struct Cyc_Absyn_Exp*_Tmp6=v;Cyc_Absyn_eq_exp(_Tmp6,Cyc_Absyn_signed_int_exp((int)i,0U),0U);});}case 7: _Tmp2=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_Tmp0)->f3;{int i=_Tmp2;struct Cyc_Absyn_Datatypedecl*tud=_Tmp5;struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;
# 2838
LOOP1: {
void*_stmttmp4C=v->r;void*_Tmp6=_stmttmp4C;void*_Tmp7;switch(*((int*)_Tmp6)){case 14: _Tmp7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp6)->f2;{struct Cyc_Absyn_Exp*e=_Tmp7;
v=e;goto LOOP1;}case 20: _Tmp7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp6)->f1;{struct Cyc_Absyn_Exp*e=_Tmp7;
v=e;goto _LL15;}default:
 goto _LL15;}_LL15:;}
# 2848
v=({void*_Tmp6=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_Tmp6,v);});
return({struct Cyc_Absyn_Exp*_Tmp6=Cyc_Absyn_deref_exp(v,0U);Cyc_Absyn_eq_exp(_Tmp6,Cyc_Absyn_signed_int_exp(i,0U),0U);});}case 8: _Tmp5=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_Tmp0)->f2;{struct _fat_ptr*f=_Tmp5;int i=_Tmp2;
# 2851
return({struct Cyc_Absyn_Exp*_Tmp6=({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_Tmp7,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_Tmp6,
Cyc_Absyn_signed_int_exp(i,0U),0U);});}default: _Tmp5=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp5;struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;
# 2855
LOOP2: {
void*_stmttmp4D=v->r;void*_Tmp6=_stmttmp4D;void*_Tmp7;switch(*((int*)_Tmp6)){case 14: _Tmp7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp6)->f2;{struct Cyc_Absyn_Exp*e=_Tmp7;
v=e;goto LOOP2;}case 20: _Tmp7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp6)->f1;{struct Cyc_Absyn_Exp*e=_Tmp7;
v=e;goto _LL1C;}default:
 goto _LL1C;}_LL1C:;}
# 2862
v=({void*_Tmp6=({void*_Tmp7=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_Tmp7,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_Tmp6,v);});
return({struct Cyc_Absyn_Exp*_Tmp6=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_Tmp6,Cyc_Absyn_var_exp(tuf->name,0U),0U);});}};}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2874
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple38{int f1;void*f2;};
# 2881
static struct _tuple38 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2883
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(1;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple34 _stmttmp4E=*((struct _tuple34*)ss->hd);struct _tuple34 _Tmp0=_stmttmp4E;void*_Tmp1;_Tmp1=_Tmp0.f1;{void*ptest=_Tmp1;
void*_Tmp2=ptest;void*_Tmp3;switch(*((int*)_Tmp2)){case 3:
# 2889
 goto _LL7;case 4: _LL7:
 goto _LL9;case 6: _LL9:
 continue;case 8: _Tmp3=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_Tmp2)->f1;{struct _fat_ptr*f=_Tmp3;
# 2893
if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct));_Tmp4->tag=3,_Tmp4->f1=f;_Tmp4;});
continue;}case 7:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0: _Tmp3=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_Tmp2)->f1;if((struct Cyc_Absyn_Exp*)_Tmp3 != 0){struct Cyc_Absyn_Exp*e=_Tmp3;
# 2898
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Toc_WhereTest_Toc_TestKind_struct));_Tmp4->tag=2,_Tmp4->f1=e;_Tmp4;});return({struct _tuple38 _Tmp4;_Tmp4.f1=0,_Tmp4.f2=k;_Tmp4;});}else{
goto _LL13;}case 1: _LL13:
 goto _LL15;case 2: _LL15:
 goto _LL17;case 5: _LL17:
 goto _LL19;default: _LL19:
 return({struct _tuple38 _Tmp4;_Tmp4.f1=0,_Tmp4.f2=k;_Tmp4;});};}}
# 2906
return({struct _tuple38 _Tmp0;_Tmp0.f1=c,_Tmp0.f2=k;_Tmp0;});}
# 2911
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_Tmp0=p;int _Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 3: _Tmp4=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;struct Cyc_Absyn_Enumfield*ef=_Tmp3;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_Tmp5->tag=32,_Tmp5->f1=ed,_Tmp5->f2=ef;_Tmp5;}),0U);goto _LL0;}case 4: _Tmp4=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_Tmp0)->f2;{void*t=_Tmp4;struct Cyc_Absyn_Enumfield*ef=_Tmp3;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_Tmp5->tag=33,_Tmp5->f1=t,_Tmp5->f2=ef;_Tmp5;}),0U);goto _LL0;}case 6: _Tmp2=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_Tmp0)->f1;{unsigned i=_Tmp2;
_Tmp1=(int)i;goto _LL8;}case 7: _Tmp1=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_Tmp0)->f1;_LL8: {int i=_Tmp1;
_Tmp1=i;goto _LLA;}case 8: _Tmp1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_Tmp0)->f2;_LLA: {int i=_Tmp1;
e=Cyc_Absyn_signed_int_exp(i,0U);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="compile_pat_test_as_case!";_tag_fat(_Tmp7,sizeof(char),26U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp6,sizeof(void*),1));});}_LL0:;}
# 2921
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct));_Tmp0->tag=17,_Tmp0->f1=e;_Tmp0;}),0U);}
# 2925
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple39{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;void*f3;int f4;};
# 2937
int Cyc_Toc_vd_in_use(struct Cyc_Absyn_Vardecl*other_vd,struct Cyc_List_List**newdecls){
{struct Cyc_List_List*ds=*newdecls;for(0;ds != 0;ds=ds->tl){
# 2941
if(other_vd == (*((struct _tuple39*)ds->hd)).f2)return 1;}}
return 0;}
# 2945
static int Cyc_Toc_is_usuable_type(void*ctype,void*other){
void*_stmttmp4F=Cyc_Absyn_compress(ctype);void*_Tmp0=_stmttmp4F;struct Cyc_Absyn_Tqual _Tmp1;if(*((int*)_Tmp0)== 3){_Tmp1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_tq;{struct Cyc_Absyn_Tqual tq=_Tmp1;
# 2948
if(Cyc_Absyn_equal_tqual(tq,Cyc_Toc_mt_tq)&& Cyc_Toc_is_void_star(other))
return 1;
goto _LL4;}}else{_LL4:
 return Cyc_Tcutil_typecmp(ctype,other)== 0;};}
# 2954
static int Cyc_Toc_can_use_voidstar(void*ctype){
void*_stmttmp50=Cyc_Absyn_compress(ctype);void*_Tmp0=_stmttmp50;struct Cyc_Absyn_Tqual _Tmp1;if(*((int*)_Tmp0)== 3){_Tmp1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_tq;{struct Cyc_Absyn_Tqual tq=_Tmp1;
return Cyc_Absyn_equal_tqual(tq,Cyc_Toc_mt_tq);}}else{
return 0;};}struct _tuple40{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple41{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 2964
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**newdecls,struct Cyc_List_List**prevdecls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 2968
void*t=_check_null(p->topt);
void*_stmttmp51=p->r;void*_Tmp0=_stmttmp51;union Cyc_Absyn_AggrInfo _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 2: _Tmp4=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 2971
struct Cyc_Absyn_Pat*p2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
p2->topt=p->topt;
_Tmp4=vd;_Tmp3=p2;goto _LL4;}case 1: _Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp3;
# 2976
void*ctype=Cyc_Toc_typ_to_c_array(vd->type);
# 2978
{struct Cyc_List_List*pds=*prevdecls;for(0;pds != 0;pds=pds->tl){
struct _tuple40 _stmttmp52=*((struct _tuple40*)pds->hd);struct _tuple40 _Tmp5=_stmttmp52;void*_Tmp6;_Tmp6=_Tmp5.f2;{struct Cyc_Absyn_Vardecl*other_vd=_Tmp6;
# 2982
if(Cyc_Toc_is_usuable_type(ctype,other_vd->type)&& !
Cyc_Toc_vd_in_use(other_vd,newdecls)){
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp9=({struct _tuple39*_TmpA=_region_malloc(rgn,sizeof(struct _tuple39));_TmpA->f1=vd,_TmpA->f2=other_vd,_TmpA->f3=ctype,_TmpA->f4=0;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=*newdecls;_Tmp8;});*newdecls=_Tmp7;});{
# 2986
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp8->tag=4,_Tmp8->f1=other_vd;_Tmp8;}),0U);Cyc_Absyn_assign_stmt(_Tmp7,
Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_Tmp7=s;Cyc_Toc_seq_stmt_opt(_Tmp7,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));});}}}}}{
# 2994
struct Cyc_Absyn_Vardecl*new_vd;
if(Cyc_Toc_can_use_voidstar(ctype)){
new_vd=({struct _tuple1*_Tmp5=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp5,Cyc_Toc_void_star_type(),0);});
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp7=({struct _tuple39*_Tmp8=_region_malloc(rgn,sizeof(struct _tuple39));_Tmp8->f1=vd,_Tmp8->f2=new_vd,_Tmp8->f3=ctype,_Tmp8->f4=0;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=*newdecls;_Tmp6;});*newdecls=_Tmp5;});}else{
# 3000
new_vd=({struct _tuple1*_Tmp5=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp5,ctype,0);});
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp7=({struct _tuple39*_Tmp8=_region_malloc(rgn,sizeof(struct _tuple39));_Tmp8->f1=vd,_Tmp8->f2=new_vd,_Tmp8->f3=ctype,_Tmp8->f4=0;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=*newdecls;_Tmp6;});*newdecls=_Tmp5;});}{
# 3004
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp5=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp6->tag=4,_Tmp6->f1=new_vd;_Tmp6;}),0U);Cyc_Absyn_assign_stmt(_Tmp5,
Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_Tmp5=s;Cyc_Toc_seq_stmt_opt(_Tmp5,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));});}}}case 4: _Tmp4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 3010
void*ctype=Cyc_Toc_typ_to_c_array(vd->type);
# 3012
{struct Cyc_List_List*pds=*prevdecls;for(0;pds != 0;pds=pds->tl){
struct _tuple40 _stmttmp53=*((struct _tuple40*)pds->hd);struct _tuple40 _Tmp5=_stmttmp53;void*_Tmp6;_Tmp6=_Tmp5.f2;{struct Cyc_Absyn_Vardecl*other_vd=_Tmp6;
# 3016
if(Cyc_Toc_is_usuable_type(ctype,other_vd->type)&& !
Cyc_Toc_vd_in_use(other_vd,newdecls)){
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp9=({struct _tuple39*_TmpA=_region_malloc(rgn,sizeof(struct _tuple39));_TmpA->f1=vd,_TmpA->f2=other_vd,_TmpA->f3=ctype,_TmpA->f4=0;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=*newdecls;_Tmp8;});*newdecls=_Tmp7;});
# 3020
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp8->tag=4,_Tmp8->f1=other_vd;_Tmp8;}),0U);Cyc_Absyn_assign_stmt(_Tmp7,
Cyc_Absyn_copy_exp(path),0U);});}}}}{
# 3025
struct Cyc_Absyn_Vardecl*new_vd;
if(Cyc_Toc_can_use_voidstar(ctype)){
new_vd=({struct _tuple1*_Tmp5=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp5,Cyc_Toc_void_star_type(),0);});
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp7=({struct _tuple39*_Tmp8=_region_malloc(rgn,sizeof(struct _tuple39));_Tmp8->f1=vd,_Tmp8->f2=new_vd,_Tmp8->f3=ctype,_Tmp8->f4=1;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=*newdecls;_Tmp6;});*newdecls=_Tmp5;});}else{
# 3031
new_vd=({struct _tuple1*_Tmp5=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp5,ctype,0);});
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp7=({struct _tuple39*_Tmp8=_region_malloc(rgn,sizeof(struct _tuple39));_Tmp8->f1=vd,_Tmp8->f2=new_vd,_Tmp8->f3=ctype,_Tmp8->f4=0;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=*newdecls;_Tmp6;});*newdecls=_Tmp5;});}
# 3035
return({struct Cyc_Absyn_Exp*_Tmp5=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp6->tag=4,_Tmp6->f1=new_vd;_Tmp6;}),0U);Cyc_Absyn_assign_stmt(_Tmp5,Cyc_Absyn_copy_exp(path),0U);});}}case 0:
 return 0;case 3: _Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp3;
# 3039
({void*_Tmp5=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_Tmp5;});{
# 3041
void*ctype=Cyc_Toc_typ_to_c_array(vd->type);
# 3043
{struct Cyc_List_List*pds=*prevdecls;for(0;pds != 0;pds=pds->tl){
struct _tuple40 _stmttmp54=*((struct _tuple40*)pds->hd);struct _tuple40 _Tmp5=_stmttmp54;void*_Tmp6;_Tmp6=_Tmp5.f2;{struct Cyc_Absyn_Vardecl*other_vd=_Tmp6;
# 3047
if(Cyc_Toc_is_usuable_type(ctype,other_vd->type)&& !
Cyc_Toc_vd_in_use(other_vd,newdecls)){
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp9=({struct _tuple39*_TmpA=_region_malloc(rgn,sizeof(struct _tuple39));_TmpA->f1=vd,_TmpA->f2=other_vd,_TmpA->f3=ctype,_TmpA->f4=0;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=*newdecls;_Tmp8;});*newdecls=_Tmp7;});{
# 3052
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp8->tag=4,_Tmp8->f1=other_vd;_Tmp8;}),0U);Cyc_Absyn_assign_stmt(_Tmp7,({
void*_Tmp8=({void*_Tmp9=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_Tmp9,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_Tmp8,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_Tmp7=s;Cyc_Toc_seq_stmt_opt(_Tmp7,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));});}}}}}{
# 3060
struct Cyc_Absyn_Vardecl*new_vd;
if(Cyc_Toc_can_use_voidstar(ctype)){
new_vd=({struct _tuple1*_Tmp5=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp5,Cyc_Toc_void_star_type(),0);});
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp7=({struct _tuple39*_Tmp8=_region_malloc(rgn,sizeof(struct _tuple39));_Tmp8->f1=vd,_Tmp8->f2=new_vd,_Tmp8->f3=ctype,_Tmp8->f4=1;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=*newdecls;_Tmp6;});*newdecls=_Tmp5;});}else{
# 3066
new_vd=({struct _tuple1*_Tmp5=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp5,ctype,0);});
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp7=({struct _tuple39*_Tmp8=_region_malloc(rgn,sizeof(struct _tuple39));_Tmp8->f1=vd,_Tmp8->f2=new_vd,_Tmp8->f3=ctype,_Tmp8->f4=0;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=*newdecls;_Tmp6;});*newdecls=_Tmp5;});}{
# 3072
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp5=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp6->tag=4,_Tmp6->f1=new_vd;_Tmp6;}),0U);Cyc_Absyn_assign_stmt(_Tmp5,({
void*_Tmp6=({void*_Tmp7=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_Tmp7,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_Tmp6,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_Tmp5=s;Cyc_Toc_seq_stmt_opt(_Tmp5,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));});}}}}case 9:
# 3077
 goto _LLE;case 10: _LLE:
 goto _LL10;case 11: _LL10:
 goto _LL12;case 12: _LL12:
 goto _LL14;case 13: _LL14:
 goto _LL16;case 14: _LL16:
 return 0;case 6: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1)->r)== 8){_Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1)->r)->f1;_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1)->r)->f2;_Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1)->r)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;struct Cyc_Absyn_Datatypefield*tuf=_Tmp3;struct Cyc_List_List*ps=_Tmp2;
# 3086
if(ps == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*field_ptr_typ=({void*_Tmp5=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_Tmp5,Cyc_Toc_mt_tq);});
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*tuf_tqts=tuf->typs;
struct Cyc_Absyn_Stmt*s=0;
for(1;ps != 0;(ps=ps->tl,tuf_tqts=(_check_null(tuf_tqts))->tl,++ cnt)){
struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)ps->hd;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*tuf_typ=(*((struct _tuple23*)(_check_null(tuf_tqts))->hd)).f2;
void*t2=_check_null(p2->topt);
void*t2c=Cyc_Toc_typ_to_c_array(t2);
struct Cyc_Absyn_Exp*arrow_exp=({struct Cyc_Absyn_Exp*_Tmp5=path;Cyc_Absyn_aggrarrow_exp(_Tmp5,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_Tmp5=s;Cyc_Toc_seq_stmt_opt(_Tmp5,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,arrow_exp,p2));});}}
# 3104
return s;}}}}else{_Tmp4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp4;
# 3149
return({struct _RegionHandle*_Tmp5=rgn;struct Cyc_Toc_Env**_Tmp6=nv;struct Cyc_List_List**_Tmp7=newdecls;struct Cyc_List_List**_Tmp8=prevdecls;struct Cyc_Absyn_Exp*_Tmp9=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,p2);});}}case 8: _Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_List_List*ps=_Tmp4;
# 3106
_Tmp4=ps;goto _LL1C;}case 5: _Tmp4=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_LL1C: {struct Cyc_List_List*ps=_Tmp4;
# 3108
struct Cyc_Absyn_Stmt*s=0;
{int cnt=1;for(0;ps != 0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)ps->hd;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*t2=_check_null(p2->topt);
struct _fat_ptr*f=Cyc_Absyn_fieldname(cnt);
s=({struct Cyc_Absyn_Stmt*_Tmp5=s;Cyc_Toc_seq_stmt_opt(_Tmp5,({struct _RegionHandle*_Tmp6=rgn;struct Cyc_Toc_Env**_Tmp7=nv;struct Cyc_List_List**_Tmp8=newdecls;struct Cyc_List_List**_Tmp9=prevdecls;struct Cyc_Absyn_Exp*_TmpA=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,p2);}));});}}}
# 3117
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="unresolved aggregate pattern!";_tag_fat(_Tmp7,sizeof(char),30U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp6,sizeof(void*),1));});else{_Tmp1=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{union Cyc_Absyn_AggrInfo info=_Tmp1;struct Cyc_List_List*dlps=_Tmp4;
# 3121
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
struct Cyc_Absyn_Stmt*s=0;
for(1;dlps != 0;dlps=dlps->tl){
struct _tuple41*tup=(struct _tuple41*)dlps->hd;
struct Cyc_Absyn_Pat*p2=(*tup).f2;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname((*tup).f1);
void*t2=_check_null(p2->topt);
void*t2c=Cyc_Toc_typ_to_c_array(t2);
struct Cyc_Absyn_Exp*memexp=Cyc_Toc_member_exp(path,f,0U);
# 3133
if((_check_null(ad->impl))->tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*ftype=(_check_null(Cyc_Absyn_lookup_field((_check_null(ad->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(ftype)))
# 3141
memexp=Cyc_Absyn_deref_exp(({void*_Tmp5=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_Tmp5,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3144
s=({struct Cyc_Absyn_Stmt*_Tmp5=s;Cyc_Toc_seq_stmt_opt(_Tmp5,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,memexp,p2));});}}}
# 3146
return s;}}case 15:
# 3151
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="unknownid pat";_tag_fat(_Tmp7,sizeof(char),14U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp6,sizeof(void*),1));});case 16:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="unknowncall pat";_tag_fat(_Tmp7,sizeof(char),16U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp6,sizeof(void*),1));});default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="exp pat";_tag_fat(_Tmp7,sizeof(char),8U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp6,sizeof(void*),1));});};}
# 3160
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_stmttmp55=e->r;void*_Tmp0=_stmttmp55;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 5: _Tmp3=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
_Tmp3=vd;goto _LL4;}case 4: _Tmp3=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 3166
{struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){
{void*ctype=Cyc_Toc_typ_to_c_array(vd->type);
struct Cyc_Absyn_Vardecl*tmpvd=({(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc;})(env,vd);
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct));_Tmp6->tag=1,({void*_Tmp7=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp8->tag=4,({struct Cyc_Absyn_Vardecl*_Tmp9=({(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc;})(env,vd);_Tmp8->f1=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;}),0U);
e1->topt=tmpvd->type;
({void*_Tmp6=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct));_Tmp7->tag=14,_Tmp7->f1=ctype,_Tmp7->f2=e1,_Tmp7->f3=0,_Tmp7->f4=1U;_Tmp7;});e->r=_Tmp6;});}
# 3167
;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7=_Tmp6;void*_Tmp8;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp7)->tag == Cyc_Core_Not_found)
# 3172
goto _LL3F;else{_Tmp8=_Tmp7;{void*exn=_Tmp8;(int)_rethrow(exn);}}_LL3F:;}}}
goto _LL0;}default: goto _LL3D;}case 6: _Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 3175
Cyc_Toc_subst_pattern_vars(env,e1);_Tmp3=e2;_Tmp2=e3;goto _LL8;}case 27: _Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LLA;}case 7: _Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LLC;}case 8: _Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LLE;}case 23: _Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL10;}case 9: _Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 3181
Cyc_Toc_subst_pattern_vars(env,e1);_Tmp3=e2;goto _LL12;}case 41: _Tmp3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL14;}case 38: _Tmp3=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL16;}case 11: _Tmp3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL18;}case 12: _Tmp3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL1A;}case 13: _Tmp3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL1C;}case 14: _Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL1E;}case 18: _Tmp3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL20;}case 20: _Tmp3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL22;}case 21: _Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL24;}case 22: _Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL26;}case 28: _Tmp3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL26: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL28;}case 15: _Tmp3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_Tmp3;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 34: _Tmp3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).rgn;_Tmp2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).num_elts;{struct Cyc_Absyn_Exp*eopt=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;
_Tmp3=eopt;_Tmp2=e;goto _LL2C;}case 16: _Tmp3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL2C: {struct Cyc_Absyn_Exp*eopt=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;
# 3196
if(eopt != 0)Cyc_Toc_subst_pattern_vars(env,eopt);
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 3: _Tmp3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp3;
_Tmp3=es;goto _LL30;}case 31: _Tmp3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL30: {struct Cyc_List_List*es=_Tmp3;
_Tmp3=es;goto _LL32;}case 24: _Tmp3=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL32: {struct Cyc_List_List*es=_Tmp3;
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 36: _Tmp3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*dles=_Tmp3;
_Tmp3=dles;goto _LL36;}case 29: _Tmp3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL36: {struct Cyc_List_List*dles=_Tmp3;
_Tmp3=dles;goto _LL38;}case 30: _Tmp3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL38: {struct Cyc_List_List*dles=_Tmp3;
_Tmp3=dles;goto _LL3A;}case 26: _Tmp3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3A: {struct Cyc_List_List*dles=_Tmp3;
_Tmp3=dles;goto _LL3C;}case 25: _Tmp3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL3C: {struct Cyc_List_List*dles=_Tmp3;
# 3206
for(1;dles != 0;dles=dles->tl){
Cyc_Toc_subst_pattern_vars(env,(*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}default: _LL3D:
 goto _LL0;}_LL0:;}struct _tuple42{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3218
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3226
void*_Tmp0=dopt;void*_Tmp1;void*_Tmp2;void*_Tmp3;if(_Tmp0 == 0)
# 3228
return Cyc_Absyn_skip_stmt(0U);else{switch(*((int*)_Tmp0)){case 0:
 return Cyc_Toc_throw_match_stmt();case 1: _Tmp3=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_Tmp0)->f1;{struct Cyc_Tcpat_Rhs*rhs=_Tmp3;
# 3232
for(1;lscs != 0;lscs=lscs->tl){
struct _tuple37*_stmttmp56=(struct _tuple37*)lscs->hd;struct _tuple37*_Tmp4=_stmttmp56;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;_Tmp8=(int*)& _Tmp4->f1;_Tmp7=_Tmp4->f2;_Tmp6=_Tmp4->f3;_Tmp5=_Tmp4->f4;{int*already_emitted=(int*)_Tmp8;struct _fat_ptr*init_lab=_Tmp7;struct _fat_ptr*code_lab=_Tmp6;struct Cyc_Absyn_Switch_clause*sc=_Tmp5;
struct Cyc_Absyn_Stmt*body=sc->body;
if(body == rhs->rhs){
# 3237
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3244
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_Tmp9=rgn;struct Cyc_List_List**_TmpA=decls;struct Cyc_Absyn_Exp*_TmpB=
Cyc_Absyn_var_exp(v,0U);
# 3244
Cyc_Toc_extract_pattern_vars(_Tmp9,& nv,& newdecls,_TmpA,_TmpB,sc->pattern);});
# 3248
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*ds=newdecls;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _stmttmp57=*((struct _tuple39*)ds->hd);struct _tuple39 _Tmp9=_stmttmp57;int _TmpA;void*_TmpB;void*_TmpC;void*_TmpD;_TmpD=_Tmp9.f1;_TmpC=_Tmp9.f2;_TmpB=_Tmp9.f3;_TmpA=_Tmp9.f4;{struct Cyc_Absyn_Vardecl*oldv=_TmpD;struct Cyc_Absyn_Vardecl*newv=_TmpC;void*oldvt=_TmpB;int cast=_TmpA;
({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple40*_Tmp10=({struct _tuple40*_Tmp11=_region_malloc(rgn,sizeof(struct _tuple40));_Tmp11->f1=oldv,_Tmp11->f2=newv;_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=*decls;_TmpF;});*decls=_TmpE;});
if(cast)({struct Cyc_Absyn_Exp*_TmpE=({void*_TmpF=oldvt;Cyc_Toc_cast_it(_TmpF,Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp10->tag=4,_Tmp10->f1=newv;_Tmp10;}),0U));});oldv->initializer=_TmpE;});else{
({struct Cyc_Absyn_Exp*_TmpE=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_TmpF->tag=4,_TmpF->f1=newv;_TmpF;}),0U);oldv->initializer=_TmpE;});}
(_check_null(oldv->initializer))->topt=newv->type;
# 3256
oldv->type=oldvt;
res=({struct Cyc_Absyn_Decl*_TmpE=({struct Cyc_Absyn_Decl*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp10=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp11->tag=0,_Tmp11->f1=oldv;_Tmp11;});_TmpF->r=_Tmp10;}),_TmpF->loc=0U;_TmpF;});Cyc_Absyn_decl_stmt(_TmpE,res,0U);});}}}
# 3259
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple42*_TmpB=({struct _tuple42*_TmpC=_region_malloc(rgn,sizeof(struct _tuple42));_TmpC->f1=nv,_TmpC->f2=code_lab,_TmpC->f3=body;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=*bodies;_TmpA;});*bodies=_Tmp9;});
return res;}}}}
# 3267
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="couldn't find rhs!";_tag_fat(_Tmp6,sizeof(char),19U);});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp5,sizeof(void*),1));});}default: _Tmp3=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_Tmp0)->f2;_Tmp1=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_Tmp0)->f3;{struct Cyc_List_List*symbolic_path=_Tmp3;struct Cyc_List_List*switches=_Tmp2;void*other_decision=_Tmp1;
# 3270
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3272
struct Cyc_Absyn_Exp*p=({struct Cyc_List_List*_Tmp4=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(symbolic_path);Cyc_Toc_compile_path(_Tmp4,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*ss=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(switches);
# 3275
struct _tuple38 _stmttmp58=Cyc_Toc_admits_switch(ss);struct _tuple38 _Tmp4=_stmttmp58;void*_Tmp5;int _Tmp6;_Tmp6=_Tmp4.f1;_Tmp5=_Tmp4.f2;{int allows_switch=_Tmp6;void*test_kind=_Tmp5;
if(allows_switch > 1){
# 3279
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));
({struct Cyc_Absyn_Switch_clause*_Tmp8=({struct Cyc_Absyn_Switch_clause*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_TmpA=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_Tmp9->pattern=_TmpA;}),_Tmp9->pat_vars=0,_Tmp9->where_clause=0,_Tmp9->body=res,_Tmp9->loc=0U;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=0;_Tmp7;});
# 3282
for(1;ss != 0;ss=ss->tl){
struct _tuple34 _stmttmp59=*((struct _tuple34*)ss->hd);struct _tuple34 _Tmp7=_stmttmp59;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{void*pat_test=_Tmp9;void*dec_tree=_Tmp8;
# 3285
struct Cyc_Absyn_Pat*case_exp=Cyc_Toc_compile_pat_test_as_case(pat_test);
# 3287
struct Cyc_Absyn_Stmt*s=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);
# 3289
new_lscs=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_TmpB=({struct Cyc_Absyn_Switch_clause*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TmpC->pattern=case_exp,_TmpC->pat_vars=0,_TmpC->where_clause=0,_TmpC->body=s,_TmpC->loc=0U;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=new_lscs;_TmpA;});}}
# 3291
{void*_Tmp7=test_kind;void*_Tmp8;switch(*((int*)_Tmp7)){case 1:
# 3293
 LOOP1: {
void*_stmttmp5A=p->r;void*_Tmp9=_stmttmp5A;void*_TmpA;switch(*((int*)_Tmp9)){case 14: _TmpA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp9)->f2;{struct Cyc_Absyn_Exp*e=_TmpA;
p=e;goto LOOP1;}case 20: _TmpA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp9)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
p=e;goto _LL1E;}default:
 goto _LL1E;}_LL1E:;}
# 3299
p=Cyc_Absyn_deref_exp(({void*_Tmp9=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_Tmp9,p);}),0U);goto _LL15;case 3: _Tmp8=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_Tmp7)->f1;{struct _fat_ptr*f=_Tmp8;
# 3301
p=({struct Cyc_Absyn_Exp*_Tmp9=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_Tmp9,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2:
 goto _LL1D;default: _LL1D:
# 3308
 p=Cyc_Toc_cast_it(Cyc_Absyn_sint_type,p);
goto _LL15;}_LL15:;}
# 3311
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct));_Tmp7->tag=10,_Tmp7->f1=p,_Tmp7->f2=new_lscs,_Tmp7->f3=0;_Tmp7;}),0U);}else{
# 3315
void*_Tmp7=test_kind;void*_Tmp8;if(*((int*)_Tmp7)== 2){_Tmp8=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_Tmp7)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 3317
struct Cyc_List_List*_Tmp9=ss;void*_TmpA;void*_TmpB;if(_Tmp9 != 0){if(*((int*)((struct _tuple34*)((struct Cyc_List_List*)_Tmp9)->hd)->f2)== 1){if(((struct Cyc_List_List*)_Tmp9)->tl == 0){_TmpB=((struct _tuple34*)_Tmp9->hd)->f1;_TmpA=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)_Tmp9->hd)->f2)->f1;{void*pat_test=_TmpB;struct Cyc_Tcpat_Rhs*rhs=_TmpA;
# 3323
for(1;lscs != 0;lscs=lscs->tl){
struct _tuple37*_stmttmp5B=(struct _tuple37*)lscs->hd;struct _tuple37*_TmpC=_stmttmp5B;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;_Tmp10=(int*)& _TmpC->f1;_TmpF=_TmpC->f2;_TmpE=_TmpC->f3;_TmpD=_TmpC->f4;{int*already_emitted=(int*)_Tmp10;struct _fat_ptr*init_lab=_TmpF;struct _fat_ptr*code_lab=_TmpE;struct Cyc_Absyn_Switch_clause*sc=_TmpD;
struct Cyc_Absyn_Stmt*body=sc->body;
if(body == rhs->rhs){
# 3328
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3333
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_Tmp11=rgn;struct Cyc_List_List**_Tmp12=decls;struct Cyc_Absyn_Exp*_Tmp13=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_Tmp11,& nv,& newdecls,_Tmp12,_Tmp13,sc->pattern);});
# 3337
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*ds=newdecls;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _stmttmp5C=*((struct _tuple39*)ds->hd);struct _tuple39 _Tmp11=_stmttmp5C;int _Tmp12;void*_Tmp13;void*_Tmp14;void*_Tmp15;_Tmp15=_Tmp11.f1;_Tmp14=_Tmp11.f2;_Tmp13=_Tmp11.f3;_Tmp12=_Tmp11.f4;{struct Cyc_Absyn_Vardecl*oldv=_Tmp15;struct Cyc_Absyn_Vardecl*newv=_Tmp14;void*oldvt=_Tmp13;int cast=_Tmp12;
({struct Cyc_List_List*_Tmp16=({struct Cyc_List_List*_Tmp17=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple40*_Tmp18=({struct _tuple40*_Tmp19=_region_malloc(rgn,sizeof(struct _tuple40));_Tmp19->f1=oldv,_Tmp19->f2=newv;_Tmp19;});_Tmp17->hd=_Tmp18;}),_Tmp17->tl=*decls;_Tmp17;});*decls=_Tmp16;});
if(cast)({struct Cyc_Absyn_Exp*_Tmp16=({void*_Tmp17=oldvt;Cyc_Toc_cast_it(_Tmp17,Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp18=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp18->tag=4,_Tmp18->f1=newv;_Tmp18;}),0U));});oldv->initializer=_Tmp16;});else{
({struct Cyc_Absyn_Exp*_Tmp16=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp17=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp17->tag=4,_Tmp17->f1=newv;_Tmp17;}),0U);oldv->initializer=_Tmp16;});}
(_check_null(oldv->initializer))->topt=newv->type;
# 3345
oldv->type=oldvt;
r=({struct Cyc_Absyn_Decl*_Tmp16=({struct Cyc_Absyn_Decl*_Tmp17=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp18=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp19=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp19->tag=0,_Tmp19->f1=oldv;_Tmp19;});_Tmp17->r=_Tmp18;}),_Tmp17->loc=0U;_Tmp17;});Cyc_Absyn_decl_stmt(_Tmp16,r,0U);});}}}
# 3348
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3352
Cyc_Toc_subst_pattern_vars(*decls,e);
Cyc_Toc_exp_to_c(nv,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_Tmp11=({struct Cyc_List_List*_Tmp12=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple42*_Tmp13=({struct _tuple42*_Tmp14=_region_malloc(rgn,sizeof(struct _tuple42));_Tmp14->f1=nv,_Tmp14->f2=code_lab,_Tmp14->f3=body;_Tmp14;});_Tmp12->hd=_Tmp13;}),_Tmp12->tl=*bodies;_Tmp12;});*bodies=_Tmp11;});
return r;}}}}
# 3362
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE="couldn't find rhs!";_tag_fat(_TmpE,sizeof(char),19U);});_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpD,sizeof(void*),1));});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE="bad where clause in match compiler";_tag_fat(_TmpE,sizeof(char),35U);});_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpD,sizeof(void*),1));});};}}else{
# 3367
for(1;ss != 0;ss=ss->tl){
struct _tuple34 _stmttmp5D=*((struct _tuple34*)ss->hd);struct _tuple34 _Tmp9=_stmttmp5D;void*_TmpA;void*_TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{void*pat_test=_TmpB;void*dec_tree=_TmpA;
struct Cyc_Absyn_Exp*test_exp=Cyc_Toc_compile_pat_test(p,pat_test);
struct Cyc_Absyn_Stmt*s=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}};}
# 3375
return res;}}}};}
# 3385
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3387
for(1;bodies != 0;bodies=bodies->tl){
struct _tuple42*_stmttmp5E=(struct _tuple42*)bodies->hd;struct _tuple42*_Tmp0=_stmttmp5E;void*_Tmp1;void*_Tmp2;_Tmp2=(struct Cyc_Toc_Env**)& _Tmp0->f1;_Tmp1=_Tmp0->f3;{struct Cyc_Toc_Env**nv=(struct Cyc_Toc_Env**)_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
if(s2 == s)return nv;}}
# 3393
return 0;}
# 3397
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3400
void*t=_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3403
struct _tuple30 _stmttmp5F=Cyc_Toc_temp_var_and_exp();struct _tuple30 _Tmp0=_stmttmp5F;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct _tuple1*v=_Tmp2;struct Cyc_Absyn_Exp*path=_Tmp1;
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _Tmp3=_new_region("rgn");struct _RegionHandle*rgn=& _Tmp3;_push_region(rgn);
{struct Cyc_Toc_Env*_Tmp4=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_Tmp4;
# 3409
struct Cyc_List_List*lscs=
({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c;})(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3414
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3422
{struct Cyc_List_List*lscs2=lscs;for(0;lscs2 != 0;lscs2=lscs2->tl){
struct _tuple37*_stmttmp60=(struct _tuple37*)lscs2->hd;struct _tuple37*_Tmp5=_stmttmp60;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5->f3;_Tmp6=_Tmp5->f4;{struct _fat_ptr*body_lab=_Tmp7;struct Cyc_Absyn_Switch_clause*body_sc=_Tmp6;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3426
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3429
if(envp == 0)continue;{
struct Cyc_Toc_Env*env=*envp;
# 3432
if(lscs2->tl != 0){
struct _tuple37*_stmttmp61=(struct _tuple37*)(_check_null(lscs2->tl))->hd;struct _tuple37*_Tmp8=_stmttmp61;void*_Tmp9;void*_TmpA;_TmpA=_Tmp8->f3;_Tmp9=_Tmp8->f4;{struct _fat_ptr*fallthru_lab=_TmpA;struct Cyc_Absyn_Switch_clause*next_sc=_Tmp9;
# 3436
struct Cyc_Toc_Env**next_case_env=Cyc_Toc_find_case_env(mybodies,next_sc->body);
# 3441
if(next_case_env == 0)
({struct Cyc_Toc_Env*_TmpB=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_TmpB,s);});else{
# 3445
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars != 0){
vs=({struct Cyc_List_List*(*_TmpB)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls;});_TmpB((({(struct _tuple0(*)(struct Cyc_List_List*))Cyc_List_split;})((struct Cyc_List_List*)(_check_null(next_sc->pat_vars))->v)).f1);});
vs=({struct Cyc_List_List*(*_TmpB)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c;});struct Cyc_Absyn_Vardecl*(*_TmpC)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*)=({(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc;});struct Cyc_List_List*_TmpD=mydecls;_TmpB(_TmpC,_TmpD,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(vs));});}
# 3450
({struct Cyc_Toc_Env*_TmpB=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_TmpB,s);});}}}else{
# 3454
({struct Cyc_Toc_Env*_Tmp8=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_Tmp8,s);});}}}}}{
# 3458
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_Tmp5=test_tree;Cyc_Absyn_seq_stmt(_Tmp5,({struct _fat_ptr*_Tmp6=end_l;Cyc_Absyn_label_stmt(_Tmp6,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3460
struct Cyc_List_List*added=0;
for(1;mydecls != 0;mydecls=(_check_null(mydecls))->tl){
struct _tuple40 _stmttmp62=*((struct _tuple40*)(_check_null(mydecls))->hd);struct _tuple40 _Tmp5=_stmttmp62;void*_Tmp6;_Tmp6=_Tmp5.f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
{struct Cyc_List_List*itr=added;for(0;itr != 0;itr=itr->tl){
if((struct Cyc_Absyn_Vardecl*)itr->hd == vd)goto bottom;}}
added=({struct Cyc_List_List*_Tmp7=_region_malloc(rgn,sizeof(struct Cyc_List_List));_Tmp7->hd=vd,_Tmp7->tl=added;_Tmp7;});
res=({struct Cyc_Absyn_Decl*_Tmp7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp8->tag=0,_Tmp8->f1=vd;_Tmp8;}),0U);Cyc_Absyn_decl_stmt(_Tmp7,res,0U);});
bottom:
 continue;}}
# 3471
({void*_Tmp5=({struct _tuple1*_Tmp6=v;void*_Tmp7=Cyc_Toc_typ_to_c(_check_null(e->topt));struct Cyc_Absyn_Exp*_Tmp8=e;Cyc_Absyn_declare_stmt(_Tmp6,_Tmp7,_Tmp8,res,0U);})->r;whole_s->r=_Tmp5;});
_npop_handler(0);return;}}
# 3406
;_pop_region();}}}
# 3479
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_Tmp0[1];({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_signed_int_exp(n - 1,0U);_Tmp0[0]=_Tmp1;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__npop_handler_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);}
# 3482
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_Tmp0=({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_Tmp1,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_Tmp0;});}
# 3487
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3489
while(1){
void*_stmttmp63=s->r;void*_Tmp0=_stmttmp63;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 return;case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Toc_exp_to_c(nv,e);return;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1 == 0){
({int _Tmp5=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_Tmp5,s);});return;}else{_Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
# 3496
void*t=Cyc_Toc_typ_to_c(_check_null((_check_null(e))->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*x=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*retn_stmt=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_Tmp5=(Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U))->r;s->r=_Tmp5;});}
# 3505
return;}}}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 3507
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 3511
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;_Tmp2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3).f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
# 3516
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_Tmp4=s2;_Tmp3=e3;goto _LL10;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;_LL10: {struct Cyc_Absyn_Stmt*s2=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp4=e;_Tmp3=s2;goto _LL12;}case 5: _Tmp4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1).f1;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 3521
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _Tmp5=_new_region("temp");struct _RegionHandle*temp=& _Tmp5;_push_region(temp);({struct Cyc_Toc_Env*_Tmp6=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_Tmp6,s2);});
_npop_handler(0);return;
# 3522
;_pop_region();}}case 6:  {
# 3525
struct Cyc_Toc_Env*_Tmp5=nv;void*_Tmp6;_Tmp6=_Tmp5->break_lab;{struct _fat_ptr**b=_Tmp6;
if(b != 0)
({void*_Tmp7=Cyc_Toc_goto_stmt_r(*b);s->r=_Tmp7;});
# 3529
({int _Tmp7=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_Tmp7,s);});
return;}}case 7:  {
# 3532
struct Cyc_Toc_Env*_Tmp5=nv;void*_Tmp6;_Tmp6=_Tmp5->continue_lab;{struct _fat_ptr**c=_Tmp6;
if(c != 0)
({void*_Tmp7=Cyc_Toc_goto_stmt_r(*c);s->r=_Tmp7;});
goto _LL18;}}case 8: _LL18:
# 3537
({int _Tmp5=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_Tmp5,s);});
return;case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*lab=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
s=s1;continue;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*scs=_Tmp3;void*dec_tree_opt=_Tmp2;
# 3541
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11: _Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp4;struct Cyc_Absyn_Switch_clause**dest_clause=_Tmp3;
# 3544
struct Cyc_Toc_Env*_Tmp5=nv;void*_Tmp6;_Tmp6=_Tmp5->fallthru_info;{struct Cyc_Toc_FallthruInfo*fi=_Tmp6;
if(fi == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="fallthru in unexpected place";_tag_fat(_Tmp9,sizeof(char),29U);});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp8,sizeof(void*),1));});{
struct Cyc_Toc_FallthruInfo _stmttmp64=*fi;struct Cyc_Toc_FallthruInfo _Tmp7=_stmttmp64;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.label;_Tmp8=_Tmp7.binders;{struct _fat_ptr*l=_Tmp9;struct Cyc_List_List*vs=_Tmp8;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3550
({int _TmpA=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_TmpA,s2);});{
struct Cyc_List_List*vs2=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(vs);
struct Cyc_List_List*es2=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(es);
for(1;vs2 != 0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)(_check_null(es2))->hd);
s2=({struct Cyc_Absyn_Stmt*_TmpA=({struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct));_TmpC->tag=5,_TmpC->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_TmpC;}),0U);Cyc_Absyn_assign_stmt(_TmpB,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_TmpA,s2,0U);});}
# 3558
s->r=s2->r;
return;}}}}}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*body=_Tmp3;
# 3564
{void*_stmttmp65=d->r;void*_Tmp5=_stmttmp65;void*_Tmp6;void*_Tmp7;void*_Tmp8;switch(*((int*)_Tmp5)){case 0: _Tmp8=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp8;
Cyc_Toc_local_decl_to_c(nv,vd,body);goto _LL2F;}case 2: _Tmp8=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;_Tmp7=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp5)->f3;_Tmp6=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp5)->f4;{struct Cyc_Absyn_Pat*p=_Tmp8;struct Cyc_Absyn_Exp*e=_Tmp7;void*dec_tree=_Tmp6;
# 3571
{void*_stmttmp66=p->r;void*_Tmp9=_stmttmp66;void*_TmpA;if(*((int*)_Tmp9)== 1){if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp9)->f2)->r)== 0){_TmpA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp9)->f1;{struct Cyc_Absyn_Vardecl*vd=_TmpA;
# 3573
if(Cyc_Absyn_var_may_appear_exp(vd->name,e)){
struct Cyc_Absyn_Vardecl*new_vd=({struct _tuple1*_TmpB=Cyc_Toc_temp_var();void*_TmpC=vd->type;Cyc_Absyn_new_vardecl(0U,_TmpB,_TmpC,e);});
({struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_TmpC->tag=4,_TmpC->f1=new_vd;_TmpC;}),0U);vd->initializer=_TmpB;});
(_check_null(vd->initializer))->topt=new_vd->type;
({void*_TmpB=({struct Cyc_Absyn_Decl*_TmpC=({void*_TmpD=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpE->tag=0,_TmpE->f1=new_vd;_TmpE;});Cyc_Absyn_new_decl(_TmpD,s->loc);});struct Cyc_Absyn_Stmt*_TmpD=({
struct Cyc_Absyn_Decl*_TmpE=({void*_TmpF=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp10->tag=0,_Tmp10->f1=vd;_Tmp10;});Cyc_Absyn_new_decl(_TmpF,s->loc);});struct Cyc_Absyn_Stmt*_TmpF=body;Cyc_Absyn_decl_stmt(_TmpE,_TmpF,s->loc);});
# 3577
Cyc_Absyn_decl_stmt(_TmpC,_TmpD,s->loc);})->r;s->r=_TmpB;});}else{
# 3581
vd->initializer=e;
({void*_TmpB=({struct Cyc_Absyn_Decl*_TmpC=({void*_TmpD=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpE->tag=0,_TmpE->f1=vd;_TmpE;});Cyc_Absyn_new_decl(_TmpD,s->loc);});struct Cyc_Absyn_Stmt*_TmpD=body;Cyc_Absyn_decl_stmt(_TmpC,_TmpD,s->loc);})->r;s->r=_TmpB;});}
# 3584
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F:
# 3590
({void*_TmpB=(Cyc_Toc_letdecl_to_c(nv,p,dec_tree,_check_null(e->topt),e,body))->r;s->r=_TmpB;});
goto _LL3C;}_LL3C:;}
# 3593
goto _LL2F;}case 3: _Tmp8=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_List_List*vds=_Tmp8;
# 3597
struct Cyc_List_List*rvds=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(vds);
if(rvds == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="empty Letv_d";_tag_fat(_TmpB,sizeof(char),13U);});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_TmpA,sizeof(void*),1));});
({void*_Tmp9=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpA->tag=0,_TmpA->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_TmpA;});d->r=_Tmp9;});
rvds=rvds->tl;
for(1;rvds != 0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*d2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp9->tag=0,_Tmp9->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_Tmp9;}),0U);
({void*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=d2;Cyc_Absyn_decl_stmt(_TmpA,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_Tmp9;});}
# 3606
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1: _Tmp8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp8;
# 3609
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,body);
goto _LL2F;}case 4: _Tmp8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;_Tmp7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp5)->f2;_Tmp6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp5)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp8;struct Cyc_Absyn_Vardecl*vd=_Tmp7;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp6;
# 3614
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _stmttmp67=Cyc_Toc_temp_var_and_exp();struct _tuple30 _Tmp9=_stmttmp67;void*_TmpA;void*_TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{struct _tuple1*rh_var=_TmpB;struct Cyc_Absyn_Exp*rh_exp=_TmpA;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3620
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Flags_no_regions)
({void*_TmpC=({struct _tuple1*_TmpD=x_var;void*_TmpE=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_TmpF=
Cyc_Absyn_signed_int_exp(0,0U);
# 3622
Cyc_Absyn_declare_stmt(_TmpD,_TmpE,_TmpF,body,0U);})->r;s->r=_TmpC;});else{
# 3624
if((unsigned)open_exp_opt){
# 3626
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_TmpC=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_TmpC,({struct _fat_ptr*_TmpD=_cycalloc(sizeof(struct _fat_ptr));*_TmpD=({const char*_TmpE="h";_tag_fat(_TmpE,sizeof(char),2U);});_TmpD;}),0U);}),0U);
({void*_TmpC=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U))->r;s->r=_TmpC;});}}else{
# 3635
({void*_TmpC=({struct _tuple1*_TmpD=rh_var;void*_TmpE=rh_struct_typ;struct Cyc_Absyn_Exp*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[1];({struct Cyc_Absyn_Exp*_Tmp11=
Cyc_Absyn_string_exp(*(*x_var).f2,0U);_Tmp10[0]=_Tmp11;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__new_region_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),1));});
# 3635
Cyc_Absyn_declare_stmt(_TmpD,_TmpE,_TmpF,({
# 3637
struct _tuple1*_Tmp10=x_var;void*_Tmp11=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_Tmp10,_Tmp11,_Tmp12,({
struct Cyc_Absyn_Stmt*_Tmp13=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_Tmp14[1];_Tmp14[0]=x_exp;Cyc_Toc_fncall_exp_dl(Cyc_Toc__push_region_e,_tag_fat(_Tmp14,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);Cyc_Absyn_seq_stmt(_Tmp13,({
struct Cyc_Absyn_Stmt*_Tmp14=body;Cyc_Absyn_seq_stmt(_Tmp14,
Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__pop_region_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3635
s->r=_TmpC;});}}
# 3642
return;}}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="bad nested declaration within function";_tag_fat(_TmpB,sizeof(char),39U);});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpA,sizeof(void*),1));});}_LL2F:;}
# 3645
return;}default: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Stmt*body=_Tmp4;struct Cyc_List_List*scs=_Tmp3;void*dec_tree=_Tmp2;
# 3659 "toc.cyc"
struct _tuple30 _stmttmp68=Cyc_Toc_temp_var_and_exp();struct _tuple30 _Tmp5=_stmttmp68;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{struct _tuple1*h_var=_Tmp7;struct Cyc_Absyn_Exp*h_exp=_Tmp6;
struct _tuple30 _stmttmp69=Cyc_Toc_temp_var_and_exp();struct _tuple30 _Tmp8=_stmttmp69;void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct _tuple1*e_var=_TmpA;struct Cyc_Absyn_Exp*e_exp=_Tmp9;
struct _tuple30 _stmttmp6A=Cyc_Toc_temp_var_and_exp();struct _tuple30 _TmpB=_stmttmp6A;void*_TmpC;void*_TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{struct _tuple1*was_thrown_var=_TmpD;struct Cyc_Absyn_Exp*was_thrown_exp=_TmpC;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3666
e_exp->topt=e_typ;{
struct _RegionHandle _TmpE=_new_region("temp");struct _RegionHandle*temp=& _TmpE;_push_region(temp);
# 3669
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*tryandpop_stmt=({struct Cyc_Absyn_Stmt*_TmpF=body;Cyc_Absyn_seq_stmt(_TmpF,
Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__pop_handler_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U),0U);});
# 3673
struct Cyc_Absyn_Stmt*handler_stmt=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct));_TmpF->tag=10,_TmpF->f1=e_exp,_TmpF->f2=scs,_TmpF->f3=dec_tree;_TmpF;}),0U);
# 3675
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3678
struct Cyc_Absyn_Exp*setjmp_call=({struct Cyc_Absyn_Exp*_TmpF[1];({struct Cyc_Absyn_Exp*_Tmp10=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_TmpF[0]=_Tmp10;});Cyc_Toc_fncall_exp_dl(Cyc_Toc_setjmp_e,_tag_fat(_TmpF,sizeof(struct Cyc_Absyn_Exp*),1));});
struct Cyc_Absyn_Stmt*pushhandler_call=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_TmpF[1];({struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_address_exp(h_exp,0U);_TmpF[0]=_Tmp10;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__push_handler_e,_tag_fat(_TmpF,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*zero_exp=Cyc_Absyn_int_exp(0U,0,0U);
struct Cyc_Absyn_Exp*one_exp=Cyc_Absyn_int_exp(0U,1,0U);
({void*_TmpF=({
struct _tuple1*_Tmp10=h_var;void*_Tmp11=h_typ;Cyc_Absyn_declare_stmt(_Tmp10,_Tmp11,0,({
struct Cyc_Absyn_Stmt*_Tmp12=pushhandler_call;Cyc_Absyn_seq_stmt(_Tmp12,({
struct _tuple1*_Tmp13=was_thrown_var;void*_Tmp14=was_thrown_typ;struct Cyc_Absyn_Exp*_Tmp15=zero_exp;Cyc_Absyn_declare_stmt(_Tmp13,_Tmp14,_Tmp15,({
struct Cyc_Absyn_Stmt*_Tmp16=({struct Cyc_Absyn_Exp*_Tmp17=setjmp_call;struct Cyc_Absyn_Stmt*_Tmp18=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3688
Cyc_Absyn_ifthenelse_stmt(_Tmp17,_Tmp18,
# 3690
Cyc_Toc_skip_stmt_dl(),0U);});
# 3688
Cyc_Absyn_seq_stmt(_Tmp16,({
# 3691
struct Cyc_Absyn_Exp*_Tmp17=Cyc_Absyn_prim1_exp(11U,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_Tmp18=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_Tmp17,_Tmp18,({
# 3693
struct _tuple1*_Tmp19=e_var;void*_Tmp1A=e_typ;struct Cyc_Absyn_Exp*_Tmp1B=({
void*_Tmp1C=e_typ;Cyc_Toc_cast_it(_Tmp1C,Cyc_Toc_get_exn_thrown_expression());});
# 3693
Cyc_Absyn_declare_stmt(_Tmp19,_Tmp1A,_Tmp1B,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3684
s->r=_TmpF;});}}
# 3697
_npop_handler(0);return;
# 3667
;_pop_region();}}}}}};}}
# 3704
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_Tmp0=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_Tmp0;});
({void*_Tmp0=Cyc_Toc_typ_to_c(_check_null(f->cached_type));f->cached_type=_Tmp0;});{
struct _RegionHandle _Tmp0=_new_region("frgn");struct _RegionHandle*frgn=& _Tmp0;_push_region(frgn);
{struct Cyc_Toc_Env*_Tmp1=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_Tmp1;
{struct Cyc_List_List*args=(f->i).args;for(0;args != 0;args=args->tl){
({void*_Tmp2=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_Tmp2;});}}
# 3718
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 3722
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_Tmp2=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));({struct Cyc_Hashtable_Table*_Tmp3=({(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;})(*_check_null(Cyc_Toc_gpop_tables),f);*_Tmp2=_Tmp3;});_Tmp2;});
if((unsigned)(f->i).cyc_varargs &&(_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _stmttmp6B=*_check_null((f->i).cyc_varargs);struct Cyc_Absyn_VarargInfo _Tmp2=_stmttmp6B;int _Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;_Tmp6=_Tmp2.name;_Tmp5=_Tmp2.tq;_Tmp4=_Tmp2.type;_Tmp3=_Tmp2.inject;{struct _fat_ptr*n=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*t=_Tmp4;int i=_Tmp3;
void*t2=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));
struct _tuple1*x2=({struct _tuple1*_Tmp7=_cycalloc(sizeof(struct _tuple1));_Tmp7->f1=Cyc_Absyn_Loc_n,_Tmp7->f2=_check_null(n);_Tmp7;});
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_Tmp9=(f->i).args;_Tmp8(_Tmp9,({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple9*_TmpB=({struct _tuple9*_TmpC=_cycalloc(sizeof(struct _tuple9));_TmpC->f1=n,_TmpC->f2=tq,_TmpC->f3=t2;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=0;_TmpA;}));});(f->i).args=_Tmp7;});
(f->i).cyc_varargs=0;}}
# 3731
{struct Cyc_List_List*arg_vds=(struct Cyc_List_List*)(_check_null(f->param_vardecls))->v;for(0;arg_vds != 0;arg_vds=arg_vds->tl){
({void*_Tmp2=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_Tmp2;});}}
({struct Cyc_Toc_Env*_Tmp2=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_Tmp2,f->body);});}
# 3713
;_pop_region();}}
# 3736
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _Tmp0=s;switch((int)_Tmp0){case Cyc_Absyn_Abstract:
 return 2U;case Cyc_Absyn_ExternC:
 return 3U;default:
 return s;};}struct _tuple43{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3752 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3754
struct _tuple43 _stmttmp6C=*env;struct _tuple43 _Tmp0=_stmttmp6C;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=*_Tmp0.f2;{struct Cyc_Toc_TocState*s=_Tmp2;struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
struct _tuple1*n=ad->name;
struct Cyc_Toc_TocState _stmttmp6D=*s;struct Cyc_Toc_TocState _Tmp3=_stmttmp6D;void*_Tmp4;_Tmp4=_Tmp3.aggrs_so_far;{struct Cyc_Dict_Dict*aggrs_so_far=_Tmp4;
int seen_defn_before;
struct _tuple18**dopt=({(struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;})(*aggrs_so_far,n);
if(dopt == 0){
seen_defn_before=0;{
struct _tuple18*v=({struct _tuple18*_Tmp5=_region_malloc(d,sizeof(struct _tuple18));
_Tmp5->f1=ad,(int)ad->kind == 0?({void*_Tmp6=Cyc_Absyn_strctq(n);_Tmp5->f2=_Tmp6;}):({void*_Tmp6=Cyc_Absyn_unionq_type(n);_Tmp5->f2=_Tmp6;});_Tmp5;});
({struct Cyc_Dict_Dict _Tmp5=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))Cyc_Dict_insert;})(*aggrs_so_far,n,v);*aggrs_so_far=_Tmp5;});}}else{
# 3765
struct _tuple18*_stmttmp6E=*dopt;struct _tuple18*_Tmp5=_stmttmp6E;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5->f1;_Tmp6=_Tmp5->f2;{struct Cyc_Absyn_Aggrdecl*ad2=_Tmp7;void*t=_Tmp6;
if(ad2->impl == 0){
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict(*_Tmp9)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))Cyc_Dict_insert;});struct Cyc_Dict_Dict _TmpA=*aggrs_so_far;struct _tuple1*_TmpB=n;_Tmp9(_TmpA,_TmpB,({struct _tuple18*_TmpC=_region_malloc(d,sizeof(struct _tuple18));_TmpC->f1=ad,_TmpC->f2=t;_TmpC;}));});*aggrs_so_far=_Tmp8;});
seen_defn_before=0;}else{
# 3770
seen_defn_before=1;}}}{
# 3772
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_Tmp5->kind=ad->kind,_Tmp5->sc=2U,_Tmp5->name=ad->name,_Tmp5->tvs=0,_Tmp5->impl=0,_Tmp5->expected_mem_kind=0,_Tmp5->attributes=ad->attributes;_Tmp5;});
# 3779
if(ad->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_Tmp5=({struct Cyc_Absyn_AggrdeclImpl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_AggrdeclImpl));_Tmp6->exist_vars=0,_Tmp6->rgn_po=0,_Tmp6->fields=0,_Tmp6->tagged=0;_Tmp6;});new_ad->impl=_Tmp5;});{
# 3784
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*fields=(_check_null(ad->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 3788
struct Cyc_Absyn_Aggrfield*old_field=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*old_type=old_field->type;
struct Cyc_List_List*old_atts=old_field->attributes;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind == 0 && fields->tl == 0 ||(int)ad->kind == 1)){
# 3802 "toc.cyc"
void*_stmttmp6F=Cyc_Absyn_compress(old_type);void*_Tmp5=_stmttmp6F;unsigned _Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;if(*((int*)_Tmp5)== 4){_Tmp9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp5)->f1).elt_type;_Tmp8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp5)->f1).tq;_Tmp7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp5)->f1).zero_term;_Tmp6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp5)->f1).zt_loc;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;void*zt=_Tmp7;unsigned ztl=_Tmp6;
# 3805
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_TmpA->tag=4,(_TmpA->f1).elt_type=et,(_TmpA->f1).tq=tq,({struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_uint_exp(0U,0U);(_TmpA->f1).num_elts=_TmpB;}),(_TmpA->f1).zero_term=zt,(_TmpA->f1).zt_loc=ztl;_TmpA;});
goto _LL9;}}else{
# 3808
old_atts=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_TmpC->tag=6,_TmpC->f1=0;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=old_atts;_TmpA;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_TmpA->tag=4,({void*_TmpB=Cyc_Toc_void_star_type();(_TmpA->f1).elt_type=_TmpB;}),(_TmpA->f1).tq=Cyc_Toc_mt_tq,({
# 3811
struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_uint_exp(0U,0U);(_TmpA->f1).num_elts=_TmpB;}),(_TmpA->f1).zero_term=Cyc_Absyn_false_type,(_TmpA->f1).zt_loc=0U;_TmpA;});}_LL9:;}{
# 3815
struct Cyc_Absyn_Aggrfield*new_field=({struct Cyc_Absyn_Aggrfield*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp5->name=old_field->name,_Tmp5->tq=Cyc_Toc_mt_tq,({
# 3817
void*_Tmp6=Cyc_Toc_typ_to_c(old_type);_Tmp5->type=_Tmp6;}),_Tmp5->width=old_field->width,({
# 3819
struct Cyc_List_List*_Tmp6=Cyc_Atts_atts2c(old_atts);_Tmp5->attributes=_Tmp6;}),_Tmp5->requires_clause=0;_Tmp5;});
# 3825
if((_check_null(ad->impl))->tagged){
void*T=new_field->type;
struct _fat_ptr*f=new_field->name;
struct _fat_ptr s=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*(*ad->name).f2;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=*f;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="_union_%s_%s";_tag_fat(_Tmp8,sizeof(char),13U);}),_tag_fat(_Tmp7,sizeof(void*),2));});
struct _fat_ptr*str=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=s;_Tmp5;});
struct Cyc_Absyn_Aggrfield*value_field=({struct Cyc_Absyn_Aggrfield*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp5->name=Cyc_Toc_val_sp,_Tmp5->tq=Cyc_Toc_mt_tq,_Tmp5->type=T,_Tmp5->width=0,_Tmp5->attributes=0,_Tmp5->requires_clause=0;_Tmp5;});
struct Cyc_Absyn_Aggrfield*tag_field=({struct Cyc_Absyn_Aggrfield*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp5->name=Cyc_Toc_tag_sp,_Tmp5->tq=Cyc_Toc_mt_tq,_Tmp5->type=Cyc_Absyn_sint_type,_Tmp5->width=0,_Tmp5->attributes=0,_Tmp5->requires_clause=0;_Tmp5;});
struct Cyc_Absyn_Aggrdecl*ad2=({struct _fat_ptr*_Tmp5=str;Cyc_Toc_make_c_struct_defn(_Tmp5,({struct Cyc_Absyn_Aggrfield*_Tmp6[2];_Tmp6[0]=tag_field,_Tmp6[1]=value_field;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Aggrfield*),2));}));});
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp7->tag=5,_Tmp7->f1=ad2;_Tmp7;}),0U);_Tmp5->hd=_Tmp6;}),_Tmp5->tl=Cyc_Toc_result_decls;_Tmp5;});
({void*_Tmp5=Cyc_Absyn_strct(str);new_field->type=_Tmp5;});}
# 3836
new_fields=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=new_field,_Tmp5->tl=new_fields;_Tmp5;});}}}
# 3838
({struct Cyc_List_List*_Tmp5=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(new_fields);(new_ad->impl)->fields=_Tmp5;});}}
# 3841
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp6=({struct Cyc_Absyn_Decl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp8=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp9->tag=5,_Tmp9->f1=new_ad;_Tmp9;});_Tmp7->r=_Tmp8;}),_Tmp7->loc=0U;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=Cyc_Toc_result_decls;_Tmp5;});
return 0;}}}}
# 3845
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
({(int(*)(struct Cyc_Absyn_Aggrdecl**,int(*)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state;})(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple44{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3873 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple44*env){
# 3876
struct _tuple44 _stmttmp70=*env;struct _tuple44 _Tmp0=_stmttmp70;void*_Tmp1;void*_Tmp2;_Tmp2=(_Tmp0.f1)->datatypes_so_far;_Tmp1=_Tmp0.f2;{struct Cyc_Set_Set**datatypes_so_far=_Tmp2;struct Cyc_Absyn_Datatypedecl*tud=_Tmp1;
struct _tuple1*n=tud->name;
if(tud->fields == 0 ||({(int(*)(struct Cyc_Set_Set*,struct _tuple1*))Cyc_Set_member;})(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_Tmp3=({(struct Cyc_Set_Set*(*)(struct _RegionHandle*,struct Cyc_Set_Set*,struct _tuple1*))Cyc_Set_rinsert;})(d,*datatypes_so_far,n);*datatypes_so_far=_Tmp3;});
{struct Cyc_List_List*fields=(struct Cyc_List_List*)(_check_null(tud->fields))->v;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3884
struct Cyc_List_List*fs=0;
int i=1;
{struct Cyc_List_List*ts=f->typs;for(0;ts != 0;(ts=ts->tl,i ++)){
struct Cyc_Absyn_Aggrfield*f=({struct Cyc_Absyn_Aggrfield*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));({struct _fat_ptr*_Tmp4=Cyc_Absyn_fieldname(i);_Tmp3->name=_Tmp4;}),_Tmp3->tq=(*((struct _tuple23*)ts->hd)).f1,({
void*_Tmp4=Cyc_Toc_typ_to_c((*((struct _tuple23*)ts->hd)).f2);_Tmp3->type=_Tmp4;}),_Tmp3->width=0,_Tmp3->attributes=0,_Tmp3->requires_clause=0;_Tmp3;});
fs=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=f,_Tmp3->tl=fs;_Tmp3;});}}
# 3891
fs=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_Tmp4=({struct Cyc_Absyn_Aggrfield*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp5->name=Cyc_Toc_tag_sp,_Tmp5->tq=Cyc_Toc_mt_tq,_Tmp5->type=Cyc_Absyn_sint_type,_Tmp5->width=0,_Tmp5->attributes=0,_Tmp5->requires_clause=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fs);_Tmp3->tl=_Tmp4;});_Tmp3;});{
struct Cyc_Absyn_Aggrdecl*ad=({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));*_Tmp4=({const char*_Tmp5="";_tag_fat(_Tmp5,sizeof(char),1U);});_Tmp4;});Cyc_Toc_make_c_struct_defn(_Tmp3,fs);});
({struct _tuple1*_Tmp3=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_Tmp3;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp5->tag=5,_Tmp5->f1=ad;_Tmp5;}),0U);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=Cyc_Toc_result_decls;_Tmp3;});}}}
# 3897
return 0;}}
# 3900
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
({(int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple44*)))Cyc_Toc_use_toc_state;})(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3919 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple44*env){
# 3922
struct _tuple44 _stmttmp71=*env;struct _tuple44 _Tmp0=_stmttmp71;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_Toc_TocState*s=_Tmp2;struct Cyc_Absyn_Datatypedecl*xd=_Tmp1;
if(xd->fields == 0)
return 0;{
struct Cyc_Toc_TocState _stmttmp72=*s;struct Cyc_Toc_TocState _Tmp3=_stmttmp72;void*_Tmp4;_Tmp4=_Tmp3.xdatatypes_so_far;{struct Cyc_Dict_Dict*xdatatypes_so_far=_Tmp4;
struct _tuple1*n=xd->name;
{struct Cyc_List_List*fs=(struct Cyc_List_List*)(_check_null(xd->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);
void*tag_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);
# 3933
int*_stmttmp73=({(int*(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;})(*xdatatypes_so_far,f->name);int*_Tmp5=_stmttmp73;if(_Tmp5 == 0){
# 3935
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != 3)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
struct Cyc_Absyn_Vardecl*tag_decl=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp8->tag=0,_Tmp8->f1=tag_decl;_Tmp8;}),0U);_Tmp6->hd=_Tmp7;}),_Tmp6->tl=Cyc_Toc_result_decls;_Tmp6;});
({struct Cyc_Dict_Dict _Tmp6=
({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))Cyc_Dict_insert;})(*xdatatypes_so_far,f->name,(int)f->sc != 3);
# 3941
*xdatatypes_so_far=_Tmp6;});{
# 3943
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*tqts=f->typs;for(0;tqts != 0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*field_n=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*newf=({struct Cyc_Absyn_Aggrfield*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp6->name=field_n,_Tmp6->tq=(*((struct _tuple23*)tqts->hd)).f1,({
void*_Tmp7=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqts->hd)).f2);_Tmp6->type=_Tmp7;}),_Tmp6->width=0,_Tmp6->attributes=0,_Tmp6->requires_clause=0;_Tmp6;});
fields=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=newf,_Tmp6->tl=fields;_Tmp6;});}}
# 3951
fields=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_Tmp7=({struct Cyc_Absyn_Aggrfield*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp8->name=Cyc_Toc_tag_sp,_Tmp8->tq=Cyc_Toc_mt_tq,({
void*_Tmp9=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_Tmp8->type=_Tmp9;}),_Tmp8->width=0,_Tmp8->attributes=0,_Tmp8->requires_clause=0;_Tmp8;});
# 3951
_Tmp6->hd=_Tmp7;}),({
# 3953
struct Cyc_List_List*_Tmp7=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fields);_Tmp6->tl=_Tmp7;});_Tmp6;});{
struct Cyc_Absyn_Aggrdecl*strct_decl=({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=({const char*_Tmp8="";_tag_fat(_Tmp8,sizeof(char),1U);});_Tmp7;});Cyc_Toc_make_c_struct_defn(_Tmp6,fields);});
({struct _tuple1*_Tmp6=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_Tmp6;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp8->tag=5,_Tmp8->f1=strct_decl;_Tmp8;}),0U);_Tmp6->hd=_Tmp7;}),_Tmp6->tl=Cyc_Toc_result_decls;_Tmp6;});
goto _LL6;}}}}else{if(*((int*)_Tmp5)== 0){
# 3959
if((int)f->sc != 3){
struct Cyc_Absyn_Exp*initopt=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*tag_decl=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp8->tag=0,_Tmp8->f1=tag_decl;_Tmp8;}),0U);_Tmp6->hd=_Tmp7;}),_Tmp6->tl=Cyc_Toc_result_decls;_Tmp6;});
({struct Cyc_Dict_Dict _Tmp6=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))Cyc_Dict_insert;})(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_Tmp6;});}
# 3966
goto _LL6;}else{
goto _LL6;}}_LL6:;}}
# 3970
return 0;}}}}
# 3973
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
({(int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple44*)))Cyc_Toc_use_toc_state;})(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3977
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=2U;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)(_check_null(ed->fields))->v);}
# 3983
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_Tmp0=Cyc_Toc_typ_to_c(old_typ);vd->type=_Tmp0;});
# 3987
if((int)vd->sc == 5 && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=2U;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=_check_null(vd->initializer);
if((int)vd->sc == 0){
# 3996
struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
{struct Cyc_Toc_Env*nv2=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(nv2,init);}
# 3997
;_pop_region();}else{
# 4001
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4004
void*_stmttmp74=Cyc_Absyn_compress(old_typ);void*_Tmp0=_stmttmp74;void*_Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)_Tmp0)== 4){_Tmp3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).num_elts;_Tmp1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zero_term;{void*et=_Tmp3;struct Cyc_Absyn_Exp*num_elts_opt=_Tmp2;void*zt=_Tmp1;
# 4006
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="can't initialize zero-terminated array -- size unknown";_tag_fat(_Tmp6,sizeof(char),55U);});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp5,sizeof(void*),1));});{
struct Cyc_Absyn_Exp*lhs=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_Tmp4,({
struct Cyc_Absyn_Exp*_Tmp5=num_elts_opt;Cyc_Absyn_add_exp(_Tmp5,Cyc_Absyn_signed_int_exp(-1,0U),0U);}),0U);});
# 4012
struct Cyc_Absyn_Exp*rhs=Cyc_Absyn_signed_int_exp(0,0U);
({void*_Tmp4=({struct Cyc_Absyn_Stmt*_Tmp5=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_Tmp5,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4013
s->r=_Tmp4;});}}
# 4016
goto _LL0;}}else{
goto _LL0;}_LL0:;}}
# 4023
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_Tmp0=d;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0:
 return d;case 1: _Tmp3=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_Tmp0)->f1;{struct Cyc_Tcpat_Rhs*rhs=_Tmp3;
rhs->rhs=success;return d;}default: _Tmp3=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_Tmp0)->f2;_Tmp1=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_Tmp0)->f3;{struct Cyc_List_List*path=_Tmp3;struct Cyc_List_List*sws=_Tmp2;void**d2=(void**)_Tmp1;
# 4028
({void*_Tmp4=Cyc_Toc_rewrite_decision(*d2,success);*d2=_Tmp4;});
for(1;sws != 0;sws=sws->tl){
struct _tuple34*_stmttmp75=(struct _tuple34*)sws->hd;struct _tuple34*_Tmp4=_stmttmp75;void*_Tmp5;_Tmp5=(void**)& _Tmp4->f2;{void**d2=(void**)_Tmp5;
({void*_Tmp6=Cyc_Toc_rewrite_decision(*d2,success);*d2=_Tmp6;});}}
# 4033
return d;}};}
# 4044 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4046
struct _RegionHandle _Tmp0=_new_region("rgn");struct _RegionHandle*rgn=& _Tmp0;_push_region(rgn);
{struct Cyc_Toc_Env*_Tmp1=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_Tmp1;
void*t=_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4056
struct Cyc_Absyn_Stmt*succ_stmt=s;
if(dopt != 0)
# 4060
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 4062
struct Cyc_Absyn_Switch_clause*c1=({struct Cyc_Absyn_Switch_clause*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_Tmp2->pattern=p,_Tmp2->pat_vars=0,_Tmp2->where_clause=0,_Tmp2->body=succ_stmt,_Tmp2->loc=0U;_Tmp2;});
struct Cyc_List_List*lscs=({struct Cyc_List_List*(*_Tmp2)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c;});struct _RegionHandle*_Tmp3=rgn;struct _RegionHandle*_Tmp4=rgn;_Tmp2(_Tmp3,Cyc_Toc_gen_labels,_Tmp4,({struct Cyc_Absyn_Switch_clause*_Tmp5[1];_Tmp5[0]=c1;({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist;})(rgn,_tag_fat(_Tmp5,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));});
# 4065
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4069
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4073
for(1;1;mybodies=(_check_null(mybodies))->tl){
if(mybodies == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="letdecl_to_c: couldn't find env!";_tag_fat(_Tmp4,sizeof(char),33U);});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp3,sizeof(void*),1));});{
struct _tuple42*_stmttmp76=(struct _tuple42*)(_check_null(mybodies))->hd;struct _tuple42*_Tmp2=_stmttmp76;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f3;{struct Cyc_Toc_Env*env=_Tmp4;struct Cyc_Absyn_Stmt*st=_Tmp3;
if(st == succ_stmt){Cyc_Toc_stmt_to_c(env,s);break;}}}}{
# 4080
struct Cyc_Absyn_Stmt*res=test_tree;
# 4082
for(1;mydecls != 0;mydecls=(_check_null(mydecls))->tl){
struct _tuple40*_stmttmp77=(struct _tuple40*)(_check_null(mydecls))->hd;struct _tuple40*_Tmp2=_stmttmp77;void*_Tmp3;_Tmp3=_Tmp2->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
res=({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp5->tag=0,_Tmp5->f1=vd;_Tmp5;}),0U);Cyc_Absyn_decl_stmt(_Tmp4,res,0U);});}}{
# 4087
struct Cyc_Absyn_Stmt*_Tmp2=({struct _tuple1*_Tmp3=v;void*_Tmp4=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_Tmp5=e;Cyc_Absyn_declare_stmt(_Tmp3,_Tmp4,_Tmp5,res,0U);});_npop_handler(0);return _Tmp2;}}}}}
# 4047
;_pop_region();}
# 4093
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_stmttmp78=e->r;void*_Tmp0=_stmttmp78;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 41: _Tmp3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL4;}case 20: _Tmp3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL6;}case 21: _Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL8;}case 22: _Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LLA;}case 15: _Tmp3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LLC;}case 11: _Tmp3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LLE;}case 12: _Tmp3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL10;}case 18: _Tmp3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL12;}case 5: _Tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_Tmp3;
Cyc_Toc_exptypes_to_c(e1);goto _LL0;}case 3: _Tmp3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp3;
({(void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_iter;})(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 7: _Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL18;}case 8: _Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL1A;}case 9: _Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL1C;}case 23: _Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL1E;}case 35: _Tmp3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL20;}case 4: _Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);goto _LL0;}case 6: _Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 4112
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);goto _LL0;}case 10: _Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp3;struct Cyc_List_List*es=_Tmp2;
# 4114
Cyc_Toc_exptypes_to_c(e);({(void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_iter;})(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 14: _Tmp3=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void**t=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;
({void*_Tmp4=Cyc_Toc_typ_to_c(*t);*t=_Tmp4;});Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 25: _Tmp3=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f3;_Tmp2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void**t=_Tmp3;struct Cyc_List_List*dles=_Tmp2;
# 4117
({void*_Tmp4=Cyc_Toc_typ_to_c(*t);*t=_Tmp4;});
_Tmp3=dles;goto _LL2A;}case 36: _Tmp3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL2A: {struct Cyc_List_List*dles=_Tmp3;
_Tmp3=dles;goto _LL2C;}case 26: _Tmp3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2C: {struct Cyc_List_List*dles=_Tmp3;
# 4121
for(1;dles != 0;dles=dles->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}case 19: _Tmp3=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void**t=_Tmp3;
_Tmp3=t;goto _LL30;}case 17: _Tmp3=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL30: {void**t=_Tmp3;
({void*_Tmp4=Cyc_Toc_typ_to_c(*t);*t=_Tmp4;});goto _LL0;}case 37: _Tmp3=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp3;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 34: _Tmp3=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_MallocInfo*m=_Tmp3;
# 4128
if(m->elt_type != 0)
({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));({void*_Tmp6=Cyc_Toc_typ_to_c(*_check_null(m->elt_type));*_Tmp5=_Tmp6;});_Tmp5;});m->elt_type=_Tmp4;});
Cyc_Toc_exptypes_to_c(m->num_elts);
goto _LL0;}case 40: _Tmp3=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_List_List*o=_Tmp3;struct Cyc_List_List*i=_Tmp2;
# 4133
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL0;}case 0:
 goto _LL3A;case 1: _LL3A:
 goto _LL3C;case 32: _LL3C:
 goto _LL3E;case 33: _LL3E:
 goto _LL0;case 42:
# 4141
 goto _LL42;case 2: _LL42:
 goto _LL44;case 30: _LL44:
 goto _LL46;case 31: _LL46:
 goto _LL48;case 29: _LL48:
 goto _LL4A;case 27: _LL4A:
 goto _LL4C;case 28: _LL4C:
 goto _LL4E;case 24: _LL4E:
 goto _LL50;case 13: _LL50:
 goto _LL52;case 16: _LL52:
 goto _LL54;case 39: _LL54:
 goto _LL56;default: _LL56:
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(e->loc,({const char*_Tmp4="Cyclone expression in C code";_tag_fat(_Tmp4,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 4156
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_stmttmp79=d->r;void*_Tmp0=_stmttmp79;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 4159
({void*_Tmp2=Cyc_Toc_typ_to_c(vd->type);vd->type=_Tmp2;});
if(vd->initializer != 0)Cyc_Toc_exptypes_to_c(_check_null(vd->initializer));
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 4163
({void*_Tmp2=Cyc_Toc_typ_to_c((fd->i).ret_type);(fd->i).ret_type=_Tmp2;});
{struct Cyc_List_List*args=(fd->i).args;for(0;args != 0;args=args->tl){
({void*_Tmp2=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_Tmp2;});}}
goto _LL0;}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
# 4169
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)(_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
if(((struct Cyc_Absyn_Enumfield*)fs->hd)->tag != 0)Cyc_Toc_exptypes_to_c(_check_null(((struct Cyc_Absyn_Enumfield*)fs->hd)->tag));}}
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
({void*_Tmp2=Cyc_Toc_typ_to_c(_check_null(td->defn));td->defn=_Tmp2;});goto _LL0;}case 2:
 goto _LLE;case 3: _LLE:
 goto _LL10;case 6: _LL10:
 goto _LL12;case 9: _LL12:
 goto _LL14;case 10: _LL14:
 goto _LL16;case 11: _LL16:
 goto _LL18;case 12: _LL18:
 goto _LL1A;case 4: _LL1A:
# 4182
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(d->loc,({const char*_Tmp2="Cyclone declaration in C code";_tag_fat(_Tmp2,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));case 13:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
 goto _LL0;}_LL0:;}
# 4190
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_stmttmp7A=s->r;void*_Tmp0=_stmttmp7A;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp4;
if(eopt != 0)Cyc_Toc_exptypes_to_c(eopt);goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 4196
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 5: _Tmp4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1).f1;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s=_Tmp3;
# 4198
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;_Tmp2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3).f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s=_Tmp1;
# 4200
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*scs=_Tmp3;void*dec_tree=_Tmp2;
# 4203
Cyc_Toc_exptypes_to_c(e);
for(1;scs != 0;scs=scs->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s=_Tmp3;
Cyc_Toc_decltypes_to_c(d);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;{struct Cyc_Absyn_Stmt*s=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
Cyc_Toc_stmttypes_to_c(s);Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s=_Tmp4;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 0:
 goto _LL18;case 6: _LL18:
 goto _LL1A;case 7: _LL1A:
 goto _LL1C;case 8: _LL1C:
 goto _LL0;case 11:
# 4215
({void*_Tmp5=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct));_Tmp6->tag=0;_Tmp6;});s->r=_Tmp5;});goto _LL0;default:
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(s->loc,({const char*_Tmp5="Cyclone statement in C code";_tag_fat(_Tmp5,sizeof(char),28U);}),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 4223
static void Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
for(1;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="decls_to_c: not at toplevel!";_tag_fat(_Tmp2,sizeof(char),29U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_stmttmp7B=d->r;void*_Tmp0=_stmttmp7B;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 4231
struct _tuple1*c_name=vd->name;
# 4233
if((int)vd->sc == 4)
c_name=({struct _tuple1*_Tmp2=_cycalloc(sizeof(struct _tuple1));({union Cyc_Absyn_Nmspace _Tmp3=Cyc_Absyn_Abs_n(0,1);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=(*c_name).f2;_Tmp2;});
if(vd->initializer != 0){
if((int)vd->sc == 4)vd->sc=2U;
if(cinclude)
Cyc_Toc_exptypes_to_c(_check_null(vd->initializer));else{
# 4240
Cyc_Toc_exp_to_c(nv,_check_null(vd->initializer));}}
# 4242
vd->name=c_name;
({enum Cyc_Absyn_Scope _Tmp2=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_Tmp2;});
({void*_Tmp2=Cyc_Toc_typ_to_c(vd->type);vd->type=_Tmp2;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=Cyc_Toc_result_decls;_Tmp2;});
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 4249
if((int)fd->sc == 4){
({struct _tuple1*_Tmp2=({struct _tuple1*_Tmp3=_cycalloc(sizeof(struct _tuple1));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Abs_n(0,1);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=(*fd->name).f2;_Tmp3;});fd->name=_Tmp2;});
fd->sc=2U;}
# 4253
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=Cyc_Toc_result_decls;_Tmp2;});
goto _LL0;}case 2:
 goto _LL8;case 3: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="letdecl at toplevel";_tag_fat(_Tmp4,sizeof(char),20U);});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp3,sizeof(void*),1));});case 4:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="region decl at toplevel";_tag_fat(_Tmp4,sizeof(char),24U);});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_Tmp3,sizeof(void*),1));});case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*sd=_Tmp1;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6: _Tmp1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp1;
# 4261
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
# 4264
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=Cyc_Toc_result_decls;_Tmp2;});
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 4268
td->tvs=0;
if(td->defn != 0){
({void*_Tmp2=Cyc_Toc_typ_to_c(_check_null(td->defn));td->defn=_Tmp2;});{
# 4273
void*_stmttmp7C=td->defn;void*_Tmp2=_stmttmp7C;unsigned _Tmp3;void*_Tmp4;if(_Tmp2 != 0){if(*((int*)_Tmp2)== 10){if(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f1)->r)== 1){_Tmp4=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f1)->r)->f1;_Tmp3=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f1)->loc;{struct Cyc_Absyn_Enumdecl*ed2=_Tmp4;unsigned loc=_Tmp3;
# 4275
struct Cyc_Absyn_Decl*ed=({struct Cyc_Absyn_Decl*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp6=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_Tmp7->tag=7,_Tmp7->f1=ed2;_Tmp7;});_Tmp5->r=_Tmp6;}),_Tmp5->loc=loc;_Tmp5;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=ed,_Tmp5->tl=Cyc_Toc_result_decls;_Tmp5;});
({void*_Tmp5=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp6->tag=0,({void*_Tmp7=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct));_Tmp8->tag=15,_Tmp8->f1=ed2->name,_Tmp8->f2=ed2;_Tmp8;});_Tmp6->f1=_Tmp7;}),_Tmp6->f2=0;_Tmp6;});td->defn=_Tmp5;});
goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26:
 goto _LL23;}_LL23:;}}else{
# 4282
({void*_Tmp2=(int)((struct Cyc_Absyn_Kind*)(_check_null(td->kind))->v)->kind == 2?Cyc_Toc_void_star_type(): Cyc_Absyn_void_type;td->defn=_Tmp2;});}
# 4286
if(Cyc_Flags_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=Cyc_Toc_result_decls;_Tmp2;});
goto _LL0;}case 13:
 goto _LL16;case 14: _LL16:
 goto _LL18;case 15: _LL18:
 goto _LL1A;case 16: _LL1A:
 goto _LL0;case 9: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*ds2=_Tmp1;
_Tmp1=ds2;goto _LL1E;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL1E: {struct Cyc_List_List*ds2=_Tmp1;
_Tmp1=ds2;goto _LL20;}case 11: _Tmp1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_LL20: {struct Cyc_List_List*ds2=_Tmp1;
Cyc_Toc_decls_to_c(nv,ds2,cinclude);goto _LL0;}default: _Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds2=_Tmp1;
Cyc_Toc_decls_to_c(nv,ds2,1);goto _LL0;}}_LL0:;}}}
# 4303
void Cyc_Toc_init (void){
struct Cyc_Core_NewDynamicRegion _stmttmp7D=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _Tmp0=_stmttmp7D;void*_Tmp1;_Tmp1=_Tmp0.key;{struct Cyc_Core_DynamicRegion*dyn=_Tmp1;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_Tmp2=_region_malloc(Cyc_Core_unique_region,sizeof(struct Cyc_Toc_TocStateWrap));_Tmp2->dyn=dyn,_Tmp2->state=ts;_Tmp2;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _Tmp2=34;({struct _fat_ptr**_Tmp3=_cycalloc(_check_times(_Tmp2,sizeof(struct _fat_ptr*)));_Tmp3[0]=& Cyc_Toc__throw_str,_Tmp3[1]=& Cyc_Toc_setjmp_str,_Tmp3[2]=& Cyc_Toc__push_handler_str,_Tmp3[3]=& Cyc_Toc__pop_handler_str,_Tmp3[4]=& Cyc_Toc__exn_thrown_str,_Tmp3[5]=& Cyc_Toc__npop_handler_str,_Tmp3[6]=& Cyc_Toc__check_null_str,_Tmp3[7]=& Cyc_Toc__check_known_subscript_null_str,_Tmp3[8]=& Cyc_Toc__check_known_subscript_notnull_str,_Tmp3[9]=& Cyc_Toc__check_fat_subscript_str,_Tmp3[10]=& Cyc_Toc__tag_fat_str,_Tmp3[11]=& Cyc_Toc__untag_fat_ptr_str,_Tmp3[12]=& Cyc_Toc__get_fat_size_str,_Tmp3[13]=& Cyc_Toc__get_zero_arr_size_str,_Tmp3[14]=& Cyc_Toc__fat_ptr_plus_str,_Tmp3[15]=& Cyc_Toc__zero_arr_plus_str,_Tmp3[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_Tmp3[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_Tmp3[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_Tmp3[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_Tmp3[20]=& Cyc_Toc__cycalloc_str,_Tmp3[21]=& Cyc_Toc__cyccalloc_str,_Tmp3[22]=& Cyc_Toc__cycalloc_atomic_str,_Tmp3[23]=& Cyc_Toc__cyccalloc_atomic_str,_Tmp3[24]=& Cyc_Toc__region_malloc_str,_Tmp3[25]=& Cyc_Toc__region_calloc_str,_Tmp3[26]=& Cyc_Toc__check_times_str,_Tmp3[27]=& Cyc_Toc__new_region_str,_Tmp3[28]=& Cyc_Toc__push_region_str,_Tmp3[29]=& Cyc_Toc__pop_region_str,_Tmp3[30]=& Cyc_Toc__throw_arraybounds_str,_Tmp3[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_Tmp3[32]=& Cyc_Toc__throw_match_str,_Tmp3[33]=& Cyc_Toc__fast_region_malloc_str;_Tmp3;});}),sizeof(struct _fat_ptr*),34U);}}
# 4351
void Cyc_Toc_finish (void){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_Tmp0=ts;struct Cyc_Toc_TocStateWrap*_Tmp1=Cyc_Toc_toc_state;ts=_Tmp1;Cyc_Toc_toc_state=_Tmp0;});{
struct Cyc_Toc_TocStateWrap _stmttmp7E=*_check_null(ts);struct Cyc_Toc_TocStateWrap _Tmp0=_stmttmp7E;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.dyn;_Tmp1=_Tmp0.state;{struct Cyc_Core_DynamicRegion*dyn=_Tmp2;struct Cyc_Toc_TocState*s=_Tmp1;
# 4356
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _stmttmp7F=*s;struct Cyc_Toc_TocState _Tmp3=_stmttmp7F;void*_Tmp4;_Tmp4=_Tmp3.temp_labels;{struct Cyc_Xarray_Xarray*tls=_Tmp4;
({(void(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_reuse;})(tls);}}
# 4357
;}
# 4360
Cyc_Core_free_rckey(dyn);
({(void(*)(struct Cyc_Toc_TocStateWrap*))Cyc_Core_ufree;})(ts);
# 4363
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4369
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4371
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_Tmp0=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));*_Tmp0=pop_tables;_Tmp0;});
Cyc_Toc_init();{
struct _RegionHandle _Tmp0=_new_region("start");struct _RegionHandle*start=& _Tmp0;_push_region(start);
({struct Cyc_Toc_Env*_Tmp1=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_Tmp1,ds,0);});{
struct Cyc_List_List*_Tmp1=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(Cyc_Toc_result_decls);_npop_handler(0);return _Tmp1;}
# 4374
;_pop_region();}}

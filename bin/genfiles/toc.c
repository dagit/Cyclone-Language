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
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 303
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 464 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 662 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 836
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 843
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 850
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 853
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 857
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 859
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 863
void*Cyc_Absyn_compress(void*);
# 881
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 883
extern void*Cyc_Absyn_sint_type;
# 888
extern void*Cyc_Absyn_heap_rgn_type;
# 892
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 894
extern void*Cyc_Absyn_void_type;
# 911
void*Cyc_Absyn_exn_type (void);
# 919
extern void*Cyc_Absyn_fat_bound_type;
# 923
void*Cyc_Absyn_bounds_one (void);
# 931
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 935
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 937
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 939
void*Cyc_Absyn_strct(struct _fat_ptr*);
void*Cyc_Absyn_strctq(struct _tuple1*);
void*Cyc_Absyn_unionq_type(struct _tuple1*);
# 945
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 950
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 963
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 965
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 971
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 976
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 979
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 987
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 989
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1035
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1040
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1048
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1052
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1057
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1061
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1066
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
# 1101
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1106
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1116
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1118
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1120
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1129
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
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
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_init (void);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};char Cyc_Toc_Dest[5U]="Dest";
# 68 "toc.cyc"
static void*Cyc_Toc_unimp(struct _fat_ptr str){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp675;_tmp675.tag=0,_tmp675.f1=str;_tmp675;});void*_tmp0[1];_tmp0[0]=& _tmp1;Cyc_Warn_impos2(_tag_fat(_tmp0,sizeof(void*),1));});}
# 71
static void*Cyc_Toc_toc_impos(struct _fat_ptr ap){
void*toc=(void*)({struct Cyc_Warn_String_Warn_Warg_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=0,({struct _fat_ptr _tmp6ED=({const char*_tmp4="Toc: ";_tag_fat(_tmp4,sizeof(char),6U);});_tmp5->f1=_tmp6ED;});_tmp5;});
struct _fat_ptr args=({unsigned _tmp3=_get_fat_size(ap,sizeof(void*))+ 1U;void**_tmp2=_cycalloc(_check_times(_tmp3,sizeof(void*)));({{unsigned _tmp676=_get_fat_size(ap,sizeof(void*))+ 1U;unsigned i;for(i=0;i < _tmp676;++ i){i == 0U?_tmp2[i]=toc:(_tmp2[i]=((void**)ap.curr)[(int)(i - 1U)]);}}0;});_tag_fat(_tmp2,sizeof(void*),_tmp3);});
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
return({struct Cyc_Toc_TocState*_tmpD=_region_malloc(d,sizeof(*_tmpD));
({struct Cyc_List_List**_tmp6F9=({struct Cyc_List_List**_tmp6=_region_malloc(d,sizeof(*_tmp6));*_tmp6=0;_tmp6;});_tmpD->tuple_types=_tmp6F9;}),({
struct Cyc_List_List**_tmp6F8=({struct Cyc_List_List**_tmp7=_region_malloc(d,sizeof(*_tmp7));*_tmp7=0;_tmp7;});_tmpD->anon_aggr_types=_tmp6F8;}),({
struct Cyc_Dict_Dict*_tmp6F7=({struct Cyc_Dict_Dict*_tmp8=_region_malloc(d,sizeof(*_tmp8));({struct Cyc_Dict_Dict _tmp6F6=({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty;})(d,Cyc_Absyn_qvar_cmp);*_tmp8=_tmp6F6;});_tmp8;});_tmpD->aggrs_so_far=_tmp6F7;}),({
struct Cyc_List_List**_tmp6F5=({struct Cyc_List_List**_tmp9=_region_malloc(d,sizeof(*_tmp9));*_tmp9=0;_tmp9;});_tmpD->abs_struct_types=_tmp6F5;}),({
struct Cyc_Set_Set**_tmp6F4=({struct Cyc_Set_Set**_tmpA=_region_malloc(d,sizeof(*_tmpA));({struct Cyc_Set_Set*_tmp6F3=({(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty;})(d,Cyc_Absyn_qvar_cmp);*_tmpA=_tmp6F3;});_tmpA;});_tmpD->datatypes_so_far=_tmp6F4;}),({
struct Cyc_Dict_Dict*_tmp6F2=({struct Cyc_Dict_Dict*_tmpB=_region_malloc(d,sizeof(*_tmpB));({struct Cyc_Dict_Dict _tmp6F1=({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty;})(d,Cyc_Absyn_qvar_cmp);*_tmpB=_tmp6F1;});_tmpB;});_tmpD->xdatatypes_so_far=_tmp6F2;}),({
struct Cyc_Dict_Dict*_tmp6F0=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp6EF=({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty;})(d,Cyc_Toc_qvar_tag_cmp);*_tmpC=_tmp6EF;});_tmpC;});_tmpD->qvar_tags=_tmp6F0;}),({
struct Cyc_Xarray_Xarray*_tmp6EE=({(struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty;})(d);_tmpD->temp_labels=_tmp6EE;});_tmpD;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 143
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 149
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 152
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpE=ts;struct Cyc_Toc_TocStateWrap*_tmpF=Cyc_Toc_toc_state;ts=_tmpF;Cyc_Toc_toc_state=_tmpE;});{
struct Cyc_Toc_TocStateWrap _tmp10=*_check_null(ts);struct Cyc_Toc_TocStateWrap _stmttmp0=_tmp10;struct Cyc_Toc_TocStateWrap _tmp11=_stmttmp0;void*_tmp13;void*_tmp12;_tmp12=_tmp11.dyn;_tmp13=_tmp11.state;{struct Cyc_Core_DynamicRegion*dyn=_tmp12;struct Cyc_Toc_TocState*s=_tmp13;
struct _fat_ptr _tmp14=({(struct _fat_ptr(*)(struct _fat_ptr))Cyc_Core_alias_refptr;})(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));struct _fat_ptr dyn2=_tmp14;
({struct Cyc_Toc_TocStateWrap _tmp6FA=({struct Cyc_Toc_TocStateWrap _tmp677;_tmp677.dyn=dyn,_tmp677.state=s;_tmp677;});*ts=_tmp6FA;});
({struct Cyc_Toc_TocStateWrap*_tmp15=ts;struct Cyc_Toc_TocStateWrap*_tmp16=Cyc_Toc_toc_state;ts=_tmp16;Cyc_Toc_toc_state=_tmp15;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp18=({struct _tuple15 _tmp678;_tmp678.f1=s,_tmp678.f2=arg;_tmp678;});struct _tuple15 env=_tmp18;
res=f(h,& env);}
# 160
;}
# 162
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 166
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 169
struct _tuple17 _tmp19=*env;struct _tuple17 _stmttmp1=_tmp19;struct _tuple17 _tmp1A=_stmttmp1;void*_tmp1D;void*_tmp1C;void*_tmp1B;_tmp1B=_tmp1A.f1;_tmp1C=(_tmp1A.f2)->f1;_tmp1D=(_tmp1A.f2)->f2;{struct Cyc_Toc_TocState*s=_tmp1B;struct _tuple1*q=_tmp1C;void*(*type_maker)(struct _tuple1*)=_tmp1D;
struct _tuple18**v=({(struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;})(*s->aggrs_so_far,q);
return v == 0?type_maker(q):(*(*v)).f2;}}
# 174
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp679;_tmp679.f1=q,_tmp679.f2=type_maker;_tmp679;});
return({(void*(*)(struct _tuple16*,void*(*)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state;})(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp1E[5U]="curr";
# 187 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_tmp1E,_tmp1E,_tmp1E + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp1F[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_tmp1F,_tmp1F,_tmp1F + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp20[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_tmp20,_tmp20,_tmp20 + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp21[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_tmp21,_tmp21,_tmp21 + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp22[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_tmp22,_tmp22,_tmp22 + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp23[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_tmp23,_tmp23,_tmp23 + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp24[7U]="_throw";
# 217 "toc.cyc"
static struct _fat_ptr Cyc_Toc__throw_str={_tmp24,_tmp24,_tmp24 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp25[7U]="setjmp";
static struct _fat_ptr Cyc_Toc_setjmp_str={_tmp25,_tmp25,_tmp25 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp26[14U]="_push_handler";
static struct _fat_ptr Cyc_Toc__push_handler_str={_tmp26,_tmp26,_tmp26 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp27[13U]="_pop_handler";
static struct _fat_ptr Cyc_Toc__pop_handler_str={_tmp27,_tmp27,_tmp27 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp28[12U]="_exn_thrown";
static struct _fat_ptr Cyc_Toc__exn_thrown_str={_tmp28,_tmp28,_tmp28 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp29[14U]="_npop_handler";
static struct _fat_ptr Cyc_Toc__npop_handler_str={_tmp29,_tmp29,_tmp29 + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2A[12U]="_check_null";
static struct _fat_ptr Cyc_Toc__check_null_str={_tmp2A,_tmp2A,_tmp2A + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2B[28U]="_check_known_subscript_null";
static struct _fat_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2B,_tmp2B,_tmp2B + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp2C[31U]="_check_known_subscript_notnull";
static struct _fat_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp2C,_tmp2C,_tmp2C + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp2D[21U]="_check_fat_subscript";
static struct _fat_ptr Cyc_Toc__check_fat_subscript_str={_tmp2D,_tmp2D,_tmp2D + 21U};static struct _tuple1 Cyc_Toc__check_fat_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_fat_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_subscript_bnd={0,& Cyc_Toc__check_fat_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_subscript_re={1,(void*)& Cyc_Toc__check_fat_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_subscript_ev={0,(void*)& Cyc_Toc__check_fat_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_subscript_e=& Cyc_Toc__check_fat_subscript_ev;static char _tmp2E[9U]="_fat_ptr";
static struct _fat_ptr Cyc_Toc__fat_ptr_str={_tmp2E,_tmp2E,_tmp2E + 9U};static struct _tuple1 Cyc_Toc__fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_bnd={0,& Cyc_Toc__fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_re={1,(void*)& Cyc_Toc__fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_ev={0,(void*)& Cyc_Toc__fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_e=& Cyc_Toc__fat_ptr_ev;static char _tmp2F[9U]="_tag_fat";
static struct _fat_ptr Cyc_Toc__tag_fat_str={_tmp2F,_tmp2F,_tmp2F + 9U};static struct _tuple1 Cyc_Toc__tag_fat_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_fat_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_fat_bnd={0,& Cyc_Toc__tag_fat_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_fat_re={1,(void*)& Cyc_Toc__tag_fat_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_fat_ev={0,(void*)& Cyc_Toc__tag_fat_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_fat_e=& Cyc_Toc__tag_fat_ev;static char _tmp30[15U]="_untag_fat_ptr";
static struct _fat_ptr Cyc_Toc__untag_fat_ptr_str={_tmp30,_tmp30,_tmp30 + 15U};static struct _tuple1 Cyc_Toc__untag_fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_fat_ptr_bnd={0,& Cyc_Toc__untag_fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_fat_ptr_re={1,(void*)& Cyc_Toc__untag_fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_fat_ptr_ev={0,(void*)& Cyc_Toc__untag_fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_fat_ptr_e=& Cyc_Toc__untag_fat_ptr_ev;static char _tmp31[14U]="_get_fat_size";
static struct _fat_ptr Cyc_Toc__get_fat_size_str={_tmp31,_tmp31,_tmp31 + 14U};static struct _tuple1 Cyc_Toc__get_fat_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_fat_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_fat_size_bnd={0,& Cyc_Toc__get_fat_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_fat_size_re={1,(void*)& Cyc_Toc__get_fat_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_fat_size_ev={0,(void*)& Cyc_Toc__get_fat_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_fat_size_e=& Cyc_Toc__get_fat_size_ev;static char _tmp32[23U]="_fat_ptr_decrease_size";
static struct _fat_ptr Cyc_Toc__fat_ptr_decrease_size_str={_tmp32,_tmp32,_tmp32 + 23U};static struct _tuple1 Cyc_Toc__fat_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_decrease_size_bnd={0,& Cyc_Toc__fat_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_decrease_size_re={1,(void*)& Cyc_Toc__fat_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__fat_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_decrease_size_e=& Cyc_Toc__fat_ptr_decrease_size_ev;static char _tmp33[19U]="_get_zero_arr_size";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_str={_tmp33,_tmp33,_tmp33 + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp34[24U]="_get_zero_arr_size_char";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp34,_tmp34,_tmp34 + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp35[25U]="_get_zero_arr_size_short";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp35,_tmp35,_tmp35 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp36[23U]="_get_zero_arr_size_int";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp36,_tmp36,_tmp36 + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp37[25U]="_get_zero_arr_size_float";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp37,_tmp37,_tmp37 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp38[26U]="_get_zero_arr_size_double";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp38,_tmp38,_tmp38 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp39[30U]="_get_zero_arr_size_longdouble";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp39,_tmp39,_tmp39 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp3A[28U]="_get_zero_arr_size_voidstar";static struct _fat_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp3A,_tmp3A,_tmp3A + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp3B[14U]="_fat_ptr_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_plus_str={_tmp3B,_tmp3B,_tmp3B + 14U};static struct _tuple1 Cyc_Toc__fat_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_plus_bnd={0,& Cyc_Toc__fat_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_plus_re={1,(void*)& Cyc_Toc__fat_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_plus_e=& Cyc_Toc__fat_ptr_plus_ev;static char _tmp3C[15U]="_zero_arr_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_str={_tmp3C,_tmp3C,_tmp3C + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp3D[20U]="_zero_arr_plus_char";static struct _fat_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp3D,_tmp3D,_tmp3D + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp3E[21U]="_zero_arr_plus_short";static struct _fat_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp3E,_tmp3E,_tmp3E + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp3F[19U]="_zero_arr_plus_int";static struct _fat_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp3F,_tmp3F,_tmp3F + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp40[21U]="_zero_arr_plus_float";static struct _fat_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp40,_tmp40,_tmp40 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp41[22U]="_zero_arr_plus_double";static struct _fat_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp41,_tmp41,_tmp41 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp42[26U]="_zero_arr_plus_longdouble";static struct _fat_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp42,_tmp42,_tmp42 + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp43[24U]="_zero_arr_plus_voidstar";static struct _fat_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp43,_tmp43,_tmp43 + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp44[22U]="_fat_ptr_inplace_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_str={_tmp44,_tmp44,_tmp44 + 22U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_re={1,(void*)& Cyc_Toc__fat_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_e=& Cyc_Toc__fat_ptr_inplace_plus_ev;static char _tmp45[23U]="_zero_arr_inplace_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp45,_tmp45,_tmp45 + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp46[28U]="_zero_arr_inplace_plus_char";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp46,_tmp46,_tmp46 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp47[29U]="_zero_arr_inplace_plus_short";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp47,_tmp47,_tmp47 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp48[27U]="_zero_arr_inplace_plus_int";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp48,_tmp48,_tmp48 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp49[29U]="_zero_arr_inplace_plus_float";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp49,_tmp49,_tmp49 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp4A[30U]="_zero_arr_inplace_plus_double";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp4A,_tmp4A,_tmp4A + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp4B[34U]="_zero_arr_inplace_plus_longdouble";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp4B,_tmp4B,_tmp4B + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp4C[32U]="_zero_arr_inplace_plus_voidstar";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp4C,_tmp4C,_tmp4C + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp4D[27U]="_fat_ptr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_post_str={_tmp4D,_tmp4D,_tmp4D + 27U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_post_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_post_re={1,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_post_e=& Cyc_Toc__fat_ptr_inplace_plus_post_ev;static char _tmp4E[28U]="_zero_arr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp4E,_tmp4E,_tmp4E + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp4F[33U]="_zero_arr_inplace_plus_post_char";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp4F,_tmp4F,_tmp4F + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp50[34U]="_zero_arr_inplace_plus_post_short";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp50,_tmp50,_tmp50 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp51[32U]="_zero_arr_inplace_plus_post_int";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp51,_tmp51,_tmp51 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp52[34U]="_zero_arr_inplace_plus_post_float";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp52,_tmp52,_tmp52 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp53[35U]="_zero_arr_inplace_plus_post_double";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp53,_tmp53,_tmp53 + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp54[39U]="_zero_arr_inplace_plus_post_longdouble";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp54,_tmp54,_tmp54 + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp55[37U]="_zero_arr_inplace_plus_post_voidstar";static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp55,_tmp55,_tmp55 + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp56[10U]="_cycalloc";
static struct _fat_ptr Cyc_Toc__cycalloc_str={_tmp56,_tmp56,_tmp56 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp57[11U]="_cyccalloc";
static struct _fat_ptr Cyc_Toc__cyccalloc_str={_tmp57,_tmp57,_tmp57 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp58[17U]="_cycalloc_atomic";
static struct _fat_ptr Cyc_Toc__cycalloc_atomic_str={_tmp58,_tmp58,_tmp58 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp59[18U]="_cyccalloc_atomic";
static struct _fat_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp59,_tmp59,_tmp59 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp5A[15U]="_region_malloc";
static struct _fat_ptr Cyc_Toc__region_malloc_str={_tmp5A,_tmp5A,_tmp5A + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp5B[15U]="_region_calloc";
static struct _fat_ptr Cyc_Toc__region_calloc_str={_tmp5B,_tmp5B,_tmp5B + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp5C[20U]="_fast_region_malloc";
static struct _fat_ptr Cyc_Toc__fast_region_malloc_str={_tmp5C,_tmp5C,_tmp5C + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;static char _tmp5D[13U]="_check_times";
static struct _fat_ptr Cyc_Toc__check_times_str={_tmp5D,_tmp5D,_tmp5D + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp5E[12U]="_new_region";
static struct _fat_ptr Cyc_Toc__new_region_str={_tmp5E,_tmp5E,_tmp5E + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp5F[13U]="_push_region";
static struct _fat_ptr Cyc_Toc__push_region_str={_tmp5F,_tmp5F,_tmp5F + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp60[12U]="_pop_region";
static struct _fat_ptr Cyc_Toc__pop_region_str={_tmp60,_tmp60,_tmp60 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp61[19U]="_throw_arraybounds";
static struct _fat_ptr Cyc_Toc__throw_arraybounds_str={_tmp61,_tmp61,_tmp61 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp62[13U]="_throw_match";
static struct _fat_ptr Cyc_Toc__throw_match_str={_tmp62,_tmp62,_tmp62 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp63[9U]="_rethrow";
static struct _fat_ptr Cyc_Toc__rethrow_str={_tmp63,_tmp63,_tmp63 + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;
# 257
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression (void){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 260
if((unsigned)_get_exn_thrown_e)
return _check_null(_get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_tmp6E=_cycalloc(sizeof(*_tmp6E));((_tmp6E->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp6FF=({struct _fat_ptr*_tmp69[1];({struct _fat_ptr*_tmp6FE=({struct _fat_ptr*_tmp6B=_cycalloc(sizeof(*_tmp6B));({struct _fat_ptr _tmp6FD=({const char*_tmp6A="Core";_tag_fat(_tmp6A,sizeof(char),5U);});*_tmp6B=_tmp6FD;});_tmp6B;});_tmp69[0]=_tmp6FE;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp69,sizeof(struct _fat_ptr*),1));});((_tmp6E->f1).Abs_n).val=_tmp6FF;}),({struct _fat_ptr*_tmp6FC=({struct _fat_ptr*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct _fat_ptr _tmp6FB=({const char*_tmp6C="get_exn_thrown";_tag_fat(_tmp6C,sizeof(char),15U);});*_tmp6D=_tmp6FB;});_tmp6D;});_tmp6E->f2=_tmp6FC;});_tmp6E;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=0,_tmp68->f1=qv;_tmp68;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->topt=0,({void*_tmp700=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=1,_tmp66->f1=bnd;_tmp66;});_tmp67->r=_tmp700;}),_tmp67->loc=0U,_tmp67->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp67;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=10,_tmp65->f1=fnname,_tmp65->f2=0,_tmp65->f3=0,_tmp65->f4=0;_tmp65;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->topt=0,_tmp64->r=fncall_re,_tmp64->loc=0U,_tmp64->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp64;});
return _check_null(_get_exn_thrown_e);}}
# 270
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 272
void*Cyc_Toc_void_star_type (void){
static void*t=0;
if(t == 0)
t=({void*_tmp704=Cyc_Absyn_void_type;void*_tmp703=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp702=Cyc_Absyn_empty_tqual(0U);void*_tmp701=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_tmp704,_tmp703,_tmp702,_tmp701,Cyc_Absyn_false_type);});
return _check_null(t);}
# 278
static void*Cyc_Toc_fat_ptr_type (void){
static void*t=0;
if(t == 0)
t=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(0U,& Cyc_Toc__fat_ptr_pr,0),0);
return _check_null(t);}
# 284
static void*Cyc_Toc_rgn_type (void){
static void*r=0;
if(r == 0)
r=({void*_tmp705=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp705,Cyc_Toc_mt_tq);});
return _check_null(r);}
# 291
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl (void){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 295
static struct _tuple19*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_List_List*_tmp707=({struct Cyc_List_List*_tmp70=_cycalloc(sizeof(*_tmp70));({void*_tmp706=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=1,_tmp6F->f1=name;_tmp6F;});_tmp70->hd=_tmp706;}),_tmp70->tl=0;_tmp70;});_tmp71->f1=_tmp707;}),_tmp71->f2=e;_tmp71;});}
# 299
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_tmp708=f;Cyc_Absyn_fncall_exp(_tmp708,({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_from_array;})(args),0U);});}
# 302
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp72=e->r;void*_stmttmp2=_tmp72;void*_tmp73=_stmttmp2;void*_tmp74;if(*((int*)_tmp73)== 14){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp73)->f4 == Cyc_Absyn_No_coercion){_tmp74=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp73)->f2;{struct Cyc_Absyn_Exp*e=_tmp74;
return Cyc_Toc_cast_it(t,e);}}else{goto _LL3;}}else{_LL3:
 return Cyc_Absyn_cast_exp(t,e,0,1U,0U);};}
# 311
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=14,_tmp75->f1=t,_tmp75->f2=e,_tmp75->f3=0,_tmp75->f4=1U;_tmp75;});}
# 314
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=20,_tmp76->f1=e;_tmp76;});}
# 317
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=23,_tmp77->f1=e1,_tmp77->f2=e2;_tmp77;});}
# 320
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=37,_tmp78->f1=s;_tmp78;});}
# 323
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=17,_tmp79->f1=t;_tmp79;});}
# 326
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=10,_tmp7A->f1=e,({struct Cyc_List_List*_tmp709=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_from_array;})(es);_tmp7A->f2=_tmp709;}),_tmp7A->f3=0,_tmp7A->f4=1;_tmp7A;});}
# 329
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=2,_tmp7B->f1=s1,_tmp7B->f2=s2;_tmp7B;});}
# 332
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=6,_tmp7C->f1=e1,_tmp7C->f2=e2,_tmp7C->f3=e3;_tmp7C;});}
# 335
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=21,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});}
# 338
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 341
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=36,_tmp7E->f1=tdopt,_tmp7E->f2=ds;_tmp7E;});}
# 343
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=8,_tmp7F->f1=v;_tmp7F;});}
# 347
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 351
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_tmp80=e->r;void*_stmttmp3=_tmp80;void*_tmp81=_stmttmp3;void*_tmp82;if(*((int*)_tmp81)== 20){_tmp82=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp81)->f1;{struct Cyc_Absyn_Exp*e1=_tmp82;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{
return Cyc_Absyn_aggrmember_exp(e,f,loc);};}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 379 "toc.cyc"
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 385
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
void*_tmp83=Cyc_Absyn_compress(t);void*_stmttmp4=_tmp83;void*_tmp84=_stmttmp4;switch(*((int*)_tmp84)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp84)->f1)){case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp84)->f1)->f2){case Cyc_Absyn_Char_sz:
 return fS->fchar;case Cyc_Absyn_Short_sz:
 return fS->fshort;case Cyc_Absyn_Int_sz:
 return fS->fint;default: goto _LLF;}case 2: switch((int)((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp84)->f1)->f1){case 0:
 return fS->ffloat;case 1:
 return fS->fdouble;default:
 return fS->flongdouble;}default: goto _LLF;}case 3:
 return fS->fvoidstar;default: _LLF:
# 395
({struct Cyc_Warn_String_Warn_Warg_struct _tmp86=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67C;_tmp67C.tag=0,({struct _fat_ptr _tmp70A=({const char*_tmp8A="expression type ";_tag_fat(_tmp8A,sizeof(char),17U);});_tmp67C.f1=_tmp70A;});_tmp67C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp87=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp67B;_tmp67B.tag=2,_tmp67B.f1=(void*)t;_tmp67B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67A;_tmp67A.tag=0,({struct _fat_ptr _tmp70B=({const char*_tmp89=" (not int, float, double, or pointer)";_tag_fat(_tmp89,sizeof(char),38U);});_tmp67A.f1=_tmp70B;});_tmp67A;});void*_tmp85[3];_tmp85[0]=& _tmp86,_tmp85[1]=& _tmp87,_tmp85[2]=& _tmp88;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp85,sizeof(void*),3));});};}
# 398
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8B=Cyc_Absyn_compress(_check_null(arr->topt));void*_stmttmp5=_tmp8B;void*_tmp8C=_stmttmp5;void*_tmp8D;if(*((int*)_tmp8C)== 3){_tmp8D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8C)->f1).elt_type;{void*et=_tmp8D;
return Cyc_Toc_getFunctionType(fS,et);}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67D;_tmp67D.tag=0,({struct _fat_ptr _tmp70C=({const char*_tmp90="impossible type (not pointer)";_tag_fat(_tmp90,sizeof(char),30U);});_tmp67D.f1=_tmp70C;});_tmp67D;});void*_tmp8E[1];_tmp8E[0]=& _tmp8F;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp8E,sizeof(void*),1));});};}
# 408
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp91=e->r;void*_stmttmp6=_tmp91;void*_tmp92=_stmttmp6;void*_tmp93;switch(*((int*)_tmp92)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp92)->f1).Null_c).tag == 1)
return 1;else{goto _LLF;}case 14: _tmp93=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp92)->f2;{struct Cyc_Absyn_Exp*e1=_tmp93;
return Cyc_Toc_is_zero(e1);}case 24: _tmp93=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp92)->f1;{struct Cyc_List_List*es=_tmp93;
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall;})(Cyc_Toc_is_zero,es);}case 26: _tmp93=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp92)->f1;{struct Cyc_List_List*dles=_tmp93;
_tmp93=dles;goto _LLA;}case 29: _tmp93=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp92)->f3;_LLA: {struct Cyc_List_List*dles=_tmp93;
_tmp93=dles;goto _LLC;}case 25: _tmp93=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp92)->f2;_LLC: {struct Cyc_List_List*dles=_tmp93;
_tmp93=dles;goto _LLE;}case 36: _tmp93=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp92)->f2;_LLE: {struct Cyc_List_List*dles=_tmp93;
# 417
for(1;dles != 0;dles=dles->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)dles->hd)).f2))return 0;}
return 1;}default: _LLF:
 return Cyc_Tcutil_is_zero(e);};}
# 425
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
struct _tuple1*_tmp94=x;void*_tmp96;union Cyc_Absyn_Nmspace _tmp95;_tmp95=_tmp94->f1;_tmp96=_tmp94->f2;{union Cyc_Absyn_Nmspace ns=_tmp95;struct _fat_ptr*v=_tmp96;
union Cyc_Absyn_Nmspace _tmp97=ns;void*_tmp98;switch((_tmp97.Abs_n).tag){case 4:
 _tmp98=0;goto _LL7;case 1: _tmp98=(_tmp97.Rel_n).val;_LL7: {struct Cyc_List_List*vs=_tmp98;
_tmp98=vs;goto _LL9;}case 2: _tmp98=(_tmp97.Abs_n).val;_LL9: {struct Cyc_List_List*vs=_tmp98;
_tmp98=vs;goto _LLB;}default: _tmp98=(_tmp97.C_n).val;_LLB: {struct Cyc_List_List*vs=_tmp98;
# 434
if(vs == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp9B=({struct Cyc_String_pa_PrintArg_struct _tmp67F;_tmp67F.tag=0,_tmp67F.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp67F;});struct Cyc_String_pa_PrintArg_struct _tmp9C=({struct Cyc_String_pa_PrintArg_struct _tmp67E;_tmp67E.tag=0,_tmp67E.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp67E;});void*_tmp99[2];_tmp99[0]=& _tmp9B,_tmp99[1]=& _tmp9C;({struct _fat_ptr _tmp70D=({const char*_tmp9A="%s_%s_struct";_tag_fat(_tmp9A,sizeof(char),13U);});Cyc_aprintf(_tmp70D,_tag_fat(_tmp99,sizeof(void*),2));});});{
struct _RegionHandle _tmp9D=_new_region("r");struct _RegionHandle*r=& _tmp9D;_push_region(r);
{struct _fat_ptr _tmpA4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA0=({struct Cyc_String_pa_PrintArg_struct _tmp682;_tmp682.tag=0,_tmp682.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp682;});struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp681;_tmp681.tag=0,({struct _fat_ptr _tmp710=(struct _fat_ptr)((struct _fat_ptr)({struct _RegionHandle*_tmp70F=r;struct Cyc_List_List*_tmp70E=vs;Cyc_rstr_sepstr(_tmp70F,_tmp70E,({const char*_tmpA3="_";_tag_fat(_tmpA3,sizeof(char),2U);}));}));_tmp681.f1=_tmp710;});_tmp681;});struct Cyc_String_pa_PrintArg_struct _tmpA2=({struct Cyc_String_pa_PrintArg_struct _tmp680;_tmp680.tag=0,_tmp680.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp680;});void*_tmp9E[3];_tmp9E[0]=& _tmpA0,_tmp9E[1]=& _tmpA1,_tmp9E[2]=& _tmpA2;({struct _fat_ptr _tmp711=({const char*_tmp9F="%s_%s_%s_struct";_tag_fat(_tmp9F,sizeof(char),16U);});Cyc_aprintf(_tmp711,_tag_fat(_tmp9E,sizeof(void*),3));});});_npop_handler(0);return _tmpA4;};_pop_region();}}};}}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 447
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 450
struct _tuple20 _tmpA5=*env;struct _tuple20 _stmttmp7=_tmpA5;struct _tuple20 _tmpA6=_stmttmp7;void*_tmpA8;void*_tmpA7;_tmpA7=(_tmpA6.f1)->qvar_tags;_tmpA8=_tmpA6.f2;{struct Cyc_Dict_Dict*qvs=_tmpA7;struct _tuple14*pair=_tmpA8;
struct _tuple14 _tmpA9=*pair;struct _tuple14 _stmttmp8=_tmpA9;struct _tuple14 _tmpAA=_stmttmp8;void*_tmpAC;void*_tmpAB;_tmpAB=_tmpAA.f1;_tmpAC=_tmpAA.f2;{struct _tuple1*fieldname=_tmpAB;struct _tuple1*dtname=_tmpAC;
struct _handler_cons _tmpAD;_push_handler(& _tmpAD);{int _tmpAF=0;if(setjmp(_tmpAD.handler))_tmpAF=1;if(!_tmpAF){{struct _tuple1*_tmpB0=({(struct _tuple1*(*)(struct Cyc_Dict_Dict,int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*))Cyc_Dict_lookup_other;})(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0);return _tmpB0;};_pop_handler();}else{void*_tmpAE=(void*)Cyc_Core_get_exn_thrown();void*_tmpB1=_tmpAE;void*_tmpB2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpB1)->tag == Cyc_Dict_Absent){
# 454
struct _tuple14*_tmpB3=({struct _tuple14*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->f1=fieldname,_tmpB9->f2=dtname;_tmpB9;});struct _tuple14*new_pair=_tmpB3;
struct _tuple1*_tmpB4=fieldname;void*_tmpB6;union Cyc_Absyn_Nmspace _tmpB5;_tmpB5=_tmpB4->f1;_tmpB6=_tmpB4->f2;{union Cyc_Absyn_Nmspace nmspace=_tmpB5;struct _fat_ptr*fieldvar=_tmpB6;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res=({struct _tuple1*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->f1=nmspace,({struct _fat_ptr*_tmp712=({struct _fat_ptr*_tmpB7=_cycalloc(sizeof(*_tmpB7));*_tmpB7=newvar;_tmpB7;});_tmpB8->f2=_tmp712;});_tmpB8;});
({struct Cyc_Dict_Dict _tmp713=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple14*,struct _tuple1*))Cyc_Dict_insert;})(*qvs,new_pair,res);*qvs=_tmp713;});
return res;}}else{_tmpB2=_tmpB1;{void*exn=_tmpB2;(int)_rethrow(exn);}};}}}}}
# 462
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp683;_tmp683.f1=fieldname,_tmp683.f2=dtname;_tmp683;});
return({(struct _tuple1*(*)(struct _tuple14*,struct _tuple1*(*)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state;})(& env,Cyc_Toc_collapse_qvars_body);}
# 469
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->kind=0U,_tmpBC->sc=2U,_tmpBC->tvs=0,_tmpBC->attributes=0,_tmpBC->expected_mem_kind=0,({
# 472
struct _tuple1*_tmp716=({struct _tuple1*_tmpBA=_cycalloc(sizeof(*_tmpBA));({union Cyc_Absyn_Nmspace _tmp715=Cyc_Absyn_Rel_n(0);_tmpBA->f1=_tmp715;}),_tmpBA->f2=name;_tmpBA;});_tmpBC->name=_tmp716;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp714=({struct Cyc_Absyn_AggrdeclImpl*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->exist_vars=0,_tmpBB->rgn_po=0,_tmpBB->fields=fs,_tmpBB->tagged=0;_tmpBB;});_tmpBC->impl=_tmp714;});_tmpBC;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 478
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 481
struct _tuple21 _tmpBD=*env;struct _tuple21 _stmttmp9=_tmpBD;struct _tuple21 _tmpBE=_stmttmp9;void*_tmpC0;void*_tmpBF;_tmpBF=(_tmpBE.f1)->tuple_types;_tmpC0=_tmpBE.f2;{struct Cyc_List_List**tuple_types=_tmpBF;struct Cyc_List_List*tqs0=_tmpC0;
# 483
{struct Cyc_List_List*_tmpC1=*tuple_types;struct Cyc_List_List*tts=_tmpC1;for(0;tts != 0;tts=tts->tl){
struct _tuple22*_tmpC2=(struct _tuple22*)tts->hd;struct _tuple22*_stmttmpA=_tmpC2;struct _tuple22*_tmpC3=_stmttmpA;void*_tmpC5;void*_tmpC4;_tmpC4=_tmpC3->f1;_tmpC5=_tmpC3->f2;{void*x=_tmpC4;struct Cyc_List_List*ts=_tmpC5;
struct Cyc_List_List*_tmpC6=tqs0;struct Cyc_List_List*tqs=_tmpC6;
for(1;tqs != 0 && ts != 0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple23*)tqs->hd)).f2,(void*)ts->hd))
break;}
if(tqs == 0 && ts == 0)
return x;}}}{
# 494
struct _fat_ptr*xname=({struct _fat_ptr*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct _fat_ptr _tmp718=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD5=({struct Cyc_Int_pa_PrintArg_struct _tmp684;_tmp684.tag=1,_tmp684.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp684;});void*_tmpD3[1];_tmpD3[0]=& _tmpD5;({struct _fat_ptr _tmp717=({const char*_tmpD4="_tuple%d";_tag_fat(_tmpD4,sizeof(char),9U);});Cyc_aprintf(_tmp717,_tag_fat(_tmpD3,sizeof(void*),1));});});*_tmpD6=_tmp718;});_tmpD6;});
struct Cyc_List_List*_tmpC7=0;struct Cyc_List_List*fs=_tmpC7;
struct Cyc_List_List*_tmpC8=0;struct Cyc_List_List*ts=_tmpC8;
{int i=1;for(0;tqs0 != 0;(tqs0=tqs0->tl,i ++)){
void*t=(*((struct _tuple23*)tqs0->hd)).f2;
fs=({struct Cyc_List_List*_tmpCA=_cycalloc(sizeof(*_tmpCA));({struct Cyc_Absyn_Aggrfield*_tmp71A=({struct Cyc_Absyn_Aggrfield*_tmpC9=_cycalloc(sizeof(*_tmpC9));({struct _fat_ptr*_tmp719=Cyc_Absyn_fieldname(i);_tmpC9->name=_tmp719;}),_tmpC9->tq=Cyc_Toc_mt_tq,_tmpC9->type=t,_tmpC9->width=0,_tmpC9->attributes=0,_tmpC9->requires_clause=0;_tmpC9;});_tmpCA->hd=_tmp71A;}),_tmpCA->tl=fs;_tmpCA;});
ts=({struct Cyc_List_List*_tmpCB=_region_malloc(d,sizeof(*_tmpCB));_tmpCB->hd=t,_tmpCB->tl=ts;_tmpCB;});}}
# 502
fs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fs);
ts=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(ts);{
struct Cyc_Absyn_Aggrdecl*_tmpCC=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpCC;
void*_tmpCD=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpD2=_cycalloc(sizeof(*_tmpD2));*_tmpD2=sd;_tmpD2;})),0);void*ans=_tmpCD;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpCF=_cycalloc(sizeof(*_tmpCF));({struct Cyc_Absyn_Decl*_tmp71B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->tag=5,_tmpCE->f1=sd;_tmpCE;}),0U);_tmpCF->hd=_tmp71B;}),_tmpCF->tl=Cyc_Toc_result_decls;_tmpCF;});
({struct Cyc_List_List*_tmp71D=({struct Cyc_List_List*_tmpD1=_region_malloc(d,sizeof(*_tmpD1));({struct _tuple22*_tmp71C=({struct _tuple22*_tmpD0=_region_malloc(d,sizeof(*_tmpD0));_tmpD0->f1=ans,_tmpD0->f2=ts;_tmpD0;});_tmpD1->hd=_tmp71C;}),_tmpD1->tl=*tuple_types;_tmpD1;});*tuple_types=_tmp71D;});
return ans;}}}}
# 510
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return({(void*(*)(struct Cyc_List_List*,void*(*)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state;})(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 515
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 518
struct _tuple25*_tmpD7=env;void*_tmpDA;enum Cyc_Absyn_AggrKind _tmpD9;void*_tmpD8;_tmpD8=(_tmpD7->f1)->anon_aggr_types;_tmpD9=(_tmpD7->f2)->f1;_tmpDA=(_tmpD7->f2)->f2;{struct Cyc_List_List**anon_aggr_types=_tmpD8;enum Cyc_Absyn_AggrKind ak=_tmpD9;struct Cyc_List_List*fs=_tmpDA;
# 520
{struct Cyc_List_List*_tmpDB=*anon_aggr_types;struct Cyc_List_List*ts=_tmpDB;for(0;ts != 0;ts=ts->tl){
struct _tuple26*_tmpDC=(struct _tuple26*)ts->hd;struct _tuple26*_stmttmpB=_tmpDC;struct _tuple26*_tmpDD=_stmttmpB;void*_tmpE0;enum Cyc_Absyn_AggrKind _tmpDF;void*_tmpDE;_tmpDE=_tmpDD->f1;_tmpDF=_tmpDD->f2;_tmpE0=_tmpDD->f3;{void*x=_tmpDE;enum Cyc_Absyn_AggrKind ak2=_tmpDF;struct Cyc_List_List*fs2=_tmpE0;
if((int)ak == (int)ak2 && !({(int(*)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 527
struct _fat_ptr*xname=({struct _fat_ptr*_tmpEB=_cycalloc(sizeof(*_tmpEB));({struct _fat_ptr _tmp71F=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpEA=({struct Cyc_Int_pa_PrintArg_struct _tmp685;_tmp685.tag=1,_tmp685.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp685;});void*_tmpE8[1];_tmpE8[0]=& _tmpEA;({struct _fat_ptr _tmp71E=({const char*_tmpE9="_tuple%d";_tag_fat(_tmpE9,sizeof(char),9U);});Cyc_aprintf(_tmp71E,_tag_fat(_tmpE8,sizeof(void*),1));});});*_tmpEB=_tmp71F;});_tmpEB;});
struct Cyc_Absyn_Aggrdecl*_tmpE1=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpE1;
sd->kind=ak;{
void*_tmpE2=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE7=_cycalloc(sizeof(*_tmpE7));*_tmpE7=sd;_tmpE7;})),0);void*ans=_tmpE2;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpE4=_cycalloc(sizeof(*_tmpE4));({struct Cyc_Absyn_Decl*_tmp720=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->tag=5,_tmpE3->f1=sd;_tmpE3;}),0U);_tmpE4->hd=_tmp720;}),_tmpE4->tl=Cyc_Toc_result_decls;_tmpE4;});
({struct Cyc_List_List*_tmp722=({struct Cyc_List_List*_tmpE6=_region_malloc(d,sizeof(*_tmpE6));({struct _tuple26*_tmp721=({struct _tuple26*_tmpE5=_region_malloc(d,sizeof(*_tmpE5));_tmpE5->f1=ans,_tmpE5->f2=ak,_tmpE5->f3=fs;_tmpE5;});_tmpE6->hd=_tmp721;}),_tmpE6->tl=*anon_aggr_types;_tmpE6;});*anon_aggr_types=_tmp722;});
return ans;}}}}
# 535
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return({void*(*_tmp723)(struct _tuple24*,void*(*)(struct _RegionHandle*,struct _tuple25*))=({(void*(*)(struct _tuple24*,void*(*)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state;});_tmp723(({struct _tuple24*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->f1=ak,_tmpEC->f2=fs;_tmpEC;}),Cyc_Toc_add_anon_aggr_type_body);});}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 544
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 553
struct _tuple28 _tmpED=*env;struct _tuple28 _stmttmpC=_tmpED;struct _tuple28 _tmpEE=_stmttmpC;void*_tmpF3;void*_tmpF2;void*_tmpF1;void*_tmpF0;void*_tmpEF;_tmpEF=(_tmpEE.f1)->abs_struct_types;_tmpF0=(_tmpEE.f2)->f1;_tmpF1=(_tmpEE.f2)->f2;_tmpF2=(_tmpEE.f2)->f3;_tmpF3=(_tmpEE.f2)->f4;{struct Cyc_List_List**abs_struct_types=_tmpEF;struct _tuple1*struct_name=_tmpF0;struct Cyc_List_List*type_vars=_tmpF1;struct Cyc_List_List*type_args=_tmpF2;struct Cyc_List_List*fields=_tmpF3;
# 557
{struct Cyc_List_List*_tmpF4=*abs_struct_types;struct Cyc_List_List*tts=_tmpF4;for(0;tts != 0;tts=tts->tl){
struct _tuple29*_tmpF5=(struct _tuple29*)tts->hd;struct _tuple29*_stmttmpD=_tmpF5;struct _tuple29*_tmpF6=_stmttmpD;void*_tmpF9;void*_tmpF8;void*_tmpF7;_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;_tmpF9=_tmpF6->f3;{struct _tuple1*x=_tmpF7;struct Cyc_List_List*ts2=_tmpF8;void*t=_tmpF9;
if(Cyc_Absyn_qvar_cmp(x,struct_name)== 0 &&({
int _tmp724=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(type_args);_tmp724 == ({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ts2);})){
int okay=1;
{struct Cyc_List_List*_tmpFA=type_args;struct Cyc_List_List*ts=_tmpFA;for(0;ts != 0;(ts=ts->tl,ts2=ts2->tl)){
void*_tmpFB=(void*)ts->hd;void*t=_tmpFB;
void*_tmpFC=(void*)(_check_null(ts2))->hd;void*t2=_tmpFC;
{struct Cyc_Absyn_Kind*_tmpFD=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmpE=_tmpFD;struct Cyc_Absyn_Kind*_tmpFE=_stmttmpE;switch((int)((struct Cyc_Absyn_Kind*)_tmpFE)->kind){case Cyc_Absyn_EffKind:
 goto _LLA;case Cyc_Absyn_RgnKind: _LLA:
 continue;default:
# 570
 if(Cyc_Unify_unify(t,t2)||({void*_tmp725=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_tmp725,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 575
break;}}
# 577
if(okay)
return t;}}}}{
# 583
struct _fat_ptr*xname=({struct _fat_ptr*_tmp111=_cycalloc(sizeof(*_tmp111));({struct _fat_ptr _tmp727=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp110=({struct Cyc_Int_pa_PrintArg_struct _tmp686;_tmp686.tag=1,_tmp686.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp686;});void*_tmp10E[1];_tmp10E[0]=& _tmp110;({struct _fat_ptr _tmp726=({const char*_tmp10F="_tuple%d";_tag_fat(_tmp10F,sizeof(char),9U);});Cyc_aprintf(_tmp726,_tag_fat(_tmp10E,sizeof(void*),1));});});*_tmp111=_tmp727;});_tmp111;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmpFF=0;struct Cyc_List_List*fs=_tmpFF;
# 587
({struct Cyc_List_List*_tmp729=({struct Cyc_List_List*_tmp101=_region_malloc(d,sizeof(*_tmp101));({struct _tuple29*_tmp728=({struct _tuple29*_tmp100=_region_malloc(d,sizeof(*_tmp100));_tmp100->f1=struct_name,_tmp100->f2=type_args,_tmp100->f3=x;_tmp100;});_tmp101->hd=_tmp728;}),_tmp101->tl=*abs_struct_types;_tmp101;});*abs_struct_types=_tmp729;});{
# 590
struct _RegionHandle _tmp102=_new_region("r");struct _RegionHandle*r=& _tmp102;_push_region(r);
{struct Cyc_List_List*_tmp103=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip;})(r,r,type_vars,type_args);struct Cyc_List_List*inst=_tmp103;
for(1;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp104=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp104;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 600
if((fields->tl == 0 &&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp106=_cycalloc(sizeof(*_tmp106));({void*_tmp72A=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=6,_tmp105->f1=0;_tmp105;});_tmp106->hd=_tmp72A;}),_tmp106->tl=atts;_tmp106;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 607
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->tag=4,({void*_tmp72C=Cyc_Toc_void_star_type();(_tmp107->f1).elt_type=_tmp72C;}),(_tmp107->f1).tq=Cyc_Toc_mt_tq,({
struct Cyc_Absyn_Exp*_tmp72B=Cyc_Absyn_uint_exp(0U,0U);(_tmp107->f1).num_elts=_tmp72B;}),(_tmp107->f1).zero_term=Cyc_Absyn_false_type,(_tmp107->f1).zt_loc=0U;_tmp107;});
fs=({struct Cyc_List_List*_tmp109=_cycalloc(sizeof(*_tmp109));({struct Cyc_Absyn_Aggrfield*_tmp72D=({struct Cyc_Absyn_Aggrfield*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->name=f->name,_tmp108->tq=Cyc_Toc_mt_tq,_tmp108->type=t,_tmp108->width=f->width,_tmp108->attributes=atts,_tmp108->requires_clause=0;_tmp108;});_tmp109->hd=_tmp72D;}),_tmp109->tl=fs;_tmp109;});}
# 612
fs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fs);{
struct Cyc_Absyn_Aggrdecl*_tmp10A=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmp10A;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp10C=_cycalloc(sizeof(*_tmp10C));({struct Cyc_Absyn_Decl*_tmp72E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->tag=5,_tmp10B->f1=sd;_tmp10B;}),0U);_tmp10C->hd=_tmp72E;}),_tmp10C->tl=Cyc_Toc_result_decls;_tmp10C;});{
void*_tmp10D=x;_npop_handler(0);return _tmp10D;}}}
# 591
;_pop_region();}}}}
# 618
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 622
struct _tuple27 env=({struct _tuple27 _tmp687;_tmp687.f1=struct_name,_tmp687.f2=type_vars,_tmp687.f3=type_args,_tmp687.f4=fields;_tmp687;});
return({(void*(*)(struct _tuple27*,void*(*)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state;})(& env,Cyc_Toc_add_struct_type_body);}
# 630
struct _tuple1*Cyc_Toc_temp_var (void){
return({struct _tuple1*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp731=({struct _fat_ptr*_tmp115=_cycalloc(sizeof(*_tmp115));({struct _fat_ptr _tmp730=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp114=({struct Cyc_Int_pa_PrintArg_struct _tmp688;_tmp688.tag=1,_tmp688.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp688;});void*_tmp112[1];_tmp112[0]=& _tmp114;({struct _fat_ptr _tmp72F=({const char*_tmp113="_tmp%X";_tag_fat(_tmp113,sizeof(char),7U);});Cyc_aprintf(_tmp72F,_tag_fat(_tmp112,sizeof(void*),1));});});*_tmp115=_tmp730;});_tmp115;});_tmp116->f2=_tmp731;});_tmp116;});}struct _tuple30{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 633
struct _tuple30 Cyc_Toc_temp_var_and_exp (void){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple30 _tmp689;_tmp689.f1=v,({struct Cyc_Absyn_Exp*_tmp732=Cyc_Absyn_var_exp(v,0U);_tmp689.f2=_tmp732;});_tmp689;});}struct _tuple31{struct Cyc_Toc_TocState*f1;int f2;};
# 640
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _tmp117=*env;struct _tuple31 _stmttmpF=_tmp117;struct _tuple31 _tmp118=_stmttmpF;void*_tmp119;_tmp119=(_tmp118.f1)->temp_labels;{struct Cyc_Xarray_Xarray*temp_labels=_tmp119;
int _tmp11A=Cyc_Toc_fresh_label_counter ++;int i=_tmp11A;
if(({int _tmp733=i;_tmp733 < ({(int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length;})(temp_labels);}))
return({(struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get;})(temp_labels,i);{
struct _fat_ptr*res=({struct _fat_ptr*_tmp121=_cycalloc(sizeof(*_tmp121));({struct _fat_ptr _tmp735=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp120=({struct Cyc_Int_pa_PrintArg_struct _tmp68B;_tmp68B.tag=1,_tmp68B.f1=(unsigned)i;_tmp68B;});void*_tmp11E[1];_tmp11E[0]=& _tmp120;({struct _fat_ptr _tmp734=({const char*_tmp11F="_LL%X";_tag_fat(_tmp11F,sizeof(char),6U);});Cyc_aprintf(_tmp734,_tag_fat(_tmp11E,sizeof(void*),1));});});*_tmp121=_tmp735;});_tmp121;});
if(({int _tmp736=({(int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind;})(temp_labels,res);_tmp736 != i;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68A;_tmp68A.tag=0,({struct _fat_ptr _tmp737=({const char*_tmp11D="fresh_label: add_ind returned bad index...";_tag_fat(_tmp11D,sizeof(char),43U);});_tmp68A.f1=_tmp737;});_tmp68A;});void*_tmp11B[1];_tmp11B[0]=& _tmp11C;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp11B,sizeof(void*),1));});
return res;}}}
# 650
static struct _fat_ptr*Cyc_Toc_fresh_label (void){
return({(struct _fat_ptr*(*)(int,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state;})(0,Cyc_Toc_fresh_label_body);}
# 656
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
{struct Cyc_List_List*_tmp122=(struct Cyc_List_List*)(_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp122;for(0;Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)(_check_null(fs))->hd)->name)!= 0;fs=fs->tl){
++ ans;}}
return Cyc_Absyn_signed_int_exp(ans,0U);}
# 666
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*);
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*);
# 673
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
return({struct _tuple9*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->f1=0,_tmp123->f2=(*a).f2,({void*_tmp738=Cyc_Toc_typ_to_c((*a).f3);_tmp123->f3=_tmp738;});_tmp123;});}
# 692 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp124=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp124;void*_tmp125=_stmttmp10;struct Cyc_Absyn_ArrayInfo _tmp126;if(*((int*)_tmp125)== 4){_tmp126=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp125)->f1;{struct Cyc_Absyn_ArrayInfo ai=_tmp126;
return({void*_tmp739=Cyc_Toc_typ_to_c_array(ai.elt_type);Cyc_Absyn_cstar_type(_tmp739,ai.tq);});}}else{
return Cyc_Toc_typ_to_c(t);};}
# 699
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 701
struct Cyc_Absyn_Aggrfield*_tmp127=({struct Cyc_Absyn_Aggrfield*_tmp128=_cycalloc(sizeof(*_tmp128));*_tmp128=*f;_tmp128;});struct Cyc_Absyn_Aggrfield*ans=_tmp127;
ans->type=new_type;
ans->requires_clause=0;
ans->tq=Cyc_Toc_mt_tq;
return ans;}
# 708
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}
# 712
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp129=Cyc_Absyn_compress(t);void*_stmttmp11=_tmp129;void*_tmp12A=_stmttmp11;void*_tmp12B;if(*((int*)_tmp12A)== 2){_tmp12B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp12A)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp12B;
return Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tbk);}}else{
return 0;};}
# 718
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp12C=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp12=_tmp12C;struct Cyc_Absyn_Kind*_tmp12D=_stmttmp12;if(((struct Cyc_Absyn_Kind*)_tmp12D)->kind == Cyc_Absyn_AnyKind)
return 1;else{
return 0;};}
# 724
static int Cyc_Toc_is_void_star(void*t){
void*_tmp12E=Cyc_Absyn_compress(t);void*_stmttmp13=_tmp12E;void*_tmp12F=_stmttmp13;void*_tmp130;if(*((int*)_tmp12F)== 3){_tmp130=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12F)->f1).elt_type;{void*t2=_tmp130;
return Cyc_Tcutil_is_void_type(t2);}}else{
return 0;};}
# 730
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 733
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 737
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp131=t;enum Cyc_Absyn_AggrKind _tmp136;void*_tmp135;void*_tmp134;int _tmp138;unsigned _tmp139;struct Cyc_Absyn_Tqual _tmp137;union Cyc_Absyn_AggrInfo _tmp13A;void*_tmp133;void*_tmp132;switch(*((int*)_tmp131)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1)){case 0:
 return t;case 18:
# 748
 return Cyc_Absyn_void_type;case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1)->f1).KnownDatatypefield).tag == 2){_tmp132=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1)->f1).KnownDatatypefield).val).f1;_tmp133=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp132;struct Cyc_Absyn_Datatypefield*tuf=_tmp133;
# 750
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{
# 752
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68C;_tmp68C.tag=0,({struct _fat_ptr _tmp73A=({const char*_tmp13D="unresolved DatatypeFieldType";_tag_fat(_tmp13D,sizeof(char),29U);});_tmp68C.f1=_tmp73A;});_tmp68C;});void*_tmp13B[1];_tmp13B[0]=& _tmp13C;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp13B,sizeof(void*),1));});}case 1:
# 760
 goto _LL12;case 2: _LL12:
 return t;case 20: _tmp132=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1;_tmp13A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1)->f1;_tmp133=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f2;{void*c=_tmp132;union Cyc_Absyn_AggrInfo info=_tmp13A;struct Cyc_List_List*ts=_tmp133;
# 793
{union Cyc_Absyn_AggrInfo _tmp14A=info;if((_tmp14A.UnknownAggr).tag == 1)
# 795
return ts == 0?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->tag=0,_tmp14B->f1=c,_tmp14B->f2=0;_tmp14B;});else{
goto _LL35;}_LL35:;}{
# 798
struct Cyc_Absyn_Aggrdecl*_tmp14C=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp14C;
# 800
if(ad->expected_mem_kind && ad->impl == 0)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp14E=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp68E;_tmp68E.tag=6,_tmp68E.f1=ad;_tmp68E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp14F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68D;_tmp68D.tag=0,({struct _fat_ptr _tmp73B=({const char*_tmp150=" was never defined.";_tag_fat(_tmp150,sizeof(char),20U);});_tmp68D.f1=_tmp73B;});_tmp68D;});void*_tmp14D[2];_tmp14D[0]=& _tmp14E,_tmp14D[1]=& _tmp14F;Cyc_Warn_warn2(0U,_tag_fat(_tmp14D,sizeof(void*),2));});
# 803
if((int)ad->kind == 1)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp151=ad->impl == 0?0:(_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp151;
if(fs == 0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(1;(_check_null(fs))->tl != 0;fs=fs->tl){;}{
void*_tmp152=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;void*last_type=_tmp152;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp154=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp690;_tmp690.tag=6,_tmp690.f1=ad;_tmp690;});struct Cyc_Warn_String_Warn_Warg_struct _tmp155=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68F;_tmp68F.tag=0,({struct _fat_ptr _tmp73C=({const char*_tmp156=" ended up being abstract.";_tag_fat(_tmp156,sizeof(char),26U);});_tmp68F.f1=_tmp73C;});_tmp68F;});void*_tmp153[2];_tmp153[0]=& _tmp154,_tmp153[1]=& _tmp155;Cyc_Warn_warn2(0U,_tag_fat(_tmp153,sizeof(void*),2));});{
# 814
struct _RegionHandle _tmp157=_new_region("r");struct _RegionHandle*r=& _tmp157;_push_region(r);
{struct Cyc_List_List*_tmp158=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip;})(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp158;
void*_tmp159=Cyc_Tcutil_rsubstitute(r,inst,last_type);void*t=_tmp159;
if(Cyc_Toc_is_abstract_type(t)){void*_tmp15A=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp15A;}{
void*_tmp15B=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,(_check_null(ad->impl))->fields);_npop_handler(0);return _tmp15B;}}
# 815
;_pop_region();}}
# 820
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 15: _tmp132=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1)->f1;{struct _tuple1*tdn=_tmp132;
return t;}case 16: _tmp132=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1)->f1;{struct Cyc_List_List*fs=_tmp132;
Cyc_Toc_enumfields_to_c(fs);return t;}case 4:
# 829
 return Cyc_Absyn_uint_type;case 3:
 return Cyc_Toc_rgn_type();case 17:
 return t;default:
# 834
 return Cyc_Toc_void_star_type();}case 1: _tmp132=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp131)->f2;{void**t2=_tmp132;
# 741
if(*t2 != 0)
return Cyc_Toc_typ_to_c(_check_null(*t2));
return*t2=Cyc_Absyn_sint_type;}case 2: _tmp132=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp131)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp132;
# 745
if((int)(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk))->kind == 0)
return Cyc_Absyn_void_type;
return Cyc_Toc_void_star_type();}case 3: _tmp132=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131)->f1).elt_type;_tmp137=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131)->f1).elt_tq;_tmp133=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131)->f1).ptr_atts).bounds;{void*t2=_tmp132;struct Cyc_Absyn_Tqual tq=_tmp137;void*bnds=_tmp133;
# 756
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)== 0)
return Cyc_Toc_fat_ptr_type();
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}case 4: _tmp132=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->f1).elt_type;_tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->f1).tq;_tmp133=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->f1).num_elts;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->f1).zt_loc;{void*t2=_tmp132;struct Cyc_Absyn_Tqual tq=_tmp137;struct Cyc_Absyn_Exp*e=_tmp133;unsigned ztl=_tmp139;
# 763
return({void*_tmp740=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _tmp73F=tq;struct Cyc_Absyn_Exp*_tmp73E=e;void*_tmp73D=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp740,_tmp73F,_tmp73E,_tmp73D,ztl);});}case 5: _tmp137=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp131)->f1).ret_tqual;_tmp132=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp131)->f1).ret_type;_tmp133=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp131)->f1).args;_tmp138=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp131)->f1).c_varargs;_tmp134=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp131)->f1).cyc_varargs;_tmp135=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp131)->f1).attributes;{struct Cyc_Absyn_Tqual tq2=_tmp137;void*t2=_tmp132;struct Cyc_List_List*args=_tmp133;int c_vararg=_tmp138;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp134;struct Cyc_List_List*atts=_tmp135;
# 767
struct Cyc_List_List*_tmp13E=Cyc_Atts_atts2c(atts);struct Cyc_List_List*new_atts=_tmp13E;
struct Cyc_List_List*_tmp13F=({(struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Toc_arg_to_c,args);struct Cyc_List_List*new_args=_tmp13F;
if(cyc_vararg != 0){
# 771
void*_tmp140=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));void*t=_tmp140;
struct _tuple9*_tmp141=({struct _tuple9*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->f1=cyc_vararg->name,_tmp143->f2=cyc_vararg->tq,_tmp143->f3=t;_tmp143;});struct _tuple9*vararg=_tmp141;
new_args=({struct Cyc_List_List*(*_tmp742)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp741=new_args;_tmp742(_tmp741,({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->hd=vararg,_tmp142->tl=0;_tmp142;}));});}
# 775
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->tag=5,(_tmp144->f1).tvars=0,(_tmp144->f1).effect=0,(_tmp144->f1).ret_tqual=tq2,({void*_tmp743=Cyc_Toc_typ_to_c(t2);(_tmp144->f1).ret_type=_tmp743;}),(_tmp144->f1).args=new_args,(_tmp144->f1).c_varargs=c_vararg,(_tmp144->f1).cyc_varargs=0,(_tmp144->f1).rgn_po=0,(_tmp144->f1).attributes=new_atts,(_tmp144->f1).requires_clause=0,(_tmp144->f1).requires_relns=0,(_tmp144->f1).ensures_clause=0,(_tmp144->f1).ensures_relns=0,(_tmp144->f1).return_value=0;_tmp144;});}case 6: _tmp132=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp131)->f1;{struct Cyc_List_List*tqs=_tmp132;
# 780
struct Cyc_List_List*_tmp145=0;struct Cyc_List_List*tqs2=_tmp145;
for(1;tqs != 0;tqs=tqs->tl){
tqs2=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));({struct _tuple23*_tmp745=({struct _tuple23*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->f1=(*((struct _tuple23*)tqs->hd)).f1,({void*_tmp744=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqs->hd)).f2);_tmp146->f2=_tmp744;});_tmp146;});_tmp147->hd=_tmp745;}),_tmp147->tl=tqs2;_tmp147;});}
return Cyc_Toc_add_tuple_type(({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(tqs2));}case 7: _tmp136=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp131)->f1;_tmp132=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp131)->f2;{enum Cyc_Absyn_AggrKind k=_tmp136;struct Cyc_List_List*fs=_tmp132;
# 787
struct Cyc_List_List*_tmp148=0;struct Cyc_List_List*fs2=_tmp148;
for(1;fs != 0;fs=fs->tl){
fs2=({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));({struct Cyc_Absyn_Aggrfield*_tmp747=({struct Cyc_Absyn_Aggrfield*_tmp746=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_tmp746,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_tmp149->hd=_tmp747;}),_tmp149->tl=fs2;_tmp149;});}
return({enum Cyc_Absyn_AggrKind _tmp748=k;Cyc_Toc_add_anon_aggr_type(_tmp748,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fs2));});}case 8: _tmp132=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp131)->f1;_tmp133=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp131)->f2;_tmp134=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp131)->f3;_tmp135=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp131)->f4;{struct _tuple1*tdn=_tmp132;struct Cyc_List_List*ts=_tmp133;struct Cyc_Absyn_Typedefdecl*td=_tmp134;void*topt=_tmp135;
# 826
if(topt == 0)
return ts == 0?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->tag=8,_tmp15C->f1=tdn,_tmp15C->f2=0,_tmp15C->f3=td,_tmp15C->f4=0;_tmp15C;});
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->tag=8,_tmp15D->f1=tdn,_tmp15D->f2=0,_tmp15D->f3=td,({void*_tmp749=Cyc_Toc_typ_to_c(topt);_tmp15D->f4=_tmp749;});_tmp15D;});}case 9: _tmp132=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp131)->f1;{struct Cyc_Absyn_Exp*e=_tmp132;
# 839
return t;}case 11: _tmp132=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp131)->f1;{struct Cyc_Absyn_Exp*e=_tmp132;
# 844
Cyc_Toc_exptypes_to_c(e);
return e->topt == 0?t: Cyc_Toc_typ_to_c(_check_null(e->topt));}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp131)->f1)->r)){case 0: _tmp132=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp131)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp132;
# 847
Cyc_Toc_aggrdecl_to_c(ad);
return Cyc_Toc_aggrdecl_type(ad->name,(int)ad->kind == 1?Cyc_Absyn_unionq_type: Cyc_Absyn_strctq);}case 1: _tmp132=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp131)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp132;
# 850
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _tmp132=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp131)->f1)->r)->f1;_tmp133=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp131)->f2;{struct Cyc_Absyn_Datatypedecl*dd=_tmp132;void**t=_tmp133;
# 853
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*_check_null(t));}}};}
# 858
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp15E=t;struct Cyc_Absyn_Tqual _tmp160;void*_tmp15F;if(*((int*)_tmp15E)== 4){_tmp15F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15E)->f1).elt_type;_tmp160=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15E)->f1).tq;{void*t2=_tmp15F;struct Cyc_Absyn_Tqual tq=_tmp160;
# 861
return({void*_tmp74A=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp74A,e);});}}else{
return Cyc_Toc_cast_it(t,e);};}
# 868
static int Cyc_Toc_atomic_type(void*t){
void*_tmp161=Cyc_Absyn_compress(t);void*_stmttmp14=_tmp161;void*_tmp162=_stmttmp14;void*_tmp164;void*_tmp163;switch(*((int*)_tmp162)){case 2:
 return 0;case 0: _tmp163=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp162)->f1;_tmp164=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp162)->f2;{void*c=_tmp163;struct Cyc_List_List*ts=_tmp164;
# 872
void*_tmp165=c;void*_tmp167;void*_tmp166;union Cyc_Absyn_AggrInfo _tmp168;switch(*((int*)_tmp165)){case 0:
 goto _LL15;case 1: _LL15: goto _LL17;case 2: _LL17: goto _LL19;case 4: _LL19:
 goto _LL1B;case 15: _LL1B: goto _LL1D;case 16: _LL1D: return 1;case 18:
 goto _LL21;case 3: _LL21:
 goto _LL23;case 17: _LL23:
 return 0;case 20: _tmp168=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp165)->f1;{union Cyc_Absyn_AggrInfo info=_tmp168;
# 884
{union Cyc_Absyn_AggrInfo _tmp169=info;if((_tmp169.UnknownAggr).tag == 1)
return 0;else{
goto _LL2A;}_LL2A:;}{
# 888
struct Cyc_Absyn_Aggrdecl*_tmp16A=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp16A;
if(ad->impl == 0)
return 0;
{struct Cyc_List_List*_tmp16B=(_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp16B;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp165)->f1).KnownDatatypefield).tag == 2){_tmp166=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp165)->f1).KnownDatatypefield).val).f1;_tmp167=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp165)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp166;struct Cyc_Absyn_Datatypefield*tuf=_tmp167;
# 895
{struct Cyc_List_List*_tmp16C=tuf->typs;struct Cyc_List_List*tqs=_tmp16C;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}}
return 1;}}else{goto _LL28;}default: _LL28:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp692;_tmp692.tag=0,({struct _fat_ptr _tmp74B=({const char*_tmp170="atomic_typ: bad type ";_tag_fat(_tmp170,sizeof(char),22U);});_tmp692.f1=_tmp74B;});_tmp692;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp691;_tmp691.tag=2,_tmp691.f1=(void*)t;_tmp691;});void*_tmp16D[2];_tmp16D[0]=& _tmp16E,_tmp16D[1]=& _tmp16F;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp16D,sizeof(void*),2));});};}case 5:
# 900
 return 1;case 4: _tmp163=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp162)->f1).elt_type;{void*t=_tmp163;
return Cyc_Toc_atomic_type(t);}case 7: _tmp163=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp162)->f2;{struct Cyc_List_List*fs=_tmp163;
# 903
for(1;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 6: _tmp163=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp162)->f1;{struct Cyc_List_List*tqs=_tmp163;
# 907
for(1;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}
return 1;}case 3:
 return 0;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp172=({struct Cyc_Warn_String_Warn_Warg_struct _tmp694;_tmp694.tag=0,({struct _fat_ptr _tmp74C=({const char*_tmp174="atomic_typ:  bad type ";_tag_fat(_tmp174,sizeof(char),23U);});_tmp694.f1=_tmp74C;});_tmp694;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp173=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp693;_tmp693.tag=2,_tmp693.f1=(void*)t;_tmp693;});void*_tmp171[2];_tmp171[0]=& _tmp172,_tmp171[1]=& _tmp173;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp171,sizeof(void*),2));});};}
# 916
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_tmp175=Cyc_Absyn_compress(t);void*_stmttmp15=_tmp175;void*_tmp176=_stmttmp15;void*_tmp177;union Cyc_Absyn_AggrInfo _tmp178;switch(*((int*)_tmp176)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f1)== 20){_tmp178=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp178;
# 919
struct Cyc_Absyn_Aggrdecl*_tmp179=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp179;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp695;_tmp695.tag=0,({struct _fat_ptr _tmp74D=({const char*_tmp17C="is_poly_field: type missing fields";_tag_fat(_tmp17C,sizeof(char),35U);});_tmp695.f1=_tmp74D;});_tmp695;});void*_tmp17A[1];_tmp17A[0]=& _tmp17B;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp17A,sizeof(void*),1));});
_tmp177=(_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7: _tmp177=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp176)->f2;_LL4: {struct Cyc_List_List*fs=_tmp177;
# 924
struct Cyc_Absyn_Aggrfield*_tmp17D=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp17D;
if(field == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp697;_tmp697.tag=0,({struct _fat_ptr _tmp74E=({const char*_tmp181="is_poly_field: bad field ";_tag_fat(_tmp181,sizeof(char),26U);});_tmp697.f1=_tmp74E;});_tmp697;});struct Cyc_Warn_String_Warn_Warg_struct _tmp180=({struct Cyc_Warn_String_Warn_Warg_struct _tmp696;_tmp696.tag=0,_tmp696.f1=*f;_tmp696;});void*_tmp17E[2];_tmp17E[0]=& _tmp17F,_tmp17E[1]=& _tmp180;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp17E,sizeof(void*),2));});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp183=({struct Cyc_Warn_String_Warn_Warg_struct _tmp699;_tmp699.tag=0,({struct _fat_ptr _tmp74F=({const char*_tmp185="is_poly_field: bad type ";_tag_fat(_tmp185,sizeof(char),25U);});_tmp699.f1=_tmp74F;});_tmp699;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp184=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp698;_tmp698.tag=2,_tmp698.f1=(void*)t;_tmp698;});void*_tmp182[2];_tmp182[0]=& _tmp183,_tmp182[1]=& _tmp184;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp182,sizeof(void*),2));});};}
# 935
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp186=e->r;void*_stmttmp16=_tmp186;void*_tmp187=_stmttmp16;void*_tmp189;void*_tmp188;switch(*((int*)_tmp187)){case 21: _tmp188=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp187)->f1;_tmp189=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp187)->f2;{struct Cyc_Absyn_Exp*e1=_tmp188;struct _fat_ptr*f=_tmp189;
# 938
return Cyc_Toc_is_poly_field(_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(_check_null(e->topt));}case 22: _tmp188=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp187)->f1;_tmp189=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp187)->f2;{struct Cyc_Absyn_Exp*e1=_tmp188;struct _fat_ptr*f=_tmp189;
# 941
void*_tmp18A=Cyc_Absyn_compress(_check_null(e1->topt));void*_stmttmp17=_tmp18A;void*_tmp18B=_stmttmp17;void*_tmp18C;if(*((int*)_tmp18B)== 3){_tmp18C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18B)->f1).elt_type;{void*t=_tmp18C;
# 943
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar(_check_null(e->topt));}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69B;_tmp69B.tag=0,({struct _fat_ptr _tmp750=({const char*_tmp190="is_poly_project: bad type ";_tag_fat(_tmp190,sizeof(char),27U);});_tmp69B.f1=_tmp750;});_tmp69B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp18F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp69A;_tmp69A.tag=2,_tmp69A.f1=(void*)_check_null(e1->topt);_tmp69A;});void*_tmp18D[2];_tmp18D[0]=& _tmp18E,_tmp18D[1]=& _tmp18F;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp18D,sizeof(void*),2));});};}default:
# 946
 return 0;};}
# 951
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp191[1];_tmp191[0]=s;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_tmp191,sizeof(struct Cyc_Absyn_Exp*),1));});}
# 956
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp192[2];_tmp192[0]=s,_tmp192[1]=n;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_tmp192,sizeof(struct Cyc_Absyn_Exp*),2));});}
# 961
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp193[2];_tmp193[0]=rgn,_tmp193[1]=s;Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_malloc_e,_tag_fat(_tmp193,sizeof(struct Cyc_Absyn_Exp*),2));});}
# 965
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp194[2];_tmp194[0]=rgn,_tmp194[1]=s;Cyc_Toc_fncall_exp_dl(Cyc_Toc__fast_region_malloc_e,_tag_fat(_tmp194,sizeof(struct Cyc_Absyn_Exp*),2));});}
# 969
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp195[3];_tmp195[0]=rgn,_tmp195[1]=s,_tmp195[2]=n;Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_calloc_e,_tag_fat(_tmp195,sizeof(struct Cyc_Absyn_Exp*),3));});}
# 973
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt (void){
return Cyc_Absyn_exp_stmt(({void*_tmp196=0U;Cyc_Toc_fncall_exp_dl(Cyc_Toc__throw_match_e,_tag_fat(_tmp196,sizeof(struct Cyc_Absyn_Exp*),0));}),0U);}
# 978
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 986
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp197=e->r;void*_stmttmp18=_tmp197;void*_tmp198=_stmttmp18;if(*((int*)_tmp198)== 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp198)->f1).Wstring_c).tag){case 8:
 goto _LL4;case 9: _LL4: {
# 991
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));({struct Cyc_Absyn_Decl*_tmp751=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->tag=0,_tmp199->f1=vd;_tmp199;}),0U);_tmp19A->hd=_tmp751;}),_tmp19A->tl=Cyc_Toc_result_decls;_tmp19A;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5:
# 999
 xexp=({void*_tmp752=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp752,e);});
# 1001
xplussz=({void*_tmp753=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp753,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1004
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp19B[3];({struct _tuple19*_tmp756=({struct _tuple19*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->f1=0,_tmp19C->f2=xexp;_tmp19C;});_tmp19B[0]=_tmp756;}),({
struct _tuple19*_tmp755=({struct _tuple19*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->f1=0,_tmp19D->f2=xexp;_tmp19D;});_tmp19B[1]=_tmp755;}),({
struct _tuple19*_tmp754=({struct _tuple19*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->f1=0,_tmp19E->f2=xplussz;_tmp19E;});_tmp19B[2]=_tmp754;});({
# 1004
(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp19B,sizeof(struct _tuple19*),3));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1039 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1A0=_region_malloc(r,sizeof(*_tmp1A0));_tmp1A0->break_lab=0,_tmp1A0->continue_lab=0,_tmp1A0->fallthru_info=0,_tmp1A0->toplevel=1,({int*_tmp757=({int*_tmp19F=_region_malloc(r,sizeof(*_tmp19F));*_tmp19F=0;_tmp19F;});_tmp1A0->in_lhs=_tmp757;}),_tmp1A0->rgn=r;_tmp1A0;});}
# 1043
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1A1=nv;int _tmp1A2;_tmp1A2=_tmp1A1->toplevel;{int t=_tmp1A2;
return t;}}
# 1047
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1A3=e;void*_tmp1A8;int _tmp1A7;void*_tmp1A6;void*_tmp1A5;void*_tmp1A4;_tmp1A4=_tmp1A3->break_lab;_tmp1A5=_tmp1A3->continue_lab;_tmp1A6=_tmp1A3->fallthru_info;_tmp1A7=_tmp1A3->toplevel;_tmp1A8=_tmp1A3->in_lhs;{struct _fat_ptr**b=_tmp1A4;struct _fat_ptr**c=_tmp1A5;struct Cyc_Toc_FallthruInfo*f=_tmp1A6;int t=_tmp1A7;int*lhs=_tmp1A8;
return({struct Cyc_Toc_Env*_tmp1A9=_region_malloc(r,sizeof(*_tmp1A9));_tmp1A9->break_lab=b,_tmp1A9->continue_lab=c,_tmp1A9->fallthru_info=f,_tmp1A9->toplevel=0,_tmp1A9->in_lhs=lhs,_tmp1A9->rgn=r;_tmp1A9;});}}
# 1051
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1AA=e;void*_tmp1AF;int _tmp1AE;void*_tmp1AD;void*_tmp1AC;void*_tmp1AB;_tmp1AB=_tmp1AA->break_lab;_tmp1AC=_tmp1AA->continue_lab;_tmp1AD=_tmp1AA->fallthru_info;_tmp1AE=_tmp1AA->toplevel;_tmp1AF=_tmp1AA->in_lhs;{struct _fat_ptr**b=_tmp1AB;struct _fat_ptr**c=_tmp1AC;struct Cyc_Toc_FallthruInfo*f=_tmp1AD;int t=_tmp1AE;int*lhs=_tmp1AF;
return({struct Cyc_Toc_Env*_tmp1B0=_region_malloc(r,sizeof(*_tmp1B0));_tmp1B0->break_lab=b,_tmp1B0->continue_lab=c,_tmp1B0->fallthru_info=f,_tmp1B0->toplevel=1,_tmp1B0->in_lhs=lhs,_tmp1B0->rgn=r;_tmp1B0;});}}
# 1055
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1B1=nv;void*_tmp1B2;_tmp1B2=_tmp1B1->in_lhs;{int*b=_tmp1B2;
return*b;}}
# 1059
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1B3=e;void*_tmp1B4;_tmp1B4=_tmp1B3->in_lhs;{int*lhs=_tmp1B4;
*lhs=b;}}
# 1064
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1B5=e;void*_tmp1BA;int _tmp1B9;void*_tmp1B8;void*_tmp1B7;void*_tmp1B6;_tmp1B6=_tmp1B5->break_lab;_tmp1B7=_tmp1B5->continue_lab;_tmp1B8=_tmp1B5->fallthru_info;_tmp1B9=_tmp1B5->toplevel;_tmp1BA=_tmp1B5->in_lhs;{struct _fat_ptr**b=_tmp1B6;struct _fat_ptr**c=_tmp1B7;struct Cyc_Toc_FallthruInfo*f=_tmp1B8;int t=_tmp1B9;int*lhs=_tmp1BA;
return({struct Cyc_Toc_Env*_tmp1BB=_region_malloc(r,sizeof(*_tmp1BB));_tmp1BB->break_lab=b,_tmp1BB->continue_lab=c,_tmp1BB->fallthru_info=f,_tmp1BB->toplevel=t,_tmp1BB->in_lhs=lhs,_tmp1BB->rgn=r;_tmp1BB;});}}
# 1071
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1BC=e;void*_tmp1C1;int _tmp1C0;void*_tmp1BF;void*_tmp1BE;void*_tmp1BD;_tmp1BD=_tmp1BC->break_lab;_tmp1BE=_tmp1BC->continue_lab;_tmp1BF=_tmp1BC->fallthru_info;_tmp1C0=_tmp1BC->toplevel;_tmp1C1=_tmp1BC->in_lhs;{struct _fat_ptr**b=_tmp1BD;struct _fat_ptr**c=_tmp1BE;struct Cyc_Toc_FallthruInfo*f=_tmp1BF;int t=_tmp1C0;int*lhs=_tmp1C1;
return({struct Cyc_Toc_Env*_tmp1C2=_region_malloc(r,sizeof(*_tmp1C2));_tmp1C2->break_lab=0,_tmp1C2->continue_lab=0,_tmp1C2->fallthru_info=f,_tmp1C2->toplevel=t,_tmp1C2->in_lhs=lhs,_tmp1C2->rgn=r;_tmp1C2;});}}
# 1077
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1081
struct Cyc_Toc_Env*_tmp1C3=e;void*_tmp1C8;int _tmp1C7;void*_tmp1C6;void*_tmp1C5;void*_tmp1C4;_tmp1C4=_tmp1C3->break_lab;_tmp1C5=_tmp1C3->continue_lab;_tmp1C6=_tmp1C3->fallthru_info;_tmp1C7=_tmp1C3->toplevel;_tmp1C8=_tmp1C3->in_lhs;{struct _fat_ptr**b=_tmp1C4;struct _fat_ptr**c=_tmp1C5;struct Cyc_Toc_FallthruInfo*f=_tmp1C6;int t=_tmp1C7;int*lhs=_tmp1C8;
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1CB=_region_malloc(r,sizeof(*_tmp1CB));
_tmp1CB->label=fallthru_l,_tmp1CB->binders=fallthru_binders;_tmp1CB;});
return({struct Cyc_Toc_Env*_tmp1CA=_region_malloc(r,sizeof(*_tmp1CA));({struct _fat_ptr**_tmp758=({struct _fat_ptr**_tmp1C9=_region_malloc(r,sizeof(*_tmp1C9));*_tmp1C9=break_l;_tmp1C9;});_tmp1CA->break_lab=_tmp758;}),_tmp1CA->continue_lab=c,_tmp1CA->fallthru_info=fi,_tmp1CA->toplevel=t,_tmp1CA->in_lhs=lhs,_tmp1CA->rgn=r;_tmp1CA;});}}
# 1086
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1088
struct Cyc_Toc_Env*_tmp1CC=e;void*_tmp1D1;int _tmp1D0;void*_tmp1CF;void*_tmp1CE;void*_tmp1CD;_tmp1CD=_tmp1CC->break_lab;_tmp1CE=_tmp1CC->continue_lab;_tmp1CF=_tmp1CC->fallthru_info;_tmp1D0=_tmp1CC->toplevel;_tmp1D1=_tmp1CC->in_lhs;{struct _fat_ptr**b=_tmp1CD;struct _fat_ptr**c=_tmp1CE;struct Cyc_Toc_FallthruInfo*f=_tmp1CF;int t=_tmp1D0;int*lhs=_tmp1D1;
return({struct Cyc_Toc_Env*_tmp1D3=_region_malloc(r,sizeof(*_tmp1D3));({struct _fat_ptr**_tmp759=({struct _fat_ptr**_tmp1D2=_region_malloc(r,sizeof(*_tmp1D2));*_tmp1D2=break_l;_tmp1D2;});_tmp1D3->break_lab=_tmp759;}),_tmp1D3->continue_lab=c,_tmp1D3->fallthru_info=0,_tmp1D3->toplevel=t,_tmp1D3->in_lhs=lhs,_tmp1D3->rgn=r;_tmp1D3;});}}
# 1095
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1097
struct Cyc_Toc_Env*_tmp1D4=e;void*_tmp1D9;int _tmp1D8;void*_tmp1D7;void*_tmp1D6;void*_tmp1D5;_tmp1D5=_tmp1D4->break_lab;_tmp1D6=_tmp1D4->continue_lab;_tmp1D7=_tmp1D4->fallthru_info;_tmp1D8=_tmp1D4->toplevel;_tmp1D9=_tmp1D4->in_lhs;{struct _fat_ptr**b=_tmp1D5;struct _fat_ptr**c=_tmp1D6;struct Cyc_Toc_FallthruInfo*f=_tmp1D7;int t=_tmp1D8;int*lhs=_tmp1D9;
return({struct Cyc_Toc_Env*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));_tmp1DB->break_lab=0,_tmp1DB->continue_lab=c,({struct Cyc_Toc_FallthruInfo*_tmp75A=({struct Cyc_Toc_FallthruInfo*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));_tmp1DA->label=next_l,_tmp1DA->binders=0;_tmp1DA;});_tmp1DB->fallthru_info=_tmp75A;}),_tmp1DB->toplevel=t,_tmp1DB->in_lhs=lhs,_tmp1DB->rgn=r;_tmp1DB;});}}
# 1110 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Stmt*);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Pat*,void*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*);
# 1114
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Stmt*);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Fndecl*,int);struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1117
static void Cyc_Toc_asm_iolist_types_toc(struct Cyc_List_List*l){
for(1;l != 0;l=l->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple32*)l->hd)).f2);}}
# 1122
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp1DC=e->annot;void*_stmttmp19=_tmp1DC;void*_tmp1DD=_stmttmp19;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp1DD)->tag == Cyc_InsertChecks_NoCheck)
return 0;else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp1DD)->tag == Cyc_InsertChecks_NullOnly)
goto _LL6;else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp1DD)->tag == Cyc_InsertChecks_NullAndFatBound){_LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp1DD)->tag == Cyc_InsertChecks_NullAndThinBound){_LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp1DD)->tag == Cyc_InsertChecks_FatBound)
goto _LLC;else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp1DD)->tag == Cyc_InsertChecks_ThinBound){_LLC:
 return 0;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69C;_tmp69C.tag=0,({struct _fat_ptr _tmp75B=({const char*_tmp1E0="Toc: do_null_check";_tag_fat(_tmp1E0,sizeof(char),19U);});_tmp69C.f1=_tmp75B;});_tmp69C;});void*_tmp1DE[1];_tmp1DE[0]=& _tmp1DF;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_tmp1DE,sizeof(void*),1));});}}}}}};}
# 1138
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1140
int ans;
int _tmp1E1=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp1E1;
void*_tmp1E2=Cyc_Absyn_compress(_check_null(ptr->topt));void*old_typ=_tmp1E2;
void*_tmp1E3=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp1E3;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp1E4=old_typ;void*_tmp1E8;void*_tmp1E7;struct Cyc_Absyn_Tqual _tmp1E6;void*_tmp1E5;if(*((int*)_tmp1E4)== 3){_tmp1E5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E4)->f1).elt_type;_tmp1E6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E4)->f1).elt_tq;_tmp1E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E4)->f1).ptr_atts).bounds;_tmp1E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E4)->f1).ptr_atts).zero_term;{void*ta=_tmp1E5;struct Cyc_Absyn_Tqual tq=_tmp1E6;void*b=_tmp1E7;void*zt=_tmp1E8;
# 1151
{void*_tmp1E9=annot;void*_tmp1EA;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp1E9)->tag == Cyc_InsertChecks_NoCheck){
# 1153
if(!((unsigned)({void*_tmp75C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp75C,b);}))){
# 1156
void*_tmp1EB=({void*_tmp75D=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp75D,tq);});void*newt=_tmp1EB;
({void*_tmp760=({void*_tmp75F=newt;Cyc_Toc_cast_it_r(_tmp75F,({struct Cyc_Absyn_Exp*_tmp75E=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp75E,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp760;});
ptr->topt=newt;}
# 1160
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp1E9)->tag == Cyc_InsertChecks_NullOnly){
# 1163
if(!((unsigned)({void*_tmp761=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp761,b);}))){
# 1165
void*_tmp1EC=({void*_tmp762=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp762,tq);});void*newt=_tmp1EC;
({void*_tmp765=({void*_tmp764=newt;Cyc_Toc_cast_it_r(_tmp764,({struct Cyc_Absyn_Exp*_tmp763=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp763,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp765;});
ptr->topt=newt;
# 1172
if(index != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69D;_tmp69D.tag=0,({struct _fat_ptr _tmp766=({const char*_tmp1EF="subscript of ? with no bounds check but need null check";_tag_fat(_tmp1EF,sizeof(char),56U);});_tmp69D.f1=_tmp766;});_tmp69D;});void*_tmp1ED[1];_tmp1ED[0]=& _tmp1EE;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp1ED,sizeof(void*),1));});}
# 1176
({void*_tmp768=({struct Cyc_Absyn_Exp*_tmp1F0[1];({struct Cyc_Absyn_Exp*_tmp767=Cyc_Absyn_new_exp(ptr->r,0U);_tmp1F0[0]=_tmp767;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_tmp1F0,sizeof(struct Cyc_Absyn_Exp*),1));})->r;ptr->r=_tmp768;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp1E9)->tag == Cyc_InsertChecks_NullAndFatBound)
goto _LLD;else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp1E9)->tag == Cyc_InsertChecks_FatBound){_LLD: {
# 1182
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp76C=({void*_tmp76B=newt;Cyc_Toc_cast_it_r(_tmp76B,({struct Cyc_Absyn_Exp*_tmp1F1[3];({
struct Cyc_Absyn_Exp*_tmp76A=Cyc_Absyn_new_exp(ptr->r,0U);_tmp1F1[0]=_tmp76A;}),({
struct Cyc_Absyn_Exp*_tmp769=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp1F1[1]=_tmp769;}),_tmp1F1[2]=ind;Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_fat_subscript_e,_tag_fat(_tmp1F1,sizeof(struct Cyc_Absyn_Exp*),3));}));});
# 1185
ptr->r=_tmp76C;});
# 1189
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp1E9)->tag == Cyc_InsertChecks_NullAndThinBound){_tmp1EA=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp1E9)->f1;{struct Cyc_Absyn_Exp*bd=_tmp1EA;
# 1193
fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp1EA=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp1E9)->tag == Cyc_InsertChecks_ThinBound){_tmp1EA=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp1E9)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_tmp1EA;
# 1196
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1201
struct _tuple13 _tmp1F2=Cyc_Evexp_eval_const_uint_exp(bd);struct _tuple13 _stmttmp1A=_tmp1F2;struct _tuple13 _tmp1F3=_stmttmp1A;int _tmp1F5;unsigned _tmp1F4;_tmp1F4=_tmp1F3.f1;_tmp1F5=_tmp1F3.f2;{unsigned i=_tmp1F4;int valid=_tmp1F5;
if((!valid || i != 1U)|| !Cyc_Tcutil_is_zeroterm_pointer_type(_check_null(ptr->topt))){
# 1204
({void*_tmp770=({void*_tmp76F=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp76F,({struct Cyc_Absyn_Exp*_tmp1F6[4];({
struct Cyc_Absyn_Exp*_tmp76E=Cyc_Absyn_new_exp(ptr->r,0U);_tmp1F6[0]=_tmp76E;}),_tmp1F6[1]=bd,({
struct Cyc_Absyn_Exp*_tmp76D=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp1F6[2]=_tmp76D;}),_tmp1F6[3]=ind;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_tmp1F6,sizeof(struct Cyc_Absyn_Exp*),4));}));});
# 1204
ptr->r=_tmp770;});
# 1207
ans=1;}else{
# 1210
if(Cyc_Toc_is_zero(bd)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1213
({void*_tmp772=({struct Cyc_Absyn_Exp*_tmp1F7[1];({struct Cyc_Absyn_Exp*_tmp771=Cyc_Absyn_new_exp(ptr->r,0U);_tmp1F7[0]=_tmp771;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_tmp1F7,sizeof(struct Cyc_Absyn_Exp*),1));})->r;ptr->r=_tmp772;});
ans=0;}else{
# 1218
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp775=({void*_tmp774=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp774,({struct Cyc_Absyn_Exp*_tmp1F8[3];({
struct Cyc_Absyn_Exp*_tmp773=Cyc_Absyn_new_exp(ptr->r,0U);_tmp1F8[0]=_tmp773;}),_tmp1F8[1]=bd,_tmp1F8[2]=ind;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_tmp1F8,sizeof(struct Cyc_Absyn_Exp*),3));}));});
# 1219
ptr->r=_tmp775;});
# 1221
ans=1;}}
# 1224
goto _LL5;}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69E;_tmp69E.tag=0,({struct _fat_ptr _tmp776=({const char*_tmp1FB="FIX: ptr_use_to_c, bad annotation";_tag_fat(_tmp1FB,sizeof(char),34U);});_tmp69E.f1=_tmp776;});_tmp69E;});void*_tmp1F9[1];_tmp1F9[0]=& _tmp1FA;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp1F9,sizeof(void*),1));});}}}}}}_LL5:;}
# 1227
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69F;_tmp69F.tag=0,({struct _fat_ptr _tmp777=({const char*_tmp1FE="ptr_use_to_c: non-pointer-type";_tag_fat(_tmp1FE,sizeof(char),31U);});_tmp69F.f1=_tmp777;});_tmp69F;});void*_tmp1FC[1];_tmp1FC[0]=& _tmp1FD;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp1FC,sizeof(void*),1));});};}}
# 1233
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp200=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A0;_tmp6A0.tag=0,({struct _fat_ptr _tmp778=({const char*_tmp201="Missing type in primop ";_tag_fat(_tmp201,sizeof(char),24U);});_tmp6A0.f1=_tmp778;});_tmp6A0;});void*_tmp1FF[1];_tmp1FF[0]=& _tmp200;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp1FF,sizeof(void*),1));});
return _check_null(e->topt);}
# 1237
static struct _tuple23*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple23*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->f1=Cyc_Toc_mt_tq,({void*_tmp779=Cyc_Toc_typ_to_c(_check_null(e->topt));_tmp202->f2=_tmp779;});_tmp202;});}
# 1242
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp203=e->r;void*_stmttmp1B=_tmp203;void*_tmp204=_stmttmp1B;int _tmp206;void*_tmp205;switch(*((int*)_tmp204)){case 26: _tmp205=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp204)->f1;{struct Cyc_List_List*dles=_tmp205;
# 1245
{struct Cyc_List_List*_tmp207=dles;struct Cyc_List_List*dles2=_tmp207;for(0;dles2 != 0;dles2=dles2->tl){
if((*((struct _tuple19*)dles2->hd)).f1 != 0)
({int(*_tmp77A)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Toc_unimp;});_tmp77A(({const char*_tmp208="array designators for abstract-field initialization";_tag_fat(_tmp208,sizeof(char),52U);}));});}}
_tmp205=Cyc_Absyn_signed_int_exp(({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(dles),0U);_tmp206=0;goto _LL4;}case 27: _tmp205=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp204)->f2;_tmp206=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp204)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_tmp205;int zt=_tmp206;
_tmp205=bd;_tmp206=zt;goto _LL6;}case 28: _tmp205=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp204)->f1;_tmp206=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp204)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_tmp205;int zt=_tmp206;
# 1251
bd=Cyc_Absyn_copy_exp(bd);
return zt?({struct Cyc_Absyn_Exp*_tmp77B=bd;Cyc_Absyn_add_exp(_tmp77B,Cyc_Absyn_uint_exp(1U,0U),0U);}): bd;}default:
 return 0;};}
# 1260
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1268
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp209=e->r;void*_stmttmp1C=_tmp209;void*_tmp20A=_stmttmp1C;void*_tmp20B;if(*((int*)_tmp20A)== 29){_tmp20B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp20A)->f3;{struct Cyc_List_List*dles2=_tmp20B;
dles=dles2;goto _LL0;}}else{
return 0;}_LL0:;}
# 1274
{void*_tmp20C=Cyc_Absyn_compress(_check_null(e->topt));void*_stmttmp1D=_tmp20C;void*_tmp20D=_stmttmp1D;void*_tmp20E;if(*((int*)_tmp20D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20D)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20D)->f1)->f1).KnownAggr).tag == 2){_tmp20E=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20D)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp20E;
# 1276
if(ad->impl == 0 ||(int)ad->kind == 1)
return 0;
field_types=(_check_null(ad->impl))->fields;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 1282
 return 0;}_LL5:;}
# 1284
if(field_types == 0)
return 0;
for(1;(_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp20F=(struct Cyc_Absyn_Aggrfield*)field_types->hd;struct Cyc_Absyn_Aggrfield*last_type_field=_tmp20F;
for(1;dles != 0;dles=dles->tl){
struct _tuple19*_tmp210=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp1E=_tmp210;struct _tuple19*_tmp211=_stmttmp1E;void*_tmp213;void*_tmp212;_tmp212=_tmp211->f1;_tmp213=_tmp211->f2;{struct Cyc_List_List*ds=_tmp212;struct Cyc_Absyn_Exp*e2=_tmp213;
struct _fat_ptr*_tmp214=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp214;
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*_tmp215=Cyc_Toc_get_varsizeexp(e2);struct Cyc_Absyn_Exp*nested_ans=_tmp215;
if(nested_ans != 0)
return nested_ans;{
void*_tmp216=Cyc_Absyn_compress(last_type_field->type);void*_stmttmp1F=_tmp216;void*_tmp217=_stmttmp1F;void*_tmp219;void*_tmp218;if(*((int*)_tmp217)== 4){_tmp218=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp217)->f1).elt_type;_tmp219=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp217)->f1).num_elts;{void*elt_type=_tmp218;struct Cyc_Absyn_Exp*type_bd=_tmp219;
# 1299
if(type_bd == 0 || !Cyc_Toc_is_zero(type_bd))
return 0;
# 1305
return({struct Cyc_Absyn_Exp*_tmp77D=({struct Cyc_Absyn_Exp*_tmp21A[2];_tmp21A[0]=
_check_null(Cyc_Toc_array_length_exp(e2)),({
struct Cyc_Absyn_Exp*_tmp77C=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_tmp21A[1]=_tmp77C;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_times_e,_tag_fat(_tmp21A,sizeof(struct Cyc_Absyn_Exp*),2));});
# 1305
Cyc_Absyn_add_exp(_tmp77D,
# 1308
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}}else{
return 0;};}}}}
# 1313
({struct Cyc_Warn_String_Warn_Warg_struct _tmp21C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A1;_tmp6A1.tag=0,({struct _fat_ptr _tmp77E=({const char*_tmp21D="get_varsizeexp: did not find last struct field";_tag_fat(_tmp21D,sizeof(char),47U);});_tmp6A1.f1=_tmp77E;});_tmp6A1;});void*_tmp21B[1];_tmp21B[0]=& _tmp21C;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp21B,sizeof(void*),1));});}}
# 1316
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
struct Cyc_List_List*_tmp21E=(_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp21E;
{int i=1;for(0;fs != 0;(fs=fs->tl,++ i)){
if(Cyc_strcmp((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,(struct _fat_ptr)*f)== 0)
return i;}}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp220=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A4;_tmp6A4.tag=0,({struct _fat_ptr _tmp77F=({const char*_tmp224="get_member_offset ";_tag_fat(_tmp224,sizeof(char),19U);});_tmp6A4.f1=_tmp77F;});_tmp6A4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp221=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A3;_tmp6A3.tag=0,_tmp6A3.f1=*f;_tmp6A3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp222=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A2;_tmp6A2.tag=0,({struct _fat_ptr _tmp780=({const char*_tmp223=" failed";_tag_fat(_tmp223,sizeof(char),8U);});_tmp6A2.f1=_tmp780;});_tmp6A2;});void*_tmp21F[3];_tmp21F[0]=& _tmp220,_tmp21F[1]=& _tmp221,_tmp21F[2]=& _tmp222;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp21F,sizeof(void*),3));});}struct _tuple33{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1325
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple33*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1328
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1331
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->tag=5,_tmp225->f1=e1,_tmp225->f2=incr;_tmp225;}),0U);}
# 1335
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,void*),void*);
# 1344
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1346
void*_tmp226=e1->r;void*_stmttmp20=_tmp226;void*_tmp227=_stmttmp20;int _tmp22B;int _tmp22A;void*_tmp229;void*_tmp228;switch(*((int*)_tmp227)){case 37: _tmp228=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Stmt*s=_tmp228;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14: _tmp228=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{void*t=_tmp228;struct Cyc_Absyn_Exp*e=_tmp229;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21: _tmp228=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227)->f2;_tmp22A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227)->f3;_tmp22B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227)->f4;{struct Cyc_Absyn_Exp*e=_tmp228;struct _fat_ptr*fld=_tmp229;int is_tagged=_tmp22A;int is_read=_tmp22B;
# 1351
e1->r=e->r;
({struct Cyc_Absyn_Exp*_tmp783=e1;struct Cyc_List_List*_tmp782=({struct Cyc_List_List*_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->hd=fld,_tmp22C->tl=fs;_tmp22C;});struct Cyc_Absyn_Exp*(*_tmp781)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp783,_tmp782,_tmp781,f_env);});
goto _LL0;}default:  {
# 1359
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1361
for(1;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp784=(f(e1_copy,f_env))->r;e1->r=_tmp784;});
goto _LL0;}}_LL0:;}
# 1367
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1369
void*_tmp22D=s->r;void*_stmttmp21=_tmp22D;void*_tmp22E=_stmttmp21;void*_tmp230;void*_tmp22F;switch(*((int*)_tmp22E)){case 1: _tmp22F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp22E)->f1;{struct Cyc_Absyn_Exp*e1=_tmp22F;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12: _tmp22F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp22E)->f1;_tmp230=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp22E)->f2;{struct Cyc_Absyn_Decl*d=_tmp22F;struct Cyc_Absyn_Stmt*s2=_tmp230;
# 1372
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2: _tmp22F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp22E)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp22F;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp232=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A6;_tmp6A6.tag=0,({struct _fat_ptr _tmp785=({const char*_tmp234="lvalue_assign_stmt: ";_tag_fat(_tmp234,sizeof(char),21U);});_tmp6A6.f1=_tmp785;});_tmp6A6;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp233=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp6A5;_tmp6A5.tag=5,_tmp6A5.f1=s;_tmp6A5;});void*_tmp231[2];_tmp231[0]=& _tmp232,_tmp231[1]=& _tmp233;Cyc_Toc_toc_impos(_tag_fat(_tmp231,sizeof(void*),2));});}_LL0:;}
# 1380
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*);
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp235=e->r;void*_stmttmp22=_tmp235;void*_tmp236=_stmttmp22;void*_tmp238;void*_tmp237;switch(*((int*)_tmp236)){case 14: _tmp237=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp236)->f1;_tmp238=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp236)->f2;{void**t=_tmp237;struct Cyc_Absyn_Exp**e1=(struct Cyc_Absyn_Exp**)_tmp238;
# 1384
({struct Cyc_Absyn_Exp*_tmp786=Cyc_Toc_push_address_exp(*e1);*e1=_tmp786;});
({void*_tmp787=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_tmp787;});
return e;}case 20: _tmp237=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp236)->f1;{struct Cyc_Absyn_Exp*e1=_tmp237;
return e1;}case 37: _tmp237=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp236)->f1;{struct Cyc_Absyn_Stmt*s=_tmp237;
# 1390
Cyc_Toc_push_address_stmt(s);return e;}default:
# 1392
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A8;_tmp6A8.tag=0,({struct _fat_ptr _tmp788=({const char*_tmp23C="can't take & of exp ";_tag_fat(_tmp23C,sizeof(char),21U);});_tmp6A8.f1=_tmp788;});_tmp6A8;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp23B=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp6A7;_tmp6A7.tag=4,_tmp6A7.f1=e;_tmp6A7;});void*_tmp239[2];_tmp239[0]=& _tmp23A,_tmp239[1]=& _tmp23B;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp239,sizeof(void*),2));});};}
# 1396
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp23D=s->r;void*_stmttmp23=_tmp23D;void*_tmp23E=_stmttmp23;void*_tmp23F;switch(*((int*)_tmp23E)){case 2: _tmp23F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp23E)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp23F;
_tmp23F=s2;goto _LL4;}case 12: _tmp23F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp23E)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp23F;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1: _tmp23F=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp23E)->f1;{struct Cyc_Absyn_Exp**e=(struct Cyc_Absyn_Exp**)_tmp23F;
({struct Cyc_Absyn_Exp*_tmp789=Cyc_Toc_push_address_exp(*e);*e=_tmp789;});goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp241=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6AA;_tmp6AA.tag=0,({struct _fat_ptr _tmp78A=({const char*_tmp243="can't take & of stmt ";_tag_fat(_tmp243,sizeof(char),22U);});_tmp6AA.f1=_tmp78A;});_tmp6AA;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp242=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp6A9;_tmp6A9.tag=5,_tmp6A9.f1=s;_tmp6A9;});void*_tmp240[2];_tmp240[0]=& _tmp241,_tmp240[1]=& _tmp242;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp240,sizeof(void*),2));});}_LL0:;}
# 1408
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1420
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type,Cyc_Absyn_false_type);
void*_tmp244=Cyc_Toc_typ_to_c(elt_type);void*c_elt_type=_tmp244;
void*_tmp245=Cyc_Toc_typ_to_c(fat_ptr_type);void*c_fat_ptr_type=_tmp245;
void*_tmp246=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);void*c_ptr_type=_tmp246;
struct Cyc_Core_Opt*_tmp247=({struct Cyc_Core_Opt*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->v=c_ptr_type;_tmp279;});struct Cyc_Core_Opt*c_ptr_type_opt=_tmp247;
struct Cyc_Absyn_Exp*xinit;
{void*_tmp248=e1->r;void*_stmttmp24=_tmp248;void*_tmp249=_stmttmp24;void*_tmp24B;void*_tmp24A;switch(*((int*)_tmp249)){case 20: _tmp24A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp249)->f1;{struct Cyc_Absyn_Exp*ea=_tmp24A;
# 1428
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,3U,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1433
Cyc_Toc_exp_to_c(nv,ea);
xinit=ea;
goto _LL0;}case 23: _tmp24A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp249)->f1;_tmp24B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp249)->f2;{struct Cyc_Absyn_Exp*ea=_tmp24A;struct Cyc_Absyn_Exp*eb=_tmp24B;
# 1437
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,3U,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1442
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_tmp24C[3];_tmp24C[0]=ea,({
struct Cyc_Absyn_Exp*_tmp78B=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp24C[1]=_tmp78B;}),_tmp24C[2]=eb;Cyc_Toc_fncall_exp_dl(Cyc_Toc__fat_ptr_plus_e,_tag_fat(_tmp24C,sizeof(struct Cyc_Absyn_Exp*),3));});
goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp24E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6AB;_tmp6AB.tag=0,({struct _fat_ptr _tmp78C=({const char*_tmp24F="found bad lhs for zero-terminated pointer assignment";_tag_fat(_tmp24F,sizeof(char),53U);});_tmp6AB.f1=_tmp78C;});_tmp6AB;});void*_tmp24D[1];_tmp24D[0]=& _tmp24E;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp24D,sizeof(void*),1));});}_LL0:;}{
# 1448
struct _tuple1*_tmp250=Cyc_Toc_temp_var();struct _tuple1*x=_tmp250;
struct _RegionHandle _tmp251=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp251;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp252=({struct Cyc_Absyn_Vardecl*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->sc=2U,_tmp278->name=x,_tmp278->varloc=0U,_tmp278->tq=Cyc_Toc_mt_tq,_tmp278->type=c_fat_ptr_type,_tmp278->initializer=xinit,_tmp278->rgn=0,_tmp278->attributes=0,_tmp278->escapes=0,_tmp278->is_proto=0;_tmp278;});struct Cyc_Absyn_Vardecl*x_vd=_tmp252;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp253=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->tag=4,_tmp277->f1=x_vd;_tmp277;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd=_tmp253;
struct Cyc_Absyn_Exp*_tmp254=Cyc_Absyn_varb_exp((void*)x_bnd,0U);struct Cyc_Absyn_Exp*x_exp=_tmp254;
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp255=Cyc_Absyn_deref_exp(x_exp,0U);struct Cyc_Absyn_Exp*deref_x=_tmp255;
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*_tmp256=Cyc_Toc_temp_var();struct _tuple1*y=_tmp256;
struct Cyc_Absyn_Vardecl*_tmp257=({struct Cyc_Absyn_Vardecl*_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276->sc=2U,_tmp276->name=y,_tmp276->varloc=0U,_tmp276->tq=Cyc_Toc_mt_tq,_tmp276->type=c_elt_type,_tmp276->initializer=deref_x,_tmp276->rgn=0,_tmp276->attributes=0,_tmp276->escapes=0,_tmp276->is_proto=0;_tmp276;});struct Cyc_Absyn_Vardecl*y_vd=_tmp257;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp258=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275->tag=4,_tmp275->f1=y_vd;_tmp275;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd=_tmp258;
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp259=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y_exp=_tmp259;
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _tmp78E=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp78D=y_exp;Cyc_Absyn_prim2_exp(_tmp78E,_tmp78D,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1469
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp25A=Cyc_Toc_temp_var();struct _tuple1*z=_tmp25A;
struct Cyc_Absyn_Vardecl*_tmp25B=({struct Cyc_Absyn_Vardecl*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->sc=2U,_tmp274->name=z,_tmp274->varloc=0U,_tmp274->tq=Cyc_Toc_mt_tq,_tmp274->type=c_elt_type,_tmp274->initializer=z_init,_tmp274->rgn=0,_tmp274->attributes=0,_tmp274->escapes=0,_tmp274->is_proto=0;_tmp274;});struct Cyc_Absyn_Vardecl*z_vd=_tmp25B;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp25C=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->tag=4,_tmp273->f1=z_vd;_tmp273;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd=_tmp25C;
# 1475
struct Cyc_Absyn_Exp*_tmp25D=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y2_exp=_tmp25D;y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp25E=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero1_exp=_tmp25E;
struct Cyc_Absyn_Exp*_tmp25F=Cyc_Absyn_prim2_exp(5U,y2_exp,zero1_exp,0U);struct Cyc_Absyn_Exp*comp1_exp=_tmp25F;
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1482
struct Cyc_Absyn_Exp*_tmp260=Cyc_Absyn_varb_exp((void*)z_bnd,0U);struct Cyc_Absyn_Exp*z_exp=_tmp260;z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp261=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero2_exp=_tmp261;
struct Cyc_Absyn_Exp*_tmp262=Cyc_Absyn_prim2_exp(6U,z_exp,zero2_exp,0U);struct Cyc_Absyn_Exp*comp2_exp=_tmp262;
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1489
struct Cyc_List_List*_tmp263=({struct Cyc_Absyn_Exp*_tmp272[2];({struct Cyc_Absyn_Exp*_tmp790=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_tmp272[0]=_tmp790;}),({
struct Cyc_Absyn_Exp*_tmp78F=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp272[1]=_tmp78F;});({
# 1489
(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp272,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_List_List*xsizeargs=_tmp263;
# 1491
struct Cyc_Absyn_Exp*_tmp264=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*oneexp=_tmp264;
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp791=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(5U,_tmp791,oneexp,0U);});{
# 1496
struct Cyc_Absyn_Exp*_tmp265=({struct Cyc_Absyn_Exp*_tmp792=xsize;Cyc_Absyn_and_exp(_tmp792,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});struct Cyc_Absyn_Exp*comp_exp=_tmp265;
struct Cyc_Absyn_Stmt*_tmp266=Cyc_Absyn_exp_stmt(({void*_tmp271=0U;Cyc_Toc_fncall_exp_dl(Cyc_Toc__throw_arraybounds_e,_tag_fat(_tmp271,sizeof(struct Cyc_Absyn_Exp*),0));}),0U);struct Cyc_Absyn_Stmt*thr_stmt=_tmp266;
struct Cyc_Absyn_Exp*_tmp267=({struct Cyc_Absyn_Exp*_tmp793=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_tmp793,Cyc_Toc_curr_sp,0U);});struct Cyc_Absyn_Exp*xcurr=_tmp267;
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*_tmp268=Cyc_Absyn_deref_exp(xcurr,0U);struct Cyc_Absyn_Exp*deref_xcurr=_tmp268;
struct Cyc_Absyn_Exp*_tmp269=({struct Cyc_Absyn_Exp*_tmp794=deref_xcurr;Cyc_Absyn_assign_exp(_tmp794,Cyc_Absyn_var_exp(z,0U),0U);});struct Cyc_Absyn_Exp*asn_exp=_tmp269;
struct Cyc_Absyn_Stmt*_tmp26A=Cyc_Absyn_exp_stmt(asn_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp26A;
s=({struct Cyc_Absyn_Stmt*_tmp797=({struct Cyc_Absyn_Exp*_tmp796=comp_exp;struct Cyc_Absyn_Stmt*_tmp795=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_tmp796,_tmp795,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp797,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp799=({struct Cyc_Absyn_Decl*_tmp26C=_cycalloc(sizeof(*_tmp26C));({void*_tmp798=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->tag=0,_tmp26B->f1=z_vd;_tmp26B;});_tmp26C->r=_tmp798;}),_tmp26C->loc=0U;_tmp26C;});Cyc_Absyn_decl_stmt(_tmp799,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp79B=({struct Cyc_Absyn_Decl*_tmp26E=_cycalloc(sizeof(*_tmp26E));({void*_tmp79A=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->tag=0,_tmp26D->f1=y_vd;_tmp26D;});_tmp26E->r=_tmp79A;}),_tmp26E->loc=0U;_tmp26E;});Cyc_Absyn_decl_stmt(_tmp79B,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp79D=({struct Cyc_Absyn_Decl*_tmp270=_cycalloc(sizeof(*_tmp270));({void*_tmp79C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->tag=0,_tmp26F->f1=x_vd;_tmp26F;});_tmp270->r=_tmp79C;}),_tmp270->loc=0U;_tmp270;});Cyc_Absyn_decl_stmt(_tmp79D,s,0U);});
({void*_tmp79E=Cyc_Toc_stmt_exp_r(s);e->r=_tmp79E;});}}}}}}}}}}
# 1450
;_pop_region();}}
# 1522 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1526
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp27A=Cyc_Absyn_compress(aggrtype);void*_stmttmp25=_tmp27A;void*_tmp27B=_stmttmp25;union Cyc_Absyn_AggrInfo _tmp27C;if(*((int*)_tmp27B)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f1)== 20){_tmp27C=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp27C;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6AE;_tmp6AE.tag=0,({struct _fat_ptr _tmp79F=({const char*_tmp282="expecting union but found ";_tag_fat(_tmp282,sizeof(char),27U);});_tmp6AE.f1=_tmp79F;});_tmp6AE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp27F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6AD;_tmp6AD.tag=2,_tmp6AD.f1=(void*)aggrtype;_tmp6AD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp280=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6AC;_tmp6AC.tag=0,({
struct _fat_ptr _tmp7A0=({const char*_tmp281=" in check_tagged_union";_tag_fat(_tmp281,sizeof(char),23U);});_tmp6AC.f1=_tmp7A0;});_tmp6AC;});void*_tmp27D[3];_tmp27D[0]=& _tmp27E,_tmp27D[1]=& _tmp27F,_tmp27D[2]=& _tmp280;({
# 1529
(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp27D,sizeof(void*),3));});}_LL0:;}{
# 1532
struct _tuple30 _tmp283=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp26=_tmp283;struct _tuple30 _tmp284=_stmttmp26;void*_tmp286;void*_tmp285;_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;{struct _tuple1*temp=_tmp285;struct Cyc_Absyn_Exp*temp_exp=_tmp286;
struct Cyc_Absyn_Exp*_tmp287=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp287;
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp288=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*temp_f_tag=_tmp288;
struct Cyc_Absyn_Exp*_tmp289=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp289;
struct Cyc_Absyn_Exp*_tmp28A=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);struct Cyc_Absyn_Exp*temp_f_val=_tmp28A;
struct Cyc_Absyn_Stmt*_tmp28B=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);struct Cyc_Absyn_Stmt*sres=_tmp28B;
struct Cyc_Absyn_Stmt*_tmp28C=({struct Cyc_Absyn_Exp*_tmp7A2=test_exp;struct Cyc_Absyn_Stmt*_tmp7A1=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp7A2,_tmp7A1,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp28C;
void*_tmp28D=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);void*e1_p_type=_tmp28D;
struct Cyc_Absyn_Exp*_tmp28E=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);struct Cyc_Absyn_Exp*e1_f=_tmp28E;
struct Cyc_Absyn_Stmt*_tmp28F=({struct _tuple1*_tmp7A5=temp;void*_tmp7A4=e1_p_type;struct Cyc_Absyn_Exp*_tmp7A3=e1_f;Cyc_Absyn_declare_stmt(_tmp7A5,_tmp7A4,_tmp7A3,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp28F;
return Cyc_Toc_stmt_exp_r(s);}else{
# 1545
struct Cyc_Absyn_Exp*_tmp290=({struct Cyc_Absyn_Exp*_tmp7A6=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp7A6,Cyc_Toc_tag_sp,0U);});struct Cyc_Absyn_Exp*temp_f_tag=_tmp290;
struct Cyc_Absyn_Exp*_tmp291=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp291;
struct Cyc_Absyn_Exp*_tmp292=({struct Cyc_Absyn_Exp*_tmp7A7=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp7A7,Cyc_Toc_val_sp,0U);});struct Cyc_Absyn_Exp*temp_f_val=_tmp292;
struct Cyc_Absyn_Stmt*_tmp293=Cyc_Absyn_exp_stmt(temp_f_val,0U);struct Cyc_Absyn_Stmt*sres=_tmp293;
struct Cyc_Absyn_Stmt*_tmp294=({struct Cyc_Absyn_Exp*_tmp7A9=test_exp;struct Cyc_Absyn_Stmt*_tmp7A8=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp7A9,_tmp7A8,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp294;
struct Cyc_Absyn_Stmt*_tmp295=({struct _tuple1*_tmp7AC=temp;void*_tmp7AB=e1_c_type;struct Cyc_Absyn_Exp*_tmp7AA=e1;Cyc_Absyn_declare_stmt(_tmp7AC,_tmp7AB,_tmp7AA,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp295;
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1555
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1558
void*_tmp296=Cyc_Absyn_compress(t);void*_stmttmp27=_tmp296;void*_tmp297=_stmttmp27;union Cyc_Absyn_AggrInfo _tmp298;if(*((int*)_tmp297)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297)->f1)== 20){_tmp298=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp298;
# 1560
struct Cyc_Absyn_Aggrdecl*_tmp299=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp299;
({int _tmp7AD=Cyc_Toc_get_member_offset(ad,f);*f_tag=_tmp7AD;});{
# 1563
struct _fat_ptr str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp29D=({struct Cyc_String_pa_PrintArg_struct _tmp6B0;_tmp6B0.tag=0,_tmp6B0.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp6B0;});struct Cyc_String_pa_PrintArg_struct _tmp29E=({struct Cyc_String_pa_PrintArg_struct _tmp6AF;_tmp6AF.tag=0,_tmp6AF.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp6AF;});void*_tmp29B[2];_tmp29B[0]=& _tmp29D,_tmp29B[1]=& _tmp29E;({struct _fat_ptr _tmp7AE=({const char*_tmp29C="_union_%s_%s";_tag_fat(_tmp29C,sizeof(char),13U);});Cyc_aprintf(_tmp7AE,_tag_fat(_tmp29B,sizeof(void*),2));});});
({void*_tmp7AF=Cyc_Absyn_strct(({struct _fat_ptr*_tmp29A=_cycalloc(sizeof(*_tmp29A));*_tmp29A=str;_tmp29A;}));*tagged_member_type=_tmp7AF;});
if(clear_read)*_check_null(is_read)=0;
return(_check_null(ad->impl))->tagged;}}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 1574
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1577
void*_tmp29F=e->r;void*_stmttmp28=_tmp29F;void*_tmp2A0=_stmttmp28;void*_tmp2A3;void*_tmp2A2;void*_tmp2A1;switch(*((int*)_tmp2A0)){case 14: _tmp2A1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2A0)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2A1;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B1;_tmp6B1.tag=0,({struct _fat_ptr _tmp7B0=({const char*_tmp2A6="cast on lhs!";_tag_fat(_tmp2A6,sizeof(char),13U);});_tmp6B1.f1=_tmp7B0;});_tmp6B1;});void*_tmp2A4[1];_tmp2A4[0]=& _tmp2A5;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp2A4,sizeof(void*),1));});}case 21: _tmp2A1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2A0)->f1;_tmp2A2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2A0)->f2;_tmp2A3=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2A0)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2A1;struct _fat_ptr*f=_tmp2A2;int*is_read=_tmp2A3;
# 1580
return Cyc_Toc_is_tagged_union_project_impl(_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22: _tmp2A1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2A0)->f1;_tmp2A2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2A0)->f2;_tmp2A3=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2A0)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2A1;struct _fat_ptr*f=_tmp2A2;int*is_read=(int*)_tmp2A3;
# 1583
void*_tmp2A7=Cyc_Absyn_compress(_check_null(e1->topt));void*_stmttmp29=_tmp2A7;void*_tmp2A8=_stmttmp29;void*_tmp2A9;if(*((int*)_tmp2A8)== 3){_tmp2A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A8)->f1).elt_type;{void*et=_tmp2A9;
# 1585
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{
# 1587
return 0;};}default:
# 1589
 return 0;};}
# 1602 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1606
struct _tuple30 _tmp2AA=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp2A=_tmp2AA;struct _tuple30 _tmp2AB=_stmttmp2A;void*_tmp2AD;void*_tmp2AC;_tmp2AC=_tmp2AB.f1;_tmp2AD=_tmp2AB.f2;{struct _tuple1*temp=_tmp2AC;struct Cyc_Absyn_Exp*temp_exp=_tmp2AD;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1615
struct Cyc_Absyn_Exp*_tmp2AE=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2AE;
s2=({struct Cyc_Absyn_Exp*_tmp7B2=test_exp;struct Cyc_Absyn_Stmt*_tmp7B1=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp7B2,_tmp7B1,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1618
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp7B5=temp;void*_tmp7B4=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp7B3=
Cyc_Toc_push_address_exp(e1);
# 1618
Cyc_Absyn_declare_stmt(_tmp7B5,_tmp7B4,_tmp7B3,
# 1620
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1625
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B3;_tmp6B3.tag=0,({struct _fat_ptr _tmp7B6=({const char*_tmp2B2="exp_to_c: no type for ";_tag_fat(_tmp2B2,sizeof(char),23U);});_tmp6B3.f1=_tmp7B6;});_tmp6B3;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2B1=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp6B2;_tmp6B2.tag=4,_tmp6B2.f1=e;_tmp6B2;});void*_tmp2AF[2];_tmp2AF[0]=& _tmp2B0,_tmp2AF[1]=& _tmp2B1;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_tmp2AF,sizeof(void*),2));});{
void*old_typ=_check_null(e->topt);
# 1630
int did_inserted_checks=0;
{void*_tmp2B3=e->annot;void*_stmttmp2B=_tmp2B3;void*_tmp2B4=_stmttmp2B;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2B4)->tag == Cyc_Absyn_EmptyAnnot)
goto _LL4;else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2B4)->tag == Cyc_InsertChecks_NoCheck){_LL4:
 did_inserted_checks=1;goto _LL0;}else{
goto _LL0;}}_LL0:;}
# 1636
{void*_tmp2B5=e->r;void*_stmttmp2C=_tmp2B5;void*_tmp2B6=_stmttmp2C;int _tmp2BC;int _tmp2BB;enum Cyc_Absyn_Coercion _tmp2BE;void*_tmp2BD;int _tmp2B9;void*_tmp2BA;void*_tmp2B8;enum Cyc_Absyn_Incrementor _tmp2BF;enum Cyc_Absyn_Primop _tmp2C0;void*_tmp2B7;switch(*((int*)_tmp2B6)){case 2:
# 1638
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1).Null_c).tag == 1){
# 1644
struct Cyc_Absyn_Exp*_tmp2C1=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero=_tmp2C1;
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp7B7=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero))->r;e->r=_tmp7B7;});else{
# 1649
({void*_tmp7B8=({struct Cyc_Absyn_Exp*_tmp2C2[3];_tmp2C2[0]=zero,_tmp2C2[1]=zero,_tmp2C2[2]=zero;Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_fat_e,_tag_fat(_tmp2C2,sizeof(struct Cyc_Absyn_Exp*),3));});e->r=_tmp7B8;});}}else{
# 1651
e->r=(void*)& Cyc_Toc_zero_exp;}
goto _LL7;}else{
goto _LL7;}case 1:
 goto _LL7;case 42:
({void*_tmp7B9=(Cyc_Absyn_int_exp(0U,1,0U))->r;e->r=_tmp7B9;});goto _LL7;case 41: _tmp2B7=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2B7;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3: _tmp2C0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{enum Cyc_Absyn_Primop p=_tmp2C0;struct Cyc_List_List*es=_tmp2B7;
# 1659
struct Cyc_List_List*_tmp2C3=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Toc_get_cyc_type,es);struct Cyc_List_List*old_types=_tmp2C3;
# 1661
({(void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Toc_exp_to_c,nv,es);
{enum Cyc_Absyn_Primop _tmp2C4=p;switch((int)_tmp2C4){case Cyc_Absyn_Numelts:  {
# 1664
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)(_check_null(es))->hd;
{void*_tmp2C5=Cyc_Absyn_compress(_check_null(arg->topt));void*_stmttmp2D=_tmp2C5;void*_tmp2C6=_stmttmp2D;void*_tmp2CA;void*_tmp2C9;void*_tmp2C8;void*_tmp2C7;if(*((int*)_tmp2C6)== 3){_tmp2C7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C6)->f1).elt_type;_tmp2C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C6)->f1).ptr_atts).nullable;_tmp2C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C6)->f1).ptr_atts).bounds;_tmp2CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C6)->f1).ptr_atts).zero_term;{void*elt_type=_tmp2C7;void*nbl=_tmp2C8;void*bound=_tmp2C9;void*zt=_tmp2CA;
# 1667
struct Cyc_Absyn_Exp*_tmp2CB=({void*_tmp7BA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7BA,bound);});struct Cyc_Absyn_Exp*eopt=_tmp2CB;
if(eopt == 0)
# 1670
({void*_tmp7BC=({struct Cyc_Absyn_Exp*_tmp2CC[2];_tmp2CC[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_tmp7BB=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2CC[1]=_tmp7BB;});Cyc_Toc_fncall_exp_r(Cyc_Toc__get_fat_size_e,_tag_fat(_tmp2CC,sizeof(struct Cyc_Absyn_Exp*),2));});
# 1670
e->r=_tmp7BC;});else{
# 1672
if(Cyc_Tcutil_force_type2bool(0,zt)){
# 1674
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)es->hd);
({void*_tmp7BD=({struct Cyc_Absyn_Exp*_tmp2CD[2];_tmp2CD[0]=(struct Cyc_Absyn_Exp*)es->hd,_tmp2CD[1]=eopt;Cyc_Toc_fncall_exp_r(function_e,_tag_fat(_tmp2CD,sizeof(struct Cyc_Absyn_Exp*),2));});e->r=_tmp7BD;});}else{
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(eopt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B4;_tmp6B4.tag=0,({struct _fat_ptr _tmp7BE=({const char*_tmp2D0="can't calculate numelts";_tag_fat(_tmp2D0,sizeof(char),24U);});_tmp6B4.f1=_tmp7BE;});_tmp6B4;});void*_tmp2CE[1];_tmp2CE[0]=& _tmp2CF;Cyc_Warn_err2(e->loc,_tag_fat(_tmp2CE,sizeof(void*),1));});
# 1681
({void*_tmp7C1=({struct Cyc_Absyn_Exp*_tmp7C0=arg;struct Cyc_Absyn_Exp*_tmp7BF=eopt;Cyc_Toc_conditional_exp_r(_tmp7C0,_tmp7BF,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp7C1;});}else{
# 1683
e->r=eopt->r;}}}
goto _LL77;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B6;_tmp6B6.tag=0,({struct _fat_ptr _tmp7C2=({const char*_tmp2D4="numelts primop applied to non-pointer ";_tag_fat(_tmp2D4,sizeof(char),39U);});_tmp6B6.f1=_tmp7C2;});_tmp6B6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2D3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6B5;_tmp6B5.tag=2,_tmp6B5.f1=(void*)
_check_null(arg->topt);_tmp6B5;});void*_tmp2D1[2];_tmp2D1[0]=& _tmp2D2,_tmp2D1[1]=& _tmp2D3;({
# 1685
(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp2D1,sizeof(void*),2));});}_LL77:;}
# 1688
goto _LL62;}case Cyc_Absyn_Plus:
# 1693
 if(Cyc_Toc_is_toplevel(nv))
({int(*_tmp7C3)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Toc_unimp;});_tmp7C3(({const char*_tmp2D5="can't do pointer arithmetic at top-level";_tag_fat(_tmp2D5,sizeof(char),41U);}));});
{void*_tmp2D6=Cyc_Absyn_compress((void*)(_check_null(old_types))->hd);void*_stmttmp2E=_tmp2D6;void*_tmp2D7=_stmttmp2E;void*_tmp2DA;void*_tmp2D9;void*_tmp2D8;if(*((int*)_tmp2D7)== 3){_tmp2D8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).elt_type;_tmp2D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).ptr_atts).bounds;_tmp2DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).ptr_atts).zero_term;{void*elt_type=_tmp2D8;void*b=_tmp2D9;void*zt=_tmp2DA;
# 1697
struct Cyc_Absyn_Exp*_tmp2DB=({void*_tmp7C4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7C4,b);});struct Cyc_Absyn_Exp*eopt=_tmp2DB;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)(_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd;
if(eopt == 0)
({void*_tmp7C6=({struct Cyc_Absyn_Exp*_tmp2DC[3];_tmp2DC[0]=e1,({
struct Cyc_Absyn_Exp*_tmp7C5=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2DC[1]=_tmp7C5;}),_tmp2DC[2]=e2;Cyc_Toc_fncall_exp_r(Cyc_Toc__fat_ptr_plus_e,_tag_fat(_tmp2DC,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1701
e->r=_tmp7C6;});else{
# 1703
if(Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp7C8=({struct Cyc_Absyn_Exp*_tmp2DD[3];_tmp2DD[0]=e1,_tmp2DD[1]=eopt,_tmp2DD[2]=e2;({struct Cyc_Absyn_Exp*_tmp7C7=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp7C7,_tag_fat(_tmp2DD,sizeof(struct Cyc_Absyn_Exp*),3));});});e->r=_tmp7C8;});}
goto _LL7C;}}else{
goto _LL7C;}_LL7C:;}
# 1708
goto _LL62;case Cyc_Absyn_Minus:  {
# 1713
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)(_check_null(old_types))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)(_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)(_check_null(old_types->tl))->hd)){
({void*_tmp7CA=({struct Cyc_Absyn_Exp*_tmp7C9=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp7C9,Cyc_Toc_curr_sp);});e1->r=_tmp7CA;});
({void*_tmp7CC=({struct Cyc_Absyn_Exp*_tmp7CB=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp7CB,Cyc_Toc_curr_sp);});e2->r=_tmp7CC;});
({void*_tmp7CE=({void*_tmp7CD=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp7CD;});e1->topt=_tmp7CE;});
({void*_tmp7D0=({struct Cyc_Absyn_Exp*_tmp7CF=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp7CF,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1721
e->r=_tmp7D0;});}else{
# 1724
({void*_tmp7D3=({struct Cyc_Absyn_Exp*_tmp2DE[3];_tmp2DE[0]=e1,({
struct Cyc_Absyn_Exp*_tmp7D2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2DE[1]=_tmp7D2;}),({
struct Cyc_Absyn_Exp*_tmp7D1=Cyc_Absyn_prim1_exp(2U,e2,0U);_tmp2DE[2]=_tmp7D1;});Cyc_Toc_fncall_exp_r(Cyc_Toc__fat_ptr_plus_e,_tag_fat(_tmp2DE,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1724
e->r=_tmp7D3;});}}
# 1728
goto _LL62;}case Cyc_Absyn_Eq:
 goto _LL6C;case Cyc_Absyn_Neq: _LL6C: goto _LL6E;case Cyc_Absyn_Gt: _LL6E: goto _LL70;case Cyc_Absyn_Gte: _LL70: goto _LL72;case Cyc_Absyn_Lt: _LL72: goto _LL74;case Cyc_Absyn_Lte: _LL74: {
# 1731
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)(_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd;
void*t1=(void*)(_check_null(old_types))->hd;
void*t2=(void*)(_check_null(old_types->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*_tmp2DF=({void*_tmp7D4=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp7D4,Cyc_Toc_mt_tq);});void*t=_tmp2DF;
({void*_tmp7D7=({void*_tmp7D6=t;Cyc_Toc_cast_it_r(_tmp7D6,({struct Cyc_Absyn_Exp*_tmp7D5=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp7D5,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp7D7;});
e1->topt=t;}
# 1741
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*_tmp2E0=({void*_tmp7D8=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp7D8,Cyc_Toc_mt_tq);});void*t=_tmp2E0;
({void*_tmp7DB=({void*_tmp7DA=t;Cyc_Toc_cast_it_r(_tmp7DA,({struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp7D9,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp7DB;});
e2->topt=t;}
# 1746
goto _LL62;}default:
 goto _LL62;}_LL62:;}
# 1749
goto _LL7;}case 5: _tmp2B7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2BF=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{struct Cyc_Absyn_Exp*e2=_tmp2B7;enum Cyc_Absyn_Incrementor incr=_tmp2BF;
# 1751
void*e2_cyc_typ=_check_null(e2->topt);
# 1760 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_tmp2FD="increment";_tag_fat(_tmp2FD,sizeof(char),10U);});
if((int)incr == 2 ||(int)incr == 3)incr_str=({const char*_tmp2E1="decrement";_tag_fat(_tmp2E1,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BA;_tmp6BA.tag=0,({struct _fat_ptr _tmp7DC=({const char*_tmp2E9="in-place ";_tag_fat(_tmp2E9,sizeof(char),10U);});_tmp6BA.f1=_tmp7DC;});_tmp6BA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B9;_tmp6B9.tag=0,_tmp6B9.f1=incr_str;_tmp6B9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B8;_tmp6B8.tag=0,({struct _fat_ptr _tmp7DD=({const char*_tmp2E8=" is not supported when ";_tag_fat(_tmp2E8,sizeof(char),24U);});_tmp6B8.f1=_tmp7DD;});_tmp6B8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B7;_tmp6B7.tag=0,({
struct _fat_ptr _tmp7DE=({const char*_tmp2E7="dereferencing a zero-terminated pointer";_tag_fat(_tmp2E7,sizeof(char),40U);});_tmp6B7.f1=_tmp7DE;});_tmp6B7;});void*_tmp2E2[4];_tmp2E2[0]=& _tmp2E3,_tmp2E2[1]=& _tmp2E4,_tmp2E2[2]=& _tmp2E5,_tmp2E2[3]=& _tmp2E6;({
# 1766
(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_tmp2E2,sizeof(void*),4));});
# 1768
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp2EA=incr;switch((int)_tmp2EA){case Cyc_Absyn_PreInc:
 op=0U;goto _LL81;case Cyc_Absyn_PreDec:
 op=2U;goto _LL81;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BD;_tmp6BD.tag=0,({struct _fat_ptr _tmp7DF=({const char*_tmp2F0="in-place post";_tag_fat(_tmp2F0,sizeof(char),14U);});_tmp6BD.f1=_tmp7DF;});_tmp6BD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BC;_tmp6BC.tag=0,_tmp6BC.f1=incr_str;_tmp6BC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BB;_tmp6BB.tag=0,({
struct _fat_ptr _tmp7E0=({const char*_tmp2EF=" is not supported on @tagged union members";_tag_fat(_tmp2EF,sizeof(char),43U);});_tmp6BB.f1=_tmp7E0;});_tmp6BB;});void*_tmp2EB[3];_tmp2EB[0]=& _tmp2EC,_tmp2EB[1]=& _tmp2ED,_tmp2EB[2]=& _tmp2EE;({
# 1775
(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_tmp2EB,sizeof(void*),3));});}_LL81:;}
# 1778
({void*_tmp7E2=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));_tmp2F2->tag=4,_tmp2F2->f1=e2,({struct Cyc_Core_Opt*_tmp7E1=({struct Cyc_Core_Opt*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1->v=(void*)op;_tmp2F1;});_tmp2F2->f2=_tmp7E1;}),_tmp2F2->f3=one;_tmp2F2;});e->r=_tmp7E2;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1782
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1787
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
int change=1;
struct Cyc_Absyn_Exp*_tmp2F3=((int)incr == 1 ||(int)incr == 3)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp2F3;
# 1792
if((int)incr == 2 ||(int)incr == 3)
change=-1;
({void*_tmp7E6=({struct Cyc_Absyn_Exp*_tmp2F4[3];({struct Cyc_Absyn_Exp*_tmp7E5=Cyc_Toc_push_address_exp(e2);_tmp2F4[0]=_tmp7E5;}),({
struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp2F4[1]=_tmp7E4;}),({
struct Cyc_Absyn_Exp*_tmp7E3=Cyc_Absyn_signed_int_exp(change,0U);_tmp2F4[2]=_tmp7E3;});Cyc_Toc_fncall_exp_r(fn_e,_tag_fat(_tmp2F4,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1794
e->r=_tmp7E6;});}else{
# 1797
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1803
did_inserted_checks=1;
if((int)incr != 1){
struct _tuple1*_tmp2F5=Cyc_Toc_temp_var();struct _tuple1*x=_tmp2F5;
void*_tmp2F6=({void*_tmp7E7=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp7E7,Cyc_Toc_mt_tq);});void*t=_tmp2F6;
struct Cyc_Absyn_Exp*_tmp2F7=Cyc_Toc_push_address_exp(e2);struct Cyc_Absyn_Exp*xexp=_tmp2F7;
struct Cyc_Absyn_Exp*_tmp2F8=({struct Cyc_Absyn_Exp*_tmp7E8=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp7E8,
Cyc_Absyn_int_exp(2U,0,0U),0U);});
# 1808
struct Cyc_Absyn_Exp*testexp=_tmp2F8;
# 1810
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_tmp7EA=({struct Cyc_Absyn_Exp*_tmp7E9=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_tmp7E9,
Cyc_Absyn_int_exp(2U,0,0U),0U);});
# 1811
Cyc_Absyn_and_exp(_tmp7EA,testexp,0U);});{
# 1813
struct Cyc_Absyn_Stmt*_tmp2F9=({struct Cyc_Absyn_Exp*_tmp7EC=testexp;struct Cyc_Absyn_Stmt*_tmp7EB=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U,0U),0U);
# 1813
Cyc_Absyn_ifthenelse_stmt(_tmp7EC,_tmp7EB,
# 1815
Cyc_Absyn_exp_stmt(({void*_tmp2FA=0U;Cyc_Toc_fncall_exp_dl(Cyc_Toc__throw_arraybounds_e,_tag_fat(_tmp2FA,sizeof(struct Cyc_Absyn_Exp*),0));}),0U),0U);});
# 1813
struct Cyc_Absyn_Stmt*s=_tmp2F9;
# 1816
s=({struct Cyc_Absyn_Stmt*_tmp7ED=s;Cyc_Absyn_seq_stmt(_tmp7ED,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_tmp7EE=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U))->r;e->r=_tmp7EE;});}}else{
# 1820
struct Cyc_Toc_functionSet*_tmp2FB=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Toc_functionSet*fnSet=_tmp2FB;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(fnSet,e2);
({void*_tmp7F1=({struct Cyc_Absyn_Exp*_tmp2FC[2];({struct Cyc_Absyn_Exp*_tmp7F0=Cyc_Toc_push_address_exp(e2);_tmp2FC[0]=_tmp7F0;}),({struct Cyc_Absyn_Exp*_tmp7EF=Cyc_Absyn_signed_int_exp(1,0U);_tmp2FC[1]=_tmp7EF;});Cyc_Toc_fncall_exp_r(fn_e,_tag_fat(_tmp2FC,sizeof(struct Cyc_Absyn_Exp*),2));});e->r=_tmp7F1;});}}else{
# 1824
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
({(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor))Cyc_Toc_lvalue_assign;})(e2,0,Cyc_Toc_incr_lvalue,incr);
e->r=e2->r;}}}
# 1828
goto _LL7;}}case 4: _tmp2B7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2BA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_Core_Opt*popt=_tmp2B8;struct Cyc_Absyn_Exp*e2=_tmp2BA;
# 1847 "toc.cyc"
void*e1_old_typ=_check_null(e1->topt);
void*e2_old_typ=_check_null(e2->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(e1,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);
({void*_tmp7F2=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp7F2;});
# 1858
goto _LL7;}{
# 1860
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
return;}{
# 1869
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1875
int done=0;
if(popt != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp2FE=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp2F=_tmp2FE;enum Cyc_Absyn_Primop _tmp2FF=_stmttmp2F;switch((int)_tmp2FF){case Cyc_Absyn_Plus:
 change=e2;goto _LL88;case Cyc_Absyn_Minus:
 change=Cyc_Absyn_prim1_exp(2U,e2,0U);goto _LL88;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp301=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BE;_tmp6BE.tag=0,({struct _fat_ptr _tmp7F3=({const char*_tmp302="bad t ? pointer arithmetic";_tag_fat(_tmp302,sizeof(char),27U);});_tmp6BE.f1=_tmp7F3;});_tmp6BE;});void*_tmp300[1];_tmp300[0]=& _tmp301;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp300,sizeof(void*),1));});}_LL88:;}
# 1885
done=1;{
# 1887
struct Cyc_Absyn_Exp*_tmp303=Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp303;
({void*_tmp7F6=({struct Cyc_Absyn_Exp*_tmp304[3];({struct Cyc_Absyn_Exp*_tmp7F5=Cyc_Toc_push_address_exp(e1);_tmp304[0]=_tmp7F5;}),({
struct Cyc_Absyn_Exp*_tmp7F4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp304[1]=_tmp7F4;}),_tmp304[2]=change;Cyc_Toc_fncall_exp_r(fn_e,_tag_fat(_tmp304,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1888
e->r=_tmp7F6;});}}else{
# 1891
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1894
enum Cyc_Absyn_Primop _tmp305=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp30=_tmp305;enum Cyc_Absyn_Primop _tmp306=_stmttmp30;if(_tmp306 == Cyc_Absyn_Plus){
# 1896
done=1;
({void*_tmp7F8=({struct Cyc_Absyn_Exp*_tmp307[2];_tmp307[0]=e1,_tmp307[1]=e2;({struct Cyc_Absyn_Exp*_tmp7F7=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp7F7,_tag_fat(_tmp307,sizeof(struct Cyc_Absyn_Exp*),2));});});e->r=_tmp7F8;});
goto _LL8F;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp309=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BF;_tmp6BF.tag=0,({struct _fat_ptr _tmp7F9=({const char*_tmp30A="bad zero-terminated pointer arithmetic";_tag_fat(_tmp30A,sizeof(char),39U);});_tmp6BF.f1=_tmp7F9;});_tmp6BF;});void*_tmp308[1];_tmp308[0]=& _tmp309;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp308,sizeof(void*),1));});}_LL8F:;}}}
# 1902
if(!done){
# 1904
if(e1_poly)
({void*_tmp7FB=({void*_tmp7FA=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp7FA,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_tmp7FB;});
# 1910
if(!Cyc_Absyn_is_lvalue(e1)){
({void(*_tmp7FD)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*)=({(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*))Cyc_Toc_lvalue_assign;});struct Cyc_Absyn_Exp*_tmp7FC=e1;_tmp7FD(_tmp7FC,0,Cyc_Toc_assignop_lvalue,({struct _tuple33*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B->f1=popt,_tmp30B->f2=e2;_tmp30B;}));});
e->r=e1->r;}}
# 1915
goto _LL7;}}}}case 6: _tmp2B7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2BA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_Absyn_Exp*e2=_tmp2B8;struct Cyc_Absyn_Exp*e3=_tmp2BA;
# 1917
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7: _tmp2B7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_Absyn_Exp*e2=_tmp2B8;
_tmp2B7=e1;_tmp2B8=e2;goto _LL1F;}case 8: _tmp2B7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_LL1F: {struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_Absyn_Exp*e2=_tmp2B8;
_tmp2B7=e1;_tmp2B8=e2;goto _LL21;}case 9: _tmp2B7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_Absyn_Exp*e2=_tmp2B8;
# 1924
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3 == 0){_tmp2B7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_List_List*es=_tmp2B8;
# 1928
void*e1_typ=_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 1933
({void*_tmp7FF=({struct Cyc_Absyn_Exp*_tmp30C[1];({struct Cyc_Absyn_Exp*_tmp7FE=Cyc_Absyn_copy_exp(e1);_tmp30C[0]=_tmp7FE;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_tmp30C,sizeof(struct Cyc_Absyn_Exp*),1));})->r;e1->r=_tmp7FF;});
({(void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_tmp2B7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2B9=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3)->num_varargs;_tmp2BA=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3)->injectors;_tmp2BD=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3)->vai;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_List_List*es=_tmp2B8;int num_varargs=_tmp2B9;struct Cyc_List_List*injectors=_tmp2BA;struct Cyc_Absyn_VarargInfo*vai=_tmp2BD;
# 1945 "toc.cyc"
struct _RegionHandle _tmp30D=_new_region("r");struct _RegionHandle*r=& _tmp30D;_push_region(r);{
struct _tuple30 _tmp30E=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp31=_tmp30E;struct _tuple30 _tmp30F=_stmttmp31;void*_tmp311;void*_tmp310;_tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;{struct _tuple1*argv=_tmp310;struct Cyc_Absyn_Exp*argvexp=_tmp311;
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_signed_int_exp(num_varargs,0U);
void*cva_type=Cyc_Toc_typ_to_c(vai->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 1952
int num_args=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(es);
int num_normargs=num_args - num_varargs;
# 1956
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,es=es->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)(_check_null(es))->hd);
new_args=({struct Cyc_List_List*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp312->tl=new_args;_tmp312;});}}
# 1961
new_args=({struct Cyc_List_List*_tmp314=_cycalloc(sizeof(*_tmp314));({struct Cyc_Absyn_Exp*_tmp801=({struct Cyc_Absyn_Exp*_tmp313[3];_tmp313[0]=argvexp,({
# 1963
struct Cyc_Absyn_Exp*_tmp800=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp313[1]=_tmp800;}),_tmp313[2]=num_varargs_exp;Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_tmp313,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1961
_tmp314->hd=_tmp801;}),_tmp314->tl=new_args;_tmp314;});
# 1966
new_args=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(new_args);{
# 1968
void*e1_typ=_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 1973
({void*_tmp803=({struct Cyc_Absyn_Exp*_tmp315[1];({struct Cyc_Absyn_Exp*_tmp802=Cyc_Absyn_copy_exp(e1);_tmp315[0]=_tmp802;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_tmp315,sizeof(struct Cyc_Absyn_Exp*),1));})->r;e1->r=_tmp803;});{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 1977
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp316=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_stmttmp32=_tmp316;void*_tmp317=_stmttmp32;void*_tmp318;if(*((int*)_tmp317)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp317)->f1)== 18){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp317)->f1)->f1).KnownDatatype).tag == 2){_tmp318=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp317)->f1)->f1).KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl*x=_tmp318;
tud=x;goto _LL97;}}else{goto _LL9A;}}else{goto _LL9A;}}else{_LL9A:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp31A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C0;_tmp6C0.tag=0,({struct _fat_ptr _tmp804=({const char*_tmp31B="unknown datatype in vararg with inject";_tag_fat(_tmp31B,sizeof(char),39U);});_tmp6C0.f1=_tmp804;});_tmp6C0;});void*_tmp319[1];_tmp319[0]=& _tmp31A;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp319,sizeof(void*),1));});}_LL97:;}{
# 1983
struct _fat_ptr vs=({unsigned _tmp327=(unsigned)num_varargs;struct _tuple1**_tmp326=({struct _RegionHandle*_tmp805=r;_region_malloc(_tmp805,_check_times(_tmp327,sizeof(struct _tuple1*)));});({{unsigned _tmp6C1=(unsigned)num_varargs;unsigned i;for(i=0;i < _tmp6C1;++ i){({struct _tuple1*_tmp806=Cyc_Toc_temp_var();_tmp326[i]=_tmp806;});}}0;});_tag_fat(_tmp326,sizeof(struct _tuple1*),_tmp327);});
# 1985
if(num_varargs != 0){
# 1989
struct Cyc_List_List*_tmp31C=0;struct Cyc_List_List*array_args=_tmp31C;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_tmp31D=_cycalloc(sizeof(*_tmp31D));({struct Cyc_Absyn_Exp*_tmp807=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp31D->hd=_tmp807;}),_tmp31D->tl=array_args;_tmp31D;});}}
s=({struct _tuple1*_tmp80A=argv;void*_tmp809=arr_type;struct Cyc_Absyn_Exp*_tmp808=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_tmp80A,_tmp809,_tmp808,s,0U);});
# 1994
es=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(es);
injectors=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(injectors);{
int i=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(es)- 1;
for(1;es != 0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp31E=(struct Cyc_Absyn_Datatypefield*)(_check_null(injectors))->hd;struct Cyc_Absyn_Datatypefield*_stmttmp33=_tmp31E;struct Cyc_Absyn_Datatypefield*_tmp31F=_stmttmp33;void*_tmp321;void*_tmp320;_tmp320=_tmp31F->name;_tmp321=_tmp31F->typs;{struct _tuple1*qv=_tmp320;struct Cyc_List_List*tqts=_tmp321;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)(_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2008
struct _tuple1*_tmp322=Cyc_Toc_collapse_qvars(qv,tud->name);struct _tuple1*tdn=_tmp322;
struct Cyc_List_List*_tmp323=({struct _tuple19*_tmp325[2];({struct _tuple19*_tmp80E=({struct _fat_ptr*_tmp80D=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp80D,Cyc_Toc_datatype_tag(tud,qv));});_tmp325[0]=_tmp80E;}),({
struct _tuple19*_tmp80C=({struct _fat_ptr*_tmp80B=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp80B,arg);});_tmp325[1]=_tmp80C;});({
# 2009
(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp325,sizeof(struct _tuple19*),2));});struct Cyc_List_List*dles=_tmp323;
# 2011
s=({struct _tuple1*_tmp811=var;void*_tmp810=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_tmp80F=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324->tag=29,_tmp324->f1=tdn,_tmp324->f2=0,_tmp324->f3=dles,_tmp324->f4=0;_tmp324;}),0U);
# 2011
Cyc_Absyn_declare_stmt(_tmp811,_tmp810,_tmp80F,s,0U);});}}}}}else{
# 2019
s=({struct _tuple1*_tmp814=argv;void*_tmp813=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp812=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp814,_tmp813,_tmp812,s,0U);});}}}else{
if(num_varargs != 0){
# 2023
struct Cyc_List_List*array_args=0;
for(1;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp328->tl=array_args;_tmp328;});}{
# 2028
struct Cyc_Absyn_Exp*_tmp329=Cyc_Absyn_array_exp(({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(array_args),0U);struct Cyc_Absyn_Exp*init=_tmp329;
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}else{
# 2031
s=({struct _tuple1*_tmp817=argv;void*_tmp816=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp815=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp817,_tmp816,_tmp815,s,0U);});}}
({void*_tmp818=Cyc_Toc_stmt_exp_r(s);e->r=_tmp818;});}}}}
# 2034
_npop_handler(0);goto _LL7;
# 1945
;_pop_region();}}case 11: _tmp2B7=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B9=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2B7;int b=_tmp2B9;
# 2037
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp81A=({void*_tmp819=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp819,({struct Cyc_Absyn_Exp*_tmp32A[1];_tmp32A[0]=e1;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_tmp32A,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);})->r;e->r=_tmp81A;});
goto _LL7;}}case 12: _tmp2B7=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2B7;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13: _tmp2B7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_List_List*ts=_tmp2B8;
# 2043
Cyc_Toc_exp_to_c(nv,e1);
# 2052 "toc.cyc"
for(1;ts != 0;ts=ts->tl){
enum Cyc_Absyn_KindQual _tmp32B=(Cyc_Tcutil_type_kind((void*)ts->hd))->kind;enum Cyc_Absyn_KindQual k=_tmp32B;
if((int)k != 4 &&(int)k != 3){
{void*_tmp32C=Cyc_Absyn_compress((void*)ts->hd);void*_stmttmp34=_tmp32C;void*_tmp32D=_stmttmp34;switch(*((int*)_tmp32D)){case 2:
 goto _LLA3;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp32D)->f1)== 18){_LLA3:
 continue;}else{goto _LLA4;}default: _LLA4: {
# 2059
void*_tmp32E=Cyc_Toc_typ_to_c(_check_null(e->topt));void*tc=_tmp32E;
({void*_tmp81B=(Cyc_Toc_array_to_ptr_cast(tc,e1,0U))->r;e->r=_tmp81B;});
# 2066
{void*_tmp32F=tc;if(*((int*)_tmp32F)== 3){if(*((int*)(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32F)->f1).elt_type)== 5){
# 2068
({void*_tmp81D=Cyc_Toc_stmt_exp_r(({struct Cyc_Absyn_Exp*_tmp81C=Cyc_Absyn_new_exp(e->r,e->loc);Cyc_Absyn_exp_stmt(_tmp81C,e->loc);}));e->r=_tmp81D;});
goto _LLA6;}else{goto _LLA9;}}else{_LLA9:
 goto _LLA6;}_LLA6:;}
# 2072
goto _LL9F;}}_LL9F:;}
# 2074
break;}}
# 2077
goto _LL7;}case 14: _tmp2B7=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2B9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;_tmp2BE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B6)->f4;{void**t=_tmp2B7;struct Cyc_Absyn_Exp*e1=_tmp2B8;int user_inserted=_tmp2B9;enum Cyc_Absyn_Coercion coercion=_tmp2BE;
# 2079
void*old_t2=_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2086
{struct _tuple34 _tmp330=({struct _tuple34 _tmp6C3;({void*_tmp81F=Cyc_Absyn_compress(old_t2);_tmp6C3.f1=_tmp81F;}),({void*_tmp81E=Cyc_Absyn_compress(new_typ);_tmp6C3.f2=_tmp81E;});_tmp6C3;});struct _tuple34 _stmttmp35=_tmp330;struct _tuple34 _tmp331=_stmttmp35;struct Cyc_Absyn_PtrInfo _tmp333;struct Cyc_Absyn_PtrInfo _tmp332;if(*((int*)_tmp331.f1)== 3)switch(*((int*)_tmp331.f2)){case 3: _tmp332=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp331.f1)->f1;_tmp333=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp331.f2)->f1;{struct Cyc_Absyn_PtrInfo p1=_tmp332;struct Cyc_Absyn_PtrInfo p2=_tmp333;
# 2088
struct Cyc_Absyn_Exp*_tmp334=({void*_tmp820=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp820,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b1=_tmp334;
struct Cyc_Absyn_Exp*_tmp335=({void*_tmp821=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp821,(p2.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b2=_tmp335;
int _tmp336=Cyc_Tcutil_force_type2bool(0,(p1.ptr_atts).zero_term);int zt1=_tmp336;
int _tmp337=Cyc_Tcutil_force_type2bool(0,(p2.ptr_atts).zero_term);int zt2=_tmp337;
{struct _tuple35 _tmp338=({struct _tuple35 _tmp6C2;_tmp6C2.f1=b1,_tmp6C2.f2=b2;_tmp6C2;});struct _tuple35 _stmttmp36=_tmp338;struct _tuple35 _tmp339=_stmttmp36;if(_tmp339.f1 != 0){if(_tmp339.f2 != 0){
# 2095
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp822=({struct Cyc_Absyn_Exp*_tmp33A[1];_tmp33A[0]=e1;Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_tmp33A,sizeof(struct Cyc_Absyn_Exp*),1));})->r;e->r=_tmp822;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;}
# 2104
goto _LLB2;}else{
# 2106
struct Cyc_Absyn_Exp*_tmp33B=_check_null(b1);struct Cyc_Absyn_Exp*e2=_tmp33B;
struct _tuple13 _tmp33C=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp37=_tmp33C;struct _tuple13 _tmp33D=_stmttmp37;int _tmp33F;unsigned _tmp33E;_tmp33E=_tmp33D.f1;_tmp33F=_tmp33D.f2;{unsigned i=_tmp33E;int valid=_tmp33F;
if(Cyc_Toc_is_toplevel(nv)){
# 2112
if((zt1 && !(p2.elt_tq).real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_tmp823=e2;Cyc_Absyn_prim2_exp(2U,_tmp823,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp824=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1))->r;e->r=_tmp824;});}else{
if(zt1){
# 2121
struct _tuple30 _tmp340=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp38=_tmp340;struct _tuple30 _tmp341=_stmttmp38;void*_tmp343;void*_tmp342;_tmp342=_tmp341.f1;_tmp343=_tmp341.f2;{struct _tuple1*x=_tmp342;struct Cyc_Absyn_Exp*x_exp=_tmp343;
struct Cyc_Absyn_Exp*arg3;
# 2125
{void*_tmp344=e1->r;void*_stmttmp39=_tmp344;void*_tmp345=_stmttmp39;void*_tmp346;switch(*((int*)_tmp345)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp345)->f1).Wstring_c).tag){case 8:
 arg3=e2;goto _LLC1;case 9:
 arg3=e2;goto _LLC1;default: goto _LLCA;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp345)->f1)){case 1: _tmp346=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp345)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp346;
_tmp346=vd;goto _LLC9;}case 4: _tmp346=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp345)->f1)->f1;_LLC9: {struct Cyc_Absyn_Vardecl*vd=_tmp346;
# 2130
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLCB;
arg3=e2;
goto _LLC1;}default: goto _LLCA;}default: _LLCA: _LLCB:
# 2137
 if(valid && i != 1U)
arg3=e2;else{
# 2140
arg3=({struct Cyc_Absyn_Exp*_tmp347[2];({
struct Cyc_Absyn_Exp*_tmp826=({void*_tmp825=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp825,x_exp);});_tmp347[0]=_tmp826;}),_tmp347[1]=e2;({struct Cyc_Absyn_Exp*_tmp827=
# 2140
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,e1);Cyc_Toc_fncall_exp_dl(_tmp827,_tag_fat(_tmp347,sizeof(struct Cyc_Absyn_Exp*),2));});});}
# 2142
goto _LLC1;}_LLC1:;}
# 2146
if(!zt2 && !(p2.elt_tq).real_const)
arg3=({struct Cyc_Absyn_Exp*_tmp828=arg3;Cyc_Absyn_prim2_exp(2U,_tmp828,Cyc_Absyn_uint_exp(1U,0U),0U);});{
struct Cyc_Absyn_Exp*_tmp348=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);struct Cyc_Absyn_Exp*arg2=_tmp348;
struct Cyc_Absyn_Exp*_tmp349=({struct Cyc_Absyn_Exp*_tmp34B[3];_tmp34B[0]=x_exp,_tmp34B[1]=arg2,_tmp34B[2]=arg3;Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_tmp34B,sizeof(struct Cyc_Absyn_Exp*),3));});struct Cyc_Absyn_Exp*tg_exp=_tmp349;
struct Cyc_Absyn_Stmt*_tmp34A=Cyc_Absyn_exp_stmt(tg_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp34A;
s=({struct _tuple1*_tmp82B=x;void*_tmp82A=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp829=e1;Cyc_Absyn_declare_stmt(_tmp82B,_tmp82A,_tmp829,s,0U);});
({void*_tmp82C=Cyc_Toc_stmt_exp_r(s);e->r=_tmp82C;});}}}else{
# 2155
({void*_tmp82E=({struct Cyc_Absyn_Exp*_tmp34C[3];_tmp34C[0]=e1,({
struct Cyc_Absyn_Exp*_tmp82D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_tmp34C[1]=_tmp82D;}),_tmp34C[2]=e2;Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_fat_e,_tag_fat(_tmp34C,sizeof(struct Cyc_Absyn_Exp*),3));});
# 2155
e->r=_tmp82E;});}}
# 2157
goto _LLB2;}}}else{if(_tmp339.f2 != 0){
# 2167 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp34D=_check_null(b2);struct Cyc_Absyn_Exp*new_e2=_tmp34D;
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_tmp82F=b2;Cyc_Absyn_add_exp(_tmp82F,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2174
struct Cyc_Absyn_Exp*_tmp34E=({struct Cyc_Absyn_Exp*_tmp350[3];_tmp350[0]=e1,({
# 2176
struct Cyc_Absyn_Exp*_tmp830=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp350[1]=_tmp830;}),_tmp350[2]=new_e2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__untag_fat_ptr_e,_tag_fat(_tmp350,sizeof(struct Cyc_Absyn_Exp*),3));});
# 2174
struct Cyc_Absyn_Exp*ptr_exp=_tmp34E;
# 2178
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp832=({struct Cyc_Absyn_Exp*_tmp34F[1];({struct Cyc_Absyn_Exp*_tmp831=Cyc_Absyn_copy_exp(ptr_exp);_tmp34F[0]=_tmp831;});Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,_tag_fat(_tmp34F,sizeof(struct Cyc_Absyn_Exp*),1));});ptr_exp->r=_tmp832;});
({void*_tmp833=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_tmp833;});
goto _LLB2;}}else{
# 2186
if((zt1 && !zt2)&& !(p2.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({int(*_tmp834)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Toc_unimp;});_tmp834(({const char*_tmp351="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_tmp351,sizeof(char),70U);}));});
({void*_tmp837=({struct Cyc_Absyn_Exp*_tmp352[3];_tmp352[0]=e1,({
struct Cyc_Absyn_Exp*_tmp836=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp352[1]=_tmp836;}),({
struct Cyc_Absyn_Exp*_tmp835=Cyc_Absyn_uint_exp(1U,0U);_tmp352[2]=_tmp835;});Cyc_Toc_fncall_exp_r(Cyc_Toc__fat_ptr_decrease_size_e,_tag_fat(_tmp352,sizeof(struct Cyc_Absyn_Exp*),3));});
# 2189
e->r=_tmp837;});}
# 2193
goto _LLB2;}}_LLB2:;}
# 2195
goto _LLAB;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp331.f2)->f1)== 1){_tmp332=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp331.f1)->f1;{struct Cyc_Absyn_PtrInfo p1=_tmp332;
# 2197
{struct Cyc_Absyn_Exp*_tmp353=({void*_tmp838=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp838,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_stmttmp3A=_tmp353;struct Cyc_Absyn_Exp*_tmp354=_stmttmp3A;if(_tmp354 == 0){
# 2199
({void*_tmp83A=({struct Cyc_Absyn_Exp*_tmp839=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_tmp839,Cyc_Toc_curr_sp);});e1->r=_tmp83A;});
e1->topt=new_typ_c;
goto _LLCC;}else{
goto _LLCC;}_LLCC:;}
# 2204
goto _LLAB;}}else{goto _LLB0;}default: goto _LLB0;}else{_LLB0:
# 2206
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;
goto _LLAB;}_LLAB:;}
# 2210
goto _LL7;}case 15: _tmp2B7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2B7;
# 2213
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
({(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,int),int))Cyc_Toc_lvalue_assign;})(e1,0,Cyc_Toc_address_lvalue,1);
# 2219
({void*_tmp83C=({void*_tmp83B=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp83B,e1);});e->r=_tmp83C;});}else{
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(_check_null(e1->topt))))
# 2222
({void*_tmp83E=({void*_tmp83D=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp83D,e1);});e->r=_tmp83E;});}
goto _LL7;}case 16: _tmp2B7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{struct Cyc_Absyn_Exp*rgnopt=_tmp2B7;struct Cyc_Absyn_Exp*e1=_tmp2B8;
# 2230
Cyc_Toc_exp_to_c(nv,e1);{
# 2232
void*elt_typ=Cyc_Toc_typ_to_c(_check_null(e1->topt));
struct _tuple30 _tmp355=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3B=_tmp355;struct _tuple30 _tmp356=_stmttmp3B;void*_tmp358;void*_tmp357;_tmp357=_tmp356.f1;_tmp358=_tmp356.f2;{struct _tuple1*x=_tmp357;struct Cyc_Absyn_Exp*xexp=_tmp358;
struct Cyc_Absyn_Exp*lhs;
{void*_tmp359=Cyc_Absyn_compress(elt_typ);void*_stmttmp3C=_tmp359;void*_tmp35A=_stmttmp3C;void*_tmp35B;if(*((int*)_tmp35A)== 4){_tmp35B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35A)->f1).elt_type;{void*t2=_tmp35B;
# 2237
elt_typ=Cyc_Toc_typ_to_c(t2);
lhs=Cyc_Absyn_copy_exp(xexp);
goto _LLD4;}}else{
# 2241
lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(xexp),0U);
goto _LLD4;}_LLD4:;}{
# 2244
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*_tmp35C=(unsigned)array_len?Cyc_Toc_temp_var(): 0;struct _tuple1*lenvar=_tmp35C;
struct Cyc_Absyn_Exp*_tmp35D=(unsigned)array_len?Cyc_Absyn_var_exp(_check_null(lenvar),0U): 0;struct Cyc_Absyn_Exp*lenexp=_tmp35D;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_tmp35E[2];_tmp35E[0]=lenexp,({struct Cyc_Absyn_Exp*_tmp83F=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp35E[1]=_tmp83F;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_times_e,_tag_fat(_tmp35E,sizeof(struct Cyc_Absyn_Exp*),2));});else{
# 2251
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(xexp,0U),0U);}{
# 2254
struct Cyc_Absyn_Exp*_tmp35F=Cyc_Toc_get_varsizeexp(e1);struct Cyc_Absyn_Exp*vse=_tmp35F;
if(vse != 0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2258
if(rgnopt == 0 || Cyc_Flags_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2261
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=Cyc_Toc_rmalloc_exp(rgnopt,mexp);}{
# 2264
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_fat_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp360[3];({
struct Cyc_Absyn_Exp*_tmp842=Cyc_Absyn_copy_exp(xexp);_tmp360[0]=_tmp842;}),({
struct Cyc_Absyn_Exp*_tmp841=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp360[1]=_tmp841;}),
(unsigned)lenexp?_tmp360[2]=lenexp:({struct Cyc_Absyn_Exp*_tmp840=Cyc_Absyn_uint_exp(1U,0U);_tmp360[2]=_tmp840;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_tmp360,sizeof(struct Cyc_Absyn_Exp*),3));});else{
# 2271
answer=Cyc_Absyn_copy_exp(xexp);}
({void*_tmp843=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp361=_cycalloc(sizeof(*_tmp361));_tmp361->tag=Cyc_Toc_Dest,_tmp361->f1=xexp;_tmp361;});e->annot=_tmp843;});{
struct Cyc_Absyn_Stmt*_tmp362=({struct _tuple1*_tmp847=x;void*_tmp846=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp845=mexp;Cyc_Absyn_declare_stmt(_tmp847,_tmp846,_tmp845,({
struct Cyc_Absyn_Stmt*_tmp844=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp844,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2273
struct Cyc_Absyn_Stmt*s=_tmp362;
# 2277
if((unsigned)array_len)
s=Cyc_Absyn_declare_stmt(_check_null(lenvar),Cyc_Absyn_uint_type,array_len,s,0U);
({void*_tmp848=Cyc_Toc_stmt_exp_r(s);e->r=_tmp848;});
if(vse != 0)
({void*_tmp84A=({void*_tmp849=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp849,Cyc_Absyn_copy_exp(e));});e->r=_tmp84A;});
goto _LL7;}}}}}}}case 18: _tmp2B7=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2B7;
# 2285
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17: _tmp2B7=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;{void*t=_tmp2B7;
({void*_tmp84C=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363->tag=17,({void*_tmp84B=Cyc_Toc_typ_to_c(t);_tmp363->f1=_tmp84B;});_tmp363;});e->r=_tmp84C;});goto _LL7;}case 19: _tmp2B7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{void*t=_tmp2B7;struct Cyc_List_List*fs=_tmp2B8;
# 2288
void*_tmp364=t;void*t2=_tmp364;
struct Cyc_List_List*_tmp365=fs;struct Cyc_List_List*l=_tmp365;
for(1;l != 0;l=l->tl){
void*_tmp366=(void*)l->hd;void*_stmttmp3D=_tmp366;void*_tmp367=_stmttmp3D;unsigned _tmp368;void*_tmp369;if(*((int*)_tmp367)== 0){_tmp369=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp367)->f1;{struct _fat_ptr*n=_tmp369;
goto _LLD9;}}else{_tmp368=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp367)->f1;{unsigned n=_tmp368;
# 2294
{void*_tmp36A=Cyc_Absyn_compress(t2);void*_stmttmp3E=_tmp36A;void*_tmp36B=_stmttmp3E;void*_tmp36C;union Cyc_Absyn_AggrInfo _tmp36D;switch(*((int*)_tmp36B)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36B)->f1)){case 20: _tmp36D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36B)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp36D;
# 2296
struct Cyc_Absyn_Aggrdecl*_tmp36E=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp36E;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp370=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C4;_tmp6C4.tag=0,({struct _fat_ptr _tmp84D=({const char*_tmp371="struct fields must be known";_tag_fat(_tmp371,sizeof(char),28U);});_tmp6C4.f1=_tmp84D;});_tmp6C4;});void*_tmp36F[1];_tmp36F[0]=& _tmp370;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp36F,sizeof(void*),1));});
_tmp36C=(_check_null(ad->impl))->fields;goto _LLE2;}case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36B)->f1)->f1).KnownDatatypefield).tag == 2){_tmp36C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36B)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypefield*tuf=_tmp36C;
# 2310
if(n == 0U)
({void*_tmp84E=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->tag=0,_tmp375->f1=Cyc_Toc_tag_sp;_tmp375;}));l->hd=_tmp84E;});else{
# 2313
t2=(*({(struct _tuple23*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(tuf->typs,(int)(n - 1U))).f2;
({void*_tmp850=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376->tag=0,({struct _fat_ptr*_tmp84F=Cyc_Absyn_fieldname((int)n);_tmp376->f1=_tmp84F;});_tmp376;}));l->hd=_tmp850;});}
# 2316
goto _LLDE;}}else{goto _LLE7;}default: goto _LLE7;}case 7: _tmp36C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36B)->f2;_LLE2: {struct Cyc_List_List*fields=_tmp36C;
# 2301
struct Cyc_Absyn_Aggrfield*_tmp372=({(struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(fields,(int)n);struct Cyc_Absyn_Aggrfield*nth_field=_tmp372;
({void*_tmp851=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->tag=0,_tmp373->f1=nth_field->name;_tmp373;}));l->hd=_tmp851;});
t2=nth_field->type;
goto _LLDE;}case 6: _tmp36C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp36B)->f1;{struct Cyc_List_List*ts=_tmp36C;
# 2306
({void*_tmp853=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->tag=0,({struct _fat_ptr*_tmp852=Cyc_Absyn_fieldname((int)(n + 1U));_tmp374->f1=_tmp852;});_tmp374;}));l->hd=_tmp853;});
t2=(*({(struct _tuple23*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(ts,(int)n)).f2;
goto _LLDE;}default: _LLE7:
# 2317
({struct Cyc_Warn_String_Warn_Warg_struct _tmp378=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C5;_tmp6C5.tag=0,({struct _fat_ptr _tmp854=({const char*_tmp379="impossible type for offsetof tuple index";_tag_fat(_tmp379,sizeof(char),41U);});_tmp6C5.f1=_tmp854;});_tmp6C5;});void*_tmp377[1];_tmp377[0]=& _tmp378;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp377,sizeof(void*),1));});}_LLDE:;}
# 2319
goto _LLD9;}}_LLD9:;}
# 2322
({void*_tmp856=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A->tag=19,({void*_tmp855=Cyc_Toc_typ_to_c(t);_tmp37A->f1=_tmp855;}),_tmp37A->f2=fs;_tmp37A;});e->r=_tmp856;});
goto _LL7;}case 21: _tmp2B7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2B9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;_tmp2BB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2B6)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct _fat_ptr*f=_tmp2B8;int is_tagged=_tmp2B9;int is_read=_tmp2BB;
# 2325
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_tmp85B=({struct Cyc_Absyn_Exp*_tmp85A=e1;void*_tmp859=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp858=e1_cyc_type;struct _fat_ptr*_tmp857=f;Cyc_Toc_check_tagged_union(_tmp85A,_tmp859,_tmp858,_tmp857,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2329
e->r=_tmp85B;});
# 2331
if(is_poly)
({void*_tmp85D=({void*_tmp85C=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp85C,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp85D;});
goto _LL7;}case 22: _tmp2B7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2B9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;_tmp2BB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2B6)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct _fat_ptr*f=_tmp2B8;int is_tagged=_tmp2B9;int is_read=_tmp2BB;
# 2335
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=_check_null(e1->topt);
void*ta;
{void*_tmp37B=Cyc_Absyn_compress(e1typ);void*_stmttmp3F=_tmp37B;void*_tmp37C=_stmttmp3F;struct Cyc_Absyn_PtrInfo _tmp37D;if(*((int*)_tmp37C)== 3){_tmp37D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C)->f1;{struct Cyc_Absyn_PtrInfo p=_tmp37D;
ta=p.elt_type;goto _LLE9;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C6;_tmp6C6.tag=0,({struct _fat_ptr _tmp85E=({const char*_tmp380="get_ptr_typ: not a pointer!";_tag_fat(_tmp380,sizeof(char),28U);});_tmp6C6.f1=_tmp85E;});_tmp6C6;});void*_tmp37E[1];_tmp37E[0]=& _tmp37F;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp37E,sizeof(void*),1));});}_LLE9:;}
# 2342
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_tmp863=({struct Cyc_Absyn_Exp*_tmp862=e1;void*_tmp861=Cyc_Toc_typ_to_c(e1typ);void*_tmp860=ta;struct _fat_ptr*_tmp85F=f;Cyc_Toc_check_tagged_union(_tmp862,_tmp861,_tmp860,_tmp85F,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp863;});
if(is_poly && is_read)
({void*_tmp865=({void*_tmp864=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp864,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp865;});
goto _LL7;}case 20: _tmp2B7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2B7;
# 2350
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23: _tmp2B7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_Absyn_Exp*e2=_tmp2B8;
# 2354
{void*_tmp381=Cyc_Absyn_compress(_check_null(e1->topt));void*_stmttmp40=_tmp381;void*_tmp382=_stmttmp40;void*_tmp383;if(*((int*)_tmp382)== 6){_tmp383=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp382)->f1;{struct Cyc_List_List*ts=_tmp383;
# 2356
Cyc_Toc_exp_to_c(nv,e1);{
int _tmp384=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp384;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
struct _tuple13 _tmp385=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp41=_tmp385;struct _tuple13 _tmp386=_stmttmp41;int _tmp388;unsigned _tmp387;_tmp387=_tmp386.f1;_tmp388=_tmp386.f2;{unsigned i=_tmp387;int known=_tmp388;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C7;_tmp6C7.tag=0,({struct _fat_ptr _tmp866=({const char*_tmp38B="unknown tuple subscript in translation to C";_tag_fat(_tmp38B,sizeof(char),44U);});_tmp6C7.f1=_tmp866;});_tmp6C7;});void*_tmp389[1];_tmp389[0]=& _tmp38A;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp389,sizeof(void*),1));});
({void*_tmp868=({struct Cyc_Absyn_Exp*_tmp867=e1;Cyc_Toc_aggrmember_exp_r(_tmp867,Cyc_Absyn_fieldname((int)(i + 1U)));});e->r=_tmp868;});
goto _LLEE;}}}}}else{
# 2366
did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_tmp869=Cyc_Toc_deref_exp_r(e1);e->r=_tmp869;});
goto _LLEE;}}_LLEE:;}
# 2372
goto _LL7;}case 24: _tmp2B7=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;{struct Cyc_List_List*es=_tmp2B7;
# 2374
if(!Cyc_Toc_is_toplevel(nv)){
# 2376
void*_tmp38C=Cyc_Toc_add_tuple_type(({(struct Cyc_List_List*(*)(struct _tuple23*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Toc_tup_to_c,es));void*strct_typ=_tmp38C;
void*_tmp38D=strct_typ;union Cyc_Absyn_AggrInfo _tmp38E;if(*((int*)_tmp38D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38D)->f1)== 20){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38D)->f2 == 0){_tmp38E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38D)->f1)->f1;{union Cyc_Absyn_AggrInfo aggrinfo=_tmp38E;
# 2379
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp38F=_cycalloc(sizeof(*_tmp38F));({struct _tuple19*_tmp86B=({struct _fat_ptr*_tmp86A=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp86A,(struct Cyc_Absyn_Exp*)es->hd);});_tmp38F->hd=_tmp86B;}),_tmp38F->tl=dles;_tmp38F;});}}
# 2385
({void*_tmp86D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->tag=29,_tmp390->f1=sd->name,_tmp390->f2=0,({struct Cyc_List_List*_tmp86C=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(dles);_tmp390->f3=_tmp86C;}),_tmp390->f4=sd;_tmp390;});e->r=_tmp86D;});
e->topt=strct_typ;
goto _LLF6;}}else{goto _LLF9;}}else{goto _LLF9;}}else{_LLF9:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp392=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C8;_tmp6C8.tag=0,({struct _fat_ptr _tmp86E=({const char*_tmp393="tuple type not an aggregate";_tag_fat(_tmp393,sizeof(char),28U);});_tmp6C8.f1=_tmp86E;});_tmp6C8;});void*_tmp391[1];_tmp391[0]=& _tmp392;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp391,sizeof(void*),1));});}_LLF6:;}else{
# 2393
struct Cyc_List_List*dles=0;
for(1;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp395=_cycalloc(sizeof(*_tmp395));({struct _tuple19*_tmp86F=({struct _tuple19*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->f1=0,_tmp394->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp394;});_tmp395->hd=_tmp86F;}),_tmp395->tl=dles;_tmp395;});}
# 2398
({void*_tmp870=Cyc_Toc_unresolvedmem_exp_r(0,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(dles));e->r=_tmp870;});}
# 2400
goto _LL7;}case 26: _tmp2B7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;{struct Cyc_List_List*dles0=_tmp2B7;
# 2406
{struct Cyc_List_List*_tmp396=dles0;struct Cyc_List_List*dles=_tmp396;for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp871=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_tmp871;});
goto _LL7;}case 27: _tmp2B7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2BA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;_tmp2B9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2B6)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmp2B7;struct Cyc_Absyn_Exp*e1=_tmp2B8;struct Cyc_Absyn_Exp*e2=_tmp2BA;int iszeroterm=_tmp2B9;
# 2414
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp397=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple13 _stmttmp42=_tmp397;struct _tuple13 _tmp398=_stmttmp42;int _tmp39A;unsigned _tmp399;_tmp399=_tmp398.f1;_tmp39A=_tmp398.f2;{unsigned sz=_tmp399;int known=_tmp39A;
void*_tmp39B=Cyc_Toc_typ_to_c(_check_null(e2->topt));void*elt_typ=_tmp39B;
struct Cyc_List_List*es=0;
# 2421
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp39D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C9;_tmp6C9.tag=0,({struct _fat_ptr _tmp872=({const char*_tmp39E="cannot determine value of constant";_tag_fat(_tmp39E,sizeof(char),35U);});_tmp6C9.f1=_tmp872;});_tmp6C9;});void*_tmp39C[1];_tmp39C[0]=& _tmp39D;Cyc_Warn_err2(e1->loc,_tag_fat(_tmp39C,sizeof(void*),1));});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({struct _tuple19*_tmp873=({struct _tuple19*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->f1=0,_tmp39F->f2=e2;_tmp39F;});_tmp3A0->hd=_tmp873;}),_tmp3A0->tl=es;_tmp3A0;});}}
# 2427
if(iszeroterm){
struct Cyc_Absyn_Exp*_tmp3A1=({void*_tmp874=elt_typ;Cyc_Toc_cast_it(_tmp874,Cyc_Absyn_uint_exp(0U,0U));});struct Cyc_Absyn_Exp*ezero=_tmp3A1;
es=({struct Cyc_List_List*(*_tmp877)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp876=es;_tmp877(_tmp876,({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct _tuple19*_tmp875=({struct _tuple19*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->f1=0,_tmp3A2->f2=ezero;_tmp3A2;});_tmp3A3->hd=_tmp875;}),_tmp3A3->tl=0;_tmp3A3;}));});}}
# 2432
({void*_tmp878=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp878;});}}
# 2434
goto _LL7;}case 28: _tmp2B7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2B9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;{struct Cyc_Absyn_Exp*e1=_tmp2B7;void*t1=_tmp2B8;int iszeroterm=_tmp2B9;
# 2441
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp879=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp879;});else{
# 2444
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 30: _tmp2B7=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{void*st=_tmp2B7;struct Cyc_List_List*dles=_tmp2B8;
# 2448
{struct Cyc_List_List*_tmp3A4=dles;struct Cyc_List_List*dles2=_tmp3A4;for(0;dles2 != 0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles2->hd)).f2);}}{
void*_tmp3A5=Cyc_Toc_typ_to_c(_check_null(e->topt));void*strct_typ=_tmp3A5;
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3A6=Cyc_Absyn_compress(strct_typ);void*_stmttmp43=_tmp3A6;void*_tmp3A7=_stmttmp43;union Cyc_Absyn_AggrInfo _tmp3A8;if(*((int*)_tmp3A7)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7)->f1)== 20){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7)->f2 == 0){_tmp3A8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7)->f1)->f1;{union Cyc_Absyn_AggrInfo aggrinfo=_tmp3A8;
# 2455
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_tmp87A=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->tag=29,_tmp3A9->f1=sd->name,_tmp3A9->f2=0,_tmp3A9->f3=dles,_tmp3A9->f4=sd;_tmp3A9;});e->r=_tmp87A;});
e->topt=strct_typ;
goto _LLFE;}}else{goto _LL101;}}else{goto _LL101;}}else{_LL101:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6CA;_tmp6CA.tag=0,({struct _fat_ptr _tmp87B=({const char*_tmp3AC="anonStruct type not an aggregate";_tag_fat(_tmp3AC,sizeof(char),33U);});_tmp6CA.f1=_tmp87B;});_tmp6CA;});void*_tmp3AA[1];_tmp3AA[0]=& _tmp3AB;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp3AA,sizeof(void*),1));});}_LLFE:;}else{
# 2462
({void*_tmp87C=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmp87C;});}
goto _LL7;}}case 29: _tmp2B7=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2BA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;_tmp2BD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2B6)->f4;{struct _tuple1**tdn=_tmp2B7;struct Cyc_List_List*exist_ts=_tmp2B8;struct Cyc_List_List*dles=_tmp2BA;struct Cyc_Absyn_Aggrdecl*sd=_tmp2BD;
# 2468
if(sd == 0 || sd->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6CB;_tmp6CB.tag=0,({struct _fat_ptr _tmp87D=({const char*_tmp3AF="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_tmp3AF,sizeof(char),58U);});_tmp6CB.f1=_tmp87D;});_tmp6CB;});void*_tmp3AD[1];_tmp3AD[0]=& _tmp3AE;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp3AD,sizeof(void*),1));});{
void*_tmp3B0=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp3B0;
{void*_tmp3B1=Cyc_Absyn_compress(new_typ);void*_stmttmp44=_tmp3B1;void*_tmp3B2=_stmttmp44;union Cyc_Absyn_AggrInfo _tmp3B3;if(*((int*)_tmp3B2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2)->f1)== 20){_tmp3B3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp3B3;
({struct _tuple1*_tmp87E=(Cyc_Absyn_aggr_kinded_name(info)).f2;*tdn=_tmp87E;});goto _LL103;}}else{goto _LL106;}}else{_LL106:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6CC;_tmp6CC.tag=0,({struct _fat_ptr _tmp87F=({const char*_tmp3B6="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_tmp3B6,sizeof(char),44U);});_tmp6CC.f1=_tmp87F;});_tmp6CC;});void*_tmp3B4[1];_tmp3B4[0]=& _tmp3B5;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp3B4,sizeof(void*),1));});}_LL103:;}{
# 2479
struct Cyc_List_List*_tmp3B7=(_check_null(sd->impl))->fields;struct Cyc_List_List*typ_fields=_tmp3B7;
if(typ_fields == 0)goto _LL7;
for(1;(_check_null(typ_fields))->tl != 0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp3B8=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;struct Cyc_Absyn_Aggrfield*last_typ_field=_tmp3B8;
struct Cyc_List_List*_tmp3B9=({(struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators;})(Cyc_Core_heap_region,e->loc,dles,sd->kind,(
_check_null(sd->impl))->fields);
# 2484
struct Cyc_List_List*fields=_tmp3B9;
# 2486
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3BA=Cyc_Absyn_compress(old_typ);void*_stmttmp45=_tmp3BA;void*_tmp3BB=_stmttmp45;void*_tmp3BC;if(*((int*)_tmp3BB)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BB)->f1)== 20){_tmp3BC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BB)->f2;{struct Cyc_List_List*param_ts=_tmp3BC;
# 2490
{struct Cyc_List_List*_tmp3BD=fields;struct Cyc_List_List*fields2=_tmp3BD;for(0;fields2 != 0;fields2=fields2->tl){
struct _tuple36*_tmp3BE=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp46=_tmp3BE;struct _tuple36*_tmp3BF=_stmttmp46;void*_tmp3C1;void*_tmp3C0;_tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;{struct Cyc_Absyn_Aggrfield*aggrfield=_tmp3C0;struct Cyc_Absyn_Exp*fieldexp=_tmp3C1;
void*_tmp3C2=fieldexp->topt;void*old_field_typ=_tmp3C2;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2495
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(_check_null(fieldexp->topt)))
({void*_tmp881=({void*_tmp880=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_tmp880,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2497
fieldexp->r=_tmp881;});
# 2500
if((_check_null(sd->impl))->tagged){
struct _fat_ptr*_tmp3C3=aggrfield->name;struct _fat_ptr*n=_tmp3C3;
struct Cyc_Absyn_Exp*_tmp3C4=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);struct Cyc_Absyn_Exp*tag_exp=_tmp3C4;
struct _tuple19*_tmp3C5=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp3C5;
struct _tuple19*_tmp3C6=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);struct _tuple19*val_dle=_tmp3C6;
struct _tuple1*s=({struct _tuple1*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({union Cyc_Absyn_Nmspace _tmp885=Cyc_Absyn_Abs_n(0,1);_tmp3D1->f1=_tmp885;}),({
struct _fat_ptr*_tmp884=({struct _fat_ptr*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({struct _fat_ptr _tmp883=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3CE=({struct Cyc_String_pa_PrintArg_struct _tmp6CE;_tmp6CE.tag=0,_tmp6CE.f1=(struct _fat_ptr)((struct _fat_ptr)*(*sd->name).f2);_tmp6CE;});struct Cyc_String_pa_PrintArg_struct _tmp3CF=({struct Cyc_String_pa_PrintArg_struct _tmp6CD;_tmp6CD.tag=0,_tmp6CD.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp6CD;});void*_tmp3CC[2];_tmp3CC[0]=& _tmp3CE,_tmp3CC[1]=& _tmp3CF;({struct _fat_ptr _tmp882=({const char*_tmp3CD="_union_%s_%s";_tag_fat(_tmp3CD,sizeof(char),13U);});Cyc_aprintf(_tmp882,_tag_fat(_tmp3CC,sizeof(void*),2));});});*_tmp3D0=_tmp883;});_tmp3D0;});_tmp3D1->f2=_tmp884;});_tmp3D1;});
# 2508
struct _tuple19*_tmp3C7=({
struct _fat_ptr*_tmp887=n;Cyc_Toc_make_field(_tmp887,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->tag=29,_tmp3CB->f1=s,_tmp3CB->f2=0,({struct Cyc_List_List*_tmp886=({struct _tuple19*_tmp3CA[2];_tmp3CA[0]=tag_dle,_tmp3CA[1]=val_dle;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp3CA,sizeof(struct _tuple19*),2));});_tmp3CB->f3=_tmp886;}),_tmp3CB->f4=0;_tmp3CB;}),0U));});
# 2508
struct _tuple19*u_dle=_tmp3C7;
# 2511
({void*_tmp889=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->tag=29,_tmp3C9->f1=*tdn,_tmp3C9->f2=0,({struct Cyc_List_List*_tmp888=({struct _tuple19*_tmp3C8[1];_tmp3C8[0]=u_dle;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp3C8,sizeof(struct _tuple19*),1));});_tmp3C9->f3=_tmp888;}),_tmp3C9->f4=sd;_tmp3C9;});e->r=_tmp889;});}
# 2517
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field == aggrfield){
struct Cyc_List_List*_tmp3D2=({struct Cyc_List_List*(*_tmp88B)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp88A=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip;})((_check_null(sd->impl))->exist_vars,exist_ts);_tmp88B(_tmp88A,
({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip;})(sd->tvs,param_ts));});
# 2518
struct Cyc_List_List*inst=_tmp3D2;
# 2520
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp3D3=(_check_null(sd->impl))->fields;struct Cyc_List_List*fs=_tmp3D3;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp3D4=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*old_f=_tmp3D4;
void*_tmp3D5=Cyc_Tcutil_substitute(inst,old_f->type);void*old_ftyp=_tmp3D5;
struct Cyc_Absyn_Aggrfield*_tmp3D6=({struct Cyc_Absyn_Aggrfield*_tmp88C=old_f;Cyc_Toc_aggrfield_to_c(_tmp88C,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
# 2524
struct Cyc_Absyn_Aggrfield*new_f=_tmp3D6;
# 2526
new_fields=({struct Cyc_List_List*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->hd=new_f,_tmp3D7->tl=new_fields;_tmp3D7;});
# 2532
if(fs->tl == 0){
{void*_tmp3D8=Cyc_Absyn_compress(new_f->type);void*_stmttmp47=_tmp3D8;void*_tmp3D9=_stmttmp47;struct Cyc_Absyn_ArrayInfo _tmp3DA;if(*((int*)_tmp3D9)== 4){_tmp3DA=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D9)->f1;{struct Cyc_Absyn_ArrayInfo ai=_tmp3DA;
# 2535
if(!Cyc_Evexp_c_can_eval(_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp3DB=ai;struct Cyc_Absyn_ArrayInfo ai2=_tmp3DB;
({struct Cyc_Absyn_Exp*_tmp88D=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_tmp88D;});
({void*_tmp88E=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->tag=4,_tmp3DC->f1=ai2;_tmp3DC;});new_f->type=_tmp88E;});}
# 2540
goto _LL110;}}else{
# 2544
if(fieldexp->topt == 0)
goto _LL110;
{void*_tmp3DD=Cyc_Absyn_compress(_check_null(fieldexp->topt));void*_stmttmp48=_tmp3DD;void*_tmp3DE=_stmttmp48;if(*((int*)_tmp3DE)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DE)->f1)== 20){
# 2548
new_f->type=_check_null(fieldexp->topt);goto _LL115;}else{goto _LL118;}}else{_LL118:
 goto _LL115;}_LL115:;}
# 2551
goto _LL110;}_LL110:;}
# 2555
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_tmp890=({struct Cyc_List_List*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({void*_tmp88F=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->tag=6,_tmp3DF->f1=0;_tmp3DF;});_tmp3E0->hd=_tmp88F;}),_tmp3E0->tl=new_f->attributes;_tmp3E0;});new_f->attributes=_tmp890;});}}}
# 2561
sd=({struct _fat_ptr*_tmp893=({struct _fat_ptr*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));({struct _fat_ptr _tmp892=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3E3=({struct Cyc_Int_pa_PrintArg_struct _tmp6CF;_tmp6CF.tag=1,_tmp6CF.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp6CF;});void*_tmp3E1[1];_tmp3E1[0]=& _tmp3E3;({struct _fat_ptr _tmp891=({const char*_tmp3E2="_genStruct%d";_tag_fat(_tmp3E2,sizeof(char),13U);});Cyc_aprintf(_tmp891,_tag_fat(_tmp3E1,sizeof(void*),1));});});*_tmp3E4=_tmp892;});_tmp3E4;});Cyc_Toc_make_c_struct_defn(_tmp893,
# 2563
({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2567
({void*_tmp894=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3E5=_cycalloc(sizeof(*_tmp3E5));*_tmp3E5=sd;_tmp3E5;})),0);e->topt=_tmp894;});}}}}
# 2570
goto _LL108;}}else{goto _LL10B;}}else{_LL10B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D0;_tmp6D0.tag=0,({struct _fat_ptr _tmp895=({const char*_tmp3E8="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_tmp3E8,sizeof(char),39U);});_tmp6D0.f1=_tmp895;});_tmp6D0;});void*_tmp3E6[1];_tmp3E6[0]=& _tmp3E7;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp3E6,sizeof(void*),1));});}_LL108:;}else{
# 2579
if((_check_null(sd->impl))->tagged){
# 2581
struct _tuple36*_tmp3E9=(struct _tuple36*)(_check_null(fields))->hd;struct _tuple36*_stmttmp49=_tmp3E9;struct _tuple36*_tmp3EA=_stmttmp49;void*_tmp3EC;void*_tmp3EB;_tmp3EB=_tmp3EA->f1;_tmp3EC=_tmp3EA->f2;{struct Cyc_Absyn_Aggrfield*field=_tmp3EB;struct Cyc_Absyn_Exp*fieldexp=_tmp3EC;
void*_tmp3ED=_check_null(fieldexp->topt);void*fieldexp_type=_tmp3ED;
void*_tmp3EE=field->type;void*fieldtyp=_tmp3EE;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_tmp897=({
void*_tmp896=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp896,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2587
fieldexp->r=_tmp897;});{
# 2590
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp3EF=({struct _tuple19*_tmp3F1[2];({struct _tuple19*_tmp899=({struct _tuple19*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2->f1=0,_tmp3F2->f2=field_tag_exp;_tmp3F2;});_tmp3F1[0]=_tmp899;}),({struct _tuple19*_tmp898=({struct _tuple19*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3->f1=0,_tmp3F3->f2=fieldexp;_tmp3F3;});_tmp3F1[1]=_tmp898;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp3F1,sizeof(struct _tuple19*),2));});struct Cyc_List_List*newdles=_tmp3EF;
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_tmp89B=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp3F0[1];({struct _tuple19*_tmp89A=Cyc_Toc_make_field(field->name,umem);_tmp3F0[0]=_tmp89A;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp3F0,sizeof(struct _tuple19*),1));}));e->r=_tmp89B;});}}}else{
# 2597
struct Cyc_List_List*_tmp3F4=0;struct Cyc_List_List*newdles=_tmp3F4;
struct Cyc_List_List*_tmp3F5=(_check_null(sd->impl))->fields;struct Cyc_List_List*sdfields=_tmp3F5;
for(1;sdfields != 0;sdfields=sdfields->tl){
struct Cyc_List_List*_tmp3F6=fields;struct Cyc_List_List*fields2=_tmp3F6;for(0;fields2 != 0;fields2=fields2->tl){
if((*((struct _tuple36*)fields2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple36*_tmp3F7=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp4A=_tmp3F7;struct _tuple36*_tmp3F8=_stmttmp4A;void*_tmp3FA;void*_tmp3F9;_tmp3F9=_tmp3F8->f1;_tmp3FA=_tmp3F8->f2;{struct Cyc_Absyn_Aggrfield*field=_tmp3F9;struct Cyc_Absyn_Exp*fieldexp=_tmp3FA;
void*_tmp3FB=Cyc_Toc_typ_to_c(field->type);void*fieldtyp=_tmp3FB;
void*_tmp3FC=Cyc_Toc_typ_to_c(_check_null(fieldexp->topt));void*fieldexp_typ=_tmp3FC;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2607
if(!Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2609
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_tmp89C=fieldtyp;Cyc_Toc_cast_it(_tmp89C,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));({struct _tuple19*_tmp89D=({struct _tuple19*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->f1=0,_tmp3FD->f2=fieldexp;_tmp3FD;});_tmp3FE->hd=_tmp89D;}),_tmp3FE->tl=newdles;_tmp3FE;});
break;}}}}
# 2616
({void*_tmp89E=Cyc_Toc_unresolvedmem_exp_r(0,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(newdles));e->r=_tmp89E;});}}
# 2619
goto _LL7;}}}}case 31: _tmp2B7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;_tmp2BA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;{struct Cyc_List_List*es=_tmp2B7;struct Cyc_Absyn_Datatypedecl*tud=_tmp2B8;struct Cyc_Absyn_Datatypefield*tuf=_tmp2BA;
# 2623
struct Cyc_List_List*_tmp3FF=tuf->typs;struct Cyc_List_List*tqts=_tmp3FF;
{struct Cyc_List_List*_tmp400=es;struct Cyc_List_List*es2=_tmp400;for(0;es2 != 0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*_tmp401=_check_null(cur_e->topt);void*cur_e_typ=_tmp401;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)(_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_tmp8A0=({void*_tmp89F=field_typ;Cyc_Toc_cast_it(_tmp89F,Cyc_Absyn_copy_exp(cur_e));})->r;cur_e->r=_tmp8A0;});}}{
# 2634
struct Cyc_Absyn_Exp*_tmp402=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2634
struct Cyc_Absyn_Exp*tag_exp=_tmp402;
# 2637
if(!Cyc_Toc_is_toplevel(nv)){
# 2639
struct Cyc_List_List*dles=0;
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_tmp403=_cycalloc(sizeof(*_tmp403));({struct _tuple19*_tmp8A2=({struct _fat_ptr*_tmp8A1=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp8A1,(struct Cyc_Absyn_Exp*)es->hd);});_tmp403->hd=_tmp8A2;}),_tmp403->tl=dles;_tmp403;});}}{
# 2645
struct _tuple19*_tmp404=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp404;
({void*_tmp8A6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->tag=29,({struct _tuple1*_tmp8A5=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_tmp406->f1=_tmp8A5;}),_tmp406->f2=0,({
struct Cyc_List_List*_tmp8A4=({struct Cyc_List_List*_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->hd=tag_dle,({struct Cyc_List_List*_tmp8A3=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(dles);_tmp405->tl=_tmp8A3;});_tmp405;});_tmp406->f3=_tmp8A4;}),_tmp406->f4=0;_tmp406;});
# 2646
e->r=_tmp8A6;});}}else{
# 2651
struct Cyc_List_List*_tmp407=0;struct Cyc_List_List*dles=_tmp407;
for(1;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp409=_cycalloc(sizeof(*_tmp409));({struct _tuple19*_tmp8A7=({struct _tuple19*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->f1=0,_tmp408->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp408;});_tmp409->hd=_tmp8A7;}),_tmp409->tl=dles;_tmp409;});}
({void*_tmp8AA=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp40B=_cycalloc(sizeof(*_tmp40B));({struct _tuple19*_tmp8A9=({struct _tuple19*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->f1=0,_tmp40A->f2=tag_exp;_tmp40A;});_tmp40B->hd=_tmp8A9;}),({
struct Cyc_List_List*_tmp8A8=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(dles);_tmp40B->tl=_tmp8A8;});_tmp40B;}));
# 2654
e->r=_tmp8AA;});}
# 2657
goto _LL7;}}case 32:
# 2659
 goto _LL51;case 33: _LL51:
 goto _LL7;case 34: _tmp2B9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1).is_calloc;_tmp2B7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1).rgn;_tmp2B8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1).elt_type;_tmp2BA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1).num_elts;_tmp2BB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1).fat_result;_tmp2BC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1).inline_call;{int is_calloc=_tmp2B9;struct Cyc_Absyn_Exp*rgnopt=_tmp2B7;void**topt=_tmp2B8;struct Cyc_Absyn_Exp*num_elts=_tmp2BA;int is_fat=_tmp2BB;int inline_call=_tmp2BC;
# 2663
void*t_c=Cyc_Toc_typ_to_c(*_check_null(topt));
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;
if(is_fat){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2672
if(is_calloc){
if(rgnopt != 0 && !Cyc_Flags_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_tmp40C[3];_tmp40C[0]=rgnopt,({
struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp40C[1]=_tmp8AB;}),_tmp40C[2]=num_elts2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_calloc_e,_tag_fat(_tmp40C,sizeof(struct Cyc_Absyn_Exp*),3));});}else{
# 2678
pexp=({void*_tmp8AD=*topt;struct Cyc_Absyn_Exp*_tmp8AC=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp8AD,_tmp8AC,num_elts2);});}}else{
# 2680
if(rgnopt != 0 && !Cyc_Flags_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);{
struct Cyc_Absyn_Exp*(*_tmp40D)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=inline_call?Cyc_Toc_rmalloc_inline_exp: Cyc_Toc_rmalloc_exp;struct Cyc_Absyn_Exp*(*fn_e)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=_tmp40D;
pexp=fn_e(rgnopt,num_elts2);}}else{
# 2685
pexp=Cyc_Toc_malloc_exp(*topt,num_elts2);}}
# 2687
if(is_fat){
struct Cyc_Absyn_Exp*elt_sz=is_calloc?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp40E[3];_tmp40E[0]=pexp,_tmp40E[1]=elt_sz,_tmp40E[2]=num_elts2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_tmp40E,sizeof(struct Cyc_Absyn_Exp*),3));});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp8B0=_check_null(x);void*_tmp8AF=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp8AE=num_elts;Cyc_Absyn_declare_stmt(_tmp8B0,_tmp8AF,_tmp8AE,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp8B1=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8B1;});}else{
# 2694
e->r=pexp->r;}
goto _LL7;}}case 35: _tmp2B7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct Cyc_Absyn_Exp*e2=_tmp2B8;
# 2704
void*e1_old_typ=_check_null(e1->topt);
void*e2_old_typ=_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp410=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D1;_tmp6D1.tag=0,({struct _fat_ptr _tmp8B2=({const char*_tmp411="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_tmp411,sizeof(char),57U);});_tmp6D1.f1=_tmp8B2;});_tmp6D1;});void*_tmp40F[1];_tmp40F[0]=& _tmp410;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp40F,sizeof(void*),1));});{
# 2712
unsigned _tmp412=e->loc;unsigned loc=_tmp412;
struct _tuple1*_tmp413=Cyc_Toc_temp_var();struct _tuple1*v1=_tmp413;
struct Cyc_Absyn_Exp*_tmp414=Cyc_Absyn_var_exp(v1,loc);struct Cyc_Absyn_Exp*v1e=_tmp414;v1e->topt=e1_old_typ;{
struct _tuple1*_tmp415=Cyc_Toc_temp_var();struct _tuple1*v2=_tmp415;
struct Cyc_Absyn_Exp*_tmp416=Cyc_Absyn_var_exp(v2,loc);struct Cyc_Absyn_Exp*v2e=_tmp416;v2e->topt=e2_old_typ;{
# 2718
struct Cyc_Absyn_Exp*_tmp417=({struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_tmp8B3=v2e;Cyc_Absyn_assign_exp(_tmp8B4,_tmp8B3,loc);});struct Cyc_Absyn_Exp*s1e=_tmp417;s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp418=Cyc_Absyn_exp_stmt(s1e,loc);struct Cyc_Absyn_Stmt*s1=_tmp418;
struct Cyc_Absyn_Exp*_tmp419=({struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_tmp8B5=v1e;Cyc_Absyn_assign_exp(_tmp8B6,_tmp8B5,loc);});struct Cyc_Absyn_Exp*s2e=_tmp419;s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp41A=Cyc_Absyn_exp_stmt(s2e,loc);struct Cyc_Absyn_Stmt*s2=_tmp41A;
# 2723
struct Cyc_Absyn_Stmt*_tmp41B=({struct _tuple1*_tmp8BE=v1;void*_tmp8BD=e1_old_typ;struct Cyc_Absyn_Exp*_tmp8BC=e1;struct Cyc_Absyn_Stmt*_tmp8BB=({
struct _tuple1*_tmp8BA=v2;void*_tmp8B9=e2_old_typ;struct Cyc_Absyn_Exp*_tmp8B8=e2;struct Cyc_Absyn_Stmt*_tmp8B7=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2724
Cyc_Absyn_declare_stmt(_tmp8BA,_tmp8B9,_tmp8B8,_tmp8B7,loc);});
# 2723
Cyc_Absyn_declare_stmt(_tmp8BE,_tmp8BD,_tmp8BC,_tmp8BB,loc);});struct Cyc_Absyn_Stmt*s=_tmp41B;
# 2726
Cyc_Toc_stmt_to_c(nv,s);
({void*_tmp8BF=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8BF;});
goto _LL7;}}}}}}case 38: _tmp2B7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2B6)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2B7;struct _fat_ptr*f=_tmp2B8;
# 2731
void*_tmp41C=Cyc_Absyn_compress(_check_null(e1->topt));void*e1_typ=_tmp41C;
Cyc_Toc_exp_to_c(nv,e1);
{void*_tmp41D=e1_typ;void*_tmp41E;if(*((int*)_tmp41D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41D)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41D)->f1)->f1).KnownAggr).tag == 2){_tmp41E=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41D)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp41E;
# 2735
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp41F;
struct Cyc_Absyn_Exp*_tmp420=Cyc_Toc_member_exp(e1,f,0U);struct Cyc_Absyn_Exp*e1_f=_tmp420;
struct Cyc_Absyn_Exp*_tmp421=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*e1_f_tag=_tmp421;
({void*_tmp8C0=(Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U))->r;e->r=_tmp8C0;});
goto _LL120;}}else{goto _LL123;}}else{goto _LL123;}}else{_LL123:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp423=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D2;_tmp6D2.tag=0,({struct _fat_ptr _tmp8C1=({const char*_tmp424="non-aggregate type in tagcheck";_tag_fat(_tmp424,sizeof(char),31U);});_tmp6D2.f1=_tmp8C1;});_tmp6D2;});void*_tmp422[1];_tmp422[0]=& _tmp423;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp422,sizeof(void*),1));});}_LL120:;}
# 2742
goto _LL7;}case 37: _tmp2B7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2B6)->f1;{struct Cyc_Absyn_Stmt*s=_tmp2B7;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 36:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp426=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D3;_tmp6D3.tag=0,({struct _fat_ptr _tmp8C2=({const char*_tmp427="UnresolvedMem";_tag_fat(_tmp427,sizeof(char),14U);});_tmp6D3.f1=_tmp8C2;});_tmp6D3;});void*_tmp425[1];_tmp425[0]=& _tmp426;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp425,sizeof(void*),1));});case 25:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp429=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D4;_tmp6D4.tag=0,({struct _fat_ptr _tmp8C3=({const char*_tmp42A="compoundlit";_tag_fat(_tmp42A,sizeof(char),12U);});_tmp6D4.f1=_tmp8C3;});_tmp6D4;});void*_tmp428[1];_tmp428[0]=& _tmp429;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp428,sizeof(void*),1));});case 39:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp42C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D5;_tmp6D5.tag=0,({struct _fat_ptr _tmp8C4=({const char*_tmp42D="valueof(-)";_tag_fat(_tmp42D,sizeof(char),11U);});_tmp6D5.f1=_tmp8C4;});_tmp6D5;});void*_tmp42B[1];_tmp42B[0]=& _tmp42C;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp42B,sizeof(void*),1));});default: _tmp2B7=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2B6)->f3;_tmp2B8=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2B6)->f4;{struct Cyc_List_List*o=_tmp2B7;struct Cyc_List_List*i=_tmp2B8;
# 2748
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL7;}}_LL7:;}
# 2752
if(!did_inserted_checks)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp42F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D7;_tmp6D7.tag=0,({struct _fat_ptr _tmp8C5=({const char*_tmp431="Toc did not examine an inserted check: ";_tag_fat(_tmp431,sizeof(char),40U);});_tmp6D7.f1=_tmp8C5;});_tmp6D7;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp430=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp6D6;_tmp6D6.tag=4,_tmp6D6.f1=e;_tmp6D6;});void*_tmp42E[2];_tmp42E[0]=& _tmp42F,_tmp42E[1]=& _tmp430;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(e->loc,_tag_fat(_tmp42E,sizeof(void*),2));});}}struct _tuple37{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2781 "toc.cyc"
static struct _tuple37*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2783
return({struct _tuple37*_tmp432=_region_malloc(r,sizeof(*_tmp432));_tmp432->f1=0,({struct _fat_ptr*_tmp8C7=Cyc_Toc_fresh_label();_tmp432->f2=_tmp8C7;}),({struct _fat_ptr*_tmp8C6=Cyc_Toc_fresh_label();_tmp432->f3=_tmp8C6;}),_tmp432->f4=sc;_tmp432;});}
# 2789
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(1;ps != 0;ps=(_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp433=(struct Cyc_Tcpat_PathNode*)ps->hd;struct Cyc_Tcpat_PathNode*p=_tmp433;
# 2795
if((int)(((p->orig_pat).pattern).tag == 1)){
void*t=_check_null(({union Cyc_Tcpat_PatOrWhere _tmp436=p->orig_pat;if((_tmp436.pattern).tag != 1)_throw_match();(_tmp436.pattern).val;})->topt);
void*_tmp434=Cyc_Absyn_compress(Cyc_Toc_typ_to_c_array(t));void*t2=_tmp434;
void*_tmp435=t2;switch(*((int*)_tmp435)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp435)->f1)){case 0:
# 2800
 goto _LL4;case 20: _LL4:
 goto _LL6;default: goto _LL7;}case 7: _LL6:
 goto _LL0;default: _LL7:
 v=({void*_tmp8C8=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp8C8,v);});goto _LL0;}_LL0:;}{
# 2806
void*_tmp437=p->access;void*_stmttmp4B=_tmp437;void*_tmp438=_stmttmp4B;void*_tmp43A;int _tmp439;unsigned _tmp43B;switch(*((int*)_tmp438)){case 0:
# 2810
 goto _LL9;case 1:
# 2815
 if(ps->tl != 0){
void*_tmp43C=((struct Cyc_Tcpat_PathNode*)(_check_null(ps->tl))->hd)->access;void*_stmttmp4C=_tmp43C;void*_tmp43D=_stmttmp4C;unsigned _tmp440;void*_tmp43F;void*_tmp43E;if(*((int*)_tmp43D)== 3){_tmp43E=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp43D)->f1;_tmp43F=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp43D)->f2;_tmp440=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp43D)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_tmp43E;struct Cyc_Absyn_Datatypefield*tuf=_tmp43F;unsigned i=_tmp440;
# 2818
ps=ps->tl;
v=({void*_tmp8CA=({void*_tmp8C9=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmp8C9,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp8CA,v);});
v=({struct Cyc_Absyn_Exp*_tmp8CB=v;Cyc_Absyn_aggrarrow_exp(_tmp8CB,Cyc_Absyn_fieldname((int)(i + 1U)),0U);});
continue;}}else{
goto _LL14;}_LL14:;}
# 2824
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3: _tmp43B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp438)->f3;{unsigned i=_tmp43B;
_tmp43B=i;goto _LL11;}case 2: _tmp43B=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp438)->f1;_LL11: {unsigned i=_tmp43B;
v=({struct Cyc_Absyn_Exp*_tmp8CC=v;Cyc_Toc_member_exp(_tmp8CC,Cyc_Absyn_fieldname((int)(i + 1U)),0U);});goto _LL9;}default: _tmp439=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp438)->f1;_tmp43A=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp438)->f2;{int tagged=_tmp439;struct _fat_ptr*f=_tmp43A;
# 2829
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2835
return v;}
# 2840
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp441=t;unsigned _tmp445;int _tmp444;struct _fat_ptr _tmp446;void*_tmp443;void*_tmp442;switch(*((int*)_tmp441)){case 0: _tmp442=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp441)->f1;{struct Cyc_Absyn_Exp*e=_tmp442;
return e == 0?v: e;}case 1:
 return({struct Cyc_Absyn_Exp*_tmp8CD=v;Cyc_Absyn_eq_exp(_tmp8CD,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2:
 return({struct Cyc_Absyn_Exp*_tmp8CE=v;Cyc_Absyn_neq_exp(_tmp8CE,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3: _tmp442=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp441)->f1;_tmp443=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp441)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp442;struct Cyc_Absyn_Enumfield*ef=_tmp443;
return({struct Cyc_Absyn_Exp*_tmp8CF=v;Cyc_Absyn_eq_exp(_tmp8CF,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->tag=32,_tmp447->f1=ed,_tmp447->f2=ef;_tmp447;}),0U),0U);});}case 4: _tmp442=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp441)->f1;_tmp443=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp441)->f2;{void*t=_tmp442;struct Cyc_Absyn_Enumfield*ef=_tmp443;
return({struct Cyc_Absyn_Exp*_tmp8D0=v;Cyc_Absyn_eq_exp(_tmp8D0,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->tag=33,_tmp448->f1=t,_tmp448->f2=ef;_tmp448;}),0U),0U);});}case 5: _tmp446=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp441)->f1;_tmp444=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp441)->f2;{struct _fat_ptr s=_tmp446;int i=_tmp444;
return({struct Cyc_Absyn_Exp*_tmp8D1=v;Cyc_Absyn_eq_exp(_tmp8D1,Cyc_Absyn_float_exp(s,i,0U),0U);});}case 6: _tmp445=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp441)->f1;{unsigned i=_tmp445;
return({struct Cyc_Absyn_Exp*_tmp8D2=v;Cyc_Absyn_eq_exp(_tmp8D2,Cyc_Absyn_signed_int_exp((int)i,0U),0U);});}case 7: _tmp444=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp441)->f1;_tmp442=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp441)->f2;_tmp443=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp441)->f3;{int i=_tmp444;struct Cyc_Absyn_Datatypedecl*tud=_tmp442;struct Cyc_Absyn_Datatypefield*tuf=_tmp443;
# 2852
LOOP1: {
void*_tmp449=v->r;void*_stmttmp4D=_tmp449;void*_tmp44A=_stmttmp4D;void*_tmp44B;switch(*((int*)_tmp44A)){case 14: _tmp44B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp44A)->f2;{struct Cyc_Absyn_Exp*e=_tmp44B;
v=e;goto LOOP1;}case 20: _tmp44B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp44A)->f1;{struct Cyc_Absyn_Exp*e=_tmp44B;
v=e;goto _LL15;}default:
 goto _LL15;}_LL15:;}
# 2862
v=({void*_tmp8D3=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp8D3,v);});
return({struct Cyc_Absyn_Exp*_tmp8D4=Cyc_Absyn_deref_exp(v,0U);Cyc_Absyn_eq_exp(_tmp8D4,Cyc_Absyn_signed_int_exp(i,0U),0U);});}case 8: _tmp442=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp441)->f1;_tmp444=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp441)->f2;{struct _fat_ptr*f=_tmp442;int i=_tmp444;
# 2865
return({struct Cyc_Absyn_Exp*_tmp8D6=({struct Cyc_Absyn_Exp*_tmp8D5=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_tmp8D5,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_tmp8D6,
Cyc_Absyn_signed_int_exp(i,0U),0U);});}default: _tmp442=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp441)->f1;_tmp443=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp441)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp442;struct Cyc_Absyn_Datatypefield*tuf=_tmp443;
# 2869
LOOP2: {
void*_tmp44C=v->r;void*_stmttmp4E=_tmp44C;void*_tmp44D=_stmttmp4E;void*_tmp44E;switch(*((int*)_tmp44D)){case 14: _tmp44E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp44D)->f2;{struct Cyc_Absyn_Exp*e=_tmp44E;
v=e;goto LOOP2;}case 20: _tmp44E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp44D)->f1;{struct Cyc_Absyn_Exp*e=_tmp44E;
v=e;goto _LL1C;}default:
 goto _LL1C;}_LL1C:;}
# 2876
v=({void*_tmp8D8=({void*_tmp8D7=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmp8D7,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp8D8,v);});
return({struct Cyc_Absyn_Exp*_tmp8D9=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp8D9,Cyc_Absyn_var_exp(tuf->name,0U),0U);});}};}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2888
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple38{int f1;void*f2;};
# 2895
static struct _tuple38 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2897
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(1;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple34 _tmp44F=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp4F=_tmp44F;struct _tuple34 _tmp450=_stmttmp4F;void*_tmp451;_tmp451=_tmp450.f1;{void*ptest=_tmp451;
void*_tmp452=ptest;void*_tmp453;switch(*((int*)_tmp452)){case 3:
# 2903
 goto _LL7;case 4: _LL7:
 goto _LL9;case 6: _LL9:
 continue;case 8: _tmp453=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp452)->f1;{struct _fat_ptr*f=_tmp453;
# 2907
if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->tag=3,_tmp454->f1=f;_tmp454;});
continue;}case 7:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0: _tmp453=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp452)->f1;if((struct Cyc_Absyn_Exp*)_tmp453 != 0){struct Cyc_Absyn_Exp*e=_tmp453;
# 2912
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->tag=2,_tmp455->f1=e;_tmp455;});return({struct _tuple38 _tmp6D8;_tmp6D8.f1=0,_tmp6D8.f2=k;_tmp6D8;});}else{
goto _LL13;}case 1: _LL13:
 goto _LL15;case 2: _LL15:
 goto _LL17;case 5: _LL17:
 goto _LL19;default: _LL19:
 return({struct _tuple38 _tmp6D9;_tmp6D9.f1=0,_tmp6D9.f2=k;_tmp6D9;});};}}
# 2920
return({struct _tuple38 _tmp6DA;_tmp6DA.f1=c,_tmp6DA.f2=k;_tmp6DA;});}
# 2925
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp456=p;int _tmp457;unsigned _tmp458;void*_tmp45A;void*_tmp459;switch(*((int*)_tmp456)){case 3: _tmp459=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp456)->f1;_tmp45A=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp456)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp459;struct Cyc_Absyn_Enumfield*ef=_tmp45A;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->tag=32,_tmp45B->f1=ed,_tmp45B->f2=ef;_tmp45B;}),0U);goto _LL0;}case 4: _tmp459=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp456)->f1;_tmp45A=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp456)->f2;{void*t=_tmp459;struct Cyc_Absyn_Enumfield*ef=_tmp45A;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=33,_tmp45C->f1=t,_tmp45C->f2=ef;_tmp45C;}),0U);goto _LL0;}case 6: _tmp458=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp456)->f1;{unsigned i=_tmp458;
_tmp457=(int)i;goto _LL8;}case 7: _tmp457=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp456)->f1;_LL8: {int i=_tmp457;
_tmp457=i;goto _LLA;}case 8: _tmp457=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp456)->f2;_LLA: {int i=_tmp457;
e=Cyc_Absyn_signed_int_exp(i,0U);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp45E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DB;_tmp6DB.tag=0,({struct _fat_ptr _tmp8DA=({const char*_tmp45F="compile_pat_test_as_case!";_tag_fat(_tmp45F,sizeof(char),26U);});_tmp6DB.f1=_tmp8DA;});_tmp6DB;});void*_tmp45D[1];_tmp45D[0]=& _tmp45E;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp45D,sizeof(void*),1));});}_LL0:;}
# 2935
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->tag=17,_tmp460->f1=e;_tmp460;}),0U);}
# 2939
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple39{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;void*f3;int f4;};
# 2951
int Cyc_Toc_vd_in_use(struct Cyc_Absyn_Vardecl*other_vd,struct Cyc_List_List**newdecls){
{struct Cyc_List_List*_tmp461=*newdecls;struct Cyc_List_List*ds=_tmp461;for(0;ds != 0;ds=ds->tl){
# 2955
if(other_vd == (*((struct _tuple39*)ds->hd)).f2)return 1;}}
return 0;}
# 2959
static int Cyc_Toc_is_usuable_type(void*ctype,void*other){
void*_tmp462=Cyc_Absyn_compress(ctype);void*_stmttmp50=_tmp462;void*_tmp463=_stmttmp50;struct Cyc_Absyn_Tqual _tmp464;if(*((int*)_tmp463)== 3){_tmp464=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp463)->f1).elt_tq;{struct Cyc_Absyn_Tqual tq=_tmp464;
# 2962
if(Cyc_Absyn_equal_tqual(tq,Cyc_Toc_mt_tq)&& Cyc_Toc_is_void_star(other))
return 1;
goto _LL4;}}else{_LL4:
 return Cyc_Tcutil_typecmp(ctype,other)== 0;};}
# 2968
static int Cyc_Toc_can_use_voidstar(void*ctype){
void*_tmp465=Cyc_Absyn_compress(ctype);void*_stmttmp51=_tmp465;void*_tmp466=_stmttmp51;struct Cyc_Absyn_Tqual _tmp467;if(*((int*)_tmp466)== 3){_tmp467=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp466)->f1).elt_tq;{struct Cyc_Absyn_Tqual tq=_tmp467;
return Cyc_Absyn_equal_tqual(tq,Cyc_Toc_mt_tq);}}else{
return 0;};}struct _tuple40{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple41{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 2978
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**newdecls,struct Cyc_List_List**prevdecls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 2982
void*t=_check_null(p->topt);
void*_tmp468=p->r;void*_stmttmp52=_tmp468;void*_tmp469=_stmttmp52;union Cyc_Absyn_AggrInfo _tmp46A;void*_tmp46D;void*_tmp46C;void*_tmp46B;switch(*((int*)_tmp469)){case 2: _tmp46B=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp469)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp46B;
# 2985
struct Cyc_Absyn_Pat*_tmp46E=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);struct Cyc_Absyn_Pat*p2=_tmp46E;
p2->topt=p->topt;
_tmp46B=vd;_tmp46C=p2;goto _LL4;}case 1: _tmp46B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp469)->f1;_tmp46C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp469)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp46B;struct Cyc_Absyn_Pat*p2=_tmp46C;
# 2990
void*_tmp46F=Cyc_Toc_typ_to_c_array(vd->type);void*ctype=_tmp46F;
# 2992
{struct Cyc_List_List*_tmp470=*prevdecls;struct Cyc_List_List*pds=_tmp470;for(0;pds != 0;pds=pds->tl){
struct _tuple40 _tmp471=*((struct _tuple40*)pds->hd);struct _tuple40 _stmttmp53=_tmp471;struct _tuple40 _tmp472=_stmttmp53;void*_tmp473;_tmp473=_tmp472.f2;{struct Cyc_Absyn_Vardecl*other_vd=_tmp473;
# 2996
if(Cyc_Toc_is_usuable_type(ctype,other_vd->type)&& !
Cyc_Toc_vd_in_use(other_vd,newdecls)){
({struct Cyc_List_List*_tmp8DC=({struct Cyc_List_List*_tmp475=_region_malloc(rgn,sizeof(*_tmp475));({struct _tuple39*_tmp8DB=({struct _tuple39*_tmp474=_region_malloc(rgn,sizeof(*_tmp474));_tmp474->f1=vd,_tmp474->f2=other_vd,_tmp474->f3=ctype,_tmp474->f4=0;_tmp474;});_tmp475->hd=_tmp8DB;}),_tmp475->tl=*newdecls;_tmp475;});*newdecls=_tmp8DC;});{
# 3000
struct Cyc_Absyn_Stmt*_tmp476=({struct Cyc_Absyn_Exp*_tmp8DD=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->tag=4,_tmp477->f1=other_vd;_tmp477;}),0U);Cyc_Absyn_assign_stmt(_tmp8DD,
Cyc_Absyn_copy_exp(path),0U);});
# 3000
struct Cyc_Absyn_Stmt*s=_tmp476;
# 3002
return({struct Cyc_Absyn_Stmt*_tmp8DE=s;Cyc_Toc_seq_stmt_opt(_tmp8DE,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));});}}}}}{
# 3008
struct Cyc_Absyn_Vardecl*new_vd;
if(Cyc_Toc_can_use_voidstar(ctype)){
new_vd=({struct _tuple1*_tmp8DF=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp8DF,Cyc_Toc_void_star_type(),0);});
({struct Cyc_List_List*_tmp8E1=({struct Cyc_List_List*_tmp479=_region_malloc(rgn,sizeof(*_tmp479));({struct _tuple39*_tmp8E0=({struct _tuple39*_tmp478=_region_malloc(rgn,sizeof(*_tmp478));_tmp478->f1=vd,_tmp478->f2=new_vd,_tmp478->f3=ctype,_tmp478->f4=0;_tmp478;});_tmp479->hd=_tmp8E0;}),_tmp479->tl=*newdecls;_tmp479;});*newdecls=_tmp8E1;});}else{
# 3014
new_vd=({struct _tuple1*_tmp8E2=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp8E2,ctype,0);});
({struct Cyc_List_List*_tmp8E4=({struct Cyc_List_List*_tmp47B=_region_malloc(rgn,sizeof(*_tmp47B));({struct _tuple39*_tmp8E3=({struct _tuple39*_tmp47A=_region_malloc(rgn,sizeof(*_tmp47A));_tmp47A->f1=vd,_tmp47A->f2=new_vd,_tmp47A->f3=ctype,_tmp47A->f4=0;_tmp47A;});_tmp47B->hd=_tmp8E3;}),_tmp47B->tl=*newdecls;_tmp47B;});*newdecls=_tmp8E4;});}{
# 3018
struct Cyc_Absyn_Stmt*_tmp47C=({struct Cyc_Absyn_Exp*_tmp8E5=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->tag=4,_tmp47D->f1=new_vd;_tmp47D;}),0U);Cyc_Absyn_assign_stmt(_tmp8E5,
Cyc_Absyn_copy_exp(path),0U);});
# 3018
struct Cyc_Absyn_Stmt*s=_tmp47C;
# 3020
return({struct Cyc_Absyn_Stmt*_tmp8E6=s;Cyc_Toc_seq_stmt_opt(_tmp8E6,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));});}}}case 4: _tmp46B=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp469)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp46B;
# 3024
void*_tmp47E=Cyc_Toc_typ_to_c_array(vd->type);void*ctype=_tmp47E;
# 3026
{struct Cyc_List_List*_tmp47F=*prevdecls;struct Cyc_List_List*pds=_tmp47F;for(0;pds != 0;pds=pds->tl){
struct _tuple40 _tmp480=*((struct _tuple40*)pds->hd);struct _tuple40 _stmttmp54=_tmp480;struct _tuple40 _tmp481=_stmttmp54;void*_tmp482;_tmp482=_tmp481.f2;{struct Cyc_Absyn_Vardecl*other_vd=_tmp482;
# 3030
if(Cyc_Toc_is_usuable_type(ctype,other_vd->type)&& !
Cyc_Toc_vd_in_use(other_vd,newdecls)){
({struct Cyc_List_List*_tmp8E8=({struct Cyc_List_List*_tmp484=_region_malloc(rgn,sizeof(*_tmp484));({struct _tuple39*_tmp8E7=({struct _tuple39*_tmp483=_region_malloc(rgn,sizeof(*_tmp483));_tmp483->f1=vd,_tmp483->f2=other_vd,_tmp483->f3=ctype,_tmp483->f4=0;_tmp483;});_tmp484->hd=_tmp8E7;}),_tmp484->tl=*newdecls;_tmp484;});*newdecls=_tmp8E8;});
# 3034
return({struct Cyc_Absyn_Exp*_tmp8E9=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->tag=4,_tmp485->f1=other_vd;_tmp485;}),0U);Cyc_Absyn_assign_stmt(_tmp8E9,
Cyc_Absyn_copy_exp(path),0U);});}}}}{
# 3039
struct Cyc_Absyn_Vardecl*new_vd;
if(Cyc_Toc_can_use_voidstar(ctype)){
new_vd=({struct _tuple1*_tmp8EA=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp8EA,Cyc_Toc_void_star_type(),0);});
({struct Cyc_List_List*_tmp8EC=({struct Cyc_List_List*_tmp487=_region_malloc(rgn,sizeof(*_tmp487));({struct _tuple39*_tmp8EB=({struct _tuple39*_tmp486=_region_malloc(rgn,sizeof(*_tmp486));_tmp486->f1=vd,_tmp486->f2=new_vd,_tmp486->f3=ctype,_tmp486->f4=1;_tmp486;});_tmp487->hd=_tmp8EB;}),_tmp487->tl=*newdecls;_tmp487;});*newdecls=_tmp8EC;});}else{
# 3045
new_vd=({struct _tuple1*_tmp8ED=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp8ED,ctype,0);});
({struct Cyc_List_List*_tmp8EF=({struct Cyc_List_List*_tmp489=_region_malloc(rgn,sizeof(*_tmp489));({struct _tuple39*_tmp8EE=({struct _tuple39*_tmp488=_region_malloc(rgn,sizeof(*_tmp488));_tmp488->f1=vd,_tmp488->f2=new_vd,_tmp488->f3=ctype,_tmp488->f4=0;_tmp488;});_tmp489->hd=_tmp8EE;}),_tmp489->tl=*newdecls;_tmp489;});*newdecls=_tmp8EF;});}
# 3049
return({struct Cyc_Absyn_Exp*_tmp8F0=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A->tag=4,_tmp48A->f1=new_vd;_tmp48A;}),0U);Cyc_Absyn_assign_stmt(_tmp8F0,Cyc_Absyn_copy_exp(path),0U);});}}case 0:
 return 0;case 3: _tmp46B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp469)->f1;_tmp46C=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp469)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp46B;struct Cyc_Absyn_Pat*p2=_tmp46C;
# 3053
({void*_tmp8F1=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_tmp8F1;});{
# 3055
void*_tmp48B=Cyc_Toc_typ_to_c_array(vd->type);void*ctype=_tmp48B;
# 3057
{struct Cyc_List_List*_tmp48C=*prevdecls;struct Cyc_List_List*pds=_tmp48C;for(0;pds != 0;pds=pds->tl){
struct _tuple40 _tmp48D=*((struct _tuple40*)pds->hd);struct _tuple40 _stmttmp55=_tmp48D;struct _tuple40 _tmp48E=_stmttmp55;void*_tmp48F;_tmp48F=_tmp48E.f2;{struct Cyc_Absyn_Vardecl*other_vd=_tmp48F;
# 3061
if(Cyc_Toc_is_usuable_type(ctype,other_vd->type)&& !
Cyc_Toc_vd_in_use(other_vd,newdecls)){
({struct Cyc_List_List*_tmp8F3=({struct Cyc_List_List*_tmp491=_region_malloc(rgn,sizeof(*_tmp491));({struct _tuple39*_tmp8F2=({struct _tuple39*_tmp490=_region_malloc(rgn,sizeof(*_tmp490));_tmp490->f1=vd,_tmp490->f2=other_vd,_tmp490->f3=ctype,_tmp490->f4=0;_tmp490;});_tmp491->hd=_tmp8F2;}),_tmp491->tl=*newdecls;_tmp491;});*newdecls=_tmp8F3;});{
# 3066
struct Cyc_Absyn_Stmt*_tmp492=({struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->tag=4,_tmp493->f1=other_vd;_tmp493;}),0U);Cyc_Absyn_assign_stmt(_tmp8F6,({
void*_tmp8F5=({void*_tmp8F4=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp8F4,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp8F5,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
# 3066
struct Cyc_Absyn_Stmt*s=_tmp492;
# 3069
return({struct Cyc_Absyn_Stmt*_tmp8F7=s;Cyc_Toc_seq_stmt_opt(_tmp8F7,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));});}}}}}{
# 3074
struct Cyc_Absyn_Vardecl*new_vd;
if(Cyc_Toc_can_use_voidstar(ctype)){
new_vd=({struct _tuple1*_tmp8F8=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp8F8,Cyc_Toc_void_star_type(),0);});
({struct Cyc_List_List*_tmp8FA=({struct Cyc_List_List*_tmp495=_region_malloc(rgn,sizeof(*_tmp495));({struct _tuple39*_tmp8F9=({struct _tuple39*_tmp494=_region_malloc(rgn,sizeof(*_tmp494));_tmp494->f1=vd,_tmp494->f2=new_vd,_tmp494->f3=ctype,_tmp494->f4=1;_tmp494;});_tmp495->hd=_tmp8F9;}),_tmp495->tl=*newdecls;_tmp495;});*newdecls=_tmp8FA;});}else{
# 3080
new_vd=({struct _tuple1*_tmp8FB=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp8FB,ctype,0);});
({struct Cyc_List_List*_tmp8FD=({struct Cyc_List_List*_tmp497=_region_malloc(rgn,sizeof(*_tmp497));({struct _tuple39*_tmp8FC=({struct _tuple39*_tmp496=_region_malloc(rgn,sizeof(*_tmp496));_tmp496->f1=vd,_tmp496->f2=new_vd,_tmp496->f3=ctype,_tmp496->f4=0;_tmp496;});_tmp497->hd=_tmp8FC;}),_tmp497->tl=*newdecls;_tmp497;});*newdecls=_tmp8FD;});}{
# 3086
struct Cyc_Absyn_Stmt*_tmp498=({struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->tag=4,_tmp499->f1=new_vd;_tmp499;}),0U);Cyc_Absyn_assign_stmt(_tmp900,({
void*_tmp8FF=({void*_tmp8FE=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp8FE,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp8FF,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
# 3086
struct Cyc_Absyn_Stmt*s=_tmp498;
# 3089
return({struct Cyc_Absyn_Stmt*_tmp901=s;Cyc_Toc_seq_stmt_opt(_tmp901,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));});}}}}case 9:
# 3091
 goto _LLE;case 10: _LLE:
 goto _LL10;case 11: _LL10:
 goto _LL12;case 12: _LL12:
 goto _LL14;case 13: _LL14:
 goto _LL16;case 14: _LL16:
 return 0;case 6: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp469)->f1)->r)== 8){_tmp46B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp469)->f1)->r)->f1;_tmp46C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp469)->f1)->r)->f2;_tmp46D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp469)->f1)->r)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_tmp46B;struct Cyc_Absyn_Datatypefield*tuf=_tmp46C;struct Cyc_List_List*ps=_tmp46D;
# 3100
if(ps == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*_tmp49A=({void*_tmp902=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmp902,Cyc_Toc_mt_tq);});void*field_ptr_typ=_tmp49A;
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*_tmp49B=tuf->typs;struct Cyc_List_List*tuf_tqts=_tmp49B;
struct Cyc_Absyn_Stmt*s=0;
for(1;ps != 0;(ps=ps->tl,tuf_tqts=(_check_null(tuf_tqts))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp49C=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp49C;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp49D=(*((struct _tuple23*)(_check_null(tuf_tqts))->hd)).f2;void*tuf_typ=_tmp49D;
void*_tmp49E=_check_null(p2->topt);void*t2=_tmp49E;
void*_tmp49F=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp49F;
struct Cyc_Absyn_Exp*_tmp4A0=({struct Cyc_Absyn_Exp*_tmp903=path;Cyc_Absyn_aggrarrow_exp(_tmp903,Cyc_Absyn_fieldname(cnt),0U);});struct Cyc_Absyn_Exp*arrow_exp=_tmp4A0;
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_tmp904=s;Cyc_Toc_seq_stmt_opt(_tmp904,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,arrow_exp,p2));});}}
# 3118
return s;}}}}else{_tmp46B=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp469)->f1;{struct Cyc_Absyn_Pat*p2=_tmp46B;
# 3163
return({struct _RegionHandle*_tmp909=rgn;struct Cyc_Toc_Env**_tmp908=nv;struct Cyc_List_List**_tmp907=newdecls;struct Cyc_List_List**_tmp906=prevdecls;struct Cyc_Absyn_Exp*_tmp905=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmp909,_tmp908,_tmp907,_tmp906,_tmp905,p2);});}}case 8: _tmp46B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp469)->f3;{struct Cyc_List_List*ps=_tmp46B;
# 3120
_tmp46B=ps;goto _LL1C;}case 5: _tmp46B=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp469)->f1;_LL1C: {struct Cyc_List_List*ps=_tmp46B;
# 3122
struct Cyc_Absyn_Stmt*s=0;
{int cnt=1;for(0;ps != 0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4A1=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp4A1;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp4A2=_check_null(p2->topt);void*t2=_tmp4A2;
struct _fat_ptr*_tmp4A3=Cyc_Absyn_fieldname(cnt);struct _fat_ptr*f=_tmp4A3;
s=({struct Cyc_Absyn_Stmt*_tmp90F=s;Cyc_Toc_seq_stmt_opt(_tmp90F,({struct _RegionHandle*_tmp90E=rgn;struct Cyc_Toc_Env**_tmp90D=nv;struct Cyc_List_List**_tmp90C=newdecls;struct Cyc_List_List**_tmp90B=prevdecls;struct Cyc_Absyn_Exp*_tmp90A=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_tmp90E,_tmp90D,_tmp90C,_tmp90B,_tmp90A,p2);}));});}}}
# 3131
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp469)->f1 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DC;_tmp6DC.tag=0,({struct _fat_ptr _tmp910=({const char*_tmp4A6="unresolved aggregate pattern!";_tag_fat(_tmp4A6,sizeof(char),30U);});_tmp6DC.f1=_tmp910;});_tmp6DC;});void*_tmp4A4[1];_tmp4A4[0]=& _tmp4A5;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp4A4,sizeof(void*),1));});else{_tmp46A=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp469)->f1;_tmp46B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp469)->f3;{union Cyc_Absyn_AggrInfo info=_tmp46A;struct Cyc_List_List*dlps=_tmp46B;
# 3135
struct Cyc_Absyn_Aggrdecl*_tmp4A7=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp4A7;
struct Cyc_Absyn_Stmt*s=0;
for(1;dlps != 0;dlps=dlps->tl){
struct _tuple41*_tmp4A8=(struct _tuple41*)dlps->hd;struct _tuple41*tup=_tmp4A8;
struct Cyc_Absyn_Pat*_tmp4A9=(*tup).f2;struct Cyc_Absyn_Pat*p2=_tmp4A9;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*_tmp4AA=Cyc_Absyn_designatorlist_to_fieldname((*tup).f1);struct _fat_ptr*f=_tmp4AA;
void*_tmp4AB=_check_null(p2->topt);void*t2=_tmp4AB;
void*_tmp4AC=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp4AC;
struct Cyc_Absyn_Exp*_tmp4AD=Cyc_Toc_member_exp(path,f,0U);struct Cyc_Absyn_Exp*memexp=_tmp4AD;
# 3147
if((_check_null(ad->impl))->tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*_tmp4AE=(_check_null(Cyc_Absyn_lookup_field((_check_null(ad->impl))->fields,f)))->type;void*ftype=_tmp4AE;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(ftype)))
# 3155
memexp=Cyc_Absyn_deref_exp(({void*_tmp911=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp911,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3158
s=({struct Cyc_Absyn_Stmt*_tmp912=s;Cyc_Toc_seq_stmt_opt(_tmp912,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,memexp,p2));});}}}
# 3160
return s;}}case 15:
# 3165
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DD;_tmp6DD.tag=0,({struct _fat_ptr _tmp913=({const char*_tmp4B1="unknownid pat";_tag_fat(_tmp4B1,sizeof(char),14U);});_tmp6DD.f1=_tmp913;});_tmp6DD;});void*_tmp4AF[1];_tmp4AF[0]=& _tmp4B0;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp4AF,sizeof(void*),1));});case 16:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DE;_tmp6DE.tag=0,({struct _fat_ptr _tmp914=({const char*_tmp4B4="unknowncall pat";_tag_fat(_tmp4B4,sizeof(char),16U);});_tmp6DE.f1=_tmp914;});_tmp6DE;});void*_tmp4B2[1];_tmp4B2[0]=& _tmp4B3;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp4B2,sizeof(void*),1));});default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DF;_tmp6DF.tag=0,({struct _fat_ptr _tmp915=({const char*_tmp4B7="exp pat";_tag_fat(_tmp4B7,sizeof(char),8U);});_tmp6DF.f1=_tmp915;});_tmp6DF;});void*_tmp4B5[1];_tmp4B5[0]=& _tmp4B6;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp4B5,sizeof(void*),1));});};}
# 3174
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_tmp4B8=e->r;void*_stmttmp56=_tmp4B8;void*_tmp4B9=_stmttmp56;void*_tmp4BC;void*_tmp4BB;void*_tmp4BA;switch(*((int*)_tmp4B9)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1)){case 5: _tmp4BA=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp4BA;
_tmp4BA=vd;goto _LL4;}case 4: _tmp4BA=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp4BA;
# 3180
{struct _handler_cons _tmp4BD;_push_handler(& _tmp4BD);{int _tmp4BF=0;if(setjmp(_tmp4BD.handler))_tmp4BF=1;if(!_tmp4BF){
{void*_tmp4C0=Cyc_Toc_typ_to_c_array(vd->type);void*ctype=_tmp4C0;
struct Cyc_Absyn_Vardecl*_tmp4C1=({(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc;})(env,vd);struct Cyc_Absyn_Vardecl*tmpvd=_tmp4C1;
struct Cyc_Absyn_Exp*_tmp4C2=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->tag=1,({void*_tmp917=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4->tag=4,({struct Cyc_Absyn_Vardecl*_tmp916=({(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc;})(env,vd);_tmp4C4->f1=_tmp916;});_tmp4C4;});_tmp4C5->f1=_tmp917;});_tmp4C5;}),0U);struct Cyc_Absyn_Exp*e1=_tmp4C2;
e1->topt=tmpvd->type;
({void*_tmp918=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3->tag=14,_tmp4C3->f1=ctype,_tmp4C3->f2=e1,_tmp4C3->f3=0,_tmp4C3->f4=1U;_tmp4C3;});e->r=_tmp918;});}
# 3181
;_pop_handler();}else{void*_tmp4BE=(void*)Cyc_Core_get_exn_thrown();void*_tmp4C6=_tmp4BE;void*_tmp4C7;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp4C6)->tag == Cyc_Core_Not_found)
# 3186
goto _LL3F;else{_tmp4C7=_tmp4C6;{void*exn=_tmp4C7;(int)_rethrow(exn);}}_LL3F:;}}}
goto _LL0;}default: goto _LL3D;}case 6: _tmp4BA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_tmp4BB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_tmp4BC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B9)->f3;{struct Cyc_Absyn_Exp*e1=_tmp4BA;struct Cyc_Absyn_Exp*e2=_tmp4BB;struct Cyc_Absyn_Exp*e3=_tmp4BC;
# 3189
Cyc_Toc_subst_pattern_vars(env,e1);_tmp4BA=e2;_tmp4BB=e3;goto _LL8;}case 27: _tmp4BA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_tmp4BB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B9)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp4BA;struct Cyc_Absyn_Exp*e2=_tmp4BB;
_tmp4BA=e1;_tmp4BB=e2;goto _LLA;}case 7: _tmp4BA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_tmp4BB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp4BA;struct Cyc_Absyn_Exp*e2=_tmp4BB;
_tmp4BA=e1;_tmp4BB=e2;goto _LLC;}case 8: _tmp4BA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_tmp4BB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp4BA;struct Cyc_Absyn_Exp*e2=_tmp4BB;
_tmp4BA=e1;_tmp4BB=e2;goto _LLE;}case 23: _tmp4BA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_tmp4BB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp4BA;struct Cyc_Absyn_Exp*e2=_tmp4BB;
_tmp4BA=e1;_tmp4BB=e2;goto _LL10;}case 9: _tmp4BA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_tmp4BB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp4BA;struct Cyc_Absyn_Exp*e2=_tmp4BB;
# 3195
Cyc_Toc_subst_pattern_vars(env,e1);_tmp4BA=e2;goto _LL12;}case 41: _tmp4BA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL14;}case 38: _tmp4BA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL16;}case 11: _tmp4BA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL18;}case 12: _tmp4BA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL1A;}case 13: _tmp4BA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL1C;}case 14: _tmp4BA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL1E;}case 18: _tmp4BA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL20;}case 20: _tmp4BA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL22;}case 21: _tmp4BA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL24;}case 22: _tmp4BA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL26;}case 28: _tmp4BA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL26: {struct Cyc_Absyn_Exp*e=_tmp4BA;
_tmp4BA=e;goto _LL28;}case 15: _tmp4BA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp4BA;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 34: _tmp4BA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1).rgn;_tmp4BB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1).num_elts;{struct Cyc_Absyn_Exp*eopt=_tmp4BA;struct Cyc_Absyn_Exp*e=_tmp4BB;
_tmp4BA=eopt;_tmp4BB=e;goto _LL2C;}case 16: _tmp4BA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_tmp4BB=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_LL2C: {struct Cyc_Absyn_Exp*eopt=_tmp4BA;struct Cyc_Absyn_Exp*e=_tmp4BB;
# 3210
if(eopt != 0)Cyc_Toc_subst_pattern_vars(env,eopt);
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 3: _tmp4BA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;{struct Cyc_List_List*es=_tmp4BA;
_tmp4BA=es;goto _LL30;}case 31: _tmp4BA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL30: {struct Cyc_List_List*es=_tmp4BA;
_tmp4BA=es;goto _LL32;}case 24: _tmp4BA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL32: {struct Cyc_List_List*es=_tmp4BA;
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 36: _tmp4BA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;{struct Cyc_List_List*dles=_tmp4BA;
_tmp4BA=dles;goto _LL36;}case 29: _tmp4BA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B9)->f3;_LL36: {struct Cyc_List_List*dles=_tmp4BA;
_tmp4BA=dles;goto _LL38;}case 30: _tmp4BA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_LL38: {struct Cyc_List_List*dles=_tmp4BA;
_tmp4BA=dles;goto _LL3A;}case 26: _tmp4BA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4B9)->f1;_LL3A: {struct Cyc_List_List*dles=_tmp4BA;
_tmp4BA=dles;goto _LL3C;}case 25: _tmp4BA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4B9)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp4BA;
# 3220
for(1;dles != 0;dles=dles->tl){
Cyc_Toc_subst_pattern_vars(env,(*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}default: _LL3D:
 goto _LL0;}_LL0:;}struct _tuple42{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3232
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3240
void*_tmp4C8=dopt;void*_tmp4CB;void*_tmp4CA;void*_tmp4C9;if(_tmp4C8 == 0)
# 3242
return Cyc_Absyn_skip_stmt(0U);else{switch(*((int*)_tmp4C8)){case 0:
 return Cyc_Toc_throw_match_stmt();case 1: _tmp4C9=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp4C8)->f1;{struct Cyc_Tcpat_Rhs*rhs=_tmp4C9;
# 3246
for(1;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp4CC=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp57=_tmp4CC;struct _tuple37*_tmp4CD=_stmttmp57;void*_tmp4D1;void*_tmp4D0;void*_tmp4CF;void*_tmp4CE;_tmp4CE=(int*)& _tmp4CD->f1;_tmp4CF=_tmp4CD->f2;_tmp4D0=_tmp4CD->f3;_tmp4D1=_tmp4CD->f4;{int*already_emitted=(int*)_tmp4CE;struct _fat_ptr*init_lab=_tmp4CF;struct _fat_ptr*code_lab=_tmp4D0;struct Cyc_Absyn_Switch_clause*sc=_tmp4D1;
struct Cyc_Absyn_Stmt*_tmp4D2=sc->body;struct Cyc_Absyn_Stmt*body=_tmp4D2;
if(body == rhs->rhs){
# 3251
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3258
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp91B=rgn;struct Cyc_List_List**_tmp91A=decls;struct Cyc_Absyn_Exp*_tmp919=
Cyc_Absyn_var_exp(v,0U);
# 3258
Cyc_Toc_extract_pattern_vars(_tmp91B,& nv,& newdecls,_tmp91A,_tmp919,sc->pattern);});
# 3262
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*_tmp4D3=newdecls;struct Cyc_List_List*ds=_tmp4D3;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp4D4=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp58=_tmp4D4;struct _tuple39 _tmp4D5=_stmttmp58;int _tmp4D9;void*_tmp4D8;void*_tmp4D7;void*_tmp4D6;_tmp4D6=_tmp4D5.f1;_tmp4D7=_tmp4D5.f2;_tmp4D8=_tmp4D5.f3;_tmp4D9=_tmp4D5.f4;{struct Cyc_Absyn_Vardecl*oldv=_tmp4D6;struct Cyc_Absyn_Vardecl*newv=_tmp4D7;void*oldvt=_tmp4D8;int cast=_tmp4D9;
({struct Cyc_List_List*_tmp91D=({struct Cyc_List_List*_tmp4DB=_region_malloc(rgn,sizeof(*_tmp4DB));({struct _tuple40*_tmp91C=({struct _tuple40*_tmp4DA=_region_malloc(rgn,sizeof(*_tmp4DA));_tmp4DA->f1=oldv,_tmp4DA->f2=newv;_tmp4DA;});_tmp4DB->hd=_tmp91C;}),_tmp4DB->tl=*decls;_tmp4DB;});*decls=_tmp91D;});
if(cast)({struct Cyc_Absyn_Exp*_tmp91F=({void*_tmp91E=oldvt;Cyc_Toc_cast_it(_tmp91E,Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->tag=4,_tmp4DC->f1=newv;_tmp4DC;}),0U));});oldv->initializer=_tmp91F;});else{
({struct Cyc_Absyn_Exp*_tmp920=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->tag=4,_tmp4DD->f1=newv;_tmp4DD;}),0U);oldv->initializer=_tmp920;});}
(_check_null(oldv->initializer))->topt=newv->type;
# 3270
oldv->type=oldvt;
res=({struct Cyc_Absyn_Decl*_tmp922=({struct Cyc_Absyn_Decl*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));({void*_tmp921=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE->tag=0,_tmp4DE->f1=oldv;_tmp4DE;});_tmp4DF->r=_tmp921;}),_tmp4DF->loc=0U;_tmp4DF;});Cyc_Absyn_decl_stmt(_tmp922,res,0U);});}}}
# 3273
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_tmp924=({struct Cyc_List_List*_tmp4E1=_region_malloc(rgn,sizeof(*_tmp4E1));({struct _tuple42*_tmp923=({struct _tuple42*_tmp4E0=_region_malloc(rgn,sizeof(*_tmp4E0));_tmp4E0->f1=nv,_tmp4E0->f2=code_lab,_tmp4E0->f3=body;_tmp4E0;});_tmp4E1->hd=_tmp923;}),_tmp4E1->tl=*bodies;_tmp4E1;});*bodies=_tmp924;});
return res;}}}}
# 3281
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E0;_tmp6E0.tag=0,({struct _fat_ptr _tmp925=({const char*_tmp4E4="couldn't find rhs!";_tag_fat(_tmp4E4,sizeof(char),19U);});_tmp6E0.f1=_tmp925;});_tmp6E0;});void*_tmp4E2[1];_tmp4E2[0]=& _tmp4E3;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp4E2,sizeof(void*),1));});}default: _tmp4C9=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp4C8)->f1;_tmp4CA=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp4C8)->f2;_tmp4CB=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp4C8)->f3;{struct Cyc_List_List*symbolic_path=_tmp4C9;struct Cyc_List_List*switches=_tmp4CA;void*other_decision=_tmp4CB;
# 3284
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3286
struct Cyc_Absyn_Exp*_tmp4E5=({struct Cyc_List_List*_tmp926=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(symbolic_path);Cyc_Toc_compile_path(_tmp926,Cyc_Absyn_var_exp(v,0U));});struct Cyc_Absyn_Exp*p=_tmp4E5;
struct Cyc_List_List*_tmp4E6=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(switches);struct Cyc_List_List*ss=_tmp4E6;
# 3289
struct _tuple38 _tmp4E7=Cyc_Toc_admits_switch(ss);struct _tuple38 _stmttmp59=_tmp4E7;struct _tuple38 _tmp4E8=_stmttmp59;void*_tmp4EA;int _tmp4E9;_tmp4E9=_tmp4E8.f1;_tmp4EA=_tmp4E8.f2;{int allows_switch=_tmp4E9;void*test_kind=_tmp4EA;
if(allows_switch > 1){
# 3293
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));
({struct Cyc_Absyn_Switch_clause*_tmp928=({struct Cyc_Absyn_Switch_clause*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));({struct Cyc_Absyn_Pat*_tmp927=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp4F9->pattern=_tmp927;}),_tmp4F9->pat_vars=0,_tmp4F9->where_clause=0,_tmp4F9->body=res,_tmp4F9->loc=0U;_tmp4F9;});_tmp4FA->hd=_tmp928;}),_tmp4FA->tl=0;_tmp4FA;});
# 3296
for(1;ss != 0;ss=ss->tl){
struct _tuple34 _tmp4EB=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp5A=_tmp4EB;struct _tuple34 _tmp4EC=_stmttmp5A;void*_tmp4EE;void*_tmp4ED;_tmp4ED=_tmp4EC.f1;_tmp4EE=_tmp4EC.f2;{void*pat_test=_tmp4ED;void*dec_tree=_tmp4EE;
# 3299
struct Cyc_Absyn_Pat*_tmp4EF=Cyc_Toc_compile_pat_test_as_case(pat_test);struct Cyc_Absyn_Pat*case_exp=_tmp4EF;
# 3301
struct Cyc_Absyn_Stmt*_tmp4F0=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp4F0;
# 3303
new_lscs=({struct Cyc_List_List*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));({struct Cyc_Absyn_Switch_clause*_tmp929=({struct Cyc_Absyn_Switch_clause*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1->pattern=case_exp,_tmp4F1->pat_vars=0,_tmp4F1->where_clause=0,_tmp4F1->body=s,_tmp4F1->loc=0U;_tmp4F1;});_tmp4F2->hd=_tmp929;}),_tmp4F2->tl=new_lscs;_tmp4F2;});}}
# 3305
{void*_tmp4F3=test_kind;void*_tmp4F4;switch(*((int*)_tmp4F3)){case 1:
# 3307
 LOOP1: {
void*_tmp4F5=p->r;void*_stmttmp5B=_tmp4F5;void*_tmp4F6=_stmttmp5B;void*_tmp4F7;switch(*((int*)_tmp4F6)){case 14: _tmp4F7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4F6)->f2;{struct Cyc_Absyn_Exp*e=_tmp4F7;
p=e;goto LOOP1;}case 20: _tmp4F7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4F6)->f1;{struct Cyc_Absyn_Exp*e=_tmp4F7;
p=e;goto _LL1E;}default:
 goto _LL1E;}_LL1E:;}
# 3313
p=Cyc_Absyn_deref_exp(({void*_tmp92A=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp92A,p);}),0U);goto _LL15;case 3: _tmp4F4=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp4F3)->f1;{struct _fat_ptr*f=_tmp4F4;
# 3315
p=({struct Cyc_Absyn_Exp*_tmp92B=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_tmp92B,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2:
 goto _LL1D;default: _LL1D:
# 3322
 p=Cyc_Toc_cast_it(Cyc_Absyn_sint_type,p);
goto _LL15;}_LL15:;}
# 3325
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->tag=10,_tmp4F8->f1=p,_tmp4F8->f2=new_lscs,_tmp4F8->f3=0;_tmp4F8;}),0U);}else{
# 3329
void*_tmp4FB=test_kind;void*_tmp4FC;if(*((int*)_tmp4FB)== 2){_tmp4FC=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp4FB)->f1;{struct Cyc_Absyn_Exp*e=_tmp4FC;
# 3331
struct Cyc_List_List*_tmp4FD=ss;void*_tmp4FF;void*_tmp4FE;if(_tmp4FD != 0){if(*((int*)((struct _tuple34*)((struct Cyc_List_List*)_tmp4FD)->hd)->f2)== 1){if(((struct Cyc_List_List*)_tmp4FD)->tl == 0){_tmp4FE=((struct _tuple34*)_tmp4FD->hd)->f1;_tmp4FF=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)_tmp4FD->hd)->f2)->f1;{void*pat_test=_tmp4FE;struct Cyc_Tcpat_Rhs*rhs=_tmp4FF;
# 3337
for(1;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp500=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp5C=_tmp500;struct _tuple37*_tmp501=_stmttmp5C;void*_tmp505;void*_tmp504;void*_tmp503;void*_tmp502;_tmp502=(int*)& _tmp501->f1;_tmp503=_tmp501->f2;_tmp504=_tmp501->f3;_tmp505=_tmp501->f4;{int*already_emitted=(int*)_tmp502;struct _fat_ptr*init_lab=_tmp503;struct _fat_ptr*code_lab=_tmp504;struct Cyc_Absyn_Switch_clause*sc=_tmp505;
struct Cyc_Absyn_Stmt*_tmp506=sc->body;struct Cyc_Absyn_Stmt*body=_tmp506;
if(body == rhs->rhs){
# 3342
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3347
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp92E=rgn;struct Cyc_List_List**_tmp92D=decls;struct Cyc_Absyn_Exp*_tmp92C=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp92E,& nv,& newdecls,_tmp92D,_tmp92C,sc->pattern);});
# 3351
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*_tmp507=newdecls;struct Cyc_List_List*ds=_tmp507;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp508=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp5D=_tmp508;struct _tuple39 _tmp509=_stmttmp5D;int _tmp50D;void*_tmp50C;void*_tmp50B;void*_tmp50A;_tmp50A=_tmp509.f1;_tmp50B=_tmp509.f2;_tmp50C=_tmp509.f3;_tmp50D=_tmp509.f4;{struct Cyc_Absyn_Vardecl*oldv=_tmp50A;struct Cyc_Absyn_Vardecl*newv=_tmp50B;void*oldvt=_tmp50C;int cast=_tmp50D;
({struct Cyc_List_List*_tmp930=({struct Cyc_List_List*_tmp50F=_region_malloc(rgn,sizeof(*_tmp50F));({struct _tuple40*_tmp92F=({struct _tuple40*_tmp50E=_region_malloc(rgn,sizeof(*_tmp50E));_tmp50E->f1=oldv,_tmp50E->f2=newv;_tmp50E;});_tmp50F->hd=_tmp92F;}),_tmp50F->tl=*decls;_tmp50F;});*decls=_tmp930;});
if(cast)({struct Cyc_Absyn_Exp*_tmp932=({void*_tmp931=oldvt;Cyc_Toc_cast_it(_tmp931,Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->tag=4,_tmp510->f1=newv;_tmp510;}),0U));});oldv->initializer=_tmp932;});else{
({struct Cyc_Absyn_Exp*_tmp933=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->tag=4,_tmp511->f1=newv;_tmp511;}),0U);oldv->initializer=_tmp933;});}
(_check_null(oldv->initializer))->topt=newv->type;
# 3359
oldv->type=oldvt;
r=({struct Cyc_Absyn_Decl*_tmp935=({struct Cyc_Absyn_Decl*_tmp513=_cycalloc(sizeof(*_tmp513));({void*_tmp934=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->tag=0,_tmp512->f1=oldv;_tmp512;});_tmp513->r=_tmp934;}),_tmp513->loc=0U;_tmp513;});Cyc_Absyn_decl_stmt(_tmp935,r,0U);});}}}
# 3362
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3366
Cyc_Toc_subst_pattern_vars(*decls,e);
Cyc_Toc_exp_to_c(nv,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_tmp937=({struct Cyc_List_List*_tmp515=_region_malloc(rgn,sizeof(*_tmp515));({struct _tuple42*_tmp936=({struct _tuple42*_tmp514=_region_malloc(rgn,sizeof(*_tmp514));_tmp514->f1=nv,_tmp514->f2=code_lab,_tmp514->f3=body;_tmp514;});_tmp515->hd=_tmp936;}),_tmp515->tl=*bodies;_tmp515;});*bodies=_tmp937;});
return r;}}}}
# 3376
({struct Cyc_Warn_String_Warn_Warg_struct _tmp517=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E1;_tmp6E1.tag=0,({struct _fat_ptr _tmp938=({const char*_tmp518="couldn't find rhs!";_tag_fat(_tmp518,sizeof(char),19U);});_tmp6E1.f1=_tmp938;});_tmp6E1;});void*_tmp516[1];_tmp516[0]=& _tmp517;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp516,sizeof(void*),1));});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp51A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E2;_tmp6E2.tag=0,({struct _fat_ptr _tmp939=({const char*_tmp51B="bad where clause in match compiler";_tag_fat(_tmp51B,sizeof(char),35U);});_tmp6E2.f1=_tmp939;});_tmp6E2;});void*_tmp519[1];_tmp519[0]=& _tmp51A;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp519,sizeof(void*),1));});};}}else{
# 3381
for(1;ss != 0;ss=ss->tl){
struct _tuple34 _tmp51C=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp5E=_tmp51C;struct _tuple34 _tmp51D=_stmttmp5E;void*_tmp51F;void*_tmp51E;_tmp51E=_tmp51D.f1;_tmp51F=_tmp51D.f2;{void*pat_test=_tmp51E;void*dec_tree=_tmp51F;
struct Cyc_Absyn_Exp*_tmp520=Cyc_Toc_compile_pat_test(p,pat_test);struct Cyc_Absyn_Exp*test_exp=_tmp520;
struct Cyc_Absyn_Stmt*_tmp521=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp521;
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}};}
# 3389
return res;}}}};}
# 3399
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3401
for(1;bodies != 0;bodies=bodies->tl){
struct _tuple42*_tmp522=(struct _tuple42*)bodies->hd;struct _tuple42*_stmttmp5F=_tmp522;struct _tuple42*_tmp523=_stmttmp5F;void*_tmp525;void*_tmp524;_tmp524=(struct Cyc_Toc_Env**)& _tmp523->f1;_tmp525=_tmp523->f3;{struct Cyc_Toc_Env**nv=(struct Cyc_Toc_Env**)_tmp524;struct Cyc_Absyn_Stmt*s2=_tmp525;
if(s2 == s)return nv;}}
# 3407
return 0;}
# 3411
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3414
void*_tmp526=_check_null(e->topt);void*t=_tmp526;
Cyc_Toc_exp_to_c(nv,e);{
# 3417
struct _tuple30 _tmp527=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp60=_tmp527;struct _tuple30 _tmp528=_stmttmp60;void*_tmp52A;void*_tmp529;_tmp529=_tmp528.f1;_tmp52A=_tmp528.f2;{struct _tuple1*v=_tmp529;struct Cyc_Absyn_Exp*path=_tmp52A;
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp52B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp52B;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp52C=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp52C;
# 3423
struct Cyc_List_List*lscs=
({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c;})(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3428
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3436
{struct Cyc_List_List*_tmp52D=lscs;struct Cyc_List_List*lscs2=_tmp52D;for(0;lscs2 != 0;lscs2=lscs2->tl){
struct _tuple37*_tmp52E=(struct _tuple37*)lscs2->hd;struct _tuple37*_stmttmp61=_tmp52E;struct _tuple37*_tmp52F=_stmttmp61;void*_tmp531;void*_tmp530;_tmp530=_tmp52F->f3;_tmp531=_tmp52F->f4;{struct _fat_ptr*body_lab=_tmp530;struct Cyc_Absyn_Switch_clause*body_sc=_tmp531;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3440
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3443
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp532=*envp;struct Cyc_Toc_Env*env=_tmp532;
# 3446
if(lscs2->tl != 0){
struct _tuple37*_tmp533=(struct _tuple37*)(_check_null(lscs2->tl))->hd;struct _tuple37*_stmttmp62=_tmp533;struct _tuple37*_tmp534=_stmttmp62;void*_tmp536;void*_tmp535;_tmp535=_tmp534->f3;_tmp536=_tmp534->f4;{struct _fat_ptr*fallthru_lab=_tmp535;struct Cyc_Absyn_Switch_clause*next_sc=_tmp536;
# 3450
struct Cyc_Toc_Env**_tmp537=Cyc_Toc_find_case_env(mybodies,next_sc->body);struct Cyc_Toc_Env**next_case_env=_tmp537;
# 3455
if(next_case_env == 0)
({struct Cyc_Toc_Env*_tmp93A=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmp93A,s);});else{
# 3459
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars != 0){
vs=({struct Cyc_List_List*(*_tmp93B)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls;});_tmp93B((({(struct _tuple0(*)(struct Cyc_List_List*))Cyc_List_split;})((struct Cyc_List_List*)(_check_null(next_sc->pat_vars))->v)).f1);});
vs=({struct Cyc_List_List*(*_tmp93E)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c;});struct Cyc_Absyn_Vardecl*(*_tmp93D)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*)=({(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc;});struct Cyc_List_List*_tmp93C=mydecls;_tmp93E(_tmp93D,_tmp93C,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(vs));});}
# 3464
({struct Cyc_Toc_Env*_tmp93F=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_tmp93F,s);});}}}else{
# 3468
({struct Cyc_Toc_Env*_tmp940=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmp940,s);});}}}}}{
# 3472
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmp942=test_tree;Cyc_Absyn_seq_stmt(_tmp942,({struct _fat_ptr*_tmp941=end_l;Cyc_Absyn_label_stmt(_tmp941,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3474
struct Cyc_List_List*added=0;
for(1;mydecls != 0;mydecls=(_check_null(mydecls))->tl){
struct _tuple40 _tmp538=*((struct _tuple40*)(_check_null(mydecls))->hd);struct _tuple40 _stmttmp63=_tmp538;struct _tuple40 _tmp539=_stmttmp63;void*_tmp53A;_tmp53A=_tmp539.f2;{struct Cyc_Absyn_Vardecl*vd=_tmp53A;
{struct Cyc_List_List*_tmp53B=added;struct Cyc_List_List*itr=_tmp53B;for(0;itr != 0;itr=itr->tl){
if((struct Cyc_Absyn_Vardecl*)itr->hd == vd)goto bottom;}}
added=({struct Cyc_List_List*_tmp53C=_region_malloc(rgn,sizeof(*_tmp53C));_tmp53C->hd=vd,_tmp53C->tl=added;_tmp53C;});
res=({struct Cyc_Absyn_Decl*_tmp943=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->tag=0,_tmp53D->f1=vd;_tmp53D;}),0U);Cyc_Absyn_decl_stmt(_tmp943,res,0U);});
bottom:
 continue;}}
# 3485
({void*_tmp947=({struct _tuple1*_tmp946=v;void*_tmp945=Cyc_Toc_typ_to_c(_check_null(e->topt));struct Cyc_Absyn_Exp*_tmp944=e;Cyc_Absyn_declare_stmt(_tmp946,_tmp945,_tmp944,res,0U);})->r;whole_s->r=_tmp947;});
_npop_handler(0);return;}}
# 3420
;_pop_region();}}}
# 3493
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp53E[1];({struct Cyc_Absyn_Exp*_tmp948=Cyc_Absyn_signed_int_exp(n - 1,0U);_tmp53E[0]=_tmp948;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__npop_handler_e,_tag_fat(_tmp53E,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);}
# 3496
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmp94A=({struct Cyc_Absyn_Stmt*_tmp949=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmp949,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmp94A;});}
# 3501
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3503
while(1){
void*_tmp53F=s->r;void*_stmttmp64=_tmp53F;void*_tmp540=_stmttmp64;void*_tmp544;void*_tmp543;void*_tmp542;void*_tmp541;switch(*((int*)_tmp540)){case 0:
 return;case 1: _tmp541=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;{struct Cyc_Absyn_Exp*e=_tmp541;
Cyc_Toc_exp_to_c(nv,e);return;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp540)->f1 == 0){
({int _tmp94B=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp94B,s);});return;}else{_tmp541=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;{struct Cyc_Absyn_Exp*e=_tmp541;
# 3510
void*t=Cyc_Toc_typ_to_c(_check_null((_check_null(e))->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*_tmp545=Cyc_Toc_temp_var();struct _tuple1*x=_tmp545;
struct Cyc_Absyn_Stmt*_tmp546=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);struct Cyc_Absyn_Stmt*retn_stmt=_tmp546;
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_tmp94C=(Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U))->r;s->r=_tmp94C;});}
# 3519
return;}}}case 2: _tmp541=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;_tmp542=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp540)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp541;struct Cyc_Absyn_Stmt*s2=_tmp542;
# 3521
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4: _tmp541=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;_tmp542=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp540)->f2;_tmp543=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp540)->f3;{struct Cyc_Absyn_Exp*e=_tmp541;struct Cyc_Absyn_Stmt*s1=_tmp542;struct Cyc_Absyn_Stmt*s2=_tmp543;
# 3525
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9: _tmp541=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;_tmp542=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp540)->f2).f1;_tmp543=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp540)->f3).f1;_tmp544=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp540)->f4;{struct Cyc_Absyn_Exp*e1=_tmp541;struct Cyc_Absyn_Exp*e2=_tmp542;struct Cyc_Absyn_Exp*e3=_tmp543;struct Cyc_Absyn_Stmt*s2=_tmp544;
# 3530
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_tmp541=s2;_tmp542=e3;goto _LL10;}case 14: _tmp541=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;_tmp542=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp540)->f2).f1;_LL10: {struct Cyc_Absyn_Stmt*s2=_tmp541;struct Cyc_Absyn_Exp*e=_tmp542;
_tmp541=e;_tmp542=s2;goto _LL12;}case 5: _tmp541=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp540)->f1).f1;_tmp542=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp540)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_tmp541;struct Cyc_Absyn_Stmt*s2=_tmp542;
# 3535
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _tmp547=_new_region("temp");struct _RegionHandle*temp=& _tmp547;_push_region(temp);({struct Cyc_Toc_Env*_tmp94D=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp94D,s2);});
_npop_handler(0);return;
# 3536
;_pop_region();}}case 6:  {
# 3539
struct Cyc_Toc_Env*_tmp548=nv;void*_tmp549;_tmp549=_tmp548->break_lab;{struct _fat_ptr**b=_tmp549;
if(b != 0)
({void*_tmp94E=Cyc_Toc_goto_stmt_r(*b);s->r=_tmp94E;});
# 3543
({int _tmp94F=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp94F,s);});
return;}}case 7:  {
# 3546
struct Cyc_Toc_Env*_tmp54A=nv;void*_tmp54B;_tmp54B=_tmp54A->continue_lab;{struct _fat_ptr**c=_tmp54B;
if(c != 0)
({void*_tmp950=Cyc_Toc_goto_stmt_r(*c);s->r=_tmp950;});
goto _LL18;}}case 8: _LL18:
# 3551
({int _tmp951=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp951,s);});
return;case 13: _tmp541=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;_tmp542=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp540)->f2;{struct _fat_ptr*lab=_tmp541;struct Cyc_Absyn_Stmt*s1=_tmp542;
s=s1;continue;}case 10: _tmp541=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;_tmp542=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp540)->f2;_tmp543=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp540)->f3;{struct Cyc_Absyn_Exp*e=_tmp541;struct Cyc_List_List*scs=_tmp542;void*dec_tree_opt=_tmp543;
# 3555
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11: _tmp541=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;_tmp542=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp540)->f2;{struct Cyc_List_List*es=_tmp541;struct Cyc_Absyn_Switch_clause**dest_clause=_tmp542;
# 3558
struct Cyc_Toc_Env*_tmp54C=nv;void*_tmp54D;_tmp54D=_tmp54C->fallthru_info;{struct Cyc_Toc_FallthruInfo*fi=_tmp54D;
if(fi == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp54F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E3;_tmp6E3.tag=0,({struct _fat_ptr _tmp952=({const char*_tmp550="fallthru in unexpected place";_tag_fat(_tmp550,sizeof(char),29U);});_tmp6E3.f1=_tmp952;});_tmp6E3;});void*_tmp54E[1];_tmp54E[0]=& _tmp54F;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp54E,sizeof(void*),1));});{
struct Cyc_Toc_FallthruInfo _tmp551=*fi;struct Cyc_Toc_FallthruInfo _stmttmp65=_tmp551;struct Cyc_Toc_FallthruInfo _tmp552=_stmttmp65;void*_tmp554;void*_tmp553;_tmp553=_tmp552.label;_tmp554=_tmp552.binders;{struct _fat_ptr*l=_tmp553;struct Cyc_List_List*vs=_tmp554;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3564
({int _tmp953=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp953,s2);});{
struct Cyc_List_List*_tmp555=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(vs);struct Cyc_List_List*vs2=_tmp555;
struct Cyc_List_List*_tmp556=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(es);struct Cyc_List_List*es2=_tmp556;
for(1;vs2 != 0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)(_check_null(es2))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmp955=({struct Cyc_Absyn_Exp*_tmp954=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->tag=5,_tmp557->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_tmp557;}),0U);Cyc_Absyn_assign_stmt(_tmp954,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_tmp955,s2,0U);});}
# 3572
s->r=s2->r;
return;}}}}}case 12: _tmp541=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;_tmp542=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp540)->f2;{struct Cyc_Absyn_Decl*d=_tmp541;struct Cyc_Absyn_Stmt*body=_tmp542;
# 3578
{void*_tmp558=d->r;void*_stmttmp66=_tmp558;void*_tmp559=_stmttmp66;void*_tmp55C;void*_tmp55B;void*_tmp55A;switch(*((int*)_tmp559)){case 0: _tmp55A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp559)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp55A;
Cyc_Toc_local_decl_to_c(nv,vd,body);goto _LL2F;}case 2: _tmp55A=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp559)->f1;_tmp55B=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp559)->f3;_tmp55C=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp559)->f4;{struct Cyc_Absyn_Pat*p=_tmp55A;struct Cyc_Absyn_Exp*e=_tmp55B;void*dec_tree=_tmp55C;
# 3585
{void*_tmp55D=p->r;void*_stmttmp67=_tmp55D;void*_tmp55E=_stmttmp67;void*_tmp55F;if(*((int*)_tmp55E)== 1){if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp55E)->f2)->r)== 0){_tmp55F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp55E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp55F;
# 3587
struct Cyc_Absyn_Vardecl*_tmp560=({struct _tuple1*_tmp957=Cyc_Toc_temp_var();void*_tmp956=vd->type;Cyc_Absyn_new_vardecl(0U,_tmp957,_tmp956,e);});struct Cyc_Absyn_Vardecl*new_vd=_tmp560;
({struct Cyc_Absyn_Exp*_tmp958=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->tag=4,_tmp561->f1=new_vd;_tmp561;}),0U);vd->initializer=_tmp958;});
(_check_null(vd->initializer))->topt=new_vd->type;
({void*_tmp95F=({struct Cyc_Absyn_Decl*_tmp95E=({void*_tmp959=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->tag=0,_tmp562->f1=new_vd;_tmp562;});Cyc_Absyn_new_decl(_tmp959,s->loc);});struct Cyc_Absyn_Stmt*_tmp95D=({
struct Cyc_Absyn_Decl*_tmp95C=({void*_tmp95A=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=0,_tmp563->f1=vd;_tmp563;});Cyc_Absyn_new_decl(_tmp95A,s->loc);});struct Cyc_Absyn_Stmt*_tmp95B=body;Cyc_Absyn_decl_stmt(_tmp95C,_tmp95B,s->loc);});
# 3590
Cyc_Absyn_decl_stmt(_tmp95E,_tmp95D,s->loc);})->r;s->r=_tmp95F;});
# 3593
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F:
# 3599
({void*_tmp960=(Cyc_Toc_letdecl_to_c(nv,p,dec_tree,_check_null(e->topt),e,body))->r;s->r=_tmp960;});
goto _LL3C;}_LL3C:;}
# 3602
goto _LL2F;}case 3: _tmp55A=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp559)->f1;{struct Cyc_List_List*vds=_tmp55A;
# 3606
struct Cyc_List_List*_tmp564=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(vds);struct Cyc_List_List*rvds=_tmp564;
if(rvds == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp566=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E4;_tmp6E4.tag=0,({struct _fat_ptr _tmp961=({const char*_tmp567="empty Letv_d";_tag_fat(_tmp567,sizeof(char),13U);});_tmp6E4.f1=_tmp961;});_tmp6E4;});void*_tmp565[1];_tmp565[0]=& _tmp566;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp565,sizeof(void*),1));});
({void*_tmp962=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->tag=0,_tmp568->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp568;});d->r=_tmp962;});
rvds=rvds->tl;
for(1;rvds != 0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*_tmp569=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->tag=0,_tmp56A->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp56A;}),0U);struct Cyc_Absyn_Decl*d2=_tmp569;
({void*_tmp964=({struct Cyc_Absyn_Decl*_tmp963=d2;Cyc_Absyn_decl_stmt(_tmp963,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmp964;});}
# 3615
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1: _tmp55A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp559)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp55A;
# 3618
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,body);
goto _LL2F;}case 4: _tmp55A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp559)->f1;_tmp55B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp559)->f2;_tmp55C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp559)->f3;{struct Cyc_Absyn_Tvar*tv=_tmp55A;struct Cyc_Absyn_Vardecl*vd=_tmp55B;struct Cyc_Absyn_Exp*open_exp_opt=_tmp55C;
# 3623
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _tmp56B=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp68=_tmp56B;struct _tuple30 _tmp56C=_stmttmp68;void*_tmp56E;void*_tmp56D;_tmp56D=_tmp56C.f1;_tmp56E=_tmp56C.f2;{struct _tuple1*rh_var=_tmp56D;struct Cyc_Absyn_Exp*rh_exp=_tmp56E;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3629
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Flags_no_regions)
({void*_tmp968=({struct _tuple1*_tmp967=x_var;void*_tmp966=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmp965=
Cyc_Absyn_signed_int_exp(0,0U);
# 3631
Cyc_Absyn_declare_stmt(_tmp967,_tmp966,_tmp965,body,0U);})->r;s->r=_tmp968;});else{
# 3633
if((unsigned)open_exp_opt){
# 3635
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmp96A=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_tmp96A,({struct _fat_ptr*_tmp570=_cycalloc(sizeof(*_tmp570));({struct _fat_ptr _tmp969=({const char*_tmp56F="h";_tag_fat(_tmp56F,sizeof(char),2U);});*_tmp570=_tmp969;});_tmp570;}),0U);}),0U);
({void*_tmp96B=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U))->r;s->r=_tmp96B;});}}else{
# 3644
({void*_tmp975=({struct _tuple1*_tmp974=rh_var;void*_tmp973=rh_struct_typ;struct Cyc_Absyn_Exp*_tmp972=({struct Cyc_Absyn_Exp*_tmp571[1];({struct Cyc_Absyn_Exp*_tmp96C=
Cyc_Absyn_string_exp(*(*x_var).f2,0U);_tmp571[0]=_tmp96C;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__new_region_e,_tag_fat(_tmp571,sizeof(struct Cyc_Absyn_Exp*),1));});
# 3644
Cyc_Absyn_declare_stmt(_tmp974,_tmp973,_tmp972,({
# 3646
struct _tuple1*_tmp971=x_var;void*_tmp970=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmp96F=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmp971,_tmp970,_tmp96F,({
struct Cyc_Absyn_Stmt*_tmp96E=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp572[1];_tmp572[0]=x_exp;Cyc_Toc_fncall_exp_dl(Cyc_Toc__push_region_e,_tag_fat(_tmp572,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);Cyc_Absyn_seq_stmt(_tmp96E,({
struct Cyc_Absyn_Stmt*_tmp96D=body;Cyc_Absyn_seq_stmt(_tmp96D,
Cyc_Absyn_exp_stmt(({void*_tmp573=0U;Cyc_Toc_fncall_exp_dl(Cyc_Toc__pop_region_e,_tag_fat(_tmp573,sizeof(struct Cyc_Absyn_Exp*),0));}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3644
s->r=_tmp975;});}}
# 3651
return;}}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp575=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E5;_tmp6E5.tag=0,({struct _fat_ptr _tmp976=({const char*_tmp576="bad nested declaration within function";_tag_fat(_tmp576,sizeof(char),39U);});_tmp6E5.f1=_tmp976;});_tmp6E5;});void*_tmp574[1];_tmp574[0]=& _tmp575;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp574,sizeof(void*),1));});}_LL2F:;}
# 3654
return;}default: _tmp541=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp540)->f1;_tmp542=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp540)->f2;_tmp543=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp540)->f3;{struct Cyc_Absyn_Stmt*body=_tmp541;struct Cyc_List_List*scs=_tmp542;void*dec_tree=_tmp543;
# 3668 "toc.cyc"
struct _tuple30 _tmp577=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp69=_tmp577;struct _tuple30 _tmp578=_stmttmp69;void*_tmp57A;void*_tmp579;_tmp579=_tmp578.f1;_tmp57A=_tmp578.f2;{struct _tuple1*h_var=_tmp579;struct Cyc_Absyn_Exp*h_exp=_tmp57A;
struct _tuple30 _tmp57B=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp6A=_tmp57B;struct _tuple30 _tmp57C=_stmttmp6A;void*_tmp57E;void*_tmp57D;_tmp57D=_tmp57C.f1;_tmp57E=_tmp57C.f2;{struct _tuple1*e_var=_tmp57D;struct Cyc_Absyn_Exp*e_exp=_tmp57E;
struct _tuple30 _tmp57F=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp6B=_tmp57F;struct _tuple30 _tmp580=_stmttmp6B;void*_tmp582;void*_tmp581;_tmp581=_tmp580.f1;_tmp582=_tmp580.f2;{struct _tuple1*was_thrown_var=_tmp581;struct Cyc_Absyn_Exp*was_thrown_exp=_tmp582;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3675
e_exp->topt=e_typ;{
struct _RegionHandle _tmp583=_new_region("temp");struct _RegionHandle*temp=& _tmp583;_push_region(temp);
# 3678
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*_tmp584=({struct Cyc_Absyn_Stmt*_tmp977=body;Cyc_Absyn_seq_stmt(_tmp977,
Cyc_Absyn_exp_stmt(({void*_tmp58D=0U;Cyc_Toc_fncall_exp_dl(Cyc_Toc__pop_handler_e,_tag_fat(_tmp58D,sizeof(struct Cyc_Absyn_Exp*),0));}),0U),0U);});
# 3679
struct Cyc_Absyn_Stmt*tryandpop_stmt=_tmp584;
# 3682
struct Cyc_Absyn_Stmt*_tmp585=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C->tag=10,_tmp58C->f1=e_exp,_tmp58C->f2=scs,_tmp58C->f3=dec_tree;_tmp58C;}),0U);struct Cyc_Absyn_Stmt*handler_stmt=_tmp585;
# 3684
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3687
struct Cyc_Absyn_Exp*_tmp586=({struct Cyc_Absyn_Exp*_tmp58B[1];({struct Cyc_Absyn_Exp*_tmp978=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp58B[0]=_tmp978;});Cyc_Toc_fncall_exp_dl(Cyc_Toc_setjmp_e,_tag_fat(_tmp58B,sizeof(struct Cyc_Absyn_Exp*),1));});
# 3687
struct Cyc_Absyn_Exp*setjmp_call=_tmp586;
# 3689
struct Cyc_Absyn_Stmt*_tmp587=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp58A[1];({struct Cyc_Absyn_Exp*_tmp979=Cyc_Absyn_address_exp(h_exp,0U);_tmp58A[0]=_tmp979;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__push_handler_e,_tag_fat(_tmp58A,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
# 3689
struct Cyc_Absyn_Stmt*pushhandler_call=_tmp587;
# 3691
struct Cyc_Absyn_Exp*_tmp588=Cyc_Absyn_int_exp(0U,0,0U);struct Cyc_Absyn_Exp*zero_exp=_tmp588;
struct Cyc_Absyn_Exp*_tmp589=Cyc_Absyn_int_exp(0U,1,0U);struct Cyc_Absyn_Exp*one_exp=_tmp589;
({void*_tmp989=({
struct _tuple1*_tmp988=h_var;void*_tmp987=h_typ;Cyc_Absyn_declare_stmt(_tmp988,_tmp987,0,({
struct Cyc_Absyn_Stmt*_tmp986=pushhandler_call;Cyc_Absyn_seq_stmt(_tmp986,({
struct _tuple1*_tmp985=was_thrown_var;void*_tmp984=was_thrown_typ;struct Cyc_Absyn_Exp*_tmp983=zero_exp;Cyc_Absyn_declare_stmt(_tmp985,_tmp984,_tmp983,({
struct Cyc_Absyn_Stmt*_tmp982=({struct Cyc_Absyn_Exp*_tmp97B=setjmp_call;struct Cyc_Absyn_Stmt*_tmp97A=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3697
Cyc_Absyn_ifthenelse_stmt(_tmp97B,_tmp97A,
# 3699
Cyc_Toc_skip_stmt_dl(),0U);});
# 3697
Cyc_Absyn_seq_stmt(_tmp982,({
# 3700
struct Cyc_Absyn_Exp*_tmp981=Cyc_Absyn_prim1_exp(11U,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmp980=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_tmp981,_tmp980,({
# 3702
struct _tuple1*_tmp97F=e_var;void*_tmp97E=e_typ;struct Cyc_Absyn_Exp*_tmp97D=({
void*_tmp97C=e_typ;Cyc_Toc_cast_it(_tmp97C,Cyc_Toc_get_exn_thrown_expression());});
# 3702
Cyc_Absyn_declare_stmt(_tmp97F,_tmp97E,_tmp97D,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3693
s->r=_tmp989;});}}
# 3706
_npop_handler(0);return;
# 3676
;_pop_region();}}}}}};}}
# 3713
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmp98A=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmp98A;});
({void*_tmp98B=Cyc_Toc_typ_to_c(_check_null(f->cached_type));f->cached_type=_tmp98B;});{
struct _RegionHandle _tmp58E=_new_region("frgn");struct _RegionHandle*frgn=& _tmp58E;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp58F=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_tmp58F;
{struct Cyc_List_List*_tmp590=(f->i).args;struct Cyc_List_List*args=_tmp590;for(0;args != 0;args=args->tl){
({void*_tmp98C=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmp98C;});}}
# 3727
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 3731
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp591=_cycalloc(sizeof(*_tmp591));({struct Cyc_Hashtable_Table*_tmp98D=({(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;})(*_check_null(Cyc_Toc_gpop_tables),f);*_tmp591=_tmp98D;});_tmp591;});
if((unsigned)(f->i).cyc_varargs &&(_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp592=*_check_null((f->i).cyc_varargs);struct Cyc_Absyn_VarargInfo _stmttmp6C=_tmp592;struct Cyc_Absyn_VarargInfo _tmp593=_stmttmp6C;int _tmp597;void*_tmp596;struct Cyc_Absyn_Tqual _tmp595;void*_tmp594;_tmp594=_tmp593.name;_tmp595=_tmp593.tq;_tmp596=_tmp593.type;_tmp597=_tmp593.inject;{struct _fat_ptr*n=_tmp594;struct Cyc_Absyn_Tqual tq=_tmp595;void*t=_tmp596;int i=_tmp597;
void*_tmp598=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));void*t2=_tmp598;
struct _tuple1*_tmp599=({struct _tuple1*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->f1=Cyc_Absyn_Loc_n,_tmp59C->f2=_check_null(n);_tmp59C;});struct _tuple1*x2=_tmp599;
({struct Cyc_List_List*_tmp991=({struct Cyc_List_List*(*_tmp990)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp98F=(f->i).args;_tmp990(_tmp98F,({struct Cyc_List_List*_tmp59B=_cycalloc(sizeof(*_tmp59B));({struct _tuple9*_tmp98E=({struct _tuple9*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->f1=n,_tmp59A->f2=tq,_tmp59A->f3=t2;_tmp59A;});_tmp59B->hd=_tmp98E;}),_tmp59B->tl=0;_tmp59B;}));});(f->i).args=_tmp991;});
(f->i).cyc_varargs=0;}}
# 3740
{struct Cyc_List_List*_tmp59D=(struct Cyc_List_List*)(_check_null(f->param_vardecls))->v;struct Cyc_List_List*arg_vds=_tmp59D;for(0;arg_vds != 0;arg_vds=arg_vds->tl){
({void*_tmp992=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_tmp992;});}}
({struct Cyc_Toc_Env*_tmp993=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_tmp993,f->body);});}
# 3722
;_pop_region();}}
# 3745
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp59E=s;switch((int)_tmp59E){case Cyc_Absyn_Abstract:
 return 2U;case Cyc_Absyn_ExternC:
 return 3U;default:
 return s;};}struct _tuple43{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3761 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3763
struct _tuple43 _tmp59F=*env;struct _tuple43 _stmttmp6D=_tmp59F;struct _tuple43 _tmp5A0=_stmttmp6D;void*_tmp5A2;void*_tmp5A1;_tmp5A1=_tmp5A0.f1;_tmp5A2=*_tmp5A0.f2;{struct Cyc_Toc_TocState*s=_tmp5A1;struct Cyc_Absyn_Aggrdecl*ad=_tmp5A2;
struct _tuple1*_tmp5A3=ad->name;struct _tuple1*n=_tmp5A3;
struct Cyc_Toc_TocState _tmp5A4=*s;struct Cyc_Toc_TocState _stmttmp6E=_tmp5A4;struct Cyc_Toc_TocState _tmp5A5=_stmttmp6E;void*_tmp5A6;_tmp5A6=_tmp5A5.aggrs_so_far;{struct Cyc_Dict_Dict*aggrs_so_far=_tmp5A6;
int seen_defn_before;
struct _tuple18**_tmp5A7=({(struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;})(*aggrs_so_far,n);struct _tuple18**dopt=_tmp5A7;
if(dopt == 0){
seen_defn_before=0;{
struct _tuple18*v=({struct _tuple18*_tmp5A8=_region_malloc(d,sizeof(*_tmp5A8));
_tmp5A8->f1=ad,(int)ad->kind == 0?({void*_tmp995=Cyc_Absyn_strctq(n);_tmp5A8->f2=_tmp995;}):({void*_tmp994=Cyc_Absyn_unionq_type(n);_tmp5A8->f2=_tmp994;});_tmp5A8;});
({struct Cyc_Dict_Dict _tmp996=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))Cyc_Dict_insert;})(*aggrs_so_far,n,v);*aggrs_so_far=_tmp996;});}}else{
# 3774
struct _tuple18*_tmp5A9=*dopt;struct _tuple18*_stmttmp6F=_tmp5A9;struct _tuple18*_tmp5AA=_stmttmp6F;void*_tmp5AC;void*_tmp5AB;_tmp5AB=_tmp5AA->f1;_tmp5AC=_tmp5AA->f2;{struct Cyc_Absyn_Aggrdecl*ad2=_tmp5AB;void*t=_tmp5AC;
if(ad2->impl == 0){
({struct Cyc_Dict_Dict _tmp99A=({struct Cyc_Dict_Dict(*_tmp999)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))Cyc_Dict_insert;});struct Cyc_Dict_Dict _tmp998=*aggrs_so_far;struct _tuple1*_tmp997=n;_tmp999(_tmp998,_tmp997,({struct _tuple18*_tmp5AD=_region_malloc(d,sizeof(*_tmp5AD));_tmp5AD->f1=ad,_tmp5AD->f2=t;_tmp5AD;}));});*aggrs_so_far=_tmp99A;});
seen_defn_before=0;}else{
# 3779
seen_defn_before=1;}}}{
# 3781
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->kind=ad->kind,_tmp5D2->sc=2U,_tmp5D2->name=ad->name,_tmp5D2->tvs=0,_tmp5D2->impl=0,_tmp5D2->expected_mem_kind=0,_tmp5D2->attributes=ad->attributes;_tmp5D2;});
# 3788
if(ad->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmp99B=({struct Cyc_Absyn_AggrdeclImpl*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->exist_vars=0,_tmp5AE->rgn_po=0,_tmp5AE->fields=0,_tmp5AE->tagged=0;_tmp5AE;});new_ad->impl=_tmp99B;});{
# 3793
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp5AF=(_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp5AF;for(0;fields != 0;fields=fields->tl){
# 3797
struct Cyc_Absyn_Aggrfield*_tmp5B0=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*old_field=_tmp5B0;
void*_tmp5B1=old_field->type;void*old_type=_tmp5B1;
struct Cyc_List_List*_tmp5B2=old_field->attributes;struct Cyc_List_List*old_atts=_tmp5B2;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind == 0 && fields->tl == 0 ||(int)ad->kind == 1)){
# 3811 "toc.cyc"
void*_tmp5B3=Cyc_Absyn_compress(old_type);void*_stmttmp70=_tmp5B3;void*_tmp5B4=_stmttmp70;unsigned _tmp5B8;void*_tmp5B7;struct Cyc_Absyn_Tqual _tmp5B6;void*_tmp5B5;if(*((int*)_tmp5B4)== 4){_tmp5B5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B4)->f1).elt_type;_tmp5B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B4)->f1).tq;_tmp5B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B4)->f1).zero_term;_tmp5B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B4)->f1).zt_loc;{void*et=_tmp5B5;struct Cyc_Absyn_Tqual tq=_tmp5B6;void*zt=_tmp5B7;unsigned ztl=_tmp5B8;
# 3814
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->tag=4,(_tmp5B9->f1).elt_type=et,(_tmp5B9->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp99C=Cyc_Absyn_uint_exp(0U,0U);(_tmp5B9->f1).num_elts=_tmp99C;}),(_tmp5B9->f1).zero_term=zt,(_tmp5B9->f1).zt_loc=ztl;_tmp5B9;});
goto _LL9;}}else{
# 3817
old_atts=({struct Cyc_List_List*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));({void*_tmp99D=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->tag=6,_tmp5BA->f1=0;_tmp5BA;});_tmp5BB->hd=_tmp99D;}),_tmp5BB->tl=old_atts;_tmp5BB;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->tag=4,({void*_tmp99F=Cyc_Toc_void_star_type();(_tmp5BC->f1).elt_type=_tmp99F;}),(_tmp5BC->f1).tq=Cyc_Toc_mt_tq,({
# 3820
struct Cyc_Absyn_Exp*_tmp99E=Cyc_Absyn_uint_exp(0U,0U);(_tmp5BC->f1).num_elts=_tmp99E;}),(_tmp5BC->f1).zero_term=Cyc_Absyn_false_type,(_tmp5BC->f1).zt_loc=0U;_tmp5BC;});}_LL9:;}{
# 3824
struct Cyc_Absyn_Aggrfield*_tmp5BD=({struct Cyc_Absyn_Aggrfield*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->name=old_field->name,_tmp5CE->tq=Cyc_Toc_mt_tq,({
# 3826
void*_tmp9A1=Cyc_Toc_typ_to_c(old_type);_tmp5CE->type=_tmp9A1;}),_tmp5CE->width=old_field->width,({
# 3828
struct Cyc_List_List*_tmp9A0=Cyc_Atts_atts2c(old_atts);_tmp5CE->attributes=_tmp9A0;}),_tmp5CE->requires_clause=0;_tmp5CE;});
# 3824
struct Cyc_Absyn_Aggrfield*new_field=_tmp5BD;
# 3834
if((_check_null(ad->impl))->tagged){
void*_tmp5BE=new_field->type;void*T=_tmp5BE;
struct _fat_ptr*_tmp5BF=new_field->name;struct _fat_ptr*f=_tmp5BF;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5CB=({struct Cyc_String_pa_PrintArg_struct _tmp6E7;_tmp6E7.tag=0,_tmp6E7.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp6E7;});struct Cyc_String_pa_PrintArg_struct _tmp5CC=({struct Cyc_String_pa_PrintArg_struct _tmp6E6;_tmp6E6.tag=0,_tmp6E6.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp6E6;});void*_tmp5C9[2];_tmp5C9[0]=& _tmp5CB,_tmp5C9[1]=& _tmp5CC;({struct _fat_ptr _tmp9A2=({const char*_tmp5CA="_union_%s_%s";_tag_fat(_tmp5CA,sizeof(char),13U);});Cyc_aprintf(_tmp9A2,_tag_fat(_tmp5C9,sizeof(void*),2));});});
struct _fat_ptr*str=({struct _fat_ptr*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));*_tmp5C8=s;_tmp5C8;});
struct Cyc_Absyn_Aggrfield*_tmp5C0=({struct Cyc_Absyn_Aggrfield*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7->name=Cyc_Toc_val_sp,_tmp5C7->tq=Cyc_Toc_mt_tq,_tmp5C7->type=T,_tmp5C7->width=0,_tmp5C7->attributes=0,_tmp5C7->requires_clause=0;_tmp5C7;});struct Cyc_Absyn_Aggrfield*value_field=_tmp5C0;
struct Cyc_Absyn_Aggrfield*_tmp5C1=({struct Cyc_Absyn_Aggrfield*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6->name=Cyc_Toc_tag_sp,_tmp5C6->tq=Cyc_Toc_mt_tq,_tmp5C6->type=Cyc_Absyn_sint_type,_tmp5C6->width=0,_tmp5C6->attributes=0,_tmp5C6->requires_clause=0;_tmp5C6;});struct Cyc_Absyn_Aggrfield*tag_field=_tmp5C1;
struct Cyc_Absyn_Aggrdecl*_tmp5C2=({struct _fat_ptr*_tmp9A3=str;Cyc_Toc_make_c_struct_defn(_tmp9A3,({struct Cyc_Absyn_Aggrfield*_tmp5C5[2];_tmp5C5[0]=tag_field,_tmp5C5[1]=value_field;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp5C5,sizeof(struct Cyc_Absyn_Aggrfield*),2));}));});struct Cyc_Absyn_Aggrdecl*ad2=_tmp5C2;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));({struct Cyc_Absyn_Decl*_tmp9A4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->tag=5,_tmp5C3->f1=ad2;_tmp5C3;}),0U);_tmp5C4->hd=_tmp9A4;}),_tmp5C4->tl=Cyc_Toc_result_decls;_tmp5C4;});
({void*_tmp9A5=Cyc_Absyn_strct(str);new_field->type=_tmp9A5;});}
# 3845
new_fields=({struct Cyc_List_List*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->hd=new_field,_tmp5CD->tl=new_fields;_tmp5CD;});}}}
# 3847
({struct Cyc_List_List*_tmp9A6=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(new_fields);(new_ad->impl)->fields=_tmp9A6;});}}
# 3850
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));({struct Cyc_Absyn_Decl*_tmp9A8=({struct Cyc_Absyn_Decl*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));({void*_tmp9A7=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF->tag=5,_tmp5CF->f1=new_ad;_tmp5CF;});_tmp5D0->r=_tmp9A7;}),_tmp5D0->loc=0U;_tmp5D0;});_tmp5D1->hd=_tmp9A8;}),_tmp5D1->tl=Cyc_Toc_result_decls;_tmp5D1;});
return 0;}}}}
# 3854
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
({(int(*)(struct Cyc_Absyn_Aggrdecl**,int(*)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state;})(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple44{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3882 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple44*env){
# 3885
struct _tuple44 _tmp5D3=*env;struct _tuple44 _stmttmp71=_tmp5D3;struct _tuple44 _tmp5D4=_stmttmp71;void*_tmp5D6;void*_tmp5D5;_tmp5D5=(_tmp5D4.f1)->datatypes_so_far;_tmp5D6=_tmp5D4.f2;{struct Cyc_Set_Set**datatypes_so_far=_tmp5D5;struct Cyc_Absyn_Datatypedecl*tud=_tmp5D6;
struct _tuple1*_tmp5D7=tud->name;struct _tuple1*n=_tmp5D7;
if(tud->fields == 0 ||({(int(*)(struct Cyc_Set_Set*,struct _tuple1*))Cyc_Set_member;})(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_tmp9A9=({(struct Cyc_Set_Set*(*)(struct _RegionHandle*,struct Cyc_Set_Set*,struct _tuple1*))Cyc_Set_rinsert;})(d,*datatypes_so_far,n);*datatypes_so_far=_tmp9A9;});
{struct Cyc_List_List*_tmp5D8=(struct Cyc_List_List*)(_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp5D8;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3893
struct Cyc_List_List*_tmp5D9=0;struct Cyc_List_List*fs=_tmp5D9;
int i=1;
{struct Cyc_List_List*_tmp5DA=f->typs;struct Cyc_List_List*ts=_tmp5DA;for(0;ts != 0;(ts=ts->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmp5DB=({struct Cyc_Absyn_Aggrfield*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));({struct _fat_ptr*_tmp9AB=Cyc_Absyn_fieldname(i);_tmp5DD->name=_tmp9AB;}),_tmp5DD->tq=(*((struct _tuple23*)ts->hd)).f1,({
void*_tmp9AA=Cyc_Toc_typ_to_c((*((struct _tuple23*)ts->hd)).f2);_tmp5DD->type=_tmp9AA;}),_tmp5DD->width=0,_tmp5DD->attributes=0,_tmp5DD->requires_clause=0;_tmp5DD;});
# 3896
struct Cyc_Absyn_Aggrfield*f=_tmp5DB;
# 3898
fs=({struct Cyc_List_List*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->hd=f,_tmp5DC->tl=fs;_tmp5DC;});}}
# 3900
fs=({struct Cyc_List_List*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));({struct Cyc_Absyn_Aggrfield*_tmp9AD=({struct Cyc_Absyn_Aggrfield*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->name=Cyc_Toc_tag_sp,_tmp5DE->tq=Cyc_Toc_mt_tq,_tmp5DE->type=Cyc_Absyn_sint_type,_tmp5DE->width=0,_tmp5DE->attributes=0,_tmp5DE->requires_clause=0;_tmp5DE;});_tmp5DF->hd=_tmp9AD;}),({
struct Cyc_List_List*_tmp9AC=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fs);_tmp5DF->tl=_tmp9AC;});_tmp5DF;});{
struct Cyc_Absyn_Aggrdecl*_tmp5E0=({struct _fat_ptr*_tmp9AF=({struct _fat_ptr*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));({struct _fat_ptr _tmp9AE=({const char*_tmp5E3="";_tag_fat(_tmp5E3,sizeof(char),1U);});*_tmp5E4=_tmp9AE;});_tmp5E4;});Cyc_Toc_make_c_struct_defn(_tmp9AF,fs);});struct Cyc_Absyn_Aggrdecl*ad=_tmp5E0;
({struct _tuple1*_tmp9B0=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_tmp9B0;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));({struct Cyc_Absyn_Decl*_tmp9B1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->tag=5,_tmp5E1->f1=ad;_tmp5E1;}),0U);_tmp5E2->hd=_tmp9B1;}),_tmp5E2->tl=Cyc_Toc_result_decls;_tmp5E2;});}}}
# 3906
return 0;}}
# 3909
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
({(int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple44*)))Cyc_Toc_use_toc_state;})(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3928 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple44*env){
# 3931
struct _tuple44 _tmp5E5=*env;struct _tuple44 _stmttmp72=_tmp5E5;struct _tuple44 _tmp5E6=_stmttmp72;void*_tmp5E8;void*_tmp5E7;_tmp5E7=_tmp5E6.f1;_tmp5E8=_tmp5E6.f2;{struct Cyc_Toc_TocState*s=_tmp5E7;struct Cyc_Absyn_Datatypedecl*xd=_tmp5E8;
if(xd->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp5E9=*s;struct Cyc_Toc_TocState _stmttmp73=_tmp5E9;struct Cyc_Toc_TocState _tmp5EA=_stmttmp73;void*_tmp5EB;_tmp5EB=_tmp5EA.xdatatypes_so_far;{struct Cyc_Dict_Dict*xdatatypes_so_far=_tmp5EB;
struct _tuple1*_tmp5EC=xd->name;struct _tuple1*n=_tmp5EC;
{struct Cyc_List_List*_tmp5ED=(struct Cyc_List_List*)(_check_null(xd->fields))->v;struct Cyc_List_List*fs=_tmp5ED;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp5EE=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);struct Cyc_Absyn_Exp*sz_exp=_tmp5EE;
void*_tmp5EF=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);void*tag_typ=_tmp5EF;
# 3942
int*_tmp5F0=({(int*(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;})(*xdatatypes_so_far,f->name);int*_stmttmp74=_tmp5F0;int*_tmp5F1=_stmttmp74;if(_tmp5F1 == 0){
# 3944
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != 3)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
struct Cyc_Absyn_Vardecl*_tmp5F2=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp5F2;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));({struct Cyc_Absyn_Decl*_tmp9B2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->tag=0,_tmp5F3->f1=tag_decl;_tmp5F3;}),0U);_tmp5F4->hd=_tmp9B2;}),_tmp5F4->tl=Cyc_Toc_result_decls;_tmp5F4;});
({struct Cyc_Dict_Dict _tmp9B3=
({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))Cyc_Dict_insert;})(*xdatatypes_so_far,f->name,(int)f->sc != 3);
# 3950
*xdatatypes_so_far=_tmp9B3;});{
# 3952
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp5F5=f->typs;struct Cyc_List_List*tqts=_tmp5F5;for(0;tqts != 0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*_tmp5F6=Cyc_Absyn_fieldname(i);struct _fat_ptr*field_n=_tmp5F6;
struct Cyc_Absyn_Aggrfield*_tmp5F7=({struct Cyc_Absyn_Aggrfield*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->name=field_n,_tmp5F9->tq=(*((struct _tuple23*)tqts->hd)).f1,({
void*_tmp9B4=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqts->hd)).f2);_tmp5F9->type=_tmp9B4;}),_tmp5F9->width=0,_tmp5F9->attributes=0,_tmp5F9->requires_clause=0;_tmp5F9;});
# 3956
struct Cyc_Absyn_Aggrfield*newf=_tmp5F7;
# 3958
fields=({struct Cyc_List_List*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->hd=newf,_tmp5F8->tl=fields;_tmp5F8;});}}
# 3960
fields=({struct Cyc_List_List*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));({struct Cyc_Absyn_Aggrfield*_tmp9B7=({struct Cyc_Absyn_Aggrfield*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->name=Cyc_Toc_tag_sp,_tmp5FA->tq=Cyc_Toc_mt_tq,({
void*_tmp9B6=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp5FA->type=_tmp9B6;}),_tmp5FA->width=0,_tmp5FA->attributes=0,_tmp5FA->requires_clause=0;_tmp5FA;});
# 3960
_tmp5FB->hd=_tmp9B7;}),({
# 3962
struct Cyc_List_List*_tmp9B5=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(fields);_tmp5FB->tl=_tmp9B5;});_tmp5FB;});{
struct Cyc_Absyn_Aggrdecl*_tmp5FC=({struct _fat_ptr*_tmp9B9=({struct _fat_ptr*_tmp600=_cycalloc(sizeof(*_tmp600));({struct _fat_ptr _tmp9B8=({const char*_tmp5FF="";_tag_fat(_tmp5FF,sizeof(char),1U);});*_tmp600=_tmp9B8;});_tmp600;});Cyc_Toc_make_c_struct_defn(_tmp9B9,fields);});struct Cyc_Absyn_Aggrdecl*strct_decl=_tmp5FC;
({struct _tuple1*_tmp9BA=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_tmp9BA;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));({struct Cyc_Absyn_Decl*_tmp9BB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->tag=5,_tmp5FD->f1=strct_decl;_tmp5FD;}),0U);_tmp5FE->hd=_tmp9BB;}),_tmp5FE->tl=Cyc_Toc_result_decls;_tmp5FE;});
goto _LL6;}}}}else{if(*((int*)_tmp5F1)== 0){
# 3968
if((int)f->sc != 3){
struct Cyc_Absyn_Exp*_tmp601=Cyc_Absyn_string_exp(*fn,0U);struct Cyc_Absyn_Exp*initopt=_tmp601;
struct Cyc_Absyn_Vardecl*_tmp602=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp602;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp604=_cycalloc(sizeof(*_tmp604));({struct Cyc_Absyn_Decl*_tmp9BC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->tag=0,_tmp603->f1=tag_decl;_tmp603;}),0U);_tmp604->hd=_tmp9BC;}),_tmp604->tl=Cyc_Toc_result_decls;_tmp604;});
({struct Cyc_Dict_Dict _tmp9BD=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))Cyc_Dict_insert;})(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_tmp9BD;});}
# 3975
goto _LL6;}else{
goto _LL6;}}_LL6:;}}
# 3979
return 0;}}}}
# 3982
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
({(int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple44*)))Cyc_Toc_use_toc_state;})(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3986
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=2U;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)(_check_null(ed->fields))->v);}
# 3992
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmp9BE=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmp9BE;});
# 3996
if((int)vd->sc == 5 && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=2U;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=_check_null(vd->initializer);
if((int)vd->sc == 0){
# 4005
struct _RegionHandle _tmp605=_new_region("temp");struct _RegionHandle*temp=& _tmp605;_push_region(temp);
{struct Cyc_Toc_Env*_tmp606=Cyc_Toc_set_toplevel(temp,nv);struct Cyc_Toc_Env*nv2=_tmp606;
Cyc_Toc_exp_to_c(nv2,init);}
# 4006
;_pop_region();}else{
# 4010
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4013
void*_tmp607=Cyc_Absyn_compress(old_typ);void*_stmttmp75=_tmp607;void*_tmp608=_stmttmp75;void*_tmp60B;void*_tmp60A;void*_tmp609;if(*((int*)_tmp608)== 4){_tmp609=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp608)->f1).elt_type;_tmp60A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp608)->f1).num_elts;_tmp60B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp608)->f1).zero_term;{void*et=_tmp609;struct Cyc_Absyn_Exp*num_elts_opt=_tmp60A;void*zt=_tmp60B;
# 4015
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E8;_tmp6E8.tag=0,({struct _fat_ptr _tmp9BF=({const char*_tmp60E="can't initialize zero-terminated array -- size unknown";_tag_fat(_tmp60E,sizeof(char),55U);});_tmp6E8.f1=_tmp9BF;});_tmp6E8;});void*_tmp60C[1];_tmp60C[0]=& _tmp60D;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp60C,sizeof(void*),1));});{
struct Cyc_Absyn_Exp*_tmp60F=({struct Cyc_Absyn_Exp*_tmp9C1=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmp9C1,({
struct Cyc_Absyn_Exp*_tmp9C0=num_elts_opt;Cyc_Absyn_add_exp(_tmp9C0,Cyc_Absyn_signed_int_exp(-1,0U),0U);}),0U);});
# 4018
struct Cyc_Absyn_Exp*lhs=_tmp60F;
# 4021
struct Cyc_Absyn_Exp*_tmp610=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*rhs=_tmp610;
({void*_tmp9C3=({struct Cyc_Absyn_Stmt*_tmp9C2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_tmp9C2,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4022
s->r=_tmp9C3;});}}
# 4025
goto _LL0;}}else{
goto _LL0;}_LL0:;}}
# 4032
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp611=d;void*_tmp614;void*_tmp613;void*_tmp612;switch(*((int*)_tmp611)){case 0:
 return d;case 1: _tmp612=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp611)->f1;{struct Cyc_Tcpat_Rhs*rhs=_tmp612;
rhs->rhs=success;return d;}default: _tmp612=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp611)->f1;_tmp613=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp611)->f2;_tmp614=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp611)->f3;{struct Cyc_List_List*path=_tmp612;struct Cyc_List_List*sws=_tmp613;void**d2=(void**)_tmp614;
# 4037
({void*_tmp9C4=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmp9C4;});
for(1;sws != 0;sws=sws->tl){
struct _tuple34*_tmp615=(struct _tuple34*)sws->hd;struct _tuple34*_stmttmp76=_tmp615;struct _tuple34*_tmp616=_stmttmp76;void*_tmp617;_tmp617=(void**)& _tmp616->f2;{void**d2=(void**)_tmp617;
({void*_tmp9C5=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmp9C5;});}}
# 4042
return d;}};}
# 4053 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4055
struct _RegionHandle _tmp618=_new_region("rgn");struct _RegionHandle*rgn=& _tmp618;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp619=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp619;
void*_tmp61A=_check_null(e->topt);void*t=_tmp61A;
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4065
struct Cyc_Absyn_Stmt*_tmp61B=s;struct Cyc_Absyn_Stmt*succ_stmt=_tmp61B;
if(dopt != 0)
# 4069
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 4071
struct Cyc_Absyn_Switch_clause*_tmp61C=({struct Cyc_Absyn_Switch_clause*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->pattern=p,_tmp62B->pat_vars=0,_tmp62B->where_clause=0,_tmp62B->body=succ_stmt,_tmp62B->loc=0U;_tmp62B;});struct Cyc_Absyn_Switch_clause*c1=_tmp61C;
struct Cyc_List_List*_tmp61D=({struct Cyc_List_List*(*_tmp9C8)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c;});struct _RegionHandle*_tmp9C7=rgn;struct _RegionHandle*_tmp9C6=rgn;_tmp9C8(_tmp9C7,Cyc_Toc_gen_labels,_tmp9C6,({struct Cyc_Absyn_Switch_clause*_tmp62A[1];_tmp62A[0]=c1;({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist;})(rgn,_tag_fat(_tmp62A,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));});struct Cyc_List_List*lscs=_tmp61D;
# 4074
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4078
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4082
for(1;1;mybodies=(_check_null(mybodies))->tl){
if(mybodies == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp61F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E9;_tmp6E9.tag=0,({struct _fat_ptr _tmp9C9=({const char*_tmp620="letdecl_to_c: couldn't find env!";_tag_fat(_tmp620,sizeof(char),33U);});_tmp6E9.f1=_tmp9C9;});_tmp6E9;});void*_tmp61E[1];_tmp61E[0]=& _tmp61F;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp61E,sizeof(void*),1));});{
struct _tuple42*_tmp621=(struct _tuple42*)(_check_null(mybodies))->hd;struct _tuple42*_stmttmp77=_tmp621;struct _tuple42*_tmp622=_stmttmp77;void*_tmp624;void*_tmp623;_tmp623=_tmp622->f1;_tmp624=_tmp622->f3;{struct Cyc_Toc_Env*env=_tmp623;struct Cyc_Absyn_Stmt*st=_tmp624;
if(st == succ_stmt){Cyc_Toc_stmt_to_c(env,s);break;}}}}{
# 4089
struct Cyc_Absyn_Stmt*res=test_tree;
# 4091
for(1;mydecls != 0;mydecls=(_check_null(mydecls))->tl){
struct _tuple40*_tmp625=(struct _tuple40*)(_check_null(mydecls))->hd;struct _tuple40*_stmttmp78=_tmp625;struct _tuple40*_tmp626=_stmttmp78;void*_tmp627;_tmp627=_tmp626->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp627;
res=({struct Cyc_Absyn_Decl*_tmp9CA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=0,_tmp628->f1=vd;_tmp628;}),0U);Cyc_Absyn_decl_stmt(_tmp9CA,res,0U);});}}{
# 4096
struct Cyc_Absyn_Stmt*_tmp629=({struct _tuple1*_tmp9CD=v;void*_tmp9CC=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_tmp9CB=e;Cyc_Absyn_declare_stmt(_tmp9CD,_tmp9CC,_tmp9CB,res,0U);});_npop_handler(0);return _tmp629;}}}}}
# 4056
;_pop_region();}
# 4102
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp62C=e->r;void*_stmttmp79=_tmp62C;void*_tmp62D=_stmttmp79;void*_tmp630;void*_tmp62F;void*_tmp62E;switch(*((int*)_tmp62D)){case 41: _tmp62E=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;{struct Cyc_Absyn_Exp*e=_tmp62E;
_tmp62E=e;goto _LL4;}case 20: _tmp62E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp62E;
_tmp62E=e;goto _LL6;}case 21: _tmp62E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp62E;
_tmp62E=e;goto _LL8;}case 22: _tmp62E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp62E;
_tmp62E=e;goto _LLA;}case 15: _tmp62E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp62E;
_tmp62E=e;goto _LLC;}case 11: _tmp62E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp62E;
_tmp62E=e;goto _LLE;}case 12: _tmp62E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp62E;
_tmp62E=e;goto _LL10;}case 18: _tmp62E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp62E;
_tmp62E=e;goto _LL12;}case 5: _tmp62E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp62E;
Cyc_Toc_exptypes_to_c(e1);goto _LL0;}case 3: _tmp62E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;{struct Cyc_List_List*es=_tmp62E;
({(void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_iter;})(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 7: _tmp62E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_tmp62F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;{struct Cyc_Absyn_Exp*e1=_tmp62E;struct Cyc_Absyn_Exp*e2=_tmp62F;
_tmp62E=e1;_tmp62F=e2;goto _LL18;}case 8: _tmp62E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_tmp62F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp62E;struct Cyc_Absyn_Exp*e2=_tmp62F;
_tmp62E=e1;_tmp62F=e2;goto _LL1A;}case 9: _tmp62E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_tmp62F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp62E;struct Cyc_Absyn_Exp*e2=_tmp62F;
_tmp62E=e1;_tmp62F=e2;goto _LL1C;}case 23: _tmp62E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_tmp62F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp62E;struct Cyc_Absyn_Exp*e2=_tmp62F;
_tmp62E=e1;_tmp62F=e2;goto _LL1E;}case 35: _tmp62E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_tmp62F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp62E;struct Cyc_Absyn_Exp*e2=_tmp62F;
_tmp62E=e1;_tmp62F=e2;goto _LL20;}case 4: _tmp62E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_tmp62F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp62D)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp62E;struct Cyc_Absyn_Exp*e2=_tmp62F;
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);goto _LL0;}case 6: _tmp62E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_tmp62F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;_tmp630=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62D)->f3;{struct Cyc_Absyn_Exp*e1=_tmp62E;struct Cyc_Absyn_Exp*e2=_tmp62F;struct Cyc_Absyn_Exp*e3=_tmp630;
# 4121
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);goto _LL0;}case 10: _tmp62E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_tmp62F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;{struct Cyc_Absyn_Exp*e=_tmp62E;struct Cyc_List_List*es=_tmp62F;
# 4123
Cyc_Toc_exptypes_to_c(e);({(void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_iter;})(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 14: _tmp62E=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_tmp62F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;{void**t=_tmp62E;struct Cyc_Absyn_Exp*e=_tmp62F;
({void*_tmp9CE=Cyc_Toc_typ_to_c(*t);*t=_tmp9CE;});Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 25: _tmp62E=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp62D)->f1)->f3;_tmp62F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;{void**t=_tmp62E;struct Cyc_List_List*dles=_tmp62F;
# 4126
({void*_tmp9CF=Cyc_Toc_typ_to_c(*t);*t=_tmp9CF;});
_tmp62E=dles;goto _LL2A;}case 36: _tmp62E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp62D)->f2;_LL2A: {struct Cyc_List_List*dles=_tmp62E;
_tmp62E=dles;goto _LL2C;}case 26: _tmp62E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LL2C: {struct Cyc_List_List*dles=_tmp62E;
# 4130
for(1;dles != 0;dles=dles->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}case 19: _tmp62E=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;{void**t=_tmp62E;
_tmp62E=t;goto _LL30;}case 17: _tmp62E=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;_LL30: {void**t=_tmp62E;
({void*_tmp9D0=Cyc_Toc_typ_to_c(*t);*t=_tmp9D0;});goto _LL0;}case 37: _tmp62E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;{struct Cyc_Absyn_Stmt*s=_tmp62E;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 34: _tmp62E=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp62D)->f1;{struct Cyc_Absyn_MallocInfo*m=_tmp62E;
# 4137
if(m->elt_type != 0)
({void**_tmp9D2=({void**_tmp631=_cycalloc(sizeof(*_tmp631));({void*_tmp9D1=Cyc_Toc_typ_to_c(*_check_null(m->elt_type));*_tmp631=_tmp9D1;});_tmp631;});m->elt_type=_tmp9D2;});
Cyc_Toc_exptypes_to_c(m->num_elts);
goto _LL0;}case 40: _tmp62E=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp62D)->f3;_tmp62F=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp62D)->f4;{struct Cyc_List_List*o=_tmp62E;struct Cyc_List_List*i=_tmp62F;
# 4142
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL0;}case 0:
 goto _LL3A;case 1: _LL3A:
 goto _LL3C;case 32: _LL3C:
 goto _LL3E;case 33: _LL3E:
 goto _LL0;case 42:
# 4150
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
({void*_tmp632=0U;({int(*_tmp9D5)(unsigned,struct _fat_ptr,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;});unsigned _tmp9D4=e->loc;struct _fat_ptr _tmp9D3=({const char*_tmp633="Cyclone expression in C code";_tag_fat(_tmp633,sizeof(char),29U);});_tmp9D5(_tmp9D4,_tmp9D3,_tag_fat(_tmp632,sizeof(void*),0));});});}_LL0:;}
# 4165
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp634=d->r;void*_stmttmp7A=_tmp634;void*_tmp635=_stmttmp7A;void*_tmp636;switch(*((int*)_tmp635)){case 0: _tmp636=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp635)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp636;
# 4168
({void*_tmp9D6=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmp9D6;});
if(vd->initializer != 0)Cyc_Toc_exptypes_to_c(_check_null(vd->initializer));
goto _LL0;}case 1: _tmp636=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp635)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp636;
# 4172
({void*_tmp9D7=Cyc_Toc_typ_to_c((fd->i).ret_type);(fd->i).ret_type=_tmp9D7;});
{struct Cyc_List_List*_tmp637=(fd->i).args;struct Cyc_List_List*args=_tmp637;for(0;args != 0;args=args->tl){
({void*_tmp9D8=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmp9D8;});}}
goto _LL0;}case 5: _tmp636=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp635)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp636;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL0;}case 7: _tmp636=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp635)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp636;
# 4178
if(ed->fields != 0){
struct Cyc_List_List*_tmp638=(struct Cyc_List_List*)(_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp638;for(0;fs != 0;fs=fs->tl){
if(((struct Cyc_Absyn_Enumfield*)fs->hd)->tag != 0)Cyc_Toc_exptypes_to_c(_check_null(((struct Cyc_Absyn_Enumfield*)fs->hd)->tag));}}
goto _LL0;}case 8: _tmp636=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp635)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp636;
({void*_tmp9D9=Cyc_Toc_typ_to_c(_check_null(td->defn));td->defn=_tmp9D9;});goto _LL0;}case 2:
 goto _LLE;case 3: _LLE:
 goto _LL10;case 6: _LL10:
 goto _LL12;case 9: _LL12:
 goto _LL14;case 10: _LL14:
 goto _LL16;case 11: _LL16:
 goto _LL18;case 12: _LL18:
 goto _LL1A;case 4: _LL1A:
# 4191
({void*_tmp639=0U;({int(*_tmp9DC)(unsigned,struct _fat_ptr,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;});unsigned _tmp9DB=d->loc;struct _fat_ptr _tmp9DA=({const char*_tmp63A="Cyclone declaration in C code";_tag_fat(_tmp63A,sizeof(char),30U);});_tmp9DC(_tmp9DB,_tmp9DA,_tag_fat(_tmp639,sizeof(void*),0));});});case 13:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
 goto _LL0;}_LL0:;}
# 4199
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp63B=s->r;void*_stmttmp7B=_tmp63B;void*_tmp63C=_stmttmp7B;void*_tmp640;void*_tmp63F;void*_tmp63E;void*_tmp63D;switch(*((int*)_tmp63C)){case 1: _tmp63D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp63C)->f1;{struct Cyc_Absyn_Exp*e=_tmp63D;
Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 2: _tmp63D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp63C)->f1;_tmp63E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp63C)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp63D;struct Cyc_Absyn_Stmt*s2=_tmp63E;
Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 3: _tmp63D=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp63C)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp63D;
if(eopt != 0)Cyc_Toc_exptypes_to_c(eopt);goto _LL0;}case 4: _tmp63D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp63C)->f1;_tmp63E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp63C)->f2;_tmp63F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp63C)->f3;{struct Cyc_Absyn_Exp*e=_tmp63D;struct Cyc_Absyn_Stmt*s1=_tmp63E;struct Cyc_Absyn_Stmt*s2=_tmp63F;
# 4205
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 5: _tmp63D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp63C)->f1).f1;_tmp63E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp63C)->f2;{struct Cyc_Absyn_Exp*e=_tmp63D;struct Cyc_Absyn_Stmt*s=_tmp63E;
# 4207
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 9: _tmp63D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp63C)->f1;_tmp63E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp63C)->f2).f1;_tmp63F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp63C)->f3).f1;_tmp640=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp63C)->f4;{struct Cyc_Absyn_Exp*e1=_tmp63D;struct Cyc_Absyn_Exp*e2=_tmp63E;struct Cyc_Absyn_Exp*e3=_tmp63F;struct Cyc_Absyn_Stmt*s=_tmp640;
# 4209
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 10: _tmp63D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp63C)->f1;_tmp63E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp63C)->f2;_tmp63F=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp63C)->f3;{struct Cyc_Absyn_Exp*e=_tmp63D;struct Cyc_List_List*scs=_tmp63E;void*dec_tree=_tmp63F;
# 4212
Cyc_Toc_exptypes_to_c(e);
for(1;scs != 0;scs=scs->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12: _tmp63D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp63C)->f1;_tmp63E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp63C)->f2;{struct Cyc_Absyn_Decl*d=_tmp63D;struct Cyc_Absyn_Stmt*s=_tmp63E;
Cyc_Toc_decltypes_to_c(d);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 14: _tmp63D=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp63C)->f1;_tmp63E=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp63C)->f2).f1;{struct Cyc_Absyn_Stmt*s=_tmp63D;struct Cyc_Absyn_Exp*e=_tmp63E;
Cyc_Toc_stmttypes_to_c(s);Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 13: _tmp63D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp63C)->f2;{struct Cyc_Absyn_Stmt*s=_tmp63D;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 0:
 goto _LL18;case 6: _LL18:
 goto _LL1A;case 7: _LL1A:
 goto _LL1C;case 8: _LL1C:
 goto _LL0;case 11:
# 4224
({void*_tmp9DD=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->tag=0;_tmp641;});s->r=_tmp9DD;});goto _LL0;default:
({void*_tmp642=0U;({int(*_tmp9E0)(unsigned,struct _fat_ptr,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;});unsigned _tmp9DF=s->loc;struct _fat_ptr _tmp9DE=({const char*_tmp643="Cyclone statement in C code";_tag_fat(_tmp643,sizeof(char),28U);});_tmp9E0(_tmp9DF,_tmp9DE,_tag_fat(_tmp642,sizeof(void*),0));});});}_LL0:;}
# 4232
static void Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
for(1;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp645=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6EA;_tmp6EA.tag=0,({struct _fat_ptr _tmp9E1=({const char*_tmp646="decls_to_c: not at toplevel!";_tag_fat(_tmp646,sizeof(char),29U);});_tmp6EA.f1=_tmp9E1;});_tmp6EA;});void*_tmp644[1];_tmp644[0]=& _tmp645;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp644,sizeof(void*),1));});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp647=d->r;void*_stmttmp7C=_tmp647;void*_tmp648=_stmttmp7C;void*_tmp649;switch(*((int*)_tmp648)){case 0: _tmp649=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp648)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp649;
# 4240
struct _tuple1*_tmp64A=vd->name;struct _tuple1*c_name=_tmp64A;
# 4242
if((int)vd->sc == 4)
c_name=({struct _tuple1*_tmp64B=_cycalloc(sizeof(*_tmp64B));({union Cyc_Absyn_Nmspace _tmp9E2=Cyc_Absyn_Abs_n(0,1);_tmp64B->f1=_tmp9E2;}),_tmp64B->f2=(*c_name).f2;_tmp64B;});
if(vd->initializer != 0){
if((int)vd->sc == 4)vd->sc=2U;
if(cinclude)
Cyc_Toc_exptypes_to_c(_check_null(vd->initializer));else{
# 4249
Cyc_Toc_exp_to_c(nv,_check_null(vd->initializer));}}
# 4251
vd->name=c_name;
({enum Cyc_Absyn_Scope _tmp9E3=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_tmp9E3;});
({void*_tmp9E4=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmp9E4;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->hd=d,_tmp64C->tl=Cyc_Toc_result_decls;_tmp64C;});
goto _LL0;}case 1: _tmp649=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp648)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp649;
# 4258
if((int)fd->sc == 4){
({struct _tuple1*_tmp9E6=({struct _tuple1*_tmp64D=_cycalloc(sizeof(*_tmp64D));({union Cyc_Absyn_Nmspace _tmp9E5=Cyc_Absyn_Abs_n(0,1);_tmp64D->f1=_tmp9E5;}),_tmp64D->f2=(*fd->name).f2;_tmp64D;});fd->name=_tmp9E6;});
fd->sc=2U;}
# 4262
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->hd=d,_tmp64E->tl=Cyc_Toc_result_decls;_tmp64E;});
goto _LL0;}case 2:
 goto _LL8;case 3: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp650=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6EB;_tmp6EB.tag=0,({struct _fat_ptr _tmp9E7=({const char*_tmp651="letdecl at toplevel";_tag_fat(_tmp651,sizeof(char),20U);});_tmp6EB.f1=_tmp9E7;});_tmp6EB;});void*_tmp64F[1];_tmp64F[0]=& _tmp650;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp64F,sizeof(void*),1));});case 4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp653=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6EC;_tmp6EC.tag=0,({struct _fat_ptr _tmp9E8=({const char*_tmp654="region decl at toplevel";_tag_fat(_tmp654,sizeof(char),24U);});_tmp6EC.f1=_tmp9E8;});_tmp6EC;});void*_tmp652[1];_tmp652[0]=& _tmp653;({(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;})(_tag_fat(_tmp652,sizeof(void*),1));});case 5: _tmp649=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp648)->f1;{struct Cyc_Absyn_Aggrdecl*sd=_tmp649;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6: _tmp649=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp648)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_tmp649;
# 4270
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7: _tmp649=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp648)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp649;
# 4273
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->hd=d,_tmp655->tl=Cyc_Toc_result_decls;_tmp655;});
goto _LL0;}case 8: _tmp649=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp648)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp649;
# 4277
td->tvs=0;
if(td->defn != 0){
({void*_tmp9E9=Cyc_Toc_typ_to_c(_check_null(td->defn));td->defn=_tmp9E9;});{
# 4282
void*_tmp656=td->defn;void*_stmttmp7D=_tmp656;void*_tmp657=_stmttmp7D;unsigned _tmp659;void*_tmp658;if(_tmp657 != 0){if(*((int*)_tmp657)== 10){if(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp657)->f1)->r)== 1){_tmp658=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp657)->f1)->r)->f1;_tmp659=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp657)->f1)->loc;{struct Cyc_Absyn_Enumdecl*ed2=_tmp658;unsigned loc=_tmp659;
# 4284
struct Cyc_Absyn_Decl*_tmp65A=({struct Cyc_Absyn_Decl*_tmp65F=_cycalloc(sizeof(*_tmp65F));({void*_tmp9EA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->tag=7,_tmp65E->f1=ed2;_tmp65E;});_tmp65F->r=_tmp9EA;}),_tmp65F->loc=loc;_tmp65F;});struct Cyc_Absyn_Decl*ed=_tmp65A;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->hd=ed,_tmp65B->tl=Cyc_Toc_result_decls;_tmp65B;});
({void*_tmp9EC=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->tag=0,({void*_tmp9EB=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->tag=15,_tmp65C->f1=ed2->name,_tmp65C->f2=ed2;_tmp65C;});_tmp65D->f1=_tmp9EB;}),_tmp65D->f2=0;_tmp65D;});td->defn=_tmp9EC;});
goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26:
 goto _LL23;}_LL23:;}}else{
# 4291
({void*_tmp9ED=(int)((struct Cyc_Absyn_Kind*)(_check_null(td->kind))->v)->kind == 2?Cyc_Toc_void_star_type(): Cyc_Absyn_void_type;td->defn=_tmp9ED;});}
# 4295
if(Cyc_Flags_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->hd=d,_tmp660->tl=Cyc_Toc_result_decls;_tmp660;});
goto _LL0;}case 13:
 goto _LL16;case 14: _LL16:
 goto _LL18;case 15: _LL18:
 goto _LL1A;case 16: _LL1A:
 goto _LL0;case 9: _tmp649=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp648)->f2;{struct Cyc_List_List*ds2=_tmp649;
_tmp649=ds2;goto _LL1E;}case 10: _tmp649=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp648)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp649;
_tmp649=ds2;goto _LL20;}case 11: _tmp649=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp648)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp649;
Cyc_Toc_decls_to_c(nv,ds2,cinclude);goto _LL0;}default: _tmp649=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp648)->f1;{struct Cyc_List_List*ds2=_tmp649;
Cyc_Toc_decls_to_c(nv,ds2,1);goto _LL0;}}_LL0:;}}}
# 4312
void Cyc_Toc_init (void){
struct Cyc_Core_NewDynamicRegion _tmp661=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp7E=_tmp661;struct Cyc_Core_NewDynamicRegion _tmp662=_stmttmp7E;void*_tmp663;_tmp663=_tmp662.key;{struct Cyc_Core_DynamicRegion*dyn=_tmp663;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp665=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp665));_tmp665->dyn=dyn,_tmp665->state=ts;_tmp665;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _tmp667=34;struct _fat_ptr**_tmp666=_cycalloc(_check_times(_tmp667,sizeof(struct _fat_ptr*)));_tmp666[0]=& Cyc_Toc__throw_str,_tmp666[1]=& Cyc_Toc_setjmp_str,_tmp666[2]=& Cyc_Toc__push_handler_str,_tmp666[3]=& Cyc_Toc__pop_handler_str,_tmp666[4]=& Cyc_Toc__exn_thrown_str,_tmp666[5]=& Cyc_Toc__npop_handler_str,_tmp666[6]=& Cyc_Toc__check_null_str,_tmp666[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp666[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp666[9]=& Cyc_Toc__check_fat_subscript_str,_tmp666[10]=& Cyc_Toc__tag_fat_str,_tmp666[11]=& Cyc_Toc__untag_fat_ptr_str,_tmp666[12]=& Cyc_Toc__get_fat_size_str,_tmp666[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp666[14]=& Cyc_Toc__fat_ptr_plus_str,_tmp666[15]=& Cyc_Toc__zero_arr_plus_str,_tmp666[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_tmp666[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp666[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_tmp666[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp666[20]=& Cyc_Toc__cycalloc_str,_tmp666[21]=& Cyc_Toc__cyccalloc_str,_tmp666[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp666[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp666[24]=& Cyc_Toc__region_malloc_str,_tmp666[25]=& Cyc_Toc__region_calloc_str,_tmp666[26]=& Cyc_Toc__check_times_str,_tmp666[27]=& Cyc_Toc__new_region_str,_tmp666[28]=& Cyc_Toc__push_region_str,_tmp666[29]=& Cyc_Toc__pop_region_str,_tmp666[30]=& Cyc_Toc__throw_arraybounds_str,_tmp666[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_tmp666[32]=& Cyc_Toc__throw_match_str,_tmp666[33]=& Cyc_Toc__fast_region_malloc_str;_tmp666;}),sizeof(struct _fat_ptr*),34U);}}
# 4360
void Cyc_Toc_finish (void){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp668=ts;struct Cyc_Toc_TocStateWrap*_tmp669=Cyc_Toc_toc_state;ts=_tmp669;Cyc_Toc_toc_state=_tmp668;});{
struct Cyc_Toc_TocStateWrap _tmp66A=*_check_null(ts);struct Cyc_Toc_TocStateWrap _stmttmp7F=_tmp66A;struct Cyc_Toc_TocStateWrap _tmp66B=_stmttmp7F;void*_tmp66D;void*_tmp66C;_tmp66C=_tmp66B.dyn;_tmp66D=_tmp66B.state;{struct Cyc_Core_DynamicRegion*dyn=_tmp66C;struct Cyc_Toc_TocState*s=_tmp66D;
# 4365
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _tmp66F=*s;struct Cyc_Toc_TocState _stmttmp80=_tmp66F;struct Cyc_Toc_TocState _tmp670=_stmttmp80;void*_tmp671;_tmp671=_tmp670.temp_labels;{struct Cyc_Xarray_Xarray*tls=_tmp671;
({(void(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_reuse;})(tls);}}
# 4366
;}
# 4369
Cyc_Core_free_rckey(dyn);
({(void(*)(struct Cyc_Toc_TocStateWrap*))Cyc_Core_ufree;})(ts);
# 4372
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4378
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4380
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp672=_cycalloc(sizeof(*_tmp672));*_tmp672=pop_tables;_tmp672;});
Cyc_Toc_init();{
struct _RegionHandle _tmp673=_new_region("start");struct _RegionHandle*start=& _tmp673;_push_region(start);
({struct Cyc_Toc_Env*_tmp9EE=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmp9EE,ds,0);});{
struct Cyc_List_List*_tmp674=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(Cyc_Toc_result_decls);_npop_handler(0);return _tmp674;}
# 4383
;_pop_region();}}

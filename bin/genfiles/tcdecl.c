#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file lib/runtime_cyc.c */
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

/***********************************************************************/
/* Runtime Stack routines (runtime_stack.c).                           */
/***********************************************************************/

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
  void (*cleanup)(struct _RuntimeStack *frame);
};

/***********************************************************************/
/* Low-level representations etc.                                      */
/***********************************************************************/

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Tagged arrays */
struct _dyneither_ptr {
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
  /* MWH: wish we didn't have to include the stuff below ... */
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
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

// A dynamic region is just a region handle.  We have the
// wrapper struct for type abstraction reasons.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern struct _RegionHandle *_open_dynregion(struct _DynRegionFrame *f,
                                             struct _DynRegionHandle *h);
extern void   _pop_dynregion();

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern void* _throw_null_fn(const char *filename, unsigned lineno);
extern void* _throw_arraybounds_fn(const char *filename, unsigned lineno);
extern void* _throw_badalloc_fn(const char *filename, unsigned lineno);
extern void* _throw_match_fn(const char *filename, unsigned lineno);
extern void* _throw_fn(void* e, const char *filename, unsigned lineno);
extern void* _rethrow(void* e);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

extern struct _xtunion_struct *_exn_thrown;

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
#define _check_known_subscript_null(ptr,bound,elt_sz,index)\
   ((char *)ptr) + (elt_sz)*(index))
#define _check_known_subscript_notnull(bound,index) (index)
#define _check_known_subscript_notnullX(bound,index)\
   ((char *)ptr) + (elt_sz)*(index))

#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  (_cks_ptr) + _cks_elt_sz*_cks_index; })
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  (_cks_ptr) + _cks_elt_sz*_cks_index; })

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
char * _zero_arr_plus_char_fn(char *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno);
short * _zero_arr_plus_short_fn(short *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno);
int * _zero_arr_plus_int_fn(int *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno);
float * _zero_arr_plus_float_fn(float *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno);
double * _zero_arr_plus_double_fn(double *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno);
long double * _zero_arr_plus_longdouble_fn(long double *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno);
void * _zero_arr_plus_voidstar_fn(void **orig_x, unsigned int orig_sz, int orig_i,const char *filename,unsigned lineno);
#endif

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

/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
int _get_zero_arr_size_char(const char *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_short(const short *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_int(const int *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_float(const float *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_double(const double *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_longdouble(const long double *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_voidstar(const void **orig_x, unsigned int orig_offset);

/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus_<type>_fn. */
char * _zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno);
short * _zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno);
int * _zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno);
float * _zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno);
double * _zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno);
long double * _zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno);
void * _zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno);
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

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
char * _zero_arr_inplace_plus_post_char_fn(char **x, int orig_i,const char *filename,unsigned lineno);
short * _zero_arr_inplace_plus_post_short_fn(short **x, int orig_i,const char *filename,unsigned lineno);
int * _zero_arr_inplace_plus_post_int_fn(int **x, int orig_i,const char *filename, unsigned lineno);
float * _zero_arr_inplace_plus_post_float_fn(float **x, int orig_i,const char *filename, unsigned lineno);
double * _zero_arr_inplace_plus_post_double_fn(double **x, int orig_i,const char *filename,unsigned lineno);
long double * _zero_arr_inplace_plus_post_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno);
void ** _zero_arr_inplace_plus_post_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno);
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

/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  /* JGM: if we're tagging NULL, ignore num_elts */ \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base ? (_tag_arr_ans.base + (elt_sz) * (num_elts)) : 0; \
  _tag_arr_ans; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

/* This is not a macro since initialization order matters.  Defined in
   runtime_zeroterm.c. */
extern struct _dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
  unsigned int sz,
  unsigned int numelts);

/* Allocation */
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);
/* bound the allocation size to be less than MAX_ALLOC_SIZE,
   which is defined in runtime_memory.c
*/
extern void* _bounded_GC_malloc(int,const char *file,int lineno);
extern void* _bounded_GC_malloc_atomic(int,const char *file,int lineno);
extern void* _bounded_GC_calloc(unsigned n, unsigned s,
                                const char *file,int lineno);
extern void* _bounded_GC_calloc_atomic(unsigned n, unsigned s,
                                       const char *file,
                                       int lineno);
/* FIX?  Not sure if we want to pass filename and lineno in here... */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n) _bounded_GC_malloc(n,__FILE__,__LINE__)
#define _cycalloc_atomic(n) _bounded_GC_malloc_atomic(n,__FILE__,__LINE__)
#define _cyccalloc(n,s) _bounded_GC_calloc(n,s,__FILE__,__LINE__)
#define _cyccalloc_atomic(n,s) _bounded_GC_calloc_atomic(n,s,__FILE__,__LINE__)
#endif

#define MAX_MALLOC_SIZE (1 << 28)
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

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,const char *file,const char *func,
                                int lineno);
extern void* _profile_GC_malloc_atomic(int,const char *file,
                                       const char *func,int lineno);
extern void* _profile_GC_calloc(unsigned n, unsigned s,
                                const char *file, const char *func, int lineno);
extern void* _profile_GC_calloc_atomic(unsigned n, unsigned s,
                                       const char *file, const char *func,
                                       int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern void* _profile_region_calloc(struct _RegionHandle *, unsigned,
                                    unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						const char *file,
						const char *func,
                                                int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 const char *file,
                                 const char *func,
                                 int lineno);
#  if !defined(RUNTIME_CYC)
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 165 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;
# 43 "position.h"
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 49
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 159
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 180
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 185
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 191
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U,Cyc_Absyn_BoolKind  = 6U,Cyc_Absyn_PtrBndKind  = 7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 213
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 215
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 903 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 917
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 1166
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 39 "tcdecl.h"
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple10{enum Cyc_Absyn_Scope f1;int f2;};
# 55 "tcdecl.h"
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int allow_externC_extern_merge);
# 60
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 63
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 65
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 67
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 73
void**Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 76
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 82
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 32 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned int);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 182 "tcutil.h"
int Cyc_Tcutil_unify_kindbound(void*,void*);
# 195
void Cyc_Tcutil_explain_failure();
# 197
int Cyc_Tcutil_unify(void*,void*);
# 202
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 287 "tcutil.h"
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 342
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);char Cyc_Tcdecl_Incompatible[13U]="Incompatible";
# 36 "tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};
# 38
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 42
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
struct _dyneither_ptr fmt2;
if(Cyc_strlen((struct _dyneither_ptr)*msg1)== (unsigned long)0)
fmt2=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp1D0=({const char*_tmp0="%s";_tag_dyneither(_tmp0,sizeof(char),3U);});Cyc_strconcat(_tmp1D0,(struct _dyneither_ptr)fmt);});else{
# 48
fmt2=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp1D1=({const char*_tmp1="%s ";_tag_dyneither(_tmp1,sizeof(char),4U);});Cyc_strconcat(_tmp1D1,(struct _dyneither_ptr)fmt);});}{
struct _dyneither_ptr ap2=({unsigned int _tmp4=_get_dyneither_size(ap,sizeof(void*))+ (unsigned int)1;void**_tmp3=_cycalloc(_check_times(_tmp4,sizeof(void*)));({{unsigned int _tmp168=_get_dyneither_size(ap,sizeof(void*))+ (unsigned int)1;unsigned int i;for(i=0;i < _tmp168;++ i){
i == (unsigned int)0?({void*_tmp1D2=(void*)({struct Cyc_String_pa_PrintArg_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=0U,_tmp2->f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*msg1);_tmp2;});_tmp3[i]=_tmp1D2;}):(_tmp3[i]=*((void**)_check_dyneither_subscript(ap,sizeof(void*),(int)(i - (unsigned int)1))));}}0;});_tag_dyneither(_tmp3,sizeof(void*),_tmp4);});
Cyc_Position_post_error(({unsigned int _tmp1D3=loc;Cyc_Position_mk_err(_tmp1D3,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2));}));};};}
# 56
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 58
struct _dyneither_ptr s0_str=(int)s0 == (int)Cyc_Absyn_Public?({const char*_tmpC="public ";_tag_dyneither(_tmpC,sizeof(char),8U);}): Cyc_Absynpp_scope2string(s0);
struct _dyneither_ptr s1_str=(int)s1 == (int)Cyc_Absyn_Public?({const char*_tmpB="public ";_tag_dyneither(_tmpB,sizeof(char),8U);}): Cyc_Absynpp_scope2string(s1);
({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp16C;_tmp16C.tag=0U,_tmp16C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp16C;});struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmp16B;_tmp16B.tag=0U,_tmp16B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp16B;});struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp16A;_tmp16A.tag=0U,_tmp16A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1_str);_tmp16A;});struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp169;_tmp169.tag=0U,_tmp169.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0_str);_tmp169;});void*_tmp5[4U];_tmp5[0]=& _tmp7,_tmp5[1]=& _tmp8,_tmp5[2]=& _tmp9,_tmp5[3]=& _tmpA;({unsigned int _tmp1D6=loc;struct _dyneither_ptr*_tmp1D5=msg;struct _dyneither_ptr _tmp1D4=({const char*_tmp6="%s %s is %sbut expected scope is %s";_tag_dyneither(_tmp6,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp1D6,_tmp1D5,_tmp1D4,_tag_dyneither(_tmp5,sizeof(void*),4U));});});}struct _tuple11{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 64
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int externCmerge){
# 66
{struct _tuple11 _tmpD=({struct _tuple11 _tmp16F;_tmp16F.f1=s0,_tmp16F.f2=s1;_tmp16F;});struct _tuple11 _tmpE=_tmpD;switch(_tmpE.f1){case Cyc_Absyn_ExternC: switch(_tmpE.f2){case Cyc_Absyn_ExternC: _LL1: _LL2:
# 68
 goto _LL0;case Cyc_Absyn_Public: _LL3: _LL4:
# 70
 goto _LL6;case Cyc_Absyn_Extern: _LL5: _LL6:
 goto _LL8;default: _LLB: _LLC:
# 75
 goto _LLE;}case Cyc_Absyn_Public: switch(_tmpE.f2){case Cyc_Absyn_ExternC: _LL7: _LL8:
# 72
 goto _LLA;case Cyc_Absyn_Extern: goto _LLF;case Cyc_Absyn_Public: _LL15: _LL16:
# 82
 goto _LL18;default: goto _LL1B;}case Cyc_Absyn_Extern: switch(_tmpE.f2){case Cyc_Absyn_ExternC: _LL9: _LLA:
# 73
 if(externCmerge)goto _LL0;
goto _LLC;case Cyc_Absyn_Extern: goto _LLF;default: _LL11: _LL12:
# 79
 goto _LL0;}default: switch(_tmpE.f2){case Cyc_Absyn_ExternC: _LLD: _LLE:
# 76
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple10 _tmp16D;_tmp16D.f1=s1,_tmp16D.f2=0;_tmp16D;});case Cyc_Absyn_Extern: _LLF: _LL10:
# 78
 s1=s0;goto _LL0;default: switch(_tmpE.f1){case Cyc_Absyn_Static: if(_tmpE.f2 == Cyc_Absyn_Static){_LL13: _LL14:
# 81
 goto _LL16;}else{goto _LL1B;}case Cyc_Absyn_Abstract: if(_tmpE.f2 == Cyc_Absyn_Abstract){_LL17: _LL18:
# 83
 goto _LL0;}else{goto _LL1B;}default: if(_tmpE.f2 == Cyc_Absyn_Register){_LL19: _LL1A:
 goto _LL0;}else{_LL1B: _LL1C:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple10 _tmp16E;_tmp16E.f1=s1,_tmp16E.f2=0;_tmp16E;});}}}}_LL0:;}
# 87
return({struct _tuple10 _tmp170;_tmp170.f1=s1,_tmp170.f2=1;_tmp170;});}
# 90
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 92
return Cyc_Tcutil_unify(t0,t1);}
# 96
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->r;void*_tmp10=_tmpF;int _tmp13;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Int_c).tag == 5){_LL1: _tmp13=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Int_c).val).f2;_LL2:
 return(unsigned int)_tmp13;}else{goto _LL3;}}else{_LL3: _LL4:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp1D7=({const char*_tmp11="Tcdecl::get_uint_const_value";_tag_dyneither(_tmp11,sizeof(char),29U);});_tmp12->f1=_tmp1D7;});_tmp12;}));}_LL0:;}
# 103
inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 106
if(({int _tmp1D8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0);_tmp1D8 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);})){
({struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0U,_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp172;});struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp171;_tmp171.tag=0U,_tmp171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp171;});void*_tmp14[2U];_tmp14[0]=& _tmp16,_tmp14[1]=& _tmp17;({unsigned int _tmp1DB=loc;struct _dyneither_ptr*_tmp1DA=msg;struct _dyneither_ptr _tmp1D9=({const char*_tmp15="%s %s has a different number of type parameters";_tag_dyneither(_tmp15,sizeof(char),48U);});Cyc_Tcdecl_merr(_tmp1DB,_tmp1DA,_tmp1D9,_tag_dyneither(_tmp14,sizeof(void*),2U));});});
return 0;}{
# 110
struct Cyc_List_List*_tmp18=tvs0;
struct Cyc_List_List*_tmp19=tvs1;
for(0;_tmp18 != 0;(_tmp18=_tmp18->tl,_tmp19=_tmp19->tl)){
Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp18->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp19))->hd)->kind);{
struct Cyc_Absyn_Kind*_tmp1A=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp18->hd)->kind);
struct Cyc_Absyn_Kind*_tmp1B=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp19->hd)->kind);
if(_tmp1A == _tmp1B)continue;
({struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp177;_tmp177.tag=0U,_tmp177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp177;});struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmp176;_tmp176.tag=0U,_tmp176.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp176;});struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0U,({
struct _dyneither_ptr _tmp1DC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp1A));_tmp175.f1=_tmp1DC;});_tmp175;});struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0U,_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp18->hd)->name);_tmp174;});struct Cyc_String_pa_PrintArg_struct _tmp22=({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0U,({struct _dyneither_ptr _tmp1DD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp1B));_tmp173.f1=_tmp1DD;});_tmp173;});void*_tmp1C[5U];_tmp1C[0]=& _tmp1E,_tmp1C[1]=& _tmp1F,_tmp1C[2]=& _tmp20,_tmp1C[3]=& _tmp21,_tmp1C[4]=& _tmp22;({unsigned int _tmp1E0=loc;struct _dyneither_ptr*_tmp1DF=msg;struct _dyneither_ptr _tmp1DE=({const char*_tmp1D="%s %s has a different kind (%s) for type parameter %s (%s)";_tag_dyneither(_tmp1D,sizeof(char),59U);});Cyc_Tcdecl_merr(_tmp1E0,_tmp1DF,_tmp1DE,_tag_dyneither(_tmp1C,sizeof(void*),5U));});});
return 0;};}
# 121
return 1;};}
# 124
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 126
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp25=({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0U,_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp179;});struct Cyc_String_pa_PrintArg_struct _tmp26=({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0U,_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp178;});void*_tmp23[2U];_tmp23[0]=& _tmp25,_tmp23[1]=& _tmp26;({unsigned int _tmp1E3=loc;struct _dyneither_ptr*_tmp1E2=msg;struct _dyneither_ptr _tmp1E1=({const char*_tmp24="%s %s has different attributes";_tag_dyneither(_tmp24,sizeof(char),31U);});Cyc_Tcdecl_merr(_tmp1E3,_tmp1E2,_tmp1E1,_tag_dyneither(_tmp23,sizeof(void*),2U));});});
return 0;}
# 130
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 133
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 135
struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
inst=({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));({struct _tuple12*_tmp1E5=({struct _tuple12*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs1))->hd,({void*_tmp1E4=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->tag=2U,_tmp27->f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp27;});_tmp28->f2=_tmp1E4;});_tmp28;});_tmp29->hd=_tmp1E5;}),_tmp29->tl=inst;_tmp29;});}
return inst;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple14{void*f1;void*f2;};
# 141
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 143
struct _dyneither_ptr _tmp2A=Cyc_Absynpp_qvar2string(d0->name);
int _tmp2B=1;
# 149
if(!((int)d0->kind == (int)d1->kind))return 0;
# 152
if(!({struct Cyc_List_List*_tmp1EA=d0->tvs;struct Cyc_List_List*_tmp1E9=d1->tvs;struct _dyneither_ptr _tmp1E8=({const char*_tmp2C="type";_tag_dyneither(_tmp2C,sizeof(char),5U);});struct _dyneither_ptr _tmp1E7=_tmp2A;unsigned int _tmp1E6=loc;Cyc_Tcdecl_check_tvs(_tmp1EA,_tmp1E9,_tmp1E8,_tmp1E7,_tmp1E6,msg);}))return 0;{
# 155
struct _tuple10 _tmp2D=({
enum Cyc_Absyn_Scope _tmp1EF=d0->sc;enum Cyc_Absyn_Scope _tmp1EE=d1->sc;struct _dyneither_ptr _tmp1ED=({const char*_tmp83="type";_tag_dyneither(_tmp83,sizeof(char),5U);});struct _dyneither_ptr _tmp1EC=_tmp2A;unsigned int _tmp1EB=loc;Cyc_Tcdecl_merge_scope(_tmp1EF,_tmp1EE,_tmp1ED,_tmp1EC,_tmp1EB,msg,1);});
# 155
struct _tuple10 _tmp2E=_tmp2D;enum Cyc_Absyn_Scope _tmp82;int _tmp81;_LL1: _tmp82=_tmp2E.f1;_tmp81=_tmp2E.f2;_LL2:;
# 157
if(!_tmp81)_tmp2B=0;
# 160
if(!({struct Cyc_List_List*_tmp1F4=d0->attributes;struct Cyc_List_List*_tmp1F3=d1->attributes;struct _dyneither_ptr _tmp1F2=({const char*_tmp2F="type";_tag_dyneither(_tmp2F,sizeof(char),5U);});struct _dyneither_ptr _tmp1F1=_tmp2A;unsigned int _tmp1F0=loc;Cyc_Tcdecl_check_atts(_tmp1F4,_tmp1F3,_tmp1F2,_tmp1F1,_tmp1F0,msg);}))_tmp2B=0;{
# 162
struct Cyc_Absyn_Aggrdecl*d2;
# 164
{struct _tuple13 _tmp30=({struct _tuple13 _tmp194;_tmp194.f1=d0->impl,_tmp194.f2=d1->impl;_tmp194;});struct _tuple13 _tmp31=_tmp30;struct Cyc_List_List*_tmp7F;struct Cyc_List_List*_tmp7E;struct Cyc_List_List*_tmp7D;int _tmp7C;struct Cyc_List_List*_tmp7B;struct Cyc_List_List*_tmp7A;struct Cyc_List_List*_tmp79;int _tmp78;if(_tmp31.f2 == 0){_LL4: _LL5:
 d2=d0;goto _LL3;}else{if(_tmp31.f1 == 0){_LL6: _LL7:
 d2=d1;goto _LL3;}else{_LL8: _tmp7F=(_tmp31.f1)->exist_vars;_tmp7E=(_tmp31.f1)->rgn_po;_tmp7D=(_tmp31.f1)->fields;_tmp7C=(_tmp31.f1)->tagged;_tmp7B=(_tmp31.f2)->exist_vars;_tmp7A=(_tmp31.f2)->rgn_po;_tmp79=(_tmp31.f2)->fields;_tmp78=(_tmp31.f2)->tagged;_LL9:
# 171
 if(!({struct Cyc_List_List*_tmp1F9=_tmp7F;struct Cyc_List_List*_tmp1F8=_tmp7B;struct _dyneither_ptr _tmp1F7=({const char*_tmp32="type";_tag_dyneither(_tmp32,sizeof(char),5U);});struct _dyneither_ptr _tmp1F6=_tmp2A;unsigned int _tmp1F5=loc;Cyc_Tcdecl_check_tvs(_tmp1F9,_tmp1F8,_tmp1F7,_tmp1F6,_tmp1F5,msg);}))
return 0;{
# 175
struct Cyc_List_List*_tmp33=({struct Cyc_List_List*_tmp1FA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp7F);Cyc_Tcdecl_build_tvs_map(_tmp1FA,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp7B));});
# 180
for(0;_tmp7E != 0  && _tmp7A != 0;(
_tmp7E=_tmp7E->tl,_tmp7A=_tmp7A->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp7E->hd)).f1,(*((struct _tuple14*)_tmp7A->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp7E->hd)).f2,(*((struct _tuple14*)_tmp7A->hd)).f2);}
# 187
for(0;_tmp7D != 0  && _tmp79 != 0;(_tmp7D=_tmp7D->tl,_tmp79=_tmp79->tl)){
struct Cyc_Absyn_Aggrfield*_tmp34=(struct Cyc_Absyn_Aggrfield*)_tmp7D->hd;struct Cyc_Absyn_Aggrfield*_tmp35=_tmp34;struct _dyneither_ptr*_tmp6C;struct Cyc_Absyn_Tqual _tmp6B;void*_tmp6A;struct Cyc_Absyn_Exp*_tmp69;struct Cyc_List_List*_tmp68;struct Cyc_Absyn_Exp*_tmp67;_LLB: _tmp6C=_tmp35->name;_tmp6B=_tmp35->tq;_tmp6A=_tmp35->type;_tmp69=_tmp35->width;_tmp68=_tmp35->attributes;_tmp67=_tmp35->requires_clause;_LLC:;{
struct Cyc_Absyn_Aggrfield*_tmp36=(struct Cyc_Absyn_Aggrfield*)_tmp79->hd;struct Cyc_Absyn_Aggrfield*_tmp37=_tmp36;struct _dyneither_ptr*_tmp66;struct Cyc_Absyn_Tqual _tmp65;void*_tmp64;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_List_List*_tmp62;struct Cyc_Absyn_Exp*_tmp61;_LLE: _tmp66=_tmp37->name;_tmp65=_tmp37->tq;_tmp64=_tmp37->type;_tmp63=_tmp37->width;_tmp62=_tmp37->attributes;_tmp61=_tmp37->requires_clause;_LLF:;
# 191
if(Cyc_strptrcmp(_tmp6C,_tmp66)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3A=({struct Cyc_String_pa_PrintArg_struct _tmp17D;_tmp17D.tag=0U,({struct _dyneither_ptr _tmp1FB=(struct _dyneither_ptr)({const char*_tmp3E="type";_tag_dyneither(_tmp3E,sizeof(char),5U);});_tmp17D.f1=_tmp1FB;});_tmp17D;});struct Cyc_String_pa_PrintArg_struct _tmp3B=({struct Cyc_String_pa_PrintArg_struct _tmp17C;_tmp17C.tag=0U,_tmp17C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp17C;});struct Cyc_String_pa_PrintArg_struct _tmp3C=({struct Cyc_String_pa_PrintArg_struct _tmp17B;_tmp17B.tag=0U,_tmp17B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);_tmp17B;});struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp17A;_tmp17A.tag=0U,_tmp17A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp66);_tmp17A;});void*_tmp38[4U];_tmp38[0]=& _tmp3A,_tmp38[1]=& _tmp3B,_tmp38[2]=& _tmp3C,_tmp38[3]=& _tmp3D;({unsigned int _tmp1FE=loc;struct _dyneither_ptr*_tmp1FD=msg;struct _dyneither_ptr _tmp1FC=({const char*_tmp39="%s %s : field name mismatch %s != %s";_tag_dyneither(_tmp39,sizeof(char),37U);});Cyc_Tcdecl_merr(_tmp1FE,_tmp1FD,_tmp1FC,_tag_dyneither(_tmp38,sizeof(void*),4U));});});
# 194
return 0;}
# 196
if(!Cyc_Tcutil_same_atts(_tmp68,_tmp62)){
({struct Cyc_String_pa_PrintArg_struct _tmp41=({struct Cyc_String_pa_PrintArg_struct _tmp180;_tmp180.tag=0U,({struct _dyneither_ptr _tmp1FF=(struct _dyneither_ptr)({const char*_tmp44="type";_tag_dyneither(_tmp44,sizeof(char),5U);});_tmp180.f1=_tmp1FF;});_tmp180;});struct Cyc_String_pa_PrintArg_struct _tmp42=({struct Cyc_String_pa_PrintArg_struct _tmp17F;_tmp17F.tag=0U,_tmp17F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp17F;});struct Cyc_String_pa_PrintArg_struct _tmp43=({struct Cyc_String_pa_PrintArg_struct _tmp17E;_tmp17E.tag=0U,_tmp17E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);_tmp17E;});void*_tmp3F[3U];_tmp3F[0]=& _tmp41,_tmp3F[1]=& _tmp42,_tmp3F[2]=& _tmp43;({unsigned int _tmp202=loc;struct _dyneither_ptr*_tmp201=msg;struct _dyneither_ptr _tmp200=({const char*_tmp40="%s %s : attribute mismatch on field %s";_tag_dyneither(_tmp40,sizeof(char),39U);});Cyc_Tcdecl_merr(_tmp202,_tmp201,_tmp200,_tag_dyneither(_tmp3F,sizeof(void*),3U));});});
_tmp2B=0;}
# 200
if(!Cyc_Tcutil_equal_tqual(_tmp6B,_tmp65)){
({struct Cyc_String_pa_PrintArg_struct _tmp47=({struct Cyc_String_pa_PrintArg_struct _tmp183;_tmp183.tag=0U,({struct _dyneither_ptr _tmp203=(struct _dyneither_ptr)({const char*_tmp4A="type";_tag_dyneither(_tmp4A,sizeof(char),5U);});_tmp183.f1=_tmp203;});_tmp183;});struct Cyc_String_pa_PrintArg_struct _tmp48=({struct Cyc_String_pa_PrintArg_struct _tmp182;_tmp182.tag=0U,_tmp182.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp182;});struct Cyc_String_pa_PrintArg_struct _tmp49=({struct Cyc_String_pa_PrintArg_struct _tmp181;_tmp181.tag=0U,_tmp181.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);_tmp181;});void*_tmp45[3U];_tmp45[0]=& _tmp47,_tmp45[1]=& _tmp48,_tmp45[2]=& _tmp49;({unsigned int _tmp206=loc;struct _dyneither_ptr*_tmp205=msg;struct _dyneither_ptr _tmp204=({const char*_tmp46="%s %s : qualifier mismatch on field %s";_tag_dyneither(_tmp46,sizeof(char),39U);});Cyc_Tcdecl_merr(_tmp206,_tmp205,_tmp204,_tag_dyneither(_tmp45,sizeof(void*),3U));});});
_tmp2B=0;}
# 204
if(((_tmp69 != 0  && _tmp63 != 0) && ({
unsigned int _tmp207=Cyc_Tcdecl_get_uint_const_value(_tmp69);_tmp207 != 
Cyc_Tcdecl_get_uint_const_value(_tmp63);}) || 
_tmp69 == 0  && _tmp63 != 0) || 
_tmp69 != 0  && _tmp63 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp4D=({struct Cyc_String_pa_PrintArg_struct _tmp186;_tmp186.tag=0U,({struct _dyneither_ptr _tmp208=(struct _dyneither_ptr)({const char*_tmp50="type";_tag_dyneither(_tmp50,sizeof(char),5U);});_tmp186.f1=_tmp208;});_tmp186;});struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp185;_tmp185.tag=0U,_tmp185.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp185;});struct Cyc_String_pa_PrintArg_struct _tmp4F=({struct Cyc_String_pa_PrintArg_struct _tmp184;_tmp184.tag=0U,_tmp184.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);_tmp184;});void*_tmp4B[3U];_tmp4B[0]=& _tmp4D,_tmp4B[1]=& _tmp4E,_tmp4B[2]=& _tmp4F;({unsigned int _tmp20B=loc;struct _dyneither_ptr*_tmp20A=msg;struct _dyneither_ptr _tmp209=({const char*_tmp4C="%s %s : bitfield mismatch on field %s";_tag_dyneither(_tmp4C,sizeof(char),38U);});Cyc_Tcdecl_merr(_tmp20B,_tmp20A,_tmp209,_tag_dyneither(_tmp4B,sizeof(void*),3U));});});
_tmp2B=0;}
# 212
if(_tmp67 != 0  && _tmp61 != 0){
void*subst_w1=({struct Cyc_List_List*_tmp20C=_tmp33;Cyc_Tcutil_substitute(_tmp20C,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=9U,_tmp57->f1=_tmp61;_tmp57;}));});
if(!({void*_tmp20D=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->tag=9U,_tmp51->f1=_tmp67;_tmp51;});Cyc_Tcdecl_check_type(_tmp20D,subst_w1);})){
({struct Cyc_String_pa_PrintArg_struct _tmp54=({struct Cyc_String_pa_PrintArg_struct _tmp189;_tmp189.tag=0U,_tmp189.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp189;});struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp188;_tmp188.tag=0U,({
struct _dyneither_ptr _tmp20E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp67));_tmp188.f1=_tmp20E;});_tmp188;});struct Cyc_String_pa_PrintArg_struct _tmp56=({struct Cyc_String_pa_PrintArg_struct _tmp187;_tmp187.tag=0U,({struct _dyneither_ptr _tmp20F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp61));_tmp187.f1=_tmp20F;});_tmp187;});void*_tmp52[3U];_tmp52[0]=& _tmp54,_tmp52[1]=& _tmp55,_tmp52[2]=& _tmp56;({unsigned int _tmp212=loc;struct _dyneither_ptr*_tmp211=msg;struct _dyneither_ptr _tmp210=({const char*_tmp53="type %s : mismatch on @requires clauses: %s != %s";_tag_dyneither(_tmp53,sizeof(char),50U);});Cyc_Tcdecl_merr(_tmp212,_tmp211,_tmp210,_tag_dyneither(_tmp52,sizeof(void*),3U));});});
_tmp2B=0;}}else{
# 219
if(_tmp67 == 0  && _tmp61 != 0  || 
_tmp67 != 0  && _tmp61 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmp18A;_tmp18A.tag=0U,_tmp18A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp18A;});void*_tmp58[1U];_tmp58[0]=& _tmp5A;({unsigned int _tmp215=loc;struct _dyneither_ptr*_tmp214=msg;struct _dyneither_ptr _tmp213=({const char*_tmp59="type %s : mismatch on @requires clauses";_tag_dyneither(_tmp59,sizeof(char),40U);});Cyc_Tcdecl_merr(_tmp215,_tmp214,_tmp213,_tag_dyneither(_tmp58,sizeof(void*),1U));});});
_tmp2B=0;}}{
# 225
void*subst_t1=Cyc_Tcutil_substitute(_tmp33,_tmp64);
if(!Cyc_Tcdecl_check_type(_tmp6A,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp18E;_tmp18E.tag=0U,_tmp18E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp18E;});struct Cyc_String_pa_PrintArg_struct _tmp5E=({struct Cyc_String_pa_PrintArg_struct _tmp18D;_tmp18D.tag=0U,_tmp18D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);_tmp18D;});struct Cyc_String_pa_PrintArg_struct _tmp5F=({struct Cyc_String_pa_PrintArg_struct _tmp18C;_tmp18C.tag=0U,({
struct _dyneither_ptr _tmp216=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp6A));_tmp18C.f1=_tmp216;});_tmp18C;});struct Cyc_String_pa_PrintArg_struct _tmp60=({struct Cyc_String_pa_PrintArg_struct _tmp18B;_tmp18B.tag=0U,({struct _dyneither_ptr _tmp217=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1));_tmp18B.f1=_tmp217;});_tmp18B;});void*_tmp5B[4U];_tmp5B[0]=& _tmp5D,_tmp5B[1]=& _tmp5E,_tmp5B[2]=& _tmp5F,_tmp5B[3]=& _tmp60;({unsigned int _tmp21A=loc;struct _dyneither_ptr*_tmp219=msg;struct _dyneither_ptr _tmp218=({const char*_tmp5C="type %s : type mismatch on field %s: %s != %s";_tag_dyneither(_tmp5C,sizeof(char),46U);});Cyc_Tcdecl_merr(_tmp21A,_tmp219,_tmp218,_tag_dyneither(_tmp5B,sizeof(void*),4U));});});
Cyc_Tcutil_explain_failure();
_tmp2B=0;}};};}
# 234
if(_tmp7D != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp190;_tmp190.tag=0U,_tmp190.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp190;});struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp18F;_tmp18F.tag=0U,_tmp18F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp7D->hd)->name);_tmp18F;});void*_tmp6D[2U];_tmp6D[0]=& _tmp6F,_tmp6D[1]=& _tmp70;({unsigned int _tmp21D=loc;struct _dyneither_ptr*_tmp21C=msg;struct _dyneither_ptr _tmp21B=({const char*_tmp6E="type %s is missing field %s";_tag_dyneither(_tmp6E,sizeof(char),28U);});Cyc_Tcdecl_merr(_tmp21D,_tmp21C,_tmp21B,_tag_dyneither(_tmp6D,sizeof(void*),2U));});});
_tmp2B=0;}
# 238
if(_tmp79 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp73=({struct Cyc_String_pa_PrintArg_struct _tmp192;_tmp192.tag=0U,_tmp192.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp192;});struct Cyc_String_pa_PrintArg_struct _tmp74=({struct Cyc_String_pa_PrintArg_struct _tmp191;_tmp191.tag=0U,_tmp191.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp79->hd)->name);_tmp191;});void*_tmp71[2U];_tmp71[0]=& _tmp73,_tmp71[1]=& _tmp74;({unsigned int _tmp220=loc;struct _dyneither_ptr*_tmp21F=msg;struct _dyneither_ptr _tmp21E=({const char*_tmp72="type %s has extra field %s";_tag_dyneither(_tmp72,sizeof(char),27U);});Cyc_Tcdecl_merr(_tmp220,_tmp21F,_tmp21E,_tag_dyneither(_tmp71,sizeof(void*),2U));});});
_tmp2B=0;}
# 243
if(_tmp7C != _tmp78){
({struct Cyc_String_pa_PrintArg_struct _tmp77=({struct Cyc_String_pa_PrintArg_struct _tmp193;_tmp193.tag=0U,_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);_tmp193;});void*_tmp75[1U];_tmp75[0]=& _tmp77;({unsigned int _tmp223=loc;struct _dyneither_ptr*_tmp222=msg;struct _dyneither_ptr _tmp221=({const char*_tmp76="%s : previous declaration disagrees with @tagged qualifier";_tag_dyneither(_tmp76,sizeof(char),59U);});Cyc_Tcdecl_merr(_tmp223,_tmp222,_tmp221,_tag_dyneither(_tmp75,sizeof(void*),1U));});});
_tmp2B=0;}
# 248
d2=d0;
goto _LL3;};}}_LL3:;}
# 252
if(!_tmp2B)return 0;
# 254
if((int)_tmp82 == (int)d2->sc)
return d2;else{
# 257
d2=({struct Cyc_Absyn_Aggrdecl*_tmp80=_cycalloc(sizeof(*_tmp80));*_tmp80=*d2;_tmp80;});
d2->sc=_tmp82;
return d2;}};};}
# 263
inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){
return is_x?({const char*_tmp84="@extensible datatype";_tag_dyneither(_tmp84,sizeof(char),21U);}):({const char*_tmp85="datatype";_tag_dyneither(_tmp85,sizeof(char),9U);});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 269
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){
# 273
unsigned int loc=f1->loc;
# 276
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp88=({struct Cyc_String_pa_PrintArg_struct _tmp198;_tmp198.tag=0U,_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp198;});struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp197;_tmp197.tag=0U,_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp197;});struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp196;_tmp196.tag=0U,_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2);_tmp196;});struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp195;_tmp195.tag=0U,_tmp195.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2);_tmp195;});void*_tmp86[4U];_tmp86[0]=& _tmp88,_tmp86[1]=& _tmp89,_tmp86[2]=& _tmp8A,_tmp86[3]=& _tmp8B;({unsigned int _tmp226=loc;struct _dyneither_ptr*_tmp225=msg;struct _dyneither_ptr _tmp224=({const char*_tmp87="%s %s: field name mismatch %s != %s";_tag_dyneither(_tmp87,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp226,_tmp225,_tmp224,_tag_dyneither(_tmp86,sizeof(void*),4U));});});
# 279
return 0;}{
# 282
struct _dyneither_ptr _tmp8C=*(*f0->name).f2;
# 285
struct _tuple10 _tmp8D=({
enum Cyc_Absyn_Scope _tmp22C=f0->sc;enum Cyc_Absyn_Scope _tmp22B=f1->sc;struct _dyneither_ptr _tmp22A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA8=({struct Cyc_String_pa_PrintArg_struct _tmp1A5;_tmp1A5.tag=0U,_tmp1A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp1A5;});struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp1A4;_tmp1A4.tag=0U,_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp1A4;});void*_tmpA6[2U];_tmpA6[0]=& _tmpA8,_tmpA6[1]=& _tmpA9;({struct _dyneither_ptr _tmp227=({const char*_tmpA7="in %s %s, field";_tag_dyneither(_tmpA7,sizeof(char),16U);});Cyc_aprintf(_tmp227,_tag_dyneither(_tmpA6,sizeof(void*),2U));});});struct _dyneither_ptr _tmp229=_tmp8C;unsigned int _tmp228=loc;Cyc_Tcdecl_merge_scope(_tmp22C,_tmp22B,_tmp22A,_tmp229,_tmp228,msg,0);});
# 285
struct _tuple10 _tmp8E=_tmp8D;enum Cyc_Absyn_Scope _tmpA5;int _tmpA4;_LL1: _tmpA5=_tmp8E.f1;_tmpA4=_tmp8E.f2;_LL2:;{
# 289
struct Cyc_List_List*_tmp8F=f0->typs;
struct Cyc_List_List*_tmp90=f1->typs;
if(({int _tmp22D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8F);_tmp22D != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp90);})){
({struct Cyc_String_pa_PrintArg_struct _tmp93=({struct Cyc_String_pa_PrintArg_struct _tmp19B;_tmp19B.tag=0U,_tmp19B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp19B;});struct Cyc_String_pa_PrintArg_struct _tmp94=({struct Cyc_String_pa_PrintArg_struct _tmp19A;_tmp19A.tag=0U,_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp19A;});struct Cyc_String_pa_PrintArg_struct _tmp95=({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0U,_tmp199.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp8C);_tmp199;});void*_tmp91[3U];_tmp91[0]=& _tmp93,_tmp91[1]=& _tmp94,_tmp91[2]=& _tmp95;({unsigned int _tmp230=loc;struct _dyneither_ptr*_tmp22F=msg;struct _dyneither_ptr _tmp22E=({const char*_tmp92="%s %s, field %s: parameter number mismatch";_tag_dyneither(_tmp92,sizeof(char),43U);});Cyc_Tcdecl_merr(_tmp230,_tmp22F,_tmp22E,_tag_dyneither(_tmp91,sizeof(void*),3U));});});
_tmpA4=0;}
# 296
for(0;_tmp8F != 0  && _tmp90 != 0;(_tmp8F=_tmp8F->tl,_tmp90=_tmp90->tl)){
# 298
if(!Cyc_Tcutil_equal_tqual((*((struct _tuple15*)_tmp8F->hd)).f1,(*((struct _tuple15*)_tmp90->hd)).f1)){
({struct Cyc_String_pa_PrintArg_struct _tmp98=({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0U,_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp19E;});struct Cyc_String_pa_PrintArg_struct _tmp99=({struct Cyc_String_pa_PrintArg_struct _tmp19D;_tmp19D.tag=0U,_tmp19D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp19D;});struct Cyc_String_pa_PrintArg_struct _tmp9A=({struct Cyc_String_pa_PrintArg_struct _tmp19C;_tmp19C.tag=0U,_tmp19C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp8C);_tmp19C;});void*_tmp96[3U];_tmp96[0]=& _tmp98,_tmp96[1]=& _tmp99,_tmp96[2]=& _tmp9A;({unsigned int _tmp233=loc;struct _dyneither_ptr*_tmp232=msg;struct _dyneither_ptr _tmp231=({const char*_tmp97="%s %s, field %s: parameter qualifier";_tag_dyneither(_tmp97,sizeof(char),37U);});Cyc_Tcdecl_merr(_tmp233,_tmp232,_tmp231,_tag_dyneither(_tmp96,sizeof(void*),3U));});});
_tmpA4=0;}{
# 302
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple15*)_tmp90->hd)).f2);
# 304
if(!Cyc_Tcdecl_check_type((*((struct _tuple15*)_tmp8F->hd)).f2,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmp9D=({struct Cyc_String_pa_PrintArg_struct _tmp1A3;_tmp1A3.tag=0U,_tmp1A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp1A3;});struct Cyc_String_pa_PrintArg_struct _tmp9E=({struct Cyc_String_pa_PrintArg_struct _tmp1A2;_tmp1A2.tag=0U,_tmp1A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp1A2;});struct Cyc_String_pa_PrintArg_struct _tmp9F=({struct Cyc_String_pa_PrintArg_struct _tmp1A1;_tmp1A1.tag=0U,_tmp1A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp8C);_tmp1A1;});struct Cyc_String_pa_PrintArg_struct _tmpA0=({struct Cyc_String_pa_PrintArg_struct _tmp1A0;_tmp1A0.tag=0U,({
struct _dyneither_ptr _tmp234=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((struct _tuple15*)_tmp8F->hd)).f2));_tmp1A0.f1=_tmp234;});_tmp1A0;});struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp19F;_tmp19F.tag=0U,({struct _dyneither_ptr _tmp235=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1));_tmp19F.f1=_tmp235;});_tmp19F;});void*_tmp9B[5U];_tmp9B[0]=& _tmp9D,_tmp9B[1]=& _tmp9E,_tmp9B[2]=& _tmp9F,_tmp9B[3]=& _tmpA0,_tmp9B[4]=& _tmpA1;({unsigned int _tmp238=loc;struct _dyneither_ptr*_tmp237=msg;struct _dyneither_ptr _tmp236=({const char*_tmp9C="%s %s, field %s: parameter type mismatch %s != %s";_tag_dyneither(_tmp9C,sizeof(char),50U);});Cyc_Tcdecl_merr(_tmp238,_tmp237,_tmp236,_tag_dyneither(_tmp9B,sizeof(void*),5U));});});
Cyc_Tcutil_explain_failure();
_tmpA4=0;}};}
# 312
if(!_tmpA4)return 0;
if((int)f0->sc != (int)_tmpA5){
struct Cyc_Absyn_Datatypefield*_tmpA2=({struct Cyc_Absyn_Datatypefield*_tmpA3=_cycalloc(sizeof(*_tmpA3));*_tmpA3=*f0;_tmpA3;});
_tmpA2->sc=_tmpA5;
return _tmpA2;}else{
return f0;}};};}
# 320
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple15*x){
# 323
struct _tuple15*_tmpAA=x;struct Cyc_Absyn_Tqual _tmpAD;void*_tmpAC;_LL1: _tmpAD=_tmpAA->f1;_tmpAC=_tmpAA->f2;_LL2:;
return({struct _tuple15*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=_tmpAD,({void*_tmp239=Cyc_Tcutil_substitute(inst,_tmpAC);_tmpAB->f2=_tmp239;});_tmpAB;});}
# 329
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 331
struct Cyc_Absyn_Datatypefield*_tmpAE=({struct Cyc_Absyn_Datatypefield*_tmpAF=_cycalloc(sizeof(*_tmpAF));*_tmpAF=*f1;_tmpAF;});
({struct Cyc_List_List*_tmp23A=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);_tmpAE->typs=_tmp23A;});
# 334
return _tmpAE;}
# 341
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 350
struct Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpB8=_cycalloc(sizeof(*_tmpB8));*_tmpB8=0;_tmpB8;});
struct Cyc_List_List**_tmpB0=f2sp;
int cmp=-1;
# 354
for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){
while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){
struct Cyc_List_List*_tmpB1=({struct Cyc_List_List*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,_tmpB2->tl=0;_tmpB2;});
*_tmpB0=_tmpB1;
_tmpB0=& _tmpB1->tl;
f0s=f0s->tl;}
# 361
if(f0s == 0  || cmp > 0){
# 363
*incl=0;{
struct Cyc_List_List*_tmpB3=({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct Cyc_Absyn_Datatypefield*_tmp23B=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd);_tmpB4->hd=_tmp23B;}),_tmpB4->tl=0;_tmpB4;});
*_tmpB0=_tmpB3;
_tmpB0=& _tmpB3->tl;};}else{
# 369
struct Cyc_Absyn_Datatypefield*_tmpB5=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
if(_tmpB5 != 0){
if(_tmpB5 != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*_tmpB6=({struct Cyc_List_List*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->hd=_tmpB5,_tmpB7->tl=0;_tmpB7;});
*_tmpB0=_tmpB6;
_tmpB0=& _tmpB6->tl;};}else{
# 376
*res=0;}
# 378
f0s=f0s->tl;}}
# 383
if(f1s != 0){
*incl=0;
*_tmpB0=f1s;}else{
# 387
*_tmpB0=f0s;}
# 390
return*f2sp;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 394
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0)return({struct _tuple16 _tmp1A6;_tmp1A6.f1=0,_tmp1A6.f2=0;_tmp1A6;});
if(f->tl == 0)return({struct _tuple16 _tmp1A7;_tmp1A7.f1=f,_tmp1A7.f2=0;_tmp1A7;});{
struct _tuple16 _tmpB9=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);struct _tuple16 _tmpBA=_tmpB9;struct Cyc_List_List*_tmpBE;struct Cyc_List_List*_tmpBD;_LL1: _tmpBE=_tmpBA.f1;_tmpBD=_tmpBA.f2;_LL2:;
return({struct _tuple16 _tmp1A8;({struct Cyc_List_List*_tmp23D=({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->hd=f->hd,_tmpBB->tl=_tmpBE;_tmpBB;});_tmp1A8.f1=_tmp23D;}),({struct Cyc_List_List*_tmp23C=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->hd=((struct Cyc_List_List*)_check_null(f->tl))->hd,_tmpBC->tl=_tmpBD;_tmpBC;});_tmp1A8.f2=_tmp23C;});_tmp1A8;});};}
# 403
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg){
# 406
struct _tuple16 _tmpBF=((struct _tuple16(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);struct _tuple16 _tmpC0=_tmpBF;struct Cyc_List_List*_tmpC4;struct Cyc_List_List*_tmpC3;_LL1: _tmpC4=_tmpC0.f1;_tmpC3=_tmpC0.f2;_LL2:;
if(_tmpC4 != 0  && _tmpC4->tl != 0)
_tmpC4=Cyc_Tcdecl_sort_xdatatype_fields(_tmpC4,res,v,loc,msg);
if(_tmpC3 != 0  && _tmpC3->tl != 0)
_tmpC3=Cyc_Tcdecl_sort_xdatatype_fields(_tmpC3,res,v,loc,msg);
return({struct Cyc_List_List*_tmp244=_tmpC4;struct Cyc_List_List*_tmp243=_tmpC3;int*_tmp242=res;int*_tmp241=({int*_tmpC1=_cycalloc_atomic(sizeof(*_tmpC1));*_tmpC1=1;_tmpC1;});struct _dyneither_ptr _tmp240=({const char*_tmpC2="@extensible datatype";_tag_dyneither(_tmpC2,sizeof(char),21U);});struct _dyneither_ptr _tmp23F=*v;unsigned int _tmp23E=loc;Cyc_Tcdecl_merge_xdatatype_fields(_tmp244,_tmp243,0,0,0,_tmp242,_tmp241,_tmp240,_tmp23F,_tmp23E,msg);});}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 415
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 417
struct _dyneither_ptr _tmpC5=Cyc_Absynpp_qvar2string(d0->name);
struct _dyneither_ptr t=({const char*_tmpE4="datatype";_tag_dyneither(_tmpE4,sizeof(char),9U);});
int _tmpC6=1;
# 424
if(d0->is_extensible != d1->is_extensible){
({struct Cyc_String_pa_PrintArg_struct _tmpC9=({struct Cyc_String_pa_PrintArg_struct _tmp1AB;_tmp1AB.tag=0U,_tmp1AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC5);_tmp1AB;});struct Cyc_String_pa_PrintArg_struct _tmpCA=({struct Cyc_String_pa_PrintArg_struct _tmp1AA;_tmp1AA.tag=0U,({
struct _dyneither_ptr _tmp245=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible));_tmp1AA.f1=_tmp245;});_tmp1AA;});struct Cyc_String_pa_PrintArg_struct _tmpCB=({struct Cyc_String_pa_PrintArg_struct _tmp1A9;_tmp1A9.tag=0U,({struct _dyneither_ptr _tmp246=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d1->is_extensible));_tmp1A9.f1=_tmp246;});_tmp1A9;});void*_tmpC7[3U];_tmpC7[0]=& _tmpC9,_tmpC7[1]=& _tmpCA,_tmpC7[2]=& _tmpCB;({unsigned int _tmp249=loc;struct _dyneither_ptr*_tmp248=msg;struct _dyneither_ptr _tmp247=({const char*_tmpC8="expected %s to be a %s instead of a %s";_tag_dyneither(_tmpC8,sizeof(char),39U);});Cyc_Tcdecl_merr(_tmp249,_tmp248,_tmp247,_tag_dyneither(_tmpC7,sizeof(void*),3U));});});
_tmpC6=0;}else{
# 429
t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}
# 433
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpC5,loc,msg))return 0;{
# 436
struct _tuple10 _tmpCC=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpC5,loc,msg,0);struct _tuple10 _tmpCD=_tmpCC;enum Cyc_Absyn_Scope _tmpE3;int _tmpE2;_LL1: _tmpE3=_tmpCD.f1;_tmpE2=_tmpCD.f2;_LL2:;
if(!_tmpE2)_tmpC6=0;{
# 439
struct Cyc_Absyn_Datatypedecl*d2;
# 442
{struct _tuple17 _tmpCE=({struct _tuple17 _tmp1B2;_tmp1B2.f1=d0->fields,_tmp1B2.f2=d1->fields;_tmp1B2;});struct _tuple17 _tmpCF=_tmpCE;struct Cyc_List_List*_tmpE0;struct Cyc_List_List*_tmpDF;if(_tmpCF.f2 == 0){_LL4: _LL5:
 d2=d0;goto _LL3;}else{if(_tmpCF.f1 == 0){_LL6: _LL7:
 d2=d1;goto _LL3;}else{_LL8: _tmpE0=(struct Cyc_List_List*)(_tmpCF.f1)->v;_tmpDF=(struct Cyc_List_List*)(_tmpCF.f2)->v;_LL9: {
# 456 "tcdecl.cyc"
struct Cyc_List_List*_tmpD0=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 459
if(d0->is_extensible){
# 463
int _tmpD1=1;
struct Cyc_List_List*_tmpD2=Cyc_Tcdecl_merge_xdatatype_fields(_tmpE0,_tmpDF,_tmpD0,d0->tvs,d1->tvs,& _tmpC6,& _tmpD1,t,_tmpC5,loc,msg);
# 467
if(_tmpD1)
d2=d0;else{
# 470
d2=({struct Cyc_Absyn_Datatypedecl*_tmpD3=_cycalloc(sizeof(*_tmpD3));*_tmpD3=*d0;_tmpD3;});
d2->sc=_tmpE3;
({struct Cyc_Core_Opt*_tmp24A=({struct Cyc_Core_Opt*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->v=_tmpD2;_tmpD4;});d2->fields=_tmp24A;});}}else{
# 476
for(0;_tmpE0 != 0  && _tmpDF != 0;(_tmpE0=_tmpE0->tl,_tmpDF=_tmpDF->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)_tmpE0->hd,(struct Cyc_Absyn_Datatypefield*)_tmpDF->hd,_tmpD0,t,_tmpC5,msg);}
# 479
if(_tmpE0 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpD7=({struct Cyc_String_pa_PrintArg_struct _tmp1AE;_tmp1AE.tag=0U,_tmp1AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp1AE;});struct Cyc_String_pa_PrintArg_struct _tmpD8=({struct Cyc_String_pa_PrintArg_struct _tmp1AD;_tmp1AD.tag=0U,_tmp1AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC5);_tmp1AD;});struct Cyc_String_pa_PrintArg_struct _tmpD9=({struct Cyc_String_pa_PrintArg_struct _tmp1AC;_tmp1AC.tag=0U,_tmp1AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpE0->hd)->name).f2);_tmp1AC;});void*_tmpD5[3U];_tmpD5[0]=& _tmpD7,_tmpD5[1]=& _tmpD8,_tmpD5[2]=& _tmpD9;({unsigned int _tmp24D=loc;struct _dyneither_ptr*_tmp24C=msg;struct _dyneither_ptr _tmp24B=({const char*_tmpD6="%s %s is missing field %s";_tag_dyneither(_tmpD6,sizeof(char),26U);});Cyc_Tcdecl_merr(_tmp24D,_tmp24C,_tmp24B,_tag_dyneither(_tmpD5,sizeof(void*),3U));});});
_tmpC6=0;}
# 483
if(_tmpDF != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpDC=({struct Cyc_String_pa_PrintArg_struct _tmp1B1;_tmp1B1.tag=0U,_tmp1B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp1B1;});struct Cyc_String_pa_PrintArg_struct _tmpDD=({struct Cyc_String_pa_PrintArg_struct _tmp1B0;_tmp1B0.tag=0U,_tmp1B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC5);_tmp1B0;});struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp1AF;_tmp1AF.tag=0U,_tmp1AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpDF->hd)->name).f2);_tmp1AF;});void*_tmpDA[3U];_tmpDA[0]=& _tmpDC,_tmpDA[1]=& _tmpDD,_tmpDA[2]=& _tmpDE;({unsigned int _tmp250=loc;struct _dyneither_ptr*_tmp24F=msg;struct _dyneither_ptr _tmp24E=({const char*_tmpDB="%s %s has extra field %s";_tag_dyneither(_tmpDB,sizeof(char),25U);});Cyc_Tcdecl_merr(_tmp250,_tmp24F,_tmp24E,_tag_dyneither(_tmpDA,sizeof(void*),3U));});});
_tmpC6=0;}
# 487
d2=d0;}
# 489
goto _LL3;}}}_LL3:;}
# 492
if(!_tmpC6)return 0;
# 494
if((int)_tmpE3 == (int)d2->sc)
return d2;else{
# 497
d2=({struct Cyc_Absyn_Datatypedecl*_tmpE1=_cycalloc(sizeof(*_tmpE1));*_tmpE1=*d2;_tmpE1;});
d2->sc=_tmpE3;
return d2;}};};}
# 503
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 505
struct _dyneither_ptr _tmpE5=Cyc_Absynpp_qvar2string(d0->name);
int _tmpE6=1;
# 509
struct _tuple10 _tmpE7=({enum Cyc_Absyn_Scope _tmp255=d0->sc;enum Cyc_Absyn_Scope _tmp254=d1->sc;struct _dyneither_ptr _tmp253=({const char*_tmp103="enum";_tag_dyneither(_tmp103,sizeof(char),5U);});struct _dyneither_ptr _tmp252=_tmpE5;unsigned int _tmp251=loc;Cyc_Tcdecl_merge_scope(_tmp255,_tmp254,_tmp253,_tmp252,_tmp251,msg,1);});struct _tuple10 _tmpE8=_tmpE7;enum Cyc_Absyn_Scope _tmp102;int _tmp101;_LL1: _tmp102=_tmpE8.f1;_tmp101=_tmpE8.f2;_LL2:;
if(!_tmp101)_tmpE6=0;{
# 512
struct Cyc_Absyn_Enumdecl*d2;
# 515
{struct _tuple17 _tmpE9=({struct _tuple17 _tmp1B8;_tmp1B8.f1=d0->fields,_tmp1B8.f2=d1->fields;_tmp1B8;});struct _tuple17 _tmpEA=_tmpE9;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmpFE;if(_tmpEA.f2 == 0){_LL4: _LL5:
 d2=d0;goto _LL3;}else{if(_tmpEA.f1 == 0){_LL6: _LL7:
 d2=d1;goto _LL3;}else{_LL8: _tmpFF=(struct Cyc_List_List*)(_tmpEA.f1)->v;_tmpFE=(struct Cyc_List_List*)(_tmpEA.f2)->v;_LL9:
# 520
 for(0;_tmpFF != 0  && _tmpFE != 0;(_tmpFF=_tmpFF->tl,_tmpFE=_tmpFE->tl)){
struct Cyc_Absyn_Enumfield*_tmpEB=(struct Cyc_Absyn_Enumfield*)_tmpFF->hd;struct Cyc_Absyn_Enumfield*_tmpEC=_tmpEB;struct _dyneither_ptr*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;unsigned int _tmpFB;_LLB: _tmpFD=(_tmpEC->name)->f2;_tmpFC=_tmpEC->tag;_tmpFB=_tmpEC->loc;_LLC:;{
struct Cyc_Absyn_Enumfield*_tmpED=(struct Cyc_Absyn_Enumfield*)_tmpFE->hd;struct Cyc_Absyn_Enumfield*_tmpEE=_tmpED;struct _dyneither_ptr*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;unsigned int _tmpF8;_LLE: _tmpFA=(_tmpEE->name)->f2;_tmpF9=_tmpEE->tag;_tmpF8=_tmpEE->loc;_LLF:;
# 525
if(Cyc_strptrcmp(_tmpFD,_tmpFA)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF1=({struct Cyc_String_pa_PrintArg_struct _tmp1B5;_tmp1B5.tag=0U,_tmp1B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE5);_tmp1B5;});struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp1B4;_tmp1B4.tag=0U,_tmp1B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFD);_tmp1B4;});struct Cyc_String_pa_PrintArg_struct _tmpF3=({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0U,_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFA);_tmp1B3;});void*_tmpEF[3U];_tmpEF[0]=& _tmpF1,_tmpEF[1]=& _tmpF2,_tmpEF[2]=& _tmpF3;({unsigned int _tmp258=loc;struct _dyneither_ptr*_tmp257=msg;struct _dyneither_ptr _tmp256=({const char*_tmpF0="enum %s: field name mismatch %s != %s";_tag_dyneither(_tmpF0,sizeof(char),38U);});Cyc_Tcdecl_merr(_tmp258,_tmp257,_tmp256,_tag_dyneither(_tmpEF,sizeof(void*),3U));});});
_tmpE6=0;}
# 530
if(({unsigned int _tmp259=Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpFC));_tmp259 != Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpF9));})){
({struct Cyc_String_pa_PrintArg_struct _tmpF6=({struct Cyc_String_pa_PrintArg_struct _tmp1B7;_tmp1B7.tag=0U,_tmp1B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE5);_tmp1B7;});struct Cyc_String_pa_PrintArg_struct _tmpF7=({struct Cyc_String_pa_PrintArg_struct _tmp1B6;_tmp1B6.tag=0U,_tmp1B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFA);_tmp1B6;});void*_tmpF4[2U];_tmpF4[0]=& _tmpF6,_tmpF4[1]=& _tmpF7;({unsigned int _tmp25C=loc;struct _dyneither_ptr*_tmp25B=msg;struct _dyneither_ptr _tmp25A=({const char*_tmpF5="enum %s, field %s, value mismatch";_tag_dyneither(_tmpF5,sizeof(char),34U);});Cyc_Tcdecl_merr(_tmp25C,_tmp25B,_tmp25A,_tag_dyneither(_tmpF4,sizeof(void*),2U));});});
_tmpE6=0;}};}
# 535
d2=d0;
goto _LL3;}}_LL3:;}
# 539
if(!_tmpE6)return 0;
# 541
if((int)d2->sc == (int)_tmp102)
return d2;else{
# 544
d2=({struct Cyc_Absyn_Enumdecl*_tmp100=_cycalloc(sizeof(*_tmp100));*_tmp100=*d2;_tmp100;});
d2->sc=_tmp102;
return d2;}};}
# 550
static struct _tuple10 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 554
int _tmp104=1;
# 557
struct _tuple10 _tmp105=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);struct _tuple10 _tmp106=_tmp105;enum Cyc_Absyn_Scope _tmp12B;int _tmp12A;_LL1: _tmp12B=_tmp106.f1;_tmp12A=_tmp106.f2;_LL2:;
if(!_tmp12A)_tmp104=0;
# 561
if(!Cyc_Tcdecl_check_type(t0,t1)){
struct _dyneither_ptr _tmp107=Cyc_Absynpp_typ2string(t0);
struct _dyneither_ptr _tmp108=Cyc_Absynpp_typ2string(t1);
if(({unsigned long _tmp260=({unsigned long _tmp25F=({unsigned long _tmp25E=({unsigned long _tmp25D=Cyc_strlen((struct _dyneither_ptr)_tmp107);_tmp25D + Cyc_strlen((struct _dyneither_ptr)_tmp108);});_tmp25E + Cyc_strlen((struct _dyneither_ptr)t);});_tmp25F + Cyc_strlen((struct _dyneither_ptr)v);});_tmp260 + 
Cyc_strlen(({const char*_tmp109="  was declared  instead of ";_tag_dyneither(_tmp109,sizeof(char),28U);}));})> (unsigned long)70)
({struct Cyc_String_pa_PrintArg_struct _tmp10C=({struct Cyc_String_pa_PrintArg_struct _tmp1BC;_tmp1BC.tag=0U,_tmp1BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp1BC;});struct Cyc_String_pa_PrintArg_struct _tmp10D=({struct Cyc_String_pa_PrintArg_struct _tmp1BB;_tmp1BB.tag=0U,_tmp1BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp1BB;});struct Cyc_String_pa_PrintArg_struct _tmp10E=({struct Cyc_String_pa_PrintArg_struct _tmp1BA;_tmp1BA.tag=0U,_tmp1BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp108);_tmp1BA;});struct Cyc_String_pa_PrintArg_struct _tmp10F=({struct Cyc_String_pa_PrintArg_struct _tmp1B9;_tmp1B9.tag=0U,_tmp1B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107);_tmp1B9;});void*_tmp10A[4U];_tmp10A[0]=& _tmp10C,_tmp10A[1]=& _tmp10D,_tmp10A[2]=& _tmp10E,_tmp10A[3]=& _tmp10F;({unsigned int _tmp263=loc;struct _dyneither_ptr*_tmp262=msg;struct _dyneither_ptr _tmp261=({const char*_tmp10B="%s %s was declared \n\t%s\n instead of \n\t%s";_tag_dyneither(_tmp10B,sizeof(char),41U);});Cyc_Tcdecl_merr(_tmp263,_tmp262,_tmp261,_tag_dyneither(_tmp10A,sizeof(void*),4U));});});else{
# 569
({struct Cyc_String_pa_PrintArg_struct _tmp112=({struct Cyc_String_pa_PrintArg_struct _tmp1C0;_tmp1C0.tag=0U,_tmp1C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp1C0;});struct Cyc_String_pa_PrintArg_struct _tmp113=({struct Cyc_String_pa_PrintArg_struct _tmp1BF;_tmp1BF.tag=0U,_tmp1BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp1BF;});struct Cyc_String_pa_PrintArg_struct _tmp114=({struct Cyc_String_pa_PrintArg_struct _tmp1BE;_tmp1BE.tag=0U,_tmp1BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp108);_tmp1BE;});struct Cyc_String_pa_PrintArg_struct _tmp115=({struct Cyc_String_pa_PrintArg_struct _tmp1BD;_tmp1BD.tag=0U,_tmp1BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107);_tmp1BD;});void*_tmp110[4U];_tmp110[0]=& _tmp112,_tmp110[1]=& _tmp113,_tmp110[2]=& _tmp114,_tmp110[3]=& _tmp115;({unsigned int _tmp266=loc;struct _dyneither_ptr*_tmp265=msg;struct _dyneither_ptr _tmp264=({const char*_tmp111="%s %s was declared %s instead of %s";_tag_dyneither(_tmp111,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp266,_tmp265,_tmp264,_tag_dyneither(_tmp110,sizeof(void*),4U));});});}
# 571
Cyc_Tcutil_explain_failure();
_tmp104=0;}
# 576
if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){
({struct Cyc_String_pa_PrintArg_struct _tmp118=({struct Cyc_String_pa_PrintArg_struct _tmp1C2;_tmp1C2.tag=0U,_tmp1C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp1C2;});struct Cyc_String_pa_PrintArg_struct _tmp119=({struct Cyc_String_pa_PrintArg_struct _tmp1C1;_tmp1C1.tag=0U,_tmp1C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp1C1;});void*_tmp116[2U];_tmp116[0]=& _tmp118,_tmp116[1]=& _tmp119;({unsigned int _tmp269=loc;struct _dyneither_ptr*_tmp268=msg;struct _dyneither_ptr _tmp267=({const char*_tmp117="%s %s has different type qualifiers";_tag_dyneither(_tmp117,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp269,_tmp268,_tmp267,_tag_dyneither(_tmp116,sizeof(void*),2U));});});
_tmp104=0;}
# 582
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp11C=({struct Cyc_String_pa_PrintArg_struct _tmp1C4;_tmp1C4.tag=0U,_tmp1C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp1C4;});struct Cyc_String_pa_PrintArg_struct _tmp11D=({struct Cyc_String_pa_PrintArg_struct _tmp1C3;_tmp1C3.tag=0U,_tmp1C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);_tmp1C3;});void*_tmp11A[2U];_tmp11A[0]=& _tmp11C,_tmp11A[1]=& _tmp11D;({unsigned int _tmp26C=loc;struct _dyneither_ptr*_tmp26B=msg;struct _dyneither_ptr _tmp26A=({const char*_tmp11B="%s %s has different attributes";_tag_dyneither(_tmp11B,sizeof(char),31U);});Cyc_Tcdecl_merr(_tmp26C,_tmp26B,_tmp26A,_tag_dyneither(_tmp11A,sizeof(void*),2U));});});
({void*_tmp11E=0U;({struct Cyc___cycFILE*_tmp26E=Cyc_stderr;struct _dyneither_ptr _tmp26D=({const char*_tmp11F="\tprevious attributes: ";_tag_dyneither(_tmp11F,sizeof(char),23U);});Cyc_fprintf(_tmp26E,_tmp26D,_tag_dyneither(_tmp11E,sizeof(void*),0U));});});
for(0;atts0 != 0;atts0=atts0->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp122=({struct Cyc_String_pa_PrintArg_struct _tmp1C5;_tmp1C5.tag=0U,({struct _dyneither_ptr _tmp26F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts0->hd));_tmp1C5.f1=_tmp26F;});_tmp1C5;});void*_tmp120[1U];_tmp120[0]=& _tmp122;({struct Cyc___cycFILE*_tmp271=Cyc_stderr;struct _dyneither_ptr _tmp270=({const char*_tmp121="%s ";_tag_dyneither(_tmp121,sizeof(char),4U);});Cyc_fprintf(_tmp271,_tmp270,_tag_dyneither(_tmp120,sizeof(void*),1U));});});}
({void*_tmp123=0U;({struct Cyc___cycFILE*_tmp273=Cyc_stderr;struct _dyneither_ptr _tmp272=({const char*_tmp124="\n\tcurrent attributes: ";_tag_dyneither(_tmp124,sizeof(char),23U);});Cyc_fprintf(_tmp273,_tmp272,_tag_dyneither(_tmp123,sizeof(void*),0U));});});
for(0;atts1 != 0;atts1=atts1->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp127=({struct Cyc_String_pa_PrintArg_struct _tmp1C6;_tmp1C6.tag=0U,({struct _dyneither_ptr _tmp274=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd));_tmp1C6.f1=_tmp274;});_tmp1C6;});void*_tmp125[1U];_tmp125[0]=& _tmp127;({struct Cyc___cycFILE*_tmp276=Cyc_stderr;struct _dyneither_ptr _tmp275=({const char*_tmp126="%s ";_tag_dyneither(_tmp126,sizeof(char),4U);});Cyc_fprintf(_tmp276,_tmp275,_tag_dyneither(_tmp125,sizeof(void*),1U));});});}
({void*_tmp128=0U;({struct Cyc___cycFILE*_tmp278=Cyc_stderr;struct _dyneither_ptr _tmp277=({const char*_tmp129="\n";_tag_dyneither(_tmp129,sizeof(char),2U);});Cyc_fprintf(_tmp278,_tmp277,_tag_dyneither(_tmp128,sizeof(void*),0U));});});
_tmp104=0;}
# 594
return({struct _tuple10 _tmp1C7;_tmp1C7.f1=_tmp12B,_tmp1C7.f2=_tmp104;_tmp1C7;});}
# 597
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 599
struct _dyneither_ptr _tmp12C=Cyc_Absynpp_qvar2string(d0->name);
# 602
struct _tuple10 _tmp12D=({
enum Cyc_Absyn_Scope _tmp283=d0->sc;void*_tmp282=d0->type;struct Cyc_Absyn_Tqual _tmp281=d0->tq;struct Cyc_List_List*_tmp280=d0->attributes;enum Cyc_Absyn_Scope _tmp27F=d1->sc;void*_tmp27E=d1->type;struct Cyc_Absyn_Tqual _tmp27D=d1->tq;struct Cyc_List_List*_tmp27C=d1->attributes;struct _dyneither_ptr _tmp27B=({const char*_tmp133="variable";_tag_dyneither(_tmp133,sizeof(char),9U);});struct _dyneither_ptr _tmp27A=_tmp12C;unsigned int _tmp279=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp283,_tmp282,_tmp281,_tmp280,_tmp27F,_tmp27E,_tmp27D,_tmp27C,_tmp27B,_tmp27A,_tmp279,msg);});
# 602
struct _tuple10 _tmp12E=_tmp12D;enum Cyc_Absyn_Scope _tmp132;int _tmp131;_LL1: _tmp132=_tmp12E.f1;_tmp131=_tmp12E.f2;_LL2:;
# 606
if(!_tmp131)return 0;
# 608
if((int)d0->sc == (int)_tmp132)
return d0;else{
# 611
struct Cyc_Absyn_Vardecl*_tmp12F=({struct Cyc_Absyn_Vardecl*_tmp130=_cycalloc(sizeof(*_tmp130));*_tmp130=*d0;_tmp130;});
_tmp12F->sc=_tmp132;
return _tmp12F;}}
# 617
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 619
struct _dyneither_ptr _tmp134=Cyc_Absynpp_qvar2string(d0->name);
# 622
if(!({struct Cyc_List_List*_tmp288=d0->tvs;struct Cyc_List_List*_tmp287=d1->tvs;struct _dyneither_ptr _tmp286=({const char*_tmp135="typedef";_tag_dyneither(_tmp135,sizeof(char),8U);});struct _dyneither_ptr _tmp285=_tmp134;unsigned int _tmp284=loc;Cyc_Tcdecl_check_tvs(_tmp288,_tmp287,_tmp286,_tmp285,_tmp284,msg);}))return 0;{
# 625
struct Cyc_List_List*_tmp136=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 628
if(d0->defn != 0  && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(_tmp136,(void*)_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
({struct Cyc_String_pa_PrintArg_struct _tmp139=({struct Cyc_String_pa_PrintArg_struct _tmp1CA;_tmp1CA.tag=0U,_tmp1CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp134);_tmp1CA;});struct Cyc_String_pa_PrintArg_struct _tmp13A=({struct Cyc_String_pa_PrintArg_struct _tmp1C9;_tmp1C9.tag=0U,({
struct _dyneither_ptr _tmp289=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1));_tmp1C9.f1=_tmp289;});_tmp1C9;});struct Cyc_String_pa_PrintArg_struct _tmp13B=({struct Cyc_String_pa_PrintArg_struct _tmp1C8;_tmp1C8.tag=0U,({struct _dyneither_ptr _tmp28A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(d0->defn)));_tmp1C8.f1=_tmp28A;});_tmp1C8;});void*_tmp137[3U];_tmp137[0]=& _tmp139,_tmp137[1]=& _tmp13A,_tmp137[2]=& _tmp13B;({unsigned int _tmp28D=loc;struct _dyneither_ptr*_tmp28C=msg;struct _dyneither_ptr _tmp28B=({const char*_tmp138="typedef %s does not refer to the same type: %s != %s";_tag_dyneither(_tmp138,sizeof(char),53U);});Cyc_Tcdecl_merr(_tmp28D,_tmp28C,_tmp28B,_tag_dyneither(_tmp137,sizeof(void*),3U));});});
return 0;}}
# 636
return d0;};}
# 639
void**Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned int loc,struct _dyneither_ptr*msg){
struct _tuple14 _tmp13C=({struct _tuple14 _tmp1CC;_tmp1CC.f1=b0,_tmp1CC.f2=b1;_tmp1CC;});struct _tuple14 _tmp13D=_tmp13C;struct Cyc_Absyn_Fndecl*_tmp156;struct Cyc_Absyn_Vardecl*_tmp155;struct Cyc_Absyn_Fndecl*_tmp154;struct Cyc_Absyn_Vardecl*_tmp153;struct Cyc_Absyn_Fndecl*_tmp152;struct Cyc_Absyn_Vardecl*_tmp151;struct Cyc_Absyn_Vardecl*_tmp150;switch(*((int*)_tmp13D.f1)){case 1U: switch(*((int*)_tmp13D.f2)){case 1U: _LL1: _tmp151=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13D.f1)->f1;_tmp150=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13D.f2)->f1;_LL2: {
# 642
struct Cyc_Absyn_Vardecl*_tmp13E=Cyc_Tcdecl_merge_vardecl(_tmp151,_tmp150,loc,msg);
if(_tmp13E == 0)return 0;
if(_tmp13E == _tmp151)return({void**_tmp13F=_cycalloc(sizeof(*_tmp13F));*_tmp13F=b0;_tmp13F;});
if(_tmp13E == _tmp150)return({void**_tmp140=_cycalloc(sizeof(*_tmp140));*_tmp140=b1;_tmp140;});
return({void**_tmp142=_cycalloc(sizeof(*_tmp142));({void*_tmp28E=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->tag=1U,_tmp141->f1=_tmp13E;_tmp141;});*_tmp142=_tmp28E;});_tmp142;});}case 2U: _LL3: _tmp153=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13D.f1)->f1;_tmp152=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13D.f2)->f1;_LL4: {
# 648
struct _tuple10 _tmp143=({enum Cyc_Absyn_Scope _tmp299=_tmp153->sc;void*_tmp298=_tmp153->type;struct Cyc_Absyn_Tqual _tmp297=_tmp153->tq;struct Cyc_List_List*_tmp296=_tmp153->attributes;enum Cyc_Absyn_Scope _tmp295=_tmp152->sc;void*_tmp294=(void*)_check_null(_tmp152->cached_type);struct Cyc_Absyn_Tqual _tmp293=
Cyc_Absyn_empty_tqual(0U);
# 648
struct Cyc_List_List*_tmp292=_tmp152->attributes;struct _dyneither_ptr _tmp291=({const char*_tmp147="function";_tag_dyneither(_tmp147,sizeof(char),9U);});struct _dyneither_ptr _tmp290=
# 651
Cyc_Absynpp_qvar2string(_tmp153->name);
# 648
unsigned int _tmp28F=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp299,_tmp298,_tmp297,_tmp296,_tmp295,_tmp294,_tmp293,_tmp292,_tmp291,_tmp290,_tmp28F,msg);});struct _tuple10 _tmp144=_tmp143;int _tmp146;_LLC: _tmp146=_tmp144.f2;_LLD:;
# 652
return _tmp146?({void**_tmp145=_cycalloc(sizeof(*_tmp145));*_tmp145=b1;_tmp145;}): 0;}default: goto _LL9;}case 2U: switch(*((int*)_tmp13D.f2)){case 2U: _LL5: _tmp154=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13D.f2)->f1;_LL6:
# 654
({struct Cyc_String_pa_PrintArg_struct _tmp14A=({struct Cyc_String_pa_PrintArg_struct _tmp1CB;_tmp1CB.tag=0U,({struct _dyneither_ptr _tmp29A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp154->name));_tmp1CB.f1=_tmp29A;});_tmp1CB;});void*_tmp148[1U];_tmp148[0]=& _tmp14A;({unsigned int _tmp29D=loc;struct _dyneither_ptr*_tmp29C=msg;struct _dyneither_ptr _tmp29B=({const char*_tmp149="redefinition of function %s";_tag_dyneither(_tmp149,sizeof(char),28U);});Cyc_Tcdecl_merr(_tmp29D,_tmp29C,_tmp29B,_tag_dyneither(_tmp148,sizeof(void*),1U));});});
return 0;case 1U: _LL7: _tmp156=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13D.f1)->f1;_tmp155=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13D.f2)->f1;_LL8: {
# 657
struct _tuple10 _tmp14B=({enum Cyc_Absyn_Scope _tmp2A8=_tmp156->sc;void*_tmp2A7=(void*)_check_null(_tmp156->cached_type);struct Cyc_Absyn_Tqual _tmp2A6=Cyc_Absyn_empty_tqual(0U);struct Cyc_List_List*_tmp2A5=_tmp156->attributes;enum Cyc_Absyn_Scope _tmp2A4=_tmp155->sc;void*_tmp2A3=_tmp155->type;struct Cyc_Absyn_Tqual _tmp2A2=_tmp155->tq;struct Cyc_List_List*_tmp2A1=_tmp155->attributes;struct _dyneither_ptr _tmp2A0=({const char*_tmp14F="variable";_tag_dyneither(_tmp14F,sizeof(char),9U);});struct _dyneither_ptr _tmp29F=
# 660
Cyc_Absynpp_qvar2string(_tmp156->name);
# 657
unsigned int _tmp29E=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp2A8,_tmp2A7,_tmp2A6,_tmp2A5,_tmp2A4,_tmp2A3,_tmp2A2,_tmp2A1,_tmp2A0,_tmp29F,_tmp29E,msg);});struct _tuple10 _tmp14C=_tmp14B;int _tmp14E;_LLF: _tmp14E=_tmp14C.f2;_LL10:;
# 661
return _tmp14E?({void**_tmp14D=_cycalloc(sizeof(*_tmp14D));*_tmp14D=b0;_tmp14D;}): 0;}default: goto _LL9;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 666
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 669
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp157=d0;struct Cyc_Absyn_Datatypedecl*_tmp167;struct Cyc_Absyn_Datatypefield*_tmp166;_LL1: _tmp167=_tmp157->base;_tmp166=_tmp157->field;_LL2:;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp158=d1;struct Cyc_Absyn_Datatypedecl*_tmp165;struct Cyc_Absyn_Datatypefield*_tmp164;_LL4: _tmp165=_tmp158->base;_tmp164=_tmp158->field;_LL5:;{
struct _dyneither_ptr _tmp159=Cyc_Absynpp_qvar2string(_tmp166->name);
# 674
if(Cyc_Absyn_qvar_cmp(_tmp167->name,_tmp165->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp1CF;_tmp1CF.tag=0U,({
struct _dyneither_ptr _tmp2A9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp167->name));_tmp1CF.f1=_tmp2A9;});_tmp1CF;});struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp1CE;_tmp1CE.tag=0U,({struct _dyneither_ptr _tmp2AA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp165->name));_tmp1CE.f1=_tmp2AA;});_tmp1CE;});struct Cyc_String_pa_PrintArg_struct _tmp15E=({struct Cyc_String_pa_PrintArg_struct _tmp1CD;_tmp1CD.tag=0U,_tmp1CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp159);_tmp1CD;});void*_tmp15A[3U];_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D,_tmp15A[2]=& _tmp15E;({unsigned int _tmp2AD=loc;struct _dyneither_ptr*_tmp2AC=msg;struct _dyneither_ptr _tmp2AB=({const char*_tmp15B="@extensible datatype %s and %s have both a field named %s";_tag_dyneither(_tmp15B,sizeof(char),58U);});Cyc_Tcdecl_merr(_tmp2AD,_tmp2AC,_tmp2AB,_tag_dyneither(_tmp15A,sizeof(void*),3U));});});
return 0;}
# 681
if(!({struct Cyc_List_List*_tmp2B2=_tmp167->tvs;struct Cyc_List_List*_tmp2B1=_tmp165->tvs;struct _dyneither_ptr _tmp2B0=({const char*_tmp15F="@extensible datatype";_tag_dyneither(_tmp15F,sizeof(char),21U);});struct _dyneither_ptr _tmp2AF=Cyc_Absynpp_qvar2string(_tmp167->name);unsigned int _tmp2AE=loc;Cyc_Tcdecl_check_tvs(_tmp2B2,_tmp2B1,_tmp2B0,_tmp2AF,_tmp2AE,msg);}))
return 0;{
# 684
struct Cyc_List_List*_tmp160=Cyc_Tcdecl_build_tvs_map(_tmp167->tvs,_tmp165->tvs);
# 687
struct Cyc_Absyn_Datatypefield*_tmp161=({struct Cyc_Absyn_Datatypefield*_tmp2B7=_tmp166;struct Cyc_Absyn_Datatypefield*_tmp2B6=_tmp164;struct Cyc_List_List*_tmp2B5=_tmp160;struct _dyneither_ptr _tmp2B4=({const char*_tmp163="@extensible datatype field";_tag_dyneither(_tmp163,sizeof(char),27U);});struct _dyneither_ptr _tmp2B3=_tmp159;Cyc_Tcdecl_merge_datatypefield(_tmp2B7,_tmp2B6,_tmp2B5,_tmp2B4,_tmp2B3,msg);});
if(_tmp161 == 0)return 0;
if(_tmp161 == _tmp166)return d0;
return({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->base=_tmp167,_tmp162->field=_tmp161;_tmp162;});};};};}

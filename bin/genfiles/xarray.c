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
extern struct _RegionHandle*Cyc_Core_unique_region;
# 175
void Cyc_Core_ufree(void*ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 40 "xarray.h"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 45
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 51
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 54
struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty();
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 60
struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int,void*);
# 63
struct Cyc_Xarray_Xarray*Cyc_Xarray_rsingleton(struct _RegionHandle*,int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 72
struct _dyneither_ptr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*);
# 75
struct _dyneither_ptr Cyc_Xarray_rto_array(struct _RegionHandle*,struct Cyc_Xarray_Xarray*);
# 78
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _dyneither_ptr arr);
# 81
struct Cyc_Xarray_Xarray*Cyc_Xarray_rfrom_array(struct _RegionHandle*,struct _dyneither_ptr arr);
# 85
struct Cyc_Xarray_Xarray*Cyc_Xarray_append(struct Cyc_Xarray_Xarray*,struct Cyc_Xarray_Xarray*);
# 89
struct Cyc_Xarray_Xarray*Cyc_Xarray_rappend(struct _RegionHandle*,struct Cyc_Xarray_Xarray*,struct Cyc_Xarray_Xarray*);
# 93
void Cyc_Xarray_app(void*(*f)(void*),struct Cyc_Xarray_Xarray*);
# 97
void Cyc_Xarray_app_c(void*(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 100
void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*);
# 104
void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 107
struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*);
# 110
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_Xarray_Xarray*);
# 114
struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 117
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);
# 124
void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*xarr,int num);
# 126
void Cyc_Xarray_remove(struct Cyc_Xarray_Xarray*xarr,int i);
# 25 "xarray.cyc"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*xarr){
return xarr->num_elmts;}
# 29
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*xarr,int i){
if(i < 0  || i >= xarr->num_elmts)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp30=({const char*_tmp0="Xarray::get: bad index";_tag_dyneither(_tmp0,sizeof(char),23U);});_tmp1->f1=_tmp30;});_tmp1;}));
return*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i));}
# 35
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*xarr,int i,void*a){
if(i < 0  || i >= xarr->num_elmts)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp31=({const char*_tmp2="Xarray::set: bad index";_tag_dyneither(_tmp2,sizeof(char),23U);});_tmp3->f1=_tmp31;});_tmp3;}));
*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i))=a;}
# 41
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*r){
struct _dyneither_ptr x=_tag_dyneither(0,0,0);
return({struct Cyc_Xarray_Xarray*_tmp4=_region_malloc(r,sizeof(*_tmp4));_tmp4->elmts=x,_tmp4->num_elmts=0;_tmp4;});}
# 45
struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty(){return Cyc_Xarray_rcreate_empty(Cyc_Core_heap_region);}
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*r,int len,void*a){
if(len < 0)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp32=({const char*_tmp5="xarrays must have a non-negative size buffer";_tag_dyneither(_tmp5,sizeof(char),45U);});_tmp6->f1=_tmp32;});_tmp6;}));
return({struct Cyc_Xarray_Xarray*_tmp9=_region_malloc(r,sizeof(*_tmp9));({struct _dyneither_ptr _tmp34=({unsigned int _tmp8=(unsigned int)len;void**_tmp7=({struct _RegionHandle*_tmp33=Cyc_Core_unique_region;_region_malloc(_tmp33,_check_times(_tmp8,sizeof(void*)));});({{unsigned int _tmp28=(unsigned int)len;unsigned int i;for(i=0;i < _tmp28;++ i){_tmp7[i]=a;}}0;});_tag_dyneither(_tmp7,sizeof(void*),_tmp8);});_tmp9->elmts=_tmp34;}),_tmp9->num_elmts=0;_tmp9;});}
# 54
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int len,void*a){
return Cyc_Xarray_rcreate(Cyc_Core_heap_region,len,a);}
# 58
struct Cyc_Xarray_Xarray*Cyc_Xarray_rsingleton(struct _RegionHandle*r,int len,void*a){
if(len < 1)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp35=({const char*_tmpA="singleton xarray must have size >=1";_tag_dyneither(_tmpA,sizeof(char),36U);});_tmpB->f1=_tmp35;});_tmpB;}));{
struct Cyc_Xarray_Xarray*x=Cyc_Xarray_rcreate(r,len,a);
x->num_elmts=1;
return x;};}
# 65
struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int len,void*a){
return Cyc_Xarray_rsingleton(Cyc_Core_heap_region,len,a);}
# 69
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*xarr,void*a){
if((unsigned int)xarr->num_elmts == _get_dyneither_size(xarr->elmts,sizeof(void*))){
if(xarr->num_elmts == 0)
({struct _dyneither_ptr _tmp37=_tag_dyneither(({unsigned int _tmpD=10U;void**_tmpC=({struct _RegionHandle*_tmp36=Cyc_Core_unique_region;_region_malloc(_tmp36,_check_times(_tmpD,sizeof(void*)));});({{unsigned int _tmp29=10U;unsigned int i;for(i=0;i < _tmp29;++ i){_tmpC[i]=a;}}0;});_tmpC;}),sizeof(void*),10U);xarr->elmts=_tmp37;});else{
# 74
struct _dyneither_ptr newarr=({unsigned int _tmp11=(unsigned int)(xarr->num_elmts * 2);void**_tmp10=({struct _RegionHandle*_tmp38=Cyc_Core_unique_region;_region_malloc(_tmp38,_check_times(_tmp11,sizeof(void*)));});({{unsigned int _tmp2A=(unsigned int)(xarr->num_elmts * 2);unsigned int i;for(i=0;i < _tmp2A;++ i){_tmp10[i]=*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),0));}}0;});_tag_dyneither(_tmp10,sizeof(void*),_tmp11);});
{int i=1;for(0;i < xarr->num_elmts;++ i){
({void*_tmp39=*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i));*((void**)_check_dyneither_subscript(newarr,sizeof(void*),i))=_tmp39;});}}
({struct _dyneither_ptr _tmpE=xarr->elmts;struct _dyneither_ptr _tmpF=newarr;xarr->elmts=_tmpF;newarr=_tmpE;});
Cyc_Core_ufree((void**)_untag_dyneither_ptr(newarr,sizeof(void*),1U));}}
# 81
*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),xarr->num_elmts ++))=a;}
# 84
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*xarr,void*a){
Cyc_Xarray_add(xarr,a);
return xarr->num_elmts - 1;}
# 89
struct _dyneither_ptr Cyc_Xarray_rto_array(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)
return _tag_dyneither(({unsigned int _tmp13=0;void**_tmp12=({struct _RegionHandle*_tmp3A=r;_region_malloc(_tmp3A,_check_times(_tmp13,sizeof(void*)));});*_tmp12=0;_tmp12;}),sizeof(void*),0U);{
struct _dyneither_ptr ans=({unsigned int _tmp15=(unsigned int)xarr->num_elmts;void**_tmp14=({struct _RegionHandle*_tmp3B=r;_region_malloc(_tmp3B,_check_times(_tmp15,sizeof(void*)));});({{unsigned int _tmp2B=(unsigned int)xarr->num_elmts;unsigned int i;for(i=0;i < _tmp2B;++ i){_tmp14[i]=*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),(int)i));}}0;});_tag_dyneither(_tmp14,sizeof(void*),_tmp15);});
return ans;};}
# 96
struct _dyneither_ptr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rto_array(Cyc_Core_heap_region,xarr);}
# 100
struct Cyc_Xarray_Xarray*Cyc_Xarray_rfrom_array(struct _RegionHandle*r,struct _dyneither_ptr arr){
if(_get_dyneither_size(arr,sizeof(void*))== (unsigned int)0)
return Cyc_Xarray_rcreate_empty(r);{
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp18=_region_malloc(r,sizeof(*_tmp18));({
struct _dyneither_ptr _tmp3D=({unsigned int _tmp17=_get_dyneither_size(arr,sizeof(void*));void**_tmp16=({struct _RegionHandle*_tmp3C=Cyc_Core_unique_region;_region_malloc(_tmp3C,_check_times(_tmp17,sizeof(void*)));});({{unsigned int _tmp2C=_get_dyneither_size(arr,sizeof(void*));unsigned int i;for(i=0;i < _tmp2C;++ i){_tmp16[i]=*((void**)_check_dyneither_subscript(arr,sizeof(void*),(int)i));}}0;});_tag_dyneither(_tmp16,sizeof(void*),_tmp17);});_tmp18->elmts=_tmp3D;}),_tmp18->num_elmts=(int)
_get_dyneither_size(arr,sizeof(void*));_tmp18;});
return ans;};}
# 109
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _dyneither_ptr arr){
return Cyc_Xarray_rfrom_array(Cyc_Core_heap_region,arr);}
# 114
struct Cyc_Xarray_Xarray*Cyc_Xarray_rappend(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){
int newsz=(int)(_get_dyneither_size(xarr1->elmts,sizeof(void*))+ _get_dyneither_size(xarr2->elmts,sizeof(void*)));
if(newsz == 0)
return Cyc_Xarray_rcreate_empty(r);{
void*init=_get_dyneither_size(xarr1->elmts,sizeof(void*))== (unsigned int)0?*((void**)_check_dyneither_subscript(xarr2->elmts,sizeof(void*),0)):*((void**)_check_dyneither_subscript(xarr1->elmts,sizeof(void*),0));
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp1B=_region_malloc(r,sizeof(*_tmp1B));({struct _dyneither_ptr _tmp3F=({unsigned int _tmp1A=(unsigned int)newsz;void**_tmp19=({struct _RegionHandle*_tmp3E=Cyc_Core_unique_region;_region_malloc(_tmp3E,_check_times(_tmp1A,sizeof(void*)));});({{unsigned int _tmp2D=(unsigned int)newsz;unsigned int i;for(i=0;i < _tmp2D;++ i){_tmp19[i]=init;}}0;});_tag_dyneither(_tmp19,sizeof(void*),_tmp1A);});_tmp1B->elmts=_tmp3F;}),_tmp1B->num_elmts=0;_tmp1B;});
# 121
{int i=0;for(0;i < xarr1->num_elmts;++ i){
Cyc_Xarray_add(ans,*((void**)_check_dyneither_subscript(xarr1->elmts,sizeof(void*),i)));}}
{int i=0;for(0;i < xarr2->num_elmts;++ i){
Cyc_Xarray_add(ans,*((void**)_check_dyneither_subscript(xarr2->elmts,sizeof(void*),i)));}}
return ans;};}
# 128
struct Cyc_Xarray_Xarray*Cyc_Xarray_append(struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){
return Cyc_Xarray_rappend(Cyc_Core_heap_region,xarr1,xarr2);}
# 132
void Cyc_Xarray_app(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 137
void Cyc_Xarray_app_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 142
void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 147
void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 152
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)return Cyc_Xarray_rcreate_empty(r);{
# 155
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp1E=_region_malloc(r,sizeof(*_tmp1E));
({struct _dyneither_ptr _tmp42=({unsigned int _tmp1D=_get_dyneither_size(xarr->elmts,sizeof(void*));void**_tmp1C=({struct _RegionHandle*_tmp40=Cyc_Core_unique_region;_region_malloc(_tmp40,_check_times(_tmp1D,sizeof(void*)));});({{unsigned int _tmp2E=_get_dyneither_size(xarr->elmts,sizeof(void*));unsigned int i;for(i=0;i < _tmp2E;++ i){({void*_tmp41=f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),0)));_tmp1C[i]=_tmp41;});}}0;});_tag_dyneither(_tmp1C,sizeof(void*),_tmp1D);});_tmp1E->elmts=_tmp42;}),_tmp1E->num_elmts=xarr->num_elmts;_tmp1E;});
# 158
{int i=1;for(0;i < xarr->num_elmts;++ i){
({void*_tmp43=f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));*((void**)_check_dyneither_subscript(ans->elmts,sizeof(void*),i))=_tmp43;});}}
return ans;};}
# 163
struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rmap(Cyc_Core_heap_region,f,xarr);}
# 167
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)return Cyc_Xarray_rcreate_empty(r);{
# 170
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp21=_region_malloc(r,sizeof(*_tmp21));
({struct _dyneither_ptr _tmp46=({unsigned int _tmp20=_get_dyneither_size(xarr->elmts,sizeof(void*));void**_tmp1F=({struct _RegionHandle*_tmp44=Cyc_Core_unique_region;_region_malloc(_tmp44,_check_times(_tmp20,sizeof(void*)));});({{unsigned int _tmp2F=_get_dyneither_size(xarr->elmts,sizeof(void*));unsigned int i;for(i=0;i < _tmp2F;++ i){({void*_tmp45=f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),0)));_tmp1F[i]=_tmp45;});}}0;});_tag_dyneither(_tmp1F,sizeof(void*),_tmp20);});_tmp21->elmts=_tmp46;}),_tmp21->num_elmts=xarr->num_elmts;_tmp21;});
# 173
{int i=1;for(0;i < xarr->num_elmts;++ i){
({void*_tmp47=f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));*((void**)_check_dyneither_subscript(ans->elmts,sizeof(void*),i))=_tmp47;});}}
return ans;};}
# 178
struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rmap_c(Cyc_Core_heap_region,f,env,xarr);}
# 182
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr){
struct _dyneither_ptr newarr=_tag_dyneither(0,0,0);
({struct _dyneither_ptr _tmp22=newarr;struct _dyneither_ptr _tmp23=xarr->elmts;newarr=_tmp23;xarr->elmts=_tmp22;});
xarr->num_elmts=0;
Cyc_Core_ufree((void**)_untag_dyneither_ptr(newarr,sizeof(void*),1U));}
# 189
void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*xarr,int num){
if(({int _tmp48=num;_tmp48 > Cyc_Xarray_length(xarr);}))
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp49=({const char*_tmp24="number deleted is greater than length of xarray";_tag_dyneither(_tmp24,sizeof(char),48U);});_tmp25->f1=_tmp49;});_tmp25;}));
xarr->num_elmts -=num;}
# 195
void Cyc_Xarray_remove(struct Cyc_Xarray_Xarray*xarr,int i){
if(i < 0  || i > xarr->num_elmts - 1)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp4A=({const char*_tmp26="xarray index out of bounds";_tag_dyneither(_tmp26,sizeof(char),27U);});_tmp27->f1=_tmp4A;});_tmp27;}));
{int j=i;for(0;j < xarr->num_elmts - 1;++ j){
({void*_tmp4B=*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),j + 1));*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),j))=_tmp4B;});}}
-- xarr->num_elmts;}

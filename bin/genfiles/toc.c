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

//extern struct _xtunion_struct *_exn_thrown;
extern struct _xtunion_struct* Cyc_Core_get_exn_thrown();
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
   (((char *)ptr) + (elt_sz)*(index))
#ifdef NO_CYC_NULL_CHECKS
#define _check_known_subscript_null _check_known_subscript_notnull
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  if (!_cks_ptr) _throw_null(); \
  (_cks_ptr) + (elt_sz)*(index); })
#endif

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
void Cyc_Core_ufree(void*ptr);
# 188
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 237
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*file,const char*func,int lineno);
# 255 "core.h"
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 67
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 40 "xarray.h"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_Position_Error;
# 26 "warn.h"
void Cyc_Warn_vwarn(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 28
void Cyc_Warn_warn(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 30
void Cyc_Warn_flush_warnings();
# 32
void Cyc_Warn_verr(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Warn_err(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 39
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void*Cyc_Warn_vimpos_loc(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
void*Cyc_Warn_impos_loc(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 323
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 705 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 881
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 888
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 897
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 918
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 920
extern void*Cyc_Absyn_sint_type;
# 925
extern void*Cyc_Absyn_heap_rgn_type;
# 929
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 931
extern void*Cyc_Absyn_void_type;
# 948
void*Cyc_Absyn_exn_type();
# 956
extern void*Cyc_Absyn_fat_bound_type;
# 960
void*Cyc_Absyn_bounds_one();
# 970
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 976
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq);
# 978
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 980
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_type(struct _tuple1*name);
# 986
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 991
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1066
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned loc);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc);
# 1085
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc);
# 1090
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s);
# 1094
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc);
# 1099
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1142
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1145
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1163
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1165
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1167
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1176
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1181
extern int Cyc_Absyn_no_regions;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 32 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr,struct _dyneither_ptr);
# 33
void Cyc_Tcutil_terr(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 35
void Cyc_Tcutil_warn(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 40
int Cyc_Tcutil_is_void_type(void*);
# 45
int Cyc_Tcutil_is_arithmetic_type(void*);
# 48
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 53
int Cyc_Tcutil_is_dyneither_ptr(void*t);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
# 58
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 68
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 76
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 81
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 83
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 89
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 102
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 105
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 109
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
# 112
void*Cyc_Tcutil_compress(void*t);
# 140
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 150
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 194
int Cyc_Tcutil_unify(void*,void*);
# 197
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 199
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 201
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 277 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 285
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 331
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 349
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 360
int Cyc_Tcutil_force_type2bool(int desired,void*t);struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 34 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
# 36
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;
# 42
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 34 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 38
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_finish();struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 58 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 73 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 75
({struct _dyneither_ptr _tmp735=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp734=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp734,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp735,ap);});}
# 77
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 79
({struct _dyneither_ptr _tmp737=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp736=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp736,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp737,ap);});}
# 82
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 85
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 95
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 97
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 123
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp2=*x;struct _tuple14 _tmp3=_tmp2;struct _tuple1*_tmp9;struct _tuple1*_tmp8;_LL1: _tmp9=_tmp3.f1;_tmp8=_tmp3.f2;_LL2:;{
struct _tuple14 _tmp4=*y;struct _tuple14 _tmp5=_tmp4;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL4: _tmp7=_tmp5.f1;_tmp6=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp9,_tmp7);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp8,_tmp6);};};}
# 132
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp11=_region_malloc(d,sizeof(*_tmp11));
({struct Cyc_List_List**_tmp743=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp743;}),({
struct Cyc_List_List**_tmp742=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp742;}),({
struct Cyc_Dict_Dict*_tmp741=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp740=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp740;});_tmpC;});_tmp11->aggrs_so_far=_tmp741;}),({
struct Cyc_List_List**_tmp73F=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp73F;}),({
struct Cyc_Set_Set**_tmp73E=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp73D=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp73D;});_tmpE;});_tmp11->datatypes_so_far=_tmp73E;}),({
struct Cyc_Dict_Dict*_tmp73C=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp73B=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp73B;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp73C;}),({
struct Cyc_Dict_Dict*_tmp73A=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp739=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp739;});_tmp10;});_tmp11->qvar_tags=_tmp73A;}),({
struct Cyc_Xarray_Xarray*_tmp738=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp738;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 151
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 157
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 160
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp744=({struct Cyc_Toc_TocStateWrap _tmp703;_tmp703.dyn=_tmp1C,_tmp703.state=_tmp1B;_tmp703;});*ts=_tmp744;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp1A=({struct _tuple15 _tmp704;_tmp704.f1=_tmp1B,_tmp704.f2=arg;_tmp704;});
res=f(h,& _tmp1A);}
# 168
;}
# 170
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 174
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 177
struct _tuple17 _tmp1D=*env;struct _tuple17 _tmp1E=_tmp1D;struct Cyc_Toc_TocState*_tmp21;struct _tuple1*_tmp20;void*(*_tmp1F)(struct _tuple1*);_LL1: _tmp21=_tmp1E.f1;_tmp20=(_tmp1E.f2)->f1;_tmp1F=(_tmp1E.f2)->f2;_LL2:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp21->aggrs_so_far,_tmp20);
return v == 0?_tmp1F(_tmp20):(*(*v)).f2;};}
# 182
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp705;_tmp705.f1=q,_tmp705.f2=type_maker;_tmp705;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp22[5U]="curr";
# 195 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp22,_tmp22,_tmp22 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp23[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp23,_tmp23,_tmp23 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp24[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp24,_tmp24,_tmp24 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp25[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp25,_tmp25,_tmp25 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp26[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp26,_tmp26,_tmp26 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp27[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp27,_tmp27,_tmp27 + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
# 215 "toc.cyc"
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression(){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 218
if((unsigned)_get_exn_thrown_e)
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_tmp32=_cycalloc(sizeof(*_tmp32));((_tmp32->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp749=({struct _dyneither_ptr*_tmp2D[1U];({struct _dyneither_ptr*_tmp748=({struct _dyneither_ptr*_tmp2F=_cycalloc(sizeof(*_tmp2F));({struct _dyneither_ptr _tmp747=({const char*_tmp2E="Core";_tag_dyneither(_tmp2E,sizeof(char),5U);});*_tmp2F=_tmp747;});_tmp2F;});_tmp2D[0]=_tmp748;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2D,sizeof(struct _dyneither_ptr*),1U));});((_tmp32->f1).Abs_n).val=_tmp749;}),({struct _dyneither_ptr*_tmp746=({struct _dyneither_ptr*_tmp31=_cycalloc(sizeof(*_tmp31));({struct _dyneither_ptr _tmp745=({const char*_tmp30="get_exn_thrown";_tag_dyneither(_tmp30,sizeof(char),15U);});*_tmp31=_tmp745;});_tmp31;});_tmp32->f2=_tmp746;});_tmp32;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,_tmp2C->f1=qv;_tmp2C;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->topt=0,({void*_tmp74A=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=1U,_tmp2A->f1=bnd;_tmp2A;});_tmp2B->r=_tmp74A;}),_tmp2B->loc=0U,_tmp2B->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp2B;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=10U,_tmp29->f1=fnname,_tmp29->f2=0,_tmp29->f3=0,_tmp29->f4=0;_tmp29;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->topt=0,_tmp28->r=fncall_re,_tmp28->loc=0U,_tmp28->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp28;});
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);};}static char _tmp33[7U]="_throw";
# 228
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp33,_tmp33,_tmp33 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp34[7U]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp34,_tmp34,_tmp34 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp35[14U]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp35,_tmp35,_tmp35 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp36[13U]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp36,_tmp36,_tmp36 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp37[12U]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp37,_tmp37,_tmp37 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp38[14U]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp38,_tmp38,_tmp38 + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp39[12U]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp39,_tmp39,_tmp39 + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp3A[28U]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp3A,_tmp3A,_tmp3A + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp3B[31U]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp3B,_tmp3B,_tmp3B + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp3C[27U]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp3C,_tmp3C,_tmp3C + 27U};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0U,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1U,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp3D[15U]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp3D,_tmp3D,_tmp3D + 15U};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0U,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1U,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp3E[15U]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp3E,_tmp3E,_tmp3E + 15U};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0U,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1U,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp3F[21U]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp3F,_tmp3F,_tmp3F + 21U};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0U,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1U,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp40[20U]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp40,_tmp40,_tmp40 + 20U};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0U,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1U,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp41[19U]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp41,_tmp41,_tmp41 + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp42[24U]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp42,_tmp42,_tmp42 + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp43[25U]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp43,_tmp43,_tmp43 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp44[23U]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp44,_tmp44,_tmp44 + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp45[25U]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp45,_tmp45,_tmp45 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp46[26U]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp46,_tmp46,_tmp46 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp47[30U]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp47,_tmp47,_tmp47 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp48[28U]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp48,_tmp48,_tmp48 + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp49[20U]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp49,_tmp49,_tmp49 + 20U};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4A[15U]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4A,_tmp4A,_tmp4A + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp4B[20U]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp4B,_tmp4B,_tmp4B + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp4C[21U]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp4C,_tmp4C,_tmp4C + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp4D[19U]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp4D,_tmp4D,_tmp4D + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp4E[21U]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp4E,_tmp4E,_tmp4E + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp4F[22U]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp4F,_tmp4F,_tmp4F + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp50[26U]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp50,_tmp50,_tmp50 + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp51[24U]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp51,_tmp51,_tmp51 + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp52[28U]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp52,_tmp52,_tmp52 + 28U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp53[23U]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp53,_tmp53,_tmp53 + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp54[28U]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp54,_tmp54,_tmp54 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp55[29U]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp55,_tmp55,_tmp55 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp56[27U]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp56,_tmp56,_tmp56 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp57[29U]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp57,_tmp57,_tmp57 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp58[30U]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp58,_tmp58,_tmp58 + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp59[34U]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp59,_tmp59,_tmp59 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp5A[32U]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp5A,_tmp5A,_tmp5A + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp5B[33U]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp5B,_tmp5B,_tmp5B + 33U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp5C[28U]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp5C,_tmp5C,_tmp5C + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp5D[33U]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp5D,_tmp5D,_tmp5D + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp5E[34U]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp5E,_tmp5E,_tmp5E + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp5F[32U]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp5F,_tmp5F,_tmp5F + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp60[34U]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp60,_tmp60,_tmp60 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp61[35U]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp61,_tmp61,_tmp61 + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp62[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp62,_tmp62,_tmp62 + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp63[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp63,_tmp63,_tmp63 + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp64[10U]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp64,_tmp64,_tmp64 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp65[11U]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp65,_tmp65,_tmp65 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp66[17U]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp66,_tmp66,_tmp66 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp67[18U]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp67,_tmp67,_tmp67 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp68[15U]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp68,_tmp68,_tmp68 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp69[15U]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp69,_tmp69,_tmp69 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp6A[13U]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp6A,_tmp6A,_tmp6A + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp6B[12U]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp6B,_tmp6B,_tmp6B + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp6C[13U]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp6C,_tmp6C,_tmp6C + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp6D[12U]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp6D,_tmp6D,_tmp6D + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp6E[19U]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp6E,_tmp6E,_tmp6E + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp6F[29U]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp6F,_tmp6F,_tmp6F + 29U};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0U,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmp70[13U]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmp70,_tmp70,_tmp70 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp71[9U]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmp71,_tmp71,_tmp71 + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp72[20U]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp72,_tmp72,_tmp72 + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 296
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 298
void*Cyc_Toc_void_star_type(){
static void*void_star_t_opt=0;
if(void_star_t_opt == 0)
void_star_t_opt=({void*_tmp74D=Cyc_Absyn_void_type;void*_tmp74C=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp74B=
Cyc_Absyn_empty_tqual(0U);
# 301
Cyc_Absyn_star_type(_tmp74D,_tmp74C,_tmp74B,Cyc_Absyn_false_type);});
# 303
return(void*)_check_null(void_star_t_opt);}
# 305
static void*Cyc_Toc_dyneither_ptr_type(){
static void*dpt=0;
if(dpt == 0)
dpt=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0),0);
return(void*)_check_null(dpt);}
# 311
static void*Cyc_Toc_rgn_type(){
static void*r=0;
if(r == 0)
r=({void*_tmp74E=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp74E,Cyc_Toc_mt_tq);});
return(void*)_check_null(r);}
# 318
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 322
static struct _tuple19*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp75=_cycalloc(sizeof(*_tmp75));({struct Cyc_List_List*_tmp750=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));({void*_tmp74F=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=1U,_tmp73->f1=name;_tmp73;});_tmp74->hd=_tmp74F;}),_tmp74->tl=0;_tmp74;});_tmp75->f1=_tmp750;}),_tmp75->f2=e;_tmp75;});}
# 326
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _dyneither_ptr args){
return({struct Cyc_Absyn_Exp*_tmp751=f;Cyc_Absyn_fncall_exp(_tmp751,((struct Cyc_List_List*(*)(struct _dyneither_ptr arr))Cyc_List_from_array)(args),0U);});}
# 329
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp76=e->r;void*_tmp77=_tmp76;struct Cyc_Absyn_Exp*_tmp78;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp78=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp78);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 337
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=14U,_tmp79->f1=t,_tmp79->f2=e,_tmp79->f3=0,_tmp79->f4=Cyc_Absyn_No_coercion;_tmp79;});}
# 340
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=20U,_tmp7A->f1=e;_tmp7A;});}
# 343
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=23U,_tmp7B->f1=e1,_tmp7B->f2=e2;_tmp7B;});}
# 346
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=37U,_tmp7C->f1=s;_tmp7C;});}
# 349
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=17U,_tmp7D->f1=t;_tmp7D;});}
# 352
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=10U,_tmp7E->f1=e,({struct Cyc_List_List*_tmp752=((struct Cyc_List_List*(*)(struct _dyneither_ptr arr))Cyc_List_from_array)(es);_tmp7E->f2=_tmp752;}),_tmp7E->f3=0,_tmp7E->f4=1;_tmp7E;});}
# 355
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=2U,_tmp7F->f1=s1,_tmp7F->f2=s2;_tmp7F;});}
# 358
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=6U,_tmp80->f1=e1,_tmp80->f2=e2,_tmp80->f3=e3;_tmp80;});}
# 361
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=21U,_tmp81->f1=e,_tmp81->f2=n,_tmp81->f3=0,_tmp81->f4=0;_tmp81;});}
# 364
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 367
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=36U,_tmp82->f1=tdopt,_tmp82->f2=ds;_tmp82;});}
# 369
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=8U,_tmp83->f1=v;_tmp83;});}
# 372
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 376
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned loc){
void*_tmp84=e->r;void*_tmp85=_tmp84;struct Cyc_Absyn_Exp*_tmp86;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp85)->tag == 20U){_LL1: _tmp86=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp85)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp86,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 395
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 404
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 413
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 422
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 433
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp87=Cyc_Tcutil_compress(t);void*_tmp88=_tmp87;enum Cyc_Absyn_Size_of _tmp8F;switch(*((int*)_tmp88)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)){case 1U: _LL1: _tmp8F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)->f2;_LL2:
# 437
{enum Cyc_Absyn_Size_of _tmp89=_tmp8F;switch(_tmp89){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp8A=0U;({struct _dyneither_ptr _tmp753=({const char*_tmp8B="impossible IntCon (not char, short or int)";_tag_dyneither(_tmp8B,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp753,_tag_dyneither(_tmp8A,sizeof(void*),0U));});});}_LLD:;}
# 443
goto _LL0;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}default: goto _LLB;}case 3U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8E=({struct Cyc_String_pa_PrintArg_struct _tmp706;_tmp706.tag=0U,({struct _dyneither_ptr _tmp754=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp706.f1=_tmp754;});_tmp706;});void*_tmp8C[1U];_tmp8C[0]=& _tmp8E;({struct _dyneither_ptr _tmp755=({const char*_tmp8D="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8D,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp755,_tag_dyneither(_tmp8C,sizeof(void*),1U));});});}_LL0:;}
# 450
return function;}
# 452
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp90=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp91=_tmp90;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->tag == 3U){_LL1: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp94);}else{_LL3: _LL4:
({void*_tmp92=0U;({struct _dyneither_ptr _tmp756=({const char*_tmp93="impossible type (not pointer)";_tag_dyneither(_tmp93,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp756,_tag_dyneither(_tmp92,sizeof(void*),0U));});});}_LL0:;}
# 462
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp95=e->r;void*_tmp96=_tmp95;struct Cyc_List_List*_tmpA2;struct Cyc_List_List*_tmpA1;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;long long _tmp9C;int _tmp9B;short _tmp9A;struct _dyneither_ptr _tmp99;char _tmp98;switch(*((int*)_tmp96)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Null_c).tag){case 2U: _LL1: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Char_c).val).f2;_LL2:
 return(int)_tmp98 == (int)'\000';case 3U: _LL3: _tmp99=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Wchar_c).val;_LL4: {
# 466
unsigned long _tmp97=Cyc_strlen((struct _dyneither_ptr)_tmp99);
int i=0;
if(_tmp97 >= (unsigned long)2 &&(int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)_tmp99.curr)[1]== (int)'x' && _tmp97 >= (unsigned long)3)&&(int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp97;++ i){
if((int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 476
return 0;}}case 4U: _LL5: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Short_c).val).f2;_LL6:
 return(int)_tmp9A == 0;case 5U: _LL7: _tmp9B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Int_c).val).f2;_LL8:
 return _tmp9B == 0;case 6U: _LL9: _tmp9C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).LongLong_c).val).f2;_LLA:
 return _tmp9C == (long long)0;case 1U: _LLD: _LLE:
# 481
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 480
 goto _LLE;case 14U: _LLF: _tmp9D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96)->f2;_LL10:
# 482
 return Cyc_Toc_is_zero(_tmp9D);case 24U: _LL11: _tmp9E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp96)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9E);case 26U: _LL13: _tmp9F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp96)->f1;_LL14:
 _tmpA0=_tmp9F;goto _LL16;case 29U: _LL15: _tmpA0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp96)->f3;_LL16:
 _tmpA1=_tmpA0;goto _LL18;case 25U: _LL17: _tmpA1=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp96)->f2;_LL18:
 _tmpA2=_tmpA1;goto _LL1A;case 36U: _LL19: _tmpA2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp96)->f2;_LL1A:
# 488
 for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmpA2->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 496
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA3=x;union Cyc_Absyn_Nmspace _tmpB5;struct _dyneither_ptr*_tmpB4;_LL1: _tmpB5=_tmpA3->f1;_tmpB4=_tmpA3->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA4=_tmpB5;struct Cyc_List_List*_tmpB3;struct Cyc_List_List*_tmpB2;struct Cyc_List_List*_tmpB1;switch((_tmpA4.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpB1=0;goto _LL7;case 1U: _LL6: _tmpB1=(_tmpA4.Rel_n).val;_LL7:
 _tmpB2=_tmpB1;goto _LL9;case 2U: _LL8: _tmpB2=(_tmpA4.Abs_n).val;_LL9:
 _tmpB3=_tmpB2;goto _LLB;default: _LLA: _tmpB3=(_tmpA4.C_n).val;_LLB:
# 505
 if(_tmpB3 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA7=({struct Cyc_String_pa_PrintArg_struct _tmp708;_tmp708.tag=0U,_tmp708.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp708;});struct Cyc_String_pa_PrintArg_struct _tmpA8=({struct Cyc_String_pa_PrintArg_struct _tmp707;_tmp707.tag=0U,_tmp707.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB4);_tmp707;});void*_tmpA5[2U];_tmpA5[0]=& _tmpA7,_tmpA5[1]=& _tmpA8;({struct _dyneither_ptr _tmp757=({const char*_tmpA6="%s_%s_struct";_tag_dyneither(_tmpA6,sizeof(char),13U);});Cyc_aprintf(_tmp757,_tag_dyneither(_tmpA5,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA9=_new_region("r");struct _RegionHandle*r=& _tmpA9;_push_region(r);
{struct _dyneither_ptr _tmpB0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp70B;_tmp70B.tag=0U,_tmp70B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp70B;});struct Cyc_String_pa_PrintArg_struct _tmpAD=({struct Cyc_String_pa_PrintArg_struct _tmp70A;_tmp70A.tag=0U,({struct _dyneither_ptr _tmp75A=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp759=r;struct Cyc_List_List*_tmp758=_tmpB3;Cyc_rstr_sepstr(_tmp759,_tmp758,({const char*_tmpAF="_";_tag_dyneither(_tmpAF,sizeof(char),2U);}));}));_tmp70A.f1=_tmp75A;});_tmp70A;});struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmp709;_tmp709.tag=0U,_tmp709.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB4);_tmp709;});void*_tmpAA[3U];_tmpAA[0]=& _tmpAC,_tmpAA[1]=& _tmpAD,_tmpAA[2]=& _tmpAE;({struct _dyneither_ptr _tmp75B=({const char*_tmpAB="%s_%s_%s_struct";_tag_dyneither(_tmpAB,sizeof(char),16U);});Cyc_aprintf(_tmp75B,_tag_dyneither(_tmpAA,sizeof(void*),3U));});});_npop_handler(0U);return _tmpB0;};_pop_region(r);};}_LL3:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 518
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 521
struct _tuple20 _tmpB6=*env;struct _tuple20 _tmpB7=_tmpB6;struct Cyc_Dict_Dict*_tmpCA;struct _tuple14*_tmpC9;_LL1: _tmpCA=(_tmpB7.f1)->qvar_tags;_tmpC9=_tmpB7.f2;_LL2:;{
struct _tuple14 _tmpB8=*_tmpC9;struct _tuple14 _tmpB9=_tmpB8;struct _tuple1*_tmpC8;struct _tuple1*_tmpC7;_LL4: _tmpC8=_tmpB9.f1;_tmpC7=_tmpB9.f2;_LL5:;{
struct _handler_cons _tmpBA;_push_handler(& _tmpBA);{int _tmpBC=0;if(setjmp(_tmpBA.handler))_tmpBC=1;if(!_tmpBC){{struct _tuple1*_tmpBD=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmpCA,Cyc_Toc_qvar_tag_cmp,_tmpC9);_npop_handler(0U);return _tmpBD;};_pop_handler();}else{void*_tmpBB=(void*)Cyc_Core_get_exn_thrown();void*_tmpBE=_tmpBB;void*_tmpC6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBE)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 525
struct _tuple14*_tmpBF=({struct _tuple14*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->f1=_tmpC8,_tmpC5->f2=_tmpC7;_tmpC5;});
struct _tuple1*_tmpC0=_tmpC8;union Cyc_Absyn_Nmspace _tmpC4;struct _dyneither_ptr*_tmpC3;_LLC: _tmpC4=_tmpC0->f1;_tmpC3=_tmpC0->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC3,_tmpC7);
struct _tuple1*res=({struct _tuple1*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->f1=_tmpC4,({struct _dyneither_ptr*_tmp75C=({struct _dyneither_ptr*_tmpC1=_cycalloc(sizeof(*_tmpC1));*_tmpC1=newvar;_tmpC1;});_tmpC2->f2=_tmp75C;});_tmpC2;});
({struct Cyc_Dict_Dict _tmp75D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpCA,_tmpBF,res);*_tmpCA=_tmp75D;});
return res;};}}else{_LL9: _tmpC6=_tmpBE;_LLA:(int)_rethrow(_tmpC6);}_LL6:;}};};};}
# 533
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp70C;_tmp70C.f1=fieldname,_tmp70C.f2=dtname;_tmp70C;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 540
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->kind=Cyc_Absyn_StructA,_tmpCD->sc=Cyc_Absyn_Public,_tmpCD->tvs=0,_tmpCD->attributes=0,_tmpCD->expected_mem_kind=0,({
# 543
struct _tuple1*_tmp760=({struct _tuple1*_tmpCB=_cycalloc(sizeof(*_tmpCB));({union Cyc_Absyn_Nmspace _tmp75F=Cyc_Absyn_Rel_n(0);_tmpCB->f1=_tmp75F;}),_tmpCB->f2=name;_tmpCB;});_tmpCD->name=_tmp760;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp75E=({struct Cyc_Absyn_AggrdeclImpl*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->exist_vars=0,_tmpCC->rgn_po=0,_tmpCC->tagged=0,_tmpCC->fields=fs;_tmpCC;});_tmpCD->impl=_tmp75E;});_tmpCD;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 552
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 555
struct _tuple21 _tmpCE=*env;struct _tuple21 _tmpCF=_tmpCE;struct Cyc_List_List**_tmpE5;struct Cyc_List_List*_tmpE4;_LL1: _tmpE5=(_tmpCF.f1)->tuple_types;_tmpE4=_tmpCF.f2;_LL2:;
# 557
{struct Cyc_List_List*_tmpD0=*_tmpE5;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){
struct _tuple22*_tmpD1=(struct _tuple22*)_tmpD0->hd;struct _tuple22*_tmpD2=_tmpD1;void*_tmpD5;struct Cyc_List_List*_tmpD4;_LL4: _tmpD5=_tmpD2->f1;_tmpD4=_tmpD2->f2;_LL5:;{
struct Cyc_List_List*_tmpD3=_tmpE4;
for(0;_tmpD3 != 0 && _tmpD4 != 0;(_tmpD3=_tmpD3->tl,_tmpD4=_tmpD4->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD3->hd)).f2,(void*)_tmpD4->hd))
break;}
if(_tmpD3 == 0 && _tmpD4 == 0)
return _tmpD5;};}}{
# 568
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE3=_cycalloc(sizeof(*_tmpE3));({struct _dyneither_ptr _tmp762=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE2=({struct Cyc_Int_pa_PrintArg_struct _tmp70D;_tmp70D.tag=1U,_tmp70D.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp70D;});void*_tmpE0[1U];_tmpE0[0]=& _tmpE2;({struct _dyneither_ptr _tmp761=({const char*_tmpE1="_tuple%d";_tag_dyneither(_tmpE1,sizeof(char),9U);});Cyc_aprintf(_tmp761,_tag_dyneither(_tmpE0,sizeof(void*),1U));});});*_tmpE3=_tmp762;});_tmpE3;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE4);
struct Cyc_List_List*_tmpD6=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmpD6=({struct Cyc_List_List*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct Cyc_Absyn_Aggrfield*_tmp764=({struct Cyc_Absyn_Aggrfield*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct _dyneither_ptr*_tmp763=Cyc_Absyn_fieldname(i);_tmpD7->name=_tmp763;}),_tmpD7->tq=Cyc_Toc_mt_tq,_tmpD7->type=(void*)ts2->hd,_tmpD7->width=0,_tmpD7->attributes=0,_tmpD7->requires_clause=0;_tmpD7;});_tmpD8->hd=_tmp764;}),_tmpD8->tl=_tmpD6;_tmpD8;});}}
_tmpD6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD6);{
struct Cyc_Absyn_Aggrdecl*_tmpD9=Cyc_Toc_make_c_struct_defn(xname,_tmpD6);
void*_tmpDA=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDF=_cycalloc(sizeof(*_tmpDF));*_tmpDF=_tmpD9;_tmpDF;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));({struct Cyc_Absyn_Decl*_tmp765=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->tag=5U,_tmpDB->f1=_tmpD9;_tmpDB;}),0U);_tmpDC->hd=_tmp765;}),_tmpDC->tl=Cyc_Toc_result_decls;_tmpDC;});
({struct Cyc_List_List*_tmp767=({struct Cyc_List_List*_tmpDE=_region_malloc(d,sizeof(*_tmpDE));({struct _tuple22*_tmp766=({struct _tuple22*_tmpDD=_region_malloc(d,sizeof(*_tmpDD));_tmpDD->f1=_tmpDA,_tmpDD->f2=ts;_tmpDD;});_tmpDE->hd=_tmp766;}),_tmpDE->tl=*_tmpE5;_tmpDE;});*_tmpE5=_tmp767;});
return _tmpDA;};};}
# 581
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 586
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 589
struct _tuple24*_tmpE6=env;struct Cyc_List_List**_tmpFA;enum Cyc_Absyn_AggrKind _tmpF9;struct Cyc_List_List*_tmpF8;_LL1: _tmpFA=(_tmpE6->f1)->anon_aggr_types;_tmpF9=(_tmpE6->f2)->f1;_tmpF8=(_tmpE6->f2)->f2;_LL2:;
# 591
{struct Cyc_List_List*_tmpE7=*_tmpFA;for(0;_tmpE7 != 0;_tmpE7=_tmpE7->tl){
struct _tuple25*_tmpE8=(struct _tuple25*)_tmpE7->hd;struct _tuple25*_tmpE9=_tmpE8;void*_tmpEC;enum Cyc_Absyn_AggrKind _tmpEB;struct Cyc_List_List*_tmpEA;_LL4: _tmpEC=_tmpE9->f1;_tmpEB=_tmpE9->f2;_tmpEA=_tmpE9->f3;_LL5:;
if((int)_tmpF9 != (int)_tmpEB)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpEA,_tmpF8))
return _tmpEC;}}{
# 600
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct _dyneither_ptr _tmp769=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp70E;_tmp70E.tag=1U,_tmp70E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp70E;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp768=({const char*_tmpF5="_tuple%d";_tag_dyneither(_tmpF5,sizeof(char),9U);});Cyc_aprintf(_tmp768,_tag_dyneither(_tmpF4,sizeof(void*),1U));});});*_tmpF7=_tmp769;});_tmpF7;});
struct Cyc_Absyn_Aggrdecl*_tmpED=Cyc_Toc_make_c_struct_defn(xname,_tmpF8);
_tmpED->kind=_tmpF9;{
void*_tmpEE=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF3=_cycalloc(sizeof(*_tmpF3));*_tmpF3=_tmpED;_tmpF3;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpF0=_cycalloc(sizeof(*_tmpF0));({struct Cyc_Absyn_Decl*_tmp76A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->tag=5U,_tmpEF->f1=_tmpED;_tmpEF;}),0U);_tmpF0->hd=_tmp76A;}),_tmpF0->tl=Cyc_Toc_result_decls;_tmpF0;});
({struct Cyc_List_List*_tmp76C=({struct Cyc_List_List*_tmpF2=_region_malloc(d,sizeof(*_tmpF2));({struct _tuple25*_tmp76B=({struct _tuple25*_tmpF1=_region_malloc(d,sizeof(*_tmpF1));_tmpF1->f1=_tmpEE,_tmpF1->f2=_tmpF9,_tmpF1->f3=_tmpF8;_tmpF1;});_tmpF2->hd=_tmp76B;}),_tmpF2->tl=*_tmpFA;_tmpF2;});*_tmpFA=_tmp76C;});
return _tmpEE;};};}
# 608
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(({struct _tuple23*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=ak,_tmpFB->f2=fs;_tmpFB;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple26{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple27{struct Cyc_Toc_TocState*f1;struct _tuple26*f2;};struct _tuple28{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 617
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple27*env){
# 626
struct _tuple27 _tmpFC=*env;struct _tuple27 _tmpFD=_tmpFC;struct Cyc_List_List**_tmp120;struct _tuple1*_tmp11F;struct Cyc_List_List*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_List_List*_tmp11C;_LL1: _tmp120=(_tmpFD.f1)->abs_struct_types;_tmp11F=(_tmpFD.f2)->f1;_tmp11E=(_tmpFD.f2)->f2;_tmp11D=(_tmpFD.f2)->f3;_tmp11C=(_tmpFD.f2)->f4;_LL2:;
# 630
{struct Cyc_List_List*_tmpFE=*_tmp120;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct _tuple28*_tmpFF=(struct _tuple28*)_tmpFE->hd;struct _tuple28*_tmp100=_tmpFF;struct _tuple1*_tmp108;struct Cyc_List_List*_tmp107;void*_tmp106;_LL4: _tmp108=_tmp100->f1;_tmp107=_tmp100->f2;_tmp106=_tmp100->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp108,_tmp11F)== 0 &&({
int _tmp76D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11D);_tmp76D == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp107);})){
int okay=1;
{struct Cyc_List_List*_tmp101=_tmp11D;for(0;_tmp101 != 0;(_tmp101=_tmp101->tl,_tmp107=_tmp107->tl)){
void*_tmp102=(void*)_tmp101->hd;
void*_tmp103=(void*)((struct Cyc_List_List*)_check_null(_tmp107))->hd;
{struct Cyc_Absyn_Kind*_tmp104=Cyc_Tcutil_type_kind(_tmp102);struct Cyc_Absyn_Kind*_tmp105=_tmp104;switch(((struct Cyc_Absyn_Kind*)_tmp105)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 641
 continue;default: _LLB: _LLC:
# 644
 if(Cyc_Tcutil_unify(_tmp102,_tmp103)||({void*_tmp76E=Cyc_Toc_typ_to_c(_tmp102);Cyc_Tcutil_unify(_tmp76E,Cyc_Toc_typ_to_c(_tmp103));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 649
break;}}
# 651
if(okay)
return _tmp106;}}}{
# 658
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct _dyneither_ptr _tmp770=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11A=({struct Cyc_Int_pa_PrintArg_struct _tmp70F;_tmp70F.tag=1U,_tmp70F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp70F;});void*_tmp118[1U];_tmp118[0]=& _tmp11A;({struct _dyneither_ptr _tmp76F=({const char*_tmp119="_tuple%d";_tag_dyneither(_tmp119,sizeof(char),9U);});Cyc_aprintf(_tmp76F,_tag_dyneither(_tmp118,sizeof(void*),1U));});});*_tmp11B=_tmp770;});_tmp11B;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp109=0;
# 662
({struct Cyc_List_List*_tmp772=({struct Cyc_List_List*_tmp10B=_region_malloc(d,sizeof(*_tmp10B));({struct _tuple28*_tmp771=({struct _tuple28*_tmp10A=_region_malloc(d,sizeof(*_tmp10A));_tmp10A->f1=_tmp11F,_tmp10A->f2=_tmp11D,_tmp10A->f3=x;_tmp10A;});_tmp10B->hd=_tmp771;}),_tmp10B->tl=*_tmp120;_tmp10B;});*_tmp120=_tmp772;});{
# 665
struct _RegionHandle _tmp10C=_new_region("r");struct _RegionHandle*r=& _tmp10C;_push_region(r);
{struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11E,_tmp11D);
for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){
struct Cyc_Absyn_Aggrfield*_tmp10E=(struct Cyc_Absyn_Aggrfield*)_tmp11C->hd;
void*t=_tmp10E->type;
struct Cyc_List_List*atts=_tmp10E->attributes;
# 675
if((_tmp11C->tl == 0 &&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({void*_tmp773=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=6U,_tmp10F->f1=0;_tmp10F;});_tmp110->hd=_tmp773;}),_tmp110->tl=atts;_tmp110;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10D,t));
# 682
if(Cyc_Tcutil_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=4U,({void*_tmp776=Cyc_Toc_void_star_type();(_tmp111->f1).elt_type=_tmp776;}),({struct Cyc_Absyn_Tqual _tmp775=Cyc_Absyn_empty_tqual(0U);(_tmp111->f1).tq=_tmp775;}),({
struct Cyc_Absyn_Exp*_tmp774=Cyc_Absyn_uint_exp(0U,0U);(_tmp111->f1).num_elts=_tmp774;}),(_tmp111->f1).zero_term=Cyc_Absyn_false_type,(_tmp111->f1).zt_loc=0U;_tmp111;});
# 686
_tmp109=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Aggrfield*_tmp777=({struct Cyc_Absyn_Aggrfield*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->name=_tmp10E->name,_tmp112->tq=Cyc_Toc_mt_tq,_tmp112->type=t,_tmp112->width=_tmp10E->width,_tmp112->attributes=atts,_tmp112->requires_clause=0;_tmp112;});_tmp113->hd=_tmp777;}),_tmp113->tl=_tmp109;_tmp113;});}
# 688
_tmp109=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp109);{
struct Cyc_Absyn_Aggrdecl*_tmp114=Cyc_Toc_make_c_struct_defn(xname,_tmp109);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Absyn_Decl*_tmp778=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=5U,_tmp115->f1=_tmp114;_tmp115;}),0U);_tmp116->hd=_tmp778;}),_tmp116->tl=Cyc_Toc_result_decls;_tmp116;});{
void*_tmp117=x;_npop_handler(0U);return _tmp117;};};}
# 666
;_pop_region(r);};};}
# 694
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 698
struct _tuple26 env=({struct _tuple26 _tmp710;_tmp710.f1=struct_name,_tmp710.f2=type_vars,_tmp710.f3=type_args,_tmp710.f4=fields;_tmp710;});
return((void*(*)(struct _tuple26*arg,void*(*f)(struct _RegionHandle*,struct _tuple27*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 706
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp77B=({struct _dyneither_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));({struct _dyneither_ptr _tmp77A=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp123=({struct Cyc_Int_pa_PrintArg_struct _tmp711;_tmp711.tag=1U,_tmp711.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp711;});void*_tmp121[1U];_tmp121[0]=& _tmp123;({struct _dyneither_ptr _tmp779=({const char*_tmp122="_tmp%X";_tag_dyneither(_tmp122,sizeof(char),7U);});Cyc_aprintf(_tmp779,_tag_dyneither(_tmp121,sizeof(void*),1U));});});*_tmp124=_tmp77A;});_tmp124;});_tmp125->f2=_tmp77B;});_tmp125;});}struct _tuple29{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 709
struct _tuple29 Cyc_Toc_temp_var_and_exp(){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple29 _tmp712;_tmp712.f1=v,({struct Cyc_Absyn_Exp*_tmp77C=Cyc_Absyn_var_exp(v,0U);_tmp712.f2=_tmp77C;});_tmp712;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 716
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp126=*env;struct _tuple30 _tmp127=_tmp126;struct Cyc_Xarray_Xarray*_tmp12F;_LL1: _tmp12F=(_tmp127.f1)->temp_labels;_LL2:;{
int _tmp128=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp77D=_tmp128;_tmp77D < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12F);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12F,_tmp128);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct _dyneither_ptr _tmp77F=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12D=({struct Cyc_Int_pa_PrintArg_struct _tmp713;_tmp713.tag=1U,_tmp713.f1=(unsigned)_tmp128;_tmp713;});void*_tmp12B[1U];_tmp12B[0]=& _tmp12D;({struct _dyneither_ptr _tmp77E=({const char*_tmp12C="_LL%X";_tag_dyneither(_tmp12C,sizeof(char),6U);});Cyc_aprintf(_tmp77E,_tag_dyneither(_tmp12B,sizeof(void*),1U));});});*_tmp12E=_tmp77F;});_tmp12E;});
if(({int _tmp780=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12F,res);_tmp780 != _tmp128;}))
({void*_tmp129=0U;({struct _dyneither_ptr _tmp781=({const char*_tmp12A="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp12A,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp781,_tag_dyneither(_tmp129,sizeof(void*),0U));});});
return res;};};}
# 726
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 732
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp130=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp130))->hd)->name)!= 0){
++ ans;
_tmp130=_tmp130->tl;}
# 739
return Cyc_Absyn_uint_exp((unsigned)ans,0U);}
# 745
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp131=*a;struct _tuple9 _tmp132=_tmp131;struct _dyneither_ptr*_tmp136;struct Cyc_Absyn_Tqual _tmp135;void*_tmp134;_LL1: _tmp136=_tmp132.f1;_tmp135=_tmp132.f2;_tmp134=_tmp132.f3;_LL2:;
return({struct _tuple9*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->f1=_tmp136,_tmp133->f2=_tmp135,({void*_tmp782=Cyc_Toc_typ_to_c(_tmp134);_tmp133->f3=_tmp782;});_tmp133;});}
# 768 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_tmp138=_tmp137;void*_tmp13E;void*_tmp13D;struct Cyc_Absyn_Tqual _tmp13C;struct Cyc_Absyn_Exp*_tmp13B;void*_tmp13A;unsigned _tmp139;switch(*((int*)_tmp138)){case 4U: _LL1: _tmp13D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).elt_type;_tmp13C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).tq;_tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).num_elts;_tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zero_term;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zt_loc;_LL2:
# 771
 return({void*_tmp783=Cyc_Toc_typ_to_c_array(_tmp13D);Cyc_Absyn_cstar_type(_tmp783,_tmp13C);});case 1U: _LL3: _tmp13E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp138)->f2;if(_tmp13E != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13E);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 777
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 779
struct Cyc_Absyn_Aggrfield*_tmp13F=({struct Cyc_Absyn_Aggrfield*_tmp140=_cycalloc(sizeof(*_tmp140));*_tmp140=*f;_tmp140;});
_tmp13F->type=new_type;
_tmp13F->requires_clause=0;
_tmp13F->tq=Cyc_Toc_mt_tq;
return _tmp13F;}
# 786
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 788
return;}
# 791
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp141=Cyc_Tcutil_compress(t);void*_tmp142=_tmp141;struct Cyc_Absyn_Tvar*_tmp143;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->tag == 2U){_LL1: _tmp143=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 797
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp144=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp145=_tmp144;if(((struct Cyc_Absyn_Kind*)_tmp145)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 803
static int Cyc_Toc_is_void_star(void*t){
void*_tmp146=Cyc_Tcutil_compress(t);void*_tmp147=_tmp146;void*_tmp148;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp147)->tag == 3U){_LL1: _tmp148=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp147)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_void_type(_tmp148);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 809
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 812
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 816
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp149=t;struct Cyc_Absyn_Datatypedecl*_tmp192;void**_tmp191;struct Cyc_Absyn_Enumdecl*_tmp190;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18D;struct _tuple1*_tmp18C;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Typedefdecl*_tmp18A;void*_tmp189;enum Cyc_Absyn_AggrKind _tmp188;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Tqual _tmp185;void*_tmp184;struct Cyc_List_List*_tmp183;int _tmp182;struct Cyc_Absyn_VarargInfo*_tmp181;struct Cyc_List_List*_tmp180;void*_tmp17F;struct Cyc_Absyn_Tqual _tmp17E;struct Cyc_Absyn_Exp*_tmp17D;unsigned _tmp17C;void*_tmp17B;struct Cyc_Absyn_Tqual _tmp17A;void*_tmp179;struct Cyc_Absyn_Tvar*_tmp178;void**_tmp177;struct Cyc_List_List*_tmp176;struct _tuple1*_tmp175;void*_tmp174;union Cyc_Absyn_AggrInfo _tmp173;struct Cyc_List_List*_tmp172;struct Cyc_Absyn_Datatypedecl*_tmp171;struct Cyc_Absyn_Datatypefield*_tmp170;switch(*((int*)_tmp149)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 831
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp171=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1).KnownDatatypefield).val).f1;_tmp170=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1).KnownDatatypefield).val).f2;_LLA:
# 833
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp170->name,_tmp171->name));}else{_LLB: _LLC:
# 835
({void*_tmp14A=0U;({struct _dyneither_ptr _tmp784=({const char*_tmp14B="unresolved DatatypeFieldType";_tag_dyneither(_tmp14B,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp784,_tag_dyneither(_tmp14A,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 844
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp174=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1;_tmp173=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1;_tmp172=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f2;_LL1C:
# 891
{union Cyc_Absyn_AggrInfo _tmp15B=_tmp173;if((_tmp15B.UnknownAggr).tag == 1){_LL45: _LL46:
# 893
 if(_tmp172 == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->tag=0U,_tmp15C->f1=_tmp174,_tmp15C->f2=0;_tmp15C;});}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 897
struct Cyc_Absyn_Aggrdecl*_tmp15D=Cyc_Absyn_get_known_aggrdecl(_tmp173);
if(_tmp15D->expected_mem_kind){
# 900
if(_tmp15D->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp160=({struct Cyc_String_pa_PrintArg_struct _tmp715;_tmp715.tag=0U,({
struct _dyneither_ptr _tmp785=(struct _dyneither_ptr)((int)_tmp15D->kind == (int)Cyc_Absyn_UnionA?({const char*_tmp162="union";_tag_dyneither(_tmp162,sizeof(char),6U);}):({const char*_tmp163="struct";_tag_dyneither(_tmp163,sizeof(char),7U);}));_tmp715.f1=_tmp785;});_tmp715;});struct Cyc_String_pa_PrintArg_struct _tmp161=({struct Cyc_String_pa_PrintArg_struct _tmp714;_tmp714.tag=0U,({
struct _dyneither_ptr _tmp786=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15D->name));_tmp714.f1=_tmp786;});_tmp714;});void*_tmp15E[2U];_tmp15E[0]=& _tmp160,_tmp15E[1]=& _tmp161;({struct _dyneither_ptr _tmp787=({const char*_tmp15F="%s %s was never defined.";_tag_dyneither(_tmp15F,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp787,_tag_dyneither(_tmp15E,sizeof(void*),2U));});});}
# 906
if((int)_tmp15D->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp164=_tmp15D->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15D->impl))->fields;
if(_tmp164 == 0)return Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp164))->tl != 0;_tmp164=_tmp164->tl){;}{
void*_tmp165=((struct Cyc_Absyn_Aggrfield*)_tmp164->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp165))){
if(_tmp15D->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp168=({struct Cyc_String_pa_PrintArg_struct _tmp716;_tmp716.tag=0U,({struct _dyneither_ptr _tmp788=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15D->name));_tmp716.f1=_tmp788;});_tmp716;});void*_tmp166[1U];_tmp166[0]=& _tmp168;({struct _dyneither_ptr _tmp789=({const char*_tmp167="struct %s ended up being abstract.";_tag_dyneither(_tmp167,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp789,_tag_dyneither(_tmp166,sizeof(void*),1U));});});{
# 919
struct _RegionHandle _tmp169=_new_region("r");struct _RegionHandle*r=& _tmp169;_push_region(r);
{struct Cyc_List_List*_tmp16A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15D->tvs,_tmp172);
void*_tmp16B=Cyc_Tcutil_rsubstitute(r,_tmp16A,_tmp165);
if(Cyc_Toc_is_abstract_type(_tmp16B)){void*_tmp16C=Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp16C;}{
void*_tmp16D=Cyc_Toc_add_struct_type(_tmp15D->name,_tmp15D->tvs,_tmp172,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15D->impl))->fields);_npop_handler(0U);return _tmp16D;};}
# 920
;_pop_region(r);};}
# 925
return Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_strctq);};};};case 15U: _LL1D: _tmp175=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1;_LL1E:
 return t;case 16U: _LL1F: _tmp176=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp176);return t;case 4U: _LL23: _LL24:
# 937
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 942
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp177=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp149)->f2;_LL4:
# 820
 if(*_tmp177 == 0){
*_tmp177=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 824
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp177));case 2U: _LL5: _tmp178=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->f1;_LL6:
# 826
 if((int)(Cyc_Tcutil_tvar_kind(_tmp178,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_AnyKind)
# 828
return Cyc_Absyn_void_type;else{
# 830
return Cyc_Toc_void_star_type();}case 3U: _LLD: _tmp17B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_type;_tmp17A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_tq;_tmp179=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).ptr_atts).bounds;_LLE:
# 839
 _tmp17B=Cyc_Toc_typ_to_c(_tmp17B);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp179)== 0)
return Cyc_Toc_dyneither_ptr_type();else{
# 843
return Cyc_Absyn_star_type(_tmp17B,Cyc_Absyn_heap_rgn_type,_tmp17A,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp17F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).elt_type;_tmp17E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).tq;_tmp17D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).num_elts;_tmp17C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).zt_loc;_LL14:
# 847
 return({void*_tmp78D=Cyc_Toc_typ_to_c(_tmp17F);struct Cyc_Absyn_Tqual _tmp78C=_tmp17E;struct Cyc_Absyn_Exp*_tmp78B=_tmp17D;void*_tmp78A=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp78D,_tmp78C,_tmp78B,_tmp78A,_tmp17C);});case 5U: _LL15: _tmp185=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_tqual;_tmp184=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_type;_tmp183=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).args;_tmp182=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).c_varargs;_tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).cyc_varargs;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).attributes;_LL16: {
# 853
struct Cyc_List_List*_tmp14C=0;
for(0;_tmp180 != 0;_tmp180=_tmp180->tl){
void*_tmp14D=(void*)_tmp180->hd;void*_tmp14E=_tmp14D;switch(*((int*)_tmp14E)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
 _tmp14C=({struct Cyc_List_List*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->hd=(void*)_tmp180->hd,_tmp14F->tl=_tmp14C;_tmp14F;});goto _LL35;}_LL35:;}{
# 864
struct Cyc_List_List*_tmp150=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp183);
if(_tmp181 != 0){
# 867
void*_tmp151=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp181->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp152=({struct _tuple9*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->f1=_tmp181->name,_tmp154->f2=_tmp181->tq,_tmp154->f3=_tmp151;_tmp154;});
_tmp150=({struct Cyc_List_List*_tmp78E=_tmp150;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp78E,({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=_tmp152,_tmp153->tl=0;_tmp153;}));});}
# 871
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->tag=5U,(_tmp155->f1).tvars=0,(_tmp155->f1).effect=0,(_tmp155->f1).ret_tqual=_tmp185,({void*_tmp78F=Cyc_Toc_typ_to_c(_tmp184);(_tmp155->f1).ret_type=_tmp78F;}),(_tmp155->f1).args=_tmp150,(_tmp155->f1).c_varargs=_tmp182,(_tmp155->f1).cyc_varargs=0,(_tmp155->f1).rgn_po=0,(_tmp155->f1).attributes=_tmp14C,(_tmp155->f1).requires_clause=0,(_tmp155->f1).requires_relns=0,(_tmp155->f1).ensures_clause=0,(_tmp155->f1).ensures_relns=0;_tmp155;});};}case 6U: _LL17: _tmp186=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp149)->f1;_LL18: {
# 876
struct Cyc_List_List*_tmp156=0;
for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
_tmp156=({struct Cyc_List_List*_tmp158=_cycalloc(sizeof(*_tmp158));({struct _tuple12*_tmp791=({struct _tuple12*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->f1=(*((struct _tuple12*)_tmp186->hd)).f1,({void*_tmp790=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp186->hd)).f2);_tmp157->f2=_tmp790;});_tmp157;});_tmp158->hd=_tmp791;}),_tmp158->tl=_tmp156;_tmp158;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp156));}case 7U: _LL19: _tmp188=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f1;_tmp187=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f2;_LL1A: {
# 885
struct Cyc_List_List*_tmp159=0;
for(0;_tmp187 != 0;_tmp187=_tmp187->tl){
_tmp159=({struct Cyc_List_List*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct Cyc_Absyn_Aggrfield*_tmp793=({struct Cyc_Absyn_Aggrfield*_tmp792=(struct Cyc_Absyn_Aggrfield*)_tmp187->hd;Cyc_Toc_aggrfield_to_c(_tmp792,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp187->hd)->type));});_tmp15A->hd=_tmp793;}),_tmp15A->tl=_tmp159;_tmp15A;});}
return({enum Cyc_Absyn_AggrKind _tmp794=_tmp188;Cyc_Toc_add_anon_aggr_type(_tmp794,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp159));});}case 8U: _LL21: _tmp18C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f1;_tmp18B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f2;_tmp18A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f3;_tmp189=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f4;_LL22:
# 931
 if(_tmp189 == 0){
if(_tmp18B != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->tag=8U,_tmp16E->f1=_tmp18C,_tmp16E->f2=0,_tmp16E->f3=_tmp18A,_tmp16E->f4=0;_tmp16E;});else{
return t;}}else{
# 936
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=8U,_tmp16F->f1=_tmp18C,_tmp16F->f2=0,_tmp16F->f3=_tmp18A,({void*_tmp795=Cyc_Toc_typ_to_c(_tmp189);_tmp16F->f4=_tmp795;});_tmp16F;});}case 9U: _LL2B: _tmp18D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp149)->f1;_LL2C:
# 947
 return t;case 11U: _LL2D: _tmp18E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp149)->f1;_LL2E:
# 952
 if(_tmp18E->topt != 0)return Cyc_Toc_typ_to_c((void*)_check_null(_tmp18E->topt));else{
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)){case 0U: _LL2F: _tmp18F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL30:
# 955
 Cyc_Toc_aggrdecl_to_c(_tmp18F);
if((int)_tmp18F->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_unionq_type);else{
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp190=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL32:
# 960
 Cyc_Toc_enumdecl_to_c(_tmp190);
return t;default: _LL33: _tmp192=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_tmp191=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f2;_LL34:
# 963
 Cyc_Toc_datatypedecl_to_c(_tmp192);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp191)));}}_LL0:;}
# 968
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp193=t;void*_tmp195;struct Cyc_Absyn_Tqual _tmp194;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->tag == 4U){_LL1: _tmp195=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).elt_type;_tmp194=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).tq;_LL2:
# 971
 return({void*_tmp796=Cyc_Absyn_star_type(_tmp195,Cyc_Absyn_heap_rgn_type,_tmp194,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp796,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 978
static int Cyc_Toc_atomic_type(void*t){
void*_tmp196=Cyc_Tcutil_compress(t);void*_tmp197=_tmp196;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*_tmp1A9;void*_tmp1A8;void*_tmp1A7;struct Cyc_List_List*_tmp1A6;switch(*((int*)_tmp197)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp1A7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f1;_tmp1A6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2;_LL4: {
# 982
void*_tmp198=_tmp1A7;struct Cyc_Absyn_Datatypedecl*_tmp1A2;struct Cyc_Absyn_Datatypefield*_tmp1A1;union Cyc_Absyn_AggrInfo _tmp1A0;switch(*((int*)_tmp198)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D:
 return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp1A0=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp198)->f1;_LL25:
# 990
{union Cyc_Absyn_AggrInfo _tmp199=_tmp1A0;if((_tmp199.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 994
struct Cyc_Absyn_Aggrdecl*_tmp19A=Cyc_Absyn_get_known_aggrdecl(_tmp1A0);
if(_tmp19A->impl == 0)
return 0;
{struct Cyc_List_List*_tmp19B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp19A->impl))->fields;for(0;_tmp19B != 0;_tmp19B=_tmp19B->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp19B->hd)->type))return 0;}}
return 1;};case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp1A2=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownDatatypefield).val).f1;_tmp1A1=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownDatatypefield).val).f2;_LL27:
# 1001
{struct Cyc_List_List*_tmp19C=_tmp1A1->typs;for(0;_tmp19C != 0;_tmp19C=_tmp19C->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp19C->hd)).f2))return 0;}}
return 1;}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp19F=({struct Cyc_String_pa_PrintArg_struct _tmp717;_tmp717.tag=0U,({struct _dyneither_ptr _tmp797=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp717.f1=_tmp797;});_tmp717;});void*_tmp19D[1U];_tmp19D[0]=& _tmp19F;({struct _dyneither_ptr _tmp798=({const char*_tmp19E="atomic_typ:  bad type %s";_tag_dyneither(_tmp19E,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp798,_tag_dyneither(_tmp19D,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 1006
 return 1;case 4U: _LL7: _tmp1A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->f1).elt_type;_LL8:
 return Cyc_Toc_atomic_type(_tmp1A8);case 7U: _LL9: _tmp1A9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp197)->f2;_LLA:
# 1014
 for(0;_tmp1A9 != 0;_tmp1A9=_tmp1A9->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp1A9->hd)->type))return 0;}
return 1;case 6U: _LLB: _tmp1AA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp197)->f1;_LLC:
# 1018
 for(0;_tmp1AA != 0;_tmp1AA=_tmp1AA->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp1AA->hd)).f2))return 0;}
return 1;case 3U: _LLD: _LLE:
# 1023
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp1A5=({struct Cyc_String_pa_PrintArg_struct _tmp718;_tmp718.tag=0U,({struct _dyneither_ptr _tmp799=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp718.f1=_tmp799;});_tmp718;});void*_tmp1A3[1U];_tmp1A3[0]=& _tmp1A5;({struct _dyneither_ptr _tmp79A=({const char*_tmp1A4="atomic_typ:  bad type %s";_tag_dyneither(_tmp1A4,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79A,_tag_dyneither(_tmp1A3,sizeof(void*),1U));});});}_LL0:;}
# 1029
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1AB=Cyc_Tcutil_compress(t);void*_tmp1AC=_tmp1AB;struct Cyc_List_List*_tmp1B8;union Cyc_Absyn_AggrInfo _tmp1B7;switch(*((int*)_tmp1AC)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->tag == 20U){_LL1: _tmp1B7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->f1;_LL2: {
# 1032
struct Cyc_Absyn_Aggrdecl*_tmp1AD=Cyc_Absyn_get_known_aggrdecl(_tmp1B7);
if(_tmp1AD->impl == 0)
({void*_tmp1AE=0U;({struct _dyneither_ptr _tmp79B=({const char*_tmp1AF="is_poly_field: type missing fields";_tag_dyneither(_tmp1AF,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79B,_tag_dyneither(_tmp1AE,sizeof(void*),0U));});});
_tmp1B8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AD->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1B8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AC)->f2;_LL4: {
# 1037
struct Cyc_Absyn_Aggrfield*_tmp1B0=Cyc_Absyn_lookup_field(_tmp1B8,f);
if(_tmp1B0 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B3=({struct Cyc_String_pa_PrintArg_struct _tmp719;_tmp719.tag=0U,_tmp719.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp719;});void*_tmp1B1[1U];_tmp1B1[0]=& _tmp1B3;({struct _dyneither_ptr _tmp79C=({const char*_tmp1B2="is_poly_field: bad field %s";_tag_dyneither(_tmp1B2,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79C,_tag_dyneither(_tmp1B1,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1B0->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B6=({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0U,({struct _dyneither_ptr _tmp79D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp71A.f1=_tmp79D;});_tmp71A;});void*_tmp1B4[1U];_tmp1B4[0]=& _tmp1B6;({struct _dyneither_ptr _tmp79E=({const char*_tmp1B5="is_poly_field: bad type %s";_tag_dyneither(_tmp1B5,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79E,_tag_dyneither(_tmp1B4,sizeof(void*),1U));});});}_LL0:;}
# 1048
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B9=e->r;void*_tmp1BA=_tmp1B9;struct Cyc_Absyn_Exp*_tmp1C4;struct _dyneither_ptr*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;switch(*((int*)_tmp1BA)){case 21U: _LL1: _tmp1C2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1BA)->f1;_tmp1C1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1BA)->f2;_LL2:
# 1051
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C2->topt),_tmp1C1)&& !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1BA)->f1;_tmp1C3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1BA)->f2;_LL4: {
# 1054
void*_tmp1BB=Cyc_Tcutil_compress((void*)_check_null(_tmp1C4->topt));void*_tmp1BC=_tmp1BB;void*_tmp1C0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BC)->tag == 3U){_LL8: _tmp1C0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BC)->f1).elt_type;_LL9:
# 1056
 return Cyc_Toc_is_poly_field(_tmp1C0,_tmp1C3)&& !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BF=({struct Cyc_String_pa_PrintArg_struct _tmp71B;_tmp71B.tag=0U,({struct _dyneither_ptr _tmp79F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C4->topt)));_tmp71B.f1=_tmp79F;});_tmp71B;});void*_tmp1BD[1U];_tmp1BD[0]=& _tmp1BF;({struct _dyneither_ptr _tmp7A0=({const char*_tmp1BE="is_poly_project: bad type %s";_tag_dyneither(_tmp1BE,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7A0,_tag_dyneither(_tmp1BD,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1059
 return 0;}_LL0:;}
# 1064
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C5[1U];_tmp1C5[0]=s;({struct Cyc_Absyn_Exp*_tmp7A1=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7A1,_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1069
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=s,_tmp1C6[1]=n;({struct Cyc_Absyn_Exp*_tmp7A2=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7A2,_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1074
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=rgn,_tmp1C7[1]=s;({struct Cyc_Absyn_Exp*_tmp7A3=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp7A3,_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1078
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=rgn,_tmp1C8[1]=s;({struct Cyc_Absyn_Exp*_tmp7A4=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp7A4,_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1082
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1C9[3U];_tmp1C9[0]=rgn,_tmp1C9[1]=s,_tmp1C9[2]=n;({struct Cyc_Absyn_Exp*_tmp7A5=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp7A5,_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1086
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({void*_tmp1CA=0U;({struct Cyc_Absyn_Exp*_tmp7A6=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp7A6,_tag_dyneither(_tmp1CA,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1091
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1099
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1CB=e->r;void*_tmp1CC=_tmp1CB;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CC)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CC)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1104
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));({struct Cyc_Absyn_Decl*_tmp7A7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->tag=0U,_tmp1CD->f1=vd;_tmp1CD;}),0U);_tmp1CE->hd=_tmp7A7;}),_tmp1CE->tl=Cyc_Toc_result_decls;_tmp1CE;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1112
 xexp=({void*_tmp7A8=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp7A8,e);});
# 1114
xplussz=({void*_tmp7A9=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp7A9,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1117
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1CF[3U];({struct _tuple19*_tmp7AC=({struct _tuple19*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->f1=0,_tmp1D0->f2=xexp;_tmp1D0;});_tmp1CF[0]=_tmp7AC;}),({
struct _tuple19*_tmp7AB=({struct _tuple19*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->f1=0,_tmp1D1->f2=xexp;_tmp1D1;});_tmp1CF[1]=_tmp7AB;}),({
struct _tuple19*_tmp7AA=({struct _tuple19*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xplussz;_tmp1D2;});_tmp1CF[2]=_tmp7AA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CF,sizeof(struct _tuple19*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1152 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1D4=_region_malloc(r,sizeof(*_tmp1D4));_tmp1D4->break_lab=0,_tmp1D4->continue_lab=0,_tmp1D4->fallthru_info=0,_tmp1D4->toplevel=1,({int*_tmp7AD=({int*_tmp1D3=_region_malloc(r,sizeof(*_tmp1D3));*_tmp1D3=0;_tmp1D3;});_tmp1D4->in_lhs=_tmp7AD;}),_tmp1D4->rgn=r;_tmp1D4;});}
# 1156
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D5=nv;int _tmp1D6;_LL1: _tmp1D6=_tmp1D5->toplevel;_LL2:;
return _tmp1D6;}
# 1160
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D7=e;struct _dyneither_ptr**_tmp1DD;struct _dyneither_ptr**_tmp1DC;struct Cyc_Toc_FallthruInfo*_tmp1DB;int _tmp1DA;int*_tmp1D9;_LL1: _tmp1DD=_tmp1D7->break_lab;_tmp1DC=_tmp1D7->continue_lab;_tmp1DB=_tmp1D7->fallthru_info;_tmp1DA=_tmp1D7->toplevel;_tmp1D9=_tmp1D7->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1D8=_region_malloc(r,sizeof(*_tmp1D8));_tmp1D8->break_lab=_tmp1DD,_tmp1D8->continue_lab=_tmp1DC,_tmp1D8->fallthru_info=_tmp1DB,_tmp1D8->toplevel=0,_tmp1D8->in_lhs=_tmp1D9,_tmp1D8->rgn=r;_tmp1D8;});}
# 1164
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DE=e;struct _dyneither_ptr**_tmp1E4;struct _dyneither_ptr**_tmp1E3;struct Cyc_Toc_FallthruInfo*_tmp1E2;int _tmp1E1;int*_tmp1E0;_LL1: _tmp1E4=_tmp1DE->break_lab;_tmp1E3=_tmp1DE->continue_lab;_tmp1E2=_tmp1DE->fallthru_info;_tmp1E1=_tmp1DE->toplevel;_tmp1E0=_tmp1DE->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DF=_region_malloc(r,sizeof(*_tmp1DF));_tmp1DF->break_lab=_tmp1E4,_tmp1DF->continue_lab=_tmp1E3,_tmp1DF->fallthru_info=_tmp1E2,_tmp1DF->toplevel=1,_tmp1DF->in_lhs=_tmp1E0,_tmp1DF->rgn=r;_tmp1DF;});}
# 1168
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1E5=nv;int*_tmp1E6;_LL1: _tmp1E6=_tmp1E5->in_lhs;_LL2:;
return*_tmp1E6;}
# 1172
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E7=e;int*_tmp1E8;_LL1: _tmp1E8=_tmp1E7->in_lhs;_LL2:;
*_tmp1E8=b;}
# 1177
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E9=e;struct _dyneither_ptr**_tmp1EF;struct _dyneither_ptr**_tmp1EE;struct Cyc_Toc_FallthruInfo*_tmp1ED;int _tmp1EC;int*_tmp1EB;_LL1: _tmp1EF=_tmp1E9->break_lab;_tmp1EE=_tmp1E9->continue_lab;_tmp1ED=_tmp1E9->fallthru_info;_tmp1EC=_tmp1E9->toplevel;_tmp1EB=_tmp1E9->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1EA=_region_malloc(r,sizeof(*_tmp1EA));_tmp1EA->break_lab=_tmp1EF,_tmp1EA->continue_lab=_tmp1EE,_tmp1EA->fallthru_info=_tmp1ED,_tmp1EA->toplevel=_tmp1EC,_tmp1EA->in_lhs=_tmp1EB,_tmp1EA->rgn=r;_tmp1EA;});}
# 1184
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F0=e;struct _dyneither_ptr**_tmp1F6;struct _dyneither_ptr**_tmp1F5;struct Cyc_Toc_FallthruInfo*_tmp1F4;int _tmp1F3;int*_tmp1F2;_LL1: _tmp1F6=_tmp1F0->break_lab;_tmp1F5=_tmp1F0->continue_lab;_tmp1F4=_tmp1F0->fallthru_info;_tmp1F3=_tmp1F0->toplevel;_tmp1F2=_tmp1F0->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));_tmp1F1->break_lab=0,_tmp1F1->continue_lab=0,_tmp1F1->fallthru_info=_tmp1F4,_tmp1F1->toplevel=_tmp1F3,_tmp1F1->in_lhs=_tmp1F2,_tmp1F1->rgn=r;_tmp1F1;});}
# 1190
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1194
struct Cyc_Toc_Env*_tmp1F7=e;struct _dyneither_ptr**_tmp1FF;struct _dyneither_ptr**_tmp1FE;struct Cyc_Toc_FallthruInfo*_tmp1FD;int _tmp1FC;int*_tmp1FB;_LL1: _tmp1FF=_tmp1F7->break_lab;_tmp1FE=_tmp1F7->continue_lab;_tmp1FD=_tmp1F7->fallthru_info;_tmp1FC=_tmp1F7->toplevel;_tmp1FB=_tmp1F7->in_lhs;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));
_tmp1FA->label=fallthru_l,_tmp1FA->binders=fallthru_binders;_tmp1FA;});
return({struct Cyc_Toc_Env*_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));({struct _dyneither_ptr**_tmp7AE=({struct _dyneither_ptr**_tmp1F8=_region_malloc(r,sizeof(*_tmp1F8));*_tmp1F8=break_l;_tmp1F8;});_tmp1F9->break_lab=_tmp7AE;}),_tmp1F9->continue_lab=_tmp1FE,_tmp1F9->fallthru_info=fi,_tmp1F9->toplevel=_tmp1FC,_tmp1F9->in_lhs=_tmp1FB,_tmp1F9->rgn=r;_tmp1F9;});};}
# 1199
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1201
struct Cyc_Toc_Env*_tmp200=e;struct _dyneither_ptr**_tmp207;struct _dyneither_ptr**_tmp206;struct Cyc_Toc_FallthruInfo*_tmp205;int _tmp204;int*_tmp203;_LL1: _tmp207=_tmp200->break_lab;_tmp206=_tmp200->continue_lab;_tmp205=_tmp200->fallthru_info;_tmp204=_tmp200->toplevel;_tmp203=_tmp200->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp202=_region_malloc(r,sizeof(*_tmp202));({struct _dyneither_ptr**_tmp7AF=({struct _dyneither_ptr**_tmp201=_region_malloc(r,sizeof(*_tmp201));*_tmp201=break_l;_tmp201;});_tmp202->break_lab=_tmp7AF;}),_tmp202->continue_lab=_tmp206,_tmp202->fallthru_info=0,_tmp202->toplevel=_tmp204,_tmp202->in_lhs=_tmp203,_tmp202->rgn=r;_tmp202;});}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1211
struct Cyc_Toc_Env*_tmp208=e;struct _dyneither_ptr**_tmp20F;struct _dyneither_ptr**_tmp20E;struct Cyc_Toc_FallthruInfo*_tmp20D;int _tmp20C;int*_tmp20B;_LL1: _tmp20F=_tmp208->break_lab;_tmp20E=_tmp208->continue_lab;_tmp20D=_tmp208->fallthru_info;_tmp20C=_tmp208->toplevel;_tmp20B=_tmp208->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp20A=_region_malloc(r,sizeof(*_tmp20A));_tmp20A->break_lab=0,_tmp20A->continue_lab=_tmp20E,({struct Cyc_Toc_FallthruInfo*_tmp7B0=({struct Cyc_Toc_FallthruInfo*_tmp209=_region_malloc(r,sizeof(*_tmp209));_tmp209->label=next_l,_tmp209->binders=0;_tmp209;});_tmp20A->fallthru_info=_tmp7B0;}),_tmp20A->toplevel=_tmp20C,_tmp20A->in_lhs=_tmp20B,_tmp20A->rgn=r;_tmp20A;});}
# 1224 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1227
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp210=e->annot;void*_tmp211=_tmp210;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({void*_tmp212=0U;({unsigned _tmp7B2=e->loc;struct _dyneither_ptr _tmp7B1=({const char*_tmp213="Toc: do_null_check";_tag_dyneither(_tmp213,sizeof(char),19U);});((int(*)(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp7B2,_tmp7B1,_tag_dyneither(_tmp212,sizeof(void*),0U));});});}}}}}}_LL0:;}
# 1243
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1245
int ans;
int _tmp214=Cyc_Toc_in_lhs(nv);
void*_tmp215=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));
void*_tmp216=Cyc_Toc_typ_to_c(_tmp215);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp217=_tmp215;void*_tmp22F;struct Cyc_Absyn_Tqual _tmp22E;void*_tmp22D;void*_tmp22C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->tag == 3U){_LL1: _tmp22F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).elt_type;_tmp22E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).elt_tq;_tmp22D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).ptr_atts).bounds;_tmp22C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).ptr_atts).zero_term;_LL2:
# 1256
{void*_tmp218=annot;struct Cyc_Absyn_Exp*_tmp229;struct Cyc_Absyn_Exp*_tmp228;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1258
 if(!((unsigned)({void*_tmp7B3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7B3,_tmp22D);}))){
# 1261
void*_tmp219=({void*_tmp7B4=Cyc_Toc_typ_to_c(_tmp22F);Cyc_Absyn_cstar_type(_tmp7B4,_tmp22E);});
({void*_tmp7B7=({void*_tmp7B6=_tmp219;Cyc_Toc_cast_it_r(_tmp7B6,({struct Cyc_Absyn_Exp*_tmp7B5=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7B5,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp7B7;});
ptr->topt=_tmp219;}
# 1265
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1268
 if(!((unsigned)({void*_tmp7B8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7B8,_tmp22D);}))){
# 1270
void*_tmp21A=({void*_tmp7B9=Cyc_Toc_typ_to_c(_tmp22F);Cyc_Absyn_cstar_type(_tmp7B9,_tmp22E);});
({void*_tmp7BC=({void*_tmp7BB=_tmp21A;Cyc_Toc_cast_it_r(_tmp7BB,({struct Cyc_Absyn_Exp*_tmp7BA=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7BA,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp7BC;});
ptr->topt=_tmp21A;
# 1277
if(index != 0)
({void*_tmp21B=0U;({struct _dyneither_ptr _tmp7BD=({const char*_tmp21C="subscript of ? with no bounds check but need null check";_tag_dyneither(_tmp21C,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7BD,_tag_dyneither(_tmp21B,sizeof(void*),0U));});});}
# 1281
({void*_tmp7C1=({void*_tmp7C0=_tmp216;Cyc_Toc_cast_it_r(_tmp7C0,({struct Cyc_Absyn_Exp*_tmp21D[1U];({struct Cyc_Absyn_Exp*_tmp7BE=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21D[0]=_tmp7BE;});({struct Cyc_Absyn_Exp*_tmp7BF=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp7BF,_tag_dyneither(_tmp21D,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp7C1;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1287
void*ta1=Cyc_Toc_typ_to_c(_tmp22F);
void*newt=Cyc_Absyn_cstar_type(ta1,_tmp22E);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp7C6=({void*_tmp7C5=newt;Cyc_Toc_cast_it_r(_tmp7C5,({struct Cyc_Absyn_Exp*_tmp21E[3U];({
# 1292
struct Cyc_Absyn_Exp*_tmp7C3=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21E[0]=_tmp7C3;}),({
struct Cyc_Absyn_Exp*_tmp7C2=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp21E[1]=_tmp7C2;}),_tmp21E[2]=ind;({struct Cyc_Absyn_Exp*_tmp7C4=Cyc_Toc__check_dyneither_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp7C4,_tag_dyneither(_tmp21E,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1290
ptr->r=_tmp7C6;});
# 1295
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp228=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp218)->f1;_LLF:
# 1299
 fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp229=_tmp228;goto _LL11;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp229=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp218)->f1;_LL11: {
# 1302
void*ta1=Cyc_Toc_typ_to_c(_tmp22F);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1307
struct _tuple13 _tmp21F=Cyc_Evexp_eval_const_uint_exp(_tmp229);struct _tuple13 _tmp220=_tmp21F;unsigned _tmp225;int _tmp224;_LL15: _tmp225=_tmp220.f1;_tmp224=_tmp220.f2;_LL16:;
if((!_tmp224 || _tmp225 != (unsigned)1)|| !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1310
({void*_tmp7CB=({void*_tmp7CA=Cyc_Absyn_cstar_type(ta1,_tmp22E);Cyc_Toc_cast_it_r(_tmp7CA,({struct Cyc_Absyn_Exp*_tmp221[4U];({
struct Cyc_Absyn_Exp*_tmp7C8=Cyc_Absyn_new_exp(ptr->r,0U);_tmp221[0]=_tmp7C8;}),_tmp221[1]=_tmp229,({
struct Cyc_Absyn_Exp*_tmp7C7=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp221[2]=_tmp7C7;}),_tmp221[3]=ind;({struct Cyc_Absyn_Exp*_tmp7C9=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7C9,_tag_dyneither(_tmp221,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1310
ptr->r=_tmp7CB;});
# 1313
ans=1;}else{
# 1316
if(Cyc_Toc_is_zero(_tmp229)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1319
({void*_tmp7CF=({void*_tmp7CE=_tmp216;Cyc_Toc_cast_it_r(_tmp7CE,({struct Cyc_Absyn_Exp*_tmp222[1U];({struct Cyc_Absyn_Exp*_tmp7CC=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp222[0]=_tmp7CC;});({struct Cyc_Absyn_Exp*_tmp7CD=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp7CD,_tag_dyneither(_tmp222,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1319
ptr->r=_tmp7CF;});
# 1321
ans=0;}else{
# 1325
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp7D3=({void*_tmp7D2=Cyc_Absyn_cstar_type(ta1,_tmp22E);Cyc_Toc_cast_it_r(_tmp7D2,({struct Cyc_Absyn_Exp*_tmp223[3U];({
struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Absyn_new_exp(ptr->r,0U);_tmp223[0]=_tmp7D0;}),_tmp223[1]=_tmp229,_tmp223[2]=ind;({struct Cyc_Absyn_Exp*_tmp7D1=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7D1,_tag_dyneither(_tmp223,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1326
ptr->r=_tmp7D3;});
# 1328
ans=1;}}
# 1331
goto _LL5;}}else{_LL12: _LL13:
({void*_tmp226=0U;({struct _dyneither_ptr _tmp7D4=({const char*_tmp227="FIX: ptr_use_to_c, bad annotation";_tag_dyneither(_tmp227,sizeof(char),34U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp7D4,_tag_dyneither(_tmp226,sizeof(void*),0U));});});}}}}}}_LL5:;}
# 1334
Cyc_Toc_set_lhs(nv,_tmp214);
return ans;}else{_LL3: _LL4:
({void*_tmp22A=0U;({struct _dyneither_ptr _tmp7D5=({const char*_tmp22B="ptr_use_to_c: non-pointer-type";_tag_dyneither(_tmp22B,sizeof(char),31U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp7D5,_tag_dyneither(_tmp22A,sizeof(void*),0U));});});}_LL0:;};}
# 1340
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp230=0U;({struct _dyneither_ptr _tmp7D6=({const char*_tmp231="Missing type in primop ";_tag_dyneither(_tmp231,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7D6,_tag_dyneither(_tmp230,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1344
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->f1=Cyc_Toc_mt_tq,({void*_tmp7D7=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp232->f2=_tmp7D7;});_tmp232;});}
# 1349
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp233=e->r;void*_tmp234=_tmp233;struct Cyc_Absyn_Exp*_tmp23C;int _tmp23B;struct Cyc_Absyn_Exp*_tmp23A;int _tmp239;struct Cyc_List_List*_tmp238;switch(*((int*)_tmp234)){case 26U: _LL1: _tmp238=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp234)->f1;_LL2:
# 1352
{struct Cyc_List_List*_tmp235=_tmp238;for(0;_tmp235 != 0;_tmp235=_tmp235->tl){
if((*((struct _tuple19*)_tmp235->hd)).f1 != 0)
({void*_tmp236=0U;({struct _dyneither_ptr _tmp7D8=({const char*_tmp237="array designators for abstract-field initialization";_tag_dyneither(_tmp237,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp7D8,_tag_dyneither(_tmp236,sizeof(void*),0U));});});}}
_tmp23A=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp238),0U);_tmp239=0;goto _LL4;case 27U: _LL3: _tmp23A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp234)->f2;_tmp239=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp234)->f4;_LL4:
 _tmp23C=_tmp23A;_tmp23B=_tmp239;goto _LL6;case 28U: _LL5: _tmp23C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp234)->f1;_tmp23B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp234)->f3;_LL6:
# 1358
 _tmp23C=Cyc_Absyn_copy_exp(_tmp23C);
return _tmp23B?({struct Cyc_Absyn_Exp*_tmp7D9=_tmp23C;Cyc_Absyn_add_exp(_tmp7D9,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp23C;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1367
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1375
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp23D=e->r;void*_tmp23E=_tmp23D;struct Cyc_List_List*_tmp23F;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23E)->tag == 29U){_LL1: _tmp23F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23E)->f3;_LL2:
 dles=_tmp23F;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1381
{void*_tmp240=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp241=_tmp240;struct Cyc_Absyn_Aggrdecl*_tmp242;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp242=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f1)->f1).KnownAggr).val;_LL7:
# 1383
 if(_tmp242->impl == 0)
return 0;
if((int)_tmp242->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp242->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1391
 return 0;}_LL5:;}
# 1393
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp243=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp244=(struct _tuple19*)dles->hd;struct _tuple19*_tmp245=_tmp244;struct Cyc_List_List*_tmp24E;struct Cyc_Absyn_Exp*_tmp24D;_LLB: _tmp24E=_tmp245->f1;_tmp24D=_tmp245->f2;_LLC:;{
struct _dyneither_ptr*_tmp246=Cyc_Absyn_designatorlist_to_fieldname(_tmp24E);
if(!Cyc_strptrcmp(_tmp246,_tmp243->name)){
struct Cyc_Absyn_Exp*_tmp247=Cyc_Toc_get_varsizeexp(_tmp24D);
if(_tmp247 != 0)
return _tmp247;{
void*_tmp248=Cyc_Tcutil_compress(_tmp243->type);void*_tmp249=_tmp248;void*_tmp24C;struct Cyc_Absyn_Exp*_tmp24B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249)->tag == 4U){_LLE: _tmp24C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249)->f1).elt_type;_tmp24B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249)->f1).num_elts;_LLF:
# 1408
 if(_tmp24B == 0 || !Cyc_Toc_is_zero(_tmp24B))
return 0;
# 1414
return({struct Cyc_Absyn_Exp*_tmp7DC=({struct Cyc_Absyn_Exp*_tmp24A[2U];_tmp24A[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp24D)),({
# 1416
struct Cyc_Absyn_Exp*_tmp7DA=Cyc_Absyn_sizeoftype_exp(_tmp24C,0U);_tmp24A[1]=_tmp7DA;});({struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp7DB,_tag_dyneither(_tmp24A,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1414
Cyc_Absyn_add_exp(_tmp7DC,
# 1417
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1422
({void*_tmp24F=0U;({struct _dyneither_ptr _tmp7DD=({const char*_tmp250="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp250,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7DD,_tag_dyneither(_tmp24F,sizeof(void*),0U));});});};}
# 1425
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp251=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp251 != 0;_tmp251=_tmp251->tl){
struct Cyc_Absyn_Aggrfield*_tmp252=(struct Cyc_Absyn_Aggrfield*)_tmp251->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp252->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1432
({void*_tmp253=0U;({struct _dyneither_ptr _tmp7DF=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp256=({struct Cyc_String_pa_PrintArg_struct _tmp71C;_tmp71C.tag=0U,_tmp71C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp71C;});void*_tmp254[1U];_tmp254[0]=& _tmp256;({struct _dyneither_ptr _tmp7DE=({const char*_tmp255="get_member_offset %s failed";_tag_dyneither(_tmp255,sizeof(char),28U);});Cyc_aprintf(_tmp7DE,_tag_dyneither(_tmp254,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7DF,_tag_dyneither(_tmp253,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1436
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1439
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1442
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->tag=5U,_tmp257->f1=e1,_tmp257->f2=incr;_tmp257;}),0U);}
# 1446
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1455
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1457
void*_tmp258=e1->r;void*_tmp259=_tmp258;struct Cyc_Absyn_Exp*_tmp261;struct _dyneither_ptr*_tmp260;int _tmp25F;int _tmp25E;void*_tmp25D;struct Cyc_Absyn_Exp*_tmp25C;struct Cyc_Absyn_Stmt*_tmp25B;switch(*((int*)_tmp259)){case 37U: _LL1: _tmp25B=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp259)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp25B,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp25D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp259)->f1;_tmp25C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp259)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp25C,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp261=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp259)->f1;_tmp260=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp259)->f2;_tmp25F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp259)->f3;_tmp25E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp259)->f4;_LL6:
# 1462
 e1->r=_tmp261->r;
({struct Cyc_Absyn_Exp*_tmp7E2=e1;struct Cyc_List_List*_tmp7E1=({struct Cyc_List_List*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->hd=_tmp260,_tmp25A->tl=fs;_tmp25A;});struct Cyc_Absyn_Exp*(*_tmp7E0)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp7E2,_tmp7E1,_tmp7E0,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1470
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1472
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp7E3=(f(e1_copy,f_env))->r;e1->r=_tmp7E3;});
goto _LL0;}}_LL0:;}
# 1478
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1480
void*_tmp262=s->r;void*_tmp263=_tmp262;struct Cyc_Absyn_Stmt*_tmp26A;struct Cyc_Absyn_Decl*_tmp269;struct Cyc_Absyn_Stmt*_tmp268;struct Cyc_Absyn_Exp*_tmp267;switch(*((int*)_tmp263)){case 1U: _LL1: _tmp267=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp263)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp267,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp269=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp263)->f1;_tmp268=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp263)->f2;_LL4:
# 1483
 Cyc_Toc_lvalue_assign_stmt(_tmp268,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp26A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp263)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp26A,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp266=({struct Cyc_String_pa_PrintArg_struct _tmp71D;_tmp71D.tag=0U,({struct _dyneither_ptr _tmp7E4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp71D.f1=_tmp7E4;});_tmp71D;});void*_tmp264[1U];_tmp264[0]=& _tmp266;({struct _dyneither_ptr _tmp7E5=({const char*_tmp265="lvalue_assign_stmt: %s";_tag_dyneither(_tmp265,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp7E5,_tag_dyneither(_tmp264,sizeof(void*),1U));});});}_LL0:;}
# 1489
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1493
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp26B=e->r;void*_tmp26C=_tmp26B;struct Cyc_Absyn_Stmt*_tmp273;struct Cyc_Absyn_Exp*_tmp272;void**_tmp271;struct Cyc_Absyn_Exp**_tmp270;switch(*((int*)_tmp26C)){case 14U: _LL1: _tmp271=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp270=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL2:
# 1496
({struct Cyc_Absyn_Exp*_tmp7E6=Cyc_Toc_push_address_exp(*_tmp270);*_tmp270=_tmp7E6;});
({void*_tmp7E7=Cyc_Absyn_cstar_type(*_tmp271,Cyc_Toc_mt_tq);*_tmp271=_tmp7E7;});
return e;case 20U: _LL3: _tmp272=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL4:
# 1500
 return _tmp272;case 37U: _LL5: _tmp273=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL6:
# 1504
 Cyc_Toc_push_address_stmt(_tmp273);
return e;default: _LL7: _LL8:
# 1507
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp26F=({struct Cyc_String_pa_PrintArg_struct _tmp71E;_tmp71E.tag=0U,({struct _dyneither_ptr _tmp7E8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp71E.f1=_tmp7E8;});_tmp71E;});void*_tmp26D[1U];_tmp26D[0]=& _tmp26F;({struct _dyneither_ptr _tmp7E9=({const char*_tmp26E="can't take & of exp %s";_tag_dyneither(_tmp26E,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7E9,_tag_dyneither(_tmp26D,sizeof(void*),1U));});});}_LL0:;}
# 1511
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp274=s->r;void*_tmp275=_tmp274;struct Cyc_Absyn_Exp**_tmp27B;struct Cyc_Absyn_Stmt*_tmp27A;struct Cyc_Absyn_Stmt*_tmp279;switch(*((int*)_tmp275)){case 2U: _LL1: _tmp279=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp275)->f2;_LL2:
 _tmp27A=_tmp279;goto _LL4;case 12U: _LL3: _tmp27A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp275)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp27A);goto _LL0;case 1U: _LL5: _tmp27B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp275)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp7EA=Cyc_Toc_push_address_exp(*_tmp27B);*_tmp27B=_tmp7EA;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp278=({struct Cyc_String_pa_PrintArg_struct _tmp71F;_tmp71F.tag=0U,({struct _dyneither_ptr _tmp7EB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp71F.f1=_tmp7EB;});_tmp71F;});void*_tmp276[1U];_tmp276[0]=& _tmp278;({struct _dyneither_ptr _tmp7EC=({const char*_tmp277="can't take & of stmt %s";_tag_dyneither(_tmp277,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7EC,_tag_dyneither(_tmp276,sizeof(void*),1U));});});}_LL0:;}
# 1523
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1535
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp27C=Cyc_Toc_typ_to_c(elt_type);
void*_tmp27D=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp27E=Cyc_Absyn_cstar_type(_tmp27C,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp27F=({struct Cyc_Core_Opt*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->v=_tmp27E;_tmp2B1;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp280=e1->r;void*_tmp281=_tmp280;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Exp*_tmp286;struct Cyc_Absyn_Exp*_tmp285;switch(*((int*)_tmp281)){case 20U: _LL1: _tmp285=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp281)->f1;_LL2:
# 1543
 if(!is_dyneither){
_tmp285=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp285,0,Cyc_Absyn_Other_coercion,0U);
_tmp285->topt=fat_ptr_type;
_tmp285->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1548
Cyc_Toc_exp_to_c(nv,_tmp285);xinit=_tmp285;goto _LL0;case 23U: _LL3: _tmp287=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp281)->f1;_tmp286=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp281)->f2;_LL4:
# 1550
 if(!is_dyneither){
_tmp287=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp287,0,Cyc_Absyn_Other_coercion,0U);
_tmp287->topt=fat_ptr_type;
_tmp287->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1555
Cyc_Toc_exp_to_c(nv,_tmp287);Cyc_Toc_exp_to_c(nv,_tmp286);
xinit=({struct Cyc_Absyn_Exp*_tmp282[3U];_tmp282[0]=_tmp287,({
struct Cyc_Absyn_Exp*_tmp7ED=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp282[1]=_tmp7ED;}),_tmp282[2]=_tmp286;({struct Cyc_Absyn_Exp*_tmp7EE=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp7EE,_tag_dyneither(_tmp282,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;default: _LL5: _LL6:
({void*_tmp283=0U;({struct _dyneither_ptr _tmp7EF=({const char*_tmp284="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp284,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7EF,_tag_dyneither(_tmp283,sizeof(void*),0U));});});}_LL0:;}{
# 1561
struct _tuple1*_tmp288=Cyc_Toc_temp_var();
struct _RegionHandle _tmp289=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp289;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp28A=({struct Cyc_Absyn_Vardecl*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));_tmp2B0->sc=Cyc_Absyn_Public,_tmp2B0->name=_tmp288,_tmp2B0->varloc=0U,_tmp2B0->tq=Cyc_Toc_mt_tq,_tmp2B0->type=_tmp27D,_tmp2B0->initializer=xinit,_tmp2B0->rgn=0,_tmp2B0->attributes=0,_tmp2B0->escapes=0;_tmp2B0;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp28B=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));_tmp2AF->tag=4U,_tmp2AF->f1=_tmp28A;_tmp2AF;});
struct Cyc_Absyn_Exp*_tmp28C=Cyc_Absyn_varb_exp((void*)_tmp28B,0U);
_tmp28C->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp28D=Cyc_Absyn_deref_exp(_tmp28C,0U);
_tmp28D->topt=elt_type;
_tmp28D->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,_tmp28D);{
struct _tuple1*_tmp28E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp28F=({struct Cyc_Absyn_Vardecl*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE->sc=Cyc_Absyn_Public,_tmp2AE->name=_tmp28E,_tmp2AE->varloc=0U,_tmp2AE->tq=Cyc_Toc_mt_tq,_tmp2AE->type=_tmp27C,_tmp2AE->initializer=_tmp28D,_tmp2AE->rgn=0,_tmp2AE->attributes=0,_tmp2AE->escapes=0;_tmp2AE;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp290=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->tag=4U,_tmp2AD->f1=_tmp28F;_tmp2AD;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp291=Cyc_Absyn_varb_exp((void*)_tmp290,0U);
_tmp291->topt=_tmp28D->topt;
z_init=({enum Cyc_Absyn_Primop _tmp7F1=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp7F0=_tmp291;Cyc_Absyn_prim2_exp(_tmp7F1,_tmp7F0,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=_tmp291->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1582
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp292=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp293=({struct Cyc_Absyn_Vardecl*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->sc=Cyc_Absyn_Public,_tmp2AC->name=_tmp292,_tmp2AC->varloc=0U,_tmp2AC->tq=Cyc_Toc_mt_tq,_tmp2AC->type=_tmp27C,_tmp2AC->initializer=z_init,_tmp2AC->rgn=0,_tmp2AC->attributes=0,_tmp2AC->escapes=0;_tmp2AC;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp294=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->tag=4U,_tmp2AB->f1=_tmp293;_tmp2AB;});
# 1588
struct Cyc_Absyn_Exp*_tmp295=Cyc_Absyn_varb_exp((void*)_tmp290,0U);_tmp295->topt=_tmp28D->topt;{
struct Cyc_Absyn_Exp*_tmp296=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*_tmp297=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp295,_tmp296,0U);
_tmp296->topt=Cyc_Absyn_sint_type;
_tmp297->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp297);{
# 1595
struct Cyc_Absyn_Exp*_tmp298=Cyc_Absyn_varb_exp((void*)_tmp294,0U);_tmp298->topt=_tmp28D->topt;{
struct Cyc_Absyn_Exp*_tmp299=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*_tmp29A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp298,_tmp299,0U);
_tmp299->topt=Cyc_Absyn_sint_type;
_tmp29A->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp29A);{
# 1602
struct Cyc_List_List*_tmp29B=({struct Cyc_Absyn_Exp*_tmp2AA[2U];({struct Cyc_Absyn_Exp*_tmp7F3=Cyc_Absyn_varb_exp((void*)_tmp28B,0U);_tmp2AA[0]=_tmp7F3;}),({
struct Cyc_Absyn_Exp*_tmp7F2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2AA[1]=_tmp7F2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2AA,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp29C=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp7F4=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp29B,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp7F4,_tmp29C,0U);});{
# 1609
struct Cyc_Absyn_Exp*_tmp29D=({struct Cyc_Absyn_Exp*_tmp7F5=xsize;Cyc_Absyn_and_exp(_tmp7F5,Cyc_Absyn_and_exp(_tmp297,_tmp29A,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp29E=Cyc_Absyn_exp_stmt(({void*_tmp2A9=0U;({struct Cyc_Absyn_Exp*_tmp7F6=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp7F6,_tag_dyneither(_tmp2A9,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);
struct Cyc_Absyn_Exp*_tmp29F=({struct Cyc_Absyn_Exp*_tmp7F7=Cyc_Absyn_varb_exp((void*)_tmp28B,0U);Cyc_Toc_member_exp(_tmp7F7,Cyc_Toc_curr_sp,0U);});
_tmp29F=Cyc_Toc_cast_it(_tmp27E,_tmp29F);{
struct Cyc_Absyn_Exp*_tmp2A0=Cyc_Absyn_deref_exp(_tmp29F,0U);
struct Cyc_Absyn_Exp*_tmp2A1=({struct Cyc_Absyn_Exp*_tmp7F8=_tmp2A0;Cyc_Absyn_assign_exp(_tmp7F8,Cyc_Absyn_var_exp(_tmp292,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2A2=Cyc_Absyn_exp_stmt(_tmp2A1,0U);
_tmp2A2=({struct Cyc_Absyn_Stmt*_tmp7FB=({struct Cyc_Absyn_Exp*_tmp7FA=_tmp29D;struct Cyc_Absyn_Stmt*_tmp7F9=_tmp29E;Cyc_Absyn_ifthenelse_stmt(_tmp7FA,_tmp7F9,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp7FB,_tmp2A2,0U);});
_tmp2A2=({struct Cyc_Absyn_Decl*_tmp7FD=({struct Cyc_Absyn_Decl*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));({void*_tmp7FC=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3->tag=0U,_tmp2A3->f1=_tmp293;_tmp2A3;});_tmp2A4->r=_tmp7FC;}),_tmp2A4->loc=0U;_tmp2A4;});Cyc_Absyn_decl_stmt(_tmp7FD,_tmp2A2,0U);});
_tmp2A2=({struct Cyc_Absyn_Decl*_tmp7FF=({struct Cyc_Absyn_Decl*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));({void*_tmp7FE=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));_tmp2A5->tag=0U,_tmp2A5->f1=_tmp28F;_tmp2A5;});_tmp2A6->r=_tmp7FE;}),_tmp2A6->loc=0U;_tmp2A6;});Cyc_Absyn_decl_stmt(_tmp7FF,_tmp2A2,0U);});
_tmp2A2=({struct Cyc_Absyn_Decl*_tmp801=({struct Cyc_Absyn_Decl*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));({void*_tmp800=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->tag=0U,_tmp2A7->f1=_tmp28A;_tmp2A7;});_tmp2A8->r=_tmp800;}),_tmp2A8->loc=0U;_tmp2A8;});Cyc_Absyn_decl_stmt(_tmp801,_tmp2A2,0U);});
({void*_tmp802=Cyc_Toc_stmt_exp_r(_tmp2A2);e->r=_tmp802;});};};};};};};};};};}
# 1563
;_pop_region(rgn2);};}
# 1635 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned)){
# 1639
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2B2=Cyc_Tcutil_compress(aggrtype);void*_tmp2B3=_tmp2B2;union Cyc_Absyn_AggrInfo _tmp2B8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B3)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B3)->f1)->tag == 20U){_LL1: _tmp2B8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B3)->f1)->f1;_LL2:
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp2B8);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp2B4=0U;({struct _dyneither_ptr _tmp805=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2B7=({struct Cyc_String_pa_PrintArg_struct _tmp720;_tmp720.tag=0U,({
struct _dyneither_ptr _tmp803=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp720.f1=_tmp803;});_tmp720;});void*_tmp2B5[1U];_tmp2B5[0]=& _tmp2B7;({struct _dyneither_ptr _tmp804=({const char*_tmp2B6="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2B6,sizeof(char),51U);});Cyc_aprintf(_tmp804,_tag_dyneither(_tmp2B5,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp805,_tag_dyneither(_tmp2B4,sizeof(void*),0U));});});}_LL0:;}{
# 1645
struct _tuple29 _tmp2B9=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp2BA=_tmp2B9;struct _tuple1*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;_LL6: _tmp2CB=_tmp2BA.f1;_tmp2CA=_tmp2BA.f2;_LL7:;{
struct Cyc_Absyn_Exp*_tmp2BB=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Absyn_aggrarrow_exp(_tmp2CA,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_neq_exp(_tmp2BC,_tmp2BB,0U);
struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Absyn_aggrarrow_exp(_tmp2CA,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2BF=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2BE,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2C0=({struct Cyc_Absyn_Exp*_tmp807=_tmp2BD;struct Cyc_Absyn_Stmt*_tmp806=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp807,_tmp806,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2C1=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2C2=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2C3=({struct _tuple1*_tmp80A=_tmp2CB;void*_tmp809=_tmp2C1;struct Cyc_Absyn_Exp*_tmp808=_tmp2C2;Cyc_Absyn_declare_stmt(_tmp80A,_tmp809,_tmp808,Cyc_Absyn_seq_stmt(_tmp2C0,_tmp2BF,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2C3);}else{
# 1658
struct Cyc_Absyn_Exp*_tmp2C4=({struct Cyc_Absyn_Exp*_tmp80B=aggrproj(_tmp2CA,f,0U);Cyc_Toc_member_exp(_tmp80B,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2C5=Cyc_Absyn_neq_exp(_tmp2C4,_tmp2BB,0U);
struct Cyc_Absyn_Exp*_tmp2C6=({struct Cyc_Absyn_Exp*_tmp80C=aggrproj(_tmp2CA,f,0U);Cyc_Toc_member_exp(_tmp80C,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2C7=Cyc_Absyn_exp_stmt(_tmp2C6,0U);
struct Cyc_Absyn_Stmt*_tmp2C8=({struct Cyc_Absyn_Exp*_tmp80E=_tmp2C5;struct Cyc_Absyn_Stmt*_tmp80D=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp80E,_tmp80D,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2C9=({struct _tuple1*_tmp811=_tmp2CB;void*_tmp810=e1_c_type;struct Cyc_Absyn_Exp*_tmp80F=e1;Cyc_Absyn_declare_stmt(_tmp811,_tmp810,_tmp80F,Cyc_Absyn_seq_stmt(_tmp2C8,_tmp2C7,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2C9);}};};}
# 1668
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1671
void*_tmp2CC=Cyc_Tcutil_compress(t);void*_tmp2CD=_tmp2CC;union Cyc_Absyn_AggrInfo _tmp2D4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2CD)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2CD)->f1)->tag == 20U){_LL1: _tmp2D4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2CD)->f1)->f1;_LL2: {
# 1673
struct Cyc_Absyn_Aggrdecl*_tmp2CE=Cyc_Absyn_get_known_aggrdecl(_tmp2D4);
({int _tmp812=Cyc_Toc_get_member_offset(_tmp2CE,f);*f_tag=_tmp812;});{
# 1676
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D2=({struct Cyc_String_pa_PrintArg_struct _tmp722;_tmp722.tag=0U,_tmp722.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2CE->name).f2);_tmp722;});struct Cyc_String_pa_PrintArg_struct _tmp2D3=({struct Cyc_String_pa_PrintArg_struct _tmp721;_tmp721.tag=0U,_tmp721.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp721;});void*_tmp2D0[2U];_tmp2D0[0]=& _tmp2D2,_tmp2D0[1]=& _tmp2D3;({struct _dyneither_ptr _tmp813=({const char*_tmp2D1="_union_%s_%s";_tag_dyneither(_tmp2D1,sizeof(char),13U);});Cyc_aprintf(_tmp813,_tag_dyneither(_tmp2D0,sizeof(void*),2U));});});
({void*_tmp814=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));*_tmp2CF=str;_tmp2CF;}));*tagged_member_type=_tmp814;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2CE->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1687
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1690
void*_tmp2D5=e->r;void*_tmp2D6=_tmp2D5;struct Cyc_Absyn_Exp*_tmp2E2;struct _dyneither_ptr*_tmp2E1;int*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2DF;struct _dyneither_ptr*_tmp2DE;int*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DC;switch(*((int*)_tmp2D6)){case 14U: _LL1: _tmp2DC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D6)->f2;_LL2:
({void*_tmp2D7=0U;({struct _dyneither_ptr _tmp815=({const char*_tmp2D8="cast on lhs!";_tag_dyneither(_tmp2D8,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp815,_tag_dyneither(_tmp2D7,sizeof(void*),0U));});});case 21U: _LL3: _tmp2DF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D6)->f1;_tmp2DE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D6)->f2;_tmp2DD=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D6)->f4;_LL4:
# 1693
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp2DF->topt),_tmp2DE,f_tag,tagged_member_type,clear_read,_tmp2DD);case 22U: _LL5: _tmp2E2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D6)->f1;_tmp2E1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D6)->f2;_tmp2E0=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D6)->f4;_LL6: {
# 1696
void*_tmp2D9=Cyc_Tcutil_compress((void*)_check_null(_tmp2E2->topt));void*_tmp2DA=_tmp2D9;void*_tmp2DB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DA)->tag == 3U){_LLA: _tmp2DB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DA)->f1).elt_type;_LLB:
# 1698
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2DB,_tmp2E1,f_tag,tagged_member_type,clear_read,_tmp2E0);}else{_LLC: _LLD:
# 1700
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1702
 return 0;}_LL0:;}
# 1715 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1719
struct _tuple29 _tmp2E3=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp2E4=_tmp2E3;struct _tuple1*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E6;_LL1: _tmp2E7=_tmp2E4.f1;_tmp2E6=_tmp2E4.f2;_LL2:;{
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(_tmp2E6,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(_tmp2E6,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1728
struct Cyc_Absyn_Exp*_tmp2E5=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
s2=({struct Cyc_Absyn_Exp*_tmp817=_tmp2E5;struct Cyc_Absyn_Stmt*_tmp816=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp817,_tmp816,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1731
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp81A=_tmp2E7;void*_tmp819=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp818=
Cyc_Toc_push_address_exp(e1);
# 1731
Cyc_Absyn_declare_stmt(_tmp81A,_tmp819,_tmp818,
# 1733
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};};}struct _tuple32{void*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1737
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp2EA=({struct Cyc_String_pa_PrintArg_struct _tmp723;_tmp723.tag=0U,({struct _dyneither_ptr _tmp81B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp723.f1=_tmp81B;});_tmp723;});void*_tmp2E8[1U];_tmp2E8[0]=& _tmp2EA;({unsigned _tmp81D=e->loc;struct _dyneither_ptr _tmp81C=({const char*_tmp2E9="exp_to_c: no type for %s";_tag_dyneither(_tmp2E9,sizeof(char),25U);});((int(*)(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp81D,_tmp81C,_tag_dyneither(_tmp2E8,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1742
int did_inserted_checks=0;
{void*_tmp2EB=e->annot;void*_tmp2EC=_tmp2EB;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2EC)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2EC)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1748
{void*_tmp2ED=e->r;void*_tmp2EE=_tmp2ED;struct Cyc_Absyn_Stmt*_tmp494;struct Cyc_Absyn_Exp*_tmp493;struct _dyneither_ptr*_tmp492;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*_tmp490;int _tmp48F;struct Cyc_Absyn_Exp*_tmp48E;void**_tmp48D;struct Cyc_Absyn_Exp*_tmp48C;int _tmp48B;int _tmp48A;struct Cyc_List_List*_tmp489;struct Cyc_Absyn_Datatypedecl*_tmp488;struct Cyc_Absyn_Datatypefield*_tmp487;struct _tuple1**_tmp486;struct Cyc_List_List*_tmp485;struct Cyc_List_List*_tmp484;struct Cyc_Absyn_Aggrdecl*_tmp483;void*_tmp482;struct Cyc_List_List*_tmp481;struct Cyc_Absyn_Exp*_tmp480;void*_tmp47F;int _tmp47E;struct Cyc_Absyn_Vardecl*_tmp47D;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Exp*_tmp47B;int _tmp47A;struct Cyc_List_List*_tmp479;struct Cyc_List_List*_tmp478;struct Cyc_Absyn_Exp*_tmp477;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_Absyn_Exp*_tmp475;struct Cyc_Absyn_Exp*_tmp474;struct _dyneither_ptr*_tmp473;int _tmp472;int _tmp471;struct Cyc_Absyn_Exp*_tmp470;struct _dyneither_ptr*_tmp46F;int _tmp46E;int _tmp46D;void*_tmp46C;struct Cyc_List_List*_tmp46B;void*_tmp46A;struct Cyc_Absyn_Exp*_tmp469;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*_tmp467;struct Cyc_Absyn_Exp*_tmp466;void**_tmp465;struct Cyc_Absyn_Exp*_tmp464;int _tmp463;enum Cyc_Absyn_Coercion _tmp462;struct Cyc_Absyn_Exp*_tmp461;struct Cyc_List_List*_tmp460;struct Cyc_Absyn_Exp*_tmp45F;struct Cyc_Absyn_Exp*_tmp45E;int _tmp45D;struct Cyc_Absyn_Exp*_tmp45C;struct Cyc_List_List*_tmp45B;int _tmp45A;struct Cyc_List_List*_tmp459;struct Cyc_Absyn_VarargInfo*_tmp458;struct Cyc_Absyn_Exp*_tmp457;struct Cyc_List_List*_tmp456;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Exp*_tmp454;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_Absyn_Exp*_tmp452;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Exp*_tmp44C;struct Cyc_Core_Opt*_tmp44B;struct Cyc_Absyn_Exp*_tmp44A;struct Cyc_Absyn_Exp*_tmp449;enum Cyc_Absyn_Incrementor _tmp448;enum Cyc_Absyn_Primop _tmp447;struct Cyc_List_List*_tmp446;struct Cyc_Absyn_Exp*_tmp445;switch(*((int*)_tmp2EE)){case 2U: _LL8: _LL9:
# 1750
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1757
struct Cyc_Absyn_Exp*_tmp2EF=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp81E=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp2EF,_tmp2EF))->r;e->r=_tmp81E;});else{
# 1762
({void*_tmp820=({struct Cyc_Absyn_Exp*_tmp2F0[3U];_tmp2F0[0]=_tmp2EF,_tmp2F0[1]=_tmp2EF,_tmp2F0[2]=_tmp2EF;({struct Cyc_Absyn_Exp*_tmp81F=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp81F,_tag_dyneither(_tmp2F0,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp820;});}}else{
# 1764
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1766
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 41U: _LL10: _tmp445=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_LL11:
 Cyc_Toc_exp_to_c(nv,_tmp445);goto _LL7;case 3U: _LL12: _tmp447=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp446=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL13: {
# 1772
struct Cyc_List_List*_tmp2F1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp446);
# 1774
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp446);
{enum Cyc_Absyn_Primop _tmp2F2=_tmp447;switch(_tmp2F2){case Cyc_Absyn_Numelts: _LL61: _LL62: {
# 1777
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp446))->hd;
{void*_tmp2F3=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp2F4=_tmp2F3;void*_tmp301;void*_tmp300;void*_tmp2FF;void*_tmp2FE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->tag == 3U){_LL76: _tmp301=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->f1).elt_type;_tmp300=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->f1).ptr_atts).nullable;_tmp2FF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->f1).ptr_atts).bounds;_tmp2FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->f1).ptr_atts).zero_term;_LL77: {
# 1780
struct Cyc_Absyn_Exp*_tmp2F5=({void*_tmp821=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp821,_tmp2FF);});
if(_tmp2F5 == 0)
# 1783
({void*_tmp824=({struct Cyc_Absyn_Exp*_tmp2F6[2U];_tmp2F6[0]=(struct Cyc_Absyn_Exp*)_tmp446->hd,({
struct Cyc_Absyn_Exp*_tmp822=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp301),0U);_tmp2F6[1]=_tmp822;});({struct Cyc_Absyn_Exp*_tmp823=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp823,_tag_dyneither(_tmp2F6,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1783
e->r=_tmp824;});else{
# 1786
struct Cyc_Absyn_Exp*_tmp2F7=_tmp2F5;
# 1788
if(Cyc_Tcutil_force_type2bool(0,_tmp2FE)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp446->hd);
# 1792
({void*_tmp826=({struct Cyc_Absyn_Exp*_tmp2F8[2U];_tmp2F8[0]=(struct Cyc_Absyn_Exp*)_tmp446->hd,_tmp2F8[1]=_tmp2F7;({struct Cyc_Absyn_Exp*_tmp825=function_e;Cyc_Toc_fncall_exp_r(_tmp825,_tag_dyneither(_tmp2F8,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp826;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp300)){
if(!Cyc_Evexp_c_can_eval(_tmp2F7))
({void*_tmp2F9=0U;({unsigned _tmp828=e->loc;struct _dyneither_ptr _tmp827=({const char*_tmp2FA="can't calculate numelts";_tag_dyneither(_tmp2FA,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp828,_tmp827,_tag_dyneither(_tmp2F9,sizeof(void*),0U));});});
# 1797
({void*_tmp82B=({struct Cyc_Absyn_Exp*_tmp82A=arg;struct Cyc_Absyn_Exp*_tmp829=_tmp2F7;Cyc_Toc_conditional_exp_r(_tmp82A,_tmp829,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp82B;});}else{
# 1799
e->r=_tmp2F7->r;goto _LL75;}}}
# 1802
goto _LL75;}}else{_LL78: _LL79:
({struct Cyc_String_pa_PrintArg_struct _tmp2FD=({struct Cyc_String_pa_PrintArg_struct _tmp724;_tmp724.tag=0U,({
struct _dyneither_ptr _tmp82C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp724.f1=_tmp82C;});_tmp724;});void*_tmp2FB[1U];_tmp2FB[0]=& _tmp2FD;({struct _dyneither_ptr _tmp82D=({const char*_tmp2FC="numelts primop applied to non-pointer %s";_tag_dyneither(_tmp2FC,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp82D,_tag_dyneither(_tmp2FB,sizeof(void*),1U));});});}_LL75:;}
# 1806
goto _LL60;}case Cyc_Absyn_Plus: _LL63: _LL64:
# 1811
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp302=0U;({struct _dyneither_ptr _tmp82E=({const char*_tmp303="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp303,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp82E,_tag_dyneither(_tmp302,sizeof(void*),0U));});});
{void*_tmp304=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp2F1))->hd);void*_tmp305=_tmp304;void*_tmp30B;void*_tmp30A;void*_tmp309;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->tag == 3U){_LL7B: _tmp30B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).elt_type;_tmp30A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).ptr_atts).bounds;_tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).ptr_atts).zero_term;_LL7C: {
# 1815
struct Cyc_Absyn_Exp*_tmp306=({void*_tmp82F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp82F,_tmp30A);});
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp446))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp446->tl))->hd;
if(_tmp306 == 0)
({void*_tmp832=({struct Cyc_Absyn_Exp*_tmp307[3U];_tmp307[0]=e1,({
struct Cyc_Absyn_Exp*_tmp830=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp30B),0U);_tmp307[1]=_tmp830;}),_tmp307[2]=e2;({struct Cyc_Absyn_Exp*_tmp831=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp831,_tag_dyneither(_tmp307,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1819
e->r=_tmp832;});else{
# 1821
if(Cyc_Tcutil_force_type2bool(0,_tmp309))
({void*_tmp834=({struct Cyc_Absyn_Exp*_tmp308[3U];_tmp308[0]=e1,_tmp308[1]=_tmp306,_tmp308[2]=e2;({struct Cyc_Absyn_Exp*_tmp833=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp833,_tag_dyneither(_tmp308,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp834;});}
goto _LL7A;}}else{_LL7D: _LL7E:
 goto _LL7A;}_LL7A:;}
# 1826
goto _LL60;case Cyc_Absyn_Minus: _LL65: _LL66: {
# 1831
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp2F1))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp446))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp446->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp2F1->tl))->hd)){
({void*_tmp836=({struct Cyc_Absyn_Exp*_tmp835=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp835,Cyc_Toc_curr_sp);});e1->r=_tmp836;});
({void*_tmp838=({struct Cyc_Absyn_Exp*_tmp837=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp837,Cyc_Toc_curr_sp);});e2->r=_tmp838;});
({void*_tmp83A=({void*_tmp839=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp839;});e1->topt=_tmp83A;});
({void*_tmp83C=({struct Cyc_Absyn_Exp*_tmp83B=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp83B,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1839
e->r=_tmp83C;});}else{
# 1842
({void*_tmp840=({struct Cyc_Absyn_Exp*_tmp30C[3U];_tmp30C[0]=e1,({
struct Cyc_Absyn_Exp*_tmp83E=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp30C[1]=_tmp83E;}),({
struct Cyc_Absyn_Exp*_tmp83D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp30C[2]=_tmp83D;});({struct Cyc_Absyn_Exp*_tmp83F=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp83F,_tag_dyneither(_tmp30C,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1842
e->r=_tmp840;});}}
# 1846
goto _LL60;}case Cyc_Absyn_Eq: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Neq: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Gt: _LL6B: _LL6C:
 goto _LL6E;case Cyc_Absyn_Gte: _LL6D: _LL6E:
 goto _LL70;case Cyc_Absyn_Lt: _LL6F: _LL70:
 goto _LL72;case Cyc_Absyn_Lte: _LL71: _LL72: {
# 1854
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp446))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp446->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp2F1))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp2F1->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& elt_type)){
void*_tmp30D=({void*_tmp841=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp841,Cyc_Toc_mt_tq);});
({void*_tmp844=({void*_tmp843=_tmp30D;Cyc_Toc_cast_it_r(_tmp843,({struct Cyc_Absyn_Exp*_tmp842=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp842,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp844;});
e1->topt=_tmp30D;}
# 1864
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
void*_tmp30E=({void*_tmp845=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp845,Cyc_Toc_mt_tq);});
({void*_tmp848=({void*_tmp847=_tmp30E;Cyc_Toc_cast_it_r(_tmp847,({struct Cyc_Absyn_Exp*_tmp846=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp846,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp848;});
e2->topt=_tmp30E;}
# 1869
goto _LL60;}default: _LL73: _LL74:
 goto _LL60;}_LL60:;}
# 1872
goto _LL7;}case 5U: _LL14: _tmp449=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp448=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL15: {
# 1874
void*e2_cyc_typ=(void*)_check_null(_tmp449->topt);
# 1883 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _dyneither_ptr incr_str=({const char*_tmp322="increment";_tag_dyneither(_tmp322,sizeof(char),10U);});
if((int)_tmp448 == (int)2U ||(int)_tmp448 == (int)3U)incr_str=({const char*_tmp30F="decrement";_tag_dyneither(_tmp30F,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp449,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp312=({struct Cyc_String_pa_PrintArg_struct _tmp725;_tmp725.tag=0U,_tmp725.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp725;});void*_tmp310[1U];_tmp310[0]=& _tmp312;({unsigned _tmp84A=e->loc;struct _dyneither_ptr _tmp849=({const char*_tmp311="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp311,sizeof(char),74U);});((int(*)(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp84A,_tmp849,_tag_dyneither(_tmp310,sizeof(void*),1U));});});
# 1891
if(Cyc_Toc_is_tagged_union_project(_tmp449,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp313=_tmp448;switch(_tmp313){case Cyc_Absyn_PreInc: _LL80: _LL81:
 op=0U;goto _LL7F;case Cyc_Absyn_PreDec: _LL82: _LL83:
 op=2U;goto _LL7F;default: _LL84: _LL85:
({struct Cyc_String_pa_PrintArg_struct _tmp316=({struct Cyc_String_pa_PrintArg_struct _tmp726;_tmp726.tag=0U,_tmp726.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp726;});void*_tmp314[1U];_tmp314[0]=& _tmp316;({unsigned _tmp84C=e->loc;struct _dyneither_ptr _tmp84B=({const char*_tmp315="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp315,sizeof(char),58U);});((int(*)(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp84C,_tmp84B,_tag_dyneither(_tmp314,sizeof(void*),1U));});});}_LL7F:;}
# 1901
({void*_tmp84E=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->tag=4U,_tmp318->f1=_tmp449,({struct Cyc_Core_Opt*_tmp84D=({struct Cyc_Core_Opt*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->v=(void*)op;_tmp317;});_tmp318->f2=_tmp84D;}),_tmp318->f3=one;_tmp318;});e->r=_tmp84E;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1905
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp449);
Cyc_Toc_set_lhs(nv,0);{
# 1910
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=((int)_tmp448 == (int)1U ||(int)_tmp448 == (int)3U)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 1916
if((int)_tmp448 == (int)2U ||(int)_tmp448 == (int)3U)
change=-1;
({void*_tmp853=({struct Cyc_Absyn_Exp*_tmp319[3U];({struct Cyc_Absyn_Exp*_tmp851=Cyc_Toc_push_address_exp(_tmp449);_tmp319[0]=_tmp851;}),({
struct Cyc_Absyn_Exp*_tmp850=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp319[1]=_tmp850;}),({
struct Cyc_Absyn_Exp*_tmp84F=Cyc_Absyn_signed_int_exp(change,0U);_tmp319[2]=_tmp84F;});({struct Cyc_Absyn_Exp*_tmp852=fn_e;Cyc_Toc_fncall_exp_r(_tmp852,_tag_dyneither(_tmp319,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1918
e->r=_tmp853;});}else{
# 1921
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1927
did_inserted_checks=1;
if((int)_tmp448 != (int)1U){
struct _tuple1*_tmp31A=Cyc_Toc_temp_var();
void*_tmp31B=({void*_tmp854=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp854,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp31C=Cyc_Toc_push_address_exp(_tmp449);
struct Cyc_Absyn_Exp*_tmp31D=({struct Cyc_Absyn_Exp*_tmp855=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp31A,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp855,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
_tmp31D=({struct Cyc_Absyn_Exp*_tmp857=({struct Cyc_Absyn_Exp*_tmp856=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp31A,0U),0U);Cyc_Absyn_neq_exp(_tmp856,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1935
Cyc_Absyn_and_exp(_tmp857,_tmp31D,0U);});{
# 1937
struct Cyc_Absyn_Stmt*_tmp31E=({struct Cyc_Absyn_Exp*_tmp85A=_tmp31D;struct Cyc_Absyn_Stmt*_tmp859=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp31A,0U),0U),0U),0U);
# 1937
Cyc_Absyn_ifthenelse_stmt(_tmp85A,_tmp859,
# 1939
Cyc_Absyn_exp_stmt(({void*_tmp31F=0U;({struct Cyc_Absyn_Exp*_tmp858=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp858,_tag_dyneither(_tmp31F,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
_tmp31E=({struct Cyc_Absyn_Stmt*_tmp85B=_tmp31E;Cyc_Absyn_seq_stmt(_tmp85B,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp31A,0U),0U),0U),0U);});
({void*_tmp85C=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp31A,_tmp31B,_tmp31C,_tmp31E,0U),0U))->r;e->r=_tmp85C;});};}else{
# 1944
struct Cyc_Toc_functionSet*_tmp320=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp320,_tmp449);
({void*_tmp860=({struct Cyc_Absyn_Exp*_tmp321[2U];({struct Cyc_Absyn_Exp*_tmp85E=Cyc_Toc_push_address_exp(_tmp449);_tmp321[0]=_tmp85E;}),({struct Cyc_Absyn_Exp*_tmp85D=Cyc_Absyn_signed_int_exp(1,0U);_tmp321[1]=_tmp85D;});({struct Cyc_Absyn_Exp*_tmp85F=fn_e;Cyc_Toc_fncall_exp_r(_tmp85F,_tag_dyneither(_tmp321,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp860;});}}else{
# 1948
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(_tmp449)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp449,0,Cyc_Toc_incr_lvalue,_tmp448);
e->r=_tmp449->r;}}}
# 1952
goto _LL7;};}case 4U: _LL16: _tmp44C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp44B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp44A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3;_LL17: {
# 1971 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp44C->topt);
void*e2_old_typ=(void*)_check_null(_tmp44A->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp44C,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp44C);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp44A);
({void*_tmp861=Cyc_Toc_tagged_union_assignop(_tmp44C,e1_old_typ,_tmp44B,_tmp44A,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp861;});
# 1982
goto _LL7;}{
# 1984
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp44C,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp44C,_tmp44B,_tmp44A,ptr_type,is_dyneither,elt_type);
# 1990
return;}{
# 1994
int e1_poly=Cyc_Toc_is_poly_project(_tmp44C);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp44C);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp44A);{
# 2000
int done=0;
if(_tmp44B != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp323=(enum Cyc_Absyn_Primop)_tmp44B->v;enum Cyc_Absyn_Primop _tmp324=_tmp323;switch(_tmp324){case Cyc_Absyn_Plus: _LL87: _LL88:
 change=_tmp44A;goto _LL86;case Cyc_Absyn_Minus: _LL89: _LL8A:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp44A,0U);goto _LL86;default: _LL8B: _LL8C:
({void*_tmp325=0U;({struct _dyneither_ptr _tmp862=({const char*_tmp326="bad t ? pointer arithmetic";_tag_dyneither(_tmp326,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp862,_tag_dyneither(_tmp325,sizeof(void*),0U));});});}_LL86:;}
# 2010
done=1;{
# 2012
struct Cyc_Absyn_Exp*_tmp327=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp866=({struct Cyc_Absyn_Exp*_tmp328[3U];({struct Cyc_Absyn_Exp*_tmp864=Cyc_Toc_push_address_exp(_tmp44C);_tmp328[0]=_tmp864;}),({
struct Cyc_Absyn_Exp*_tmp863=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp328[1]=_tmp863;}),_tmp328[2]=change;({struct Cyc_Absyn_Exp*_tmp865=_tmp327;Cyc_Toc_fncall_exp_r(_tmp865,_tag_dyneither(_tmp328,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2013
e->r=_tmp866;});};}else{
# 2016
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2019
enum Cyc_Absyn_Primop _tmp329=(enum Cyc_Absyn_Primop)_tmp44B->v;enum Cyc_Absyn_Primop _tmp32A=_tmp329;if(_tmp32A == Cyc_Absyn_Plus){_LL8E: _LL8F:
# 2021
 done=1;
({void*_tmp868=({struct Cyc_Absyn_Exp*_tmp32B[2U];_tmp32B[0]=_tmp44C,_tmp32B[1]=_tmp44A;({struct Cyc_Absyn_Exp*_tmp867=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp44C);Cyc_Toc_fncall_exp_r(_tmp867,_tag_dyneither(_tmp32B,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp868;});
goto _LL8D;}else{_LL90: _LL91:
({void*_tmp32C=0U;({struct _dyneither_ptr _tmp869=({const char*_tmp32D="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp32D,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp869,_tag_dyneither(_tmp32C,sizeof(void*),0U));});});}_LL8D:;}}}
# 2028
if(!done){
# 2030
if(e1_poly)
({void*_tmp86B=({void*_tmp86A=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp86A,Cyc_Absyn_new_exp(_tmp44A->r,0U));});_tmp44A->r=_tmp86B;});
# 2036
if(!Cyc_Absyn_is_lvalue(_tmp44C)){
({struct Cyc_Absyn_Exp*_tmp86C=_tmp44C;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp86C,0,Cyc_Toc_assignop_lvalue,({struct _tuple31*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->f1=_tmp44B,_tmp32E->f2=_tmp44A;_tmp32E;}));});
e->r=_tmp44C->r;}}
# 2041
goto _LL7;};};};}case 6U: _LL18: _tmp44F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp44E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp44D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3;_LL19:
# 2043
 Cyc_Toc_exp_to_c(nv,_tmp44F);
Cyc_Toc_exp_to_c(nv,_tmp44E);
Cyc_Toc_exp_to_c(nv,_tmp44D);
goto _LL7;case 7U: _LL1A: _tmp451=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp450=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL1B:
 _tmp453=_tmp451;_tmp452=_tmp450;goto _LL1D;case 8U: _LL1C: _tmp453=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp452=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL1D:
 _tmp455=_tmp453;_tmp454=_tmp452;goto _LL1F;case 9U: _LL1E: _tmp455=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp454=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL1F:
# 2050
 Cyc_Toc_exp_to_c(nv,_tmp455);
Cyc_Toc_exp_to_c(nv,_tmp454);
goto _LL7;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3 == 0){_LL20: _tmp457=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp456=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL21: {
# 2054
void*e1_typ=(void*)_check_null(_tmp457->topt);
Cyc_Toc_exp_to_c(nv,_tmp457);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2059
({void*_tmp870=({void*_tmp86F=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp86F,({struct Cyc_Absyn_Exp*_tmp32F[1U];({struct Cyc_Absyn_Exp*_tmp86D=
Cyc_Absyn_copy_exp(_tmp457);_tmp32F[0]=_tmp86D;});({struct Cyc_Absyn_Exp*_tmp86E=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp86E,_tag_dyneither(_tmp32F,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2059
_tmp457->r=_tmp870;});
# 2061
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp456);
goto _LL7;}}else{_LL22: _tmp45C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp45B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp45A=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3)->num_varargs;_tmp459=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3)->injectors;_tmp458=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3)->vai;_LL23: {
# 2072 "toc.cyc"
struct _RegionHandle _tmp330=_new_region("r");struct _RegionHandle*r=& _tmp330;_push_region(r);{
struct _tuple29 _tmp331=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp332=_tmp331;struct _tuple1*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;_LL93: _tmp34B=_tmp332.f1;_tmp34A=_tmp332.f2;_LL94:;{
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned)_tmp45A,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp458->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2079
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp45B);
int num_normargs=num_args - _tmp45A;
# 2083
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp45B=_tmp45B->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45B))->hd);
new_args=({struct Cyc_List_List*_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333->hd=(struct Cyc_Absyn_Exp*)_tmp45B->hd,_tmp333->tl=new_args;_tmp333;});}}
# 2088
new_args=({struct Cyc_List_List*_tmp335=_cycalloc(sizeof(*_tmp335));({struct Cyc_Absyn_Exp*_tmp873=({struct Cyc_Absyn_Exp*_tmp334[3U];_tmp334[0]=_tmp34A,({
# 2090
struct Cyc_Absyn_Exp*_tmp871=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp334[1]=_tmp871;}),_tmp334[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp872=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp872,_tag_dyneither(_tmp334,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2088
_tmp335->hd=_tmp873;}),_tmp335->tl=new_args;_tmp335;});
# 2093
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2095
void*e1_typ=(void*)_check_null(_tmp45C->topt);
Cyc_Toc_exp_to_c(nv,_tmp45C);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2100
({void*_tmp877=({void*_tmp876=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp876,({struct Cyc_Absyn_Exp*_tmp336[1U];({struct Cyc_Absyn_Exp*_tmp874=
Cyc_Absyn_copy_exp(_tmp45C);_tmp336[0]=_tmp874;});({struct Cyc_Absyn_Exp*_tmp875=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp875,_tag_dyneither(_tmp336,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2100
_tmp45C->r=_tmp877;});{
# 2102
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp45C,new_args,0U),0U);
# 2105
if(_tmp458->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp337=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp458->type));void*_tmp338=_tmp337;struct Cyc_Absyn_Datatypedecl*_tmp33B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp338)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp338)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp338)->f1)->f1).KnownDatatype).tag == 2){_LL96: _tmp33B=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp338)->f1)->f1).KnownDatatype).val;_LL97:
 tud=_tmp33B;goto _LL95;}else{goto _LL98;}}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp339=0U;({struct _dyneither_ptr _tmp878=({const char*_tmp33A="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp33A,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp878,_tag_dyneither(_tmp339,sizeof(void*),0U));});});}_LL95:;}{
# 2111
struct _dyneither_ptr vs=({unsigned _tmp347=(unsigned)_tmp45A;struct _tuple1**_tmp346=({struct _RegionHandle*_tmp879=r;_region_malloc(_tmp879,_check_times(_tmp347,sizeof(struct _tuple1*)));});({{unsigned _tmp727=(unsigned)_tmp45A;unsigned i;for(i=0;i < _tmp727;++ i){({struct _tuple1*_tmp87A=Cyc_Toc_temp_var();_tmp346[i]=_tmp87A;});}}0;});_tag_dyneither(_tmp346,sizeof(struct _tuple1*),_tmp347);});
# 2113
if(_tmp45A != 0){
# 2117
struct Cyc_List_List*_tmp33C=0;
{int i=_tmp45A - 1;for(0;i >= 0;-- i){
_tmp33C=({struct Cyc_List_List*_tmp33D=_cycalloc(sizeof(*_tmp33D));({struct Cyc_Absyn_Exp*_tmp87B=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp33D->hd=_tmp87B;}),_tmp33D->tl=_tmp33C;_tmp33D;});}}
s=({struct _tuple1*_tmp87E=_tmp34B;void*_tmp87D=arr_type;struct Cyc_Absyn_Exp*_tmp87C=Cyc_Absyn_array_exp(_tmp33C,0U);Cyc_Absyn_declare_stmt(_tmp87E,_tmp87D,_tmp87C,s,0U);});
# 2122
_tmp45B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp45B);
_tmp459=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp459);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp45B)- 1;
for(0;_tmp45B != 0;(_tmp45B=_tmp45B->tl,_tmp459=_tmp459->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp45B->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp33E=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp459))->hd;struct Cyc_Absyn_Datatypefield*_tmp33F=_tmp33E;struct _tuple1*_tmp345;struct Cyc_List_List*_tmp344;_LL9B: _tmp345=_tmp33F->name;_tmp344=_tmp33F->typs;_LL9C:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp344))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2136
struct _tuple1*_tmp340=Cyc_Toc_collapse_qvars(_tmp345,tud->name);
struct Cyc_List_List*_tmp341=({struct _tuple19*_tmp343[2U];({struct _tuple19*_tmp882=({struct _dyneither_ptr*_tmp881=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp881,Cyc_Toc_datatype_tag(tud,_tmp345));});_tmp343[0]=_tmp882;}),({
struct _tuple19*_tmp880=({struct _dyneither_ptr*_tmp87F=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp87F,arg);});_tmp343[1]=_tmp880;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp343,sizeof(struct _tuple19*),2U));});
s=({struct _tuple1*_tmp885=var;void*_tmp884=Cyc_Absyn_strctq(_tmp340);struct Cyc_Absyn_Exp*_tmp883=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342->tag=29U,_tmp342->f1=_tmp340,_tmp342->f2=0,_tmp342->f3=_tmp341,_tmp342->f4=0;_tmp342;}),0U);
# 2139
Cyc_Absyn_declare_stmt(_tmp885,_tmp884,_tmp883,s,0U);});};};}};}else{
# 2147
s=({struct _tuple1*_tmp888=_tmp34B;void*_tmp887=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp886=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp888,_tmp887,_tmp886,s,0U);});}};}else{
# 2152
if(_tmp45A != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp45B != 0;_tmp45B=_tmp45B->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp45B->hd);
array_args=({struct Cyc_List_List*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348->hd=(struct Cyc_Absyn_Exp*)_tmp45B->hd,_tmp348->tl=array_args;_tmp348;});}{
# 2158
struct Cyc_Absyn_Exp*_tmp349=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
s=Cyc_Absyn_declare_stmt(_tmp34B,arr_type,_tmp349,s,0U);};}else{
# 2161
s=({struct _tuple1*_tmp88B=_tmp34B;void*_tmp88A=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp889=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp88B,_tmp88A,_tmp889,s,0U);});}}
# 2164
({void*_tmp88C=Cyc_Toc_stmt_exp_r(s);e->r=_tmp88C;});};};};}
# 2166
_npop_handler(0U);goto _LL7;
# 2072
;_pop_region(r);}}case 11U: _LL24: _tmp45E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp45D=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL25:
# 2169
 Cyc_Toc_exp_to_c(nv,_tmp45E);{
struct Cyc_Absyn_Exp*fn_e=_tmp45D?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp88F=({void*_tmp88E=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp88E,({struct Cyc_Absyn_Exp*_tmp34C[1U];_tmp34C[0]=_tmp45E;({struct Cyc_Absyn_Exp*_tmp88D=fn_e;Cyc_Toc_fncall_exp_dl(_tmp88D,_tag_dyneither(_tmp34C,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp88F;});
goto _LL7;};case 12U: _LL26: _tmp45F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_LL27:
 Cyc_Toc_exp_to_c(nv,_tmp45F);goto _LL7;case 13U: _LL28: _tmp461=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp460=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL29:
# 2175
 Cyc_Toc_exp_to_c(nv,_tmp461);
# 2184 "toc.cyc"
for(0;_tmp460 != 0;_tmp460=_tmp460->tl){
enum Cyc_Absyn_KindQual _tmp34D=(Cyc_Tcutil_type_kind((void*)_tmp460->hd))->kind;
if((int)_tmp34D != (int)4U &&(int)_tmp34D != (int)3U){
{void*_tmp34E=Cyc_Tcutil_compress((void*)_tmp460->hd);void*_tmp34F=_tmp34E;switch(*((int*)_tmp34F)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34F)->f1)->tag == 18U){_LLA0: _LLA1:
 continue;}else{goto _LLA2;}default: _LLA2: _LLA3:
# 2191
({void*_tmp891=({void*_tmp890=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp890,_tmp461,0U);})->r;e->r=_tmp891;});
goto _LL9D;}_LL9D:;}
# 2194
break;}}
# 2197
goto _LL7;case 14U: _LL2A: _tmp465=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp464=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp463=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3;_tmp462=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EE)->f4;_LL2B: {
# 2199
void*old_t2=(void*)_check_null(_tmp464->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp465;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp465=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp464);
# 2206
{struct _tuple32 _tmp350=({struct _tuple32 _tmp729;({void*_tmp893=Cyc_Tcutil_compress(old_t2);_tmp729.f1=_tmp893;}),({void*_tmp892=Cyc_Tcutil_compress(new_typ);_tmp729.f2=_tmp892;});_tmp729;});struct _tuple32 _tmp351=_tmp350;struct Cyc_Absyn_PtrInfo _tmp377;struct Cyc_Absyn_PtrInfo _tmp376;struct Cyc_Absyn_PtrInfo _tmp375;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp351.f1)->tag == 3U)switch(*((int*)_tmp351.f2)){case 3U: _LLA5: _tmp376=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp351.f1)->f1;_tmp375=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp351.f2)->f1;_LLA6: {
# 2208
struct Cyc_Absyn_Exp*_tmp352=({void*_tmp894=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp894,(_tmp376.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp353=({void*_tmp895=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp895,(_tmp375.ptr_atts).bounds);});
int _tmp354=Cyc_Tcutil_force_type2bool(0,(_tmp376.ptr_atts).zero_term);
int _tmp355=Cyc_Tcutil_force_type2bool(0,(_tmp375.ptr_atts).zero_term);
{struct _tuple33 _tmp356=({struct _tuple33 _tmp728;_tmp728.f1=_tmp352,_tmp728.f2=_tmp353;_tmp728;});struct _tuple33 _tmp357=_tmp356;if(_tmp357.f1 != 0){if(_tmp357.f2 != 0){_LLAC: _LLAD:
# 2215
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp898=({void*_tmp897=*_tmp465;Cyc_Toc_cast_it_r(_tmp897,({struct Cyc_Absyn_Exp*_tmp358[1U];_tmp358[0]=_tmp464;({struct Cyc_Absyn_Exp*_tmp896=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp896,_tag_dyneither(_tmp358,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp898;});else{
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp464->r;}
# 2224
goto _LLAB;}else{_LLAE: _LLAF: {
# 2226
struct Cyc_Absyn_Exp*_tmp359=(struct Cyc_Absyn_Exp*)_check_null(_tmp352);
struct _tuple13 _tmp35A=Cyc_Evexp_eval_const_uint_exp(_tmp359);struct _tuple13 _tmp35B=_tmp35A;unsigned _tmp36B;int _tmp36A;_LLB5: _tmp36B=_tmp35B.f1;_tmp36A=_tmp35B.f2;_LLB6:;
if(Cyc_Toc_is_toplevel(nv)){
# 2232
if((_tmp354 && !(_tmp375.elt_tq).real_const)&& !_tmp355)
_tmp359=({struct Cyc_Absyn_Exp*_tmp899=_tmp359;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp899,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp89A=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp359,_tmp464))->r;e->r=_tmp89A;});}else{
# 2237
if(_tmp354){
# 2242
struct _tuple29 _tmp35C=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp35D=_tmp35C;struct _tuple1*_tmp368;struct Cyc_Absyn_Exp*_tmp367;_LLB8: _tmp368=_tmp35D.f1;_tmp367=_tmp35D.f2;_LLB9:;{
struct Cyc_Absyn_Exp*arg3;
# 2246
{void*_tmp35E=_tmp464->r;void*_tmp35F=_tmp35E;struct Cyc_Absyn_Vardecl*_tmp362;struct Cyc_Absyn_Vardecl*_tmp361;switch(*((int*)_tmp35F)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp35F)->f1).Wstring_c).tag){case 8U: _LLBB: _LLBC:
 arg3=_tmp359;goto _LLBA;case 9U: _LLBD: _LLBE:
 arg3=_tmp359;goto _LLBA;default: goto _LLC3;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35F)->f1)){case 1U: _LLBF: _tmp361=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35F)->f1)->f1;_LLC0:
 _tmp362=_tmp361;goto _LLC2;case 4U: _LLC1: _tmp362=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35F)->f1)->f1;_LLC2:
# 2251
 if(!Cyc_Tcutil_is_array_type(_tmp362->type))
goto _LLC4;
arg3=_tmp359;
goto _LLBA;default: goto _LLC3;}default: _LLC3: _LLC4:
# 2258
 if(_tmp36A && _tmp36B != (unsigned)1)
arg3=_tmp359;else{
# 2261
arg3=({struct Cyc_Absyn_Exp*_tmp360[2U];({
struct Cyc_Absyn_Exp*_tmp89C=({void*_tmp89B=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp89B,_tmp367);});_tmp360[0]=_tmp89C;}),_tmp360[1]=_tmp359;({struct Cyc_Absyn_Exp*_tmp89D=
# 2261
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp464);Cyc_Toc_fncall_exp_dl(_tmp89D,_tag_dyneither(_tmp360,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2263
goto _LLBA;}_LLBA:;}
# 2265
if(!_tmp355 && !(_tmp375.elt_tq).real_const)
# 2268
arg3=({struct Cyc_Absyn_Exp*_tmp89E=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp89E,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2270
struct Cyc_Absyn_Exp*_tmp363=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp375.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp364=({struct Cyc_Absyn_Exp*_tmp366[3U];_tmp366[0]=_tmp367,_tmp366[1]=_tmp363,_tmp366[2]=arg3;({struct Cyc_Absyn_Exp*_tmp89F=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp89F,_tag_dyneither(_tmp366,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*_tmp365=Cyc_Absyn_exp_stmt(_tmp364,0U);
_tmp365=({struct _tuple1*_tmp8A2=_tmp368;void*_tmp8A1=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp8A0=_tmp464;Cyc_Absyn_declare_stmt(_tmp8A2,_tmp8A1,_tmp8A0,_tmp365,0U);});
({void*_tmp8A3=Cyc_Toc_stmt_exp_r(_tmp365);e->r=_tmp8A3;});};};}else{
# 2277
({void*_tmp8A6=({struct Cyc_Absyn_Exp*_tmp369[3U];_tmp369[0]=_tmp464,({
# 2279
struct Cyc_Absyn_Exp*_tmp8A4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp375.elt_type),0U);_tmp369[1]=_tmp8A4;}),_tmp369[2]=_tmp359;({struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp8A5,_tag_dyneither(_tmp369,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2277
e->r=_tmp8A6;});}}
# 2282
goto _LLAB;}}}else{if(_tmp357.f2 != 0){_LLB0: _LLB1: {
# 2292 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp36C=(struct Cyc_Absyn_Exp*)_check_null(_tmp353);
if(_tmp354 && !_tmp355)
_tmp36C=({struct Cyc_Absyn_Exp*_tmp8A7=_tmp353;Cyc_Absyn_add_exp(_tmp8A7,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2299
struct Cyc_Absyn_Exp*_tmp36D=({struct Cyc_Absyn_Exp*_tmp36F[3U];_tmp36F[0]=_tmp464,({
# 2301
struct Cyc_Absyn_Exp*_tmp8A8=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp376.elt_type),0U);_tmp36F[1]=_tmp8A8;}),_tmp36F[2]=_tmp36C;({struct Cyc_Absyn_Exp*_tmp8A9=Cyc_Toc__untag_dyneither_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp8A9,_tag_dyneither(_tmp36F,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2303
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8AC=({struct Cyc_Absyn_Exp*_tmp36E[1U];({struct Cyc_Absyn_Exp*_tmp8AA=Cyc_Absyn_copy_exp(_tmp36D);_tmp36E[0]=_tmp8AA;});({struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp8AB,_tag_dyneither(_tmp36E,sizeof(struct Cyc_Absyn_Exp*),1U));});});_tmp36D->r=_tmp8AC;});
({void*_tmp8AD=Cyc_Toc_cast_it_r(*_tmp465,_tmp36D);e->r=_tmp8AD;});
goto _LLAB;};}}else{_LLB2: _LLB3:
# 2311
 if((_tmp354 && !_tmp355)&& !(_tmp375.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp370=0U;({struct _dyneither_ptr _tmp8AE=({const char*_tmp371="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp371,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp8AE,_tag_dyneither(_tmp370,sizeof(void*),0U));});});
({void*_tmp8B2=({struct Cyc_Absyn_Exp*_tmp372[3U];_tmp372[0]=_tmp464,({
struct Cyc_Absyn_Exp*_tmp8B0=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp376.elt_type),0U);_tmp372[1]=_tmp8B0;}),({
struct Cyc_Absyn_Exp*_tmp8AF=Cyc_Absyn_uint_exp(1U,0U);_tmp372[2]=_tmp8AF;});({struct Cyc_Absyn_Exp*_tmp8B1=Cyc_Toc__dyneither_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp8B1,_tag_dyneither(_tmp372,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2314
e->r=_tmp8B2;});}
# 2318
goto _LLAB;}}_LLAB:;}
# 2320
goto _LLA4;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp351.f2)->f1)->tag == 1U){_LLA7: _tmp377=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp351.f1)->f1;_LLA8:
# 2322
{struct Cyc_Absyn_Exp*_tmp373=({void*_tmp8B3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8B3,(_tmp377.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp374=_tmp373;if(_tmp374 == 0){_LLC6: _LLC7:
# 2324
({void*_tmp8B5=({struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_new_exp(_tmp464->r,_tmp464->loc);Cyc_Toc_aggrmember_exp_r(_tmp8B4,Cyc_Toc_curr_sp);});_tmp464->r=_tmp8B5;});
_tmp464->topt=new_typ_c;
goto _LLC5;}else{_LLC8: _LLC9:
 goto _LLC5;}_LLC5:;}
# 2329
goto _LLA4;}else{goto _LLA9;}default: goto _LLA9;}else{_LLA9: _LLAA:
# 2331
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp464->r;
goto _LLA4;}_LLA4:;}
# 2335
goto _LL7;}case 15U: _LL2C: _tmp466=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_LL2D:
# 2338
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp466);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp466)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp466,0,Cyc_Toc_address_lvalue,1);
# 2344
({void*_tmp8B7=({void*_tmp8B6=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8B6,_tmp466);});e->r=_tmp8B7;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp466->topt))))
# 2348
({void*_tmp8B9=({void*_tmp8B8=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8B8,_tmp466);});e->r=_tmp8B9;});}
# 2350
goto _LL7;case 16U: _LL2E: _tmp468=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp467=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL2F:
# 2357
 Cyc_Toc_exp_to_c(nv,_tmp467);{
# 2359
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp467->topt));
struct _tuple29 _tmp378=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp379=_tmp378;struct _tuple1*_tmp385;struct Cyc_Absyn_Exp*_tmp384;_LLCB: _tmp385=_tmp379.f1;_tmp384=_tmp379.f2;_LLCC:;{
struct Cyc_Absyn_Exp*lhs;
{void*_tmp37A=Cyc_Tcutil_compress(elt_typ);void*_tmp37B=_tmp37A;void*_tmp37C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37B)->tag == 4U){_LLCE: _tmp37C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37B)->f1).elt_type;_LLCF:
# 2364
 elt_typ=Cyc_Toc_typ_to_c(_tmp37C);
lhs=Cyc_Absyn_copy_exp(_tmp384);
goto _LLCD;}else{_LLD0: _LLD1:
# 2368
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp384),0U);
goto _LLCD;}_LLCD:;}{
# 2371
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp467);
struct _tuple1*_tmp37D=(unsigned)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp37E=(unsigned)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp37D),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)_tmp37E)
mexp=({struct Cyc_Absyn_Exp*_tmp37F[2U];_tmp37F[0]=_tmp37E,({struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp37F[1]=_tmp8BA;});({struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp8BB,_tag_dyneither(_tmp37F,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2378
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp384,0U),0U);}{
# 2381
struct Cyc_Absyn_Exp*_tmp380=Cyc_Toc_get_varsizeexp(_tmp467);
if(_tmp380 != 0)
mexp=Cyc_Absyn_add_exp(mexp,_tmp380,0U);
# 2385
if(_tmp468 == 0 || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2388
struct Cyc_Absyn_Exp*r=_tmp468;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 2392
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp381[3U];({
struct Cyc_Absyn_Exp*_tmp8BE=Cyc_Absyn_copy_exp(_tmp384);_tmp381[0]=_tmp8BE;}),({
struct Cyc_Absyn_Exp*_tmp8BD=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp381[1]=_tmp8BD;}),
(unsigned)_tmp37E?_tmp381[2]=_tmp37E:({struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Absyn_uint_exp(1U,0U);_tmp381[2]=_tmp8BC;});({struct Cyc_Absyn_Exp*_tmp8BF=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp8BF,_tag_dyneither(_tmp381,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2399
answer=Cyc_Absyn_copy_exp(_tmp384);}
({void*_tmp8C0=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382->tag=Cyc_Toc_Dest,_tmp382->f1=_tmp384;_tmp382;});e->annot=_tmp8C0;});{
struct Cyc_Absyn_Stmt*_tmp383=({struct _tuple1*_tmp8C4=_tmp385;void*_tmp8C3=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8C2=mexp;Cyc_Absyn_declare_stmt(_tmp8C4,_tmp8C3,_tmp8C2,({
struct Cyc_Absyn_Stmt*_tmp8C1=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp8C1,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2405
if((unsigned)array_len)
_tmp383=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp37D),Cyc_Absyn_uint_type,array_len,_tmp383,0U);
({void*_tmp8C5=Cyc_Toc_stmt_exp_r(_tmp383);e->r=_tmp8C5;});
if(_tmp380 != 0)
({void*_tmp8C7=({void*_tmp8C6=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp8C6,Cyc_Absyn_copy_exp(e));});e->r=_tmp8C7;});
goto _LL7;};};};};};};case 18U: _LL30: _tmp469=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_LL31:
# 2413
 Cyc_Toc_exp_to_c(nv,_tmp469);goto _LL7;case 17U: _LL32: _tmp46A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_LL33:
({void*_tmp8C9=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386->tag=17U,({void*_tmp8C8=Cyc_Toc_typ_to_c(_tmp46A);_tmp386->f1=_tmp8C8;});_tmp386;});e->r=_tmp8C9;});goto _LL7;case 19U: _LL34: _tmp46C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp46B=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL35: {
# 2416
void*_tmp387=_tmp46C;
struct Cyc_List_List*_tmp388=_tmp46B;
for(0;_tmp388 != 0;_tmp388=_tmp388->tl){
void*_tmp389=(void*)_tmp388->hd;void*_tmp38A=_tmp389;unsigned _tmp39C;struct _dyneither_ptr*_tmp39B;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp38A)->tag == 0U){_LLD3: _tmp39B=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp38A)->f1;_LLD4:
 goto _LLD2;}else{_LLD5: _tmp39C=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp38A)->f1;_LLD6:
# 2422
{void*_tmp38B=Cyc_Tcutil_compress(_tmp387);void*_tmp38C=_tmp38B;struct Cyc_List_List*_tmp39A;struct Cyc_List_List*_tmp399;struct Cyc_Absyn_Datatypefield*_tmp398;union Cyc_Absyn_AggrInfo _tmp397;switch(*((int*)_tmp38C)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38C)->f1)){case 20U: _LLD8: _tmp397=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38C)->f1)->f1;_LLD9: {
# 2424
struct Cyc_Absyn_Aggrdecl*_tmp38D=Cyc_Absyn_get_known_aggrdecl(_tmp397);
if(_tmp38D->impl == 0)
({void*_tmp38E=0U;({struct _dyneither_ptr _tmp8CA=({const char*_tmp38F="struct fields must be known";_tag_dyneither(_tmp38F,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8CA,_tag_dyneither(_tmp38E,sizeof(void*),0U));});});
_tmp399=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp38D->impl))->fields;goto _LLDB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38C)->f1)->f1).KnownDatatypefield).tag == 2){_LLDE: _tmp398=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38C)->f1)->f1).KnownDatatypefield).val).f2;_LLDF:
# 2438
 if(_tmp39C == (unsigned)0)
({void*_tmp8CB=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->tag=0U,_tmp393->f1=Cyc_Toc_tag_sp;_tmp393;}));_tmp388->hd=_tmp8CB;});else{
# 2441
_tmp387=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp398->typs,(int)(_tmp39C - (unsigned)1))).f2;
({void*_tmp8CD=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->tag=0U,({struct _dyneither_ptr*_tmp8CC=Cyc_Absyn_fieldname((int)_tmp39C);_tmp394->f1=_tmp8CC;});_tmp394;}));_tmp388->hd=_tmp8CD;});}
# 2444
goto _LLD7;}else{goto _LLE0;}default: goto _LLE0;}case 7U: _LLDA: _tmp399=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C)->f2;_LLDB: {
# 2429
struct Cyc_Absyn_Aggrfield*_tmp390=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp399,(int)_tmp39C);
({void*_tmp8CE=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->tag=0U,_tmp391->f1=_tmp390->name;_tmp391;}));_tmp388->hd=_tmp8CE;});
_tmp387=_tmp390->type;
goto _LLD7;}case 6U: _LLDC: _tmp39A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38C)->f1;_LLDD:
# 2434
({void*_tmp8D0=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->tag=0U,({struct _dyneither_ptr*_tmp8CF=Cyc_Absyn_fieldname((int)(_tmp39C + (unsigned)1));_tmp392->f1=_tmp8CF;});_tmp392;}));_tmp388->hd=_tmp8D0;});
_tmp387=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp39A,(int)_tmp39C)).f2;
goto _LLD7;default: _LLE0: _LLE1:
# 2445
({void*_tmp395=0U;({struct _dyneither_ptr _tmp8D1=({const char*_tmp396="impossible type for offsetof tuple index";_tag_dyneither(_tmp396,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D1,_tag_dyneither(_tmp395,sizeof(void*),0U));});});}_LLD7:;}
# 2447
goto _LLD2;}_LLD2:;}
# 2450
({void*_tmp8D3=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->tag=19U,({void*_tmp8D2=Cyc_Toc_typ_to_c(_tmp46C);_tmp39D->f1=_tmp8D2;}),_tmp39D->f2=_tmp46B;_tmp39D;});e->r=_tmp8D3;});
goto _LL7;}case 21U: _LL36: _tmp470=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp46F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp46E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3;_tmp46D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2EE)->f4;_LL37: {
# 2453
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp470->topt);
Cyc_Toc_exp_to_c(nv,_tmp470);
if(_tmp46E && _tmp46D)
({void*_tmp8D8=({struct Cyc_Absyn_Exp*_tmp8D7=_tmp470;void*_tmp8D6=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp8D5=e1_cyc_type;struct _dyneither_ptr*_tmp8D4=_tmp46F;Cyc_Toc_check_tagged_union(_tmp8D7,_tmp8D6,_tmp8D5,_tmp8D4,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2457
e->r=_tmp8D8;});
# 2459
if(is_poly)
({void*_tmp8DA=({void*_tmp8D9=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8D9,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp8DA;});
goto _LL7;}case 22U: _LL38: _tmp474=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp473=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp472=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3;_tmp471=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2EE)->f4;_LL39: {
# 2463
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(_tmp474->topt);
void*ta;
{void*_tmp39E=Cyc_Tcutil_compress(e1typ);void*_tmp39F=_tmp39E;struct Cyc_Absyn_PtrInfo _tmp3A2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->tag == 3U){_LLE3: _tmp3A2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->f1;_LLE4:
 ta=_tmp3A2.elt_type;goto _LLE2;}else{_LLE5: _LLE6:
({void*_tmp3A0=0U;({struct _dyneither_ptr _tmp8DB=({const char*_tmp3A1="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3A1,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8DB,_tag_dyneither(_tmp3A0,sizeof(void*),0U));});});}_LLE2:;}
# 2470
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp474,e->annot,0);
if(_tmp472 && _tmp471)
({void*_tmp8E0=({struct Cyc_Absyn_Exp*_tmp8DF=_tmp474;void*_tmp8DE=Cyc_Toc_typ_to_c(e1typ);void*_tmp8DD=ta;struct _dyneither_ptr*_tmp8DC=_tmp473;Cyc_Toc_check_tagged_union(_tmp8DF,_tmp8DE,_tmp8DD,_tmp8DC,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp8E0;});
if(is_poly && _tmp471)
({void*_tmp8E2=({void*_tmp8E1=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8E1,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp8E2;});
goto _LL7;}case 20U: _LL3A: _tmp475=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_LL3B:
# 2478
 did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp475,e->annot,0);
goto _LL7;case 23U: _LL3C: _tmp477=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp476=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL3D:
# 2482
{void*_tmp3A3=Cyc_Tcutil_compress((void*)_check_null(_tmp477->topt));void*_tmp3A4=_tmp3A3;struct Cyc_List_List*_tmp3AC;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A4)->tag == 6U){_LLE8: _tmp3AC=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A4)->f1;_LLE9:
# 2484
 Cyc_Toc_exp_to_c(nv,_tmp477);{
int _tmp3A5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp476);{
struct _tuple13 _tmp3A6=Cyc_Evexp_eval_const_uint_exp(_tmp476);struct _tuple13 _tmp3A7=_tmp3A6;unsigned _tmp3AB;int _tmp3AA;_LLED: _tmp3AB=_tmp3A7.f1;_tmp3AA=_tmp3A7.f2;_LLEE:;
if(!_tmp3AA)
({void*_tmp3A8=0U;({struct _dyneither_ptr _tmp8E3=({const char*_tmp3A9="unknown tuple subscript in translation to C";_tag_dyneither(_tmp3A9,sizeof(char),44U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp8E3,_tag_dyneither(_tmp3A8,sizeof(void*),0U));});});
({void*_tmp8E5=({struct Cyc_Absyn_Exp*_tmp8E4=_tmp477;Cyc_Toc_aggrmember_exp_r(_tmp8E4,Cyc_Absyn_fieldname((int)(_tmp3AB + (unsigned)1)));});e->r=_tmp8E5;});
goto _LLE7;};};}else{_LLEA: _LLEB:
# 2494
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,_tmp477,e->annot,_tmp476);
if(index_used)
({void*_tmp8E6=Cyc_Toc_deref_exp_r(_tmp477);e->r=_tmp8E6;});
goto _LLE7;};}_LLE7:;}
# 2500
goto _LL7;case 24U: _LL3E: _tmp478=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_LL3F:
# 2502
 if(!Cyc_Toc_is_toplevel(nv)){
# 2504
void*_tmp3AD=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp478));
{void*_tmp3AE=_tmp3AD;union Cyc_Absyn_AggrInfo _tmp3B3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f2 == 0){_LLF0: _tmp3B3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f1)->f1;_LLF1: {
# 2507
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3B3);
{int i=1;for(0;_tmp478 != 0;(_tmp478=_tmp478->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp478->hd);
dles=({struct Cyc_List_List*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));({struct _tuple19*_tmp8E8=({struct _dyneither_ptr*_tmp8E7=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp8E7,(struct Cyc_Absyn_Exp*)_tmp478->hd);});_tmp3AF->hd=_tmp8E8;}),_tmp3AF->tl=dles;_tmp3AF;});}}
# 2513
({void*_tmp8EA=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0->tag=29U,_tmp3B0->f1=sd->name,_tmp3B0->f2=0,({struct Cyc_List_List*_tmp8E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp3B0->f3=_tmp8E9;}),_tmp3B0->f4=sd;_tmp3B0;});e->r=_tmp8EA;});
e->topt=_tmp3AD;
goto _LLEF;}}else{goto _LLF2;}}else{goto _LLF2;}}else{_LLF2: _LLF3:
({void*_tmp3B1=0U;({struct _dyneither_ptr _tmp8EB=({const char*_tmp3B2="tuple type not an aggregate";_tag_dyneither(_tmp3B2,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8EB,_tag_dyneither(_tmp3B1,sizeof(void*),0U));});});}_LLEF:;}
# 2518
goto _LL7;}else{
# 2522
struct Cyc_List_List*dles=0;
for(0;_tmp478 != 0;_tmp478=_tmp478->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp478->hd);
dles=({struct Cyc_List_List*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));({struct _tuple19*_tmp8EC=({struct _tuple19*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4->f1=0,_tmp3B4->f2=(struct Cyc_Absyn_Exp*)_tmp478->hd;_tmp3B4;});_tmp3B5->hd=_tmp8EC;}),_tmp3B5->tl=dles;_tmp3B5;});}
# 2527
({void*_tmp8ED=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp8ED;});}
# 2529
goto _LL7;case 26U: _LL40: _tmp479=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_LL41:
# 2535
{struct Cyc_List_List*_tmp3B6=_tmp479;for(0;_tmp3B6 != 0;_tmp3B6=_tmp3B6->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3B6->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8EE=Cyc_Toc_unresolvedmem_exp_r(0,_tmp479);e->r=_tmp8EE;});
goto _LL7;case 27U: _LL42: _tmp47D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp47C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp47B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3;_tmp47A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EE)->f4;_LL43:
# 2543
 Cyc_Toc_exp_to_c(nv,_tmp47C);
Cyc_Toc_exp_to_c(nv,_tmp47B);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp3B7=Cyc_Evexp_eval_const_uint_exp(_tmp47C);struct _tuple13 _tmp3B8=_tmp3B7;unsigned _tmp3C2;int _tmp3C1;_LLF5: _tmp3C2=_tmp3B8.f1;_tmp3C1=_tmp3B8.f2;_LLF6:;{
void*_tmp3B9=Cyc_Toc_typ_to_c((void*)_check_null(_tmp47B->topt));
struct Cyc_List_List*es=0;
# 2550
if(!Cyc_Toc_is_zero(_tmp47B)){
if(!_tmp3C1)
({void*_tmp3BA=0U;({unsigned _tmp8F0=_tmp47C->loc;struct _dyneither_ptr _tmp8EF=({const char*_tmp3BB="cannot determine value of constant";_tag_dyneither(_tmp3BB,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp8F0,_tmp8EF,_tag_dyneither(_tmp3BA,sizeof(void*),0U));});});
{unsigned i=0U;for(0;i < _tmp3C2;++ i){
es=({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({struct _tuple19*_tmp8F1=({struct _tuple19*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->f1=0,_tmp3BC->f2=_tmp47B;_tmp3BC;});_tmp3BD->hd=_tmp8F1;}),_tmp3BD->tl=es;_tmp3BD;});}}
# 2556
if(_tmp47A){
struct Cyc_Absyn_Exp*_tmp3BE=({void*_tmp8F2=_tmp3B9;Cyc_Toc_cast_it(_tmp8F2,Cyc_Absyn_uint_exp(0U,0U));});
es=({struct Cyc_List_List*_tmp8F4=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8F4,({struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct _tuple19*_tmp8F3=({struct _tuple19*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->f1=0,_tmp3BF->f2=_tmp3BE;_tmp3BF;});_tmp3C0->hd=_tmp8F3;}),_tmp3C0->tl=0;_tmp3C0;}));});}}
# 2561
({void*_tmp8F5=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp8F5;});};}
# 2563
goto _LL7;case 28U: _LL44: _tmp480=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp47F=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp47E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3;_LL45:
# 2570
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8F6=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp8F6;});else{
# 2573
Cyc_Toc_exp_to_c(nv,_tmp480);}
goto _LL7;case 30U: _LL46: _tmp482=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp481=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL47:
# 2577
{struct Cyc_List_List*_tmp3C3=_tmp481;for(0;_tmp3C3 != 0;_tmp3C3=_tmp3C3->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3C3->hd)).f2);}}{
void*_tmp3C4=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp3C4;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3C5=Cyc_Tcutil_compress(_tmp3C4);void*_tmp3C6=_tmp3C5;union Cyc_Absyn_AggrInfo _tmp3CA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C6)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C6)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C6)->f2 == 0){_LLF8: _tmp3CA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C6)->f1)->f1;_LLF9: {
# 2584
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3CA);
({void*_tmp8F7=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->tag=29U,_tmp3C7->f1=sd->name,_tmp3C7->f2=0,_tmp3C7->f3=_tmp481,_tmp3C7->f4=sd;_tmp3C7;});e->r=_tmp8F7;});
e->topt=_tmp3C4;
goto _LLF7;}}else{goto _LLFA;}}else{goto _LLFA;}}else{_LLFA: _LLFB:
({void*_tmp3C8=0U;({struct _dyneither_ptr _tmp8F8=({const char*_tmp3C9="anonStruct type not an aggregate";_tag_dyneither(_tmp3C9,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F8,_tag_dyneither(_tmp3C8,sizeof(void*),0U));});});}_LLF7:;}else{
# 2591
({void*_tmp8F9=Cyc_Toc_unresolvedmem_exp_r(0,_tmp481);e->r=_tmp8F9;});}
goto _LL7;};case 29U: _LL48: _tmp486=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp485=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp484=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3;_tmp483=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2EE)->f4;_LL49:
# 2597
 if(_tmp483 == 0 || _tmp483->impl == 0)
({void*_tmp3CB=0U;({struct _dyneither_ptr _tmp8FA=({const char*_tmp3CC="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp3CC,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8FA,_tag_dyneither(_tmp3CB,sizeof(void*),0U));});});{
void*_tmp3CD=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp3CE=Cyc_Tcutil_compress(_tmp3CD);void*_tmp3CF=_tmp3CE;union Cyc_Absyn_AggrInfo _tmp3D2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CF)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CF)->f1)->tag == 20U){_LLFD: _tmp3D2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CF)->f1)->f1;_LLFE:
({struct _tuple1*_tmp8FB=(Cyc_Absyn_aggr_kinded_name(_tmp3D2)).f2;*_tmp486=_tmp8FB;});goto _LLFC;}else{goto _LLFF;}}else{_LLFF: _LL100:
({void*_tmp3D0=0U;({struct _dyneither_ptr _tmp8FC=({const char*_tmp3D1="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp3D1,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8FC,_tag_dyneither(_tmp3D0,sizeof(void*),0U));});});}_LLFC:;}{
# 2608
struct Cyc_List_List*_tmp3D3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp483->impl))->fields;
if(_tmp3D3 == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(_tmp3D3))->tl != 0;_tmp3D3=_tmp3D3->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp3D4=(struct Cyc_Absyn_Aggrfield*)_tmp3D3->hd;
struct Cyc_List_List*_tmp3D5=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp484,_tmp483->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp483->impl))->fields);
# 2615
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3D6=Cyc_Tcutil_compress(old_typ);void*_tmp3D7=_tmp3D6;struct Cyc_List_List*_tmp403;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->tag == 20U){_LL102: _tmp403=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f2;_LL103:
# 2619
{struct Cyc_List_List*_tmp3D8=_tmp3D5;for(0;_tmp3D8 != 0;_tmp3D8=_tmp3D8->tl){
struct _tuple34*_tmp3D9=(struct _tuple34*)_tmp3D8->hd;struct _tuple34*_tmp3DA=_tmp3D9;struct Cyc_Absyn_Aggrfield*_tmp400;struct Cyc_Absyn_Exp*_tmp3FF;_LL107: _tmp400=_tmp3DA->f1;_tmp3FF=_tmp3DA->f2;_LL108:;{
void*_tmp3DB=_tmp3FF->topt;
Cyc_Toc_exp_to_c(nv,_tmp3FF);
# 2624
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp400->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp3FF->topt)))
({void*_tmp8FE=({void*_tmp8FD=Cyc_Toc_typ_to_c(_tmp400->type);Cyc_Toc_cast_it(_tmp8FD,
Cyc_Absyn_copy_exp(_tmp3FF));})->r;
# 2626
_tmp3FF->r=_tmp8FE;});
# 2629
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp483->impl))->tagged){
struct _dyneither_ptr*_tmp3DC=_tmp400->name;
struct Cyc_Absyn_Exp*_tmp3DD=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(_tmp483,_tmp3DC),0U);
struct _tuple19*_tmp3DE=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp3DD);
struct _tuple19*_tmp3DF=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp3FF);
struct _tuple1*s=({struct _tuple1*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));({union Cyc_Absyn_Nmspace _tmp902=Cyc_Absyn_Abs_n(0,1);_tmp3EA->f1=_tmp902;}),({
struct _dyneither_ptr*_tmp901=({struct _dyneither_ptr*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));({struct _dyneither_ptr _tmp900=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3E7=({struct Cyc_String_pa_PrintArg_struct _tmp72B;_tmp72B.tag=0U,_tmp72B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp483->name).f2);_tmp72B;});struct Cyc_String_pa_PrintArg_struct _tmp3E8=({struct Cyc_String_pa_PrintArg_struct _tmp72A;_tmp72A.tag=0U,_tmp72A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3DC);_tmp72A;});void*_tmp3E5[2U];_tmp3E5[0]=& _tmp3E7,_tmp3E5[1]=& _tmp3E8;({struct _dyneither_ptr _tmp8FF=({const char*_tmp3E6="_union_%s_%s";_tag_dyneither(_tmp3E6,sizeof(char),13U);});Cyc_aprintf(_tmp8FF,_tag_dyneither(_tmp3E5,sizeof(void*),2U));});});*_tmp3E9=_tmp900;});_tmp3E9;});_tmp3EA->f2=_tmp901;});_tmp3EA;});
# 2637
struct _tuple19*_tmp3E0=({
struct _dyneither_ptr*_tmp904=_tmp3DC;Cyc_Toc_make_field(_tmp904,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->tag=29U,_tmp3E4->f1=s,_tmp3E4->f2=0,({struct Cyc_List_List*_tmp903=({struct _tuple19*_tmp3E3[2U];_tmp3E3[0]=_tmp3DE,_tmp3E3[1]=_tmp3DF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3E3,sizeof(struct _tuple19*),2U));});_tmp3E4->f3=_tmp903;}),_tmp3E4->f4=0;_tmp3E4;}),0U));});
# 2640
({void*_tmp906=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->tag=29U,_tmp3E2->f1=*_tmp486,_tmp3E2->f2=0,({struct Cyc_List_List*_tmp905=({struct _tuple19*_tmp3E1[1U];_tmp3E1[0]=_tmp3E0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3E1,sizeof(struct _tuple19*),1U));});_tmp3E2->f3=_tmp905;}),_tmp3E2->f4=_tmp483;_tmp3E2;});e->r=_tmp906;});}
# 2646
if(Cyc_Toc_is_abstract_type(old_typ)&& _tmp3D4 == _tmp400){
struct Cyc_List_List*_tmp3EB=({struct Cyc_List_List*_tmp907=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp483->impl))->exist_vars,_tmp485);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp907,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp483->tvs,_tmp403));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp3EC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp483->impl))->fields;for(0;_tmp3EC != 0;_tmp3EC=_tmp3EC->tl){
struct Cyc_Absyn_Aggrfield*_tmp3ED=(struct Cyc_Absyn_Aggrfield*)_tmp3EC->hd;
void*_tmp3EE=Cyc_Tcutil_substitute(_tmp3EB,_tmp3ED->type);
struct Cyc_Absyn_Aggrfield*_tmp3EF=({struct Cyc_Absyn_Aggrfield*_tmp908=_tmp3ED;Cyc_Toc_aggrfield_to_c(_tmp908,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp3EB,_tmp3EE)));});
new_fields=({struct Cyc_List_List*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0->hd=_tmp3EF,_tmp3F0->tl=new_fields;_tmp3F0;});
# 2661
if(_tmp3EC->tl == 0){
{void*_tmp3F1=Cyc_Tcutil_compress(_tmp3EF->type);void*_tmp3F2=_tmp3F1;struct Cyc_Absyn_ArrayInfo _tmp3F7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F2)->tag == 4U){_LL10A: _tmp3F7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F2)->f1;_LL10B:
# 2664
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3F7.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp3F3=_tmp3F7;
({struct Cyc_Absyn_Exp*_tmp909=Cyc_Absyn_uint_exp(0U,0U);_tmp3F3.num_elts=_tmp909;});
({void*_tmp90A=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->tag=4U,_tmp3F4->f1=_tmp3F3;_tmp3F4;});_tmp3EF->type=_tmp90A;});}
# 2669
goto _LL109;}else{_LL10C: _LL10D:
# 2673
 if(_tmp3FF->topt == 0)
goto _LL109;
{void*_tmp3F5=Cyc_Tcutil_compress((void*)_check_null(_tmp3FF->topt));void*_tmp3F6=_tmp3F5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F6)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F6)->f1)->tag == 20U){_LL10F: _LL110:
# 2677
 _tmp3EF->type=(void*)_check_null(_tmp3FF->topt);goto _LL10E;}else{goto _LL111;}}else{_LL111: _LL112:
 goto _LL10E;}_LL10E:;}
# 2680
goto _LL109;}_LL109:;}
# 2684
if(!Cyc_Tcutil_is_array_type(_tmp3ED->type)&&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3ED->type)))
({struct Cyc_List_List*_tmp90C=({struct Cyc_List_List*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));({void*_tmp90B=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8->tag=6U,_tmp3F8->f1=0;_tmp3F8;});_tmp3F9->hd=_tmp90B;}),_tmp3F9->tl=_tmp3EF->attributes;_tmp3F9;});_tmp3EF->attributes=_tmp90C;});}}}
# 2690
_tmp483=({struct _dyneither_ptr*_tmp90F=({struct _dyneither_ptr*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));({struct _dyneither_ptr _tmp90E=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3FC=({struct Cyc_Int_pa_PrintArg_struct _tmp72C;_tmp72C.tag=1U,_tmp72C.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp72C;});void*_tmp3FA[1U];_tmp3FA[0]=& _tmp3FC;({struct _dyneither_ptr _tmp90D=({const char*_tmp3FB="_genStruct%d";_tag_dyneither(_tmp3FB,sizeof(char),13U);});Cyc_aprintf(_tmp90D,_tag_dyneither(_tmp3FA,sizeof(void*),1U));});});*_tmp3FD=_tmp90E;});_tmp3FD;});Cyc_Toc_make_c_struct_defn(_tmp90F,
# 2692
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*_tmp486=_tmp483->name;
Cyc_Toc_aggrdecl_to_c(_tmp483);
# 2696
({void*_tmp910=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3FE=_cycalloc(sizeof(*_tmp3FE));*_tmp3FE=_tmp483;_tmp3FE;})),0);e->topt=_tmp910;});}};}}
# 2699
goto _LL101;}else{goto _LL104;}}else{_LL104: _LL105:
({void*_tmp401=0U;({struct _dyneither_ptr _tmp911=({const char*_tmp402="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp402,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp911,_tag_dyneither(_tmp401,sizeof(void*),0U));});});}_LL101:;}else{
# 2708
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp483->impl))->tagged){
# 2710
struct _tuple34*_tmp404=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp3D5))->hd;struct _tuple34*_tmp405=_tmp404;struct Cyc_Absyn_Aggrfield*_tmp40E;struct Cyc_Absyn_Exp*_tmp40D;_LL114: _tmp40E=_tmp405->f1;_tmp40D=_tmp405->f2;_LL115:;{
void*_tmp406=(void*)_check_null(_tmp40D->topt);
void*_tmp407=_tmp40E->type;
Cyc_Toc_exp_to_c(nv,_tmp40D);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp407)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp406))
({void*_tmp913=({
void*_tmp912=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp912,Cyc_Absyn_new_exp(_tmp40D->r,0U));});
# 2716
_tmp40D->r=_tmp913;});{
# 2719
int i=Cyc_Toc_get_member_offset(_tmp483,_tmp40E->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp408=({struct _tuple19*_tmp40A[2U];({struct _tuple19*_tmp915=({struct _tuple19*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->f1=0,_tmp40B->f2=field_tag_exp;_tmp40B;});_tmp40A[0]=_tmp915;}),({struct _tuple19*_tmp914=({struct _tuple19*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->f1=0,_tmp40C->f2=_tmp40D;_tmp40C;});_tmp40A[1]=_tmp914;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp40A,sizeof(struct _tuple19*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp408,0U);
({void*_tmp917=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp409[1U];({struct _tuple19*_tmp916=Cyc_Toc_make_field(_tmp40E->name,umem);_tmp409[0]=_tmp916;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp409,sizeof(struct _tuple19*),1U));}));e->r=_tmp917;});};};}else{
# 2726
struct Cyc_List_List*_tmp40F=0;
struct Cyc_List_List*_tmp410=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp483->impl))->fields;
for(0;_tmp410 != 0;_tmp410=_tmp410->tl){
struct Cyc_List_List*_tmp411=_tmp3D5;for(0;_tmp411 != 0;_tmp411=_tmp411->tl){
if((*((struct _tuple34*)_tmp411->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp410->hd){
struct _tuple34*_tmp412=(struct _tuple34*)_tmp411->hd;struct _tuple34*_tmp413=_tmp412;struct Cyc_Absyn_Aggrfield*_tmp419;struct Cyc_Absyn_Exp*_tmp418;_LL117: _tmp419=_tmp413->f1;_tmp418=_tmp413->f2;_LL118:;{
void*_tmp414=Cyc_Toc_typ_to_c(_tmp419->type);
void*_tmp415=Cyc_Toc_typ_to_c((void*)_check_null(_tmp418->topt));
Cyc_Toc_exp_to_c(nv,_tmp418);
# 2736
if(!Cyc_Tcutil_unify(_tmp414,_tmp415)){
# 2738
if(!Cyc_Tcutil_is_arithmetic_type(_tmp414)|| !
Cyc_Tcutil_is_arithmetic_type(_tmp415))
_tmp418=({void*_tmp918=_tmp414;Cyc_Toc_cast_it(_tmp918,Cyc_Absyn_copy_exp(_tmp418));});}
_tmp40F=({struct Cyc_List_List*_tmp417=_cycalloc(sizeof(*_tmp417));({struct _tuple19*_tmp919=({struct _tuple19*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->f1=0,_tmp416->f2=_tmp418;_tmp416;});_tmp417->hd=_tmp919;}),_tmp417->tl=_tmp40F;_tmp417;});
break;};}}}
# 2745
({void*_tmp91A=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp40F));e->r=_tmp91A;});}}
# 2748
goto _LL7;};};};case 31U: _LL4A: _tmp489=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp488=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_tmp487=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2EE)->f3;_LL4B: {
# 2752
struct Cyc_List_List*_tmp41A=_tmp487->typs;
{struct Cyc_List_List*_tmp41B=_tmp489;for(0;_tmp41B != 0;(_tmp41B=_tmp41B->tl,_tmp41A=_tmp41A->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp41B->hd;
void*_tmp41C=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp41A))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp41C))
({void*_tmp91B=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp91B;});}}{
# 2763
struct Cyc_Absyn_Exp*_tmp41D=
_tmp488->is_extensible?Cyc_Absyn_var_exp(_tmp487->name,0U): Cyc_Toc_datatype_tag(_tmp488,_tmp487->name);
# 2766
if(!Cyc_Toc_is_toplevel(nv)){
# 2768
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp489 != 0;(_tmp489=_tmp489->tl,++ i)){
dles=({struct Cyc_List_List*_tmp41E=_cycalloc(sizeof(*_tmp41E));({struct _tuple19*_tmp91D=({struct _dyneither_ptr*_tmp91C=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp91C,(struct Cyc_Absyn_Exp*)_tmp489->hd);});_tmp41E->hd=_tmp91D;}),_tmp41E->tl=dles;_tmp41E;});}}{
# 2774
struct _tuple19*_tmp41F=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp41D);
({void*_tmp921=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->tag=29U,({struct _tuple1*_tmp920=Cyc_Toc_collapse_qvars(_tmp487->name,_tmp488->name);_tmp421->f1=_tmp920;}),_tmp421->f2=0,({
struct Cyc_List_List*_tmp91F=({struct Cyc_List_List*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->hd=_tmp41F,({struct Cyc_List_List*_tmp91E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp420->tl=_tmp91E;});_tmp420;});_tmp421->f3=_tmp91F;}),_tmp421->f4=0;_tmp421;});
# 2775
e->r=_tmp921;});};}else{
# 2780
struct Cyc_List_List*_tmp422=0;
for(0;_tmp489 != 0;_tmp489=_tmp489->tl){
_tmp422=({struct Cyc_List_List*_tmp424=_cycalloc(sizeof(*_tmp424));({struct _tuple19*_tmp922=({struct _tuple19*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->f1=0,_tmp423->f2=(struct Cyc_Absyn_Exp*)_tmp489->hd;_tmp423;});_tmp424->hd=_tmp922;}),_tmp424->tl=_tmp422;_tmp424;});}
({void*_tmp925=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp426=_cycalloc(sizeof(*_tmp426));({struct _tuple19*_tmp924=({struct _tuple19*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->f1=0,_tmp425->f2=_tmp41D;_tmp425;});_tmp426->hd=_tmp924;}),({
struct Cyc_List_List*_tmp923=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp422);_tmp426->tl=_tmp923;});_tmp426;}));
# 2783
e->r=_tmp925;});}
# 2786
goto _LL7;};}case 32U: _LL4C: _LL4D:
# 2788
 goto _LL4F;case 33U: _LL4E: _LL4F:
 goto _LL7;case 34U: _LL50: _tmp48F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1).is_calloc;_tmp48E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1).rgn;_tmp48D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1).elt_type;_tmp48C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1).num_elts;_tmp48B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1).fat_result;_tmp48A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1).inline_call;_LL51: {
# 2792
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp48D)));
Cyc_Toc_exp_to_c(nv,_tmp48C);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=_tmp48C;
if(_tmp48B){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2801
if(_tmp48F){
if(_tmp48E != 0 && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp48E;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=({struct Cyc_Absyn_Exp*_tmp427[3U];_tmp427[0]=rgn,({
struct Cyc_Absyn_Exp*_tmp926=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp427[1]=_tmp926;}),_tmp427[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp927=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp927,_tag_dyneither(_tmp427,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2808
pexp=({void*_tmp929=*_tmp48D;struct Cyc_Absyn_Exp*_tmp928=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp929,_tmp928,num_elts2);});}}else{
# 2810
if(_tmp48E != 0 && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp48E;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp48A)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,num_elts2);else{
# 2816
pexp=Cyc_Toc_rmalloc_exp(rgn,num_elts2);}}else{
# 2818
pexp=Cyc_Toc_malloc_exp(*_tmp48D,num_elts2);}}
# 2820
if(_tmp48B){
struct Cyc_Absyn_Exp*elt_sz=_tmp48F?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp428[3U];_tmp428[0]=pexp,_tmp428[1]=elt_sz,_tmp428[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp92A=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp92A,_tag_dyneither(_tmp428,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp92D=(struct _tuple1*)_check_null(x);void*_tmp92C=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp92B=_tmp48C;Cyc_Absyn_declare_stmt(_tmp92D,_tmp92C,_tmp92B,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp92E=Cyc_Toc_stmt_exp_r(s);e->r=_tmp92E;});}else{
# 2827
e->r=pexp->r;}
goto _LL7;};}case 35U: _LL52: _tmp491=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp490=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL53: {
# 2837
void*e1_old_typ=(void*)_check_null(_tmp491->topt);
void*e2_old_typ=(void*)_check_null(_tmp490->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp429=0U;({struct _dyneither_ptr _tmp92F=({const char*_tmp42A="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp42A,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp92F,_tag_dyneither(_tmp429,sizeof(void*),0U));});});{
# 2845
unsigned _tmp42B=e->loc;
struct _tuple1*_tmp42C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp42D=Cyc_Absyn_var_exp(_tmp42C,_tmp42B);_tmp42D->topt=e1_old_typ;{
struct _tuple1*_tmp42E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_var_exp(_tmp42E,_tmp42B);_tmp42F->topt=e2_old_typ;{
# 2851
struct Cyc_Absyn_Exp*_tmp430=({struct Cyc_Absyn_Exp*_tmp931=Cyc_Tcutil_deep_copy_exp(1,_tmp491);struct Cyc_Absyn_Exp*_tmp930=_tmp42F;Cyc_Absyn_assign_exp(_tmp931,_tmp930,_tmp42B);});_tmp430->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp431=Cyc_Absyn_exp_stmt(_tmp430,_tmp42B);
struct Cyc_Absyn_Exp*_tmp432=({struct Cyc_Absyn_Exp*_tmp933=Cyc_Tcutil_deep_copy_exp(1,_tmp490);struct Cyc_Absyn_Exp*_tmp932=_tmp42D;Cyc_Absyn_assign_exp(_tmp933,_tmp932,_tmp42B);});_tmp432->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp433=Cyc_Absyn_exp_stmt(_tmp432,_tmp42B);
# 2856
struct Cyc_Absyn_Stmt*_tmp434=({struct _tuple1*_tmp93B=_tmp42C;void*_tmp93A=e1_old_typ;struct Cyc_Absyn_Exp*_tmp939=_tmp491;struct Cyc_Absyn_Stmt*_tmp938=({
struct _tuple1*_tmp937=_tmp42E;void*_tmp936=e2_old_typ;struct Cyc_Absyn_Exp*_tmp935=_tmp490;struct Cyc_Absyn_Stmt*_tmp934=
Cyc_Absyn_seq_stmt(_tmp431,_tmp433,_tmp42B);
# 2857
Cyc_Absyn_declare_stmt(_tmp937,_tmp936,_tmp935,_tmp934,_tmp42B);});
# 2856
Cyc_Absyn_declare_stmt(_tmp93B,_tmp93A,_tmp939,_tmp938,_tmp42B);});
# 2859
Cyc_Toc_stmt_to_c(nv,_tmp434);
({void*_tmp93C=Cyc_Toc_stmt_exp_r(_tmp434);e->r=_tmp93C;});
goto _LL7;};};};};};}case 38U: _LL54: _tmp493=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_tmp492=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2EE)->f2;_LL55: {
# 2864
void*_tmp435=Cyc_Tcutil_compress((void*)_check_null(_tmp493->topt));
Cyc_Toc_exp_to_c(nv,_tmp493);
{void*_tmp436=_tmp435;struct Cyc_Absyn_Aggrdecl*_tmp43C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->f1).KnownAggr).tag == 2){_LL11A: _tmp43C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->f1).KnownAggr).val;_LL11B: {
# 2868
struct Cyc_Absyn_Exp*_tmp437=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp43C,_tmp492),0U);
struct Cyc_Absyn_Exp*_tmp438=Cyc_Toc_member_exp(_tmp493,_tmp492,0U);
struct Cyc_Absyn_Exp*_tmp439=Cyc_Toc_member_exp(_tmp438,Cyc_Toc_tag_sp,0U);
({void*_tmp93D=(Cyc_Absyn_eq_exp(_tmp439,_tmp437,0U))->r;e->r=_tmp93D;});
goto _LL119;}}else{goto _LL11C;}}else{goto _LL11C;}}else{_LL11C: _LL11D:
({void*_tmp43A=0U;({struct _dyneither_ptr _tmp93E=({const char*_tmp43B="non-aggregate type in tagcheck";_tag_dyneither(_tmp43B,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp93E,_tag_dyneither(_tmp43A,sizeof(void*),0U));});});}_LL119:;}
# 2875
goto _LL7;}case 37U: _LL56: _tmp494=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2EE)->f1;_LL57:
 Cyc_Toc_stmt_to_c(nv,_tmp494);goto _LL7;case 36U: _LL58: _LL59:
# 2878
({void*_tmp43D=0U;({struct _dyneither_ptr _tmp93F=({const char*_tmp43E="UnresolvedMem";_tag_dyneither(_tmp43E,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp93F,_tag_dyneither(_tmp43D,sizeof(void*),0U));});});case 25U: _LL5A: _LL5B:
({void*_tmp43F=0U;({struct _dyneither_ptr _tmp940=({const char*_tmp440="compoundlit";_tag_dyneither(_tmp440,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp940,_tag_dyneither(_tmp43F,sizeof(void*),0U));});});case 39U: _LL5C: _LL5D:
({void*_tmp441=0U;({struct _dyneither_ptr _tmp941=({const char*_tmp442="valueof(-)";_tag_dyneither(_tmp442,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp941,_tag_dyneither(_tmp441,sizeof(void*),0U));});});default: _LL5E: _LL5F:
({void*_tmp443=0U;({struct _dyneither_ptr _tmp942=({const char*_tmp444="__asm__";_tag_dyneither(_tmp444,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp942,_tag_dyneither(_tmp443,sizeof(void*),0U));});});}_LL7:;}
# 2883
if(!did_inserted_checks)
({void*_tmp495=0U;({unsigned _tmp946=e->loc;struct _dyneither_ptr _tmp945=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp498=({struct Cyc_String_pa_PrintArg_struct _tmp72D;_tmp72D.tag=0U,({
struct _dyneither_ptr _tmp943=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp72D.f1=_tmp943;});_tmp72D;});void*_tmp496[1U];_tmp496[0]=& _tmp498;({struct _dyneither_ptr _tmp944=({const char*_tmp497="Toc did not examine an inserted check: %s";_tag_dyneither(_tmp497,sizeof(char),42U);});Cyc_aprintf(_tmp944,_tag_dyneither(_tmp496,sizeof(void*),1U));});});((int(*)(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp946,_tmp945,_tag_dyneither(_tmp495,sizeof(void*),0U));});});};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2912 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2914
return({struct _tuple35*_tmp499=_region_malloc(r,sizeof(*_tmp499));_tmp499->f1=0,({struct _dyneither_ptr*_tmp948=Cyc_Toc_fresh_label();_tmp499->f2=_tmp948;}),({struct _dyneither_ptr*_tmp947=Cyc_Toc_fresh_label();_tmp499->f3=_tmp947;}),_tmp499->f4=sc;_tmp499;});}
# 2919
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp49A=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 2925
if((int)(((_tmp49A->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp49D=_tmp49A->orig_pat;if((_tmp49D.pattern).tag != 1)_throw_match();(_tmp49D.pattern).val;})->topt);
void*_tmp49B=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp49C=_tmp49B;switch(*((int*)_tmp49C)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49C)->f1)){case 0U: _LL1: _LL2:
# 2930
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmp949=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp949,v);});goto _LL0;}_LL0:;}{
# 2936
void*_tmp49E=_tmp49A->access;void*_tmp49F=_tmp49E;struct Cyc_Absyn_Datatypedecl*_tmp4AA;struct Cyc_Absyn_Datatypefield*_tmp4A9;unsigned _tmp4A8;int _tmp4A7;struct _dyneither_ptr*_tmp4A6;unsigned _tmp4A5;switch(*((int*)_tmp49F)){case 0U: _LLA: _LLB:
# 2941
 goto _LL9;case 1U: _LLC: _LLD:
# 2946
 if(ps->tl != 0){
void*_tmp4A0=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp4A1=_tmp4A0;struct Cyc_Absyn_Datatypedecl*_tmp4A4;struct Cyc_Absyn_Datatypefield*_tmp4A3;unsigned _tmp4A2;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A1)->tag == 3U){_LL15: _tmp4A4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A1)->f1;_tmp4A3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A1)->f2;_tmp4A2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A1)->f3;_LL16:
# 2949
 ps=ps->tl;
v=({void*_tmp94B=({void*_tmp94A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4A3->name,_tmp4A4->name));Cyc_Absyn_cstar_type(_tmp94A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp94B,v);});
v=({struct Cyc_Absyn_Exp*_tmp94C=v;Cyc_Absyn_aggrarrow_exp(_tmp94C,Cyc_Absyn_fieldname((int)(_tmp4A2 + (unsigned)1)),0U);});
continue;}else{_LL17: _LL18:
# 2954
 goto _LL14;}_LL14:;}
# 2957
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 2U: _LLE: _tmp4A5=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp49F)->f1;_LLF:
 v=({struct Cyc_Absyn_Exp*_tmp94D=v;Cyc_Toc_member_exp(_tmp94D,Cyc_Absyn_fieldname((int)(_tmp4A5 + (unsigned)1)),0U);});goto _LL9;case 4U: _LL10: _tmp4A7=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp49F)->f1;_tmp4A6=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp49F)->f2;_LL11:
# 2961
 v=Cyc_Toc_member_exp(v,_tmp4A6,0U);
if(_tmp4A7)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;default: _LL12: _tmp4AA=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49F)->f1;_tmp4A9=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49F)->f2;_tmp4A8=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49F)->f3;_LL13:
# 2966
 v=({struct Cyc_Absyn_Exp*_tmp94E=v;Cyc_Toc_member_exp(_tmp94E,Cyc_Absyn_fieldname((int)(_tmp4A8 + (unsigned)1)),0U);});
goto _LL9;}_LL9:;};}
# 2970
return v;}
# 2975
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4AB=t;struct Cyc_Absyn_Datatypedecl*_tmp4C4;struct Cyc_Absyn_Datatypefield*_tmp4C3;struct _dyneither_ptr*_tmp4C2;int _tmp4C1;int _tmp4C0;struct Cyc_Absyn_Datatypedecl*_tmp4BF;struct Cyc_Absyn_Datatypefield*_tmp4BE;unsigned _tmp4BD;struct _dyneither_ptr _tmp4BC;int _tmp4BB;void*_tmp4BA;struct Cyc_Absyn_Enumfield*_tmp4B9;struct Cyc_Absyn_Enumdecl*_tmp4B8;struct Cyc_Absyn_Enumfield*_tmp4B7;struct Cyc_Absyn_Exp*_tmp4B6;switch(*((int*)_tmp4AB)){case 0U: _LL1: _tmp4B6=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4AB)->f1;_LL2:
# 2979
 if(_tmp4B6 == 0)return v;else{return _tmp4B6;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmp94F=v;Cyc_Absyn_eq_exp(_tmp94F,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmp950=v;Cyc_Absyn_neq_exp(_tmp950,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4B8=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4AB)->f1;_tmp4B7=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4AB)->f2;_LL8:
# 2983
 return({struct Cyc_Absyn_Exp*_tmp951=v;Cyc_Absyn_eq_exp(_tmp951,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->tag=32U,_tmp4AC->f1=_tmp4B8,_tmp4AC->f2=_tmp4B7;_tmp4AC;}),0U),0U);});case 4U: _LL9: _tmp4BA=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4AB)->f1;_tmp4B9=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4AB)->f2;_LLA:
# 2985
 return({struct Cyc_Absyn_Exp*_tmp952=v;Cyc_Absyn_eq_exp(_tmp952,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->tag=33U,_tmp4AD->f1=_tmp4BA,_tmp4AD->f2=_tmp4B9;_tmp4AD;}),0U),0U);});case 5U: _LLB: _tmp4BC=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4AB)->f1;_tmp4BB=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4AB)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmp953=v;Cyc_Absyn_eq_exp(_tmp953,Cyc_Absyn_float_exp(_tmp4BC,_tmp4BB,0U),0U);});case 6U: _LLD: _tmp4BD=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4AB)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmp954=v;Cyc_Absyn_eq_exp(_tmp954,Cyc_Absyn_signed_int_exp((int)_tmp4BD,0U),0U);});case 7U: _LLF: _tmp4C0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4AB)->f1;_tmp4BF=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4AB)->f2;_tmp4BE=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4AB)->f3;_LL10:
# 2991
 LOOP1: {
void*_tmp4AE=v->r;void*_tmp4AF=_tmp4AE;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;switch(*((int*)_tmp4AF)){case 14U: _LL16: _tmp4B0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AF)->f2;_LL17:
 v=_tmp4B0;goto LOOP1;case 20U: _LL18: _tmp4B1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4AF)->f1;_LL19:
 v=_tmp4B1;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2998
v=({void*_tmp956=({void*_tmp955=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4BE->name,_tmp4BF->name));Cyc_Absyn_cstar_type(_tmp955,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp956,v);});
return({struct Cyc_Absyn_Exp*_tmp957=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp957,Cyc_Absyn_uint_exp((unsigned)_tmp4C0,0U),0U);});case 8U: _LL11: _tmp4C2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4AB)->f1;_tmp4C1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4AB)->f2;_LL12:
# 3003
 v=({struct Cyc_Absyn_Exp*_tmp958=Cyc_Toc_member_exp(v,_tmp4C2,0U);Cyc_Toc_member_exp(_tmp958,Cyc_Toc_tag_sp,0U);});
return({struct Cyc_Absyn_Exp*_tmp959=v;Cyc_Absyn_eq_exp(_tmp959,Cyc_Absyn_signed_int_exp(_tmp4C1,0U),0U);});default: _LL13: _tmp4C4=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4AB)->f1;_tmp4C3=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4AB)->f2;_LL14:
# 3007
 LOOP2: {
void*_tmp4B2=v->r;void*_tmp4B3=_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B4;switch(*((int*)_tmp4B3)){case 14U: _LL1D: _tmp4B4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B3)->f2;_LL1E:
 v=_tmp4B4;goto LOOP2;case 20U: _LL1F: _tmp4B5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B3)->f1;_LL20:
 v=_tmp4B5;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3014
v=({void*_tmp95B=({void*_tmp95A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4C3->name,_tmp4C4->name));Cyc_Absyn_cstar_type(_tmp95A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp95B,v);});
return({struct Cyc_Absyn_Exp*_tmp95C=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp95C,Cyc_Absyn_var_exp(_tmp4C3->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3028
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3035
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3037
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple32 _tmp4C5=*((struct _tuple32*)ss->hd);struct _tuple32 _tmp4C6=_tmp4C5;void*_tmp4CC;_LL1: _tmp4CC=_tmp4C6.f1;_LL2:;{
void*_tmp4C7=_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CB;struct _dyneither_ptr*_tmp4CA;switch(*((int*)_tmp4C7)){case 3U: _LL4: _LL5:
# 3043
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3046
 continue;case 8U: _LLA: _tmp4CA=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4C7)->f1;_LLB:
# 3048
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8->tag=3U,_tmp4C8->f1=_tmp4CA;_tmp4C8;});
continue;case 7U: _LLC: _LLD:
# 3052
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp4CB=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4C7)->f1;if(_tmp4CB != 0){_LLF:
# 3056
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->tag=2U,_tmp4C9->f1=_tmp4CB;_tmp4C9;});
return({struct _tuple36 _tmp72E;_tmp72E.f1=0,_tmp72E.f2=k;_tmp72E;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp72F;_tmp72F.f1=0,_tmp72F.f2=k;_tmp72F;});}_LL3:;};}
# 3065
return({struct _tuple36 _tmp730;_tmp730.f1=c,_tmp730.f2=k;_tmp730;});}
# 3070
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4CD=p;int _tmp4D8;int _tmp4D7;unsigned _tmp4D6;void*_tmp4D5;struct Cyc_Absyn_Enumfield*_tmp4D4;struct Cyc_Absyn_Enumdecl*_tmp4D3;struct Cyc_Absyn_Enumfield*_tmp4D2;switch(*((int*)_tmp4CD)){case 3U: _LL1: _tmp4D3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4CD)->f1;_tmp4D2=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4CD)->f2;_LL2:
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE->tag=32U,_tmp4CE->f1=_tmp4D3,_tmp4CE->f2=_tmp4D2;_tmp4CE;}),0U);goto _LL0;case 4U: _LL3: _tmp4D5=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4CD)->f1;_tmp4D4=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4CD)->f2;_LL4:
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF->tag=33U,_tmp4CF->f1=_tmp4D5,_tmp4CF->f2=_tmp4D4;_tmp4CF;}),0U);goto _LL0;case 6U: _LL5: _tmp4D6=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4CD)->f1;_LL6:
 _tmp4D7=(int)_tmp4D6;goto _LL8;case 7U: _LL7: _tmp4D7=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4CD)->f1;_LL8:
 _tmp4D8=_tmp4D7;goto _LLA;case 8U: _LL9: _tmp4D8=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4CD)->f2;_LLA:
 e=Cyc_Absyn_uint_exp((unsigned)_tmp4D8,0U);goto _LL0;default: _LLB: _LLC:
({void*_tmp4D0=0U;({struct _dyneither_ptr _tmp95D=({const char*_tmp4D1="compile_pat_test_as_case!";_tag_dyneither(_tmp4D1,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp95D,_tag_dyneither(_tmp4D0,sizeof(void*),0U));});});}_LL0:;}
# 3080
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->tag=17U,_tmp4D9->f1=e;_tmp4D9;}),0U);}
# 3084
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3093
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3096
void*t=(void*)_check_null(p->topt);
void*_tmp4DA=p->r;void*_tmp4DB=_tmp4DA;union Cyc_Absyn_AggrInfo _tmp50C;struct Cyc_List_List*_tmp50B;struct Cyc_List_List*_tmp50A;struct Cyc_List_List*_tmp509;struct Cyc_Absyn_Pat*_tmp508;struct Cyc_Absyn_Datatypedecl*_tmp507;struct Cyc_Absyn_Datatypefield*_tmp506;struct Cyc_List_List*_tmp505;struct Cyc_Absyn_Vardecl*_tmp504;struct Cyc_Absyn_Pat*_tmp503;struct Cyc_Absyn_Vardecl*_tmp502;struct Cyc_Absyn_Vardecl*_tmp501;struct Cyc_Absyn_Pat*_tmp500;struct Cyc_Absyn_Vardecl*_tmp4FF;switch(*((int*)_tmp4DB)){case 2U: _LL1: _tmp4FF=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4DB)->f2;_LL2: {
# 3099
struct Cyc_Absyn_Pat*_tmp4DC=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp4DC->topt=p->topt;
_tmp501=_tmp4FF;_tmp500=_tmp4DC;goto _LL4;}case 1U: _LL3: _tmp501=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1;_tmp500=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4DB)->f2;_LL4:
# 3104
({struct Cyc_List_List*_tmp95E=({struct Cyc_List_List*_tmp4DD=_region_malloc(rgn,sizeof(*_tmp4DD));_tmp4DD->hd=_tmp501,_tmp4DD->tl=*decls;_tmp4DD;});*decls=_tmp95E;});{
struct Cyc_Absyn_Stmt*_tmp4DE=({struct Cyc_Absyn_Exp*_tmp95F=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->tag=4U,_tmp4DF->f1=_tmp501;_tmp4DF;}),0U);Cyc_Absyn_assign_stmt(_tmp95F,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmp960=_tmp4DE;Cyc_Toc_seq_stmt_opt(_tmp960,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp500));});};case 4U: _LL5: _tmp502=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4DB)->f2;_LL6:
# 3109
({struct Cyc_List_List*_tmp961=({struct Cyc_List_List*_tmp4E0=_region_malloc(rgn,sizeof(*_tmp4E0));_tmp4E0->hd=_tmp502,_tmp4E0->tl=*decls;_tmp4E0;});*decls=_tmp961;});
return({struct Cyc_Absyn_Exp*_tmp962=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->tag=4U,_tmp4E1->f1=_tmp502;_tmp4E1;}),0U);Cyc_Absyn_assign_stmt(_tmp962,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp504=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1;_tmp503=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4DB)->f2;_LLA:
# 3114
({struct Cyc_List_List*_tmp963=({struct Cyc_List_List*_tmp4E2=_region_malloc(rgn,sizeof(*_tmp4E2));_tmp4E2->hd=_tmp504,_tmp4E2->tl=*decls;_tmp4E2;});*decls=_tmp963;});
({void*_tmp964=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp504->type=_tmp964;});{
# 3117
struct Cyc_Absyn_Stmt*_tmp4E3=({struct Cyc_Absyn_Exp*_tmp967=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->tag=4U,_tmp4E4->f1=_tmp504;_tmp4E4;}),0U);Cyc_Absyn_assign_stmt(_tmp967,({
void*_tmp966=({void*_tmp965=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp965,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp966,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmp968=_tmp4E3;Cyc_Toc_seq_stmt_opt(_tmp968,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp503));});};case 9U: _LLB: _LLC:
# 3122
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1)->r)->tag == 8U){_LL17: _tmp507=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1)->r)->f1;_tmp506=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1)->r)->f2;_tmp505=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1)->r)->f3;_LL18:
# 3131
 if(_tmp505 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp506->name,_tmp507->name);
void*_tmp4E5=({void*_tmp969=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmp969,Cyc_Toc_mt_tq);});
path=Cyc_Toc_cast_it(_tmp4E5,path);{
int cnt=1;
struct Cyc_List_List*_tmp4E6=_tmp506->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp505 != 0;(_tmp505=_tmp505->tl,_tmp4E6=((struct Cyc_List_List*)_check_null(_tmp4E6))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4E7=(struct Cyc_Absyn_Pat*)_tmp505->hd;
if(_tmp4E7->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp4E8=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd)).f2;
void*_tmp4E9=(void*)_check_null(_tmp4E7->topt);
void*_tmp4EA=Cyc_Toc_typ_to_c_array(_tmp4E9);
struct Cyc_Absyn_Exp*_tmp4EB=({struct Cyc_Absyn_Exp*_tmp96A=path;Cyc_Absyn_aggrarrow_exp(_tmp96A,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp4E8)))
_tmp4EB=Cyc_Toc_cast_it(_tmp4EA,_tmp4EB);
s=({struct Cyc_Absyn_Stmt*_tmp96B=s;Cyc_Toc_seq_stmt_opt(_tmp96B,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4EB,_tmp4E7));});};}
# 3149
return s;};};}else{_LL21: _tmp508=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1;_LL22:
# 3194
 return({struct _RegionHandle*_tmp96F=rgn;struct Cyc_Toc_Env**_tmp96E=nv;struct Cyc_List_List**_tmp96D=decls;struct Cyc_Absyn_Exp*_tmp96C=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmp96F,_tmp96E,_tmp96D,_tmp96C,_tmp508);});}case 8U: _LL19: _tmp509=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4DB)->f3;_LL1A:
# 3151
 _tmp50A=_tmp509;goto _LL1C;case 5U: _LL1B: _tmp50A=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1;_LL1C: {
# 3153
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp50A != 0;(_tmp50A=_tmp50A->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4EC=(struct Cyc_Absyn_Pat*)_tmp50A->hd;
if(_tmp4EC->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp4ED=(void*)_check_null(_tmp4EC->topt);
struct _dyneither_ptr*_tmp4EE=Cyc_Absyn_fieldname(cnt);
s=({struct Cyc_Absyn_Stmt*_tmp974=s;Cyc_Toc_seq_stmt_opt(_tmp974,({struct _RegionHandle*_tmp973=rgn;struct Cyc_Toc_Env**_tmp972=nv;struct Cyc_List_List**_tmp971=decls;struct Cyc_Absyn_Exp*_tmp970=Cyc_Toc_member_exp(path,_tmp4EE,0U);Cyc_Toc_extract_pattern_vars(_tmp973,_tmp972,_tmp971,_tmp970,_tmp4EC);}));});};}
# 3163
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1 == 0){_LL1D: _LL1E:
({void*_tmp4EF=0U;({struct _dyneither_ptr _tmp975=({const char*_tmp4F0="unresolved aggregate pattern!";_tag_dyneither(_tmp4F0,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp975,_tag_dyneither(_tmp4EF,sizeof(void*),0U));});});}else{_LL1F: _tmp50C=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4DB)->f1;_tmp50B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4DB)->f3;_LL20: {
# 3167
struct Cyc_Absyn_Aggrdecl*_tmp4F1=Cyc_Absyn_get_known_aggrdecl(_tmp50C);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp50B != 0;_tmp50B=_tmp50B->tl){
struct _tuple37*_tmp4F2=(struct _tuple37*)_tmp50B->hd;
struct Cyc_Absyn_Pat*_tmp4F3=(*_tmp4F2).f2;
if(_tmp4F3->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp4F4=Cyc_Absyn_designatorlist_to_fieldname((*_tmp4F2).f1);
void*_tmp4F5=(void*)_check_null(_tmp4F3->topt);
void*_tmp4F6=Cyc_Toc_typ_to_c_array(_tmp4F5);
struct Cyc_Absyn_Exp*_tmp4F7=Cyc_Toc_member_exp(path,_tmp4F4,0U);
# 3179
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F1->impl))->tagged)_tmp4F7=Cyc_Toc_member_exp(_tmp4F7,Cyc_Toc_val_sp,0U);{
void*_tmp4F8=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F1->impl))->fields,_tmp4F4)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp4F8))
_tmp4F7=Cyc_Toc_cast_it(_tmp4F6,_tmp4F7);else{
if(!Cyc_Tcutil_is_array_type(_tmp4F8)&& Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4F8)))
# 3186
_tmp4F7=Cyc_Absyn_deref_exp(({void*_tmp976=Cyc_Absyn_cstar_type(_tmp4F6,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp976,
Cyc_Absyn_address_exp(_tmp4F7,0U));}),0U);}
# 3189
s=({struct Cyc_Absyn_Stmt*_tmp977=s;Cyc_Toc_seq_stmt_opt(_tmp977,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4F7,_tmp4F3));});};};}
# 3191
return s;}}case 15U: _LL23: _LL24:
# 3196
({void*_tmp4F9=0U;({struct _dyneither_ptr _tmp978=({const char*_tmp4FA="unknownid pat";_tag_dyneither(_tmp4FA,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp978,_tag_dyneither(_tmp4F9,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp4FB=0U;({struct _dyneither_ptr _tmp979=({const char*_tmp4FC="unknowncall pat";_tag_dyneither(_tmp4FC,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp979,_tag_dyneither(_tmp4FB,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp4FD=0U;({struct _dyneither_ptr _tmp97A=({const char*_tmp4FE="exp pat";_tag_dyneither(_tmp4FE,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp97A,_tag_dyneither(_tmp4FD,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3207
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3215
void*_tmp50D=dopt;struct Cyc_List_List*_tmp54B;struct Cyc_List_List*_tmp54A;void*_tmp549;struct Cyc_Tcpat_Rhs*_tmp548;if(_tmp50D == 0){_LL1: _LL2:
# 3217
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp50D)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp548=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp50D)->f1;_LL6:
# 3222
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp50E=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp50F=_tmp50E;int*_tmp516;struct _dyneither_ptr*_tmp515;struct _dyneither_ptr*_tmp514;struct Cyc_Absyn_Switch_clause*_tmp513;_LLA: _tmp516=(int*)& _tmp50F->f1;_tmp515=_tmp50F->f2;_tmp514=_tmp50F->f3;_tmp513=_tmp50F->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp510=_tmp513->body;
if(_tmp510 == _tmp548->rhs){
# 3227
if(*_tmp516)
return Cyc_Absyn_goto_stmt(_tmp515,0U);
*_tmp516=1;{
# 3231
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp97D=rgn;struct Cyc_List_List**_tmp97C=decls;struct Cyc_Absyn_Exp*_tmp97B=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp97D,& nv,_tmp97C,_tmp97B,_tmp513->pattern);});
# 3234
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp514,_tmp513->body,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(_tmp515,res,0U);
({struct Cyc_List_List*_tmp97F=({struct Cyc_List_List*_tmp512=_region_malloc(rgn,sizeof(*_tmp512));({struct _tuple38*_tmp97E=({struct _tuple38*_tmp511=_region_malloc(rgn,sizeof(*_tmp511));_tmp511->f1=nv,_tmp511->f2=_tmp514,_tmp511->f3=_tmp510;_tmp511;});_tmp512->hd=_tmp97E;}),_tmp512->tl=*bodies;_tmp512;});*bodies=_tmp97F;});
return res;};}};}
# 3242
({void*_tmp517=0U;({struct _dyneither_ptr _tmp980=({const char*_tmp518="couldn't find rhs!";_tag_dyneither(_tmp518,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp980,_tag_dyneither(_tmp517,sizeof(void*),0U));});});default: _LL7: _tmp54B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp50D)->f1;_tmp54A=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp50D)->f2;_tmp549=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp50D)->f3;_LL8: {
# 3245
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp549,lscs,v);
# 3247
struct Cyc_Absyn_Exp*_tmp519=({struct Cyc_List_List*_tmp981=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp54B);Cyc_Toc_compile_path(_tmp981,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp51A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp54A);
# 3250
struct _tuple36 _tmp51B=Cyc_Toc_admits_switch(_tmp51A);struct _tuple36 _tmp51C=_tmp51B;int _tmp547;void*_tmp546;_LLD: _tmp547=_tmp51C.f1;_tmp546=_tmp51C.f2;_LLE:;
if(_tmp547 > 1){
# 3254
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp52D=_cycalloc(sizeof(*_tmp52D));
({struct Cyc_Absyn_Switch_clause*_tmp983=({struct Cyc_Absyn_Switch_clause*_tmp52C=_cycalloc(sizeof(*_tmp52C));({struct Cyc_Absyn_Pat*_tmp982=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp52C->pattern=_tmp982;}),_tmp52C->pat_vars=0,_tmp52C->where_clause=0,_tmp52C->body=res,_tmp52C->loc=0U;_tmp52C;});_tmp52D->hd=_tmp983;}),_tmp52D->tl=0;_tmp52D;});
# 3257
for(0;_tmp51A != 0;_tmp51A=_tmp51A->tl){
struct _tuple32 _tmp51D=*((struct _tuple32*)_tmp51A->hd);struct _tuple32 _tmp51E=_tmp51D;void*_tmp524;void*_tmp523;_LL10: _tmp524=_tmp51E.f1;_tmp523=_tmp51E.f2;_LL11:;{
# 3260
struct Cyc_Absyn_Pat*_tmp51F=Cyc_Toc_compile_pat_test_as_case(_tmp524);
# 3262
struct Cyc_Absyn_Stmt*_tmp520=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp523,lscs,v);
# 3264
new_lscs=({struct Cyc_List_List*_tmp522=_cycalloc(sizeof(*_tmp522));({struct Cyc_Absyn_Switch_clause*_tmp984=({struct Cyc_Absyn_Switch_clause*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->pattern=_tmp51F,_tmp521->pat_vars=0,_tmp521->where_clause=0,_tmp521->body=_tmp520,_tmp521->loc=0U;_tmp521;});_tmp522->hd=_tmp984;}),_tmp522->tl=new_lscs;_tmp522;});};}
# 3266
{void*_tmp525=_tmp546;struct _dyneither_ptr*_tmp52A;switch(*((int*)_tmp525)){case 1U: _LL13: _LL14:
# 3269
 LOOP1: {
void*_tmp526=_tmp519->r;void*_tmp527=_tmp526;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp528;switch(*((int*)_tmp527)){case 14U: _LL1C: _tmp528=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp527)->f2;_LL1D:
 _tmp519=_tmp528;goto LOOP1;case 20U: _LL1E: _tmp529=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp527)->f1;_LL1F:
 _tmp519=_tmp529;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3275
_tmp519=Cyc_Absyn_deref_exp(({void*_tmp985=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp985,_tmp519);}),0U);goto _LL12;case 3U: _LL15: _tmp52A=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp525)->f1;_LL16:
# 3278
 _tmp519=({struct Cyc_Absyn_Exp*_tmp986=Cyc_Toc_member_exp(_tmp519,_tmp52A,0U);Cyc_Toc_member_exp(_tmp986,Cyc_Toc_tag_sp,0U);});goto _LL12;case 2U: _LL17: _LL18:
# 3280
 goto _LL1A;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3283
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->tag=10U,_tmp52B->f1=_tmp519,_tmp52B->f2=new_lscs,_tmp52B->f3=0;_tmp52B;}),0U);}else{
# 3287
void*_tmp52E=_tmp546;struct Cyc_Absyn_Exp*_tmp545;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp52E)->tag == 2U){_LL23: _tmp545=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp52E)->f1;_LL24: {
# 3289
struct Cyc_List_List*_tmp52F=_tmp51A;void*_tmp53E;struct Cyc_Tcpat_Rhs*_tmp53D;if(_tmp52F != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)((struct Cyc_List_List*)_tmp52F)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp52F)->tl == 0){_LL28: _tmp53E=((struct _tuple32*)_tmp52F->hd)->f1;_tmp53D=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)_tmp52F->hd)->f2)->f1;_LL29:
# 3295
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp530=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp531=_tmp530;int*_tmp538;struct _dyneither_ptr*_tmp537;struct _dyneither_ptr*_tmp536;struct Cyc_Absyn_Switch_clause*_tmp535;_LL2D: _tmp538=(int*)& _tmp531->f1;_tmp537=_tmp531->f2;_tmp536=_tmp531->f3;_tmp535=_tmp531->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp532=_tmp535->body;
if(_tmp532 == _tmp53D->rhs){
# 3300
if(*_tmp538)
return Cyc_Absyn_goto_stmt(_tmp537,0U);
*_tmp538=1;{
# 3304
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp989=rgn;struct Cyc_List_List**_tmp988=decls;struct Cyc_Absyn_Exp*_tmp987=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp989,& nv,_tmp988,_tmp987,_tmp535->pattern);});
# 3307
Cyc_Toc_exp_to_c(nv,_tmp545);{
# 3310
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp536,_tmp535->body,0U);
r=Cyc_Absyn_ifthenelse_stmt(_tmp545,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(_tmp537,r,0U);
({struct Cyc_List_List*_tmp98B=({struct Cyc_List_List*_tmp534=_region_malloc(rgn,sizeof(*_tmp534));({struct _tuple38*_tmp98A=({struct _tuple38*_tmp533=_region_malloc(rgn,sizeof(*_tmp533));_tmp533->f1=nv,_tmp533->f2=_tmp536,_tmp533->f3=_tmp532;_tmp533;});_tmp534->hd=_tmp98A;}),_tmp534->tl=*bodies;_tmp534;});*bodies=_tmp98B;});
return r;};};}};}
# 3319
({void*_tmp539=0U;({struct _dyneither_ptr _tmp98C=({const char*_tmp53A="couldn't find rhs!";_tag_dyneither(_tmp53A,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98C,_tag_dyneither(_tmp539,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp53B=0U;({struct _dyneither_ptr _tmp98D=({const char*_tmp53C="bad where clause in match compiler";_tag_dyneither(_tmp53C,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98D,_tag_dyneither(_tmp53B,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3324
 for(0;_tmp51A != 0;_tmp51A=_tmp51A->tl){
struct _tuple32 _tmp53F=*((struct _tuple32*)_tmp51A->hd);struct _tuple32 _tmp540=_tmp53F;void*_tmp544;void*_tmp543;_LL30: _tmp544=_tmp540.f1;_tmp543=_tmp540.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp541=Cyc_Toc_compile_pat_test(_tmp519,_tmp544);
struct Cyc_Absyn_Stmt*_tmp542=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp543,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp541,_tmp542,res,0U);};}}_LL22:;}
# 3332
return res;}}}_LL0:;}
# 3342
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3344
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp54C=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp54D=_tmp54C;struct Cyc_Toc_Env**_tmp54F;struct Cyc_Absyn_Stmt*_tmp54E;_LL1: _tmp54F=(struct Cyc_Toc_Env**)& _tmp54D->f1;_tmp54E=_tmp54D->f3;_LL2:;
if(_tmp54E == s)return _tmp54F;}
# 3350
return 0;}
# 3354
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3357
void*_tmp550=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3360
struct _tuple29 _tmp551=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp552=_tmp551;struct _tuple1*_tmp563;struct Cyc_Absyn_Exp*_tmp562;_LL1: _tmp563=_tmp552.f1;_tmp562=_tmp552.f2;_LL2:;{
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp553=_new_region("rgn");struct _RegionHandle*rgn=& _tmp553;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp554=Cyc_Toc_share_env(rgn,nv);
# 3366
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3371
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp554,& mydecls,& mybodies,dopt,lscs,_tmp563);
# 3379
{struct Cyc_List_List*_tmp555=lscs;for(0;_tmp555 != 0;_tmp555=_tmp555->tl){
struct _tuple35*_tmp556=(struct _tuple35*)_tmp555->hd;struct _tuple35*_tmp557=_tmp556;struct _dyneither_ptr*_tmp55F;struct Cyc_Absyn_Switch_clause*_tmp55E;_LL4: _tmp55F=_tmp557->f3;_tmp55E=_tmp557->f4;_LL5:;{
struct Cyc_Absyn_Stmt*s=_tmp55E->body;
# 3383
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3386
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp558=*envp;
# 3389
if(_tmp555->tl != 0){
struct _tuple35*_tmp559=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp555->tl))->hd;struct _tuple35*_tmp55A=_tmp559;struct _dyneither_ptr*_tmp55D;struct Cyc_Absyn_Switch_clause*_tmp55C;_LL7: _tmp55D=_tmp55A->f3;_tmp55C=_tmp55A->f4;_LL8:;{
# 3393
struct Cyc_Toc_Env**_tmp55B=Cyc_Toc_find_case_env(mybodies,_tmp55C->body);
# 3398
if(_tmp55B == 0)
({struct Cyc_Toc_Env*_tmp98E=Cyc_Toc_last_switchclause_env(rgn,_tmp558,end_l);Cyc_Toc_stmt_to_c(_tmp98E,s);});else{
# 3402
struct Cyc_List_List*vs=0;
if(_tmp55C->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp55C->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 3407
({struct Cyc_Toc_Env*_tmp98F=Cyc_Toc_non_last_switchclause_env(rgn,_tmp558,end_l,_tmp55D,vs);Cyc_Toc_stmt_to_c(_tmp98F,s);});}};}else{
# 3411
({struct Cyc_Toc_Env*_tmp990=Cyc_Toc_last_switchclause_env(rgn,_tmp558,end_l);Cyc_Toc_stmt_to_c(_tmp990,s);});}};};}}{
# 3415
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmp992=test_tree;Cyc_Absyn_seq_stmt(_tmp992,({struct _dyneither_ptr*_tmp991=end_l;Cyc_Absyn_label_stmt(_tmp991,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3417
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp560=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmp993=Cyc_Toc_temp_var();_tmp560->name=_tmp993;});
({void*_tmp994=Cyc_Toc_typ_to_c_array(_tmp560->type);_tmp560->type=_tmp994;});
res=({struct Cyc_Absyn_Decl*_tmp995=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->tag=0U,_tmp561->f1=_tmp560;_tmp561;}),0U);Cyc_Absyn_decl_stmt(_tmp995,res,0U);});}
# 3424
({void*_tmp999=({struct _tuple1*_tmp998=_tmp563;void*_tmp997=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmp996=e;Cyc_Absyn_declare_stmt(_tmp998,_tmp997,_tmp996,res,0U);})->r;whole_s->r=_tmp999;});
_npop_handler(0U);return;};}
# 3363
;_pop_region(rgn);};};}
# 3430
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3432
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3437
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3439
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp564[1U];({struct Cyc_Absyn_Exp*_tmp99A=Cyc_Absyn_uint_exp((unsigned)(n - 1),0U);_tmp564[0]=_tmp99A;});({struct Cyc_Absyn_Exp*_tmp99B=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmp99B,_tag_dyneither(_tmp564,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3442
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmp99D=({struct Cyc_Absyn_Stmt*_tmp99C=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmp99C,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmp99D;});}
# 3447
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3449
while(1){
void*_tmp565=s->r;void*_tmp566=_tmp565;struct Cyc_Absyn_Stmt*_tmp5CD;struct Cyc_List_List*_tmp5CC;void*_tmp5CB;struct Cyc_Absyn_Stmt*_tmp5CA;struct Cyc_Absyn_Exp*_tmp5C9;struct _dyneither_ptr*_tmp5C8;struct Cyc_Absyn_Stmt*_tmp5C7;struct Cyc_Absyn_Decl*_tmp5C6;struct Cyc_Absyn_Stmt*_tmp5C5;struct Cyc_List_List*_tmp5C4;struct Cyc_Absyn_Switch_clause**_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;struct Cyc_List_List*_tmp5C1;void*_tmp5C0;struct Cyc_Absyn_Exp*_tmp5BF;struct Cyc_Absyn_Exp*_tmp5BE;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Stmt*_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BB;struct Cyc_Absyn_Stmt*_tmp5BA;struct Cyc_Absyn_Exp*_tmp5B9;struct Cyc_Absyn_Stmt*_tmp5B8;struct Cyc_Absyn_Stmt*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B6;struct Cyc_Absyn_Stmt*_tmp5B5;struct Cyc_Absyn_Stmt*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B3;switch(*((int*)_tmp566)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp5B3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_LL4:
 Cyc_Toc_exp_to_c(nv,_tmp5B3);return;case 2U: _LL5: _tmp5B5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_tmp5B4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp566)->f2;_LL6:
# 3454
 Cyc_Toc_stmt_to_c(nv,_tmp5B5);
s=_tmp5B4;
continue;case 3U: _LL7: _tmp5B6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_LL8:
# 3458
 if(_tmp5B6 != 0){
void*t=Cyc_Toc_typ_to_c((void*)_check_null(_tmp5B6->topt));
Cyc_Toc_exp_to_c(nv,_tmp5B6);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
# 3464
struct _tuple1*_tmp567=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp568=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp567,0U),0U);
Cyc_Toc_do_npop_before(npop,_tmp568);
({void*_tmp99E=(Cyc_Absyn_declare_stmt(_tmp567,t,_tmp5B6,_tmp568,0U))->r;s->r=_tmp99E;});}};}else{
# 3470
({int _tmp99F=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp99F,s);});}
return;case 4U: _LL9: _tmp5B9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_tmp5B8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp566)->f2;_tmp5B7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp566)->f3;_LLA:
# 3473
 Cyc_Toc_exp_to_c(nv,_tmp5B9);
Cyc_Toc_stmt_to_c(nv,_tmp5B8);
s=_tmp5B7;
continue;case 5U: _LLB: _tmp5BB=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp566)->f1).f1;_tmp5BA=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp566)->f2;_LLC:
# 3478
 Cyc_Toc_exp_to_c(nv,_tmp5BB);{
struct _RegionHandle _tmp569=_new_region("temp");struct _RegionHandle*temp=& _tmp569;_push_region(temp);
({struct Cyc_Toc_Env*_tmp9A0=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp9A0,_tmp5BA);});
# 3482
_npop_handler(0U);return;
# 3479
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3484
struct Cyc_Toc_Env*_tmp56A=nv;struct _dyneither_ptr**_tmp56B;_LL22: _tmp56B=_tmp56A->break_lab;_LL23:;
if(_tmp56B != 0)
({void*_tmp9A1=Cyc_Toc_goto_stmt_r(*_tmp56B);s->r=_tmp9A1;});
# 3488
({int _tmp9A2=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9A2,s);});
return;}case 7U: _LLF: _LL10: {
# 3491
struct Cyc_Toc_Env*_tmp56C=nv;struct _dyneither_ptr**_tmp56D;_LL25: _tmp56D=_tmp56C->continue_lab;_LL26:;
if(_tmp56D != 0)
({void*_tmp9A3=Cyc_Toc_goto_stmt_r(*_tmp56D);s->r=_tmp9A3;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3497
({int _tmp9A4=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9A4,s);});
return;case 9U: _LL13: _tmp5BF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_tmp5BE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp566)->f2).f1;_tmp5BD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp566)->f3).f1;_tmp5BC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp566)->f4;_LL14:
# 3501
 Cyc_Toc_exp_to_c(nv,_tmp5BF);Cyc_Toc_exp_to_c(nv,_tmp5BE);Cyc_Toc_exp_to_c(nv,_tmp5BD);{
struct _RegionHandle _tmp56E=_new_region("temp");struct _RegionHandle*temp=& _tmp56E;_push_region(temp);
({struct Cyc_Toc_Env*_tmp9A5=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp9A5,_tmp5BC);});
# 3505
_npop_handler(0U);return;
# 3502
;_pop_region(temp);};case 10U: _LL15: _tmp5C2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_tmp5C1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp566)->f2;_tmp5C0=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp566)->f3;_LL16:
# 3507
 Cyc_Toc_xlate_switch(nv,s,_tmp5C2,_tmp5C1,_tmp5C0);
return;case 11U: _LL17: _tmp5C4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_tmp5C3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp566)->f2;_LL18: {
# 3510
struct Cyc_Toc_Env*_tmp56F=nv;struct Cyc_Toc_FallthruInfo*_tmp579;_LL28: _tmp579=_tmp56F->fallthru_info;_LL29:;
if(_tmp579 == 0)
({void*_tmp570=0U;({struct _dyneither_ptr _tmp9A6=({const char*_tmp571="fallthru in unexpected place";_tag_dyneither(_tmp571,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9A6,_tag_dyneither(_tmp570,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp572=*_tmp579;struct Cyc_Toc_FallthruInfo _tmp573=_tmp572;struct _dyneither_ptr*_tmp578;struct Cyc_List_List*_tmp577;_LL2B: _tmp578=_tmp573.label;_tmp577=_tmp573.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp578,0U);
# 3516
({int _tmp9A7=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9A7,s2);});{
struct Cyc_List_List*_tmp574=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp577);
struct Cyc_List_List*_tmp575=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5C4);
for(0;_tmp574 != 0;(_tmp574=_tmp574->tl,_tmp575=_tmp575->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp575))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmp9A9=({struct Cyc_Absyn_Exp*_tmp9A8=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->tag=5U,_tmp576->f1=(struct Cyc_Absyn_Vardecl*)_tmp574->hd;_tmp576;}),0U);Cyc_Absyn_assign_stmt(_tmp9A8,(struct Cyc_Absyn_Exp*)_tmp575->hd,0U);});Cyc_Absyn_seq_stmt(_tmp9A9,s2,0U);});}
# 3524
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp5C6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_tmp5C5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp566)->f2;_LL1A:
# 3530
{void*_tmp57A=_tmp5C6->r;void*_tmp57B=_tmp57A;struct Cyc_Absyn_Tvar*_tmp59A;struct Cyc_Absyn_Vardecl*_tmp599;struct Cyc_Absyn_Exp*_tmp598;struct Cyc_Absyn_Fndecl*_tmp597;struct Cyc_List_List*_tmp596;struct Cyc_Absyn_Pat*_tmp595;struct Cyc_Absyn_Exp*_tmp594;void*_tmp593;struct Cyc_Absyn_Vardecl*_tmp592;switch(*((int*)_tmp57B)){case 0U: _LL2E: _tmp592=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp592,_tmp5C5);goto _LL2D;case 2U: _LL30: _tmp595=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_tmp594=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp57B)->f3;_tmp593=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp57B)->f4;_LL31:
# 3538
{void*_tmp57C=_tmp595->r;void*_tmp57D=_tmp57C;struct Cyc_Absyn_Vardecl*_tmp57F;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp57D)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp57D)->f2)->r)->tag == 0U){_LL3B: _tmp57F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp57D)->f1;_LL3C:
# 3540
({struct _tuple1*_tmp9AA=Cyc_Toc_temp_var();_tmp57F->name=_tmp9AA;});
_tmp57F->initializer=_tmp594;
({void*_tmp9AB=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->tag=0U,_tmp57E->f1=_tmp57F;_tmp57E;});_tmp5C6->r=_tmp9AB;});
Cyc_Toc_local_decl_to_c(nv,_tmp57F,_tmp5C5);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3549
({void*_tmp9AC=(Cyc_Toc_letdecl_to_c(nv,_tmp595,_tmp593,(void*)_check_null(_tmp594->topt),_tmp594,_tmp5C5))->r;s->r=_tmp9AC;});
goto _LL3A;}_LL3A:;}
# 3552
goto _LL2D;case 3U: _LL32: _tmp596=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_LL33: {
# 3556
struct Cyc_List_List*_tmp580=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp596);
if(_tmp580 == 0)
({void*_tmp581=0U;({struct _dyneither_ptr _tmp9AD=({const char*_tmp582="empty Letv_d";_tag_dyneither(_tmp582,sizeof(char),13U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp9AD,_tag_dyneither(_tmp581,sizeof(void*),0U));});});
({void*_tmp9AE=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->tag=0U,_tmp583->f1=(struct Cyc_Absyn_Vardecl*)_tmp580->hd;_tmp583;});_tmp5C6->r=_tmp9AE;});
_tmp580=_tmp580->tl;
for(0;_tmp580 != 0;_tmp580=_tmp580->tl){
struct Cyc_Absyn_Decl*_tmp584=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->tag=0U,_tmp585->f1=(struct Cyc_Absyn_Vardecl*)_tmp580->hd;_tmp585;}),0U);
({void*_tmp9B0=({struct Cyc_Absyn_Decl*_tmp9AF=_tmp584;Cyc_Absyn_decl_stmt(_tmp9AF,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmp9B0;});}
# 3565
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp597=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_LL35:
# 3568
 Cyc_Toc_fndecl_to_c(nv,_tmp597,0);
Cyc_Toc_stmt_to_c(nv,_tmp5C5);
goto _LL2D;case 4U: _LL36: _tmp59A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_tmp599=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp57B)->f2;_tmp598=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp57B)->f3;_LL37: {
# 3572
struct Cyc_Absyn_Stmt*_tmp586=_tmp5C5;
# 3574
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple29 _tmp587=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp588=_tmp587;struct _tuple1*_tmp58F;struct Cyc_Absyn_Exp*_tmp58E;_LL40: _tmp58F=_tmp588.f1;_tmp58E=_tmp588.f2;_LL41:;{
struct _tuple1*x_var=_tmp599->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3580
Cyc_Toc_stmt_to_c(nv,_tmp586);
if(Cyc_Absyn_no_regions)
({void*_tmp9B4=({struct _tuple1*_tmp9B3=x_var;void*_tmp9B2=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmp9B1=
Cyc_Absyn_uint_exp(0U,0U);
# 3582
Cyc_Absyn_declare_stmt(_tmp9B3,_tmp9B2,_tmp9B1,_tmp586,0U);})->r;s->r=_tmp9B4;});else{
# 3584
if((unsigned)_tmp598){
# 3586
Cyc_Toc_exp_to_c(nv,_tmp598);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmp9B6=_tmp598;Cyc_Absyn_aggrarrow_exp(_tmp9B6,({struct _dyneither_ptr*_tmp58A=_cycalloc(sizeof(*_tmp58A));({struct _dyneither_ptr _tmp9B5=({const char*_tmp589="h";_tag_dyneither(_tmp589,sizeof(char),2U);});*_tmp58A=_tmp9B5;});_tmp58A;}),0U);}),0U);
({void*_tmp9B7=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp586,0U))->r;s->r=_tmp9B7;});};}else{
# 3595
({void*_tmp9C4=({
struct _tuple1*_tmp9C3=_tmp58F;void*_tmp9C2=rh_struct_typ;struct Cyc_Absyn_Exp*_tmp9C1=({struct Cyc_Absyn_Exp*_tmp58B[1U];({struct Cyc_Absyn_Exp*_tmp9B8=
# 3598
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp58B[0]=_tmp9B8;});({struct Cyc_Absyn_Exp*_tmp9B9=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmp9B9,_tag_dyneither(_tmp58B,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3596
Cyc_Absyn_declare_stmt(_tmp9C3,_tmp9C2,_tmp9C1,({
# 3599
struct _tuple1*_tmp9C0=x_var;void*_tmp9BF=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmp9BE=Cyc_Absyn_address_exp(_tmp58E,0U);Cyc_Absyn_declare_stmt(_tmp9C0,_tmp9BF,_tmp9BE,({
struct Cyc_Absyn_Stmt*_tmp9BD=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp58C[1U];_tmp58C[0]=x_exp;({struct Cyc_Absyn_Exp*_tmp9BA=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmp9BA,_tag_dyneither(_tmp58C,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmp9BD,({
struct Cyc_Absyn_Stmt*_tmp9BC=_tmp586;Cyc_Absyn_seq_stmt(_tmp9BC,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp58D[1U];_tmp58D[0]=x_exp;({struct Cyc_Absyn_Exp*_tmp9BB=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmp9BB,_tag_dyneither(_tmp58D,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3595
s->r=_tmp9C4;});}}
# 3605
return;};}default: _LL38: _LL39:
({void*_tmp590=0U;({struct _dyneither_ptr _tmp9C5=({const char*_tmp591="bad nested declaration within function";_tag_dyneither(_tmp591,sizeof(char),39U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp9C5,_tag_dyneither(_tmp590,sizeof(void*),0U));});});}_LL2D:;}
# 3608
return;case 13U: _LL1B: _tmp5C8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_tmp5C7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp566)->f2;_LL1C:
 s=_tmp5C7;continue;case 14U: _LL1D: _tmp5CA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_tmp5C9=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp566)->f2).f1;_LL1E: {
# 3611
struct _RegionHandle _tmp59B=_new_region("temp");struct _RegionHandle*temp=& _tmp59B;_push_region(temp);
({struct Cyc_Toc_Env*_tmp9C6=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp9C6,_tmp5CA);});
Cyc_Toc_exp_to_c(nv,_tmp5C9);
# 3615
_npop_handler(0U);return;
# 3611
;_pop_region(temp);}default: _LL1F: _tmp5CD=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp566)->f1;_tmp5CC=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp566)->f2;_tmp5CB=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp566)->f3;_LL20: {
# 3629 "toc.cyc"
struct _tuple29 _tmp59C=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp59D=_tmp59C;struct _tuple1*_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B1;_LL43: _tmp5B2=_tmp59D.f1;_tmp5B1=_tmp59D.f2;_LL44:;{
struct _tuple29 _tmp59E=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp59F=_tmp59E;struct _tuple1*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5AF;_LL46: _tmp5B0=_tmp59F.f1;_tmp5AF=_tmp59F.f2;_LL47:;{
struct _tuple29 _tmp5A0=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp5A1=_tmp5A0;struct _tuple1*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;_LL49: _tmp5AE=_tmp5A1.f1;_tmp5AD=_tmp5A1.f2;_LL4A:;{
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3636
_tmp5AF->topt=e_typ;{
struct _RegionHandle _tmp5A2=_new_region("temp");struct _RegionHandle*temp=& _tmp5A2;_push_region(temp);
# 3639
Cyc_Toc_stmt_to_c(nv,_tmp5CD);{
struct Cyc_Absyn_Stmt*_tmp5A3=({struct Cyc_Absyn_Stmt*_tmp9C8=_tmp5CD;Cyc_Absyn_seq_stmt(_tmp9C8,
Cyc_Absyn_exp_stmt(({void*_tmp5AC=0U;({struct Cyc_Absyn_Exp*_tmp9C7=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmp9C7,_tag_dyneither(_tmp5AC,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3643
struct Cyc_Absyn_Stmt*_tmp5A4=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->tag=10U,_tmp5AB->f1=_tmp5AF,_tmp5AB->f2=_tmp5CC,_tmp5AB->f3=_tmp5CB;_tmp5AB;}),0U);
# 3645
Cyc_Toc_stmt_to_c(nv,_tmp5A4);{
# 3648
struct Cyc_Absyn_Exp*_tmp5A5=({struct Cyc_Absyn_Exp*_tmp5AA[1U];({struct Cyc_Absyn_Exp*_tmp9C9=
Cyc_Toc_member_exp(_tmp5B1,Cyc_Toc_handler_sp,0U);_tmp5AA[0]=_tmp9C9;});({struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmp9CA,_tag_dyneither(_tmp5AA,sizeof(struct Cyc_Absyn_Exp*),1U));});});
struct Cyc_Absyn_Stmt*_tmp5A6=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5A9[1U];({struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Absyn_address_exp(_tmp5B1,0U);_tmp5A9[0]=_tmp9CB;});({struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmp9CC,_tag_dyneither(_tmp5A9,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
struct Cyc_Absyn_Exp*_tmp5A7=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp5A8=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmp9DC=({
struct _tuple1*_tmp9DB=_tmp5B2;void*_tmp9DA=h_typ;Cyc_Absyn_declare_stmt(_tmp9DB,_tmp9DA,0,({
struct Cyc_Absyn_Stmt*_tmp9D9=_tmp5A6;Cyc_Absyn_seq_stmt(_tmp9D9,({
struct _tuple1*_tmp9D8=_tmp5AE;void*_tmp9D7=was_thrown_typ;struct Cyc_Absyn_Exp*_tmp9D6=_tmp5A7;Cyc_Absyn_declare_stmt(_tmp9D8,_tmp9D7,_tmp9D6,({
struct Cyc_Absyn_Stmt*_tmp9D5=({struct Cyc_Absyn_Exp*_tmp9CE=_tmp5A5;struct Cyc_Absyn_Stmt*_tmp9CD=
Cyc_Absyn_assign_stmt(_tmp5AD,_tmp5A8,0U);
# 3658
Cyc_Absyn_ifthenelse_stmt(_tmp9CE,_tmp9CD,
# 3660
Cyc_Toc_skip_stmt_dl(),0U);});
# 3658
Cyc_Absyn_seq_stmt(_tmp9D5,({
# 3661
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp5AD,0U);struct Cyc_Absyn_Stmt*_tmp9D3=_tmp5A3;Cyc_Absyn_ifthenelse_stmt(_tmp9D4,_tmp9D3,({
# 3663
struct _tuple1*_tmp9D2=_tmp5B0;void*_tmp9D1=e_typ;struct Cyc_Absyn_Exp*_tmp9D0=({
void*_tmp9CF=e_typ;Cyc_Toc_cast_it(_tmp9CF,Cyc_Toc_get_exn_thrown_expression());});
# 3663
Cyc_Absyn_declare_stmt(_tmp9D2,_tmp9D1,_tmp9D0,_tmp5A4,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3654
s->r=_tmp9DC;});};};
# 3667
_npop_handler(0U);return;
# 3637
;_pop_region(temp);};};};};}}_LL0:;}}
# 3676
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmp9DD=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmp9DD;});
({void*_tmp9DE=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmp9DE;});{
struct _RegionHandle _tmp5CE=_new_region("frgn");struct _RegionHandle*frgn=& _tmp5CE;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp5CF=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp5D0=(f->i).args;for(0;_tmp5D0 != 0;_tmp5D0=_tmp5D0->tl){
struct _tuple1*_tmp5D1=({struct _tuple1*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->f1=Cyc_Absyn_Loc_n,_tmp5D2->f2=(*((struct _tuple9*)_tmp5D0->hd)).f1;_tmp5D2;});
({void*_tmp9DF=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp5D0->hd)).f3);(*((struct _tuple9*)_tmp5D0->hd)).f3=_tmp9DF;});}}
# 3693
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3697
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp5D3=_cycalloc(sizeof(*_tmp5D3));({struct Cyc_Hashtable_Table*_tmp9E0=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp5D3=_tmp9E0;});_tmp5D3;});
if((unsigned)(f->i).cyc_varargs &&((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp5D4=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp5D5=_tmp5D4;struct _dyneither_ptr*_tmp5DE;struct Cyc_Absyn_Tqual _tmp5DD;void*_tmp5DC;int _tmp5DB;_LL1: _tmp5DE=_tmp5D5.name;_tmp5DD=_tmp5D5.tq;_tmp5DC=_tmp5D5.type;_tmp5DB=_tmp5D5.inject;_LL2:;{
void*_tmp5D6=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp5DC,Cyc_Absyn_heap_rgn_type,_tmp5DD,Cyc_Absyn_false_type));
struct _tuple1*_tmp5D7=({struct _tuple1*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->f1=Cyc_Absyn_Loc_n,_tmp5DA->f2=(struct _dyneither_ptr*)_check_null(_tmp5DE);_tmp5DA;});
({struct Cyc_List_List*_tmp9E3=({struct Cyc_List_List*_tmp9E2=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9E2,({struct Cyc_List_List*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));({struct _tuple9*_tmp9E1=({struct _tuple9*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->f1=_tmp5DE,_tmp5D8->f2=_tmp5DD,_tmp5D8->f3=_tmp5D6;_tmp5D8;});_tmp5D9->hd=_tmp9E1;}),_tmp5D9->tl=0;_tmp5D9;}));});(f->i).args=_tmp9E3;});
(f->i).cyc_varargs=0;};}
# 3706
{struct Cyc_List_List*_tmp5DF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp5DF != 0;_tmp5DF=_tmp5DF->tl){
({void*_tmp9E4=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp5DF->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp5DF->hd)->type=_tmp9E4;});}}
# 3709
({struct Cyc_Toc_Env*_tmp9E5=Cyc_Toc_clear_toplevel(frgn,_tmp5CF);Cyc_Toc_stmt_to_c(_tmp9E5,f->body);});}
# 3686
;_pop_region(frgn);};}
# 3713
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp5E0=s;switch(_tmp5E0){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3729 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 3731
struct _tuple39 _tmp5E1=*env;struct _tuple39 _tmp5E2=_tmp5E1;struct Cyc_Toc_TocState*_tmp616;struct Cyc_Absyn_Aggrdecl*_tmp615;_LL1: _tmp616=_tmp5E2.f1;_tmp615=*_tmp5E2.f2;_LL2:;{
struct _tuple1*_tmp5E3=_tmp615->name;
struct Cyc_Toc_TocState _tmp5E4=*_tmp616;struct Cyc_Toc_TocState _tmp5E5=_tmp5E4;struct Cyc_Dict_Dict*_tmp614;_LL4: _tmp614=_tmp5E5.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple18**_tmp5E6=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp614,_tmp5E3);
if(_tmp5E6 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if((int)_tmp615->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp5E7=_region_malloc(d,sizeof(*_tmp5E7));_tmp5E7->f1=_tmp615,({void*_tmp9E6=Cyc_Absyn_strctq(_tmp5E3);_tmp5E7->f2=_tmp9E6;});_tmp5E7;});else{
# 3742
v=({struct _tuple18*_tmp5E8=_region_malloc(d,sizeof(*_tmp5E8));_tmp5E8->f1=_tmp615,({void*_tmp9E7=Cyc_Absyn_unionq_type(_tmp5E3);_tmp5E8->f2=_tmp9E7;});_tmp5E8;});}
({struct Cyc_Dict_Dict _tmp9E8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp614,_tmp5E3,v);*_tmp614=_tmp9E8;});};}else{
# 3745
struct _tuple18*_tmp5E9=*_tmp5E6;struct _tuple18*_tmp5EA=_tmp5E9;struct Cyc_Absyn_Aggrdecl*_tmp5ED;void*_tmp5EC;_LL7: _tmp5ED=_tmp5EA->f1;_tmp5EC=_tmp5EA->f2;_LL8:;
if(_tmp5ED->impl == 0){
({struct Cyc_Dict_Dict _tmp9EB=({struct Cyc_Dict_Dict _tmp9EA=*_tmp614;struct _tuple1*_tmp9E9=_tmp5E3;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmp9EA,_tmp9E9,({struct _tuple18*_tmp5EB=_region_malloc(d,sizeof(*_tmp5EB));_tmp5EB->f1=_tmp615,_tmp5EB->f2=_tmp5EC;_tmp5EB;}));});*_tmp614=_tmp9EB;});
seen_defn_before=0;}else{
# 3750
seen_defn_before=1;}}{
# 3752
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->kind=_tmp615->kind,_tmp613->sc=Cyc_Absyn_Public,_tmp613->name=_tmp615->name,_tmp613->tvs=0,_tmp613->impl=0,_tmp613->expected_mem_kind=0,_tmp613->attributes=_tmp615->attributes;_tmp613;});
# 3759
if(_tmp615->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmp9EC=({struct Cyc_Absyn_AggrdeclImpl*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->exist_vars=0,_tmp5EE->rgn_po=0,_tmp5EE->fields=0,_tmp5EE->tagged=0;_tmp5EE;});new_ad->impl=_tmp9EC;});{
# 3764
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp5EF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp615->impl))->fields;for(0;_tmp5EF != 0;_tmp5EF=_tmp5EF->tl){
# 3768
struct Cyc_Absyn_Aggrfield*_tmp5F0=(struct Cyc_Absyn_Aggrfield*)_tmp5EF->hd;
void*_tmp5F1=_tmp5F0->type;
struct Cyc_List_List*_tmp5F2=_tmp5F0->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp5F1))&&(
(int)_tmp615->kind == (int)Cyc_Absyn_StructA && _tmp5EF->tl == 0 ||(int)_tmp615->kind == (int)Cyc_Absyn_UnionA)){
# 3782 "toc.cyc"
void*_tmp5F3=Cyc_Tcutil_compress(_tmp5F1);void*_tmp5F4=_tmp5F3;void*_tmp5FC;struct Cyc_Absyn_Tqual _tmp5FB;void*_tmp5FA;unsigned _tmp5F9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->tag == 4U){_LLA: _tmp5FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).elt_type;_tmp5FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).tq;_tmp5FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).zero_term;_tmp5F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).zt_loc;_LLB:
# 3785
 _tmp5F1=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->tag=4U,(_tmp5F5->f1).elt_type=_tmp5FC,(_tmp5F5->f1).tq=_tmp5FB,({struct Cyc_Absyn_Exp*_tmp9ED=Cyc_Absyn_uint_exp(0U,0U);(_tmp5F5->f1).num_elts=_tmp9ED;}),(_tmp5F5->f1).zero_term=_tmp5FA,(_tmp5F5->f1).zt_loc=_tmp5F9;_tmp5F5;});
goto _LL9;}else{_LLC: _LLD:
# 3788
 _tmp5F2=({struct Cyc_List_List*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));({void*_tmp9EE=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->tag=6U,_tmp5F6->f1=0;_tmp5F6;});_tmp5F7->hd=_tmp9EE;}),_tmp5F7->tl=_tmp5F2;_tmp5F7;});
_tmp5F1=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->tag=4U,({void*_tmp9F1=Cyc_Toc_void_star_type();(_tmp5F8->f1).elt_type=_tmp9F1;}),({
struct Cyc_Absyn_Tqual _tmp9F0=Cyc_Absyn_empty_tqual(0U);(_tmp5F8->f1).tq=_tmp9F0;}),({
struct Cyc_Absyn_Exp*_tmp9EF=Cyc_Absyn_uint_exp(0U,0U);(_tmp5F8->f1).num_elts=_tmp9EF;}),(_tmp5F8->f1).zero_term=Cyc_Absyn_false_type,(_tmp5F8->f1).zt_loc=0U;_tmp5F8;});}_LL9:;}{
# 3795
struct Cyc_Absyn_Aggrfield*_tmp5FD=({struct Cyc_Absyn_Aggrfield*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->name=_tmp5F0->name,_tmp60F->tq=Cyc_Toc_mt_tq,({
# 3797
void*_tmp9F2=Cyc_Toc_typ_to_c(_tmp5F1);_tmp60F->type=_tmp9F2;}),_tmp60F->width=_tmp5F0->width,_tmp60F->attributes=_tmp5F2,_tmp60F->requires_clause=0;_tmp60F;});
# 3805
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp615->impl))->tagged){
void*_tmp5FE=_tmp5FD->type;
struct _dyneither_ptr*_tmp5FF=_tmp5FD->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp60C=({struct Cyc_String_pa_PrintArg_struct _tmp732;_tmp732.tag=0U,_tmp732.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp615->name).f2);_tmp732;});struct Cyc_String_pa_PrintArg_struct _tmp60D=({struct Cyc_String_pa_PrintArg_struct _tmp731;_tmp731.tag=0U,_tmp731.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5FF);_tmp731;});void*_tmp60A[2U];_tmp60A[0]=& _tmp60C,_tmp60A[1]=& _tmp60D;({struct _dyneither_ptr _tmp9F3=({const char*_tmp60B="_union_%s_%s";_tag_dyneither(_tmp60B,sizeof(char),13U);});Cyc_aprintf(_tmp9F3,_tag_dyneither(_tmp60A,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp609=_cycalloc(sizeof(*_tmp609));*_tmp609=s;_tmp609;});
struct Cyc_Absyn_Aggrfield*_tmp600=({struct Cyc_Absyn_Aggrfield*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->name=Cyc_Toc_val_sp,_tmp608->tq=Cyc_Toc_mt_tq,_tmp608->type=_tmp5FE,_tmp608->width=0,_tmp608->attributes=0,_tmp608->requires_clause=0;_tmp608;});
struct Cyc_Absyn_Aggrfield*_tmp601=({struct Cyc_Absyn_Aggrfield*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->name=Cyc_Toc_tag_sp,_tmp607->tq=Cyc_Toc_mt_tq,_tmp607->type=Cyc_Absyn_sint_type,_tmp607->width=0,_tmp607->attributes=0,_tmp607->requires_clause=0;_tmp607;});
struct Cyc_List_List*_tmp602=({struct Cyc_Absyn_Aggrfield*_tmp606[2U];_tmp606[0]=_tmp601,_tmp606[1]=_tmp600;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp606,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp603=Cyc_Toc_make_c_struct_defn(str,_tmp602);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp605=_cycalloc(sizeof(*_tmp605));({struct Cyc_Absyn_Decl*_tmp9F4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->tag=5U,_tmp604->f1=_tmp603;_tmp604;}),0U);_tmp605->hd=_tmp9F4;}),_tmp605->tl=Cyc_Toc_result_decls;_tmp605;});
({void*_tmp9F5=Cyc_Absyn_strct(str);_tmp5FD->type=_tmp9F5;});}
# 3817
new_fields=({struct Cyc_List_List*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->hd=_tmp5FD,_tmp60E->tl=new_fields;_tmp60E;});};}}
# 3819
({struct Cyc_List_List*_tmp9F6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmp9F6;});};}
# 3822
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp612=_cycalloc(sizeof(*_tmp612));({struct Cyc_Absyn_Decl*_tmp9F8=({struct Cyc_Absyn_Decl*_tmp611=_cycalloc(sizeof(*_tmp611));({void*_tmp9F7=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=5U,_tmp610->f1=new_ad;_tmp610;});_tmp611->r=_tmp9F7;}),_tmp611->loc=0U;_tmp611;});_tmp612->hd=_tmp9F8;}),_tmp612->tl=Cyc_Toc_result_decls;_tmp612;});
return 0;};};};}
# 3826
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple40{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3854 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3857
struct _tuple40 _tmp617=*env;struct _tuple40 _tmp618=_tmp617;struct Cyc_Set_Set**_tmp629;struct Cyc_Absyn_Datatypedecl*_tmp628;_LL1: _tmp629=(_tmp618.f1)->datatypes_so_far;_tmp628=_tmp618.f2;_LL2:;{
struct _tuple1*_tmp619=_tmp628->name;
if(_tmp628->fields == 0 ||((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp629,_tmp619))
return 0;
({struct Cyc_Set_Set*_tmp9F9=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp629,_tmp619);*_tmp629=_tmp9F9;});
{struct Cyc_List_List*_tmp61A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp628->fields))->v;for(0;_tmp61A != 0;_tmp61A=_tmp61A->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp61A->hd;
# 3865
struct Cyc_List_List*_tmp61B=0;
int i=1;
{struct Cyc_List_List*_tmp61C=f->typs;for(0;_tmp61C != 0;(_tmp61C=_tmp61C->tl,i ++)){
struct _dyneither_ptr*_tmp61D=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp61E=({struct Cyc_Absyn_Aggrfield*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->name=_tmp61D,_tmp620->tq=(*((struct _tuple12*)_tmp61C->hd)).f1,({
void*_tmp9FA=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp61C->hd)).f2);_tmp620->type=_tmp9FA;}),_tmp620->width=0,_tmp620->attributes=0,_tmp620->requires_clause=0;_tmp620;});
_tmp61B=({struct Cyc_List_List*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->hd=_tmp61E,_tmp61F->tl=_tmp61B;_tmp61F;});}}
# 3873
_tmp61B=({struct Cyc_List_List*_tmp622=_cycalloc(sizeof(*_tmp622));({struct Cyc_Absyn_Aggrfield*_tmp9FC=({struct Cyc_Absyn_Aggrfield*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->name=Cyc_Toc_tag_sp,_tmp621->tq=Cyc_Toc_mt_tq,_tmp621->type=Cyc_Absyn_sint_type,_tmp621->width=0,_tmp621->attributes=0,_tmp621->requires_clause=0;_tmp621;});_tmp622->hd=_tmp9FC;}),({
struct Cyc_List_List*_tmp9FB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp61B);_tmp622->tl=_tmp9FB;});_tmp622;});{
struct Cyc_Absyn_Aggrdecl*_tmp623=({struct _dyneither_ptr*_tmp9FE=({struct _dyneither_ptr*_tmp627=_cycalloc(sizeof(*_tmp627));({struct _dyneither_ptr _tmp9FD=({const char*_tmp626="";_tag_dyneither(_tmp626,sizeof(char),1U);});*_tmp627=_tmp9FD;});_tmp627;});Cyc_Toc_make_c_struct_defn(_tmp9FE,_tmp61B);});
({struct _tuple1*_tmp9FF=Cyc_Toc_collapse_qvars(f->name,_tmp628->name);_tmp623->name=_tmp9FF;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp625=_cycalloc(sizeof(*_tmp625));({struct Cyc_Absyn_Decl*_tmpA00=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->tag=5U,_tmp624->f1=_tmp623;_tmp624;}),0U);_tmp625->hd=_tmpA00;}),_tmp625->tl=Cyc_Toc_result_decls;_tmp625;});};}}
# 3879
return 0;};}
# 3882
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3901 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3904
struct _tuple40 _tmp62A=*env;struct _tuple40 _tmp62B=_tmp62A;struct Cyc_Toc_TocState*_tmp64D;struct Cyc_Absyn_Datatypedecl*_tmp64C;_LL1: _tmp64D=_tmp62B.f1;_tmp64C=_tmp62B.f2;_LL2:;
if(_tmp64C->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp62C=*_tmp64D;struct Cyc_Toc_TocState _tmp62D=_tmp62C;struct Cyc_Dict_Dict*_tmp64B;_LL4: _tmp64B=_tmp62D.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp62E=_tmp64C->name;
{struct Cyc_List_List*_tmp62F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp64C->fields))->v;for(0;_tmp62F != 0;_tmp62F=_tmp62F->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp62F->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp630=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp631=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp630,Cyc_Absyn_false_type,0U);
# 3915
int*_tmp632=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp64B,f->name);int*_tmp633=_tmp632;if(_tmp633 == 0){_LL7: _LL8: {
# 3917
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3921
struct Cyc_Absyn_Vardecl*_tmp634=Cyc_Absyn_new_vardecl(0U,f->name,_tmp631,initopt);
_tmp634->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp636=_cycalloc(sizeof(*_tmp636));({struct Cyc_Absyn_Decl*_tmpA01=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->tag=0U,_tmp635->f1=_tmp634;_tmp635;}),0U);_tmp636->hd=_tmpA01;}),_tmp636->tl=Cyc_Toc_result_decls;_tmp636;});
({struct Cyc_Dict_Dict _tmpA02=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp64B,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3924
*_tmp64B=_tmpA02;});{
# 3926
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp637=f->typs;for(0;_tmp637 != 0;(_tmp637=_tmp637->tl,i ++)){
struct _dyneither_ptr*_tmp638=({struct _dyneither_ptr*_tmp63F=_cycalloc(sizeof(*_tmp63F));({struct _dyneither_ptr _tmpA04=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp63E=({struct Cyc_Int_pa_PrintArg_struct _tmp733;_tmp733.tag=1U,_tmp733.f1=(unsigned long)i;_tmp733;});void*_tmp63C[1U];_tmp63C[0]=& _tmp63E;({struct _dyneither_ptr _tmpA03=({const char*_tmp63D="f%d";_tag_dyneither(_tmp63D,sizeof(char),4U);});Cyc_aprintf(_tmpA03,_tag_dyneither(_tmp63C,sizeof(void*),1U));});});*_tmp63F=_tmpA04;});_tmp63F;});
struct Cyc_Absyn_Aggrfield*_tmp639=({struct Cyc_Absyn_Aggrfield*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->name=_tmp638,_tmp63B->tq=(*((struct _tuple12*)_tmp637->hd)).f1,({
void*_tmpA05=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp637->hd)).f2);_tmp63B->type=_tmpA05;}),_tmp63B->width=0,_tmp63B->attributes=0,_tmp63B->requires_clause=0;_tmp63B;});
fields=({struct Cyc_List_List*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->hd=_tmp639,_tmp63A->tl=fields;_tmp63A;});}}
# 3934
fields=({struct Cyc_List_List*_tmp641=_cycalloc(sizeof(*_tmp641));({struct Cyc_Absyn_Aggrfield*_tmpA08=({struct Cyc_Absyn_Aggrfield*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->name=Cyc_Toc_tag_sp,_tmp640->tq=Cyc_Toc_mt_tq,({
void*_tmpA07=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp640->type=_tmpA07;}),_tmp640->width=0,_tmp640->attributes=0,_tmp640->requires_clause=0;_tmp640;});
# 3934
_tmp641->hd=_tmpA08;}),({
# 3936
struct Cyc_List_List*_tmpA06=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp641->tl=_tmpA06;});_tmp641;});{
struct Cyc_Absyn_Aggrdecl*_tmp642=({struct _dyneither_ptr*_tmpA0A=({struct _dyneither_ptr*_tmp646=_cycalloc(sizeof(*_tmp646));({struct _dyneither_ptr _tmpA09=({const char*_tmp645="";_tag_dyneither(_tmp645,sizeof(char),1U);});*_tmp646=_tmpA09;});_tmp646;});Cyc_Toc_make_c_struct_defn(_tmpA0A,fields);});
({struct _tuple1*_tmpA0B=Cyc_Toc_collapse_qvars(f->name,_tmp64C->name);_tmp642->name=_tmpA0B;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp644=_cycalloc(sizeof(*_tmp644));({struct Cyc_Absyn_Decl*_tmpA0C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->tag=5U,_tmp643->f1=_tmp642;_tmp643;}),0U);_tmp644->hd=_tmpA0C;}),_tmp644->tl=Cyc_Toc_result_decls;_tmp644;});
goto _LL6;};};};}}else{if(*((int*)_tmp633)== 0){_LL9: _LLA:
# 3942
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp647=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp648=Cyc_Absyn_new_vardecl(0U,f->name,_tmp631,_tmp647);
_tmp648->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct Cyc_Absyn_Decl*_tmpA0D=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->tag=0U,_tmp649->f1=_tmp648;_tmp649;}),0U);_tmp64A->hd=_tmpA0D;}),_tmp64A->tl=Cyc_Toc_result_decls;_tmp64A;});
({struct Cyc_Dict_Dict _tmpA0E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp64B,f->name,1);*_tmp64B=_tmpA0E;});}
# 3949
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3953
return 0;};};}
# 3956
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3960
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3966
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpA0F=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpA0F;});
# 3970
if((int)vd->sc == (int)Cyc_Absyn_Register && Cyc_Tcutil_is_tagged_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 3979
struct _RegionHandle _tmp64E=_new_region("temp");struct _RegionHandle*temp=& _tmp64E;_push_region(temp);
{struct Cyc_Toc_Env*_tmp64F=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp64F,init);}
# 3980
;_pop_region(temp);}else{
# 3984
Cyc_Toc_exp_to_c(nv,init);}}else{
# 3987
void*_tmp650=Cyc_Tcutil_compress(old_typ);void*_tmp651=_tmp650;void*_tmp658;struct Cyc_Absyn_Exp*_tmp657;void*_tmp656;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp651)->tag == 4U){_LL1: _tmp658=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp651)->f1).elt_type;_tmp657=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp651)->f1).num_elts;_tmp656=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp651)->f1).zero_term;_LL2:
# 3989
 if(Cyc_Tcutil_force_type2bool(0,_tmp656)){
if(_tmp657 == 0)
({void*_tmp652=0U;({struct _dyneither_ptr _tmpA10=({const char*_tmp653="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp653,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA10,_tag_dyneither(_tmp652,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp657;
struct Cyc_Absyn_Exp*_tmp654=({struct Cyc_Absyn_Exp*_tmpA12=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpA12,({
struct Cyc_Absyn_Exp*_tmpA11=num_elts;Cyc_Absyn_add_exp(_tmpA11,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 3996
struct Cyc_Absyn_Exp*_tmp655=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpA14=({struct Cyc_Absyn_Stmt*_tmpA13=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp654,_tmp655,0U),0U);Cyc_Toc_seq_stmt_r(_tmpA13,
Cyc_Absyn_new_stmt(s->r,0U));});
# 3997
s->r=_tmpA14;});};}
# 4000
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4008
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp659=d;struct Cyc_List_List*_tmp660;struct Cyc_List_List*_tmp65F;void**_tmp65E;struct Cyc_Tcpat_Rhs*_tmp65D;switch(*((int*)_tmp659)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp65D=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp659)->f1;_LL4:
 _tmp65D->rhs=success;return d;default: _LL5: _tmp660=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp659)->f1;_tmp65F=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp659)->f2;_tmp65E=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp659)->f3;_LL6:
# 4013
({void*_tmpA15=Cyc_Toc_rewrite_decision(*_tmp65E,success);*_tmp65E=_tmpA15;});
for(0;_tmp65F != 0;_tmp65F=_tmp65F->tl){
struct _tuple32*_tmp65A=(struct _tuple32*)_tmp65F->hd;struct _tuple32*_tmp65B=_tmp65A;void**_tmp65C;_LL8: _tmp65C=(void**)& _tmp65B->f2;_LL9:;
({void*_tmpA16=Cyc_Toc_rewrite_decision(*_tmp65C,success);*_tmp65C=_tmpA16;});}
# 4018
return d;}_LL0:;}
# 4029 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4031
struct _RegionHandle _tmp661=_new_region("rgn");struct _RegionHandle*rgn=& _tmp661;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp662=Cyc_Toc_share_env(rgn,nv);
void*_tmp663=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp662,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4041
struct Cyc_Absyn_Stmt*_tmp664=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4045
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp664);{
# 4047
struct Cyc_Absyn_Switch_clause*_tmp665=({struct Cyc_Absyn_Switch_clause*_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->pattern=p,_tmp671->pat_vars=0,_tmp671->where_clause=0,_tmp671->body=_tmp664,_tmp671->loc=0U;_tmp671;});
struct Cyc_List_List*_tmp666=({struct _RegionHandle*_tmpA19=rgn;struct _RegionHandle*_tmpA18=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpA19,Cyc_Toc_gen_labels,_tmpA18,({struct Cyc_Absyn_Switch_clause*_tmp670[1U];_tmp670[0]=_tmp665;({struct _RegionHandle*_tmpA17=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpA17,_tag_dyneither(_tmp670,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4050
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4054
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp662,& mydecls,& mybodies,dopt,_tmp666,v);
# 4057
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp667=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp668=_tmp667;struct Cyc_Toc_Env*_tmp66A;struct Cyc_Absyn_Stmt*_tmp669;_LL1: _tmp66A=_tmp668->f1;_tmp669=_tmp668->f3;_LL2:;
if(_tmp669 == _tmp664){senv=_tmp66A;goto FOUND_ENV;}}
# 4062
({void*_tmp66B=0U;({struct _dyneither_ptr _tmpA1A=({const char*_tmp66C="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp66C,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA1A,_tag_dyneither(_tmp66B,sizeof(void*),0U));});});
FOUND_ENV:
# 4066
 Cyc_Toc_stmt_to_c(senv,s);{
# 4068
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4070
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp66D=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpA1B=Cyc_Toc_temp_var();_tmp66D->name=_tmpA1B;});
({void*_tmpA1C=Cyc_Toc_typ_to_c_array(_tmp66D->type);_tmp66D->type=_tmpA1C;});
res=({struct Cyc_Absyn_Decl*_tmpA1D=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->tag=0U,_tmp66E->f1=_tmp66D;_tmp66E;}),0U);Cyc_Absyn_decl_stmt(_tmpA1D,res,0U);});}
# 4077
res=({struct _tuple1*_tmpA20=v;void*_tmpA1F=Cyc_Toc_typ_to_c(_tmp663);struct Cyc_Absyn_Exp*_tmpA1E=e;Cyc_Absyn_declare_stmt(_tmpA20,_tmpA1F,_tmpA1E,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp66F=res;_npop_handler(0U);return _tmp66F;};};};};}
# 4032
;_pop_region(rgn);}
# 4085
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp672=e->r;void*_tmp673=_tmp672;struct Cyc_Absyn_MallocInfo*_tmp69E;struct Cyc_Absyn_Stmt*_tmp69D;void**_tmp69C;void**_tmp69B;struct Cyc_List_List*_tmp69A;struct Cyc_List_List*_tmp699;void**_tmp698;struct Cyc_List_List*_tmp697;void**_tmp696;struct Cyc_Absyn_Exp*_tmp695;struct Cyc_Absyn_Exp*_tmp694;struct Cyc_List_List*_tmp693;struct Cyc_Absyn_Exp*_tmp692;struct Cyc_Absyn_Exp*_tmp691;struct Cyc_Absyn_Exp*_tmp690;struct Cyc_Absyn_Exp*_tmp68F;struct Cyc_Absyn_Exp*_tmp68E;struct Cyc_Absyn_Exp*_tmp68D;struct Cyc_Absyn_Exp*_tmp68C;struct Cyc_Absyn_Exp*_tmp68B;struct Cyc_Absyn_Exp*_tmp68A;struct Cyc_Absyn_Exp*_tmp689;struct Cyc_Absyn_Exp*_tmp688;struct Cyc_Absyn_Exp*_tmp687;struct Cyc_Absyn_Exp*_tmp686;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_Absyn_Exp*_tmp684;struct Cyc_List_List*_tmp683;struct Cyc_Absyn_Exp*_tmp682;struct Cyc_Absyn_Exp*_tmp681;struct Cyc_Absyn_Exp*_tmp680;struct Cyc_Absyn_Exp*_tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;struct Cyc_Absyn_Exp*_tmp67A;switch(*((int*)_tmp673)){case 41U: _LL1: _tmp67A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL2:
 _tmp67B=_tmp67A;goto _LL4;case 20U: _LL3: _tmp67B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL4:
 _tmp67C=_tmp67B;goto _LL6;case 21U: _LL5: _tmp67C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL6:
 _tmp67D=_tmp67C;goto _LL8;case 22U: _LL7: _tmp67D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL8:
 _tmp67E=_tmp67D;goto _LLA;case 15U: _LL9: _tmp67E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LLA:
 _tmp67F=_tmp67E;goto _LLC;case 11U: _LLB: _tmp67F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LLC:
 _tmp680=_tmp67F;goto _LLE;case 12U: _LLD: _tmp680=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LLE:
 _tmp681=_tmp680;goto _LL10;case 18U: _LLF: _tmp681=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL10:
 _tmp682=_tmp681;goto _LL12;case 5U: _LL11: _tmp682=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp682);goto _LL0;case 3U: _LL13: _tmp683=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp683);goto _LL0;case 7U: _LL15: _tmp685=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp684=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL16:
 _tmp687=_tmp685;_tmp686=_tmp684;goto _LL18;case 8U: _LL17: _tmp687=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp686=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL18:
 _tmp689=_tmp687;_tmp688=_tmp686;goto _LL1A;case 9U: _LL19: _tmp689=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp688=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL1A:
 _tmp68B=_tmp689;_tmp68A=_tmp688;goto _LL1C;case 23U: _LL1B: _tmp68B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp68A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL1C:
 _tmp68D=_tmp68B;_tmp68C=_tmp68A;goto _LL1E;case 35U: _LL1D: _tmp68D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp68C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL1E:
 _tmp68F=_tmp68D;_tmp68E=_tmp68C;goto _LL20;case 4U: _LL1F: _tmp68F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp68E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp673)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp68F);Cyc_Toc_exptypes_to_c(_tmp68E);goto _LL0;case 6U: _LL21: _tmp692=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp691=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_tmp690=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp673)->f3;_LL22:
# 4104
 Cyc_Toc_exptypes_to_c(_tmp692);Cyc_Toc_exptypes_to_c(_tmp691);Cyc_Toc_exptypes_to_c(_tmp690);goto _LL0;case 10U: _LL23: _tmp694=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp693=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL24:
# 4106
 Cyc_Toc_exptypes_to_c(_tmp694);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp693);goto _LL0;case 14U: _LL25: _tmp696=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp695=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL26:
({void*_tmpA21=Cyc_Toc_typ_to_c(*_tmp696);*_tmp696=_tmpA21;});Cyc_Toc_exptypes_to_c(_tmp695);goto _LL0;case 25U: _LL27: _tmp698=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp673)->f1)->f3;_tmp697=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL28:
# 4109
({void*_tmpA22=Cyc_Toc_typ_to_c(*_tmp698);*_tmp698=_tmpA22;});
_tmp699=_tmp697;goto _LL2A;case 36U: _LL29: _tmp699=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL2A:
 _tmp69A=_tmp699;goto _LL2C;case 26U: _LL2B: _tmp69A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL2C:
# 4113
 for(0;_tmp69A != 0;_tmp69A=_tmp69A->tl){
struct _tuple19 _tmp674=*((struct _tuple19*)_tmp69A->hd);struct _tuple19 _tmp675=_tmp674;struct Cyc_Absyn_Exp*_tmp676;_LL56: _tmp676=_tmp675.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp676);}
# 4117
goto _LL0;case 19U: _LL2D: _tmp69B=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL2E:
 _tmp69C=_tmp69B;goto _LL30;case 17U: _LL2F: _tmp69C=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL30:
({void*_tmpA23=Cyc_Toc_typ_to_c(*_tmp69C);*_tmp69C=_tmpA23;});goto _LL0;case 37U: _LL31: _tmp69D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp69D);goto _LL0;case 34U: _LL33: _tmp69E=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL34:
# 4122
 if(_tmp69E->elt_type != 0)
({void**_tmpA25=({void**_tmp677=_cycalloc(sizeof(*_tmp677));({void*_tmpA24=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp69E->elt_type)));*_tmp677=_tmpA24;});_tmp677;});_tmp69E->elt_type=_tmpA25;});
Cyc_Toc_exptypes_to_c(_tmp69E->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4132
 goto _LL42;case 30U: _LL41: _LL42:
 goto _LL44;case 31U: _LL43: _LL44:
 goto _LL46;case 29U: _LL45: _LL46:
 goto _LL48;case 27U: _LL47: _LL48:
 goto _LL4A;case 28U: _LL49: _LL4A:
 goto _LL4C;case 24U: _LL4B: _LL4C:
 goto _LL4E;case 13U: _LL4D: _LL4E:
 goto _LL50;case 16U: _LL4F: _LL50:
 goto _LL52;case 39U: _LL51: _LL52:
 goto _LL54;default: _LL53: _LL54:
# 4143
({void*_tmp678=0U;({unsigned _tmpA27=e->loc;struct _dyneither_ptr _tmpA26=({const char*_tmp679="Cyclone expression within C code";_tag_dyneither(_tmp679,sizeof(char),33U);});((int(*)(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA27,_tmpA26,_tag_dyneither(_tmp678,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4147
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp69F=d->r;void*_tmp6A0=_tmp69F;struct Cyc_Absyn_Typedefdecl*_tmp6AA;struct Cyc_Absyn_Enumdecl*_tmp6A9;struct Cyc_Absyn_Aggrdecl*_tmp6A8;struct Cyc_Absyn_Fndecl*_tmp6A7;struct Cyc_Absyn_Vardecl*_tmp6A6;switch(*((int*)_tmp6A0)){case 0U: _LL1: _tmp6A6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LL2:
# 4150
({void*_tmpA28=Cyc_Toc_typ_to_c(_tmp6A6->type);_tmp6A6->type=_tmpA28;});
if(_tmp6A6->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6A6->initializer));
goto _LL0;case 1U: _LL3: _tmp6A7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LL4:
# 4154
({void*_tmpA29=Cyc_Toc_typ_to_c((_tmp6A7->i).ret_type);(_tmp6A7->i).ret_type=_tmpA29;});
{struct Cyc_List_List*_tmp6A1=(_tmp6A7->i).args;for(0;_tmp6A1 != 0;_tmp6A1=_tmp6A1->tl){
({void*_tmpA2A=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp6A1->hd)).f3);(*((struct _tuple9*)_tmp6A1->hd)).f3=_tmpA2A;});}}
# 4158
goto _LL0;case 5U: _LL5: _tmp6A8=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp6A8);goto _LL0;case 7U: _LL7: _tmp6A9=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LL8:
# 4161
 if(_tmp6A9->fields != 0){
struct Cyc_List_List*_tmp6A2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6A9->fields))->v;for(0;_tmp6A2 != 0;_tmp6A2=_tmp6A2->tl){
struct Cyc_Absyn_Enumfield*_tmp6A3=(struct Cyc_Absyn_Enumfield*)_tmp6A2->hd;
if(_tmp6A3->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6A3->tag));}}
# 4166
goto _LL0;case 8U: _LL9: _tmp6AA=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LLA:
({void*_tmpA2B=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6AA->defn));_tmp6AA->defn=_tmpA2B;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4176
({void*_tmp6A4=0U;({unsigned _tmpA2D=d->loc;struct _dyneither_ptr _tmpA2C=({const char*_tmp6A5="Cyclone declaration within C code";_tag_dyneither(_tmp6A5,sizeof(char),34U);});((int(*)(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA2D,_tmpA2C,_tag_dyneither(_tmp6A4,sizeof(void*),0U));});});goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 4181
 goto _LL0;}_LL0:;}
# 4185
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp6AB=s->r;void*_tmp6AC=_tmp6AB;struct Cyc_Absyn_Stmt*_tmp6C4;struct Cyc_Absyn_Stmt*_tmp6C3;struct Cyc_Absyn_Exp*_tmp6C2;struct Cyc_Absyn_Decl*_tmp6C1;struct Cyc_Absyn_Stmt*_tmp6C0;struct Cyc_Absyn_Exp*_tmp6BF;struct Cyc_List_List*_tmp6BE;void*_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BB;struct Cyc_Absyn_Exp*_tmp6BA;struct Cyc_Absyn_Stmt*_tmp6B9;struct Cyc_Absyn_Exp*_tmp6B8;struct Cyc_Absyn_Stmt*_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B6;struct Cyc_Absyn_Stmt*_tmp6B5;struct Cyc_Absyn_Stmt*_tmp6B4;struct Cyc_Absyn_Exp*_tmp6B3;struct Cyc_Absyn_Stmt*_tmp6B2;struct Cyc_Absyn_Stmt*_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B0;switch(*((int*)_tmp6AC)){case 1U: _LL1: _tmp6B0=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp6B0);goto _LL0;case 2U: _LL3: _tmp6B2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6B1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp6B2);Cyc_Toc_stmttypes_to_c(_tmp6B1);goto _LL0;case 3U: _LL5: _tmp6B3=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_LL6:
 if(_tmp6B3 != 0)Cyc_Toc_exptypes_to_c(_tmp6B3);goto _LL0;case 4U: _LL7: _tmp6B6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6B5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_tmp6B4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f3;_LL8:
# 4191
 Cyc_Toc_exptypes_to_c(_tmp6B6);Cyc_Toc_stmttypes_to_c(_tmp6B5);Cyc_Toc_stmttypes_to_c(_tmp6B4);goto _LL0;case 5U: _LL9: _tmp6B8=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1).f1;_tmp6B7=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_LLA:
# 4193
 Cyc_Toc_exptypes_to_c(_tmp6B8);Cyc_Toc_stmttypes_to_c(_tmp6B7);goto _LL0;case 9U: _LLB: _tmp6BC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6BB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2).f1;_tmp6BA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f3).f1;_tmp6B9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f4;_LLC:
# 4195
 Cyc_Toc_exptypes_to_c(_tmp6BC);Cyc_Toc_exptypes_to_c(_tmp6BB);Cyc_Toc_exptypes_to_c(_tmp6BA);
Cyc_Toc_stmttypes_to_c(_tmp6B9);goto _LL0;case 10U: _LLD: _tmp6BF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6BE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_tmp6BD=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f3;_LLE:
# 4198
 Cyc_Toc_exptypes_to_c(_tmp6BF);
for(0;_tmp6BE != 0;_tmp6BE=_tmp6BE->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp6BE->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp6C1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6C0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp6C1);Cyc_Toc_stmttypes_to_c(_tmp6C0);goto _LL0;case 14U: _LL11: _tmp6C3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6C2=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp6C3);Cyc_Toc_exptypes_to_c(_tmp6C2);goto _LL0;case 13U: _LL13: _tmp6C4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp6C4);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4208
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4211
({void*_tmpA2E=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->tag=0U;_tmp6AD;});s->r=_tmpA2E;});
goto _LL0;default: _LL1F: _LL20:
# 4214
({void*_tmp6AE=0U;({unsigned _tmpA30=s->loc;struct _dyneither_ptr _tmpA2F=({const char*_tmp6AF="Cyclone statement in C code";_tag_dyneither(_tmp6AF,sizeof(char),28U);});((int(*)(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA30,_tmpA2F,_tag_dyneither(_tmp6AE,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4218
static void*Cyc_Toc_decl_to_enumtype(struct Cyc_Absyn_Decl*d){
void*_tmp6C5=d->r;void*_tmp6C6=_tmp6C5;struct Cyc_Absyn_Enumdecl*_tmp6CB;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C6)->tag == 7U){_LL1: _tmp6CB=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C6)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->tag=0U,({void*_tmpA31=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->tag=15U,_tmp6C7->f1=_tmp6CB->name,_tmp6C7->f2=_tmp6CB;_tmp6C7;});_tmp6C8->f1=_tmpA31;}),_tmp6C8->f2=0;_tmp6C8;});}else{_LL3: _LL4:
({void*_tmp6C9=0U;({struct _dyneither_ptr _tmpA32=({const char*_tmp6CA="Toc::get_enumdecl_name";_tag_dyneither(_tmp6CA,sizeof(char),23U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmpA32,_tag_dyneither(_tmp6C9,sizeof(void*),0U));});});}_LL0:;}
# 4225
static struct Cyc_Absyn_Decl*Cyc_Toc_type2enumdecl(void*t){
void*_tmp6CC=t;struct Cyc_Absyn_Enumdecl*_tmp6D0;unsigned _tmp6CF;if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CC)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CC)->f1)->r)->tag == 1U){_LL1: _tmp6D0=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CC)->f1)->r)->f1;_tmp6CF=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CC)->f1)->loc;_LL2:
# 4228
 return({struct Cyc_Absyn_Decl*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));({void*_tmpA33=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->tag=7U,_tmp6CD->f1=_tmp6D0;_tmp6CD;});_tmp6CE->r=_tmpA33;}),_tmp6CE->loc=_tmp6CF;_tmp6CE;});}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 4236
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp6D1=0U;({struct _dyneither_ptr _tmpA34=({const char*_tmp6D2="decls_to_c: not at toplevel!";_tag_dyneither(_tmp6D2,sizeof(char),29U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmpA34,_tag_dyneither(_tmp6D1,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp6D3=d->r;void*_tmp6D4=_tmp6D3;struct Cyc_List_List*_tmp6EF;struct Cyc_List_List*_tmp6EE;struct Cyc_List_List*_tmp6ED;struct Cyc_List_List*_tmp6EC;struct Cyc_Absyn_Typedefdecl*_tmp6EB;struct Cyc_Absyn_Enumdecl*_tmp6EA;struct Cyc_Absyn_Datatypedecl*_tmp6E9;struct Cyc_Absyn_Aggrdecl*_tmp6E8;struct Cyc_Absyn_Fndecl*_tmp6E7;struct Cyc_Absyn_Vardecl*_tmp6E6;switch(*((int*)_tmp6D4)){case 0U: _LL1: _tmp6E6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL2: {
# 4244
struct _tuple1*_tmp6D5=_tmp6E6->name;
# 4246
if((int)_tmp6E6->sc == (int)Cyc_Absyn_ExternC)
_tmp6D5=({struct _tuple1*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));({union Cyc_Absyn_Nmspace _tmpA35=Cyc_Absyn_Abs_n(0,1);_tmp6D6->f1=_tmpA35;}),_tmp6D6->f2=(*_tmp6D5).f2;_tmp6D6;});
if(_tmp6E6->initializer != 0){
if((int)_tmp6E6->sc == (int)Cyc_Absyn_ExternC)_tmp6E6->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6E6->initializer));else{
# 4253
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp6E6->initializer));}}
# 4255
_tmp6E6->name=_tmp6D5;
({enum Cyc_Absyn_Scope _tmpA36=Cyc_Toc_scope_to_c(_tmp6E6->sc);_tmp6E6->sc=_tmpA36;});
({void*_tmpA37=Cyc_Toc_typ_to_c(_tmp6E6->type);_tmp6E6->type=_tmpA37;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7->hd=d,_tmp6D7->tl=Cyc_Toc_result_decls;_tmp6D7;});
goto _LL0;}case 1U: _LL3: _tmp6E7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL4: {
# 4261
struct _tuple1*_tmp6D8=_tmp6E7->name;
# 4263
if((int)_tmp6E7->sc == (int)Cyc_Absyn_ExternC){
_tmp6D8=({struct _tuple1*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));({union Cyc_Absyn_Nmspace _tmpA38=Cyc_Absyn_Abs_n(0,1);_tmp6D9->f1=_tmpA38;}),_tmp6D9->f2=(*_tmp6D8).f2;_tmp6D9;});
_tmp6E7->sc=Cyc_Absyn_Public;}
# 4267
_tmp6E7->name=_tmp6D8;
Cyc_Toc_fndecl_to_c(nv,_tmp6E7,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA->hd=d,_tmp6DA->tl=Cyc_Toc_result_decls;_tmp6DA;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp6DB=0U;({struct _dyneither_ptr _tmpA39=({const char*_tmp6DC="letdecl at toplevel";_tag_dyneither(_tmp6DC,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA39,_tag_dyneither(_tmp6DB,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp6DD=0U;({struct _dyneither_ptr _tmpA3A=({const char*_tmp6DE="region decl at toplevel";_tag_dyneither(_tmp6DE,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA3A,_tag_dyneither(_tmp6DD,sizeof(void*),0U));});});case 5U: _LLB: _tmp6E8=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LLC:
# 4275
 Cyc_Toc_aggrdecl_to_c(_tmp6E8);
goto _LL0;case 6U: _LLD: _tmp6E9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LLE:
# 4278
 if(_tmp6E9->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp6E9);else{
# 4281
Cyc_Toc_datatypedecl_to_c(_tmp6E9);}
goto _LL0;case 7U: _LLF: _tmp6EA=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL10:
# 4284
 Cyc_Toc_enumdecl_to_c(_tmp6EA);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->hd=d,_tmp6DF->tl=Cyc_Toc_result_decls;_tmp6DF;});
goto _LL0;case 8U: _LL11: _tmp6EB=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL12:
# 4288
 _tmp6EB->tvs=0;
if(_tmp6EB->defn != 0){
void*_tmp6E0=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6EB->defn));
_tmp6EB->defn=_tmp6E0;{
# 4294
struct Cyc_Absyn_Decl*_tmp6E1=Cyc_Toc_type2enumdecl(_tmp6E0);
if(_tmp6E1 != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2->hd=_tmp6E1,_tmp6E2->tl=Cyc_Toc_result_decls;_tmp6E2;});
({void*_tmpA3B=Cyc_Toc_decl_to_enumtype(_tmp6E1);_tmp6EB->defn=_tmpA3B;});}};}else{
# 4300
enum Cyc_Absyn_KindQual _tmp6E3=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp6EB->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp6E4=_tmp6E3;if(_tmp6E4 == Cyc_Absyn_BoxKind){_LL24: _LL25:
({void*_tmpA3C=Cyc_Toc_void_star_type();_tmp6EB->defn=_tmpA3C;});goto _LL23;}else{_LL26: _LL27:
 _tmp6EB->defn=Cyc_Absyn_void_type;goto _LL23;}_LL23:;}
# 4308
if(Cyc_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5->hd=d,_tmp6E5->tl=Cyc_Toc_result_decls;_tmp6E5;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
# 4315
 goto _LL0;case 9U: _LL1B: _tmp6EC=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6D4)->f2;_LL1C:
 _tmp6ED=_tmp6EC;goto _LL1E;case 10U: _LL1D: _tmp6ED=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6D4)->f2;_LL1E:
 _tmp6EE=_tmp6ED;goto _LL20;case 11U: _LL1F: _tmp6EE=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL20:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp6EE,top,cinclude);goto _LL0;default: _LL21: _tmp6EF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL22:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp6EF,top,1);goto _LL0;}_LL0:;};}
# 4322
return nv;}
# 4326
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp6F0=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp6F1=_tmp6F0;struct Cyc_Core_DynamicRegion*_tmp6F6;_LL1: _tmp6F6=_tmp6F1.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp6F6->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp6F3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6F3));_tmp6F3->dyn=_tmp6F6,_tmp6F3->state=ts;_tmp6F3;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_dyneither(({unsigned _tmp6F5=34;struct _dyneither_ptr**_tmp6F4=_cycalloc(_check_times(_tmp6F5,sizeof(struct _dyneither_ptr*)));_tmp6F4[0]=& Cyc_Toc__throw_str,_tmp6F4[1]=& Cyc_Toc_setjmp_str,_tmp6F4[2]=& Cyc_Toc__push_handler_str,_tmp6F4[3]=& Cyc_Toc__pop_handler_str,_tmp6F4[4]=& Cyc_Toc__exn_thrown_str,_tmp6F4[5]=& Cyc_Toc__npop_handler_str,_tmp6F4[6]=& Cyc_Toc__check_null_str,_tmp6F4[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp6F4[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp6F4[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp6F4[10]=& Cyc_Toc__tag_dyneither_str,_tmp6F4[11]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp6F4[12]=& Cyc_Toc__get_dyneither_size_str,_tmp6F4[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp6F4[14]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp6F4[15]=& Cyc_Toc__zero_arr_plus_str,_tmp6F4[16]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp6F4[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp6F4[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp6F4[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp6F4[20]=& Cyc_Toc__cycalloc_str,_tmp6F4[21]=& Cyc_Toc__cyccalloc_str,_tmp6F4[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp6F4[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp6F4[24]=& Cyc_Toc__region_malloc_str,_tmp6F4[25]=& Cyc_Toc__region_calloc_str,_tmp6F4[26]=& Cyc_Toc__check_times_str,_tmp6F4[27]=& Cyc_Toc__new_region_str,_tmp6F4[28]=& Cyc_Toc__push_region_str,_tmp6F4[29]=& Cyc_Toc__pop_region_str,_tmp6F4[30]=& Cyc_Toc__throw_arraybounds_str,_tmp6F4[31]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp6F4[32]=& Cyc_Toc__throw_match_str,_tmp6F4[33]=& Cyc_Toc__fast_region_malloc_str;_tmp6F4;}),sizeof(struct _dyneither_ptr*),34U);};}
# 4374
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp6F7=ts;struct Cyc_Toc_TocStateWrap*_tmp6F8=Cyc_Toc_toc_state;ts=_tmp6F8;Cyc_Toc_toc_state=_tmp6F7;});{
struct Cyc_Toc_TocStateWrap _tmp6F9=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp6FA=_tmp6F9;struct Cyc_Core_DynamicRegion*_tmp700;struct Cyc_Toc_TocState*_tmp6FF;_LL1: _tmp700=_tmp6FA.dyn;_tmp6FF=_tmp6FA.state;_LL2:;
# 4379
{struct _RegionHandle*h=& _tmp700->h;
{struct Cyc_Toc_TocState _tmp6FC=*_tmp6FF;struct Cyc_Toc_TocState _tmp6FD=_tmp6FC;struct Cyc_Xarray_Xarray*_tmp6FE;_LL4: _tmp6FE=_tmp6FD.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp6FE);}
# 4380
;}
# 4383
Cyc_Core_free_rckey(_tmp700);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4386
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4392
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4394
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp701=_cycalloc(sizeof(*_tmp701));*_tmp701=pop_tables;_tmp701;});
Cyc_Toc_init();
{struct _RegionHandle _tmp702=_new_region("start");struct _RegionHandle*start=& _tmp702;_push_region(start);
({struct _RegionHandle*_tmpA3E=start;struct Cyc_Toc_Env*_tmpA3D=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpA3E,_tmpA3D,ds,1,0);});;_pop_region(start);}
# 4399
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}

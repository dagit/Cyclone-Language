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
void Cyc_Core_ufree(void*ptr);
# 188
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 237
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*file,const char*func,int lineno);
# 255 "core.h"
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
void Cyc_Warn_vwarn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 28
void Cyc_Warn_warn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 30
void Cyc_Warn_flush_warnings();
# 32
void Cyc_Warn_verr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 39
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void*Cyc_Warn_vimpos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 323
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 705 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 881
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 888
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 897
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
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
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 991
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1066
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1085
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1090
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1094
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1099
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr,struct _dyneither_ptr);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr,struct _dyneither_ptr);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr,struct _dyneither_ptr);
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
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 285
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 331
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 349
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 360
int Cyc_Tcutil_force_type2bool(int desired,void*t);struct _tuple13{unsigned int f1;int f2;};
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
void Cyc_Toc_finish();struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 58 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 73 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 75
({struct _dyneither_ptr _tmp731=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp730=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp730,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp731,ap);});}
# 77
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 79
({struct _dyneither_ptr _tmp733=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp732=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp732,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp733,ap);});}
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
({struct Cyc_List_List**_tmp73F=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp73F;}),({
struct Cyc_List_List**_tmp73E=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp73E;}),({
struct Cyc_Dict_Dict*_tmp73D=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp73C=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp73C;});_tmpC;});_tmp11->aggrs_so_far=_tmp73D;}),({
struct Cyc_List_List**_tmp73B=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp73B;}),({
struct Cyc_Set_Set**_tmp73A=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp739=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp739;});_tmpE;});_tmp11->datatypes_so_far=_tmp73A;}),({
struct Cyc_Dict_Dict*_tmp738=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp737=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp737;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp738;}),({
struct Cyc_Dict_Dict*_tmp736=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp735=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp735;});_tmp10;});_tmp11->qvar_tags=_tmp736;}),({
struct Cyc_Xarray_Xarray*_tmp734=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp734;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 151
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 157
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 160
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp740=({struct Cyc_Toc_TocStateWrap _tmp6FF;_tmp6FF.dyn=_tmp1C,_tmp6FF.state=_tmp1B;_tmp6FF;});*ts=_tmp740;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp1A=({struct _tuple15 _tmp700;_tmp700.f1=_tmp1B,_tmp700.f2=arg;_tmp700;});
res=f(h,& _tmp1A);}
# 168
;}
# 170
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)));
return res;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 174
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 177
struct _tuple17 _tmp1D=*env;struct _tuple17 _tmp1E=_tmp1D;struct Cyc_Toc_TocState*_tmp21;struct _tuple1*_tmp20;void*(*_tmp1F)(struct _tuple1*);_LL1: _tmp21=_tmp1E.f1;_tmp20=(_tmp1E.f2)->f1;_tmp1F=(_tmp1E.f2)->f2;_LL2:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp21->aggrs_so_far,_tmp20);
return v == 0?_tmp1F(_tmp20):(*(*v)).f2;};}
# 182
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp701;_tmp701.f1=q,_tmp701.f2=type_maker;_tmp701;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp22[5U]="curr";
# 195 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp22,_tmp22,_tmp22 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp23[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp23,_tmp23,_tmp23 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp24[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp24,_tmp24,_tmp24 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp25[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp25,_tmp25,_tmp25 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp26[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp26,_tmp26,_tmp26 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp27[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp27,_tmp27,_tmp27 + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
# 215 "toc.cyc"
static struct Cyc_Absyn_Exp*Cyc_Toc__get_exn_thrown_e=0;
# 217
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression(){
# 219
if((unsigned int)Cyc_Toc__get_exn_thrown_e)
return(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc__get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_tmp32=_cycalloc(sizeof(*_tmp32));((_tmp32->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp745=({struct _dyneither_ptr*_tmp2D[1U];({struct _dyneither_ptr*_tmp744=({struct _dyneither_ptr*_tmp2F=_cycalloc(sizeof(*_tmp2F));({struct _dyneither_ptr _tmp743=({const char*_tmp2E="Core";_tag_dyneither(_tmp2E,sizeof(char),5U);});*_tmp2F=_tmp743;});_tmp2F;});_tmp2D[0]=_tmp744;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2D,sizeof(struct _dyneither_ptr*),1U));});((_tmp32->f1).Abs_n).val=_tmp745;}),({struct _dyneither_ptr*_tmp742=({struct _dyneither_ptr*_tmp31=_cycalloc(sizeof(*_tmp31));({struct _dyneither_ptr _tmp741=({const char*_tmp30="get_exn_thrown";_tag_dyneither(_tmp30,sizeof(char),15U);});*_tmp31=_tmp741;});_tmp31;});_tmp32->f2=_tmp742;});_tmp32;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,_tmp2C->f1=qv;_tmp2C;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->topt=0,({void*_tmp746=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=1U,_tmp2A->f1=bnd;_tmp2A;});_tmp2B->r=_tmp746;}),_tmp2B->loc=0U,_tmp2B->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp2B;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=10U,_tmp29->f1=fnname,_tmp29->f2=0,_tmp29->f3=0,_tmp29->f4=0;_tmp29;});
Cyc_Toc__get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->topt=0,_tmp28->r=fncall_re,_tmp28->loc=0U,_tmp28->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp28;});
return(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc__get_exn_thrown_e);};}static char _tmp33[7U]="_throw";
# 229
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
# 297
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 299
void*Cyc_Toc_void_star_type(){
static void*void_star_t_opt=0;
if(void_star_t_opt == 0)
void_star_t_opt=({void*_tmp749=Cyc_Absyn_void_type;void*_tmp748=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp747=
Cyc_Absyn_empty_tqual(0U);
# 302
Cyc_Absyn_star_type(_tmp749,_tmp748,_tmp747,Cyc_Absyn_false_type);});
# 304
return void_star_t_opt;}
# 306
static void*Cyc_Toc_dyneither_ptr_type(){
static void*dpt=0;
if(dpt == 0)
dpt=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0),0);
return dpt;}
# 312
static void*Cyc_Toc_rgn_type(){
static void*r=0;
if(r == 0)
r=({void*_tmp74A=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp74A,Cyc_Toc_mt_tq);});
return r;}
# 319
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 323
static struct _tuple19*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp75=_cycalloc(sizeof(*_tmp75));({struct Cyc_List_List*_tmp74C=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));({void*_tmp74B=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=1U,_tmp73->f1=name;_tmp73;});_tmp74->hd=_tmp74B;}),_tmp74->tl=0;_tmp74;});_tmp75->f1=_tmp74C;}),_tmp75->f2=e;_tmp75;});}
# 327
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _dyneither_ptr args){
return({struct Cyc_Absyn_Exp*_tmp74D=f;Cyc_Absyn_fncall_exp(_tmp74D,((struct Cyc_List_List*(*)(struct _dyneither_ptr arr))Cyc_List_from_array)(args),0U);});}
# 330
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp76=e->r;void*_tmp77=_tmp76;struct Cyc_Absyn_Exp*_tmp78;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp78=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp78);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 338
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=14U,_tmp79->f1=t,_tmp79->f2=e,_tmp79->f3=0,_tmp79->f4=Cyc_Absyn_No_coercion;_tmp79;});}
# 341
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=20U,_tmp7A->f1=e;_tmp7A;});}
# 344
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=23U,_tmp7B->f1=e1,_tmp7B->f2=e2;_tmp7B;});}
# 347
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=37U,_tmp7C->f1=s;_tmp7C;});}
# 350
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=17U,_tmp7D->f1=t;_tmp7D;});}
# 353
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=10U,_tmp7E->f1=e,({struct Cyc_List_List*_tmp74E=((struct Cyc_List_List*(*)(struct _dyneither_ptr arr))Cyc_List_from_array)(es);_tmp7E->f2=_tmp74E;}),_tmp7E->f3=0,_tmp7E->f4=1;_tmp7E;});}
# 356
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=2U,_tmp7F->f1=s1,_tmp7F->f2=s2;_tmp7F;});}
# 359
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=6U,_tmp80->f1=e1,_tmp80->f2=e2,_tmp80->f3=e3;_tmp80;});}
# 362
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=21U,_tmp81->f1=e,_tmp81->f2=n,_tmp81->f3=0,_tmp81->f4=0;_tmp81;});}
# 365
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 368
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=36U,_tmp82->f1=tdopt,_tmp82->f2=ds;_tmp82;});}
# 370
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=8U,_tmp83->f1=v;_tmp83;});}
# 373
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 377
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp84=e->r;void*_tmp85=_tmp84;struct Cyc_Absyn_Exp*_tmp86;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp85)->tag == 20U){_LL1: _tmp86=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp85)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp86,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 396
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 405
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 414
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 423
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 434
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp87=Cyc_Tcutil_compress(t);void*_tmp88=_tmp87;enum Cyc_Absyn_Size_of _tmp8F;switch(*((int*)_tmp88)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)){case 1U: _LL1: _tmp8F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)->f2;_LL2:
# 438
{enum Cyc_Absyn_Size_of _tmp89=_tmp8F;switch(_tmp89){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp8A=0U;({struct _dyneither_ptr _tmp74F=({const char*_tmp8B="impossible IntCon (not char, short or int)";_tag_dyneither(_tmp8B,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp74F,_tag_dyneither(_tmp8A,sizeof(void*),0U));});});}_LLD:;}
# 444
goto _LL0;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}default: goto _LLB;}case 3U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8E=({struct Cyc_String_pa_PrintArg_struct _tmp702;_tmp702.tag=0U,({struct _dyneither_ptr _tmp750=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp702.f1=_tmp750;});_tmp702;});void*_tmp8C[1U];_tmp8C[0]=& _tmp8E;({struct _dyneither_ptr _tmp751=({const char*_tmp8D="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8D,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp751,_tag_dyneither(_tmp8C,sizeof(void*),1U));});});}_LL0:;}
# 451
return function;}
# 453
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp90=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp91=_tmp90;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->tag == 3U){_LL1: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp94);}else{_LL3: _LL4:
({void*_tmp92=0U;({struct _dyneither_ptr _tmp752=({const char*_tmp93="impossible type (not pointer)";_tag_dyneither(_tmp93,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp752,_tag_dyneither(_tmp92,sizeof(void*),0U));});});}_LL0:;}
# 463
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp95=e->r;void*_tmp96=_tmp95;struct Cyc_List_List*_tmpA2;struct Cyc_List_List*_tmpA1;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;long long _tmp9C;int _tmp9B;short _tmp9A;struct _dyneither_ptr _tmp99;char _tmp98;switch(*((int*)_tmp96)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Null_c).tag){case 2U: _LL1: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Char_c).val).f2;_LL2:
 return(int)_tmp98 == (int)'\000';case 3U: _LL3: _tmp99=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Wchar_c).val;_LL4: {
# 467
unsigned long _tmp97=Cyc_strlen((struct _dyneither_ptr)_tmp99);
int i=0;
if(_tmp97 >= (unsigned long)2  && (int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),1))== (int)'0')i=2;else{
if(((int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),1))== (int)'x'  && _tmp97 >= (unsigned long)3) && (int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp97;++ i){
if((int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 477
return 0;}}case 4U: _LL5: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Short_c).val).f2;_LL6:
 return(int)_tmp9A == 0;case 5U: _LL7: _tmp9B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Int_c).val).f2;_LL8:
 return _tmp9B == 0;case 6U: _LL9: _tmp9C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).LongLong_c).val).f2;_LLA:
 return _tmp9C == (long long)0;case 1U: _LLD: _LLE:
# 482
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 481
 goto _LLE;case 14U: _LLF: _tmp9D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96)->f2;_LL10:
# 483
 return Cyc_Toc_is_zero(_tmp9D);case 24U: _LL11: _tmp9E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp96)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9E);case 26U: _LL13: _tmp9F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp96)->f1;_LL14:
 _tmpA0=_tmp9F;goto _LL16;case 29U: _LL15: _tmpA0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp96)->f3;_LL16:
 _tmpA1=_tmpA0;goto _LL18;case 25U: _LL17: _tmpA1=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp96)->f2;_LL18:
 _tmpA2=_tmpA1;goto _LL1A;case 36U: _LL19: _tmpA2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp96)->f2;_LL1A:
# 489
 for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmpA2->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 497
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA3=x;union Cyc_Absyn_Nmspace _tmpB5;struct _dyneither_ptr*_tmpB4;_LL1: _tmpB5=_tmpA3->f1;_tmpB4=_tmpA3->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA4=_tmpB5;struct Cyc_List_List*_tmpB3;struct Cyc_List_List*_tmpB2;struct Cyc_List_List*_tmpB1;switch((_tmpA4.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpB1=0;goto _LL7;case 1U: _LL6: _tmpB1=(_tmpA4.Rel_n).val;_LL7:
 _tmpB2=_tmpB1;goto _LL9;case 2U: _LL8: _tmpB2=(_tmpA4.Abs_n).val;_LL9:
 _tmpB3=_tmpB2;goto _LLB;default: _LLA: _tmpB3=(_tmpA4.C_n).val;_LLB:
# 506
 if(_tmpB3 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA7=({struct Cyc_String_pa_PrintArg_struct _tmp704;_tmp704.tag=0U,_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp704;});struct Cyc_String_pa_PrintArg_struct _tmpA8=({struct Cyc_String_pa_PrintArg_struct _tmp703;_tmp703.tag=0U,_tmp703.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB4);_tmp703;});void*_tmpA5[2U];_tmpA5[0]=& _tmpA7,_tmpA5[1]=& _tmpA8;({struct _dyneither_ptr _tmp753=({const char*_tmpA6="%s_%s_struct";_tag_dyneither(_tmpA6,sizeof(char),13U);});Cyc_aprintf(_tmp753,_tag_dyneither(_tmpA5,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA9=_new_region("r");struct _RegionHandle*r=& _tmpA9;_push_region(r);
{struct _dyneither_ptr _tmpB0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp707;_tmp707.tag=0U,_tmp707.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp707;});struct Cyc_String_pa_PrintArg_struct _tmpAD=({struct Cyc_String_pa_PrintArg_struct _tmp706;_tmp706.tag=0U,({struct _dyneither_ptr _tmp756=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp755=r;struct Cyc_List_List*_tmp754=_tmpB3;Cyc_rstr_sepstr(_tmp755,_tmp754,({const char*_tmpAF="_";_tag_dyneither(_tmpAF,sizeof(char),2U);}));}));_tmp706.f1=_tmp756;});_tmp706;});struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmp705;_tmp705.tag=0U,_tmp705.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB4);_tmp705;});void*_tmpAA[3U];_tmpAA[0]=& _tmpAC,_tmpAA[1]=& _tmpAD,_tmpAA[2]=& _tmpAE;({struct _dyneither_ptr _tmp757=({const char*_tmpAB="%s_%s_%s_struct";_tag_dyneither(_tmpAB,sizeof(char),16U);});Cyc_aprintf(_tmp757,_tag_dyneither(_tmpAA,sizeof(void*),3U));});});_npop_handler(0U);return _tmpB0;};_pop_region(r);};}_LL3:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 519
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 522
struct _tuple20 _tmpB6=*env;struct _tuple20 _tmpB7=_tmpB6;struct Cyc_Dict_Dict*_tmpCA;struct _tuple14*_tmpC9;_LL1: _tmpCA=(_tmpB7.f1)->qvar_tags;_tmpC9=_tmpB7.f2;_LL2:;{
struct _tuple14 _tmpB8=*_tmpC9;struct _tuple14 _tmpB9=_tmpB8;struct _tuple1*_tmpC8;struct _tuple1*_tmpC7;_LL4: _tmpC8=_tmpB9.f1;_tmpC7=_tmpB9.f2;_LL5:;{
struct _handler_cons _tmpBA;_push_handler(& _tmpBA);{int _tmpBC=0;if(setjmp(_tmpBA.handler))_tmpBC=1;if(!_tmpBC){{struct _tuple1*_tmpBD=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmpCA,Cyc_Toc_qvar_tag_cmp,_tmpC9);_npop_handler(0U);return _tmpBD;};_pop_handler();}else{void*_tmpBB=(void*)Cyc_Core_get_exn_thrown();void*_tmpBE=_tmpBB;void*_tmpC6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBE)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 526
struct _tuple14*_tmpBF=({struct _tuple14*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->f1=_tmpC8,_tmpC5->f2=_tmpC7;_tmpC5;});
struct _tuple1*_tmpC0=_tmpC8;union Cyc_Absyn_Nmspace _tmpC4;struct _dyneither_ptr*_tmpC3;_LLC: _tmpC4=_tmpC0->f1;_tmpC3=_tmpC0->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC3,_tmpC7);
struct _tuple1*res=({struct _tuple1*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->f1=_tmpC4,({struct _dyneither_ptr*_tmp758=({struct _dyneither_ptr*_tmpC1=_cycalloc(sizeof(*_tmpC1));*_tmpC1=newvar;_tmpC1;});_tmpC2->f2=_tmp758;});_tmpC2;});
({struct Cyc_Dict_Dict _tmp759=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpCA,_tmpBF,res);*_tmpCA=_tmp759;});
return res;};}}else{_LL9: _tmpC6=_tmpBE;_LLA:(int)_rethrow(_tmpC6);}_LL6:;}};};};}
# 534
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp708;_tmp708.f1=fieldname,_tmp708.f2=dtname;_tmp708;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 541
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->kind=Cyc_Absyn_StructA,_tmpCD->sc=Cyc_Absyn_Public,_tmpCD->tvs=0,_tmpCD->attributes=0,_tmpCD->expected_mem_kind=0,({
# 544
struct _tuple1*_tmp75C=({struct _tuple1*_tmpCB=_cycalloc(sizeof(*_tmpCB));({union Cyc_Absyn_Nmspace _tmp75B=Cyc_Absyn_Rel_n(0);_tmpCB->f1=_tmp75B;}),_tmpCB->f2=name;_tmpCB;});_tmpCD->name=_tmp75C;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp75A=({struct Cyc_Absyn_AggrdeclImpl*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->exist_vars=0,_tmpCC->rgn_po=0,_tmpCC->tagged=0,_tmpCC->fields=fs;_tmpCC;});_tmpCD->impl=_tmp75A;});_tmpCD;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 553
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 556
struct _tuple21 _tmpCE=*env;struct _tuple21 _tmpCF=_tmpCE;struct Cyc_List_List**_tmpE5;struct Cyc_List_List*_tmpE4;_LL1: _tmpE5=(_tmpCF.f1)->tuple_types;_tmpE4=_tmpCF.f2;_LL2:;
# 558
{struct Cyc_List_List*_tmpD0=*_tmpE5;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){
struct _tuple22*_tmpD1=(struct _tuple22*)_tmpD0->hd;struct _tuple22*_tmpD2=_tmpD1;void*_tmpD5;struct Cyc_List_List*_tmpD4;_LL4: _tmpD5=_tmpD2->f1;_tmpD4=_tmpD2->f2;_LL5:;{
struct Cyc_List_List*_tmpD3=_tmpE4;
for(0;_tmpD3 != 0  && _tmpD4 != 0;(_tmpD3=_tmpD3->tl,_tmpD4=_tmpD4->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD3->hd)).f2,(void*)_tmpD4->hd))
break;}
if(_tmpD3 == 0  && _tmpD4 == 0)
return _tmpD5;};}}{
# 569
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE3=_cycalloc(sizeof(*_tmpE3));({struct _dyneither_ptr _tmp75E=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE2=({struct Cyc_Int_pa_PrintArg_struct _tmp709;_tmp709.tag=1U,_tmp709.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp709;});void*_tmpE0[1U];_tmpE0[0]=& _tmpE2;({struct _dyneither_ptr _tmp75D=({const char*_tmpE1="_tuple%d";_tag_dyneither(_tmpE1,sizeof(char),9U);});Cyc_aprintf(_tmp75D,_tag_dyneither(_tmpE0,sizeof(void*),1U));});});*_tmpE3=_tmp75E;});_tmpE3;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE4);
struct Cyc_List_List*_tmpD6=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmpD6=({struct Cyc_List_List*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct Cyc_Absyn_Aggrfield*_tmp760=({struct Cyc_Absyn_Aggrfield*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct _dyneither_ptr*_tmp75F=Cyc_Absyn_fieldname(i);_tmpD7->name=_tmp75F;}),_tmpD7->tq=Cyc_Toc_mt_tq,_tmpD7->type=(void*)ts2->hd,_tmpD7->width=0,_tmpD7->attributes=0,_tmpD7->requires_clause=0;_tmpD7;});_tmpD8->hd=_tmp760;}),_tmpD8->tl=_tmpD6;_tmpD8;});}}
_tmpD6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD6);{
struct Cyc_Absyn_Aggrdecl*_tmpD9=Cyc_Toc_make_c_struct_defn(xname,_tmpD6);
void*_tmpDA=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDF=_cycalloc(sizeof(*_tmpDF));*_tmpDF=_tmpD9;_tmpDF;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));({struct Cyc_Absyn_Decl*_tmp761=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->tag=5U,_tmpDB->f1=_tmpD9;_tmpDB;}),0U);_tmpDC->hd=_tmp761;}),_tmpDC->tl=Cyc_Toc_result_decls;_tmpDC;});
({struct Cyc_List_List*_tmp763=({struct Cyc_List_List*_tmpDE=_region_malloc(d,sizeof(*_tmpDE));({struct _tuple22*_tmp762=({struct _tuple22*_tmpDD=_region_malloc(d,sizeof(*_tmpDD));_tmpDD->f1=_tmpDA,_tmpDD->f2=ts;_tmpDD;});_tmpDE->hd=_tmp762;}),_tmpDE->tl=*_tmpE5;_tmpDE;});*_tmpE5=_tmp763;});
return _tmpDA;};};}
# 582
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 587
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 590
struct _tuple24*_tmpE6=env;struct Cyc_List_List**_tmpFA;enum Cyc_Absyn_AggrKind _tmpF9;struct Cyc_List_List*_tmpF8;_LL1: _tmpFA=(_tmpE6->f1)->anon_aggr_types;_tmpF9=(_tmpE6->f2)->f1;_tmpF8=(_tmpE6->f2)->f2;_LL2:;
# 592
{struct Cyc_List_List*_tmpE7=*_tmpFA;for(0;_tmpE7 != 0;_tmpE7=_tmpE7->tl){
struct _tuple25*_tmpE8=(struct _tuple25*)_tmpE7->hd;struct _tuple25*_tmpE9=_tmpE8;void*_tmpEC;enum Cyc_Absyn_AggrKind _tmpEB;struct Cyc_List_List*_tmpEA;_LL4: _tmpEC=_tmpE9->f1;_tmpEB=_tmpE9->f2;_tmpEA=_tmpE9->f3;_LL5:;
if((int)_tmpF9 != (int)_tmpEB)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpEA,_tmpF8))
return _tmpEC;}}{
# 601
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct _dyneither_ptr _tmp765=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp70A;_tmp70A.tag=1U,_tmp70A.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp70A;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp764=({const char*_tmpF5="_tuple%d";_tag_dyneither(_tmpF5,sizeof(char),9U);});Cyc_aprintf(_tmp764,_tag_dyneither(_tmpF4,sizeof(void*),1U));});});*_tmpF7=_tmp765;});_tmpF7;});
struct Cyc_Absyn_Aggrdecl*_tmpED=Cyc_Toc_make_c_struct_defn(xname,_tmpF8);
_tmpED->kind=_tmpF9;{
void*_tmpEE=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF3=_cycalloc(sizeof(*_tmpF3));*_tmpF3=_tmpED;_tmpF3;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpF0=_cycalloc(sizeof(*_tmpF0));({struct Cyc_Absyn_Decl*_tmp766=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->tag=5U,_tmpEF->f1=_tmpED;_tmpEF;}),0U);_tmpF0->hd=_tmp766;}),_tmpF0->tl=Cyc_Toc_result_decls;_tmpF0;});
({struct Cyc_List_List*_tmp768=({struct Cyc_List_List*_tmpF2=_region_malloc(d,sizeof(*_tmpF2));({struct _tuple25*_tmp767=({struct _tuple25*_tmpF1=_region_malloc(d,sizeof(*_tmpF1));_tmpF1->f1=_tmpEE,_tmpF1->f2=_tmpF9,_tmpF1->f3=_tmpF8;_tmpF1;});_tmpF2->hd=_tmp767;}),_tmpF2->tl=*_tmpFA;_tmpF2;});*_tmpFA=_tmp768;});
return _tmpEE;};};}
# 609
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(({struct _tuple23*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=ak,_tmpFB->f2=fs;_tmpFB;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple26{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple27{struct Cyc_Toc_TocState*f1;struct _tuple26*f2;};struct _tuple28{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 618
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple27*env){
# 627
struct _tuple27 _tmpFC=*env;struct _tuple27 _tmpFD=_tmpFC;struct Cyc_List_List**_tmp120;struct _tuple1*_tmp11F;struct Cyc_List_List*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_List_List*_tmp11C;_LL1: _tmp120=(_tmpFD.f1)->abs_struct_types;_tmp11F=(_tmpFD.f2)->f1;_tmp11E=(_tmpFD.f2)->f2;_tmp11D=(_tmpFD.f2)->f3;_tmp11C=(_tmpFD.f2)->f4;_LL2:;
# 631
{struct Cyc_List_List*_tmpFE=*_tmp120;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct _tuple28*_tmpFF=(struct _tuple28*)_tmpFE->hd;struct _tuple28*_tmp100=_tmpFF;struct _tuple1*_tmp108;struct Cyc_List_List*_tmp107;void*_tmp106;_LL4: _tmp108=_tmp100->f1;_tmp107=_tmp100->f2;_tmp106=_tmp100->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp108,_tmp11F)== 0  && ({
int _tmp769=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11D);_tmp769 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp107);})){
int okay=1;
{struct Cyc_List_List*_tmp101=_tmp11D;for(0;_tmp101 != 0;(_tmp101=_tmp101->tl,_tmp107=_tmp107->tl)){
void*_tmp102=(void*)_tmp101->hd;
void*_tmp103=(void*)((struct Cyc_List_List*)_check_null(_tmp107))->hd;
{struct Cyc_Absyn_Kind*_tmp104=Cyc_Tcutil_type_kind(_tmp102);struct Cyc_Absyn_Kind*_tmp105=_tmp104;switch(((struct Cyc_Absyn_Kind*)_tmp105)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 642
 continue;default: _LLB: _LLC:
# 645
 if(Cyc_Tcutil_unify(_tmp102,_tmp103) || ({void*_tmp76A=Cyc_Toc_typ_to_c(_tmp102);Cyc_Tcutil_unify(_tmp76A,Cyc_Toc_typ_to_c(_tmp103));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 650
break;}}
# 652
if(okay)
return _tmp106;}}}{
# 659
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct _dyneither_ptr _tmp76C=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11A=({struct Cyc_Int_pa_PrintArg_struct _tmp70B;_tmp70B.tag=1U,_tmp70B.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp70B;});void*_tmp118[1U];_tmp118[0]=& _tmp11A;({struct _dyneither_ptr _tmp76B=({const char*_tmp119="_tuple%d";_tag_dyneither(_tmp119,sizeof(char),9U);});Cyc_aprintf(_tmp76B,_tag_dyneither(_tmp118,sizeof(void*),1U));});});*_tmp11B=_tmp76C;});_tmp11B;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp109=0;
# 663
({struct Cyc_List_List*_tmp76E=({struct Cyc_List_List*_tmp10B=_region_malloc(d,sizeof(*_tmp10B));({struct _tuple28*_tmp76D=({struct _tuple28*_tmp10A=_region_malloc(d,sizeof(*_tmp10A));_tmp10A->f1=_tmp11F,_tmp10A->f2=_tmp11D,_tmp10A->f3=x;_tmp10A;});_tmp10B->hd=_tmp76D;}),_tmp10B->tl=*_tmp120;_tmp10B;});*_tmp120=_tmp76E;});{
# 666
struct _RegionHandle _tmp10C=_new_region("r");struct _RegionHandle*r=& _tmp10C;_push_region(r);
{struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11E,_tmp11D);
for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){
struct Cyc_Absyn_Aggrfield*_tmp10E=(struct Cyc_Absyn_Aggrfield*)_tmp11C->hd;
void*t=_tmp10E->type;
struct Cyc_List_List*atts=_tmp10E->attributes;
# 676
if((_tmp11C->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t))) && !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({void*_tmp76F=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=6U,_tmp10F->f1=0;_tmp10F;});_tmp110->hd=_tmp76F;}),_tmp110->tl=atts;_tmp110;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10D,t));
# 683
if(Cyc_Tcutil_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=4U,({void*_tmp772=Cyc_Toc_void_star_type();(_tmp111->f1).elt_type=_tmp772;}),({struct Cyc_Absyn_Tqual _tmp771=Cyc_Absyn_empty_tqual(0U);(_tmp111->f1).tq=_tmp771;}),({
struct Cyc_Absyn_Exp*_tmp770=Cyc_Absyn_uint_exp(0U,0U);(_tmp111->f1).num_elts=_tmp770;}),(_tmp111->f1).zero_term=Cyc_Absyn_false_type,(_tmp111->f1).zt_loc=0U;_tmp111;});
# 687
_tmp109=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Aggrfield*_tmp773=({struct Cyc_Absyn_Aggrfield*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->name=_tmp10E->name,_tmp112->tq=Cyc_Toc_mt_tq,_tmp112->type=t,_tmp112->width=_tmp10E->width,_tmp112->attributes=atts,_tmp112->requires_clause=0;_tmp112;});_tmp113->hd=_tmp773;}),_tmp113->tl=_tmp109;_tmp113;});}
# 689
_tmp109=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp109);{
struct Cyc_Absyn_Aggrdecl*_tmp114=Cyc_Toc_make_c_struct_defn(xname,_tmp109);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Absyn_Decl*_tmp774=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=5U,_tmp115->f1=_tmp114;_tmp115;}),0U);_tmp116->hd=_tmp774;}),_tmp116->tl=Cyc_Toc_result_decls;_tmp116;});{
void*_tmp117=x;_npop_handler(0U);return _tmp117;};};}
# 667
;_pop_region(r);};};}
# 695
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 699
struct _tuple26 env=({struct _tuple26 _tmp70C;_tmp70C.f1=struct_name,_tmp70C.f2=type_vars,_tmp70C.f3=type_args,_tmp70C.f4=fields;_tmp70C;});
return((void*(*)(struct _tuple26*arg,void*(*f)(struct _RegionHandle*,struct _tuple27*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 707
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp777=({struct _dyneither_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));({struct _dyneither_ptr _tmp776=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp123=({struct Cyc_Int_pa_PrintArg_struct _tmp70D;_tmp70D.tag=1U,_tmp70D.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp70D;});void*_tmp121[1U];_tmp121[0]=& _tmp123;({struct _dyneither_ptr _tmp775=({const char*_tmp122="_tmp%X";_tag_dyneither(_tmp122,sizeof(char),7U);});Cyc_aprintf(_tmp775,_tag_dyneither(_tmp121,sizeof(void*),1U));});});*_tmp124=_tmp776;});_tmp124;});_tmp125->f2=_tmp777;});_tmp125;});}struct _tuple29{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 710
struct _tuple29 Cyc_Toc_temp_var_and_exp(){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple29 _tmp70E;_tmp70E.f1=v,({struct Cyc_Absyn_Exp*_tmp778=Cyc_Absyn_var_exp(v,0U);_tmp70E.f2=_tmp778;});_tmp70E;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 717
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp126=*env;struct _tuple30 _tmp127=_tmp126;struct Cyc_Xarray_Xarray*_tmp12F;_LL1: _tmp12F=(_tmp127.f1)->temp_labels;_LL2:;{
int _tmp128=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp779=_tmp128;_tmp779 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12F);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12F,_tmp128);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct _dyneither_ptr _tmp77B=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12D=({struct Cyc_Int_pa_PrintArg_struct _tmp70F;_tmp70F.tag=1U,_tmp70F.f1=(unsigned int)_tmp128;_tmp70F;});void*_tmp12B[1U];_tmp12B[0]=& _tmp12D;({struct _dyneither_ptr _tmp77A=({const char*_tmp12C="_LL%X";_tag_dyneither(_tmp12C,sizeof(char),6U);});Cyc_aprintf(_tmp77A,_tag_dyneither(_tmp12B,sizeof(void*),1U));});});*_tmp12E=_tmp77B;});_tmp12E;});
if(({int _tmp77C=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12F,res);_tmp77C != _tmp128;}))
({void*_tmp129=0U;({struct _dyneither_ptr _tmp77D=({const char*_tmp12A="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp12A,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp77D,_tag_dyneither(_tmp129,sizeof(void*),0U));});});
return res;};};}
# 727
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 733
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp130=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp130))->hd)->name)!= 0){
++ ans;
_tmp130=_tmp130->tl;}
# 740
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 746
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp131=*a;struct _tuple9 _tmp132=_tmp131;struct _dyneither_ptr*_tmp136;struct Cyc_Absyn_Tqual _tmp135;void*_tmp134;_LL1: _tmp136=_tmp132.f1;_tmp135=_tmp132.f2;_tmp134=_tmp132.f3;_LL2:;
return({struct _tuple9*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->f1=_tmp136,_tmp133->f2=_tmp135,({void*_tmp77E=Cyc_Toc_typ_to_c(_tmp134);_tmp133->f3=_tmp77E;});_tmp133;});}
# 769 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_tmp138=_tmp137;void*_tmp13E;void*_tmp13D;struct Cyc_Absyn_Tqual _tmp13C;struct Cyc_Absyn_Exp*_tmp13B;void*_tmp13A;unsigned int _tmp139;switch(*((int*)_tmp138)){case 4U: _LL1: _tmp13D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).elt_type;_tmp13C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).tq;_tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).num_elts;_tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zero_term;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zt_loc;_LL2:
# 772
 return({void*_tmp77F=Cyc_Toc_typ_to_c_array(_tmp13D);Cyc_Absyn_cstar_type(_tmp77F,_tmp13C);});case 1U: _LL3: _tmp13E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp138)->f2;if(_tmp13E != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13E);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 778
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 780
struct Cyc_Absyn_Aggrfield*_tmp13F=({struct Cyc_Absyn_Aggrfield*_tmp140=_cycalloc(sizeof(*_tmp140));*_tmp140=*f;_tmp140;});
_tmp13F->type=new_type;
_tmp13F->requires_clause=0;
_tmp13F->tq=Cyc_Toc_mt_tq;
return _tmp13F;}
# 787
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 789
return;}
# 792
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp141=Cyc_Tcutil_compress(t);void*_tmp142=_tmp141;struct Cyc_Absyn_Tvar*_tmp143;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->tag == 2U){_LL1: _tmp143=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 798
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp144=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp145=_tmp144;if(((struct Cyc_Absyn_Kind*)_tmp145)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 804
static int Cyc_Toc_is_void_star(void*t){
void*_tmp146=Cyc_Tcutil_compress(t);void*_tmp147=_tmp146;void*_tmp148;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp147)->tag == 3U){_LL1: _tmp148=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp147)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_void_type(_tmp148);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 810
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 813
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 817
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp149=t;struct Cyc_Absyn_Datatypedecl*_tmp192;void**_tmp191;struct Cyc_Absyn_Enumdecl*_tmp190;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18D;struct _tuple1*_tmp18C;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Typedefdecl*_tmp18A;void*_tmp189;enum Cyc_Absyn_AggrKind _tmp188;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Tqual _tmp185;void*_tmp184;struct Cyc_List_List*_tmp183;int _tmp182;struct Cyc_Absyn_VarargInfo*_tmp181;struct Cyc_List_List*_tmp180;void*_tmp17F;struct Cyc_Absyn_Tqual _tmp17E;struct Cyc_Absyn_Exp*_tmp17D;unsigned int _tmp17C;void*_tmp17B;struct Cyc_Absyn_Tqual _tmp17A;void*_tmp179;struct Cyc_Absyn_Tvar*_tmp178;void**_tmp177;struct Cyc_List_List*_tmp176;struct _tuple1*_tmp175;void*_tmp174;union Cyc_Absyn_AggrInfo _tmp173;struct Cyc_List_List*_tmp172;struct Cyc_Absyn_Datatypedecl*_tmp171;struct Cyc_Absyn_Datatypefield*_tmp170;switch(*((int*)_tmp149)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 832
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp171=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1).KnownDatatypefield).val).f1;_tmp170=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1).KnownDatatypefield).val).f2;_LLA:
# 834
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp170->name,_tmp171->name));}else{_LLB: _LLC:
# 836
({void*_tmp14A=0U;({struct _dyneither_ptr _tmp780=({const char*_tmp14B="unresolved DatatypeFieldType";_tag_dyneither(_tmp14B,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp780,_tag_dyneither(_tmp14A,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 845
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp174=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1;_tmp173=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1;_tmp172=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f2;_LL1C:
# 892
{union Cyc_Absyn_AggrInfo _tmp15B=_tmp173;if((_tmp15B.UnknownAggr).tag == 1){_LL45: _LL46:
# 894
 if(_tmp172 == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->tag=0U,_tmp15C->f1=_tmp174,_tmp15C->f2=0;_tmp15C;});}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 898
struct Cyc_Absyn_Aggrdecl*_tmp15D=Cyc_Absyn_get_known_aggrdecl(_tmp173);
if(_tmp15D->expected_mem_kind){
# 901
if(_tmp15D->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp160=({struct Cyc_String_pa_PrintArg_struct _tmp711;_tmp711.tag=0U,({
struct _dyneither_ptr _tmp781=(struct _dyneither_ptr)((int)_tmp15D->kind == (int)Cyc_Absyn_UnionA?({const char*_tmp162="union";_tag_dyneither(_tmp162,sizeof(char),6U);}):({const char*_tmp163="struct";_tag_dyneither(_tmp163,sizeof(char),7U);}));_tmp711.f1=_tmp781;});_tmp711;});struct Cyc_String_pa_PrintArg_struct _tmp161=({struct Cyc_String_pa_PrintArg_struct _tmp710;_tmp710.tag=0U,({
struct _dyneither_ptr _tmp782=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15D->name));_tmp710.f1=_tmp782;});_tmp710;});void*_tmp15E[2U];_tmp15E[0]=& _tmp160,_tmp15E[1]=& _tmp161;({struct _dyneither_ptr _tmp783=({const char*_tmp15F="%s %s was never defined.";_tag_dyneither(_tmp15F,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp783,_tag_dyneither(_tmp15E,sizeof(void*),2U));});});}
# 907
if((int)_tmp15D->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp164=_tmp15D->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15D->impl))->fields;
if(_tmp164 == 0)return Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp164))->tl != 0;_tmp164=_tmp164->tl){;}{
void*_tmp165=((struct Cyc_Absyn_Aggrfield*)_tmp164->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp165))){
if(_tmp15D->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp168=({struct Cyc_String_pa_PrintArg_struct _tmp712;_tmp712.tag=0U,({struct _dyneither_ptr _tmp784=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15D->name));_tmp712.f1=_tmp784;});_tmp712;});void*_tmp166[1U];_tmp166[0]=& _tmp168;({struct _dyneither_ptr _tmp785=({const char*_tmp167="struct %s ended up being abstract.";_tag_dyneither(_tmp167,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp785,_tag_dyneither(_tmp166,sizeof(void*),1U));});});{
# 920
struct _RegionHandle _tmp169=_new_region("r");struct _RegionHandle*r=& _tmp169;_push_region(r);
{struct Cyc_List_List*_tmp16A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15D->tvs,_tmp172);
void*_tmp16B=Cyc_Tcutil_rsubstitute(r,_tmp16A,_tmp165);
if(Cyc_Toc_is_abstract_type(_tmp16B)){void*_tmp16C=Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp16C;}{
void*_tmp16D=Cyc_Toc_add_struct_type(_tmp15D->name,_tmp15D->tvs,_tmp172,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15D->impl))->fields);_npop_handler(0U);return _tmp16D;};}
# 921
;_pop_region(r);};}
# 926
return Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_strctq);};};};case 15U: _LL1D: _tmp175=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1;_LL1E:
 return t;case 16U: _LL1F: _tmp176=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp176);return t;case 4U: _LL23: _LL24:
# 938
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 943
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp177=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp149)->f2;_LL4:
# 821
 if(*_tmp177 == 0){
*_tmp177=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 825
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp177));case 2U: _LL5: _tmp178=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->f1;_LL6:
# 827
 if((int)(Cyc_Tcutil_tvar_kind(_tmp178,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_AnyKind)
# 829
return Cyc_Absyn_void_type;else{
# 831
return Cyc_Toc_void_star_type();}case 3U: _LLD: _tmp17B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_type;_tmp17A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_tq;_tmp179=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).ptr_atts).bounds;_LLE:
# 840
 _tmp17B=Cyc_Toc_typ_to_c(_tmp17B);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp179)== 0)
return Cyc_Toc_dyneither_ptr_type();else{
# 844
return Cyc_Absyn_star_type(_tmp17B,Cyc_Absyn_heap_rgn_type,_tmp17A,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp17F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).elt_type;_tmp17E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).tq;_tmp17D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).num_elts;_tmp17C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).zt_loc;_LL14:
# 848
 return({void*_tmp789=Cyc_Toc_typ_to_c(_tmp17F);struct Cyc_Absyn_Tqual _tmp788=_tmp17E;struct Cyc_Absyn_Exp*_tmp787=_tmp17D;void*_tmp786=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp789,_tmp788,_tmp787,_tmp786,_tmp17C);});case 5U: _LL15: _tmp185=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_tqual;_tmp184=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_type;_tmp183=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).args;_tmp182=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).c_varargs;_tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).cyc_varargs;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).attributes;_LL16: {
# 854
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
# 865
struct Cyc_List_List*_tmp150=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp183);
if(_tmp181 != 0){
# 868
void*_tmp151=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp181->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp152=({struct _tuple9*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->f1=_tmp181->name,_tmp154->f2=_tmp181->tq,_tmp154->f3=_tmp151;_tmp154;});
_tmp150=({struct Cyc_List_List*_tmp78A=_tmp150;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp78A,({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=_tmp152,_tmp153->tl=0;_tmp153;}));});}
# 872
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->tag=5U,(_tmp155->f1).tvars=0,(_tmp155->f1).effect=0,(_tmp155->f1).ret_tqual=_tmp185,({void*_tmp78B=Cyc_Toc_typ_to_c(_tmp184);(_tmp155->f1).ret_type=_tmp78B;}),(_tmp155->f1).args=_tmp150,(_tmp155->f1).c_varargs=_tmp182,(_tmp155->f1).cyc_varargs=0,(_tmp155->f1).rgn_po=0,(_tmp155->f1).attributes=_tmp14C,(_tmp155->f1).requires_clause=0,(_tmp155->f1).requires_relns=0,(_tmp155->f1).ensures_clause=0,(_tmp155->f1).ensures_relns=0;_tmp155;});};}case 6U: _LL17: _tmp186=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp149)->f1;_LL18: {
# 877
struct Cyc_List_List*_tmp156=0;
for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
_tmp156=({struct Cyc_List_List*_tmp158=_cycalloc(sizeof(*_tmp158));({struct _tuple12*_tmp78D=({struct _tuple12*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->f1=(*((struct _tuple12*)_tmp186->hd)).f1,({void*_tmp78C=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp186->hd)).f2);_tmp157->f2=_tmp78C;});_tmp157;});_tmp158->hd=_tmp78D;}),_tmp158->tl=_tmp156;_tmp158;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp156));}case 7U: _LL19: _tmp188=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f1;_tmp187=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f2;_LL1A: {
# 886
struct Cyc_List_List*_tmp159=0;
for(0;_tmp187 != 0;_tmp187=_tmp187->tl){
_tmp159=({struct Cyc_List_List*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct Cyc_Absyn_Aggrfield*_tmp78F=({struct Cyc_Absyn_Aggrfield*_tmp78E=(struct Cyc_Absyn_Aggrfield*)_tmp187->hd;Cyc_Toc_aggrfield_to_c(_tmp78E,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp187->hd)->type));});_tmp15A->hd=_tmp78F;}),_tmp15A->tl=_tmp159;_tmp15A;});}
return({enum Cyc_Absyn_AggrKind _tmp790=_tmp188;Cyc_Toc_add_anon_aggr_type(_tmp790,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp159));});}case 8U: _LL21: _tmp18C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f1;_tmp18B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f2;_tmp18A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f3;_tmp189=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f4;_LL22:
# 932
 if(_tmp189 == 0){
if(_tmp18B != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->tag=8U,_tmp16E->f1=_tmp18C,_tmp16E->f2=0,_tmp16E->f3=_tmp18A,_tmp16E->f4=0;_tmp16E;});else{
return t;}}else{
# 937
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=8U,_tmp16F->f1=_tmp18C,_tmp16F->f2=0,_tmp16F->f3=_tmp18A,({void*_tmp791=Cyc_Toc_typ_to_c(_tmp189);_tmp16F->f4=_tmp791;});_tmp16F;});}case 9U: _LL2B: _tmp18D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp149)->f1;_LL2C:
# 948
 return t;case 11U: _LL2D: _tmp18E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp149)->f1;_LL2E:
# 953
 if(_tmp18E->topt != 0)return Cyc_Toc_typ_to_c((void*)_check_null(_tmp18E->topt));else{
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)){case 0U: _LL2F: _tmp18F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL30:
# 956
 Cyc_Toc_aggrdecl_to_c(_tmp18F);
if((int)_tmp18F->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_unionq_type);else{
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp190=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL32:
# 961
 Cyc_Toc_enumdecl_to_c(_tmp190);
return t;default: _LL33: _tmp192=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_tmp191=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f2;_LL34:
# 964
 Cyc_Toc_datatypedecl_to_c(_tmp192);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp191)));}}_LL0:;}
# 969
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp193=t;void*_tmp195;struct Cyc_Absyn_Tqual _tmp194;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->tag == 4U){_LL1: _tmp195=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).elt_type;_tmp194=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).tq;_LL2:
# 972
 return({void*_tmp792=Cyc_Absyn_star_type(_tmp195,Cyc_Absyn_heap_rgn_type,_tmp194,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp792,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 979
static int Cyc_Toc_atomic_type(void*t){
void*_tmp196=Cyc_Tcutil_compress(t);void*_tmp197=_tmp196;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*_tmp1A9;void*_tmp1A8;void*_tmp1A7;struct Cyc_List_List*_tmp1A6;switch(*((int*)_tmp197)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp1A7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f1;_tmp1A6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2;_LL4: {
# 983
void*_tmp198=_tmp1A7;struct Cyc_Absyn_Datatypedecl*_tmp1A2;struct Cyc_Absyn_Datatypefield*_tmp1A1;union Cyc_Absyn_AggrInfo _tmp1A0;switch(*((int*)_tmp198)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D:
 return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp1A0=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp198)->f1;_LL25:
# 991
{union Cyc_Absyn_AggrInfo _tmp199=_tmp1A0;if((_tmp199.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 995
struct Cyc_Absyn_Aggrdecl*_tmp19A=Cyc_Absyn_get_known_aggrdecl(_tmp1A0);
if(_tmp19A->impl == 0)
return 0;
{struct Cyc_List_List*_tmp19B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp19A->impl))->fields;for(0;_tmp19B != 0;_tmp19B=_tmp19B->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp19B->hd)->type))return 0;}}
return 1;};case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp1A2=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownDatatypefield).val).f1;_tmp1A1=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownDatatypefield).val).f2;_LL27:
# 1002
{struct Cyc_List_List*_tmp19C=_tmp1A1->typs;for(0;_tmp19C != 0;_tmp19C=_tmp19C->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp19C->hd)).f2))return 0;}}
return 1;}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp19F=({struct Cyc_String_pa_PrintArg_struct _tmp713;_tmp713.tag=0U,({struct _dyneither_ptr _tmp793=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp713.f1=_tmp793;});_tmp713;});void*_tmp19D[1U];_tmp19D[0]=& _tmp19F;({struct _dyneither_ptr _tmp794=({const char*_tmp19E="atomic_typ:  bad type %s";_tag_dyneither(_tmp19E,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp794,_tag_dyneither(_tmp19D,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 1007
 return 1;case 4U: _LL7: _tmp1A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->f1).elt_type;_LL8:
 return Cyc_Toc_atomic_type(_tmp1A8);case 7U: _LL9: _tmp1A9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp197)->f2;_LLA:
# 1015
 for(0;_tmp1A9 != 0;_tmp1A9=_tmp1A9->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp1A9->hd)->type))return 0;}
return 1;case 6U: _LLB: _tmp1AA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp197)->f1;_LLC:
# 1019
 for(0;_tmp1AA != 0;_tmp1AA=_tmp1AA->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp1AA->hd)).f2))return 0;}
return 1;case 3U: _LLD: _LLE:
# 1024
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp1A5=({struct Cyc_String_pa_PrintArg_struct _tmp714;_tmp714.tag=0U,({struct _dyneither_ptr _tmp795=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp714.f1=_tmp795;});_tmp714;});void*_tmp1A3[1U];_tmp1A3[0]=& _tmp1A5;({struct _dyneither_ptr _tmp796=({const char*_tmp1A4="atomic_typ:  bad type %s";_tag_dyneither(_tmp1A4,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp796,_tag_dyneither(_tmp1A3,sizeof(void*),1U));});});}_LL0:;}
# 1030
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1AB=Cyc_Tcutil_compress(t);void*_tmp1AC=_tmp1AB;struct Cyc_List_List*_tmp1B8;union Cyc_Absyn_AggrInfo _tmp1B7;switch(*((int*)_tmp1AC)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->tag == 20U){_LL1: _tmp1B7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->f1;_LL2: {
# 1033
struct Cyc_Absyn_Aggrdecl*_tmp1AD=Cyc_Absyn_get_known_aggrdecl(_tmp1B7);
if(_tmp1AD->impl == 0)
({void*_tmp1AE=0U;({struct _dyneither_ptr _tmp797=({const char*_tmp1AF="is_poly_field: type missing fields";_tag_dyneither(_tmp1AF,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp797,_tag_dyneither(_tmp1AE,sizeof(void*),0U));});});
_tmp1B8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AD->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1B8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AC)->f2;_LL4: {
# 1038
struct Cyc_Absyn_Aggrfield*_tmp1B0=Cyc_Absyn_lookup_field(_tmp1B8,f);
if(_tmp1B0 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B3=({struct Cyc_String_pa_PrintArg_struct _tmp715;_tmp715.tag=0U,_tmp715.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp715;});void*_tmp1B1[1U];_tmp1B1[0]=& _tmp1B3;({struct _dyneither_ptr _tmp798=({const char*_tmp1B2="is_poly_field: bad field %s";_tag_dyneither(_tmp1B2,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp798,_tag_dyneither(_tmp1B1,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1B0->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B6=({struct Cyc_String_pa_PrintArg_struct _tmp716;_tmp716.tag=0U,({struct _dyneither_ptr _tmp799=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp716.f1=_tmp799;});_tmp716;});void*_tmp1B4[1U];_tmp1B4[0]=& _tmp1B6;({struct _dyneither_ptr _tmp79A=({const char*_tmp1B5="is_poly_field: bad type %s";_tag_dyneither(_tmp1B5,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79A,_tag_dyneither(_tmp1B4,sizeof(void*),1U));});});}_LL0:;}
# 1049
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B9=e->r;void*_tmp1BA=_tmp1B9;struct Cyc_Absyn_Exp*_tmp1C4;struct _dyneither_ptr*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;switch(*((int*)_tmp1BA)){case 21U: _LL1: _tmp1C2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1BA)->f1;_tmp1C1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1BA)->f2;_LL2:
# 1052
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C2->topt),_tmp1C1) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1BA)->f1;_tmp1C3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1BA)->f2;_LL4: {
# 1055
void*_tmp1BB=Cyc_Tcutil_compress((void*)_check_null(_tmp1C4->topt));void*_tmp1BC=_tmp1BB;void*_tmp1C0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BC)->tag == 3U){_LL8: _tmp1C0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BC)->f1).elt_type;_LL9:
# 1057
 return Cyc_Toc_is_poly_field(_tmp1C0,_tmp1C3) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BF=({struct Cyc_String_pa_PrintArg_struct _tmp717;_tmp717.tag=0U,({struct _dyneither_ptr _tmp79B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C4->topt)));_tmp717.f1=_tmp79B;});_tmp717;});void*_tmp1BD[1U];_tmp1BD[0]=& _tmp1BF;({struct _dyneither_ptr _tmp79C=({const char*_tmp1BE="is_poly_project: bad type %s";_tag_dyneither(_tmp1BE,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79C,_tag_dyneither(_tmp1BD,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1060
 return 0;}_LL0:;}
# 1065
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C5[1U];_tmp1C5[0]=s;({struct Cyc_Absyn_Exp*_tmp79D=fn_e;Cyc_Toc_fncall_exp_dl(_tmp79D,_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1070
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=s,_tmp1C6[1]=n;({struct Cyc_Absyn_Exp*_tmp79E=fn_e;Cyc_Toc_fncall_exp_dl(_tmp79E,_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1075
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=rgn,_tmp1C7[1]=s;({struct Cyc_Absyn_Exp*_tmp79F=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp79F,_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1079
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=rgn,_tmp1C8[1]=s;({struct Cyc_Absyn_Exp*_tmp7A0=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp7A0,_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1083
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1C9[3U];_tmp1C9[0]=rgn,_tmp1C9[1]=s,_tmp1C9[2]=n;({struct Cyc_Absyn_Exp*_tmp7A1=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp7A1,_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1087
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({void*_tmp1CA=0U;({struct Cyc_Absyn_Exp*_tmp7A2=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp7A2,_tag_dyneither(_tmp1CA,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1092
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1100
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1CB=e->r;void*_tmp1CC=_tmp1CB;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CC)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CC)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1105
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));({struct Cyc_Absyn_Decl*_tmp7A3=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->tag=0U,_tmp1CD->f1=vd;_tmp1CD;}),0U);_tmp1CE->hd=_tmp7A3;}),_tmp1CE->tl=Cyc_Toc_result_decls;_tmp1CE;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1113
 xexp=({void*_tmp7A4=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp7A4,e);});
# 1115
xplussz=({void*_tmp7A5=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp7A5,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1118
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1CF[3U];({struct _tuple19*_tmp7A8=({struct _tuple19*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->f1=0,_tmp1D0->f2=xexp;_tmp1D0;});_tmp1CF[0]=_tmp7A8;}),({
struct _tuple19*_tmp7A7=({struct _tuple19*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->f1=0,_tmp1D1->f2=xexp;_tmp1D1;});_tmp1CF[1]=_tmp7A7;}),({
struct _tuple19*_tmp7A6=({struct _tuple19*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xplussz;_tmp1D2;});_tmp1CF[2]=_tmp7A6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CF,sizeof(struct _tuple19*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1153 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1D4=_region_malloc(r,sizeof(*_tmp1D4));_tmp1D4->break_lab=0,_tmp1D4->continue_lab=0,_tmp1D4->fallthru_info=0,_tmp1D4->toplevel=1,({int*_tmp7A9=({int*_tmp1D3=_region_malloc(r,sizeof(*_tmp1D3));*_tmp1D3=0;_tmp1D3;});_tmp1D4->in_lhs=_tmp7A9;}),_tmp1D4->rgn=r;_tmp1D4;});}
# 1157
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D5=nv;int _tmp1D6;_LL1: _tmp1D6=_tmp1D5->toplevel;_LL2:;
return _tmp1D6;}
# 1161
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D7=e;struct _dyneither_ptr**_tmp1DD;struct _dyneither_ptr**_tmp1DC;struct Cyc_Toc_FallthruInfo*_tmp1DB;int _tmp1DA;int*_tmp1D9;_LL1: _tmp1DD=_tmp1D7->break_lab;_tmp1DC=_tmp1D7->continue_lab;_tmp1DB=_tmp1D7->fallthru_info;_tmp1DA=_tmp1D7->toplevel;_tmp1D9=_tmp1D7->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1D8=_region_malloc(r,sizeof(*_tmp1D8));_tmp1D8->break_lab=_tmp1DD,_tmp1D8->continue_lab=_tmp1DC,_tmp1D8->fallthru_info=_tmp1DB,_tmp1D8->toplevel=0,_tmp1D8->in_lhs=_tmp1D9,_tmp1D8->rgn=r;_tmp1D8;});}
# 1165
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DE=e;struct _dyneither_ptr**_tmp1E4;struct _dyneither_ptr**_tmp1E3;struct Cyc_Toc_FallthruInfo*_tmp1E2;int _tmp1E1;int*_tmp1E0;_LL1: _tmp1E4=_tmp1DE->break_lab;_tmp1E3=_tmp1DE->continue_lab;_tmp1E2=_tmp1DE->fallthru_info;_tmp1E1=_tmp1DE->toplevel;_tmp1E0=_tmp1DE->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DF=_region_malloc(r,sizeof(*_tmp1DF));_tmp1DF->break_lab=_tmp1E4,_tmp1DF->continue_lab=_tmp1E3,_tmp1DF->fallthru_info=_tmp1E2,_tmp1DF->toplevel=1,_tmp1DF->in_lhs=_tmp1E0,_tmp1DF->rgn=r;_tmp1DF;});}
# 1169
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1E5=nv;int*_tmp1E6;_LL1: _tmp1E6=_tmp1E5->in_lhs;_LL2:;
return*_tmp1E6;}
# 1173
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E7=e;int*_tmp1E8;_LL1: _tmp1E8=_tmp1E7->in_lhs;_LL2:;
*_tmp1E8=b;}
# 1178
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E9=e;struct _dyneither_ptr**_tmp1EF;struct _dyneither_ptr**_tmp1EE;struct Cyc_Toc_FallthruInfo*_tmp1ED;int _tmp1EC;int*_tmp1EB;_LL1: _tmp1EF=_tmp1E9->break_lab;_tmp1EE=_tmp1E9->continue_lab;_tmp1ED=_tmp1E9->fallthru_info;_tmp1EC=_tmp1E9->toplevel;_tmp1EB=_tmp1E9->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1EA=_region_malloc(r,sizeof(*_tmp1EA));_tmp1EA->break_lab=_tmp1EF,_tmp1EA->continue_lab=_tmp1EE,_tmp1EA->fallthru_info=_tmp1ED,_tmp1EA->toplevel=_tmp1EC,_tmp1EA->in_lhs=_tmp1EB,_tmp1EA->rgn=r;_tmp1EA;});}
# 1185
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F0=e;struct _dyneither_ptr**_tmp1F6;struct _dyneither_ptr**_tmp1F5;struct Cyc_Toc_FallthruInfo*_tmp1F4;int _tmp1F3;int*_tmp1F2;_LL1: _tmp1F6=_tmp1F0->break_lab;_tmp1F5=_tmp1F0->continue_lab;_tmp1F4=_tmp1F0->fallthru_info;_tmp1F3=_tmp1F0->toplevel;_tmp1F2=_tmp1F0->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));_tmp1F1->break_lab=0,_tmp1F1->continue_lab=0,_tmp1F1->fallthru_info=_tmp1F4,_tmp1F1->toplevel=_tmp1F3,_tmp1F1->in_lhs=_tmp1F2,_tmp1F1->rgn=r;_tmp1F1;});}
# 1191
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1195
struct Cyc_Toc_Env*_tmp1F7=e;struct _dyneither_ptr**_tmp1FF;struct _dyneither_ptr**_tmp1FE;struct Cyc_Toc_FallthruInfo*_tmp1FD;int _tmp1FC;int*_tmp1FB;_LL1: _tmp1FF=_tmp1F7->break_lab;_tmp1FE=_tmp1F7->continue_lab;_tmp1FD=_tmp1F7->fallthru_info;_tmp1FC=_tmp1F7->toplevel;_tmp1FB=_tmp1F7->in_lhs;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));
_tmp1FA->label=fallthru_l,_tmp1FA->binders=fallthru_binders;_tmp1FA;});
return({struct Cyc_Toc_Env*_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));({struct _dyneither_ptr**_tmp7AA=({struct _dyneither_ptr**_tmp1F8=_region_malloc(r,sizeof(*_tmp1F8));*_tmp1F8=break_l;_tmp1F8;});_tmp1F9->break_lab=_tmp7AA;}),_tmp1F9->continue_lab=_tmp1FE,_tmp1F9->fallthru_info=fi,_tmp1F9->toplevel=_tmp1FC,_tmp1F9->in_lhs=_tmp1FB,_tmp1F9->rgn=r;_tmp1F9;});};}
# 1200
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1202
struct Cyc_Toc_Env*_tmp200=e;struct _dyneither_ptr**_tmp207;struct _dyneither_ptr**_tmp206;struct Cyc_Toc_FallthruInfo*_tmp205;int _tmp204;int*_tmp203;_LL1: _tmp207=_tmp200->break_lab;_tmp206=_tmp200->continue_lab;_tmp205=_tmp200->fallthru_info;_tmp204=_tmp200->toplevel;_tmp203=_tmp200->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp202=_region_malloc(r,sizeof(*_tmp202));({struct _dyneither_ptr**_tmp7AB=({struct _dyneither_ptr**_tmp201=_region_malloc(r,sizeof(*_tmp201));*_tmp201=break_l;_tmp201;});_tmp202->break_lab=_tmp7AB;}),_tmp202->continue_lab=_tmp206,_tmp202->fallthru_info=0,_tmp202->toplevel=_tmp204,_tmp202->in_lhs=_tmp203,_tmp202->rgn=r;_tmp202;});}
# 1209
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1212
struct Cyc_Toc_Env*_tmp208=e;struct _dyneither_ptr**_tmp20F;struct _dyneither_ptr**_tmp20E;struct Cyc_Toc_FallthruInfo*_tmp20D;int _tmp20C;int*_tmp20B;_LL1: _tmp20F=_tmp208->break_lab;_tmp20E=_tmp208->continue_lab;_tmp20D=_tmp208->fallthru_info;_tmp20C=_tmp208->toplevel;_tmp20B=_tmp208->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp20A=_region_malloc(r,sizeof(*_tmp20A));_tmp20A->break_lab=0,_tmp20A->continue_lab=_tmp20E,({struct Cyc_Toc_FallthruInfo*_tmp7AC=({struct Cyc_Toc_FallthruInfo*_tmp209=_region_malloc(r,sizeof(*_tmp209));_tmp209->label=next_l,_tmp209->binders=0;_tmp209;});_tmp20A->fallthru_info=_tmp7AC;}),_tmp20A->toplevel=_tmp20C,_tmp20A->in_lhs=_tmp20B,_tmp20A->rgn=r;_tmp20A;});}
# 1225 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1228
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp210=e->annot;void*_tmp211=_tmp210;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({void*_tmp212=0U;({unsigned int _tmp7AE=e->loc;struct _dyneither_ptr _tmp7AD=({const char*_tmp213="Toc: do_null_check";_tag_dyneither(_tmp213,sizeof(char),19U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp7AE,_tmp7AD,_tag_dyneither(_tmp212,sizeof(void*),0U));});});}}}}}}_LL0:;}
# 1244
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1246
int ans;
int _tmp214=Cyc_Toc_in_lhs(nv);
void*_tmp215=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));
void*_tmp216=Cyc_Toc_typ_to_c(_tmp215);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp217=_tmp215;void*_tmp22B;struct Cyc_Absyn_Tqual _tmp22A;void*_tmp229;void*_tmp228;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->tag == 3U){_LL1: _tmp22B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).elt_type;_tmp22A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).elt_tq;_tmp229=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).ptr_atts).bounds;_tmp228=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).ptr_atts).zero_term;_LL2:
# 1257
{void*_tmp218=annot;struct Cyc_Absyn_Exp*_tmp225;struct Cyc_Absyn_Exp*_tmp224;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1259
 if(!((unsigned int)({void*_tmp7AF=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7AF,_tmp229);})))
({void*_tmp7B3=({void*_tmp7B2=({void*_tmp7B0=Cyc_Toc_typ_to_c(_tmp22B);Cyc_Absyn_cstar_type(_tmp7B0,_tmp22A);});Cyc_Toc_cast_it_r(_tmp7B2,({
struct Cyc_Absyn_Exp*_tmp7B1=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7B1,Cyc_Toc_curr_sp,0U);}));});
# 1260
ptr->r=_tmp7B3;});
# 1262
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1265
 if(!((unsigned int)({void*_tmp7B4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7B4,_tmp229);})))
goto _LLB;
# 1268
({void*_tmp7B8=({void*_tmp7B7=_tmp216;Cyc_Toc_cast_it_r(_tmp7B7,({struct Cyc_Absyn_Exp*_tmp219[1U];({struct Cyc_Absyn_Exp*_tmp7B5=Cyc_Absyn_new_exp(ptr->r,0U);_tmp219[0]=_tmp7B5;});({struct Cyc_Absyn_Exp*_tmp7B6=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp7B6,_tag_dyneither(_tmp219,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp7B8;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1274
void*ta1=Cyc_Toc_typ_to_c(_tmp22B);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp7BD=({void*_tmp7BC=Cyc_Absyn_cstar_type(ta1,_tmp22A);Cyc_Toc_cast_it_r(_tmp7BC,({struct Cyc_Absyn_Exp*_tmp21A[3U];({
# 1278
struct Cyc_Absyn_Exp*_tmp7BA=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21A[0]=_tmp7BA;}),({
struct Cyc_Absyn_Exp*_tmp7B9=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp21A[1]=_tmp7B9;}),_tmp21A[2]=ind;({struct Cyc_Absyn_Exp*_tmp7BB=Cyc_Toc__check_dyneither_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp7BB,_tag_dyneither(_tmp21A,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1276
ptr->r=_tmp7BD;});
# 1281
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp224=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp218)->f1;_LLF:
# 1284
 fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp225=_tmp224;goto _LL11;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp218)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp225=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp218)->f1;_LL11: {
# 1287
void*ta1=Cyc_Toc_typ_to_c(_tmp22B);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1292
struct _tuple13 _tmp21B=Cyc_Evexp_eval_const_uint_exp(_tmp225);struct _tuple13 _tmp21C=_tmp21B;unsigned int _tmp221;int _tmp220;_LL15: _tmp221=_tmp21C.f1;_tmp220=_tmp21C.f2;_LL16:;
if((!_tmp220  || _tmp221 != (unsigned int)1) || !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1295
({void*_tmp7C2=({void*_tmp7C1=Cyc_Absyn_cstar_type(ta1,_tmp22A);Cyc_Toc_cast_it_r(_tmp7C1,({struct Cyc_Absyn_Exp*_tmp21D[4U];({
struct Cyc_Absyn_Exp*_tmp7BF=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21D[0]=_tmp7BF;}),_tmp21D[1]=_tmp225,({
struct Cyc_Absyn_Exp*_tmp7BE=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp21D[2]=_tmp7BE;}),_tmp21D[3]=ind;({struct Cyc_Absyn_Exp*_tmp7C0=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7C0,_tag_dyneither(_tmp21D,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1295
ptr->r=_tmp7C2;});
# 1298
ans=1;}else{
# 1301
if(Cyc_Toc_is_zero(_tmp225)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1304
({void*_tmp7C6=({void*_tmp7C5=_tmp216;Cyc_Toc_cast_it_r(_tmp7C5,({struct Cyc_Absyn_Exp*_tmp21E[1U];({struct Cyc_Absyn_Exp*_tmp7C3=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp21E[0]=_tmp7C3;});({struct Cyc_Absyn_Exp*_tmp7C4=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp7C4,_tag_dyneither(_tmp21E,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1304
ptr->r=_tmp7C6;});
# 1306
ans=0;}else{
# 1310
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp7CA=({void*_tmp7C9=Cyc_Absyn_cstar_type(ta1,_tmp22A);Cyc_Toc_cast_it_r(_tmp7C9,({struct Cyc_Absyn_Exp*_tmp21F[3U];({
struct Cyc_Absyn_Exp*_tmp7C7=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21F[0]=_tmp7C7;}),_tmp21F[1]=_tmp225,_tmp21F[2]=ind;({struct Cyc_Absyn_Exp*_tmp7C8=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7C8,_tag_dyneither(_tmp21F,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1311
ptr->r=_tmp7CA;});
# 1313
ans=1;}}
# 1316
goto _LL5;}}else{_LL12: _LL13:
({void*_tmp222=0U;({struct _dyneither_ptr _tmp7CB=({const char*_tmp223="FIX: ptr_use_to_c, bad annotation";_tag_dyneither(_tmp223,sizeof(char),34U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp7CB,_tag_dyneither(_tmp222,sizeof(void*),0U));});});}}}}}}_LL5:;}
# 1319
Cyc_Toc_set_lhs(nv,_tmp214);
return ans;}else{_LL3: _LL4:
({void*_tmp226=0U;({struct _dyneither_ptr _tmp7CC=({const char*_tmp227="ptr_use_to_c: non-pointer-type";_tag_dyneither(_tmp227,sizeof(char),31U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp7CC,_tag_dyneither(_tmp226,sizeof(void*),0U));});});}_LL0:;};}
# 1325
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp22C=0U;({struct _dyneither_ptr _tmp7CD=({const char*_tmp22D="Missing type in primop ";_tag_dyneither(_tmp22D,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7CD,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1329
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->f1=Cyc_Toc_mt_tq,({void*_tmp7CE=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp22E->f2=_tmp7CE;});_tmp22E;});}
# 1334
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp22F=e->r;void*_tmp230=_tmp22F;struct Cyc_Absyn_Exp*_tmp238;int _tmp237;struct Cyc_Absyn_Exp*_tmp236;int _tmp235;struct Cyc_List_List*_tmp234;switch(*((int*)_tmp230)){case 26U: _LL1: _tmp234=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp230)->f1;_LL2:
# 1337
{struct Cyc_List_List*_tmp231=_tmp234;for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
if((*((struct _tuple19*)_tmp231->hd)).f1 != 0)
({void*_tmp232=0U;({struct _dyneither_ptr _tmp7CF=({const char*_tmp233="array designators for abstract-field initialization";_tag_dyneither(_tmp233,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp7CF,_tag_dyneither(_tmp232,sizeof(void*),0U));});});}}
_tmp236=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp234),0U);_tmp235=0;goto _LL4;case 27U: _LL3: _tmp236=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp230)->f2;_tmp235=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp230)->f4;_LL4:
 _tmp238=_tmp236;_tmp237=_tmp235;goto _LL6;case 28U: _LL5: _tmp238=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp230)->f1;_tmp237=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp230)->f3;_LL6:
# 1343
 _tmp238=Cyc_Absyn_copy_exp(_tmp238);
return _tmp237?({struct Cyc_Absyn_Exp*_tmp7D0=_tmp238;Cyc_Absyn_add_exp(_tmp7D0,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp238;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1352
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1360
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp239=e->r;void*_tmp23A=_tmp239;struct Cyc_List_List*_tmp23B;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23A)->tag == 29U){_LL1: _tmp23B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23A)->f3;_LL2:
 dles=_tmp23B;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1366
{void*_tmp23C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp23D=_tmp23C;struct Cyc_Absyn_Aggrdecl*_tmp23E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23D)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23D)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp23E=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23D)->f1)->f1).KnownAggr).val;_LL7:
# 1368
 if(_tmp23E->impl == 0)
return 0;
if((int)_tmp23E->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1376
 return 0;}_LL5:;}
# 1378
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp23F=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp240=(struct _tuple19*)dles->hd;struct _tuple19*_tmp241=_tmp240;struct Cyc_List_List*_tmp24A;struct Cyc_Absyn_Exp*_tmp249;_LLB: _tmp24A=_tmp241->f1;_tmp249=_tmp241->f2;_LLC:;{
struct _dyneither_ptr*_tmp242=Cyc_Absyn_designatorlist_to_fieldname(_tmp24A);
if(!Cyc_strptrcmp(_tmp242,_tmp23F->name)){
struct Cyc_Absyn_Exp*_tmp243=Cyc_Toc_get_varsizeexp(_tmp249);
if(_tmp243 != 0)
return _tmp243;{
void*_tmp244=Cyc_Tcutil_compress(_tmp23F->type);void*_tmp245=_tmp244;void*_tmp248;struct Cyc_Absyn_Exp*_tmp247;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245)->tag == 4U){_LLE: _tmp248=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245)->f1).elt_type;_tmp247=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245)->f1).num_elts;_LLF:
# 1393
 if(_tmp247 == 0  || !Cyc_Toc_is_zero(_tmp247))
return 0;
# 1399
return({struct Cyc_Absyn_Exp*_tmp7D3=({struct Cyc_Absyn_Exp*_tmp246[2U];_tmp246[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp249)),({
# 1401
struct Cyc_Absyn_Exp*_tmp7D1=Cyc_Absyn_sizeoftype_exp(_tmp248,0U);_tmp246[1]=_tmp7D1;});({struct Cyc_Absyn_Exp*_tmp7D2=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp7D2,_tag_dyneither(_tmp246,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1399
Cyc_Absyn_add_exp(_tmp7D3,
# 1402
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1407
({void*_tmp24B=0U;({struct _dyneither_ptr _tmp7D4=({const char*_tmp24C="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp24C,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7D4,_tag_dyneither(_tmp24B,sizeof(void*),0U));});});};}
# 1410
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp24D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp24D != 0;_tmp24D=_tmp24D->tl){
struct Cyc_Absyn_Aggrfield*_tmp24E=(struct Cyc_Absyn_Aggrfield*)_tmp24D->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp24E->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1417
({void*_tmp24F=0U;({struct _dyneither_ptr _tmp7D6=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp252=({struct Cyc_String_pa_PrintArg_struct _tmp718;_tmp718.tag=0U,_tmp718.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp718;});void*_tmp250[1U];_tmp250[0]=& _tmp252;({struct _dyneither_ptr _tmp7D5=({const char*_tmp251="get_member_offset %s failed";_tag_dyneither(_tmp251,sizeof(char),28U);});Cyc_aprintf(_tmp7D5,_tag_dyneither(_tmp250,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7D6,_tag_dyneither(_tmp24F,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1421
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1424
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1427
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->tag=5U,_tmp253->f1=e1,_tmp253->f2=incr;_tmp253;}),0U);}
# 1431
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1440
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1442
void*_tmp254=e1->r;void*_tmp255=_tmp254;struct Cyc_Absyn_Exp*_tmp25D;struct _dyneither_ptr*_tmp25C;int _tmp25B;int _tmp25A;void*_tmp259;struct Cyc_Absyn_Exp*_tmp258;struct Cyc_Absyn_Stmt*_tmp257;switch(*((int*)_tmp255)){case 37U: _LL1: _tmp257=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp255)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp257,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp259=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp255)->f1;_tmp258=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp255)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp258,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp25D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp255)->f1;_tmp25C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp255)->f2;_tmp25B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp255)->f3;_tmp25A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp255)->f4;_LL6:
# 1447
 e1->r=_tmp25D->r;
({struct Cyc_Absyn_Exp*_tmp7D9=e1;struct Cyc_List_List*_tmp7D8=({struct Cyc_List_List*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->hd=_tmp25C,_tmp256->tl=fs;_tmp256;});struct Cyc_Absyn_Exp*(*_tmp7D7)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp7D9,_tmp7D8,_tmp7D7,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1455
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1457
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp7DA=(f(e1_copy,f_env))->r;e1->r=_tmp7DA;});
goto _LL0;}}_LL0:;}
# 1463
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1465
void*_tmp25E=s->r;void*_tmp25F=_tmp25E;struct Cyc_Absyn_Stmt*_tmp266;struct Cyc_Absyn_Decl*_tmp265;struct Cyc_Absyn_Stmt*_tmp264;struct Cyc_Absyn_Exp*_tmp263;switch(*((int*)_tmp25F)){case 1U: _LL1: _tmp263=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp25F)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp263,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp265=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp25F)->f1;_tmp264=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp25F)->f2;_LL4:
# 1468
 Cyc_Toc_lvalue_assign_stmt(_tmp264,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp266=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp25F)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp266,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp262=({struct Cyc_String_pa_PrintArg_struct _tmp719;_tmp719.tag=0U,({struct _dyneither_ptr _tmp7DB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp719.f1=_tmp7DB;});_tmp719;});void*_tmp260[1U];_tmp260[0]=& _tmp262;({struct _dyneither_ptr _tmp7DC=({const char*_tmp261="lvalue_assign_stmt: %s";_tag_dyneither(_tmp261,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp7DC,_tag_dyneither(_tmp260,sizeof(void*),1U));});});}_LL0:;}
# 1474
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1478
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp267=e->r;void*_tmp268=_tmp267;struct Cyc_Absyn_Stmt*_tmp26F;struct Cyc_Absyn_Exp*_tmp26E;void**_tmp26D;struct Cyc_Absyn_Exp**_tmp26C;switch(*((int*)_tmp268)){case 14U: _LL1: _tmp26D=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp268)->f1;_tmp26C=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp268)->f2;_LL2:
# 1481
({struct Cyc_Absyn_Exp*_tmp7DD=Cyc_Toc_push_address_exp(*_tmp26C);*_tmp26C=_tmp7DD;});
({void*_tmp7DE=Cyc_Absyn_cstar_type(*_tmp26D,Cyc_Toc_mt_tq);*_tmp26D=_tmp7DE;});
return e;case 20U: _LL3: _tmp26E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp268)->f1;_LL4:
# 1485
 return _tmp26E;case 37U: _LL5: _tmp26F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp268)->f1;_LL6:
# 1489
 Cyc_Toc_push_address_stmt(_tmp26F);
return e;default: _LL7: _LL8:
# 1492
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp26B=({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0U,({struct _dyneither_ptr _tmp7DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp71A.f1=_tmp7DF;});_tmp71A;});void*_tmp269[1U];_tmp269[0]=& _tmp26B;({struct _dyneither_ptr _tmp7E0=({const char*_tmp26A="can't take & of exp %s";_tag_dyneither(_tmp26A,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7E0,_tag_dyneither(_tmp269,sizeof(void*),1U));});});}_LL0:;}
# 1496
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp270=s->r;void*_tmp271=_tmp270;struct Cyc_Absyn_Exp**_tmp277;struct Cyc_Absyn_Stmt*_tmp276;struct Cyc_Absyn_Stmt*_tmp275;switch(*((int*)_tmp271)){case 2U: _LL1: _tmp275=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp271)->f2;_LL2:
 _tmp276=_tmp275;goto _LL4;case 12U: _LL3: _tmp276=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp271)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp276);goto _LL0;case 1U: _LL5: _tmp277=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp271)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp7E1=Cyc_Toc_push_address_exp(*_tmp277);*_tmp277=_tmp7E1;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp274=({struct Cyc_String_pa_PrintArg_struct _tmp71B;_tmp71B.tag=0U,({struct _dyneither_ptr _tmp7E2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp71B.f1=_tmp7E2;});_tmp71B;});void*_tmp272[1U];_tmp272[0]=& _tmp274;({struct _dyneither_ptr _tmp7E3=({const char*_tmp273="can't take & of stmt %s";_tag_dyneither(_tmp273,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7E3,_tag_dyneither(_tmp272,sizeof(void*),1U));});});}_LL0:;}
# 1508
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1520
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp278=Cyc_Toc_typ_to_c(elt_type);
void*_tmp279=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp27A=Cyc_Absyn_cstar_type(_tmp278,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp27B=({struct Cyc_Core_Opt*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->v=_tmp27A;_tmp2AD;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp27C=e1->r;void*_tmp27D=_tmp27C;struct Cyc_Absyn_Exp*_tmp283;struct Cyc_Absyn_Exp*_tmp282;struct Cyc_Absyn_Exp*_tmp281;switch(*((int*)_tmp27D)){case 20U: _LL1: _tmp281=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp27D)->f1;_LL2:
# 1528
 if(!is_dyneither){
_tmp281=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp281,0,Cyc_Absyn_Other_coercion,0U);
_tmp281->topt=fat_ptr_type;
_tmp281->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1533
Cyc_Toc_exp_to_c(nv,_tmp281);xinit=_tmp281;goto _LL0;case 23U: _LL3: _tmp283=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp27D)->f1;_tmp282=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp27D)->f2;_LL4:
# 1535
 if(!is_dyneither){
_tmp283=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp283,0,Cyc_Absyn_Other_coercion,0U);
_tmp283->topt=fat_ptr_type;
_tmp283->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1540
Cyc_Toc_exp_to_c(nv,_tmp283);Cyc_Toc_exp_to_c(nv,_tmp282);
xinit=({struct Cyc_Absyn_Exp*_tmp27E[3U];_tmp27E[0]=_tmp283,({
struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp27E[1]=_tmp7E4;}),_tmp27E[2]=_tmp282;({struct Cyc_Absyn_Exp*_tmp7E5=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp7E5,_tag_dyneither(_tmp27E,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;default: _LL5: _LL6:
({void*_tmp27F=0U;({struct _dyneither_ptr _tmp7E6=({const char*_tmp280="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp280,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7E6,_tag_dyneither(_tmp27F,sizeof(void*),0U));});});}_LL0:;}{
# 1546
struct _tuple1*_tmp284=Cyc_Toc_temp_var();
struct _RegionHandle _tmp285=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp285;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp286=({struct Cyc_Absyn_Vardecl*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->sc=Cyc_Absyn_Public,_tmp2AC->name=_tmp284,_tmp2AC->varloc=0U,_tmp2AC->tq=Cyc_Toc_mt_tq,_tmp2AC->type=_tmp279,_tmp2AC->initializer=xinit,_tmp2AC->rgn=0,_tmp2AC->attributes=0,_tmp2AC->escapes=0;_tmp2AC;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp287=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->tag=4U,_tmp2AB->f1=_tmp286;_tmp2AB;});
struct Cyc_Absyn_Exp*_tmp288=Cyc_Absyn_varb_exp((void*)_tmp287,0U);
_tmp288->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp289=Cyc_Absyn_deref_exp(_tmp288,0U);
_tmp289->topt=elt_type;
_tmp289->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,_tmp289);{
struct _tuple1*_tmp28A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp28B=({struct Cyc_Absyn_Vardecl*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->sc=Cyc_Absyn_Public,_tmp2AA->name=_tmp28A,_tmp2AA->varloc=0U,_tmp2AA->tq=Cyc_Toc_mt_tq,_tmp2AA->type=_tmp278,_tmp2AA->initializer=_tmp289,_tmp2AA->rgn=0,_tmp2AA->attributes=0,_tmp2AA->escapes=0;_tmp2AA;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp28C=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->tag=4U,_tmp2A9->f1=_tmp28B;_tmp2A9;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp28D=Cyc_Absyn_varb_exp((void*)_tmp28C,0U);
_tmp28D->topt=_tmp289->topt;
z_init=({enum Cyc_Absyn_Primop _tmp7E8=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp7E7=_tmp28D;Cyc_Absyn_prim2_exp(_tmp7E8,_tmp7E7,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=_tmp28D->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1567
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp28E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp28F=({struct Cyc_Absyn_Vardecl*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->sc=Cyc_Absyn_Public,_tmp2A8->name=_tmp28E,_tmp2A8->varloc=0U,_tmp2A8->tq=Cyc_Toc_mt_tq,_tmp2A8->type=_tmp278,_tmp2A8->initializer=z_init,_tmp2A8->rgn=0,_tmp2A8->attributes=0,_tmp2A8->escapes=0;_tmp2A8;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp290=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->tag=4U,_tmp2A7->f1=_tmp28F;_tmp2A7;});
# 1573
struct Cyc_Absyn_Exp*_tmp291=Cyc_Absyn_varb_exp((void*)_tmp28C,0U);_tmp291->topt=_tmp289->topt;{
struct Cyc_Absyn_Exp*_tmp292=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*_tmp293=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp291,_tmp292,0U);
_tmp292->topt=Cyc_Absyn_sint_type;
_tmp293->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp293);{
# 1580
struct Cyc_Absyn_Exp*_tmp294=Cyc_Absyn_varb_exp((void*)_tmp290,0U);_tmp294->topt=_tmp289->topt;{
struct Cyc_Absyn_Exp*_tmp295=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*_tmp296=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp294,_tmp295,0U);
_tmp295->topt=Cyc_Absyn_sint_type;
_tmp296->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp296);{
# 1587
struct Cyc_List_List*_tmp297=({struct Cyc_Absyn_Exp*_tmp2A6[2U];({struct Cyc_Absyn_Exp*_tmp7EA=Cyc_Absyn_varb_exp((void*)_tmp287,0U);_tmp2A6[0]=_tmp7EA;}),({
struct Cyc_Absyn_Exp*_tmp7E9=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2A6[1]=_tmp7E9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A6,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp298=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp7EB=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp297,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp7EB,_tmp298,0U);});{
# 1594
struct Cyc_Absyn_Exp*_tmp299=({struct Cyc_Absyn_Exp*_tmp7EC=xsize;Cyc_Absyn_and_exp(_tmp7EC,Cyc_Absyn_and_exp(_tmp293,_tmp296,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp29A=Cyc_Absyn_exp_stmt(({void*_tmp2A5=0U;({struct Cyc_Absyn_Exp*_tmp7ED=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp7ED,_tag_dyneither(_tmp2A5,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);
struct Cyc_Absyn_Exp*_tmp29B=({struct Cyc_Absyn_Exp*_tmp7EE=Cyc_Absyn_varb_exp((void*)_tmp287,0U);Cyc_Toc_member_exp(_tmp7EE,Cyc_Toc_curr_sp,0U);});
_tmp29B=Cyc_Toc_cast_it(_tmp27A,_tmp29B);{
struct Cyc_Absyn_Exp*_tmp29C=Cyc_Absyn_deref_exp(_tmp29B,0U);
struct Cyc_Absyn_Exp*_tmp29D=({struct Cyc_Absyn_Exp*_tmp7EF=_tmp29C;Cyc_Absyn_assign_exp(_tmp7EF,Cyc_Absyn_var_exp(_tmp28E,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp29E=Cyc_Absyn_exp_stmt(_tmp29D,0U);
_tmp29E=({struct Cyc_Absyn_Stmt*_tmp7F2=({struct Cyc_Absyn_Exp*_tmp7F1=_tmp299;struct Cyc_Absyn_Stmt*_tmp7F0=_tmp29A;Cyc_Absyn_ifthenelse_stmt(_tmp7F1,_tmp7F0,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp7F2,_tmp29E,0U);});
_tmp29E=({struct Cyc_Absyn_Decl*_tmp7F4=({struct Cyc_Absyn_Decl*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));({void*_tmp7F3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->tag=0U,_tmp29F->f1=_tmp28F;_tmp29F;});_tmp2A0->r=_tmp7F3;}),_tmp2A0->loc=0U;_tmp2A0;});Cyc_Absyn_decl_stmt(_tmp7F4,_tmp29E,0U);});
_tmp29E=({struct Cyc_Absyn_Decl*_tmp7F6=({struct Cyc_Absyn_Decl*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));({void*_tmp7F5=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->tag=0U,_tmp2A1->f1=_tmp28B;_tmp2A1;});_tmp2A2->r=_tmp7F5;}),_tmp2A2->loc=0U;_tmp2A2;});Cyc_Absyn_decl_stmt(_tmp7F6,_tmp29E,0U);});
_tmp29E=({struct Cyc_Absyn_Decl*_tmp7F8=({struct Cyc_Absyn_Decl*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));({void*_tmp7F7=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3->tag=0U,_tmp2A3->f1=_tmp286;_tmp2A3;});_tmp2A4->r=_tmp7F7;}),_tmp2A4->loc=0U;_tmp2A4;});Cyc_Absyn_decl_stmt(_tmp7F8,_tmp29E,0U);});
({void*_tmp7F9=Cyc_Toc_stmt_exp_r(_tmp29E);e->r=_tmp7F9;});};};};};};};};};};}
# 1548
;_pop_region(rgn2);};}
# 1620 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1624
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2AE=Cyc_Tcutil_compress(aggrtype);void*_tmp2AF=_tmp2AE;union Cyc_Absyn_AggrInfo _tmp2B4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AF)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AF)->f1)->tag == 20U){_LL1: _tmp2B4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AF)->f1)->f1;_LL2:
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp2B4);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp2B0=0U;({struct _dyneither_ptr _tmp7FC=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2B3=({struct Cyc_String_pa_PrintArg_struct _tmp71C;_tmp71C.tag=0U,({
struct _dyneither_ptr _tmp7FA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp71C.f1=_tmp7FA;});_tmp71C;});void*_tmp2B1[1U];_tmp2B1[0]=& _tmp2B3;({struct _dyneither_ptr _tmp7FB=({const char*_tmp2B2="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2B2,sizeof(char),51U);});Cyc_aprintf(_tmp7FB,_tag_dyneither(_tmp2B1,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp7FC,_tag_dyneither(_tmp2B0,sizeof(void*),0U));});});}_LL0:;}{
# 1630
struct _tuple29 _tmp2B5=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp2B6=_tmp2B5;struct _tuple1*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;_LL6: _tmp2C7=_tmp2B6.f1;_tmp2C6=_tmp2B6.f2;_LL7:;{
struct Cyc_Absyn_Exp*_tmp2B7=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2B8=Cyc_Absyn_aggrarrow_exp(_tmp2C6,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2B9=Cyc_Absyn_neq_exp(_tmp2B8,_tmp2B7,0U);
struct Cyc_Absyn_Exp*_tmp2BA=Cyc_Absyn_aggrarrow_exp(_tmp2C6,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2BB=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2BA,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2BC=({struct Cyc_Absyn_Exp*_tmp7FE=_tmp2B9;struct Cyc_Absyn_Stmt*_tmp7FD=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp7FE,_tmp7FD,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2BD=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2BF=({struct _tuple1*_tmp801=_tmp2C7;void*_tmp800=_tmp2BD;struct Cyc_Absyn_Exp*_tmp7FF=_tmp2BE;Cyc_Absyn_declare_stmt(_tmp801,_tmp800,_tmp7FF,Cyc_Absyn_seq_stmt(_tmp2BC,_tmp2BB,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2BF);}else{
# 1643
struct Cyc_Absyn_Exp*_tmp2C0=({struct Cyc_Absyn_Exp*_tmp802=aggrproj(_tmp2C6,f,0U);Cyc_Toc_member_exp(_tmp802,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2C1=Cyc_Absyn_neq_exp(_tmp2C0,_tmp2B7,0U);
struct Cyc_Absyn_Exp*_tmp2C2=({struct Cyc_Absyn_Exp*_tmp803=aggrproj(_tmp2C6,f,0U);Cyc_Toc_member_exp(_tmp803,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2C3=Cyc_Absyn_exp_stmt(_tmp2C2,0U);
struct Cyc_Absyn_Stmt*_tmp2C4=({struct Cyc_Absyn_Exp*_tmp805=_tmp2C1;struct Cyc_Absyn_Stmt*_tmp804=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp805,_tmp804,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2C5=({struct _tuple1*_tmp808=_tmp2C7;void*_tmp807=e1_c_type;struct Cyc_Absyn_Exp*_tmp806=e1;Cyc_Absyn_declare_stmt(_tmp808,_tmp807,_tmp806,Cyc_Absyn_seq_stmt(_tmp2C4,_tmp2C3,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2C5);}};};}
# 1653
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1656
void*_tmp2C8=Cyc_Tcutil_compress(t);void*_tmp2C9=_tmp2C8;union Cyc_Absyn_AggrInfo _tmp2D0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C9)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C9)->f1)->tag == 20U){_LL1: _tmp2D0=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C9)->f1)->f1;_LL2: {
# 1658
struct Cyc_Absyn_Aggrdecl*_tmp2CA=Cyc_Absyn_get_known_aggrdecl(_tmp2D0);
({int _tmp809=Cyc_Toc_get_member_offset(_tmp2CA,f);*f_tag=_tmp809;});{
# 1661
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2CE=({struct Cyc_String_pa_PrintArg_struct _tmp71E;_tmp71E.tag=0U,_tmp71E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2CA->name).f2);_tmp71E;});struct Cyc_String_pa_PrintArg_struct _tmp2CF=({struct Cyc_String_pa_PrintArg_struct _tmp71D;_tmp71D.tag=0U,_tmp71D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp71D;});void*_tmp2CC[2U];_tmp2CC[0]=& _tmp2CE,_tmp2CC[1]=& _tmp2CF;({struct _dyneither_ptr _tmp80A=({const char*_tmp2CD="_union_%s_%s";_tag_dyneither(_tmp2CD,sizeof(char),13U);});Cyc_aprintf(_tmp80A,_tag_dyneither(_tmp2CC,sizeof(void*),2U));});});
({void*_tmp80B=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));*_tmp2CB=str;_tmp2CB;}));*tagged_member_type=_tmp80B;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2CA->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1672
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1675
void*_tmp2D1=e->r;void*_tmp2D2=_tmp2D1;struct Cyc_Absyn_Exp*_tmp2DE;struct _dyneither_ptr*_tmp2DD;int*_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DB;struct _dyneither_ptr*_tmp2DA;int*_tmp2D9;struct Cyc_Absyn_Exp*_tmp2D8;switch(*((int*)_tmp2D2)){case 14U: _LL1: _tmp2D8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D2)->f2;_LL2:
({void*_tmp2D3=0U;({struct _dyneither_ptr _tmp80C=({const char*_tmp2D4="cast on lhs!";_tag_dyneither(_tmp2D4,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp80C,_tag_dyneither(_tmp2D3,sizeof(void*),0U));});});case 21U: _LL3: _tmp2DB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D2)->f1;_tmp2DA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D2)->f2;_tmp2D9=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D2)->f4;_LL4:
# 1678
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp2DB->topt),_tmp2DA,f_tag,tagged_member_type,clear_read,_tmp2D9);case 22U: _LL5: _tmp2DE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D2)->f1;_tmp2DD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D2)->f2;_tmp2DC=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D2)->f4;_LL6: {
# 1681
void*_tmp2D5=Cyc_Tcutil_compress((void*)_check_null(_tmp2DE->topt));void*_tmp2D6=_tmp2D5;void*_tmp2D7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D6)->tag == 3U){_LLA: _tmp2D7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D6)->f1).elt_type;_LLB:
# 1683
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2D7,_tmp2DD,f_tag,tagged_member_type,clear_read,_tmp2DC);}else{_LLC: _LLD:
# 1685
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1687
 return 0;}_LL0:;}
# 1700 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1704
struct _tuple29 _tmp2DF=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp2E0=_tmp2DF;struct _tuple1*_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E2;_LL1: _tmp2E3=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;_LL2:;{
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(_tmp2E2,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(_tmp2E2,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1713
struct Cyc_Absyn_Exp*_tmp2E1=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
s2=({struct Cyc_Absyn_Exp*_tmp80E=_tmp2E1;struct Cyc_Absyn_Stmt*_tmp80D=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp80E,_tmp80D,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1716
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp811=_tmp2E3;void*_tmp810=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp80F=
Cyc_Toc_push_address_exp(e1);
# 1716
Cyc_Absyn_declare_stmt(_tmp811,_tmp810,_tmp80F,
# 1718
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};};}struct _tuple32{void*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1722
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp2E6=({struct Cyc_String_pa_PrintArg_struct _tmp71F;_tmp71F.tag=0U,({struct _dyneither_ptr _tmp812=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp71F.f1=_tmp812;});_tmp71F;});void*_tmp2E4[1U];_tmp2E4[0]=& _tmp2E6;({unsigned int _tmp814=e->loc;struct _dyneither_ptr _tmp813=({const char*_tmp2E5="exp_to_c: no type for %s";_tag_dyneither(_tmp2E5,sizeof(char),25U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp814,_tmp813,_tag_dyneither(_tmp2E4,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1727
int did_inserted_checks=0;
{void*_tmp2E7=e->annot;void*_tmp2E8=_tmp2E7;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2E8)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2E8)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1733
{void*_tmp2E9=e->r;void*_tmp2EA=_tmp2E9;struct Cyc_Absyn_Stmt*_tmp490;struct Cyc_Absyn_Exp*_tmp48F;struct _dyneither_ptr*_tmp48E;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48C;int _tmp48B;struct Cyc_Absyn_Exp*_tmp48A;void**_tmp489;struct Cyc_Absyn_Exp*_tmp488;int _tmp487;int _tmp486;struct Cyc_List_List*_tmp485;struct Cyc_Absyn_Datatypedecl*_tmp484;struct Cyc_Absyn_Datatypefield*_tmp483;struct _tuple1**_tmp482;struct Cyc_List_List*_tmp481;struct Cyc_List_List*_tmp480;struct Cyc_Absyn_Aggrdecl*_tmp47F;void*_tmp47E;struct Cyc_List_List*_tmp47D;struct Cyc_Absyn_Exp*_tmp47C;void*_tmp47B;int _tmp47A;struct Cyc_Absyn_Vardecl*_tmp479;struct Cyc_Absyn_Exp*_tmp478;struct Cyc_Absyn_Exp*_tmp477;int _tmp476;struct Cyc_List_List*_tmp475;struct Cyc_List_List*_tmp474;struct Cyc_Absyn_Exp*_tmp473;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*_tmp471;struct Cyc_Absyn_Exp*_tmp470;struct _dyneither_ptr*_tmp46F;int _tmp46E;int _tmp46D;struct Cyc_Absyn_Exp*_tmp46C;struct _dyneither_ptr*_tmp46B;int _tmp46A;int _tmp469;void*_tmp468;struct Cyc_List_List*_tmp467;void*_tmp466;struct Cyc_Absyn_Exp*_tmp465;struct Cyc_Absyn_Exp*_tmp464;struct Cyc_Absyn_Exp*_tmp463;struct Cyc_Absyn_Exp*_tmp462;void**_tmp461;struct Cyc_Absyn_Exp*_tmp460;int _tmp45F;enum Cyc_Absyn_Coercion _tmp45E;struct Cyc_Absyn_Exp*_tmp45D;struct Cyc_List_List*_tmp45C;struct Cyc_Absyn_Exp*_tmp45B;struct Cyc_Absyn_Exp*_tmp45A;int _tmp459;struct Cyc_Absyn_Exp*_tmp458;struct Cyc_List_List*_tmp457;int _tmp456;struct Cyc_List_List*_tmp455;struct Cyc_Absyn_VarargInfo*_tmp454;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Exp*_tmp44C;struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_Absyn_Exp*_tmp44A;struct Cyc_Absyn_Exp*_tmp449;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Core_Opt*_tmp447;struct Cyc_Absyn_Exp*_tmp446;struct Cyc_Absyn_Exp*_tmp445;enum Cyc_Absyn_Incrementor _tmp444;enum Cyc_Absyn_Primop _tmp443;struct Cyc_List_List*_tmp442;struct Cyc_Absyn_Exp*_tmp441;switch(*((int*)_tmp2EA)){case 2U: _LL8: _LL9:
# 1735
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1742
struct Cyc_Absyn_Exp*_tmp2EB=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp815=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp2EB,_tmp2EB))->r;e->r=_tmp815;});else{
# 1747
({void*_tmp817=({struct Cyc_Absyn_Exp*_tmp2EC[3U];_tmp2EC[0]=_tmp2EB,_tmp2EC[1]=_tmp2EB,_tmp2EC[2]=_tmp2EB;({struct Cyc_Absyn_Exp*_tmp816=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp816,_tag_dyneither(_tmp2EC,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp817;});}}else{
# 1749
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1751
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 41U: _LL10: _tmp441=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_LL11:
 Cyc_Toc_exp_to_c(nv,_tmp441);goto _LL7;case 3U: _LL12: _tmp443=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp442=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL13: {
# 1757
struct Cyc_List_List*_tmp2ED=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp442);
# 1759
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp442);
{enum Cyc_Absyn_Primop _tmp2EE=_tmp443;switch(_tmp2EE){case Cyc_Absyn_Numelts: _LL61: _LL62: {
# 1762
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp442))->hd;
{void*_tmp2EF=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp2F0=_tmp2EF;void*_tmp2FD;void*_tmp2FC;void*_tmp2FB;void*_tmp2FA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->tag == 3U){_LL76: _tmp2FD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).elt_type;_tmp2FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).ptr_atts).nullable;_tmp2FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).ptr_atts).bounds;_tmp2FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).ptr_atts).zero_term;_LL77: {
# 1765
struct Cyc_Absyn_Exp*_tmp2F1=({void*_tmp818=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp818,_tmp2FB);});
if(_tmp2F1 == 0)
# 1768
({void*_tmp81B=({struct Cyc_Absyn_Exp*_tmp2F2[2U];_tmp2F2[0]=(struct Cyc_Absyn_Exp*)_tmp442->hd,({
struct Cyc_Absyn_Exp*_tmp819=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp2FD),0U);_tmp2F2[1]=_tmp819;});({struct Cyc_Absyn_Exp*_tmp81A=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp81A,_tag_dyneither(_tmp2F2,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1768
e->r=_tmp81B;});else{
# 1771
struct Cyc_Absyn_Exp*_tmp2F3=_tmp2F1;
# 1773
if(Cyc_Tcutil_force_type2bool(0,_tmp2FA)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp442->hd);
# 1777
({void*_tmp81D=({struct Cyc_Absyn_Exp*_tmp2F4[2U];_tmp2F4[0]=(struct Cyc_Absyn_Exp*)_tmp442->hd,_tmp2F4[1]=_tmp2F3;({struct Cyc_Absyn_Exp*_tmp81C=function_e;Cyc_Toc_fncall_exp_r(_tmp81C,_tag_dyneither(_tmp2F4,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp81D;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp2FC)){
if(!Cyc_Evexp_c_can_eval(_tmp2F3))
({void*_tmp2F5=0U;({unsigned int _tmp81F=e->loc;struct _dyneither_ptr _tmp81E=({const char*_tmp2F6="can't calculate numelts";_tag_dyneither(_tmp2F6,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp81F,_tmp81E,_tag_dyneither(_tmp2F5,sizeof(void*),0U));});});
# 1782
({void*_tmp822=({struct Cyc_Absyn_Exp*_tmp821=arg;struct Cyc_Absyn_Exp*_tmp820=_tmp2F3;Cyc_Toc_conditional_exp_r(_tmp821,_tmp820,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp822;});}else{
# 1784
e->r=_tmp2F3->r;goto _LL75;}}}
# 1787
goto _LL75;}}else{_LL78: _LL79:
({struct Cyc_String_pa_PrintArg_struct _tmp2F9=({struct Cyc_String_pa_PrintArg_struct _tmp720;_tmp720.tag=0U,({
struct _dyneither_ptr _tmp823=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp720.f1=_tmp823;});_tmp720;});void*_tmp2F7[1U];_tmp2F7[0]=& _tmp2F9;({struct _dyneither_ptr _tmp824=({const char*_tmp2F8="numelts primop applied to non-pointer %s";_tag_dyneither(_tmp2F8,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp824,_tag_dyneither(_tmp2F7,sizeof(void*),1U));});});}_LL75:;}
# 1791
goto _LL60;}case Cyc_Absyn_Plus: _LL63: _LL64:
# 1796
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp2FE=0U;({struct _dyneither_ptr _tmp825=({const char*_tmp2FF="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp2FF,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp825,_tag_dyneither(_tmp2FE,sizeof(void*),0U));});});
{void*_tmp300=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp2ED))->hd);void*_tmp301=_tmp300;void*_tmp307;void*_tmp306;void*_tmp305;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->tag == 3U){_LL7B: _tmp307=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).elt_type;_tmp306=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).bounds;_tmp305=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).zero_term;_LL7C: {
# 1800
struct Cyc_Absyn_Exp*_tmp302=({void*_tmp826=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp826,_tmp306);});
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp442))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp442->tl))->hd;
if(_tmp302 == 0)
({void*_tmp829=({struct Cyc_Absyn_Exp*_tmp303[3U];_tmp303[0]=e1,({
struct Cyc_Absyn_Exp*_tmp827=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp307),0U);_tmp303[1]=_tmp827;}),_tmp303[2]=e2;({struct Cyc_Absyn_Exp*_tmp828=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp828,_tag_dyneither(_tmp303,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1804
e->r=_tmp829;});else{
# 1806
if(Cyc_Tcutil_force_type2bool(0,_tmp305))
({void*_tmp82B=({struct Cyc_Absyn_Exp*_tmp304[3U];_tmp304[0]=e1,_tmp304[1]=_tmp302,_tmp304[2]=e2;({struct Cyc_Absyn_Exp*_tmp82A=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp82A,_tag_dyneither(_tmp304,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp82B;});}
goto _LL7A;}}else{_LL7D: _LL7E:
 goto _LL7A;}_LL7A:;}
# 1811
goto _LL60;case Cyc_Absyn_Minus: _LL65: _LL66: {
# 1816
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp2ED))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp442))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp442->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp2ED->tl))->hd)){
({void*_tmp82D=({struct Cyc_Absyn_Exp*_tmp82C=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp82C,Cyc_Toc_curr_sp);});e1->r=_tmp82D;});
({void*_tmp82F=({struct Cyc_Absyn_Exp*_tmp82E=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp82E,Cyc_Toc_curr_sp);});e2->r=_tmp82F;});
({void*_tmp831=({void*_tmp830=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp830;});e1->topt=_tmp831;});
({void*_tmp833=({struct Cyc_Absyn_Exp*_tmp832=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp832,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1824
e->r=_tmp833;});}else{
# 1827
({void*_tmp837=({struct Cyc_Absyn_Exp*_tmp308[3U];_tmp308[0]=e1,({
struct Cyc_Absyn_Exp*_tmp835=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp308[1]=_tmp835;}),({
struct Cyc_Absyn_Exp*_tmp834=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp308[2]=_tmp834;});({struct Cyc_Absyn_Exp*_tmp836=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp836,_tag_dyneither(_tmp308,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1827
e->r=_tmp837;});}}
# 1831
goto _LL60;}case Cyc_Absyn_Eq: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Neq: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Gt: _LL6B: _LL6C:
 goto _LL6E;case Cyc_Absyn_Gte: _LL6D: _LL6E:
 goto _LL70;case Cyc_Absyn_Lt: _LL6F: _LL70:
 goto _LL72;case Cyc_Absyn_Lte: _LL71: _LL72: {
# 1839
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp442))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp442->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp2ED))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp2ED->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& elt_type)){
void*_tmp309=({void*_tmp838=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp838,Cyc_Toc_mt_tq);});
({void*_tmp83B=({void*_tmp83A=_tmp309;Cyc_Toc_cast_it_r(_tmp83A,({struct Cyc_Absyn_Exp*_tmp839=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp839,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp83B;});
e1->topt=_tmp309;}
# 1849
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
void*_tmp30A=({void*_tmp83C=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp83C,Cyc_Toc_mt_tq);});
({void*_tmp83F=({void*_tmp83E=_tmp30A;Cyc_Toc_cast_it_r(_tmp83E,({struct Cyc_Absyn_Exp*_tmp83D=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp83D,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp83F;});
e2->topt=_tmp30A;}
# 1854
goto _LL60;}default: _LL73: _LL74:
 goto _LL60;}_LL60:;}
# 1857
goto _LL7;}case 5U: _LL14: _tmp445=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp444=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL15: {
# 1859
void*e2_cyc_typ=(void*)_check_null(_tmp445->topt);
# 1868 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _dyneither_ptr incr_str=({const char*_tmp31E="increment";_tag_dyneither(_tmp31E,sizeof(char),10U);});
if((int)_tmp444 == (int)2U  || (int)_tmp444 == (int)3U)incr_str=({const char*_tmp30B="decrement";_tag_dyneither(_tmp30B,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp445,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp30E=({struct Cyc_String_pa_PrintArg_struct _tmp721;_tmp721.tag=0U,_tmp721.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp721;});void*_tmp30C[1U];_tmp30C[0]=& _tmp30E;({unsigned int _tmp841=e->loc;struct _dyneither_ptr _tmp840=({const char*_tmp30D="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp30D,sizeof(char),74U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp841,_tmp840,_tag_dyneither(_tmp30C,sizeof(void*),1U));});});
# 1876
if(Cyc_Toc_is_tagged_union_project(_tmp445,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp30F=_tmp444;switch(_tmp30F){case Cyc_Absyn_PreInc: _LL80: _LL81:
 op=0U;goto _LL7F;case Cyc_Absyn_PreDec: _LL82: _LL83:
 op=2U;goto _LL7F;default: _LL84: _LL85:
({struct Cyc_String_pa_PrintArg_struct _tmp312=({struct Cyc_String_pa_PrintArg_struct _tmp722;_tmp722.tag=0U,_tmp722.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp722;});void*_tmp310[1U];_tmp310[0]=& _tmp312;({unsigned int _tmp843=e->loc;struct _dyneither_ptr _tmp842=({const char*_tmp311="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp311,sizeof(char),58U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp843,_tmp842,_tag_dyneither(_tmp310,sizeof(void*),1U));});});}_LL7F:;}
# 1886
({void*_tmp845=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->tag=4U,_tmp314->f1=_tmp445,({struct Cyc_Core_Opt*_tmp844=({struct Cyc_Core_Opt*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->v=(void*)op;_tmp313;});_tmp314->f2=_tmp844;}),_tmp314->f3=one;_tmp314;});e->r=_tmp845;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1890
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp445);
Cyc_Toc_set_lhs(nv,0);{
# 1895
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=((int)_tmp444 == (int)1U  || (int)_tmp444 == (int)3U)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 1901
if((int)_tmp444 == (int)2U  || (int)_tmp444 == (int)3U)
change=-1;
({void*_tmp84A=({struct Cyc_Absyn_Exp*_tmp315[3U];({struct Cyc_Absyn_Exp*_tmp848=Cyc_Toc_push_address_exp(_tmp445);_tmp315[0]=_tmp848;}),({
struct Cyc_Absyn_Exp*_tmp847=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp315[1]=_tmp847;}),({
struct Cyc_Absyn_Exp*_tmp846=Cyc_Absyn_signed_int_exp(change,0U);_tmp315[2]=_tmp846;});({struct Cyc_Absyn_Exp*_tmp849=fn_e;Cyc_Toc_fncall_exp_r(_tmp849,_tag_dyneither(_tmp315,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1903
e->r=_tmp84A;});}else{
# 1906
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1912
did_inserted_checks=1;
if((int)_tmp444 != (int)1U){
struct _tuple1*_tmp316=Cyc_Toc_temp_var();
void*_tmp317=({void*_tmp84B=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp84B,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp318=Cyc_Toc_push_address_exp(_tmp445);
struct Cyc_Absyn_Exp*_tmp319=({struct Cyc_Absyn_Exp*_tmp84C=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp316,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp84C,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
_tmp319=({struct Cyc_Absyn_Exp*_tmp84E=({struct Cyc_Absyn_Exp*_tmp84D=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp316,0U),0U);Cyc_Absyn_neq_exp(_tmp84D,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1920
Cyc_Absyn_and_exp(_tmp84E,_tmp319,0U);});{
# 1922
struct Cyc_Absyn_Stmt*_tmp31A=({struct Cyc_Absyn_Exp*_tmp851=_tmp319;struct Cyc_Absyn_Stmt*_tmp850=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp316,0U),0U),0U),0U);
# 1922
Cyc_Absyn_ifthenelse_stmt(_tmp851,_tmp850,
# 1924
Cyc_Absyn_exp_stmt(({void*_tmp31B=0U;({struct Cyc_Absyn_Exp*_tmp84F=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp84F,_tag_dyneither(_tmp31B,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
_tmp31A=({struct Cyc_Absyn_Stmt*_tmp852=_tmp31A;Cyc_Absyn_seq_stmt(_tmp852,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp316,0U),0U),0U),0U);});
({void*_tmp853=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp316,_tmp317,_tmp318,_tmp31A,0U),0U))->r;e->r=_tmp853;});};}else{
# 1929
struct Cyc_Toc_functionSet*_tmp31C=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp31C,_tmp445);
({void*_tmp857=({struct Cyc_Absyn_Exp*_tmp31D[2U];({struct Cyc_Absyn_Exp*_tmp855=Cyc_Toc_push_address_exp(_tmp445);_tmp31D[0]=_tmp855;}),({struct Cyc_Absyn_Exp*_tmp854=Cyc_Absyn_signed_int_exp(1,0U);_tmp31D[1]=_tmp854;});({struct Cyc_Absyn_Exp*_tmp856=fn_e;Cyc_Toc_fncall_exp_r(_tmp856,_tag_dyneither(_tmp31D,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp857;});}}else{
# 1933
if(elt_typ == Cyc_Absyn_void_type  && !Cyc_Absyn_is_lvalue(_tmp445)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp445,0,Cyc_Toc_incr_lvalue,_tmp444);
e->r=_tmp445->r;}}}
# 1937
goto _LL7;};}case 4U: _LL16: _tmp448=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp447=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp446=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3;_LL17: {
# 1956 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp448->topt);
void*e2_old_typ=(void*)_check_null(_tmp446->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp448,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp448);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp446);
({void*_tmp858=Cyc_Toc_tagged_union_assignop(_tmp448,e1_old_typ,_tmp447,_tmp446,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp858;});
# 1967
goto _LL7;}{
# 1969
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp448,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp448,_tmp447,_tmp446,ptr_type,is_dyneither,elt_type);
# 1975
return;}{
# 1979
int e1_poly=Cyc_Toc_is_poly_project(_tmp448);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp448);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp446);{
# 1985
int done=0;
if(_tmp447 != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp31F=(enum Cyc_Absyn_Primop)_tmp447->v;enum Cyc_Absyn_Primop _tmp320=_tmp31F;switch(_tmp320){case Cyc_Absyn_Plus: _LL87: _LL88:
 change=_tmp446;goto _LL86;case Cyc_Absyn_Minus: _LL89: _LL8A:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp446,0U);goto _LL86;default: _LL8B: _LL8C:
({void*_tmp321=0U;({struct _dyneither_ptr _tmp859=({const char*_tmp322="bad t ? pointer arithmetic";_tag_dyneither(_tmp322,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp859,_tag_dyneither(_tmp321,sizeof(void*),0U));});});}_LL86:;}
# 1995
done=1;{
# 1997
struct Cyc_Absyn_Exp*_tmp323=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp85D=({struct Cyc_Absyn_Exp*_tmp324[3U];({struct Cyc_Absyn_Exp*_tmp85B=Cyc_Toc_push_address_exp(_tmp448);_tmp324[0]=_tmp85B;}),({
struct Cyc_Absyn_Exp*_tmp85A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp324[1]=_tmp85A;}),_tmp324[2]=change;({struct Cyc_Absyn_Exp*_tmp85C=_tmp323;Cyc_Toc_fncall_exp_r(_tmp85C,_tag_dyneither(_tmp324,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1998
e->r=_tmp85D;});};}else{
# 2001
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2004
enum Cyc_Absyn_Primop _tmp325=(enum Cyc_Absyn_Primop)_tmp447->v;enum Cyc_Absyn_Primop _tmp326=_tmp325;if(_tmp326 == Cyc_Absyn_Plus){_LL8E: _LL8F:
# 2006
 done=1;
({void*_tmp85F=({struct Cyc_Absyn_Exp*_tmp327[2U];_tmp327[0]=_tmp448,_tmp327[1]=_tmp446;({struct Cyc_Absyn_Exp*_tmp85E=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp448);Cyc_Toc_fncall_exp_r(_tmp85E,_tag_dyneither(_tmp327,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp85F;});
goto _LL8D;}else{_LL90: _LL91:
({void*_tmp328=0U;({struct _dyneither_ptr _tmp860=({const char*_tmp329="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp329,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp860,_tag_dyneither(_tmp328,sizeof(void*),0U));});});}_LL8D:;}}}
# 2013
if(!done){
# 2015
if(e1_poly)
({void*_tmp862=({void*_tmp861=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp861,Cyc_Absyn_new_exp(_tmp446->r,0U));});_tmp446->r=_tmp862;});
# 2021
if(!Cyc_Absyn_is_lvalue(_tmp448)){
({struct Cyc_Absyn_Exp*_tmp863=_tmp448;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp863,0,Cyc_Toc_assignop_lvalue,({struct _tuple31*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->f1=_tmp447,_tmp32A->f2=_tmp446;_tmp32A;}));});
e->r=_tmp448->r;}}
# 2026
goto _LL7;};};};}case 6U: _LL18: _tmp44B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp44A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp449=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3;_LL19:
# 2028
 Cyc_Toc_exp_to_c(nv,_tmp44B);
Cyc_Toc_exp_to_c(nv,_tmp44A);
Cyc_Toc_exp_to_c(nv,_tmp449);
goto _LL7;case 7U: _LL1A: _tmp44D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp44C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL1B:
 _tmp44F=_tmp44D;_tmp44E=_tmp44C;goto _LL1D;case 8U: _LL1C: _tmp44F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp44E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL1D:
 _tmp451=_tmp44F;_tmp450=_tmp44E;goto _LL1F;case 9U: _LL1E: _tmp451=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp450=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL1F:
# 2035
 Cyc_Toc_exp_to_c(nv,_tmp451);
Cyc_Toc_exp_to_c(nv,_tmp450);
goto _LL7;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3 == 0){_LL20: _tmp453=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp452=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL21: {
# 2039
void*e1_typ=(void*)_check_null(_tmp453->topt);
Cyc_Toc_exp_to_c(nv,_tmp453);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2044
({void*_tmp867=({void*_tmp866=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp866,({struct Cyc_Absyn_Exp*_tmp32B[1U];({struct Cyc_Absyn_Exp*_tmp864=
Cyc_Absyn_copy_exp(_tmp453);_tmp32B[0]=_tmp864;});({struct Cyc_Absyn_Exp*_tmp865=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp865,_tag_dyneither(_tmp32B,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2044
_tmp453->r=_tmp867;});
# 2046
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp452);
goto _LL7;}}else{_LL22: _tmp458=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp457=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp456=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3)->num_varargs;_tmp455=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3)->injectors;_tmp454=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3)->vai;_LL23: {
# 2057 "toc.cyc"
struct _RegionHandle _tmp32C=_new_region("r");struct _RegionHandle*r=& _tmp32C;_push_region(r);{
struct _tuple29 _tmp32D=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp32E=_tmp32D;struct _tuple1*_tmp347;struct Cyc_Absyn_Exp*_tmp346;_LL93: _tmp347=_tmp32E.f1;_tmp346=_tmp32E.f2;_LL94:;{
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp456,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp454->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2064
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp457);
int num_normargs=num_args - _tmp456;
# 2068
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp457=_tmp457->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp457))->hd);
new_args=({struct Cyc_List_List*_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F->hd=(struct Cyc_Absyn_Exp*)_tmp457->hd,_tmp32F->tl=new_args;_tmp32F;});}}
# 2073
new_args=({struct Cyc_List_List*_tmp331=_cycalloc(sizeof(*_tmp331));({struct Cyc_Absyn_Exp*_tmp86A=({struct Cyc_Absyn_Exp*_tmp330[3U];_tmp330[0]=_tmp346,({
# 2075
struct Cyc_Absyn_Exp*_tmp868=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp330[1]=_tmp868;}),_tmp330[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp869=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp869,_tag_dyneither(_tmp330,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2073
_tmp331->hd=_tmp86A;}),_tmp331->tl=new_args;_tmp331;});
# 2078
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2080
void*e1_typ=(void*)_check_null(_tmp458->topt);
Cyc_Toc_exp_to_c(nv,_tmp458);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2085
({void*_tmp86E=({void*_tmp86D=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp86D,({struct Cyc_Absyn_Exp*_tmp332[1U];({struct Cyc_Absyn_Exp*_tmp86B=
Cyc_Absyn_copy_exp(_tmp458);_tmp332[0]=_tmp86B;});({struct Cyc_Absyn_Exp*_tmp86C=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp86C,_tag_dyneither(_tmp332,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2085
_tmp458->r=_tmp86E;});{
# 2087
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp458,new_args,0U),0U);
# 2090
if(_tmp454->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp333=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp454->type));void*_tmp334=_tmp333;struct Cyc_Absyn_Datatypedecl*_tmp337;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp334)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp334)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp334)->f1)->f1).KnownDatatype).tag == 2){_LL96: _tmp337=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp334)->f1)->f1).KnownDatatype).val;_LL97:
 tud=_tmp337;goto _LL95;}else{goto _LL98;}}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp335=0U;({struct _dyneither_ptr _tmp86F=({const char*_tmp336="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp336,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp86F,_tag_dyneither(_tmp335,sizeof(void*),0U));});});}_LL95:;}{
# 2096
struct _dyneither_ptr vs=({unsigned int _tmp343=(unsigned int)_tmp456;struct _tuple1**_tmp342=({struct _RegionHandle*_tmp870=r;_region_malloc(_tmp870,_check_times(_tmp343,sizeof(struct _tuple1*)));});({{unsigned int _tmp723=(unsigned int)_tmp456;unsigned int i;for(i=0;i < _tmp723;++ i){({struct _tuple1*_tmp871=Cyc_Toc_temp_var();_tmp342[i]=_tmp871;});}}0;});_tag_dyneither(_tmp342,sizeof(struct _tuple1*),_tmp343);});
# 2098
if(_tmp456 != 0){
# 2102
struct Cyc_List_List*_tmp338=0;
{int i=_tmp456 - 1;for(0;i >= 0;-- i){
_tmp338=({struct Cyc_List_List*_tmp339=_cycalloc(sizeof(*_tmp339));({struct Cyc_Absyn_Exp*_tmp872=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp339->hd=_tmp872;}),_tmp339->tl=_tmp338;_tmp339;});}}
s=({struct _tuple1*_tmp875=_tmp347;void*_tmp874=arr_type;struct Cyc_Absyn_Exp*_tmp873=Cyc_Absyn_array_exp(_tmp338,0U);Cyc_Absyn_declare_stmt(_tmp875,_tmp874,_tmp873,s,0U);});
# 2107
_tmp457=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp457);
_tmp455=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp455);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp457)- 1;
for(0;_tmp457 != 0;(_tmp457=_tmp457->tl,_tmp455=_tmp455->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp457->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp33A=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp455))->hd;struct Cyc_Absyn_Datatypefield*_tmp33B=_tmp33A;struct _tuple1*_tmp341;struct Cyc_List_List*_tmp340;_LL9B: _tmp341=_tmp33B->name;_tmp340=_tmp33B->typs;_LL9C:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp340))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2121
struct _tuple1*_tmp33C=Cyc_Toc_collapse_qvars(_tmp341,tud->name);
struct Cyc_List_List*_tmp33D=({struct _tuple19*_tmp33F[2U];({struct _tuple19*_tmp879=({struct _dyneither_ptr*_tmp878=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp878,Cyc_Toc_datatype_tag(tud,_tmp341));});_tmp33F[0]=_tmp879;}),({
struct _tuple19*_tmp877=({struct _dyneither_ptr*_tmp876=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp876,arg);});_tmp33F[1]=_tmp877;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp33F,sizeof(struct _tuple19*),2U));});
s=({struct _tuple1*_tmp87C=var;void*_tmp87B=Cyc_Absyn_strctq(_tmp33C);struct Cyc_Absyn_Exp*_tmp87A=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E->tag=29U,_tmp33E->f1=_tmp33C,_tmp33E->f2=0,_tmp33E->f3=_tmp33D,_tmp33E->f4=0;_tmp33E;}),0U);
# 2124
Cyc_Absyn_declare_stmt(_tmp87C,_tmp87B,_tmp87A,s,0U);});};};}};}else{
# 2132
s=({struct _tuple1*_tmp87F=_tmp347;void*_tmp87E=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp87D=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp87F,_tmp87E,_tmp87D,s,0U);});}};}else{
# 2137
if(_tmp456 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp457 != 0;_tmp457=_tmp457->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp457->hd);
array_args=({struct Cyc_List_List*_tmp344=_cycalloc(sizeof(*_tmp344));_tmp344->hd=(struct Cyc_Absyn_Exp*)_tmp457->hd,_tmp344->tl=array_args;_tmp344;});}{
# 2143
struct Cyc_Absyn_Exp*_tmp345=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
s=Cyc_Absyn_declare_stmt(_tmp347,arr_type,_tmp345,s,0U);};}else{
# 2146
s=({struct _tuple1*_tmp882=_tmp347;void*_tmp881=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp880=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp882,_tmp881,_tmp880,s,0U);});}}
# 2149
({void*_tmp883=Cyc_Toc_stmt_exp_r(s);e->r=_tmp883;});};};};}
# 2151
_npop_handler(0U);goto _LL7;
# 2057
;_pop_region(r);}}case 11U: _LL24: _tmp45A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp459=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL25:
# 2154
 Cyc_Toc_exp_to_c(nv,_tmp45A);{
struct Cyc_Absyn_Exp*fn_e=_tmp459?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp886=({void*_tmp885=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp885,({struct Cyc_Absyn_Exp*_tmp348[1U];_tmp348[0]=_tmp45A;({struct Cyc_Absyn_Exp*_tmp884=fn_e;Cyc_Toc_fncall_exp_dl(_tmp884,_tag_dyneither(_tmp348,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp886;});
goto _LL7;};case 12U: _LL26: _tmp45B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_LL27:
 Cyc_Toc_exp_to_c(nv,_tmp45B);goto _LL7;case 13U: _LL28: _tmp45D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp45C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL29:
# 2160
 Cyc_Toc_exp_to_c(nv,_tmp45D);
# 2169 "toc.cyc"
for(0;_tmp45C != 0;_tmp45C=_tmp45C->tl){
enum Cyc_Absyn_KindQual _tmp349=(Cyc_Tcutil_type_kind((void*)_tmp45C->hd))->kind;
if((int)_tmp349 != (int)4U  && (int)_tmp349 != (int)3U){
{void*_tmp34A=Cyc_Tcutil_compress((void*)_tmp45C->hd);void*_tmp34B=_tmp34A;switch(*((int*)_tmp34B)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34B)->f1)->tag == 18U){_LLA0: _LLA1:
 continue;}else{goto _LLA2;}default: _LLA2: _LLA3:
# 2176
({void*_tmp888=({void*_tmp887=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp887,_tmp45D,0U);})->r;e->r=_tmp888;});
goto _LL9D;}_LL9D:;}
# 2179
break;}}
# 2182
goto _LL7;case 14U: _LL2A: _tmp461=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp460=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp45F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3;_tmp45E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EA)->f4;_LL2B: {
# 2184
void*old_t2=(void*)_check_null(_tmp460->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp461;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp461=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp460);
# 2191
{struct _tuple32 _tmp34C=({struct _tuple32 _tmp725;({void*_tmp88A=Cyc_Tcutil_compress(old_t2);_tmp725.f1=_tmp88A;}),({void*_tmp889=Cyc_Tcutil_compress(new_typ);_tmp725.f2=_tmp889;});_tmp725;});struct _tuple32 _tmp34D=_tmp34C;struct Cyc_Absyn_PtrInfo _tmp373;struct Cyc_Absyn_PtrInfo _tmp372;struct Cyc_Absyn_PtrInfo _tmp371;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f1)->tag == 3U)switch(*((int*)_tmp34D.f2)){case 3U: _LLA5: _tmp372=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f1)->f1;_tmp371=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f2)->f1;_LLA6: {
# 2193
struct Cyc_Absyn_Exp*_tmp34E=({void*_tmp88B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp88B,(_tmp372.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp34F=({void*_tmp88C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp88C,(_tmp371.ptr_atts).bounds);});
int _tmp350=Cyc_Tcutil_force_type2bool(0,(_tmp372.ptr_atts).zero_term);
int _tmp351=Cyc_Tcutil_force_type2bool(0,(_tmp371.ptr_atts).zero_term);
{struct _tuple33 _tmp352=({struct _tuple33 _tmp724;_tmp724.f1=_tmp34E,_tmp724.f2=_tmp34F;_tmp724;});struct _tuple33 _tmp353=_tmp352;if(_tmp353.f1 != 0){if(_tmp353.f2 != 0){_LLAC: _LLAD:
# 2200
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp88F=({void*_tmp88E=*_tmp461;Cyc_Toc_cast_it_r(_tmp88E,({struct Cyc_Absyn_Exp*_tmp354[1U];_tmp354[0]=_tmp460;({struct Cyc_Absyn_Exp*_tmp88D=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp88D,_tag_dyneither(_tmp354,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp88F;});else{
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp460->r;}
# 2209
goto _LLAB;}else{_LLAE: _LLAF: {
# 2211
struct Cyc_Absyn_Exp*_tmp355=(struct Cyc_Absyn_Exp*)_check_null(_tmp34E);
struct _tuple13 _tmp356=Cyc_Evexp_eval_const_uint_exp(_tmp355);struct _tuple13 _tmp357=_tmp356;unsigned int _tmp367;int _tmp366;_LLB5: _tmp367=_tmp357.f1;_tmp366=_tmp357.f2;_LLB6:;
if(Cyc_Toc_is_toplevel(nv)){
# 2217
if((_tmp350  && !(_tmp371.elt_tq).real_const) && !_tmp351)
_tmp355=({struct Cyc_Absyn_Exp*_tmp890=_tmp355;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp890,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp891=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp355,_tmp460))->r;e->r=_tmp891;});}else{
# 2222
if(_tmp350){
# 2227
struct _tuple29 _tmp358=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp359=_tmp358;struct _tuple1*_tmp364;struct Cyc_Absyn_Exp*_tmp363;_LLB8: _tmp364=_tmp359.f1;_tmp363=_tmp359.f2;_LLB9:;{
struct Cyc_Absyn_Exp*arg3;
# 2231
{void*_tmp35A=_tmp460->r;void*_tmp35B=_tmp35A;struct Cyc_Absyn_Vardecl*_tmp35E;struct Cyc_Absyn_Vardecl*_tmp35D;switch(*((int*)_tmp35B)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp35B)->f1).Wstring_c).tag){case 8U: _LLBB: _LLBC:
 arg3=_tmp355;goto _LLBA;case 9U: _LLBD: _LLBE:
 arg3=_tmp355;goto _LLBA;default: goto _LLC3;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35B)->f1)){case 1U: _LLBF: _tmp35D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35B)->f1)->f1;_LLC0:
 _tmp35E=_tmp35D;goto _LLC2;case 4U: _LLC1: _tmp35E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35B)->f1)->f1;_LLC2:
# 2236
 if(!Cyc_Tcutil_is_array_type(_tmp35E->type))
goto _LLC4;
arg3=_tmp355;
goto _LLBA;default: goto _LLC3;}default: _LLC3: _LLC4:
# 2243
 if(_tmp366  && _tmp367 != (unsigned int)1)
arg3=_tmp355;else{
# 2246
arg3=({struct Cyc_Absyn_Exp*_tmp35C[2U];({
struct Cyc_Absyn_Exp*_tmp893=({void*_tmp892=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp892,_tmp363);});_tmp35C[0]=_tmp893;}),_tmp35C[1]=_tmp355;({struct Cyc_Absyn_Exp*_tmp894=
# 2246
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp460);Cyc_Toc_fncall_exp_dl(_tmp894,_tag_dyneither(_tmp35C,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2248
goto _LLBA;}_LLBA:;}
# 2250
if(!_tmp351  && !(_tmp371.elt_tq).real_const)
# 2253
arg3=({struct Cyc_Absyn_Exp*_tmp895=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp895,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2255
struct Cyc_Absyn_Exp*_tmp35F=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp371.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp360=({struct Cyc_Absyn_Exp*_tmp362[3U];_tmp362[0]=_tmp363,_tmp362[1]=_tmp35F,_tmp362[2]=arg3;({struct Cyc_Absyn_Exp*_tmp896=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp896,_tag_dyneither(_tmp362,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*_tmp361=Cyc_Absyn_exp_stmt(_tmp360,0U);
_tmp361=({struct _tuple1*_tmp899=_tmp364;void*_tmp898=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp897=_tmp460;Cyc_Absyn_declare_stmt(_tmp899,_tmp898,_tmp897,_tmp361,0U);});
({void*_tmp89A=Cyc_Toc_stmt_exp_r(_tmp361);e->r=_tmp89A;});};};}else{
# 2262
({void*_tmp89D=({struct Cyc_Absyn_Exp*_tmp365[3U];_tmp365[0]=_tmp460,({
# 2264
struct Cyc_Absyn_Exp*_tmp89B=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp371.elt_type),0U);_tmp365[1]=_tmp89B;}),_tmp365[2]=_tmp355;({struct Cyc_Absyn_Exp*_tmp89C=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp89C,_tag_dyneither(_tmp365,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2262
e->r=_tmp89D;});}}
# 2267
goto _LLAB;}}}else{if(_tmp353.f2 != 0){_LLB0: _LLB1: {
# 2277 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp368=(struct Cyc_Absyn_Exp*)_check_null(_tmp34F);
if(_tmp350  && !_tmp351)
_tmp368=({struct Cyc_Absyn_Exp*_tmp89E=_tmp34F;Cyc_Absyn_add_exp(_tmp89E,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2284
struct Cyc_Absyn_Exp*_tmp369=({struct Cyc_Absyn_Exp*_tmp36B[3U];_tmp36B[0]=_tmp460,({
# 2286
struct Cyc_Absyn_Exp*_tmp89F=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp372.elt_type),0U);_tmp36B[1]=_tmp89F;}),_tmp36B[2]=_tmp368;({struct Cyc_Absyn_Exp*_tmp8A0=Cyc_Toc__untag_dyneither_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp8A0,_tag_dyneither(_tmp36B,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2288
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8A3=({struct Cyc_Absyn_Exp*_tmp36A[1U];({struct Cyc_Absyn_Exp*_tmp8A1=Cyc_Absyn_copy_exp(_tmp369);_tmp36A[0]=_tmp8A1;});({struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp8A2,_tag_dyneither(_tmp36A,sizeof(struct Cyc_Absyn_Exp*),1U));});});_tmp369->r=_tmp8A3;});
({void*_tmp8A4=Cyc_Toc_cast_it_r(*_tmp461,_tmp369);e->r=_tmp8A4;});
goto _LLAB;};}}else{_LLB2: _LLB3:
# 2296
 if((_tmp350  && !_tmp351) && !(_tmp371.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp36C=0U;({struct _dyneither_ptr _tmp8A5=({const char*_tmp36D="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp36D,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp8A5,_tag_dyneither(_tmp36C,sizeof(void*),0U));});});
({void*_tmp8A9=({struct Cyc_Absyn_Exp*_tmp36E[3U];_tmp36E[0]=_tmp460,({
struct Cyc_Absyn_Exp*_tmp8A7=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp372.elt_type),0U);_tmp36E[1]=_tmp8A7;}),({
struct Cyc_Absyn_Exp*_tmp8A6=Cyc_Absyn_uint_exp(1U,0U);_tmp36E[2]=_tmp8A6;});({struct Cyc_Absyn_Exp*_tmp8A8=Cyc_Toc__dyneither_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp8A8,_tag_dyneither(_tmp36E,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2299
e->r=_tmp8A9;});}
# 2303
goto _LLAB;}}_LLAB:;}
# 2305
goto _LLA4;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f2)->f1)->tag == 1U){_LLA7: _tmp373=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f1)->f1;_LLA8:
# 2307
{struct Cyc_Absyn_Exp*_tmp36F=({void*_tmp8AA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8AA,(_tmp373.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp370=_tmp36F;if(_tmp370 == 0){_LLC6: _LLC7:
# 2309
({void*_tmp8AC=({struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_new_exp(_tmp460->r,_tmp460->loc);Cyc_Toc_aggrmember_exp_r(_tmp8AB,Cyc_Toc_curr_sp);});_tmp460->r=_tmp8AC;});
_tmp460->topt=new_typ_c;
goto _LLC5;}else{_LLC8: _LLC9:
 goto _LLC5;}_LLC5:;}
# 2314
goto _LLA4;}else{goto _LLA9;}default: goto _LLA9;}else{_LLA9: _LLAA:
# 2316
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp460->r;
goto _LLA4;}_LLA4:;}
# 2320
goto _LL7;}case 15U: _LL2C: _tmp462=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_LL2D:
# 2323
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp462);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp462)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp462,0,Cyc_Toc_address_lvalue,1);
# 2329
({void*_tmp8AE=({void*_tmp8AD=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8AD,_tmp462);});e->r=_tmp8AE;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp462->topt))))
# 2333
({void*_tmp8B0=({void*_tmp8AF=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8AF,_tmp462);});e->r=_tmp8B0;});}
# 2335
goto _LL7;case 16U: _LL2E: _tmp464=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp463=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL2F:
# 2342
 Cyc_Toc_exp_to_c(nv,_tmp463);{
# 2344
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp463->topt));
struct _tuple29 _tmp374=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp375=_tmp374;struct _tuple1*_tmp381;struct Cyc_Absyn_Exp*_tmp380;_LLCB: _tmp381=_tmp375.f1;_tmp380=_tmp375.f2;_LLCC:;{
struct Cyc_Absyn_Exp*lhs;
{void*_tmp376=Cyc_Tcutil_compress(elt_typ);void*_tmp377=_tmp376;void*_tmp378;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp377)->tag == 4U){_LLCE: _tmp378=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp377)->f1).elt_type;_LLCF:
# 2349
 elt_typ=Cyc_Toc_typ_to_c(_tmp378);
lhs=Cyc_Absyn_copy_exp(_tmp380);
goto _LLCD;}else{_LLD0: _LLD1:
# 2353
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp380),0U);
goto _LLCD;}_LLCD:;}{
# 2356
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp463);
struct _tuple1*_tmp379=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp37A=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp379),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)_tmp37A)
mexp=({struct Cyc_Absyn_Exp*_tmp37B[2U];_tmp37B[0]=_tmp37A,({struct Cyc_Absyn_Exp*_tmp8B1=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp37B[1]=_tmp8B1;});({struct Cyc_Absyn_Exp*_tmp8B2=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp8B2,_tag_dyneither(_tmp37B,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2363
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp380,0U),0U);}{
# 2366
struct Cyc_Absyn_Exp*_tmp37C=Cyc_Toc_get_varsizeexp(_tmp463);
if(_tmp37C != 0)
mexp=Cyc_Absyn_add_exp(mexp,_tmp37C,0U);
# 2370
if(_tmp464 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2373
struct Cyc_Absyn_Exp*r=_tmp464;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 2377
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp37D[3U];({
struct Cyc_Absyn_Exp*_tmp8B5=Cyc_Absyn_copy_exp(_tmp380);_tmp37D[0]=_tmp8B5;}),({
struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp37D[1]=_tmp8B4;}),
(unsigned int)_tmp37A?_tmp37D[2]=_tmp37A:({struct Cyc_Absyn_Exp*_tmp8B3=Cyc_Absyn_uint_exp(1U,0U);_tmp37D[2]=_tmp8B3;});({struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp8B6,_tag_dyneither(_tmp37D,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2384
answer=Cyc_Absyn_copy_exp(_tmp380);}
({void*_tmp8B7=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E->tag=Cyc_Toc_Dest,_tmp37E->f1=_tmp380;_tmp37E;});e->annot=_tmp8B7;});{
struct Cyc_Absyn_Stmt*_tmp37F=({struct _tuple1*_tmp8BB=_tmp381;void*_tmp8BA=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8B9=mexp;Cyc_Absyn_declare_stmt(_tmp8BB,_tmp8BA,_tmp8B9,({
struct Cyc_Absyn_Stmt*_tmp8B8=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp8B8,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2390
if((unsigned int)array_len)
_tmp37F=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp379),Cyc_Absyn_uint_type,array_len,_tmp37F,0U);
({void*_tmp8BC=Cyc_Toc_stmt_exp_r(_tmp37F);e->r=_tmp8BC;});
if(_tmp37C != 0)
({void*_tmp8BE=({void*_tmp8BD=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp8BD,Cyc_Absyn_copy_exp(e));});e->r=_tmp8BE;});
goto _LL7;};};};};};};case 18U: _LL30: _tmp465=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_LL31:
# 2398
 Cyc_Toc_exp_to_c(nv,_tmp465);goto _LL7;case 17U: _LL32: _tmp466=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_LL33:
({void*_tmp8C0=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382->tag=17U,({void*_tmp8BF=Cyc_Toc_typ_to_c(_tmp466);_tmp382->f1=_tmp8BF;});_tmp382;});e->r=_tmp8C0;});goto _LL7;case 19U: _LL34: _tmp468=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp467=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL35: {
# 2401
void*_tmp383=_tmp468;
struct Cyc_List_List*_tmp384=_tmp467;
for(0;_tmp384 != 0;_tmp384=_tmp384->tl){
void*_tmp385=(void*)_tmp384->hd;void*_tmp386=_tmp385;unsigned int _tmp398;struct _dyneither_ptr*_tmp397;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp386)->tag == 0U){_LLD3: _tmp397=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp386)->f1;_LLD4:
 goto _LLD2;}else{_LLD5: _tmp398=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp386)->f1;_LLD6:
# 2407
{void*_tmp387=Cyc_Tcutil_compress(_tmp383);void*_tmp388=_tmp387;struct Cyc_List_List*_tmp396;struct Cyc_List_List*_tmp395;struct Cyc_Absyn_Datatypefield*_tmp394;union Cyc_Absyn_AggrInfo _tmp393;switch(*((int*)_tmp388)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp388)->f1)){case 20U: _LLD8: _tmp393=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp388)->f1)->f1;_LLD9: {
# 2409
struct Cyc_Absyn_Aggrdecl*_tmp389=Cyc_Absyn_get_known_aggrdecl(_tmp393);
if(_tmp389->impl == 0)
({void*_tmp38A=0U;({struct _dyneither_ptr _tmp8C1=({const char*_tmp38B="struct fields must be known";_tag_dyneither(_tmp38B,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8C1,_tag_dyneither(_tmp38A,sizeof(void*),0U));});});
_tmp395=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp389->impl))->fields;goto _LLDB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp388)->f1)->f1).KnownDatatypefield).tag == 2){_LLDE: _tmp394=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp388)->f1)->f1).KnownDatatypefield).val).f2;_LLDF:
# 2423
 if(_tmp398 == (unsigned int)0)
({void*_tmp8C2=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F->tag=0U,_tmp38F->f1=Cyc_Toc_tag_sp;_tmp38F;}));_tmp384->hd=_tmp8C2;});else{
# 2426
_tmp383=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp394->typs,(int)(_tmp398 - (unsigned int)1))).f2;
({void*_tmp8C4=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->tag=0U,({struct _dyneither_ptr*_tmp8C3=Cyc_Absyn_fieldname((int)_tmp398);_tmp390->f1=_tmp8C3;});_tmp390;}));_tmp384->hd=_tmp8C4;});}
# 2429
goto _LLD7;}else{goto _LLE0;}default: goto _LLE0;}case 7U: _LLDA: _tmp395=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp388)->f2;_LLDB: {
# 2414
struct Cyc_Absyn_Aggrfield*_tmp38C=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp395,(int)_tmp398);
({void*_tmp8C5=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D->tag=0U,_tmp38D->f1=_tmp38C->name;_tmp38D;}));_tmp384->hd=_tmp8C5;});
_tmp383=_tmp38C->type;
goto _LLD7;}case 6U: _LLDC: _tmp396=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp388)->f1;_LLDD:
# 2419
({void*_tmp8C7=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E->tag=0U,({struct _dyneither_ptr*_tmp8C6=Cyc_Absyn_fieldname((int)(_tmp398 + (unsigned int)1));_tmp38E->f1=_tmp8C6;});_tmp38E;}));_tmp384->hd=_tmp8C7;});
_tmp383=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp396,(int)_tmp398)).f2;
goto _LLD7;default: _LLE0: _LLE1:
# 2430
({void*_tmp391=0U;({struct _dyneither_ptr _tmp8C8=({const char*_tmp392="impossible type for offsetof tuple index";_tag_dyneither(_tmp392,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8C8,_tag_dyneither(_tmp391,sizeof(void*),0U));});});}_LLD7:;}
# 2432
goto _LLD2;}_LLD2:;}
# 2435
({void*_tmp8CA=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399->tag=19U,({void*_tmp8C9=Cyc_Toc_typ_to_c(_tmp468);_tmp399->f1=_tmp8C9;}),_tmp399->f2=_tmp467;_tmp399;});e->r=_tmp8CA;});
goto _LL7;}case 21U: _LL36: _tmp46C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp46B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp46A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3;_tmp469=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2EA)->f4;_LL37: {
# 2438
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp46C->topt);
Cyc_Toc_exp_to_c(nv,_tmp46C);
if(_tmp46A  && _tmp469)
({void*_tmp8CF=({struct Cyc_Absyn_Exp*_tmp8CE=_tmp46C;void*_tmp8CD=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp8CC=e1_cyc_type;struct _dyneither_ptr*_tmp8CB=_tmp46B;Cyc_Toc_check_tagged_union(_tmp8CE,_tmp8CD,_tmp8CC,_tmp8CB,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2442
e->r=_tmp8CF;});
# 2444
if(is_poly)
({void*_tmp8D1=({void*_tmp8D0=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8D0,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp8D1;});
goto _LL7;}case 22U: _LL38: _tmp470=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp46F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp46E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3;_tmp46D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2EA)->f4;_LL39: {
# 2448
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(_tmp470->topt);
void*ta;
{void*_tmp39A=Cyc_Tcutil_compress(e1typ);void*_tmp39B=_tmp39A;struct Cyc_Absyn_PtrInfo _tmp39E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39B)->tag == 3U){_LLE3: _tmp39E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39B)->f1;_LLE4:
 ta=_tmp39E.elt_type;goto _LLE2;}else{_LLE5: _LLE6:
({void*_tmp39C=0U;({struct _dyneither_ptr _tmp8D2=({const char*_tmp39D="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp39D,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D2,_tag_dyneither(_tmp39C,sizeof(void*),0U));});});}_LLE2:;}
# 2455
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp470,e->annot,0);
if(_tmp46E  && _tmp46D)
({void*_tmp8D7=({struct Cyc_Absyn_Exp*_tmp8D6=_tmp470;void*_tmp8D5=Cyc_Toc_typ_to_c(e1typ);void*_tmp8D4=ta;struct _dyneither_ptr*_tmp8D3=_tmp46F;Cyc_Toc_check_tagged_union(_tmp8D6,_tmp8D5,_tmp8D4,_tmp8D3,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp8D7;});
if(is_poly  && _tmp46D)
({void*_tmp8D9=({void*_tmp8D8=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8D8,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp8D9;});
goto _LL7;}case 20U: _LL3A: _tmp471=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_LL3B:
# 2463
 did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp471,e->annot,0);
goto _LL7;case 23U: _LL3C: _tmp473=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp472=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL3D:
# 2467
{void*_tmp39F=Cyc_Tcutil_compress((void*)_check_null(_tmp473->topt));void*_tmp3A0=_tmp39F;struct Cyc_List_List*_tmp3A8;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A0)->tag == 6U){_LLE8: _tmp3A8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A0)->f1;_LLE9:
# 2469
 Cyc_Toc_exp_to_c(nv,_tmp473);{
int _tmp3A1=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp472);{
struct _tuple13 _tmp3A2=Cyc_Evexp_eval_const_uint_exp(_tmp472);struct _tuple13 _tmp3A3=_tmp3A2;unsigned int _tmp3A7;int _tmp3A6;_LLED: _tmp3A7=_tmp3A3.f1;_tmp3A6=_tmp3A3.f2;_LLEE:;
if(!_tmp3A6)
({void*_tmp3A4=0U;({struct _dyneither_ptr _tmp8DA=({const char*_tmp3A5="unknown tuple subscript in translation to C";_tag_dyneither(_tmp3A5,sizeof(char),44U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp8DA,_tag_dyneither(_tmp3A4,sizeof(void*),0U));});});
({void*_tmp8DC=({struct Cyc_Absyn_Exp*_tmp8DB=_tmp473;Cyc_Toc_aggrmember_exp_r(_tmp8DB,Cyc_Absyn_fieldname((int)(_tmp3A7 + (unsigned int)1)));});e->r=_tmp8DC;});
goto _LLE7;};};}else{_LLEA: _LLEB:
# 2479
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,_tmp473,e->annot,_tmp472);
if(index_used)
({void*_tmp8DD=Cyc_Toc_deref_exp_r(_tmp473);e->r=_tmp8DD;});
goto _LLE7;};}_LLE7:;}
# 2485
goto _LL7;case 24U: _LL3E: _tmp474=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_LL3F:
# 2487
 if(!Cyc_Toc_is_toplevel(nv)){
# 2489
void*_tmp3A9=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp474));
{void*_tmp3AA=_tmp3A9;union Cyc_Absyn_AggrInfo _tmp3AF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f2 == 0){_LLF0: _tmp3AF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f1)->f1;_LLF1: {
# 2492
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3AF);
{int i=1;for(0;_tmp474 != 0;(_tmp474=_tmp474->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp474->hd);
dles=({struct Cyc_List_List*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));({struct _tuple19*_tmp8DF=({struct _dyneither_ptr*_tmp8DE=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp8DE,(struct Cyc_Absyn_Exp*)_tmp474->hd);});_tmp3AB->hd=_tmp8DF;}),_tmp3AB->tl=dles;_tmp3AB;});}}
# 2498
({void*_tmp8E1=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->tag=29U,_tmp3AC->f1=sd->name,_tmp3AC->f2=0,({struct Cyc_List_List*_tmp8E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp3AC->f3=_tmp8E0;}),_tmp3AC->f4=sd;_tmp3AC;});e->r=_tmp8E1;});
e->topt=_tmp3A9;
goto _LLEF;}}else{goto _LLF2;}}else{goto _LLF2;}}else{_LLF2: _LLF3:
({void*_tmp3AD=0U;({struct _dyneither_ptr _tmp8E2=({const char*_tmp3AE="tuple type not an aggregate";_tag_dyneither(_tmp3AE,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8E2,_tag_dyneither(_tmp3AD,sizeof(void*),0U));});});}_LLEF:;}
# 2503
goto _LL7;}else{
# 2507
struct Cyc_List_List*dles=0;
for(0;_tmp474 != 0;_tmp474=_tmp474->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp474->hd);
dles=({struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));({struct _tuple19*_tmp8E3=({struct _tuple19*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0->f1=0,_tmp3B0->f2=(struct Cyc_Absyn_Exp*)_tmp474->hd;_tmp3B0;});_tmp3B1->hd=_tmp8E3;}),_tmp3B1->tl=dles;_tmp3B1;});}
# 2512
({void*_tmp8E4=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp8E4;});}
# 2514
goto _LL7;case 26U: _LL40: _tmp475=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_LL41:
# 2520
{struct Cyc_List_List*_tmp3B2=_tmp475;for(0;_tmp3B2 != 0;_tmp3B2=_tmp3B2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3B2->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8E5=Cyc_Toc_unresolvedmem_exp_r(0,_tmp475);e->r=_tmp8E5;});
goto _LL7;case 27U: _LL42: _tmp479=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp478=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp477=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3;_tmp476=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EA)->f4;_LL43:
# 2528
 Cyc_Toc_exp_to_c(nv,_tmp478);
Cyc_Toc_exp_to_c(nv,_tmp477);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp3B3=Cyc_Evexp_eval_const_uint_exp(_tmp478);struct _tuple13 _tmp3B4=_tmp3B3;unsigned int _tmp3BE;int _tmp3BD;_LLF5: _tmp3BE=_tmp3B4.f1;_tmp3BD=_tmp3B4.f2;_LLF6:;{
void*_tmp3B5=Cyc_Toc_typ_to_c((void*)_check_null(_tmp477->topt));
struct Cyc_List_List*es=0;
# 2535
if(!Cyc_Toc_is_zero(_tmp477)){
if(!_tmp3BD)
({void*_tmp3B6=0U;({unsigned int _tmp8E7=_tmp478->loc;struct _dyneither_ptr _tmp8E6=({const char*_tmp3B7="cannot determine value of constant";_tag_dyneither(_tmp3B7,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp8E7,_tmp8E6,_tag_dyneither(_tmp3B6,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp3BE;++ i){
es=({struct Cyc_List_List*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({struct _tuple19*_tmp8E8=({struct _tuple19*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->f1=0,_tmp3B8->f2=_tmp477;_tmp3B8;});_tmp3B9->hd=_tmp8E8;}),_tmp3B9->tl=es;_tmp3B9;});}}
# 2541
if(_tmp476){
struct Cyc_Absyn_Exp*_tmp3BA=({void*_tmp8E9=_tmp3B5;Cyc_Toc_cast_it(_tmp8E9,Cyc_Absyn_uint_exp(0U,0U));});
es=({struct Cyc_List_List*_tmp8EB=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8EB,({struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));({struct _tuple19*_tmp8EA=({struct _tuple19*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->f1=0,_tmp3BB->f2=_tmp3BA;_tmp3BB;});_tmp3BC->hd=_tmp8EA;}),_tmp3BC->tl=0;_tmp3BC;}));});}}
# 2546
({void*_tmp8EC=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp8EC;});};}
# 2548
goto _LL7;case 28U: _LL44: _tmp47C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp47B=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp47A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3;_LL45:
# 2555
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8ED=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp8ED;});else{
# 2558
Cyc_Toc_exp_to_c(nv,_tmp47C);}
goto _LL7;case 30U: _LL46: _tmp47E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp47D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL47:
# 2562
{struct Cyc_List_List*_tmp3BF=_tmp47D;for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3BF->hd)).f2);}}{
void*_tmp3C0=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp3C0;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3C1=Cyc_Tcutil_compress(_tmp3C0);void*_tmp3C2=_tmp3C1;union Cyc_Absyn_AggrInfo _tmp3C6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2)->f2 == 0){_LLF8: _tmp3C6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2)->f1)->f1;_LLF9: {
# 2569
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3C6);
({void*_tmp8EE=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->tag=29U,_tmp3C3->f1=sd->name,_tmp3C3->f2=0,_tmp3C3->f3=_tmp47D,_tmp3C3->f4=sd;_tmp3C3;});e->r=_tmp8EE;});
e->topt=_tmp3C0;
goto _LLF7;}}else{goto _LLFA;}}else{goto _LLFA;}}else{_LLFA: _LLFB:
({void*_tmp3C4=0U;({struct _dyneither_ptr _tmp8EF=({const char*_tmp3C5="anonStruct type not an aggregate";_tag_dyneither(_tmp3C5,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8EF,_tag_dyneither(_tmp3C4,sizeof(void*),0U));});});}_LLF7:;}else{
# 2576
({void*_tmp8F0=Cyc_Toc_unresolvedmem_exp_r(0,_tmp47D);e->r=_tmp8F0;});}
goto _LL7;};case 29U: _LL48: _tmp482=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp481=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp480=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3;_tmp47F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2EA)->f4;_LL49:
# 2582
 if(_tmp47F == 0  || _tmp47F->impl == 0)
({void*_tmp3C7=0U;({struct _dyneither_ptr _tmp8F1=({const char*_tmp3C8="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp3C8,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F1,_tag_dyneither(_tmp3C7,sizeof(void*),0U));});});{
void*_tmp3C9=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp3CA=Cyc_Tcutil_compress(_tmp3C9);void*_tmp3CB=_tmp3CA;union Cyc_Absyn_AggrInfo _tmp3CE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CB)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CB)->f1)->tag == 20U){_LLFD: _tmp3CE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CB)->f1)->f1;_LLFE:
({struct _tuple1*_tmp8F2=(Cyc_Absyn_aggr_kinded_name(_tmp3CE)).f2;*_tmp482=_tmp8F2;});goto _LLFC;}else{goto _LLFF;}}else{_LLFF: _LL100:
({void*_tmp3CC=0U;({struct _dyneither_ptr _tmp8F3=({const char*_tmp3CD="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp3CD,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F3,_tag_dyneither(_tmp3CC,sizeof(void*),0U));});});}_LLFC:;}{
# 2593
struct Cyc_List_List*_tmp3CF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47F->impl))->fields;
if(_tmp3CF == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(_tmp3CF))->tl != 0;_tmp3CF=_tmp3CF->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp3D0=(struct Cyc_Absyn_Aggrfield*)_tmp3CF->hd;
struct Cyc_List_List*_tmp3D1=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp480,_tmp47F->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47F->impl))->fields);
# 2600
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3D2=Cyc_Tcutil_compress(old_typ);void*_tmp3D3=_tmp3D2;struct Cyc_List_List*_tmp3FF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D3)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D3)->f1)->tag == 20U){_LL102: _tmp3FF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D3)->f2;_LL103:
# 2604
{struct Cyc_List_List*_tmp3D4=_tmp3D1;for(0;_tmp3D4 != 0;_tmp3D4=_tmp3D4->tl){
struct _tuple34*_tmp3D5=(struct _tuple34*)_tmp3D4->hd;struct _tuple34*_tmp3D6=_tmp3D5;struct Cyc_Absyn_Aggrfield*_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FB;_LL107: _tmp3FC=_tmp3D6->f1;_tmp3FB=_tmp3D6->f2;_LL108:;{
void*_tmp3D7=_tmp3FB->topt;
Cyc_Toc_exp_to_c(nv,_tmp3FB);
# 2609
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp3FC->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp3FB->topt)))
({void*_tmp8F5=({void*_tmp8F4=Cyc_Toc_typ_to_c(_tmp3FC->type);Cyc_Toc_cast_it(_tmp8F4,
Cyc_Absyn_copy_exp(_tmp3FB));})->r;
# 2611
_tmp3FB->r=_tmp8F5;});
# 2614
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47F->impl))->tagged){
struct _dyneither_ptr*_tmp3D8=_tmp3FC->name;
struct Cyc_Absyn_Exp*_tmp3D9=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp47F,_tmp3D8),0U);
struct _tuple19*_tmp3DA=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp3D9);
struct _tuple19*_tmp3DB=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp3FB);
struct _tuple1*s=({struct _tuple1*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));({union Cyc_Absyn_Nmspace _tmp8F9=Cyc_Absyn_Abs_n(0,1);_tmp3E6->f1=_tmp8F9;}),({
struct _dyneither_ptr*_tmp8F8=({struct _dyneither_ptr*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));({struct _dyneither_ptr _tmp8F7=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3E3=({struct Cyc_String_pa_PrintArg_struct _tmp727;_tmp727.tag=0U,_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp47F->name).f2);_tmp727;});struct Cyc_String_pa_PrintArg_struct _tmp3E4=({struct Cyc_String_pa_PrintArg_struct _tmp726;_tmp726.tag=0U,_tmp726.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3D8);_tmp726;});void*_tmp3E1[2U];_tmp3E1[0]=& _tmp3E3,_tmp3E1[1]=& _tmp3E4;({struct _dyneither_ptr _tmp8F6=({const char*_tmp3E2="_union_%s_%s";_tag_dyneither(_tmp3E2,sizeof(char),13U);});Cyc_aprintf(_tmp8F6,_tag_dyneither(_tmp3E1,sizeof(void*),2U));});});*_tmp3E5=_tmp8F7;});_tmp3E5;});_tmp3E6->f2=_tmp8F8;});_tmp3E6;});
# 2622
struct _tuple19*_tmp3DC=({
struct _dyneither_ptr*_tmp8FB=_tmp3D8;Cyc_Toc_make_field(_tmp8FB,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));_tmp3E0->tag=29U,_tmp3E0->f1=s,_tmp3E0->f2=0,({struct Cyc_List_List*_tmp8FA=({struct _tuple19*_tmp3DF[2U];_tmp3DF[0]=_tmp3DA,_tmp3DF[1]=_tmp3DB;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3DF,sizeof(struct _tuple19*),2U));});_tmp3E0->f3=_tmp8FA;}),_tmp3E0->f4=0;_tmp3E0;}),0U));});
# 2625
({void*_tmp8FD=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->tag=29U,_tmp3DE->f1=*_tmp482,_tmp3DE->f2=0,({struct Cyc_List_List*_tmp8FC=({struct _tuple19*_tmp3DD[1U];_tmp3DD[0]=_tmp3DC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3DD,sizeof(struct _tuple19*),1U));});_tmp3DE->f3=_tmp8FC;}),_tmp3DE->f4=_tmp47F;_tmp3DE;});e->r=_tmp8FD;});}
# 2631
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp3D0 == _tmp3FC){
struct Cyc_List_List*_tmp3E7=({struct Cyc_List_List*_tmp8FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47F->impl))->exist_vars,_tmp481);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8FE,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp47F->tvs,_tmp3FF));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp3E8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47F->impl))->fields;for(0;_tmp3E8 != 0;_tmp3E8=_tmp3E8->tl){
struct Cyc_Absyn_Aggrfield*_tmp3E9=(struct Cyc_Absyn_Aggrfield*)_tmp3E8->hd;
void*_tmp3EA=Cyc_Tcutil_substitute(_tmp3E7,_tmp3E9->type);
struct Cyc_Absyn_Aggrfield*_tmp3EB=({struct Cyc_Absyn_Aggrfield*_tmp8FF=_tmp3E9;Cyc_Toc_aggrfield_to_c(_tmp8FF,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp3E7,_tmp3EA)));});
new_fields=({struct Cyc_List_List*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC->hd=_tmp3EB,_tmp3EC->tl=new_fields;_tmp3EC;});
# 2646
if(_tmp3E8->tl == 0){
{void*_tmp3ED=Cyc_Tcutil_compress(_tmp3EB->type);void*_tmp3EE=_tmp3ED;struct Cyc_Absyn_ArrayInfo _tmp3F3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EE)->tag == 4U){_LL10A: _tmp3F3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EE)->f1;_LL10B:
# 2649
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3F3.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp3EF=_tmp3F3;
({struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_uint_exp(0U,0U);_tmp3EF.num_elts=_tmp900;});
({void*_tmp901=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0->tag=4U,_tmp3F0->f1=_tmp3EF;_tmp3F0;});_tmp3EB->type=_tmp901;});}
# 2654
goto _LL109;}else{_LL10C: _LL10D:
# 2658
 if(_tmp3FB->topt == 0)
goto _LL109;
{void*_tmp3F1=Cyc_Tcutil_compress((void*)_check_null(_tmp3FB->topt));void*_tmp3F2=_tmp3F1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F2)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F2)->f1)->tag == 20U){_LL10F: _LL110:
# 2662
 _tmp3EB->type=(void*)_check_null(_tmp3FB->topt);goto _LL10E;}else{goto _LL111;}}else{_LL111: _LL112:
 goto _LL10E;}_LL10E:;}
# 2665
goto _LL109;}_LL109:;}
# 2669
if(!Cyc_Tcutil_is_array_type(_tmp3E9->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3E9->type)))
({struct Cyc_List_List*_tmp903=({struct Cyc_List_List*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));({void*_tmp902=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->tag=6U,_tmp3F4->f1=0;_tmp3F4;});_tmp3F5->hd=_tmp902;}),_tmp3F5->tl=_tmp3EB->attributes;_tmp3F5;});_tmp3EB->attributes=_tmp903;});}}}
# 2675
_tmp47F=({struct _dyneither_ptr*_tmp906=({struct _dyneither_ptr*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));({struct _dyneither_ptr _tmp905=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3F8=({struct Cyc_Int_pa_PrintArg_struct _tmp728;_tmp728.tag=1U,_tmp728.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp728;});void*_tmp3F6[1U];_tmp3F6[0]=& _tmp3F8;({struct _dyneither_ptr _tmp904=({const char*_tmp3F7="_genStruct%d";_tag_dyneither(_tmp3F7,sizeof(char),13U);});Cyc_aprintf(_tmp904,_tag_dyneither(_tmp3F6,sizeof(void*),1U));});});*_tmp3F9=_tmp905;});_tmp3F9;});Cyc_Toc_make_c_struct_defn(_tmp906,
# 2677
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*_tmp482=_tmp47F->name;
Cyc_Toc_aggrdecl_to_c(_tmp47F);
# 2681
({void*_tmp907=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3FA=_cycalloc(sizeof(*_tmp3FA));*_tmp3FA=_tmp47F;_tmp3FA;})),0);e->topt=_tmp907;});}};}}
# 2684
goto _LL101;}else{goto _LL104;}}else{_LL104: _LL105:
({void*_tmp3FD=0U;({struct _dyneither_ptr _tmp908=({const char*_tmp3FE="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp3FE,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp908,_tag_dyneither(_tmp3FD,sizeof(void*),0U));});});}_LL101:;}else{
# 2693
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47F->impl))->tagged){
# 2695
struct _tuple34*_tmp400=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp3D1))->hd;struct _tuple34*_tmp401=_tmp400;struct Cyc_Absyn_Aggrfield*_tmp40A;struct Cyc_Absyn_Exp*_tmp409;_LL114: _tmp40A=_tmp401->f1;_tmp409=_tmp401->f2;_LL115:;{
void*_tmp402=(void*)_check_null(_tmp409->topt);
void*_tmp403=_tmp40A->type;
Cyc_Toc_exp_to_c(nv,_tmp409);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp403) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp402))
({void*_tmp90A=({
void*_tmp909=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp909,Cyc_Absyn_new_exp(_tmp409->r,0U));});
# 2701
_tmp409->r=_tmp90A;});{
# 2704
int i=Cyc_Toc_get_member_offset(_tmp47F,_tmp40A->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp404=({struct _tuple19*_tmp406[2U];({struct _tuple19*_tmp90C=({struct _tuple19*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->f1=0,_tmp407->f2=field_tag_exp;_tmp407;});_tmp406[0]=_tmp90C;}),({struct _tuple19*_tmp90B=({struct _tuple19*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->f1=0,_tmp408->f2=_tmp409;_tmp408;});_tmp406[1]=_tmp90B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp406,sizeof(struct _tuple19*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp404,0U);
({void*_tmp90E=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp405[1U];({struct _tuple19*_tmp90D=Cyc_Toc_make_field(_tmp40A->name,umem);_tmp405[0]=_tmp90D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp405,sizeof(struct _tuple19*),1U));}));e->r=_tmp90E;});};};}else{
# 2711
struct Cyc_List_List*_tmp40B=0;
struct Cyc_List_List*_tmp40C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47F->impl))->fields;
for(0;_tmp40C != 0;_tmp40C=_tmp40C->tl){
struct Cyc_List_List*_tmp40D=_tmp3D1;for(0;_tmp40D != 0;_tmp40D=_tmp40D->tl){
if((*((struct _tuple34*)_tmp40D->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp40C->hd){
struct _tuple34*_tmp40E=(struct _tuple34*)_tmp40D->hd;struct _tuple34*_tmp40F=_tmp40E;struct Cyc_Absyn_Aggrfield*_tmp415;struct Cyc_Absyn_Exp*_tmp414;_LL117: _tmp415=_tmp40F->f1;_tmp414=_tmp40F->f2;_LL118:;{
void*_tmp410=Cyc_Toc_typ_to_c(_tmp415->type);
void*_tmp411=Cyc_Toc_typ_to_c((void*)_check_null(_tmp414->topt));
Cyc_Toc_exp_to_c(nv,_tmp414);
# 2721
if(!Cyc_Tcutil_unify(_tmp410,_tmp411)){
# 2723
if(!Cyc_Tcutil_is_arithmetic_type(_tmp410) || !
Cyc_Tcutil_is_arithmetic_type(_tmp411))
_tmp414=({void*_tmp90F=_tmp410;Cyc_Toc_cast_it(_tmp90F,Cyc_Absyn_copy_exp(_tmp414));});}
_tmp40B=({struct Cyc_List_List*_tmp413=_cycalloc(sizeof(*_tmp413));({struct _tuple19*_tmp910=({struct _tuple19*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->f1=0,_tmp412->f2=_tmp414;_tmp412;});_tmp413->hd=_tmp910;}),_tmp413->tl=_tmp40B;_tmp413;});
break;};}}}
# 2730
({void*_tmp911=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp40B));e->r=_tmp911;});}}
# 2733
goto _LL7;};};};case 31U: _LL4A: _tmp485=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp484=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_tmp483=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2EA)->f3;_LL4B: {
# 2737
struct Cyc_List_List*_tmp416=_tmp483->typs;
{struct Cyc_List_List*_tmp417=_tmp485;for(0;_tmp417 != 0;(_tmp417=_tmp417->tl,_tmp416=_tmp416->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp417->hd;
void*_tmp418=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp416))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp418))
({void*_tmp912=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp912;});}}{
# 2748
struct Cyc_Absyn_Exp*_tmp419=
_tmp484->is_extensible?Cyc_Absyn_var_exp(_tmp483->name,0U): Cyc_Toc_datatype_tag(_tmp484,_tmp483->name);
# 2751
if(!Cyc_Toc_is_toplevel(nv)){
# 2753
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp485 != 0;(_tmp485=_tmp485->tl,++ i)){
dles=({struct Cyc_List_List*_tmp41A=_cycalloc(sizeof(*_tmp41A));({struct _tuple19*_tmp914=({struct _dyneither_ptr*_tmp913=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp913,(struct Cyc_Absyn_Exp*)_tmp485->hd);});_tmp41A->hd=_tmp914;}),_tmp41A->tl=dles;_tmp41A;});}}{
# 2759
struct _tuple19*_tmp41B=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp419);
({void*_tmp918=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->tag=29U,({struct _tuple1*_tmp917=Cyc_Toc_collapse_qvars(_tmp483->name,_tmp484->name);_tmp41D->f1=_tmp917;}),_tmp41D->f2=0,({
struct Cyc_List_List*_tmp916=({struct Cyc_List_List*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->hd=_tmp41B,({struct Cyc_List_List*_tmp915=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp41C->tl=_tmp915;});_tmp41C;});_tmp41D->f3=_tmp916;}),_tmp41D->f4=0;_tmp41D;});
# 2760
e->r=_tmp918;});};}else{
# 2765
struct Cyc_List_List*_tmp41E=0;
for(0;_tmp485 != 0;_tmp485=_tmp485->tl){
_tmp41E=({struct Cyc_List_List*_tmp420=_cycalloc(sizeof(*_tmp420));({struct _tuple19*_tmp919=({struct _tuple19*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->f1=0,_tmp41F->f2=(struct Cyc_Absyn_Exp*)_tmp485->hd;_tmp41F;});_tmp420->hd=_tmp919;}),_tmp420->tl=_tmp41E;_tmp420;});}
({void*_tmp91C=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp422=_cycalloc(sizeof(*_tmp422));({struct _tuple19*_tmp91B=({struct _tuple19*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->f1=0,_tmp421->f2=_tmp419;_tmp421;});_tmp422->hd=_tmp91B;}),({
struct Cyc_List_List*_tmp91A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp41E);_tmp422->tl=_tmp91A;});_tmp422;}));
# 2768
e->r=_tmp91C;});}
# 2771
goto _LL7;};}case 32U: _LL4C: _LL4D:
# 2773
 goto _LL4F;case 33U: _LL4E: _LL4F:
 goto _LL7;case 34U: _LL50: _tmp48B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1).is_calloc;_tmp48A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1).rgn;_tmp489=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1).elt_type;_tmp488=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1).num_elts;_tmp487=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1).fat_result;_tmp486=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1).inline_call;_LL51: {
# 2777
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp489)));
Cyc_Toc_exp_to_c(nv,_tmp488);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=_tmp488;
if(_tmp487){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2786
if(_tmp48B){
if(_tmp48A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp48A;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=({struct Cyc_Absyn_Exp*_tmp423[3U];_tmp423[0]=rgn,({
struct Cyc_Absyn_Exp*_tmp91D=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp423[1]=_tmp91D;}),_tmp423[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp91E=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp91E,_tag_dyneither(_tmp423,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2793
pexp=({void*_tmp920=*_tmp489;struct Cyc_Absyn_Exp*_tmp91F=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp920,_tmp91F,num_elts2);});}}else{
# 2795
if(_tmp48A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp48A;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp486)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,num_elts2);else{
# 2801
pexp=Cyc_Toc_rmalloc_exp(rgn,num_elts2);}}else{
# 2803
pexp=Cyc_Toc_malloc_exp(*_tmp489,num_elts2);}}
# 2805
if(_tmp487){
struct Cyc_Absyn_Exp*elt_sz=_tmp48B?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp424[3U];_tmp424[0]=pexp,_tmp424[1]=elt_sz,_tmp424[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp921=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp921,_tag_dyneither(_tmp424,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp924=(struct _tuple1*)_check_null(x);void*_tmp923=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp922=_tmp488;Cyc_Absyn_declare_stmt(_tmp924,_tmp923,_tmp922,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp925=Cyc_Toc_stmt_exp_r(s);e->r=_tmp925;});}else{
# 2812
e->r=pexp->r;}
goto _LL7;};}case 35U: _LL52: _tmp48D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp48C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL53: {
# 2822
void*e1_old_typ=(void*)_check_null(_tmp48D->topt);
void*e2_old_typ=(void*)_check_null(_tmp48C->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp425=0U;({struct _dyneither_ptr _tmp926=({const char*_tmp426="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp426,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp926,_tag_dyneither(_tmp425,sizeof(void*),0U));});});{
# 2830
unsigned int _tmp427=e->loc;
struct _tuple1*_tmp428=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_var_exp(_tmp428,_tmp427);_tmp429->topt=e1_old_typ;{
struct _tuple1*_tmp42A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_var_exp(_tmp42A,_tmp427);_tmp42B->topt=e2_old_typ;{
# 2836
struct Cyc_Absyn_Exp*_tmp42C=({struct Cyc_Absyn_Exp*_tmp928=Cyc_Tcutil_deep_copy_exp(1,_tmp48D);struct Cyc_Absyn_Exp*_tmp927=_tmp42B;Cyc_Absyn_assign_exp(_tmp928,_tmp927,_tmp427);});_tmp42C->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp42D=Cyc_Absyn_exp_stmt(_tmp42C,_tmp427);
struct Cyc_Absyn_Exp*_tmp42E=({struct Cyc_Absyn_Exp*_tmp92A=Cyc_Tcutil_deep_copy_exp(1,_tmp48C);struct Cyc_Absyn_Exp*_tmp929=_tmp429;Cyc_Absyn_assign_exp(_tmp92A,_tmp929,_tmp427);});_tmp42E->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp42F=Cyc_Absyn_exp_stmt(_tmp42E,_tmp427);
# 2841
struct Cyc_Absyn_Stmt*_tmp430=({struct _tuple1*_tmp932=_tmp428;void*_tmp931=e1_old_typ;struct Cyc_Absyn_Exp*_tmp930=_tmp48D;struct Cyc_Absyn_Stmt*_tmp92F=({
struct _tuple1*_tmp92E=_tmp42A;void*_tmp92D=e2_old_typ;struct Cyc_Absyn_Exp*_tmp92C=_tmp48C;struct Cyc_Absyn_Stmt*_tmp92B=
Cyc_Absyn_seq_stmt(_tmp42D,_tmp42F,_tmp427);
# 2842
Cyc_Absyn_declare_stmt(_tmp92E,_tmp92D,_tmp92C,_tmp92B,_tmp427);});
# 2841
Cyc_Absyn_declare_stmt(_tmp932,_tmp931,_tmp930,_tmp92F,_tmp427);});
# 2844
Cyc_Toc_stmt_to_c(nv,_tmp430);
({void*_tmp933=Cyc_Toc_stmt_exp_r(_tmp430);e->r=_tmp933;});
goto _LL7;};};};};};}case 38U: _LL54: _tmp48F=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_tmp48E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2EA)->f2;_LL55: {
# 2849
void*_tmp431=Cyc_Tcutil_compress((void*)_check_null(_tmp48F->topt));
Cyc_Toc_exp_to_c(nv,_tmp48F);
{void*_tmp432=_tmp431;struct Cyc_Absyn_Aggrdecl*_tmp438;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp432)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp432)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp432)->f1)->f1).KnownAggr).tag == 2){_LL11A: _tmp438=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp432)->f1)->f1).KnownAggr).val;_LL11B: {
# 2853
struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp438,_tmp48E),0U);
struct Cyc_Absyn_Exp*_tmp434=Cyc_Toc_member_exp(_tmp48F,_tmp48E,0U);
struct Cyc_Absyn_Exp*_tmp435=Cyc_Toc_member_exp(_tmp434,Cyc_Toc_tag_sp,0U);
({void*_tmp934=(Cyc_Absyn_eq_exp(_tmp435,_tmp433,0U))->r;e->r=_tmp934;});
goto _LL119;}}else{goto _LL11C;}}else{goto _LL11C;}}else{_LL11C: _LL11D:
({void*_tmp436=0U;({struct _dyneither_ptr _tmp935=({const char*_tmp437="non-aggregate type in tagcheck";_tag_dyneither(_tmp437,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp935,_tag_dyneither(_tmp436,sizeof(void*),0U));});});}_LL119:;}
# 2860
goto _LL7;}case 37U: _LL56: _tmp490=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2EA)->f1;_LL57:
 Cyc_Toc_stmt_to_c(nv,_tmp490);goto _LL7;case 36U: _LL58: _LL59:
# 2863
({void*_tmp439=0U;({struct _dyneither_ptr _tmp936=({const char*_tmp43A="UnresolvedMem";_tag_dyneither(_tmp43A,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp936,_tag_dyneither(_tmp439,sizeof(void*),0U));});});case 25U: _LL5A: _LL5B:
({void*_tmp43B=0U;({struct _dyneither_ptr _tmp937=({const char*_tmp43C="compoundlit";_tag_dyneither(_tmp43C,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp937,_tag_dyneither(_tmp43B,sizeof(void*),0U));});});case 39U: _LL5C: _LL5D:
({void*_tmp43D=0U;({struct _dyneither_ptr _tmp938=({const char*_tmp43E="valueof(-)";_tag_dyneither(_tmp43E,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp938,_tag_dyneither(_tmp43D,sizeof(void*),0U));});});default: _LL5E: _LL5F:
({void*_tmp43F=0U;({struct _dyneither_ptr _tmp939=({const char*_tmp440="__asm__";_tag_dyneither(_tmp440,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp939,_tag_dyneither(_tmp43F,sizeof(void*),0U));});});}_LL7:;}
# 2868
if(!did_inserted_checks)
({void*_tmp491=0U;({unsigned int _tmp93D=e->loc;struct _dyneither_ptr _tmp93C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp494=({struct Cyc_String_pa_PrintArg_struct _tmp729;_tmp729.tag=0U,({
struct _dyneither_ptr _tmp93A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp729.f1=_tmp93A;});_tmp729;});void*_tmp492[1U];_tmp492[0]=& _tmp494;({struct _dyneither_ptr _tmp93B=({const char*_tmp493="Toc did not examine an inserted check: %s";_tag_dyneither(_tmp493,sizeof(char),42U);});Cyc_aprintf(_tmp93B,_tag_dyneither(_tmp492,sizeof(void*),1U));});});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp93D,_tmp93C,_tag_dyneither(_tmp491,sizeof(void*),0U));});});};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2897 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2899
return({struct _tuple35*_tmp495=_region_malloc(r,sizeof(*_tmp495));_tmp495->f1=0,({struct _dyneither_ptr*_tmp93F=Cyc_Toc_fresh_label();_tmp495->f2=_tmp93F;}),({struct _dyneither_ptr*_tmp93E=Cyc_Toc_fresh_label();_tmp495->f3=_tmp93E;}),_tmp495->f4=sc;_tmp495;});}
# 2904
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp496=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 2910
if((int)(((_tmp496->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp499=_tmp496->orig_pat;if((_tmp499.pattern).tag != 1)_throw_match();(_tmp499.pattern).val;})->topt);
void*_tmp497=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp498=_tmp497;switch(*((int*)_tmp498)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp498)->f1)){case 0U: _LL1: _LL2:
# 2915
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmp940=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp940,v);});goto _LL0;}_LL0:;}{
# 2921
void*_tmp49A=_tmp496->access;void*_tmp49B=_tmp49A;struct Cyc_Absyn_Datatypedecl*_tmp4A6;struct Cyc_Absyn_Datatypefield*_tmp4A5;unsigned int _tmp4A4;int _tmp4A3;struct _dyneither_ptr*_tmp4A2;unsigned int _tmp4A1;switch(*((int*)_tmp49B)){case 0U: _LLA: _LLB:
# 2926
 goto _LL9;case 1U: _LLC: _LLD:
# 2931
 if(ps->tl != 0){
void*_tmp49C=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp49D=_tmp49C;struct Cyc_Absyn_Datatypedecl*_tmp4A0;struct Cyc_Absyn_Datatypefield*_tmp49F;unsigned int _tmp49E;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49D)->tag == 3U){_LL15: _tmp4A0=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49D)->f1;_tmp49F=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49D)->f2;_tmp49E=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49D)->f3;_LL16:
# 2934
 ps=ps->tl;
v=({void*_tmp942=({void*_tmp941=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp49F->name,_tmp4A0->name));Cyc_Absyn_cstar_type(_tmp941,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp942,v);});
v=({struct Cyc_Absyn_Exp*_tmp943=v;Cyc_Absyn_aggrarrow_exp(_tmp943,Cyc_Absyn_fieldname((int)(_tmp49E + (unsigned int)1)),0U);});
continue;}else{_LL17: _LL18:
# 2939
 goto _LL14;}_LL14:;}
# 2942
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 2U: _LLE: _tmp4A1=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp49B)->f1;_LLF:
 v=({struct Cyc_Absyn_Exp*_tmp944=v;Cyc_Toc_member_exp(_tmp944,Cyc_Absyn_fieldname((int)(_tmp4A1 + (unsigned int)1)),0U);});goto _LL9;case 4U: _LL10: _tmp4A3=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp49B)->f1;_tmp4A2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp49B)->f2;_LL11:
# 2946
 v=Cyc_Toc_member_exp(v,_tmp4A2,0U);
if(_tmp4A3)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;default: _LL12: _tmp4A6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49B)->f1;_tmp4A5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49B)->f2;_tmp4A4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp49B)->f3;_LL13:
# 2951
 v=({struct Cyc_Absyn_Exp*_tmp945=v;Cyc_Toc_member_exp(_tmp945,Cyc_Absyn_fieldname((int)(_tmp4A4 + (unsigned int)1)),0U);});
goto _LL9;}_LL9:;};}
# 2955
return v;}
# 2960
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4A7=t;struct Cyc_Absyn_Datatypedecl*_tmp4C0;struct Cyc_Absyn_Datatypefield*_tmp4BF;struct _dyneither_ptr*_tmp4BE;int _tmp4BD;int _tmp4BC;struct Cyc_Absyn_Datatypedecl*_tmp4BB;struct Cyc_Absyn_Datatypefield*_tmp4BA;unsigned int _tmp4B9;struct _dyneither_ptr _tmp4B8;int _tmp4B7;void*_tmp4B6;struct Cyc_Absyn_Enumfield*_tmp4B5;struct Cyc_Absyn_Enumdecl*_tmp4B4;struct Cyc_Absyn_Enumfield*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B2;switch(*((int*)_tmp4A7)){case 0U: _LL1: _tmp4B2=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4A7)->f1;_LL2:
# 2964
 if(_tmp4B2 == 0)return v;else{return _tmp4B2;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmp946=v;Cyc_Absyn_eq_exp(_tmp946,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmp947=v;Cyc_Absyn_neq_exp(_tmp947,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4B4=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4A7)->f1;_tmp4B3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4A7)->f2;_LL8:
# 2968
 return({struct Cyc_Absyn_Exp*_tmp948=v;Cyc_Absyn_eq_exp(_tmp948,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8->tag=32U,_tmp4A8->f1=_tmp4B4,_tmp4A8->f2=_tmp4B3;_tmp4A8;}),0U),0U);});case 4U: _LL9: _tmp4B6=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4A7)->f1;_tmp4B5=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4A7)->f2;_LLA:
# 2970
 return({struct Cyc_Absyn_Exp*_tmp949=v;Cyc_Absyn_eq_exp(_tmp949,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->tag=33U,_tmp4A9->f1=_tmp4B6,_tmp4A9->f2=_tmp4B5;_tmp4A9;}),0U),0U);});case 5U: _LLB: _tmp4B8=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4A7)->f1;_tmp4B7=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4A7)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmp94A=v;Cyc_Absyn_eq_exp(_tmp94A,Cyc_Absyn_float_exp(_tmp4B8,_tmp4B7,0U),0U);});case 6U: _LLD: _tmp4B9=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4A7)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmp94B=v;Cyc_Absyn_eq_exp(_tmp94B,Cyc_Absyn_signed_int_exp((int)_tmp4B9,0U),0U);});case 7U: _LLF: _tmp4BC=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4A7)->f1;_tmp4BB=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4A7)->f2;_tmp4BA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4A7)->f3;_LL10:
# 2976
 LOOP1: {
void*_tmp4AA=v->r;void*_tmp4AB=_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AC;switch(*((int*)_tmp4AB)){case 14U: _LL16: _tmp4AC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL17:
 v=_tmp4AC;goto LOOP1;case 20U: _LL18: _tmp4AD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL19:
 v=_tmp4AD;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2983
v=({void*_tmp94D=({void*_tmp94C=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4BA->name,_tmp4BB->name));Cyc_Absyn_cstar_type(_tmp94C,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp94D,v);});
return({struct Cyc_Absyn_Exp*_tmp94E=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp94E,Cyc_Absyn_uint_exp((unsigned int)_tmp4BC,0U),0U);});case 8U: _LL11: _tmp4BE=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4A7)->f1;_tmp4BD=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4A7)->f2;_LL12:
# 2988
 v=({struct Cyc_Absyn_Exp*_tmp94F=Cyc_Toc_member_exp(v,_tmp4BE,0U);Cyc_Toc_member_exp(_tmp94F,Cyc_Toc_tag_sp,0U);});
return({struct Cyc_Absyn_Exp*_tmp950=v;Cyc_Absyn_eq_exp(_tmp950,Cyc_Absyn_signed_int_exp(_tmp4BD,0U),0U);});default: _LL13: _tmp4C0=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4A7)->f1;_tmp4BF=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4A7)->f2;_LL14:
# 2992
 LOOP2: {
void*_tmp4AE=v->r;void*_tmp4AF=_tmp4AE;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;switch(*((int*)_tmp4AF)){case 14U: _LL1D: _tmp4B0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AF)->f2;_LL1E:
 v=_tmp4B0;goto LOOP2;case 20U: _LL1F: _tmp4B1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4AF)->f1;_LL20:
 v=_tmp4B1;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 2999
v=({void*_tmp952=({void*_tmp951=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4BF->name,_tmp4C0->name));Cyc_Absyn_cstar_type(_tmp951,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp952,v);});
return({struct Cyc_Absyn_Exp*_tmp953=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp953,Cyc_Absyn_var_exp(_tmp4BF->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3013
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3020
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3022
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple32 _tmp4C1=*((struct _tuple32*)ss->hd);struct _tuple32 _tmp4C2=_tmp4C1;void*_tmp4C8;_LL1: _tmp4C8=_tmp4C2.f1;_LL2:;{
void*_tmp4C3=_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C7;struct _dyneither_ptr*_tmp4C6;switch(*((int*)_tmp4C3)){case 3U: _LL4: _LL5:
# 3028
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3031
 continue;case 8U: _LLA: _tmp4C6=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4C3)->f1;_LLB:
# 3033
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4->tag=3U,_tmp4C4->f1=_tmp4C6;_tmp4C4;});
continue;case 7U: _LLC: _LLD:
# 3037
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp4C7=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4C3)->f1;if(_tmp4C7 != 0){_LLF:
# 3041
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->tag=2U,_tmp4C5->f1=_tmp4C7;_tmp4C5;});
return({struct _tuple36 _tmp72A;_tmp72A.f1=0,_tmp72A.f2=k;_tmp72A;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp72B;_tmp72B.f1=0,_tmp72B.f2=k;_tmp72B;});}_LL3:;};}
# 3050
return({struct _tuple36 _tmp72C;_tmp72C.f1=c,_tmp72C.f2=k;_tmp72C;});}
# 3055
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4C9=p;int _tmp4D4;int _tmp4D3;unsigned int _tmp4D2;void*_tmp4D1;struct Cyc_Absyn_Enumfield*_tmp4D0;struct Cyc_Absyn_Enumdecl*_tmp4CF;struct Cyc_Absyn_Enumfield*_tmp4CE;switch(*((int*)_tmp4C9)){case 3U: _LL1: _tmp4CF=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4C9)->f1;_tmp4CE=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4C9)->f2;_LL2:
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->tag=32U,_tmp4CA->f1=_tmp4CF,_tmp4CA->f2=_tmp4CE;_tmp4CA;}),0U);goto _LL0;case 4U: _LL3: _tmp4D1=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4C9)->f1;_tmp4D0=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4C9)->f2;_LL4:
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->tag=33U,_tmp4CB->f1=_tmp4D1,_tmp4CB->f2=_tmp4D0;_tmp4CB;}),0U);goto _LL0;case 6U: _LL5: _tmp4D2=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4C9)->f1;_LL6:
 _tmp4D3=(int)_tmp4D2;goto _LL8;case 7U: _LL7: _tmp4D3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4C9)->f1;_LL8:
 _tmp4D4=_tmp4D3;goto _LLA;case 8U: _LL9: _tmp4D4=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4C9)->f2;_LLA:
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp4D4,0U);goto _LL0;default: _LLB: _LLC:
({void*_tmp4CC=0U;({struct _dyneither_ptr _tmp954=({const char*_tmp4CD="compile_pat_test_as_case!";_tag_dyneither(_tmp4CD,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp954,_tag_dyneither(_tmp4CC,sizeof(void*),0U));});});}_LL0:;}
# 3065
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5->tag=17U,_tmp4D5->f1=e;_tmp4D5;}),0U);}
# 3069
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3078
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3081
void*t=(void*)_check_null(p->topt);
void*_tmp4D6=p->r;void*_tmp4D7=_tmp4D6;union Cyc_Absyn_AggrInfo _tmp508;struct Cyc_List_List*_tmp507;struct Cyc_List_List*_tmp506;struct Cyc_List_List*_tmp505;struct Cyc_Absyn_Pat*_tmp504;struct Cyc_Absyn_Datatypedecl*_tmp503;struct Cyc_Absyn_Datatypefield*_tmp502;struct Cyc_List_List*_tmp501;struct Cyc_Absyn_Vardecl*_tmp500;struct Cyc_Absyn_Pat*_tmp4FF;struct Cyc_Absyn_Vardecl*_tmp4FE;struct Cyc_Absyn_Vardecl*_tmp4FD;struct Cyc_Absyn_Pat*_tmp4FC;struct Cyc_Absyn_Vardecl*_tmp4FB;switch(*((int*)_tmp4D7)){case 2U: _LL1: _tmp4FB=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4D7)->f2;_LL2: {
# 3084
struct Cyc_Absyn_Pat*_tmp4D8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp4D8->topt=p->topt;
_tmp4FD=_tmp4FB;_tmp4FC=_tmp4D8;goto _LL4;}case 1U: _LL3: _tmp4FD=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1;_tmp4FC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4D7)->f2;_LL4:
# 3089
({struct Cyc_List_List*_tmp955=({struct Cyc_List_List*_tmp4D9=_region_malloc(rgn,sizeof(*_tmp4D9));_tmp4D9->hd=_tmp4FD,_tmp4D9->tl=*decls;_tmp4D9;});*decls=_tmp955;});{
struct Cyc_Absyn_Stmt*_tmp4DA=({struct Cyc_Absyn_Exp*_tmp956=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->tag=4U,_tmp4DB->f1=_tmp4FD;_tmp4DB;}),0U);Cyc_Absyn_assign_stmt(_tmp956,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmp957=_tmp4DA;Cyc_Toc_seq_stmt_opt(_tmp957,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp4FC));});};case 4U: _LL5: _tmp4FE=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4D7)->f2;_LL6:
# 3094
({struct Cyc_List_List*_tmp958=({struct Cyc_List_List*_tmp4DC=_region_malloc(rgn,sizeof(*_tmp4DC));_tmp4DC->hd=_tmp4FE,_tmp4DC->tl=*decls;_tmp4DC;});*decls=_tmp958;});
return({struct Cyc_Absyn_Exp*_tmp959=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->tag=4U,_tmp4DD->f1=_tmp4FE;_tmp4DD;}),0U);Cyc_Absyn_assign_stmt(_tmp959,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp500=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1;_tmp4FF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4D7)->f2;_LLA:
# 3099
({struct Cyc_List_List*_tmp95A=({struct Cyc_List_List*_tmp4DE=_region_malloc(rgn,sizeof(*_tmp4DE));_tmp4DE->hd=_tmp500,_tmp4DE->tl=*decls;_tmp4DE;});*decls=_tmp95A;});
({void*_tmp95B=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp500->type=_tmp95B;});{
# 3102
struct Cyc_Absyn_Stmt*_tmp4DF=({struct Cyc_Absyn_Exp*_tmp95E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->tag=4U,_tmp4E0->f1=_tmp500;_tmp4E0;}),0U);Cyc_Absyn_assign_stmt(_tmp95E,({
void*_tmp95D=({void*_tmp95C=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp95C,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp95D,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmp95F=_tmp4DF;Cyc_Toc_seq_stmt_opt(_tmp95F,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp4FF));});};case 9U: _LLB: _LLC:
# 3107
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1)->r)->tag == 8U){_LL17: _tmp503=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1)->r)->f1;_tmp502=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1)->r)->f2;_tmp501=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1)->r)->f3;_LL18:
# 3116
 if(_tmp501 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp502->name,_tmp503->name);
void*_tmp4E1=({void*_tmp960=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmp960,Cyc_Toc_mt_tq);});
path=Cyc_Toc_cast_it(_tmp4E1,path);{
int cnt=1;
struct Cyc_List_List*_tmp4E2=_tmp502->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp501 != 0;(_tmp501=_tmp501->tl,_tmp4E2=((struct Cyc_List_List*)_check_null(_tmp4E2))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4E3=(struct Cyc_Absyn_Pat*)_tmp501->hd;
if(_tmp4E3->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp4E4=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4E2))->hd)).f2;
void*_tmp4E5=(void*)_check_null(_tmp4E3->topt);
void*_tmp4E6=Cyc_Toc_typ_to_c_array(_tmp4E5);
struct Cyc_Absyn_Exp*_tmp4E7=({struct Cyc_Absyn_Exp*_tmp961=path;Cyc_Absyn_aggrarrow_exp(_tmp961,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp4E4)))
_tmp4E7=Cyc_Toc_cast_it(_tmp4E6,_tmp4E7);
s=({struct Cyc_Absyn_Stmt*_tmp962=s;Cyc_Toc_seq_stmt_opt(_tmp962,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4E7,_tmp4E3));});};}
# 3134
return s;};};}else{_LL21: _tmp504=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1;_LL22:
# 3179
 return({struct _RegionHandle*_tmp966=rgn;struct Cyc_Toc_Env**_tmp965=nv;struct Cyc_List_List**_tmp964=decls;struct Cyc_Absyn_Exp*_tmp963=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmp966,_tmp965,_tmp964,_tmp963,_tmp504);});}case 8U: _LL19: _tmp505=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4D7)->f3;_LL1A:
# 3136
 _tmp506=_tmp505;goto _LL1C;case 5U: _LL1B: _tmp506=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1;_LL1C: {
# 3138
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp506 != 0;(_tmp506=_tmp506->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4E8=(struct Cyc_Absyn_Pat*)_tmp506->hd;
if(_tmp4E8->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp4E9=(void*)_check_null(_tmp4E8->topt);
struct _dyneither_ptr*_tmp4EA=Cyc_Absyn_fieldname(cnt);
s=({struct Cyc_Absyn_Stmt*_tmp96B=s;Cyc_Toc_seq_stmt_opt(_tmp96B,({struct _RegionHandle*_tmp96A=rgn;struct Cyc_Toc_Env**_tmp969=nv;struct Cyc_List_List**_tmp968=decls;struct Cyc_Absyn_Exp*_tmp967=Cyc_Toc_member_exp(path,_tmp4EA,0U);Cyc_Toc_extract_pattern_vars(_tmp96A,_tmp969,_tmp968,_tmp967,_tmp4E8);}));});};}
# 3148
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1 == 0){_LL1D: _LL1E:
({void*_tmp4EB=0U;({struct _dyneither_ptr _tmp96C=({const char*_tmp4EC="unresolved aggregate pattern!";_tag_dyneither(_tmp4EC,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp96C,_tag_dyneither(_tmp4EB,sizeof(void*),0U));});});}else{_LL1F: _tmp508=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4D7)->f1;_tmp507=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4D7)->f3;_LL20: {
# 3152
struct Cyc_Absyn_Aggrdecl*_tmp4ED=Cyc_Absyn_get_known_aggrdecl(_tmp508);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp507 != 0;_tmp507=_tmp507->tl){
struct _tuple37*_tmp4EE=(struct _tuple37*)_tmp507->hd;
struct Cyc_Absyn_Pat*_tmp4EF=(*_tmp4EE).f2;
if(_tmp4EF->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp4F0=Cyc_Absyn_designatorlist_to_fieldname((*_tmp4EE).f1);
void*_tmp4F1=(void*)_check_null(_tmp4EF->topt);
void*_tmp4F2=Cyc_Toc_typ_to_c_array(_tmp4F1);
struct Cyc_Absyn_Exp*_tmp4F3=Cyc_Toc_member_exp(path,_tmp4F0,0U);
# 3164
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4ED->impl))->tagged)_tmp4F3=Cyc_Toc_member_exp(_tmp4F3,Cyc_Toc_val_sp,0U);{
void*_tmp4F4=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4ED->impl))->fields,_tmp4F0)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp4F4))
_tmp4F3=Cyc_Toc_cast_it(_tmp4F2,_tmp4F3);else{
if(!Cyc_Tcutil_is_array_type(_tmp4F4) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4F4)))
# 3171
_tmp4F3=Cyc_Absyn_deref_exp(({void*_tmp96D=Cyc_Absyn_cstar_type(_tmp4F2,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp96D,
Cyc_Absyn_address_exp(_tmp4F3,0U));}),0U);}
# 3174
s=({struct Cyc_Absyn_Stmt*_tmp96E=s;Cyc_Toc_seq_stmt_opt(_tmp96E,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4F3,_tmp4EF));});};};}
# 3176
return s;}}case 15U: _LL23: _LL24:
# 3181
({void*_tmp4F5=0U;({struct _dyneither_ptr _tmp96F=({const char*_tmp4F6="unknownid pat";_tag_dyneither(_tmp4F6,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp96F,_tag_dyneither(_tmp4F5,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp4F7=0U;({struct _dyneither_ptr _tmp970=({const char*_tmp4F8="unknowncall pat";_tag_dyneither(_tmp4F8,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp970,_tag_dyneither(_tmp4F7,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp4F9=0U;({struct _dyneither_ptr _tmp971=({const char*_tmp4FA="exp pat";_tag_dyneither(_tmp4FA,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp971,_tag_dyneither(_tmp4F9,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3192
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3200
void*_tmp509=dopt;struct Cyc_List_List*_tmp547;struct Cyc_List_List*_tmp546;void*_tmp545;struct Cyc_Tcpat_Rhs*_tmp544;if(_tmp509 == 0){_LL1: _LL2:
# 3202
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp509)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp544=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp509)->f1;_LL6:
# 3207
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp50A=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp50B=_tmp50A;int*_tmp512;struct _dyneither_ptr*_tmp511;struct _dyneither_ptr*_tmp510;struct Cyc_Absyn_Switch_clause*_tmp50F;_LLA: _tmp512=(int*)& _tmp50B->f1;_tmp511=_tmp50B->f2;_tmp510=_tmp50B->f3;_tmp50F=_tmp50B->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp50C=_tmp50F->body;
if(_tmp50C == _tmp544->rhs){
# 3212
if(*_tmp512)
return Cyc_Absyn_goto_stmt(_tmp511,0U);
*_tmp512=1;{
# 3216
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp974=rgn;struct Cyc_List_List**_tmp973=decls;struct Cyc_Absyn_Exp*_tmp972=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp974,& nv,_tmp973,_tmp972,_tmp50F->pattern);});
# 3219
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp510,_tmp50F->body,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(_tmp511,res,0U);
({struct Cyc_List_List*_tmp976=({struct Cyc_List_List*_tmp50E=_region_malloc(rgn,sizeof(*_tmp50E));({struct _tuple38*_tmp975=({struct _tuple38*_tmp50D=_region_malloc(rgn,sizeof(*_tmp50D));_tmp50D->f1=nv,_tmp50D->f2=_tmp510,_tmp50D->f3=_tmp50C;_tmp50D;});_tmp50E->hd=_tmp975;}),_tmp50E->tl=*bodies;_tmp50E;});*bodies=_tmp976;});
return res;};}};}
# 3227
({void*_tmp513=0U;({struct _dyneither_ptr _tmp977=({const char*_tmp514="couldn't find rhs!";_tag_dyneither(_tmp514,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp977,_tag_dyneither(_tmp513,sizeof(void*),0U));});});default: _LL7: _tmp547=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp509)->f1;_tmp546=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp509)->f2;_tmp545=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp509)->f3;_LL8: {
# 3230
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp545,lscs,v);
# 3232
struct Cyc_Absyn_Exp*_tmp515=({struct Cyc_List_List*_tmp978=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp547);Cyc_Toc_compile_path(_tmp978,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp516=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp546);
# 3235
struct _tuple36 _tmp517=Cyc_Toc_admits_switch(_tmp516);struct _tuple36 _tmp518=_tmp517;int _tmp543;void*_tmp542;_LLD: _tmp543=_tmp518.f1;_tmp542=_tmp518.f2;_LLE:;
if(_tmp543 > 1){
# 3239
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp529=_cycalloc(sizeof(*_tmp529));
({struct Cyc_Absyn_Switch_clause*_tmp97A=({struct Cyc_Absyn_Switch_clause*_tmp528=_cycalloc(sizeof(*_tmp528));({struct Cyc_Absyn_Pat*_tmp979=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp528->pattern=_tmp979;}),_tmp528->pat_vars=0,_tmp528->where_clause=0,_tmp528->body=res,_tmp528->loc=0U;_tmp528;});_tmp529->hd=_tmp97A;}),_tmp529->tl=0;_tmp529;});
# 3242
for(0;_tmp516 != 0;_tmp516=_tmp516->tl){
struct _tuple32 _tmp519=*((struct _tuple32*)_tmp516->hd);struct _tuple32 _tmp51A=_tmp519;void*_tmp520;void*_tmp51F;_LL10: _tmp520=_tmp51A.f1;_tmp51F=_tmp51A.f2;_LL11:;{
# 3245
struct Cyc_Absyn_Pat*_tmp51B=Cyc_Toc_compile_pat_test_as_case(_tmp520);
# 3247
struct Cyc_Absyn_Stmt*_tmp51C=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp51F,lscs,v);
# 3249
new_lscs=({struct Cyc_List_List*_tmp51E=_cycalloc(sizeof(*_tmp51E));({struct Cyc_Absyn_Switch_clause*_tmp97B=({struct Cyc_Absyn_Switch_clause*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->pattern=_tmp51B,_tmp51D->pat_vars=0,_tmp51D->where_clause=0,_tmp51D->body=_tmp51C,_tmp51D->loc=0U;_tmp51D;});_tmp51E->hd=_tmp97B;}),_tmp51E->tl=new_lscs;_tmp51E;});};}
# 3251
{void*_tmp521=_tmp542;struct _dyneither_ptr*_tmp526;switch(*((int*)_tmp521)){case 1U: _LL13: _LL14:
# 3254
 LOOP1: {
void*_tmp522=_tmp515->r;void*_tmp523=_tmp522;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp524;switch(*((int*)_tmp523)){case 14U: _LL1C: _tmp524=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp523)->f2;_LL1D:
 _tmp515=_tmp524;goto LOOP1;case 20U: _LL1E: _tmp525=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp523)->f1;_LL1F:
 _tmp515=_tmp525;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3260
_tmp515=Cyc_Absyn_deref_exp(({void*_tmp97C=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp97C,_tmp515);}),0U);goto _LL12;case 3U: _LL15: _tmp526=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp521)->f1;_LL16:
# 3263
 _tmp515=({struct Cyc_Absyn_Exp*_tmp97D=Cyc_Toc_member_exp(_tmp515,_tmp526,0U);Cyc_Toc_member_exp(_tmp97D,Cyc_Toc_tag_sp,0U);});goto _LL12;case 2U: _LL17: _LL18:
# 3265
 goto _LL1A;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3268
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527->tag=10U,_tmp527->f1=_tmp515,_tmp527->f2=new_lscs,_tmp527->f3=0;_tmp527;}),0U);}else{
# 3272
void*_tmp52A=_tmp542;struct Cyc_Absyn_Exp*_tmp541;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp52A)->tag == 2U){_LL23: _tmp541=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp52A)->f1;_LL24: {
# 3274
struct Cyc_List_List*_tmp52B=_tmp516;void*_tmp53A;struct Cyc_Tcpat_Rhs*_tmp539;if(_tmp52B != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)((struct Cyc_List_List*)_tmp52B)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp52B)->tl == 0){_LL28: _tmp53A=((struct _tuple32*)_tmp52B->hd)->f1;_tmp539=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)_tmp52B->hd)->f2)->f1;_LL29:
# 3280
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp52C=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp52D=_tmp52C;int*_tmp534;struct _dyneither_ptr*_tmp533;struct _dyneither_ptr*_tmp532;struct Cyc_Absyn_Switch_clause*_tmp531;_LL2D: _tmp534=(int*)& _tmp52D->f1;_tmp533=_tmp52D->f2;_tmp532=_tmp52D->f3;_tmp531=_tmp52D->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp52E=_tmp531->body;
if(_tmp52E == _tmp539->rhs){
# 3285
if(*_tmp534)
return Cyc_Absyn_goto_stmt(_tmp533,0U);
*_tmp534=1;{
# 3289
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp980=rgn;struct Cyc_List_List**_tmp97F=decls;struct Cyc_Absyn_Exp*_tmp97E=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp980,& nv,_tmp97F,_tmp97E,_tmp531->pattern);});
# 3292
Cyc_Toc_exp_to_c(nv,_tmp541);{
# 3295
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp532,_tmp531->body,0U);
r=Cyc_Absyn_ifthenelse_stmt(_tmp541,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(_tmp533,r,0U);
({struct Cyc_List_List*_tmp982=({struct Cyc_List_List*_tmp530=_region_malloc(rgn,sizeof(*_tmp530));({struct _tuple38*_tmp981=({struct _tuple38*_tmp52F=_region_malloc(rgn,sizeof(*_tmp52F));_tmp52F->f1=nv,_tmp52F->f2=_tmp532,_tmp52F->f3=_tmp52E;_tmp52F;});_tmp530->hd=_tmp981;}),_tmp530->tl=*bodies;_tmp530;});*bodies=_tmp982;});
return r;};};}};}
# 3304
({void*_tmp535=0U;({struct _dyneither_ptr _tmp983=({const char*_tmp536="couldn't find rhs!";_tag_dyneither(_tmp536,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp983,_tag_dyneither(_tmp535,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp537=0U;({struct _dyneither_ptr _tmp984=({const char*_tmp538="bad where clause in match compiler";_tag_dyneither(_tmp538,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp984,_tag_dyneither(_tmp537,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3309
 for(0;_tmp516 != 0;_tmp516=_tmp516->tl){
struct _tuple32 _tmp53B=*((struct _tuple32*)_tmp516->hd);struct _tuple32 _tmp53C=_tmp53B;void*_tmp540;void*_tmp53F;_LL30: _tmp540=_tmp53C.f1;_tmp53F=_tmp53C.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp53D=Cyc_Toc_compile_pat_test(_tmp515,_tmp540);
struct Cyc_Absyn_Stmt*_tmp53E=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp53F,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp53D,_tmp53E,res,0U);};}}_LL22:;}
# 3317
return res;}}}_LL0:;}
# 3327
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3329
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp548=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp549=_tmp548;struct Cyc_Toc_Env**_tmp54B;struct Cyc_Absyn_Stmt*_tmp54A;_LL1: _tmp54B=(struct Cyc_Toc_Env**)& _tmp549->f1;_tmp54A=_tmp549->f3;_LL2:;
if(_tmp54A == s)return _tmp54B;}
# 3335
return 0;}
# 3339
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3342
void*_tmp54C=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3345
struct _tuple29 _tmp54D=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp54E=_tmp54D;struct _tuple1*_tmp55F;struct Cyc_Absyn_Exp*_tmp55E;_LL1: _tmp55F=_tmp54E.f1;_tmp55E=_tmp54E.f2;_LL2:;{
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp54F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp54F;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp550=Cyc_Toc_share_env(rgn,nv);
# 3351
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3356
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp550,& mydecls,& mybodies,dopt,lscs,_tmp55F);
# 3364
{struct Cyc_List_List*_tmp551=lscs;for(0;_tmp551 != 0;_tmp551=_tmp551->tl){
struct _tuple35*_tmp552=(struct _tuple35*)_tmp551->hd;struct _tuple35*_tmp553=_tmp552;struct _dyneither_ptr*_tmp55B;struct Cyc_Absyn_Switch_clause*_tmp55A;_LL4: _tmp55B=_tmp553->f3;_tmp55A=_tmp553->f4;_LL5:;{
struct Cyc_Absyn_Stmt*s=_tmp55A->body;
# 3368
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3371
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp554=*envp;
# 3374
if(_tmp551->tl != 0){
struct _tuple35*_tmp555=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp551->tl))->hd;struct _tuple35*_tmp556=_tmp555;struct _dyneither_ptr*_tmp559;struct Cyc_Absyn_Switch_clause*_tmp558;_LL7: _tmp559=_tmp556->f3;_tmp558=_tmp556->f4;_LL8:;{
# 3378
struct Cyc_Toc_Env**_tmp557=Cyc_Toc_find_case_env(mybodies,_tmp558->body);
# 3383
if(_tmp557 == 0)
({struct Cyc_Toc_Env*_tmp985=Cyc_Toc_last_switchclause_env(rgn,_tmp554,end_l);Cyc_Toc_stmt_to_c(_tmp985,s);});else{
# 3387
struct Cyc_List_List*vs=0;
if(_tmp558->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp558->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 3392
({struct Cyc_Toc_Env*_tmp986=Cyc_Toc_non_last_switchclause_env(rgn,_tmp554,end_l,_tmp559,vs);Cyc_Toc_stmt_to_c(_tmp986,s);});}};}else{
# 3396
({struct Cyc_Toc_Env*_tmp987=Cyc_Toc_last_switchclause_env(rgn,_tmp554,end_l);Cyc_Toc_stmt_to_c(_tmp987,s);});}};};}}{
# 3400
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmp989=test_tree;Cyc_Absyn_seq_stmt(_tmp989,({struct _dyneither_ptr*_tmp988=end_l;Cyc_Absyn_label_stmt(_tmp988,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3402
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp55C=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmp98A=Cyc_Toc_temp_var();_tmp55C->name=_tmp98A;});
({void*_tmp98B=Cyc_Toc_typ_to_c_array(_tmp55C->type);_tmp55C->type=_tmp98B;});
res=({struct Cyc_Absyn_Decl*_tmp98C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->tag=0U,_tmp55D->f1=_tmp55C;_tmp55D;}),0U);Cyc_Absyn_decl_stmt(_tmp98C,res,0U);});}
# 3409
({void*_tmp990=({struct _tuple1*_tmp98F=_tmp55F;void*_tmp98E=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmp98D=e;Cyc_Absyn_declare_stmt(_tmp98F,_tmp98E,_tmp98D,res,0U);})->r;whole_s->r=_tmp990;});
_npop_handler(0U);return;};}
# 3348
;_pop_region(rgn);};};}
# 3415
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3417
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3422
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3424
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp560[1U];({struct Cyc_Absyn_Exp*_tmp991=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp560[0]=_tmp991;});({struct Cyc_Absyn_Exp*_tmp992=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmp992,_tag_dyneither(_tmp560,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3427
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmp994=({struct Cyc_Absyn_Stmt*_tmp993=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmp993,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmp994;});}
# 3432
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3434
while(1){
void*_tmp561=s->r;void*_tmp562=_tmp561;struct Cyc_Absyn_Stmt*_tmp5C9;struct Cyc_List_List*_tmp5C8;void*_tmp5C7;struct Cyc_Absyn_Stmt*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C5;struct _dyneither_ptr*_tmp5C4;struct Cyc_Absyn_Stmt*_tmp5C3;struct Cyc_Absyn_Decl*_tmp5C2;struct Cyc_Absyn_Stmt*_tmp5C1;struct Cyc_List_List*_tmp5C0;struct Cyc_Absyn_Switch_clause**_tmp5BF;struct Cyc_Absyn_Exp*_tmp5BE;struct Cyc_List_List*_tmp5BD;void*_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BA;struct Cyc_Absyn_Exp*_tmp5B9;struct Cyc_Absyn_Stmt*_tmp5B8;struct Cyc_Absyn_Exp*_tmp5B7;struct Cyc_Absyn_Stmt*_tmp5B6;struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Stmt*_tmp5B4;struct Cyc_Absyn_Stmt*_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B2;struct Cyc_Absyn_Stmt*_tmp5B1;struct Cyc_Absyn_Stmt*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5AF;switch(*((int*)_tmp562)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp5AF=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_LL4:
 Cyc_Toc_exp_to_c(nv,_tmp5AF);return;case 2U: _LL5: _tmp5B1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_tmp5B0=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp562)->f2;_LL6:
# 3439
 Cyc_Toc_stmt_to_c(nv,_tmp5B1);
s=_tmp5B0;
continue;case 3U: _LL7: _tmp5B2=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_LL8:
# 3443
 if(_tmp5B2 != 0){
void*t=Cyc_Toc_typ_to_c((void*)_check_null(_tmp5B2->topt));
Cyc_Toc_exp_to_c(nv,_tmp5B2);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
# 3449
struct _tuple1*_tmp563=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp564=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp563,0U),0U);
Cyc_Toc_do_npop_before(npop,_tmp564);
({void*_tmp995=(Cyc_Absyn_declare_stmt(_tmp563,t,_tmp5B2,_tmp564,0U))->r;s->r=_tmp995;});}};}else{
# 3455
({int _tmp996=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp996,s);});}
return;case 4U: _LL9: _tmp5B5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_tmp5B4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp562)->f2;_tmp5B3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp562)->f3;_LLA:
# 3458
 Cyc_Toc_exp_to_c(nv,_tmp5B5);
Cyc_Toc_stmt_to_c(nv,_tmp5B4);
s=_tmp5B3;
continue;case 5U: _LLB: _tmp5B7=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp562)->f1).f1;_tmp5B6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp562)->f2;_LLC:
# 3463
 Cyc_Toc_exp_to_c(nv,_tmp5B7);{
struct _RegionHandle _tmp565=_new_region("temp");struct _RegionHandle*temp=& _tmp565;_push_region(temp);
({struct Cyc_Toc_Env*_tmp997=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp997,_tmp5B6);});
# 3467
_npop_handler(0U);return;
# 3464
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3469
struct Cyc_Toc_Env*_tmp566=nv;struct _dyneither_ptr**_tmp567;_LL22: _tmp567=_tmp566->break_lab;_LL23:;
if(_tmp567 != 0)
({void*_tmp998=Cyc_Toc_goto_stmt_r(*_tmp567);s->r=_tmp998;});
# 3473
({int _tmp999=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp999,s);});
return;}case 7U: _LLF: _LL10: {
# 3476
struct Cyc_Toc_Env*_tmp568=nv;struct _dyneither_ptr**_tmp569;_LL25: _tmp569=_tmp568->continue_lab;_LL26:;
if(_tmp569 != 0)
({void*_tmp99A=Cyc_Toc_goto_stmt_r(*_tmp569);s->r=_tmp99A;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3482
({int _tmp99B=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp99B,s);});
return;case 9U: _LL13: _tmp5BB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_tmp5BA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp562)->f2).f1;_tmp5B9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp562)->f3).f1;_tmp5B8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp562)->f4;_LL14:
# 3486
 Cyc_Toc_exp_to_c(nv,_tmp5BB);Cyc_Toc_exp_to_c(nv,_tmp5BA);Cyc_Toc_exp_to_c(nv,_tmp5B9);{
struct _RegionHandle _tmp56A=_new_region("temp");struct _RegionHandle*temp=& _tmp56A;_push_region(temp);
({struct Cyc_Toc_Env*_tmp99C=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp99C,_tmp5B8);});
# 3490
_npop_handler(0U);return;
# 3487
;_pop_region(temp);};case 10U: _LL15: _tmp5BE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_tmp5BD=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp562)->f2;_tmp5BC=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp562)->f3;_LL16:
# 3492
 Cyc_Toc_xlate_switch(nv,s,_tmp5BE,_tmp5BD,_tmp5BC);
return;case 11U: _LL17: _tmp5C0=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_tmp5BF=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp562)->f2;_LL18: {
# 3495
struct Cyc_Toc_Env*_tmp56B=nv;struct Cyc_Toc_FallthruInfo*_tmp575;_LL28: _tmp575=_tmp56B->fallthru_info;_LL29:;
if(_tmp575 == 0)
({void*_tmp56C=0U;({struct _dyneither_ptr _tmp99D=({const char*_tmp56D="fallthru in unexpected place";_tag_dyneither(_tmp56D,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp99D,_tag_dyneither(_tmp56C,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp56E=*_tmp575;struct Cyc_Toc_FallthruInfo _tmp56F=_tmp56E;struct _dyneither_ptr*_tmp574;struct Cyc_List_List*_tmp573;_LL2B: _tmp574=_tmp56F.label;_tmp573=_tmp56F.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp574,0U);
# 3501
({int _tmp99E=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp99E,s2);});{
struct Cyc_List_List*_tmp570=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp573);
struct Cyc_List_List*_tmp571=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5C0);
for(0;_tmp570 != 0;(_tmp570=_tmp570->tl,_tmp571=_tmp571->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp571))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmp9A0=({struct Cyc_Absyn_Exp*_tmp99F=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->tag=5U,_tmp572->f1=(struct Cyc_Absyn_Vardecl*)_tmp570->hd;_tmp572;}),0U);Cyc_Absyn_assign_stmt(_tmp99F,(struct Cyc_Absyn_Exp*)_tmp571->hd,0U);});Cyc_Absyn_seq_stmt(_tmp9A0,s2,0U);});}
# 3509
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp5C2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_tmp5C1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp562)->f2;_LL1A:
# 3515
{void*_tmp576=_tmp5C2->r;void*_tmp577=_tmp576;struct Cyc_Absyn_Tvar*_tmp596;struct Cyc_Absyn_Vardecl*_tmp595;struct Cyc_Absyn_Exp*_tmp594;struct Cyc_Absyn_Fndecl*_tmp593;struct Cyc_List_List*_tmp592;struct Cyc_Absyn_Pat*_tmp591;struct Cyc_Absyn_Exp*_tmp590;void*_tmp58F;struct Cyc_Absyn_Vardecl*_tmp58E;switch(*((int*)_tmp577)){case 0U: _LL2E: _tmp58E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp577)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp58E,_tmp5C1);goto _LL2D;case 2U: _LL30: _tmp591=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp577)->f1;_tmp590=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp577)->f3;_tmp58F=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp577)->f4;_LL31:
# 3523
{void*_tmp578=_tmp591->r;void*_tmp579=_tmp578;struct Cyc_Absyn_Vardecl*_tmp57B;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp579)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp579)->f2)->r)->tag == 0U){_LL3B: _tmp57B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp579)->f1;_LL3C:
# 3525
({struct _tuple1*_tmp9A1=Cyc_Toc_temp_var();_tmp57B->name=_tmp9A1;});
_tmp57B->initializer=_tmp590;
({void*_tmp9A2=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->tag=0U,_tmp57A->f1=_tmp57B;_tmp57A;});_tmp5C2->r=_tmp9A2;});
Cyc_Toc_local_decl_to_c(nv,_tmp57B,_tmp5C1);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3534
({void*_tmp9A3=(Cyc_Toc_letdecl_to_c(nv,_tmp591,_tmp58F,(void*)_check_null(_tmp590->topt),_tmp590,_tmp5C1))->r;s->r=_tmp9A3;});
goto _LL3A;}_LL3A:;}
# 3537
goto _LL2D;case 3U: _LL32: _tmp592=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp577)->f1;_LL33: {
# 3541
struct Cyc_List_List*_tmp57C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp592);
if(_tmp57C == 0)
({void*_tmp57D=0U;({struct _dyneither_ptr _tmp9A4=({const char*_tmp57E="empty Letv_d";_tag_dyneither(_tmp57E,sizeof(char),13U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp9A4,_tag_dyneither(_tmp57D,sizeof(void*),0U));});});
({void*_tmp9A5=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->tag=0U,_tmp57F->f1=(struct Cyc_Absyn_Vardecl*)_tmp57C->hd;_tmp57F;});_tmp5C2->r=_tmp9A5;});
_tmp57C=_tmp57C->tl;
for(0;_tmp57C != 0;_tmp57C=_tmp57C->tl){
struct Cyc_Absyn_Decl*_tmp580=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->tag=0U,_tmp581->f1=(struct Cyc_Absyn_Vardecl*)_tmp57C->hd;_tmp581;}),0U);
({void*_tmp9A7=({struct Cyc_Absyn_Decl*_tmp9A6=_tmp580;Cyc_Absyn_decl_stmt(_tmp9A6,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmp9A7;});}
# 3550
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp593=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp577)->f1;_LL35:
# 3553
 Cyc_Toc_fndecl_to_c(nv,_tmp593,0);
Cyc_Toc_stmt_to_c(nv,_tmp5C1);
goto _LL2D;case 4U: _LL36: _tmp596=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp577)->f1;_tmp595=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp577)->f2;_tmp594=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp577)->f3;_LL37: {
# 3557
struct Cyc_Absyn_Stmt*_tmp582=_tmp5C1;
# 3559
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple29 _tmp583=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp584=_tmp583;struct _tuple1*_tmp58B;struct Cyc_Absyn_Exp*_tmp58A;_LL40: _tmp58B=_tmp584.f1;_tmp58A=_tmp584.f2;_LL41:;{
struct _tuple1*x_var=_tmp595->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3565
Cyc_Toc_stmt_to_c(nv,_tmp582);
if(Cyc_Absyn_no_regions)
({void*_tmp9AB=({struct _tuple1*_tmp9AA=x_var;void*_tmp9A9=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmp9A8=
Cyc_Absyn_uint_exp(0U,0U);
# 3567
Cyc_Absyn_declare_stmt(_tmp9AA,_tmp9A9,_tmp9A8,_tmp582,0U);})->r;s->r=_tmp9AB;});else{
# 3569
if((unsigned int)_tmp594){
# 3571
Cyc_Toc_exp_to_c(nv,_tmp594);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmp9AD=_tmp594;Cyc_Absyn_aggrarrow_exp(_tmp9AD,({struct _dyneither_ptr*_tmp586=_cycalloc(sizeof(*_tmp586));({struct _dyneither_ptr _tmp9AC=({const char*_tmp585="h";_tag_dyneither(_tmp585,sizeof(char),2U);});*_tmp586=_tmp9AC;});_tmp586;}),0U);}),0U);
({void*_tmp9AE=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp582,0U))->r;s->r=_tmp9AE;});};}else{
# 3580
({void*_tmp9BB=({
struct _tuple1*_tmp9BA=_tmp58B;void*_tmp9B9=rh_struct_typ;struct Cyc_Absyn_Exp*_tmp9B8=({struct Cyc_Absyn_Exp*_tmp587[1U];({struct Cyc_Absyn_Exp*_tmp9AF=
# 3583
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp587[0]=_tmp9AF;});({struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmp9B0,_tag_dyneither(_tmp587,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3581
Cyc_Absyn_declare_stmt(_tmp9BA,_tmp9B9,_tmp9B8,({
# 3584
struct _tuple1*_tmp9B7=x_var;void*_tmp9B6=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmp9B5=Cyc_Absyn_address_exp(_tmp58A,0U);Cyc_Absyn_declare_stmt(_tmp9B7,_tmp9B6,_tmp9B5,({
struct Cyc_Absyn_Stmt*_tmp9B4=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp588[1U];_tmp588[0]=x_exp;({struct Cyc_Absyn_Exp*_tmp9B1=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmp9B1,_tag_dyneither(_tmp588,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmp9B4,({
struct Cyc_Absyn_Stmt*_tmp9B3=_tmp582;Cyc_Absyn_seq_stmt(_tmp9B3,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp589[1U];_tmp589[0]=x_exp;({struct Cyc_Absyn_Exp*_tmp9B2=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmp9B2,_tag_dyneither(_tmp589,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3580
s->r=_tmp9BB;});}}
# 3590
return;};}default: _LL38: _LL39:
({void*_tmp58C=0U;({struct _dyneither_ptr _tmp9BC=({const char*_tmp58D="bad nested declaration within function";_tag_dyneither(_tmp58D,sizeof(char),39U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp9BC,_tag_dyneither(_tmp58C,sizeof(void*),0U));});});}_LL2D:;}
# 3593
return;case 13U: _LL1B: _tmp5C4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_tmp5C3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp562)->f2;_LL1C:
 s=_tmp5C3;continue;case 14U: _LL1D: _tmp5C6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_tmp5C5=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp562)->f2).f1;_LL1E: {
# 3596
struct _RegionHandle _tmp597=_new_region("temp");struct _RegionHandle*temp=& _tmp597;_push_region(temp);
({struct Cyc_Toc_Env*_tmp9BD=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp9BD,_tmp5C6);});
Cyc_Toc_exp_to_c(nv,_tmp5C5);
# 3600
_npop_handler(0U);return;
# 3596
;_pop_region(temp);}default: _LL1F: _tmp5C9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp562)->f1;_tmp5C8=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp562)->f2;_tmp5C7=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp562)->f3;_LL20: {
# 3614 "toc.cyc"
struct _tuple29 _tmp598=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp599=_tmp598;struct _tuple1*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;_LL43: _tmp5AE=_tmp599.f1;_tmp5AD=_tmp599.f2;_LL44:;{
struct _tuple29 _tmp59A=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp59B=_tmp59A;struct _tuple1*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AB;_LL46: _tmp5AC=_tmp59B.f1;_tmp5AB=_tmp59B.f2;_LL47:;{
struct _tuple29 _tmp59C=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp59D=_tmp59C;struct _tuple1*_tmp5AA;struct Cyc_Absyn_Exp*_tmp5A9;_LL49: _tmp5AA=_tmp59D.f1;_tmp5A9=_tmp59D.f2;_LL4A:;{
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3621
_tmp5AB->topt=e_typ;{
struct _RegionHandle _tmp59E=_new_region("temp");struct _RegionHandle*temp=& _tmp59E;_push_region(temp);
# 3624
Cyc_Toc_stmt_to_c(nv,_tmp5C9);{
struct Cyc_Absyn_Stmt*_tmp59F=({struct Cyc_Absyn_Stmt*_tmp9BF=_tmp5C9;Cyc_Absyn_seq_stmt(_tmp9BF,
Cyc_Absyn_exp_stmt(({void*_tmp5A8=0U;({struct Cyc_Absyn_Exp*_tmp9BE=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmp9BE,_tag_dyneither(_tmp5A8,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3628
struct Cyc_Absyn_Stmt*_tmp5A0=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->tag=10U,_tmp5A7->f1=_tmp5AB,_tmp5A7->f2=_tmp5C8,_tmp5A7->f3=_tmp5C7;_tmp5A7;}),0U);
# 3630
Cyc_Toc_stmt_to_c(nv,_tmp5A0);{
# 3633
struct Cyc_Absyn_Exp*_tmp5A1=({struct Cyc_Absyn_Exp*_tmp5A6[1U];({struct Cyc_Absyn_Exp*_tmp9C0=
Cyc_Toc_member_exp(_tmp5AD,Cyc_Toc_handler_sp,0U);_tmp5A6[0]=_tmp9C0;});({struct Cyc_Absyn_Exp*_tmp9C1=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmp9C1,_tag_dyneither(_tmp5A6,sizeof(struct Cyc_Absyn_Exp*),1U));});});
struct Cyc_Absyn_Stmt*_tmp5A2=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5A5[1U];({struct Cyc_Absyn_Exp*_tmp9C2=Cyc_Absyn_address_exp(_tmp5AD,0U);_tmp5A5[0]=_tmp9C2;});({struct Cyc_Absyn_Exp*_tmp9C3=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmp9C3,_tag_dyneither(_tmp5A5,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
struct Cyc_Absyn_Exp*_tmp5A3=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp5A4=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmp9D3=({
struct _tuple1*_tmp9D2=_tmp5AE;void*_tmp9D1=h_typ;Cyc_Absyn_declare_stmt(_tmp9D2,_tmp9D1,0,({
struct Cyc_Absyn_Stmt*_tmp9D0=_tmp5A2;Cyc_Absyn_seq_stmt(_tmp9D0,({
struct _tuple1*_tmp9CF=_tmp5AA;void*_tmp9CE=was_thrown_typ;struct Cyc_Absyn_Exp*_tmp9CD=_tmp5A3;Cyc_Absyn_declare_stmt(_tmp9CF,_tmp9CE,_tmp9CD,({
struct Cyc_Absyn_Stmt*_tmp9CC=({struct Cyc_Absyn_Exp*_tmp9C5=_tmp5A1;struct Cyc_Absyn_Stmt*_tmp9C4=
Cyc_Absyn_assign_stmt(_tmp5A9,_tmp5A4,0U);
# 3643
Cyc_Absyn_ifthenelse_stmt(_tmp9C5,_tmp9C4,
# 3645
Cyc_Toc_skip_stmt_dl(),0U);});
# 3643
Cyc_Absyn_seq_stmt(_tmp9CC,({
# 3646
struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp5A9,0U);struct Cyc_Absyn_Stmt*_tmp9CA=_tmp59F;Cyc_Absyn_ifthenelse_stmt(_tmp9CB,_tmp9CA,({
# 3648
struct _tuple1*_tmp9C9=_tmp5AC;void*_tmp9C8=e_typ;struct Cyc_Absyn_Exp*_tmp9C7=({
void*_tmp9C6=e_typ;Cyc_Toc_cast_it(_tmp9C6,Cyc_Toc_get_exn_thrown_expression());});
# 3648
Cyc_Absyn_declare_stmt(_tmp9C9,_tmp9C8,_tmp9C7,_tmp5A0,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3639
s->r=_tmp9D3;});};};
# 3652
_npop_handler(0U);return;
# 3622
;_pop_region(temp);};};};};}}_LL0:;}}
# 3661
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmp9D4=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmp9D4;});
({void*_tmp9D5=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmp9D5;});{
struct _RegionHandle _tmp5CA=_new_region("frgn");struct _RegionHandle*frgn=& _tmp5CA;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp5CB=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp5CC=(f->i).args;for(0;_tmp5CC != 0;_tmp5CC=_tmp5CC->tl){
struct _tuple1*_tmp5CD=({struct _tuple1*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->f1=Cyc_Absyn_Loc_n,_tmp5CE->f2=(*((struct _tuple9*)_tmp5CC->hd)).f1;_tmp5CE;});
({void*_tmp9D6=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp5CC->hd)).f3);(*((struct _tuple9*)_tmp5CC->hd)).f3=_tmp9D6;});}}
# 3678
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3682
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp5CF=_cycalloc(sizeof(*_tmp5CF));({struct Cyc_Hashtable_Table*_tmp9D7=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp5CF=_tmp9D7;});_tmp5CF;});
if((unsigned int)(f->i).cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp5D0=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp5D1=_tmp5D0;struct _dyneither_ptr*_tmp5DA;struct Cyc_Absyn_Tqual _tmp5D9;void*_tmp5D8;int _tmp5D7;_LL1: _tmp5DA=_tmp5D1.name;_tmp5D9=_tmp5D1.tq;_tmp5D8=_tmp5D1.type;_tmp5D7=_tmp5D1.inject;_LL2:;{
void*_tmp5D2=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp5D8,Cyc_Absyn_heap_rgn_type,_tmp5D9,Cyc_Absyn_false_type));
struct _tuple1*_tmp5D3=({struct _tuple1*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->f1=Cyc_Absyn_Loc_n,_tmp5D6->f2=(struct _dyneither_ptr*)_check_null(_tmp5DA);_tmp5D6;});
({struct Cyc_List_List*_tmp9DA=({struct Cyc_List_List*_tmp9D9=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9D9,({struct Cyc_List_List*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));({struct _tuple9*_tmp9D8=({struct _tuple9*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->f1=_tmp5DA,_tmp5D4->f2=_tmp5D9,_tmp5D4->f3=_tmp5D2;_tmp5D4;});_tmp5D5->hd=_tmp9D8;}),_tmp5D5->tl=0;_tmp5D5;}));});(f->i).args=_tmp9DA;});
(f->i).cyc_varargs=0;};}
# 3691
{struct Cyc_List_List*_tmp5DB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp5DB != 0;_tmp5DB=_tmp5DB->tl){
({void*_tmp9DB=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp5DB->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp5DB->hd)->type=_tmp9DB;});}}
# 3694
({struct Cyc_Toc_Env*_tmp9DC=Cyc_Toc_clear_toplevel(frgn,_tmp5CB);Cyc_Toc_stmt_to_c(_tmp9DC,f->body);});}
# 3671
;_pop_region(frgn);};}
# 3698
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp5DC=s;switch(_tmp5DC){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3714 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 3716
struct _tuple39 _tmp5DD=*env;struct _tuple39 _tmp5DE=_tmp5DD;struct Cyc_Toc_TocState*_tmp612;struct Cyc_Absyn_Aggrdecl*_tmp611;_LL1: _tmp612=_tmp5DE.f1;_tmp611=*_tmp5DE.f2;_LL2:;{
struct _tuple1*_tmp5DF=_tmp611->name;
struct Cyc_Toc_TocState _tmp5E0=*_tmp612;struct Cyc_Toc_TocState _tmp5E1=_tmp5E0;struct Cyc_Dict_Dict*_tmp610;_LL4: _tmp610=_tmp5E1.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple18**_tmp5E2=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp610,_tmp5DF);
if(_tmp5E2 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if((int)_tmp611->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp5E3=_region_malloc(d,sizeof(*_tmp5E3));_tmp5E3->f1=_tmp611,({void*_tmp9DD=Cyc_Absyn_strctq(_tmp5DF);_tmp5E3->f2=_tmp9DD;});_tmp5E3;});else{
# 3727
v=({struct _tuple18*_tmp5E4=_region_malloc(d,sizeof(*_tmp5E4));_tmp5E4->f1=_tmp611,({void*_tmp9DE=Cyc_Absyn_unionq_type(_tmp5DF);_tmp5E4->f2=_tmp9DE;});_tmp5E4;});}
({struct Cyc_Dict_Dict _tmp9DF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp610,_tmp5DF,v);*_tmp610=_tmp9DF;});};}else{
# 3730
struct _tuple18*_tmp5E5=*_tmp5E2;struct _tuple18*_tmp5E6=_tmp5E5;struct Cyc_Absyn_Aggrdecl*_tmp5E9;void*_tmp5E8;_LL7: _tmp5E9=_tmp5E6->f1;_tmp5E8=_tmp5E6->f2;_LL8:;
if(_tmp5E9->impl == 0){
({struct Cyc_Dict_Dict _tmp9E2=({struct Cyc_Dict_Dict _tmp9E1=*_tmp610;struct _tuple1*_tmp9E0=_tmp5DF;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmp9E1,_tmp9E0,({struct _tuple18*_tmp5E7=_region_malloc(d,sizeof(*_tmp5E7));_tmp5E7->f1=_tmp611,_tmp5E7->f2=_tmp5E8;_tmp5E7;}));});*_tmp610=_tmp9E2;});
seen_defn_before=0;}else{
# 3735
seen_defn_before=1;}}{
# 3737
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->kind=_tmp611->kind,_tmp60F->sc=Cyc_Absyn_Public,_tmp60F->name=_tmp611->name,_tmp60F->tvs=0,_tmp60F->impl=0,_tmp60F->expected_mem_kind=0,_tmp60F->attributes=_tmp611->attributes;_tmp60F;});
# 3744
if(_tmp611->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmp9E3=({struct Cyc_Absyn_AggrdeclImpl*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA->exist_vars=0,_tmp5EA->rgn_po=0,_tmp5EA->fields=0,_tmp5EA->tagged=0;_tmp5EA;});new_ad->impl=_tmp9E3;});{
# 3749
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp5EB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp611->impl))->fields;for(0;_tmp5EB != 0;_tmp5EB=_tmp5EB->tl){
# 3753
struct Cyc_Absyn_Aggrfield*_tmp5EC=(struct Cyc_Absyn_Aggrfield*)_tmp5EB->hd;
void*_tmp5ED=_tmp5EC->type;
struct Cyc_List_List*_tmp5EE=_tmp5EC->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp5ED)) && (
(int)_tmp611->kind == (int)Cyc_Absyn_StructA  && _tmp5EB->tl == 0  || (int)_tmp611->kind == (int)Cyc_Absyn_UnionA)){
# 3767 "toc.cyc"
void*_tmp5EF=Cyc_Tcutil_compress(_tmp5ED);void*_tmp5F0=_tmp5EF;void*_tmp5F8;struct Cyc_Absyn_Tqual _tmp5F7;void*_tmp5F6;unsigned int _tmp5F5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F0)->tag == 4U){_LLA: _tmp5F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F0)->f1).elt_type;_tmp5F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F0)->f1).tq;_tmp5F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F0)->f1).zero_term;_tmp5F5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F0)->f1).zt_loc;_LLB:
# 3770
 _tmp5ED=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->tag=4U,(_tmp5F1->f1).elt_type=_tmp5F8,(_tmp5F1->f1).tq=_tmp5F7,({struct Cyc_Absyn_Exp*_tmp9E4=Cyc_Absyn_uint_exp(0U,0U);(_tmp5F1->f1).num_elts=_tmp9E4;}),(_tmp5F1->f1).zero_term=_tmp5F6,(_tmp5F1->f1).zt_loc=_tmp5F5;_tmp5F1;});
goto _LL9;}else{_LLC: _LLD:
# 3773
 _tmp5EE=({struct Cyc_List_List*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));({void*_tmp9E5=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->tag=6U,_tmp5F2->f1=0;_tmp5F2;});_tmp5F3->hd=_tmp9E5;}),_tmp5F3->tl=_tmp5EE;_tmp5F3;});
_tmp5ED=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->tag=4U,({void*_tmp9E8=Cyc_Toc_void_star_type();(_tmp5F4->f1).elt_type=_tmp9E8;}),({
struct Cyc_Absyn_Tqual _tmp9E7=Cyc_Absyn_empty_tqual(0U);(_tmp5F4->f1).tq=_tmp9E7;}),({
struct Cyc_Absyn_Exp*_tmp9E6=Cyc_Absyn_uint_exp(0U,0U);(_tmp5F4->f1).num_elts=_tmp9E6;}),(_tmp5F4->f1).zero_term=Cyc_Absyn_false_type,(_tmp5F4->f1).zt_loc=0U;_tmp5F4;});}_LL9:;}{
# 3780
struct Cyc_Absyn_Aggrfield*_tmp5F9=({struct Cyc_Absyn_Aggrfield*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->name=_tmp5EC->name,_tmp60B->tq=Cyc_Toc_mt_tq,({
# 3782
void*_tmp9E9=Cyc_Toc_typ_to_c(_tmp5ED);_tmp60B->type=_tmp9E9;}),_tmp60B->width=_tmp5EC->width,_tmp60B->attributes=_tmp5EE,_tmp60B->requires_clause=0;_tmp60B;});
# 3790
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp611->impl))->tagged){
void*_tmp5FA=_tmp5F9->type;
struct _dyneither_ptr*_tmp5FB=_tmp5F9->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp608=({struct Cyc_String_pa_PrintArg_struct _tmp72E;_tmp72E.tag=0U,_tmp72E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp611->name).f2);_tmp72E;});struct Cyc_String_pa_PrintArg_struct _tmp609=({struct Cyc_String_pa_PrintArg_struct _tmp72D;_tmp72D.tag=0U,_tmp72D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5FB);_tmp72D;});void*_tmp606[2U];_tmp606[0]=& _tmp608,_tmp606[1]=& _tmp609;({struct _dyneither_ptr _tmp9EA=({const char*_tmp607="_union_%s_%s";_tag_dyneither(_tmp607,sizeof(char),13U);});Cyc_aprintf(_tmp9EA,_tag_dyneither(_tmp606,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp605=_cycalloc(sizeof(*_tmp605));*_tmp605=s;_tmp605;});
struct Cyc_Absyn_Aggrfield*_tmp5FC=({struct Cyc_Absyn_Aggrfield*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->name=Cyc_Toc_val_sp,_tmp604->tq=Cyc_Toc_mt_tq,_tmp604->type=_tmp5FA,_tmp604->width=0,_tmp604->attributes=0,_tmp604->requires_clause=0;_tmp604;});
struct Cyc_Absyn_Aggrfield*_tmp5FD=({struct Cyc_Absyn_Aggrfield*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->name=Cyc_Toc_tag_sp,_tmp603->tq=Cyc_Toc_mt_tq,_tmp603->type=Cyc_Absyn_sint_type,_tmp603->width=0,_tmp603->attributes=0,_tmp603->requires_clause=0;_tmp603;});
struct Cyc_List_List*_tmp5FE=({struct Cyc_Absyn_Aggrfield*_tmp602[2U];_tmp602[0]=_tmp5FD,_tmp602[1]=_tmp5FC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp602,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp5FF=Cyc_Toc_make_c_struct_defn(str,_tmp5FE);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp601=_cycalloc(sizeof(*_tmp601));({struct Cyc_Absyn_Decl*_tmp9EB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->tag=5U,_tmp600->f1=_tmp5FF;_tmp600;}),0U);_tmp601->hd=_tmp9EB;}),_tmp601->tl=Cyc_Toc_result_decls;_tmp601;});
({void*_tmp9EC=Cyc_Absyn_strct(str);_tmp5F9->type=_tmp9EC;});}
# 3802
new_fields=({struct Cyc_List_List*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->hd=_tmp5F9,_tmp60A->tl=new_fields;_tmp60A;});};}}
# 3804
({struct Cyc_List_List*_tmp9ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmp9ED;});};}
# 3807
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp60E=_cycalloc(sizeof(*_tmp60E));({struct Cyc_Absyn_Decl*_tmp9EF=({struct Cyc_Absyn_Decl*_tmp60D=_cycalloc(sizeof(*_tmp60D));({void*_tmp9EE=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->tag=5U,_tmp60C->f1=new_ad;_tmp60C;});_tmp60D->r=_tmp9EE;}),_tmp60D->loc=0U;_tmp60D;});_tmp60E->hd=_tmp9EF;}),_tmp60E->tl=Cyc_Toc_result_decls;_tmp60E;});
return 0;};};};}
# 3811
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple40{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3839 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3842
struct _tuple40 _tmp613=*env;struct _tuple40 _tmp614=_tmp613;struct Cyc_Set_Set**_tmp625;struct Cyc_Absyn_Datatypedecl*_tmp624;_LL1: _tmp625=(_tmp614.f1)->datatypes_so_far;_tmp624=_tmp614.f2;_LL2:;{
struct _tuple1*_tmp615=_tmp624->name;
if(_tmp624->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp625,_tmp615))
return 0;
({struct Cyc_Set_Set*_tmp9F0=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp625,_tmp615);*_tmp625=_tmp9F0;});
{struct Cyc_List_List*_tmp616=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp624->fields))->v;for(0;_tmp616 != 0;_tmp616=_tmp616->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp616->hd;
# 3850
struct Cyc_List_List*_tmp617=0;
int i=1;
{struct Cyc_List_List*_tmp618=f->typs;for(0;_tmp618 != 0;(_tmp618=_tmp618->tl,i ++)){
struct _dyneither_ptr*_tmp619=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp61A=({struct Cyc_Absyn_Aggrfield*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->name=_tmp619,_tmp61C->tq=(*((struct _tuple12*)_tmp618->hd)).f1,({
void*_tmp9F1=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp618->hd)).f2);_tmp61C->type=_tmp9F1;}),_tmp61C->width=0,_tmp61C->attributes=0,_tmp61C->requires_clause=0;_tmp61C;});
_tmp617=({struct Cyc_List_List*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->hd=_tmp61A,_tmp61B->tl=_tmp617;_tmp61B;});}}
# 3858
_tmp617=({struct Cyc_List_List*_tmp61E=_cycalloc(sizeof(*_tmp61E));({struct Cyc_Absyn_Aggrfield*_tmp9F3=({struct Cyc_Absyn_Aggrfield*_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->name=Cyc_Toc_tag_sp,_tmp61D->tq=Cyc_Toc_mt_tq,_tmp61D->type=Cyc_Absyn_sint_type,_tmp61D->width=0,_tmp61D->attributes=0,_tmp61D->requires_clause=0;_tmp61D;});_tmp61E->hd=_tmp9F3;}),({
struct Cyc_List_List*_tmp9F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp617);_tmp61E->tl=_tmp9F2;});_tmp61E;});{
struct Cyc_Absyn_Aggrdecl*_tmp61F=({struct _dyneither_ptr*_tmp9F5=({struct _dyneither_ptr*_tmp623=_cycalloc(sizeof(*_tmp623));({struct _dyneither_ptr _tmp9F4=({const char*_tmp622="";_tag_dyneither(_tmp622,sizeof(char),1U);});*_tmp623=_tmp9F4;});_tmp623;});Cyc_Toc_make_c_struct_defn(_tmp9F5,_tmp617);});
({struct _tuple1*_tmp9F6=Cyc_Toc_collapse_qvars(f->name,_tmp624->name);_tmp61F->name=_tmp9F6;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp621=_cycalloc(sizeof(*_tmp621));({struct Cyc_Absyn_Decl*_tmp9F7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->tag=5U,_tmp620->f1=_tmp61F;_tmp620;}),0U);_tmp621->hd=_tmp9F7;}),_tmp621->tl=Cyc_Toc_result_decls;_tmp621;});};}}
# 3864
return 0;};}
# 3867
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3886 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3889
struct _tuple40 _tmp626=*env;struct _tuple40 _tmp627=_tmp626;struct Cyc_Toc_TocState*_tmp649;struct Cyc_Absyn_Datatypedecl*_tmp648;_LL1: _tmp649=_tmp627.f1;_tmp648=_tmp627.f2;_LL2:;
if(_tmp648->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp628=*_tmp649;struct Cyc_Toc_TocState _tmp629=_tmp628;struct Cyc_Dict_Dict*_tmp647;_LL4: _tmp647=_tmp629.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp62A=_tmp648->name;
{struct Cyc_List_List*_tmp62B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp648->fields))->v;for(0;_tmp62B != 0;_tmp62B=_tmp62B->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp62B->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp62C=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp62D=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp62C,Cyc_Absyn_false_type,0U);
# 3900
int*_tmp62E=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp647,f->name);int*_tmp62F=_tmp62E;if(_tmp62F == 0){_LL7: _LL8: {
# 3902
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3906
struct Cyc_Absyn_Vardecl*_tmp630=Cyc_Absyn_new_vardecl(0U,f->name,_tmp62D,initopt);
_tmp630->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp632=_cycalloc(sizeof(*_tmp632));({struct Cyc_Absyn_Decl*_tmp9F8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tag=0U,_tmp631->f1=_tmp630;_tmp631;}),0U);_tmp632->hd=_tmp9F8;}),_tmp632->tl=Cyc_Toc_result_decls;_tmp632;});
({struct Cyc_Dict_Dict _tmp9F9=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp647,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3909
*_tmp647=_tmp9F9;});{
# 3911
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp633=f->typs;for(0;_tmp633 != 0;(_tmp633=_tmp633->tl,i ++)){
struct _dyneither_ptr*_tmp634=({struct _dyneither_ptr*_tmp63B=_cycalloc(sizeof(*_tmp63B));({struct _dyneither_ptr _tmp9FB=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp63A=({struct Cyc_Int_pa_PrintArg_struct _tmp72F;_tmp72F.tag=1U,_tmp72F.f1=(unsigned long)i;_tmp72F;});void*_tmp638[1U];_tmp638[0]=& _tmp63A;({struct _dyneither_ptr _tmp9FA=({const char*_tmp639="f%d";_tag_dyneither(_tmp639,sizeof(char),4U);});Cyc_aprintf(_tmp9FA,_tag_dyneither(_tmp638,sizeof(void*),1U));});});*_tmp63B=_tmp9FB;});_tmp63B;});
struct Cyc_Absyn_Aggrfield*_tmp635=({struct Cyc_Absyn_Aggrfield*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->name=_tmp634,_tmp637->tq=(*((struct _tuple12*)_tmp633->hd)).f1,({
void*_tmp9FC=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp633->hd)).f2);_tmp637->type=_tmp9FC;}),_tmp637->width=0,_tmp637->attributes=0,_tmp637->requires_clause=0;_tmp637;});
fields=({struct Cyc_List_List*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->hd=_tmp635,_tmp636->tl=fields;_tmp636;});}}
# 3919
fields=({struct Cyc_List_List*_tmp63D=_cycalloc(sizeof(*_tmp63D));({struct Cyc_Absyn_Aggrfield*_tmp9FF=({struct Cyc_Absyn_Aggrfield*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->name=Cyc_Toc_tag_sp,_tmp63C->tq=Cyc_Toc_mt_tq,({
void*_tmp9FE=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp63C->type=_tmp9FE;}),_tmp63C->width=0,_tmp63C->attributes=0,_tmp63C->requires_clause=0;_tmp63C;});
# 3919
_tmp63D->hd=_tmp9FF;}),({
# 3921
struct Cyc_List_List*_tmp9FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp63D->tl=_tmp9FD;});_tmp63D;});{
struct Cyc_Absyn_Aggrdecl*_tmp63E=({struct _dyneither_ptr*_tmpA01=({struct _dyneither_ptr*_tmp642=_cycalloc(sizeof(*_tmp642));({struct _dyneither_ptr _tmpA00=({const char*_tmp641="";_tag_dyneither(_tmp641,sizeof(char),1U);});*_tmp642=_tmpA00;});_tmp642;});Cyc_Toc_make_c_struct_defn(_tmpA01,fields);});
({struct _tuple1*_tmpA02=Cyc_Toc_collapse_qvars(f->name,_tmp648->name);_tmp63E->name=_tmpA02;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp640=_cycalloc(sizeof(*_tmp640));({struct Cyc_Absyn_Decl*_tmpA03=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->tag=5U,_tmp63F->f1=_tmp63E;_tmp63F;}),0U);_tmp640->hd=_tmpA03;}),_tmp640->tl=Cyc_Toc_result_decls;_tmp640;});
goto _LL6;};};};}}else{if(*((int*)_tmp62F)== 0){_LL9: _LLA:
# 3927
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp643=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp644=Cyc_Absyn_new_vardecl(0U,f->name,_tmp62D,_tmp643);
_tmp644->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp646=_cycalloc(sizeof(*_tmp646));({struct Cyc_Absyn_Decl*_tmpA04=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->tag=0U,_tmp645->f1=_tmp644;_tmp645;}),0U);_tmp646->hd=_tmpA04;}),_tmp646->tl=Cyc_Toc_result_decls;_tmp646;});
({struct Cyc_Dict_Dict _tmpA05=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp647,f->name,1);*_tmp647=_tmpA05;});}
# 3934
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3938
return 0;};};}
# 3941
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3945
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3951
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpA06=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpA06;});
# 3955
if((int)vd->sc == (int)Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 3964
struct _RegionHandle _tmp64A=_new_region("temp");struct _RegionHandle*temp=& _tmp64A;_push_region(temp);
{struct Cyc_Toc_Env*_tmp64B=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp64B,init);}
# 3965
;_pop_region(temp);}else{
# 3969
Cyc_Toc_exp_to_c(nv,init);}}else{
# 3972
void*_tmp64C=Cyc_Tcutil_compress(old_typ);void*_tmp64D=_tmp64C;void*_tmp654;struct Cyc_Absyn_Exp*_tmp653;void*_tmp652;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp64D)->tag == 4U){_LL1: _tmp654=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp64D)->f1).elt_type;_tmp653=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp64D)->f1).num_elts;_tmp652=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp64D)->f1).zero_term;_LL2:
# 3974
 if(Cyc_Tcutil_force_type2bool(0,_tmp652)){
if(_tmp653 == 0)
({void*_tmp64E=0U;({struct _dyneither_ptr _tmpA07=({const char*_tmp64F="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp64F,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA07,_tag_dyneither(_tmp64E,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp653;
struct Cyc_Absyn_Exp*_tmp650=({struct Cyc_Absyn_Exp*_tmpA09=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpA09,({
struct Cyc_Absyn_Exp*_tmpA08=num_elts;Cyc_Absyn_add_exp(_tmpA08,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 3981
struct Cyc_Absyn_Exp*_tmp651=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpA0B=({struct Cyc_Absyn_Stmt*_tmpA0A=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp650,_tmp651,0U),0U);Cyc_Toc_seq_stmt_r(_tmpA0A,
Cyc_Absyn_new_stmt(s->r,0U));});
# 3982
s->r=_tmpA0B;});};}
# 3985
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 3993
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp655=d;struct Cyc_List_List*_tmp65C;struct Cyc_List_List*_tmp65B;void**_tmp65A;struct Cyc_Tcpat_Rhs*_tmp659;switch(*((int*)_tmp655)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp659=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp655)->f1;_LL4:
 _tmp659->rhs=success;return d;default: _LL5: _tmp65C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp655)->f1;_tmp65B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp655)->f2;_tmp65A=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp655)->f3;_LL6:
# 3998
({void*_tmpA0C=Cyc_Toc_rewrite_decision(*_tmp65A,success);*_tmp65A=_tmpA0C;});
for(0;_tmp65B != 0;_tmp65B=_tmp65B->tl){
struct _tuple32*_tmp656=(struct _tuple32*)_tmp65B->hd;struct _tuple32*_tmp657=_tmp656;void**_tmp658;_LL8: _tmp658=(void**)& _tmp657->f2;_LL9:;
({void*_tmpA0D=Cyc_Toc_rewrite_decision(*_tmp658,success);*_tmp658=_tmpA0D;});}
# 4003
return d;}_LL0:;}
# 4014 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4016
struct _RegionHandle _tmp65D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp65D;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp65E=Cyc_Toc_share_env(rgn,nv);
void*_tmp65F=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp65E,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4026
struct Cyc_Absyn_Stmt*_tmp660=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4030
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp660);{
# 4032
struct Cyc_Absyn_Switch_clause*_tmp661=({struct Cyc_Absyn_Switch_clause*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->pattern=p,_tmp66D->pat_vars=0,_tmp66D->where_clause=0,_tmp66D->body=_tmp660,_tmp66D->loc=0U;_tmp66D;});
struct Cyc_List_List*_tmp662=({struct _RegionHandle*_tmpA10=rgn;struct _RegionHandle*_tmpA0F=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpA10,Cyc_Toc_gen_labels,_tmpA0F,({struct Cyc_Absyn_Switch_clause*_tmp66C[1U];_tmp66C[0]=_tmp661;({struct _RegionHandle*_tmpA0E=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpA0E,_tag_dyneither(_tmp66C,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4035
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4039
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp65E,& mydecls,& mybodies,dopt,_tmp662,v);
# 4042
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp663=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp664=_tmp663;struct Cyc_Toc_Env*_tmp666;struct Cyc_Absyn_Stmt*_tmp665;_LL1: _tmp666=_tmp664->f1;_tmp665=_tmp664->f3;_LL2:;
if(_tmp665 == _tmp660){senv=_tmp666;goto FOUND_ENV;}}
# 4047
({void*_tmp667=0U;({struct _dyneither_ptr _tmpA11=({const char*_tmp668="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp668,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA11,_tag_dyneither(_tmp667,sizeof(void*),0U));});});
FOUND_ENV:
# 4051
 Cyc_Toc_stmt_to_c(senv,s);{
# 4053
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4055
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp669=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpA12=Cyc_Toc_temp_var();_tmp669->name=_tmpA12;});
({void*_tmpA13=Cyc_Toc_typ_to_c_array(_tmp669->type);_tmp669->type=_tmpA13;});
res=({struct Cyc_Absyn_Decl*_tmpA14=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->tag=0U,_tmp66A->f1=_tmp669;_tmp66A;}),0U);Cyc_Absyn_decl_stmt(_tmpA14,res,0U);});}
# 4062
res=({struct _tuple1*_tmpA17=v;void*_tmpA16=Cyc_Toc_typ_to_c(_tmp65F);struct Cyc_Absyn_Exp*_tmpA15=e;Cyc_Absyn_declare_stmt(_tmpA17,_tmpA16,_tmpA15,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp66B=res;_npop_handler(0U);return _tmp66B;};};};};}
# 4017
;_pop_region(rgn);}
# 4070
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp66E=e->r;void*_tmp66F=_tmp66E;struct Cyc_Absyn_MallocInfo*_tmp69A;struct Cyc_Absyn_Stmt*_tmp699;void**_tmp698;void**_tmp697;struct Cyc_List_List*_tmp696;struct Cyc_List_List*_tmp695;void**_tmp694;struct Cyc_List_List*_tmp693;void**_tmp692;struct Cyc_Absyn_Exp*_tmp691;struct Cyc_Absyn_Exp*_tmp690;struct Cyc_List_List*_tmp68F;struct Cyc_Absyn_Exp*_tmp68E;struct Cyc_Absyn_Exp*_tmp68D;struct Cyc_Absyn_Exp*_tmp68C;struct Cyc_Absyn_Exp*_tmp68B;struct Cyc_Absyn_Exp*_tmp68A;struct Cyc_Absyn_Exp*_tmp689;struct Cyc_Absyn_Exp*_tmp688;struct Cyc_Absyn_Exp*_tmp687;struct Cyc_Absyn_Exp*_tmp686;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_Absyn_Exp*_tmp684;struct Cyc_Absyn_Exp*_tmp683;struct Cyc_Absyn_Exp*_tmp682;struct Cyc_Absyn_Exp*_tmp681;struct Cyc_Absyn_Exp*_tmp680;struct Cyc_List_List*_tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_Absyn_Exp*_tmp679;struct Cyc_Absyn_Exp*_tmp678;struct Cyc_Absyn_Exp*_tmp677;struct Cyc_Absyn_Exp*_tmp676;switch(*((int*)_tmp66F)){case 41U: _LL1: _tmp676=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL2:
 _tmp677=_tmp676;goto _LL4;case 20U: _LL3: _tmp677=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL4:
 _tmp678=_tmp677;goto _LL6;case 21U: _LL5: _tmp678=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL6:
 _tmp679=_tmp678;goto _LL8;case 22U: _LL7: _tmp679=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL8:
 _tmp67A=_tmp679;goto _LLA;case 15U: _LL9: _tmp67A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LLA:
 _tmp67B=_tmp67A;goto _LLC;case 11U: _LLB: _tmp67B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LLC:
 _tmp67C=_tmp67B;goto _LLE;case 12U: _LLD: _tmp67C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LLE:
 _tmp67D=_tmp67C;goto _LL10;case 18U: _LLF: _tmp67D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL10:
 _tmp67E=_tmp67D;goto _LL12;case 5U: _LL11: _tmp67E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp67E);goto _LL0;case 3U: _LL13: _tmp67F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp67F);goto _LL0;case 7U: _LL15: _tmp681=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_tmp680=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL16:
 _tmp683=_tmp681;_tmp682=_tmp680;goto _LL18;case 8U: _LL17: _tmp683=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_tmp682=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL18:
 _tmp685=_tmp683;_tmp684=_tmp682;goto _LL1A;case 9U: _LL19: _tmp685=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_tmp684=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL1A:
 _tmp687=_tmp685;_tmp686=_tmp684;goto _LL1C;case 23U: _LL1B: _tmp687=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_tmp686=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL1C:
 _tmp689=_tmp687;_tmp688=_tmp686;goto _LL1E;case 35U: _LL1D: _tmp689=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_tmp688=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL1E:
 _tmp68B=_tmp689;_tmp68A=_tmp688;goto _LL20;case 4U: _LL1F: _tmp68B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_tmp68A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp66F)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp68B);Cyc_Toc_exptypes_to_c(_tmp68A);goto _LL0;case 6U: _LL21: _tmp68E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_tmp68D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_tmp68C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66F)->f3;_LL22:
# 4089
 Cyc_Toc_exptypes_to_c(_tmp68E);Cyc_Toc_exptypes_to_c(_tmp68D);Cyc_Toc_exptypes_to_c(_tmp68C);goto _LL0;case 10U: _LL23: _tmp690=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_tmp68F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL24:
# 4091
 Cyc_Toc_exptypes_to_c(_tmp690);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp68F);goto _LL0;case 14U: _LL25: _tmp692=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_tmp691=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL26:
({void*_tmpA18=Cyc_Toc_typ_to_c(*_tmp692);*_tmp692=_tmpA18;});Cyc_Toc_exptypes_to_c(_tmp691);goto _LL0;case 25U: _LL27: _tmp694=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp66F)->f1)->f3;_tmp693=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL28:
# 4094
({void*_tmpA19=Cyc_Toc_typ_to_c(*_tmp694);*_tmp694=_tmpA19;});
_tmp695=_tmp693;goto _LL2A;case 36U: _LL29: _tmp695=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp66F)->f2;_LL2A:
 _tmp696=_tmp695;goto _LL2C;case 26U: _LL2B: _tmp696=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL2C:
# 4098
 for(0;_tmp696 != 0;_tmp696=_tmp696->tl){
struct _tuple19 _tmp670=*((struct _tuple19*)_tmp696->hd);struct _tuple19 _tmp671=_tmp670;struct Cyc_Absyn_Exp*_tmp672;_LL56: _tmp672=_tmp671.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp672);}
# 4102
goto _LL0;case 19U: _LL2D: _tmp697=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL2E:
 _tmp698=_tmp697;goto _LL30;case 17U: _LL2F: _tmp698=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL30:
({void*_tmpA1A=Cyc_Toc_typ_to_c(*_tmp698);*_tmp698=_tmpA1A;});goto _LL0;case 37U: _LL31: _tmp699=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp699);goto _LL0;case 34U: _LL33: _tmp69A=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp66F)->f1;_LL34:
# 4107
 if(_tmp69A->elt_type != 0)
({void**_tmpA1C=({void**_tmp673=_cycalloc(sizeof(*_tmp673));({void*_tmpA1B=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp69A->elt_type)));*_tmp673=_tmpA1B;});_tmp673;});_tmp69A->elt_type=_tmpA1C;});
Cyc_Toc_exptypes_to_c(_tmp69A->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4117
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
# 4128
({void*_tmp674=0U;({unsigned int _tmpA1E=e->loc;struct _dyneither_ptr _tmpA1D=({const char*_tmp675="Cyclone expression within C code";_tag_dyneither(_tmp675,sizeof(char),33U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA1E,_tmpA1D,_tag_dyneither(_tmp674,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4132
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp69B=d->r;void*_tmp69C=_tmp69B;struct Cyc_Absyn_Typedefdecl*_tmp6A6;struct Cyc_Absyn_Enumdecl*_tmp6A5;struct Cyc_Absyn_Aggrdecl*_tmp6A4;struct Cyc_Absyn_Fndecl*_tmp6A3;struct Cyc_Absyn_Vardecl*_tmp6A2;switch(*((int*)_tmp69C)){case 0U: _LL1: _tmp6A2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp69C)->f1;_LL2:
# 4135
({void*_tmpA1F=Cyc_Toc_typ_to_c(_tmp6A2->type);_tmp6A2->type=_tmpA1F;});
if(_tmp6A2->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6A2->initializer));
goto _LL0;case 1U: _LL3: _tmp6A3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp69C)->f1;_LL4:
# 4139
({void*_tmpA20=Cyc_Toc_typ_to_c((_tmp6A3->i).ret_type);(_tmp6A3->i).ret_type=_tmpA20;});
{struct Cyc_List_List*_tmp69D=(_tmp6A3->i).args;for(0;_tmp69D != 0;_tmp69D=_tmp69D->tl){
({void*_tmpA21=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp69D->hd)).f3);(*((struct _tuple9*)_tmp69D->hd)).f3=_tmpA21;});}}
# 4143
goto _LL0;case 5U: _LL5: _tmp6A4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp69C)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp6A4);goto _LL0;case 7U: _LL7: _tmp6A5=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp69C)->f1;_LL8:
# 4146
 if(_tmp6A5->fields != 0){
struct Cyc_List_List*_tmp69E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6A5->fields))->v;for(0;_tmp69E != 0;_tmp69E=_tmp69E->tl){
struct Cyc_Absyn_Enumfield*_tmp69F=(struct Cyc_Absyn_Enumfield*)_tmp69E->hd;
if(_tmp69F->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp69F->tag));}}
# 4151
goto _LL0;case 8U: _LL9: _tmp6A6=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp69C)->f1;_LLA:
({void*_tmpA22=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6A6->defn));_tmp6A6->defn=_tmpA22;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4161
({void*_tmp6A0=0U;({unsigned int _tmpA24=d->loc;struct _dyneither_ptr _tmpA23=({const char*_tmp6A1="Cyclone declaration within C code";_tag_dyneither(_tmp6A1,sizeof(char),34U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA24,_tmpA23,_tag_dyneither(_tmp6A0,sizeof(void*),0U));});});goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 4166
 goto _LL0;}_LL0:;}
# 4170
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp6A7=s->r;void*_tmp6A8=_tmp6A7;struct Cyc_Absyn_Stmt*_tmp6C0;struct Cyc_Absyn_Stmt*_tmp6BF;struct Cyc_Absyn_Exp*_tmp6BE;struct Cyc_Absyn_Decl*_tmp6BD;struct Cyc_Absyn_Stmt*_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BB;struct Cyc_List_List*_tmp6BA;void*_tmp6B9;struct Cyc_Absyn_Exp*_tmp6B8;struct Cyc_Absyn_Exp*_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B6;struct Cyc_Absyn_Stmt*_tmp6B5;struct Cyc_Absyn_Exp*_tmp6B4;struct Cyc_Absyn_Stmt*_tmp6B3;struct Cyc_Absyn_Exp*_tmp6B2;struct Cyc_Absyn_Stmt*_tmp6B1;struct Cyc_Absyn_Stmt*_tmp6B0;struct Cyc_Absyn_Exp*_tmp6AF;struct Cyc_Absyn_Stmt*_tmp6AE;struct Cyc_Absyn_Stmt*_tmp6AD;struct Cyc_Absyn_Exp*_tmp6AC;switch(*((int*)_tmp6A8)){case 1U: _LL1: _tmp6AC=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp6AC);goto _LL0;case 2U: _LL3: _tmp6AE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f1;_tmp6AD=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp6AE);Cyc_Toc_stmttypes_to_c(_tmp6AD);goto _LL0;case 3U: _LL5: _tmp6AF=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f1;_LL6:
 if(_tmp6AF != 0)Cyc_Toc_exptypes_to_c(_tmp6AF);goto _LL0;case 4U: _LL7: _tmp6B2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f1;_tmp6B1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f2;_tmp6B0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f3;_LL8:
# 4176
 Cyc_Toc_exptypes_to_c(_tmp6B2);Cyc_Toc_stmttypes_to_c(_tmp6B1);Cyc_Toc_stmttypes_to_c(_tmp6B0);goto _LL0;case 5U: _LL9: _tmp6B4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f1).f1;_tmp6B3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f2;_LLA:
# 4178
 Cyc_Toc_exptypes_to_c(_tmp6B4);Cyc_Toc_stmttypes_to_c(_tmp6B3);goto _LL0;case 9U: _LLB: _tmp6B8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f1;_tmp6B7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f2).f1;_tmp6B6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f3).f1;_tmp6B5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f4;_LLC:
# 4180
 Cyc_Toc_exptypes_to_c(_tmp6B8);Cyc_Toc_exptypes_to_c(_tmp6B7);Cyc_Toc_exptypes_to_c(_tmp6B6);
Cyc_Toc_stmttypes_to_c(_tmp6B5);goto _LL0;case 10U: _LLD: _tmp6BB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f1;_tmp6BA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f2;_tmp6B9=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f3;_LLE:
# 4183
 Cyc_Toc_exptypes_to_c(_tmp6BB);
for(0;_tmp6BA != 0;_tmp6BA=_tmp6BA->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp6BA->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp6BD=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f1;_tmp6BC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp6BD);Cyc_Toc_stmttypes_to_c(_tmp6BC);goto _LL0;case 14U: _LL11: _tmp6BF=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f1;_tmp6BE=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp6BF);Cyc_Toc_exptypes_to_c(_tmp6BE);goto _LL0;case 13U: _LL13: _tmp6C0=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp6A8)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp6C0);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4193
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4196
({void*_tmpA25=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->tag=0U;_tmp6A9;});s->r=_tmpA25;});
goto _LL0;default: _LL1F: _LL20:
# 4199
({void*_tmp6AA=0U;({unsigned int _tmpA27=s->loc;struct _dyneither_ptr _tmpA26=({const char*_tmp6AB="Cyclone statement in C code";_tag_dyneither(_tmp6AB,sizeof(char),28U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA27,_tmpA26,_tag_dyneither(_tmp6AA,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4203
static void*Cyc_Toc_decl_to_enumtype(struct Cyc_Absyn_Decl*d){
void*_tmp6C1=d->r;void*_tmp6C2=_tmp6C1;struct Cyc_Absyn_Enumdecl*_tmp6C7;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C2)->tag == 7U){_LL1: _tmp6C7=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C2)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4->tag=0U,({void*_tmpA28=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->tag=15U,_tmp6C3->f1=_tmp6C7->name,_tmp6C3->f2=_tmp6C7;_tmp6C3;});_tmp6C4->f1=_tmpA28;}),_tmp6C4->f2=0;_tmp6C4;});}else{_LL3: _LL4:
({void*_tmp6C5=0U;({struct _dyneither_ptr _tmpA29=({const char*_tmp6C6="Toc::get_enumdecl_name";_tag_dyneither(_tmp6C6,sizeof(char),23U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmpA29,_tag_dyneither(_tmp6C5,sizeof(void*),0U));});});}_LL0:;}
# 4210
static struct Cyc_Absyn_Decl*Cyc_Toc_type2enumdecl(void*t){
void*_tmp6C8=t;struct Cyc_Absyn_Enumdecl*_tmp6CC;unsigned int _tmp6CB;if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6C8)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6C8)->f1)->r)->tag == 1U){_LL1: _tmp6CC=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6C8)->f1)->r)->f1;_tmp6CB=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6C8)->f1)->loc;_LL2:
# 4213
 return({struct Cyc_Absyn_Decl*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));({void*_tmpA2A=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9->tag=7U,_tmp6C9->f1=_tmp6CC;_tmp6C9;});_tmp6CA->r=_tmpA2A;}),_tmp6CA->loc=_tmp6CB;_tmp6CA;});}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 4221
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp6CD=0U;({struct _dyneither_ptr _tmpA2B=({const char*_tmp6CE="decls_to_c: not at toplevel!";_tag_dyneither(_tmp6CE,sizeof(char),29U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmpA2B,_tag_dyneither(_tmp6CD,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp6CF=d->r;void*_tmp6D0=_tmp6CF;struct Cyc_List_List*_tmp6EB;struct Cyc_List_List*_tmp6EA;struct Cyc_List_List*_tmp6E9;struct Cyc_List_List*_tmp6E8;struct Cyc_Absyn_Typedefdecl*_tmp6E7;struct Cyc_Absyn_Enumdecl*_tmp6E6;struct Cyc_Absyn_Datatypedecl*_tmp6E5;struct Cyc_Absyn_Aggrdecl*_tmp6E4;struct Cyc_Absyn_Fndecl*_tmp6E3;struct Cyc_Absyn_Vardecl*_tmp6E2;switch(*((int*)_tmp6D0)){case 0U: _LL1: _tmp6E2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6D0)->f1;_LL2: {
# 4229
struct _tuple1*_tmp6D1=_tmp6E2->name;
# 4231
if((int)_tmp6E2->sc == (int)Cyc_Absyn_ExternC)
_tmp6D1=({struct _tuple1*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));({union Cyc_Absyn_Nmspace _tmpA2C=Cyc_Absyn_Abs_n(0,1);_tmp6D2->f1=_tmpA2C;}),_tmp6D2->f2=(*_tmp6D1).f2;_tmp6D2;});
if(_tmp6E2->initializer != 0){
if((int)_tmp6E2->sc == (int)Cyc_Absyn_ExternC)_tmp6E2->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6E2->initializer));else{
# 4238
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp6E2->initializer));}}
# 4240
_tmp6E2->name=_tmp6D1;
({enum Cyc_Absyn_Scope _tmpA2D=Cyc_Toc_scope_to_c(_tmp6E2->sc);_tmp6E2->sc=_tmpA2D;});
({void*_tmpA2E=Cyc_Toc_typ_to_c(_tmp6E2->type);_tmp6E2->type=_tmpA2E;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->hd=d,_tmp6D3->tl=Cyc_Toc_result_decls;_tmp6D3;});
goto _LL0;}case 1U: _LL3: _tmp6E3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6D0)->f1;_LL4: {
# 4246
struct _tuple1*_tmp6D4=_tmp6E3->name;
# 4248
if((int)_tmp6E3->sc == (int)Cyc_Absyn_ExternC){
_tmp6D4=({struct _tuple1*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));({union Cyc_Absyn_Nmspace _tmpA2F=Cyc_Absyn_Abs_n(0,1);_tmp6D5->f1=_tmpA2F;}),_tmp6D5->f2=(*_tmp6D4).f2;_tmp6D5;});
_tmp6E3->sc=Cyc_Absyn_Public;}
# 4252
_tmp6E3->name=_tmp6D4;
Cyc_Toc_fndecl_to_c(nv,_tmp6E3,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6->hd=d,_tmp6D6->tl=Cyc_Toc_result_decls;_tmp6D6;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp6D7=0U;({struct _dyneither_ptr _tmpA30=({const char*_tmp6D8="letdecl at toplevel";_tag_dyneither(_tmp6D8,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA30,_tag_dyneither(_tmp6D7,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp6D9=0U;({struct _dyneither_ptr _tmpA31=({const char*_tmp6DA="region decl at toplevel";_tag_dyneither(_tmp6DA,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA31,_tag_dyneither(_tmp6D9,sizeof(void*),0U));});});case 5U: _LLB: _tmp6E4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6D0)->f1;_LLC:
# 4260
 Cyc_Toc_aggrdecl_to_c(_tmp6E4);
goto _LL0;case 6U: _LLD: _tmp6E5=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp6D0)->f1;_LLE:
# 4263
 if(_tmp6E5->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp6E5);else{
# 4266
Cyc_Toc_datatypedecl_to_c(_tmp6E5);}
goto _LL0;case 7U: _LLF: _tmp6E6=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6D0)->f1;_LL10:
# 4269
 Cyc_Toc_enumdecl_to_c(_tmp6E6);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB->hd=d,_tmp6DB->tl=Cyc_Toc_result_decls;_tmp6DB;});
goto _LL0;case 8U: _LL11: _tmp6E7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6D0)->f1;_LL12:
# 4273
 _tmp6E7->tvs=0;
if(_tmp6E7->defn != 0){
void*_tmp6DC=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6E7->defn));
_tmp6E7->defn=_tmp6DC;{
# 4279
struct Cyc_Absyn_Decl*_tmp6DD=Cyc_Toc_type2enumdecl(_tmp6DC);
if(_tmp6DD != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE->hd=_tmp6DD,_tmp6DE->tl=Cyc_Toc_result_decls;_tmp6DE;});
({void*_tmpA32=Cyc_Toc_decl_to_enumtype(_tmp6DD);_tmp6E7->defn=_tmpA32;});}};}else{
# 4285
enum Cyc_Absyn_KindQual _tmp6DF=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp6E7->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp6E0=_tmp6DF;if(_tmp6E0 == Cyc_Absyn_BoxKind){_LL24: _LL25:
({void*_tmpA33=Cyc_Toc_void_star_type();_tmp6E7->defn=_tmpA33;});goto _LL23;}else{_LL26: _LL27:
 _tmp6E7->defn=Cyc_Absyn_void_type;goto _LL23;}_LL23:;}
# 4293
if(Cyc_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1->hd=d,_tmp6E1->tl=Cyc_Toc_result_decls;_tmp6E1;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
# 4300
 goto _LL0;case 9U: _LL1B: _tmp6E8=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6D0)->f2;_LL1C:
 _tmp6E9=_tmp6E8;goto _LL1E;case 10U: _LL1D: _tmp6E9=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6D0)->f2;_LL1E:
 _tmp6EA=_tmp6E9;goto _LL20;case 11U: _LL1F: _tmp6EA=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6D0)->f1;_LL20:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp6EA,top,cinclude);goto _LL0;default: _LL21: _tmp6EB=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6D0)->f1;_LL22:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp6EB,top,1);goto _LL0;}_LL0:;};}
# 4307
return nv;}
# 4311
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp6EC=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp6ED=_tmp6EC;struct Cyc_Core_DynamicRegion*_tmp6F2;_LL1: _tmp6F2=_tmp6ED.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp6F2->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp6EF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6EF));_tmp6EF->dyn=_tmp6F2,_tmp6EF->state=ts;_tmp6EF;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_dyneither(({unsigned int _tmp6F1=34;struct _dyneither_ptr**_tmp6F0=_cycalloc(_check_times(_tmp6F1,sizeof(struct _dyneither_ptr*)));_tmp6F0[0]=& Cyc_Toc__throw_str,_tmp6F0[1]=& Cyc_Toc_setjmp_str,_tmp6F0[2]=& Cyc_Toc__push_handler_str,_tmp6F0[3]=& Cyc_Toc__pop_handler_str,_tmp6F0[4]=& Cyc_Toc__exn_thrown_str,_tmp6F0[5]=& Cyc_Toc__npop_handler_str,_tmp6F0[6]=& Cyc_Toc__check_null_str,_tmp6F0[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp6F0[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp6F0[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp6F0[10]=& Cyc_Toc__tag_dyneither_str,_tmp6F0[11]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp6F0[12]=& Cyc_Toc__get_dyneither_size_str,_tmp6F0[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp6F0[14]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp6F0[15]=& Cyc_Toc__zero_arr_plus_str,_tmp6F0[16]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp6F0[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp6F0[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp6F0[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp6F0[20]=& Cyc_Toc__cycalloc_str,_tmp6F0[21]=& Cyc_Toc__cyccalloc_str,_tmp6F0[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp6F0[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp6F0[24]=& Cyc_Toc__region_malloc_str,_tmp6F0[25]=& Cyc_Toc__region_calloc_str,_tmp6F0[26]=& Cyc_Toc__check_times_str,_tmp6F0[27]=& Cyc_Toc__new_region_str,_tmp6F0[28]=& Cyc_Toc__push_region_str,_tmp6F0[29]=& Cyc_Toc__pop_region_str,_tmp6F0[30]=& Cyc_Toc__throw_arraybounds_str,_tmp6F0[31]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp6F0[32]=& Cyc_Toc__throw_match_str,_tmp6F0[33]=& Cyc_Toc__fast_region_malloc_str;_tmp6F0;}),sizeof(struct _dyneither_ptr*),34U);};}
# 4359
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp6F3=ts;struct Cyc_Toc_TocStateWrap*_tmp6F4=Cyc_Toc_toc_state;ts=_tmp6F4;Cyc_Toc_toc_state=_tmp6F3;});{
struct Cyc_Toc_TocStateWrap _tmp6F5=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp6F6=_tmp6F5;struct Cyc_Core_DynamicRegion*_tmp6FC;struct Cyc_Toc_TocState*_tmp6FB;_LL1: _tmp6FC=_tmp6F6.dyn;_tmp6FB=_tmp6F6.state;_LL2:;
# 4364
{struct _RegionHandle*h=& _tmp6FC->h;
{struct Cyc_Toc_TocState _tmp6F8=*_tmp6FB;struct Cyc_Toc_TocState _tmp6F9=_tmp6F8;struct Cyc_Xarray_Xarray*_tmp6FA;_LL4: _tmp6FA=_tmp6F9.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp6FA);}
# 4365
;}
# 4368
Cyc_Core_free_rckey(_tmp6FC);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4371
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4377
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4379
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp6FD=_cycalloc(sizeof(*_tmp6FD));*_tmp6FD=pop_tables;_tmp6FD;});
Cyc_Toc_init();
{struct _RegionHandle _tmp6FE=_new_region("start");struct _RegionHandle*start=& _tmp6FE;_push_region(start);
({struct _RegionHandle*_tmpA35=start;struct Cyc_Toc_Env*_tmpA34=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpA35,_tmpA34,ds,1,0);});;_pop_region(start);}
# 4384
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}

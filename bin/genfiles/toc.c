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
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;long double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U,Cyc_Absyn_BoolKind  = 6U,Cyc_Absyn_PtrBndKind  = 7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 323
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
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
({struct _dyneither_ptr _tmp726=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp725=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp725,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp726,ap);});}
# 77
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 79
({struct _dyneither_ptr _tmp728=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp727=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp727,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp728,ap);});}
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
({struct Cyc_List_List**_tmp734=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp734;}),({
struct Cyc_List_List**_tmp733=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp733;}),({
struct Cyc_Dict_Dict*_tmp732=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp731=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp731;});_tmpC;});_tmp11->aggrs_so_far=_tmp732;}),({
struct Cyc_List_List**_tmp730=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp730;}),({
struct Cyc_Set_Set**_tmp72F=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp72E=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp72E;});_tmpE;});_tmp11->datatypes_so_far=_tmp72F;}),({
struct Cyc_Dict_Dict*_tmp72D=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp72C=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp72C;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp72D;}),({
struct Cyc_Dict_Dict*_tmp72B=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp72A=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp72A;});_tmp10;});_tmp11->qvar_tags=_tmp72B;}),({
struct Cyc_Xarray_Xarray*_tmp729=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp729;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 151
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 157
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 160
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp735=({struct Cyc_Toc_TocStateWrap _tmp6F4;_tmp6F4.dyn=_tmp1C,_tmp6F4.state=_tmp1B;_tmp6F4;});*ts=_tmp735;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp1A=({struct _tuple15 _tmp6F5;_tmp6F5.f1=_tmp1B,_tmp6F5.f2=arg;_tmp6F5;});
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
struct _tuple16 env=({struct _tuple16 _tmp6F6;_tmp6F6.f1=q,_tmp6F6.f2=type_maker;_tmp6F6;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp22[5U]="curr";
# 195 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp22,_tmp22,_tmp22 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp23[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp23,_tmp23,_tmp23 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp24[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp24,_tmp24,_tmp24 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp25[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp25,_tmp25,_tmp25 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp26[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp26,_tmp26,_tmp26 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp27[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp27,_tmp27,_tmp27 + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp28[7U]="_throw";
# 214 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp28,_tmp28,_tmp28 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp29[7U]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp29,_tmp29,_tmp29 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp2A[14U]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp2B[13U]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp2B,_tmp2B,_tmp2B + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp2C[12U]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp2C,_tmp2C,_tmp2C + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp2D[14U]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp2D,_tmp2D,_tmp2D + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2E[12U]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp2E,_tmp2E,_tmp2E + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2F[28U]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2F,_tmp2F,_tmp2F + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp30[31U]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp30,_tmp30,_tmp30 + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp31[27U]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp31,_tmp31,_tmp31 + 27U};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0U,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1U,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp32[15U]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp32,_tmp32,_tmp32 + 15U};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0U,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1U,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp33[15U]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp33,_tmp33,_tmp33 + 15U};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0U,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1U,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp34[21U]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp34,_tmp34,_tmp34 + 21U};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0U,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1U,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp35[20U]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp35,_tmp35,_tmp35 + 20U};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0U,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1U,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp36[19U]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp36,_tmp36,_tmp36 + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp37[24U]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp37,_tmp37,_tmp37 + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp38[25U]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp38,_tmp38,_tmp38 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp39[23U]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp39,_tmp39,_tmp39 + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp3A[25U]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp3A,_tmp3A,_tmp3A + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp3B[26U]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp3B,_tmp3B,_tmp3B + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp3C[30U]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp3C,_tmp3C,_tmp3C + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp3D[28U]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp3D,_tmp3D,_tmp3D + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp3E[20U]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp3E,_tmp3E,_tmp3E + 20U};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp3F[15U]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp3F,_tmp3F,_tmp3F + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp40[20U]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp40,_tmp40,_tmp40 + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp41[21U]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp41,_tmp41,_tmp41 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp42[19U]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp42,_tmp42,_tmp42 + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp43[21U]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp43,_tmp43,_tmp43 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp44[22U]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp44,_tmp44,_tmp44 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp45[26U]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp45,_tmp45,_tmp45 + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp46[24U]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp46,_tmp46,_tmp46 + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp47[28U]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp47,_tmp47,_tmp47 + 28U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp48[23U]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp48,_tmp48,_tmp48 + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp49[28U]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp49,_tmp49,_tmp49 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp4A[29U]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp4A,_tmp4A,_tmp4A + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp4B[27U]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp4B,_tmp4B,_tmp4B + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp4C[29U]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp4C,_tmp4C,_tmp4C + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp4D[30U]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp4D,_tmp4D,_tmp4D + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp4E[34U]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp4E,_tmp4E,_tmp4E + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp4F[32U]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp4F,_tmp4F,_tmp4F + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp50[33U]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp50,_tmp50,_tmp50 + 33U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp51[28U]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp51,_tmp51,_tmp51 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp52[33U]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp52,_tmp52,_tmp52 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp53[34U]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp53,_tmp53,_tmp53 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp54[32U]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp54,_tmp54,_tmp54 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp55[34U]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp55,_tmp55,_tmp55 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp56[35U]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp56,_tmp56,_tmp56 + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp57[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp57,_tmp57,_tmp57 + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp58[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp58,_tmp58,_tmp58 + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp59[10U]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp59,_tmp59,_tmp59 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp5A[11U]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp5A,_tmp5A,_tmp5A + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp5B[17U]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp5B,_tmp5B,_tmp5B + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp5C[18U]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp5C,_tmp5C,_tmp5C + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp5D[15U]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp5D,_tmp5D,_tmp5D + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp5E[15U]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp5E,_tmp5E,_tmp5E + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp5F[13U]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp5F,_tmp5F,_tmp5F + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp60[12U]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp60,_tmp60,_tmp60 + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp61[13U]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp61,_tmp61,_tmp61 + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp62[12U]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp62,_tmp62,_tmp62 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp63[19U]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp63,_tmp63,_tmp63 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp64[29U]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp64,_tmp64,_tmp64 + 29U};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0U,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmp65[13U]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmp65,_tmp65,_tmp65 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp66[9U]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmp66,_tmp66,_tmp66 + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp67[20U]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp67,_tmp67,_tmp67 + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 282
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 284
void*Cyc_Toc_void_star_type(){
static void*void_star_t_opt=0;
if(void_star_t_opt == 0)
void_star_t_opt=({void*_tmp738=Cyc_Absyn_void_type;void*_tmp737=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp736=
Cyc_Absyn_empty_tqual(0U);
# 287
Cyc_Absyn_star_type(_tmp738,_tmp737,_tmp736,Cyc_Absyn_false_type);});
# 289
return void_star_t_opt;}
# 291
static void*Cyc_Toc_dyneither_ptr_type(){
static void*dpt=0;
if(dpt == 0)
dpt=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0),0);
return dpt;}
# 297
static void*Cyc_Toc_rgn_type(){
static void*r=0;
if(r == 0)
r=({void*_tmp739=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp739,Cyc_Toc_mt_tq);});
return r;}
# 304
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 308
static struct _tuple19*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp6A=_cycalloc(sizeof(*_tmp6A));({struct Cyc_List_List*_tmp73B=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));({void*_tmp73A=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=1U,_tmp68->f1=name;_tmp68;});_tmp69->hd=_tmp73A;}),_tmp69->tl=0;_tmp69;});_tmp6A->f1=_tmp73B;}),_tmp6A->f2=e;_tmp6A;});}
# 312
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _dyneither_ptr args){
return({struct Cyc_Absyn_Exp*_tmp73C=f;Cyc_Absyn_fncall_exp(_tmp73C,((struct Cyc_List_List*(*)(struct _dyneither_ptr arr))Cyc_List_from_array)(args),0U);});}
# 315
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp6B=e->r;void*_tmp6C=_tmp6B;struct Cyc_Absyn_Exp*_tmp6D;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6C)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6C)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp6D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6C)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp6D);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 323
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=14U,_tmp6E->f1=t,_tmp6E->f2=e,_tmp6E->f3=0,_tmp6E->f4=Cyc_Absyn_No_coercion;_tmp6E;});}
# 326
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=20U,_tmp6F->f1=e;_tmp6F;});}
# 329
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=23U,_tmp70->f1=e1,_tmp70->f2=e2;_tmp70;});}
# 332
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=37U,_tmp71->f1=s;_tmp71;});}
# 335
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=17U,_tmp72->f1=t;_tmp72;});}
# 338
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=10U,_tmp73->f1=e,({struct Cyc_List_List*_tmp73D=((struct Cyc_List_List*(*)(struct _dyneither_ptr arr))Cyc_List_from_array)(es);_tmp73->f2=_tmp73D;}),_tmp73->f3=0,_tmp73->f4=1;_tmp73;});}
# 341
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=2U,_tmp74->f1=s1,_tmp74->f2=s2;_tmp74;});}
# 344
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=6U,_tmp75->f1=e1,_tmp75->f2=e2,_tmp75->f3=e3;_tmp75;});}
# 347
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=21U,_tmp76->f1=e,_tmp76->f2=n,_tmp76->f3=0,_tmp76->f4=0;_tmp76;});}
# 350
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 353
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=36U,_tmp77->f1=tdopt,_tmp77->f2=ds;_tmp77;});}
# 355
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=8U,_tmp78->f1=v;_tmp78;});}
# 358
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 362
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp79=e->r;void*_tmp7A=_tmp79;struct Cyc_Absyn_Exp*_tmp7B;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7A)->tag == 20U){_LL1: _tmp7B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7A)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp7B,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 381
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 390
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 399
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 408
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 419
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp7C=Cyc_Tcutil_compress(t);void*_tmp7D=_tmp7C;enum Cyc_Absyn_Size_of _tmp84;switch(*((int*)_tmp7D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp7D)->f1)){case 1U: _LL1: _tmp84=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp7D)->f1)->f2;_LL2:
# 423
{enum Cyc_Absyn_Size_of _tmp7E=_tmp84;switch(_tmp7E){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp7F=0U;({struct _dyneither_ptr _tmp73E=({const char*_tmp80="impossible IntCon (not char, short or int)";_tag_dyneither(_tmp80,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp73E,_tag_dyneither(_tmp7F,sizeof(void*),0U));});});}_LLD:;}
# 429
goto _LL0;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp7D)->f1)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}default: goto _LLB;}case 3U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp6F7;_tmp6F7.tag=0U,({struct _dyneither_ptr _tmp73F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp6F7.f1=_tmp73F;});_tmp6F7;});void*_tmp81[1U];_tmp81[0]=& _tmp83;({struct _dyneither_ptr _tmp740=({const char*_tmp82="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp82,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp740,_tag_dyneither(_tmp81,sizeof(void*),1U));});});}_LL0:;}
# 436
return function;}
# 438
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp85=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp86=_tmp85;void*_tmp89;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp86)->tag == 3U){_LL1: _tmp89=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp86)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp89);}else{_LL3: _LL4:
({void*_tmp87=0U;({struct _dyneither_ptr _tmp741=({const char*_tmp88="impossible type (not pointer)";_tag_dyneither(_tmp88,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp741,_tag_dyneither(_tmp87,sizeof(void*),0U));});});}_LL0:;}
# 448
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp8A=e->r;void*_tmp8B=_tmp8A;struct Cyc_List_List*_tmp97;struct Cyc_List_List*_tmp96;struct Cyc_List_List*_tmp95;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Exp*_tmp92;long long _tmp91;int _tmp90;short _tmp8F;struct _dyneither_ptr _tmp8E;char _tmp8D;switch(*((int*)_tmp8B)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8B)->f1).Null_c).tag){case 2U: _LL1: _tmp8D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8B)->f1).Char_c).val).f2;_LL2:
 return(int)_tmp8D == (int)'\000';case 3U: _LL3: _tmp8E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8B)->f1).Wchar_c).val;_LL4: {
# 452
unsigned long _tmp8C=Cyc_strlen((struct _dyneither_ptr)_tmp8E);
int i=0;
if(_tmp8C >= (unsigned long)2  && (int)*((const char*)_check_dyneither_subscript(_tmp8E,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_dyneither_subscript(_tmp8E,sizeof(char),1))== (int)'0')i=2;else{
if(((int)*((const char*)_check_dyneither_subscript(_tmp8E,sizeof(char),1))== (int)'x'  && _tmp8C >= (unsigned long)3) && (int)*((const char*)_check_dyneither_subscript(_tmp8E,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp8C;++ i){
if((int)*((const char*)_check_dyneither_subscript(_tmp8E,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 462
return 0;}}case 4U: _LL5: _tmp8F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8B)->f1).Short_c).val).f2;_LL6:
 return(int)_tmp8F == 0;case 5U: _LL7: _tmp90=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8B)->f1).Int_c).val).f2;_LL8:
 return _tmp90 == 0;case 6U: _LL9: _tmp91=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8B)->f1).LongLong_c).val).f2;_LLA:
 return _tmp91 == (long long)0;case 1U: _LLD: _LLE:
# 467
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 466
 goto _LLE;case 14U: _LLF: _tmp92=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8B)->f2;_LL10:
# 468
 return Cyc_Toc_is_zero(_tmp92);case 24U: _LL11: _tmp93=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8B)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp93);case 26U: _LL13: _tmp94=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8B)->f1;_LL14:
 _tmp95=_tmp94;goto _LL16;case 29U: _LL15: _tmp95=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8B)->f3;_LL16:
 _tmp96=_tmp95;goto _LL18;case 25U: _LL17: _tmp96=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8B)->f2;_LL18:
 _tmp97=_tmp96;goto _LL1A;case 36U: _LL19: _tmp97=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8B)->f2;_LL1A:
# 474
 for(0;_tmp97 != 0;_tmp97=_tmp97->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp97->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 482
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmp98=x;union Cyc_Absyn_Nmspace _tmpAA;struct _dyneither_ptr*_tmpA9;_LL1: _tmpAA=_tmp98->f1;_tmpA9=_tmp98->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp99=_tmpAA;struct Cyc_List_List*_tmpA8;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA6;switch((_tmp99.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpA6=0;goto _LL7;case 1U: _LL6: _tmpA6=(_tmp99.Rel_n).val;_LL7:
 _tmpA7=_tmpA6;goto _LL9;case 2U: _LL8: _tmpA7=(_tmp99.Abs_n).val;_LL9:
 _tmpA8=_tmpA7;goto _LLB;default: _LLA: _tmpA8=(_tmp99.C_n).val;_LLB:
# 491
 if(_tmpA8 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp9C=({struct Cyc_String_pa_PrintArg_struct _tmp6F9;_tmp6F9.tag=0U,_tmp6F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp6F9;});struct Cyc_String_pa_PrintArg_struct _tmp9D=({struct Cyc_String_pa_PrintArg_struct _tmp6F8;_tmp6F8.tag=0U,_tmp6F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpA9);_tmp6F8;});void*_tmp9A[2U];_tmp9A[0]=& _tmp9C,_tmp9A[1]=& _tmp9D;({struct _dyneither_ptr _tmp742=({const char*_tmp9B="%s_%s_struct";_tag_dyneither(_tmp9B,sizeof(char),13U);});Cyc_aprintf(_tmp742,_tag_dyneither(_tmp9A,sizeof(void*),2U));});});{
struct _RegionHandle _tmp9E=_new_region("r");struct _RegionHandle*r=& _tmp9E;_push_region(r);
{struct _dyneither_ptr _tmpA5=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp6FC;_tmp6FC.tag=0U,_tmp6FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp6FC;});struct Cyc_String_pa_PrintArg_struct _tmpA2=({struct Cyc_String_pa_PrintArg_struct _tmp6FB;_tmp6FB.tag=0U,({struct _dyneither_ptr _tmp745=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp744=r;struct Cyc_List_List*_tmp743=_tmpA8;Cyc_rstr_sepstr(_tmp744,_tmp743,({const char*_tmpA4="_";_tag_dyneither(_tmpA4,sizeof(char),2U);}));}));_tmp6FB.f1=_tmp745;});_tmp6FB;});struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp6FA;_tmp6FA.tag=0U,_tmp6FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpA9);_tmp6FA;});void*_tmp9F[3U];_tmp9F[0]=& _tmpA1,_tmp9F[1]=& _tmpA2,_tmp9F[2]=& _tmpA3;({struct _dyneither_ptr _tmp746=({const char*_tmpA0="%s_%s_%s_struct";_tag_dyneither(_tmpA0,sizeof(char),16U);});Cyc_aprintf(_tmp746,_tag_dyneither(_tmp9F,sizeof(void*),3U));});});_npop_handler(0U);return _tmpA5;};_pop_region(r);};}_LL3:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 504
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 507
struct _tuple20 _tmpAB=*env;struct _tuple20 _tmpAC=_tmpAB;struct Cyc_Dict_Dict*_tmpBF;struct _tuple14*_tmpBE;_LL1: _tmpBF=(_tmpAC.f1)->qvar_tags;_tmpBE=_tmpAC.f2;_LL2:;{
struct _tuple14 _tmpAD=*_tmpBE;struct _tuple14 _tmpAE=_tmpAD;struct _tuple1*_tmpBD;struct _tuple1*_tmpBC;_LL4: _tmpBD=_tmpAE.f1;_tmpBC=_tmpAE.f2;_LL5:;{
struct _handler_cons _tmpAF;_push_handler(& _tmpAF);{int _tmpB1=0;if(setjmp(_tmpAF.handler))_tmpB1=1;if(!_tmpB1){{struct _tuple1*_tmpB2=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmpBF,Cyc_Toc_qvar_tag_cmp,_tmpBE);_npop_handler(0U);return _tmpB2;};_pop_handler();}else{void*_tmpB0=(void*)_exn_thrown;void*_tmpB3=_tmpB0;void*_tmpBB;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpB3)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 511
struct _tuple14*_tmpB4=({struct _tuple14*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=_tmpBD,_tmpBA->f2=_tmpBC;_tmpBA;});
struct _tuple1*_tmpB5=_tmpBD;union Cyc_Absyn_Nmspace _tmpB9;struct _dyneither_ptr*_tmpB8;_LLC: _tmpB9=_tmpB5->f1;_tmpB8=_tmpB5->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpB8,_tmpBC);
struct _tuple1*res=({struct _tuple1*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->f1=_tmpB9,({struct _dyneither_ptr*_tmp747=({struct _dyneither_ptr*_tmpB6=_cycalloc(sizeof(*_tmpB6));*_tmpB6=newvar;_tmpB6;});_tmpB7->f2=_tmp747;});_tmpB7;});
({struct Cyc_Dict_Dict _tmp748=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpBF,_tmpB4,res);*_tmpBF=_tmp748;});
return res;};}}else{_LL9: _tmpBB=_tmpB3;_LLA:(int)_rethrow(_tmpBB);}_LL6:;}};};};}
# 519
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp6FD;_tmp6FD.f1=fieldname,_tmp6FD.f2=dtname;_tmp6FD;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 526
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->kind=Cyc_Absyn_StructA,_tmpC2->sc=Cyc_Absyn_Public,_tmpC2->tvs=0,_tmpC2->attributes=0,_tmpC2->expected_mem_kind=0,({
# 529
struct _tuple1*_tmp74B=({struct _tuple1*_tmpC0=_cycalloc(sizeof(*_tmpC0));({union Cyc_Absyn_Nmspace _tmp74A=Cyc_Absyn_Rel_n(0);_tmpC0->f1=_tmp74A;}),_tmpC0->f2=name;_tmpC0;});_tmpC2->name=_tmp74B;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp749=({struct Cyc_Absyn_AggrdeclImpl*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->exist_vars=0,_tmpC1->rgn_po=0,_tmpC1->tagged=0,_tmpC1->fields=fs;_tmpC1;});_tmpC2->impl=_tmp749;});_tmpC2;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 538
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 541
struct _tuple21 _tmpC3=*env;struct _tuple21 _tmpC4=_tmpC3;struct Cyc_List_List**_tmpDA;struct Cyc_List_List*_tmpD9;_LL1: _tmpDA=(_tmpC4.f1)->tuple_types;_tmpD9=_tmpC4.f2;_LL2:;
# 543
{struct Cyc_List_List*_tmpC5=*_tmpDA;for(0;_tmpC5 != 0;_tmpC5=_tmpC5->tl){
struct _tuple22*_tmpC6=(struct _tuple22*)_tmpC5->hd;struct _tuple22*_tmpC7=_tmpC6;void*_tmpCA;struct Cyc_List_List*_tmpC9;_LL4: _tmpCA=_tmpC7->f1;_tmpC9=_tmpC7->f2;_LL5:;{
struct Cyc_List_List*_tmpC8=_tmpD9;
for(0;_tmpC8 != 0  && _tmpC9 != 0;(_tmpC8=_tmpC8->tl,_tmpC9=_tmpC9->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpC8->hd)).f2,(void*)_tmpC9->hd))
break;}
if(_tmpC8 == 0  && _tmpC9 == 0)
return _tmpCA;};}}{
# 554
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct _dyneither_ptr _tmp74D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD7=({struct Cyc_Int_pa_PrintArg_struct _tmp6FE;_tmp6FE.tag=1U,_tmp6FE.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp6FE;});void*_tmpD5[1U];_tmpD5[0]=& _tmpD7;({struct _dyneither_ptr _tmp74C=({const char*_tmpD6="_tuple%d";_tag_dyneither(_tmpD6,sizeof(char),9U);});Cyc_aprintf(_tmp74C,_tag_dyneither(_tmpD5,sizeof(void*),1U));});});*_tmpD8=_tmp74D;});_tmpD8;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpD9);
struct Cyc_List_List*_tmpCB=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmpCB=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));({struct Cyc_Absyn_Aggrfield*_tmp74F=({struct Cyc_Absyn_Aggrfield*_tmpCC=_cycalloc(sizeof(*_tmpCC));({struct _dyneither_ptr*_tmp74E=Cyc_Absyn_fieldname(i);_tmpCC->name=_tmp74E;}),_tmpCC->tq=Cyc_Toc_mt_tq,_tmpCC->type=(void*)ts2->hd,_tmpCC->width=0,_tmpCC->attributes=0,_tmpCC->requires_clause=0;_tmpCC;});_tmpCD->hd=_tmp74F;}),_tmpCD->tl=_tmpCB;_tmpCD;});}}
_tmpCB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCB);{
struct Cyc_Absyn_Aggrdecl*_tmpCE=Cyc_Toc_make_c_struct_defn(xname,_tmpCB);
void*_tmpCF=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpD4=_cycalloc(sizeof(*_tmpD4));*_tmpD4=_tmpCE;_tmpD4;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));({struct Cyc_Absyn_Decl*_tmp750=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->tag=5U,_tmpD0->f1=_tmpCE;_tmpD0;}),0U);_tmpD1->hd=_tmp750;}),_tmpD1->tl=Cyc_Toc_result_decls;_tmpD1;});
({struct Cyc_List_List*_tmp752=({struct Cyc_List_List*_tmpD3=_region_malloc(d,sizeof(*_tmpD3));({struct _tuple22*_tmp751=({struct _tuple22*_tmpD2=_region_malloc(d,sizeof(*_tmpD2));_tmpD2->f1=_tmpCF,_tmpD2->f2=ts;_tmpD2;});_tmpD3->hd=_tmp751;}),_tmpD3->tl=*_tmpDA;_tmpD3;});*_tmpDA=_tmp752;});
return _tmpCF;};};}
# 567
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 572
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 575
struct _tuple24*_tmpDB=env;struct Cyc_List_List**_tmpEF;enum Cyc_Absyn_AggrKind _tmpEE;struct Cyc_List_List*_tmpED;_LL1: _tmpEF=(_tmpDB->f1)->anon_aggr_types;_tmpEE=(_tmpDB->f2)->f1;_tmpED=(_tmpDB->f2)->f2;_LL2:;
# 577
{struct Cyc_List_List*_tmpDC=*_tmpEF;for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){
struct _tuple25*_tmpDD=(struct _tuple25*)_tmpDC->hd;struct _tuple25*_tmpDE=_tmpDD;void*_tmpE1;enum Cyc_Absyn_AggrKind _tmpE0;struct Cyc_List_List*_tmpDF;_LL4: _tmpE1=_tmpDE->f1;_tmpE0=_tmpDE->f2;_tmpDF=_tmpDE->f3;_LL5:;
if((int)_tmpEE != (int)_tmpE0)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpDF,_tmpED))
return _tmpE1;}}{
# 586
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpEC=_cycalloc(sizeof(*_tmpEC));({struct _dyneither_ptr _tmp754=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpEB=({struct Cyc_Int_pa_PrintArg_struct _tmp6FF;_tmp6FF.tag=1U,_tmp6FF.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp6FF;});void*_tmpE9[1U];_tmpE9[0]=& _tmpEB;({struct _dyneither_ptr _tmp753=({const char*_tmpEA="_tuple%d";_tag_dyneither(_tmpEA,sizeof(char),9U);});Cyc_aprintf(_tmp753,_tag_dyneither(_tmpE9,sizeof(void*),1U));});});*_tmpEC=_tmp754;});_tmpEC;});
struct Cyc_Absyn_Aggrdecl*_tmpE2=Cyc_Toc_make_c_struct_defn(xname,_tmpED);
_tmpE2->kind=_tmpEE;{
void*_tmpE3=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE8=_cycalloc(sizeof(*_tmpE8));*_tmpE8=_tmpE2;_tmpE8;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpE5=_cycalloc(sizeof(*_tmpE5));({struct Cyc_Absyn_Decl*_tmp755=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->tag=5U,_tmpE4->f1=_tmpE2;_tmpE4;}),0U);_tmpE5->hd=_tmp755;}),_tmpE5->tl=Cyc_Toc_result_decls;_tmpE5;});
({struct Cyc_List_List*_tmp757=({struct Cyc_List_List*_tmpE7=_region_malloc(d,sizeof(*_tmpE7));({struct _tuple25*_tmp756=({struct _tuple25*_tmpE6=_region_malloc(d,sizeof(*_tmpE6));_tmpE6->f1=_tmpE3,_tmpE6->f2=_tmpEE,_tmpE6->f3=_tmpED;_tmpE6;});_tmpE7->hd=_tmp756;}),_tmpE7->tl=*_tmpEF;_tmpE7;});*_tmpEF=_tmp757;});
return _tmpE3;};};}
# 594
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(({struct _tuple23*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->f1=ak,_tmpF0->f2=fs;_tmpF0;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple26{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple27{struct Cyc_Toc_TocState*f1;struct _tuple26*f2;};struct _tuple28{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 603
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple27*env){
# 612
struct _tuple27 _tmpF1=*env;struct _tuple27 _tmpF2=_tmpF1;struct Cyc_List_List**_tmp115;struct _tuple1*_tmp114;struct Cyc_List_List*_tmp113;struct Cyc_List_List*_tmp112;struct Cyc_List_List*_tmp111;_LL1: _tmp115=(_tmpF2.f1)->abs_struct_types;_tmp114=(_tmpF2.f2)->f1;_tmp113=(_tmpF2.f2)->f2;_tmp112=(_tmpF2.f2)->f3;_tmp111=(_tmpF2.f2)->f4;_LL2:;
# 616
{struct Cyc_List_List*_tmpF3=*_tmp115;for(0;_tmpF3 != 0;_tmpF3=_tmpF3->tl){
struct _tuple28*_tmpF4=(struct _tuple28*)_tmpF3->hd;struct _tuple28*_tmpF5=_tmpF4;struct _tuple1*_tmpFD;struct Cyc_List_List*_tmpFC;void*_tmpFB;_LL4: _tmpFD=_tmpF5->f1;_tmpFC=_tmpF5->f2;_tmpFB=_tmpF5->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmpFD,_tmp114)== 0  && ({
int _tmp758=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp112);_tmp758 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFC);})){
int okay=1;
{struct Cyc_List_List*_tmpF6=_tmp112;for(0;_tmpF6 != 0;(_tmpF6=_tmpF6->tl,_tmpFC=_tmpFC->tl)){
void*_tmpF7=(void*)_tmpF6->hd;
void*_tmpF8=(void*)((struct Cyc_List_List*)_check_null(_tmpFC))->hd;
{struct Cyc_Absyn_Kind*_tmpF9=Cyc_Tcutil_type_kind(_tmpF7);struct Cyc_Absyn_Kind*_tmpFA=_tmpF9;switch(((struct Cyc_Absyn_Kind*)_tmpFA)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 627
 continue;default: _LLB: _LLC:
# 630
 if(Cyc_Tcutil_unify(_tmpF7,_tmpF8) || ({void*_tmp759=Cyc_Toc_typ_to_c(_tmpF7);Cyc_Tcutil_unify(_tmp759,Cyc_Toc_typ_to_c(_tmpF8));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 635
break;}}
# 637
if(okay)
return _tmpFB;}}}{
# 644
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp110=_cycalloc(sizeof(*_tmp110));({struct _dyneither_ptr _tmp75B=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp10F=({struct Cyc_Int_pa_PrintArg_struct _tmp700;_tmp700.tag=1U,_tmp700.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp700;});void*_tmp10D[1U];_tmp10D[0]=& _tmp10F;({struct _dyneither_ptr _tmp75A=({const char*_tmp10E="_tuple%d";_tag_dyneither(_tmp10E,sizeof(char),9U);});Cyc_aprintf(_tmp75A,_tag_dyneither(_tmp10D,sizeof(void*),1U));});});*_tmp110=_tmp75B;});_tmp110;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmpFE=0;
# 648
({struct Cyc_List_List*_tmp75D=({struct Cyc_List_List*_tmp100=_region_malloc(d,sizeof(*_tmp100));({struct _tuple28*_tmp75C=({struct _tuple28*_tmpFF=_region_malloc(d,sizeof(*_tmpFF));_tmpFF->f1=_tmp114,_tmpFF->f2=_tmp112,_tmpFF->f3=x;_tmpFF;});_tmp100->hd=_tmp75C;}),_tmp100->tl=*_tmp115;_tmp100;});*_tmp115=_tmp75D;});{
# 651
struct _RegionHandle _tmp101=_new_region("r");struct _RegionHandle*r=& _tmp101;_push_region(r);
{struct Cyc_List_List*_tmp102=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp113,_tmp112);
for(0;_tmp111 != 0;_tmp111=_tmp111->tl){
struct Cyc_Absyn_Aggrfield*_tmp103=(struct Cyc_Absyn_Aggrfield*)_tmp111->hd;
void*t=_tmp103->type;
struct Cyc_List_List*atts=_tmp103->attributes;
# 661
if((_tmp111->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t))) && !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp105=_cycalloc(sizeof(*_tmp105));({void*_tmp75E=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=6U,_tmp104->f1=0;_tmp104;});_tmp105->hd=_tmp75E;}),_tmp105->tl=atts;_tmp105;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp102,t));
# 668
if(Cyc_Tcutil_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=4U,({void*_tmp761=Cyc_Toc_void_star_type();(_tmp106->f1).elt_type=_tmp761;}),({struct Cyc_Absyn_Tqual _tmp760=Cyc_Absyn_empty_tqual(0U);(_tmp106->f1).tq=_tmp760;}),({
struct Cyc_Absyn_Exp*_tmp75F=Cyc_Absyn_uint_exp(0U,0U);(_tmp106->f1).num_elts=_tmp75F;}),(_tmp106->f1).zero_term=Cyc_Absyn_false_type,(_tmp106->f1).zt_loc=0U;_tmp106;});
# 672
_tmpFE=({struct Cyc_List_List*_tmp108=_cycalloc(sizeof(*_tmp108));({struct Cyc_Absyn_Aggrfield*_tmp762=({struct Cyc_Absyn_Aggrfield*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->name=_tmp103->name,_tmp107->tq=Cyc_Toc_mt_tq,_tmp107->type=t,_tmp107->width=_tmp103->width,_tmp107->attributes=atts,_tmp107->requires_clause=0;_tmp107;});_tmp108->hd=_tmp762;}),_tmp108->tl=_tmpFE;_tmp108;});}
# 674
_tmpFE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpFE);{
struct Cyc_Absyn_Aggrdecl*_tmp109=Cyc_Toc_make_c_struct_defn(xname,_tmpFE);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp10B=_cycalloc(sizeof(*_tmp10B));({struct Cyc_Absyn_Decl*_tmp763=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->tag=5U,_tmp10A->f1=_tmp109;_tmp10A;}),0U);_tmp10B->hd=_tmp763;}),_tmp10B->tl=Cyc_Toc_result_decls;_tmp10B;});{
void*_tmp10C=x;_npop_handler(0U);return _tmp10C;};};}
# 652
;_pop_region(r);};};}
# 680
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 684
struct _tuple26 env=({struct _tuple26 _tmp701;_tmp701.f1=struct_name,_tmp701.f2=type_vars,_tmp701.f3=type_args,_tmp701.f4=fields;_tmp701;});
return((void*(*)(struct _tuple26*arg,void*(*f)(struct _RegionHandle*,struct _tuple27*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 692
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp766=({struct _dyneither_ptr*_tmp119=_cycalloc(sizeof(*_tmp119));({struct _dyneither_ptr _tmp765=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp118=({struct Cyc_Int_pa_PrintArg_struct _tmp702;_tmp702.tag=1U,_tmp702.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp702;});void*_tmp116[1U];_tmp116[0]=& _tmp118;({struct _dyneither_ptr _tmp764=({const char*_tmp117="_tmp%X";_tag_dyneither(_tmp117,sizeof(char),7U);});Cyc_aprintf(_tmp764,_tag_dyneither(_tmp116,sizeof(void*),1U));});});*_tmp119=_tmp765;});_tmp119;});_tmp11A->f2=_tmp766;});_tmp11A;});}struct _tuple29{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 695
struct _tuple29 Cyc_Toc_temp_var_and_exp(){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple29 _tmp703;_tmp703.f1=v,({struct Cyc_Absyn_Exp*_tmp767=Cyc_Absyn_var_exp(v,0U);_tmp703.f2=_tmp767;});_tmp703;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 702
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp11B=*env;struct _tuple30 _tmp11C=_tmp11B;struct Cyc_Xarray_Xarray*_tmp124;_LL1: _tmp124=(_tmp11C.f1)->temp_labels;_LL2:;{
int _tmp11D=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp768=_tmp11D;_tmp768 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp124);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp124,_tmp11D);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp123=_cycalloc(sizeof(*_tmp123));({struct _dyneither_ptr _tmp76A=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp122=({struct Cyc_Int_pa_PrintArg_struct _tmp704;_tmp704.tag=1U,_tmp704.f1=(unsigned int)_tmp11D;_tmp704;});void*_tmp120[1U];_tmp120[0]=& _tmp122;({struct _dyneither_ptr _tmp769=({const char*_tmp121="_LL%X";_tag_dyneither(_tmp121,sizeof(char),6U);});Cyc_aprintf(_tmp769,_tag_dyneither(_tmp120,sizeof(void*),1U));});});*_tmp123=_tmp76A;});_tmp123;});
if(({int _tmp76B=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp124,res);_tmp76B != _tmp11D;}))
({void*_tmp11E=0U;({struct _dyneither_ptr _tmp76C=({const char*_tmp11F="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp11F,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp76C,_tag_dyneither(_tmp11E,sizeof(void*),0U));});});
return res;};};}
# 712
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 718
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp125=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp125))->hd)->name)!= 0){
++ ans;
_tmp125=_tmp125->tl;}
# 725
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 731
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp126=*a;struct _tuple9 _tmp127=_tmp126;struct _dyneither_ptr*_tmp12B;struct Cyc_Absyn_Tqual _tmp12A;void*_tmp129;_LL1: _tmp12B=_tmp127.f1;_tmp12A=_tmp127.f2;_tmp129=_tmp127.f3;_LL2:;
return({struct _tuple9*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->f1=_tmp12B,_tmp128->f2=_tmp12A,({void*_tmp76D=Cyc_Toc_typ_to_c(_tmp129);_tmp128->f3=_tmp76D;});_tmp128;});}
# 754 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp12C=Cyc_Tcutil_compress(t);void*_tmp12D=_tmp12C;void*_tmp133;void*_tmp132;struct Cyc_Absyn_Tqual _tmp131;struct Cyc_Absyn_Exp*_tmp130;void*_tmp12F;unsigned int _tmp12E;switch(*((int*)_tmp12D)){case 4U: _LL1: _tmp132=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12D)->f1).elt_type;_tmp131=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12D)->f1).tq;_tmp130=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12D)->f1).num_elts;_tmp12F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12D)->f1).zero_term;_tmp12E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12D)->f1).zt_loc;_LL2:
# 757
 return({void*_tmp76E=Cyc_Toc_typ_to_c_array(_tmp132);Cyc_Absyn_cstar_type(_tmp76E,_tmp131);});case 1U: _LL3: _tmp133=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp12D)->f2;if(_tmp133 != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp133);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 763
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 765
struct Cyc_Absyn_Aggrfield*_tmp134=({struct Cyc_Absyn_Aggrfield*_tmp135=_cycalloc(sizeof(*_tmp135));*_tmp135=*f;_tmp135;});
_tmp134->type=new_type;
_tmp134->requires_clause=0;
_tmp134->tq=Cyc_Toc_mt_tq;
return _tmp134;}
# 772
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 774
return;}
# 777
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp136=Cyc_Tcutil_compress(t);void*_tmp137=_tmp136;struct Cyc_Absyn_Tvar*_tmp138;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp137)->tag == 2U){_LL1: _tmp138=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp137)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 783
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp139=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp13A=_tmp139;if(((struct Cyc_Absyn_Kind*)_tmp13A)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 789
static int Cyc_Toc_is_void_star(void*t){
void*_tmp13B=Cyc_Tcutil_compress(t);void*_tmp13C=_tmp13B;void*_tmp13D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13C)->tag == 3U){_LL1: _tmp13D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13C)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_void_type(_tmp13D);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 795
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 798
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 802
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp13E=t;struct Cyc_Absyn_Datatypedecl*_tmp187;void**_tmp186;struct Cyc_Absyn_Enumdecl*_tmp185;struct Cyc_Absyn_Aggrdecl*_tmp184;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp182;struct _tuple1*_tmp181;struct Cyc_List_List*_tmp180;struct Cyc_Absyn_Typedefdecl*_tmp17F;void*_tmp17E;enum Cyc_Absyn_AggrKind _tmp17D;struct Cyc_List_List*_tmp17C;struct Cyc_List_List*_tmp17B;struct Cyc_Absyn_Tqual _tmp17A;void*_tmp179;struct Cyc_List_List*_tmp178;int _tmp177;struct Cyc_Absyn_VarargInfo*_tmp176;struct Cyc_List_List*_tmp175;void*_tmp174;struct Cyc_Absyn_Tqual _tmp173;struct Cyc_Absyn_Exp*_tmp172;unsigned int _tmp171;void*_tmp170;struct Cyc_Absyn_Tqual _tmp16F;void*_tmp16E;struct Cyc_Absyn_Tvar*_tmp16D;void**_tmp16C;struct Cyc_List_List*_tmp16B;struct _tuple1*_tmp16A;void*_tmp169;union Cyc_Absyn_AggrInfo _tmp168;struct Cyc_List_List*_tmp167;struct Cyc_Absyn_Datatypedecl*_tmp166;struct Cyc_Absyn_Datatypefield*_tmp165;switch(*((int*)_tmp13E)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 817
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp166=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)->f1).KnownDatatypefield).val).f1;_tmp165=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)->f1).KnownDatatypefield).val).f2;_LLA:
# 819
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp165->name,_tmp166->name));}else{_LLB: _LLC:
# 821
({void*_tmp13F=0U;({struct _dyneither_ptr _tmp76F=({const char*_tmp140="unresolved DatatypeFieldType";_tag_dyneither(_tmp140,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp76F,_tag_dyneither(_tmp13F,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 830
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp169=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1;_tmp168=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)->f1;_tmp167=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f2;_LL1C:
# 877
{union Cyc_Absyn_AggrInfo _tmp150=_tmp168;if((_tmp150.UnknownAggr).tag == 1){_LL45: _LL46:
# 879
 if(_tmp167 == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->tag=0U,_tmp151->f1=_tmp169,_tmp151->f2=0;_tmp151;});}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 883
struct Cyc_Absyn_Aggrdecl*_tmp152=Cyc_Absyn_get_known_aggrdecl(_tmp168);
if(_tmp152->expected_mem_kind){
# 886
if(_tmp152->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp155=({struct Cyc_String_pa_PrintArg_struct _tmp706;_tmp706.tag=0U,({
struct _dyneither_ptr _tmp770=(struct _dyneither_ptr)((int)_tmp152->kind == (int)Cyc_Absyn_UnionA?({const char*_tmp157="union";_tag_dyneither(_tmp157,sizeof(char),6U);}):({const char*_tmp158="struct";_tag_dyneither(_tmp158,sizeof(char),7U);}));_tmp706.f1=_tmp770;});_tmp706;});struct Cyc_String_pa_PrintArg_struct _tmp156=({struct Cyc_String_pa_PrintArg_struct _tmp705;_tmp705.tag=0U,({
struct _dyneither_ptr _tmp771=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp152->name));_tmp705.f1=_tmp771;});_tmp705;});void*_tmp153[2U];_tmp153[0]=& _tmp155,_tmp153[1]=& _tmp156;({struct _dyneither_ptr _tmp772=({const char*_tmp154="%s %s was never defined.";_tag_dyneither(_tmp154,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp772,_tag_dyneither(_tmp153,sizeof(void*),2U));});});}
# 892
if((int)_tmp152->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp152->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp159=_tmp152->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp152->impl))->fields;
if(_tmp159 == 0)return Cyc_Toc_aggrdecl_type(_tmp152->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp159))->tl != 0;_tmp159=_tmp159->tl){;}{
void*_tmp15A=((struct Cyc_Absyn_Aggrfield*)_tmp159->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp15A))){
if(_tmp152->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp707;_tmp707.tag=0U,({struct _dyneither_ptr _tmp773=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp152->name));_tmp707.f1=_tmp773;});_tmp707;});void*_tmp15B[1U];_tmp15B[0]=& _tmp15D;({struct _dyneither_ptr _tmp774=({const char*_tmp15C="struct %s ended up being abstract.";_tag_dyneither(_tmp15C,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp774,_tag_dyneither(_tmp15B,sizeof(void*),1U));});});{
# 905
struct _RegionHandle _tmp15E=_new_region("r");struct _RegionHandle*r=& _tmp15E;_push_region(r);
{struct Cyc_List_List*_tmp15F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp152->tvs,_tmp167);
void*_tmp160=Cyc_Tcutil_rsubstitute(r,_tmp15F,_tmp15A);
if(Cyc_Toc_is_abstract_type(_tmp160)){void*_tmp161=Cyc_Toc_aggrdecl_type(_tmp152->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp161;}{
void*_tmp162=Cyc_Toc_add_struct_type(_tmp152->name,_tmp152->tvs,_tmp167,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp152->impl))->fields);_npop_handler(0U);return _tmp162;};}
# 906
;_pop_region(r);};}
# 911
return Cyc_Toc_aggrdecl_type(_tmp152->name,Cyc_Absyn_strctq);};};};case 15U: _LL1D: _tmp16A=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)->f1;_LL1E:
 return t;case 16U: _LL1F: _tmp16B=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp16B);return t;case 4U: _LL23: _LL24:
# 923
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 928
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp16C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp13E)->f2;_LL4:
# 806
 if(*_tmp16C == 0){
*_tmp16C=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 810
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp16C));case 2U: _LL5: _tmp16D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13E)->f1;_LL6:
# 812
 if((int)(Cyc_Tcutil_tvar_kind(_tmp16D,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_AnyKind)
# 814
return Cyc_Absyn_void_type;else{
# 816
return Cyc_Toc_void_star_type();}case 3U: _LLD: _tmp170=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13E)->f1).elt_type;_tmp16F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13E)->f1).elt_tq;_tmp16E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13E)->f1).ptr_atts).bounds;_LLE:
# 825
 _tmp170=Cyc_Toc_typ_to_c(_tmp170);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp16E)== 0)
return Cyc_Toc_dyneither_ptr_type();else{
# 829
return Cyc_Absyn_star_type(_tmp170,Cyc_Absyn_heap_rgn_type,_tmp16F,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp174=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13E)->f1).elt_type;_tmp173=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13E)->f1).tq;_tmp172=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13E)->f1).num_elts;_tmp171=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13E)->f1).zt_loc;_LL14:
# 833
 return({void*_tmp778=Cyc_Toc_typ_to_c(_tmp174);struct Cyc_Absyn_Tqual _tmp777=_tmp173;struct Cyc_Absyn_Exp*_tmp776=_tmp172;void*_tmp775=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp778,_tmp777,_tmp776,_tmp775,_tmp171);});case 5U: _LL15: _tmp17A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13E)->f1).ret_tqual;_tmp179=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13E)->f1).ret_type;_tmp178=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13E)->f1).args;_tmp177=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13E)->f1).c_varargs;_tmp176=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13E)->f1).cyc_varargs;_tmp175=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13E)->f1).attributes;_LL16: {
# 839
struct Cyc_List_List*_tmp141=0;
for(0;_tmp175 != 0;_tmp175=_tmp175->tl){
void*_tmp142=(void*)_tmp175->hd;void*_tmp143=_tmp142;switch(*((int*)_tmp143)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
 _tmp141=({struct Cyc_List_List*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->hd=(void*)_tmp175->hd,_tmp144->tl=_tmp141;_tmp144;});goto _LL35;}_LL35:;}{
# 850
struct Cyc_List_List*_tmp145=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp178);
if(_tmp176 != 0){
# 853
void*_tmp146=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp176->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp147=({struct _tuple9*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->f1=_tmp176->name,_tmp149->f2=_tmp176->tq,_tmp149->f3=_tmp146;_tmp149;});
_tmp145=({struct Cyc_List_List*_tmp779=_tmp145;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp779,({struct Cyc_List_List*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->hd=_tmp147,_tmp148->tl=0;_tmp148;}));});}
# 857
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->tag=5U,(_tmp14A->f1).tvars=0,(_tmp14A->f1).effect=0,(_tmp14A->f1).ret_tqual=_tmp17A,({void*_tmp77A=Cyc_Toc_typ_to_c(_tmp179);(_tmp14A->f1).ret_type=_tmp77A;}),(_tmp14A->f1).args=_tmp145,(_tmp14A->f1).c_varargs=_tmp177,(_tmp14A->f1).cyc_varargs=0,(_tmp14A->f1).rgn_po=0,(_tmp14A->f1).attributes=_tmp141,(_tmp14A->f1).requires_clause=0,(_tmp14A->f1).requires_relns=0,(_tmp14A->f1).ensures_clause=0,(_tmp14A->f1).ensures_relns=0;_tmp14A;});};}case 6U: _LL17: _tmp17B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp13E)->f1;_LL18: {
# 862
struct Cyc_List_List*_tmp14B=0;
for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
_tmp14B=({struct Cyc_List_List*_tmp14D=_cycalloc(sizeof(*_tmp14D));({struct _tuple12*_tmp77C=({struct _tuple12*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->f1=(*((struct _tuple12*)_tmp17B->hd)).f1,({void*_tmp77B=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp17B->hd)).f2);_tmp14C->f2=_tmp77B;});_tmp14C;});_tmp14D->hd=_tmp77C;}),_tmp14D->tl=_tmp14B;_tmp14D;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14B));}case 7U: _LL19: _tmp17D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13E)->f1;_tmp17C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13E)->f2;_LL1A: {
# 871
struct Cyc_List_List*_tmp14E=0;
for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
_tmp14E=({struct Cyc_List_List*_tmp14F=_cycalloc(sizeof(*_tmp14F));({struct Cyc_Absyn_Aggrfield*_tmp77E=({struct Cyc_Absyn_Aggrfield*_tmp77D=(struct Cyc_Absyn_Aggrfield*)_tmp17C->hd;Cyc_Toc_aggrfield_to_c(_tmp77D,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp17C->hd)->type));});_tmp14F->hd=_tmp77E;}),_tmp14F->tl=_tmp14E;_tmp14F;});}
return({enum Cyc_Absyn_AggrKind _tmp77F=_tmp17D;Cyc_Toc_add_anon_aggr_type(_tmp77F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14E));});}case 8U: _LL21: _tmp181=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13E)->f1;_tmp180=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13E)->f2;_tmp17F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13E)->f3;_tmp17E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13E)->f4;_LL22:
# 917
 if(_tmp17E == 0){
if(_tmp180 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->tag=8U,_tmp163->f1=_tmp181,_tmp163->f2=0,_tmp163->f3=_tmp17F,_tmp163->f4=0;_tmp163;});else{
return t;}}else{
# 922
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->tag=8U,_tmp164->f1=_tmp181,_tmp164->f2=0,_tmp164->f3=_tmp17F,({void*_tmp780=Cyc_Toc_typ_to_c(_tmp17E);_tmp164->f4=_tmp780;});_tmp164;});}case 9U: _LL2B: _tmp182=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp13E)->f1;_LL2C:
# 933
 return t;case 11U: _LL2D: _tmp183=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp13E)->f1;_LL2E:
# 938
 if(_tmp183->topt != 0)return Cyc_Toc_typ_to_c((void*)_check_null(_tmp183->topt));else{
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13E)->f1)->r)){case 0U: _LL2F: _tmp184=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13E)->f1)->r)->f1;_LL30:
# 941
 Cyc_Toc_aggrdecl_to_c(_tmp184);
if((int)_tmp184->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp184->name,Cyc_Absyn_unionq_type);else{
return Cyc_Toc_aggrdecl_type(_tmp184->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp185=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13E)->f1)->r)->f1;_LL32:
# 946
 Cyc_Toc_enumdecl_to_c(_tmp185);
return t;default: _LL33: _tmp187=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13E)->f1)->r)->f1;_tmp186=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13E)->f2;_LL34:
# 949
 Cyc_Toc_datatypedecl_to_c(_tmp187);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp186)));}}_LL0:;}
# 954
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp188=t;void*_tmp18A;struct Cyc_Absyn_Tqual _tmp189;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188)->tag == 4U){_LL1: _tmp18A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188)->f1).elt_type;_tmp189=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188)->f1).tq;_LL2:
# 957
 return({void*_tmp781=Cyc_Absyn_star_type(_tmp18A,Cyc_Absyn_heap_rgn_type,_tmp189,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp781,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 964
static int Cyc_Toc_atomic_type(void*t){
void*_tmp18B=Cyc_Tcutil_compress(t);void*_tmp18C=_tmp18B;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp19E;void*_tmp19D;void*_tmp19C;struct Cyc_List_List*_tmp19B;switch(*((int*)_tmp18C)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp19C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->f1;_tmp19B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->f2;_LL4: {
# 968
void*_tmp18D=_tmp19C;struct Cyc_Absyn_Datatypedecl*_tmp197;struct Cyc_Absyn_Datatypefield*_tmp196;union Cyc_Absyn_AggrInfo _tmp195;switch(*((int*)_tmp18D)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D:
 return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp195=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp18D)->f1;_LL25:
# 976
{union Cyc_Absyn_AggrInfo _tmp18E=_tmp195;if((_tmp18E.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 980
struct Cyc_Absyn_Aggrdecl*_tmp18F=Cyc_Absyn_get_known_aggrdecl(_tmp195);
if(_tmp18F->impl == 0)
return 0;
{struct Cyc_List_List*_tmp190=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp18F->impl))->fields;for(0;_tmp190 != 0;_tmp190=_tmp190->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp190->hd)->type))return 0;}}
return 1;};case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp18D)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp197=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp18D)->f1).KnownDatatypefield).val).f1;_tmp196=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp18D)->f1).KnownDatatypefield).val).f2;_LL27:
# 987
{struct Cyc_List_List*_tmp191=_tmp196->typs;for(0;_tmp191 != 0;_tmp191=_tmp191->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp191->hd)).f2))return 0;}}
return 1;}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp194=({struct Cyc_String_pa_PrintArg_struct _tmp708;_tmp708.tag=0U,({struct _dyneither_ptr _tmp782=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp708.f1=_tmp782;});_tmp708;});void*_tmp192[1U];_tmp192[0]=& _tmp194;({struct _dyneither_ptr _tmp783=({const char*_tmp193="atomic_typ:  bad type %s";_tag_dyneither(_tmp193,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp783,_tag_dyneither(_tmp192,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 992
 return 1;case 4U: _LL7: _tmp19D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18C)->f1).elt_type;_LL8:
 return Cyc_Toc_atomic_type(_tmp19D);case 7U: _LL9: _tmp19E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp18C)->f2;_LLA:
# 1000
 for(0;_tmp19E != 0;_tmp19E=_tmp19E->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp19E->hd)->type))return 0;}
return 1;case 6U: _LLB: _tmp19F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp18C)->f1;_LLC:
# 1004
 for(0;_tmp19F != 0;_tmp19F=_tmp19F->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp19F->hd)).f2))return 0;}
return 1;case 3U: _LLD: _LLE:
# 1009
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp19A=({struct Cyc_String_pa_PrintArg_struct _tmp709;_tmp709.tag=0U,({struct _dyneither_ptr _tmp784=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp709.f1=_tmp784;});_tmp709;});void*_tmp198[1U];_tmp198[0]=& _tmp19A;({struct _dyneither_ptr _tmp785=({const char*_tmp199="atomic_typ:  bad type %s";_tag_dyneither(_tmp199,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp785,_tag_dyneither(_tmp198,sizeof(void*),1U));});});}_LL0:;}
# 1015
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A0=Cyc_Tcutil_compress(t);void*_tmp1A1=_tmp1A0;struct Cyc_List_List*_tmp1AD;union Cyc_Absyn_AggrInfo _tmp1AC;switch(*((int*)_tmp1A1)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A1)->f1)->tag == 20U){_LL1: _tmp1AC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A1)->f1)->f1;_LL2: {
# 1018
struct Cyc_Absyn_Aggrdecl*_tmp1A2=Cyc_Absyn_get_known_aggrdecl(_tmp1AC);
if(_tmp1A2->impl == 0)
({void*_tmp1A3=0U;({struct _dyneither_ptr _tmp786=({const char*_tmp1A4="is_poly_field: type missing fields";_tag_dyneither(_tmp1A4,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp786,_tag_dyneither(_tmp1A3,sizeof(void*),0U));});});
_tmp1AD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1A2->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1AD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A1)->f2;_LL4: {
# 1023
struct Cyc_Absyn_Aggrfield*_tmp1A5=Cyc_Absyn_lookup_field(_tmp1AD,f);
if(_tmp1A5 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1A8=({struct Cyc_String_pa_PrintArg_struct _tmp70A;_tmp70A.tag=0U,_tmp70A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp70A;});void*_tmp1A6[1U];_tmp1A6[0]=& _tmp1A8;({struct _dyneither_ptr _tmp787=({const char*_tmp1A7="is_poly_field: bad field %s";_tag_dyneither(_tmp1A7,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp787,_tag_dyneither(_tmp1A6,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1A5->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1AB=({struct Cyc_String_pa_PrintArg_struct _tmp70B;_tmp70B.tag=0U,({struct _dyneither_ptr _tmp788=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp70B.f1=_tmp788;});_tmp70B;});void*_tmp1A9[1U];_tmp1A9[0]=& _tmp1AB;({struct _dyneither_ptr _tmp789=({const char*_tmp1AA="is_poly_field: bad type %s";_tag_dyneither(_tmp1AA,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp789,_tag_dyneither(_tmp1A9,sizeof(void*),1U));});});}_LL0:;}
# 1034
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1AE=e->r;void*_tmp1AF=_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B9;struct _dyneither_ptr*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B7;struct _dyneither_ptr*_tmp1B6;switch(*((int*)_tmp1AF)){case 21U: _LL1: _tmp1B7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1AF)->f1;_tmp1B6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1AF)->f2;_LL2:
# 1037
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1B7->topt),_tmp1B6) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1B9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1AF)->f1;_tmp1B8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1AF)->f2;_LL4: {
# 1040
void*_tmp1B0=Cyc_Tcutil_compress((void*)_check_null(_tmp1B9->topt));void*_tmp1B1=_tmp1B0;void*_tmp1B5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1)->tag == 3U){_LL8: _tmp1B5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1)->f1).elt_type;_LL9:
# 1042
 return Cyc_Toc_is_poly_field(_tmp1B5,_tmp1B8) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp70C;_tmp70C.tag=0U,({struct _dyneither_ptr _tmp78A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1B9->topt)));_tmp70C.f1=_tmp78A;});_tmp70C;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp78B=({const char*_tmp1B3="is_poly_project: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp78B,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1045
 return 0;}_LL0:;}
# 1050
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1BA[1U];_tmp1BA[0]=s;({struct Cyc_Absyn_Exp*_tmp78C=fn_e;Cyc_Toc_fncall_exp_dl(_tmp78C,_tag_dyneither(_tmp1BA,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1055
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1BB[2U];_tmp1BB[0]=s,_tmp1BB[1]=n;({struct Cyc_Absyn_Exp*_tmp78D=fn_e;Cyc_Toc_fncall_exp_dl(_tmp78D,_tag_dyneither(_tmp1BB,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1060
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1BC[2U];_tmp1BC[0]=rgn,_tmp1BC[1]=s;({struct Cyc_Absyn_Exp*_tmp78E=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp78E,_tag_dyneither(_tmp1BC,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1064
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1BD[2U];_tmp1BD[0]=rgn,_tmp1BD[1]=s;({struct Cyc_Absyn_Exp*_tmp78F=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp78F,_tag_dyneither(_tmp1BD,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1068
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1BE[3U];_tmp1BE[0]=rgn,_tmp1BE[1]=s,_tmp1BE[2]=n;({struct Cyc_Absyn_Exp*_tmp790=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp790,_tag_dyneither(_tmp1BE,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1072
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({void*_tmp1BF=0U;({struct Cyc_Absyn_Exp*_tmp791=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp791,_tag_dyneither(_tmp1BF,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1077
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1085
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1C0=e->r;void*_tmp1C1=_tmp1C0;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C1)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C1)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1090
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));({struct Cyc_Absyn_Decl*_tmp792=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->tag=0U,_tmp1C2->f1=vd;_tmp1C2;}),0U);_tmp1C3->hd=_tmp792;}),_tmp1C3->tl=Cyc_Toc_result_decls;_tmp1C3;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1098
 xexp=({void*_tmp793=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp793,e);});
# 1100
xplussz=({void*_tmp794=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp794,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1103
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1C4[3U];({struct _tuple19*_tmp797=({struct _tuple19*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->f1=0,_tmp1C5->f2=xexp;_tmp1C5;});_tmp1C4[0]=_tmp797;}),({
struct _tuple19*_tmp796=({struct _tuple19*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->f1=0,_tmp1C6->f2=xexp;_tmp1C6;});_tmp1C4[1]=_tmp796;}),({
struct _tuple19*_tmp795=({struct _tuple19*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->f1=0,_tmp1C7->f2=xplussz;_tmp1C7;});_tmp1C4[2]=_tmp795;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C4,sizeof(struct _tuple19*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1138 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1C9=_region_malloc(r,sizeof(*_tmp1C9));_tmp1C9->break_lab=0,_tmp1C9->continue_lab=0,_tmp1C9->fallthru_info=0,_tmp1C9->toplevel=1,({int*_tmp798=({int*_tmp1C8=_region_malloc(r,sizeof(*_tmp1C8));*_tmp1C8=0;_tmp1C8;});_tmp1C9->in_lhs=_tmp798;}),_tmp1C9->rgn=r;_tmp1C9;});}
# 1142
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1CA=nv;int _tmp1CB;_LL1: _tmp1CB=_tmp1CA->toplevel;_LL2:;
return _tmp1CB;}
# 1146
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1CC=e;struct _dyneither_ptr**_tmp1D2;struct _dyneither_ptr**_tmp1D1;struct Cyc_Toc_FallthruInfo*_tmp1D0;int _tmp1CF;int*_tmp1CE;_LL1: _tmp1D2=_tmp1CC->break_lab;_tmp1D1=_tmp1CC->continue_lab;_tmp1D0=_tmp1CC->fallthru_info;_tmp1CF=_tmp1CC->toplevel;_tmp1CE=_tmp1CC->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1CD=_region_malloc(r,sizeof(*_tmp1CD));_tmp1CD->break_lab=_tmp1D2,_tmp1CD->continue_lab=_tmp1D1,_tmp1CD->fallthru_info=_tmp1D0,_tmp1CD->toplevel=0,_tmp1CD->in_lhs=_tmp1CE,_tmp1CD->rgn=r;_tmp1CD;});}
# 1150
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D3=e;struct _dyneither_ptr**_tmp1D9;struct _dyneither_ptr**_tmp1D8;struct Cyc_Toc_FallthruInfo*_tmp1D7;int _tmp1D6;int*_tmp1D5;_LL1: _tmp1D9=_tmp1D3->break_lab;_tmp1D8=_tmp1D3->continue_lab;_tmp1D7=_tmp1D3->fallthru_info;_tmp1D6=_tmp1D3->toplevel;_tmp1D5=_tmp1D3->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1D4=_region_malloc(r,sizeof(*_tmp1D4));_tmp1D4->break_lab=_tmp1D9,_tmp1D4->continue_lab=_tmp1D8,_tmp1D4->fallthru_info=_tmp1D7,_tmp1D4->toplevel=1,_tmp1D4->in_lhs=_tmp1D5,_tmp1D4->rgn=r;_tmp1D4;});}
# 1154
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1DA=nv;int*_tmp1DB;_LL1: _tmp1DB=_tmp1DA->in_lhs;_LL2:;
return*_tmp1DB;}
# 1158
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1DC=e;int*_tmp1DD;_LL1: _tmp1DD=_tmp1DC->in_lhs;_LL2:;
*_tmp1DD=b;}
# 1163
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DE=e;struct _dyneither_ptr**_tmp1E4;struct _dyneither_ptr**_tmp1E3;struct Cyc_Toc_FallthruInfo*_tmp1E2;int _tmp1E1;int*_tmp1E0;_LL1: _tmp1E4=_tmp1DE->break_lab;_tmp1E3=_tmp1DE->continue_lab;_tmp1E2=_tmp1DE->fallthru_info;_tmp1E1=_tmp1DE->toplevel;_tmp1E0=_tmp1DE->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DF=_region_malloc(r,sizeof(*_tmp1DF));_tmp1DF->break_lab=_tmp1E4,_tmp1DF->continue_lab=_tmp1E3,_tmp1DF->fallthru_info=_tmp1E2,_tmp1DF->toplevel=_tmp1E1,_tmp1DF->in_lhs=_tmp1E0,_tmp1DF->rgn=r;_tmp1DF;});}
# 1170
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E5=e;struct _dyneither_ptr**_tmp1EB;struct _dyneither_ptr**_tmp1EA;struct Cyc_Toc_FallthruInfo*_tmp1E9;int _tmp1E8;int*_tmp1E7;_LL1: _tmp1EB=_tmp1E5->break_lab;_tmp1EA=_tmp1E5->continue_lab;_tmp1E9=_tmp1E5->fallthru_info;_tmp1E8=_tmp1E5->toplevel;_tmp1E7=_tmp1E5->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E6=_region_malloc(r,sizeof(*_tmp1E6));_tmp1E6->break_lab=0,_tmp1E6->continue_lab=0,_tmp1E6->fallthru_info=_tmp1E9,_tmp1E6->toplevel=_tmp1E8,_tmp1E6->in_lhs=_tmp1E7,_tmp1E6->rgn=r;_tmp1E6;});}
# 1176
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1180
struct Cyc_Toc_Env*_tmp1EC=e;struct _dyneither_ptr**_tmp1F4;struct _dyneither_ptr**_tmp1F3;struct Cyc_Toc_FallthruInfo*_tmp1F2;int _tmp1F1;int*_tmp1F0;_LL1: _tmp1F4=_tmp1EC->break_lab;_tmp1F3=_tmp1EC->continue_lab;_tmp1F2=_tmp1EC->fallthru_info;_tmp1F1=_tmp1EC->toplevel;_tmp1F0=_tmp1EC->in_lhs;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1EF=_region_malloc(r,sizeof(*_tmp1EF));
_tmp1EF->label=fallthru_l,_tmp1EF->binders=fallthru_binders;_tmp1EF;});
return({struct Cyc_Toc_Env*_tmp1EE=_region_malloc(r,sizeof(*_tmp1EE));({struct _dyneither_ptr**_tmp799=({struct _dyneither_ptr**_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));*_tmp1ED=break_l;_tmp1ED;});_tmp1EE->break_lab=_tmp799;}),_tmp1EE->continue_lab=_tmp1F3,_tmp1EE->fallthru_info=fi,_tmp1EE->toplevel=_tmp1F1,_tmp1EE->in_lhs=_tmp1F0,_tmp1EE->rgn=r;_tmp1EE;});};}
# 1185
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1187
struct Cyc_Toc_Env*_tmp1F5=e;struct _dyneither_ptr**_tmp1FC;struct _dyneither_ptr**_tmp1FB;struct Cyc_Toc_FallthruInfo*_tmp1FA;int _tmp1F9;int*_tmp1F8;_LL1: _tmp1FC=_tmp1F5->break_lab;_tmp1FB=_tmp1F5->continue_lab;_tmp1FA=_tmp1F5->fallthru_info;_tmp1F9=_tmp1F5->toplevel;_tmp1F8=_tmp1F5->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1F7=_region_malloc(r,sizeof(*_tmp1F7));({struct _dyneither_ptr**_tmp79A=({struct _dyneither_ptr**_tmp1F6=_region_malloc(r,sizeof(*_tmp1F6));*_tmp1F6=break_l;_tmp1F6;});_tmp1F7->break_lab=_tmp79A;}),_tmp1F7->continue_lab=_tmp1FB,_tmp1F7->fallthru_info=0,_tmp1F7->toplevel=_tmp1F9,_tmp1F7->in_lhs=_tmp1F8,_tmp1F7->rgn=r;_tmp1F7;});}
# 1194
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1197
struct Cyc_Toc_Env*_tmp1FD=e;struct _dyneither_ptr**_tmp204;struct _dyneither_ptr**_tmp203;struct Cyc_Toc_FallthruInfo*_tmp202;int _tmp201;int*_tmp200;_LL1: _tmp204=_tmp1FD->break_lab;_tmp203=_tmp1FD->continue_lab;_tmp202=_tmp1FD->fallthru_info;_tmp201=_tmp1FD->toplevel;_tmp200=_tmp1FD->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1FF=_region_malloc(r,sizeof(*_tmp1FF));_tmp1FF->break_lab=0,_tmp1FF->continue_lab=_tmp203,({struct Cyc_Toc_FallthruInfo*_tmp79B=({struct Cyc_Toc_FallthruInfo*_tmp1FE=_region_malloc(r,sizeof(*_tmp1FE));_tmp1FE->label=next_l,_tmp1FE->binders=0;_tmp1FE;});_tmp1FF->fallthru_info=_tmp79B;}),_tmp1FF->toplevel=_tmp201,_tmp1FF->in_lhs=_tmp200,_tmp1FF->rgn=r;_tmp1FF;});}
# 1210 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1213
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp205=e->annot;void*_tmp206=_tmp205;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({void*_tmp207=0U;({unsigned int _tmp79D=e->loc;struct _dyneither_ptr _tmp79C=({const char*_tmp208="Toc: do_null_check";_tag_dyneither(_tmp208,sizeof(char),19U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp79D,_tmp79C,_tag_dyneither(_tmp207,sizeof(void*),0U));});});}}}}}}_LL0:;}
# 1229
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1231
int ans;
int _tmp209=Cyc_Toc_in_lhs(nv);
void*_tmp20A=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));
void*_tmp20B=Cyc_Toc_typ_to_c(_tmp20A);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp20C=_tmp20A;void*_tmp220;struct Cyc_Absyn_Tqual _tmp21F;void*_tmp21E;void*_tmp21D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20C)->tag == 3U){_LL1: _tmp220=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20C)->f1).elt_type;_tmp21F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20C)->f1).elt_tq;_tmp21E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20C)->f1).ptr_atts).bounds;_tmp21D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20C)->f1).ptr_atts).zero_term;_LL2:
# 1242
{void*_tmp20D=annot;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp219;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1244
 if(!((unsigned int)({void*_tmp79E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp79E,_tmp21E);})))
({void*_tmp7A2=({void*_tmp7A1=({void*_tmp79F=Cyc_Toc_typ_to_c(_tmp220);Cyc_Absyn_cstar_type(_tmp79F,_tmp21F);});Cyc_Toc_cast_it_r(_tmp7A1,({
struct Cyc_Absyn_Exp*_tmp7A0=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7A0,Cyc_Toc_curr_sp,0U);}));});
# 1245
ptr->r=_tmp7A2;});
# 1247
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1250
 if(!((unsigned int)({void*_tmp7A3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7A3,_tmp21E);})))
goto _LLB;
# 1253
({void*_tmp7A7=({void*_tmp7A6=_tmp20B;Cyc_Toc_cast_it_r(_tmp7A6,({struct Cyc_Absyn_Exp*_tmp20E[1U];({struct Cyc_Absyn_Exp*_tmp7A4=Cyc_Absyn_new_exp(ptr->r,0U);_tmp20E[0]=_tmp7A4;});({struct Cyc_Absyn_Exp*_tmp7A5=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp7A5,_tag_dyneither(_tmp20E,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp7A7;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1259
void*ta1=Cyc_Toc_typ_to_c(_tmp220);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp7AC=({void*_tmp7AB=Cyc_Absyn_cstar_type(ta1,_tmp21F);Cyc_Toc_cast_it_r(_tmp7AB,({struct Cyc_Absyn_Exp*_tmp20F[3U];({
# 1263
struct Cyc_Absyn_Exp*_tmp7A9=Cyc_Absyn_new_exp(ptr->r,0U);_tmp20F[0]=_tmp7A9;}),({
struct Cyc_Absyn_Exp*_tmp7A8=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp20F[1]=_tmp7A8;}),_tmp20F[2]=ind;({struct Cyc_Absyn_Exp*_tmp7AA=Cyc_Toc__check_dyneither_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp7AA,_tag_dyneither(_tmp20F,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1261
ptr->r=_tmp7AC;});
# 1266
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp219=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp20D)->f1;_LLF:
# 1269
 fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp21A=_tmp219;goto _LL11;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp21A=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp20D)->f1;_LL11: {
# 1272
void*ta1=Cyc_Toc_typ_to_c(_tmp220);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1277
struct _tuple13 _tmp210=Cyc_Evexp_eval_const_uint_exp(_tmp21A);struct _tuple13 _tmp211=_tmp210;unsigned int _tmp216;int _tmp215;_LL15: _tmp216=_tmp211.f1;_tmp215=_tmp211.f2;_LL16:;
if((!_tmp215  || _tmp216 != (unsigned int)1) || !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1280
({void*_tmp7B1=({void*_tmp7B0=Cyc_Absyn_cstar_type(ta1,_tmp21F);Cyc_Toc_cast_it_r(_tmp7B0,({struct Cyc_Absyn_Exp*_tmp212[4U];({
struct Cyc_Absyn_Exp*_tmp7AE=Cyc_Absyn_new_exp(ptr->r,0U);_tmp212[0]=_tmp7AE;}),_tmp212[1]=_tmp21A,({
struct Cyc_Absyn_Exp*_tmp7AD=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp212[2]=_tmp7AD;}),_tmp212[3]=ind;({struct Cyc_Absyn_Exp*_tmp7AF=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7AF,_tag_dyneither(_tmp212,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1280
ptr->r=_tmp7B1;});
# 1283
ans=1;}else{
# 1286
if(Cyc_Toc_is_zero(_tmp21A)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1289
({void*_tmp7B5=({void*_tmp7B4=_tmp20B;Cyc_Toc_cast_it_r(_tmp7B4,({struct Cyc_Absyn_Exp*_tmp213[1U];({struct Cyc_Absyn_Exp*_tmp7B2=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp213[0]=_tmp7B2;});({struct Cyc_Absyn_Exp*_tmp7B3=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp7B3,_tag_dyneither(_tmp213,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1289
ptr->r=_tmp7B5;});
# 1291
ans=0;}else{
# 1295
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp7B9=({void*_tmp7B8=Cyc_Absyn_cstar_type(ta1,_tmp21F);Cyc_Toc_cast_it_r(_tmp7B8,({struct Cyc_Absyn_Exp*_tmp214[3U];({
struct Cyc_Absyn_Exp*_tmp7B6=Cyc_Absyn_new_exp(ptr->r,0U);_tmp214[0]=_tmp7B6;}),_tmp214[1]=_tmp21A,_tmp214[2]=ind;({struct Cyc_Absyn_Exp*_tmp7B7=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7B7,_tag_dyneither(_tmp214,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1296
ptr->r=_tmp7B9;});
# 1298
ans=1;}}
# 1301
goto _LL5;}}else{_LL12: _LL13:
({void*_tmp217=0U;({struct _dyneither_ptr _tmp7BA=({const char*_tmp218="FIX: ptr_use_to_c, bad annotation";_tag_dyneither(_tmp218,sizeof(char),34U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp7BA,_tag_dyneither(_tmp217,sizeof(void*),0U));});});}}}}}}_LL5:;}
# 1304
Cyc_Toc_set_lhs(nv,_tmp209);
return ans;}else{_LL3: _LL4:
({void*_tmp21B=0U;({struct _dyneither_ptr _tmp7BB=({const char*_tmp21C="ptr_use_to_c: non-pointer-type";_tag_dyneither(_tmp21C,sizeof(char),31U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp7BB,_tag_dyneither(_tmp21B,sizeof(void*),0U));});});}_LL0:;};}
# 1310
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp221=0U;({struct _dyneither_ptr _tmp7BC=({const char*_tmp222="Missing type in primop ";_tag_dyneither(_tmp222,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7BC,_tag_dyneither(_tmp221,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1314
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->f1=Cyc_Toc_mt_tq,({void*_tmp7BD=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp223->f2=_tmp7BD;});_tmp223;});}
# 1319
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp224=e->r;void*_tmp225=_tmp224;struct Cyc_Absyn_Exp*_tmp22D;int _tmp22C;struct Cyc_Absyn_Exp*_tmp22B;int _tmp22A;struct Cyc_List_List*_tmp229;switch(*((int*)_tmp225)){case 26U: _LL1: _tmp229=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp225)->f1;_LL2:
# 1322
{struct Cyc_List_List*_tmp226=_tmp229;for(0;_tmp226 != 0;_tmp226=_tmp226->tl){
if((*((struct _tuple19*)_tmp226->hd)).f1 != 0)
({void*_tmp227=0U;({struct _dyneither_ptr _tmp7BE=({const char*_tmp228="array designators for abstract-field initialization";_tag_dyneither(_tmp228,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp7BE,_tag_dyneither(_tmp227,sizeof(void*),0U));});});}}
_tmp22B=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp229),0U);_tmp22A=0;goto _LL4;case 27U: _LL3: _tmp22B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp225)->f2;_tmp22A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp225)->f4;_LL4:
 _tmp22D=_tmp22B;_tmp22C=_tmp22A;goto _LL6;case 28U: _LL5: _tmp22D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp225)->f1;_tmp22C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp225)->f3;_LL6:
# 1328
 _tmp22D=Cyc_Absyn_copy_exp(_tmp22D);
return _tmp22C?({struct Cyc_Absyn_Exp*_tmp7BF=_tmp22D;Cyc_Absyn_add_exp(_tmp7BF,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp22D;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1337
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1345
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp22E=e->r;void*_tmp22F=_tmp22E;struct Cyc_List_List*_tmp230;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp22F)->tag == 29U){_LL1: _tmp230=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp22F)->f3;_LL2:
 dles=_tmp230;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1351
{void*_tmp231=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp232=_tmp231;struct Cyc_Absyn_Aggrdecl*_tmp233;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp232)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp232)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp232)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp233=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp232)->f1)->f1).KnownAggr).val;_LL7:
# 1353
 if(_tmp233->impl == 0)
return 0;
if((int)_tmp233->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp233->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1361
 return 0;}_LL5:;}
# 1363
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp234=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp235=(struct _tuple19*)dles->hd;struct _tuple19*_tmp236=_tmp235;struct Cyc_List_List*_tmp23F;struct Cyc_Absyn_Exp*_tmp23E;_LLB: _tmp23F=_tmp236->f1;_tmp23E=_tmp236->f2;_LLC:;{
struct _dyneither_ptr*_tmp237=Cyc_Absyn_designatorlist_to_fieldname(_tmp23F);
if(!Cyc_strptrcmp(_tmp237,_tmp234->name)){
struct Cyc_Absyn_Exp*_tmp238=Cyc_Toc_get_varsizeexp(_tmp23E);
if(_tmp238 != 0)
return _tmp238;{
void*_tmp239=Cyc_Tcutil_compress(_tmp234->type);void*_tmp23A=_tmp239;void*_tmp23D;struct Cyc_Absyn_Exp*_tmp23C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23A)->tag == 4U){_LLE: _tmp23D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23A)->f1).elt_type;_tmp23C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23A)->f1).num_elts;_LLF:
# 1378
 if(_tmp23C == 0  || !Cyc_Toc_is_zero(_tmp23C))
return 0;
# 1384
return({struct Cyc_Absyn_Exp*_tmp7C2=({struct Cyc_Absyn_Exp*_tmp23B[2U];_tmp23B[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp23E)),({
# 1386
struct Cyc_Absyn_Exp*_tmp7C0=Cyc_Absyn_sizeoftype_exp(_tmp23D,0U);_tmp23B[1]=_tmp7C0;});({struct Cyc_Absyn_Exp*_tmp7C1=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp7C1,_tag_dyneither(_tmp23B,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1384
Cyc_Absyn_add_exp(_tmp7C2,
# 1387
Cyc_Absyn_signed_int_exp((int)sizeof(long double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1392
({void*_tmp240=0U;({struct _dyneither_ptr _tmp7C3=({const char*_tmp241="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp241,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C3,_tag_dyneither(_tmp240,sizeof(void*),0U));});});};}
# 1395
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp242=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp242 != 0;_tmp242=_tmp242->tl){
struct Cyc_Absyn_Aggrfield*_tmp243=(struct Cyc_Absyn_Aggrfield*)_tmp242->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp243->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1402
({void*_tmp244=0U;({struct _dyneither_ptr _tmp7C5=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp247=({struct Cyc_String_pa_PrintArg_struct _tmp70D;_tmp70D.tag=0U,_tmp70D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp70D;});void*_tmp245[1U];_tmp245[0]=& _tmp247;({struct _dyneither_ptr _tmp7C4=({const char*_tmp246="get_member_offset %s failed";_tag_dyneither(_tmp246,sizeof(char),28U);});Cyc_aprintf(_tmp7C4,_tag_dyneither(_tmp245,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C5,_tag_dyneither(_tmp244,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1406
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1409
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1412
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248->tag=5U,_tmp248->f1=e1,_tmp248->f2=incr;_tmp248;}),0U);}
# 1416
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1425
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1427
void*_tmp249=e1->r;void*_tmp24A=_tmp249;struct Cyc_Absyn_Exp*_tmp252;struct _dyneither_ptr*_tmp251;int _tmp250;int _tmp24F;void*_tmp24E;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_Absyn_Stmt*_tmp24C;switch(*((int*)_tmp24A)){case 37U: _LL1: _tmp24C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp24A)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp24C,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp24E=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp24A)->f1;_tmp24D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp24A)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp24D,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp252=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24A)->f1;_tmp251=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24A)->f2;_tmp250=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24A)->f3;_tmp24F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24A)->f4;_LL6:
# 1432
 e1->r=_tmp252->r;
({struct Cyc_Absyn_Exp*_tmp7C8=e1;struct Cyc_List_List*_tmp7C7=({struct Cyc_List_List*_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B->hd=_tmp251,_tmp24B->tl=fs;_tmp24B;});struct Cyc_Absyn_Exp*(*_tmp7C6)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp7C8,_tmp7C7,_tmp7C6,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1440
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1442
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp7C9=(f(e1_copy,f_env))->r;e1->r=_tmp7C9;});
goto _LL0;}}_LL0:;}
# 1448
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1450
void*_tmp253=s->r;void*_tmp254=_tmp253;struct Cyc_Absyn_Stmt*_tmp25B;struct Cyc_Absyn_Decl*_tmp25A;struct Cyc_Absyn_Stmt*_tmp259;struct Cyc_Absyn_Exp*_tmp258;switch(*((int*)_tmp254)){case 1U: _LL1: _tmp258=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp254)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp258,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp25A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp254)->f1;_tmp259=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp254)->f2;_LL4:
# 1453
 Cyc_Toc_lvalue_assign_stmt(_tmp259,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp25B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp254)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp25B,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp257=({struct Cyc_String_pa_PrintArg_struct _tmp70E;_tmp70E.tag=0U,({struct _dyneither_ptr _tmp7CA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp70E.f1=_tmp7CA;});_tmp70E;});void*_tmp255[1U];_tmp255[0]=& _tmp257;({struct _dyneither_ptr _tmp7CB=({const char*_tmp256="lvalue_assign_stmt: %s";_tag_dyneither(_tmp256,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp7CB,_tag_dyneither(_tmp255,sizeof(void*),1U));});});}_LL0:;}
# 1459
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1463
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp25C=e->r;void*_tmp25D=_tmp25C;struct Cyc_Absyn_Stmt*_tmp264;struct Cyc_Absyn_Exp*_tmp263;void**_tmp262;struct Cyc_Absyn_Exp**_tmp261;switch(*((int*)_tmp25D)){case 14U: _LL1: _tmp262=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp261=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp25D)->f2;_LL2:
# 1466
({struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Toc_push_address_exp(*_tmp261);*_tmp261=_tmp7CC;});
({void*_tmp7CD=Cyc_Absyn_cstar_type(*_tmp262,Cyc_Toc_mt_tq);*_tmp262=_tmp7CD;});
return e;case 20U: _LL3: _tmp263=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_LL4:
# 1470
 return _tmp263;case 37U: _LL5: _tmp264=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_LL6:
# 1474
 Cyc_Toc_push_address_stmt(_tmp264);
return e;default: _LL7: _LL8:
# 1477
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp260=({struct Cyc_String_pa_PrintArg_struct _tmp70F;_tmp70F.tag=0U,({struct _dyneither_ptr _tmp7CE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp70F.f1=_tmp7CE;});_tmp70F;});void*_tmp25E[1U];_tmp25E[0]=& _tmp260;({struct _dyneither_ptr _tmp7CF=({const char*_tmp25F="can't take & of exp %s";_tag_dyneither(_tmp25F,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7CF,_tag_dyneither(_tmp25E,sizeof(void*),1U));});});}_LL0:;}
# 1481
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp265=s->r;void*_tmp266=_tmp265;struct Cyc_Absyn_Exp**_tmp26C;struct Cyc_Absyn_Stmt*_tmp26B;struct Cyc_Absyn_Stmt*_tmp26A;switch(*((int*)_tmp266)){case 2U: _LL1: _tmp26A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp266)->f2;_LL2:
 _tmp26B=_tmp26A;goto _LL4;case 12U: _LL3: _tmp26B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp266)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp26B);goto _LL0;case 1U: _LL5: _tmp26C=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp266)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Toc_push_address_exp(*_tmp26C);*_tmp26C=_tmp7D0;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp269=({struct Cyc_String_pa_PrintArg_struct _tmp710;_tmp710.tag=0U,({struct _dyneither_ptr _tmp7D1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp710.f1=_tmp7D1;});_tmp710;});void*_tmp267[1U];_tmp267[0]=& _tmp269;({struct _dyneither_ptr _tmp7D2=({const char*_tmp268="can't take & of stmt %s";_tag_dyneither(_tmp268,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7D2,_tag_dyneither(_tmp267,sizeof(void*),1U));});});}_LL0:;}
# 1493
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1505
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp26D=Cyc_Toc_typ_to_c(elt_type);
void*_tmp26E=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp26F=Cyc_Absyn_cstar_type(_tmp26D,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp270=({struct Cyc_Core_Opt*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->v=_tmp26F;_tmp2A2;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp271=e1->r;void*_tmp272=_tmp271;struct Cyc_Absyn_Exp*_tmp278;struct Cyc_Absyn_Exp*_tmp277;struct Cyc_Absyn_Exp*_tmp276;switch(*((int*)_tmp272)){case 20U: _LL1: _tmp276=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp272)->f1;_LL2:
# 1513
 if(!is_dyneither){
_tmp276=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp276,0,Cyc_Absyn_Other_coercion,0U);
_tmp276->topt=fat_ptr_type;
_tmp276->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1518
Cyc_Toc_exp_to_c(nv,_tmp276);xinit=_tmp276;goto _LL0;case 23U: _LL3: _tmp278=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp272)->f1;_tmp277=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp272)->f2;_LL4:
# 1520
 if(!is_dyneither){
_tmp278=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp278,0,Cyc_Absyn_Other_coercion,0U);
_tmp278->topt=fat_ptr_type;
_tmp278->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1525
Cyc_Toc_exp_to_c(nv,_tmp278);Cyc_Toc_exp_to_c(nv,_tmp277);
xinit=({struct Cyc_Absyn_Exp*_tmp273[3U];_tmp273[0]=_tmp278,({
struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp273[1]=_tmp7D3;}),_tmp273[2]=_tmp277;({struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp7D4,_tag_dyneither(_tmp273,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;default: _LL5: _LL6:
({void*_tmp274=0U;({struct _dyneither_ptr _tmp7D5=({const char*_tmp275="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp275,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7D5,_tag_dyneither(_tmp274,sizeof(void*),0U));});});}_LL0:;}{
# 1531
struct _tuple1*_tmp279=Cyc_Toc_temp_var();
struct _RegionHandle _tmp27A=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp27A;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp27B=({struct Cyc_Absyn_Vardecl*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->sc=Cyc_Absyn_Public,_tmp2A1->name=_tmp279,_tmp2A1->varloc=0U,_tmp2A1->tq=Cyc_Toc_mt_tq,_tmp2A1->type=_tmp26E,_tmp2A1->initializer=xinit,_tmp2A1->rgn=0,_tmp2A1->attributes=0,_tmp2A1->escapes=0;_tmp2A1;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp27C=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->tag=4U,_tmp2A0->f1=_tmp27B;_tmp2A0;});
struct Cyc_Absyn_Exp*_tmp27D=Cyc_Absyn_varb_exp((void*)_tmp27C,0U);
_tmp27D->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp27E=Cyc_Absyn_deref_exp(_tmp27D,0U);
_tmp27E->topt=elt_type;
_tmp27E->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,_tmp27E);{
struct _tuple1*_tmp27F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp280=({struct Cyc_Absyn_Vardecl*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->sc=Cyc_Absyn_Public,_tmp29F->name=_tmp27F,_tmp29F->varloc=0U,_tmp29F->tq=Cyc_Toc_mt_tq,_tmp29F->type=_tmp26D,_tmp29F->initializer=_tmp27E,_tmp29F->rgn=0,_tmp29F->attributes=0,_tmp29F->escapes=0;_tmp29F;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp281=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E->tag=4U,_tmp29E->f1=_tmp280;_tmp29E;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp282=Cyc_Absyn_varb_exp((void*)_tmp281,0U);
_tmp282->topt=_tmp27E->topt;
z_init=({enum Cyc_Absyn_Primop _tmp7D7=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp7D6=_tmp282;Cyc_Absyn_prim2_exp(_tmp7D7,_tmp7D6,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=_tmp282->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1552
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp283=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp284=({struct Cyc_Absyn_Vardecl*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->sc=Cyc_Absyn_Public,_tmp29D->name=_tmp283,_tmp29D->varloc=0U,_tmp29D->tq=Cyc_Toc_mt_tq,_tmp29D->type=_tmp26D,_tmp29D->initializer=z_init,_tmp29D->rgn=0,_tmp29D->attributes=0,_tmp29D->escapes=0;_tmp29D;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp285=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->tag=4U,_tmp29C->f1=_tmp284;_tmp29C;});
# 1558
struct Cyc_Absyn_Exp*_tmp286=Cyc_Absyn_varb_exp((void*)_tmp281,0U);_tmp286->topt=_tmp27E->topt;{
struct Cyc_Absyn_Exp*_tmp287=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*_tmp288=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp286,_tmp287,0U);
_tmp287->topt=Cyc_Absyn_sint_type;
_tmp288->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp288);{
# 1565
struct Cyc_Absyn_Exp*_tmp289=Cyc_Absyn_varb_exp((void*)_tmp285,0U);_tmp289->topt=_tmp27E->topt;{
struct Cyc_Absyn_Exp*_tmp28A=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*_tmp28B=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp289,_tmp28A,0U);
_tmp28A->topt=Cyc_Absyn_sint_type;
_tmp28B->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp28B);{
# 1572
struct Cyc_List_List*_tmp28C=({struct Cyc_Absyn_Exp*_tmp29B[2U];({struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Absyn_varb_exp((void*)_tmp27C,0U);_tmp29B[0]=_tmp7D9;}),({
struct Cyc_Absyn_Exp*_tmp7D8=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp29B[1]=_tmp7D8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp29B,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp28D=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp7DA=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp28C,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp7DA,_tmp28D,0U);});{
# 1579
struct Cyc_Absyn_Exp*_tmp28E=({struct Cyc_Absyn_Exp*_tmp7DB=xsize;Cyc_Absyn_and_exp(_tmp7DB,Cyc_Absyn_and_exp(_tmp288,_tmp28B,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp28F=Cyc_Absyn_exp_stmt(({void*_tmp29A=0U;({struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp7DC,_tag_dyneither(_tmp29A,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);
struct Cyc_Absyn_Exp*_tmp290=({struct Cyc_Absyn_Exp*_tmp7DD=Cyc_Absyn_varb_exp((void*)_tmp27C,0U);Cyc_Toc_member_exp(_tmp7DD,Cyc_Toc_curr_sp,0U);});
_tmp290=Cyc_Toc_cast_it(_tmp26F,_tmp290);{
struct Cyc_Absyn_Exp*_tmp291=Cyc_Absyn_deref_exp(_tmp290,0U);
struct Cyc_Absyn_Exp*_tmp292=({struct Cyc_Absyn_Exp*_tmp7DE=_tmp291;Cyc_Absyn_assign_exp(_tmp7DE,Cyc_Absyn_var_exp(_tmp283,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp293=Cyc_Absyn_exp_stmt(_tmp292,0U);
_tmp293=({struct Cyc_Absyn_Stmt*_tmp7E1=({struct Cyc_Absyn_Exp*_tmp7E0=_tmp28E;struct Cyc_Absyn_Stmt*_tmp7DF=_tmp28F;Cyc_Absyn_ifthenelse_stmt(_tmp7E0,_tmp7DF,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp7E1,_tmp293,0U);});
_tmp293=({struct Cyc_Absyn_Decl*_tmp7E3=({struct Cyc_Absyn_Decl*_tmp295=_cycalloc(sizeof(*_tmp295));({void*_tmp7E2=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294->tag=0U,_tmp294->f1=_tmp284;_tmp294;});_tmp295->r=_tmp7E2;}),_tmp295->loc=0U;_tmp295;});Cyc_Absyn_decl_stmt(_tmp7E3,_tmp293,0U);});
_tmp293=({struct Cyc_Absyn_Decl*_tmp7E5=({struct Cyc_Absyn_Decl*_tmp297=_cycalloc(sizeof(*_tmp297));({void*_tmp7E4=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296->tag=0U,_tmp296->f1=_tmp280;_tmp296;});_tmp297->r=_tmp7E4;}),_tmp297->loc=0U;_tmp297;});Cyc_Absyn_decl_stmt(_tmp7E5,_tmp293,0U);});
_tmp293=({struct Cyc_Absyn_Decl*_tmp7E7=({struct Cyc_Absyn_Decl*_tmp299=_cycalloc(sizeof(*_tmp299));({void*_tmp7E6=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->tag=0U,_tmp298->f1=_tmp27B;_tmp298;});_tmp299->r=_tmp7E6;}),_tmp299->loc=0U;_tmp299;});Cyc_Absyn_decl_stmt(_tmp7E7,_tmp293,0U);});
({void*_tmp7E8=Cyc_Toc_stmt_exp_r(_tmp293);e->r=_tmp7E8;});};};};};};};};};};}
# 1533
;_pop_region(rgn2);};}
# 1605 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1609
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2A3=Cyc_Tcutil_compress(aggrtype);void*_tmp2A4=_tmp2A3;union Cyc_Absyn_AggrInfo _tmp2A9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4)->f1)->tag == 20U){_LL1: _tmp2A9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4)->f1)->f1;_LL2:
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp2A9);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp2A5=0U;({struct _dyneither_ptr _tmp7EB=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A8=({struct Cyc_String_pa_PrintArg_struct _tmp711;_tmp711.tag=0U,({
struct _dyneither_ptr _tmp7E9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp711.f1=_tmp7E9;});_tmp711;});void*_tmp2A6[1U];_tmp2A6[0]=& _tmp2A8;({struct _dyneither_ptr _tmp7EA=({const char*_tmp2A7="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2A7,sizeof(char),51U);});Cyc_aprintf(_tmp7EA,_tag_dyneither(_tmp2A6,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp7EB,_tag_dyneither(_tmp2A5,sizeof(void*),0U));});});}_LL0:;}{
# 1615
struct _tuple29 _tmp2AA=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp2AB=_tmp2AA;struct _tuple1*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BB;_LL6: _tmp2BC=_tmp2AB.f1;_tmp2BB=_tmp2AB.f2;_LL7:;{
struct Cyc_Absyn_Exp*_tmp2AC=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2AD=Cyc_Absyn_aggrarrow_exp(_tmp2BB,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2AE=Cyc_Absyn_neq_exp(_tmp2AD,_tmp2AC,0U);
struct Cyc_Absyn_Exp*_tmp2AF=Cyc_Absyn_aggrarrow_exp(_tmp2BB,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2B0=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2AF,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2B1=({struct Cyc_Absyn_Exp*_tmp7ED=_tmp2AE;struct Cyc_Absyn_Stmt*_tmp7EC=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp7ED,_tmp7EC,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2B2=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2B3=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2B4=({struct _tuple1*_tmp7F0=_tmp2BC;void*_tmp7EF=_tmp2B2;struct Cyc_Absyn_Exp*_tmp7EE=_tmp2B3;Cyc_Absyn_declare_stmt(_tmp7F0,_tmp7EF,_tmp7EE,Cyc_Absyn_seq_stmt(_tmp2B1,_tmp2B0,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2B4);}else{
# 1628
struct Cyc_Absyn_Exp*_tmp2B5=({struct Cyc_Absyn_Exp*_tmp7F1=aggrproj(_tmp2BB,f,0U);Cyc_Toc_member_exp(_tmp7F1,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2B6=Cyc_Absyn_neq_exp(_tmp2B5,_tmp2AC,0U);
struct Cyc_Absyn_Exp*_tmp2B7=({struct Cyc_Absyn_Exp*_tmp7F2=aggrproj(_tmp2BB,f,0U);Cyc_Toc_member_exp(_tmp7F2,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2B8=Cyc_Absyn_exp_stmt(_tmp2B7,0U);
struct Cyc_Absyn_Stmt*_tmp2B9=({struct Cyc_Absyn_Exp*_tmp7F4=_tmp2B6;struct Cyc_Absyn_Stmt*_tmp7F3=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp7F4,_tmp7F3,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2BA=({struct _tuple1*_tmp7F7=_tmp2BC;void*_tmp7F6=e1_c_type;struct Cyc_Absyn_Exp*_tmp7F5=e1;Cyc_Absyn_declare_stmt(_tmp7F7,_tmp7F6,_tmp7F5,Cyc_Absyn_seq_stmt(_tmp2B9,_tmp2B8,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2BA);}};};}
# 1638
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1641
void*_tmp2BD=Cyc_Tcutil_compress(t);void*_tmp2BE=_tmp2BD;union Cyc_Absyn_AggrInfo _tmp2C5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BE)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BE)->f1)->tag == 20U){_LL1: _tmp2C5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BE)->f1)->f1;_LL2: {
# 1643
struct Cyc_Absyn_Aggrdecl*_tmp2BF=Cyc_Absyn_get_known_aggrdecl(_tmp2C5);
({int _tmp7F8=Cyc_Toc_get_member_offset(_tmp2BF,f);*f_tag=_tmp7F8;});{
# 1646
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C3=({struct Cyc_String_pa_PrintArg_struct _tmp713;_tmp713.tag=0U,_tmp713.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2BF->name).f2);_tmp713;});struct Cyc_String_pa_PrintArg_struct _tmp2C4=({struct Cyc_String_pa_PrintArg_struct _tmp712;_tmp712.tag=0U,_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp712;});void*_tmp2C1[2U];_tmp2C1[0]=& _tmp2C3,_tmp2C1[1]=& _tmp2C4;({struct _dyneither_ptr _tmp7F9=({const char*_tmp2C2="_union_%s_%s";_tag_dyneither(_tmp2C2,sizeof(char),13U);});Cyc_aprintf(_tmp7F9,_tag_dyneither(_tmp2C1,sizeof(void*),2U));});});
({void*_tmp7FA=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));*_tmp2C0=str;_tmp2C0;}));*tagged_member_type=_tmp7FA;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BF->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1657
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1660
void*_tmp2C6=e->r;void*_tmp2C7=_tmp2C6;struct Cyc_Absyn_Exp*_tmp2D3;struct _dyneither_ptr*_tmp2D2;int*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;struct _dyneither_ptr*_tmp2CF;int*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CD;switch(*((int*)_tmp2C7)){case 14U: _LL1: _tmp2CD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C7)->f2;_LL2:
({void*_tmp2C8=0U;({struct _dyneither_ptr _tmp7FB=({const char*_tmp2C9="cast on lhs!";_tag_dyneither(_tmp2C9,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7FB,_tag_dyneither(_tmp2C8,sizeof(void*),0U));});});case 21U: _LL3: _tmp2D0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C7)->f1;_tmp2CF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C7)->f2;_tmp2CE=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C7)->f4;_LL4:
# 1663
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp2D0->topt),_tmp2CF,f_tag,tagged_member_type,clear_read,_tmp2CE);case 22U: _LL5: _tmp2D3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C7)->f1;_tmp2D2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C7)->f2;_tmp2D1=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C7)->f4;_LL6: {
# 1666
void*_tmp2CA=Cyc_Tcutil_compress((void*)_check_null(_tmp2D3->topt));void*_tmp2CB=_tmp2CA;void*_tmp2CC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CB)->tag == 3U){_LLA: _tmp2CC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CB)->f1).elt_type;_LLB:
# 1668
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2CC,_tmp2D2,f_tag,tagged_member_type,clear_read,_tmp2D1);}else{_LLC: _LLD:
# 1670
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1672
 return 0;}_LL0:;}
# 1685 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1689
struct _tuple29 _tmp2D4=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp2D5=_tmp2D4;struct _tuple1*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D7;_LL1: _tmp2D8=_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;_LL2:;{
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(_tmp2D7,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(_tmp2D7,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1698
struct Cyc_Absyn_Exp*_tmp2D6=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
s2=({struct Cyc_Absyn_Exp*_tmp7FD=_tmp2D6;struct Cyc_Absyn_Stmt*_tmp7FC=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp7FD,_tmp7FC,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1701
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp800=_tmp2D8;void*_tmp7FF=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp7FE=
Cyc_Toc_push_address_exp(e1);
# 1701
Cyc_Absyn_declare_stmt(_tmp800,_tmp7FF,_tmp7FE,
# 1703
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};};}struct _tuple32{void*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1707
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp2DB=({struct Cyc_String_pa_PrintArg_struct _tmp714;_tmp714.tag=0U,({struct _dyneither_ptr _tmp801=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp714.f1=_tmp801;});_tmp714;});void*_tmp2D9[1U];_tmp2D9[0]=& _tmp2DB;({unsigned int _tmp803=e->loc;struct _dyneither_ptr _tmp802=({const char*_tmp2DA="exp_to_c: no type for %s";_tag_dyneither(_tmp2DA,sizeof(char),25U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp803,_tmp802,_tag_dyneither(_tmp2D9,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1712
int did_inserted_checks=0;
{void*_tmp2DC=e->annot;void*_tmp2DD=_tmp2DC;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2DD)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2DD)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1718
{void*_tmp2DE=e->r;void*_tmp2DF=_tmp2DE;struct Cyc_Absyn_Stmt*_tmp485;struct Cyc_Absyn_Exp*_tmp484;struct _dyneither_ptr*_tmp483;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*_tmp481;int _tmp480;struct Cyc_Absyn_Exp*_tmp47F;void**_tmp47E;struct Cyc_Absyn_Exp*_tmp47D;int _tmp47C;int _tmp47B;struct Cyc_List_List*_tmp47A;struct Cyc_Absyn_Datatypedecl*_tmp479;struct Cyc_Absyn_Datatypefield*_tmp478;struct _tuple1**_tmp477;struct Cyc_List_List*_tmp476;struct Cyc_List_List*_tmp475;struct Cyc_Absyn_Aggrdecl*_tmp474;void*_tmp473;struct Cyc_List_List*_tmp472;struct Cyc_Absyn_Exp*_tmp471;void*_tmp470;int _tmp46F;struct Cyc_Absyn_Vardecl*_tmp46E;struct Cyc_Absyn_Exp*_tmp46D;struct Cyc_Absyn_Exp*_tmp46C;int _tmp46B;struct Cyc_List_List*_tmp46A;struct Cyc_List_List*_tmp469;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*_tmp467;struct Cyc_Absyn_Exp*_tmp466;struct Cyc_Absyn_Exp*_tmp465;struct _dyneither_ptr*_tmp464;int _tmp463;int _tmp462;struct Cyc_Absyn_Exp*_tmp461;struct _dyneither_ptr*_tmp460;int _tmp45F;int _tmp45E;void*_tmp45D;struct Cyc_List_List*_tmp45C;void*_tmp45B;struct Cyc_Absyn_Exp*_tmp45A;struct Cyc_Absyn_Exp*_tmp459;struct Cyc_Absyn_Exp*_tmp458;struct Cyc_Absyn_Exp*_tmp457;void**_tmp456;struct Cyc_Absyn_Exp*_tmp455;int _tmp454;enum Cyc_Absyn_Coercion _tmp453;struct Cyc_Absyn_Exp*_tmp452;struct Cyc_List_List*_tmp451;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Exp*_tmp44F;int _tmp44E;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_List_List*_tmp44C;int _tmp44B;struct Cyc_List_List*_tmp44A;struct Cyc_Absyn_VarargInfo*_tmp449;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_List_List*_tmp447;struct Cyc_Absyn_Exp*_tmp446;struct Cyc_Absyn_Exp*_tmp445;struct Cyc_Absyn_Exp*_tmp444;struct Cyc_Absyn_Exp*_tmp443;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Exp*_tmp440;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_Core_Opt*_tmp43C;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_Absyn_Exp*_tmp43A;enum Cyc_Absyn_Incrementor _tmp439;enum Cyc_Absyn_Primop _tmp438;struct Cyc_List_List*_tmp437;struct Cyc_Absyn_Exp*_tmp436;switch(*((int*)_tmp2DF)){case 2U: _LL8: _LL9:
# 1720
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1727
struct Cyc_Absyn_Exp*_tmp2E0=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp804=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp2E0,_tmp2E0))->r;e->r=_tmp804;});else{
# 1732
({void*_tmp806=({struct Cyc_Absyn_Exp*_tmp2E1[3U];_tmp2E1[0]=_tmp2E0,_tmp2E1[1]=_tmp2E0,_tmp2E1[2]=_tmp2E0;({struct Cyc_Absyn_Exp*_tmp805=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp805,_tag_dyneither(_tmp2E1,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp806;});}}else{
# 1734
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1736
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 41U: _LL10: _tmp436=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_LL11:
 Cyc_Toc_exp_to_c(nv,_tmp436);goto _LL7;case 3U: _LL12: _tmp438=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp437=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL13: {
# 1742
struct Cyc_List_List*_tmp2E2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp437);
# 1744
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp437);
{enum Cyc_Absyn_Primop _tmp2E3=_tmp438;switch(_tmp2E3){case Cyc_Absyn_Numelts: _LL61: _LL62: {
# 1747
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp437))->hd;
{void*_tmp2E4=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp2E5=_tmp2E4;void*_tmp2F2;void*_tmp2F1;void*_tmp2F0;void*_tmp2EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E5)->tag == 3U){_LL76: _tmp2F2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E5)->f1).elt_type;_tmp2F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E5)->f1).ptr_atts).nullable;_tmp2F0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E5)->f1).ptr_atts).bounds;_tmp2EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E5)->f1).ptr_atts).zero_term;_LL77: {
# 1750
struct Cyc_Absyn_Exp*_tmp2E6=({void*_tmp807=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp807,_tmp2F0);});
if(_tmp2E6 == 0)
# 1753
({void*_tmp80A=({struct Cyc_Absyn_Exp*_tmp2E7[2U];_tmp2E7[0]=(struct Cyc_Absyn_Exp*)_tmp437->hd,({
struct Cyc_Absyn_Exp*_tmp808=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp2F2),0U);_tmp2E7[1]=_tmp808;});({struct Cyc_Absyn_Exp*_tmp809=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp809,_tag_dyneither(_tmp2E7,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1753
e->r=_tmp80A;});else{
# 1756
struct Cyc_Absyn_Exp*_tmp2E8=_tmp2E6;
# 1758
if(Cyc_Tcutil_force_type2bool(0,_tmp2EF)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp437->hd);
# 1762
({void*_tmp80C=({struct Cyc_Absyn_Exp*_tmp2E9[2U];_tmp2E9[0]=(struct Cyc_Absyn_Exp*)_tmp437->hd,_tmp2E9[1]=_tmp2E8;({struct Cyc_Absyn_Exp*_tmp80B=function_e;Cyc_Toc_fncall_exp_r(_tmp80B,_tag_dyneither(_tmp2E9,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp80C;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp2F1)){
if(!Cyc_Evexp_c_can_eval(_tmp2E8))
({void*_tmp2EA=0U;({unsigned int _tmp80E=e->loc;struct _dyneither_ptr _tmp80D=({const char*_tmp2EB="can't calculate numelts";_tag_dyneither(_tmp2EB,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp80E,_tmp80D,_tag_dyneither(_tmp2EA,sizeof(void*),0U));});});
# 1767
({void*_tmp811=({struct Cyc_Absyn_Exp*_tmp810=arg;struct Cyc_Absyn_Exp*_tmp80F=_tmp2E8;Cyc_Toc_conditional_exp_r(_tmp810,_tmp80F,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp811;});}else{
# 1769
e->r=_tmp2E8->r;goto _LL75;}}}
# 1772
goto _LL75;}}else{_LL78: _LL79:
({struct Cyc_String_pa_PrintArg_struct _tmp2EE=({struct Cyc_String_pa_PrintArg_struct _tmp715;_tmp715.tag=0U,({
struct _dyneither_ptr _tmp812=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp715.f1=_tmp812;});_tmp715;});void*_tmp2EC[1U];_tmp2EC[0]=& _tmp2EE;({struct _dyneither_ptr _tmp813=({const char*_tmp2ED="numelts primop applied to non-pointer %s";_tag_dyneither(_tmp2ED,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp813,_tag_dyneither(_tmp2EC,sizeof(void*),1U));});});}_LL75:;}
# 1776
goto _LL60;}case Cyc_Absyn_Plus: _LL63: _LL64:
# 1781
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp2F3=0U;({struct _dyneither_ptr _tmp814=({const char*_tmp2F4="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp2F4,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp814,_tag_dyneither(_tmp2F3,sizeof(void*),0U));});});
{void*_tmp2F5=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp2E2))->hd);void*_tmp2F6=_tmp2F5;void*_tmp2FC;void*_tmp2FB;void*_tmp2FA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6)->tag == 3U){_LL7B: _tmp2FC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6)->f1).elt_type;_tmp2FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6)->f1).ptr_atts).bounds;_tmp2FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6)->f1).ptr_atts).zero_term;_LL7C: {
# 1785
struct Cyc_Absyn_Exp*_tmp2F7=({void*_tmp815=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp815,_tmp2FB);});
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp437))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp437->tl))->hd;
if(_tmp2F7 == 0)
({void*_tmp818=({struct Cyc_Absyn_Exp*_tmp2F8[3U];_tmp2F8[0]=e1,({
struct Cyc_Absyn_Exp*_tmp816=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp2FC),0U);_tmp2F8[1]=_tmp816;}),_tmp2F8[2]=e2;({struct Cyc_Absyn_Exp*_tmp817=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp817,_tag_dyneither(_tmp2F8,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1789
e->r=_tmp818;});else{
# 1791
if(Cyc_Tcutil_force_type2bool(0,_tmp2FA))
({void*_tmp81A=({struct Cyc_Absyn_Exp*_tmp2F9[3U];_tmp2F9[0]=e1,_tmp2F9[1]=_tmp2F7,_tmp2F9[2]=e2;({struct Cyc_Absyn_Exp*_tmp819=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp819,_tag_dyneither(_tmp2F9,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp81A;});}
goto _LL7A;}}else{_LL7D: _LL7E:
 goto _LL7A;}_LL7A:;}
# 1796
goto _LL60;case Cyc_Absyn_Minus: _LL65: _LL66: {
# 1801
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp2E2))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp437))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp437->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp2E2->tl))->hd)){
({void*_tmp81C=({struct Cyc_Absyn_Exp*_tmp81B=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp81B,Cyc_Toc_curr_sp);});e1->r=_tmp81C;});
({void*_tmp81E=({struct Cyc_Absyn_Exp*_tmp81D=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp81D,Cyc_Toc_curr_sp);});e2->r=_tmp81E;});
({void*_tmp820=({void*_tmp81F=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp81F;});e1->topt=_tmp820;});
({void*_tmp822=({struct Cyc_Absyn_Exp*_tmp821=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp821,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1809
e->r=_tmp822;});}else{
# 1812
({void*_tmp826=({struct Cyc_Absyn_Exp*_tmp2FD[3U];_tmp2FD[0]=e1,({
struct Cyc_Absyn_Exp*_tmp824=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2FD[1]=_tmp824;}),({
struct Cyc_Absyn_Exp*_tmp823=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp2FD[2]=_tmp823;});({struct Cyc_Absyn_Exp*_tmp825=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp825,_tag_dyneither(_tmp2FD,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1812
e->r=_tmp826;});}}
# 1816
goto _LL60;}case Cyc_Absyn_Eq: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Neq: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Gt: _LL6B: _LL6C:
 goto _LL6E;case Cyc_Absyn_Gte: _LL6D: _LL6E:
 goto _LL70;case Cyc_Absyn_Lt: _LL6F: _LL70:
 goto _LL72;case Cyc_Absyn_Lte: _LL71: _LL72: {
# 1824
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp437))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp437->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp2E2))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp2E2->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& elt_type)){
void*_tmp2FE=({void*_tmp827=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp827,Cyc_Toc_mt_tq);});
({void*_tmp82A=({void*_tmp829=_tmp2FE;Cyc_Toc_cast_it_r(_tmp829,({struct Cyc_Absyn_Exp*_tmp828=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp828,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp82A;});
e1->topt=_tmp2FE;}
# 1834
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
void*_tmp2FF=({void*_tmp82B=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp82B,Cyc_Toc_mt_tq);});
({void*_tmp82E=({void*_tmp82D=_tmp2FF;Cyc_Toc_cast_it_r(_tmp82D,({struct Cyc_Absyn_Exp*_tmp82C=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp82C,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp82E;});
e2->topt=_tmp2FF;}
# 1839
goto _LL60;}default: _LL73: _LL74:
 goto _LL60;}_LL60:;}
# 1842
goto _LL7;}case 5U: _LL14: _tmp43A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp439=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL15: {
# 1844
void*e2_cyc_typ=(void*)_check_null(_tmp43A->topt);
# 1853 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _dyneither_ptr incr_str=({const char*_tmp313="increment";_tag_dyneither(_tmp313,sizeof(char),10U);});
if((int)_tmp439 == (int)2U  || (int)_tmp439 == (int)3U)incr_str=({const char*_tmp300="decrement";_tag_dyneither(_tmp300,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp43A,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp303=({struct Cyc_String_pa_PrintArg_struct _tmp716;_tmp716.tag=0U,_tmp716.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp716;});void*_tmp301[1U];_tmp301[0]=& _tmp303;({unsigned int _tmp830=e->loc;struct _dyneither_ptr _tmp82F=({const char*_tmp302="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp302,sizeof(char),74U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp830,_tmp82F,_tag_dyneither(_tmp301,sizeof(void*),1U));});});
# 1861
if(Cyc_Toc_is_tagged_union_project(_tmp43A,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp304=_tmp439;switch(_tmp304){case Cyc_Absyn_PreInc: _LL80: _LL81:
 op=0U;goto _LL7F;case Cyc_Absyn_PreDec: _LL82: _LL83:
 op=2U;goto _LL7F;default: _LL84: _LL85:
({struct Cyc_String_pa_PrintArg_struct _tmp307=({struct Cyc_String_pa_PrintArg_struct _tmp717;_tmp717.tag=0U,_tmp717.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp717;});void*_tmp305[1U];_tmp305[0]=& _tmp307;({unsigned int _tmp832=e->loc;struct _dyneither_ptr _tmp831=({const char*_tmp306="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp306,sizeof(char),58U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp832,_tmp831,_tag_dyneither(_tmp305,sizeof(void*),1U));});});}_LL7F:;}
# 1871
({void*_tmp834=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309->tag=4U,_tmp309->f1=_tmp43A,({struct Cyc_Core_Opt*_tmp833=({struct Cyc_Core_Opt*_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308->v=(void*)op;_tmp308;});_tmp309->f2=_tmp833;}),_tmp309->f3=one;_tmp309;});e->r=_tmp834;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1875
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp43A);
Cyc_Toc_set_lhs(nv,0);{
# 1880
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=((int)_tmp439 == (int)1U  || (int)_tmp439 == (int)3U)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 1886
if((int)_tmp439 == (int)2U  || (int)_tmp439 == (int)3U)
change=-1;
({void*_tmp839=({struct Cyc_Absyn_Exp*_tmp30A[3U];({struct Cyc_Absyn_Exp*_tmp837=Cyc_Toc_push_address_exp(_tmp43A);_tmp30A[0]=_tmp837;}),({
struct Cyc_Absyn_Exp*_tmp836=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp30A[1]=_tmp836;}),({
struct Cyc_Absyn_Exp*_tmp835=Cyc_Absyn_signed_int_exp(change,0U);_tmp30A[2]=_tmp835;});({struct Cyc_Absyn_Exp*_tmp838=fn_e;Cyc_Toc_fncall_exp_r(_tmp838,_tag_dyneither(_tmp30A,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1888
e->r=_tmp839;});}else{
# 1891
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1897
did_inserted_checks=1;
if((int)_tmp439 != (int)1U){
struct _tuple1*_tmp30B=Cyc_Toc_temp_var();
void*_tmp30C=({void*_tmp83A=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp83A,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp30D=Cyc_Toc_push_address_exp(_tmp43A);
struct Cyc_Absyn_Exp*_tmp30E=({struct Cyc_Absyn_Exp*_tmp83B=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp30B,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp83B,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
_tmp30E=({struct Cyc_Absyn_Exp*_tmp83D=({struct Cyc_Absyn_Exp*_tmp83C=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp30B,0U),0U);Cyc_Absyn_neq_exp(_tmp83C,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1905
Cyc_Absyn_and_exp(_tmp83D,_tmp30E,0U);});{
# 1907
struct Cyc_Absyn_Stmt*_tmp30F=({struct Cyc_Absyn_Exp*_tmp840=_tmp30E;struct Cyc_Absyn_Stmt*_tmp83F=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp30B,0U),0U),0U),0U);
# 1907
Cyc_Absyn_ifthenelse_stmt(_tmp840,_tmp83F,
# 1909
Cyc_Absyn_exp_stmt(({void*_tmp310=0U;({struct Cyc_Absyn_Exp*_tmp83E=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp83E,_tag_dyneither(_tmp310,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
_tmp30F=({struct Cyc_Absyn_Stmt*_tmp841=_tmp30F;Cyc_Absyn_seq_stmt(_tmp841,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp30B,0U),0U),0U),0U);});
({void*_tmp842=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp30B,_tmp30C,_tmp30D,_tmp30F,0U),0U))->r;e->r=_tmp842;});};}else{
# 1914
struct Cyc_Toc_functionSet*_tmp311=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp311,_tmp43A);
({void*_tmp846=({struct Cyc_Absyn_Exp*_tmp312[2U];({struct Cyc_Absyn_Exp*_tmp844=Cyc_Toc_push_address_exp(_tmp43A);_tmp312[0]=_tmp844;}),({struct Cyc_Absyn_Exp*_tmp843=Cyc_Absyn_signed_int_exp(1,0U);_tmp312[1]=_tmp843;});({struct Cyc_Absyn_Exp*_tmp845=fn_e;Cyc_Toc_fncall_exp_r(_tmp845,_tag_dyneither(_tmp312,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp846;});}}else{
# 1918
if(elt_typ == Cyc_Absyn_void_type  && !Cyc_Absyn_is_lvalue(_tmp43A)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp43A,0,Cyc_Toc_incr_lvalue,_tmp439);
e->r=_tmp43A->r;}}}
# 1922
goto _LL7;};}case 4U: _LL16: _tmp43D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp43C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp43B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3;_LL17: {
# 1941 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp43D->topt);
void*e2_old_typ=(void*)_check_null(_tmp43B->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp43D,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp43D);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp43B);
({void*_tmp847=Cyc_Toc_tagged_union_assignop(_tmp43D,e1_old_typ,_tmp43C,_tmp43B,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp847;});
# 1952
goto _LL7;}{
# 1954
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp43D,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp43D,_tmp43C,_tmp43B,ptr_type,is_dyneither,elt_type);
# 1960
return;}{
# 1964
int e1_poly=Cyc_Toc_is_poly_project(_tmp43D);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp43D);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp43B);{
# 1970
int done=0;
if(_tmp43C != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp314=(enum Cyc_Absyn_Primop)_tmp43C->v;enum Cyc_Absyn_Primop _tmp315=_tmp314;switch(_tmp315){case Cyc_Absyn_Plus: _LL87: _LL88:
 change=_tmp43B;goto _LL86;case Cyc_Absyn_Minus: _LL89: _LL8A:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp43B,0U);goto _LL86;default: _LL8B: _LL8C:
({void*_tmp316=0U;({struct _dyneither_ptr _tmp848=({const char*_tmp317="bad t ? pointer arithmetic";_tag_dyneither(_tmp317,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp848,_tag_dyneither(_tmp316,sizeof(void*),0U));});});}_LL86:;}
# 1980
done=1;{
# 1982
struct Cyc_Absyn_Exp*_tmp318=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp84C=({struct Cyc_Absyn_Exp*_tmp319[3U];({struct Cyc_Absyn_Exp*_tmp84A=Cyc_Toc_push_address_exp(_tmp43D);_tmp319[0]=_tmp84A;}),({
struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp319[1]=_tmp849;}),_tmp319[2]=change;({struct Cyc_Absyn_Exp*_tmp84B=_tmp318;Cyc_Toc_fncall_exp_r(_tmp84B,_tag_dyneither(_tmp319,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1983
e->r=_tmp84C;});};}else{
# 1986
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1989
enum Cyc_Absyn_Primop _tmp31A=(enum Cyc_Absyn_Primop)_tmp43C->v;enum Cyc_Absyn_Primop _tmp31B=_tmp31A;if(_tmp31B == Cyc_Absyn_Plus){_LL8E: _LL8F:
# 1991
 done=1;
({void*_tmp84E=({struct Cyc_Absyn_Exp*_tmp31C[2U];_tmp31C[0]=_tmp43D,_tmp31C[1]=_tmp43B;({struct Cyc_Absyn_Exp*_tmp84D=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp43D);Cyc_Toc_fncall_exp_r(_tmp84D,_tag_dyneither(_tmp31C,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp84E;});
goto _LL8D;}else{_LL90: _LL91:
({void*_tmp31D=0U;({struct _dyneither_ptr _tmp84F=({const char*_tmp31E="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp31E,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp84F,_tag_dyneither(_tmp31D,sizeof(void*),0U));});});}_LL8D:;}}}
# 1998
if(!done){
# 2000
if(e1_poly)
({void*_tmp851=({void*_tmp850=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp850,Cyc_Absyn_new_exp(_tmp43B->r,0U));});_tmp43B->r=_tmp851;});
# 2006
if(!Cyc_Absyn_is_lvalue(_tmp43D)){
({struct Cyc_Absyn_Exp*_tmp852=_tmp43D;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp852,0,Cyc_Toc_assignop_lvalue,({struct _tuple31*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F->f1=_tmp43C,_tmp31F->f2=_tmp43B;_tmp31F;}));});
e->r=_tmp43D->r;}}
# 2011
goto _LL7;};};};}case 6U: _LL18: _tmp440=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp43F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp43E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3;_LL19:
# 2013
 Cyc_Toc_exp_to_c(nv,_tmp440);
Cyc_Toc_exp_to_c(nv,_tmp43F);
Cyc_Toc_exp_to_c(nv,_tmp43E);
goto _LL7;case 7U: _LL1A: _tmp442=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp441=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL1B:
 _tmp444=_tmp442;_tmp443=_tmp441;goto _LL1D;case 8U: _LL1C: _tmp444=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp443=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL1D:
 _tmp446=_tmp444;_tmp445=_tmp443;goto _LL1F;case 9U: _LL1E: _tmp446=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp445=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL1F:
# 2020
 Cyc_Toc_exp_to_c(nv,_tmp446);
Cyc_Toc_exp_to_c(nv,_tmp445);
goto _LL7;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3 == 0){_LL20: _tmp448=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp447=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL21: {
# 2024
void*e1_typ=(void*)_check_null(_tmp448->topt);
Cyc_Toc_exp_to_c(nv,_tmp448);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2029
({void*_tmp856=({void*_tmp855=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp855,({struct Cyc_Absyn_Exp*_tmp320[1U];({struct Cyc_Absyn_Exp*_tmp853=
Cyc_Absyn_copy_exp(_tmp448);_tmp320[0]=_tmp853;});({struct Cyc_Absyn_Exp*_tmp854=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp854,_tag_dyneither(_tmp320,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2029
_tmp448->r=_tmp856;});
# 2031
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp447);
goto _LL7;}}else{_LL22: _tmp44D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp44C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp44B=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3)->num_varargs;_tmp44A=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3)->injectors;_tmp449=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3)->vai;_LL23: {
# 2042 "toc.cyc"
struct _RegionHandle _tmp321=_new_region("r");struct _RegionHandle*r=& _tmp321;_push_region(r);{
struct _tuple29 _tmp322=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp323=_tmp322;struct _tuple1*_tmp33C;struct Cyc_Absyn_Exp*_tmp33B;_LL93: _tmp33C=_tmp323.f1;_tmp33B=_tmp323.f2;_LL94:;{
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp44B,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp449->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2049
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp44C);
int num_normargs=num_args - _tmp44B;
# 2053
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp44C=_tmp44C->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp44C))->hd);
new_args=({struct Cyc_List_List*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324->hd=(struct Cyc_Absyn_Exp*)_tmp44C->hd,_tmp324->tl=new_args;_tmp324;});}}
# 2058
new_args=({struct Cyc_List_List*_tmp326=_cycalloc(sizeof(*_tmp326));({struct Cyc_Absyn_Exp*_tmp859=({struct Cyc_Absyn_Exp*_tmp325[3U];_tmp325[0]=_tmp33B,({
# 2060
struct Cyc_Absyn_Exp*_tmp857=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp325[1]=_tmp857;}),_tmp325[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp858=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp858,_tag_dyneither(_tmp325,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2058
_tmp326->hd=_tmp859;}),_tmp326->tl=new_args;_tmp326;});
# 2063
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2065
void*e1_typ=(void*)_check_null(_tmp44D->topt);
Cyc_Toc_exp_to_c(nv,_tmp44D);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2070
({void*_tmp85D=({void*_tmp85C=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp85C,({struct Cyc_Absyn_Exp*_tmp327[1U];({struct Cyc_Absyn_Exp*_tmp85A=
Cyc_Absyn_copy_exp(_tmp44D);_tmp327[0]=_tmp85A;});({struct Cyc_Absyn_Exp*_tmp85B=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp85B,_tag_dyneither(_tmp327,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2070
_tmp44D->r=_tmp85D;});{
# 2072
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp44D,new_args,0U),0U);
# 2075
if(_tmp449->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp328=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp449->type));void*_tmp329=_tmp328;struct Cyc_Absyn_Datatypedecl*_tmp32C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp329)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp329)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp329)->f1)->f1).KnownDatatype).tag == 2){_LL96: _tmp32C=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp329)->f1)->f1).KnownDatatype).val;_LL97:
 tud=_tmp32C;goto _LL95;}else{goto _LL98;}}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp32A=0U;({struct _dyneither_ptr _tmp85E=({const char*_tmp32B="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp32B,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85E,_tag_dyneither(_tmp32A,sizeof(void*),0U));});});}_LL95:;}{
# 2081
struct _dyneither_ptr vs=({unsigned int _tmp338=(unsigned int)_tmp44B;struct _tuple1**_tmp337=({struct _RegionHandle*_tmp85F=r;_region_malloc(_tmp85F,_check_times(_tmp338,sizeof(struct _tuple1*)));});({{unsigned int _tmp718=(unsigned int)_tmp44B;unsigned int i;for(i=0;i < _tmp718;++ i){({struct _tuple1*_tmp860=Cyc_Toc_temp_var();_tmp337[i]=_tmp860;});}}0;});_tag_dyneither(_tmp337,sizeof(struct _tuple1*),_tmp338);});
# 2083
if(_tmp44B != 0){
# 2087
struct Cyc_List_List*_tmp32D=0;
{int i=_tmp44B - 1;for(0;i >= 0;-- i){
_tmp32D=({struct Cyc_List_List*_tmp32E=_cycalloc(sizeof(*_tmp32E));({struct Cyc_Absyn_Exp*_tmp861=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp32E->hd=_tmp861;}),_tmp32E->tl=_tmp32D;_tmp32E;});}}
s=({struct _tuple1*_tmp864=_tmp33C;void*_tmp863=arr_type;struct Cyc_Absyn_Exp*_tmp862=Cyc_Absyn_array_exp(_tmp32D,0U);Cyc_Absyn_declare_stmt(_tmp864,_tmp863,_tmp862,s,0U);});
# 2092
_tmp44C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp44C);
_tmp44A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp44A);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp44C)- 1;
for(0;_tmp44C != 0;(_tmp44C=_tmp44C->tl,_tmp44A=_tmp44A->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp44C->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp32F=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp44A))->hd;struct Cyc_Absyn_Datatypefield*_tmp330=_tmp32F;struct _tuple1*_tmp336;struct Cyc_List_List*_tmp335;_LL9B: _tmp336=_tmp330->name;_tmp335=_tmp330->typs;_LL9C:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp335))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2106
struct _tuple1*_tmp331=Cyc_Toc_collapse_qvars(_tmp336,tud->name);
struct Cyc_List_List*_tmp332=({struct _tuple19*_tmp334[2U];({struct _tuple19*_tmp868=({struct _dyneither_ptr*_tmp867=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp867,Cyc_Toc_datatype_tag(tud,_tmp336));});_tmp334[0]=_tmp868;}),({
struct _tuple19*_tmp866=({struct _dyneither_ptr*_tmp865=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp865,arg);});_tmp334[1]=_tmp866;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp334,sizeof(struct _tuple19*),2U));});
s=({struct _tuple1*_tmp86B=var;void*_tmp86A=Cyc_Absyn_strctq(_tmp331);struct Cyc_Absyn_Exp*_tmp869=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333->tag=29U,_tmp333->f1=_tmp331,_tmp333->f2=0,_tmp333->f3=_tmp332,_tmp333->f4=0;_tmp333;}),0U);
# 2109
Cyc_Absyn_declare_stmt(_tmp86B,_tmp86A,_tmp869,s,0U);});};};}};}else{
# 2117
s=({struct _tuple1*_tmp86E=_tmp33C;void*_tmp86D=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp86C=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp86E,_tmp86D,_tmp86C,s,0U);});}};}else{
# 2122
if(_tmp44B != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp44C != 0;_tmp44C=_tmp44C->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp44C->hd);
array_args=({struct Cyc_List_List*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->hd=(struct Cyc_Absyn_Exp*)_tmp44C->hd,_tmp339->tl=array_args;_tmp339;});}{
# 2128
struct Cyc_Absyn_Exp*_tmp33A=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
s=Cyc_Absyn_declare_stmt(_tmp33C,arr_type,_tmp33A,s,0U);};}else{
# 2131
s=({struct _tuple1*_tmp871=_tmp33C;void*_tmp870=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp86F=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp871,_tmp870,_tmp86F,s,0U);});}}
# 2134
({void*_tmp872=Cyc_Toc_stmt_exp_r(s);e->r=_tmp872;});};};};}
# 2136
_npop_handler(0U);goto _LL7;
# 2042
;_pop_region(r);}}case 11U: _LL24: _tmp44F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp44E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL25:
# 2139
 Cyc_Toc_exp_to_c(nv,_tmp44F);{
struct Cyc_Absyn_Exp*fn_e=_tmp44E?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp875=({void*_tmp874=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp874,({struct Cyc_Absyn_Exp*_tmp33D[1U];_tmp33D[0]=_tmp44F;({struct Cyc_Absyn_Exp*_tmp873=fn_e;Cyc_Toc_fncall_exp_dl(_tmp873,_tag_dyneither(_tmp33D,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp875;});
goto _LL7;};case 12U: _LL26: _tmp450=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_LL27:
 Cyc_Toc_exp_to_c(nv,_tmp450);goto _LL7;case 13U: _LL28: _tmp452=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp451=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL29:
# 2145
 Cyc_Toc_exp_to_c(nv,_tmp452);
# 2154 "toc.cyc"
for(0;_tmp451 != 0;_tmp451=_tmp451->tl){
enum Cyc_Absyn_KindQual _tmp33E=(Cyc_Tcutil_type_kind((void*)_tmp451->hd))->kind;
if((int)_tmp33E != (int)4U  && (int)_tmp33E != (int)3U){
{void*_tmp33F=Cyc_Tcutil_compress((void*)_tmp451->hd);void*_tmp340=_tmp33F;switch(*((int*)_tmp340)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp340)->f1)->tag == 18U){_LLA0: _LLA1:
 continue;}else{goto _LLA2;}default: _LLA2: _LLA3:
# 2161
({void*_tmp877=({void*_tmp876=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp876,_tmp452,0U);})->r;e->r=_tmp877;});
goto _LL9D;}_LL9D:;}
# 2164
break;}}
# 2167
goto _LL7;case 14U: _LL2A: _tmp456=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp455=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp454=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3;_tmp453=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f4;_LL2B: {
# 2169
void*old_t2=(void*)_check_null(_tmp455->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp456;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp456=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp455);
# 2176
{struct _tuple32 _tmp341=({struct _tuple32 _tmp71A;({void*_tmp879=Cyc_Tcutil_compress(old_t2);_tmp71A.f1=_tmp879;}),({void*_tmp878=Cyc_Tcutil_compress(new_typ);_tmp71A.f2=_tmp878;});_tmp71A;});struct _tuple32 _tmp342=_tmp341;struct Cyc_Absyn_PtrInfo _tmp368;struct Cyc_Absyn_PtrInfo _tmp367;struct Cyc_Absyn_PtrInfo _tmp366;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp342.f1)->tag == 3U)switch(*((int*)_tmp342.f2)){case 3U: _LLA5: _tmp367=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp342.f1)->f1;_tmp366=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp342.f2)->f1;_LLA6: {
# 2178
struct Cyc_Absyn_Exp*_tmp343=({void*_tmp87A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp87A,(_tmp367.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp344=({void*_tmp87B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp87B,(_tmp366.ptr_atts).bounds);});
int _tmp345=Cyc_Tcutil_force_type2bool(0,(_tmp367.ptr_atts).zero_term);
int _tmp346=Cyc_Tcutil_force_type2bool(0,(_tmp366.ptr_atts).zero_term);
{struct _tuple33 _tmp347=({struct _tuple33 _tmp719;_tmp719.f1=_tmp343,_tmp719.f2=_tmp344;_tmp719;});struct _tuple33 _tmp348=_tmp347;if(_tmp348.f1 != 0){if(_tmp348.f2 != 0){_LLAC: _LLAD:
# 2185
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp87E=({void*_tmp87D=*_tmp456;Cyc_Toc_cast_it_r(_tmp87D,({struct Cyc_Absyn_Exp*_tmp349[1U];_tmp349[0]=_tmp455;({struct Cyc_Absyn_Exp*_tmp87C=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp87C,_tag_dyneither(_tmp349,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp87E;});else{
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp455->r;}
# 2194
goto _LLAB;}else{_LLAE: _LLAF: {
# 2196
struct Cyc_Absyn_Exp*_tmp34A=(struct Cyc_Absyn_Exp*)_check_null(_tmp343);
struct _tuple13 _tmp34B=Cyc_Evexp_eval_const_uint_exp(_tmp34A);struct _tuple13 _tmp34C=_tmp34B;unsigned int _tmp35C;int _tmp35B;_LLB5: _tmp35C=_tmp34C.f1;_tmp35B=_tmp34C.f2;_LLB6:;
if(Cyc_Toc_is_toplevel(nv)){
# 2202
if((_tmp345  && !(_tmp366.elt_tq).real_const) && !_tmp346)
_tmp34A=({struct Cyc_Absyn_Exp*_tmp87F=_tmp34A;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp87F,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp880=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp34A,_tmp455))->r;e->r=_tmp880;});}else{
# 2207
if(_tmp345){
# 2212
struct _tuple29 _tmp34D=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp34E=_tmp34D;struct _tuple1*_tmp359;struct Cyc_Absyn_Exp*_tmp358;_LLB8: _tmp359=_tmp34E.f1;_tmp358=_tmp34E.f2;_LLB9:;{
struct Cyc_Absyn_Exp*arg3;
# 2216
{void*_tmp34F=_tmp455->r;void*_tmp350=_tmp34F;struct Cyc_Absyn_Vardecl*_tmp353;struct Cyc_Absyn_Vardecl*_tmp352;switch(*((int*)_tmp350)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp350)->f1).Wstring_c).tag){case 8U: _LLBB: _LLBC:
 arg3=_tmp34A;goto _LLBA;case 9U: _LLBD: _LLBE:
 arg3=_tmp34A;goto _LLBA;default: goto _LLC3;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp350)->f1)){case 1U: _LLBF: _tmp352=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp350)->f1)->f1;_LLC0:
 _tmp353=_tmp352;goto _LLC2;case 4U: _LLC1: _tmp353=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp350)->f1)->f1;_LLC2:
# 2221
 if(!Cyc_Tcutil_is_array_type(_tmp353->type))
goto _LLC4;
arg3=_tmp34A;
goto _LLBA;default: goto _LLC3;}default: _LLC3: _LLC4:
# 2228
 if(_tmp35B  && _tmp35C != (unsigned int)1)
arg3=_tmp34A;else{
# 2231
arg3=({struct Cyc_Absyn_Exp*_tmp351[2U];({
struct Cyc_Absyn_Exp*_tmp882=({void*_tmp881=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp881,_tmp358);});_tmp351[0]=_tmp882;}),_tmp351[1]=_tmp34A;({struct Cyc_Absyn_Exp*_tmp883=
# 2231
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp455);Cyc_Toc_fncall_exp_dl(_tmp883,_tag_dyneither(_tmp351,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2233
goto _LLBA;}_LLBA:;}
# 2235
if(!_tmp346  && !(_tmp366.elt_tq).real_const)
# 2238
arg3=({struct Cyc_Absyn_Exp*_tmp884=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp884,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2240
struct Cyc_Absyn_Exp*_tmp354=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp366.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp355=({struct Cyc_Absyn_Exp*_tmp357[3U];_tmp357[0]=_tmp358,_tmp357[1]=_tmp354,_tmp357[2]=arg3;({struct Cyc_Absyn_Exp*_tmp885=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp885,_tag_dyneither(_tmp357,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*_tmp356=Cyc_Absyn_exp_stmt(_tmp355,0U);
_tmp356=({struct _tuple1*_tmp888=_tmp359;void*_tmp887=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp886=_tmp455;Cyc_Absyn_declare_stmt(_tmp888,_tmp887,_tmp886,_tmp356,0U);});
({void*_tmp889=Cyc_Toc_stmt_exp_r(_tmp356);e->r=_tmp889;});};};}else{
# 2247
({void*_tmp88C=({struct Cyc_Absyn_Exp*_tmp35A[3U];_tmp35A[0]=_tmp455,({
# 2249
struct Cyc_Absyn_Exp*_tmp88A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp366.elt_type),0U);_tmp35A[1]=_tmp88A;}),_tmp35A[2]=_tmp34A;({struct Cyc_Absyn_Exp*_tmp88B=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp88B,_tag_dyneither(_tmp35A,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2247
e->r=_tmp88C;});}}
# 2252
goto _LLAB;}}}else{if(_tmp348.f2 != 0){_LLB0: _LLB1: {
# 2262 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp35D=(struct Cyc_Absyn_Exp*)_check_null(_tmp344);
if(_tmp345  && !_tmp346)
_tmp35D=({struct Cyc_Absyn_Exp*_tmp88D=_tmp344;Cyc_Absyn_add_exp(_tmp88D,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2269
struct Cyc_Absyn_Exp*_tmp35E=({struct Cyc_Absyn_Exp*_tmp360[3U];_tmp360[0]=_tmp455,({
# 2271
struct Cyc_Absyn_Exp*_tmp88E=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp367.elt_type),0U);_tmp360[1]=_tmp88E;}),_tmp360[2]=_tmp35D;({struct Cyc_Absyn_Exp*_tmp88F=Cyc_Toc__untag_dyneither_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp88F,_tag_dyneither(_tmp360,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2273
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp892=({struct Cyc_Absyn_Exp*_tmp35F[1U];({struct Cyc_Absyn_Exp*_tmp890=Cyc_Absyn_copy_exp(_tmp35E);_tmp35F[0]=_tmp890;});({struct Cyc_Absyn_Exp*_tmp891=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp891,_tag_dyneither(_tmp35F,sizeof(struct Cyc_Absyn_Exp*),1U));});});_tmp35E->r=_tmp892;});
({void*_tmp893=Cyc_Toc_cast_it_r(*_tmp456,_tmp35E);e->r=_tmp893;});
goto _LLAB;};}}else{_LLB2: _LLB3:
# 2281
 if((_tmp345  && !_tmp346) && !(_tmp366.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp361=0U;({struct _dyneither_ptr _tmp894=({const char*_tmp362="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp362,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp894,_tag_dyneither(_tmp361,sizeof(void*),0U));});});
({void*_tmp898=({struct Cyc_Absyn_Exp*_tmp363[3U];_tmp363[0]=_tmp455,({
struct Cyc_Absyn_Exp*_tmp896=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp367.elt_type),0U);_tmp363[1]=_tmp896;}),({
struct Cyc_Absyn_Exp*_tmp895=Cyc_Absyn_uint_exp(1U,0U);_tmp363[2]=_tmp895;});({struct Cyc_Absyn_Exp*_tmp897=Cyc_Toc__dyneither_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp897,_tag_dyneither(_tmp363,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2284
e->r=_tmp898;});}
# 2288
goto _LLAB;}}_LLAB:;}
# 2290
goto _LLA4;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp342.f2)->f1)->tag == 1U){_LLA7: _tmp368=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp342.f1)->f1;_LLA8:
# 2292
{struct Cyc_Absyn_Exp*_tmp364=({void*_tmp899=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp899,(_tmp368.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp365=_tmp364;if(_tmp365 == 0){_LLC6: _LLC7:
# 2294
({void*_tmp89B=({struct Cyc_Absyn_Exp*_tmp89A=Cyc_Absyn_new_exp(_tmp455->r,_tmp455->loc);Cyc_Toc_aggrmember_exp_r(_tmp89A,Cyc_Toc_curr_sp);});_tmp455->r=_tmp89B;});
_tmp455->topt=new_typ_c;
goto _LLC5;}else{_LLC8: _LLC9:
 goto _LLC5;}_LLC5:;}
# 2299
goto _LLA4;}else{goto _LLA9;}default: goto _LLA9;}else{_LLA9: _LLAA:
# 2301
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp455->r;
goto _LLA4;}_LLA4:;}
# 2305
goto _LL7;}case 15U: _LL2C: _tmp457=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_LL2D:
# 2308
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp457);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp457)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp457,0,Cyc_Toc_address_lvalue,1);
# 2314
({void*_tmp89D=({void*_tmp89C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp89C,_tmp457);});e->r=_tmp89D;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp457->topt))))
# 2318
({void*_tmp89F=({void*_tmp89E=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp89E,_tmp457);});e->r=_tmp89F;});}
# 2320
goto _LL7;case 16U: _LL2E: _tmp459=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp458=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL2F:
# 2327
 Cyc_Toc_exp_to_c(nv,_tmp458);{
# 2329
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp458->topt));
struct _tuple29 _tmp369=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp36A=_tmp369;struct _tuple1*_tmp376;struct Cyc_Absyn_Exp*_tmp375;_LLCB: _tmp376=_tmp36A.f1;_tmp375=_tmp36A.f2;_LLCC:;{
struct Cyc_Absyn_Exp*lhs;
{void*_tmp36B=Cyc_Tcutil_compress(elt_typ);void*_tmp36C=_tmp36B;void*_tmp36D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->tag == 4U){_LLCE: _tmp36D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1).elt_type;_LLCF:
# 2334
 elt_typ=Cyc_Toc_typ_to_c(_tmp36D);
lhs=Cyc_Absyn_copy_exp(_tmp375);
goto _LLCD;}else{_LLD0: _LLD1:
# 2338
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp375),0U);
goto _LLCD;}_LLCD:;}{
# 2341
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp458);
struct _tuple1*_tmp36E=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp36F=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp36E),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)_tmp36F)
mexp=({struct Cyc_Absyn_Exp*_tmp370[2U];_tmp370[0]=_tmp36F,({struct Cyc_Absyn_Exp*_tmp8A0=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp370[1]=_tmp8A0;});({struct Cyc_Absyn_Exp*_tmp8A1=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp8A1,_tag_dyneither(_tmp370,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2348
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp375,0U),0U);}{
# 2351
struct Cyc_Absyn_Exp*_tmp371=Cyc_Toc_get_varsizeexp(_tmp458);
if(_tmp371 != 0)
mexp=Cyc_Absyn_add_exp(mexp,_tmp371,0U);
# 2355
if(_tmp459 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2358
struct Cyc_Absyn_Exp*r=_tmp459;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 2362
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp372[3U];({
struct Cyc_Absyn_Exp*_tmp8A4=Cyc_Absyn_copy_exp(_tmp375);_tmp372[0]=_tmp8A4;}),({
struct Cyc_Absyn_Exp*_tmp8A3=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp372[1]=_tmp8A3;}),
(unsigned int)_tmp36F?_tmp372[2]=_tmp36F:({struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Absyn_uint_exp(1U,0U);_tmp372[2]=_tmp8A2;});({struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp8A5,_tag_dyneither(_tmp372,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2369
answer=Cyc_Absyn_copy_exp(_tmp375);}
({void*_tmp8A6=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->tag=Cyc_Toc_Dest,_tmp373->f1=_tmp375;_tmp373;});e->annot=_tmp8A6;});{
struct Cyc_Absyn_Stmt*_tmp374=({struct _tuple1*_tmp8AA=_tmp376;void*_tmp8A9=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8A8=mexp;Cyc_Absyn_declare_stmt(_tmp8AA,_tmp8A9,_tmp8A8,({
struct Cyc_Absyn_Stmt*_tmp8A7=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp8A7,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2375
if((unsigned int)array_len)
_tmp374=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp36E),Cyc_Absyn_uint_type,array_len,_tmp374,0U);
({void*_tmp8AB=Cyc_Toc_stmt_exp_r(_tmp374);e->r=_tmp8AB;});
if(_tmp371 != 0)
({void*_tmp8AD=({void*_tmp8AC=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp8AC,Cyc_Absyn_copy_exp(e));});e->r=_tmp8AD;});
goto _LL7;};};};};};};case 18U: _LL30: _tmp45A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_LL31:
# 2383
 Cyc_Toc_exp_to_c(nv,_tmp45A);goto _LL7;case 17U: _LL32: _tmp45B=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_LL33:
({void*_tmp8AF=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377->tag=17U,({void*_tmp8AE=Cyc_Toc_typ_to_c(_tmp45B);_tmp377->f1=_tmp8AE;});_tmp377;});e->r=_tmp8AF;});goto _LL7;case 19U: _LL34: _tmp45D=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp45C=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL35: {
# 2386
void*_tmp378=_tmp45D;
struct Cyc_List_List*_tmp379=_tmp45C;
for(0;_tmp379 != 0;_tmp379=_tmp379->tl){
void*_tmp37A=(void*)_tmp379->hd;void*_tmp37B=_tmp37A;unsigned int _tmp38D;struct _dyneither_ptr*_tmp38C;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp37B)->tag == 0U){_LLD3: _tmp38C=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp37B)->f1;_LLD4:
 goto _LLD2;}else{_LLD5: _tmp38D=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp37B)->f1;_LLD6:
# 2392
{void*_tmp37C=Cyc_Tcutil_compress(_tmp378);void*_tmp37D=_tmp37C;struct Cyc_List_List*_tmp38B;struct Cyc_List_List*_tmp38A;struct Cyc_Absyn_Datatypefield*_tmp389;union Cyc_Absyn_AggrInfo _tmp388;switch(*((int*)_tmp37D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D)->f1)){case 20U: _LLD8: _tmp388=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D)->f1)->f1;_LLD9: {
# 2394
struct Cyc_Absyn_Aggrdecl*_tmp37E=Cyc_Absyn_get_known_aggrdecl(_tmp388);
if(_tmp37E->impl == 0)
({void*_tmp37F=0U;({struct _dyneither_ptr _tmp8B0=({const char*_tmp380="struct fields must be known";_tag_dyneither(_tmp380,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8B0,_tag_dyneither(_tmp37F,sizeof(void*),0U));});});
_tmp38A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp37E->impl))->fields;goto _LLDB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D)->f1)->f1).KnownDatatypefield).tag == 2){_LLDE: _tmp389=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D)->f1)->f1).KnownDatatypefield).val).f2;_LLDF:
# 2408
 if(_tmp38D == (unsigned int)0)
({void*_tmp8B1=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384->tag=0U,_tmp384->f1=Cyc_Toc_tag_sp;_tmp384;}));_tmp379->hd=_tmp8B1;});else{
# 2411
_tmp378=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp389->typs,(int)(_tmp38D - (unsigned int)1))).f2;
({void*_tmp8B3=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->tag=0U,({struct _dyneither_ptr*_tmp8B2=Cyc_Absyn_fieldname((int)_tmp38D);_tmp385->f1=_tmp8B2;});_tmp385;}));_tmp379->hd=_tmp8B3;});}
# 2414
goto _LLD7;}else{goto _LLE0;}default: goto _LLE0;}case 7U: _LLDA: _tmp38A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp37D)->f2;_LLDB: {
# 2399
struct Cyc_Absyn_Aggrfield*_tmp381=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp38A,(int)_tmp38D);
({void*_tmp8B4=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382->tag=0U,_tmp382->f1=_tmp381->name;_tmp382;}));_tmp379->hd=_tmp8B4;});
_tmp378=_tmp381->type;
goto _LLD7;}case 6U: _LLDC: _tmp38B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp37D)->f1;_LLDD:
# 2404
({void*_tmp8B6=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383->tag=0U,({struct _dyneither_ptr*_tmp8B5=Cyc_Absyn_fieldname((int)(_tmp38D + (unsigned int)1));_tmp383->f1=_tmp8B5;});_tmp383;}));_tmp379->hd=_tmp8B6;});
_tmp378=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp38B,(int)_tmp38D)).f2;
goto _LLD7;default: _LLE0: _LLE1:
# 2415
({void*_tmp386=0U;({struct _dyneither_ptr _tmp8B7=({const char*_tmp387="impossible type for offsetof tuple index";_tag_dyneither(_tmp387,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8B7,_tag_dyneither(_tmp386,sizeof(void*),0U));});});}_LLD7:;}
# 2417
goto _LLD2;}_LLD2:;}
# 2420
({void*_tmp8B9=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E->tag=19U,({void*_tmp8B8=Cyc_Toc_typ_to_c(_tmp45D);_tmp38E->f1=_tmp8B8;}),_tmp38E->f2=_tmp45C;_tmp38E;});e->r=_tmp8B9;});
goto _LL7;}case 21U: _LL36: _tmp461=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp460=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp45F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3;_tmp45E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DF)->f4;_LL37: {
# 2423
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp461->topt);
Cyc_Toc_exp_to_c(nv,_tmp461);
if(_tmp45F  && _tmp45E)
({void*_tmp8BE=({struct Cyc_Absyn_Exp*_tmp8BD=_tmp461;void*_tmp8BC=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp8BB=e1_cyc_type;struct _dyneither_ptr*_tmp8BA=_tmp460;Cyc_Toc_check_tagged_union(_tmp8BD,_tmp8BC,_tmp8BB,_tmp8BA,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2427
e->r=_tmp8BE;});
# 2429
if(is_poly)
({void*_tmp8C0=({void*_tmp8BF=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8BF,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp8C0;});
goto _LL7;}case 22U: _LL38: _tmp465=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp464=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp463=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3;_tmp462=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DF)->f4;_LL39: {
# 2433
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(_tmp465->topt);
void*ta;
{void*_tmp38F=Cyc_Tcutil_compress(e1typ);void*_tmp390=_tmp38F;struct Cyc_Absyn_PtrInfo _tmp393;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp390)->tag == 3U){_LLE3: _tmp393=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp390)->f1;_LLE4:
 ta=_tmp393.elt_type;goto _LLE2;}else{_LLE5: _LLE6:
({void*_tmp391=0U;({struct _dyneither_ptr _tmp8C1=({const char*_tmp392="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp392,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8C1,_tag_dyneither(_tmp391,sizeof(void*),0U));});});}_LLE2:;}
# 2440
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp465,e->annot,0);
if(_tmp463  && _tmp462)
({void*_tmp8C6=({struct Cyc_Absyn_Exp*_tmp8C5=_tmp465;void*_tmp8C4=Cyc_Toc_typ_to_c(e1typ);void*_tmp8C3=ta;struct _dyneither_ptr*_tmp8C2=_tmp464;Cyc_Toc_check_tagged_union(_tmp8C5,_tmp8C4,_tmp8C3,_tmp8C2,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp8C6;});
if(is_poly  && _tmp462)
({void*_tmp8C8=({void*_tmp8C7=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8C7,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp8C8;});
goto _LL7;}case 20U: _LL3A: _tmp466=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_LL3B:
# 2448
 did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp466,e->annot,0);
goto _LL7;case 23U: _LL3C: _tmp468=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp467=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL3D:
# 2452
{void*_tmp394=Cyc_Tcutil_compress((void*)_check_null(_tmp468->topt));void*_tmp395=_tmp394;struct Cyc_List_List*_tmp39D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp395)->tag == 6U){_LLE8: _tmp39D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp395)->f1;_LLE9:
# 2454
 Cyc_Toc_exp_to_c(nv,_tmp468);{
int _tmp396=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp467);{
struct _tuple13 _tmp397=Cyc_Evexp_eval_const_uint_exp(_tmp467);struct _tuple13 _tmp398=_tmp397;unsigned int _tmp39C;int _tmp39B;_LLED: _tmp39C=_tmp398.f1;_tmp39B=_tmp398.f2;_LLEE:;
if(!_tmp39B)
({void*_tmp399=0U;({struct _dyneither_ptr _tmp8C9=({const char*_tmp39A="unknown tuple subscript in translation to C";_tag_dyneither(_tmp39A,sizeof(char),44U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp8C9,_tag_dyneither(_tmp399,sizeof(void*),0U));});});
({void*_tmp8CB=({struct Cyc_Absyn_Exp*_tmp8CA=_tmp468;Cyc_Toc_aggrmember_exp_r(_tmp8CA,Cyc_Absyn_fieldname((int)(_tmp39C + (unsigned int)1)));});e->r=_tmp8CB;});
goto _LLE7;};};}else{_LLEA: _LLEB:
# 2464
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,_tmp468,e->annot,_tmp467);
if(index_used)
({void*_tmp8CC=Cyc_Toc_deref_exp_r(_tmp468);e->r=_tmp8CC;});
goto _LLE7;};}_LLE7:;}
# 2470
goto _LL7;case 24U: _LL3E: _tmp469=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_LL3F:
# 2472
 if(!Cyc_Toc_is_toplevel(nv)){
# 2474
void*_tmp39E=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp469));
{void*_tmp39F=_tmp39E;union Cyc_Absyn_AggrInfo _tmp3A4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39F)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39F)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39F)->f2 == 0){_LLF0: _tmp3A4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39F)->f1)->f1;_LLF1: {
# 2477
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3A4);
{int i=1;for(0;_tmp469 != 0;(_tmp469=_tmp469->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp469->hd);
dles=({struct Cyc_List_List*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({struct _tuple19*_tmp8CE=({struct _dyneither_ptr*_tmp8CD=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp8CD,(struct Cyc_Absyn_Exp*)_tmp469->hd);});_tmp3A0->hd=_tmp8CE;}),_tmp3A0->tl=dles;_tmp3A0;});}}
# 2483
({void*_tmp8D0=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->tag=29U,_tmp3A1->f1=sd->name,_tmp3A1->f2=0,({struct Cyc_List_List*_tmp8CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp3A1->f3=_tmp8CF;}),_tmp3A1->f4=sd;_tmp3A1;});e->r=_tmp8D0;});
e->topt=_tmp39E;
goto _LLEF;}}else{goto _LLF2;}}else{goto _LLF2;}}else{_LLF2: _LLF3:
({void*_tmp3A2=0U;({struct _dyneither_ptr _tmp8D1=({const char*_tmp3A3="tuple type not an aggregate";_tag_dyneither(_tmp3A3,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D1,_tag_dyneither(_tmp3A2,sizeof(void*),0U));});});}_LLEF:;}
# 2488
goto _LL7;}else{
# 2492
struct Cyc_List_List*dles=0;
for(0;_tmp469 != 0;_tmp469=_tmp469->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp469->hd);
dles=({struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct _tuple19*_tmp8D2=({struct _tuple19*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->f1=0,_tmp3A5->f2=(struct Cyc_Absyn_Exp*)_tmp469->hd;_tmp3A5;});_tmp3A6->hd=_tmp8D2;}),_tmp3A6->tl=dles;_tmp3A6;});}
# 2497
({void*_tmp8D3=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp8D3;});}
# 2499
goto _LL7;case 26U: _LL40: _tmp46A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_LL41:
# 2505
{struct Cyc_List_List*_tmp3A7=_tmp46A;for(0;_tmp3A7 != 0;_tmp3A7=_tmp3A7->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3A7->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8D4=Cyc_Toc_unresolvedmem_exp_r(0,_tmp46A);e->r=_tmp8D4;});
goto _LL7;case 27U: _LL42: _tmp46E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp46D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp46C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3;_tmp46B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2DF)->f4;_LL43:
# 2513
 Cyc_Toc_exp_to_c(nv,_tmp46D);
Cyc_Toc_exp_to_c(nv,_tmp46C);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp3A8=Cyc_Evexp_eval_const_uint_exp(_tmp46D);struct _tuple13 _tmp3A9=_tmp3A8;unsigned int _tmp3B3;int _tmp3B2;_LLF5: _tmp3B3=_tmp3A9.f1;_tmp3B2=_tmp3A9.f2;_LLF6:;{
void*_tmp3AA=Cyc_Toc_typ_to_c((void*)_check_null(_tmp46C->topt));
struct Cyc_List_List*es=0;
# 2520
if(!Cyc_Toc_is_zero(_tmp46C)){
if(!_tmp3B2)
({void*_tmp3AB=0U;({unsigned int _tmp8D6=_tmp46D->loc;struct _dyneither_ptr _tmp8D5=({const char*_tmp3AC="cannot determine value of constant";_tag_dyneither(_tmp3AC,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp8D6,_tmp8D5,_tag_dyneither(_tmp3AB,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp3B3;++ i){
es=({struct Cyc_List_List*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));({struct _tuple19*_tmp8D7=({struct _tuple19*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD->f1=0,_tmp3AD->f2=_tmp46C;_tmp3AD;});_tmp3AE->hd=_tmp8D7;}),_tmp3AE->tl=es;_tmp3AE;});}}
# 2526
if(_tmp46B){
struct Cyc_Absyn_Exp*_tmp3AF=({void*_tmp8D8=_tmp3AA;Cyc_Toc_cast_it(_tmp8D8,Cyc_Absyn_uint_exp(0U,0U));});
es=({struct Cyc_List_List*_tmp8DA=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8DA,({struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));({struct _tuple19*_tmp8D9=({struct _tuple19*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0->f1=0,_tmp3B0->f2=_tmp3AF;_tmp3B0;});_tmp3B1->hd=_tmp8D9;}),_tmp3B1->tl=0;_tmp3B1;}));});}}
# 2531
({void*_tmp8DB=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp8DB;});};}
# 2533
goto _LL7;case 28U: _LL44: _tmp471=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp470=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp46F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3;_LL45:
# 2540
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8DC=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp8DC;});else{
# 2543
Cyc_Toc_exp_to_c(nv,_tmp471);}
goto _LL7;case 30U: _LL46: _tmp473=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp472=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL47:
# 2547
{struct Cyc_List_List*_tmp3B4=_tmp472;for(0;_tmp3B4 != 0;_tmp3B4=_tmp3B4->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3B4->hd)).f2);}}{
void*_tmp3B5=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp3B5;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3B6=Cyc_Tcutil_compress(_tmp3B5);void*_tmp3B7=_tmp3B6;union Cyc_Absyn_AggrInfo _tmp3BB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B7)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B7)->f2 == 0){_LLF8: _tmp3BB=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B7)->f1)->f1;_LLF9: {
# 2554
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3BB);
({void*_tmp8DD=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->tag=29U,_tmp3B8->f1=sd->name,_tmp3B8->f2=0,_tmp3B8->f3=_tmp472,_tmp3B8->f4=sd;_tmp3B8;});e->r=_tmp8DD;});
e->topt=_tmp3B5;
goto _LLF7;}}else{goto _LLFA;}}else{goto _LLFA;}}else{_LLFA: _LLFB:
({void*_tmp3B9=0U;({struct _dyneither_ptr _tmp8DE=({const char*_tmp3BA="anonStruct type not an aggregate";_tag_dyneither(_tmp3BA,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8DE,_tag_dyneither(_tmp3B9,sizeof(void*),0U));});});}_LLF7:;}else{
# 2561
({void*_tmp8DF=Cyc_Toc_unresolvedmem_exp_r(0,_tmp472);e->r=_tmp8DF;});}
goto _LL7;};case 29U: _LL48: _tmp477=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp476=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp475=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3;_tmp474=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2DF)->f4;_LL49:
# 2567
 if(_tmp474 == 0  || _tmp474->impl == 0)
({void*_tmp3BC=0U;({struct _dyneither_ptr _tmp8E0=({const char*_tmp3BD="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp3BD,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8E0,_tag_dyneither(_tmp3BC,sizeof(void*),0U));});});{
void*_tmp3BE=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp3BF=Cyc_Tcutil_compress(_tmp3BE);void*_tmp3C0=_tmp3BF;union Cyc_Absyn_AggrInfo _tmp3C3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->f1)->tag == 20U){_LLFD: _tmp3C3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->f1)->f1;_LLFE:
({struct _tuple1*_tmp8E1=(Cyc_Absyn_aggr_kinded_name(_tmp3C3)).f2;*_tmp477=_tmp8E1;});goto _LLFC;}else{goto _LLFF;}}else{_LLFF: _LL100:
({void*_tmp3C1=0U;({struct _dyneither_ptr _tmp8E2=({const char*_tmp3C2="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp3C2,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8E2,_tag_dyneither(_tmp3C1,sizeof(void*),0U));});});}_LLFC:;}{
# 2578
struct Cyc_List_List*_tmp3C4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp474->impl))->fields;
if(_tmp3C4 == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(_tmp3C4))->tl != 0;_tmp3C4=_tmp3C4->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp3C5=(struct Cyc_Absyn_Aggrfield*)_tmp3C4->hd;
struct Cyc_List_List*_tmp3C6=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp475,_tmp474->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp474->impl))->fields);
# 2585
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3C7=Cyc_Tcutil_compress(old_typ);void*_tmp3C8=_tmp3C7;struct Cyc_List_List*_tmp3F4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->f1)->tag == 20U){_LL102: _tmp3F4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->f2;_LL103:
# 2589
{struct Cyc_List_List*_tmp3C9=_tmp3C6;for(0;_tmp3C9 != 0;_tmp3C9=_tmp3C9->tl){
struct _tuple34*_tmp3CA=(struct _tuple34*)_tmp3C9->hd;struct _tuple34*_tmp3CB=_tmp3CA;struct Cyc_Absyn_Aggrfield*_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F0;_LL107: _tmp3F1=_tmp3CB->f1;_tmp3F0=_tmp3CB->f2;_LL108:;{
void*_tmp3CC=_tmp3F0->topt;
Cyc_Toc_exp_to_c(nv,_tmp3F0);
# 2594
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp3F1->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp3F0->topt)))
({void*_tmp8E4=({void*_tmp8E3=Cyc_Toc_typ_to_c(_tmp3F1->type);Cyc_Toc_cast_it(_tmp8E3,
Cyc_Absyn_copy_exp(_tmp3F0));})->r;
# 2596
_tmp3F0->r=_tmp8E4;});
# 2599
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp474->impl))->tagged){
struct _dyneither_ptr*_tmp3CD=_tmp3F1->name;
struct Cyc_Absyn_Exp*_tmp3CE=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp474,_tmp3CD),0U);
struct _tuple19*_tmp3CF=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp3CE);
struct _tuple19*_tmp3D0=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp3F0);
struct _tuple1*s=({struct _tuple1*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));({union Cyc_Absyn_Nmspace _tmp8E8=Cyc_Absyn_Abs_n(0,1);_tmp3DB->f1=_tmp8E8;}),({
struct _dyneither_ptr*_tmp8E7=({struct _dyneither_ptr*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));({struct _dyneither_ptr _tmp8E6=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D8=({struct Cyc_String_pa_PrintArg_struct _tmp71C;_tmp71C.tag=0U,_tmp71C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp474->name).f2);_tmp71C;});struct Cyc_String_pa_PrintArg_struct _tmp3D9=({struct Cyc_String_pa_PrintArg_struct _tmp71B;_tmp71B.tag=0U,_tmp71B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3CD);_tmp71B;});void*_tmp3D6[2U];_tmp3D6[0]=& _tmp3D8,_tmp3D6[1]=& _tmp3D9;({struct _dyneither_ptr _tmp8E5=({const char*_tmp3D7="_union_%s_%s";_tag_dyneither(_tmp3D7,sizeof(char),13U);});Cyc_aprintf(_tmp8E5,_tag_dyneither(_tmp3D6,sizeof(void*),2U));});});*_tmp3DA=_tmp8E6;});_tmp3DA;});_tmp3DB->f2=_tmp8E7;});_tmp3DB;});
# 2607
struct _tuple19*_tmp3D1=({
struct _dyneither_ptr*_tmp8EA=_tmp3CD;Cyc_Toc_make_field(_tmp8EA,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->tag=29U,_tmp3D5->f1=s,_tmp3D5->f2=0,({struct Cyc_List_List*_tmp8E9=({struct _tuple19*_tmp3D4[2U];_tmp3D4[0]=_tmp3CF,_tmp3D4[1]=_tmp3D0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3D4,sizeof(struct _tuple19*),2U));});_tmp3D5->f3=_tmp8E9;}),_tmp3D5->f4=0;_tmp3D5;}),0U));});
# 2610
({void*_tmp8EC=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->tag=29U,_tmp3D3->f1=*_tmp477,_tmp3D3->f2=0,({struct Cyc_List_List*_tmp8EB=({struct _tuple19*_tmp3D2[1U];_tmp3D2[0]=_tmp3D1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3D2,sizeof(struct _tuple19*),1U));});_tmp3D3->f3=_tmp8EB;}),_tmp3D3->f4=_tmp474;_tmp3D3;});e->r=_tmp8EC;});}
# 2616
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp3C5 == _tmp3F1){
struct Cyc_List_List*_tmp3DC=({struct Cyc_List_List*_tmp8ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp474->impl))->exist_vars,_tmp476);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8ED,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp474->tvs,_tmp3F4));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp3DD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp474->impl))->fields;for(0;_tmp3DD != 0;_tmp3DD=_tmp3DD->tl){
struct Cyc_Absyn_Aggrfield*_tmp3DE=(struct Cyc_Absyn_Aggrfield*)_tmp3DD->hd;
void*_tmp3DF=Cyc_Tcutil_substitute(_tmp3DC,_tmp3DE->type);
struct Cyc_Absyn_Aggrfield*_tmp3E0=({struct Cyc_Absyn_Aggrfield*_tmp8EE=_tmp3DE;Cyc_Toc_aggrfield_to_c(_tmp8EE,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp3DC,_tmp3DF)));});
new_fields=({struct Cyc_List_List*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1->hd=_tmp3E0,_tmp3E1->tl=new_fields;_tmp3E1;});
# 2631
if(_tmp3DD->tl == 0){
{void*_tmp3E2=Cyc_Tcutil_compress(_tmp3E0->type);void*_tmp3E3=_tmp3E2;struct Cyc_Absyn_ArrayInfo _tmp3E8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E3)->tag == 4U){_LL10A: _tmp3E8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E3)->f1;_LL10B:
# 2634
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3E8.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp3E4=_tmp3E8;
({struct Cyc_Absyn_Exp*_tmp8EF=Cyc_Absyn_uint_exp(0U,0U);_tmp3E4.num_elts=_tmp8EF;});
({void*_tmp8F0=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->tag=4U,_tmp3E5->f1=_tmp3E4;_tmp3E5;});_tmp3E0->type=_tmp8F0;});}
# 2639
goto _LL109;}else{_LL10C: _LL10D:
# 2643
 if(_tmp3F0->topt == 0)
goto _LL109;
{void*_tmp3E6=Cyc_Tcutil_compress((void*)_check_null(_tmp3F0->topt));void*_tmp3E7=_tmp3E6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E7)->f1)->tag == 20U){_LL10F: _LL110:
# 2647
 _tmp3E0->type=(void*)_check_null(_tmp3F0->topt);goto _LL10E;}else{goto _LL111;}}else{_LL111: _LL112:
 goto _LL10E;}_LL10E:;}
# 2650
goto _LL109;}_LL109:;}
# 2654
if(!Cyc_Tcutil_is_array_type(_tmp3DE->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3DE->type)))
({struct Cyc_List_List*_tmp8F2=({struct Cyc_List_List*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));({void*_tmp8F1=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9->tag=6U,_tmp3E9->f1=0;_tmp3E9;});_tmp3EA->hd=_tmp8F1;}),_tmp3EA->tl=_tmp3E0->attributes;_tmp3EA;});_tmp3E0->attributes=_tmp8F2;});}}}
# 2660
_tmp474=({struct _dyneither_ptr*_tmp8F5=({struct _dyneither_ptr*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));({struct _dyneither_ptr _tmp8F4=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3ED=({struct Cyc_Int_pa_PrintArg_struct _tmp71D;_tmp71D.tag=1U,_tmp71D.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp71D;});void*_tmp3EB[1U];_tmp3EB[0]=& _tmp3ED;({struct _dyneither_ptr _tmp8F3=({const char*_tmp3EC="_genStruct%d";_tag_dyneither(_tmp3EC,sizeof(char),13U);});Cyc_aprintf(_tmp8F3,_tag_dyneither(_tmp3EB,sizeof(void*),1U));});});*_tmp3EE=_tmp8F4;});_tmp3EE;});Cyc_Toc_make_c_struct_defn(_tmp8F5,
# 2662
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*_tmp477=_tmp474->name;
Cyc_Toc_aggrdecl_to_c(_tmp474);
# 2666
({void*_tmp8F6=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3EF=_cycalloc(sizeof(*_tmp3EF));*_tmp3EF=_tmp474;_tmp3EF;})),0);e->topt=_tmp8F6;});}};}}
# 2669
goto _LL101;}else{goto _LL104;}}else{_LL104: _LL105:
({void*_tmp3F2=0U;({struct _dyneither_ptr _tmp8F7=({const char*_tmp3F3="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp3F3,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F7,_tag_dyneither(_tmp3F2,sizeof(void*),0U));});});}_LL101:;}else{
# 2678
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp474->impl))->tagged){
# 2680
struct _tuple34*_tmp3F5=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp3C6))->hd;struct _tuple34*_tmp3F6=_tmp3F5;struct Cyc_Absyn_Aggrfield*_tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;_LL114: _tmp3FF=_tmp3F6->f1;_tmp3FE=_tmp3F6->f2;_LL115:;{
void*_tmp3F7=(void*)_check_null(_tmp3FE->topt);
void*_tmp3F8=_tmp3FF->type;
Cyc_Toc_exp_to_c(nv,_tmp3FE);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp3F8) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp3F7))
({void*_tmp8F9=({
void*_tmp8F8=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp8F8,Cyc_Absyn_new_exp(_tmp3FE->r,0U));});
# 2686
_tmp3FE->r=_tmp8F9;});{
# 2689
int i=Cyc_Toc_get_member_offset(_tmp474,_tmp3FF->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp3F9=({struct _tuple19*_tmp3FB[2U];({struct _tuple19*_tmp8FB=({struct _tuple19*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->f1=0,_tmp3FC->f2=field_tag_exp;_tmp3FC;});_tmp3FB[0]=_tmp8FB;}),({struct _tuple19*_tmp8FA=({struct _tuple19*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->f1=0,_tmp3FD->f2=_tmp3FE;_tmp3FD;});_tmp3FB[1]=_tmp8FA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3FB,sizeof(struct _tuple19*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp3F9,0U);
({void*_tmp8FD=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp3FA[1U];({struct _tuple19*_tmp8FC=Cyc_Toc_make_field(_tmp3FF->name,umem);_tmp3FA[0]=_tmp8FC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3FA,sizeof(struct _tuple19*),1U));}));e->r=_tmp8FD;});};};}else{
# 2696
struct Cyc_List_List*_tmp400=0;
struct Cyc_List_List*_tmp401=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp474->impl))->fields;
for(0;_tmp401 != 0;_tmp401=_tmp401->tl){
struct Cyc_List_List*_tmp402=_tmp3C6;for(0;_tmp402 != 0;_tmp402=_tmp402->tl){
if((*((struct _tuple34*)_tmp402->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp401->hd){
struct _tuple34*_tmp403=(struct _tuple34*)_tmp402->hd;struct _tuple34*_tmp404=_tmp403;struct Cyc_Absyn_Aggrfield*_tmp40A;struct Cyc_Absyn_Exp*_tmp409;_LL117: _tmp40A=_tmp404->f1;_tmp409=_tmp404->f2;_LL118:;{
void*_tmp405=Cyc_Toc_typ_to_c(_tmp40A->type);
void*_tmp406=Cyc_Toc_typ_to_c((void*)_check_null(_tmp409->topt));
Cyc_Toc_exp_to_c(nv,_tmp409);
# 2706
if(!Cyc_Tcutil_unify(_tmp405,_tmp406)){
# 2708
if(!Cyc_Tcutil_is_arithmetic_type(_tmp405) || !
Cyc_Tcutil_is_arithmetic_type(_tmp406))
_tmp409=({void*_tmp8FE=_tmp405;Cyc_Toc_cast_it(_tmp8FE,Cyc_Absyn_copy_exp(_tmp409));});}
_tmp400=({struct Cyc_List_List*_tmp408=_cycalloc(sizeof(*_tmp408));({struct _tuple19*_tmp8FF=({struct _tuple19*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->f1=0,_tmp407->f2=_tmp409;_tmp407;});_tmp408->hd=_tmp8FF;}),_tmp408->tl=_tmp400;_tmp408;});
break;};}}}
# 2715
({void*_tmp900=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp400));e->r=_tmp900;});}}
# 2718
goto _LL7;};};};case 31U: _LL4A: _tmp47A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp479=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_tmp478=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2DF)->f3;_LL4B: {
# 2722
struct Cyc_List_List*_tmp40B=_tmp478->typs;
{struct Cyc_List_List*_tmp40C=_tmp47A;for(0;_tmp40C != 0;(_tmp40C=_tmp40C->tl,_tmp40B=_tmp40B->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp40C->hd;
void*_tmp40D=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp40B))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp40D))
({void*_tmp901=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp901;});}}{
# 2733
struct Cyc_Absyn_Exp*_tmp40E=
_tmp479->is_extensible?Cyc_Absyn_var_exp(_tmp478->name,0U): Cyc_Toc_datatype_tag(_tmp479,_tmp478->name);
# 2736
if(!Cyc_Toc_is_toplevel(nv)){
# 2738
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp47A != 0;(_tmp47A=_tmp47A->tl,++ i)){
dles=({struct Cyc_List_List*_tmp40F=_cycalloc(sizeof(*_tmp40F));({struct _tuple19*_tmp903=({struct _dyneither_ptr*_tmp902=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp902,(struct Cyc_Absyn_Exp*)_tmp47A->hd);});_tmp40F->hd=_tmp903;}),_tmp40F->tl=dles;_tmp40F;});}}{
# 2744
struct _tuple19*_tmp410=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp40E);
({void*_tmp907=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->tag=29U,({struct _tuple1*_tmp906=Cyc_Toc_collapse_qvars(_tmp478->name,_tmp479->name);_tmp412->f1=_tmp906;}),_tmp412->f2=0,({
struct Cyc_List_List*_tmp905=({struct Cyc_List_List*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->hd=_tmp410,({struct Cyc_List_List*_tmp904=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp411->tl=_tmp904;});_tmp411;});_tmp412->f3=_tmp905;}),_tmp412->f4=0;_tmp412;});
# 2745
e->r=_tmp907;});};}else{
# 2750
struct Cyc_List_List*_tmp413=0;
for(0;_tmp47A != 0;_tmp47A=_tmp47A->tl){
_tmp413=({struct Cyc_List_List*_tmp415=_cycalloc(sizeof(*_tmp415));({struct _tuple19*_tmp908=({struct _tuple19*_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->f1=0,_tmp414->f2=(struct Cyc_Absyn_Exp*)_tmp47A->hd;_tmp414;});_tmp415->hd=_tmp908;}),_tmp415->tl=_tmp413;_tmp415;});}
({void*_tmp90B=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp417=_cycalloc(sizeof(*_tmp417));({struct _tuple19*_tmp90A=({struct _tuple19*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->f1=0,_tmp416->f2=_tmp40E;_tmp416;});_tmp417->hd=_tmp90A;}),({
struct Cyc_List_List*_tmp909=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp413);_tmp417->tl=_tmp909;});_tmp417;}));
# 2753
e->r=_tmp90B;});}
# 2756
goto _LL7;};}case 32U: _LL4C: _LL4D:
# 2758
 goto _LL4F;case 33U: _LL4E: _LL4F:
 goto _LL7;case 34U: _LL50: _tmp480=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1).is_calloc;_tmp47F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1).rgn;_tmp47E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1).elt_type;_tmp47D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1).num_elts;_tmp47C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1).fat_result;_tmp47B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1).inline_call;_LL51: {
# 2762
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp47E)));
Cyc_Toc_exp_to_c(nv,_tmp47D);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=_tmp47D;
if(_tmp47C){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2771
if(_tmp480){
if(_tmp47F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp47F;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=({struct Cyc_Absyn_Exp*_tmp418[3U];_tmp418[0]=rgn,({
struct Cyc_Absyn_Exp*_tmp90C=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp418[1]=_tmp90C;}),_tmp418[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp90D=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp90D,_tag_dyneither(_tmp418,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2778
pexp=({void*_tmp90F=*_tmp47E;struct Cyc_Absyn_Exp*_tmp90E=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp90F,_tmp90E,num_elts2);});}}else{
# 2780
if(_tmp47F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp47F;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp47B)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,num_elts2);else{
# 2786
pexp=Cyc_Toc_rmalloc_exp(rgn,num_elts2);}}else{
# 2788
pexp=Cyc_Toc_malloc_exp(*_tmp47E,num_elts2);}}
# 2790
if(_tmp47C){
struct Cyc_Absyn_Exp*elt_sz=_tmp480?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp419[3U];_tmp419[0]=pexp,_tmp419[1]=elt_sz,_tmp419[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp910=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_dl(_tmp910,_tag_dyneither(_tmp419,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp913=(struct _tuple1*)_check_null(x);void*_tmp912=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp911=_tmp47D;Cyc_Absyn_declare_stmt(_tmp913,_tmp912,_tmp911,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp914=Cyc_Toc_stmt_exp_r(s);e->r=_tmp914;});}else{
# 2797
e->r=pexp->r;}
goto _LL7;};}case 35U: _LL52: _tmp482=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp481=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL53: {
# 2807
void*e1_old_typ=(void*)_check_null(_tmp482->topt);
void*e2_old_typ=(void*)_check_null(_tmp481->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp41A=0U;({struct _dyneither_ptr _tmp915=({const char*_tmp41B="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp41B,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp915,_tag_dyneither(_tmp41A,sizeof(void*),0U));});});{
# 2815
unsigned int _tmp41C=e->loc;
struct _tuple1*_tmp41D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp41E=Cyc_Absyn_var_exp(_tmp41D,_tmp41C);_tmp41E->topt=e1_old_typ;{
struct _tuple1*_tmp41F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp420=Cyc_Absyn_var_exp(_tmp41F,_tmp41C);_tmp420->topt=e2_old_typ;{
# 2821
struct Cyc_Absyn_Exp*_tmp421=({struct Cyc_Absyn_Exp*_tmp917=Cyc_Tcutil_deep_copy_exp(1,_tmp482);struct Cyc_Absyn_Exp*_tmp916=_tmp420;Cyc_Absyn_assign_exp(_tmp917,_tmp916,_tmp41C);});_tmp421->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp422=Cyc_Absyn_exp_stmt(_tmp421,_tmp41C);
struct Cyc_Absyn_Exp*_tmp423=({struct Cyc_Absyn_Exp*_tmp919=Cyc_Tcutil_deep_copy_exp(1,_tmp481);struct Cyc_Absyn_Exp*_tmp918=_tmp41E;Cyc_Absyn_assign_exp(_tmp919,_tmp918,_tmp41C);});_tmp423->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp424=Cyc_Absyn_exp_stmt(_tmp423,_tmp41C);
# 2826
struct Cyc_Absyn_Stmt*_tmp425=({struct _tuple1*_tmp921=_tmp41D;void*_tmp920=e1_old_typ;struct Cyc_Absyn_Exp*_tmp91F=_tmp482;struct Cyc_Absyn_Stmt*_tmp91E=({
struct _tuple1*_tmp91D=_tmp41F;void*_tmp91C=e2_old_typ;struct Cyc_Absyn_Exp*_tmp91B=_tmp481;struct Cyc_Absyn_Stmt*_tmp91A=
Cyc_Absyn_seq_stmt(_tmp422,_tmp424,_tmp41C);
# 2827
Cyc_Absyn_declare_stmt(_tmp91D,_tmp91C,_tmp91B,_tmp91A,_tmp41C);});
# 2826
Cyc_Absyn_declare_stmt(_tmp921,_tmp920,_tmp91F,_tmp91E,_tmp41C);});
# 2829
Cyc_Toc_stmt_to_c(nv,_tmp425);
({void*_tmp922=Cyc_Toc_stmt_exp_r(_tmp425);e->r=_tmp922;});
goto _LL7;};};};};};}case 38U: _LL54: _tmp484=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp483=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL55: {
# 2834
void*_tmp426=Cyc_Tcutil_compress((void*)_check_null(_tmp484->topt));
Cyc_Toc_exp_to_c(nv,_tmp484);
{void*_tmp427=_tmp426;struct Cyc_Absyn_Aggrdecl*_tmp42D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp427)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp427)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp427)->f1)->f1).KnownAggr).tag == 2){_LL11A: _tmp42D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp427)->f1)->f1).KnownAggr).val;_LL11B: {
# 2838
struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp42D,_tmp483),0U);
struct Cyc_Absyn_Exp*_tmp429=Cyc_Toc_member_exp(_tmp484,_tmp483,0U);
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Toc_member_exp(_tmp429,Cyc_Toc_tag_sp,0U);
({void*_tmp923=(Cyc_Absyn_eq_exp(_tmp42A,_tmp428,0U))->r;e->r=_tmp923;});
goto _LL119;}}else{goto _LL11C;}}else{goto _LL11C;}}else{_LL11C: _LL11D:
({void*_tmp42B=0U;({struct _dyneither_ptr _tmp924=({const char*_tmp42C="non-aggregate type in tagcheck";_tag_dyneither(_tmp42C,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp924,_tag_dyneither(_tmp42B,sizeof(void*),0U));});});}_LL119:;}
# 2845
goto _LL7;}case 37U: _LL56: _tmp485=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_LL57:
 Cyc_Toc_stmt_to_c(nv,_tmp485);goto _LL7;case 36U: _LL58: _LL59:
# 2848
({void*_tmp42E=0U;({struct _dyneither_ptr _tmp925=({const char*_tmp42F="UnresolvedMem";_tag_dyneither(_tmp42F,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp925,_tag_dyneither(_tmp42E,sizeof(void*),0U));});});case 25U: _LL5A: _LL5B:
({void*_tmp430=0U;({struct _dyneither_ptr _tmp926=({const char*_tmp431="compoundlit";_tag_dyneither(_tmp431,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp926,_tag_dyneither(_tmp430,sizeof(void*),0U));});});case 39U: _LL5C: _LL5D:
({void*_tmp432=0U;({struct _dyneither_ptr _tmp927=({const char*_tmp433="valueof(-)";_tag_dyneither(_tmp433,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp927,_tag_dyneither(_tmp432,sizeof(void*),0U));});});default: _LL5E: _LL5F:
({void*_tmp434=0U;({struct _dyneither_ptr _tmp928=({const char*_tmp435="__asm__";_tag_dyneither(_tmp435,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp928,_tag_dyneither(_tmp434,sizeof(void*),0U));});});}_LL7:;}
# 2853
if(!did_inserted_checks)
({void*_tmp486=0U;({unsigned int _tmp92C=e->loc;struct _dyneither_ptr _tmp92B=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp489=({struct Cyc_String_pa_PrintArg_struct _tmp71E;_tmp71E.tag=0U,({
struct _dyneither_ptr _tmp929=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp71E.f1=_tmp929;});_tmp71E;});void*_tmp487[1U];_tmp487[0]=& _tmp489;({struct _dyneither_ptr _tmp92A=({const char*_tmp488="Toc did not examine an inserted check: %s";_tag_dyneither(_tmp488,sizeof(char),42U);});Cyc_aprintf(_tmp92A,_tag_dyneither(_tmp487,sizeof(void*),1U));});});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp92C,_tmp92B,_tag_dyneither(_tmp486,sizeof(void*),0U));});});};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2882 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2884
return({struct _tuple35*_tmp48A=_region_malloc(r,sizeof(*_tmp48A));_tmp48A->f1=0,({struct _dyneither_ptr*_tmp92E=Cyc_Toc_fresh_label();_tmp48A->f2=_tmp92E;}),({struct _dyneither_ptr*_tmp92D=Cyc_Toc_fresh_label();_tmp48A->f3=_tmp92D;}),_tmp48A->f4=sc;_tmp48A;});}
# 2889
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp48B=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 2895
if((int)(((_tmp48B->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp48E=_tmp48B->orig_pat;if((_tmp48E.pattern).tag != 1)_throw_match();(_tmp48E.pattern).val;})->topt);
void*_tmp48C=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp48D=_tmp48C;switch(*((int*)_tmp48D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D)->f1)){case 0U: _LL1: _LL2:
# 2900
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmp92F=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp92F,v);});goto _LL0;}_LL0:;}{
# 2906
void*_tmp48F=_tmp48B->access;void*_tmp490=_tmp48F;struct Cyc_Absyn_Datatypedecl*_tmp49B;struct Cyc_Absyn_Datatypefield*_tmp49A;unsigned int _tmp499;int _tmp498;struct _dyneither_ptr*_tmp497;unsigned int _tmp496;switch(*((int*)_tmp490)){case 0U: _LLA: _LLB:
# 2911
 goto _LL9;case 1U: _LLC: _LLD:
# 2916
 if(ps->tl != 0){
void*_tmp491=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp492=_tmp491;struct Cyc_Absyn_Datatypedecl*_tmp495;struct Cyc_Absyn_Datatypefield*_tmp494;unsigned int _tmp493;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp492)->tag == 3U){_LL15: _tmp495=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp492)->f1;_tmp494=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp492)->f2;_tmp493=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp492)->f3;_LL16:
# 2919
 ps=ps->tl;
v=({void*_tmp931=({void*_tmp930=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp494->name,_tmp495->name));Cyc_Absyn_cstar_type(_tmp930,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp931,v);});
v=({struct Cyc_Absyn_Exp*_tmp932=v;Cyc_Absyn_aggrarrow_exp(_tmp932,Cyc_Absyn_fieldname((int)(_tmp493 + (unsigned int)1)),0U);});
continue;}else{_LL17: _LL18:
# 2924
 goto _LL14;}_LL14:;}
# 2927
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 2U: _LLE: _tmp496=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp490)->f1;_LLF:
 v=({struct Cyc_Absyn_Exp*_tmp933=v;Cyc_Toc_member_exp(_tmp933,Cyc_Absyn_fieldname((int)(_tmp496 + (unsigned int)1)),0U);});goto _LL9;case 4U: _LL10: _tmp498=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp490)->f1;_tmp497=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp490)->f2;_LL11:
# 2931
 v=Cyc_Toc_member_exp(v,_tmp497,0U);
if(_tmp498)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;default: _LL12: _tmp49B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp490)->f1;_tmp49A=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp490)->f2;_tmp499=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp490)->f3;_LL13:
# 2936
 v=({struct Cyc_Absyn_Exp*_tmp934=v;Cyc_Toc_member_exp(_tmp934,Cyc_Absyn_fieldname((int)(_tmp499 + (unsigned int)1)),0U);});
goto _LL9;}_LL9:;};}
# 2940
return v;}
# 2945
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp49C=t;struct Cyc_Absyn_Datatypedecl*_tmp4B5;struct Cyc_Absyn_Datatypefield*_tmp4B4;struct _dyneither_ptr*_tmp4B3;int _tmp4B2;int _tmp4B1;struct Cyc_Absyn_Datatypedecl*_tmp4B0;struct Cyc_Absyn_Datatypefield*_tmp4AF;unsigned int _tmp4AE;struct _dyneither_ptr _tmp4AD;int _tmp4AC;void*_tmp4AB;struct Cyc_Absyn_Enumfield*_tmp4AA;struct Cyc_Absyn_Enumdecl*_tmp4A9;struct Cyc_Absyn_Enumfield*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A7;switch(*((int*)_tmp49C)){case 0U: _LL1: _tmp4A7=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp49C)->f1;_LL2:
# 2949
 if(_tmp4A7 == 0)return v;else{return _tmp4A7;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmp935=v;Cyc_Absyn_eq_exp(_tmp935,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmp936=v;Cyc_Absyn_neq_exp(_tmp936,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4A9=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp49C)->f1;_tmp4A8=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp49C)->f2;_LL8:
# 2953
 return({struct Cyc_Absyn_Exp*_tmp937=v;Cyc_Absyn_eq_exp(_tmp937,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->tag=32U,_tmp49D->f1=_tmp4A9,_tmp49D->f2=_tmp4A8;_tmp49D;}),0U),0U);});case 4U: _LL9: _tmp4AB=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp49C)->f1;_tmp4AA=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp49C)->f2;_LLA:
# 2955
 return({struct Cyc_Absyn_Exp*_tmp938=v;Cyc_Absyn_eq_exp(_tmp938,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E->tag=33U,_tmp49E->f1=_tmp4AB,_tmp49E->f2=_tmp4AA;_tmp49E;}),0U),0U);});case 5U: _LLB: _tmp4AD=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp49C)->f1;_tmp4AC=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp49C)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmp939=v;Cyc_Absyn_eq_exp(_tmp939,Cyc_Absyn_float_exp(_tmp4AD,_tmp4AC,0U),0U);});case 6U: _LLD: _tmp4AE=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp49C)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmp93A=v;Cyc_Absyn_eq_exp(_tmp93A,Cyc_Absyn_signed_int_exp((int)_tmp4AE,0U),0U);});case 7U: _LLF: _tmp4B1=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp49C)->f1;_tmp4B0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp49C)->f2;_tmp4AF=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp49C)->f3;_LL10:
# 2961
 LOOP1: {
void*_tmp49F=v->r;void*_tmp4A0=_tmp49F;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;switch(*((int*)_tmp4A0)){case 14U: _LL16: _tmp4A1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4A0)->f2;_LL17:
 v=_tmp4A1;goto LOOP1;case 20U: _LL18: _tmp4A2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4A0)->f1;_LL19:
 v=_tmp4A2;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2968
v=({void*_tmp93C=({void*_tmp93B=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4AF->name,_tmp4B0->name));Cyc_Absyn_cstar_type(_tmp93B,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp93C,v);});
return({struct Cyc_Absyn_Exp*_tmp93D=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp93D,Cyc_Absyn_uint_exp((unsigned int)_tmp4B1,0U),0U);});case 8U: _LL11: _tmp4B3=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp49C)->f1;_tmp4B2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp49C)->f2;_LL12:
# 2973
 v=({struct Cyc_Absyn_Exp*_tmp93E=Cyc_Toc_member_exp(v,_tmp4B3,0U);Cyc_Toc_member_exp(_tmp93E,Cyc_Toc_tag_sp,0U);});
return({struct Cyc_Absyn_Exp*_tmp93F=v;Cyc_Absyn_eq_exp(_tmp93F,Cyc_Absyn_signed_int_exp(_tmp4B2,0U),0U);});default: _LL13: _tmp4B5=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp49C)->f1;_tmp4B4=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp49C)->f2;_LL14:
# 2977
 LOOP2: {
void*_tmp4A3=v->r;void*_tmp4A4=_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;switch(*((int*)_tmp4A4)){case 14U: _LL1D: _tmp4A5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4A4)->f2;_LL1E:
 v=_tmp4A5;goto LOOP2;case 20U: _LL1F: _tmp4A6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4A4)->f1;_LL20:
 v=_tmp4A6;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 2984
v=({void*_tmp941=({void*_tmp940=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4B4->name,_tmp4B5->name));Cyc_Absyn_cstar_type(_tmp940,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp941,v);});
return({struct Cyc_Absyn_Exp*_tmp942=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp942,Cyc_Absyn_var_exp(_tmp4B4->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 2998
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3005
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3007
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple32 _tmp4B6=*((struct _tuple32*)ss->hd);struct _tuple32 _tmp4B7=_tmp4B6;void*_tmp4BD;_LL1: _tmp4BD=_tmp4B7.f1;_LL2:;{
void*_tmp4B8=_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BC;struct _dyneither_ptr*_tmp4BB;switch(*((int*)_tmp4B8)){case 3U: _LL4: _LL5:
# 3013
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3016
 continue;case 8U: _LLA: _tmp4BB=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4B8)->f1;_LLB:
# 3018
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->tag=3U,_tmp4B9->f1=_tmp4BB;_tmp4B9;});
continue;case 7U: _LLC: _LLD:
# 3022
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp4BC=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4B8)->f1;if(_tmp4BC != 0){_LLF:
# 3026
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA->tag=2U,_tmp4BA->f1=_tmp4BC;_tmp4BA;});
return({struct _tuple36 _tmp71F;_tmp71F.f1=0,_tmp71F.f2=k;_tmp71F;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp720;_tmp720.f1=0,_tmp720.f2=k;_tmp720;});}_LL3:;};}
# 3035
return({struct _tuple36 _tmp721;_tmp721.f1=c,_tmp721.f2=k;_tmp721;});}
# 3040
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4BE=p;int _tmp4C9;int _tmp4C8;unsigned int _tmp4C7;void*_tmp4C6;struct Cyc_Absyn_Enumfield*_tmp4C5;struct Cyc_Absyn_Enumdecl*_tmp4C4;struct Cyc_Absyn_Enumfield*_tmp4C3;switch(*((int*)_tmp4BE)){case 3U: _LL1: _tmp4C4=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4BE)->f1;_tmp4C3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4BE)->f2;_LL2:
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF->tag=32U,_tmp4BF->f1=_tmp4C4,_tmp4BF->f2=_tmp4C3;_tmp4BF;}),0U);goto _LL0;case 4U: _LL3: _tmp4C6=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4BE)->f1;_tmp4C5=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4BE)->f2;_LL4:
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->tag=33U,_tmp4C0->f1=_tmp4C6,_tmp4C0->f2=_tmp4C5;_tmp4C0;}),0U);goto _LL0;case 6U: _LL5: _tmp4C7=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4BE)->f1;_LL6:
 _tmp4C8=(int)_tmp4C7;goto _LL8;case 7U: _LL7: _tmp4C8=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4BE)->f1;_LL8:
 _tmp4C9=_tmp4C8;goto _LLA;case 8U: _LL9: _tmp4C9=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4BE)->f2;_LLA:
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp4C9,0U);goto _LL0;default: _LLB: _LLC:
({void*_tmp4C1=0U;({struct _dyneither_ptr _tmp943=({const char*_tmp4C2="compile_pat_test_as_case!";_tag_dyneither(_tmp4C2,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp943,_tag_dyneither(_tmp4C1,sizeof(void*),0U));});});}_LL0:;}
# 3050
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->tag=17U,_tmp4CA->f1=e;_tmp4CA;}),0U);}
# 3054
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3063
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3066
void*t=(void*)_check_null(p->topt);
void*_tmp4CB=p->r;void*_tmp4CC=_tmp4CB;union Cyc_Absyn_AggrInfo _tmp4FD;struct Cyc_List_List*_tmp4FC;struct Cyc_List_List*_tmp4FB;struct Cyc_List_List*_tmp4FA;struct Cyc_Absyn_Pat*_tmp4F9;struct Cyc_Absyn_Datatypedecl*_tmp4F8;struct Cyc_Absyn_Datatypefield*_tmp4F7;struct Cyc_List_List*_tmp4F6;struct Cyc_Absyn_Vardecl*_tmp4F5;struct Cyc_Absyn_Pat*_tmp4F4;struct Cyc_Absyn_Vardecl*_tmp4F3;struct Cyc_Absyn_Vardecl*_tmp4F2;struct Cyc_Absyn_Pat*_tmp4F1;struct Cyc_Absyn_Vardecl*_tmp4F0;switch(*((int*)_tmp4CC)){case 2U: _LL1: _tmp4F0=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4CC)->f2;_LL2: {
# 3069
struct Cyc_Absyn_Pat*_tmp4CD=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp4CD->topt=p->topt;
_tmp4F2=_tmp4F0;_tmp4F1=_tmp4CD;goto _LL4;}case 1U: _LL3: _tmp4F2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1;_tmp4F1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4CC)->f2;_LL4:
# 3074
({struct Cyc_List_List*_tmp944=({struct Cyc_List_List*_tmp4CE=_region_malloc(rgn,sizeof(*_tmp4CE));_tmp4CE->hd=_tmp4F2,_tmp4CE->tl=*decls;_tmp4CE;});*decls=_tmp944;});{
struct Cyc_Absyn_Stmt*_tmp4CF=({struct Cyc_Absyn_Exp*_tmp945=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->tag=4U,_tmp4D0->f1=_tmp4F2;_tmp4D0;}),0U);Cyc_Absyn_assign_stmt(_tmp945,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmp946=_tmp4CF;Cyc_Toc_seq_stmt_opt(_tmp946,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp4F1));});};case 4U: _LL5: _tmp4F3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4CC)->f2;_LL6:
# 3079
({struct Cyc_List_List*_tmp947=({struct Cyc_List_List*_tmp4D1=_region_malloc(rgn,sizeof(*_tmp4D1));_tmp4D1->hd=_tmp4F3,_tmp4D1->tl=*decls;_tmp4D1;});*decls=_tmp947;});
return({struct Cyc_Absyn_Exp*_tmp948=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2->tag=4U,_tmp4D2->f1=_tmp4F3;_tmp4D2;}),0U);Cyc_Absyn_assign_stmt(_tmp948,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp4F5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1;_tmp4F4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4CC)->f2;_LLA:
# 3084
({struct Cyc_List_List*_tmp949=({struct Cyc_List_List*_tmp4D3=_region_malloc(rgn,sizeof(*_tmp4D3));_tmp4D3->hd=_tmp4F5,_tmp4D3->tl=*decls;_tmp4D3;});*decls=_tmp949;});
({void*_tmp94A=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp4F5->type=_tmp94A;});{
# 3087
struct Cyc_Absyn_Stmt*_tmp4D4=({struct Cyc_Absyn_Exp*_tmp94D=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5->tag=4U,_tmp4D5->f1=_tmp4F5;_tmp4D5;}),0U);Cyc_Absyn_assign_stmt(_tmp94D,({
void*_tmp94C=({void*_tmp94B=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp94B,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp94C,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmp94E=_tmp4D4;Cyc_Toc_seq_stmt_opt(_tmp94E,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp4F4));});};case 9U: _LLB: _LLC:
# 3092
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1)->r)->tag == 8U){_LL17: _tmp4F8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1)->r)->f1;_tmp4F7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1)->r)->f2;_tmp4F6=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1)->r)->f3;_LL18:
# 3101
 if(_tmp4F6 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp4F7->name,_tmp4F8->name);
void*_tmp4D6=({void*_tmp94F=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmp94F,Cyc_Toc_mt_tq);});
path=Cyc_Toc_cast_it(_tmp4D6,path);{
int cnt=1;
struct Cyc_List_List*_tmp4D7=_tmp4F7->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp4F6 != 0;(_tmp4F6=_tmp4F6->tl,_tmp4D7=((struct Cyc_List_List*)_check_null(_tmp4D7))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4D8=(struct Cyc_Absyn_Pat*)_tmp4F6->hd;
if(_tmp4D8->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp4D9=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4D7))->hd)).f2;
void*_tmp4DA=(void*)_check_null(_tmp4D8->topt);
void*_tmp4DB=Cyc_Toc_typ_to_c_array(_tmp4DA);
struct Cyc_Absyn_Exp*_tmp4DC=({struct Cyc_Absyn_Exp*_tmp950=path;Cyc_Absyn_aggrarrow_exp(_tmp950,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp4D9)))
_tmp4DC=Cyc_Toc_cast_it(_tmp4DB,_tmp4DC);
s=({struct Cyc_Absyn_Stmt*_tmp951=s;Cyc_Toc_seq_stmt_opt(_tmp951,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4DC,_tmp4D8));});};}
# 3119
return s;};};}else{_LL21: _tmp4F9=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1;_LL22:
# 3164
 return({struct _RegionHandle*_tmp955=rgn;struct Cyc_Toc_Env**_tmp954=nv;struct Cyc_List_List**_tmp953=decls;struct Cyc_Absyn_Exp*_tmp952=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmp955,_tmp954,_tmp953,_tmp952,_tmp4F9);});}case 8U: _LL19: _tmp4FA=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4CC)->f3;_LL1A:
# 3121
 _tmp4FB=_tmp4FA;goto _LL1C;case 5U: _LL1B: _tmp4FB=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1;_LL1C: {
# 3123
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp4FB != 0;(_tmp4FB=_tmp4FB->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4DD=(struct Cyc_Absyn_Pat*)_tmp4FB->hd;
if(_tmp4DD->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp4DE=(void*)_check_null(_tmp4DD->topt);
struct _dyneither_ptr*_tmp4DF=Cyc_Absyn_fieldname(cnt);
s=({struct Cyc_Absyn_Stmt*_tmp95A=s;Cyc_Toc_seq_stmt_opt(_tmp95A,({struct _RegionHandle*_tmp959=rgn;struct Cyc_Toc_Env**_tmp958=nv;struct Cyc_List_List**_tmp957=decls;struct Cyc_Absyn_Exp*_tmp956=Cyc_Toc_member_exp(path,_tmp4DF,0U);Cyc_Toc_extract_pattern_vars(_tmp959,_tmp958,_tmp957,_tmp956,_tmp4DD);}));});};}
# 3133
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1 == 0){_LL1D: _LL1E:
({void*_tmp4E0=0U;({struct _dyneither_ptr _tmp95B=({const char*_tmp4E1="unresolved aggregate pattern!";_tag_dyneither(_tmp4E1,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp95B,_tag_dyneither(_tmp4E0,sizeof(void*),0U));});});}else{_LL1F: _tmp4FD=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4CC)->f1;_tmp4FC=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4CC)->f3;_LL20: {
# 3137
struct Cyc_Absyn_Aggrdecl*_tmp4E2=Cyc_Absyn_get_known_aggrdecl(_tmp4FD);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp4FC != 0;_tmp4FC=_tmp4FC->tl){
struct _tuple37*_tmp4E3=(struct _tuple37*)_tmp4FC->hd;
struct Cyc_Absyn_Pat*_tmp4E4=(*_tmp4E3).f2;
if(_tmp4E4->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp4E5=Cyc_Absyn_designatorlist_to_fieldname((*_tmp4E3).f1);
void*_tmp4E6=(void*)_check_null(_tmp4E4->topt);
void*_tmp4E7=Cyc_Toc_typ_to_c_array(_tmp4E6);
struct Cyc_Absyn_Exp*_tmp4E8=Cyc_Toc_member_exp(path,_tmp4E5,0U);
# 3149
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4E2->impl))->tagged)_tmp4E8=Cyc_Toc_member_exp(_tmp4E8,Cyc_Toc_val_sp,0U);{
void*_tmp4E9=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4E2->impl))->fields,_tmp4E5)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp4E9))
_tmp4E8=Cyc_Toc_cast_it(_tmp4E7,_tmp4E8);else{
if(!Cyc_Tcutil_is_array_type(_tmp4E9) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4E9)))
# 3156
_tmp4E8=Cyc_Absyn_deref_exp(({void*_tmp95C=Cyc_Absyn_cstar_type(_tmp4E7,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp95C,
Cyc_Absyn_address_exp(_tmp4E8,0U));}),0U);}
# 3159
s=({struct Cyc_Absyn_Stmt*_tmp95D=s;Cyc_Toc_seq_stmt_opt(_tmp95D,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4E8,_tmp4E4));});};};}
# 3161
return s;}}case 15U: _LL23: _LL24:
# 3166
({void*_tmp4EA=0U;({struct _dyneither_ptr _tmp95E=({const char*_tmp4EB="unknownid pat";_tag_dyneither(_tmp4EB,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp95E,_tag_dyneither(_tmp4EA,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp4EC=0U;({struct _dyneither_ptr _tmp95F=({const char*_tmp4ED="unknowncall pat";_tag_dyneither(_tmp4ED,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp95F,_tag_dyneither(_tmp4EC,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp4EE=0U;({struct _dyneither_ptr _tmp960=({const char*_tmp4EF="exp pat";_tag_dyneither(_tmp4EF,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp960,_tag_dyneither(_tmp4EE,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3177
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3185
void*_tmp4FE=dopt;struct Cyc_List_List*_tmp53C;struct Cyc_List_List*_tmp53B;void*_tmp53A;struct Cyc_Tcpat_Rhs*_tmp539;if(_tmp4FE == 0){_LL1: _LL2:
# 3187
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp4FE)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp539=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp4FE)->f1;_LL6:
# 3192
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp4FF=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp500=_tmp4FF;int*_tmp507;struct _dyneither_ptr*_tmp506;struct _dyneither_ptr*_tmp505;struct Cyc_Absyn_Switch_clause*_tmp504;_LLA: _tmp507=(int*)& _tmp500->f1;_tmp506=_tmp500->f2;_tmp505=_tmp500->f3;_tmp504=_tmp500->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp501=_tmp504->body;
if(_tmp501 == _tmp539->rhs){
# 3197
if(*_tmp507)
return Cyc_Absyn_goto_stmt(_tmp506,0U);
*_tmp507=1;{
# 3201
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp963=rgn;struct Cyc_List_List**_tmp962=decls;struct Cyc_Absyn_Exp*_tmp961=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp963,& nv,_tmp962,_tmp961,_tmp504->pattern);});
# 3204
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp505,_tmp504->body,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(_tmp506,res,0U);
({struct Cyc_List_List*_tmp965=({struct Cyc_List_List*_tmp503=_region_malloc(rgn,sizeof(*_tmp503));({struct _tuple38*_tmp964=({struct _tuple38*_tmp502=_region_malloc(rgn,sizeof(*_tmp502));_tmp502->f1=nv,_tmp502->f2=_tmp505,_tmp502->f3=_tmp501;_tmp502;});_tmp503->hd=_tmp964;}),_tmp503->tl=*bodies;_tmp503;});*bodies=_tmp965;});
return res;};}};}
# 3212
({void*_tmp508=0U;({struct _dyneither_ptr _tmp966=({const char*_tmp509="couldn't find rhs!";_tag_dyneither(_tmp509,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp966,_tag_dyneither(_tmp508,sizeof(void*),0U));});});default: _LL7: _tmp53C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp4FE)->f1;_tmp53B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp4FE)->f2;_tmp53A=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp4FE)->f3;_LL8: {
# 3215
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp53A,lscs,v);
# 3217
struct Cyc_Absyn_Exp*_tmp50A=({struct Cyc_List_List*_tmp967=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp53C);Cyc_Toc_compile_path(_tmp967,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp50B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp53B);
# 3220
struct _tuple36 _tmp50C=Cyc_Toc_admits_switch(_tmp50B);struct _tuple36 _tmp50D=_tmp50C;int _tmp538;void*_tmp537;_LLD: _tmp538=_tmp50D.f1;_tmp537=_tmp50D.f2;_LLE:;
if(_tmp538 > 1){
# 3224
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp51E=_cycalloc(sizeof(*_tmp51E));
({struct Cyc_Absyn_Switch_clause*_tmp969=({struct Cyc_Absyn_Switch_clause*_tmp51D=_cycalloc(sizeof(*_tmp51D));({struct Cyc_Absyn_Pat*_tmp968=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp51D->pattern=_tmp968;}),_tmp51D->pat_vars=0,_tmp51D->where_clause=0,_tmp51D->body=res,_tmp51D->loc=0U;_tmp51D;});_tmp51E->hd=_tmp969;}),_tmp51E->tl=0;_tmp51E;});
# 3227
for(0;_tmp50B != 0;_tmp50B=_tmp50B->tl){
struct _tuple32 _tmp50E=*((struct _tuple32*)_tmp50B->hd);struct _tuple32 _tmp50F=_tmp50E;void*_tmp515;void*_tmp514;_LL10: _tmp515=_tmp50F.f1;_tmp514=_tmp50F.f2;_LL11:;{
# 3230
struct Cyc_Absyn_Pat*_tmp510=Cyc_Toc_compile_pat_test_as_case(_tmp515);
# 3232
struct Cyc_Absyn_Stmt*_tmp511=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp514,lscs,v);
# 3234
new_lscs=({struct Cyc_List_List*_tmp513=_cycalloc(sizeof(*_tmp513));({struct Cyc_Absyn_Switch_clause*_tmp96A=({struct Cyc_Absyn_Switch_clause*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->pattern=_tmp510,_tmp512->pat_vars=0,_tmp512->where_clause=0,_tmp512->body=_tmp511,_tmp512->loc=0U;_tmp512;});_tmp513->hd=_tmp96A;}),_tmp513->tl=new_lscs;_tmp513;});};}
# 3236
{void*_tmp516=_tmp537;struct _dyneither_ptr*_tmp51B;switch(*((int*)_tmp516)){case 1U: _LL13: _LL14:
# 3239
 LOOP1: {
void*_tmp517=_tmp50A->r;void*_tmp518=_tmp517;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;switch(*((int*)_tmp518)){case 14U: _LL1C: _tmp519=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp518)->f2;_LL1D:
 _tmp50A=_tmp519;goto LOOP1;case 20U: _LL1E: _tmp51A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp518)->f1;_LL1F:
 _tmp50A=_tmp51A;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3245
_tmp50A=Cyc_Absyn_deref_exp(({void*_tmp96B=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp96B,_tmp50A);}),0U);goto _LL12;case 3U: _LL15: _tmp51B=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp516)->f1;_LL16:
# 3248
 _tmp50A=({struct Cyc_Absyn_Exp*_tmp96C=Cyc_Toc_member_exp(_tmp50A,_tmp51B,0U);Cyc_Toc_member_exp(_tmp96C,Cyc_Toc_tag_sp,0U);});goto _LL12;case 2U: _LL17: _LL18:
# 3250
 goto _LL1A;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3253
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->tag=10U,_tmp51C->f1=_tmp50A,_tmp51C->f2=new_lscs,_tmp51C->f3=0;_tmp51C;}),0U);}else{
# 3257
void*_tmp51F=_tmp537;struct Cyc_Absyn_Exp*_tmp536;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp51F)->tag == 2U){_LL23: _tmp536=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp51F)->f1;_LL24: {
# 3259
struct Cyc_List_List*_tmp520=_tmp50B;void*_tmp52F;struct Cyc_Tcpat_Rhs*_tmp52E;if(_tmp520 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)((struct Cyc_List_List*)_tmp520)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp520)->tl == 0){_LL28: _tmp52F=((struct _tuple32*)_tmp520->hd)->f1;_tmp52E=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)_tmp520->hd)->f2)->f1;_LL29:
# 3265
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp521=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp522=_tmp521;int*_tmp529;struct _dyneither_ptr*_tmp528;struct _dyneither_ptr*_tmp527;struct Cyc_Absyn_Switch_clause*_tmp526;_LL2D: _tmp529=(int*)& _tmp522->f1;_tmp528=_tmp522->f2;_tmp527=_tmp522->f3;_tmp526=_tmp522->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp523=_tmp526->body;
if(_tmp523 == _tmp52E->rhs){
# 3270
if(*_tmp529)
return Cyc_Absyn_goto_stmt(_tmp528,0U);
*_tmp529=1;{
# 3274
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp96F=rgn;struct Cyc_List_List**_tmp96E=decls;struct Cyc_Absyn_Exp*_tmp96D=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp96F,& nv,_tmp96E,_tmp96D,_tmp526->pattern);});
# 3277
Cyc_Toc_exp_to_c(nv,_tmp536);{
# 3280
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp527,_tmp526->body,0U);
r=Cyc_Absyn_ifthenelse_stmt(_tmp536,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(_tmp528,r,0U);
({struct Cyc_List_List*_tmp971=({struct Cyc_List_List*_tmp525=_region_malloc(rgn,sizeof(*_tmp525));({struct _tuple38*_tmp970=({struct _tuple38*_tmp524=_region_malloc(rgn,sizeof(*_tmp524));_tmp524->f1=nv,_tmp524->f2=_tmp527,_tmp524->f3=_tmp523;_tmp524;});_tmp525->hd=_tmp970;}),_tmp525->tl=*bodies;_tmp525;});*bodies=_tmp971;});
return r;};};}};}
# 3289
({void*_tmp52A=0U;({struct _dyneither_ptr _tmp972=({const char*_tmp52B="couldn't find rhs!";_tag_dyneither(_tmp52B,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp972,_tag_dyneither(_tmp52A,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp52C=0U;({struct _dyneither_ptr _tmp973=({const char*_tmp52D="bad where clause in match compiler";_tag_dyneither(_tmp52D,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp973,_tag_dyneither(_tmp52C,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3294
 for(0;_tmp50B != 0;_tmp50B=_tmp50B->tl){
struct _tuple32 _tmp530=*((struct _tuple32*)_tmp50B->hd);struct _tuple32 _tmp531=_tmp530;void*_tmp535;void*_tmp534;_LL30: _tmp535=_tmp531.f1;_tmp534=_tmp531.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp532=Cyc_Toc_compile_pat_test(_tmp50A,_tmp535);
struct Cyc_Absyn_Stmt*_tmp533=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp534,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp532,_tmp533,res,0U);};}}_LL22:;}
# 3302
return res;}}}_LL0:;}
# 3312
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3314
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp53D=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp53E=_tmp53D;struct Cyc_Toc_Env**_tmp540;struct Cyc_Absyn_Stmt*_tmp53F;_LL1: _tmp540=(struct Cyc_Toc_Env**)& _tmp53E->f1;_tmp53F=_tmp53E->f3;_LL2:;
if(_tmp53F == s)return _tmp540;}
# 3320
return 0;}
# 3324
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3327
void*_tmp541=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3330
struct _tuple29 _tmp542=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp543=_tmp542;struct _tuple1*_tmp554;struct Cyc_Absyn_Exp*_tmp553;_LL1: _tmp554=_tmp543.f1;_tmp553=_tmp543.f2;_LL2:;{
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp544=_new_region("rgn");struct _RegionHandle*rgn=& _tmp544;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp545=Cyc_Toc_share_env(rgn,nv);
# 3336
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3341
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp545,& mydecls,& mybodies,dopt,lscs,_tmp554);
# 3349
{struct Cyc_List_List*_tmp546=lscs;for(0;_tmp546 != 0;_tmp546=_tmp546->tl){
struct _tuple35*_tmp547=(struct _tuple35*)_tmp546->hd;struct _tuple35*_tmp548=_tmp547;struct _dyneither_ptr*_tmp550;struct Cyc_Absyn_Switch_clause*_tmp54F;_LL4: _tmp550=_tmp548->f3;_tmp54F=_tmp548->f4;_LL5:;{
struct Cyc_Absyn_Stmt*s=_tmp54F->body;
# 3353
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3356
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp549=*envp;
# 3359
if(_tmp546->tl != 0){
struct _tuple35*_tmp54A=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp546->tl))->hd;struct _tuple35*_tmp54B=_tmp54A;struct _dyneither_ptr*_tmp54E;struct Cyc_Absyn_Switch_clause*_tmp54D;_LL7: _tmp54E=_tmp54B->f3;_tmp54D=_tmp54B->f4;_LL8:;{
# 3363
struct Cyc_Toc_Env**_tmp54C=Cyc_Toc_find_case_env(mybodies,_tmp54D->body);
# 3368
if(_tmp54C == 0)
({struct Cyc_Toc_Env*_tmp974=Cyc_Toc_last_switchclause_env(rgn,_tmp549,end_l);Cyc_Toc_stmt_to_c(_tmp974,s);});else{
# 3372
struct Cyc_List_List*vs=0;
if(_tmp54D->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp54D->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 3377
({struct Cyc_Toc_Env*_tmp975=Cyc_Toc_non_last_switchclause_env(rgn,_tmp549,end_l,_tmp54E,vs);Cyc_Toc_stmt_to_c(_tmp975,s);});}};}else{
# 3381
({struct Cyc_Toc_Env*_tmp976=Cyc_Toc_last_switchclause_env(rgn,_tmp549,end_l);Cyc_Toc_stmt_to_c(_tmp976,s);});}};};}}{
# 3385
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmp978=test_tree;Cyc_Absyn_seq_stmt(_tmp978,({struct _dyneither_ptr*_tmp977=end_l;Cyc_Absyn_label_stmt(_tmp977,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3387
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp551=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmp979=Cyc_Toc_temp_var();_tmp551->name=_tmp979;});
({void*_tmp97A=Cyc_Toc_typ_to_c_array(_tmp551->type);_tmp551->type=_tmp97A;});
res=({struct Cyc_Absyn_Decl*_tmp97B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->tag=0U,_tmp552->f1=_tmp551;_tmp552;}),0U);Cyc_Absyn_decl_stmt(_tmp97B,res,0U);});}
# 3394
({void*_tmp97F=({struct _tuple1*_tmp97E=_tmp554;void*_tmp97D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmp97C=e;Cyc_Absyn_declare_stmt(_tmp97E,_tmp97D,_tmp97C,res,0U);})->r;whole_s->r=_tmp97F;});
_npop_handler(0U);return;};}
# 3333
;_pop_region(rgn);};};}
# 3400
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3402
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3407
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3409
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp555[1U];({struct Cyc_Absyn_Exp*_tmp980=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp555[0]=_tmp980;});({struct Cyc_Absyn_Exp*_tmp981=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmp981,_tag_dyneither(_tmp555,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3412
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmp983=({struct Cyc_Absyn_Stmt*_tmp982=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmp982,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmp983;});}
# 3417
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3419
while(1){
void*_tmp556=s->r;void*_tmp557=_tmp556;struct Cyc_Absyn_Stmt*_tmp5BE;struct Cyc_List_List*_tmp5BD;void*_tmp5BC;struct Cyc_Absyn_Stmt*_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BA;struct _dyneither_ptr*_tmp5B9;struct Cyc_Absyn_Stmt*_tmp5B8;struct Cyc_Absyn_Decl*_tmp5B7;struct Cyc_Absyn_Stmt*_tmp5B6;struct Cyc_List_List*_tmp5B5;struct Cyc_Absyn_Switch_clause**_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B3;struct Cyc_List_List*_tmp5B2;void*_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Stmt*_tmp5AD;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Stmt*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_Stmt*_tmp5A9;struct Cyc_Absyn_Stmt*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A7;struct Cyc_Absyn_Stmt*_tmp5A6;struct Cyc_Absyn_Stmt*_tmp5A5;struct Cyc_Absyn_Exp*_tmp5A4;switch(*((int*)_tmp557)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp5A4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_LL4:
 Cyc_Toc_exp_to_c(nv,_tmp5A4);return;case 2U: _LL5: _tmp5A6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_tmp5A5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp557)->f2;_LL6:
# 3424
 Cyc_Toc_stmt_to_c(nv,_tmp5A6);
s=_tmp5A5;
continue;case 3U: _LL7: _tmp5A7=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_LL8:
# 3428
 if(_tmp5A7 != 0){
void*t=Cyc_Toc_typ_to_c((void*)_check_null(_tmp5A7->topt));
Cyc_Toc_exp_to_c(nv,_tmp5A7);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
# 3434
struct _tuple1*_tmp558=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp559=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp558,0U),0U);
Cyc_Toc_do_npop_before(npop,_tmp559);
({void*_tmp984=(Cyc_Absyn_declare_stmt(_tmp558,t,_tmp5A7,_tmp559,0U))->r;s->r=_tmp984;});}};}else{
# 3440
({int _tmp985=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp985,s);});}
return;case 4U: _LL9: _tmp5AA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_tmp5A9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp557)->f2;_tmp5A8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp557)->f3;_LLA:
# 3443
 Cyc_Toc_exp_to_c(nv,_tmp5AA);
Cyc_Toc_stmt_to_c(nv,_tmp5A9);
s=_tmp5A8;
continue;case 5U: _LLB: _tmp5AC=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp557)->f1).f1;_tmp5AB=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp557)->f2;_LLC:
# 3448
 Cyc_Toc_exp_to_c(nv,_tmp5AC);{
struct _RegionHandle _tmp55A=_new_region("temp");struct _RegionHandle*temp=& _tmp55A;_push_region(temp);
({struct Cyc_Toc_Env*_tmp986=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp986,_tmp5AB);});
# 3452
_npop_handler(0U);return;
# 3449
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3454
struct Cyc_Toc_Env*_tmp55B=nv;struct _dyneither_ptr**_tmp55C;_LL22: _tmp55C=_tmp55B->break_lab;_LL23:;
if(_tmp55C != 0)
({void*_tmp987=Cyc_Toc_goto_stmt_r(*_tmp55C);s->r=_tmp987;});
# 3458
({int _tmp988=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp988,s);});
return;}case 7U: _LLF: _LL10: {
# 3461
struct Cyc_Toc_Env*_tmp55D=nv;struct _dyneither_ptr**_tmp55E;_LL25: _tmp55E=_tmp55D->continue_lab;_LL26:;
if(_tmp55E != 0)
({void*_tmp989=Cyc_Toc_goto_stmt_r(*_tmp55E);s->r=_tmp989;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3467
({int _tmp98A=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp98A,s);});
return;case 9U: _LL13: _tmp5B0=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_tmp5AF=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp557)->f2).f1;_tmp5AE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp557)->f3).f1;_tmp5AD=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp557)->f4;_LL14:
# 3471
 Cyc_Toc_exp_to_c(nv,_tmp5B0);Cyc_Toc_exp_to_c(nv,_tmp5AF);Cyc_Toc_exp_to_c(nv,_tmp5AE);{
struct _RegionHandle _tmp55F=_new_region("temp");struct _RegionHandle*temp=& _tmp55F;_push_region(temp);
({struct Cyc_Toc_Env*_tmp98B=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp98B,_tmp5AD);});
# 3475
_npop_handler(0U);return;
# 3472
;_pop_region(temp);};case 10U: _LL15: _tmp5B3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_tmp5B2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp557)->f2;_tmp5B1=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp557)->f3;_LL16:
# 3477
 Cyc_Toc_xlate_switch(nv,s,_tmp5B3,_tmp5B2,_tmp5B1);
return;case 11U: _LL17: _tmp5B5=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_tmp5B4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp557)->f2;_LL18: {
# 3480
struct Cyc_Toc_Env*_tmp560=nv;struct Cyc_Toc_FallthruInfo*_tmp56A;_LL28: _tmp56A=_tmp560->fallthru_info;_LL29:;
if(_tmp56A == 0)
({void*_tmp561=0U;({struct _dyneither_ptr _tmp98C=({const char*_tmp562="fallthru in unexpected place";_tag_dyneither(_tmp562,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98C,_tag_dyneither(_tmp561,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp563=*_tmp56A;struct Cyc_Toc_FallthruInfo _tmp564=_tmp563;struct _dyneither_ptr*_tmp569;struct Cyc_List_List*_tmp568;_LL2B: _tmp569=_tmp564.label;_tmp568=_tmp564.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp569,0U);
# 3486
({int _tmp98D=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp98D,s2);});{
struct Cyc_List_List*_tmp565=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp568);
struct Cyc_List_List*_tmp566=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5B5);
for(0;_tmp565 != 0;(_tmp565=_tmp565->tl,_tmp566=_tmp566->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp566))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmp98F=({struct Cyc_Absyn_Exp*_tmp98E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567->tag=5U,_tmp567->f1=(struct Cyc_Absyn_Vardecl*)_tmp565->hd;_tmp567;}),0U);Cyc_Absyn_assign_stmt(_tmp98E,(struct Cyc_Absyn_Exp*)_tmp566->hd,0U);});Cyc_Absyn_seq_stmt(_tmp98F,s2,0U);});}
# 3494
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp5B7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_tmp5B6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp557)->f2;_LL1A:
# 3500
{void*_tmp56B=_tmp5B7->r;void*_tmp56C=_tmp56B;struct Cyc_Absyn_Tvar*_tmp58B;struct Cyc_Absyn_Vardecl*_tmp58A;struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Absyn_Fndecl*_tmp588;struct Cyc_List_List*_tmp587;struct Cyc_Absyn_Pat*_tmp586;struct Cyc_Absyn_Exp*_tmp585;void*_tmp584;struct Cyc_Absyn_Vardecl*_tmp583;switch(*((int*)_tmp56C)){case 0U: _LL2E: _tmp583=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp56C)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp583,_tmp5B6);goto _LL2D;case 2U: _LL30: _tmp586=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp56C)->f1;_tmp585=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp56C)->f3;_tmp584=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp56C)->f4;_LL31:
# 3508
{void*_tmp56D=_tmp586->r;void*_tmp56E=_tmp56D;struct Cyc_Absyn_Vardecl*_tmp570;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp56E)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp56E)->f2)->r)->tag == 0U){_LL3B: _tmp570=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp56E)->f1;_LL3C:
# 3510
({struct _tuple1*_tmp990=Cyc_Toc_temp_var();_tmp570->name=_tmp990;});
_tmp570->initializer=_tmp585;
({void*_tmp991=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=0U,_tmp56F->f1=_tmp570;_tmp56F;});_tmp5B7->r=_tmp991;});
Cyc_Toc_local_decl_to_c(nv,_tmp570,_tmp5B6);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3519
({void*_tmp992=(Cyc_Toc_letdecl_to_c(nv,_tmp586,_tmp584,(void*)_check_null(_tmp585->topt),_tmp585,_tmp5B6))->r;s->r=_tmp992;});
goto _LL3A;}_LL3A:;}
# 3522
goto _LL2D;case 3U: _LL32: _tmp587=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp56C)->f1;_LL33: {
# 3526
struct Cyc_List_List*_tmp571=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp587);
if(_tmp571 == 0)
({void*_tmp572=0U;({struct _dyneither_ptr _tmp993=({const char*_tmp573="empty Letv_d";_tag_dyneither(_tmp573,sizeof(char),13U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp993,_tag_dyneither(_tmp572,sizeof(void*),0U));});});
({void*_tmp994=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->tag=0U,_tmp574->f1=(struct Cyc_Absyn_Vardecl*)_tmp571->hd;_tmp574;});_tmp5B7->r=_tmp994;});
_tmp571=_tmp571->tl;
for(0;_tmp571 != 0;_tmp571=_tmp571->tl){
struct Cyc_Absyn_Decl*_tmp575=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->tag=0U,_tmp576->f1=(struct Cyc_Absyn_Vardecl*)_tmp571->hd;_tmp576;}),0U);
({void*_tmp996=({struct Cyc_Absyn_Decl*_tmp995=_tmp575;Cyc_Absyn_decl_stmt(_tmp995,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmp996;});}
# 3535
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp588=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp56C)->f1;_LL35:
# 3538
 Cyc_Toc_fndecl_to_c(nv,_tmp588,0);
Cyc_Toc_stmt_to_c(nv,_tmp5B6);
goto _LL2D;case 4U: _LL36: _tmp58B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp56C)->f1;_tmp58A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp56C)->f2;_tmp589=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp56C)->f3;_LL37: {
# 3542
struct Cyc_Absyn_Stmt*_tmp577=_tmp5B6;
# 3544
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple29 _tmp578=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp579=_tmp578;struct _tuple1*_tmp580;struct Cyc_Absyn_Exp*_tmp57F;_LL40: _tmp580=_tmp579.f1;_tmp57F=_tmp579.f2;_LL41:;{
struct _tuple1*x_var=_tmp58A->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3550
Cyc_Toc_stmt_to_c(nv,_tmp577);
if(Cyc_Absyn_no_regions)
({void*_tmp99A=({struct _tuple1*_tmp999=x_var;void*_tmp998=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmp997=
Cyc_Absyn_uint_exp(0U,0U);
# 3552
Cyc_Absyn_declare_stmt(_tmp999,_tmp998,_tmp997,_tmp577,0U);})->r;s->r=_tmp99A;});else{
# 3554
if((unsigned int)_tmp589){
# 3556
Cyc_Toc_exp_to_c(nv,_tmp589);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmp99C=_tmp589;Cyc_Absyn_aggrarrow_exp(_tmp99C,({struct _dyneither_ptr*_tmp57B=_cycalloc(sizeof(*_tmp57B));({struct _dyneither_ptr _tmp99B=({const char*_tmp57A="h";_tag_dyneither(_tmp57A,sizeof(char),2U);});*_tmp57B=_tmp99B;});_tmp57B;}),0U);}),0U);
({void*_tmp99D=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp577,0U))->r;s->r=_tmp99D;});};}else{
# 3565
({void*_tmp9AA=({
struct _tuple1*_tmp9A9=_tmp580;void*_tmp9A8=rh_struct_typ;struct Cyc_Absyn_Exp*_tmp9A7=({struct Cyc_Absyn_Exp*_tmp57C[1U];({struct Cyc_Absyn_Exp*_tmp99E=
# 3568
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp57C[0]=_tmp99E;});({struct Cyc_Absyn_Exp*_tmp99F=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmp99F,_tag_dyneither(_tmp57C,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3566
Cyc_Absyn_declare_stmt(_tmp9A9,_tmp9A8,_tmp9A7,({
# 3569
struct _tuple1*_tmp9A6=x_var;void*_tmp9A5=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmp9A4=Cyc_Absyn_address_exp(_tmp57F,0U);Cyc_Absyn_declare_stmt(_tmp9A6,_tmp9A5,_tmp9A4,({
struct Cyc_Absyn_Stmt*_tmp9A3=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp57D[1U];_tmp57D[0]=x_exp;({struct Cyc_Absyn_Exp*_tmp9A0=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmp9A0,_tag_dyneither(_tmp57D,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmp9A3,({
struct Cyc_Absyn_Stmt*_tmp9A2=_tmp577;Cyc_Absyn_seq_stmt(_tmp9A2,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp57E[1U];_tmp57E[0]=x_exp;({struct Cyc_Absyn_Exp*_tmp9A1=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmp9A1,_tag_dyneither(_tmp57E,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3565
s->r=_tmp9AA;});}}
# 3575
return;};}default: _LL38: _LL39:
({void*_tmp581=0U;({struct _dyneither_ptr _tmp9AB=({const char*_tmp582="bad nested declaration within function";_tag_dyneither(_tmp582,sizeof(char),39U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp9AB,_tag_dyneither(_tmp581,sizeof(void*),0U));});});}_LL2D:;}
# 3578
return;case 13U: _LL1B: _tmp5B9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_tmp5B8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp557)->f2;_LL1C:
 s=_tmp5B8;continue;case 14U: _LL1D: _tmp5BB=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_tmp5BA=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp557)->f2).f1;_LL1E: {
# 3581
struct _RegionHandle _tmp58C=_new_region("temp");struct _RegionHandle*temp=& _tmp58C;_push_region(temp);
({struct Cyc_Toc_Env*_tmp9AC=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp9AC,_tmp5BB);});
Cyc_Toc_exp_to_c(nv,_tmp5BA);
# 3585
_npop_handler(0U);return;
# 3581
;_pop_region(temp);}default: _LL1F: _tmp5BE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp557)->f1;_tmp5BD=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp557)->f2;_tmp5BC=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp557)->f3;_LL20: {
# 3599 "toc.cyc"
struct _tuple29 _tmp58D=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp58E=_tmp58D;struct _tuple1*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A2;_LL43: _tmp5A3=_tmp58E.f1;_tmp5A2=_tmp58E.f2;_LL44:;{
struct _tuple29 _tmp58F=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp590=_tmp58F;struct _tuple1*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A0;_LL46: _tmp5A1=_tmp590.f1;_tmp5A0=_tmp590.f2;_LL47:;{
struct _tuple29 _tmp591=Cyc_Toc_temp_var_and_exp();struct _tuple29 _tmp592=_tmp591;struct _tuple1*_tmp59F;struct Cyc_Absyn_Exp*_tmp59E;_LL49: _tmp59F=_tmp592.f1;_tmp59E=_tmp592.f2;_LL4A:;{
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3606
_tmp5A0->topt=e_typ;{
struct _RegionHandle _tmp593=_new_region("temp");struct _RegionHandle*temp=& _tmp593;_push_region(temp);
# 3609
Cyc_Toc_stmt_to_c(nv,_tmp5BE);{
struct Cyc_Absyn_Stmt*_tmp594=({struct Cyc_Absyn_Stmt*_tmp9AE=_tmp5BE;Cyc_Absyn_seq_stmt(_tmp9AE,
Cyc_Absyn_exp_stmt(({void*_tmp59D=0U;({struct Cyc_Absyn_Exp*_tmp9AD=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmp9AD,_tag_dyneither(_tmp59D,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3613
struct Cyc_Absyn_Stmt*_tmp595=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->tag=10U,_tmp59C->f1=_tmp5A0,_tmp59C->f2=_tmp5BD,_tmp59C->f3=_tmp5BC;_tmp59C;}),0U);
# 3615
Cyc_Toc_stmt_to_c(nv,_tmp595);{
# 3618
struct Cyc_Absyn_Exp*_tmp596=({struct Cyc_Absyn_Exp*_tmp59B[1U];({struct Cyc_Absyn_Exp*_tmp9AF=
Cyc_Toc_member_exp(_tmp5A2,Cyc_Toc_handler_sp,0U);_tmp59B[0]=_tmp9AF;});({struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmp9B0,_tag_dyneither(_tmp59B,sizeof(struct Cyc_Absyn_Exp*),1U));});});
struct Cyc_Absyn_Stmt*_tmp597=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp59A[1U];({struct Cyc_Absyn_Exp*_tmp9B1=Cyc_Absyn_address_exp(_tmp5A2,0U);_tmp59A[0]=_tmp9B1;});({struct Cyc_Absyn_Exp*_tmp9B2=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmp9B2,_tag_dyneither(_tmp59A,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
struct Cyc_Absyn_Exp*_tmp598=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp599=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmp9C1=({
struct _tuple1*_tmp9C0=_tmp5A3;void*_tmp9BF=h_typ;Cyc_Absyn_declare_stmt(_tmp9C0,_tmp9BF,0,({
struct Cyc_Absyn_Stmt*_tmp9BE=_tmp597;Cyc_Absyn_seq_stmt(_tmp9BE,({
struct _tuple1*_tmp9BD=_tmp59F;void*_tmp9BC=was_thrown_typ;struct Cyc_Absyn_Exp*_tmp9BB=_tmp598;Cyc_Absyn_declare_stmt(_tmp9BD,_tmp9BC,_tmp9BB,({
struct Cyc_Absyn_Stmt*_tmp9BA=({struct Cyc_Absyn_Exp*_tmp9B4=_tmp596;struct Cyc_Absyn_Stmt*_tmp9B3=
Cyc_Absyn_assign_stmt(_tmp59E,_tmp599,0U);
# 3628
Cyc_Absyn_ifthenelse_stmt(_tmp9B4,_tmp9B3,
# 3630
Cyc_Toc_skip_stmt_dl(),0U);});
# 3628
Cyc_Absyn_seq_stmt(_tmp9BA,({
# 3631
struct Cyc_Absyn_Exp*_tmp9B9=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp59E,0U);struct Cyc_Absyn_Stmt*_tmp9B8=_tmp594;Cyc_Absyn_ifthenelse_stmt(_tmp9B9,_tmp9B8,({
# 3633
struct _tuple1*_tmp9B7=_tmp5A1;void*_tmp9B6=e_typ;struct Cyc_Absyn_Exp*_tmp9B5=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 3633
Cyc_Absyn_declare_stmt(_tmp9B7,_tmp9B6,_tmp9B5,_tmp595,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3624
s->r=_tmp9C1;});};};
# 3637
_npop_handler(0U);return;
# 3607
;_pop_region(temp);};};};};}}_LL0:;}}
# 3646
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmp9C2=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmp9C2;});
({void*_tmp9C3=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmp9C3;});{
struct _RegionHandle _tmp5BF=_new_region("frgn");struct _RegionHandle*frgn=& _tmp5BF;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp5C0=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp5C1=(f->i).args;for(0;_tmp5C1 != 0;_tmp5C1=_tmp5C1->tl){
struct _tuple1*_tmp5C2=({struct _tuple1*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->f1=Cyc_Absyn_Loc_n,_tmp5C3->f2=(*((struct _tuple9*)_tmp5C1->hd)).f1;_tmp5C3;});
({void*_tmp9C4=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp5C1->hd)).f3);(*((struct _tuple9*)_tmp5C1->hd)).f3=_tmp9C4;});}}
# 3663
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3667
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp5C4=_cycalloc(sizeof(*_tmp5C4));({struct Cyc_Hashtable_Table*_tmp9C5=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp5C4=_tmp9C5;});_tmp5C4;});
if((unsigned int)(f->i).cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp5C5=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp5C6=_tmp5C5;struct _dyneither_ptr*_tmp5CF;struct Cyc_Absyn_Tqual _tmp5CE;void*_tmp5CD;int _tmp5CC;_LL1: _tmp5CF=_tmp5C6.name;_tmp5CE=_tmp5C6.tq;_tmp5CD=_tmp5C6.type;_tmp5CC=_tmp5C6.inject;_LL2:;{
void*_tmp5C7=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp5CD,Cyc_Absyn_heap_rgn_type,_tmp5CE,Cyc_Absyn_false_type));
struct _tuple1*_tmp5C8=({struct _tuple1*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->f1=Cyc_Absyn_Loc_n,_tmp5CB->f2=(struct _dyneither_ptr*)_check_null(_tmp5CF);_tmp5CB;});
({struct Cyc_List_List*_tmp9C8=({struct Cyc_List_List*_tmp9C7=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9C7,({struct Cyc_List_List*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));({struct _tuple9*_tmp9C6=({struct _tuple9*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->f1=_tmp5CF,_tmp5C9->f2=_tmp5CE,_tmp5C9->f3=_tmp5C7;_tmp5C9;});_tmp5CA->hd=_tmp9C6;}),_tmp5CA->tl=0;_tmp5CA;}));});(f->i).args=_tmp9C8;});
(f->i).cyc_varargs=0;};}
# 3676
{struct Cyc_List_List*_tmp5D0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp5D0 != 0;_tmp5D0=_tmp5D0->tl){
({void*_tmp9C9=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp5D0->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp5D0->hd)->type=_tmp9C9;});}}
# 3679
({struct Cyc_Toc_Env*_tmp9CA=Cyc_Toc_clear_toplevel(frgn,_tmp5C0);Cyc_Toc_stmt_to_c(_tmp9CA,f->body);});}
# 3656
;_pop_region(frgn);};}
# 3683
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp5D1=s;switch(_tmp5D1){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3699 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 3701
struct _tuple39 _tmp5D2=*env;struct _tuple39 _tmp5D3=_tmp5D2;struct Cyc_Toc_TocState*_tmp607;struct Cyc_Absyn_Aggrdecl*_tmp606;_LL1: _tmp607=_tmp5D3.f1;_tmp606=*_tmp5D3.f2;_LL2:;{
struct _tuple1*_tmp5D4=_tmp606->name;
struct Cyc_Toc_TocState _tmp5D5=*_tmp607;struct Cyc_Toc_TocState _tmp5D6=_tmp5D5;struct Cyc_Dict_Dict*_tmp605;_LL4: _tmp605=_tmp5D6.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple18**_tmp5D7=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp605,_tmp5D4);
if(_tmp5D7 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if((int)_tmp606->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp5D8=_region_malloc(d,sizeof(*_tmp5D8));_tmp5D8->f1=_tmp606,({void*_tmp9CB=Cyc_Absyn_strctq(_tmp5D4);_tmp5D8->f2=_tmp9CB;});_tmp5D8;});else{
# 3712
v=({struct _tuple18*_tmp5D9=_region_malloc(d,sizeof(*_tmp5D9));_tmp5D9->f1=_tmp606,({void*_tmp9CC=Cyc_Absyn_unionq_type(_tmp5D4);_tmp5D9->f2=_tmp9CC;});_tmp5D9;});}
({struct Cyc_Dict_Dict _tmp9CD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp605,_tmp5D4,v);*_tmp605=_tmp9CD;});};}else{
# 3715
struct _tuple18*_tmp5DA=*_tmp5D7;struct _tuple18*_tmp5DB=_tmp5DA;struct Cyc_Absyn_Aggrdecl*_tmp5DE;void*_tmp5DD;_LL7: _tmp5DE=_tmp5DB->f1;_tmp5DD=_tmp5DB->f2;_LL8:;
if(_tmp5DE->impl == 0){
({struct Cyc_Dict_Dict _tmp9D0=({struct Cyc_Dict_Dict _tmp9CF=*_tmp605;struct _tuple1*_tmp9CE=_tmp5D4;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmp9CF,_tmp9CE,({struct _tuple18*_tmp5DC=_region_malloc(d,sizeof(*_tmp5DC));_tmp5DC->f1=_tmp606,_tmp5DC->f2=_tmp5DD;_tmp5DC;}));});*_tmp605=_tmp9D0;});
seen_defn_before=0;}else{
# 3720
seen_defn_before=1;}}{
# 3722
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->kind=_tmp606->kind,_tmp604->sc=Cyc_Absyn_Public,_tmp604->name=_tmp606->name,_tmp604->tvs=0,_tmp604->impl=0,_tmp604->expected_mem_kind=0,_tmp604->attributes=_tmp606->attributes;_tmp604;});
# 3729
if(_tmp606->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmp9D1=({struct Cyc_Absyn_AggrdeclImpl*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->exist_vars=0,_tmp5DF->rgn_po=0,_tmp5DF->fields=0,_tmp5DF->tagged=0;_tmp5DF;});new_ad->impl=_tmp9D1;});{
# 3734
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp5E0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp606->impl))->fields;for(0;_tmp5E0 != 0;_tmp5E0=_tmp5E0->tl){
# 3738
struct Cyc_Absyn_Aggrfield*_tmp5E1=(struct Cyc_Absyn_Aggrfield*)_tmp5E0->hd;
void*_tmp5E2=_tmp5E1->type;
struct Cyc_List_List*_tmp5E3=_tmp5E1->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp5E2)) && (
(int)_tmp606->kind == (int)Cyc_Absyn_StructA  && _tmp5E0->tl == 0  || (int)_tmp606->kind == (int)Cyc_Absyn_UnionA)){
# 3752 "toc.cyc"
void*_tmp5E4=Cyc_Tcutil_compress(_tmp5E2);void*_tmp5E5=_tmp5E4;void*_tmp5ED;struct Cyc_Absyn_Tqual _tmp5EC;void*_tmp5EB;unsigned int _tmp5EA;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E5)->tag == 4U){_LLA: _tmp5ED=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E5)->f1).elt_type;_tmp5EC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E5)->f1).tq;_tmp5EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E5)->f1).zero_term;_tmp5EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E5)->f1).zt_loc;_LLB:
# 3755
 _tmp5E2=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->tag=4U,(_tmp5E6->f1).elt_type=_tmp5ED,(_tmp5E6->f1).tq=_tmp5EC,({struct Cyc_Absyn_Exp*_tmp9D2=Cyc_Absyn_uint_exp(0U,0U);(_tmp5E6->f1).num_elts=_tmp9D2;}),(_tmp5E6->f1).zero_term=_tmp5EB,(_tmp5E6->f1).zt_loc=_tmp5EA;_tmp5E6;});
goto _LL9;}else{_LLC: _LLD:
# 3758
 _tmp5E3=({struct Cyc_List_List*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));({void*_tmp9D3=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->tag=6U,_tmp5E7->f1=0;_tmp5E7;});_tmp5E8->hd=_tmp9D3;}),_tmp5E8->tl=_tmp5E3;_tmp5E8;});
_tmp5E2=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->tag=4U,({void*_tmp9D6=Cyc_Toc_void_star_type();(_tmp5E9->f1).elt_type=_tmp9D6;}),({
struct Cyc_Absyn_Tqual _tmp9D5=Cyc_Absyn_empty_tqual(0U);(_tmp5E9->f1).tq=_tmp9D5;}),({
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Absyn_uint_exp(0U,0U);(_tmp5E9->f1).num_elts=_tmp9D4;}),(_tmp5E9->f1).zero_term=Cyc_Absyn_false_type,(_tmp5E9->f1).zt_loc=0U;_tmp5E9;});}_LL9:;}{
# 3765
struct Cyc_Absyn_Aggrfield*_tmp5EE=({struct Cyc_Absyn_Aggrfield*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->name=_tmp5E1->name,_tmp600->tq=Cyc_Toc_mt_tq,({
# 3767
void*_tmp9D7=Cyc_Toc_typ_to_c(_tmp5E2);_tmp600->type=_tmp9D7;}),_tmp600->width=_tmp5E1->width,_tmp600->attributes=_tmp5E3,_tmp600->requires_clause=0;_tmp600;});
# 3775
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp606->impl))->tagged){
void*_tmp5EF=_tmp5EE->type;
struct _dyneither_ptr*_tmp5F0=_tmp5EE->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5FD=({struct Cyc_String_pa_PrintArg_struct _tmp723;_tmp723.tag=0U,_tmp723.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp606->name).f2);_tmp723;});struct Cyc_String_pa_PrintArg_struct _tmp5FE=({struct Cyc_String_pa_PrintArg_struct _tmp722;_tmp722.tag=0U,_tmp722.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5F0);_tmp722;});void*_tmp5FB[2U];_tmp5FB[0]=& _tmp5FD,_tmp5FB[1]=& _tmp5FE;({struct _dyneither_ptr _tmp9D8=({const char*_tmp5FC="_union_%s_%s";_tag_dyneither(_tmp5FC,sizeof(char),13U);});Cyc_aprintf(_tmp9D8,_tag_dyneither(_tmp5FB,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));*_tmp5FA=s;_tmp5FA;});
struct Cyc_Absyn_Aggrfield*_tmp5F1=({struct Cyc_Absyn_Aggrfield*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->name=Cyc_Toc_val_sp,_tmp5F9->tq=Cyc_Toc_mt_tq,_tmp5F9->type=_tmp5EF,_tmp5F9->width=0,_tmp5F9->attributes=0,_tmp5F9->requires_clause=0;_tmp5F9;});
struct Cyc_Absyn_Aggrfield*_tmp5F2=({struct Cyc_Absyn_Aggrfield*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->name=Cyc_Toc_tag_sp,_tmp5F8->tq=Cyc_Toc_mt_tq,_tmp5F8->type=Cyc_Absyn_sint_type,_tmp5F8->width=0,_tmp5F8->attributes=0,_tmp5F8->requires_clause=0;_tmp5F8;});
struct Cyc_List_List*_tmp5F3=({struct Cyc_Absyn_Aggrfield*_tmp5F7[2U];_tmp5F7[0]=_tmp5F2,_tmp5F7[1]=_tmp5F1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5F7,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp5F4=Cyc_Toc_make_c_struct_defn(str,_tmp5F3);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));({struct Cyc_Absyn_Decl*_tmp9D9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->tag=5U,_tmp5F5->f1=_tmp5F4;_tmp5F5;}),0U);_tmp5F6->hd=_tmp9D9;}),_tmp5F6->tl=Cyc_Toc_result_decls;_tmp5F6;});
({void*_tmp9DA=Cyc_Absyn_strct(str);_tmp5EE->type=_tmp9DA;});}
# 3787
new_fields=({struct Cyc_List_List*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->hd=_tmp5EE,_tmp5FF->tl=new_fields;_tmp5FF;});};}}
# 3789
({struct Cyc_List_List*_tmp9DB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmp9DB;});};}
# 3792
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp603=_cycalloc(sizeof(*_tmp603));({struct Cyc_Absyn_Decl*_tmp9DD=({struct Cyc_Absyn_Decl*_tmp602=_cycalloc(sizeof(*_tmp602));({void*_tmp9DC=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->tag=5U,_tmp601->f1=new_ad;_tmp601;});_tmp602->r=_tmp9DC;}),_tmp602->loc=0U;_tmp602;});_tmp603->hd=_tmp9DD;}),_tmp603->tl=Cyc_Toc_result_decls;_tmp603;});
return 0;};};};}
# 3796
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple40{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3824 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3827
struct _tuple40 _tmp608=*env;struct _tuple40 _tmp609=_tmp608;struct Cyc_Set_Set**_tmp61A;struct Cyc_Absyn_Datatypedecl*_tmp619;_LL1: _tmp61A=(_tmp609.f1)->datatypes_so_far;_tmp619=_tmp609.f2;_LL2:;{
struct _tuple1*_tmp60A=_tmp619->name;
if(_tmp619->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp61A,_tmp60A))
return 0;
({struct Cyc_Set_Set*_tmp9DE=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp61A,_tmp60A);*_tmp61A=_tmp9DE;});
{struct Cyc_List_List*_tmp60B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp619->fields))->v;for(0;_tmp60B != 0;_tmp60B=_tmp60B->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp60B->hd;
# 3835
struct Cyc_List_List*_tmp60C=0;
int i=1;
{struct Cyc_List_List*_tmp60D=f->typs;for(0;_tmp60D != 0;(_tmp60D=_tmp60D->tl,i ++)){
struct _dyneither_ptr*_tmp60E=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp60F=({struct Cyc_Absyn_Aggrfield*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->name=_tmp60E,_tmp611->tq=(*((struct _tuple12*)_tmp60D->hd)).f1,({
void*_tmp9DF=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp60D->hd)).f2);_tmp611->type=_tmp9DF;}),_tmp611->width=0,_tmp611->attributes=0,_tmp611->requires_clause=0;_tmp611;});
_tmp60C=({struct Cyc_List_List*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->hd=_tmp60F,_tmp610->tl=_tmp60C;_tmp610;});}}
# 3843
_tmp60C=({struct Cyc_List_List*_tmp613=_cycalloc(sizeof(*_tmp613));({struct Cyc_Absyn_Aggrfield*_tmp9E1=({struct Cyc_Absyn_Aggrfield*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->name=Cyc_Toc_tag_sp,_tmp612->tq=Cyc_Toc_mt_tq,_tmp612->type=Cyc_Absyn_sint_type,_tmp612->width=0,_tmp612->attributes=0,_tmp612->requires_clause=0;_tmp612;});_tmp613->hd=_tmp9E1;}),({
struct Cyc_List_List*_tmp9E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp60C);_tmp613->tl=_tmp9E0;});_tmp613;});{
struct Cyc_Absyn_Aggrdecl*_tmp614=({struct _dyneither_ptr*_tmp9E3=({struct _dyneither_ptr*_tmp618=_cycalloc(sizeof(*_tmp618));({struct _dyneither_ptr _tmp9E2=({const char*_tmp617="";_tag_dyneither(_tmp617,sizeof(char),1U);});*_tmp618=_tmp9E2;});_tmp618;});Cyc_Toc_make_c_struct_defn(_tmp9E3,_tmp60C);});
({struct _tuple1*_tmp9E4=Cyc_Toc_collapse_qvars(f->name,_tmp619->name);_tmp614->name=_tmp9E4;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp616=_cycalloc(sizeof(*_tmp616));({struct Cyc_Absyn_Decl*_tmp9E5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=5U,_tmp615->f1=_tmp614;_tmp615;}),0U);_tmp616->hd=_tmp9E5;}),_tmp616->tl=Cyc_Toc_result_decls;_tmp616;});};}}
# 3849
return 0;};}
# 3852
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3871 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3874
struct _tuple40 _tmp61B=*env;struct _tuple40 _tmp61C=_tmp61B;struct Cyc_Toc_TocState*_tmp63E;struct Cyc_Absyn_Datatypedecl*_tmp63D;_LL1: _tmp63E=_tmp61C.f1;_tmp63D=_tmp61C.f2;_LL2:;
if(_tmp63D->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp61D=*_tmp63E;struct Cyc_Toc_TocState _tmp61E=_tmp61D;struct Cyc_Dict_Dict*_tmp63C;_LL4: _tmp63C=_tmp61E.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp61F=_tmp63D->name;
{struct Cyc_List_List*_tmp620=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp63D->fields))->v;for(0;_tmp620 != 0;_tmp620=_tmp620->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp620->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp621=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp622=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp621,Cyc_Absyn_false_type,0U);
# 3885
int*_tmp623=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp63C,f->name);int*_tmp624=_tmp623;if(_tmp624 == 0){_LL7: _LL8: {
# 3887
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3891
struct Cyc_Absyn_Vardecl*_tmp625=Cyc_Absyn_new_vardecl(0U,f->name,_tmp622,initopt);
_tmp625->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp627=_cycalloc(sizeof(*_tmp627));({struct Cyc_Absyn_Decl*_tmp9E6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->tag=0U,_tmp626->f1=_tmp625;_tmp626;}),0U);_tmp627->hd=_tmp9E6;}),_tmp627->tl=Cyc_Toc_result_decls;_tmp627;});
({struct Cyc_Dict_Dict _tmp9E7=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp63C,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3894
*_tmp63C=_tmp9E7;});{
# 3896
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp628=f->typs;for(0;_tmp628 != 0;(_tmp628=_tmp628->tl,i ++)){
struct _dyneither_ptr*_tmp629=({struct _dyneither_ptr*_tmp630=_cycalloc(sizeof(*_tmp630));({struct _dyneither_ptr _tmp9E9=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp62F=({struct Cyc_Int_pa_PrintArg_struct _tmp724;_tmp724.tag=1U,_tmp724.f1=(unsigned long)i;_tmp724;});void*_tmp62D[1U];_tmp62D[0]=& _tmp62F;({struct _dyneither_ptr _tmp9E8=({const char*_tmp62E="f%d";_tag_dyneither(_tmp62E,sizeof(char),4U);});Cyc_aprintf(_tmp9E8,_tag_dyneither(_tmp62D,sizeof(void*),1U));});});*_tmp630=_tmp9E9;});_tmp630;});
struct Cyc_Absyn_Aggrfield*_tmp62A=({struct Cyc_Absyn_Aggrfield*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->name=_tmp629,_tmp62C->tq=(*((struct _tuple12*)_tmp628->hd)).f1,({
void*_tmp9EA=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp628->hd)).f2);_tmp62C->type=_tmp9EA;}),_tmp62C->width=0,_tmp62C->attributes=0,_tmp62C->requires_clause=0;_tmp62C;});
fields=({struct Cyc_List_List*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->hd=_tmp62A,_tmp62B->tl=fields;_tmp62B;});}}
# 3904
fields=({struct Cyc_List_List*_tmp632=_cycalloc(sizeof(*_tmp632));({struct Cyc_Absyn_Aggrfield*_tmp9ED=({struct Cyc_Absyn_Aggrfield*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->name=Cyc_Toc_tag_sp,_tmp631->tq=Cyc_Toc_mt_tq,({
void*_tmp9EC=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp631->type=_tmp9EC;}),_tmp631->width=0,_tmp631->attributes=0,_tmp631->requires_clause=0;_tmp631;});
# 3904
_tmp632->hd=_tmp9ED;}),({
# 3906
struct Cyc_List_List*_tmp9EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp632->tl=_tmp9EB;});_tmp632;});{
struct Cyc_Absyn_Aggrdecl*_tmp633=({struct _dyneither_ptr*_tmp9EF=({struct _dyneither_ptr*_tmp637=_cycalloc(sizeof(*_tmp637));({struct _dyneither_ptr _tmp9EE=({const char*_tmp636="";_tag_dyneither(_tmp636,sizeof(char),1U);});*_tmp637=_tmp9EE;});_tmp637;});Cyc_Toc_make_c_struct_defn(_tmp9EF,fields);});
({struct _tuple1*_tmp9F0=Cyc_Toc_collapse_qvars(f->name,_tmp63D->name);_tmp633->name=_tmp9F0;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp635=_cycalloc(sizeof(*_tmp635));({struct Cyc_Absyn_Decl*_tmp9F1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=5U,_tmp634->f1=_tmp633;_tmp634;}),0U);_tmp635->hd=_tmp9F1;}),_tmp635->tl=Cyc_Toc_result_decls;_tmp635;});
goto _LL6;};};};}}else{if(*((int*)_tmp624)== 0){_LL9: _LLA:
# 3912
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp638=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp639=Cyc_Absyn_new_vardecl(0U,f->name,_tmp622,_tmp638);
_tmp639->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp63B=_cycalloc(sizeof(*_tmp63B));({struct Cyc_Absyn_Decl*_tmp9F2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->tag=0U,_tmp63A->f1=_tmp639;_tmp63A;}),0U);_tmp63B->hd=_tmp9F2;}),_tmp63B->tl=Cyc_Toc_result_decls;_tmp63B;});
({struct Cyc_Dict_Dict _tmp9F3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp63C,f->name,1);*_tmp63C=_tmp9F3;});}
# 3919
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3923
return 0;};};}
# 3926
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3930
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3936
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmp9F4=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmp9F4;});
# 3940
if((int)vd->sc == (int)Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 3949
struct _RegionHandle _tmp63F=_new_region("temp");struct _RegionHandle*temp=& _tmp63F;_push_region(temp);
{struct Cyc_Toc_Env*_tmp640=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp640,init);}
# 3950
;_pop_region(temp);}else{
# 3954
Cyc_Toc_exp_to_c(nv,init);}}else{
# 3957
void*_tmp641=Cyc_Tcutil_compress(old_typ);void*_tmp642=_tmp641;void*_tmp649;struct Cyc_Absyn_Exp*_tmp648;void*_tmp647;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp642)->tag == 4U){_LL1: _tmp649=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp642)->f1).elt_type;_tmp648=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp642)->f1).num_elts;_tmp647=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp642)->f1).zero_term;_LL2:
# 3959
 if(Cyc_Tcutil_force_type2bool(0,_tmp647)){
if(_tmp648 == 0)
({void*_tmp643=0U;({struct _dyneither_ptr _tmp9F5=({const char*_tmp644="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp644,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F5,_tag_dyneither(_tmp643,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp648;
struct Cyc_Absyn_Exp*_tmp645=({struct Cyc_Absyn_Exp*_tmp9F7=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmp9F7,({
struct Cyc_Absyn_Exp*_tmp9F6=num_elts;Cyc_Absyn_add_exp(_tmp9F6,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 3966
struct Cyc_Absyn_Exp*_tmp646=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmp9F9=({struct Cyc_Absyn_Stmt*_tmp9F8=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp645,_tmp646,0U),0U);Cyc_Toc_seq_stmt_r(_tmp9F8,
Cyc_Absyn_new_stmt(s->r,0U));});
# 3967
s->r=_tmp9F9;});};}
# 3970
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 3978
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp64A=d;struct Cyc_List_List*_tmp651;struct Cyc_List_List*_tmp650;void**_tmp64F;struct Cyc_Tcpat_Rhs*_tmp64E;switch(*((int*)_tmp64A)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp64E=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp64A)->f1;_LL4:
 _tmp64E->rhs=success;return d;default: _LL5: _tmp651=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp64A)->f1;_tmp650=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp64A)->f2;_tmp64F=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp64A)->f3;_LL6:
# 3983
({void*_tmp9FA=Cyc_Toc_rewrite_decision(*_tmp64F,success);*_tmp64F=_tmp9FA;});
for(0;_tmp650 != 0;_tmp650=_tmp650->tl){
struct _tuple32*_tmp64B=(struct _tuple32*)_tmp650->hd;struct _tuple32*_tmp64C=_tmp64B;void**_tmp64D;_LL8: _tmp64D=(void**)& _tmp64C->f2;_LL9:;
({void*_tmp9FB=Cyc_Toc_rewrite_decision(*_tmp64D,success);*_tmp64D=_tmp9FB;});}
# 3988
return d;}_LL0:;}
# 3999 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4001
struct _RegionHandle _tmp652=_new_region("rgn");struct _RegionHandle*rgn=& _tmp652;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp653=Cyc_Toc_share_env(rgn,nv);
void*_tmp654=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp653,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4011
struct Cyc_Absyn_Stmt*_tmp655=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4015
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp655);{
# 4017
struct Cyc_Absyn_Switch_clause*_tmp656=({struct Cyc_Absyn_Switch_clause*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->pattern=p,_tmp662->pat_vars=0,_tmp662->where_clause=0,_tmp662->body=_tmp655,_tmp662->loc=0U;_tmp662;});
struct Cyc_List_List*_tmp657=({struct _RegionHandle*_tmp9FE=rgn;struct _RegionHandle*_tmp9FD=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp9FE,Cyc_Toc_gen_labels,_tmp9FD,({struct Cyc_Absyn_Switch_clause*_tmp661[1U];_tmp661[0]=_tmp656;({struct _RegionHandle*_tmp9FC=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp9FC,_tag_dyneither(_tmp661,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4020
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4024
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp653,& mydecls,& mybodies,dopt,_tmp657,v);
# 4027
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp658=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp659=_tmp658;struct Cyc_Toc_Env*_tmp65B;struct Cyc_Absyn_Stmt*_tmp65A;_LL1: _tmp65B=_tmp659->f1;_tmp65A=_tmp659->f3;_LL2:;
if(_tmp65A == _tmp655){senv=_tmp65B;goto FOUND_ENV;}}
# 4032
({void*_tmp65C=0U;({struct _dyneither_ptr _tmp9FF=({const char*_tmp65D="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp65D,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FF,_tag_dyneither(_tmp65C,sizeof(void*),0U));});});
FOUND_ENV:
# 4036
 Cyc_Toc_stmt_to_c(senv,s);{
# 4038
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4040
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp65E=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpA00=Cyc_Toc_temp_var();_tmp65E->name=_tmpA00;});
({void*_tmpA01=Cyc_Toc_typ_to_c_array(_tmp65E->type);_tmp65E->type=_tmpA01;});
res=({struct Cyc_Absyn_Decl*_tmpA02=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->tag=0U,_tmp65F->f1=_tmp65E;_tmp65F;}),0U);Cyc_Absyn_decl_stmt(_tmpA02,res,0U);});}
# 4047
res=({struct _tuple1*_tmpA05=v;void*_tmpA04=Cyc_Toc_typ_to_c(_tmp654);struct Cyc_Absyn_Exp*_tmpA03=e;Cyc_Absyn_declare_stmt(_tmpA05,_tmpA04,_tmpA03,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp660=res;_npop_handler(0U);return _tmp660;};};};};}
# 4002
;_pop_region(rgn);}
# 4055
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp663=e->r;void*_tmp664=_tmp663;struct Cyc_Absyn_MallocInfo*_tmp68F;struct Cyc_Absyn_Stmt*_tmp68E;void**_tmp68D;void**_tmp68C;struct Cyc_List_List*_tmp68B;struct Cyc_List_List*_tmp68A;void**_tmp689;struct Cyc_List_List*_tmp688;void**_tmp687;struct Cyc_Absyn_Exp*_tmp686;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_List_List*_tmp684;struct Cyc_Absyn_Exp*_tmp683;struct Cyc_Absyn_Exp*_tmp682;struct Cyc_Absyn_Exp*_tmp681;struct Cyc_Absyn_Exp*_tmp680;struct Cyc_Absyn_Exp*_tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_Absyn_Exp*_tmp679;struct Cyc_Absyn_Exp*_tmp678;struct Cyc_Absyn_Exp*_tmp677;struct Cyc_Absyn_Exp*_tmp676;struct Cyc_Absyn_Exp*_tmp675;struct Cyc_List_List*_tmp674;struct Cyc_Absyn_Exp*_tmp673;struct Cyc_Absyn_Exp*_tmp672;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_Absyn_Exp*_tmp670;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_Absyn_Exp*_tmp66D;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66B;switch(*((int*)_tmp664)){case 41U: _LL1: _tmp66B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL2:
 _tmp66C=_tmp66B;goto _LL4;case 20U: _LL3: _tmp66C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL4:
 _tmp66D=_tmp66C;goto _LL6;case 21U: _LL5: _tmp66D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL6:
 _tmp66E=_tmp66D;goto _LL8;case 22U: _LL7: _tmp66E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL8:
 _tmp66F=_tmp66E;goto _LLA;case 15U: _LL9: _tmp66F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LLA:
 _tmp670=_tmp66F;goto _LLC;case 11U: _LLB: _tmp670=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LLC:
 _tmp671=_tmp670;goto _LLE;case 12U: _LLD: _tmp671=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LLE:
 _tmp672=_tmp671;goto _LL10;case 18U: _LLF: _tmp672=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL10:
 _tmp673=_tmp672;goto _LL12;case 5U: _LL11: _tmp673=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp673);goto _LL0;case 3U: _LL13: _tmp674=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp674);goto _LL0;case 7U: _LL15: _tmp676=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_tmp675=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL16:
 _tmp678=_tmp676;_tmp677=_tmp675;goto _LL18;case 8U: _LL17: _tmp678=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_tmp677=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL18:
 _tmp67A=_tmp678;_tmp679=_tmp677;goto _LL1A;case 9U: _LL19: _tmp67A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_tmp679=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL1A:
 _tmp67C=_tmp67A;_tmp67B=_tmp679;goto _LL1C;case 23U: _LL1B: _tmp67C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_tmp67B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL1C:
 _tmp67E=_tmp67C;_tmp67D=_tmp67B;goto _LL1E;case 35U: _LL1D: _tmp67E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_tmp67D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL1E:
 _tmp680=_tmp67E;_tmp67F=_tmp67D;goto _LL20;case 4U: _LL1F: _tmp680=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_tmp67F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp664)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp680);Cyc_Toc_exptypes_to_c(_tmp67F);goto _LL0;case 6U: _LL21: _tmp683=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_tmp682=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_tmp681=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp664)->f3;_LL22:
# 4074
 Cyc_Toc_exptypes_to_c(_tmp683);Cyc_Toc_exptypes_to_c(_tmp682);Cyc_Toc_exptypes_to_c(_tmp681);goto _LL0;case 10U: _LL23: _tmp685=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_tmp684=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL24:
# 4076
 Cyc_Toc_exptypes_to_c(_tmp685);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp684);goto _LL0;case 14U: _LL25: _tmp687=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_tmp686=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL26:
({void*_tmpA06=Cyc_Toc_typ_to_c(*_tmp687);*_tmp687=_tmpA06;});Cyc_Toc_exptypes_to_c(_tmp686);goto _LL0;case 25U: _LL27: _tmp689=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp664)->f1)->f3;_tmp688=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL28:
# 4079
({void*_tmpA07=Cyc_Toc_typ_to_c(*_tmp689);*_tmp689=_tmpA07;});
_tmp68A=_tmp688;goto _LL2A;case 36U: _LL29: _tmp68A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp664)->f2;_LL2A:
 _tmp68B=_tmp68A;goto _LL2C;case 26U: _LL2B: _tmp68B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL2C:
# 4083
 for(0;_tmp68B != 0;_tmp68B=_tmp68B->tl){
struct _tuple19 _tmp665=*((struct _tuple19*)_tmp68B->hd);struct _tuple19 _tmp666=_tmp665;struct Cyc_Absyn_Exp*_tmp667;_LL56: _tmp667=_tmp666.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp667);}
# 4087
goto _LL0;case 19U: _LL2D: _tmp68C=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL2E:
 _tmp68D=_tmp68C;goto _LL30;case 17U: _LL2F: _tmp68D=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL30:
({void*_tmpA08=Cyc_Toc_typ_to_c(*_tmp68D);*_tmp68D=_tmpA08;});goto _LL0;case 37U: _LL31: _tmp68E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp68E);goto _LL0;case 34U: _LL33: _tmp68F=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp664)->f1;_LL34:
# 4092
 if(_tmp68F->elt_type != 0)
({void**_tmpA0A=({void**_tmp668=_cycalloc(sizeof(*_tmp668));({void*_tmpA09=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp68F->elt_type)));*_tmp668=_tmpA09;});_tmp668;});_tmp68F->elt_type=_tmpA0A;});
Cyc_Toc_exptypes_to_c(_tmp68F->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4102
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
# 4113
({void*_tmp669=0U;({unsigned int _tmpA0C=e->loc;struct _dyneither_ptr _tmpA0B=({const char*_tmp66A="Cyclone expression within C code";_tag_dyneither(_tmp66A,sizeof(char),33U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA0C,_tmpA0B,_tag_dyneither(_tmp669,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4117
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp690=d->r;void*_tmp691=_tmp690;struct Cyc_Absyn_Typedefdecl*_tmp69B;struct Cyc_Absyn_Enumdecl*_tmp69A;struct Cyc_Absyn_Aggrdecl*_tmp699;struct Cyc_Absyn_Fndecl*_tmp698;struct Cyc_Absyn_Vardecl*_tmp697;switch(*((int*)_tmp691)){case 0U: _LL1: _tmp697=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp691)->f1;_LL2:
# 4120
({void*_tmpA0D=Cyc_Toc_typ_to_c(_tmp697->type);_tmp697->type=_tmpA0D;});
if(_tmp697->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp697->initializer));
goto _LL0;case 1U: _LL3: _tmp698=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp691)->f1;_LL4:
# 4124
({void*_tmpA0E=Cyc_Toc_typ_to_c((_tmp698->i).ret_type);(_tmp698->i).ret_type=_tmpA0E;});
{struct Cyc_List_List*_tmp692=(_tmp698->i).args;for(0;_tmp692 != 0;_tmp692=_tmp692->tl){
({void*_tmpA0F=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp692->hd)).f3);(*((struct _tuple9*)_tmp692->hd)).f3=_tmpA0F;});}}
# 4128
goto _LL0;case 5U: _LL5: _tmp699=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp691)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp699);goto _LL0;case 7U: _LL7: _tmp69A=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp691)->f1;_LL8:
# 4131
 if(_tmp69A->fields != 0){
struct Cyc_List_List*_tmp693=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp69A->fields))->v;for(0;_tmp693 != 0;_tmp693=_tmp693->tl){
struct Cyc_Absyn_Enumfield*_tmp694=(struct Cyc_Absyn_Enumfield*)_tmp693->hd;
if(_tmp694->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp694->tag));}}
# 4136
goto _LL0;case 8U: _LL9: _tmp69B=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp691)->f1;_LLA:
({void*_tmpA10=Cyc_Toc_typ_to_c((void*)_check_null(_tmp69B->defn));_tmp69B->defn=_tmpA10;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4146
({void*_tmp695=0U;({unsigned int _tmpA12=d->loc;struct _dyneither_ptr _tmpA11=({const char*_tmp696="Cyclone declaration within C code";_tag_dyneither(_tmp696,sizeof(char),34U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA12,_tmpA11,_tag_dyneither(_tmp695,sizeof(void*),0U));});});goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 4151
 goto _LL0;}_LL0:;}
# 4155
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp69C=s->r;void*_tmp69D=_tmp69C;struct Cyc_Absyn_Stmt*_tmp6B5;struct Cyc_Absyn_Stmt*_tmp6B4;struct Cyc_Absyn_Exp*_tmp6B3;struct Cyc_Absyn_Decl*_tmp6B2;struct Cyc_Absyn_Stmt*_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B0;struct Cyc_List_List*_tmp6AF;void*_tmp6AE;struct Cyc_Absyn_Exp*_tmp6AD;struct Cyc_Absyn_Exp*_tmp6AC;struct Cyc_Absyn_Exp*_tmp6AB;struct Cyc_Absyn_Stmt*_tmp6AA;struct Cyc_Absyn_Exp*_tmp6A9;struct Cyc_Absyn_Stmt*_tmp6A8;struct Cyc_Absyn_Exp*_tmp6A7;struct Cyc_Absyn_Stmt*_tmp6A6;struct Cyc_Absyn_Stmt*_tmp6A5;struct Cyc_Absyn_Exp*_tmp6A4;struct Cyc_Absyn_Stmt*_tmp6A3;struct Cyc_Absyn_Stmt*_tmp6A2;struct Cyc_Absyn_Exp*_tmp6A1;switch(*((int*)_tmp69D)){case 1U: _LL1: _tmp6A1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp69D)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp6A1);goto _LL0;case 2U: _LL3: _tmp6A3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp69D)->f1;_tmp6A2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp69D)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp6A3);Cyc_Toc_stmttypes_to_c(_tmp6A2);goto _LL0;case 3U: _LL5: _tmp6A4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp69D)->f1;_LL6:
 if(_tmp6A4 != 0)Cyc_Toc_exptypes_to_c(_tmp6A4);goto _LL0;case 4U: _LL7: _tmp6A7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp69D)->f1;_tmp6A6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp69D)->f2;_tmp6A5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp69D)->f3;_LL8:
# 4161
 Cyc_Toc_exptypes_to_c(_tmp6A7);Cyc_Toc_stmttypes_to_c(_tmp6A6);Cyc_Toc_stmttypes_to_c(_tmp6A5);goto _LL0;case 5U: _LL9: _tmp6A9=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp69D)->f1).f1;_tmp6A8=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp69D)->f2;_LLA:
# 4163
 Cyc_Toc_exptypes_to_c(_tmp6A9);Cyc_Toc_stmttypes_to_c(_tmp6A8);goto _LL0;case 9U: _LLB: _tmp6AD=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp69D)->f1;_tmp6AC=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp69D)->f2).f1;_tmp6AB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp69D)->f3).f1;_tmp6AA=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp69D)->f4;_LLC:
# 4165
 Cyc_Toc_exptypes_to_c(_tmp6AD);Cyc_Toc_exptypes_to_c(_tmp6AC);Cyc_Toc_exptypes_to_c(_tmp6AB);
Cyc_Toc_stmttypes_to_c(_tmp6AA);goto _LL0;case 10U: _LLD: _tmp6B0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp69D)->f1;_tmp6AF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp69D)->f2;_tmp6AE=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp69D)->f3;_LLE:
# 4168
 Cyc_Toc_exptypes_to_c(_tmp6B0);
for(0;_tmp6AF != 0;_tmp6AF=_tmp6AF->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp6AF->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp6B2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp69D)->f1;_tmp6B1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp69D)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp6B2);Cyc_Toc_stmttypes_to_c(_tmp6B1);goto _LL0;case 14U: _LL11: _tmp6B4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp69D)->f1;_tmp6B3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp69D)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp6B4);Cyc_Toc_exptypes_to_c(_tmp6B3);goto _LL0;case 13U: _LL13: _tmp6B5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp69D)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp6B5);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4178
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4181
({void*_tmpA13=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E->tag=0U;_tmp69E;});s->r=_tmpA13;});
goto _LL0;default: _LL1F: _LL20:
# 4184
({void*_tmp69F=0U;({unsigned int _tmpA15=s->loc;struct _dyneither_ptr _tmpA14=({const char*_tmp6A0="Cyclone statement in C code";_tag_dyneither(_tmp6A0,sizeof(char),28U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA15,_tmpA14,_tag_dyneither(_tmp69F,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4188
static void*Cyc_Toc_decl_to_enumtype(struct Cyc_Absyn_Decl*d){
void*_tmp6B6=d->r;void*_tmp6B7=_tmp6B6;struct Cyc_Absyn_Enumdecl*_tmp6BC;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6B7)->tag == 7U){_LL1: _tmp6BC=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6B7)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->tag=0U,({void*_tmpA16=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->tag=15U,_tmp6B8->f1=_tmp6BC->name,_tmp6B8->f2=_tmp6BC;_tmp6B8;});_tmp6B9->f1=_tmpA16;}),_tmp6B9->f2=0;_tmp6B9;});}else{_LL3: _LL4:
({void*_tmp6BA=0U;({struct _dyneither_ptr _tmpA17=({const char*_tmp6BB="Toc::get_enumdecl_name";_tag_dyneither(_tmp6BB,sizeof(char),23U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmpA17,_tag_dyneither(_tmp6BA,sizeof(void*),0U));});});}_LL0:;}
# 4195
static struct Cyc_Absyn_Decl*Cyc_Toc_type2enumdecl(void*t){
void*_tmp6BD=t;struct Cyc_Absyn_Enumdecl*_tmp6C1;unsigned int _tmp6C0;if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6BD)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6BD)->f1)->r)->tag == 1U){_LL1: _tmp6C1=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6BD)->f1)->r)->f1;_tmp6C0=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6BD)->f1)->loc;_LL2:
# 4198
 return({struct Cyc_Absyn_Decl*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));({void*_tmpA18=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->tag=7U,_tmp6BE->f1=_tmp6C1;_tmp6BE;});_tmp6BF->r=_tmpA18;}),_tmp6BF->loc=_tmp6C0;_tmp6BF;});}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 4206
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp6C2=0U;({struct _dyneither_ptr _tmpA19=({const char*_tmp6C3="decls_to_c: not at toplevel!";_tag_dyneither(_tmp6C3,sizeof(char),29U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmpA19,_tag_dyneither(_tmp6C2,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp6C4=d->r;void*_tmp6C5=_tmp6C4;struct Cyc_List_List*_tmp6E0;struct Cyc_List_List*_tmp6DF;struct Cyc_List_List*_tmp6DE;struct Cyc_List_List*_tmp6DD;struct Cyc_Absyn_Typedefdecl*_tmp6DC;struct Cyc_Absyn_Enumdecl*_tmp6DB;struct Cyc_Absyn_Datatypedecl*_tmp6DA;struct Cyc_Absyn_Aggrdecl*_tmp6D9;struct Cyc_Absyn_Fndecl*_tmp6D8;struct Cyc_Absyn_Vardecl*_tmp6D7;switch(*((int*)_tmp6C5)){case 0U: _LL1: _tmp6D7=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6C5)->f1;_LL2: {
# 4214
struct _tuple1*_tmp6C6=_tmp6D7->name;
# 4216
if((int)_tmp6D7->sc == (int)Cyc_Absyn_ExternC)
_tmp6C6=({struct _tuple1*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));({union Cyc_Absyn_Nmspace _tmpA1A=Cyc_Absyn_Abs_n(0,1);_tmp6C7->f1=_tmpA1A;}),_tmp6C7->f2=(*_tmp6C6).f2;_tmp6C7;});
if(_tmp6D7->initializer != 0){
if((int)_tmp6D7->sc == (int)Cyc_Absyn_ExternC)_tmp6D7->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6D7->initializer));else{
# 4223
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp6D7->initializer));}}
# 4225
_tmp6D7->name=_tmp6C6;
({enum Cyc_Absyn_Scope _tmpA1B=Cyc_Toc_scope_to_c(_tmp6D7->sc);_tmp6D7->sc=_tmpA1B;});
({void*_tmpA1C=Cyc_Toc_typ_to_c(_tmp6D7->type);_tmp6D7->type=_tmpA1C;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->hd=d,_tmp6C8->tl=Cyc_Toc_result_decls;_tmp6C8;});
goto _LL0;}case 1U: _LL3: _tmp6D8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6C5)->f1;_LL4: {
# 4231
struct _tuple1*_tmp6C9=_tmp6D8->name;
# 4233
if((int)_tmp6D8->sc == (int)Cyc_Absyn_ExternC){
_tmp6C9=({struct _tuple1*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));({union Cyc_Absyn_Nmspace _tmpA1D=Cyc_Absyn_Abs_n(0,1);_tmp6CA->f1=_tmpA1D;}),_tmp6CA->f2=(*_tmp6C9).f2;_tmp6CA;});
_tmp6D8->sc=Cyc_Absyn_Public;}
# 4237
_tmp6D8->name=_tmp6C9;
Cyc_Toc_fndecl_to_c(nv,_tmp6D8,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->hd=d,_tmp6CB->tl=Cyc_Toc_result_decls;_tmp6CB;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp6CC=0U;({struct _dyneither_ptr _tmpA1E=({const char*_tmp6CD="letdecl at toplevel";_tag_dyneither(_tmp6CD,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA1E,_tag_dyneither(_tmp6CC,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp6CE=0U;({struct _dyneither_ptr _tmpA1F=({const char*_tmp6CF="region decl at toplevel";_tag_dyneither(_tmp6CF,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA1F,_tag_dyneither(_tmp6CE,sizeof(void*),0U));});});case 5U: _LLB: _tmp6D9=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6C5)->f1;_LLC:
# 4245
 Cyc_Toc_aggrdecl_to_c(_tmp6D9);
goto _LL0;case 6U: _LLD: _tmp6DA=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp6C5)->f1;_LLE:
# 4248
 if(_tmp6DA->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp6DA);else{
# 4251
Cyc_Toc_datatypedecl_to_c(_tmp6DA);}
goto _LL0;case 7U: _LLF: _tmp6DB=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C5)->f1;_LL10:
# 4254
 Cyc_Toc_enumdecl_to_c(_tmp6DB);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->hd=d,_tmp6D0->tl=Cyc_Toc_result_decls;_tmp6D0;});
goto _LL0;case 8U: _LL11: _tmp6DC=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6C5)->f1;_LL12:
# 4258
 _tmp6DC->tvs=0;
if(_tmp6DC->defn != 0){
void*_tmp6D1=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6DC->defn));
_tmp6DC->defn=_tmp6D1;{
# 4264
struct Cyc_Absyn_Decl*_tmp6D2=Cyc_Toc_type2enumdecl(_tmp6D1);
if(_tmp6D2 != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->hd=_tmp6D2,_tmp6D3->tl=Cyc_Toc_result_decls;_tmp6D3;});
({void*_tmpA20=Cyc_Toc_decl_to_enumtype(_tmp6D2);_tmp6DC->defn=_tmpA20;});}};}else{
# 4270
enum Cyc_Absyn_KindQual _tmp6D4=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp6DC->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp6D5=_tmp6D4;if(_tmp6D5 == Cyc_Absyn_BoxKind){_LL24: _LL25:
({void*_tmpA21=Cyc_Toc_void_star_type();_tmp6DC->defn=_tmpA21;});goto _LL23;}else{_LL26: _LL27:
 _tmp6DC->defn=Cyc_Absyn_void_type;goto _LL23;}_LL23:;}
# 4278
if(Cyc_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6->hd=d,_tmp6D6->tl=Cyc_Toc_result_decls;_tmp6D6;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
# 4285
 goto _LL0;case 9U: _LL1B: _tmp6DD=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6C5)->f2;_LL1C:
 _tmp6DE=_tmp6DD;goto _LL1E;case 10U: _LL1D: _tmp6DE=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6C5)->f2;_LL1E:
 _tmp6DF=_tmp6DE;goto _LL20;case 11U: _LL1F: _tmp6DF=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6C5)->f1;_LL20:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp6DF,top,cinclude);goto _LL0;default: _LL21: _tmp6E0=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6C5)->f1;_LL22:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp6E0,top,1);goto _LL0;}_LL0:;};}
# 4292
return nv;}
# 4296
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp6E1=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp6E2=_tmp6E1;struct Cyc_Core_DynamicRegion*_tmp6E7;_LL1: _tmp6E7=_tmp6E2.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp6E7->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp6E4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6E4));_tmp6E4->dyn=_tmp6E7,_tmp6E4->state=ts;_tmp6E4;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_dyneither(({unsigned int _tmp6E6=34;struct _dyneither_ptr**_tmp6E5=_cycalloc(_check_times(_tmp6E6,sizeof(struct _dyneither_ptr*)));_tmp6E5[0]=& Cyc_Toc__throw_str,_tmp6E5[1]=& Cyc_Toc_setjmp_str,_tmp6E5[2]=& Cyc_Toc__push_handler_str,_tmp6E5[3]=& Cyc_Toc__pop_handler_str,_tmp6E5[4]=& Cyc_Toc__exn_thrown_str,_tmp6E5[5]=& Cyc_Toc__npop_handler_str,_tmp6E5[6]=& Cyc_Toc__check_null_str,_tmp6E5[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp6E5[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp6E5[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp6E5[10]=& Cyc_Toc__tag_dyneither_str,_tmp6E5[11]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp6E5[12]=& Cyc_Toc__get_dyneither_size_str,_tmp6E5[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp6E5[14]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp6E5[15]=& Cyc_Toc__zero_arr_plus_str,_tmp6E5[16]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp6E5[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp6E5[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp6E5[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp6E5[20]=& Cyc_Toc__cycalloc_str,_tmp6E5[21]=& Cyc_Toc__cyccalloc_str,_tmp6E5[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp6E5[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp6E5[24]=& Cyc_Toc__region_malloc_str,_tmp6E5[25]=& Cyc_Toc__region_calloc_str,_tmp6E5[26]=& Cyc_Toc__check_times_str,_tmp6E5[27]=& Cyc_Toc__new_region_str,_tmp6E5[28]=& Cyc_Toc__push_region_str,_tmp6E5[29]=& Cyc_Toc__pop_region_str,_tmp6E5[30]=& Cyc_Toc__throw_arraybounds_str,_tmp6E5[31]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp6E5[32]=& Cyc_Toc__throw_match_str,_tmp6E5[33]=& Cyc_Toc__fast_region_malloc_str;_tmp6E5;}),sizeof(struct _dyneither_ptr*),34U);};}
# 4344
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp6E8=ts;struct Cyc_Toc_TocStateWrap*_tmp6E9=Cyc_Toc_toc_state;ts=_tmp6E9;Cyc_Toc_toc_state=_tmp6E8;});{
struct Cyc_Toc_TocStateWrap _tmp6EA=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp6EB=_tmp6EA;struct Cyc_Core_DynamicRegion*_tmp6F1;struct Cyc_Toc_TocState*_tmp6F0;_LL1: _tmp6F1=_tmp6EB.dyn;_tmp6F0=_tmp6EB.state;_LL2:;
# 4349
{struct _RegionHandle*h=& _tmp6F1->h;
{struct Cyc_Toc_TocState _tmp6ED=*_tmp6F0;struct Cyc_Toc_TocState _tmp6EE=_tmp6ED;struct Cyc_Xarray_Xarray*_tmp6EF;_LL4: _tmp6EF=_tmp6EE.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp6EF);}
# 4350
;}
# 4353
Cyc_Core_free_rckey(_tmp6F1);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4356
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4362
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4364
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp6F2=_cycalloc(sizeof(*_tmp6F2));*_tmp6F2=pop_tables;_tmp6F2;});
Cyc_Toc_init();
{struct _RegionHandle _tmp6F3=_new_region("start");struct _RegionHandle*start=& _tmp6F3;_push_region(start);
({struct _RegionHandle*_tmpA23=start;struct Cyc_Toc_Env*_tmpA22=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpA23,_tmpA22,ds,1,0);});;_pop_region(start);}
# 4369
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}

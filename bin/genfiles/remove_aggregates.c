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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Position_Error;
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
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918 "absyn.h"
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 981
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_type(struct _tuple0*name);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1068
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1176
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Hashtable_Table;
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple0*Cyc_Toc_temp_var();extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_finish();struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 49 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*t);
# 26 "remove_aggregates.h"
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*decls);
# 50 "remove_aggregates.cyc"
static void Cyc_RemoveAggrs_massage_toplevel_aggr(struct Cyc_Absyn_Exp*e){
if(e == 0)
return;}
# 58
static struct Cyc_Absyn_Exp*Cyc_RemoveAggrs_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp2;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 20U){_LL1: _tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp2,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}
# 67
enum Cyc_RemoveAggrs_ExpContext{Cyc_RemoveAggrs_Initializer  = 0U,Cyc_RemoveAggrs_NewDest  = 1U,Cyc_RemoveAggrs_AggrField  = 2U,Cyc_RemoveAggrs_Other  = 3U};struct Cyc_RemoveAggrs_Env{enum Cyc_RemoveAggrs_ExpContext ctxt;struct Cyc_Absyn_Exp*dest;};
# 78
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_other_env={Cyc_RemoveAggrs_Other,0};
# 80
enum Cyc_RemoveAggrs_ExpResult{Cyc_RemoveAggrs_WasArray  = 0U,Cyc_RemoveAggrs_OtherRes  = 1U};struct Cyc_RemoveAggrs_Result{enum Cyc_RemoveAggrs_ExpResult res;};
# 89
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env,struct Cyc_Absyn_Exp*);
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*s);
# 92
static void Cyc_RemoveAggrs_noarray_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
struct Cyc_RemoveAggrs_Result _tmp3=Cyc_RemoveAggrs_remove_aggrs_exp(env,e);struct Cyc_RemoveAggrs_Result _tmp4=_tmp3;enum Cyc_RemoveAggrs_ExpResult _tmp8;_LL1: _tmp8=_tmp4.res;_LL2:;
if((int)_tmp8 != (int)1U)
({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp89;_tmp89.tag=0U,({
struct _dyneither_ptr _tmp92=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp89.f1=_tmp92;});_tmp89;});void*_tmp5[1U];_tmp5[0]=& _tmp7;({struct _dyneither_ptr _tmp93=({const char*_tmp6="remove_aggrs_exp -- unexpected array or comprehension: %s";_tag_dyneither(_tmp6,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp93,_tag_dyneither(_tmp5,sizeof(void*),1U));});});}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 99
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
enum Cyc_RemoveAggrs_ExpResult res=1U;
int did_assign=0;
# 103
{void*_tmp9=e->r;void*_tmpA=_tmp9;struct Cyc_Absyn_Stmt*_tmp59;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Exp*_tmp51;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_Absyn_Exp*_tmp4F;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_Absyn_Exp*_tmp4D;struct Cyc_Absyn_Exp*_tmp4C;struct Cyc_Absyn_Exp*_tmp4B;struct Cyc_Absyn_Exp*_tmp4A;struct Cyc_List_List*_tmp49;struct Cyc_Absyn_Exp*_tmp48;struct Cyc_List_List*_tmp47;struct Cyc_Absyn_Exp*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct Cyc_Absyn_Exp*_tmp43;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Vardecl*_tmp3E;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Exp*_tmp3C;int _tmp3B;struct Cyc_List_List*_tmp3A;struct _tuple0*_tmp39;struct Cyc_List_List*_tmp38;struct Cyc_Absyn_Aggrdecl*_tmp37;struct Cyc_Absyn_Exp*_tmp36;struct _dyneither_ptr _tmp35;switch(*((int*)_tmpA)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA)->f1).String_c).tag == 8){_LL1: _tmp35=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA)->f1).String_c).val;_LL2:
# 106
 if((int)env.ctxt == (int)Cyc_RemoveAggrs_AggrField  && Cyc_Tcutil_is_array_type((void*)_check_null(e->topt))){
struct Cyc_List_List*dles=0;
unsigned int _tmpB=_get_dyneither_size(_tmp35,sizeof(char));
{unsigned int i=0U;for(0;i < _tmpB;++ i){
struct Cyc_Absyn_Exp*_tmpC=Cyc_Absyn_char_exp(*((const char*)_check_dyneither_subscript(_tmp35,sizeof(char),(int)i)),0U);
_tmpC->topt=Cyc_Absyn_char_type;
dles=({struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _tuple10*_tmp94=({struct _tuple10*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->f1=0,_tmpD->f2=_tmpC;_tmpD;});_tmpE->hd=_tmp94;}),_tmpE->tl=dles;_tmpE;});}}
# 114
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
({void*_tmp95=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->tag=26U,_tmpF->f1=dles;_tmpF;});e->r=_tmp95;});
return Cyc_RemoveAggrs_remove_aggrs_exp(env,e);}else{
goto _LL0;}}else{_LL3: _LL4:
 goto _LL6;}case 17U: _LL5: _LL6:
 goto _LL8;case 19U: _LL7: _LL8:
 goto _LLA;case 33U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 40U: _LLD: _LLE:
 goto _LL10;case 1U: _LLF: _LL10:
 goto _LL0;case 16U: _LL11: _tmp36=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL12:
# 127
{void*_tmp10=e->annot;void*_tmp11=_tmp10;struct Cyc_Absyn_Exp*_tmp14;if(((struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*)_tmp11)->tag == Cyc_Toc_Dest){_LL42: _tmp14=((struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*)_tmp11)->f1;_LL43:
# 129
({struct Cyc_RemoveAggrs_Env _tmp96=({struct Cyc_RemoveAggrs_Env _tmp8A;_tmp8A.ctxt=Cyc_RemoveAggrs_NewDest,_tmp8A.dest=_tmp14;_tmp8A;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp96,_tmp36);});
*e=*_tmp36;
goto _LL41;}else{_LL44: _LL45:
({void*_tmp12=0U;({struct _dyneither_ptr _tmp97=({const char*_tmp13="removeAggrs: toc did not set a new-destination";_tag_dyneither(_tmp13,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp97,_tag_dyneither(_tmp12,sizeof(void*),0U));});});}_LL41:;}
# 134
goto _LL0;case 29U: _LL13: _tmp39=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp38=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA)->f3;_tmp37=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA)->f4;_LL14:
# 137
 did_assign=1;{
int do_stmt_exp=1;
if((int)env.ctxt == (int)Cyc_RemoveAggrs_AggrField  || (int)env.ctxt == (int)Cyc_RemoveAggrs_NewDest)
do_stmt_exp=0;{
struct Cyc_Absyn_Exp*dest;
struct _tuple0**v;
if(do_stmt_exp){
v=({struct _tuple0**_tmp15=_cycalloc(sizeof(*_tmp15));({struct _tuple0*_tmp98=Cyc_Toc_temp_var();*_tmp15=_tmp98;});_tmp15;});
dest=Cyc_Absyn_var_exp(*v,0U);}else{
# 147
v=0;
dest=(struct Cyc_Absyn_Exp*)_check_null(env.dest);
if((int)env.ctxt == (int)Cyc_RemoveAggrs_NewDest)
dest=Cyc_Absyn_deref_exp(dest,0U);}
# 153
{struct Cyc_List_List*_tmp16=_tmp38;for(0;_tmp16 != 0;_tmp16=_tmp16->tl){
struct _tuple10*_tmp17=(struct _tuple10*)_tmp16->hd;struct _tuple10*_tmp18=_tmp17;struct Cyc_List_List*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;_LL47: _tmp1C=_tmp18->f1;_tmp1B=_tmp18->f2;_LL48:;{
struct _dyneither_ptr*_tmp19=Cyc_Absyn_designatorlist_to_fieldname(_tmp1C);
struct Cyc_Absyn_Exp*_tmp1A=({struct Cyc_Absyn_Exp*_tmp99=Cyc_Absyn_copy_exp(dest);Cyc_RemoveAggrs_member_exp(_tmp99,_tmp19,0U);});
({struct Cyc_RemoveAggrs_Env _tmp9A=({struct Cyc_RemoveAggrs_Env _tmp8B;_tmp8B.ctxt=Cyc_RemoveAggrs_AggrField,_tmp8B.dest=_tmp1A;_tmp8B;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp9A,_tmp1B);});};}}
# 160
if(_tmp38 == 0)
({void*_tmp1D=0U;({struct _dyneither_ptr _tmp9B=({const char*_tmp1E="zero-field aggregate";_tag_dyneither(_tmp1E,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp9B,_tag_dyneither(_tmp1D,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple10*)_tmp38->hd)).f2;
for(_tmp38=_tmp38->tl;_tmp38 != 0;_tmp38=_tmp38->tl){
init_e=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple10*)_tmp38->hd)).f2,0U);}
if(do_stmt_exp){
void*(*_tmp1F)(struct _tuple0*name)=(unsigned int)_tmp37  && (int)_tmp37->kind == (int)Cyc_Absyn_UnionA?Cyc_Absyn_unionq_type: Cyc_Absyn_strctq;
({void*_tmp9F=(Cyc_Absyn_stmt_exp(({struct _tuple0*_tmp9E=*((struct _tuple0**)_check_null(v));void*_tmp9D=_tmp1F(_tmp39);Cyc_Absyn_declare_stmt(_tmp9E,_tmp9D,0,({
struct Cyc_Absyn_Stmt*_tmp9C=Cyc_Absyn_exp_stmt(init_e,0U);Cyc_Absyn_seq_stmt(_tmp9C,
Cyc_Absyn_exp_stmt(dest,0U),0U);}),0U);}),0U))->r;
# 167
e->r=_tmp9F;});}else{
# 171
e->r=init_e->r;
e->topt=0;}
# 174
goto _LL0;};};};case 26U: _LL15: _tmp3A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL16:
# 179
 res=0U;
if(_tmp3A == 0){
({struct Cyc_Absyn_Exp _tmpA0=*Cyc_Absyn_signed_int_exp(0,0U);*e=_tmpA0;});
goto _LL0;}
# 184
{enum Cyc_RemoveAggrs_ExpContext _tmp20=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _tmp21=_tmp20;if(_tmp21 == Cyc_RemoveAggrs_Other){_LL4A: _LL4B:
({void*_tmp22=0U;({struct _dyneither_ptr _tmpA1=({const char*_tmp23="remove-aggrs: Array_e in bad position";_tag_dyneither(_tmp23,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpA1,_tag_dyneither(_tmp22,sizeof(void*),0U));});});}else{_LL4C: _LL4D:
 goto _LL49;}_LL49:;}
# 188
did_assign=1;{
struct Cyc_Absyn_Exp*dest=(struct Cyc_Absyn_Exp*)_check_null(env.dest);
# 191
int i=0;
{struct Cyc_List_List*_tmp24=_tmp3A;for(0;_tmp24 != 0;(_tmp24=_tmp24->tl,++ i)){
# 195
struct _tuple10*_tmp25=(struct _tuple10*)_tmp24->hd;struct _tuple10*_tmp26=_tmp25;struct Cyc_Absyn_Exp*_tmp28;_LL4F: _tmp28=_tmp26->f2;_LL50:;{
struct Cyc_Absyn_Exp*_tmp27=({struct Cyc_Absyn_Exp*_tmpA2=Cyc_Absyn_copy_exp(dest);Cyc_Absyn_subscript_exp(_tmpA2,Cyc_Absyn_signed_int_exp(i,0U),0U);});
({struct Cyc_RemoveAggrs_Env _tmpA3=({struct Cyc_RemoveAggrs_Env _tmp8C;_tmp8C.ctxt=Cyc_RemoveAggrs_AggrField,_tmp8C.dest=_tmp27;_tmp8C;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmpA3,_tmp28);});};}}{
# 199
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple10*)_tmp3A->hd)).f2;
for(_tmp3A=_tmp3A->tl;_tmp3A != 0;_tmp3A=_tmp3A->tl){
init_e=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple10*)_tmp3A->hd)).f2,0U);}
e->r=init_e->r;
e->topt=0;
goto _LL0;};};case 27U: _LL17: _tmp3E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp3D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_tmp3C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA)->f3;_tmp3B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA)->f4;_LL18:
# 208
 did_assign=1;
res=0U;
{enum Cyc_RemoveAggrs_ExpContext _tmp29=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _tmp2A=_tmp29;if(_tmp2A == Cyc_RemoveAggrs_Other){_LL52: _LL53:
({void*_tmp2B=0U;({struct _dyneither_ptr _tmpA4=({const char*_tmp2C="remove-aggrs: comprehension in bad position";_tag_dyneither(_tmp2C,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpA4,_tag_dyneither(_tmp2B,sizeof(void*),0U));});});}else{_LL54: _LL55:
 goto _LL51;}_LL51:;}{
# 214
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*i=_tmp3E->name;
struct Cyc_Absyn_Exp*ea=({struct Cyc_Absyn_Exp*_tmpA5=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_assign_exp(_tmpA5,Cyc_Absyn_signed_int_exp(0,0U),0U);});
struct Cyc_Absyn_Exp*eb=({struct Cyc_Absyn_Exp*_tmpA6=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_lt_exp(_tmpA6,Cyc_Absyn_var_exp(max,0U),0U);});
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_pre_inc_exp(Cyc_Absyn_var_exp(i,0U),0U);
struct Cyc_Absyn_Exp*lval=({struct Cyc_Absyn_Exp*_tmpA7=(struct Cyc_Absyn_Exp*)_check_null(env.dest);Cyc_Absyn_subscript_exp(_tmpA7,Cyc_Absyn_var_exp(i,0U),0U);});
({struct Cyc_RemoveAggrs_Env _tmpA9=({struct Cyc_RemoveAggrs_Env _tmp8D;_tmp8D.ctxt=Cyc_RemoveAggrs_AggrField,({struct Cyc_Absyn_Exp*_tmpA8=Cyc_Absyn_copy_exp(lval);_tmp8D.dest=_tmpA8;});_tmp8D;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmpA9,_tmp3C);});{
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_tmpAC=ea;struct Cyc_Absyn_Exp*_tmpAB=eb;struct Cyc_Absyn_Exp*_tmpAA=ec;Cyc_Absyn_for_stmt(_tmpAC,_tmpAB,_tmpAA,Cyc_Absyn_exp_stmt(_tmp3C,0U),0U);});
if(_tmp3B){
# 229
struct Cyc_Absyn_Exp*ex=({struct Cyc_Absyn_Exp*_tmpAE=({struct Cyc_Absyn_Exp*_tmpAD=Cyc_Absyn_copy_exp(env.dest);Cyc_Absyn_subscript_exp(_tmpAD,
Cyc_Absyn_var_exp(max,0U),0U);});
# 229
Cyc_Absyn_assign_exp(_tmpAE,
# 231
Cyc_Absyn_signed_int_exp(0,0U),0U);});
s=({struct Cyc_Absyn_Stmt*_tmpAF=s;Cyc_Absyn_seq_stmt(_tmpAF,Cyc_Absyn_exp_stmt(ex,0U),0U);});}
# 235
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp3D);
({void*_tmpB4=(Cyc_Absyn_stmt_exp(({struct Cyc_Absyn_Stmt*_tmpB3=({struct _tuple0*_tmpB2=max;void*_tmpB1=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmpB0=_tmp3D;Cyc_Absyn_declare_stmt(_tmpB2,_tmpB1,_tmpB0,
Cyc_Absyn_declare_stmt(i,Cyc_Absyn_uint_type,0,s,0U),0U);});
# 236
Cyc_Absyn_seq_stmt(_tmpB3,
# 238
Cyc_Absyn_exp_stmt(Cyc_Absyn_signed_int_exp(0,0U),0U),0U);}),0U))->r;
# 236
e->r=_tmpB4;});
# 239
e->topt=0;
goto _LL0;};};case 28U: _LL19: _tmp3F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL1A:
# 243
 did_assign=1;
res=0U;
{enum Cyc_RemoveAggrs_ExpContext _tmp2D=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _tmp2E=_tmp2D;if(_tmp2E == Cyc_RemoveAggrs_Other){_LL57: _LL58:
({void*_tmp2F=0U;({struct _dyneither_ptr _tmpB5=({const char*_tmp30="remove-aggrs: no-init-comp in bad position";_tag_dyneither(_tmp30,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpB5,_tag_dyneither(_tmp2F,sizeof(void*),0U));});});}else{_LL59: _LL5A:
 goto _LL56;}_LL56:;}
# 250
*e=*_tmp3F;
goto _LL0;case 4U: _LL1B: _tmp41=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp40=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA)->f3;_LL1C:
# 254
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp41);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp40);
goto _LL0;case 9U: _LL1D: _tmp43=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp42=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL1E:
# 258
 did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp43);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env,_tmp42);
goto _LL0;case 6U: _LL1F: _tmp46=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp45=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_tmp44=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA)->f3;_LL20:
# 263
 did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp46);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env,_tmp45);{
struct Cyc_RemoveAggrs_Env _tmp31=({struct Cyc_RemoveAggrs_Env _tmp8E;_tmp8E.ctxt=env.ctxt,env.dest == 0?_tmp8E.dest=0:({struct Cyc_Absyn_Exp*_tmpB6=Cyc_Absyn_copy_exp(env.dest);_tmp8E.dest=_tmpB6;});_tmp8E;});
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_tmp31,_tmp44);
goto _LL0;};case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA)->f3 == 0){_LL21: _tmp48=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp47=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL22:
# 271
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp48);
_tmp49=_tmp47;goto _LL24;}else{goto _LL3F;}case 3U: _LL23: _tmp49=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL24:
# 274
 for(0;_tmp49 != 0;_tmp49=_tmp49->tl){
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,(struct Cyc_Absyn_Exp*)_tmp49->hd);}
goto _LL0;case 23U: _LL25: _tmp4B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp4A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL26:
# 278
 _tmp4D=_tmp4B;_tmp4C=_tmp4A;goto _LL28;case 7U: _LL27: _tmp4D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp4C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL28:
 _tmp4F=_tmp4D;_tmp4E=_tmp4C;goto _LL2A;case 8U: _LL29: _tmp4F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp4E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL2A:
# 281
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp4F);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp4E);
goto _LL0;case 13U: _LL2B: _tmp50=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL2C:
# 286
 _tmp51=_tmp50;goto _LL2E;case 12U: _LL2D: _tmp51=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL2E:
 _tmp52=_tmp51;goto _LL30;case 14U: _LL2F: _tmp52=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL30:
 _tmp53=_tmp52;goto _LL32;case 15U: _LL31: _tmp53=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL32:
 _tmp54=_tmp53;goto _LL34;case 20U: _LL33: _tmp54=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL34:
 _tmp55=_tmp54;goto _LL36;case 21U: _LL35: _tmp55=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL36:
 _tmp56=_tmp55;goto _LL38;case 22U: _LL37: _tmp56=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL38:
 _tmp57=_tmp56;goto _LL3A;case 18U: _LL39: _tmp57=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL3A:
 _tmp58=_tmp57;goto _LL3C;case 5U: _LL3B: _tmp58=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL3C:
# 295
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp58);
goto _LL0;case 37U: _LL3D: _tmp59=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL3E:
# 299
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp59);
goto _LL0;default: _LL3F: _LL40:
# 302
({struct Cyc_String_pa_PrintArg_struct _tmp34=({struct Cyc_String_pa_PrintArg_struct _tmp8F;_tmp8F.tag=0U,({
struct _dyneither_ptr _tmpB7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp8F.f1=_tmpB7;});_tmp8F;});void*_tmp32[1U];_tmp32[0]=& _tmp34;({struct _dyneither_ptr _tmpB8=({const char*_tmp33="bad exp after translation to C: %s";_tag_dyneither(_tmp33,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpB8,_tag_dyneither(_tmp32,sizeof(void*),1U));});});}_LL0:;}
# 305
if((int)env.ctxt == (int)Cyc_RemoveAggrs_AggrField  && !did_assign)
({void*_tmpBA=({struct Cyc_Absyn_Exp*_tmpB9=(struct Cyc_Absyn_Exp*)_check_null(env.dest);Cyc_Absyn_assign_exp(_tmpB9,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_tmpBA;});
if((int)env.ctxt == (int)Cyc_RemoveAggrs_NewDest  && !did_assign)
({void*_tmpBC=({struct Cyc_Absyn_Exp*_tmpBB=Cyc_Absyn_deref_exp((struct Cyc_Absyn_Exp*)_check_null(env.dest),0U);Cyc_Absyn_assign_exp(_tmpBB,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_tmpBC;});
return({struct Cyc_RemoveAggrs_Result _tmp90;_tmp90.res=res;_tmp90;});}
# 316
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp5A=s->r;void*_tmp5B=_tmp5A;struct Cyc_Absyn_Decl*_tmp7A;struct Cyc_Absyn_Stmt*_tmp79;struct Cyc_Absyn_Exp*_tmp78;struct Cyc_List_List*_tmp77;struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_Absyn_Stmt*_tmp73;struct Cyc_Absyn_Exp*_tmp72;struct Cyc_Absyn_Stmt*_tmp71;struct Cyc_Absyn_Stmt*_tmp70;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_Absyn_Stmt*_tmp6D;struct Cyc_Absyn_Stmt*_tmp6C;struct Cyc_Absyn_Stmt*_tmp6B;struct Cyc_Absyn_Stmt*_tmp6A;struct Cyc_Absyn_Exp*_tmp69;struct Cyc_Absyn_Exp*_tmp68;struct Cyc_Absyn_Stmt*_tmp67;switch(*((int*)_tmp5B)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 goto _LL8;case 8U: _LL7: _LL8:
 return;case 13U: _LL9: _tmp67=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5B)->f2;_LLA:
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp67);return;case 3U: _LLB: _tmp68=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5B)->f1;_LLC:
# 324
 if(_tmp68 == 0)
return;
_tmp69=_tmp68;goto _LLE;case 1U: _LLD: _tmp69=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5B)->f1;_LLE:
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp69);return;case 2U: _LLF: _tmp6B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5B)->f1;_tmp6A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5B)->f2;_LL10:
# 329
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp6B);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp6A);
return;case 4U: _LL11: _tmp6E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5B)->f1;_tmp6D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5B)->f2;_tmp6C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5B)->f3;_LL12:
# 333
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp6E);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp6D);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp6C);
return;case 14U: _LL13: _tmp70=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5B)->f1;_tmp6F=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5B)->f2).f1;_LL14:
 _tmp72=_tmp6F;_tmp71=_tmp70;goto _LL16;case 5U: _LL15: _tmp72=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5B)->f1).f1;_tmp71=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5B)->f2;_LL16:
# 339
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp72);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp71);
return;case 9U: _LL17: _tmp76=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5B)->f1;_tmp75=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5B)->f2).f1;_tmp74=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5B)->f3).f1;_tmp73=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5B)->f4;_LL18:
# 343
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp76);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp75);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp74);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp73);
return;case 10U: _LL19: _tmp78=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5B)->f1;_tmp77=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5B)->f2;_LL1A:
# 351
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp78);
for(0;_tmp77 != 0;_tmp77=_tmp77->tl){
Cyc_RemoveAggrs_remove_aggrs_stmt(((struct Cyc_Absyn_Switch_clause*)_tmp77->hd)->body);}
return;case 12U: _LL1B: _tmp7A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5B)->f1;_tmp79=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5B)->f2;_LL1C:
# 356
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp79);
{void*_tmp5C=_tmp7A->r;void*_tmp5D=_tmp5C;struct Cyc_Absyn_Fndecl*_tmp64;struct Cyc_Absyn_Vardecl*_tmp63;switch(*((int*)_tmp5D)){case 0U: _LL20: _tmp63=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5D)->f1;_LL21:
# 359
 if((int)_tmp63->sc == (int)Cyc_Absyn_Static){
Cyc_RemoveAggrs_massage_toplevel_aggr(_tmp63->initializer);
goto _LL1F;}
# 370
if(_tmp63->initializer != 0){
struct Cyc_RemoveAggrs_Result _tmp5E=({struct Cyc_RemoveAggrs_Env _tmpBE=({struct Cyc_RemoveAggrs_Env _tmp91;_tmp91.ctxt=Cyc_RemoveAggrs_Initializer,({struct Cyc_Absyn_Exp*_tmpBD=Cyc_Absyn_var_exp(_tmp63->name,0U);_tmp91.dest=_tmpBD;});_tmp91;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmpBE,(struct Cyc_Absyn_Exp*)_check_null(_tmp63->initializer));});struct Cyc_RemoveAggrs_Result _tmp5F=_tmp5E;enum Cyc_RemoveAggrs_ExpResult _tmp60;_LL27: _tmp60=_tmp5F.res;_LL28:;
# 373
if((int)_tmp60 == (int)0U){
({void*_tmpC1=({struct Cyc_Absyn_Decl*_tmpC0=_tmp7A;Cyc_Absyn_decl_stmt(_tmpC0,({struct Cyc_Absyn_Stmt*_tmpBF=Cyc_Absyn_exp_stmt((struct Cyc_Absyn_Exp*)_check_null(_tmp63->initializer),0U);Cyc_Absyn_seq_stmt(_tmpBF,_tmp79,0U);}),0U);})->r;s->r=_tmpC1;});
# 376
_tmp63->initializer=0;}}
# 379
goto _LL1F;case 1U: _LL22: _tmp64=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5D)->f1;_LL23:
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp64->body);goto _LL1F;default: _LL24: _LL25:
({void*_tmp61=0U;({struct _dyneither_ptr _tmpC2=({const char*_tmp62="bad local declaration after xlation to C";_tag_dyneither(_tmp62,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpC2,_tag_dyneither(_tmp61,sizeof(void*),0U));});});}_LL1F:;}
# 383
return;default: _LL1D: _LL1E:
({void*_tmp65=0U;({struct _dyneither_ptr _tmpC3=({const char*_tmp66="bad stmt after xlation to C";_tag_dyneither(_tmp66,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpC3,_tag_dyneither(_tmp65,sizeof(void*),0U));});});}_LL0:;}
# 388
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*ds){
{struct Cyc_List_List*_tmp7B=ds;for(0;_tmp7B != 0;_tmp7B=_tmp7B->tl){
void*_tmp7C=((struct Cyc_Absyn_Decl*)_tmp7B->hd)->r;void*_tmp7D=_tmp7C;struct Cyc_List_List*_tmp88;struct Cyc_List_List*_tmp87;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp85;struct Cyc_Absyn_Aggrdecl*_tmp84;struct Cyc_Absyn_Vardecl*_tmp83;struct Cyc_Absyn_Fndecl*_tmp82;switch(*((int*)_tmp7D)){case 1U: _LL1: _tmp82=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7D)->f1;_LL2:
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp82->body);goto _LL0;case 0U: _LL3: _tmp83=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D)->f1;_LL4:
 Cyc_RemoveAggrs_massage_toplevel_aggr(_tmp83->initializer);goto _LL0;case 5U: _LL5: _tmp84=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp7D)->f1;_LL6:
 goto _LL8;case 7U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 goto _LL0;case 4U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 2U: _LLF: _LL10:
 goto _LL12;case 3U: _LL11: _LL12:
({void*_tmp7E=0U;({struct _dyneither_ptr _tmpC4=({const char*_tmp7F="Cyclone decl after xlation to C";_tag_dyneither(_tmp7F,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpC4,_tag_dyneither(_tmp7E,sizeof(void*),0U));});});case 9U: _LL13: _tmp85=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp7D)->f2;_LL14:
 _tmp86=_tmp85;goto _LL16;case 10U: _LL15: _tmp86=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp7D)->f2;_LL16:
 _tmp87=_tmp86;goto _LL18;case 11U: _LL17: _tmp87=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp7D)->f1;_LL18:
 _tmp88=_tmp87;goto _LL1A;case 12U: _LL19: _tmp88=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp7D)->f1;_LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 408
({void*_tmp80=0U;({struct _dyneither_ptr _tmpC5=({const char*_tmp81="nested translation unit after xlation to C";_tag_dyneither(_tmp81,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpC5,_tag_dyneither(_tmp80,sizeof(void*),0U));});});}_LL0:;}}
# 410
return ds;}

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
#define _check_null(ptr) (ptr ? : (void*)_throw_null())
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index)\
   ((char *)ptr) + (elt_sz)*(index))
#define _check_known_subscript_notnull(bound,index) (index)

#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })

#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })

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
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
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

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct
 _dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Allocation */

extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

/* FIX?  Not sure if we want to pass filename and lineno in here... */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n)            (GC_malloc(n)          ? : _throw_badalloc())
#define _cycalloc_atomic(n)     (GC_malloc_atomic(n)   ? : _throw_badalloc())
#define _cyccalloc(n,s)         (GC_calloc(n,s)        ? : _throw_badalloc())
#define _cyccalloc_atomic(n,s)  (GC_calloc_atomic(n,s) ? : _throw_badalloc())
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
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172 "list.h"
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 101
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 210
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 212
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 332
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 726 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 942
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 965
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 991
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1103
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1112
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 25 "warn.h"
void Cyc_Warn_vwarn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 27
void Cyc_Warn_warn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29
void Cyc_Warn_flush_warnings();
# 31
void Cyc_Warn_verr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 36
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 38
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29 "binding.h"
extern int Cyc_Binding_warn_override;
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);char Cyc_Binding_BindingError[13U]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 44 "binding.cyc"
int Cyc_Binding_warn_override=0;struct Cyc_Binding_NSCtxt;
# 58
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);
return({struct Cyc_Binding_NSCtxt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->curr_ns=0,({
struct Cyc_List_List*_tmp2B4=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));({void*_tmp2B3=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=0U,_tmp1->f1=0;_tmp1;});_tmp2->hd=_tmp2B3;}),_tmp2->tl=0;_tmp2;});_tmp3->availables=_tmp2B4;}),({
struct Cyc_Dict_Dict _tmp2B2=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,_tmp0);_tmp3->ns_data=_tmp2B2;});_tmp3;});}
# 64
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp4=abs_ns;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;switch((_tmp4.Abs_n).tag){case 3U: _LL1: _tmp7=(_tmp4.C_n).val;_LL2:
 _tmp8=_tmp7;goto _LL4;case 2U: _LL3: _tmp8=(_tmp4.Abs_n).val;_LL4:
 return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp8);default: _LL5: _LL6:
({void*_tmp5=0U;({struct _dyneither_ptr _tmp2B5=({const char*_tmp6="Binding:get_ns_data: relative ns";_tag_dyneither(_tmp6,sizeof(char),33U);});Cyc_Warn_impos(_tmp2B5,_tag_dyneither(_tmp5,sizeof(void*),0U));});});}_LL0:;}
# 82 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 85
struct Cyc_List_List*_tmp9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp9))
return _tmp9;
{struct Cyc_List_List*_tmpA=ctxt->availables;for(0;_tmpA != 0;_tmpA=_tmpA->tl){
void*_tmpB=(void*)_tmpA->hd;void*_tmpC=_tmpB;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmpE;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->tag == 1U){_LL1: _tmpE=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->f1;_LL2:
 _tmpF=_tmpE;goto _LL4;}else{_LL3: _tmpF=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpC)->f1;_LL4: {
# 92
struct Cyc_List_List*_tmpD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpF,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpD))
return _tmpD;
goto _LL0;}}_LL0:;}}
# 97
({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp2A0;_tmp2A0.tag=0U,({struct _dyneither_ptr _tmp2B7=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_List_List*_tmp2B6=rel_ns;Cyc_str_sepstr(_tmp2B6,({const char*_tmp13="::";_tag_dyneither(_tmp13,sizeof(char),3U);}));}));_tmp2A0.f1=_tmp2B7;});_tmp2A0;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({unsigned int _tmp2B9=loc;struct _dyneither_ptr _tmp2B8=({const char*_tmp11="namespace %s not found";_tag_dyneither(_tmp11,sizeof(char),23U);});Cyc_Warn_err(_tmp2B9,_tmp2B8,_tag_dyneither(_tmp10,sizeof(void*),1U));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=Cyc_Binding_BindingError;_tmp14;}));}
# 102
void*Cyc_Binding_resolve_lookup(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _dyneither_ptr*)){
struct _tuple0*_tmp15=qv;union Cyc_Absyn_Nmspace _tmp3E;struct _dyneither_ptr*_tmp3D;_LL1: _tmp3E=_tmp15->f1;_tmp3D=_tmp15->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp16=_tmp3E;struct Cyc_List_List*_tmp3C;struct Cyc_List_List*_tmp3B;struct Cyc_List_List*_tmp3A;switch((_tmp16.Abs_n).tag){case 1U: if((_tmp16.Rel_n).val == 0){_LL4: _LL5:
# 106
{struct _handler_cons _tmp17;_push_handler(& _tmp17);{int _tmp19=0;if(setjmp(_tmp17.handler))_tmp19=1;if(!_tmp19){{void*_tmp1A=({void*(*_tmp2BB)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2BA=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns);_tmp2BB(_tmp2BA,_tmp3D);});_npop_handler(0U);return _tmp1A;};_pop_handler();}else{void*_tmp18=(void*)_exn_thrown;void*_tmp1B=_tmp18;void*_tmp1C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B)->tag == Cyc_Dict_Absent){_LLF: _LL10:
 goto _LLE;}else{_LL11: _tmp1C=_tmp1B;_LL12:(int)_rethrow(_tmp1C);}_LLE:;}};}
{struct Cyc_List_List*_tmp1D=ctxt->availables;for(0;_tmp1D != 0;_tmp1D=_tmp1D->tl){
void*_tmp1E=(void*)_tmp1D->hd;void*_tmp1F=_tmp1E;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp26;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp1F)->tag == 1U){_LL14: _tmp26=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp1F)->f1;_LL15:
 _tmp27=_tmp26;goto _LL17;}else{_LL16: _tmp27=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp1F)->f1;_LL17:
# 112
{struct _handler_cons _tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(_tmp20.handler))_tmp22=1;if(!_tmp22){{void*_tmp23=({void*(*_tmp2BD)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2BC=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp27);_tmp2BD(_tmp2BC,_tmp3D);});_npop_handler(0U);return _tmp23;};_pop_handler();}else{void*_tmp21=(void*)_exn_thrown;void*_tmp24=_tmp21;void*_tmp25;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp24)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
 goto _LL18;}else{_LL1B: _tmp25=_tmp24;_LL1C:(int)_rethrow(_tmp25);}_LL18:;}};}
goto _LL13;}_LL13:;}}
# 116
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=Cyc_Binding_BindingError;_tmp28;}));}else{_LL6: _tmp3A=(_tmp16.Rel_n).val;_LL7: {
# 118
struct _handler_cons _tmp29;_push_handler(& _tmp29);{int _tmp2B=0;if(setjmp(_tmp29.handler))_tmp2B=1;if(!_tmp2B){
{struct Cyc_List_List*_tmp2C=Cyc_Binding_resolve_rel_ns(loc,ctxt,_tmp3A);
void*_tmp2D=({void*(*_tmp2BF)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2BE=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp2C);_tmp2BF(_tmp2BE,_tmp3D);});_npop_handler(0U);return _tmp2D;}
# 119
;_pop_handler();}else{void*_tmp2A=(void*)_exn_thrown;void*_tmp2E=_tmp2A;void*_tmp30;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2E)->tag == Cyc_Dict_Absent){_LL1E: _LL1F:
# 121
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=Cyc_Binding_BindingError;_tmp2F;}));}else{_LL20: _tmp30=_tmp2E;_LL21:(int)_rethrow(_tmp30);}_LL1D:;}};}}case 3U: _LL8: _tmp3B=(_tmp16.C_n).val;_LL9:
 _tmp3C=_tmp3B;goto _LLB;case 2U: _LLA: _tmp3C=(_tmp16.Abs_n).val;_LLB: {
# 124
struct _handler_cons _tmp31;_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))_tmp33=1;if(!_tmp33){{void*_tmp34=({void*(*_tmp2C1)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2C0=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp3C);_tmp2C1(_tmp2C0,_tmp3D);});_npop_handler(0U);return _tmp34;};_pop_handler();}else{void*_tmp32=(void*)_exn_thrown;void*_tmp35=_tmp32;void*_tmp37;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp35)->tag == Cyc_Dict_Absent){_LL23: _LL24:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->tag=Cyc_Binding_BindingError;_tmp36;}));}else{_LL25: _tmp37=_tmp35;_LL26:(int)_rethrow(_tmp37);}_LL22:;}};}default: _LLC: _LLD:
({void*_tmp38=0U;({struct _dyneither_ptr _tmp2C2=({const char*_tmp39="lookup local in global";_tag_dyneither(_tmp39,sizeof(char),23U);});Cyc_Warn_impos(_tmp2C2,_tag_dyneither(_tmp38,sizeof(void*),0U));});});}_LL3:;};}
# 130
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp3F=ctxt->curr_ns;
struct Cyc_List_List*_tmp40=({struct Cyc_List_List*_tmp2C3=_tmp3F;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2C3,({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->hd=subname,_tmp43->tl=0;_tmp43;}));});
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp40))
({struct Cyc_Dict_Dict _tmp2C6=({struct Cyc_Dict_Dict _tmp2C5=ctxt->ns_data;struct Cyc_List_List*_tmp2C4=_tmp40;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(_tmp2C5,_tmp2C4,mkdata(env));});ctxt->ns_data=_tmp2C6;});
ctxt->curr_ns=_tmp40;
({struct Cyc_List_List*_tmp2C8=({struct Cyc_List_List*_tmp42=_cycalloc(sizeof(*_tmp42));({void*_tmp2C7=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->tag=0U,_tmp41->f1=_tmp40;_tmp41;});_tmp42->hd=_tmp2C7;}),_tmp42->tl=ctxt->availables;_tmp42;});ctxt->availables=_tmp2C8;});}
# 138
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp44=0U;({struct _dyneither_ptr _tmp2C9=({const char*_tmp45="leaving topmost namespace";_tag_dyneither(_tmp45,sizeof(char),26U);});Cyc_Warn_impos(_tmp2C9,_tag_dyneither(_tmp44,sizeof(void*),0U));});});{
void*_tmp46=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp47=_tmp46;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp47)->tag == 0U){_LL1: _LL2:
# 143
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
({struct Cyc_List_List*_tmp2CA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);ctxt->curr_ns=_tmp2CA;});
goto _LL0;}else{_LL3: _LL4:
# 147
({void*_tmp48=0U;({struct _dyneither_ptr _tmp2CB=({const char*_tmp49="leaving using as namespace";_tag_dyneither(_tmp49,sizeof(char),27U);});Cyc_Warn_impos(_tmp2CB,_tag_dyneither(_tmp48,sizeof(void*),0U));});});}_LL0:;};}
# 150
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp4A=usename;union Cyc_Absyn_Nmspace _tmp58;struct _dyneither_ptr*_tmp57;_LL1: _tmp58=_tmp4A->f1;_tmp57=_tmp4A->f2;_LL2:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp4B=_tmp58;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp53;switch((_tmp4B.Loc_n).tag){case 1U: _LL4: _tmp53=(_tmp4B.Rel_n).val;_LL5:
# 155
({struct Cyc_List_List*_tmp2CF=({unsigned int _tmp2CE=loc;struct Cyc_Binding_NSCtxt*_tmp2CD=ctxt;Cyc_Binding_resolve_rel_ns(_tmp2CE,_tmp2CD,({struct Cyc_List_List*_tmp2CC=_tmp53;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2CC,({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=_tmp57,_tmp4C->tl=0;_tmp4C;}));}));});ns=_tmp2CF;});{
struct Cyc_List_List*_tmp4D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);
({union Cyc_Absyn_Nmspace _tmp2D0=Cyc_Absyn_Abs_n(_tmp4D,0);(*usename).f1=_tmp2D0;});
goto _LL3;};case 2U: _LL6: _tmp54=(_tmp4B.Abs_n).val;_LL7:
# 160
({struct Cyc_List_List*_tmp2D2=({struct Cyc_List_List*_tmp2D1=_tmp54;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2D1,({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->hd=_tmp57,_tmp4E->tl=0;_tmp4E;}));});ns=_tmp2D2;});
goto _LL3;case 4U: _LL8: _LL9:
({void*_tmp4F=0U;({struct _dyneither_ptr _tmp2D3=({const char*_tmp50="enter_using, Loc_n";_tag_dyneither(_tmp50,sizeof(char),19U);});Cyc_Warn_impos(_tmp2D3,_tag_dyneither(_tmp4F,sizeof(void*),0U));});});default: _LLA: _LLB:
({void*_tmp51=0U;({struct _dyneither_ptr _tmp2D4=({const char*_tmp52="enter_using, C_n";_tag_dyneither(_tmp52,sizeof(char),17U);});Cyc_Warn_impos(_tmp2D4,_tag_dyneither(_tmp51,sizeof(void*),0U));});});}_LL3:;}
# 165
({struct Cyc_List_List*_tmp2D6=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));({void*_tmp2D5=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=1U,_tmp55->f1=ns;_tmp55;});_tmp56->hd=_tmp2D5;}),_tmp56->tl=ctxt->availables;_tmp56;});ctxt->availables=_tmp2D6;});
return ns;};}
# 168
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp59=0U;({struct _dyneither_ptr _tmp2D7=({const char*_tmp5A="leaving topmost namespace as a using";_tag_dyneither(_tmp5A,sizeof(char),37U);});Cyc_Warn_impos(_tmp2D7,_tag_dyneither(_tmp59,sizeof(void*),0U));});});{
void*_tmp5B=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp5C=_tmp5B;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp5C)->tag == 1U){_LL1: _LL2:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL0;}else{_LL3: _LL4:
({void*_tmp5D=0U;({struct _dyneither_ptr _tmp2D8=({const char*_tmp5E="leaving namespace as using";_tag_dyneither(_tmp5E,sizeof(char),27U);});Cyc_Warn_impos(_tmp2D8,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});}_LL0:;};}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 212 "binding.cyc"
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 215
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 218
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp5F=_cycalloc(sizeof(*_tmp5F));({struct Cyc_Dict_Dict _tmp2DD=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->aggrdecls=_tmp2DD;}),({
struct Cyc_Dict_Dict _tmp2DC=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->datatypedecls=_tmp2DC;}),({
struct Cyc_Dict_Dict _tmp2DB=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->enumdecls=_tmp2DB;}),({
struct Cyc_Dict_Dict _tmp2DA=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->typedefs=_tmp2DA;}),({
struct Cyc_Dict_Dict _tmp2D9=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->ordinaries=_tmp2D9;});_tmp5F;});}
# 226
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->aggrdecls,v);}
# 229
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->datatypedecls,v);}
# 232
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->enumdecls,v);}
# 235
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->typedefs,v);}
# 238
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->ordinaries,v);}
# 241
static void*Cyc_Binding_lookup_ordinary(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){
struct _tuple0*_tmp60=qv;union Cyc_Absyn_Nmspace _tmp64;struct _dyneither_ptr*_tmp63;_LL1: _tmp64=_tmp60->f1;_tmp63=_tmp60->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp61=_tmp64;switch((_tmp61.Rel_n).tag){case 4U: _LL4: _LL5:
# 245
 if(Cyc_Binding_at_toplevel(env) || !((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=Cyc_Binding_BindingError;_tmp62;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63);case 1U: if((_tmp61.Rel_n).val == 0){_LL6: _LL7:
# 249
 if(!Cyc_Binding_at_toplevel(env) && ((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63))
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63);
goto _LL9;}else{goto _LL8;}default: _LL8: _LL9:
# 253
 return((void*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL3:;};}
# 257
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*,struct Cyc_Absyn_Decl*);
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*);
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*,struct Cyc_Absyn_Pat*);
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);
void Cyc_Binding_resolve_type(unsigned int,struct Cyc_Binding_Env*,void*);
void Cyc_Binding_resolve_rgnpo(unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_aggrfields(unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_function_stuff(unsigned int,struct Cyc_Binding_Env*,void*,void*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 269
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int,unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*);
# 274
void Cyc_Binding_absolutize_decl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp65=(*qv).f1;union Cyc_Absyn_Nmspace _tmp66=_tmp65;switch((_tmp66.Abs_n).tag){case 1U: if((_tmp66.Rel_n).val == 0){_LL1: _LL2:
# 277
 if(Cyc_Binding_at_toplevel(env))
({union Cyc_Absyn_Nmspace _tmp2DF=({struct Cyc_List_List*_tmp2DE=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp2DE,Cyc_Binding_in_cinclude(env) || sc == Cyc_Absyn_ExternC);});(*qv).f1=_tmp2DF;});else{
# 280
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL0;}else{goto _LL7;}case 4U: _LL3: _LL4:
 goto _LL0;case 2U: _LL5: _LL6:
# 284
 if(!Cyc_Binding_at_toplevel(env))
goto _LL8;
goto _LL0;default: _LL7: _LL8:
# 288
({struct Cyc_String_pa_PrintArg_struct _tmp69=({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0U,({
struct _dyneither_ptr _tmp2E0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(qv));_tmp2A1.f1=_tmp2E0;});_tmp2A1;});void*_tmp67[1U];_tmp67[0]=& _tmp69;({unsigned int _tmp2E2=loc;struct _dyneither_ptr _tmp2E1=({const char*_tmp68="qualified names in declarations unimplemented (%s)";_tag_dyneither(_tmp68,sizeof(char),51U);});Cyc_Warn_err(_tmp2E2,_tmp2E1,_tag_dyneither(_tmp67,sizeof(void*),1U));});});}_LL0:;}
# 295
void Cyc_Binding_absolutize_topdecl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp6A=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=_tmp6A;}
# 302
void Cyc_Binding_check_warn_override(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _dyneither_ptr hides_what;
struct _handler_cons _tmp6B;_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(_tmp6B.handler))_tmp6D=1;if(!_tmp6D){
{void*_tmp6E=Cyc_Binding_lookup_ordinary(loc,env,q);void*_tmp6F=_tmp6E;void*_tmp7B;switch(*((int*)_tmp6F)){case 0U: _LL1: _tmp7B=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp6F)->f1;_LL2:
# 307
 if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0U);return;}
{void*_tmp70=_tmp7B;switch(*((int*)_tmp70)){case 1U: _LLC: _LLD:
({struct _dyneither_ptr _tmp2E3=({const char*_tmp71="global variable";_tag_dyneither(_tmp71,sizeof(char),16U);});hides_what=_tmp2E3;});goto _LLB;case 4U: _LLE: _LLF:
({struct _dyneither_ptr _tmp2E4=({const char*_tmp72="local variable";_tag_dyneither(_tmp72,sizeof(char),15U);});hides_what=_tmp2E4;});goto _LLB;case 3U: _LL10: _LL11:
({struct _dyneither_ptr _tmp2E5=({const char*_tmp73="parameter";_tag_dyneither(_tmp73,sizeof(char),10U);});hides_what=_tmp2E5;});goto _LLB;case 5U: _LL12: _LL13:
({struct _dyneither_ptr _tmp2E6=({const char*_tmp74="pattern variable";_tag_dyneither(_tmp74,sizeof(char),17U);});hides_what=_tmp2E6;});goto _LLB;case 2U: _LL14: _LL15:
({struct _dyneither_ptr _tmp2E7=({const char*_tmp75="function";_tag_dyneither(_tmp75,sizeof(char),9U);});hides_what=_tmp2E7;});goto _LLB;default: _LL16: _LL17:
({void*_tmp76=0U;({struct _dyneither_ptr _tmp2E8=({const char*_tmp77="shadowing free variable!";_tag_dyneither(_tmp77,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2E8,_tag_dyneither(_tmp76,sizeof(void*),0U));});});}_LLB:;}
# 317
goto _LL0;case 1U: _LL3: _LL4:
({struct _dyneither_ptr _tmp2E9=({const char*_tmp78="struct constructor";_tag_dyneither(_tmp78,sizeof(char),19U);});hides_what=_tmp2E9;});goto _LL0;case 2U: _LL5: _LL6:
({struct _dyneither_ptr _tmp2EA=({const char*_tmp79="datatype constructor";_tag_dyneither(_tmp79,sizeof(char),21U);});hides_what=_tmp2EA;});goto _LL0;case 3U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
({struct _dyneither_ptr _tmp2EB=({const char*_tmp7A="enum tag";_tag_dyneither(_tmp7A,sizeof(char),9U);});hides_what=_tmp2EB;});goto _LL0;}_LL0:;}
# 323
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0U,_tmp2A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)hides_what);_tmp2A2;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp2ED=loc;struct _dyneither_ptr _tmp2EC=({const char*_tmp7D="declaration hides %s";_tag_dyneither(_tmp7D,sizeof(char),21U);});Cyc_Warn_warn(_tmp2ED,_tmp2EC,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});
_npop_handler(0U);return;
# 305
;_pop_handler();}else{void*_tmp6C=(void*)_exn_thrown;void*_tmp7F=_tmp6C;void*_tmp80;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp7F)->tag == Cyc_Binding_BindingError){_LL19: _LL1A:
# 325
 return;}else{_LL1B: _tmp80=_tmp7F;_LL1C:(int)_rethrow(_tmp80);}_LL18:;}};}
# 328
void Cyc_Binding_resolve_and_add_vardecl(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp81=vd->name;struct _tuple0*_tmp82=_tmp81;union Cyc_Absyn_Nmspace _tmp89;struct _dyneither_ptr*_tmp88;_LL1: _tmp89=_tmp82->f1;_tmp88=_tmp82->f2;_LL2:;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _tmp2F1=({struct Cyc_Dict_Dict _tmp2F0=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp2EF=_tmp88;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp2F0,_tmp2EF,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp84=_cycalloc(sizeof(*_tmp84));
_tmp84->tag=0U,({void*_tmp2EE=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=4U,_tmp83->f1=vd;_tmp83;});_tmp84->f1=_tmp2EE;});_tmp84;}));});
# 335
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp2F1;});else{
# 338
struct Cyc_Binding_ResolveNSEnv*_tmp85=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp89);
({struct Cyc_Dict_Dict _tmp2F5=({struct Cyc_Dict_Dict _tmp2F4=_tmp85->ordinaries;struct _dyneither_ptr*_tmp2F3=_tmp88;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp2F4,_tmp2F3,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp87=_cycalloc(sizeof(*_tmp87));
# 341
_tmp87->tag=0U,({void*_tmp2F2=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=1U,_tmp86->f1=vd;_tmp86;});_tmp87->f1=_tmp2F2;});_tmp87;}));});
# 339
_tmp85->ordinaries=_tmp2F5;});}};}
# 345
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp8A=s->r;void*_tmp8B=_tmp8A;struct Cyc_Absyn_Stmt*_tmpA3;struct Cyc_List_List*_tmpA2;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_List_List*_tmpA0;struct Cyc_Absyn_Decl*_tmp9F;struct Cyc_Absyn_Stmt*_tmp9E;struct Cyc_Absyn_Stmt*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Stmt*_tmp98;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Stmt*_tmp96;struct Cyc_Absyn_Stmt*_tmp95;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Stmt*_tmp92;struct Cyc_Absyn_Stmt*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Stmt*_tmp8F;struct Cyc_Absyn_Stmt*_tmp8E;struct Cyc_Absyn_Exp*_tmp8D;switch(*((int*)_tmp8B)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp8D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_LL4:
 Cyc_Binding_resolve_exp(env,_tmp8D);return;case 2U: _LL5: _tmp8F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp8E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL6:
 Cyc_Binding_resolve_stmt(env,_tmp8F);Cyc_Binding_resolve_stmt(env,_tmp8E);return;case 3U: _LL7: _tmp90=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_LL8:
 if((unsigned int)_tmp90)Cyc_Binding_resolve_exp(env,_tmp90);return;case 4U: _LL9: _tmp93=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp92=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_tmp91=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8B)->f3;_LLA:
# 352
 Cyc_Binding_resolve_exp(env,_tmp93);Cyc_Binding_resolve_stmt(env,_tmp92);Cyc_Binding_resolve_stmt(env,_tmp91);return;case 14U: _LLB: _tmp95=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp94=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2).f1;_LLC:
 _tmp97=_tmp94;_tmp96=_tmp95;goto _LLE;case 5U: _LLD: _tmp97=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1).f1;_tmp96=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LLE:
 Cyc_Binding_resolve_exp(env,_tmp97);Cyc_Binding_resolve_stmt(env,_tmp96);return;case 9U: _LLF: _tmp9B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp9A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2).f1;_tmp99=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f3).f1;_tmp98=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f4;_LL10:
# 356
 Cyc_Binding_resolve_exp(env,_tmp9B);Cyc_Binding_resolve_exp(env,_tmp9A);Cyc_Binding_resolve_exp(env,_tmp99);
Cyc_Binding_resolve_stmt(env,_tmp98);
return;case 6U: _LL11: _LL12:
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp9C=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_LL18:
# 363
 for(0;_tmp9C != 0;_tmp9C=_tmp9C->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp9C->hd);}
return;case 13U: _LL19: _tmp9D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL1A:
 Cyc_Binding_resolve_stmt(env,_tmp9D);return;case 12U: _LL1B: _tmp9F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp9E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL1C: {
# 368
struct Cyc_Dict_Dict _tmp8C=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_decl(env,_tmp9F);
Cyc_Binding_resolve_stmt(env,_tmp9E);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp8C;
return;}case 10U: _LL1D: _tmpA1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmpA0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL1E:
 Cyc_Binding_resolve_exp(env,_tmpA1);Cyc_Binding_resolve_scs(env,_tmpA0);return;default: _LL1F: _tmpA3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmpA2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL20:
 Cyc_Binding_resolve_stmt(env,_tmpA3);Cyc_Binding_resolve_scs(env,_tmpA2);return;}_LL0:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 377
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpA4=e->r;void*_tmpA5=_tmpA4;void*_tmp12E;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_Datatypedecl*_tmp12B;struct Cyc_Absyn_Datatypefield*_tmp12A;struct Cyc_Absyn_Enumdecl*_tmp129;struct Cyc_Absyn_Enumfield*_tmp128;void*_tmp127;struct Cyc_Absyn_Enumfield*_tmp126;struct Cyc_List_List*_tmp125;void*_tmp124;void*_tmp123;void*_tmp122;struct Cyc_Absyn_Stmt*_tmp121;void*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;void**_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Vardecl*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;struct _tuple0**_tmpF6;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Aggrdecl**_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_List_List*_tmpF1;int*_tmpF0;void**_tmpEF;switch(*((int*)_tmpA5)){case 1U: _LL1: _tmpEF=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL2: {
# 380
void*_tmpA6=*_tmpEF;void*_tmpA7=_tmpA6;struct _tuple0*_tmpC0;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA7)->tag == 0U){_LL56: _tmpC0=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA7)->f1;_LL57: {
# 382
struct _handler_cons _tmpA8;_push_handler(& _tmpA8);{int _tmpAA=0;if(setjmp(_tmpA8.handler))_tmpAA=1;if(!_tmpAA){{void*_tmpAB=Cyc_Binding_lookup_ordinary(e->loc,env,_tmpC0);void*_tmpAC=_tmpAB;struct Cyc_Absyn_Aggrdecl*_tmpBA;void*_tmpB9;struct Cyc_Absyn_Enumfield*_tmpB8;struct Cyc_Absyn_Enumdecl*_tmpB7;struct Cyc_Absyn_Enumfield*_tmpB6;struct Cyc_Absyn_Datatypedecl*_tmpB5;struct Cyc_Absyn_Datatypefield*_tmpB4;void*_tmpB3;switch(*((int*)_tmpAC)){case 0U: _LL5B: _tmpB3=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpAC)->f1;_LL5C:
*_tmpEF=_tmpB3;_npop_handler(0U);return;case 2U: _LL5D: _tmpB5=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpAC)->f1;_tmpB4=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpAC)->f2;_LL5E:
({void*_tmp2F6=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=31U,_tmpAD->f1=0,_tmpAD->f2=_tmpB5,_tmpAD->f3=_tmpB4;_tmpAD;});e->r=_tmp2F6;});_npop_handler(0U);return;case 3U: _LL5F: _tmpB7=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpAC)->f1;_tmpB6=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpAC)->f2;_LL60:
({void*_tmp2F7=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=32U,_tmpAE->f1=_tmpB7,_tmpAE->f2=_tmpB6;_tmpAE;});e->r=_tmp2F7;});_npop_handler(0U);return;case 4U: _LL61: _tmpB9=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpAC)->f1;_tmpB8=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpAC)->f2;_LL62:
({void*_tmp2F8=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=33U,_tmpAF->f1=_tmpB9,_tmpAF->f2=_tmpB8;_tmpAF;});e->r=_tmp2F8;});_npop_handler(0U);return;default: _LL63: _tmpBA=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpAC)->f1;_LL64:
# 388
({struct Cyc_String_pa_PrintArg_struct _tmpB2=({struct Cyc_String_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=0U,({struct _dyneither_ptr _tmp2F9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC0));_tmp2A3.f1=_tmp2F9;});_tmp2A3;});void*_tmpB0[1U];_tmpB0[0]=& _tmpB2;({unsigned int _tmp2FB=e->loc;struct _dyneither_ptr _tmp2FA=({const char*_tmpB1="bad occurrence of type name %s";_tag_dyneither(_tmpB1,sizeof(char),31U);});Cyc_Warn_err(_tmp2FB,_tmp2FA,_tag_dyneither(_tmpB0,sizeof(void*),1U));});});
_npop_handler(0U);return;}_LL5A:;}
# 382
;_pop_handler();}else{void*_tmpA9=(void*)_exn_thrown;void*_tmpBB=_tmpA9;void*_tmpBF;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpBB)->tag == Cyc_Binding_BindingError){_LL66: _LL67:
# 392
({struct Cyc_String_pa_PrintArg_struct _tmpBE=({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0U,({struct _dyneither_ptr _tmp2FC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC0));_tmp2A4.f1=_tmp2FC;});_tmp2A4;});void*_tmpBC[1U];_tmpBC[0]=& _tmpBE;({unsigned int _tmp2FE=e->loc;struct _dyneither_ptr _tmp2FD=({const char*_tmpBD="undeclared identifier %s";_tag_dyneither(_tmpBD,sizeof(char),25U);});Cyc_Warn_err(_tmp2FE,_tmp2FD,_tag_dyneither(_tmpBC,sizeof(void*),1U));});});
return;}else{_LL68: _tmpBF=_tmpBB;_LL69:(int)_rethrow(_tmpBF);}_LL65:;}};}}else{_LL58: _LL59:
# 395
 return;}_LL55:;}case 10U: _LL3: _tmpF2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpF1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmpF0=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA5)->f4;_LL4:
# 398
*_tmpF0=1;
# 400
{struct Cyc_List_List*_tmpC1=_tmpF1;for(0;_tmpC1 != 0;_tmpC1=_tmpC1->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpC1->hd);}}{
void*_tmpC2=_tmpF2->r;void*_tmpC3=_tmpC2;void**_tmpE0;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC3)->tag == 1U){_LL6B: _tmpE0=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC3)->f1;_LL6C: {
# 404
void*_tmpC4=*_tmpE0;void*_tmpC5=_tmpC4;struct _tuple0*_tmpDF;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC5)->tag == 0U){_LL70: _tmpDF=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC5)->f1;_LL71: {
# 406
struct _handler_cons _tmpC6;_push_handler(& _tmpC6);{int _tmpC8=0;if(setjmp(_tmpC6.handler))_tmpC8=1;if(!_tmpC8){{void*_tmpC9=Cyc_Binding_lookup_ordinary(_tmpF2->loc,env,_tmpDF);void*_tmpCA=_tmpC9;struct Cyc_Absyn_Datatypedecl*_tmpD9;struct Cyc_Absyn_Datatypefield*_tmpD8;struct Cyc_Absyn_Aggrdecl*_tmpD7;void*_tmpD6;switch(*((int*)_tmpCA)){case 0U: _LL75: _tmpD6=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpCA)->f1;_LL76:
# 408
*_tmpE0=_tmpD6;_npop_handler(0U);return;case 1U: _LL77: _tmpD7=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpCA)->f1;_LL78: {
# 410
struct Cyc_List_List*_tmpCB=0;
for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
({struct Cyc_List_List*_tmp300=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));({struct _tuple10*_tmp2FF=({struct _tuple10*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->f1=0,_tmpCC->f2=(struct Cyc_Absyn_Exp*)_tmpF1->hd;_tmpCC;});_tmpCD->hd=_tmp2FF;}),_tmpCD->tl=_tmpCB;_tmpCD;});_tmpCB=_tmp300;});}
({void*_tmp302=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->tag=29U,_tmpCE->f1=_tmpD7->name,_tmpCE->f2=0,({struct Cyc_List_List*_tmp301=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCB);_tmpCE->f3=_tmp301;}),_tmpCE->f4=_tmpD7;_tmpCE;});e->r=_tmp302;});
_npop_handler(0U);return;}case 2U: _LL79: _tmpD9=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpCA)->f1;_tmpD8=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpCA)->f2;_LL7A:
# 416
 if(_tmpD8->typs == 0)
# 418
({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmp2A5;_tmp2A5.tag=0U,({
struct _dyneither_ptr _tmp303=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD8->name));_tmp2A5.f1=_tmp303;});_tmp2A5;});void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({unsigned int _tmp305=e->loc;struct _dyneither_ptr _tmp304=({const char*_tmpD0="%s is a constant, not a function";_tag_dyneither(_tmpD0,sizeof(char),33U);});Cyc_Warn_err(_tmp305,_tmp304,_tag_dyneither(_tmpCF,sizeof(void*),1U));});});
({void*_tmp306=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->tag=31U,_tmpD2->f1=_tmpF1,_tmpD2->f2=_tmpD9,_tmpD2->f3=_tmpD8;_tmpD2;});e->r=_tmp306;});
_npop_handler(0U);return;case 4U: _LL7B: _LL7C:
 goto _LL7E;default: _LL7D: _LL7E:
# 424
({struct Cyc_String_pa_PrintArg_struct _tmpD5=({struct Cyc_String_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=0U,({
struct _dyneither_ptr _tmp307=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpDF));_tmp2A6.f1=_tmp307;});_tmp2A6;});void*_tmpD3[1U];_tmpD3[0]=& _tmpD5;({unsigned int _tmp309=e->loc;struct _dyneither_ptr _tmp308=({const char*_tmpD4="%s is an enum constructor, not a function";_tag_dyneither(_tmpD4,sizeof(char),42U);});Cyc_Warn_err(_tmp309,_tmp308,_tag_dyneither(_tmpD3,sizeof(void*),1U));});});
_npop_handler(0U);return;}_LL74:;}
# 406
;_pop_handler();}else{void*_tmpC7=(void*)_exn_thrown;void*_tmpDA=_tmpC7;void*_tmpDE;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpDA)->tag == Cyc_Binding_BindingError){_LL80: _LL81:
# 428
({struct Cyc_String_pa_PrintArg_struct _tmpDD=({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0U,({struct _dyneither_ptr _tmp30A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpDF));_tmp2A7.f1=_tmp30A;});_tmp2A7;});void*_tmpDB[1U];_tmpDB[0]=& _tmpDD;({unsigned int _tmp30C=e->loc;struct _dyneither_ptr _tmp30B=({const char*_tmpDC="undeclared identifier %s";_tag_dyneither(_tmpDC,sizeof(char),25U);});Cyc_Warn_err(_tmp30C,_tmp30B,_tag_dyneither(_tmpDB,sizeof(void*),1U));});});
return;}else{_LL82: _tmpDE=_tmpDA;_LL83:(int)_rethrow(_tmpDE);}_LL7F:;}};}}else{_LL72: _LL73:
# 431
 return;}_LL6F:;}}else{_LL6D: _LL6E:
# 433
 Cyc_Binding_resolve_exp(env,_tmpF2);return;}_LL6A:;};case 29U: _LL5: _tmpF6=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpF5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmpF4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_tmpF3=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f4;_LL6:
# 437
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpF4->hd)).f2);}{
struct _handler_cons _tmpE1;_push_handler(& _tmpE1);{int _tmpE3=0;if(setjmp(_tmpE1.handler))_tmpE3=1;if(!_tmpE3){
({struct Cyc_Absyn_Aggrdecl*_tmp30D=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*_tmpF6,Cyc_Binding_lookup_aggrdecl);*_tmpF3=_tmp30D;});
*_tmpF6=((struct Cyc_Absyn_Aggrdecl*)_check_null(*_tmpF3))->name;
_npop_handler(0U);return;
# 440
;_pop_handler();}else{void*_tmpE2=(void*)_exn_thrown;void*_tmpE4=_tmpE2;void*_tmpE8;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpE4)->tag == Cyc_Binding_BindingError){_LL85: _LL86:
# 444
({struct Cyc_String_pa_PrintArg_struct _tmpE7=({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0U,({struct _dyneither_ptr _tmp30E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmpF6));_tmp2A8.f1=_tmp30E;});_tmp2A8;});void*_tmpE5[1U];_tmpE5[0]=& _tmpE7;({unsigned int _tmp310=e->loc;struct _dyneither_ptr _tmp30F=({const char*_tmpE6="unbound struct/union name %s";_tag_dyneither(_tmpE6,sizeof(char),29U);});Cyc_Warn_err(_tmp310,_tmp30F,_tag_dyneither(_tmpE5,sizeof(void*),1U));});});
return;}else{_LL87: _tmpE8=_tmpE4;_LL88:(int)_rethrow(_tmpE8);}_LL84:;}};};case 27U: _LL7: _tmpF9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpF8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmpF7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL8:
# 449
 Cyc_Binding_resolve_exp(env,_tmpF8);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_tmp312=({struct Cyc_Dict_Dict*_tmpE9=_cycalloc(sizeof(*_tmpE9));({struct Cyc_Dict_Dict _tmp311=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmpE9=_tmp311;});_tmpE9;});env->local_vars=_tmp312;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmpF9);
Cyc_Binding_resolve_exp(env,_tmpF7);
env->local_vars=0;
return;}{
# 457
struct Cyc_Dict_Dict _tmpEA=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmpF9);
Cyc_Binding_resolve_exp(env,_tmpF7);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmpEA;
return;};case 36U: _LL9: _tmpFA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LLA:
# 465
 for(0;_tmpFA != 0;_tmpFA=_tmpFA->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpFA->hd)).f2);}
return;case 28U: _LLB: _tmpFC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpFB=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LLC:
# 471
 Cyc_Binding_resolve_exp(env,_tmpFC);
Cyc_Binding_resolve_type(e->loc,env,_tmpFB);
return;case 2U: _LLD: _LLE:
# 475
 goto _LL10;case 0U: _LLF: _LL10:
 return;case 24U: _LL11: _tmpFD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL12:
# 478
 _tmpFE=_tmpFD;goto _LL14;case 3U: _LL13: _tmpFE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL14:
# 480
 for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpFE->hd);}
return;case 41U: _LL15: _tmpFF=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL16:
# 484
 _tmp100=_tmpFF;goto _LL18;case 38U: _LL17: _tmp100=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL18:
 _tmp101=_tmp100;goto _LL1A;case 12U: _LL19: _tmp101=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL1A:
 _tmp102=_tmp101;goto _LL1C;case 18U: _LL1B: _tmp102=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL1C:
 _tmp103=_tmp102;goto _LL1E;case 11U: _LL1D: _tmp103=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL1E:
 _tmp104=_tmp103;goto _LL20;case 5U: _LL1F: _tmp104=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL20:
 _tmp105=_tmp104;goto _LL22;case 22U: _LL21: _tmp105=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL22:
 _tmp106=_tmp105;goto _LL24;case 21U: _LL23: _tmp106=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL24:
 _tmp107=_tmp106;goto _LL26;case 15U: _LL25: _tmp107=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL26:
 _tmp108=_tmp107;goto _LL28;case 20U: _LL27: _tmp108=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL28:
 Cyc_Binding_resolve_exp(env,_tmp108);return;case 35U: _LL29: _tmp10A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp109=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL2A:
# 495
 _tmp10C=_tmp10A;_tmp10B=_tmp109;goto _LL2C;case 9U: _LL2B: _tmp10C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp10B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL2C:
 _tmp10E=_tmp10C;_tmp10D=_tmp10B;goto _LL2E;case 4U: _LL2D: _tmp10E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp10D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL2E:
 _tmp110=_tmp10E;_tmp10F=_tmp10D;goto _LL30;case 23U: _LL2F: _tmp110=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp10F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL30:
 _tmp112=_tmp110;_tmp111=_tmp10F;goto _LL32;case 7U: _LL31: _tmp112=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp111=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL32:
 _tmp114=_tmp112;_tmp113=_tmp111;goto _LL34;case 8U: _LL33: _tmp114=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp113=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL34:
 Cyc_Binding_resolve_exp(env,_tmp114);Cyc_Binding_resolve_exp(env,_tmp113);return;case 34U: _LL35: _tmp117=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).rgn;_tmp116=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).elt_type;_tmp115=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).num_elts;_LL36:
# 503
 if(_tmp117 != 0)Cyc_Binding_resolve_exp(env,_tmp117);
if(_tmp116 != 0)Cyc_Binding_resolve_type(e->loc,env,*_tmp116);
Cyc_Binding_resolve_exp(env,_tmp115);
return;case 16U: _LL37: _tmp119=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp118=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL38:
# 509
 if(_tmp119 != 0)Cyc_Binding_resolve_exp(env,_tmp119);
Cyc_Binding_resolve_exp(env,_tmp118);
return;case 6U: _LL39: _tmp11C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp11B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmp11A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL3A:
# 514
 Cyc_Binding_resolve_exp(env,_tmp11C);Cyc_Binding_resolve_exp(env,_tmp11B);Cyc_Binding_resolve_exp(env,_tmp11A);return;case 13U: _LL3B: _tmp11E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp11D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL3C:
# 517
 Cyc_Binding_resolve_exp(env,_tmp11E);
for(0;_tmp11D != 0;_tmp11D=_tmp11D->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)_tmp11D->hd);}
return;case 14U: _LL3D: _tmp120=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp11F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL3E:
# 523
 Cyc_Binding_resolve_exp(env,_tmp11F);Cyc_Binding_resolve_type(e->loc,env,_tmp120);return;case 37U: _LL3F: _tmp121=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL40:
# 525
 Cyc_Binding_resolve_stmt(env,_tmp121);return;case 39U: _LL41: _tmp122=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL42:
# 527
 _tmp123=_tmp122;goto _LL44;case 19U: _LL43: _tmp123=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL44:
 _tmp124=_tmp123;goto _LL46;case 17U: _LL45: _tmp124=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL46:
 Cyc_Binding_resolve_type(e->loc,env,_tmp124);return;case 26U: _LL47: _tmp125=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL48:
# 532
 for(0;_tmp125 != 0;_tmp125=_tmp125->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp125->hd)).f2);}
return;case 25U: _LL49: _LL4A:
# 537
({void*_tmpEB=0U;({unsigned int _tmp314=e->loc;struct _dyneither_ptr _tmp313=({const char*_tmpEC="binding: compound literals unimplemented";_tag_dyneither(_tmpEC,sizeof(char),41U);});Cyc_Warn_err(_tmp314,_tmp313,_tag_dyneither(_tmpEB,sizeof(void*),0U));});});return;case 33U: _LL4B: _tmp127=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp126=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL4C:
# 540
 Cyc_Binding_resolve_type(e->loc,env,_tmp127);return;case 32U: _LL4D: _tmp129=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp128=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL4E:
 return;case 31U: _LL4F: _tmp12C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp12B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmp12A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL50:
# 543
 for(0;_tmp12C != 0;_tmp12C=_tmp12C->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp12C->hd);}
return;case 30U: _LL51: _tmp12E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp12D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL52:
# 547
 Cyc_Binding_resolve_type(e->loc,env,_tmp12E);
for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp12D->hd)).f2);}
return;default: _LL53: _LL54:
# 553
({void*_tmpED=0U;({unsigned int _tmp316=e->loc;struct _dyneither_ptr _tmp315=({const char*_tmpEE="asm expressions cannot occur within Cyclone code";_tag_dyneither(_tmpEE,sizeof(char),49U);});Cyc_Warn_err(_tmp316,_tmp315,_tag_dyneither(_tmpED,sizeof(void*),0U));});});}_LL0:;}
# 557
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp12F=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp130=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp131=_tmp130;struct Cyc_Absyn_Pat*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Stmt*_tmp132;_LL1: _tmp134=_tmp131->pattern;_tmp133=_tmp131->where_clause;_tmp132=_tmp131->body;_LL2:;
Cyc_Binding_resolve_pat(env,_tmp134);
if(_tmp133 != 0)
Cyc_Binding_resolve_exp(env,_tmp133);
Cyc_Binding_resolve_stmt(env,_tmp132);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp12F;}
# 567
return;}
# 569
void Cyc_Binding_resolve_aggrfields(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp135=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_tmp136=_tmp135;struct _dyneither_ptr*_tmp13C;struct Cyc_Absyn_Tqual _tmp13B;void*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Exp*_tmp137;_LL1: _tmp13C=_tmp136->name;_tmp13B=_tmp136->tq;_tmp13A=_tmp136->type;_tmp139=_tmp136->width;_tmp138=_tmp136->attributes;_tmp137=_tmp136->requires_clause;_LL2:;
Cyc_Binding_resolve_type(loc,env,_tmp13A);
if(_tmp139 != 0)
Cyc_Binding_resolve_exp(env,_tmp139);
if(_tmp137 != 0)
Cyc_Binding_resolve_exp(env,_tmp137);}
# 578
return;}struct _tuple11{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 581
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(struct _dyneither_ptr*(*f)(void*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 587
struct Cyc_List_List*_tmp13D=0;
for(0;args != 0;args=args->tl){
struct _tuple11*_tmp13E=(struct _tuple11*)args->hd;struct _tuple11*_tmp13F=_tmp13E;void*_tmp146;struct Cyc_Absyn_Tqual _tmp145;void*_tmp144;_LL1: _tmp146=_tmp13F->f1;_tmp145=_tmp13F->f2;_tmp144=_tmp13F->f3;_LL2:;{
struct _dyneither_ptr*_tmp140=f(_tmp146);
if(_tmp140 == 0)continue;{
struct Cyc_Absyn_Vardecl*_tmp141=({struct _tuple0*_tmp317=({struct _tuple0*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->f1=Cyc_Absyn_Loc_n,_tmp143->f2=_tmp140;_tmp143;});Cyc_Absyn_new_vardecl(0U,_tmp317,_tmp144,0);});
_tmp141->tq=_tmp145;
({struct Cyc_List_List*_tmp318=({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->hd=_tmp141,_tmp142->tl=_tmp13D;_tmp142;});_tmp13D=_tmp318;});};};}
# 596
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp147=*vai;struct Cyc_Absyn_VarargInfo _tmp148=_tmp147;struct _dyneither_ptr*_tmp152;struct Cyc_Absyn_Tqual _tmp151;void*_tmp150;int _tmp14F;_LL4: _tmp152=_tmp148.name;_tmp151=_tmp148.tq;_tmp150=_tmp148.type;_tmp14F=_tmp148.inject;_LL5:;
if(_tmp152 == 0){
if(need_va_name)
({void*_tmp149=0U;({unsigned int _tmp31A=loc;struct _dyneither_ptr _tmp319=({const char*_tmp14A="missing name for vararg";_tag_dyneither(_tmp14A,sizeof(char),24U);});Cyc_Warn_err(_tmp31A,_tmp319,_tag_dyneither(_tmp149,sizeof(void*),0U));});});}else{
# 603
void*_tmp14B=Cyc_Absyn_dyneither_typ(_tmp150,(void*)& Cyc_Absyn_HeapRgn_val,_tmp151,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp14C=({struct _tuple0*_tmp31B=({struct _tuple0*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->f1=Cyc_Absyn_Loc_n,_tmp14E->f2=_tmp152;_tmp14E;});Cyc_Absyn_new_vardecl(0U,_tmp31B,_tmp14B,0);});
({struct Cyc_List_List*_tmp31C=({struct Cyc_List_List*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->hd=_tmp14C,_tmp14D->tl=_tmp13D;_tmp14D;});_tmp13D=_tmp31C;});}}
# 608
({struct Cyc_List_List*_tmp31D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13D);_tmp13D=_tmp31D;});
return _tmp13D;}
# 611
struct _dyneither_ptr*Cyc_Binding_get_vds_fndecl(struct _dyneither_ptr*v){return v;}
struct _dyneither_ptr*Cyc_Binding_get_vds_fntype(struct _dyneither_ptr*v){return v;}
# 614
void Cyc_Binding_resolve_function_stuff(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause){
# 618
if(eff != 0)
Cyc_Binding_resolve_type(loc,env,eff);
Cyc_Binding_resolve_type(loc,env,rettyp);
for(0;args != 0;args=args->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)args->hd)).f3);}
if(vai != 0)
Cyc_Binding_resolve_type(loc,env,vai->type);
Cyc_Binding_resolve_rgnpo(loc,env,rpo);
if(req_clause != 0)
Cyc_Binding_resolve_exp(env,req_clause);
if(ens_clause != 0){
# 630
struct Cyc_Dict_Dict _tmp153=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
struct _dyneither_ptr*_tmp154=({struct _dyneither_ptr*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct _dyneither_ptr _tmp31E=({const char*_tmp159="return_value";_tag_dyneither(_tmp159,sizeof(char),13U);});*_tmp15A=_tmp31E;});_tmp15A;});
struct Cyc_Absyn_Vardecl*_tmp155=({struct _tuple0*_tmp31F=({struct _tuple0*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->f1=Cyc_Absyn_Loc_n,_tmp158->f2=_tmp154;_tmp158;});Cyc_Absyn_new_vardecl(0U,_tmp31F,rettyp,0);});
({struct Cyc_Dict_Dict _tmp323=({struct Cyc_Dict_Dict _tmp322=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp321=_tmp154;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp322,_tmp321,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp157=_cycalloc(sizeof(*_tmp157));
_tmp157->tag=0U,({void*_tmp320=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->tag=4U,_tmp156->f1=_tmp155;_tmp156;});_tmp157->f1=_tmp320;});_tmp157;}));});
# 633
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp323;});
# 635
Cyc_Binding_resolve_exp(env,ens_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp153;}}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 645
void Cyc_Binding_resolve_type(unsigned int loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp15B=t;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D2;void*_tmp1D1;void*_tmp1D0;void*_tmp1CF;void*_tmp1CE;void*_tmp1CD;void*_tmp1CC;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_FnInfo _tmp1CA;void*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_List_List*_tmp1C7;void*_tmp1C6;void*_tmp1C5;union Cyc_Absyn_Constraint*_tmp1C4;void*_tmp1C3;struct Cyc_List_List*_tmp1C2;struct Cyc_List_List*_tmp1C1;struct Cyc_Absyn_TypeDecl*_tmp1C0;void***_tmp1BF;struct _tuple0**_tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Typedefdecl**_tmp1BC;struct _tuple0**_tmp1BB;struct Cyc_Absyn_Enumdecl*_tmp1BA;union Cyc_Absyn_AggrInfoU*_tmp1B9;struct Cyc_List_List*_tmp1B8;union Cyc_Absyn_DatatypeFieldInfoU*_tmp1B7;struct Cyc_List_List*_tmp1B6;union Cyc_Absyn_DatatypeInfoU*_tmp1B5;struct Cyc_List_List*_tmp1B4;switch(*((int*)_tmp15B)){case 3U: _LL1: _tmp1B5=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15B)->f1).datatype_info;_tmp1B4=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15B)->f1).targs;_LL2:
# 648
 for(0;_tmp1B4 != 0;_tmp1B4=_tmp1B4->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B4->hd);}{
union Cyc_Absyn_DatatypeInfoU _tmp15C=*_tmp1B5;union Cyc_Absyn_DatatypeInfoU _tmp15D=_tmp15C;struct _tuple0*_tmp165;int _tmp164;if((_tmp15D.UnknownDatatype).tag == 1){_LL3C: _tmp165=((_tmp15D.UnknownDatatype).val).name;_tmp164=((_tmp15D.UnknownDatatype).val).is_extensible;_LL3D: {
# 652
struct _handler_cons _tmp15E;_push_handler(& _tmp15E);{int _tmp160=0;if(setjmp(_tmp15E.handler))_tmp160=1;if(!_tmp160){
{struct Cyc_Absyn_Datatypedecl*_tmp161=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp165,Cyc_Binding_lookup_datatypedecl))->hd;
({union Cyc_Absyn_DatatypeInfoU _tmp324=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp2A9;_tmp2A9.name=_tmp161->name,_tmp2A9.is_extensible=_tmp164;_tmp2A9;}));*_tmp1B5=_tmp324;});
_npop_handler(0U);return;}
# 653
;_pop_handler();}else{void*_tmp15F=(void*)_exn_thrown;void*_tmp162=_tmp15F;void*_tmp163;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp162)->tag == Cyc_Binding_BindingError){_LL41: _LL42:
# 657
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp165,Cyc_Absyn_Public);return;}else{_LL43: _tmp163=_tmp162;_LL44:(int)_rethrow(_tmp163);}_LL40:;}};}}else{_LL3E: _LL3F:
 return;}_LL3B:;};case 4U: _LL3: _tmp1B7=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15B)->f1).field_info;_tmp1B6=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15B)->f1).targs;_LL4:
# 661
 for(0;_tmp1B6 != 0;_tmp1B6=_tmp1B6->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B6->hd);}{
union Cyc_Absyn_DatatypeFieldInfoU _tmp166=*_tmp1B7;union Cyc_Absyn_DatatypeFieldInfoU _tmp167=_tmp166;struct _tuple0*_tmp17C;struct _tuple0*_tmp17B;int _tmp17A;if((_tmp167.UnknownDatatypefield).tag == 1){_LL46: _tmp17C=((_tmp167.UnknownDatatypefield).val).datatype_name;_tmp17B=((_tmp167.UnknownDatatypefield).val).field_name;_tmp17A=((_tmp167.UnknownDatatypefield).val).is_extensible;_LL47:
# 667
{struct _handler_cons _tmp168;_push_handler(& _tmp168);{int _tmp16A=0;if(setjmp(_tmp168.handler))_tmp16A=1;if(!_tmp16A){
{void*_tmp16B=Cyc_Binding_lookup_ordinary(loc,env,_tmp17B);void*_tmp16C=_tmp16B;struct Cyc_Absyn_Datatypedecl*_tmp174;struct Cyc_Absyn_Datatypefield*_tmp173;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16C)->tag == 2U){_LL4B: _tmp174=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16C)->f1;_tmp173=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16C)->f2;_LL4C: {
# 670
struct Cyc_Absyn_Datatypedecl*_tmp16D=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp17C,Cyc_Binding_lookup_datatypedecl))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp174->name,_tmp16D->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp170=({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0U,({
struct _dyneither_ptr _tmp325=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp173->name));_tmp2AC.f1=_tmp325;});_tmp2AC;});struct Cyc_String_pa_PrintArg_struct _tmp171=({struct Cyc_String_pa_PrintArg_struct _tmp2AB;_tmp2AB.tag=0U,({
struct _dyneither_ptr _tmp326=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp16D->name));_tmp2AB.f1=_tmp326;});_tmp2AB;});struct Cyc_String_pa_PrintArg_struct _tmp172=({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0U,({
struct _dyneither_ptr _tmp327=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp174->name));_tmp2AA.f1=_tmp327;});_tmp2AA;});void*_tmp16E[3U];_tmp16E[0]=& _tmp170,_tmp16E[1]=& _tmp171,_tmp16E[2]=& _tmp172;({unsigned int _tmp329=loc;struct _dyneither_ptr _tmp328=({const char*_tmp16F="%s is a variant of %s, not %s (shadowing not yet implemented properly)";_tag_dyneither(_tmp16F,sizeof(char),71U);});Cyc_Warn_err(_tmp329,_tmp328,_tag_dyneither(_tmp16E,sizeof(void*),3U));});});
_npop_handler(0U);return;}
# 678
({union Cyc_Absyn_DatatypeFieldInfoU _tmp32A=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp2AD;_tmp2AD.datatype_name=_tmp174->name,_tmp2AD.field_name=_tmp173->name,_tmp2AD.is_extensible=_tmp17A;_tmp2AD;}));*_tmp1B7=_tmp32A;});
# 680
_npop_handler(0U);return;}}else{_LL4D: _LL4E:
 goto _LL4A;}_LL4A:;}
# 668
;_pop_handler();}else{void*_tmp169=(void*)_exn_thrown;void*_tmp175=_tmp169;void*_tmp176;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp175)->tag == Cyc_Binding_BindingError){_LL50: _LL51:
# 683
 goto _LL4F;}else{_LL52: _tmp176=_tmp175;_LL53:(int)_rethrow(_tmp176);}_LL4F:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0U,({struct _dyneither_ptr _tmp32B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp17B));_tmp2AE.f1=_tmp32B;});_tmp2AE;});void*_tmp177[1U];_tmp177[0]=& _tmp179;({unsigned int _tmp32D=loc;struct _dyneither_ptr _tmp32C=({const char*_tmp178="%s is not a datatype field";_tag_dyneither(_tmp178,sizeof(char),27U);});Cyc_Warn_err(_tmp32D,_tmp32C,_tag_dyneither(_tmp177,sizeof(void*),1U));});});
return;}else{_LL48: _LL49:
 return;}_LL45:;};case 11U: _LL5: _tmp1B9=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15B)->f1).aggr_info;_tmp1B8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15B)->f1).targs;_LL6:
# 690
 for(0;_tmp1B8 != 0;_tmp1B8=_tmp1B8->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B8->hd);}{
union Cyc_Absyn_AggrInfoU _tmp17D=*_tmp1B9;union Cyc_Absyn_AggrInfoU _tmp17E=_tmp17D;enum Cyc_Absyn_AggrKind _tmp18B;struct _tuple0*_tmp18A;struct Cyc_Core_Opt*_tmp189;if((_tmp17E.UnknownAggr).tag == 1){_LL55: _tmp18B=((_tmp17E.UnknownAggr).val).f1;_tmp18A=((_tmp17E.UnknownAggr).val).f2;_tmp189=((_tmp17E.UnknownAggr).val).f3;_LL56: {
# 694
struct _handler_cons _tmp17F;_push_handler(& _tmp17F);{int _tmp181=0;if(setjmp(_tmp17F.handler))_tmp181=1;if(!_tmp181){
{struct Cyc_Absyn_Aggrdecl*_tmp182=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp18A,Cyc_Binding_lookup_aggrdecl);
if(_tmp182->kind != _tmp18B)
({void*_tmp183=0U;({unsigned int _tmp32F=loc;struct _dyneither_ptr _tmp32E=({const char*_tmp184="struct vs. union mismatch with earlier declaration";_tag_dyneither(_tmp184,sizeof(char),51U);});Cyc_Warn_err(_tmp32F,_tmp32E,_tag_dyneither(_tmp183,sizeof(void*),0U));});});
if(((_tmp18B == Cyc_Absyn_UnionA  && _tmp189 != 0) && _tmp182->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp182->impl))->tagged != (int)_tmp189->v)
# 700
({void*_tmp185=0U;({unsigned int _tmp331=loc;struct _dyneither_ptr _tmp330=({const char*_tmp186="@tagged mismatch with earlier declaration";_tag_dyneither(_tmp186,sizeof(char),42U);});Cyc_Warn_err(_tmp331,_tmp330,_tag_dyneither(_tmp185,sizeof(void*),0U));});});
({union Cyc_Absyn_AggrInfoU _tmp332=Cyc_Absyn_UnknownAggr(_tmp18B,_tmp182->name,_tmp189);*_tmp1B9=_tmp332;});
_npop_handler(0U);return;}
# 695
;_pop_handler();}else{void*_tmp180=(void*)_exn_thrown;void*_tmp187=_tmp180;void*_tmp188;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp187)->tag == Cyc_Binding_BindingError){_LL5A: _LL5B:
# 704
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp18A,Cyc_Absyn_Public);return;}else{_LL5C: _tmp188=_tmp187;_LL5D:(int)_rethrow(_tmp188);}_LL59:;}};}}else{_LL57: _LL58:
 return;}_LL54:;};case 13U: _LL7: _tmp1BB=(struct _tuple0**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp15B)->f1;_tmp1BA=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp15B)->f2;_LL8:
# 708
 if(_tmp1BA != 0)
return;{
struct _handler_cons _tmp18C;_push_handler(& _tmp18C);{int _tmp18E=0;if(setjmp(_tmp18C.handler))_tmp18E=1;if(!_tmp18E){
{struct Cyc_Absyn_Enumdecl*_tmp18F=((struct Cyc_Absyn_Enumdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1BB,Cyc_Binding_lookup_enumdecl);
*_tmp1BB=_tmp18F->name;
_npop_handler(0U);return;}
# 711
;_pop_handler();}else{void*_tmp18D=(void*)_exn_thrown;void*_tmp190=_tmp18D;void*_tmp191;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp190)->tag == Cyc_Binding_BindingError){_LL5F: _LL60:
# 715
 Cyc_Binding_absolutize_topdecl(loc,env,*_tmp1BB,Cyc_Absyn_Public);return;}else{_LL61: _tmp191=_tmp190;_LL62:(int)_rethrow(_tmp191);}_LL5E:;}};};case 17U: _LL9: _tmp1BE=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15B)->f1;_tmp1BD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15B)->f2;_tmp1BC=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15B)->f3;_LLA:
# 718
 for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1BD->hd);}
{struct _handler_cons _tmp192;_push_handler(& _tmp192);{int _tmp194=0;if(setjmp(_tmp192.handler))_tmp194=1;if(!_tmp194){
{struct Cyc_Absyn_Typedefdecl*_tmp195=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1BE,Cyc_Binding_lookup_typedefdecl);
# 723
*_tmp1BE=_tmp195->name;
_npop_handler(0U);return;}
# 721
;_pop_handler();}else{void*_tmp193=(void*)_exn_thrown;void*_tmp196=_tmp193;void*_tmp197;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp196)->tag == Cyc_Binding_BindingError){_LL64: _LL65:
# 725
 goto _LL63;}else{_LL66: _tmp197=_tmp196;_LL67:(int)_rethrow(_tmp197);}_LL63:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp19A=({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0U,({struct _dyneither_ptr _tmp333=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp1BE));_tmp2AF.f1=_tmp333;});_tmp2AF;});void*_tmp198[1U];_tmp198[0]=& _tmp19A;({unsigned int _tmp335=loc;struct _dyneither_ptr _tmp334=({const char*_tmp199="unbound typdef name %s";_tag_dyneither(_tmp199,sizeof(char),23U);});Cyc_Warn_err(_tmp335,_tmp334,_tag_dyneither(_tmp198,sizeof(void*),1U));});});
return;case 26U: _LLB: _tmp1C0=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15B)->f1;_tmp1BF=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15B)->f2;_LLC: {
# 731
struct Cyc_Dict_Dict*_tmp19B=env->local_vars;
env->local_vars=0;
{void*_tmp19C=_tmp1C0->r;void*_tmp19D=_tmp19C;struct Cyc_Absyn_Datatypedecl*_tmp1A6;struct Cyc_Absyn_Enumdecl*_tmp1A5;struct Cyc_Absyn_Aggrdecl*_tmp1A4;switch(*((int*)_tmp19D)){case 0U: _LL69: _tmp1A4=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp19D)->f1;_LL6A:
# 735
({struct Cyc_Binding_Env*_tmp337=env;Cyc_Binding_resolve_decl(_tmp337,({struct Cyc_Absyn_Decl*_tmp19F=_cycalloc(sizeof(*_tmp19F));({void*_tmp336=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=5U,_tmp19E->f1=_tmp1A4;_tmp19E;});_tmp19F->r=_tmp336;}),_tmp19F->loc=_tmp1C0->loc;_tmp19F;}));});goto _LL68;case 1U: _LL6B: _tmp1A5=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp19D)->f1;_LL6C:
# 737
({struct Cyc_Binding_Env*_tmp339=env;Cyc_Binding_resolve_decl(_tmp339,({struct Cyc_Absyn_Decl*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));({void*_tmp338=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->tag=7U,_tmp1A0->f1=_tmp1A5;_tmp1A0;});_tmp1A1->r=_tmp338;}),_tmp1A1->loc=_tmp1C0->loc;_tmp1A1;}));});goto _LL68;default: _LL6D: _tmp1A6=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp19D)->f1;_LL6E:
# 739
({struct Cyc_Binding_Env*_tmp33B=env;Cyc_Binding_resolve_decl(_tmp33B,({struct Cyc_Absyn_Decl*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));({void*_tmp33A=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->tag=6U,_tmp1A2->f1=_tmp1A6;_tmp1A2;});_tmp1A3->r=_tmp33A;}),_tmp1A3->loc=_tmp1C0->loc;_tmp1A3;}));});goto _LL68;}_LL68:;}
# 741
env->local_vars=_tmp19B;
return;}case 12U: _LLD: _tmp1C1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp15B)->f2;_LLE:
# 748
 Cyc_Binding_resolve_aggrfields(loc,env,_tmp1C1);
return;case 14U: _LLF: _tmp1C2=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp15B)->f1;_LL10:
# 753
 for(0;_tmp1C2 != 0;_tmp1C2=_tmp1C2->tl){
struct Cyc_Absyn_Enumfield*_tmp1A7=(struct Cyc_Absyn_Enumfield*)_tmp1C2->hd;
if(_tmp1A7->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp1A7->tag));}
# 758
return;case 1U: _LL11: _tmp1C3=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15B)->f2;_LL12:
 if(_tmp1C3 != 0)Cyc_Binding_resolve_type(loc,env,_tmp1C3);return;case 28U: _LL13: _LL14:
# 761
 goto _LL16;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 20U: _LL1B: _LL1C:
 goto _LL1E;case 21U: _LL1D: _LL1E:
 goto _LL20;case 22U: _LL1F: _LL20:
 goto _LL22;case 2U: _LL21: _LL22:
 return;case 5U: _LL23: _tmp1C6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15B)->f1).elt_typ;_tmp1C5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15B)->f1).ptr_atts).rgn;_tmp1C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15B)->f1).ptr_atts).bounds;_LL24:
# 771
 Cyc_Binding_resolve_type(loc,env,_tmp1C6);
Cyc_Binding_resolve_type(loc,env,_tmp1C5);
while(1){
union Cyc_Absyn_Constraint*_tmp1A8=_tmp1C4;void*_tmp1AC;union Cyc_Absyn_Constraint*_tmp1AB;switch((((union Cyc_Absyn_Constraint*)_tmp1A8)->Forward_constr).tag){case 3U: _LL70: _LL71:
 return;case 2U: _LL72: _tmp1AB=(_tmp1A8->Forward_constr).val;_LL73:
 _tmp1C4=_tmp1AB;continue;default: _LL74: _tmp1AC=(void*)(_tmp1A8->Eq_constr).val;_LL75: {
# 778
void*_tmp1A9=_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AA;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1A9)->tag == 0U){_LL77: _LL78:
 return;}else{_LL79: _tmp1AA=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1A9)->f1;_LL7A:
 Cyc_Binding_resolve_exp(env,_tmp1AA);return;}_LL76:;}}_LL6F:;}case 10U: _LL25: _tmp1C7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp15B)->f1;_LL26:
# 784
 for(0;_tmp1C7 != 0;_tmp1C7=_tmp1C7->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp1C7->hd)).f2);}
return;case 8U: _LL27: _tmp1C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15B)->f1).elt_type;_tmp1C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15B)->f1).num_elts;_LL28:
# 788
 Cyc_Binding_resolve_type(loc,env,_tmp1C9);
if(_tmp1C8 != 0)Cyc_Binding_resolve_exp(env,_tmp1C8);
return;case 9U: _LL29: _tmp1CA=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15B)->f1;_LL2A: {
# 795
struct Cyc_List_List*_tmp1AD=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fntype,0,loc,env,_tmp1CA.args,_tmp1CA.cyc_varargs);
# 798
struct Cyc_Dict_Dict*_tmp1AE=env->local_vars;
if(_tmp1AE != 0)
({struct Cyc_Dict_Dict*_tmp33C=({struct Cyc_Dict_Dict*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));*_tmp1AF=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1AF;});env->local_vars=_tmp33C;});else{
# 802
({struct Cyc_Dict_Dict*_tmp33E=({struct Cyc_Dict_Dict*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));({struct Cyc_Dict_Dict _tmp33D=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp1B0=_tmp33D;});_tmp1B0;});env->local_vars=_tmp33E;});}
{struct Cyc_List_List*_tmp1B1=_tmp1AD;for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){
({struct Cyc_Dict_Dict _tmp342=({struct Cyc_Dict_Dict _tmp341=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp340=(*((struct Cyc_Absyn_Vardecl*)_tmp1B1->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp341,_tmp340,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));
_tmp1B3->tag=0U,({void*_tmp33F=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->tag=3U,_tmp1B2->f1=(struct Cyc_Absyn_Vardecl*)_tmp1B1->hd;_tmp1B2;});_tmp1B3->f1=_tmp33F;});_tmp1B3;}));});
# 804
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp342;});}}
# 807
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp1CA.effect,_tmp1CA.ret_typ,_tmp1CA.args,_tmp1CA.cyc_varargs,_tmp1CA.rgn_po,_tmp1CA.requires_clause,_tmp1CA.ensures_clause);
# 811
env->local_vars=_tmp1AE;
return;}case 24U: _LL2B: _tmp1CB=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp15B)->f1;_LL2C:
# 815
 for(0;_tmp1CB != 0;_tmp1CB=_tmp1CB->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1CB->hd);}
return;case 19U: _LL2D: _tmp1CC=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp15B)->f1;_LL2E:
# 819
 _tmp1CD=_tmp1CC;goto _LL30;case 23U: _LL2F: _tmp1CD=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp15B)->f1;_LL30:
 _tmp1CE=_tmp1CD;goto _LL32;case 25U: _LL31: _tmp1CE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp15B)->f1;_LL32:
 _tmp1CF=_tmp1CE;goto _LL34;case 15U: _LL33: _tmp1CF=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15B)->f1;_LL34:
 Cyc_Binding_resolve_type(loc,env,_tmp1CF);return;case 16U: _LL35: _tmp1D1=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp15B)->f1;_tmp1D0=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp15B)->f2;_LL36:
# 825
 Cyc_Binding_resolve_type(loc,env,_tmp1D1);Cyc_Binding_resolve_type(loc,env,_tmp1D0);return;case 18U: _LL37: _tmp1D2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp15B)->f1;_LL38:
# 827
 _tmp1D3=_tmp1D2;goto _LL3A;default: _LL39: _tmp1D3=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp15B)->f1;_LL3A:
 Cyc_Binding_resolve_exp(env,_tmp1D3);return;}_LL0:;}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 832
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1D4=p;void*_tmp223;unsigned int _tmp222;_LL1: _tmp223=_tmp1D4->r;_tmp222=_tmp1D4->loc;_LL2:;{
void*_tmp1D5=_tmp223;struct Cyc_List_List*_tmp221;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_List_List*_tmp21F;struct Cyc_Absyn_Pat*_tmp21E;struct Cyc_Absyn_Vardecl*_tmp21D;struct Cyc_Absyn_Vardecl*_tmp21C;struct Cyc_Absyn_Vardecl*_tmp21B;struct Cyc_Absyn_Pat*_tmp21A;struct Cyc_Absyn_Vardecl*_tmp219;struct Cyc_Absyn_Pat*_tmp218;struct Cyc_List_List*_tmp217;struct Cyc_List_List*_tmp216;struct _tuple0*_tmp215;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp213;struct Cyc_List_List*_tmp212;int _tmp211;struct _tuple0*_tmp210;struct Cyc_List_List*_tmp20F;int _tmp20E;struct _tuple0*_tmp20D;switch(*((int*)_tmp1D5)){case 15U: _LL4: _tmp20D=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL5:
# 836
{struct _handler_cons _tmp1D6;_push_handler(& _tmp1D6);{int _tmp1D8=0;if(setjmp(_tmp1D6.handler))_tmp1D8=1;if(!_tmp1D8){{void*_tmp1D9=Cyc_Binding_lookup_ordinary(_tmp222,env,_tmp20D);void*_tmp1DA=_tmp1D9;void*_tmp1E5;struct Cyc_Absyn_Enumfield*_tmp1E4;struct Cyc_Absyn_Enumdecl*_tmp1E3;struct Cyc_Absyn_Enumfield*_tmp1E2;struct Cyc_Absyn_Datatypedecl*_tmp1E1;struct Cyc_Absyn_Datatypefield*_tmp1E0;switch(*((int*)_tmp1DA)){case 0U: _LL2D: _LL2E:
 goto _LL2C;case 1U: _LL2F: _LL30:
# 839
({void*_tmp1DB=0U;({unsigned int _tmp344=_tmp222;struct _dyneither_ptr _tmp343=({const char*_tmp1DC="struct tag used without arguments in pattern";_tag_dyneither(_tmp1DC,sizeof(char),45U);});Cyc_Warn_err(_tmp344,_tmp343,_tag_dyneither(_tmp1DB,sizeof(void*),0U));});});
_npop_handler(0U);return;case 2U: _LL31: _tmp1E1=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1DA)->f1;_tmp1E0=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1DA)->f2;_LL32:
# 842
({void*_tmp345=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->tag=8U,_tmp1DD->f1=_tmp1E1,_tmp1DD->f2=_tmp1E0,_tmp1DD->f3=0,_tmp1DD->f4=0;_tmp1DD;});p->r=_tmp345;});_npop_handler(0U);return;case 3U: _LL33: _tmp1E3=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1DA)->f1;_tmp1E2=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1DA)->f2;_LL34:
# 844
({void*_tmp346=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->tag=13U,_tmp1DE->f1=_tmp1E3,_tmp1DE->f2=_tmp1E2;_tmp1DE;});p->r=_tmp346;});_npop_handler(0U);return;default: _LL35: _tmp1E5=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1DA)->f1;_tmp1E4=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1DA)->f2;_LL36:
# 846
({void*_tmp347=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF->tag=14U,_tmp1DF->f1=_tmp1E5,_tmp1DF->f2=_tmp1E4;_tmp1DF;});p->r=_tmp347;});_npop_handler(0U);return;}_LL2C:;}
# 836
;_pop_handler();}else{void*_tmp1D7=(void*)_exn_thrown;void*_tmp1E6=_tmp1D7;void*_tmp1E7;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1E6)->tag == Cyc_Binding_BindingError){_LL38: _LL39:
# 847
 goto _LL37;}else{_LL3A: _tmp1E7=_tmp1E6;_LL3B:(int)_rethrow(_tmp1E7);}_LL37:;}};}{
struct Cyc_Absyn_Vardecl*_tmp1E8=Cyc_Absyn_new_vardecl(0U,_tmp20D,(void*)& Cyc_Absyn_VoidType_val,0);
Cyc_Binding_resolve_and_add_vardecl(_tmp222,env,_tmp1E8);
({void*_tmp349=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->tag=1U,_tmp1E9->f1=_tmp1E8,({struct Cyc_Absyn_Pat*_tmp348=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp1E9->f2=_tmp348;});_tmp1E9;});p->r=_tmp349;});
return;};case 16U: _LL6: _tmp210=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp20F=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_tmp20E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_LL7:
# 854
{struct Cyc_List_List*_tmp1EA=_tmp20F;for(0;_tmp1EA != 0;_tmp1EA=_tmp1EA->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp1EA->hd);}}
{struct _handler_cons _tmp1EB;_push_handler(& _tmp1EB);{int _tmp1ED=0;if(setjmp(_tmp1EB.handler))_tmp1ED=1;if(!_tmp1ED){{void*_tmp1EE=Cyc_Binding_lookup_ordinary(_tmp222,env,_tmp210);void*_tmp1EF=_tmp1EE;struct Cyc_Absyn_Datatypedecl*_tmp1FB;struct Cyc_Absyn_Datatypefield*_tmp1FA;struct Cyc_Absyn_Aggrdecl*_tmp1F9;switch(*((int*)_tmp1EF)){case 0U: _LL3D: _LL3E:
 goto _LL3C;case 1U: _LL3F: _tmp1F9=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp1EF)->f1;_LL40: {
# 859
struct Cyc_List_List*_tmp1F0=0;
for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){
({struct Cyc_List_List*_tmp34B=({struct Cyc_List_List*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));({struct _tuple13*_tmp34A=({struct _tuple13*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->f1=0,_tmp1F1->f2=(struct Cyc_Absyn_Pat*)_tmp20F->hd;_tmp1F1;});_tmp1F2->hd=_tmp34A;}),_tmp1F2->tl=_tmp1F0;_tmp1F2;});_tmp1F0=_tmp34B;});}
({void*_tmp34F=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->tag=7U,({struct Cyc_Absyn_AggrInfo*_tmp34E=({struct Cyc_Absyn_AggrInfo*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));({union Cyc_Absyn_AggrInfoU _tmp34D=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp1F3=_cycalloc(sizeof(*_tmp1F3));*_tmp1F3=_tmp1F9;_tmp1F3;}));_tmp1F4->aggr_info=_tmp34D;}),_tmp1F4->targs=0;_tmp1F4;});_tmp1F5->f1=_tmp34E;}),_tmp1F5->f2=0,({
struct Cyc_List_List*_tmp34C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1F0);_tmp1F5->f3=_tmp34C;}),_tmp1F5->f4=_tmp20E;_tmp1F5;});
# 862
p->r=_tmp34F;});
# 864
_npop_handler(0U);return;}case 2U: _LL41: _tmp1FB=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1EF)->f1;_tmp1FA=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1EF)->f2;_LL42:
# 866
({void*_tmp350=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6->tag=8U,_tmp1F6->f1=_tmp1FB,_tmp1F6->f2=_tmp1FA,_tmp1F6->f3=_tmp20F,_tmp1F6->f4=_tmp20E;_tmp1F6;});p->r=_tmp350;});_npop_handler(0U);return;case 3U: _LL43: _LL44:
 goto _LL46;default: _LL45: _LL46:
# 869
({void*_tmp1F7=0U;({unsigned int _tmp352=_tmp222;struct _dyneither_ptr _tmp351=({const char*_tmp1F8="enum tag used with arguments in pattern";_tag_dyneither(_tmp1F8,sizeof(char),40U);});Cyc_Warn_err(_tmp352,_tmp351,_tag_dyneither(_tmp1F7,sizeof(void*),0U));});});_npop_handler(0U);return;}_LL3C:;}
# 856
;_pop_handler();}else{void*_tmp1EC=(void*)_exn_thrown;void*_tmp1FC=_tmp1EC;void*_tmp1FD;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1FC)->tag == Cyc_Binding_BindingError){_LL48: _LL49:
# 870
 goto _LL47;}else{_LL4A: _tmp1FD=_tmp1FC;_LL4B:(int)_rethrow(_tmp1FD);}_LL47:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp200=({struct Cyc_String_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=0U,({struct _dyneither_ptr _tmp353=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp210));_tmp2B0.f1=_tmp353;});_tmp2B0;});void*_tmp1FE[1U];_tmp1FE[0]=& _tmp200;({unsigned int _tmp355=_tmp222;struct _dyneither_ptr _tmp354=({const char*_tmp1FF="%s is not a constructor in pattern";_tag_dyneither(_tmp1FF,sizeof(char),35U);});Cyc_Warn_err(_tmp355,_tmp354,_tag_dyneither(_tmp1FE,sizeof(void*),1U));});});
return;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1)->aggr_info).UnknownAggr).tag == 1){_LL8: _tmp215=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1)->aggr_info).UnknownAggr).val).f2;_tmp214=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1)->targs;_tmp213=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_tmp212=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_tmp211=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f4;_LL9:
# 875
{struct Cyc_List_List*_tmp201=_tmp212;for(0;_tmp201 != 0;_tmp201=_tmp201->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp201->hd)).f2);}}
{struct _handler_cons _tmp202;_push_handler(& _tmp202);{int _tmp204=0;if(setjmp(_tmp202.handler))_tmp204=1;if(!_tmp204){
{struct Cyc_Absyn_Aggrdecl*_tmp205=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(_tmp222,env->ns,_tmp215,Cyc_Binding_lookup_aggrdecl);
({void*_tmp358=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208->tag=7U,({struct Cyc_Absyn_AggrInfo*_tmp357=({struct Cyc_Absyn_AggrInfo*_tmp207=_cycalloc(sizeof(*_tmp207));({union Cyc_Absyn_AggrInfoU _tmp356=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp206=_cycalloc(sizeof(*_tmp206));*_tmp206=_tmp205;_tmp206;}));_tmp207->aggr_info=_tmp356;}),_tmp207->targs=_tmp214;_tmp207;});_tmp208->f1=_tmp357;}),_tmp208->f2=_tmp213,_tmp208->f3=_tmp212,_tmp208->f4=_tmp211;_tmp208;});p->r=_tmp358;});}
# 878
;_pop_handler();}else{void*_tmp203=(void*)_exn_thrown;void*_tmp209=_tmp203;void*_tmp20C;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp209)->tag == Cyc_Binding_BindingError){_LL4D: _LL4E:
# 882
({void*_tmp20A=0U;({unsigned int _tmp35A=_tmp222;struct _dyneither_ptr _tmp359=({const char*_tmp20B="non-aggregate name has designator patterns";_tag_dyneither(_tmp20B,sizeof(char),43U);});Cyc_Warn_err(_tmp35A,_tmp359,_tag_dyneither(_tmp20A,sizeof(void*),0U));});});
return;}else{_LL4F: _tmp20C=_tmp209;_LL50:(int)_rethrow(_tmp20C);}_LL4C:;}};}
# 885
return;}else{_LL24: _tmp216=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_LL25:
# 914
 for(0;_tmp216 != 0;_tmp216=_tmp216->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)_tmp216->hd)).f2);}
return;}}else{_LL22: _tmp217=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_LL23:
# 912
 _tmp216=_tmp217;goto _LL25;}case 0U: _LLA: _LLB:
# 887
 return;case 3U: _LLC: _tmp219=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp218=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LLD:
# 889
 _tmp21B=_tmp219;_tmp21A=_tmp218;goto _LLF;case 1U: _LLE: _tmp21B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp21A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LLF:
# 891
 Cyc_Binding_resolve_pat(env,_tmp21A);
_tmp21C=_tmp21B;goto _LL11;case 4U: _LL10: _tmp21C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL11:
 _tmp21D=_tmp21C;goto _LL13;case 2U: _LL12: _tmp21D=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL13:
# 895
 Cyc_Binding_resolve_and_add_vardecl(_tmp222,env,_tmp21D);goto _LL3;case 6U: _LL14: _tmp21E=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL15:
# 897
 Cyc_Binding_resolve_pat(env,_tmp21E);return;case 5U: _LL16: _tmp21F=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL17:
# 899
 for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp21F->hd);}
goto _LL3;case 17U: _LL18: _tmp220=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL19:
# 903
 Cyc_Binding_resolve_exp(env,_tmp220);return;case 9U: _LL1A: _LL1B:
# 905
 goto _LL1D;case 10U: _LL1C: _LL1D:
 goto _LL1F;case 11U: _LL1E: _LL1F:
 goto _LL21;case 12U: _LL20: _LL21:
 return;case 8U: _LL26: _tmp221=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_LL27:
# 918
 for(0;_tmp221 != 0;_tmp221=_tmp221->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp221->hd);}
return;case 13U: _LL28: _LL29:
 goto _LL2B;default: _LL2A: _LL2B:
 return;}_LL3:;};}struct _tuple14{void*f1;void*f2;};
# 926
void Cyc_Binding_resolve_rgnpo(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f2);}}struct _tuple15{unsigned int f1;struct _tuple0*f2;int f3;};
# 940 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 943
unsigned int loc=d->loc;
void*_tmp224=d->r;void*_tmp225=_tmp224;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*_tmp299;struct _tuple0*_tmp298;struct Cyc_List_List*_tmp297;struct _dyneither_ptr*_tmp296;struct Cyc_List_List*_tmp295;struct Cyc_Absyn_Datatypedecl*_tmp294;struct Cyc_Absyn_Enumdecl*_tmp293;struct Cyc_Absyn_Aggrdecl*_tmp292;struct Cyc_Absyn_Typedefdecl*_tmp291;struct Cyc_Absyn_Tvar*_tmp290;struct Cyc_Absyn_Vardecl*_tmp28F;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_List_List*_tmp28D;struct Cyc_Absyn_Pat*_tmp28C;struct Cyc_Absyn_Exp*_tmp28B;struct Cyc_Absyn_Fndecl*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp289;switch(*((int*)_tmp225)){case 0U: _LL1: _tmp289=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_LL2:
# 947
 Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp289);
# 952
if(_tmp289->initializer != 0  && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp289->initializer));
goto _LL0;case 1U: _LL3: _tmp28A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_LL4:
# 957
 Cyc_Binding_absolutize_decl(loc,env,_tmp28A->name,_tmp28A->sc);{
struct _tuple0*_tmp226=_tmp28A->name;struct _tuple0*_tmp227=_tmp226;union Cyc_Absyn_Nmspace _tmp238;struct _dyneither_ptr*_tmp237;_LL20: _tmp238=_tmp227->f1;_tmp237=_tmp227->f2;_LL21:;{
# 960
struct Cyc_List_List*_tmp228=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai))Cyc_Binding_get_fun_vardecls)(Cyc_Binding_get_vds_fndecl,1,loc,env,_tmp28A->args,_tmp28A->cyc_varargs);
# 962
({struct Cyc_Core_Opt*_tmp35B=({struct Cyc_Core_Opt*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->v=_tmp228;_tmp229;});_tmp28A->param_vardecls=_tmp35B;});
# 964
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,_tmp28A->name);{
# 967
struct Cyc_Dict_Dict*_tmp22A=env->local_vars;
if(_tmp22A != 0)
({struct Cyc_Dict_Dict*_tmp35C=({struct Cyc_Dict_Dict*_tmp22B=_cycalloc(sizeof(*_tmp22B));*_tmp22B=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp22B;});env->local_vars=_tmp35C;});else{
# 971
({struct Cyc_Dict_Dict*_tmp35E=({struct Cyc_Dict_Dict*_tmp22C=_cycalloc(sizeof(*_tmp22C));({struct Cyc_Dict_Dict _tmp35D=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp22C=_tmp35D;});_tmp22C;});env->local_vars=_tmp35E;});}
{struct Cyc_List_List*_tmp22D=_tmp228;for(0;_tmp22D != 0;_tmp22D=_tmp22D->tl){
({struct Cyc_Dict_Dict _tmp362=({struct Cyc_Dict_Dict _tmp361=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp360=(*((struct Cyc_Absyn_Vardecl*)_tmp22D->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp361,_tmp360,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp22F=_cycalloc(sizeof(*_tmp22F));
_tmp22F->tag=0U,({void*_tmp35F=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->tag=3U,_tmp22E->f1=(struct Cyc_Absyn_Vardecl*)_tmp22D->hd;_tmp22E;});_tmp22F->f1=_tmp35F;});_tmp22F;}));});
# 973
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp362;});}}
# 976
((void(*)(unsigned int loc,struct Cyc_Binding_Env*env,void*eff,void*rettyp,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai,struct Cyc_List_List*rpo,struct Cyc_Absyn_Exp*req_clause,struct Cyc_Absyn_Exp*ens_clause))Cyc_Binding_resolve_function_stuff)(loc,env,_tmp28A->effect,_tmp28A->ret_type,_tmp28A->args,_tmp28A->cyc_varargs,_tmp28A->rgn_po,_tmp28A->requires_clause,_tmp28A->ensures_clause);
# 980
if(_tmp22A != 0){
# 982
({struct Cyc_Dict_Dict _tmp366=({struct Cyc_Dict_Dict _tmp365=*_tmp22A;struct _dyneither_ptr*_tmp364=_tmp237;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp365,_tmp364,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp231=_cycalloc(sizeof(*_tmp231));
_tmp231->tag=0U,({void*_tmp363=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230->tag=2U,_tmp230->f1=_tmp28A;_tmp230;});_tmp231->f1=_tmp363;});_tmp231;}));});
# 982
*_tmp22A=_tmp366;});
# 984
({struct Cyc_Dict_Dict _tmp36A=({struct Cyc_Dict_Dict _tmp369=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp368=_tmp237;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp369,_tmp368,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp233=_cycalloc(sizeof(*_tmp233));
_tmp233->tag=0U,({void*_tmp367=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->tag=2U,_tmp232->f1=_tmp28A;_tmp232;});_tmp233->f1=_tmp367;});_tmp233;}));});
# 984
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp36A;});}else{
# 987
struct Cyc_Binding_ResolveNSEnv*_tmp234=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp238);
({struct Cyc_Dict_Dict _tmp36E=({struct Cyc_Dict_Dict _tmp36D=_tmp234->ordinaries;struct _dyneither_ptr*_tmp36C=_tmp237;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp36D,_tmp36C,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp236=_cycalloc(sizeof(*_tmp236));
# 990
_tmp236->tag=0U,({void*_tmp36B=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp235=_cycalloc(sizeof(*_tmp235));_tmp235->tag=2U,_tmp235->f1=_tmp28A;_tmp235;});_tmp236->f1=_tmp36B;});_tmp236;}));});
# 988
_tmp234->ordinaries=_tmp36E;});}
# 993
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,_tmp28A->body);
# 996
env->local_vars=_tmp22A;
goto _LL0;};};};case 2U: _LL5: _tmp28C=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_tmp28B=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp225)->f3;_LL6:
# 1000
 if(Cyc_Binding_at_toplevel(env)){
({void*_tmp239=0U;({unsigned int _tmp370=loc;struct _dyneither_ptr _tmp36F=({const char*_tmp23A="let not allowed at top-level";_tag_dyneither(_tmp23A,sizeof(char),29U);});Cyc_Warn_err(_tmp370,_tmp36F,_tag_dyneither(_tmp239,sizeof(void*),0U));});});
goto _LL0;}
# 1004
Cyc_Binding_resolve_exp(env,_tmp28B);
Cyc_Binding_resolve_pat(env,_tmp28C);
goto _LL0;case 3U: _LL7: _tmp28D=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_LL8:
# 1009
 for(0;_tmp28D != 0;_tmp28D=_tmp28D->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)_tmp28D->hd);}
goto _LL0;case 4U: _LL9: _tmp290=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_tmp28F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp225)->f2;_tmp28E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp225)->f3;_LLA:
# 1014
 if(_tmp28E != 0)
Cyc_Binding_resolve_exp(env,_tmp28E);
Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp28F);
goto _LL0;case 8U: _LLB: _tmp291=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_LLC:
# 1020
 Cyc_Binding_absolutize_decl(loc,env,_tmp291->name,_tmp291->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp23B=_tmp291->name;struct _tuple0*_tmp23C=_tmp23B;union Cyc_Absyn_Nmspace _tmp23F;struct _dyneither_ptr*_tmp23E;_LL23: _tmp23F=_tmp23C->f1;_tmp23E=_tmp23C->f2;_LL24:;{
# 1023
struct Cyc_Binding_ResolveNSEnv*_tmp23D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp23F);
if(_tmp291->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp291->defn));
# 1027
({struct Cyc_Dict_Dict _tmp371=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp23D->typedefs,_tmp23E,_tmp291);_tmp23D->typedefs=_tmp371;});
# 1029
goto _LL0;};};case 5U: _LLD: _tmp292=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_LLE:
# 1032
 Cyc_Binding_absolutize_decl(loc,env,_tmp292->name,_tmp292->sc);{
struct _tuple0*_tmp240=_tmp292->name;struct _tuple0*_tmp241=_tmp240;union Cyc_Absyn_Nmspace _tmp249;struct _dyneither_ptr*_tmp248;_LL26: _tmp249=_tmp241->f1;_tmp248=_tmp241->f2;_LL27:;{
# 1035
struct Cyc_Binding_ResolveNSEnv*_tmp242=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp249);
# 1037
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp242->aggrdecls,_tmp248) && _tmp292->impl == 0)
goto _LL0;
# 1040
({struct Cyc_Dict_Dict _tmp372=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp242->aggrdecls,_tmp248,_tmp292);_tmp242->aggrdecls=_tmp372;});
# 1042
({struct Cyc_Dict_Dict _tmp375=({struct Cyc_Dict_Dict _tmp374=_tmp242->ordinaries;struct _dyneither_ptr*_tmp373=_tmp248;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp374,_tmp373,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp243=_cycalloc(sizeof(*_tmp243));
_tmp243->tag=1U,_tmp243->f1=_tmp292;_tmp243;}));});
# 1042
_tmp242->ordinaries=_tmp375;});
# 1044
if(_tmp292->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp244=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp292->impl);struct Cyc_Absyn_AggrdeclImpl*_tmp245=_tmp244;struct Cyc_List_List*_tmp247;struct Cyc_List_List*_tmp246;_LL29: _tmp247=_tmp245->rgn_po;_tmp246=_tmp245->fields;_LL2A:;
Cyc_Binding_resolve_rgnpo(loc,env,_tmp247);
Cyc_Binding_resolve_aggrfields(loc,env,_tmp246);}
# 1049
goto _LL0;};};case 7U: _LLF: _tmp293=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_LL10:
# 1052
 Cyc_Binding_absolutize_decl(loc,env,_tmp293->name,_tmp293->sc);{
struct _tuple0*_tmp24A=_tmp293->name;struct _tuple0*_tmp24B=_tmp24A;union Cyc_Absyn_Nmspace _tmp250;struct _dyneither_ptr*_tmp24F;_LL2C: _tmp250=_tmp24B->f1;_tmp24F=_tmp24B->f2;_LL2D:;{
# 1055
struct Cyc_Binding_ResolveNSEnv*_tmp24C=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp250);
# 1057
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp24C->enumdecls,_tmp24F) && _tmp293->fields == 0)
goto _LL0;
# 1061
({struct Cyc_Dict_Dict _tmp376=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp24C->enumdecls,_tmp24F,_tmp293);_tmp24C->enumdecls=_tmp376;});
# 1063
if(_tmp293->fields != 0){
# 1065
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp293->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp24D=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(_tmp24D->loc,env,_tmp24D->name,_tmp293->sc);
if(_tmp24D->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp24D->tag));
({struct Cyc_Dict_Dict _tmp379=({struct Cyc_Dict_Dict _tmp378=_tmp24C->ordinaries;struct _dyneither_ptr*_tmp377=(*_tmp24D->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp378,_tmp377,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp24E=_cycalloc(sizeof(*_tmp24E));
# 1072
_tmp24E->tag=3U,_tmp24E->f1=_tmp293,_tmp24E->f2=_tmp24D;_tmp24E;}));});
# 1070
_tmp24C->ordinaries=_tmp379;});}}
# 1074
goto _LL0;};};case 6U: _LL11: _tmp294=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_LL12:
# 1078
{struct _handler_cons _tmp251;_push_handler(& _tmp251);{int _tmp253=0;if(setjmp(_tmp251.handler))_tmp253=1;if(!_tmp253){
{struct Cyc_List_List*_tmp254=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp294->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*_tmp255=(struct Cyc_Absyn_Datatypedecl*)_tmp254->hd;
if(!_tmp255->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->tag=Cyc_Binding_BindingError;_tmp256;}));
_tmp294->name=_tmp255->name;
_tmp294->is_extensible=1;{
struct _tuple0*_tmp257=_tmp294->name;struct _tuple0*_tmp258=_tmp257;union Cyc_Absyn_Nmspace _tmp25C;struct _dyneither_ptr*_tmp25B;_LL2F: _tmp25C=_tmp258->f1;_tmp25B=_tmp258->f2;_LL30:;{
struct Cyc_Binding_ResolveNSEnv*_tmp259=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp25C);
({struct Cyc_Dict_Dict _tmp37C=({struct Cyc_Dict_Dict _tmp37B=_tmp259->datatypedecls;struct _dyneither_ptr*_tmp37A=_tmp25B;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp37B,_tmp37A,({struct Cyc_List_List*_tmp25A=_cycalloc(sizeof(*_tmp25A));
_tmp25A->hd=_tmp294,_tmp25A->tl=_tmp254;_tmp25A;}));});
# 1087
_tmp259->datatypedecls=_tmp37C;});};};}
# 1079
;_pop_handler();}else{void*_tmp252=(void*)_exn_thrown;void*_tmp25D=_tmp252;void*_tmp264;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp25D)->tag == Cyc_Binding_BindingError){_LL32: _LL33:
# 1091
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp294->name,_tmp294->sc);{
struct _tuple0*_tmp25E=_tmp294->name;struct _tuple0*_tmp25F=_tmp25E;union Cyc_Absyn_Nmspace _tmp263;struct _dyneither_ptr*_tmp262;_LL37: _tmp263=_tmp25F->f1;_tmp262=_tmp25F->f2;_LL38:;{
struct Cyc_Binding_ResolveNSEnv*_tmp260=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp263);
# 1095
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp260->datatypedecls,_tmp262) && _tmp294->fields == 0)
# 1097
goto _LL31;
({struct Cyc_Dict_Dict _tmp37F=({struct Cyc_Dict_Dict _tmp37E=_tmp260->datatypedecls;struct _dyneither_ptr*_tmp37D=_tmp262;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp37E,_tmp37D,({struct Cyc_List_List*_tmp261=_cycalloc(sizeof(*_tmp261));
_tmp261->hd=_tmp294,_tmp261->tl=0;_tmp261;}));});
# 1098
_tmp260->datatypedecls=_tmp37F;});
# 1100
goto _LL31;};};}else{_LL34: _tmp264=_tmp25D;_LL35:(int)_rethrow(_tmp264);}_LL31:;}};}{
# 1103
struct _tuple0*_tmp265=_tmp294->name;struct _tuple0*_tmp266=_tmp265;union Cyc_Absyn_Nmspace _tmp273;struct _dyneither_ptr*_tmp272;_LL3A: _tmp273=_tmp266->f1;_tmp272=_tmp266->f2;_LL3B:;
if(_tmp294->fields != 0){
struct Cyc_List_List*_tmp267=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp294->fields))->v;for(0;_tmp267 != 0;_tmp267=_tmp267->tl){
struct Cyc_Absyn_Datatypefield*_tmp268=(struct Cyc_Absyn_Datatypefield*)_tmp267->hd;
{struct Cyc_List_List*_tmp269=_tmp268->typs;for(0;_tmp269 != 0;_tmp269=_tmp269->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)_tmp269->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp26A=(*_tmp268->name).f1;union Cyc_Absyn_Nmspace _tmp26B=_tmp26A;switch((_tmp26B.Abs_n).tag){case 1U: if((_tmp26B.Rel_n).val == 0){_LL3D: _LL3E:
# 1111
 if(_tmp294->is_extensible)
({union Cyc_Absyn_Nmspace _tmp380=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);(*_tmp268->name).f1=_tmp380;});else{
# 1114
(*_tmp268->name).f1=(*_tmp294->name).f1;}
goto _LL3C;}else{_LL3F: _LL40:
# 1117
({void*_tmp26C=0U;({unsigned int _tmp382=_tmp268->loc;struct _dyneither_ptr _tmp381=({const char*_tmp26D="qualified datatype field declaratations are not allowed";_tag_dyneither(_tmp26D,sizeof(char),56U);});Cyc_Warn_err(_tmp382,_tmp381,_tag_dyneither(_tmp26C,sizeof(void*),0U));});});
return;}case 2U: _LL41: _LL42:
 goto _LL3C;default: _LL43: _LL44:
({void*_tmp26E=0U;({struct _dyneither_ptr _tmp383=({const char*_tmp26F="datatype field Loc_n or C_n";_tag_dyneither(_tmp26F,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp383,_tag_dyneither(_tmp26E,sizeof(void*),0U));});});}_LL3C:;}{
# 1122
struct Cyc_Binding_ResolveNSEnv*_tmp270=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*_tmp268->name).f1);
({struct Cyc_Dict_Dict _tmp386=({struct Cyc_Dict_Dict _tmp385=_tmp270->ordinaries;struct _dyneither_ptr*_tmp384=(*_tmp268->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp385,_tmp384,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp271=_cycalloc(sizeof(*_tmp271));
# 1125
_tmp271->tag=2U,_tmp271->f1=_tmp294,_tmp271->f2=_tmp268;_tmp271;}));});
# 1123
_tmp270->ordinaries=_tmp386;});};}}
# 1127
goto _LL0;};case 9U: _LL13: _tmp296=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_tmp295=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp225)->f2;_LL14:
# 1130
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,_tmp296,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,_tmp295);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL0;case 10U: _LL15: _tmp298=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_tmp297=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp225)->f2;_LL16:
# 1135
((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,_tmp298);
Cyc_Binding_resolve_decls(env,_tmp297);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL0;case 11U: _LL17: _tmp299=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_LL18: {
# 1141
int _tmp274=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp299);
env->in_cinclude=_tmp274;
goto _LL0;}case 12U: _LL19: _tmp29B=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp225)->f1;_tmp29A=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp225)->f2;_LL1A:
# 1152
{struct Cyc_List_List*_tmp275=_tmp29A;for(0;_tmp275 != 0;_tmp275=_tmp275->tl){
struct _tuple15*_tmp276=(struct _tuple15*)_tmp275->hd;struct _tuple15*_tmp277=_tmp276;unsigned int _tmp279;struct _tuple0*_tmp278;_LL46: _tmp279=_tmp277->f1;_tmp278=_tmp277->f2;_LL47:;
Cyc_Binding_absolutize_decl(_tmp279,env,_tmp278,Cyc_Absyn_ExternC);}}
# 1160
if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp27A=0U;({unsigned int _tmp388=loc;struct _dyneither_ptr _tmp387=({const char*_tmp27B="extern \"C include\" not at toplevel";_tag_dyneither(_tmp27B,sizeof(char),35U);});Cyc_Warn_err(_tmp388,_tmp387,_tag_dyneither(_tmp27A,sizeof(void*),0U));});});
goto _LL0;}{
# 1165
struct Cyc_Binding_ResolveNSEnv*_tmp27C=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);
struct Cyc_Dict_Dict _tmp27D=_tmp27C->ordinaries;
int _tmp27E=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp29B);{
struct Cyc_Dict_Dict _tmp27F=_tmp27C->ordinaries;
struct Cyc_Dict_Dict _tmp280=_tmp27D;
{struct Cyc_List_List*_tmp281=_tmp29A;for(0;_tmp281 != 0;_tmp281=_tmp281->tl){
struct _tuple15*_tmp282=(struct _tuple15*)_tmp281->hd;struct _tuple15*_tmp283=_tmp282;unsigned int _tmp288;struct _dyneither_ptr*_tmp287;_LL49: _tmp288=_tmp283->f1;_tmp287=(_tmp283->f2)->f2;_LL4A:;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp27F,_tmp287) || 
((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp27D,_tmp287) && ({
void*_tmp389=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27D,_tmp287);_tmp389 == ((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27F,_tmp287);}))
({struct Cyc_String_pa_PrintArg_struct _tmp286=({struct Cyc_String_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=0U,_tmp2B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp287);_tmp2B1;});void*_tmp284[1U];_tmp284[0]=& _tmp286;({unsigned int _tmp38B=_tmp288;struct _dyneither_ptr _tmp38A=({const char*_tmp285="%s is exported but not defined";_tag_dyneither(_tmp285,sizeof(char),31U);});Cyc_Warn_err(_tmp38B,_tmp38A,_tag_dyneither(_tmp284,sizeof(void*),1U));});});
({struct Cyc_Dict_Dict _tmp38E=({struct Cyc_Dict_Dict _tmp38D=_tmp280;struct _dyneither_ptr*_tmp38C=_tmp287;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp38D,_tmp38C,((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27F,_tmp287));});_tmp280=_tmp38E;});}}
# 1180
_tmp27C->ordinaries=_tmp280;
env->in_cinclude=_tmp27E;
goto _LL0;};};case 13U: _LL1B: _LL1C:
# 1184
 goto _LL1E;default: _LL1D: _LL1E:
 goto _LL0;}_LL0:;}
# 1189
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1194
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp29C=({struct Cyc_Binding_Env*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->in_cinclude=0,({struct Cyc_Binding_NSCtxt*_tmp38F=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp29F->ns=_tmp38F;}),_tmp29F->local_vars=0;_tmp29F;});
({struct Cyc_Binding_Env*_tmp391=_tmp29C;Cyc_Binding_resolve_decl(_tmp391,({struct Cyc_Absyn_Decl*_tmp29E=_cycalloc(sizeof(*_tmp29E));({void*_tmp390=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->tag=6U,_tmp29D->f1=Cyc_Absyn_exn_tud;_tmp29D;});_tmp29E->r=_tmp390;}),_tmp29E->loc=0U;_tmp29E;}));});
Cyc_Binding_resolve_decls(_tmp29C,tds);}

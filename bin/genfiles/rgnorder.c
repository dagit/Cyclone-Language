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
#endif

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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};
# 231 "dict.h"
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 287
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 429
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 954
extern void*Cyc_Absyn_uint_typ;
# 960
extern void*Cyc_Absyn_empty_effect;struct Cyc_RgnOrder_RgnPO;
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 98
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 147
int Cyc_Tcutil_unify(void*,void*);
# 291 "tcutil.h"
void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_RgnOrder_RgnPO{struct Cyc_Dict_Dict d;void*these_outlive_heap;void*these_outlive_unique;struct Cyc_Absyn_Tvar*youngest;void*opened_regions;};
# 65 "rgnorder.cyc"
static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn){
struct Cyc_Absyn_Kind*_tmp0=Cyc_Tcutil_typ_kind(rgn);struct Cyc_Absyn_Kind*_tmp1=_tmp0;enum Cyc_Absyn_KindQual _tmp16;enum Cyc_Absyn_AliasQual _tmp15;_LL1: _tmp16=_tmp1->kind;_tmp15=_tmp1->aliasqual;_LL2:;
if(_tmp16 != Cyc_Absyn_RgnKind)
({struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmpB8;_tmpB8.tag=0U,({
struct _dyneither_ptr _tmpC0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));_tmpB8.f1=_tmpC0;});_tmpB8;});void*_tmp2[1U];_tmp2[0]=& _tmp4;({struct _dyneither_ptr _tmpC1=({const char*_tmp3="bad region type |%s| passed to add_outlives_constraint";_tag_dyneither(_tmp3,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC1,_tag_dyneither(_tmp2,sizeof(void*),1U));});});{
void*_tmp5=Cyc_Tcutil_compress(eff);void*_tmp6=_tmp5;void*_tmp14;void*_tmp13;struct Cyc_List_List*_tmp12;switch(*((int*)_tmp6)){case 24U: _LL4: _tmp12=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6)->f1;_LL5:
# 72
 for(0;_tmp12 != 0;_tmp12=_tmp12->tl){
if(!Cyc_RgnOrder_valid_constraint((void*)_tmp12->hd,rgn))return 0;}
return 1;case 1U: _LL6: _LL7:
# 76
 goto _LL9;case 2U: _LL8: _LL9:
 return 1;case 25U: _LLA: _tmp13=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6)->f1;_LLB: {
# 81
struct Cyc_Absyn_Kind*_tmp7=Cyc_Tcutil_typ_kind(_tmp13);struct Cyc_Absyn_Kind*_tmp8=_tmp7;enum Cyc_Absyn_KindQual _tmpA;enum Cyc_Absyn_AliasQual _tmp9;_LL11: _tmpA=_tmp8->kind;_tmp9=_tmp8->aliasqual;_LL12:;
return _tmp15 == Cyc_Absyn_Aliasable  || _tmp9 == _tmp15;}case 23U: _LLC: _tmp14=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6)->f1;_LLD: {
# 86
struct Cyc_Absyn_Kind*_tmpB=Cyc_Tcutil_typ_kind(_tmp14);struct Cyc_Absyn_Kind*_tmpC=_tmpB;enum Cyc_Absyn_KindQual _tmpE;enum Cyc_Absyn_AliasQual _tmpD;_LL14: _tmpE=_tmpC->kind;_tmpD=_tmpC->aliasqual;_LL15:;
return _tmp16 == Cyc_Absyn_RgnKind  && (_tmp15 == Cyc_Absyn_Aliasable  || _tmpD == _tmp15);}default: _LLE: _LLF:
# 89
({struct Cyc_String_pa_PrintArg_struct _tmp11=({struct Cyc_String_pa_PrintArg_struct _tmpB9;_tmpB9.tag=0U,({
struct _dyneither_ptr _tmpC2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(eff));_tmpB9.f1=_tmpC2;});_tmpB9;});void*_tmpF[1U];_tmpF[0]=& _tmp11;({struct _dyneither_ptr _tmpC3=({const char*_tmp10="bad effect |%s| passed to add_outlives_constraint";_tag_dyneither(_tmp10,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC3,_tag_dyneither(_tmpF,sizeof(void*),1U));});});}_LL3:;};}
# 97
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc){
# 99
({void*_tmpC4=Cyc_Tcutil_normalize_effect(eff);eff=_tmpC4;});{
struct Cyc_RgnOrder_RgnPO*_tmp17=({struct Cyc_RgnOrder_RgnPO*_tmp29=_cycalloc(sizeof(*_tmp29));*_tmp29=*po;_tmp29;});
# 102
if(!Cyc_RgnOrder_valid_constraint(eff,rgn)){
({void*_tmp18=0U;({unsigned int _tmpC6=loc;struct _dyneither_ptr _tmpC5=({const char*_tmp19="Invalid region ordering constraint; kind mismatch";_tag_dyneither(_tmp19,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpC6,_tmpC5,_tag_dyneither(_tmp18,sizeof(void*),0U));});});
return _tmp17;}{
# 107
void*_tmp1A=Cyc_Tcutil_compress(rgn);void*_tmp1B=_tmp1A;struct Cyc_Absyn_Tvar*_tmp28;switch(*((int*)_tmp1B)){case 2U: _LL1: _tmp28=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B)->f1;_LL2: {
# 109
struct Cyc_Dict_Dict _tmp1C=po->d;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(_tmp1C,_tmp28)){
void*_tmp1D=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(_tmp1C,_tmp28);
({struct Cyc_Dict_Dict _tmpCA=({struct Cyc_Dict_Dict _tmpC9=_tmp1C;struct Cyc_Absyn_Tvar*_tmpC8=_tmp28;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmpC9,_tmpC8,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=24U,({struct Cyc_List_List*_tmpC7=({void*_tmp1E[2U];_tmp1E[0]=eff,_tmp1E[1]=_tmp1D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1E,sizeof(void*),2U));});_tmp1F->f1=_tmpC7;});_tmp1F;}));});_tmp1C=_tmpCA;});}else{
# 114
({struct Cyc_Dict_Dict _tmpCB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmp1C,_tmp28,eff);_tmp1C=_tmpCB;});}
_tmp17->d=_tmp1C;
return _tmp17;}case 1U: _LL3: _LL4:
# 118
 Cyc_Tcutil_unify(rgn,(void*)& Cyc_Absyn_HeapRgn_val);
goto _LL6;case 20U: _LL5: _LL6:
# 121
({void*_tmpCD=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=24U,({struct Cyc_List_List*_tmpCC=({void*_tmp20[2U];_tmp20[0]=eff,_tmp20[1]=po->these_outlive_heap;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp20,sizeof(void*),2U));});_tmp21->f1=_tmpCC;});_tmp21;});_tmp17->these_outlive_heap=_tmpCD;});
return _tmp17;case 21U: _LL7: _LL8:
# 124
({void*_tmpCF=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=24U,({struct Cyc_List_List*_tmpCE=({void*_tmp22[2U];_tmp22[0]=eff,_tmp22[1]=po->these_outlive_unique;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp22,sizeof(void*),2U));});_tmp23->f1=_tmpCE;});_tmp23;});_tmp17->these_outlive_unique=_tmpCF;});
return _tmp17;case 22U: _LL9: _LLA:
# 127
({void*_tmp24=0U;({struct _dyneither_ptr _tmpD0=({const char*_tmp25="RgnOrder::add_outlives_constraint can't outlive RC for now";_tag_dyneither(_tmp25,sizeof(char),59U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD0,_tag_dyneither(_tmp24,sizeof(void*),0U));});});default: _LLB: _LLC:
({void*_tmp26=0U;({struct _dyneither_ptr _tmpD1=({const char*_tmp27="RgnOrder::add_outlives_constraint passed a bad region";_tag_dyneither(_tmp27,sizeof(char),54U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD1,_tag_dyneither(_tmp26,sizeof(void*),0U));});});}_LL0:;};};}
# 132
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int opened){
# 134
if(!opened  && ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,rgn))
({void*_tmp2A=0U;({struct _dyneither_ptr _tmpD2=({const char*_tmp2B="RgnOrder::add_youngest: repeated region";_tag_dyneither(_tmp2B,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD2,_tag_dyneither(_tmp2A,sizeof(void*),0U));});});{
struct Cyc_RgnOrder_RgnPO*_tmp2C=({struct Cyc_RgnOrder_RgnPO*_tmp36=_cycalloc(sizeof(*_tmp36));*_tmp36=*po;_tmp36;});
if(opened){
({struct Cyc_Dict_Dict _tmpD3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(po->d,rgn,Cyc_Absyn_empty_effect);_tmp2C->d=_tmpD3;});
({void*_tmpD7=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=24U,({struct Cyc_List_List*_tmpD6=({void*_tmp2D[2U];({void*_tmpD5=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=23U,({void*_tmpD4=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=2U,_tmp2E->f1=rgn;_tmp2E;});_tmp2F->f1=_tmpD4;});_tmp2F;});_tmp2D[0]=_tmpD5;}),_tmp2D[1]=_tmp2C->opened_regions;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2D,sizeof(void*),2U));});_tmp30->f1=_tmpD6;});_tmp30;});_tmp2C->opened_regions=_tmpD7;});}else{
# 141
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp31=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=24U,({struct Cyc_List_List*_tmpDA=({void*_tmp32[2U];({void*_tmpD9=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->tag=23U,({void*_tmpD8=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->tag=2U,_tmp33->f1=po->youngest;_tmp33;});_tmp34->f1=_tmpD8;});_tmp34;});_tmp32[0]=_tmpD9;}),_tmp32[1]=_tmp2C->opened_regions;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32,sizeof(void*),2U));});_tmp35->f1=_tmpDA;});_tmp35;});
# 143
({struct Cyc_Dict_Dict _tmpDB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(po->d,rgn,(void*)_tmp31);_tmp2C->d=_tmpDB;});
_tmp2C->youngest=rgn;}
# 146
return _tmp2C;};}
# 148
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn){
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,rgn))
({void*_tmp37=0U;({struct _dyneither_ptr _tmpDC=({const char*_tmp38="RgnOrder::add_unordered: repeated region";_tag_dyneither(_tmp38,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDC,_tag_dyneither(_tmp37,sizeof(void*),0U));});});{
struct Cyc_RgnOrder_RgnPO*_tmp39=({struct Cyc_RgnOrder_RgnPO*_tmp3A=_cycalloc(sizeof(*_tmp3A));*_tmp3A=*po;_tmp3A;});
({struct Cyc_Dict_Dict _tmpDD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmp39->d,rgn,Cyc_Absyn_empty_effect);_tmp39->d=_tmpDD;});
return _tmp39;};}
# 156
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int loc){
# 159
struct Cyc_Dict_Dict _tmp3B=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*)))Cyc_Dict_empty)(Cyc_Absyn_tvar_cmp);
{struct Cyc_List_List*_tmp3C=tvs;for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){
if((Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3C->hd,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_RgnKind)
# 164
({struct Cyc_Dict_Dict _tmpDE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmp3B,(struct Cyc_Absyn_Tvar*)_tmp3C->hd,Cyc_Absyn_empty_effect);_tmp3B=_tmpDE;});}}
# 167
if(!({void*_tmpDF=effect;Cyc_RgnOrder_valid_constraint(_tmpDF,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->tag=2U,_tmp3D->f1=fst_rgn;_tmp3D;}));})){
# 169
({void*_tmp3E=0U;({unsigned int _tmpE1=loc;struct _dyneither_ptr _tmpE0=({const char*_tmp3F="Invalid region ordering constraint; kind mismatch";_tag_dyneither(_tmp3F,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpE1,_tmpE0,_tag_dyneither(_tmp3E,sizeof(void*),0U));});});
return({struct Cyc_RgnOrder_RgnPO*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->d=_tmp3B,_tmp40->these_outlive_heap=Cyc_Absyn_empty_effect,_tmp40->these_outlive_unique=Cyc_Absyn_empty_effect,_tmp40->youngest=fst_rgn,_tmp40->opened_regions=Cyc_Absyn_empty_effect;_tmp40;});}
# 175
({struct Cyc_Dict_Dict _tmpE2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmp3B,fst_rgn,effect);_tmp3B=_tmpE2;});{
# 177
struct Cyc_RgnOrder_RgnPO*_tmp41=({struct Cyc_RgnOrder_RgnPO*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->d=_tmp3B,_tmp42->these_outlive_heap=Cyc_Absyn_empty_effect,_tmp42->these_outlive_unique=Cyc_Absyn_empty_effect,_tmp42->youngest=fst_rgn,_tmp42->opened_regions=Cyc_Absyn_empty_effect;_tmp42;});
for(0;po != 0;po=po->tl){
({struct Cyc_RgnOrder_RgnPO*_tmpE3=Cyc_RgnOrder_add_outlives_constraint(_tmp41,(*((struct _tuple0*)po->hd)).f1,(*((struct _tuple0*)po->hd)).f2,loc);_tmp41=_tmpE3;});}
# 181
return _tmp41;};}
# 206 "rgnorder.cyc"
static int Cyc_RgnOrder_contains_rgnseff(struct Cyc_Absyn_Tvar*rgns_of_var,void*eff){
void*_tmp43=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(eff));void*_tmp44=_tmp43;struct Cyc_List_List*_tmp49;void*_tmp48;switch(*((int*)_tmp44)){case 25U: _LL1: _tmp48=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp44)->f1;_LL2: {
# 209
void*_tmp45=Cyc_Tcutil_compress(_tmp48);void*_tmp46=_tmp45;struct Cyc_Absyn_Tvar*_tmp47;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp46)->tag == 2U){_LL8: _tmp47=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp46)->f1;_LL9:
 return Cyc_Absyn_tvar_cmp(_tmp47,rgns_of_var)== 0;}else{_LLA: _LLB:
 return 0;}_LL7:;}case 24U: _LL3: _tmp49=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp44)->f1;_LL4:
# 214
 for(0;_tmp49 != 0;_tmp49=_tmp49->tl){
if(Cyc_RgnOrder_contains_rgnseff(rgns_of_var,(void*)_tmp49->hd))
return 1;}
return 0;default: _LL5: _LL6:
 return 0;}_LL0:;}struct Cyc_RgnOrder_OutlivesEnv{struct _RegionHandle*r;struct Cyc_List_List*seen;struct Cyc_List_List*todo;};
# 228
static void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*env,void*eff){
void*_tmp4A=Cyc_Tcutil_compress(eff);void*_tmp4B=_tmp4A;struct Cyc_List_List*_tmp50;struct Cyc_Absyn_Tvar*_tmp4F;switch(*((int*)_tmp4B)){case 23U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4B)->f1)->tag == 2U){_LL1: _tmp4F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4B)->f1)->f1;_LL2:
# 231
{struct Cyc_List_List*_tmp4C=env->seen;for(0;_tmp4C != 0;_tmp4C=_tmp4C->tl){
if(Cyc_Absyn_tvar_cmp(_tmp4F,(struct Cyc_Absyn_Tvar*)_tmp4C->hd)== 0)
return;}}
({struct Cyc_List_List*_tmpE4=({struct Cyc_List_List*_tmp4D=_region_malloc(env->r,sizeof(*_tmp4D));_tmp4D->hd=_tmp4F,_tmp4D->tl=env->seen;_tmp4D;});env->seen=_tmpE4;});
({struct Cyc_List_List*_tmpE5=({struct Cyc_List_List*_tmp4E=_region_malloc(env->r,sizeof(*_tmp4E));_tmp4E->hd=_tmp4F,_tmp4E->tl=env->todo;_tmp4E;});env->todo=_tmpE5;});
return;}else{goto _LL5;}case 24U: _LL3: _tmp50=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4B)->f1;_LL4:
# 238
 for(0;_tmp50 != 0;_tmp50=_tmp50->tl){
Cyc_RgnOrder_add_to_search(env,(void*)_tmp50->hd);}
return;default: _LL5: _LL6:
 return;}_LL0:;}
# 245
static struct Cyc_RgnOrder_OutlivesEnv Cyc_RgnOrder_initial_env(struct _RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,void*rgn){
# 247
struct Cyc_RgnOrder_OutlivesEnv _tmp51=({struct Cyc_RgnOrder_OutlivesEnv _tmpBA;_tmpBA.r=listrgn,_tmpBA.seen=0,_tmpBA.todo=0;_tmpBA;});
void*r=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Kind*_tmp52=Cyc_Tcutil_typ_kind(r);struct Cyc_Absyn_Kind*_tmp53=_tmp52;enum Cyc_Absyn_KindQual _tmp5C;enum Cyc_Absyn_AliasQual _tmp5B;_LL1: _tmp5C=_tmp53->kind;_tmp5B=_tmp53->aliasqual;_LL2:;
if(_tmp5C == Cyc_Absyn_RgnKind){
enum Cyc_Absyn_AliasQual _tmp54=_tmp5B;switch(_tmp54){case Cyc_Absyn_Aliasable: _LL4: _LL5:
# 254
 Cyc_RgnOrder_add_to_search(& _tmp51,po->these_outlive_unique);
Cyc_RgnOrder_add_to_search(& _tmp51,po->these_outlive_heap);
goto _LL3;case Cyc_Absyn_Unique: _LL6: _LL7:
# 258
 Cyc_RgnOrder_add_to_search(& _tmp51,po->these_outlive_unique);goto _LL3;default: _LL8: _LL9:
# 263
 goto _LL3;}_LL3:;}else{
# 267
({void*_tmp55=0U;({struct _dyneither_ptr _tmpE6=({const char*_tmp56="RgnOrder: rgn not of correct kind";_tag_dyneither(_tmp56,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE6,_tag_dyneither(_tmp55,sizeof(void*),0U));});});}{
# 269
void*_tmp57=r;struct Cyc_Absyn_Tvar*_tmp5A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp57)->tag == 2U){_LLB: _tmp5A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp57)->f1;_LLC:
# 271
({struct Cyc_List_List*_tmpE7=({struct Cyc_List_List*_tmp58=_region_malloc(listrgn,sizeof(*_tmp58));_tmp58->hd=_tmp5A,_tmp58->tl=_tmp51.seen;_tmp58;});_tmp51.seen=_tmpE7;});
({struct Cyc_List_List*_tmpE8=({struct Cyc_List_List*_tmp59=_region_malloc(listrgn,sizeof(*_tmp59));_tmp59->hd=_tmp5A,_tmp59->tl=_tmp51.todo;_tmp59;});_tmp51.todo=_tmpE8;});
goto _LLE;}else{_LLD: _LLE:
 return _tmp51;}_LLA:;};}
# 280
static int Cyc_RgnOrder_atomic_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
struct _RegionHandle _tmp5D=_new_region("listrgn");struct _RegionHandle*listrgn=& _tmp5D;_push_region(listrgn);
{void*_tmp5E=Cyc_Tcutil_compress(eff);void*_tmp5F=_tmp5E;void*_tmp87;void*_tmp86;switch(*((int*)_tmp5F)){case 23U: _LL1: _tmp86=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5F)->f1;_LL2: {
# 285
void*_tmp60=Cyc_Tcutil_compress(_tmp86);void*_tmp61=_tmp60;struct Cyc_Absyn_Tvar*_tmp73;switch(*((int*)_tmp61)){case 21U: _LL8: _LL9: {
# 288
struct Cyc_Absyn_Kind*_tmp62=Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn));struct Cyc_Absyn_Kind*_tmp63=_tmp62;enum Cyc_Absyn_KindQual _tmp66;enum Cyc_Absyn_AliasQual _tmp65;_LL13: _tmp66=_tmp63->kind;_tmp65=_tmp63->aliasqual;_LL14:;{
int _tmp64=_tmp66 == Cyc_Absyn_RgnKind  && _tmp65 != Cyc_Absyn_Top;_npop_handler(0U);return _tmp64;};}case 22U: _LLA: _LLB:
# 293
 if(Cyc_Tcutil_compress(rgn)== (void*)& Cyc_Absyn_RefCntRgn_val){int _tmp67=1;_npop_handler(0U);return _tmp67;}
goto _LLD;case 20U: _LLC: _LLD: {
# 297
struct Cyc_Absyn_Kind*_tmp68=Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn));struct Cyc_Absyn_Kind*_tmp69=_tmp68;enum Cyc_Absyn_KindQual _tmp6C;enum Cyc_Absyn_AliasQual _tmp6B;_LL16: _tmp6C=_tmp69->kind;_tmp6B=_tmp69->aliasqual;_LL17:;{
int _tmp6A=_tmp6C == Cyc_Absyn_RgnKind  && _tmp6B == Cyc_Absyn_Aliasable;_npop_handler(0U);return _tmp6A;};}case 2U: _LLE: _tmp73=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp61)->f1;_LLF: {
# 307
struct Cyc_RgnOrder_OutlivesEnv _tmp6D=Cyc_RgnOrder_initial_env(listrgn,po,rgn);
while(_tmp6D.todo != 0){
struct Cyc_Absyn_Tvar*_tmp6E=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6D.todo))->hd;
_tmp6D.todo=((struct Cyc_List_List*)_check_null(_tmp6D.todo))->tl;
# 313
if(Cyc_Absyn_tvar_cmp(_tmp6E,_tmp73)== 0){
# 315
int _tmp6F=1;_npop_handler(0U);return _tmp6F;}
# 321
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp6E))
# 325
Cyc_RgnOrder_add_to_search(& _tmp6D,((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,_tmp6E));else{
# 328
int _tmp70=0;_npop_handler(0U);return _tmp70;}}{
# 331
int _tmp71=0;_npop_handler(0U);return _tmp71;};}default: _LL10: _LL11: {
int _tmp72=0;_npop_handler(0U);return _tmp72;}}_LL7:;}case 25U: _LL3: _tmp87=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5F)->f1;_LL4: {
# 335
void*_tmp74=Cyc_Tcutil_compress(_tmp87);void*_tmp75=_tmp74;struct Cyc_Absyn_Tvar*_tmp84;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->tag == 2U){_LL19: _tmp84=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->f1;_LL1A: {
# 341
struct Cyc_RgnOrder_OutlivesEnv _tmp76=Cyc_RgnOrder_initial_env(listrgn,po,rgn);
struct Cyc_Absyn_Kind*_tmp77=Cyc_Tcutil_typ_kind(rgn);struct Cyc_Absyn_Kind*_tmp78=_tmp77;enum Cyc_Absyn_KindQual _tmp82;enum Cyc_Absyn_AliasQual _tmp81;_LL1E: _tmp82=_tmp78->kind;_tmp81=_tmp78->aliasqual;_LL1F:;
# 344
if(_tmp82 == Cyc_Absyn_RgnKind){
if(_tmp81 == Cyc_Absyn_Aliasable){
if(Cyc_RgnOrder_contains_rgnseff(_tmp84,po->these_outlive_heap) || 
Cyc_RgnOrder_contains_rgnseff(_tmp84,po->these_outlive_unique)){
int _tmp79=1;_npop_handler(0U);return _tmp79;}}else{
# 350
if(_tmp81 == Cyc_Absyn_Unique){
if(Cyc_RgnOrder_contains_rgnseff(_tmp84,po->these_outlive_unique)){
int _tmp7A=1;_npop_handler(0U);return _tmp7A;}}}}
# 355
while(_tmp76.todo != 0){
struct Cyc_Absyn_Tvar*_tmp7B=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp76.todo))->hd;
_tmp76.todo=((struct Cyc_List_List*)_check_null(_tmp76.todo))->tl;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp7B)){
void*_tmp7C=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,_tmp7B);
if(Cyc_RgnOrder_contains_rgnseff(_tmp84,_tmp7C)){
int _tmp7D=1;_npop_handler(0U);return _tmp7D;}
Cyc_RgnOrder_add_to_search(& _tmp76,_tmp7C);}else{
({void*_tmp7E=0U;({struct _dyneither_ptr _tmpE9=({const char*_tmp7F="RgnOrder: type variable not in dict";_tag_dyneither(_tmp7F,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE9,_tag_dyneither(_tmp7E,sizeof(void*),0U));});});}}{
# 365
int _tmp80=0;_npop_handler(0U);return _tmp80;};}}else{_LL1B: _LL1C: {
int _tmp83=0;_npop_handler(0U);return _tmp83;}}_LL18:;}default: _LL5: _LL6: {
# 368
int _tmp85=0;_npop_handler(0U);return _tmp85;}}_LL0:;}
# 282
;_pop_region(listrgn);}
# 375
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
({void*_tmpEA=Cyc_Tcutil_normalize_effect(eff);eff=_tmpEA;});{
void*_tmp88=Cyc_Tcutil_compress(eff);void*_tmp89=_tmp88;struct Cyc_List_List*_tmp8A;if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp89)->tag == 24U){_LL1: _tmp8A=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp89)->f1;_LL2:
# 379
 for(0;_tmp8A != 0;_tmp8A=_tmp8A->tl){
if(!Cyc_RgnOrder_effect_outlives(po,(void*)_tmp8A->hd,rgn))
return 0;}
return 1;}else{_LL3: _LL4:
# 384
 return Cyc_RgnOrder_atomic_effect_outlives(po,eff,rgn);}_LL0:;};}
# 388
static void Cyc_RgnOrder_pin_effect(void*eff,void*bd){
({void*_tmpEB=Cyc_Tcutil_normalize_effect(eff);eff=_tmpEB;});{
void*_tmp8B=Cyc_Tcutil_compress(eff);void*_tmp8C=_tmp8B;void*_tmp93;void*_tmp92;struct Cyc_List_List*_tmp91;switch(*((int*)_tmp8C)){case 24U: _LL1: _tmp91=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp8C)->f1;_LL2:
# 392
 for(0;_tmp91 != 0;_tmp91=_tmp91->tl){
Cyc_RgnOrder_pin_effect((void*)_tmp91->hd,bd);}
return;case 25U: _LL3: _tmp92=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp8C)->f1;_LL4: {
# 396
void*_tmp8D=Cyc_Tcutil_compress(_tmp92);void*_tmp8E=_tmp8D;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8E)->tag == 1U){_LLC: _LLD:
 Cyc_Tcutil_unify(_tmp92,Cyc_Absyn_uint_typ);return;}else{_LLE: _LLF:
 return;}_LLB:;}case 23U: _LL5: _tmp93=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp8C)->f1;_LL6: {
# 401
void*_tmp8F=Cyc_Tcutil_compress(_tmp93);void*_tmp90=_tmp8F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp90)->tag == 1U){_LL11: _LL12:
 Cyc_Tcutil_unify(_tmp93,bd);return;}else{_LL13: _LL14:
 return;}_LL10:;}case 1U: _LL7: _LL8:
# 405
 Cyc_Tcutil_unify(eff,Cyc_Absyn_empty_effect);return;default: _LL9: _LLA:
 return;}_LL0:;};}
# 420 "rgnorder.cyc"
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin){
# 423
{struct Cyc_List_List*_tmp94=constraints;for(0;_tmp94 != 0;_tmp94=_tmp94->tl){
struct _tuple0*_tmp95=(struct _tuple0*)_tmp94->hd;struct _tuple0*_tmp96=_tmp95;void*_tmp99;_LL1: _tmp99=_tmp96->f2;_LL2:;{
void*_tmp97=Cyc_Tcutil_compress(_tmp99);void*_tmp98=_tmp97;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp98)->tag == 1U){_LL4: _LL5:
# 427
 if(do_pin)
Cyc_Tcutil_unify(_tmp99,default_bound);
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}}
# 433
{struct Cyc_List_List*_tmp9A=constraints;for(0;_tmp9A != 0;_tmp9A=_tmp9A->tl){
struct _tuple0*_tmp9B=(struct _tuple0*)_tmp9A->hd;struct _tuple0*_tmp9C=_tmp9B;void*_tmp9E;void*_tmp9D;_LL9: _tmp9E=_tmp9C->f1;_tmp9D=_tmp9C->f2;_LLA:;
if(do_pin)
Cyc_RgnOrder_pin_effect(_tmp9E,_tmp9D);
if(!Cyc_RgnOrder_effect_outlives(po,_tmp9E,_tmp9D))
return 0;}}
# 440
return 1;}
# 443
static int Cyc_RgnOrder_eff_outlives_atomic_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
# 445
({void*_tmpEC=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff2));eff2=_tmpEC;});{
void*_tmp9F=eff2;void*_tmpA8;void*_tmpA7;struct Cyc_List_List*_tmpA6;switch(*((int*)_tmp9F)){case 24U: _LL1: _tmpA6=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp9F)->f1;_LL2:
# 448
 for(0;_tmpA6 != 0;_tmpA6=_tmpA6->tl){
if(Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,(void*)_tmpA6->hd))
return 1;}
return 0;case 23U: _LL3: _tmpA7=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp9F)->f1;_LL4:
 return Cyc_RgnOrder_effect_outlives(po,eff1,_tmpA7);case 25U: _LL5: _tmpA8=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp9F)->f1;_LL6: {
# 454
void*_tmpA0=eff1;void*_tmpA5;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpA0)->tag == 25U){_LLA: _tmpA5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpA0)->f1;_LLB:
# 456
{struct _tuple0 _tmpA1=({struct _tuple0 _tmpBB;({void*_tmpEE=Cyc_Tcutil_compress(_tmpA8);_tmpBB.f1=_tmpEE;}),({void*_tmpED=Cyc_Tcutil_compress(_tmpA5);_tmpBB.f2=_tmpED;});_tmpBB;});struct _tuple0 _tmpA2=_tmpA1;struct Cyc_Absyn_Tvar*_tmpA4;struct Cyc_Absyn_Tvar*_tmpA3;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2.f2)->tag == 2U){_LLF: _tmpA4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2.f1)->f1;_tmpA3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2.f2)->f1;_LL10:
 return Cyc_Absyn_tvar_cmp(_tmpA4,_tmpA3)== 0;}else{goto _LL11;}}else{_LL11: _LL12:
 goto _LLE;}_LLE:;}
# 460
goto _LLD;}else{_LLC: _LLD:
 return eff1 == (void*)& Cyc_Absyn_HeapRgn_val;}_LL9:;}default: _LL7: _LL8:
# 463
 return eff1 == (void*)& Cyc_Absyn_HeapRgn_val;}_LL0:;};}
# 467
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
({void*_tmpEF=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff1));eff1=_tmpEF;});{
void*_tmpA9=eff1;struct Cyc_List_List*_tmpAA;if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpA9)->tag == 24U){_LL1: _tmpAA=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpA9)->f1;_LL2:
# 471
 for(0;_tmpAA != 0;_tmpAA=_tmpAA->tl){
if(!Cyc_RgnOrder_eff_outlives_eff(po,(void*)_tmpAA->hd,eff2))
return 0;}
return 1;}else{_LL3: _LL4:
 return Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,eff2);}_LL0:;};}struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 480
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po){
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,po->d);
struct _tuple11 _tmpAB=*((struct _tuple11*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(Cyc_Core_heap_region,po->d);
({void*_tmpAC=0U;({struct Cyc___cycFILE*_tmpF1=Cyc_stderr;struct _dyneither_ptr _tmpF0=({const char*_tmpAD="region po:\n";_tag_dyneither(_tmpAD,sizeof(char),12U);});Cyc_fprintf(_tmpF1,_tmpF0,_tag_dyneither(_tmpAC,sizeof(void*),0U));});});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple11*))Cyc_Iter_next)(iter,& _tmpAB)){
({struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmpBD;_tmpBD.tag=0U,_tmpBD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(_tmpAB.f1)->name);_tmpBD;});struct Cyc_String_pa_PrintArg_struct _tmpB1=({struct Cyc_String_pa_PrintArg_struct _tmpBC;_tmpBC.tag=0U,({
struct _dyneither_ptr _tmpF2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAB.f2));_tmpBC.f1=_tmpF2;});_tmpBC;});void*_tmpAE[2U];_tmpAE[0]=& _tmpB0,_tmpAE[1]=& _tmpB1;({struct Cyc___cycFILE*_tmpF4=Cyc_stderr;struct _dyneither_ptr _tmpF3=({const char*_tmpAF="  %s outlived by %s\n";_tag_dyneither(_tmpAF,sizeof(char),21U);});Cyc_fprintf(_tmpF4,_tmpF3,_tag_dyneither(_tmpAE,sizeof(void*),2U));});});}
({struct Cyc_String_pa_PrintArg_struct _tmpB4=({struct Cyc_String_pa_PrintArg_struct _tmpBE;_tmpBE.tag=0U,({
struct _dyneither_ptr _tmpF5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(po->these_outlive_heap));_tmpBE.f1=_tmpF5;});_tmpBE;});void*_tmpB2[1U];_tmpB2[0]=& _tmpB4;({struct Cyc___cycFILE*_tmpF7=Cyc_stderr;struct _dyneither_ptr _tmpF6=({const char*_tmpB3="  these outlive heap: %s\n";_tag_dyneither(_tmpB3,sizeof(char),26U);});Cyc_fprintf(_tmpF7,_tmpF6,_tag_dyneither(_tmpB2,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmpB7=({struct Cyc_String_pa_PrintArg_struct _tmpBF;_tmpBF.tag=0U,({
struct _dyneither_ptr _tmpF8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(po->these_outlive_unique));_tmpBF.f1=_tmpF8;});_tmpBF;});void*_tmpB5[1U];_tmpB5[0]=& _tmpB7;({struct Cyc___cycFILE*_tmpFA=Cyc_stderr;struct _dyneither_ptr _tmpF9=({const char*_tmpB6="  these outlive unique: %s\n";_tag_dyneither(_tmpB6,sizeof(char),28U);});Cyc_fprintf(_tmpFA,_tmpF9,_tag_dyneither(_tmpB5,sizeof(void*),1U));});});}

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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Splay_node;struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};struct Cyc_Splay_Leaf_Splay_tree_struct{int tag;int f1;};struct Cyc_Splay_Node_Splay_tree_struct{int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{void*key;void*data;void*left;void*right;};
# 49 "splay.h"
int Cyc_Splay_splay(int(*f)(void*,void*),void*,void*);struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[8U];struct Cyc_SlowDict_Present_exn_struct{char*tag;};extern char Cyc_SlowDict_Absent[7U];struct Cyc_SlowDict_Absent_exn_struct{char*tag;};
# 49 "slowdict.h"
struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*cmp)(void*,void*));
# 56
int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d);
# 60
int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*k);
# 64
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*k,void*v);
# 69
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*k,void*v);
# 73
struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*l);
# 77
struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 81
void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*k);
# 84
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*k);
# 89
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*k);
# 94
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*k);
# 98
void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*d,void*accum);
# 102
void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d,void*accum);
# 106
void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d);
# 110
void Cyc_SlowDict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d);
# 114
void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d);
# 116
void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d);
# 120
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d);
# 125
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d);struct _tuple0{void*f1;void*f2;};
# 129
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d);
# 134
struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d);char Cyc_SlowDict_Absent[7U]="Absent";char Cyc_SlowDict_Present[8U]="Present";
# 32 "slowdict.cyc"
struct Cyc_SlowDict_Absent_exn_struct Cyc_SlowDict_Absent_val={Cyc_SlowDict_Absent};
struct Cyc_SlowDict_Present_exn_struct Cyc_SlowDict_Present_val={Cyc_SlowDict_Present};struct Cyc_SlowDict_Dict{int(*reln)(void*,void*);void*tree;};
# 41
struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*comp)(void*,void*)){
void*t=(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=0U,_tmp1->f1=0;_tmp1;});
return({struct Cyc_SlowDict_Dict*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->reln=comp,_tmp0->tree=t;_tmp0;});}
# 49
int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d){
void*_tmp2=d->tree;void*_tmp3=_tmp2;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp3)->tag == 0U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 56
int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*key){
return Cyc_Splay_splay(d->reln,key,d->tree);}
# 60
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*key,void*data){
void*leaf=(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=0U,_tmp15->f1=0;_tmp15;});
void*newleft=leaf;void*newright=leaf;
if(Cyc_Splay_splay(d->reln,key,d->tree)){
# 65
void*_tmp4=d->tree;void*_tmp5=_tmp4;struct Cyc_Splay_noderef*_tmp6;if(((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp5)->tag == 1U){_LL1: _tmp6=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp5)->f1;_LL2:
# 67
 newleft=(_tmp6->v)->left;
newright=(_tmp6->v)->right;
goto _LL0;}else{_LL3: _LL4:
# 71
 goto _LL0;}_LL0:;}else{
# 75
void*_tmp7=d->tree;void*_tmp8=_tmp7;struct Cyc_Splay_noderef*_tmp10;if(((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp8)->tag == 1U){_LL6: _tmp10=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp8)->f1;_LL7: {
# 77
struct Cyc_Splay_node*_tmp9=_tmp10->v;
if((d->reln)(key,_tmp9->key)< 0){
newleft=_tmp9->left;
({void*_tmp5D=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->tag=1U,({struct Cyc_Splay_noderef*_tmp5C=({struct Cyc_Splay_noderef*_tmpB=_cycalloc(sizeof(*_tmpB));({struct Cyc_Splay_node*_tmp5B=({struct Cyc_Splay_node*_tmpA=_cycalloc(sizeof(*_tmpA));
_tmpA->key=_tmp9->key,_tmpA->data=_tmp9->data,_tmpA->left=leaf,_tmpA->right=_tmp9->right;_tmpA;});
# 80
_tmpB->v=_tmp5B;});_tmpB;});_tmpC->f1=_tmp5C;});_tmpC;});newright=_tmp5D;});}else{
# 84
({void*_tmp60=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->tag=1U,({struct Cyc_Splay_noderef*_tmp5F=({struct Cyc_Splay_noderef*_tmpE=_cycalloc(sizeof(*_tmpE));({struct Cyc_Splay_node*_tmp5E=({struct Cyc_Splay_node*_tmpD=_cycalloc(sizeof(*_tmpD));
_tmpD->key=_tmp9->key,_tmpD->data=_tmp9->data,_tmpD->left=_tmp9->left,_tmpD->right=leaf;_tmpD;});
# 84
_tmpE->v=_tmp5E;});_tmpE;});_tmpF->f1=_tmp5F;});_tmpF;});newleft=_tmp60;});
# 86
newright=_tmp9->right;}
# 88
goto _LL5;}}else{_LL8: _LL9:
# 90
 goto _LL5;}_LL5:;}
# 93
return({struct Cyc_SlowDict_Dict*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->reln=d->reln,({void*_tmp63=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->tag=1U,({struct Cyc_Splay_noderef*_tmp62=({struct Cyc_Splay_noderef*_tmp12=_cycalloc(sizeof(*_tmp12));({
struct Cyc_Splay_node*_tmp61=({struct Cyc_Splay_node*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->key=key,_tmp11->data=data,_tmp11->left=newleft,_tmp11->right=newright;_tmp11;});_tmp12->v=_tmp61;});_tmp12;});
# 93
_tmp13->f1=_tmp62;});_tmp13;});_tmp14->tree=_tmp63;});_tmp14;});}
# 98
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*key,void*data){
# 100
if(Cyc_Splay_splay(d->reln,key,d->tree))
(int)_throw((void*)& Cyc_SlowDict_Present_val);
return Cyc_SlowDict_insert(d,key,data);}
# 105
struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*kds){
# 107
for(0;kds != 0;kds=kds->tl){
({struct Cyc_SlowDict_Dict*_tmp64=Cyc_SlowDict_insert(d,(*((struct _tuple0*)kds->hd)).f1,(*((struct _tuple0*)kds->hd)).f2);d=_tmp64;});}
return d;}
# 112
struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(int(*comp)(void*,void*),void*key,void*data){
struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp16=({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0U,_tmp1B->f1=0;_tmp1B;});
return({struct Cyc_SlowDict_Dict*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->reln=comp,({void*_tmp67=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=1U,({struct Cyc_Splay_noderef*_tmp66=({struct Cyc_Splay_noderef*_tmp18=_cycalloc(sizeof(*_tmp18));({struct Cyc_Splay_node*_tmp65=({struct Cyc_Splay_node*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->key=key,_tmp17->data=data,_tmp17->left=(void*)_tmp16,_tmp17->right=(void*)_tmp16;_tmp17;});_tmp18->v=_tmp65;});_tmp18;});_tmp19->f1=_tmp66;});_tmp19;});_tmp1A->tree=_tmp67;});_tmp1A;});}
# 117
void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_tmp1C=d->tree;void*_tmp1D=_tmp1C;struct Cyc_Splay_noderef*_tmp20;if(((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp1D)->tag == 1U){_LL1: _tmp20=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp1D)->f1;_LL2:
 return(_tmp20->v)->data;}else{_LL3: _LL4:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp68=({const char*_tmp1E="Dict::lookup";_tag_dyneither(_tmp1E,sizeof(char),13U);});_tmp1F->f1=_tmp68;});_tmp1F;}));}_LL0:;}
# 123
(int)_throw((void*)& Cyc_SlowDict_Absent_val);}
# 126
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_tmp21=d->tree;void*_tmp22=_tmp21;struct Cyc_Splay_noderef*_tmp26;if(((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp22)->tag == 1U){_LL1: _tmp26=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp22)->f1;_LL2:
 return({struct Cyc_Core_Opt*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->v=(_tmp26->v)->data;_tmp23;});}else{_LL3: _LL4:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp69=({const char*_tmp24="Dict::lookup";_tag_dyneither(_tmp24,sizeof(char),13U);});_tmp25->f1=_tmp69;});_tmp25;}));}_LL0:;}
# 132
return 0;}
# 135
static int Cyc_SlowDict_get_largest(void*x,void*y){return 1;}
# 137
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_tmp27=d->tree;void*_tmp28=_tmp27;struct Cyc_Splay_noderef*_tmp37;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp28)->tag == 0U){_LL1: _LL2:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp6A=({const char*_tmp29="Dict::lookup";_tag_dyneither(_tmp29,sizeof(char),13U);});_tmp2A->f1=_tmp6A;});_tmp2A;}));}else{_LL3: _tmp37=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp28)->f1;_LL4: {
# 142
struct Cyc_Splay_node*n=_tmp37->v;
void*_tmp2B=n->left;void*_tmp2C=_tmp2B;struct Cyc_Splay_noderef*_tmp36;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp2C)->tag == 0U){_LL6: _LL7:
 return({struct Cyc_SlowDict_Dict*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->reln=d->reln,_tmp2D->tree=n->right;_tmp2D;});}else{_LL8: _tmp36=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp2C)->f1;_LL9: {
# 146
void*_tmp2E=n->right;void*_tmp2F=_tmp2E;struct Cyc_Splay_noderef*_tmp35;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp2F)->tag == 0U){_LLB: _LLC:
 return({struct Cyc_SlowDict_Dict*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->reln=d->reln,_tmp30->tree=n->left;_tmp30;});}else{_LLD: _tmp35=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp2F)->f1;_LLE:
# 149
 Cyc_Splay_splay(Cyc_SlowDict_get_largest,key,n->left);{
struct Cyc_Splay_node*newtop=_tmp36->v;
return({struct Cyc_SlowDict_Dict*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->reln=d->reln,({
void*_tmp6D=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->tag=1U,({struct Cyc_Splay_noderef*_tmp6C=({struct Cyc_Splay_noderef*_tmp32=_cycalloc(sizeof(*_tmp32));({struct Cyc_Splay_node*_tmp6B=({struct Cyc_Splay_node*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->key=newtop->key,_tmp31->data=newtop->data,_tmp31->left=newtop->left,_tmp31->right=n->right;_tmp31;});_tmp32->v=_tmp6B;});_tmp32;});_tmp33->f1=_tmp6C;});_tmp33;});_tmp34->tree=_tmp6D;});_tmp34;});};}_LLA:;}}_LL5:;}}_LL0:;}else{
# 158
return d;}}
# 161
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*key){
struct Cyc_SlowDict_Dict*_tmp38=Cyc_SlowDict_delete(d,key);
if(d == _tmp38)
(int)_throw((void*)& Cyc_SlowDict_Absent_val);
return _tmp38;}
# 168
static void*Cyc_SlowDict_fold_tree(void*(*f)(void*,void*,void*),void*t,void*accum){
void*_tmp39=t;struct Cyc_Splay_noderef*_tmp3A;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp39)->tag == 0U){_LL1: _LL2:
# 171
 return accum;}else{_LL3: _tmp3A=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp39)->f1;_LL4: {
# 173
struct Cyc_Splay_node*n=_tmp3A->v;
return({void*(*_tmp72)(void*,void*,void*)=f;void*_tmp71=n->key;void*_tmp70=n->data;_tmp72(_tmp71,_tmp70,({void*(*_tmp6F)(void*,void*,void*)=f;void*_tmp6E=n->left;Cyc_SlowDict_fold_tree(_tmp6F,_tmp6E,
Cyc_SlowDict_fold_tree(f,n->right,accum));}));});}}_LL0:;}
# 178
void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*d,void*accum){
return Cyc_SlowDict_fold_tree(f,d->tree,accum);}
# 182
static void*Cyc_SlowDict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,void*t,void*accum){
void*_tmp3B=t;struct Cyc_Splay_noderef*_tmp3C;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp3B)->tag == 0U){_LL1: _LL2:
# 185
 return accum;}else{_LL3: _tmp3C=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp3B)->f1;_LL4: {
# 187
struct Cyc_Splay_node*n=_tmp3C->v;
return({void*(*_tmp79)(void*,void*,void*,void*)=f;void*_tmp78=env;void*_tmp77=n->key;void*_tmp76=n->data;_tmp79(_tmp78,_tmp77,_tmp76,({
void*(*_tmp75)(void*,void*,void*,void*)=f;void*_tmp74=env;void*_tmp73=n->left;Cyc_SlowDict_fold_tree_c(_tmp75,_tmp74,_tmp73,Cyc_SlowDict_fold_tree_c(f,env,n->right,accum));}));});}}_LL0:;}
# 192
void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*dict,void*accum){
return Cyc_SlowDict_fold_tree_c(f,env,dict->tree,accum);}
# 196
static void Cyc_SlowDict_app_tree(void*(*f)(void*,void*),void*t){
void*_tmp3D=t;struct Cyc_Splay_noderef*_tmp3F;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp3D)->tag == 0U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp3F=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp3D)->f1;_LL4: {
# 200
struct Cyc_Splay_node*_tmp3E=_tmp3F->v;
Cyc_SlowDict_app_tree(f,_tmp3E->left);
f(_tmp3E->key,_tmp3E->data);
Cyc_SlowDict_app_tree(f,_tmp3E->right);
goto _LL0;}}_LL0:;}
# 207
void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree(f,d->tree);}
# 211
static void Cyc_SlowDict_iter_tree(void(*f)(void*,void*),void*t){
void*_tmp40=t;struct Cyc_Splay_noderef*_tmp41;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp40)->tag == 0U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp41=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp40)->f1;_LL4: {
# 215
struct Cyc_Splay_node*n=_tmp41->v;
Cyc_SlowDict_iter_tree(f,n->left);
f(n->key,n->data);
Cyc_SlowDict_iter_tree(f,n->right);
goto _LL0;}}_LL0:;}
# 222
void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_iter_tree(f,d->tree);}
# 226
static void Cyc_SlowDict_app_tree_c(void*(*f)(void*,void*,void*),void*env,void*t){
void*_tmp42=t;struct Cyc_Splay_noderef*_tmp43;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp42)->tag == 0U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp43=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp42)->f1;_LL4: {
# 230
struct Cyc_Splay_node*n=_tmp43->v;
Cyc_SlowDict_app_tree_c(f,env,n->left);
f(env,n->key,n->data);
Cyc_SlowDict_app_tree_c(f,env,n->right);
goto _LL0;}}_LL0:;}
# 237
void Cyc_SlowDict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree_c(f,env,d->tree);}
# 241
static void Cyc_SlowDict_iter_tree_c(void(*f)(void*,void*,void*),void*env,void*t){
void*_tmp44=t;struct Cyc_Splay_noderef*_tmp45;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp44)->tag == 0U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp45=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp44)->f1;_LL4: {
# 245
struct Cyc_Splay_node*n=_tmp45->v;
Cyc_SlowDict_iter_tree_c(f,env,n->left);
f(env,n->key,n->data);
Cyc_SlowDict_iter_tree_c(f,env,n->right);
goto _LL0;}}_LL0:;}
# 252
void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_iter_tree_c(f,env,d->tree);}
# 256
static void*Cyc_SlowDict_map_tree(void*(*f)(void*),void*t){
void*_tmp46=t;struct Cyc_Splay_noderef*_tmp4C;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp46)->tag == 0U){_LL1: _LL2:
 return(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->tag=0U,_tmp47->f1=0;_tmp47;});}else{_LL3: _tmp4C=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp46)->f1;_LL4: {
# 260
struct Cyc_Splay_node*_tmp48=_tmp4C->v;
return(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->tag=1U,({struct Cyc_Splay_noderef*_tmp7E=({struct Cyc_Splay_noderef*_tmp4A=_cycalloc(sizeof(*_tmp4A));({struct Cyc_Splay_node*_tmp7D=({struct Cyc_Splay_node*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->key=_tmp48->key,({
void*_tmp7C=f(_tmp48->data);_tmp49->data=_tmp7C;}),({
void*_tmp7B=Cyc_SlowDict_map_tree(f,_tmp48->left);_tmp49->left=_tmp7B;}),({
void*_tmp7A=Cyc_SlowDict_map_tree(f,_tmp48->right);_tmp49->right=_tmp7A;});_tmp49;});
# 261
_tmp4A->v=_tmp7D;});_tmp4A;});_tmp4B->f1=_tmp7E;});_tmp4B;});}}_LL0:;}
# 267
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d){
return({struct Cyc_SlowDict_Dict*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->reln=d->reln,({void*_tmp7F=Cyc_SlowDict_map_tree(f,d->tree);_tmp4D->tree=_tmp7F;});_tmp4D;});}
# 271
static void*Cyc_SlowDict_map_tree_c(void*(*f)(void*,void*),void*env,void*t){
void*_tmp4E=t;struct Cyc_Splay_noderef*_tmp53;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp4E)->tag == 0U){_LL1: _LL2:
 return(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=0U,_tmp4F->f1=0;_tmp4F;});}else{_LL3: _tmp53=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp4E)->f1;_LL4: {
# 275
struct Cyc_Splay_node*n=_tmp53->v;
return(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=1U,({struct Cyc_Splay_noderef*_tmp84=({struct Cyc_Splay_noderef*_tmp51=_cycalloc(sizeof(*_tmp51));({struct Cyc_Splay_node*_tmp83=({struct Cyc_Splay_node*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->key=n->key,({void*_tmp82=f(env,n->data);_tmp50->data=_tmp82;}),({
void*_tmp81=Cyc_SlowDict_map_tree_c(f,env,n->left);_tmp50->left=_tmp81;}),({
void*_tmp80=Cyc_SlowDict_map_tree_c(f,env,n->right);_tmp50->right=_tmp80;});_tmp50;});
# 276
_tmp51->v=_tmp83;});_tmp51;});_tmp52->f1=_tmp84;});_tmp52;});}}_LL0:;}
# 281
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
return({struct Cyc_SlowDict_Dict*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->reln=d->reln,({void*_tmp85=Cyc_SlowDict_map_tree_c(f,env,d->tree);_tmp54->tree=_tmp85;});_tmp54;});}
# 285
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d){
void*_tmp55=d->tree;void*_tmp56=_tmp55;struct Cyc_Splay_noderef*_tmp58;if(((struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp56)->tag == 0U){_LL1: _LL2:
(int)_throw((void*)& Cyc_SlowDict_Absent_val);}else{_LL3: _tmp58=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp56)->f1;_LL4:
 return({struct _tuple0*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=(_tmp58->v)->key,_tmp57->f2=(_tmp58->v)->data;_tmp57;});}_LL0:;}
# 292
struct Cyc_List_List*Cyc_SlowDict_to_list_f(void*k,void*v,struct Cyc_List_List*accum){
return({struct Cyc_List_List*_tmp5A=_cycalloc(sizeof(*_tmp5A));({struct _tuple0*_tmp86=({struct _tuple0*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=k,_tmp59->f2=v;_tmp59;});_tmp5A->hd=_tmp86;}),_tmp5A->tl=accum;_tmp5A;});}
# 296
struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*accum))Cyc_SlowDict_fold)(Cyc_SlowDict_to_list_f,d,0);}

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

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct
 _dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  unsigned delta = sz * numelts;
  /* Don't let someone decrease the size so much that it wraps around.
   * This is crucial to get NULL right. */
  if (x.last_plus_one - x.base >= delta)
    x.last_plus_one -= delta;
  else x.last_plus_one = x.base;
  return x; 
}

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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;
# 177
void Cyc_Core_ufree(void*ptr);
# 190
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 239
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*file,const char*func,int lineno);
# 257 "core.h"
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
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 507
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 509
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 892
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 899
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 928
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 930
extern void*Cyc_Absyn_sint_type;
# 935
extern void*Cyc_Absyn_heap_rgn_type;
# 939
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 941
extern void*Cyc_Absyn_void_type;
# 958
void*Cyc_Absyn_exn_type();
# 966
extern void*Cyc_Absyn_fat_bound_type;
# 970
void*Cyc_Absyn_bounds_one();
# 980
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 986
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq);
# 988
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 990
void*Cyc_Absyn_void_star_type();
# 992
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_type(struct _tuple1*name);
# 998
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 1003
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1096
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1098
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1103
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1107
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1112
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1155
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1158
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1168
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1170
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1172
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1181
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1186
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
int Cyc_Tcutil_is_void_type(void*);
# 46
int Cyc_Tcutil_is_arithmetic_type(void*);
# 48
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 53
int Cyc_Tcutil_is_dyneither_ptr(void*t);
# 55
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
# 58
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 68
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 76
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 79
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 81
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 83
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 89
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 104
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 107
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 111
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
# 114
void*Cyc_Tcutil_compress(void*t);
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 152
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 196
int Cyc_Tcutil_unify(void*,void*);
# 199
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 201
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 203
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 288 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 296
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 350
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 373
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 376
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 387
int Cyc_Tcutil_force_type2bool(int desired,void*t);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_Hashtable_Table;
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
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;
void Cyc_Toc_finish();struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 61 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 76 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 78
({struct _dyneither_ptr _tmp7C4=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7C3=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp7C3,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7C4,ap);});}
# 80
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 82
({struct _dyneither_ptr _tmp7C6=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7C5=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp7C5,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7C6,ap);});}
# 86
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0U;
unsigned int Cyc_Toc_bounds_checks_eliminated=0U;
# 91
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 94
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 104
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 106
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple15{struct _tuple1*f1;struct _tuple1*f2;};
# 132
static int Cyc_Toc_qvar_tag_cmp(struct _tuple15*x,struct _tuple15*y){
struct _tuple15 _tmp2=*x;struct _tuple15 _tmp3=_tmp2;struct _tuple1*_tmp9;struct _tuple1*_tmp8;_LL1: _tmp9=_tmp3.f1;_tmp8=_tmp3.f2;_LL2:;{
struct _tuple15 _tmp4=*y;struct _tuple15 _tmp5=_tmp4;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL4: _tmp7=_tmp5.f1;_tmp6=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp9,_tmp7);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp8,_tmp6);};};}
# 141
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp11=_region_malloc(d,sizeof(*_tmp11));
({struct Cyc_List_List**_tmp7D2=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp7D2;}),({
struct Cyc_List_List**_tmp7D1=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp7D1;}),({
struct Cyc_Dict_Dict*_tmp7D0=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp7CF=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp7CF;});_tmpC;});_tmp11->aggrs_so_far=_tmp7D0;}),({
struct Cyc_List_List**_tmp7CE=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp7CE;}),({
struct Cyc_Set_Set**_tmp7CD=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp7CC=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp7CC;});_tmpE;});_tmp11->datatypes_so_far=_tmp7CD;}),({
struct Cyc_Dict_Dict*_tmp7CB=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp7CA=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp7CA;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp7CB;}),({
struct Cyc_Dict_Dict*_tmp7C9=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp7C8=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp7C8;});_tmp10;});_tmp11->qvar_tags=_tmp7C9;}),({
struct Cyc_Xarray_Xarray*_tmp7C7=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp7C7;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 160
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 166
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 169
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp7D3=({struct Cyc_Toc_TocStateWrap _tmp78C;_tmp78C.dyn=_tmp1C,_tmp78C.state=_tmp1B;_tmp78C;});*ts=_tmp7D3;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp1A=({struct _tuple16 _tmp78D;_tmp78D.f1=_tmp1B,_tmp78D.f2=arg;_tmp78D;});
({void*_tmp7D4=f(h,& _tmp1A);res=_tmp7D4;});}
# 177
;}
# 179
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)));
return res;};};};}struct _tuple17{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple18{struct Cyc_Toc_TocState*f1;struct _tuple17*f2;};struct _tuple19{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 183
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple18*env){
# 186
struct _tuple18 _tmp1D=*env;struct _tuple18 _tmp1E=_tmp1D;struct Cyc_Toc_TocState*_tmp24;struct _tuple1*_tmp23;void*(*_tmp22)(struct _tuple1*);_LL1: _tmp24=_tmp1E.f1;_tmp23=(_tmp1E.f2)->f1;_tmp22=(_tmp1E.f2)->f2;_LL2:;{
struct _tuple19**v=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp24->aggrs_so_far,_tmp23);
if(v == 0)
return _tmp22(_tmp23);else{
# 191
struct _tuple19*_tmp1F=*v;struct _tuple19*_tmp20=_tmp1F;void*_tmp21;_LL4: _tmp21=_tmp20->f2;_LL5:;
return _tmp21;}};}
# 196
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple17 env=({struct _tuple17 _tmp78E;_tmp78E.f1=q,_tmp78E.f2=type_maker;_tmp78E;});
return((void*(*)(struct _tuple17*arg,void*(*f)(struct _RegionHandle*,struct _tuple18*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp25[5U]="curr";
# 209 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp25,_tmp25,_tmp25 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp26[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp26,_tmp26,_tmp26 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp27[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp27,_tmp27,_tmp27 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp28[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp28,_tmp28,_tmp28 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp29[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp29,_tmp29,_tmp29 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2A[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2B[17U]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2B,_tmp2B,_tmp2B + 17U};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp2C[16U]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp2C,_tmp2C,_tmp2C + 16U};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;static char _tmp2D[7U]="_throw";
# 230 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp2D,_tmp2D,_tmp2D + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp2E[7U]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp2E,_tmp2E,_tmp2E + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp2F[14U]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp2F,_tmp2F,_tmp2F + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp30[13U]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp30,_tmp30,_tmp30 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp31[12U]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp31,_tmp31,_tmp31 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp32[14U]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp32,_tmp32,_tmp32 + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp33[12U]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp33,_tmp33,_tmp33 + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp34[28U]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp34,_tmp34,_tmp34 + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp35[31U]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp35,_tmp35,_tmp35 + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[27U]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp36,_tmp36,_tmp36 + 27U};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0U,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1U,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp37[15U]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp37,_tmp37,_tmp37 + 15U};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0U,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1U,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp38[15U]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp38,_tmp38,_tmp38 + 15U};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0U,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1U,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp39[21U]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39 + 21U};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0U,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1U,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3A[20U]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3A,_tmp3A,_tmp3A + 20U};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0U,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1U,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3B[19U]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3B,_tmp3B,_tmp3B + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3C[24U]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3C,_tmp3C,_tmp3C + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp3D[25U]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp3D,_tmp3D,_tmp3D + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp3E[23U]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp3E,_tmp3E,_tmp3E + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp3F[25U]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp3F,_tmp3F,_tmp3F + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp40[26U]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp40,_tmp40,_tmp40 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp41[30U]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp41,_tmp41,_tmp41 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp42[28U]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp42,_tmp42,_tmp42 + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp43[20U]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp43,_tmp43,_tmp43 + 20U};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp44[15U]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp44,_tmp44,_tmp44 + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp45[20U]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp45,_tmp45,_tmp45 + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp46[21U]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp46,_tmp46,_tmp46 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp47[19U]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp47,_tmp47,_tmp47 + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp48[21U]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp48,_tmp48,_tmp48 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp49[22U]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp49,_tmp49,_tmp49 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp4A[26U]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp4A,_tmp4A,_tmp4A + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp4B[24U]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp4B,_tmp4B,_tmp4B + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp4C[28U]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp4C,_tmp4C,_tmp4C + 28U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp4D[23U]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp4D,_tmp4D,_tmp4D + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp4E[28U]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp4E,_tmp4E,_tmp4E + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp4F[29U]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp4F,_tmp4F,_tmp4F + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp50[27U]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp50,_tmp50,_tmp50 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp51[29U]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp51,_tmp51,_tmp51 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp52[30U]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp52,_tmp52,_tmp52 + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp53[34U]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp53,_tmp53,_tmp53 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp54[32U]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp54,_tmp54,_tmp54 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp55[33U]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp55,_tmp55,_tmp55 + 33U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp56[28U]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp56,_tmp56,_tmp56 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp57[33U]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp57,_tmp57,_tmp57 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp58[34U]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp58,_tmp58,_tmp58 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp59[32U]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp59,_tmp59,_tmp59 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp5A[34U]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp5A,_tmp5A,_tmp5A + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp5B[35U]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp5B,_tmp5B,_tmp5B + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp5C[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp5C,_tmp5C,_tmp5C + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp5D[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp5D,_tmp5D,_tmp5D + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp5E[10U]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp5E,_tmp5E,_tmp5E + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp5F[11U]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp5F,_tmp5F,_tmp5F + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp60[17U]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp60,_tmp60,_tmp60 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp61[18U]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp61,_tmp61,_tmp61 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp62[15U]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp62,_tmp62,_tmp62 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp63[15U]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp63,_tmp63,_tmp63 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp64[13U]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp64,_tmp64,_tmp64 + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp65[12U]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp65,_tmp65,_tmp65 + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp66[13U]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp66,_tmp66,_tmp66 + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp67[12U]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp67,_tmp67,_tmp67 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp68[19U]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp68,_tmp68,_tmp68 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp69[29U]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp69,_tmp69,_tmp69 + 29U};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0U,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmp6A[13U]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmp6A,_tmp6A,_tmp6A + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp6B[9U]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmp6B,_tmp6B,_tmp6B + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp6C[20U]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp6C,_tmp6C,_tmp6C + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 298
void*Cyc_Toc_dyneither_ptr_type(){
static void*dpt=0;
void*_tmp6D=dpt;
if(_tmp6D == 0){
({void*_tmp7D5=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0),0);_tmp6D=_tmp7D5;});
dpt=_tmp6D;}
# 305
return _tmp6D;}
# 308
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 310
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 314
static struct _tuple20*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple20*_tmp70=_cycalloc(sizeof(*_tmp70));({struct Cyc_List_List*_tmp7D7=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));({void*_tmp7D6=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=1U,_tmp6E->f1=name;_tmp6E;});_tmp6F->hd=_tmp7D6;}),_tmp6F->tl=0;_tmp6F;});_tmp70->f1=_tmp7D7;}),_tmp70->f2=e;_tmp70;});}
# 320
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp71=e->r;void*_tmp72=_tmp71;struct Cyc_Absyn_Exp*_tmp73;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp73=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp73);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 326
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=14U,_tmp74->f1=t,_tmp74->f2=e,_tmp74->f3=0,_tmp74->f4=Cyc_Absyn_No_coercion;_tmp74;});}
# 329
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=20U,_tmp75->f1=e;_tmp75;});}
# 332
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=23U,_tmp76->f1=e1,_tmp76->f2=e2;_tmp76;});}
# 335
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=37U,_tmp77->f1=s;_tmp77;});}
# 338
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=17U,_tmp78->f1=t;_tmp78;});}
# 341
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=10U,_tmp79->f1=e,_tmp79->f2=es,_tmp79->f3=0,_tmp79->f4=1;_tmp79;});}
# 344
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=1U,_tmp7A->f1=e;_tmp7A;});}
# 347
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=2U,_tmp7B->f1=s1,_tmp7B->f2=s2;_tmp7B;});}
# 350
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=6U,_tmp7C->f1=e1,_tmp7C->f2=e2,_tmp7C->f3=e3;_tmp7C;});}
# 353
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=21U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});}
# 356
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=22U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});}
# 359
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 362
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=36U,_tmp7F->f1=tdopt,_tmp7F->f2=ds;_tmp7F;});}
# 364
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=8U,_tmp80->f1=v;_tmp80;});}
# 367
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 371
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp81=e->r;void*_tmp82=_tmp81;struct Cyc_Absyn_Exp*_tmp83;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->tag == 20U){_LL1: _tmp83=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp83,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 390
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 399
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 408
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 417
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 428
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp84=Cyc_Tcutil_compress(t);void*_tmp85=_tmp84;enum Cyc_Absyn_Size_of _tmp8C;switch(*((int*)_tmp85)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)){case 1U: _LL1: _tmp8C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f2;_LL2:
# 432
{enum Cyc_Absyn_Size_of _tmp86=_tmp8C;switch(_tmp86){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp87=0U;({struct _dyneither_ptr _tmp7D8=({const char*_tmp88="impossible IntCon (not char, short or int)";_tag_dyneither(_tmp88,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7D8,_tag_dyneither(_tmp87,sizeof(void*),0U));});});}_LLD:;}
# 438
goto _LL0;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}default: goto _LLB;}case 3U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp78F;_tmp78F.tag=0U,({struct _dyneither_ptr _tmp7D9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp78F.f1=_tmp7D9;});_tmp78F;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({struct _dyneither_ptr _tmp7DA=({const char*_tmp8A="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8A,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7DA,_tag_dyneither(_tmp89,sizeof(void*),1U));});});}_LL0:;}
# 445
return function;}
# 447
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8D=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8E=_tmp8D;void*_tmp91;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->tag == 3U){_LL1: _tmp91=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp91);}else{_LL3: _LL4:
({void*_tmp8F=0U;({struct _dyneither_ptr _tmp7DB=({const char*_tmp90="impossible type (not pointer)";_tag_dyneither(_tmp90,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7DB,_tag_dyneither(_tmp8F,sizeof(void*),0U));});});}_LL0:;}
# 457
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp92=e->r;void*_tmp93=_tmp92;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;long long _tmp99;int _tmp98;short _tmp97;struct _dyneither_ptr _tmp96;char _tmp95;switch(*((int*)_tmp93)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Null_c).tag){case 2U: _LL1: _tmp95=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Char_c).val).f2;_LL2:
 return _tmp95 == '\000';case 3U: _LL3: _tmp96=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Wchar_c).val;_LL4: {
# 461
unsigned long _tmp94=Cyc_strlen((struct _dyneither_ptr)_tmp96);
int i=0;
if(_tmp94 >= 2  && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp96.curr)[1]== 'x'  && _tmp94 >= 3) && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp94;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 471
return 0;}}case 4U: _LL5: _tmp97=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Short_c).val).f2;_LL6:
 return _tmp97 == 0;case 5U: _LL7: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Int_c).val).f2;_LL8:
 return _tmp98 == 0;case 6U: _LL9: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).LongLong_c).val).f2;_LLA:
 return _tmp99 == 0;case 1U: _LLD: _LLE:
# 476
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 475
 goto _LLE;case 14U: _LLF: _tmp9A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL10:
# 477
 return Cyc_Toc_is_zero(_tmp9A);case 24U: _LL11: _tmp9B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9B);case 26U: _LL13: _tmp9C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL14:
 _tmp9D=_tmp9C;goto _LL16;case 29U: _LL15: _tmp9D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_LL16:
 _tmp9E=_tmp9D;goto _LL18;case 25U: _LL17: _tmp9E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL18:
 _tmp9F=_tmp9E;goto _LL1A;case 36U: _LL19: _tmp9F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL1A:
# 483
 for(0;_tmp9F != 0;_tmp9F=_tmp9F->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple20*)_tmp9F->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 492
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA0=x;union Cyc_Absyn_Nmspace _tmpB2;struct _dyneither_ptr*_tmpB1;_LL1: _tmpB2=_tmpA0->f1;_tmpB1=_tmpA0->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA1=_tmpB2;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpAE;switch((_tmpA1.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAE=0;goto _LL7;case 1U: _LL6: _tmpAE=(_tmpA1.Rel_n).val;_LL7:
 _tmpAF=_tmpAE;goto _LL9;case 2U: _LL8: _tmpAF=(_tmpA1.Abs_n).val;_LL9:
 _tmpB0=_tmpAF;goto _LLB;default: _LLA: _tmpB0=(_tmpA1.C_n).val;_LLB:
# 501
 if(_tmpB0 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp791;_tmp791.tag=0U,_tmp791.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp791;});struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp790;_tmp790.tag=0U,_tmp790.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp790;});void*_tmpA2[2U];_tmpA2[0]=& _tmpA4,_tmpA2[1]=& _tmpA5;({struct _dyneither_ptr _tmp7DC=({const char*_tmpA3="%s_%s_struct";_tag_dyneither(_tmpA3,sizeof(char),13U);});Cyc_aprintf(_tmp7DC,_tag_dyneither(_tmpA2,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA6=_new_region("r");struct _RegionHandle*r=& _tmpA6;_push_region(r);
{struct _dyneither_ptr _tmpAD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp794;_tmp794.tag=0U,_tmp794.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp794;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp793;_tmp793.tag=0U,({struct _dyneither_ptr _tmp7DF=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp7DE=r;struct Cyc_List_List*_tmp7DD=_tmpB0;Cyc_rstr_sepstr(_tmp7DE,_tmp7DD,({const char*_tmpAC="_";_tag_dyneither(_tmpAC,sizeof(char),2U);}));}));_tmp793.f1=_tmp7DF;});_tmp793;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp792;_tmp792.tag=0U,_tmp792.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp792;});void*_tmpA7[3U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA,_tmpA7[2]=& _tmpAB;({struct _dyneither_ptr _tmp7E0=({const char*_tmpA8="%s_%s_%s_struct";_tag_dyneither(_tmpA8,sizeof(char),16U);});Cyc_aprintf(_tmp7E0,_tag_dyneither(_tmpA7,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAD;};_pop_region(r);};}_LL3:;};}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 514
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 517
struct _tuple21 _tmpB3=*env;struct _tuple21 _tmpB4=_tmpB3;struct Cyc_Dict_Dict*_tmpC7;struct _tuple15*_tmpC6;_LL1: _tmpC7=(_tmpB4.f1)->qvar_tags;_tmpC6=_tmpB4.f2;_LL2:;{
struct _tuple15 _tmpB5=*_tmpC6;struct _tuple15 _tmpB6=_tmpB5;struct _tuple1*_tmpC5;struct _tuple1*_tmpC4;_LL4: _tmpC5=_tmpB6.f1;_tmpC4=_tmpB6.f2;_LL5:;{
struct _handler_cons _tmpB7;_push_handler(& _tmpB7);{int _tmpB9=0;if(setjmp(_tmpB7.handler))_tmpB9=1;if(!_tmpB9){{struct _tuple1*_tmpBA=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*_tmpC7,Cyc_Toc_qvar_tag_cmp,_tmpC6);_npop_handler(0U);return _tmpBA;};_pop_handler();}else{void*_tmpB8=(void*)_exn_thrown;void*_tmpBB=_tmpB8;void*_tmpC3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBB)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 521
struct _tuple15*_tmpBC=({struct _tuple15*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->f1=_tmpC5,_tmpC2->f2=_tmpC4;_tmpC2;});
struct _tuple1*_tmpBD=_tmpC5;union Cyc_Absyn_Nmspace _tmpC1;struct _dyneither_ptr*_tmpC0;_LLC: _tmpC1=_tmpBD->f1;_tmpC0=_tmpBD->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC0,_tmpC4);
struct _tuple1*res=({struct _tuple1*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->f1=_tmpC1,({struct _dyneither_ptr*_tmp7E1=({struct _dyneither_ptr*_tmpBE=_cycalloc(sizeof(*_tmpBE));*_tmpBE=newvar;_tmpBE;});_tmpBF->f2=_tmp7E1;});_tmpBF;});
({struct Cyc_Dict_Dict _tmp7E2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC7,_tmpBC,res);*_tmpC7=_tmp7E2;});
return res;};}}else{_LL9: _tmpC3=_tmpBB;_LLA:(int)_rethrow(_tmpC3);}_LL6:;}};};};}
# 530
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp795;_tmp795.f1=fieldname,_tmp795.f2=dtname;_tmp795;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 537
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->kind=Cyc_Absyn_StructA,_tmpCA->sc=Cyc_Absyn_Public,_tmpCA->tvs=0,_tmpCA->attributes=0,_tmpCA->expected_mem_kind=0,({
# 540
struct _tuple1*_tmp7E5=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));({union Cyc_Absyn_Nmspace _tmp7E4=Cyc_Absyn_Rel_n(0);_tmpC8->f1=_tmp7E4;}),_tmpC8->f2=name;_tmpC8;});_tmpCA->name=_tmp7E5;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp7E3=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->exist_vars=0,_tmpC9->rgn_po=0,_tmpC9->tagged=0,_tmpC9->fields=fs;_tmpC9;});_tmpCA->impl=_tmp7E3;});_tmpCA;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 549
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 552
struct _tuple22 _tmpCB=*env;struct _tuple22 _tmpCC=_tmpCB;struct Cyc_List_List**_tmpE2;struct Cyc_List_List*_tmpE1;_LL1: _tmpE2=(_tmpCC.f1)->tuple_types;_tmpE1=_tmpCC.f2;_LL2:;
# 554
{struct Cyc_List_List*_tmpCD=*_tmpE2;for(0;_tmpCD != 0;_tmpCD=_tmpCD->tl){
struct _tuple23*_tmpCE=(struct _tuple23*)_tmpCD->hd;struct _tuple23*_tmpCF=_tmpCE;void*_tmpD2;struct Cyc_List_List*_tmpD1;_LL4: _tmpD2=_tmpCF->f1;_tmpD1=_tmpCF->f2;_LL5:;{
struct Cyc_List_List*_tmpD0=_tmpE1;
for(0;_tmpD0 != 0  && _tmpD1 != 0;(_tmpD0=_tmpD0->tl,_tmpD1=_tmpD1->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD0->hd)).f2,(void*)_tmpD1->hd))
break;}
if(_tmpD0 == 0  && _tmpD1 == 0)
return _tmpD2;};}}{
# 565
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct _dyneither_ptr _tmp7E7=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDF=({struct Cyc_Int_pa_PrintArg_struct _tmp796;_tmp796.tag=1U,_tmp796.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp796;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({struct _dyneither_ptr _tmp7E6=({const char*_tmpDE="_tuple%d";_tag_dyneither(_tmpDE,sizeof(char),9U);});Cyc_aprintf(_tmp7E6,_tag_dyneither(_tmpDD,sizeof(void*),1U));});});*_tmpE0=_tmp7E7;});_tmpE0;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE1);
struct Cyc_List_List*_tmpD3=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp7EA=({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct Cyc_Absyn_Aggrfield*_tmp7E9=({struct Cyc_Absyn_Aggrfield*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct _dyneither_ptr*_tmp7E8=Cyc_Absyn_fieldname(i);_tmpD4->name=_tmp7E8;}),_tmpD4->tq=Cyc_Toc_mt_tq,_tmpD4->type=(void*)ts2->hd,_tmpD4->width=0,_tmpD4->attributes=0,_tmpD4->requires_clause=0;_tmpD4;});_tmpD5->hd=_tmp7E9;}),_tmpD5->tl=_tmpD3;_tmpD5;});_tmpD3=_tmp7EA;});}}
({struct Cyc_List_List*_tmp7EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD3);_tmpD3=_tmp7EB;});{
struct Cyc_Absyn_Aggrdecl*_tmpD6=Cyc_Toc_make_c_struct_defn(xname,_tmpD3);
void*_tmpD7=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDC=_cycalloc(sizeof(*_tmpDC));*_tmpDC=_tmpD6;_tmpDC;})),0);
({struct Cyc_List_List*_tmp7ED=({struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct Cyc_Absyn_Decl*_tmp7EC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=5U,_tmpD8->f1=_tmpD6;_tmpD8;}),0U);_tmpD9->hd=_tmp7EC;}),_tmpD9->tl=Cyc_Toc_result_decls;_tmpD9;});Cyc_Toc_result_decls=_tmp7ED;});
({struct Cyc_List_List*_tmp7EF=({struct Cyc_List_List*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));({struct _tuple23*_tmp7EE=({struct _tuple23*_tmpDA=_region_malloc(d,sizeof(*_tmpDA));_tmpDA->f1=_tmpD7,_tmpDA->f2=ts;_tmpDA;});_tmpDB->hd=_tmp7EE;}),_tmpDB->tl=*_tmpE2;_tmpDB;});*_tmpE2=_tmp7EF;});
return _tmpD7;};};}
# 578
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 583
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 586
struct _tuple25*_tmpE3=env;struct Cyc_List_List**_tmpF7;enum Cyc_Absyn_AggrKind _tmpF6;struct Cyc_List_List*_tmpF5;_LL1: _tmpF7=(_tmpE3->f1)->anon_aggr_types;_tmpF6=(_tmpE3->f2)->f1;_tmpF5=(_tmpE3->f2)->f2;_LL2:;
# 588
{struct Cyc_List_List*_tmpE4=*_tmpF7;for(0;_tmpE4 != 0;_tmpE4=_tmpE4->tl){
struct _tuple26*_tmpE5=(struct _tuple26*)_tmpE4->hd;struct _tuple26*_tmpE6=_tmpE5;void*_tmpE9;enum Cyc_Absyn_AggrKind _tmpE8;struct Cyc_List_List*_tmpE7;_LL4: _tmpE9=_tmpE6->f1;_tmpE8=_tmpE6->f2;_tmpE7=_tmpE6->f3;_LL5:;
if(_tmpF6 != _tmpE8)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE7,_tmpF5))
return _tmpE9;}}{
# 597
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF4=_cycalloc(sizeof(*_tmpF4));({struct _dyneither_ptr _tmp7F1=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF3=({struct Cyc_Int_pa_PrintArg_struct _tmp797;_tmp797.tag=1U,_tmp797.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp797;});void*_tmpF1[1U];_tmpF1[0]=& _tmpF3;({struct _dyneither_ptr _tmp7F0=({const char*_tmpF2="_tuple%d";_tag_dyneither(_tmpF2,sizeof(char),9U);});Cyc_aprintf(_tmp7F0,_tag_dyneither(_tmpF1,sizeof(void*),1U));});});*_tmpF4=_tmp7F1;});_tmpF4;});
struct Cyc_Absyn_Aggrdecl*_tmpEA=Cyc_Toc_make_c_struct_defn(xname,_tmpF5);
_tmpEA->kind=_tmpF6;{
void*_tmpEB=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF0=_cycalloc(sizeof(*_tmpF0));*_tmpF0=_tmpEA;_tmpF0;})),0);
({struct Cyc_List_List*_tmp7F3=({struct Cyc_List_List*_tmpED=_cycalloc(sizeof(*_tmpED));({struct Cyc_Absyn_Decl*_tmp7F2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->tag=5U,_tmpEC->f1=_tmpEA;_tmpEC;}),0U);_tmpED->hd=_tmp7F2;}),_tmpED->tl=Cyc_Toc_result_decls;_tmpED;});Cyc_Toc_result_decls=_tmp7F3;});
({struct Cyc_List_List*_tmp7F5=({struct Cyc_List_List*_tmpEF=_region_malloc(d,sizeof(*_tmpEF));({struct _tuple26*_tmp7F4=({struct _tuple26*_tmpEE=_region_malloc(d,sizeof(*_tmpEE));_tmpEE->f1=_tmpEB,_tmpEE->f2=_tmpF6,_tmpEE->f3=_tmpF5;_tmpEE;});_tmpEF->hd=_tmp7F4;}),_tmpEF->tl=*_tmpF7;_tmpEF;});*_tmpF7=_tmp7F5;});
return _tmpEB;};};}
# 605
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->f1=ak,_tmpF8->f2=fs;_tmpF8;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 614
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 623
struct _tuple28 _tmpF9=*env;struct _tuple28 _tmpFA=_tmpF9;struct Cyc_List_List**_tmp11D;struct _tuple1*_tmp11C;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_List_List*_tmp119;_LL1: _tmp11D=(_tmpFA.f1)->abs_struct_types;_tmp11C=(_tmpFA.f2)->f1;_tmp11B=(_tmpFA.f2)->f2;_tmp11A=(_tmpFA.f2)->f3;_tmp119=(_tmpFA.f2)->f4;_LL2:;
# 627
{struct Cyc_List_List*_tmpFB=*_tmp11D;for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){
struct _tuple29*_tmpFC=(struct _tuple29*)_tmpFB->hd;struct _tuple29*_tmpFD=_tmpFC;struct _tuple1*_tmp105;struct Cyc_List_List*_tmp104;void*_tmp103;_LL4: _tmp105=_tmpFD->f1;_tmp104=_tmpFD->f2;_tmp103=_tmpFD->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp105,_tmp11C)== 0  && ({
int _tmp7F6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11A);_tmp7F6 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp104);})){
int okay=1;
{struct Cyc_List_List*_tmpFE=_tmp11A;for(0;_tmpFE != 0;(_tmpFE=_tmpFE->tl,_tmp104=_tmp104->tl)){
void*_tmpFF=(void*)_tmpFE->hd;
void*_tmp100=(void*)((struct Cyc_List_List*)_check_null(_tmp104))->hd;
{struct Cyc_Absyn_Kind*_tmp101=Cyc_Tcutil_type_kind(_tmpFF);struct Cyc_Absyn_Kind*_tmp102=_tmp101;switch(((struct Cyc_Absyn_Kind*)_tmp102)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 638
 continue;default: _LLB: _LLC:
# 641
 if(Cyc_Tcutil_unify(_tmpFF,_tmp100) || ({void*_tmp7F7=Cyc_Toc_typ_to_c(_tmpFF);Cyc_Tcutil_unify(_tmp7F7,Cyc_Toc_typ_to_c(_tmp100));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 646
break;}}
# 648
if(okay)
return _tmp103;}}}{
# 655
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp118=_cycalloc(sizeof(*_tmp118));({struct _dyneither_ptr _tmp7F9=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp117=({struct Cyc_Int_pa_PrintArg_struct _tmp798;_tmp798.tag=1U,_tmp798.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp798;});void*_tmp115[1U];_tmp115[0]=& _tmp117;({struct _dyneither_ptr _tmp7F8=({const char*_tmp116="_tuple%d";_tag_dyneither(_tmp116,sizeof(char),9U);});Cyc_aprintf(_tmp7F8,_tag_dyneither(_tmp115,sizeof(void*),1U));});});*_tmp118=_tmp7F9;});_tmp118;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp106=0;
# 659
({struct Cyc_List_List*_tmp7FB=({struct Cyc_List_List*_tmp108=_region_malloc(d,sizeof(*_tmp108));({struct _tuple29*_tmp7FA=({struct _tuple29*_tmp107=_region_malloc(d,sizeof(*_tmp107));_tmp107->f1=_tmp11C,_tmp107->f2=_tmp11A,_tmp107->f3=x;_tmp107;});_tmp108->hd=_tmp7FA;}),_tmp108->tl=*_tmp11D;_tmp108;});*_tmp11D=_tmp7FB;});{
# 662
struct _RegionHandle _tmp109=_new_region("r");struct _RegionHandle*r=& _tmp109;_push_region(r);
{struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11B,_tmp11A);
for(0;_tmp119 != 0;_tmp119=_tmp119->tl){
struct Cyc_Absyn_Aggrfield*_tmp10B=(struct Cyc_Absyn_Aggrfield*)_tmp119->hd;
void*t=_tmp10B->type;
struct Cyc_List_List*atts=_tmp10B->attributes;
# 672
if((_tmp119->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t))) && !
Cyc_Tcutil_is_array_type(t))
({struct Cyc_List_List*_tmp7FD=({struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*_tmp10D));({void*_tmp7FC=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->tag=6U,_tmp10C->f1=0;_tmp10C;});_tmp10D->hd=_tmp7FC;}),_tmp10D->tl=atts;_tmp10D;});atts=_tmp7FD;});
({void*_tmp7FE=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10A,t));t=_tmp7FE;});
# 679
if(Cyc_Tcutil_unify(t,Cyc_Absyn_void_type))
({void*_tmp802=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=4U,({void*_tmp801=Cyc_Absyn_void_star_type();(_tmp10E->f1).elt_type=_tmp801;}),({struct Cyc_Absyn_Tqual _tmp800=Cyc_Absyn_empty_tqual(0U);(_tmp10E->f1).tq=_tmp800;}),({
struct Cyc_Absyn_Exp*_tmp7FF=Cyc_Absyn_uint_exp(0U,0U);(_tmp10E->f1).num_elts=_tmp7FF;}),(_tmp10E->f1).zero_term=Cyc_Absyn_false_type,(_tmp10E->f1).zt_loc=0U;_tmp10E;});
# 680
t=_tmp802;});
# 683
({struct Cyc_List_List*_tmp804=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Absyn_Aggrfield*_tmp803=({struct Cyc_Absyn_Aggrfield*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->name=_tmp10B->name,_tmp10F->tq=Cyc_Toc_mt_tq,_tmp10F->type=t,_tmp10F->width=_tmp10B->width,_tmp10F->attributes=atts,_tmp10F->requires_clause=0;_tmp10F;});_tmp110->hd=_tmp803;}),_tmp110->tl=_tmp106;_tmp110;});_tmp106=_tmp804;});}
# 685
({struct Cyc_List_List*_tmp805=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp106);_tmp106=_tmp805;});{
struct Cyc_Absyn_Aggrdecl*_tmp111=Cyc_Toc_make_c_struct_defn(xname,_tmp106);
({struct Cyc_List_List*_tmp807=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Decl*_tmp806=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=5U,_tmp112->f1=_tmp111;_tmp112;}),0U);_tmp113->hd=_tmp806;}),_tmp113->tl=Cyc_Toc_result_decls;_tmp113;});Cyc_Toc_result_decls=_tmp807;});{
void*_tmp114=x;_npop_handler(0U);return _tmp114;};};}
# 663
;_pop_region(r);};};}
# 691
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 695
struct _tuple27 env=({struct _tuple27 _tmp799;_tmp799.f1=struct_name,_tmp799.f2=type_vars,_tmp799.f3=type_args,_tmp799.f4=fields;_tmp799;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 703
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp80A=({struct _dyneither_ptr*_tmp121=_cycalloc(sizeof(*_tmp121));({struct _dyneither_ptr _tmp809=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp120=({struct Cyc_Int_pa_PrintArg_struct _tmp79A;_tmp79A.tag=1U,_tmp79A.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp79A;});void*_tmp11E[1U];_tmp11E[0]=& _tmp120;({struct _dyneither_ptr _tmp808=({const char*_tmp11F="_tmp%X";_tag_dyneither(_tmp11F,sizeof(char),7U);});Cyc_aprintf(_tmp808,_tag_dyneither(_tmp11E,sizeof(void*),1U));});});*_tmp121=_tmp809;});_tmp121;});_tmp122->f2=_tmp80A;});_tmp122;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 709
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp123=*env;struct _tuple30 _tmp124=_tmp123;struct Cyc_Xarray_Xarray*_tmp12C;_LL1: _tmp12C=(_tmp124.f1)->temp_labels;_LL2:;{
int _tmp125=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp80B=_tmp125;_tmp80B < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12C);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12C,_tmp125);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12B=_cycalloc(sizeof(*_tmp12B));({struct _dyneither_ptr _tmp80D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12A=({struct Cyc_Int_pa_PrintArg_struct _tmp79B;_tmp79B.tag=1U,_tmp79B.f1=(unsigned int)_tmp125;_tmp79B;});void*_tmp128[1U];_tmp128[0]=& _tmp12A;({struct _dyneither_ptr _tmp80C=({const char*_tmp129="_LL%X";_tag_dyneither(_tmp129,sizeof(char),6U);});Cyc_aprintf(_tmp80C,_tag_dyneither(_tmp128,sizeof(void*),1U));});});*_tmp12B=_tmp80D;});_tmp12B;});
if(({int _tmp80E=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12C,res);_tmp80E != _tmp125;}))
({void*_tmp126=0U;({struct _dyneither_ptr _tmp80F=({const char*_tmp127="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp127,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp80F,_tag_dyneither(_tmp126,sizeof(void*),0U));});});
return res;};};}
# 720
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 726
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp12D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp12D))->hd)->name)!= 0){
++ ans;
_tmp12D=_tmp12D->tl;}
# 733
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 739
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp12E=*a;struct _tuple9 _tmp12F=_tmp12E;struct _dyneither_ptr*_tmp133;struct Cyc_Absyn_Tqual _tmp132;void*_tmp131;_LL1: _tmp133=_tmp12F.f1;_tmp132=_tmp12F.f2;_tmp131=_tmp12F.f3;_LL2:;
return({struct _tuple9*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->f1=_tmp133,_tmp130->f2=_tmp132,({void*_tmp810=Cyc_Toc_typ_to_c(_tmp131);_tmp130->f3=_tmp810;});_tmp130;});}
# 762 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp134=Cyc_Tcutil_compress(t);void*_tmp135=_tmp134;void*_tmp13B;void*_tmp13A;struct Cyc_Absyn_Tqual _tmp139;struct Cyc_Absyn_Exp*_tmp138;void*_tmp137;unsigned int _tmp136;switch(*((int*)_tmp135)){case 4U: _LL1: _tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).elt_type;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).tq;_tmp138=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).num_elts;_tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zero_term;_tmp136=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zt_loc;_LL2:
# 765
 return({void*_tmp811=Cyc_Toc_typ_to_c_array(_tmp13A);Cyc_Absyn_cstar_type(_tmp811,_tmp139);});case 1U: _LL3: _tmp13B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp135)->f2;if(_tmp13B != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13B);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 771
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 773
struct Cyc_Absyn_Aggrfield*_tmp13C=({struct Cyc_Absyn_Aggrfield*_tmp13D=_cycalloc(sizeof(*_tmp13D));*_tmp13D=*f;_tmp13D;});
_tmp13C->type=new_type;
_tmp13C->requires_clause=0;
_tmp13C->tq=Cyc_Toc_mt_tq;
return _tmp13C;}
# 780
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 782
return;}
# 785
static void*Cyc_Toc_char_star_type(){
static void**cs=0;
if(cs == 0)
({void**_tmp813=({void**_tmp13E=_cycalloc(sizeof(*_tmp13E));({void*_tmp812=Cyc_Absyn_star_type(Cyc_Absyn_char_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type);*_tmp13E=_tmp812;});_tmp13E;});cs=_tmp813;});
return*cs;}
# 791
static void*Cyc_Toc_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp816=({void**_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp815=({void*_tmp814=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp814,Cyc_Toc_mt_tq);});*_tmp13F=_tmp815;});_tmp13F;});r=_tmp816;});
return*r;}
# 797
static void*Cyc_Toc_dyn_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp819=({void**_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp818=({void*_tmp817=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_type(_tmp817,Cyc_Toc_mt_tq);});*_tmp140=_tmp818;});_tmp140;});r=_tmp819;});
return*r;}
# 803
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp141=Cyc_Tcutil_compress(t);void*_tmp142=_tmp141;struct Cyc_Absyn_Tvar*_tmp143;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->tag == 2U){_LL1: _tmp143=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 809
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp144=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp145=_tmp144;if(((struct Cyc_Absyn_Kind*)_tmp145)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 816
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp146=t;struct Cyc_Absyn_Datatypedecl*_tmp18D;void**_tmp18C;struct Cyc_Absyn_Enumdecl*_tmp18B;struct Cyc_Absyn_Aggrdecl*_tmp18A;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp188;struct _tuple1*_tmp187;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Typedefdecl*_tmp185;void*_tmp184;enum Cyc_Absyn_AggrKind _tmp183;struct Cyc_List_List*_tmp182;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Tqual _tmp180;void*_tmp17F;struct Cyc_List_List*_tmp17E;int _tmp17D;struct Cyc_Absyn_VarargInfo*_tmp17C;struct Cyc_List_List*_tmp17B;void*_tmp17A;struct Cyc_Absyn_Tqual _tmp179;struct Cyc_Absyn_Exp*_tmp178;unsigned int _tmp177;void*_tmp176;struct Cyc_Absyn_Tqual _tmp175;void*_tmp174;struct Cyc_Absyn_Tvar*_tmp173;void**_tmp172;struct Cyc_List_List*_tmp171;struct _tuple1*_tmp170;union Cyc_Absyn_AggrInfo _tmp16F;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Datatypedecl*_tmp16D;struct Cyc_Absyn_Datatypefield*_tmp16C;switch(*((int*)_tmp146)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 831
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp16D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).val).f1;_tmp16C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).val).f2;_LLA:
# 833
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp16C->name,_tmp16D->name));}else{_LLB: _LLC:
# 835
({void*_tmp147=0U;({struct _dyneither_ptr _tmp81A=({const char*_tmp148="unresolved DatatypeFieldType";_tag_dyneither(_tmp148,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81A,_tag_dyneither(_tmp147,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 844
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp16F=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_tmp16E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f2;_LL1C:
# 891
{union Cyc_Absyn_AggrInfo _tmp158=_tmp16F;if((_tmp158.UnknownAggr).tag == 1){_LL45: _LL46:
 return t;}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 895
struct Cyc_Absyn_Aggrdecl*_tmp159=Cyc_Absyn_get_known_aggrdecl(_tmp16F);
if(_tmp159->expected_mem_kind){
# 898
if(_tmp159->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp79D;_tmp79D.tag=0U,({
struct _dyneither_ptr _tmp81B=(struct _dyneither_ptr)(_tmp159->kind == Cyc_Absyn_UnionA?({const char*_tmp15E="union";_tag_dyneither(_tmp15E,sizeof(char),6U);}):({const char*_tmp15F="struct";_tag_dyneither(_tmp15F,sizeof(char),7U);}));_tmp79D.f1=_tmp81B;});_tmp79D;});struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp79C;_tmp79C.tag=0U,({
struct _dyneither_ptr _tmp81C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp79C.f1=_tmp81C;});_tmp79C;});void*_tmp15A[2U];_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D;({struct _dyneither_ptr _tmp81D=({const char*_tmp15B="%s %s was never defined.";_tag_dyneither(_tmp15B,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp81D,_tag_dyneither(_tmp15A,sizeof(void*),2U));});});}
# 906
if(_tmp159->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp160=_tmp159->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp159->impl))->fields;
if(_tmp160 == 0)return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp160))->tl != 0;_tmp160=_tmp160->tl){;}{
void*_tmp161=((struct Cyc_Absyn_Aggrfield*)_tmp160->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp161))){
if(_tmp159->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp79E;_tmp79E.tag=0U,({struct _dyneither_ptr _tmp81E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp79E.f1=_tmp81E;});_tmp79E;});void*_tmp162[1U];_tmp162[0]=& _tmp164;({struct _dyneither_ptr _tmp81F=({const char*_tmp163="struct %s ended up being abstract.";_tag_dyneither(_tmp163,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp81F,_tag_dyneither(_tmp162,sizeof(void*),1U));});});{
# 919
struct _RegionHandle _tmp165=_new_region("r");struct _RegionHandle*r=& _tmp165;_push_region(r);
{struct Cyc_List_List*_tmp166=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp159->tvs,_tmp16E);
void*_tmp167=Cyc_Tcutil_rsubstitute(r,_tmp166,_tmp161);
if(Cyc_Toc_is_abstract_type(_tmp167)){void*_tmp168=Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp168;}{
void*_tmp169=Cyc_Toc_add_struct_type(_tmp159->name,_tmp159->tvs,_tmp16E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp159->impl))->fields);_npop_handler(0U);return _tmp169;};}
# 920
;_pop_region(r);};}
# 925
return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);};};};case 15U: _LL1D: _tmp170=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_LL1E:
 return t;case 16U: _LL1F: _tmp171=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp171);return t;case 4U: _LL23: _LL24:
# 937
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 942
 return Cyc_Absyn_void_star_type();}case 1U: _LL3: _tmp172=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp146)->f2;_LL4:
# 820
 if(*_tmp172 == 0){
*_tmp172=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 824
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp172));case 2U: _LL5: _tmp173=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->f1;_LL6:
# 826
 if((Cyc_Tcutil_tvar_kind(_tmp173,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 828
return Cyc_Absyn_void_type;else{
# 830
return Cyc_Absyn_void_star_type();}case 3U: _LLD: _tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp175=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_tq;_tmp174=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).ptr_atts).bounds;_LLE:
# 839
({void*_tmp820=Cyc_Toc_typ_to_c(_tmp176);_tmp176=_tmp820;});
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp174)== 0)
return Cyc_Toc_dyneither_ptr_type();else{
# 843
return Cyc_Absyn_star_type(_tmp176,Cyc_Absyn_heap_rgn_type,_tmp175,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).tq;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).num_elts;_tmp177=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).zt_loc;_LL14:
# 847
 return({void*_tmp824=Cyc_Toc_typ_to_c(_tmp17A);struct Cyc_Absyn_Tqual _tmp823=_tmp179;struct Cyc_Absyn_Exp*_tmp822=_tmp178;void*_tmp821=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp824,_tmp823,_tmp822,_tmp821,_tmp177);});case 5U: _LL15: _tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_tqual;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_type;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).args;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).c_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).cyc_varargs;_tmp17B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).attributes;_LL16: {
# 853
struct Cyc_List_List*_tmp149=0;
for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
void*_tmp14A=(void*)_tmp17B->hd;void*_tmp14B=_tmp14A;switch(*((int*)_tmp14B)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
({struct Cyc_List_List*_tmp825=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=(void*)_tmp17B->hd,_tmp14C->tl=_tmp149;_tmp14C;});_tmp149=_tmp825;});goto _LL35;}_LL35:;}{
# 864
struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17E);
if(_tmp17C != 0){
# 867
void*_tmp14E=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp17C->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp14F=({struct _tuple9*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->f1=_tmp17C->name,_tmp151->f2=_tmp17C->tq,_tmp151->f3=_tmp14E;_tmp151;});
({struct Cyc_List_List*_tmp827=({struct Cyc_List_List*_tmp826=_tmp14D;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp826,({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->hd=_tmp14F,_tmp150->tl=0;_tmp150;}));});_tmp14D=_tmp827;});}
# 871
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->tag=5U,(_tmp152->f1).tvars=0,(_tmp152->f1).effect=0,(_tmp152->f1).ret_tqual=_tmp180,({void*_tmp828=Cyc_Toc_typ_to_c(_tmp17F);(_tmp152->f1).ret_type=_tmp828;}),(_tmp152->f1).args=_tmp14D,(_tmp152->f1).c_varargs=_tmp17D,(_tmp152->f1).cyc_varargs=0,(_tmp152->f1).rgn_po=0,(_tmp152->f1).attributes=_tmp149,(_tmp152->f1).requires_clause=0,(_tmp152->f1).requires_relns=0,(_tmp152->f1).ensures_clause=0,(_tmp152->f1).ensures_relns=0;_tmp152;});};}case 6U: _LL17: _tmp181=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp146)->f1;_LL18: {
# 876
struct Cyc_List_List*_tmp153=0;
for(0;_tmp181 != 0;_tmp181=_tmp181->tl){
({struct Cyc_List_List*_tmp82B=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));({struct _tuple12*_tmp82A=({struct _tuple12*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->f1=(*((struct _tuple12*)_tmp181->hd)).f1,({void*_tmp829=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp181->hd)).f2);_tmp154->f2=_tmp829;});_tmp154;});_tmp155->hd=_tmp82A;}),_tmp155->tl=_tmp153;_tmp155;});_tmp153=_tmp82B;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp153));}case 7U: _LL19: _tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f1;_tmp182=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f2;_LL1A: {
# 885
struct Cyc_List_List*_tmp156=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp82E=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*_tmp157));({struct Cyc_Absyn_Aggrfield*_tmp82D=({struct Cyc_Absyn_Aggrfield*_tmp82C=(struct Cyc_Absyn_Aggrfield*)_tmp182->hd;Cyc_Toc_aggrfield_to_c(_tmp82C,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp182->hd)->type));});_tmp157->hd=_tmp82D;}),_tmp157->tl=_tmp156;_tmp157;});_tmp156=_tmp82E;});}
return({enum Cyc_Absyn_AggrKind _tmp82F=_tmp183;Cyc_Toc_add_anon_aggr_type(_tmp82F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp156));});}case 8U: _LL21: _tmp187=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f1;_tmp186=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f2;_tmp185=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f3;_tmp184=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f4;_LL22:
# 931
 if(_tmp184 == 0){
if(_tmp186 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=8U,_tmp16A->f1=_tmp187,_tmp16A->f2=0,_tmp16A->f3=_tmp185,_tmp16A->f4=0;_tmp16A;});else{
return t;}}else{
# 936
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=8U,_tmp16B->f1=_tmp187,_tmp16B->f2=0,_tmp16B->f3=_tmp185,({void*_tmp830=Cyc_Toc_typ_to_c(_tmp184);_tmp16B->f4=_tmp830;});_tmp16B;});}case 9U: _LL2B: _tmp188=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp146)->f1;_LL2C:
# 947
 return t;case 11U: _LL2D: _tmp189=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp146)->f1;_LL2E:
# 952
 if(_tmp189->topt != 0)return Cyc_Toc_typ_to_c((void*)_check_null(_tmp189->topt));else{
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)){case 0U: _LL2F: _tmp18A=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_LL30:
# 955
 Cyc_Toc_aggrdecl_to_c(_tmp18A,1);
if(_tmp18A->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18A->name,Cyc_Absyn_unionq_type);else{
return Cyc_Toc_aggrdecl_type(_tmp18A->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp18B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_LL32:
# 960
 Cyc_Toc_enumdecl_to_c(_tmp18B);
return t;default: _LL33: _tmp18D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_tmp18C=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f2;_LL34:
# 963
 Cyc_Toc_datatypedecl_to_c(_tmp18D);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp18C)));}}_LL0:;}
# 968
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp18E=t;void*_tmp190;struct Cyc_Absyn_Tqual _tmp18F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->tag == 4U){_LL1: _tmp190=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->f1).elt_type;_tmp18F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->f1).tq;_LL2:
# 971
 return({void*_tmp831=Cyc_Absyn_star_type(_tmp190,Cyc_Absyn_heap_rgn_type,_tmp18F,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp831,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 978
static int Cyc_Toc_atomic_type(void*t){
void*_tmp191=Cyc_Tcutil_compress(t);void*_tmp192=_tmp191;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A4;void*_tmp1A3;void*_tmp1A2;struct Cyc_List_List*_tmp1A1;switch(*((int*)_tmp192)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp1A2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1;_tmp1A1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL4: {
# 982
void*_tmp193=_tmp1A2;struct Cyc_Absyn_Datatypedecl*_tmp19D;struct Cyc_Absyn_Datatypefield*_tmp19C;union Cyc_Absyn_AggrInfo _tmp19B;switch(*((int*)_tmp193)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D:
 return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp19B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp193)->f1;_LL25:
# 990
{union Cyc_Absyn_AggrInfo _tmp194=_tmp19B;if((_tmp194.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 994
struct Cyc_Absyn_Aggrdecl*_tmp195=Cyc_Absyn_get_known_aggrdecl(_tmp19B);
if(_tmp195->impl == 0)
return 0;
{struct Cyc_List_List*_tmp196=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp195->impl))->fields;for(0;_tmp196 != 0;_tmp196=_tmp196->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp196->hd)->type))return 0;}}
return 1;};case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp19D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f1;_tmp19C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f2;_LL27:
# 1001
{struct Cyc_List_List*_tmp197=_tmp19C->typs;for(0;_tmp197 != 0;_tmp197=_tmp197->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp197->hd)).f2))return 0;}}
return 1;}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp19A=({struct Cyc_String_pa_PrintArg_struct _tmp79F;_tmp79F.tag=0U,({struct _dyneither_ptr _tmp832=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp79F.f1=_tmp832;});_tmp79F;});void*_tmp198[1U];_tmp198[0]=& _tmp19A;({struct _dyneither_ptr _tmp833=({const char*_tmp199="atomic_typ:  bad type %s";_tag_dyneither(_tmp199,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp833,_tag_dyneither(_tmp198,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 1006
 return 1;case 4U: _LL7: _tmp1A3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192)->f1).elt_type;_LL8:
 return Cyc_Toc_atomic_type(_tmp1A3);case 7U: _LL9: _tmp1A4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp192)->f2;_LLA:
# 1014
 for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp1A4->hd)->type))return 0;}
return 1;case 6U: _LLB: _tmp1A5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp192)->f1;_LLC:
# 1018
 for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp1A5->hd)).f2))return 0;}
return 1;case 3U: _LLD: _LLE:
# 1023
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp7A0;_tmp7A0.tag=0U,({struct _dyneither_ptr _tmp834=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp7A0.f1=_tmp834;});_tmp7A0;});void*_tmp19E[1U];_tmp19E[0]=& _tmp1A0;({struct _dyneither_ptr _tmp835=({const char*_tmp19F="atomic_typ:  bad type %s";_tag_dyneither(_tmp19F,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp835,_tag_dyneither(_tmp19E,sizeof(void*),1U));});});}_LL0:;}
# 1028
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A6=Cyc_Tcutil_compress(t);void*_tmp1A7=_tmp1A6;void*_tmp1A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A7)->tag == 3U){_LL1: _tmp1A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A7)->f1).elt_type;_LL2:
# 1031
 return Cyc_Tcutil_is_void_type(_tmp1A8);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1036
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1040
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1045
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A9=Cyc_Tcutil_compress(t);void*_tmp1AA=_tmp1A9;struct Cyc_List_List*_tmp1B6;union Cyc_Absyn_AggrInfo _tmp1B5;switch(*((int*)_tmp1AA)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->tag == 20U){_LL1: _tmp1B5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->f1;_LL2: {
# 1048
struct Cyc_Absyn_Aggrdecl*_tmp1AB=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);
if(_tmp1AB->impl == 0)
({void*_tmp1AC=0U;({struct _dyneither_ptr _tmp836=({const char*_tmp1AD="is_poly_field: type missing fields";_tag_dyneither(_tmp1AD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp836,_tag_dyneither(_tmp1AC,sizeof(void*),0U));});});
_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AB->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AA)->f2;_LL4: {
# 1053
struct Cyc_Absyn_Aggrfield*_tmp1AE=Cyc_Absyn_lookup_field(_tmp1B6,f);
if(_tmp1AE == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp7A1;_tmp7A1.tag=0U,_tmp7A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp7A1;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _dyneither_ptr _tmp837=({const char*_tmp1B0="is_poly_field: bad field %s";_tag_dyneither(_tmp1B0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp837,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AE->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp7A2;_tmp7A2.tag=0U,({struct _dyneither_ptr _tmp838=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp7A2.f1=_tmp838;});_tmp7A2;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp839=({const char*_tmp1B3="is_poly_field: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp839,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL0:;}
# 1064
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B7=e->r;void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct _dyneither_ptr*_tmp1BF;switch(*((int*)_tmp1B8)){case 21U: _LL1: _tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL2:
# 1067
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C0->topt),_tmp1BF) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL4: {
# 1070
void*_tmp1B9=Cyc_Tcutil_compress((void*)_check_null(_tmp1C2->topt));void*_tmp1BA=_tmp1B9;void*_tmp1BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->tag == 3U){_LL8: _tmp1BE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->f1).elt_type;_LL9:
# 1072
 return Cyc_Toc_is_poly_field(_tmp1BE,_tmp1C1) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp7A3;_tmp7A3.tag=0U,({struct _dyneither_ptr _tmp83A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C2->topt)));_tmp7A3.f1=_tmp83A;});_tmp7A3;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct _dyneither_ptr _tmp83B=({const char*_tmp1BC="is_poly_project: bad type %s";_tag_dyneither(_tmp1BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp83B,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1075
 return 0;}_LL0:;}
# 1080
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp83C=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp83C,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=s,_tmp1C3->tl=0;_tmp1C3;}),0U);});}
# 1084
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp83D=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp83D,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1088
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_type(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1094
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp83E=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp83E,({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1098
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp83F=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp83F,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_type(elt_type))
return({struct Cyc_Absyn_Exp*_tmp840=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp840,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=s,_tmp1C7[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1106
return({struct Cyc_Absyn_Exp*_tmp841=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp841,({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=s,_tmp1C8[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1109
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp842=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp842,({struct Cyc_Absyn_Exp*_tmp1C9[3U];_tmp1C9[0]=rgn,_tmp1C9[1]=s,_tmp1C9[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1113
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp843=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp843,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=e,_tmp1CA->tl=0;_tmp1CA;}),0U);});}
# 1116
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp844=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp844,({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=e,_tmp1CB->tl=0;_tmp1CB;}),0U);});}
# 1120
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp845=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp845,({void*_tmp1CC=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CC,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1125
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1133
int is_string=0;
{void*_tmp1CD=e->r;void*_tmp1CE=_tmp1CD;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1139
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp847=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct Cyc_Absyn_Decl*_tmp846=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->tag=0U,_tmp1CF->f1=vd;_tmp1CF;}),0U);_tmp1D0->hd=_tmp846;}),_tmp1D0->tl=Cyc_Toc_result_decls;_tmp1D0;});Cyc_Toc_result_decls=_tmp847;});
({struct Cyc_Absyn_Exp*_tmp848=Cyc_Absyn_var_exp(x,0U);xexp=_tmp848;});
({struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp849;});}else{
# 1149
({struct Cyc_Absyn_Exp*_tmp84B=({void*_tmp84A=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp84A,e);});xexp=_tmp84B;});
# 1151
({struct Cyc_Absyn_Exp*_tmp84D=({void*_tmp84C=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp84C,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp84D;});}
# 1153
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1D1[3U];({struct _tuple20*_tmp850=({struct _tuple20*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xexp;_tmp1D2;});_tmp1D1[0]=_tmp850;}),({
struct _tuple20*_tmp84F=({struct _tuple20*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xexp;_tmp1D3;});_tmp1D1[1]=_tmp84F;}),({
struct _tuple20*_tmp84E=({struct _tuple20*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->f1=0,_tmp1D4->f2=xplussz;_tmp1D4;});_tmp1D1[2]=_tmp84E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D1,sizeof(struct _tuple20*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1190 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D5=nv;int _tmp1D6;_LL1: _tmp1D6=_tmp1D5->toplevel;_LL2:;
return _tmp1D6;}
# 1194
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D7=nv;int*_tmp1D8;_LL1: _tmp1D8=_tmp1D7->in_lhs;_LL2:;
return*_tmp1D8;}
# 1200
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));_tmp1DB->break_lab=0,_tmp1DB->continue_lab=0,_tmp1DB->fallthru_info=0,_tmp1DB->toplevel=1,({
# 1205
int*_tmp852=({int*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));*_tmp1D9=0;_tmp1D9;});_tmp1DB->in_lhs=_tmp852;}),({
int*_tmp851=({int*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));*_tmp1DA=0;_tmp1DA;});_tmp1DB->in_sizeof=_tmp851;}),_tmp1DB->rgn=r;_tmp1DB;});}
# 1210
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DC=e;struct _dyneither_ptr**_tmp1E3;struct _dyneither_ptr**_tmp1E2;struct Cyc_Toc_FallthruInfo*_tmp1E1;int _tmp1E0;int*_tmp1DF;int*_tmp1DE;_LL1: _tmp1E3=_tmp1DC->break_lab;_tmp1E2=_tmp1DC->continue_lab;_tmp1E1=_tmp1DC->fallthru_info;_tmp1E0=_tmp1DC->toplevel;_tmp1DF=_tmp1DC->in_lhs;_tmp1DE=_tmp1DC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->break_lab=_tmp1E3,_tmp1DD->continue_lab=_tmp1E2,_tmp1DD->fallthru_info=_tmp1E1,_tmp1DD->toplevel=_tmp1E0,_tmp1DD->in_lhs=_tmp1DF,_tmp1DD->in_sizeof=_tmp1DE,_tmp1DD->rgn=r;_tmp1DD;});}
# 1215
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E4=e;struct _dyneither_ptr**_tmp1EB;struct _dyneither_ptr**_tmp1EA;struct Cyc_Toc_FallthruInfo*_tmp1E9;int _tmp1E8;int*_tmp1E7;int*_tmp1E6;_LL1: _tmp1EB=_tmp1E4->break_lab;_tmp1EA=_tmp1E4->continue_lab;_tmp1E9=_tmp1E4->fallthru_info;_tmp1E8=_tmp1E4->toplevel;_tmp1E7=_tmp1E4->in_lhs;_tmp1E6=_tmp1E4->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->break_lab=_tmp1EB,_tmp1E5->continue_lab=_tmp1EA,_tmp1E5->fallthru_info=_tmp1E9,_tmp1E5->toplevel=0,_tmp1E5->in_lhs=_tmp1E7,_tmp1E5->in_sizeof=_tmp1E6,_tmp1E5->rgn=r;_tmp1E5;});}
# 1219
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EC=e;struct _dyneither_ptr**_tmp1F3;struct _dyneither_ptr**_tmp1F2;struct Cyc_Toc_FallthruInfo*_tmp1F1;int _tmp1F0;int*_tmp1EF;int*_tmp1EE;_LL1: _tmp1F3=_tmp1EC->break_lab;_tmp1F2=_tmp1EC->continue_lab;_tmp1F1=_tmp1EC->fallthru_info;_tmp1F0=_tmp1EC->toplevel;_tmp1EF=_tmp1EC->in_lhs;_tmp1EE=_tmp1EC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->break_lab=_tmp1F3,_tmp1ED->continue_lab=_tmp1F2,_tmp1ED->fallthru_info=_tmp1F1,_tmp1ED->toplevel=1,_tmp1ED->in_lhs=_tmp1EF,_tmp1ED->in_sizeof=_tmp1EE,_tmp1ED->rgn=r;_tmp1ED;});}
# 1223
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F4=e;int*_tmp1F5;_LL1: _tmp1F5=_tmp1F4->in_lhs;_LL2:;
*_tmp1F5=b;}
# 1227
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F6=e;int*_tmp1F8;_LL1: _tmp1F8=_tmp1F6->in_sizeof;_LL2:;{
int _tmp1F7=*_tmp1F8;
*_tmp1F8=b;
return _tmp1F7;};}
# 1233
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F9=e;int*_tmp1FA;_LL1: _tmp1FA=_tmp1F9->in_sizeof;_LL2:;
return*_tmp1FA;}
# 1240
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FB=e;struct _dyneither_ptr**_tmp202;struct _dyneither_ptr**_tmp201;struct Cyc_Toc_FallthruInfo*_tmp200;int _tmp1FF;int*_tmp1FE;int*_tmp1FD;_LL1: _tmp202=_tmp1FB->break_lab;_tmp201=_tmp1FB->continue_lab;_tmp200=_tmp1FB->fallthru_info;_tmp1FF=_tmp1FB->toplevel;_tmp1FE=_tmp1FB->in_lhs;_tmp1FD=_tmp1FB->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));_tmp1FC->break_lab=0,_tmp1FC->continue_lab=0,_tmp1FC->fallthru_info=_tmp200,_tmp1FC->toplevel=_tmp1FF,_tmp1FC->in_lhs=_tmp1FE,_tmp1FC->in_sizeof=_tmp1FD,_tmp1FC->rgn=r;_tmp1FC;});}
# 1246
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1250
struct Cyc_Toc_Env*_tmp203=e;struct _dyneither_ptr**_tmp20C;struct _dyneither_ptr**_tmp20B;struct Cyc_Toc_FallthruInfo*_tmp20A;int _tmp209;int*_tmp208;int*_tmp207;_LL1: _tmp20C=_tmp203->break_lab;_tmp20B=_tmp203->continue_lab;_tmp20A=_tmp203->fallthru_info;_tmp209=_tmp203->toplevel;_tmp208=_tmp203->in_lhs;_tmp207=_tmp203->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp206=_region_malloc(r,sizeof(*_tmp206));
_tmp206->label=fallthru_l,_tmp206->binders=fallthru_binders;_tmp206;});
return({struct Cyc_Toc_Env*_tmp205=_region_malloc(r,sizeof(*_tmp205));({struct _dyneither_ptr**_tmp853=({struct _dyneither_ptr**_tmp204=_region_malloc(r,sizeof(*_tmp204));*_tmp204=break_l;_tmp204;});_tmp205->break_lab=_tmp853;}),_tmp205->continue_lab=_tmp20B,_tmp205->fallthru_info=fi,_tmp205->toplevel=_tmp209,_tmp205->in_lhs=_tmp208,_tmp205->in_sizeof=_tmp207,_tmp205->rgn=r;_tmp205;});};}
# 1256
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1258
struct Cyc_Toc_Env*_tmp20D=e;struct _dyneither_ptr**_tmp215;struct _dyneither_ptr**_tmp214;struct Cyc_Toc_FallthruInfo*_tmp213;int _tmp212;int*_tmp211;int*_tmp210;_LL1: _tmp215=_tmp20D->break_lab;_tmp214=_tmp20D->continue_lab;_tmp213=_tmp20D->fallthru_info;_tmp212=_tmp20D->toplevel;_tmp211=_tmp20D->in_lhs;_tmp210=_tmp20D->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp20F=_region_malloc(r,sizeof(*_tmp20F));({struct _dyneither_ptr**_tmp854=({struct _dyneither_ptr**_tmp20E=_region_malloc(r,sizeof(*_tmp20E));*_tmp20E=break_l;_tmp20E;});_tmp20F->break_lab=_tmp854;}),_tmp20F->continue_lab=_tmp214,_tmp20F->fallthru_info=0,_tmp20F->toplevel=_tmp212,_tmp20F->in_lhs=_tmp211,_tmp20F->in_sizeof=_tmp210,_tmp20F->rgn=r;_tmp20F;});}
# 1265
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1268
struct Cyc_Toc_Env*_tmp216=e;struct _dyneither_ptr**_tmp21E;struct _dyneither_ptr**_tmp21D;struct Cyc_Toc_FallthruInfo*_tmp21C;int _tmp21B;int*_tmp21A;int*_tmp219;_LL1: _tmp21E=_tmp216->break_lab;_tmp21D=_tmp216->continue_lab;_tmp21C=_tmp216->fallthru_info;_tmp21B=_tmp216->toplevel;_tmp21A=_tmp216->in_lhs;_tmp219=_tmp216->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->break_lab=0,_tmp218->continue_lab=_tmp21D,({struct Cyc_Toc_FallthruInfo*_tmp855=({struct Cyc_Toc_FallthruInfo*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->label=next_l,_tmp217->binders=0;_tmp217;});_tmp218->fallthru_info=_tmp855;}),_tmp218->toplevel=_tmp21B,_tmp218->in_lhs=_tmp21A,_tmp218->in_sizeof=_tmp219,_tmp218->rgn=r;_tmp218;});}
# 1281 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1284
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1286
void*_tmp21F=e->annot;void*_tmp220=_tmp21F;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _LL2: {
# 1289
void*_tmp221=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp222=_tmp221;void*_tmp225;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->tag == 3U){_LLC: _tmp225=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->f1).ptr_atts).nullable;_LLD:
# 1292
 return Cyc_Tcutil_force_type2bool(0,_tmp225);}else{_LLE: _LLF:
({void*_tmp223=0U;({struct _dyneither_ptr _tmp856=({const char*_tmp224="Toc::is_nullable";_tag_dyneither(_tmp224,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp856,_tag_dyneither(_tmp223,sizeof(void*),0U));});});}_LLB:;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _LL4:
# 1297
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1299
({void*_tmp226=0U;({unsigned int _tmp858=e->loc;struct _dyneither_ptr _tmp857=({const char*_tmp227="NULL pointer check will definitely fail";_tag_dyneither(_tmp227,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp858,_tmp857,_tag_dyneither(_tmp226,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
# 1305
 return 1;}else{_LL9: _LLA:
({void*_tmp228=0U;({struct _dyneither_ptr _tmp859=({const char*_tmp229="need_null_check";_tag_dyneither(_tmp229,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp859,_tag_dyneither(_tmp228,sizeof(void*),0U));});});}}}}_LL0:;}
# 1310
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp22A=e->annot;void*_tmp22B=_tmp22A;struct Cyc_List_List*_tmp231;struct Cyc_List_List*_tmp230;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp230=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL2:
 return _tmp230;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp231=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL4:
 return _tmp231;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1315
({void*_tmp22C=0U;({unsigned int _tmp85B=e->loc;struct _dyneither_ptr _tmp85A=({const char*_tmp22D="dereference of NULL pointer";_tag_dyneither(_tmp22D,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp85B,_tmp85A,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp22E=0U;({struct _dyneither_ptr _tmp85C=({const char*_tmp22F="get_relns";_tag_dyneither(_tmp22F,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85C,_tag_dyneither(_tmp22E,sizeof(void*),0U));});});}}}}_LL0:;}static char _tmp235[8U]="*bogus*";
# 1325
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1335 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1338
struct Cyc_Absyn_Vardecl*x;
{void*_tmp232=a->r;void*_tmp233=_tmp232;void*_tmp236;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->tag == 1U){_LL1: _tmp236=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->f1;_LL2: {
# 1341
struct Cyc_Absyn_Vardecl*_tmp234=Cyc_Tcutil_nonesc_vardecl(_tmp236);
if(_tmp234 == 0)goto _LL4;
x=_tmp234;
goto _LL0;}}else{_LL3: _LL4: {
# 1347
static struct _dyneither_ptr bogus_string={_tmp235,_tmp235,_tmp235 + 8U};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1350
static struct Cyc_Absyn_TupleType_Absyn_Type_struct tt={6U,0};
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0U,{0,0,0,0,0U},(void*)& tt,0,0,0,0};
# 1357
x=& bogus_vardecl;
x->type=a_typ;}}_LL0:;}{
# 1363
void*_tmp237=a_typ;
void*_tmp238=x->type;
x->type=_tmp237;{
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1372
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp237);
if(bound != 0){
# 1375
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
({struct Cyc_List_List*_tmp85D=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);relns=_tmp85D;});}
# 1382
if(valid_rop_i){
struct Cyc_List_List*_tmp239=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1390
int _tmp23A=Cyc_Relations_consistent_relations(_tmp239);
x->type=_tmp238;
if(!_tmp23A)return 1;}
# 1395
inner_loop: {
void*_tmp23B=i->r;void*_tmp23C=_tmp23B;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Absyn_Exp*_tmp241;void*_tmp240;struct Cyc_Absyn_Exp*_tmp23F;switch(*((int*)_tmp23C)){case 14U: _LL6: _tmp240=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23C)->f1;_tmp23F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23C)->f2;_LL7:
 i=_tmp23F;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->tl != 0){_LL8: _tmp242=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->hd;_tmp241=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->tl)->hd;_LL9: {
# 1401
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp241,& rop_z)){
# 1404
struct Cyc_List_List*_tmp23D=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1406
int _tmp23E=Cyc_Relations_consistent_relations(_tmp23D);
x->type=_tmp238;
return !_tmp23E;}
# 1410
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1413
x->type=_tmp238;
return 0;};};}
# 1417
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp243=0U;({struct _dyneither_ptr _tmp85E=({const char*_tmp244="Missing type in primop ";_tag_dyneither(_tmp244,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85E,_tag_dyneither(_tmp243,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1421
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->f1=Cyc_Toc_mt_tq,({void*_tmp85F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp245->f2=_tmp85F;});_tmp245;});}
# 1426
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp246=e->r;void*_tmp247=_tmp246;struct Cyc_Absyn_Exp*_tmp24F;int _tmp24E;struct Cyc_Absyn_Exp*_tmp24D;int _tmp24C;struct Cyc_List_List*_tmp24B;switch(*((int*)_tmp247)){case 26U: _LL1: _tmp24B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp247)->f1;_LL2:
# 1429
{struct Cyc_List_List*_tmp248=_tmp24B;for(0;_tmp248 != 0;_tmp248=_tmp248->tl){
if((*((struct _tuple20*)_tmp248->hd)).f1 != 0)
({void*_tmp249=0U;({struct _dyneither_ptr _tmp860=({const char*_tmp24A="array designators for abstract-field initialization";_tag_dyneither(_tmp24A,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp860,_tag_dyneither(_tmp249,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp861=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp24B),0U);_tmp24D=_tmp861;});_tmp24C=0;goto _LL4;case 27U: _LL3: _tmp24D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp247)->f2;_tmp24C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp247)->f4;_LL4:
 _tmp24F=_tmp24D;_tmp24E=_tmp24C;goto _LL6;case 28U: _LL5: _tmp24F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp247)->f1;_tmp24E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp247)->f3;_LL6:
# 1435
({struct Cyc_Absyn_Exp*_tmp862=Cyc_Absyn_copy_exp(_tmp24F);_tmp24F=_tmp862;});
return _tmp24E?({struct Cyc_Absyn_Exp*_tmp863=_tmp24F;Cyc_Absyn_add_exp(_tmp863,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp24F;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1444
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1452
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp250=e->r;void*_tmp251=_tmp250;struct Cyc_List_List*_tmp252;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp251)->tag == 29U){_LL1: _tmp252=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp251)->f3;_LL2:
 dles=_tmp252;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1458
{void*_tmp253=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp254=_tmp253;struct Cyc_Absyn_Aggrdecl*_tmp255;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp255=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->f1)->f1).KnownAggr).val;_LL7:
# 1460
 if(_tmp255->impl == 0)
return 0;
if(_tmp255->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp255->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1468
 return 0;}_LL5:;}
# 1470
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp256=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp257=(struct _tuple20*)dles->hd;struct _tuple20*_tmp258=_tmp257;struct Cyc_List_List*_tmp261;struct Cyc_Absyn_Exp*_tmp260;_LLB: _tmp261=_tmp258->f1;_tmp260=_tmp258->f2;_LLC:;{
struct _dyneither_ptr*_tmp259=Cyc_Absyn_designatorlist_to_fieldname(_tmp261);
if(!Cyc_strptrcmp(_tmp259,_tmp256->name)){
struct Cyc_Absyn_Exp*_tmp25A=Cyc_Toc_get_varsizeexp(_tmp260);
if(_tmp25A != 0)
return _tmp25A;{
void*_tmp25B=Cyc_Tcutil_compress(_tmp256->type);void*_tmp25C=_tmp25B;void*_tmp25F;struct Cyc_Absyn_Exp*_tmp25E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->tag == 4U){_LLE: _tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->f1).elt_type;_tmp25E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->f1).num_elts;_LLF:
# 1485
 if(_tmp25E == 0  || !Cyc_Toc_is_zero(_tmp25E))
return 0;
# 1491
return({struct Cyc_Absyn_Exp*_tmp866=({struct Cyc_Absyn_Exp*_tmp865=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp865,({struct Cyc_Absyn_Exp*_tmp25D[2U];_tmp25D[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp260)),({
# 1493
struct Cyc_Absyn_Exp*_tmp864=Cyc_Absyn_sizeoftype_exp(_tmp25F,0U);_tmp25D[1]=_tmp864;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25D,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1491
Cyc_Absyn_add_exp(_tmp866,
# 1494
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1499
({void*_tmp262=0U;({struct _dyneither_ptr _tmp867=({const char*_tmp263="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp263,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp867,_tag_dyneither(_tmp262,sizeof(void*),0U));});});};}
# 1502
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp264=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp264 != 0;_tmp264=_tmp264->tl){
struct Cyc_Absyn_Aggrfield*_tmp265=(struct Cyc_Absyn_Aggrfield*)_tmp264->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp265->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1509
({void*_tmp266=0U;({struct _dyneither_ptr _tmp869=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp269=({struct Cyc_String_pa_PrintArg_struct _tmp7A4;_tmp7A4.tag=0U,_tmp7A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp7A4;});void*_tmp267[1U];_tmp267[0]=& _tmp269;({struct _dyneither_ptr _tmp868=({const char*_tmp268="get_member_offset %s failed";_tag_dyneither(_tmp268,sizeof(char),28U);});Cyc_aprintf(_tmp868,_tag_dyneither(_tmp267,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp869,_tag_dyneither(_tmp266,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1513
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1516
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1519
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->tag=5U,_tmp26A->f1=e1,_tmp26A->f2=incr;_tmp26A;}),0U);}
# 1523
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1532
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1534
void*_tmp26B=e1->r;void*_tmp26C=_tmp26B;struct Cyc_Absyn_Exp*_tmp274;struct _dyneither_ptr*_tmp273;int _tmp272;int _tmp271;void*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Stmt*_tmp26E;switch(*((int*)_tmp26C)){case 37U: _LL1: _tmp26E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp26E,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp270=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp26F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp26F,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp274=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp273=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_tmp272=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_tmp271=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f4;_LL6:
# 1539
 e1->r=_tmp274->r;
({struct Cyc_Absyn_Exp*_tmp86C=e1;struct Cyc_List_List*_tmp86B=({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->hd=_tmp273,_tmp26D->tl=fs;_tmp26D;});struct Cyc_Absyn_Exp*(*_tmp86A)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp86C,_tmp86B,_tmp86A,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1547
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1549
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp86D=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp86D;});}
({void*_tmp86E=(f(e1_copy,f_env))->r;e1->r=_tmp86E;});
goto _LL0;}}_LL0:;}
# 1555
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1557
void*_tmp275=s->r;void*_tmp276=_tmp275;struct Cyc_Absyn_Stmt*_tmp27D;struct Cyc_Absyn_Decl*_tmp27C;struct Cyc_Absyn_Stmt*_tmp27B;struct Cyc_Absyn_Exp*_tmp27A;switch(*((int*)_tmp276)){case 1U: _LL1: _tmp27A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp276)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp27A,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp27C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp276)->f1;_tmp27B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp276)->f2;_LL4:
# 1560
 Cyc_Toc_lvalue_assign_stmt(_tmp27B,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp27D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp276)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp27D,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp279=({struct Cyc_String_pa_PrintArg_struct _tmp7A5;_tmp7A5.tag=0U,({struct _dyneither_ptr _tmp86F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp7A5.f1=_tmp86F;});_tmp7A5;});void*_tmp277[1U];_tmp277[0]=& _tmp279;({struct _dyneither_ptr _tmp870=({const char*_tmp278="lvalue_assign_stmt: %s";_tag_dyneither(_tmp278,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp870,_tag_dyneither(_tmp277,sizeof(void*),1U));});});}_LL0:;}
# 1566
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1570
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp27E=e->r;void*_tmp27F=_tmp27E;struct Cyc_Absyn_Stmt*_tmp286;struct Cyc_Absyn_Exp*_tmp285;void**_tmp284;struct Cyc_Absyn_Exp**_tmp283;switch(*((int*)_tmp27F)){case 14U: _LL1: _tmp284=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_tmp283=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27F)->f2;_LL2:
# 1573
({struct Cyc_Absyn_Exp*_tmp871=Cyc_Toc_push_address_exp(*_tmp283);*_tmp283=_tmp871;});
({void*_tmp872=Cyc_Absyn_cstar_type(*_tmp284,Cyc_Toc_mt_tq);*_tmp284=_tmp872;});
return e;case 20U: _LL3: _tmp285=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_LL4:
# 1577
 return _tmp285;case 37U: _LL5: _tmp286=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_LL6:
# 1581
 Cyc_Toc_push_address_stmt(_tmp286);
return e;default: _LL7: _LL8:
# 1584
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp7A6;_tmp7A6.tag=0U,({struct _dyneither_ptr _tmp873=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp7A6.f1=_tmp873;});_tmp7A6;});void*_tmp280[1U];_tmp280[0]=& _tmp282;({struct _dyneither_ptr _tmp874=({const char*_tmp281="can't take & of exp %s";_tag_dyneither(_tmp281,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp874,_tag_dyneither(_tmp280,sizeof(void*),1U));});});}_LL0:;}
# 1589
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp287=s->r;void*_tmp288=_tmp287;struct Cyc_Absyn_Exp**_tmp28E;struct Cyc_Absyn_Stmt*_tmp28D;struct Cyc_Absyn_Stmt*_tmp28C;switch(*((int*)_tmp288)){case 2U: _LL1: _tmp28C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL2:
 _tmp28D=_tmp28C;goto _LL4;case 12U: _LL3: _tmp28D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp28D);goto _LL0;case 1U: _LL5: _tmp28E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp875=Cyc_Toc_push_address_exp(*_tmp28E);*_tmp28E=_tmp875;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp28B=({struct Cyc_String_pa_PrintArg_struct _tmp7A7;_tmp7A7.tag=0U,({struct _dyneither_ptr _tmp876=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp7A7.f1=_tmp876;});_tmp7A7;});void*_tmp289[1U];_tmp289[0]=& _tmp28B;({struct _dyneither_ptr _tmp877=({const char*_tmp28A="can't take & of stmt %s";_tag_dyneither(_tmp28A,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp877,_tag_dyneither(_tmp289,sizeof(void*),1U));});});}_LL0:;}
# 1598
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp28F=Cyc_Tcutil_compress(t);void*_tmp290=_tmp28F;struct Cyc_Absyn_PtrInfo _tmp293;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp290)->tag == 3U){_LL1: _tmp293=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp290)->f1;_LL2:
 return _tmp293;}else{_LL3: _LL4:
({void*_tmp291=0U;({struct _dyneither_ptr _tmp878=({const char*_tmp292="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp292,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp878,_tag_dyneither(_tmp291,sizeof(void*),0U));});});}_LL0:;}
# 1608
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp294=Cyc_Tcutil_compress(t);void*_tmp295=_tmp294;void*_tmp2A6;switch(*((int*)_tmp295)){case 3U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp879=Cyc_Absyn_null_exp(0U);res=_tmp879;});goto _LL0;case 0U: _LL3: _tmp2A6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp295)->f1;_LL4:
# 1613
{void*_tmp296=_tmp2A6;int _tmp2A2;enum Cyc_Absyn_Sign _tmp2A1;enum Cyc_Absyn_Sign _tmp2A0;enum Cyc_Absyn_Sign _tmp29F;enum Cyc_Absyn_Sign _tmp29E;enum Cyc_Absyn_Sign _tmp29D;switch(*((int*)_tmp296)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f2){case Cyc_Absyn_Char_sz: _LL8: _tmp29D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f1;_LL9:
# 1615
({struct Cyc_Absyn_Exp*_tmp87A=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp29D,'\000'),0U);res=_tmp87A;});goto _LL7;case Cyc_Absyn_Short_sz: _LLA: _tmp29E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f1;_LLB:
# 1617
({struct Cyc_Absyn_Exp*_tmp87B=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp29E,0),0U);res=_tmp87B;});goto _LL7;case Cyc_Absyn_Int_sz: _LL10: _tmp29F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f1;_LL11:
# 1620
 _tmp2A0=_tmp29F;goto _LL13;case Cyc_Absyn_Long_sz: _LL12: _tmp2A0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f1;_LL13:
# 1622
({struct Cyc_Absyn_Exp*_tmp87C=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp2A0,0),0U);res=_tmp87C;});goto _LL7;default: _LL14: _tmp2A1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f1;_LL15:
# 1624
({struct Cyc_Absyn_Exp*_tmp87D=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp2A1,(long long)0),0U);res=_tmp87D;});goto _LL7;}case 15U: _LLC: _LLD:
# 1618
 goto _LLF;case 16U: _LLE: _LLF:
 _tmp29F=1U;goto _LL11;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp296)->f1){case 0U: _LL16: _LL17:
# 1625
({struct Cyc_Absyn_Exp*_tmp87E=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp297="0.0F";_tag_dyneither(_tmp297,sizeof(char),5U);}),0),0U);res=_tmp87E;});goto _LL7;case 1U: _LL18: _LL19:
({struct Cyc_Absyn_Exp*_tmp87F=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp298="0.0";_tag_dyneither(_tmp298,sizeof(char),4U);}),1),0U);res=_tmp87F;});goto _LL7;default: _LL1A: _tmp2A2=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp296)->f1;_LL1B:
({struct Cyc_Absyn_Exp*_tmp881=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp880=({const char*_tmp299="0.0L";_tag_dyneither(_tmp299,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp880,_tmp2A2);}),0U);res=_tmp881;});goto _LL7;}default: _LL1C: _LL1D:
# 1629
({struct Cyc_String_pa_PrintArg_struct _tmp29C=({struct Cyc_String_pa_PrintArg_struct _tmp7A8;_tmp7A8.tag=0U,({struct _dyneither_ptr _tmp882=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp7A8.f1=_tmp882;});_tmp7A8;});void*_tmp29A[1U];_tmp29A[0]=& _tmp29C;({struct _dyneither_ptr _tmp883=({const char*_tmp29B="found non-zero type %s in generate_zero";_tag_dyneither(_tmp29B,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp883,_tag_dyneither(_tmp29A,sizeof(void*),1U));});});}_LL7:;}
# 1631
goto _LL0;default: _LL5: _LL6:
# 1633
({struct Cyc_String_pa_PrintArg_struct _tmp2A5=({struct Cyc_String_pa_PrintArg_struct _tmp7A9;_tmp7A9.tag=0U,({struct _dyneither_ptr _tmp884=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp7A9.f1=_tmp884;});_tmp7A9;});void*_tmp2A3[1U];_tmp2A3[0]=& _tmp2A5;({struct _dyneither_ptr _tmp885=({const char*_tmp2A4="found non-zero type %s in generate_zero";_tag_dyneither(_tmp2A4,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp885,_tag_dyneither(_tmp2A3,sizeof(void*),1U));});});}_LL0:;}
# 1635
res->topt=t;
return res;}
# 1642
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1655 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp2A7=Cyc_Toc_typ_to_c(elt_type);
void*_tmp2A8=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp2A9=Cyc_Absyn_cstar_type(_tmp2A7,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp2AA=({struct Cyc_Core_Opt*_tmp2DB=_cycalloc(sizeof(*_tmp2DB));_tmp2DB->v=_tmp2A9;_tmp2DB;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp2AB=e1->r;void*_tmp2AC=_tmp2AB;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B0;switch(*((int*)_tmp2AC)){case 20U: _LL1: _tmp2B0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2AC)->f1;_LL2:
# 1663
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp886=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2B0,0,Cyc_Absyn_Other_coercion,0U);_tmp2B0=_tmp886;});
_tmp2B0->topt=fat_ptr_type;}
# 1667
Cyc_Toc_exp_to_c(nv,_tmp2B0);xinit=_tmp2B0;goto _LL0;case 23U: _LL3: _tmp2B2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2AC)->f1;_tmp2B1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2AC)->f2;_LL4:
# 1669
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp887=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2B2,0,Cyc_Absyn_Other_coercion,0U);_tmp2B2=_tmp887;});
_tmp2B2->topt=fat_ptr_type;}
# 1673
Cyc_Toc_exp_to_c(nv,_tmp2B2);Cyc_Toc_exp_to_c(nv,_tmp2B1);
({struct Cyc_Absyn_Exp*_tmp88A=({struct Cyc_Absyn_Exp*_tmp889=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp889,({struct Cyc_Absyn_Exp*_tmp2AD[3U];_tmp2AD[0]=_tmp2B2,({
struct Cyc_Absyn_Exp*_tmp888=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2AD[1]=_tmp888;}),_tmp2AD[2]=_tmp2B1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2AD,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1674
xinit=_tmp88A;});
# 1676
goto _LL0;default: _LL5: _LL6:
({void*_tmp2AE=0U;({struct _dyneither_ptr _tmp88B=({const char*_tmp2AF="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp2AF,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp88B,_tag_dyneither(_tmp2AE,sizeof(void*),0U));});});}_LL0:;}{
# 1679
struct _tuple1*_tmp2B3=Cyc_Toc_temp_var();
struct _RegionHandle _tmp2B4=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp2B4;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp2B5=({struct Cyc_Absyn_Vardecl*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));_tmp2DA->sc=Cyc_Absyn_Public,_tmp2DA->name=_tmp2B3,_tmp2DA->varloc=0U,_tmp2DA->tq=Cyc_Toc_mt_tq,_tmp2DA->type=_tmp2A8,_tmp2DA->initializer=xinit,_tmp2DA->rgn=0,_tmp2DA->attributes=0,_tmp2DA->escapes=0;_tmp2DA;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B6=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));_tmp2D9->tag=4U,_tmp2D9->f1=_tmp2B5;_tmp2D9;});
struct Cyc_Absyn_Exp*_tmp2B7=Cyc_Absyn_varb_exp((void*)_tmp2B6,0U);
_tmp2B7->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp2B8=Cyc_Absyn_deref_exp(_tmp2B7,0U);
_tmp2B8->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp2B8);{
struct _tuple1*_tmp2B9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2BA=({struct Cyc_Absyn_Vardecl*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8->sc=Cyc_Absyn_Public,_tmp2D8->name=_tmp2B9,_tmp2D8->varloc=0U,_tmp2D8->tq=Cyc_Toc_mt_tq,_tmp2D8->type=_tmp2A7,_tmp2D8->initializer=_tmp2B8,_tmp2D8->rgn=0,_tmp2D8->attributes=0,_tmp2D8->escapes=0;_tmp2D8;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BB=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->tag=4U,_tmp2D7->f1=_tmp2BA;_tmp2D7;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Absyn_varb_exp((void*)_tmp2BB,0U);
_tmp2BC->topt=_tmp2B8->topt;
({struct Cyc_Absyn_Exp*_tmp88C=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2BC,e2,0U);z_init=_tmp88C;});
z_init->topt=_tmp2BC->topt;}
# 1698
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2BD=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2BE=({struct Cyc_Absyn_Vardecl*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->sc=Cyc_Absyn_Public,_tmp2D6->name=_tmp2BD,_tmp2D6->varloc=0U,_tmp2D6->tq=Cyc_Toc_mt_tq,_tmp2D6->type=_tmp2A7,_tmp2D6->initializer=z_init,_tmp2D6->rgn=0,_tmp2D6->attributes=0,_tmp2D6->escapes=0;_tmp2D6;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BF=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5->tag=4U,_tmp2D5->f1=_tmp2BE;_tmp2D5;});
# 1704
struct Cyc_Absyn_Exp*_tmp2C0=Cyc_Absyn_varb_exp((void*)_tmp2BB,0U);_tmp2C0->topt=_tmp2B8->topt;{
struct Cyc_Absyn_Exp*_tmp2C1=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2C2=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2C0,_tmp2C1,0U);
_tmp2C2->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2C2);{
# 1710
struct Cyc_Absyn_Exp*_tmp2C3=Cyc_Absyn_varb_exp((void*)_tmp2BF,0U);_tmp2C3->topt=_tmp2B8->topt;{
struct Cyc_Absyn_Exp*_tmp2C4=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2C5=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2C3,_tmp2C4,0U);
_tmp2C5->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2C5);{
# 1716
struct Cyc_List_List*_tmp2C6=({struct Cyc_Absyn_Exp*_tmp2D4[2U];({struct Cyc_Absyn_Exp*_tmp88E=Cyc_Absyn_varb_exp((void*)_tmp2B6,0U);_tmp2D4[0]=_tmp88E;}),({
struct Cyc_Absyn_Exp*_tmp88D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2D4[1]=_tmp88D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2D4,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2C7=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp890=({struct Cyc_Absyn_Exp*_tmp88F=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2C6,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp88F,_tmp2C7,0U);});xsize=_tmp890;});{
# 1723
struct Cyc_Absyn_Exp*_tmp2C8=({struct Cyc_Absyn_Exp*_tmp891=xsize;Cyc_Absyn_and_exp(_tmp891,Cyc_Absyn_and_exp(_tmp2C2,_tmp2C5,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C9=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2CA=({struct Cyc_Absyn_Exp*_tmp892=Cyc_Absyn_varb_exp((void*)_tmp2B6,0U);Cyc_Toc_member_exp(_tmp892,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp893=Cyc_Toc_cast_it(_tmp2A9,_tmp2CA);_tmp2CA=_tmp893;});{
struct Cyc_Absyn_Exp*_tmp2CB=Cyc_Absyn_deref_exp(_tmp2CA,0U);
struct Cyc_Absyn_Exp*_tmp2CC=({struct Cyc_Absyn_Exp*_tmp894=_tmp2CB;Cyc_Absyn_assign_exp(_tmp894,Cyc_Absyn_var_exp(_tmp2BD,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2CD=Cyc_Absyn_exp_stmt(_tmp2CC,0U);
({struct Cyc_Absyn_Stmt*_tmp898=({struct Cyc_Absyn_Stmt*_tmp897=({struct Cyc_Absyn_Exp*_tmp896=_tmp2C8;struct Cyc_Absyn_Stmt*_tmp895=_tmp2C9;Cyc_Absyn_ifthenelse_stmt(_tmp896,_tmp895,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp897,_tmp2CD,0U);});_tmp2CD=_tmp898;});
({struct Cyc_Absyn_Stmt*_tmp89B=({struct Cyc_Absyn_Decl*_tmp89A=({struct Cyc_Absyn_Decl*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));({void*_tmp899=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->tag=0U,_tmp2CE->f1=_tmp2BE;_tmp2CE;});_tmp2CF->r=_tmp899;}),_tmp2CF->loc=0U;_tmp2CF;});Cyc_Absyn_decl_stmt(_tmp89A,_tmp2CD,0U);});_tmp2CD=_tmp89B;});
({struct Cyc_Absyn_Stmt*_tmp89E=({struct Cyc_Absyn_Decl*_tmp89D=({struct Cyc_Absyn_Decl*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));({void*_tmp89C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->tag=0U,_tmp2D0->f1=_tmp2BA;_tmp2D0;});_tmp2D1->r=_tmp89C;}),_tmp2D1->loc=0U;_tmp2D1;});Cyc_Absyn_decl_stmt(_tmp89D,_tmp2CD,0U);});_tmp2CD=_tmp89E;});
({struct Cyc_Absyn_Stmt*_tmp8A1=({struct Cyc_Absyn_Decl*_tmp8A0=({struct Cyc_Absyn_Decl*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));({void*_tmp89F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->tag=0U,_tmp2D2->f1=_tmp2B5;_tmp2D2;});_tmp2D3->r=_tmp89F;}),_tmp2D3->loc=0U;_tmp2D3;});Cyc_Absyn_decl_stmt(_tmp8A0,_tmp2CD,0U);});_tmp2CD=_tmp8A1;});
({void*_tmp8A2=Cyc_Toc_stmt_exp_r(_tmp2CD);e->r=_tmp8A2;});};};};};};};};};};}
# 1681
;_pop_region(rgn2);};}
# 1749 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1753
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2DC=Cyc_Tcutil_compress(aggrtype);void*_tmp2DD=_tmp2DC;union Cyc_Absyn_AggrInfo _tmp2E2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DD)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DD)->f1)->tag == 20U){_LL1: _tmp2E2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DD)->f1)->f1;_LL2:
# 1756
({struct Cyc_Absyn_Aggrdecl*_tmp8A3=Cyc_Absyn_get_known_aggrdecl(_tmp2E2);ad=_tmp8A3;});goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp2DE=0U;({struct _dyneither_ptr _tmp8A6=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E1=({struct Cyc_String_pa_PrintArg_struct _tmp7AA;_tmp7AA.tag=0U,({
struct _dyneither_ptr _tmp8A4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp7AA.f1=_tmp8A4;});_tmp7AA;});void*_tmp2DF[1U];_tmp2DF[0]=& _tmp2E1;({struct _dyneither_ptr _tmp8A5=({const char*_tmp2E0="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2E0,sizeof(char),51U);});Cyc_aprintf(_tmp8A5,_tag_dyneither(_tmp2DF,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp8A6,_tag_dyneither(_tmp2DE,sizeof(void*),0U));});});}_LL0:;}{
# 1760
struct _tuple1*_tmp2E3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2E4=Cyc_Absyn_var_exp(_tmp2E3,0U);
struct Cyc_Absyn_Exp*_tmp2E5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2E6=Cyc_Absyn_aggrarrow_exp(_tmp2E4,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2E7=Cyc_Absyn_neq_exp(_tmp2E6,_tmp2E5,0U);
struct Cyc_Absyn_Exp*_tmp2E8=Cyc_Absyn_aggrarrow_exp(_tmp2E4,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2E9=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2E8,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2EA=({struct Cyc_Absyn_Exp*_tmp8A8=_tmp2E7;struct Cyc_Absyn_Stmt*_tmp8A7=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8A8,_tmp8A7,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2EB=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2EC=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2ED=({struct _tuple1*_tmp8AB=_tmp2E3;void*_tmp8AA=_tmp2EB;struct Cyc_Absyn_Exp*_tmp8A9=_tmp2EC;Cyc_Absyn_declare_stmt(_tmp8AB,_tmp8AA,_tmp8A9,Cyc_Absyn_seq_stmt(_tmp2EA,_tmp2E9,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2ED);}else{
# 1774
struct Cyc_Absyn_Exp*_tmp2EE=({struct Cyc_Absyn_Exp*_tmp8AC=aggrproj(_tmp2E4,f,0U);Cyc_Toc_member_exp(_tmp8AC,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2EF=Cyc_Absyn_neq_exp(_tmp2EE,_tmp2E5,0U);
struct Cyc_Absyn_Exp*_tmp2F0=({struct Cyc_Absyn_Exp*_tmp8AD=aggrproj(_tmp2E4,f,0U);Cyc_Toc_member_exp(_tmp8AD,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2F1=Cyc_Absyn_exp_stmt(_tmp2F0,0U);
struct Cyc_Absyn_Stmt*_tmp2F2=({struct Cyc_Absyn_Exp*_tmp8AF=_tmp2EF;struct Cyc_Absyn_Stmt*_tmp8AE=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8AF,_tmp8AE,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2F3=({struct _tuple1*_tmp8B2=_tmp2E3;void*_tmp8B1=e1_c_type;struct Cyc_Absyn_Exp*_tmp8B0=e1;Cyc_Absyn_declare_stmt(_tmp8B2,_tmp8B1,_tmp8B0,Cyc_Absyn_seq_stmt(_tmp2F2,_tmp2F1,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2F3);}};}
# 1784
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1787
void*_tmp2F4=Cyc_Tcutil_compress(t);void*_tmp2F5=_tmp2F4;union Cyc_Absyn_AggrInfo _tmp2FC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F5)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F5)->f1)->tag == 20U){_LL1: _tmp2FC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F5)->f1)->f1;_LL2: {
# 1789
struct Cyc_Absyn_Aggrdecl*_tmp2F6=Cyc_Absyn_get_known_aggrdecl(_tmp2FC);
({int _tmp8B3=Cyc_Toc_get_member_offset(_tmp2F6,f);*f_tag=_tmp8B3;});{
# 1792
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FA=({struct Cyc_String_pa_PrintArg_struct _tmp7AC;_tmp7AC.tag=0U,_tmp7AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2F6->name).f2);_tmp7AC;});struct Cyc_String_pa_PrintArg_struct _tmp2FB=({struct Cyc_String_pa_PrintArg_struct _tmp7AB;_tmp7AB.tag=0U,_tmp7AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp7AB;});void*_tmp2F8[2U];_tmp2F8[0]=& _tmp2FA,_tmp2F8[1]=& _tmp2FB;({struct _dyneither_ptr _tmp8B4=({const char*_tmp2F9="_union_%s_%s";_tag_dyneither(_tmp2F9,sizeof(char),13U);});Cyc_aprintf(_tmp8B4,_tag_dyneither(_tmp2F8,sizeof(void*),2U));});});
({void*_tmp8B5=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));*_tmp2F7=str;_tmp2F7;}));*tagged_member_type=_tmp8B5;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2F6->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1803
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1806
void*_tmp2FD=e->r;void*_tmp2FE=_tmp2FD;struct Cyc_Absyn_Exp*_tmp30A;struct _dyneither_ptr*_tmp309;int*_tmp308;struct Cyc_Absyn_Exp*_tmp307;struct _dyneither_ptr*_tmp306;int*_tmp305;struct Cyc_Absyn_Exp*_tmp304;switch(*((int*)_tmp2FE)){case 14U: _LL1: _tmp304=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FE)->f2;_LL2:
({void*_tmp2FF=0U;({struct _dyneither_ptr _tmp8B6=({const char*_tmp300="cast on lhs!";_tag_dyneither(_tmp300,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8B6,_tag_dyneither(_tmp2FF,sizeof(void*),0U));});});case 21U: _LL3: _tmp307=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FE)->f1;_tmp306=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FE)->f2;_tmp305=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FE)->f4;_LL4:
# 1809
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp307->topt),_tmp306,f_tag,tagged_member_type,clear_read,_tmp305);case 22U: _LL5: _tmp30A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FE)->f1;_tmp309=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FE)->f2;_tmp308=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FE)->f4;_LL6: {
# 1812
void*_tmp301=Cyc_Tcutil_compress((void*)_check_null(_tmp30A->topt));void*_tmp302=_tmp301;void*_tmp303;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->tag == 3U){_LLA: _tmp303=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).elt_type;_LLB:
# 1814
 return Cyc_Toc_is_tagged_union_project_impl(_tmp303,_tmp309,f_tag,tagged_member_type,clear_read,_tmp308);}else{_LLC: _LLD:
# 1816
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1818
 return 0;}_LL0:;}
# 1831 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1835
struct _tuple1*_tmp30B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp30B,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp8B7=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp8B7;});else{
# 1845
struct Cyc_Absyn_Exp*_tmp30C=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp8BA=({struct Cyc_Absyn_Exp*_tmp8B9=_tmp30C;struct Cyc_Absyn_Stmt*_tmp8B8=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8B9,_tmp8B8,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp8BA;});}{
# 1848
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8BD=_tmp30B;void*_tmp8BC=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8BB=
Cyc_Toc_push_address_exp(e1);
# 1848
Cyc_Absyn_declare_stmt(_tmp8BD,_tmp8BC,_tmp8BB,
# 1850
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple32{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple35{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1854
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp30D=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp310=({struct Cyc_String_pa_PrintArg_struct _tmp7AD;_tmp7AD.tag=0U,({struct _dyneither_ptr _tmp8BE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp7AD.f1=_tmp8BE;});_tmp7AD;});void*_tmp30E[1U];_tmp30E[0]=& _tmp310;({struct _dyneither_ptr _tmp8BF=({const char*_tmp30F="exp_to_c: no type for %s";_tag_dyneither(_tmp30F,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8BF,_tag_dyneither(_tmp30E,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp311=_tmp30D;struct Cyc_Absyn_Stmt*_tmp532;struct Cyc_Absyn_Exp*_tmp531;struct _dyneither_ptr*_tmp530;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*_tmp52E;int _tmp52D;struct Cyc_Absyn_Exp*_tmp52C;void**_tmp52B;struct Cyc_Absyn_Exp*_tmp52A;int _tmp529;int _tmp528;struct Cyc_List_List*_tmp527;struct Cyc_Absyn_Datatypedecl*_tmp526;struct Cyc_Absyn_Datatypefield*_tmp525;struct _tuple1**_tmp524;struct Cyc_List_List*_tmp523;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_Aggrdecl*_tmp521;void*_tmp520;struct Cyc_List_List*_tmp51F;struct Cyc_Absyn_Exp*_tmp51E;void*_tmp51D;int _tmp51C;struct Cyc_Absyn_Vardecl*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;int _tmp518;struct Cyc_List_List*_tmp517;struct Cyc_List_List*_tmp516;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp513;struct _dyneither_ptr*_tmp512;int _tmp511;int _tmp510;struct Cyc_Absyn_Exp*_tmp50F;struct _dyneither_ptr*_tmp50E;int _tmp50D;int _tmp50C;struct Cyc_Absyn_Exp*_tmp50B;void*_tmp50A;struct Cyc_List_List*_tmp509;void*_tmp508;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*_tmp504;void**_tmp503;struct Cyc_Absyn_Exp*_tmp502;int _tmp501;enum Cyc_Absyn_Coercion _tmp500;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_List_List*_tmp4FE;struct Cyc_Absyn_Exp*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FC;int _tmp4FB;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_List_List*_tmp4F9;int _tmp4F8;struct Cyc_List_List*_tmp4F7;struct Cyc_Absyn_VarargInfo*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_List_List*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Core_Opt*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;enum Cyc_Absyn_Incrementor _tmp4E6;enum Cyc_Absyn_Primop _tmp4E5;struct Cyc_List_List*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;switch(*((int*)_tmp311)){case 2U: _LL1: _LL2:
# 1861
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp311)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1868
struct Cyc_Absyn_Exp*_tmp312=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8C0=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp312,_tmp312))->r;e->r=_tmp8C0;});else{
# 1873
({void*_tmp8C2=({struct Cyc_Absyn_Exp*_tmp8C1=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp8C1,({struct Cyc_Absyn_Exp*_tmp313[3U];_tmp313[0]=_tmp312,_tmp313[1]=_tmp312,_tmp313[2]=_tmp312;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp313,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp8C2;});}}else{
# 1875
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1877
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 41U: _LL9: _tmp4E3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_LLA:
 Cyc_Toc_exp_to_c(nv,_tmp4E3);goto _LL0;case 3U: _LLB: _tmp4E5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4E4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LLC: {
# 1883
struct Cyc_List_List*_tmp314=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp4E4);
# 1885
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4E4);
{enum Cyc_Absyn_Primop _tmp315=_tmp4E5;switch(_tmp315){case Cyc_Absyn_Numelts: _LL5A: _LL5B: {
# 1888
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
{void*_tmp316=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp317=_tmp316;void*_tmp328;void*_tmp327;void*_tmp326;void*_tmp325;struct Cyc_Absyn_Exp*_tmp324;switch(*((int*)_tmp317)){case 4U: _LL6F: _tmp324=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp317)->f1).num_elts;_LL70:
# 1892
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp324)))
({void*_tmp318=0U;({unsigned int _tmp8C4=e->loc;struct _dyneither_ptr _tmp8C3=({const char*_tmp319="can't calculate numelts";_tag_dyneither(_tmp319,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8C4,_tmp8C3,_tag_dyneither(_tmp318,sizeof(void*),0U));});});
e->r=_tmp324->r;goto _LL6E;case 3U: _LL71: _tmp328=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp317)->f1).elt_type;_tmp327=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp317)->f1).ptr_atts).nullable;_tmp326=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp317)->f1).ptr_atts).bounds;_tmp325=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp317)->f1).ptr_atts).zero_term;_LL72: {
# 1896
struct Cyc_Absyn_Exp*_tmp31A=({void*_tmp8C5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8C5,_tmp326);});
if(_tmp31A == 0)
# 1899
({void*_tmp8C8=({struct Cyc_Absyn_Exp*_tmp8C7=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp8C7,({struct Cyc_Absyn_Exp*_tmp31B[2U];_tmp31B[0]=(struct Cyc_Absyn_Exp*)_tmp4E4->hd,({
# 1901
struct Cyc_Absyn_Exp*_tmp8C6=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp328),0U);_tmp31B[1]=_tmp8C6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp31B,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1899
e->r=_tmp8C8;});else{
# 1903
struct Cyc_Absyn_Exp*_tmp31C=_tmp31A;
# 1905
if(Cyc_Tcutil_force_type2bool(0,_tmp325)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4E4->hd);
# 1909
({void*_tmp8CA=({struct Cyc_Absyn_Exp*_tmp8C9=function_e;Cyc_Toc_fncall_exp_r(_tmp8C9,({struct Cyc_Absyn_Exp*_tmp31D[2U];_tmp31D[0]=(struct Cyc_Absyn_Exp*)_tmp4E4->hd,_tmp31D[1]=_tmp31C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp31D,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8CA;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp327)){
if(!Cyc_Evexp_c_can_eval(_tmp31C))
({void*_tmp31E=0U;({unsigned int _tmp8CC=e->loc;struct _dyneither_ptr _tmp8CB=({const char*_tmp31F="can't calculate numelts";_tag_dyneither(_tmp31F,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8CC,_tmp8CB,_tag_dyneither(_tmp31E,sizeof(void*),0U));});});
# 1914
({void*_tmp8CF=({struct Cyc_Absyn_Exp*_tmp8CE=arg;struct Cyc_Absyn_Exp*_tmp8CD=_tmp31C;Cyc_Toc_conditional_exp_r(_tmp8CE,_tmp8CD,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8CF;});}else{
# 1916
e->r=_tmp31C->r;goto _LL6E;}}}
# 1919
goto _LL6E;}default: _LL73: _LL74:
# 1921
({struct Cyc_String_pa_PrintArg_struct _tmp322=({struct Cyc_String_pa_PrintArg_struct _tmp7AF;_tmp7AF.tag=0U,({
struct _dyneither_ptr _tmp8D0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp7AF.f1=_tmp8D0;});_tmp7AF;});struct Cyc_String_pa_PrintArg_struct _tmp323=({struct Cyc_String_pa_PrintArg_struct _tmp7AE;_tmp7AE.tag=0U,({struct _dyneither_ptr _tmp8D1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp7AE.f1=_tmp8D1;});_tmp7AE;});void*_tmp320[2U];_tmp320[0]=& _tmp322,_tmp320[1]=& _tmp323;({struct _dyneither_ptr _tmp8D2=({const char*_tmp321="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp321,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D2,_tag_dyneither(_tmp320,sizeof(void*),2U));});});}_LL6E:;}
# 1924
goto _LL59;}case Cyc_Absyn_Plus: _LL5C: _LL5D:
# 1929
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp329=0U;({struct _dyneither_ptr _tmp8D3=({const char*_tmp32A="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp32A,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp8D3,_tag_dyneither(_tmp329,sizeof(void*),0U));});});
{void*_tmp32B=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp314))->hd);void*_tmp32C=_tmp32B;void*_tmp333;void*_tmp332;void*_tmp331;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32C)->tag == 3U){_LL76: _tmp333=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32C)->f1).elt_type;_tmp332=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32C)->f1).ptr_atts).bounds;_tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32C)->f1).ptr_atts).zero_term;_LL77: {
# 1933
struct Cyc_Absyn_Exp*_tmp32D=({void*_tmp8D4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8D4,_tmp332);});
if(_tmp32D == 0){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4->tl))->hd;
({void*_tmp8D7=({struct Cyc_Absyn_Exp*_tmp8D6=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8D6,({struct Cyc_Absyn_Exp*_tmp32E[3U];_tmp32E[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8D5=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp333),0U);_tmp32E[1]=_tmp8D5;}),_tmp32E[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32E,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1937
e->r=_tmp8D7;});}else{
# 1940
struct Cyc_Absyn_Exp*_tmp32F=_tmp32D;
if(Cyc_Tcutil_force_type2bool(0,_tmp331)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4->tl))->hd;
({void*_tmp8D9=({struct Cyc_Absyn_Exp*_tmp8D8=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp8D8,({struct Cyc_Absyn_Exp*_tmp330[3U];_tmp330[0]=e1,_tmp330[1]=_tmp32F,_tmp330[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp330,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp8D9;});}}
# 1947
goto _LL75;}}else{_LL78: _LL79:
 goto _LL75;}_LL75:;}
# 1950
goto _LL59;case Cyc_Absyn_Minus: _LL5E: _LL5F: {
# 1955
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp314))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp314->tl))->hd)){
({void*_tmp8DB=({struct Cyc_Absyn_Exp*_tmp8DA=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8DA,Cyc_Toc_curr_sp);});e1->r=_tmp8DB;});
({void*_tmp8DD=({struct Cyc_Absyn_Exp*_tmp8DC=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8DC,Cyc_Toc_curr_sp);});e2->r=_tmp8DD;});
({void*_tmp8DF=({void*_tmp8DE=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp8DE;});e1->topt=_tmp8DF;});
({void*_tmp8E1=({struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8E0,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1963
e->r=_tmp8E1;});}else{
# 1966
({void*_tmp8E5=({struct Cyc_Absyn_Exp*_tmp8E4=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8E4,({struct Cyc_Absyn_Exp*_tmp334[3U];_tmp334[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8E3=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp334[1]=_tmp8E3;}),({
struct Cyc_Absyn_Exp*_tmp8E2=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp334[2]=_tmp8E2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp334,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1966
e->r=_tmp8E5;});}}
# 1971
goto _LL59;}case Cyc_Absyn_Eq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Neq: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gt: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Gte: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lt: _LL68: _LL69:
 goto _LL6B;case Cyc_Absyn_Lte: _LL6A: _LL6B: {
# 1979
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp314))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp314->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& elt_type)){
void*_tmp335=({void*_tmp8E6=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp8E6,Cyc_Toc_mt_tq);});
({void*_tmp8E9=({void*_tmp8E8=_tmp335;Cyc_Toc_cast_it_r(_tmp8E8,({struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8E7,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8E9;});
e1->topt=_tmp335;}
# 1989
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
void*_tmp336=({void*_tmp8EA=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp8EA,Cyc_Toc_mt_tq);});
({void*_tmp8ED=({void*_tmp8EC=_tmp336;Cyc_Toc_cast_it_r(_tmp8EC,({struct Cyc_Absyn_Exp*_tmp8EB=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8EB,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8ED;});
e2->topt=_tmp336;}
# 1994
goto _LL59;}default: _LL6C: _LL6D:
 goto _LL59;}_LL59:;}
# 1997
goto _LL0;}case 5U: _LLD: _tmp4E7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4E6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LLE: {
# 1999
void*e2_cyc_typ=(void*)_check_null(_tmp4E7->topt);
# 2008 "toc.cyc"
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp350="increment";_tag_dyneither(_tmp350,sizeof(char),10U);});
if(_tmp4E6 == Cyc_Absyn_PreDec  || _tmp4E6 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp8EE=({const char*_tmp337="decrement";_tag_dyneither(_tmp337,sizeof(char),10U);});incr_str=_tmp8EE;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E7,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp33A=({struct Cyc_String_pa_PrintArg_struct _tmp7B0;_tmp7B0.tag=0U,_tmp7B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp7B0;});void*_tmp338[1U];_tmp338[0]=& _tmp33A;({unsigned int _tmp8F0=e->loc;struct _dyneither_ptr _tmp8EF=({const char*_tmp339="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp339,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp8F0,_tmp8EF,_tag_dyneither(_tmp338,sizeof(void*),1U));});});
({void*_tmp33B=0U;({struct _dyneither_ptr _tmp8F1=({const char*_tmp33C="in-place inc/dec on zero-term";_tag_dyneither(_tmp33C,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F1,_tag_dyneither(_tmp33B,sizeof(void*),0U));});});}{
# 2017
void*tunion_member_type=Cyc_Absyn_void_type;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4E7,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp33D=Cyc_Absyn_signed_int_exp(1,0U);
_tmp33D->topt=Cyc_Absyn_sint_type;{
enum Cyc_Absyn_Incrementor _tmp33E=_tmp4E6;switch(_tmp33E){case Cyc_Absyn_PreInc: _LL7B: _LL7C:
# 2024
({void*_tmp8F3=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340->tag=4U,_tmp340->f1=_tmp4E7,({struct Cyc_Core_Opt*_tmp8F2=({struct Cyc_Core_Opt*_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F->v=(void*)Cyc_Absyn_Plus;_tmp33F;});_tmp340->f2=_tmp8F2;}),_tmp340->f3=_tmp33D;_tmp340;});e->r=_tmp8F3;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL7D: _LL7E:
# 2028
({void*_tmp8F5=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342->tag=4U,_tmp342->f1=_tmp4E7,({struct Cyc_Core_Opt*_tmp8F4=({struct Cyc_Core_Opt*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341->v=(void*)Cyc_Absyn_Minus;_tmp341;});_tmp342->f2=_tmp8F4;}),_tmp342->f3=_tmp33D;_tmp342;});e->r=_tmp8F5;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL7F: _LL80:
# 2032
({struct Cyc_String_pa_PrintArg_struct _tmp345=({struct Cyc_String_pa_PrintArg_struct _tmp7B1;_tmp7B1.tag=0U,_tmp7B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp7B1;});void*_tmp343[1U];_tmp343[0]=& _tmp345;({unsigned int _tmp8F7=e->loc;struct _dyneither_ptr _tmp8F6=({const char*_tmp344="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp344,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp8F7,_tmp8F6,_tag_dyneither(_tmp343,sizeof(void*),1U));});});
# 2034
({void*_tmp346=0U;({struct _dyneither_ptr _tmp8F8=({const char*_tmp347="in-place inc/dec on @tagged union";_tag_dyneither(_tmp347,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F8,_tag_dyneither(_tmp346,sizeof(void*),0U));});});}_LL7A:;};}{
# 2037
int need_null_ck=(Cyc_Tcutil_is_pointer_type(old_typ) && 
Cyc_Tcutil_is_nullable_pointer_type(old_typ)) && Cyc_Toc_need_null_check(_tmp4E7);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E7);
Cyc_Toc_set_lhs(nv,0);{
# 2044
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4E6 == Cyc_Absyn_PostInc  || _tmp4E6 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2050
if(_tmp4E6 == Cyc_Absyn_PreDec  || _tmp4E6 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp8FD=({struct Cyc_Absyn_Exp*_tmp8FC=fn_e;Cyc_Toc_fncall_exp_r(_tmp8FC,({struct Cyc_Absyn_Exp*_tmp348[3U];({struct Cyc_Absyn_Exp*_tmp8FB=Cyc_Toc_push_address_exp(_tmp4E7);_tmp348[0]=_tmp8FB;}),({
struct Cyc_Absyn_Exp*_tmp8FA=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp348[1]=_tmp8FA;}),({
struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Absyn_signed_int_exp(change,0U);_tmp348[2]=_tmp8F9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp348,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2052
e->r=_tmp8FD;});}else{
# 2055
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2061
if(_tmp4E6 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp349=Cyc_Toc_temp_var();
void*_tmp34A=({void*_tmp8FE=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp8FE,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp34B=Cyc_Toc_push_address_exp(_tmp4E7);
struct Cyc_Absyn_Exp*_tmp34C=({struct Cyc_Absyn_Exp*_tmp8FF=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp349,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp8FF,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(need_null_ck)
({struct Cyc_Absyn_Exp*_tmp902=({struct Cyc_Absyn_Exp*_tmp901=({struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp349,0U),0U);Cyc_Absyn_neq_exp(_tmp900,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2068
Cyc_Absyn_and_exp(_tmp901,_tmp34C,0U);});_tmp34C=_tmp902;});{
# 2070
struct Cyc_Absyn_Stmt*_tmp34D=({struct Cyc_Absyn_Exp*_tmp904=_tmp34C;struct Cyc_Absyn_Stmt*_tmp903=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp349,0U),0U),0U),0U);
# 2070
Cyc_Absyn_ifthenelse_stmt(_tmp904,_tmp903,
# 2072
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp906=({struct Cyc_Absyn_Stmt*_tmp905=_tmp34D;Cyc_Absyn_seq_stmt(_tmp905,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp349,0U),0U),0U),0U);});_tmp34D=_tmp906;});
({void*_tmp907=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp349,_tmp34A,_tmp34B,_tmp34D,0U),0U))->r;e->r=_tmp907;});};}else{
# 2077
struct Cyc_Toc_functionSet*_tmp34E=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp34E,_tmp4E7);
({void*_tmp90B=({struct Cyc_Absyn_Exp*_tmp90A=fn_e;Cyc_Toc_fncall_exp_r(_tmp90A,({struct Cyc_Absyn_Exp*_tmp34F[2U];({struct Cyc_Absyn_Exp*_tmp909=Cyc_Toc_push_address_exp(_tmp4E7);_tmp34F[0]=_tmp909;}),({
struct Cyc_Absyn_Exp*_tmp908=Cyc_Absyn_signed_int_exp(1,0U);_tmp34F[1]=_tmp908;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34F,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2079
e->r=_tmp90B;});}}else{
# 2082
if(elt_typ == Cyc_Absyn_void_type  && !Cyc_Absyn_is_lvalue(_tmp4E7)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4E7,0,Cyc_Toc_incr_lvalue,_tmp4E6);
e->r=_tmp4E7->r;}}}
# 2086
goto _LL0;};};};}case 4U: _LLF: _tmp4EA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4E9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp4E8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp311)->f3;_LL10: {
# 2105 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4EA->topt);
void*e2_old_typ=(void*)_check_null(_tmp4E8->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp4EA,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4EA);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E8);
({void*_tmp90C=Cyc_Toc_tagged_union_assignop(_tmp4EA,e1_old_typ,_tmp4E9,_tmp4E8,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp90C;});
# 2116
return;}{
# 2118
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4EA,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4EA,_tmp4E9,_tmp4E8,ptr_type,is_dyneither,elt_type);
# 2124
return;}{
# 2128
int e1_poly=Cyc_Toc_is_poly_project(_tmp4EA);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4EA);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E8);{
# 2134
int done=0;
if(_tmp4E9 != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp351=(enum Cyc_Absyn_Primop)_tmp4E9->v;enum Cyc_Absyn_Primop _tmp352=_tmp351;switch(_tmp352){case Cyc_Absyn_Plus: _LL82: _LL83:
 change=_tmp4E8;goto _LL81;case Cyc_Absyn_Minus: _LL84: _LL85:
# 2142
({struct Cyc_Absyn_Exp*_tmp90D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4E8,0U);change=_tmp90D;});goto _LL81;default: _LL86: _LL87:
({void*_tmp353=0U;({struct _dyneither_ptr _tmp90E=({const char*_tmp354="bad t ? pointer arithmetic";_tag_dyneither(_tmp354,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp90E,_tag_dyneither(_tmp353,sizeof(void*),0U));});});}_LL81:;}
# 2145
done=1;{
# 2147
struct Cyc_Absyn_Exp*_tmp355=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp912=({struct Cyc_Absyn_Exp*_tmp911=_tmp355;Cyc_Toc_fncall_exp_r(_tmp911,({struct Cyc_Absyn_Exp*_tmp356[3U];({struct Cyc_Absyn_Exp*_tmp910=Cyc_Toc_push_address_exp(_tmp4EA);_tmp356[0]=_tmp910;}),({
struct Cyc_Absyn_Exp*_tmp90F=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp356[1]=_tmp90F;}),_tmp356[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp356,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2148
e->r=_tmp912;});};}else{
# 2151
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2154
enum Cyc_Absyn_Primop _tmp357=(enum Cyc_Absyn_Primop)_tmp4E9->v;enum Cyc_Absyn_Primop _tmp358=_tmp357;if(_tmp358 == Cyc_Absyn_Plus){_LL89: _LL8A:
# 2156
 done=1;
({void*_tmp914=({struct Cyc_Absyn_Exp*_tmp913=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4EA);Cyc_Toc_fncall_exp_r(_tmp913,({struct Cyc_Absyn_Exp*_tmp359[2U];_tmp359[0]=_tmp4EA,_tmp359[1]=_tmp4E8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp359,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp914;});
goto _LL88;}else{_LL8B: _LL8C:
({void*_tmp35A=0U;({struct _dyneither_ptr _tmp915=({const char*_tmp35B="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp35B,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp915,_tag_dyneither(_tmp35A,sizeof(void*),0U));});});}_LL88:;}}}
# 2163
if(!done){
# 2165
if(e1_poly)
({void*_tmp917=({void*_tmp916=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp916,Cyc_Absyn_new_exp(_tmp4E8->r,0U));});_tmp4E8->r=_tmp917;});
# 2171
if(!Cyc_Absyn_is_lvalue(_tmp4EA)){
({struct Cyc_Absyn_Exp*_tmp918=_tmp4EA;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp918,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp35D=1;struct _tuple31*_tmp35C=_cycalloc(_check_times(_tmp35D,sizeof(struct _tuple31)));(_tmp35C[0]).f1=_tmp4E9,(_tmp35C[0]).f2=_tmp4E8;_tmp35C;}));});
e->r=_tmp4EA->r;}}
# 2176
goto _LL0;};};};}case 6U: _LL11: _tmp4ED=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4EC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp4EB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp311)->f3;_LL12:
# 2178
 Cyc_Toc_exp_to_c(nv,_tmp4ED);
Cyc_Toc_exp_to_c(nv,_tmp4EC);
Cyc_Toc_exp_to_c(nv,_tmp4EB);
goto _LL0;case 7U: _LL13: _tmp4EF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4EE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL14:
 _tmp4F1=_tmp4EF;_tmp4F0=_tmp4EE;goto _LL16;case 8U: _LL15: _tmp4F1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4F0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL16:
 _tmp4F3=_tmp4F1;_tmp4F2=_tmp4F0;goto _LL18;case 9U: _LL17: _tmp4F3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4F2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL18:
# 2185
 Cyc_Toc_exp_to_c(nv,_tmp4F3);
Cyc_Toc_exp_to_c(nv,_tmp4F2);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp311)->f3 == 0){_LL19: _tmp4F5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4F4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL1A:
# 2189
 Cyc_Toc_exp_to_c(nv,_tmp4F5);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4F4);
goto _LL0;}else{_LL1B: _tmp4FA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4F9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp4F8=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp311)->f3)->num_varargs;_tmp4F7=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp311)->f3)->injectors;_tmp4F6=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp311)->f3)->vai;_LL1C: {
# 2201 "toc.cyc"
struct _RegionHandle _tmp35E=_new_region("r");struct _RegionHandle*r=& _tmp35E;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4F8,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4F6->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2209
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F9);
int num_normargs=num_args - _tmp4F8;
# 2213
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4F9=_tmp4F9->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4F9))->hd);
({struct Cyc_List_List*_tmp919=({struct Cyc_List_List*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F->hd=(struct Cyc_Absyn_Exp*)_tmp4F9->hd,_tmp35F->tl=new_args;_tmp35F;});new_args=_tmp919;});}}
# 2218
({struct Cyc_List_List*_tmp91D=({struct Cyc_List_List*_tmp361=_cycalloc(sizeof(*_tmp361));({struct Cyc_Absyn_Exp*_tmp91C=({struct Cyc_Absyn_Exp*_tmp91B=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp91B,({struct Cyc_Absyn_Exp*_tmp360[3U];_tmp360[0]=argvexp,({
# 2220
struct Cyc_Absyn_Exp*_tmp91A=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp360[1]=_tmp91A;}),_tmp360[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp360,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2218
_tmp361->hd=_tmp91C;}),_tmp361->tl=new_args;_tmp361;});new_args=_tmp91D;});
# 2223
({struct Cyc_List_List*_tmp91E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp91E;});
# 2225
Cyc_Toc_exp_to_c(nv,_tmp4FA);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4FA,new_args,0U),0U);
# 2229
if(_tmp4F6->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp362=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4F6->type));void*_tmp363=_tmp362;struct Cyc_Absyn_Datatypedecl*_tmp366;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp363)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp363)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp363)->f1)->f1).KnownDatatype).tag == 2){_LL8E: _tmp366=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp363)->f1)->f1).KnownDatatype).val;_LL8F:
 tud=_tmp366;goto _LL8D;}else{goto _LL90;}}else{goto _LL90;}}else{_LL90: _LL91:
({void*_tmp364=0U;({struct _dyneither_ptr _tmp91F=({const char*_tmp365="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp365,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp91F,_tag_dyneither(_tmp364,sizeof(void*),0U));});});}_LL8D:;}{
# 2235
struct _dyneither_ptr vs=({unsigned int _tmp377=(unsigned int)_tmp4F8;struct _tuple1**_tmp376=({struct _RegionHandle*_tmp920=r;_region_malloc(_tmp920,_check_times(_tmp377,sizeof(struct _tuple1*)));});({{unsigned int _tmp7B2=(unsigned int)_tmp4F8;unsigned int i;for(i=0;i < _tmp7B2;++ i){({struct _tuple1*_tmp921=Cyc_Toc_temp_var();_tmp376[i]=_tmp921;});}}0;});_tag_dyneither(_tmp376,sizeof(struct _tuple1*),_tmp377);});
# 2237
if(_tmp4F8 != 0){
# 2241
struct Cyc_List_List*_tmp367=0;
{int i=_tmp4F8 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp923=({struct Cyc_List_List*_tmp368=_cycalloc(sizeof(*_tmp368));({struct Cyc_Absyn_Exp*_tmp922=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp368->hd=_tmp922;}),_tmp368->tl=_tmp367;_tmp368;});_tmp367=_tmp923;});}}
({struct Cyc_Absyn_Stmt*_tmp927=({struct _tuple1*_tmp926=argv;void*_tmp925=arr_type;struct Cyc_Absyn_Exp*_tmp924=Cyc_Absyn_array_exp(_tmp367,0U);Cyc_Absyn_declare_stmt(_tmp926,_tmp925,_tmp924,s,0U);});s=_tmp927;});
# 2246
({struct Cyc_List_List*_tmp928=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4F9);_tmp4F9=_tmp928;});
({struct Cyc_List_List*_tmp929=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4F7);_tmp4F7=_tmp929;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F9)- 1;
for(0;_tmp4F9 != 0;(_tmp4F9=_tmp4F9->tl,_tmp4F7=_tmp4F7->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4F9->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp369=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4F7))->hd;struct Cyc_Absyn_Datatypefield*_tmp36A=_tmp369;struct _tuple1*_tmp370;struct Cyc_List_List*_tmp36F;_LL93: _tmp370=_tmp36A->name;_tmp36F=_tmp36A->typs;_LL94:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp36F))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp92A=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp92A;});{
# 2260
struct _tuple1*_tmp36B=Cyc_Toc_collapse_qvars(_tmp370,tud->name);
struct Cyc_List_List*_tmp36C=({struct _tuple20*_tmp36E[2U];({struct _tuple20*_tmp92E=({struct _dyneither_ptr*_tmp92D=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp92D,Cyc_Toc_datatype_tag(tud,_tmp370));});_tmp36E[0]=_tmp92E;}),({
struct _tuple20*_tmp92C=({struct _dyneither_ptr*_tmp92B=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp92B,arg);});_tmp36E[1]=_tmp92C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36E,sizeof(struct _tuple20*),2U));});
({struct Cyc_Absyn_Stmt*_tmp932=({struct _tuple1*_tmp931=var;void*_tmp930=Cyc_Absyn_strctq(_tmp36B);struct Cyc_Absyn_Exp*_tmp92F=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D->tag=29U,_tmp36D->f1=_tmp36B,_tmp36D->f2=0,_tmp36D->f3=_tmp36C,_tmp36D->f4=0;_tmp36D;}),0U);
# 2263
Cyc_Absyn_declare_stmt(_tmp931,_tmp930,_tmp92F,s,0U);});s=_tmp932;});};};}};}else{
# 2271
struct Cyc_List_List*_tmp371=({struct _tuple32*_tmp372[3U];({struct _tuple32*_tmp93B=({struct _tuple32*_tmp373=_cycalloc(sizeof(*_tmp373));({struct _dyneither_ptr _tmp93A=_tag_dyneither(0,0,0);_tmp373->f1=_tmp93A;}),({struct Cyc_Absyn_Exp*_tmp939=Cyc_Absyn_uint_exp(0U,0U);_tmp373->f2=_tmp939;});_tmp373;});_tmp372[0]=_tmp93B;}),({
struct _tuple32*_tmp938=({struct _tuple32*_tmp374=_cycalloc(sizeof(*_tmp374));({struct _dyneither_ptr _tmp937=_tag_dyneither(0,0,0);_tmp374->f1=_tmp937;}),({struct Cyc_Absyn_Exp*_tmp936=Cyc_Absyn_uint_exp(0U,0U);_tmp374->f2=_tmp936;});_tmp374;});_tmp372[1]=_tmp938;}),({
struct _tuple32*_tmp935=({struct _tuple32*_tmp375=_cycalloc(sizeof(*_tmp375));({struct _dyneither_ptr _tmp934=_tag_dyneither(0,0,0);_tmp375->f1=_tmp934;}),({struct Cyc_Absyn_Exp*_tmp933=Cyc_Absyn_uint_exp(0U,0U);_tmp375->f2=_tmp933;});_tmp375;});_tmp372[2]=_tmp935;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp372,sizeof(struct _tuple32*),3U));});
({struct Cyc_Absyn_Stmt*_tmp93F=({struct _tuple1*_tmp93E=argv;void*_tmp93D=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp93C=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp93E,_tmp93D,_tmp93C,s,0U);});s=_tmp93F;});}};}else{
# 2279
if(_tmp4F8 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp4F9 != 0;_tmp4F9=_tmp4F9->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F9->hd);
({struct Cyc_List_List*_tmp940=({struct Cyc_List_List*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->hd=(struct Cyc_Absyn_Exp*)_tmp4F9->hd,_tmp378->tl=array_args;_tmp378;});array_args=_tmp940;});}{
# 2285
struct Cyc_Absyn_Exp*_tmp379=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp941=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp379,s,0U);s=_tmp941;});};}else{
# 2288
({struct Cyc_Absyn_Stmt*_tmp945=({struct _tuple1*_tmp944=argv;void*_tmp943=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp942=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp944,_tmp943,_tmp942,s,0U);});s=_tmp945;});}}
# 2291
({void*_tmp946=Cyc_Toc_stmt_exp_r(s);e->r=_tmp946;});};}
# 2293
_npop_handler(0U);goto _LL0;
# 2201
;_pop_region(r);}}case 11U: _LL1D: _tmp4FC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4FB=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL1E:
# 2296
 Cyc_Toc_exp_to_c(nv,_tmp4FC);{
struct Cyc_Absyn_Exp*_tmp37A=_tmp4FB?Cyc_Toc_newrethrow_exp(_tmp4FC): Cyc_Toc_newthrow_exp(_tmp4FC);
({void*_tmp948=({void*_tmp947=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp947,_tmp37A,0U);})->r;e->r=_tmp948;});
goto _LL0;};case 12U: _LL1F: _tmp4FD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_LL20:
 Cyc_Toc_exp_to_c(nv,_tmp4FD);goto _LL0;case 13U: _LL21: _tmp4FF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp4FE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL22:
# 2302
 Cyc_Toc_exp_to_c(nv,_tmp4FF);
# 2311 "toc.cyc"
for(0;_tmp4FE != 0;_tmp4FE=_tmp4FE->tl){
enum Cyc_Absyn_KindQual _tmp37B=(Cyc_Tcutil_type_kind((void*)_tmp4FE->hd))->kind;
if(_tmp37B != Cyc_Absyn_EffKind  && _tmp37B != Cyc_Absyn_RgnKind){
{void*_tmp37C=Cyc_Tcutil_compress((void*)_tmp4FE->hd);void*_tmp37D=_tmp37C;switch(*((int*)_tmp37D)){case 2U: _LL96: _LL97:
 goto _LL99;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D)->f1)->tag == 18U){_LL98: _LL99:
 continue;}else{goto _LL9A;}default: _LL9A: _LL9B:
# 2318
({void*_tmp94A=({void*_tmp949=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp949,_tmp4FF,0U);})->r;e->r=_tmp94A;});
goto _LL95;}_LL95:;}
# 2321
break;}}
# 2324
goto _LL0;case 14U: _LL23: _tmp503=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp502=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp501=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp311)->f3;_tmp500=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp311)->f4;_LL24: {
# 2326
void*old_t2=(void*)_check_null(_tmp502->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp503;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp503=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp502);
# 2333
{struct _tuple33 _tmp37E=({struct _tuple33 _tmp7B6;({void*_tmp94C=Cyc_Tcutil_compress(old_t2);_tmp7B6.f1=_tmp94C;}),({void*_tmp94B=Cyc_Tcutil_compress(new_typ);_tmp7B6.f2=_tmp94B;});_tmp7B6;});struct _tuple33 _tmp37F=_tmp37E;struct Cyc_Absyn_PtrInfo _tmp3BD;struct Cyc_Absyn_PtrInfo _tmp3BC;struct Cyc_Absyn_PtrInfo _tmp3BB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37F.f1)->tag == 3U)switch(*((int*)_tmp37F.f2)){case 3U: _LL9D: _tmp3BC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37F.f1)->f1;_tmp3BB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37F.f2)->f1;_LL9E: {
# 2335
int _tmp380=Cyc_Tcutil_force_type2bool(0,(_tmp3BC.ptr_atts).nullable);
int _tmp381=Cyc_Tcutil_force_type2bool(0,(_tmp3BB.ptr_atts).nullable);
struct Cyc_Absyn_Exp*_tmp382=({void*_tmp94D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp94D,(_tmp3BC.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp383=({void*_tmp94E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp94E,(_tmp3BB.ptr_atts).bounds);});
int _tmp384=Cyc_Tcutil_force_type2bool(0,(_tmp3BC.ptr_atts).zero_term);
int _tmp385=Cyc_Tcutil_force_type2bool(0,(_tmp3BB.ptr_atts).zero_term);
{struct _tuple34 _tmp386=({struct _tuple34 _tmp7B5;_tmp7B5.f1=_tmp382,_tmp7B5.f2=_tmp383;_tmp7B5;});struct _tuple34 _tmp387=_tmp386;if(_tmp387.f1 != 0){if(_tmp387.f2 != 0){_LLA4: _LLA5: {
# 2343
struct Cyc_Absyn_Exp*_tmp388=(struct Cyc_Absyn_Exp*)_check_null(_tmp382);
struct Cyc_Absyn_Exp*_tmp389=(struct Cyc_Absyn_Exp*)_check_null(_tmp383);
if((!Cyc_Evexp_c_can_eval(_tmp388) || !Cyc_Evexp_c_can_eval(_tmp389)) && !
Cyc_Evexp_same_const_exp(_tmp388,_tmp389))
({void*_tmp38A=0U;({unsigned int _tmp950=e->loc;struct _dyneither_ptr _tmp94F=({const char*_tmp38B="can't validate cast due to potential size differences";_tag_dyneither(_tmp38B,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp950,_tmp94F,_tag_dyneither(_tmp38A,sizeof(void*),0U));});});
if(_tmp380  && !_tmp381){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp38C=0U;({struct _dyneither_ptr _tmp951=({const char*_tmp38D="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp38D,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp951,_tag_dyneither(_tmp38C,sizeof(void*),0U));});});
# 2354
if(_tmp500 != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp390=({struct Cyc_String_pa_PrintArg_struct _tmp7B3;_tmp7B3.tag=0U,({struct _dyneither_ptr _tmp952=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp7B3.f1=_tmp952;});_tmp7B3;});void*_tmp38E[1U];_tmp38E[0]=& _tmp390;({struct _dyneither_ptr _tmp953=({const char*_tmp38F="null-check conversion mis-classified: %s";_tag_dyneither(_tmp38F,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp953,_tag_dyneither(_tmp38E,sizeof(void*),1U));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp502);
if(do_null_check){
if(!_tmp501)
({void*_tmp391=0U;({unsigned int _tmp955=e->loc;struct _dyneither_ptr _tmp954=({const char*_tmp392="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp392,sizeof(char),58U);});Cyc_Tcutil_warn(_tmp955,_tmp954,_tag_dyneither(_tmp391,sizeof(void*),0U));});});
# 2361
({void*_tmp958=({void*_tmp957=*_tmp503;Cyc_Toc_cast_it_r(_tmp957,({struct Cyc_Absyn_Exp*_tmp956=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp956,({struct Cyc_List_List*_tmp393=_cycalloc(sizeof(*_tmp393));
_tmp393->hd=_tmp502,_tmp393->tl=0;_tmp393;}),0U);}));});
# 2361
e->r=_tmp958;});}else{
# 2365
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp502->r;}};}else{
# 2370
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp502->r;}
# 2377
goto _LLA3;}}else{_LLA6: _LLA7: {
# 2379
struct Cyc_Absyn_Exp*_tmp394=(struct Cyc_Absyn_Exp*)_check_null(_tmp382);
if(!Cyc_Evexp_c_can_eval(_tmp394))
({void*_tmp395=0U;({unsigned int _tmp95A=e->loc;struct _dyneither_ptr _tmp959=({const char*_tmp396="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp396,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp95A,_tmp959,_tag_dyneither(_tmp395,sizeof(void*),0U));});});{
struct _tuple13 _tmp397=Cyc_Evexp_eval_const_uint_exp(_tmp394);struct _tuple13 _tmp398=_tmp397;unsigned int _tmp3AA;int _tmp3A9;_LLAD: _tmp3AA=_tmp398.f1;_tmp3A9=_tmp398.f2;_LLAE:;
# 2384
if(_tmp500 == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp39B=({struct Cyc_String_pa_PrintArg_struct _tmp7B4;_tmp7B4.tag=0U,({struct _dyneither_ptr _tmp95B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp7B4.f1=_tmp95B;});_tmp7B4;});void*_tmp399[1U];_tmp399[0]=& _tmp39B;({struct _dyneither_ptr _tmp95C=({const char*_tmp39A="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp39A,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp95C,_tag_dyneither(_tmp399,sizeof(void*),1U));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2388
if((_tmp384  && !(_tmp3BB.elt_tq).real_const) && !_tmp385)
# 2391
({struct Cyc_Absyn_Exp*_tmp95E=({struct Cyc_Absyn_Exp*_tmp95D=_tmp394;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp95D,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp394=_tmp95E;});
# 2393
({void*_tmp95F=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp394,_tmp502))->r;e->r=_tmp95F;});}else{
# 2395
struct Cyc_Absyn_Exp*_tmp39C=Cyc_Toc__tag_dyneither_e;
# 2397
if(_tmp384){
# 2402
struct _tuple1*_tmp39D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_var_exp(_tmp39D,0U);
struct Cyc_Absyn_Exp*arg3;
# 2407
{void*_tmp39F=_tmp502->r;void*_tmp3A0=_tmp39F;struct Cyc_Absyn_Vardecl*_tmp3A3;struct Cyc_Absyn_Vardecl*_tmp3A2;switch(*((int*)_tmp3A0)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3A0)->f1).Wstring_c).tag){case 8U: _LLB0: _LLB1:
# 2409
 arg3=_tmp394;goto _LLAF;case 9U: _LLB2: _LLB3:
# 2411
 arg3=_tmp394;goto _LLAF;default: goto _LLB8;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A0)->f1)){case 1U: _LLB4: _tmp3A2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A0)->f1)->f1;_LLB5:
 _tmp3A3=_tmp3A2;goto _LLB7;case 4U: _LLB6: _tmp3A3=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A0)->f1)->f1;_LLB7:
# 2414
 if(!Cyc_Tcutil_is_array_type(_tmp3A3->type))goto _LLB9;
arg3=_tmp394;
goto _LLAF;default: goto _LLB8;}default: _LLB8: _LLB9:
# 2420
 if(_tmp3A9  && _tmp3AA != 1)
arg3=_tmp394;else{
# 2423
({struct Cyc_Absyn_Exp*_tmp963=({struct Cyc_Absyn_Exp*_tmp962=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp502);Cyc_Absyn_fncall_exp(_tmp962,({struct Cyc_Absyn_Exp*_tmp3A1[2U];({
# 2425
struct Cyc_Absyn_Exp*_tmp961=({void*_tmp960=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp960,_tmp39E);});_tmp3A1[0]=_tmp961;}),_tmp3A1[1]=_tmp394;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A1,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2423
arg3=_tmp963;});}
# 2426
goto _LLAF;}_LLAF:;}
# 2428
if(!_tmp385  && !(_tmp3BB.elt_tq).real_const)
# 2431
({struct Cyc_Absyn_Exp*_tmp965=({struct Cyc_Absyn_Exp*_tmp964=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp964,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp965;});{
# 2433
struct Cyc_Absyn_Exp*_tmp3A4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp3BB.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp3A5=({struct Cyc_Absyn_Exp*_tmp966=_tmp39C;Cyc_Absyn_fncall_exp(_tmp966,({struct Cyc_Absyn_Exp*_tmp3A7[3U];_tmp3A7[0]=_tmp39E,_tmp3A7[1]=_tmp3A4,_tmp3A7[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A7,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp3A6=Cyc_Absyn_exp_stmt(_tmp3A5,0U);
({struct Cyc_Absyn_Stmt*_tmp96A=({struct _tuple1*_tmp969=_tmp39D;void*_tmp968=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp967=_tmp502;Cyc_Absyn_declare_stmt(_tmp969,_tmp968,_tmp967,_tmp3A6,0U);});_tmp3A6=_tmp96A;});
({void*_tmp96B=Cyc_Toc_stmt_exp_r(_tmp3A6);e->r=_tmp96B;});};}else{
# 2440
({void*_tmp96E=({struct Cyc_Absyn_Exp*_tmp96D=_tmp39C;Cyc_Toc_fncall_exp_r(_tmp96D,({struct Cyc_Absyn_Exp*_tmp3A8[3U];_tmp3A8[0]=_tmp502,({
# 2442
struct Cyc_Absyn_Exp*_tmp96C=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp3BB.elt_type),0U);_tmp3A8[1]=_tmp96C;}),_tmp3A8[2]=_tmp394;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A8,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2440
e->r=_tmp96E;});}}
# 2446
goto _LLA3;};}}}else{if(_tmp387.f2 != 0){_LLA8: _LLA9: {
# 2448
struct Cyc_Absyn_Exp*_tmp3AB=(struct Cyc_Absyn_Exp*)_check_null(_tmp383);
if(!Cyc_Evexp_c_can_eval(_tmp3AB))
({void*_tmp3AC=0U;({unsigned int _tmp970=e->loc;struct _dyneither_ptr _tmp96F=({const char*_tmp3AD="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp3AD,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp970,_tmp96F,_tag_dyneither(_tmp3AC,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3AE=0U;({struct _dyneither_ptr _tmp971=({const char*_tmp3AF="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp3AF,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp971,_tag_dyneither(_tmp3AE,sizeof(void*),0U));});});{
# 2461 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3B0=_tmp3AB;
if(_tmp384  && !_tmp385)
({struct Cyc_Absyn_Exp*_tmp973=({struct Cyc_Absyn_Exp*_tmp972=_tmp3AB;Cyc_Absyn_add_exp(_tmp972,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3B0=_tmp973;});{
# 2468
struct Cyc_Absyn_Exp*_tmp3B1=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp3B2=({struct Cyc_Absyn_Exp*_tmp975=_tmp3B1;Cyc_Absyn_fncall_exp(_tmp975,({struct Cyc_Absyn_Exp*_tmp3B4[3U];_tmp3B4[0]=_tmp502,({
# 2471
struct Cyc_Absyn_Exp*_tmp974=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp3BC.elt_type),0U);_tmp3B4[1]=_tmp974;}),_tmp3B4[2]=_tmp3B0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B4,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2473
if(!_tmp381)
({void*_tmp978=({struct Cyc_Absyn_Exp*_tmp977=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp977,({struct Cyc_List_List*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));
({struct Cyc_Absyn_Exp*_tmp976=Cyc_Absyn_copy_exp(_tmp3B2);_tmp3B3->hd=_tmp976;}),_tmp3B3->tl=0;_tmp3B3;}));});
# 2474
_tmp3B2->r=_tmp978;});
# 2476
({void*_tmp979=Cyc_Toc_cast_it_r(*_tmp503,_tmp3B2);e->r=_tmp979;});
goto _LLA3;};};}}else{_LLAA: _LLAB:
# 2481
 DynCast:
 if((_tmp384  && !_tmp385) && !(_tmp3BB.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3B5=0U;({struct _dyneither_ptr _tmp97A=({const char*_tmp3B6="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp3B6,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp97A,_tag_dyneither(_tmp3B5,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp3B7=Cyc_Toc__dyneither_ptr_decrease_size_e;
({void*_tmp97E=({struct Cyc_Absyn_Exp*_tmp97D=_tmp3B7;Cyc_Toc_fncall_exp_r(_tmp97D,({struct Cyc_Absyn_Exp*_tmp3B8[3U];_tmp3B8[0]=_tmp502,({
struct Cyc_Absyn_Exp*_tmp97C=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp3BC.elt_type),0U);_tmp3B8[1]=_tmp97C;}),({
struct Cyc_Absyn_Exp*_tmp97B=Cyc_Absyn_uint_exp(1U,0U);_tmp3B8[2]=_tmp97B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B8,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2486
e->r=_tmp97E;});};}
# 2490
goto _LLA3;}}_LLA3:;}
# 2492
goto _LL9C;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37F.f2)->f1)->tag == 1U){_LL9F: _tmp3BD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37F.f1)->f1;_LLA0:
# 2494
{struct Cyc_Absyn_Exp*_tmp3B9=({void*_tmp97F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp97F,(_tmp3BD.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp3BA=_tmp3B9;if(_tmp3BA == 0){_LLBB: _LLBC:
# 2496
({void*_tmp981=({struct Cyc_Absyn_Exp*_tmp980=Cyc_Absyn_new_exp(_tmp502->r,_tmp502->loc);Cyc_Toc_aggrmember_exp_r(_tmp980,Cyc_Toc_curr_sp);});_tmp502->r=_tmp981;});
_tmp502->topt=new_typ_c;
goto _LLBA;}else{_LLBD: _LLBE:
 goto _LLBA;}_LLBA:;}
# 2501
goto _LL9C;}else{goto _LLA1;}default: goto _LLA1;}else{_LLA1: _LLA2:
# 2503
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp502->r;
goto _LL9C;}_LL9C:;}
# 2507
goto _LL0;}case 15U: _LL25: _tmp504=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_LL26:
# 2510
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp504);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp504)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp504,0,Cyc_Toc_address_lvalue,1);
# 2516
({void*_tmp983=({void*_tmp982=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp982,_tmp504);});e->r=_tmp983;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp504->topt))))
# 2520
({void*_tmp985=({void*_tmp984=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp984,_tmp504);});e->r=_tmp985;});}
# 2522
goto _LL0;case 16U: _LL27: _tmp506=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp505=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL28:
# 2529
 Cyc_Toc_exp_to_c(nv,_tmp505);{
# 2531
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp505->topt));
struct _tuple1*_tmp3BE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3BF=Cyc_Absyn_var_exp(_tmp3BE,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3C0=Cyc_Tcutil_compress(elt_typ);void*_tmp3C1=_tmp3C0;void*_tmp3C2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C1)->tag == 4U){_LLC0: _tmp3C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C1)->f1).elt_type;_LLC1:
# 2537
({void*_tmp986=Cyc_Toc_typ_to_c(_tmp3C2);elt_typ=_tmp986;});
({struct Cyc_Absyn_Exp*_tmp987=Cyc_Absyn_copy_exp(_tmp3BF);lhs=_tmp987;});
goto _LLBF;}else{_LLC2: _LLC3:
# 2541
({struct Cyc_Absyn_Exp*_tmp988=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp3BF),0U);lhs=_tmp988;});
goto _LLBF;}_LLBF:;}{
# 2544
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp505);
struct _tuple1*_tmp3C3=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp3C4=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp3C3),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp98B=({struct Cyc_Absyn_Exp*_tmp98A=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp98A,({struct Cyc_Absyn_Exp*_tmp3C5[2U];_tmp3C5[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp3C4),({
struct Cyc_Absyn_Exp*_tmp989=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3C5[1]=_tmp989;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2549
mexp=_tmp98B;});else{
# 2552
({struct Cyc_Absyn_Exp*_tmp98C=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp3BF,0U),0U);mexp=_tmp98C;});}{
# 2555
struct Cyc_Absyn_Exp*_tmp3C6=Cyc_Toc_get_varsizeexp(_tmp505);
if(_tmp3C6 != 0)
({struct Cyc_Absyn_Exp*_tmp98D=Cyc_Absyn_add_exp(mexp,_tmp3C6,0U);mexp=_tmp98D;});
# 2559
if(_tmp506 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp98E=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp98E;});else{
# 2562
struct Cyc_Absyn_Exp*r=_tmp506;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp98F=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp98F;});}{
# 2566
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp994=({struct Cyc_Absyn_Exp*_tmp993=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp993,({struct Cyc_Absyn_Exp*_tmp3C7[3U];({
struct Cyc_Absyn_Exp*_tmp992=Cyc_Absyn_copy_exp(_tmp3BF);_tmp3C7[0]=_tmp992;}),({
struct Cyc_Absyn_Exp*_tmp991=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3C7[1]=_tmp991;}),
(unsigned int)_tmp3C4?_tmp3C7[2]=_tmp3C4:({struct Cyc_Absyn_Exp*_tmp990=Cyc_Absyn_uint_exp(1U,0U);_tmp3C7[2]=_tmp990;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3C7,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2568
answer=_tmp994;});else{
# 2573
({struct Cyc_Absyn_Exp*_tmp995=Cyc_Absyn_copy_exp(_tmp3BF);answer=_tmp995;});}
({void*_tmp996=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->tag=Cyc_Toc_Dest,_tmp3C8->f1=_tmp3BF;_tmp3C8;});e->annot=_tmp996;});{
struct Cyc_Absyn_Stmt*_tmp3C9=({struct _tuple1*_tmp99A=_tmp3BE;void*_tmp999=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp998=mexp;Cyc_Absyn_declare_stmt(_tmp99A,_tmp999,_tmp998,({
struct Cyc_Absyn_Stmt*_tmp997=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp997,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2579
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp99B=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp3C3),Cyc_Absyn_uint_type,array_len,_tmp3C9,0U);_tmp3C9=_tmp99B;});
({void*_tmp99C=Cyc_Toc_stmt_exp_r(_tmp3C9);e->r=_tmp99C;});
if(_tmp3C6 != 0)
({void*_tmp99E=({void*_tmp99D=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp99D,Cyc_Absyn_copy_exp(e));});e->r=_tmp99E;});
goto _LL0;};};};};};case 18U: _LL29: _tmp507=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_LL2A: {
# 2588
int _tmp3CA=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp507);
Cyc_Toc_set_in_sizeof(nv,_tmp3CA);
goto _LL0;}case 17U: _LL2B: _tmp508=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_LL2C:
({void*_tmp9A0=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->tag=17U,({void*_tmp99F=Cyc_Toc_typ_to_c(_tmp508);_tmp3CB->f1=_tmp99F;});_tmp3CB;});e->r=_tmp9A0;});goto _LL0;case 19U: _LL2D: _tmp50A=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp509=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL2E: {
# 2594
void*_tmp3CC=_tmp50A;
struct Cyc_List_List*_tmp3CD=_tmp509;
for(0;_tmp3CD != 0;_tmp3CD=_tmp3CD->tl){
void*_tmp3CE=(void*)_tmp3CD->hd;void*_tmp3CF=_tmp3CE;unsigned int _tmp3E1;struct _dyneither_ptr*_tmp3E0;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3CF)->tag == 0U){_LLC5: _tmp3E0=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3CF)->f1;_LLC6:
 goto _LLC4;}else{_LLC7: _tmp3E1=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3CF)->f1;_LLC8:
# 2600
{void*_tmp3D0=Cyc_Tcutil_compress(_tmp3CC);void*_tmp3D1=_tmp3D0;struct Cyc_List_List*_tmp3DF;struct Cyc_List_List*_tmp3DE;struct Cyc_Absyn_Datatypefield*_tmp3DD;union Cyc_Absyn_AggrInfo _tmp3DC;switch(*((int*)_tmp3D1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->f1)){case 20U: _LLCA: _tmp3DC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->f1)->f1;_LLCB: {
# 2602
struct Cyc_Absyn_Aggrdecl*_tmp3D2=Cyc_Absyn_get_known_aggrdecl(_tmp3DC);
if(_tmp3D2->impl == 0)
({void*_tmp3D3=0U;({struct _dyneither_ptr _tmp9A1=({const char*_tmp3D4="struct fields must be known";_tag_dyneither(_tmp3D4,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9A1,_tag_dyneither(_tmp3D3,sizeof(void*),0U));});});
_tmp3DE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D2->impl))->fields;goto _LLCD;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->f1)->f1).KnownDatatypefield).tag == 2){_LLD0: _tmp3DD=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->f1)->f1).KnownDatatypefield).val).f2;_LLD1:
# 2616
 if(_tmp3E1 == 0)
({void*_tmp9A2=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->tag=0U,_tmp3D8->f1=Cyc_Toc_tag_sp;_tmp3D8;}));_tmp3CD->hd=_tmp9A2;});else{
# 2619
({void*_tmp9A3=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DD->typs,(int)(_tmp3E1 - 1))).f2;_tmp3CC=_tmp9A3;});
({void*_tmp9A5=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->tag=0U,({struct _dyneither_ptr*_tmp9A4=Cyc_Absyn_fieldname((int)_tmp3E1);_tmp3D9->f1=_tmp9A4;});_tmp3D9;}));_tmp3CD->hd=_tmp9A5;});}
# 2622
goto _LLC9;}else{goto _LLD2;}default: goto _LLD2;}case 7U: _LLCC: _tmp3DE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D1)->f2;_LLCD: {
# 2607
struct Cyc_Absyn_Aggrfield*_tmp3D5=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DE,(int)_tmp3E1);
({void*_tmp9A6=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->tag=0U,_tmp3D6->f1=_tmp3D5->name;_tmp3D6;}));_tmp3CD->hd=_tmp9A6;});
_tmp3CC=_tmp3D5->type;
goto _LLC9;}case 6U: _LLCE: _tmp3DF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3D1)->f1;_LLCF:
# 2612
({void*_tmp9A8=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->tag=0U,({struct _dyneither_ptr*_tmp9A7=Cyc_Absyn_fieldname((int)(_tmp3E1 + 1));_tmp3D7->f1=_tmp9A7;});_tmp3D7;}));_tmp3CD->hd=_tmp9A8;});
({void*_tmp9A9=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DF,(int)_tmp3E1)).f2;_tmp3CC=_tmp9A9;});
goto _LLC9;default: _LLD2: _LLD3:
# 2623
({void*_tmp3DA=0U;({struct _dyneither_ptr _tmp9AA=({const char*_tmp3DB="impossible type for offsetof tuple index";_tag_dyneither(_tmp3DB,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9AA,_tag_dyneither(_tmp3DA,sizeof(void*),0U));});});
goto _LLC9;}_LLC9:;}
# 2626
goto _LLC4;}_LLC4:;}
# 2629
({void*_tmp9AC=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->tag=19U,({void*_tmp9AB=Cyc_Toc_typ_to_c(_tmp50A);_tmp3E2->f1=_tmp9AB;}),_tmp3E2->f2=_tmp509;_tmp3E2;});e->r=_tmp9AC;});
goto _LL0;}case 20U: _LL2F: _tmp50B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_LL30: {
# 2632
int _tmp3E3=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp3E4=Cyc_Tcutil_compress((void*)_check_null(_tmp50B->topt));
{void*_tmp3E5=_tmp3E4;void*_tmp3FB;struct Cyc_Absyn_Tqual _tmp3FA;void*_tmp3F9;void*_tmp3F8;void*_tmp3F7;void*_tmp3F6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E5)->tag == 3U){_LLD5: _tmp3FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E5)->f1).elt_type;_tmp3FA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E5)->f1).elt_tq;_tmp3F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E5)->f1).ptr_atts).rgn;_tmp3F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E5)->f1).ptr_atts).nullable;_tmp3F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E5)->f1).ptr_atts).bounds;_tmp3F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E5)->f1).ptr_atts).zero_term;_LLD6:
# 2637
{struct Cyc_Absyn_Exp*_tmp3E6=({void*_tmp9AD=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp9AD,_tmp3F7);});struct Cyc_Absyn_Exp*_tmp3E7=_tmp3E6;struct Cyc_Absyn_Exp*_tmp3F3;if(_tmp3E7 != 0){_LLDA: _tmp3F3=_tmp3E7;_LLDB: {
# 2639
struct Cyc_Absyn_Exp*_tmp3E8=(struct Cyc_Absyn_Exp*)_check_null(_tmp3F3);
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp50B);
Cyc_Toc_exp_to_c(nv,_tmp50B);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3E9=0U;({unsigned int _tmp9AF=e->loc;struct _dyneither_ptr _tmp9AE=({const char*_tmp3EA="inserted null check due to dereference";_tag_dyneither(_tmp3EA,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9AF,_tmp9AE,_tag_dyneither(_tmp3E9,sizeof(void*),0U));});});
# 2646
({void*_tmp9B3=({void*_tmp9B2=Cyc_Toc_typ_to_c(_tmp3E4);Cyc_Toc_cast_it_r(_tmp9B2,({
struct Cyc_Absyn_Exp*_tmp9B1=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9B1,({struct Cyc_List_List*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));
({struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Absyn_copy_exp(_tmp50B);_tmp3EB->hd=_tmp9B0;}),_tmp3EB->tl=0;_tmp3EB;}),0U);}));});
# 2646
_tmp50B->r=_tmp9B3;});}
# 2654
if(!Cyc_Toc_in_sizeof(nv) && !Cyc_Tcutil_force_type2bool(0,_tmp3F6)){
struct _tuple13 _tmp3EC=Cyc_Evexp_eval_const_uint_exp(_tmp3E8);struct _tuple13 _tmp3ED=_tmp3EC;unsigned int _tmp3F1;int _tmp3F0;_LLDF: _tmp3F1=_tmp3ED.f1;_tmp3F0=_tmp3ED.f2;_LLE0:;
# 2660
if(!_tmp3F0  || _tmp3F1 <= 0)
({void*_tmp3EE=0U;({unsigned int _tmp9B5=e->loc;struct _dyneither_ptr _tmp9B4=({const char*_tmp3EF="cannot determine dereference is in bounds";_tag_dyneither(_tmp3EF,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp9B5,_tmp9B4,_tag_dyneither(_tmp3EE,sizeof(void*),0U));});});}
# 2663
goto _LLD9;}}else{_LLDC: _LLDD: {
# 2666
struct Cyc_Absyn_Exp*_tmp3F2=Cyc_Absyn_uint_exp(0U,0U);
_tmp3F2->topt=Cyc_Absyn_uint_type;
({void*_tmp9B6=Cyc_Toc_subscript_exp_r(_tmp50B,_tmp3F2);e->r=_tmp9B6;});
Cyc_Toc_exp_to_c(nv,e);
goto _LLD9;}}_LLD9:;}
# 2672
goto _LLD4;}else{_LLD7: _LLD8:
({void*_tmp3F4=0U;({struct _dyneither_ptr _tmp9B7=({const char*_tmp3F5="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp3F5,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9B7,_tag_dyneither(_tmp3F4,sizeof(void*),0U));});});}_LLD4:;}
# 2675
Cyc_Toc_set_lhs(nv,_tmp3E3);
goto _LL0;};}case 21U: _LL31: _tmp50F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp50E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp50D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp311)->f3;_tmp50C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp311)->f4;_LL32: {
# 2678
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp50F->topt);
Cyc_Toc_exp_to_c(nv,_tmp50F);
if(_tmp50D  && _tmp50C)
({void*_tmp9BC=({struct Cyc_Absyn_Exp*_tmp9BB=_tmp50F;void*_tmp9BA=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp9B9=e1_cyc_type;struct _dyneither_ptr*_tmp9B8=_tmp50E;Cyc_Toc_check_tagged_union(_tmp9BB,_tmp9BA,_tmp9B9,_tmp9B8,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2682
e->r=_tmp9BC;});
# 2686
if(is_poly)
({void*_tmp9BE=({void*_tmp9BD=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9BD,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9BE;});
goto _LL0;}case 22U: _LL33: _tmp513=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp512=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp511=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp311)->f3;_tmp510=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp311)->f4;_LL34: {
# 2690
int _tmp3FC=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp513->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp513);
Cyc_Toc_exp_to_c(nv,_tmp513);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp3FD=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp3FE=_tmp3FD;void*_tmp415;struct Cyc_Absyn_Tqual _tmp414;void*_tmp413;void*_tmp412;void*_tmp411;void*_tmp410;_LLE2: _tmp415=_tmp3FE.elt_type;_tmp414=_tmp3FE.elt_tq;_tmp413=(_tmp3FE.ptr_atts).rgn;_tmp412=(_tmp3FE.ptr_atts).nullable;_tmp411=(_tmp3FE.ptr_atts).bounds;_tmp410=(_tmp3FE.ptr_atts).zero_term;_LLE3:;
{struct Cyc_Absyn_Exp*_tmp3FF=({void*_tmp9BF=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp9BF,_tmp411);});struct Cyc_Absyn_Exp*_tmp400=_tmp3FF;struct Cyc_Absyn_Exp*_tmp40F;if(_tmp400 != 0){_LLE5: _tmp40F=_tmp400;_LLE6: {
# 2701
struct Cyc_Absyn_Exp*_tmp401=(struct Cyc_Absyn_Exp*)_check_null(_tmp40F);
struct _tuple13 _tmp402=Cyc_Evexp_eval_const_uint_exp(_tmp401);struct _tuple13 _tmp403=_tmp402;unsigned int _tmp40D;int _tmp40C;_LLEA: _tmp40D=_tmp403.f1;_tmp40C=_tmp403.f2;_LLEB:;
if(_tmp40C){
if(_tmp40D < 1)
({void*_tmp404=0U;({struct _dyneither_ptr _tmp9C0=({const char*_tmp405="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp405,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp9C0,_tag_dyneither(_tmp404,sizeof(void*),0U));});});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp406=0U;({unsigned int _tmp9C2=e->loc;struct _dyneither_ptr _tmp9C1=({const char*_tmp407="inserted null check due to dereference";_tag_dyneither(_tmp407,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9C2,_tmp9C1,_tag_dyneither(_tmp406,sizeof(void*),0U));});});
# 2710
({void*_tmp9C6=({void*_tmp9C5=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9C5,({
struct Cyc_Absyn_Exp*_tmp9C4=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9C4,({struct Cyc_Absyn_Exp*_tmp408[1U];({struct Cyc_Absyn_Exp*_tmp9C3=
Cyc_Absyn_new_exp(_tmp513->r,0U);_tmp408[0]=_tmp9C3;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp408,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2710
_tmp513->r=_tmp9C6;});}}else{
# 2715
if(!Cyc_Evexp_c_can_eval(_tmp401))
({void*_tmp409=0U;({unsigned int _tmp9C8=e->loc;struct _dyneither_ptr _tmp9C7=({const char*_tmp40A="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp40A,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp9C8,_tmp9C7,_tag_dyneither(_tmp409,sizeof(void*),0U));});});
# 2719
({void*_tmp9CE=({void*_tmp9CD=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9CD,({
struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9CC,({struct Cyc_Absyn_Exp*_tmp40B[4U];({
struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Absyn_new_exp(_tmp513->r,0U);_tmp40B[0]=_tmp9CB;}),_tmp40B[1]=_tmp401,({
struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Absyn_sizeoftype_exp(_tmp415,0U);_tmp40B[2]=_tmp9CA;}),({
struct Cyc_Absyn_Exp*_tmp9C9=Cyc_Absyn_uint_exp(0U,0U);_tmp40B[3]=_tmp9C9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp40B,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 2719
_tmp513->r=_tmp9CE;});}
# 2725
goto _LLE4;}}else{_LLE7: _LLE8: {
# 2728
void*ta1=Cyc_Toc_typ_to_c(_tmp415);
({void*_tmp9D4=({void*_tmp9D3=Cyc_Absyn_cstar_type(ta1,_tmp414);Cyc_Toc_cast_it_r(_tmp9D3,({
struct Cyc_Absyn_Exp*_tmp9D2=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp9D2,({struct Cyc_Absyn_Exp*_tmp40E[3U];({
struct Cyc_Absyn_Exp*_tmp9D1=Cyc_Absyn_new_exp(_tmp513->r,0U);_tmp40E[0]=_tmp9D1;}),({
struct Cyc_Absyn_Exp*_tmp9D0=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp40E[1]=_tmp9D0;}),({
struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Absyn_uint_exp(0U,0U);_tmp40E[2]=_tmp9CF;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp40E,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 2729
_tmp513->r=_tmp9D4;});
# 2736
goto _LLE4;}}_LLE4:;}
# 2738
if(_tmp511  && _tmp510)
({void*_tmp9D9=({struct Cyc_Absyn_Exp*_tmp9D8=_tmp513;void*_tmp9D7=Cyc_Toc_typ_to_c(e1typ);void*_tmp9D6=_tmp415;struct _dyneither_ptr*_tmp9D5=_tmp512;Cyc_Toc_check_tagged_union(_tmp9D8,_tmp9D7,_tmp9D6,_tmp9D5,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9D9;});
# 2741
if(is_poly  && _tmp510)
({void*_tmp9DB=({void*_tmp9DA=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9DA,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9DB;});
Cyc_Toc_set_lhs(nv,_tmp3FC);
goto _LL0;};};}case 23U: _LL35: _tmp515=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp514=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL36: {
# 2746
int _tmp416=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp417=Cyc_Tcutil_compress((void*)_check_null(_tmp515->topt));
# 2750
{void*_tmp418=_tmp417;void*_tmp447;struct Cyc_Absyn_Tqual _tmp446;void*_tmp445;void*_tmp444;void*_tmp443;void*_tmp442;struct Cyc_List_List*_tmp441;switch(*((int*)_tmp418)){case 6U: _LLED: _tmp441=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp418)->f1;_LLEE:
# 2753
 Cyc_Toc_exp_to_c(nv,_tmp515);
Cyc_Toc_exp_to_c(nv,_tmp514);{
struct _tuple13 _tmp419=Cyc_Evexp_eval_const_uint_exp(_tmp514);struct _tuple13 _tmp41A=_tmp419;unsigned int _tmp41E;int _tmp41D;_LLF4: _tmp41E=_tmp41A.f1;_tmp41D=_tmp41A.f2;_LLF5:;
if(!_tmp41D)
({void*_tmp41B=0U;({struct _dyneither_ptr _tmp9DC=({const char*_tmp41C="unknown tuple subscript in translation to C";_tag_dyneither(_tmp41C,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp9DC,_tag_dyneither(_tmp41B,sizeof(void*),0U));});});
({void*_tmp9DE=({struct Cyc_Absyn_Exp*_tmp9DD=_tmp515;Cyc_Toc_aggrmember_exp_r(_tmp9DD,Cyc_Absyn_fieldname((int)(_tmp41E + 1)));});e->r=_tmp9DE;});
goto _LLEC;};case 3U: _LLEF: _tmp447=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp418)->f1).elt_type;_tmp446=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp418)->f1).elt_tq;_tmp445=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp418)->f1).ptr_atts).rgn;_tmp444=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp418)->f1).ptr_atts).nullable;_tmp443=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp418)->f1).ptr_atts).bounds;_tmp442=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp418)->f1).ptr_atts).zero_term;_LLF0: {
# 2761
struct Cyc_List_List*_tmp41F=Cyc_Toc_get_relns(_tmp515);
int _tmp420=Cyc_Toc_need_null_check(_tmp515);
int _tmp421=Cyc_Toc_in_sizeof(nv);
# 2769
int in_bnds=_tmp421;
{struct Cyc_Absyn_Exp*_tmp422=({void*_tmp9DF=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp9DF,_tmp443);});struct Cyc_Absyn_Exp*_tmp423=_tmp422;_LLF7: _LLF8:
# 2772
({int _tmp9E0=in_bnds  || Cyc_Toc_check_bounds(_tmp417,_tmp41F,_tmp515,_tmp514);in_bnds=_tmp9E0;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp426=({struct Cyc_String_pa_PrintArg_struct _tmp7B7;_tmp7B7.tag=0U,({struct _dyneither_ptr _tmp9E1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp7B7.f1=_tmp9E1;});_tmp7B7;});void*_tmp424[1U];_tmp424[0]=& _tmp426;({unsigned int _tmp9E3=e->loc;struct _dyneither_ptr _tmp9E2=({const char*_tmp425="bounds check necessary for %s";_tag_dyneither(_tmp425,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp9E3,_tmp9E2,_tag_dyneither(_tmp424,sizeof(void*),1U));});});_LLF6:;}
# 2779
Cyc_Toc_exp_to_c(nv,_tmp515);
Cyc_Toc_exp_to_c(nv,_tmp514);
++ Cyc_Toc_total_bounds_checks;
{struct Cyc_Absyn_Exp*_tmp427=({void*_tmp9E4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp9E4,_tmp443);});struct Cyc_Absyn_Exp*_tmp428=_tmp427;struct Cyc_Absyn_Exp*_tmp43E;if(_tmp428 != 0){_LLFA: _tmp43E=_tmp428;_LLFB: {
# 2784
struct Cyc_Absyn_Exp*_tmp429=(struct Cyc_Absyn_Exp*)_check_null(_tmp43E);
int possibly_null=Cyc_Tcutil_force_type2bool(0,_tmp444) && _tmp420;
void*ta1=Cyc_Toc_typ_to_c(_tmp447);
void*ta2=Cyc_Absyn_cstar_type(ta1,_tmp446);
struct _tuple13 _tmp42A=Cyc_Evexp_eval_const_uint_exp(_tmp429);struct _tuple13 _tmp42B=_tmp42A;unsigned int _tmp43B;int _tmp43A;_LLFF: _tmp43B=_tmp42B.f1;_tmp43A=_tmp42B.f2;_LL100:;
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp42C=0U;({unsigned int _tmp9E6=e->loc;struct _dyneither_ptr _tmp9E5=({const char*_tmp42D="inserted null check due to dereference";_tag_dyneither(_tmp42D,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9E6,_tmp9E5,_tag_dyneither(_tmp42C,sizeof(void*),0U));});});
({void*_tmp9EA=({void*_tmp9E9=ta2;Cyc_Toc_cast_it_r(_tmp9E9,({struct Cyc_Absyn_Exp*_tmp9E8=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9E8,({struct Cyc_Absyn_Exp*_tmp42E[1U];({struct Cyc_Absyn_Exp*_tmp9E7=
Cyc_Absyn_copy_exp(_tmp515);_tmp42E[0]=_tmp9E7;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42E,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2795
_tmp515->r=_tmp9EA;});}else{
# 2799
if((_tmp43A  && _tmp43B == 1) && Cyc_Tcutil_force_type2bool(0,_tmp442)){
# 2801
if(!Cyc_Evexp_c_can_eval(_tmp429))
({void*_tmp42F=0U;({unsigned int _tmp9EC=e->loc;struct _dyneither_ptr _tmp9EB=({const char*_tmp430="cannot determine subscript is in bounds";_tag_dyneither(_tmp430,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9EC,_tmp9EB,_tag_dyneither(_tmp42F,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp515);
({void*_tmp9EF=Cyc_Toc_deref_exp_r(({void*_tmp9EE=ta2;Cyc_Toc_cast_it(_tmp9EE,({
struct Cyc_Absyn_Exp*_tmp9ED=function_e;Cyc_Absyn_fncall_exp(_tmp9ED,({struct Cyc_Absyn_Exp*_tmp431[3U];_tmp431[0]=_tmp515,_tmp431[1]=_tmp429,_tmp431[2]=_tmp514;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp431,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2804
e->r=_tmp9EF;});};}else{
# 2807
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp429))
({void*_tmp432=0U;({unsigned int _tmp9F1=e->loc;struct _dyneither_ptr _tmp9F0=({const char*_tmp433="cannot determine subscript is in bounds";_tag_dyneither(_tmp433,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9F1,_tmp9F0,_tag_dyneither(_tmp432,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp434=0U;({unsigned int _tmp9F3=e->loc;struct _dyneither_ptr _tmp9F2=({const char*_tmp435="inserted null check due to dereference";_tag_dyneither(_tmp435,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9F3,_tmp9F2,_tag_dyneither(_tmp434,sizeof(void*),0U));});});
# 2813
({void*_tmp9F7=Cyc_Toc_deref_exp_r(({void*_tmp9F6=ta2;Cyc_Toc_cast_it(_tmp9F6,({
struct Cyc_Absyn_Exp*_tmp9F5=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9F5,({struct Cyc_Absyn_Exp*_tmp436[4U];_tmp436[0]=_tmp515,_tmp436[1]=_tmp429,({
# 2816
struct Cyc_Absyn_Exp*_tmp9F4=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp436[2]=_tmp9F4;}),_tmp436[3]=_tmp514;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp436,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 2813
e->r=_tmp9F7;});}else{
# 2819
if(!Cyc_Evexp_c_can_eval(_tmp429))
({void*_tmp437=0U;({unsigned int _tmp9F9=e->loc;struct _dyneither_ptr _tmp9F8=({const char*_tmp438="cannot determine subscript is in bounds";_tag_dyneither(_tmp438,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9F9,_tmp9F8,_tag_dyneither(_tmp437,sizeof(void*),0U));});});
# 2822
({void*_tmp9FC=({struct Cyc_Absyn_Exp*_tmp9FB=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmp9FB,({struct Cyc_Absyn_Exp*_tmp439[2U];_tmp439[0]=_tmp429,({
struct Cyc_Absyn_Exp*_tmp9FA=Cyc_Absyn_copy_exp(_tmp514);_tmp439[1]=_tmp9FA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp439,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2822
_tmp514->r=_tmp9FC;});}}}}
# 2825
goto _LLF9;}}else{_LLFC: _LLFD: {
# 2827
void*ta1=Cyc_Toc_typ_to_c(_tmp447);
if(in_bnds){
# 2831
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmp9FF=({struct Cyc_Absyn_Exp*_tmp9FE=({void*_tmp9FD=Cyc_Absyn_cstar_type(ta1,_tmp446);Cyc_Toc_cast_it(_tmp9FD,
Cyc_Toc_member_exp(_tmp515,Cyc_Toc_curr_sp,0U));});
# 2832
Cyc_Toc_subscript_exp_r(_tmp9FE,_tmp514);});e->r=_tmp9FF;});}else{
# 2836
struct Cyc_Absyn_Exp*_tmp43C=Cyc_Toc__check_dyneither_subscript_e;
({void*_tmpA03=Cyc_Toc_deref_exp_r(({void*_tmpA02=Cyc_Absyn_cstar_type(ta1,_tmp446);Cyc_Toc_cast_it(_tmpA02,({
struct Cyc_Absyn_Exp*_tmpA01=_tmp43C;Cyc_Absyn_fncall_exp(_tmpA01,({struct Cyc_Absyn_Exp*_tmp43D[3U];_tmp43D[0]=_tmp515,({
struct Cyc_Absyn_Exp*_tmpA00=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp43D[1]=_tmpA00;}),_tmp43D[2]=_tmp514;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43D,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2837
e->r=_tmpA03;});}
# 2842
goto _LLF9;}}_LLF9:;}
# 2844
goto _LLEC;}default: _LLF1: _LLF2:
({void*_tmp43F=0U;({struct _dyneither_ptr _tmpA04=({const char*_tmp440="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp440,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA04,_tag_dyneither(_tmp43F,sizeof(void*),0U));});});}_LLEC:;}
# 2847
Cyc_Toc_set_lhs(nv,_tmp416);
goto _LL0;};}case 24U: _LL37: _tmp516=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_LL38:
# 2850
 if(!Cyc_Toc_is_toplevel(nv)){
# 2852
void*_tmp448=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp516));
{void*_tmp449=_tmp448;union Cyc_Absyn_AggrInfo _tmp44E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp449)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp449)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp449)->f2 == 0){_LL102: _tmp44E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp449)->f1)->f1;_LL103: {
# 2855
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp44E);
{int i=1;for(0;_tmp516 != 0;(_tmp516=_tmp516->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp516->hd);
({struct Cyc_List_List*_tmpA07=({struct Cyc_List_List*_tmp44A=_cycalloc(sizeof(*_tmp44A));({struct _tuple20*_tmpA06=({struct _dyneither_ptr*_tmpA05=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmpA05,(struct Cyc_Absyn_Exp*)_tmp516->hd);});_tmp44A->hd=_tmpA06;}),_tmp44A->tl=dles;_tmp44A;});dles=_tmpA07;});}}
# 2861
({void*_tmpA09=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->tag=29U,_tmp44B->f1=sd->name,_tmp44B->f2=0,({struct Cyc_List_List*_tmpA08=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp44B->f3=_tmpA08;}),_tmp44B->f4=sd;_tmp44B;});e->r=_tmpA09;});
e->topt=_tmp448;
goto _LL101;}}else{goto _LL104;}}else{goto _LL104;}}else{_LL104: _LL105:
({void*_tmp44C=0U;({struct _dyneither_ptr _tmpA0A=({const char*_tmp44D="tuple type not an aggregate";_tag_dyneither(_tmp44D,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA0A,_tag_dyneither(_tmp44C,sizeof(void*),0U));});});}_LL101:;}
# 2866
goto _LL0;}else{
# 2870
struct Cyc_List_List*dles=0;
for(0;_tmp516 != 0;_tmp516=_tmp516->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp516->hd);
({struct Cyc_List_List*_tmpA0C=({struct Cyc_List_List*_tmp450=_cycalloc(sizeof(*_tmp450));({struct _tuple20*_tmpA0B=({struct _tuple20*_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->f1=0,_tmp44F->f2=(struct Cyc_Absyn_Exp*)_tmp516->hd;_tmp44F;});_tmp450->hd=_tmpA0B;}),_tmp450->tl=dles;_tmp450;});dles=_tmpA0C;});}
# 2875
({void*_tmpA0D=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmpA0D;});}
# 2877
goto _LL0;case 26U: _LL39: _tmp517=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_LL3A:
# 2883
{struct Cyc_List_List*_tmp451=_tmp517;for(0;_tmp451 != 0;_tmp451=_tmp451->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp451->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmpA0E=Cyc_Toc_unresolvedmem_exp_r(0,_tmp517);e->r=_tmpA0E;});
goto _LL0;case 27U: _LL3B: _tmp51B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp51A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp519=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp311)->f3;_tmp518=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp311)->f4;_LL3C:
# 2891
 Cyc_Toc_exp_to_c(nv,_tmp51A);
Cyc_Toc_exp_to_c(nv,_tmp519);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp452=Cyc_Evexp_eval_const_uint_exp(_tmp51A);struct _tuple13 _tmp453=_tmp452;unsigned int _tmp45D;int _tmp45C;_LL107: _tmp45D=_tmp453.f1;_tmp45C=_tmp453.f2;_LL108:;{
void*_tmp454=Cyc_Toc_typ_to_c((void*)_check_null(_tmp519->topt));
struct Cyc_List_List*es=0;
# 2898
if(!Cyc_Toc_is_zero(_tmp519)){
if(!_tmp45C)
({void*_tmp455=0U;({unsigned int _tmpA10=_tmp51A->loc;struct _dyneither_ptr _tmpA0F=({const char*_tmp456="cannot determine value of constant";_tag_dyneither(_tmp456,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpA10,_tmpA0F,_tag_dyneither(_tmp455,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp45D;++ i){
({struct Cyc_List_List*_tmpA12=({struct Cyc_List_List*_tmp458=_cycalloc(sizeof(*_tmp458));({struct _tuple20*_tmpA11=({struct _tuple20*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->f1=0,_tmp457->f2=_tmp519;_tmp457;});_tmp458->hd=_tmpA11;}),_tmp458->tl=es;_tmp458;});es=_tmpA12;});}}
# 2904
if(_tmp518){
struct Cyc_Absyn_Exp*_tmp459=({void*_tmpA13=_tmp454;Cyc_Toc_cast_it(_tmpA13,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmpA16=({struct Cyc_List_List*_tmpA15=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA15,({struct Cyc_List_List*_tmp45B=_cycalloc(sizeof(*_tmp45B));({struct _tuple20*_tmpA14=({struct _tuple20*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->f1=0,_tmp45A->f2=_tmp459;_tmp45A;});_tmp45B->hd=_tmpA14;}),_tmp45B->tl=0;_tmp45B;}));});es=_tmpA16;});}}
# 2909
({void*_tmpA17=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmpA17;});};}
# 2911
goto _LL0;case 28U: _LL3D: _tmp51E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp51D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp51C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp311)->f3;_LL3E:
# 2918
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmpA18=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmpA18;});else{
# 2921
Cyc_Toc_exp_to_c(nv,_tmp51E);}
goto _LL0;case 30U: _LL3F: _tmp520=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp51F=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL40:
# 2925
{struct Cyc_List_List*_tmp45E=_tmp51F;for(0;_tmp45E != 0;_tmp45E=_tmp45E->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp45E->hd)).f2);}}{
void*_tmp45F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp45F;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp460=Cyc_Tcutil_compress(_tmp45F);void*_tmp461=_tmp460;union Cyc_Absyn_AggrInfo _tmp465;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp461)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp461)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp461)->f2 == 0){_LL10A: _tmp465=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp461)->f1)->f1;_LL10B: {
# 2932
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp465);
({void*_tmpA19=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->tag=29U,_tmp462->f1=sd->name,_tmp462->f2=0,_tmp462->f3=_tmp51F,_tmp462->f4=sd;_tmp462;});e->r=_tmpA19;});
e->topt=_tmp45F;
goto _LL109;}}else{goto _LL10C;}}else{goto _LL10C;}}else{_LL10C: _LL10D:
({void*_tmp463=0U;({struct _dyneither_ptr _tmpA1A=({const char*_tmp464="anonStruct type not an aggregate";_tag_dyneither(_tmp464,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA1A,_tag_dyneither(_tmp463,sizeof(void*),0U));});});}_LL109:;}else{
# 2939
({void*_tmpA1B=Cyc_Toc_unresolvedmem_exp_r(0,_tmp51F);e->r=_tmpA1B;});}
goto _LL0;};case 29U: _LL41: _tmp524=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp523=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp522=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp311)->f3;_tmp521=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp311)->f4;_LL42:
# 2945
 if(_tmp521 == 0  || _tmp521->impl == 0)
({void*_tmp466=0U;({struct _dyneither_ptr _tmpA1C=({const char*_tmp467="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp467,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA1C,_tag_dyneither(_tmp466,sizeof(void*),0U));});});{
void*_tmp468=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp469=Cyc_Tcutil_compress(_tmp468);void*_tmp46A=_tmp469;union Cyc_Absyn_AggrInfo _tmp46D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46A)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46A)->f1)->tag == 20U){_LL10F: _tmp46D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46A)->f1)->f1;_LL110:
({struct _tuple1*_tmpA1D=(Cyc_Absyn_aggr_kinded_name(_tmp46D)).f2;*_tmp524=_tmpA1D;});goto _LL10E;}else{goto _LL111;}}else{_LL111: _LL112:
({void*_tmp46B=0U;({struct _dyneither_ptr _tmpA1E=({const char*_tmp46C="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp46C,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA1E,_tag_dyneither(_tmp46B,sizeof(void*),0U));});});}_LL10E:;}{
# 2956
struct Cyc_List_List*_tmp46E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->fields;
if(_tmp46E == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp46E))->tl != 0;_tmp46E=_tmp46E->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp46F=(struct Cyc_Absyn_Aggrfield*)_tmp46E->hd;
struct Cyc_List_List*_tmp470=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp522,_tmp521->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->fields);
# 2963
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp471=Cyc_Tcutil_compress(old_typ);void*_tmp472=_tmp471;struct Cyc_List_List*_tmp49E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp472)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp472)->f1)->tag == 20U){_LL114: _tmp49E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp472)->f2;_LL115:
# 2967
{struct Cyc_List_List*_tmp473=_tmp470;for(0;_tmp473 != 0;_tmp473=_tmp473->tl){
struct _tuple35*_tmp474=(struct _tuple35*)_tmp473->hd;struct _tuple35*_tmp475=_tmp474;struct Cyc_Absyn_Aggrfield*_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;_LL119: _tmp49B=_tmp475->f1;_tmp49A=_tmp475->f2;_LL11A:;{
void*_tmp476=_tmp49A->topt;
Cyc_Toc_exp_to_c(nv,_tmp49A);
# 2972
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp49B->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp49A->topt)))
({void*_tmpA20=({void*_tmpA1F=Cyc_Toc_typ_to_c(_tmp49B->type);Cyc_Toc_cast_it(_tmpA1F,
Cyc_Absyn_copy_exp(_tmp49A));})->r;
# 2974
_tmp49A->r=_tmpA20;});
# 2977
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->tagged){
struct _dyneither_ptr*_tmp477=_tmp49B->name;
struct Cyc_Absyn_Exp*_tmp478=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp521,_tmp477),0U);
struct _tuple20*_tmp479=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp478);
struct _tuple20*_tmp47A=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp49A);
struct _tuple1*s=({struct _tuple1*_tmp485=_cycalloc(sizeof(*_tmp485));({union Cyc_Absyn_Nmspace _tmpA24=Cyc_Absyn_Abs_n(0,1);_tmp485->f1=_tmpA24;}),({
struct _dyneither_ptr*_tmpA23=({struct _dyneither_ptr*_tmp484=_cycalloc(sizeof(*_tmp484));({struct _dyneither_ptr _tmpA22=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp482=({struct Cyc_String_pa_PrintArg_struct _tmp7B9;_tmp7B9.tag=0U,_tmp7B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp521->name).f2);_tmp7B9;});struct Cyc_String_pa_PrintArg_struct _tmp483=({struct Cyc_String_pa_PrintArg_struct _tmp7B8;_tmp7B8.tag=0U,_tmp7B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp477);_tmp7B8;});void*_tmp480[2U];_tmp480[0]=& _tmp482,_tmp480[1]=& _tmp483;({struct _dyneither_ptr _tmpA21=({const char*_tmp481="_union_%s_%s";_tag_dyneither(_tmp481,sizeof(char),13U);});Cyc_aprintf(_tmpA21,_tag_dyneither(_tmp480,sizeof(void*),2U));});});*_tmp484=_tmpA22;});_tmp484;});_tmp485->f2=_tmpA23;});_tmp485;});
# 2985
struct _tuple20*_tmp47B=({
struct _dyneither_ptr*_tmpA26=_tmp477;Cyc_Toc_make_field(_tmpA26,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->tag=29U,_tmp47F->f1=s,_tmp47F->f2=0,({struct Cyc_List_List*_tmpA25=({struct _tuple20*_tmp47E[2U];_tmp47E[0]=_tmp479,_tmp47E[1]=_tmp47A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47E,sizeof(struct _tuple20*),2U));});_tmp47F->f3=_tmpA25;}),_tmp47F->f4=0;_tmp47F;}),0U));});
# 2988
({void*_tmpA28=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->tag=29U,_tmp47D->f1=*_tmp524,_tmp47D->f2=0,({struct Cyc_List_List*_tmpA27=({struct _tuple20*_tmp47C[1U];_tmp47C[0]=_tmp47B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47C,sizeof(struct _tuple20*),1U));});_tmp47D->f3=_tmpA27;}),_tmp47D->f4=_tmp521;_tmp47D;});e->r=_tmpA28;});}
# 2994
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp46F == _tmp49B){
struct Cyc_List_List*_tmp486=({struct Cyc_List_List*_tmpA29=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->exist_vars,_tmp523);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA29,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp521->tvs,_tmp49E));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp487=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->fields;for(0;_tmp487 != 0;_tmp487=_tmp487->tl){
struct Cyc_Absyn_Aggrfield*_tmp488=(struct Cyc_Absyn_Aggrfield*)_tmp487->hd;
void*_tmp489=Cyc_Tcutil_substitute(_tmp486,_tmp488->type);
struct Cyc_Absyn_Aggrfield*_tmp48A=({struct Cyc_Absyn_Aggrfield*_tmpA2A=_tmp488;Cyc_Toc_aggrfield_to_c(_tmpA2A,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp486,_tmp489)));});
({struct Cyc_List_List*_tmpA2B=({struct Cyc_List_List*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B->hd=_tmp48A,_tmp48B->tl=new_fields;_tmp48B;});new_fields=_tmpA2B;});
# 3009
if(_tmp487->tl == 0){
{void*_tmp48C=Cyc_Tcutil_compress(_tmp48A->type);void*_tmp48D=_tmp48C;struct Cyc_Absyn_ArrayInfo _tmp492;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48D)->tag == 4U){_LL11C: _tmp492=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48D)->f1;_LL11D:
# 3012
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp492.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp48E=_tmp492;
({struct Cyc_Absyn_Exp*_tmpA2C=Cyc_Absyn_uint_exp(0U,0U);_tmp48E.num_elts=_tmpA2C;});
({void*_tmpA2D=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F->tag=4U,_tmp48F->f1=_tmp48E;_tmp48F;});_tmp48A->type=_tmpA2D;});}
# 3017
goto _LL11B;}else{_LL11E: _LL11F:
# 3021
 if(_tmp49A->topt == 0)
goto _LL11B;
{void*_tmp490=Cyc_Tcutil_compress((void*)_check_null(_tmp49A->topt));void*_tmp491=_tmp490;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp491)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp491)->f1)->tag == 20U){_LL121: _LL122:
# 3025
 _tmp48A->type=(void*)_check_null(_tmp49A->topt);goto _LL120;}else{goto _LL123;}}else{_LL123: _LL124:
 goto _LL120;}_LL120:;}
# 3028
goto _LL11B;}_LL11B:;}
# 3032
if(!Cyc_Tcutil_is_array_type(_tmp488->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp488->type)))
({struct Cyc_List_List*_tmpA2F=({struct Cyc_List_List*_tmp494=_cycalloc(sizeof(*_tmp494));({void*_tmpA2E=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->tag=6U,_tmp493->f1=0;_tmp493;});_tmp494->hd=_tmpA2E;}),_tmp494->tl=_tmp48A->attributes;_tmp494;});_tmp48A->attributes=_tmpA2F;});}}}
# 3038
({struct Cyc_Absyn_Aggrdecl*_tmpA33=({struct _dyneither_ptr*_tmpA32=({struct _dyneither_ptr*_tmp498=_cycalloc(sizeof(*_tmp498));({struct _dyneither_ptr _tmpA31=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp497=({struct Cyc_Int_pa_PrintArg_struct _tmp7BA;_tmp7BA.tag=1U,_tmp7BA.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7BA;});void*_tmp495[1U];_tmp495[0]=& _tmp497;({struct _dyneither_ptr _tmpA30=({const char*_tmp496="_genStruct%d";_tag_dyneither(_tmp496,sizeof(char),13U);});Cyc_aprintf(_tmpA30,_tag_dyneither(_tmp495,sizeof(void*),1U));});});*_tmp498=_tmpA31;});_tmp498;});Cyc_Toc_make_c_struct_defn(_tmpA32,
# 3040
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 3038
_tmp521=_tmpA33;});
# 3041
*_tmp524=_tmp521->name;
Cyc_Toc_aggrdecl_to_c(_tmp521,1);
# 3044
({void*_tmpA34=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp499=_cycalloc(sizeof(*_tmp499));*_tmp499=_tmp521;_tmp499;})),0);e->topt=_tmpA34;});}};}}
# 3047
goto _LL113;}else{goto _LL116;}}else{_LL116: _LL117:
({void*_tmp49C=0U;({struct _dyneither_ptr _tmpA35=({const char*_tmp49D="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp49D,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA35,_tag_dyneither(_tmp49C,sizeof(void*),0U));});});}_LL113:;}else{
# 3056
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->tagged){
# 3058
struct _tuple35*_tmp49F=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp470))->hd;struct _tuple35*_tmp4A0=_tmp49F;struct Cyc_Absyn_Aggrfield*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4A8;_LL126: _tmp4A9=_tmp4A0->f1;_tmp4A8=_tmp4A0->f2;_LL127:;{
void*_tmp4A1=(void*)_check_null(_tmp4A8->topt);
void*_tmp4A2=_tmp4A9->type;
Cyc_Toc_exp_to_c(nv,_tmp4A8);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp4A2) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp4A1))
({void*_tmpA37=({
void*_tmpA36=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmpA36,Cyc_Absyn_new_exp(_tmp4A8->r,0U));});
# 3064
_tmp4A8->r=_tmpA37;});{
# 3067
int i=Cyc_Toc_get_member_offset(_tmp521,_tmp4A9->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp4A3=({struct _tuple20*_tmp4A5[2U];({struct _tuple20*_tmpA39=({struct _tuple20*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->f1=0,_tmp4A6->f2=field_tag_exp;_tmp4A6;});_tmp4A5[0]=_tmpA39;}),({struct _tuple20*_tmpA38=({struct _tuple20*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->f1=0,_tmp4A7->f2=_tmp4A8;_tmp4A7;});_tmp4A5[1]=_tmpA38;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A5,sizeof(struct _tuple20*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp4A3,0U);
({void*_tmpA3B=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp4A4[1U];({struct _tuple20*_tmpA3A=Cyc_Toc_make_field(_tmp4A9->name,umem);_tmp4A4[0]=_tmpA3A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A4,sizeof(struct _tuple20*),1U));}));e->r=_tmpA3B;});};};}else{
# 3074
struct Cyc_List_List*_tmp4AA=0;
struct Cyc_List_List*_tmp4AB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->fields;
for(0;_tmp4AB != 0;_tmp4AB=_tmp4AB->tl){
struct Cyc_List_List*_tmp4AC=_tmp470;for(0;_tmp4AC != 0;_tmp4AC=_tmp4AC->tl){
if((*((struct _tuple35*)_tmp4AC->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp4AB->hd){
struct _tuple35*_tmp4AD=(struct _tuple35*)_tmp4AC->hd;struct _tuple35*_tmp4AE=_tmp4AD;struct Cyc_Absyn_Aggrfield*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B3;_LL129: _tmp4B4=_tmp4AE->f1;_tmp4B3=_tmp4AE->f2;_LL12A:;{
void*_tmp4AF=Cyc_Toc_typ_to_c(_tmp4B4->type);
void*_tmp4B0=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4B3->topt));
Cyc_Toc_exp_to_c(nv,_tmp4B3);
# 3084
if(!Cyc_Tcutil_unify(_tmp4AF,_tmp4B0)){
# 3086
if(!Cyc_Tcutil_is_arithmetic_type(_tmp4AF) || !
Cyc_Tcutil_is_arithmetic_type(_tmp4B0))
({struct Cyc_Absyn_Exp*_tmpA3D=({void*_tmpA3C=_tmp4AF;Cyc_Toc_cast_it(_tmpA3C,Cyc_Absyn_copy_exp(_tmp4B3));});_tmp4B3=_tmpA3D;});}
({struct Cyc_List_List*_tmpA3F=({struct Cyc_List_List*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));({struct _tuple20*_tmpA3E=({struct _tuple20*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->f1=0,_tmp4B1->f2=_tmp4B3;_tmp4B1;});_tmp4B2->hd=_tmpA3E;}),_tmp4B2->tl=_tmp4AA;_tmp4B2;});_tmp4AA=_tmpA3F;});
break;};}}}
# 3093
({void*_tmpA40=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4AA));e->r=_tmpA40;});}}
# 3096
goto _LL0;};};};case 31U: _LL43: _tmp527=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp526=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_tmp525=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp311)->f3;_LL44: {
# 3100
struct Cyc_List_List*_tmp4B5=_tmp525->typs;
{struct Cyc_List_List*_tmp4B6=_tmp527;for(0;_tmp4B6 != 0;(_tmp4B6=_tmp4B6->tl,_tmp4B5=_tmp4B5->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4B6->hd;
void*_tmp4B7=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4B5))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp4B7))
({void*_tmpA41=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmpA41;});}}{
# 3111
struct Cyc_Absyn_Exp*_tmp4B8=_tmp526->is_extensible?Cyc_Absyn_var_exp(_tmp525->name,0U):
 Cyc_Toc_datatype_tag(_tmp526,_tmp525->name);
# 3114
if(!Cyc_Toc_is_toplevel(nv)){
# 3116
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp527 != 0;(_tmp527=_tmp527->tl,++ i)){
({struct Cyc_List_List*_tmpA44=({struct Cyc_List_List*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));({struct _tuple20*_tmpA43=({struct _dyneither_ptr*_tmpA42=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmpA42,(struct Cyc_Absyn_Exp*)_tmp527->hd);});_tmp4B9->hd=_tmpA43;}),_tmp4B9->tl=dles;_tmp4B9;});dles=_tmpA44;});}}{
# 3122
struct _tuple20*_tmp4BA=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp4B8);
({void*_tmpA48=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC->tag=29U,({struct _tuple1*_tmpA47=Cyc_Toc_collapse_qvars(_tmp525->name,_tmp526->name);_tmp4BC->f1=_tmpA47;}),_tmp4BC->f2=0,({
struct Cyc_List_List*_tmpA46=({struct Cyc_List_List*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->hd=_tmp4BA,({struct Cyc_List_List*_tmpA45=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp4BB->tl=_tmpA45;});_tmp4BB;});_tmp4BC->f3=_tmpA46;}),_tmp4BC->f4=0;_tmp4BC;});
# 3123
e->r=_tmpA48;});};}else{
# 3128
struct Cyc_List_List*_tmp4BD=0;
for(0;_tmp527 != 0;_tmp527=_tmp527->tl){
({struct Cyc_List_List*_tmpA4A=({struct Cyc_List_List*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));({struct _tuple20*_tmpA49=({struct _tuple20*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->f1=0,_tmp4BE->f2=(struct Cyc_Absyn_Exp*)_tmp527->hd;_tmp4BE;});_tmp4BF->hd=_tmpA49;}),_tmp4BF->tl=_tmp4BD;_tmp4BF;});_tmp4BD=_tmpA4A;});}
({void*_tmpA4D=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));({struct _tuple20*_tmpA4C=({struct _tuple20*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->f1=0,_tmp4C0->f2=_tmp4B8;_tmp4C0;});_tmp4C1->hd=_tmpA4C;}),({
struct Cyc_List_List*_tmpA4B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4BD);_tmp4C1->tl=_tmpA4B;});_tmp4C1;}));
# 3131
e->r=_tmpA4D;});}
# 3134
goto _LL0;};}case 32U: _LL45: _LL46:
# 3136
 goto _LL48;case 33U: _LL47: _LL48:
 goto _LL0;case 34U: _LL49: _tmp52D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp311)->f1).is_calloc;_tmp52C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp311)->f1).rgn;_tmp52B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp311)->f1).elt_type;_tmp52A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp311)->f1).num_elts;_tmp529=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp311)->f1).fat_result;_tmp528=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp311)->f1).inline_call;_LL4A: {
# 3140
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp52B)));
Cyc_Toc_exp_to_c(nv,_tmp52A);
# 3144
if(_tmp529){
struct _tuple1*_tmp4C2=Cyc_Toc_temp_var();
struct _tuple1*_tmp4C3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp52D){
xexp=_tmp52A;
if(_tmp52C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52C;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpA50=({struct Cyc_Absyn_Exp*_tmpA4F=rgn;struct Cyc_Absyn_Exp*_tmpA4E=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA4F,_tmpA4E,Cyc_Absyn_var_exp(_tmp4C2,0U));});pexp=_tmpA50;});}else{
# 3155
({struct Cyc_Absyn_Exp*_tmpA53=({void*_tmpA52=*_tmp52B;struct Cyc_Absyn_Exp*_tmpA51=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA52,_tmpA51,Cyc_Absyn_var_exp(_tmp4C2,0U));});pexp=_tmpA53;});}
# 3157
({struct Cyc_Absyn_Exp*_tmpA58=({struct Cyc_Absyn_Exp*_tmpA57=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA57,({struct Cyc_Absyn_Exp*_tmp4C4[3U];({
struct Cyc_Absyn_Exp*_tmpA56=Cyc_Absyn_var_exp(_tmp4C3,0U);_tmp4C4[0]=_tmpA56;}),({struct Cyc_Absyn_Exp*_tmpA55=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp4C4[1]=_tmpA55;}),({
struct Cyc_Absyn_Exp*_tmpA54=Cyc_Absyn_var_exp(_tmp4C2,0U);_tmp4C4[2]=_tmpA54;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C4,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3157
rexp=_tmpA58;});}else{
# 3161
if(_tmp52C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52C;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp528)
({struct Cyc_Absyn_Exp*_tmpA5A=({struct Cyc_Absyn_Exp*_tmpA59=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpA59,Cyc_Absyn_var_exp(_tmp4C2,0U));});pexp=_tmpA5A;});else{
# 3167
({struct Cyc_Absyn_Exp*_tmpA5C=({struct Cyc_Absyn_Exp*_tmpA5B=rgn;Cyc_Toc_rmalloc_exp(_tmpA5B,Cyc_Absyn_var_exp(_tmp4C2,0U));});pexp=_tmpA5C;});}}else{
# 3169
({struct Cyc_Absyn_Exp*_tmpA5E=({void*_tmpA5D=*_tmp52B;Cyc_Toc_malloc_exp(_tmpA5D,Cyc_Absyn_var_exp(_tmp4C2,0U));});pexp=_tmpA5E;});}
# 3171
({struct Cyc_Absyn_Exp*_tmpA63=({struct Cyc_Absyn_Exp*_tmpA62=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA62,({struct Cyc_Absyn_Exp*_tmp4C5[3U];({struct Cyc_Absyn_Exp*_tmpA61=Cyc_Absyn_var_exp(_tmp4C3,0U);_tmp4C5[0]=_tmpA61;}),({struct Cyc_Absyn_Exp*_tmpA60=Cyc_Absyn_uint_exp(1U,0U);_tmp4C5[1]=_tmpA60;}),({
struct Cyc_Absyn_Exp*_tmpA5F=Cyc_Absyn_var_exp(_tmp4C2,0U);_tmp4C5[2]=_tmpA5F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C5,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3171
rexp=_tmpA63;});}{
# 3174
struct Cyc_Absyn_Stmt*_tmp4C6=({struct _tuple1*_tmpA69=_tmp4C2;void*_tmpA68=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmpA67=_tmp52A;Cyc_Absyn_declare_stmt(_tmpA69,_tmpA68,_tmpA67,({
struct _tuple1*_tmpA66=_tmp4C3;void*_tmpA65=Cyc_Absyn_cstar_type(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpA64=pexp;Cyc_Absyn_declare_stmt(_tmpA66,_tmpA65,_tmpA64,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpA6A=Cyc_Toc_stmt_exp_r(_tmp4C6);e->r=_tmpA6A;});};}else{
# 3179
if(_tmp52D){
if(_tmp52C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52C;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpA6D=({struct Cyc_Absyn_Exp*_tmpA6C=rgn;struct Cyc_Absyn_Exp*_tmpA6B=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA6C,_tmpA6B,_tmp52A);})->r;e->r=_tmpA6D;});}else{
# 3185
({void*_tmpA70=({void*_tmpA6F=*_tmp52B;struct Cyc_Absyn_Exp*_tmpA6E=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA6F,_tmpA6E,_tmp52A);})->r;e->r=_tmpA70;});}}else{
# 3188
if(_tmp52C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52C;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp528)
({void*_tmpA71=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp52A))->r;e->r=_tmpA71;});else{
# 3194
({void*_tmpA72=(Cyc_Toc_rmalloc_exp(rgn,_tmp52A))->r;e->r=_tmpA72;});}}else{
# 3196
({void*_tmpA73=(Cyc_Toc_malloc_exp(*_tmp52B,_tmp52A))->r;e->r=_tmpA73;});}}}
# 3200
goto _LL0;}case 35U: _LL4B: _tmp52F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp52E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL4C: {
# 3209
void*e1_old_typ=(void*)_check_null(_tmp52F->topt);
void*e2_old_typ=(void*)_check_null(_tmp52E->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp4C7=0U;({struct _dyneither_ptr _tmpA74=({const char*_tmp4C8="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp4C8,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA74,_tag_dyneither(_tmp4C7,sizeof(void*),0U));});});{
# 3217
unsigned int _tmp4C9=e->loc;
struct _tuple1*_tmp4CA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4CB=Cyc_Absyn_var_exp(_tmp4CA,_tmp4C9);_tmp4CB->topt=e1_old_typ;{
struct _tuple1*_tmp4CC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4CD=Cyc_Absyn_var_exp(_tmp4CC,_tmp4C9);_tmp4CD->topt=e2_old_typ;{
# 3223
struct Cyc_Absyn_Exp*_tmp4CE=({struct Cyc_Absyn_Exp*_tmpA76=Cyc_Tcutil_deep_copy_exp(1,_tmp52F);struct Cyc_Absyn_Exp*_tmpA75=_tmp4CD;Cyc_Absyn_assign_exp(_tmpA76,_tmpA75,_tmp4C9);});_tmp4CE->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4CF=Cyc_Absyn_exp_stmt(_tmp4CE,_tmp4C9);
struct Cyc_Absyn_Exp*_tmp4D0=({struct Cyc_Absyn_Exp*_tmpA78=Cyc_Tcutil_deep_copy_exp(1,_tmp52E);struct Cyc_Absyn_Exp*_tmpA77=_tmp4CB;Cyc_Absyn_assign_exp(_tmpA78,_tmpA77,_tmp4C9);});_tmp4D0->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4D1=Cyc_Absyn_exp_stmt(_tmp4D0,_tmp4C9);
# 3228
struct Cyc_Absyn_Stmt*_tmp4D2=({struct _tuple1*_tmpA80=_tmp4CA;void*_tmpA7F=e1_old_typ;struct Cyc_Absyn_Exp*_tmpA7E=_tmp52F;struct Cyc_Absyn_Stmt*_tmpA7D=({
struct _tuple1*_tmpA7C=_tmp4CC;void*_tmpA7B=e2_old_typ;struct Cyc_Absyn_Exp*_tmpA7A=_tmp52E;struct Cyc_Absyn_Stmt*_tmpA79=
Cyc_Absyn_seq_stmt(_tmp4CF,_tmp4D1,_tmp4C9);
# 3229
Cyc_Absyn_declare_stmt(_tmpA7C,_tmpA7B,_tmpA7A,_tmpA79,_tmp4C9);});
# 3228
Cyc_Absyn_declare_stmt(_tmpA80,_tmpA7F,_tmpA7E,_tmpA7D,_tmp4C9);});
# 3231
Cyc_Toc_stmt_to_c(nv,_tmp4D2);
({void*_tmpA81=Cyc_Toc_stmt_exp_r(_tmp4D2);e->r=_tmpA81;});
goto _LL0;};};};};};}case 38U: _LL4D: _tmp531=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_tmp530=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp311)->f2;_LL4E: {
# 3236
void*_tmp4D3=Cyc_Tcutil_compress((void*)_check_null(_tmp531->topt));
Cyc_Toc_exp_to_c(nv,_tmp531);
{void*_tmp4D4=_tmp4D3;struct Cyc_Absyn_Aggrdecl*_tmp4DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D4)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D4)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D4)->f1)->f1).KnownAggr).tag == 2){_LL12C: _tmp4DA=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D4)->f1)->f1).KnownAggr).val;_LL12D: {
# 3240
struct Cyc_Absyn_Exp*_tmp4D5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp4DA,_tmp530),0U);
struct Cyc_Absyn_Exp*_tmp4D6=Cyc_Toc_member_exp(_tmp531,_tmp530,0U);
struct Cyc_Absyn_Exp*_tmp4D7=Cyc_Toc_member_exp(_tmp4D6,Cyc_Toc_tag_sp,0U);
({void*_tmpA82=(Cyc_Absyn_eq_exp(_tmp4D7,_tmp4D5,0U))->r;e->r=_tmpA82;});
goto _LL12B;}}else{goto _LL12E;}}else{goto _LL12E;}}else{_LL12E: _LL12F:
({void*_tmp4D8=0U;({struct _dyneither_ptr _tmpA83=({const char*_tmp4D9="non-aggregate type in tagcheck";_tag_dyneither(_tmp4D9,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA83,_tag_dyneither(_tmp4D8,sizeof(void*),0U));});});}_LL12B:;}
# 3247
goto _LL0;}case 37U: _LL4F: _tmp532=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp311)->f1;_LL50:
 Cyc_Toc_stmt_to_c(nv,_tmp532);goto _LL0;case 36U: _LL51: _LL52:
({void*_tmp4DB=0U;({struct _dyneither_ptr _tmpA84=({const char*_tmp4DC="UnresolvedMem";_tag_dyneither(_tmp4DC,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA84,_tag_dyneither(_tmp4DB,sizeof(void*),0U));});});case 25U: _LL53: _LL54:
({void*_tmp4DD=0U;({struct _dyneither_ptr _tmpA85=({const char*_tmp4DE="compoundlit";_tag_dyneither(_tmp4DE,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpA85,_tag_dyneither(_tmp4DD,sizeof(void*),0U));});});case 39U: _LL55: _LL56:
({void*_tmp4DF=0U;({struct _dyneither_ptr _tmpA86=({const char*_tmp4E0="valueof(-)";_tag_dyneither(_tmp4E0,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA86,_tag_dyneither(_tmp4DF,sizeof(void*),0U));});});default: _LL57: _LL58:
({void*_tmp4E1=0U;({struct _dyneither_ptr _tmpA87=({const char*_tmp4E2="__asm__";_tag_dyneither(_tmp4E2,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA87,_tag_dyneither(_tmp4E1,sizeof(void*),0U));});});}_LL0:;};}struct _tuple36{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3280 "toc.cyc"
static struct _tuple36*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3282
return({struct _tuple36*_tmp533=_region_malloc(r,sizeof(*_tmp533));_tmp533->f1=0,({struct _dyneither_ptr*_tmpA89=Cyc_Toc_fresh_label();_tmp533->f2=_tmpA89;}),({struct _dyneither_ptr*_tmpA88=Cyc_Toc_fresh_label();_tmp533->f3=_tmpA88;}),_tmp533->f4=sc;_tmp533;});}
# 3287
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp534=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3293
if((int)(((_tmp534->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp537=_tmp534->orig_pat;if((_tmp537.pattern).tag != 1)_throw_match();(_tmp537.pattern).val;})->topt);
void*_tmp535=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp536=_tmp535;switch(*((int*)_tmp536)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp536)->f1)){case 0U: _LL1: _LL2:
# 3298
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpA8B=({void*_tmpA8A=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA8A,v);});v=_tmpA8B;});goto _LL0;}_LL0:;}{
# 3304
void*_tmp538=_tmp534->access;void*_tmp539=_tmp538;struct Cyc_Absyn_Datatypedecl*_tmp544;struct Cyc_Absyn_Datatypefield*_tmp543;unsigned int _tmp542;int _tmp541;struct _dyneither_ptr*_tmp540;unsigned int _tmp53F;switch(*((int*)_tmp539)){case 0U: _LLA: _LLB:
# 3309
 goto _LL9;case 1U: _LLC: _LLD:
# 3314
 if(ps->tl != 0){
void*_tmp53A=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp53B=_tmp53A;struct Cyc_Absyn_Datatypedecl*_tmp53E;struct Cyc_Absyn_Datatypefield*_tmp53D;unsigned int _tmp53C;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp53B)->tag == 3U){_LL15: _tmp53E=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp53B)->f1;_tmp53D=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp53B)->f2;_tmp53C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp53B)->f3;_LL16:
# 3317
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpA8E=({void*_tmpA8D=({void*_tmpA8C=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp53D->name,_tmp53E->name));Cyc_Absyn_cstar_type(_tmpA8C,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA8D,v);});v=_tmpA8E;});
({struct Cyc_Absyn_Exp*_tmpA90=({struct Cyc_Absyn_Exp*_tmpA8F=v;Cyc_Absyn_aggrarrow_exp(_tmpA8F,Cyc_Absyn_fieldname((int)(_tmp53C + 1)),0U);});v=_tmpA90;});
continue;}else{_LL17: _LL18:
# 3322
 goto _LL14;}_LL14:;}
# 3325
({struct Cyc_Absyn_Exp*_tmpA91=Cyc_Absyn_deref_exp(v,0U);v=_tmpA91;});
goto _LL9;case 2U: _LLE: _tmp53F=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp539)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpA93=({struct Cyc_Absyn_Exp*_tmpA92=v;Cyc_Toc_member_exp(_tmpA92,Cyc_Absyn_fieldname((int)(_tmp53F + 1)),0U);});v=_tmpA93;});goto _LL9;case 4U: _LL10: _tmp541=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp539)->f1;_tmp540=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp539)->f2;_LL11:
# 3329
({struct Cyc_Absyn_Exp*_tmpA94=Cyc_Toc_member_exp(v,_tmp540,0U);v=_tmpA94;});
if(_tmp541)
({struct Cyc_Absyn_Exp*_tmpA95=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpA95;});
goto _LL9;default: _LL12: _tmp544=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp539)->f1;_tmp543=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp539)->f2;_tmp542=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp539)->f3;_LL13:
# 3334
({struct Cyc_Absyn_Exp*_tmpA97=({struct Cyc_Absyn_Exp*_tmpA96=v;Cyc_Toc_member_exp(_tmpA96,Cyc_Absyn_fieldname((int)(_tmp542 + 1)),0U);});v=_tmpA97;});
goto _LL9;}_LL9:;};}
# 3338
return v;}
# 3343
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp545=t;struct Cyc_Absyn_Datatypedecl*_tmp55E;struct Cyc_Absyn_Datatypefield*_tmp55D;struct _dyneither_ptr*_tmp55C;int _tmp55B;int _tmp55A;struct Cyc_Absyn_Datatypedecl*_tmp559;struct Cyc_Absyn_Datatypefield*_tmp558;unsigned int _tmp557;struct _dyneither_ptr _tmp556;int _tmp555;void*_tmp554;struct Cyc_Absyn_Enumfield*_tmp553;struct Cyc_Absyn_Enumdecl*_tmp552;struct Cyc_Absyn_Enumfield*_tmp551;struct Cyc_Absyn_Exp*_tmp550;switch(*((int*)_tmp545)){case 0U: _LL1: _tmp550=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp545)->f1;_LL2:
# 3347
 if(_tmp550 == 0)return v;else{return _tmp550;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA98=v;Cyc_Absyn_eq_exp(_tmpA98,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA99=v;Cyc_Absyn_neq_exp(_tmpA99,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp552=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp551=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp545)->f2;_LL8:
# 3351
 return({struct Cyc_Absyn_Exp*_tmpA9A=v;Cyc_Absyn_eq_exp(_tmpA9A,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->tag=32U,_tmp546->f1=_tmp552,_tmp546->f2=_tmp551;_tmp546;}),0U),0U);});case 4U: _LL9: _tmp554=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp553=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp545)->f2;_LLA:
# 3353
 return({struct Cyc_Absyn_Exp*_tmpA9B=v;Cyc_Absyn_eq_exp(_tmpA9B,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->tag=33U,_tmp547->f1=_tmp554,_tmp547->f2=_tmp553;_tmp547;}),0U),0U);});case 5U: _LLB: _tmp556=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp555=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp545)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpA9C=v;Cyc_Absyn_eq_exp(_tmpA9C,Cyc_Absyn_float_exp(_tmp556,_tmp555,0U),0U);});case 6U: _LLD: _tmp557=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp545)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpA9D=v;Cyc_Absyn_eq_exp(_tmpA9D,Cyc_Absyn_signed_int_exp((int)_tmp557,0U),0U);});case 7U: _LLF: _tmp55A=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp559=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp545)->f2;_tmp558=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp545)->f3;_LL10:
# 3359
 LOOP1: {
void*_tmp548=v->r;void*_tmp549=_tmp548;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54A;switch(*((int*)_tmp549)){case 14U: _LL16: _tmp54A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp549)->f2;_LL17:
 v=_tmp54A;goto LOOP1;case 20U: _LL18: _tmp54B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp549)->f1;_LL19:
 v=_tmp54B;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3366
({struct Cyc_Absyn_Exp*_tmpAA0=({void*_tmpA9F=({void*_tmpA9E=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp558->name,_tmp559->name));Cyc_Absyn_cstar_type(_tmpA9E,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA9F,v);});v=_tmpAA0;});
return({struct Cyc_Absyn_Exp*_tmpAA1=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpAA1,Cyc_Absyn_uint_exp((unsigned int)_tmp55A,0U),0U);});case 8U: _LL11: _tmp55C=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp55B=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp545)->f2;_LL12:
# 3371
({struct Cyc_Absyn_Exp*_tmpAA3=({struct Cyc_Absyn_Exp*_tmpAA2=Cyc_Toc_member_exp(v,_tmp55C,0U);Cyc_Toc_member_exp(_tmpAA2,Cyc_Toc_tag_sp,0U);});v=_tmpAA3;});
return({struct Cyc_Absyn_Exp*_tmpAA4=v;Cyc_Absyn_eq_exp(_tmpAA4,Cyc_Absyn_signed_int_exp(_tmp55B,0U),0U);});default: _LL13: _tmp55E=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp55D=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp545)->f2;_LL14:
# 3375
 LOOP2: {
void*_tmp54C=v->r;void*_tmp54D=_tmp54C;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp54E;switch(*((int*)_tmp54D)){case 14U: _LL1D: _tmp54E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp54D)->f2;_LL1E:
 v=_tmp54E;goto LOOP2;case 20U: _LL1F: _tmp54F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp54D)->f1;_LL20:
 v=_tmp54F;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3382
({struct Cyc_Absyn_Exp*_tmpAA7=({void*_tmpAA6=({void*_tmpAA5=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp55D->name,_tmp55E->name));Cyc_Absyn_cstar_type(_tmpAA5,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpAA6,v);});v=_tmpAA7;});
return({struct Cyc_Absyn_Exp*_tmpAA8=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpAA8,Cyc_Absyn_var_exp(_tmp55D->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3396
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple37{int f1;void*f2;};
# 3404
static struct _tuple37 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3406
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp55F=*((struct _tuple33*)ss->hd);struct _tuple33 _tmp560=_tmp55F;void*_tmp566;_LL1: _tmp566=_tmp560.f1;_LL2:;{
void*_tmp561=_tmp566;struct Cyc_Absyn_Exp*_tmp565;struct _dyneither_ptr*_tmp564;switch(*((int*)_tmp561)){case 3U: _LL4: _LL5:
# 3412
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3415
 continue;case 8U: _LLA: _tmp564=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp561)->f1;_LLB:
# 3417
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpAA9=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->tag=3U,_tmp562->f1=_tmp564;_tmp562;});k=_tmpAA9;});
continue;case 7U: _LLC: _LLD:
# 3421
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp565=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp561)->f1;if(_tmp565 != 0){_LLF:
# 3425
({void*_tmpAAA=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=2U,_tmp563->f1=_tmp565;_tmp563;});k=_tmpAAA;});
return({struct _tuple37 _tmp7BB;_tmp7BB.f1=0,_tmp7BB.f2=k;_tmp7BB;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple37 _tmp7BC;_tmp7BC.f1=0,_tmp7BC.f2=k;_tmp7BC;});}_LL3:;};}
# 3434
return({struct _tuple37 _tmp7BD;_tmp7BD.f1=c,_tmp7BD.f2=k;_tmp7BD;});}
# 3439
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp567=p;int _tmp572;int _tmp571;unsigned int _tmp570;void*_tmp56F;struct Cyc_Absyn_Enumfield*_tmp56E;struct Cyc_Absyn_Enumdecl*_tmp56D;struct Cyc_Absyn_Enumfield*_tmp56C;switch(*((int*)_tmp567)){case 3U: _LL1: _tmp56D=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp567)->f1;_tmp56C=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp567)->f2;_LL2:
# 3443
({struct Cyc_Absyn_Exp*_tmpAAB=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->tag=32U,_tmp568->f1=_tmp56D,_tmp568->f2=_tmp56C;_tmp568;}),0U);e=_tmpAAB;});goto _LL0;case 4U: _LL3: _tmp56F=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp567)->f1;_tmp56E=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp567)->f2;_LL4:
# 3445
({struct Cyc_Absyn_Exp*_tmpAAC=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->tag=33U,_tmp569->f1=_tmp56F,_tmp569->f2=_tmp56E;_tmp569;}),0U);e=_tmpAAC;});goto _LL0;case 6U: _LL5: _tmp570=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp567)->f1;_LL6:
 _tmp571=(int)_tmp570;goto _LL8;case 7U: _LL7: _tmp571=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp567)->f1;_LL8:
 _tmp572=_tmp571;goto _LLA;case 8U: _LL9: _tmp572=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp567)->f2;_LLA:
# 3449
({struct Cyc_Absyn_Exp*_tmpAAD=Cyc_Absyn_uint_exp((unsigned int)_tmp572,0U);e=_tmpAAD;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp56A=0U;({struct _dyneither_ptr _tmpAAE=({const char*_tmp56B="compile_pat_test_as_case!";_tag_dyneither(_tmp56B,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAAE,_tag_dyneither(_tmp56A,sizeof(void*),0U));});});}_LL0:;}
# 3453
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=17U,_tmp573->f1=e;_tmp573;}),0U);}
# 3457
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple38{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3466
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3469
void*t=(void*)_check_null(p->topt);
void*_tmp574=p->r;void*_tmp575=_tmp574;union Cyc_Absyn_AggrInfo _tmp5A6;struct Cyc_List_List*_tmp5A5;struct Cyc_List_List*_tmp5A4;struct Cyc_List_List*_tmp5A3;struct Cyc_Absyn_Pat*_tmp5A2;struct Cyc_Absyn_Datatypedecl*_tmp5A1;struct Cyc_Absyn_Datatypefield*_tmp5A0;struct Cyc_List_List*_tmp59F;struct Cyc_Absyn_Vardecl*_tmp59E;struct Cyc_Absyn_Pat*_tmp59D;struct Cyc_Absyn_Vardecl*_tmp59C;struct Cyc_Absyn_Vardecl*_tmp59B;struct Cyc_Absyn_Pat*_tmp59A;struct Cyc_Absyn_Vardecl*_tmp599;switch(*((int*)_tmp575)){case 2U: _LL1: _tmp599=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp575)->f2;_LL2: {
# 3472
struct Cyc_Absyn_Pat*_tmp576=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp576->topt=p->topt;
_tmp59B=_tmp599;_tmp59A=_tmp576;goto _LL4;}case 1U: _LL3: _tmp59B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp575)->f1;_tmp59A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp575)->f2;_LL4:
# 3477
({struct Cyc_List_List*_tmpAAF=({struct Cyc_List_List*_tmp577=_region_malloc(rgn,sizeof(*_tmp577));_tmp577->hd=_tmp59B,_tmp577->tl=*decls;_tmp577;});*decls=_tmpAAF;});{
struct Cyc_Absyn_Stmt*_tmp578=({struct Cyc_Absyn_Exp*_tmpAB0=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->tag=4U,_tmp579->f1=_tmp59B;_tmp579;}),0U);Cyc_Absyn_assign_stmt(_tmpAB0,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpAB1=_tmp578;Cyc_Toc_seq_stmt_opt(_tmpAB1,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp59A));});};case 4U: _LL5: _tmp59C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp575)->f2;_LL6:
# 3482
({struct Cyc_List_List*_tmpAB2=({struct Cyc_List_List*_tmp57A=_region_malloc(rgn,sizeof(*_tmp57A));_tmp57A->hd=_tmp59C,_tmp57A->tl=*decls;_tmp57A;});*decls=_tmpAB2;});
return({struct Cyc_Absyn_Exp*_tmpAB3=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->tag=4U,_tmp57B->f1=_tmp59C;_tmp57B;}),0U);Cyc_Absyn_assign_stmt(_tmpAB3,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp59E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp575)->f1;_tmp59D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp575)->f2;_LLA:
# 3487
({struct Cyc_List_List*_tmpAB4=({struct Cyc_List_List*_tmp57C=_region_malloc(rgn,sizeof(*_tmp57C));_tmp57C->hd=_tmp59E,_tmp57C->tl=*decls;_tmp57C;});*decls=_tmpAB4;});
({void*_tmpAB5=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp59E->type=_tmpAB5;});{
# 3490
struct Cyc_Absyn_Stmt*_tmp57D=({struct Cyc_Absyn_Exp*_tmpAB8=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->tag=4U,_tmp57E->f1=_tmp59E;_tmp57E;}),0U);Cyc_Absyn_assign_stmt(_tmpAB8,({
void*_tmpAB7=({void*_tmpAB6=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmpAB6,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpAB7,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpAB9=_tmp57D;Cyc_Toc_seq_stmt_opt(_tmpAB9,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp59D));});};case 9U: _LLB: _LLC:
# 3495
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1)->r)->tag == 8U){_LL17: _tmp5A1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1)->r)->f1;_tmp5A0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1)->r)->f2;_tmp59F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1)->r)->f3;_LL18:
# 3504
 if(_tmp59F == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp5A0->name,_tmp5A1->name);
void*_tmp57F=({void*_tmpABA=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmpABA,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpABB=Cyc_Toc_cast_it(_tmp57F,path);path=_tmpABB;});{
int cnt=1;
struct Cyc_List_List*_tmp580=_tmp5A0->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp59F != 0;(_tmp59F=_tmp59F->tl,_tmp580=((struct Cyc_List_List*)_check_null(_tmp580))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp581=(struct Cyc_Absyn_Pat*)_tmp59F->hd;
if(_tmp581->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp582=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp580))->hd)).f2;
void*_tmp583=(void*)_check_null(_tmp581->topt);
void*_tmp584=Cyc_Toc_typ_to_c_array(_tmp583);
struct Cyc_Absyn_Exp*_tmp585=({struct Cyc_Absyn_Exp*_tmpABC=path;Cyc_Absyn_aggrarrow_exp(_tmpABC,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp582)))
({struct Cyc_Absyn_Exp*_tmpABD=Cyc_Toc_cast_it(_tmp584,_tmp585);_tmp585=_tmpABD;});
({struct Cyc_Absyn_Stmt*_tmpABF=({struct Cyc_Absyn_Stmt*_tmpABE=s;Cyc_Toc_seq_stmt_opt(_tmpABE,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp585,_tmp581));});s=_tmpABF;});};}
# 3522
return s;};};}else{_LL21: _tmp5A2=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1;_LL22:
# 3567
 return({struct _RegionHandle*_tmpAC3=rgn;struct Cyc_Toc_Env**_tmpAC2=nv;struct Cyc_List_List**_tmpAC1=decls;struct Cyc_Absyn_Exp*_tmpAC0=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpAC3,_tmpAC2,_tmpAC1,_tmpAC0,_tmp5A2);});}case 8U: _LL19: _tmp5A3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp575)->f3;_LL1A:
# 3524
 _tmp5A4=_tmp5A3;goto _LL1C;case 5U: _LL1B: _tmp5A4=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp575)->f1;_LL1C: {
# 3526
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp5A4 != 0;(_tmp5A4=_tmp5A4->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp586=(struct Cyc_Absyn_Pat*)_tmp5A4->hd;
if(_tmp586->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp587=(void*)_check_null(_tmp586->topt);
struct _dyneither_ptr*_tmp588=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpAC9=({struct Cyc_Absyn_Stmt*_tmpAC8=s;Cyc_Toc_seq_stmt_opt(_tmpAC8,({struct _RegionHandle*_tmpAC7=rgn;struct Cyc_Toc_Env**_tmpAC6=nv;struct Cyc_List_List**_tmpAC5=decls;struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Toc_member_exp(path,_tmp588,0U);Cyc_Toc_extract_pattern_vars(_tmpAC7,_tmpAC6,_tmpAC5,_tmpAC4,_tmp586);}));});s=_tmpAC9;});};}
# 3536
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp575)->f1 == 0){_LL1D: _LL1E:
({void*_tmp589=0U;({struct _dyneither_ptr _tmpACA=({const char*_tmp58A="unresolved aggregate pattern!";_tag_dyneither(_tmp58A,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACA,_tag_dyneither(_tmp589,sizeof(void*),0U));});});}else{_LL1F: _tmp5A6=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp575)->f1;_tmp5A5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp575)->f3;_LL20: {
# 3540
struct Cyc_Absyn_Aggrdecl*_tmp58B=Cyc_Absyn_get_known_aggrdecl(_tmp5A6);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp5A5 != 0;_tmp5A5=_tmp5A5->tl){
struct _tuple38*_tmp58C=(struct _tuple38*)_tmp5A5->hd;
struct Cyc_Absyn_Pat*_tmp58D=(*_tmp58C).f2;
if(_tmp58D->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp58E=Cyc_Absyn_designatorlist_to_fieldname((*_tmp58C).f1);
void*_tmp58F=(void*)_check_null(_tmp58D->topt);
void*_tmp590=Cyc_Toc_typ_to_c_array(_tmp58F);
struct Cyc_Absyn_Exp*_tmp591=Cyc_Toc_member_exp(path,_tmp58E,0U);
# 3552
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp58B->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpACB=Cyc_Toc_member_exp(_tmp591,Cyc_Toc_val_sp,0U);_tmp591=_tmpACB;});{
void*_tmp592=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp58B->impl))->fields,_tmp58E)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp592))
({struct Cyc_Absyn_Exp*_tmpACC=Cyc_Toc_cast_it(_tmp590,_tmp591);_tmp591=_tmpACC;});else{
if(!Cyc_Tcutil_is_array_type(_tmp592) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp592)))
# 3559
({struct Cyc_Absyn_Exp*_tmpACE=Cyc_Absyn_deref_exp(({void*_tmpACD=Cyc_Absyn_cstar_type(_tmp590,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpACD,
Cyc_Absyn_address_exp(_tmp591,0U));}),0U);
# 3559
_tmp591=_tmpACE;});}
# 3562
({struct Cyc_Absyn_Stmt*_tmpAD0=({struct Cyc_Absyn_Stmt*_tmpACF=s;Cyc_Toc_seq_stmt_opt(_tmpACF,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp591,_tmp58D));});s=_tmpAD0;});};};}
# 3564
return s;}}case 15U: _LL23: _LL24:
# 3569
({void*_tmp593=0U;({struct _dyneither_ptr _tmpAD1=({const char*_tmp594="unknownid pat";_tag_dyneither(_tmp594,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAD1,_tag_dyneither(_tmp593,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp595=0U;({struct _dyneither_ptr _tmpAD2=({const char*_tmp596="unknowncall pat";_tag_dyneither(_tmp596,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAD2,_tag_dyneither(_tmp595,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp597=0U;({struct _dyneither_ptr _tmpAD3=({const char*_tmp598="exp pat";_tag_dyneither(_tmp598,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAD3,_tag_dyneither(_tmp597,sizeof(void*),0U));});});}_LL0:;}struct _tuple39{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3580
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3588
void*_tmp5A7=dopt;struct Cyc_List_List*_tmp5E5;struct Cyc_List_List*_tmp5E4;void*_tmp5E3;struct Cyc_Tcpat_Rhs*_tmp5E2;if(_tmp5A7 == 0){_LL1: _LL2:
# 3590
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp5A7)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp5E2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp5A7)->f1;_LL6:
# 3595
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple36*_tmp5A8=(struct _tuple36*)lscs->hd;struct _tuple36*_tmp5A9=_tmp5A8;int*_tmp5B0;struct _dyneither_ptr*_tmp5AF;struct _dyneither_ptr*_tmp5AE;struct Cyc_Absyn_Switch_clause*_tmp5AD;_LLA: _tmp5B0=(int*)& _tmp5A9->f1;_tmp5AF=_tmp5A9->f2;_tmp5AE=_tmp5A9->f3;_tmp5AD=_tmp5A9->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp5AA=_tmp5AD->body;
if(_tmp5AA == _tmp5E2->rhs){
# 3600
if(*_tmp5B0)
return Cyc_Absyn_goto_stmt(_tmp5AF,0U);
*_tmp5B0=1;{
# 3604
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAD6=rgn;struct Cyc_List_List**_tmpAD5=decls;struct Cyc_Absyn_Exp*_tmpAD4=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAD6,& nv,_tmpAD5,_tmpAD4,_tmp5AD->pattern);});
# 3607
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp5AE,_tmp5AD->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAD7=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpAD7;});
({struct Cyc_Absyn_Stmt*_tmpAD8=Cyc_Absyn_label_stmt(_tmp5AF,res,0U);res=_tmpAD8;});
({struct Cyc_List_List*_tmpADA=({struct Cyc_List_List*_tmp5AC=_region_malloc(rgn,sizeof(*_tmp5AC));({struct _tuple39*_tmpAD9=({struct _tuple39*_tmp5AB=_region_malloc(rgn,sizeof(*_tmp5AB));_tmp5AB->f1=nv,_tmp5AB->f2=_tmp5AE,_tmp5AB->f3=_tmp5AA;_tmp5AB;});_tmp5AC->hd=_tmpAD9;}),_tmp5AC->tl=*bodies;_tmp5AC;});*bodies=_tmpADA;});
return res;};}};}
# 3615
({void*_tmp5B1=0U;({struct _dyneither_ptr _tmpADB=({const char*_tmp5B2="couldn't find rhs!";_tag_dyneither(_tmp5B2,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpADB,_tag_dyneither(_tmp5B1,sizeof(void*),0U));});});default: _LL7: _tmp5E5=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A7)->f1;_tmp5E4=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A7)->f2;_tmp5E3=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A7)->f3;_LL8: {
# 3618
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5E3,lscs,v);
# 3620
struct Cyc_Absyn_Exp*_tmp5B3=({struct Cyc_List_List*_tmpADC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5E5);Cyc_Toc_compile_path(_tmpADC,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp5B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5E4);
# 3623
struct _tuple37 _tmp5B5=Cyc_Toc_admits_switch(_tmp5B4);struct _tuple37 _tmp5B6=_tmp5B5;int _tmp5E1;void*_tmp5E0;_LLD: _tmp5E1=_tmp5B6.f1;_tmp5E0=_tmp5B6.f2;_LLE:;
if(_tmp5E1 > 1){
# 3627
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));
({struct Cyc_Absyn_Switch_clause*_tmpADE=({struct Cyc_Absyn_Switch_clause*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));({struct Cyc_Absyn_Pat*_tmpADD=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5C6->pattern=_tmpADD;}),_tmp5C6->pat_vars=0,_tmp5C6->where_clause=0,_tmp5C6->body=res,_tmp5C6->loc=0U;_tmp5C6;});_tmp5C7->hd=_tmpADE;}),_tmp5C7->tl=0;_tmp5C7;});
# 3630
for(0;_tmp5B4 != 0;_tmp5B4=_tmp5B4->tl){
struct _tuple33 _tmp5B7=*((struct _tuple33*)_tmp5B4->hd);struct _tuple33 _tmp5B8=_tmp5B7;void*_tmp5BE;void*_tmp5BD;_LL10: _tmp5BE=_tmp5B8.f1;_tmp5BD=_tmp5B8.f2;_LL11:;{
# 3633
struct Cyc_Absyn_Pat*_tmp5B9=Cyc_Toc_compile_pat_test_as_case(_tmp5BE);
# 3635
struct Cyc_Absyn_Stmt*_tmp5BA=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5BD,lscs,v);
# 3637
({struct Cyc_List_List*_tmpAE0=({struct Cyc_List_List*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));({struct Cyc_Absyn_Switch_clause*_tmpADF=({struct Cyc_Absyn_Switch_clause*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->pattern=_tmp5B9,_tmp5BB->pat_vars=0,_tmp5BB->where_clause=0,_tmp5BB->body=_tmp5BA,_tmp5BB->loc=0U;_tmp5BB;});_tmp5BC->hd=_tmpADF;}),_tmp5BC->tl=new_lscs;_tmp5BC;});new_lscs=_tmpAE0;});};}
# 3639
{void*_tmp5BF=_tmp5E0;struct _dyneither_ptr*_tmp5C4;switch(*((int*)_tmp5BF)){case 1U: _LL13: _LL14:
# 3642
 LOOP1: {
void*_tmp5C0=_tmp5B3->r;void*_tmp5C1=_tmp5C0;struct Cyc_Absyn_Exp*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;switch(*((int*)_tmp5C1)){case 14U: _LL1C: _tmp5C2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C1)->f2;_LL1D:
 _tmp5B3=_tmp5C2;goto LOOP1;case 20U: _LL1E: _tmp5C3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5C1)->f1;_LL1F:
 _tmp5B3=_tmp5C3;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3648
({struct Cyc_Absyn_Exp*_tmpAE2=Cyc_Absyn_deref_exp(({void*_tmpAE1=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpAE1,_tmp5B3);}),0U);_tmp5B3=_tmpAE2;});goto _LL12;case 3U: _LL15: _tmp5C4=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp5BF)->f1;_LL16:
# 3651
({struct Cyc_Absyn_Exp*_tmpAE4=({struct Cyc_Absyn_Exp*_tmpAE3=Cyc_Toc_member_exp(_tmp5B3,_tmp5C4,0U);Cyc_Toc_member_exp(_tmpAE3,Cyc_Toc_tag_sp,0U);});_tmp5B3=_tmpAE4;});goto _LL12;case 2U: _LL17: _LL18:
# 3653
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3656
({struct Cyc_Absyn_Stmt*_tmpAE5=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->tag=10U,_tmp5C5->f1=_tmp5B3,_tmp5C5->f2=new_lscs,_tmp5C5->f3=0;_tmp5C5;}),0U);res=_tmpAE5;});}else{
# 3660
void*_tmp5C8=_tmp5E0;struct Cyc_Absyn_Exp*_tmp5DF;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C8)->tag == 2U){_LL23: _tmp5DF=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C8)->f1;_LL24: {
# 3662
struct Cyc_List_List*_tmp5C9=_tmp5B4;void*_tmp5D8;struct Cyc_Tcpat_Rhs*_tmp5D7;if(_tmp5C9 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp5C9)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5C9)->tl == 0){_LL28: _tmp5D8=((struct _tuple33*)_tmp5C9->hd)->f1;_tmp5D7=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp5C9->hd)->f2)->f1;_LL29:
# 3668
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple36*_tmp5CA=(struct _tuple36*)lscs->hd;struct _tuple36*_tmp5CB=_tmp5CA;int*_tmp5D2;struct _dyneither_ptr*_tmp5D1;struct _dyneither_ptr*_tmp5D0;struct Cyc_Absyn_Switch_clause*_tmp5CF;_LL2D: _tmp5D2=(int*)& _tmp5CB->f1;_tmp5D1=_tmp5CB->f2;_tmp5D0=_tmp5CB->f3;_tmp5CF=_tmp5CB->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp5CC=_tmp5CF->body;
if(_tmp5CC == _tmp5D7->rhs){
# 3673
if(*_tmp5D2)
return Cyc_Absyn_goto_stmt(_tmp5D1,0U);
*_tmp5D2=1;{
# 3677
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAE8=rgn;struct Cyc_List_List**_tmpAE7=decls;struct Cyc_Absyn_Exp*_tmpAE6=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAE8,& nv,_tmpAE7,_tmpAE6,_tmp5CF->pattern);});
# 3680
Cyc_Toc_exp_to_c(nv,_tmp5DF);{
# 3683
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp5D0,_tmp5CF->body,0U);
({struct Cyc_Absyn_Stmt*_tmpAE9=Cyc_Absyn_ifthenelse_stmt(_tmp5DF,r,res,0U);r=_tmpAE9;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAEA=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpAEA;});
({struct Cyc_Absyn_Stmt*_tmpAEB=Cyc_Absyn_label_stmt(_tmp5D1,r,0U);r=_tmpAEB;});
({struct Cyc_List_List*_tmpAED=({struct Cyc_List_List*_tmp5CE=_region_malloc(rgn,sizeof(*_tmp5CE));({struct _tuple39*_tmpAEC=({struct _tuple39*_tmp5CD=_region_malloc(rgn,sizeof(*_tmp5CD));_tmp5CD->f1=nv,_tmp5CD->f2=_tmp5D0,_tmp5CD->f3=_tmp5CC;_tmp5CD;});_tmp5CE->hd=_tmpAEC;}),_tmp5CE->tl=*bodies;_tmp5CE;});*bodies=_tmpAED;});
return r;};};}};}
# 3692
({void*_tmp5D3=0U;({struct _dyneither_ptr _tmpAEE=({const char*_tmp5D4="couldn't find rhs!";_tag_dyneither(_tmp5D4,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAEE,_tag_dyneither(_tmp5D3,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp5D5=0U;({struct _dyneither_ptr _tmpAEF=({const char*_tmp5D6="bad where clause in match compiler";_tag_dyneither(_tmp5D6,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAEF,_tag_dyneither(_tmp5D5,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3697
 for(0;_tmp5B4 != 0;_tmp5B4=_tmp5B4->tl){
struct _tuple33 _tmp5D9=*((struct _tuple33*)_tmp5B4->hd);struct _tuple33 _tmp5DA=_tmp5D9;void*_tmp5DE;void*_tmp5DD;_LL30: _tmp5DE=_tmp5DA.f1;_tmp5DD=_tmp5DA.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp5DB=Cyc_Toc_compile_pat_test(_tmp5B3,_tmp5DE);
struct Cyc_Absyn_Stmt*_tmp5DC=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5DD,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpAF0=Cyc_Absyn_ifthenelse_stmt(_tmp5DB,_tmp5DC,res,0U);res=_tmpAF0;});};}}_LL22:;}
# 3705
return res;}}}_LL0:;}
# 3715
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3717
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple39*_tmp5E6=(struct _tuple39*)bodies->hd;struct _tuple39*_tmp5E7=_tmp5E6;struct Cyc_Toc_Env**_tmp5E9;struct Cyc_Absyn_Stmt*_tmp5E8;_LL1: _tmp5E9=(struct Cyc_Toc_Env**)& _tmp5E7->f1;_tmp5E8=_tmp5E7->f3;_LL2:;
if(_tmp5E8 == s)return _tmp5E9;}
# 3723
return 0;}
# 3727
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3730
void*_tmp5EA=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3733
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5EB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5EB;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5EC=Cyc_Toc_share_env(rgn,nv);
# 3740
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple36*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3745
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp5EC,& mydecls,& mybodies,dopt,lscs,v);
# 3753
{struct Cyc_List_List*_tmp5ED=lscs;for(0;_tmp5ED != 0;_tmp5ED=_tmp5ED->tl){
struct _tuple36*_tmp5EE=(struct _tuple36*)_tmp5ED->hd;struct _tuple36*_tmp5EF=_tmp5EE;struct _dyneither_ptr*_tmp5F7;struct Cyc_Absyn_Switch_clause*_tmp5F6;_LL1: _tmp5F7=_tmp5EF->f3;_tmp5F6=_tmp5EF->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp5F6->body;
# 3757
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3760
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5F0=*envp;
# 3763
if(_tmp5ED->tl != 0){
struct _tuple36*_tmp5F1=(struct _tuple36*)((struct Cyc_List_List*)_check_null(_tmp5ED->tl))->hd;struct _tuple36*_tmp5F2=_tmp5F1;struct _dyneither_ptr*_tmp5F5;struct Cyc_Absyn_Switch_clause*_tmp5F4;_LL4: _tmp5F5=_tmp5F2->f3;_tmp5F4=_tmp5F2->f4;_LL5:;{
# 3767
struct Cyc_Toc_Env**_tmp5F3=Cyc_Toc_find_case_env(mybodies,_tmp5F4->body);
# 3772
if(_tmp5F3 == 0)
({struct Cyc_Toc_Env*_tmpAF1=Cyc_Toc_last_switchclause_env(rgn,_tmp5F0,end_l);Cyc_Toc_stmt_to_c(_tmpAF1,s);});else{
# 3776
struct Cyc_List_List*vs=0;
if(_tmp5F4->pat_vars != 0){
({struct Cyc_List_List*_tmpAF2=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5F4->pat_vars))->v)).f1);vs=_tmpAF2;});
({struct Cyc_List_List*_tmpAF3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpAF3;});}
# 3781
({struct Cyc_Toc_Env*_tmpAF4=Cyc_Toc_non_last_switchclause_env(rgn,_tmp5F0,end_l,_tmp5F5,vs);Cyc_Toc_stmt_to_c(_tmpAF4,s);});}};}else{
# 3785
({struct Cyc_Toc_Env*_tmpAF5=Cyc_Toc_last_switchclause_env(rgn,_tmp5F0,end_l);Cyc_Toc_stmt_to_c(_tmpAF5,s);});}};};}}{
# 3789
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpAF7=test_tree;Cyc_Absyn_seq_stmt(_tmpAF7,({struct _dyneither_ptr*_tmpAF6=end_l;Cyc_Absyn_label_stmt(_tmpAF6,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3791
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp5F8=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpAF8=Cyc_Toc_temp_var();_tmp5F8->name=_tmpAF8;});
({void*_tmpAF9=Cyc_Toc_typ_to_c_array(_tmp5F8->type);_tmp5F8->type=_tmpAF9;});
({struct Cyc_Absyn_Stmt*_tmpAFB=({struct Cyc_Absyn_Decl*_tmpAFA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->tag=0U,_tmp5F9->f1=_tmp5F8;_tmp5F9;}),0U);Cyc_Absyn_decl_stmt(_tmpAFA,res,0U);});res=_tmpAFB;});}
# 3798
({void*_tmpAFF=({struct _tuple1*_tmpAFE=v;void*_tmpAFD=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpAFC=e;Cyc_Absyn_declare_stmt(_tmpAFE,_tmpAFD,_tmpAFC,res,0U);})->r;whole_s->r=_tmpAFF;});
_npop_handler(0U);return;};}
# 3737
;_pop_region(rgn);};}
# 3804
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3806
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3811
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3813
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpB01,({struct Cyc_List_List*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));
({struct Cyc_Absyn_Exp*_tmpB00=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp5FA->hd=_tmpB00;}),_tmp5FA->tl=0;_tmp5FA;}),0U);}),0U);}
# 3817
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpB03=({struct Cyc_Absyn_Stmt*_tmpB02=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpB02,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpB03;});}
# 3822
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3824
while(1){
void*_tmp5FB=s->r;void*_tmp5FC=_tmp5FB;struct Cyc_Absyn_Stmt*_tmp653;struct Cyc_List_List*_tmp652;void*_tmp651;struct Cyc_Absyn_Stmt*_tmp650;struct Cyc_Absyn_Exp*_tmp64F;struct _dyneither_ptr*_tmp64E;struct Cyc_Absyn_Stmt*_tmp64D;struct Cyc_Absyn_Decl*_tmp64C;struct Cyc_Absyn_Stmt*_tmp64B;struct Cyc_List_List*_tmp64A;struct Cyc_Absyn_Switch_clause**_tmp649;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_List_List*_tmp647;void*_tmp646;struct Cyc_Absyn_Exp*_tmp645;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_Absyn_Stmt*_tmp642;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Stmt*_tmp640;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Stmt*_tmp63E;struct Cyc_Absyn_Stmt*_tmp63D;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Stmt*_tmp63B;struct Cyc_Absyn_Stmt*_tmp63A;struct Cyc_Absyn_Exp*_tmp639;switch(*((int*)_tmp5FC)){case 0U: _LL1: _LL2:
# 3827
 return;case 1U: _LL3: _tmp639=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_LL4:
# 3829
 Cyc_Toc_exp_to_c(nv,_tmp639);
return;case 2U: _LL5: _tmp63B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp63A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LL6:
# 3832
 Cyc_Toc_stmt_to_c(nv,_tmp63B);
s=_tmp63A;
continue;case 3U: _LL7: _tmp63C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_LL8: {
# 3836
void*topt=0;
if(_tmp63C != 0){
({void*_tmpB04=Cyc_Toc_typ_to_c((void*)_check_null(_tmp63C->topt));topt=_tmpB04;});
Cyc_Toc_exp_to_c(nv,_tmp63C);}{
# 3842
int _tmp5FD=Cyc_Toc_get_npop(s);
if(_tmp5FD > 0){
if(topt != 0){
struct _tuple1*_tmp5FE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp5FF=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp5FE,0U),0U);
({void*_tmpB09=({struct _tuple1*_tmpB08=_tmp5FE;void*_tmpB07=topt;struct Cyc_Absyn_Exp*_tmpB06=_tmp63C;Cyc_Absyn_declare_stmt(_tmpB08,_tmpB07,_tmpB06,({
struct Cyc_Absyn_Stmt*_tmpB05=Cyc_Toc_make_npop_handler(_tmp5FD);Cyc_Absyn_seq_stmt(_tmpB05,_tmp5FF,0U);}),0U);})->r;
# 3847
s->r=_tmpB09;});}else{
# 3851
Cyc_Toc_do_npop_before(_tmp5FD,s);}}
# 3853
return;};}case 4U: _LL9: _tmp63F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp63E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_tmp63D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f3;_LLA:
# 3855
 Cyc_Toc_exp_to_c(nv,_tmp63F);
Cyc_Toc_stmt_to_c(nv,_tmp63E);
s=_tmp63D;
continue;case 5U: _LLB: _tmp641=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1).f1;_tmp640=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LLC:
# 3860
 Cyc_Toc_exp_to_c(nv,_tmp641);{
struct _RegionHandle _tmp600=_new_region("temp");struct _RegionHandle*temp=& _tmp600;_push_region(temp);
({struct Cyc_Toc_Env*_tmpB0A=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpB0A,_tmp640);});
# 3864
_npop_handler(0U);return;
# 3861
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3866
struct Cyc_Toc_Env*_tmp601=nv;struct _dyneither_ptr**_tmp602;_LL22: _tmp602=_tmp601->break_lab;_LL23:;
if(_tmp602 != 0)
({void*_tmpB0B=Cyc_Toc_goto_stmt_r(*_tmp602);s->r=_tmpB0B;});
# 3870
({int _tmpB0C=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpB0C,s);});
return;}case 7U: _LLF: _LL10: {
# 3873
struct Cyc_Toc_Env*_tmp603=nv;struct _dyneither_ptr**_tmp604;_LL25: _tmp604=_tmp603->continue_lab;_LL26:;
if(_tmp604 != 0)
({void*_tmpB0D=Cyc_Toc_goto_stmt_r(*_tmp604);s->r=_tmpB0D;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3879
({int _tmpB0E=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpB0E,s);});
return;case 9U: _LL13: _tmp645=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp644=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2).f1;_tmp643=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f3).f1;_tmp642=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f4;_LL14:
# 3883
 Cyc_Toc_exp_to_c(nv,_tmp645);Cyc_Toc_exp_to_c(nv,_tmp644);Cyc_Toc_exp_to_c(nv,_tmp643);{
struct _RegionHandle _tmp605=_new_region("temp");struct _RegionHandle*temp=& _tmp605;_push_region(temp);
({struct Cyc_Toc_Env*_tmpB0F=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpB0F,_tmp642);});
# 3887
_npop_handler(0U);return;
# 3884
;_pop_region(temp);};case 10U: _LL15: _tmp648=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp647=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_tmp646=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f3;_LL16:
# 3889
 Cyc_Toc_xlate_switch(nv,s,_tmp648,_tmp647,_tmp646);
return;case 11U: _LL17: _tmp64A=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp649=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LL18: {
# 3892
struct Cyc_Toc_Env*_tmp606=nv;struct Cyc_Toc_FallthruInfo*_tmp610;_LL28: _tmp610=_tmp606->fallthru_info;_LL29:;
if(_tmp610 == 0)
({void*_tmp607=0U;({struct _dyneither_ptr _tmpB10=({const char*_tmp608="fallthru in unexpected place";_tag_dyneither(_tmp608,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB10,_tag_dyneither(_tmp607,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp609=*_tmp610;struct Cyc_Toc_FallthruInfo _tmp60A=_tmp609;struct _dyneither_ptr*_tmp60F;struct Cyc_List_List*_tmp60E;_LL2B: _tmp60F=_tmp60A.label;_tmp60E=_tmp60A.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp60F,0U);
# 3898
({int _tmpB11=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpB11,s2);});{
struct Cyc_List_List*_tmp60B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp60E);
struct Cyc_List_List*_tmp60C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp64A);
for(0;_tmp60B != 0;(_tmp60B=_tmp60B->tl,_tmp60C=_tmp60C->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp60C))->hd);
({struct Cyc_Absyn_Stmt*_tmpB14=({struct Cyc_Absyn_Stmt*_tmpB13=({struct Cyc_Absyn_Exp*_tmpB12=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->tag=5U,_tmp60D->f1=(struct Cyc_Absyn_Vardecl*)_tmp60B->hd;_tmp60D;}),0U);Cyc_Absyn_assign_stmt(_tmpB12,(struct Cyc_Absyn_Exp*)_tmp60C->hd,0U);});Cyc_Absyn_seq_stmt(_tmpB13,s2,0U);});s2=_tmpB14;});}
# 3906
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp64C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp64B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LL1A:
# 3912
{void*_tmp611=_tmp64C->r;void*_tmp612=_tmp611;struct Cyc_Absyn_Tvar*_tmp62D;struct Cyc_Absyn_Vardecl*_tmp62C;struct Cyc_Absyn_Exp*_tmp62B;struct Cyc_Absyn_Fndecl*_tmp62A;struct Cyc_List_List*_tmp629;struct Cyc_Absyn_Pat*_tmp628;struct Cyc_Absyn_Exp*_tmp627;void*_tmp626;struct Cyc_Absyn_Vardecl*_tmp625;switch(*((int*)_tmp612)){case 0U: _LL2E: _tmp625=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp625,_tmp64B);goto _LL2D;case 2U: _LL30: _tmp628=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_tmp627=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp612)->f3;_tmp626=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp612)->f4;_LL31:
# 3920
{void*_tmp613=_tmp628->r;void*_tmp614=_tmp613;struct Cyc_Absyn_Vardecl*_tmp616;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp614)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp614)->f2)->r)->tag == 0U){_LL3B: _tmp616=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp614)->f1;_LL3C:
# 3922
({struct _tuple1*_tmpB15=Cyc_Toc_temp_var();_tmp616->name=_tmpB15;});
_tmp616->initializer=_tmp627;
({void*_tmpB16=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=0U,_tmp615->f1=_tmp616;_tmp615;});_tmp64C->r=_tmpB16;});
Cyc_Toc_local_decl_to_c(nv,_tmp616,_tmp64B);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3931
({void*_tmpB17=(Cyc_Toc_letdecl_to_c(nv,_tmp628,_tmp626,(void*)_check_null(_tmp627->topt),_tmp627,_tmp64B))->r;s->r=_tmpB17;});
goto _LL3A;}_LL3A:;}
# 3934
goto _LL2D;case 3U: _LL32: _tmp629=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_LL33: {
# 3938
struct Cyc_List_List*_tmp617=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp629);
if(_tmp617 == 0)
({void*_tmp618=0U;({struct _dyneither_ptr _tmpB18=({const char*_tmp619="empty Letv_d";_tag_dyneither(_tmp619,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB18,_tag_dyneither(_tmp618,sizeof(void*),0U));});});
({void*_tmpB19=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tag=0U,_tmp61A->f1=(struct Cyc_Absyn_Vardecl*)_tmp617->hd;_tmp61A;});_tmp64C->r=_tmpB19;});
_tmp617=_tmp617->tl;
for(0;_tmp617 != 0;_tmp617=_tmp617->tl){
struct Cyc_Absyn_Decl*_tmp61B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->tag=0U,_tmp61C->f1=(struct Cyc_Absyn_Vardecl*)_tmp617->hd;_tmp61C;}),0U);
({void*_tmpB1B=({struct Cyc_Absyn_Decl*_tmpB1A=_tmp61B;Cyc_Absyn_decl_stmt(_tmpB1A,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpB1B;});}
# 3947
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp62A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_LL35:
# 3950
 Cyc_Toc_fndecl_to_c(nv,_tmp62A,0);
Cyc_Toc_stmt_to_c(nv,_tmp64B);
goto _LL2D;case 4U: _LL36: _tmp62D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_tmp62C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp612)->f2;_tmp62B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp612)->f3;_LL37: {
# 3954
struct Cyc_Absyn_Stmt*_tmp61D=_tmp64B;
# 3956
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp62C->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3963
Cyc_Toc_stmt_to_c(nv,_tmp61D);
if(Cyc_Absyn_no_regions)
({void*_tmpB1F=({struct _tuple1*_tmpB1E=x_var;void*_tmpB1D=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpB1C=
Cyc_Absyn_uint_exp(0U,0U);
# 3965
Cyc_Absyn_declare_stmt(_tmpB1E,_tmpB1D,_tmpB1C,_tmp61D,0U);})->r;s->r=_tmpB1F;});else{
# 3967
if((unsigned int)_tmp62B){
# 3969
Cyc_Toc_exp_to_c(nv,_tmp62B);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpB21=_tmp62B;Cyc_Absyn_aggrarrow_exp(_tmpB21,({struct _dyneither_ptr*_tmp61F=_cycalloc(sizeof(*_tmp61F));({struct _dyneither_ptr _tmpB20=({const char*_tmp61E="h";_tag_dyneither(_tmp61E,sizeof(char),2U);});*_tmp61F=_tmpB20;});_tmp61F;}),0U);}),0U);
({void*_tmpB22=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp61D,0U))->r;s->r=_tmpB22;});};}else{
# 3979
({void*_tmpB2F=({
struct _tuple1*_tmpB2E=rh_var;void*_tmpB2D=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpB2C=({
struct Cyc_Absyn_Exp*_tmpB24=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpB24,({struct Cyc_List_List*_tmp620=_cycalloc(sizeof(*_tmp620));
({struct Cyc_Absyn_Exp*_tmpB23=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp620->hd=_tmpB23;}),_tmp620->tl=0;_tmp620;}),0U);});
# 3980
Cyc_Absyn_declare_stmt(_tmpB2E,_tmpB2D,_tmpB2C,({
# 3984
struct _tuple1*_tmpB2B=x_var;void*_tmpB2A=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpB29=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpB2B,_tmpB2A,_tmpB29,({
struct Cyc_Absyn_Stmt*_tmpB28=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB25=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpB25,({struct Cyc_Absyn_Exp*_tmp621[1U];_tmp621[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp621,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpB28,({
struct Cyc_Absyn_Stmt*_tmpB27=_tmp61D;Cyc_Absyn_seq_stmt(_tmpB27,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB26=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpB26,({struct Cyc_Absyn_Exp*_tmp622[1U];_tmp622[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp622,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3979
s->r=_tmpB2F;});}}
# 3990
return;}default: _LL38: _LL39:
({void*_tmp623=0U;({struct _dyneither_ptr _tmpB30=({const char*_tmp624="bad nested declaration within function";_tag_dyneither(_tmp624,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB30,_tag_dyneither(_tmp623,sizeof(void*),0U));});});}_LL2D:;}
# 3993
return;case 13U: _LL1B: _tmp64E=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp64D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LL1C:
# 3995
 s=_tmp64D;continue;case 14U: _LL1D: _tmp650=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp64F=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2).f1;_LL1E: {
# 3997
struct _RegionHandle _tmp62E=_new_region("temp");struct _RegionHandle*temp=& _tmp62E;_push_region(temp);
({struct Cyc_Toc_Env*_tmpB31=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpB31,_tmp650);});
Cyc_Toc_exp_to_c(nv,_tmp64F);
# 4001
_npop_handler(0U);return;
# 3997
;_pop_region(temp);}default: _LL1F: _tmp653=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp652=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_tmp651=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f3;_LL20: {
# 4015 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 4025
e_exp->topt=e_typ;{
struct _RegionHandle _tmp62F=_new_region("temp");struct _RegionHandle*temp=& _tmp62F;_push_region(temp);
# 4028
Cyc_Toc_stmt_to_c(nv,_tmp653);{
struct Cyc_Absyn_Stmt*_tmp630=({struct Cyc_Absyn_Stmt*_tmpB32=_tmp653;Cyc_Absyn_seq_stmt(_tmpB32,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 4033
struct Cyc_Absyn_Stmt*_tmp631=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->tag=10U,_tmp638->f1=e_exp,_tmp638->f2=_tmp652,_tmp638->f3=_tmp651;_tmp638;}),0U);
# 4035
Cyc_Toc_stmt_to_c(nv,_tmp631);{
# 4038
struct Cyc_Absyn_Exp*_tmp632=({
struct Cyc_Absyn_Exp*_tmpB34=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpB34,({struct Cyc_List_List*_tmp637=_cycalloc(sizeof(*_tmp637));
({struct Cyc_Absyn_Exp*_tmpB33=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp637->hd=_tmpB33;}),_tmp637->tl=0;_tmp637;}),0U);});
# 4042
struct Cyc_Absyn_Stmt*_tmp633=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB36=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpB36,({struct Cyc_List_List*_tmp636=_cycalloc(sizeof(*_tmp636));
({struct Cyc_Absyn_Exp*_tmpB35=Cyc_Absyn_address_exp(h_exp,0U);_tmp636->hd=_tmpB35;}),_tmp636->tl=0;_tmp636;}),0U);}),0U);
# 4046
struct Cyc_Absyn_Exp*_tmp634=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp635=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpB45=({
struct _tuple1*_tmpB44=h_var;void*_tmpB43=h_typ;Cyc_Absyn_declare_stmt(_tmpB44,_tmpB43,0,({
struct Cyc_Absyn_Stmt*_tmpB42=_tmp633;Cyc_Absyn_seq_stmt(_tmpB42,({
struct _tuple1*_tmpB41=was_thrown_var;void*_tmpB40=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpB3F=_tmp634;Cyc_Absyn_declare_stmt(_tmpB41,_tmpB40,_tmpB3F,({
struct Cyc_Absyn_Stmt*_tmpB3E=({struct Cyc_Absyn_Exp*_tmpB38=_tmp632;struct Cyc_Absyn_Stmt*_tmpB37=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp635,0U);
# 4052
Cyc_Absyn_ifthenelse_stmt(_tmpB38,_tmpB37,
# 4054
Cyc_Toc_skip_stmt_dl(),0U);});
# 4052
Cyc_Absyn_seq_stmt(_tmpB3E,({
# 4055
struct Cyc_Absyn_Exp*_tmpB3D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpB3C=_tmp630;Cyc_Absyn_ifthenelse_stmt(_tmpB3D,_tmpB3C,({
# 4057
struct _tuple1*_tmpB3B=e_var;void*_tmpB3A=e_typ;struct Cyc_Absyn_Exp*_tmpB39=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 4057
Cyc_Absyn_declare_stmt(_tmpB3B,_tmpB3A,_tmpB39,_tmp631,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 4048
s->r=_tmpB45;});};};
# 4061
_npop_handler(0U);return;
# 4026
;_pop_region(temp);};}}_LL0:;}}
# 4070
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpB46=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpB46;});{
struct _RegionHandle _tmp654=_new_region("frgn");struct _RegionHandle*frgn=& _tmp654;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp655=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp656=f->args;for(0;_tmp656 != 0;_tmp656=_tmp656->tl){
struct _tuple1*_tmp657=({struct _tuple1*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->f1=Cyc_Absyn_Loc_n,_tmp658->f2=(*((struct _tuple9*)_tmp656->hd)).f1;_tmp658;});
({void*_tmpB47=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp656->hd)).f3);(*((struct _tuple9*)_tmp656->hd)).f3=_tmpB47;});}}
# 4086
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 4090
({struct Cyc_Hashtable_Table**_tmpB49=({struct Cyc_Hashtable_Table**_tmp659=_cycalloc(sizeof(*_tmp659));({struct Cyc_Hashtable_Table*_tmpB48=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp659=_tmpB48;});_tmp659;});Cyc_Toc_fn_pop_table=_tmpB49;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp65A=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp65B=_tmp65A;struct _dyneither_ptr*_tmp664;struct Cyc_Absyn_Tqual _tmp663;void*_tmp662;int _tmp661;_LL1: _tmp664=_tmp65B.name;_tmp663=_tmp65B.tq;_tmp662=_tmp65B.type;_tmp661=_tmp65B.inject;_LL2:;{
void*_tmp65C=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp662,Cyc_Absyn_heap_rgn_type,_tmp663,Cyc_Absyn_false_type));
struct _tuple1*_tmp65D=({struct _tuple1*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->f1=Cyc_Absyn_Loc_n,_tmp660->f2=(struct _dyneither_ptr*)_check_null(_tmp664);_tmp660;});
({struct Cyc_List_List*_tmpB4C=({struct Cyc_List_List*_tmpB4B=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB4B,({struct Cyc_List_List*_tmp65F=_cycalloc(sizeof(*_tmp65F));({struct _tuple9*_tmpB4A=({struct _tuple9*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->f1=_tmp664,_tmp65E->f2=_tmp663,_tmp65E->f3=_tmp65C;_tmp65E;});_tmp65F->hd=_tmpB4A;}),_tmp65F->tl=0;_tmp65F;}));});f->args=_tmpB4C;});
f->cyc_varargs=0;};}
# 4099
{struct Cyc_List_List*_tmp665=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp665 != 0;_tmp665=_tmp665->tl){
({void*_tmpB4D=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp665->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp665->hd)->type=_tmpB4D;});}}
# 4102
({struct Cyc_Toc_Env*_tmpB4E=Cyc_Toc_clear_toplevel(frgn,_tmp655);Cyc_Toc_stmt_to_c(_tmpB4E,f->body);});}
# 4079
;_pop_region(frgn);};}
# 4106
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp666=s;switch(_tmp666){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple40{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple41{struct Cyc_Toc_TocState*f1;struct _tuple40*f2;};
# 4123 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4126
struct _tuple41 _tmp667=*env;struct _tuple41 _tmp668=_tmp667;struct Cyc_Toc_TocState*_tmp69D;struct Cyc_Absyn_Aggrdecl*_tmp69C;int _tmp69B;_LL1: _tmp69D=_tmp668.f1;_tmp69C=(_tmp668.f2)->f1;_tmp69B=(_tmp668.f2)->f2;_LL2:;{
struct _tuple1*_tmp669=_tmp69C->name;
struct Cyc_Toc_TocState _tmp66A=*_tmp69D;struct Cyc_Toc_TocState _tmp66B=_tmp66A;struct Cyc_Dict_Dict*_tmp69A;_LL4: _tmp69A=_tmp66B.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple19**_tmp66C=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp69A,_tmp669);
if(_tmp66C == 0){
seen_defn_before=0;{
struct _tuple19*v;
if(_tmp69C->kind == Cyc_Absyn_StructA)
({struct _tuple19*_tmpB50=({struct _tuple19*_tmp66D=_region_malloc(d,sizeof(*_tmp66D));_tmp66D->f1=_tmp69C,({void*_tmpB4F=Cyc_Absyn_strctq(_tmp669);_tmp66D->f2=_tmpB4F;});_tmp66D;});v=_tmpB50;});else{
# 4137
({struct _tuple19*_tmpB52=({struct _tuple19*_tmp66E=_region_malloc(d,sizeof(*_tmp66E));_tmp66E->f1=_tmp69C,({void*_tmpB51=Cyc_Absyn_unionq_type(_tmp669);_tmp66E->f2=_tmpB51;});_tmp66E;});v=_tmpB52;});}
({struct Cyc_Dict_Dict _tmpB53=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*_tmp69A,_tmp669,v);*_tmp69A=_tmpB53;});};}else{
# 4140
struct _tuple19*_tmp66F=*_tmp66C;struct _tuple19*_tmp670=_tmp66F;struct Cyc_Absyn_Aggrdecl*_tmp673;void*_tmp672;_LL7: _tmp673=_tmp670->f1;_tmp672=_tmp670->f2;_LL8:;
if(_tmp673->impl == 0){
({struct Cyc_Dict_Dict _tmpB56=({struct Cyc_Dict_Dict _tmpB55=*_tmp69A;struct _tuple1*_tmpB54=_tmp669;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpB55,_tmpB54,({struct _tuple19*_tmp671=_region_malloc(d,sizeof(*_tmp671));_tmp671->f1=_tmp69C,_tmp671->f2=_tmp672;_tmp671;}));});*_tmp69A=_tmpB56;});
seen_defn_before=0;}else{
# 4145
seen_defn_before=1;}}{
# 4147
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->kind=_tmp69C->kind,_tmp699->sc=Cyc_Absyn_Public,_tmp699->name=_tmp69C->name,_tmp699->tvs=0,_tmp699->impl=0,_tmp699->expected_mem_kind=0,_tmp699->attributes=_tmp69C->attributes;_tmp699;});
# 4154
if(_tmp69C->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpB57=({struct Cyc_Absyn_AggrdeclImpl*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->exist_vars=0,_tmp674->rgn_po=0,_tmp674->fields=0,_tmp674->tagged=0;_tmp674;});new_ad->impl=_tmpB57;});{
# 4159
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp675=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69C->impl))->fields;for(0;_tmp675 != 0;_tmp675=_tmp675->tl){
# 4163
struct Cyc_Absyn_Aggrfield*_tmp676=(struct Cyc_Absyn_Aggrfield*)_tmp675->hd;
void*_tmp677=_tmp676->type;
struct Cyc_List_List*_tmp678=_tmp676->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp677)) && (
_tmp69C->kind == Cyc_Absyn_StructA  && _tmp675->tl == 0  || _tmp69C->kind == Cyc_Absyn_UnionA)){
# 4177 "toc.cyc"
void*_tmp679=Cyc_Tcutil_compress(_tmp677);void*_tmp67A=_tmp679;void*_tmp682;struct Cyc_Absyn_Tqual _tmp681;void*_tmp680;unsigned int _tmp67F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->tag == 4U){_LLA: _tmp682=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->f1).elt_type;_tmp681=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->f1).tq;_tmp680=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->f1).zero_term;_tmp67F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->f1).zt_loc;_LLB:
# 4180
({void*_tmpB59=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->tag=4U,(_tmp67B->f1).elt_type=_tmp682,(_tmp67B->f1).tq=_tmp681,({struct Cyc_Absyn_Exp*_tmpB58=Cyc_Absyn_uint_exp(0U,0U);(_tmp67B->f1).num_elts=_tmpB58;}),(_tmp67B->f1).zero_term=_tmp680,(_tmp67B->f1).zt_loc=_tmp67F;_tmp67B;});_tmp677=_tmpB59;});
goto _LL9;}else{_LLC: _LLD:
# 4183
({struct Cyc_List_List*_tmpB5B=({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));({void*_tmpB5A=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->tag=6U,_tmp67C->f1=0;_tmp67C;});_tmp67D->hd=_tmpB5A;}),_tmp67D->tl=_tmp678;_tmp67D;});_tmp678=_tmpB5B;});
({void*_tmpB5F=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->tag=4U,({void*_tmpB5E=Cyc_Absyn_void_star_type();(_tmp67E->f1).elt_type=_tmpB5E;}),({
struct Cyc_Absyn_Tqual _tmpB5D=Cyc_Absyn_empty_tqual(0U);(_tmp67E->f1).tq=_tmpB5D;}),({
struct Cyc_Absyn_Exp*_tmpB5C=Cyc_Absyn_uint_exp(0U,0U);(_tmp67E->f1).num_elts=_tmpB5C;}),(_tmp67E->f1).zero_term=Cyc_Absyn_false_type,(_tmp67E->f1).zt_loc=0U;_tmp67E;});
# 4184
_tmp677=_tmpB5F;});}_LL9:;}{
# 4190
struct Cyc_Absyn_Aggrfield*_tmp683=({struct Cyc_Absyn_Aggrfield*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->name=_tmp676->name,_tmp695->tq=Cyc_Toc_mt_tq,({
# 4192
void*_tmpB60=Cyc_Toc_typ_to_c(_tmp677);_tmp695->type=_tmpB60;}),_tmp695->width=_tmp676->width,_tmp695->attributes=_tmp678,_tmp695->requires_clause=0;_tmp695;});
# 4200
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69C->impl))->tagged){
void*_tmp684=_tmp683->type;
struct _dyneither_ptr*_tmp685=_tmp683->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp692=({struct Cyc_String_pa_PrintArg_struct _tmp7BF;_tmp7BF.tag=0U,_tmp7BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp69C->name).f2);_tmp7BF;});struct Cyc_String_pa_PrintArg_struct _tmp693=({struct Cyc_String_pa_PrintArg_struct _tmp7BE;_tmp7BE.tag=0U,_tmp7BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp685);_tmp7BE;});void*_tmp690[2U];_tmp690[0]=& _tmp692,_tmp690[1]=& _tmp693;({struct _dyneither_ptr _tmpB61=({const char*_tmp691="_union_%s_%s";_tag_dyneither(_tmp691,sizeof(char),13U);});Cyc_aprintf(_tmpB61,_tag_dyneither(_tmp690,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp68F=_cycalloc(sizeof(*_tmp68F));*_tmp68F=s;_tmp68F;});
struct Cyc_Absyn_Aggrfield*_tmp686=({struct Cyc_Absyn_Aggrfield*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->name=Cyc_Toc_val_sp,_tmp68E->tq=Cyc_Toc_mt_tq,_tmp68E->type=_tmp684,_tmp68E->width=0,_tmp68E->attributes=0,_tmp68E->requires_clause=0;_tmp68E;});
struct Cyc_Absyn_Aggrfield*_tmp687=({struct Cyc_Absyn_Aggrfield*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D->name=Cyc_Toc_tag_sp,_tmp68D->tq=Cyc_Toc_mt_tq,_tmp68D->type=Cyc_Absyn_sint_type,_tmp68D->width=0,_tmp68D->attributes=0,_tmp68D->requires_clause=0;_tmp68D;});
struct Cyc_List_List*_tmp688=({struct Cyc_Absyn_Aggrfield*_tmp68C[2U];_tmp68C[0]=_tmp687,_tmp68C[1]=_tmp686;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp68C,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp689=Cyc_Toc_make_c_struct_defn(str,_tmp688);
({struct Cyc_List_List*_tmpB63=({struct Cyc_List_List*_tmp68B=_cycalloc(sizeof(*_tmp68B));({struct Cyc_Absyn_Decl*_tmpB62=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->tag=5U,_tmp68A->f1=_tmp689;_tmp68A;}),0U);_tmp68B->hd=_tmpB62;}),_tmp68B->tl=Cyc_Toc_result_decls;_tmp68B;});Cyc_Toc_result_decls=_tmpB63;});
({void*_tmpB64=Cyc_Absyn_strct(str);_tmp683->type=_tmpB64;});}
# 4212
({struct Cyc_List_List*_tmpB65=({struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->hd=_tmp683,_tmp694->tl=new_fields;_tmp694;});new_fields=_tmpB65;});};}}
# 4214
({struct Cyc_List_List*_tmpB66=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpB66;});};}
# 4216
if(_tmp69B)
({struct Cyc_List_List*_tmpB69=({struct Cyc_List_List*_tmp698=_cycalloc(sizeof(*_tmp698));({struct Cyc_Absyn_Decl*_tmpB68=({struct Cyc_Absyn_Decl*_tmp697=_cycalloc(sizeof(*_tmp697));({void*_tmpB67=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->tag=5U,_tmp696->f1=new_ad;_tmp696;});_tmp697->r=_tmpB67;}),_tmp697->loc=0U;_tmp697;});_tmp698->hd=_tmpB68;}),_tmp698->tl=Cyc_Toc_result_decls;_tmp698;});Cyc_Toc_result_decls=_tmpB69;});
return 0;};};};}
# 4221
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple40 p=({struct _tuple40 _tmp7C0;_tmp7C0.f1=ad,_tmp7C0.f2=add_to_decls;_tmp7C0;});
((int(*)(struct _tuple40*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple42{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4250 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 4253
struct _tuple42 _tmp69E=*env;struct _tuple42 _tmp69F=_tmp69E;struct Cyc_Set_Set**_tmp6B0;struct Cyc_Absyn_Datatypedecl*_tmp6AF;_LL1: _tmp6B0=(_tmp69F.f1)->datatypes_so_far;_tmp6AF=_tmp69F.f2;_LL2:;{
struct _tuple1*_tmp6A0=_tmp6AF->name;
if(_tmp6AF->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp6B0,_tmp6A0))
return 0;
({struct Cyc_Set_Set*_tmpB6A=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp6B0,_tmp6A0);*_tmp6B0=_tmpB6A;});
{struct Cyc_List_List*_tmp6A1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6AF->fields))->v;for(0;_tmp6A1 != 0;_tmp6A1=_tmp6A1->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp6A1->hd;
# 4261
struct Cyc_List_List*_tmp6A2=0;
int i=1;
{struct Cyc_List_List*_tmp6A3=f->typs;for(0;_tmp6A3 != 0;(_tmp6A3=_tmp6A3->tl,i ++)){
struct _dyneither_ptr*_tmp6A4=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp6A5=({struct Cyc_Absyn_Aggrfield*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->name=_tmp6A4,_tmp6A7->tq=(*((struct _tuple12*)_tmp6A3->hd)).f1,({
void*_tmpB6B=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp6A3->hd)).f2);_tmp6A7->type=_tmpB6B;}),_tmp6A7->width=0,_tmp6A7->attributes=0,_tmp6A7->requires_clause=0;_tmp6A7;});
({struct Cyc_List_List*_tmpB6C=({struct Cyc_List_List*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->hd=_tmp6A5,_tmp6A6->tl=_tmp6A2;_tmp6A6;});_tmp6A2=_tmpB6C;});}}
# 4269
({struct Cyc_List_List*_tmpB6F=({struct Cyc_List_List*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));({struct Cyc_Absyn_Aggrfield*_tmpB6E=({struct Cyc_Absyn_Aggrfield*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8->name=Cyc_Toc_tag_sp,_tmp6A8->tq=Cyc_Toc_mt_tq,_tmp6A8->type=Cyc_Absyn_sint_type,_tmp6A8->width=0,_tmp6A8->attributes=0,_tmp6A8->requires_clause=0;_tmp6A8;});_tmp6A9->hd=_tmpB6E;}),({
struct Cyc_List_List*_tmpB6D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6A2);_tmp6A9->tl=_tmpB6D;});_tmp6A9;});
# 4269
_tmp6A2=_tmpB6F;});{
# 4271
struct Cyc_Absyn_Aggrdecl*_tmp6AA=({struct _dyneither_ptr*_tmpB71=({struct _dyneither_ptr*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));({struct _dyneither_ptr _tmpB70=({const char*_tmp6AD="";_tag_dyneither(_tmp6AD,sizeof(char),1U);});*_tmp6AE=_tmpB70;});_tmp6AE;});Cyc_Toc_make_c_struct_defn(_tmpB71,_tmp6A2);});
({struct _tuple1*_tmpB72=Cyc_Toc_collapse_qvars(f->name,_tmp6AF->name);_tmp6AA->name=_tmpB72;});
({struct Cyc_List_List*_tmpB74=({struct Cyc_List_List*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));({struct Cyc_Absyn_Decl*_tmpB73=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB->tag=5U,_tmp6AB->f1=_tmp6AA;_tmp6AB;}),0U);_tmp6AC->hd=_tmpB73;}),_tmp6AC->tl=Cyc_Toc_result_decls;_tmp6AC;});Cyc_Toc_result_decls=_tmpB74;});};}}
# 4275
return 0;};}
# 4278
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4297 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 4300
struct _tuple42 _tmp6B1=*env;struct _tuple42 _tmp6B2=_tmp6B1;struct Cyc_Toc_TocState*_tmp6D4;struct Cyc_Absyn_Datatypedecl*_tmp6D3;_LL1: _tmp6D4=_tmp6B2.f1;_tmp6D3=_tmp6B2.f2;_LL2:;
if(_tmp6D3->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6B3=*_tmp6D4;struct Cyc_Toc_TocState _tmp6B4=_tmp6B3;struct Cyc_Dict_Dict*_tmp6D2;_LL4: _tmp6D2=_tmp6B4.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp6B5=_tmp6D3->name;
{struct Cyc_List_List*_tmp6B6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6D3->fields))->v;for(0;_tmp6B6 != 0;_tmp6B6=_tmp6B6->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp6B6->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6B7=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp6B8=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp6B7,Cyc_Absyn_false_type,0U);
# 4311
int*_tmp6B9=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp6D2,f->name);int*_tmp6BA=_tmp6B9;if(_tmp6BA == 0){_LL7: _LL8: {
# 4313
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpB75=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpB75;});{
# 4317
struct Cyc_Absyn_Vardecl*_tmp6BB=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B8,initopt);
_tmp6BB->sc=f->sc;
({struct Cyc_List_List*_tmpB77=({struct Cyc_List_List*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));({struct Cyc_Absyn_Decl*_tmpB76=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->tag=0U,_tmp6BC->f1=_tmp6BB;_tmp6BC;}),0U);_tmp6BD->hd=_tmpB76;}),_tmp6BD->tl=Cyc_Toc_result_decls;_tmp6BD;});Cyc_Toc_result_decls=_tmpB77;});
({struct Cyc_Dict_Dict _tmpB78=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6D2,f->name,f->sc != Cyc_Absyn_Extern);
# 4320
*_tmp6D2=_tmpB78;});{
# 4322
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6BE=f->typs;for(0;_tmp6BE != 0;(_tmp6BE=_tmp6BE->tl,i ++)){
struct _dyneither_ptr*_tmp6BF=({struct _dyneither_ptr*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));({struct _dyneither_ptr _tmpB7A=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6C5=({struct Cyc_Int_pa_PrintArg_struct _tmp7C1;_tmp7C1.tag=1U,_tmp7C1.f1=(unsigned long)i;_tmp7C1;});void*_tmp6C3[1U];_tmp6C3[0]=& _tmp6C5;({struct _dyneither_ptr _tmpB79=({const char*_tmp6C4="f%d";_tag_dyneither(_tmp6C4,sizeof(char),4U);});Cyc_aprintf(_tmpB79,_tag_dyneither(_tmp6C3,sizeof(void*),1U));});});*_tmp6C6=_tmpB7A;});_tmp6C6;});
struct Cyc_Absyn_Aggrfield*_tmp6C0=({struct Cyc_Absyn_Aggrfield*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->name=_tmp6BF,_tmp6C2->tq=(*((struct _tuple12*)_tmp6BE->hd)).f1,({
void*_tmpB7B=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp6BE->hd)).f2);_tmp6C2->type=_tmpB7B;}),_tmp6C2->width=0,_tmp6C2->attributes=0,_tmp6C2->requires_clause=0;_tmp6C2;});
({struct Cyc_List_List*_tmpB7C=({struct Cyc_List_List*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->hd=_tmp6C0,_tmp6C1->tl=fields;_tmp6C1;});fields=_tmpB7C;});}}
# 4330
({struct Cyc_List_List*_tmpB80=({struct Cyc_List_List*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));({struct Cyc_Absyn_Aggrfield*_tmpB7F=({struct Cyc_Absyn_Aggrfield*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->name=Cyc_Toc_tag_sp,_tmp6C7->tq=Cyc_Toc_mt_tq,({
void*_tmpB7E=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp6C7->type=_tmpB7E;}),_tmp6C7->width=0,_tmp6C7->attributes=0,_tmp6C7->requires_clause=0;_tmp6C7;});
# 4330
_tmp6C8->hd=_tmpB7F;}),({
# 4332
struct Cyc_List_List*_tmpB7D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6C8->tl=_tmpB7D;});_tmp6C8;});
# 4330
fields=_tmpB80;});{
# 4333
struct Cyc_Absyn_Aggrdecl*_tmp6C9=({struct _dyneither_ptr*_tmpB82=({struct _dyneither_ptr*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));({struct _dyneither_ptr _tmpB81=({const char*_tmp6CC="";_tag_dyneither(_tmp6CC,sizeof(char),1U);});*_tmp6CD=_tmpB81;});_tmp6CD;});Cyc_Toc_make_c_struct_defn(_tmpB82,fields);});
({struct _tuple1*_tmpB83=Cyc_Toc_collapse_qvars(f->name,_tmp6D3->name);_tmp6C9->name=_tmpB83;});
({struct Cyc_List_List*_tmpB85=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));({struct Cyc_Absyn_Decl*_tmpB84=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->tag=5U,_tmp6CA->f1=_tmp6C9;_tmp6CA;}),0U);_tmp6CB->hd=_tmpB84;}),_tmp6CB->tl=Cyc_Toc_result_decls;_tmp6CB;});Cyc_Toc_result_decls=_tmpB85;});
goto _LL6;};};};}}else{if(*((int*)_tmp6BA)== 0){_LL9: _LLA:
# 4338
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6CE=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp6CF=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B8,_tmp6CE);
_tmp6CF->sc=f->sc;
({struct Cyc_List_List*_tmpB87=({struct Cyc_List_List*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));({struct Cyc_Absyn_Decl*_tmpB86=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->tag=0U,_tmp6D0->f1=_tmp6CF;_tmp6D0;}),0U);_tmp6D1->hd=_tmpB86;}),_tmp6D1->tl=Cyc_Toc_result_decls;_tmp6D1;});Cyc_Toc_result_decls=_tmpB87;});
({struct Cyc_Dict_Dict _tmpB88=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6D2,f->name,1);*_tmp6D2=_tmpB88;});}
# 4345
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4349
return 0;};};}
# 4352
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4356
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4362
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpB89=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpB89;});
# 4366
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4375
struct _RegionHandle _tmp6D5=_new_region("temp");struct _RegionHandle*temp=& _tmp6D5;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6D6=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp6D6,init);}
# 4376
;_pop_region(temp);}else{
# 4380
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4383
void*_tmp6D7=Cyc_Tcutil_compress(old_typ);void*_tmp6D8=_tmp6D7;void*_tmp6DF;struct Cyc_Absyn_Exp*_tmp6DE;void*_tmp6DD;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->tag == 4U){_LL1: _tmp6DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->f1).elt_type;_tmp6DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->f1).num_elts;_tmp6DD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->f1).zero_term;_LL2:
# 4385
 if(Cyc_Tcutil_force_type2bool(0,_tmp6DD)){
if(_tmp6DE == 0)
({void*_tmp6D9=0U;({struct _dyneither_ptr _tmpB8A=({const char*_tmp6DA="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp6DA,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB8A,_tag_dyneither(_tmp6D9,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp6DE;
struct Cyc_Absyn_Exp*_tmp6DB=({struct Cyc_Absyn_Exp*_tmpB8C=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpB8C,({
struct Cyc_Absyn_Exp*_tmpB8B=num_elts;Cyc_Absyn_add_exp(_tmpB8B,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4392
struct Cyc_Absyn_Exp*_tmp6DC=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpB8E=({struct Cyc_Absyn_Stmt*_tmpB8D=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp6DB,_tmp6DC,0U),0U);Cyc_Toc_seq_stmt_r(_tmpB8D,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4393
s->r=_tmpB8E;});};}
# 4396
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4404
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6E0=d;struct Cyc_List_List*_tmp6E7;struct Cyc_List_List*_tmp6E6;void**_tmp6E5;struct Cyc_Tcpat_Rhs*_tmp6E4;switch(*((int*)_tmp6E0)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6E4=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6E0)->f1;_LL4:
 _tmp6E4->rhs=success;return d;default: _LL5: _tmp6E7=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E0)->f1;_tmp6E6=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E0)->f2;_tmp6E5=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E0)->f3;_LL6:
# 4409
({void*_tmpB8F=Cyc_Toc_rewrite_decision(*_tmp6E5,success);*_tmp6E5=_tmpB8F;});
for(0;_tmp6E6 != 0;_tmp6E6=_tmp6E6->tl){
struct _tuple33*_tmp6E1=(struct _tuple33*)_tmp6E6->hd;struct _tuple33*_tmp6E2=_tmp6E1;void**_tmp6E3;_LL8: _tmp6E3=(void**)& _tmp6E2->f2;_LL9:;
({void*_tmpB90=Cyc_Toc_rewrite_decision(*_tmp6E3,success);*_tmp6E3=_tmpB90;});}
# 4414
return d;}_LL0:;}
# 4425 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4427
struct _RegionHandle _tmp6E8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6E8;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6E9=Cyc_Toc_share_env(rgn,nv);
void*_tmp6EA=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp6E9,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4437
struct Cyc_Absyn_Stmt*_tmp6EB=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4441
({void*_tmpB91=Cyc_Toc_rewrite_decision(dopt,_tmp6EB);dopt=_tmpB91;});{
# 4443
struct Cyc_Absyn_Switch_clause*_tmp6EC=({struct Cyc_Absyn_Switch_clause*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8->pattern=p,_tmp6F8->pat_vars=0,_tmp6F8->where_clause=0,_tmp6F8->body=_tmp6EB,_tmp6F8->loc=0U;_tmp6F8;});
struct Cyc_List_List*_tmp6ED=({struct _RegionHandle*_tmpB94=rgn;struct _RegionHandle*_tmpB93=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple36*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpB94,Cyc_Toc_gen_labels,_tmpB93,({struct Cyc_Absyn_Switch_clause*_tmp6F7[1U];_tmp6F7[0]=_tmp6EC;({struct _RegionHandle*_tmpB92=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpB92,_tag_dyneither(_tmp6F7,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4446
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4450
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp6E9,& mydecls,& mybodies,dopt,_tmp6ED,v);
# 4453
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple39*_tmp6EE=(struct _tuple39*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple39*_tmp6EF=_tmp6EE;struct Cyc_Toc_Env*_tmp6F1;struct Cyc_Absyn_Stmt*_tmp6F0;_LL1: _tmp6F1=_tmp6EF->f1;_tmp6F0=_tmp6EF->f3;_LL2:;
if(_tmp6F0 == _tmp6EB){senv=_tmp6F1;goto FOUND_ENV;}}
# 4458
({void*_tmp6F2=0U;({struct _dyneither_ptr _tmpB95=({const char*_tmp6F3="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp6F3,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB95,_tag_dyneither(_tmp6F2,sizeof(void*),0U));});});
FOUND_ENV:
# 4462
 Cyc_Toc_stmt_to_c(senv,s);{
# 4464
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4466
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp6F4=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpB96=Cyc_Toc_temp_var();_tmp6F4->name=_tmpB96;});
({void*_tmpB97=Cyc_Toc_typ_to_c_array(_tmp6F4->type);_tmp6F4->type=_tmpB97;});
({struct Cyc_Absyn_Stmt*_tmpB99=({struct Cyc_Absyn_Decl*_tmpB98=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5->tag=0U,_tmp6F5->f1=_tmp6F4;_tmp6F5;}),0U);Cyc_Absyn_decl_stmt(_tmpB98,res,0U);});res=_tmpB99;});}
# 4473
({struct Cyc_Absyn_Stmt*_tmpB9D=({struct _tuple1*_tmpB9C=v;void*_tmpB9B=Cyc_Toc_typ_to_c(_tmp6EA);struct Cyc_Absyn_Exp*_tmpB9A=e;Cyc_Absyn_declare_stmt(_tmpB9C,_tmpB9B,_tmpB9A,res,0U);});res=_tmpB9D;});{
struct Cyc_Absyn_Stmt*_tmp6F6=res;_npop_handler(0U);return _tmp6F6;};};};};}
# 4428
;_pop_region(rgn);}
# 4481
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6F9=e->r;void*_tmp6FA=_tmp6F9;struct Cyc_Absyn_MallocInfo*_tmp725;struct Cyc_Absyn_Stmt*_tmp724;void**_tmp723;void**_tmp722;struct Cyc_List_List*_tmp721;struct Cyc_List_List*_tmp720;void**_tmp71F;struct Cyc_List_List*_tmp71E;void**_tmp71D;struct Cyc_Absyn_Exp*_tmp71C;struct Cyc_Absyn_Exp*_tmp71B;struct Cyc_List_List*_tmp71A;struct Cyc_Absyn_Exp*_tmp719;struct Cyc_Absyn_Exp*_tmp718;struct Cyc_Absyn_Exp*_tmp717;struct Cyc_Absyn_Exp*_tmp716;struct Cyc_Absyn_Exp*_tmp715;struct Cyc_Absyn_Exp*_tmp714;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_List_List*_tmp70A;struct Cyc_Absyn_Exp*_tmp709;struct Cyc_Absyn_Exp*_tmp708;struct Cyc_Absyn_Exp*_tmp707;struct Cyc_Absyn_Exp*_tmp706;struct Cyc_Absyn_Exp*_tmp705;struct Cyc_Absyn_Exp*_tmp704;struct Cyc_Absyn_Exp*_tmp703;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_Absyn_Exp*_tmp701;switch(*((int*)_tmp6FA)){case 41U: _LL1: _tmp701=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL2:
 _tmp702=_tmp701;goto _LL4;case 20U: _LL3: _tmp702=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL4:
 _tmp703=_tmp702;goto _LL6;case 21U: _LL5: _tmp703=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL6:
 _tmp704=_tmp703;goto _LL8;case 22U: _LL7: _tmp704=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL8:
 _tmp705=_tmp704;goto _LLA;case 15U: _LL9: _tmp705=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LLA:
 _tmp706=_tmp705;goto _LLC;case 11U: _LLB: _tmp706=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LLC:
 _tmp707=_tmp706;goto _LLE;case 12U: _LLD: _tmp707=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LLE:
 _tmp708=_tmp707;goto _LL10;case 18U: _LLF: _tmp708=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL10:
 _tmp709=_tmp708;goto _LL12;case 5U: _LL11: _tmp709=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp709);goto _LL0;case 3U: _LL13: _tmp70A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp70A);goto _LL0;case 7U: _LL15: _tmp70C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp70B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL16:
 _tmp70E=_tmp70C;_tmp70D=_tmp70B;goto _LL18;case 8U: _LL17: _tmp70E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp70D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL18:
 _tmp710=_tmp70E;_tmp70F=_tmp70D;goto _LL1A;case 9U: _LL19: _tmp710=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp70F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL1A:
 _tmp712=_tmp710;_tmp711=_tmp70F;goto _LL1C;case 23U: _LL1B: _tmp712=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp711=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL1C:
 _tmp714=_tmp712;_tmp713=_tmp711;goto _LL1E;case 35U: _LL1D: _tmp714=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp713=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL1E:
 _tmp716=_tmp714;_tmp715=_tmp713;goto _LL20;case 4U: _LL1F: _tmp716=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp715=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp716);Cyc_Toc_exptypes_to_c(_tmp715);goto _LL0;case 6U: _LL21: _tmp719=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp718=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_tmp717=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FA)->f3;_LL22:
# 4500
 Cyc_Toc_exptypes_to_c(_tmp719);Cyc_Toc_exptypes_to_c(_tmp718);Cyc_Toc_exptypes_to_c(_tmp717);goto _LL0;case 10U: _LL23: _tmp71B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp71A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL24:
# 4502
 Cyc_Toc_exptypes_to_c(_tmp71B);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp71A);goto _LL0;case 14U: _LL25: _tmp71D=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp71C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL26:
({void*_tmpB9E=Cyc_Toc_typ_to_c(*_tmp71D);*_tmp71D=_tmpB9E;});Cyc_Toc_exptypes_to_c(_tmp71C);goto _LL0;case 25U: _LL27: _tmp71F=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1)->f3;_tmp71E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL28:
# 4505
({void*_tmpB9F=Cyc_Toc_typ_to_c(*_tmp71F);*_tmp71F=_tmpB9F;});
_tmp720=_tmp71E;goto _LL2A;case 36U: _LL29: _tmp720=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL2A:
 _tmp721=_tmp720;goto _LL2C;case 26U: _LL2B: _tmp721=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL2C:
# 4509
 for(0;_tmp721 != 0;_tmp721=_tmp721->tl){
struct _tuple20 _tmp6FB=*((struct _tuple20*)_tmp721->hd);struct _tuple20 _tmp6FC=_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FD;_LL56: _tmp6FD=_tmp6FC.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp6FD);}
# 4513
goto _LL0;case 19U: _LL2D: _tmp722=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL2E:
 _tmp723=_tmp722;goto _LL30;case 17U: _LL2F: _tmp723=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL30:
({void*_tmpBA0=Cyc_Toc_typ_to_c(*_tmp723);*_tmp723=_tmpBA0;});goto _LL0;case 37U: _LL31: _tmp724=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp724);goto _LL0;case 34U: _LL33: _tmp725=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL34:
# 4518
 if(_tmp725->elt_type != 0)
({void**_tmpBA2=({void**_tmp6FE=_cycalloc(sizeof(*_tmp6FE));({void*_tmpBA1=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp725->elt_type)));*_tmp6FE=_tmpBA1;});_tmp6FE;});_tmp725->elt_type=_tmpBA2;});
Cyc_Toc_exptypes_to_c(_tmp725->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4528
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
({void*_tmp6FF=0U;({unsigned int _tmpBA4=e->loc;struct _dyneither_ptr _tmpBA3=({const char*_tmp700="Cyclone expression within C code";_tag_dyneither(_tmp700,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpBA4,_tmpBA3,_tag_dyneither(_tmp6FF,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4542
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp726=d->r;void*_tmp727=_tmp726;struct Cyc_Absyn_Typedefdecl*_tmp731;struct Cyc_Absyn_Enumdecl*_tmp730;struct Cyc_Absyn_Aggrdecl*_tmp72F;struct Cyc_Absyn_Fndecl*_tmp72E;struct Cyc_Absyn_Vardecl*_tmp72D;switch(*((int*)_tmp727)){case 0U: _LL1: _tmp72D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LL2:
# 4545
({void*_tmpBA5=Cyc_Toc_typ_to_c(_tmp72D->type);_tmp72D->type=_tmpBA5;});
if(_tmp72D->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp72D->initializer));
goto _LL0;case 1U: _LL3: _tmp72E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LL4:
# 4549
({void*_tmpBA6=Cyc_Toc_typ_to_c(_tmp72E->ret_type);_tmp72E->ret_type=_tmpBA6;});
{struct Cyc_List_List*_tmp728=_tmp72E->args;for(0;_tmp728 != 0;_tmp728=_tmp728->tl){
({void*_tmpBA7=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp728->hd)).f3);(*((struct _tuple9*)_tmp728->hd)).f3=_tmpBA7;});}}
# 4553
goto _LL0;case 5U: _LL5: _tmp72F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp72F,1);goto _LL0;case 7U: _LL7: _tmp730=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LL8:
# 4556
 if(_tmp730->fields != 0){
struct Cyc_List_List*_tmp729=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp730->fields))->v;for(0;_tmp729 != 0;_tmp729=_tmp729->tl){
struct Cyc_Absyn_Enumfield*_tmp72A=(struct Cyc_Absyn_Enumfield*)_tmp729->hd;
if(_tmp72A->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp72A->tag));}}
# 4561
goto _LL0;case 8U: _LL9: _tmp731=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LLA:
({void*_tmpBA8=Cyc_Toc_typ_to_c((void*)_check_null(_tmp731->defn));_tmp731->defn=_tmpBA8;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4571
({void*_tmp72B=0U;({unsigned int _tmpBAA=d->loc;struct _dyneither_ptr _tmpBA9=({const char*_tmp72C="Cyclone declaration within C code";_tag_dyneither(_tmp72C,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpBAA,_tmpBA9,_tag_dyneither(_tmp72B,sizeof(void*),0U));});});
goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 4575
 goto _LL0;}_LL0:;}
# 4579
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp732=s->r;void*_tmp733=_tmp732;struct Cyc_Absyn_Stmt*_tmp74D;struct Cyc_Absyn_Stmt*_tmp74C;struct Cyc_Absyn_Exp*_tmp74B;struct Cyc_Absyn_Decl*_tmp74A;struct Cyc_Absyn_Stmt*_tmp749;struct Cyc_Absyn_Exp*_tmp748;struct Cyc_List_List*_tmp747;void*_tmp746;struct Cyc_Absyn_Exp*_tmp745;struct Cyc_Absyn_Exp*_tmp744;struct Cyc_Absyn_Exp*_tmp743;struct Cyc_Absyn_Stmt*_tmp742;struct Cyc_Absyn_Exp*_tmp741;struct Cyc_Absyn_Stmt*_tmp740;struct Cyc_Absyn_Exp*_tmp73F;struct Cyc_Absyn_Stmt*_tmp73E;struct Cyc_Absyn_Stmt*_tmp73D;struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Stmt*_tmp73B;struct Cyc_Absyn_Stmt*_tmp73A;struct Cyc_Absyn_Exp*_tmp739;switch(*((int*)_tmp733)){case 1U: _LL1: _tmp739=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp739);goto _LL0;case 2U: _LL3: _tmp73B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp73A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp73B);Cyc_Toc_stmttypes_to_c(_tmp73A);goto _LL0;case 3U: _LL5: _tmp73C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_LL6:
 if(_tmp73C != 0)Cyc_Toc_exptypes_to_c(_tmp73C);goto _LL0;case 4U: _LL7: _tmp73F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp73E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_tmp73D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp733)->f3;_LL8:
# 4585
 Cyc_Toc_exptypes_to_c(_tmp73F);Cyc_Toc_stmttypes_to_c(_tmp73E);Cyc_Toc_stmttypes_to_c(_tmp73D);goto _LL0;case 5U: _LL9: _tmp741=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp733)->f1).f1;_tmp740=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_LLA:
# 4587
 Cyc_Toc_exptypes_to_c(_tmp741);Cyc_Toc_stmttypes_to_c(_tmp740);goto _LL0;case 9U: _LLB: _tmp745=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp744=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp733)->f2).f1;_tmp743=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp733)->f3).f1;_tmp742=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp733)->f4;_LLC:
# 4589
 Cyc_Toc_exptypes_to_c(_tmp745);Cyc_Toc_exptypes_to_c(_tmp744);Cyc_Toc_exptypes_to_c(_tmp743);
Cyc_Toc_stmttypes_to_c(_tmp742);goto _LL0;case 10U: _LLD: _tmp748=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp747=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_tmp746=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp733)->f3;_LLE:
# 4592
 Cyc_Toc_exptypes_to_c(_tmp748);
for(0;_tmp747 != 0;_tmp747=_tmp747->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp747->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp74A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp749=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp74A);Cyc_Toc_stmttypes_to_c(_tmp749);goto _LL0;case 14U: _LL11: _tmp74C=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp74B=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp733)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp74C);Cyc_Toc_exptypes_to_c(_tmp74B);goto _LL0;case 13U: _LL13: _tmp74D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp74D);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4602
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4605
({void*_tmpBAB=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734->tag=0U;_tmp734;});s->r=_tmpBAB;});
goto _LL0;default: _LL1F: _LL20:
# 4608
({void*_tmp735=0U;({unsigned int _tmpBAF=s->loc;struct _dyneither_ptr _tmpBAE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp738=({struct Cyc_String_pa_PrintArg_struct _tmp7C2;_tmp7C2.tag=0U,({struct _dyneither_ptr _tmpBAC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp7C2.f1=_tmpBAC;});_tmp7C2;});void*_tmp736[1U];_tmp736[0]=& _tmp738;({struct _dyneither_ptr _tmpBAD=({const char*_tmp737="Cyclone statement in C code: %s";_tag_dyneither(_tmp737,sizeof(char),32U);});Cyc_aprintf(_tmpBAD,_tag_dyneither(_tmp736,sizeof(void*),1U));});});Cyc_Tcutil_terr(_tmpBAF,_tmpBAE,_tag_dyneither(_tmp735,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 4613
static void*Cyc_Toc_decl_to_enumtype(struct Cyc_Absyn_Decl*d){
void*_tmp74E=d->r;void*_tmp74F=_tmp74E;struct Cyc_Absyn_Enumdecl*_tmp754;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp74F)->tag == 7U){_LL1: _tmp754=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp74F)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp751=_cycalloc(sizeof(*_tmp751));_tmp751->tag=0U,({void*_tmpBB0=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750->tag=15U,_tmp750->f1=_tmp754->name,_tmp750->f2=_tmp754;_tmp750;});_tmp751->f1=_tmpBB0;}),_tmp751->f2=0;_tmp751;});}else{_LL3: _LL4:
({void*_tmp752=0U;({struct _dyneither_ptr _tmpBB1=({const char*_tmp753="Toc::get_enumdecl_name";_tag_dyneither(_tmp753,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBB1,_tag_dyneither(_tmp752,sizeof(void*),0U));});});}_LL0:;}
# 4620
static struct Cyc_Absyn_Decl*Cyc_Toc_type2enumdecl(void*t){
void*_tmp755=t;struct Cyc_Absyn_Enumdecl*_tmp759;unsigned int _tmp758;if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp755)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp755)->f1)->r)->tag == 1U){_LL1: _tmp759=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp755)->f1)->r)->f1;_tmp758=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp755)->f1)->loc;_LL2:
# 4623
 return({struct Cyc_Absyn_Decl*_tmp757=_cycalloc(sizeof(*_tmp757));({void*_tmpBB2=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756->tag=7U,_tmp756->f1=_tmp759;_tmp756;});_tmp757->r=_tmpBB2;}),_tmp757->loc=_tmp758;_tmp757;});}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 4631
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp75A=0U;({struct _dyneither_ptr _tmpBB3=({const char*_tmp75B="decls_to_c: not at toplevel!";_tag_dyneither(_tmp75B,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBB3,_tag_dyneither(_tmp75A,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp75C=d->r;void*_tmp75D=_tmp75C;struct Cyc_List_List*_tmp778;struct Cyc_List_List*_tmp777;struct Cyc_List_List*_tmp776;struct Cyc_List_List*_tmp775;struct Cyc_Absyn_Typedefdecl*_tmp774;struct Cyc_Absyn_Enumdecl*_tmp773;struct Cyc_Absyn_Datatypedecl*_tmp772;struct Cyc_Absyn_Aggrdecl*_tmp771;struct Cyc_Absyn_Fndecl*_tmp770;struct Cyc_Absyn_Vardecl*_tmp76F;switch(*((int*)_tmp75D)){case 0U: _LL1: _tmp76F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp75D)->f1;_LL2: {
# 4639
struct _tuple1*_tmp75E=_tmp76F->name;
# 4641
if(_tmp76F->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpBB5=({struct _tuple1*_tmp75F=_cycalloc(sizeof(*_tmp75F));({union Cyc_Absyn_Nmspace _tmpBB4=Cyc_Absyn_Abs_n(0,1);_tmp75F->f1=_tmpBB4;}),_tmp75F->f2=(*_tmp75E).f2;_tmp75F;});_tmp75E=_tmpBB5;});
if(_tmp76F->initializer != 0){
if(_tmp76F->sc == Cyc_Absyn_ExternC)_tmp76F->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp76F->initializer));else{
# 4648
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp76F->initializer));}}
# 4650
_tmp76F->name=_tmp75E;
({enum Cyc_Absyn_Scope _tmpBB6=Cyc_Toc_scope_to_c(_tmp76F->sc);_tmp76F->sc=_tmpBB6;});
({void*_tmpBB7=Cyc_Toc_typ_to_c(_tmp76F->type);_tmp76F->type=_tmpBB7;});
({struct Cyc_List_List*_tmpBB8=({struct Cyc_List_List*_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760->hd=d,_tmp760->tl=Cyc_Toc_result_decls;_tmp760;});Cyc_Toc_result_decls=_tmpBB8;});
goto _LL0;}case 1U: _LL3: _tmp770=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp75D)->f1;_LL4: {
# 4656
struct _tuple1*_tmp761=_tmp770->name;
# 4658
if(_tmp770->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpBBA=({struct _tuple1*_tmp762=_cycalloc(sizeof(*_tmp762));({union Cyc_Absyn_Nmspace _tmpBB9=Cyc_Absyn_Abs_n(0,1);_tmp762->f1=_tmpBB9;}),_tmp762->f2=(*_tmp761).f2;_tmp762;});_tmp761=_tmpBBA;});
_tmp770->sc=Cyc_Absyn_Public;}
# 4662
_tmp770->name=_tmp761;
Cyc_Toc_fndecl_to_c(nv,_tmp770,cinclude);
({struct Cyc_List_List*_tmpBBB=({struct Cyc_List_List*_tmp763=_cycalloc(sizeof(*_tmp763));_tmp763->hd=d,_tmp763->tl=Cyc_Toc_result_decls;_tmp763;});Cyc_Toc_result_decls=_tmpBBB;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp764=0U;({struct _dyneither_ptr _tmpBBC=({const char*_tmp765="letdecl at toplevel";_tag_dyneither(_tmp765,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBBC,_tag_dyneither(_tmp764,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp766=0U;({struct _dyneither_ptr _tmpBBD=({const char*_tmp767="region decl at toplevel";_tag_dyneither(_tmp767,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBBD,_tag_dyneither(_tmp766,sizeof(void*),0U));});});case 5U: _LLB: _tmp771=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp75D)->f1;_LLC:
# 4670
 Cyc_Toc_aggrdecl_to_c(_tmp771,1);
goto _LL0;case 6U: _LLD: _tmp772=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp75D)->f1;_LLE:
# 4673
 if(_tmp772->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp772);else{
# 4676
Cyc_Toc_datatypedecl_to_c(_tmp772);}
goto _LL0;case 7U: _LLF: _tmp773=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp75D)->f1;_LL10:
# 4679
 Cyc_Toc_enumdecl_to_c(_tmp773);
({struct Cyc_List_List*_tmpBBE=({struct Cyc_List_List*_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768->hd=d,_tmp768->tl=Cyc_Toc_result_decls;_tmp768;});Cyc_Toc_result_decls=_tmpBBE;});
goto _LL0;case 8U: _LL11: _tmp774=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp75D)->f1;_LL12:
# 4683
 _tmp774->tvs=0;
if(_tmp774->defn != 0){
void*_tmp769=Cyc_Toc_typ_to_c((void*)_check_null(_tmp774->defn));
_tmp774->defn=_tmp769;{
# 4689
struct Cyc_Absyn_Decl*_tmp76A=Cyc_Toc_type2enumdecl(_tmp769);
if(_tmp76A != 0){
({struct Cyc_List_List*_tmpBBF=({struct Cyc_List_List*_tmp76B=_cycalloc(sizeof(*_tmp76B));_tmp76B->hd=_tmp76A,_tmp76B->tl=Cyc_Toc_result_decls;_tmp76B;});Cyc_Toc_result_decls=_tmpBBF;});
({void*_tmpBC0=Cyc_Toc_decl_to_enumtype(_tmp76A);_tmp774->defn=_tmpBC0;});}};}else{
# 4695
enum Cyc_Absyn_KindQual _tmp76C=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp774->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp76D=_tmp76C;if(_tmp76D == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpBC1=Cyc_Absyn_void_star_type();_tmp774->defn=_tmpBC1;});goto _LL1F;}else{_LL22: _LL23:
 _tmp774->defn=Cyc_Absyn_void_type;goto _LL1F;}_LL1F:;}
# 4703
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpBC2=({struct Cyc_List_List*_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E->hd=d,_tmp76E->tl=Cyc_Toc_result_decls;_tmp76E;});Cyc_Toc_result_decls=_tmpBC2;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp775=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp75D)->f2;_LL18:
 _tmp776=_tmp775;goto _LL1A;case 10U: _LL19: _tmp776=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp75D)->f2;_LL1A:
 _tmp777=_tmp776;goto _LL1C;case 11U: _LL1B: _tmp777=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp75D)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpBC3=Cyc_Toc_decls_to_c(r,nv,_tmp777,top,cinclude);nv=_tmpBC3;});goto _LL0;default: _LL1D: _tmp778=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp75D)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpBC4=Cyc_Toc_decls_to_c(r,nv,_tmp778,top,1);nv=_tmpBC4;});goto _LL0;}_LL0:;};}
# 4714
return nv;}
# 4718
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp779=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp77A=_tmp779;struct Cyc_Core_DynamicRegion*_tmp77F;_LL1: _tmp77F=_tmp77A.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp77F->h;
({struct Cyc_Toc_TocState*_tmpBC5=Cyc_Toc_empty_toc_state(h);ts=_tmpBC5;});;}
({struct Cyc_Toc_TocStateWrap*_tmpBC6=({struct Cyc_Toc_TocStateWrap*_tmp77C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp77C));_tmp77C->dyn=_tmp77F,_tmp77C->state=ts;_tmp77C;});Cyc_Toc_toc_state=_tmpBC6;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpBC7=_tag_dyneither(({unsigned int _tmp77E=35;struct _dyneither_ptr**_tmp77D=_cycalloc(_check_times(_tmp77E,sizeof(struct _dyneither_ptr*)));_tmp77D[0]=& Cyc_Toc__throw_str,_tmp77D[1]=& Cyc_Toc_setjmp_str,_tmp77D[2]=& Cyc_Toc__push_handler_str,_tmp77D[3]=& Cyc_Toc__pop_handler_str,_tmp77D[4]=& Cyc_Toc__exn_thrown_str,_tmp77D[5]=& Cyc_Toc__npop_handler_str,_tmp77D[6]=& Cyc_Toc__check_null_str,_tmp77D[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp77D[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp77D[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp77D[10]=& Cyc_Toc__dyneither_ptr_str,_tmp77D[11]=& Cyc_Toc__tag_dyneither_str,_tmp77D[12]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp77D[13]=& Cyc_Toc__get_dyneither_size_str,_tmp77D[14]=& Cyc_Toc__get_zero_arr_size_str,_tmp77D[15]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp77D[16]=& Cyc_Toc__zero_arr_plus_str,_tmp77D[17]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp77D[18]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp77D[19]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp77D[20]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp77D[21]=& Cyc_Toc__cycalloc_str,_tmp77D[22]=& Cyc_Toc__cyccalloc_str,_tmp77D[23]=& Cyc_Toc__cycalloc_atomic_str,_tmp77D[24]=& Cyc_Toc__cyccalloc_atomic_str,_tmp77D[25]=& Cyc_Toc__region_malloc_str,_tmp77D[26]=& Cyc_Toc__region_calloc_str,_tmp77D[27]=& Cyc_Toc__check_times_str,_tmp77D[28]=& Cyc_Toc__new_region_str,_tmp77D[29]=& Cyc_Toc__push_region_str,_tmp77D[30]=& Cyc_Toc__pop_region_str,_tmp77D[31]=& Cyc_Toc__throw_arraybounds_str,_tmp77D[32]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp77D[33]=& Cyc_Toc__throw_match_str,_tmp77D[34]=& Cyc_Toc__fast_region_malloc_str;_tmp77D;}),sizeof(struct _dyneither_ptr*),35U);Cyc_Toc_globals=_tmpBC7;});};}
# 4769
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp780=ts;struct Cyc_Toc_TocStateWrap*_tmp781=Cyc_Toc_toc_state;ts=_tmp781;Cyc_Toc_toc_state=_tmp780;});{
struct Cyc_Toc_TocStateWrap _tmp782=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp783=_tmp782;struct Cyc_Core_DynamicRegion*_tmp789;struct Cyc_Toc_TocState*_tmp788;_LL1: _tmp789=_tmp783.dyn;_tmp788=_tmp783.state;_LL2:;
# 4774
{struct _RegionHandle*h=& _tmp789->h;
{struct Cyc_Toc_TocState _tmp785=*_tmp788;struct Cyc_Toc_TocState _tmp786=_tmp785;struct Cyc_Xarray_Xarray*_tmp787;_LL4: _tmp787=_tmp786.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp787);}
# 4775
;}
# 4778
Cyc_Core_free_rckey(_tmp789);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4781
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4787
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4789
({struct Cyc_Hashtable_Table**_tmpBC8=({struct Cyc_Hashtable_Table**_tmp78A=_cycalloc(sizeof(*_tmp78A));*_tmp78A=pop_tables;_tmp78A;});Cyc_Toc_gpop_tables=_tmpBC8;});
Cyc_Toc_init();
{struct _RegionHandle _tmp78B=_new_region("start");struct _RegionHandle*start=& _tmp78B;_push_region(start);
({struct _RegionHandle*_tmpBCA=start;struct Cyc_Toc_Env*_tmpBC9=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpBCA,_tmpBC9,ds,1,0);});;_pop_region(start);}
# 4798
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}

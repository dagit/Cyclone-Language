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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 726 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 911
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 918
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 931
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 938
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 941
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 956
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_uint_typ;
# 958
extern void*Cyc_Absyn_sint_typ;
# 966
void*Cyc_Absyn_exn_typ();
# 975
extern void*Cyc_Absyn_bounds_one;
# 983
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 989
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 991
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 994
void*Cyc_Absyn_void_star_typ();
# 996
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1002
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
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
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1172
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);
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
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 48
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 65
int Cyc_Tcutil_is_arithmetic_type(void*);
# 86
int Cyc_Tcutil_is_pointer_type(void*t);
# 88
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
# 90
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 97
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 147
int Cyc_Tcutil_unify(void*,void*);
# 150
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 154
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 241 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 249
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 251
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 261
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 319
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 348
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 352
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 355
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 358
int Cyc_Tcutil_is_array(void*t);struct _tuple13{unsigned int f1;int f2;};
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
({struct _dyneither_ptr _tmp7AE=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7AD=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp7AD,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7AE,ap);});}
# 80
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 82
({struct _dyneither_ptr _tmp7B0=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7AF=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp7AF,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7B0,ap);});}
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
({struct Cyc_List_List**_tmp7BC=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp7BC;}),({
struct Cyc_List_List**_tmp7BB=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp7BB;}),({
struct Cyc_Dict_Dict*_tmp7BA=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp7B9=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp7B9;});_tmpC;});_tmp11->aggrs_so_far=_tmp7BA;}),({
struct Cyc_List_List**_tmp7B8=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp7B8;}),({
struct Cyc_Set_Set**_tmp7B7=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp7B6=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp7B6;});_tmpE;});_tmp11->datatypes_so_far=_tmp7B7;}),({
struct Cyc_Dict_Dict*_tmp7B5=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp7B4=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp7B4;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp7B5;}),({
struct Cyc_Dict_Dict*_tmp7B3=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp7B2=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp7B2;});_tmp10;});_tmp11->qvar_tags=_tmp7B3;}),({
struct Cyc_Xarray_Xarray*_tmp7B1=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp7B1;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 160
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 166
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 169
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp7BD=({struct Cyc_Toc_TocStateWrap _tmp778;_tmp778.dyn=_tmp1C,_tmp778.state=_tmp1B;_tmp778;});*ts=_tmp7BD;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp1A=({struct _tuple16 _tmp779;_tmp779.f1=_tmp1B,_tmp779.f2=arg;_tmp779;});
({void*_tmp7BE=f(h,& _tmp1A);res=_tmp7BE;});}
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
struct _tuple17 env=({struct _tuple17 _tmp77A;_tmp77A.f1=q,_tmp77A.f2=type_maker;_tmp77A;});
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
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11U,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 301
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 303
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 305
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 309
static struct _tuple20*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple20*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_List_List*_tmp7C0=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));({void*_tmp7BF=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=1U,_tmp6D->f1=name;_tmp6D;});_tmp6E->hd=_tmp7BF;}),_tmp6E->tl=0;_tmp6E;});_tmp6F->f1=_tmp7C0;}),_tmp6F->f2=e;_tmp6F;});}
# 315
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp70=e->r;void*_tmp71=_tmp70;struct Cyc_Absyn_Exp*_tmp72;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp72=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp72);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 321
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=14U,_tmp73->f1=t,_tmp73->f2=e,_tmp73->f3=0,_tmp73->f4=Cyc_Absyn_No_coercion;_tmp73;});}
# 324
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=20U,_tmp74->f1=e;_tmp74;});}
# 327
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=23U,_tmp75->f1=e1,_tmp75->f2=e2;_tmp75;});}
# 330
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=37U,_tmp76->f1=s;_tmp76;});}
# 333
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=17U,_tmp77->f1=t;_tmp77;});}
# 336
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=10U,_tmp78->f1=e,_tmp78->f2=es,_tmp78->f3=0,_tmp78->f4=1;_tmp78;});}
# 339
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=1U,_tmp79->f1=e;_tmp79;});}
# 342
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=2U,_tmp7A->f1=s1,_tmp7A->f2=s2;_tmp7A;});}
# 345
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=6U,_tmp7B->f1=e1,_tmp7B->f2=e2,_tmp7B->f3=e3;_tmp7B;});}
# 348
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=21U,_tmp7C->f1=e,_tmp7C->f2=n,_tmp7C->f3=0,_tmp7C->f4=0;_tmp7C;});}
# 351
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=22U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});}
# 354
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 357
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=36U,_tmp7E->f1=tdopt,_tmp7E->f2=ds;_tmp7E;});}
# 359
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=8U,_tmp7F->f1=v;_tmp7F;});}
# 362
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 366
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp80=e->r;void*_tmp81=_tmp80;struct Cyc_Absyn_Exp*_tmp82;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp81)->tag == 20U){_LL1: _tmp82=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp81)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp82,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 385
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 394
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 403
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 412
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 423
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp83=Cyc_Tcutil_compress(t);void*_tmp84=_tmp83;enum Cyc_Absyn_Size_of _tmp8B;switch(*((int*)_tmp84)){case 6U: _LL1: _tmp8B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp84)->f2;_LL2:
# 427
{enum Cyc_Absyn_Size_of _tmp85=_tmp8B;switch(_tmp85){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp86=0U;({struct _dyneither_ptr _tmp7C1=({const char*_tmp87="impossible IntType (not char, short or int)";_tag_dyneither(_tmp87,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C1,_tag_dyneither(_tmp86,sizeof(void*),0U));});});}_LLD:;}
# 433
goto _LL0;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp84)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}case 5U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp77B;_tmp77B.tag=0U,({struct _dyneither_ptr _tmp7C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp77B.f1=_tmp7C2;});_tmp77B;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({struct _dyneither_ptr _tmp7C3=({const char*_tmp89="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp89,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C3,_tag_dyneither(_tmp88,sizeof(void*),1U));});});}_LL0:;}
# 440
return function;}
# 442
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8D=_tmp8C;void*_tmp90;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->tag == 5U){_LL1: _tmp90=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->f1).elt_typ;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp90);}else{_LL3: _LL4:
({void*_tmp8E=0U;({struct _dyneither_ptr _tmp7C4=({const char*_tmp8F="impossible type (not pointer)";_tag_dyneither(_tmp8F,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C4,_tag_dyneither(_tmp8E,sizeof(void*),0U));});});}_LL0:;}
# 451
static int Cyc_Toc_is_array_type(void*t){
void*_tmp91=Cyc_Tcutil_compress(t);void*_tmp92=_tmp91;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp92)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 459
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp93=e->r;void*_tmp94=_tmp93;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;long long _tmp9A;int _tmp99;short _tmp98;struct _dyneither_ptr _tmp97;char _tmp96;switch(*((int*)_tmp94)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Null_c).tag){case 2U: _LL1: _tmp96=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Char_c).val).f2;_LL2:
 return _tmp96 == '\000';case 3U: _LL3: _tmp97=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Wchar_c).val;_LL4: {
# 463
unsigned long _tmp95=Cyc_strlen((struct _dyneither_ptr)_tmp97);
int i=0;
if(_tmp95 >= 2  && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp97.curr)[1]== 'x'  && _tmp95 >= 3) && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp95;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 473
return 0;}}case 4U: _LL5: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Short_c).val).f2;_LL6:
 return _tmp98 == 0;case 5U: _LL7: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Int_c).val).f2;_LL8:
 return _tmp99 == 0;case 6U: _LL9: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).LongLong_c).val).f2;_LLA:
 return _tmp9A == 0;case 1U: _LLD: _LLE:
# 478
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 477
 goto _LLE;case 14U: _LLF: _tmp9B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL10:
# 479
 return Cyc_Toc_is_zero(_tmp9B);case 24U: _LL11: _tmp9C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9C);case 26U: _LL13: _tmp9D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL14:
 _tmp9E=_tmp9D;goto _LL16;case 29U: _LL15: _tmp9E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp94)->f3;_LL16:
 _tmp9F=_tmp9E;goto _LL18;case 25U: _LL17: _tmp9F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL18:
 _tmpA0=_tmp9F;goto _LL1A;case 36U: _LL19: _tmpA0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL1A:
# 485
 for(0;_tmpA0 != 0;_tmpA0=_tmpA0->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple20*)_tmpA0->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 494
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA1=x;union Cyc_Absyn_Nmspace _tmpB3;struct _dyneither_ptr*_tmpB2;_LL1: _tmpB3=_tmpA1->f1;_tmpB2=_tmpA1->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA2=_tmpB3;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;switch((_tmpA2.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAF=0;goto _LL7;case 1U: _LL6: _tmpAF=(_tmpA2.Rel_n).val;_LL7:
 _tmpB0=_tmpAF;goto _LL9;case 2U: _LL8: _tmpB0=(_tmpA2.Abs_n).val;_LL9:
 _tmpB1=_tmpB0;goto _LLB;default: _LLA: _tmpB1=(_tmpA2.C_n).val;_LLB:
# 503
 if(_tmpB1 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp77D;_tmp77D.tag=0U,_tmp77D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp77D;});struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp77C;_tmp77C.tag=0U,_tmp77C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp77C;});void*_tmpA3[2U];_tmpA3[0]=& _tmpA5,_tmpA3[1]=& _tmpA6;({struct _dyneither_ptr _tmp7C5=({const char*_tmpA4="%s_%s_struct";_tag_dyneither(_tmpA4,sizeof(char),13U);});Cyc_aprintf(_tmp7C5,_tag_dyneither(_tmpA3,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA7=_new_region("r");struct _RegionHandle*r=& _tmpA7;_push_region(r);
{struct _dyneither_ptr _tmpAE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp780;_tmp780.tag=0U,_tmp780.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp780;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp77F;_tmp77F.tag=0U,({struct _dyneither_ptr _tmp7C8=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp7C7=r;struct Cyc_List_List*_tmp7C6=_tmpB1;Cyc_rstr_sepstr(_tmp7C7,_tmp7C6,({const char*_tmpAD="_";_tag_dyneither(_tmpAD,sizeof(char),2U);}));}));_tmp77F.f1=_tmp7C8;});_tmp77F;});struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp77E;_tmp77E.tag=0U,_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp77E;});void*_tmpA8[3U];_tmpA8[0]=& _tmpAA,_tmpA8[1]=& _tmpAB,_tmpA8[2]=& _tmpAC;({struct _dyneither_ptr _tmp7C9=({const char*_tmpA9="%s_%s_%s_struct";_tag_dyneither(_tmpA9,sizeof(char),16U);});Cyc_aprintf(_tmp7C9,_tag_dyneither(_tmpA8,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAE;};_pop_region(r);};}_LL3:;};}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 516
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 519
struct _tuple21 _tmpB4=*env;struct _tuple21 _tmpB5=_tmpB4;struct Cyc_Dict_Dict*_tmpC8;struct _tuple15*_tmpC7;_LL1: _tmpC8=(_tmpB5.f1)->qvar_tags;_tmpC7=_tmpB5.f2;_LL2:;{
struct _tuple15 _tmpB6=*_tmpC7;struct _tuple15 _tmpB7=_tmpB6;struct _tuple1*_tmpC6;struct _tuple1*_tmpC5;_LL4: _tmpC6=_tmpB7.f1;_tmpC5=_tmpB7.f2;_LL5:;{
struct _handler_cons _tmpB8;_push_handler(& _tmpB8);{int _tmpBA=0;if(setjmp(_tmpB8.handler))_tmpBA=1;if(!_tmpBA){{struct _tuple1*_tmpBB=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*_tmpC8,Cyc_Toc_qvar_tag_cmp,_tmpC7);_npop_handler(0U);return _tmpBB;};_pop_handler();}else{void*_tmpB9=(void*)_exn_thrown;void*_tmpBC=_tmpB9;void*_tmpC4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBC)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 523
struct _tuple15*_tmpBD=({struct _tuple15*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=_tmpC6,_tmpC3->f2=_tmpC5;_tmpC3;});
struct _tuple1*_tmpBE=_tmpC6;union Cyc_Absyn_Nmspace _tmpC2;struct _dyneither_ptr*_tmpC1;_LLC: _tmpC2=_tmpBE->f1;_tmpC1=_tmpBE->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC1,_tmpC5);
struct _tuple1*res=({struct _tuple1*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->f1=_tmpC2,({struct _dyneither_ptr*_tmp7CA=({struct _dyneither_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));*_tmpBF=newvar;_tmpBF;});_tmpC0->f2=_tmp7CA;});_tmpC0;});
({struct Cyc_Dict_Dict _tmp7CB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC8,_tmpBD,res);*_tmpC8=_tmp7CB;});
return res;};}}else{_LL9: _tmpC4=_tmpBC;_LLA:(int)_rethrow(_tmpC4);}_LL6:;}};};};}
# 532
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp781;_tmp781.f1=fieldname,_tmp781.f2=dtname;_tmp781;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 539
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->kind=Cyc_Absyn_StructA,_tmpCB->sc=Cyc_Absyn_Public,_tmpCB->tvs=0,_tmpCB->attributes=0,_tmpCB->expected_mem_kind=0,({
# 542
struct _tuple1*_tmp7CE=({struct _tuple1*_tmpC9=_cycalloc(sizeof(*_tmpC9));({union Cyc_Absyn_Nmspace _tmp7CD=Cyc_Absyn_Rel_n(0);_tmpC9->f1=_tmp7CD;}),_tmpC9->f2=name;_tmpC9;});_tmpCB->name=_tmp7CE;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp7CC=({struct Cyc_Absyn_AggrdeclImpl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->exist_vars=0,_tmpCA->rgn_po=0,_tmpCA->tagged=0,_tmpCA->fields=fs;_tmpCA;});_tmpCB->impl=_tmp7CC;});_tmpCB;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 551
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 554
struct _tuple22 _tmpCC=*env;struct _tuple22 _tmpCD=_tmpCC;struct Cyc_List_List**_tmpE4;struct Cyc_List_List*_tmpE3;_LL1: _tmpE4=(_tmpCD.f1)->tuple_types;_tmpE3=_tmpCD.f2;_LL2:;
# 556
{struct Cyc_List_List*_tmpCE=*_tmpE4;for(0;_tmpCE != 0;_tmpCE=_tmpCE->tl){
struct _tuple23*_tmpCF=(struct _tuple23*)_tmpCE->hd;struct _tuple23*_tmpD0=_tmpCF;void*_tmpD3;struct Cyc_List_List*_tmpD2;_LL4: _tmpD3=_tmpD0->f1;_tmpD2=_tmpD0->f2;_LL5:;{
struct Cyc_List_List*_tmpD1=_tmpE3;
for(0;_tmpD1 != 0  && _tmpD2 != 0;(_tmpD1=_tmpD1->tl,_tmpD2=_tmpD2->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD1->hd)).f2,(void*)_tmpD2->hd))
break;}
if(_tmpD1 == 0  && _tmpD2 == 0)
return _tmpD3;};}}{
# 567
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct _dyneither_ptr _tmp7D0=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE1=({struct Cyc_Int_pa_PrintArg_struct _tmp782;_tmp782.tag=1U,_tmp782.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp782;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _dyneither_ptr _tmp7CF=({const char*_tmpE0="_tuple%d";_tag_dyneither(_tmpE0,sizeof(char),9U);});Cyc_aprintf(_tmp7CF,_tag_dyneither(_tmpDF,sizeof(void*),1U));});});*_tmpE2=_tmp7D0;});_tmpE2;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE3);
struct Cyc_List_List*_tmpD4=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp7D3=({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct Cyc_Absyn_Aggrfield*_tmp7D2=({struct Cyc_Absyn_Aggrfield*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct _dyneither_ptr*_tmp7D1=Cyc_Absyn_fieldname(i);_tmpD5->name=_tmp7D1;}),_tmpD5->tq=Cyc_Toc_mt_tq,_tmpD5->type=(void*)ts2->hd,_tmpD5->width=0,_tmpD5->attributes=0,_tmpD5->requires_clause=0;_tmpD5;});_tmpD6->hd=_tmp7D2;}),_tmpD6->tl=_tmpD4;_tmpD6;});_tmpD4=_tmp7D3;});}}
({struct Cyc_List_List*_tmp7D4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD4);_tmpD4=_tmp7D4;});{
struct Cyc_Absyn_Aggrdecl*_tmpD7=Cyc_Toc_make_c_struct_defn(xname,_tmpD4);
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD8=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7D5=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDD=_cycalloc(sizeof(*_tmpDD));*_tmpDD=_tmpD7;_tmpDD;}));(_tmpDE->f1).aggr_info=_tmp7D5;}),(_tmpDE->f1).targs=0;_tmpDE;});
({struct Cyc_List_List*_tmp7D7=({struct Cyc_List_List*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Absyn_Decl*_tmp7D6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->tag=5U,_tmpD9->f1=_tmpD7;_tmpD9;}),0U);_tmpDA->hd=_tmp7D6;}),_tmpDA->tl=Cyc_Toc_result_decls;_tmpDA;});Cyc_Toc_result_decls=_tmp7D7;});
({struct Cyc_List_List*_tmp7D9=({struct Cyc_List_List*_tmpDC=_region_malloc(d,sizeof(*_tmpDC));({struct _tuple23*_tmp7D8=({struct _tuple23*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));_tmpDB->f1=(void*)_tmpD8,_tmpDB->f2=ts;_tmpDB;});_tmpDC->hd=_tmp7D8;}),_tmpDC->tl=*_tmpE4;_tmpDC;});*_tmpE4=_tmp7D9;});
return(void*)_tmpD8;};};}
# 580
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 585
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 588
struct _tuple25*_tmpE5=env;struct Cyc_List_List**_tmpFA;enum Cyc_Absyn_AggrKind _tmpF9;struct Cyc_List_List*_tmpF8;_LL1: _tmpFA=(_tmpE5->f1)->anon_aggr_types;_tmpF9=(_tmpE5->f2)->f1;_tmpF8=(_tmpE5->f2)->f2;_LL2:;
# 590
{struct Cyc_List_List*_tmpE6=*_tmpFA;for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
struct _tuple26*_tmpE7=(struct _tuple26*)_tmpE6->hd;struct _tuple26*_tmpE8=_tmpE7;void*_tmpEB;enum Cyc_Absyn_AggrKind _tmpEA;struct Cyc_List_List*_tmpE9;_LL4: _tmpEB=_tmpE8->f1;_tmpEA=_tmpE8->f2;_tmpE9=_tmpE8->f3;_LL5:;
if(_tmpF9 != _tmpEA)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE9,_tmpF8))
return _tmpEB;}}{
# 599
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct _dyneither_ptr _tmp7DB=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp783;_tmp783.tag=1U,_tmp783.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp783;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp7DA=({const char*_tmpF5="_tuple%d";_tag_dyneither(_tmpF5,sizeof(char),9U);});Cyc_aprintf(_tmp7DA,_tag_dyneither(_tmpF4,sizeof(void*),1U));});});*_tmpF7=_tmp7DB;});_tmpF7;});
struct Cyc_Absyn_Aggrdecl*_tmpEC=Cyc_Toc_make_c_struct_defn(xname,_tmpF8);
_tmpEC->kind=_tmpF9;{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpED=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7DC=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF2=_cycalloc(sizeof(*_tmpF2));*_tmpF2=_tmpEC;_tmpF2;}));(_tmpF3->f1).aggr_info=_tmp7DC;}),(_tmpF3->f1).targs=0;_tmpF3;});
({struct Cyc_List_List*_tmp7DE=({struct Cyc_List_List*_tmpEF=_cycalloc(sizeof(*_tmpEF));({struct Cyc_Absyn_Decl*_tmp7DD=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->tag=5U,_tmpEE->f1=_tmpEC;_tmpEE;}),0U);_tmpEF->hd=_tmp7DD;}),_tmpEF->tl=Cyc_Toc_result_decls;_tmpEF;});Cyc_Toc_result_decls=_tmp7DE;});
({struct Cyc_List_List*_tmp7E0=({struct Cyc_List_List*_tmpF1=_region_malloc(d,sizeof(*_tmpF1));({struct _tuple26*_tmp7DF=({struct _tuple26*_tmpF0=_region_malloc(d,sizeof(*_tmpF0));_tmpF0->f1=(void*)_tmpED,_tmpF0->f2=_tmpF9,_tmpF0->f3=_tmpF8;_tmpF0;});_tmpF1->hd=_tmp7DF;}),_tmpF1->tl=*_tmpFA;_tmpF1;});*_tmpFA=_tmp7E0;});
return(void*)_tmpED;};};}
# 607
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=ak,_tmpFB->f2=fs;_tmpFB;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 616
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 625
struct _tuple28 _tmpFC=*env;struct _tuple28 _tmpFD=_tmpFC;struct Cyc_List_List**_tmp120;struct _tuple1*_tmp11F;struct Cyc_List_List*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_List_List*_tmp11C;_LL1: _tmp120=(_tmpFD.f1)->abs_struct_types;_tmp11F=(_tmpFD.f2)->f1;_tmp11E=(_tmpFD.f2)->f2;_tmp11D=(_tmpFD.f2)->f3;_tmp11C=(_tmpFD.f2)->f4;_LL2:;
# 629
{struct Cyc_List_List*_tmpFE=*_tmp120;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct _tuple29*_tmpFF=(struct _tuple29*)_tmpFE->hd;struct _tuple29*_tmp100=_tmpFF;struct _tuple1*_tmp108;struct Cyc_List_List*_tmp107;void*_tmp106;_LL4: _tmp108=_tmp100->f1;_tmp107=_tmp100->f2;_tmp106=_tmp100->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp108,_tmp11F)== 0  && ({
int _tmp7E1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11D);_tmp7E1 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp107);})){
int okay=1;
{struct Cyc_List_List*_tmp101=_tmp11D;for(0;_tmp101 != 0;(_tmp101=_tmp101->tl,_tmp107=_tmp107->tl)){
void*_tmp102=(void*)_tmp101->hd;
void*_tmp103=(void*)((struct Cyc_List_List*)_check_null(_tmp107))->hd;
{struct Cyc_Absyn_Kind*_tmp104=Cyc_Tcutil_typ_kind(_tmp102);struct Cyc_Absyn_Kind*_tmp105=_tmp104;switch(((struct Cyc_Absyn_Kind*)_tmp105)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 640
 continue;default: _LLB: _LLC:
# 643
 if(Cyc_Tcutil_unify(_tmp102,_tmp103) || ({void*_tmp7E2=Cyc_Toc_typ_to_c(_tmp102);Cyc_Tcutil_unify(_tmp7E2,Cyc_Toc_typ_to_c(_tmp103));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 648
break;}}
# 650
if(okay)
return _tmp106;}}}{
# 657
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct _dyneither_ptr _tmp7E4=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11A=({struct Cyc_Int_pa_PrintArg_struct _tmp784;_tmp784.tag=1U,_tmp784.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp784;});void*_tmp118[1U];_tmp118[0]=& _tmp11A;({struct _dyneither_ptr _tmp7E3=({const char*_tmp119="_tuple%d";_tag_dyneither(_tmp119,sizeof(char),9U);});Cyc_aprintf(_tmp7E3,_tag_dyneither(_tmp118,sizeof(void*),1U));});});*_tmp11B=_tmp7E4;});_tmp11B;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp109=0;
# 661
({struct Cyc_List_List*_tmp7E6=({struct Cyc_List_List*_tmp10B=_region_malloc(d,sizeof(*_tmp10B));({struct _tuple29*_tmp7E5=({struct _tuple29*_tmp10A=_region_malloc(d,sizeof(*_tmp10A));_tmp10A->f1=_tmp11F,_tmp10A->f2=_tmp11D,_tmp10A->f3=x;_tmp10A;});_tmp10B->hd=_tmp7E5;}),_tmp10B->tl=*_tmp120;_tmp10B;});*_tmp120=_tmp7E6;});{
# 664
struct _RegionHandle _tmp10C=_new_region("r");struct _RegionHandle*r=& _tmp10C;_push_region(r);
{struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11E,_tmp11D);
for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){
struct Cyc_Absyn_Aggrfield*_tmp10E=(struct Cyc_Absyn_Aggrfield*)_tmp11C->hd;
void*t=_tmp10E->type;
struct Cyc_List_List*atts=_tmp10E->attributes;
# 674
if((_tmp11C->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))) && !
Cyc_Toc_is_array_type(t))
({struct Cyc_List_List*_tmp7E8=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({void*_tmp7E7=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=6U,_tmp10F->f1=0;_tmp10F;});_tmp110->hd=_tmp7E7;}),_tmp110->tl=atts;_tmp110;});atts=_tmp7E8;});
({void*_tmp7E9=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10D,t));t=_tmp7E9;});
# 681
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
({void*_tmp7EE=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=8U,({void*_tmp7ED=Cyc_Absyn_void_star_typ();(_tmp111->f1).elt_type=_tmp7ED;}),({struct Cyc_Absyn_Tqual _tmp7EC=Cyc_Absyn_empty_tqual(0U);(_tmp111->f1).tq=_tmp7EC;}),({
struct Cyc_Absyn_Exp*_tmp7EB=Cyc_Absyn_uint_exp(0U,0U);(_tmp111->f1).num_elts=_tmp7EB;}),({union Cyc_Absyn_Constraint*_tmp7EA=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp111->f1).zero_term=_tmp7EA;}),(_tmp111->f1).zt_loc=0U;_tmp111;});
# 682
t=_tmp7EE;});
# 685
({struct Cyc_List_List*_tmp7F0=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Aggrfield*_tmp7EF=({struct Cyc_Absyn_Aggrfield*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->name=_tmp10E->name,_tmp112->tq=Cyc_Toc_mt_tq,_tmp112->type=t,_tmp112->width=_tmp10E->width,_tmp112->attributes=atts,_tmp112->requires_clause=0;_tmp112;});_tmp113->hd=_tmp7EF;}),_tmp113->tl=_tmp109;_tmp113;});_tmp109=_tmp7F0;});}
# 687
({struct Cyc_List_List*_tmp7F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp109);_tmp109=_tmp7F1;});{
struct Cyc_Absyn_Aggrdecl*_tmp114=Cyc_Toc_make_c_struct_defn(xname,_tmp109);
({struct Cyc_List_List*_tmp7F3=({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Absyn_Decl*_tmp7F2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=5U,_tmp115->f1=_tmp114;_tmp115;}),0U);_tmp116->hd=_tmp7F2;}),_tmp116->tl=Cyc_Toc_result_decls;_tmp116;});Cyc_Toc_result_decls=_tmp7F3;});{
void*_tmp117=x;_npop_handler(0U);return _tmp117;};};}
# 665
;_pop_region(r);};};}
# 693
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 697
struct _tuple27 env=({struct _tuple27 _tmp785;_tmp785.f1=struct_name,_tmp785.f2=type_vars,_tmp785.f3=type_args,_tmp785.f4=fields;_tmp785;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 705
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp7F6=({struct _dyneither_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));({struct _dyneither_ptr _tmp7F5=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp123=({struct Cyc_Int_pa_PrintArg_struct _tmp786;_tmp786.tag=1U,_tmp786.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp786;});void*_tmp121[1U];_tmp121[0]=& _tmp123;({struct _dyneither_ptr _tmp7F4=({const char*_tmp122="_tmp%X";_tag_dyneither(_tmp122,sizeof(char),7U);});Cyc_aprintf(_tmp7F4,_tag_dyneither(_tmp121,sizeof(void*),1U));});});*_tmp124=_tmp7F5;});_tmp124;});_tmp125->f2=_tmp7F6;});_tmp125;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 711
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp126=*env;struct _tuple30 _tmp127=_tmp126;struct Cyc_Xarray_Xarray*_tmp12F;_LL1: _tmp12F=(_tmp127.f1)->temp_labels;_LL2:;{
int _tmp128=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp7F7=_tmp128;_tmp7F7 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12F);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12F,_tmp128);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct _dyneither_ptr _tmp7F9=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12D=({struct Cyc_Int_pa_PrintArg_struct _tmp787;_tmp787.tag=1U,_tmp787.f1=(unsigned int)_tmp128;_tmp787;});void*_tmp12B[1U];_tmp12B[0]=& _tmp12D;({struct _dyneither_ptr _tmp7F8=({const char*_tmp12C="_LL%X";_tag_dyneither(_tmp12C,sizeof(char),6U);});Cyc_aprintf(_tmp7F8,_tag_dyneither(_tmp12B,sizeof(void*),1U));});});*_tmp12E=_tmp7F9;});_tmp12E;});
if(({int _tmp7FA=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12F,res);_tmp7FA != _tmp128;}))
({void*_tmp129=0U;({struct _dyneither_ptr _tmp7FB=({const char*_tmp12A="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp12A,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7FB,_tag_dyneither(_tmp129,sizeof(void*),0U));});});
return res;};};}
# 722
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 728
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp130=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp130))->hd)->name)!= 0){
++ ans;
_tmp130=_tmp130->tl;}
# 735
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 741
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp131=*a;struct _tuple9 _tmp132=_tmp131;struct _dyneither_ptr*_tmp136;struct Cyc_Absyn_Tqual _tmp135;void*_tmp134;_LL1: _tmp136=_tmp132.f1;_tmp135=_tmp132.f2;_tmp134=_tmp132.f3;_LL2:;
return({struct _tuple9*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->f1=_tmp136,_tmp133->f2=_tmp135,({void*_tmp7FC=Cyc_Toc_typ_to_c(_tmp134);_tmp133->f3=_tmp7FC;});_tmp133;});}
# 764 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_tmp138=_tmp137;void*_tmp13E;void*_tmp13D;struct Cyc_Absyn_Tqual _tmp13C;struct Cyc_Absyn_Exp*_tmp13B;union Cyc_Absyn_Constraint*_tmp13A;unsigned int _tmp139;switch(*((int*)_tmp138)){case 8U: _LL1: _tmp13D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).elt_type;_tmp13C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).tq;_tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).num_elts;_tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zero_term;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zt_loc;_LL2:
# 767
 return({void*_tmp7FD=Cyc_Toc_typ_to_c_array(_tmp13D);Cyc_Absyn_cstar_typ(_tmp7FD,_tmp13C);});case 1U: _LL3: _tmp13E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp138)->f2;if(_tmp13E != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13E);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 773
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 775
struct Cyc_Absyn_Aggrfield*_tmp13F=({struct Cyc_Absyn_Aggrfield*_tmp140=_cycalloc(sizeof(*_tmp140));*_tmp140=*f;_tmp140;});
_tmp13F->type=new_type;
_tmp13F->requires_clause=0;
_tmp13F->tq=Cyc_Toc_mt_tq;
return _tmp13F;}
# 782
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 784
return;}
# 787
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
({void**_tmp7FF=({void**_tmp141=_cycalloc(sizeof(*_tmp141));({void*_tmp7FE=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);*_tmp141=_tmp7FE;});_tmp141;});cs=_tmp7FF;});
return*cs;}
# 793
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp802=({void**_tmp142=_cycalloc(sizeof(*_tmp142));({void*_tmp801=({void*_tmp800=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp800,Cyc_Toc_mt_tq);});*_tmp142=_tmp801;});_tmp142;});r=_tmp802;});
return*r;}
# 799
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp805=({void**_tmp143=_cycalloc(sizeof(*_tmp143));({void*_tmp804=({void*_tmp803=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp803,Cyc_Toc_mt_tq);});*_tmp143=_tmp804;});_tmp143;});r=_tmp805;});
return*r;}
# 805
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp144=Cyc_Tcutil_compress(t);void*_tmp145=_tmp144;struct Cyc_Absyn_Tvar*_tmp146;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp145)->tag == 2U){_LL1: _tmp146=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp145)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 811
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp147=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp148=_tmp147;if(((struct Cyc_Absyn_Kind*)_tmp148)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 818
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp149=t;struct Cyc_Absyn_Datatypedecl*_tmp192;void**_tmp191;struct Cyc_Absyn_Enumdecl*_tmp190;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18D;struct _tuple1*_tmp18C;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Typedefdecl*_tmp18A;void*_tmp189;struct Cyc_List_List*_tmp188;struct _tuple1*_tmp187;union Cyc_Absyn_AggrInfoU _tmp186;struct Cyc_List_List*_tmp185;enum Cyc_Absyn_AggrKind _tmp184;struct Cyc_List_List*_tmp183;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Tqual _tmp181;void*_tmp180;struct Cyc_List_List*_tmp17F;int _tmp17E;struct Cyc_Absyn_VarargInfo*_tmp17D;struct Cyc_List_List*_tmp17C;void*_tmp17B;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_Absyn_Exp*_tmp179;unsigned int _tmp178;void*_tmp177;struct Cyc_Absyn_Tqual _tmp176;union Cyc_Absyn_Constraint*_tmp175;struct Cyc_Absyn_Datatypedecl*_tmp174;struct Cyc_Absyn_Datatypefield*_tmp173;struct Cyc_Absyn_Tvar*_tmp172;void**_tmp171;switch(*((int*)_tmp149)){case 0U: _LL1: _LL2:
 return t;case 1U: _LL3: _tmp171=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp149)->f2;_LL4:
# 822
 if(*_tmp171 == 0){
*_tmp171=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 826
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp171));case 2U: _LL5: _tmp172=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->f1;_LL6:
# 828
 if((Cyc_Tcutil_tvar_kind(_tmp172,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 830
return(void*)& Cyc_Absyn_VoidType_val;else{
# 832
return Cyc_Absyn_void_star_typ();}case 3U: _LL7: _LL8:
# 834
 return(void*)& Cyc_Absyn_VoidType_val;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).tag == 2){_LL9: _tmp174=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).val).f1;_tmp173=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).val).f2;_LLA:
# 836
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp173->name,_tmp174->name));}else{_LLB: _LLC:
({void*_tmp14A=0U;({struct _dyneither_ptr _tmp806=({const char*_tmp14B="unresolved DatatypeFieldType";_tag_dyneither(_tmp14B,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp806,_tag_dyneither(_tmp14A,sizeof(void*),0U));});});}case 5U: _LLD: _tmp177=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_typ;_tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_tq;_tmp175=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).ptr_atts).bounds;_LLE:
# 841
({void*_tmp807=Cyc_Toc_typ_to_c(_tmp177);_tmp177=_tmp807;});{
void*_tmp14C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp175);void*_tmp14D=_tmp14C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp14D)->tag == 0U){_LL42: _LL43:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LL44: _LL45:
 return Cyc_Absyn_star_typ(_tmp177,(void*)& Cyc_Absyn_HeapRgn_val,_tmp176,Cyc_Absyn_false_conref);}_LL41:;};case 6U: _LLF: _LL10:
# 846
 goto _LL12;case 7U: _LL11: _LL12:
 return t;case 8U: _LL13: _tmp17B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).elt_type;_tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).tq;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).num_elts;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).zt_loc;_LL14:
# 849
 return({void*_tmp80B=Cyc_Toc_typ_to_c(_tmp17B);struct Cyc_Absyn_Tqual _tmp80A=_tmp17A;struct Cyc_Absyn_Exp*_tmp809=_tmp179;union Cyc_Absyn_Constraint*_tmp808=Cyc_Absyn_false_conref;Cyc_Absyn_array_typ(_tmp80B,_tmp80A,_tmp809,_tmp808,_tmp178);});case 9U: _LL15: _tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_tqual;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_typ;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).args;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).c_varargs;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).cyc_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).attributes;_LL16: {
# 855
struct Cyc_List_List*_tmp14E=0;
for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
void*_tmp14F=(void*)_tmp17C->hd;void*_tmp150=_tmp14F;switch(*((int*)_tmp150)){case 4U: _LL47: _LL48:
 goto _LL4A;case 5U: _LL49: _LL4A:
 goto _LL4C;case 19U: _LL4B: _LL4C:
 continue;case 22U: _LL4D: _LL4E:
 continue;case 21U: _LL4F: _LL50:
 continue;case 20U: _LL51: _LL52:
 continue;default: _LL53: _LL54:
({struct Cyc_List_List*_tmp80C=({struct Cyc_List_List*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->hd=(void*)_tmp17C->hd,_tmp151->tl=_tmp14E;_tmp151;});_tmp14E=_tmp80C;});goto _LL46;}_LL46:;}{
# 866
struct Cyc_List_List*_tmp152=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17F);
if(_tmp17D != 0){
# 869
void*_tmp153=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp17D->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp154=({struct _tuple9*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->f1=_tmp17D->name,_tmp156->f2=_tmp17D->tq,_tmp156->f3=_tmp153;_tmp156;});
({struct Cyc_List_List*_tmp80E=({struct Cyc_List_List*_tmp80D=_tmp152;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp80D,({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->hd=_tmp154,_tmp155->tl=0;_tmp155;}));});_tmp152=_tmp80E;});}
# 873
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->tag=9U,(_tmp157->f1).tvars=0,(_tmp157->f1).effect=0,(_tmp157->f1).ret_tqual=_tmp181,({void*_tmp80F=Cyc_Toc_typ_to_c(_tmp180);(_tmp157->f1).ret_typ=_tmp80F;}),(_tmp157->f1).args=_tmp152,(_tmp157->f1).c_varargs=_tmp17E,(_tmp157->f1).cyc_varargs=0,(_tmp157->f1).rgn_po=0,(_tmp157->f1).attributes=_tmp14E,(_tmp157->f1).requires_clause=0,(_tmp157->f1).requires_relns=0,(_tmp157->f1).ensures_clause=0,(_tmp157->f1).ensures_relns=0;_tmp157;});};}case 10U: _LL17: _tmp182=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp149)->f1;_LL18: {
# 878
struct Cyc_List_List*_tmp158=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp812=({struct Cyc_List_List*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct _tuple12*_tmp811=({struct _tuple12*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->f1=(*((struct _tuple12*)_tmp182->hd)).f1,({void*_tmp810=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp182->hd)).f2);_tmp159->f2=_tmp810;});_tmp159;});_tmp15A->hd=_tmp811;}),_tmp15A->tl=_tmp158;_tmp15A;});_tmp158=_tmp812;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp158));}case 12U: _LL19: _tmp184=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f1;_tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f2;_LL1A: {
# 887
struct Cyc_List_List*_tmp15B=0;
for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
({struct Cyc_List_List*_tmp815=({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));({struct Cyc_Absyn_Aggrfield*_tmp814=({struct Cyc_Absyn_Aggrfield*_tmp813=(struct Cyc_Absyn_Aggrfield*)_tmp183->hd;Cyc_Toc_aggrfield_to_c(_tmp813,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp183->hd)->type));});_tmp15C->hd=_tmp814;}),_tmp15C->tl=_tmp15B;_tmp15C;});_tmp15B=_tmp815;});}
return({enum Cyc_Absyn_AggrKind _tmp816=_tmp184;Cyc_Toc_add_anon_aggr_type(_tmp816,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15B));});}case 11U: _LL1B: _tmp186=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp149)->f1).aggr_info;_tmp185=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp149)->f1).targs;_LL1C:
# 893
{union Cyc_Absyn_AggrInfoU _tmp15D=_tmp186;if((_tmp15D.UnknownAggr).tag == 1){_LL56: _LL57:
 return t;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}{
# 897
struct Cyc_Absyn_Aggrdecl*_tmp15E=Cyc_Absyn_get_known_aggrdecl(_tmp186);
if(_tmp15E->expected_mem_kind){
# 900
if(_tmp15E->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp161=({struct Cyc_String_pa_PrintArg_struct _tmp789;_tmp789.tag=0U,({
struct _dyneither_ptr _tmp817=(struct _dyneither_ptr)(_tmp15E->kind == Cyc_Absyn_UnionA?({const char*_tmp163="union";_tag_dyneither(_tmp163,sizeof(char),6U);}):({const char*_tmp164="struct";_tag_dyneither(_tmp164,sizeof(char),7U);}));_tmp789.f1=_tmp817;});_tmp789;});struct Cyc_String_pa_PrintArg_struct _tmp162=({struct Cyc_String_pa_PrintArg_struct _tmp788;_tmp788.tag=0U,({
struct _dyneither_ptr _tmp818=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name));_tmp788.f1=_tmp818;});_tmp788;});void*_tmp15F[2U];_tmp15F[0]=& _tmp161,_tmp15F[1]=& _tmp162;({struct _dyneither_ptr _tmp819=({const char*_tmp160="%s %s was never defined.";_tag_dyneither(_tmp160,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp819,_tag_dyneither(_tmp15F,sizeof(void*),2U));});});}
# 908
if(_tmp15E->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp165=_tmp15E->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15E->impl))->fields;
if(_tmp165 == 0)return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp165))->tl != 0;_tmp165=_tmp165->tl){;}{
void*_tmp166=((struct Cyc_Absyn_Aggrfield*)_tmp165->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp166))){
if(_tmp15E->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp169=({struct Cyc_String_pa_PrintArg_struct _tmp78A;_tmp78A.tag=0U,({struct _dyneither_ptr _tmp81A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name));_tmp78A.f1=_tmp81A;});_tmp78A;});void*_tmp167[1U];_tmp167[0]=& _tmp169;({struct _dyneither_ptr _tmp81B=({const char*_tmp168="struct %s ended up being abstract.";_tag_dyneither(_tmp168,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp81B,_tag_dyneither(_tmp167,sizeof(void*),1U));});});{
# 921
struct _RegionHandle _tmp16A=_new_region("r");struct _RegionHandle*r=& _tmp16A;_push_region(r);
{struct Cyc_List_List*_tmp16B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15E->tvs,_tmp185);
void*_tmp16C=Cyc_Tcutil_rsubstitute(r,_tmp16B,_tmp166);
if(Cyc_Toc_is_abstract_type(_tmp16C)){void*_tmp16D=Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp16D;}{
void*_tmp16E=Cyc_Toc_add_struct_type(_tmp15E->name,_tmp15E->tvs,_tmp185,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15E->impl))->fields);_npop_handler(0U);return _tmp16E;};}
# 922
;_pop_region(r);};}
# 927
return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);};};};case 13U: _LL1D: _tmp187=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp149)->f1;_LL1E:
 return t;case 14U: _LL1F: _tmp188=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp149)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp188);return t;case 17U: _LL21: _tmp18C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f1;_tmp18B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f2;_tmp18A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f3;_tmp189=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f4;_LL22:
# 933
 if(_tmp189 == 0){
if(_tmp18B != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=17U,_tmp16F->f1=_tmp18C,_tmp16F->f2=0,_tmp16F->f3=_tmp18A,_tmp16F->f4=0;_tmp16F;});else{
return t;}}else{
# 938
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->tag=17U,_tmp170->f1=_tmp18C,_tmp170->f2=0,_tmp170->f3=_tmp18A,({void*_tmp81C=Cyc_Toc_typ_to_c(_tmp189);_tmp170->f4=_tmp81C;});_tmp170;});}case 19U: _LL23: _LL24:
 return Cyc_Absyn_uint_typ;case 15U: _LL25: _LL26:
 return Cyc_Toc_rgn_typ();case 16U: _LL27: _LL28:
 return Cyc_Toc_dyn_rgn_typ();case 20U: _LL29: _LL2A:
# 944
 goto _LL2C;case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 23U: _LL2F: _LL30:
 goto _LL32;case 24U: _LL31: _LL32:
 goto _LL34;case 25U: _LL33: _LL34:
 return Cyc_Absyn_void_star_typ();case 18U: _LL35: _tmp18D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp149)->f1;_LL36:
# 954
 return t;case 27U: _LL37: _tmp18E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp149)->f1;_LL38:
# 959
 if(_tmp18E->topt != 0)return Cyc_Toc_typ_to_c((void*)_check_null(_tmp18E->topt));else{
return t;}case 28U: _LL39: _LL3A:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)){case 0U: _LL3B: _tmp18F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL3C:
# 963
 Cyc_Toc_aggrdecl_to_c(_tmp18F,1);
if(_tmp18F->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_strctq);}case 1U: _LL3D: _tmp190=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL3E:
# 968
 Cyc_Toc_enumdecl_to_c(_tmp190);
return t;default: _LL3F: _tmp192=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_tmp191=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f2;_LL40:
# 971
 Cyc_Toc_datatypedecl_to_c(_tmp192);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp191)));}}_LL0:;}
# 976
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp193=t;void*_tmp195;struct Cyc_Absyn_Tqual _tmp194;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->tag == 8U){_LL1: _tmp195=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).elt_type;_tmp194=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).tq;_LL2:
# 979
 return({void*_tmp81D=Cyc_Absyn_star_typ(_tmp195,(void*)& Cyc_Absyn_HeapRgn_val,_tmp194,Cyc_Absyn_false_conref);Cyc_Toc_cast_it(_tmp81D,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 986
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp196=Cyc_Tcutil_compress(t);void*_tmp197=_tmp196;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Datatypedecl*_tmp1A2;struct Cyc_Absyn_Datatypefield*_tmp1A1;struct Cyc_List_List*_tmp1A0;union Cyc_Absyn_AggrInfoU _tmp19F;void*_tmp19E;switch(*((int*)_tmp197)){case 0U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 0;case 6U: _LL5: _LL6:
 goto _LL8;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;case 9U: _LLD: _LLE:
 goto _LL10;case 19U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmp19E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->f1).elt_type;_LL12:
 return Cyc_Toc_atomic_typ(_tmp19E);case 11U: _LL13: _tmp19F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp197)->f1).aggr_info;_LL14:
# 1003
{union Cyc_Absyn_AggrInfoU _tmp198=_tmp19F;if((_tmp198.UnknownAggr).tag == 1){_LL26: _LL27:
 return 0;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}{
# 1007
struct Cyc_Absyn_Aggrdecl*_tmp199=Cyc_Absyn_get_known_aggrdecl(_tmp19F);
if(_tmp199->impl == 0)
return 0;
{struct Cyc_List_List*_tmp19A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp199->impl))->fields;for(0;_tmp19A != 0;_tmp19A=_tmp19A->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp19A->hd)->type))return 0;}}
return 1;};case 12U: _LL15: _tmp1A0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp197)->f2;_LL16:
# 1014
 for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1A0->hd)->type))return 0;}
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).tag == 2){_LL17: _tmp1A2=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).val).f1;_tmp1A1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).val).f2;_LL18:
# 1018
 _tmp1A3=_tmp1A1->typs;goto _LL1A;}else{goto _LL23;}case 10U: _LL19: _tmp1A3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp197)->f1;_LL1A:
# 1020
 for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple12*)_tmp1A3->hd)).f2))return 0;}
return 1;case 3U: _LL1B: _LL1C:
# 1025
 goto _LL1E;case 5U: _LL1D: _LL1E:
 goto _LL20;case 16U: _LL1F: _LL20:
 goto _LL22;case 15U: _LL21: _LL22:
 return 0;default: _LL23: _LL24:
({struct Cyc_String_pa_PrintArg_struct _tmp19D=({struct Cyc_String_pa_PrintArg_struct _tmp78B;_tmp78B.tag=0U,({struct _dyneither_ptr _tmp81E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp78B.f1=_tmp81E;});_tmp78B;});void*_tmp19B[1U];_tmp19B[0]=& _tmp19D;({struct _dyneither_ptr _tmp81F=({const char*_tmp19C="atomic_typ:  bad type %s";_tag_dyneither(_tmp19C,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81F,_tag_dyneither(_tmp19B,sizeof(void*),1U));});});}_LL0:;}
# 1033
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A4=Cyc_Tcutil_compress(t);void*_tmp1A5=_tmp1A4;void*_tmp1A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->tag == 5U){_LL1: _tmp1A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_typ;_LL2: {
# 1036
void*_tmp1A6=Cyc_Tcutil_compress(_tmp1A8);void*_tmp1A7=_tmp1A6;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1A7)->tag == 0U){_LL6: _LL7:
 return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 1040
 return 0;}_LL0:;}
# 1044
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1048
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1053
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A9=Cyc_Tcutil_compress(t);void*_tmp1AA=_tmp1A9;struct Cyc_List_List*_tmp1B6;union Cyc_Absyn_AggrInfoU _tmp1B5;switch(*((int*)_tmp1AA)){case 11U: _LL1: _tmp1B5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1AA)->f1).aggr_info;_LL2: {
# 1056
struct Cyc_Absyn_Aggrdecl*_tmp1AB=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);
if(_tmp1AB->impl == 0)
({void*_tmp1AC=0U;({struct _dyneither_ptr _tmp820=({const char*_tmp1AD="is_poly_field: type missing fields";_tag_dyneither(_tmp1AD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp820,_tag_dyneither(_tmp1AC,sizeof(void*),0U));});});
_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AB->impl))->fields;goto _LL4;}case 12U: _LL3: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AA)->f2;_LL4: {
# 1061
struct Cyc_Absyn_Aggrfield*_tmp1AE=Cyc_Absyn_lookup_field(_tmp1B6,f);
if(_tmp1AE == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp78C;_tmp78C.tag=0U,_tmp78C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp78C;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _dyneither_ptr _tmp821=({const char*_tmp1B0="is_poly_field: bad field %s";_tag_dyneither(_tmp1B0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp821,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AE->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp78D;_tmp78D.tag=0U,({struct _dyneither_ptr _tmp822=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp78D.f1=_tmp822;});_tmp78D;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp823=({const char*_tmp1B3="is_poly_field: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp823,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL0:;}
# 1072
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B7=e->r;void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct _dyneither_ptr*_tmp1BF;switch(*((int*)_tmp1B8)){case 21U: _LL1: _tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL2:
# 1075
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C0->topt),_tmp1BF) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL4: {
# 1078
void*_tmp1B9=Cyc_Tcutil_compress((void*)_check_null(_tmp1C2->topt));void*_tmp1BA=_tmp1B9;void*_tmp1BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->tag == 5U){_LL8: _tmp1BE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->f1).elt_typ;_LL9:
# 1080
 return Cyc_Toc_is_poly_field(_tmp1BE,_tmp1C1) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp78E;_tmp78E.tag=0U,({struct _dyneither_ptr _tmp824=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C2->topt)));_tmp78E.f1=_tmp824;});_tmp78E;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct _dyneither_ptr _tmp825=({const char*_tmp1BC="is_poly_project: bad type %s";_tag_dyneither(_tmp1BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp825,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1083
 return 0;}_LL0:;}
# 1088
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp826=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp826,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=s,_tmp1C3->tl=0;_tmp1C3;}),0U);});}
# 1092
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp827=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp827,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1096
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp828=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp828,({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1106
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp829=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp829,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1110
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return({struct Cyc_Absyn_Exp*_tmp82A=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp82A,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=s,_tmp1C7[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1114
return({struct Cyc_Absyn_Exp*_tmp82B=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp82B,({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=s,_tmp1C8[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1117
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp82C=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp82C,({struct Cyc_Absyn_Exp*_tmp1C9[3U];_tmp1C9[0]=rgn,_tmp1C9[1]=s,_tmp1C9[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1121
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp82D=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp82D,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=e,_tmp1CA->tl=0;_tmp1CA;}),0U);});}
# 1124
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp82E=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp82E,({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=e,_tmp1CB->tl=0;_tmp1CB;}),0U);});}
# 1128
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp82F=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp82F,({void*_tmp1CC=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CC,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1133
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1141
int is_string=0;
{void*_tmp1CD=e->r;void*_tmp1CE=_tmp1CD;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1147
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp831=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct Cyc_Absyn_Decl*_tmp830=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->tag=0U,_tmp1CF->f1=vd;_tmp1CF;}),0U);_tmp1D0->hd=_tmp830;}),_tmp1D0->tl=Cyc_Toc_result_decls;_tmp1D0;});Cyc_Toc_result_decls=_tmp831;});
({struct Cyc_Absyn_Exp*_tmp832=Cyc_Absyn_var_exp(x,0U);xexp=_tmp832;});
({struct Cyc_Absyn_Exp*_tmp833=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp833;});}else{
# 1157
({struct Cyc_Absyn_Exp*_tmp835=({void*_tmp834=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp834,e);});xexp=_tmp835;});
# 1159
({struct Cyc_Absyn_Exp*_tmp837=({void*_tmp836=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp836,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp837;});}
# 1161
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1D1[3U];({struct _tuple20*_tmp83A=({struct _tuple20*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xexp;_tmp1D2;});_tmp1D1[0]=_tmp83A;}),({
struct _tuple20*_tmp839=({struct _tuple20*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xexp;_tmp1D3;});_tmp1D1[1]=_tmp839;}),({
struct _tuple20*_tmp838=({struct _tuple20*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->f1=0,_tmp1D4->f2=xplussz;_tmp1D4;});_tmp1D1[2]=_tmp838;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D1,sizeof(struct _tuple20*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1198 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D5=nv;int _tmp1D6;_LL1: _tmp1D6=_tmp1D5->toplevel;_LL2:;
return _tmp1D6;}
# 1202
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D7=nv;int*_tmp1D8;_LL1: _tmp1D8=_tmp1D7->in_lhs;_LL2:;
return*_tmp1D8;}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));_tmp1DB->break_lab=0,_tmp1DB->continue_lab=0,_tmp1DB->fallthru_info=0,_tmp1DB->toplevel=1,({
# 1213
int*_tmp83C=({int*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));*_tmp1D9=0;_tmp1D9;});_tmp1DB->in_lhs=_tmp83C;}),({
int*_tmp83B=({int*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));*_tmp1DA=0;_tmp1DA;});_tmp1DB->in_sizeof=_tmp83B;}),_tmp1DB->rgn=r;_tmp1DB;});}
# 1218
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DC=e;struct _dyneither_ptr**_tmp1E3;struct _dyneither_ptr**_tmp1E2;struct Cyc_Toc_FallthruInfo*_tmp1E1;int _tmp1E0;int*_tmp1DF;int*_tmp1DE;_LL1: _tmp1E3=_tmp1DC->break_lab;_tmp1E2=_tmp1DC->continue_lab;_tmp1E1=_tmp1DC->fallthru_info;_tmp1E0=_tmp1DC->toplevel;_tmp1DF=_tmp1DC->in_lhs;_tmp1DE=_tmp1DC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->break_lab=_tmp1E3,_tmp1DD->continue_lab=_tmp1E2,_tmp1DD->fallthru_info=_tmp1E1,_tmp1DD->toplevel=_tmp1E0,_tmp1DD->in_lhs=_tmp1DF,_tmp1DD->in_sizeof=_tmp1DE,_tmp1DD->rgn=r;_tmp1DD;});}
# 1223
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E4=e;struct _dyneither_ptr**_tmp1EB;struct _dyneither_ptr**_tmp1EA;struct Cyc_Toc_FallthruInfo*_tmp1E9;int _tmp1E8;int*_tmp1E7;int*_tmp1E6;_LL1: _tmp1EB=_tmp1E4->break_lab;_tmp1EA=_tmp1E4->continue_lab;_tmp1E9=_tmp1E4->fallthru_info;_tmp1E8=_tmp1E4->toplevel;_tmp1E7=_tmp1E4->in_lhs;_tmp1E6=_tmp1E4->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->break_lab=_tmp1EB,_tmp1E5->continue_lab=_tmp1EA,_tmp1E5->fallthru_info=_tmp1E9,_tmp1E5->toplevel=0,_tmp1E5->in_lhs=_tmp1E7,_tmp1E5->in_sizeof=_tmp1E6,_tmp1E5->rgn=r;_tmp1E5;});}
# 1227
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EC=e;struct _dyneither_ptr**_tmp1F3;struct _dyneither_ptr**_tmp1F2;struct Cyc_Toc_FallthruInfo*_tmp1F1;int _tmp1F0;int*_tmp1EF;int*_tmp1EE;_LL1: _tmp1F3=_tmp1EC->break_lab;_tmp1F2=_tmp1EC->continue_lab;_tmp1F1=_tmp1EC->fallthru_info;_tmp1F0=_tmp1EC->toplevel;_tmp1EF=_tmp1EC->in_lhs;_tmp1EE=_tmp1EC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->break_lab=_tmp1F3,_tmp1ED->continue_lab=_tmp1F2,_tmp1ED->fallthru_info=_tmp1F1,_tmp1ED->toplevel=1,_tmp1ED->in_lhs=_tmp1EF,_tmp1ED->in_sizeof=_tmp1EE,_tmp1ED->rgn=r;_tmp1ED;});}
# 1231
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F4=e;int*_tmp1F5;_LL1: _tmp1F5=_tmp1F4->in_lhs;_LL2:;
*_tmp1F5=b;}
# 1235
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F6=e;int*_tmp1F8;_LL1: _tmp1F8=_tmp1F6->in_sizeof;_LL2:;{
int _tmp1F7=*_tmp1F8;
*_tmp1F8=b;
return _tmp1F7;};}
# 1241
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F9=e;int*_tmp1FA;_LL1: _tmp1FA=_tmp1F9->in_sizeof;_LL2:;
return*_tmp1FA;}
# 1248
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FB=e;struct _dyneither_ptr**_tmp202;struct _dyneither_ptr**_tmp201;struct Cyc_Toc_FallthruInfo*_tmp200;int _tmp1FF;int*_tmp1FE;int*_tmp1FD;_LL1: _tmp202=_tmp1FB->break_lab;_tmp201=_tmp1FB->continue_lab;_tmp200=_tmp1FB->fallthru_info;_tmp1FF=_tmp1FB->toplevel;_tmp1FE=_tmp1FB->in_lhs;_tmp1FD=_tmp1FB->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));_tmp1FC->break_lab=0,_tmp1FC->continue_lab=0,_tmp1FC->fallthru_info=_tmp200,_tmp1FC->toplevel=_tmp1FF,_tmp1FC->in_lhs=_tmp1FE,_tmp1FC->in_sizeof=_tmp1FD,_tmp1FC->rgn=r;_tmp1FC;});}
# 1254
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1258
struct Cyc_Toc_Env*_tmp203=e;struct _dyneither_ptr**_tmp20C;struct _dyneither_ptr**_tmp20B;struct Cyc_Toc_FallthruInfo*_tmp20A;int _tmp209;int*_tmp208;int*_tmp207;_LL1: _tmp20C=_tmp203->break_lab;_tmp20B=_tmp203->continue_lab;_tmp20A=_tmp203->fallthru_info;_tmp209=_tmp203->toplevel;_tmp208=_tmp203->in_lhs;_tmp207=_tmp203->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp206=_region_malloc(r,sizeof(*_tmp206));
_tmp206->label=fallthru_l,_tmp206->binders=fallthru_binders;_tmp206;});
return({struct Cyc_Toc_Env*_tmp205=_region_malloc(r,sizeof(*_tmp205));({struct _dyneither_ptr**_tmp83D=({struct _dyneither_ptr**_tmp204=_region_malloc(r,sizeof(*_tmp204));*_tmp204=break_l;_tmp204;});_tmp205->break_lab=_tmp83D;}),_tmp205->continue_lab=_tmp20B,_tmp205->fallthru_info=fi,_tmp205->toplevel=_tmp209,_tmp205->in_lhs=_tmp208,_tmp205->in_sizeof=_tmp207,_tmp205->rgn=r;_tmp205;});};}
# 1264
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1266
struct Cyc_Toc_Env*_tmp20D=e;struct _dyneither_ptr**_tmp215;struct _dyneither_ptr**_tmp214;struct Cyc_Toc_FallthruInfo*_tmp213;int _tmp212;int*_tmp211;int*_tmp210;_LL1: _tmp215=_tmp20D->break_lab;_tmp214=_tmp20D->continue_lab;_tmp213=_tmp20D->fallthru_info;_tmp212=_tmp20D->toplevel;_tmp211=_tmp20D->in_lhs;_tmp210=_tmp20D->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp20F=_region_malloc(r,sizeof(*_tmp20F));({struct _dyneither_ptr**_tmp83E=({struct _dyneither_ptr**_tmp20E=_region_malloc(r,sizeof(*_tmp20E));*_tmp20E=break_l;_tmp20E;});_tmp20F->break_lab=_tmp83E;}),_tmp20F->continue_lab=_tmp214,_tmp20F->fallthru_info=0,_tmp20F->toplevel=_tmp212,_tmp20F->in_lhs=_tmp211,_tmp20F->in_sizeof=_tmp210,_tmp20F->rgn=r;_tmp20F;});}
# 1273
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1276
struct Cyc_Toc_Env*_tmp216=e;struct _dyneither_ptr**_tmp21E;struct _dyneither_ptr**_tmp21D;struct Cyc_Toc_FallthruInfo*_tmp21C;int _tmp21B;int*_tmp21A;int*_tmp219;_LL1: _tmp21E=_tmp216->break_lab;_tmp21D=_tmp216->continue_lab;_tmp21C=_tmp216->fallthru_info;_tmp21B=_tmp216->toplevel;_tmp21A=_tmp216->in_lhs;_tmp219=_tmp216->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->break_lab=0,_tmp218->continue_lab=_tmp21D,({struct Cyc_Toc_FallthruInfo*_tmp83F=({struct Cyc_Toc_FallthruInfo*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->label=next_l,_tmp217->binders=0;_tmp217;});_tmp218->fallthru_info=_tmp83F;}),_tmp218->toplevel=_tmp21B,_tmp218->in_lhs=_tmp21A,_tmp218->in_sizeof=_tmp219,_tmp218->rgn=r;_tmp218;});}
# 1289 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1292
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1294
void*_tmp21F=e->annot;void*_tmp220=_tmp21F;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _LL2: {
# 1297
void*_tmp221=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp222=_tmp221;union Cyc_Absyn_Constraint*_tmp225;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->tag == 5U){_LLC: _tmp225=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->f1).ptr_atts).nullable;_LLD:
# 1300
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp225);}else{_LLE: _LLF:
({void*_tmp223=0U;({struct _dyneither_ptr _tmp840=({const char*_tmp224="Toc::is_nullable";_tag_dyneither(_tmp224,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp840,_tag_dyneither(_tmp223,sizeof(void*),0U));});});}_LLB:;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _LL4:
# 1305
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1307
({void*_tmp226=0U;({unsigned int _tmp842=e->loc;struct _dyneither_ptr _tmp841=({const char*_tmp227="NULL pointer check will definitely fail";_tag_dyneither(_tmp227,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp842,_tmp841,_tag_dyneither(_tmp226,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
# 1313
 return 1;}else{_LL9: _LLA:
({void*_tmp228=0U;({struct _dyneither_ptr _tmp843=({const char*_tmp229="need_null_check";_tag_dyneither(_tmp229,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp843,_tag_dyneither(_tmp228,sizeof(void*),0U));});});}}}}_LL0:;}
# 1318
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp22A=e->annot;void*_tmp22B=_tmp22A;struct Cyc_List_List*_tmp231;struct Cyc_List_List*_tmp230;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp230=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL2:
 return _tmp230;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp231=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL4:
 return _tmp231;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1323
({void*_tmp22C=0U;({unsigned int _tmp845=e->loc;struct _dyneither_ptr _tmp844=({const char*_tmp22D="dereference of NULL pointer";_tag_dyneither(_tmp22D,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp845,_tmp844,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp22E=0U;({struct _dyneither_ptr _tmp846=({const char*_tmp22F="get_relns";_tag_dyneither(_tmp22F,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp846,_tag_dyneither(_tmp22E,sizeof(void*),0U));});});}}}}_LL0:;}static char _tmp235[8U]="*bogus*";
# 1333
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1343 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1346
struct Cyc_Absyn_Vardecl*x;
{void*_tmp232=a->r;void*_tmp233=_tmp232;void*_tmp236;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->tag == 1U){_LL1: _tmp236=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->f1;_LL2: {
# 1349
struct Cyc_Absyn_Vardecl*_tmp234=Cyc_Tcutil_nonesc_vardecl(_tmp236);
if(_tmp234 == 0)goto _LL4;
x=_tmp234;
goto _LL0;}}else{_LL3: _LL4: {
# 1355
static struct _dyneither_ptr bogus_string={_tmp235,_tmp235,_tmp235 + 8U};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1358
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1364
x=& bogus_vardecl;
x->type=a_typ;}}_LL0:;}{
# 1370
void*_tmp237=a_typ;
void*_tmp238=x->type;
x->type=_tmp237;{
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1379
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp237);
if(bound != 0){
# 1382
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
({struct Cyc_List_List*_tmp847=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);relns=_tmp847;});}
# 1389
if(valid_rop_i){
struct Cyc_List_List*_tmp239=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1397
int _tmp23A=Cyc_Relations_consistent_relations(_tmp239);
x->type=_tmp238;
if(!_tmp23A)return 1;}
# 1402
inner_loop: {
void*_tmp23B=i->r;void*_tmp23C=_tmp23B;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Absyn_Exp*_tmp241;void*_tmp240;struct Cyc_Absyn_Exp*_tmp23F;switch(*((int*)_tmp23C)){case 14U: _LL6: _tmp240=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23C)->f1;_tmp23F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23C)->f2;_LL7:
 i=_tmp23F;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->tl != 0){_LL8: _tmp242=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->hd;_tmp241=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->tl)->hd;_LL9: {
# 1408
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp241,& rop_z)){
# 1411
struct Cyc_List_List*_tmp23D=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1413
int _tmp23E=Cyc_Relations_consistent_relations(_tmp23D);
x->type=_tmp238;
return !_tmp23E;}
# 1417
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1420
x->type=_tmp238;
return 0;};};}
# 1424
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp243=0U;({struct _dyneither_ptr _tmp848=({const char*_tmp244="Missing type in primop ";_tag_dyneither(_tmp244,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp848,_tag_dyneither(_tmp243,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1428
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->f1=Cyc_Toc_mt_tq,({void*_tmp849=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp245->f2=_tmp849;});_tmp245;});}
# 1433
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp246=e->r;void*_tmp247=_tmp246;struct Cyc_Absyn_Exp*_tmp24F;int _tmp24E;struct Cyc_Absyn_Exp*_tmp24D;int _tmp24C;struct Cyc_List_List*_tmp24B;switch(*((int*)_tmp247)){case 26U: _LL1: _tmp24B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp247)->f1;_LL2:
# 1436
{struct Cyc_List_List*_tmp248=_tmp24B;for(0;_tmp248 != 0;_tmp248=_tmp248->tl){
if((*((struct _tuple20*)_tmp248->hd)).f1 != 0)
({void*_tmp249=0U;({struct _dyneither_ptr _tmp84A=({const char*_tmp24A="array designators for abstract-field initialization";_tag_dyneither(_tmp24A,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp84A,_tag_dyneither(_tmp249,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp84B=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp24B),0U);_tmp24D=_tmp84B;});_tmp24C=0;goto _LL4;case 27U: _LL3: _tmp24D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp247)->f2;_tmp24C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp247)->f4;_LL4:
 _tmp24F=_tmp24D;_tmp24E=_tmp24C;goto _LL6;case 28U: _LL5: _tmp24F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp247)->f1;_tmp24E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp247)->f3;_LL6:
# 1442
({struct Cyc_Absyn_Exp*_tmp84C=Cyc_Absyn_copy_exp(_tmp24F);_tmp24F=_tmp84C;});
return _tmp24E?({struct Cyc_Absyn_Exp*_tmp84D=_tmp24F;Cyc_Absyn_add_exp(_tmp84D,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp24F;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1451
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1459
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp250=e->r;void*_tmp251=_tmp250;struct Cyc_List_List*_tmp252;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp251)->tag == 29U){_LL1: _tmp252=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp251)->f3;_LL2:
 dles=_tmp252;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1465
{void*_tmp253=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp254=_tmp253;struct Cyc_Absyn_Aggrdecl*_tmp255;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp254)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp254)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp255=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp254)->f1).aggr_info).KnownAggr).val;_LL7:
# 1467
 if(_tmp255->impl == 0)
return 0;
if(_tmp255->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp255->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{_LL8: _LL9:
# 1475
 return 0;}_LL5:;}
# 1477
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
void*_tmp25B=Cyc_Tcutil_compress(_tmp256->type);void*_tmp25C=_tmp25B;void*_tmp25F;struct Cyc_Absyn_Exp*_tmp25E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->tag == 8U){_LLE: _tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->f1).elt_type;_tmp25E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->f1).num_elts;_LLF:
# 1492
 if(_tmp25E == 0  || !Cyc_Toc_is_zero(_tmp25E))
return 0;
# 1498
return({struct Cyc_Absyn_Exp*_tmp850=({struct Cyc_Absyn_Exp*_tmp84F=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp84F,({struct Cyc_Absyn_Exp*_tmp25D[2U];_tmp25D[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp260)),({
# 1500
struct Cyc_Absyn_Exp*_tmp84E=Cyc_Absyn_sizeoftyp_exp(_tmp25F,0U);_tmp25D[1]=_tmp84E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25D,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1498
Cyc_Absyn_add_exp(_tmp850,
# 1501
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1506
({void*_tmp262=0U;({struct _dyneither_ptr _tmp851=({const char*_tmp263="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp263,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp851,_tag_dyneither(_tmp262,sizeof(void*),0U));});});};}
# 1509
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp264=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp264 != 0;_tmp264=_tmp264->tl){
struct Cyc_Absyn_Aggrfield*_tmp265=(struct Cyc_Absyn_Aggrfield*)_tmp264->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp265->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1516
({void*_tmp266=0U;({struct _dyneither_ptr _tmp853=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp269=({struct Cyc_String_pa_PrintArg_struct _tmp78F;_tmp78F.tag=0U,_tmp78F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp78F;});void*_tmp267[1U];_tmp267[0]=& _tmp269;({struct _dyneither_ptr _tmp852=({const char*_tmp268="get_member_offset %s failed";_tag_dyneither(_tmp268,sizeof(char),28U);});Cyc_aprintf(_tmp852,_tag_dyneither(_tmp267,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp853,_tag_dyneither(_tmp266,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1520
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1523
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1526
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->tag=5U,_tmp26A->f1=e1,_tmp26A->f2=incr;_tmp26A;}),0U);}
# 1530
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1539
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1541
void*_tmp26B=e1->r;void*_tmp26C=_tmp26B;struct Cyc_Absyn_Exp*_tmp274;struct _dyneither_ptr*_tmp273;int _tmp272;int _tmp271;void*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Stmt*_tmp26E;switch(*((int*)_tmp26C)){case 37U: _LL1: _tmp26E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp26E,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp270=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp26F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp26F,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp274=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp273=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_tmp272=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_tmp271=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f4;_LL6:
# 1546
 e1->r=_tmp274->r;
({struct Cyc_Absyn_Exp*_tmp856=e1;struct Cyc_List_List*_tmp855=({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->hd=_tmp273,_tmp26D->tl=fs;_tmp26D;});struct Cyc_Absyn_Exp*(*_tmp854)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp856,_tmp855,_tmp854,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1554
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1556
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp857=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp857;});}
({void*_tmp858=(f(e1_copy,f_env))->r;e1->r=_tmp858;});
goto _LL0;}}_LL0:;}
# 1562
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1564
void*_tmp275=s->r;void*_tmp276=_tmp275;struct Cyc_Absyn_Stmt*_tmp27D;struct Cyc_Absyn_Decl*_tmp27C;struct Cyc_Absyn_Stmt*_tmp27B;struct Cyc_Absyn_Exp*_tmp27A;switch(*((int*)_tmp276)){case 1U: _LL1: _tmp27A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp276)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp27A,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp27C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp276)->f1;_tmp27B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp276)->f2;_LL4:
# 1567
 Cyc_Toc_lvalue_assign_stmt(_tmp27B,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp27D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp276)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp27D,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp279=({struct Cyc_String_pa_PrintArg_struct _tmp790;_tmp790.tag=0U,({struct _dyneither_ptr _tmp859=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp790.f1=_tmp859;});_tmp790;});void*_tmp277[1U];_tmp277[0]=& _tmp279;({struct _dyneither_ptr _tmp85A=({const char*_tmp278="lvalue_assign_stmt: %s";_tag_dyneither(_tmp278,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp85A,_tag_dyneither(_tmp277,sizeof(void*),1U));});});}_LL0:;}
# 1573
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1577
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp27E=e->r;void*_tmp27F=_tmp27E;struct Cyc_Absyn_Stmt*_tmp286;struct Cyc_Absyn_Exp*_tmp285;void**_tmp284;struct Cyc_Absyn_Exp**_tmp283;switch(*((int*)_tmp27F)){case 14U: _LL1: _tmp284=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_tmp283=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27F)->f2;_LL2:
# 1580
({struct Cyc_Absyn_Exp*_tmp85B=Cyc_Toc_push_address_exp(*_tmp283);*_tmp283=_tmp85B;});
({void*_tmp85C=Cyc_Absyn_cstar_typ(*_tmp284,Cyc_Toc_mt_tq);*_tmp284=_tmp85C;});
return e;case 20U: _LL3: _tmp285=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_LL4:
# 1584
 return _tmp285;case 37U: _LL5: _tmp286=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_LL6:
# 1588
 Cyc_Toc_push_address_stmt(_tmp286);
return e;default: _LL7: _LL8:
# 1591
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp791;_tmp791.tag=0U,({struct _dyneither_ptr _tmp85D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp791.f1=_tmp85D;});_tmp791;});void*_tmp280[1U];_tmp280[0]=& _tmp282;({struct _dyneither_ptr _tmp85E=({const char*_tmp281="can't take & of exp %s";_tag_dyneither(_tmp281,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85E,_tag_dyneither(_tmp280,sizeof(void*),1U));});});}_LL0:;}
# 1596
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp287=s->r;void*_tmp288=_tmp287;struct Cyc_Absyn_Exp**_tmp28E;struct Cyc_Absyn_Stmt*_tmp28D;struct Cyc_Absyn_Stmt*_tmp28C;switch(*((int*)_tmp288)){case 2U: _LL1: _tmp28C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL2:
 _tmp28D=_tmp28C;goto _LL4;case 12U: _LL3: _tmp28D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp28D);goto _LL0;case 1U: _LL5: _tmp28E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp85F=Cyc_Toc_push_address_exp(*_tmp28E);*_tmp28E=_tmp85F;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp28B=({struct Cyc_String_pa_PrintArg_struct _tmp792;_tmp792.tag=0U,({struct _dyneither_ptr _tmp860=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp792.f1=_tmp860;});_tmp792;});void*_tmp289[1U];_tmp289[0]=& _tmp28B;({struct _dyneither_ptr _tmp861=({const char*_tmp28A="can't take & of stmt %s";_tag_dyneither(_tmp28A,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp861,_tag_dyneither(_tmp289,sizeof(void*),1U));});});}_LL0:;}
# 1605
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp28F=Cyc_Tcutil_compress(t);void*_tmp290=_tmp28F;struct Cyc_Absyn_PtrInfo _tmp293;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp290)->tag == 5U){_LL1: _tmp293=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp290)->f1;_LL2:
 return _tmp293;}else{_LL3: _LL4:
({void*_tmp291=0U;({struct _dyneither_ptr _tmp862=({const char*_tmp292="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp292,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp862,_tag_dyneither(_tmp291,sizeof(void*),0U));});});}_LL0:;}
# 1615
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp294=Cyc_Tcutil_compress(t);void*_tmp295=_tmp294;int _tmp2A1;enum Cyc_Absyn_Sign _tmp2A0;enum Cyc_Absyn_Sign _tmp29F;enum Cyc_Absyn_Sign _tmp29E;enum Cyc_Absyn_Sign _tmp29D;enum Cyc_Absyn_Sign _tmp29C;switch(*((int*)_tmp295)){case 5U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp863=Cyc_Absyn_null_exp(0U);res=_tmp863;});goto _LL0;case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f2){case Cyc_Absyn_Char_sz: _LL3: _tmp29C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmp864=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp29C,'\000'),0U);res=_tmp864;});goto _LL0;case Cyc_Absyn_Short_sz: _LL5: _tmp29D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp865=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp29D,0),0U);res=_tmp865;});goto _LL0;case Cyc_Absyn_Int_sz: _LLB: _tmp29E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LLC:
# 1623
 _tmp29F=_tmp29E;goto _LLE;case Cyc_Absyn_Long_sz: _LLD: _tmp29F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LLE:
# 1625
({struct Cyc_Absyn_Exp*_tmp866=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp29F,0),0U);res=_tmp866;});goto _LL0;default: _LLF: _tmp2A0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LL10:
# 1627
({struct Cyc_Absyn_Exp*_tmp867=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp2A0,(long long)0),0U);res=_tmp867;});goto _LL0;}case 13U: _LL7: _LL8:
# 1621
 goto _LLA;case 14U: _LL9: _LLA:
 _tmp29E=Cyc_Absyn_Unsigned;goto _LLC;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp295)->f1){case 0U: _LL11: _LL12:
# 1628
({struct Cyc_Absyn_Exp*_tmp868=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp296="0.0F";_tag_dyneither(_tmp296,sizeof(char),5U);}),0),0U);res=_tmp868;});goto _LL0;case 1U: _LL13: _LL14:
({struct Cyc_Absyn_Exp*_tmp869=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp297="0.0";_tag_dyneither(_tmp297,sizeof(char),4U);}),1),0U);res=_tmp869;});goto _LL0;default: _LL15: _tmp2A1=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp295)->f1;_LL16:
({struct Cyc_Absyn_Exp*_tmp86B=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp86A=({const char*_tmp298="0.0L";_tag_dyneither(_tmp298,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp86A,_tmp2A1);}),0U);res=_tmp86B;});goto _LL0;}default: _LL17: _LL18:
# 1632
({struct Cyc_String_pa_PrintArg_struct _tmp29B=({struct Cyc_String_pa_PrintArg_struct _tmp793;_tmp793.tag=0U,({struct _dyneither_ptr _tmp86C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp793.f1=_tmp86C;});_tmp793;});void*_tmp299[1U];_tmp299[0]=& _tmp29B;({struct _dyneither_ptr _tmp86D=({const char*_tmp29A="found non-zero type %s in generate_zero";_tag_dyneither(_tmp29A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp86D,_tag_dyneither(_tmp299,sizeof(void*),1U));});});}_LL0:;}
# 1634
res->topt=t;
return res;}
# 1641
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1696 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp2A2=Cyc_Toc_typ_to_c(elt_type);
void*_tmp2A3=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp2A4=Cyc_Absyn_cstar_typ(_tmp2A2,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp2A5=({struct Cyc_Core_Opt*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->v=_tmp2A4;_tmp2D6;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp2A6=e1->r;void*_tmp2A7=_tmp2A6;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;switch(*((int*)_tmp2A7)){case 20U: _LL1: _tmp2AB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2A7)->f1;_LL2:
# 1704
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp86E=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AB,0,Cyc_Absyn_Other_coercion,0U);_tmp2AB=_tmp86E;});
_tmp2AB->topt=fat_ptr_type;}
# 1708
Cyc_Toc_exp_to_c(nv,_tmp2AB);xinit=_tmp2AB;goto _LL0;case 23U: _LL3: _tmp2AD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A7)->f1;_tmp2AC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A7)->f2;_LL4:
# 1710
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AD,0,Cyc_Absyn_Other_coercion,0U);_tmp2AD=_tmp86F;});
_tmp2AD->topt=fat_ptr_type;}
# 1714
Cyc_Toc_exp_to_c(nv,_tmp2AD);Cyc_Toc_exp_to_c(nv,_tmp2AC);
({struct Cyc_Absyn_Exp*_tmp872=({struct Cyc_Absyn_Exp*_tmp871=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp871,({struct Cyc_Absyn_Exp*_tmp2A8[3U];_tmp2A8[0]=_tmp2AD,({
struct Cyc_Absyn_Exp*_tmp870=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2A8[1]=_tmp870;}),_tmp2A8[2]=_tmp2AC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A8,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1715
xinit=_tmp872;});
# 1717
goto _LL0;default: _LL5: _LL6:
({void*_tmp2A9=0U;({struct _dyneither_ptr _tmp873=({const char*_tmp2AA="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp2AA,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp873,_tag_dyneither(_tmp2A9,sizeof(void*),0U));});});}_LL0:;}{
# 1720
struct _tuple1*_tmp2AE=Cyc_Toc_temp_var();
struct _RegionHandle _tmp2AF=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp2AF;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp2B0=({struct Cyc_Absyn_Vardecl*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5->sc=Cyc_Absyn_Public,_tmp2D5->name=_tmp2AE,_tmp2D5->varloc=0U,_tmp2D5->tq=Cyc_Toc_mt_tq,_tmp2D5->type=_tmp2A3,_tmp2D5->initializer=xinit,_tmp2D5->rgn=0,_tmp2D5->attributes=0,_tmp2D5->escapes=0;_tmp2D5;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B1=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4->tag=4U,_tmp2D4->f1=_tmp2B0;_tmp2D4;});
struct Cyc_Absyn_Exp*_tmp2B2=Cyc_Absyn_varb_exp((void*)_tmp2B1,0U);
_tmp2B2->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp2B3=Cyc_Absyn_deref_exp(_tmp2B2,0U);
_tmp2B3->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp2B3);{
struct _tuple1*_tmp2B4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B5=({struct Cyc_Absyn_Vardecl*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->sc=Cyc_Absyn_Public,_tmp2D3->name=_tmp2B4,_tmp2D3->varloc=0U,_tmp2D3->tq=Cyc_Toc_mt_tq,_tmp2D3->type=_tmp2A2,_tmp2D3->initializer=_tmp2B3,_tmp2D3->rgn=0,_tmp2D3->attributes=0,_tmp2D3->escapes=0;_tmp2D3;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B6=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->tag=4U,_tmp2D2->f1=_tmp2B5;_tmp2D2;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2B7=Cyc_Absyn_varb_exp((void*)_tmp2B6,0U);
_tmp2B7->topt=_tmp2B3->topt;
({struct Cyc_Absyn_Exp*_tmp874=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2B7,e2,0U);z_init=_tmp874;});
z_init->topt=_tmp2B7->topt;}
# 1739
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2B8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B9=({struct Cyc_Absyn_Vardecl*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->sc=Cyc_Absyn_Public,_tmp2D1->name=_tmp2B8,_tmp2D1->varloc=0U,_tmp2D1->tq=Cyc_Toc_mt_tq,_tmp2D1->type=_tmp2A2,_tmp2D1->initializer=z_init,_tmp2D1->rgn=0,_tmp2D1->attributes=0,_tmp2D1->escapes=0;_tmp2D1;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BA=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->tag=4U,_tmp2D0->f1=_tmp2B9;_tmp2D0;});
# 1745
struct Cyc_Absyn_Exp*_tmp2BB=Cyc_Absyn_varb_exp((void*)_tmp2B6,0U);_tmp2BB->topt=_tmp2B3->topt;{
struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2BB,_tmp2BC,0U);
_tmp2BD->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2BD);{
# 1751
struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Absyn_varb_exp((void*)_tmp2BA,0U);_tmp2BE->topt=_tmp2B3->topt;{
struct Cyc_Absyn_Exp*_tmp2BF=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2C0=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2BE,_tmp2BF,0U);
_tmp2C0->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2C0);{
# 1757
struct Cyc_List_List*_tmp2C1=({struct Cyc_Absyn_Exp*_tmp2CF[2U];({struct Cyc_Absyn_Exp*_tmp876=Cyc_Absyn_varb_exp((void*)_tmp2B1,0U);_tmp2CF[0]=_tmp876;}),({
struct Cyc_Absyn_Exp*_tmp875=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2CF[1]=_tmp875;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2CF,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2C2=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp878=({struct Cyc_Absyn_Exp*_tmp877=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2C1,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp877,_tmp2C2,0U);});xsize=_tmp878;});{
# 1764
struct Cyc_Absyn_Exp*_tmp2C3=({struct Cyc_Absyn_Exp*_tmp879=xsize;Cyc_Absyn_and_exp(_tmp879,Cyc_Absyn_and_exp(_tmp2BD,_tmp2C0,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C4=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2C5=({struct Cyc_Absyn_Exp*_tmp87A=Cyc_Absyn_varb_exp((void*)_tmp2B1,0U);Cyc_Toc_member_exp(_tmp87A,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp87B=Cyc_Toc_cast_it(_tmp2A4,_tmp2C5);_tmp2C5=_tmp87B;});{
struct Cyc_Absyn_Exp*_tmp2C6=Cyc_Absyn_deref_exp(_tmp2C5,0U);
struct Cyc_Absyn_Exp*_tmp2C7=({struct Cyc_Absyn_Exp*_tmp87C=_tmp2C6;Cyc_Absyn_assign_exp(_tmp87C,Cyc_Absyn_var_exp(_tmp2B8,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C8=Cyc_Absyn_exp_stmt(_tmp2C7,0U);
({struct Cyc_Absyn_Stmt*_tmp880=({struct Cyc_Absyn_Stmt*_tmp87F=({struct Cyc_Absyn_Exp*_tmp87E=_tmp2C3;struct Cyc_Absyn_Stmt*_tmp87D=_tmp2C4;Cyc_Absyn_ifthenelse_stmt(_tmp87E,_tmp87D,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp87F,_tmp2C8,0U);});_tmp2C8=_tmp880;});
({struct Cyc_Absyn_Stmt*_tmp883=({struct Cyc_Absyn_Decl*_tmp882=({struct Cyc_Absyn_Decl*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));({void*_tmp881=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9->tag=0U,_tmp2C9->f1=_tmp2B9;_tmp2C9;});_tmp2CA->r=_tmp881;}),_tmp2CA->loc=0U;_tmp2CA;});Cyc_Absyn_decl_stmt(_tmp882,_tmp2C8,0U);});_tmp2C8=_tmp883;});
({struct Cyc_Absyn_Stmt*_tmp886=({struct Cyc_Absyn_Decl*_tmp885=({struct Cyc_Absyn_Decl*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));({void*_tmp884=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->tag=0U,_tmp2CB->f1=_tmp2B5;_tmp2CB;});_tmp2CC->r=_tmp884;}),_tmp2CC->loc=0U;_tmp2CC;});Cyc_Absyn_decl_stmt(_tmp885,_tmp2C8,0U);});_tmp2C8=_tmp886;});
({struct Cyc_Absyn_Stmt*_tmp889=({struct Cyc_Absyn_Decl*_tmp888=({struct Cyc_Absyn_Decl*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));({void*_tmp887=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->tag=0U,_tmp2CD->f1=_tmp2B0;_tmp2CD;});_tmp2CE->r=_tmp887;}),_tmp2CE->loc=0U;_tmp2CE;});Cyc_Absyn_decl_stmt(_tmp888,_tmp2C8,0U);});_tmp2C8=_tmp889;});
({void*_tmp88A=Cyc_Toc_stmt_exp_r(_tmp2C8);e->r=_tmp88A;});};};};};};};};};};}
# 1722
;_pop_region(rgn2);};}
# 1790 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1794
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2D7=Cyc_Tcutil_compress(aggrtype);void*_tmp2D8=_tmp2D7;union Cyc_Absyn_AggrInfoU _tmp2DD;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D8)->tag == 11U){_LL1: _tmp2DD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D8)->f1).aggr_info;_LL2:
# 1797
({struct Cyc_Absyn_Aggrdecl*_tmp88B=Cyc_Absyn_get_known_aggrdecl(_tmp2DD);ad=_tmp88B;});goto _LL0;}else{_LL3: _LL4:
({void*_tmp2D9=0U;({struct _dyneither_ptr _tmp88E=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DC=({struct Cyc_String_pa_PrintArg_struct _tmp794;_tmp794.tag=0U,({
struct _dyneither_ptr _tmp88C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp794.f1=_tmp88C;});_tmp794;});void*_tmp2DA[1U];_tmp2DA[0]=& _tmp2DC;({struct _dyneither_ptr _tmp88D=({const char*_tmp2DB="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2DB,sizeof(char),51U);});Cyc_aprintf(_tmp88D,_tag_dyneither(_tmp2DA,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp88E,_tag_dyneither(_tmp2D9,sizeof(void*),0U));});});}_LL0:;}{
# 1801
struct _tuple1*_tmp2DE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2DF=Cyc_Absyn_var_exp(_tmp2DE,0U);
struct Cyc_Absyn_Exp*_tmp2E0=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2E1=Cyc_Absyn_aggrarrow_exp(_tmp2DF,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2E2=Cyc_Absyn_neq_exp(_tmp2E1,_tmp2E0,0U);
struct Cyc_Absyn_Exp*_tmp2E3=Cyc_Absyn_aggrarrow_exp(_tmp2DF,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2E4=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2E3,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E5=({struct Cyc_Absyn_Exp*_tmp890=_tmp2E2;struct Cyc_Absyn_Stmt*_tmp88F=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp890,_tmp88F,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2E6=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2E7=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E8=({struct _tuple1*_tmp893=_tmp2DE;void*_tmp892=_tmp2E6;struct Cyc_Absyn_Exp*_tmp891=_tmp2E7;Cyc_Absyn_declare_stmt(_tmp893,_tmp892,_tmp891,Cyc_Absyn_seq_stmt(_tmp2E5,_tmp2E4,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2E8);}else{
# 1815
struct Cyc_Absyn_Exp*_tmp2E9=({struct Cyc_Absyn_Exp*_tmp894=aggrproj(_tmp2DF,f,0U);Cyc_Toc_member_exp(_tmp894,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2EA=Cyc_Absyn_neq_exp(_tmp2E9,_tmp2E0,0U);
struct Cyc_Absyn_Exp*_tmp2EB=({struct Cyc_Absyn_Exp*_tmp895=aggrproj(_tmp2DF,f,0U);Cyc_Toc_member_exp(_tmp895,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2EC=Cyc_Absyn_exp_stmt(_tmp2EB,0U);
struct Cyc_Absyn_Stmt*_tmp2ED=({struct Cyc_Absyn_Exp*_tmp897=_tmp2EA;struct Cyc_Absyn_Stmt*_tmp896=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp897,_tmp896,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2EE=({struct _tuple1*_tmp89A=_tmp2DE;void*_tmp899=e1_c_type;struct Cyc_Absyn_Exp*_tmp898=e1;Cyc_Absyn_declare_stmt(_tmp89A,_tmp899,_tmp898,Cyc_Absyn_seq_stmt(_tmp2ED,_tmp2EC,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2EE);}};}
# 1825
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1828
void*_tmp2EF=Cyc_Tcutil_compress(t);void*_tmp2F0=_tmp2EF;union Cyc_Absyn_AggrInfoU _tmp2F7;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F0)->tag == 11U){_LL1: _tmp2F7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F0)->f1).aggr_info;_LL2: {
# 1830
struct Cyc_Absyn_Aggrdecl*_tmp2F1=Cyc_Absyn_get_known_aggrdecl(_tmp2F7);
({int _tmp89B=Cyc_Toc_get_member_offset(_tmp2F1,f);*f_tag=_tmp89B;});{
# 1833
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F5=({struct Cyc_String_pa_PrintArg_struct _tmp796;_tmp796.tag=0U,_tmp796.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2F1->name).f2);_tmp796;});struct Cyc_String_pa_PrintArg_struct _tmp2F6=({struct Cyc_String_pa_PrintArg_struct _tmp795;_tmp795.tag=0U,_tmp795.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp795;});void*_tmp2F3[2U];_tmp2F3[0]=& _tmp2F5,_tmp2F3[1]=& _tmp2F6;({struct _dyneither_ptr _tmp89C=({const char*_tmp2F4="_union_%s_%s";_tag_dyneither(_tmp2F4,sizeof(char),13U);});Cyc_aprintf(_tmp89C,_tag_dyneither(_tmp2F3,sizeof(void*),2U));});});
({void*_tmp89D=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));*_tmp2F2=str;_tmp2F2;}));*tagged_member_type=_tmp89D;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2F1->impl))->tagged;};}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1844
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1847
void*_tmp2F8=e->r;void*_tmp2F9=_tmp2F8;struct Cyc_Absyn_Exp*_tmp305;struct _dyneither_ptr*_tmp304;int*_tmp303;struct Cyc_Absyn_Exp*_tmp302;struct _dyneither_ptr*_tmp301;int*_tmp300;struct Cyc_Absyn_Exp*_tmp2FF;switch(*((int*)_tmp2F9)){case 14U: _LL1: _tmp2FF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F9)->f2;_LL2:
({void*_tmp2FA=0U;({struct _dyneither_ptr _tmp89E=({const char*_tmp2FB="cast on lhs!";_tag_dyneither(_tmp2FB,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp89E,_tag_dyneither(_tmp2FA,sizeof(void*),0U));});});case 21U: _LL3: _tmp302=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9)->f1;_tmp301=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9)->f2;_tmp300=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9)->f4;_LL4:
# 1850
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp302->topt),_tmp301,f_tag,tagged_member_type,clear_read,_tmp300);case 22U: _LL5: _tmp305=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9)->f1;_tmp304=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9)->f2;_tmp303=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9)->f4;_LL6: {
# 1853
void*_tmp2FC=Cyc_Tcutil_compress((void*)_check_null(_tmp305->topt));void*_tmp2FD=_tmp2FC;void*_tmp2FE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->tag == 5U){_LLA: _tmp2FE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_typ;_LLB:
# 1855
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2FE,_tmp304,f_tag,tagged_member_type,clear_read,_tmp303);}else{_LLC: _LLD:
# 1857
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1859
 return 0;}_LL0:;}
# 1872 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1876
struct _tuple1*_tmp306=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp306,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp89F=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp89F;});else{
# 1886
struct Cyc_Absyn_Exp*_tmp307=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp8A2=({struct Cyc_Absyn_Exp*_tmp8A1=_tmp307;struct Cyc_Absyn_Stmt*_tmp8A0=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8A1,_tmp8A0,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp8A2;});}{
# 1889
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8A5=_tmp306;void*_tmp8A4=Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8A3=
Cyc_Toc_push_address_exp(e1);
# 1889
Cyc_Absyn_declare_stmt(_tmp8A5,_tmp8A4,_tmp8A3,
# 1891
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple32{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1895
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp308=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp30B=({struct Cyc_String_pa_PrintArg_struct _tmp797;_tmp797.tag=0U,({struct _dyneither_ptr _tmp8A6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp797.f1=_tmp8A6;});_tmp797;});void*_tmp309[1U];_tmp309[0]=& _tmp30B;({struct _dyneither_ptr _tmp8A7=({const char*_tmp30A="exp_to_c: no type for %s";_tag_dyneither(_tmp30A,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8A7,_tag_dyneither(_tmp309,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp30C=_tmp308;struct Cyc_Absyn_Stmt*_tmp52D;struct Cyc_Absyn_Exp*_tmp52C;struct _dyneither_ptr*_tmp52B;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;int _tmp528;struct Cyc_Absyn_Exp*_tmp527;void**_tmp526;struct Cyc_Absyn_Exp*_tmp525;int _tmp524;int _tmp523;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_Datatypedecl*_tmp521;struct Cyc_Absyn_Datatypefield*_tmp520;struct _tuple1**_tmp51F;struct Cyc_List_List*_tmp51E;struct Cyc_List_List*_tmp51D;struct Cyc_Absyn_Aggrdecl*_tmp51C;void*_tmp51B;struct Cyc_List_List*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;void*_tmp518;int _tmp517;struct Cyc_Absyn_Vardecl*_tmp516;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp514;int _tmp513;struct Cyc_List_List*_tmp512;struct Cyc_List_List*_tmp511;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp50E;struct _dyneither_ptr*_tmp50D;int _tmp50C;int _tmp50B;struct Cyc_Absyn_Exp*_tmp50A;struct _dyneither_ptr*_tmp509;int _tmp508;int _tmp507;struct Cyc_Absyn_Exp*_tmp506;void*_tmp505;struct Cyc_List_List*_tmp504;void*_tmp503;struct Cyc_Absyn_Exp*_tmp502;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;void**_tmp4FE;struct Cyc_Absyn_Exp*_tmp4FD;int _tmp4FC;enum Cyc_Absyn_Coercion _tmp4FB;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_List_List*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F7;int _tmp4F6;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_List_List*_tmp4F4;int _tmp4F3;struct Cyc_List_List*_tmp4F2;struct Cyc_Absyn_VarargInfo*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_List_List*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Core_Opt*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E2;enum Cyc_Absyn_Incrementor _tmp4E1;enum Cyc_Absyn_Primop _tmp4E0;struct Cyc_List_List*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;switch(*((int*)_tmp30C)){case 2U: _LL1: _LL2:
# 1902
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1909
struct Cyc_Absyn_Exp*_tmp30D=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8A8=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp30D,_tmp30D))->r;e->r=_tmp8A8;});else{
# 1914
({void*_tmp8AA=({struct Cyc_Absyn_Exp*_tmp8A9=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp8A9,({struct Cyc_Absyn_Exp*_tmp30E[3U];_tmp30E[0]=_tmp30D,_tmp30E[1]=_tmp30D,_tmp30E[2]=_tmp30D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp30E,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp8AA;});}}else{
# 1916
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1918
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 41U: _LL9: _tmp4DE=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LLA:
 Cyc_Toc_exp_to_c(nv,_tmp4DE);goto _LL0;case 3U: _LLB: _tmp4E0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4DF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LLC: {
# 1924
struct Cyc_List_List*_tmp30F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4DF);
# 1926
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4DF);
{enum Cyc_Absyn_Primop _tmp310=_tmp4E0;switch(_tmp310){case Cyc_Absyn_Numelts: _LL5A: _LL5B: {
# 1929
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
{void*_tmp311=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp312=_tmp311;void*_tmp324;union Cyc_Absyn_Constraint*_tmp323;union Cyc_Absyn_Constraint*_tmp322;union Cyc_Absyn_Constraint*_tmp321;struct Cyc_Absyn_Exp*_tmp320;switch(*((int*)_tmp312)){case 8U: _LL6F: _tmp320=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312)->f1).num_elts;_LL70:
# 1933
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp320)))
({void*_tmp313=0U;({unsigned int _tmp8AC=e->loc;struct _dyneither_ptr _tmp8AB=({const char*_tmp314="can't calculate numelts";_tag_dyneither(_tmp314,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8AC,_tmp8AB,_tag_dyneither(_tmp313,sizeof(void*),0U));});});
e->r=_tmp320->r;goto _LL6E;case 5U: _LL71: _tmp324=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).elt_typ;_tmp323=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).nullable;_tmp322=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).bounds;_tmp321=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).zero_term;_LL72:
# 1937
{void*_tmp315=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp322);void*_tmp316=_tmp315;struct Cyc_Absyn_Exp*_tmp31B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp316)->tag == 0U){_LL76: _LL77:
# 1939
({void*_tmp8AF=({struct Cyc_Absyn_Exp*_tmp8AE=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp8AE,({struct Cyc_Absyn_Exp*_tmp317[2U];_tmp317[0]=(struct Cyc_Absyn_Exp*)_tmp4DF->hd,({
# 1941
struct Cyc_Absyn_Exp*_tmp8AD=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp324),0U);_tmp317[1]=_tmp8AD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp317,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1939
e->r=_tmp8AF;});
# 1942
goto _LL75;}else{_LL78: _tmp31B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp316)->f1;_LL79:
# 1945
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp321)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4DF->hd);
# 1949
({void*_tmp8B1=({struct Cyc_Absyn_Exp*_tmp8B0=function_e;Cyc_Toc_fncall_exp_r(_tmp8B0,({struct Cyc_Absyn_Exp*_tmp318[2U];_tmp318[0]=(struct Cyc_Absyn_Exp*)_tmp4DF->hd,_tmp318[1]=_tmp31B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp318,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8B1;});}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp323)){
if(!Cyc_Evexp_c_can_eval(_tmp31B))
({void*_tmp319=0U;({unsigned int _tmp8B3=e->loc;struct _dyneither_ptr _tmp8B2=({const char*_tmp31A="can't calculate numelts";_tag_dyneither(_tmp31A,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8B3,_tmp8B2,_tag_dyneither(_tmp319,sizeof(void*),0U));});});
# 1954
({void*_tmp8B6=({struct Cyc_Absyn_Exp*_tmp8B5=arg;struct Cyc_Absyn_Exp*_tmp8B4=_tmp31B;Cyc_Toc_conditional_exp_r(_tmp8B5,_tmp8B4,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8B6;});}else{
# 1956
e->r=_tmp31B->r;goto _LL75;}}
# 1958
goto _LL75;}_LL75:;}
# 1960
goto _LL6E;default: _LL73: _LL74:
# 1962
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp799;_tmp799.tag=0U,({
struct _dyneither_ptr _tmp8B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp799.f1=_tmp8B7;});_tmp799;});struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp798;_tmp798.tag=0U,({struct _dyneither_ptr _tmp8B8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp798.f1=_tmp8B8;});_tmp798;});void*_tmp31C[2U];_tmp31C[0]=& _tmp31E,_tmp31C[1]=& _tmp31F;({struct _dyneither_ptr _tmp8B9=({const char*_tmp31D="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp31D,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8B9,_tag_dyneither(_tmp31C,sizeof(void*),2U));});});}_LL6E:;}
# 1965
goto _LL59;}case Cyc_Absyn_Plus: _LL5C: _LL5D:
# 1970
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp325=0U;({struct _dyneither_ptr _tmp8BA=({const char*_tmp326="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp326,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp8BA,_tag_dyneither(_tmp325,sizeof(void*),0U));});});
{void*_tmp327=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd);void*_tmp328=_tmp327;void*_tmp330;union Cyc_Absyn_Constraint*_tmp32F;union Cyc_Absyn_Constraint*_tmp32E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->tag == 5U){_LL7B: _tmp330=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).elt_typ;_tmp32F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).ptr_atts).bounds;_tmp32E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).ptr_atts).zero_term;_LL7C:
# 1974
{void*_tmp329=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp32F);void*_tmp32A=_tmp329;struct Cyc_Absyn_Exp*_tmp32D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp32A)->tag == 0U){_LL80: _LL81: {
# 1976
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF->tl))->hd;
({void*_tmp8BD=({struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8BC,({struct Cyc_Absyn_Exp*_tmp32B[3U];_tmp32B[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp330),0U);_tmp32B[1]=_tmp8BB;}),_tmp32B[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32B,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1978
e->r=_tmp8BD;});
# 1980
goto _LL7F;}}else{_LL82: _tmp32D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp32A)->f1;_LL83:
# 1982
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp32E)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF->tl))->hd;
({void*_tmp8BF=({struct Cyc_Absyn_Exp*_tmp8BE=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp8BE,({struct Cyc_Absyn_Exp*_tmp32C[3U];_tmp32C[0]=e1,_tmp32C[1]=_tmp32D,_tmp32C[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp8BF;});}
# 1987
goto _LL7F;}_LL7F:;}
# 1989
goto _LL7A;}else{_LL7D: _LL7E:
 goto _LL7A;}_LL7A:;}
# 1992
goto _LL59;case Cyc_Absyn_Minus: _LL5E: _LL5F: {
# 1997
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp30F->tl))->hd)){
({void*_tmp8C1=({struct Cyc_Absyn_Exp*_tmp8C0=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8C0,Cyc_Toc_curr_sp);});e1->r=_tmp8C1;});
({void*_tmp8C3=({struct Cyc_Absyn_Exp*_tmp8C2=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8C2,Cyc_Toc_curr_sp);});e2->r=_tmp8C3;});
({void*_tmp8C5=({void*_tmp8C4=Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_typ,Cyc_Toc_mt_tq);e2->topt=_tmp8C4;});e1->topt=_tmp8C5;});
({void*_tmp8C7=({struct Cyc_Absyn_Exp*_tmp8C6=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8C6,
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U),0U);})->r;
# 2005
e->r=_tmp8C7;});}else{
# 2008
({void*_tmp8CB=({struct Cyc_Absyn_Exp*_tmp8CA=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8CA,({struct Cyc_Absyn_Exp*_tmp331[3U];_tmp331[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8C9=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp331[1]=_tmp8C9;}),({
struct Cyc_Absyn_Exp*_tmp8C8=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp331[2]=_tmp8C8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp331,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2008
e->r=_tmp8CB;});}}
# 2013
goto _LL59;}case Cyc_Absyn_Eq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Neq: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gt: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Gte: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lt: _LL68: _LL69:
 goto _LL6B;case Cyc_Absyn_Lte: _LL6A: _LL6B: {
# 2021
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp30F->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ)){
void*_tmp332=({void*_tmp8CC=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8CC,Cyc_Toc_mt_tq);});
({void*_tmp8CF=({void*_tmp8CE=_tmp332;Cyc_Toc_cast_it_r(_tmp8CE,({struct Cyc_Absyn_Exp*_tmp8CD=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8CD,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8CF;});
e1->topt=_tmp332;}
# 2031
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
void*_tmp333=({void*_tmp8D0=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8D0,Cyc_Toc_mt_tq);});
({void*_tmp8D3=({void*_tmp8D2=_tmp333;Cyc_Toc_cast_it_r(_tmp8D2,({struct Cyc_Absyn_Exp*_tmp8D1=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8D1,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8D3;});
e2->topt=_tmp333;}
# 2036
goto _LL59;}default: _LL6C: _LL6D:
 goto _LL59;}_LL59:;}
# 2039
goto _LL0;}case 5U: _LLD: _tmp4E2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LLE: {
# 2041
void*e2_cyc_typ=(void*)_check_null(_tmp4E2->topt);
# 2050 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp34D="increment";_tag_dyneither(_tmp34D,sizeof(char),10U);});
if(_tmp4E1 == Cyc_Absyn_PreDec  || _tmp4E1 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp8D4=({const char*_tmp334="decrement";_tag_dyneither(_tmp334,sizeof(char),10U);});incr_str=_tmp8D4;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E2,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp337=({struct Cyc_String_pa_PrintArg_struct _tmp79A;_tmp79A.tag=0U,_tmp79A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp79A;});void*_tmp335[1U];_tmp335[0]=& _tmp337;({unsigned int _tmp8D6=e->loc;struct _dyneither_ptr _tmp8D5=({const char*_tmp336="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp336,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp8D6,_tmp8D5,_tag_dyneither(_tmp335,sizeof(void*),1U));});});
({void*_tmp338=0U;({struct _dyneither_ptr _tmp8D7=({const char*_tmp339="in-place inc/dec on zero-term";_tag_dyneither(_tmp339,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D7,_tag_dyneither(_tmp338,sizeof(void*),0U));});});}{
# 2059
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4E2,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp33A=Cyc_Absyn_signed_int_exp(1,0U);
_tmp33A->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp33B=_tmp4E1;switch(_tmp33B){case Cyc_Absyn_PreInc: _LL85: _LL86:
# 2066
({void*_tmp8D9=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->tag=4U,_tmp33D->f1=_tmp4E2,({struct Cyc_Core_Opt*_tmp8D8=({struct Cyc_Core_Opt*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->v=(void*)Cyc_Absyn_Plus;_tmp33C;});_tmp33D->f2=_tmp8D8;}),_tmp33D->f3=_tmp33A;_tmp33D;});e->r=_tmp8D9;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL87: _LL88:
# 2070
({void*_tmp8DB=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F->tag=4U,_tmp33F->f1=_tmp4E2,({struct Cyc_Core_Opt*_tmp8DA=({struct Cyc_Core_Opt*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E->v=(void*)Cyc_Absyn_Minus;_tmp33E;});_tmp33F->f2=_tmp8DA;}),_tmp33F->f3=_tmp33A;_tmp33F;});e->r=_tmp8DB;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL89: _LL8A:
# 2074
({struct Cyc_String_pa_PrintArg_struct _tmp342=({struct Cyc_String_pa_PrintArg_struct _tmp79B;_tmp79B.tag=0U,_tmp79B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp79B;});void*_tmp340[1U];_tmp340[0]=& _tmp342;({unsigned int _tmp8DD=e->loc;struct _dyneither_ptr _tmp8DC=({const char*_tmp341="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp341,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp8DD,_tmp8DC,_tag_dyneither(_tmp340,sizeof(void*),1U));});});
# 2076
({void*_tmp343=0U;({struct _dyneither_ptr _tmp8DE=({const char*_tmp344="in-place inc/dec on @tagged union";_tag_dyneither(_tmp344,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8DE,_tag_dyneither(_tmp343,sizeof(void*),0U));});});}_LL84:;};}{
# 2079
int need_null_ck=(Cyc_Tcutil_is_pointer_type(old_typ) && 
Cyc_Tcutil_is_nullable_pointer_type(old_typ)) && Cyc_Toc_need_null_check(_tmp4E2);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E2);
Cyc_Toc_set_lhs(nv,0);{
# 2086
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4E1 == Cyc_Absyn_PostInc  || _tmp4E1 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2092
if(_tmp4E1 == Cyc_Absyn_PreDec  || _tmp4E1 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp8E3=({struct Cyc_Absyn_Exp*_tmp8E2=fn_e;Cyc_Toc_fncall_exp_r(_tmp8E2,({struct Cyc_Absyn_Exp*_tmp345[3U];({struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Toc_push_address_exp(_tmp4E2);_tmp345[0]=_tmp8E1;}),({
struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp345[1]=_tmp8E0;}),({
struct Cyc_Absyn_Exp*_tmp8DF=Cyc_Absyn_signed_int_exp(change,0U);_tmp345[2]=_tmp8DF;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp345,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2094
e->r=_tmp8E3;});}else{
# 2097
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2103
if(_tmp4E1 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp346=Cyc_Toc_temp_var();
void*_tmp347=({void*_tmp8E4=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_typ(_tmp8E4,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp348=Cyc_Toc_push_address_exp(_tmp4E2);
struct Cyc_Absyn_Exp*_tmp349=({struct Cyc_Absyn_Exp*_tmp8E5=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp346,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp8E5,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(need_null_ck)
({struct Cyc_Absyn_Exp*_tmp8E8=({struct Cyc_Absyn_Exp*_tmp8E7=({struct Cyc_Absyn_Exp*_tmp8E6=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp346,0U),0U);Cyc_Absyn_neq_exp(_tmp8E6,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2110
Cyc_Absyn_and_exp(_tmp8E7,_tmp349,0U);});_tmp349=_tmp8E8;});{
# 2112
struct Cyc_Absyn_Stmt*_tmp34A=({struct Cyc_Absyn_Exp*_tmp8EA=_tmp349;struct Cyc_Absyn_Stmt*_tmp8E9=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp346,0U),0U),0U),0U);
# 2112
Cyc_Absyn_ifthenelse_stmt(_tmp8EA,_tmp8E9,
# 2114
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp8EC=({struct Cyc_Absyn_Stmt*_tmp8EB=_tmp34A;Cyc_Absyn_seq_stmt(_tmp8EB,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp346,0U),0U),0U),0U);});_tmp34A=_tmp8EC;});
({void*_tmp8ED=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp346,_tmp347,_tmp348,_tmp34A,0U),0U))->r;e->r=_tmp8ED;});};}else{
# 2119
struct Cyc_Toc_functionSet*_tmp34B=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp34B,_tmp4E2);
({void*_tmp8F1=({struct Cyc_Absyn_Exp*_tmp8F0=fn_e;Cyc_Toc_fncall_exp_r(_tmp8F0,({struct Cyc_Absyn_Exp*_tmp34C[2U];({struct Cyc_Absyn_Exp*_tmp8EF=Cyc_Toc_push_address_exp(_tmp4E2);_tmp34C[0]=_tmp8EF;}),({
struct Cyc_Absyn_Exp*_tmp8EE=Cyc_Absyn_signed_int_exp(1,0U);_tmp34C[1]=_tmp8EE;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34C,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2121
e->r=_tmp8F1;});}}else{
# 2124
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4E2)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4E2,0,Cyc_Toc_incr_lvalue,_tmp4E1);
e->r=_tmp4E2->r;}}}
# 2128
goto _LL0;};};};}case 4U: _LLF: _tmp4E5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4E3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL10: {
# 2147 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4E5->topt);
void*e2_old_typ=(void*)_check_null(_tmp4E3->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4E5,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E5);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E3);
({void*_tmp8F2=Cyc_Toc_tagged_union_assignop(_tmp4E5,e1_old_typ,_tmp4E4,_tmp4E3,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp8F2;});
# 2158
return;}{
# 2160
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E5,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4E5,_tmp4E4,_tmp4E3,ptr_type,is_dyneither,elt_type);
# 2166
return;}{
# 2170
int e1_poly=Cyc_Toc_is_poly_project(_tmp4E5);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E5);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E3);{
# 2176
int done=0;
if(_tmp4E4 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp34E=(enum Cyc_Absyn_Primop)_tmp4E4->v;enum Cyc_Absyn_Primop _tmp34F=_tmp34E;switch(_tmp34F){case Cyc_Absyn_Plus: _LL8C: _LL8D:
 change=_tmp4E3;goto _LL8B;case Cyc_Absyn_Minus: _LL8E: _LL8F:
# 2184
({struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4E3,0U);change=_tmp8F3;});goto _LL8B;default: _LL90: _LL91:
({void*_tmp350=0U;({struct _dyneither_ptr _tmp8F4=({const char*_tmp351="bad t ? pointer arithmetic";_tag_dyneither(_tmp351,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F4,_tag_dyneither(_tmp350,sizeof(void*),0U));});});}_LL8B:;}
# 2187
done=1;{
# 2189
struct Cyc_Absyn_Exp*_tmp352=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp8F8=({struct Cyc_Absyn_Exp*_tmp8F7=_tmp352;Cyc_Toc_fncall_exp_r(_tmp8F7,({struct Cyc_Absyn_Exp*_tmp353[3U];({struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Toc_push_address_exp(_tmp4E5);_tmp353[0]=_tmp8F6;}),({
struct Cyc_Absyn_Exp*_tmp8F5=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp353[1]=_tmp8F5;}),_tmp353[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp353,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2190
e->r=_tmp8F8;});};}else{
# 2193
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2196
enum Cyc_Absyn_Primop _tmp354=(enum Cyc_Absyn_Primop)_tmp4E4->v;enum Cyc_Absyn_Primop _tmp355=_tmp354;if(_tmp355 == Cyc_Absyn_Plus){_LL93: _LL94:
# 2198
 done=1;
({void*_tmp8FA=({struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4E5);Cyc_Toc_fncall_exp_r(_tmp8F9,({struct Cyc_Absyn_Exp*_tmp356[2U];_tmp356[0]=_tmp4E5,_tmp356[1]=_tmp4E3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp356,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8FA;});
goto _LL92;}else{_LL95: _LL96:
({void*_tmp357=0U;({struct _dyneither_ptr _tmp8FB=({const char*_tmp358="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp358,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8FB,_tag_dyneither(_tmp357,sizeof(void*),0U));});});}_LL92:;}}}
# 2205
if(!done){
# 2207
if(e1_poly)
({void*_tmp8FD=({void*_tmp8FC=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmp8FC,Cyc_Absyn_new_exp(_tmp4E3->r,0U));});_tmp4E3->r=_tmp8FD;});
# 2213
if(!Cyc_Absyn_is_lvalue(_tmp4E5)){
({struct Cyc_Absyn_Exp*_tmp8FE=_tmp4E5;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp8FE,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp35A=1;struct _tuple31*_tmp359=_cycalloc(_check_times(_tmp35A,sizeof(struct _tuple31)));(_tmp359[0]).f1=_tmp4E4,(_tmp359[0]).f2=_tmp4E3;_tmp359;}));});
e->r=_tmp4E5->r;}}
# 2218
goto _LL0;};};};}case 6U: _LL11: _tmp4E8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4E6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL12:
# 2220
 Cyc_Toc_exp_to_c(nv,_tmp4E8);
Cyc_Toc_exp_to_c(nv,_tmp4E7);
Cyc_Toc_exp_to_c(nv,_tmp4E6);
goto _LL0;case 7U: _LL13: _tmp4EA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL14:
 _tmp4EC=_tmp4EA;_tmp4EB=_tmp4E9;goto _LL16;case 8U: _LL15: _tmp4EC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4EB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL16:
 _tmp4EE=_tmp4EC;_tmp4ED=_tmp4EB;goto _LL18;case 9U: _LL17: _tmp4EE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4ED=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL18:
# 2227
 Cyc_Toc_exp_to_c(nv,_tmp4EE);
Cyc_Toc_exp_to_c(nv,_tmp4ED);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3 == 0){_LL19: _tmp4F0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4EF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL1A:
# 2231
 Cyc_Toc_exp_to_c(nv,_tmp4F0);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4EF);
goto _LL0;}else{_LL1B: _tmp4F5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4F4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4F3=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3)->num_varargs;_tmp4F2=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3)->injectors;_tmp4F1=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3)->vai;_LL1C: {
# 2243 "toc.cyc"
struct _RegionHandle _tmp35B=_new_region("r");struct _RegionHandle*r=& _tmp35B;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4F3,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4F1->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0U);
# 2251
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F4);
int num_normargs=num_args - _tmp4F3;
# 2255
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4F4=_tmp4F4->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4F4))->hd);
({struct Cyc_List_List*_tmp8FF=({struct Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C->hd=(struct Cyc_Absyn_Exp*)_tmp4F4->hd,_tmp35C->tl=new_args;_tmp35C;});new_args=_tmp8FF;});}}
# 2260
({struct Cyc_List_List*_tmp903=({struct Cyc_List_List*_tmp35E=_cycalloc(sizeof(*_tmp35E));({struct Cyc_Absyn_Exp*_tmp902=({struct Cyc_Absyn_Exp*_tmp901=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp901,({struct Cyc_Absyn_Exp*_tmp35D[3U];_tmp35D[0]=argvexp,({
# 2262
struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_sizeoftyp_exp(cva_type,0U);_tmp35D[1]=_tmp900;}),_tmp35D[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp35D,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2260
_tmp35E->hd=_tmp902;}),_tmp35E->tl=new_args;_tmp35E;});new_args=_tmp903;});
# 2265
({struct Cyc_List_List*_tmp904=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp904;});
# 2267
Cyc_Toc_exp_to_c(nv,_tmp4F5);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4F5,new_args,0U),0U);
# 2271
if(_tmp4F1->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp35F=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4F1->type));void*_tmp360=_tmp35F;struct Cyc_Absyn_Datatypedecl*_tmp363;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp360)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp360)->f1).datatype_info).KnownDatatype).tag == 2){_LL98: _tmp363=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp360)->f1).datatype_info).KnownDatatype).val;_LL99:
 tud=_tmp363;goto _LL97;}else{goto _LL9A;}}else{_LL9A: _LL9B:
({void*_tmp361=0U;({struct _dyneither_ptr _tmp905=({const char*_tmp362="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp362,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp905,_tag_dyneither(_tmp361,sizeof(void*),0U));});});}_LL97:;}{
# 2277
struct _dyneither_ptr vs=({unsigned int _tmp374=(unsigned int)_tmp4F3;struct _tuple1**_tmp373=({struct _RegionHandle*_tmp906=r;_region_malloc(_tmp906,_check_times(_tmp374,sizeof(struct _tuple1*)));});({{unsigned int _tmp79C=(unsigned int)_tmp4F3;unsigned int i;for(i=0;i < _tmp79C;++ i){({struct _tuple1*_tmp907=Cyc_Toc_temp_var();_tmp373[i]=_tmp907;});}}0;});_tag_dyneither(_tmp373,sizeof(struct _tuple1*),_tmp374);});
# 2279
if(_tmp4F3 != 0){
# 2283
struct Cyc_List_List*_tmp364=0;
{int i=_tmp4F3 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp909=({struct Cyc_List_List*_tmp365=_cycalloc(sizeof(*_tmp365));({struct Cyc_Absyn_Exp*_tmp908=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp365->hd=_tmp908;}),_tmp365->tl=_tmp364;_tmp365;});_tmp364=_tmp909;});}}
({struct Cyc_Absyn_Stmt*_tmp90D=({struct _tuple1*_tmp90C=argv;void*_tmp90B=arr_type;struct Cyc_Absyn_Exp*_tmp90A=Cyc_Absyn_array_exp(_tmp364,0U);Cyc_Absyn_declare_stmt(_tmp90C,_tmp90B,_tmp90A,s,0U);});s=_tmp90D;});
# 2288
({struct Cyc_List_List*_tmp90E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4F4);_tmp4F4=_tmp90E;});
({struct Cyc_List_List*_tmp90F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4F2);_tmp4F2=_tmp90F;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F4)- 1;
for(0;_tmp4F4 != 0;(_tmp4F4=_tmp4F4->tl,_tmp4F2=_tmp4F2->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4F4->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp366=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4F2))->hd;struct Cyc_Absyn_Datatypefield*_tmp367=_tmp366;struct _tuple1*_tmp36D;struct Cyc_List_List*_tmp36C;_LL9D: _tmp36D=_tmp367->name;_tmp36C=_tmp367->typs;_LL9E:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp36C))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp910=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp910;});{
# 2302
struct _tuple1*_tmp368=Cyc_Toc_collapse_qvars(_tmp36D,tud->name);
struct Cyc_List_List*_tmp369=({struct _tuple20*_tmp36B[2U];({struct _tuple20*_tmp914=({struct _dyneither_ptr*_tmp913=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp913,Cyc_Toc_datatype_tag(tud,_tmp36D));});_tmp36B[0]=_tmp914;}),({
struct _tuple20*_tmp912=({struct _dyneither_ptr*_tmp911=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp911,arg);});_tmp36B[1]=_tmp912;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36B,sizeof(struct _tuple20*),2U));});
({struct Cyc_Absyn_Stmt*_tmp918=({struct _tuple1*_tmp917=var;void*_tmp916=Cyc_Absyn_strctq(_tmp368);struct Cyc_Absyn_Exp*_tmp915=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->tag=29U,_tmp36A->f1=_tmp368,_tmp36A->f2=0,_tmp36A->f3=_tmp369,_tmp36A->f4=0;_tmp36A;}),0U);
# 2305
Cyc_Absyn_declare_stmt(_tmp917,_tmp916,_tmp915,s,0U);});s=_tmp918;});};};}};}else{
# 2313
struct Cyc_List_List*_tmp36E=({struct _tuple32*_tmp36F[3U];({struct _tuple32*_tmp921=({struct _tuple32*_tmp370=_cycalloc(sizeof(*_tmp370));({struct _dyneither_ptr _tmp920=_tag_dyneither(0,0,0);_tmp370->f1=_tmp920;}),({struct Cyc_Absyn_Exp*_tmp91F=Cyc_Absyn_uint_exp(0U,0U);_tmp370->f2=_tmp91F;});_tmp370;});_tmp36F[0]=_tmp921;}),({
struct _tuple32*_tmp91E=({struct _tuple32*_tmp371=_cycalloc(sizeof(*_tmp371));({struct _dyneither_ptr _tmp91D=_tag_dyneither(0,0,0);_tmp371->f1=_tmp91D;}),({struct Cyc_Absyn_Exp*_tmp91C=Cyc_Absyn_uint_exp(0U,0U);_tmp371->f2=_tmp91C;});_tmp371;});_tmp36F[1]=_tmp91E;}),({
struct _tuple32*_tmp91B=({struct _tuple32*_tmp372=_cycalloc(sizeof(*_tmp372));({struct _dyneither_ptr _tmp91A=_tag_dyneither(0,0,0);_tmp372->f1=_tmp91A;}),({struct Cyc_Absyn_Exp*_tmp919=Cyc_Absyn_uint_exp(0U,0U);_tmp372->f2=_tmp919;});_tmp372;});_tmp36F[2]=_tmp91B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36F,sizeof(struct _tuple32*),3U));});
({struct Cyc_Absyn_Stmt*_tmp925=({struct _tuple1*_tmp924=argv;void*_tmp923=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp922=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp924,_tmp923,_tmp922,s,0U);});s=_tmp925;});}};}else{
# 2321
if(_tmp4F3 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp4F4 != 0;_tmp4F4=_tmp4F4->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F4->hd);
({struct Cyc_List_List*_tmp926=({struct Cyc_List_List*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->hd=(struct Cyc_Absyn_Exp*)_tmp4F4->hd,_tmp375->tl=array_args;_tmp375;});array_args=_tmp926;});}{
# 2327
struct Cyc_Absyn_Exp*_tmp376=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp927=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp376,s,0U);s=_tmp927;});};}else{
# 2330
({struct Cyc_Absyn_Stmt*_tmp92B=({struct _tuple1*_tmp92A=argv;void*_tmp929=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp928=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp92A,_tmp929,_tmp928,s,0U);});s=_tmp92B;});}}
# 2333
({void*_tmp92C=Cyc_Toc_stmt_exp_r(s);e->r=_tmp92C;});};}
# 2335
_npop_handler(0U);goto _LL0;
# 2243
;_pop_region(r);}}case 11U: _LL1D: _tmp4F7=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4F6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL1E:
# 2338
 Cyc_Toc_exp_to_c(nv,_tmp4F7);{
struct Cyc_Absyn_Exp*_tmp377=_tmp4F6?Cyc_Toc_newrethrow_exp(_tmp4F7): Cyc_Toc_newthrow_exp(_tmp4F7);
({void*_tmp92E=({void*_tmp92D=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp92D,_tmp377,0U);})->r;e->r=_tmp92E;});
goto _LL0;};case 12U: _LL1F: _tmp4F8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL20:
 Cyc_Toc_exp_to_c(nv,_tmp4F8);goto _LL0;case 13U: _LL21: _tmp4FA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4F9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL22:
# 2344
 Cyc_Toc_exp_to_c(nv,_tmp4FA);
# 2353 "toc.cyc"
for(0;_tmp4F9 != 0;_tmp4F9=_tmp4F9->tl){
enum Cyc_Absyn_KindQual _tmp378=(Cyc_Tcutil_typ_kind((void*)_tmp4F9->hd))->kind;
if(_tmp378 != Cyc_Absyn_EffKind  && _tmp378 != Cyc_Absyn_RgnKind){
{void*_tmp379=Cyc_Tcutil_compress((void*)_tmp4F9->hd);void*_tmp37A=_tmp379;switch(*((int*)_tmp37A)){case 2U: _LLA0: _LLA1:
 goto _LLA3;case 3U: _LLA2: _LLA3:
 continue;default: _LLA4: _LLA5:
# 2360
({void*_tmp930=({void*_tmp92F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp92F,_tmp4FA,0U);})->r;e->r=_tmp930;});
goto _LL9F;}_LL9F:;}
# 2363
break;}}
# 2366
goto _LL0;case 14U: _LL23: _tmp4FE=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4FD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4FC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp4FB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL24: {
# 2368
void*old_t2=(void*)_check_null(_tmp4FD->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4FE;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4FE=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4FD);
# 2375
{struct _tuple33 _tmp37B=({struct _tuple33 _tmp7A0;({void*_tmp932=Cyc_Tcutil_compress(old_t2);_tmp7A0.f1=_tmp932;}),({void*_tmp931=Cyc_Tcutil_compress(new_typ);_tmp7A0.f2=_tmp931;});_tmp7A0;});struct _tuple33 _tmp37C=_tmp37B;struct Cyc_Absyn_PtrInfo _tmp3BA;struct Cyc_Absyn_PtrInfo _tmp3B9;struct Cyc_Absyn_PtrInfo _tmp3B8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C.f1)->tag == 5U)switch(*((int*)_tmp37C.f2)){case 5U: _LLA7: _tmp3B9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C.f1)->f1;_tmp3B8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C.f2)->f1;_LLA8: {
# 2377
int _tmp37D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B9.ptr_atts).nullable);
int _tmp37E=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B8.ptr_atts).nullable);
void*_tmp37F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B9.ptr_atts).bounds);
void*_tmp380=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B8.ptr_atts).bounds);
int _tmp381=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B9.ptr_atts).zero_term);
int _tmp382=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B8.ptr_atts).zero_term);
{struct _tuple33 _tmp383=({struct _tuple33 _tmp79F;_tmp79F.f1=_tmp37F,_tmp79F.f2=_tmp380;_tmp79F;});struct _tuple33 _tmp384=_tmp383;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B2;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f2)->tag == 1U){_LLAE: _tmp3B3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f1)->f1;_tmp3B2=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f2)->f1;_LLAF:
# 2385
 if((!Cyc_Evexp_c_can_eval(_tmp3B3) || !Cyc_Evexp_c_can_eval(_tmp3B2)) && !
Cyc_Evexp_same_const_exp(_tmp3B3,_tmp3B2))
({void*_tmp385=0U;({unsigned int _tmp934=e->loc;struct _dyneither_ptr _tmp933=({const char*_tmp386="can't validate cast due to potential size differences";_tag_dyneither(_tmp386,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp934,_tmp933,_tag_dyneither(_tmp385,sizeof(void*),0U));});});
if(_tmp37D  && !_tmp37E){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp387=0U;({struct _dyneither_ptr _tmp935=({const char*_tmp388="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp388,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp935,_tag_dyneither(_tmp387,sizeof(void*),0U));});});
# 2394
if(_tmp4FB != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp38B=({struct Cyc_String_pa_PrintArg_struct _tmp79D;_tmp79D.tag=0U,({struct _dyneither_ptr _tmp936=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp79D.f1=_tmp936;});_tmp79D;});void*_tmp389[1U];_tmp389[0]=& _tmp38B;({struct _dyneither_ptr _tmp937=({const char*_tmp38A="null-check conversion mis-classified: %s";_tag_dyneither(_tmp38A,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp937,_tag_dyneither(_tmp389,sizeof(void*),1U));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp4FD);
if(do_null_check){
if(!_tmp4FC)
({void*_tmp38C=0U;({unsigned int _tmp939=e->loc;struct _dyneither_ptr _tmp938=({const char*_tmp38D="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp38D,sizeof(char),58U);});Cyc_Tcutil_warn(_tmp939,_tmp938,_tag_dyneither(_tmp38C,sizeof(void*),0U));});});
# 2401
({void*_tmp93C=({void*_tmp93B=*_tmp4FE;Cyc_Toc_cast_it_r(_tmp93B,({struct Cyc_Absyn_Exp*_tmp93A=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp93A,({struct Cyc_List_List*_tmp38E=_cycalloc(sizeof(*_tmp38E));
_tmp38E->hd=_tmp4FD,_tmp38E->tl=0;_tmp38E;}),0U);}));});
# 2401
e->r=_tmp93C;});}else{
# 2405
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4FD->r;}};}else{
# 2410
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4FD->r;}
# 2417
goto _LLAD;}else{_LLB0: _tmp3B4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f1)->f1;_LLB1:
# 2419
 if(!Cyc_Evexp_c_can_eval(_tmp3B4))
({void*_tmp38F=0U;({unsigned int _tmp93E=e->loc;struct _dyneither_ptr _tmp93D=({const char*_tmp390="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp390,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp93E,_tmp93D,_tag_dyneither(_tmp38F,sizeof(void*),0U));});});{
struct _tuple13 _tmp391=Cyc_Evexp_eval_const_uint_exp(_tmp3B4);struct _tuple13 _tmp392=_tmp391;unsigned int _tmp3A4;int _tmp3A3;_LLB7: _tmp3A4=_tmp392.f1;_tmp3A3=_tmp392.f2;_LLB8:;
# 2423
if(_tmp4FB == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp395=({struct Cyc_String_pa_PrintArg_struct _tmp79E;_tmp79E.tag=0U,({struct _dyneither_ptr _tmp93F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp79E.f1=_tmp93F;});_tmp79E;});void*_tmp393[1U];_tmp393[0]=& _tmp395;({struct _dyneither_ptr _tmp940=({const char*_tmp394="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp394,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp940,_tag_dyneither(_tmp393,sizeof(void*),1U));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2427
if((_tmp381  && !(_tmp3B8.elt_tq).real_const) && !_tmp382)
# 2430
({struct Cyc_Absyn_Exp*_tmp942=({struct Cyc_Absyn_Exp*_tmp941=_tmp3B4;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp941,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3B4=_tmp942;});
# 2432
({void*_tmp943=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp3B4,_tmp4FD))->r;e->r=_tmp943;});}else{
# 2434
struct Cyc_Absyn_Exp*_tmp396=Cyc_Toc__tag_dyneither_e;
# 2436
if(_tmp381){
# 2441
struct _tuple1*_tmp397=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp398=Cyc_Absyn_var_exp(_tmp397,0U);
struct Cyc_Absyn_Exp*arg3;
# 2446
{void*_tmp399=_tmp4FD->r;void*_tmp39A=_tmp399;struct Cyc_Absyn_Vardecl*_tmp39D;struct Cyc_Absyn_Vardecl*_tmp39C;switch(*((int*)_tmp39A)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp39A)->f1).Wstring_c).tag){case 8U: _LLBA: _LLBB:
# 2448
 arg3=_tmp3B4;goto _LLB9;case 9U: _LLBC: _LLBD:
# 2450
 arg3=_tmp3B4;goto _LLB9;default: goto _LLC2;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39A)->f1)){case 1U: _LLBE: _tmp39C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39A)->f1)->f1;_LLBF:
 _tmp39D=_tmp39C;goto _LLC1;case 4U: _LLC0: _tmp39D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39A)->f1)->f1;_LLC1:
# 2453
 if(!Cyc_Tcutil_is_array(_tmp39D->type))goto _LLC3;
arg3=_tmp3B4;
goto _LLB9;default: goto _LLC2;}default: _LLC2: _LLC3:
# 2459
 if(_tmp3A3  && _tmp3A4 != 1)
arg3=_tmp3B4;else{
# 2462
({struct Cyc_Absyn_Exp*_tmp947=({struct Cyc_Absyn_Exp*_tmp946=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4FD);Cyc_Absyn_fncall_exp(_tmp946,({struct Cyc_Absyn_Exp*_tmp39B[2U];({
# 2464
struct Cyc_Absyn_Exp*_tmp945=({void*_tmp944=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp944,_tmp398);});_tmp39B[0]=_tmp945;}),_tmp39B[1]=_tmp3B4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39B,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2462
arg3=_tmp947;});}
# 2465
goto _LLB9;}_LLB9:;}
# 2467
if(!_tmp382  && !(_tmp3B8.elt_tq).real_const)
# 2470
({struct Cyc_Absyn_Exp*_tmp949=({struct Cyc_Absyn_Exp*_tmp948=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp948,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp949;});{
# 2472
struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B8.elt_typ),0U);
struct Cyc_Absyn_Exp*_tmp39F=({struct Cyc_Absyn_Exp*_tmp94A=_tmp396;Cyc_Absyn_fncall_exp(_tmp94A,({struct Cyc_Absyn_Exp*_tmp3A1[3U];_tmp3A1[0]=_tmp398,_tmp3A1[1]=_tmp39E,_tmp3A1[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A1,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp3A0=Cyc_Absyn_exp_stmt(_tmp39F,0U);
({struct Cyc_Absyn_Stmt*_tmp94E=({struct _tuple1*_tmp94D=_tmp397;void*_tmp94C=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp94B=_tmp4FD;Cyc_Absyn_declare_stmt(_tmp94D,_tmp94C,_tmp94B,_tmp3A0,0U);});_tmp3A0=_tmp94E;});
({void*_tmp94F=Cyc_Toc_stmt_exp_r(_tmp3A0);e->r=_tmp94F;});};}else{
# 2479
({void*_tmp952=({struct Cyc_Absyn_Exp*_tmp951=_tmp396;Cyc_Toc_fncall_exp_r(_tmp951,({struct Cyc_Absyn_Exp*_tmp3A2[3U];_tmp3A2[0]=_tmp4FD,({
# 2481
struct Cyc_Absyn_Exp*_tmp950=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B8.elt_typ),0U);_tmp3A2[1]=_tmp950;}),_tmp3A2[2]=_tmp3B4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A2,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2479
e->r=_tmp952;});}}
# 2485
goto _LLAD;};}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f2)->tag == 1U){_LLB2: _tmp3B5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f2)->f1;_LLB3:
# 2487
 if(!Cyc_Evexp_c_can_eval(_tmp3B5))
({void*_tmp3A5=0U;({unsigned int _tmp954=e->loc;struct _dyneither_ptr _tmp953=({const char*_tmp3A6="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp3A6,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp954,_tmp953,_tag_dyneither(_tmp3A5,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3A7=0U;({struct _dyneither_ptr _tmp955=({const char*_tmp3A8="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp3A8,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp955,_tag_dyneither(_tmp3A7,sizeof(void*),0U));});});{
# 2499 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3A9=_tmp3B5;
if(_tmp381  && !_tmp382)
({struct Cyc_Absyn_Exp*_tmp957=({struct Cyc_Absyn_Exp*_tmp956=_tmp3B5;Cyc_Absyn_add_exp(_tmp956,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3A9=_tmp957;});{
# 2506
struct Cyc_Absyn_Exp*_tmp3AA=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp3AB=({struct Cyc_Absyn_Exp*_tmp959=_tmp3AA;Cyc_Absyn_fncall_exp(_tmp959,({struct Cyc_Absyn_Exp*_tmp3AD[3U];_tmp3AD[0]=_tmp4FD,({
# 2509
struct Cyc_Absyn_Exp*_tmp958=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B9.elt_typ),0U);_tmp3AD[1]=_tmp958;}),_tmp3AD[2]=_tmp3A9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3AD,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2511
if(!_tmp37E)
({void*_tmp95C=({struct Cyc_Absyn_Exp*_tmp95B=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp95B,({struct Cyc_List_List*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));
({struct Cyc_Absyn_Exp*_tmp95A=Cyc_Absyn_copy_exp(_tmp3AB);_tmp3AC->hd=_tmp95A;}),_tmp3AC->tl=0;_tmp3AC;}));});
# 2512
_tmp3AB->r=_tmp95C;});
# 2514
({void*_tmp95D=Cyc_Toc_cast_it_r(*_tmp4FE,_tmp3AB);e->r=_tmp95D;});
goto _LLAD;};};}else{_LLB4: _LLB5:
# 2519
 DynCast:
 if((_tmp381  && !_tmp382) && !(_tmp3B8.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3AE=0U;({struct _dyneither_ptr _tmp95E=({const char*_tmp3AF="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp3AF,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp95E,_tag_dyneither(_tmp3AE,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp3B0=Cyc_Toc__dyneither_ptr_decrease_size_e;
({void*_tmp962=({struct Cyc_Absyn_Exp*_tmp961=_tmp3B0;Cyc_Toc_fncall_exp_r(_tmp961,({struct Cyc_Absyn_Exp*_tmp3B1[3U];_tmp3B1[0]=_tmp4FD,({
struct Cyc_Absyn_Exp*_tmp960=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B9.elt_typ),0U);_tmp3B1[1]=_tmp960;}),({
struct Cyc_Absyn_Exp*_tmp95F=Cyc_Absyn_uint_exp(1U,0U);_tmp3B1[2]=_tmp95F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B1,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2524
e->r=_tmp962;});};}
# 2528
goto _LLAD;}}_LLAD:;}
# 2530
goto _LLA6;}case 6U: _LLA9: _tmp3BA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C.f1)->f1;_LLAA:
# 2532
{void*_tmp3B6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3BA.ptr_atts).bounds);void*_tmp3B7=_tmp3B6;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3B7)->tag == 0U){_LLC5: _LLC6:
# 2534
({void*_tmp964=({struct Cyc_Absyn_Exp*_tmp963=Cyc_Absyn_new_exp(_tmp4FD->r,_tmp4FD->loc);Cyc_Toc_aggrmember_exp_r(_tmp963,Cyc_Toc_curr_sp);});_tmp4FD->r=_tmp964;});
_tmp4FD->topt=new_typ_c;
goto _LLC4;}else{_LLC7: _LLC8:
 goto _LLC4;}_LLC4:;}
# 2539
goto _LLA6;default: goto _LLAB;}else{_LLAB: _LLAC:
# 2541
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4FD->r;
goto _LLA6;}_LLA6:;}
# 2545
goto _LL0;}case 15U: _LL25: _tmp4FF=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL26:
# 2548
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4FF);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4FF)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4FF,0,Cyc_Toc_address_lvalue,1);
# 2554
({void*_tmp966=({void*_tmp965=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp965,_tmp4FF);});e->r=_tmp966;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp4FF->topt))))
# 2558
({void*_tmp968=({void*_tmp967=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp967,_tmp4FF);});e->r=_tmp968;});}
# 2560
goto _LL0;case 16U: _LL27: _tmp501=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp500=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL28:
# 2567
 Cyc_Toc_exp_to_c(nv,_tmp500);{
# 2569
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp500->topt));
struct _tuple1*_tmp3BB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3BC=Cyc_Absyn_var_exp(_tmp3BB,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3BD=Cyc_Tcutil_compress(elt_typ);void*_tmp3BE=_tmp3BD;void*_tmp3BF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE)->tag == 8U){_LLCA: _tmp3BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE)->f1).elt_type;_LLCB:
# 2575
({void*_tmp969=Cyc_Toc_typ_to_c(_tmp3BF);elt_typ=_tmp969;});
({struct Cyc_Absyn_Exp*_tmp96A=Cyc_Absyn_copy_exp(_tmp3BC);lhs=_tmp96A;});
goto _LLC9;}else{_LLCC: _LLCD:
# 2579
({struct Cyc_Absyn_Exp*_tmp96B=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp3BC),0U);lhs=_tmp96B;});
goto _LLC9;}_LLC9:;}{
# 2582
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp500);
struct _tuple1*_tmp3C0=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp3C1=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp3C0),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp96E=({struct Cyc_Absyn_Exp*_tmp96D=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp96D,({struct Cyc_Absyn_Exp*_tmp3C2[2U];_tmp3C2[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp3C1),({
struct Cyc_Absyn_Exp*_tmp96C=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3C2[1]=_tmp96C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3C2,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2587
mexp=_tmp96E;});else{
# 2590
({struct Cyc_Absyn_Exp*_tmp96F=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp3BC,0U),0U);mexp=_tmp96F;});}{
# 2593
struct Cyc_Absyn_Exp*_tmp3C3=Cyc_Toc_get_varsizeexp(_tmp500);
if(_tmp3C3 != 0)
({struct Cyc_Absyn_Exp*_tmp970=Cyc_Absyn_add_exp(mexp,_tmp3C3,0U);mexp=_tmp970;});
# 2597
if(_tmp501 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp971=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp971;});else{
# 2600
struct Cyc_Absyn_Exp*r=_tmp501;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp972=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp972;});}{
# 2604
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp977=({struct Cyc_Absyn_Exp*_tmp976=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp976,({struct Cyc_Absyn_Exp*_tmp3C4[3U];({
struct Cyc_Absyn_Exp*_tmp975=Cyc_Absyn_copy_exp(_tmp3BC);_tmp3C4[0]=_tmp975;}),({
struct Cyc_Absyn_Exp*_tmp974=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3C4[1]=_tmp974;}),
(unsigned int)_tmp3C1?_tmp3C4[2]=_tmp3C1:({struct Cyc_Absyn_Exp*_tmp973=Cyc_Absyn_uint_exp(1U,0U);_tmp3C4[2]=_tmp973;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3C4,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2606
answer=_tmp977;});else{
# 2611
({struct Cyc_Absyn_Exp*_tmp978=Cyc_Absyn_copy_exp(_tmp3BC);answer=_tmp978;});}
({void*_tmp979=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->tag=Cyc_Toc_Dest,_tmp3C5->f1=_tmp3BC;_tmp3C5;});e->annot=_tmp979;});{
struct Cyc_Absyn_Stmt*_tmp3C6=({struct _tuple1*_tmp97D=_tmp3BB;void*_tmp97C=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp97B=mexp;Cyc_Absyn_declare_stmt(_tmp97D,_tmp97C,_tmp97B,({
struct Cyc_Absyn_Stmt*_tmp97A=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp97A,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2617
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp97E=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp3C0),Cyc_Absyn_uint_typ,array_len,_tmp3C6,0U);_tmp3C6=_tmp97E;});
({void*_tmp97F=Cyc_Toc_stmt_exp_r(_tmp3C6);e->r=_tmp97F;});
if(_tmp3C3 != 0)
({void*_tmp981=({void*_tmp980=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp980,Cyc_Absyn_copy_exp(e));});e->r=_tmp981;});
goto _LL0;};};};};};case 18U: _LL29: _tmp502=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL2A: {
# 2626
int _tmp3C7=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp502);
Cyc_Toc_set_in_sizeof(nv,_tmp3C7);
goto _LL0;}case 17U: _LL2B: _tmp503=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL2C:
({void*_tmp983=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->tag=17U,({void*_tmp982=Cyc_Toc_typ_to_c(_tmp503);_tmp3C8->f1=_tmp982;});_tmp3C8;});e->r=_tmp983;});goto _LL0;case 19U: _LL2D: _tmp505=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp504=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL2E: {
# 2632
void*_tmp3C9=_tmp505;
struct Cyc_List_List*_tmp3CA=_tmp504;
for(0;_tmp3CA != 0;_tmp3CA=_tmp3CA->tl){
void*_tmp3CB=(void*)_tmp3CA->hd;void*_tmp3CC=_tmp3CB;unsigned int _tmp3DE;struct _dyneither_ptr*_tmp3DD;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3CC)->tag == 0U){_LLCF: _tmp3DD=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3CC)->f1;_LLD0:
 goto _LLCE;}else{_LLD1: _tmp3DE=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3CC)->f1;_LLD2:
# 2638
{void*_tmp3CD=Cyc_Tcutil_compress(_tmp3C9);void*_tmp3CE=_tmp3CD;struct Cyc_Absyn_Datatypefield*_tmp3DC;struct Cyc_List_List*_tmp3DB;struct Cyc_List_List*_tmp3DA;union Cyc_Absyn_AggrInfoU _tmp3D9;switch(*((int*)_tmp3CE)){case 11U: _LLD4: _tmp3D9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CE)->f1).aggr_info;_LLD5: {
# 2640
struct Cyc_Absyn_Aggrdecl*_tmp3CF=Cyc_Absyn_get_known_aggrdecl(_tmp3D9);
if(_tmp3CF->impl == 0)
({void*_tmp3D0=0U;({struct _dyneither_ptr _tmp984=({const char*_tmp3D1="struct fields must be known";_tag_dyneither(_tmp3D1,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp984,_tag_dyneither(_tmp3D0,sizeof(void*),0U));});});
_tmp3DA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3CF->impl))->fields;goto _LLD7;}case 12U: _LLD6: _tmp3DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3CE)->f2;_LLD7: {
# 2645
struct Cyc_Absyn_Aggrfield*_tmp3D2=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DA,(int)_tmp3DE);
({void*_tmp985=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->tag=0U,_tmp3D3->f1=_tmp3D2->name;_tmp3D3;}));_tmp3CA->hd=_tmp985;});
_tmp3C9=_tmp3D2->type;
goto _LLD3;}case 10U: _LLD8: _tmp3DB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3CE)->f1;_LLD9:
# 2650
({void*_tmp987=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->tag=0U,({struct _dyneither_ptr*_tmp986=Cyc_Absyn_fieldname((int)(_tmp3DE + 1));_tmp3D4->f1=_tmp986;});_tmp3D4;}));_tmp3CA->hd=_tmp987;});
({void*_tmp988=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DB,(int)_tmp3DE)).f2;_tmp3C9=_tmp988;});
goto _LLD3;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3CE)->f1).field_info).KnownDatatypefield).tag == 2){_LLDA: _tmp3DC=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3CE)->f1).field_info).KnownDatatypefield).val).f2;_LLDB:
# 2654
 if(_tmp3DE == 0)
({void*_tmp989=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->tag=0U,_tmp3D5->f1=Cyc_Toc_tag_sp;_tmp3D5;}));_tmp3CA->hd=_tmp989;});else{
# 2657
({void*_tmp98A=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DC->typs,(int)(_tmp3DE - 1))).f2;_tmp3C9=_tmp98A;});
({void*_tmp98C=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->tag=0U,({struct _dyneither_ptr*_tmp98B=Cyc_Absyn_fieldname((int)_tmp3DE);_tmp3D6->f1=_tmp98B;});_tmp3D6;}));_tmp3CA->hd=_tmp98C;});}
# 2660
goto _LLD3;}else{goto _LLDC;}default: _LLDC: _LLDD:
({void*_tmp3D7=0U;({struct _dyneither_ptr _tmp98D=({const char*_tmp3D8="impossible type for offsetof tuple index";_tag_dyneither(_tmp3D8,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98D,_tag_dyneither(_tmp3D7,sizeof(void*),0U));});});
goto _LLD3;}_LLD3:;}
# 2664
goto _LLCE;}_LLCE:;}
# 2667
({void*_tmp98F=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->tag=19U,({void*_tmp98E=Cyc_Toc_typ_to_c(_tmp505);_tmp3DF->f1=_tmp98E;}),_tmp3DF->f2=_tmp504;_tmp3DF;});e->r=_tmp98F;});
goto _LL0;}case 20U: _LL2F: _tmp506=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL30: {
# 2670
int _tmp3E0=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp3E1=Cyc_Tcutil_compress((void*)_check_null(_tmp506->topt));
{void*_tmp3E2=_tmp3E1;void*_tmp3F7;struct Cyc_Absyn_Tqual _tmp3F6;void*_tmp3F5;union Cyc_Absyn_Constraint*_tmp3F4;union Cyc_Absyn_Constraint*_tmp3F3;union Cyc_Absyn_Constraint*_tmp3F2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->tag == 5U){_LLDF: _tmp3F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).elt_typ;_tmp3F6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).elt_tq;_tmp3F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).ptr_atts).rgn;_tmp3F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).ptr_atts).nullable;_tmp3F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).ptr_atts).bounds;_tmp3F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).ptr_atts).zero_term;_LLE0:
# 2675
{void*_tmp3E3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3F3);void*_tmp3E4=_tmp3E3;struct Cyc_Absyn_Exp*_tmp3EF;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3E4)->tag == 1U){_LLE4: _tmp3EF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3E4)->f1;_LLE5: {
# 2677
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp506);
Cyc_Toc_exp_to_c(nv,_tmp506);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3E5=0U;({unsigned int _tmp991=e->loc;struct _dyneither_ptr _tmp990=({const char*_tmp3E6="inserted null check due to dereference";_tag_dyneither(_tmp3E6,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp991,_tmp990,_tag_dyneither(_tmp3E5,sizeof(void*),0U));});});
# 2683
({void*_tmp995=({void*_tmp994=Cyc_Toc_typ_to_c(_tmp3E1);Cyc_Toc_cast_it_r(_tmp994,({
struct Cyc_Absyn_Exp*_tmp993=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp993,({struct Cyc_List_List*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));
({struct Cyc_Absyn_Exp*_tmp992=Cyc_Absyn_copy_exp(_tmp506);_tmp3E7->hd=_tmp992;}),_tmp3E7->tl=0;_tmp3E7;}),0U);}));});
# 2683
_tmp506->r=_tmp995;});}
# 2691
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F2)){
struct _tuple13 _tmp3E8=Cyc_Evexp_eval_const_uint_exp(_tmp3EF);struct _tuple13 _tmp3E9=_tmp3E8;unsigned int _tmp3ED;int _tmp3EC;_LLE9: _tmp3ED=_tmp3E9.f1;_tmp3EC=_tmp3E9.f2;_LLEA:;
# 2697
if(!_tmp3EC  || _tmp3ED <= 0)
({void*_tmp3EA=0U;({unsigned int _tmp997=e->loc;struct _dyneither_ptr _tmp996=({const char*_tmp3EB="cannot determine dereference is in bounds";_tag_dyneither(_tmp3EB,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp997,_tmp996,_tag_dyneither(_tmp3EA,sizeof(void*),0U));});});}
# 2700
goto _LLE3;}}else{_LLE6: _LLE7: {
# 2703
struct Cyc_Absyn_Exp*_tmp3EE=Cyc_Absyn_uint_exp(0U,0U);
_tmp3EE->topt=Cyc_Absyn_uint_typ;
({void*_tmp998=Cyc_Toc_subscript_exp_r(_tmp506,_tmp3EE);e->r=_tmp998;});
Cyc_Toc_exp_to_c(nv,e);
goto _LLE3;}}_LLE3:;}
# 2709
goto _LLDE;}else{_LLE1: _LLE2:
({void*_tmp3F0=0U;({struct _dyneither_ptr _tmp999=({const char*_tmp3F1="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp3F1,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp999,_tag_dyneither(_tmp3F0,sizeof(void*),0U));});});}_LLDE:;}
# 2712
Cyc_Toc_set_lhs(nv,_tmp3E0);
goto _LL0;};}case 21U: _LL31: _tmp50A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp509=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp508=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp507=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL32: {
# 2715
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp50A->topt);
Cyc_Toc_exp_to_c(nv,_tmp50A);
if(_tmp508  && _tmp507)
({void*_tmp99E=({struct Cyc_Absyn_Exp*_tmp99D=_tmp50A;void*_tmp99C=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp99B=e1_cyc_type;struct _dyneither_ptr*_tmp99A=_tmp509;Cyc_Toc_check_tagged_union(_tmp99D,_tmp99C,_tmp99B,_tmp99A,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2719
e->r=_tmp99E;});
# 2723
if(is_poly)
({void*_tmp9A0=({void*_tmp99F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp99F,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9A0;});
goto _LL0;}case 22U: _LL33: _tmp50E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp50D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp50C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp50B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL34: {
# 2727
int _tmp3F8=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp50E->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp50E);
Cyc_Toc_exp_to_c(nv,_tmp50E);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp3F9=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp3FA=_tmp3F9;void*_tmp410;struct Cyc_Absyn_Tqual _tmp40F;void*_tmp40E;union Cyc_Absyn_Constraint*_tmp40D;union Cyc_Absyn_Constraint*_tmp40C;union Cyc_Absyn_Constraint*_tmp40B;_LLEC: _tmp410=_tmp3FA.elt_typ;_tmp40F=_tmp3FA.elt_tq;_tmp40E=(_tmp3FA.ptr_atts).rgn;_tmp40D=(_tmp3FA.ptr_atts).nullable;_tmp40C=(_tmp3FA.ptr_atts).bounds;_tmp40B=(_tmp3FA.ptr_atts).zero_term;_LLED:;
{void*_tmp3FB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp40C);void*_tmp3FC=_tmp3FB;struct Cyc_Absyn_Exp*_tmp40A;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3FC)->tag == 1U){_LLEF: _tmp40A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3FC)->f1;_LLF0: {
# 2738
struct _tuple13 _tmp3FD=Cyc_Evexp_eval_const_uint_exp(_tmp40A);struct _tuple13 _tmp3FE=_tmp3FD;unsigned int _tmp408;int _tmp407;_LLF4: _tmp408=_tmp3FE.f1;_tmp407=_tmp3FE.f2;_LLF5:;
if(_tmp407){
if(_tmp408 < 1)
({void*_tmp3FF=0U;({struct _dyneither_ptr _tmp9A1=({const char*_tmp400="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp400,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp9A1,_tag_dyneither(_tmp3FF,sizeof(void*),0U));});});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp401=0U;({unsigned int _tmp9A3=e->loc;struct _dyneither_ptr _tmp9A2=({const char*_tmp402="inserted null check due to dereference";_tag_dyneither(_tmp402,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9A3,_tmp9A2,_tag_dyneither(_tmp401,sizeof(void*),0U));});});
# 2746
({void*_tmp9A7=({void*_tmp9A6=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9A6,({
struct Cyc_Absyn_Exp*_tmp9A5=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9A5,({struct Cyc_Absyn_Exp*_tmp403[1U];({struct Cyc_Absyn_Exp*_tmp9A4=
Cyc_Absyn_new_exp(_tmp50E->r,0U);_tmp403[0]=_tmp9A4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp403,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2746
_tmp50E->r=_tmp9A7;});}}else{
# 2751
if(!Cyc_Evexp_c_can_eval(_tmp40A))
({void*_tmp404=0U;({unsigned int _tmp9A9=e->loc;struct _dyneither_ptr _tmp9A8=({const char*_tmp405="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp405,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp9A9,_tmp9A8,_tag_dyneither(_tmp404,sizeof(void*),0U));});});
# 2755
({void*_tmp9AF=({void*_tmp9AE=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9AE,({
struct Cyc_Absyn_Exp*_tmp9AD=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9AD,({struct Cyc_Absyn_Exp*_tmp406[4U];({
struct Cyc_Absyn_Exp*_tmp9AC=Cyc_Absyn_new_exp(_tmp50E->r,0U);_tmp406[0]=_tmp9AC;}),_tmp406[1]=_tmp40A,({
struct Cyc_Absyn_Exp*_tmp9AB=Cyc_Absyn_sizeoftyp_exp(_tmp410,0U);_tmp406[2]=_tmp9AB;}),({
struct Cyc_Absyn_Exp*_tmp9AA=Cyc_Absyn_uint_exp(0U,0U);_tmp406[3]=_tmp9AA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp406,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 2755
_tmp50E->r=_tmp9AF;});}
# 2761
goto _LLEE;}}else{_LLF1: _LLF2: {
# 2764
void*ta1=Cyc_Toc_typ_to_c(_tmp410);
({void*_tmp9B5=({void*_tmp9B4=Cyc_Absyn_cstar_typ(ta1,_tmp40F);Cyc_Toc_cast_it_r(_tmp9B4,({
struct Cyc_Absyn_Exp*_tmp9B3=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp9B3,({struct Cyc_Absyn_Exp*_tmp409[3U];({
struct Cyc_Absyn_Exp*_tmp9B2=Cyc_Absyn_new_exp(_tmp50E->r,0U);_tmp409[0]=_tmp9B2;}),({
struct Cyc_Absyn_Exp*_tmp9B1=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp409[1]=_tmp9B1;}),({
struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Absyn_uint_exp(0U,0U);_tmp409[2]=_tmp9B0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp409,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 2765
_tmp50E->r=_tmp9B5;});
# 2772
goto _LLEE;}}_LLEE:;}
# 2774
if(_tmp50C  && _tmp50B)
({void*_tmp9BA=({struct Cyc_Absyn_Exp*_tmp9B9=_tmp50E;void*_tmp9B8=Cyc_Toc_typ_to_c(e1typ);void*_tmp9B7=_tmp410;struct _dyneither_ptr*_tmp9B6=_tmp50D;Cyc_Toc_check_tagged_union(_tmp9B9,_tmp9B8,_tmp9B7,_tmp9B6,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9BA;});
# 2777
if(is_poly  && _tmp50B)
({void*_tmp9BC=({void*_tmp9BB=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9BB,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9BC;});
Cyc_Toc_set_lhs(nv,_tmp3F8);
goto _LL0;};};}case 23U: _LL35: _tmp510=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp50F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL36: {
# 2782
int _tmp411=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp412=Cyc_Tcutil_compress((void*)_check_null(_tmp510->topt));
# 2786
{void*_tmp413=_tmp412;void*_tmp441;struct Cyc_Absyn_Tqual _tmp440;void*_tmp43F;union Cyc_Absyn_Constraint*_tmp43E;union Cyc_Absyn_Constraint*_tmp43D;union Cyc_Absyn_Constraint*_tmp43C;struct Cyc_List_List*_tmp43B;switch(*((int*)_tmp413)){case 10U: _LLF7: _tmp43B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp413)->f1;_LLF8:
# 2789
 Cyc_Toc_exp_to_c(nv,_tmp510);
Cyc_Toc_exp_to_c(nv,_tmp50F);{
struct _tuple13 _tmp414=Cyc_Evexp_eval_const_uint_exp(_tmp50F);struct _tuple13 _tmp415=_tmp414;unsigned int _tmp419;int _tmp418;_LLFE: _tmp419=_tmp415.f1;_tmp418=_tmp415.f2;_LLFF:;
if(!_tmp418)
({void*_tmp416=0U;({struct _dyneither_ptr _tmp9BD=({const char*_tmp417="unknown tuple subscript in translation to C";_tag_dyneither(_tmp417,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp9BD,_tag_dyneither(_tmp416,sizeof(void*),0U));});});
({void*_tmp9BF=({struct Cyc_Absyn_Exp*_tmp9BE=_tmp510;Cyc_Toc_aggrmember_exp_r(_tmp9BE,Cyc_Absyn_fieldname((int)(_tmp419 + 1)));});e->r=_tmp9BF;});
goto _LLF6;};case 5U: _LLF9: _tmp441=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).elt_typ;_tmp440=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).elt_tq;_tmp43F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).ptr_atts).rgn;_tmp43E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).ptr_atts).nullable;_tmp43D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).ptr_atts).bounds;_tmp43C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).ptr_atts).zero_term;_LLFA: {
# 2797
struct Cyc_List_List*_tmp41A=Cyc_Toc_get_relns(_tmp510);
int _tmp41B=Cyc_Toc_need_null_check(_tmp510);
int _tmp41C=Cyc_Toc_in_sizeof(nv);
# 2805
int in_bnds=_tmp41C;
{void*_tmp41D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp43D);void*_tmp41E=_tmp41D;_LL101: _LL102:
# 2808
({int _tmp9C0=in_bnds  || Cyc_Toc_check_bounds(_tmp412,_tmp41A,_tmp510,_tmp50F);in_bnds=_tmp9C0;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp421=({struct Cyc_String_pa_PrintArg_struct _tmp7A1;_tmp7A1.tag=0U,({struct _dyneither_ptr _tmp9C1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp7A1.f1=_tmp9C1;});_tmp7A1;});void*_tmp41F[1U];_tmp41F[0]=& _tmp421;({unsigned int _tmp9C3=e->loc;struct _dyneither_ptr _tmp9C2=({const char*_tmp420="bounds check necessary for %s";_tag_dyneither(_tmp420,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp9C3,_tmp9C2,_tag_dyneither(_tmp41F,sizeof(void*),1U));});});_LL100:;}
# 2815
Cyc_Toc_exp_to_c(nv,_tmp510);
Cyc_Toc_exp_to_c(nv,_tmp50F);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp422=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp43D);void*_tmp423=_tmp422;struct Cyc_Absyn_Exp*_tmp438;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp423)->tag == 1U){_LL104: _tmp438=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp423)->f1;_LL105: {
# 2820
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp43E) && _tmp41B;
void*ta1=Cyc_Toc_typ_to_c(_tmp441);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp440);
struct _tuple13 _tmp424=Cyc_Evexp_eval_const_uint_exp(_tmp438);struct _tuple13 _tmp425=_tmp424;unsigned int _tmp435;int _tmp434;_LL109: _tmp435=_tmp425.f1;_tmp434=_tmp425.f2;_LL10A:;
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp426=0U;({unsigned int _tmp9C5=e->loc;struct _dyneither_ptr _tmp9C4=({const char*_tmp427="inserted null check due to dereference";_tag_dyneither(_tmp427,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9C5,_tmp9C4,_tag_dyneither(_tmp426,sizeof(void*),0U));});});
({void*_tmp9C9=({void*_tmp9C8=ta2;Cyc_Toc_cast_it_r(_tmp9C8,({struct Cyc_Absyn_Exp*_tmp9C7=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9C7,({struct Cyc_Absyn_Exp*_tmp428[1U];({struct Cyc_Absyn_Exp*_tmp9C6=
Cyc_Absyn_copy_exp(_tmp510);_tmp428[0]=_tmp9C6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp428,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2830
_tmp510->r=_tmp9C9;});}else{
# 2834
if((_tmp434  && _tmp435 == 1) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp43C)){
# 2836
if(!Cyc_Evexp_c_can_eval(_tmp438))
({void*_tmp429=0U;({unsigned int _tmp9CB=e->loc;struct _dyneither_ptr _tmp9CA=({const char*_tmp42A="cannot determine subscript is in bounds";_tag_dyneither(_tmp42A,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9CB,_tmp9CA,_tag_dyneither(_tmp429,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp510);
({void*_tmp9CE=Cyc_Toc_deref_exp_r(({void*_tmp9CD=ta2;Cyc_Toc_cast_it(_tmp9CD,({
struct Cyc_Absyn_Exp*_tmp9CC=function_e;Cyc_Absyn_fncall_exp(_tmp9CC,({struct Cyc_Absyn_Exp*_tmp42B[3U];_tmp42B[0]=_tmp510,_tmp42B[1]=_tmp438,_tmp42B[2]=_tmp50F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42B,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2839
e->r=_tmp9CE;});};}else{
# 2842
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp438))
({void*_tmp42C=0U;({unsigned int _tmp9D0=e->loc;struct _dyneither_ptr _tmp9CF=({const char*_tmp42D="cannot determine subscript is in bounds";_tag_dyneither(_tmp42D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9D0,_tmp9CF,_tag_dyneither(_tmp42C,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp42E=0U;({unsigned int _tmp9D2=e->loc;struct _dyneither_ptr _tmp9D1=({const char*_tmp42F="inserted null check due to dereference";_tag_dyneither(_tmp42F,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9D2,_tmp9D1,_tag_dyneither(_tmp42E,sizeof(void*),0U));});});
# 2848
({void*_tmp9D6=Cyc_Toc_deref_exp_r(({void*_tmp9D5=ta2;Cyc_Toc_cast_it(_tmp9D5,({
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9D4,({struct Cyc_Absyn_Exp*_tmp430[4U];_tmp430[0]=_tmp510,_tmp430[1]=_tmp438,({
# 2851
struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp430[2]=_tmp9D3;}),_tmp430[3]=_tmp50F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp430,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 2848
e->r=_tmp9D6;});}else{
# 2854
if(!Cyc_Evexp_c_can_eval(_tmp438))
({void*_tmp431=0U;({unsigned int _tmp9D8=e->loc;struct _dyneither_ptr _tmp9D7=({const char*_tmp432="cannot determine subscript is in bounds";_tag_dyneither(_tmp432,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9D8,_tmp9D7,_tag_dyneither(_tmp431,sizeof(void*),0U));});});
# 2857
({void*_tmp9DB=({struct Cyc_Absyn_Exp*_tmp9DA=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmp9DA,({struct Cyc_Absyn_Exp*_tmp433[2U];_tmp433[0]=_tmp438,({
struct Cyc_Absyn_Exp*_tmp9D9=Cyc_Absyn_copy_exp(_tmp50F);_tmp433[1]=_tmp9D9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp433,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2857
_tmp50F->r=_tmp9DB;});}}}}
# 2860
goto _LL103;}}else{_LL106: _LL107: {
# 2862
void*ta1=Cyc_Toc_typ_to_c(_tmp441);
if(in_bnds){
# 2866
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmp9DE=({struct Cyc_Absyn_Exp*_tmp9DD=({void*_tmp9DC=Cyc_Absyn_cstar_typ(ta1,_tmp440);Cyc_Toc_cast_it(_tmp9DC,
Cyc_Toc_member_exp(_tmp510,Cyc_Toc_curr_sp,0U));});
# 2867
Cyc_Toc_subscript_exp_r(_tmp9DD,_tmp50F);});e->r=_tmp9DE;});}else{
# 2871
struct Cyc_Absyn_Exp*_tmp436=Cyc_Toc__check_dyneither_subscript_e;
({void*_tmp9E2=Cyc_Toc_deref_exp_r(({void*_tmp9E1=Cyc_Absyn_cstar_typ(ta1,_tmp440);Cyc_Toc_cast_it(_tmp9E1,({
struct Cyc_Absyn_Exp*_tmp9E0=_tmp436;Cyc_Absyn_fncall_exp(_tmp9E0,({struct Cyc_Absyn_Exp*_tmp437[3U];_tmp437[0]=_tmp510,({
struct Cyc_Absyn_Exp*_tmp9DF=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp437[1]=_tmp9DF;}),_tmp437[2]=_tmp50F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp437,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2872
e->r=_tmp9E2;});}
# 2877
goto _LL103;}}_LL103:;}
# 2879
goto _LLF6;}default: _LLFB: _LLFC:
({void*_tmp439=0U;({struct _dyneither_ptr _tmp9E3=({const char*_tmp43A="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp43A,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E3,_tag_dyneither(_tmp439,sizeof(void*),0U));});});}_LLF6:;}
# 2882
Cyc_Toc_set_lhs(nv,_tmp411);
goto _LL0;};}case 24U: _LL37: _tmp511=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL38:
# 2885
 if(!Cyc_Toc_is_toplevel(nv)){
# 2887
void*_tmp442=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp511));
{void*_tmp443=_tmp442;union Cyc_Absyn_AggrInfoU _tmp448;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp443)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp443)->f1).targs == 0){_LL10C: _tmp448=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp443)->f1).aggr_info;_LL10D: {
# 2890
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp448);
{int i=1;for(0;_tmp511 != 0;(_tmp511=_tmp511->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp511->hd);
({struct Cyc_List_List*_tmp9E6=({struct Cyc_List_List*_tmp444=_cycalloc(sizeof(*_tmp444));({struct _tuple20*_tmp9E5=({struct _dyneither_ptr*_tmp9E4=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9E4,(struct Cyc_Absyn_Exp*)_tmp511->hd);});_tmp444->hd=_tmp9E5;}),_tmp444->tl=dles;_tmp444;});dles=_tmp9E6;});}}
# 2896
({void*_tmp9E8=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->tag=29U,_tmp445->f1=sd->name,_tmp445->f2=0,({struct Cyc_List_List*_tmp9E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp445->f3=_tmp9E7;}),_tmp445->f4=sd;_tmp445;});e->r=_tmp9E8;});
e->topt=_tmp442;
goto _LL10B;}}else{goto _LL10E;}}else{_LL10E: _LL10F:
({void*_tmp446=0U;({struct _dyneither_ptr _tmp9E9=({const char*_tmp447="tuple type not an aggregate";_tag_dyneither(_tmp447,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E9,_tag_dyneither(_tmp446,sizeof(void*),0U));});});}_LL10B:;}
# 2901
goto _LL0;}else{
# 2905
struct Cyc_List_List*dles=0;
for(0;_tmp511 != 0;_tmp511=_tmp511->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp511->hd);
({struct Cyc_List_List*_tmp9EB=({struct Cyc_List_List*_tmp44A=_cycalloc(sizeof(*_tmp44A));({struct _tuple20*_tmp9EA=({struct _tuple20*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->f1=0,_tmp449->f2=(struct Cyc_Absyn_Exp*)_tmp511->hd;_tmp449;});_tmp44A->hd=_tmp9EA;}),_tmp44A->tl=dles;_tmp44A;});dles=_tmp9EB;});}
# 2910
({void*_tmp9EC=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9EC;});}
# 2912
goto _LL0;case 26U: _LL39: _tmp512=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL3A:
# 2918
{struct Cyc_List_List*_tmp44B=_tmp512;for(0;_tmp44B != 0;_tmp44B=_tmp44B->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp44B->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9ED=Cyc_Toc_unresolvedmem_exp_r(0,_tmp512);e->r=_tmp9ED;});
goto _LL0;case 27U: _LL3B: _tmp516=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp515=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp514=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp513=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL3C:
# 2926
 Cyc_Toc_exp_to_c(nv,_tmp515);
Cyc_Toc_exp_to_c(nv,_tmp514);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp44C=Cyc_Evexp_eval_const_uint_exp(_tmp515);struct _tuple13 _tmp44D=_tmp44C;unsigned int _tmp457;int _tmp456;_LL111: _tmp457=_tmp44D.f1;_tmp456=_tmp44D.f2;_LL112:;{
void*_tmp44E=Cyc_Toc_typ_to_c((void*)_check_null(_tmp514->topt));
struct Cyc_List_List*es=0;
# 2933
if(!Cyc_Toc_is_zero(_tmp514)){
if(!_tmp456)
({void*_tmp44F=0U;({unsigned int _tmp9EF=_tmp515->loc;struct _dyneither_ptr _tmp9EE=({const char*_tmp450="cannot determine value of constant";_tag_dyneither(_tmp450,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9EF,_tmp9EE,_tag_dyneither(_tmp44F,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp457;++ i){
({struct Cyc_List_List*_tmp9F1=({struct Cyc_List_List*_tmp452=_cycalloc(sizeof(*_tmp452));({struct _tuple20*_tmp9F0=({struct _tuple20*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->f1=0,_tmp451->f2=_tmp514;_tmp451;});_tmp452->hd=_tmp9F0;}),_tmp452->tl=es;_tmp452;});es=_tmp9F1;});}}
# 2939
if(_tmp513){
struct Cyc_Absyn_Exp*_tmp453=({void*_tmp9F2=_tmp44E;Cyc_Toc_cast_it(_tmp9F2,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp9F5=({struct Cyc_List_List*_tmp9F4=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F4,({struct Cyc_List_List*_tmp455=_cycalloc(sizeof(*_tmp455));({struct _tuple20*_tmp9F3=({struct _tuple20*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->f1=0,_tmp454->f2=_tmp453;_tmp454;});_tmp455->hd=_tmp9F3;}),_tmp455->tl=0;_tmp455;}));});es=_tmp9F5;});}}
# 2944
({void*_tmp9F6=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9F6;});};}
# 2946
goto _LL0;case 28U: _LL3D: _tmp519=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp518=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp517=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL3E:
# 2953
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9F7=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9F7;});else{
# 2956
Cyc_Toc_exp_to_c(nv,_tmp519);}
goto _LL0;case 30U: _LL3F: _tmp51B=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp51A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL40:
# 2960
{struct Cyc_List_List*_tmp458=_tmp51A;for(0;_tmp458 != 0;_tmp458=_tmp458->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp458->hd)).f2);}}{
void*_tmp459=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp459;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp45A=Cyc_Tcutil_compress(_tmp459);void*_tmp45B=_tmp45A;union Cyc_Absyn_AggrInfoU _tmp45F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45B)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45B)->f1).targs == 0){_LL114: _tmp45F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45B)->f1).aggr_info;_LL115: {
# 2967
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp45F);
({void*_tmp9F8=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=29U,_tmp45C->f1=sd->name,_tmp45C->f2=0,_tmp45C->f3=_tmp51A,_tmp45C->f4=sd;_tmp45C;});e->r=_tmp9F8;});
e->topt=_tmp459;
goto _LL113;}}else{goto _LL116;}}else{_LL116: _LL117:
({void*_tmp45D=0U;({struct _dyneither_ptr _tmp9F9=({const char*_tmp45E="anonStruct type not an aggregate";_tag_dyneither(_tmp45E,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F9,_tag_dyneither(_tmp45D,sizeof(void*),0U));});});}_LL113:;}else{
# 2974
({void*_tmp9FA=Cyc_Toc_unresolvedmem_exp_r(0,_tmp51A);e->r=_tmp9FA;});}
goto _LL0;};case 29U: _LL41: _tmp51F=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp51E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp51D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp51C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL42:
# 2980
 if(_tmp51C == 0  || _tmp51C->impl == 0)
({void*_tmp460=0U;({struct _dyneither_ptr _tmp9FB=({const char*_tmp461="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp461,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FB,_tag_dyneither(_tmp460,sizeof(void*),0U));});});{
void*_tmp462=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp463=Cyc_Tcutil_compress(_tmp462);void*_tmp464=_tmp463;union Cyc_Absyn_AggrInfoU _tmp467;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp464)->tag == 11U){_LL119: _tmp467=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp464)->f1).aggr_info;_LL11A:
({struct _tuple1*_tmp9FC=(Cyc_Absyn_aggr_kinded_name(_tmp467)).f2;*_tmp51F=_tmp9FC;});goto _LL118;}else{_LL11B: _LL11C:
({void*_tmp465=0U;({struct _dyneither_ptr _tmp9FD=({const char*_tmp466="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp466,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FD,_tag_dyneither(_tmp465,sizeof(void*),0U));});});}_LL118:;}{
# 2991
struct Cyc_List_List*_tmp468=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51C->impl))->fields;
if(_tmp468 == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp468))->tl != 0;_tmp468=_tmp468->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp469=(struct Cyc_Absyn_Aggrfield*)_tmp468->hd;
struct Cyc_List_List*_tmp46A=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp51D,_tmp51C->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51C->impl))->fields);
# 2998
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp46B=Cyc_Tcutil_compress(old_typ);void*_tmp46C=_tmp46B;struct Cyc_List_List*_tmp499;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46C)->tag == 11U){_LL11E: _tmp499=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46C)->f1).targs;_LL11F:
# 3002
{struct Cyc_List_List*_tmp46D=_tmp46A;for(0;_tmp46D != 0;_tmp46D=_tmp46D->tl){
struct _tuple34*_tmp46E=(struct _tuple34*)_tmp46D->hd;struct _tuple34*_tmp46F=_tmp46E;struct Cyc_Absyn_Aggrfield*_tmp496;struct Cyc_Absyn_Exp*_tmp495;_LL123: _tmp496=_tmp46F->f1;_tmp495=_tmp46F->f2;_LL124:;{
void*_tmp470=_tmp495->topt;
Cyc_Toc_exp_to_c(nv,_tmp495);
# 3007
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp496->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp495->topt)))
({void*_tmp9FF=({void*_tmp9FE=Cyc_Toc_typ_to_c(_tmp496->type);Cyc_Toc_cast_it(_tmp9FE,
Cyc_Absyn_copy_exp(_tmp495));})->r;
# 3009
_tmp495->r=_tmp9FF;});
# 3012
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51C->impl))->tagged){
struct _dyneither_ptr*_tmp471=_tmp496->name;
struct Cyc_Absyn_Exp*_tmp472=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp51C,_tmp471),0U);
struct _tuple20*_tmp473=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp472);
struct _tuple20*_tmp474=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp495);
struct _tuple1*s=({struct _tuple1*_tmp47F=_cycalloc(sizeof(*_tmp47F));({union Cyc_Absyn_Nmspace _tmpA03=Cyc_Absyn_Abs_n(0,1);_tmp47F->f1=_tmpA03;}),({
struct _dyneither_ptr*_tmpA02=({struct _dyneither_ptr*_tmp47E=_cycalloc(sizeof(*_tmp47E));({struct _dyneither_ptr _tmpA01=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp47C=({struct Cyc_String_pa_PrintArg_struct _tmp7A3;_tmp7A3.tag=0U,_tmp7A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp51C->name).f2);_tmp7A3;});struct Cyc_String_pa_PrintArg_struct _tmp47D=({struct Cyc_String_pa_PrintArg_struct _tmp7A2;_tmp7A2.tag=0U,_tmp7A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp471);_tmp7A2;});void*_tmp47A[2U];_tmp47A[0]=& _tmp47C,_tmp47A[1]=& _tmp47D;({struct _dyneither_ptr _tmpA00=({const char*_tmp47B="_union_%s_%s";_tag_dyneither(_tmp47B,sizeof(char),13U);});Cyc_aprintf(_tmpA00,_tag_dyneither(_tmp47A,sizeof(void*),2U));});});*_tmp47E=_tmpA01;});_tmp47E;});_tmp47F->f2=_tmpA02;});_tmp47F;});
# 3020
struct _tuple20*_tmp475=({
struct _dyneither_ptr*_tmpA05=_tmp471;Cyc_Toc_make_field(_tmpA05,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->tag=29U,_tmp479->f1=s,_tmp479->f2=0,({struct Cyc_List_List*_tmpA04=({struct _tuple20*_tmp478[2U];_tmp478[0]=_tmp473,_tmp478[1]=_tmp474;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp478,sizeof(struct _tuple20*),2U));});_tmp479->f3=_tmpA04;}),_tmp479->f4=0;_tmp479;}),0U));});
# 3023
({void*_tmpA07=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->tag=29U,_tmp477->f1=*_tmp51F,_tmp477->f2=0,({struct Cyc_List_List*_tmpA06=({struct _tuple20*_tmp476[1U];_tmp476[0]=_tmp475;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp476,sizeof(struct _tuple20*),1U));});_tmp477->f3=_tmpA06;}),_tmp477->f4=_tmp51C;_tmp477;});e->r=_tmpA07;});}
# 3029
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp469 == _tmp496){
struct Cyc_List_List*_tmp480=({struct Cyc_List_List*_tmpA08=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51C->impl))->exist_vars,_tmp51E);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA08,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp51C->tvs,_tmp499));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp481=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51C->impl))->fields;for(0;_tmp481 != 0;_tmp481=_tmp481->tl){
struct Cyc_Absyn_Aggrfield*_tmp482=(struct Cyc_Absyn_Aggrfield*)_tmp481->hd;
void*_tmp483=Cyc_Tcutil_substitute(_tmp480,_tmp482->type);
struct Cyc_Absyn_Aggrfield*_tmp484=({struct Cyc_Absyn_Aggrfield*_tmpA09=_tmp482;Cyc_Toc_aggrfield_to_c(_tmpA09,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp480,_tmp483)));});
({struct Cyc_List_List*_tmpA0A=({struct Cyc_List_List*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->hd=_tmp484,_tmp485->tl=new_fields;_tmp485;});new_fields=_tmpA0A;});
# 3044
if(_tmp481->tl == 0){
{void*_tmp486=Cyc_Tcutil_compress(_tmp484->type);void*_tmp487=_tmp486;struct Cyc_Absyn_ArrayInfo _tmp48C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp487)->tag == 8U){_LL126: _tmp48C=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp487)->f1;_LL127:
# 3047
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp48C.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp488=_tmp48C;
({struct Cyc_Absyn_Exp*_tmpA0B=Cyc_Absyn_uint_exp(0U,0U);_tmp488.num_elts=_tmpA0B;});
({void*_tmpA0C=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->tag=8U,_tmp489->f1=_tmp488;_tmp489;});_tmp484->type=_tmpA0C;});}
# 3052
goto _LL125;}else{_LL128: _LL129:
# 3056
 if(_tmp495->topt == 0)
goto _LL125;
{void*_tmp48A=Cyc_Tcutil_compress((void*)_check_null(_tmp495->topt));void*_tmp48B=_tmp48A;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp48B)->tag == 11U){_LL12B: _LL12C:
# 3060
 _tmp484->type=(void*)_check_null(_tmp495->topt);goto _LL12A;}else{_LL12D: _LL12E:
 goto _LL12A;}_LL12A:;}
# 3063
goto _LL125;}_LL125:;}
# 3067
if(!Cyc_Toc_is_array_type(_tmp482->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp482->type)))
({struct Cyc_List_List*_tmpA0E=({struct Cyc_List_List*_tmp48E=_cycalloc(sizeof(*_tmp48E));({void*_tmpA0D=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->tag=6U,_tmp48D->f1=0;_tmp48D;});_tmp48E->hd=_tmpA0D;}),_tmp48E->tl=_tmp484->attributes;_tmp48E;});_tmp484->attributes=_tmpA0E;});}}}
# 3073
({struct Cyc_Absyn_Aggrdecl*_tmpA12=({struct _dyneither_ptr*_tmpA11=({struct _dyneither_ptr*_tmp492=_cycalloc(sizeof(*_tmp492));({struct _dyneither_ptr _tmpA10=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp491=({struct Cyc_Int_pa_PrintArg_struct _tmp7A4;_tmp7A4.tag=1U,_tmp7A4.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7A4;});void*_tmp48F[1U];_tmp48F[0]=& _tmp491;({struct _dyneither_ptr _tmpA0F=({const char*_tmp490="_genStruct%d";_tag_dyneither(_tmp490,sizeof(char),13U);});Cyc_aprintf(_tmpA0F,_tag_dyneither(_tmp48F,sizeof(void*),1U));});});*_tmp492=_tmpA10;});_tmp492;});Cyc_Toc_make_c_struct_defn(_tmpA11,
# 3075
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 3073
_tmp51C=_tmpA12;});
# 3076
*_tmp51F=_tmp51C->name;
Cyc_Toc_aggrdecl_to_c(_tmp51C,1);
# 3079
({void*_tmpA14=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpA13=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp493=_cycalloc(sizeof(*_tmp493));*_tmp493=_tmp51C;_tmp493;}));(_tmp494->f1).aggr_info=_tmpA13;}),(_tmp494->f1).targs=0;_tmp494;});e->topt=_tmpA14;});}};}}
# 3082
goto _LL11D;}else{_LL120: _LL121:
({void*_tmp497=0U;({struct _dyneither_ptr _tmpA15=({const char*_tmp498="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp498,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA15,_tag_dyneither(_tmp497,sizeof(void*),0U));});});}_LL11D:;}else{
# 3091
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51C->impl))->tagged){
# 3093
struct _tuple34*_tmp49A=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp46A))->hd;struct _tuple34*_tmp49B=_tmp49A;struct Cyc_Absyn_Aggrfield*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A3;_LL130: _tmp4A4=_tmp49B->f1;_tmp4A3=_tmp49B->f2;_LL131:;{
void*_tmp49C=(void*)_check_null(_tmp4A3->topt);
void*_tmp49D=_tmp4A4->type;
Cyc_Toc_exp_to_c(nv,_tmp4A3);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp49D) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp49C))
({void*_tmpA17=({
void*_tmpA16=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpA16,Cyc_Absyn_new_exp(_tmp4A3->r,0U));});
# 3099
_tmp4A3->r=_tmpA17;});{
# 3102
int i=Cyc_Toc_get_member_offset(_tmp51C,_tmp4A4->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp49E=({struct _tuple20*_tmp4A0[2U];({struct _tuple20*_tmpA19=({struct _tuple20*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->f1=0,_tmp4A1->f2=field_tag_exp;_tmp4A1;});_tmp4A0[0]=_tmpA19;}),({struct _tuple20*_tmpA18=({struct _tuple20*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2->f1=0,_tmp4A2->f2=_tmp4A3;_tmp4A2;});_tmp4A0[1]=_tmpA18;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A0,sizeof(struct _tuple20*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp49E,0U);
({void*_tmpA1B=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp49F[1U];({struct _tuple20*_tmpA1A=Cyc_Toc_make_field(_tmp4A4->name,umem);_tmp49F[0]=_tmpA1A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49F,sizeof(struct _tuple20*),1U));}));e->r=_tmpA1B;});};};}else{
# 3109
struct Cyc_List_List*_tmp4A5=0;
struct Cyc_List_List*_tmp4A6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51C->impl))->fields;
for(0;_tmp4A6 != 0;_tmp4A6=_tmp4A6->tl){
struct Cyc_List_List*_tmp4A7=_tmp46A;for(0;_tmp4A7 != 0;_tmp4A7=_tmp4A7->tl){
if((*((struct _tuple34*)_tmp4A7->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp4A6->hd){
struct _tuple34*_tmp4A8=(struct _tuple34*)_tmp4A7->hd;struct _tuple34*_tmp4A9=_tmp4A8;struct Cyc_Absyn_Aggrfield*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;_LL133: _tmp4AF=_tmp4A9->f1;_tmp4AE=_tmp4A9->f2;_LL134:;{
void*_tmp4AA=Cyc_Toc_typ_to_c(_tmp4AF->type);
void*_tmp4AB=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4AE->topt));
Cyc_Toc_exp_to_c(nv,_tmp4AE);
# 3119
if(!Cyc_Tcutil_unify(_tmp4AA,_tmp4AB)){
# 3121
if(!Cyc_Tcutil_is_arithmetic_type(_tmp4AA) || !
Cyc_Tcutil_is_arithmetic_type(_tmp4AB))
({struct Cyc_Absyn_Exp*_tmpA1D=({void*_tmpA1C=_tmp4AA;Cyc_Toc_cast_it(_tmpA1C,Cyc_Absyn_copy_exp(_tmp4AE));});_tmp4AE=_tmpA1D;});}
({struct Cyc_List_List*_tmpA1F=({struct Cyc_List_List*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));({struct _tuple20*_tmpA1E=({struct _tuple20*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->f1=0,_tmp4AC->f2=_tmp4AE;_tmp4AC;});_tmp4AD->hd=_tmpA1E;}),_tmp4AD->tl=_tmp4A5;_tmp4AD;});_tmp4A5=_tmpA1F;});
break;};}}}
# 3128
({void*_tmpA20=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4A5));e->r=_tmpA20;});}}
# 3131
goto _LL0;};};};case 31U: _LL43: _tmp522=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp521=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp520=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL44: {
# 3135
struct Cyc_List_List*_tmp4B0=_tmp520->typs;
{struct Cyc_List_List*_tmp4B1=_tmp522;for(0;_tmp4B1 != 0;(_tmp4B1=_tmp4B1->tl,_tmp4B0=_tmp4B0->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4B1->hd;
void*_tmp4B2=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4B0))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp4B2))
({void*_tmpA21=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmpA21;});}}{
# 3146
struct Cyc_Absyn_Exp*_tmp4B3=_tmp521->is_extensible?Cyc_Absyn_var_exp(_tmp520->name,0U):
 Cyc_Toc_datatype_tag(_tmp521,_tmp520->name);
# 3149
if(!Cyc_Toc_is_toplevel(nv)){
# 3151
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp522 != 0;(_tmp522=_tmp522->tl,++ i)){
({struct Cyc_List_List*_tmpA24=({struct Cyc_List_List*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));({struct _tuple20*_tmpA23=({struct _dyneither_ptr*_tmpA22=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmpA22,(struct Cyc_Absyn_Exp*)_tmp522->hd);});_tmp4B4->hd=_tmpA23;}),_tmp4B4->tl=dles;_tmp4B4;});dles=_tmpA24;});}}{
# 3157
struct _tuple20*_tmp4B5=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp4B3);
({void*_tmpA28=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7->tag=29U,({struct _tuple1*_tmpA27=Cyc_Toc_collapse_qvars(_tmp520->name,_tmp521->name);_tmp4B7->f1=_tmpA27;}),_tmp4B7->f2=0,({
struct Cyc_List_List*_tmpA26=({struct Cyc_List_List*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->hd=_tmp4B5,({struct Cyc_List_List*_tmpA25=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp4B6->tl=_tmpA25;});_tmp4B6;});_tmp4B7->f3=_tmpA26;}),_tmp4B7->f4=0;_tmp4B7;});
# 3158
e->r=_tmpA28;});};}else{
# 3163
struct Cyc_List_List*_tmp4B8=0;
for(0;_tmp522 != 0;_tmp522=_tmp522->tl){
({struct Cyc_List_List*_tmpA2A=({struct Cyc_List_List*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));({struct _tuple20*_tmpA29=({struct _tuple20*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->f1=0,_tmp4B9->f2=(struct Cyc_Absyn_Exp*)_tmp522->hd;_tmp4B9;});_tmp4BA->hd=_tmpA29;}),_tmp4BA->tl=_tmp4B8;_tmp4BA;});_tmp4B8=_tmpA2A;});}
({void*_tmpA2D=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));({struct _tuple20*_tmpA2C=({struct _tuple20*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->f1=0,_tmp4BB->f2=_tmp4B3;_tmp4BB;});_tmp4BC->hd=_tmpA2C;}),({
struct Cyc_List_List*_tmpA2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4B8);_tmp4BC->tl=_tmpA2B;});_tmp4BC;}));
# 3166
e->r=_tmpA2D;});}
# 3169
goto _LL0;};}case 32U: _LL45: _LL46:
# 3171
 goto _LL48;case 33U: _LL47: _LL48:
 goto _LL0;case 34U: _LL49: _tmp528=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).is_calloc;_tmp527=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).rgn;_tmp526=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).elt_type;_tmp525=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).num_elts;_tmp524=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).fat_result;_tmp523=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).inline_call;_LL4A: {
# 3175
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp526)));
Cyc_Toc_exp_to_c(nv,_tmp525);
# 3179
if(_tmp524){
struct _tuple1*_tmp4BD=Cyc_Toc_temp_var();
struct _tuple1*_tmp4BE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp528){
xexp=_tmp525;
if(_tmp527 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp527;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpA30=({struct Cyc_Absyn_Exp*_tmpA2F=rgn;struct Cyc_Absyn_Exp*_tmpA2E=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA2F,_tmpA2E,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA30;});}else{
# 3190
({struct Cyc_Absyn_Exp*_tmpA33=({void*_tmpA32=*_tmp526;struct Cyc_Absyn_Exp*_tmpA31=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA32,_tmpA31,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA33;});}
# 3192
({struct Cyc_Absyn_Exp*_tmpA38=({struct Cyc_Absyn_Exp*_tmpA37=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA37,({struct Cyc_Absyn_Exp*_tmp4BF[3U];({
struct Cyc_Absyn_Exp*_tmpA36=Cyc_Absyn_var_exp(_tmp4BE,0U);_tmp4BF[0]=_tmpA36;}),({struct Cyc_Absyn_Exp*_tmpA35=Cyc_Absyn_sizeoftyp_exp(t_c,0U);_tmp4BF[1]=_tmpA35;}),({
struct Cyc_Absyn_Exp*_tmpA34=Cyc_Absyn_var_exp(_tmp4BD,0U);_tmp4BF[2]=_tmpA34;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BF,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3192
rexp=_tmpA38;});}else{
# 3196
if(_tmp527 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp527;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp523)
({struct Cyc_Absyn_Exp*_tmpA3A=({struct Cyc_Absyn_Exp*_tmpA39=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpA39,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA3A;});else{
# 3202
({struct Cyc_Absyn_Exp*_tmpA3C=({struct Cyc_Absyn_Exp*_tmpA3B=rgn;Cyc_Toc_rmalloc_exp(_tmpA3B,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA3C;});}}else{
# 3204
({struct Cyc_Absyn_Exp*_tmpA3E=({void*_tmpA3D=*_tmp526;Cyc_Toc_malloc_exp(_tmpA3D,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA3E;});}
# 3206
({struct Cyc_Absyn_Exp*_tmpA43=({struct Cyc_Absyn_Exp*_tmpA42=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA42,({struct Cyc_Absyn_Exp*_tmp4C0[3U];({struct Cyc_Absyn_Exp*_tmpA41=Cyc_Absyn_var_exp(_tmp4BE,0U);_tmp4C0[0]=_tmpA41;}),({struct Cyc_Absyn_Exp*_tmpA40=Cyc_Absyn_uint_exp(1U,0U);_tmp4C0[1]=_tmpA40;}),({
struct Cyc_Absyn_Exp*_tmpA3F=Cyc_Absyn_var_exp(_tmp4BD,0U);_tmp4C0[2]=_tmpA3F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C0,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3206
rexp=_tmpA43;});}{
# 3209
struct Cyc_Absyn_Stmt*_tmp4C1=({struct _tuple1*_tmpA49=_tmp4BD;void*_tmpA48=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpA47=_tmp525;Cyc_Absyn_declare_stmt(_tmpA49,_tmpA48,_tmpA47,({
struct _tuple1*_tmpA46=_tmp4BE;void*_tmpA45=Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpA44=pexp;Cyc_Absyn_declare_stmt(_tmpA46,_tmpA45,_tmpA44,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpA4A=Cyc_Toc_stmt_exp_r(_tmp4C1);e->r=_tmpA4A;});};}else{
# 3214
if(_tmp528){
if(_tmp527 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp527;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpA4D=({struct Cyc_Absyn_Exp*_tmpA4C=rgn;struct Cyc_Absyn_Exp*_tmpA4B=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA4C,_tmpA4B,_tmp525);})->r;e->r=_tmpA4D;});}else{
# 3220
({void*_tmpA50=({void*_tmpA4F=*_tmp526;struct Cyc_Absyn_Exp*_tmpA4E=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA4F,_tmpA4E,_tmp525);})->r;e->r=_tmpA50;});}}else{
# 3223
if(_tmp527 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp527;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp523)
({void*_tmpA51=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp525))->r;e->r=_tmpA51;});else{
# 3229
({void*_tmpA52=(Cyc_Toc_rmalloc_exp(rgn,_tmp525))->r;e->r=_tmpA52;});}}else{
# 3231
({void*_tmpA53=(Cyc_Toc_malloc_exp(*_tmp526,_tmp525))->r;e->r=_tmpA53;});}}}
# 3235
goto _LL0;}case 35U: _LL4B: _tmp52A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp529=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL4C: {
# 3244
void*e1_old_typ=(void*)_check_null(_tmp52A->topt);
void*e2_old_typ=(void*)_check_null(_tmp529->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp4C2=0U;({struct _dyneither_ptr _tmpA54=({const char*_tmp4C3="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp4C3,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA54,_tag_dyneither(_tmp4C2,sizeof(void*),0U));});});{
# 3252
unsigned int _tmp4C4=e->loc;
struct _tuple1*_tmp4C5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C6=Cyc_Absyn_var_exp(_tmp4C5,_tmp4C4);_tmp4C6->topt=e1_old_typ;{
struct _tuple1*_tmp4C7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C8=Cyc_Absyn_var_exp(_tmp4C7,_tmp4C4);_tmp4C8->topt=e2_old_typ;{
# 3258
struct Cyc_Absyn_Exp*_tmp4C9=({struct Cyc_Absyn_Exp*_tmpA56=Cyc_Tcutil_deep_copy_exp(1,_tmp52A);struct Cyc_Absyn_Exp*_tmpA55=_tmp4C8;Cyc_Absyn_assign_exp(_tmpA56,_tmpA55,_tmp4C4);});_tmp4C9->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4CA=Cyc_Absyn_exp_stmt(_tmp4C9,_tmp4C4);
struct Cyc_Absyn_Exp*_tmp4CB=({struct Cyc_Absyn_Exp*_tmpA58=Cyc_Tcutil_deep_copy_exp(1,_tmp529);struct Cyc_Absyn_Exp*_tmpA57=_tmp4C6;Cyc_Absyn_assign_exp(_tmpA58,_tmpA57,_tmp4C4);});_tmp4CB->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4CC=Cyc_Absyn_exp_stmt(_tmp4CB,_tmp4C4);
# 3263
struct Cyc_Absyn_Stmt*_tmp4CD=({struct _tuple1*_tmpA60=_tmp4C5;void*_tmpA5F=e1_old_typ;struct Cyc_Absyn_Exp*_tmpA5E=_tmp52A;struct Cyc_Absyn_Stmt*_tmpA5D=({
struct _tuple1*_tmpA5C=_tmp4C7;void*_tmpA5B=e2_old_typ;struct Cyc_Absyn_Exp*_tmpA5A=_tmp529;struct Cyc_Absyn_Stmt*_tmpA59=
Cyc_Absyn_seq_stmt(_tmp4CA,_tmp4CC,_tmp4C4);
# 3264
Cyc_Absyn_declare_stmt(_tmpA5C,_tmpA5B,_tmpA5A,_tmpA59,_tmp4C4);});
# 3263
Cyc_Absyn_declare_stmt(_tmpA60,_tmpA5F,_tmpA5E,_tmpA5D,_tmp4C4);});
# 3266
Cyc_Toc_stmt_to_c(nv,_tmp4CD);
({void*_tmpA61=Cyc_Toc_stmt_exp_r(_tmp4CD);e->r=_tmpA61;});
goto _LL0;};};};};};}case 38U: _LL4D: _tmp52C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp52B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL4E: {
# 3271
void*_tmp4CE=Cyc_Tcutil_compress((void*)_check_null(_tmp52C->topt));
Cyc_Toc_exp_to_c(nv,_tmp52C);
{void*_tmp4CF=_tmp4CE;struct Cyc_Absyn_Aggrdecl*_tmp4D5;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CF)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CF)->f1).aggr_info).KnownAggr).tag == 2){_LL136: _tmp4D5=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CF)->f1).aggr_info).KnownAggr).val;_LL137: {
# 3275
struct Cyc_Absyn_Exp*_tmp4D0=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp4D5,_tmp52B),0U);
struct Cyc_Absyn_Exp*_tmp4D1=Cyc_Toc_member_exp(_tmp52C,_tmp52B,0U);
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_Toc_member_exp(_tmp4D1,Cyc_Toc_tag_sp,0U);
({void*_tmpA62=(Cyc_Absyn_eq_exp(_tmp4D2,_tmp4D0,0U))->r;e->r=_tmpA62;});
goto _LL135;}}else{goto _LL138;}}else{_LL138: _LL139:
({void*_tmp4D3=0U;({struct _dyneither_ptr _tmpA63=({const char*_tmp4D4="non-aggregate type in tagcheck";_tag_dyneither(_tmp4D4,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA63,_tag_dyneither(_tmp4D3,sizeof(void*),0U));});});}_LL135:;}
# 3282
goto _LL0;}case 37U: _LL4F: _tmp52D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL50:
 Cyc_Toc_stmt_to_c(nv,_tmp52D);goto _LL0;case 36U: _LL51: _LL52:
({void*_tmp4D6=0U;({struct _dyneither_ptr _tmpA64=({const char*_tmp4D7="UnresolvedMem";_tag_dyneither(_tmp4D7,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA64,_tag_dyneither(_tmp4D6,sizeof(void*),0U));});});case 25U: _LL53: _LL54:
({void*_tmp4D8=0U;({struct _dyneither_ptr _tmpA65=({const char*_tmp4D9="compoundlit";_tag_dyneither(_tmp4D9,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpA65,_tag_dyneither(_tmp4D8,sizeof(void*),0U));});});case 39U: _LL55: _LL56:
({void*_tmp4DA=0U;({struct _dyneither_ptr _tmpA66=({const char*_tmp4DB="valueof(-)";_tag_dyneither(_tmp4DB,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA66,_tag_dyneither(_tmp4DA,sizeof(void*),0U));});});default: _LL57: _LL58:
({void*_tmp4DC=0U;({struct _dyneither_ptr _tmpA67=({const char*_tmp4DD="__asm__";_tag_dyneither(_tmp4DD,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA67,_tag_dyneither(_tmp4DC,sizeof(void*),0U));});});}_LL0:;};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3315 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3317
return({struct _tuple35*_tmp52E=_region_malloc(r,sizeof(*_tmp52E));_tmp52E->f1=0,({struct _dyneither_ptr*_tmpA69=Cyc_Toc_fresh_label();_tmp52E->f2=_tmpA69;}),({struct _dyneither_ptr*_tmpA68=Cyc_Toc_fresh_label();_tmp52E->f3=_tmpA68;}),_tmp52E->f4=sc;_tmp52E;});}
# 3322
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp52F=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3328
if((int)(((_tmp52F->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp532=_tmp52F->orig_pat;if((_tmp532.pattern).tag != 1)_throw_match();(_tmp532.pattern).val;})->topt);
void*_tmp530=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp531=_tmp530;switch(*((int*)_tmp531)){case 0U: _LL1: _LL2:
# 3333
 goto _LL4;case 11U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpA6B=({void*_tmpA6A=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA6A,v);});v=_tmpA6B;});goto _LL0;}_LL0:;}{
# 3339
void*_tmp533=_tmp52F->access;void*_tmp534=_tmp533;struct Cyc_Absyn_Datatypedecl*_tmp53F;struct Cyc_Absyn_Datatypefield*_tmp53E;unsigned int _tmp53D;int _tmp53C;struct _dyneither_ptr*_tmp53B;unsigned int _tmp53A;switch(*((int*)_tmp534)){case 0U: _LLA: _LLB:
# 3344
 goto _LL9;case 1U: _LLC: _LLD:
# 3349
 if(ps->tl != 0){
void*_tmp535=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp536=_tmp535;struct Cyc_Absyn_Datatypedecl*_tmp539;struct Cyc_Absyn_Datatypefield*_tmp538;unsigned int _tmp537;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp536)->tag == 3U){_LL15: _tmp539=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp536)->f1;_tmp538=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp536)->f2;_tmp537=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp536)->f3;_LL16:
# 3352
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpA6E=({void*_tmpA6D=({void*_tmpA6C=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp538->name,_tmp539->name));Cyc_Absyn_cstar_typ(_tmpA6C,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA6D,v);});v=_tmpA6E;});
({struct Cyc_Absyn_Exp*_tmpA70=({struct Cyc_Absyn_Exp*_tmpA6F=v;Cyc_Absyn_aggrarrow_exp(_tmpA6F,Cyc_Absyn_fieldname((int)(_tmp537 + 1)),0U);});v=_tmpA70;});
continue;}else{_LL17: _LL18:
# 3357
 goto _LL14;}_LL14:;}
# 3360
({struct Cyc_Absyn_Exp*_tmpA71=Cyc_Absyn_deref_exp(v,0U);v=_tmpA71;});
goto _LL9;case 2U: _LLE: _tmp53A=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp534)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpA73=({struct Cyc_Absyn_Exp*_tmpA72=v;Cyc_Toc_member_exp(_tmpA72,Cyc_Absyn_fieldname((int)(_tmp53A + 1)),0U);});v=_tmpA73;});goto _LL9;case 4U: _LL10: _tmp53C=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp534)->f1;_tmp53B=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp534)->f2;_LL11:
# 3364
({struct Cyc_Absyn_Exp*_tmpA74=Cyc_Toc_member_exp(v,_tmp53B,0U);v=_tmpA74;});
if(_tmp53C)
({struct Cyc_Absyn_Exp*_tmpA75=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpA75;});
goto _LL9;default: _LL12: _tmp53F=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp534)->f1;_tmp53E=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp534)->f2;_tmp53D=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp534)->f3;_LL13:
# 3369
({struct Cyc_Absyn_Exp*_tmpA77=({struct Cyc_Absyn_Exp*_tmpA76=v;Cyc_Toc_member_exp(_tmpA76,Cyc_Absyn_fieldname((int)(_tmp53D + 1)),0U);});v=_tmpA77;});
goto _LL9;}_LL9:;};}
# 3373
return v;}
# 3378
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp540=t;struct Cyc_Absyn_Datatypedecl*_tmp559;struct Cyc_Absyn_Datatypefield*_tmp558;struct _dyneither_ptr*_tmp557;int _tmp556;int _tmp555;struct Cyc_Absyn_Datatypedecl*_tmp554;struct Cyc_Absyn_Datatypefield*_tmp553;unsigned int _tmp552;struct _dyneither_ptr _tmp551;int _tmp550;void*_tmp54F;struct Cyc_Absyn_Enumfield*_tmp54E;struct Cyc_Absyn_Enumdecl*_tmp54D;struct Cyc_Absyn_Enumfield*_tmp54C;struct Cyc_Absyn_Exp*_tmp54B;switch(*((int*)_tmp540)){case 0U: _LL1: _tmp54B=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp540)->f1;_LL2:
# 3382
 if(_tmp54B == 0)return v;else{return _tmp54B;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA78=v;Cyc_Absyn_eq_exp(_tmpA78,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA79=v;Cyc_Absyn_neq_exp(_tmpA79,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp54D=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp540)->f1;_tmp54C=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp540)->f2;_LL8:
# 3386
 return({struct Cyc_Absyn_Exp*_tmpA7A=v;Cyc_Absyn_eq_exp(_tmpA7A,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->tag=32U,_tmp541->f1=_tmp54D,_tmp541->f2=_tmp54C;_tmp541;}),0U),0U);});case 4U: _LL9: _tmp54F=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp540)->f1;_tmp54E=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp540)->f2;_LLA:
# 3388
 return({struct Cyc_Absyn_Exp*_tmpA7B=v;Cyc_Absyn_eq_exp(_tmpA7B,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->tag=33U,_tmp542->f1=_tmp54F,_tmp542->f2=_tmp54E;_tmp542;}),0U),0U);});case 5U: _LLB: _tmp551=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp540)->f1;_tmp550=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp540)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpA7C=v;Cyc_Absyn_eq_exp(_tmpA7C,Cyc_Absyn_float_exp(_tmp551,_tmp550,0U),0U);});case 6U: _LLD: _tmp552=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp540)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpA7D=v;Cyc_Absyn_eq_exp(_tmpA7D,Cyc_Absyn_signed_int_exp((int)_tmp552,0U),0U);});case 7U: _LLF: _tmp555=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp540)->f1;_tmp554=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp540)->f2;_tmp553=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp540)->f3;_LL10:
# 3394
 LOOP1: {
void*_tmp543=v->r;void*_tmp544=_tmp543;struct Cyc_Absyn_Exp*_tmp546;struct Cyc_Absyn_Exp*_tmp545;switch(*((int*)_tmp544)){case 14U: _LL16: _tmp545=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LL17:
 v=_tmp545;goto LOOP1;case 20U: _LL18: _tmp546=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL19:
 v=_tmp546;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3401
({struct Cyc_Absyn_Exp*_tmpA80=({void*_tmpA7F=({void*_tmpA7E=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp553->name,_tmp554->name));Cyc_Absyn_cstar_typ(_tmpA7E,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA7F,v);});v=_tmpA80;});
return({struct Cyc_Absyn_Exp*_tmpA81=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA81,Cyc_Absyn_uint_exp((unsigned int)_tmp555,0U),0U);});case 8U: _LL11: _tmp557=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp540)->f1;_tmp556=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp540)->f2;_LL12:
# 3406
({struct Cyc_Absyn_Exp*_tmpA83=({struct Cyc_Absyn_Exp*_tmpA82=Cyc_Toc_member_exp(v,_tmp557,0U);Cyc_Toc_member_exp(_tmpA82,Cyc_Toc_tag_sp,0U);});v=_tmpA83;});
return({struct Cyc_Absyn_Exp*_tmpA84=v;Cyc_Absyn_eq_exp(_tmpA84,Cyc_Absyn_signed_int_exp(_tmp556,0U),0U);});default: _LL13: _tmp559=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp540)->f1;_tmp558=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp540)->f2;_LL14:
# 3410
 LOOP2: {
void*_tmp547=v->r;void*_tmp548=_tmp547;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp549;switch(*((int*)_tmp548)){case 14U: _LL1D: _tmp549=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp548)->f2;_LL1E:
 v=_tmp549;goto LOOP2;case 20U: _LL1F: _tmp54A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp548)->f1;_LL20:
 v=_tmp54A;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3417
({struct Cyc_Absyn_Exp*_tmpA87=({void*_tmpA86=({void*_tmpA85=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp558->name,_tmp559->name));Cyc_Absyn_cstar_typ(_tmpA85,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA86,v);});v=_tmpA87;});
return({struct Cyc_Absyn_Exp*_tmpA88=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA88,Cyc_Absyn_var_exp(_tmp558->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3431
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3439
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3441
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp55A=*((struct _tuple33*)ss->hd);struct _tuple33 _tmp55B=_tmp55A;void*_tmp561;_LL1: _tmp561=_tmp55B.f1;_LL2:;{
void*_tmp55C=_tmp561;struct Cyc_Absyn_Exp*_tmp560;struct _dyneither_ptr*_tmp55F;switch(*((int*)_tmp55C)){case 3U: _LL4: _LL5:
# 3447
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3450
 continue;case 8U: _LLA: _tmp55F=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp55C)->f1;_LLB:
# 3452
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpA89=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->tag=3U,_tmp55D->f1=_tmp55F;_tmp55D;});k=_tmpA89;});
continue;case 7U: _LLC: _LLD:
# 3456
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp560=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp55C)->f1;if(_tmp560 != 0){_LLF:
# 3460
({void*_tmpA8A=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->tag=2U,_tmp55E->f1=_tmp560;_tmp55E;});k=_tmpA8A;});
return({struct _tuple36 _tmp7A5;_tmp7A5.f1=0,_tmp7A5.f2=k;_tmp7A5;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp7A6;_tmp7A6.f1=0,_tmp7A6.f2=k;_tmp7A6;});}_LL3:;};}
# 3469
return({struct _tuple36 _tmp7A7;_tmp7A7.f1=c,_tmp7A7.f2=k;_tmp7A7;});}
# 3474
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp562=p;int _tmp56D;int _tmp56C;unsigned int _tmp56B;void*_tmp56A;struct Cyc_Absyn_Enumfield*_tmp569;struct Cyc_Absyn_Enumdecl*_tmp568;struct Cyc_Absyn_Enumfield*_tmp567;switch(*((int*)_tmp562)){case 3U: _LL1: _tmp568=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp562)->f1;_tmp567=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp562)->f2;_LL2:
# 3478
({struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=32U,_tmp563->f1=_tmp568,_tmp563->f2=_tmp567;_tmp563;}),0U);e=_tmpA8B;});goto _LL0;case 4U: _LL3: _tmp56A=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp562)->f1;_tmp569=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp562)->f2;_LL4:
# 3480
({struct Cyc_Absyn_Exp*_tmpA8C=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->tag=33U,_tmp564->f1=_tmp56A,_tmp564->f2=_tmp569;_tmp564;}),0U);e=_tmpA8C;});goto _LL0;case 6U: _LL5: _tmp56B=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp562)->f1;_LL6:
 _tmp56C=(int)_tmp56B;goto _LL8;case 7U: _LL7: _tmp56C=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp562)->f1;_LL8:
 _tmp56D=_tmp56C;goto _LLA;case 8U: _LL9: _tmp56D=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp562)->f2;_LLA:
# 3484
({struct Cyc_Absyn_Exp*_tmpA8D=Cyc_Absyn_uint_exp((unsigned int)_tmp56D,0U);e=_tmpA8D;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp565=0U;({struct _dyneither_ptr _tmpA8E=({const char*_tmp566="compile_pat_test_as_case!";_tag_dyneither(_tmp566,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA8E,_tag_dyneither(_tmp565,sizeof(void*),0U));});});}_LL0:;}
# 3488
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->tag=17U,_tmp56E->f1=e;_tmp56E;}),0U);}
# 3492
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3501
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3504
void*t=(void*)_check_null(p->topt);
void*_tmp56F=p->r;void*_tmp570=_tmp56F;union Cyc_Absyn_AggrInfoU _tmp5A1;struct Cyc_List_List*_tmp5A0;struct Cyc_List_List*_tmp59F;struct Cyc_List_List*_tmp59E;struct Cyc_Absyn_Pat*_tmp59D;struct Cyc_Absyn_Datatypedecl*_tmp59C;struct Cyc_Absyn_Datatypefield*_tmp59B;struct Cyc_List_List*_tmp59A;struct Cyc_Absyn_Vardecl*_tmp599;struct Cyc_Absyn_Pat*_tmp598;struct Cyc_Absyn_Vardecl*_tmp597;struct Cyc_Absyn_Vardecl*_tmp596;struct Cyc_Absyn_Pat*_tmp595;struct Cyc_Absyn_Vardecl*_tmp594;switch(*((int*)_tmp570)){case 2U: _LL1: _tmp594=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp570)->f2;_LL2: {
# 3507
struct Cyc_Absyn_Pat*_tmp571=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp571->topt=p->topt;
_tmp596=_tmp594;_tmp595=_tmp571;goto _LL4;}case 1U: _LL3: _tmp596=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp570)->f1;_tmp595=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp570)->f2;_LL4:
# 3512
({struct Cyc_List_List*_tmpA8F=({struct Cyc_List_List*_tmp572=_region_malloc(rgn,sizeof(*_tmp572));_tmp572->hd=_tmp596,_tmp572->tl=*decls;_tmp572;});*decls=_tmpA8F;});{
struct Cyc_Absyn_Stmt*_tmp573=({struct Cyc_Absyn_Exp*_tmpA90=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->tag=4U,_tmp574->f1=_tmp596;_tmp574;}),0U);Cyc_Absyn_assign_stmt(_tmpA90,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA91=_tmp573;Cyc_Toc_seq_stmt_opt(_tmpA91,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp595));});};case 4U: _LL5: _tmp597=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp570)->f2;_LL6:
# 3517
({struct Cyc_List_List*_tmpA92=({struct Cyc_List_List*_tmp575=_region_malloc(rgn,sizeof(*_tmp575));_tmp575->hd=_tmp597,_tmp575->tl=*decls;_tmp575;});*decls=_tmpA92;});
return({struct Cyc_Absyn_Exp*_tmpA93=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->tag=4U,_tmp576->f1=_tmp597;_tmp576;}),0U);Cyc_Absyn_assign_stmt(_tmpA93,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp599=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp570)->f1;_tmp598=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp570)->f2;_LLA:
# 3522
({struct Cyc_List_List*_tmpA94=({struct Cyc_List_List*_tmp577=_region_malloc(rgn,sizeof(*_tmp577));_tmp577->hd=_tmp599,_tmp577->tl=*decls;_tmp577;});*decls=_tmpA94;});
({void*_tmpA95=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);_tmp599->type=_tmpA95;});{
# 3525
struct Cyc_Absyn_Stmt*_tmp578=({struct Cyc_Absyn_Exp*_tmpA98=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->tag=4U,_tmp579->f1=_tmp599;_tmp579;}),0U);Cyc_Absyn_assign_stmt(_tmpA98,({
void*_tmpA97=({void*_tmpA96=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_typ(_tmpA96,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA97,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA99=_tmp578;Cyc_Toc_seq_stmt_opt(_tmpA99,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp598));});};case 9U: _LLB: _LLC:
# 3530
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp570)->f1)->r)->tag == 8U){_LL17: _tmp59C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp570)->f1)->r)->f1;_tmp59B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp570)->f1)->r)->f2;_tmp59A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp570)->f1)->r)->f3;_LL18:
# 3539
 if(_tmp59A == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp59B->name,_tmp59C->name);
void*_tmp57A=({void*_tmpA9A=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_typ(_tmpA9A,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Toc_cast_it(_tmp57A,path);path=_tmpA9B;});{
int cnt=1;
struct Cyc_List_List*_tmp57B=_tmp59B->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp59A != 0;(_tmp59A=_tmp59A->tl,_tmp57B=((struct Cyc_List_List*)_check_null(_tmp57B))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp57C=(struct Cyc_Absyn_Pat*)_tmp59A->hd;
if(_tmp57C->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp57D=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp57B))->hd)).f2;
void*_tmp57E=(void*)_check_null(_tmp57C->topt);
void*_tmp57F=Cyc_Toc_typ_to_c_array(_tmp57E);
struct Cyc_Absyn_Exp*_tmp580=({struct Cyc_Absyn_Exp*_tmpA9C=path;Cyc_Absyn_aggrarrow_exp(_tmpA9C,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp57D)))
({struct Cyc_Absyn_Exp*_tmpA9D=Cyc_Toc_cast_it(_tmp57F,_tmp580);_tmp580=_tmpA9D;});
({struct Cyc_Absyn_Stmt*_tmpA9F=({struct Cyc_Absyn_Stmt*_tmpA9E=s;Cyc_Toc_seq_stmt_opt(_tmpA9E,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp580,_tmp57C));});s=_tmpA9F;});};}
# 3557
return s;};};}else{_LL21: _tmp59D=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp570)->f1;_LL22:
# 3602
 return({struct _RegionHandle*_tmpAA3=rgn;struct Cyc_Toc_Env**_tmpAA2=nv;struct Cyc_List_List**_tmpAA1=decls;struct Cyc_Absyn_Exp*_tmpAA0=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpAA3,_tmpAA2,_tmpAA1,_tmpAA0,_tmp59D);});}case 8U: _LL19: _tmp59E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp570)->f3;_LL1A:
# 3559
 _tmp59F=_tmp59E;goto _LL1C;case 5U: _LL1B: _tmp59F=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp570)->f1;_LL1C: {
# 3561
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp59F != 0;(_tmp59F=_tmp59F->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp581=(struct Cyc_Absyn_Pat*)_tmp59F->hd;
if(_tmp581->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp582=(void*)_check_null(_tmp581->topt);
struct _dyneither_ptr*_tmp583=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpAA9=({struct Cyc_Absyn_Stmt*_tmpAA8=s;Cyc_Toc_seq_stmt_opt(_tmpAA8,({struct _RegionHandle*_tmpAA7=rgn;struct Cyc_Toc_Env**_tmpAA6=nv;struct Cyc_List_List**_tmpAA5=decls;struct Cyc_Absyn_Exp*_tmpAA4=Cyc_Toc_member_exp(path,_tmp583,0U);Cyc_Toc_extract_pattern_vars(_tmpAA7,_tmpAA6,_tmpAA5,_tmpAA4,_tmp581);}));});s=_tmpAA9;});};}
# 3571
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp570)->f1 == 0){_LL1D: _LL1E:
({void*_tmp584=0U;({struct _dyneither_ptr _tmpAAA=({const char*_tmp585="unresolved aggregate pattern!";_tag_dyneither(_tmp585,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAAA,_tag_dyneither(_tmp584,sizeof(void*),0U));});});}else{_LL1F: _tmp5A1=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp570)->f1)->aggr_info;_tmp5A0=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp570)->f3;_LL20: {
# 3575
struct Cyc_Absyn_Aggrdecl*_tmp586=Cyc_Absyn_get_known_aggrdecl(_tmp5A1);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp5A0 != 0;_tmp5A0=_tmp5A0->tl){
struct _tuple37*_tmp587=(struct _tuple37*)_tmp5A0->hd;
struct Cyc_Absyn_Pat*_tmp588=(*_tmp587).f2;
if(_tmp588->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp589=Cyc_Absyn_designatorlist_to_fieldname((*_tmp587).f1);
void*_tmp58A=(void*)_check_null(_tmp588->topt);
void*_tmp58B=Cyc_Toc_typ_to_c_array(_tmp58A);
struct Cyc_Absyn_Exp*_tmp58C=Cyc_Toc_member_exp(path,_tmp589,0U);
# 3587
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp586->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpAAB=Cyc_Toc_member_exp(_tmp58C,Cyc_Toc_val_sp,0U);_tmp58C=_tmpAAB;});{
void*_tmp58D=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp586->impl))->fields,_tmp589)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp58D))
({struct Cyc_Absyn_Exp*_tmpAAC=Cyc_Toc_cast_it(_tmp58B,_tmp58C);_tmp58C=_tmpAAC;});else{
if(!Cyc_Toc_is_array_type(_tmp58D) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp58D)))
# 3594
({struct Cyc_Absyn_Exp*_tmpAAE=Cyc_Absyn_deref_exp(({void*_tmpAAD=Cyc_Absyn_cstar_typ(_tmp58B,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpAAD,
Cyc_Absyn_address_exp(_tmp58C,0U));}),0U);
# 3594
_tmp58C=_tmpAAE;});}
# 3597
({struct Cyc_Absyn_Stmt*_tmpAB0=({struct Cyc_Absyn_Stmt*_tmpAAF=s;Cyc_Toc_seq_stmt_opt(_tmpAAF,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp58C,_tmp588));});s=_tmpAB0;});};};}
# 3599
return s;}}case 15U: _LL23: _LL24:
# 3604
({void*_tmp58E=0U;({struct _dyneither_ptr _tmpAB1=({const char*_tmp58F="unknownid pat";_tag_dyneither(_tmp58F,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB1,_tag_dyneither(_tmp58E,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp590=0U;({struct _dyneither_ptr _tmpAB2=({const char*_tmp591="unknowncall pat";_tag_dyneither(_tmp591,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB2,_tag_dyneither(_tmp590,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp592=0U;({struct _dyneither_ptr _tmpAB3=({const char*_tmp593="exp pat";_tag_dyneither(_tmp593,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB3,_tag_dyneither(_tmp592,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3615
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3623
void*_tmp5A2=dopt;struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*_tmp5DF;void*_tmp5DE;struct Cyc_Tcpat_Rhs*_tmp5DD;if(_tmp5A2 == 0){_LL1: _LL2:
# 3625
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp5A2)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp5DD=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp5A2)->f1;_LL6:
# 3630
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5A3=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5A4=_tmp5A3;int*_tmp5AB;struct _dyneither_ptr*_tmp5AA;struct _dyneither_ptr*_tmp5A9;struct Cyc_Absyn_Switch_clause*_tmp5A8;_LLA: _tmp5AB=(int*)& _tmp5A4->f1;_tmp5AA=_tmp5A4->f2;_tmp5A9=_tmp5A4->f3;_tmp5A8=_tmp5A4->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp5A5=_tmp5A8->body;
if(_tmp5A5 == _tmp5DD->rhs){
# 3635
if(*_tmp5AB)
return Cyc_Absyn_goto_stmt(_tmp5AA,0U);
*_tmp5AB=1;{
# 3639
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAB6=rgn;struct Cyc_List_List**_tmpAB5=decls;struct Cyc_Absyn_Exp*_tmpAB4=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAB6,& nv,_tmpAB5,_tmpAB4,_tmp5A8->pattern);});
# 3642
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp5A9,_tmp5A8->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAB7=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpAB7;});
({struct Cyc_Absyn_Stmt*_tmpAB8=Cyc_Absyn_label_stmt(_tmp5AA,res,0U);res=_tmpAB8;});
({struct Cyc_List_List*_tmpABA=({struct Cyc_List_List*_tmp5A7=_region_malloc(rgn,sizeof(*_tmp5A7));({struct _tuple38*_tmpAB9=({struct _tuple38*_tmp5A6=_region_malloc(rgn,sizeof(*_tmp5A6));_tmp5A6->f1=nv,_tmp5A6->f2=_tmp5A9,_tmp5A6->f3=_tmp5A5;_tmp5A6;});_tmp5A7->hd=_tmpAB9;}),_tmp5A7->tl=*bodies;_tmp5A7;});*bodies=_tmpABA;});
return res;};}};}
# 3650
({void*_tmp5AC=0U;({struct _dyneither_ptr _tmpABB=({const char*_tmp5AD="couldn't find rhs!";_tag_dyneither(_tmp5AD,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpABB,_tag_dyneither(_tmp5AC,sizeof(void*),0U));});});default: _LL7: _tmp5E0=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A2)->f1;_tmp5DF=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A2)->f2;_tmp5DE=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A2)->f3;_LL8: {
# 3653
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5DE,lscs,v);
# 3655
struct Cyc_Absyn_Exp*_tmp5AE=({struct Cyc_List_List*_tmpABC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5E0);Cyc_Toc_compile_path(_tmpABC,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp5AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5DF);
# 3658
struct _tuple36 _tmp5B0=Cyc_Toc_admits_switch(_tmp5AF);struct _tuple36 _tmp5B1=_tmp5B0;int _tmp5DC;void*_tmp5DB;_LLD: _tmp5DC=_tmp5B1.f1;_tmp5DB=_tmp5B1.f2;_LLE:;
if(_tmp5DC > 1){
# 3662
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));
({struct Cyc_Absyn_Switch_clause*_tmpABE=({struct Cyc_Absyn_Switch_clause*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));({struct Cyc_Absyn_Pat*_tmpABD=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5C1->pattern=_tmpABD;}),_tmp5C1->pat_vars=0,_tmp5C1->where_clause=0,_tmp5C1->body=res,_tmp5C1->loc=0U;_tmp5C1;});_tmp5C2->hd=_tmpABE;}),_tmp5C2->tl=0;_tmp5C2;});
# 3665
for(0;_tmp5AF != 0;_tmp5AF=_tmp5AF->tl){
struct _tuple33 _tmp5B2=*((struct _tuple33*)_tmp5AF->hd);struct _tuple33 _tmp5B3=_tmp5B2;void*_tmp5B9;void*_tmp5B8;_LL10: _tmp5B9=_tmp5B3.f1;_tmp5B8=_tmp5B3.f2;_LL11:;{
# 3668
struct Cyc_Absyn_Pat*_tmp5B4=Cyc_Toc_compile_pat_test_as_case(_tmp5B9);
# 3670
struct Cyc_Absyn_Stmt*_tmp5B5=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5B8,lscs,v);
# 3672
({struct Cyc_List_List*_tmpAC0=({struct Cyc_List_List*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));({struct Cyc_Absyn_Switch_clause*_tmpABF=({struct Cyc_Absyn_Switch_clause*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->pattern=_tmp5B4,_tmp5B6->pat_vars=0,_tmp5B6->where_clause=0,_tmp5B6->body=_tmp5B5,_tmp5B6->loc=0U;_tmp5B6;});_tmp5B7->hd=_tmpABF;}),_tmp5B7->tl=new_lscs;_tmp5B7;});new_lscs=_tmpAC0;});};}
# 3674
{void*_tmp5BA=_tmp5DB;struct _dyneither_ptr*_tmp5BF;switch(*((int*)_tmp5BA)){case 1U: _LL13: _LL14:
# 3677
 LOOP1: {
void*_tmp5BB=_tmp5AE->r;void*_tmp5BC=_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BE;struct Cyc_Absyn_Exp*_tmp5BD;switch(*((int*)_tmp5BC)){case 14U: _LL1C: _tmp5BD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5BC)->f2;_LL1D:
 _tmp5AE=_tmp5BD;goto LOOP1;case 20U: _LL1E: _tmp5BE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5BC)->f1;_LL1F:
 _tmp5AE=_tmp5BE;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3683
({struct Cyc_Absyn_Exp*_tmpAC2=Cyc_Absyn_deref_exp(({void*_tmpAC1=Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpAC1,_tmp5AE);}),0U);_tmp5AE=_tmpAC2;});goto _LL12;case 3U: _LL15: _tmp5BF=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp5BA)->f1;_LL16:
# 3686
({struct Cyc_Absyn_Exp*_tmpAC4=({struct Cyc_Absyn_Exp*_tmpAC3=Cyc_Toc_member_exp(_tmp5AE,_tmp5BF,0U);Cyc_Toc_member_exp(_tmpAC3,Cyc_Toc_tag_sp,0U);});_tmp5AE=_tmpAC4;});goto _LL12;case 2U: _LL17: _LL18:
# 3688
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3691
({struct Cyc_Absyn_Stmt*_tmpAC5=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->tag=10U,_tmp5C0->f1=_tmp5AE,_tmp5C0->f2=new_lscs,_tmp5C0->f3=0;_tmp5C0;}),0U);res=_tmpAC5;});}else{
# 3695
void*_tmp5C3=_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DA;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C3)->tag == 2U){_LL23: _tmp5DA=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C3)->f1;_LL24: {
# 3697
struct Cyc_List_List*_tmp5C4=_tmp5AF;void*_tmp5D3;struct Cyc_Tcpat_Rhs*_tmp5D2;if(_tmp5C4 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp5C4)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5C4)->tl == 0){_LL28: _tmp5D3=((struct _tuple33*)_tmp5C4->hd)->f1;_tmp5D2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp5C4->hd)->f2)->f1;_LL29:
# 3703
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5C5=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5C6=_tmp5C5;int*_tmp5CD;struct _dyneither_ptr*_tmp5CC;struct _dyneither_ptr*_tmp5CB;struct Cyc_Absyn_Switch_clause*_tmp5CA;_LL2D: _tmp5CD=(int*)& _tmp5C6->f1;_tmp5CC=_tmp5C6->f2;_tmp5CB=_tmp5C6->f3;_tmp5CA=_tmp5C6->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp5C7=_tmp5CA->body;
if(_tmp5C7 == _tmp5D2->rhs){
# 3708
if(*_tmp5CD)
return Cyc_Absyn_goto_stmt(_tmp5CC,0U);
*_tmp5CD=1;{
# 3712
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAC8=rgn;struct Cyc_List_List**_tmpAC7=decls;struct Cyc_Absyn_Exp*_tmpAC6=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAC8,& nv,_tmpAC7,_tmpAC6,_tmp5CA->pattern);});
# 3715
Cyc_Toc_exp_to_c(nv,_tmp5DA);{
# 3718
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp5CB,_tmp5CA->body,0U);
({struct Cyc_Absyn_Stmt*_tmpAC9=Cyc_Absyn_ifthenelse_stmt(_tmp5DA,r,res,0U);r=_tmpAC9;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpACA=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpACA;});
({struct Cyc_Absyn_Stmt*_tmpACB=Cyc_Absyn_label_stmt(_tmp5CC,r,0U);r=_tmpACB;});
({struct Cyc_List_List*_tmpACD=({struct Cyc_List_List*_tmp5C9=_region_malloc(rgn,sizeof(*_tmp5C9));({struct _tuple38*_tmpACC=({struct _tuple38*_tmp5C8=_region_malloc(rgn,sizeof(*_tmp5C8));_tmp5C8->f1=nv,_tmp5C8->f2=_tmp5CB,_tmp5C8->f3=_tmp5C7;_tmp5C8;});_tmp5C9->hd=_tmpACC;}),_tmp5C9->tl=*bodies;_tmp5C9;});*bodies=_tmpACD;});
return r;};};}};}
# 3727
({void*_tmp5CE=0U;({struct _dyneither_ptr _tmpACE=({const char*_tmp5CF="couldn't find rhs!";_tag_dyneither(_tmp5CF,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACE,_tag_dyneither(_tmp5CE,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp5D0=0U;({struct _dyneither_ptr _tmpACF=({const char*_tmp5D1="bad where clause in match compiler";_tag_dyneither(_tmp5D1,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACF,_tag_dyneither(_tmp5D0,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3732
 for(0;_tmp5AF != 0;_tmp5AF=_tmp5AF->tl){
struct _tuple33 _tmp5D4=*((struct _tuple33*)_tmp5AF->hd);struct _tuple33 _tmp5D5=_tmp5D4;void*_tmp5D9;void*_tmp5D8;_LL30: _tmp5D9=_tmp5D5.f1;_tmp5D8=_tmp5D5.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp5D6=Cyc_Toc_compile_pat_test(_tmp5AE,_tmp5D9);
struct Cyc_Absyn_Stmt*_tmp5D7=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5D8,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpAD0=Cyc_Absyn_ifthenelse_stmt(_tmp5D6,_tmp5D7,res,0U);res=_tmpAD0;});};}}_LL22:;}
# 3740
return res;}}}_LL0:;}
# 3750
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3752
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp5E1=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp5E2=_tmp5E1;struct Cyc_Toc_Env**_tmp5E4;struct Cyc_Absyn_Stmt*_tmp5E3;_LL1: _tmp5E4=(struct Cyc_Toc_Env**)& _tmp5E2->f1;_tmp5E3=_tmp5E2->f3;_LL2:;
if(_tmp5E3 == s)return _tmp5E4;}
# 3758
return 0;}
# 3762
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3765
void*_tmp5E5=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3768
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5E6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5E6;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5E7=Cyc_Toc_share_env(rgn,nv);
# 3775
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3780
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp5E7,& mydecls,& mybodies,dopt,lscs,v);
# 3788
{struct Cyc_List_List*_tmp5E8=lscs;for(0;_tmp5E8 != 0;_tmp5E8=_tmp5E8->tl){
struct _tuple35*_tmp5E9=(struct _tuple35*)_tmp5E8->hd;struct _tuple35*_tmp5EA=_tmp5E9;struct _dyneither_ptr*_tmp5F2;struct Cyc_Absyn_Switch_clause*_tmp5F1;_LL1: _tmp5F2=_tmp5EA->f3;_tmp5F1=_tmp5EA->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp5F1->body;
# 3792
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3795
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5EB=*envp;
# 3798
if(_tmp5E8->tl != 0){
struct _tuple35*_tmp5EC=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp5E8->tl))->hd;struct _tuple35*_tmp5ED=_tmp5EC;struct _dyneither_ptr*_tmp5F0;struct Cyc_Absyn_Switch_clause*_tmp5EF;_LL4: _tmp5F0=_tmp5ED->f3;_tmp5EF=_tmp5ED->f4;_LL5:;{
# 3802
struct Cyc_Toc_Env**_tmp5EE=Cyc_Toc_find_case_env(mybodies,_tmp5EF->body);
# 3807
if(_tmp5EE == 0)
({struct Cyc_Toc_Env*_tmpAD1=Cyc_Toc_last_switchclause_env(rgn,_tmp5EB,end_l);Cyc_Toc_stmt_to_c(_tmpAD1,s);});else{
# 3811
struct Cyc_List_List*vs=0;
if(_tmp5EF->pat_vars != 0){
({struct Cyc_List_List*_tmpAD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5EF->pat_vars))->v)).f1);vs=_tmpAD2;});
({struct Cyc_List_List*_tmpAD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpAD3;});}
# 3816
({struct Cyc_Toc_Env*_tmpAD4=Cyc_Toc_non_last_switchclause_env(rgn,_tmp5EB,end_l,_tmp5F0,vs);Cyc_Toc_stmt_to_c(_tmpAD4,s);});}};}else{
# 3820
({struct Cyc_Toc_Env*_tmpAD5=Cyc_Toc_last_switchclause_env(rgn,_tmp5EB,end_l);Cyc_Toc_stmt_to_c(_tmpAD5,s);});}};};}}{
# 3824
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpAD7=test_tree;Cyc_Absyn_seq_stmt(_tmpAD7,({struct _dyneither_ptr*_tmpAD6=end_l;Cyc_Absyn_label_stmt(_tmpAD6,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3826
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp5F3=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpAD8=Cyc_Toc_temp_var();_tmp5F3->name=_tmpAD8;});
({void*_tmpAD9=Cyc_Toc_typ_to_c_array(_tmp5F3->type);_tmp5F3->type=_tmpAD9;});
({struct Cyc_Absyn_Stmt*_tmpADB=({struct Cyc_Absyn_Decl*_tmpADA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->tag=0U,_tmp5F4->f1=_tmp5F3;_tmp5F4;}),0U);Cyc_Absyn_decl_stmt(_tmpADA,res,0U);});res=_tmpADB;});}
# 3833
({void*_tmpADF=({struct _tuple1*_tmpADE=v;void*_tmpADD=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpADC=e;Cyc_Absyn_declare_stmt(_tmpADE,_tmpADD,_tmpADC,res,0U);})->r;whole_s->r=_tmpADF;});
_npop_handler(0U);return;};}
# 3772
;_pop_region(rgn);};}
# 3839
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3841
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3846
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3848
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAE1=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpAE1,({struct Cyc_List_List*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));
({struct Cyc_Absyn_Exp*_tmpAE0=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp5F5->hd=_tmpAE0;}),_tmp5F5->tl=0;_tmp5F5;}),0U);}),0U);}
# 3852
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpAE3=({struct Cyc_Absyn_Stmt*_tmpAE2=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpAE2,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpAE3;});}
# 3857
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3859
while(1){
void*_tmp5F6=s->r;void*_tmp5F7=_tmp5F6;struct Cyc_Absyn_Stmt*_tmp64E;struct Cyc_List_List*_tmp64D;void*_tmp64C;struct Cyc_Absyn_Stmt*_tmp64B;struct Cyc_Absyn_Exp*_tmp64A;struct _dyneither_ptr*_tmp649;struct Cyc_Absyn_Stmt*_tmp648;struct Cyc_Absyn_Decl*_tmp647;struct Cyc_Absyn_Stmt*_tmp646;struct Cyc_List_List*_tmp645;struct Cyc_Absyn_Switch_clause**_tmp644;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_List_List*_tmp642;void*_tmp641;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Stmt*_tmp63D;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Stmt*_tmp63B;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Stmt*_tmp639;struct Cyc_Absyn_Stmt*_tmp638;struct Cyc_Absyn_Exp*_tmp637;struct Cyc_Absyn_Stmt*_tmp636;struct Cyc_Absyn_Stmt*_tmp635;struct Cyc_Absyn_Exp*_tmp634;switch(*((int*)_tmp5F7)){case 0U: _LL1: _LL2:
# 3862
 return;case 1U: _LL3: _tmp634=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_LL4:
# 3864
 Cyc_Toc_exp_to_c(nv,_tmp634);
return;case 2U: _LL5: _tmp636=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_tmp635=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2;_LL6:
# 3867
 Cyc_Toc_stmt_to_c(nv,_tmp636);
s=_tmp635;
continue;case 3U: _LL7: _tmp637=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_LL8: {
# 3871
void*topt=0;
if(_tmp637 != 0){
({void*_tmpAE4=Cyc_Toc_typ_to_c((void*)_check_null(_tmp637->topt));topt=_tmpAE4;});
Cyc_Toc_exp_to_c(nv,_tmp637);}{
# 3877
int _tmp5F8=Cyc_Toc_get_npop(s);
if(_tmp5F8 > 0){
if(topt != 0){
struct _tuple1*_tmp5F9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp5FA=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp5F9,0U),0U);
({void*_tmpAE9=({struct _tuple1*_tmpAE8=_tmp5F9;void*_tmpAE7=topt;struct Cyc_Absyn_Exp*_tmpAE6=_tmp637;Cyc_Absyn_declare_stmt(_tmpAE8,_tmpAE7,_tmpAE6,({
struct Cyc_Absyn_Stmt*_tmpAE5=Cyc_Toc_make_npop_handler(_tmp5F8);Cyc_Absyn_seq_stmt(_tmpAE5,_tmp5FA,0U);}),0U);})->r;
# 3882
s->r=_tmpAE9;});}else{
# 3886
Cyc_Toc_do_npop_before(_tmp5F8,s);}}
# 3888
return;};}case 4U: _LL9: _tmp63A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_tmp639=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2;_tmp638=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f3;_LLA:
# 3890
 Cyc_Toc_exp_to_c(nv,_tmp63A);
Cyc_Toc_stmt_to_c(nv,_tmp639);
s=_tmp638;
continue;case 5U: _LLB: _tmp63C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1).f1;_tmp63B=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2;_LLC:
# 3895
 Cyc_Toc_exp_to_c(nv,_tmp63C);{
struct _RegionHandle _tmp5FB=_new_region("temp");struct _RegionHandle*temp=& _tmp5FB;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAEA=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAEA,_tmp63B);});
# 3899
_npop_handler(0U);return;
# 3896
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3901
struct Cyc_Toc_Env*_tmp5FC=nv;struct _dyneither_ptr**_tmp5FD;_LL22: _tmp5FD=_tmp5FC->break_lab;_LL23:;
if(_tmp5FD != 0)
({void*_tmpAEB=Cyc_Toc_goto_stmt_r(*_tmp5FD);s->r=_tmpAEB;});
# 3905
({int _tmpAEC=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAEC,s);});
return;}case 7U: _LLF: _LL10: {
# 3908
struct Cyc_Toc_Env*_tmp5FE=nv;struct _dyneither_ptr**_tmp5FF;_LL25: _tmp5FF=_tmp5FE->continue_lab;_LL26:;
if(_tmp5FF != 0)
({void*_tmpAED=Cyc_Toc_goto_stmt_r(*_tmp5FF);s->r=_tmpAED;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3914
({int _tmpAEE=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAEE,s);});
return;case 9U: _LL13: _tmp640=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_tmp63F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2).f1;_tmp63E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f3).f1;_tmp63D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f4;_LL14:
# 3918
 Cyc_Toc_exp_to_c(nv,_tmp640);Cyc_Toc_exp_to_c(nv,_tmp63F);Cyc_Toc_exp_to_c(nv,_tmp63E);{
struct _RegionHandle _tmp600=_new_region("temp");struct _RegionHandle*temp=& _tmp600;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAEF=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAEF,_tmp63D);});
# 3922
_npop_handler(0U);return;
# 3919
;_pop_region(temp);};case 10U: _LL15: _tmp643=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_tmp642=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2;_tmp641=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f3;_LL16:
# 3924
 Cyc_Toc_xlate_switch(nv,s,_tmp643,_tmp642,_tmp641);
return;case 11U: _LL17: _tmp645=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_tmp644=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2;_LL18: {
# 3927
struct Cyc_Toc_Env*_tmp601=nv;struct Cyc_Toc_FallthruInfo*_tmp60B;_LL28: _tmp60B=_tmp601->fallthru_info;_LL29:;
if(_tmp60B == 0)
({void*_tmp602=0U;({struct _dyneither_ptr _tmpAF0=({const char*_tmp603="fallthru in unexpected place";_tag_dyneither(_tmp603,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAF0,_tag_dyneither(_tmp602,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp604=*_tmp60B;struct Cyc_Toc_FallthruInfo _tmp605=_tmp604;struct _dyneither_ptr*_tmp60A;struct Cyc_List_List*_tmp609;_LL2B: _tmp60A=_tmp605.label;_tmp609=_tmp605.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp60A,0U);
# 3933
({int _tmpAF1=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAF1,s2);});{
struct Cyc_List_List*_tmp606=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp609);
struct Cyc_List_List*_tmp607=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp645);
for(0;_tmp606 != 0;(_tmp606=_tmp606->tl,_tmp607=_tmp607->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp607))->hd);
({struct Cyc_Absyn_Stmt*_tmpAF4=({struct Cyc_Absyn_Stmt*_tmpAF3=({struct Cyc_Absyn_Exp*_tmpAF2=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->tag=5U,_tmp608->f1=(struct Cyc_Absyn_Vardecl*)_tmp606->hd;_tmp608;}),0U);Cyc_Absyn_assign_stmt(_tmpAF2,(struct Cyc_Absyn_Exp*)_tmp607->hd,0U);});Cyc_Absyn_seq_stmt(_tmpAF3,s2,0U);});s2=_tmpAF4;});}
# 3941
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp647=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_tmp646=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2;_LL1A:
# 3947
{void*_tmp60C=_tmp647->r;void*_tmp60D=_tmp60C;struct Cyc_Absyn_Tvar*_tmp628;struct Cyc_Absyn_Vardecl*_tmp627;struct Cyc_Absyn_Exp*_tmp626;struct Cyc_Absyn_Fndecl*_tmp625;struct Cyc_List_List*_tmp624;struct Cyc_Absyn_Pat*_tmp623;struct Cyc_Absyn_Exp*_tmp622;void*_tmp621;struct Cyc_Absyn_Vardecl*_tmp620;switch(*((int*)_tmp60D)){case 0U: _LL2E: _tmp620=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp60D)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp620,_tmp646);goto _LL2D;case 2U: _LL30: _tmp623=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60D)->f1;_tmp622=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60D)->f3;_tmp621=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60D)->f4;_LL31:
# 3955
{void*_tmp60E=_tmp623->r;void*_tmp60F=_tmp60E;struct Cyc_Absyn_Vardecl*_tmp611;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp60F)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp60F)->f2)->r)->tag == 0U){_LL3B: _tmp611=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp60F)->f1;_LL3C:
# 3957
({struct _tuple1*_tmpAF5=Cyc_Toc_temp_var();_tmp611->name=_tmpAF5;});
_tmp611->initializer=_tmp622;
({void*_tmpAF6=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=0U,_tmp610->f1=_tmp611;_tmp610;});_tmp647->r=_tmpAF6;});
Cyc_Toc_local_decl_to_c(nv,_tmp611,_tmp646);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3966
({void*_tmpAF7=(Cyc_Toc_letdecl_to_c(nv,_tmp623,_tmp621,(void*)_check_null(_tmp622->topt),_tmp622,_tmp646))->r;s->r=_tmpAF7;});
goto _LL3A;}_LL3A:;}
# 3969
goto _LL2D;case 3U: _LL32: _tmp624=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp60D)->f1;_LL33: {
# 3973
struct Cyc_List_List*_tmp612=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp624);
if(_tmp612 == 0)
({void*_tmp613=0U;({struct _dyneither_ptr _tmpAF8=({const char*_tmp614="empty Letv_d";_tag_dyneither(_tmp614,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF8,_tag_dyneither(_tmp613,sizeof(void*),0U));});});
({void*_tmpAF9=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=0U,_tmp615->f1=(struct Cyc_Absyn_Vardecl*)_tmp612->hd;_tmp615;});_tmp647->r=_tmpAF9;});
_tmp612=_tmp612->tl;
for(0;_tmp612 != 0;_tmp612=_tmp612->tl){
struct Cyc_Absyn_Decl*_tmp616=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->tag=0U,_tmp617->f1=(struct Cyc_Absyn_Vardecl*)_tmp612->hd;_tmp617;}),0U);
({void*_tmpAFB=({struct Cyc_Absyn_Decl*_tmpAFA=_tmp616;Cyc_Absyn_decl_stmt(_tmpAFA,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpAFB;});}
# 3982
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp625=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp60D)->f1;_LL35:
# 3985
 Cyc_Toc_fndecl_to_c(nv,_tmp625,0);
Cyc_Toc_stmt_to_c(nv,_tmp646);
goto _LL2D;case 4U: _LL36: _tmp628=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60D)->f1;_tmp627=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60D)->f2;_tmp626=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60D)->f3;_LL37: {
# 3989
struct Cyc_Absyn_Stmt*_tmp618=_tmp646;
# 3991
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp627->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3998
Cyc_Toc_stmt_to_c(nv,_tmp618);
if(Cyc_Absyn_no_regions)
({void*_tmpAFF=({struct _tuple1*_tmpAFE=x_var;void*_tmpAFD=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAFC=
Cyc_Absyn_uint_exp(0U,0U);
# 4000
Cyc_Absyn_declare_stmt(_tmpAFE,_tmpAFD,_tmpAFC,_tmp618,0U);})->r;s->r=_tmpAFF;});else{
# 4002
if((unsigned int)_tmp626){
# 4004
Cyc_Toc_exp_to_c(nv,_tmp626);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpB01=_tmp626;Cyc_Absyn_aggrarrow_exp(_tmpB01,({struct _dyneither_ptr*_tmp61A=_cycalloc(sizeof(*_tmp61A));({struct _dyneither_ptr _tmpB00=({const char*_tmp619="h";_tag_dyneither(_tmp619,sizeof(char),2U);});*_tmp61A=_tmpB00;});_tmp61A;}),0U);}),0U);
({void*_tmpB02=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp618,0U))->r;s->r=_tmpB02;});};}else{
# 4014
({void*_tmpB0F=({
struct _tuple1*_tmpB0E=rh_var;void*_tmpB0D=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpB0C=({
struct Cyc_Absyn_Exp*_tmpB04=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpB04,({struct Cyc_List_List*_tmp61B=_cycalloc(sizeof(*_tmp61B));
({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp61B->hd=_tmpB03;}),_tmp61B->tl=0;_tmp61B;}),0U);});
# 4015
Cyc_Absyn_declare_stmt(_tmpB0E,_tmpB0D,_tmpB0C,({
# 4019
struct _tuple1*_tmpB0B=x_var;void*_tmpB0A=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpB09=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpB0B,_tmpB0A,_tmpB09,({
struct Cyc_Absyn_Stmt*_tmpB08=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB05=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpB05,({struct Cyc_Absyn_Exp*_tmp61C[1U];_tmp61C[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61C,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpB08,({
struct Cyc_Absyn_Stmt*_tmpB07=_tmp618;Cyc_Absyn_seq_stmt(_tmpB07,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB06=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpB06,({struct Cyc_Absyn_Exp*_tmp61D[1U];_tmp61D[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61D,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 4014
s->r=_tmpB0F;});}}
# 4025
return;}default: _LL38: _LL39:
({void*_tmp61E=0U;({struct _dyneither_ptr _tmpB10=({const char*_tmp61F="bad nested declaration within function";_tag_dyneither(_tmp61F,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB10,_tag_dyneither(_tmp61E,sizeof(void*),0U));});});}_LL2D:;}
# 4028
return;case 13U: _LL1B: _tmp649=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_tmp648=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2;_LL1C:
# 4030
 s=_tmp648;continue;case 14U: _LL1D: _tmp64B=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_tmp64A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2).f1;_LL1E: {
# 4032
struct _RegionHandle _tmp629=_new_region("temp");struct _RegionHandle*temp=& _tmp629;_push_region(temp);
({struct Cyc_Toc_Env*_tmpB11=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpB11,_tmp64B);});
Cyc_Toc_exp_to_c(nv,_tmp64A);
# 4036
_npop_handler(0U);return;
# 4032
;_pop_region(temp);}default: _LL1F: _tmp64E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f1;_tmp64D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f2;_tmp64C=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5F7)->f3;_LL20: {
# 4050 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4060
e_exp->topt=e_typ;{
struct _RegionHandle _tmp62A=_new_region("temp");struct _RegionHandle*temp=& _tmp62A;_push_region(temp);
# 4063
Cyc_Toc_stmt_to_c(nv,_tmp64E);{
struct Cyc_Absyn_Stmt*_tmp62B=({struct Cyc_Absyn_Stmt*_tmpB12=_tmp64E;Cyc_Absyn_seq_stmt(_tmpB12,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 4068
struct Cyc_Absyn_Stmt*_tmp62C=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->tag=10U,_tmp633->f1=e_exp,_tmp633->f2=_tmp64D,_tmp633->f3=_tmp64C;_tmp633;}),0U);
# 4070
Cyc_Toc_stmt_to_c(nv,_tmp62C);{
# 4073
struct Cyc_Absyn_Exp*_tmp62D=({
struct Cyc_Absyn_Exp*_tmpB14=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpB14,({struct Cyc_List_List*_tmp632=_cycalloc(sizeof(*_tmp632));
({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp632->hd=_tmpB13;}),_tmp632->tl=0;_tmp632;}),0U);});
# 4077
struct Cyc_Absyn_Stmt*_tmp62E=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB16=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpB16,({struct Cyc_List_List*_tmp631=_cycalloc(sizeof(*_tmp631));
({struct Cyc_Absyn_Exp*_tmpB15=Cyc_Absyn_address_exp(h_exp,0U);_tmp631->hd=_tmpB15;}),_tmp631->tl=0;_tmp631;}),0U);}),0U);
# 4081
struct Cyc_Absyn_Exp*_tmp62F=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp630=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpB25=({
struct _tuple1*_tmpB24=h_var;void*_tmpB23=h_typ;Cyc_Absyn_declare_stmt(_tmpB24,_tmpB23,0,({
struct Cyc_Absyn_Stmt*_tmpB22=_tmp62E;Cyc_Absyn_seq_stmt(_tmpB22,({
struct _tuple1*_tmpB21=was_thrown_var;void*_tmpB20=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpB1F=_tmp62F;Cyc_Absyn_declare_stmt(_tmpB21,_tmpB20,_tmpB1F,({
struct Cyc_Absyn_Stmt*_tmpB1E=({struct Cyc_Absyn_Exp*_tmpB18=_tmp62D;struct Cyc_Absyn_Stmt*_tmpB17=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp630,0U);
# 4087
Cyc_Absyn_ifthenelse_stmt(_tmpB18,_tmpB17,
# 4089
Cyc_Toc_skip_stmt_dl(),0U);});
# 4087
Cyc_Absyn_seq_stmt(_tmpB1E,({
# 4090
struct Cyc_Absyn_Exp*_tmpB1D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpB1C=_tmp62B;Cyc_Absyn_ifthenelse_stmt(_tmpB1D,_tmpB1C,({
# 4092
struct _tuple1*_tmpB1B=e_var;void*_tmpB1A=e_typ;struct Cyc_Absyn_Exp*_tmpB19=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 4092
Cyc_Absyn_declare_stmt(_tmpB1B,_tmpB1A,_tmpB19,_tmp62C,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 4083
s->r=_tmpB25;});};};
# 4096
_npop_handler(0U);return;
# 4061
;_pop_region(temp);};}}_LL0:;}}
# 4105
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpB26=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpB26;});{
struct _RegionHandle _tmp64F=_new_region("frgn");struct _RegionHandle*frgn=& _tmp64F;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp650=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp651=f->args;for(0;_tmp651 != 0;_tmp651=_tmp651->tl){
struct _tuple1*_tmp652=({struct _tuple1*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->f1=Cyc_Absyn_Loc_n,_tmp653->f2=(*((struct _tuple9*)_tmp651->hd)).f1;_tmp653;});
({void*_tmpB27=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp651->hd)).f3);(*((struct _tuple9*)_tmp651->hd)).f3=_tmpB27;});}}
# 4121
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 4125
({struct Cyc_Hashtable_Table**_tmpB29=({struct Cyc_Hashtable_Table**_tmp654=_cycalloc(sizeof(*_tmp654));({struct Cyc_Hashtable_Table*_tmpB28=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp654=_tmpB28;});_tmp654;});Cyc_Toc_fn_pop_table=_tmpB29;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp655=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp656=_tmp655;struct _dyneither_ptr*_tmp65F;struct Cyc_Absyn_Tqual _tmp65E;void*_tmp65D;int _tmp65C;_LL1: _tmp65F=_tmp656.name;_tmp65E=_tmp656.tq;_tmp65D=_tmp656.type;_tmp65C=_tmp656.inject;_LL2:;{
void*_tmp657=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp65D,(void*)& Cyc_Absyn_HeapRgn_val,_tmp65E,Cyc_Absyn_false_conref));
struct _tuple1*_tmp658=({struct _tuple1*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->f1=Cyc_Absyn_Loc_n,_tmp65B->f2=(struct _dyneither_ptr*)_check_null(_tmp65F);_tmp65B;});
({struct Cyc_List_List*_tmpB2C=({struct Cyc_List_List*_tmpB2B=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB2B,({struct Cyc_List_List*_tmp65A=_cycalloc(sizeof(*_tmp65A));({struct _tuple9*_tmpB2A=({struct _tuple9*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->f1=_tmp65F,_tmp659->f2=_tmp65E,_tmp659->f3=_tmp657;_tmp659;});_tmp65A->hd=_tmpB2A;}),_tmp65A->tl=0;_tmp65A;}));});f->args=_tmpB2C;});
f->cyc_varargs=0;};}
# 4134
{struct Cyc_List_List*_tmp660=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp660 != 0;_tmp660=_tmp660->tl){
({void*_tmpB2D=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp660->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp660->hd)->type=_tmpB2D;});}}
# 4137
({struct Cyc_Toc_Env*_tmpB2E=Cyc_Toc_clear_toplevel(frgn,_tmp650);Cyc_Toc_stmt_to_c(_tmpB2E,f->body);});}
# 4114
;_pop_region(frgn);};}
# 4141
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp661=s;switch(_tmp661){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple40{struct Cyc_Toc_TocState*f1;struct _tuple39*f2;};
# 4158 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 4161
struct _tuple40 _tmp662=*env;struct _tuple40 _tmp663=_tmp662;struct Cyc_Toc_TocState*_tmp698;struct Cyc_Absyn_Aggrdecl*_tmp697;int _tmp696;_LL1: _tmp698=_tmp663.f1;_tmp697=(_tmp663.f2)->f1;_tmp696=(_tmp663.f2)->f2;_LL2:;{
struct _tuple1*_tmp664=_tmp697->name;
struct Cyc_Toc_TocState _tmp665=*_tmp698;struct Cyc_Toc_TocState _tmp666=_tmp665;struct Cyc_Dict_Dict*_tmp695;_LL4: _tmp695=_tmp666.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple19**_tmp667=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp695,_tmp664);
if(_tmp667 == 0){
seen_defn_before=0;{
struct _tuple19*v;
if(_tmp697->kind == Cyc_Absyn_StructA)
({struct _tuple19*_tmpB30=({struct _tuple19*_tmp668=_region_malloc(d,sizeof(*_tmp668));_tmp668->f1=_tmp697,({void*_tmpB2F=Cyc_Absyn_strctq(_tmp664);_tmp668->f2=_tmpB2F;});_tmp668;});v=_tmpB30;});else{
# 4172
({struct _tuple19*_tmpB32=({struct _tuple19*_tmp669=_region_malloc(d,sizeof(*_tmp669));_tmp669->f1=_tmp697,({void*_tmpB31=Cyc_Absyn_unionq_typ(_tmp664);_tmp669->f2=_tmpB31;});_tmp669;});v=_tmpB32;});}
({struct Cyc_Dict_Dict _tmpB33=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*_tmp695,_tmp664,v);*_tmp695=_tmpB33;});};}else{
# 4175
struct _tuple19*_tmp66A=*_tmp667;struct _tuple19*_tmp66B=_tmp66A;struct Cyc_Absyn_Aggrdecl*_tmp66E;void*_tmp66D;_LL7: _tmp66E=_tmp66B->f1;_tmp66D=_tmp66B->f2;_LL8:;
if(_tmp66E->impl == 0){
({struct Cyc_Dict_Dict _tmpB36=({struct Cyc_Dict_Dict _tmpB35=*_tmp695;struct _tuple1*_tmpB34=_tmp664;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpB35,_tmpB34,({struct _tuple19*_tmp66C=_region_malloc(d,sizeof(*_tmp66C));_tmp66C->f1=_tmp697,_tmp66C->f2=_tmp66D;_tmp66C;}));});*_tmp695=_tmpB36;});
seen_defn_before=0;}else{
# 4180
seen_defn_before=1;}}{
# 4182
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->kind=_tmp697->kind,_tmp694->sc=Cyc_Absyn_Public,_tmp694->name=_tmp697->name,_tmp694->tvs=0,_tmp694->impl=0,_tmp694->expected_mem_kind=0,_tmp694->attributes=_tmp697->attributes;_tmp694;});
# 4189
if(_tmp697->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpB37=({struct Cyc_Absyn_AggrdeclImpl*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->exist_vars=0,_tmp66F->rgn_po=0,_tmp66F->fields=0,_tmp66F->tagged=0;_tmp66F;});new_ad->impl=_tmpB37;});{
# 4194
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp670=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp697->impl))->fields;for(0;_tmp670 != 0;_tmp670=_tmp670->tl){
# 4198
struct Cyc_Absyn_Aggrfield*_tmp671=(struct Cyc_Absyn_Aggrfield*)_tmp670->hd;
void*_tmp672=_tmp671->type;
struct Cyc_List_List*_tmp673=_tmp671->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp672)) && (
_tmp697->kind == Cyc_Absyn_StructA  && _tmp670->tl == 0  || _tmp697->kind == Cyc_Absyn_UnionA)){
# 4212 "toc.cyc"
void*_tmp674=Cyc_Tcutil_compress(_tmp672);void*_tmp675=_tmp674;void*_tmp67D;struct Cyc_Absyn_Tqual _tmp67C;union Cyc_Absyn_Constraint*_tmp67B;unsigned int _tmp67A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->tag == 8U){_LLA: _tmp67D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->f1).elt_type;_tmp67C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->f1).tq;_tmp67B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->f1).zero_term;_tmp67A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->f1).zt_loc;_LLB:
# 4215
({void*_tmpB39=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->tag=8U,(_tmp676->f1).elt_type=_tmp67D,(_tmp676->f1).tq=_tmp67C,({struct Cyc_Absyn_Exp*_tmpB38=Cyc_Absyn_uint_exp(0U,0U);(_tmp676->f1).num_elts=_tmpB38;}),(_tmp676->f1).zero_term=_tmp67B,(_tmp676->f1).zt_loc=_tmp67A;_tmp676;});_tmp672=_tmpB39;});
goto _LL9;}else{_LLC: _LLD:
# 4218
({struct Cyc_List_List*_tmpB3B=({struct Cyc_List_List*_tmp678=_cycalloc(sizeof(*_tmp678));({void*_tmpB3A=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->tag=6U,_tmp677->f1=0;_tmp677;});_tmp678->hd=_tmpB3A;}),_tmp678->tl=_tmp673;_tmp678;});_tmp673=_tmpB3B;});
({void*_tmpB40=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->tag=8U,({void*_tmpB3F=Cyc_Absyn_void_star_typ();(_tmp679->f1).elt_type=_tmpB3F;}),({
struct Cyc_Absyn_Tqual _tmpB3E=Cyc_Absyn_empty_tqual(0U);(_tmp679->f1).tq=_tmpB3E;}),({
struct Cyc_Absyn_Exp*_tmpB3D=Cyc_Absyn_uint_exp(0U,0U);(_tmp679->f1).num_elts=_tmpB3D;}),({
union Cyc_Absyn_Constraint*_tmpB3C=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp679->f1).zero_term=_tmpB3C;}),(_tmp679->f1).zt_loc=0U;_tmp679;});
# 4219
_tmp672=_tmpB40;});}_LL9:;}{
# 4225
struct Cyc_Absyn_Aggrfield*_tmp67E=({struct Cyc_Absyn_Aggrfield*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->name=_tmp671->name,_tmp690->tq=Cyc_Toc_mt_tq,({
# 4227
void*_tmpB41=Cyc_Toc_typ_to_c(_tmp672);_tmp690->type=_tmpB41;}),_tmp690->width=_tmp671->width,_tmp690->attributes=_tmp673,_tmp690->requires_clause=0;_tmp690;});
# 4235
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp697->impl))->tagged){
void*_tmp67F=_tmp67E->type;
struct _dyneither_ptr*_tmp680=_tmp67E->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68D=({struct Cyc_String_pa_PrintArg_struct _tmp7A9;_tmp7A9.tag=0U,_tmp7A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp697->name).f2);_tmp7A9;});struct Cyc_String_pa_PrintArg_struct _tmp68E=({struct Cyc_String_pa_PrintArg_struct _tmp7A8;_tmp7A8.tag=0U,_tmp7A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp680);_tmp7A8;});void*_tmp68B[2U];_tmp68B[0]=& _tmp68D,_tmp68B[1]=& _tmp68E;({struct _dyneither_ptr _tmpB42=({const char*_tmp68C="_union_%s_%s";_tag_dyneither(_tmp68C,sizeof(char),13U);});Cyc_aprintf(_tmpB42,_tag_dyneither(_tmp68B,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp68A=_cycalloc(sizeof(*_tmp68A));*_tmp68A=s;_tmp68A;});
struct Cyc_Absyn_Aggrfield*_tmp681=({struct Cyc_Absyn_Aggrfield*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689->name=Cyc_Toc_val_sp,_tmp689->tq=Cyc_Toc_mt_tq,_tmp689->type=_tmp67F,_tmp689->width=0,_tmp689->attributes=0,_tmp689->requires_clause=0;_tmp689;});
struct Cyc_Absyn_Aggrfield*_tmp682=({struct Cyc_Absyn_Aggrfield*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->name=Cyc_Toc_tag_sp,_tmp688->tq=Cyc_Toc_mt_tq,_tmp688->type=Cyc_Absyn_sint_typ,_tmp688->width=0,_tmp688->attributes=0,_tmp688->requires_clause=0;_tmp688;});
struct Cyc_List_List*_tmp683=({struct Cyc_Absyn_Aggrfield*_tmp687[2U];_tmp687[0]=_tmp682,_tmp687[1]=_tmp681;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp687,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp684=Cyc_Toc_make_c_struct_defn(str,_tmp683);
({struct Cyc_List_List*_tmpB44=({struct Cyc_List_List*_tmp686=_cycalloc(sizeof(*_tmp686));({struct Cyc_Absyn_Decl*_tmpB43=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->tag=5U,_tmp685->f1=_tmp684;_tmp685;}),0U);_tmp686->hd=_tmpB43;}),_tmp686->tl=Cyc_Toc_result_decls;_tmp686;});Cyc_Toc_result_decls=_tmpB44;});
({void*_tmpB45=Cyc_Absyn_strct(str);_tmp67E->type=_tmpB45;});}
# 4247
({struct Cyc_List_List*_tmpB46=({struct Cyc_List_List*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->hd=_tmp67E,_tmp68F->tl=new_fields;_tmp68F;});new_fields=_tmpB46;});};}}
# 4249
({struct Cyc_List_List*_tmpB47=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpB47;});};}
# 4251
if(_tmp696)
({struct Cyc_List_List*_tmpB4A=({struct Cyc_List_List*_tmp693=_cycalloc(sizeof(*_tmp693));({struct Cyc_Absyn_Decl*_tmpB49=({struct Cyc_Absyn_Decl*_tmp692=_cycalloc(sizeof(*_tmp692));({void*_tmpB48=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->tag=5U,_tmp691->f1=new_ad;_tmp691;});_tmp692->r=_tmpB48;}),_tmp692->loc=0U;_tmp692;});_tmp693->hd=_tmpB49;}),_tmp693->tl=Cyc_Toc_result_decls;_tmp693;});Cyc_Toc_result_decls=_tmpB4A;});
return 0;};};};}
# 4256
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple39 p=({struct _tuple39 _tmp7AA;_tmp7AA.f1=ad,_tmp7AA.f2=add_to_decls;_tmp7AA;});
((int(*)(struct _tuple39*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple41{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4285 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4288
struct _tuple41 _tmp699=*env;struct _tuple41 _tmp69A=_tmp699;struct Cyc_Set_Set**_tmp6AB;struct Cyc_Absyn_Datatypedecl*_tmp6AA;_LL1: _tmp6AB=(_tmp69A.f1)->datatypes_so_far;_tmp6AA=_tmp69A.f2;_LL2:;{
struct _tuple1*_tmp69B=_tmp6AA->name;
if(_tmp6AA->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp6AB,_tmp69B))
return 0;
({struct Cyc_Set_Set*_tmpB4B=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp6AB,_tmp69B);*_tmp6AB=_tmpB4B;});
{struct Cyc_List_List*_tmp69C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6AA->fields))->v;for(0;_tmp69C != 0;_tmp69C=_tmp69C->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp69C->hd;
# 4296
struct Cyc_List_List*_tmp69D=0;
int i=1;
{struct Cyc_List_List*_tmp69E=f->typs;for(0;_tmp69E != 0;(_tmp69E=_tmp69E->tl,i ++)){
struct _dyneither_ptr*_tmp69F=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp6A0=({struct Cyc_Absyn_Aggrfield*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2->name=_tmp69F,_tmp6A2->tq=(*((struct _tuple12*)_tmp69E->hd)).f1,({
void*_tmpB4C=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp69E->hd)).f2);_tmp6A2->type=_tmpB4C;}),_tmp6A2->width=0,_tmp6A2->attributes=0,_tmp6A2->requires_clause=0;_tmp6A2;});
({struct Cyc_List_List*_tmpB4D=({struct Cyc_List_List*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->hd=_tmp6A0,_tmp6A1->tl=_tmp69D;_tmp6A1;});_tmp69D=_tmpB4D;});}}
# 4304
({struct Cyc_List_List*_tmpB50=({struct Cyc_List_List*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));({struct Cyc_Absyn_Aggrfield*_tmpB4F=({struct Cyc_Absyn_Aggrfield*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->name=Cyc_Toc_tag_sp,_tmp6A3->tq=Cyc_Toc_mt_tq,_tmp6A3->type=Cyc_Absyn_sint_typ,_tmp6A3->width=0,_tmp6A3->attributes=0,_tmp6A3->requires_clause=0;_tmp6A3;});_tmp6A4->hd=_tmpB4F;}),({
struct Cyc_List_List*_tmpB4E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp69D);_tmp6A4->tl=_tmpB4E;});_tmp6A4;});
# 4304
_tmp69D=_tmpB50;});{
# 4306
struct Cyc_Absyn_Aggrdecl*_tmp6A5=({struct _dyneither_ptr*_tmpB52=({struct _dyneither_ptr*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));({struct _dyneither_ptr _tmpB51=({const char*_tmp6A8="";_tag_dyneither(_tmp6A8,sizeof(char),1U);});*_tmp6A9=_tmpB51;});_tmp6A9;});Cyc_Toc_make_c_struct_defn(_tmpB52,_tmp69D);});
({struct _tuple1*_tmpB53=Cyc_Toc_collapse_qvars(f->name,_tmp6AA->name);_tmp6A5->name=_tmpB53;});
({struct Cyc_List_List*_tmpB55=({struct Cyc_List_List*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));({struct Cyc_Absyn_Decl*_tmpB54=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->tag=5U,_tmp6A6->f1=_tmp6A5;_tmp6A6;}),0U);_tmp6A7->hd=_tmpB54;}),_tmp6A7->tl=Cyc_Toc_result_decls;_tmp6A7;});Cyc_Toc_result_decls=_tmpB55;});};}}
# 4310
return 0;};}
# 4313
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4332 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4335
struct _tuple41 _tmp6AC=*env;struct _tuple41 _tmp6AD=_tmp6AC;struct Cyc_Toc_TocState*_tmp6CF;struct Cyc_Absyn_Datatypedecl*_tmp6CE;_LL1: _tmp6CF=_tmp6AD.f1;_tmp6CE=_tmp6AD.f2;_LL2:;
if(_tmp6CE->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6AE=*_tmp6CF;struct Cyc_Toc_TocState _tmp6AF=_tmp6AE;struct Cyc_Dict_Dict*_tmp6CD;_LL4: _tmp6CD=_tmp6AF.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp6B0=_tmp6CE->name;
{struct Cyc_List_List*_tmp6B1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6CE->fields))->v;for(0;_tmp6B1 != 0;_tmp6B1=_tmp6B1->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp6B1->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6B2=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp6B3=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp6B2,Cyc_Absyn_false_conref,0U);
# 4346
int*_tmp6B4=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp6CD,f->name);int*_tmp6B5=_tmp6B4;if(_tmp6B5 == 0){_LL7: _LL8: {
# 4348
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpB56=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpB56;});{
# 4352
struct Cyc_Absyn_Vardecl*_tmp6B6=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B3,initopt);
_tmp6B6->sc=f->sc;
({struct Cyc_List_List*_tmpB58=({struct Cyc_List_List*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));({struct Cyc_Absyn_Decl*_tmpB57=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->tag=0U,_tmp6B7->f1=_tmp6B6;_tmp6B7;}),0U);_tmp6B8->hd=_tmpB57;}),_tmp6B8->tl=Cyc_Toc_result_decls;_tmp6B8;});Cyc_Toc_result_decls=_tmpB58;});
({struct Cyc_Dict_Dict _tmpB59=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6CD,f->name,f->sc != Cyc_Absyn_Extern);
# 4355
*_tmp6CD=_tmpB59;});{
# 4357
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6B9=f->typs;for(0;_tmp6B9 != 0;(_tmp6B9=_tmp6B9->tl,i ++)){
struct _dyneither_ptr*_tmp6BA=({struct _dyneither_ptr*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));({struct _dyneither_ptr _tmpB5B=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6C0=({struct Cyc_Int_pa_PrintArg_struct _tmp7AB;_tmp7AB.tag=1U,_tmp7AB.f1=(unsigned long)i;_tmp7AB;});void*_tmp6BE[1U];_tmp6BE[0]=& _tmp6C0;({struct _dyneither_ptr _tmpB5A=({const char*_tmp6BF="f%d";_tag_dyneither(_tmp6BF,sizeof(char),4U);});Cyc_aprintf(_tmpB5A,_tag_dyneither(_tmp6BE,sizeof(void*),1U));});});*_tmp6C1=_tmpB5B;});_tmp6C1;});
struct Cyc_Absyn_Aggrfield*_tmp6BB=({struct Cyc_Absyn_Aggrfield*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD->name=_tmp6BA,_tmp6BD->tq=(*((struct _tuple12*)_tmp6B9->hd)).f1,({
void*_tmpB5C=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp6B9->hd)).f2);_tmp6BD->type=_tmpB5C;}),_tmp6BD->width=0,_tmp6BD->attributes=0,_tmp6BD->requires_clause=0;_tmp6BD;});
({struct Cyc_List_List*_tmpB5D=({struct Cyc_List_List*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->hd=_tmp6BB,_tmp6BC->tl=fields;_tmp6BC;});fields=_tmpB5D;});}}
# 4365
({struct Cyc_List_List*_tmpB61=({struct Cyc_List_List*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));({struct Cyc_Absyn_Aggrfield*_tmpB60=({struct Cyc_Absyn_Aggrfield*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->name=Cyc_Toc_tag_sp,_tmp6C2->tq=Cyc_Toc_mt_tq,({
void*_tmpB5F=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp6C2->type=_tmpB5F;}),_tmp6C2->width=0,_tmp6C2->attributes=0,_tmp6C2->requires_clause=0;_tmp6C2;});
# 4365
_tmp6C3->hd=_tmpB60;}),({
# 4367
struct Cyc_List_List*_tmpB5E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6C3->tl=_tmpB5E;});_tmp6C3;});
# 4365
fields=_tmpB61;});{
# 4368
struct Cyc_Absyn_Aggrdecl*_tmp6C4=({struct _dyneither_ptr*_tmpB63=({struct _dyneither_ptr*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));({struct _dyneither_ptr _tmpB62=({const char*_tmp6C7="";_tag_dyneither(_tmp6C7,sizeof(char),1U);});*_tmp6C8=_tmpB62;});_tmp6C8;});Cyc_Toc_make_c_struct_defn(_tmpB63,fields);});
({struct _tuple1*_tmpB64=Cyc_Toc_collapse_qvars(f->name,_tmp6CE->name);_tmp6C4->name=_tmpB64;});
({struct Cyc_List_List*_tmpB66=({struct Cyc_List_List*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));({struct Cyc_Absyn_Decl*_tmpB65=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->tag=5U,_tmp6C5->f1=_tmp6C4;_tmp6C5;}),0U);_tmp6C6->hd=_tmpB65;}),_tmp6C6->tl=Cyc_Toc_result_decls;_tmp6C6;});Cyc_Toc_result_decls=_tmpB66;});
goto _LL6;};};};}}else{if(*((int*)_tmp6B5)== 0){_LL9: _LLA:
# 4373
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6C9=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp6CA=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B3,_tmp6C9);
_tmp6CA->sc=f->sc;
({struct Cyc_List_List*_tmpB68=({struct Cyc_List_List*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));({struct Cyc_Absyn_Decl*_tmpB67=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->tag=0U,_tmp6CB->f1=_tmp6CA;_tmp6CB;}),0U);_tmp6CC->hd=_tmpB67;}),_tmp6CC->tl=Cyc_Toc_result_decls;_tmp6CC;});Cyc_Toc_result_decls=_tmpB68;});
({struct Cyc_Dict_Dict _tmpB69=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6CD,f->name,1);*_tmp6CD=_tmpB69;});}
# 4380
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4384
return 0;};};}
# 4387
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4391
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4397
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpB6A=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpB6A;});
# 4401
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4410
struct _RegionHandle _tmp6D0=_new_region("temp");struct _RegionHandle*temp=& _tmp6D0;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6D1=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp6D1,init);}
# 4411
;_pop_region(temp);}else{
# 4415
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4418
void*_tmp6D2=Cyc_Tcutil_compress(old_typ);void*_tmp6D3=_tmp6D2;void*_tmp6DA;struct Cyc_Absyn_Exp*_tmp6D9;union Cyc_Absyn_Constraint*_tmp6D8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D3)->tag == 8U){_LL1: _tmp6DA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D3)->f1).elt_type;_tmp6D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D3)->f1).num_elts;_tmp6D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D3)->f1).zero_term;_LL2:
# 4420
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D8)){
if(_tmp6D9 == 0)
({void*_tmp6D4=0U;({struct _dyneither_ptr _tmpB6B=({const char*_tmp6D5="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp6D5,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB6B,_tag_dyneither(_tmp6D4,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp6D9;
struct Cyc_Absyn_Exp*_tmp6D6=({struct Cyc_Absyn_Exp*_tmpB6D=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpB6D,({
struct Cyc_Absyn_Exp*_tmpB6C=num_elts;Cyc_Absyn_add_exp(_tmpB6C,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4427
struct Cyc_Absyn_Exp*_tmp6D7=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpB6F=({struct Cyc_Absyn_Stmt*_tmpB6E=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp6D6,_tmp6D7,0U),0U);Cyc_Toc_seq_stmt_r(_tmpB6E,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4428
s->r=_tmpB6F;});};}
# 4431
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4439
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6DB=d;struct Cyc_List_List*_tmp6E2;struct Cyc_List_List*_tmp6E1;void**_tmp6E0;struct Cyc_Tcpat_Rhs*_tmp6DF;switch(*((int*)_tmp6DB)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6DF=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6DB)->f1;_LL4:
 _tmp6DF->rhs=success;return d;default: _LL5: _tmp6E2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DB)->f1;_tmp6E1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DB)->f2;_tmp6E0=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DB)->f3;_LL6:
# 4444
({void*_tmpB70=Cyc_Toc_rewrite_decision(*_tmp6E0,success);*_tmp6E0=_tmpB70;});
for(0;_tmp6E1 != 0;_tmp6E1=_tmp6E1->tl){
struct _tuple33*_tmp6DC=(struct _tuple33*)_tmp6E1->hd;struct _tuple33*_tmp6DD=_tmp6DC;void**_tmp6DE;_LL8: _tmp6DE=(void**)& _tmp6DD->f2;_LL9:;
({void*_tmpB71=Cyc_Toc_rewrite_decision(*_tmp6DE,success);*_tmp6DE=_tmpB71;});}
# 4449
return d;}_LL0:;}
# 4460 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4462
struct _RegionHandle _tmp6E3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6E3;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6E4=Cyc_Toc_share_env(rgn,nv);
void*_tmp6E5=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp6E4,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4472
struct Cyc_Absyn_Stmt*_tmp6E6=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4476
({void*_tmpB72=Cyc_Toc_rewrite_decision(dopt,_tmp6E6);dopt=_tmpB72;});{
# 4478
struct Cyc_Absyn_Switch_clause*_tmp6E7=({struct Cyc_Absyn_Switch_clause*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->pattern=p,_tmp6F3->pat_vars=0,_tmp6F3->where_clause=0,_tmp6F3->body=_tmp6E6,_tmp6F3->loc=0U;_tmp6F3;});
struct Cyc_List_List*_tmp6E8=({struct _RegionHandle*_tmpB75=rgn;struct _RegionHandle*_tmpB74=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpB75,Cyc_Toc_gen_labels,_tmpB74,({struct Cyc_Absyn_Switch_clause*_tmp6F2[1U];_tmp6F2[0]=_tmp6E7;({struct _RegionHandle*_tmpB73=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpB73,_tag_dyneither(_tmp6F2,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4481
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4485
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp6E4,& mydecls,& mybodies,dopt,_tmp6E8,v);
# 4488
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp6E9=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp6EA=_tmp6E9;struct Cyc_Toc_Env*_tmp6EC;struct Cyc_Absyn_Stmt*_tmp6EB;_LL1: _tmp6EC=_tmp6EA->f1;_tmp6EB=_tmp6EA->f3;_LL2:;
if(_tmp6EB == _tmp6E6){senv=_tmp6EC;goto FOUND_ENV;}}
# 4493
({void*_tmp6ED=0U;({struct _dyneither_ptr _tmpB76=({const char*_tmp6EE="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp6EE,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB76,_tag_dyneither(_tmp6ED,sizeof(void*),0U));});});
FOUND_ENV:
# 4497
 Cyc_Toc_stmt_to_c(senv,s);{
# 4499
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4501
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp6EF=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpB77=Cyc_Toc_temp_var();_tmp6EF->name=_tmpB77;});
({void*_tmpB78=Cyc_Toc_typ_to_c_array(_tmp6EF->type);_tmp6EF->type=_tmpB78;});
({struct Cyc_Absyn_Stmt*_tmpB7A=({struct Cyc_Absyn_Decl*_tmpB79=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0->tag=0U,_tmp6F0->f1=_tmp6EF;_tmp6F0;}),0U);Cyc_Absyn_decl_stmt(_tmpB79,res,0U);});res=_tmpB7A;});}
# 4508
({struct Cyc_Absyn_Stmt*_tmpB7E=({struct _tuple1*_tmpB7D=v;void*_tmpB7C=Cyc_Toc_typ_to_c(_tmp6E5);struct Cyc_Absyn_Exp*_tmpB7B=e;Cyc_Absyn_declare_stmt(_tmpB7D,_tmpB7C,_tmpB7B,res,0U);});res=_tmpB7E;});{
struct Cyc_Absyn_Stmt*_tmp6F1=res;_npop_handler(0U);return _tmp6F1;};};};};}
# 4463
;_pop_region(rgn);}
# 4516
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6F4=e->r;void*_tmp6F5=_tmp6F4;struct Cyc_Absyn_MallocInfo*_tmp720;struct Cyc_Absyn_Stmt*_tmp71F;void**_tmp71E;void**_tmp71D;struct Cyc_List_List*_tmp71C;struct Cyc_List_List*_tmp71B;void**_tmp71A;struct Cyc_List_List*_tmp719;void**_tmp718;struct Cyc_Absyn_Exp*_tmp717;struct Cyc_Absyn_Exp*_tmp716;struct Cyc_List_List*_tmp715;struct Cyc_Absyn_Exp*_tmp714;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Exp*_tmp70A;struct Cyc_Absyn_Exp*_tmp709;struct Cyc_Absyn_Exp*_tmp708;struct Cyc_Absyn_Exp*_tmp707;struct Cyc_Absyn_Exp*_tmp706;struct Cyc_List_List*_tmp705;struct Cyc_Absyn_Exp*_tmp704;struct Cyc_Absyn_Exp*_tmp703;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_Absyn_Exp*_tmp701;struct Cyc_Absyn_Exp*_tmp700;struct Cyc_Absyn_Exp*_tmp6FF;struct Cyc_Absyn_Exp*_tmp6FE;struct Cyc_Absyn_Exp*_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FC;switch(*((int*)_tmp6F5)){case 41U: _LL1: _tmp6FC=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL2:
 _tmp6FD=_tmp6FC;goto _LL4;case 20U: _LL3: _tmp6FD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL4:
 _tmp6FE=_tmp6FD;goto _LL6;case 21U: _LL5: _tmp6FE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL6:
 _tmp6FF=_tmp6FE;goto _LL8;case 22U: _LL7: _tmp6FF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL8:
 _tmp700=_tmp6FF;goto _LLA;case 15U: _LL9: _tmp700=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LLA:
 _tmp701=_tmp700;goto _LLC;case 11U: _LLB: _tmp701=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LLC:
 _tmp702=_tmp701;goto _LLE;case 12U: _LLD: _tmp702=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LLE:
 _tmp703=_tmp702;goto _LL10;case 18U: _LLF: _tmp703=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL10:
 _tmp704=_tmp703;goto _LL12;case 5U: _LL11: _tmp704=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp704);goto _LL0;case 3U: _LL13: _tmp705=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp705);goto _LL0;case 7U: _LL15: _tmp707=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_tmp706=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL16:
 _tmp709=_tmp707;_tmp708=_tmp706;goto _LL18;case 8U: _LL17: _tmp709=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_tmp708=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL18:
 _tmp70B=_tmp709;_tmp70A=_tmp708;goto _LL1A;case 9U: _LL19: _tmp70B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_tmp70A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL1A:
 _tmp70D=_tmp70B;_tmp70C=_tmp70A;goto _LL1C;case 23U: _LL1B: _tmp70D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_tmp70C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL1C:
 _tmp70F=_tmp70D;_tmp70E=_tmp70C;goto _LL1E;case 35U: _LL1D: _tmp70F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_tmp70E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL1E:
 _tmp711=_tmp70F;_tmp710=_tmp70E;goto _LL20;case 4U: _LL1F: _tmp711=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_tmp710=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6F5)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp711);Cyc_Toc_exptypes_to_c(_tmp710);goto _LL0;case 6U: _LL21: _tmp714=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_tmp713=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_tmp712=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F5)->f3;_LL22:
# 4535
 Cyc_Toc_exptypes_to_c(_tmp714);Cyc_Toc_exptypes_to_c(_tmp713);Cyc_Toc_exptypes_to_c(_tmp712);goto _LL0;case 10U: _LL23: _tmp716=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_tmp715=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL24:
# 4537
 Cyc_Toc_exptypes_to_c(_tmp716);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp715);goto _LL0;case 14U: _LL25: _tmp718=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_tmp717=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL26:
({void*_tmpB7F=Cyc_Toc_typ_to_c(*_tmp718);*_tmp718=_tmpB7F;});Cyc_Toc_exptypes_to_c(_tmp717);goto _LL0;case 25U: _LL27: _tmp71A=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1)->f3;_tmp719=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL28:
# 4540
({void*_tmpB80=Cyc_Toc_typ_to_c(*_tmp71A);*_tmp71A=_tmpB80;});
_tmp71B=_tmp719;goto _LL2A;case 36U: _LL29: _tmp71B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6F5)->f2;_LL2A:
 _tmp71C=_tmp71B;goto _LL2C;case 26U: _LL2B: _tmp71C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL2C:
# 4544
 for(0;_tmp71C != 0;_tmp71C=_tmp71C->tl){
struct _tuple20 _tmp6F6=*((struct _tuple20*)_tmp71C->hd);struct _tuple20 _tmp6F7=_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F8;_LL56: _tmp6F8=_tmp6F7.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp6F8);}
# 4548
goto _LL0;case 19U: _LL2D: _tmp71D=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL2E:
 _tmp71E=_tmp71D;goto _LL30;case 17U: _LL2F: _tmp71E=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL30:
({void*_tmpB81=Cyc_Toc_typ_to_c(*_tmp71E);*_tmp71E=_tmpB81;});goto _LL0;case 37U: _LL31: _tmp71F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp71F);goto _LL0;case 34U: _LL33: _tmp720=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6F5)->f1;_LL34:
# 4553
 if(_tmp720->elt_type != 0)
({void**_tmpB83=({void**_tmp6F9=_cycalloc(sizeof(*_tmp6F9));({void*_tmpB82=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp720->elt_type)));*_tmp6F9=_tmpB82;});_tmp6F9;});_tmp720->elt_type=_tmpB83;});
Cyc_Toc_exptypes_to_c(_tmp720->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4563
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
({void*_tmp6FA=0U;({unsigned int _tmpB85=e->loc;struct _dyneither_ptr _tmpB84=({const char*_tmp6FB="Cyclone expression within C code";_tag_dyneither(_tmp6FB,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpB85,_tmpB84,_tag_dyneither(_tmp6FA,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4577
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp721=d->r;void*_tmp722=_tmp721;struct Cyc_Absyn_Typedefdecl*_tmp72C;struct Cyc_Absyn_Enumdecl*_tmp72B;struct Cyc_Absyn_Aggrdecl*_tmp72A;struct Cyc_Absyn_Fndecl*_tmp729;struct Cyc_Absyn_Vardecl*_tmp728;switch(*((int*)_tmp722)){case 0U: _LL1: _tmp728=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp722)->f1;_LL2:
# 4580
({void*_tmpB86=Cyc_Toc_typ_to_c(_tmp728->type);_tmp728->type=_tmpB86;});
if(_tmp728->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp728->initializer));
goto _LL0;case 1U: _LL3: _tmp729=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp722)->f1;_LL4:
# 4584
({void*_tmpB87=Cyc_Toc_typ_to_c(_tmp729->ret_type);_tmp729->ret_type=_tmpB87;});
{struct Cyc_List_List*_tmp723=_tmp729->args;for(0;_tmp723 != 0;_tmp723=_tmp723->tl){
({void*_tmpB88=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp723->hd)).f3);(*((struct _tuple9*)_tmp723->hd)).f3=_tmpB88;});}}
# 4588
goto _LL0;case 5U: _LL5: _tmp72A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp722)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp72A,1);goto _LL0;case 7U: _LL7: _tmp72B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp722)->f1;_LL8:
# 4591
 if(_tmp72B->fields != 0){
struct Cyc_List_List*_tmp724=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp72B->fields))->v;for(0;_tmp724 != 0;_tmp724=_tmp724->tl){
struct Cyc_Absyn_Enumfield*_tmp725=(struct Cyc_Absyn_Enumfield*)_tmp724->hd;
if(_tmp725->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp725->tag));}}
# 4596
goto _LL0;case 8U: _LL9: _tmp72C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp722)->f1;_LLA:
({void*_tmpB89=Cyc_Toc_typ_to_c((void*)_check_null(_tmp72C->defn));_tmp72C->defn=_tmpB89;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4606
({void*_tmp726=0U;({unsigned int _tmpB8B=d->loc;struct _dyneither_ptr _tmpB8A=({const char*_tmp727="Cyclone declaration within C code";_tag_dyneither(_tmp727,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpB8B,_tmpB8A,_tag_dyneither(_tmp726,sizeof(void*),0U));});});
goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 4610
 goto _LL0;}_LL0:;}
# 4614
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp72D=s->r;void*_tmp72E=_tmp72D;struct Cyc_Absyn_Stmt*_tmp748;struct Cyc_Absyn_Stmt*_tmp747;struct Cyc_Absyn_Exp*_tmp746;struct Cyc_Absyn_Decl*_tmp745;struct Cyc_Absyn_Stmt*_tmp744;struct Cyc_Absyn_Exp*_tmp743;struct Cyc_List_List*_tmp742;void*_tmp741;struct Cyc_Absyn_Exp*_tmp740;struct Cyc_Absyn_Exp*_tmp73F;struct Cyc_Absyn_Exp*_tmp73E;struct Cyc_Absyn_Stmt*_tmp73D;struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Stmt*_tmp73B;struct Cyc_Absyn_Exp*_tmp73A;struct Cyc_Absyn_Stmt*_tmp739;struct Cyc_Absyn_Stmt*_tmp738;struct Cyc_Absyn_Exp*_tmp737;struct Cyc_Absyn_Stmt*_tmp736;struct Cyc_Absyn_Stmt*_tmp735;struct Cyc_Absyn_Exp*_tmp734;switch(*((int*)_tmp72E)){case 1U: _LL1: _tmp734=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp72E)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp734);goto _LL0;case 2U: _LL3: _tmp736=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp72E)->f1;_tmp735=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp72E)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp736);Cyc_Toc_stmttypes_to_c(_tmp735);goto _LL0;case 3U: _LL5: _tmp737=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp72E)->f1;_LL6:
 if(_tmp737 != 0)Cyc_Toc_exptypes_to_c(_tmp737);goto _LL0;case 4U: _LL7: _tmp73A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72E)->f1;_tmp739=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72E)->f2;_tmp738=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72E)->f3;_LL8:
# 4620
 Cyc_Toc_exptypes_to_c(_tmp73A);Cyc_Toc_stmttypes_to_c(_tmp739);Cyc_Toc_stmttypes_to_c(_tmp738);goto _LL0;case 5U: _LL9: _tmp73C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp72E)->f1).f1;_tmp73B=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp72E)->f2;_LLA:
# 4622
 Cyc_Toc_exptypes_to_c(_tmp73C);Cyc_Toc_stmttypes_to_c(_tmp73B);goto _LL0;case 9U: _LLB: _tmp740=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72E)->f1;_tmp73F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72E)->f2).f1;_tmp73E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72E)->f3).f1;_tmp73D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72E)->f4;_LLC:
# 4624
 Cyc_Toc_exptypes_to_c(_tmp740);Cyc_Toc_exptypes_to_c(_tmp73F);Cyc_Toc_exptypes_to_c(_tmp73E);
Cyc_Toc_stmttypes_to_c(_tmp73D);goto _LL0;case 10U: _LLD: _tmp743=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72E)->f1;_tmp742=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72E)->f2;_tmp741=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72E)->f3;_LLE:
# 4627
 Cyc_Toc_exptypes_to_c(_tmp743);
for(0;_tmp742 != 0;_tmp742=_tmp742->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp742->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp745=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp72E)->f1;_tmp744=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp72E)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp745);Cyc_Toc_stmttypes_to_c(_tmp744);goto _LL0;case 14U: _LL11: _tmp747=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp72E)->f1;_tmp746=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp72E)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp747);Cyc_Toc_exptypes_to_c(_tmp746);goto _LL0;case 13U: _LL13: _tmp748=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp72E)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp748);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4637
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4640
({void*_tmpB8C=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F->tag=0U;_tmp72F;});s->r=_tmpB8C;});
goto _LL0;default: _LL1F: _LL20:
# 4643
({void*_tmp730=0U;({unsigned int _tmpB90=s->loc;struct _dyneither_ptr _tmpB8F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp733=({struct Cyc_String_pa_PrintArg_struct _tmp7AC;_tmp7AC.tag=0U,({struct _dyneither_ptr _tmpB8D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp7AC.f1=_tmpB8D;});_tmp7AC;});void*_tmp731[1U];_tmp731[0]=& _tmp733;({struct _dyneither_ptr _tmpB8E=({const char*_tmp732="Cyclone statement in C code: %s";_tag_dyneither(_tmp732,sizeof(char),32U);});Cyc_aprintf(_tmpB8E,_tag_dyneither(_tmp731,sizeof(void*),1U));});});Cyc_Tcutil_terr(_tmpB90,_tmpB8F,_tag_dyneither(_tmp730,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 4651
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp749=0U;({struct _dyneither_ptr _tmpB91=({const char*_tmp74A="decls_to_c: not at toplevel!";_tag_dyneither(_tmp74A,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB91,_tag_dyneither(_tmp749,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp74B=d->r;void*_tmp74C=_tmp74B;struct Cyc_List_List*_tmp764;struct Cyc_List_List*_tmp763;struct Cyc_List_List*_tmp762;struct Cyc_List_List*_tmp761;struct Cyc_Absyn_Typedefdecl*_tmp760;struct Cyc_Absyn_Enumdecl*_tmp75F;struct Cyc_Absyn_Datatypedecl*_tmp75E;struct Cyc_Absyn_Aggrdecl*_tmp75D;struct Cyc_Absyn_Fndecl*_tmp75C;struct Cyc_Absyn_Vardecl*_tmp75B;switch(*((int*)_tmp74C)){case 0U: _LL1: _tmp75B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp74C)->f1;_LL2: {
# 4659
struct _tuple1*_tmp74D=_tmp75B->name;
# 4661
if(_tmp75B->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpB93=({struct _tuple1*_tmp74E=_cycalloc(sizeof(*_tmp74E));({union Cyc_Absyn_Nmspace _tmpB92=Cyc_Absyn_Abs_n(0,1);_tmp74E->f1=_tmpB92;}),_tmp74E->f2=(*_tmp74D).f2;_tmp74E;});_tmp74D=_tmpB93;});
if(_tmp75B->initializer != 0){
if(_tmp75B->sc == Cyc_Absyn_ExternC)_tmp75B->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp75B->initializer));else{
# 4668
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp75B->initializer));}}
# 4670
_tmp75B->name=_tmp74D;
({enum Cyc_Absyn_Scope _tmpB94=Cyc_Toc_scope_to_c(_tmp75B->sc);_tmp75B->sc=_tmpB94;});
({void*_tmpB95=Cyc_Toc_typ_to_c(_tmp75B->type);_tmp75B->type=_tmpB95;});
({struct Cyc_List_List*_tmpB96=({struct Cyc_List_List*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->hd=d,_tmp74F->tl=Cyc_Toc_result_decls;_tmp74F;});Cyc_Toc_result_decls=_tmpB96;});
goto _LL0;}case 1U: _LL3: _tmp75C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp74C)->f1;_LL4: {
# 4676
struct _tuple1*_tmp750=_tmp75C->name;
# 4678
if(_tmp75C->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB98=({struct _tuple1*_tmp751=_cycalloc(sizeof(*_tmp751));({union Cyc_Absyn_Nmspace _tmpB97=Cyc_Absyn_Abs_n(0,1);_tmp751->f1=_tmpB97;}),_tmp751->f2=(*_tmp750).f2;_tmp751;});_tmp750=_tmpB98;});
_tmp75C->sc=Cyc_Absyn_Public;}
# 4682
_tmp75C->name=_tmp750;
Cyc_Toc_fndecl_to_c(nv,_tmp75C,cinclude);
({struct Cyc_List_List*_tmpB99=({struct Cyc_List_List*_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752->hd=d,_tmp752->tl=Cyc_Toc_result_decls;_tmp752;});Cyc_Toc_result_decls=_tmpB99;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp753=0U;({struct _dyneither_ptr _tmpB9A=({const char*_tmp754="letdecl at toplevel";_tag_dyneither(_tmp754,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB9A,_tag_dyneither(_tmp753,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp755=0U;({struct _dyneither_ptr _tmpB9B=({const char*_tmp756="region decl at toplevel";_tag_dyneither(_tmp756,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB9B,_tag_dyneither(_tmp755,sizeof(void*),0U));});});case 5U: _LLB: _tmp75D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp74C)->f1;_LLC:
# 4690
 Cyc_Toc_aggrdecl_to_c(_tmp75D,1);
goto _LL0;case 6U: _LLD: _tmp75E=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp74C)->f1;_LLE:
# 4693
 if(_tmp75E->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp75E);else{
# 4696
Cyc_Toc_datatypedecl_to_c(_tmp75E);}
goto _LL0;case 7U: _LLF: _tmp75F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp74C)->f1;_LL10:
# 4699
 Cyc_Toc_enumdecl_to_c(_tmp75F);
({struct Cyc_List_List*_tmpB9C=({struct Cyc_List_List*_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757->hd=d,_tmp757->tl=Cyc_Toc_result_decls;_tmp757;});Cyc_Toc_result_decls=_tmpB9C;});
goto _LL0;case 8U: _LL11: _tmp760=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp74C)->f1;_LL12:
# 4703
 _tmp760->tvs=0;
if(_tmp760->defn != 0)
({void*_tmpB9D=Cyc_Toc_typ_to_c((void*)_check_null(_tmp760->defn));_tmp760->defn=_tmpB9D;});else{
# 4707
enum Cyc_Absyn_KindQual _tmp758=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp760->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp759=_tmp758;if(_tmp759 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpB9E=Cyc_Absyn_void_star_typ();_tmp760->defn=_tmpB9E;});goto _LL1F;}else{_LL22: _LL23:
 _tmp760->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL1F;}_LL1F:;}
# 4715
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpB9F=({struct Cyc_List_List*_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A->hd=d,_tmp75A->tl=Cyc_Toc_result_decls;_tmp75A;});Cyc_Toc_result_decls=_tmpB9F;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp761=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp74C)->f2;_LL18:
 _tmp762=_tmp761;goto _LL1A;case 10U: _LL19: _tmp762=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp74C)->f2;_LL1A:
 _tmp763=_tmp762;goto _LL1C;case 11U: _LL1B: _tmp763=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp74C)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpBA0=Cyc_Toc_decls_to_c(r,nv,_tmp763,top,cinclude);nv=_tmpBA0;});goto _LL0;default: _LL1D: _tmp764=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp74C)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpBA1=Cyc_Toc_decls_to_c(r,nv,_tmp764,top,1);nv=_tmpBA1;});goto _LL0;}_LL0:;};}
# 4726
return nv;}
# 4730
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp765=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp766=_tmp765;struct Cyc_Core_DynamicRegion*_tmp76B;_LL1: _tmp76B=_tmp766.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp76B->h;
({struct Cyc_Toc_TocState*_tmpBA2=Cyc_Toc_empty_toc_state(h);ts=_tmpBA2;});;}
({struct Cyc_Toc_TocStateWrap*_tmpBA3=({struct Cyc_Toc_TocStateWrap*_tmp768=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp768));_tmp768->dyn=_tmp76B,_tmp768->state=ts;_tmp768;});Cyc_Toc_toc_state=_tmpBA3;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpBA4=_tag_dyneither(({unsigned int _tmp76A=35;struct _dyneither_ptr**_tmp769=_cycalloc(_check_times(_tmp76A,sizeof(struct _dyneither_ptr*)));_tmp769[0]=& Cyc_Toc__throw_str,_tmp769[1]=& Cyc_Toc_setjmp_str,_tmp769[2]=& Cyc_Toc__push_handler_str,_tmp769[3]=& Cyc_Toc__pop_handler_str,_tmp769[4]=& Cyc_Toc__exn_thrown_str,_tmp769[5]=& Cyc_Toc__npop_handler_str,_tmp769[6]=& Cyc_Toc__check_null_str,_tmp769[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp769[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp769[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp769[10]=& Cyc_Toc__dyneither_ptr_str,_tmp769[11]=& Cyc_Toc__tag_dyneither_str,_tmp769[12]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp769[13]=& Cyc_Toc__get_dyneither_size_str,_tmp769[14]=& Cyc_Toc__get_zero_arr_size_str,_tmp769[15]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp769[16]=& Cyc_Toc__zero_arr_plus_str,_tmp769[17]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp769[18]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp769[19]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp769[20]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp769[21]=& Cyc_Toc__cycalloc_str,_tmp769[22]=& Cyc_Toc__cyccalloc_str,_tmp769[23]=& Cyc_Toc__cycalloc_atomic_str,_tmp769[24]=& Cyc_Toc__cyccalloc_atomic_str,_tmp769[25]=& Cyc_Toc__region_malloc_str,_tmp769[26]=& Cyc_Toc__region_calloc_str,_tmp769[27]=& Cyc_Toc__check_times_str,_tmp769[28]=& Cyc_Toc__new_region_str,_tmp769[29]=& Cyc_Toc__push_region_str,_tmp769[30]=& Cyc_Toc__pop_region_str,_tmp769[31]=& Cyc_Toc__throw_arraybounds_str,_tmp769[32]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp769[33]=& Cyc_Toc__throw_match_str,_tmp769[34]=& Cyc_Toc__fast_region_malloc_str;_tmp769;}),sizeof(struct _dyneither_ptr*),35U);Cyc_Toc_globals=_tmpBA4;});};}
# 4781
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp76C=ts;struct Cyc_Toc_TocStateWrap*_tmp76D=Cyc_Toc_toc_state;ts=_tmp76D;Cyc_Toc_toc_state=_tmp76C;});{
struct Cyc_Toc_TocStateWrap _tmp76E=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp76F=_tmp76E;struct Cyc_Core_DynamicRegion*_tmp775;struct Cyc_Toc_TocState*_tmp774;_LL1: _tmp775=_tmp76F.dyn;_tmp774=_tmp76F.state;_LL2:;
# 4786
{struct _RegionHandle*h=& _tmp775->h;
{struct Cyc_Toc_TocState _tmp771=*_tmp774;struct Cyc_Toc_TocState _tmp772=_tmp771;struct Cyc_Xarray_Xarray*_tmp773;_LL4: _tmp773=_tmp772.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp773);}
# 4787
;}
# 4790
Cyc_Core_free_rckey(_tmp775);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4793
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4799
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4801
({struct Cyc_Hashtable_Table**_tmpBA5=({struct Cyc_Hashtable_Table**_tmp776=_cycalloc(sizeof(*_tmp776));*_tmp776=pop_tables;_tmp776;});Cyc_Toc_gpop_tables=_tmpBA5;});
Cyc_Toc_init();
{struct _RegionHandle _tmp777=_new_region("start");struct _RegionHandle*start=& _tmp777;_push_region(start);
({struct _RegionHandle*_tmpBA7=start;struct Cyc_Toc_Env*_tmpBA6=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpBA7,_tmpBA6,ds,1,0);});;_pop_region(start);}
# 4810
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}

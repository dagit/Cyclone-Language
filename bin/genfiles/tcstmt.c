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
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 113
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 907
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 923
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 935
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 939
extern void*Cyc_Absyn_false_type;
# 941
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_rgn_handle_type(void*);
# 958
void*Cyc_Absyn_exn_type();
# 983
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 998
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 1003
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1103
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1108
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1112
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1164
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 68 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 92
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 96
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 98
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 102
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 120
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 123
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 126
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned int);
# 133
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 147
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
int Cyc_Tcutil_is_void_type(void*);
# 43
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 115
void*Cyc_Tcutil_compress(void*t);
# 117
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 151
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 154
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 181
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 195
void Cyc_Tcutil_explain_failure();
# 197
int Cyc_Tcutil_unify(void*,void*);
# 202
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 226
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 230
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 265 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 273
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 310
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 315
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 332
int Cyc_Tcutil_new_tvar_id();
# 345
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 348
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 363
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 367
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 377
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 53 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 55
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 107
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 109
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p,void**);
# 111
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 48 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp5;struct Cyc_Absyn_Exp*_tmp4;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 5U)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
({void*_tmp15E=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=5U,_tmp2->f1=_tmp4,_tmp2->f2=Cyc_Absyn_PreInc;_tmp2;});e->r=_tmp15E;});goto _LL0;case Cyc_Absyn_PostDec: _LL3: _tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4:
({void*_tmp15F=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=5U,_tmp3->f1=_tmp5,_tmp3->f2=Cyc_Absyn_PreDec;_tmp3;});e->r=_tmp15F;});goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 61
static int Cyc_Tcstmt_noassign_exp(struct Cyc_Absyn_Exp*e){
void*_tmp6=e->r;void*_tmp7=_tmp6;struct Cyc_List_List*_tmp2F;struct Cyc_List_List*_tmp2E;struct Cyc_List_List*_tmp2D;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2A;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpD;struct Cyc_Absyn_Exp*_tmpC;struct Cyc_Absyn_Exp*_tmpB;switch(*((int*)_tmp7)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 70
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmpD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmpC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_tmpB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7)->f3;_LL1E:
# 80
 if(!Cyc_Tcstmt_noassign_exp(_tmpD))return 0;
_tmpF=_tmpC;_tmpE=_tmpB;goto _LL20;case 27U: _LL1F: _tmpF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_tmpE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp7)->f3;_LL20:
 _tmp11=_tmpF;_tmp10=_tmpE;goto _LL22;case 7U: _LL21: _tmp11=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp10=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL22:
 _tmp13=_tmp11;_tmp12=_tmp10;goto _LL24;case 8U: _LL23: _tmp13=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp12=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL24:
 _tmp15=_tmp13;_tmp14=_tmp12;goto _LL26;case 23U: _LL25: _tmp15=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp14=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL26:
 _tmp17=_tmp15;_tmp16=_tmp14;goto _LL28;case 9U: _LL27: _tmp17=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp16=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL28:
 return Cyc_Tcstmt_noassign_exp(_tmp17) && Cyc_Tcstmt_noassign_exp(_tmp16);case 41U: _LL29: _tmp18=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL2A:
# 88
 _tmp19=_tmp18;goto _LL2C;case 38U: _LL2B: _tmp19=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL2C:
 _tmp1A=_tmp19;goto _LL2E;case 11U: _LL2D: _tmp1A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL2E:
 _tmp1B=_tmp1A;goto _LL30;case 12U: _LL2F: _tmp1B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL30:
 _tmp1C=_tmp1B;goto _LL32;case 13U: _LL31: _tmp1C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL32:
 _tmp1D=_tmp1C;goto _LL34;case 14U: _LL33: _tmp1D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL34:
 _tmp1E=_tmp1D;goto _LL36;case 18U: _LL35: _tmp1E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL36:
 _tmp1F=_tmp1E;goto _LL38;case 20U: _LL37: _tmp1F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL38:
 _tmp20=_tmp1F;goto _LL3A;case 21U: _LL39: _tmp20=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL3A:
 _tmp21=_tmp20;goto _LL3C;case 22U: _LL3B: _tmp21=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL3C:
 _tmp22=_tmp21;goto _LL3E;case 28U: _LL3D: _tmp22=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL3E:
 _tmp23=_tmp22;goto _LL40;case 15U: _LL3F: _tmp23=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL40:
 return Cyc_Tcstmt_noassign_exp(_tmp23);case 34U: _LL41: _tmp25=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp7)->f1).rgn;_tmp24=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp7)->f1).num_elts;_LL42:
# 102
 _tmp27=_tmp25;_tmp26=_tmp24;goto _LL44;case 16U: _LL43: _tmp27=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp26=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL44:
# 104
 if(!Cyc_Tcstmt_noassign_exp(_tmp26))return 0;
if(_tmp27 != 0)return Cyc_Tcstmt_noassign_exp(_tmp27);else{
return 1;}case 3U: _LL45: _tmp28=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL46:
# 108
 _tmp29=_tmp28;goto _LL48;case 31U: _LL47: _tmp29=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL48:
 _tmp2A=_tmp29;goto _LL4A;case 24U: _LL49: _tmp2A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL4A:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcstmt_noassign_exp,_tmp2A);case 36U: _LL4B: _tmp2B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL4C:
# 112
 _tmp2C=_tmp2B;goto _LL4E;case 29U: _LL4D: _tmp2C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp7)->f3;_LL4E:
 _tmp2D=_tmp2C;goto _LL50;case 30U: _LL4F: _tmp2D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL50:
 _tmp2E=_tmp2D;goto _LL52;case 26U: _LL51: _tmp2E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL52:
 _tmp2F=_tmp2E;goto _LL54;default: _LL53: _tmp2F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL54:
# 117
 for(0;_tmp2F != 0;_tmp2F=_tmp2F->tl){
struct _tuple15*_tmp8=(struct _tuple15*)_tmp2F->hd;struct _tuple15*_tmp9=_tmp8;struct Cyc_Absyn_Exp*_tmpA;_LL56: _tmpA=_tmp9->f2;_LL57:;
if(!Cyc_Tcstmt_noassign_exp(_tmpA))return 0;}
# 121
return 1;}_LL0:;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 126
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 128
struct Cyc_Tcpat_TcPatResult _tmp30=pat_res;struct _tuple1*_tmp3C;struct Cyc_List_List*_tmp3B;_LL1: _tmp3C=_tmp30.tvars_and_bounds_opt;_tmp3B=_tmp30.patvars;_LL2:;{
struct Cyc_List_List*_tmp31=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple17*))Cyc_Core_fst,_tmp3B);
struct Cyc_List_List*_tmp32=_tmp3C == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*_tmp3C).f1);
struct Cyc_List_List*_tmp33=_tmp3C == 0?0:(*_tmp3C).f2;
struct Cyc_List_List*_tmp34=_tmp3C == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple16*))Cyc_Core_snd,(*_tmp3C).f1));
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,_tmp32);
for(0;_tmp34 != 0;_tmp34=_tmp34->tl){
({struct Cyc_Tcenv_Tenv*_tmp161=({struct Cyc_Tcenv_Tenv*_tmp160=te2;Cyc_Tcenv_add_region(_tmp160,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=2U,_tmp35->f1=(struct Cyc_Absyn_Tvar*)_tmp34->hd;_tmp35;}),0);});te2=_tmp161;});}
({struct Cyc_Tcenv_Tenv*_tmp162=Cyc_Tcenv_new_outlives_constraints(te2,_tmp33,loc);te2=_tmp162;});
if(new_block)
({struct Cyc_Tcenv_Tenv*_tmp163=Cyc_Tcenv_new_block(loc,te2);te2=_tmp163;});{
void*_tmp36=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp37=_tmp31;for(0;_tmp37 != 0;_tmp37=_tmp37->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp37->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp37->hd)))->rgn=_tmp36;}}
# 144
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp165=te2;struct Cyc_Absyn_Exp*_tmp164=where_opt;Cyc_Tcexp_tcTest(_tmp165,_tmp164,({const char*_tmp38="switch clause guard";_tag_dyneither(_tmp38,sizeof(char),20U);}));});
if(!Cyc_Tcstmt_noassign_exp(where_opt))
({void*_tmp39=0U;({unsigned int _tmp167=where_opt->loc;struct _dyneither_ptr _tmp166=({const char*_tmp3A="cannot show &&-clause in pattern has no effects";_tag_dyneither(_tmp3A,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp167,_tmp166,_tag_dyneither(_tmp39,sizeof(void*),0U));});});}
# 149
Cyc_Tcstmt_tcStmt(te2,s,0);};};}
# 152
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp3D=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _tuple2*res=({struct _tuple2*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp16A=({struct _dyneither_ptr*_tmp41=_cycalloc(sizeof(*_tmp41));({struct _dyneither_ptr _tmp169=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp40=({struct Cyc_Int_pa_PrintArg_struct _tmp143;_tmp143.tag=1U,_tmp143.f1=(unsigned int)_tmp3D;_tmp143;});void*_tmp3E[1U];_tmp3E[0]=& _tmp40;({struct _dyneither_ptr _tmp168=({const char*_tmp3F="_stmttmp%X";_tag_dyneither(_tmp3F,sizeof(char),11U);});Cyc_aprintf(_tmp168,_tag_dyneither(_tmp3E,sizeof(void*),1U));});});*_tmp41=_tmp169;});_tmp41;});_tmp42->f2=_tmp16A;});_tmp42;});
return res;}
# 166 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 168
void*_tmp43=s0->r;void*_tmp44=_tmp43;struct Cyc_Absyn_Decl*_tmp142;struct Cyc_Absyn_Stmt*_tmp141;struct Cyc_Absyn_Stmt*_tmp140;struct Cyc_List_List**_tmp13F;void**_tmp13E;struct Cyc_Absyn_Exp**_tmp13D;struct Cyc_List_List*_tmp13C;void**_tmp13B;struct _dyneither_ptr*_tmp13A;struct Cyc_Absyn_Stmt*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Switch_clause***_tmp137;struct Cyc_Absyn_Stmt*_tmp136;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Stmt*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Stmt*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Stmt*_tmp12F;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Stmt*_tmp12C;struct Cyc_Absyn_Stmt*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Stmt*_tmp129;struct Cyc_Absyn_Stmt*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Stmt*_tmp126;struct Cyc_Absyn_Stmt*_tmp125;struct Cyc_Absyn_Exp*_tmp124;switch(*((int*)_tmp44)){case 0U: _LL1: _LL2:
# 170
 return;case 1U: _LL3: _tmp124=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_LL4:
# 173
 Cyc_Tcexp_tcExp(te,0,_tmp124);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(_tmp124);
return;case 2U: _LL5: _tmp126=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp125=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL6:
# 179
 Cyc_Tcstmt_tcStmt(te,_tmp126,1);
Cyc_Tcstmt_tcStmt(te,_tmp125,1);
return;case 3U: _LL7: _tmp127=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_LL8: {
# 184
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp127 == 0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t) || Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_String_pa_PrintArg_struct _tmp47=({struct Cyc_String_pa_PrintArg_struct _tmp144;_tmp144.tag=0U,({struct _dyneither_ptr _tmp16B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp144.f1=_tmp16B;});_tmp144;});void*_tmp45[1U];_tmp45[0]=& _tmp47;({unsigned int _tmp16D=s0->loc;struct _dyneither_ptr _tmp16C=({const char*_tmp46="should return a value of type %s";_tag_dyneither(_tmp46,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp16D,_tmp16C,_tag_dyneither(_tmp45,sizeof(void*),1U));});});else{
# 190
({struct Cyc_String_pa_PrintArg_struct _tmp4A=({struct Cyc_String_pa_PrintArg_struct _tmp145;_tmp145.tag=0U,({struct _dyneither_ptr _tmp16E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp145.f1=_tmp16E;});_tmp145;});void*_tmp48[1U];_tmp48[0]=& _tmp4A;({unsigned int _tmp170=s0->loc;struct _dyneither_ptr _tmp16F=({const char*_tmp49="must return a value of type %s";_tag_dyneither(_tmp49,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp170,_tmp16F,_tag_dyneither(_tmp48,sizeof(void*),1U));});});}}}else{
# 194
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp127;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp4D=({struct Cyc_String_pa_PrintArg_struct _tmp147;_tmp147.tag=0U,({
struct _dyneither_ptr _tmp171=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp147.f1=_tmp171;});_tmp147;});struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp146;_tmp146.tag=0U,({struct _dyneither_ptr _tmp172=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp146.f1=_tmp172;});_tmp146;});void*_tmp4B[2U];_tmp4B[0]=& _tmp4D,_tmp4B[1]=& _tmp4E;({unsigned int _tmp174=s0->loc;struct _dyneither_ptr _tmp173=({const char*_tmp4C="returns value of type %s but requires %s";_tag_dyneither(_tmp4C,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp174,_tmp173,_tag_dyneither(_tmp4B,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 202
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
({void*_tmp4F=0U;({unsigned int _tmp176=e->loc;struct _dyneither_ptr _tmp175=({const char*_tmp50="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp50,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp176,_tmp175,_tag_dyneither(_tmp4F,sizeof(void*),0U));});});}
# 205
return;}case 4U: _LL9: _tmp12A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp129=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_tmp128=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp44)->f3;_LLA:
# 208
({struct Cyc_Tcenv_Tenv*_tmp178=te;struct Cyc_Absyn_Exp*_tmp177=_tmp12A;Cyc_Tcexp_tcTest(_tmp178,_tmp177,({const char*_tmp51="if statement";_tag_dyneither(_tmp51,sizeof(char),13U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp129,1);
Cyc_Tcstmt_tcStmt(te,_tmp128,1);
return;case 5U: _LLB: _tmp12D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp44)->f1).f1;_tmp12C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp44)->f1).f2;_tmp12B=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LLC:
# 214
({struct Cyc_Tcenv_Tenv*_tmp17A=te;struct Cyc_Absyn_Exp*_tmp179=_tmp12D;Cyc_Tcexp_tcTest(_tmp17A,_tmp179,({const char*_tmp52="while loop";_tag_dyneither(_tmp52,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp12B,1);
return;case 9U: _LLD: _tmp133=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp132=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f1;_tmp131=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f2;_tmp130=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f3).f1;_tmp12F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f3).f2;_tmp12E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f4;_LLE:
# 219
 Cyc_Tcexp_tcExp(te,0,_tmp133);
({struct Cyc_Tcenv_Tenv*_tmp17C=te;struct Cyc_Absyn_Exp*_tmp17B=_tmp132;Cyc_Tcexp_tcTest(_tmp17C,_tmp17B,({const char*_tmp53="for loop";_tag_dyneither(_tmp53,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp12E,1);
Cyc_Tcexp_tcExp(te,0,_tmp130);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp130);
return;case 14U: _LLF: _tmp136=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp135=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f1;_tmp134=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f2;_LL10:
# 227
 Cyc_Tcstmt_tcStmt(te,_tmp136,1);
({struct Cyc_Tcenv_Tenv*_tmp17E=te;struct Cyc_Absyn_Exp*_tmp17D=_tmp135;Cyc_Tcexp_tcTest(_tmp17E,_tmp17D,({const char*_tmp54="do loop";_tag_dyneither(_tmp54,sizeof(char),8U);}));});
return;case 6U: _LL11: _LL12:
# 231
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp138=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp137=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL18: {
# 236
const struct _tuple12*_tmp55=Cyc_Tcenv_process_fallthru(te,s0,_tmp137);
if(_tmp55 == (const struct _tuple12*)0){
({void*_tmp56=0U;({unsigned int _tmp180=s0->loc;struct _dyneither_ptr _tmp17F=({const char*_tmp57="fallthru not in a non-last case";_tag_dyneither(_tmp57,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp180,_tmp17F,_tag_dyneither(_tmp56,sizeof(void*),0U));});});
return;}{
# 241
struct Cyc_List_List*_tmp58=(*_tmp55).f2;
struct Cyc_List_List*_tmp59=(*_tmp55).f3;
struct Cyc_List_List*instantiation=({
struct Cyc_List_List*_tmp181=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp181,_tmp58);});
struct Cyc_List_List*_tmp5A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,_tmp59);
for(0;_tmp5A != 0  && _tmp138 != 0;(_tmp5A=_tmp5A->tl,_tmp138=_tmp138->tl)){
# 248
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp138->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp138->hd,(void*)_tmp5A->hd,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0U,({
# 253
struct _dyneither_ptr _tmp182=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp138->hd)->topt)));_tmp149.f1=_tmp182;});_tmp149;});struct Cyc_String_pa_PrintArg_struct _tmp5E=({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0U,({struct _dyneither_ptr _tmp183=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmp5A->hd));_tmp148.f1=_tmp183;});_tmp148;});void*_tmp5B[2U];_tmp5B[0]=& _tmp5D,_tmp5B[1]=& _tmp5E;({unsigned int _tmp185=s0->loc;struct _dyneither_ptr _tmp184=({const char*_tmp5C="fallthru argument has type %s but pattern variable has type %s";_tag_dyneither(_tmp5C,sizeof(char),63U);});Cyc_Tcutil_terr(_tmp185,_tmp184,_tag_dyneither(_tmp5B,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 257
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_tmp5A->hd) && !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp138->hd))
({void*_tmp5F=0U;({unsigned int _tmp187=((struct Cyc_Absyn_Exp*)_tmp138->hd)->loc;struct _dyneither_ptr _tmp186=({const char*_tmp60="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp60,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp187,_tmp186,_tag_dyneither(_tmp5F,sizeof(void*),0U));});});}
# 260
if(_tmp138 != 0)
({void*_tmp61=0U;({unsigned int _tmp189=s0->loc;struct _dyneither_ptr _tmp188=({const char*_tmp62="too many arguments in explicit fallthru";_tag_dyneither(_tmp62,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp189,_tmp188,_tag_dyneither(_tmp61,sizeof(void*),0U));});});
if(_tmp5A != 0)
({void*_tmp63=0U;({unsigned int _tmp18B=s0->loc;struct _dyneither_ptr _tmp18A=({const char*_tmp64="too few arguments in explicit fallthru";_tag_dyneither(_tmp64,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp18B,_tmp18A,_tag_dyneither(_tmp63,sizeof(void*),0U));});});
return;};}case 13U: _LL19: _tmp13A=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp139=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL1A:
# 270
({struct Cyc_Tcenv_Tenv*_tmp193=({unsigned int _tmp192=s0->loc;struct Cyc_Tcenv_Tenv*_tmp191=te;Cyc_Tcenv_new_named_block(_tmp192,_tmp191,({struct Cyc_Absyn_Tvar*_tmp69=_cycalloc(sizeof(*_tmp69));
({struct _dyneither_ptr*_tmp190=({struct _dyneither_ptr*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _dyneither_ptr _tmp18F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0U,_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp13A);_tmp14A;});void*_tmp65[1U];_tmp65[0]=& _tmp67;({struct _dyneither_ptr _tmp18E=({const char*_tmp66="`%s";_tag_dyneither(_tmp66,sizeof(char),4U);});Cyc_aprintf(_tmp18E,_tag_dyneither(_tmp65,sizeof(void*),1U));});});*_tmp68=_tmp18F;});_tmp68;});_tmp69->name=_tmp190;}),({
int _tmp18D=Cyc_Tcutil_new_tvar_id();_tmp69->identity=_tmp18D;}),({
void*_tmp18C=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp69->kind=_tmp18C;});_tmp69;}));});
# 270
Cyc_Tcstmt_tcStmt(_tmp193,_tmp139,0);});
# 274
return;case 10U: _LL1B: _tmp13D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp13C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_tmp13B=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp44)->f3;_LL1C: {
# 279
struct Cyc_Absyn_Exp*_tmp6A=*_tmp13D;
if(!Cyc_Tcutil_is_var_exp(_tmp6A)){
struct Cyc_Absyn_Vardecl*_tmp6B=({struct _tuple2*_tmp194=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp194,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmp6C=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmp6D=({struct Cyc_Absyn_Pat*_tmp198=({void*_tmp196=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=1U,_tmp70->f1=_tmp6B,({struct Cyc_Absyn_Pat*_tmp195=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp6A->loc);_tmp70->f2=_tmp195;});_tmp70;});Cyc_Absyn_new_pat(_tmp196,_tmp6A->loc);});struct Cyc_Absyn_Exp*_tmp197=_tmp6A;Cyc_Absyn_let_decl(_tmp198,_tmp197,s0->loc);});
# 286
struct Cyc_Absyn_Stmt*_tmp6E=Cyc_Absyn_decl_stmt(_tmp6D,_tmp6C,s0->loc);
s0->r=_tmp6E->r;
({struct Cyc_Absyn_Exp*_tmp19A=({void*_tmp199=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=4U,_tmp6F->f1=_tmp6B;_tmp6F;});Cyc_Absyn_varb_exp(_tmp199,_tmp6A->loc);});*_tmp13D=_tmp19A;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 293
void*_tmp71=Cyc_Tcexp_tcExp(te,0,_tmp6A);
# 295
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp71) && !Cyc_Tcutil_is_noalias_path(_tmp6A))
({void*_tmp72=0U;({unsigned int _tmp19C=_tmp6A->loc;struct _dyneither_ptr _tmp19B=({const char*_tmp73="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp73,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp19C,_tmp19B,_tag_dyneither(_tmp72,sizeof(void*),0U));});});{
# 300
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp74=_tmp13C;for(0;_tmp74 != 0;_tmp74=_tmp74->tl){
if(first_case){
first_case=0;
({struct Cyc_Tcpat_TcPatResult _tmp19D=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->pattern,& _tmp71,_tmp6A);this_pattern=_tmp19D;});}else{
# 308
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 310
if(_tmp74->tl != 0){
({struct Cyc_Tcpat_TcPatResult*_tmp19F=({struct Cyc_Tcpat_TcPatResult*_tmp75=_cycalloc(sizeof(*_tmp75));({struct Cyc_Tcpat_TcPatResult _tmp19E=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp74->tl))->hd)->pattern,& _tmp71,_tmp6A);*_tmp75=_tmp19E;});_tmp75;});next_pattern=_tmp19F;});
if(next_pattern->tvars_and_bounds_opt != 0  && (*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 314
({struct Cyc_Tcenv_Tenv*_tmp1A0=Cyc_Tcenv_clear_fallthru(te);te=_tmp1A0;});else{
# 316
struct Cyc_List_List*_tmp76=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp77=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
({struct Cyc_Tcenv_Tenv*_tmp1A1=Cyc_Tcenv_set_fallthru(te,_tmp77,_tmp76,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp74->tl))->hd);te=_tmp1A1;});}}else{
# 322
({struct Cyc_Tcenv_Tenv*_tmp1A2=Cyc_Tcenv_clear_fallthru(te);te=_tmp1A2;});}{
# 324
struct Cyc_Absyn_Pat*_tmp78=((struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp78->topt),_tmp71)){
({struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0U,({
struct _dyneither_ptr _tmp1A3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp71));_tmp14C.f1=_tmp1A3;});_tmp14C;});struct Cyc_String_pa_PrintArg_struct _tmp7C=({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0U,({struct _dyneither_ptr _tmp1A4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp78->topt)));_tmp14B.f1=_tmp1A4;});_tmp14B;});void*_tmp79[2U];_tmp79[0]=& _tmp7B,_tmp79[1]=& _tmp7C;({unsigned int _tmp1A6=((struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->loc;struct _dyneither_ptr _tmp1A5=({const char*_tmp7A="switch on type %s, but case expects type %s";_tag_dyneither(_tmp7A,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1A6,_tmp1A5,_tag_dyneither(_tmp79,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}else{
# 331
Cyc_Tcpat_check_pat_regions(te,_tmp78,this_pattern.patvars);}
# 333
({struct Cyc_Core_Opt*_tmp1A7=({struct Cyc_Core_Opt*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->v=this_pattern.patvars;_tmp7D;});((struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->pat_vars=_tmp1A7;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->where_clause,1);};}}
# 337
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp13C,_tmp13B);
return;};};}case 15U: _LL1D: _tmp140=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp13F=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_tmp13E=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp44)->f3;_LL1E:
# 345
({void*_tmp1AB=({void*_tmp1AA=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1A9=Cyc_Absyn_new_stmt(_tmp140->r,_tmp140->loc);_tmp7E->f1=_tmp1A9;}),({struct Cyc_Absyn_Stmt*_tmp1A8=Cyc_Absyn_skip_stmt(_tmp140->loc);_tmp7E->f2=_tmp1A8;});_tmp7E;});Cyc_Absyn_new_stmt(_tmp1AA,_tmp140->loc);})->r;_tmp140->r=_tmp1AB;});
# 347
Cyc_Tcstmt_tcStmt(te,_tmp140,1);{
# 350
struct _tuple2*def_v=({struct _tuple2*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp1AD=({struct _dyneither_ptr*_tmp94=_cycalloc(sizeof(*_tmp94));({struct _dyneither_ptr _tmp1AC=({const char*_tmp93="exn";_tag_dyneither(_tmp93,sizeof(char),4U);});*_tmp94=_tmp1AC;});_tmp94;});_tmp95->f2=_tmp1AD;});_tmp95;});
struct Cyc_Absyn_Vardecl*_tmp7F=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0);
struct Cyc_Absyn_Pat*_tmp80=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=1U,_tmp92->f1=_tmp7F,({struct Cyc_Absyn_Pat*_tmp1AE=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp92->f2=_tmp1AE;});_tmp92;}),0U);
struct Cyc_Absyn_Stmt*_tmp81=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->tag=11U,({struct Cyc_Absyn_Exp*_tmp1AF=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=4U,_tmp90->f1=_tmp7F;_tmp90;}),0U);_tmp91->f1=_tmp1AF;}),_tmp91->f2=1;_tmp91;}),0U),0U);
struct Cyc_Absyn_Switch_clause*_tmp82=({struct Cyc_Absyn_Switch_clause*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->pattern=_tmp80,_tmp8F->pat_vars=0,_tmp8F->where_clause=0,_tmp8F->body=_tmp81,_tmp8F->loc=0U;_tmp8F;});
({struct Cyc_List_List*_tmp1B1=({struct Cyc_List_List*_tmp1B0=*_tmp13F;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1B0,({struct Cyc_Absyn_Switch_clause*_tmp83[1U];_tmp83[0]=_tmp82;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp83,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*_tmp13F=_tmp1B1;});{
# 362
void*_tmp84=Cyc_Absyn_exn_type();
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp85=*_tmp13F;for(0;_tmp85 != 0;_tmp85=_tmp85->tl){
if(first_case){
first_case=0;
({struct Cyc_Tcpat_TcPatResult _tmp1B2=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp85->hd)->pattern,& _tmp84,0);this_pattern=_tmp1B2;});}else{
# 371
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 373
if(_tmp85->tl != 0){
({struct Cyc_Tcpat_TcPatResult*_tmp1B4=({struct Cyc_Tcpat_TcPatResult*_tmp86=_cycalloc(sizeof(*_tmp86));({struct Cyc_Tcpat_TcPatResult _tmp1B3=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp85->tl))->hd)->pattern,& _tmp84,0);*_tmp86=_tmp1B3;});_tmp86;});next_pattern=_tmp1B4;});
if(next_pattern->tvars_and_bounds_opt != 0  && (*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 377
({struct Cyc_Tcenv_Tenv*_tmp1B5=Cyc_Tcenv_clear_fallthru(te);te=_tmp1B5;});else{
# 379
struct Cyc_List_List*_tmp87=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp88=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
({struct Cyc_Tcenv_Tenv*_tmp1B6=Cyc_Tcenv_set_fallthru(te,_tmp88,_tmp87,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp85->tl))->hd);te=_tmp1B6;});}}else{
# 385
({struct Cyc_Tcenv_Tenv*_tmp1B7=Cyc_Tcenv_clear_fallthru(te);te=_tmp1B7;});}{
# 387
struct Cyc_Absyn_Pat*_tmp89=((struct Cyc_Absyn_Switch_clause*)_tmp85->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp89->topt),_tmp84)){
({struct Cyc_String_pa_PrintArg_struct _tmp8C=({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0U,({
struct _dyneither_ptr _tmp1B8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp84));_tmp14E.f1=_tmp1B8;});_tmp14E;});struct Cyc_String_pa_PrintArg_struct _tmp8D=({struct Cyc_String_pa_PrintArg_struct _tmp14D;_tmp14D.tag=0U,({struct _dyneither_ptr _tmp1B9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp89->topt)));_tmp14D.f1=_tmp1B9;});_tmp14D;});void*_tmp8A[2U];_tmp8A[0]=& _tmp8C,_tmp8A[1]=& _tmp8D;({unsigned int _tmp1BB=((struct Cyc_Absyn_Switch_clause*)_tmp85->hd)->loc;struct _dyneither_ptr _tmp1BA=({const char*_tmp8B="switch on type %s, but case expects type %s";_tag_dyneither(_tmp8B,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1BB,_tmp1BA,_tag_dyneither(_tmp8A,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}else{
# 394
Cyc_Tcpat_check_pat_regions(te,_tmp89,this_pattern.patvars);}
# 396
({struct Cyc_Core_Opt*_tmp1BC=({struct Cyc_Core_Opt*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->v=this_pattern.patvars;_tmp8E;});((struct Cyc_Absyn_Switch_clause*)_tmp85->hd)->pat_vars=_tmp1BC;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp85->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp85->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp85->hd)->where_clause,1);};}}
# 400
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*_tmp13F,_tmp13E);
return;};};default: _LL1F: _tmp142=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp141=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL20: {
# 404
struct _dyneither_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmp96=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;
{void*_tmp97=_tmp142->r;void*_tmp98=_tmp97;struct _tuple2*_tmp120;struct Cyc_List_List*_tmp11F;struct _dyneither_ptr*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_Absyn_Fndecl*_tmp11C;struct Cyc_Absyn_Tvar*_tmp11B;struct Cyc_Absyn_Vardecl*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_List_List*_tmp118;struct Cyc_Absyn_Pat*_tmp117;struct Cyc_Core_Opt**_tmp116;struct Cyc_Absyn_Exp**_tmp115;void**_tmp114;struct Cyc_Absyn_Vardecl*_tmp113;switch(*((int*)_tmp98)){case 0U: _LL22: _tmp113=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp98)->f1;_LL23: {
# 408
struct Cyc_Absyn_Vardecl*_tmp99=_tmp113;enum Cyc_Absyn_Scope _tmpCC;union Cyc_Absyn_Nmspace _tmpCB;struct _dyneither_ptr*_tmpCA;struct Cyc_Absyn_Tqual _tmpC9;void*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;void**_tmpC6;struct Cyc_List_List*_tmpC5;_LL41: _tmpCC=_tmp99->sc;_tmpCB=(_tmp99->name)->f1;_tmpCA=(_tmp99->name)->f2;_tmpC9=_tmp99->tq;_tmpC8=_tmp99->type;_tmpC7=_tmp99->initializer;_tmpC6=(void**)& _tmp99->rgn;_tmpC5=_tmp99->attributes;_LL42:;{
void*_tmp9A=Cyc_Tcenv_curr_rgn(_tmp96);
int is_local;
{enum Cyc_Absyn_Scope _tmp9B=_tmpCC;switch(_tmp9B){case Cyc_Absyn_Static: _LL44: _LL45:
 goto _LL47;case Cyc_Absyn_Extern: _LL46: _LL47:
 goto _LL49;case Cyc_Absyn_ExternC: _LL48: _LL49:
# 415
 _tmp113->escapes=1;
is_local=0;goto _LL43;case Cyc_Absyn_Abstract: _LL4A: _LL4B:
# 418
({void*_tmp9C=0U;({unsigned int _tmp1BE=_tmp142->loc;struct _dyneither_ptr _tmp1BD=({const char*_tmp9D="bad abstract scope for local variable";_tag_dyneither(_tmp9D,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp1BE,_tmp1BD,_tag_dyneither(_tmp9C,sizeof(void*),0U));});});
goto _LL4D;case Cyc_Absyn_Register: _LL4C: _LL4D:
 goto _LL4F;default: _LL4E: _LL4F:
 is_local=1;goto _LL43;}_LL43:;}
# 424
*_tmpC6=is_local?_tmp9A: Cyc_Absyn_heap_rgn_type;
# 426
{void*_tmp9E=Cyc_Tcutil_compress(_tmpC8);void*_tmp9F=_tmp9E;void*_tmpAB;struct Cyc_Absyn_Tqual _tmpAA;void*_tmpA9;unsigned int _tmpA8;switch(*((int*)_tmp9F)){case 5U: _LL51: if(is_local){_LL52:
# 429
 _tmp113->escapes=1;
_tmpCC=3U;
is_local=0;
goto _LL50;}else{goto _LL55;}case 4U: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F)->f1).num_elts == 0){_LL53: _tmpAB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F)->f1).elt_type;_tmpAA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F)->f1).tq;_tmpA9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F)->f1).zero_term;_tmpA8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F)->f1).zt_loc;if(_tmp113->initializer != 0){_LL54:
# 434
{void*_tmpA0=((struct Cyc_Absyn_Exp*)_check_null(_tmp113->initializer))->r;void*_tmpA1=_tmpA0;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA6;struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*_tmpA4;struct _dyneither_ptr _tmpA3;struct _dyneither_ptr _tmpA2;switch(*((int*)_tmpA1)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA1)->f1).Wstring_c).tag){case 8U: _LL58: _tmpA2=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA1)->f1).String_c).val;_LL59:
# 436
({void*_tmp1C4=({void*_tmp1C3=({void*_tmp1C2=_tmpAB;struct Cyc_Absyn_Tqual _tmp1C1=_tmpAA;struct Cyc_Absyn_Exp*_tmp1C0=Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpA2,sizeof(char)),0U);void*_tmp1BF=_tmpA9;Cyc_Absyn_array_type(_tmp1C2,_tmp1C1,_tmp1C0,_tmp1BF,_tmpA8);});_tmp113->type=_tmp1C3;});_tmpC8=_tmp1C4;});
goto _LL57;case 9U: _LL5A: _tmpA3=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA1)->f1).Wstring_c).val;_LL5B:
# 439
({void*_tmp1CA=({void*_tmp1C9=({void*_tmp1C8=_tmpAB;struct Cyc_Absyn_Tqual _tmp1C7=_tmpAA;struct Cyc_Absyn_Exp*_tmp1C6=Cyc_Absyn_uint_exp(1U,0U);void*_tmp1C5=_tmpA9;Cyc_Absyn_array_type(_tmp1C8,_tmp1C7,_tmp1C6,_tmp1C5,_tmpA8);});_tmp113->type=_tmp1C9;});_tmpC8=_tmp1CA;});
goto _LL57;default: goto _LL64;}case 27U: _LL5C: _tmpA4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA1)->f2;_LL5D:
 _tmpA5=_tmpA4;goto _LL5F;case 28U: _LL5E: _tmpA5=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA1)->f1;_LL5F:
# 444
({void*_tmp1CC=({void*_tmp1CB=Cyc_Absyn_array_type(_tmpAB,_tmpAA,_tmpA5,_tmpA9,_tmpA8);_tmp113->type=_tmp1CB;});_tmpC8=_tmp1CC;});
goto _LL57;case 36U: _LL60: _tmpA6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA1)->f2;_LL61:
 _tmpA7=_tmpA6;goto _LL63;case 26U: _LL62: _tmpA7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA1)->f1;_LL63:
# 448
({void*_tmp1D2=({void*_tmp1D1=({void*_tmp1D0=_tmpAB;struct Cyc_Absyn_Tqual _tmp1CF=_tmpAA;struct Cyc_Absyn_Exp*_tmp1CE=Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA7),0U);void*_tmp1CD=_tmpA9;Cyc_Absyn_array_type(_tmp1D0,_tmp1CF,_tmp1CE,_tmp1CD,_tmpA8);});_tmp113->type=_tmp1D1;});_tmpC8=_tmp1D2;});
goto _LL57;default: _LL64: _LL65:
 goto _LL57;}_LL57:;}
# 452
goto _LL50;}else{goto _LL55;}}else{goto _LL55;}default: _LL55: _LL56:
 goto _LL50;}_LL50:;}{
# 459
struct Cyc_List_List*_tmpAC=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmp96);
int _tmpAD=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmp96,_tmpAC,& Cyc_Tcutil_tmk,_tmpAD,1,_tmpC8);
({int _tmp1D3=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmp113->tq).print_const,_tmpC8);(_tmp113->tq).real_const=_tmp1D3;});{
# 464
struct Cyc_Tcenv_Tenv*_tmpAE=_tmp96;
if(_tmpCC == Cyc_Absyn_Extern  || _tmpCC == Cyc_Absyn_ExternC)
({void*_tmpAF=0U;({unsigned int _tmp1D5=_tmp142->loc;struct _dyneither_ptr _tmp1D4=({const char*_tmpB0="extern declarations are not yet supported within functions";_tag_dyneither(_tmpB0,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1D5,_tmp1D4,_tag_dyneither(_tmpAF,sizeof(void*),0U));});});
if(_tmpC7 != 0){
Cyc_Tcexp_tcExpInitializer(_tmpAE,& _tmpC8,_tmpC7);
# 470
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpC7))
({void*_tmpB1=0U;({unsigned int _tmp1D7=_tmp142->loc;struct _dyneither_ptr _tmp1D6=({const char*_tmpB2="initializer for static variable needs to be a constant expression";_tag_dyneither(_tmpB2,sizeof(char),66U);});Cyc_Tcutil_terr(_tmp1D7,_tmp1D6,_tag_dyneither(_tmpB1,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(_tmpAE,_tmpC7,_tmpC8)){
struct _dyneither_ptr _tmpB3=*_tmpCA;
struct _dyneither_ptr _tmpB4=Cyc_Absynpp_typ2string(_tmpC8);
struct _dyneither_ptr _tmpB5=Cyc_Absynpp_typ2string((void*)_check_null(_tmpC7->topt));
if(((_get_dyneither_size(_tmpB3,sizeof(char))+ _get_dyneither_size(_tmpB4,sizeof(char)))+ _get_dyneither_size(_tmpB5,sizeof(char)))+ 50 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpB8=({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0U,_tmp151.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpB3);_tmp151;});struct Cyc_String_pa_PrintArg_struct _tmpB9=({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0U,_tmp150.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpB4);_tmp150;});struct Cyc_String_pa_PrintArg_struct _tmpBA=({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0U,_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpB5);_tmp14F;});void*_tmpB6[3U];_tmpB6[0]=& _tmpB8,_tmpB6[1]=& _tmpB9,_tmpB6[2]=& _tmpBA;({unsigned int _tmp1D9=_tmp142->loc;struct _dyneither_ptr _tmp1D8=({const char*_tmpB7="%s was declared with type %s but initialized with type %s.";_tag_dyneither(_tmpB7,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1D9,_tmp1D8,_tag_dyneither(_tmpB6,sizeof(void*),3U));});});else{
# 479
if((_get_dyneither_size(_tmpB3,sizeof(char))+ _get_dyneither_size(_tmpB4,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmpB5,sizeof(char))+ 25 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpBD=({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0U,_tmp154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpB3);_tmp154;});struct Cyc_String_pa_PrintArg_struct _tmpBE=({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0U,_tmp153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpB4);_tmp153;});struct Cyc_String_pa_PrintArg_struct _tmpBF=({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0U,_tmp152.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpB5);_tmp152;});void*_tmpBB[3U];_tmpBB[0]=& _tmpBD,_tmpBB[1]=& _tmpBE,_tmpBB[2]=& _tmpBF;({unsigned int _tmp1DB=_tmp142->loc;struct _dyneither_ptr _tmp1DA=({const char*_tmpBC="%s was declared with type %s\n but initialized with type %s.";_tag_dyneither(_tmpBC,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp1DB,_tmp1DA,_tag_dyneither(_tmpBB,sizeof(void*),3U));});});else{
# 484
({struct Cyc_String_pa_PrintArg_struct _tmpC2=({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0U,_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpB3);_tmp157;});struct Cyc_String_pa_PrintArg_struct _tmpC3=({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0U,_tmp156.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpB4);_tmp156;});struct Cyc_String_pa_PrintArg_struct _tmpC4=({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0U,_tmp155.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpB5);_tmp155;});void*_tmpC0[3U];_tmpC0[0]=& _tmpC2,_tmpC0[1]=& _tmpC3,_tmpC0[2]=& _tmpC4;({unsigned int _tmp1DD=_tmp142->loc;struct _dyneither_ptr _tmp1DC=({const char*_tmpC1="%s declared with type \n%s\n but initialized with type \n%s.";_tag_dyneither(_tmpC1,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp1DD,_tmp1DC,_tag_dyneither(_tmpC0,sizeof(void*),3U));});});}}
# 486
Cyc_Tcutil_unify(_tmpC8,(void*)_check_null(_tmpC7->topt));
Cyc_Tcutil_explain_failure();}}
# 490
Cyc_Tcstmt_tcStmt(_tmpAE,_tmp141,0);
return;};};};}case 2U: _LL24: _tmp117=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp98)->f1;_tmp116=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp98)->f2;_tmp115=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp98)->f3;_tmp114=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp98)->f4;_LL25: {
# 498
struct Cyc_Absyn_Exp*_tmpCD=*_tmp115;
{void*_tmpCE=_tmp117->r;void*_tmpCF=_tmpCE;switch(*((int*)_tmpCF)){case 1U: _LL67: _LL68:
 goto _LL6A;case 2U: _LL69: _LL6A:
 goto _LL6C;case 15U: _LL6B: _LL6C:
 goto _LL66;default: _LL6D: _LL6E:
# 504
 if(!Cyc_Tcutil_is_var_exp(_tmpCD)){
struct Cyc_Absyn_Vardecl*_tmpD0=({struct _tuple2*_tmp1DE=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1DE,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmpD1=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmpD2=({struct Cyc_Absyn_Pat*_tmp1E2=({void*_tmp1E0=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->tag=1U,_tmpD5->f1=_tmpD0,({struct Cyc_Absyn_Pat*_tmp1DF=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmpCD->loc);_tmpD5->f2=_tmp1DF;});_tmpD5;});Cyc_Absyn_new_pat(_tmp1E0,_tmpCD->loc);});struct Cyc_Absyn_Exp*_tmp1E1=_tmpCD;Cyc_Absyn_let_decl(_tmp1E2,_tmp1E1,s0->loc);});
# 510
struct Cyc_Absyn_Stmt*_tmpD3=Cyc_Absyn_decl_stmt(_tmpD2,_tmpD1,s0->loc);
# 512
s0->r=_tmpD3->r;
({struct Cyc_Absyn_Exp*_tmp1E4=({void*_tmp1E3=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->tag=4U,_tmpD4->f1=_tmpD0;_tmpD4;});Cyc_Absyn_varb_exp(_tmp1E3,_tmpCD->loc);});*_tmp115=_tmp1E4;});
Cyc_Tcstmt_tcStmt(_tmp96,s0,new_block);
return;}}_LL66:;}
# 518
Cyc_Tcexp_tcExpInitializer(_tmp96,0,_tmpCD);{
# 520
void*pat_type=(void*)_check_null(_tmpCD->topt);
# 522
struct Cyc_Tcpat_TcPatResult _tmpD6=Cyc_Tcpat_tcPat(_tmp96,_tmp117,& pat_type,_tmpCD);
({struct Cyc_Core_Opt*_tmp1E5=({struct Cyc_Core_Opt*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->v=_tmpD6.patvars;_tmpD7;});*_tmp116=_tmp1E5;});
if(!({void*_tmp1E6=(void*)_check_null(_tmp117->topt);Cyc_Tcutil_unify(_tmp1E6,(void*)_check_null(_tmpCD->topt));}) && !
Cyc_Tcutil_coerce_assign(_tmp96,_tmpCD,(void*)_check_null(_tmp117->topt))){
({struct Cyc_String_pa_PrintArg_struct _tmpDA=({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0U,({
struct _dyneither_ptr _tmp1E7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp117->topt)));_tmp159.f1=_tmp1E7;});_tmp159;});struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp158;_tmp158.tag=0U,({struct _dyneither_ptr _tmp1E8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpCD->topt)));_tmp158.f1=_tmp1E8;});_tmp158;});void*_tmpD8[2U];_tmpD8[0]=& _tmpDA,_tmpD8[1]=& _tmpDB;({unsigned int _tmp1EA=_tmp142->loc;struct _dyneither_ptr _tmp1E9=({const char*_tmpD9="pattern type %s does not match definition type %s";_tag_dyneither(_tmpD9,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp1EA,_tmp1E9,_tag_dyneither(_tmpD8,sizeof(void*),2U));});});
({void*_tmp1EB=(void*)_check_null(_tmp117->topt);Cyc_Tcutil_unify(_tmp1EB,(void*)_check_null(_tmpCD->topt));});
Cyc_Tcutil_explain_failure();}else{
# 532
Cyc_Tcpat_check_pat_regions(_tmp96,_tmp117,_tmpD6.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmp117->loc,_tmp96,_tmp117,_tmp114);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmp96,_tmpD6,_tmp141,0,0);
return;};}case 3U: _LL26: _tmp118=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp98)->f1;_LL27: {
# 538
void*_tmpDC=Cyc_Tcenv_curr_rgn(_tmp96);
struct Cyc_Tcenv_Tenv*_tmpDD=_tmp96;
for(0;_tmp118 != 0;_tmp118=_tmp118->tl){
struct Cyc_Absyn_Vardecl*_tmpDE=(struct Cyc_Absyn_Vardecl*)_tmp118->hd;
struct Cyc_Absyn_Vardecl*_tmpDF=_tmpDE;union Cyc_Absyn_Nmspace _tmpE2;void*_tmpE1;void**_tmpE0;_LL70: _tmpE2=(_tmpDF->name)->f1;_tmpE1=_tmpDF->type;_tmpE0=(void**)& _tmpDF->rgn;_LL71:;
*_tmpE0=_tmpDC;
({unsigned int _tmp1EE=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1ED=_tmpDD;struct Cyc_List_List*_tmp1EC=Cyc_Tcenv_lookup_type_vars(_tmpDD);Cyc_Tcutil_check_type(_tmp1EE,_tmp1ED,_tmp1EC,& Cyc_Tcutil_tmk,1,1,_tmpE1);});}
# 546
Cyc_Tcstmt_tcStmt(_tmpDD,_tmp141,0);
return;}case 4U: _LL28: _tmp11B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp98)->f1;_tmp11A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp98)->f2;_tmp119=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp98)->f3;_LL29:
# 552
({void*_tmp1F2=({void*_tmp1F1=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1F0=Cyc_Absyn_new_stmt(_tmp141->r,_tmp141->loc);_tmpE3->f1=_tmp1F0;}),({struct Cyc_Absyn_Stmt*_tmp1EF=Cyc_Absyn_skip_stmt(_tmp141->loc);_tmpE3->f2=_tmp1EF;});_tmpE3;});Cyc_Absyn_new_stmt(_tmp1F1,_tmp141->loc);})->r;_tmp141->r=_tmp1F2;});{
# 554
struct Cyc_Tcenv_Tenv*_tmpE4=_tmp96;
struct Cyc_Absyn_Vardecl*_tmpE5=_tmp11A;void**_tmpFF;void**_tmpFE;_LL73: _tmpFF=(void**)& _tmpE5->type;_tmpFE=(void**)& _tmpE5->rgn;_LL74:;{
void*_tmpE6=Cyc_Tcenv_curr_rgn(_tmp96);
*_tmpFE=_tmpE6;{
void*rgn_typ;
if((unsigned int)_tmp119){
# 562
struct _tuple2*drname=({struct _tuple2*_tmpF9=_cycalloc(sizeof(*_tmpF9));((_tmpF9->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp1F7=({struct _dyneither_ptr*_tmpF4[1U];({struct _dyneither_ptr*_tmp1F6=({struct _dyneither_ptr*_tmpF6=_cycalloc(sizeof(*_tmpF6));({struct _dyneither_ptr _tmp1F5=({const char*_tmpF5="Core";_tag_dyneither(_tmpF5,sizeof(char),5U);});*_tmpF6=_tmp1F5;});_tmpF6;});_tmpF4[0]=_tmp1F6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF4,sizeof(struct _dyneither_ptr*),1U));});((_tmpF9->f1).Abs_n).val=_tmp1F7;}),({
struct _dyneither_ptr*_tmp1F4=({struct _dyneither_ptr*_tmpF8=_cycalloc(sizeof(*_tmpF8));({struct _dyneither_ptr _tmp1F3=({const char*_tmpF7="DynamicRegion";_tag_dyneither(_tmpF7,sizeof(char),14U);});*_tmpF8=_tmp1F3;});_tmpF8;});_tmpF9->f2=_tmp1F4;});_tmpF9;});
void*_tmpE7=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->v=& Cyc_Tcutil_trk;_tmpF3;}),0);
({void*_tmp1F8=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->v=& Cyc_Tcutil_rk;_tmpE8;}),0);rgn_typ=_tmp1F8;});{
void*_tmpE9=({
union Cyc_Absyn_AggrInfo _tmp1F9=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);Cyc_Absyn_aggr_type(_tmp1F9,({void*_tmpF2[1U];_tmpF2[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF2,sizeof(void*),1U));}));});
void*_tmpEA=({void*_tmp1FC=_tmpE9;void*_tmp1FB=_tmpE7;struct Cyc_Absyn_Tqual _tmp1FA=
Cyc_Absyn_empty_tqual(0U);
# 568
Cyc_Absyn_at_type(_tmp1FC,_tmp1FB,_tmp1FA,Cyc_Absyn_false_type);});
# 570
({unsigned int _tmp1FF=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1FE=_tmp96;struct Cyc_List_List*_tmp1FD=Cyc_Tcenv_lookup_type_vars(_tmp96);Cyc_Tcutil_check_type(_tmp1FF,_tmp1FE,_tmp1FD,& Cyc_Tcutil_tmk,1,0,_tmpEA);});{
void*_tmpEB=Cyc_Tcexp_tcExp(_tmp96,& _tmpEA,_tmp119);
if(!Cyc_Tcutil_unify(_tmpEA,_tmpEB) && !
Cyc_Tcutil_coerce_assign(_tmp96,_tmp119,_tmpEA))
({struct Cyc_String_pa_PrintArg_struct _tmpEE=({struct Cyc_String_pa_PrintArg_struct _tmp15B;_tmp15B.tag=0U,({
struct _dyneither_ptr _tmp200=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpEA));_tmp15B.f1=_tmp200;});_tmp15B;});struct Cyc_String_pa_PrintArg_struct _tmpEF=({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0U,({struct _dyneither_ptr _tmp201=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpEB));_tmp15A.f1=_tmp201;});_tmp15A;});void*_tmpEC[2U];_tmpEC[0]=& _tmpEE,_tmpEC[1]=& _tmpEF;({unsigned int _tmp203=s0->loc;struct _dyneither_ptr _tmp202=({const char*_tmpED="expected %s but found %s";_tag_dyneither(_tmpED,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp203,_tmp202,_tag_dyneither(_tmpEC,sizeof(void*),2U));});});
if(!Cyc_Tcutil_unify(_tmpE7,Cyc_Absyn_unique_rgn_type) && !
Cyc_Tcutil_unify(_tmpE7,Cyc_Absyn_refcnt_rgn_type))
({void*_tmpF0=0U;({unsigned int _tmp205=s0->loc;struct _dyneither_ptr _tmp204=({const char*_tmpF1="open is only allowed on unique or reference-counted keys";_tag_dyneither(_tmpF1,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp205,_tmp204,_tag_dyneither(_tmpF0,sizeof(void*),0U));});});
({void*_tmp206=Cyc_Absyn_rgn_handle_type(rgn_typ);*_tmpFF=_tmp206;});
({struct Cyc_Tcenv_Tenv*_tmp207=Cyc_Tcenv_add_region(_tmpE4,rgn_typ,1);_tmpE4=_tmp207;});};};}else{
# 582
({void*_tmp208=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=2U,_tmpFA->f1=_tmp11B;_tmpFA;});rgn_typ=_tmp208;});
({struct Cyc_Tcenv_Tenv*_tmp20B=({unsigned int _tmp20A=s0->loc;struct Cyc_Tcenv_Tenv*_tmp209=_tmpE4;Cyc_Tcenv_add_type_vars(_tmp20A,_tmp209,({struct Cyc_List_List*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->hd=_tmp11B,_tmpFB->tl=0;_tmpFB;}));});_tmpE4=_tmp20B;});
({struct Cyc_Tcenv_Tenv*_tmp20C=Cyc_Tcenv_add_region(_tmpE4,rgn_typ,0);_tmpE4=_tmp20C;});}
# 586
({unsigned int _tmp20F=s0->loc;struct Cyc_Tcenv_Tenv*_tmp20E=_tmp96;struct Cyc_List_List*_tmp20D=Cyc_Tcenv_lookup_type_vars(_tmpE4);Cyc_Tcutil_check_type(_tmp20F,_tmp20E,_tmp20D,& Cyc_Tcutil_bk,1,0,*_tmpFF);});
if(!({void*_tmp210=*_tmpFF;Cyc_Tcutil_unify(_tmp210,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({void*_tmpFC=0U;({struct _dyneither_ptr _tmp211=({const char*_tmpFD="region stmt: type of region handle is wrong!";_tag_dyneither(_tmpFD,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp211,_tag_dyneither(_tmpFC,sizeof(void*),0U));});});
# 590
Cyc_Tcstmt_tcStmt(_tmpE4,_tmp141,0);
return;};};};case 1U: _LL2A: _tmp11C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp98)->f1;_LL2B: {
# 595
void*_tmp100=Cyc_Tcenv_curr_rgn(_tmp96);
if(_tmp11C->sc != Cyc_Absyn_Public)
({void*_tmp101=0U;({unsigned int _tmp213=_tmp142->loc;struct _dyneither_ptr _tmp212=({const char*_tmp102="bad storage class for inner function";_tag_dyneither(_tmp102,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp213,_tmp212,_tag_dyneither(_tmp101,sizeof(void*),0U));});});
Cyc_Tcutil_check_fndecl_valid_type(_tmp142->loc,_tmp96,_tmp11C);{
void*t=Cyc_Tcutil_fndecl2type(_tmp11C);
({struct Cyc_List_List*_tmp214=Cyc_Tcutil_transfer_fn_type_atts(t,_tmp11C->attributes);_tmp11C->attributes=_tmp214;});
# 602
{struct Cyc_List_List*atts=_tmp11C->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmp103=(void*)atts->hd;void*_tmp104=_tmp103;switch(*((int*)_tmp104)){case 7U: _LL76: _LL77:
 goto _LL79;case 6U: _LL78: _LL79:
({struct Cyc_String_pa_PrintArg_struct _tmp107=({struct Cyc_String_pa_PrintArg_struct _tmp15C;_tmp15C.tag=0U,({
struct _dyneither_ptr _tmp215=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp15C.f1=_tmp215;});_tmp15C;});void*_tmp105[1U];_tmp105[0]=& _tmp107;({unsigned int _tmp217=_tmp142->loc;struct _dyneither_ptr _tmp216=({const char*_tmp106="bad attribute %s for function";_tag_dyneither(_tmp106,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp217,_tmp216,_tag_dyneither(_tmp105,sizeof(void*),1U));});});
goto _LL75;default: _LL7A: _LL7B:
 goto _LL75;}_LL75:;}}{
# 610
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->sc=_tmp11C->sc,_tmp108->name=_tmp11C->name,_tmp108->varloc=0U,({struct Cyc_Absyn_Tqual _tmp21C=Cyc_Absyn_const_tqual(0U);_tmp108->tq=_tmp21C;}),({
void*_tmp21B=({void*_tmp21A=t;void*_tmp219=_tmp100;struct Cyc_Absyn_Tqual _tmp218=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp21A,_tmp219,_tmp218,Cyc_Absyn_false_type);});_tmp108->type=_tmp21B;}),_tmp108->initializer=0,_tmp108->rgn=_tmp100,_tmp108->attributes=0,_tmp108->escapes=0;_tmp108;});
# 614
_tmp11C->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmp96->le);
({struct Cyc_Tcenv_Fenv*_tmp21D=Cyc_Tcenv_nested_fenv(_tmp142->loc,old_fenv,_tmp11C);_tmp96->le=_tmp21D;});
Cyc_Tcstmt_tcStmt(_tmp96,_tmp11C->body,0);
Cyc_Tcenv_check_delayed_effects(_tmp96);
Cyc_Tcenv_check_delayed_constraints(_tmp96);
_tmp96->le=old_fenv;
Cyc_Tcstmt_tcStmt(_tmp96,_tmp141,0);
return;};};};}case 9U: _LL2C: _tmp11E=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp98)->f1;_tmp11D=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp98)->f2;_LL2D:
({struct _dyneither_ptr _tmp21E=({const char*_tmp109="namespace";_tag_dyneither(_tmp109,sizeof(char),10U);});unimp_msg_part=_tmp21E;});goto _LL21;case 10U: _LL2E: _tmp120=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp98)->f1;_tmp11F=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp98)->f2;_LL2F:
({struct _dyneither_ptr _tmp21F=({const char*_tmp10A="using";_tag_dyneither(_tmp10A,sizeof(char),6U);});unimp_msg_part=_tmp21F;});goto _LL21;case 5U: _LL30: _LL31:
({struct _dyneither_ptr _tmp220=({const char*_tmp10B="type";_tag_dyneither(_tmp10B,sizeof(char),5U);});unimp_msg_part=_tmp220;});goto _LL21;case 6U: _LL32: _LL33:
({struct _dyneither_ptr _tmp221=({const char*_tmp10C="datatype";_tag_dyneither(_tmp10C,sizeof(char),9U);});unimp_msg_part=_tmp221;});goto _LL21;case 7U: _LL34: _LL35:
({struct _dyneither_ptr _tmp222=({const char*_tmp10D="enum";_tag_dyneither(_tmp10D,sizeof(char),5U);});unimp_msg_part=_tmp222;});goto _LL21;case 8U: _LL36: _LL37:
({struct _dyneither_ptr _tmp223=({const char*_tmp10E="typedef";_tag_dyneither(_tmp10E,sizeof(char),8U);});unimp_msg_part=_tmp223;});goto _LL21;case 11U: _LL38: _LL39:
({struct _dyneither_ptr _tmp224=({const char*_tmp10F="extern \"C\"";_tag_dyneither(_tmp10F,sizeof(char),11U);});unimp_msg_part=_tmp224;});goto _LL21;case 12U: _LL3A: _LL3B:
# 631
({struct _dyneither_ptr _tmp225=({const char*_tmp110="extern \"C include\"";_tag_dyneither(_tmp110,sizeof(char),19U);});unimp_msg_part=_tmp225;});goto _LL21;case 13U: _LL3C: _LL3D:
({struct _dyneither_ptr _tmp226=({const char*_tmp111="__cyclone_port_on__";_tag_dyneither(_tmp111,sizeof(char),20U);});unimp_msg_part=_tmp226;});goto _LL21;default: _LL3E: _LL3F:
({struct _dyneither_ptr _tmp227=({const char*_tmp112="__cyclone_port_off__";_tag_dyneither(_tmp112,sizeof(char),21U);});unimp_msg_part=_tmp227;});goto _LL21;}_LL21:;}
# 635
(int)_throw(({struct Cyc_String_pa_PrintArg_struct _tmp123=({struct Cyc_String_pa_PrintArg_struct _tmp15D;_tmp15D.tag=0U,_tmp15D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part);_tmp15D;});void*_tmp121[1U];_tmp121[0]=& _tmp123;({struct _dyneither_ptr _tmp228=({const char*_tmp122="tcStmt: nested %s declarations unimplemented";_tag_dyneither(_tmp122,sizeof(char),45U);});((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp228,_tag_dyneither(_tmp121,sizeof(void*),1U));});}));}}_LL0:;}

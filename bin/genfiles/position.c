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
 struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 72 "string.h"
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 33 "lineno.h"
struct Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(struct _dyneither_ptr,int);
# 28 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
void Cyc_Position_set_position_file(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Position_get_position_file();
# 34
unsigned int Cyc_Position_segment_join(unsigned int,unsigned int);
int Cyc_Position_segment_equals(unsigned int,unsigned int);
struct _dyneither_ptr Cyc_Position_string_of_loc(unsigned int);
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
# 43
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 46
extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);
int Cyc_Position_error_p();
# 55
struct _dyneither_ptr Cyc_Position_get_line_directive(unsigned int loc);
# 34 "position.cyc"
int Cyc_Position_use_gcc_style_location=1;static char _tmp0[1U]="";
# 37
static struct _dyneither_ptr Cyc_Position_source={_tmp0,_tmp0,_tmp0 + 1U};
# 39
unsigned int Cyc_Position_segment_join(unsigned int s1,unsigned int s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return s1;}
# 44
int Cyc_Position_segment_equals(unsigned int s1,unsigned int s2){
return s1 == s2;}
# 48
struct _dyneither_ptr Cyc_Position_string_of_loc(unsigned int loc){
struct Cyc_Lineno_Pos*pos=Cyc_Lineno_pos_of_abs(Cyc_Position_source,(int)loc);
if(Cyc_Position_use_gcc_style_location)
return({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmp26;_tmp26.tag=0U,_tmp26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos->logical_file);_tmp26;});struct Cyc_Int_pa_PrintArg_struct _tmp4=({struct Cyc_Int_pa_PrintArg_struct _tmp25;_tmp25.tag=1U,_tmp25.f1=(unsigned long)pos->line_no;_tmp25;});void*_tmp1[2U];_tmp1[0]=& _tmp3,_tmp1[1]=& _tmp4;({struct _dyneither_ptr _tmp33=({const char*_tmp2="%s:%d";_tag_dyneither(_tmp2,sizeof(char),6U);});Cyc_aprintf(_tmp33,_tag_dyneither(_tmp1,sizeof(void*),2U));});});else{
# 53
return({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp29;_tmp29.tag=0U,_tmp29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos->logical_file);_tmp29;});struct Cyc_Int_pa_PrintArg_struct _tmp8=({struct Cyc_Int_pa_PrintArg_struct _tmp28;_tmp28.tag=1U,_tmp28.f1=(unsigned long)pos->line_no;_tmp28;});struct Cyc_Int_pa_PrintArg_struct _tmp9=({struct Cyc_Int_pa_PrintArg_struct _tmp27;_tmp27.tag=1U,_tmp27.f1=(unsigned long)pos->col;_tmp27;});void*_tmp5[3U];_tmp5[0]=& _tmp7,_tmp5[1]=& _tmp8,_tmp5[2]=& _tmp9;({struct _dyneither_ptr _tmp34=({const char*_tmp6="%s:(%d:%d)";_tag_dyneither(_tmp6,sizeof(char),11U);});Cyc_aprintf(_tmp34,_tag_dyneither(_tmp5,sizeof(void*),3U));});});}}
# 56
static struct _dyneither_ptr Cyc_Position_string_of_pos_pr(struct Cyc_Lineno_Pos*pos){
if(Cyc_Position_use_gcc_style_location)
return({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0U,_tmp2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos->logical_file);_tmp2B;});struct Cyc_Int_pa_PrintArg_struct _tmpD=({struct Cyc_Int_pa_PrintArg_struct _tmp2A;_tmp2A.tag=1U,_tmp2A.f1=(unsigned long)pos->line_no;_tmp2A;});void*_tmpA[2U];_tmpA[0]=& _tmpC,_tmpA[1]=& _tmpD;({struct _dyneither_ptr _tmp35=({const char*_tmpB="%s:%d";_tag_dyneither(_tmpB,sizeof(char),6U);});Cyc_aprintf(_tmp35,_tag_dyneither(_tmpA,sizeof(void*),2U));});});else{
# 60
return({struct Cyc_String_pa_PrintArg_struct _tmp10=({struct Cyc_String_pa_PrintArg_struct _tmp2E;_tmp2E.tag=0U,_tmp2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos->logical_file);_tmp2E;});struct Cyc_Int_pa_PrintArg_struct _tmp11=({struct Cyc_Int_pa_PrintArg_struct _tmp2D;_tmp2D.tag=1U,_tmp2D.f1=(unsigned long)pos->line_no;_tmp2D;});struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp2C;_tmp2C.tag=1U,_tmp2C.f1=(unsigned long)pos->col;_tmp2C;});void*_tmpE[3U];_tmpE[0]=& _tmp10,_tmpE[1]=& _tmp11,_tmpE[2]=& _tmp12;({struct _dyneither_ptr _tmp36=({const char*_tmpF="%s:(%d:%d)";_tag_dyneither(_tmpF,sizeof(char),11U);});Cyc_aprintf(_tmp36,_tag_dyneither(_tmpE,sizeof(void*),3U));});});}}
# 63
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int s){
return Cyc_Position_string_of_loc(s);}
# 67
static struct Cyc_Lineno_Pos*Cyc_Position_new_pos(){
return({struct Cyc_Lineno_Pos*_tmp14=_cycalloc(sizeof(*_tmp14));({struct _dyneither_ptr _tmp38=({const char*_tmp13="";_tag_dyneither(_tmp13,sizeof(char),1U);});_tmp14->logical_file=_tmp38;}),({struct _dyneither_ptr _tmp37=Cyc_Core_new_string(0U);_tmp14->line=_tmp37;}),_tmp14->line_no=0,_tmp14->col=0;_tmp14;});}
# 71
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*segs){
# 73
struct Cyc_List_List*ans=0;
for(0;segs != 0;segs=segs->tl){
({struct Cyc_List_List*_tmp3B=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct _dyneither_ptr*_tmp3A=({struct _dyneither_ptr*_tmp15=_cycalloc(sizeof(*_tmp15));({struct _dyneither_ptr _tmp39=Cyc_Position_string_of_segment((unsigned int)segs->hd);*_tmp15=_tmp39;});_tmp15;});_tmp16->hd=_tmp3A;}),_tmp16->tl=ans;_tmp16;});ans=_tmp3B;});}
return ans;}struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;struct _dyneither_ptr desc;};
# 99 "position.cyc"
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int l,struct _dyneither_ptr desc){
return({struct Cyc_Position_Error*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->source=Cyc_Position_source,_tmp17->seg=l,_tmp17->desc=desc;_tmp17;});}
# 105
static struct _dyneither_ptr Cyc_Position_trunc(int n,struct _dyneither_ptr s){
int len=(int)Cyc_strlen((struct _dyneither_ptr)s);
if(len < n)
return s;{
int len_one=(n - 3)/ 2;
int len_two=(n - 3)- len_one;
struct _dyneither_ptr mans=Cyc_Core_new_string((unsigned int)(n + 1));
struct _dyneither_ptr ans=_dyneither_ptr_decrease_size(mans,sizeof(char),1U);
Cyc_strncpy(ans,(struct _dyneither_ptr)s,(unsigned long)len_one);
({struct _dyneither_ptr _tmp3C=_dyneither_ptr_plus(ans,sizeof(char),len_one);Cyc_strncpy(_tmp3C,({const char*_tmp18="...";_tag_dyneither(_tmp18,sizeof(char),4U);}),3U);});
({struct _dyneither_ptr _tmp3E=_dyneither_ptr_plus(ans,sizeof(char),len_one + 3);struct _dyneither_ptr _tmp3D=(struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),len - len_two);Cyc_strncpy(_tmp3E,_tmp3D,(unsigned long)len_two);});
return mans;};}
# 119
static int Cyc_Position_line_length=76;
# 121
static int Cyc_Position_error_b=0;
int Cyc_Position_error_p(){return Cyc_Position_error_b;}
# 124
int Cyc_Position_print_context=0;
# 126
int Cyc_Position_first_error=1;
# 128
int Cyc_Position_num_errors=0;
int Cyc_Position_max_errors=10;
# 131
void Cyc_Position_post_error(struct Cyc_Position_Error*e){
Cyc_Position_error_b=1;
Cyc_fflush(Cyc_stdout);
if(Cyc_Position_first_error){
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmp40=Cyc_stderr;struct _dyneither_ptr _tmp3F=({const char*_tmp1A="\n";_tag_dyneither(_tmp1A,sizeof(char),2U);});Cyc_fprintf(_tmp40,_tmp3F,_tag_dyneither(_tmp19,sizeof(void*),0U));});});
Cyc_Position_first_error=0;}
# 138
if(Cyc_Position_num_errors <= Cyc_Position_max_errors)
# 140
({struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp30;_tmp30.tag=0U,({struct _dyneither_ptr _tmp41=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e->seg));_tmp30.f1=_tmp41;});_tmp30;});struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0U,_tmp2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)e->desc);_tmp2F;});void*_tmp1B[2U];_tmp1B[0]=& _tmp1D,_tmp1B[1]=& _tmp1E;({struct Cyc___cycFILE*_tmp43=Cyc_stderr;struct _dyneither_ptr _tmp42=({const char*_tmp1C="%s: %s\n";_tag_dyneither(_tmp1C,sizeof(char),8U);});Cyc_fprintf(_tmp43,_tmp42,_tag_dyneither(_tmp1B,sizeof(void*),2U));});});
# 142
if(Cyc_Position_num_errors == Cyc_Position_max_errors)
({void*_tmp1F=0U;({struct Cyc___cycFILE*_tmp45=Cyc_stderr;struct _dyneither_ptr _tmp44=({const char*_tmp20="Too many error messages!\n";_tag_dyneither(_tmp20,sizeof(char),26U);});Cyc_fprintf(_tmp45,_tmp44,_tag_dyneither(_tmp1F,sizeof(void*),0U));});});
# 145
Cyc_fflush(Cyc_stderr);
++ Cyc_Position_num_errors;}
# 150
void Cyc_Position_reset_position(struct _dyneither_ptr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}
void Cyc_Position_set_position_file(struct _dyneither_ptr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}
struct _dyneither_ptr Cyc_Position_get_position_file(){return Cyc_Position_source;}
# 155
struct _dyneither_ptr Cyc_Position_get_line_directive(unsigned int s){
struct Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,(int)s);
if(pos_s != 0)
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp23=({struct Cyc_Int_pa_PrintArg_struct _tmp32;_tmp32.tag=1U,_tmp32.f1=(unsigned long)pos_s->line_no;_tmp32;});struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp31;_tmp31.tag=0U,_tmp31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos_s->logical_file);_tmp31;});void*_tmp21[2U];_tmp21[0]=& _tmp23,_tmp21[1]=& _tmp24;({struct _dyneither_ptr _tmp46=({const char*_tmp22="\n#line %d \"%s\"\n";_tag_dyneither(_tmp22,sizeof(char),16U);});Cyc_aprintf(_tmp46,_tag_dyneither(_tmp21,sizeof(void*),2U));});});else{
# 160
return _tag_dyneither(0,0,0);}}

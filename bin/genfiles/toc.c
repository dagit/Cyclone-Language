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

#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#define _check_known_subscript_nullX(ptr,bound,elt_sz,index) ({ \
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
void*Cyc_Warn_vimpos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 49
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 54
int Cyc_Tcutil_is_dyneither_ptr(void*t);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
# 59
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 69
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 77
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 82
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 84
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 90
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 105
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 108
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 112
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
# 115
void*Cyc_Tcutil_compress(void*t);
# 143
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 153
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 197
int Cyc_Tcutil_unify(void*,void*);
# 200
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 202
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 204
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 289 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 297
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 351
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 377
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 388
int Cyc_Tcutil_force_type2bool(int desired,void*t);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 36 "insert_checks.h"
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
# 60 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 75 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 77
({struct _dyneither_ptr _tmp756=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp755=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp755,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp756,ap);});}
# 79
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 81
({struct _dyneither_ptr _tmp758=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp757=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp757,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp758,ap);});}
# 84
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 87
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 97
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 99
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 125
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp2=*x;struct _tuple14 _tmp3=_tmp2;struct _tuple1*_tmp9;struct _tuple1*_tmp8;_LL1: _tmp9=_tmp3.f1;_tmp8=_tmp3.f2;_LL2:;{
struct _tuple14 _tmp4=*y;struct _tuple14 _tmp5=_tmp4;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL4: _tmp7=_tmp5.f1;_tmp6=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp9,_tmp7);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp8,_tmp6);};};}
# 134
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp11=_region_malloc(d,sizeof(*_tmp11));
({struct Cyc_List_List**_tmp764=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp764;}),({
struct Cyc_List_List**_tmp763=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp763;}),({
struct Cyc_Dict_Dict*_tmp762=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp761=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp761;});_tmpC;});_tmp11->aggrs_so_far=_tmp762;}),({
struct Cyc_List_List**_tmp760=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp760;}),({
struct Cyc_Set_Set**_tmp75F=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp75E=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp75E;});_tmpE;});_tmp11->datatypes_so_far=_tmp75F;}),({
struct Cyc_Dict_Dict*_tmp75D=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp75C=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp75C;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp75D;}),({
struct Cyc_Dict_Dict*_tmp75B=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp75A=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp75A;});_tmp10;});_tmp11->qvar_tags=_tmp75B;}),({
struct Cyc_Xarray_Xarray*_tmp759=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp759;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 153
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 159
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 162
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp765=({struct Cyc_Toc_TocStateWrap _tmp724;_tmp724.dyn=_tmp1C,_tmp724.state=_tmp1B;_tmp724;});*ts=_tmp765;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp1A=({struct _tuple15 _tmp725;_tmp725.f1=_tmp1B,_tmp725.f2=arg;_tmp725;});
({void*_tmp766=f(h,& _tmp1A);res=_tmp766;});}
# 170
;}
# 172
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)));
return res;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 176
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 179
struct _tuple17 _tmp1D=*env;struct _tuple17 _tmp1E=_tmp1D;struct Cyc_Toc_TocState*_tmp24;struct _tuple1*_tmp23;void*(*_tmp22)(struct _tuple1*);_LL1: _tmp24=_tmp1E.f1;_tmp23=(_tmp1E.f2)->f1;_tmp22=(_tmp1E.f2)->f2;_LL2:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp24->aggrs_so_far,_tmp23);
if(v == 0)
return _tmp22(_tmp23);else{
# 184
struct _tuple18*_tmp1F=*v;struct _tuple18*_tmp20=_tmp1F;void*_tmp21;_LL4: _tmp21=_tmp20->f2;_LL5:;
return _tmp21;}};}
# 189
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp726;_tmp726.f1=q,_tmp726.f2=type_maker;_tmp726;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp25[5U]="curr";
# 202 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp25,_tmp25,_tmp25 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp26[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp26,_tmp26,_tmp26 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp27[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp27,_tmp27,_tmp27 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp28[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp28,_tmp28,_tmp28 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp29[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp29,_tmp29,_tmp29 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2A[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2B[17U]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2B,_tmp2B,_tmp2B + 17U};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp2C[16U]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp2C,_tmp2C,_tmp2C + 16U};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;static char _tmp2D[7U]="_throw";
# 223 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp2D,_tmp2D,_tmp2D + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp2E[7U]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp2E,_tmp2E,_tmp2E + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp2F[14U]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp2F,_tmp2F,_tmp2F + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp30[13U]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp30,_tmp30,_tmp30 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp31[12U]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp31,_tmp31,_tmp31 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp32[14U]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp32,_tmp32,_tmp32 + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp33[12U]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp33,_tmp33,_tmp33 + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp34[28U]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp34,_tmp34,_tmp34 + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp35[31U]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp35,_tmp35,_tmp35 + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[32U]="_check_known_subscript_notnullX";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnullX_str={_tmp36,_tmp36,_tmp36 + 32U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnullX_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnullX_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnullX_bnd={0U,& Cyc_Toc__check_known_subscript_notnullX_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnullX_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnullX_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnullX_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnullX_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnullX_e=& Cyc_Toc__check_known_subscript_notnullX_ev;static char _tmp37[27U]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp37,_tmp37,_tmp37 + 27U};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0U,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1U,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp38[15U]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp38,_tmp38,_tmp38 + 15U};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0U,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1U,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp39[15U]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp39,_tmp39,_tmp39 + 15U};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0U,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1U,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp3A[21U]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp3A,_tmp3A,_tmp3A + 21U};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0U,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1U,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3B[20U]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3B,_tmp3B,_tmp3B + 20U};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0U,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1U,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3C[19U]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3C,_tmp3C,_tmp3C + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3D[24U]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3D,_tmp3D,_tmp3D + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp3E[25U]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp3E,_tmp3E,_tmp3E + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp3F[23U]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp3F,_tmp3F,_tmp3F + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp40[25U]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp40,_tmp40,_tmp40 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp41[26U]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp41,_tmp41,_tmp41 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp42[30U]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp42,_tmp42,_tmp42 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp43[28U]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp43,_tmp43,_tmp43 + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp44[20U]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp44,_tmp44,_tmp44 + 20U};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp45[15U]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp45,_tmp45,_tmp45 + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp46[20U]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp46,_tmp46,_tmp46 + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp47[21U]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp47,_tmp47,_tmp47 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp48[19U]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp48,_tmp48,_tmp48 + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp49[21U]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp49,_tmp49,_tmp49 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp4A[22U]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp4A,_tmp4A,_tmp4A + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp4B[26U]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp4B,_tmp4B,_tmp4B + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp4C[24U]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp4C,_tmp4C,_tmp4C + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp4D[28U]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp4D,_tmp4D,_tmp4D + 28U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp4E[23U]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp4E,_tmp4E,_tmp4E + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp4F[28U]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp4F,_tmp4F,_tmp4F + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp50[29U]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp50,_tmp50,_tmp50 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp51[27U]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp51,_tmp51,_tmp51 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp52[29U]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp52,_tmp52,_tmp52 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp53[30U]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp53,_tmp53,_tmp53 + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp54[34U]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp54,_tmp54,_tmp54 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp55[32U]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp55,_tmp55,_tmp55 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp56[33U]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp56,_tmp56,_tmp56 + 33U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp57[28U]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp57,_tmp57,_tmp57 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp58[33U]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp58,_tmp58,_tmp58 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp59[34U]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp59,_tmp59,_tmp59 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp5A[32U]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp5A,_tmp5A,_tmp5A + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp5B[34U]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp5B,_tmp5B,_tmp5B + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp5C[35U]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp5C,_tmp5C,_tmp5C + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp5D[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp5D,_tmp5D,_tmp5D + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp5E[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp5E,_tmp5E,_tmp5E + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp5F[10U]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp5F,_tmp5F,_tmp5F + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp60[11U]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp60,_tmp60,_tmp60 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp61[17U]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp61,_tmp61,_tmp61 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp62[18U]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp62,_tmp62,_tmp62 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp63[15U]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp63,_tmp63,_tmp63 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp64[15U]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp64,_tmp64,_tmp64 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp65[13U]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp65,_tmp65,_tmp65 + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp66[12U]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp66,_tmp66,_tmp66 + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp67[13U]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp67,_tmp67,_tmp67 + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp68[12U]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp68,_tmp68,_tmp68 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp69[19U]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp69,_tmp69,_tmp69 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp6A[29U]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp6A,_tmp6A,_tmp6A + 29U};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0U,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmp6B[13U]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmp6B,_tmp6B,_tmp6B + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp6C[9U]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmp6C,_tmp6C,_tmp6C + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp6D[20U]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp6D,_tmp6D,_tmp6D + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 292
void*Cyc_Toc_dyneither_ptr_type(){
static void*dpt=0;
void*_tmp6E=dpt;
if(_tmp6E == 0){
({void*_tmp767=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0),0);_tmp6E=_tmp767;});
dpt=_tmp6E;}
# 299
return _tmp6E;}
# 302
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 304
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 308
static struct _tuple19*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_List_List*_tmp769=({struct Cyc_List_List*_tmp70=_cycalloc(sizeof(*_tmp70));({void*_tmp768=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=1U,_tmp6F->f1=name;_tmp6F;});_tmp70->hd=_tmp768;}),_tmp70->tl=0;_tmp70;});_tmp71->f1=_tmp769;}),_tmp71->f2=e;_tmp71;});}
# 314
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp72=e->r;void*_tmp73=_tmp72;struct Cyc_Absyn_Exp*_tmp74;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp73)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp73)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp74=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp73)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp74);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 320
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=14U,_tmp75->f1=t,_tmp75->f2=e,_tmp75->f3=0,_tmp75->f4=Cyc_Absyn_No_coercion;_tmp75;});}
# 323
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=20U,_tmp76->f1=e;_tmp76;});}
# 326
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=23U,_tmp77->f1=e1,_tmp77->f2=e2;_tmp77;});}
# 329
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=37U,_tmp78->f1=s;_tmp78;});}
# 332
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=17U,_tmp79->f1=t;_tmp79;});}
# 335
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=10U,_tmp7A->f1=e,_tmp7A->f2=es,_tmp7A->f3=0,_tmp7A->f4=1;_tmp7A;});}
# 338
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=1U,_tmp7B->f1=e;_tmp7B;});}
# 341
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=2U,_tmp7C->f1=s1,_tmp7C->f2=s2;_tmp7C;});}
# 344
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=6U,_tmp7D->f1=e1,_tmp7D->f2=e2,_tmp7D->f3=e3;_tmp7D;});}
# 347
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=21U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});}
# 350
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=22U,_tmp7F->f1=e,_tmp7F->f2=n,_tmp7F->f3=0,_tmp7F->f4=0;_tmp7F;});}
# 353
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 356
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=36U,_tmp80->f1=tdopt,_tmp80->f2=ds;_tmp80;});}
# 358
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=8U,_tmp81->f1=v;_tmp81;});}
# 361
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 365
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp82=e->r;void*_tmp83=_tmp82;struct Cyc_Absyn_Exp*_tmp84;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp83)->tag == 20U){_LL1: _tmp84=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp83)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp84,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 384
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 393
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 402
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 411
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 422
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp85=Cyc_Tcutil_compress(t);void*_tmp86=_tmp85;enum Cyc_Absyn_Size_of _tmp8D;switch(*((int*)_tmp86)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f1)){case 1U: _LL1: _tmp8D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f1)->f2;_LL2:
# 426
{enum Cyc_Absyn_Size_of _tmp87=_tmp8D;switch(_tmp87){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp88=0U;({struct _dyneither_ptr _tmp76A=({const char*_tmp89="impossible IntCon (not char, short or int)";_tag_dyneither(_tmp89,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp76A,_tag_dyneither(_tmp88,sizeof(void*),0U));});});}_LLD:;}
# 432
goto _LL0;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f1)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}default: goto _LLB;}case 3U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8C=({struct Cyc_String_pa_PrintArg_struct _tmp727;_tmp727.tag=0U,({struct _dyneither_ptr _tmp76B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp727.f1=_tmp76B;});_tmp727;});void*_tmp8A[1U];_tmp8A[0]=& _tmp8C;({struct _dyneither_ptr _tmp76C=({const char*_tmp8B="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8B,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp76C,_tag_dyneither(_tmp8A,sizeof(void*),1U));});});}_LL0:;}
# 439
return function;}
# 441
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8E=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8F=_tmp8E;void*_tmp92;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F)->tag == 3U){_LL1: _tmp92=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp92);}else{_LL3: _LL4:
({void*_tmp90=0U;({struct _dyneither_ptr _tmp76D=({const char*_tmp91="impossible type (not pointer)";_tag_dyneither(_tmp91,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp76D,_tag_dyneither(_tmp90,sizeof(void*),0U));});});}_LL0:;}
# 451
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp93=e->r;void*_tmp94=_tmp93;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;long long _tmp9A;int _tmp99;short _tmp98;struct _dyneither_ptr _tmp97;char _tmp96;switch(*((int*)_tmp94)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Null_c).tag){case 2U: _LL1: _tmp96=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Char_c).val).f2;_LL2:
 return _tmp96 == '\000';case 3U: _LL3: _tmp97=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Wchar_c).val;_LL4: {
# 455
unsigned long _tmp95=Cyc_strlen((struct _dyneither_ptr)_tmp97);
int i=0;
if(_tmp95 >= 2  && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp97.curr)[1]== 'x'  && _tmp95 >= 3) && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp95;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 465
return 0;}}case 4U: _LL5: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Short_c).val).f2;_LL6:
 return _tmp98 == 0;case 5U: _LL7: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Int_c).val).f2;_LL8:
 return _tmp99 == 0;case 6U: _LL9: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).LongLong_c).val).f2;_LLA:
 return _tmp9A == 0;case 1U: _LLD: _LLE:
# 470
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 469
 goto _LLE;case 14U: _LLF: _tmp9B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL10:
# 471
 return Cyc_Toc_is_zero(_tmp9B);case 24U: _LL11: _tmp9C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9C);case 26U: _LL13: _tmp9D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL14:
 _tmp9E=_tmp9D;goto _LL16;case 29U: _LL15: _tmp9E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp94)->f3;_LL16:
 _tmp9F=_tmp9E;goto _LL18;case 25U: _LL17: _tmp9F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL18:
 _tmpA0=_tmp9F;goto _LL1A;case 36U: _LL19: _tmpA0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL1A:
# 477
 for(0;_tmpA0 != 0;_tmpA0=_tmpA0->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmpA0->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 486
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA1=x;union Cyc_Absyn_Nmspace _tmpB3;struct _dyneither_ptr*_tmpB2;_LL1: _tmpB3=_tmpA1->f1;_tmpB2=_tmpA1->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA2=_tmpB3;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;switch((_tmpA2.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAF=0;goto _LL7;case 1U: _LL6: _tmpAF=(_tmpA2.Rel_n).val;_LL7:
 _tmpB0=_tmpAF;goto _LL9;case 2U: _LL8: _tmpB0=(_tmpA2.Abs_n).val;_LL9:
 _tmpB1=_tmpB0;goto _LLB;default: _LLA: _tmpB1=(_tmpA2.C_n).val;_LLB:
# 495
 if(_tmpB1 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp729;_tmp729.tag=0U,_tmp729.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp729;});struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp728;_tmp728.tag=0U,_tmp728.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp728;});void*_tmpA3[2U];_tmpA3[0]=& _tmpA5,_tmpA3[1]=& _tmpA6;({struct _dyneither_ptr _tmp76E=({const char*_tmpA4="%s_%s_struct";_tag_dyneither(_tmpA4,sizeof(char),13U);});Cyc_aprintf(_tmp76E,_tag_dyneither(_tmpA3,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA7=_new_region("r");struct _RegionHandle*r=& _tmpA7;_push_region(r);
{struct _dyneither_ptr _tmpAE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp72C;_tmp72C.tag=0U,_tmp72C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp72C;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp72B;_tmp72B.tag=0U,({struct _dyneither_ptr _tmp771=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp770=r;struct Cyc_List_List*_tmp76F=_tmpB1;Cyc_rstr_sepstr(_tmp770,_tmp76F,({const char*_tmpAD="_";_tag_dyneither(_tmpAD,sizeof(char),2U);}));}));_tmp72B.f1=_tmp771;});_tmp72B;});struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp72A;_tmp72A.tag=0U,_tmp72A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp72A;});void*_tmpA8[3U];_tmpA8[0]=& _tmpAA,_tmpA8[1]=& _tmpAB,_tmpA8[2]=& _tmpAC;({struct _dyneither_ptr _tmp772=({const char*_tmpA9="%s_%s_%s_struct";_tag_dyneither(_tmpA9,sizeof(char),16U);});Cyc_aprintf(_tmp772,_tag_dyneither(_tmpA8,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAE;};_pop_region(r);};}_LL3:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 508
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 511
struct _tuple20 _tmpB4=*env;struct _tuple20 _tmpB5=_tmpB4;struct Cyc_Dict_Dict*_tmpC8;struct _tuple14*_tmpC7;_LL1: _tmpC8=(_tmpB5.f1)->qvar_tags;_tmpC7=_tmpB5.f2;_LL2:;{
struct _tuple14 _tmpB6=*_tmpC7;struct _tuple14 _tmpB7=_tmpB6;struct _tuple1*_tmpC6;struct _tuple1*_tmpC5;_LL4: _tmpC6=_tmpB7.f1;_tmpC5=_tmpB7.f2;_LL5:;{
struct _handler_cons _tmpB8;_push_handler(& _tmpB8);{int _tmpBA=0;if(setjmp(_tmpB8.handler))_tmpBA=1;if(!_tmpBA){{struct _tuple1*_tmpBB=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmpC8,Cyc_Toc_qvar_tag_cmp,_tmpC7);_npop_handler(0U);return _tmpBB;};_pop_handler();}else{void*_tmpB9=(void*)_exn_thrown;void*_tmpBC=_tmpB9;void*_tmpC4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBC)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 515
struct _tuple14*_tmpBD=({struct _tuple14*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=_tmpC6,_tmpC3->f2=_tmpC5;_tmpC3;});
struct _tuple1*_tmpBE=_tmpC6;union Cyc_Absyn_Nmspace _tmpC2;struct _dyneither_ptr*_tmpC1;_LLC: _tmpC2=_tmpBE->f1;_tmpC1=_tmpBE->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC1,_tmpC5);
struct _tuple1*res=({struct _tuple1*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->f1=_tmpC2,({struct _dyneither_ptr*_tmp773=({struct _dyneither_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));*_tmpBF=newvar;_tmpBF;});_tmpC0->f2=_tmp773;});_tmpC0;});
({struct Cyc_Dict_Dict _tmp774=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC8,_tmpBD,res);*_tmpC8=_tmp774;});
return res;};}}else{_LL9: _tmpC4=_tmpBC;_LLA:(int)_rethrow(_tmpC4);}_LL6:;}};};};}
# 524
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp72D;_tmp72D.f1=fieldname,_tmp72D.f2=dtname;_tmp72D;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 531
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->kind=Cyc_Absyn_StructA,_tmpCB->sc=Cyc_Absyn_Public,_tmpCB->tvs=0,_tmpCB->attributes=0,_tmpCB->expected_mem_kind=0,({
# 534
struct _tuple1*_tmp777=({struct _tuple1*_tmpC9=_cycalloc(sizeof(*_tmpC9));({union Cyc_Absyn_Nmspace _tmp776=Cyc_Absyn_Rel_n(0);_tmpC9->f1=_tmp776;}),_tmpC9->f2=name;_tmpC9;});_tmpCB->name=_tmp777;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp775=({struct Cyc_Absyn_AggrdeclImpl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->exist_vars=0,_tmpCA->rgn_po=0,_tmpCA->tagged=0,_tmpCA->fields=fs;_tmpCA;});_tmpCB->impl=_tmp775;});_tmpCB;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 543
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 546
struct _tuple21 _tmpCC=*env;struct _tuple21 _tmpCD=_tmpCC;struct Cyc_List_List**_tmpE3;struct Cyc_List_List*_tmpE2;_LL1: _tmpE3=(_tmpCD.f1)->tuple_types;_tmpE2=_tmpCD.f2;_LL2:;
# 548
{struct Cyc_List_List*_tmpCE=*_tmpE3;for(0;_tmpCE != 0;_tmpCE=_tmpCE->tl){
struct _tuple22*_tmpCF=(struct _tuple22*)_tmpCE->hd;struct _tuple22*_tmpD0=_tmpCF;void*_tmpD3;struct Cyc_List_List*_tmpD2;_LL4: _tmpD3=_tmpD0->f1;_tmpD2=_tmpD0->f2;_LL5:;{
struct Cyc_List_List*_tmpD1=_tmpE2;
for(0;_tmpD1 != 0  && _tmpD2 != 0;(_tmpD1=_tmpD1->tl,_tmpD2=_tmpD2->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD1->hd)).f2,(void*)_tmpD2->hd))
break;}
if(_tmpD1 == 0  && _tmpD2 == 0)
return _tmpD3;};}}{
# 559
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE1=_cycalloc(sizeof(*_tmpE1));({struct _dyneither_ptr _tmp779=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE0=({struct Cyc_Int_pa_PrintArg_struct _tmp72E;_tmp72E.tag=1U,_tmp72E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp72E;});void*_tmpDE[1U];_tmpDE[0]=& _tmpE0;({struct _dyneither_ptr _tmp778=({const char*_tmpDF="_tuple%d";_tag_dyneither(_tmpDF,sizeof(char),9U);});Cyc_aprintf(_tmp778,_tag_dyneither(_tmpDE,sizeof(void*),1U));});});*_tmpE1=_tmp779;});_tmpE1;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE2);
struct Cyc_List_List*_tmpD4=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp77C=({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct Cyc_Absyn_Aggrfield*_tmp77B=({struct Cyc_Absyn_Aggrfield*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct _dyneither_ptr*_tmp77A=Cyc_Absyn_fieldname(i);_tmpD5->name=_tmp77A;}),_tmpD5->tq=Cyc_Toc_mt_tq,_tmpD5->type=(void*)ts2->hd,_tmpD5->width=0,_tmpD5->attributes=0,_tmpD5->requires_clause=0;_tmpD5;});_tmpD6->hd=_tmp77B;}),_tmpD6->tl=_tmpD4;_tmpD6;});_tmpD4=_tmp77C;});}}
({struct Cyc_List_List*_tmp77D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD4);_tmpD4=_tmp77D;});{
struct Cyc_Absyn_Aggrdecl*_tmpD7=Cyc_Toc_make_c_struct_defn(xname,_tmpD4);
void*_tmpD8=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDD=_cycalloc(sizeof(*_tmpDD));*_tmpDD=_tmpD7;_tmpDD;})),0);
({struct Cyc_List_List*_tmp77F=({struct Cyc_List_List*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Absyn_Decl*_tmp77E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->tag=5U,_tmpD9->f1=_tmpD7;_tmpD9;}),0U);_tmpDA->hd=_tmp77E;}),_tmpDA->tl=Cyc_Toc_result_decls;_tmpDA;});Cyc_Toc_result_decls=_tmp77F;});
({struct Cyc_List_List*_tmp781=({struct Cyc_List_List*_tmpDC=_region_malloc(d,sizeof(*_tmpDC));({struct _tuple22*_tmp780=({struct _tuple22*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));_tmpDB->f1=_tmpD8,_tmpDB->f2=ts;_tmpDB;});_tmpDC->hd=_tmp780;}),_tmpDC->tl=*_tmpE3;_tmpDC;});*_tmpE3=_tmp781;});
return _tmpD8;};};}
# 572
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 577
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 580
struct _tuple24*_tmpE4=env;struct Cyc_List_List**_tmpF8;enum Cyc_Absyn_AggrKind _tmpF7;struct Cyc_List_List*_tmpF6;_LL1: _tmpF8=(_tmpE4->f1)->anon_aggr_types;_tmpF7=(_tmpE4->f2)->f1;_tmpF6=(_tmpE4->f2)->f2;_LL2:;
# 582
{struct Cyc_List_List*_tmpE5=*_tmpF8;for(0;_tmpE5 != 0;_tmpE5=_tmpE5->tl){
struct _tuple25*_tmpE6=(struct _tuple25*)_tmpE5->hd;struct _tuple25*_tmpE7=_tmpE6;void*_tmpEA;enum Cyc_Absyn_AggrKind _tmpE9;struct Cyc_List_List*_tmpE8;_LL4: _tmpEA=_tmpE7->f1;_tmpE9=_tmpE7->f2;_tmpE8=_tmpE7->f3;_LL5:;
if(_tmpF7 != _tmpE9)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE8,_tmpF6))
return _tmpEA;}}{
# 591
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF5=_cycalloc(sizeof(*_tmpF5));({struct _dyneither_ptr _tmp783=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF4=({struct Cyc_Int_pa_PrintArg_struct _tmp72F;_tmp72F.tag=1U,_tmp72F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp72F;});void*_tmpF2[1U];_tmpF2[0]=& _tmpF4;({struct _dyneither_ptr _tmp782=({const char*_tmpF3="_tuple%d";_tag_dyneither(_tmpF3,sizeof(char),9U);});Cyc_aprintf(_tmp782,_tag_dyneither(_tmpF2,sizeof(void*),1U));});});*_tmpF5=_tmp783;});_tmpF5;});
struct Cyc_Absyn_Aggrdecl*_tmpEB=Cyc_Toc_make_c_struct_defn(xname,_tmpF6);
_tmpEB->kind=_tmpF7;{
void*_tmpEC=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF1=_cycalloc(sizeof(*_tmpF1));*_tmpF1=_tmpEB;_tmpF1;})),0);
({struct Cyc_List_List*_tmp785=({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));({struct Cyc_Absyn_Decl*_tmp784=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->tag=5U,_tmpED->f1=_tmpEB;_tmpED;}),0U);_tmpEE->hd=_tmp784;}),_tmpEE->tl=Cyc_Toc_result_decls;_tmpEE;});Cyc_Toc_result_decls=_tmp785;});
({struct Cyc_List_List*_tmp787=({struct Cyc_List_List*_tmpF0=_region_malloc(d,sizeof(*_tmpF0));({struct _tuple25*_tmp786=({struct _tuple25*_tmpEF=_region_malloc(d,sizeof(*_tmpEF));_tmpEF->f1=_tmpEC,_tmpEF->f2=_tmpF7,_tmpEF->f3=_tmpF6;_tmpEF;});_tmpF0->hd=_tmp786;}),_tmpF0->tl=*_tmpF8;_tmpF0;});*_tmpF8=_tmp787;});
return _tmpEC;};};}
# 599
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(({struct _tuple23*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->f1=ak,_tmpF9->f2=fs;_tmpF9;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple26{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple27{struct Cyc_Toc_TocState*f1;struct _tuple26*f2;};struct _tuple28{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 608
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple27*env){
# 617
struct _tuple27 _tmpFA=*env;struct _tuple27 _tmpFB=_tmpFA;struct Cyc_List_List**_tmp11E;struct _tuple1*_tmp11D;struct Cyc_List_List*_tmp11C;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11A;_LL1: _tmp11E=(_tmpFB.f1)->abs_struct_types;_tmp11D=(_tmpFB.f2)->f1;_tmp11C=(_tmpFB.f2)->f2;_tmp11B=(_tmpFB.f2)->f3;_tmp11A=(_tmpFB.f2)->f4;_LL2:;
# 621
{struct Cyc_List_List*_tmpFC=*_tmp11E;for(0;_tmpFC != 0;_tmpFC=_tmpFC->tl){
struct _tuple28*_tmpFD=(struct _tuple28*)_tmpFC->hd;struct _tuple28*_tmpFE=_tmpFD;struct _tuple1*_tmp106;struct Cyc_List_List*_tmp105;void*_tmp104;_LL4: _tmp106=_tmpFE->f1;_tmp105=_tmpFE->f2;_tmp104=_tmpFE->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp106,_tmp11D)== 0  && ({
int _tmp788=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11B);_tmp788 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp105);})){
int okay=1;
{struct Cyc_List_List*_tmpFF=_tmp11B;for(0;_tmpFF != 0;(_tmpFF=_tmpFF->tl,_tmp105=_tmp105->tl)){
void*_tmp100=(void*)_tmpFF->hd;
void*_tmp101=(void*)((struct Cyc_List_List*)_check_null(_tmp105))->hd;
{struct Cyc_Absyn_Kind*_tmp102=Cyc_Tcutil_type_kind(_tmp100);struct Cyc_Absyn_Kind*_tmp103=_tmp102;switch(((struct Cyc_Absyn_Kind*)_tmp103)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 632
 continue;default: _LLB: _LLC:
# 635
 if(Cyc_Tcutil_unify(_tmp100,_tmp101) || ({void*_tmp789=Cyc_Toc_typ_to_c(_tmp100);Cyc_Tcutil_unify(_tmp789,Cyc_Toc_typ_to_c(_tmp101));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 640
break;}}
# 642
if(okay)
return _tmp104;}}}{
# 649
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp119=_cycalloc(sizeof(*_tmp119));({struct _dyneither_ptr _tmp78B=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp118=({struct Cyc_Int_pa_PrintArg_struct _tmp730;_tmp730.tag=1U,_tmp730.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp730;});void*_tmp116[1U];_tmp116[0]=& _tmp118;({struct _dyneither_ptr _tmp78A=({const char*_tmp117="_tuple%d";_tag_dyneither(_tmp117,sizeof(char),9U);});Cyc_aprintf(_tmp78A,_tag_dyneither(_tmp116,sizeof(void*),1U));});});*_tmp119=_tmp78B;});_tmp119;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp107=0;
# 653
({struct Cyc_List_List*_tmp78D=({struct Cyc_List_List*_tmp109=_region_malloc(d,sizeof(*_tmp109));({struct _tuple28*_tmp78C=({struct _tuple28*_tmp108=_region_malloc(d,sizeof(*_tmp108));_tmp108->f1=_tmp11D,_tmp108->f2=_tmp11B,_tmp108->f3=x;_tmp108;});_tmp109->hd=_tmp78C;}),_tmp109->tl=*_tmp11E;_tmp109;});*_tmp11E=_tmp78D;});{
# 656
struct _RegionHandle _tmp10A=_new_region("r");struct _RegionHandle*r=& _tmp10A;_push_region(r);
{struct Cyc_List_List*_tmp10B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11C,_tmp11B);
for(0;_tmp11A != 0;_tmp11A=_tmp11A->tl){
struct Cyc_Absyn_Aggrfield*_tmp10C=(struct Cyc_Absyn_Aggrfield*)_tmp11A->hd;
void*t=_tmp10C->type;
struct Cyc_List_List*atts=_tmp10C->attributes;
# 666
if((_tmp11A->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t))) && !
Cyc_Tcutil_is_array_type(t))
({struct Cyc_List_List*_tmp78F=({struct Cyc_List_List*_tmp10E=_cycalloc(sizeof(*_tmp10E));({void*_tmp78E=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->tag=6U,_tmp10D->f1=0;_tmp10D;});_tmp10E->hd=_tmp78E;}),_tmp10E->tl=atts;_tmp10E;});atts=_tmp78F;});
({void*_tmp790=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10B,t));t=_tmp790;});
# 673
if(Cyc_Tcutil_unify(t,Cyc_Absyn_void_type))
({void*_tmp794=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=4U,({void*_tmp793=Cyc_Absyn_void_star_type();(_tmp10F->f1).elt_type=_tmp793;}),({struct Cyc_Absyn_Tqual _tmp792=Cyc_Absyn_empty_tqual(0U);(_tmp10F->f1).tq=_tmp792;}),({
struct Cyc_Absyn_Exp*_tmp791=Cyc_Absyn_uint_exp(0U,0U);(_tmp10F->f1).num_elts=_tmp791;}),(_tmp10F->f1).zero_term=Cyc_Absyn_false_type,(_tmp10F->f1).zt_loc=0U;_tmp10F;});
# 674
t=_tmp794;});
# 677
({struct Cyc_List_List*_tmp796=({struct Cyc_List_List*_tmp111=_cycalloc(sizeof(*_tmp111));({struct Cyc_Absyn_Aggrfield*_tmp795=({struct Cyc_Absyn_Aggrfield*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->name=_tmp10C->name,_tmp110->tq=Cyc_Toc_mt_tq,_tmp110->type=t,_tmp110->width=_tmp10C->width,_tmp110->attributes=atts,_tmp110->requires_clause=0;_tmp110;});_tmp111->hd=_tmp795;}),_tmp111->tl=_tmp107;_tmp111;});_tmp107=_tmp796;});}
# 679
({struct Cyc_List_List*_tmp797=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp107);_tmp107=_tmp797;});{
struct Cyc_Absyn_Aggrdecl*_tmp112=Cyc_Toc_make_c_struct_defn(xname,_tmp107);
({struct Cyc_List_List*_tmp799=({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));({struct Cyc_Absyn_Decl*_tmp798=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=5U,_tmp113->f1=_tmp112;_tmp113;}),0U);_tmp114->hd=_tmp798;}),_tmp114->tl=Cyc_Toc_result_decls;_tmp114;});Cyc_Toc_result_decls=_tmp799;});{
void*_tmp115=x;_npop_handler(0U);return _tmp115;};};}
# 657
;_pop_region(r);};};}
# 685
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 689
struct _tuple26 env=({struct _tuple26 _tmp731;_tmp731.f1=struct_name,_tmp731.f2=type_vars,_tmp731.f3=type_args,_tmp731.f4=fields;_tmp731;});
return((void*(*)(struct _tuple26*arg,void*(*f)(struct _RegionHandle*,struct _tuple27*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 697
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp79C=({struct _dyneither_ptr*_tmp122=_cycalloc(sizeof(*_tmp122));({struct _dyneither_ptr _tmp79B=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp121=({struct Cyc_Int_pa_PrintArg_struct _tmp732;_tmp732.tag=1U,_tmp732.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp732;});void*_tmp11F[1U];_tmp11F[0]=& _tmp121;({struct _dyneither_ptr _tmp79A=({const char*_tmp120="_tmp%X";_tag_dyneither(_tmp120,sizeof(char),7U);});Cyc_aprintf(_tmp79A,_tag_dyneither(_tmp11F,sizeof(void*),1U));});});*_tmp122=_tmp79B;});_tmp122;});_tmp123->f2=_tmp79C;});_tmp123;});}struct _tuple29{struct Cyc_Toc_TocState*f1;int f2;};
# 703
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple29*env){
struct _tuple29 _tmp124=*env;struct _tuple29 _tmp125=_tmp124;struct Cyc_Xarray_Xarray*_tmp12D;_LL1: _tmp12D=(_tmp125.f1)->temp_labels;_LL2:;{
int _tmp126=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp79D=_tmp126;_tmp79D < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12D);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12D,_tmp126);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12C=_cycalloc(sizeof(*_tmp12C));({struct _dyneither_ptr _tmp79F=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12B=({struct Cyc_Int_pa_PrintArg_struct _tmp733;_tmp733.tag=1U,_tmp733.f1=(unsigned int)_tmp126;_tmp733;});void*_tmp129[1U];_tmp129[0]=& _tmp12B;({struct _dyneither_ptr _tmp79E=({const char*_tmp12A="_LL%X";_tag_dyneither(_tmp12A,sizeof(char),6U);});Cyc_aprintf(_tmp79E,_tag_dyneither(_tmp129,sizeof(void*),1U));});});*_tmp12C=_tmp79F;});_tmp12C;});
if(({int _tmp7A0=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12D,res);_tmp7A0 != _tmp126;}))
({void*_tmp127=0U;({struct _dyneither_ptr _tmp7A1=({const char*_tmp128="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp128,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7A1,_tag_dyneither(_tmp127,sizeof(void*),0U));});});
return res;};};}
# 714
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple29*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 720
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp12E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp12E))->hd)->name)!= 0){
++ ans;
_tmp12E=_tmp12E->tl;}
# 727
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 733
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp12F=*a;struct _tuple9 _tmp130=_tmp12F;struct _dyneither_ptr*_tmp134;struct Cyc_Absyn_Tqual _tmp133;void*_tmp132;_LL1: _tmp134=_tmp130.f1;_tmp133=_tmp130.f2;_tmp132=_tmp130.f3;_LL2:;
return({struct _tuple9*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->f1=_tmp134,_tmp131->f2=_tmp133,({void*_tmp7A2=Cyc_Toc_typ_to_c(_tmp132);_tmp131->f3=_tmp7A2;});_tmp131;});}
# 756 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp135=Cyc_Tcutil_compress(t);void*_tmp136=_tmp135;void*_tmp13C;void*_tmp13B;struct Cyc_Absyn_Tqual _tmp13A;struct Cyc_Absyn_Exp*_tmp139;void*_tmp138;unsigned int _tmp137;switch(*((int*)_tmp136)){case 4U: _LL1: _tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp136)->f1).elt_type;_tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp136)->f1).tq;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp136)->f1).num_elts;_tmp138=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp136)->f1).zero_term;_tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp136)->f1).zt_loc;_LL2:
# 759
 return({void*_tmp7A3=Cyc_Toc_typ_to_c_array(_tmp13B);Cyc_Absyn_cstar_type(_tmp7A3,_tmp13A);});case 1U: _LL3: _tmp13C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp136)->f2;if(_tmp13C != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13C);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 765
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 767
struct Cyc_Absyn_Aggrfield*_tmp13D=({struct Cyc_Absyn_Aggrfield*_tmp13E=_cycalloc(sizeof(*_tmp13E));*_tmp13E=*f;_tmp13E;});
_tmp13D->type=new_type;
_tmp13D->requires_clause=0;
_tmp13D->tq=Cyc_Toc_mt_tq;
return _tmp13D;}
# 774
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 776
return;}
# 779
static void*Cyc_Toc_char_star_type(){
static void**cs=0;
if(cs == 0)
({void**_tmp7A5=({void**_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp7A4=Cyc_Absyn_star_type(Cyc_Absyn_char_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type);*_tmp13F=_tmp7A4;});_tmp13F;});cs=_tmp7A5;});
return*cs;}
# 785
static void*Cyc_Toc_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp7A8=({void**_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp7A7=({void*_tmp7A6=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp7A6,Cyc_Toc_mt_tq);});*_tmp140=_tmp7A7;});_tmp140;});r=_tmp7A8;});
return*r;}
# 791
static void*Cyc_Toc_dyn_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp7AB=({void**_tmp141=_cycalloc(sizeof(*_tmp141));({void*_tmp7AA=({void*_tmp7A9=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_type(_tmp7A9,Cyc_Toc_mt_tq);});*_tmp141=_tmp7AA;});_tmp141;});r=_tmp7AB;});
return*r;}
# 797
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp142=Cyc_Tcutil_compress(t);void*_tmp143=_tmp142;struct Cyc_Absyn_Tvar*_tmp144;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->tag == 2U){_LL1: _tmp144=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 803
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp145=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp146=_tmp145;if(((struct Cyc_Absyn_Kind*)_tmp146)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 810
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp147=t;struct Cyc_Absyn_Datatypedecl*_tmp18E;void**_tmp18D;struct Cyc_Absyn_Enumdecl*_tmp18C;struct Cyc_Absyn_Aggrdecl*_tmp18B;struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_Absyn_Exp*_tmp189;struct _tuple1*_tmp188;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_Typedefdecl*_tmp186;void*_tmp185;enum Cyc_Absyn_AggrKind _tmp184;struct Cyc_List_List*_tmp183;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Tqual _tmp181;void*_tmp180;struct Cyc_List_List*_tmp17F;int _tmp17E;struct Cyc_Absyn_VarargInfo*_tmp17D;struct Cyc_List_List*_tmp17C;void*_tmp17B;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_Absyn_Exp*_tmp179;unsigned int _tmp178;void*_tmp177;struct Cyc_Absyn_Tqual _tmp176;void*_tmp175;struct Cyc_Absyn_Tvar*_tmp174;void**_tmp173;struct Cyc_List_List*_tmp172;struct _tuple1*_tmp171;union Cyc_Absyn_AggrInfo _tmp170;struct Cyc_List_List*_tmp16F;struct Cyc_Absyn_Datatypedecl*_tmp16E;struct Cyc_Absyn_Datatypefield*_tmp16D;switch(*((int*)_tmp147)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 825
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp16E=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1).KnownDatatypefield).val).f1;_tmp16D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1).KnownDatatypefield).val).f2;_LLA:
# 827
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp16D->name,_tmp16E->name));}else{_LLB: _LLC:
# 829
({void*_tmp148=0U;({struct _dyneither_ptr _tmp7AC=({const char*_tmp149="unresolved DatatypeFieldType";_tag_dyneither(_tmp149,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7AC,_tag_dyneither(_tmp148,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 838
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp170=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1;_tmp16F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f2;_LL1C:
# 885
{union Cyc_Absyn_AggrInfo _tmp159=_tmp170;if((_tmp159.UnknownAggr).tag == 1){_LL45: _LL46:
 return t;}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 889
struct Cyc_Absyn_Aggrdecl*_tmp15A=Cyc_Absyn_get_known_aggrdecl(_tmp170);
if(_tmp15A->expected_mem_kind){
# 892
if(_tmp15A->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp735;_tmp735.tag=0U,({
struct _dyneither_ptr _tmp7AD=(struct _dyneither_ptr)(_tmp15A->kind == Cyc_Absyn_UnionA?({const char*_tmp15F="union";_tag_dyneither(_tmp15F,sizeof(char),6U);}):({const char*_tmp160="struct";_tag_dyneither(_tmp160,sizeof(char),7U);}));_tmp735.f1=_tmp7AD;});_tmp735;});struct Cyc_String_pa_PrintArg_struct _tmp15E=({struct Cyc_String_pa_PrintArg_struct _tmp734;_tmp734.tag=0U,({
struct _dyneither_ptr _tmp7AE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15A->name));_tmp734.f1=_tmp7AE;});_tmp734;});void*_tmp15B[2U];_tmp15B[0]=& _tmp15D,_tmp15B[1]=& _tmp15E;({struct _dyneither_ptr _tmp7AF=({const char*_tmp15C="%s %s was never defined.";_tag_dyneither(_tmp15C,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp7AF,_tag_dyneither(_tmp15B,sizeof(void*),2U));});});}
# 900
if(_tmp15A->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp15A->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp161=_tmp15A->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15A->impl))->fields;
if(_tmp161 == 0)return Cyc_Toc_aggrdecl_type(_tmp15A->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp161))->tl != 0;_tmp161=_tmp161->tl){;}{
void*_tmp162=((struct Cyc_Absyn_Aggrfield*)_tmp161->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp162))){
if(_tmp15A->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp165=({struct Cyc_String_pa_PrintArg_struct _tmp736;_tmp736.tag=0U,({struct _dyneither_ptr _tmp7B0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15A->name));_tmp736.f1=_tmp7B0;});_tmp736;});void*_tmp163[1U];_tmp163[0]=& _tmp165;({struct _dyneither_ptr _tmp7B1=({const char*_tmp164="struct %s ended up being abstract.";_tag_dyneither(_tmp164,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp7B1,_tag_dyneither(_tmp163,sizeof(void*),1U));});});{
# 913
struct _RegionHandle _tmp166=_new_region("r");struct _RegionHandle*r=& _tmp166;_push_region(r);
{struct Cyc_List_List*_tmp167=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15A->tvs,_tmp16F);
void*_tmp168=Cyc_Tcutil_rsubstitute(r,_tmp167,_tmp162);
if(Cyc_Toc_is_abstract_type(_tmp168)){void*_tmp169=Cyc_Toc_aggrdecl_type(_tmp15A->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp169;}{
void*_tmp16A=Cyc_Toc_add_struct_type(_tmp15A->name,_tmp15A->tvs,_tmp16F,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15A->impl))->fields);_npop_handler(0U);return _tmp16A;};}
# 914
;_pop_region(r);};}
# 919
return Cyc_Toc_aggrdecl_type(_tmp15A->name,Cyc_Absyn_strctq);};};};case 15U: _LL1D: _tmp171=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1;_LL1E:
 return t;case 16U: _LL1F: _tmp172=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp147)->f1)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp172);return t;case 4U: _LL23: _LL24:
# 931
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 936
 return Cyc_Absyn_void_star_type();}case 1U: _LL3: _tmp173=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp147)->f2;_LL4:
# 814
 if(*_tmp173 == 0){
*_tmp173=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 818
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp173));case 2U: _LL5: _tmp174=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp147)->f1;_LL6:
# 820
 if((Cyc_Tcutil_tvar_kind(_tmp174,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 822
return Cyc_Absyn_void_type;else{
# 824
return Cyc_Absyn_void_star_type();}case 3U: _LLD: _tmp177=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp147)->f1).elt_type;_tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp147)->f1).elt_tq;_tmp175=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp147)->f1).ptr_atts).bounds;_LLE:
# 833
({void*_tmp7B2=Cyc_Toc_typ_to_c(_tmp177);_tmp177=_tmp7B2;});
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp175)== 0)
return Cyc_Toc_dyneither_ptr_type();else{
# 837
return Cyc_Absyn_star_type(_tmp177,Cyc_Absyn_heap_rgn_type,_tmp176,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp17B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp147)->f1).elt_type;_tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp147)->f1).tq;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp147)->f1).num_elts;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp147)->f1).zt_loc;_LL14:
# 841
 return({void*_tmp7B6=Cyc_Toc_typ_to_c(_tmp17B);struct Cyc_Absyn_Tqual _tmp7B5=_tmp17A;struct Cyc_Absyn_Exp*_tmp7B4=_tmp179;void*_tmp7B3=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp7B6,_tmp7B5,_tmp7B4,_tmp7B3,_tmp178);});case 5U: _LL15: _tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp147)->f1).ret_tqual;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp147)->f1).ret_type;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp147)->f1).args;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp147)->f1).c_varargs;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp147)->f1).cyc_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp147)->f1).attributes;_LL16: {
# 847
struct Cyc_List_List*_tmp14A=0;
for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
void*_tmp14B=(void*)_tmp17C->hd;void*_tmp14C=_tmp14B;switch(*((int*)_tmp14C)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
({struct Cyc_List_List*_tmp7B7=({struct Cyc_List_List*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->hd=(void*)_tmp17C->hd,_tmp14D->tl=_tmp14A;_tmp14D;});_tmp14A=_tmp7B7;});goto _LL35;}_LL35:;}{
# 858
struct Cyc_List_List*_tmp14E=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17F);
if(_tmp17D != 0){
# 861
void*_tmp14F=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp17D->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp150=({struct _tuple9*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->f1=_tmp17D->name,_tmp152->f2=_tmp17D->tq,_tmp152->f3=_tmp14F;_tmp152;});
({struct Cyc_List_List*_tmp7B9=({struct Cyc_List_List*_tmp7B8=_tmp14E;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp7B8,({struct Cyc_List_List*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->hd=_tmp150,_tmp151->tl=0;_tmp151;}));});_tmp14E=_tmp7B9;});}
# 865
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->tag=5U,(_tmp153->f1).tvars=0,(_tmp153->f1).effect=0,(_tmp153->f1).ret_tqual=_tmp181,({void*_tmp7BA=Cyc_Toc_typ_to_c(_tmp180);(_tmp153->f1).ret_type=_tmp7BA;}),(_tmp153->f1).args=_tmp14E,(_tmp153->f1).c_varargs=_tmp17E,(_tmp153->f1).cyc_varargs=0,(_tmp153->f1).rgn_po=0,(_tmp153->f1).attributes=_tmp14A,(_tmp153->f1).requires_clause=0,(_tmp153->f1).requires_relns=0,(_tmp153->f1).ensures_clause=0,(_tmp153->f1).ensures_relns=0;_tmp153;});};}case 6U: _LL17: _tmp182=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp147)->f1;_LL18: {
# 870
struct Cyc_List_List*_tmp154=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp7BD=({struct Cyc_List_List*_tmp156=_cycalloc(sizeof(*_tmp156));({struct _tuple12*_tmp7BC=({struct _tuple12*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->f1=(*((struct _tuple12*)_tmp182->hd)).f1,({void*_tmp7BB=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp182->hd)).f2);_tmp155->f2=_tmp7BB;});_tmp155;});_tmp156->hd=_tmp7BC;}),_tmp156->tl=_tmp154;_tmp156;});_tmp154=_tmp7BD;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp154));}case 7U: _LL19: _tmp184=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp147)->f1;_tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp147)->f2;_LL1A: {
# 879
struct Cyc_List_List*_tmp157=0;
for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
({struct Cyc_List_List*_tmp7C0=({struct Cyc_List_List*_tmp158=_cycalloc(sizeof(*_tmp158));({struct Cyc_Absyn_Aggrfield*_tmp7BF=({struct Cyc_Absyn_Aggrfield*_tmp7BE=(struct Cyc_Absyn_Aggrfield*)_tmp183->hd;Cyc_Toc_aggrfield_to_c(_tmp7BE,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp183->hd)->type));});_tmp158->hd=_tmp7BF;}),_tmp158->tl=_tmp157;_tmp158;});_tmp157=_tmp7C0;});}
return({enum Cyc_Absyn_AggrKind _tmp7C1=_tmp184;Cyc_Toc_add_anon_aggr_type(_tmp7C1,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp157));});}case 8U: _LL21: _tmp188=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp147)->f1;_tmp187=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp147)->f2;_tmp186=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp147)->f3;_tmp185=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp147)->f4;_LL22:
# 925
 if(_tmp185 == 0){
if(_tmp187 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=8U,_tmp16B->f1=_tmp188,_tmp16B->f2=0,_tmp16B->f3=_tmp186,_tmp16B->f4=0;_tmp16B;});else{
return t;}}else{
# 930
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->tag=8U,_tmp16C->f1=_tmp188,_tmp16C->f2=0,_tmp16C->f3=_tmp186,({void*_tmp7C2=Cyc_Toc_typ_to_c(_tmp185);_tmp16C->f4=_tmp7C2;});_tmp16C;});}case 9U: _LL2B: _tmp189=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp147)->f1;_LL2C:
# 941
 return t;case 11U: _LL2D: _tmp18A=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp147)->f1;_LL2E:
# 946
 if(_tmp18A->topt != 0)return Cyc_Toc_typ_to_c((void*)_check_null(_tmp18A->topt));else{
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp147)->f1)->r)){case 0U: _LL2F: _tmp18B=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp147)->f1)->r)->f1;_LL30:
# 949
 Cyc_Toc_aggrdecl_to_c(_tmp18B);
if(_tmp18B->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18B->name,Cyc_Absyn_unionq_type);else{
return Cyc_Toc_aggrdecl_type(_tmp18B->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp18C=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp147)->f1)->r)->f1;_LL32:
# 954
 Cyc_Toc_enumdecl_to_c(_tmp18C);
return t;default: _LL33: _tmp18E=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp147)->f1)->r)->f1;_tmp18D=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp147)->f2;_LL34:
# 957
 Cyc_Toc_datatypedecl_to_c(_tmp18E);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp18D)));}}_LL0:;}
# 962
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp18F=t;void*_tmp191;struct Cyc_Absyn_Tqual _tmp190;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18F)->tag == 4U){_LL1: _tmp191=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18F)->f1).elt_type;_tmp190=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18F)->f1).tq;_LL2:
# 965
 return({void*_tmp7C3=Cyc_Absyn_star_type(_tmp191,Cyc_Absyn_heap_rgn_type,_tmp190,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp7C3,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 972
static int Cyc_Toc_atomic_type(void*t){
void*_tmp192=Cyc_Tcutil_compress(t);void*_tmp193=_tmp192;struct Cyc_List_List*_tmp1A6;struct Cyc_List_List*_tmp1A5;void*_tmp1A4;void*_tmp1A3;struct Cyc_List_List*_tmp1A2;switch(*((int*)_tmp193)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp1A3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f1;_tmp1A2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2;_LL4: {
# 976
void*_tmp194=_tmp1A3;struct Cyc_Absyn_Datatypedecl*_tmp19E;struct Cyc_Absyn_Datatypefield*_tmp19D;union Cyc_Absyn_AggrInfo _tmp19C;switch(*((int*)_tmp194)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D:
 return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp19C=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp194)->f1;_LL25:
# 984
{union Cyc_Absyn_AggrInfo _tmp195=_tmp19C;if((_tmp195.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 988
struct Cyc_Absyn_Aggrdecl*_tmp196=Cyc_Absyn_get_known_aggrdecl(_tmp19C);
if(_tmp196->impl == 0)
return 0;
{struct Cyc_List_List*_tmp197=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp196->impl))->fields;for(0;_tmp197 != 0;_tmp197=_tmp197->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp197->hd)->type))return 0;}}
return 1;};case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp194)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp19E=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp194)->f1).KnownDatatypefield).val).f1;_tmp19D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp194)->f1).KnownDatatypefield).val).f2;_LL27:
# 995
{struct Cyc_List_List*_tmp198=_tmp19D->typs;for(0;_tmp198 != 0;_tmp198=_tmp198->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp198->hd)).f2))return 0;}}
return 1;}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp19B=({struct Cyc_String_pa_PrintArg_struct _tmp737;_tmp737.tag=0U,({struct _dyneither_ptr _tmp7C4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp737.f1=_tmp7C4;});_tmp737;});void*_tmp199[1U];_tmp199[0]=& _tmp19B;({struct _dyneither_ptr _tmp7C5=({const char*_tmp19A="atomic_typ:  bad type %s";_tag_dyneither(_tmp19A,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C5,_tag_dyneither(_tmp199,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 1000
 return 1;case 4U: _LL7: _tmp1A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).elt_type;_LL8:
 return Cyc_Toc_atomic_type(_tmp1A4);case 7U: _LL9: _tmp1A5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp193)->f2;_LLA:
# 1008
 for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp1A5->hd)->type))return 0;}
return 1;case 6U: _LLB: _tmp1A6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp193)->f1;_LLC:
# 1012
 for(0;_tmp1A6 != 0;_tmp1A6=_tmp1A6->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp1A6->hd)).f2))return 0;}
return 1;case 3U: _LLD: _LLE:
# 1017
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp1A1=({struct Cyc_String_pa_PrintArg_struct _tmp738;_tmp738.tag=0U,({struct _dyneither_ptr _tmp7C6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp738.f1=_tmp7C6;});_tmp738;});void*_tmp19F[1U];_tmp19F[0]=& _tmp1A1;({struct _dyneither_ptr _tmp7C7=({const char*_tmp1A0="atomic_typ:  bad type %s";_tag_dyneither(_tmp1A0,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C7,_tag_dyneither(_tmp19F,sizeof(void*),1U));});});}_LL0:;}
# 1022
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A7=Cyc_Tcutil_compress(t);void*_tmp1A8=_tmp1A7;void*_tmp1A9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->tag == 3U){_LL1: _tmp1A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_void_type(_tmp1A9);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1029
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1033
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1038
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1AA=Cyc_Tcutil_compress(t);void*_tmp1AB=_tmp1AA;struct Cyc_List_List*_tmp1B7;union Cyc_Absyn_AggrInfo _tmp1B6;switch(*((int*)_tmp1AB)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AB)->f1)->tag == 20U){_LL1: _tmp1B6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AB)->f1)->f1;_LL2: {
# 1041
struct Cyc_Absyn_Aggrdecl*_tmp1AC=Cyc_Absyn_get_known_aggrdecl(_tmp1B6);
if(_tmp1AC->impl == 0)
({void*_tmp1AD=0U;({struct _dyneither_ptr _tmp7C8=({const char*_tmp1AE="is_poly_field: type missing fields";_tag_dyneither(_tmp1AE,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C8,_tag_dyneither(_tmp1AD,sizeof(void*),0U));});});
_tmp1B7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AC->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1B7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AB)->f2;_LL4: {
# 1046
struct Cyc_Absyn_Aggrfield*_tmp1AF=Cyc_Absyn_lookup_field(_tmp1B7,f);
if(_tmp1AF == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B2=({struct Cyc_String_pa_PrintArg_struct _tmp739;_tmp739.tag=0U,_tmp739.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp739;});void*_tmp1B0[1U];_tmp1B0[0]=& _tmp1B2;({struct _dyneither_ptr _tmp7C9=({const char*_tmp1B1="is_poly_field: bad field %s";_tag_dyneither(_tmp1B1,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C9,_tag_dyneither(_tmp1B0,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AF->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B5=({struct Cyc_String_pa_PrintArg_struct _tmp73A;_tmp73A.tag=0U,({struct _dyneither_ptr _tmp7CA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp73A.f1=_tmp7CA;});_tmp73A;});void*_tmp1B3[1U];_tmp1B3[0]=& _tmp1B5;({struct _dyneither_ptr _tmp7CB=({const char*_tmp1B4="is_poly_field: bad type %s";_tag_dyneither(_tmp1B4,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7CB,_tag_dyneither(_tmp1B3,sizeof(void*),1U));});});}_LL0:;}
# 1057
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B8=e->r;void*_tmp1B9=_tmp1B8;struct Cyc_Absyn_Exp*_tmp1C3;struct _dyneither_ptr*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C1;struct _dyneither_ptr*_tmp1C0;switch(*((int*)_tmp1B9)){case 21U: _LL1: _tmp1C1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B9)->f1;_tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B9)->f2;_LL2:
# 1060
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C1->topt),_tmp1C0) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B9)->f1;_tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B9)->f2;_LL4: {
# 1063
void*_tmp1BA=Cyc_Tcutil_compress((void*)_check_null(_tmp1C3->topt));void*_tmp1BB=_tmp1BA;void*_tmp1BF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BB)->tag == 3U){_LL8: _tmp1BF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BB)->f1).elt_type;_LL9:
# 1065
 return Cyc_Toc_is_poly_field(_tmp1BF,_tmp1C2) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BE=({struct Cyc_String_pa_PrintArg_struct _tmp73B;_tmp73B.tag=0U,({struct _dyneither_ptr _tmp7CC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C3->topt)));_tmp73B.f1=_tmp7CC;});_tmp73B;});void*_tmp1BC[1U];_tmp1BC[0]=& _tmp1BE;({struct _dyneither_ptr _tmp7CD=({const char*_tmp1BD="is_poly_project: bad type %s";_tag_dyneither(_tmp1BD,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7CD,_tag_dyneither(_tmp1BC,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1068
 return 0;}_LL0:;}
# 1073
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp7CE=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp7CE,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1077
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp7CF=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp7CF,({struct Cyc_List_List*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->hd=s,_tmp1C5->tl=0;_tmp1C5;}),0U);});}
# 1081
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_type(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1087
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp7D0,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1091
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp7D1=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp7D1,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=rgn,_tmp1C7[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1095
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_type(elt_type))
return({struct Cyc_Absyn_Exp*_tmp7D2=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp7D2,({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=s,_tmp1C8[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1099
return({struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp7D3,({struct Cyc_Absyn_Exp*_tmp1C9[2U];_tmp1C9[0]=s,_tmp1C9[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp7D4,({struct Cyc_Absyn_Exp*_tmp1CA[3U];_tmp1CA[0]=rgn,_tmp1CA[1]=s,_tmp1CA[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CA,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1106
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp7D5=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp7D5,({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=e,_tmp1CB->tl=0;_tmp1CB;}),0U);});}
# 1109
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp7D6=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp7D6,({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=e,_tmp1CC->tl=0;_tmp1CC;}),0U);});}
# 1113
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp7D7=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp7D7,({void*_tmp1CD=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CD,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1118
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1126
int is_string=0;
{void*_tmp1CE=e->r;void*_tmp1CF=_tmp1CE;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CF)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CF)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1132
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp7D9=({struct Cyc_List_List*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));({struct Cyc_Absyn_Decl*_tmp7D8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->tag=0U,_tmp1D0->f1=vd;_tmp1D0;}),0U);_tmp1D1->hd=_tmp7D8;}),_tmp1D1->tl=Cyc_Toc_result_decls;_tmp1D1;});Cyc_Toc_result_decls=_tmp7D9;});
({struct Cyc_Absyn_Exp*_tmp7DA=Cyc_Absyn_var_exp(x,0U);xexp=_tmp7DA;});
({struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp7DB;});}else{
# 1142
({struct Cyc_Absyn_Exp*_tmp7DD=({void*_tmp7DC=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp7DC,e);});xexp=_tmp7DD;});
# 1144
({struct Cyc_Absyn_Exp*_tmp7DF=({void*_tmp7DE=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp7DE,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp7DF;});}
# 1146
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1D2[3U];({struct _tuple19*_tmp7E2=({struct _tuple19*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xexp;_tmp1D3;});_tmp1D2[0]=_tmp7E2;}),({
struct _tuple19*_tmp7E1=({struct _tuple19*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->f1=0,_tmp1D4->f2=xexp;_tmp1D4;});_tmp1D2[1]=_tmp7E1;}),({
struct _tuple19*_tmp7E0=({struct _tuple19*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->f1=0,_tmp1D5->f2=xplussz;_tmp1D5;});_tmp1D2[2]=_tmp7E0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D2,sizeof(struct _tuple19*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1183 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D6=nv;int _tmp1D7;_LL1: _tmp1D7=_tmp1D6->toplevel;_LL2:;
return _tmp1D7;}
# 1187
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D8=nv;int*_tmp1D9;_LL1: _tmp1D9=_tmp1D8->in_lhs;_LL2:;
return*_tmp1D9;}
# 1193
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1DC=_region_malloc(r,sizeof(*_tmp1DC));_tmp1DC->break_lab=0,_tmp1DC->continue_lab=0,_tmp1DC->fallthru_info=0,_tmp1DC->toplevel=1,({
# 1198
int*_tmp7E4=({int*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));*_tmp1DA=0;_tmp1DA;});_tmp1DC->in_lhs=_tmp7E4;}),({
int*_tmp7E3=({int*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));*_tmp1DB=0;_tmp1DB;});_tmp1DC->in_sizeof=_tmp7E3;}),_tmp1DC->rgn=r;_tmp1DC;});}
# 1203
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DD=e;struct _dyneither_ptr**_tmp1E4;struct _dyneither_ptr**_tmp1E3;struct Cyc_Toc_FallthruInfo*_tmp1E2;int _tmp1E1;int*_tmp1E0;int*_tmp1DF;_LL1: _tmp1E4=_tmp1DD->break_lab;_tmp1E3=_tmp1DD->continue_lab;_tmp1E2=_tmp1DD->fallthru_info;_tmp1E1=_tmp1DD->toplevel;_tmp1E0=_tmp1DD->in_lhs;_tmp1DF=_tmp1DD->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DE=_region_malloc(r,sizeof(*_tmp1DE));_tmp1DE->break_lab=_tmp1E4,_tmp1DE->continue_lab=_tmp1E3,_tmp1DE->fallthru_info=_tmp1E2,_tmp1DE->toplevel=_tmp1E1,_tmp1DE->in_lhs=_tmp1E0,_tmp1DE->in_sizeof=_tmp1DF,_tmp1DE->rgn=r;_tmp1DE;});}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E5=e;struct _dyneither_ptr**_tmp1EC;struct _dyneither_ptr**_tmp1EB;struct Cyc_Toc_FallthruInfo*_tmp1EA;int _tmp1E9;int*_tmp1E8;int*_tmp1E7;_LL1: _tmp1EC=_tmp1E5->break_lab;_tmp1EB=_tmp1E5->continue_lab;_tmp1EA=_tmp1E5->fallthru_info;_tmp1E9=_tmp1E5->toplevel;_tmp1E8=_tmp1E5->in_lhs;_tmp1E7=_tmp1E5->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E6=_region_malloc(r,sizeof(*_tmp1E6));_tmp1E6->break_lab=_tmp1EC,_tmp1E6->continue_lab=_tmp1EB,_tmp1E6->fallthru_info=_tmp1EA,_tmp1E6->toplevel=0,_tmp1E6->in_lhs=_tmp1E8,_tmp1E6->in_sizeof=_tmp1E7,_tmp1E6->rgn=r;_tmp1E6;});}
# 1212
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1ED=e;struct _dyneither_ptr**_tmp1F4;struct _dyneither_ptr**_tmp1F3;struct Cyc_Toc_FallthruInfo*_tmp1F2;int _tmp1F1;int*_tmp1F0;int*_tmp1EF;_LL1: _tmp1F4=_tmp1ED->break_lab;_tmp1F3=_tmp1ED->continue_lab;_tmp1F2=_tmp1ED->fallthru_info;_tmp1F1=_tmp1ED->toplevel;_tmp1F0=_tmp1ED->in_lhs;_tmp1EF=_tmp1ED->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1EE=_region_malloc(r,sizeof(*_tmp1EE));_tmp1EE->break_lab=_tmp1F4,_tmp1EE->continue_lab=_tmp1F3,_tmp1EE->fallthru_info=_tmp1F2,_tmp1EE->toplevel=1,_tmp1EE->in_lhs=_tmp1F0,_tmp1EE->in_sizeof=_tmp1EF,_tmp1EE->rgn=r;_tmp1EE;});}
# 1216
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F5=e;int*_tmp1F6;_LL1: _tmp1F6=_tmp1F5->in_lhs;_LL2:;
*_tmp1F6=b;}
# 1220
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F7=e;int*_tmp1F9;_LL1: _tmp1F9=_tmp1F7->in_sizeof;_LL2:;{
int _tmp1F8=*_tmp1F9;
*_tmp1F9=b;
return _tmp1F8;};}
# 1226
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FA=e;int*_tmp1FB;_LL1: _tmp1FB=_tmp1FA->in_sizeof;_LL2:;
return*_tmp1FB;}
# 1233
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FC=e;struct _dyneither_ptr**_tmp203;struct _dyneither_ptr**_tmp202;struct Cyc_Toc_FallthruInfo*_tmp201;int _tmp200;int*_tmp1FF;int*_tmp1FE;_LL1: _tmp203=_tmp1FC->break_lab;_tmp202=_tmp1FC->continue_lab;_tmp201=_tmp1FC->fallthru_info;_tmp200=_tmp1FC->toplevel;_tmp1FF=_tmp1FC->in_lhs;_tmp1FE=_tmp1FC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1FD=_region_malloc(r,sizeof(*_tmp1FD));_tmp1FD->break_lab=0,_tmp1FD->continue_lab=0,_tmp1FD->fallthru_info=_tmp201,_tmp1FD->toplevel=_tmp200,_tmp1FD->in_lhs=_tmp1FF,_tmp1FD->in_sizeof=_tmp1FE,_tmp1FD->rgn=r;_tmp1FD;});}
# 1239
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1243
struct Cyc_Toc_Env*_tmp204=e;struct _dyneither_ptr**_tmp20D;struct _dyneither_ptr**_tmp20C;struct Cyc_Toc_FallthruInfo*_tmp20B;int _tmp20A;int*_tmp209;int*_tmp208;_LL1: _tmp20D=_tmp204->break_lab;_tmp20C=_tmp204->continue_lab;_tmp20B=_tmp204->fallthru_info;_tmp20A=_tmp204->toplevel;_tmp209=_tmp204->in_lhs;_tmp208=_tmp204->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp207=_region_malloc(r,sizeof(*_tmp207));
_tmp207->label=fallthru_l,_tmp207->binders=fallthru_binders;_tmp207;});
return({struct Cyc_Toc_Env*_tmp206=_region_malloc(r,sizeof(*_tmp206));({struct _dyneither_ptr**_tmp7E5=({struct _dyneither_ptr**_tmp205=_region_malloc(r,sizeof(*_tmp205));*_tmp205=break_l;_tmp205;});_tmp206->break_lab=_tmp7E5;}),_tmp206->continue_lab=_tmp20C,_tmp206->fallthru_info=fi,_tmp206->toplevel=_tmp20A,_tmp206->in_lhs=_tmp209,_tmp206->in_sizeof=_tmp208,_tmp206->rgn=r;_tmp206;});};}
# 1249
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1251
struct Cyc_Toc_Env*_tmp20E=e;struct _dyneither_ptr**_tmp216;struct _dyneither_ptr**_tmp215;struct Cyc_Toc_FallthruInfo*_tmp214;int _tmp213;int*_tmp212;int*_tmp211;_LL1: _tmp216=_tmp20E->break_lab;_tmp215=_tmp20E->continue_lab;_tmp214=_tmp20E->fallthru_info;_tmp213=_tmp20E->toplevel;_tmp212=_tmp20E->in_lhs;_tmp211=_tmp20E->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp210=_region_malloc(r,sizeof(*_tmp210));({struct _dyneither_ptr**_tmp7E6=({struct _dyneither_ptr**_tmp20F=_region_malloc(r,sizeof(*_tmp20F));*_tmp20F=break_l;_tmp20F;});_tmp210->break_lab=_tmp7E6;}),_tmp210->continue_lab=_tmp215,_tmp210->fallthru_info=0,_tmp210->toplevel=_tmp213,_tmp210->in_lhs=_tmp212,_tmp210->in_sizeof=_tmp211,_tmp210->rgn=r;_tmp210;});}
# 1258
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1261
struct Cyc_Toc_Env*_tmp217=e;struct _dyneither_ptr**_tmp21F;struct _dyneither_ptr**_tmp21E;struct Cyc_Toc_FallthruInfo*_tmp21D;int _tmp21C;int*_tmp21B;int*_tmp21A;_LL1: _tmp21F=_tmp217->break_lab;_tmp21E=_tmp217->continue_lab;_tmp21D=_tmp217->fallthru_info;_tmp21C=_tmp217->toplevel;_tmp21B=_tmp217->in_lhs;_tmp21A=_tmp217->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp219=_region_malloc(r,sizeof(*_tmp219));_tmp219->break_lab=0,_tmp219->continue_lab=_tmp21E,({struct Cyc_Toc_FallthruInfo*_tmp7E7=({struct Cyc_Toc_FallthruInfo*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->label=next_l,_tmp218->binders=0;_tmp218;});_tmp219->fallthru_info=_tmp7E7;}),_tmp219->toplevel=_tmp21C,_tmp219->in_lhs=_tmp21B,_tmp219->in_sizeof=_tmp21A,_tmp219->rgn=r;_tmp219;});}
# 1274 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1277
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp220=e->annot;void*_tmp221=_tmp220;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp221)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp221)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp221)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp221)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp221)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp221)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp221)->tag == Cyc_Absyn_EmptyAnnot){_LLD: _LLE:
 return 1;}else{_LLF: _LL10:
({void*_tmp222=0U;({unsigned int _tmp7E9=e->loc;struct _dyneither_ptr _tmp7E8=({const char*_tmp223="Toc: do_null_check";_tag_dyneither(_tmp223,sizeof(char),19U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp7E9,_tmp7E8,_tag_dyneither(_tmp222,sizeof(void*),0U));});});}}}}}}}_LL0:;}
# 1289
static int Cyc_Toc_do_bounds_check(struct Cyc_Absyn_Exp*e){
void*_tmp224=e->annot;void*_tmp225=_tmp224;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp225)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp225)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 return 0;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp225)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp225)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 goto _LLA;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp225)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp225)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 1;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp225)->tag == Cyc_Absyn_EmptyAnnot){_LLD: _LLE:
 return 1;}else{_LLF: _LL10:
({void*_tmp226=0U;({unsigned int _tmp7EB=e->loc;struct _dyneither_ptr _tmp7EA=({const char*_tmp227="Toc: do_bounds_check";_tag_dyneither(_tmp227,sizeof(char),21U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp7EB,_tmp7EA,_tag_dyneither(_tmp226,sizeof(void*),0U));});});}}}}}}}_LL0:;}
# 1306
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1308
int ans;
int _tmp228=Cyc_Toc_in_lhs(nv);
void*_tmp229=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));
void*_tmp22A=Cyc_Toc_typ_to_c(_tmp229);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnullX_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp22B=_tmp229;void*_tmp23F;struct Cyc_Absyn_Tqual _tmp23E;void*_tmp23D;void*_tmp23C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->tag == 3U){_LL1: _tmp23F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_type;_tmp23E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_tq;_tmp23D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).ptr_atts).bounds;_tmp23C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).ptr_atts).zero_term;_LL2:
# 1319
{void*_tmp22C=annot;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Exp*_tmp238;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp22C)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1321
 if(!((unsigned int)({void*_tmp7EC=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7EC,_tmp23D);})))
({void*_tmp7F0=({void*_tmp7EF=({void*_tmp7ED=Cyc_Toc_typ_to_c(_tmp23F);Cyc_Absyn_cstar_type(_tmp7ED,_tmp23E);});Cyc_Toc_cast_it_r(_tmp7EF,({
struct Cyc_Absyn_Exp*_tmp7EE=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7EE,Cyc_Toc_curr_sp,0U);}));});
# 1322
ptr->r=_tmp7F0;});
# 1324
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp22C)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1327
 if(!((unsigned int)({void*_tmp7F1=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7F1,_tmp23D);})))
goto _LLB;
# 1330
({void*_tmp7F5=({void*_tmp7F4=_tmp22A;Cyc_Toc_cast_it_r(_tmp7F4,({
struct Cyc_Absyn_Exp*_tmp7F3=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp7F3,({struct Cyc_Absyn_Exp*_tmp22D[1U];({struct Cyc_Absyn_Exp*_tmp7F2=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22D[0]=_tmp7F2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp22D,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 1330
ptr->r=_tmp7F5;});
# 1332
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp22C)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp22C)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1337
void*ta1=Cyc_Toc_typ_to_c(_tmp23F);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp7FA=({void*_tmp7F9=Cyc_Absyn_cstar_type(ta1,_tmp23E);Cyc_Toc_cast_it_r(_tmp7F9,({
struct Cyc_Absyn_Exp*_tmp7F8=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp7F8,({struct Cyc_Absyn_Exp*_tmp22E[3U];({
struct Cyc_Absyn_Exp*_tmp7F7=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22E[0]=_tmp7F7;}),({
struct Cyc_Absyn_Exp*_tmp7F6=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp22E[1]=_tmp7F6;}),_tmp22E[2]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp22E,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 1339
ptr->r=_tmp7FA;});
# 1344
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp22C)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp238=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp22C)->f1;_LLF:
# 1347
 fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp239=_tmp238;goto _LL11;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp22C)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp239=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp22C)->f1;_LL11: {
# 1350
void*ta1=Cyc_Toc_typ_to_c(_tmp23F);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1355
struct _tuple13 _tmp22F=Cyc_Evexp_eval_const_uint_exp(_tmp239);struct _tuple13 _tmp230=_tmp22F;unsigned int _tmp235;int _tmp234;_LL15: _tmp235=_tmp230.f1;_tmp234=_tmp230.f2;_LL16:;
if((!_tmp234  || _tmp235 != 1) || !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1358
({void*_tmp7FF=({void*_tmp7FE=ta1;Cyc_Toc_cast_it_r(_tmp7FE,({struct Cyc_Absyn_Exp*_tmp7FD=fn_e;Cyc_Absyn_fncall_exp(_tmp7FD,({struct Cyc_Absyn_Exp*_tmp231[4U];({struct Cyc_Absyn_Exp*_tmp7FC=Cyc_Absyn_new_exp(ptr->r,0U);_tmp231[0]=_tmp7FC;}),_tmp231[1]=_tmp239,({
struct Cyc_Absyn_Exp*_tmp7FB=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp231[2]=_tmp7FB;}),_tmp231[3]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp231,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 1358
ptr->r=_tmp7FF;});
# 1361
ans=1;}else{
# 1364
if(Cyc_Toc_is_zero(_tmp239)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1367
({void*_tmp803=({void*_tmp802=_tmp22A;Cyc_Toc_cast_it_r(_tmp802,({
struct Cyc_Absyn_Exp*_tmp801=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp801,({struct Cyc_Absyn_Exp*_tmp232[1U];({struct Cyc_Absyn_Exp*_tmp800=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp232[0]=_tmp800;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp232,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 1367
ptr->r=_tmp803;});
# 1370
ans=0;}else{
# 1374
({struct Cyc_Absyn_Exp*_tmp804=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);fn_e=_tmp804;});
({void*_tmp808=({void*_tmp807=ta1;Cyc_Toc_cast_it_r(_tmp807,({struct Cyc_Absyn_Exp*_tmp806=fn_e;Cyc_Absyn_fncall_exp(_tmp806,({struct Cyc_Absyn_Exp*_tmp233[3U];({
struct Cyc_Absyn_Exp*_tmp805=Cyc_Absyn_new_exp(ptr->r,0U);_tmp233[0]=_tmp805;}),_tmp233[1]=_tmp239,_tmp233[2]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp233,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 1375
ptr->r=_tmp808;});
# 1378
ans=1;}}
# 1381
goto _LL5;}}else{_LL12: _LL13:
({void*_tmp236=0U;({struct _dyneither_ptr _tmp809=({const char*_tmp237="FIX: ptr_use_to_c, late-generated bad annot";_tag_dyneither(_tmp237,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp809,_tag_dyneither(_tmp236,sizeof(void*),0U));});});}}}}}}_LL5:;}
# 1384
Cyc_Toc_set_lhs(nv,_tmp228);
return ans;}else{_LL3: _LL4:
({void*_tmp23A=0U;({struct _dyneither_ptr _tmp80A=({const char*_tmp23B="ptr_use_to_c: non-pointer-type";_tag_dyneither(_tmp23B,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp80A,_tag_dyneither(_tmp23A,sizeof(void*),0U));});});}_LL0:;};}
# 1390
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp240=0U;({struct _dyneither_ptr _tmp80B=({const char*_tmp241="Missing type in primop ";_tag_dyneither(_tmp241,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp80B,_tag_dyneither(_tmp240,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1394
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242->f1=Cyc_Toc_mt_tq,({void*_tmp80C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp242->f2=_tmp80C;});_tmp242;});}
# 1399
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp243=e->r;void*_tmp244=_tmp243;struct Cyc_Absyn_Exp*_tmp24C;int _tmp24B;struct Cyc_Absyn_Exp*_tmp24A;int _tmp249;struct Cyc_List_List*_tmp248;switch(*((int*)_tmp244)){case 26U: _LL1: _tmp248=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp244)->f1;_LL2:
# 1402
{struct Cyc_List_List*_tmp245=_tmp248;for(0;_tmp245 != 0;_tmp245=_tmp245->tl){
if((*((struct _tuple19*)_tmp245->hd)).f1 != 0)
({void*_tmp246=0U;({struct _dyneither_ptr _tmp80D=({const char*_tmp247="array designators for abstract-field initialization";_tag_dyneither(_tmp247,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp80D,_tag_dyneither(_tmp246,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp80E=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp248),0U);_tmp24A=_tmp80E;});_tmp249=0;goto _LL4;case 27U: _LL3: _tmp24A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp244)->f2;_tmp249=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp244)->f4;_LL4:
 _tmp24C=_tmp24A;_tmp24B=_tmp249;goto _LL6;case 28U: _LL5: _tmp24C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp244)->f1;_tmp24B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp244)->f3;_LL6:
# 1408
({struct Cyc_Absyn_Exp*_tmp80F=Cyc_Absyn_copy_exp(_tmp24C);_tmp24C=_tmp80F;});
return _tmp24B?({struct Cyc_Absyn_Exp*_tmp810=_tmp24C;Cyc_Absyn_add_exp(_tmp810,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp24C;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1417
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1425
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp24D=e->r;void*_tmp24E=_tmp24D;struct Cyc_List_List*_tmp24F;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24E)->tag == 29U){_LL1: _tmp24F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24E)->f3;_LL2:
 dles=_tmp24F;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1431
{void*_tmp250=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp251=_tmp250;struct Cyc_Absyn_Aggrdecl*_tmp252;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp251)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp251)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp251)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp252=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp251)->f1)->f1).KnownAggr).val;_LL7:
# 1433
 if(_tmp252->impl == 0)
return 0;
if(_tmp252->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp252->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1441
 return 0;}_LL5:;}
# 1443
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp253=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp254=(struct _tuple19*)dles->hd;struct _tuple19*_tmp255=_tmp254;struct Cyc_List_List*_tmp25E;struct Cyc_Absyn_Exp*_tmp25D;_LLB: _tmp25E=_tmp255->f1;_tmp25D=_tmp255->f2;_LLC:;{
struct _dyneither_ptr*_tmp256=Cyc_Absyn_designatorlist_to_fieldname(_tmp25E);
if(!Cyc_strptrcmp(_tmp256,_tmp253->name)){
struct Cyc_Absyn_Exp*_tmp257=Cyc_Toc_get_varsizeexp(_tmp25D);
if(_tmp257 != 0)
return _tmp257;{
void*_tmp258=Cyc_Tcutil_compress(_tmp253->type);void*_tmp259=_tmp258;void*_tmp25C;struct Cyc_Absyn_Exp*_tmp25B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp259)->tag == 4U){_LLE: _tmp25C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp259)->f1).elt_type;_tmp25B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp259)->f1).num_elts;_LLF:
# 1458
 if(_tmp25B == 0  || !Cyc_Toc_is_zero(_tmp25B))
return 0;
# 1464
return({struct Cyc_Absyn_Exp*_tmp813=({struct Cyc_Absyn_Exp*_tmp812=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp812,({struct Cyc_Absyn_Exp*_tmp25A[2U];_tmp25A[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp25D)),({
# 1466
struct Cyc_Absyn_Exp*_tmp811=Cyc_Absyn_sizeoftype_exp(_tmp25C,0U);_tmp25A[1]=_tmp811;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25A,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1464
Cyc_Absyn_add_exp(_tmp813,
# 1467
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1472
({void*_tmp25F=0U;({struct _dyneither_ptr _tmp814=({const char*_tmp260="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp260,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp814,_tag_dyneither(_tmp25F,sizeof(void*),0U));});});};}
# 1475
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp261=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
struct Cyc_Absyn_Aggrfield*_tmp262=(struct Cyc_Absyn_Aggrfield*)_tmp261->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp262->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1482
({void*_tmp263=0U;({struct _dyneither_ptr _tmp816=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp266=({struct Cyc_String_pa_PrintArg_struct _tmp73C;_tmp73C.tag=0U,_tmp73C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp73C;});void*_tmp264[1U];_tmp264[0]=& _tmp266;({struct _dyneither_ptr _tmp815=({const char*_tmp265="get_member_offset %s failed";_tag_dyneither(_tmp265,sizeof(char),28U);});Cyc_aprintf(_tmp815,_tag_dyneither(_tmp264,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp816,_tag_dyneither(_tmp263,sizeof(void*),0U));});});}struct _tuple30{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1486
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple30*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1489
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1492
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->tag=5U,_tmp267->f1=e1,_tmp267->f2=incr;_tmp267;}),0U);}
# 1496
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1505
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1507
void*_tmp268=e1->r;void*_tmp269=_tmp268;struct Cyc_Absyn_Exp*_tmp271;struct _dyneither_ptr*_tmp270;int _tmp26F;int _tmp26E;void*_tmp26D;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Stmt*_tmp26B;switch(*((int*)_tmp269)){case 37U: _LL1: _tmp26B=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp26B,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp26D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_tmp26C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp269)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp26C,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp271=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_tmp270=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269)->f2;_tmp26F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269)->f3;_tmp26E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269)->f4;_LL6:
# 1512
 e1->r=_tmp271->r;
({struct Cyc_Absyn_Exp*_tmp819=e1;struct Cyc_List_List*_tmp818=({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->hd=_tmp270,_tmp26A->tl=fs;_tmp26A;});struct Cyc_Absyn_Exp*(*_tmp817)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp819,_tmp818,_tmp817,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1520
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1522
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp81A=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp81A;});}
({void*_tmp81B=(f(e1_copy,f_env))->r;e1->r=_tmp81B;});
goto _LL0;}}_LL0:;}
# 1528
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1530
void*_tmp272=s->r;void*_tmp273=_tmp272;struct Cyc_Absyn_Stmt*_tmp27A;struct Cyc_Absyn_Decl*_tmp279;struct Cyc_Absyn_Stmt*_tmp278;struct Cyc_Absyn_Exp*_tmp277;switch(*((int*)_tmp273)){case 1U: _LL1: _tmp277=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp273)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp277,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp279=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp273)->f1;_tmp278=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp273)->f2;_LL4:
# 1533
 Cyc_Toc_lvalue_assign_stmt(_tmp278,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp27A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp273)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp27A,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp276=({struct Cyc_String_pa_PrintArg_struct _tmp73D;_tmp73D.tag=0U,({struct _dyneither_ptr _tmp81C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp73D.f1=_tmp81C;});_tmp73D;});void*_tmp274[1U];_tmp274[0]=& _tmp276;({struct _dyneither_ptr _tmp81D=({const char*_tmp275="lvalue_assign_stmt: %s";_tag_dyneither(_tmp275,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp81D,_tag_dyneither(_tmp274,sizeof(void*),1U));});});}_LL0:;}
# 1539
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1543
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp27B=e->r;void*_tmp27C=_tmp27B;struct Cyc_Absyn_Stmt*_tmp283;struct Cyc_Absyn_Exp*_tmp282;void**_tmp281;struct Cyc_Absyn_Exp**_tmp280;switch(*((int*)_tmp27C)){case 14U: _LL1: _tmp281=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27C)->f1;_tmp280=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27C)->f2;_LL2:
# 1546
({struct Cyc_Absyn_Exp*_tmp81E=Cyc_Toc_push_address_exp(*_tmp280);*_tmp280=_tmp81E;});
({void*_tmp81F=Cyc_Absyn_cstar_type(*_tmp281,Cyc_Toc_mt_tq);*_tmp281=_tmp81F;});
return e;case 20U: _LL3: _tmp282=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp27C)->f1;_LL4:
# 1550
 return _tmp282;case 37U: _LL5: _tmp283=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp27C)->f1;_LL6:
# 1554
 Cyc_Toc_push_address_stmt(_tmp283);
return e;default: _LL7: _LL8:
# 1557
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp73E;_tmp73E.tag=0U,({struct _dyneither_ptr _tmp820=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp73E.f1=_tmp820;});_tmp73E;});void*_tmp27D[1U];_tmp27D[0]=& _tmp27F;({struct _dyneither_ptr _tmp821=({const char*_tmp27E="can't take & of exp %s";_tag_dyneither(_tmp27E,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp821,_tag_dyneither(_tmp27D,sizeof(void*),1U));});});}_LL0:;}
# 1562
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp284=s->r;void*_tmp285=_tmp284;struct Cyc_Absyn_Exp**_tmp28B;struct Cyc_Absyn_Stmt*_tmp28A;struct Cyc_Absyn_Stmt*_tmp289;switch(*((int*)_tmp285)){case 2U: _LL1: _tmp289=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp285)->f2;_LL2:
 _tmp28A=_tmp289;goto _LL4;case 12U: _LL3: _tmp28A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp285)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp28A);goto _LL0;case 1U: _LL5: _tmp28B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp285)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp822=Cyc_Toc_push_address_exp(*_tmp28B);*_tmp28B=_tmp822;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp288=({struct Cyc_String_pa_PrintArg_struct _tmp73F;_tmp73F.tag=0U,({struct _dyneither_ptr _tmp823=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp73F.f1=_tmp823;});_tmp73F;});void*_tmp286[1U];_tmp286[0]=& _tmp288;({struct _dyneither_ptr _tmp824=({const char*_tmp287="can't take & of stmt %s";_tag_dyneither(_tmp287,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp824,_tag_dyneither(_tmp286,sizeof(void*),1U));});});}_LL0:;}
# 1571
static void*Cyc_Toc_get_ptr_elt_type(void*t){
void*_tmp28C=Cyc_Tcutil_compress(t);void*_tmp28D=_tmp28C;struct Cyc_Absyn_PtrInfo _tmp290;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28D)->tag == 3U){_LL1: _tmp290=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28D)->f1;_LL2:
 return _tmp290.elt_type;}else{_LL3: _LL4:
({void*_tmp28E=0U;({struct _dyneither_ptr _tmp825=({const char*_tmp28F="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp28F,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp825,_tag_dyneither(_tmp28E,sizeof(void*),0U));});});}_LL0:;}
# 1581
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp291=Cyc_Tcutil_compress(t);void*_tmp292=_tmp291;void*_tmp2A3;switch(*((int*)_tmp292)){case 3U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp826=Cyc_Absyn_null_exp(0U);res=_tmp826;});goto _LL0;case 0U: _LL3: _tmp2A3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp292)->f1;_LL4:
# 1586
{void*_tmp293=_tmp2A3;int _tmp29F;enum Cyc_Absyn_Sign _tmp29E;enum Cyc_Absyn_Sign _tmp29D;enum Cyc_Absyn_Sign _tmp29C;enum Cyc_Absyn_Sign _tmp29B;enum Cyc_Absyn_Sign _tmp29A;switch(*((int*)_tmp293)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f2){case Cyc_Absyn_Char_sz: _LL8: _tmp29A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f1;_LL9:
# 1588
({struct Cyc_Absyn_Exp*_tmp827=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp29A,'\000'),0U);res=_tmp827;});goto _LL7;case Cyc_Absyn_Short_sz: _LLA: _tmp29B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f1;_LLB:
# 1590
({struct Cyc_Absyn_Exp*_tmp828=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp29B,0),0U);res=_tmp828;});goto _LL7;case Cyc_Absyn_Int_sz: _LL10: _tmp29C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f1;_LL11:
# 1593
 _tmp29D=_tmp29C;goto _LL13;case Cyc_Absyn_Long_sz: _LL12: _tmp29D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f1;_LL13:
# 1595
({struct Cyc_Absyn_Exp*_tmp829=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp29D,0),0U);res=_tmp829;});goto _LL7;default: _LL14: _tmp29E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f1;_LL15:
# 1597
({struct Cyc_Absyn_Exp*_tmp82A=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp29E,(long long)0),0U);res=_tmp82A;});goto _LL7;}case 15U: _LLC: _LLD:
# 1591
 goto _LLF;case 16U: _LLE: _LLF:
 _tmp29C=1U;goto _LL11;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp293)->f1){case 0U: _LL16: _LL17:
# 1598
({struct Cyc_Absyn_Exp*_tmp82B=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp294="0.0F";_tag_dyneither(_tmp294,sizeof(char),5U);}),0),0U);res=_tmp82B;});goto _LL7;case 1U: _LL18: _LL19:
({struct Cyc_Absyn_Exp*_tmp82C=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp295="0.0";_tag_dyneither(_tmp295,sizeof(char),4U);}),1),0U);res=_tmp82C;});goto _LL7;default: _LL1A: _tmp29F=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp293)->f1;_LL1B:
({struct Cyc_Absyn_Exp*_tmp82E=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp82D=({const char*_tmp296="0.0L";_tag_dyneither(_tmp296,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp82D,_tmp29F);}),0U);res=_tmp82E;});goto _LL7;}default: _LL1C: _LL1D:
# 1602
({struct Cyc_String_pa_PrintArg_struct _tmp299=({struct Cyc_String_pa_PrintArg_struct _tmp740;_tmp740.tag=0U,({struct _dyneither_ptr _tmp82F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp740.f1=_tmp82F;});_tmp740;});void*_tmp297[1U];_tmp297[0]=& _tmp299;({struct _dyneither_ptr _tmp830=({const char*_tmp298="found non-zero type %s in generate_zero";_tag_dyneither(_tmp298,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp830,_tag_dyneither(_tmp297,sizeof(void*),1U));});});}_LL7:;}
# 1604
goto _LL0;default: _LL5: _LL6:
# 1606
({struct Cyc_String_pa_PrintArg_struct _tmp2A2=({struct Cyc_String_pa_PrintArg_struct _tmp741;_tmp741.tag=0U,({struct _dyneither_ptr _tmp831=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp741.f1=_tmp831;});_tmp741;});void*_tmp2A0[1U];_tmp2A0[0]=& _tmp2A2;({struct _dyneither_ptr _tmp832=({const char*_tmp2A1="found non-zero type %s in generate_zero";_tag_dyneither(_tmp2A1,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp832,_tag_dyneither(_tmp2A0,sizeof(void*),1U));});});}_LL0:;}
# 1608
res->topt=t;
return res;}
# 1615
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1628 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp2A4=Cyc_Toc_typ_to_c(elt_type);
void*_tmp2A5=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp2A6=Cyc_Absyn_cstar_type(_tmp2A4,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp2A7=({struct Cyc_Core_Opt*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8->v=_tmp2A6;_tmp2D8;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp2A8=e1->r;void*_tmp2A9=_tmp2A8;struct Cyc_Absyn_Exp*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AD;switch(*((int*)_tmp2A9)){case 20U: _LL1: _tmp2AD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2A9)->f1;_LL2:
# 1636
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp833=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AD,0,Cyc_Absyn_Other_coercion,0U);_tmp2AD=_tmp833;});
_tmp2AD->topt=fat_ptr_type;
_tmp2AD->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;}
# 1641
Cyc_Toc_exp_to_c(nv,_tmp2AD);xinit=_tmp2AD;goto _LL0;case 23U: _LL3: _tmp2AF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A9)->f1;_tmp2AE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A9)->f2;_LL4:
# 1643
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp834=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AF,0,Cyc_Absyn_Other_coercion,0U);_tmp2AF=_tmp834;});
_tmp2AF->topt=fat_ptr_type;
_tmp2AF->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;}
# 1648
Cyc_Toc_exp_to_c(nv,_tmp2AF);Cyc_Toc_exp_to_c(nv,_tmp2AE);
({struct Cyc_Absyn_Exp*_tmp837=({struct Cyc_Absyn_Exp*_tmp836=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp836,({struct Cyc_Absyn_Exp*_tmp2AA[3U];_tmp2AA[0]=_tmp2AF,({
struct Cyc_Absyn_Exp*_tmp835=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2AA[1]=_tmp835;}),_tmp2AA[2]=_tmp2AE;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2AA,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1649
xinit=_tmp837;});
# 1651
goto _LL0;default: _LL5: _LL6:
({void*_tmp2AB=0U;({struct _dyneither_ptr _tmp838=({const char*_tmp2AC="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp2AC,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp838,_tag_dyneither(_tmp2AB,sizeof(void*),0U));});});}_LL0:;}{
# 1654
struct _tuple1*_tmp2B0=Cyc_Toc_temp_var();
struct _RegionHandle _tmp2B1=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp2B1;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp2B2=({struct Cyc_Absyn_Vardecl*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->sc=Cyc_Absyn_Public,_tmp2D7->name=_tmp2B0,_tmp2D7->varloc=0U,_tmp2D7->tq=Cyc_Toc_mt_tq,_tmp2D7->type=_tmp2A5,_tmp2D7->initializer=xinit,_tmp2D7->rgn=0,_tmp2D7->attributes=0,_tmp2D7->escapes=0;_tmp2D7;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B3=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->tag=4U,_tmp2D6->f1=_tmp2B2;_tmp2D6;});
struct Cyc_Absyn_Exp*_tmp2B4=Cyc_Absyn_varb_exp((void*)_tmp2B3,0U);
_tmp2B4->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp2B5=Cyc_Absyn_deref_exp(_tmp2B4,0U);
_tmp2B5->topt=elt_type;
_tmp2B5->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,_tmp2B5);{
struct _tuple1*_tmp2B6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B7=({struct Cyc_Absyn_Vardecl*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5->sc=Cyc_Absyn_Public,_tmp2D5->name=_tmp2B6,_tmp2D5->varloc=0U,_tmp2D5->tq=Cyc_Toc_mt_tq,_tmp2D5->type=_tmp2A4,_tmp2D5->initializer=_tmp2B5,_tmp2D5->rgn=0,_tmp2D5->attributes=0,_tmp2D5->escapes=0;_tmp2D5;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B8=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4->tag=4U,_tmp2D4->f1=_tmp2B7;_tmp2D4;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2B9=Cyc_Absyn_varb_exp((void*)_tmp2B8,0U);
_tmp2B9->topt=_tmp2B5->topt;
({struct Cyc_Absyn_Exp*_tmp839=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2B9,e2,0U);z_init=_tmp839;});
z_init->topt=_tmp2B9->topt;}
# 1674
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2BA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2BB=({struct Cyc_Absyn_Vardecl*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->sc=Cyc_Absyn_Public,_tmp2D3->name=_tmp2BA,_tmp2D3->varloc=0U,_tmp2D3->tq=Cyc_Toc_mt_tq,_tmp2D3->type=_tmp2A4,_tmp2D3->initializer=z_init,_tmp2D3->rgn=0,_tmp2D3->attributes=0,_tmp2D3->escapes=0;_tmp2D3;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BC=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->tag=4U,_tmp2D2->f1=_tmp2BB;_tmp2D2;});
# 1680
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_varb_exp((void*)_tmp2B8,0U);_tmp2BD->topt=_tmp2B5->topt;{
struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BF=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2BD,_tmp2BE,0U);
_tmp2BF->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2BF);{
# 1686
struct Cyc_Absyn_Exp*_tmp2C0=Cyc_Absyn_varb_exp((void*)_tmp2BC,0U);_tmp2C0->topt=_tmp2B5->topt;{
struct Cyc_Absyn_Exp*_tmp2C1=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2C2=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2C0,_tmp2C1,0U);
_tmp2C2->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2C2);{
# 1692
struct Cyc_List_List*_tmp2C3=({struct Cyc_Absyn_Exp*_tmp2D1[2U];({struct Cyc_Absyn_Exp*_tmp83B=Cyc_Absyn_varb_exp((void*)_tmp2B3,0U);_tmp2D1[0]=_tmp83B;}),({
struct Cyc_Absyn_Exp*_tmp83A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2D1[1]=_tmp83A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2D1,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2C4=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp83D=({struct Cyc_Absyn_Exp*_tmp83C=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2C3,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp83C,_tmp2C4,0U);});xsize=_tmp83D;});{
# 1699
struct Cyc_Absyn_Exp*_tmp2C5=({struct Cyc_Absyn_Exp*_tmp83E=xsize;Cyc_Absyn_and_exp(_tmp83E,Cyc_Absyn_and_exp(_tmp2BF,_tmp2C2,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C6=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2C7=({struct Cyc_Absyn_Exp*_tmp83F=Cyc_Absyn_varb_exp((void*)_tmp2B3,0U);Cyc_Toc_member_exp(_tmp83F,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp840=Cyc_Toc_cast_it(_tmp2A6,_tmp2C7);_tmp2C7=_tmp840;});{
struct Cyc_Absyn_Exp*_tmp2C8=Cyc_Absyn_deref_exp(_tmp2C7,0U);
struct Cyc_Absyn_Exp*_tmp2C9=({struct Cyc_Absyn_Exp*_tmp841=_tmp2C8;Cyc_Absyn_assign_exp(_tmp841,Cyc_Absyn_var_exp(_tmp2BA,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2CA=Cyc_Absyn_exp_stmt(_tmp2C9,0U);
({struct Cyc_Absyn_Stmt*_tmp845=({struct Cyc_Absyn_Stmt*_tmp844=({struct Cyc_Absyn_Exp*_tmp843=_tmp2C5;struct Cyc_Absyn_Stmt*_tmp842=_tmp2C6;Cyc_Absyn_ifthenelse_stmt(_tmp843,_tmp842,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp844,_tmp2CA,0U);});_tmp2CA=_tmp845;});
({struct Cyc_Absyn_Stmt*_tmp848=({struct Cyc_Absyn_Decl*_tmp847=({struct Cyc_Absyn_Decl*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));({void*_tmp846=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->tag=0U,_tmp2CB->f1=_tmp2BB;_tmp2CB;});_tmp2CC->r=_tmp846;}),_tmp2CC->loc=0U;_tmp2CC;});Cyc_Absyn_decl_stmt(_tmp847,_tmp2CA,0U);});_tmp2CA=_tmp848;});
({struct Cyc_Absyn_Stmt*_tmp84B=({struct Cyc_Absyn_Decl*_tmp84A=({struct Cyc_Absyn_Decl*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));({void*_tmp849=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->tag=0U,_tmp2CD->f1=_tmp2B7;_tmp2CD;});_tmp2CE->r=_tmp849;}),_tmp2CE->loc=0U;_tmp2CE;});Cyc_Absyn_decl_stmt(_tmp84A,_tmp2CA,0U);});_tmp2CA=_tmp84B;});
({struct Cyc_Absyn_Stmt*_tmp84E=({struct Cyc_Absyn_Decl*_tmp84D=({struct Cyc_Absyn_Decl*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));({void*_tmp84C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF->tag=0U,_tmp2CF->f1=_tmp2B2;_tmp2CF;});_tmp2D0->r=_tmp84C;}),_tmp2D0->loc=0U;_tmp2D0;});Cyc_Absyn_decl_stmt(_tmp84D,_tmp2CA,0U);});_tmp2CA=_tmp84E;});
({void*_tmp84F=Cyc_Toc_stmt_exp_r(_tmp2CA);e->r=_tmp84F;});};};};};};};};};};}
# 1656
;_pop_region(rgn2);};}
# 1725 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1729
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2D9=Cyc_Tcutil_compress(aggrtype);void*_tmp2DA=_tmp2D9;union Cyc_Absyn_AggrInfo _tmp2DF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->f1)->tag == 20U){_LL1: _tmp2DF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->f1)->f1;_LL2:
# 1732
({struct Cyc_Absyn_Aggrdecl*_tmp850=Cyc_Absyn_get_known_aggrdecl(_tmp2DF);ad=_tmp850;});goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp2DB=0U;({struct _dyneither_ptr _tmp853=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DE=({struct Cyc_String_pa_PrintArg_struct _tmp742;_tmp742.tag=0U,({
struct _dyneither_ptr _tmp851=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp742.f1=_tmp851;});_tmp742;});void*_tmp2DC[1U];_tmp2DC[0]=& _tmp2DE;({struct _dyneither_ptr _tmp852=({const char*_tmp2DD="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2DD,sizeof(char),51U);});Cyc_aprintf(_tmp852,_tag_dyneither(_tmp2DC,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp853,_tag_dyneither(_tmp2DB,sizeof(void*),0U));});});}_LL0:;}{
# 1736
struct _tuple1*_tmp2E0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2E1=Cyc_Absyn_var_exp(_tmp2E0,0U);
struct Cyc_Absyn_Exp*_tmp2E2=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2E3=Cyc_Absyn_aggrarrow_exp(_tmp2E1,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2E4=Cyc_Absyn_neq_exp(_tmp2E3,_tmp2E2,0U);
struct Cyc_Absyn_Exp*_tmp2E5=Cyc_Absyn_aggrarrow_exp(_tmp2E1,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2E6=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2E5,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E7=({struct Cyc_Absyn_Exp*_tmp855=_tmp2E4;struct Cyc_Absyn_Stmt*_tmp854=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp855,_tmp854,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2E8=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2E9=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2EA=({struct _tuple1*_tmp858=_tmp2E0;void*_tmp857=_tmp2E8;struct Cyc_Absyn_Exp*_tmp856=_tmp2E9;Cyc_Absyn_declare_stmt(_tmp858,_tmp857,_tmp856,Cyc_Absyn_seq_stmt(_tmp2E7,_tmp2E6,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2EA);}else{
# 1750
struct Cyc_Absyn_Exp*_tmp2EB=({struct Cyc_Absyn_Exp*_tmp859=aggrproj(_tmp2E1,f,0U);Cyc_Toc_member_exp(_tmp859,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2EC=Cyc_Absyn_neq_exp(_tmp2EB,_tmp2E2,0U);
struct Cyc_Absyn_Exp*_tmp2ED=({struct Cyc_Absyn_Exp*_tmp85A=aggrproj(_tmp2E1,f,0U);Cyc_Toc_member_exp(_tmp85A,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2EE=Cyc_Absyn_exp_stmt(_tmp2ED,0U);
struct Cyc_Absyn_Stmt*_tmp2EF=({struct Cyc_Absyn_Exp*_tmp85C=_tmp2EC;struct Cyc_Absyn_Stmt*_tmp85B=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp85C,_tmp85B,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2F0=({struct _tuple1*_tmp85F=_tmp2E0;void*_tmp85E=e1_c_type;struct Cyc_Absyn_Exp*_tmp85D=e1;Cyc_Absyn_declare_stmt(_tmp85F,_tmp85E,_tmp85D,Cyc_Absyn_seq_stmt(_tmp2EF,_tmp2EE,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2F0);}};}
# 1760
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1763
void*_tmp2F1=Cyc_Tcutil_compress(t);void*_tmp2F2=_tmp2F1;union Cyc_Absyn_AggrInfo _tmp2F9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F2)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F2)->f1)->tag == 20U){_LL1: _tmp2F9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F2)->f1)->f1;_LL2: {
# 1765
struct Cyc_Absyn_Aggrdecl*_tmp2F3=Cyc_Absyn_get_known_aggrdecl(_tmp2F9);
({int _tmp860=Cyc_Toc_get_member_offset(_tmp2F3,f);*f_tag=_tmp860;});{
# 1768
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F7=({struct Cyc_String_pa_PrintArg_struct _tmp744;_tmp744.tag=0U,_tmp744.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2F3->name).f2);_tmp744;});struct Cyc_String_pa_PrintArg_struct _tmp2F8=({struct Cyc_String_pa_PrintArg_struct _tmp743;_tmp743.tag=0U,_tmp743.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp743;});void*_tmp2F5[2U];_tmp2F5[0]=& _tmp2F7,_tmp2F5[1]=& _tmp2F8;({struct _dyneither_ptr _tmp861=({const char*_tmp2F6="_union_%s_%s";_tag_dyneither(_tmp2F6,sizeof(char),13U);});Cyc_aprintf(_tmp861,_tag_dyneither(_tmp2F5,sizeof(void*),2U));});});
({void*_tmp862=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));*_tmp2F4=str;_tmp2F4;}));*tagged_member_type=_tmp862;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2F3->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1779
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1782
void*_tmp2FA=e->r;void*_tmp2FB=_tmp2FA;struct Cyc_Absyn_Exp*_tmp307;struct _dyneither_ptr*_tmp306;int*_tmp305;struct Cyc_Absyn_Exp*_tmp304;struct _dyneither_ptr*_tmp303;int*_tmp302;struct Cyc_Absyn_Exp*_tmp301;switch(*((int*)_tmp2FB)){case 14U: _LL1: _tmp301=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FB)->f2;_LL2:
({void*_tmp2FC=0U;({struct _dyneither_ptr _tmp863=({const char*_tmp2FD="cast on lhs!";_tag_dyneither(_tmp2FD,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp863,_tag_dyneither(_tmp2FC,sizeof(void*),0U));});});case 21U: _LL3: _tmp304=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FB)->f1;_tmp303=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FB)->f2;_tmp302=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FB)->f4;_LL4:
# 1785
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp304->topt),_tmp303,f_tag,tagged_member_type,clear_read,_tmp302);case 22U: _LL5: _tmp307=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FB)->f1;_tmp306=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FB)->f2;_tmp305=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FB)->f4;_LL6: {
# 1788
void*_tmp2FE=Cyc_Tcutil_compress((void*)_check_null(_tmp307->topt));void*_tmp2FF=_tmp2FE;void*_tmp300;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->tag == 3U){_LLA: _tmp300=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->f1).elt_type;_LLB:
# 1790
 return Cyc_Toc_is_tagged_union_project_impl(_tmp300,_tmp306,f_tag,tagged_member_type,clear_read,_tmp305);}else{_LLC: _LLD:
# 1792
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1794
 return 0;}_LL0:;}
# 1807 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1811
struct _tuple1*_tmp308=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp308,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp864=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp864;});else{
# 1821
struct Cyc_Absyn_Exp*_tmp309=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp867=({struct Cyc_Absyn_Exp*_tmp866=_tmp309;struct Cyc_Absyn_Stmt*_tmp865=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp866,_tmp865,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp867;});}{
# 1824
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp86A=_tmp308;void*_tmp869=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp868=
Cyc_Toc_push_address_exp(e1);
# 1824
Cyc_Absyn_declare_stmt(_tmp86A,_tmp869,_tmp868,
# 1826
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple31{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple32{void*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1830
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp30C=({struct Cyc_String_pa_PrintArg_struct _tmp745;_tmp745.tag=0U,({struct _dyneither_ptr _tmp86B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp745.f1=_tmp86B;});_tmp745;});void*_tmp30A[1U];_tmp30A[0]=& _tmp30C;({unsigned int _tmp86D=e->loc;struct _dyneither_ptr _tmp86C=({const char*_tmp30B="exp_to_c: no type for %s";_tag_dyneither(_tmp30B,sizeof(char),25U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp86D,_tmp86C,_tag_dyneither(_tmp30A,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp30D=e->r;void*_tmp30E=_tmp30D;struct Cyc_Absyn_Stmt*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CC;struct _dyneither_ptr*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4C9;int _tmp4C8;struct Cyc_Absyn_Exp*_tmp4C7;void**_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C5;int _tmp4C4;int _tmp4C3;struct Cyc_List_List*_tmp4C2;struct Cyc_Absyn_Datatypedecl*_tmp4C1;struct Cyc_Absyn_Datatypefield*_tmp4C0;struct _tuple1**_tmp4BF;struct Cyc_List_List*_tmp4BE;struct Cyc_List_List*_tmp4BD;struct Cyc_Absyn_Aggrdecl*_tmp4BC;void*_tmp4BB;struct Cyc_List_List*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4B9;void*_tmp4B8;int _tmp4B7;struct Cyc_Absyn_Vardecl*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B4;int _tmp4B3;struct Cyc_List_List*_tmp4B2;struct Cyc_List_List*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;struct _dyneither_ptr*_tmp4AD;int _tmp4AC;int _tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct _dyneither_ptr*_tmp4A9;int _tmp4A8;int _tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;void*_tmp4A5;struct Cyc_List_List*_tmp4A4;void*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*_tmp49F;void**_tmp49E;struct Cyc_Absyn_Exp*_tmp49D;int _tmp49C;enum Cyc_Absyn_Coercion _tmp49B;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp497;int _tmp496;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_List_List*_tmp494;int _tmp493;struct Cyc_List_List*_tmp492;struct Cyc_Absyn_VarargInfo*_tmp491;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_List_List*_tmp48F;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Absyn_Exp*_tmp48A;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Exp*_tmp488;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Core_Opt*_tmp484;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Exp*_tmp482;enum Cyc_Absyn_Incrementor _tmp481;enum Cyc_Absyn_Primop _tmp480;struct Cyc_List_List*_tmp47F;struct Cyc_Absyn_Exp*_tmp47E;switch(*((int*)_tmp30E)){case 2U: _LL1: _LL2:
# 1836
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30E)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1843
struct Cyc_Absyn_Exp*_tmp30F=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp86E=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp30F,_tmp30F))->r;e->r=_tmp86E;});else{
# 1848
({void*_tmp870=({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp86F,({struct Cyc_Absyn_Exp*_tmp310[3U];_tmp310[0]=_tmp30F,_tmp310[1]=_tmp30F,_tmp310[2]=_tmp30F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp310,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp870;});}}else{
# 1850
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1852
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 41U: _LL9: _tmp47E=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_LLA:
 Cyc_Toc_exp_to_c(nv,_tmp47E);goto _LL0;case 3U: _LLB: _tmp480=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp47F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LLC: {
# 1858
struct Cyc_List_List*_tmp311=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp47F);
# 1860
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp47F);
{enum Cyc_Absyn_Primop _tmp312=_tmp480;switch(_tmp312){case Cyc_Absyn_Numelts: _LL5A: _LL5B: {
# 1863
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F))->hd;
{void*_tmp313=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp314=_tmp313;void*_tmp322;void*_tmp321;void*_tmp320;void*_tmp31F;struct Cyc_Absyn_Exp*_tmp31E;switch(*((int*)_tmp314)){case 4U: _LL6F: _tmp31E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp314)->f1).num_elts;_LL70:
# 1868
 e->r=((struct Cyc_Absyn_Exp*)_check_null(_tmp31E))->r;
goto _LL6E;case 3U: _LL71: _tmp322=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp314)->f1).elt_type;_tmp321=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp314)->f1).ptr_atts).nullable;_tmp320=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp314)->f1).ptr_atts).bounds;_tmp31F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp314)->f1).ptr_atts).zero_term;_LL72: {
# 1871
struct Cyc_Absyn_Exp*_tmp315=({void*_tmp871=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp871,_tmp320);});
if(_tmp315 == 0)
# 1874
({void*_tmp874=({struct Cyc_Absyn_Exp*_tmp873=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp873,({struct Cyc_Absyn_Exp*_tmp316[2U];_tmp316[0]=(struct Cyc_Absyn_Exp*)_tmp47F->hd,({
# 1876
struct Cyc_Absyn_Exp*_tmp872=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp322),0U);_tmp316[1]=_tmp872;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp316,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1874
e->r=_tmp874;});else{
# 1878
struct Cyc_Absyn_Exp*_tmp317=_tmp315;
# 1880
if(Cyc_Tcutil_force_type2bool(0,_tmp31F)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp47F->hd);
# 1884
({void*_tmp876=({struct Cyc_Absyn_Exp*_tmp875=function_e;Cyc_Toc_fncall_exp_r(_tmp875,({struct Cyc_Absyn_Exp*_tmp318[2U];_tmp318[0]=(struct Cyc_Absyn_Exp*)_tmp47F->hd,_tmp318[1]=_tmp317;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp318,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp876;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp321)){
if(!Cyc_Evexp_c_can_eval(_tmp317))
({void*_tmp319=0U;({unsigned int _tmp878=e->loc;struct _dyneither_ptr _tmp877=({const char*_tmp31A="can't calculate numelts";_tag_dyneither(_tmp31A,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp878,_tmp877,_tag_dyneither(_tmp319,sizeof(void*),0U));});});
# 1889
({void*_tmp87B=({struct Cyc_Absyn_Exp*_tmp87A=arg;struct Cyc_Absyn_Exp*_tmp879=_tmp317;Cyc_Toc_conditional_exp_r(_tmp87A,_tmp879,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp87B;});}else{
# 1891
e->r=_tmp317->r;goto _LL6E;}}}
# 1894
goto _LL6E;}default: _LL73: _LL74:
# 1896
({struct Cyc_String_pa_PrintArg_struct _tmp31D=({struct Cyc_String_pa_PrintArg_struct _tmp746;_tmp746.tag=0U,({
struct _dyneither_ptr _tmp87C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp746.f1=_tmp87C;});_tmp746;});void*_tmp31B[1U];_tmp31B[0]=& _tmp31D;({struct _dyneither_ptr _tmp87D=({const char*_tmp31C="numelts primop applied to non-array %s";_tag_dyneither(_tmp31C,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp87D,_tag_dyneither(_tmp31B,sizeof(void*),1U));});});}_LL6E:;}
# 1899
goto _LL59;}case Cyc_Absyn_Plus: _LL5C: _LL5D:
# 1904
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp323=0U;({struct _dyneither_ptr _tmp87E=({const char*_tmp324="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp324,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp87E,_tag_dyneither(_tmp323,sizeof(void*),0U));});});
{void*_tmp325=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp311))->hd);void*_tmp326=_tmp325;void*_tmp32C;void*_tmp32B;void*_tmp32A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->tag == 3U){_LL76: _tmp32C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).elt_type;_tmp32B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).ptr_atts).bounds;_tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).ptr_atts).zero_term;_LL77: {
# 1908
struct Cyc_Absyn_Exp*_tmp327=({void*_tmp87F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp87F,_tmp32B);});
if(_tmp327 == 0){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F->tl))->hd;
({void*_tmp882=({struct Cyc_Absyn_Exp*_tmp881=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp881,({struct Cyc_Absyn_Exp*_tmp328[3U];_tmp328[0]=e1,({
struct Cyc_Absyn_Exp*_tmp880=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp32C),0U);_tmp328[1]=_tmp880;}),_tmp328[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp328,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1912
e->r=_tmp882;});}else{
# 1915
if(Cyc_Tcutil_force_type2bool(0,_tmp32A)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F->tl))->hd;
({void*_tmp884=({struct Cyc_Absyn_Exp*_tmp883=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp883,({struct Cyc_Absyn_Exp*_tmp329[3U];_tmp329[0]=e1,_tmp329[1]=_tmp327,_tmp329[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp329,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp884;});}}
# 1921
goto _LL75;}}else{_LL78: _LL79:
 goto _LL75;}_LL75:;}
# 1924
goto _LL59;case Cyc_Absyn_Minus: _LL5E: _LL5F: {
# 1929
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp311))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp311->tl))->hd)){
({void*_tmp886=({struct Cyc_Absyn_Exp*_tmp885=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp885,Cyc_Toc_curr_sp);});e1->r=_tmp886;});
({void*_tmp888=({struct Cyc_Absyn_Exp*_tmp887=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp887,Cyc_Toc_curr_sp);});e2->r=_tmp888;});
({void*_tmp88A=({void*_tmp889=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp889;});e1->topt=_tmp88A;});
({void*_tmp88C=({struct Cyc_Absyn_Exp*_tmp88B=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp88B,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1937
e->r=_tmp88C;});}else{
# 1940
({void*_tmp890=({struct Cyc_Absyn_Exp*_tmp88F=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp88F,({struct Cyc_Absyn_Exp*_tmp32D[3U];_tmp32D[0]=e1,({
struct Cyc_Absyn_Exp*_tmp88E=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp32D[1]=_tmp88E;}),({
struct Cyc_Absyn_Exp*_tmp88D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp32D[2]=_tmp88D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32D,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1940
e->r=_tmp890;});}}
# 1944
goto _LL59;}case Cyc_Absyn_Eq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Neq: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gt: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Gte: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lt: _LL68: _LL69:
 goto _LL6B;case Cyc_Absyn_Lte: _LL6A: _LL6B: {
# 1952
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp311))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp311->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& elt_type)){
void*_tmp32E=({void*_tmp891=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp891,Cyc_Toc_mt_tq);});
({void*_tmp894=({void*_tmp893=_tmp32E;Cyc_Toc_cast_it_r(_tmp893,({struct Cyc_Absyn_Exp*_tmp892=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp892,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp894;});
e1->topt=_tmp32E;}
# 1962
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
void*_tmp32F=({void*_tmp895=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp895,Cyc_Toc_mt_tq);});
({void*_tmp898=({void*_tmp897=_tmp32F;Cyc_Toc_cast_it_r(_tmp897,({struct Cyc_Absyn_Exp*_tmp896=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp896,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp898;});
e2->topt=_tmp32F;}
# 1967
goto _LL59;}default: _LL6C: _LL6D:
 goto _LL59;}_LL59:;}
# 1970
goto _LL0;}case 5U: _LLD: _tmp482=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp481=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LLE: {
# 1972
void*e2_cyc_typ=(void*)_check_null(_tmp482->topt);
# 1981 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _dyneither_ptr incr_str=({const char*_tmp342="increment";_tag_dyneither(_tmp342,sizeof(char),10U);});
if(_tmp481 == Cyc_Absyn_PreDec  || _tmp481 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp899=({const char*_tmp330="decrement";_tag_dyneither(_tmp330,sizeof(char),10U);});incr_str=_tmp899;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp482,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp333=({struct Cyc_String_pa_PrintArg_struct _tmp747;_tmp747.tag=0U,_tmp747.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp747;});void*_tmp331[1U];_tmp331[0]=& _tmp333;({unsigned int _tmp89B=e->loc;struct _dyneither_ptr _tmp89A=({const char*_tmp332="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp332,sizeof(char),74U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp89B,_tmp89A,_tag_dyneither(_tmp331,sizeof(void*),1U));});});
# 1989
if(Cyc_Toc_is_tagged_union_project(_tmp482,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp334=_tmp481;switch(_tmp334){case Cyc_Absyn_PreInc: _LL7B: _LL7C:
 op=0U;goto _LL7A;case Cyc_Absyn_PreDec: _LL7D: _LL7E:
 op=2U;goto _LL7A;default: _LL7F: _LL80:
({struct Cyc_String_pa_PrintArg_struct _tmp337=({struct Cyc_String_pa_PrintArg_struct _tmp748;_tmp748.tag=0U,_tmp748.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp748;});void*_tmp335[1U];_tmp335[0]=& _tmp337;({unsigned int _tmp89D=e->loc;struct _dyneither_ptr _tmp89C=({const char*_tmp336="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp336,sizeof(char),58U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp89D,_tmp89C,_tag_dyneither(_tmp335,sizeof(void*),1U));});});}_LL7A:;}
# 1999
({void*_tmp89F=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->tag=4U,_tmp339->f1=_tmp482,({struct Cyc_Core_Opt*_tmp89E=({struct Cyc_Core_Opt*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338->v=(void*)op;_tmp338;});_tmp339->f2=_tmp89E;}),_tmp339->f3=one;_tmp339;});e->r=_tmp89F;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 2003
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp482);
Cyc_Toc_set_lhs(nv,0);{
# 2008
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp481 == Cyc_Absyn_PostInc  || _tmp481 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2014
if(_tmp481 == Cyc_Absyn_PreDec  || _tmp481 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp8A4=({struct Cyc_Absyn_Exp*_tmp8A3=fn_e;Cyc_Toc_fncall_exp_r(_tmp8A3,({struct Cyc_Absyn_Exp*_tmp33A[3U];({struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Toc_push_address_exp(_tmp482);_tmp33A[0]=_tmp8A2;}),({
struct Cyc_Absyn_Exp*_tmp8A1=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp33A[1]=_tmp8A1;}),({
struct Cyc_Absyn_Exp*_tmp8A0=Cyc_Absyn_signed_int_exp(change,0U);_tmp33A[2]=_tmp8A0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp33A,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2016
e->r=_tmp8A4;});}else{
# 2019
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2025
if(_tmp481 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp33B=Cyc_Toc_temp_var();
void*_tmp33C=({void*_tmp8A5=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp8A5,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp33D=Cyc_Toc_push_address_exp(_tmp482);
struct Cyc_Absyn_Exp*_tmp33E=({struct Cyc_Absyn_Exp*_tmp8A6=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp33B,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp8A6,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
({struct Cyc_Absyn_Exp*_tmp8A9=({struct Cyc_Absyn_Exp*_tmp8A8=({struct Cyc_Absyn_Exp*_tmp8A7=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp33B,0U),0U);Cyc_Absyn_neq_exp(_tmp8A7,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2032
Cyc_Absyn_and_exp(_tmp8A8,_tmp33E,0U);});_tmp33E=_tmp8A9;});{
# 2034
struct Cyc_Absyn_Stmt*_tmp33F=({struct Cyc_Absyn_Exp*_tmp8AB=_tmp33E;struct Cyc_Absyn_Stmt*_tmp8AA=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp33B,0U),0U),0U),0U);
# 2034
Cyc_Absyn_ifthenelse_stmt(_tmp8AB,_tmp8AA,
# 2036
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp8AD=({struct Cyc_Absyn_Stmt*_tmp8AC=_tmp33F;Cyc_Absyn_seq_stmt(_tmp8AC,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp33B,0U),0U),0U),0U);});_tmp33F=_tmp8AD;});
({void*_tmp8AE=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp33B,_tmp33C,_tmp33D,_tmp33F,0U),0U))->r;e->r=_tmp8AE;});};}else{
# 2041
struct Cyc_Toc_functionSet*_tmp340=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp340,_tmp482);
({void*_tmp8B2=({struct Cyc_Absyn_Exp*_tmp8B1=fn_e;Cyc_Toc_fncall_exp_r(_tmp8B1,({struct Cyc_Absyn_Exp*_tmp341[2U];({struct Cyc_Absyn_Exp*_tmp8B0=Cyc_Toc_push_address_exp(_tmp482);_tmp341[0]=_tmp8B0;}),({
struct Cyc_Absyn_Exp*_tmp8AF=Cyc_Absyn_signed_int_exp(1,0U);_tmp341[1]=_tmp8AF;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp341,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2043
e->r=_tmp8B2;});}}else{
# 2046
if(elt_typ == Cyc_Absyn_void_type  && !Cyc_Absyn_is_lvalue(_tmp482)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp482,0,Cyc_Toc_incr_lvalue,_tmp481);
e->r=_tmp482->r;}}}
# 2050
goto _LL0;};}case 4U: _LLF: _tmp485=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp484=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp483=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30E)->f3;_LL10: {
# 2069 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp485->topt);
void*e2_old_typ=(void*)_check_null(_tmp483->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp485,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp485);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp483);
({void*_tmp8B3=Cyc_Toc_tagged_union_assignop(_tmp485,e1_old_typ,_tmp484,_tmp483,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp8B3;});
# 2080
return;}{
# 2082
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp485,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp485,_tmp484,_tmp483,ptr_type,is_dyneither,elt_type);
# 2088
return;}{
# 2092
int e1_poly=Cyc_Toc_is_poly_project(_tmp485);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp485);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp483);{
# 2098
int done=0;
if(_tmp484 != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp343=(enum Cyc_Absyn_Primop)_tmp484->v;enum Cyc_Absyn_Primop _tmp344=_tmp343;switch(_tmp344){case Cyc_Absyn_Plus: _LL82: _LL83:
 change=_tmp483;goto _LL81;case Cyc_Absyn_Minus: _LL84: _LL85:
# 2106
({struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp483,0U);change=_tmp8B4;});goto _LL81;default: _LL86: _LL87:
({void*_tmp345=0U;({struct _dyneither_ptr _tmp8B5=({const char*_tmp346="bad t ? pointer arithmetic";_tag_dyneither(_tmp346,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8B5,_tag_dyneither(_tmp345,sizeof(void*),0U));});});}_LL81:;}
# 2109
done=1;{
# 2111
struct Cyc_Absyn_Exp*_tmp347=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp8B9=({struct Cyc_Absyn_Exp*_tmp8B8=_tmp347;Cyc_Toc_fncall_exp_r(_tmp8B8,({struct Cyc_Absyn_Exp*_tmp348[3U];({struct Cyc_Absyn_Exp*_tmp8B7=Cyc_Toc_push_address_exp(_tmp485);_tmp348[0]=_tmp8B7;}),({
struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp348[1]=_tmp8B6;}),_tmp348[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp348,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2112
e->r=_tmp8B9;});};}else{
# 2115
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2118
enum Cyc_Absyn_Primop _tmp349=(enum Cyc_Absyn_Primop)_tmp484->v;enum Cyc_Absyn_Primop _tmp34A=_tmp349;if(_tmp34A == Cyc_Absyn_Plus){_LL89: _LL8A:
# 2120
 done=1;
({void*_tmp8BB=({struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp485);Cyc_Toc_fncall_exp_r(_tmp8BA,({struct Cyc_Absyn_Exp*_tmp34B[2U];_tmp34B[0]=_tmp485,_tmp34B[1]=_tmp483;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34B,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8BB;});
goto _LL88;}else{_LL8B: _LL8C:
({void*_tmp34C=0U;({struct _dyneither_ptr _tmp8BC=({const char*_tmp34D="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp34D,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8BC,_tag_dyneither(_tmp34C,sizeof(void*),0U));});});}_LL88:;}}}
# 2127
if(!done){
# 2129
if(e1_poly)
({void*_tmp8BE=({void*_tmp8BD=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp8BD,Cyc_Absyn_new_exp(_tmp483->r,0U));});_tmp483->r=_tmp8BE;});
# 2135
if(!Cyc_Absyn_is_lvalue(_tmp485)){
({struct Cyc_Absyn_Exp*_tmp8BF=_tmp485;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple30*),struct _tuple30*f_env))Cyc_Toc_lvalue_assign)(_tmp8BF,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp34F=1;struct _tuple30*_tmp34E=_cycalloc(_check_times(_tmp34F,sizeof(struct _tuple30)));(_tmp34E[0]).f1=_tmp484,(_tmp34E[0]).f2=_tmp483;_tmp34E;}));});
e->r=_tmp485->r;}}
# 2140
goto _LL0;};};};}case 6U: _LL11: _tmp488=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp487=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp486=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30E)->f3;_LL12:
# 2142
 Cyc_Toc_exp_to_c(nv,_tmp488);
Cyc_Toc_exp_to_c(nv,_tmp487);
Cyc_Toc_exp_to_c(nv,_tmp486);
goto _LL0;case 7U: _LL13: _tmp48A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp489=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL14:
 _tmp48C=_tmp48A;_tmp48B=_tmp489;goto _LL16;case 8U: _LL15: _tmp48C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp48B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL16:
 _tmp48E=_tmp48C;_tmp48D=_tmp48B;goto _LL18;case 9U: _LL17: _tmp48E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp48D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL18:
# 2149
 Cyc_Toc_exp_to_c(nv,_tmp48E);
Cyc_Toc_exp_to_c(nv,_tmp48D);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30E)->f3 == 0){_LL19: _tmp490=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp48F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL1A: {
# 2153
void*e1_typ=(void*)_check_null(_tmp490->topt);
Cyc_Toc_exp_to_c(nv,_tmp490);
if(Cyc_Toc_do_null_check(e))
# 2157
({void*_tmp8C3=({void*_tmp8C2=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp8C2,({
struct Cyc_Absyn_Exp*_tmp8C1=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp8C1,({struct Cyc_List_List*_tmp350=_cycalloc(sizeof(*_tmp350));
({struct Cyc_Absyn_Exp*_tmp8C0=Cyc_Absyn_copy_exp(_tmp490);_tmp350->hd=_tmp8C0;}),_tmp350->tl=0;_tmp350;}),0U);}));});
# 2157
_tmp490->r=_tmp8C3;});
# 2160
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp48F);
goto _LL0;}}else{_LL1B: _tmp495=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp494=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp493=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30E)->f3)->num_varargs;_tmp492=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30E)->f3)->injectors;_tmp491=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30E)->f3)->vai;_LL1C: {
# 2171 "toc.cyc"
struct _RegionHandle _tmp351=_new_region("r");struct _RegionHandle*r=& _tmp351;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp493,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp491->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2179
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp494);
int num_normargs=num_args - _tmp493;
# 2183
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp494=_tmp494->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp494))->hd);
({struct Cyc_List_List*_tmp8C4=({struct Cyc_List_List*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->hd=(struct Cyc_Absyn_Exp*)_tmp494->hd,_tmp352->tl=new_args;_tmp352;});new_args=_tmp8C4;});}}
# 2188
({struct Cyc_List_List*_tmp8C8=({struct Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));({struct Cyc_Absyn_Exp*_tmp8C7=({struct Cyc_Absyn_Exp*_tmp8C6=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8C6,({struct Cyc_Absyn_Exp*_tmp353[3U];_tmp353[0]=argvexp,({
# 2190
struct Cyc_Absyn_Exp*_tmp8C5=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp353[1]=_tmp8C5;}),_tmp353[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp353,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2188
_tmp354->hd=_tmp8C7;}),_tmp354->tl=new_args;_tmp354;});new_args=_tmp8C8;});
# 2193
({struct Cyc_List_List*_tmp8C9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp8C9;});{
# 2195
void*e1_typ=(void*)_check_null(_tmp495->topt);
Cyc_Toc_exp_to_c(nv,_tmp495);
if(Cyc_Toc_do_null_check(e))
# 2199
({void*_tmp8CD=({void*_tmp8CC=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp8CC,({
struct Cyc_Absyn_Exp*_tmp8CB=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp8CB,({struct Cyc_List_List*_tmp355=_cycalloc(sizeof(*_tmp355));
({struct Cyc_Absyn_Exp*_tmp8CA=Cyc_Absyn_copy_exp(_tmp495);_tmp355->hd=_tmp8CA;}),_tmp355->tl=0;_tmp355;}),0U);}));});
# 2199
_tmp495->r=_tmp8CD;});{
# 2202
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp495,new_args,0U),0U);
# 2205
if(_tmp491->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp356=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp491->type));void*_tmp357=_tmp356;struct Cyc_Absyn_Datatypedecl*_tmp35A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp357)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp357)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp357)->f1)->f1).KnownDatatype).tag == 2){_LL8E: _tmp35A=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp357)->f1)->f1).KnownDatatype).val;_LL8F:
 tud=_tmp35A;goto _LL8D;}else{goto _LL90;}}else{goto _LL90;}}else{_LL90: _LL91:
({void*_tmp358=0U;({struct _dyneither_ptr _tmp8CE=({const char*_tmp359="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp359,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8CE,_tag_dyneither(_tmp358,sizeof(void*),0U));});});}_LL8D:;}{
# 2211
struct _dyneither_ptr vs=({unsigned int _tmp36B=(unsigned int)_tmp493;struct _tuple1**_tmp36A=({struct _RegionHandle*_tmp8CF=r;_region_malloc(_tmp8CF,_check_times(_tmp36B,sizeof(struct _tuple1*)));});({{unsigned int _tmp749=(unsigned int)_tmp493;unsigned int i;for(i=0;i < _tmp749;++ i){({struct _tuple1*_tmp8D0=Cyc_Toc_temp_var();_tmp36A[i]=_tmp8D0;});}}0;});_tag_dyneither(_tmp36A,sizeof(struct _tuple1*),_tmp36B);});
# 2213
if(_tmp493 != 0){
# 2217
struct Cyc_List_List*_tmp35B=0;
{int i=_tmp493 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp8D2=({struct Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));({struct Cyc_Absyn_Exp*_tmp8D1=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp35C->hd=_tmp8D1;}),_tmp35C->tl=_tmp35B;_tmp35C;});_tmp35B=_tmp8D2;});}}
({struct Cyc_Absyn_Stmt*_tmp8D6=({struct _tuple1*_tmp8D5=argv;void*_tmp8D4=arr_type;struct Cyc_Absyn_Exp*_tmp8D3=Cyc_Absyn_array_exp(_tmp35B,0U);Cyc_Absyn_declare_stmt(_tmp8D5,_tmp8D4,_tmp8D3,s,0U);});s=_tmp8D6;});
# 2222
({struct Cyc_List_List*_tmp8D7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp494);_tmp494=_tmp8D7;});
({struct Cyc_List_List*_tmp8D8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp492);_tmp492=_tmp8D8;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp494)- 1;
for(0;_tmp494 != 0;(_tmp494=_tmp494->tl,_tmp492=_tmp492->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp494->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp35D=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp492))->hd;struct Cyc_Absyn_Datatypefield*_tmp35E=_tmp35D;struct _tuple1*_tmp364;struct Cyc_List_List*_tmp363;_LL93: _tmp364=_tmp35E->name;_tmp363=_tmp35E->typs;_LL94:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp363))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp8D9=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp8D9;});{
# 2236
struct _tuple1*_tmp35F=Cyc_Toc_collapse_qvars(_tmp364,tud->name);
struct Cyc_List_List*_tmp360=({struct _tuple19*_tmp362[2U];({struct _tuple19*_tmp8DD=({struct _dyneither_ptr*_tmp8DC=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp8DC,Cyc_Toc_datatype_tag(tud,_tmp364));});_tmp362[0]=_tmp8DD;}),({
struct _tuple19*_tmp8DB=({struct _dyneither_ptr*_tmp8DA=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp8DA,arg);});_tmp362[1]=_tmp8DB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp362,sizeof(struct _tuple19*),2U));});
({struct Cyc_Absyn_Stmt*_tmp8E1=({struct _tuple1*_tmp8E0=var;void*_tmp8DF=Cyc_Absyn_strctq(_tmp35F);struct Cyc_Absyn_Exp*_tmp8DE=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp361=_cycalloc(sizeof(*_tmp361));_tmp361->tag=29U,_tmp361->f1=_tmp35F,_tmp361->f2=0,_tmp361->f3=_tmp360,_tmp361->f4=0;_tmp361;}),0U);
# 2239
Cyc_Absyn_declare_stmt(_tmp8E0,_tmp8DF,_tmp8DE,s,0U);});s=_tmp8E1;});};};}};}else{
# 2247
struct Cyc_List_List*_tmp365=({struct _tuple31*_tmp366[3U];({struct _tuple31*_tmp8EA=({struct _tuple31*_tmp367=_cycalloc(sizeof(*_tmp367));({struct _dyneither_ptr _tmp8E9=_tag_dyneither(0,0,0);_tmp367->f1=_tmp8E9;}),({struct Cyc_Absyn_Exp*_tmp8E8=Cyc_Absyn_uint_exp(0U,0U);_tmp367->f2=_tmp8E8;});_tmp367;});_tmp366[0]=_tmp8EA;}),({
struct _tuple31*_tmp8E7=({struct _tuple31*_tmp368=_cycalloc(sizeof(*_tmp368));({struct _dyneither_ptr _tmp8E6=_tag_dyneither(0,0,0);_tmp368->f1=_tmp8E6;}),({struct Cyc_Absyn_Exp*_tmp8E5=Cyc_Absyn_uint_exp(0U,0U);_tmp368->f2=_tmp8E5;});_tmp368;});_tmp366[1]=_tmp8E7;}),({
struct _tuple31*_tmp8E4=({struct _tuple31*_tmp369=_cycalloc(sizeof(*_tmp369));({struct _dyneither_ptr _tmp8E3=_tag_dyneither(0,0,0);_tmp369->f1=_tmp8E3;}),({struct Cyc_Absyn_Exp*_tmp8E2=Cyc_Absyn_uint_exp(0U,0U);_tmp369->f2=_tmp8E2;});_tmp369;});_tmp366[2]=_tmp8E4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp366,sizeof(struct _tuple31*),3U));});
({struct Cyc_Absyn_Stmt*_tmp8EE=({struct _tuple1*_tmp8ED=argv;void*_tmp8EC=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp8EB=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8ED,_tmp8EC,_tmp8EB,s,0U);});s=_tmp8EE;});}};}else{
# 2255
if(_tmp493 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp494 != 0;_tmp494=_tmp494->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp494->hd);
({struct Cyc_List_List*_tmp8EF=({struct Cyc_List_List*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->hd=(struct Cyc_Absyn_Exp*)_tmp494->hd,_tmp36C->tl=array_args;_tmp36C;});array_args=_tmp8EF;});}{
# 2261
struct Cyc_Absyn_Exp*_tmp36D=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp8F0=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp36D,s,0U);s=_tmp8F0;});};}else{
# 2264
({struct Cyc_Absyn_Stmt*_tmp8F4=({struct _tuple1*_tmp8F3=argv;void*_tmp8F2=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8F3,_tmp8F2,_tmp8F1,s,0U);});s=_tmp8F4;});}}
# 2267
({void*_tmp8F5=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8F5;});};};}
# 2269
_npop_handler(0U);goto _LL0;
# 2171
;_pop_region(r);}}case 11U: _LL1D: _tmp497=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp496=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL1E:
# 2272
 Cyc_Toc_exp_to_c(nv,_tmp497);{
struct Cyc_Absyn_Exp*_tmp36E=_tmp496?Cyc_Toc_newrethrow_exp(_tmp497): Cyc_Toc_newthrow_exp(_tmp497);
({void*_tmp8F7=({void*_tmp8F6=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp8F6,_tmp36E,0U);})->r;e->r=_tmp8F7;});
goto _LL0;};case 12U: _LL1F: _tmp498=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_LL20:
 Cyc_Toc_exp_to_c(nv,_tmp498);goto _LL0;case 13U: _LL21: _tmp49A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp499=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL22:
# 2278
 Cyc_Toc_exp_to_c(nv,_tmp49A);
# 2287 "toc.cyc"
for(0;_tmp499 != 0;_tmp499=_tmp499->tl){
enum Cyc_Absyn_KindQual _tmp36F=(Cyc_Tcutil_type_kind((void*)_tmp499->hd))->kind;
if(_tmp36F != Cyc_Absyn_EffKind  && _tmp36F != Cyc_Absyn_RgnKind){
{void*_tmp370=Cyc_Tcutil_compress((void*)_tmp499->hd);void*_tmp371=_tmp370;switch(*((int*)_tmp371)){case 2U: _LL96: _LL97:
 goto _LL99;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp371)->f1)->tag == 18U){_LL98: _LL99:
 continue;}else{goto _LL9A;}default: _LL9A: _LL9B:
# 2294
({void*_tmp8F9=({void*_tmp8F8=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8F8,_tmp49A,0U);})->r;e->r=_tmp8F9;});
goto _LL95;}_LL95:;}
# 2297
break;}}
# 2300
goto _LL0;case 14U: _LL23: _tmp49E=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp49D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp49C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30E)->f3;_tmp49B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30E)->f4;_LL24: {
# 2302
void*old_t2=(void*)_check_null(_tmp49D->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp49E;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp49E=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp49D);
# 2309
{struct _tuple32 _tmp372=({struct _tuple32 _tmp74B;({void*_tmp8FB=Cyc_Tcutil_compress(old_t2);_tmp74B.f1=_tmp8FB;}),({void*_tmp8FA=Cyc_Tcutil_compress(new_typ);_tmp74B.f2=_tmp8FA;});_tmp74B;});struct _tuple32 _tmp373=_tmp372;struct Cyc_Absyn_PtrInfo _tmp39A;struct Cyc_Absyn_PtrInfo _tmp399;struct Cyc_Absyn_PtrInfo _tmp398;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f1)->tag == 3U)switch(*((int*)_tmp373.f2)){case 3U: _LL9D: _tmp399=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f1)->f1;_tmp398=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f2)->f1;_LL9E: {
# 2311
int _tmp374=Cyc_Tcutil_force_type2bool(0,(_tmp399.ptr_atts).nullable);
int _tmp375=Cyc_Tcutil_force_type2bool(0,(_tmp398.ptr_atts).nullable);
struct Cyc_Absyn_Exp*_tmp376=({void*_tmp8FC=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8FC,(_tmp399.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp377=({void*_tmp8FD=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8FD,(_tmp398.ptr_atts).bounds);});
int _tmp378=Cyc_Tcutil_force_type2bool(0,(_tmp399.ptr_atts).zero_term);
int _tmp379=Cyc_Tcutil_force_type2bool(0,(_tmp398.ptr_atts).zero_term);
{struct _tuple33 _tmp37A=({struct _tuple33 _tmp74A;_tmp74A.f1=_tmp376,_tmp74A.f2=_tmp377;_tmp74A;});struct _tuple33 _tmp37B=_tmp37A;if(_tmp37B.f1 != 0){if(_tmp37B.f2 != 0){_LLA4: _LLA5:
# 2322
 if((_tmp374  && !_tmp375) && Cyc_Toc_do_null_check(e))
({void*_tmp900=({void*_tmp8FF=*_tmp49E;Cyc_Toc_cast_it_r(_tmp8FF,({struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp8FE,({struct Cyc_List_List*_tmp37C=_cycalloc(sizeof(*_tmp37C));
_tmp37C->hd=_tmp49D,_tmp37C->tl=0;_tmp37C;}),0U);}));});
# 2323
e->r=_tmp900;});else{
# 2325
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49D->r;}
# 2331
goto _LLA3;}else{_LLA6: _LLA7: {
# 2333
struct Cyc_Absyn_Exp*_tmp37D=(struct Cyc_Absyn_Exp*)_check_null(_tmp376);
struct _tuple13 _tmp37E=Cyc_Evexp_eval_const_uint_exp(_tmp37D);struct _tuple13 _tmp37F=_tmp37E;unsigned int _tmp38D;int _tmp38C;_LLAD: _tmp38D=_tmp37F.f1;_tmp38C=_tmp37F.f2;_LLAE:;
if(Cyc_Toc_is_toplevel(nv)){
# 2339
if((_tmp378  && !(_tmp398.elt_tq).real_const) && !_tmp379)
({struct Cyc_Absyn_Exp*_tmp902=({struct Cyc_Absyn_Exp*_tmp901=_tmp37D;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp901,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp37D=_tmp902;});
({void*_tmp903=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp37D,_tmp49D))->r;e->r=_tmp903;});}else{
# 2344
if(_tmp378){
# 2349
struct _tuple1*_tmp380=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp381=Cyc_Absyn_var_exp(_tmp380,0U);
struct Cyc_Absyn_Exp*arg3;
# 2354
{void*_tmp382=_tmp49D->r;void*_tmp383=_tmp382;struct Cyc_Absyn_Vardecl*_tmp386;struct Cyc_Absyn_Vardecl*_tmp385;switch(*((int*)_tmp383)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp383)->f1).Wstring_c).tag){case 8U: _LLB0: _LLB1:
 arg3=_tmp37D;goto _LLAF;case 9U: _LLB2: _LLB3:
 arg3=_tmp37D;goto _LLAF;default: goto _LLB8;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp383)->f1)){case 1U: _LLB4: _tmp385=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp383)->f1)->f1;_LLB5:
 _tmp386=_tmp385;goto _LLB7;case 4U: _LLB6: _tmp386=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp383)->f1)->f1;_LLB7:
# 2359
 if(!Cyc_Tcutil_is_array_type(_tmp386->type))
goto _LLB9;
arg3=_tmp37D;
goto _LLAF;default: goto _LLB8;}default: _LLB8: _LLB9:
# 2366
 if(_tmp38C  && _tmp38D != 1)
arg3=_tmp37D;else{
# 2369
({struct Cyc_Absyn_Exp*_tmp907=({struct Cyc_Absyn_Exp*_tmp906=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp49D);Cyc_Absyn_fncall_exp(_tmp906,({struct Cyc_Absyn_Exp*_tmp384[2U];({
struct Cyc_Absyn_Exp*_tmp905=({void*_tmp904=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp904,_tmp381);});_tmp384[0]=_tmp905;}),_tmp384[1]=_tmp37D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp384,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2369
arg3=_tmp907;});}
# 2371
goto _LLAF;}_LLAF:;}
# 2373
if(!_tmp379  && !(_tmp398.elt_tq).real_const)
# 2376
({struct Cyc_Absyn_Exp*_tmp909=({struct Cyc_Absyn_Exp*_tmp908=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp908,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp909;});{
# 2378
struct Cyc_Absyn_Exp*_tmp387=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp398.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp388=({struct Cyc_Absyn_Exp*_tmp90A=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp90A,({struct Cyc_Absyn_Exp*_tmp38A[3U];_tmp38A[0]=_tmp381,_tmp38A[1]=_tmp387,_tmp38A[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp38A,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp389=Cyc_Absyn_exp_stmt(_tmp388,0U);
({struct Cyc_Absyn_Stmt*_tmp90E=({struct _tuple1*_tmp90D=_tmp380;void*_tmp90C=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp90B=_tmp49D;Cyc_Absyn_declare_stmt(_tmp90D,_tmp90C,_tmp90B,_tmp389,0U);});_tmp389=_tmp90E;});
({void*_tmp90F=Cyc_Toc_stmt_exp_r(_tmp389);e->r=_tmp90F;});};}else{
# 2385
({void*_tmp912=({struct Cyc_Absyn_Exp*_tmp911=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp911,({struct Cyc_Absyn_Exp*_tmp38B[3U];_tmp38B[0]=_tmp49D,({
# 2387
struct Cyc_Absyn_Exp*_tmp910=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp398.elt_type),0U);_tmp38B[1]=_tmp910;}),_tmp38B[2]=_tmp37D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp38B,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2385
e->r=_tmp912;});}}
# 2391
goto _LLA3;}}}else{if(_tmp37B.f2 != 0){_LLA8: _LLA9: {
# 2393
struct Cyc_Absyn_Exp*_tmp38E=(struct Cyc_Absyn_Exp*)_check_null(_tmp377);
# 2402 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp38F=_tmp38E;
if(_tmp378  && !_tmp379)
({struct Cyc_Absyn_Exp*_tmp914=({struct Cyc_Absyn_Exp*_tmp913=_tmp38E;Cyc_Absyn_add_exp(_tmp913,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp38F=_tmp914;});{
# 2409
struct Cyc_Absyn_Exp*_tmp390=({struct Cyc_Absyn_Exp*_tmp916=Cyc_Toc__untag_dyneither_ptr_e;Cyc_Absyn_fncall_exp(_tmp916,({struct Cyc_Absyn_Exp*_tmp392[3U];_tmp392[0]=_tmp49D,({
# 2411
struct Cyc_Absyn_Exp*_tmp915=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp399.elt_type),0U);_tmp392[1]=_tmp915;}),_tmp392[2]=_tmp38F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp392,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2413
if(!_tmp375)
({void*_tmp919=({struct Cyc_Absyn_Exp*_tmp918=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp918,({struct Cyc_List_List*_tmp391=_cycalloc(sizeof(*_tmp391));
({struct Cyc_Absyn_Exp*_tmp917=Cyc_Absyn_copy_exp(_tmp390);_tmp391->hd=_tmp917;}),_tmp391->tl=0;_tmp391;}));});
# 2414
_tmp390->r=_tmp919;});
# 2416
({void*_tmp91A=Cyc_Toc_cast_it_r(*_tmp49E,_tmp390);e->r=_tmp91A;});
goto _LLA3;};}}else{_LLAA: _LLAB:
# 2421
 if((_tmp378  && !_tmp379) && !(_tmp398.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp393=0U;({struct _dyneither_ptr _tmp91B=({const char*_tmp394="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp394,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp91B,_tag_dyneither(_tmp393,sizeof(void*),0U));});});
({void*_tmp91F=({struct Cyc_Absyn_Exp*_tmp91E=Cyc_Toc__dyneither_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp91E,({struct Cyc_Absyn_Exp*_tmp395[3U];_tmp395[0]=_tmp49D,({
struct Cyc_Absyn_Exp*_tmp91D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp399.elt_type),0U);_tmp395[1]=_tmp91D;}),({
struct Cyc_Absyn_Exp*_tmp91C=Cyc_Absyn_uint_exp(1U,0U);_tmp395[2]=_tmp91C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp395,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2424
e->r=_tmp91F;});}
# 2428
goto _LLA3;}}_LLA3:;}
# 2430
goto _LL9C;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f2)->f1)->tag == 1U){_LL9F: _tmp39A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f1)->f1;_LLA0:
# 2432
{struct Cyc_Absyn_Exp*_tmp396=({void*_tmp920=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp920,(_tmp39A.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp397=_tmp396;if(_tmp397 == 0){_LLBB: _LLBC:
# 2434
({void*_tmp922=({struct Cyc_Absyn_Exp*_tmp921=Cyc_Absyn_new_exp(_tmp49D->r,_tmp49D->loc);Cyc_Toc_aggrmember_exp_r(_tmp921,Cyc_Toc_curr_sp);});_tmp49D->r=_tmp922;});
_tmp49D->topt=new_typ_c;
goto _LLBA;}else{_LLBD: _LLBE:
 goto _LLBA;}_LLBA:;}
# 2439
goto _LL9C;}else{goto _LLA1;}default: goto _LLA1;}else{_LLA1: _LLA2:
# 2441
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49D->r;
goto _LL9C;}_LL9C:;}
# 2445
goto _LL0;}case 15U: _LL25: _tmp49F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_LL26:
# 2448
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp49F);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp49F)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp49F,0,Cyc_Toc_address_lvalue,1);
# 2454
({void*_tmp924=({void*_tmp923=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp923,_tmp49F);});e->r=_tmp924;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp49F->topt))))
# 2458
({void*_tmp926=({void*_tmp925=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp925,_tmp49F);});e->r=_tmp926;});}
# 2460
goto _LL0;case 16U: _LL27: _tmp4A1=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4A0=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL28:
# 2467
 Cyc_Toc_exp_to_c(nv,_tmp4A0);{
# 2469
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4A0->topt));
struct _tuple1*_tmp39B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39C=Cyc_Absyn_var_exp(_tmp39B,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp39D=Cyc_Tcutil_compress(elt_typ);void*_tmp39E=_tmp39D;void*_tmp39F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->tag == 4U){_LLC0: _tmp39F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->f1).elt_type;_LLC1:
# 2475
({void*_tmp927=Cyc_Toc_typ_to_c(_tmp39F);elt_typ=_tmp927;});
({struct Cyc_Absyn_Exp*_tmp928=Cyc_Absyn_copy_exp(_tmp39C);lhs=_tmp928;});
goto _LLBF;}else{_LLC2: _LLC3:
# 2479
({struct Cyc_Absyn_Exp*_tmp929=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp39C),0U);lhs=_tmp929;});
goto _LLBF;}_LLBF:;}{
# 2482
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp4A0);
struct _tuple1*_tmp3A0=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp3A1=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp3A0),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp92C=({struct Cyc_Absyn_Exp*_tmp92B=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp92B,({struct Cyc_Absyn_Exp*_tmp3A2[2U];_tmp3A2[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp3A1),({
struct Cyc_Absyn_Exp*_tmp92A=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3A2[1]=_tmp92A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A2,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2487
mexp=_tmp92C;});else{
# 2490
({struct Cyc_Absyn_Exp*_tmp92D=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp39C,0U),0U);mexp=_tmp92D;});}{
# 2493
struct Cyc_Absyn_Exp*_tmp3A3=Cyc_Toc_get_varsizeexp(_tmp4A0);
if(_tmp3A3 != 0)
({struct Cyc_Absyn_Exp*_tmp92E=Cyc_Absyn_add_exp(mexp,_tmp3A3,0U);mexp=_tmp92E;});
# 2497
if(_tmp4A1 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp92F=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp92F;});else{
# 2500
struct Cyc_Absyn_Exp*r=_tmp4A1;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp930=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp930;});}{
# 2504
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp935=({struct Cyc_Absyn_Exp*_tmp934=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp934,({struct Cyc_Absyn_Exp*_tmp3A4[3U];({
struct Cyc_Absyn_Exp*_tmp933=Cyc_Absyn_copy_exp(_tmp39C);_tmp3A4[0]=_tmp933;}),({
struct Cyc_Absyn_Exp*_tmp932=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3A4[1]=_tmp932;}),
(unsigned int)_tmp3A1?_tmp3A4[2]=_tmp3A1:({struct Cyc_Absyn_Exp*_tmp931=Cyc_Absyn_uint_exp(1U,0U);_tmp3A4[2]=_tmp931;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A4,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2506
answer=_tmp935;});else{
# 2511
({struct Cyc_Absyn_Exp*_tmp936=Cyc_Absyn_copy_exp(_tmp39C);answer=_tmp936;});}
({void*_tmp937=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->tag=Cyc_Toc_Dest,_tmp3A5->f1=_tmp39C;_tmp3A5;});e->annot=_tmp937;});{
struct Cyc_Absyn_Stmt*_tmp3A6=({struct _tuple1*_tmp93B=_tmp39B;void*_tmp93A=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp939=mexp;Cyc_Absyn_declare_stmt(_tmp93B,_tmp93A,_tmp939,({
struct Cyc_Absyn_Stmt*_tmp938=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp938,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2517
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp93C=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp3A0),Cyc_Absyn_uint_type,array_len,_tmp3A6,0U);_tmp3A6=_tmp93C;});
({void*_tmp93D=Cyc_Toc_stmt_exp_r(_tmp3A6);e->r=_tmp93D;});
if(_tmp3A3 != 0)
({void*_tmp93F=({void*_tmp93E=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp93E,Cyc_Absyn_copy_exp(e));});e->r=_tmp93F;});
goto _LL0;};};};};};case 18U: _LL29: _tmp4A2=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_LL2A: {
# 2526
int _tmp3A7=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4A2);
Cyc_Toc_set_in_sizeof(nv,_tmp3A7);
goto _LL0;}case 17U: _LL2B: _tmp4A3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_LL2C:
({void*_tmp941=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->tag=17U,({void*_tmp940=Cyc_Toc_typ_to_c(_tmp4A3);_tmp3A8->f1=_tmp940;});_tmp3A8;});e->r=_tmp941;});goto _LL0;case 19U: _LL2D: _tmp4A5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4A4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL2E: {
# 2532
void*_tmp3A9=_tmp4A5;
struct Cyc_List_List*_tmp3AA=_tmp4A4;
for(0;_tmp3AA != 0;_tmp3AA=_tmp3AA->tl){
void*_tmp3AB=(void*)_tmp3AA->hd;void*_tmp3AC=_tmp3AB;unsigned int _tmp3BE;struct _dyneither_ptr*_tmp3BD;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3AC)->tag == 0U){_LLC5: _tmp3BD=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3AC)->f1;_LLC6:
 goto _LLC4;}else{_LLC7: _tmp3BE=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3AC)->f1;_LLC8:
# 2538
{void*_tmp3AD=Cyc_Tcutil_compress(_tmp3A9);void*_tmp3AE=_tmp3AD;struct Cyc_List_List*_tmp3BC;struct Cyc_List_List*_tmp3BB;struct Cyc_Absyn_Datatypefield*_tmp3BA;union Cyc_Absyn_AggrInfo _tmp3B9;switch(*((int*)_tmp3AE)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f1)){case 20U: _LLCA: _tmp3B9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f1)->f1;_LLCB: {
# 2540
struct Cyc_Absyn_Aggrdecl*_tmp3AF=Cyc_Absyn_get_known_aggrdecl(_tmp3B9);
if(_tmp3AF->impl == 0)
({void*_tmp3B0=0U;({struct _dyneither_ptr _tmp942=({const char*_tmp3B1="struct fields must be known";_tag_dyneither(_tmp3B1,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp942,_tag_dyneither(_tmp3B0,sizeof(void*),0U));});});
_tmp3BB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AF->impl))->fields;goto _LLCD;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f1)->f1).KnownDatatypefield).tag == 2){_LLD0: _tmp3BA=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f1)->f1).KnownDatatypefield).val).f2;_LLD1:
# 2554
 if(_tmp3BE == 0)
({void*_tmp943=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->tag=0U,_tmp3B5->f1=Cyc_Toc_tag_sp;_tmp3B5;}));_tmp3AA->hd=_tmp943;});else{
# 2557
({void*_tmp944=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3BA->typs,(int)(_tmp3BE - 1))).f2;_tmp3A9=_tmp944;});
({void*_tmp946=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->tag=0U,({struct _dyneither_ptr*_tmp945=Cyc_Absyn_fieldname((int)_tmp3BE);_tmp3B6->f1=_tmp945;});_tmp3B6;}));_tmp3AA->hd=_tmp946;});}
# 2560
goto _LLC9;}else{goto _LLD2;}default: goto _LLD2;}case 7U: _LLCC: _tmp3BB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3AE)->f2;_LLCD: {
# 2545
struct Cyc_Absyn_Aggrfield*_tmp3B2=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3BB,(int)_tmp3BE);
({void*_tmp947=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->tag=0U,_tmp3B3->f1=_tmp3B2->name;_tmp3B3;}));_tmp3AA->hd=_tmp947;});
_tmp3A9=_tmp3B2->type;
goto _LLC9;}case 6U: _LLCE: _tmp3BC=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3AE)->f1;_LLCF:
# 2550
({void*_tmp949=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4->tag=0U,({struct _dyneither_ptr*_tmp948=Cyc_Absyn_fieldname((int)(_tmp3BE + 1));_tmp3B4->f1=_tmp948;});_tmp3B4;}));_tmp3AA->hd=_tmp949;});
({void*_tmp94A=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3BC,(int)_tmp3BE)).f2;_tmp3A9=_tmp94A;});
goto _LLC9;default: _LLD2: _LLD3:
# 2561
({void*_tmp3B7=0U;({struct _dyneither_ptr _tmp94B=({const char*_tmp3B8="impossible type for offsetof tuple index";_tag_dyneither(_tmp3B8,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp94B,_tag_dyneither(_tmp3B7,sizeof(void*),0U));});});
goto _LLC9;}_LLC9:;}
# 2564
goto _LLC4;}_LLC4:;}
# 2567
({void*_tmp94D=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->tag=19U,({void*_tmp94C=Cyc_Toc_typ_to_c(_tmp4A5);_tmp3BF->f1=_tmp94C;}),_tmp3BF->f2=_tmp4A4;_tmp3BF;});e->r=_tmp94D;});
goto _LL0;}case 20U: _LL2F: _tmp4A6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_LL30:
# 2570
 Cyc_Toc_ptr_use_to_c(nv,_tmp4A6,e->annot,0);
goto _LL0;case 21U: _LL31: _tmp4AA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4A9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp4A8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30E)->f3;_tmp4A7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30E)->f4;_LL32: {
# 2573
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4AA->topt);
Cyc_Toc_exp_to_c(nv,_tmp4AA);
if(_tmp4A8  && _tmp4A7)
({void*_tmp952=({struct Cyc_Absyn_Exp*_tmp951=_tmp4AA;void*_tmp950=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp94F=e1_cyc_type;struct _dyneither_ptr*_tmp94E=_tmp4A9;Cyc_Toc_check_tagged_union(_tmp951,_tmp950,_tmp94F,_tmp94E,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2577
e->r=_tmp952;});
# 2579
if(is_poly)
({void*_tmp954=({void*_tmp953=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp953,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp954;});
goto _LL0;}case 22U: _LL33: _tmp4AE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4AD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp4AC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30E)->f3;_tmp4AB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30E)->f4;_LL34: {
# 2583
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(_tmp4AE->topt);
void*ta=Cyc_Toc_get_ptr_elt_type(e1typ);
Cyc_Toc_ptr_use_to_c(nv,_tmp4AE,e->annot,0);
if(_tmp4AC  && _tmp4AB)
({void*_tmp959=({struct Cyc_Absyn_Exp*_tmp958=_tmp4AE;void*_tmp957=Cyc_Toc_typ_to_c(e1typ);void*_tmp956=ta;struct _dyneither_ptr*_tmp955=_tmp4AD;Cyc_Toc_check_tagged_union(_tmp958,_tmp957,_tmp956,_tmp955,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp959;});
if(is_poly  && _tmp4AB)
({void*_tmp95B=({void*_tmp95A=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp95A,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp95B;});
goto _LL0;}case 23U: _LL35: _tmp4B0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4AF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL36: {
# 2593
int _tmp3C0=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);
{void*_tmp3C1=Cyc_Tcutil_compress((void*)_check_null(_tmp4B0->topt));void*_tmp3C2=_tmp3C1;void*_tmp3E2;struct Cyc_Absyn_Tqual _tmp3E1;void*_tmp3E0;void*_tmp3DF;void*_tmp3DE;void*_tmp3DD;struct Cyc_List_List*_tmp3DC;switch(*((int*)_tmp3C2)){case 6U: _LLD5: _tmp3DC=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3C2)->f1;_LLD6:
# 2597
 Cyc_Toc_exp_to_c(nv,_tmp4B0);
Cyc_Toc_exp_to_c(nv,_tmp4AF);{
# 2600
struct _tuple13 _tmp3C3=Cyc_Evexp_eval_const_uint_exp(_tmp4AF);struct _tuple13 _tmp3C4=_tmp3C3;unsigned int _tmp3C8;int _tmp3C7;_LLDC: _tmp3C8=_tmp3C4.f1;_tmp3C7=_tmp3C4.f2;_LLDD:;
if(!_tmp3C7)
({void*_tmp3C5=0U;({struct _dyneither_ptr _tmp95C=({const char*_tmp3C6="unknown tuple subscript in translation to C";_tag_dyneither(_tmp3C6,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp95C,_tag_dyneither(_tmp3C5,sizeof(void*),0U));});});
({void*_tmp95E=({struct Cyc_Absyn_Exp*_tmp95D=_tmp4B0;Cyc_Toc_aggrmember_exp_r(_tmp95D,Cyc_Absyn_fieldname((int)(_tmp3C8 + 1)));});e->r=_tmp95E;});
goto _LLD4;};case 3U: _LLD7: _tmp3E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).elt_type;_tmp3E1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).elt_tq;_tmp3E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).ptr_atts).rgn;_tmp3DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).ptr_atts).nullable;_tmp3DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).ptr_atts).bounds;_tmp3DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).ptr_atts).zero_term;_LLD8: {
# 2606
int possibly_null=Cyc_Toc_do_null_check(e);
int in_bnds=!Cyc_Toc_do_bounds_check(e);
Cyc_Toc_exp_to_c(nv,_tmp4B0);
Cyc_Toc_exp_to_c(nv,_tmp4AF);
{struct Cyc_Absyn_Exp*_tmp3C9=({void*_tmp95F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp95F,_tmp3DE);});struct Cyc_Absyn_Exp*_tmp3CA=_tmp3C9;struct Cyc_Absyn_Exp*_tmp3D9;if(_tmp3CA != 0){_LLDF: _tmp3D9=_tmp3CA;_LLE0: {
# 2612
struct Cyc_Absyn_Exp*_tmp3CB=(struct Cyc_Absyn_Exp*)_check_null(_tmp3D9);
void*ta1=Cyc_Toc_typ_to_c(_tmp3E2);
void*ta2=Cyc_Absyn_cstar_type(ta1,_tmp3E1);
struct _tuple13 _tmp3CC=Cyc_Evexp_eval_const_uint_exp(_tmp3CB);struct _tuple13 _tmp3CD=_tmp3CC;unsigned int _tmp3D7;int _tmp3D6;_LLE4: _tmp3D7=_tmp3CD.f1;_tmp3D6=_tmp3CD.f2;_LLE5:;
if(in_bnds){
if(possibly_null)
({void*_tmp963=({void*_tmp962=ta2;Cyc_Toc_cast_it_r(_tmp962,({struct Cyc_Absyn_Exp*_tmp961=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp961,({struct Cyc_Absyn_Exp*_tmp3CE[1U];({struct Cyc_Absyn_Exp*_tmp960=
Cyc_Absyn_copy_exp(_tmp4B0);_tmp3CE[0]=_tmp960;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3CE,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2618
_tmp4B0->r=_tmp963;});}else{
# 2622
if((_tmp3D6  && _tmp3D7 == 1) && Cyc_Tcutil_force_type2bool(0,_tmp3DD)){
# 2624
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4B0);
({void*_tmp966=Cyc_Toc_deref_exp_r(({void*_tmp965=ta2;Cyc_Toc_cast_it(_tmp965,({
struct Cyc_Absyn_Exp*_tmp964=function_e;Cyc_Absyn_fncall_exp(_tmp964,({struct Cyc_Absyn_Exp*_tmp3CF[3U];_tmp3CF[0]=_tmp4B0,_tmp3CF[1]=_tmp3CB,_tmp3CF[2]=_tmp4AF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3CF,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2625
e->r=_tmp966;});}else{
# 2628
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp3CB))
({void*_tmp3D0=0U;({unsigned int _tmp968=e->loc;struct _dyneither_ptr _tmp967=({const char*_tmp3D1="cannot determine subscript is in bounds";_tag_dyneither(_tmp3D1,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp968,_tmp967,_tag_dyneither(_tmp3D0,sizeof(void*),0U));});});
# 2632
({void*_tmp96C=Cyc_Toc_deref_exp_r(({void*_tmp96B=ta2;Cyc_Toc_cast_it(_tmp96B,({
struct Cyc_Absyn_Exp*_tmp96A=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp96A,({struct Cyc_Absyn_Exp*_tmp3D2[4U];_tmp3D2[0]=_tmp4B0,_tmp3D2[1]=_tmp3CB,({
# 2635
struct Cyc_Absyn_Exp*_tmp969=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp3D2[2]=_tmp969;}),_tmp3D2[3]=_tmp4AF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3D2,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 2632
e->r=_tmp96C;});}else{
# 2638
if(!Cyc_Evexp_c_can_eval(_tmp3CB))
({void*_tmp3D3=0U;({unsigned int _tmp96E=e->loc;struct _dyneither_ptr _tmp96D=({const char*_tmp3D4="cannot determine subscript is in bounds";_tag_dyneither(_tmp3D4,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp96E,_tmp96D,_tag_dyneither(_tmp3D3,sizeof(void*),0U));});});
# 2641
({void*_tmp971=({struct Cyc_Absyn_Exp*_tmp970=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmp970,({struct Cyc_Absyn_Exp*_tmp3D5[2U];_tmp3D5[0]=_tmp3CB,({
struct Cyc_Absyn_Exp*_tmp96F=Cyc_Absyn_copy_exp(_tmp4AF);_tmp3D5[1]=_tmp96F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3D5,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2641
_tmp4AF->r=_tmp971;});}}}
# 2644
goto _LLDE;}}else{_LLE1: _LLE2: {
# 2646
void*ta1=Cyc_Toc_typ_to_c(_tmp3E2);
if(in_bnds)
# 2650
({void*_tmp974=({struct Cyc_Absyn_Exp*_tmp973=({void*_tmp972=Cyc_Absyn_cstar_type(ta1,_tmp3E1);Cyc_Toc_cast_it(_tmp972,
Cyc_Toc_member_exp(_tmp4B0,Cyc_Toc_curr_sp,0U));});
# 2650
Cyc_Toc_subscript_exp_r(_tmp973,_tmp4AF);});e->r=_tmp974;});else{
# 2654
({void*_tmp978=Cyc_Toc_deref_exp_r(({void*_tmp977=Cyc_Absyn_cstar_type(ta1,_tmp3E1);Cyc_Toc_cast_it(_tmp977,({
struct Cyc_Absyn_Exp*_tmp976=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp976,({struct Cyc_Absyn_Exp*_tmp3D8[3U];_tmp3D8[0]=_tmp4B0,({
struct Cyc_Absyn_Exp*_tmp975=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp3D8[1]=_tmp975;}),_tmp3D8[2]=_tmp4AF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3D8,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2654
e->r=_tmp978;});}
# 2658
goto _LLDE;}}_LLDE:;}
# 2660
goto _LLD4;}default: _LLD9: _LLDA:
({void*_tmp3DA=0U;({struct _dyneither_ptr _tmp979=({const char*_tmp3DB="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp3DB,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp979,_tag_dyneither(_tmp3DA,sizeof(void*),0U));});});}_LLD4:;}
# 2663
Cyc_Toc_set_lhs(nv,_tmp3C0);
goto _LL0;}case 24U: _LL37: _tmp4B1=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_LL38:
# 2666
 if(!Cyc_Toc_is_toplevel(nv)){
# 2668
void*_tmp3E3=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4B1));
{void*_tmp3E4=_tmp3E3;union Cyc_Absyn_AggrInfo _tmp3E9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->f2 == 0){_LLE7: _tmp3E9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->f1)->f1;_LLE8: {
# 2671
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3E9);
{int i=1;for(0;_tmp4B1 != 0;(_tmp4B1=_tmp4B1->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4B1->hd);
({struct Cyc_List_List*_tmp97C=({struct Cyc_List_List*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));({struct _tuple19*_tmp97B=({struct _dyneither_ptr*_tmp97A=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp97A,(struct Cyc_Absyn_Exp*)_tmp4B1->hd);});_tmp3E5->hd=_tmp97B;}),_tmp3E5->tl=dles;_tmp3E5;});dles=_tmp97C;});}}
# 2677
({void*_tmp97E=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->tag=29U,_tmp3E6->f1=sd->name,_tmp3E6->f2=0,({struct Cyc_List_List*_tmp97D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp3E6->f3=_tmp97D;}),_tmp3E6->f4=sd;_tmp3E6;});e->r=_tmp97E;});
e->topt=_tmp3E3;
goto _LLE6;}}else{goto _LLE9;}}else{goto _LLE9;}}else{_LLE9: _LLEA:
({void*_tmp3E7=0U;({struct _dyneither_ptr _tmp97F=({const char*_tmp3E8="tuple type not an aggregate";_tag_dyneither(_tmp3E8,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp97F,_tag_dyneither(_tmp3E7,sizeof(void*),0U));});});}_LLE6:;}
# 2682
goto _LL0;}else{
# 2686
struct Cyc_List_List*dles=0;
for(0;_tmp4B1 != 0;_tmp4B1=_tmp4B1->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4B1->hd);
({struct Cyc_List_List*_tmp981=({struct Cyc_List_List*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));({struct _tuple19*_tmp980=({struct _tuple19*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->f1=0,_tmp3EA->f2=(struct Cyc_Absyn_Exp*)_tmp4B1->hd;_tmp3EA;});_tmp3EB->hd=_tmp980;}),_tmp3EB->tl=dles;_tmp3EB;});dles=_tmp981;});}
# 2691
({void*_tmp982=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp982;});}
# 2693
goto _LL0;case 26U: _LL39: _tmp4B2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_LL3A:
# 2699
{struct Cyc_List_List*_tmp3EC=_tmp4B2;for(0;_tmp3EC != 0;_tmp3EC=_tmp3EC->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3EC->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp983=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4B2);e->r=_tmp983;});
goto _LL0;case 27U: _LL3B: _tmp4B6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4B5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp4B4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30E)->f3;_tmp4B3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30E)->f4;_LL3C:
# 2707
 Cyc_Toc_exp_to_c(nv,_tmp4B5);
Cyc_Toc_exp_to_c(nv,_tmp4B4);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp3ED=Cyc_Evexp_eval_const_uint_exp(_tmp4B5);struct _tuple13 _tmp3EE=_tmp3ED;unsigned int _tmp3F8;int _tmp3F7;_LLEC: _tmp3F8=_tmp3EE.f1;_tmp3F7=_tmp3EE.f2;_LLED:;{
void*_tmp3EF=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4B4->topt));
struct Cyc_List_List*es=0;
# 2714
if(!Cyc_Toc_is_zero(_tmp4B4)){
if(!_tmp3F7)
({void*_tmp3F0=0U;({unsigned int _tmp985=_tmp4B5->loc;struct _dyneither_ptr _tmp984=({const char*_tmp3F1="cannot determine value of constant";_tag_dyneither(_tmp3F1,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp985,_tmp984,_tag_dyneither(_tmp3F0,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp3F8;++ i){
({struct Cyc_List_List*_tmp987=({struct Cyc_List_List*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));({struct _tuple19*_tmp986=({struct _tuple19*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2->f1=0,_tmp3F2->f2=_tmp4B4;_tmp3F2;});_tmp3F3->hd=_tmp986;}),_tmp3F3->tl=es;_tmp3F3;});es=_tmp987;});}}
# 2720
if(_tmp4B3){
struct Cyc_Absyn_Exp*_tmp3F4=({void*_tmp988=_tmp3EF;Cyc_Toc_cast_it(_tmp988,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp98B=({struct Cyc_List_List*_tmp98A=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp98A,({struct Cyc_List_List*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));({struct _tuple19*_tmp989=({struct _tuple19*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->f1=0,_tmp3F5->f2=_tmp3F4;_tmp3F5;});_tmp3F6->hd=_tmp989;}),_tmp3F6->tl=0;_tmp3F6;}));});es=_tmp98B;});}}
# 2725
({void*_tmp98C=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp98C;});};}
# 2727
goto _LL0;case 28U: _LL3D: _tmp4B9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4B8=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp4B7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30E)->f3;_LL3E:
# 2734
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp98D=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp98D;});else{
# 2737
Cyc_Toc_exp_to_c(nv,_tmp4B9);}
goto _LL0;case 30U: _LL3F: _tmp4BB=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4BA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL40:
# 2741
{struct Cyc_List_List*_tmp3F9=_tmp4BA;for(0;_tmp3F9 != 0;_tmp3F9=_tmp3F9->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3F9->hd)).f2);}}{
void*_tmp3FA=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp3FA;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3FB=Cyc_Tcutil_compress(_tmp3FA);void*_tmp3FC=_tmp3FB;union Cyc_Absyn_AggrInfo _tmp400;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FC)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FC)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FC)->f2 == 0){_LLEF: _tmp400=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FC)->f1)->f1;_LLF0: {
# 2748
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp400);
({void*_tmp98E=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->tag=29U,_tmp3FD->f1=sd->name,_tmp3FD->f2=0,_tmp3FD->f3=_tmp4BA,_tmp3FD->f4=sd;_tmp3FD;});e->r=_tmp98E;});
e->topt=_tmp3FA;
goto _LLEE;}}else{goto _LLF1;}}else{goto _LLF1;}}else{_LLF1: _LLF2:
({void*_tmp3FE=0U;({struct _dyneither_ptr _tmp98F=({const char*_tmp3FF="anonStruct type not an aggregate";_tag_dyneither(_tmp3FF,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98F,_tag_dyneither(_tmp3FE,sizeof(void*),0U));});});}_LLEE:;}else{
# 2755
({void*_tmp990=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4BA);e->r=_tmp990;});}
goto _LL0;};case 29U: _LL41: _tmp4BF=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4BE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp4BD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30E)->f3;_tmp4BC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30E)->f4;_LL42:
# 2761
 if(_tmp4BC == 0  || _tmp4BC->impl == 0)
({void*_tmp401=0U;({struct _dyneither_ptr _tmp991=({const char*_tmp402="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp402,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp991,_tag_dyneither(_tmp401,sizeof(void*),0U));});});{
void*_tmp403=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp404=Cyc_Tcutil_compress(_tmp403);void*_tmp405=_tmp404;union Cyc_Absyn_AggrInfo _tmp408;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp405)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp405)->f1)->tag == 20U){_LLF4: _tmp408=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp405)->f1)->f1;_LLF5:
({struct _tuple1*_tmp992=(Cyc_Absyn_aggr_kinded_name(_tmp408)).f2;*_tmp4BF=_tmp992;});goto _LLF3;}else{goto _LLF6;}}else{_LLF6: _LLF7:
({void*_tmp406=0U;({struct _dyneither_ptr _tmp993=({const char*_tmp407="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp407,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp993,_tag_dyneither(_tmp406,sizeof(void*),0U));});});}_LLF3:;}{
# 2772
struct Cyc_List_List*_tmp409=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BC->impl))->fields;
if(_tmp409 == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp409))->tl != 0;_tmp409=_tmp409->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp40A=(struct Cyc_Absyn_Aggrfield*)_tmp409->hd;
struct Cyc_List_List*_tmp40B=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp4BD,_tmp4BC->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BC->impl))->fields);
# 2779
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp40C=Cyc_Tcutil_compress(old_typ);void*_tmp40D=_tmp40C;struct Cyc_List_List*_tmp439;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->f1)->tag == 20U){_LLF9: _tmp439=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->f2;_LLFA:
# 2783
{struct Cyc_List_List*_tmp40E=_tmp40B;for(0;_tmp40E != 0;_tmp40E=_tmp40E->tl){
struct _tuple34*_tmp40F=(struct _tuple34*)_tmp40E->hd;struct _tuple34*_tmp410=_tmp40F;struct Cyc_Absyn_Aggrfield*_tmp436;struct Cyc_Absyn_Exp*_tmp435;_LLFE: _tmp436=_tmp410->f1;_tmp435=_tmp410->f2;_LLFF:;{
void*_tmp411=_tmp435->topt;
Cyc_Toc_exp_to_c(nv,_tmp435);
# 2788
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp436->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp435->topt)))
({void*_tmp995=({void*_tmp994=Cyc_Toc_typ_to_c(_tmp436->type);Cyc_Toc_cast_it(_tmp994,
Cyc_Absyn_copy_exp(_tmp435));})->r;
# 2790
_tmp435->r=_tmp995;});
# 2793
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BC->impl))->tagged){
struct _dyneither_ptr*_tmp412=_tmp436->name;
struct Cyc_Absyn_Exp*_tmp413=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp4BC,_tmp412),0U);
struct _tuple19*_tmp414=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp413);
struct _tuple19*_tmp415=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp435);
struct _tuple1*s=({struct _tuple1*_tmp420=_cycalloc(sizeof(*_tmp420));({union Cyc_Absyn_Nmspace _tmp999=Cyc_Absyn_Abs_n(0,1);_tmp420->f1=_tmp999;}),({
struct _dyneither_ptr*_tmp998=({struct _dyneither_ptr*_tmp41F=_cycalloc(sizeof(*_tmp41F));({struct _dyneither_ptr _tmp997=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp41D=({struct Cyc_String_pa_PrintArg_struct _tmp74D;_tmp74D.tag=0U,_tmp74D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4BC->name).f2);_tmp74D;});struct Cyc_String_pa_PrintArg_struct _tmp41E=({struct Cyc_String_pa_PrintArg_struct _tmp74C;_tmp74C.tag=0U,_tmp74C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp412);_tmp74C;});void*_tmp41B[2U];_tmp41B[0]=& _tmp41D,_tmp41B[1]=& _tmp41E;({struct _dyneither_ptr _tmp996=({const char*_tmp41C="_union_%s_%s";_tag_dyneither(_tmp41C,sizeof(char),13U);});Cyc_aprintf(_tmp996,_tag_dyneither(_tmp41B,sizeof(void*),2U));});});*_tmp41F=_tmp997;});_tmp41F;});_tmp420->f2=_tmp998;});_tmp420;});
# 2801
struct _tuple19*_tmp416=({
struct _dyneither_ptr*_tmp99B=_tmp412;Cyc_Toc_make_field(_tmp99B,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->tag=29U,_tmp41A->f1=s,_tmp41A->f2=0,({struct Cyc_List_List*_tmp99A=({struct _tuple19*_tmp419[2U];_tmp419[0]=_tmp414,_tmp419[1]=_tmp415;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp419,sizeof(struct _tuple19*),2U));});_tmp41A->f3=_tmp99A;}),_tmp41A->f4=0;_tmp41A;}),0U));});
# 2804
({void*_tmp99D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->tag=29U,_tmp418->f1=*_tmp4BF,_tmp418->f2=0,({struct Cyc_List_List*_tmp99C=({struct _tuple19*_tmp417[1U];_tmp417[0]=_tmp416;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp417,sizeof(struct _tuple19*),1U));});_tmp418->f3=_tmp99C;}),_tmp418->f4=_tmp4BC;_tmp418;});e->r=_tmp99D;});}
# 2810
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp40A == _tmp436){
struct Cyc_List_List*_tmp421=({struct Cyc_List_List*_tmp99E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BC->impl))->exist_vars,_tmp4BE);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp99E,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp4BC->tvs,_tmp439));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp422=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BC->impl))->fields;for(0;_tmp422 != 0;_tmp422=_tmp422->tl){
struct Cyc_Absyn_Aggrfield*_tmp423=(struct Cyc_Absyn_Aggrfield*)_tmp422->hd;
void*_tmp424=Cyc_Tcutil_substitute(_tmp421,_tmp423->type);
struct Cyc_Absyn_Aggrfield*_tmp425=({struct Cyc_Absyn_Aggrfield*_tmp99F=_tmp423;Cyc_Toc_aggrfield_to_c(_tmp99F,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp421,_tmp424)));});
({struct Cyc_List_List*_tmp9A0=({struct Cyc_List_List*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426->hd=_tmp425,_tmp426->tl=new_fields;_tmp426;});new_fields=_tmp9A0;});
# 2825
if(_tmp422->tl == 0){
{void*_tmp427=Cyc_Tcutil_compress(_tmp425->type);void*_tmp428=_tmp427;struct Cyc_Absyn_ArrayInfo _tmp42D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp428)->tag == 4U){_LL101: _tmp42D=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp428)->f1;_LL102:
# 2828
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp42D.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp429=_tmp42D;
({struct Cyc_Absyn_Exp*_tmp9A1=Cyc_Absyn_uint_exp(0U,0U);_tmp429.num_elts=_tmp9A1;});
({void*_tmp9A2=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A->tag=4U,_tmp42A->f1=_tmp429;_tmp42A;});_tmp425->type=_tmp9A2;});}
# 2833
goto _LL100;}else{_LL103: _LL104:
# 2837
 if(_tmp435->topt == 0)
goto _LL100;
{void*_tmp42B=Cyc_Tcutil_compress((void*)_check_null(_tmp435->topt));void*_tmp42C=_tmp42B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42C)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42C)->f1)->tag == 20U){_LL106: _LL107:
# 2841
 _tmp425->type=(void*)_check_null(_tmp435->topt);goto _LL105;}else{goto _LL108;}}else{_LL108: _LL109:
 goto _LL105;}_LL105:;}
# 2844
goto _LL100;}_LL100:;}
# 2848
if(!Cyc_Tcutil_is_array_type(_tmp423->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp423->type)))
({struct Cyc_List_List*_tmp9A4=({struct Cyc_List_List*_tmp42F=_cycalloc(sizeof(*_tmp42F));({void*_tmp9A3=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E->tag=6U,_tmp42E->f1=0;_tmp42E;});_tmp42F->hd=_tmp9A3;}),_tmp42F->tl=_tmp425->attributes;_tmp42F;});_tmp425->attributes=_tmp9A4;});}}}
# 2854
({struct Cyc_Absyn_Aggrdecl*_tmp9A8=({struct _dyneither_ptr*_tmp9A7=({struct _dyneither_ptr*_tmp433=_cycalloc(sizeof(*_tmp433));({struct _dyneither_ptr _tmp9A6=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp432=({struct Cyc_Int_pa_PrintArg_struct _tmp74E;_tmp74E.tag=1U,_tmp74E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp74E;});void*_tmp430[1U];_tmp430[0]=& _tmp432;({struct _dyneither_ptr _tmp9A5=({const char*_tmp431="_genStruct%d";_tag_dyneither(_tmp431,sizeof(char),13U);});Cyc_aprintf(_tmp9A5,_tag_dyneither(_tmp430,sizeof(void*),1U));});});*_tmp433=_tmp9A6;});_tmp433;});Cyc_Toc_make_c_struct_defn(_tmp9A7,
# 2856
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 2854
_tmp4BC=_tmp9A8;});
# 2857
*_tmp4BF=_tmp4BC->name;
Cyc_Toc_aggrdecl_to_c(_tmp4BC);
# 2860
({void*_tmp9A9=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp434=_cycalloc(sizeof(*_tmp434));*_tmp434=_tmp4BC;_tmp434;})),0);e->topt=_tmp9A9;});}};}}
# 2863
goto _LLF8;}else{goto _LLFB;}}else{_LLFB: _LLFC:
({void*_tmp437=0U;({struct _dyneither_ptr _tmp9AA=({const char*_tmp438="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp438,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9AA,_tag_dyneither(_tmp437,sizeof(void*),0U));});});}_LLF8:;}else{
# 2872
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BC->impl))->tagged){
# 2874
struct _tuple34*_tmp43A=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp40B))->hd;struct _tuple34*_tmp43B=_tmp43A;struct Cyc_Absyn_Aggrfield*_tmp444;struct Cyc_Absyn_Exp*_tmp443;_LL10B: _tmp444=_tmp43B->f1;_tmp443=_tmp43B->f2;_LL10C:;{
void*_tmp43C=(void*)_check_null(_tmp443->topt);
void*_tmp43D=_tmp444->type;
Cyc_Toc_exp_to_c(nv,_tmp443);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp43D) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp43C))
({void*_tmp9AC=({
void*_tmp9AB=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp9AB,Cyc_Absyn_new_exp(_tmp443->r,0U));});
# 2880
_tmp443->r=_tmp9AC;});{
# 2883
int i=Cyc_Toc_get_member_offset(_tmp4BC,_tmp444->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp43E=({struct _tuple19*_tmp440[2U];({struct _tuple19*_tmp9AE=({struct _tuple19*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->f1=0,_tmp441->f2=field_tag_exp;_tmp441;});_tmp440[0]=_tmp9AE;}),({struct _tuple19*_tmp9AD=({struct _tuple19*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->f1=0,_tmp442->f2=_tmp443;_tmp442;});_tmp440[1]=_tmp9AD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp440,sizeof(struct _tuple19*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp43E,0U);
({void*_tmp9B0=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp43F[1U];({struct _tuple19*_tmp9AF=Cyc_Toc_make_field(_tmp444->name,umem);_tmp43F[0]=_tmp9AF;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43F,sizeof(struct _tuple19*),1U));}));e->r=_tmp9B0;});};};}else{
# 2890
struct Cyc_List_List*_tmp445=0;
struct Cyc_List_List*_tmp446=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BC->impl))->fields;
for(0;_tmp446 != 0;_tmp446=_tmp446->tl){
struct Cyc_List_List*_tmp447=_tmp40B;for(0;_tmp447 != 0;_tmp447=_tmp447->tl){
if((*((struct _tuple34*)_tmp447->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp446->hd){
struct _tuple34*_tmp448=(struct _tuple34*)_tmp447->hd;struct _tuple34*_tmp449=_tmp448;struct Cyc_Absyn_Aggrfield*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;_LL10E: _tmp44F=_tmp449->f1;_tmp44E=_tmp449->f2;_LL10F:;{
void*_tmp44A=Cyc_Toc_typ_to_c(_tmp44F->type);
void*_tmp44B=Cyc_Toc_typ_to_c((void*)_check_null(_tmp44E->topt));
Cyc_Toc_exp_to_c(nv,_tmp44E);
# 2900
if(!Cyc_Tcutil_unify(_tmp44A,_tmp44B)){
# 2902
if(!Cyc_Tcutil_is_arithmetic_type(_tmp44A) || !
Cyc_Tcutil_is_arithmetic_type(_tmp44B))
({struct Cyc_Absyn_Exp*_tmp9B2=({void*_tmp9B1=_tmp44A;Cyc_Toc_cast_it(_tmp9B1,Cyc_Absyn_copy_exp(_tmp44E));});_tmp44E=_tmp9B2;});}
({struct Cyc_List_List*_tmp9B4=({struct Cyc_List_List*_tmp44D=_cycalloc(sizeof(*_tmp44D));({struct _tuple19*_tmp9B3=({struct _tuple19*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->f1=0,_tmp44C->f2=_tmp44E;_tmp44C;});_tmp44D->hd=_tmp9B3;}),_tmp44D->tl=_tmp445;_tmp44D;});_tmp445=_tmp9B4;});
break;};}}}
# 2909
({void*_tmp9B5=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp445));e->r=_tmp9B5;});}}
# 2912
goto _LL0;};};};case 31U: _LL43: _tmp4C2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4C1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_tmp4C0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30E)->f3;_LL44: {
# 2916
struct Cyc_List_List*_tmp450=_tmp4C0->typs;
{struct Cyc_List_List*_tmp451=_tmp4C2;for(0;_tmp451 != 0;(_tmp451=_tmp451->tl,_tmp450=_tmp450->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp451->hd;
void*_tmp452=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp450))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp452))
({void*_tmp9B6=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp9B6;});}}{
# 2927
struct Cyc_Absyn_Exp*_tmp453=_tmp4C1->is_extensible?Cyc_Absyn_var_exp(_tmp4C0->name,0U):
 Cyc_Toc_datatype_tag(_tmp4C1,_tmp4C0->name);
# 2930
if(!Cyc_Toc_is_toplevel(nv)){
# 2932
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4C2 != 0;(_tmp4C2=_tmp4C2->tl,++ i)){
({struct Cyc_List_List*_tmp9B9=({struct Cyc_List_List*_tmp454=_cycalloc(sizeof(*_tmp454));({struct _tuple19*_tmp9B8=({struct _dyneither_ptr*_tmp9B7=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9B7,(struct Cyc_Absyn_Exp*)_tmp4C2->hd);});_tmp454->hd=_tmp9B8;}),_tmp454->tl=dles;_tmp454;});dles=_tmp9B9;});}}{
# 2938
struct _tuple19*_tmp455=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp453);
({void*_tmp9BD=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->tag=29U,({struct _tuple1*_tmp9BC=Cyc_Toc_collapse_qvars(_tmp4C0->name,_tmp4C1->name);_tmp457->f1=_tmp9BC;}),_tmp457->f2=0,({
struct Cyc_List_List*_tmp9BB=({struct Cyc_List_List*_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456->hd=_tmp455,({struct Cyc_List_List*_tmp9BA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp456->tl=_tmp9BA;});_tmp456;});_tmp457->f3=_tmp9BB;}),_tmp457->f4=0;_tmp457;});
# 2939
e->r=_tmp9BD;});};}else{
# 2944
struct Cyc_List_List*_tmp458=0;
for(0;_tmp4C2 != 0;_tmp4C2=_tmp4C2->tl){
({struct Cyc_List_List*_tmp9BF=({struct Cyc_List_List*_tmp45A=_cycalloc(sizeof(*_tmp45A));({struct _tuple19*_tmp9BE=({struct _tuple19*_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459->f1=0,_tmp459->f2=(struct Cyc_Absyn_Exp*)_tmp4C2->hd;_tmp459;});_tmp45A->hd=_tmp9BE;}),_tmp45A->tl=_tmp458;_tmp45A;});_tmp458=_tmp9BF;});}
({void*_tmp9C2=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp45C=_cycalloc(sizeof(*_tmp45C));({struct _tuple19*_tmp9C1=({struct _tuple19*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->f1=0,_tmp45B->f2=_tmp453;_tmp45B;});_tmp45C->hd=_tmp9C1;}),({
struct Cyc_List_List*_tmp9C0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp458);_tmp45C->tl=_tmp9C0;});_tmp45C;}));
# 2947
e->r=_tmp9C2;});}
# 2950
goto _LL0;};}case 32U: _LL45: _LL46:
# 2952
 goto _LL48;case 33U: _LL47: _LL48:
 goto _LL0;case 34U: _LL49: _tmp4C8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30E)->f1).is_calloc;_tmp4C7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30E)->f1).rgn;_tmp4C6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30E)->f1).elt_type;_tmp4C5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30E)->f1).num_elts;_tmp4C4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30E)->f1).fat_result;_tmp4C3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30E)->f1).inline_call;_LL4A: {
# 2956
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4C6)));
Cyc_Toc_exp_to_c(nv,_tmp4C5);
if(_tmp4C4){
struct _tuple1*_tmp45D=Cyc_Toc_temp_var();
struct _tuple1*_tmp45E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp4C8){
xexp=_tmp4C5;
if(_tmp4C7 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4C7;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmp9C5=({struct Cyc_Absyn_Exp*_tmp9C4=rgn;struct Cyc_Absyn_Exp*_tmp9C3=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmp9C4,_tmp9C3,Cyc_Absyn_var_exp(_tmp45D,0U));});pexp=_tmp9C5;});}else{
# 2969
({struct Cyc_Absyn_Exp*_tmp9C8=({void*_tmp9C7=*_tmp4C6;struct Cyc_Absyn_Exp*_tmp9C6=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp9C7,_tmp9C6,Cyc_Absyn_var_exp(_tmp45D,0U));});pexp=_tmp9C8;});}
# 2971
({struct Cyc_Absyn_Exp*_tmp9CD=({struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp9CC,({struct Cyc_Absyn_Exp*_tmp45F[3U];({
struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Absyn_var_exp(_tmp45E,0U);_tmp45F[0]=_tmp9CB;}),({struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp45F[1]=_tmp9CA;}),({
struct Cyc_Absyn_Exp*_tmp9C9=Cyc_Absyn_var_exp(_tmp45D,0U);_tmp45F[2]=_tmp9C9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp45F,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2971
rexp=_tmp9CD;});}else{
# 2975
if(_tmp4C7 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4C7;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp4C3)
({struct Cyc_Absyn_Exp*_tmp9CF=({struct Cyc_Absyn_Exp*_tmp9CE=rgn;Cyc_Toc_rmalloc_inline_exp(_tmp9CE,Cyc_Absyn_var_exp(_tmp45D,0U));});pexp=_tmp9CF;});else{
# 2981
({struct Cyc_Absyn_Exp*_tmp9D1=({struct Cyc_Absyn_Exp*_tmp9D0=rgn;Cyc_Toc_rmalloc_exp(_tmp9D0,Cyc_Absyn_var_exp(_tmp45D,0U));});pexp=_tmp9D1;});}}else{
# 2983
({struct Cyc_Absyn_Exp*_tmp9D3=({void*_tmp9D2=*_tmp4C6;Cyc_Toc_malloc_exp(_tmp9D2,Cyc_Absyn_var_exp(_tmp45D,0U));});pexp=_tmp9D3;});}
# 2985
({struct Cyc_Absyn_Exp*_tmp9D8=({struct Cyc_Absyn_Exp*_tmp9D7=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp9D7,({struct Cyc_Absyn_Exp*_tmp460[3U];({struct Cyc_Absyn_Exp*_tmp9D6=Cyc_Absyn_var_exp(_tmp45E,0U);_tmp460[0]=_tmp9D6;}),({struct Cyc_Absyn_Exp*_tmp9D5=Cyc_Absyn_uint_exp(1U,0U);_tmp460[1]=_tmp9D5;}),({
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Absyn_var_exp(_tmp45D,0U);_tmp460[2]=_tmp9D4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp460,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2985
rexp=_tmp9D8;});}{
# 2988
struct Cyc_Absyn_Stmt*_tmp461=({struct _tuple1*_tmp9DE=_tmp45D;void*_tmp9DD=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp9DC=_tmp4C5;Cyc_Absyn_declare_stmt(_tmp9DE,_tmp9DD,_tmp9DC,({
struct _tuple1*_tmp9DB=_tmp45E;void*_tmp9DA=Cyc_Absyn_cstar_type(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp9D9=pexp;Cyc_Absyn_declare_stmt(_tmp9DB,_tmp9DA,_tmp9D9,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmp9DF=Cyc_Toc_stmt_exp_r(_tmp461);e->r=_tmp9DF;});};}else{
# 2993
if(_tmp4C8){
if(_tmp4C7 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4C7;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmp9E2=({struct Cyc_Absyn_Exp*_tmp9E1=rgn;struct Cyc_Absyn_Exp*_tmp9E0=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmp9E1,_tmp9E0,_tmp4C5);})->r;e->r=_tmp9E2;});}else{
# 2999
({void*_tmp9E5=({void*_tmp9E4=*_tmp4C6;struct Cyc_Absyn_Exp*_tmp9E3=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp9E4,_tmp9E3,_tmp4C5);})->r;e->r=_tmp9E5;});}}else{
# 3002
if(_tmp4C7 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4C7;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp4C3)
({void*_tmp9E6=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp4C5))->r;e->r=_tmp9E6;});else{
# 3008
({void*_tmp9E7=(Cyc_Toc_rmalloc_exp(rgn,_tmp4C5))->r;e->r=_tmp9E7;});}}else{
# 3010
({void*_tmp9E8=(Cyc_Toc_malloc_exp(*_tmp4C6,_tmp4C5))->r;e->r=_tmp9E8;});}}}
# 3014
goto _LL0;}case 35U: _LL4B: _tmp4CA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4C9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL4C: {
# 3023
void*e1_old_typ=(void*)_check_null(_tmp4CA->topt);
void*e2_old_typ=(void*)_check_null(_tmp4C9->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp462=0U;({struct _dyneither_ptr _tmp9E9=({const char*_tmp463="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp463,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E9,_tag_dyneither(_tmp462,sizeof(void*),0U));});});{
# 3031
unsigned int _tmp464=e->loc;
struct _tuple1*_tmp465=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_var_exp(_tmp465,_tmp464);_tmp466->topt=e1_old_typ;{
struct _tuple1*_tmp467=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp468=Cyc_Absyn_var_exp(_tmp467,_tmp464);_tmp468->topt=e2_old_typ;{
# 3037
struct Cyc_Absyn_Exp*_tmp469=({struct Cyc_Absyn_Exp*_tmp9EB=Cyc_Tcutil_deep_copy_exp(1,_tmp4CA);struct Cyc_Absyn_Exp*_tmp9EA=_tmp468;Cyc_Absyn_assign_exp(_tmp9EB,_tmp9EA,_tmp464);});_tmp469->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp46A=Cyc_Absyn_exp_stmt(_tmp469,_tmp464);
struct Cyc_Absyn_Exp*_tmp46B=({struct Cyc_Absyn_Exp*_tmp9ED=Cyc_Tcutil_deep_copy_exp(1,_tmp4C9);struct Cyc_Absyn_Exp*_tmp9EC=_tmp466;Cyc_Absyn_assign_exp(_tmp9ED,_tmp9EC,_tmp464);});_tmp46B->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp46C=Cyc_Absyn_exp_stmt(_tmp46B,_tmp464);
# 3042
struct Cyc_Absyn_Stmt*_tmp46D=({struct _tuple1*_tmp9F5=_tmp465;void*_tmp9F4=e1_old_typ;struct Cyc_Absyn_Exp*_tmp9F3=_tmp4CA;struct Cyc_Absyn_Stmt*_tmp9F2=({
struct _tuple1*_tmp9F1=_tmp467;void*_tmp9F0=e2_old_typ;struct Cyc_Absyn_Exp*_tmp9EF=_tmp4C9;struct Cyc_Absyn_Stmt*_tmp9EE=
Cyc_Absyn_seq_stmt(_tmp46A,_tmp46C,_tmp464);
# 3043
Cyc_Absyn_declare_stmt(_tmp9F1,_tmp9F0,_tmp9EF,_tmp9EE,_tmp464);});
# 3042
Cyc_Absyn_declare_stmt(_tmp9F5,_tmp9F4,_tmp9F3,_tmp9F2,_tmp464);});
# 3045
Cyc_Toc_stmt_to_c(nv,_tmp46D);
({void*_tmp9F6=Cyc_Toc_stmt_exp_r(_tmp46D);e->r=_tmp9F6;});
goto _LL0;};};};};};}case 38U: _LL4D: _tmp4CC=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_tmp4CB=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30E)->f2;_LL4E: {
# 3050
void*_tmp46E=Cyc_Tcutil_compress((void*)_check_null(_tmp4CC->topt));
Cyc_Toc_exp_to_c(nv,_tmp4CC);
{void*_tmp46F=_tmp46E;struct Cyc_Absyn_Aggrdecl*_tmp475;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46F)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46F)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46F)->f1)->f1).KnownAggr).tag == 2){_LL111: _tmp475=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46F)->f1)->f1).KnownAggr).val;_LL112: {
# 3054
struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp475,_tmp4CB),0U);
struct Cyc_Absyn_Exp*_tmp471=Cyc_Toc_member_exp(_tmp4CC,_tmp4CB,0U);
struct Cyc_Absyn_Exp*_tmp472=Cyc_Toc_member_exp(_tmp471,Cyc_Toc_tag_sp,0U);
({void*_tmp9F7=(Cyc_Absyn_eq_exp(_tmp472,_tmp470,0U))->r;e->r=_tmp9F7;});
goto _LL110;}}else{goto _LL113;}}else{goto _LL113;}}else{_LL113: _LL114:
({void*_tmp473=0U;({struct _dyneither_ptr _tmp9F8=({const char*_tmp474="non-aggregate type in tagcheck";_tag_dyneither(_tmp474,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F8,_tag_dyneither(_tmp473,sizeof(void*),0U));});});}_LL110:;}
# 3061
goto _LL0;}case 37U: _LL4F: _tmp4CD=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp30E)->f1;_LL50:
 Cyc_Toc_stmt_to_c(nv,_tmp4CD);goto _LL0;case 36U: _LL51: _LL52:
# 3064
({void*_tmp476=0U;({struct _dyneither_ptr _tmp9F9=({const char*_tmp477="UnresolvedMem";_tag_dyneither(_tmp477,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F9,_tag_dyneither(_tmp476,sizeof(void*),0U));});});case 25U: _LL53: _LL54:
({void*_tmp478=0U;({struct _dyneither_ptr _tmp9FA=({const char*_tmp479="compoundlit";_tag_dyneither(_tmp479,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp9FA,_tag_dyneither(_tmp478,sizeof(void*),0U));});});case 39U: _LL55: _LL56:
({void*_tmp47A=0U;({struct _dyneither_ptr _tmp9FB=({const char*_tmp47B="valueof(-)";_tag_dyneither(_tmp47B,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FB,_tag_dyneither(_tmp47A,sizeof(void*),0U));});});default: _LL57: _LL58:
({void*_tmp47C=0U;({struct _dyneither_ptr _tmp9FC=({const char*_tmp47D="__asm__";_tag_dyneither(_tmp47D,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FC,_tag_dyneither(_tmp47C,sizeof(void*),0U));});});}_LL0:;};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3095 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3097
return({struct _tuple35*_tmp4CE=_region_malloc(r,sizeof(*_tmp4CE));_tmp4CE->f1=0,({struct _dyneither_ptr*_tmp9FE=Cyc_Toc_fresh_label();_tmp4CE->f2=_tmp9FE;}),({struct _dyneither_ptr*_tmp9FD=Cyc_Toc_fresh_label();_tmp4CE->f3=_tmp9FD;}),_tmp4CE->f4=sc;_tmp4CE;});}
# 3102
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp4CF=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3108
if((int)(((_tmp4CF->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp4D2=_tmp4CF->orig_pat;if((_tmp4D2.pattern).tag != 1)_throw_match();(_tmp4D2.pattern).val;})->topt);
void*_tmp4D0=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp4D1=_tmp4D0;switch(*((int*)_tmp4D1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D1)->f1)){case 0U: _LL1: _LL2:
# 3113
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpA00=({void*_tmp9FF=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp9FF,v);});v=_tmpA00;});goto _LL0;}_LL0:;}{
# 3119
void*_tmp4D3=_tmp4CF->access;void*_tmp4D4=_tmp4D3;struct Cyc_Absyn_Datatypedecl*_tmp4DF;struct Cyc_Absyn_Datatypefield*_tmp4DE;unsigned int _tmp4DD;int _tmp4DC;struct _dyneither_ptr*_tmp4DB;unsigned int _tmp4DA;switch(*((int*)_tmp4D4)){case 0U: _LLA: _LLB:
# 3124
 goto _LL9;case 1U: _LLC: _LLD:
# 3129
 if(ps->tl != 0){
void*_tmp4D5=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp4D6=_tmp4D5;struct Cyc_Absyn_Datatypedecl*_tmp4D9;struct Cyc_Absyn_Datatypefield*_tmp4D8;unsigned int _tmp4D7;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4D6)->tag == 3U){_LL15: _tmp4D9=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4D6)->f1;_tmp4D8=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4D6)->f2;_tmp4D7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4D6)->f3;_LL16:
# 3132
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpA03=({void*_tmpA02=({void*_tmpA01=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4D8->name,_tmp4D9->name));Cyc_Absyn_cstar_type(_tmpA01,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA02,v);});v=_tmpA03;});
({struct Cyc_Absyn_Exp*_tmpA05=({struct Cyc_Absyn_Exp*_tmpA04=v;Cyc_Absyn_aggrarrow_exp(_tmpA04,Cyc_Absyn_fieldname((int)(_tmp4D7 + 1)),0U);});v=_tmpA05;});
continue;}else{_LL17: _LL18:
# 3137
 goto _LL14;}_LL14:;}
# 3140
({struct Cyc_Absyn_Exp*_tmpA06=Cyc_Absyn_deref_exp(v,0U);v=_tmpA06;});
goto _LL9;case 2U: _LLE: _tmp4DA=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4D4)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpA08=({struct Cyc_Absyn_Exp*_tmpA07=v;Cyc_Toc_member_exp(_tmpA07,Cyc_Absyn_fieldname((int)(_tmp4DA + 1)),0U);});v=_tmpA08;});goto _LL9;case 4U: _LL10: _tmp4DC=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4D4)->f1;_tmp4DB=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4D4)->f2;_LL11:
# 3144
({struct Cyc_Absyn_Exp*_tmpA09=Cyc_Toc_member_exp(v,_tmp4DB,0U);v=_tmpA09;});
if(_tmp4DC)
({struct Cyc_Absyn_Exp*_tmpA0A=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpA0A;});
goto _LL9;default: _LL12: _tmp4DF=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4D4)->f1;_tmp4DE=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4D4)->f2;_tmp4DD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4D4)->f3;_LL13:
# 3149
({struct Cyc_Absyn_Exp*_tmpA0C=({struct Cyc_Absyn_Exp*_tmpA0B=v;Cyc_Toc_member_exp(_tmpA0B,Cyc_Absyn_fieldname((int)(_tmp4DD + 1)),0U);});v=_tmpA0C;});
goto _LL9;}_LL9:;};}
# 3153
return v;}
# 3158
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4E0=t;struct Cyc_Absyn_Datatypedecl*_tmp4F9;struct Cyc_Absyn_Datatypefield*_tmp4F8;struct _dyneither_ptr*_tmp4F7;int _tmp4F6;int _tmp4F5;struct Cyc_Absyn_Datatypedecl*_tmp4F4;struct Cyc_Absyn_Datatypefield*_tmp4F3;unsigned int _tmp4F2;struct _dyneither_ptr _tmp4F1;int _tmp4F0;void*_tmp4EF;struct Cyc_Absyn_Enumfield*_tmp4EE;struct Cyc_Absyn_Enumdecl*_tmp4ED;struct Cyc_Absyn_Enumfield*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;switch(*((int*)_tmp4E0)){case 0U: _LL1: _tmp4EB=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4E0)->f1;_LL2:
# 3162
 if(_tmp4EB == 0)return v;else{return _tmp4EB;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA0D=v;Cyc_Absyn_eq_exp(_tmpA0D,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA0E=v;Cyc_Absyn_neq_exp(_tmpA0E,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4ED=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4E0)->f1;_tmp4EC=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4E0)->f2;_LL8:
# 3166
 return({struct Cyc_Absyn_Exp*_tmpA0F=v;Cyc_Absyn_eq_exp(_tmpA0F,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->tag=32U,_tmp4E1->f1=_tmp4ED,_tmp4E1->f2=_tmp4EC;_tmp4E1;}),0U),0U);});case 4U: _LL9: _tmp4EF=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4E0)->f1;_tmp4EE=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4E0)->f2;_LLA:
# 3168
 return({struct Cyc_Absyn_Exp*_tmpA10=v;Cyc_Absyn_eq_exp(_tmpA10,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->tag=33U,_tmp4E2->f1=_tmp4EF,_tmp4E2->f2=_tmp4EE;_tmp4E2;}),0U),0U);});case 5U: _LLB: _tmp4F1=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4E0)->f1;_tmp4F0=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4E0)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpA11=v;Cyc_Absyn_eq_exp(_tmpA11,Cyc_Absyn_float_exp(_tmp4F1,_tmp4F0,0U),0U);});case 6U: _LLD: _tmp4F2=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4E0)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpA12=v;Cyc_Absyn_eq_exp(_tmpA12,Cyc_Absyn_signed_int_exp((int)_tmp4F2,0U),0U);});case 7U: _LLF: _tmp4F5=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4E0)->f1;_tmp4F4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4E0)->f2;_tmp4F3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4E0)->f3;_LL10:
# 3174
 LOOP1: {
void*_tmp4E3=v->r;void*_tmp4E4=_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;switch(*((int*)_tmp4E4)){case 14U: _LL16: _tmp4E5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E4)->f2;_LL17:
 v=_tmp4E5;goto LOOP1;case 20U: _LL18: _tmp4E6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E4)->f1;_LL19:
 v=_tmp4E6;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3181
({struct Cyc_Absyn_Exp*_tmpA15=({void*_tmpA14=({void*_tmpA13=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4F3->name,_tmp4F4->name));Cyc_Absyn_cstar_type(_tmpA13,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA14,v);});v=_tmpA15;});
return({struct Cyc_Absyn_Exp*_tmpA16=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA16,Cyc_Absyn_uint_exp((unsigned int)_tmp4F5,0U),0U);});case 8U: _LL11: _tmp4F7=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4E0)->f1;_tmp4F6=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4E0)->f2;_LL12:
# 3186
({struct Cyc_Absyn_Exp*_tmpA18=({struct Cyc_Absyn_Exp*_tmpA17=Cyc_Toc_member_exp(v,_tmp4F7,0U);Cyc_Toc_member_exp(_tmpA17,Cyc_Toc_tag_sp,0U);});v=_tmpA18;});
return({struct Cyc_Absyn_Exp*_tmpA19=v;Cyc_Absyn_eq_exp(_tmpA19,Cyc_Absyn_signed_int_exp(_tmp4F6,0U),0U);});default: _LL13: _tmp4F9=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4E0)->f1;_tmp4F8=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4E0)->f2;_LL14:
# 3190
 LOOP2: {
void*_tmp4E7=v->r;void*_tmp4E8=_tmp4E7;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4E9;switch(*((int*)_tmp4E8)){case 14U: _LL1D: _tmp4E9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E8)->f2;_LL1E:
 v=_tmp4E9;goto LOOP2;case 20U: _LL1F: _tmp4EA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E8)->f1;_LL20:
 v=_tmp4EA;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3197
({struct Cyc_Absyn_Exp*_tmpA1C=({void*_tmpA1B=({void*_tmpA1A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4F8->name,_tmp4F9->name));Cyc_Absyn_cstar_type(_tmpA1A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA1B,v);});v=_tmpA1C;});
return({struct Cyc_Absyn_Exp*_tmpA1D=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA1D,Cyc_Absyn_var_exp(_tmp4F8->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3211
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3219
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3221
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple32 _tmp4FA=*((struct _tuple32*)ss->hd);struct _tuple32 _tmp4FB=_tmp4FA;void*_tmp501;_LL1: _tmp501=_tmp4FB.f1;_LL2:;{
void*_tmp4FC=_tmp501;struct Cyc_Absyn_Exp*_tmp500;struct _dyneither_ptr*_tmp4FF;switch(*((int*)_tmp4FC)){case 3U: _LL4: _LL5:
# 3227
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3230
 continue;case 8U: _LLA: _tmp4FF=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4FC)->f1;_LLB:
# 3232
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpA1E=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->tag=3U,_tmp4FD->f1=_tmp4FF;_tmp4FD;});k=_tmpA1E;});
continue;case 7U: _LLC: _LLD:
# 3236
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp500=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4FC)->f1;if(_tmp500 != 0){_LLF:
# 3240
({void*_tmpA1F=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->tag=2U,_tmp4FE->f1=_tmp500;_tmp4FE;});k=_tmpA1F;});
return({struct _tuple36 _tmp74F;_tmp74F.f1=0,_tmp74F.f2=k;_tmp74F;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp750;_tmp750.f1=0,_tmp750.f2=k;_tmp750;});}_LL3:;};}
# 3249
return({struct _tuple36 _tmp751;_tmp751.f1=c,_tmp751.f2=k;_tmp751;});}
# 3254
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp502=p;int _tmp50D;int _tmp50C;unsigned int _tmp50B;void*_tmp50A;struct Cyc_Absyn_Enumfield*_tmp509;struct Cyc_Absyn_Enumdecl*_tmp508;struct Cyc_Absyn_Enumfield*_tmp507;switch(*((int*)_tmp502)){case 3U: _LL1: _tmp508=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp502)->f1;_tmp507=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp502)->f2;_LL2:
# 3258
({struct Cyc_Absyn_Exp*_tmpA20=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503->tag=32U,_tmp503->f1=_tmp508,_tmp503->f2=_tmp507;_tmp503;}),0U);e=_tmpA20;});goto _LL0;case 4U: _LL3: _tmp50A=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp502)->f1;_tmp509=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp502)->f2;_LL4:
# 3260
({struct Cyc_Absyn_Exp*_tmpA21=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->tag=33U,_tmp504->f1=_tmp50A,_tmp504->f2=_tmp509;_tmp504;}),0U);e=_tmpA21;});goto _LL0;case 6U: _LL5: _tmp50B=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp502)->f1;_LL6:
 _tmp50C=(int)_tmp50B;goto _LL8;case 7U: _LL7: _tmp50C=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp502)->f1;_LL8:
 _tmp50D=_tmp50C;goto _LLA;case 8U: _LL9: _tmp50D=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp502)->f2;_LLA:
# 3264
({struct Cyc_Absyn_Exp*_tmpA22=Cyc_Absyn_uint_exp((unsigned int)_tmp50D,0U);e=_tmpA22;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp505=0U;({struct _dyneither_ptr _tmpA23=({const char*_tmp506="compile_pat_test_as_case!";_tag_dyneither(_tmp506,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA23,_tag_dyneither(_tmp505,sizeof(void*),0U));});});}_LL0:;}
# 3268
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->tag=17U,_tmp50E->f1=e;_tmp50E;}),0U);}
# 3272
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3281
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3284
void*t=(void*)_check_null(p->topt);
void*_tmp50F=p->r;void*_tmp510=_tmp50F;union Cyc_Absyn_AggrInfo _tmp541;struct Cyc_List_List*_tmp540;struct Cyc_List_List*_tmp53F;struct Cyc_List_List*_tmp53E;struct Cyc_Absyn_Pat*_tmp53D;struct Cyc_Absyn_Datatypedecl*_tmp53C;struct Cyc_Absyn_Datatypefield*_tmp53B;struct Cyc_List_List*_tmp53A;struct Cyc_Absyn_Vardecl*_tmp539;struct Cyc_Absyn_Pat*_tmp538;struct Cyc_Absyn_Vardecl*_tmp537;struct Cyc_Absyn_Vardecl*_tmp536;struct Cyc_Absyn_Pat*_tmp535;struct Cyc_Absyn_Vardecl*_tmp534;switch(*((int*)_tmp510)){case 2U: _LL1: _tmp534=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp510)->f2;_LL2: {
# 3287
struct Cyc_Absyn_Pat*_tmp511=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp511->topt=p->topt;
_tmp536=_tmp534;_tmp535=_tmp511;goto _LL4;}case 1U: _LL3: _tmp536=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp510)->f1;_tmp535=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp510)->f2;_LL4:
# 3292
({struct Cyc_List_List*_tmpA24=({struct Cyc_List_List*_tmp512=_region_malloc(rgn,sizeof(*_tmp512));_tmp512->hd=_tmp536,_tmp512->tl=*decls;_tmp512;});*decls=_tmpA24;});{
struct Cyc_Absyn_Stmt*_tmp513=({struct Cyc_Absyn_Exp*_tmpA25=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->tag=4U,_tmp514->f1=_tmp536;_tmp514;}),0U);Cyc_Absyn_assign_stmt(_tmpA25,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA26=_tmp513;Cyc_Toc_seq_stmt_opt(_tmpA26,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp535));});};case 4U: _LL5: _tmp537=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp510)->f2;_LL6:
# 3297
({struct Cyc_List_List*_tmpA27=({struct Cyc_List_List*_tmp515=_region_malloc(rgn,sizeof(*_tmp515));_tmp515->hd=_tmp537,_tmp515->tl=*decls;_tmp515;});*decls=_tmpA27;});
return({struct Cyc_Absyn_Exp*_tmpA28=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->tag=4U,_tmp516->f1=_tmp537;_tmp516;}),0U);Cyc_Absyn_assign_stmt(_tmpA28,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp539=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp510)->f1;_tmp538=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp510)->f2;_LLA:
# 3302
({struct Cyc_List_List*_tmpA29=({struct Cyc_List_List*_tmp517=_region_malloc(rgn,sizeof(*_tmp517));_tmp517->hd=_tmp539,_tmp517->tl=*decls;_tmp517;});*decls=_tmpA29;});
({void*_tmpA2A=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp539->type=_tmpA2A;});{
# 3305
struct Cyc_Absyn_Stmt*_tmp518=({struct Cyc_Absyn_Exp*_tmpA2D=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->tag=4U,_tmp519->f1=_tmp539;_tmp519;}),0U);Cyc_Absyn_assign_stmt(_tmpA2D,({
void*_tmpA2C=({void*_tmpA2B=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmpA2B,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA2C,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA2E=_tmp518;Cyc_Toc_seq_stmt_opt(_tmpA2E,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp538));});};case 9U: _LLB: _LLC:
# 3310
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp510)->f1)->r)->tag == 8U){_LL17: _tmp53C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp510)->f1)->r)->f1;_tmp53B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp510)->f1)->r)->f2;_tmp53A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp510)->f1)->r)->f3;_LL18:
# 3319
 if(_tmp53A == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp53B->name,_tmp53C->name);
void*_tmp51A=({void*_tmpA2F=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmpA2F,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpA30=Cyc_Toc_cast_it(_tmp51A,path);path=_tmpA30;});{
int cnt=1;
struct Cyc_List_List*_tmp51B=_tmp53B->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp53A != 0;(_tmp53A=_tmp53A->tl,_tmp51B=((struct Cyc_List_List*)_check_null(_tmp51B))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp51C=(struct Cyc_Absyn_Pat*)_tmp53A->hd;
if(_tmp51C->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp51D=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp51B))->hd)).f2;
void*_tmp51E=(void*)_check_null(_tmp51C->topt);
void*_tmp51F=Cyc_Toc_typ_to_c_array(_tmp51E);
struct Cyc_Absyn_Exp*_tmp520=({struct Cyc_Absyn_Exp*_tmpA31=path;Cyc_Absyn_aggrarrow_exp(_tmpA31,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp51D)))
({struct Cyc_Absyn_Exp*_tmpA32=Cyc_Toc_cast_it(_tmp51F,_tmp520);_tmp520=_tmpA32;});
({struct Cyc_Absyn_Stmt*_tmpA34=({struct Cyc_Absyn_Stmt*_tmpA33=s;Cyc_Toc_seq_stmt_opt(_tmpA33,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp520,_tmp51C));});s=_tmpA34;});};}
# 3337
return s;};};}else{_LL21: _tmp53D=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp510)->f1;_LL22:
# 3382
 return({struct _RegionHandle*_tmpA38=rgn;struct Cyc_Toc_Env**_tmpA37=nv;struct Cyc_List_List**_tmpA36=decls;struct Cyc_Absyn_Exp*_tmpA35=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA38,_tmpA37,_tmpA36,_tmpA35,_tmp53D);});}case 8U: _LL19: _tmp53E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp510)->f3;_LL1A:
# 3339
 _tmp53F=_tmp53E;goto _LL1C;case 5U: _LL1B: _tmp53F=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp510)->f1;_LL1C: {
# 3341
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp53F != 0;(_tmp53F=_tmp53F->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp521=(struct Cyc_Absyn_Pat*)_tmp53F->hd;
if(_tmp521->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp522=(void*)_check_null(_tmp521->topt);
struct _dyneither_ptr*_tmp523=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpA3E=({struct Cyc_Absyn_Stmt*_tmpA3D=s;Cyc_Toc_seq_stmt_opt(_tmpA3D,({struct _RegionHandle*_tmpA3C=rgn;struct Cyc_Toc_Env**_tmpA3B=nv;struct Cyc_List_List**_tmpA3A=decls;struct Cyc_Absyn_Exp*_tmpA39=Cyc_Toc_member_exp(path,_tmp523,0U);Cyc_Toc_extract_pattern_vars(_tmpA3C,_tmpA3B,_tmpA3A,_tmpA39,_tmp521);}));});s=_tmpA3E;});};}
# 3351
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp510)->f1 == 0){_LL1D: _LL1E:
({void*_tmp524=0U;({struct _dyneither_ptr _tmpA3F=({const char*_tmp525="unresolved aggregate pattern!";_tag_dyneither(_tmp525,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA3F,_tag_dyneither(_tmp524,sizeof(void*),0U));});});}else{_LL1F: _tmp541=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp510)->f1;_tmp540=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp510)->f3;_LL20: {
# 3355
struct Cyc_Absyn_Aggrdecl*_tmp526=Cyc_Absyn_get_known_aggrdecl(_tmp541);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp540 != 0;_tmp540=_tmp540->tl){
struct _tuple37*_tmp527=(struct _tuple37*)_tmp540->hd;
struct Cyc_Absyn_Pat*_tmp528=(*_tmp527).f2;
if(_tmp528->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp529=Cyc_Absyn_designatorlist_to_fieldname((*_tmp527).f1);
void*_tmp52A=(void*)_check_null(_tmp528->topt);
void*_tmp52B=Cyc_Toc_typ_to_c_array(_tmp52A);
struct Cyc_Absyn_Exp*_tmp52C=Cyc_Toc_member_exp(path,_tmp529,0U);
# 3367
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp526->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpA40=Cyc_Toc_member_exp(_tmp52C,Cyc_Toc_val_sp,0U);_tmp52C=_tmpA40;});{
void*_tmp52D=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp526->impl))->fields,_tmp529)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp52D))
({struct Cyc_Absyn_Exp*_tmpA41=Cyc_Toc_cast_it(_tmp52B,_tmp52C);_tmp52C=_tmpA41;});else{
if(!Cyc_Tcutil_is_array_type(_tmp52D) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp52D)))
# 3374
({struct Cyc_Absyn_Exp*_tmpA43=Cyc_Absyn_deref_exp(({void*_tmpA42=Cyc_Absyn_cstar_type(_tmp52B,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA42,
Cyc_Absyn_address_exp(_tmp52C,0U));}),0U);
# 3374
_tmp52C=_tmpA43;});}
# 3377
({struct Cyc_Absyn_Stmt*_tmpA45=({struct Cyc_Absyn_Stmt*_tmpA44=s;Cyc_Toc_seq_stmt_opt(_tmpA44,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp52C,_tmp528));});s=_tmpA45;});};};}
# 3379
return s;}}case 15U: _LL23: _LL24:
# 3384
({void*_tmp52E=0U;({struct _dyneither_ptr _tmpA46=({const char*_tmp52F="unknownid pat";_tag_dyneither(_tmp52F,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA46,_tag_dyneither(_tmp52E,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp530=0U;({struct _dyneither_ptr _tmpA47=({const char*_tmp531="unknowncall pat";_tag_dyneither(_tmp531,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA47,_tag_dyneither(_tmp530,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp532=0U;({struct _dyneither_ptr _tmpA48=({const char*_tmp533="exp pat";_tag_dyneither(_tmp533,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA48,_tag_dyneither(_tmp532,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3395
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3403
void*_tmp542=dopt;struct Cyc_List_List*_tmp580;struct Cyc_List_List*_tmp57F;void*_tmp57E;struct Cyc_Tcpat_Rhs*_tmp57D;if(_tmp542 == 0){_LL1: _LL2:
# 3405
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp542)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp57D=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp542)->f1;_LL6:
# 3410
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp543=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp544=_tmp543;int*_tmp54B;struct _dyneither_ptr*_tmp54A;struct _dyneither_ptr*_tmp549;struct Cyc_Absyn_Switch_clause*_tmp548;_LLA: _tmp54B=(int*)& _tmp544->f1;_tmp54A=_tmp544->f2;_tmp549=_tmp544->f3;_tmp548=_tmp544->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp545=_tmp548->body;
if(_tmp545 == _tmp57D->rhs){
# 3415
if(*_tmp54B)
return Cyc_Absyn_goto_stmt(_tmp54A,0U);
*_tmp54B=1;{
# 3419
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA4B=rgn;struct Cyc_List_List**_tmpA4A=decls;struct Cyc_Absyn_Exp*_tmpA49=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA4B,& nv,_tmpA4A,_tmpA49,_tmp548->pattern);});
# 3422
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp549,_tmp548->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpA4C=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpA4C;});
({struct Cyc_Absyn_Stmt*_tmpA4D=Cyc_Absyn_label_stmt(_tmp54A,res,0U);res=_tmpA4D;});
({struct Cyc_List_List*_tmpA4F=({struct Cyc_List_List*_tmp547=_region_malloc(rgn,sizeof(*_tmp547));({struct _tuple38*_tmpA4E=({struct _tuple38*_tmp546=_region_malloc(rgn,sizeof(*_tmp546));_tmp546->f1=nv,_tmp546->f2=_tmp549,_tmp546->f3=_tmp545;_tmp546;});_tmp547->hd=_tmpA4E;}),_tmp547->tl=*bodies;_tmp547;});*bodies=_tmpA4F;});
return res;};}};}
# 3430
({void*_tmp54C=0U;({struct _dyneither_ptr _tmpA50=({const char*_tmp54D="couldn't find rhs!";_tag_dyneither(_tmp54D,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA50,_tag_dyneither(_tmp54C,sizeof(void*),0U));});});default: _LL7: _tmp580=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp542)->f1;_tmp57F=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp542)->f2;_tmp57E=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp542)->f3;_LL8: {
# 3433
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp57E,lscs,v);
# 3435
struct Cyc_Absyn_Exp*_tmp54E=({struct Cyc_List_List*_tmpA51=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp580);Cyc_Toc_compile_path(_tmpA51,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp54F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp57F);
# 3438
struct _tuple36 _tmp550=Cyc_Toc_admits_switch(_tmp54F);struct _tuple36 _tmp551=_tmp550;int _tmp57C;void*_tmp57B;_LLD: _tmp57C=_tmp551.f1;_tmp57B=_tmp551.f2;_LLE:;
if(_tmp57C > 1){
# 3442
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp562=_cycalloc(sizeof(*_tmp562));
({struct Cyc_Absyn_Switch_clause*_tmpA53=({struct Cyc_Absyn_Switch_clause*_tmp561=_cycalloc(sizeof(*_tmp561));({struct Cyc_Absyn_Pat*_tmpA52=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp561->pattern=_tmpA52;}),_tmp561->pat_vars=0,_tmp561->where_clause=0,_tmp561->body=res,_tmp561->loc=0U;_tmp561;});_tmp562->hd=_tmpA53;}),_tmp562->tl=0;_tmp562;});
# 3445
for(0;_tmp54F != 0;_tmp54F=_tmp54F->tl){
struct _tuple32 _tmp552=*((struct _tuple32*)_tmp54F->hd);struct _tuple32 _tmp553=_tmp552;void*_tmp559;void*_tmp558;_LL10: _tmp559=_tmp553.f1;_tmp558=_tmp553.f2;_LL11:;{
# 3448
struct Cyc_Absyn_Pat*_tmp554=Cyc_Toc_compile_pat_test_as_case(_tmp559);
# 3450
struct Cyc_Absyn_Stmt*_tmp555=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp558,lscs,v);
# 3452
({struct Cyc_List_List*_tmpA55=({struct Cyc_List_List*_tmp557=_cycalloc(sizeof(*_tmp557));({struct Cyc_Absyn_Switch_clause*_tmpA54=({struct Cyc_Absyn_Switch_clause*_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->pattern=_tmp554,_tmp556->pat_vars=0,_tmp556->where_clause=0,_tmp556->body=_tmp555,_tmp556->loc=0U;_tmp556;});_tmp557->hd=_tmpA54;}),_tmp557->tl=new_lscs;_tmp557;});new_lscs=_tmpA55;});};}
# 3454
{void*_tmp55A=_tmp57B;struct _dyneither_ptr*_tmp55F;switch(*((int*)_tmp55A)){case 1U: _LL13: _LL14:
# 3457
 LOOP1: {
void*_tmp55B=_tmp54E->r;void*_tmp55C=_tmp55B;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp55D;switch(*((int*)_tmp55C)){case 14U: _LL1C: _tmp55D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp55C)->f2;_LL1D:
 _tmp54E=_tmp55D;goto LOOP1;case 20U: _LL1E: _tmp55E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp55C)->f1;_LL1F:
 _tmp54E=_tmp55E;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3463
({struct Cyc_Absyn_Exp*_tmpA57=Cyc_Absyn_deref_exp(({void*_tmpA56=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA56,_tmp54E);}),0U);_tmp54E=_tmpA57;});goto _LL12;case 3U: _LL15: _tmp55F=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp55A)->f1;_LL16:
# 3466
({struct Cyc_Absyn_Exp*_tmpA59=({struct Cyc_Absyn_Exp*_tmpA58=Cyc_Toc_member_exp(_tmp54E,_tmp55F,0U);Cyc_Toc_member_exp(_tmpA58,Cyc_Toc_tag_sp,0U);});_tmp54E=_tmpA59;});goto _LL12;case 2U: _LL17: _LL18:
# 3468
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3471
({struct Cyc_Absyn_Stmt*_tmpA5A=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->tag=10U,_tmp560->f1=_tmp54E,_tmp560->f2=new_lscs,_tmp560->f3=0;_tmp560;}),0U);res=_tmpA5A;});}else{
# 3475
void*_tmp563=_tmp57B;struct Cyc_Absyn_Exp*_tmp57A;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp563)->tag == 2U){_LL23: _tmp57A=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp563)->f1;_LL24: {
# 3477
struct Cyc_List_List*_tmp564=_tmp54F;void*_tmp573;struct Cyc_Tcpat_Rhs*_tmp572;if(_tmp564 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)((struct Cyc_List_List*)_tmp564)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp564)->tl == 0){_LL28: _tmp573=((struct _tuple32*)_tmp564->hd)->f1;_tmp572=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)_tmp564->hd)->f2)->f1;_LL29:
# 3483
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp565=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp566=_tmp565;int*_tmp56D;struct _dyneither_ptr*_tmp56C;struct _dyneither_ptr*_tmp56B;struct Cyc_Absyn_Switch_clause*_tmp56A;_LL2D: _tmp56D=(int*)& _tmp566->f1;_tmp56C=_tmp566->f2;_tmp56B=_tmp566->f3;_tmp56A=_tmp566->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp567=_tmp56A->body;
if(_tmp567 == _tmp572->rhs){
# 3488
if(*_tmp56D)
return Cyc_Absyn_goto_stmt(_tmp56C,0U);
*_tmp56D=1;{
# 3492
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA5D=rgn;struct Cyc_List_List**_tmpA5C=decls;struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA5D,& nv,_tmpA5C,_tmpA5B,_tmp56A->pattern);});
# 3495
Cyc_Toc_exp_to_c(nv,_tmp57A);{
# 3498
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp56B,_tmp56A->body,0U);
({struct Cyc_Absyn_Stmt*_tmpA5E=Cyc_Absyn_ifthenelse_stmt(_tmp57A,r,res,0U);r=_tmpA5E;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpA5F=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpA5F;});
({struct Cyc_Absyn_Stmt*_tmpA60=Cyc_Absyn_label_stmt(_tmp56C,r,0U);r=_tmpA60;});
({struct Cyc_List_List*_tmpA62=({struct Cyc_List_List*_tmp569=_region_malloc(rgn,sizeof(*_tmp569));({struct _tuple38*_tmpA61=({struct _tuple38*_tmp568=_region_malloc(rgn,sizeof(*_tmp568));_tmp568->f1=nv,_tmp568->f2=_tmp56B,_tmp568->f3=_tmp567;_tmp568;});_tmp569->hd=_tmpA61;}),_tmp569->tl=*bodies;_tmp569;});*bodies=_tmpA62;});
return r;};};}};}
# 3507
({void*_tmp56E=0U;({struct _dyneither_ptr _tmpA63=({const char*_tmp56F="couldn't find rhs!";_tag_dyneither(_tmp56F,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA63,_tag_dyneither(_tmp56E,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp570=0U;({struct _dyneither_ptr _tmpA64=({const char*_tmp571="bad where clause in match compiler";_tag_dyneither(_tmp571,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA64,_tag_dyneither(_tmp570,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3512
 for(0;_tmp54F != 0;_tmp54F=_tmp54F->tl){
struct _tuple32 _tmp574=*((struct _tuple32*)_tmp54F->hd);struct _tuple32 _tmp575=_tmp574;void*_tmp579;void*_tmp578;_LL30: _tmp579=_tmp575.f1;_tmp578=_tmp575.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp576=Cyc_Toc_compile_pat_test(_tmp54E,_tmp579);
struct Cyc_Absyn_Stmt*_tmp577=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp578,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpA65=Cyc_Absyn_ifthenelse_stmt(_tmp576,_tmp577,res,0U);res=_tmpA65;});};}}_LL22:;}
# 3520
return res;}}}_LL0:;}
# 3530
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3532
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp581=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp582=_tmp581;struct Cyc_Toc_Env**_tmp584;struct Cyc_Absyn_Stmt*_tmp583;_LL1: _tmp584=(struct Cyc_Toc_Env**)& _tmp582->f1;_tmp583=_tmp582->f3;_LL2:;
if(_tmp583 == s)return _tmp584;}
# 3538
return 0;}
# 3542
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3545
void*_tmp585=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3548
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp586=_new_region("rgn");struct _RegionHandle*rgn=& _tmp586;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp587=Cyc_Toc_share_env(rgn,nv);
# 3555
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3560
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp587,& mydecls,& mybodies,dopt,lscs,v);
# 3568
{struct Cyc_List_List*_tmp588=lscs;for(0;_tmp588 != 0;_tmp588=_tmp588->tl){
struct _tuple35*_tmp589=(struct _tuple35*)_tmp588->hd;struct _tuple35*_tmp58A=_tmp589;struct _dyneither_ptr*_tmp592;struct Cyc_Absyn_Switch_clause*_tmp591;_LL1: _tmp592=_tmp58A->f3;_tmp591=_tmp58A->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp591->body;
# 3572
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3575
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp58B=*envp;
# 3578
if(_tmp588->tl != 0){
struct _tuple35*_tmp58C=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp588->tl))->hd;struct _tuple35*_tmp58D=_tmp58C;struct _dyneither_ptr*_tmp590;struct Cyc_Absyn_Switch_clause*_tmp58F;_LL4: _tmp590=_tmp58D->f3;_tmp58F=_tmp58D->f4;_LL5:;{
# 3582
struct Cyc_Toc_Env**_tmp58E=Cyc_Toc_find_case_env(mybodies,_tmp58F->body);
# 3587
if(_tmp58E == 0)
({struct Cyc_Toc_Env*_tmpA66=Cyc_Toc_last_switchclause_env(rgn,_tmp58B,end_l);Cyc_Toc_stmt_to_c(_tmpA66,s);});else{
# 3591
struct Cyc_List_List*vs=0;
if(_tmp58F->pat_vars != 0){
({struct Cyc_List_List*_tmpA67=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp58F->pat_vars))->v)).f1);vs=_tmpA67;});
({struct Cyc_List_List*_tmpA68=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpA68;});}
# 3596
({struct Cyc_Toc_Env*_tmpA69=Cyc_Toc_non_last_switchclause_env(rgn,_tmp58B,end_l,_tmp590,vs);Cyc_Toc_stmt_to_c(_tmpA69,s);});}};}else{
# 3600
({struct Cyc_Toc_Env*_tmpA6A=Cyc_Toc_last_switchclause_env(rgn,_tmp58B,end_l);Cyc_Toc_stmt_to_c(_tmpA6A,s);});}};};}}{
# 3604
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA6C=test_tree;Cyc_Absyn_seq_stmt(_tmpA6C,({struct _dyneither_ptr*_tmpA6B=end_l;Cyc_Absyn_label_stmt(_tmpA6B,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3606
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp593=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpA6D=Cyc_Toc_temp_var();_tmp593->name=_tmpA6D;});
({void*_tmpA6E=Cyc_Toc_typ_to_c_array(_tmp593->type);_tmp593->type=_tmpA6E;});
({struct Cyc_Absyn_Stmt*_tmpA70=({struct Cyc_Absyn_Decl*_tmpA6F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->tag=0U,_tmp594->f1=_tmp593;_tmp594;}),0U);Cyc_Absyn_decl_stmt(_tmpA6F,res,0U);});res=_tmpA70;});}
# 3613
({void*_tmpA74=({struct _tuple1*_tmpA73=v;void*_tmpA72=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA71=e;Cyc_Absyn_declare_stmt(_tmpA73,_tmpA72,_tmpA71,res,0U);})->r;whole_s->r=_tmpA74;});
_npop_handler(0U);return;};}
# 3552
;_pop_region(rgn);};}
# 3619
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3621
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3626
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3628
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA76=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpA76,({struct Cyc_List_List*_tmp595=_cycalloc(sizeof(*_tmp595));
({struct Cyc_Absyn_Exp*_tmpA75=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp595->hd=_tmpA75;}),_tmp595->tl=0;_tmp595;}),0U);}),0U);}
# 3632
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA78=({struct Cyc_Absyn_Stmt*_tmpA77=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA77,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA78;});}
# 3637
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3639
while(1){
void*_tmp596=s->r;void*_tmp597=_tmp596;struct Cyc_Absyn_Stmt*_tmp5EE;struct Cyc_List_List*_tmp5ED;void*_tmp5EC;struct Cyc_Absyn_Stmt*_tmp5EB;struct Cyc_Absyn_Exp*_tmp5EA;struct _dyneither_ptr*_tmp5E9;struct Cyc_Absyn_Stmt*_tmp5E8;struct Cyc_Absyn_Decl*_tmp5E7;struct Cyc_Absyn_Stmt*_tmp5E6;struct Cyc_List_List*_tmp5E5;struct Cyc_Absyn_Switch_clause**_tmp5E4;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_List_List*_tmp5E2;void*_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_Absyn_Exp*_tmp5DE;struct Cyc_Absyn_Stmt*_tmp5DD;struct Cyc_Absyn_Exp*_tmp5DC;struct Cyc_Absyn_Stmt*_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DA;struct Cyc_Absyn_Stmt*_tmp5D9;struct Cyc_Absyn_Stmt*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D7;struct Cyc_Absyn_Stmt*_tmp5D6;struct Cyc_Absyn_Stmt*_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D4;switch(*((int*)_tmp597)){case 0U: _LL1: _LL2:
# 3642
 return;case 1U: _LL3: _tmp5D4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_LL4:
# 3644
 Cyc_Toc_exp_to_c(nv,_tmp5D4);
return;case 2U: _LL5: _tmp5D6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_tmp5D5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp597)->f2;_LL6:
# 3647
 Cyc_Toc_stmt_to_c(nv,_tmp5D6);
s=_tmp5D5;
continue;case 3U: _LL7: _tmp5D7=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_LL8: {
# 3651
void*topt=0;
if(_tmp5D7 != 0){
({void*_tmpA79=Cyc_Toc_typ_to_c((void*)_check_null(_tmp5D7->topt));topt=_tmpA79;});
Cyc_Toc_exp_to_c(nv,_tmp5D7);}{
# 3657
int _tmp598=Cyc_Toc_get_npop(s);
if(_tmp598 > 0){
if(topt != 0){
struct _tuple1*_tmp599=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp59A=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp599,0U),0U);
({void*_tmpA7E=({struct _tuple1*_tmpA7D=_tmp599;void*_tmpA7C=topt;struct Cyc_Absyn_Exp*_tmpA7B=_tmp5D7;Cyc_Absyn_declare_stmt(_tmpA7D,_tmpA7C,_tmpA7B,({
struct Cyc_Absyn_Stmt*_tmpA7A=Cyc_Toc_make_npop_handler(_tmp598);Cyc_Absyn_seq_stmt(_tmpA7A,_tmp59A,0U);}),0U);})->r;
# 3662
s->r=_tmpA7E;});}else{
# 3666
Cyc_Toc_do_npop_before(_tmp598,s);}}
# 3668
return;};}case 4U: _LL9: _tmp5DA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_tmp5D9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp597)->f2;_tmp5D8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp597)->f3;_LLA:
# 3670
 Cyc_Toc_exp_to_c(nv,_tmp5DA);
Cyc_Toc_stmt_to_c(nv,_tmp5D9);
s=_tmp5D8;
continue;case 5U: _LLB: _tmp5DC=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp597)->f1).f1;_tmp5DB=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp597)->f2;_LLC:
# 3675
 Cyc_Toc_exp_to_c(nv,_tmp5DC);{
struct _RegionHandle _tmp59B=_new_region("temp");struct _RegionHandle*temp=& _tmp59B;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA7F=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA7F,_tmp5DB);});
# 3679
_npop_handler(0U);return;
# 3676
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3681
struct Cyc_Toc_Env*_tmp59C=nv;struct _dyneither_ptr**_tmp59D;_LL22: _tmp59D=_tmp59C->break_lab;_LL23:;
if(_tmp59D != 0)
({void*_tmpA80=Cyc_Toc_goto_stmt_r(*_tmp59D);s->r=_tmpA80;});
# 3685
({int _tmpA81=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA81,s);});
return;}case 7U: _LLF: _LL10: {
# 3688
struct Cyc_Toc_Env*_tmp59E=nv;struct _dyneither_ptr**_tmp59F;_LL25: _tmp59F=_tmp59E->continue_lab;_LL26:;
if(_tmp59F != 0)
({void*_tmpA82=Cyc_Toc_goto_stmt_r(*_tmp59F);s->r=_tmpA82;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3694
({int _tmpA83=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA83,s);});
return;case 9U: _LL13: _tmp5E0=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_tmp5DF=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp597)->f2).f1;_tmp5DE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp597)->f3).f1;_tmp5DD=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp597)->f4;_LL14:
# 3698
 Cyc_Toc_exp_to_c(nv,_tmp5E0);Cyc_Toc_exp_to_c(nv,_tmp5DF);Cyc_Toc_exp_to_c(nv,_tmp5DE);{
struct _RegionHandle _tmp5A0=_new_region("temp");struct _RegionHandle*temp=& _tmp5A0;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA84=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA84,_tmp5DD);});
# 3702
_npop_handler(0U);return;
# 3699
;_pop_region(temp);};case 10U: _LL15: _tmp5E3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_tmp5E2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp597)->f2;_tmp5E1=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp597)->f3;_LL16:
# 3704
 Cyc_Toc_xlate_switch(nv,s,_tmp5E3,_tmp5E2,_tmp5E1);
return;case 11U: _LL17: _tmp5E5=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_tmp5E4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp597)->f2;_LL18: {
# 3707
struct Cyc_Toc_Env*_tmp5A1=nv;struct Cyc_Toc_FallthruInfo*_tmp5AB;_LL28: _tmp5AB=_tmp5A1->fallthru_info;_LL29:;
if(_tmp5AB == 0)
({void*_tmp5A2=0U;({struct _dyneither_ptr _tmpA85=({const char*_tmp5A3="fallthru in unexpected place";_tag_dyneither(_tmp5A3,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA85,_tag_dyneither(_tmp5A2,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp5A4=*_tmp5AB;struct Cyc_Toc_FallthruInfo _tmp5A5=_tmp5A4;struct _dyneither_ptr*_tmp5AA;struct Cyc_List_List*_tmp5A9;_LL2B: _tmp5AA=_tmp5A5.label;_tmp5A9=_tmp5A5.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp5AA,0U);
# 3713
({int _tmpA86=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA86,s2);});{
struct Cyc_List_List*_tmp5A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5A9);
struct Cyc_List_List*_tmp5A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5E5);
for(0;_tmp5A6 != 0;(_tmp5A6=_tmp5A6->tl,_tmp5A7=_tmp5A7->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5A7))->hd);
({struct Cyc_Absyn_Stmt*_tmpA89=({struct Cyc_Absyn_Stmt*_tmpA88=({struct Cyc_Absyn_Exp*_tmpA87=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->tag=5U,_tmp5A8->f1=(struct Cyc_Absyn_Vardecl*)_tmp5A6->hd;_tmp5A8;}),0U);Cyc_Absyn_assign_stmt(_tmpA87,(struct Cyc_Absyn_Exp*)_tmp5A7->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA88,s2,0U);});s2=_tmpA89;});}
# 3721
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp5E7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_tmp5E6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp597)->f2;_LL1A:
# 3727
{void*_tmp5AC=_tmp5E7->r;void*_tmp5AD=_tmp5AC;struct Cyc_Absyn_Tvar*_tmp5C8;struct Cyc_Absyn_Vardecl*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_Fndecl*_tmp5C5;struct Cyc_List_List*_tmp5C4;struct Cyc_Absyn_Pat*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;void*_tmp5C1;struct Cyc_Absyn_Vardecl*_tmp5C0;switch(*((int*)_tmp5AD)){case 0U: _LL2E: _tmp5C0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5AD)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp5C0,_tmp5E6);goto _LL2D;case 2U: _LL30: _tmp5C3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5AD)->f1;_tmp5C2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5AD)->f3;_tmp5C1=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5AD)->f4;_LL31:
# 3735
{void*_tmp5AE=_tmp5C3->r;void*_tmp5AF=_tmp5AE;struct Cyc_Absyn_Vardecl*_tmp5B1;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5AF)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5AF)->f2)->r)->tag == 0U){_LL3B: _tmp5B1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5AF)->f1;_LL3C:
# 3737
({struct _tuple1*_tmpA8A=Cyc_Toc_temp_var();_tmp5B1->name=_tmpA8A;});
_tmp5B1->initializer=_tmp5C2;
({void*_tmpA8B=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->tag=0U,_tmp5B0->f1=_tmp5B1;_tmp5B0;});_tmp5E7->r=_tmpA8B;});
Cyc_Toc_local_decl_to_c(nv,_tmp5B1,_tmp5E6);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3746
({void*_tmpA8C=(Cyc_Toc_letdecl_to_c(nv,_tmp5C3,_tmp5C1,(void*)_check_null(_tmp5C2->topt),_tmp5C2,_tmp5E6))->r;s->r=_tmpA8C;});
goto _LL3A;}_LL3A:;}
# 3749
goto _LL2D;case 3U: _LL32: _tmp5C4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5AD)->f1;_LL33: {
# 3753
struct Cyc_List_List*_tmp5B2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5C4);
if(_tmp5B2 == 0)
({void*_tmp5B3=0U;({struct _dyneither_ptr _tmpA8D=({const char*_tmp5B4="empty Letv_d";_tag_dyneither(_tmp5B4,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA8D,_tag_dyneither(_tmp5B3,sizeof(void*),0U));});});
({void*_tmpA8E=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->tag=0U,_tmp5B5->f1=(struct Cyc_Absyn_Vardecl*)_tmp5B2->hd;_tmp5B5;});_tmp5E7->r=_tmpA8E;});
_tmp5B2=_tmp5B2->tl;
for(0;_tmp5B2 != 0;_tmp5B2=_tmp5B2->tl){
struct Cyc_Absyn_Decl*_tmp5B6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->tag=0U,_tmp5B7->f1=(struct Cyc_Absyn_Vardecl*)_tmp5B2->hd;_tmp5B7;}),0U);
({void*_tmpA90=({struct Cyc_Absyn_Decl*_tmpA8F=_tmp5B6;Cyc_Absyn_decl_stmt(_tmpA8F,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA90;});}
# 3762
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp5C5=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5AD)->f1;_LL35:
# 3765
 Cyc_Toc_fndecl_to_c(nv,_tmp5C5,0);
Cyc_Toc_stmt_to_c(nv,_tmp5E6);
goto _LL2D;case 4U: _LL36: _tmp5C8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5AD)->f1;_tmp5C7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5AD)->f2;_tmp5C6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5AD)->f3;_LL37: {
# 3769
struct Cyc_Absyn_Stmt*_tmp5B8=_tmp5E6;
# 3771
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp5C7->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3778
Cyc_Toc_stmt_to_c(nv,_tmp5B8);
if(Cyc_Absyn_no_regions)
({void*_tmpA94=({struct _tuple1*_tmpA93=x_var;void*_tmpA92=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA91=
Cyc_Absyn_uint_exp(0U,0U);
# 3780
Cyc_Absyn_declare_stmt(_tmpA93,_tmpA92,_tmpA91,_tmp5B8,0U);})->r;s->r=_tmpA94;});else{
# 3782
if((unsigned int)_tmp5C6){
# 3784
Cyc_Toc_exp_to_c(nv,_tmp5C6);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA96=_tmp5C6;Cyc_Absyn_aggrarrow_exp(_tmpA96,({struct _dyneither_ptr*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));({struct _dyneither_ptr _tmpA95=({const char*_tmp5B9="h";_tag_dyneither(_tmp5B9,sizeof(char),2U);});*_tmp5BA=_tmpA95;});_tmp5BA;}),0U);}),0U);
({void*_tmpA97=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp5B8,0U))->r;s->r=_tmpA97;});};}else{
# 3794
({void*_tmpAA4=({
struct _tuple1*_tmpAA3=rh_var;void*_tmpAA2=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpAA1=({
struct Cyc_Absyn_Exp*_tmpA99=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpA99,({struct Cyc_List_List*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));
({struct Cyc_Absyn_Exp*_tmpA98=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp5BB->hd=_tmpA98;}),_tmp5BB->tl=0;_tmp5BB;}),0U);});
# 3795
Cyc_Absyn_declare_stmt(_tmpAA3,_tmpAA2,_tmpAA1,({
# 3799
struct _tuple1*_tmpAA0=x_var;void*_tmpA9F=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA9E=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpAA0,_tmpA9F,_tmpA9E,({
struct Cyc_Absyn_Stmt*_tmpA9D=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA9A=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpA9A,({struct Cyc_Absyn_Exp*_tmp5BC[1U];_tmp5BC[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5BC,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpA9D,({
struct Cyc_Absyn_Stmt*_tmpA9C=_tmp5B8;Cyc_Absyn_seq_stmt(_tmpA9C,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpA9B,({struct Cyc_Absyn_Exp*_tmp5BD[1U];_tmp5BD[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5BD,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3794
s->r=_tmpAA4;});}}
# 3805
return;}default: _LL38: _LL39:
({void*_tmp5BE=0U;({struct _dyneither_ptr _tmpAA5=({const char*_tmp5BF="bad nested declaration within function";_tag_dyneither(_tmp5BF,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA5,_tag_dyneither(_tmp5BE,sizeof(void*),0U));});});}_LL2D:;}
# 3808
return;case 13U: _LL1B: _tmp5E9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_tmp5E8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp597)->f2;_LL1C:
# 3810
 s=_tmp5E8;continue;case 14U: _LL1D: _tmp5EB=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_tmp5EA=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp597)->f2).f1;_LL1E: {
# 3812
struct _RegionHandle _tmp5C9=_new_region("temp");struct _RegionHandle*temp=& _tmp5C9;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAA6=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAA6,_tmp5EB);});
Cyc_Toc_exp_to_c(nv,_tmp5EA);
# 3816
_npop_handler(0U);return;
# 3812
;_pop_region(temp);}default: _LL1F: _tmp5EE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp597)->f1;_tmp5ED=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp597)->f2;_tmp5EC=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp597)->f3;_LL20: {
# 3830 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3840
e_exp->topt=e_typ;{
struct _RegionHandle _tmp5CA=_new_region("temp");struct _RegionHandle*temp=& _tmp5CA;_push_region(temp);
# 3843
Cyc_Toc_stmt_to_c(nv,_tmp5EE);{
struct Cyc_Absyn_Stmt*_tmp5CB=({struct Cyc_Absyn_Stmt*_tmpAA7=_tmp5EE;Cyc_Absyn_seq_stmt(_tmpAA7,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 3848
struct Cyc_Absyn_Stmt*_tmp5CC=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->tag=10U,_tmp5D3->f1=e_exp,_tmp5D3->f2=_tmp5ED,_tmp5D3->f3=_tmp5EC;_tmp5D3;}),0U);
# 3850
Cyc_Toc_stmt_to_c(nv,_tmp5CC);{
# 3853
struct Cyc_Absyn_Exp*_tmp5CD=({
struct Cyc_Absyn_Exp*_tmpAA9=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpAA9,({struct Cyc_List_List*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));
({struct Cyc_Absyn_Exp*_tmpAA8=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp5D2->hd=_tmpAA8;}),_tmp5D2->tl=0;_tmp5D2;}),0U);});
# 3857
struct Cyc_Absyn_Stmt*_tmp5CE=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAAB=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpAAB,({struct Cyc_List_List*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));
({struct Cyc_Absyn_Exp*_tmpAAA=Cyc_Absyn_address_exp(h_exp,0U);_tmp5D1->hd=_tmpAAA;}),_tmp5D1->tl=0;_tmp5D1;}),0U);}),0U);
# 3861
struct Cyc_Absyn_Exp*_tmp5CF=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp5D0=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpABA=({
struct _tuple1*_tmpAB9=h_var;void*_tmpAB8=h_typ;Cyc_Absyn_declare_stmt(_tmpAB9,_tmpAB8,0,({
struct Cyc_Absyn_Stmt*_tmpAB7=_tmp5CE;Cyc_Absyn_seq_stmt(_tmpAB7,({
struct _tuple1*_tmpAB6=was_thrown_var;void*_tmpAB5=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpAB4=_tmp5CF;Cyc_Absyn_declare_stmt(_tmpAB6,_tmpAB5,_tmpAB4,({
struct Cyc_Absyn_Stmt*_tmpAB3=({struct Cyc_Absyn_Exp*_tmpAAD=_tmp5CD;struct Cyc_Absyn_Stmt*_tmpAAC=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp5D0,0U);
# 3867
Cyc_Absyn_ifthenelse_stmt(_tmpAAD,_tmpAAC,
# 3869
Cyc_Toc_skip_stmt_dl(),0U);});
# 3867
Cyc_Absyn_seq_stmt(_tmpAB3,({
# 3870
struct Cyc_Absyn_Exp*_tmpAB2=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpAB1=_tmp5CB;Cyc_Absyn_ifthenelse_stmt(_tmpAB2,_tmpAB1,({
# 3872
struct _tuple1*_tmpAB0=e_var;void*_tmpAAF=e_typ;struct Cyc_Absyn_Exp*_tmpAAE=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 3872
Cyc_Absyn_declare_stmt(_tmpAB0,_tmpAAF,_tmpAAE,_tmp5CC,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3863
s->r=_tmpABA;});};};
# 3876
_npop_handler(0U);return;
# 3841
;_pop_region(temp);};}}_LL0:;}}
# 3885
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpABB=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpABB;});{
struct _RegionHandle _tmp5EF=_new_region("frgn");struct _RegionHandle*frgn=& _tmp5EF;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp5F0=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp5F1=f->args;for(0;_tmp5F1 != 0;_tmp5F1=_tmp5F1->tl){
struct _tuple1*_tmp5F2=({struct _tuple1*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->f1=Cyc_Absyn_Loc_n,_tmp5F3->f2=(*((struct _tuple9*)_tmp5F1->hd)).f1;_tmp5F3;});
({void*_tmpABC=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp5F1->hd)).f3);(*((struct _tuple9*)_tmp5F1->hd)).f3=_tmpABC;});}}
# 3901
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3905
({struct Cyc_Hashtable_Table**_tmpABE=({struct Cyc_Hashtable_Table**_tmp5F4=_cycalloc(sizeof(*_tmp5F4));({struct Cyc_Hashtable_Table*_tmpABD=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp5F4=_tmpABD;});_tmp5F4;});Cyc_Toc_fn_pop_table=_tmpABE;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp5F5=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp5F6=_tmp5F5;struct _dyneither_ptr*_tmp5FF;struct Cyc_Absyn_Tqual _tmp5FE;void*_tmp5FD;int _tmp5FC;_LL1: _tmp5FF=_tmp5F6.name;_tmp5FE=_tmp5F6.tq;_tmp5FD=_tmp5F6.type;_tmp5FC=_tmp5F6.inject;_LL2:;{
void*_tmp5F7=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp5FD,Cyc_Absyn_heap_rgn_type,_tmp5FE,Cyc_Absyn_false_type));
struct _tuple1*_tmp5F8=({struct _tuple1*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->f1=Cyc_Absyn_Loc_n,_tmp5FB->f2=(struct _dyneither_ptr*)_check_null(_tmp5FF);_tmp5FB;});
({struct Cyc_List_List*_tmpAC1=({struct Cyc_List_List*_tmpAC0=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpAC0,({struct Cyc_List_List*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));({struct _tuple9*_tmpABF=({struct _tuple9*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->f1=_tmp5FF,_tmp5F9->f2=_tmp5FE,_tmp5F9->f3=_tmp5F7;_tmp5F9;});_tmp5FA->hd=_tmpABF;}),_tmp5FA->tl=0;_tmp5FA;}));});f->args=_tmpAC1;});
f->cyc_varargs=0;};}
# 3914
{struct Cyc_List_List*_tmp600=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp600 != 0;_tmp600=_tmp600->tl){
({void*_tmpAC2=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp600->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp600->hd)->type=_tmpAC2;});}}
# 3917
({struct Cyc_Toc_Env*_tmpAC3=Cyc_Toc_clear_toplevel(frgn,_tmp5F0);Cyc_Toc_stmt_to_c(_tmpAC3,f->body);});}
# 3894
;_pop_region(frgn);};}
# 3921
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp601=s;switch(_tmp601){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3938 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 3940
struct _tuple39 _tmp602=*env;struct _tuple39 _tmp603=_tmp602;struct Cyc_Toc_TocState*_tmp637;struct Cyc_Absyn_Aggrdecl*_tmp636;_LL1: _tmp637=_tmp603.f1;_tmp636=*_tmp603.f2;_LL2:;{
struct _tuple1*_tmp604=_tmp636->name;
struct Cyc_Toc_TocState _tmp605=*_tmp637;struct Cyc_Toc_TocState _tmp606=_tmp605;struct Cyc_Dict_Dict*_tmp635;_LL4: _tmp635=_tmp606.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple18**_tmp607=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp635,_tmp604);
if(_tmp607 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp636->kind == Cyc_Absyn_StructA)
({struct _tuple18*_tmpAC5=({struct _tuple18*_tmp608=_region_malloc(d,sizeof(*_tmp608));_tmp608->f1=_tmp636,({void*_tmpAC4=Cyc_Absyn_strctq(_tmp604);_tmp608->f2=_tmpAC4;});_tmp608;});v=_tmpAC5;});else{
# 3951
({struct _tuple18*_tmpAC7=({struct _tuple18*_tmp609=_region_malloc(d,sizeof(*_tmp609));_tmp609->f1=_tmp636,({void*_tmpAC6=Cyc_Absyn_unionq_type(_tmp604);_tmp609->f2=_tmpAC6;});_tmp609;});v=_tmpAC7;});}
({struct Cyc_Dict_Dict _tmpAC8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp635,_tmp604,v);*_tmp635=_tmpAC8;});};}else{
# 3954
struct _tuple18*_tmp60A=*_tmp607;struct _tuple18*_tmp60B=_tmp60A;struct Cyc_Absyn_Aggrdecl*_tmp60E;void*_tmp60D;_LL7: _tmp60E=_tmp60B->f1;_tmp60D=_tmp60B->f2;_LL8:;
if(_tmp60E->impl == 0){
({struct Cyc_Dict_Dict _tmpACB=({struct Cyc_Dict_Dict _tmpACA=*_tmp635;struct _tuple1*_tmpAC9=_tmp604;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpACA,_tmpAC9,({struct _tuple18*_tmp60C=_region_malloc(d,sizeof(*_tmp60C));_tmp60C->f1=_tmp636,_tmp60C->f2=_tmp60D;_tmp60C;}));});*_tmp635=_tmpACB;});
seen_defn_before=0;}else{
# 3959
seen_defn_before=1;}}{
# 3961
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->kind=_tmp636->kind,_tmp634->sc=Cyc_Absyn_Public,_tmp634->name=_tmp636->name,_tmp634->tvs=0,_tmp634->impl=0,_tmp634->expected_mem_kind=0,_tmp634->attributes=_tmp636->attributes;_tmp634;});
# 3968
if(_tmp636->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpACC=({struct Cyc_Absyn_AggrdeclImpl*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->exist_vars=0,_tmp60F->rgn_po=0,_tmp60F->fields=0,_tmp60F->tagged=0;_tmp60F;});new_ad->impl=_tmpACC;});{
# 3973
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp610=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp636->impl))->fields;for(0;_tmp610 != 0;_tmp610=_tmp610->tl){
# 3977
struct Cyc_Absyn_Aggrfield*_tmp611=(struct Cyc_Absyn_Aggrfield*)_tmp610->hd;
void*_tmp612=_tmp611->type;
struct Cyc_List_List*_tmp613=_tmp611->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp612)) && (
_tmp636->kind == Cyc_Absyn_StructA  && _tmp610->tl == 0  || _tmp636->kind == Cyc_Absyn_UnionA)){
# 3991 "toc.cyc"
void*_tmp614=Cyc_Tcutil_compress(_tmp612);void*_tmp615=_tmp614;void*_tmp61D;struct Cyc_Absyn_Tqual _tmp61C;void*_tmp61B;unsigned int _tmp61A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp615)->tag == 4U){_LLA: _tmp61D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp615)->f1).elt_type;_tmp61C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp615)->f1).tq;_tmp61B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp615)->f1).zero_term;_tmp61A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp615)->f1).zt_loc;_LLB:
# 3994
({void*_tmpACE=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->tag=4U,(_tmp616->f1).elt_type=_tmp61D,(_tmp616->f1).tq=_tmp61C,({struct Cyc_Absyn_Exp*_tmpACD=Cyc_Absyn_uint_exp(0U,0U);(_tmp616->f1).num_elts=_tmpACD;}),(_tmp616->f1).zero_term=_tmp61B,(_tmp616->f1).zt_loc=_tmp61A;_tmp616;});_tmp612=_tmpACE;});
goto _LL9;}else{_LLC: _LLD:
# 3997
({struct Cyc_List_List*_tmpAD0=({struct Cyc_List_List*_tmp618=_cycalloc(sizeof(*_tmp618));({void*_tmpACF=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->tag=6U,_tmp617->f1=0;_tmp617;});_tmp618->hd=_tmpACF;}),_tmp618->tl=_tmp613;_tmp618;});_tmp613=_tmpAD0;});
({void*_tmpAD4=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->tag=4U,({void*_tmpAD3=Cyc_Absyn_void_star_type();(_tmp619->f1).elt_type=_tmpAD3;}),({
struct Cyc_Absyn_Tqual _tmpAD2=Cyc_Absyn_empty_tqual(0U);(_tmp619->f1).tq=_tmpAD2;}),({
struct Cyc_Absyn_Exp*_tmpAD1=Cyc_Absyn_uint_exp(0U,0U);(_tmp619->f1).num_elts=_tmpAD1;}),(_tmp619->f1).zero_term=Cyc_Absyn_false_type,(_tmp619->f1).zt_loc=0U;_tmp619;});
# 3998
_tmp612=_tmpAD4;});}_LL9:;}{
# 4004
struct Cyc_Absyn_Aggrfield*_tmp61E=({struct Cyc_Absyn_Aggrfield*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->name=_tmp611->name,_tmp630->tq=Cyc_Toc_mt_tq,({
# 4006
void*_tmpAD5=Cyc_Toc_typ_to_c(_tmp612);_tmp630->type=_tmpAD5;}),_tmp630->width=_tmp611->width,_tmp630->attributes=_tmp613,_tmp630->requires_clause=0;_tmp630;});
# 4014
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp636->impl))->tagged){
void*_tmp61F=_tmp61E->type;
struct _dyneither_ptr*_tmp620=_tmp61E->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp62D=({struct Cyc_String_pa_PrintArg_struct _tmp753;_tmp753.tag=0U,_tmp753.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp636->name).f2);_tmp753;});struct Cyc_String_pa_PrintArg_struct _tmp62E=({struct Cyc_String_pa_PrintArg_struct _tmp752;_tmp752.tag=0U,_tmp752.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp620);_tmp752;});void*_tmp62B[2U];_tmp62B[0]=& _tmp62D,_tmp62B[1]=& _tmp62E;({struct _dyneither_ptr _tmpAD6=({const char*_tmp62C="_union_%s_%s";_tag_dyneither(_tmp62C,sizeof(char),13U);});Cyc_aprintf(_tmpAD6,_tag_dyneither(_tmp62B,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp62A=_cycalloc(sizeof(*_tmp62A));*_tmp62A=s;_tmp62A;});
struct Cyc_Absyn_Aggrfield*_tmp621=({struct Cyc_Absyn_Aggrfield*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->name=Cyc_Toc_val_sp,_tmp629->tq=Cyc_Toc_mt_tq,_tmp629->type=_tmp61F,_tmp629->width=0,_tmp629->attributes=0,_tmp629->requires_clause=0;_tmp629;});
struct Cyc_Absyn_Aggrfield*_tmp622=({struct Cyc_Absyn_Aggrfield*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->name=Cyc_Toc_tag_sp,_tmp628->tq=Cyc_Toc_mt_tq,_tmp628->type=Cyc_Absyn_sint_type,_tmp628->width=0,_tmp628->attributes=0,_tmp628->requires_clause=0;_tmp628;});
struct Cyc_List_List*_tmp623=({struct Cyc_Absyn_Aggrfield*_tmp627[2U];_tmp627[0]=_tmp622,_tmp627[1]=_tmp621;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp627,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp624=Cyc_Toc_make_c_struct_defn(str,_tmp623);
({struct Cyc_List_List*_tmpAD8=({struct Cyc_List_List*_tmp626=_cycalloc(sizeof(*_tmp626));({struct Cyc_Absyn_Decl*_tmpAD7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->tag=5U,_tmp625->f1=_tmp624;_tmp625;}),0U);_tmp626->hd=_tmpAD7;}),_tmp626->tl=Cyc_Toc_result_decls;_tmp626;});Cyc_Toc_result_decls=_tmpAD8;});
({void*_tmpAD9=Cyc_Absyn_strct(str);_tmp61E->type=_tmpAD9;});}
# 4026
({struct Cyc_List_List*_tmpADA=({struct Cyc_List_List*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->hd=_tmp61E,_tmp62F->tl=new_fields;_tmp62F;});new_fields=_tmpADA;});};}}
# 4028
({struct Cyc_List_List*_tmpADB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpADB;});};}
# 4031
if(!seen_defn_before)
({struct Cyc_List_List*_tmpADE=({struct Cyc_List_List*_tmp633=_cycalloc(sizeof(*_tmp633));({struct Cyc_Absyn_Decl*_tmpADD=({struct Cyc_Absyn_Decl*_tmp632=_cycalloc(sizeof(*_tmp632));({void*_tmpADC=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tag=5U,_tmp631->f1=new_ad;_tmp631;});_tmp632->r=_tmpADC;}),_tmp632->loc=0U;_tmp632;});_tmp633->hd=_tmpADD;}),_tmp633->tl=Cyc_Toc_result_decls;_tmp633;});Cyc_Toc_result_decls=_tmpADE;});
return 0;};};};}
# 4035
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple40{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4063 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 4066
struct _tuple40 _tmp638=*env;struct _tuple40 _tmp639=_tmp638;struct Cyc_Set_Set**_tmp64A;struct Cyc_Absyn_Datatypedecl*_tmp649;_LL1: _tmp64A=(_tmp639.f1)->datatypes_so_far;_tmp649=_tmp639.f2;_LL2:;{
struct _tuple1*_tmp63A=_tmp649->name;
if(_tmp649->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp64A,_tmp63A))
return 0;
({struct Cyc_Set_Set*_tmpADF=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp64A,_tmp63A);*_tmp64A=_tmpADF;});
{struct Cyc_List_List*_tmp63B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp649->fields))->v;for(0;_tmp63B != 0;_tmp63B=_tmp63B->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp63B->hd;
# 4074
struct Cyc_List_List*_tmp63C=0;
int i=1;
{struct Cyc_List_List*_tmp63D=f->typs;for(0;_tmp63D != 0;(_tmp63D=_tmp63D->tl,i ++)){
struct _dyneither_ptr*_tmp63E=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp63F=({struct Cyc_Absyn_Aggrfield*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->name=_tmp63E,_tmp641->tq=(*((struct _tuple12*)_tmp63D->hd)).f1,({
void*_tmpAE0=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp63D->hd)).f2);_tmp641->type=_tmpAE0;}),_tmp641->width=0,_tmp641->attributes=0,_tmp641->requires_clause=0;_tmp641;});
({struct Cyc_List_List*_tmpAE1=({struct Cyc_List_List*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->hd=_tmp63F,_tmp640->tl=_tmp63C;_tmp640;});_tmp63C=_tmpAE1;});}}
# 4082
({struct Cyc_List_List*_tmpAE4=({struct Cyc_List_List*_tmp643=_cycalloc(sizeof(*_tmp643));({struct Cyc_Absyn_Aggrfield*_tmpAE3=({struct Cyc_Absyn_Aggrfield*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->name=Cyc_Toc_tag_sp,_tmp642->tq=Cyc_Toc_mt_tq,_tmp642->type=Cyc_Absyn_sint_type,_tmp642->width=0,_tmp642->attributes=0,_tmp642->requires_clause=0;_tmp642;});_tmp643->hd=_tmpAE3;}),({
struct Cyc_List_List*_tmpAE2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp63C);_tmp643->tl=_tmpAE2;});_tmp643;});
# 4082
_tmp63C=_tmpAE4;});{
# 4084
struct Cyc_Absyn_Aggrdecl*_tmp644=({struct _dyneither_ptr*_tmpAE6=({struct _dyneither_ptr*_tmp648=_cycalloc(sizeof(*_tmp648));({struct _dyneither_ptr _tmpAE5=({const char*_tmp647="";_tag_dyneither(_tmp647,sizeof(char),1U);});*_tmp648=_tmpAE5;});_tmp648;});Cyc_Toc_make_c_struct_defn(_tmpAE6,_tmp63C);});
({struct _tuple1*_tmpAE7=Cyc_Toc_collapse_qvars(f->name,_tmp649->name);_tmp644->name=_tmpAE7;});
({struct Cyc_List_List*_tmpAE9=({struct Cyc_List_List*_tmp646=_cycalloc(sizeof(*_tmp646));({struct Cyc_Absyn_Decl*_tmpAE8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->tag=5U,_tmp645->f1=_tmp644;_tmp645;}),0U);_tmp646->hd=_tmpAE8;}),_tmp646->tl=Cyc_Toc_result_decls;_tmp646;});Cyc_Toc_result_decls=_tmpAE9;});};}}
# 4088
return 0;};}
# 4091
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4110 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 4113
struct _tuple40 _tmp64B=*env;struct _tuple40 _tmp64C=_tmp64B;struct Cyc_Toc_TocState*_tmp66E;struct Cyc_Absyn_Datatypedecl*_tmp66D;_LL1: _tmp66E=_tmp64C.f1;_tmp66D=_tmp64C.f2;_LL2:;
if(_tmp66D->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp64D=*_tmp66E;struct Cyc_Toc_TocState _tmp64E=_tmp64D;struct Cyc_Dict_Dict*_tmp66C;_LL4: _tmp66C=_tmp64E.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp64F=_tmp66D->name;
{struct Cyc_List_List*_tmp650=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp66D->fields))->v;for(0;_tmp650 != 0;_tmp650=_tmp650->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp650->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp651=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp652=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp651,Cyc_Absyn_false_type,0U);
# 4124
int*_tmp653=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp66C,f->name);int*_tmp654=_tmp653;if(_tmp654 == 0){_LL7: _LL8: {
# 4126
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpAEA=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpAEA;});{
# 4130
struct Cyc_Absyn_Vardecl*_tmp655=Cyc_Absyn_new_vardecl(0U,f->name,_tmp652,initopt);
_tmp655->sc=f->sc;
({struct Cyc_List_List*_tmpAEC=({struct Cyc_List_List*_tmp657=_cycalloc(sizeof(*_tmp657));({struct Cyc_Absyn_Decl*_tmpAEB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->tag=0U,_tmp656->f1=_tmp655;_tmp656;}),0U);_tmp657->hd=_tmpAEB;}),_tmp657->tl=Cyc_Toc_result_decls;_tmp657;});Cyc_Toc_result_decls=_tmpAEC;});
({struct Cyc_Dict_Dict _tmpAED=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp66C,f->name,f->sc != Cyc_Absyn_Extern);
# 4133
*_tmp66C=_tmpAED;});{
# 4135
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp658=f->typs;for(0;_tmp658 != 0;(_tmp658=_tmp658->tl,i ++)){
struct _dyneither_ptr*_tmp659=({struct _dyneither_ptr*_tmp660=_cycalloc(sizeof(*_tmp660));({struct _dyneither_ptr _tmpAEF=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp65F=({struct Cyc_Int_pa_PrintArg_struct _tmp754;_tmp754.tag=1U,_tmp754.f1=(unsigned long)i;_tmp754;});void*_tmp65D[1U];_tmp65D[0]=& _tmp65F;({struct _dyneither_ptr _tmpAEE=({const char*_tmp65E="f%d";_tag_dyneither(_tmp65E,sizeof(char),4U);});Cyc_aprintf(_tmpAEE,_tag_dyneither(_tmp65D,sizeof(void*),1U));});});*_tmp660=_tmpAEF;});_tmp660;});
struct Cyc_Absyn_Aggrfield*_tmp65A=({struct Cyc_Absyn_Aggrfield*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->name=_tmp659,_tmp65C->tq=(*((struct _tuple12*)_tmp658->hd)).f1,({
void*_tmpAF0=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp658->hd)).f2);_tmp65C->type=_tmpAF0;}),_tmp65C->width=0,_tmp65C->attributes=0,_tmp65C->requires_clause=0;_tmp65C;});
({struct Cyc_List_List*_tmpAF1=({struct Cyc_List_List*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->hd=_tmp65A,_tmp65B->tl=fields;_tmp65B;});fields=_tmpAF1;});}}
# 4143
({struct Cyc_List_List*_tmpAF5=({struct Cyc_List_List*_tmp662=_cycalloc(sizeof(*_tmp662));({struct Cyc_Absyn_Aggrfield*_tmpAF4=({struct Cyc_Absyn_Aggrfield*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->name=Cyc_Toc_tag_sp,_tmp661->tq=Cyc_Toc_mt_tq,({
void*_tmpAF3=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp661->type=_tmpAF3;}),_tmp661->width=0,_tmp661->attributes=0,_tmp661->requires_clause=0;_tmp661;});
# 4143
_tmp662->hd=_tmpAF4;}),({
# 4145
struct Cyc_List_List*_tmpAF2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp662->tl=_tmpAF2;});_tmp662;});
# 4143
fields=_tmpAF5;});{
# 4146
struct Cyc_Absyn_Aggrdecl*_tmp663=({struct _dyneither_ptr*_tmpAF7=({struct _dyneither_ptr*_tmp667=_cycalloc(sizeof(*_tmp667));({struct _dyneither_ptr _tmpAF6=({const char*_tmp666="";_tag_dyneither(_tmp666,sizeof(char),1U);});*_tmp667=_tmpAF6;});_tmp667;});Cyc_Toc_make_c_struct_defn(_tmpAF7,fields);});
({struct _tuple1*_tmpAF8=Cyc_Toc_collapse_qvars(f->name,_tmp66D->name);_tmp663->name=_tmpAF8;});
({struct Cyc_List_List*_tmpAFA=({struct Cyc_List_List*_tmp665=_cycalloc(sizeof(*_tmp665));({struct Cyc_Absyn_Decl*_tmpAF9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->tag=5U,_tmp664->f1=_tmp663;_tmp664;}),0U);_tmp665->hd=_tmpAF9;}),_tmp665->tl=Cyc_Toc_result_decls;_tmp665;});Cyc_Toc_result_decls=_tmpAFA;});
goto _LL6;};};};}}else{if(*((int*)_tmp654)== 0){_LL9: _LLA:
# 4151
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp668=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp669=Cyc_Absyn_new_vardecl(0U,f->name,_tmp652,_tmp668);
_tmp669->sc=f->sc;
({struct Cyc_List_List*_tmpAFC=({struct Cyc_List_List*_tmp66B=_cycalloc(sizeof(*_tmp66B));({struct Cyc_Absyn_Decl*_tmpAFB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->tag=0U,_tmp66A->f1=_tmp669;_tmp66A;}),0U);_tmp66B->hd=_tmpAFB;}),_tmp66B->tl=Cyc_Toc_result_decls;_tmp66B;});Cyc_Toc_result_decls=_tmpAFC;});
({struct Cyc_Dict_Dict _tmpAFD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp66C,f->name,1);*_tmp66C=_tmpAFD;});}
# 4158
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4162
return 0;};};}
# 4165
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4169
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4175
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpAFE=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpAFE;});
# 4179
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4188
struct _RegionHandle _tmp66F=_new_region("temp");struct _RegionHandle*temp=& _tmp66F;_push_region(temp);
{struct Cyc_Toc_Env*_tmp670=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp670,init);}
# 4189
;_pop_region(temp);}else{
# 4193
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4196
void*_tmp671=Cyc_Tcutil_compress(old_typ);void*_tmp672=_tmp671;void*_tmp679;struct Cyc_Absyn_Exp*_tmp678;void*_tmp677;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp672)->tag == 4U){_LL1: _tmp679=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp672)->f1).elt_type;_tmp678=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp672)->f1).num_elts;_tmp677=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp672)->f1).zero_term;_LL2:
# 4198
 if(Cyc_Tcutil_force_type2bool(0,_tmp677)){
if(_tmp678 == 0)
({void*_tmp673=0U;({struct _dyneither_ptr _tmpAFF=({const char*_tmp674="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp674,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAFF,_tag_dyneither(_tmp673,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp678;
struct Cyc_Absyn_Exp*_tmp675=({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpB01,({
struct Cyc_Absyn_Exp*_tmpB00=num_elts;Cyc_Absyn_add_exp(_tmpB00,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4205
struct Cyc_Absyn_Exp*_tmp676=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpB03=({struct Cyc_Absyn_Stmt*_tmpB02=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp675,_tmp676,0U),0U);Cyc_Toc_seq_stmt_r(_tmpB02,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4206
s->r=_tmpB03;});};}
# 4209
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4217
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp67A=d;struct Cyc_List_List*_tmp681;struct Cyc_List_List*_tmp680;void**_tmp67F;struct Cyc_Tcpat_Rhs*_tmp67E;switch(*((int*)_tmp67A)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp67E=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp67A)->f1;_LL4:
 _tmp67E->rhs=success;return d;default: _LL5: _tmp681=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp67A)->f1;_tmp680=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp67A)->f2;_tmp67F=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp67A)->f3;_LL6:
# 4222
({void*_tmpB04=Cyc_Toc_rewrite_decision(*_tmp67F,success);*_tmp67F=_tmpB04;});
for(0;_tmp680 != 0;_tmp680=_tmp680->tl){
struct _tuple32*_tmp67B=(struct _tuple32*)_tmp680->hd;struct _tuple32*_tmp67C=_tmp67B;void**_tmp67D;_LL8: _tmp67D=(void**)& _tmp67C->f2;_LL9:;
({void*_tmpB05=Cyc_Toc_rewrite_decision(*_tmp67D,success);*_tmp67D=_tmpB05;});}
# 4227
return d;}_LL0:;}
# 4238 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4240
struct _RegionHandle _tmp682=_new_region("rgn");struct _RegionHandle*rgn=& _tmp682;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp683=Cyc_Toc_share_env(rgn,nv);
void*_tmp684=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp683,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4250
struct Cyc_Absyn_Stmt*_tmp685=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4254
({void*_tmpB06=Cyc_Toc_rewrite_decision(dopt,_tmp685);dopt=_tmpB06;});{
# 4256
struct Cyc_Absyn_Switch_clause*_tmp686=({struct Cyc_Absyn_Switch_clause*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->pattern=p,_tmp692->pat_vars=0,_tmp692->where_clause=0,_tmp692->body=_tmp685,_tmp692->loc=0U;_tmp692;});
struct Cyc_List_List*_tmp687=({struct _RegionHandle*_tmpB09=rgn;struct _RegionHandle*_tmpB08=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpB09,Cyc_Toc_gen_labels,_tmpB08,({struct Cyc_Absyn_Switch_clause*_tmp691[1U];_tmp691[0]=_tmp686;({struct _RegionHandle*_tmpB07=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpB07,_tag_dyneither(_tmp691,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4259
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4263
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp683,& mydecls,& mybodies,dopt,_tmp687,v);
# 4266
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp688=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp689=_tmp688;struct Cyc_Toc_Env*_tmp68B;struct Cyc_Absyn_Stmt*_tmp68A;_LL1: _tmp68B=_tmp689->f1;_tmp68A=_tmp689->f3;_LL2:;
if(_tmp68A == _tmp685){senv=_tmp68B;goto FOUND_ENV;}}
# 4271
({void*_tmp68C=0U;({struct _dyneither_ptr _tmpB0A=({const char*_tmp68D="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp68D,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB0A,_tag_dyneither(_tmp68C,sizeof(void*),0U));});});
FOUND_ENV:
# 4275
 Cyc_Toc_stmt_to_c(senv,s);{
# 4277
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4279
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp68E=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpB0B=Cyc_Toc_temp_var();_tmp68E->name=_tmpB0B;});
({void*_tmpB0C=Cyc_Toc_typ_to_c_array(_tmp68E->type);_tmp68E->type=_tmpB0C;});
({struct Cyc_Absyn_Stmt*_tmpB0E=({struct Cyc_Absyn_Decl*_tmpB0D=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->tag=0U,_tmp68F->f1=_tmp68E;_tmp68F;}),0U);Cyc_Absyn_decl_stmt(_tmpB0D,res,0U);});res=_tmpB0E;});}
# 4286
({struct Cyc_Absyn_Stmt*_tmpB12=({struct _tuple1*_tmpB11=v;void*_tmpB10=Cyc_Toc_typ_to_c(_tmp684);struct Cyc_Absyn_Exp*_tmpB0F=e;Cyc_Absyn_declare_stmt(_tmpB11,_tmpB10,_tmpB0F,res,0U);});res=_tmpB12;});{
struct Cyc_Absyn_Stmt*_tmp690=res;_npop_handler(0U);return _tmp690;};};};};}
# 4241
;_pop_region(rgn);}
# 4294
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp693=e->r;void*_tmp694=_tmp693;struct Cyc_Absyn_MallocInfo*_tmp6BF;struct Cyc_Absyn_Stmt*_tmp6BE;void**_tmp6BD;void**_tmp6BC;struct Cyc_List_List*_tmp6BB;struct Cyc_List_List*_tmp6BA;void**_tmp6B9;struct Cyc_List_List*_tmp6B8;void**_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B6;struct Cyc_Absyn_Exp*_tmp6B5;struct Cyc_List_List*_tmp6B4;struct Cyc_Absyn_Exp*_tmp6B3;struct Cyc_Absyn_Exp*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B0;struct Cyc_Absyn_Exp*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6AE;struct Cyc_Absyn_Exp*_tmp6AD;struct Cyc_Absyn_Exp*_tmp6AC;struct Cyc_Absyn_Exp*_tmp6AB;struct Cyc_Absyn_Exp*_tmp6AA;struct Cyc_Absyn_Exp*_tmp6A9;struct Cyc_Absyn_Exp*_tmp6A8;struct Cyc_Absyn_Exp*_tmp6A7;struct Cyc_Absyn_Exp*_tmp6A6;struct Cyc_Absyn_Exp*_tmp6A5;struct Cyc_List_List*_tmp6A4;struct Cyc_Absyn_Exp*_tmp6A3;struct Cyc_Absyn_Exp*_tmp6A2;struct Cyc_Absyn_Exp*_tmp6A1;struct Cyc_Absyn_Exp*_tmp6A0;struct Cyc_Absyn_Exp*_tmp69F;struct Cyc_Absyn_Exp*_tmp69E;struct Cyc_Absyn_Exp*_tmp69D;struct Cyc_Absyn_Exp*_tmp69C;struct Cyc_Absyn_Exp*_tmp69B;switch(*((int*)_tmp694)){case 41U: _LL1: _tmp69B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL2:
 _tmp69C=_tmp69B;goto _LL4;case 20U: _LL3: _tmp69C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL4:
 _tmp69D=_tmp69C;goto _LL6;case 21U: _LL5: _tmp69D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL6:
 _tmp69E=_tmp69D;goto _LL8;case 22U: _LL7: _tmp69E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL8:
 _tmp69F=_tmp69E;goto _LLA;case 15U: _LL9: _tmp69F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LLA:
 _tmp6A0=_tmp69F;goto _LLC;case 11U: _LLB: _tmp6A0=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LLC:
 _tmp6A1=_tmp6A0;goto _LLE;case 12U: _LLD: _tmp6A1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LLE:
 _tmp6A2=_tmp6A1;goto _LL10;case 18U: _LLF: _tmp6A2=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL10:
 _tmp6A3=_tmp6A2;goto _LL12;case 5U: _LL11: _tmp6A3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp6A3);goto _LL0;case 3U: _LL13: _tmp6A4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6A4);goto _LL0;case 7U: _LL15: _tmp6A6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_tmp6A5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL16:
 _tmp6A8=_tmp6A6;_tmp6A7=_tmp6A5;goto _LL18;case 8U: _LL17: _tmp6A8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_tmp6A7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL18:
 _tmp6AA=_tmp6A8;_tmp6A9=_tmp6A7;goto _LL1A;case 9U: _LL19: _tmp6AA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_tmp6A9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL1A:
 _tmp6AC=_tmp6AA;_tmp6AB=_tmp6A9;goto _LL1C;case 23U: _LL1B: _tmp6AC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_tmp6AB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL1C:
 _tmp6AE=_tmp6AC;_tmp6AD=_tmp6AB;goto _LL1E;case 35U: _LL1D: _tmp6AE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_tmp6AD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL1E:
 _tmp6B0=_tmp6AE;_tmp6AF=_tmp6AD;goto _LL20;case 4U: _LL1F: _tmp6B0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_tmp6AF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp694)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp6B0);Cyc_Toc_exptypes_to_c(_tmp6AF);goto _LL0;case 6U: _LL21: _tmp6B3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_tmp6B2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_tmp6B1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp694)->f3;_LL22:
# 4313
 Cyc_Toc_exptypes_to_c(_tmp6B3);Cyc_Toc_exptypes_to_c(_tmp6B2);Cyc_Toc_exptypes_to_c(_tmp6B1);goto _LL0;case 10U: _LL23: _tmp6B5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_tmp6B4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL24:
# 4315
 Cyc_Toc_exptypes_to_c(_tmp6B5);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6B4);goto _LL0;case 14U: _LL25: _tmp6B7=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_tmp6B6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL26:
({void*_tmpB13=Cyc_Toc_typ_to_c(*_tmp6B7);*_tmp6B7=_tmpB13;});Cyc_Toc_exptypes_to_c(_tmp6B6);goto _LL0;case 25U: _LL27: _tmp6B9=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp694)->f1)->f3;_tmp6B8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL28:
# 4318
({void*_tmpB14=Cyc_Toc_typ_to_c(*_tmp6B9);*_tmp6B9=_tmpB14;});
_tmp6BA=_tmp6B8;goto _LL2A;case 36U: _LL29: _tmp6BA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp694)->f2;_LL2A:
 _tmp6BB=_tmp6BA;goto _LL2C;case 26U: _LL2B: _tmp6BB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL2C:
# 4322
 for(0;_tmp6BB != 0;_tmp6BB=_tmp6BB->tl){
struct _tuple19 _tmp695=*((struct _tuple19*)_tmp6BB->hd);struct _tuple19 _tmp696=_tmp695;struct Cyc_Absyn_Exp*_tmp697;_LL56: _tmp697=_tmp696.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp697);}
# 4326
goto _LL0;case 19U: _LL2D: _tmp6BC=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL2E:
 _tmp6BD=_tmp6BC;goto _LL30;case 17U: _LL2F: _tmp6BD=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL30:
({void*_tmpB15=Cyc_Toc_typ_to_c(*_tmp6BD);*_tmp6BD=_tmpB15;});goto _LL0;case 37U: _LL31: _tmp6BE=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp6BE);goto _LL0;case 34U: _LL33: _tmp6BF=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp694)->f1;_LL34:
# 4331
 if(_tmp6BF->elt_type != 0)
({void**_tmpB17=({void**_tmp698=_cycalloc(sizeof(*_tmp698));({void*_tmpB16=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp6BF->elt_type)));*_tmp698=_tmpB16;});_tmp698;});_tmp6BF->elt_type=_tmpB17;});
Cyc_Toc_exptypes_to_c(_tmp6BF->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4341
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
# 4352
({void*_tmp699=0U;({unsigned int _tmpB19=e->loc;struct _dyneither_ptr _tmpB18=({const char*_tmp69A="Cyclone expression within C code";_tag_dyneither(_tmp69A,sizeof(char),33U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpB19,_tmpB18,_tag_dyneither(_tmp699,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4356
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp6C0=d->r;void*_tmp6C1=_tmp6C0;struct Cyc_Absyn_Typedefdecl*_tmp6CB;struct Cyc_Absyn_Enumdecl*_tmp6CA;struct Cyc_Absyn_Aggrdecl*_tmp6C9;struct Cyc_Absyn_Fndecl*_tmp6C8;struct Cyc_Absyn_Vardecl*_tmp6C7;switch(*((int*)_tmp6C1)){case 0U: _LL1: _tmp6C7=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6C1)->f1;_LL2:
# 4359
({void*_tmpB1A=Cyc_Toc_typ_to_c(_tmp6C7->type);_tmp6C7->type=_tmpB1A;});
if(_tmp6C7->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6C7->initializer));
goto _LL0;case 1U: _LL3: _tmp6C8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6C1)->f1;_LL4:
# 4363
({void*_tmpB1B=Cyc_Toc_typ_to_c(_tmp6C8->ret_type);_tmp6C8->ret_type=_tmpB1B;});
{struct Cyc_List_List*_tmp6C2=_tmp6C8->args;for(0;_tmp6C2 != 0;_tmp6C2=_tmp6C2->tl){
({void*_tmpB1C=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp6C2->hd)).f3);(*((struct _tuple9*)_tmp6C2->hd)).f3=_tmpB1C;});}}
# 4367
goto _LL0;case 5U: _LL5: _tmp6C9=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6C1)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp6C9);goto _LL0;case 7U: _LL7: _tmp6CA=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C1)->f1;_LL8:
# 4370
 if(_tmp6CA->fields != 0){
struct Cyc_List_List*_tmp6C3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6CA->fields))->v;for(0;_tmp6C3 != 0;_tmp6C3=_tmp6C3->tl){
struct Cyc_Absyn_Enumfield*_tmp6C4=(struct Cyc_Absyn_Enumfield*)_tmp6C3->hd;
if(_tmp6C4->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6C4->tag));}}
# 4375
goto _LL0;case 8U: _LL9: _tmp6CB=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6C1)->f1;_LLA:
({void*_tmpB1D=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6CB->defn));_tmp6CB->defn=_tmpB1D;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4385
({void*_tmp6C5=0U;({unsigned int _tmpB1F=d->loc;struct _dyneither_ptr _tmpB1E=({const char*_tmp6C6="Cyclone declaration within C code";_tag_dyneither(_tmp6C6,sizeof(char),34U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpB1F,_tmpB1E,_tag_dyneither(_tmp6C5,sizeof(void*),0U));});});goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
 goto _LL0;}_LL0:;}
# 4391
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp6CC=s->r;void*_tmp6CD=_tmp6CC;struct Cyc_Absyn_Stmt*_tmp6E5;struct Cyc_Absyn_Stmt*_tmp6E4;struct Cyc_Absyn_Exp*_tmp6E3;struct Cyc_Absyn_Decl*_tmp6E2;struct Cyc_Absyn_Stmt*_tmp6E1;struct Cyc_Absyn_Exp*_tmp6E0;struct Cyc_List_List*_tmp6DF;void*_tmp6DE;struct Cyc_Absyn_Exp*_tmp6DD;struct Cyc_Absyn_Exp*_tmp6DC;struct Cyc_Absyn_Exp*_tmp6DB;struct Cyc_Absyn_Stmt*_tmp6DA;struct Cyc_Absyn_Exp*_tmp6D9;struct Cyc_Absyn_Stmt*_tmp6D8;struct Cyc_Absyn_Exp*_tmp6D7;struct Cyc_Absyn_Stmt*_tmp6D6;struct Cyc_Absyn_Stmt*_tmp6D5;struct Cyc_Absyn_Exp*_tmp6D4;struct Cyc_Absyn_Stmt*_tmp6D3;struct Cyc_Absyn_Stmt*_tmp6D2;struct Cyc_Absyn_Exp*_tmp6D1;switch(*((int*)_tmp6CD)){case 1U: _LL1: _tmp6D1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp6D1);goto _LL0;case 2U: _LL3: _tmp6D3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f1;_tmp6D2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp6D3);Cyc_Toc_stmttypes_to_c(_tmp6D2);goto _LL0;case 3U: _LL5: _tmp6D4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f1;_LL6:
 if(_tmp6D4 != 0)Cyc_Toc_exptypes_to_c(_tmp6D4);goto _LL0;case 4U: _LL7: _tmp6D7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f1;_tmp6D6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f2;_tmp6D5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f3;_LL8:
# 4397
 Cyc_Toc_exptypes_to_c(_tmp6D7);Cyc_Toc_stmttypes_to_c(_tmp6D6);Cyc_Toc_stmttypes_to_c(_tmp6D5);goto _LL0;case 5U: _LL9: _tmp6D9=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f1).f1;_tmp6D8=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f2;_LLA:
# 4399
 Cyc_Toc_exptypes_to_c(_tmp6D9);Cyc_Toc_stmttypes_to_c(_tmp6D8);goto _LL0;case 9U: _LLB: _tmp6DD=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f1;_tmp6DC=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f2).f1;_tmp6DB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f3).f1;_tmp6DA=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f4;_LLC:
# 4401
 Cyc_Toc_exptypes_to_c(_tmp6DD);Cyc_Toc_exptypes_to_c(_tmp6DC);Cyc_Toc_exptypes_to_c(_tmp6DB);
Cyc_Toc_stmttypes_to_c(_tmp6DA);goto _LL0;case 10U: _LLD: _tmp6E0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f1;_tmp6DF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f2;_tmp6DE=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f3;_LLE:
# 4404
 Cyc_Toc_exptypes_to_c(_tmp6E0);
for(0;_tmp6DF != 0;_tmp6DF=_tmp6DF->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp6DF->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp6E2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f1;_tmp6E1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp6E2);Cyc_Toc_stmttypes_to_c(_tmp6E1);goto _LL0;case 14U: _LL11: _tmp6E4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f1;_tmp6E3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp6E4);Cyc_Toc_exptypes_to_c(_tmp6E3);goto _LL0;case 13U: _LL13: _tmp6E5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp6CD)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp6E5);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4414
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4417
({void*_tmpB20=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->tag=0U;_tmp6CE;});s->r=_tmpB20;});
goto _LL0;default: _LL1F: _LL20:
# 4420
({void*_tmp6CF=0U;({unsigned int _tmpB22=s->loc;struct _dyneither_ptr _tmpB21=({const char*_tmp6D0="Cyclone statement in C code";_tag_dyneither(_tmp6D0,sizeof(char),28U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpB22,_tmpB21,_tag_dyneither(_tmp6CF,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4424
static void*Cyc_Toc_decl_to_enumtype(struct Cyc_Absyn_Decl*d){
void*_tmp6E6=d->r;void*_tmp6E7=_tmp6E6;struct Cyc_Absyn_Enumdecl*_tmp6EC;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6E7)->tag == 7U){_LL1: _tmp6EC=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6E7)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->tag=0U,({void*_tmpB23=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->tag=15U,_tmp6E8->f1=_tmp6EC->name,_tmp6E8->f2=_tmp6EC;_tmp6E8;});_tmp6E9->f1=_tmpB23;}),_tmp6E9->f2=0;_tmp6E9;});}else{_LL3: _LL4:
({void*_tmp6EA=0U;({struct _dyneither_ptr _tmpB24=({const char*_tmp6EB="Toc::get_enumdecl_name";_tag_dyneither(_tmp6EB,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB24,_tag_dyneither(_tmp6EA,sizeof(void*),0U));});});}_LL0:;}
# 4431
static struct Cyc_Absyn_Decl*Cyc_Toc_type2enumdecl(void*t){
void*_tmp6ED=t;struct Cyc_Absyn_Enumdecl*_tmp6F1;unsigned int _tmp6F0;if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6ED)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6ED)->f1)->r)->tag == 1U){_LL1: _tmp6F1=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6ED)->f1)->r)->f1;_tmp6F0=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6ED)->f1)->loc;_LL2:
# 4434
 return({struct Cyc_Absyn_Decl*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));({void*_tmpB25=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp6EE=_cycalloc(sizeof(*_tmp6EE));_tmp6EE->tag=7U,_tmp6EE->f1=_tmp6F1;_tmp6EE;});_tmp6EF->r=_tmpB25;}),_tmp6EF->loc=_tmp6F0;_tmp6EF;});}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 4442
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp6F2=0U;({struct _dyneither_ptr _tmpB26=({const char*_tmp6F3="decls_to_c: not at toplevel!";_tag_dyneither(_tmp6F3,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB26,_tag_dyneither(_tmp6F2,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp6F4=d->r;void*_tmp6F5=_tmp6F4;struct Cyc_List_List*_tmp710;struct Cyc_List_List*_tmp70F;struct Cyc_List_List*_tmp70E;struct Cyc_List_List*_tmp70D;struct Cyc_Absyn_Typedefdecl*_tmp70C;struct Cyc_Absyn_Enumdecl*_tmp70B;struct Cyc_Absyn_Datatypedecl*_tmp70A;struct Cyc_Absyn_Aggrdecl*_tmp709;struct Cyc_Absyn_Fndecl*_tmp708;struct Cyc_Absyn_Vardecl*_tmp707;switch(*((int*)_tmp6F5)){case 0U: _LL1: _tmp707=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6F5)->f1;_LL2: {
# 4450
struct _tuple1*_tmp6F6=_tmp707->name;
# 4452
if(_tmp707->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpB28=({struct _tuple1*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));({union Cyc_Absyn_Nmspace _tmpB27=Cyc_Absyn_Abs_n(0,1);_tmp6F7->f1=_tmpB27;}),_tmp6F7->f2=(*_tmp6F6).f2;_tmp6F7;});_tmp6F6=_tmpB28;});
if(_tmp707->initializer != 0){
if(_tmp707->sc == Cyc_Absyn_ExternC)_tmp707->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp707->initializer));else{
# 4459
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp707->initializer));}}
# 4461
_tmp707->name=_tmp6F6;
({enum Cyc_Absyn_Scope _tmpB29=Cyc_Toc_scope_to_c(_tmp707->sc);_tmp707->sc=_tmpB29;});
({void*_tmpB2A=Cyc_Toc_typ_to_c(_tmp707->type);_tmp707->type=_tmpB2A;});
({struct Cyc_List_List*_tmpB2B=({struct Cyc_List_List*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8->hd=d,_tmp6F8->tl=Cyc_Toc_result_decls;_tmp6F8;});Cyc_Toc_result_decls=_tmpB2B;});
goto _LL0;}case 1U: _LL3: _tmp708=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6F5)->f1;_LL4: {
# 4467
struct _tuple1*_tmp6F9=_tmp708->name;
# 4469
if(_tmp708->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB2D=({struct _tuple1*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));({union Cyc_Absyn_Nmspace _tmpB2C=Cyc_Absyn_Abs_n(0,1);_tmp6FA->f1=_tmpB2C;}),_tmp6FA->f2=(*_tmp6F9).f2;_tmp6FA;});_tmp6F9=_tmpB2D;});
_tmp708->sc=Cyc_Absyn_Public;}
# 4473
_tmp708->name=_tmp6F9;
Cyc_Toc_fndecl_to_c(nv,_tmp708,cinclude);
({struct Cyc_List_List*_tmpB2E=({struct Cyc_List_List*_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB->hd=d,_tmp6FB->tl=Cyc_Toc_result_decls;_tmp6FB;});Cyc_Toc_result_decls=_tmpB2E;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp6FC=0U;({struct _dyneither_ptr _tmpB2F=({const char*_tmp6FD="letdecl at toplevel";_tag_dyneither(_tmp6FD,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB2F,_tag_dyneither(_tmp6FC,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp6FE=0U;({struct _dyneither_ptr _tmpB30=({const char*_tmp6FF="region decl at toplevel";_tag_dyneither(_tmp6FF,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB30,_tag_dyneither(_tmp6FE,sizeof(void*),0U));});});case 5U: _LLB: _tmp709=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6F5)->f1;_LLC:
# 4481
 Cyc_Toc_aggrdecl_to_c(_tmp709);
goto _LL0;case 6U: _LLD: _tmp70A=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp6F5)->f1;_LLE:
# 4484
 if(_tmp70A->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp70A);else{
# 4487
Cyc_Toc_datatypedecl_to_c(_tmp70A);}
goto _LL0;case 7U: _LLF: _tmp70B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6F5)->f1;_LL10:
# 4490
 Cyc_Toc_enumdecl_to_c(_tmp70B);
({struct Cyc_List_List*_tmpB31=({struct Cyc_List_List*_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700->hd=d,_tmp700->tl=Cyc_Toc_result_decls;_tmp700;});Cyc_Toc_result_decls=_tmpB31;});
goto _LL0;case 8U: _LL11: _tmp70C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6F5)->f1;_LL12:
# 4494
 _tmp70C->tvs=0;
if(_tmp70C->defn != 0){
void*_tmp701=Cyc_Toc_typ_to_c((void*)_check_null(_tmp70C->defn));
_tmp70C->defn=_tmp701;{
# 4500
struct Cyc_Absyn_Decl*_tmp702=Cyc_Toc_type2enumdecl(_tmp701);
if(_tmp702 != 0){
({struct Cyc_List_List*_tmpB32=({struct Cyc_List_List*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703->hd=_tmp702,_tmp703->tl=Cyc_Toc_result_decls;_tmp703;});Cyc_Toc_result_decls=_tmpB32;});
({void*_tmpB33=Cyc_Toc_decl_to_enumtype(_tmp702);_tmp70C->defn=_tmpB33;});}};}else{
# 4506
enum Cyc_Absyn_KindQual _tmp704=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp70C->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp705=_tmp704;if(_tmp705 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpB34=Cyc_Absyn_void_star_type();_tmp70C->defn=_tmpB34;});goto _LL1F;}else{_LL22: _LL23:
 _tmp70C->defn=Cyc_Absyn_void_type;goto _LL1F;}_LL1F:;}
# 4514
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpB35=({struct Cyc_List_List*_tmp706=_cycalloc(sizeof(*_tmp706));_tmp706->hd=d,_tmp706->tl=Cyc_Toc_result_decls;_tmp706;});Cyc_Toc_result_decls=_tmpB35;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp70D=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6F5)->f2;_LL18:
 _tmp70E=_tmp70D;goto _LL1A;case 10U: _LL19: _tmp70E=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6F5)->f2;_LL1A:
 _tmp70F=_tmp70E;goto _LL1C;case 11U: _LL1B: _tmp70F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6F5)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpB36=Cyc_Toc_decls_to_c(r,nv,_tmp70F,top,cinclude);nv=_tmpB36;});goto _LL0;default: _LL1D: _tmp710=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6F5)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpB37=Cyc_Toc_decls_to_c(r,nv,_tmp710,top,1);nv=_tmpB37;});goto _LL0;}_LL0:;};}
# 4525
return nv;}
# 4529
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp711=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp712=_tmp711;struct Cyc_Core_DynamicRegion*_tmp717;_LL1: _tmp717=_tmp712.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp717->h;
({struct Cyc_Toc_TocState*_tmpB38=Cyc_Toc_empty_toc_state(h);ts=_tmpB38;});;}
({struct Cyc_Toc_TocStateWrap*_tmpB39=({struct Cyc_Toc_TocStateWrap*_tmp714=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp714));_tmp714->dyn=_tmp717,_tmp714->state=ts;_tmp714;});Cyc_Toc_toc_state=_tmpB39;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
({struct _dyneither_ptr _tmpB3A=_tag_dyneither(({unsigned int _tmp716=36;struct _dyneither_ptr**_tmp715=_cycalloc(_check_times(_tmp716,sizeof(struct _dyneither_ptr*)));_tmp715[0]=& Cyc_Toc__throw_str,_tmp715[1]=& Cyc_Toc_setjmp_str,_tmp715[2]=& Cyc_Toc__push_handler_str,_tmp715[3]=& Cyc_Toc__pop_handler_str,_tmp715[4]=& Cyc_Toc__exn_thrown_str,_tmp715[5]=& Cyc_Toc__npop_handler_str,_tmp715[6]=& Cyc_Toc__check_null_str,_tmp715[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp715[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp715[9]=& Cyc_Toc__check_known_subscript_notnullX_str,_tmp715[10]=& Cyc_Toc__check_dyneither_subscript_str,_tmp715[11]=& Cyc_Toc__dyneither_ptr_str,_tmp715[12]=& Cyc_Toc__tag_dyneither_str,_tmp715[13]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp715[14]=& Cyc_Toc__get_dyneither_size_str,_tmp715[15]=& Cyc_Toc__get_zero_arr_size_str,_tmp715[16]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp715[17]=& Cyc_Toc__zero_arr_plus_str,_tmp715[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp715[19]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp715[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp715[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp715[22]=& Cyc_Toc__cycalloc_str,_tmp715[23]=& Cyc_Toc__cyccalloc_str,_tmp715[24]=& Cyc_Toc__cycalloc_atomic_str,_tmp715[25]=& Cyc_Toc__cyccalloc_atomic_str,_tmp715[26]=& Cyc_Toc__region_malloc_str,_tmp715[27]=& Cyc_Toc__region_calloc_str,_tmp715[28]=& Cyc_Toc__check_times_str,_tmp715[29]=& Cyc_Toc__new_region_str,_tmp715[30]=& Cyc_Toc__push_region_str,_tmp715[31]=& Cyc_Toc__pop_region_str,_tmp715[32]=& Cyc_Toc__throw_arraybounds_str,_tmp715[33]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp715[34]=& Cyc_Toc__throw_match_str,_tmp715[35]=& Cyc_Toc__fast_region_malloc_str;_tmp715;}),sizeof(struct _dyneither_ptr*),36U);Cyc_Toc_globals=_tmpB3A;});};}
# 4579
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp718=ts;struct Cyc_Toc_TocStateWrap*_tmp719=Cyc_Toc_toc_state;ts=_tmp719;Cyc_Toc_toc_state=_tmp718;});{
struct Cyc_Toc_TocStateWrap _tmp71A=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp71B=_tmp71A;struct Cyc_Core_DynamicRegion*_tmp721;struct Cyc_Toc_TocState*_tmp720;_LL1: _tmp721=_tmp71B.dyn;_tmp720=_tmp71B.state;_LL2:;
# 4584
{struct _RegionHandle*h=& _tmp721->h;
{struct Cyc_Toc_TocState _tmp71D=*_tmp720;struct Cyc_Toc_TocState _tmp71E=_tmp71D;struct Cyc_Xarray_Xarray*_tmp71F;_LL4: _tmp71F=_tmp71E.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp71F);}
# 4585
;}
# 4588
Cyc_Core_free_rckey(_tmp721);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4591
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4597
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4599
({struct Cyc_Hashtable_Table**_tmpB3B=({struct Cyc_Hashtable_Table**_tmp722=_cycalloc(sizeof(*_tmp722));*_tmp722=pop_tables;_tmp722;});Cyc_Toc_gpop_tables=_tmpB3B;});
Cyc_Toc_init();
{struct _RegionHandle _tmp723=_new_region("start");struct _RegionHandle*start=& _tmp723;_push_region(start);
({struct _RegionHandle*_tmpB3D=start;struct Cyc_Toc_Env*_tmpB3C=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB3D,_tmpB3C,ds,1,0);});;_pop_region(start);}
# 4604
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}

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
# 56
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
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
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 35 "insert_checks.h"
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
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;
void Cyc_Toc_finish();struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 61 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 76 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 78
({struct _dyneither_ptr _tmp79E=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp79D=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp79D,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp79E,ap);});}
# 80
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 82
({struct _dyneither_ptr _tmp7A0=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp79F=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp79F,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7A0,ap);});}
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
({struct Cyc_List_List**_tmp7AC=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp7AC;}),({
struct Cyc_List_List**_tmp7AB=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp7AB;}),({
struct Cyc_Dict_Dict*_tmp7AA=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp7A9=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp7A9;});_tmpC;});_tmp11->aggrs_so_far=_tmp7AA;}),({
struct Cyc_List_List**_tmp7A8=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp7A8;}),({
struct Cyc_Set_Set**_tmp7A7=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp7A6=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp7A6;});_tmpE;});_tmp11->datatypes_so_far=_tmp7A7;}),({
struct Cyc_Dict_Dict*_tmp7A5=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp7A4=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp7A4;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp7A5;}),({
struct Cyc_Dict_Dict*_tmp7A3=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp7A2=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp7A2;});_tmp10;});_tmp11->qvar_tags=_tmp7A3;}),({
struct Cyc_Xarray_Xarray*_tmp7A1=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp7A1;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 160
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 166
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 169
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp7AD=({struct Cyc_Toc_TocStateWrap _tmp767;_tmp767.dyn=_tmp1C,_tmp767.state=_tmp1B;_tmp767;});*ts=_tmp7AD;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp1A=({struct _tuple16 _tmp768;_tmp768.f1=_tmp1B,_tmp768.f2=arg;_tmp768;});
({void*_tmp7AE=f(h,& _tmp1A);res=_tmp7AE;});}
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
struct _tuple17 env=({struct _tuple17 _tmp769;_tmp769.f1=q,_tmp769.f2=type_maker;_tmp769;});
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
({void*_tmp7AF=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0),0);_tmp6D=_tmp7AF;});
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
return({struct _tuple20*_tmp70=_cycalloc(sizeof(*_tmp70));({struct Cyc_List_List*_tmp7B1=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));({void*_tmp7B0=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=1U,_tmp6E->f1=name;_tmp6E;});_tmp6F->hd=_tmp7B0;}),_tmp6F->tl=0;_tmp6F;});_tmp70->f1=_tmp7B1;}),_tmp70->f2=e;_tmp70;});}
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
({void*_tmp87=0U;({struct _dyneither_ptr _tmp7B2=({const char*_tmp88="impossible IntCon (not char, short or int)";_tag_dyneither(_tmp88,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7B2,_tag_dyneither(_tmp87,sizeof(void*),0U));});});}_LLD:;}
# 438
goto _LL0;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}default: goto _LLB;}case 3U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp76A;_tmp76A.tag=0U,({struct _dyneither_ptr _tmp7B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp76A.f1=_tmp7B3;});_tmp76A;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({struct _dyneither_ptr _tmp7B4=({const char*_tmp8A="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8A,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7B4,_tag_dyneither(_tmp89,sizeof(void*),1U));});});}_LL0:;}
# 445
return function;}
# 447
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8D=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8E=_tmp8D;void*_tmp91;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->tag == 3U){_LL1: _tmp91=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp91);}else{_LL3: _LL4:
({void*_tmp8F=0U;({struct _dyneither_ptr _tmp7B5=({const char*_tmp90="impossible type (not pointer)";_tag_dyneither(_tmp90,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7B5,_tag_dyneither(_tmp8F,sizeof(void*),0U));});});}_LL0:;}
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
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp76C;_tmp76C.tag=0U,_tmp76C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp76C;});struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp76B;_tmp76B.tag=0U,_tmp76B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp76B;});void*_tmpA2[2U];_tmpA2[0]=& _tmpA4,_tmpA2[1]=& _tmpA5;({struct _dyneither_ptr _tmp7B6=({const char*_tmpA3="%s_%s_struct";_tag_dyneither(_tmpA3,sizeof(char),13U);});Cyc_aprintf(_tmp7B6,_tag_dyneither(_tmpA2,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA6=_new_region("r");struct _RegionHandle*r=& _tmpA6;_push_region(r);
{struct _dyneither_ptr _tmpAD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp76F;_tmp76F.tag=0U,_tmp76F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp76F;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp76E;_tmp76E.tag=0U,({struct _dyneither_ptr _tmp7B9=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp7B8=r;struct Cyc_List_List*_tmp7B7=_tmpB0;Cyc_rstr_sepstr(_tmp7B8,_tmp7B7,({const char*_tmpAC="_";_tag_dyneither(_tmpAC,sizeof(char),2U);}));}));_tmp76E.f1=_tmp7B9;});_tmp76E;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp76D;_tmp76D.tag=0U,_tmp76D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp76D;});void*_tmpA7[3U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA,_tmpA7[2]=& _tmpAB;({struct _dyneither_ptr _tmp7BA=({const char*_tmpA8="%s_%s_%s_struct";_tag_dyneither(_tmpA8,sizeof(char),16U);});Cyc_aprintf(_tmp7BA,_tag_dyneither(_tmpA7,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAD;};_pop_region(r);};}_LL3:;};}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
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
struct _tuple1*res=({struct _tuple1*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->f1=_tmpC1,({struct _dyneither_ptr*_tmp7BB=({struct _dyneither_ptr*_tmpBE=_cycalloc(sizeof(*_tmpBE));*_tmpBE=newvar;_tmpBE;});_tmpBF->f2=_tmp7BB;});_tmpBF;});
({struct Cyc_Dict_Dict _tmp7BC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC7,_tmpBC,res);*_tmpC7=_tmp7BC;});
return res;};}}else{_LL9: _tmpC3=_tmpBB;_LLA:(int)_rethrow(_tmpC3);}_LL6:;}};};};}
# 530
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp770;_tmp770.f1=fieldname,_tmp770.f2=dtname;_tmp770;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 537
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->kind=Cyc_Absyn_StructA,_tmpCA->sc=Cyc_Absyn_Public,_tmpCA->tvs=0,_tmpCA->attributes=0,_tmpCA->expected_mem_kind=0,({
# 540
struct _tuple1*_tmp7BF=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));({union Cyc_Absyn_Nmspace _tmp7BE=Cyc_Absyn_Rel_n(0);_tmpC8->f1=_tmp7BE;}),_tmpC8->f2=name;_tmpC8;});_tmpCA->name=_tmp7BF;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp7BD=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->exist_vars=0,_tmpC9->rgn_po=0,_tmpC9->tagged=0,_tmpC9->fields=fs;_tmpC9;});_tmpCA->impl=_tmp7BD;});_tmpCA;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
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
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct _dyneither_ptr _tmp7C1=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDF=({struct Cyc_Int_pa_PrintArg_struct _tmp771;_tmp771.tag=1U,_tmp771.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp771;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({struct _dyneither_ptr _tmp7C0=({const char*_tmpDE="_tuple%d";_tag_dyneither(_tmpDE,sizeof(char),9U);});Cyc_aprintf(_tmp7C0,_tag_dyneither(_tmpDD,sizeof(void*),1U));});});*_tmpE0=_tmp7C1;});_tmpE0;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE1);
struct Cyc_List_List*_tmpD3=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp7C4=({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct Cyc_Absyn_Aggrfield*_tmp7C3=({struct Cyc_Absyn_Aggrfield*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct _dyneither_ptr*_tmp7C2=Cyc_Absyn_fieldname(i);_tmpD4->name=_tmp7C2;}),_tmpD4->tq=Cyc_Toc_mt_tq,_tmpD4->type=(void*)ts2->hd,_tmpD4->width=0,_tmpD4->attributes=0,_tmpD4->requires_clause=0;_tmpD4;});_tmpD5->hd=_tmp7C3;}),_tmpD5->tl=_tmpD3;_tmpD5;});_tmpD3=_tmp7C4;});}}
({struct Cyc_List_List*_tmp7C5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD3);_tmpD3=_tmp7C5;});{
struct Cyc_Absyn_Aggrdecl*_tmpD6=Cyc_Toc_make_c_struct_defn(xname,_tmpD3);
void*_tmpD7=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDC=_cycalloc(sizeof(*_tmpDC));*_tmpDC=_tmpD6;_tmpDC;})),0);
({struct Cyc_List_List*_tmp7C7=({struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct Cyc_Absyn_Decl*_tmp7C6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=5U,_tmpD8->f1=_tmpD6;_tmpD8;}),0U);_tmpD9->hd=_tmp7C6;}),_tmpD9->tl=Cyc_Toc_result_decls;_tmpD9;});Cyc_Toc_result_decls=_tmp7C7;});
({struct Cyc_List_List*_tmp7C9=({struct Cyc_List_List*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));({struct _tuple23*_tmp7C8=({struct _tuple23*_tmpDA=_region_malloc(d,sizeof(*_tmpDA));_tmpDA->f1=_tmpD7,_tmpDA->f2=ts;_tmpDA;});_tmpDB->hd=_tmp7C8;}),_tmpDB->tl=*_tmpE2;_tmpDB;});*_tmpE2=_tmp7C9;});
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
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF4=_cycalloc(sizeof(*_tmpF4));({struct _dyneither_ptr _tmp7CB=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF3=({struct Cyc_Int_pa_PrintArg_struct _tmp772;_tmp772.tag=1U,_tmp772.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp772;});void*_tmpF1[1U];_tmpF1[0]=& _tmpF3;({struct _dyneither_ptr _tmp7CA=({const char*_tmpF2="_tuple%d";_tag_dyneither(_tmpF2,sizeof(char),9U);});Cyc_aprintf(_tmp7CA,_tag_dyneither(_tmpF1,sizeof(void*),1U));});});*_tmpF4=_tmp7CB;});_tmpF4;});
struct Cyc_Absyn_Aggrdecl*_tmpEA=Cyc_Toc_make_c_struct_defn(xname,_tmpF5);
_tmpEA->kind=_tmpF6;{
void*_tmpEB=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF0=_cycalloc(sizeof(*_tmpF0));*_tmpF0=_tmpEA;_tmpF0;})),0);
({struct Cyc_List_List*_tmp7CD=({struct Cyc_List_List*_tmpED=_cycalloc(sizeof(*_tmpED));({struct Cyc_Absyn_Decl*_tmp7CC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->tag=5U,_tmpEC->f1=_tmpEA;_tmpEC;}),0U);_tmpED->hd=_tmp7CC;}),_tmpED->tl=Cyc_Toc_result_decls;_tmpED;});Cyc_Toc_result_decls=_tmp7CD;});
({struct Cyc_List_List*_tmp7CF=({struct Cyc_List_List*_tmpEF=_region_malloc(d,sizeof(*_tmpEF));({struct _tuple26*_tmp7CE=({struct _tuple26*_tmpEE=_region_malloc(d,sizeof(*_tmpEE));_tmpEE->f1=_tmpEB,_tmpEE->f2=_tmpF6,_tmpEE->f3=_tmpF5;_tmpEE;});_tmpEF->hd=_tmp7CE;}),_tmpEF->tl=*_tmpF7;_tmpEF;});*_tmpF7=_tmp7CF;});
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
int _tmp7D0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11A);_tmp7D0 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp104);})){
int okay=1;
{struct Cyc_List_List*_tmpFE=_tmp11A;for(0;_tmpFE != 0;(_tmpFE=_tmpFE->tl,_tmp104=_tmp104->tl)){
void*_tmpFF=(void*)_tmpFE->hd;
void*_tmp100=(void*)((struct Cyc_List_List*)_check_null(_tmp104))->hd;
{struct Cyc_Absyn_Kind*_tmp101=Cyc_Tcutil_type_kind(_tmpFF);struct Cyc_Absyn_Kind*_tmp102=_tmp101;switch(((struct Cyc_Absyn_Kind*)_tmp102)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 638
 continue;default: _LLB: _LLC:
# 641
 if(Cyc_Tcutil_unify(_tmpFF,_tmp100) || ({void*_tmp7D1=Cyc_Toc_typ_to_c(_tmpFF);Cyc_Tcutil_unify(_tmp7D1,Cyc_Toc_typ_to_c(_tmp100));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 646
break;}}
# 648
if(okay)
return _tmp103;}}}{
# 655
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp118=_cycalloc(sizeof(*_tmp118));({struct _dyneither_ptr _tmp7D3=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp117=({struct Cyc_Int_pa_PrintArg_struct _tmp773;_tmp773.tag=1U,_tmp773.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp773;});void*_tmp115[1U];_tmp115[0]=& _tmp117;({struct _dyneither_ptr _tmp7D2=({const char*_tmp116="_tuple%d";_tag_dyneither(_tmp116,sizeof(char),9U);});Cyc_aprintf(_tmp7D2,_tag_dyneither(_tmp115,sizeof(void*),1U));});});*_tmp118=_tmp7D3;});_tmp118;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp106=0;
# 659
({struct Cyc_List_List*_tmp7D5=({struct Cyc_List_List*_tmp108=_region_malloc(d,sizeof(*_tmp108));({struct _tuple29*_tmp7D4=({struct _tuple29*_tmp107=_region_malloc(d,sizeof(*_tmp107));_tmp107->f1=_tmp11C,_tmp107->f2=_tmp11A,_tmp107->f3=x;_tmp107;});_tmp108->hd=_tmp7D4;}),_tmp108->tl=*_tmp11D;_tmp108;});*_tmp11D=_tmp7D5;});{
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
({struct Cyc_List_List*_tmp7D7=({struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*_tmp10D));({void*_tmp7D6=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->tag=6U,_tmp10C->f1=0;_tmp10C;});_tmp10D->hd=_tmp7D6;}),_tmp10D->tl=atts;_tmp10D;});atts=_tmp7D7;});
({void*_tmp7D8=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10A,t));t=_tmp7D8;});
# 679
if(Cyc_Tcutil_unify(t,Cyc_Absyn_void_type))
({void*_tmp7DC=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=4U,({void*_tmp7DB=Cyc_Absyn_void_star_type();(_tmp10E->f1).elt_type=_tmp7DB;}),({struct Cyc_Absyn_Tqual _tmp7DA=Cyc_Absyn_empty_tqual(0U);(_tmp10E->f1).tq=_tmp7DA;}),({
struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Absyn_uint_exp(0U,0U);(_tmp10E->f1).num_elts=_tmp7D9;}),(_tmp10E->f1).zero_term=Cyc_Absyn_false_type,(_tmp10E->f1).zt_loc=0U;_tmp10E;});
# 680
t=_tmp7DC;});
# 683
({struct Cyc_List_List*_tmp7DE=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Absyn_Aggrfield*_tmp7DD=({struct Cyc_Absyn_Aggrfield*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->name=_tmp10B->name,_tmp10F->tq=Cyc_Toc_mt_tq,_tmp10F->type=t,_tmp10F->width=_tmp10B->width,_tmp10F->attributes=atts,_tmp10F->requires_clause=0;_tmp10F;});_tmp110->hd=_tmp7DD;}),_tmp110->tl=_tmp106;_tmp110;});_tmp106=_tmp7DE;});}
# 685
({struct Cyc_List_List*_tmp7DF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp106);_tmp106=_tmp7DF;});{
struct Cyc_Absyn_Aggrdecl*_tmp111=Cyc_Toc_make_c_struct_defn(xname,_tmp106);
({struct Cyc_List_List*_tmp7E1=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Decl*_tmp7E0=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=5U,_tmp112->f1=_tmp111;_tmp112;}),0U);_tmp113->hd=_tmp7E0;}),_tmp113->tl=Cyc_Toc_result_decls;_tmp113;});Cyc_Toc_result_decls=_tmp7E1;});{
void*_tmp114=x;_npop_handler(0U);return _tmp114;};};}
# 663
;_pop_region(r);};};}
# 691
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 695
struct _tuple27 env=({struct _tuple27 _tmp774;_tmp774.f1=struct_name,_tmp774.f2=type_vars,_tmp774.f3=type_args,_tmp774.f4=fields;_tmp774;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 703
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp7E4=({struct _dyneither_ptr*_tmp121=_cycalloc(sizeof(*_tmp121));({struct _dyneither_ptr _tmp7E3=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp120=({struct Cyc_Int_pa_PrintArg_struct _tmp775;_tmp775.tag=1U,_tmp775.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp775;});void*_tmp11E[1U];_tmp11E[0]=& _tmp120;({struct _dyneither_ptr _tmp7E2=({const char*_tmp11F="_tmp%X";_tag_dyneither(_tmp11F,sizeof(char),7U);});Cyc_aprintf(_tmp7E2,_tag_dyneither(_tmp11E,sizeof(void*),1U));});});*_tmp121=_tmp7E3;});_tmp121;});_tmp122->f2=_tmp7E4;});_tmp122;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 709
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp123=*env;struct _tuple30 _tmp124=_tmp123;struct Cyc_Xarray_Xarray*_tmp12C;_LL1: _tmp12C=(_tmp124.f1)->temp_labels;_LL2:;{
int _tmp125=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp7E5=_tmp125;_tmp7E5 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12C);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12C,_tmp125);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12B=_cycalloc(sizeof(*_tmp12B));({struct _dyneither_ptr _tmp7E7=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12A=({struct Cyc_Int_pa_PrintArg_struct _tmp776;_tmp776.tag=1U,_tmp776.f1=(unsigned int)_tmp125;_tmp776;});void*_tmp128[1U];_tmp128[0]=& _tmp12A;({struct _dyneither_ptr _tmp7E6=({const char*_tmp129="_LL%X";_tag_dyneither(_tmp129,sizeof(char),6U);});Cyc_aprintf(_tmp7E6,_tag_dyneither(_tmp128,sizeof(void*),1U));});});*_tmp12B=_tmp7E7;});_tmp12B;});
if(({int _tmp7E8=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12C,res);_tmp7E8 != _tmp125;}))
({void*_tmp126=0U;({struct _dyneither_ptr _tmp7E9=({const char*_tmp127="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp127,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7E9,_tag_dyneither(_tmp126,sizeof(void*),0U));});});
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
return({struct _tuple9*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->f1=_tmp133,_tmp130->f2=_tmp132,({void*_tmp7EA=Cyc_Toc_typ_to_c(_tmp131);_tmp130->f3=_tmp7EA;});_tmp130;});}
# 762 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp134=Cyc_Tcutil_compress(t);void*_tmp135=_tmp134;void*_tmp13B;void*_tmp13A;struct Cyc_Absyn_Tqual _tmp139;struct Cyc_Absyn_Exp*_tmp138;void*_tmp137;unsigned int _tmp136;switch(*((int*)_tmp135)){case 4U: _LL1: _tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).elt_type;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).tq;_tmp138=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).num_elts;_tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zero_term;_tmp136=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zt_loc;_LL2:
# 765
 return({void*_tmp7EB=Cyc_Toc_typ_to_c_array(_tmp13A);Cyc_Absyn_cstar_type(_tmp7EB,_tmp139);});case 1U: _LL3: _tmp13B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp135)->f2;if(_tmp13B != 0){_LL4:
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
({void**_tmp7ED=({void**_tmp13E=_cycalloc(sizeof(*_tmp13E));({void*_tmp7EC=Cyc_Absyn_star_type(Cyc_Absyn_char_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type);*_tmp13E=_tmp7EC;});_tmp13E;});cs=_tmp7ED;});
return*cs;}
# 791
static void*Cyc_Toc_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp7F0=({void**_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp7EF=({void*_tmp7EE=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp7EE,Cyc_Toc_mt_tq);});*_tmp13F=_tmp7EF;});_tmp13F;});r=_tmp7F0;});
return*r;}
# 797
static void*Cyc_Toc_dyn_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp7F3=({void**_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp7F2=({void*_tmp7F1=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_type(_tmp7F1,Cyc_Toc_mt_tq);});*_tmp140=_tmp7F2;});_tmp140;});r=_tmp7F3;});
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
({void*_tmp147=0U;({struct _dyneither_ptr _tmp7F4=({const char*_tmp148="unresolved DatatypeFieldType";_tag_dyneither(_tmp148,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F4,_tag_dyneither(_tmp147,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
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
({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp778;_tmp778.tag=0U,({
struct _dyneither_ptr _tmp7F5=(struct _dyneither_ptr)(_tmp159->kind == Cyc_Absyn_UnionA?({const char*_tmp15E="union";_tag_dyneither(_tmp15E,sizeof(char),6U);}):({const char*_tmp15F="struct";_tag_dyneither(_tmp15F,sizeof(char),7U);}));_tmp778.f1=_tmp7F5;});_tmp778;});struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp777;_tmp777.tag=0U,({
struct _dyneither_ptr _tmp7F6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp777.f1=_tmp7F6;});_tmp777;});void*_tmp15A[2U];_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D;({struct _dyneither_ptr _tmp7F7=({const char*_tmp15B="%s %s was never defined.";_tag_dyneither(_tmp15B,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp7F7,_tag_dyneither(_tmp15A,sizeof(void*),2U));});});}
# 906
if(_tmp159->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp160=_tmp159->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp159->impl))->fields;
if(_tmp160 == 0)return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp160))->tl != 0;_tmp160=_tmp160->tl){;}{
void*_tmp161=((struct Cyc_Absyn_Aggrfield*)_tmp160->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp161))){
if(_tmp159->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp779;_tmp779.tag=0U,({struct _dyneither_ptr _tmp7F8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp779.f1=_tmp7F8;});_tmp779;});void*_tmp162[1U];_tmp162[0]=& _tmp164;({struct _dyneither_ptr _tmp7F9=({const char*_tmp163="struct %s ended up being abstract.";_tag_dyneither(_tmp163,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp7F9,_tag_dyneither(_tmp162,sizeof(void*),1U));});});{
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
({void*_tmp7FA=Cyc_Toc_typ_to_c(_tmp176);_tmp176=_tmp7FA;});
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp174)== 0)
return Cyc_Toc_dyneither_ptr_type();else{
# 843
return Cyc_Absyn_star_type(_tmp176,Cyc_Absyn_heap_rgn_type,_tmp175,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).tq;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).num_elts;_tmp177=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).zt_loc;_LL14:
# 847
 return({void*_tmp7FE=Cyc_Toc_typ_to_c(_tmp17A);struct Cyc_Absyn_Tqual _tmp7FD=_tmp179;struct Cyc_Absyn_Exp*_tmp7FC=_tmp178;void*_tmp7FB=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp7FE,_tmp7FD,_tmp7FC,_tmp7FB,_tmp177);});case 5U: _LL15: _tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_tqual;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_type;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).args;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).c_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).cyc_varargs;_tmp17B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).attributes;_LL16: {
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
({struct Cyc_List_List*_tmp7FF=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=(void*)_tmp17B->hd,_tmp14C->tl=_tmp149;_tmp14C;});_tmp149=_tmp7FF;});goto _LL35;}_LL35:;}{
# 864
struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17E);
if(_tmp17C != 0){
# 867
void*_tmp14E=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp17C->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp14F=({struct _tuple9*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->f1=_tmp17C->name,_tmp151->f2=_tmp17C->tq,_tmp151->f3=_tmp14E;_tmp151;});
({struct Cyc_List_List*_tmp801=({struct Cyc_List_List*_tmp800=_tmp14D;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp800,({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->hd=_tmp14F,_tmp150->tl=0;_tmp150;}));});_tmp14D=_tmp801;});}
# 871
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->tag=5U,(_tmp152->f1).tvars=0,(_tmp152->f1).effect=0,(_tmp152->f1).ret_tqual=_tmp180,({void*_tmp802=Cyc_Toc_typ_to_c(_tmp17F);(_tmp152->f1).ret_type=_tmp802;}),(_tmp152->f1).args=_tmp14D,(_tmp152->f1).c_varargs=_tmp17D,(_tmp152->f1).cyc_varargs=0,(_tmp152->f1).rgn_po=0,(_tmp152->f1).attributes=_tmp149,(_tmp152->f1).requires_clause=0,(_tmp152->f1).requires_relns=0,(_tmp152->f1).ensures_clause=0,(_tmp152->f1).ensures_relns=0;_tmp152;});};}case 6U: _LL17: _tmp181=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp146)->f1;_LL18: {
# 876
struct Cyc_List_List*_tmp153=0;
for(0;_tmp181 != 0;_tmp181=_tmp181->tl){
({struct Cyc_List_List*_tmp805=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));({struct _tuple12*_tmp804=({struct _tuple12*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->f1=(*((struct _tuple12*)_tmp181->hd)).f1,({void*_tmp803=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp181->hd)).f2);_tmp154->f2=_tmp803;});_tmp154;});_tmp155->hd=_tmp804;}),_tmp155->tl=_tmp153;_tmp155;});_tmp153=_tmp805;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp153));}case 7U: _LL19: _tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f1;_tmp182=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f2;_LL1A: {
# 885
struct Cyc_List_List*_tmp156=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp808=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*_tmp157));({struct Cyc_Absyn_Aggrfield*_tmp807=({struct Cyc_Absyn_Aggrfield*_tmp806=(struct Cyc_Absyn_Aggrfield*)_tmp182->hd;Cyc_Toc_aggrfield_to_c(_tmp806,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp182->hd)->type));});_tmp157->hd=_tmp807;}),_tmp157->tl=_tmp156;_tmp157;});_tmp156=_tmp808;});}
return({enum Cyc_Absyn_AggrKind _tmp809=_tmp183;Cyc_Toc_add_anon_aggr_type(_tmp809,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp156));});}case 8U: _LL21: _tmp187=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f1;_tmp186=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f2;_tmp185=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f3;_tmp184=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f4;_LL22:
# 931
 if(_tmp184 == 0){
if(_tmp186 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=8U,_tmp16A->f1=_tmp187,_tmp16A->f2=0,_tmp16A->f3=_tmp185,_tmp16A->f4=0;_tmp16A;});else{
return t;}}else{
# 936
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=8U,_tmp16B->f1=_tmp187,_tmp16B->f2=0,_tmp16B->f3=_tmp185,({void*_tmp80A=Cyc_Toc_typ_to_c(_tmp184);_tmp16B->f4=_tmp80A;});_tmp16B;});}case 9U: _LL2B: _tmp188=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp146)->f1;_LL2C:
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
 return({void*_tmp80B=Cyc_Absyn_star_type(_tmp190,Cyc_Absyn_heap_rgn_type,_tmp18F,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp80B,e);});}else{_LL3: _LL4:
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
({struct Cyc_String_pa_PrintArg_struct _tmp19A=({struct Cyc_String_pa_PrintArg_struct _tmp77A;_tmp77A.tag=0U,({struct _dyneither_ptr _tmp80C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp77A.f1=_tmp80C;});_tmp77A;});void*_tmp198[1U];_tmp198[0]=& _tmp19A;({struct _dyneither_ptr _tmp80D=({const char*_tmp199="atomic_typ:  bad type %s";_tag_dyneither(_tmp199,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp80D,_tag_dyneither(_tmp198,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
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
({struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp77B;_tmp77B.tag=0U,({struct _dyneither_ptr _tmp80E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp77B.f1=_tmp80E;});_tmp77B;});void*_tmp19E[1U];_tmp19E[0]=& _tmp1A0;({struct _dyneither_ptr _tmp80F=({const char*_tmp19F="atomic_typ:  bad type %s";_tag_dyneither(_tmp19F,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp80F,_tag_dyneither(_tmp19E,sizeof(void*),1U));});});}_LL0:;}
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
({void*_tmp1AC=0U;({struct _dyneither_ptr _tmp810=({const char*_tmp1AD="is_poly_field: type missing fields";_tag_dyneither(_tmp1AD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp810,_tag_dyneither(_tmp1AC,sizeof(void*),0U));});});
_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AB->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AA)->f2;_LL4: {
# 1053
struct Cyc_Absyn_Aggrfield*_tmp1AE=Cyc_Absyn_lookup_field(_tmp1B6,f);
if(_tmp1AE == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp77C;_tmp77C.tag=0U,_tmp77C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp77C;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _dyneither_ptr _tmp811=({const char*_tmp1B0="is_poly_field: bad field %s";_tag_dyneither(_tmp1B0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp811,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AE->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp77D;_tmp77D.tag=0U,({struct _dyneither_ptr _tmp812=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp77D.f1=_tmp812;});_tmp77D;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp813=({const char*_tmp1B3="is_poly_field: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp813,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL0:;}
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
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp77E;_tmp77E.tag=0U,({struct _dyneither_ptr _tmp814=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C2->topt)));_tmp77E.f1=_tmp814;});_tmp77E;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct _dyneither_ptr _tmp815=({const char*_tmp1BC="is_poly_project: bad type %s";_tag_dyneither(_tmp1BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp815,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1075
 return 0;}_LL0:;}
# 1080
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp816=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp816,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=s,_tmp1C3->tl=0;_tmp1C3;}),0U);});}
# 1084
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp817=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp817,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1088
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_type(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1094
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp818=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp818,({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1098
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp819=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp819,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_type(elt_type))
return({struct Cyc_Absyn_Exp*_tmp81A=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp81A,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=s,_tmp1C7[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1106
return({struct Cyc_Absyn_Exp*_tmp81B=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp81B,({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=s,_tmp1C8[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1109
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp81C=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp81C,({struct Cyc_Absyn_Exp*_tmp1C9[3U];_tmp1C9[0]=rgn,_tmp1C9[1]=s,_tmp1C9[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1113
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp81D=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp81D,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=e,_tmp1CA->tl=0;_tmp1CA;}),0U);});}
# 1116
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp81E=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp81E,({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=e,_tmp1CB->tl=0;_tmp1CB;}),0U);});}
# 1120
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp81F=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp81F,({void*_tmp1CC=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CC,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
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
({struct Cyc_List_List*_tmp821=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct Cyc_Absyn_Decl*_tmp820=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->tag=0U,_tmp1CF->f1=vd;_tmp1CF;}),0U);_tmp1D0->hd=_tmp820;}),_tmp1D0->tl=Cyc_Toc_result_decls;_tmp1D0;});Cyc_Toc_result_decls=_tmp821;});
({struct Cyc_Absyn_Exp*_tmp822=Cyc_Absyn_var_exp(x,0U);xexp=_tmp822;});
({struct Cyc_Absyn_Exp*_tmp823=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp823;});}else{
# 1149
({struct Cyc_Absyn_Exp*_tmp825=({void*_tmp824=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp824,e);});xexp=_tmp825;});
# 1151
({struct Cyc_Absyn_Exp*_tmp827=({void*_tmp826=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp826,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp827;});}
# 1153
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1D1[3U];({struct _tuple20*_tmp82A=({struct _tuple20*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xexp;_tmp1D2;});_tmp1D1[0]=_tmp82A;}),({
struct _tuple20*_tmp829=({struct _tuple20*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xexp;_tmp1D3;});_tmp1D1[1]=_tmp829;}),({
struct _tuple20*_tmp828=({struct _tuple20*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->f1=0,_tmp1D4->f2=xplussz;_tmp1D4;});_tmp1D1[2]=_tmp828;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D1,sizeof(struct _tuple20*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
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
int*_tmp82C=({int*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));*_tmp1D9=0;_tmp1D9;});_tmp1DB->in_lhs=_tmp82C;}),({
int*_tmp82B=({int*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));*_tmp1DA=0;_tmp1DA;});_tmp1DB->in_sizeof=_tmp82B;}),_tmp1DB->rgn=r;_tmp1DB;});}
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
return({struct Cyc_Toc_Env*_tmp205=_region_malloc(r,sizeof(*_tmp205));({struct _dyneither_ptr**_tmp82D=({struct _dyneither_ptr**_tmp204=_region_malloc(r,sizeof(*_tmp204));*_tmp204=break_l;_tmp204;});_tmp205->break_lab=_tmp82D;}),_tmp205->continue_lab=_tmp20B,_tmp205->fallthru_info=fi,_tmp205->toplevel=_tmp209,_tmp205->in_lhs=_tmp208,_tmp205->in_sizeof=_tmp207,_tmp205->rgn=r;_tmp205;});};}
# 1256
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1258
struct Cyc_Toc_Env*_tmp20D=e;struct _dyneither_ptr**_tmp215;struct _dyneither_ptr**_tmp214;struct Cyc_Toc_FallthruInfo*_tmp213;int _tmp212;int*_tmp211;int*_tmp210;_LL1: _tmp215=_tmp20D->break_lab;_tmp214=_tmp20D->continue_lab;_tmp213=_tmp20D->fallthru_info;_tmp212=_tmp20D->toplevel;_tmp211=_tmp20D->in_lhs;_tmp210=_tmp20D->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp20F=_region_malloc(r,sizeof(*_tmp20F));({struct _dyneither_ptr**_tmp82E=({struct _dyneither_ptr**_tmp20E=_region_malloc(r,sizeof(*_tmp20E));*_tmp20E=break_l;_tmp20E;});_tmp20F->break_lab=_tmp82E;}),_tmp20F->continue_lab=_tmp214,_tmp20F->fallthru_info=0,_tmp20F->toplevel=_tmp212,_tmp20F->in_lhs=_tmp211,_tmp20F->in_sizeof=_tmp210,_tmp20F->rgn=r;_tmp20F;});}
# 1265
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1268
struct Cyc_Toc_Env*_tmp216=e;struct _dyneither_ptr**_tmp21E;struct _dyneither_ptr**_tmp21D;struct Cyc_Toc_FallthruInfo*_tmp21C;int _tmp21B;int*_tmp21A;int*_tmp219;_LL1: _tmp21E=_tmp216->break_lab;_tmp21D=_tmp216->continue_lab;_tmp21C=_tmp216->fallthru_info;_tmp21B=_tmp216->toplevel;_tmp21A=_tmp216->in_lhs;_tmp219=_tmp216->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->break_lab=0,_tmp218->continue_lab=_tmp21D,({struct Cyc_Toc_FallthruInfo*_tmp82F=({struct Cyc_Toc_FallthruInfo*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->label=next_l,_tmp217->binders=0;_tmp217;});_tmp218->fallthru_info=_tmp82F;}),_tmp218->toplevel=_tmp21B,_tmp218->in_lhs=_tmp21A,_tmp218->in_sizeof=_tmp219,_tmp218->rgn=r;_tmp218;});}
# 1281 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1284
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp21F=e->annot;void*_tmp220=_tmp21F;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_Absyn_EmptyAnnot){_LLD: _LLE:
 return 1;}else{_LLF: _LL10:
({void*_tmp221=0U;({unsigned int _tmp833=e->loc;struct _dyneither_ptr _tmp832=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp224=({struct Cyc_String_pa_PrintArg_struct _tmp77F;_tmp77F.tag=0U,({struct _dyneither_ptr _tmp830=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp77F.f1=_tmp830;});_tmp77F;});void*_tmp222[1U];_tmp222[0]=& _tmp224;({struct _dyneither_ptr _tmp831=({const char*_tmp223="Toc: do_null_check %s";_tag_dyneither(_tmp223,sizeof(char),22U);});Cyc_aprintf(_tmp831,_tag_dyneither(_tmp222,sizeof(void*),1U));});});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp833,_tmp832,_tag_dyneither(_tmp221,sizeof(void*),0U));});});}}}}}}}_LL0:;}
# 1296
static int Cyc_Toc_do_bounds_check(struct Cyc_Absyn_Exp*e){
void*_tmp225=e->annot;void*_tmp226=_tmp225;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp226)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp226)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 return 0;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp226)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp226)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 goto _LLA;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp226)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp226)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 1;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp226)->tag == Cyc_Absyn_EmptyAnnot){_LLD: _LLE:
 return 1;}else{_LLF: _LL10:
({void*_tmp227=0U;({unsigned int _tmp835=e->loc;struct _dyneither_ptr _tmp834=({const char*_tmp228="Toc: do_bounds_check";_tag_dyneither(_tmp228,sizeof(char),21U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp835,_tmp834,_tag_dyneither(_tmp227,sizeof(void*),0U));});});}}}}}}}_LL0:;}
# 1309
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp229=0U;({struct _dyneither_ptr _tmp836=({const char*_tmp22A="Missing type in primop ";_tag_dyneither(_tmp22A,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp836,_tag_dyneither(_tmp229,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1313
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp22B=_cycalloc(sizeof(*_tmp22B));_tmp22B->f1=Cyc_Toc_mt_tq,({void*_tmp837=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp22B->f2=_tmp837;});_tmp22B;});}
# 1318
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp22C=e->r;void*_tmp22D=_tmp22C;struct Cyc_Absyn_Exp*_tmp235;int _tmp234;struct Cyc_Absyn_Exp*_tmp233;int _tmp232;struct Cyc_List_List*_tmp231;switch(*((int*)_tmp22D)){case 26U: _LL1: _tmp231=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp22D)->f1;_LL2:
# 1321
{struct Cyc_List_List*_tmp22E=_tmp231;for(0;_tmp22E != 0;_tmp22E=_tmp22E->tl){
if((*((struct _tuple20*)_tmp22E->hd)).f1 != 0)
({void*_tmp22F=0U;({struct _dyneither_ptr _tmp838=({const char*_tmp230="array designators for abstract-field initialization";_tag_dyneither(_tmp230,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp838,_tag_dyneither(_tmp22F,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp839=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp231),0U);_tmp233=_tmp839;});_tmp232=0;goto _LL4;case 27U: _LL3: _tmp233=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp22D)->f2;_tmp232=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp22D)->f4;_LL4:
 _tmp235=_tmp233;_tmp234=_tmp232;goto _LL6;case 28U: _LL5: _tmp235=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp22D)->f1;_tmp234=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp22D)->f3;_LL6:
# 1327
({struct Cyc_Absyn_Exp*_tmp83A=Cyc_Absyn_copy_exp(_tmp235);_tmp235=_tmp83A;});
return _tmp234?({struct Cyc_Absyn_Exp*_tmp83B=_tmp235;Cyc_Absyn_add_exp(_tmp83B,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp235;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1336
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1344
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp236=e->r;void*_tmp237=_tmp236;struct Cyc_List_List*_tmp238;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp237)->tag == 29U){_LL1: _tmp238=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp237)->f3;_LL2:
 dles=_tmp238;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1350
{void*_tmp239=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp23A=_tmp239;struct Cyc_Absyn_Aggrdecl*_tmp23B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23A)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23A)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp23B=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23A)->f1)->f1).KnownAggr).val;_LL7:
# 1352
 if(_tmp23B->impl == 0)
return 0;
if(_tmp23B->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23B->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1360
 return 0;}_LL5:;}
# 1362
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp23C=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp23D=(struct _tuple20*)dles->hd;struct _tuple20*_tmp23E=_tmp23D;struct Cyc_List_List*_tmp247;struct Cyc_Absyn_Exp*_tmp246;_LLB: _tmp247=_tmp23E->f1;_tmp246=_tmp23E->f2;_LLC:;{
struct _dyneither_ptr*_tmp23F=Cyc_Absyn_designatorlist_to_fieldname(_tmp247);
if(!Cyc_strptrcmp(_tmp23F,_tmp23C->name)){
struct Cyc_Absyn_Exp*_tmp240=Cyc_Toc_get_varsizeexp(_tmp246);
if(_tmp240 != 0)
return _tmp240;{
void*_tmp241=Cyc_Tcutil_compress(_tmp23C->type);void*_tmp242=_tmp241;void*_tmp245;struct Cyc_Absyn_Exp*_tmp244;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp242)->tag == 4U){_LLE: _tmp245=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp242)->f1).elt_type;_tmp244=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp242)->f1).num_elts;_LLF:
# 1377
 if(_tmp244 == 0  || !Cyc_Toc_is_zero(_tmp244))
return 0;
# 1383
return({struct Cyc_Absyn_Exp*_tmp83E=({struct Cyc_Absyn_Exp*_tmp83D=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp83D,({struct Cyc_Absyn_Exp*_tmp243[2U];_tmp243[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp246)),({
# 1385
struct Cyc_Absyn_Exp*_tmp83C=Cyc_Absyn_sizeoftype_exp(_tmp245,0U);_tmp243[1]=_tmp83C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp243,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1383
Cyc_Absyn_add_exp(_tmp83E,
# 1386
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1391
({void*_tmp248=0U;({struct _dyneither_ptr _tmp83F=({const char*_tmp249="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp249,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp83F,_tag_dyneither(_tmp248,sizeof(void*),0U));});});};}
# 1394
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp24A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp24A != 0;_tmp24A=_tmp24A->tl){
struct Cyc_Absyn_Aggrfield*_tmp24B=(struct Cyc_Absyn_Aggrfield*)_tmp24A->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp24B->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1401
({void*_tmp24C=0U;({struct _dyneither_ptr _tmp841=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp24F=({struct Cyc_String_pa_PrintArg_struct _tmp780;_tmp780.tag=0U,_tmp780.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp780;});void*_tmp24D[1U];_tmp24D[0]=& _tmp24F;({struct _dyneither_ptr _tmp840=({const char*_tmp24E="get_member_offset %s failed";_tag_dyneither(_tmp24E,sizeof(char),28U);});Cyc_aprintf(_tmp840,_tag_dyneither(_tmp24D,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp841,_tag_dyneither(_tmp24C,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1405
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1408
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1411
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250->tag=5U,_tmp250->f1=e1,_tmp250->f2=incr;_tmp250;}),0U);}
# 1415
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1424
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1426
void*_tmp251=e1->r;void*_tmp252=_tmp251;struct Cyc_Absyn_Exp*_tmp25A;struct _dyneither_ptr*_tmp259;int _tmp258;int _tmp257;void*_tmp256;struct Cyc_Absyn_Exp*_tmp255;struct Cyc_Absyn_Stmt*_tmp254;switch(*((int*)_tmp252)){case 37U: _LL1: _tmp254=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp252)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp254,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp256=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp252)->f1;_tmp255=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp252)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp255,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp25A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp252)->f1;_tmp259=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp252)->f2;_tmp258=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp252)->f3;_tmp257=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp252)->f4;_LL6:
# 1431
 e1->r=_tmp25A->r;
({struct Cyc_Absyn_Exp*_tmp844=e1;struct Cyc_List_List*_tmp843=({struct Cyc_List_List*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->hd=_tmp259,_tmp253->tl=fs;_tmp253;});struct Cyc_Absyn_Exp*(*_tmp842)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp844,_tmp843,_tmp842,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1439
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1441
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp845=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp845;});}
({void*_tmp846=(f(e1_copy,f_env))->r;e1->r=_tmp846;});
goto _LL0;}}_LL0:;}
# 1447
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1449
void*_tmp25B=s->r;void*_tmp25C=_tmp25B;struct Cyc_Absyn_Stmt*_tmp263;struct Cyc_Absyn_Decl*_tmp262;struct Cyc_Absyn_Stmt*_tmp261;struct Cyc_Absyn_Exp*_tmp260;switch(*((int*)_tmp25C)){case 1U: _LL1: _tmp260=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp25C)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp260,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp262=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp25C)->f1;_tmp261=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp25C)->f2;_LL4:
# 1452
 Cyc_Toc_lvalue_assign_stmt(_tmp261,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp263=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp25C)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp263,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp25F=({struct Cyc_String_pa_PrintArg_struct _tmp781;_tmp781.tag=0U,({struct _dyneither_ptr _tmp847=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp781.f1=_tmp847;});_tmp781;});void*_tmp25D[1U];_tmp25D[0]=& _tmp25F;({struct _dyneither_ptr _tmp848=({const char*_tmp25E="lvalue_assign_stmt: %s";_tag_dyneither(_tmp25E,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp848,_tag_dyneither(_tmp25D,sizeof(void*),1U));});});}_LL0:;}
# 1458
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1462
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp264=e->r;void*_tmp265=_tmp264;struct Cyc_Absyn_Stmt*_tmp26C;struct Cyc_Absyn_Exp*_tmp26B;void**_tmp26A;struct Cyc_Absyn_Exp**_tmp269;switch(*((int*)_tmp265)){case 14U: _LL1: _tmp26A=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp265)->f1;_tmp269=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp265)->f2;_LL2:
# 1465
({struct Cyc_Absyn_Exp*_tmp849=Cyc_Toc_push_address_exp(*_tmp269);*_tmp269=_tmp849;});
({void*_tmp84A=Cyc_Absyn_cstar_type(*_tmp26A,Cyc_Toc_mt_tq);*_tmp26A=_tmp84A;});
return e;case 20U: _LL3: _tmp26B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp265)->f1;_LL4:
# 1469
 return _tmp26B;case 37U: _LL5: _tmp26C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp265)->f1;_LL6:
# 1473
 Cyc_Toc_push_address_stmt(_tmp26C);
return e;default: _LL7: _LL8:
# 1476
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp268=({struct Cyc_String_pa_PrintArg_struct _tmp782;_tmp782.tag=0U,({struct _dyneither_ptr _tmp84B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp782.f1=_tmp84B;});_tmp782;});void*_tmp266[1U];_tmp266[0]=& _tmp268;({struct _dyneither_ptr _tmp84C=({const char*_tmp267="can't take & of exp %s";_tag_dyneither(_tmp267,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp84C,_tag_dyneither(_tmp266,sizeof(void*),1U));});});}_LL0:;}
# 1481
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp26D=s->r;void*_tmp26E=_tmp26D;struct Cyc_Absyn_Exp**_tmp274;struct Cyc_Absyn_Stmt*_tmp273;struct Cyc_Absyn_Stmt*_tmp272;switch(*((int*)_tmp26E)){case 2U: _LL1: _tmp272=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp26E)->f2;_LL2:
 _tmp273=_tmp272;goto _LL4;case 12U: _LL3: _tmp273=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26E)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp273);goto _LL0;case 1U: _LL5: _tmp274=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp26E)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp84D=Cyc_Toc_push_address_exp(*_tmp274);*_tmp274=_tmp84D;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp271=({struct Cyc_String_pa_PrintArg_struct _tmp783;_tmp783.tag=0U,({struct _dyneither_ptr _tmp84E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp783.f1=_tmp84E;});_tmp783;});void*_tmp26F[1U];_tmp26F[0]=& _tmp271;({struct _dyneither_ptr _tmp84F=({const char*_tmp270="can't take & of stmt %s";_tag_dyneither(_tmp270,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp84F,_tag_dyneither(_tmp26F,sizeof(void*),1U));});});}_LL0:;}
# 1490
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp275=Cyc_Tcutil_compress(t);void*_tmp276=_tmp275;struct Cyc_Absyn_PtrInfo _tmp279;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp276)->tag == 3U){_LL1: _tmp279=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp276)->f1;_LL2:
 return _tmp279;}else{_LL3: _LL4:
({void*_tmp277=0U;({struct _dyneither_ptr _tmp850=({const char*_tmp278="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp278,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp850,_tag_dyneither(_tmp277,sizeof(void*),0U));});});}_LL0:;}
# 1500
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp27A=Cyc_Tcutil_compress(t);void*_tmp27B=_tmp27A;void*_tmp28C;switch(*((int*)_tmp27B)){case 3U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp851=Cyc_Absyn_null_exp(0U);res=_tmp851;});goto _LL0;case 0U: _LL3: _tmp28C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f1;_LL4:
# 1505
{void*_tmp27C=_tmp28C;int _tmp288;enum Cyc_Absyn_Sign _tmp287;enum Cyc_Absyn_Sign _tmp286;enum Cyc_Absyn_Sign _tmp285;enum Cyc_Absyn_Sign _tmp284;enum Cyc_Absyn_Sign _tmp283;switch(*((int*)_tmp27C)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27C)->f2){case Cyc_Absyn_Char_sz: _LL8: _tmp283=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27C)->f1;_LL9:
# 1507
({struct Cyc_Absyn_Exp*_tmp852=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp283,'\000'),0U);res=_tmp852;});goto _LL7;case Cyc_Absyn_Short_sz: _LLA: _tmp284=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27C)->f1;_LLB:
# 1509
({struct Cyc_Absyn_Exp*_tmp853=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp284,0),0U);res=_tmp853;});goto _LL7;case Cyc_Absyn_Int_sz: _LL10: _tmp285=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27C)->f1;_LL11:
# 1512
 _tmp286=_tmp285;goto _LL13;case Cyc_Absyn_Long_sz: _LL12: _tmp286=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27C)->f1;_LL13:
# 1514
({struct Cyc_Absyn_Exp*_tmp854=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp286,0),0U);res=_tmp854;});goto _LL7;default: _LL14: _tmp287=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27C)->f1;_LL15:
# 1516
({struct Cyc_Absyn_Exp*_tmp855=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp287,(long long)0),0U);res=_tmp855;});goto _LL7;}case 15U: _LLC: _LLD:
# 1510
 goto _LLF;case 16U: _LLE: _LLF:
 _tmp285=1U;goto _LL11;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27C)->f1){case 0U: _LL16: _LL17:
# 1517
({struct Cyc_Absyn_Exp*_tmp856=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp27D="0.0F";_tag_dyneither(_tmp27D,sizeof(char),5U);}),0),0U);res=_tmp856;});goto _LL7;case 1U: _LL18: _LL19:
({struct Cyc_Absyn_Exp*_tmp857=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp27E="0.0";_tag_dyneither(_tmp27E,sizeof(char),4U);}),1),0U);res=_tmp857;});goto _LL7;default: _LL1A: _tmp288=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27C)->f1;_LL1B:
({struct Cyc_Absyn_Exp*_tmp859=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp858=({const char*_tmp27F="0.0L";_tag_dyneither(_tmp27F,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp858,_tmp288);}),0U);res=_tmp859;});goto _LL7;}default: _LL1C: _LL1D:
# 1521
({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp784;_tmp784.tag=0U,({struct _dyneither_ptr _tmp85A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp784.f1=_tmp85A;});_tmp784;});void*_tmp280[1U];_tmp280[0]=& _tmp282;({struct _dyneither_ptr _tmp85B=({const char*_tmp281="found non-zero type %s in generate_zero";_tag_dyneither(_tmp281,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85B,_tag_dyneither(_tmp280,sizeof(void*),1U));});});}_LL7:;}
# 1523
goto _LL0;default: _LL5: _LL6:
# 1525
({struct Cyc_String_pa_PrintArg_struct _tmp28B=({struct Cyc_String_pa_PrintArg_struct _tmp785;_tmp785.tag=0U,({struct _dyneither_ptr _tmp85C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp785.f1=_tmp85C;});_tmp785;});void*_tmp289[1U];_tmp289[0]=& _tmp28B;({struct _dyneither_ptr _tmp85D=({const char*_tmp28A="found non-zero type %s in generate_zero";_tag_dyneither(_tmp28A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85D,_tag_dyneither(_tmp289,sizeof(void*),1U));});});}_LL0:;}
# 1527
res->topt=t;
return res;}
# 1534
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1547 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp28D=Cyc_Toc_typ_to_c(elt_type);
void*_tmp28E=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp28F=Cyc_Absyn_cstar_type(_tmp28D,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp290=({struct Cyc_Core_Opt*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1->v=_tmp28F;_tmp2C1;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp291=e1->r;void*_tmp292=_tmp291;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp296;switch(*((int*)_tmp292)){case 20U: _LL1: _tmp296=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp292)->f1;_LL2:
# 1555
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp85E=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp296,0,Cyc_Absyn_Other_coercion,0U);_tmp296=_tmp85E;});
_tmp296->topt=fat_ptr_type;}
# 1559
Cyc_Toc_exp_to_c(nv,_tmp296);xinit=_tmp296;goto _LL0;case 23U: _LL3: _tmp298=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp292)->f1;_tmp297=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp292)->f2;_LL4:
# 1561
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp85F=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp298,0,Cyc_Absyn_Other_coercion,0U);_tmp298=_tmp85F;});
_tmp298->topt=fat_ptr_type;}
# 1565
Cyc_Toc_exp_to_c(nv,_tmp298);Cyc_Toc_exp_to_c(nv,_tmp297);
({struct Cyc_Absyn_Exp*_tmp862=({struct Cyc_Absyn_Exp*_tmp861=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp861,({struct Cyc_Absyn_Exp*_tmp293[3U];_tmp293[0]=_tmp298,({
struct Cyc_Absyn_Exp*_tmp860=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp293[1]=_tmp860;}),_tmp293[2]=_tmp297;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp293,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1566
xinit=_tmp862;});
# 1568
goto _LL0;default: _LL5: _LL6:
({void*_tmp294=0U;({struct _dyneither_ptr _tmp863=({const char*_tmp295="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp295,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp863,_tag_dyneither(_tmp294,sizeof(void*),0U));});});}_LL0:;}{
# 1571
struct _tuple1*_tmp299=Cyc_Toc_temp_var();
struct _RegionHandle _tmp29A=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp29A;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp29B=({struct Cyc_Absyn_Vardecl*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->sc=Cyc_Absyn_Public,_tmp2C0->name=_tmp299,_tmp2C0->varloc=0U,_tmp2C0->tq=Cyc_Toc_mt_tq,_tmp2C0->type=_tmp28E,_tmp2C0->initializer=xinit,_tmp2C0->rgn=0,_tmp2C0->attributes=0,_tmp2C0->escapes=0;_tmp2C0;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29C=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->tag=4U,_tmp2BF->f1=_tmp29B;_tmp2BF;});
struct Cyc_Absyn_Exp*_tmp29D=Cyc_Absyn_varb_exp((void*)_tmp29C,0U);
_tmp29D->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp29E=Cyc_Absyn_deref_exp(_tmp29D,0U);
_tmp29E->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp29E);{
struct _tuple1*_tmp29F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2A0=({struct Cyc_Absyn_Vardecl*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->sc=Cyc_Absyn_Public,_tmp2BE->name=_tmp29F,_tmp2BE->varloc=0U,_tmp2BE->tq=Cyc_Toc_mt_tq,_tmp2BE->type=_tmp28D,_tmp2BE->initializer=_tmp29E,_tmp2BE->rgn=0,_tmp2BE->attributes=0,_tmp2BE->escapes=0;_tmp2BE;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A1=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD->tag=4U,_tmp2BD->f1=_tmp2A0;_tmp2BD;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2A2=Cyc_Absyn_varb_exp((void*)_tmp2A1,0U);
_tmp2A2->topt=_tmp29E->topt;
({struct Cyc_Absyn_Exp*_tmp864=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2A2,e2,0U);z_init=_tmp864;});
z_init->topt=_tmp2A2->topt;}
# 1590
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2A3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2A4=({struct Cyc_Absyn_Vardecl*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC->sc=Cyc_Absyn_Public,_tmp2BC->name=_tmp2A3,_tmp2BC->varloc=0U,_tmp2BC->tq=Cyc_Toc_mt_tq,_tmp2BC->type=_tmp28D,_tmp2BC->initializer=z_init,_tmp2BC->rgn=0,_tmp2BC->attributes=0,_tmp2BC->escapes=0;_tmp2BC;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A5=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->tag=4U,_tmp2BB->f1=_tmp2A4;_tmp2BB;});
# 1596
struct Cyc_Absyn_Exp*_tmp2A6=Cyc_Absyn_varb_exp((void*)_tmp2A1,0U);_tmp2A6->topt=_tmp29E->topt;{
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2A8=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2A6,_tmp2A7,0U);
_tmp2A8->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2A8);{
# 1602
struct Cyc_Absyn_Exp*_tmp2A9=Cyc_Absyn_varb_exp((void*)_tmp2A5,0U);_tmp2A9->topt=_tmp29E->topt;{
struct Cyc_Absyn_Exp*_tmp2AA=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2AB=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2A9,_tmp2AA,0U);
_tmp2AB->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2AB);{
# 1608
struct Cyc_List_List*_tmp2AC=({struct Cyc_Absyn_Exp*_tmp2BA[2U];({struct Cyc_Absyn_Exp*_tmp866=Cyc_Absyn_varb_exp((void*)_tmp29C,0U);_tmp2BA[0]=_tmp866;}),({
struct Cyc_Absyn_Exp*_tmp865=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2BA[1]=_tmp865;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2BA,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2AD=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp868=({struct Cyc_Absyn_Exp*_tmp867=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2AC,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp867,_tmp2AD,0U);});xsize=_tmp868;});{
# 1615
struct Cyc_Absyn_Exp*_tmp2AE=({struct Cyc_Absyn_Exp*_tmp869=xsize;Cyc_Absyn_and_exp(_tmp869,Cyc_Absyn_and_exp(_tmp2A8,_tmp2AB,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2AF=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2B0=({struct Cyc_Absyn_Exp*_tmp86A=Cyc_Absyn_varb_exp((void*)_tmp29C,0U);Cyc_Toc_member_exp(_tmp86A,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp86B=Cyc_Toc_cast_it(_tmp28F,_tmp2B0);_tmp2B0=_tmp86B;});{
struct Cyc_Absyn_Exp*_tmp2B1=Cyc_Absyn_deref_exp(_tmp2B0,0U);
struct Cyc_Absyn_Exp*_tmp2B2=({struct Cyc_Absyn_Exp*_tmp86C=_tmp2B1;Cyc_Absyn_assign_exp(_tmp86C,Cyc_Absyn_var_exp(_tmp2A3,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2B3=Cyc_Absyn_exp_stmt(_tmp2B2,0U);
({struct Cyc_Absyn_Stmt*_tmp870=({struct Cyc_Absyn_Stmt*_tmp86F=({struct Cyc_Absyn_Exp*_tmp86E=_tmp2AE;struct Cyc_Absyn_Stmt*_tmp86D=_tmp2AF;Cyc_Absyn_ifthenelse_stmt(_tmp86E,_tmp86D,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp86F,_tmp2B3,0U);});_tmp2B3=_tmp870;});
({struct Cyc_Absyn_Stmt*_tmp873=({struct Cyc_Absyn_Decl*_tmp872=({struct Cyc_Absyn_Decl*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));({void*_tmp871=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4->tag=0U,_tmp2B4->f1=_tmp2A4;_tmp2B4;});_tmp2B5->r=_tmp871;}),_tmp2B5->loc=0U;_tmp2B5;});Cyc_Absyn_decl_stmt(_tmp872,_tmp2B3,0U);});_tmp2B3=_tmp873;});
({struct Cyc_Absyn_Stmt*_tmp876=({struct Cyc_Absyn_Decl*_tmp875=({struct Cyc_Absyn_Decl*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));({void*_tmp874=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->tag=0U,_tmp2B6->f1=_tmp2A0;_tmp2B6;});_tmp2B7->r=_tmp874;}),_tmp2B7->loc=0U;_tmp2B7;});Cyc_Absyn_decl_stmt(_tmp875,_tmp2B3,0U);});_tmp2B3=_tmp876;});
({struct Cyc_Absyn_Stmt*_tmp879=({struct Cyc_Absyn_Decl*_tmp878=({struct Cyc_Absyn_Decl*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));({void*_tmp877=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8->tag=0U,_tmp2B8->f1=_tmp29B;_tmp2B8;});_tmp2B9->r=_tmp877;}),_tmp2B9->loc=0U;_tmp2B9;});Cyc_Absyn_decl_stmt(_tmp878,_tmp2B3,0U);});_tmp2B3=_tmp879;});
({void*_tmp87A=Cyc_Toc_stmt_exp_r(_tmp2B3);e->r=_tmp87A;});};};};};};};};};};}
# 1573
;_pop_region(rgn2);};}
# 1641 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1645
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2C2=Cyc_Tcutil_compress(aggrtype);void*_tmp2C3=_tmp2C2;union Cyc_Absyn_AggrInfo _tmp2C8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C3)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C3)->f1)->tag == 20U){_LL1: _tmp2C8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C3)->f1)->f1;_LL2:
# 1648
({struct Cyc_Absyn_Aggrdecl*_tmp87B=Cyc_Absyn_get_known_aggrdecl(_tmp2C8);ad=_tmp87B;});goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp2C4=0U;({struct _dyneither_ptr _tmp87E=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C7=({struct Cyc_String_pa_PrintArg_struct _tmp786;_tmp786.tag=0U,({
struct _dyneither_ptr _tmp87C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp786.f1=_tmp87C;});_tmp786;});void*_tmp2C5[1U];_tmp2C5[0]=& _tmp2C7;({struct _dyneither_ptr _tmp87D=({const char*_tmp2C6="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2C6,sizeof(char),51U);});Cyc_aprintf(_tmp87D,_tag_dyneither(_tmp2C5,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp87E,_tag_dyneither(_tmp2C4,sizeof(void*),0U));});});}_LL0:;}{
# 1652
struct _tuple1*_tmp2C9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2CA=Cyc_Absyn_var_exp(_tmp2C9,0U);
struct Cyc_Absyn_Exp*_tmp2CB=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2CC=Cyc_Absyn_aggrarrow_exp(_tmp2CA,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2CD=Cyc_Absyn_neq_exp(_tmp2CC,_tmp2CB,0U);
struct Cyc_Absyn_Exp*_tmp2CE=Cyc_Absyn_aggrarrow_exp(_tmp2CA,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2CF=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2CE,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2D0=({struct Cyc_Absyn_Exp*_tmp880=_tmp2CD;struct Cyc_Absyn_Stmt*_tmp87F=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp880,_tmp87F,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2D1=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2D2=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2D3=({struct _tuple1*_tmp883=_tmp2C9;void*_tmp882=_tmp2D1;struct Cyc_Absyn_Exp*_tmp881=_tmp2D2;Cyc_Absyn_declare_stmt(_tmp883,_tmp882,_tmp881,Cyc_Absyn_seq_stmt(_tmp2D0,_tmp2CF,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2D3);}else{
# 1666
struct Cyc_Absyn_Exp*_tmp2D4=({struct Cyc_Absyn_Exp*_tmp884=aggrproj(_tmp2CA,f,0U);Cyc_Toc_member_exp(_tmp884,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2D5=Cyc_Absyn_neq_exp(_tmp2D4,_tmp2CB,0U);
struct Cyc_Absyn_Exp*_tmp2D6=({struct Cyc_Absyn_Exp*_tmp885=aggrproj(_tmp2CA,f,0U);Cyc_Toc_member_exp(_tmp885,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2D7=Cyc_Absyn_exp_stmt(_tmp2D6,0U);
struct Cyc_Absyn_Stmt*_tmp2D8=({struct Cyc_Absyn_Exp*_tmp887=_tmp2D5;struct Cyc_Absyn_Stmt*_tmp886=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp887,_tmp886,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2D9=({struct _tuple1*_tmp88A=_tmp2C9;void*_tmp889=e1_c_type;struct Cyc_Absyn_Exp*_tmp888=e1;Cyc_Absyn_declare_stmt(_tmp88A,_tmp889,_tmp888,Cyc_Absyn_seq_stmt(_tmp2D8,_tmp2D7,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2D9);}};}
# 1676
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1679
void*_tmp2DA=Cyc_Tcutil_compress(t);void*_tmp2DB=_tmp2DA;union Cyc_Absyn_AggrInfo _tmp2E2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DB)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DB)->f1)->tag == 20U){_LL1: _tmp2E2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DB)->f1)->f1;_LL2: {
# 1681
struct Cyc_Absyn_Aggrdecl*_tmp2DC=Cyc_Absyn_get_known_aggrdecl(_tmp2E2);
({int _tmp88B=Cyc_Toc_get_member_offset(_tmp2DC,f);*f_tag=_tmp88B;});{
# 1684
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E0=({struct Cyc_String_pa_PrintArg_struct _tmp788;_tmp788.tag=0U,_tmp788.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2DC->name).f2);_tmp788;});struct Cyc_String_pa_PrintArg_struct _tmp2E1=({struct Cyc_String_pa_PrintArg_struct _tmp787;_tmp787.tag=0U,_tmp787.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp787;});void*_tmp2DE[2U];_tmp2DE[0]=& _tmp2E0,_tmp2DE[1]=& _tmp2E1;({struct _dyneither_ptr _tmp88C=({const char*_tmp2DF="_union_%s_%s";_tag_dyneither(_tmp2DF,sizeof(char),13U);});Cyc_aprintf(_tmp88C,_tag_dyneither(_tmp2DE,sizeof(void*),2U));});});
({void*_tmp88D=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));*_tmp2DD=str;_tmp2DD;}));*tagged_member_type=_tmp88D;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DC->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1695
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1698
void*_tmp2E3=e->r;void*_tmp2E4=_tmp2E3;struct Cyc_Absyn_Exp*_tmp2F0;struct _dyneither_ptr*_tmp2EF;int*_tmp2EE;struct Cyc_Absyn_Exp*_tmp2ED;struct _dyneither_ptr*_tmp2EC;int*_tmp2EB;struct Cyc_Absyn_Exp*_tmp2EA;switch(*((int*)_tmp2E4)){case 14U: _LL1: _tmp2EA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E4)->f2;_LL2:
({void*_tmp2E5=0U;({struct _dyneither_ptr _tmp88E=({const char*_tmp2E6="cast on lhs!";_tag_dyneither(_tmp2E6,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp88E,_tag_dyneither(_tmp2E5,sizeof(void*),0U));});});case 21U: _LL3: _tmp2ED=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E4)->f1;_tmp2EC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E4)->f2;_tmp2EB=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E4)->f4;_LL4:
# 1701
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp2ED->topt),_tmp2EC,f_tag,tagged_member_type,clear_read,_tmp2EB);case 22U: _LL5: _tmp2F0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E4)->f1;_tmp2EF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E4)->f2;_tmp2EE=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E4)->f4;_LL6: {
# 1704
void*_tmp2E7=Cyc_Tcutil_compress((void*)_check_null(_tmp2F0->topt));void*_tmp2E8=_tmp2E7;void*_tmp2E9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->tag == 3U){_LLA: _tmp2E9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->f1).elt_type;_LLB:
# 1706
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2E9,_tmp2EF,f_tag,tagged_member_type,clear_read,_tmp2EE);}else{_LLC: _LLD:
# 1708
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1710
 return 0;}_LL0:;}
# 1723 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1727
struct _tuple1*_tmp2F1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp2F1,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp88F=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp88F;});else{
# 1737
struct Cyc_Absyn_Exp*_tmp2F2=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp892=({struct Cyc_Absyn_Exp*_tmp891=_tmp2F2;struct Cyc_Absyn_Stmt*_tmp890=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp891,_tmp890,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp892;});}{
# 1740
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp895=_tmp2F1;void*_tmp894=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp893=
Cyc_Toc_push_address_exp(e1);
# 1740
Cyc_Absyn_declare_stmt(_tmp895,_tmp894,_tmp893,
# 1742
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple32{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple35{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1746
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp2F5=({struct Cyc_String_pa_PrintArg_struct _tmp789;_tmp789.tag=0U,({struct _dyneither_ptr _tmp896=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp789.f1=_tmp896;});_tmp789;});void*_tmp2F3[1U];_tmp2F3[0]=& _tmp2F5;({struct _dyneither_ptr _tmp897=({const char*_tmp2F4="exp_to_c: no type for %s";_tag_dyneither(_tmp2F4,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp897,_tag_dyneither(_tmp2F3,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp2F6=e->r;void*_tmp2F7=_tmp2F6;struct Cyc_Absyn_Stmt*_tmp50F;struct Cyc_Absyn_Exp*_tmp50E;struct _dyneither_ptr*_tmp50D;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*_tmp50B;int _tmp50A;struct Cyc_Absyn_Exp*_tmp509;void**_tmp508;struct Cyc_Absyn_Exp*_tmp507;int _tmp506;int _tmp505;struct Cyc_List_List*_tmp504;struct Cyc_Absyn_Datatypedecl*_tmp503;struct Cyc_Absyn_Datatypefield*_tmp502;struct _tuple1**_tmp501;struct Cyc_List_List*_tmp500;struct Cyc_List_List*_tmp4FF;struct Cyc_Absyn_Aggrdecl*_tmp4FE;void*_tmp4FD;struct Cyc_List_List*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FB;void*_tmp4FA;int _tmp4F9;struct Cyc_Absyn_Vardecl*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F6;int _tmp4F5;struct Cyc_List_List*_tmp4F4;struct Cyc_List_List*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F0;struct _dyneither_ptr*_tmp4EF;int _tmp4EE;int _tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;struct _dyneither_ptr*_tmp4EB;int _tmp4EA;int _tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;void*_tmp4E7;struct Cyc_List_List*_tmp4E6;void*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E1;void**_tmp4E0;struct Cyc_Absyn_Exp*_tmp4DF;int _tmp4DE;enum Cyc_Absyn_Coercion _tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_List_List*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;int _tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_List_List*_tmp4D6;int _tmp4D5;struct Cyc_List_List*_tmp4D4;struct Cyc_Absyn_VarargInfo*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D2;struct Cyc_List_List*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Core_Opt*_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C4;enum Cyc_Absyn_Incrementor _tmp4C3;enum Cyc_Absyn_Primop _tmp4C2;struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C0;switch(*((int*)_tmp2F7)){case 2U: _LL1: _LL2:
# 1752
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1759
struct Cyc_Absyn_Exp*_tmp2F8=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp898=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp2F8,_tmp2F8))->r;e->r=_tmp898;});else{
# 1764
({void*_tmp89A=({struct Cyc_Absyn_Exp*_tmp899=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp899,({struct Cyc_Absyn_Exp*_tmp2F9[3U];_tmp2F9[0]=_tmp2F8,_tmp2F9[1]=_tmp2F8,_tmp2F9[2]=_tmp2F8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2F9,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp89A;});}}else{
# 1766
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1768
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 41U: _LL9: _tmp4C0=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LLA:
 Cyc_Toc_exp_to_c(nv,_tmp4C0);goto _LL0;case 3U: _LLB: _tmp4C2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4C1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LLC: {
# 1774
struct Cyc_List_List*_tmp2FA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp4C1);
# 1776
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4C1);
{enum Cyc_Absyn_Primop _tmp2FB=_tmp4C2;switch(_tmp2FB){case Cyc_Absyn_Numelts: _LL5A: _LL5B: {
# 1779
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1))->hd;
{void*_tmp2FC=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp2FD=_tmp2FC;void*_tmp30C;void*_tmp30B;void*_tmp30A;void*_tmp309;struct Cyc_Absyn_Exp*_tmp308;switch(*((int*)_tmp2FD)){case 4U: _LL6F: _tmp308=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2FD)->f1).num_elts;_LL70:
# 1784
 e->r=((struct Cyc_Absyn_Exp*)_check_null(_tmp308))->r;
goto _LL6E;case 3U: _LL71: _tmp30C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_type;_tmp30B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).nullable;_tmp30A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).bounds;_tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).zero_term;_LL72: {
# 1787
struct Cyc_Absyn_Exp*_tmp2FE=({void*_tmp89B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp89B,_tmp30A);});
if(_tmp2FE == 0)
# 1790
({void*_tmp89E=({struct Cyc_Absyn_Exp*_tmp89D=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp89D,({struct Cyc_Absyn_Exp*_tmp2FF[2U];_tmp2FF[0]=(struct Cyc_Absyn_Exp*)_tmp4C1->hd,({
# 1792
struct Cyc_Absyn_Exp*_tmp89C=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp30C),0U);_tmp2FF[1]=_tmp89C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2FF,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1790
e->r=_tmp89E;});else{
# 1794
struct Cyc_Absyn_Exp*_tmp300=_tmp2FE;
# 1796
if(Cyc_Tcutil_force_type2bool(0,_tmp309)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4C1->hd);
# 1800
({void*_tmp8A0=({struct Cyc_Absyn_Exp*_tmp89F=function_e;Cyc_Toc_fncall_exp_r(_tmp89F,({struct Cyc_Absyn_Exp*_tmp301[2U];_tmp301[0]=(struct Cyc_Absyn_Exp*)_tmp4C1->hd,_tmp301[1]=_tmp300;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp301,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8A0;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp30B)){
if(!Cyc_Evexp_c_can_eval(_tmp300))
({void*_tmp302=0U;({unsigned int _tmp8A2=e->loc;struct _dyneither_ptr _tmp8A1=({const char*_tmp303="can't calculate numelts";_tag_dyneither(_tmp303,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8A2,_tmp8A1,_tag_dyneither(_tmp302,sizeof(void*),0U));});});
# 1805
({void*_tmp8A5=({struct Cyc_Absyn_Exp*_tmp8A4=arg;struct Cyc_Absyn_Exp*_tmp8A3=_tmp300;Cyc_Toc_conditional_exp_r(_tmp8A4,_tmp8A3,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8A5;});}else{
# 1807
e->r=_tmp300->r;goto _LL6E;}}}
# 1810
goto _LL6E;}default: _LL73: _LL74:
# 1812
({struct Cyc_String_pa_PrintArg_struct _tmp306=({struct Cyc_String_pa_PrintArg_struct _tmp78B;_tmp78B.tag=0U,({
struct _dyneither_ptr _tmp8A6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp78B.f1=_tmp8A6;});_tmp78B;});struct Cyc_String_pa_PrintArg_struct _tmp307=({struct Cyc_String_pa_PrintArg_struct _tmp78A;_tmp78A.tag=0U,({struct _dyneither_ptr _tmp8A7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp78A.f1=_tmp8A7;});_tmp78A;});void*_tmp304[2U];_tmp304[0]=& _tmp306,_tmp304[1]=& _tmp307;({struct _dyneither_ptr _tmp8A8=({const char*_tmp305="numelts primop applied to non-array %s (%s)";_tag_dyneither(_tmp305,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8A8,_tag_dyneither(_tmp304,sizeof(void*),2U));});});}_LL6E:;}
# 1815
goto _LL59;}case Cyc_Absyn_Plus: _LL5C: _LL5D:
# 1820
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp30D=0U;({struct _dyneither_ptr _tmp8A9=({const char*_tmp30E="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp30E,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp8A9,_tag_dyneither(_tmp30D,sizeof(void*),0U));});});
{void*_tmp30F=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp2FA))->hd);void*_tmp310=_tmp30F;void*_tmp317;void*_tmp316;void*_tmp315;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->tag == 3U){_LL76: _tmp317=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).elt_type;_tmp316=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).ptr_atts).bounds;_tmp315=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).ptr_atts).zero_term;_LL77: {
# 1824
struct Cyc_Absyn_Exp*_tmp311=({void*_tmp8AA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8AA,_tmp316);});
if(_tmp311 == 0){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1->tl))->hd;
({void*_tmp8AD=({struct Cyc_Absyn_Exp*_tmp8AC=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8AC,({struct Cyc_Absyn_Exp*_tmp312[3U];_tmp312[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp317),0U);_tmp312[1]=_tmp8AB;}),_tmp312[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp312,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1828
e->r=_tmp8AD;});}else{
# 1831
struct Cyc_Absyn_Exp*_tmp313=_tmp311;
if(Cyc_Tcutil_force_type2bool(0,_tmp315)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1->tl))->hd;
({void*_tmp8AF=({struct Cyc_Absyn_Exp*_tmp8AE=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp8AE,({struct Cyc_Absyn_Exp*_tmp314[3U];_tmp314[0]=e1,_tmp314[1]=_tmp313,_tmp314[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp314,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp8AF;});}}
# 1838
goto _LL75;}}else{_LL78: _LL79:
 goto _LL75;}_LL75:;}
# 1841
goto _LL59;case Cyc_Absyn_Minus: _LL5E: _LL5F: {
# 1846
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp2FA))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp2FA->tl))->hd)){
({void*_tmp8B1=({struct Cyc_Absyn_Exp*_tmp8B0=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8B0,Cyc_Toc_curr_sp);});e1->r=_tmp8B1;});
({void*_tmp8B3=({struct Cyc_Absyn_Exp*_tmp8B2=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8B2,Cyc_Toc_curr_sp);});e2->r=_tmp8B3;});
({void*_tmp8B5=({void*_tmp8B4=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp8B4;});e1->topt=_tmp8B5;});
({void*_tmp8B7=({struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8B6,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1854
e->r=_tmp8B7;});}else{
# 1857
({void*_tmp8BB=({struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8BA,({struct Cyc_Absyn_Exp*_tmp318[3U];_tmp318[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8B9=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp318[1]=_tmp8B9;}),({
struct Cyc_Absyn_Exp*_tmp8B8=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp318[2]=_tmp8B8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp318,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1857
e->r=_tmp8BB;});}}
# 1862
goto _LL59;}case Cyc_Absyn_Eq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Neq: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gt: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Gte: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lt: _LL68: _LL69:
 goto _LL6B;case Cyc_Absyn_Lte: _LL6A: _LL6B: {
# 1870
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp2FA))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp2FA->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& elt_type)){
void*_tmp319=({void*_tmp8BC=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp8BC,Cyc_Toc_mt_tq);});
({void*_tmp8BF=({void*_tmp8BE=_tmp319;Cyc_Toc_cast_it_r(_tmp8BE,({struct Cyc_Absyn_Exp*_tmp8BD=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8BD,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8BF;});
e1->topt=_tmp319;}
# 1880
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
void*_tmp31A=({void*_tmp8C0=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp8C0,Cyc_Toc_mt_tq);});
({void*_tmp8C3=({void*_tmp8C2=_tmp31A;Cyc_Toc_cast_it_r(_tmp8C2,({struct Cyc_Absyn_Exp*_tmp8C1=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8C1,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8C3;});
e2->topt=_tmp31A;}
# 1885
goto _LL59;}default: _LL6C: _LL6D:
 goto _LL59;}_LL59:;}
# 1888
goto _LL0;}case 5U: _LLD: _tmp4C4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4C3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LLE: {
# 1890
void*e2_cyc_typ=(void*)_check_null(_tmp4C4->topt);
# 1899 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _dyneither_ptr incr_str=({const char*_tmp32E="increment";_tag_dyneither(_tmp32E,sizeof(char),10U);});
if(_tmp4C3 == Cyc_Absyn_PreDec  || _tmp4C3 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp8C4=({const char*_tmp31B="decrement";_tag_dyneither(_tmp31B,sizeof(char),10U);});incr_str=_tmp8C4;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4C4,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp78C;_tmp78C.tag=0U,_tmp78C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp78C;});void*_tmp31C[1U];_tmp31C[0]=& _tmp31E;({unsigned int _tmp8C6=e->loc;struct _dyneither_ptr _tmp8C5=({const char*_tmp31D="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp31D,sizeof(char),74U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp8C6,_tmp8C5,_tag_dyneither(_tmp31C,sizeof(void*),1U));});});
# 1907
if(Cyc_Toc_is_tagged_union_project(_tmp4C4,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp31F=_tmp4C3;switch(_tmp31F){case Cyc_Absyn_PreInc: _LL7B: _LL7C:
 op=0U;goto _LL7A;case Cyc_Absyn_PreDec: _LL7D: _LL7E:
 op=2U;goto _LL7A;default: _LL7F: _LL80:
# 1915
({struct Cyc_String_pa_PrintArg_struct _tmp322=({struct Cyc_String_pa_PrintArg_struct _tmp78D;_tmp78D.tag=0U,_tmp78D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp78D;});void*_tmp320[1U];_tmp320[0]=& _tmp322;({unsigned int _tmp8C8=e->loc;struct _dyneither_ptr _tmp8C7=({const char*_tmp321="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp321,sizeof(char),58U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp8C8,_tmp8C7,_tag_dyneither(_tmp320,sizeof(void*),1U));});});}_LL7A:;}
# 1918
({void*_tmp8CA=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324->tag=4U,_tmp324->f1=_tmp4C4,({struct Cyc_Core_Opt*_tmp8C9=({struct Cyc_Core_Opt*_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323->v=(void*)op;_tmp323;});_tmp324->f2=_tmp8C9;}),_tmp324->f3=one;_tmp324;});e->r=_tmp8CA;});
Cyc_Toc_exp_to_c(nv,e);
return;}{
# 1922
int _tmp325=Cyc_Tcutil_is_nullable_pointer_type(old_typ) && Cyc_Toc_do_null_check(e);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4C4);
Cyc_Toc_set_lhs(nv,0);{
# 1928
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4C3 == Cyc_Absyn_PostInc  || _tmp4C3 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 1934
if(_tmp4C3 == Cyc_Absyn_PreDec  || _tmp4C3 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp8CF=({struct Cyc_Absyn_Exp*_tmp8CE=fn_e;Cyc_Toc_fncall_exp_r(_tmp8CE,({struct Cyc_Absyn_Exp*_tmp326[3U];({struct Cyc_Absyn_Exp*_tmp8CD=Cyc_Toc_push_address_exp(_tmp4C4);_tmp326[0]=_tmp8CD;}),({
struct Cyc_Absyn_Exp*_tmp8CC=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp326[1]=_tmp8CC;}),({
struct Cyc_Absyn_Exp*_tmp8CB=Cyc_Absyn_signed_int_exp(change,0U);_tmp326[2]=_tmp8CB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp326,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1936
e->r=_tmp8CF;});}else{
# 1939
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1945
if(_tmp4C3 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp327=Cyc_Toc_temp_var();
void*_tmp328=({void*_tmp8D0=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp8D0,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp329=Cyc_Toc_push_address_exp(_tmp4C4);
struct Cyc_Absyn_Exp*_tmp32A=({struct Cyc_Absyn_Exp*_tmp8D1=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp327,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp8D1,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(_tmp325)
({struct Cyc_Absyn_Exp*_tmp8D4=({struct Cyc_Absyn_Exp*_tmp8D3=({struct Cyc_Absyn_Exp*_tmp8D2=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp327,0U),0U);Cyc_Absyn_neq_exp(_tmp8D2,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1952
Cyc_Absyn_and_exp(_tmp8D3,_tmp32A,0U);});_tmp32A=_tmp8D4;});{
# 1954
struct Cyc_Absyn_Stmt*_tmp32B=({struct Cyc_Absyn_Exp*_tmp8D6=_tmp32A;struct Cyc_Absyn_Stmt*_tmp8D5=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp327,0U),0U),0U),0U);
# 1954
Cyc_Absyn_ifthenelse_stmt(_tmp8D6,_tmp8D5,
# 1956
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp8D8=({struct Cyc_Absyn_Stmt*_tmp8D7=_tmp32B;Cyc_Absyn_seq_stmt(_tmp8D7,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp327,0U),0U),0U),0U);});_tmp32B=_tmp8D8;});
({void*_tmp8D9=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp327,_tmp328,_tmp329,_tmp32B,0U),0U))->r;e->r=_tmp8D9;});};}else{
# 1961
struct Cyc_Toc_functionSet*_tmp32C=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp32C,_tmp4C4);
({void*_tmp8DD=({struct Cyc_Absyn_Exp*_tmp8DC=fn_e;Cyc_Toc_fncall_exp_r(_tmp8DC,({struct Cyc_Absyn_Exp*_tmp32D[2U];({struct Cyc_Absyn_Exp*_tmp8DB=Cyc_Toc_push_address_exp(_tmp4C4);_tmp32D[0]=_tmp8DB;}),({
struct Cyc_Absyn_Exp*_tmp8DA=Cyc_Absyn_signed_int_exp(1,0U);_tmp32D[1]=_tmp8DA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32D,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1963
e->r=_tmp8DD;});}}else{
# 1966
if(elt_typ == Cyc_Absyn_void_type  && !Cyc_Absyn_is_lvalue(_tmp4C4)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4C4,0,Cyc_Toc_incr_lvalue,_tmp4C3);
e->r=_tmp4C4->r;}}}
# 1970
goto _LL0;};};}case 4U: _LLF: _tmp4C7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4C6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp4C5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_LL10: {
# 1989 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4C7->topt);
void*e2_old_typ=(void*)_check_null(_tmp4C5->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp4C7,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4C7);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4C5);
({void*_tmp8DE=Cyc_Toc_tagged_union_assignop(_tmp4C7,e1_old_typ,_tmp4C6,_tmp4C5,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp8DE;});
# 2000
return;}{
# 2002
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4C7,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4C7,_tmp4C6,_tmp4C5,ptr_type,is_dyneither,elt_type);
# 2008
return;}{
# 2012
int e1_poly=Cyc_Toc_is_poly_project(_tmp4C7);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4C7);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4C5);{
# 2018
int done=0;
if(_tmp4C6 != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp32F=(enum Cyc_Absyn_Primop)_tmp4C6->v;enum Cyc_Absyn_Primop _tmp330=_tmp32F;switch(_tmp330){case Cyc_Absyn_Plus: _LL82: _LL83:
 change=_tmp4C5;goto _LL81;case Cyc_Absyn_Minus: _LL84: _LL85:
# 2026
({struct Cyc_Absyn_Exp*_tmp8DF=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4C5,0U);change=_tmp8DF;});goto _LL81;default: _LL86: _LL87:
({void*_tmp331=0U;({struct _dyneither_ptr _tmp8E0=({const char*_tmp332="bad t ? pointer arithmetic";_tag_dyneither(_tmp332,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8E0,_tag_dyneither(_tmp331,sizeof(void*),0U));});});}_LL81:;}
# 2029
done=1;{
# 2031
struct Cyc_Absyn_Exp*_tmp333=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp8E4=({struct Cyc_Absyn_Exp*_tmp8E3=_tmp333;Cyc_Toc_fncall_exp_r(_tmp8E3,({struct Cyc_Absyn_Exp*_tmp334[3U];({struct Cyc_Absyn_Exp*_tmp8E2=Cyc_Toc_push_address_exp(_tmp4C7);_tmp334[0]=_tmp8E2;}),({
struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp334[1]=_tmp8E1;}),_tmp334[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp334,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2032
e->r=_tmp8E4;});};}else{
# 2035
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2038
enum Cyc_Absyn_Primop _tmp335=(enum Cyc_Absyn_Primop)_tmp4C6->v;enum Cyc_Absyn_Primop _tmp336=_tmp335;if(_tmp336 == Cyc_Absyn_Plus){_LL89: _LL8A:
# 2040
 done=1;
({void*_tmp8E6=({struct Cyc_Absyn_Exp*_tmp8E5=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4C7);Cyc_Toc_fncall_exp_r(_tmp8E5,({struct Cyc_Absyn_Exp*_tmp337[2U];_tmp337[0]=_tmp4C7,_tmp337[1]=_tmp4C5;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp337,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8E6;});
goto _LL88;}else{_LL8B: _LL8C:
({void*_tmp338=0U;({struct _dyneither_ptr _tmp8E7=({const char*_tmp339="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp339,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8E7,_tag_dyneither(_tmp338,sizeof(void*),0U));});});}_LL88:;}}}
# 2047
if(!done){
# 2049
if(e1_poly)
({void*_tmp8E9=({void*_tmp8E8=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp8E8,Cyc_Absyn_new_exp(_tmp4C5->r,0U));});_tmp4C5->r=_tmp8E9;});
# 2055
if(!Cyc_Absyn_is_lvalue(_tmp4C7)){
({struct Cyc_Absyn_Exp*_tmp8EA=_tmp4C7;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp8EA,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp33B=1;struct _tuple31*_tmp33A=_cycalloc(_check_times(_tmp33B,sizeof(struct _tuple31)));(_tmp33A[0]).f1=_tmp4C6,(_tmp33A[0]).f2=_tmp4C5;_tmp33A;}));});
e->r=_tmp4C7->r;}}
# 2060
goto _LL0;};};};}case 6U: _LL11: _tmp4CA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4C9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp4C8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_LL12:
# 2062
 Cyc_Toc_exp_to_c(nv,_tmp4CA);
Cyc_Toc_exp_to_c(nv,_tmp4C9);
Cyc_Toc_exp_to_c(nv,_tmp4C8);
goto _LL0;case 7U: _LL13: _tmp4CC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4CB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL14:
 _tmp4CE=_tmp4CC;_tmp4CD=_tmp4CB;goto _LL16;case 8U: _LL15: _tmp4CE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4CD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL16:
 _tmp4D0=_tmp4CE;_tmp4CF=_tmp4CD;goto _LL18;case 9U: _LL17: _tmp4D0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4CF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL18:
# 2069
 Cyc_Toc_exp_to_c(nv,_tmp4D0);
Cyc_Toc_exp_to_c(nv,_tmp4CF);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3 == 0){_LL19: _tmp4D2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4D1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL1A: {
# 2073
int null_ck=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_do_null_check(e);
void*e1_typ=(void*)_check_null(_tmp4D2->topt);
Cyc_Toc_exp_to_c(nv,_tmp4D2);
if(null_ck){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp33C=0U;({unsigned int _tmp8EC=e->loc;struct _dyneither_ptr _tmp8EB=({const char*_tmp33D="inserted null check";_tag_dyneither(_tmp33D,sizeof(char),20U);});Cyc_Tcutil_warn(_tmp8EC,_tmp8EB,_tag_dyneither(_tmp33C,sizeof(void*),0U));});});
# 2080
({void*_tmp8F0=({void*_tmp8EF=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp8EF,({
struct Cyc_Absyn_Exp*_tmp8EE=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp8EE,({struct Cyc_List_List*_tmp33E=_cycalloc(sizeof(*_tmp33E));
({struct Cyc_Absyn_Exp*_tmp8ED=Cyc_Absyn_copy_exp(_tmp4D2);_tmp33E->hd=_tmp8ED;}),_tmp33E->tl=0;_tmp33E;}),0U);}));});
# 2080
_tmp4D2->r=_tmp8F0;});}
# 2084
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4D1);
goto _LL0;}}else{_LL1B: _tmp4D7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4D6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp4D5=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3)->num_varargs;_tmp4D4=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3)->injectors;_tmp4D3=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3)->vai;_LL1C: {
# 2095 "toc.cyc"
struct _RegionHandle _tmp33F=_new_region("r");struct _RegionHandle*r=& _tmp33F;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4D5,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4D3->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2103
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4D6);
int num_normargs=num_args - _tmp4D5;
# 2107
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4D6=_tmp4D6->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D6))->hd);
({struct Cyc_List_List*_tmp8F1=({struct Cyc_List_List*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340->hd=(struct Cyc_Absyn_Exp*)_tmp4D6->hd,_tmp340->tl=new_args;_tmp340;});new_args=_tmp8F1;});}}
# 2112
({struct Cyc_List_List*_tmp8F5=({struct Cyc_List_List*_tmp342=_cycalloc(sizeof(*_tmp342));({struct Cyc_Absyn_Exp*_tmp8F4=({struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8F3,({struct Cyc_Absyn_Exp*_tmp341[3U];_tmp341[0]=argvexp,({
# 2114
struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp341[1]=_tmp8F2;}),_tmp341[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp341,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2112
_tmp342->hd=_tmp8F4;}),_tmp342->tl=new_args;_tmp342;});new_args=_tmp8F5;});
# 2117
({struct Cyc_List_List*_tmp8F6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp8F6;});{
# 2119
int null_ck=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_do_null_check(e);
void*e1_typ=(void*)_check_null(_tmp4D7->topt);
Cyc_Toc_exp_to_c(nv,_tmp4D7);
if(null_ck){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp343=0U;({unsigned int _tmp8F8=e->loc;struct _dyneither_ptr _tmp8F7=({const char*_tmp344="inserted null check";_tag_dyneither(_tmp344,sizeof(char),20U);});Cyc_Tcutil_warn(_tmp8F8,_tmp8F7,_tag_dyneither(_tmp343,sizeof(void*),0U));});});
# 2126
({void*_tmp8FC=({void*_tmp8FB=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp8FB,({
struct Cyc_Absyn_Exp*_tmp8FA=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp8FA,({struct Cyc_List_List*_tmp345=_cycalloc(sizeof(*_tmp345));
({struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Absyn_copy_exp(_tmp4D7);_tmp345->hd=_tmp8F9;}),_tmp345->tl=0;_tmp345;}),0U);}));});
# 2126
_tmp4D7->r=_tmp8FC;});}{
# 2130
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4D7,new_args,0U),0U);
# 2133
if(_tmp4D3->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp346=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4D3->type));void*_tmp347=_tmp346;struct Cyc_Absyn_Datatypedecl*_tmp34A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)->f1).KnownDatatype).tag == 2){_LL8E: _tmp34A=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)->f1).KnownDatatype).val;_LL8F:
 tud=_tmp34A;goto _LL8D;}else{goto _LL90;}}else{goto _LL90;}}else{_LL90: _LL91:
({void*_tmp348=0U;({struct _dyneither_ptr _tmp8FD=({const char*_tmp349="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp349,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8FD,_tag_dyneither(_tmp348,sizeof(void*),0U));});});}_LL8D:;}{
# 2139
struct _dyneither_ptr vs=({unsigned int _tmp35B=(unsigned int)_tmp4D5;struct _tuple1**_tmp35A=({struct _RegionHandle*_tmp8FE=r;_region_malloc(_tmp8FE,_check_times(_tmp35B,sizeof(struct _tuple1*)));});({{unsigned int _tmp78E=(unsigned int)_tmp4D5;unsigned int i;for(i=0;i < _tmp78E;++ i){({struct _tuple1*_tmp8FF=Cyc_Toc_temp_var();_tmp35A[i]=_tmp8FF;});}}0;});_tag_dyneither(_tmp35A,sizeof(struct _tuple1*),_tmp35B);});
# 2141
if(_tmp4D5 != 0){
# 2145
struct Cyc_List_List*_tmp34B=0;
{int i=_tmp4D5 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp901=({struct Cyc_List_List*_tmp34C=_cycalloc(sizeof(*_tmp34C));({struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp34C->hd=_tmp900;}),_tmp34C->tl=_tmp34B;_tmp34C;});_tmp34B=_tmp901;});}}
({struct Cyc_Absyn_Stmt*_tmp905=({struct _tuple1*_tmp904=argv;void*_tmp903=arr_type;struct Cyc_Absyn_Exp*_tmp902=Cyc_Absyn_array_exp(_tmp34B,0U);Cyc_Absyn_declare_stmt(_tmp904,_tmp903,_tmp902,s,0U);});s=_tmp905;});
# 2150
({struct Cyc_List_List*_tmp906=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4D6);_tmp4D6=_tmp906;});
({struct Cyc_List_List*_tmp907=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4D4);_tmp4D4=_tmp907;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4D6)- 1;
for(0;_tmp4D6 != 0;(_tmp4D6=_tmp4D6->tl,_tmp4D4=_tmp4D4->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4D6->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp34D=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4D4))->hd;struct Cyc_Absyn_Datatypefield*_tmp34E=_tmp34D;struct _tuple1*_tmp354;struct Cyc_List_List*_tmp353;_LL93: _tmp354=_tmp34E->name;_tmp353=_tmp34E->typs;_LL94:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp353))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp908=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp908;});{
# 2164
struct _tuple1*_tmp34F=Cyc_Toc_collapse_qvars(_tmp354,tud->name);
struct Cyc_List_List*_tmp350=({struct _tuple20*_tmp352[2U];({struct _tuple20*_tmp90C=({struct _dyneither_ptr*_tmp90B=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp90B,Cyc_Toc_datatype_tag(tud,_tmp354));});_tmp352[0]=_tmp90C;}),({
struct _tuple20*_tmp90A=({struct _dyneither_ptr*_tmp909=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp909,arg);});_tmp352[1]=_tmp90A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp352,sizeof(struct _tuple20*),2U));});
({struct Cyc_Absyn_Stmt*_tmp910=({struct _tuple1*_tmp90F=var;void*_tmp90E=Cyc_Absyn_strctq(_tmp34F);struct Cyc_Absyn_Exp*_tmp90D=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->tag=29U,_tmp351->f1=_tmp34F,_tmp351->f2=0,_tmp351->f3=_tmp350,_tmp351->f4=0;_tmp351;}),0U);
# 2167
Cyc_Absyn_declare_stmt(_tmp90F,_tmp90E,_tmp90D,s,0U);});s=_tmp910;});};};}};}else{
# 2175
struct Cyc_List_List*_tmp355=({struct _tuple32*_tmp356[3U];({struct _tuple32*_tmp919=({struct _tuple32*_tmp357=_cycalloc(sizeof(*_tmp357));({struct _dyneither_ptr _tmp918=_tag_dyneither(0,0,0);_tmp357->f1=_tmp918;}),({struct Cyc_Absyn_Exp*_tmp917=Cyc_Absyn_uint_exp(0U,0U);_tmp357->f2=_tmp917;});_tmp357;});_tmp356[0]=_tmp919;}),({
struct _tuple32*_tmp916=({struct _tuple32*_tmp358=_cycalloc(sizeof(*_tmp358));({struct _dyneither_ptr _tmp915=_tag_dyneither(0,0,0);_tmp358->f1=_tmp915;}),({struct Cyc_Absyn_Exp*_tmp914=Cyc_Absyn_uint_exp(0U,0U);_tmp358->f2=_tmp914;});_tmp358;});_tmp356[1]=_tmp916;}),({
struct _tuple32*_tmp913=({struct _tuple32*_tmp359=_cycalloc(sizeof(*_tmp359));({struct _dyneither_ptr _tmp912=_tag_dyneither(0,0,0);_tmp359->f1=_tmp912;}),({struct Cyc_Absyn_Exp*_tmp911=Cyc_Absyn_uint_exp(0U,0U);_tmp359->f2=_tmp911;});_tmp359;});_tmp356[2]=_tmp913;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp356,sizeof(struct _tuple32*),3U));});
({struct Cyc_Absyn_Stmt*_tmp91D=({struct _tuple1*_tmp91C=argv;void*_tmp91B=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp91A=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp91C,_tmp91B,_tmp91A,s,0U);});s=_tmp91D;});}};}else{
# 2183
if(_tmp4D5 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp4D6 != 0;_tmp4D6=_tmp4D6->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4D6->hd);
({struct Cyc_List_List*_tmp91E=({struct Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C->hd=(struct Cyc_Absyn_Exp*)_tmp4D6->hd,_tmp35C->tl=array_args;_tmp35C;});array_args=_tmp91E;});}{
# 2189
struct Cyc_Absyn_Exp*_tmp35D=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp91F=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp35D,s,0U);s=_tmp91F;});};}else{
# 2192
({struct Cyc_Absyn_Stmt*_tmp923=({struct _tuple1*_tmp922=argv;void*_tmp921=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp920=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp922,_tmp921,_tmp920,s,0U);});s=_tmp923;});}}
# 2195
({void*_tmp924=Cyc_Toc_stmt_exp_r(s);e->r=_tmp924;});};};}
# 2197
_npop_handler(0U);goto _LL0;
# 2095
;_pop_region(r);}}case 11U: _LL1D: _tmp4D9=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4D8=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL1E:
# 2200
 Cyc_Toc_exp_to_c(nv,_tmp4D9);{
struct Cyc_Absyn_Exp*_tmp35E=_tmp4D8?Cyc_Toc_newrethrow_exp(_tmp4D9): Cyc_Toc_newthrow_exp(_tmp4D9);
({void*_tmp926=({void*_tmp925=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp925,_tmp35E,0U);})->r;e->r=_tmp926;});
goto _LL0;};case 12U: _LL1F: _tmp4DA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL20:
 Cyc_Toc_exp_to_c(nv,_tmp4DA);goto _LL0;case 13U: _LL21: _tmp4DC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4DB=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL22:
# 2206
 Cyc_Toc_exp_to_c(nv,_tmp4DC);
# 2215 "toc.cyc"
for(0;_tmp4DB != 0;_tmp4DB=_tmp4DB->tl){
enum Cyc_Absyn_KindQual _tmp35F=(Cyc_Tcutil_type_kind((void*)_tmp4DB->hd))->kind;
if(_tmp35F != Cyc_Absyn_EffKind  && _tmp35F != Cyc_Absyn_RgnKind){
{void*_tmp360=Cyc_Tcutil_compress((void*)_tmp4DB->hd);void*_tmp361=_tmp360;switch(*((int*)_tmp361)){case 2U: _LL96: _LL97:
 goto _LL99;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp361)->f1)->tag == 18U){_LL98: _LL99:
 continue;}else{goto _LL9A;}default: _LL9A: _LL9B:
# 2222
({void*_tmp928=({void*_tmp927=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp927,_tmp4DC,0U);})->r;e->r=_tmp928;});
goto _LL95;}_LL95:;}
# 2225
break;}}
# 2228
goto _LL0;case 14U: _LL23: _tmp4E0=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4DF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp4DE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp4DD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL24: {
# 2230
void*old_t2=(void*)_check_null(_tmp4DF->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4E0;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4E0=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4DF);
# 2237
{struct _tuple33 _tmp362=({struct _tuple33 _tmp791;({void*_tmp92A=Cyc_Tcutil_compress(old_t2);_tmp791.f1=_tmp92A;}),({void*_tmp929=Cyc_Tcutil_compress(new_typ);_tmp791.f2=_tmp929;});_tmp791;});struct _tuple33 _tmp363=_tmp362;struct Cyc_Absyn_PtrInfo _tmp39C;struct Cyc_Absyn_PtrInfo _tmp39B;struct Cyc_Absyn_PtrInfo _tmp39A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp363.f1)->tag == 3U)switch(*((int*)_tmp363.f2)){case 3U: _LL9D: _tmp39B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp363.f1)->f1;_tmp39A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp363.f2)->f1;_LL9E: {
# 2239
int _tmp364=Cyc_Tcutil_force_type2bool(0,(_tmp39B.ptr_atts).nullable);
int _tmp365=Cyc_Tcutil_force_type2bool(0,(_tmp39A.ptr_atts).nullable);
struct Cyc_Absyn_Exp*_tmp366=({void*_tmp92B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp92B,(_tmp39B.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp367=({void*_tmp92C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp92C,(_tmp39A.ptr_atts).bounds);});
int _tmp368=Cyc_Tcutil_force_type2bool(0,(_tmp39B.ptr_atts).zero_term);
int _tmp369=Cyc_Tcutil_force_type2bool(0,(_tmp39A.ptr_atts).zero_term);
{struct _tuple34 _tmp36A=({struct _tuple34 _tmp790;_tmp790.f1=_tmp366,_tmp790.f2=_tmp367;_tmp790;});struct _tuple34 _tmp36B=_tmp36A;if(_tmp36B.f1 != 0){if(_tmp36B.f2 != 0){_LLA4: _LLA5:
# 2248
 if((!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp366)) || !Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp367))) && !
Cyc_Evexp_same_const_exp(_tmp366,(struct Cyc_Absyn_Exp*)_check_null(_tmp367)))
({void*_tmp36C=0U;({unsigned int _tmp92E=e->loc;struct _dyneither_ptr _tmp92D=({const char*_tmp36D="Toc: cast to possibly shorter pointer type";_tag_dyneither(_tmp36D,sizeof(char),43U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp92E,_tmp92D,_tag_dyneither(_tmp36C,sizeof(void*),0U));});});
if(_tmp364  && !_tmp365){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp36E=0U;({struct _dyneither_ptr _tmp92F=({const char*_tmp36F="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp36F,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp92F,_tag_dyneither(_tmp36E,sizeof(void*),0U));});});
# 2257
if(_tmp4DD != Cyc_Absyn_Null_to_NonNull)
({void*_tmp370=0U;({unsigned int _tmp931=e->loc;struct _dyneither_ptr _tmp930=({const char*_tmp371="null-check conversion mis-classified";_tag_dyneither(_tmp371,sizeof(char),37U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp931,_tmp930,_tag_dyneither(_tmp370,sizeof(void*),0U));});});
if(Cyc_Toc_do_null_check(e)){
if(!_tmp4DE)
({void*_tmp372=0U;({unsigned int _tmp933=e->loc;struct _dyneither_ptr _tmp932=({const char*_tmp373="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp373,sizeof(char),58U);});Cyc_Tcutil_warn(_tmp933,_tmp932,_tag_dyneither(_tmp372,sizeof(void*),0U));});});
# 2263
({void*_tmp936=({void*_tmp935=*_tmp4E0;Cyc_Toc_cast_it_r(_tmp935,({struct Cyc_Absyn_Exp*_tmp934=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp934,({struct Cyc_List_List*_tmp374=_cycalloc(sizeof(*_tmp374));
_tmp374->hd=_tmp4DF,_tmp374->tl=0;_tmp374;}),0U);}));});
# 2263
e->r=_tmp936;});}else{
# 2267
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4DF->r;}}else{
# 2272
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4DF->r;}
# 2279
goto _LLA3;}else{_LLA6: _LLA7: {
# 2281
struct Cyc_Absyn_Exp*_tmp375=(struct Cyc_Absyn_Exp*)_check_null(_tmp366);
if(!Cyc_Evexp_c_can_eval(_tmp375))
({void*_tmp376=0U;({unsigned int _tmp938=e->loc;struct _dyneither_ptr _tmp937=({const char*_tmp377="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp377,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp938,_tmp937,_tag_dyneither(_tmp376,sizeof(void*),0U));});});{
struct _tuple13 _tmp378=Cyc_Evexp_eval_const_uint_exp(_tmp375);struct _tuple13 _tmp379=_tmp378;unsigned int _tmp38B;int _tmp38A;_LLAD: _tmp38B=_tmp379.f1;_tmp38A=_tmp379.f2;_LLAE:;
# 2286
if(_tmp4DD == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp37C=({struct Cyc_String_pa_PrintArg_struct _tmp78F;_tmp78F.tag=0U,({struct _dyneither_ptr _tmp939=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp78F.f1=_tmp939;});_tmp78F;});void*_tmp37A[1U];_tmp37A[0]=& _tmp37C;({struct _dyneither_ptr _tmp93A=({const char*_tmp37B="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp37B,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp93A,_tag_dyneither(_tmp37A,sizeof(void*),1U));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2290
if((_tmp368  && !(_tmp39A.elt_tq).real_const) && !_tmp369)
# 2293
({struct Cyc_Absyn_Exp*_tmp93C=({struct Cyc_Absyn_Exp*_tmp93B=_tmp375;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp93B,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp375=_tmp93C;});
# 2295
({void*_tmp93D=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp375,_tmp4DF))->r;e->r=_tmp93D;});}else{
# 2297
struct Cyc_Absyn_Exp*_tmp37D=Cyc_Toc__tag_dyneither_e;
# 2299
if(_tmp368){
# 2304
struct _tuple1*_tmp37E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp37F=Cyc_Absyn_var_exp(_tmp37E,0U);
struct Cyc_Absyn_Exp*arg3;
# 2309
{void*_tmp380=_tmp4DF->r;void*_tmp381=_tmp380;struct Cyc_Absyn_Vardecl*_tmp384;struct Cyc_Absyn_Vardecl*_tmp383;switch(*((int*)_tmp381)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp381)->f1).Wstring_c).tag){case 8U: _LLB0: _LLB1:
# 2311
 arg3=_tmp375;goto _LLAF;case 9U: _LLB2: _LLB3:
# 2313
 arg3=_tmp375;goto _LLAF;default: goto _LLB8;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp381)->f1)){case 1U: _LLB4: _tmp383=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp381)->f1)->f1;_LLB5:
 _tmp384=_tmp383;goto _LLB7;case 4U: _LLB6: _tmp384=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp381)->f1)->f1;_LLB7:
# 2316
 if(!Cyc_Tcutil_is_array_type(_tmp384->type))goto _LLB9;
arg3=_tmp375;
goto _LLAF;default: goto _LLB8;}default: _LLB8: _LLB9:
# 2322
 if(_tmp38A  && _tmp38B != 1)
arg3=_tmp375;else{
# 2325
({struct Cyc_Absyn_Exp*_tmp941=({struct Cyc_Absyn_Exp*_tmp940=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4DF);Cyc_Absyn_fncall_exp(_tmp940,({struct Cyc_Absyn_Exp*_tmp382[2U];({
# 2327
struct Cyc_Absyn_Exp*_tmp93F=({void*_tmp93E=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp93E,_tmp37F);});_tmp382[0]=_tmp93F;}),_tmp382[1]=_tmp375;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp382,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2325
arg3=_tmp941;});}
# 2328
goto _LLAF;}_LLAF:;}
# 2330
if(!_tmp369  && !(_tmp39A.elt_tq).real_const)
# 2333
({struct Cyc_Absyn_Exp*_tmp943=({struct Cyc_Absyn_Exp*_tmp942=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp942,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp943;});{
# 2335
struct Cyc_Absyn_Exp*_tmp385=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp39A.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp386=({struct Cyc_Absyn_Exp*_tmp944=_tmp37D;Cyc_Absyn_fncall_exp(_tmp944,({struct Cyc_Absyn_Exp*_tmp388[3U];_tmp388[0]=_tmp37F,_tmp388[1]=_tmp385,_tmp388[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp388,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp387=Cyc_Absyn_exp_stmt(_tmp386,0U);
({struct Cyc_Absyn_Stmt*_tmp948=({struct _tuple1*_tmp947=_tmp37E;void*_tmp946=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp945=_tmp4DF;Cyc_Absyn_declare_stmt(_tmp947,_tmp946,_tmp945,_tmp387,0U);});_tmp387=_tmp948;});
({void*_tmp949=Cyc_Toc_stmt_exp_r(_tmp387);e->r=_tmp949;});};}else{
# 2342
({void*_tmp94C=({struct Cyc_Absyn_Exp*_tmp94B=_tmp37D;Cyc_Toc_fncall_exp_r(_tmp94B,({struct Cyc_Absyn_Exp*_tmp389[3U];_tmp389[0]=_tmp4DF,({
# 2344
struct Cyc_Absyn_Exp*_tmp94A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp39A.elt_type),0U);_tmp389[1]=_tmp94A;}),_tmp389[2]=_tmp375;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp389,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2342
e->r=_tmp94C;});}}
# 2348
goto _LLA3;};}}}else{if(_tmp36B.f2 != 0){_LLA8: _LLA9: {
# 2350
struct Cyc_Absyn_Exp*_tmp38C=(struct Cyc_Absyn_Exp*)_check_null(_tmp367);
if(!Cyc_Evexp_c_can_eval(_tmp38C))
({void*_tmp38D=0U;({unsigned int _tmp94E=e->loc;struct _dyneither_ptr _tmp94D=({const char*_tmp38E="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp38E,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp94E,_tmp94D,_tag_dyneither(_tmp38D,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp38F=0U;({struct _dyneither_ptr _tmp94F=({const char*_tmp390="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp390,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp94F,_tag_dyneither(_tmp38F,sizeof(void*),0U));});});{
# 2363 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp391=_tmp38C;
if(_tmp368  && !_tmp369)
({struct Cyc_Absyn_Exp*_tmp951=({struct Cyc_Absyn_Exp*_tmp950=_tmp38C;Cyc_Absyn_add_exp(_tmp950,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp391=_tmp951;});{
# 2370
struct Cyc_Absyn_Exp*_tmp392=({struct Cyc_Absyn_Exp*_tmp953=Cyc_Toc__untag_dyneither_ptr_e;Cyc_Absyn_fncall_exp(_tmp953,({struct Cyc_Absyn_Exp*_tmp394[3U];_tmp394[0]=_tmp4DF,({
# 2372
struct Cyc_Absyn_Exp*_tmp952=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp39B.elt_type),0U);_tmp394[1]=_tmp952;}),_tmp394[2]=_tmp391;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp394,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2374
if(!_tmp365)
({void*_tmp956=({struct Cyc_Absyn_Exp*_tmp955=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp955,({struct Cyc_List_List*_tmp393=_cycalloc(sizeof(*_tmp393));
({struct Cyc_Absyn_Exp*_tmp954=Cyc_Absyn_copy_exp(_tmp392);_tmp393->hd=_tmp954;}),_tmp393->tl=0;_tmp393;}));});
# 2375
_tmp392->r=_tmp956;});
# 2377
({void*_tmp957=Cyc_Toc_cast_it_r(*_tmp4E0,_tmp392);e->r=_tmp957;});
goto _LLA3;};};}}else{_LLAA: _LLAB:
# 2382
 DynCast:
 if((_tmp368  && !_tmp369) && !(_tmp39A.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp395=0U;({struct _dyneither_ptr _tmp958=({const char*_tmp396="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp396,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp958,_tag_dyneither(_tmp395,sizeof(void*),0U));});});
({void*_tmp95C=({struct Cyc_Absyn_Exp*_tmp95B=Cyc_Toc__dyneither_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp95B,({struct Cyc_Absyn_Exp*_tmp397[3U];_tmp397[0]=_tmp4DF,({
struct Cyc_Absyn_Exp*_tmp95A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp39B.elt_type),0U);_tmp397[1]=_tmp95A;}),({
struct Cyc_Absyn_Exp*_tmp959=Cyc_Absyn_uint_exp(1U,0U);_tmp397[2]=_tmp959;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp397,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2386
e->r=_tmp95C;});}
# 2390
goto _LLA3;}}_LLA3:;}
# 2392
goto _LL9C;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp363.f2)->f1)->tag == 1U){_LL9F: _tmp39C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp363.f1)->f1;_LLA0:
# 2394
{struct Cyc_Absyn_Exp*_tmp398=({void*_tmp95D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp95D,(_tmp39C.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp399=_tmp398;if(_tmp399 == 0){_LLBB: _LLBC:
# 2396
({void*_tmp95F=({struct Cyc_Absyn_Exp*_tmp95E=Cyc_Absyn_new_exp(_tmp4DF->r,_tmp4DF->loc);Cyc_Toc_aggrmember_exp_r(_tmp95E,Cyc_Toc_curr_sp);});_tmp4DF->r=_tmp95F;});
_tmp4DF->topt=new_typ_c;
goto _LLBA;}else{_LLBD: _LLBE:
 goto _LLBA;}_LLBA:;}
# 2401
goto _LL9C;}else{goto _LLA1;}default: goto _LLA1;}else{_LLA1: _LLA2:
# 2403
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4DF->r;
goto _LL9C;}_LL9C:;}
# 2407
goto _LL0;}case 15U: _LL25: _tmp4E1=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL26:
# 2410
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4E1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4E1,0,Cyc_Toc_address_lvalue,1);
# 2416
({void*_tmp961=({void*_tmp960=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp960,_tmp4E1);});e->r=_tmp961;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp4E1->topt))))
# 2420
({void*_tmp963=({void*_tmp962=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp962,_tmp4E1);});e->r=_tmp963;});}
# 2422
goto _LL0;case 16U: _LL27: _tmp4E3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4E2=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL28:
# 2429
 Cyc_Toc_exp_to_c(nv,_tmp4E2);{
# 2431
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4E2->topt));
struct _tuple1*_tmp39D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_var_exp(_tmp39D,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp39F=Cyc_Tcutil_compress(elt_typ);void*_tmp3A0=_tmp39F;void*_tmp3A1;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A0)->tag == 4U){_LLC0: _tmp3A1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A0)->f1).elt_type;_LLC1:
# 2437
({void*_tmp964=Cyc_Toc_typ_to_c(_tmp3A1);elt_typ=_tmp964;});
({struct Cyc_Absyn_Exp*_tmp965=Cyc_Absyn_copy_exp(_tmp39E);lhs=_tmp965;});
goto _LLBF;}else{_LLC2: _LLC3:
# 2441
({struct Cyc_Absyn_Exp*_tmp966=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp39E),0U);lhs=_tmp966;});
goto _LLBF;}_LLBF:;}{
# 2444
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp4E2);
struct _tuple1*_tmp3A2=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp3A3=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp3A2),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp969=({struct Cyc_Absyn_Exp*_tmp968=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp968,({struct Cyc_Absyn_Exp*_tmp3A4[2U];_tmp3A4[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp3A3),({
struct Cyc_Absyn_Exp*_tmp967=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3A4[1]=_tmp967;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A4,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2449
mexp=_tmp969;});else{
# 2452
({struct Cyc_Absyn_Exp*_tmp96A=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp39E,0U),0U);mexp=_tmp96A;});}{
# 2455
struct Cyc_Absyn_Exp*_tmp3A5=Cyc_Toc_get_varsizeexp(_tmp4E2);
if(_tmp3A5 != 0)
({struct Cyc_Absyn_Exp*_tmp96B=Cyc_Absyn_add_exp(mexp,_tmp3A5,0U);mexp=_tmp96B;});
# 2459
if(_tmp4E3 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp96C=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp96C;});else{
# 2462
struct Cyc_Absyn_Exp*r=_tmp4E3;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp96D=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp96D;});}{
# 2466
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp972=({struct Cyc_Absyn_Exp*_tmp971=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp971,({struct Cyc_Absyn_Exp*_tmp3A6[3U];({
struct Cyc_Absyn_Exp*_tmp970=Cyc_Absyn_copy_exp(_tmp39E);_tmp3A6[0]=_tmp970;}),({
struct Cyc_Absyn_Exp*_tmp96F=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3A6[1]=_tmp96F;}),
(unsigned int)_tmp3A3?_tmp3A6[2]=_tmp3A3:({struct Cyc_Absyn_Exp*_tmp96E=Cyc_Absyn_uint_exp(1U,0U);_tmp3A6[2]=_tmp96E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A6,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2468
answer=_tmp972;});else{
# 2473
({struct Cyc_Absyn_Exp*_tmp973=Cyc_Absyn_copy_exp(_tmp39E);answer=_tmp973;});}
({void*_tmp974=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7->tag=Cyc_Toc_Dest,_tmp3A7->f1=_tmp39E;_tmp3A7;});e->annot=_tmp974;});{
struct Cyc_Absyn_Stmt*_tmp3A8=({struct _tuple1*_tmp978=_tmp39D;void*_tmp977=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp976=mexp;Cyc_Absyn_declare_stmt(_tmp978,_tmp977,_tmp976,({
struct Cyc_Absyn_Stmt*_tmp975=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp975,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2479
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp979=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp3A2),Cyc_Absyn_uint_type,array_len,_tmp3A8,0U);_tmp3A8=_tmp979;});
({void*_tmp97A=Cyc_Toc_stmt_exp_r(_tmp3A8);e->r=_tmp97A;});
if(_tmp3A5 != 0)
({void*_tmp97C=({void*_tmp97B=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp97B,Cyc_Absyn_copy_exp(e));});e->r=_tmp97C;});
goto _LL0;};};};};};case 18U: _LL29: _tmp4E4=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL2A: {
# 2488
int _tmp3A9=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E4);
Cyc_Toc_set_in_sizeof(nv,_tmp3A9);
goto _LL0;}case 17U: _LL2B: _tmp4E5=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL2C:
({void*_tmp97E=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA->tag=17U,({void*_tmp97D=Cyc_Toc_typ_to_c(_tmp4E5);_tmp3AA->f1=_tmp97D;});_tmp3AA;});e->r=_tmp97E;});goto _LL0;case 19U: _LL2D: _tmp4E7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4E6=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL2E: {
# 2494
void*_tmp3AB=_tmp4E7;
struct Cyc_List_List*_tmp3AC=_tmp4E6;
for(0;_tmp3AC != 0;_tmp3AC=_tmp3AC->tl){
void*_tmp3AD=(void*)_tmp3AC->hd;void*_tmp3AE=_tmp3AD;unsigned int _tmp3C0;struct _dyneither_ptr*_tmp3BF;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3AE)->tag == 0U){_LLC5: _tmp3BF=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3AE)->f1;_LLC6:
 goto _LLC4;}else{_LLC7: _tmp3C0=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3AE)->f1;_LLC8:
# 2500
{void*_tmp3AF=Cyc_Tcutil_compress(_tmp3AB);void*_tmp3B0=_tmp3AF;struct Cyc_List_List*_tmp3BE;struct Cyc_List_List*_tmp3BD;struct Cyc_Absyn_Datatypefield*_tmp3BC;union Cyc_Absyn_AggrInfo _tmp3BB;switch(*((int*)_tmp3B0)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f1)){case 20U: _LLCA: _tmp3BB=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f1)->f1;_LLCB: {
# 2502
struct Cyc_Absyn_Aggrdecl*_tmp3B1=Cyc_Absyn_get_known_aggrdecl(_tmp3BB);
if(_tmp3B1->impl == 0)
({void*_tmp3B2=0U;({struct _dyneither_ptr _tmp97F=({const char*_tmp3B3="struct fields must be known";_tag_dyneither(_tmp3B3,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp97F,_tag_dyneither(_tmp3B2,sizeof(void*),0U));});});
_tmp3BD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3B1->impl))->fields;goto _LLCD;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f1)->f1).KnownDatatypefield).tag == 2){_LLD0: _tmp3BC=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f1)->f1).KnownDatatypefield).val).f2;_LLD1:
# 2516
 if(_tmp3C0 == 0)
({void*_tmp980=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->tag=0U,_tmp3B7->f1=Cyc_Toc_tag_sp;_tmp3B7;}));_tmp3AC->hd=_tmp980;});else{
# 2519
({void*_tmp981=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3BC->typs,(int)(_tmp3C0 - 1))).f2;_tmp3AB=_tmp981;});
({void*_tmp983=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->tag=0U,({struct _dyneither_ptr*_tmp982=Cyc_Absyn_fieldname((int)_tmp3C0);_tmp3B8->f1=_tmp982;});_tmp3B8;}));_tmp3AC->hd=_tmp983;});}
# 2522
goto _LLC9;}else{goto _LLD2;}default: goto _LLD2;}case 7U: _LLCC: _tmp3BD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B0)->f2;_LLCD: {
# 2507
struct Cyc_Absyn_Aggrfield*_tmp3B4=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3BD,(int)_tmp3C0);
({void*_tmp984=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->tag=0U,_tmp3B5->f1=_tmp3B4->name;_tmp3B5;}));_tmp3AC->hd=_tmp984;});
_tmp3AB=_tmp3B4->type;
goto _LLC9;}case 6U: _LLCE: _tmp3BE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3B0)->f1;_LLCF:
# 2512
({void*_tmp986=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->tag=0U,({struct _dyneither_ptr*_tmp985=Cyc_Absyn_fieldname((int)(_tmp3C0 + 1));_tmp3B6->f1=_tmp985;});_tmp3B6;}));_tmp3AC->hd=_tmp986;});
({void*_tmp987=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3BE,(int)_tmp3C0)).f2;_tmp3AB=_tmp987;});
goto _LLC9;default: _LLD2: _LLD3:
# 2523
({void*_tmp3B9=0U;({struct _dyneither_ptr _tmp988=({const char*_tmp3BA="impossible type for offsetof tuple index";_tag_dyneither(_tmp3BA,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp988,_tag_dyneither(_tmp3B9,sizeof(void*),0U));});});
goto _LLC9;}_LLC9:;}
# 2526
goto _LLC4;}_LLC4:;}
# 2529
({void*_tmp98A=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->tag=19U,({void*_tmp989=Cyc_Toc_typ_to_c(_tmp4E7);_tmp3C1->f1=_tmp989;}),_tmp3C1->f2=_tmp4E6;_tmp3C1;});e->r=_tmp98A;});
goto _LL0;}case 20U: _LL2F: _tmp4E8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL30: {
# 2532
int _tmp3C2=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp3C3=Cyc_Tcutil_compress((void*)_check_null(_tmp4E8->topt));
{void*_tmp3C4=_tmp3C3;void*_tmp3DA;struct Cyc_Absyn_Tqual _tmp3D9;void*_tmp3D8;void*_tmp3D7;void*_tmp3D6;void*_tmp3D5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C4)->tag == 3U){_LLD5: _tmp3DA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C4)->f1).elt_type;_tmp3D9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C4)->f1).elt_tq;_tmp3D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C4)->f1).ptr_atts).rgn;_tmp3D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C4)->f1).ptr_atts).nullable;_tmp3D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C4)->f1).ptr_atts).bounds;_tmp3D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C4)->f1).ptr_atts).zero_term;_LLD6:
# 2537
{struct Cyc_Absyn_Exp*_tmp3C5=({void*_tmp98B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp98B,_tmp3D6);});struct Cyc_Absyn_Exp*_tmp3C6=_tmp3C5;struct Cyc_Absyn_Exp*_tmp3D2;if(_tmp3C6 != 0){_LLDA: _tmp3D2=_tmp3C6;_LLDB: {
# 2539
struct Cyc_Absyn_Exp*_tmp3C7=(struct Cyc_Absyn_Exp*)_check_null(_tmp3D2);
int null_ck=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_do_null_check(e);
Cyc_Toc_exp_to_c(nv,_tmp4E8);
if(null_ck){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3C8=0U;({unsigned int _tmp98D=e->loc;struct _dyneither_ptr _tmp98C=({const char*_tmp3C9="inserted null check due to dereference";_tag_dyneither(_tmp3C9,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp98D,_tmp98C,_tag_dyneither(_tmp3C8,sizeof(void*),0U));});});
# 2546
({void*_tmp991=({void*_tmp990=Cyc_Toc_typ_to_c(_tmp3C3);Cyc_Toc_cast_it_r(_tmp990,({
struct Cyc_Absyn_Exp*_tmp98F=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp98F,({struct Cyc_List_List*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));
({struct Cyc_Absyn_Exp*_tmp98E=Cyc_Absyn_copy_exp(_tmp4E8);_tmp3CA->hd=_tmp98E;}),_tmp3CA->tl=0;_tmp3CA;}),0U);}));});
# 2546
_tmp4E8->r=_tmp991;});}
# 2554
if(!Cyc_Toc_in_sizeof(nv) && !Cyc_Tcutil_force_type2bool(0,_tmp3D5)){
struct _tuple13 _tmp3CB=Cyc_Evexp_eval_const_uint_exp(_tmp3C7);struct _tuple13 _tmp3CC=_tmp3CB;unsigned int _tmp3D0;int _tmp3CF;_LLDF: _tmp3D0=_tmp3CC.f1;_tmp3CF=_tmp3CC.f2;_LLE0:;
# 2560
if(!_tmp3CF  || _tmp3D0 <= 0)
({void*_tmp3CD=0U;({unsigned int _tmp993=e->loc;struct _dyneither_ptr _tmp992=({const char*_tmp3CE="cannot determine dereference is in bounds";_tag_dyneither(_tmp3CE,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp993,_tmp992,_tag_dyneither(_tmp3CD,sizeof(void*),0U));});});}
# 2563
goto _LLD9;}}else{_LLDC: _LLDD: {
# 2566
struct Cyc_Absyn_Exp*_tmp3D1=Cyc_Absyn_uint_exp(0U,0U);
_tmp3D1->topt=Cyc_Absyn_uint_type;
({void*_tmp994=Cyc_Toc_subscript_exp_r(_tmp4E8,_tmp3D1);e->r=_tmp994;});
Cyc_Toc_exp_to_c(nv,e);
goto _LLD9;}}_LLD9:;}
# 2572
goto _LLD4;}else{_LLD7: _LLD8:
({void*_tmp3D3=0U;({struct _dyneither_ptr _tmp995=({const char*_tmp3D4="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp3D4,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp995,_tag_dyneither(_tmp3D3,sizeof(void*),0U));});});}_LLD4:;}
# 2575
Cyc_Toc_set_lhs(nv,_tmp3C2);
goto _LL0;};}case 21U: _LL31: _tmp4EC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4EB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp4EA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp4E9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL32: {
# 2578
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4EC->topt);
Cyc_Toc_exp_to_c(nv,_tmp4EC);
if(_tmp4EA  && _tmp4E9)
({void*_tmp99A=({struct Cyc_Absyn_Exp*_tmp999=_tmp4EC;void*_tmp998=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp997=e1_cyc_type;struct _dyneither_ptr*_tmp996=_tmp4EB;Cyc_Toc_check_tagged_union(_tmp999,_tmp998,_tmp997,_tmp996,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2582
e->r=_tmp99A;});
# 2586
if(is_poly)
({void*_tmp99C=({void*_tmp99B=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp99B,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp99C;});
goto _LL0;}case 22U: _LL33: _tmp4F0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4EF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp4EE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp4ED=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL34: {
# 2590
int _tmp3DB=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4F0->topt));
int null_ck=Cyc_Toc_do_null_check(e);
Cyc_Toc_exp_to_c(nv,_tmp4F0);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp3DC=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp3DD=_tmp3DC;void*_tmp3F4;struct Cyc_Absyn_Tqual _tmp3F3;void*_tmp3F2;void*_tmp3F1;void*_tmp3F0;void*_tmp3EF;_LLE2: _tmp3F4=_tmp3DD.elt_type;_tmp3F3=_tmp3DD.elt_tq;_tmp3F2=(_tmp3DD.ptr_atts).rgn;_tmp3F1=(_tmp3DD.ptr_atts).nullable;_tmp3F0=(_tmp3DD.ptr_atts).bounds;_tmp3EF=(_tmp3DD.ptr_atts).zero_term;_LLE3:;
{struct Cyc_Absyn_Exp*_tmp3DE=({void*_tmp99D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp99D,_tmp3F0);});struct Cyc_Absyn_Exp*_tmp3DF=_tmp3DE;struct Cyc_Absyn_Exp*_tmp3EE;if(_tmp3DF != 0){_LLE5: _tmp3EE=_tmp3DF;_LLE6: {
# 2601
struct Cyc_Absyn_Exp*_tmp3E0=(struct Cyc_Absyn_Exp*)_check_null(_tmp3EE);
struct _tuple13 _tmp3E1=Cyc_Evexp_eval_const_uint_exp(_tmp3E0);struct _tuple13 _tmp3E2=_tmp3E1;unsigned int _tmp3EC;int _tmp3EB;_LLEA: _tmp3EC=_tmp3E2.f1;_tmp3EB=_tmp3E2.f2;_LLEB:;
if(_tmp3EB){
if(_tmp3EC < 1)
({void*_tmp3E3=0U;({struct _dyneither_ptr _tmp99E=({const char*_tmp3E4="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp3E4,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp99E,_tag_dyneither(_tmp3E3,sizeof(void*),0U));});});
if(null_ck){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3E5=0U;({unsigned int _tmp9A0=e->loc;struct _dyneither_ptr _tmp99F=({const char*_tmp3E6="inserted null check due to dereference";_tag_dyneither(_tmp3E6,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9A0,_tmp99F,_tag_dyneither(_tmp3E5,sizeof(void*),0U));});});
# 2610
({void*_tmp9A4=({void*_tmp9A3=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9A3,({
struct Cyc_Absyn_Exp*_tmp9A2=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9A2,({struct Cyc_Absyn_Exp*_tmp3E7[1U];({struct Cyc_Absyn_Exp*_tmp9A1=
Cyc_Absyn_new_exp(_tmp4F0->r,0U);_tmp3E7[0]=_tmp9A1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3E7,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2610
_tmp4F0->r=_tmp9A4;});}}else{
# 2615
if(!Cyc_Evexp_c_can_eval(_tmp3E0))
({void*_tmp3E8=0U;({unsigned int _tmp9A6=e->loc;struct _dyneither_ptr _tmp9A5=({const char*_tmp3E9="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp3E9,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp9A6,_tmp9A5,_tag_dyneither(_tmp3E8,sizeof(void*),0U));});});
# 2619
({void*_tmp9AC=({void*_tmp9AB=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9AB,({
struct Cyc_Absyn_Exp*_tmp9AA=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9AA,({struct Cyc_Absyn_Exp*_tmp3EA[4U];({
struct Cyc_Absyn_Exp*_tmp9A9=Cyc_Absyn_new_exp(_tmp4F0->r,0U);_tmp3EA[0]=_tmp9A9;}),_tmp3EA[1]=_tmp3E0,({
struct Cyc_Absyn_Exp*_tmp9A8=Cyc_Absyn_sizeoftype_exp(_tmp3F4,0U);_tmp3EA[2]=_tmp9A8;}),({
struct Cyc_Absyn_Exp*_tmp9A7=Cyc_Absyn_uint_exp(0U,0U);_tmp3EA[3]=_tmp9A7;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3EA,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 2619
_tmp4F0->r=_tmp9AC;});}
# 2625
goto _LLE4;}}else{_LLE7: _LLE8: {
# 2628
void*ta1=Cyc_Toc_typ_to_c(_tmp3F4);
({void*_tmp9B2=({void*_tmp9B1=Cyc_Absyn_cstar_type(ta1,_tmp3F3);Cyc_Toc_cast_it_r(_tmp9B1,({
struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp9B0,({struct Cyc_Absyn_Exp*_tmp3ED[3U];({
struct Cyc_Absyn_Exp*_tmp9AF=Cyc_Absyn_new_exp(_tmp4F0->r,0U);_tmp3ED[0]=_tmp9AF;}),({
struct Cyc_Absyn_Exp*_tmp9AE=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp3ED[1]=_tmp9AE;}),({
struct Cyc_Absyn_Exp*_tmp9AD=Cyc_Absyn_uint_exp(0U,0U);_tmp3ED[2]=_tmp9AD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3ED,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 2629
_tmp4F0->r=_tmp9B2;});
# 2636
goto _LLE4;}}_LLE4:;}
# 2638
if(_tmp4EE  && _tmp4ED)
({void*_tmp9B7=({struct Cyc_Absyn_Exp*_tmp9B6=_tmp4F0;void*_tmp9B5=Cyc_Toc_typ_to_c(e1typ);void*_tmp9B4=_tmp3F4;struct _dyneither_ptr*_tmp9B3=_tmp4EF;Cyc_Toc_check_tagged_union(_tmp9B6,_tmp9B5,_tmp9B4,_tmp9B3,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9B7;});
# 2641
if(is_poly  && _tmp4ED)
({void*_tmp9B9=({void*_tmp9B8=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9B8,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9B9;});
Cyc_Toc_set_lhs(nv,_tmp3DB);
goto _LL0;};};}case 23U: _LL35: _tmp4F2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4F1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL36: {
# 2646
int _tmp3F5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp3F6=Cyc_Tcutil_compress((void*)_check_null(_tmp4F2->topt));
# 2650
{void*_tmp3F7=_tmp3F6;void*_tmp424;struct Cyc_Absyn_Tqual _tmp423;void*_tmp422;void*_tmp421;void*_tmp420;void*_tmp41F;struct Cyc_List_List*_tmp41E;switch(*((int*)_tmp3F7)){case 6U: _LLED: _tmp41E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F7)->f1;_LLEE:
# 2653
 Cyc_Toc_exp_to_c(nv,_tmp4F2);
Cyc_Toc_exp_to_c(nv,_tmp4F1);{
struct _tuple13 _tmp3F8=Cyc_Evexp_eval_const_uint_exp(_tmp4F1);struct _tuple13 _tmp3F9=_tmp3F8;unsigned int _tmp3FD;int _tmp3FC;_LLF4: _tmp3FD=_tmp3F9.f1;_tmp3FC=_tmp3F9.f2;_LLF5:;
if(!_tmp3FC)
({void*_tmp3FA=0U;({struct _dyneither_ptr _tmp9BA=({const char*_tmp3FB="unknown tuple subscript in translation to C";_tag_dyneither(_tmp3FB,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp9BA,_tag_dyneither(_tmp3FA,sizeof(void*),0U));});});
({void*_tmp9BC=({struct Cyc_Absyn_Exp*_tmp9BB=_tmp4F2;Cyc_Toc_aggrmember_exp_r(_tmp9BB,Cyc_Absyn_fieldname((int)(_tmp3FD + 1)));});e->r=_tmp9BC;});
goto _LLEC;};case 3U: _LLEF: _tmp424=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F7)->f1).elt_type;_tmp423=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F7)->f1).elt_tq;_tmp422=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F7)->f1).ptr_atts).rgn;_tmp421=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F7)->f1).ptr_atts).nullable;_tmp420=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F7)->f1).ptr_atts).bounds;_tmp41F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F7)->f1).ptr_atts).zero_term;_LLF0: {
# 2661
int _tmp3FE=Cyc_Toc_do_null_check(e);
int _tmp3FF=Cyc_Toc_in_sizeof(nv);
int in_bnds=_tmp3FF;
{struct Cyc_Absyn_Exp*_tmp400=({void*_tmp9BD=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp9BD,_tmp420);});struct Cyc_Absyn_Exp*_tmp401=_tmp400;_LLF7: _LLF8:
# 2666
({int _tmp9BE=in_bnds  || !Cyc_Toc_do_bounds_check(e);in_bnds=_tmp9BE;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp404=({struct Cyc_String_pa_PrintArg_struct _tmp792;_tmp792.tag=0U,({struct _dyneither_ptr _tmp9BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp792.f1=_tmp9BF;});_tmp792;});void*_tmp402[1U];_tmp402[0]=& _tmp404;({unsigned int _tmp9C1=e->loc;struct _dyneither_ptr _tmp9C0=({const char*_tmp403="bounds check necessary for %s";_tag_dyneither(_tmp403,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp9C1,_tmp9C0,_tag_dyneither(_tmp402,sizeof(void*),1U));});});_LLF6:;}
# 2673
Cyc_Toc_exp_to_c(nv,_tmp4F2);
Cyc_Toc_exp_to_c(nv,_tmp4F1);
++ Cyc_Toc_total_bounds_checks;
{struct Cyc_Absyn_Exp*_tmp405=({void*_tmp9C2=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp9C2,_tmp420);});struct Cyc_Absyn_Exp*_tmp406=_tmp405;struct Cyc_Absyn_Exp*_tmp41B;if(_tmp406 != 0){_LLFA: _tmp41B=_tmp406;_LLFB: {
# 2678
struct Cyc_Absyn_Exp*_tmp407=(struct Cyc_Absyn_Exp*)_check_null(_tmp41B);
int possibly_null=Cyc_Tcutil_force_type2bool(0,_tmp421) && _tmp3FE;
void*ta1=Cyc_Toc_typ_to_c(_tmp424);
void*ta2=Cyc_Absyn_cstar_type(ta1,_tmp423);
struct _tuple13 _tmp408=Cyc_Evexp_eval_const_uint_exp(_tmp407);struct _tuple13 _tmp409=_tmp408;unsigned int _tmp419;int _tmp418;_LLFF: _tmp419=_tmp409.f1;_tmp418=_tmp409.f2;_LL100:;
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp40A=0U;({unsigned int _tmp9C4=e->loc;struct _dyneither_ptr _tmp9C3=({const char*_tmp40B="inserted null check due to dereference";_tag_dyneither(_tmp40B,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9C4,_tmp9C3,_tag_dyneither(_tmp40A,sizeof(void*),0U));});});
({void*_tmp9C8=({void*_tmp9C7=ta2;Cyc_Toc_cast_it_r(_tmp9C7,({struct Cyc_Absyn_Exp*_tmp9C6=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9C6,({struct Cyc_Absyn_Exp*_tmp40C[1U];({struct Cyc_Absyn_Exp*_tmp9C5=
Cyc_Absyn_copy_exp(_tmp4F2);_tmp40C[0]=_tmp9C5;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp40C,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2689
_tmp4F2->r=_tmp9C8;});}else{
# 2693
if((_tmp418  && _tmp419 == 1) && Cyc_Tcutil_force_type2bool(0,_tmp41F)){
# 2695
if(!Cyc_Evexp_c_can_eval(_tmp407))
({void*_tmp40D=0U;({unsigned int _tmp9CA=e->loc;struct _dyneither_ptr _tmp9C9=({const char*_tmp40E="cannot determine subscript is in bounds";_tag_dyneither(_tmp40E,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9CA,_tmp9C9,_tag_dyneither(_tmp40D,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4F2);
({void*_tmp9CD=Cyc_Toc_deref_exp_r(({void*_tmp9CC=ta2;Cyc_Toc_cast_it(_tmp9CC,({
struct Cyc_Absyn_Exp*_tmp9CB=function_e;Cyc_Absyn_fncall_exp(_tmp9CB,({struct Cyc_Absyn_Exp*_tmp40F[3U];_tmp40F[0]=_tmp4F2,_tmp40F[1]=_tmp407,_tmp40F[2]=_tmp4F1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp40F,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2698
e->r=_tmp9CD;});};}else{
# 2701
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp407))
({void*_tmp410=0U;({unsigned int _tmp9CF=e->loc;struct _dyneither_ptr _tmp9CE=({const char*_tmp411="cannot determine subscript is in bounds";_tag_dyneither(_tmp411,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9CF,_tmp9CE,_tag_dyneither(_tmp410,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp412=0U;({unsigned int _tmp9D1=e->loc;struct _dyneither_ptr _tmp9D0=({const char*_tmp413="inserted null check due to dereference";_tag_dyneither(_tmp413,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9D1,_tmp9D0,_tag_dyneither(_tmp412,sizeof(void*),0U));});});
# 2707
({void*_tmp9D5=Cyc_Toc_deref_exp_r(({void*_tmp9D4=ta2;Cyc_Toc_cast_it(_tmp9D4,({
struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9D3,({struct Cyc_Absyn_Exp*_tmp414[4U];_tmp414[0]=_tmp4F2,_tmp414[1]=_tmp407,({
# 2710
struct Cyc_Absyn_Exp*_tmp9D2=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp414[2]=_tmp9D2;}),_tmp414[3]=_tmp4F1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp414,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 2707
e->r=_tmp9D5;});}else{
# 2713
if(!Cyc_Evexp_c_can_eval(_tmp407))
({void*_tmp415=0U;({unsigned int _tmp9D7=e->loc;struct _dyneither_ptr _tmp9D6=({const char*_tmp416="cannot determine subscript is in bounds";_tag_dyneither(_tmp416,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9D7,_tmp9D6,_tag_dyneither(_tmp415,sizeof(void*),0U));});});
# 2716
({void*_tmp9DA=({struct Cyc_Absyn_Exp*_tmp9D9=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmp9D9,({struct Cyc_Absyn_Exp*_tmp417[2U];_tmp417[0]=_tmp407,({
struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Absyn_copy_exp(_tmp4F1);_tmp417[1]=_tmp9D8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp417,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2716
_tmp4F1->r=_tmp9DA;});}}}}
# 2719
goto _LLF9;}}else{_LLFC: _LLFD: {
# 2721
void*ta1=Cyc_Toc_typ_to_c(_tmp424);
if(in_bnds){
# 2725
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmp9DD=({struct Cyc_Absyn_Exp*_tmp9DC=({void*_tmp9DB=Cyc_Absyn_cstar_type(ta1,_tmp423);Cyc_Toc_cast_it(_tmp9DB,
Cyc_Toc_member_exp(_tmp4F2,Cyc_Toc_curr_sp,0U));});
# 2726
Cyc_Toc_subscript_exp_r(_tmp9DC,_tmp4F1);});e->r=_tmp9DD;});}else{
# 2730
({void*_tmp9E1=Cyc_Toc_deref_exp_r(({void*_tmp9E0=Cyc_Absyn_cstar_type(ta1,_tmp423);Cyc_Toc_cast_it(_tmp9E0,({
struct Cyc_Absyn_Exp*_tmp9DF=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp9DF,({struct Cyc_Absyn_Exp*_tmp41A[3U];_tmp41A[0]=_tmp4F2,({
struct Cyc_Absyn_Exp*_tmp9DE=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp41A[1]=_tmp9DE;}),_tmp41A[2]=_tmp4F1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp41A,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2730
e->r=_tmp9E1;});}
# 2735
goto _LLF9;}}_LLF9:;}
# 2737
goto _LLEC;}default: _LLF1: _LLF2:
({void*_tmp41C=0U;({struct _dyneither_ptr _tmp9E2=({const char*_tmp41D="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp41D,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E2,_tag_dyneither(_tmp41C,sizeof(void*),0U));});});}_LLEC:;}
# 2740
Cyc_Toc_set_lhs(nv,_tmp3F5);
goto _LL0;};}case 24U: _LL37: _tmp4F3=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL38:
# 2743
 if(!Cyc_Toc_is_toplevel(nv)){
# 2745
void*_tmp425=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4F3));
{void*_tmp426=_tmp425;union Cyc_Absyn_AggrInfo _tmp42B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp426)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp426)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp426)->f2 == 0){_LL102: _tmp42B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp426)->f1)->f1;_LL103: {
# 2748
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp42B);
{int i=1;for(0;_tmp4F3 != 0;(_tmp4F3=_tmp4F3->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F3->hd);
({struct Cyc_List_List*_tmp9E5=({struct Cyc_List_List*_tmp427=_cycalloc(sizeof(*_tmp427));({struct _tuple20*_tmp9E4=({struct _dyneither_ptr*_tmp9E3=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9E3,(struct Cyc_Absyn_Exp*)_tmp4F3->hd);});_tmp427->hd=_tmp9E4;}),_tmp427->tl=dles;_tmp427;});dles=_tmp9E5;});}}
# 2754
({void*_tmp9E7=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->tag=29U,_tmp428->f1=sd->name,_tmp428->f2=0,({struct Cyc_List_List*_tmp9E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp428->f3=_tmp9E6;}),_tmp428->f4=sd;_tmp428;});e->r=_tmp9E7;});
e->topt=_tmp425;
goto _LL101;}}else{goto _LL104;}}else{goto _LL104;}}else{_LL104: _LL105:
({void*_tmp429=0U;({struct _dyneither_ptr _tmp9E8=({const char*_tmp42A="tuple type not an aggregate";_tag_dyneither(_tmp42A,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E8,_tag_dyneither(_tmp429,sizeof(void*),0U));});});}_LL101:;}
# 2759
goto _LL0;}else{
# 2763
struct Cyc_List_List*dles=0;
for(0;_tmp4F3 != 0;_tmp4F3=_tmp4F3->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F3->hd);
({struct Cyc_List_List*_tmp9EA=({struct Cyc_List_List*_tmp42D=_cycalloc(sizeof(*_tmp42D));({struct _tuple20*_tmp9E9=({struct _tuple20*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->f1=0,_tmp42C->f2=(struct Cyc_Absyn_Exp*)_tmp4F3->hd;_tmp42C;});_tmp42D->hd=_tmp9E9;}),_tmp42D->tl=dles;_tmp42D;});dles=_tmp9EA;});}
# 2768
({void*_tmp9EB=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9EB;});}
# 2770
goto _LL0;case 26U: _LL39: _tmp4F4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL3A:
# 2776
{struct Cyc_List_List*_tmp42E=_tmp4F4;for(0;_tmp42E != 0;_tmp42E=_tmp42E->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp42E->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9EC=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F4);e->r=_tmp9EC;});
goto _LL0;case 27U: _LL3B: _tmp4F8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4F7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp4F6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp4F5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL3C:
# 2784
 Cyc_Toc_exp_to_c(nv,_tmp4F7);
Cyc_Toc_exp_to_c(nv,_tmp4F6);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp42F=Cyc_Evexp_eval_const_uint_exp(_tmp4F7);struct _tuple13 _tmp430=_tmp42F;unsigned int _tmp43A;int _tmp439;_LL107: _tmp43A=_tmp430.f1;_tmp439=_tmp430.f2;_LL108:;{
void*_tmp431=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4F6->topt));
struct Cyc_List_List*es=0;
# 2791
if(!Cyc_Toc_is_zero(_tmp4F6)){
if(!_tmp439)
({void*_tmp432=0U;({unsigned int _tmp9EE=_tmp4F7->loc;struct _dyneither_ptr _tmp9ED=({const char*_tmp433="cannot determine value of constant";_tag_dyneither(_tmp433,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9EE,_tmp9ED,_tag_dyneither(_tmp432,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp43A;++ i){
({struct Cyc_List_List*_tmp9F0=({struct Cyc_List_List*_tmp435=_cycalloc(sizeof(*_tmp435));({struct _tuple20*_tmp9EF=({struct _tuple20*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->f1=0,_tmp434->f2=_tmp4F6;_tmp434;});_tmp435->hd=_tmp9EF;}),_tmp435->tl=es;_tmp435;});es=_tmp9F0;});}}
# 2797
if(_tmp4F5){
struct Cyc_Absyn_Exp*_tmp436=({void*_tmp9F1=_tmp431;Cyc_Toc_cast_it(_tmp9F1,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp9F4=({struct Cyc_List_List*_tmp9F3=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F3,({struct Cyc_List_List*_tmp438=_cycalloc(sizeof(*_tmp438));({struct _tuple20*_tmp9F2=({struct _tuple20*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->f1=0,_tmp437->f2=_tmp436;_tmp437;});_tmp438->hd=_tmp9F2;}),_tmp438->tl=0;_tmp438;}));});es=_tmp9F4;});}}
# 2802
({void*_tmp9F5=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9F5;});};}
# 2804
goto _LL0;case 28U: _LL3D: _tmp4FB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4FA=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp4F9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_LL3E:
# 2811
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9F6=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9F6;});else{
# 2814
Cyc_Toc_exp_to_c(nv,_tmp4FB);}
goto _LL0;case 30U: _LL3F: _tmp4FD=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp4FC=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL40:
# 2818
{struct Cyc_List_List*_tmp43B=_tmp4FC;for(0;_tmp43B != 0;_tmp43B=_tmp43B->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp43B->hd)).f2);}}{
void*_tmp43C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp43C;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp43D=Cyc_Tcutil_compress(_tmp43C);void*_tmp43E=_tmp43D;union Cyc_Absyn_AggrInfo _tmp442;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43E)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43E)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43E)->f2 == 0){_LL10A: _tmp442=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43E)->f1)->f1;_LL10B: {
# 2825
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp442);
({void*_tmp9F7=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=29U,_tmp43F->f1=sd->name,_tmp43F->f2=0,_tmp43F->f3=_tmp4FC,_tmp43F->f4=sd;_tmp43F;});e->r=_tmp9F7;});
e->topt=_tmp43C;
goto _LL109;}}else{goto _LL10C;}}else{goto _LL10C;}}else{_LL10C: _LL10D:
({void*_tmp440=0U;({struct _dyneither_ptr _tmp9F8=({const char*_tmp441="anonStruct type not an aggregate";_tag_dyneither(_tmp441,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F8,_tag_dyneither(_tmp440,sizeof(void*),0U));});});}_LL109:;}else{
# 2832
({void*_tmp9F9=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4FC);e->r=_tmp9F9;});}
goto _LL0;};case 29U: _LL41: _tmp501=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp500=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp4FF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp4FE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL42:
# 2838
 if(_tmp4FE == 0  || _tmp4FE->impl == 0)
({void*_tmp443=0U;({struct _dyneither_ptr _tmp9FA=({const char*_tmp444="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp444,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FA,_tag_dyneither(_tmp443,sizeof(void*),0U));});});{
void*_tmp445=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp446=Cyc_Tcutil_compress(_tmp445);void*_tmp447=_tmp446;union Cyc_Absyn_AggrInfo _tmp44A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f1)->tag == 20U){_LL10F: _tmp44A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f1)->f1;_LL110:
({struct _tuple1*_tmp9FB=(Cyc_Absyn_aggr_kinded_name(_tmp44A)).f2;*_tmp501=_tmp9FB;});goto _LL10E;}else{goto _LL111;}}else{_LL111: _LL112:
({void*_tmp448=0U;({struct _dyneither_ptr _tmp9FC=({const char*_tmp449="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp449,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FC,_tag_dyneither(_tmp448,sizeof(void*),0U));});});}_LL10E:;}{
# 2849
struct Cyc_List_List*_tmp44B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FE->impl))->fields;
if(_tmp44B == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp44B))->tl != 0;_tmp44B=_tmp44B->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp44C=(struct Cyc_Absyn_Aggrfield*)_tmp44B->hd;
struct Cyc_List_List*_tmp44D=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp4FF,_tmp4FE->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FE->impl))->fields);
# 2856
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp44E=Cyc_Tcutil_compress(old_typ);void*_tmp44F=_tmp44E;struct Cyc_List_List*_tmp47B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f1)->tag == 20U){_LL114: _tmp47B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f2;_LL115:
# 2860
{struct Cyc_List_List*_tmp450=_tmp44D;for(0;_tmp450 != 0;_tmp450=_tmp450->tl){
struct _tuple35*_tmp451=(struct _tuple35*)_tmp450->hd;struct _tuple35*_tmp452=_tmp451;struct Cyc_Absyn_Aggrfield*_tmp478;struct Cyc_Absyn_Exp*_tmp477;_LL119: _tmp478=_tmp452->f1;_tmp477=_tmp452->f2;_LL11A:;{
void*_tmp453=_tmp477->topt;
Cyc_Toc_exp_to_c(nv,_tmp477);
# 2865
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp478->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp477->topt)))
({void*_tmp9FE=({void*_tmp9FD=Cyc_Toc_typ_to_c(_tmp478->type);Cyc_Toc_cast_it(_tmp9FD,
Cyc_Absyn_copy_exp(_tmp477));})->r;
# 2867
_tmp477->r=_tmp9FE;});
# 2870
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FE->impl))->tagged){
struct _dyneither_ptr*_tmp454=_tmp478->name;
struct Cyc_Absyn_Exp*_tmp455=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp4FE,_tmp454),0U);
struct _tuple20*_tmp456=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp455);
struct _tuple20*_tmp457=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp477);
struct _tuple1*s=({struct _tuple1*_tmp462=_cycalloc(sizeof(*_tmp462));({union Cyc_Absyn_Nmspace _tmpA02=Cyc_Absyn_Abs_n(0,1);_tmp462->f1=_tmpA02;}),({
struct _dyneither_ptr*_tmpA01=({struct _dyneither_ptr*_tmp461=_cycalloc(sizeof(*_tmp461));({struct _dyneither_ptr _tmpA00=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp45F=({struct Cyc_String_pa_PrintArg_struct _tmp794;_tmp794.tag=0U,_tmp794.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4FE->name).f2);_tmp794;});struct Cyc_String_pa_PrintArg_struct _tmp460=({struct Cyc_String_pa_PrintArg_struct _tmp793;_tmp793.tag=0U,_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp454);_tmp793;});void*_tmp45D[2U];_tmp45D[0]=& _tmp45F,_tmp45D[1]=& _tmp460;({struct _dyneither_ptr _tmp9FF=({const char*_tmp45E="_union_%s_%s";_tag_dyneither(_tmp45E,sizeof(char),13U);});Cyc_aprintf(_tmp9FF,_tag_dyneither(_tmp45D,sizeof(void*),2U));});});*_tmp461=_tmpA00;});_tmp461;});_tmp462->f2=_tmpA01;});_tmp462;});
# 2878
struct _tuple20*_tmp458=({
struct _dyneither_ptr*_tmpA04=_tmp454;Cyc_Toc_make_field(_tmpA04,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=29U,_tmp45C->f1=s,_tmp45C->f2=0,({struct Cyc_List_List*_tmpA03=({struct _tuple20*_tmp45B[2U];_tmp45B[0]=_tmp456,_tmp45B[1]=_tmp457;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp45B,sizeof(struct _tuple20*),2U));});_tmp45C->f3=_tmpA03;}),_tmp45C->f4=0;_tmp45C;}),0U));});
# 2881
({void*_tmpA06=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->tag=29U,_tmp45A->f1=*_tmp501,_tmp45A->f2=0,({struct Cyc_List_List*_tmpA05=({struct _tuple20*_tmp459[1U];_tmp459[0]=_tmp458;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp459,sizeof(struct _tuple20*),1U));});_tmp45A->f3=_tmpA05;}),_tmp45A->f4=_tmp4FE;_tmp45A;});e->r=_tmpA06;});}
# 2887
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp44C == _tmp478){
struct Cyc_List_List*_tmp463=({struct Cyc_List_List*_tmpA07=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FE->impl))->exist_vars,_tmp500);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA07,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp4FE->tvs,_tmp47B));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp464=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FE->impl))->fields;for(0;_tmp464 != 0;_tmp464=_tmp464->tl){
struct Cyc_Absyn_Aggrfield*_tmp465=(struct Cyc_Absyn_Aggrfield*)_tmp464->hd;
void*_tmp466=Cyc_Tcutil_substitute(_tmp463,_tmp465->type);
struct Cyc_Absyn_Aggrfield*_tmp467=({struct Cyc_Absyn_Aggrfield*_tmpA08=_tmp465;Cyc_Toc_aggrfield_to_c(_tmpA08,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp463,_tmp466)));});
({struct Cyc_List_List*_tmpA09=({struct Cyc_List_List*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->hd=_tmp467,_tmp468->tl=new_fields;_tmp468;});new_fields=_tmpA09;});
# 2902
if(_tmp464->tl == 0){
{void*_tmp469=Cyc_Tcutil_compress(_tmp467->type);void*_tmp46A=_tmp469;struct Cyc_Absyn_ArrayInfo _tmp46F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46A)->tag == 4U){_LL11C: _tmp46F=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46A)->f1;_LL11D:
# 2905
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp46F.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp46B=_tmp46F;
({struct Cyc_Absyn_Exp*_tmpA0A=Cyc_Absyn_uint_exp(0U,0U);_tmp46B.num_elts=_tmpA0A;});
({void*_tmpA0B=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->tag=4U,_tmp46C->f1=_tmp46B;_tmp46C;});_tmp467->type=_tmpA0B;});}
# 2910
goto _LL11B;}else{_LL11E: _LL11F:
# 2914
 if(_tmp477->topt == 0)
goto _LL11B;
{void*_tmp46D=Cyc_Tcutil_compress((void*)_check_null(_tmp477->topt));void*_tmp46E=_tmp46D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46E)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46E)->f1)->tag == 20U){_LL121: _LL122:
# 2918
 _tmp467->type=(void*)_check_null(_tmp477->topt);goto _LL120;}else{goto _LL123;}}else{_LL123: _LL124:
 goto _LL120;}_LL120:;}
# 2921
goto _LL11B;}_LL11B:;}
# 2925
if(!Cyc_Tcutil_is_array_type(_tmp465->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp465->type)))
({struct Cyc_List_List*_tmpA0D=({struct Cyc_List_List*_tmp471=_cycalloc(sizeof(*_tmp471));({void*_tmpA0C=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->tag=6U,_tmp470->f1=0;_tmp470;});_tmp471->hd=_tmpA0C;}),_tmp471->tl=_tmp467->attributes;_tmp471;});_tmp467->attributes=_tmpA0D;});}}}
# 2931
({struct Cyc_Absyn_Aggrdecl*_tmpA11=({struct _dyneither_ptr*_tmpA10=({struct _dyneither_ptr*_tmp475=_cycalloc(sizeof(*_tmp475));({struct _dyneither_ptr _tmpA0F=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp474=({struct Cyc_Int_pa_PrintArg_struct _tmp795;_tmp795.tag=1U,_tmp795.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp795;});void*_tmp472[1U];_tmp472[0]=& _tmp474;({struct _dyneither_ptr _tmpA0E=({const char*_tmp473="_genStruct%d";_tag_dyneither(_tmp473,sizeof(char),13U);});Cyc_aprintf(_tmpA0E,_tag_dyneither(_tmp472,sizeof(void*),1U));});});*_tmp475=_tmpA0F;});_tmp475;});Cyc_Toc_make_c_struct_defn(_tmpA10,
# 2933
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 2931
_tmp4FE=_tmpA11;});
# 2934
*_tmp501=_tmp4FE->name;
Cyc_Toc_aggrdecl_to_c(_tmp4FE,1);
# 2937
({void*_tmpA12=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp476=_cycalloc(sizeof(*_tmp476));*_tmp476=_tmp4FE;_tmp476;})),0);e->topt=_tmpA12;});}};}}
# 2940
goto _LL113;}else{goto _LL116;}}else{_LL116: _LL117:
({void*_tmp479=0U;({struct _dyneither_ptr _tmpA13=({const char*_tmp47A="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp47A,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA13,_tag_dyneither(_tmp479,sizeof(void*),0U));});});}_LL113:;}else{
# 2949
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FE->impl))->tagged){
# 2951
struct _tuple35*_tmp47C=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp44D))->hd;struct _tuple35*_tmp47D=_tmp47C;struct Cyc_Absyn_Aggrfield*_tmp486;struct Cyc_Absyn_Exp*_tmp485;_LL126: _tmp486=_tmp47D->f1;_tmp485=_tmp47D->f2;_LL127:;{
void*_tmp47E=(void*)_check_null(_tmp485->topt);
void*_tmp47F=_tmp486->type;
Cyc_Toc_exp_to_c(nv,_tmp485);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp47F) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp47E))
({void*_tmpA15=({
void*_tmpA14=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmpA14,Cyc_Absyn_new_exp(_tmp485->r,0U));});
# 2957
_tmp485->r=_tmpA15;});{
# 2960
int i=Cyc_Toc_get_member_offset(_tmp4FE,_tmp486->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp480=({struct _tuple20*_tmp482[2U];({struct _tuple20*_tmpA17=({struct _tuple20*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->f1=0,_tmp483->f2=field_tag_exp;_tmp483;});_tmp482[0]=_tmpA17;}),({struct _tuple20*_tmpA16=({struct _tuple20*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->f1=0,_tmp484->f2=_tmp485;_tmp484;});_tmp482[1]=_tmpA16;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp482,sizeof(struct _tuple20*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp480,0U);
({void*_tmpA19=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp481[1U];({struct _tuple20*_tmpA18=Cyc_Toc_make_field(_tmp486->name,umem);_tmp481[0]=_tmpA18;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp481,sizeof(struct _tuple20*),1U));}));e->r=_tmpA19;});};};}else{
# 2967
struct Cyc_List_List*_tmp487=0;
struct Cyc_List_List*_tmp488=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FE->impl))->fields;
for(0;_tmp488 != 0;_tmp488=_tmp488->tl){
struct Cyc_List_List*_tmp489=_tmp44D;for(0;_tmp489 != 0;_tmp489=_tmp489->tl){
if((*((struct _tuple35*)_tmp489->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp488->hd){
struct _tuple35*_tmp48A=(struct _tuple35*)_tmp489->hd;struct _tuple35*_tmp48B=_tmp48A;struct Cyc_Absyn_Aggrfield*_tmp491;struct Cyc_Absyn_Exp*_tmp490;_LL129: _tmp491=_tmp48B->f1;_tmp490=_tmp48B->f2;_LL12A:;{
void*_tmp48C=Cyc_Toc_typ_to_c(_tmp491->type);
void*_tmp48D=Cyc_Toc_typ_to_c((void*)_check_null(_tmp490->topt));
Cyc_Toc_exp_to_c(nv,_tmp490);
# 2977
if(!Cyc_Tcutil_unify(_tmp48C,_tmp48D)){
# 2979
if(!Cyc_Tcutil_is_arithmetic_type(_tmp48C) || !
Cyc_Tcutil_is_arithmetic_type(_tmp48D))
({struct Cyc_Absyn_Exp*_tmpA1B=({void*_tmpA1A=_tmp48C;Cyc_Toc_cast_it(_tmpA1A,Cyc_Absyn_copy_exp(_tmp490));});_tmp490=_tmpA1B;});}
({struct Cyc_List_List*_tmpA1D=({struct Cyc_List_List*_tmp48F=_cycalloc(sizeof(*_tmp48F));({struct _tuple20*_tmpA1C=({struct _tuple20*_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E->f1=0,_tmp48E->f2=_tmp490;_tmp48E;});_tmp48F->hd=_tmpA1C;}),_tmp48F->tl=_tmp487;_tmp48F;});_tmp487=_tmpA1D;});
break;};}}}
# 2986
({void*_tmpA1E=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp487));e->r=_tmpA1E;});}}
# 2989
goto _LL0;};};};case 31U: _LL43: _tmp504=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp503=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp502=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_LL44: {
# 2993
struct Cyc_List_List*_tmp492=_tmp502->typs;
{struct Cyc_List_List*_tmp493=_tmp504;for(0;_tmp493 != 0;(_tmp493=_tmp493->tl,_tmp492=_tmp492->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp493->hd;
void*_tmp494=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp492))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp494))
({void*_tmpA1F=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmpA1F;});}}{
# 3004
struct Cyc_Absyn_Exp*_tmp495=_tmp503->is_extensible?Cyc_Absyn_var_exp(_tmp502->name,0U):
 Cyc_Toc_datatype_tag(_tmp503,_tmp502->name);
# 3007
if(!Cyc_Toc_is_toplevel(nv)){
# 3009
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp504 != 0;(_tmp504=_tmp504->tl,++ i)){
({struct Cyc_List_List*_tmpA22=({struct Cyc_List_List*_tmp496=_cycalloc(sizeof(*_tmp496));({struct _tuple20*_tmpA21=({struct _dyneither_ptr*_tmpA20=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmpA20,(struct Cyc_Absyn_Exp*)_tmp504->hd);});_tmp496->hd=_tmpA21;}),_tmp496->tl=dles;_tmp496;});dles=_tmpA22;});}}{
# 3015
struct _tuple20*_tmp497=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp495);
({void*_tmpA26=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->tag=29U,({struct _tuple1*_tmpA25=Cyc_Toc_collapse_qvars(_tmp502->name,_tmp503->name);_tmp499->f1=_tmpA25;}),_tmp499->f2=0,({
struct Cyc_List_List*_tmpA24=({struct Cyc_List_List*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->hd=_tmp497,({struct Cyc_List_List*_tmpA23=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp498->tl=_tmpA23;});_tmp498;});_tmp499->f3=_tmpA24;}),_tmp499->f4=0;_tmp499;});
# 3016
e->r=_tmpA26;});};}else{
# 3021
struct Cyc_List_List*_tmp49A=0;
for(0;_tmp504 != 0;_tmp504=_tmp504->tl){
({struct Cyc_List_List*_tmpA28=({struct Cyc_List_List*_tmp49C=_cycalloc(sizeof(*_tmp49C));({struct _tuple20*_tmpA27=({struct _tuple20*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->f1=0,_tmp49B->f2=(struct Cyc_Absyn_Exp*)_tmp504->hd;_tmp49B;});_tmp49C->hd=_tmpA27;}),_tmp49C->tl=_tmp49A;_tmp49C;});_tmp49A=_tmpA28;});}
({void*_tmpA2B=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp49E=_cycalloc(sizeof(*_tmp49E));({struct _tuple20*_tmpA2A=({struct _tuple20*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->f1=0,_tmp49D->f2=_tmp495;_tmp49D;});_tmp49E->hd=_tmpA2A;}),({
struct Cyc_List_List*_tmpA29=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp49A);_tmp49E->tl=_tmpA29;});_tmp49E;}));
# 3024
e->r=_tmpA2B;});}
# 3027
goto _LL0;};}case 32U: _LL45: _LL46:
# 3029
 goto _LL48;case 33U: _LL47: _LL48:
 goto _LL0;case 34U: _LL49: _tmp50A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).is_calloc;_tmp509=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).rgn;_tmp508=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).elt_type;_tmp507=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).num_elts;_tmp506=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).fat_result;_tmp505=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).inline_call;_LL4A: {
# 3033
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp508)));
Cyc_Toc_exp_to_c(nv,_tmp507);
if(_tmp506){
struct _tuple1*_tmp49F=Cyc_Toc_temp_var();
struct _tuple1*_tmp4A0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp50A){
xexp=_tmp507;
if(_tmp509 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp509;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpA2E=({struct Cyc_Absyn_Exp*_tmpA2D=rgn;struct Cyc_Absyn_Exp*_tmpA2C=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA2D,_tmpA2C,Cyc_Absyn_var_exp(_tmp49F,0U));});pexp=_tmpA2E;});}else{
# 3046
({struct Cyc_Absyn_Exp*_tmpA31=({void*_tmpA30=*_tmp508;struct Cyc_Absyn_Exp*_tmpA2F=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA30,_tmpA2F,Cyc_Absyn_var_exp(_tmp49F,0U));});pexp=_tmpA31;});}
# 3048
({struct Cyc_Absyn_Exp*_tmpA36=({struct Cyc_Absyn_Exp*_tmpA35=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA35,({struct Cyc_Absyn_Exp*_tmp4A1[3U];({
struct Cyc_Absyn_Exp*_tmpA34=Cyc_Absyn_var_exp(_tmp4A0,0U);_tmp4A1[0]=_tmpA34;}),({struct Cyc_Absyn_Exp*_tmpA33=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp4A1[1]=_tmpA33;}),({
struct Cyc_Absyn_Exp*_tmpA32=Cyc_Absyn_var_exp(_tmp49F,0U);_tmp4A1[2]=_tmpA32;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A1,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3048
rexp=_tmpA36;});}else{
# 3052
if(_tmp509 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp509;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp505)
({struct Cyc_Absyn_Exp*_tmpA38=({struct Cyc_Absyn_Exp*_tmpA37=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpA37,Cyc_Absyn_var_exp(_tmp49F,0U));});pexp=_tmpA38;});else{
# 3058
({struct Cyc_Absyn_Exp*_tmpA3A=({struct Cyc_Absyn_Exp*_tmpA39=rgn;Cyc_Toc_rmalloc_exp(_tmpA39,Cyc_Absyn_var_exp(_tmp49F,0U));});pexp=_tmpA3A;});}}else{
# 3060
({struct Cyc_Absyn_Exp*_tmpA3C=({void*_tmpA3B=*_tmp508;Cyc_Toc_malloc_exp(_tmpA3B,Cyc_Absyn_var_exp(_tmp49F,0U));});pexp=_tmpA3C;});}
# 3062
({struct Cyc_Absyn_Exp*_tmpA41=({struct Cyc_Absyn_Exp*_tmpA40=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA40,({struct Cyc_Absyn_Exp*_tmp4A2[3U];({struct Cyc_Absyn_Exp*_tmpA3F=Cyc_Absyn_var_exp(_tmp4A0,0U);_tmp4A2[0]=_tmpA3F;}),({struct Cyc_Absyn_Exp*_tmpA3E=Cyc_Absyn_uint_exp(1U,0U);_tmp4A2[1]=_tmpA3E;}),({
struct Cyc_Absyn_Exp*_tmpA3D=Cyc_Absyn_var_exp(_tmp49F,0U);_tmp4A2[2]=_tmpA3D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A2,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3062
rexp=_tmpA41;});}{
# 3065
struct Cyc_Absyn_Stmt*_tmp4A3=({struct _tuple1*_tmpA47=_tmp49F;void*_tmpA46=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmpA45=_tmp507;Cyc_Absyn_declare_stmt(_tmpA47,_tmpA46,_tmpA45,({
struct _tuple1*_tmpA44=_tmp4A0;void*_tmpA43=Cyc_Absyn_cstar_type(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpA42=pexp;Cyc_Absyn_declare_stmt(_tmpA44,_tmpA43,_tmpA42,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpA48=Cyc_Toc_stmt_exp_r(_tmp4A3);e->r=_tmpA48;});};}else{
# 3070
if(_tmp50A){
if(_tmp509 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp509;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpA4B=({struct Cyc_Absyn_Exp*_tmpA4A=rgn;struct Cyc_Absyn_Exp*_tmpA49=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA4A,_tmpA49,_tmp507);})->r;e->r=_tmpA4B;});}else{
# 3076
({void*_tmpA4E=({void*_tmpA4D=*_tmp508;struct Cyc_Absyn_Exp*_tmpA4C=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA4D,_tmpA4C,_tmp507);})->r;e->r=_tmpA4E;});}}else{
# 3079
if(_tmp509 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp509;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp505)
({void*_tmpA4F=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp507))->r;e->r=_tmpA4F;});else{
# 3085
({void*_tmpA50=(Cyc_Toc_rmalloc_exp(rgn,_tmp507))->r;e->r=_tmpA50;});}}else{
# 3087
({void*_tmpA51=(Cyc_Toc_malloc_exp(*_tmp508,_tmp507))->r;e->r=_tmpA51;});}}}
# 3091
goto _LL0;}case 35U: _LL4B: _tmp50C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp50B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL4C: {
# 3100
void*e1_old_typ=(void*)_check_null(_tmp50C->topt);
void*e2_old_typ=(void*)_check_null(_tmp50B->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp4A4=0U;({struct _dyneither_ptr _tmpA52=({const char*_tmp4A5="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp4A5,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA52,_tag_dyneither(_tmp4A4,sizeof(void*),0U));});});{
# 3108
unsigned int _tmp4A6=e->loc;
struct _tuple1*_tmp4A7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4A8=Cyc_Absyn_var_exp(_tmp4A7,_tmp4A6);_tmp4A8->topt=e1_old_typ;{
struct _tuple1*_tmp4A9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4AA=Cyc_Absyn_var_exp(_tmp4A9,_tmp4A6);_tmp4AA->topt=e2_old_typ;{
# 3114
struct Cyc_Absyn_Exp*_tmp4AB=({struct Cyc_Absyn_Exp*_tmpA54=Cyc_Tcutil_deep_copy_exp(1,_tmp50C);struct Cyc_Absyn_Exp*_tmpA53=_tmp4AA;Cyc_Absyn_assign_exp(_tmpA54,_tmpA53,_tmp4A6);});_tmp4AB->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4AC=Cyc_Absyn_exp_stmt(_tmp4AB,_tmp4A6);
struct Cyc_Absyn_Exp*_tmp4AD=({struct Cyc_Absyn_Exp*_tmpA56=Cyc_Tcutil_deep_copy_exp(1,_tmp50B);struct Cyc_Absyn_Exp*_tmpA55=_tmp4A8;Cyc_Absyn_assign_exp(_tmpA56,_tmpA55,_tmp4A6);});_tmp4AD->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4AE=Cyc_Absyn_exp_stmt(_tmp4AD,_tmp4A6);
# 3119
struct Cyc_Absyn_Stmt*_tmp4AF=({struct _tuple1*_tmpA5E=_tmp4A7;void*_tmpA5D=e1_old_typ;struct Cyc_Absyn_Exp*_tmpA5C=_tmp50C;struct Cyc_Absyn_Stmt*_tmpA5B=({
struct _tuple1*_tmpA5A=_tmp4A9;void*_tmpA59=e2_old_typ;struct Cyc_Absyn_Exp*_tmpA58=_tmp50B;struct Cyc_Absyn_Stmt*_tmpA57=
Cyc_Absyn_seq_stmt(_tmp4AC,_tmp4AE,_tmp4A6);
# 3120
Cyc_Absyn_declare_stmt(_tmpA5A,_tmpA59,_tmpA58,_tmpA57,_tmp4A6);});
# 3119
Cyc_Absyn_declare_stmt(_tmpA5E,_tmpA5D,_tmpA5C,_tmpA5B,_tmp4A6);});
# 3122
Cyc_Toc_stmt_to_c(nv,_tmp4AF);
({void*_tmpA5F=Cyc_Toc_stmt_exp_r(_tmp4AF);e->r=_tmpA5F;});
goto _LL0;};};};};};}case 38U: _LL4D: _tmp50E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp50D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL4E: {
# 3127
void*_tmp4B0=Cyc_Tcutil_compress((void*)_check_null(_tmp50E->topt));
Cyc_Toc_exp_to_c(nv,_tmp50E);
{void*_tmp4B1=_tmp4B0;struct Cyc_Absyn_Aggrdecl*_tmp4B7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->f1)->f1).KnownAggr).tag == 2){_LL12C: _tmp4B7=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->f1)->f1).KnownAggr).val;_LL12D: {
# 3131
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp4B7,_tmp50D),0U);
struct Cyc_Absyn_Exp*_tmp4B3=Cyc_Toc_member_exp(_tmp50E,_tmp50D,0U);
struct Cyc_Absyn_Exp*_tmp4B4=Cyc_Toc_member_exp(_tmp4B3,Cyc_Toc_tag_sp,0U);
({void*_tmpA60=(Cyc_Absyn_eq_exp(_tmp4B4,_tmp4B2,0U))->r;e->r=_tmpA60;});
goto _LL12B;}}else{goto _LL12E;}}else{goto _LL12E;}}else{_LL12E: _LL12F:
({void*_tmp4B5=0U;({struct _dyneither_ptr _tmpA61=({const char*_tmp4B6="non-aggregate type in tagcheck";_tag_dyneither(_tmp4B6,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA61,_tag_dyneither(_tmp4B5,sizeof(void*),0U));});});}_LL12B:;}
# 3138
goto _LL0;}case 37U: _LL4F: _tmp50F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL50:
 Cyc_Toc_stmt_to_c(nv,_tmp50F);goto _LL0;case 36U: _LL51: _LL52:
({void*_tmp4B8=0U;({struct _dyneither_ptr _tmpA62=({const char*_tmp4B9="UnresolvedMem";_tag_dyneither(_tmp4B9,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA62,_tag_dyneither(_tmp4B8,sizeof(void*),0U));});});case 25U: _LL53: _LL54:
({void*_tmp4BA=0U;({struct _dyneither_ptr _tmpA63=({const char*_tmp4BB="compoundlit";_tag_dyneither(_tmp4BB,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpA63,_tag_dyneither(_tmp4BA,sizeof(void*),0U));});});case 39U: _LL55: _LL56:
({void*_tmp4BC=0U;({struct _dyneither_ptr _tmpA64=({const char*_tmp4BD="valueof(-)";_tag_dyneither(_tmp4BD,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA64,_tag_dyneither(_tmp4BC,sizeof(void*),0U));});});default: _LL57: _LL58:
({void*_tmp4BE=0U;({struct _dyneither_ptr _tmpA65=({const char*_tmp4BF="__asm__";_tag_dyneither(_tmp4BF,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA65,_tag_dyneither(_tmp4BE,sizeof(void*),0U));});});}_LL0:;};}struct _tuple36{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3171 "toc.cyc"
static struct _tuple36*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3173
return({struct _tuple36*_tmp510=_region_malloc(r,sizeof(*_tmp510));_tmp510->f1=0,({struct _dyneither_ptr*_tmpA67=Cyc_Toc_fresh_label();_tmp510->f2=_tmpA67;}),({struct _dyneither_ptr*_tmpA66=Cyc_Toc_fresh_label();_tmp510->f3=_tmpA66;}),_tmp510->f4=sc;_tmp510;});}
# 3178
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp511=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3184
if((int)(((_tmp511->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp514=_tmp511->orig_pat;if((_tmp514.pattern).tag != 1)_throw_match();(_tmp514.pattern).val;})->topt);
void*_tmp512=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp513=_tmp512;switch(*((int*)_tmp513)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp513)->f1)){case 0U: _LL1: _LL2:
# 3189
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpA69=({void*_tmpA68=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA68,v);});v=_tmpA69;});goto _LL0;}_LL0:;}{
# 3195
void*_tmp515=_tmp511->access;void*_tmp516=_tmp515;struct Cyc_Absyn_Datatypedecl*_tmp521;struct Cyc_Absyn_Datatypefield*_tmp520;unsigned int _tmp51F;int _tmp51E;struct _dyneither_ptr*_tmp51D;unsigned int _tmp51C;switch(*((int*)_tmp516)){case 0U: _LLA: _LLB:
# 3200
 goto _LL9;case 1U: _LLC: _LLD:
# 3205
 if(ps->tl != 0){
void*_tmp517=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp518=_tmp517;struct Cyc_Absyn_Datatypedecl*_tmp51B;struct Cyc_Absyn_Datatypefield*_tmp51A;unsigned int _tmp519;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp518)->tag == 3U){_LL15: _tmp51B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp518)->f1;_tmp51A=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp518)->f2;_tmp519=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp518)->f3;_LL16:
# 3208
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpA6C=({void*_tmpA6B=({void*_tmpA6A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp51A->name,_tmp51B->name));Cyc_Absyn_cstar_type(_tmpA6A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA6B,v);});v=_tmpA6C;});
({struct Cyc_Absyn_Exp*_tmpA6E=({struct Cyc_Absyn_Exp*_tmpA6D=v;Cyc_Absyn_aggrarrow_exp(_tmpA6D,Cyc_Absyn_fieldname((int)(_tmp519 + 1)),0U);});v=_tmpA6E;});
continue;}else{_LL17: _LL18:
# 3213
 goto _LL14;}_LL14:;}
# 3216
({struct Cyc_Absyn_Exp*_tmpA6F=Cyc_Absyn_deref_exp(v,0U);v=_tmpA6F;});
goto _LL9;case 2U: _LLE: _tmp51C=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp516)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpA71=({struct Cyc_Absyn_Exp*_tmpA70=v;Cyc_Toc_member_exp(_tmpA70,Cyc_Absyn_fieldname((int)(_tmp51C + 1)),0U);});v=_tmpA71;});goto _LL9;case 4U: _LL10: _tmp51E=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp516)->f1;_tmp51D=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp516)->f2;_LL11:
# 3220
({struct Cyc_Absyn_Exp*_tmpA72=Cyc_Toc_member_exp(v,_tmp51D,0U);v=_tmpA72;});
if(_tmp51E)
({struct Cyc_Absyn_Exp*_tmpA73=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpA73;});
goto _LL9;default: _LL12: _tmp521=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp516)->f1;_tmp520=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp516)->f2;_tmp51F=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp516)->f3;_LL13:
# 3225
({struct Cyc_Absyn_Exp*_tmpA75=({struct Cyc_Absyn_Exp*_tmpA74=v;Cyc_Toc_member_exp(_tmpA74,Cyc_Absyn_fieldname((int)(_tmp51F + 1)),0U);});v=_tmpA75;});
goto _LL9;}_LL9:;};}
# 3229
return v;}
# 3234
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp522=t;struct Cyc_Absyn_Datatypedecl*_tmp53B;struct Cyc_Absyn_Datatypefield*_tmp53A;struct _dyneither_ptr*_tmp539;int _tmp538;int _tmp537;struct Cyc_Absyn_Datatypedecl*_tmp536;struct Cyc_Absyn_Datatypefield*_tmp535;unsigned int _tmp534;struct _dyneither_ptr _tmp533;int _tmp532;void*_tmp531;struct Cyc_Absyn_Enumfield*_tmp530;struct Cyc_Absyn_Enumdecl*_tmp52F;struct Cyc_Absyn_Enumfield*_tmp52E;struct Cyc_Absyn_Exp*_tmp52D;switch(*((int*)_tmp522)){case 0U: _LL1: _tmp52D=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp522)->f1;_LL2:
# 3238
 if(_tmp52D == 0)return v;else{return _tmp52D;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA76=v;Cyc_Absyn_eq_exp(_tmpA76,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA77=v;Cyc_Absyn_neq_exp(_tmpA77,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp52F=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp522)->f1;_tmp52E=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp522)->f2;_LL8:
# 3242
 return({struct Cyc_Absyn_Exp*_tmpA78=v;Cyc_Absyn_eq_exp(_tmpA78,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->tag=32U,_tmp523->f1=_tmp52F,_tmp523->f2=_tmp52E;_tmp523;}),0U),0U);});case 4U: _LL9: _tmp531=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp522)->f1;_tmp530=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp522)->f2;_LLA:
# 3244
 return({struct Cyc_Absyn_Exp*_tmpA79=v;Cyc_Absyn_eq_exp(_tmpA79,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->tag=33U,_tmp524->f1=_tmp531,_tmp524->f2=_tmp530;_tmp524;}),0U),0U);});case 5U: _LLB: _tmp533=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp522)->f1;_tmp532=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp522)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpA7A=v;Cyc_Absyn_eq_exp(_tmpA7A,Cyc_Absyn_float_exp(_tmp533,_tmp532,0U),0U);});case 6U: _LLD: _tmp534=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp522)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpA7B=v;Cyc_Absyn_eq_exp(_tmpA7B,Cyc_Absyn_signed_int_exp((int)_tmp534,0U),0U);});case 7U: _LLF: _tmp537=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp522)->f1;_tmp536=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp522)->f2;_tmp535=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp522)->f3;_LL10:
# 3250
 LOOP1: {
void*_tmp525=v->r;void*_tmp526=_tmp525;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_Exp*_tmp527;switch(*((int*)_tmp526)){case 14U: _LL16: _tmp527=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp526)->f2;_LL17:
 v=_tmp527;goto LOOP1;case 20U: _LL18: _tmp528=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp526)->f1;_LL19:
 v=_tmp528;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3257
({struct Cyc_Absyn_Exp*_tmpA7E=({void*_tmpA7D=({void*_tmpA7C=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp535->name,_tmp536->name));Cyc_Absyn_cstar_type(_tmpA7C,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA7D,v);});v=_tmpA7E;});
return({struct Cyc_Absyn_Exp*_tmpA7F=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA7F,Cyc_Absyn_uint_exp((unsigned int)_tmp537,0U),0U);});case 8U: _LL11: _tmp539=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp522)->f1;_tmp538=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp522)->f2;_LL12:
# 3262
({struct Cyc_Absyn_Exp*_tmpA81=({struct Cyc_Absyn_Exp*_tmpA80=Cyc_Toc_member_exp(v,_tmp539,0U);Cyc_Toc_member_exp(_tmpA80,Cyc_Toc_tag_sp,0U);});v=_tmpA81;});
return({struct Cyc_Absyn_Exp*_tmpA82=v;Cyc_Absyn_eq_exp(_tmpA82,Cyc_Absyn_signed_int_exp(_tmp538,0U),0U);});default: _LL13: _tmp53B=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp522)->f1;_tmp53A=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp522)->f2;_LL14:
# 3266
 LOOP2: {
void*_tmp529=v->r;void*_tmp52A=_tmp529;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp52B;switch(*((int*)_tmp52A)){case 14U: _LL1D: _tmp52B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp52A)->f2;_LL1E:
 v=_tmp52B;goto LOOP2;case 20U: _LL1F: _tmp52C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp52A)->f1;_LL20:
 v=_tmp52C;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3273
({struct Cyc_Absyn_Exp*_tmpA85=({void*_tmpA84=({void*_tmpA83=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp53A->name,_tmp53B->name));Cyc_Absyn_cstar_type(_tmpA83,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA84,v);});v=_tmpA85;});
return({struct Cyc_Absyn_Exp*_tmpA86=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA86,Cyc_Absyn_var_exp(_tmp53A->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3287
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple37{int f1;void*f2;};
# 3295
static struct _tuple37 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3297
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp53C=*((struct _tuple33*)ss->hd);struct _tuple33 _tmp53D=_tmp53C;void*_tmp543;_LL1: _tmp543=_tmp53D.f1;_LL2:;{
void*_tmp53E=_tmp543;struct Cyc_Absyn_Exp*_tmp542;struct _dyneither_ptr*_tmp541;switch(*((int*)_tmp53E)){case 3U: _LL4: _LL5:
# 3303
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3306
 continue;case 8U: _LLA: _tmp541=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp53E)->f1;_LLB:
# 3308
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpA87=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->tag=3U,_tmp53F->f1=_tmp541;_tmp53F;});k=_tmpA87;});
continue;case 7U: _LLC: _LLD:
# 3312
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp542=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp53E)->f1;if(_tmp542 != 0){_LLF:
# 3316
({void*_tmpA88=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540->tag=2U,_tmp540->f1=_tmp542;_tmp540;});k=_tmpA88;});
return({struct _tuple37 _tmp796;_tmp796.f1=0,_tmp796.f2=k;_tmp796;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple37 _tmp797;_tmp797.f1=0,_tmp797.f2=k;_tmp797;});}_LL3:;};}
# 3325
return({struct _tuple37 _tmp798;_tmp798.f1=c,_tmp798.f2=k;_tmp798;});}
# 3330
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp544=p;int _tmp54F;int _tmp54E;unsigned int _tmp54D;void*_tmp54C;struct Cyc_Absyn_Enumfield*_tmp54B;struct Cyc_Absyn_Enumdecl*_tmp54A;struct Cyc_Absyn_Enumfield*_tmp549;switch(*((int*)_tmp544)){case 3U: _LL1: _tmp54A=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp544)->f1;_tmp549=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp544)->f2;_LL2:
# 3334
({struct Cyc_Absyn_Exp*_tmpA89=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->tag=32U,_tmp545->f1=_tmp54A,_tmp545->f2=_tmp549;_tmp545;}),0U);e=_tmpA89;});goto _LL0;case 4U: _LL3: _tmp54C=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp544)->f1;_tmp54B=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp544)->f2;_LL4:
# 3336
({struct Cyc_Absyn_Exp*_tmpA8A=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->tag=33U,_tmp546->f1=_tmp54C,_tmp546->f2=_tmp54B;_tmp546;}),0U);e=_tmpA8A;});goto _LL0;case 6U: _LL5: _tmp54D=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp544)->f1;_LL6:
 _tmp54E=(int)_tmp54D;goto _LL8;case 7U: _LL7: _tmp54E=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp544)->f1;_LL8:
 _tmp54F=_tmp54E;goto _LLA;case 8U: _LL9: _tmp54F=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp544)->f2;_LLA:
# 3340
({struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Absyn_uint_exp((unsigned int)_tmp54F,0U);e=_tmpA8B;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp547=0U;({struct _dyneither_ptr _tmpA8C=({const char*_tmp548="compile_pat_test_as_case!";_tag_dyneither(_tmp548,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA8C,_tag_dyneither(_tmp547,sizeof(void*),0U));});});}_LL0:;}
# 3344
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->tag=17U,_tmp550->f1=e;_tmp550;}),0U);}
# 3348
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple38{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3357
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3360
void*t=(void*)_check_null(p->topt);
void*_tmp551=p->r;void*_tmp552=_tmp551;union Cyc_Absyn_AggrInfo _tmp583;struct Cyc_List_List*_tmp582;struct Cyc_List_List*_tmp581;struct Cyc_List_List*_tmp580;struct Cyc_Absyn_Pat*_tmp57F;struct Cyc_Absyn_Datatypedecl*_tmp57E;struct Cyc_Absyn_Datatypefield*_tmp57D;struct Cyc_List_List*_tmp57C;struct Cyc_Absyn_Vardecl*_tmp57B;struct Cyc_Absyn_Pat*_tmp57A;struct Cyc_Absyn_Vardecl*_tmp579;struct Cyc_Absyn_Vardecl*_tmp578;struct Cyc_Absyn_Pat*_tmp577;struct Cyc_Absyn_Vardecl*_tmp576;switch(*((int*)_tmp552)){case 2U: _LL1: _tmp576=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp552)->f2;_LL2: {
# 3363
struct Cyc_Absyn_Pat*_tmp553=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp553->topt=p->topt;
_tmp578=_tmp576;_tmp577=_tmp553;goto _LL4;}case 1U: _LL3: _tmp578=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp552)->f1;_tmp577=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp552)->f2;_LL4:
# 3368
({struct Cyc_List_List*_tmpA8D=({struct Cyc_List_List*_tmp554=_region_malloc(rgn,sizeof(*_tmp554));_tmp554->hd=_tmp578,_tmp554->tl=*decls;_tmp554;});*decls=_tmpA8D;});{
struct Cyc_Absyn_Stmt*_tmp555=({struct Cyc_Absyn_Exp*_tmpA8E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->tag=4U,_tmp556->f1=_tmp578;_tmp556;}),0U);Cyc_Absyn_assign_stmt(_tmpA8E,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA8F=_tmp555;Cyc_Toc_seq_stmt_opt(_tmpA8F,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp577));});};case 4U: _LL5: _tmp579=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp552)->f2;_LL6:
# 3373
({struct Cyc_List_List*_tmpA90=({struct Cyc_List_List*_tmp557=_region_malloc(rgn,sizeof(*_tmp557));_tmp557->hd=_tmp579,_tmp557->tl=*decls;_tmp557;});*decls=_tmpA90;});
return({struct Cyc_Absyn_Exp*_tmpA91=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->tag=4U,_tmp558->f1=_tmp579;_tmp558;}),0U);Cyc_Absyn_assign_stmt(_tmpA91,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp57B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp552)->f1;_tmp57A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp552)->f2;_LLA:
# 3378
({struct Cyc_List_List*_tmpA92=({struct Cyc_List_List*_tmp559=_region_malloc(rgn,sizeof(*_tmp559));_tmp559->hd=_tmp57B,_tmp559->tl=*decls;_tmp559;});*decls=_tmpA92;});
({void*_tmpA93=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp57B->type=_tmpA93;});{
# 3381
struct Cyc_Absyn_Stmt*_tmp55A=({struct Cyc_Absyn_Exp*_tmpA96=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->tag=4U,_tmp55B->f1=_tmp57B;_tmp55B;}),0U);Cyc_Absyn_assign_stmt(_tmpA96,({
void*_tmpA95=({void*_tmpA94=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmpA94,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA95,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA97=_tmp55A;Cyc_Toc_seq_stmt_opt(_tmpA97,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp57A));});};case 9U: _LLB: _LLC:
# 3386
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp552)->f1)->r)->tag == 8U){_LL17: _tmp57E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp552)->f1)->r)->f1;_tmp57D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp552)->f1)->r)->f2;_tmp57C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp552)->f1)->r)->f3;_LL18:
# 3395
 if(_tmp57C == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp57D->name,_tmp57E->name);
void*_tmp55C=({void*_tmpA98=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmpA98,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpA99=Cyc_Toc_cast_it(_tmp55C,path);path=_tmpA99;});{
int cnt=1;
struct Cyc_List_List*_tmp55D=_tmp57D->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp57C != 0;(_tmp57C=_tmp57C->tl,_tmp55D=((struct Cyc_List_List*)_check_null(_tmp55D))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp55E=(struct Cyc_Absyn_Pat*)_tmp57C->hd;
if(_tmp55E->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp55F=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp55D))->hd)).f2;
void*_tmp560=(void*)_check_null(_tmp55E->topt);
void*_tmp561=Cyc_Toc_typ_to_c_array(_tmp560);
struct Cyc_Absyn_Exp*_tmp562=({struct Cyc_Absyn_Exp*_tmpA9A=path;Cyc_Absyn_aggrarrow_exp(_tmpA9A,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp55F)))
({struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Toc_cast_it(_tmp561,_tmp562);_tmp562=_tmpA9B;});
({struct Cyc_Absyn_Stmt*_tmpA9D=({struct Cyc_Absyn_Stmt*_tmpA9C=s;Cyc_Toc_seq_stmt_opt(_tmpA9C,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp562,_tmp55E));});s=_tmpA9D;});};}
# 3413
return s;};};}else{_LL21: _tmp57F=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp552)->f1;_LL22:
# 3458
 return({struct _RegionHandle*_tmpAA1=rgn;struct Cyc_Toc_Env**_tmpAA0=nv;struct Cyc_List_List**_tmpA9F=decls;struct Cyc_Absyn_Exp*_tmpA9E=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpAA1,_tmpAA0,_tmpA9F,_tmpA9E,_tmp57F);});}case 8U: _LL19: _tmp580=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp552)->f3;_LL1A:
# 3415
 _tmp581=_tmp580;goto _LL1C;case 5U: _LL1B: _tmp581=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp552)->f1;_LL1C: {
# 3417
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp581 != 0;(_tmp581=_tmp581->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp563=(struct Cyc_Absyn_Pat*)_tmp581->hd;
if(_tmp563->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp564=(void*)_check_null(_tmp563->topt);
struct _dyneither_ptr*_tmp565=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpAA7=({struct Cyc_Absyn_Stmt*_tmpAA6=s;Cyc_Toc_seq_stmt_opt(_tmpAA6,({struct _RegionHandle*_tmpAA5=rgn;struct Cyc_Toc_Env**_tmpAA4=nv;struct Cyc_List_List**_tmpAA3=decls;struct Cyc_Absyn_Exp*_tmpAA2=Cyc_Toc_member_exp(path,_tmp565,0U);Cyc_Toc_extract_pattern_vars(_tmpAA5,_tmpAA4,_tmpAA3,_tmpAA2,_tmp563);}));});s=_tmpAA7;});};}
# 3427
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp552)->f1 == 0){_LL1D: _LL1E:
({void*_tmp566=0U;({struct _dyneither_ptr _tmpAA8=({const char*_tmp567="unresolved aggregate pattern!";_tag_dyneither(_tmp567,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA8,_tag_dyneither(_tmp566,sizeof(void*),0U));});});}else{_LL1F: _tmp583=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp552)->f1;_tmp582=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp552)->f3;_LL20: {
# 3431
struct Cyc_Absyn_Aggrdecl*_tmp568=Cyc_Absyn_get_known_aggrdecl(_tmp583);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp582 != 0;_tmp582=_tmp582->tl){
struct _tuple38*_tmp569=(struct _tuple38*)_tmp582->hd;
struct Cyc_Absyn_Pat*_tmp56A=(*_tmp569).f2;
if(_tmp56A->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp56B=Cyc_Absyn_designatorlist_to_fieldname((*_tmp569).f1);
void*_tmp56C=(void*)_check_null(_tmp56A->topt);
void*_tmp56D=Cyc_Toc_typ_to_c_array(_tmp56C);
struct Cyc_Absyn_Exp*_tmp56E=Cyc_Toc_member_exp(path,_tmp56B,0U);
# 3443
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp568->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpAA9=Cyc_Toc_member_exp(_tmp56E,Cyc_Toc_val_sp,0U);_tmp56E=_tmpAA9;});{
void*_tmp56F=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp568->impl))->fields,_tmp56B)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp56F))
({struct Cyc_Absyn_Exp*_tmpAAA=Cyc_Toc_cast_it(_tmp56D,_tmp56E);_tmp56E=_tmpAAA;});else{
if(!Cyc_Tcutil_is_array_type(_tmp56F) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp56F)))
# 3450
({struct Cyc_Absyn_Exp*_tmpAAC=Cyc_Absyn_deref_exp(({void*_tmpAAB=Cyc_Absyn_cstar_type(_tmp56D,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpAAB,
Cyc_Absyn_address_exp(_tmp56E,0U));}),0U);
# 3450
_tmp56E=_tmpAAC;});}
# 3453
({struct Cyc_Absyn_Stmt*_tmpAAE=({struct Cyc_Absyn_Stmt*_tmpAAD=s;Cyc_Toc_seq_stmt_opt(_tmpAAD,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp56E,_tmp56A));});s=_tmpAAE;});};};}
# 3455
return s;}}case 15U: _LL23: _LL24:
# 3460
({void*_tmp570=0U;({struct _dyneither_ptr _tmpAAF=({const char*_tmp571="unknownid pat";_tag_dyneither(_tmp571,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAAF,_tag_dyneither(_tmp570,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp572=0U;({struct _dyneither_ptr _tmpAB0=({const char*_tmp573="unknowncall pat";_tag_dyneither(_tmp573,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB0,_tag_dyneither(_tmp572,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp574=0U;({struct _dyneither_ptr _tmpAB1=({const char*_tmp575="exp pat";_tag_dyneither(_tmp575,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB1,_tag_dyneither(_tmp574,sizeof(void*),0U));});});}_LL0:;}struct _tuple39{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3471
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3479
void*_tmp584=dopt;struct Cyc_List_List*_tmp5C2;struct Cyc_List_List*_tmp5C1;void*_tmp5C0;struct Cyc_Tcpat_Rhs*_tmp5BF;if(_tmp584 == 0){_LL1: _LL2:
# 3481
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp584)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp5BF=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp584)->f1;_LL6:
# 3486
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple36*_tmp585=(struct _tuple36*)lscs->hd;struct _tuple36*_tmp586=_tmp585;int*_tmp58D;struct _dyneither_ptr*_tmp58C;struct _dyneither_ptr*_tmp58B;struct Cyc_Absyn_Switch_clause*_tmp58A;_LLA: _tmp58D=(int*)& _tmp586->f1;_tmp58C=_tmp586->f2;_tmp58B=_tmp586->f3;_tmp58A=_tmp586->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp587=_tmp58A->body;
if(_tmp587 == _tmp5BF->rhs){
# 3491
if(*_tmp58D)
return Cyc_Absyn_goto_stmt(_tmp58C,0U);
*_tmp58D=1;{
# 3495
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAB4=rgn;struct Cyc_List_List**_tmpAB3=decls;struct Cyc_Absyn_Exp*_tmpAB2=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAB4,& nv,_tmpAB3,_tmpAB2,_tmp58A->pattern);});
# 3498
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp58B,_tmp58A->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAB5=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpAB5;});
({struct Cyc_Absyn_Stmt*_tmpAB6=Cyc_Absyn_label_stmt(_tmp58C,res,0U);res=_tmpAB6;});
({struct Cyc_List_List*_tmpAB8=({struct Cyc_List_List*_tmp589=_region_malloc(rgn,sizeof(*_tmp589));({struct _tuple39*_tmpAB7=({struct _tuple39*_tmp588=_region_malloc(rgn,sizeof(*_tmp588));_tmp588->f1=nv,_tmp588->f2=_tmp58B,_tmp588->f3=_tmp587;_tmp588;});_tmp589->hd=_tmpAB7;}),_tmp589->tl=*bodies;_tmp589;});*bodies=_tmpAB8;});
return res;};}};}
# 3506
({void*_tmp58E=0U;({struct _dyneither_ptr _tmpAB9=({const char*_tmp58F="couldn't find rhs!";_tag_dyneither(_tmp58F,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB9,_tag_dyneither(_tmp58E,sizeof(void*),0U));});});default: _LL7: _tmp5C2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp584)->f1;_tmp5C1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp584)->f2;_tmp5C0=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp584)->f3;_LL8: {
# 3509
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5C0,lscs,v);
# 3511
struct Cyc_Absyn_Exp*_tmp590=({struct Cyc_List_List*_tmpABA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5C2);Cyc_Toc_compile_path(_tmpABA,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp591=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5C1);
# 3514
struct _tuple37 _tmp592=Cyc_Toc_admits_switch(_tmp591);struct _tuple37 _tmp593=_tmp592;int _tmp5BE;void*_tmp5BD;_LLD: _tmp5BE=_tmp593.f1;_tmp5BD=_tmp593.f2;_LLE:;
if(_tmp5BE > 1){
# 3518
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));
({struct Cyc_Absyn_Switch_clause*_tmpABC=({struct Cyc_Absyn_Switch_clause*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));({struct Cyc_Absyn_Pat*_tmpABB=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5A3->pattern=_tmpABB;}),_tmp5A3->pat_vars=0,_tmp5A3->where_clause=0,_tmp5A3->body=res,_tmp5A3->loc=0U;_tmp5A3;});_tmp5A4->hd=_tmpABC;}),_tmp5A4->tl=0;_tmp5A4;});
# 3521
for(0;_tmp591 != 0;_tmp591=_tmp591->tl){
struct _tuple33 _tmp594=*((struct _tuple33*)_tmp591->hd);struct _tuple33 _tmp595=_tmp594;void*_tmp59B;void*_tmp59A;_LL10: _tmp59B=_tmp595.f1;_tmp59A=_tmp595.f2;_LL11:;{
# 3524
struct Cyc_Absyn_Pat*_tmp596=Cyc_Toc_compile_pat_test_as_case(_tmp59B);
# 3526
struct Cyc_Absyn_Stmt*_tmp597=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp59A,lscs,v);
# 3528
({struct Cyc_List_List*_tmpABE=({struct Cyc_List_List*_tmp599=_cycalloc(sizeof(*_tmp599));({struct Cyc_Absyn_Switch_clause*_tmpABD=({struct Cyc_Absyn_Switch_clause*_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->pattern=_tmp596,_tmp598->pat_vars=0,_tmp598->where_clause=0,_tmp598->body=_tmp597,_tmp598->loc=0U;_tmp598;});_tmp599->hd=_tmpABD;}),_tmp599->tl=new_lscs;_tmp599;});new_lscs=_tmpABE;});};}
# 3530
{void*_tmp59C=_tmp5BD;struct _dyneither_ptr*_tmp5A1;switch(*((int*)_tmp59C)){case 1U: _LL13: _LL14:
# 3533
 LOOP1: {
void*_tmp59D=_tmp590->r;void*_tmp59E=_tmp59D;struct Cyc_Absyn_Exp*_tmp5A0;struct Cyc_Absyn_Exp*_tmp59F;switch(*((int*)_tmp59E)){case 14U: _LL1C: _tmp59F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL1D:
 _tmp590=_tmp59F;goto LOOP1;case 20U: _LL1E: _tmp5A0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL1F:
 _tmp590=_tmp5A0;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3539
({struct Cyc_Absyn_Exp*_tmpAC0=Cyc_Absyn_deref_exp(({void*_tmpABF=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpABF,_tmp590);}),0U);_tmp590=_tmpAC0;});goto _LL12;case 3U: _LL15: _tmp5A1=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp59C)->f1;_LL16:
# 3542
({struct Cyc_Absyn_Exp*_tmpAC2=({struct Cyc_Absyn_Exp*_tmpAC1=Cyc_Toc_member_exp(_tmp590,_tmp5A1,0U);Cyc_Toc_member_exp(_tmpAC1,Cyc_Toc_tag_sp,0U);});_tmp590=_tmpAC2;});goto _LL12;case 2U: _LL17: _LL18:
# 3544
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3547
({struct Cyc_Absyn_Stmt*_tmpAC3=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->tag=10U,_tmp5A2->f1=_tmp590,_tmp5A2->f2=new_lscs,_tmp5A2->f3=0;_tmp5A2;}),0U);res=_tmpAC3;});}else{
# 3551
void*_tmp5A5=_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BC;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5A5)->tag == 2U){_LL23: _tmp5BC=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5A5)->f1;_LL24: {
# 3553
struct Cyc_List_List*_tmp5A6=_tmp591;void*_tmp5B5;struct Cyc_Tcpat_Rhs*_tmp5B4;if(_tmp5A6 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp5A6)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5A6)->tl == 0){_LL28: _tmp5B5=((struct _tuple33*)_tmp5A6->hd)->f1;_tmp5B4=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp5A6->hd)->f2)->f1;_LL29:
# 3559
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple36*_tmp5A7=(struct _tuple36*)lscs->hd;struct _tuple36*_tmp5A8=_tmp5A7;int*_tmp5AF;struct _dyneither_ptr*_tmp5AE;struct _dyneither_ptr*_tmp5AD;struct Cyc_Absyn_Switch_clause*_tmp5AC;_LL2D: _tmp5AF=(int*)& _tmp5A8->f1;_tmp5AE=_tmp5A8->f2;_tmp5AD=_tmp5A8->f3;_tmp5AC=_tmp5A8->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp5A9=_tmp5AC->body;
if(_tmp5A9 == _tmp5B4->rhs){
# 3564
if(*_tmp5AF)
return Cyc_Absyn_goto_stmt(_tmp5AE,0U);
*_tmp5AF=1;{
# 3568
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAC6=rgn;struct Cyc_List_List**_tmpAC5=decls;struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAC6,& nv,_tmpAC5,_tmpAC4,_tmp5AC->pattern);});
# 3571
Cyc_Toc_exp_to_c(nv,_tmp5BC);{
# 3574
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp5AD,_tmp5AC->body,0U);
({struct Cyc_Absyn_Stmt*_tmpAC7=Cyc_Absyn_ifthenelse_stmt(_tmp5BC,r,res,0U);r=_tmpAC7;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAC8=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpAC8;});
({struct Cyc_Absyn_Stmt*_tmpAC9=Cyc_Absyn_label_stmt(_tmp5AE,r,0U);r=_tmpAC9;});
({struct Cyc_List_List*_tmpACB=({struct Cyc_List_List*_tmp5AB=_region_malloc(rgn,sizeof(*_tmp5AB));({struct _tuple39*_tmpACA=({struct _tuple39*_tmp5AA=_region_malloc(rgn,sizeof(*_tmp5AA));_tmp5AA->f1=nv,_tmp5AA->f2=_tmp5AD,_tmp5AA->f3=_tmp5A9;_tmp5AA;});_tmp5AB->hd=_tmpACA;}),_tmp5AB->tl=*bodies;_tmp5AB;});*bodies=_tmpACB;});
return r;};};}};}
# 3583
({void*_tmp5B0=0U;({struct _dyneither_ptr _tmpACC=({const char*_tmp5B1="couldn't find rhs!";_tag_dyneither(_tmp5B1,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACC,_tag_dyneither(_tmp5B0,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp5B2=0U;({struct _dyneither_ptr _tmpACD=({const char*_tmp5B3="bad where clause in match compiler";_tag_dyneither(_tmp5B3,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACD,_tag_dyneither(_tmp5B2,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3588
 for(0;_tmp591 != 0;_tmp591=_tmp591->tl){
struct _tuple33 _tmp5B6=*((struct _tuple33*)_tmp591->hd);struct _tuple33 _tmp5B7=_tmp5B6;void*_tmp5BB;void*_tmp5BA;_LL30: _tmp5BB=_tmp5B7.f1;_tmp5BA=_tmp5B7.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp5B8=Cyc_Toc_compile_pat_test(_tmp590,_tmp5BB);
struct Cyc_Absyn_Stmt*_tmp5B9=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5BA,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpACE=Cyc_Absyn_ifthenelse_stmt(_tmp5B8,_tmp5B9,res,0U);res=_tmpACE;});};}}_LL22:;}
# 3596
return res;}}}_LL0:;}
# 3606
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3608
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple39*_tmp5C3=(struct _tuple39*)bodies->hd;struct _tuple39*_tmp5C4=_tmp5C3;struct Cyc_Toc_Env**_tmp5C6;struct Cyc_Absyn_Stmt*_tmp5C5;_LL1: _tmp5C6=(struct Cyc_Toc_Env**)& _tmp5C4->f1;_tmp5C5=_tmp5C4->f3;_LL2:;
if(_tmp5C5 == s)return _tmp5C6;}
# 3614
return 0;}
# 3618
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3621
void*_tmp5C7=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3624
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5C8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5C8;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5C9=Cyc_Toc_share_env(rgn,nv);
# 3631
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple36*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3636
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp5C9,& mydecls,& mybodies,dopt,lscs,v);
# 3644
{struct Cyc_List_List*_tmp5CA=lscs;for(0;_tmp5CA != 0;_tmp5CA=_tmp5CA->tl){
struct _tuple36*_tmp5CB=(struct _tuple36*)_tmp5CA->hd;struct _tuple36*_tmp5CC=_tmp5CB;struct _dyneither_ptr*_tmp5D4;struct Cyc_Absyn_Switch_clause*_tmp5D3;_LL1: _tmp5D4=_tmp5CC->f3;_tmp5D3=_tmp5CC->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp5D3->body;
# 3648
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3651
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5CD=*envp;
# 3654
if(_tmp5CA->tl != 0){
struct _tuple36*_tmp5CE=(struct _tuple36*)((struct Cyc_List_List*)_check_null(_tmp5CA->tl))->hd;struct _tuple36*_tmp5CF=_tmp5CE;struct _dyneither_ptr*_tmp5D2;struct Cyc_Absyn_Switch_clause*_tmp5D1;_LL4: _tmp5D2=_tmp5CF->f3;_tmp5D1=_tmp5CF->f4;_LL5:;{
# 3658
struct Cyc_Toc_Env**_tmp5D0=Cyc_Toc_find_case_env(mybodies,_tmp5D1->body);
# 3663
if(_tmp5D0 == 0)
({struct Cyc_Toc_Env*_tmpACF=Cyc_Toc_last_switchclause_env(rgn,_tmp5CD,end_l);Cyc_Toc_stmt_to_c(_tmpACF,s);});else{
# 3667
struct Cyc_List_List*vs=0;
if(_tmp5D1->pat_vars != 0){
({struct Cyc_List_List*_tmpAD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5D1->pat_vars))->v)).f1);vs=_tmpAD0;});
({struct Cyc_List_List*_tmpAD1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpAD1;});}
# 3672
({struct Cyc_Toc_Env*_tmpAD2=Cyc_Toc_non_last_switchclause_env(rgn,_tmp5CD,end_l,_tmp5D2,vs);Cyc_Toc_stmt_to_c(_tmpAD2,s);});}};}else{
# 3676
({struct Cyc_Toc_Env*_tmpAD3=Cyc_Toc_last_switchclause_env(rgn,_tmp5CD,end_l);Cyc_Toc_stmt_to_c(_tmpAD3,s);});}};};}}{
# 3680
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpAD5=test_tree;Cyc_Absyn_seq_stmt(_tmpAD5,({struct _dyneither_ptr*_tmpAD4=end_l;Cyc_Absyn_label_stmt(_tmpAD4,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3682
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp5D5=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpAD6=Cyc_Toc_temp_var();_tmp5D5->name=_tmpAD6;});
({void*_tmpAD7=Cyc_Toc_typ_to_c_array(_tmp5D5->type);_tmp5D5->type=_tmpAD7;});
({struct Cyc_Absyn_Stmt*_tmpAD9=({struct Cyc_Absyn_Decl*_tmpAD8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->tag=0U,_tmp5D6->f1=_tmp5D5;_tmp5D6;}),0U);Cyc_Absyn_decl_stmt(_tmpAD8,res,0U);});res=_tmpAD9;});}
# 3689
({void*_tmpADD=({struct _tuple1*_tmpADC=v;void*_tmpADB=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpADA=e;Cyc_Absyn_declare_stmt(_tmpADC,_tmpADB,_tmpADA,res,0U);})->r;whole_s->r=_tmpADD;});
_npop_handler(0U);return;};}
# 3628
;_pop_region(rgn);};}
# 3695
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3697
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3702
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3704
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpADF=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpADF,({struct Cyc_List_List*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));
({struct Cyc_Absyn_Exp*_tmpADE=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp5D7->hd=_tmpADE;}),_tmp5D7->tl=0;_tmp5D7;}),0U);}),0U);}
# 3708
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpAE1=({struct Cyc_Absyn_Stmt*_tmpAE0=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpAE0,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpAE1;});}
# 3713
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3715
while(1){
void*_tmp5D8=s->r;void*_tmp5D9=_tmp5D8;struct Cyc_Absyn_Stmt*_tmp630;struct Cyc_List_List*_tmp62F;void*_tmp62E;struct Cyc_Absyn_Stmt*_tmp62D;struct Cyc_Absyn_Exp*_tmp62C;struct _dyneither_ptr*_tmp62B;struct Cyc_Absyn_Stmt*_tmp62A;struct Cyc_Absyn_Decl*_tmp629;struct Cyc_Absyn_Stmt*_tmp628;struct Cyc_List_List*_tmp627;struct Cyc_Absyn_Switch_clause**_tmp626;struct Cyc_Absyn_Exp*_tmp625;struct Cyc_List_List*_tmp624;void*_tmp623;struct Cyc_Absyn_Exp*_tmp622;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_Absyn_Exp*_tmp620;struct Cyc_Absyn_Stmt*_tmp61F;struct Cyc_Absyn_Exp*_tmp61E;struct Cyc_Absyn_Stmt*_tmp61D;struct Cyc_Absyn_Exp*_tmp61C;struct Cyc_Absyn_Stmt*_tmp61B;struct Cyc_Absyn_Stmt*_tmp61A;struct Cyc_Absyn_Exp*_tmp619;struct Cyc_Absyn_Stmt*_tmp618;struct Cyc_Absyn_Stmt*_tmp617;struct Cyc_Absyn_Exp*_tmp616;switch(*((int*)_tmp5D9)){case 0U: _LL1: _LL2:
# 3718
 return;case 1U: _LL3: _tmp616=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_LL4:
# 3720
 Cyc_Toc_exp_to_c(nv,_tmp616);
return;case 2U: _LL5: _tmp618=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_tmp617=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2;_LL6:
# 3723
 Cyc_Toc_stmt_to_c(nv,_tmp618);
s=_tmp617;
continue;case 3U: _LL7: _tmp619=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_LL8: {
# 3727
void*topt=0;
if(_tmp619 != 0){
({void*_tmpAE2=Cyc_Toc_typ_to_c((void*)_check_null(_tmp619->topt));topt=_tmpAE2;});
Cyc_Toc_exp_to_c(nv,_tmp619);}{
# 3733
int _tmp5DA=Cyc_Toc_get_npop(s);
if(_tmp5DA > 0){
if(topt != 0){
struct _tuple1*_tmp5DB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp5DC=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp5DB,0U),0U);
({void*_tmpAE7=({struct _tuple1*_tmpAE6=_tmp5DB;void*_tmpAE5=topt;struct Cyc_Absyn_Exp*_tmpAE4=_tmp619;Cyc_Absyn_declare_stmt(_tmpAE6,_tmpAE5,_tmpAE4,({
struct Cyc_Absyn_Stmt*_tmpAE3=Cyc_Toc_make_npop_handler(_tmp5DA);Cyc_Absyn_seq_stmt(_tmpAE3,_tmp5DC,0U);}),0U);})->r;
# 3738
s->r=_tmpAE7;});}else{
# 3742
Cyc_Toc_do_npop_before(_tmp5DA,s);}}
# 3744
return;};}case 4U: _LL9: _tmp61C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_tmp61B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2;_tmp61A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f3;_LLA:
# 3746
 Cyc_Toc_exp_to_c(nv,_tmp61C);
Cyc_Toc_stmt_to_c(nv,_tmp61B);
s=_tmp61A;
continue;case 5U: _LLB: _tmp61E=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1).f1;_tmp61D=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2;_LLC:
# 3751
 Cyc_Toc_exp_to_c(nv,_tmp61E);{
struct _RegionHandle _tmp5DD=_new_region("temp");struct _RegionHandle*temp=& _tmp5DD;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAE8=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAE8,_tmp61D);});
# 3755
_npop_handler(0U);return;
# 3752
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3757
struct Cyc_Toc_Env*_tmp5DE=nv;struct _dyneither_ptr**_tmp5DF;_LL22: _tmp5DF=_tmp5DE->break_lab;_LL23:;
if(_tmp5DF != 0)
({void*_tmpAE9=Cyc_Toc_goto_stmt_r(*_tmp5DF);s->r=_tmpAE9;});
# 3761
({int _tmpAEA=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAEA,s);});
return;}case 7U: _LLF: _LL10: {
# 3764
struct Cyc_Toc_Env*_tmp5E0=nv;struct _dyneither_ptr**_tmp5E1;_LL25: _tmp5E1=_tmp5E0->continue_lab;_LL26:;
if(_tmp5E1 != 0)
({void*_tmpAEB=Cyc_Toc_goto_stmt_r(*_tmp5E1);s->r=_tmpAEB;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3770
({int _tmpAEC=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAEC,s);});
return;case 9U: _LL13: _tmp622=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_tmp621=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2).f1;_tmp620=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f3).f1;_tmp61F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f4;_LL14:
# 3774
 Cyc_Toc_exp_to_c(nv,_tmp622);Cyc_Toc_exp_to_c(nv,_tmp621);Cyc_Toc_exp_to_c(nv,_tmp620);{
struct _RegionHandle _tmp5E2=_new_region("temp");struct _RegionHandle*temp=& _tmp5E2;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAED=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAED,_tmp61F);});
# 3778
_npop_handler(0U);return;
# 3775
;_pop_region(temp);};case 10U: _LL15: _tmp625=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_tmp624=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2;_tmp623=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f3;_LL16:
# 3780
 Cyc_Toc_xlate_switch(nv,s,_tmp625,_tmp624,_tmp623);
return;case 11U: _LL17: _tmp627=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_tmp626=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2;_LL18: {
# 3783
struct Cyc_Toc_Env*_tmp5E3=nv;struct Cyc_Toc_FallthruInfo*_tmp5ED;_LL28: _tmp5ED=_tmp5E3->fallthru_info;_LL29:;
if(_tmp5ED == 0)
({void*_tmp5E4=0U;({struct _dyneither_ptr _tmpAEE=({const char*_tmp5E5="fallthru in unexpected place";_tag_dyneither(_tmp5E5,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAEE,_tag_dyneither(_tmp5E4,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp5E6=*_tmp5ED;struct Cyc_Toc_FallthruInfo _tmp5E7=_tmp5E6;struct _dyneither_ptr*_tmp5EC;struct Cyc_List_List*_tmp5EB;_LL2B: _tmp5EC=_tmp5E7.label;_tmp5EB=_tmp5E7.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp5EC,0U);
# 3789
({int _tmpAEF=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAEF,s2);});{
struct Cyc_List_List*_tmp5E8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5EB);
struct Cyc_List_List*_tmp5E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp627);
for(0;_tmp5E8 != 0;(_tmp5E8=_tmp5E8->tl,_tmp5E9=_tmp5E9->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5E9))->hd);
({struct Cyc_Absyn_Stmt*_tmpAF2=({struct Cyc_Absyn_Stmt*_tmpAF1=({struct Cyc_Absyn_Exp*_tmpAF0=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA->tag=5U,_tmp5EA->f1=(struct Cyc_Absyn_Vardecl*)_tmp5E8->hd;_tmp5EA;}),0U);Cyc_Absyn_assign_stmt(_tmpAF0,(struct Cyc_Absyn_Exp*)_tmp5E9->hd,0U);});Cyc_Absyn_seq_stmt(_tmpAF1,s2,0U);});s2=_tmpAF2;});}
# 3797
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp629=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_tmp628=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2;_LL1A:
# 3803
{void*_tmp5EE=_tmp629->r;void*_tmp5EF=_tmp5EE;struct Cyc_Absyn_Tvar*_tmp60A;struct Cyc_Absyn_Vardecl*_tmp609;struct Cyc_Absyn_Exp*_tmp608;struct Cyc_Absyn_Fndecl*_tmp607;struct Cyc_List_List*_tmp606;struct Cyc_Absyn_Pat*_tmp605;struct Cyc_Absyn_Exp*_tmp604;void*_tmp603;struct Cyc_Absyn_Vardecl*_tmp602;switch(*((int*)_tmp5EF)){case 0U: _LL2E: _tmp602=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5EF)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp602,_tmp628);goto _LL2D;case 2U: _LL30: _tmp605=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5EF)->f1;_tmp604=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5EF)->f3;_tmp603=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5EF)->f4;_LL31:
# 3811
{void*_tmp5F0=_tmp605->r;void*_tmp5F1=_tmp5F0;struct Cyc_Absyn_Vardecl*_tmp5F3;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5F1)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5F1)->f2)->r)->tag == 0U){_LL3B: _tmp5F3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5F1)->f1;_LL3C:
# 3813
({struct _tuple1*_tmpAF3=Cyc_Toc_temp_var();_tmp5F3->name=_tmpAF3;});
_tmp5F3->initializer=_tmp604;
({void*_tmpAF4=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->tag=0U,_tmp5F2->f1=_tmp5F3;_tmp5F2;});_tmp629->r=_tmpAF4;});
Cyc_Toc_local_decl_to_c(nv,_tmp5F3,_tmp628);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3822
({void*_tmpAF5=(Cyc_Toc_letdecl_to_c(nv,_tmp605,_tmp603,(void*)_check_null(_tmp604->topt),_tmp604,_tmp628))->r;s->r=_tmpAF5;});
goto _LL3A;}_LL3A:;}
# 3825
goto _LL2D;case 3U: _LL32: _tmp606=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5EF)->f1;_LL33: {
# 3829
struct Cyc_List_List*_tmp5F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp606);
if(_tmp5F4 == 0)
({void*_tmp5F5=0U;({struct _dyneither_ptr _tmpAF6=({const char*_tmp5F6="empty Letv_d";_tag_dyneither(_tmp5F6,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF6,_tag_dyneither(_tmp5F5,sizeof(void*),0U));});});
({void*_tmpAF7=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->tag=0U,_tmp5F7->f1=(struct Cyc_Absyn_Vardecl*)_tmp5F4->hd;_tmp5F7;});_tmp629->r=_tmpAF7;});
_tmp5F4=_tmp5F4->tl;
for(0;_tmp5F4 != 0;_tmp5F4=_tmp5F4->tl){
struct Cyc_Absyn_Decl*_tmp5F8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->tag=0U,_tmp5F9->f1=(struct Cyc_Absyn_Vardecl*)_tmp5F4->hd;_tmp5F9;}),0U);
({void*_tmpAF9=({struct Cyc_Absyn_Decl*_tmpAF8=_tmp5F8;Cyc_Absyn_decl_stmt(_tmpAF8,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpAF9;});}
# 3838
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp607=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5EF)->f1;_LL35:
# 3841
 Cyc_Toc_fndecl_to_c(nv,_tmp607,0);
Cyc_Toc_stmt_to_c(nv,_tmp628);
goto _LL2D;case 4U: _LL36: _tmp60A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5EF)->f1;_tmp609=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5EF)->f2;_tmp608=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5EF)->f3;_LL37: {
# 3845
struct Cyc_Absyn_Stmt*_tmp5FA=_tmp628;
# 3847
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp609->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3854
Cyc_Toc_stmt_to_c(nv,_tmp5FA);
if(Cyc_Absyn_no_regions)
({void*_tmpAFD=({struct _tuple1*_tmpAFC=x_var;void*_tmpAFB=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAFA=
Cyc_Absyn_uint_exp(0U,0U);
# 3856
Cyc_Absyn_declare_stmt(_tmpAFC,_tmpAFB,_tmpAFA,_tmp5FA,0U);})->r;s->r=_tmpAFD;});else{
# 3858
if((unsigned int)_tmp608){
# 3860
Cyc_Toc_exp_to_c(nv,_tmp608);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpAFF=_tmp608;Cyc_Absyn_aggrarrow_exp(_tmpAFF,({struct _dyneither_ptr*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));({struct _dyneither_ptr _tmpAFE=({const char*_tmp5FB="h";_tag_dyneither(_tmp5FB,sizeof(char),2U);});*_tmp5FC=_tmpAFE;});_tmp5FC;}),0U);}),0U);
({void*_tmpB00=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp5FA,0U))->r;s->r=_tmpB00;});};}else{
# 3870
({void*_tmpB0D=({
struct _tuple1*_tmpB0C=rh_var;void*_tmpB0B=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpB0A=({
struct Cyc_Absyn_Exp*_tmpB02=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpB02,({struct Cyc_List_List*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));
({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp5FD->hd=_tmpB01;}),_tmp5FD->tl=0;_tmp5FD;}),0U);});
# 3871
Cyc_Absyn_declare_stmt(_tmpB0C,_tmpB0B,_tmpB0A,({
# 3875
struct _tuple1*_tmpB09=x_var;void*_tmpB08=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpB07=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpB09,_tmpB08,_tmpB07,({
struct Cyc_Absyn_Stmt*_tmpB06=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpB03,({struct Cyc_Absyn_Exp*_tmp5FE[1U];_tmp5FE[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5FE,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpB06,({
struct Cyc_Absyn_Stmt*_tmpB05=_tmp5FA;Cyc_Absyn_seq_stmt(_tmpB05,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB04=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpB04,({struct Cyc_Absyn_Exp*_tmp5FF[1U];_tmp5FF[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5FF,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3870
s->r=_tmpB0D;});}}
# 3881
return;}default: _LL38: _LL39:
({void*_tmp600=0U;({struct _dyneither_ptr _tmpB0E=({const char*_tmp601="bad nested declaration within function";_tag_dyneither(_tmp601,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB0E,_tag_dyneither(_tmp600,sizeof(void*),0U));});});}_LL2D:;}
# 3884
return;case 13U: _LL1B: _tmp62B=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_tmp62A=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2;_LL1C:
# 3886
 s=_tmp62A;continue;case 14U: _LL1D: _tmp62D=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_tmp62C=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2).f1;_LL1E: {
# 3888
struct _RegionHandle _tmp60B=_new_region("temp");struct _RegionHandle*temp=& _tmp60B;_push_region(temp);
({struct Cyc_Toc_Env*_tmpB0F=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpB0F,_tmp62D);});
Cyc_Toc_exp_to_c(nv,_tmp62C);
# 3892
_npop_handler(0U);return;
# 3888
;_pop_region(temp);}default: _LL1F: _tmp630=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f1;_tmp62F=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f2;_tmp62E=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5D9)->f3;_LL20: {
# 3906 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3916
e_exp->topt=e_typ;{
struct _RegionHandle _tmp60C=_new_region("temp");struct _RegionHandle*temp=& _tmp60C;_push_region(temp);
# 3919
Cyc_Toc_stmt_to_c(nv,_tmp630);{
struct Cyc_Absyn_Stmt*_tmp60D=({struct Cyc_Absyn_Stmt*_tmpB10=_tmp630;Cyc_Absyn_seq_stmt(_tmpB10,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 3924
struct Cyc_Absyn_Stmt*_tmp60E=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=10U,_tmp615->f1=e_exp,_tmp615->f2=_tmp62F,_tmp615->f3=_tmp62E;_tmp615;}),0U);
# 3926
Cyc_Toc_stmt_to_c(nv,_tmp60E);{
# 3929
struct Cyc_Absyn_Exp*_tmp60F=({
struct Cyc_Absyn_Exp*_tmpB12=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpB12,({struct Cyc_List_List*_tmp614=_cycalloc(sizeof(*_tmp614));
({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp614->hd=_tmpB11;}),_tmp614->tl=0;_tmp614;}),0U);});
# 3933
struct Cyc_Absyn_Stmt*_tmp610=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB14=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpB14,({struct Cyc_List_List*_tmp613=_cycalloc(sizeof(*_tmp613));
({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Absyn_address_exp(h_exp,0U);_tmp613->hd=_tmpB13;}),_tmp613->tl=0;_tmp613;}),0U);}),0U);
# 3937
struct Cyc_Absyn_Exp*_tmp611=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp612=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpB23=({
struct _tuple1*_tmpB22=h_var;void*_tmpB21=h_typ;Cyc_Absyn_declare_stmt(_tmpB22,_tmpB21,0,({
struct Cyc_Absyn_Stmt*_tmpB20=_tmp610;Cyc_Absyn_seq_stmt(_tmpB20,({
struct _tuple1*_tmpB1F=was_thrown_var;void*_tmpB1E=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpB1D=_tmp611;Cyc_Absyn_declare_stmt(_tmpB1F,_tmpB1E,_tmpB1D,({
struct Cyc_Absyn_Stmt*_tmpB1C=({struct Cyc_Absyn_Exp*_tmpB16=_tmp60F;struct Cyc_Absyn_Stmt*_tmpB15=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp612,0U);
# 3943
Cyc_Absyn_ifthenelse_stmt(_tmpB16,_tmpB15,
# 3945
Cyc_Toc_skip_stmt_dl(),0U);});
# 3943
Cyc_Absyn_seq_stmt(_tmpB1C,({
# 3946
struct Cyc_Absyn_Exp*_tmpB1B=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpB1A=_tmp60D;Cyc_Absyn_ifthenelse_stmt(_tmpB1B,_tmpB1A,({
# 3948
struct _tuple1*_tmpB19=e_var;void*_tmpB18=e_typ;struct Cyc_Absyn_Exp*_tmpB17=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 3948
Cyc_Absyn_declare_stmt(_tmpB19,_tmpB18,_tmpB17,_tmp60E,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3939
s->r=_tmpB23;});};};
# 3952
_npop_handler(0U);return;
# 3917
;_pop_region(temp);};}}_LL0:;}}
# 3961
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpB24=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpB24;});{
struct _RegionHandle _tmp631=_new_region("frgn");struct _RegionHandle*frgn=& _tmp631;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp632=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp633=f->args;for(0;_tmp633 != 0;_tmp633=_tmp633->tl){
struct _tuple1*_tmp634=({struct _tuple1*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->f1=Cyc_Absyn_Loc_n,_tmp635->f2=(*((struct _tuple9*)_tmp633->hd)).f1;_tmp635;});
({void*_tmpB25=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp633->hd)).f3);(*((struct _tuple9*)_tmp633->hd)).f3=_tmpB25;});}}
# 3977
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3981
({struct Cyc_Hashtable_Table**_tmpB27=({struct Cyc_Hashtable_Table**_tmp636=_cycalloc(sizeof(*_tmp636));({struct Cyc_Hashtable_Table*_tmpB26=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp636=_tmpB26;});_tmp636;});Cyc_Toc_fn_pop_table=_tmpB27;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp637=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp638=_tmp637;struct _dyneither_ptr*_tmp641;struct Cyc_Absyn_Tqual _tmp640;void*_tmp63F;int _tmp63E;_LL1: _tmp641=_tmp638.name;_tmp640=_tmp638.tq;_tmp63F=_tmp638.type;_tmp63E=_tmp638.inject;_LL2:;{
void*_tmp639=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp63F,Cyc_Absyn_heap_rgn_type,_tmp640,Cyc_Absyn_false_type));
struct _tuple1*_tmp63A=({struct _tuple1*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->f1=Cyc_Absyn_Loc_n,_tmp63D->f2=(struct _dyneither_ptr*)_check_null(_tmp641);_tmp63D;});
({struct Cyc_List_List*_tmpB2A=({struct Cyc_List_List*_tmpB29=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB29,({struct Cyc_List_List*_tmp63C=_cycalloc(sizeof(*_tmp63C));({struct _tuple9*_tmpB28=({struct _tuple9*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->f1=_tmp641,_tmp63B->f2=_tmp640,_tmp63B->f3=_tmp639;_tmp63B;});_tmp63C->hd=_tmpB28;}),_tmp63C->tl=0;_tmp63C;}));});f->args=_tmpB2A;});
f->cyc_varargs=0;};}
# 3990
{struct Cyc_List_List*_tmp642=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp642 != 0;_tmp642=_tmp642->tl){
({void*_tmpB2B=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp642->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp642->hd)->type=_tmpB2B;});}}
# 3993
({struct Cyc_Toc_Env*_tmpB2C=Cyc_Toc_clear_toplevel(frgn,_tmp632);Cyc_Toc_stmt_to_c(_tmpB2C,f->body);});}
# 3970
;_pop_region(frgn);};}
# 3997
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp643=s;switch(_tmp643){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple40{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple41{struct Cyc_Toc_TocState*f1;struct _tuple40*f2;};
# 4014 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4017
struct _tuple41 _tmp644=*env;struct _tuple41 _tmp645=_tmp644;struct Cyc_Toc_TocState*_tmp67A;struct Cyc_Absyn_Aggrdecl*_tmp679;int _tmp678;_LL1: _tmp67A=_tmp645.f1;_tmp679=(_tmp645.f2)->f1;_tmp678=(_tmp645.f2)->f2;_LL2:;{
struct _tuple1*_tmp646=_tmp679->name;
struct Cyc_Toc_TocState _tmp647=*_tmp67A;struct Cyc_Toc_TocState _tmp648=_tmp647;struct Cyc_Dict_Dict*_tmp677;_LL4: _tmp677=_tmp648.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple19**_tmp649=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp677,_tmp646);
if(_tmp649 == 0){
seen_defn_before=0;{
struct _tuple19*v;
if(_tmp679->kind == Cyc_Absyn_StructA)
({struct _tuple19*_tmpB2E=({struct _tuple19*_tmp64A=_region_malloc(d,sizeof(*_tmp64A));_tmp64A->f1=_tmp679,({void*_tmpB2D=Cyc_Absyn_strctq(_tmp646);_tmp64A->f2=_tmpB2D;});_tmp64A;});v=_tmpB2E;});else{
# 4028
({struct _tuple19*_tmpB30=({struct _tuple19*_tmp64B=_region_malloc(d,sizeof(*_tmp64B));_tmp64B->f1=_tmp679,({void*_tmpB2F=Cyc_Absyn_unionq_type(_tmp646);_tmp64B->f2=_tmpB2F;});_tmp64B;});v=_tmpB30;});}
({struct Cyc_Dict_Dict _tmpB31=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*_tmp677,_tmp646,v);*_tmp677=_tmpB31;});};}else{
# 4031
struct _tuple19*_tmp64C=*_tmp649;struct _tuple19*_tmp64D=_tmp64C;struct Cyc_Absyn_Aggrdecl*_tmp650;void*_tmp64F;_LL7: _tmp650=_tmp64D->f1;_tmp64F=_tmp64D->f2;_LL8:;
if(_tmp650->impl == 0){
({struct Cyc_Dict_Dict _tmpB34=({struct Cyc_Dict_Dict _tmpB33=*_tmp677;struct _tuple1*_tmpB32=_tmp646;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpB33,_tmpB32,({struct _tuple19*_tmp64E=_region_malloc(d,sizeof(*_tmp64E));_tmp64E->f1=_tmp679,_tmp64E->f2=_tmp64F;_tmp64E;}));});*_tmp677=_tmpB34;});
seen_defn_before=0;}else{
# 4036
seen_defn_before=1;}}{
# 4038
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->kind=_tmp679->kind,_tmp676->sc=Cyc_Absyn_Public,_tmp676->name=_tmp679->name,_tmp676->tvs=0,_tmp676->impl=0,_tmp676->expected_mem_kind=0,_tmp676->attributes=_tmp679->attributes;_tmp676;});
# 4045
if(_tmp679->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpB35=({struct Cyc_Absyn_AggrdeclImpl*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->exist_vars=0,_tmp651->rgn_po=0,_tmp651->fields=0,_tmp651->tagged=0;_tmp651;});new_ad->impl=_tmpB35;});{
# 4050
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp652=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp679->impl))->fields;for(0;_tmp652 != 0;_tmp652=_tmp652->tl){
# 4054
struct Cyc_Absyn_Aggrfield*_tmp653=(struct Cyc_Absyn_Aggrfield*)_tmp652->hd;
void*_tmp654=_tmp653->type;
struct Cyc_List_List*_tmp655=_tmp653->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp654)) && (
_tmp679->kind == Cyc_Absyn_StructA  && _tmp652->tl == 0  || _tmp679->kind == Cyc_Absyn_UnionA)){
# 4068 "toc.cyc"
void*_tmp656=Cyc_Tcutil_compress(_tmp654);void*_tmp657=_tmp656;void*_tmp65F;struct Cyc_Absyn_Tqual _tmp65E;void*_tmp65D;unsigned int _tmp65C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp657)->tag == 4U){_LLA: _tmp65F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp657)->f1).elt_type;_tmp65E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp657)->f1).tq;_tmp65D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp657)->f1).zero_term;_tmp65C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp657)->f1).zt_loc;_LLB:
# 4071
({void*_tmpB37=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->tag=4U,(_tmp658->f1).elt_type=_tmp65F,(_tmp658->f1).tq=_tmp65E,({struct Cyc_Absyn_Exp*_tmpB36=Cyc_Absyn_uint_exp(0U,0U);(_tmp658->f1).num_elts=_tmpB36;}),(_tmp658->f1).zero_term=_tmp65D,(_tmp658->f1).zt_loc=_tmp65C;_tmp658;});_tmp654=_tmpB37;});
goto _LL9;}else{_LLC: _LLD:
# 4074
({struct Cyc_List_List*_tmpB39=({struct Cyc_List_List*_tmp65A=_cycalloc(sizeof(*_tmp65A));({void*_tmpB38=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->tag=6U,_tmp659->f1=0;_tmp659;});_tmp65A->hd=_tmpB38;}),_tmp65A->tl=_tmp655;_tmp65A;});_tmp655=_tmpB39;});
({void*_tmpB3D=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->tag=4U,({void*_tmpB3C=Cyc_Absyn_void_star_type();(_tmp65B->f1).elt_type=_tmpB3C;}),({
struct Cyc_Absyn_Tqual _tmpB3B=Cyc_Absyn_empty_tqual(0U);(_tmp65B->f1).tq=_tmpB3B;}),({
struct Cyc_Absyn_Exp*_tmpB3A=Cyc_Absyn_uint_exp(0U,0U);(_tmp65B->f1).num_elts=_tmpB3A;}),(_tmp65B->f1).zero_term=Cyc_Absyn_false_type,(_tmp65B->f1).zt_loc=0U;_tmp65B;});
# 4075
_tmp654=_tmpB3D;});}_LL9:;}{
# 4081
struct Cyc_Absyn_Aggrfield*_tmp660=({struct Cyc_Absyn_Aggrfield*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->name=_tmp653->name,_tmp672->tq=Cyc_Toc_mt_tq,({
# 4083
void*_tmpB3E=Cyc_Toc_typ_to_c(_tmp654);_tmp672->type=_tmpB3E;}),_tmp672->width=_tmp653->width,_tmp672->attributes=_tmp655,_tmp672->requires_clause=0;_tmp672;});
# 4091
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp679->impl))->tagged){
void*_tmp661=_tmp660->type;
struct _dyneither_ptr*_tmp662=_tmp660->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp66F=({struct Cyc_String_pa_PrintArg_struct _tmp79A;_tmp79A.tag=0U,_tmp79A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp679->name).f2);_tmp79A;});struct Cyc_String_pa_PrintArg_struct _tmp670=({struct Cyc_String_pa_PrintArg_struct _tmp799;_tmp799.tag=0U,_tmp799.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp662);_tmp799;});void*_tmp66D[2U];_tmp66D[0]=& _tmp66F,_tmp66D[1]=& _tmp670;({struct _dyneither_ptr _tmpB3F=({const char*_tmp66E="_union_%s_%s";_tag_dyneither(_tmp66E,sizeof(char),13U);});Cyc_aprintf(_tmpB3F,_tag_dyneither(_tmp66D,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp66C=_cycalloc(sizeof(*_tmp66C));*_tmp66C=s;_tmp66C;});
struct Cyc_Absyn_Aggrfield*_tmp663=({struct Cyc_Absyn_Aggrfield*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->name=Cyc_Toc_val_sp,_tmp66B->tq=Cyc_Toc_mt_tq,_tmp66B->type=_tmp661,_tmp66B->width=0,_tmp66B->attributes=0,_tmp66B->requires_clause=0;_tmp66B;});
struct Cyc_Absyn_Aggrfield*_tmp664=({struct Cyc_Absyn_Aggrfield*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->name=Cyc_Toc_tag_sp,_tmp66A->tq=Cyc_Toc_mt_tq,_tmp66A->type=Cyc_Absyn_sint_type,_tmp66A->width=0,_tmp66A->attributes=0,_tmp66A->requires_clause=0;_tmp66A;});
struct Cyc_List_List*_tmp665=({struct Cyc_Absyn_Aggrfield*_tmp669[2U];_tmp669[0]=_tmp664,_tmp669[1]=_tmp663;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp669,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp666=Cyc_Toc_make_c_struct_defn(str,_tmp665);
({struct Cyc_List_List*_tmpB41=({struct Cyc_List_List*_tmp668=_cycalloc(sizeof(*_tmp668));({struct Cyc_Absyn_Decl*_tmpB40=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->tag=5U,_tmp667->f1=_tmp666;_tmp667;}),0U);_tmp668->hd=_tmpB40;}),_tmp668->tl=Cyc_Toc_result_decls;_tmp668;});Cyc_Toc_result_decls=_tmpB41;});
({void*_tmpB42=Cyc_Absyn_strct(str);_tmp660->type=_tmpB42;});}
# 4103
({struct Cyc_List_List*_tmpB43=({struct Cyc_List_List*_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->hd=_tmp660,_tmp671->tl=new_fields;_tmp671;});new_fields=_tmpB43;});};}}
# 4105
({struct Cyc_List_List*_tmpB44=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpB44;});};}
# 4107
if(_tmp678)
({struct Cyc_List_List*_tmpB47=({struct Cyc_List_List*_tmp675=_cycalloc(sizeof(*_tmp675));({struct Cyc_Absyn_Decl*_tmpB46=({struct Cyc_Absyn_Decl*_tmp674=_cycalloc(sizeof(*_tmp674));({void*_tmpB45=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->tag=5U,_tmp673->f1=new_ad;_tmp673;});_tmp674->r=_tmpB45;}),_tmp674->loc=0U;_tmp674;});_tmp675->hd=_tmpB46;}),_tmp675->tl=Cyc_Toc_result_decls;_tmp675;});Cyc_Toc_result_decls=_tmpB47;});
return 0;};};};}
# 4112
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple40 p=({struct _tuple40 _tmp79B;_tmp79B.f1=ad,_tmp79B.f2=add_to_decls;_tmp79B;});
((int(*)(struct _tuple40*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple42{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4141 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 4144
struct _tuple42 _tmp67B=*env;struct _tuple42 _tmp67C=_tmp67B;struct Cyc_Set_Set**_tmp68D;struct Cyc_Absyn_Datatypedecl*_tmp68C;_LL1: _tmp68D=(_tmp67C.f1)->datatypes_so_far;_tmp68C=_tmp67C.f2;_LL2:;{
struct _tuple1*_tmp67D=_tmp68C->name;
if(_tmp68C->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp68D,_tmp67D))
return 0;
({struct Cyc_Set_Set*_tmpB48=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp68D,_tmp67D);*_tmp68D=_tmpB48;});
{struct Cyc_List_List*_tmp67E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp68C->fields))->v;for(0;_tmp67E != 0;_tmp67E=_tmp67E->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp67E->hd;
# 4152
struct Cyc_List_List*_tmp67F=0;
int i=1;
{struct Cyc_List_List*_tmp680=f->typs;for(0;_tmp680 != 0;(_tmp680=_tmp680->tl,i ++)){
struct _dyneither_ptr*_tmp681=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp682=({struct Cyc_Absyn_Aggrfield*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->name=_tmp681,_tmp684->tq=(*((struct _tuple12*)_tmp680->hd)).f1,({
void*_tmpB49=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp680->hd)).f2);_tmp684->type=_tmpB49;}),_tmp684->width=0,_tmp684->attributes=0,_tmp684->requires_clause=0;_tmp684;});
({struct Cyc_List_List*_tmpB4A=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->hd=_tmp682,_tmp683->tl=_tmp67F;_tmp683;});_tmp67F=_tmpB4A;});}}
# 4160
({struct Cyc_List_List*_tmpB4D=({struct Cyc_List_List*_tmp686=_cycalloc(sizeof(*_tmp686));({struct Cyc_Absyn_Aggrfield*_tmpB4C=({struct Cyc_Absyn_Aggrfield*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->name=Cyc_Toc_tag_sp,_tmp685->tq=Cyc_Toc_mt_tq,_tmp685->type=Cyc_Absyn_sint_type,_tmp685->width=0,_tmp685->attributes=0,_tmp685->requires_clause=0;_tmp685;});_tmp686->hd=_tmpB4C;}),({
struct Cyc_List_List*_tmpB4B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp67F);_tmp686->tl=_tmpB4B;});_tmp686;});
# 4160
_tmp67F=_tmpB4D;});{
# 4162
struct Cyc_Absyn_Aggrdecl*_tmp687=({struct _dyneither_ptr*_tmpB4F=({struct _dyneither_ptr*_tmp68B=_cycalloc(sizeof(*_tmp68B));({struct _dyneither_ptr _tmpB4E=({const char*_tmp68A="";_tag_dyneither(_tmp68A,sizeof(char),1U);});*_tmp68B=_tmpB4E;});_tmp68B;});Cyc_Toc_make_c_struct_defn(_tmpB4F,_tmp67F);});
({struct _tuple1*_tmpB50=Cyc_Toc_collapse_qvars(f->name,_tmp68C->name);_tmp687->name=_tmpB50;});
({struct Cyc_List_List*_tmpB52=({struct Cyc_List_List*_tmp689=_cycalloc(sizeof(*_tmp689));({struct Cyc_Absyn_Decl*_tmpB51=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->tag=5U,_tmp688->f1=_tmp687;_tmp688;}),0U);_tmp689->hd=_tmpB51;}),_tmp689->tl=Cyc_Toc_result_decls;_tmp689;});Cyc_Toc_result_decls=_tmpB52;});};}}
# 4166
return 0;};}
# 4169
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4188 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 4191
struct _tuple42 _tmp68E=*env;struct _tuple42 _tmp68F=_tmp68E;struct Cyc_Toc_TocState*_tmp6B1;struct Cyc_Absyn_Datatypedecl*_tmp6B0;_LL1: _tmp6B1=_tmp68F.f1;_tmp6B0=_tmp68F.f2;_LL2:;
if(_tmp6B0->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp690=*_tmp6B1;struct Cyc_Toc_TocState _tmp691=_tmp690;struct Cyc_Dict_Dict*_tmp6AF;_LL4: _tmp6AF=_tmp691.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp692=_tmp6B0->name;
{struct Cyc_List_List*_tmp693=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6B0->fields))->v;for(0;_tmp693 != 0;_tmp693=_tmp693->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp693->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp694=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp695=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp694,Cyc_Absyn_false_type,0U);
# 4202
int*_tmp696=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp6AF,f->name);int*_tmp697=_tmp696;if(_tmp697 == 0){_LL7: _LL8: {
# 4204
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpB53=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpB53;});{
# 4208
struct Cyc_Absyn_Vardecl*_tmp698=Cyc_Absyn_new_vardecl(0U,f->name,_tmp695,initopt);
_tmp698->sc=f->sc;
({struct Cyc_List_List*_tmpB55=({struct Cyc_List_List*_tmp69A=_cycalloc(sizeof(*_tmp69A));({struct Cyc_Absyn_Decl*_tmpB54=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->tag=0U,_tmp699->f1=_tmp698;_tmp699;}),0U);_tmp69A->hd=_tmpB54;}),_tmp69A->tl=Cyc_Toc_result_decls;_tmp69A;});Cyc_Toc_result_decls=_tmpB55;});
({struct Cyc_Dict_Dict _tmpB56=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6AF,f->name,f->sc != Cyc_Absyn_Extern);
# 4211
*_tmp6AF=_tmpB56;});{
# 4213
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp69B=f->typs;for(0;_tmp69B != 0;(_tmp69B=_tmp69B->tl,i ++)){
struct _dyneither_ptr*_tmp69C=({struct _dyneither_ptr*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));({struct _dyneither_ptr _tmpB58=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6A2=({struct Cyc_Int_pa_PrintArg_struct _tmp79C;_tmp79C.tag=1U,_tmp79C.f1=(unsigned long)i;_tmp79C;});void*_tmp6A0[1U];_tmp6A0[0]=& _tmp6A2;({struct _dyneither_ptr _tmpB57=({const char*_tmp6A1="f%d";_tag_dyneither(_tmp6A1,sizeof(char),4U);});Cyc_aprintf(_tmpB57,_tag_dyneither(_tmp6A0,sizeof(void*),1U));});});*_tmp6A3=_tmpB58;});_tmp6A3;});
struct Cyc_Absyn_Aggrfield*_tmp69D=({struct Cyc_Absyn_Aggrfield*_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F->name=_tmp69C,_tmp69F->tq=(*((struct _tuple12*)_tmp69B->hd)).f1,({
void*_tmpB59=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp69B->hd)).f2);_tmp69F->type=_tmpB59;}),_tmp69F->width=0,_tmp69F->attributes=0,_tmp69F->requires_clause=0;_tmp69F;});
({struct Cyc_List_List*_tmpB5A=({struct Cyc_List_List*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E->hd=_tmp69D,_tmp69E->tl=fields;_tmp69E;});fields=_tmpB5A;});}}
# 4221
({struct Cyc_List_List*_tmpB5E=({struct Cyc_List_List*_tmp6A5=_cycalloc(sizeof(*_tmp6A5));({struct Cyc_Absyn_Aggrfield*_tmpB5D=({struct Cyc_Absyn_Aggrfield*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4->name=Cyc_Toc_tag_sp,_tmp6A4->tq=Cyc_Toc_mt_tq,({
void*_tmpB5C=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp6A4->type=_tmpB5C;}),_tmp6A4->width=0,_tmp6A4->attributes=0,_tmp6A4->requires_clause=0;_tmp6A4;});
# 4221
_tmp6A5->hd=_tmpB5D;}),({
# 4223
struct Cyc_List_List*_tmpB5B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6A5->tl=_tmpB5B;});_tmp6A5;});
# 4221
fields=_tmpB5E;});{
# 4224
struct Cyc_Absyn_Aggrdecl*_tmp6A6=({struct _dyneither_ptr*_tmpB60=({struct _dyneither_ptr*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));({struct _dyneither_ptr _tmpB5F=({const char*_tmp6A9="";_tag_dyneither(_tmp6A9,sizeof(char),1U);});*_tmp6AA=_tmpB5F;});_tmp6AA;});Cyc_Toc_make_c_struct_defn(_tmpB60,fields);});
({struct _tuple1*_tmpB61=Cyc_Toc_collapse_qvars(f->name,_tmp6B0->name);_tmp6A6->name=_tmpB61;});
({struct Cyc_List_List*_tmpB63=({struct Cyc_List_List*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));({struct Cyc_Absyn_Decl*_tmpB62=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->tag=5U,_tmp6A7->f1=_tmp6A6;_tmp6A7;}),0U);_tmp6A8->hd=_tmpB62;}),_tmp6A8->tl=Cyc_Toc_result_decls;_tmp6A8;});Cyc_Toc_result_decls=_tmpB63;});
goto _LL6;};};};}}else{if(*((int*)_tmp697)== 0){_LL9: _LLA:
# 4229
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6AB=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp6AC=Cyc_Absyn_new_vardecl(0U,f->name,_tmp695,_tmp6AB);
_tmp6AC->sc=f->sc;
({struct Cyc_List_List*_tmpB65=({struct Cyc_List_List*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));({struct Cyc_Absyn_Decl*_tmpB64=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->tag=0U,_tmp6AD->f1=_tmp6AC;_tmp6AD;}),0U);_tmp6AE->hd=_tmpB64;}),_tmp6AE->tl=Cyc_Toc_result_decls;_tmp6AE;});Cyc_Toc_result_decls=_tmpB65;});
({struct Cyc_Dict_Dict _tmpB66=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6AF,f->name,1);*_tmp6AF=_tmpB66;});}
# 4236
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4240
return 0;};};}
# 4243
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4247
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4253
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpB67=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpB67;});
# 4257
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4266
struct _RegionHandle _tmp6B2=_new_region("temp");struct _RegionHandle*temp=& _tmp6B2;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6B3=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp6B3,init);}
# 4267
;_pop_region(temp);}else{
# 4271
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4274
void*_tmp6B4=Cyc_Tcutil_compress(old_typ);void*_tmp6B5=_tmp6B4;void*_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BB;void*_tmp6BA;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B5)->tag == 4U){_LL1: _tmp6BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B5)->f1).elt_type;_tmp6BB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B5)->f1).num_elts;_tmp6BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B5)->f1).zero_term;_LL2:
# 4276
 if(Cyc_Tcutil_force_type2bool(0,_tmp6BA)){
if(_tmp6BB == 0)
({void*_tmp6B6=0U;({struct _dyneither_ptr _tmpB68=({const char*_tmp6B7="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp6B7,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB68,_tag_dyneither(_tmp6B6,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp6BB;
struct Cyc_Absyn_Exp*_tmp6B8=({struct Cyc_Absyn_Exp*_tmpB6A=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpB6A,({
struct Cyc_Absyn_Exp*_tmpB69=num_elts;Cyc_Absyn_add_exp(_tmpB69,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4283
struct Cyc_Absyn_Exp*_tmp6B9=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpB6C=({struct Cyc_Absyn_Stmt*_tmpB6B=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp6B8,_tmp6B9,0U),0U);Cyc_Toc_seq_stmt_r(_tmpB6B,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4284
s->r=_tmpB6C;});};}
# 4287
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4295
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6BD=d;struct Cyc_List_List*_tmp6C4;struct Cyc_List_List*_tmp6C3;void**_tmp6C2;struct Cyc_Tcpat_Rhs*_tmp6C1;switch(*((int*)_tmp6BD)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6C1=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6BD)->f1;_LL4:
 _tmp6C1->rhs=success;return d;default: _LL5: _tmp6C4=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6BD)->f1;_tmp6C3=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6BD)->f2;_tmp6C2=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6BD)->f3;_LL6:
# 4300
({void*_tmpB6D=Cyc_Toc_rewrite_decision(*_tmp6C2,success);*_tmp6C2=_tmpB6D;});
for(0;_tmp6C3 != 0;_tmp6C3=_tmp6C3->tl){
struct _tuple33*_tmp6BE=(struct _tuple33*)_tmp6C3->hd;struct _tuple33*_tmp6BF=_tmp6BE;void**_tmp6C0;_LL8: _tmp6C0=(void**)& _tmp6BF->f2;_LL9:;
({void*_tmpB6E=Cyc_Toc_rewrite_decision(*_tmp6C0,success);*_tmp6C0=_tmpB6E;});}
# 4305
return d;}_LL0:;}
# 4316 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4318
struct _RegionHandle _tmp6C5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6C5;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6C6=Cyc_Toc_share_env(rgn,nv);
void*_tmp6C7=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp6C6,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4328
struct Cyc_Absyn_Stmt*_tmp6C8=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4332
({void*_tmpB6F=Cyc_Toc_rewrite_decision(dopt,_tmp6C8);dopt=_tmpB6F;});{
# 4334
struct Cyc_Absyn_Switch_clause*_tmp6C9=({struct Cyc_Absyn_Switch_clause*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5->pattern=p,_tmp6D5->pat_vars=0,_tmp6D5->where_clause=0,_tmp6D5->body=_tmp6C8,_tmp6D5->loc=0U;_tmp6D5;});
struct Cyc_List_List*_tmp6CA=({struct _RegionHandle*_tmpB72=rgn;struct _RegionHandle*_tmpB71=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple36*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpB72,Cyc_Toc_gen_labels,_tmpB71,({struct Cyc_Absyn_Switch_clause*_tmp6D4[1U];_tmp6D4[0]=_tmp6C9;({struct _RegionHandle*_tmpB70=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpB70,_tag_dyneither(_tmp6D4,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4337
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4341
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp6C6,& mydecls,& mybodies,dopt,_tmp6CA,v);
# 4344
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple39*_tmp6CB=(struct _tuple39*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple39*_tmp6CC=_tmp6CB;struct Cyc_Toc_Env*_tmp6CE;struct Cyc_Absyn_Stmt*_tmp6CD;_LL1: _tmp6CE=_tmp6CC->f1;_tmp6CD=_tmp6CC->f3;_LL2:;
if(_tmp6CD == _tmp6C8){senv=_tmp6CE;goto FOUND_ENV;}}
# 4349
({void*_tmp6CF=0U;({struct _dyneither_ptr _tmpB73=({const char*_tmp6D0="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp6D0,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB73,_tag_dyneither(_tmp6CF,sizeof(void*),0U));});});
FOUND_ENV:
# 4353
 Cyc_Toc_stmt_to_c(senv,s);{
# 4355
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4357
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp6D1=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpB74=Cyc_Toc_temp_var();_tmp6D1->name=_tmpB74;});
({void*_tmpB75=Cyc_Toc_typ_to_c_array(_tmp6D1->type);_tmp6D1->type=_tmpB75;});
({struct Cyc_Absyn_Stmt*_tmpB77=({struct Cyc_Absyn_Decl*_tmpB76=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2->tag=0U,_tmp6D2->f1=_tmp6D1;_tmp6D2;}),0U);Cyc_Absyn_decl_stmt(_tmpB76,res,0U);});res=_tmpB77;});}
# 4364
({struct Cyc_Absyn_Stmt*_tmpB7B=({struct _tuple1*_tmpB7A=v;void*_tmpB79=Cyc_Toc_typ_to_c(_tmp6C7);struct Cyc_Absyn_Exp*_tmpB78=e;Cyc_Absyn_declare_stmt(_tmpB7A,_tmpB79,_tmpB78,res,0U);});res=_tmpB7B;});{
struct Cyc_Absyn_Stmt*_tmp6D3=res;_npop_handler(0U);return _tmp6D3;};};};};}
# 4319
;_pop_region(rgn);}
# 4372
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6D6=e->r;void*_tmp6D7=_tmp6D6;struct Cyc_Absyn_MallocInfo*_tmp702;struct Cyc_Absyn_Stmt*_tmp701;void**_tmp700;void**_tmp6FF;struct Cyc_List_List*_tmp6FE;struct Cyc_List_List*_tmp6FD;void**_tmp6FC;struct Cyc_List_List*_tmp6FB;void**_tmp6FA;struct Cyc_Absyn_Exp*_tmp6F9;struct Cyc_Absyn_Exp*_tmp6F8;struct Cyc_List_List*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F5;struct Cyc_Absyn_Exp*_tmp6F4;struct Cyc_Absyn_Exp*_tmp6F3;struct Cyc_Absyn_Exp*_tmp6F2;struct Cyc_Absyn_Exp*_tmp6F1;struct Cyc_Absyn_Exp*_tmp6F0;struct Cyc_Absyn_Exp*_tmp6EF;struct Cyc_Absyn_Exp*_tmp6EE;struct Cyc_Absyn_Exp*_tmp6ED;struct Cyc_Absyn_Exp*_tmp6EC;struct Cyc_Absyn_Exp*_tmp6EB;struct Cyc_Absyn_Exp*_tmp6EA;struct Cyc_Absyn_Exp*_tmp6E9;struct Cyc_Absyn_Exp*_tmp6E8;struct Cyc_List_List*_tmp6E7;struct Cyc_Absyn_Exp*_tmp6E6;struct Cyc_Absyn_Exp*_tmp6E5;struct Cyc_Absyn_Exp*_tmp6E4;struct Cyc_Absyn_Exp*_tmp6E3;struct Cyc_Absyn_Exp*_tmp6E2;struct Cyc_Absyn_Exp*_tmp6E1;struct Cyc_Absyn_Exp*_tmp6E0;struct Cyc_Absyn_Exp*_tmp6DF;struct Cyc_Absyn_Exp*_tmp6DE;switch(*((int*)_tmp6D7)){case 41U: _LL1: _tmp6DE=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL2:
 _tmp6DF=_tmp6DE;goto _LL4;case 20U: _LL3: _tmp6DF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL4:
 _tmp6E0=_tmp6DF;goto _LL6;case 21U: _LL5: _tmp6E0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL6:
 _tmp6E1=_tmp6E0;goto _LL8;case 22U: _LL7: _tmp6E1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL8:
 _tmp6E2=_tmp6E1;goto _LLA;case 15U: _LL9: _tmp6E2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LLA:
 _tmp6E3=_tmp6E2;goto _LLC;case 11U: _LLB: _tmp6E3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LLC:
 _tmp6E4=_tmp6E3;goto _LLE;case 12U: _LLD: _tmp6E4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LLE:
 _tmp6E5=_tmp6E4;goto _LL10;case 18U: _LLF: _tmp6E5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL10:
 _tmp6E6=_tmp6E5;goto _LL12;case 5U: _LL11: _tmp6E6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp6E6);goto _LL0;case 3U: _LL13: _tmp6E7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6E7);goto _LL0;case 7U: _LL15: _tmp6E9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_tmp6E8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL16:
 _tmp6EB=_tmp6E9;_tmp6EA=_tmp6E8;goto _LL18;case 8U: _LL17: _tmp6EB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_tmp6EA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL18:
 _tmp6ED=_tmp6EB;_tmp6EC=_tmp6EA;goto _LL1A;case 9U: _LL19: _tmp6ED=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_tmp6EC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL1A:
 _tmp6EF=_tmp6ED;_tmp6EE=_tmp6EC;goto _LL1C;case 23U: _LL1B: _tmp6EF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_tmp6EE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL1C:
 _tmp6F1=_tmp6EF;_tmp6F0=_tmp6EE;goto _LL1E;case 35U: _LL1D: _tmp6F1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_tmp6F0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL1E:
 _tmp6F3=_tmp6F1;_tmp6F2=_tmp6F0;goto _LL20;case 4U: _LL1F: _tmp6F3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_tmp6F2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6D7)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp6F3);Cyc_Toc_exptypes_to_c(_tmp6F2);goto _LL0;case 6U: _LL21: _tmp6F6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_tmp6F5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_tmp6F4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6D7)->f3;_LL22:
# 4391
 Cyc_Toc_exptypes_to_c(_tmp6F6);Cyc_Toc_exptypes_to_c(_tmp6F5);Cyc_Toc_exptypes_to_c(_tmp6F4);goto _LL0;case 10U: _LL23: _tmp6F8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_tmp6F7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL24:
# 4393
 Cyc_Toc_exptypes_to_c(_tmp6F8);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6F7);goto _LL0;case 14U: _LL25: _tmp6FA=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_tmp6F9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL26:
({void*_tmpB7C=Cyc_Toc_typ_to_c(*_tmp6FA);*_tmp6FA=_tmpB7C;});Cyc_Toc_exptypes_to_c(_tmp6F9);goto _LL0;case 25U: _LL27: _tmp6FC=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1)->f3;_tmp6FB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL28:
# 4396
({void*_tmpB7D=Cyc_Toc_typ_to_c(*_tmp6FC);*_tmp6FC=_tmpB7D;});
_tmp6FD=_tmp6FB;goto _LL2A;case 36U: _LL29: _tmp6FD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6D7)->f2;_LL2A:
 _tmp6FE=_tmp6FD;goto _LL2C;case 26U: _LL2B: _tmp6FE=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL2C:
# 4400
 for(0;_tmp6FE != 0;_tmp6FE=_tmp6FE->tl){
struct _tuple20 _tmp6D8=*((struct _tuple20*)_tmp6FE->hd);struct _tuple20 _tmp6D9=_tmp6D8;struct Cyc_Absyn_Exp*_tmp6DA;_LL56: _tmp6DA=_tmp6D9.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp6DA);}
# 4404
goto _LL0;case 19U: _LL2D: _tmp6FF=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL2E:
 _tmp700=_tmp6FF;goto _LL30;case 17U: _LL2F: _tmp700=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL30:
({void*_tmpB7E=Cyc_Toc_typ_to_c(*_tmp700);*_tmp700=_tmpB7E;});goto _LL0;case 37U: _LL31: _tmp701=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp701);goto _LL0;case 34U: _LL33: _tmp702=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6D7)->f1;_LL34:
# 4409
 if(_tmp702->elt_type != 0)
({void**_tmpB80=({void**_tmp6DB=_cycalloc(sizeof(*_tmp6DB));({void*_tmpB7F=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp702->elt_type)));*_tmp6DB=_tmpB7F;});_tmp6DB;});_tmp702->elt_type=_tmpB80;});
Cyc_Toc_exptypes_to_c(_tmp702->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4419
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
# 4430
({void*_tmp6DC=0U;({unsigned int _tmpB82=e->loc;struct _dyneither_ptr _tmpB81=({const char*_tmp6DD="Cyclone expression within C code";_tag_dyneither(_tmp6DD,sizeof(char),33U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpB82,_tmpB81,_tag_dyneither(_tmp6DC,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4434
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp703=d->r;void*_tmp704=_tmp703;struct Cyc_Absyn_Typedefdecl*_tmp70E;struct Cyc_Absyn_Enumdecl*_tmp70D;struct Cyc_Absyn_Aggrdecl*_tmp70C;struct Cyc_Absyn_Fndecl*_tmp70B;struct Cyc_Absyn_Vardecl*_tmp70A;switch(*((int*)_tmp704)){case 0U: _LL1: _tmp70A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL2:
# 4437
({void*_tmpB83=Cyc_Toc_typ_to_c(_tmp70A->type);_tmp70A->type=_tmpB83;});
if(_tmp70A->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp70A->initializer));
goto _LL0;case 1U: _LL3: _tmp70B=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL4:
# 4441
({void*_tmpB84=Cyc_Toc_typ_to_c(_tmp70B->ret_type);_tmp70B->ret_type=_tmpB84;});
{struct Cyc_List_List*_tmp705=_tmp70B->args;for(0;_tmp705 != 0;_tmp705=_tmp705->tl){
({void*_tmpB85=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp705->hd)).f3);(*((struct _tuple9*)_tmp705->hd)).f3=_tmpB85;});}}
# 4445
goto _LL0;case 5U: _LL5: _tmp70C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp70C,1);goto _LL0;case 7U: _LL7: _tmp70D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL8:
# 4448
 if(_tmp70D->fields != 0){
struct Cyc_List_List*_tmp706=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp70D->fields))->v;for(0;_tmp706 != 0;_tmp706=_tmp706->tl){
struct Cyc_Absyn_Enumfield*_tmp707=(struct Cyc_Absyn_Enumfield*)_tmp706->hd;
if(_tmp707->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp707->tag));}}
# 4453
goto _LL0;case 8U: _LL9: _tmp70E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LLA:
({void*_tmpB86=Cyc_Toc_typ_to_c((void*)_check_null(_tmp70E->defn));_tmp70E->defn=_tmpB86;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4463
({void*_tmp708=0U;({unsigned int _tmpB88=d->loc;struct _dyneither_ptr _tmpB87=({const char*_tmp709="Cyclone declaration within C code";_tag_dyneither(_tmp709,sizeof(char),34U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpB88,_tmpB87,_tag_dyneither(_tmp708,sizeof(void*),0U));});});goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
 goto _LL0;}_LL0:;}
# 4469
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp70F=s->r;void*_tmp710=_tmp70F;struct Cyc_Absyn_Stmt*_tmp728;struct Cyc_Absyn_Stmt*_tmp727;struct Cyc_Absyn_Exp*_tmp726;struct Cyc_Absyn_Decl*_tmp725;struct Cyc_Absyn_Stmt*_tmp724;struct Cyc_Absyn_Exp*_tmp723;struct Cyc_List_List*_tmp722;void*_tmp721;struct Cyc_Absyn_Exp*_tmp720;struct Cyc_Absyn_Exp*_tmp71F;struct Cyc_Absyn_Exp*_tmp71E;struct Cyc_Absyn_Stmt*_tmp71D;struct Cyc_Absyn_Exp*_tmp71C;struct Cyc_Absyn_Stmt*_tmp71B;struct Cyc_Absyn_Exp*_tmp71A;struct Cyc_Absyn_Stmt*_tmp719;struct Cyc_Absyn_Stmt*_tmp718;struct Cyc_Absyn_Exp*_tmp717;struct Cyc_Absyn_Stmt*_tmp716;struct Cyc_Absyn_Stmt*_tmp715;struct Cyc_Absyn_Exp*_tmp714;switch(*((int*)_tmp710)){case 1U: _LL1: _tmp714=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp710)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp714);goto _LL0;case 2U: _LL3: _tmp716=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp710)->f1;_tmp715=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp710)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp716);Cyc_Toc_stmttypes_to_c(_tmp715);goto _LL0;case 3U: _LL5: _tmp717=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp710)->f1;_LL6:
 if(_tmp717 != 0)Cyc_Toc_exptypes_to_c(_tmp717);goto _LL0;case 4U: _LL7: _tmp71A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp710)->f1;_tmp719=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp710)->f2;_tmp718=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp710)->f3;_LL8:
# 4475
 Cyc_Toc_exptypes_to_c(_tmp71A);Cyc_Toc_stmttypes_to_c(_tmp719);Cyc_Toc_stmttypes_to_c(_tmp718);goto _LL0;case 5U: _LL9: _tmp71C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp710)->f1).f1;_tmp71B=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp710)->f2;_LLA:
# 4477
 Cyc_Toc_exptypes_to_c(_tmp71C);Cyc_Toc_stmttypes_to_c(_tmp71B);goto _LL0;case 9U: _LLB: _tmp720=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp710)->f1;_tmp71F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp710)->f2).f1;_tmp71E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp710)->f3).f1;_tmp71D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp710)->f4;_LLC:
# 4479
 Cyc_Toc_exptypes_to_c(_tmp720);Cyc_Toc_exptypes_to_c(_tmp71F);Cyc_Toc_exptypes_to_c(_tmp71E);
Cyc_Toc_stmttypes_to_c(_tmp71D);goto _LL0;case 10U: _LLD: _tmp723=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp710)->f1;_tmp722=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp710)->f2;_tmp721=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp710)->f3;_LLE:
# 4482
 Cyc_Toc_exptypes_to_c(_tmp723);
for(0;_tmp722 != 0;_tmp722=_tmp722->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp722->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp725=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp710)->f1;_tmp724=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp710)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp725);Cyc_Toc_stmttypes_to_c(_tmp724);goto _LL0;case 14U: _LL11: _tmp727=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp710)->f1;_tmp726=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp710)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp727);Cyc_Toc_exptypes_to_c(_tmp726);goto _LL0;case 13U: _LL13: _tmp728=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp710)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp728);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4492
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4495
({void*_tmpB89=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->tag=0U;_tmp711;});s->r=_tmpB89;});
goto _LL0;default: _LL1F: _LL20:
# 4498
({void*_tmp712=0U;({unsigned int _tmpB8B=s->loc;struct _dyneither_ptr _tmpB8A=({const char*_tmp713="Cyclone statement in C code";_tag_dyneither(_tmp713,sizeof(char),28U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpB8B,_tmpB8A,_tag_dyneither(_tmp712,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4502
static void*Cyc_Toc_decl_to_enumtype(struct Cyc_Absyn_Decl*d){
void*_tmp729=d->r;void*_tmp72A=_tmp729;struct Cyc_Absyn_Enumdecl*_tmp72F;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp72A)->tag == 7U){_LL1: _tmp72F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp72A)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C->tag=0U,({void*_tmpB8C=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B->tag=15U,_tmp72B->f1=_tmp72F->name,_tmp72B->f2=_tmp72F;_tmp72B;});_tmp72C->f1=_tmpB8C;}),_tmp72C->f2=0;_tmp72C;});}else{_LL3: _LL4:
({void*_tmp72D=0U;({struct _dyneither_ptr _tmpB8D=({const char*_tmp72E="Toc::get_enumdecl_name";_tag_dyneither(_tmp72E,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB8D,_tag_dyneither(_tmp72D,sizeof(void*),0U));});});}_LL0:;}
# 4509
static struct Cyc_Absyn_Decl*Cyc_Toc_type2enumdecl(void*t){
void*_tmp730=t;struct Cyc_Absyn_Enumdecl*_tmp734;unsigned int _tmp733;if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp730)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp730)->f1)->r)->tag == 1U){_LL1: _tmp734=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp730)->f1)->r)->f1;_tmp733=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp730)->f1)->loc;_LL2:
# 4512
 return({struct Cyc_Absyn_Decl*_tmp732=_cycalloc(sizeof(*_tmp732));({void*_tmpB8E=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731->tag=7U,_tmp731->f1=_tmp734;_tmp731;});_tmp732->r=_tmpB8E;}),_tmp732->loc=_tmp733;_tmp732;});}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 4520
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp735=0U;({struct _dyneither_ptr _tmpB8F=({const char*_tmp736="decls_to_c: not at toplevel!";_tag_dyneither(_tmp736,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB8F,_tag_dyneither(_tmp735,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp737=d->r;void*_tmp738=_tmp737;struct Cyc_List_List*_tmp753;struct Cyc_List_List*_tmp752;struct Cyc_List_List*_tmp751;struct Cyc_List_List*_tmp750;struct Cyc_Absyn_Typedefdecl*_tmp74F;struct Cyc_Absyn_Enumdecl*_tmp74E;struct Cyc_Absyn_Datatypedecl*_tmp74D;struct Cyc_Absyn_Aggrdecl*_tmp74C;struct Cyc_Absyn_Fndecl*_tmp74B;struct Cyc_Absyn_Vardecl*_tmp74A;switch(*((int*)_tmp738)){case 0U: _LL1: _tmp74A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp738)->f1;_LL2: {
# 4528
struct _tuple1*_tmp739=_tmp74A->name;
# 4530
if(_tmp74A->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpB91=({struct _tuple1*_tmp73A=_cycalloc(sizeof(*_tmp73A));({union Cyc_Absyn_Nmspace _tmpB90=Cyc_Absyn_Abs_n(0,1);_tmp73A->f1=_tmpB90;}),_tmp73A->f2=(*_tmp739).f2;_tmp73A;});_tmp739=_tmpB91;});
if(_tmp74A->initializer != 0){
if(_tmp74A->sc == Cyc_Absyn_ExternC)_tmp74A->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp74A->initializer));else{
# 4537
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp74A->initializer));}}
# 4539
_tmp74A->name=_tmp739;
({enum Cyc_Absyn_Scope _tmpB92=Cyc_Toc_scope_to_c(_tmp74A->sc);_tmp74A->sc=_tmpB92;});
({void*_tmpB93=Cyc_Toc_typ_to_c(_tmp74A->type);_tmp74A->type=_tmpB93;});
({struct Cyc_List_List*_tmpB94=({struct Cyc_List_List*_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B->hd=d,_tmp73B->tl=Cyc_Toc_result_decls;_tmp73B;});Cyc_Toc_result_decls=_tmpB94;});
goto _LL0;}case 1U: _LL3: _tmp74B=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp738)->f1;_LL4: {
# 4545
struct _tuple1*_tmp73C=_tmp74B->name;
# 4547
if(_tmp74B->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB96=({struct _tuple1*_tmp73D=_cycalloc(sizeof(*_tmp73D));({union Cyc_Absyn_Nmspace _tmpB95=Cyc_Absyn_Abs_n(0,1);_tmp73D->f1=_tmpB95;}),_tmp73D->f2=(*_tmp73C).f2;_tmp73D;});_tmp73C=_tmpB96;});
_tmp74B->sc=Cyc_Absyn_Public;}
# 4551
_tmp74B->name=_tmp73C;
Cyc_Toc_fndecl_to_c(nv,_tmp74B,cinclude);
({struct Cyc_List_List*_tmpB97=({struct Cyc_List_List*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E->hd=d,_tmp73E->tl=Cyc_Toc_result_decls;_tmp73E;});Cyc_Toc_result_decls=_tmpB97;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp73F=0U;({struct _dyneither_ptr _tmpB98=({const char*_tmp740="letdecl at toplevel";_tag_dyneither(_tmp740,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB98,_tag_dyneither(_tmp73F,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp741=0U;({struct _dyneither_ptr _tmpB99=({const char*_tmp742="region decl at toplevel";_tag_dyneither(_tmp742,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB99,_tag_dyneither(_tmp741,sizeof(void*),0U));});});case 5U: _LLB: _tmp74C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp738)->f1;_LLC:
# 4559
 Cyc_Toc_aggrdecl_to_c(_tmp74C,1);
goto _LL0;case 6U: _LLD: _tmp74D=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp738)->f1;_LLE:
# 4562
 if(_tmp74D->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp74D);else{
# 4565
Cyc_Toc_datatypedecl_to_c(_tmp74D);}
goto _LL0;case 7U: _LLF: _tmp74E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp738)->f1;_LL10:
# 4568
 Cyc_Toc_enumdecl_to_c(_tmp74E);
({struct Cyc_List_List*_tmpB9A=({struct Cyc_List_List*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743->hd=d,_tmp743->tl=Cyc_Toc_result_decls;_tmp743;});Cyc_Toc_result_decls=_tmpB9A;});
goto _LL0;case 8U: _LL11: _tmp74F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp738)->f1;_LL12:
# 4572
 _tmp74F->tvs=0;
if(_tmp74F->defn != 0){
void*_tmp744=Cyc_Toc_typ_to_c((void*)_check_null(_tmp74F->defn));
_tmp74F->defn=_tmp744;{
# 4578
struct Cyc_Absyn_Decl*_tmp745=Cyc_Toc_type2enumdecl(_tmp744);
if(_tmp745 != 0){
({struct Cyc_List_List*_tmpB9B=({struct Cyc_List_List*_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746->hd=_tmp745,_tmp746->tl=Cyc_Toc_result_decls;_tmp746;});Cyc_Toc_result_decls=_tmpB9B;});
({void*_tmpB9C=Cyc_Toc_decl_to_enumtype(_tmp745);_tmp74F->defn=_tmpB9C;});}};}else{
# 4584
enum Cyc_Absyn_KindQual _tmp747=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp74F->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp748=_tmp747;if(_tmp748 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpB9D=Cyc_Absyn_void_star_type();_tmp74F->defn=_tmpB9D;});goto _LL1F;}else{_LL22: _LL23:
 _tmp74F->defn=Cyc_Absyn_void_type;goto _LL1F;}_LL1F:;}
# 4592
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpB9E=({struct Cyc_List_List*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749->hd=d,_tmp749->tl=Cyc_Toc_result_decls;_tmp749;});Cyc_Toc_result_decls=_tmpB9E;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp750=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp738)->f2;_LL18:
 _tmp751=_tmp750;goto _LL1A;case 10U: _LL19: _tmp751=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp738)->f2;_LL1A:
 _tmp752=_tmp751;goto _LL1C;case 11U: _LL1B: _tmp752=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp738)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpB9F=Cyc_Toc_decls_to_c(r,nv,_tmp752,top,cinclude);nv=_tmpB9F;});goto _LL0;default: _LL1D: _tmp753=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp738)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpBA0=Cyc_Toc_decls_to_c(r,nv,_tmp753,top,1);nv=_tmpBA0;});goto _LL0;}_LL0:;};}
# 4603
return nv;}
# 4607
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp754=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp755=_tmp754;struct Cyc_Core_DynamicRegion*_tmp75A;_LL1: _tmp75A=_tmp755.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp75A->h;
({struct Cyc_Toc_TocState*_tmpBA1=Cyc_Toc_empty_toc_state(h);ts=_tmpBA1;});;}
({struct Cyc_Toc_TocStateWrap*_tmpBA2=({struct Cyc_Toc_TocStateWrap*_tmp757=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp757));_tmp757->dyn=_tmp75A,_tmp757->state=ts;_tmp757;});Cyc_Toc_toc_state=_tmpBA2;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpBA3=_tag_dyneither(({unsigned int _tmp759=35;struct _dyneither_ptr**_tmp758=_cycalloc(_check_times(_tmp759,sizeof(struct _dyneither_ptr*)));_tmp758[0]=& Cyc_Toc__throw_str,_tmp758[1]=& Cyc_Toc_setjmp_str,_tmp758[2]=& Cyc_Toc__push_handler_str,_tmp758[3]=& Cyc_Toc__pop_handler_str,_tmp758[4]=& Cyc_Toc__exn_thrown_str,_tmp758[5]=& Cyc_Toc__npop_handler_str,_tmp758[6]=& Cyc_Toc__check_null_str,_tmp758[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp758[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp758[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp758[10]=& Cyc_Toc__dyneither_ptr_str,_tmp758[11]=& Cyc_Toc__tag_dyneither_str,_tmp758[12]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp758[13]=& Cyc_Toc__get_dyneither_size_str,_tmp758[14]=& Cyc_Toc__get_zero_arr_size_str,_tmp758[15]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp758[16]=& Cyc_Toc__zero_arr_plus_str,_tmp758[17]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp758[18]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp758[19]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp758[20]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp758[21]=& Cyc_Toc__cycalloc_str,_tmp758[22]=& Cyc_Toc__cyccalloc_str,_tmp758[23]=& Cyc_Toc__cycalloc_atomic_str,_tmp758[24]=& Cyc_Toc__cyccalloc_atomic_str,_tmp758[25]=& Cyc_Toc__region_malloc_str,_tmp758[26]=& Cyc_Toc__region_calloc_str,_tmp758[27]=& Cyc_Toc__check_times_str,_tmp758[28]=& Cyc_Toc__new_region_str,_tmp758[29]=& Cyc_Toc__push_region_str,_tmp758[30]=& Cyc_Toc__pop_region_str,_tmp758[31]=& Cyc_Toc__throw_arraybounds_str,_tmp758[32]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp758[33]=& Cyc_Toc__throw_match_str,_tmp758[34]=& Cyc_Toc__fast_region_malloc_str;_tmp758;}),sizeof(struct _dyneither_ptr*),35U);Cyc_Toc_globals=_tmpBA3;});};}
# 4658
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp75B=ts;struct Cyc_Toc_TocStateWrap*_tmp75C=Cyc_Toc_toc_state;ts=_tmp75C;Cyc_Toc_toc_state=_tmp75B;});{
struct Cyc_Toc_TocStateWrap _tmp75D=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp75E=_tmp75D;struct Cyc_Core_DynamicRegion*_tmp764;struct Cyc_Toc_TocState*_tmp763;_LL1: _tmp764=_tmp75E.dyn;_tmp763=_tmp75E.state;_LL2:;
# 4663
{struct _RegionHandle*h=& _tmp764->h;
{struct Cyc_Toc_TocState _tmp760=*_tmp763;struct Cyc_Toc_TocState _tmp761=_tmp760;struct Cyc_Xarray_Xarray*_tmp762;_LL4: _tmp762=_tmp761.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp762);}
# 4664
;}
# 4667
Cyc_Core_free_rckey(_tmp764);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4670
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4676
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4678
({struct Cyc_Hashtable_Table**_tmpBA4=({struct Cyc_Hashtable_Table**_tmp765=_cycalloc(sizeof(*_tmp765));*_tmp765=pop_tables;_tmp765;});Cyc_Toc_gpop_tables=_tmpBA4;});
Cyc_Toc_init();
{struct _RegionHandle _tmp766=_new_region("start");struct _RegionHandle*start=& _tmp766;_push_region(start);
({struct _RegionHandle*_tmpBA6=start;struct Cyc_Toc_Env*_tmpBA5=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpBA6,_tmpBA5,ds,1,0);});;_pop_region(start);}
# 4687
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}

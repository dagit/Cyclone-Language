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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Buffer_t;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 258
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 387
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 916
void*Cyc_Absyn_compress_kb(void*);
# 922
int Cyc_Absyn_type2bool(int def,void*);
# 927
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
# 1166
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1172
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1180
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 115 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 337 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 339
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 51 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 53
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
# 61
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long);
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*);
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p);
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 76
int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 86
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 38 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*requires,struct Cyc_Absyn_Exp*ensures);
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 66
static int Cyc_Absynpp_expand_typedefs;
# 70
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4U]="Cyc";
# 72
struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4U};
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 75
static int Cyc_Absynpp_add_cyc_prefix;
# 79
static int Cyc_Absynpp_to_VC;
# 82
static int Cyc_Absynpp_decls_first;
# 86
static int Cyc_Absynpp_rewrite_temp_tvars;
# 89
static int Cyc_Absynpp_print_all_tvars;
# 92
static int Cyc_Absynpp_print_all_kinds;
# 95
static int Cyc_Absynpp_print_all_effects;
# 98
static int Cyc_Absynpp_print_using_stmts;
# 103
static int Cyc_Absynpp_print_externC_stmts;
# 107
static int Cyc_Absynpp_print_full_evars;
# 110
static int Cyc_Absynpp_generate_line_directives;
# 113
static int Cyc_Absynpp_use_curr_namespace;
# 116
static int Cyc_Absynpp_print_zeroterm;
# 119
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 122
int Cyc_Absynpp_print_for_cycdoc=0;
# 143
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absynpp_to_VC=fs->to_VC;
Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}
# 162
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 182
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 202
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 222
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 243
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp550=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp550,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v,_tmp1->tl=0;_tmp1;}));});}
# 247
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 251
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 255
static void Cyc_Absynpp_curr_namespace_drop(){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 259
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){
char _tmp2=c;switch(_tmp2){case 7U: _LL1: _LL2:
 return({const char*_tmp3="\\a";_tag_dyneither(_tmp3,sizeof(char),3U);});case 8U: _LL3: _LL4:
 return({const char*_tmp4="\\b";_tag_dyneither(_tmp4,sizeof(char),3U);});case 12U: _LL5: _LL6:
 return({const char*_tmp5="\\f";_tag_dyneither(_tmp5,sizeof(char),3U);});case 10U: _LL7: _LL8:
 return({const char*_tmp6="\\n";_tag_dyneither(_tmp6,sizeof(char),3U);});case 13U: _LL9: _LLA:
 return({const char*_tmp7="\\r";_tag_dyneither(_tmp7,sizeof(char),3U);});case 9U: _LLB: _LLC:
 return({const char*_tmp8="\\t";_tag_dyneither(_tmp8,sizeof(char),3U);});case 11U: _LLD: _LLE:
 return({const char*_tmp9="\\v";_tag_dyneither(_tmp9,sizeof(char),3U);});case 92U: _LLF: _LL10:
 return({const char*_tmpA="\\\\";_tag_dyneither(_tmpA,sizeof(char),3U);});case 34U: _LL11: _LL12:
 return({const char*_tmpB="\"";_tag_dyneither(_tmpB,sizeof(char),2U);});case 39U: _LL13: _LL14:
 return({const char*_tmpC="\\'";_tag_dyneither(_tmpC,sizeof(char),3U);});default: _LL15: _LL16:
# 272
 if((int)c >= (int)' '  && (int)c <= (int)'~'){
struct _dyneither_ptr _tmpD=Cyc_Core_new_string(2U);
({struct _dyneither_ptr _tmpE=_dyneither_ptr_plus(_tmpD,sizeof(char),0);char _tmpF=*((char*)_check_dyneither_subscript(_tmpE,sizeof(char),0U));char _tmp10=c;if(_get_dyneither_size(_tmpE,sizeof(char))== 1U  && (_tmpF == 0  && _tmp10 != 0))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _dyneither_ptr)_tmpD;}else{
# 277
struct _dyneither_ptr _tmp11=Cyc_Core_new_string(5U);
int j=0;
({struct _dyneither_ptr _tmp12=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp13=*((char*)_check_dyneither_subscript(_tmp12,sizeof(char),0U));char _tmp14='\\';if(_get_dyneither_size(_tmp12,sizeof(char))== 1U  && (_tmp13 == 0  && _tmp14 != 0))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _dyneither_ptr _tmp15=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp16=*((char*)_check_dyneither_subscript(_tmp15,sizeof(char),0U));char _tmp17=(char)((int)'0' + ((int)((unsigned char)c)>> 6 & 3));if(_get_dyneither_size(_tmp15,sizeof(char))== 1U  && (_tmp16 == 0  && _tmp17 != 0))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _dyneither_ptr _tmp18=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp19=*((char*)_check_dyneither_subscript(_tmp18,sizeof(char),0U));char _tmp1A=(char)((int)'0' + ((int)c >> 3 & 7));if(_get_dyneither_size(_tmp18,sizeof(char))== 1U  && (_tmp19 == 0  && _tmp1A != 0))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _dyneither_ptr _tmp1B=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp1C=*((char*)_check_dyneither_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=(char)((int)'0' + ((int)c & 7));if(_get_dyneither_size(_tmp1B,sizeof(char))== 1U  && (_tmp1C == 0  && _tmp1D != 0))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
return(struct _dyneither_ptr)_tmp11;}}_LL0:;}
# 288
static int Cyc_Absynpp_special(struct _dyneither_ptr s){
int sz=(int)(_get_dyneither_size(s,sizeof(char))- (unsigned int)1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if((((int)c <= (int)' '  || (int)c >= (int)'~') || (int)c == (int)'"') || (int)c == (int)'\\')
return 1;}}
# 295
return 0;}
# 298
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 301
int n=(int)(_get_dyneither_size(s,sizeof(char))- (unsigned int)1);
# 303
if(n > 0  && (int)*((const char*)_check_dyneither_subscript(s,sizeof(char),n))== (int)'\000')-- n;{
# 305
int len=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1F=_tmp1E;char _tmp20;switch(_tmp1F){case 7U: _LL1: _LL2:
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL8;case 10U: _LL7: _LL8:
 goto _LLA;case 13U: _LL9: _LLA:
 goto _LLC;case 9U: _LLB: _LLC:
 goto _LLE;case 11U: _LLD: _LLE:
 goto _LL10;case 92U: _LLF: _LL10:
 goto _LL12;case 34U: _LL11: _LL12:
 len +=2;goto _LL0;default: _LL13: _tmp20=_tmp1F;_LL14:
# 318
 if((int)_tmp20 >= (int)' '  && (int)_tmp20 <= (int)'~')++ len;else{
len +=4;}
goto _LL0;}_LL0:;}}{
# 323
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp21=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp22=_tmp21;char _tmp68;switch(_tmp22){case 7U: _LL16: _LL17:
({struct _dyneither_ptr _tmp23=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp24=*((char*)_check_dyneither_subscript(_tmp23,sizeof(char),0U));char _tmp25='\\';if(_get_dyneither_size(_tmp23,sizeof(char))== 1U  && (_tmp24 == 0  && _tmp25 != 0))_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});({struct _dyneither_ptr _tmp26=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp27=*((char*)_check_dyneither_subscript(_tmp26,sizeof(char),0U));char _tmp28='a';if(_get_dyneither_size(_tmp26,sizeof(char))== 1U  && (_tmp27 == 0  && _tmp28 != 0))_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});goto _LL15;case 8U: _LL18: _LL19:
({struct _dyneither_ptr _tmp29=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2A=*((char*)_check_dyneither_subscript(_tmp29,sizeof(char),0U));char _tmp2B='\\';if(_get_dyneither_size(_tmp29,sizeof(char))== 1U  && (_tmp2A == 0  && _tmp2B != 0))_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});({struct _dyneither_ptr _tmp2C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2D=*((char*)_check_dyneither_subscript(_tmp2C,sizeof(char),0U));char _tmp2E='b';if(_get_dyneither_size(_tmp2C,sizeof(char))== 1U  && (_tmp2D == 0  && _tmp2E != 0))_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});goto _LL15;case 12U: _LL1A: _LL1B:
({struct _dyneither_ptr _tmp2F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp30=*((char*)_check_dyneither_subscript(_tmp2F,sizeof(char),0U));char _tmp31='\\';if(_get_dyneither_size(_tmp2F,sizeof(char))== 1U  && (_tmp30 == 0  && _tmp31 != 0))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});({struct _dyneither_ptr _tmp32=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp33=*((char*)_check_dyneither_subscript(_tmp32,sizeof(char),0U));char _tmp34='f';if(_get_dyneither_size(_tmp32,sizeof(char))== 1U  && (_tmp33 == 0  && _tmp34 != 0))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});goto _LL15;case 10U: _LL1C: _LL1D:
({struct _dyneither_ptr _tmp35=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp36=*((char*)_check_dyneither_subscript(_tmp35,sizeof(char),0U));char _tmp37='\\';if(_get_dyneither_size(_tmp35,sizeof(char))== 1U  && (_tmp36 == 0  && _tmp37 != 0))_throw_arraybounds();*((char*)_tmp35.curr)=_tmp37;});({struct _dyneither_ptr _tmp38=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp39=*((char*)_check_dyneither_subscript(_tmp38,sizeof(char),0U));char _tmp3A='n';if(_get_dyneither_size(_tmp38,sizeof(char))== 1U  && (_tmp39 == 0  && _tmp3A != 0))_throw_arraybounds();*((char*)_tmp38.curr)=_tmp3A;});goto _LL15;case 13U: _LL1E: _LL1F:
({struct _dyneither_ptr _tmp3B=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3C=*((char*)_check_dyneither_subscript(_tmp3B,sizeof(char),0U));char _tmp3D='\\';if(_get_dyneither_size(_tmp3B,sizeof(char))== 1U  && (_tmp3C == 0  && _tmp3D != 0))_throw_arraybounds();*((char*)_tmp3B.curr)=_tmp3D;});({struct _dyneither_ptr _tmp3E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3F=*((char*)_check_dyneither_subscript(_tmp3E,sizeof(char),0U));char _tmp40='r';if(_get_dyneither_size(_tmp3E,sizeof(char))== 1U  && (_tmp3F == 0  && _tmp40 != 0))_throw_arraybounds();*((char*)_tmp3E.curr)=_tmp40;});goto _LL15;case 9U: _LL20: _LL21:
({struct _dyneither_ptr _tmp41=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp42=*((char*)_check_dyneither_subscript(_tmp41,sizeof(char),0U));char _tmp43='\\';if(_get_dyneither_size(_tmp41,sizeof(char))== 1U  && (_tmp42 == 0  && _tmp43 != 0))_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});({struct _dyneither_ptr _tmp44=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp45=*((char*)_check_dyneither_subscript(_tmp44,sizeof(char),0U));char _tmp46='t';if(_get_dyneither_size(_tmp44,sizeof(char))== 1U  && (_tmp45 == 0  && _tmp46 != 0))_throw_arraybounds();*((char*)_tmp44.curr)=_tmp46;});goto _LL15;case 11U: _LL22: _LL23:
({struct _dyneither_ptr _tmp47=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp48=*((char*)_check_dyneither_subscript(_tmp47,sizeof(char),0U));char _tmp49='\\';if(_get_dyneither_size(_tmp47,sizeof(char))== 1U  && (_tmp48 == 0  && _tmp49 != 0))_throw_arraybounds();*((char*)_tmp47.curr)=_tmp49;});({struct _dyneither_ptr _tmp4A=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4B=*((char*)_check_dyneither_subscript(_tmp4A,sizeof(char),0U));char _tmp4C='v';if(_get_dyneither_size(_tmp4A,sizeof(char))== 1U  && (_tmp4B == 0  && _tmp4C != 0))_throw_arraybounds();*((char*)_tmp4A.curr)=_tmp4C;});goto _LL15;case 92U: _LL24: _LL25:
({struct _dyneither_ptr _tmp4D=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4E=*((char*)_check_dyneither_subscript(_tmp4D,sizeof(char),0U));char _tmp4F='\\';if(_get_dyneither_size(_tmp4D,sizeof(char))== 1U  && (_tmp4E == 0  && _tmp4F != 0))_throw_arraybounds();*((char*)_tmp4D.curr)=_tmp4F;});({struct _dyneither_ptr _tmp50=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp51=*((char*)_check_dyneither_subscript(_tmp50,sizeof(char),0U));char _tmp52='\\';if(_get_dyneither_size(_tmp50,sizeof(char))== 1U  && (_tmp51 == 0  && _tmp52 != 0))_throw_arraybounds();*((char*)_tmp50.curr)=_tmp52;});goto _LL15;case 34U: _LL26: _LL27:
({struct _dyneither_ptr _tmp53=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp54=*((char*)_check_dyneither_subscript(_tmp53,sizeof(char),0U));char _tmp55='\\';if(_get_dyneither_size(_tmp53,sizeof(char))== 1U  && (_tmp54 == 0  && _tmp55 != 0))_throw_arraybounds();*((char*)_tmp53.curr)=_tmp55;});({struct _dyneither_ptr _tmp56=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp57=*((char*)_check_dyneither_subscript(_tmp56,sizeof(char),0U));char _tmp58='"';if(_get_dyneither_size(_tmp56,sizeof(char))== 1U  && (_tmp57 == 0  && _tmp58 != 0))_throw_arraybounds();*((char*)_tmp56.curr)=_tmp58;});goto _LL15;default: _LL28: _tmp68=_tmp22;_LL29:
# 337
 if((int)_tmp68 >= (int)' '  && (int)_tmp68 <= (int)'~')({struct _dyneither_ptr _tmp59=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5A=*((char*)_check_dyneither_subscript(_tmp59,sizeof(char),0U));char _tmp5B=_tmp68;if(_get_dyneither_size(_tmp59,sizeof(char))== 1U  && (_tmp5A == 0  && _tmp5B != 0))_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;});else{
# 339
({struct _dyneither_ptr _tmp5C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5D=*((char*)_check_dyneither_subscript(_tmp5C,sizeof(char),0U));char _tmp5E='\\';if(_get_dyneither_size(_tmp5C,sizeof(char))== 1U  && (_tmp5D == 0  && _tmp5E != 0))_throw_arraybounds();*((char*)_tmp5C.curr)=_tmp5E;});
({struct _dyneither_ptr _tmp5F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp60=*((char*)_check_dyneither_subscript(_tmp5F,sizeof(char),0U));char _tmp61=(char)((int)'0' + ((int)_tmp68 >> 6 & 7));if(_get_dyneither_size(_tmp5F,sizeof(char))== 1U  && (_tmp60 == 0  && _tmp61 != 0))_throw_arraybounds();*((char*)_tmp5F.curr)=_tmp61;});
({struct _dyneither_ptr _tmp62=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp63=*((char*)_check_dyneither_subscript(_tmp62,sizeof(char),0U));char _tmp64=(char)((int)'0' + ((int)_tmp68 >> 3 & 7));if(_get_dyneither_size(_tmp62,sizeof(char))== 1U  && (_tmp63 == 0  && _tmp64 != 0))_throw_arraybounds();*((char*)_tmp62.curr)=_tmp64;});
({struct _dyneither_ptr _tmp65=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp66=*((char*)_check_dyneither_subscript(_tmp65,sizeof(char),0U));char _tmp67=(char)((int)'0' + ((int)_tmp68 & 7));if(_get_dyneither_size(_tmp65,sizeof(char))== 1U  && (_tmp66 == 0  && _tmp67 != 0))_throw_arraybounds();*((char*)_tmp65.curr)=_tmp67;});}
# 344
goto _LL15;}_LL15:;}}
# 346
return(struct _dyneither_ptr)t;};};};}static char _tmp69[9U]="restrict";
# 349
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp69,_tmp69,_tmp69 + 9U};static char _tmp6A[9U]="volatile";
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp6A,_tmp6A,_tmp6A + 9U};static char _tmp6B[6U]="const";
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp6B,_tmp6B,_tmp6B + 6U};
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 356
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 359
if(tq.q_restrict)l=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=Cyc_Absynpp_restrict_sp,_tmp6C->tl=l;_tmp6C;});
if(tq.q_volatile)l=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_volatile_sp,_tmp6D->tl=l;_tmp6D;});
if(tq.print_const)l=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_const_sp,_tmp6E->tl=l;_tmp6E;});
return({struct _dyneither_ptr _tmp553=({const char*_tmp6F="";_tag_dyneither(_tmp6F,sizeof(char),1U);});struct _dyneither_ptr _tmp552=({const char*_tmp70=" ";_tag_dyneither(_tmp70,sizeof(char),2U);});struct _dyneither_ptr _tmp551=({const char*_tmp71=" ";_tag_dyneither(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp553,_tmp552,_tmp551,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 365
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_KindQual _tmp89;enum Cyc_Absyn_AliasQual _tmp88;_LL1: _tmp89=_tmp72->kind;_tmp88=_tmp72->aliasqual;_LL2:;{
enum Cyc_Absyn_KindQual _tmp73=_tmp89;switch(_tmp73){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 369
enum Cyc_Absyn_AliasQual _tmp74=_tmp88;switch(_tmp74){case Cyc_Absyn_Aliasable: _LL15: _LL16:
 return({const char*_tmp75="A";_tag_dyneither(_tmp75,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL17: _LL18:
 return({const char*_tmp76="UA";_tag_dyneither(_tmp76,sizeof(char),3U);});default: _LL19: _LL1A:
 return({const char*_tmp77="TA";_tag_dyneither(_tmp77,sizeof(char),3U);});}_LL14:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 375
enum Cyc_Absyn_AliasQual _tmp78=_tmp88;switch(_tmp78){case Cyc_Absyn_Aliasable: _LL1C: _LL1D:
 return({const char*_tmp79="M";_tag_dyneither(_tmp79,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL1E: _LL1F:
 return({const char*_tmp7A="UM";_tag_dyneither(_tmp7A,sizeof(char),3U);});default: _LL20: _LL21:
 return({const char*_tmp7B="TM";_tag_dyneither(_tmp7B,sizeof(char),3U);});}_LL1B:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 381
enum Cyc_Absyn_AliasQual _tmp7C=_tmp88;switch(_tmp7C){case Cyc_Absyn_Aliasable: _LL23: _LL24:
 return({const char*_tmp7D="B";_tag_dyneither(_tmp7D,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL25: _LL26:
 return({const char*_tmp7E="UB";_tag_dyneither(_tmp7E,sizeof(char),3U);});default: _LL27: _LL28:
 return({const char*_tmp7F="TB";_tag_dyneither(_tmp7F,sizeof(char),3U);});}_LL22:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 387
enum Cyc_Absyn_AliasQual _tmp80=_tmp88;switch(_tmp80){case Cyc_Absyn_Aliasable: _LL2A: _LL2B:
 return({const char*_tmp81="R";_tag_dyneither(_tmp81,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL2C: _LL2D:
 return({const char*_tmp82="UR";_tag_dyneither(_tmp82,sizeof(char),3U);});default: _LL2E: _LL2F:
 return({const char*_tmp83="TR";_tag_dyneither(_tmp83,sizeof(char),3U);});}_LL29:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 392
 return({const char*_tmp84="E";_tag_dyneither(_tmp84,sizeof(char),2U);});case Cyc_Absyn_IntKind: _LLE: _LLF:
 return({const char*_tmp85="I";_tag_dyneither(_tmp85,sizeof(char),2U);});case Cyc_Absyn_BoolKind: _LL10: _LL11:
 return({const char*_tmp86="BOOL";_tag_dyneither(_tmp86,sizeof(char),5U);});default: _LL12: _LL13:
 return({const char*_tmp87="PTRBND";_tag_dyneither(_tmp87,sizeof(char),7U);});}_LL3:;};}
# 398
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 400
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp8A=Cyc_Absyn_compress_kb(c);void*_tmp8B=_tmp8A;struct Cyc_Absyn_Kind*_tmp92;struct Cyc_Absyn_Kind*_tmp91;switch(*((int*)_tmp8B)){case 0U: _LL1: _tmp91=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B)->f1;_LL2:
 return Cyc_Absynpp_kind2string(_tmp91);case 1U: _LL3: _LL4:
# 404
 if(Cyc_PP_tex_output)
return({const char*_tmp8C="{?}";_tag_dyneither(_tmp8C,sizeof(char),4U);});else{
return({const char*_tmp8D="?";_tag_dyneither(_tmp8D,sizeof(char),2U);});}default: _LL5: _tmp92=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B)->f2;_LL6:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp534;_tmp534.tag=0U,({struct _dyneither_ptr _tmp554=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp92));_tmp534.f1=_tmp554;});_tmp534;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({struct _dyneither_ptr _tmp555=({const char*_tmp8F="<=%s";_tag_dyneither(_tmp8F,sizeof(char),5U);});Cyc_aprintf(_tmp555,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});}_LL0:;}
# 411
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp93=Cyc_Absyn_compress_kb(c);void*_tmp94=_tmp93;struct Cyc_Absyn_Kind*_tmp98;struct Cyc_Absyn_Kind*_tmp97;switch(*((int*)_tmp94)){case 0U: _LL1: _tmp97=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL2:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp97));case 1U: _LL3: _LL4:
# 415
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp95="{?}";_tag_dyneither(_tmp95,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp96="?";_tag_dyneither(_tmp96,sizeof(char),2U);}));}default: _LL5: _tmp98=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;_LL6:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp98));}_LL0:;}
# 422
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp558=({const char*_tmp99="<";_tag_dyneither(_tmp99,sizeof(char),2U);});struct _dyneither_ptr _tmp557=({const char*_tmp9A=">";_tag_dyneither(_tmp9A,sizeof(char),2U);});struct _dyneither_ptr _tmp556=({const char*_tmp9B=",";_tag_dyneither(_tmp9B,sizeof(char),2U);});Cyc_PP_egroup(_tmp558,_tmp557,_tmp556,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 426
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9C=*tv->name;
# 430
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 432
struct _dyneither_ptr kstring=({const char*_tmpA5="K";_tag_dyneither(_tmpA5,sizeof(char),2U);});
{void*_tmp9D=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9E=_tmp9D;struct Cyc_Absyn_Kind*_tmpA0;struct Cyc_Absyn_Kind*_tmp9F;switch(*((int*)_tmp9E)){case 2U: _LL1: _tmp9F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f2;_LL2:
 _tmpA0=_tmp9F;goto _LL4;case 0U: _LL3: _tmpA0=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9E)->f1;_LL4:
 kstring=Cyc_Absynpp_kind2string(_tmpA0);goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 438
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp536;_tmp536.tag=0U,_tmp536.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring);_tmp536;});struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0U,({struct _dyneither_ptr _tmp559=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9C,sizeof(char),1));_tmp535.f1=_tmp559;});_tmp535;});void*_tmpA1[2U];_tmpA1[0]=& _tmpA3,_tmpA1[1]=& _tmpA4;({struct _dyneither_ptr _tmp55A=({const char*_tmpA2="`G%s%s";_tag_dyneither(_tmpA2,sizeof(char),7U);});Cyc_aprintf(_tmp55A,_tag_dyneither(_tmpA1,sizeof(void*),2U));});}));}
# 440
return Cyc_PP_text(_tmp9C);}
# 443
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA7=_tmpA6;struct Cyc_Absyn_Kind*_tmpAB;struct Cyc_Absyn_Kind*_tmpAA;switch(*((int*)_tmpA7)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpAA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1;_LL8:
# 448
 return({struct Cyc_PP_Doc*_tmpA8[3U];({struct Cyc_PP_Doc*_tmp55D=Cyc_Absynpp_tvar2doc(tv);_tmpA8[0]=_tmp55D;}),({struct Cyc_PP_Doc*_tmp55C=Cyc_PP_text(({const char*_tmpA9="::";_tag_dyneither(_tmpA9,sizeof(char),3U);}));_tmpA8[1]=_tmp55C;}),({struct Cyc_PP_Doc*_tmp55B=Cyc_Absynpp_kind2doc(_tmpAA);_tmpA8[2]=_tmp55B;});Cyc_PP_cat(_tag_dyneither(_tmpA8,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL5: _tmpAB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7)->f2;_LL6:
# 447
 _tmpAA=_tmpAB;goto _LL8;}_LL0:;}
# 452
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _dyneither_ptr _tmp560=({const char*_tmpAC="<";_tag_dyneither(_tmpAC,sizeof(char),2U);});struct _dyneither_ptr _tmp55F=({const char*_tmpAD=">";_tag_dyneither(_tmpAD,sizeof(char),2U);});struct _dyneither_ptr _tmp55E=({const char*_tmpAE=",";_tag_dyneither(_tmpAE,sizeof(char),2U);});Cyc_PP_egroup(_tmp560,_tmp55F,_tmp55E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 456
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _dyneither_ptr _tmp563=({const char*_tmpAF="<";_tag_dyneither(_tmpAF,sizeof(char),2U);});struct _dyneither_ptr _tmp562=({const char*_tmpB0=">";_tag_dyneither(_tmpB0,sizeof(char),2U);});struct _dyneither_ptr _tmp561=({const char*_tmpB1=",";_tag_dyneither(_tmpB1,sizeof(char),2U);});Cyc_PP_egroup(_tmp563,_tmp562,_tmp561,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 462
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 466
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp566=({const char*_tmpB2="(";_tag_dyneither(_tmpB2,sizeof(char),2U);});struct _dyneither_ptr _tmp565=({const char*_tmpB3=")";_tag_dyneither(_tmpB3,sizeof(char),2U);});struct _dyneither_ptr _tmp564=({const char*_tmpB4=",";_tag_dyneither(_tmpB4,sizeof(char),2U);});Cyc_PP_group(_tmp566,_tmp565,_tmp564,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
# 470
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB5=att;switch(*((int*)_tmpB5)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();default: _LL7: _LL8:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));}_LL0:;}
# 479
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB6=(void*)atts->hd;void*_tmpB7=_tmpB6;switch(*((int*)_tmpB7)){case 1U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmpB8=" _stdcall ";_tag_dyneither(_tmpB8,sizeof(char),11U);}));case 2U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmpB9=" _cdecl ";_tag_dyneither(_tmpB9,sizeof(char),9U);}));case 3U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmpBA=" _fastcall ";_tag_dyneither(_tmpBA,sizeof(char),12U);}));default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 487
return Cyc_PP_nil_doc();}
# 490
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpBB=(void*)atts2->hd;void*_tmpBC=_tmpBB;switch(*((int*)_tmpBC)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 499
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBD[3U];({struct Cyc_PP_Doc*_tmp56C=Cyc_PP_text(({const char*_tmpBE=" __declspec(";_tag_dyneither(_tmpBE,sizeof(char),13U);}));_tmpBD[0]=_tmp56C;}),({
struct Cyc_PP_Doc*_tmp56B=({struct _dyneither_ptr _tmp56A=({const char*_tmpBF="";_tag_dyneither(_tmpBF,sizeof(char),1U);});struct _dyneither_ptr _tmp569=({const char*_tmpC0="";_tag_dyneither(_tmpC0,sizeof(char),1U);});struct _dyneither_ptr _tmp568=({const char*_tmpC1=" ";_tag_dyneither(_tmpC1,sizeof(char),2U);});Cyc_PP_group(_tmp56A,_tmp569,_tmp568,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBD[1]=_tmp56B;}),({
struct Cyc_PP_Doc*_tmp567=Cyc_PP_text(({const char*_tmpC2=")";_tag_dyneither(_tmpC2,sizeof(char),2U);}));_tmpBD[2]=_tmp567;});Cyc_PP_cat(_tag_dyneither(_tmpBD,sizeof(struct Cyc_PP_Doc*),3U));});}
# 506
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 510
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC3=Cyc_Cyclone_c_compiler;if(_tmpC3 == Cyc_Cyclone_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
 return({struct Cyc_PP_Doc*_tmpC4[2U];({struct Cyc_PP_Doc*_tmp571=Cyc_PP_text(({const char*_tmpC5=" __attribute__";_tag_dyneither(_tmpC5,sizeof(char),15U);}));_tmpC4[0]=_tmp571;}),({
struct Cyc_PP_Doc*_tmp570=({struct _dyneither_ptr _tmp56F=({const char*_tmpC6="((";_tag_dyneither(_tmpC6,sizeof(char),3U);});struct _dyneither_ptr _tmp56E=({const char*_tmpC7="))";_tag_dyneither(_tmpC7,sizeof(char),3U);});struct _dyneither_ptr _tmp56D=({const char*_tmpC8=",";_tag_dyneither(_tmpC8,sizeof(char),2U);});Cyc_PP_group(_tmp56F,_tmp56E,_tmp56D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC4[1]=_tmp570;});Cyc_PP_cat(_tag_dyneither(_tmpC4,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;};}
# 519
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC9=(void*)tms->hd;void*_tmpCA=_tmpC9;switch(*((int*)_tmpCA)){case 2U: _LL1: _LL2:
 return 1;case 5U: _LL3: _LL4: {
# 524
enum Cyc_Cyclone_C_Compilers _tmpCB=Cyc_Cyclone_c_compiler;if(_tmpCB == Cyc_Cyclone_Gcc_c){_LL8: _LL9:
 return 0;}else{_LLA: _LLB:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL7:;}default: _LL5: _LL6:
# 528
 return 0;}_LL0:;};}
# 532
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 535
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmpCC="{?}";_tag_dyneither(_tmpCC,sizeof(char),4U);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmpCD="?";_tag_dyneither(_tmpCD,sizeof(char),2U);}));}}
# 542
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 544
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmpCE="{\\lb}";_tag_dyneither(_tmpCE,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmpCF="{";_tag_dyneither(_tmpCF,sizeof(char),2U);}));}}
# 551
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 553
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmpD0="{\\rb}";_tag_dyneither(_tmpD0,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmpD1="}";_tag_dyneither(_tmpD1,sizeof(char),2U);}));}}
# 560
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 562
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmpD2="\\$";_tag_dyneither(_tmpD2,sizeof(char),3U);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmpD3="$";_tag_dyneither(_tmpD3,sizeof(char),2U);}));}}
# 569
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 571
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpD4[3U];({struct Cyc_PP_Doc*_tmp574=Cyc_Absynpp_lb();_tmpD4[0]=_tmp574;}),({struct Cyc_PP_Doc*_tmp573=Cyc_PP_seq(sep,ss);_tmpD4[1]=_tmp573;}),({struct Cyc_PP_Doc*_tmp572=Cyc_Absynpp_rb();_tmpD4[2]=_tmp572;});Cyc_PP_cat(_tag_dyneither(_tmpD4,sizeof(struct Cyc_PP_Doc*),3U));});}
# 576
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD5=(void*)tms->hd;void*_tmpD6=_tmpD5;struct Cyc_List_List*_tmpED;switch(*((int*)_tmpD6)){case 0U: _LL1: _LL2:
({void*_tmpD7=0U;({struct Cyc___cycFILE*_tmp576=Cyc_stderr;struct _dyneither_ptr _tmp575=({const char*_tmpD8="Carray_mod ";_tag_dyneither(_tmpD8,sizeof(char),12U);});Cyc_fprintf(_tmp576,_tmp575,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmpD9=0U;({struct Cyc___cycFILE*_tmp578=Cyc_stderr;struct _dyneither_ptr _tmp577=({const char*_tmpDA="ConstArray_mod ";_tag_dyneither(_tmpDA,sizeof(char),16U);});Cyc_fprintf(_tmp578,_tmp577,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->tag == 1U){_LL5: _tmpED=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->f1;_LL6:
# 582
({void*_tmpDB=0U;({struct Cyc___cycFILE*_tmp57A=Cyc_stderr;struct _dyneither_ptr _tmp579=({const char*_tmpDC="Function_mod(";_tag_dyneither(_tmpDC,sizeof(char),14U);});Cyc_fprintf(_tmp57A,_tmp579,_tag_dyneither(_tmpDB,sizeof(void*),0U));});});
for(0;_tmpED != 0;_tmpED=_tmpED->tl){
struct _dyneither_ptr*_tmpDD=(*((struct _tuple8*)_tmpED->hd)).f1;
if(_tmpDD == 0)({void*_tmpDE=0U;({struct Cyc___cycFILE*_tmp57C=Cyc_stderr;struct _dyneither_ptr _tmp57B=({const char*_tmpDF="?";_tag_dyneither(_tmpDF,sizeof(char),2U);});Cyc_fprintf(_tmp57C,_tmp57B,_tag_dyneither(_tmpDE,sizeof(void*),0U));});});else{
({void*_tmpE0=0U;({struct Cyc___cycFILE*_tmp57E=Cyc_stderr;struct _dyneither_ptr _tmp57D=*_tmpDD;Cyc_fprintf(_tmp57E,_tmp57D,_tag_dyneither(_tmpE0,sizeof(void*),0U));});});}
if(_tmpED->tl != 0)({void*_tmpE1=0U;({struct Cyc___cycFILE*_tmp580=Cyc_stderr;struct _dyneither_ptr _tmp57F=({const char*_tmpE2=",";_tag_dyneither(_tmpE2,sizeof(char),2U);});Cyc_fprintf(_tmp580,_tmp57F,_tag_dyneither(_tmpE1,sizeof(void*),0U));});});}
# 589
({void*_tmpE3=0U;({struct Cyc___cycFILE*_tmp582=Cyc_stderr;struct _dyneither_ptr _tmp581=({const char*_tmpE4=") ";_tag_dyneither(_tmpE4,sizeof(char),3U);});Cyc_fprintf(_tmp582,_tmp581,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});
goto _LL0;}else{_LL7: _LL8:
# 592
({void*_tmpE5=0U;({struct Cyc___cycFILE*_tmp584=Cyc_stderr;struct _dyneither_ptr _tmp583=({const char*_tmpE6="Function_mod()";_tag_dyneither(_tmpE6,sizeof(char),15U);});Cyc_fprintf(_tmp584,_tmp583,_tag_dyneither(_tmpE5,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmpE7=0U;({struct Cyc___cycFILE*_tmp586=Cyc_stderr;struct _dyneither_ptr _tmp585=({const char*_tmpE8="Attributes_mod ";_tag_dyneither(_tmpE8,sizeof(char),16U);});Cyc_fprintf(_tmp586,_tmp585,_tag_dyneither(_tmpE7,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmpE9=0U;({struct Cyc___cycFILE*_tmp588=Cyc_stderr;struct _dyneither_ptr _tmp587=({const char*_tmpEA="TypeParams_mod ";_tag_dyneither(_tmpEA,sizeof(char),16U);});Cyc_fprintf(_tmp588,_tmp587,_tag_dyneither(_tmpE9,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmpEB=0U;({struct Cyc___cycFILE*_tmp58A=Cyc_stderr;struct _dyneither_ptr _tmp589=({const char*_tmpEC="Pointer_mod ";_tag_dyneither(_tmpEC,sizeof(char),13U);});Cyc_fprintf(_tmp58A,_tmp589,_tag_dyneither(_tmpEB,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 597
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp58C=Cyc_stderr;struct _dyneither_ptr _tmp58B=({const char*_tmpEF="\n";_tag_dyneither(_tmpEF,sizeof(char),2U);});Cyc_fprintf(_tmp58C,_tmp58B,_tag_dyneither(_tmpEE,sizeof(void*),0U));});});}
# 600
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 602
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp12E[3U];({struct Cyc_PP_Doc*_tmp58E=Cyc_PP_text(({const char*_tmp12F="(";_tag_dyneither(_tmp12F,sizeof(char),2U);}));_tmp12E[0]=_tmp58E;}),_tmp12E[1]=rest,({struct Cyc_PP_Doc*_tmp58D=Cyc_PP_text(({const char*_tmp130=")";_tag_dyneither(_tmp130,sizeof(char),2U);}));_tmp12E[2]=_tmp58D;});Cyc_PP_cat(_tag_dyneither(_tmp12E,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmpF0=(void*)tms->hd;void*_tmpF1=_tmpF0;void*_tmp12D;void*_tmp12C;void*_tmp12B;void*_tmp12A;struct Cyc_Absyn_Tqual _tmp129;struct Cyc_List_List*_tmp128;unsigned int _tmp127;int _tmp126;struct Cyc_List_List*_tmp125;void*_tmp124;struct Cyc_Absyn_Exp*_tmp123;void*_tmp122;void*_tmp121;switch(*((int*)_tmpF1)){case 0U: _LL1: _tmp121=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL2:
# 608
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF2[2U];_tmpF2[0]=rest,Cyc_Absyn_type2bool(0,_tmp121)?({
struct Cyc_PP_Doc*_tmp590=Cyc_PP_text(({const char*_tmpF3="[]@zeroterm";_tag_dyneither(_tmpF3,sizeof(char),12U);}));_tmpF2[1]=_tmp590;}):({struct Cyc_PP_Doc*_tmp58F=Cyc_PP_text(({const char*_tmpF4="[]";_tag_dyneither(_tmpF4,sizeof(char),3U);}));_tmpF2[1]=_tmp58F;});Cyc_PP_cat(_tag_dyneither(_tmpF2,sizeof(struct Cyc_PP_Doc*),2U));});case 1U: _LL3: _tmp123=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmp122=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL4:
# 612
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF5[4U];_tmpF5[0]=rest,({struct Cyc_PP_Doc*_tmp594=Cyc_PP_text(({const char*_tmpF6="[";_tag_dyneither(_tmpF6,sizeof(char),2U);}));_tmpF5[1]=_tmp594;}),({struct Cyc_PP_Doc*_tmp593=Cyc_Absynpp_exp2doc(_tmp123);_tmpF5[2]=_tmp593;}),
Cyc_Absyn_type2bool(0,_tmp122)?({struct Cyc_PP_Doc*_tmp592=Cyc_PP_text(({const char*_tmpF7="]@zeroterm";_tag_dyneither(_tmpF7,sizeof(char),11U);}));_tmpF5[3]=_tmp592;}):({struct Cyc_PP_Doc*_tmp591=Cyc_PP_text(({const char*_tmpF8="]";_tag_dyneither(_tmpF8,sizeof(char),2U);}));_tmpF5[3]=_tmp591;});Cyc_PP_cat(_tag_dyneither(_tmpF5,sizeof(struct Cyc_PP_Doc*),4U));});case 3U: _LL5: _tmp124=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL6:
# 616
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmpF9=_tmp124;struct Cyc_List_List*_tmp107;unsigned int _tmp106;struct Cyc_List_List*_tmp105;int _tmp104;struct Cyc_Absyn_VarargInfo*_tmp103;void*_tmp102;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->tag == 1U){_LLE: _tmp105=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f1;_tmp104=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f2;_tmp103=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f3;_tmp102=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f4;_tmp101=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f5;_tmp100=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f6;_tmpFF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f7;_LLF:
# 619
 return({struct Cyc_PP_Doc*_tmpFA[2U];_tmpFA[0]=rest,({struct Cyc_PP_Doc*_tmp595=Cyc_Absynpp_funargs2doc(_tmp105,_tmp104,_tmp103,_tmp102,_tmp101,_tmp100,_tmpFF);_tmpFA[1]=_tmp595;});Cyc_PP_cat(_tag_dyneither(_tmpFA,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL10: _tmp107=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF9)->f1;_tmp106=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF9)->f2;_LL11:
# 621
 return({struct Cyc_PP_Doc*_tmpFB[2U];_tmpFB[0]=rest,({struct Cyc_PP_Doc*_tmp599=({struct _dyneither_ptr _tmp598=({const char*_tmpFC="(";_tag_dyneither(_tmpFC,sizeof(char),2U);});struct _dyneither_ptr _tmp597=({const char*_tmpFD=")";_tag_dyneither(_tmpFD,sizeof(char),2U);});struct _dyneither_ptr _tmp596=({const char*_tmpFE=",";_tag_dyneither(_tmpFE,sizeof(char),2U);});Cyc_PP_group(_tmp598,_tmp597,_tmp596,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp107));});_tmpFB[1]=_tmp599;});Cyc_PP_cat(_tag_dyneither(_tmpFB,sizeof(struct Cyc_PP_Doc*),2U));});}_LLD:;};case 5U: _LL7: _tmp125=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL8: {
# 624
enum Cyc_Cyclone_C_Compilers _tmp108=Cyc_Cyclone_c_compiler;if(_tmp108 == Cyc_Cyclone_Gcc_c){_LL13: _LL14:
# 626
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp109[2U];_tmp109[0]=rest,({struct Cyc_PP_Doc*_tmp59A=Cyc_Absynpp_atts2doc(_tmp125);_tmp109[1]=_tmp59A;});Cyc_PP_cat(_tag_dyneither(_tmp109,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 629
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp10A[2U];({struct Cyc_PP_Doc*_tmp59B=Cyc_Absynpp_callconv2doc(_tmp125);_tmp10A[0]=_tmp59B;}),_tmp10A[1]=rest;Cyc_PP_cat(_tag_dyneither(_tmp10A,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmp128=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmp127=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_tmp126=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f3;_LLA:
# 634
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp126)
return({struct Cyc_PP_Doc*_tmp10B[2U];_tmp10B[0]=rest,({struct Cyc_PP_Doc*_tmp59C=Cyc_Absynpp_ktvars2doc(_tmp128);_tmp10B[1]=_tmp59C;});Cyc_PP_cat(_tag_dyneither(_tmp10B,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 638
return({struct Cyc_PP_Doc*_tmp10C[2U];_tmp10C[0]=rest,({struct Cyc_PP_Doc*_tmp59D=Cyc_Absynpp_tvars2doc(_tmp128);_tmp10C[1]=_tmp59D;});Cyc_PP_cat(_tag_dyneither(_tmp10C,sizeof(struct Cyc_PP_Doc*),2U));});}default: _LLB: _tmp12D=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).rgn;_tmp12C=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).nullable;_tmp12B=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).bounds;_tmp12A=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).zero_term;_tmp129=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LLC: {
# 642
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp129);
{void*_tmp10D=Cyc_Tcutil_compress(_tmp12B);void*_tmp10E=_tmp10D;struct Cyc_Absyn_Exp*_tmp119;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)){case 14U: _LL18: _LL19:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2 != 0){if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2)->hd)->tag == 9U){_LL1A: _tmp119=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2)->hd)->f1;_LL1B:
# 650
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp12C)?({const char*_tmp10F="*";_tag_dyneither(_tmp10F,sizeof(char),2U);}):({const char*_tmp110="@";_tag_dyneither(_tmp110,sizeof(char),2U);}));{
struct _tuple11 _tmp111=Cyc_Evexp_eval_const_uint_exp(_tmp119);struct _tuple11 _tmp112=_tmp111;unsigned int _tmp115;int _tmp114;_LL1F: _tmp115=_tmp112.f1;_tmp114=_tmp112.f2;_LL20:;
if(!_tmp114  || _tmp115 != (unsigned int)1)
ptr=({struct Cyc_PP_Doc*_tmp113[4U];_tmp113[0]=ptr,({struct Cyc_PP_Doc*_tmp5A0=Cyc_Absynpp_lb();_tmp113[1]=_tmp5A0;}),({struct Cyc_PP_Doc*_tmp59F=Cyc_Absynpp_exp2doc(_tmp119);_tmp113[2]=_tmp59F;}),({struct Cyc_PP_Doc*_tmp59E=Cyc_Absynpp_rb();_tmp113[3]=_tmp59E;});Cyc_PP_cat(_tag_dyneither(_tmp113,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;};}else{goto _LL1C;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 656
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp12C)?({const char*_tmp116="*";_tag_dyneither(_tmp116,sizeof(char),2U);}):({const char*_tmp117="@";_tag_dyneither(_tmp117,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp118[4U];_tmp118[0]=ptr,({struct Cyc_PP_Doc*_tmp5A3=Cyc_Absynpp_lb();_tmp118[1]=_tmp5A3;}),({struct Cyc_PP_Doc*_tmp5A2=Cyc_Absynpp_typ2doc(_tmp12B);_tmp118[2]=_tmp5A2;}),({struct Cyc_PP_Doc*_tmp5A1=Cyc_Absynpp_rb();_tmp118[3]=_tmp5A1;});Cyc_PP_cat(_tag_dyneither(_tmp118,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;}_LL17:;}
# 661
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && Cyc_Absyn_type2bool(0,_tmp12A))
ztd=Cyc_PP_text(({const char*_tmp11A="@zeroterm";_tag_dyneither(_tmp11A,sizeof(char),10U);}));else{
if(is_char_ptr  && !Cyc_Absyn_type2bool(0,_tmp12A))
ztd=Cyc_PP_text(({const char*_tmp11B="@nozeroterm";_tag_dyneither(_tmp11B,sizeof(char),12U);}));}}
# 667
{void*_tmp11C=Cyc_Tcutil_compress(_tmp12D);void*_tmp11D=_tmp11C;switch(*((int*)_tmp11D)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11D)->f1)->tag == 5U){_LL22: _LL23:
 goto _LL21;}else{goto _LL26;}case 1U: _LL24: if(Cyc_Absynpp_print_for_cycdoc){_LL25:
 goto _LL21;}else{goto _LL26;}default: _LL26: _LL27:
 rgd=Cyc_Absynpp_rgn2doc(_tmp12D);}_LL21:;}{
# 672
struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(({const char*_tmp120=" ";_tag_dyneither(_tmp120,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp11F=" ";_tag_dyneither(_tmp11F,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp11E[7U];_tmp11E[0]=ptr,_tmp11E[1]=ztd,_tmp11E[2]=rgd,_tmp11E[3]=spacer1,_tmp11E[4]=tqd,_tmp11E[5]=spacer2,_tmp11E[6]=rest;Cyc_PP_cat(_tag_dyneither(_tmp11E,sizeof(struct Cyc_PP_Doc*),7U));});};}}_LL0:;};}
# 678
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp131=Cyc_Tcutil_compress(t);void*_tmp132=_tmp131;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp132)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp132)->f1)){case 5U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp133="`H";_tag_dyneither(_tmp133,sizeof(char),3U);}));case 6U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp134="`U";_tag_dyneither(_tmp134,sizeof(char),3U);}));case 7U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp135="`RC";_tag_dyneither(_tmp135,sizeof(char),4U);}));default: goto _LL7;}else{_LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 688
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 692
void*_tmp136=Cyc_Tcutil_compress(t);void*_tmp137=_tmp136;struct Cyc_List_List*_tmp13B;void*_tmp13A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2 != 0){_LL1: _tmp13A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2)->hd;_LL2:
({struct Cyc_List_List*_tmp5A5=({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));({struct Cyc_PP_Doc*_tmp5A4=Cyc_Absynpp_rgn2doc(_tmp13A);_tmp138->hd=_tmp5A4;}),_tmp138->tl=*rgions;_tmp138;});*rgions=_tmp5A5;});goto _LL0;}else{goto _LL5;}case 9U: _LL3: _tmp13B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2;_LL4:
# 695
 for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp13B->hd);}
# 698
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp5A7=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));({struct Cyc_PP_Doc*_tmp5A6=Cyc_Absynpp_typ2doc(t);_tmp139->hd=_tmp5A6;}),_tmp139->tl=*effects;_tmp139;});*effects=_tmp5A7;});goto _LL0;}_LL0:;}
# 703
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0)
return({struct _dyneither_ptr _tmp5AA=({const char*_tmp13C="";_tag_dyneither(_tmp13C,sizeof(char),1U);});struct _dyneither_ptr _tmp5A9=({const char*_tmp13D="";_tag_dyneither(_tmp13D,sizeof(char),1U);});struct _dyneither_ptr _tmp5A8=({const char*_tmp13E="+";_tag_dyneither(_tmp13E,sizeof(char),2U);});Cyc_PP_group(_tmp5AA,_tmp5A9,_tmp5A8,effects);});else{
# 711
struct Cyc_PP_Doc*_tmp13F=({struct _dyneither_ptr _tmp5AB=({const char*_tmp144=",";_tag_dyneither(_tmp144,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp5AB,rgions);});
return({struct _dyneither_ptr _tmp5AF=({const char*_tmp140="";_tag_dyneither(_tmp140,sizeof(char),1U);});struct _dyneither_ptr _tmp5AE=({const char*_tmp141="";_tag_dyneither(_tmp141,sizeof(char),1U);});struct _dyneither_ptr _tmp5AD=({const char*_tmp142="+";_tag_dyneither(_tmp142,sizeof(char),2U);});Cyc_PP_group(_tmp5AF,_tmp5AE,_tmp5AD,({struct Cyc_List_List*_tmp5AC=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5AC,({struct Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=_tmp13F,_tmp143->tl=0;_tmp143;}));}));});}}
# 716
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp145=k;if(_tmp145 == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp146="struct ";_tag_dyneither(_tmp146,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp147="union ";_tag_dyneither(_tmp147,sizeof(char),7U);}));}_LL0:;}
# 724
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp148=t;struct Cyc_Absyn_Datatypedecl*_tmp1BD;struct Cyc_Absyn_Enumdecl*_tmp1BC;struct Cyc_Absyn_Aggrdecl*_tmp1BB;struct _tuple0*_tmp1BA;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Typedefdecl*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;enum Cyc_Absyn_AggrKind _tmp1B5;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_Tvar*_tmp1B2;struct Cyc_Core_Opt*_tmp1B1;void*_tmp1B0;int _tmp1AF;struct Cyc_Core_Opt*_tmp1AE;void*_tmp1AD;void*_tmp1AC;void*_tmp1AB;void*_tmp1AA;struct _dyneither_ptr _tmp1A9;struct _tuple0*_tmp1A8;struct Cyc_List_List*_tmp1A7;union Cyc_Absyn_AggrInfo _tmp1A6;struct Cyc_List_List*_tmp1A5;int _tmp1A4;enum Cyc_Absyn_Sign _tmp1A3;enum Cyc_Absyn_Size_of _tmp1A2;union Cyc_Absyn_DatatypeFieldInfo _tmp1A1;struct Cyc_List_List*_tmp1A0;union Cyc_Absyn_DatatypeInfo _tmp19F;struct Cyc_List_List*_tmp19E;switch(*((int*)_tmp148)){case 4U: _LL1: _LL2:
# 728
 return Cyc_PP_text(({const char*_tmp149="[[[array]]]";_tag_dyneither(_tmp149,sizeof(char),12U);}));case 5U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)){case 0U: _LL7: _LL8:
# 732
 s=Cyc_PP_text(({const char*_tmp14A="void";_tag_dyneither(_tmp14A,sizeof(char),5U);}));goto _LL0;case 18U: _LLD: _tmp19F=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_tmp19E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2;_LLE:
# 763 "absynpp.cyc"
{union Cyc_Absyn_DatatypeInfo _tmp152=_tmp19F;struct _tuple0*_tmp15B;int _tmp15A;struct _tuple0*_tmp159;int _tmp158;if((_tmp152.UnknownDatatype).tag == 1){_LL48: _tmp159=((_tmp152.UnknownDatatype).val).name;_tmp158=((_tmp152.UnknownDatatype).val).is_extensible;_LL49:
# 765
 _tmp15B=_tmp159;_tmp15A=_tmp158;goto _LL4B;}else{_LL4A: _tmp15B=(*(_tmp152.KnownDatatype).val)->name;_tmp15A=(*(_tmp152.KnownDatatype).val)->is_extensible;_LL4B: {
# 767
struct Cyc_PP_Doc*_tmp153=Cyc_PP_text(({const char*_tmp157="datatype ";_tag_dyneither(_tmp157,sizeof(char),10U);}));
struct Cyc_PP_Doc*_tmp154=_tmp15A?Cyc_PP_text(({const char*_tmp156="@extensible ";_tag_dyneither(_tmp156,sizeof(char),13U);})): Cyc_PP_nil_doc();
s=({struct Cyc_PP_Doc*_tmp155[4U];_tmp155[0]=_tmp154,_tmp155[1]=_tmp153,({struct Cyc_PP_Doc*_tmp5B1=Cyc_Absynpp_qvar2doc(_tmp15B);_tmp155[2]=_tmp5B1;}),({struct Cyc_PP_Doc*_tmp5B0=Cyc_Absynpp_tps2doc(_tmp19E);_tmp155[3]=_tmp5B0;});Cyc_PP_cat(_tag_dyneither(_tmp155,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 772
goto _LL0;case 19U: _LLF: _tmp1A1=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_tmp1A0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2;_LL10:
# 774
{union Cyc_Absyn_DatatypeFieldInfo _tmp15C=_tmp1A1;struct _tuple0*_tmp167;int _tmp166;struct _tuple0*_tmp165;struct _tuple0*_tmp164;struct _tuple0*_tmp163;int _tmp162;if((_tmp15C.UnknownDatatypefield).tag == 1){_LL4D: _tmp164=((_tmp15C.UnknownDatatypefield).val).datatype_name;_tmp163=((_tmp15C.UnknownDatatypefield).val).field_name;_tmp162=((_tmp15C.UnknownDatatypefield).val).is_extensible;_LL4E:
# 776
 _tmp167=_tmp164;_tmp166=_tmp162;_tmp165=_tmp163;goto _LL50;}else{_LL4F: _tmp167=(((_tmp15C.KnownDatatypefield).val).f1)->name;_tmp166=(((_tmp15C.KnownDatatypefield).val).f1)->is_extensible;_tmp165=(((_tmp15C.KnownDatatypefield).val).f2)->name;_LL50: {
# 779
struct Cyc_PP_Doc*_tmp15D=Cyc_PP_text(_tmp166?({const char*_tmp160="@extensible datatype ";_tag_dyneither(_tmp160,sizeof(char),22U);}):({const char*_tmp161="datatype ";_tag_dyneither(_tmp161,sizeof(char),10U);}));
s=({struct Cyc_PP_Doc*_tmp15E[4U];_tmp15E[0]=_tmp15D,({struct Cyc_PP_Doc*_tmp5B4=Cyc_Absynpp_qvar2doc(_tmp167);_tmp15E[1]=_tmp5B4;}),({struct Cyc_PP_Doc*_tmp5B3=Cyc_PP_text(({const char*_tmp15F=".";_tag_dyneither(_tmp15F,sizeof(char),2U);}));_tmp15E[2]=_tmp5B3;}),({struct Cyc_PP_Doc*_tmp5B2=Cyc_Absynpp_qvar2doc(_tmp165);_tmp15E[3]=_tmp5B2;});Cyc_PP_cat(_tag_dyneither(_tmp15E,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL4C;}}_LL4C:;}
# 783
goto _LL0;case 1U: _LL11: _tmp1A3=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_tmp1A2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f2;_LL12: {
# 785
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
{enum Cyc_Absyn_Sign _tmp168=_tmp1A3;switch(_tmp168){case Cyc_Absyn_None: _LL52: _LL53:
 goto _LL55;case Cyc_Absyn_Signed: _LL54: _LL55:
 sns=({const char*_tmp169="";_tag_dyneither(_tmp169,sizeof(char),1U);});goto _LL51;default: _LL56: _LL57:
 sns=({const char*_tmp16A="unsigned ";_tag_dyneither(_tmp16A,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 792
{enum Cyc_Absyn_Size_of _tmp16B=_tmp1A2;switch(_tmp16B){case Cyc_Absyn_Char_sz: _LL59: _LL5A:
# 794
{enum Cyc_Absyn_Sign _tmp16C=_tmp1A3;switch(_tmp16C){case Cyc_Absyn_None: _LL64: _LL65:
 sns=({const char*_tmp16D="";_tag_dyneither(_tmp16D,sizeof(char),1U);});goto _LL63;case Cyc_Absyn_Signed: _LL66: _LL67:
 sns=({const char*_tmp16E="signed ";_tag_dyneither(_tmp16E,sizeof(char),8U);});goto _LL63;default: _LL68: _LL69:
 sns=({const char*_tmp16F="unsigned ";_tag_dyneither(_tmp16F,sizeof(char),10U);});goto _LL63;}_LL63:;}
# 799
ts=({const char*_tmp170="char";_tag_dyneither(_tmp170,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz: _LL5B: _LL5C:
 ts=({const char*_tmp171="short";_tag_dyneither(_tmp171,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz: _LL5D: _LL5E:
 ts=({const char*_tmp172="int";_tag_dyneither(_tmp172,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz: _LL5F: _LL60:
 ts=({const char*_tmp173="long";_tag_dyneither(_tmp173,sizeof(char),5U);});goto _LL58;default: _LL61: _LL62:
# 805
{enum Cyc_Cyclone_C_Compilers _tmp174=Cyc_Cyclone_c_compiler;if(_tmp174 == Cyc_Cyclone_Gcc_c){_LL6B: _LL6C:
 ts=({const char*_tmp175="long long";_tag_dyneither(_tmp175,sizeof(char),10U);});goto _LL6A;}else{_LL6D: _LL6E:
 ts=({const char*_tmp176="__int64";_tag_dyneither(_tmp176,sizeof(char),8U);});goto _LL6A;}_LL6A:;}
# 809
goto _LL58;}_LL58:;}
# 811
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp538;_tmp538.tag=0U,_tmp538.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns);_tmp538;});struct Cyc_String_pa_PrintArg_struct _tmp17A=({struct Cyc_String_pa_PrintArg_struct _tmp537;_tmp537.tag=0U,_tmp537.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts);_tmp537;});void*_tmp177[2U];_tmp177[0]=& _tmp179,_tmp177[1]=& _tmp17A;({struct _dyneither_ptr _tmp5B5=({const char*_tmp178="%s%s";_tag_dyneither(_tmp178,sizeof(char),5U);});Cyc_aprintf(_tmp5B5,_tag_dyneither(_tmp177,sizeof(void*),2U));});}));
goto _LL0;}case 2U: _LL13: _tmp1A4=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_LL14:
# 814
 if(_tmp1A4 == 0)
s=Cyc_PP_text(({const char*_tmp17B="float";_tag_dyneither(_tmp17B,sizeof(char),6U);}));else{
if(_tmp1A4 == 1)
s=Cyc_PP_text(({const char*_tmp17C="double";_tag_dyneither(_tmp17C,sizeof(char),7U);}));else{
# 819
s=Cyc_PP_text(({const char*_tmp17D="long double";_tag_dyneither(_tmp17D,sizeof(char),12U);}));}}
goto _LL0;case 20U: _LL17: _tmp1A6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_tmp1A5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2;_LL18: {
# 825
struct _tuple10 _tmp17F=Cyc_Absyn_aggr_kinded_name(_tmp1A6);struct _tuple10 _tmp180=_tmp17F;enum Cyc_Absyn_AggrKind _tmp183;struct _tuple0*_tmp182;_LL70: _tmp183=_tmp180.f1;_tmp182=_tmp180.f2;_LL71:;
s=({struct Cyc_PP_Doc*_tmp181[3U];({struct Cyc_PP_Doc*_tmp5B8=Cyc_Absynpp_aggr_kind2doc(_tmp183);_tmp181[0]=_tmp5B8;}),({struct Cyc_PP_Doc*_tmp5B7=Cyc_Absynpp_qvar2doc(_tmp182);_tmp181[1]=_tmp5B7;}),({struct Cyc_PP_Doc*_tmp5B6=Cyc_Absynpp_tps2doc(_tmp1A5);_tmp181[2]=_tmp5B6;});Cyc_PP_cat(_tag_dyneither(_tmp181,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 16U: _LL1B: _tmp1A7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_LL1C:
# 834
 s=({struct Cyc_PP_Doc*_tmp185[4U];({struct Cyc_PP_Doc*_tmp5BC=Cyc_PP_text(({const char*_tmp186="enum ";_tag_dyneither(_tmp186,sizeof(char),6U);}));_tmp185[0]=_tmp5BC;}),({struct Cyc_PP_Doc*_tmp5BB=Cyc_Absynpp_lb();_tmp185[1]=_tmp5BB;}),({struct Cyc_PP_Doc*_tmp5BA=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp1A7));_tmp185[2]=_tmp5BA;}),({struct Cyc_PP_Doc*_tmp5B9=Cyc_Absynpp_rb();_tmp185[3]=_tmp5B9;});Cyc_PP_cat(_tag_dyneither(_tmp185,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 15U: _LL1D: _tmp1A8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_LL1E:
# 836
 s=({struct Cyc_PP_Doc*_tmp187[2U];({struct Cyc_PP_Doc*_tmp5BE=Cyc_PP_text(({const char*_tmp188="enum ";_tag_dyneither(_tmp188,sizeof(char),6U);}));_tmp187[0]=_tmp5BE;}),({struct Cyc_PP_Doc*_tmp5BD=Cyc_Absynpp_qvar2doc(_tmp1A8);_tmp187[1]=_tmp5BD;});Cyc_PP_cat(_tag_dyneither(_tmp187,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 17U: _LL23: _tmp1A9=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_LL24:
# 842
 s=Cyc_PP_text(_tmp1A9);goto _LL0;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2 != 0){_LL27: _tmp1AA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2)->hd;_LL28:
# 852
 s=({struct Cyc_PP_Doc*_tmp190[3U];({struct Cyc_PP_Doc*_tmp5C1=Cyc_PP_text(({const char*_tmp191="region_t<";_tag_dyneither(_tmp191,sizeof(char),10U);}));_tmp190[0]=_tmp5C1;}),({struct Cyc_PP_Doc*_tmp5C0=Cyc_Absynpp_rgn2doc(_tmp1AA);_tmp190[1]=_tmp5C0;}),({struct Cyc_PP_Doc*_tmp5BF=Cyc_PP_text(({const char*_tmp192=">";_tag_dyneither(_tmp192,sizeof(char),2U);}));_tmp190[2]=_tmp5BF;});Cyc_PP_cat(_tag_dyneither(_tmp190,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}else{goto _LL45;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2 != 0){_LL29: _tmp1AB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2)->hd;_LL2A:
# 854
 s=({struct Cyc_PP_Doc*_tmp193[3U];({struct Cyc_PP_Doc*_tmp5C4=Cyc_PP_text(({const char*_tmp194="tag_t<";_tag_dyneither(_tmp194,sizeof(char),7U);}));_tmp193[0]=_tmp5C4;}),({struct Cyc_PP_Doc*_tmp5C3=Cyc_Absynpp_typ2doc(_tmp1AB);_tmp193[1]=_tmp5C3;}),({struct Cyc_PP_Doc*_tmp5C2=Cyc_PP_text(({const char*_tmp195=">";_tag_dyneither(_tmp195,sizeof(char),2U);}));_tmp193[2]=_tmp5C2;});Cyc_PP_cat(_tag_dyneither(_tmp193,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}else{goto _LL45;}case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
# 858
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2 != 0){_LL31: _tmp1AC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2)->hd;_LL32:
# 860
 s=({struct Cyc_PP_Doc*_tmp196[3U];({struct Cyc_PP_Doc*_tmp5C7=Cyc_PP_text(({const char*_tmp197="regions(";_tag_dyneither(_tmp197,sizeof(char),9U);}));_tmp196[0]=_tmp5C7;}),({struct Cyc_PP_Doc*_tmp5C6=Cyc_Absynpp_typ2doc(_tmp1AC);_tmp196[1]=_tmp5C6;}),({struct Cyc_PP_Doc*_tmp5C5=Cyc_PP_text(({const char*_tmp198=")";_tag_dyneither(_tmp198,sizeof(char),2U);}));_tmp196[2]=_tmp5C5;});Cyc_PP_cat(_tag_dyneither(_tmp196,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}else{_LL45: _tmp1AD=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1;_LL46:
# 879
 s=Cyc_PP_text(({const char*_tmp19D="*bad type application*";_tag_dyneither(_tmp19D,sizeof(char),23U);}));goto _LL0;}case 8U: _LL33: _LL34:
# 861
 goto _LL36;case 9U: _LL35: _LL36:
# 863
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 871
 s=Cyc_PP_text(({const char*_tmp199="@true";_tag_dyneither(_tmp199,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
# 873
 s=Cyc_PP_text(({const char*_tmp19A="@false";_tag_dyneither(_tmp19A,sizeof(char),7U);}));goto _LL0;case 13U: _LL41: _LL42:
# 875
 s=Cyc_PP_text(({const char*_tmp19B="@thin";_tag_dyneither(_tmp19B,sizeof(char),6U);}));goto _LL0;default: _LL43: _LL44:
# 877
 s=Cyc_PP_text(({const char*_tmp19C="@fat";_tag_dyneither(_tmp19C,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp1B1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp148)->f1;_tmp1B0=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp148)->f2;_tmp1AF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp148)->f3;_tmp1AE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp148)->f4;_LLA:
# 734 "absynpp.cyc"
 if(_tmp1B0 != 0)
# 736
return Cyc_Absynpp_ntyp2doc(_tmp1B0);else{
# 738
struct _dyneither_ptr kindstring=_tmp1B1 == 0?({const char*_tmp14F="K";_tag_dyneither(_tmp14F,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp1B1->v);
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14D=({struct Cyc_String_pa_PrintArg_struct _tmp53A;_tmp53A.tag=0U,_tmp53A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring);_tmp53A;});struct Cyc_Int_pa_PrintArg_struct _tmp14E=({struct Cyc_Int_pa_PrintArg_struct _tmp539;_tmp539.tag=1U,_tmp539.f1=(unsigned long)_tmp1AF;_tmp539;});void*_tmp14B[2U];_tmp14B[0]=& _tmp14D,_tmp14B[1]=& _tmp14E;({struct _dyneither_ptr _tmp5C8=({const char*_tmp14C="`E%s%d";_tag_dyneither(_tmp14C,sizeof(char),7U);});Cyc_aprintf(_tmp5C8,_tag_dyneither(_tmp14B,sizeof(void*),2U));});}));}
# 755 "absynpp.cyc"
goto _LL0;case 2U: _LLB: _tmp1B2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp148)->f1;_LLC:
# 757
 s=Cyc_Absynpp_tvar2doc(_tmp1B2);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp150[3U];_tmp150[0]=s,({struct Cyc_PP_Doc*_tmp5CA=Cyc_PP_text(({const char*_tmp151="::";_tag_dyneither(_tmp151,sizeof(char),3U);}));_tmp150[1]=_tmp5CA;}),({struct Cyc_PP_Doc*_tmp5C9=Cyc_Absynpp_kindbound2doc(_tmp1B2->kind);_tmp150[2]=_tmp5C9;});Cyc_PP_cat(_tag_dyneither(_tmp150,sizeof(struct Cyc_PP_Doc*),3U));});
# 761
goto _LL0;case 6U: _LL15: _tmp1B3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp148)->f1;_LL16:
# 822
 s=({struct Cyc_PP_Doc*_tmp17E[2U];({struct Cyc_PP_Doc*_tmp5CC=Cyc_Absynpp_dollar();_tmp17E[0]=_tmp5CC;}),({struct Cyc_PP_Doc*_tmp5CB=Cyc_Absynpp_args2doc(_tmp1B3);_tmp17E[1]=_tmp5CB;});Cyc_PP_cat(_tag_dyneither(_tmp17E,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 7U: _LL19: _tmp1B5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp148)->f1;_tmp1B4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp148)->f2;_LL1A:
# 829
 s=({struct Cyc_PP_Doc*_tmp184[4U];({struct Cyc_PP_Doc*_tmp5D0=Cyc_Absynpp_aggr_kind2doc(_tmp1B5);_tmp184[0]=_tmp5D0;}),({struct Cyc_PP_Doc*_tmp5CF=Cyc_Absynpp_lb();_tmp184[1]=_tmp5CF;}),({
struct Cyc_PP_Doc*_tmp5CE=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp1B4));_tmp184[2]=_tmp5CE;}),({
struct Cyc_PP_Doc*_tmp5CD=Cyc_Absynpp_rb();_tmp184[3]=_tmp5CD;});Cyc_PP_cat(_tag_dyneither(_tmp184,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 9U: _LL1F: _tmp1B6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp148)->f1;_LL20:
# 838
 s=({struct Cyc_PP_Doc*_tmp189[3U];({struct Cyc_PP_Doc*_tmp5D3=Cyc_PP_text(({const char*_tmp18A="valueof_t(";_tag_dyneither(_tmp18A,sizeof(char),11U);}));_tmp189[0]=_tmp5D3;}),({struct Cyc_PP_Doc*_tmp5D2=Cyc_Absynpp_exp2doc(_tmp1B6);_tmp189[1]=_tmp5D2;}),({struct Cyc_PP_Doc*_tmp5D1=Cyc_PP_text(({const char*_tmp18B=")";_tag_dyneither(_tmp18B,sizeof(char),2U);}));_tmp189[2]=_tmp5D1;});Cyc_PP_cat(_tag_dyneither(_tmp189,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 11U: _LL21: _tmp1B7=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp148)->f1;_LL22:
# 840
 s=({struct Cyc_PP_Doc*_tmp18C[3U];({struct Cyc_PP_Doc*_tmp5D6=Cyc_PP_text(({const char*_tmp18D="typeof(";_tag_dyneither(_tmp18D,sizeof(char),8U);}));_tmp18C[0]=_tmp5D6;}),({struct Cyc_PP_Doc*_tmp5D5=Cyc_Absynpp_exp2doc(_tmp1B7);_tmp18C[1]=_tmp5D5;}),({struct Cyc_PP_Doc*_tmp5D4=Cyc_PP_text(({const char*_tmp18E=")";_tag_dyneither(_tmp18E,sizeof(char),2U);}));_tmp18C[2]=_tmp5D4;});Cyc_PP_cat(_tag_dyneither(_tmp18C,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 8U: _LL25: _tmp1BA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f1;_tmp1B9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f2;_tmp1B8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f3;_LL26:
# 849
 s=({struct Cyc_PP_Doc*_tmp18F[2U];({struct Cyc_PP_Doc*_tmp5D8=Cyc_Absynpp_qvar2doc(_tmp1BA);_tmp18F[0]=_tmp5D8;}),({struct Cyc_PP_Doc*_tmp5D7=Cyc_Absynpp_tps2doc(_tmp1B9);_tmp18F[1]=_tmp5D7;});Cyc_PP_cat(_tag_dyneither(_tmp18F,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)){case 0U: _LL37: _tmp1BB=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)->f1;_LL38:
# 865
 s=Cyc_Absynpp_aggrdecl2doc(_tmp1BB);goto _LL0;case 1U: _LL39: _tmp1BC=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)->f1;_LL3A:
# 867
 s=Cyc_Absynpp_enumdecl2doc(_tmp1BC);goto _LL0;default: _LL3B: _tmp1BD=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)->f1;_LL3C:
# 869
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1BD);goto _LL0;}}_LL0:;}
# 881
return s;}
# 884
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 888
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16*_tmp1BE=cmp;void*_tmp1C2;void*_tmp1C1;_LL1: _tmp1C2=_tmp1BE->f1;_tmp1C1=_tmp1BE->f2;_LL2:;
return({struct Cyc_PP_Doc*_tmp1BF[3U];({struct Cyc_PP_Doc*_tmp5DB=Cyc_Absynpp_rgn2doc(_tmp1C2);_tmp1BF[0]=_tmp5DB;}),({struct Cyc_PP_Doc*_tmp5DA=Cyc_PP_text(({const char*_tmp1C0=" > ";_tag_dyneither(_tmp1C0,sizeof(char),4U);}));_tmp1BF[1]=_tmp5DA;}),({struct Cyc_PP_Doc*_tmp5D9=Cyc_Absynpp_rgn2doc(_tmp1C1);_tmp1BF[2]=_tmp5D9;});Cyc_PP_cat(_tag_dyneither(_tmp1BF,sizeof(struct Cyc_PP_Doc*),3U));});}
# 893
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _dyneither_ptr _tmp5DE=({const char*_tmp1C3="";_tag_dyneither(_tmp1C3,sizeof(char),1U);});struct _dyneither_ptr _tmp5DD=({const char*_tmp1C4="";_tag_dyneither(_tmp1C4,sizeof(char),1U);});struct _dyneither_ptr _tmp5DC=({const char*_tmp1C5=",";_tag_dyneither(_tmp1C5,sizeof(char),2U);});Cyc_PP_group(_tmp5DE,_tmp5DD,_tmp5DC,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 897
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1C6=(*t).f1;
struct Cyc_Core_Opt*dopt=_tmp1C6 == 0?0:({struct Cyc_Core_Opt*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({struct Cyc_PP_Doc*_tmp5DF=Cyc_PP_text(*_tmp1C6);_tmp1C7->v=_tmp5DF;});_tmp1C7;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 903
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 907
struct Cyc_List_List*_tmp1C8=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
_tmp1C8=({struct Cyc_List_List*_tmp5E1=_tmp1C8;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5E1,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));({struct Cyc_PP_Doc*_tmp5E0=Cyc_PP_text(({const char*_tmp1C9="...";_tag_dyneither(_tmp1C9,sizeof(char),4U);}));_tmp1CA->hd=_tmp5E0;}),_tmp1CA->tl=0;_tmp1CA;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1CB=({struct Cyc_PP_Doc*_tmp1CD[3U];({struct Cyc_PP_Doc*_tmp5E5=Cyc_PP_text(({const char*_tmp1CE="...";_tag_dyneither(_tmp1CE,sizeof(char),4U);}));_tmp1CD[0]=_tmp5E5;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp5E4=Cyc_PP_text(({const char*_tmp1CF=" inject ";_tag_dyneither(_tmp1CF,sizeof(char),9U);}));_tmp1CD[1]=_tmp5E4;}):({struct Cyc_PP_Doc*_tmp5E3=Cyc_PP_text(({const char*_tmp1D0=" ";_tag_dyneither(_tmp1D0,sizeof(char),2U);}));_tmp1CD[1]=_tmp5E3;}),({
struct Cyc_PP_Doc*_tmp5E2=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->f1=cyc_varargs->name,_tmp1D1->f2=cyc_varargs->tq,_tmp1D1->f3=cyc_varargs->type;_tmp1D1;}));_tmp1CD[2]=_tmp5E2;});Cyc_PP_cat(_tag_dyneither(_tmp1CD,sizeof(struct Cyc_PP_Doc*),3U));});
# 916
_tmp1C8=({struct Cyc_List_List*_tmp5E6=_tmp1C8;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5E6,({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=_tmp1CB,_tmp1CC->tl=0;_tmp1CC;}));});}}{
# 918
struct Cyc_PP_Doc*_tmp1D2=({struct _dyneither_ptr _tmp5E9=({const char*_tmp1E1="";_tag_dyneither(_tmp1E1,sizeof(char),1U);});struct _dyneither_ptr _tmp5E8=({const char*_tmp1E2="";_tag_dyneither(_tmp1E2,sizeof(char),1U);});struct _dyneither_ptr _tmp5E7=({const char*_tmp1E3=",";_tag_dyneither(_tmp1E3,sizeof(char),2U);});Cyc_PP_group(_tmp5E9,_tmp5E8,_tmp5E7,_tmp1C8);});
if(effopt != 0  && Cyc_Absynpp_print_all_effects)
_tmp1D2=({struct Cyc_PP_Doc*_tmp1D3[3U];_tmp1D3[0]=_tmp1D2,({struct Cyc_PP_Doc*_tmp5EB=Cyc_PP_text(({const char*_tmp1D4=";";_tag_dyneither(_tmp1D4,sizeof(char),2U);}));_tmp1D3[1]=_tmp5EB;}),({struct Cyc_PP_Doc*_tmp5EA=Cyc_Absynpp_eff2doc(effopt);_tmp1D3[2]=_tmp5EA;});Cyc_PP_cat(_tag_dyneither(_tmp1D3,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
_tmp1D2=({struct Cyc_PP_Doc*_tmp1D5[3U];_tmp1D5[0]=_tmp1D2,({struct Cyc_PP_Doc*_tmp5ED=Cyc_PP_text(({const char*_tmp1D6=":";_tag_dyneither(_tmp1D6,sizeof(char),2U);}));_tmp1D5[1]=_tmp5ED;}),({struct Cyc_PP_Doc*_tmp5EC=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1D5[2]=_tmp5EC;});Cyc_PP_cat(_tag_dyneither(_tmp1D5,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp1D7=({struct Cyc_PP_Doc*_tmp1DE[3U];({struct Cyc_PP_Doc*_tmp5EF=Cyc_PP_text(({const char*_tmp1DF="(";_tag_dyneither(_tmp1DF,sizeof(char),2U);}));_tmp1DE[0]=_tmp5EF;}),_tmp1DE[1]=_tmp1D2,({struct Cyc_PP_Doc*_tmp5EE=Cyc_PP_text(({const char*_tmp1E0=")";_tag_dyneither(_tmp1E0,sizeof(char),2U);}));_tmp1DE[2]=_tmp5EE;});Cyc_PP_cat(_tag_dyneither(_tmp1DE,sizeof(struct Cyc_PP_Doc*),3U));});
if(req != 0)
_tmp1D7=({struct Cyc_PP_Doc*_tmp1D8[4U];_tmp1D8[0]=_tmp1D7,({struct Cyc_PP_Doc*_tmp5F2=Cyc_PP_text(({const char*_tmp1D9=" @requires(";_tag_dyneither(_tmp1D9,sizeof(char),12U);}));_tmp1D8[1]=_tmp5F2;}),({struct Cyc_PP_Doc*_tmp5F1=Cyc_Absynpp_exp2doc(req);_tmp1D8[2]=_tmp5F1;}),({struct Cyc_PP_Doc*_tmp5F0=Cyc_PP_text(({const char*_tmp1DA=")";_tag_dyneither(_tmp1DA,sizeof(char),2U);}));_tmp1D8[3]=_tmp5F0;});Cyc_PP_cat(_tag_dyneither(_tmp1D8,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
_tmp1D7=({struct Cyc_PP_Doc*_tmp1DB[4U];_tmp1DB[0]=_tmp1D7,({struct Cyc_PP_Doc*_tmp5F5=Cyc_PP_text(({const char*_tmp1DC=" @ensures(";_tag_dyneither(_tmp1DC,sizeof(char),11U);}));_tmp1DB[1]=_tmp5F5;}),({struct Cyc_PP_Doc*_tmp5F4=Cyc_Absynpp_exp2doc(ens);_tmp1DB[2]=_tmp5F4;}),({struct Cyc_PP_Doc*_tmp5F3=Cyc_PP_text(({const char*_tmp1DD=")";_tag_dyneither(_tmp1DD,sizeof(char),2U);}));_tmp1DB[3]=_tmp5F3;});Cyc_PP_cat(_tag_dyneither(_tmp1DB,sizeof(struct Cyc_PP_Doc*),4U));});
return _tmp1D7;};};}
# 931
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _tuple8*_tmp1E4=arg;struct _dyneither_ptr*_tmp1E8;struct Cyc_Absyn_Tqual _tmp1E7;void*_tmp1E6;_LL1: _tmp1E8=_tmp1E4->f1;_tmp1E7=_tmp1E4->f2;_tmp1E6=_tmp1E4->f3;_LL2:;
return({struct _tuple8*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->f1=_tmp1E8,_tmp1E5->f2=_tmp1E7,_tmp1E5->f3=_tmp1E6;_tmp1E5;});}
# 936
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 938
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1E9=0;
int match;
{union Cyc_Absyn_Nmspace _tmp1EA=(*q).f1;union Cyc_Absyn_Nmspace _tmp1EB=_tmp1EA;struct Cyc_List_List*_tmp1EF;struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*_tmp1ED;switch((_tmp1EB.C_n).tag){case 4U: _LL1: _LL2:
 _tmp1ED=0;goto _LL4;case 1U: _LL3: _tmp1ED=(_tmp1EB.Rel_n).val;_LL4:
# 944
 match=0;
_tmp1E9=_tmp1ED;
goto _LL0;case 3U: _LL5: _tmp1EE=(_tmp1EB.C_n).val;_LL6:
# 948
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1EE,Cyc_Absynpp_curr_namespace);
# 950
goto _LL0;default: _LL7: _tmp1EF=(_tmp1EB.Abs_n).val;_LL8:
# 952
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1EF,Cyc_Absynpp_curr_namespace);
_tmp1E9=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->hd=Cyc_Absynpp_cyc_stringptr,_tmp1EC->tl=_tmp1EF;_tmp1EC;}): _tmp1EF;
goto _LL0;}_LL0:;}
# 956
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp5F7=({struct Cyc_List_List*_tmp5F6=_tmp1E9;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5F6,({struct Cyc_List_List*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));
_tmp1F0->hd=(*q).f2,_tmp1F0->tl=0;_tmp1F0;}));});
# 957
Cyc_str_sepstr(_tmp5F7,({const char*_tmp1F1="_";_tag_dyneither(_tmp1F1,sizeof(char),2U);}));});else{
# 961
if(match)
return*(*q).f2;else{
# 964
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp5F9=({struct Cyc_List_List*_tmp5F8=_tmp1E9;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5F8,({struct Cyc_List_List*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->hd=(*q).f2,_tmp1F2->tl=0;_tmp1F2;}));});Cyc_str_sepstr(_tmp5F9,({const char*_tmp1F3="::";_tag_dyneither(_tmp1F3,sizeof(char),3U);}));});}}}
# 968
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 972
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1F4=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 976
return({struct _dyneither_ptr _tmp5FC=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp5FB=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp5FA=({const char*_tmp1F5="\\textbf{";_tag_dyneither(_tmp1F5,sizeof(char),9U);});Cyc_strconcat(_tmp5FA,(struct _dyneither_ptr)_tmp1F4);});Cyc_strconcat(_tmp5FB,({const char*_tmp1F6="}";_tag_dyneither(_tmp1F6,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp5FC,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1F4));});else{
return Cyc_PP_text(_tmp1F4);}}
# 981
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 983
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 986
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1F7=(*v).f1;union Cyc_Absyn_Nmspace _tmp1F8=_tmp1F7;struct Cyc_List_List*_tmp1FB;struct Cyc_List_List*_tmp1FA;switch((_tmp1F8.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp1F8.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 997
 return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp5FD=({const char*_tmp1F9="/* bad namespace : */ ";_tag_dyneither(_tmp1F9,sizeof(char),23U);});Cyc_strconcat(_tmp5FD,(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp1FA=(_tmp1F8.C_n).val;_LL6:
# 990
 _tmp1FB=_tmp1FA;goto _LL8;default: _LL7: _tmp1FB=(_tmp1F8.Abs_n).val;_LL8:
# 992
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1FB,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 995
goto _LLA;}}_LL0:;}else{
# 1000
return*(*v).f2;}}
# 1003
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 1006
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp1FC=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)
# 1010
return({struct _dyneither_ptr _tmp600=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp5FF=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp5FE=({const char*_tmp1FD="\\textbf{";_tag_dyneither(_tmp1FD,sizeof(char),9U);});Cyc_strconcat(_tmp5FE,(struct _dyneither_ptr)_tmp1FC);});Cyc_strconcat(_tmp5FF,({const char*_tmp1FE="}";_tag_dyneither(_tmp1FE,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp600,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1FC));});else{
return Cyc_PP_text(_tmp1FC);}}
# 1015
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp601=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp601,t,0);});}
# 1026 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp1FF=e->r;void*_tmp200=_tmp1FF;struct Cyc_Absyn_Exp*_tmp204;struct Cyc_Absyn_Exp*_tmp203;enum Cyc_Absyn_Primop _tmp202;switch(*((int*)_tmp200)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp202=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp200)->f1;_LL6: {
# 1031
enum Cyc_Absyn_Primop _tmp201=_tmp202;switch(_tmp201){case Cyc_Absyn_Plus: _LL56: _LL57:
 return 100;case Cyc_Absyn_Times: _LL58: _LL59:
 return 110;case Cyc_Absyn_Minus: _LL5A: _LL5B:
 return 100;case Cyc_Absyn_Div: _LL5C: _LL5D:
 goto _LL5F;case Cyc_Absyn_Mod: _LL5E: _LL5F:
 return 110;case Cyc_Absyn_Eq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Neq: _LL62: _LL63:
 return 70;case Cyc_Absyn_Gt: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Lt: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Gte: _LL68: _LL69:
 goto _LL6B;case Cyc_Absyn_Lte: _LL6A: _LL6B:
 return 80;case Cyc_Absyn_Not: _LL6C: _LL6D:
 goto _LL6F;case Cyc_Absyn_Bitnot: _LL6E: _LL6F:
 return 130;case Cyc_Absyn_Bitand: _LL70: _LL71:
 return 60;case Cyc_Absyn_Bitor: _LL72: _LL73:
 return 40;case Cyc_Absyn_Bitxor: _LL74: _LL75:
 return 50;case Cyc_Absyn_Bitlshift: _LL76: _LL77:
 return 90;case Cyc_Absyn_Bitlrshift: _LL78: _LL79:
 return 80;case Cyc_Absyn_Bitarshift: _LL7A: _LL7B:
 return 80;default: _LL7C: _LL7D:
 return 140;}_LL55:;}case 4U: _LL7: _LL8:
# 1053
 return 20;case 5U: _LL9: _LLA:
 return 130;case 6U: _LLB: _LLC:
 return 30;case 7U: _LLD: _LLE:
 return 35;case 8U: _LLF: _LL10:
 return 30;case 9U: _LL11: _LL12:
 return - 10;case 10U: _LL13: _LL14:
 return 140;case 2U: _LL15: _LL16:
 return 140;case 11U: _LL17: _LL18:
 return 130;case 12U: _LL19: _tmp203=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp200)->f1;_LL1A:
 return Cyc_Absynpp_exp_prec(_tmp203);case 13U: _LL1B: _tmp204=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp200)->f1;_LL1C:
 return Cyc_Absynpp_exp_prec(_tmp204);case 14U: _LL1D: _LL1E:
 return 120;case 16U: _LL1F: _LL20:
 return 15;case 15U: _LL21: _LL22:
 goto _LL24;case 17U: _LL23: _LL24:
 goto _LL26;case 18U: _LL25: _LL26:
 goto _LL28;case 39U: _LL27: _LL28:
 goto _LL2A;case 40U: _LL29: _LL2A:
 goto _LL2C;case 38U: _LL2B: _LL2C:
 goto _LL2E;case 19U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 41U: _LL31: _LL32:
 return 130;case 21U: _LL33: _LL34:
 goto _LL36;case 22U: _LL35: _LL36:
 goto _LL38;case 23U: _LL37: _LL38:
 return 140;case 24U: _LL39: _LL3A:
 return 150;case 25U: _LL3B: _LL3C:
 goto _LL3E;case 26U: _LL3D: _LL3E:
 goto _LL40;case 27U: _LL3F: _LL40:
 goto _LL42;case 28U: _LL41: _LL42:
 goto _LL44;case 29U: _LL43: _LL44:
 goto _LL46;case 30U: _LL45: _LL46:
 goto _LL48;case 31U: _LL47: _LL48:
 goto _LL4A;case 32U: _LL49: _LL4A:
 goto _LL4C;case 33U: _LL4B: _LL4C:
 goto _LL4E;case 34U: _LL4D: _LL4E:
 goto _LL50;case 35U: _LL4F: _LL50:
 goto _LL52;case 36U: _LL51: _LL52:
 return 140;default: _LL53: _LL54:
 return 10000;}_LL0:;}
# 1095
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1099
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp205=e->r;void*_tmp206=_tmp205;struct Cyc_Absyn_Stmt*_tmp2DB;struct Cyc_Core_Opt*_tmp2DA;struct Cyc_List_List*_tmp2D9;struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D7;int _tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;void**_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D3;int _tmp2D2;struct Cyc_Absyn_Enumfield*_tmp2D1;struct Cyc_Absyn_Enumfield*_tmp2D0;struct Cyc_List_List*_tmp2CF;struct Cyc_Absyn_Datatypefield*_tmp2CE;struct Cyc_List_List*_tmp2CD;struct _tuple0*_tmp2CC;struct Cyc_List_List*_tmp2CB;struct Cyc_List_List*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2C9;void*_tmp2C8;struct Cyc_Absyn_Vardecl*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_Absyn_Exp*_tmp2C5;struct Cyc_List_List*_tmp2C4;struct _tuple8*_tmp2C3;struct Cyc_List_List*_tmp2C2;struct Cyc_List_List*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct _dyneither_ptr*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BC;struct _dyneither_ptr*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BA;void*_tmp2B9;struct Cyc_List_List*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B7;struct _dyneither_ptr*_tmp2B6;int _tmp2B5;struct _dyneither_ptr _tmp2B4;void*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B1;void*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AD;void*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;struct Cyc_Absyn_Exp*_tmp2A7;struct Cyc_List_List*_tmp2A6;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29C;enum Cyc_Absyn_Incrementor _tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Core_Opt*_tmp299;struct Cyc_Absyn_Exp*_tmp298;enum Cyc_Absyn_Primop _tmp297;struct Cyc_List_List*_tmp296;struct _dyneither_ptr _tmp295;void*_tmp294;union Cyc_Absyn_Cnst _tmp293;switch(*((int*)_tmp206)){case 0U: _LL1: _tmp293=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL2:
 s=Cyc_Absynpp_cnst2doc(_tmp293);goto _LL0;case 1U: _LL3: _tmp294=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL4:
 s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(_tmp294));goto _LL0;case 2U: _LL5: _tmp295=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL6:
# 1106
 s=({struct Cyc_PP_Doc*_tmp207[4U];({struct Cyc_PP_Doc*_tmp605=Cyc_PP_text(({const char*_tmp208="__cyclone_pragma__";_tag_dyneither(_tmp208,sizeof(char),19U);}));_tmp207[0]=_tmp605;}),({struct Cyc_PP_Doc*_tmp604=Cyc_PP_text(({const char*_tmp209="(";_tag_dyneither(_tmp209,sizeof(char),2U);}));_tmp207[1]=_tmp604;}),({struct Cyc_PP_Doc*_tmp603=Cyc_PP_text(_tmp295);_tmp207[2]=_tmp603;}),({struct Cyc_PP_Doc*_tmp602=Cyc_PP_text(({const char*_tmp20A=")";_tag_dyneither(_tmp20A,sizeof(char),2U);}));_tmp207[3]=_tmp602;});Cyc_PP_cat(_tag_dyneither(_tmp207,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 3U: _LL7: _tmp297=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp296=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL8:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp297,_tmp296);goto _LL0;case 4U: _LL9: _tmp29A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp299=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_tmp298=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp206)->f3;_LLA:
# 1109
 s=({struct Cyc_PP_Doc*_tmp20B[5U];({struct Cyc_PP_Doc*_tmp60B=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29A);_tmp20B[0]=_tmp60B;}),({
struct Cyc_PP_Doc*_tmp60A=Cyc_PP_text(({const char*_tmp20C=" ";_tag_dyneither(_tmp20C,sizeof(char),2U);}));_tmp20B[1]=_tmp60A;}),
_tmp299 == 0?({struct Cyc_PP_Doc*_tmp609=Cyc_PP_nil_doc();_tmp20B[2]=_tmp609;}):({struct Cyc_PP_Doc*_tmp608=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp299->v);_tmp20B[2]=_tmp608;}),({
struct Cyc_PP_Doc*_tmp607=Cyc_PP_text(({const char*_tmp20D="= ";_tag_dyneither(_tmp20D,sizeof(char),3U);}));_tmp20B[3]=_tmp607;}),({
struct Cyc_PP_Doc*_tmp606=Cyc_Absynpp_exp2doc_prec(myprec,_tmp298);_tmp20B[4]=_tmp606;});Cyc_PP_cat(_tag_dyneither(_tmp20B,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 5U: _LLB: _tmp29C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp29B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LLC: {
# 1116
struct Cyc_PP_Doc*_tmp20E=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29C);
{enum Cyc_Absyn_Incrementor _tmp20F=_tmp29B;switch(_tmp20F){case Cyc_Absyn_PreInc: _LL56: _LL57:
 s=({struct Cyc_PP_Doc*_tmp210[2U];({struct Cyc_PP_Doc*_tmp60C=Cyc_PP_text(({const char*_tmp211="++";_tag_dyneither(_tmp211,sizeof(char),3U);}));_tmp210[0]=_tmp60C;}),_tmp210[1]=_tmp20E;Cyc_PP_cat(_tag_dyneither(_tmp210,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PreDec: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp212[2U];({struct Cyc_PP_Doc*_tmp60D=Cyc_PP_text(({const char*_tmp213="--";_tag_dyneither(_tmp213,sizeof(char),3U);}));_tmp212[0]=_tmp60D;}),_tmp212[1]=_tmp20E;Cyc_PP_cat(_tag_dyneither(_tmp212,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostInc: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp214[2U];_tmp214[0]=_tmp20E,({struct Cyc_PP_Doc*_tmp60E=Cyc_PP_text(({const char*_tmp215="++";_tag_dyneither(_tmp215,sizeof(char),3U);}));_tmp214[1]=_tmp60E;});Cyc_PP_cat(_tag_dyneither(_tmp214,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;default: _LL5C: _LL5D:
 s=({struct Cyc_PP_Doc*_tmp216[2U];_tmp216[0]=_tmp20E,({struct Cyc_PP_Doc*_tmp60F=Cyc_PP_text(({const char*_tmp217="--";_tag_dyneither(_tmp217,sizeof(char),3U);}));_tmp216[1]=_tmp60F;});Cyc_PP_cat(_tag_dyneither(_tmp216,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;}_LL55:;}
# 1123
goto _LL0;}case 6U: _LLD: _tmp29F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp29E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_tmp29D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp206)->f3;_LLE:
# 1125
 s=({struct Cyc_PP_Doc*_tmp218[5U];({struct Cyc_PP_Doc*_tmp614=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29F);_tmp218[0]=_tmp614;}),({struct Cyc_PP_Doc*_tmp613=Cyc_PP_text(({const char*_tmp219=" ? ";_tag_dyneither(_tmp219,sizeof(char),4U);}));_tmp218[1]=_tmp613;}),({struct Cyc_PP_Doc*_tmp612=Cyc_Absynpp_exp2doc_prec(0,_tmp29E);_tmp218[2]=_tmp612;}),({
struct Cyc_PP_Doc*_tmp611=Cyc_PP_text(({const char*_tmp21A=" : ";_tag_dyneither(_tmp21A,sizeof(char),4U);}));_tmp218[3]=_tmp611;}),({struct Cyc_PP_Doc*_tmp610=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29D);_tmp218[4]=_tmp610;});Cyc_PP_cat(_tag_dyneither(_tmp218,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 7U: _LLF: _tmp2A1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2A0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL10:
# 1129
 s=({struct Cyc_PP_Doc*_tmp21B[3U];({struct Cyc_PP_Doc*_tmp617=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A1);_tmp21B[0]=_tmp617;}),({struct Cyc_PP_Doc*_tmp616=Cyc_PP_text(({const char*_tmp21C=" && ";_tag_dyneither(_tmp21C,sizeof(char),5U);}));_tmp21B[1]=_tmp616;}),({struct Cyc_PP_Doc*_tmp615=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A0);_tmp21B[2]=_tmp615;});Cyc_PP_cat(_tag_dyneither(_tmp21B,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 8U: _LL11: _tmp2A3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2A2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL12:
# 1132
 s=({struct Cyc_PP_Doc*_tmp21D[3U];({struct Cyc_PP_Doc*_tmp61A=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A3);_tmp21D[0]=_tmp61A;}),({struct Cyc_PP_Doc*_tmp619=Cyc_PP_text(({const char*_tmp21E=" || ";_tag_dyneither(_tmp21E,sizeof(char),5U);}));_tmp21D[1]=_tmp619;}),({struct Cyc_PP_Doc*_tmp618=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A2);_tmp21D[2]=_tmp618;});Cyc_PP_cat(_tag_dyneither(_tmp21D,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 9U: _LL13: _tmp2A5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2A4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL14:
# 1137
 s=({struct Cyc_PP_Doc*_tmp21F[3U];({struct Cyc_PP_Doc*_tmp61D=Cyc_Absynpp_exp2doc_prec(myprec - 1,_tmp2A5);_tmp21F[0]=_tmp61D;}),({struct Cyc_PP_Doc*_tmp61C=Cyc_PP_text(({const char*_tmp220=", ";_tag_dyneither(_tmp220,sizeof(char),3U);}));_tmp21F[1]=_tmp61C;}),({struct Cyc_PP_Doc*_tmp61B=Cyc_Absynpp_exp2doc_prec(myprec - 1,_tmp2A4);_tmp21F[2]=_tmp61B;});Cyc_PP_cat(_tag_dyneither(_tmp21F,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 10U: _LL15: _tmp2A7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2A6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL16:
# 1140
 s=({struct Cyc_PP_Doc*_tmp221[4U];({struct Cyc_PP_Doc*_tmp621=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A7);_tmp221[0]=_tmp621;}),({
struct Cyc_PP_Doc*_tmp620=Cyc_PP_text(({const char*_tmp222="(";_tag_dyneither(_tmp222,sizeof(char),2U);}));_tmp221[1]=_tmp620;}),({
struct Cyc_PP_Doc*_tmp61F=Cyc_Absynpp_exps2doc_prec(20,_tmp2A6);_tmp221[2]=_tmp61F;}),({
struct Cyc_PP_Doc*_tmp61E=Cyc_PP_text(({const char*_tmp223=")";_tag_dyneither(_tmp223,sizeof(char),2U);}));_tmp221[3]=_tmp61E;});Cyc_PP_cat(_tag_dyneither(_tmp221,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 11U: _LL17: _tmp2A8=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL18:
# 1146
 s=({struct Cyc_PP_Doc*_tmp224[2U];({struct Cyc_PP_Doc*_tmp623=Cyc_PP_text(({const char*_tmp225="throw ";_tag_dyneither(_tmp225,sizeof(char),7U);}));_tmp224[0]=_tmp623;}),({struct Cyc_PP_Doc*_tmp622=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A8);_tmp224[1]=_tmp622;});Cyc_PP_cat(_tag_dyneither(_tmp224,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 12U: _LL19: _tmp2A9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL1A:
# 1149
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2A9);
goto _LL0;case 13U: _LL1B: _tmp2AA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL1C:
# 1152
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2AA);
goto _LL0;case 14U: _LL1D: _tmp2AC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2AB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL1E:
# 1155
 s=({struct Cyc_PP_Doc*_tmp226[4U];({struct Cyc_PP_Doc*_tmp627=Cyc_PP_text(({const char*_tmp227="(";_tag_dyneither(_tmp227,sizeof(char),2U);}));_tmp226[0]=_tmp627;}),({
struct Cyc_PP_Doc*_tmp626=Cyc_Absynpp_typ2doc(_tmp2AC);_tmp226[1]=_tmp626;}),({
struct Cyc_PP_Doc*_tmp625=Cyc_PP_text(({const char*_tmp228=")";_tag_dyneither(_tmp228,sizeof(char),2U);}));_tmp226[2]=_tmp625;}),({
struct Cyc_PP_Doc*_tmp624=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AB);_tmp226[3]=_tmp624;});Cyc_PP_cat(_tag_dyneither(_tmp226,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 15U: _LL1F: _tmp2AD=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL20:
# 1161
 s=({struct Cyc_PP_Doc*_tmp229[2U];({struct Cyc_PP_Doc*_tmp629=Cyc_PP_text(({const char*_tmp22A="&";_tag_dyneither(_tmp22A,sizeof(char),2U);}));_tmp229[0]=_tmp629;}),({
struct Cyc_PP_Doc*_tmp628=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AD);_tmp229[1]=_tmp628;});Cyc_PP_cat(_tag_dyneither(_tmp229,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 16U: _LL21: _tmp2AF=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2AE=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL22:
# 1165
 if(_tmp2AF == 0)
s=({struct Cyc_PP_Doc*_tmp22B[2U];({struct Cyc_PP_Doc*_tmp62B=Cyc_PP_text(({const char*_tmp22C="new ";_tag_dyneither(_tmp22C,sizeof(char),5U);}));_tmp22B[0]=_tmp62B;}),({struct Cyc_PP_Doc*_tmp62A=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AE);_tmp22B[1]=_tmp62A;});Cyc_PP_cat(_tag_dyneither(_tmp22B,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1168
s=({struct Cyc_PP_Doc*_tmp22D[4U];({struct Cyc_PP_Doc*_tmp62F=Cyc_PP_text(({const char*_tmp22E="rnew(";_tag_dyneither(_tmp22E,sizeof(char),6U);}));_tmp22D[0]=_tmp62F;}),({struct Cyc_PP_Doc*_tmp62E=Cyc_Absynpp_exp2doc(_tmp2AF);_tmp22D[1]=_tmp62E;}),({struct Cyc_PP_Doc*_tmp62D=Cyc_PP_text(({const char*_tmp22F=") ";_tag_dyneither(_tmp22F,sizeof(char),3U);}));_tmp22D[2]=_tmp62D;}),({
struct Cyc_PP_Doc*_tmp62C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AE);_tmp22D[3]=_tmp62C;});Cyc_PP_cat(_tag_dyneither(_tmp22D,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;case 17U: _LL23: _tmp2B0=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL24:
 s=({struct Cyc_PP_Doc*_tmp230[3U];({struct Cyc_PP_Doc*_tmp632=Cyc_PP_text(({const char*_tmp231="sizeof(";_tag_dyneither(_tmp231,sizeof(char),8U);}));_tmp230[0]=_tmp632;}),({struct Cyc_PP_Doc*_tmp631=Cyc_Absynpp_typ2doc(_tmp2B0);_tmp230[1]=_tmp631;}),({struct Cyc_PP_Doc*_tmp630=Cyc_PP_text(({const char*_tmp232=")";_tag_dyneither(_tmp232,sizeof(char),2U);}));_tmp230[2]=_tmp630;});Cyc_PP_cat(_tag_dyneither(_tmp230,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 18U: _LL25: _tmp2B1=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL26:
 s=({struct Cyc_PP_Doc*_tmp233[3U];({struct Cyc_PP_Doc*_tmp635=Cyc_PP_text(({const char*_tmp234="sizeof(";_tag_dyneither(_tmp234,sizeof(char),8U);}));_tmp233[0]=_tmp635;}),({struct Cyc_PP_Doc*_tmp634=Cyc_Absynpp_exp2doc(_tmp2B1);_tmp233[1]=_tmp634;}),({struct Cyc_PP_Doc*_tmp633=Cyc_PP_text(({const char*_tmp235=")";_tag_dyneither(_tmp235,sizeof(char),2U);}));_tmp233[2]=_tmp633;});Cyc_PP_cat(_tag_dyneither(_tmp233,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 41U: _LL27: _tmp2B2=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL28:
 s=({struct Cyc_PP_Doc*_tmp236[3U];({struct Cyc_PP_Doc*_tmp638=Cyc_PP_text(({const char*_tmp237="__extension__(";_tag_dyneither(_tmp237,sizeof(char),15U);}));_tmp236[0]=_tmp638;}),({struct Cyc_PP_Doc*_tmp637=Cyc_Absynpp_exp2doc(_tmp2B2);_tmp236[1]=_tmp637;}),({struct Cyc_PP_Doc*_tmp636=Cyc_PP_text(({const char*_tmp238=")";_tag_dyneither(_tmp238,sizeof(char),2U);}));_tmp236[2]=_tmp636;});Cyc_PP_cat(_tag_dyneither(_tmp236,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 39U: _LL29: _tmp2B3=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL2A:
 s=({struct Cyc_PP_Doc*_tmp239[3U];({struct Cyc_PP_Doc*_tmp63B=Cyc_PP_text(({const char*_tmp23A="valueof(";_tag_dyneither(_tmp23A,sizeof(char),9U);}));_tmp239[0]=_tmp63B;}),({struct Cyc_PP_Doc*_tmp63A=Cyc_Absynpp_typ2doc(_tmp2B3);_tmp239[1]=_tmp63A;}),({struct Cyc_PP_Doc*_tmp639=Cyc_PP_text(({const char*_tmp23B=")";_tag_dyneither(_tmp23B,sizeof(char),2U);}));_tmp239[2]=_tmp639;});Cyc_PP_cat(_tag_dyneither(_tmp239,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 40U: _LL2B: _tmp2B5=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2B4=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL2C:
# 1176
 if(_tmp2B5)
s=({struct Cyc_PP_Doc*_tmp23C[4U];({struct Cyc_PP_Doc*_tmp63F=Cyc_PP_text(({const char*_tmp23D="__asm__";_tag_dyneither(_tmp23D,sizeof(char),8U);}));_tmp23C[0]=_tmp63F;}),({struct Cyc_PP_Doc*_tmp63E=Cyc_PP_text(({const char*_tmp23E=" volatile (";_tag_dyneither(_tmp23E,sizeof(char),12U);}));_tmp23C[1]=_tmp63E;}),({struct Cyc_PP_Doc*_tmp63D=Cyc_PP_text(_tmp2B4);_tmp23C[2]=_tmp63D;}),({struct Cyc_PP_Doc*_tmp63C=Cyc_PP_text(({const char*_tmp23F=")";_tag_dyneither(_tmp23F,sizeof(char),2U);}));_tmp23C[3]=_tmp63C;});Cyc_PP_cat(_tag_dyneither(_tmp23C,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1179
s=({struct Cyc_PP_Doc*_tmp240[3U];({struct Cyc_PP_Doc*_tmp642=Cyc_PP_text(({const char*_tmp241="__asm__(";_tag_dyneither(_tmp241,sizeof(char),9U);}));_tmp240[0]=_tmp642;}),({struct Cyc_PP_Doc*_tmp641=Cyc_PP_text(_tmp2B4);_tmp240[1]=_tmp641;}),({struct Cyc_PP_Doc*_tmp640=Cyc_PP_text(({const char*_tmp242=")";_tag_dyneither(_tmp242,sizeof(char),2U);}));_tmp240[2]=_tmp640;});Cyc_PP_cat(_tag_dyneither(_tmp240,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 38U: _LL2D: _tmp2B7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2B6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL2E:
# 1182
 s=({struct Cyc_PP_Doc*_tmp243[5U];({struct Cyc_PP_Doc*_tmp647=Cyc_PP_text(({const char*_tmp244="tagcheck(";_tag_dyneither(_tmp244,sizeof(char),10U);}));_tmp243[0]=_tmp647;}),({struct Cyc_PP_Doc*_tmp646=Cyc_Absynpp_exp2doc(_tmp2B7);_tmp243[1]=_tmp646;}),({struct Cyc_PP_Doc*_tmp645=Cyc_PP_text(({const char*_tmp245=".";_tag_dyneither(_tmp245,sizeof(char),2U);}));_tmp243[2]=_tmp645;}),({struct Cyc_PP_Doc*_tmp644=Cyc_PP_textptr(_tmp2B6);_tmp243[3]=_tmp644;}),({struct Cyc_PP_Doc*_tmp643=Cyc_PP_text(({const char*_tmp246=")";_tag_dyneither(_tmp246,sizeof(char),2U);}));_tmp243[4]=_tmp643;});Cyc_PP_cat(_tag_dyneither(_tmp243,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 19U: _LL2F: _tmp2B9=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2B8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL30:
# 1185
 s=({struct Cyc_PP_Doc*_tmp247[3U];({struct Cyc_PP_Doc*_tmp64A=Cyc_PP_text(({const char*_tmp248="offsetof(";_tag_dyneither(_tmp248,sizeof(char),10U);}));_tmp247[0]=_tmp64A;}),({struct Cyc_PP_Doc*_tmp649=Cyc_Absynpp_typ2doc(_tmp2B9);_tmp247[1]=_tmp649;}),({struct Cyc_PP_Doc*_tmp648=Cyc_PP_text(({const char*_tmp249=",";_tag_dyneither(_tmp249,sizeof(char),2U);}));_tmp247[2]=_tmp648;});Cyc_PP_cat(_tag_dyneither(_tmp247,sizeof(struct Cyc_PP_Doc*),3U));});
for(0;_tmp2B8 != 0;_tmp2B8=_tmp2B8->tl){
void*_tmp24A=(void*)_tmp2B8->hd;void*_tmp24B=_tmp24A;unsigned int _tmp254;struct _dyneither_ptr*_tmp253;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24B)->tag == 0U){_LL5F: _tmp253=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24B)->f1;_LL60:
 s=({struct Cyc_PP_Doc*_tmp24C[3U];_tmp24C[0]=s,({struct Cyc_PP_Doc*_tmp64D=Cyc_PP_textptr(_tmp253);_tmp24C[1]=_tmp64D;}),
_tmp2B8->tl != 0?({struct Cyc_PP_Doc*_tmp64C=Cyc_PP_text(({const char*_tmp24D=".";_tag_dyneither(_tmp24D,sizeof(char),2U);}));_tmp24C[2]=_tmp64C;}):({struct Cyc_PP_Doc*_tmp64B=Cyc_PP_nil_doc();_tmp24C[2]=_tmp64B;});Cyc_PP_cat(_tag_dyneither(_tmp24C,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL5E;}else{_LL61: _tmp254=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp24B)->f1;_LL62:
 s=({struct Cyc_PP_Doc*_tmp24E[3U];_tmp24E[0]=s,({struct Cyc_PP_Doc*_tmp651=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp251=({struct Cyc_Int_pa_PrintArg_struct _tmp53B;_tmp53B.tag=1U,_tmp53B.f1=(unsigned long)((int)_tmp254);_tmp53B;});void*_tmp24F[1U];_tmp24F[0]=& _tmp251;({struct _dyneither_ptr _tmp650=({const char*_tmp250="%d";_tag_dyneither(_tmp250,sizeof(char),3U);});Cyc_aprintf(_tmp650,_tag_dyneither(_tmp24F,sizeof(void*),1U));});}));_tmp24E[1]=_tmp651;}),
_tmp2B8->tl != 0?({struct Cyc_PP_Doc*_tmp64F=Cyc_PP_text(({const char*_tmp252=".";_tag_dyneither(_tmp252,sizeof(char),2U);}));_tmp24E[2]=_tmp64F;}):({struct Cyc_PP_Doc*_tmp64E=Cyc_PP_nil_doc();_tmp24E[2]=_tmp64E;});Cyc_PP_cat(_tag_dyneither(_tmp24E,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL5E;}_LL5E:;}
# 1195
s=({struct Cyc_PP_Doc*_tmp255[2U];_tmp255[0]=s,({struct Cyc_PP_Doc*_tmp652=Cyc_PP_text(({const char*_tmp256=")";_tag_dyneither(_tmp256,sizeof(char),2U);}));_tmp255[1]=_tmp652;});Cyc_PP_cat(_tag_dyneither(_tmp255,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 20U: _LL31: _tmp2BA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL32:
# 1198
 s=({struct Cyc_PP_Doc*_tmp257[2U];({struct Cyc_PP_Doc*_tmp654=Cyc_PP_text(({const char*_tmp258="*";_tag_dyneither(_tmp258,sizeof(char),2U);}));_tmp257[0]=_tmp654;}),({struct Cyc_PP_Doc*_tmp653=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BA);_tmp257[1]=_tmp653;});Cyc_PP_cat(_tag_dyneither(_tmp257,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 21U: _LL33: _tmp2BC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2BB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL34:
# 1201
 s=({struct Cyc_PP_Doc*_tmp259[3U];({struct Cyc_PP_Doc*_tmp657=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BC);_tmp259[0]=_tmp657;}),({struct Cyc_PP_Doc*_tmp656=Cyc_PP_text(({const char*_tmp25A=".";_tag_dyneither(_tmp25A,sizeof(char),2U);}));_tmp259[1]=_tmp656;}),({struct Cyc_PP_Doc*_tmp655=Cyc_PP_textptr(_tmp2BB);_tmp259[2]=_tmp655;});Cyc_PP_cat(_tag_dyneither(_tmp259,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 22U: _LL35: _tmp2BE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2BD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL36:
# 1204
 s=({struct Cyc_PP_Doc*_tmp25B[3U];({struct Cyc_PP_Doc*_tmp65A=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BE);_tmp25B[0]=_tmp65A;}),({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(({const char*_tmp25C="->";_tag_dyneither(_tmp25C,sizeof(char),3U);}));_tmp25B[1]=_tmp659;}),({struct Cyc_PP_Doc*_tmp658=Cyc_PP_textptr(_tmp2BD);_tmp25B[2]=_tmp658;});Cyc_PP_cat(_tag_dyneither(_tmp25B,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 23U: _LL37: _tmp2C0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2BF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL38:
# 1207
 s=({struct Cyc_PP_Doc*_tmp25D[4U];({struct Cyc_PP_Doc*_tmp65E=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C0);_tmp25D[0]=_tmp65E;}),({
struct Cyc_PP_Doc*_tmp65D=Cyc_PP_text(({const char*_tmp25E="[";_tag_dyneither(_tmp25E,sizeof(char),2U);}));_tmp25D[1]=_tmp65D;}),({
struct Cyc_PP_Doc*_tmp65C=Cyc_Absynpp_exp2doc(_tmp2BF);_tmp25D[2]=_tmp65C;}),({
struct Cyc_PP_Doc*_tmp65B=Cyc_PP_text(({const char*_tmp25F="]";_tag_dyneither(_tmp25F,sizeof(char),2U);}));_tmp25D[3]=_tmp65B;});Cyc_PP_cat(_tag_dyneither(_tmp25D,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 24U: _LL39: _tmp2C1=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL3A:
# 1213
 s=({struct Cyc_PP_Doc*_tmp260[4U];({struct Cyc_PP_Doc*_tmp662=Cyc_Absynpp_dollar();_tmp260[0]=_tmp662;}),({
struct Cyc_PP_Doc*_tmp661=Cyc_PP_text(({const char*_tmp261="(";_tag_dyneither(_tmp261,sizeof(char),2U);}));_tmp260[1]=_tmp661;}),({
struct Cyc_PP_Doc*_tmp660=Cyc_Absynpp_exps2doc_prec(20,_tmp2C1);_tmp260[2]=_tmp660;}),({
struct Cyc_PP_Doc*_tmp65F=Cyc_PP_text(({const char*_tmp262=")";_tag_dyneither(_tmp262,sizeof(char),2U);}));_tmp260[3]=_tmp65F;});Cyc_PP_cat(_tag_dyneither(_tmp260,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 25U: _LL3B: _tmp2C3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2C2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL3C:
# 1219
 s=({struct Cyc_PP_Doc*_tmp263[4U];({struct Cyc_PP_Doc*_tmp667=Cyc_PP_text(({const char*_tmp264="(";_tag_dyneither(_tmp264,sizeof(char),2U);}));_tmp263[0]=_tmp667;}),({
struct Cyc_PP_Doc*_tmp666=Cyc_Absynpp_typ2doc((*_tmp2C3).f3);_tmp263[1]=_tmp666;}),({
struct Cyc_PP_Doc*_tmp665=Cyc_PP_text(({const char*_tmp265=")";_tag_dyneither(_tmp265,sizeof(char),2U);}));_tmp263[2]=_tmp665;}),({
struct Cyc_PP_Doc*_tmp664=({struct _dyneither_ptr _tmp663=({const char*_tmp266=",";_tag_dyneither(_tmp266,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp663,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C2));});_tmp263[3]=_tmp664;});Cyc_PP_cat(_tag_dyneither(_tmp263,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 26U: _LL3D: _tmp2C4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL3E:
# 1225
 s=({struct _dyneither_ptr _tmp668=({const char*_tmp267=",";_tag_dyneither(_tmp267,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp668,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C4));});
goto _LL0;case 27U: _LL3F: _tmp2C7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2C6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_tmp2C5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp206)->f3;_LL40:
# 1228
 s=({struct Cyc_PP_Doc*_tmp268[8U];({struct Cyc_PP_Doc*_tmp670=Cyc_Absynpp_lb();_tmp268[0]=_tmp670;}),({struct Cyc_PP_Doc*_tmp66F=Cyc_PP_text(({const char*_tmp269="for ";_tag_dyneither(_tmp269,sizeof(char),5U);}));_tmp268[1]=_tmp66F;}),({
struct Cyc_PP_Doc*_tmp66E=Cyc_PP_text(*(*_tmp2C7->name).f2);_tmp268[2]=_tmp66E;}),({
struct Cyc_PP_Doc*_tmp66D=Cyc_PP_text(({const char*_tmp26A=" < ";_tag_dyneither(_tmp26A,sizeof(char),4U);}));_tmp268[3]=_tmp66D;}),({
struct Cyc_PP_Doc*_tmp66C=Cyc_Absynpp_exp2doc(_tmp2C6);_tmp268[4]=_tmp66C;}),({
struct Cyc_PP_Doc*_tmp66B=Cyc_PP_text(({const char*_tmp26B=" : ";_tag_dyneither(_tmp26B,sizeof(char),4U);}));_tmp268[5]=_tmp66B;}),({
struct Cyc_PP_Doc*_tmp66A=Cyc_Absynpp_exp2doc(_tmp2C5);_tmp268[6]=_tmp66A;}),({
struct Cyc_PP_Doc*_tmp669=Cyc_Absynpp_rb();_tmp268[7]=_tmp669;});Cyc_PP_cat(_tag_dyneither(_tmp268,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;case 28U: _LL41: _tmp2C9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2C8=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL42:
# 1237
 s=({struct Cyc_PP_Doc*_tmp26C[7U];({struct Cyc_PP_Doc*_tmp677=Cyc_Absynpp_lb();_tmp26C[0]=_tmp677;}),({struct Cyc_PP_Doc*_tmp676=Cyc_PP_text(({const char*_tmp26D="for x ";_tag_dyneither(_tmp26D,sizeof(char),7U);}));_tmp26C[1]=_tmp676;}),({
struct Cyc_PP_Doc*_tmp675=Cyc_PP_text(({const char*_tmp26E=" < ";_tag_dyneither(_tmp26E,sizeof(char),4U);}));_tmp26C[2]=_tmp675;}),({
struct Cyc_PP_Doc*_tmp674=Cyc_Absynpp_exp2doc(_tmp2C9);_tmp26C[3]=_tmp674;}),({
struct Cyc_PP_Doc*_tmp673=Cyc_PP_text(({const char*_tmp26F=" : ";_tag_dyneither(_tmp26F,sizeof(char),4U);}));_tmp26C[4]=_tmp673;}),({
struct Cyc_PP_Doc*_tmp672=Cyc_Absynpp_typ2doc(_tmp2C8);_tmp26C[5]=_tmp672;}),({
struct Cyc_PP_Doc*_tmp671=Cyc_Absynpp_rb();_tmp26C[6]=_tmp671;});Cyc_PP_cat(_tag_dyneither(_tmp26C,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 29U: _LL43: _tmp2CC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2CB=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_tmp2CA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp206)->f3;_LL44: {
# 1245
struct Cyc_List_List*_tmp270=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2CA);
s=({struct Cyc_PP_Doc*_tmp271[2U];({struct Cyc_PP_Doc*_tmp67B=Cyc_Absynpp_qvar2doc(_tmp2CC);_tmp271[0]=_tmp67B;}),({
struct Cyc_PP_Doc*_tmp67A=({struct _dyneither_ptr _tmp679=({const char*_tmp272=",";_tag_dyneither(_tmp272,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp679,
_tmp2CB != 0?({struct Cyc_List_List*_tmp273=_cycalloc(sizeof(*_tmp273));({struct Cyc_PP_Doc*_tmp678=Cyc_Absynpp_tps2doc(_tmp2CB);_tmp273->hd=_tmp678;}),_tmp273->tl=_tmp270;_tmp273;}): _tmp270);});
# 1247
_tmp271[1]=_tmp67A;});Cyc_PP_cat(_tag_dyneither(_tmp271,sizeof(struct Cyc_PP_Doc*),2U));});
# 1249
goto _LL0;}case 30U: _LL45: _tmp2CD=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL46:
# 1251
 s=({struct _dyneither_ptr _tmp67C=({const char*_tmp274=",";_tag_dyneither(_tmp274,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp67C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2CD));});
goto _LL0;case 31U: _LL47: _tmp2CF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2CE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp206)->f3;_LL48:
# 1254
 if(_tmp2CF == 0)
# 1256
s=Cyc_Absynpp_qvar2doc(_tmp2CE->name);else{
# 1258
s=({struct Cyc_PP_Doc*_tmp275[2U];({struct Cyc_PP_Doc*_tmp681=Cyc_Absynpp_qvar2doc(_tmp2CE->name);_tmp275[0]=_tmp681;}),({
struct Cyc_PP_Doc*_tmp680=({struct _dyneither_ptr _tmp67F=({const char*_tmp276="(";_tag_dyneither(_tmp276,sizeof(char),2U);});struct _dyneither_ptr _tmp67E=({const char*_tmp277=")";_tag_dyneither(_tmp277,sizeof(char),2U);});struct _dyneither_ptr _tmp67D=({const char*_tmp278=",";_tag_dyneither(_tmp278,sizeof(char),2U);});Cyc_PP_egroup(_tmp67F,_tmp67E,_tmp67D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2CF));});_tmp275[1]=_tmp680;});Cyc_PP_cat(_tag_dyneither(_tmp275,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;case 32U: _LL49: _tmp2D0=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL4A:
# 1262
 s=Cyc_Absynpp_qvar2doc(_tmp2D0->name);
goto _LL0;case 33U: _LL4B: _tmp2D1=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL4C:
# 1265
 s=Cyc_Absynpp_qvar2doc(_tmp2D1->name);
goto _LL0;case 34U: _LL4D: _tmp2D6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp206)->f1).is_calloc;_tmp2D5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp206)->f1).rgn;_tmp2D4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp206)->f1).elt_type;_tmp2D3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp206)->f1).num_elts;_tmp2D2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp206)->f1).inline_call;_LL4E:
# 1270
 if(_tmp2D6){
# 1272
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(_tmp2D4)),0U);
if(_tmp2D5 == 0)
s=({struct Cyc_PP_Doc*_tmp279[5U];({struct Cyc_PP_Doc*_tmp686=Cyc_PP_text(({const char*_tmp27A="calloc(";_tag_dyneither(_tmp27A,sizeof(char),8U);}));_tmp279[0]=_tmp686;}),({struct Cyc_PP_Doc*_tmp685=Cyc_Absynpp_exp2doc(_tmp2D3);_tmp279[1]=_tmp685;}),({struct Cyc_PP_Doc*_tmp684=Cyc_PP_text(({const char*_tmp27B=",";_tag_dyneither(_tmp27B,sizeof(char),2U);}));_tmp279[2]=_tmp684;}),({struct Cyc_PP_Doc*_tmp683=Cyc_Absynpp_exp2doc(st);_tmp279[3]=_tmp683;}),({struct Cyc_PP_Doc*_tmp682=Cyc_PP_text(({const char*_tmp27C=")";_tag_dyneither(_tmp27C,sizeof(char),2U);}));_tmp279[4]=_tmp682;});Cyc_PP_cat(_tag_dyneither(_tmp279,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1276
s=({struct Cyc_PP_Doc*_tmp27D[7U];({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_text(({const char*_tmp27E="rcalloc(";_tag_dyneither(_tmp27E,sizeof(char),9U);}));_tmp27D[0]=_tmp68D;}),({struct Cyc_PP_Doc*_tmp68C=Cyc_Absynpp_exp2doc(_tmp2D5);_tmp27D[1]=_tmp68C;}),({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(({const char*_tmp27F=",";_tag_dyneither(_tmp27F,sizeof(char),2U);}));_tmp27D[2]=_tmp68B;}),({
struct Cyc_PP_Doc*_tmp68A=Cyc_Absynpp_exp2doc(_tmp2D3);_tmp27D[3]=_tmp68A;}),({struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(({const char*_tmp280=",";_tag_dyneither(_tmp280,sizeof(char),2U);}));_tmp27D[4]=_tmp689;}),({struct Cyc_PP_Doc*_tmp688=Cyc_Absynpp_exp2doc(st);_tmp27D[5]=_tmp688;}),({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp281=")";_tag_dyneither(_tmp281,sizeof(char),2U);}));_tmp27D[6]=_tmp687;});Cyc_PP_cat(_tag_dyneither(_tmp27D,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1279
struct Cyc_Absyn_Exp*new_e;
# 1281
if(_tmp2D4 == 0)
new_e=_tmp2D3;else{
# 1284
new_e=({struct Cyc_Absyn_Exp*_tmp68E=Cyc_Absyn_sizeoftype_exp(*_tmp2D4,0U);Cyc_Absyn_times_exp(_tmp68E,_tmp2D3,0U);});}
# 1286
if(_tmp2D5 == 0)
s=({struct Cyc_PP_Doc*_tmp282[3U];({struct Cyc_PP_Doc*_tmp691=Cyc_PP_text(({const char*_tmp283="malloc(";_tag_dyneither(_tmp283,sizeof(char),8U);}));_tmp282[0]=_tmp691;}),({struct Cyc_PP_Doc*_tmp690=Cyc_Absynpp_exp2doc(new_e);_tmp282[1]=_tmp690;}),({struct Cyc_PP_Doc*_tmp68F=Cyc_PP_text(({const char*_tmp284=")";_tag_dyneither(_tmp284,sizeof(char),2U);}));_tmp282[2]=_tmp68F;});Cyc_PP_cat(_tag_dyneither(_tmp282,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1289
if(_tmp2D2)
s=({struct Cyc_PP_Doc*_tmp285[5U];({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp286="rmalloc_inline(";_tag_dyneither(_tmp286,sizeof(char),16U);}));_tmp285[0]=_tmp696;}),({struct Cyc_PP_Doc*_tmp695=Cyc_Absynpp_exp2doc(_tmp2D5);_tmp285[1]=_tmp695;}),({struct Cyc_PP_Doc*_tmp694=Cyc_PP_text(({const char*_tmp287=",";_tag_dyneither(_tmp287,sizeof(char),2U);}));_tmp285[2]=_tmp694;}),({
struct Cyc_PP_Doc*_tmp693=Cyc_Absynpp_exp2doc(new_e);_tmp285[3]=_tmp693;}),({struct Cyc_PP_Doc*_tmp692=Cyc_PP_text(({const char*_tmp288=")";_tag_dyneither(_tmp288,sizeof(char),2U);}));_tmp285[4]=_tmp692;});Cyc_PP_cat(_tag_dyneither(_tmp285,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1293
s=({struct Cyc_PP_Doc*_tmp289[5U];({struct Cyc_PP_Doc*_tmp69B=Cyc_PP_text(({const char*_tmp28A="rmalloc(";_tag_dyneither(_tmp28A,sizeof(char),9U);}));_tmp289[0]=_tmp69B;}),({struct Cyc_PP_Doc*_tmp69A=Cyc_Absynpp_exp2doc(_tmp2D5);_tmp289[1]=_tmp69A;}),({struct Cyc_PP_Doc*_tmp699=Cyc_PP_text(({const char*_tmp28B=",";_tag_dyneither(_tmp28B,sizeof(char),2U);}));_tmp289[2]=_tmp699;}),({
struct Cyc_PP_Doc*_tmp698=Cyc_Absynpp_exp2doc(new_e);_tmp289[3]=_tmp698;}),({struct Cyc_PP_Doc*_tmp697=Cyc_PP_text(({const char*_tmp28C=")";_tag_dyneither(_tmp28C,sizeof(char),2U);}));_tmp289[4]=_tmp697;});Cyc_PP_cat(_tag_dyneither(_tmp289,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1297
goto _LL0;case 35U: _LL4F: _tmp2D8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2D7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL50:
# 1299
 s=({struct Cyc_PP_Doc*_tmp28D[3U];({struct Cyc_PP_Doc*_tmp69E=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D8);_tmp28D[0]=_tmp69E;}),({
struct Cyc_PP_Doc*_tmp69D=Cyc_PP_text(({const char*_tmp28E=" :=: ";_tag_dyneither(_tmp28E,sizeof(char),6U);}));_tmp28D[1]=_tmp69D;}),({
struct Cyc_PP_Doc*_tmp69C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D7);_tmp28D[2]=_tmp69C;});Cyc_PP_cat(_tag_dyneither(_tmp28D,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 36U: _LL51: _tmp2DA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_tmp2D9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp206)->f2;_LL52:
# 1305
 s=({struct _dyneither_ptr _tmp69F=({const char*_tmp28F=",";_tag_dyneither(_tmp28F,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp69F,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D9));});
goto _LL0;default: _LL53: _tmp2DB=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp206)->f1;_LL54:
# 1308
 s=({struct Cyc_PP_Doc*_tmp290[7U];({struct Cyc_PP_Doc*_tmp6A6=Cyc_PP_text(({const char*_tmp291="(";_tag_dyneither(_tmp291,sizeof(char),2U);}));_tmp290[0]=_tmp6A6;}),({struct Cyc_PP_Doc*_tmp6A5=Cyc_Absynpp_lb();_tmp290[1]=_tmp6A5;}),({struct Cyc_PP_Doc*_tmp6A4=Cyc_PP_blank_doc();_tmp290[2]=_tmp6A4;}),({
struct Cyc_PP_Doc*_tmp6A3=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2DB,1));_tmp290[3]=_tmp6A3;}),({
struct Cyc_PP_Doc*_tmp6A2=Cyc_PP_blank_doc();_tmp290[4]=_tmp6A2;}),({struct Cyc_PP_Doc*_tmp6A1=Cyc_Absynpp_rb();_tmp290[5]=_tmp6A1;}),({struct Cyc_PP_Doc*_tmp6A0=Cyc_PP_text(({const char*_tmp292=")";_tag_dyneither(_tmp292,sizeof(char),2U);}));_tmp290[6]=_tmp6A0;});Cyc_PP_cat(_tag_dyneither(_tmp290,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}_LL0:;}
# 1313
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2DC[3U];({struct Cyc_PP_Doc*_tmp6A8=Cyc_PP_text(({const char*_tmp2DD="(";_tag_dyneither(_tmp2DD,sizeof(char),2U);}));_tmp2DC[0]=_tmp6A8;}),_tmp2DC[1]=s,({struct Cyc_PP_Doc*_tmp6A7=Cyc_PP_text(({const char*_tmp2DE=")";_tag_dyneither(_tmp2DE,sizeof(char),2U);}));_tmp2DC[2]=_tmp6A7;});Cyc_PP_cat(_tag_dyneither(_tmp2DC,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1318
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2DF=d;struct _dyneither_ptr*_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E5;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2DF)->tag == 0U){_LL1: _tmp2E5=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2DF)->f1;_LL2:
 return({struct Cyc_PP_Doc*_tmp2E0[3U];({struct Cyc_PP_Doc*_tmp6AB=Cyc_PP_text(({const char*_tmp2E1=".[";_tag_dyneither(_tmp2E1,sizeof(char),3U);}));_tmp2E0[0]=_tmp6AB;}),({struct Cyc_PP_Doc*_tmp6AA=Cyc_Absynpp_exp2doc(_tmp2E5);_tmp2E0[1]=_tmp6AA;}),({struct Cyc_PP_Doc*_tmp6A9=Cyc_PP_text(({const char*_tmp2E2="]";_tag_dyneither(_tmp2E2,sizeof(char),2U);}));_tmp2E0[2]=_tmp6A9;});Cyc_PP_cat(_tag_dyneither(_tmp2E0,sizeof(struct Cyc_PP_Doc*),3U));});}else{_LL3: _tmp2E6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2DF)->f1;_LL4:
 return({struct Cyc_PP_Doc*_tmp2E3[2U];({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp2E4=".";_tag_dyneither(_tmp2E4,sizeof(char),2U);}));_tmp2E3[0]=_tmp6AD;}),({struct Cyc_PP_Doc*_tmp6AC=Cyc_PP_textptr(_tmp2E6);_tmp2E3[1]=_tmp6AC;});Cyc_PP_cat(_tag_dyneither(_tmp2E3,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}
# 1325
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2E7[2U];({struct Cyc_PP_Doc*_tmp6B2=({struct _dyneither_ptr _tmp6B1=({const char*_tmp2E8="";_tag_dyneither(_tmp2E8,sizeof(char),1U);});struct _dyneither_ptr _tmp6B0=({const char*_tmp2E9="=";_tag_dyneither(_tmp2E9,sizeof(char),2U);});struct _dyneither_ptr _tmp6AF=({const char*_tmp2EA="=";_tag_dyneither(_tmp2EA,sizeof(char),2U);});Cyc_PP_egroup(_tmp6B1,_tmp6B0,_tmp6AF,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp2E7[0]=_tmp6B2;}),({
struct Cyc_PP_Doc*_tmp6AE=Cyc_Absynpp_exp2doc((*de).f2);_tmp2E7[1]=_tmp6AE;});Cyc_PP_cat(_tag_dyneither(_tmp2E7,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1331
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _dyneither_ptr _tmp6B5=({const char*_tmp2EB="";_tag_dyneither(_tmp2EB,sizeof(char),1U);});struct _dyneither_ptr _tmp6B4=({const char*_tmp2EC="";_tag_dyneither(_tmp2EC,sizeof(char),1U);});struct _dyneither_ptr _tmp6B3=({const char*_tmp2ED=",";_tag_dyneither(_tmp2ED,sizeof(char),2U);});Cyc_PP_group(_tmp6B5,_tmp6B4,_tmp6B3,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1335
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _dyneither_ptr x=({char*_tmp302=({unsigned int _tmp301=28U + 1U;char*_tmp300=_cycalloc_atomic(_check_times(_tmp301,sizeof(char)));({{unsigned int _tmp53C=28U;unsigned int i;for(i=0;i < _tmp53C;++ i){_tmp300[i]='z';}_tmp300[_tmp53C]=0;}0;});_tmp300;});_tag_dyneither(_tmp302,sizeof(char),29U);});
({struct _dyneither_ptr _tmp2EE=_dyneither_ptr_plus(x,sizeof(char),27);char _tmp2EF=*((char*)_check_dyneither_subscript(_tmp2EE,sizeof(char),0U));char _tmp2F0='\000';if(_get_dyneither_size(_tmp2EE,sizeof(char))== 1U  && (_tmp2EF == 0  && _tmp2F0 != 0))_throw_arraybounds();*((char*)_tmp2EE.curr)=_tmp2F0;});
({struct _dyneither_ptr _tmp2F1=_dyneither_ptr_plus(x,sizeof(char),26);char _tmp2F2=*((char*)_check_dyneither_subscript(_tmp2F1,sizeof(char),0U));char _tmp2F3='L';if(_get_dyneither_size(_tmp2F1,sizeof(char))== 1U  && (_tmp2F2 == 0  && _tmp2F3 != 0))_throw_arraybounds();*((char*)_tmp2F1.curr)=_tmp2F3;});
({struct _dyneither_ptr _tmp2F4=_dyneither_ptr_plus(x,sizeof(char),25);char _tmp2F5=*((char*)_check_dyneither_subscript(_tmp2F4,sizeof(char),0U));char _tmp2F6='L';if(_get_dyneither_size(_tmp2F4,sizeof(char))== 1U  && (_tmp2F5 == 0  && _tmp2F6 != 0))_throw_arraybounds();*((char*)_tmp2F4.curr)=_tmp2F6;});
({struct _dyneither_ptr _tmp2F7=_dyneither_ptr_plus(x,sizeof(char),24);char _tmp2F8=*((char*)_check_dyneither_subscript(_tmp2F7,sizeof(char),0U));char _tmp2F9='U';if(_get_dyneither_size(_tmp2F7,sizeof(char))== 1U  && (_tmp2F8 == 0  && _tmp2F9 != 0))_throw_arraybounds();*((char*)_tmp2F7.curr)=_tmp2F9;});
({struct _dyneither_ptr _tmp2FA=_dyneither_ptr_plus(x,sizeof(char),23);char _tmp2FB=*((char*)_check_dyneither_subscript(_tmp2FA,sizeof(char),0U));char _tmp2FC='0';if(_get_dyneither_size(_tmp2FA,sizeof(char))== 1U  && (_tmp2FB == 0  && _tmp2FC != 0))_throw_arraybounds();*((char*)_tmp2FA.curr)=_tmp2FC;});{
int index=23;
while(i != (unsigned long long)0){
char c=(char)((unsigned long long)'0' + i % (unsigned long long)10);
({struct _dyneither_ptr _tmp2FD=_dyneither_ptr_plus(x,sizeof(char),index);char _tmp2FE=*((char*)_check_dyneither_subscript(_tmp2FD,sizeof(char),0U));char _tmp2FF=c;if(_get_dyneither_size(_tmp2FD,sizeof(char))== 1U  && (_tmp2FE == 0  && _tmp2FF != 0))_throw_arraybounds();*((char*)_tmp2FD.curr)=_tmp2FF;});
i=i / (unsigned long long)10;
-- index;}
# 1349
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1353
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp303=c;struct _dyneither_ptr _tmp325;struct _dyneither_ptr _tmp324;struct _dyneither_ptr _tmp323;enum Cyc_Absyn_Sign _tmp322;long long _tmp321;enum Cyc_Absyn_Sign _tmp320;int _tmp31F;enum Cyc_Absyn_Sign _tmp31E;short _tmp31D;struct _dyneither_ptr _tmp31C;enum Cyc_Absyn_Sign _tmp31B;char _tmp31A;switch((_tmp303.String_c).tag){case 2U: _LL1: _tmp31B=((_tmp303.Char_c).val).f1;_tmp31A=((_tmp303.Char_c).val).f2;_LL2:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp306=({struct Cyc_String_pa_PrintArg_struct _tmp53D;_tmp53D.tag=0U,({struct _dyneither_ptr _tmp6B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp31A));_tmp53D.f1=_tmp6B6;});_tmp53D;});void*_tmp304[1U];_tmp304[0]=& _tmp306;({struct _dyneither_ptr _tmp6B7=({const char*_tmp305="'%s'";_tag_dyneither(_tmp305,sizeof(char),5U);});Cyc_aprintf(_tmp6B7,_tag_dyneither(_tmp304,sizeof(void*),1U));});}));case 3U: _LL3: _tmp31C=(_tmp303.Wchar_c).val;_LL4:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp309=({struct Cyc_String_pa_PrintArg_struct _tmp53E;_tmp53E.tag=0U,_tmp53E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31C);_tmp53E;});void*_tmp307[1U];_tmp307[0]=& _tmp309;({struct _dyneither_ptr _tmp6B8=({const char*_tmp308="L'%s'";_tag_dyneither(_tmp308,sizeof(char),6U);});Cyc_aprintf(_tmp6B8,_tag_dyneither(_tmp307,sizeof(void*),1U));});}));case 4U: _LL5: _tmp31E=((_tmp303.Short_c).val).f1;_tmp31D=((_tmp303.Short_c).val).f2;_LL6:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30C=({struct Cyc_Int_pa_PrintArg_struct _tmp53F;_tmp53F.tag=1U,_tmp53F.f1=(unsigned long)((int)_tmp31D);_tmp53F;});void*_tmp30A[1U];_tmp30A[0]=& _tmp30C;({struct _dyneither_ptr _tmp6B9=({const char*_tmp30B="%d";_tag_dyneither(_tmp30B,sizeof(char),3U);});Cyc_aprintf(_tmp6B9,_tag_dyneither(_tmp30A,sizeof(void*),1U));});}));case 5U: _LL7: _tmp320=((_tmp303.Int_c).val).f1;_tmp31F=((_tmp303.Int_c).val).f2;_LL8:
# 1359
 if((int)_tmp320 == (int)Cyc_Absyn_Unsigned)
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30F=({struct Cyc_Int_pa_PrintArg_struct _tmp540;_tmp540.tag=1U,_tmp540.f1=(unsigned int)_tmp31F;_tmp540;});void*_tmp30D[1U];_tmp30D[0]=& _tmp30F;({struct _dyneither_ptr _tmp6BA=({const char*_tmp30E="%uU";_tag_dyneither(_tmp30E,sizeof(char),4U);});Cyc_aprintf(_tmp6BA,_tag_dyneither(_tmp30D,sizeof(void*),1U));});}));else{
# 1362
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp312=({struct Cyc_Int_pa_PrintArg_struct _tmp541;_tmp541.tag=1U,_tmp541.f1=(unsigned long)_tmp31F;_tmp541;});void*_tmp310[1U];_tmp310[0]=& _tmp312;({struct _dyneither_ptr _tmp6BB=({const char*_tmp311="%d";_tag_dyneither(_tmp311,sizeof(char),3U);});Cyc_aprintf(_tmp6BB,_tag_dyneither(_tmp310,sizeof(void*),1U));});}));}case 6U: _LL9: _tmp322=((_tmp303.LongLong_c).val).f1;_tmp321=((_tmp303.LongLong_c).val).f2;_LLA:
# 1365
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp321));case 7U: _LLB: _tmp323=((_tmp303.Float_c).val).f1;_LLC:
 return Cyc_PP_text(_tmp323);case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp313="NULL";_tag_dyneither(_tmp313,sizeof(char),5U);}));case 8U: _LLF: _tmp324=(_tmp303.String_c).val;_LL10:
# 1369
 return({struct Cyc_PP_Doc*_tmp314[3U];({struct Cyc_PP_Doc*_tmp6BE=Cyc_PP_text(({const char*_tmp315="\"";_tag_dyneither(_tmp315,sizeof(char),2U);}));_tmp314[0]=_tmp6BE;}),({struct Cyc_PP_Doc*_tmp6BD=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp324));_tmp314[1]=_tmp6BD;}),({struct Cyc_PP_Doc*_tmp6BC=Cyc_PP_text(({const char*_tmp316="\"";_tag_dyneither(_tmp316,sizeof(char),2U);}));_tmp314[2]=_tmp6BC;});Cyc_PP_cat(_tag_dyneither(_tmp314,sizeof(struct Cyc_PP_Doc*),3U));});default: _LL11: _tmp325=(_tmp303.Wstring_c).val;_LL12:
# 1371
 return({struct Cyc_PP_Doc*_tmp317[3U];({struct Cyc_PP_Doc*_tmp6C1=Cyc_PP_text(({const char*_tmp318="L\"";_tag_dyneither(_tmp318,sizeof(char),3U);}));_tmp317[0]=_tmp6C1;}),({struct Cyc_PP_Doc*_tmp6C0=Cyc_PP_text(_tmp325);_tmp317[1]=_tmp6C0;}),({struct Cyc_PP_Doc*_tmp6BF=Cyc_PP_text(({const char*_tmp319="\"";_tag_dyneither(_tmp319,sizeof(char),2U);}));_tmp317[2]=_tmp6BF;});Cyc_PP_cat(_tag_dyneither(_tmp317,sizeof(struct Cyc_PP_Doc*),3U));});}_LL0:;}
# 1375
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp6C4=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp328=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,({struct _dyneither_ptr _tmp6C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp542.f1=_tmp6C2;});_tmp542;});void*_tmp326[1U];_tmp326[0]=& _tmp328;({struct _dyneither_ptr _tmp6C3=({const char*_tmp327="Absynpp::primapp2doc Numelts: %s with bad args";_tag_dyneither(_tmp327,sizeof(char),47U);});Cyc_aprintf(_tmp6C3,_tag_dyneither(_tmp326,sizeof(void*),1U));});});_tmp329->f1=_tmp6C4;});_tmp329;}));
# 1381
return({struct Cyc_PP_Doc*_tmp32A[3U];({struct Cyc_PP_Doc*_tmp6C7=Cyc_PP_text(({const char*_tmp32B="numelts(";_tag_dyneither(_tmp32B,sizeof(char),9U);}));_tmp32A[0]=_tmp6C7;}),({struct Cyc_PP_Doc*_tmp6C6=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp32A[1]=_tmp6C6;}),({struct Cyc_PP_Doc*_tmp6C5=Cyc_PP_text(({const char*_tmp32C=")";_tag_dyneither(_tmp32C,sizeof(char),2U);}));_tmp32A[2]=_tmp6C5;});Cyc_PP_cat(_tag_dyneither(_tmp32A,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1383
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp6CA=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32F=({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0U,({struct _dyneither_ptr _tmp6C8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp543.f1=_tmp6C8;});_tmp543;});void*_tmp32D[1U];_tmp32D[0]=& _tmp32F;({struct _dyneither_ptr _tmp6C9=({const char*_tmp32E="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp32E,sizeof(char),38U);});Cyc_aprintf(_tmp6C9,_tag_dyneither(_tmp32D,sizeof(void*),1U));});});_tmp330->f1=_tmp6CA;});_tmp330;}));else{
# 1387
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp331[3U];_tmp331[0]=ps,({struct Cyc_PP_Doc*_tmp6CB=Cyc_PP_text(({const char*_tmp332=" ";_tag_dyneither(_tmp332,sizeof(char),2U);}));_tmp331[1]=_tmp6CB;}),_tmp331[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_dyneither(_tmp331,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp6CE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp335=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,({struct _dyneither_ptr _tmp6CC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp544.f1=_tmp6CC;});_tmp544;});void*_tmp333[1U];_tmp333[0]=& _tmp335;({struct _dyneither_ptr _tmp6CD=({const char*_tmp334="Absynpp::primapp2doc: %s with more than 2 args";_tag_dyneither(_tmp334,sizeof(char),47U);});Cyc_aprintf(_tmp6CD,_tag_dyneither(_tmp333,sizeof(void*),1U));});});_tmp336->f1=_tmp6CE;});_tmp336;}));else{
# 1393
return({struct Cyc_PP_Doc*_tmp337[5U];_tmp337[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp6D0=Cyc_PP_text(({const char*_tmp338=" ";_tag_dyneither(_tmp338,sizeof(char),2U);}));_tmp337[1]=_tmp6D0;}),_tmp337[2]=ps,({struct Cyc_PP_Doc*_tmp6CF=Cyc_PP_text(({const char*_tmp339=" ";_tag_dyneither(_tmp339,sizeof(char),2U);}));_tmp337[3]=_tmp6CF;}),_tmp337[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_dyneither(_tmp337,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1397
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp33A=p;switch(_tmp33A){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp33B="+";_tag_dyneither(_tmp33B,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp33C="*";_tag_dyneither(_tmp33C,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp33D="-";_tag_dyneither(_tmp33D,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp33E="/";_tag_dyneither(_tmp33E,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp33F="\\%";_tag_dyneither(_tmp33F,sizeof(char),3U);}):({const char*_tmp340="%";_tag_dyneither(_tmp340,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp341="==";_tag_dyneither(_tmp341,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp342="!=";_tag_dyneither(_tmp342,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp343=">";_tag_dyneither(_tmp343,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp344="<";_tag_dyneither(_tmp344,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp345=">=";_tag_dyneither(_tmp345,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp346="<=";_tag_dyneither(_tmp346,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp347="!";_tag_dyneither(_tmp347,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp348="~";_tag_dyneither(_tmp348,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp349="&";_tag_dyneither(_tmp349,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp34A="|";_tag_dyneither(_tmp34A,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp34B="^";_tag_dyneither(_tmp34B,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp34C="<<";_tag_dyneither(_tmp34C,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp34D=">>";_tag_dyneither(_tmp34D,sizeof(char),3U);});case Cyc_Absyn_Bitarshift: _LL25: _LL26:
 return({const char*_tmp34E=">>>";_tag_dyneither(_tmp34E,sizeof(char),4U);});default: _LL27: _LL28:
 return({const char*_tmp34F="numelts";_tag_dyneither(_tmp34F,sizeof(char),8U);});}_LL0:;}
# 1422
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1426
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp350=s->r;void*_tmp351=_tmp350;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp351)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1433
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp352=st->r;void*_tmp353=_tmp352;struct Cyc_Absyn_Stmt*_tmp3AD;struct Cyc_List_List*_tmp3AC;struct Cyc_Absyn_Stmt*_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AA;struct _dyneither_ptr*_tmp3A9;struct Cyc_Absyn_Stmt*_tmp3A8;struct Cyc_Absyn_Decl*_tmp3A7;struct Cyc_Absyn_Stmt*_tmp3A6;struct Cyc_List_List*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A4;struct Cyc_List_List*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A0;struct Cyc_Absyn_Stmt*_tmp39F;struct _dyneither_ptr*_tmp39E;struct Cyc_Absyn_Exp*_tmp39D;struct Cyc_Absyn_Stmt*_tmp39C;struct Cyc_Absyn_Exp*_tmp39B;struct Cyc_Absyn_Stmt*_tmp39A;struct Cyc_Absyn_Stmt*_tmp399;struct Cyc_Absyn_Exp*_tmp398;struct Cyc_Absyn_Stmt*_tmp397;struct Cyc_Absyn_Stmt*_tmp396;struct Cyc_Absyn_Exp*_tmp395;switch(*((int*)_tmp353)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp354=";";_tag_dyneither(_tmp354,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp395=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_LL4:
 s=({struct Cyc_PP_Doc*_tmp355[2U];({struct Cyc_PP_Doc*_tmp6D2=Cyc_Absynpp_exp2doc(_tmp395);_tmp355[0]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_PP_text(({const char*_tmp356=";";_tag_dyneither(_tmp356,sizeof(char),2U);}));_tmp355[1]=_tmp6D1;});Cyc_PP_cat(_tag_dyneither(_tmp355,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 2U: _LL5: _tmp397=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_tmp396=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp353)->f2;_LL6:
# 1439
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp397))
s=({struct Cyc_PP_Doc*_tmp357[7U];({struct Cyc_PP_Doc*_tmp6E7=Cyc_Absynpp_lb();_tmp357[0]=_tmp6E7;}),({
struct Cyc_PP_Doc*_tmp6E6=Cyc_PP_blank_doc();_tmp357[1]=_tmp6E6;}),({
struct Cyc_PP_Doc*_tmp6E5=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp397,0));_tmp357[2]=_tmp6E5;}),({
struct Cyc_PP_Doc*_tmp6E4=Cyc_PP_line_doc();_tmp357[3]=_tmp6E4;}),({
struct Cyc_PP_Doc*_tmp6E3=Cyc_Absynpp_rb();_tmp357[4]=_tmp6E3;}),({
struct Cyc_PP_Doc*_tmp6E2=Cyc_PP_line_doc();_tmp357[5]=_tmp6E2;}),
Cyc_Absynpp_is_declaration(_tmp396)?
stmtexp?({
struct Cyc_PP_Doc*_tmp6E1=({struct Cyc_PP_Doc*_tmp358[7U];({struct Cyc_PP_Doc*_tmp6E0=Cyc_PP_text(({const char*_tmp359="(";_tag_dyneither(_tmp359,sizeof(char),2U);}));_tmp358[0]=_tmp6E0;}),({struct Cyc_PP_Doc*_tmp6DF=Cyc_Absynpp_lb();_tmp358[1]=_tmp6DF;}),({struct Cyc_PP_Doc*_tmp6DE=Cyc_PP_blank_doc();_tmp358[2]=_tmp6DE;}),({
struct Cyc_PP_Doc*_tmp6DD=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp396,stmtexp));_tmp358[3]=_tmp6DD;}),({
struct Cyc_PP_Doc*_tmp6DC=Cyc_Absynpp_rb();_tmp358[4]=_tmp6DC;}),({struct Cyc_PP_Doc*_tmp6DB=Cyc_PP_text(({const char*_tmp35A=");";_tag_dyneither(_tmp35A,sizeof(char),3U);}));_tmp358[5]=_tmp6DB;}),({
struct Cyc_PP_Doc*_tmp6DA=Cyc_PP_line_doc();_tmp358[6]=_tmp6DA;});Cyc_PP_cat(_tag_dyneither(_tmp358,sizeof(struct Cyc_PP_Doc*),7U));});
# 1449
_tmp357[6]=_tmp6E1;}):({
# 1453
struct Cyc_PP_Doc*_tmp6D9=({struct Cyc_PP_Doc*_tmp35B[5U];({struct Cyc_PP_Doc*_tmp6D8=Cyc_Absynpp_lb();_tmp35B[0]=_tmp6D8;}),({struct Cyc_PP_Doc*_tmp6D7=Cyc_PP_blank_doc();_tmp35B[1]=_tmp6D7;}),({
struct Cyc_PP_Doc*_tmp6D6=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp396,stmtexp));_tmp35B[2]=_tmp6D6;}),({
struct Cyc_PP_Doc*_tmp6D5=Cyc_Absynpp_rb();_tmp35B[3]=_tmp6D5;}),({
struct Cyc_PP_Doc*_tmp6D4=Cyc_PP_line_doc();_tmp35B[4]=_tmp6D4;});Cyc_PP_cat(_tag_dyneither(_tmp35B,sizeof(struct Cyc_PP_Doc*),5U));});
# 1453
_tmp357[6]=_tmp6D9;}):({
# 1457
struct Cyc_PP_Doc*_tmp6D3=Cyc_Absynpp_stmt2doc(_tmp396,stmtexp);_tmp357[6]=_tmp6D3;});Cyc_PP_cat(_tag_dyneither(_tmp357,sizeof(struct Cyc_PP_Doc*),7U));});else{
if(Cyc_Absynpp_is_declaration(_tmp396))
s=({struct Cyc_PP_Doc*_tmp35C[4U];({struct Cyc_PP_Doc*_tmp6F6=Cyc_Absynpp_stmt2doc(_tmp397,0);_tmp35C[0]=_tmp6F6;}),({
struct Cyc_PP_Doc*_tmp6F5=Cyc_PP_line_doc();_tmp35C[1]=_tmp6F5;}),
stmtexp?({
struct Cyc_PP_Doc*_tmp6F4=({struct Cyc_PP_Doc*_tmp35D[6U];({struct Cyc_PP_Doc*_tmp6F3=Cyc_PP_text(({const char*_tmp35E="(";_tag_dyneither(_tmp35E,sizeof(char),2U);}));_tmp35D[0]=_tmp6F3;}),({struct Cyc_PP_Doc*_tmp6F2=Cyc_Absynpp_lb();_tmp35D[1]=_tmp6F2;}),({struct Cyc_PP_Doc*_tmp6F1=Cyc_PP_blank_doc();_tmp35D[2]=_tmp6F1;}),({
struct Cyc_PP_Doc*_tmp6F0=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp396,stmtexp));_tmp35D[3]=_tmp6F0;}),({
struct Cyc_PP_Doc*_tmp6EF=Cyc_Absynpp_rb();_tmp35D[4]=_tmp6EF;}),({struct Cyc_PP_Doc*_tmp6EE=Cyc_PP_text(({const char*_tmp35F=");";_tag_dyneither(_tmp35F,sizeof(char),3U);}));_tmp35D[5]=_tmp6EE;});Cyc_PP_cat(_tag_dyneither(_tmp35D,sizeof(struct Cyc_PP_Doc*),6U));});
# 1462
_tmp35C[2]=_tmp6F4;}):({
# 1466
struct Cyc_PP_Doc*_tmp6ED=({struct Cyc_PP_Doc*_tmp360[4U];({struct Cyc_PP_Doc*_tmp6EC=Cyc_Absynpp_lb();_tmp360[0]=_tmp6EC;}),({struct Cyc_PP_Doc*_tmp6EB=Cyc_PP_blank_doc();_tmp360[1]=_tmp6EB;}),({
struct Cyc_PP_Doc*_tmp6EA=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp396,stmtexp));_tmp360[2]=_tmp6EA;}),({
struct Cyc_PP_Doc*_tmp6E9=Cyc_Absynpp_rb();_tmp360[3]=_tmp6E9;});Cyc_PP_cat(_tag_dyneither(_tmp360,sizeof(struct Cyc_PP_Doc*),4U));});
# 1466
_tmp35C[2]=_tmp6ED;}),({
# 1469
struct Cyc_PP_Doc*_tmp6E8=Cyc_PP_line_doc();_tmp35C[3]=_tmp6E8;});Cyc_PP_cat(_tag_dyneither(_tmp35C,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1471
s=({struct Cyc_PP_Doc*_tmp361[3U];({struct Cyc_PP_Doc*_tmp6F9=Cyc_Absynpp_stmt2doc(_tmp397,0);_tmp361[0]=_tmp6F9;}),({struct Cyc_PP_Doc*_tmp6F8=Cyc_PP_line_doc();_tmp361[1]=_tmp6F8;}),({struct Cyc_PP_Doc*_tmp6F7=Cyc_Absynpp_stmt2doc(_tmp396,stmtexp);_tmp361[2]=_tmp6F7;});Cyc_PP_cat(_tag_dyneither(_tmp361,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1474
s=({struct Cyc_PP_Doc*_tmp362[3U];({struct Cyc_PP_Doc*_tmp6FC=Cyc_Absynpp_stmt2doc(_tmp397,0);_tmp362[0]=_tmp6FC;}),({struct Cyc_PP_Doc*_tmp6FB=Cyc_PP_line_doc();_tmp362[1]=_tmp6FB;}),({struct Cyc_PP_Doc*_tmp6FA=Cyc_Absynpp_stmt2doc(_tmp396,stmtexp);_tmp362[2]=_tmp6FA;});Cyc_PP_cat(_tag_dyneither(_tmp362,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 3U: _LL7: _tmp398=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_LL8:
# 1477
 if(_tmp398 == 0)
s=Cyc_PP_text(({const char*_tmp363="return;";_tag_dyneither(_tmp363,sizeof(char),8U);}));else{
# 1480
s=({struct Cyc_PP_Doc*_tmp364[3U];({struct Cyc_PP_Doc*_tmp700=Cyc_PP_text(({const char*_tmp365="return ";_tag_dyneither(_tmp365,sizeof(char),8U);}));_tmp364[0]=_tmp700;}),
_tmp398 == 0?({struct Cyc_PP_Doc*_tmp6FF=Cyc_PP_nil_doc();_tmp364[1]=_tmp6FF;}):({struct Cyc_PP_Doc*_tmp6FE=Cyc_Absynpp_exp2doc(_tmp398);_tmp364[1]=_tmp6FE;}),({
struct Cyc_PP_Doc*_tmp6FD=Cyc_PP_text(({const char*_tmp366=";";_tag_dyneither(_tmp366,sizeof(char),2U);}));_tmp364[2]=_tmp6FD;});Cyc_PP_cat(_tag_dyneither(_tmp364,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 4U: _LL9: _tmp39B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_tmp39A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp353)->f2;_tmp399=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp353)->f3;_LLA: {
# 1485
int print_else;
{void*_tmp367=_tmp399->r;void*_tmp368=_tmp367;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp368)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1490
s=({struct Cyc_PP_Doc*_tmp369[8U];({struct Cyc_PP_Doc*_tmp713=Cyc_PP_text(({const char*_tmp36A="if (";_tag_dyneither(_tmp36A,sizeof(char),5U);}));_tmp369[0]=_tmp713;}),({
struct Cyc_PP_Doc*_tmp712=Cyc_Absynpp_exp2doc(_tmp39B);_tmp369[1]=_tmp712;}),({
struct Cyc_PP_Doc*_tmp711=Cyc_PP_text(({const char*_tmp36B=") ";_tag_dyneither(_tmp36B,sizeof(char),3U);}));_tmp369[2]=_tmp711;}),({
struct Cyc_PP_Doc*_tmp710=Cyc_Absynpp_lb();_tmp369[3]=_tmp710;}),({
struct Cyc_PP_Doc*_tmp70F=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp36C[2U];({struct Cyc_PP_Doc*_tmp70E=Cyc_PP_line_doc();_tmp36C[0]=_tmp70E;}),({struct Cyc_PP_Doc*_tmp70D=Cyc_Absynpp_stmt2doc(_tmp39A,0);_tmp36C[1]=_tmp70D;});Cyc_PP_cat(_tag_dyneither(_tmp36C,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp369[4]=_tmp70F;}),({
struct Cyc_PP_Doc*_tmp70C=Cyc_PP_line_doc();_tmp369[5]=_tmp70C;}),({
struct Cyc_PP_Doc*_tmp70B=Cyc_Absynpp_rb();_tmp369[6]=_tmp70B;}),
print_else?({
struct Cyc_PP_Doc*_tmp70A=({struct Cyc_PP_Doc*_tmp36D[6U];({struct Cyc_PP_Doc*_tmp709=Cyc_PP_line_doc();_tmp36D[0]=_tmp709;}),({
struct Cyc_PP_Doc*_tmp708=Cyc_PP_text(({const char*_tmp36E="else ";_tag_dyneither(_tmp36E,sizeof(char),6U);}));_tmp36D[1]=_tmp708;}),({
struct Cyc_PP_Doc*_tmp707=Cyc_Absynpp_lb();_tmp36D[2]=_tmp707;}),({
struct Cyc_PP_Doc*_tmp706=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp36F[2U];({struct Cyc_PP_Doc*_tmp705=Cyc_PP_line_doc();_tmp36F[0]=_tmp705;}),({struct Cyc_PP_Doc*_tmp704=Cyc_Absynpp_stmt2doc(_tmp399,0);_tmp36F[1]=_tmp704;});Cyc_PP_cat(_tag_dyneither(_tmp36F,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp36D[3]=_tmp706;}),({
struct Cyc_PP_Doc*_tmp703=Cyc_PP_line_doc();_tmp36D[4]=_tmp703;}),({
struct Cyc_PP_Doc*_tmp702=Cyc_Absynpp_rb();_tmp36D[5]=_tmp702;});Cyc_PP_cat(_tag_dyneither(_tmp36D,sizeof(struct Cyc_PP_Doc*),6U));});
# 1498
_tmp369[7]=_tmp70A;}):({
# 1504
struct Cyc_PP_Doc*_tmp701=Cyc_PP_nil_doc();_tmp369[7]=_tmp701;});Cyc_PP_cat(_tag_dyneither(_tmp369,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 5U: _LLB: _tmp39D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp353)->f1).f1;_tmp39C=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp353)->f2;_LLC:
# 1507
 s=({struct Cyc_PP_Doc*_tmp370[7U];({struct Cyc_PP_Doc*_tmp71C=Cyc_PP_text(({const char*_tmp371="while (";_tag_dyneither(_tmp371,sizeof(char),8U);}));_tmp370[0]=_tmp71C;}),({
struct Cyc_PP_Doc*_tmp71B=Cyc_Absynpp_exp2doc(_tmp39D);_tmp370[1]=_tmp71B;}),({
struct Cyc_PP_Doc*_tmp71A=Cyc_PP_text(({const char*_tmp372=") ";_tag_dyneither(_tmp372,sizeof(char),3U);}));_tmp370[2]=_tmp71A;}),({
struct Cyc_PP_Doc*_tmp719=Cyc_Absynpp_lb();_tmp370[3]=_tmp719;}),({
struct Cyc_PP_Doc*_tmp718=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp373[2U];({struct Cyc_PP_Doc*_tmp717=Cyc_PP_line_doc();_tmp373[0]=_tmp717;}),({struct Cyc_PP_Doc*_tmp716=Cyc_Absynpp_stmt2doc(_tmp39C,0);_tmp373[1]=_tmp716;});Cyc_PP_cat(_tag_dyneither(_tmp373,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp370[4]=_tmp718;}),({
struct Cyc_PP_Doc*_tmp715=Cyc_PP_line_doc();_tmp370[5]=_tmp715;}),({
struct Cyc_PP_Doc*_tmp714=Cyc_Absynpp_rb();_tmp370[6]=_tmp714;});Cyc_PP_cat(_tag_dyneither(_tmp370,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp374="break;";_tag_dyneither(_tmp374,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp375="continue;";_tag_dyneither(_tmp375,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp39E=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_LL12:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp378=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,_tmp545.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39E);_tmp545;});void*_tmp376[1U];_tmp376[0]=& _tmp378;({struct _dyneither_ptr _tmp71D=({const char*_tmp377="goto %s;";_tag_dyneither(_tmp377,sizeof(char),9U);});Cyc_aprintf(_tmp71D,_tag_dyneither(_tmp376,sizeof(void*),1U));});}));goto _LL0;case 9U: _LL13: _tmp3A2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_tmp3A1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp353)->f2).f1;_tmp3A0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp353)->f3).f1;_tmp39F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp353)->f4;_LL14:
# 1519
 s=({struct Cyc_PP_Doc*_tmp379[11U];({struct Cyc_PP_Doc*_tmp72A=Cyc_PP_text(({const char*_tmp37A="for(";_tag_dyneither(_tmp37A,sizeof(char),5U);}));_tmp379[0]=_tmp72A;}),({
struct Cyc_PP_Doc*_tmp729=Cyc_Absynpp_exp2doc(_tmp3A2);_tmp379[1]=_tmp729;}),({
struct Cyc_PP_Doc*_tmp728=Cyc_PP_text(({const char*_tmp37B="; ";_tag_dyneither(_tmp37B,sizeof(char),3U);}));_tmp379[2]=_tmp728;}),({
struct Cyc_PP_Doc*_tmp727=Cyc_Absynpp_exp2doc(_tmp3A1);_tmp379[3]=_tmp727;}),({
struct Cyc_PP_Doc*_tmp726=Cyc_PP_text(({const char*_tmp37C="; ";_tag_dyneither(_tmp37C,sizeof(char),3U);}));_tmp379[4]=_tmp726;}),({
struct Cyc_PP_Doc*_tmp725=Cyc_Absynpp_exp2doc(_tmp3A0);_tmp379[5]=_tmp725;}),({
struct Cyc_PP_Doc*_tmp724=Cyc_PP_text(({const char*_tmp37D=") ";_tag_dyneither(_tmp37D,sizeof(char),3U);}));_tmp379[6]=_tmp724;}),({
struct Cyc_PP_Doc*_tmp723=Cyc_Absynpp_lb();_tmp379[7]=_tmp723;}),({
struct Cyc_PP_Doc*_tmp722=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp37E[2U];({struct Cyc_PP_Doc*_tmp721=Cyc_PP_line_doc();_tmp37E[0]=_tmp721;}),({struct Cyc_PP_Doc*_tmp720=Cyc_Absynpp_stmt2doc(_tmp39F,0);_tmp37E[1]=_tmp720;});Cyc_PP_cat(_tag_dyneither(_tmp37E,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp379[8]=_tmp722;}),({
struct Cyc_PP_Doc*_tmp71F=Cyc_PP_line_doc();_tmp379[9]=_tmp71F;}),({
struct Cyc_PP_Doc*_tmp71E=Cyc_Absynpp_rb();_tmp379[10]=_tmp71E;});Cyc_PP_cat(_tag_dyneither(_tmp379,sizeof(struct Cyc_PP_Doc*),11U));});
goto _LL0;case 10U: _LL15: _tmp3A4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_tmp3A3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp353)->f2;_LL16:
# 1532
 s=({struct Cyc_PP_Doc*_tmp37F[8U];({struct Cyc_PP_Doc*_tmp732=Cyc_PP_text(({const char*_tmp380="switch (";_tag_dyneither(_tmp380,sizeof(char),9U);}));_tmp37F[0]=_tmp732;}),({
struct Cyc_PP_Doc*_tmp731=Cyc_Absynpp_exp2doc(_tmp3A4);_tmp37F[1]=_tmp731;}),({
struct Cyc_PP_Doc*_tmp730=Cyc_PP_text(({const char*_tmp381=") ";_tag_dyneither(_tmp381,sizeof(char),3U);}));_tmp37F[2]=_tmp730;}),({
struct Cyc_PP_Doc*_tmp72F=Cyc_Absynpp_lb();_tmp37F[3]=_tmp72F;}),({
struct Cyc_PP_Doc*_tmp72E=Cyc_PP_line_doc();_tmp37F[4]=_tmp72E;}),({
struct Cyc_PP_Doc*_tmp72D=Cyc_Absynpp_switchclauses2doc(_tmp3A3);_tmp37F[5]=_tmp72D;}),({
struct Cyc_PP_Doc*_tmp72C=Cyc_PP_line_doc();_tmp37F[6]=_tmp72C;}),({
struct Cyc_PP_Doc*_tmp72B=Cyc_Absynpp_rb();_tmp37F[7]=_tmp72B;});Cyc_PP_cat(_tag_dyneither(_tmp37F,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp353)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp382="fallthru;";_tag_dyneither(_tmp382,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp3A5=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_LL1A:
# 1543
 s=({struct Cyc_PP_Doc*_tmp383[3U];({struct Cyc_PP_Doc*_tmp735=Cyc_PP_text(({const char*_tmp384="fallthru(";_tag_dyneither(_tmp384,sizeof(char),10U);}));_tmp383[0]=_tmp735;}),({struct Cyc_PP_Doc*_tmp734=Cyc_Absynpp_exps2doc_prec(20,_tmp3A5);_tmp383[1]=_tmp734;}),({struct Cyc_PP_Doc*_tmp733=Cyc_PP_text(({const char*_tmp385=");";_tag_dyneither(_tmp385,sizeof(char),3U);}));_tmp383[2]=_tmp733;});Cyc_PP_cat(_tag_dyneither(_tmp383,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 12U: _LL1B: _tmp3A7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_tmp3A6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp353)->f2;_LL1C:
# 1545
 s=({struct Cyc_PP_Doc*_tmp386[3U];({struct Cyc_PP_Doc*_tmp738=Cyc_Absynpp_decl2doc(_tmp3A7);_tmp386[0]=_tmp738;}),({
struct Cyc_PP_Doc*_tmp737=Cyc_PP_line_doc();_tmp386[1]=_tmp737;}),({
struct Cyc_PP_Doc*_tmp736=Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp);_tmp386[2]=_tmp736;});Cyc_PP_cat(_tag_dyneither(_tmp386,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 13U: _LL1D: _tmp3A9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_tmp3A8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp353)->f2;_LL1E:
# 1550
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3A8)){
if(stmtexp)
s=({struct Cyc_PP_Doc*_tmp387[8U];({struct Cyc_PP_Doc*_tmp740=Cyc_PP_textptr(_tmp3A9);_tmp387[0]=_tmp740;}),({
struct Cyc_PP_Doc*_tmp73F=Cyc_PP_text(({const char*_tmp388=": (";_tag_dyneither(_tmp388,sizeof(char),4U);}));_tmp387[1]=_tmp73F;}),({
struct Cyc_PP_Doc*_tmp73E=Cyc_Absynpp_lb();_tmp387[2]=_tmp73E;}),({
struct Cyc_PP_Doc*_tmp73D=Cyc_PP_line_doc();_tmp387[3]=_tmp73D;}),({
struct Cyc_PP_Doc*_tmp73C=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A8,1));_tmp387[4]=_tmp73C;}),({
struct Cyc_PP_Doc*_tmp73B=Cyc_PP_line_doc();_tmp387[5]=_tmp73B;}),({
struct Cyc_PP_Doc*_tmp73A=Cyc_Absynpp_rb();_tmp387[6]=_tmp73A;}),({struct Cyc_PP_Doc*_tmp739=Cyc_PP_text(({const char*_tmp389=");";_tag_dyneither(_tmp389,sizeof(char),3U);}));_tmp387[7]=_tmp739;});Cyc_PP_cat(_tag_dyneither(_tmp387,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1560
s=({struct Cyc_PP_Doc*_tmp38A[7U];({struct Cyc_PP_Doc*_tmp747=Cyc_PP_textptr(_tmp3A9);_tmp38A[0]=_tmp747;}),({
struct Cyc_PP_Doc*_tmp746=Cyc_PP_text(({const char*_tmp38B=": ";_tag_dyneither(_tmp38B,sizeof(char),3U);}));_tmp38A[1]=_tmp746;}),({
struct Cyc_PP_Doc*_tmp745=Cyc_Absynpp_lb();_tmp38A[2]=_tmp745;}),({
struct Cyc_PP_Doc*_tmp744=Cyc_PP_line_doc();_tmp38A[3]=_tmp744;}),({
struct Cyc_PP_Doc*_tmp743=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A8,0));_tmp38A[4]=_tmp743;}),({
struct Cyc_PP_Doc*_tmp742=Cyc_PP_line_doc();_tmp38A[5]=_tmp742;}),({
struct Cyc_PP_Doc*_tmp741=Cyc_Absynpp_rb();_tmp38A[6]=_tmp741;});Cyc_PP_cat(_tag_dyneither(_tmp38A,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1568
s=({struct Cyc_PP_Doc*_tmp38C[3U];({struct Cyc_PP_Doc*_tmp74A=Cyc_PP_textptr(_tmp3A9);_tmp38C[0]=_tmp74A;}),({struct Cyc_PP_Doc*_tmp749=Cyc_PP_text(({const char*_tmp38D=": ";_tag_dyneither(_tmp38D,sizeof(char),3U);}));_tmp38C[1]=_tmp749;}),({struct Cyc_PP_Doc*_tmp748=Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp);_tmp38C[2]=_tmp748;});Cyc_PP_cat(_tag_dyneither(_tmp38C,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 14U: _LL1F: _tmp3AB=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_tmp3AA=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp353)->f2).f1;_LL20:
# 1571
 s=({struct Cyc_PP_Doc*_tmp38E[9U];({struct Cyc_PP_Doc*_tmp753=Cyc_PP_text(({const char*_tmp38F="do ";_tag_dyneither(_tmp38F,sizeof(char),4U);}));_tmp38E[0]=_tmp753;}),({
struct Cyc_PP_Doc*_tmp752=Cyc_Absynpp_lb();_tmp38E[1]=_tmp752;}),({
struct Cyc_PP_Doc*_tmp751=Cyc_PP_line_doc();_tmp38E[2]=_tmp751;}),({
struct Cyc_PP_Doc*_tmp750=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3AB,0));_tmp38E[3]=_tmp750;}),({
struct Cyc_PP_Doc*_tmp74F=Cyc_PP_line_doc();_tmp38E[4]=_tmp74F;}),({
struct Cyc_PP_Doc*_tmp74E=Cyc_Absynpp_rb();_tmp38E[5]=_tmp74E;}),({
struct Cyc_PP_Doc*_tmp74D=Cyc_PP_text(({const char*_tmp390=" while (";_tag_dyneither(_tmp390,sizeof(char),9U);}));_tmp38E[6]=_tmp74D;}),({
struct Cyc_PP_Doc*_tmp74C=Cyc_Absynpp_exp2doc(_tmp3AA);_tmp38E[7]=_tmp74C;}),({
struct Cyc_PP_Doc*_tmp74B=Cyc_PP_text(({const char*_tmp391=");";_tag_dyneither(_tmp391,sizeof(char),3U);}));_tmp38E[8]=_tmp74B;});Cyc_PP_cat(_tag_dyneither(_tmp38E,sizeof(struct Cyc_PP_Doc*),9U));});
goto _LL0;default: _LL21: _tmp3AD=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp353)->f1;_tmp3AC=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp353)->f2;_LL22:
# 1582
 s=({struct Cyc_PP_Doc*_tmp392[12U];({struct Cyc_PP_Doc*_tmp75F=Cyc_PP_text(({const char*_tmp393="try ";_tag_dyneither(_tmp393,sizeof(char),5U);}));_tmp392[0]=_tmp75F;}),({
struct Cyc_PP_Doc*_tmp75E=Cyc_Absynpp_lb();_tmp392[1]=_tmp75E;}),({
struct Cyc_PP_Doc*_tmp75D=Cyc_PP_line_doc();_tmp392[2]=_tmp75D;}),({
struct Cyc_PP_Doc*_tmp75C=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3AD,0));_tmp392[3]=_tmp75C;}),({
struct Cyc_PP_Doc*_tmp75B=Cyc_PP_line_doc();_tmp392[4]=_tmp75B;}),({
struct Cyc_PP_Doc*_tmp75A=Cyc_Absynpp_rb();_tmp392[5]=_tmp75A;}),({
struct Cyc_PP_Doc*_tmp759=Cyc_PP_text(({const char*_tmp394=" catch ";_tag_dyneither(_tmp394,sizeof(char),8U);}));_tmp392[6]=_tmp759;}),({
struct Cyc_PP_Doc*_tmp758=Cyc_Absynpp_lb();_tmp392[7]=_tmp758;}),({
struct Cyc_PP_Doc*_tmp757=Cyc_PP_line_doc();_tmp392[8]=_tmp757;}),({
struct Cyc_PP_Doc*_tmp756=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3AC));_tmp392[9]=_tmp756;}),({
struct Cyc_PP_Doc*_tmp755=Cyc_PP_line_doc();_tmp392[10]=_tmp755;}),({
struct Cyc_PP_Doc*_tmp754=Cyc_Absynpp_rb();_tmp392[11]=_tmp754;});Cyc_PP_cat(_tag_dyneither(_tmp392,sizeof(struct Cyc_PP_Doc*),12U));});
goto _LL0;}_LL0:;}
# 1596
return s;}
# 1599
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3AE=p->r;void*_tmp3AF=_tmp3AE;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Datatypefield*_tmp40E;struct Cyc_List_List*_tmp40D;int _tmp40C;struct Cyc_Absyn_Datatypefield*_tmp40B;struct Cyc_Absyn_Enumfield*_tmp40A;struct Cyc_Absyn_Enumfield*_tmp409;struct Cyc_List_List*_tmp408;struct Cyc_List_List*_tmp407;int _tmp406;union Cyc_Absyn_AggrInfo _tmp405;struct Cyc_List_List*_tmp404;struct Cyc_List_List*_tmp403;int _tmp402;struct _tuple0*_tmp401;struct Cyc_List_List*_tmp400;int _tmp3FF;struct _tuple0*_tmp3FE;struct Cyc_Absyn_Vardecl*_tmp3FD;struct Cyc_Absyn_Pat*_tmp3FC;struct Cyc_Absyn_Vardecl*_tmp3FB;struct Cyc_Absyn_Pat*_tmp3FA;struct Cyc_List_List*_tmp3F9;int _tmp3F8;struct Cyc_Absyn_Tvar*_tmp3F7;struct Cyc_Absyn_Vardecl*_tmp3F6;struct Cyc_Absyn_Tvar*_tmp3F5;struct Cyc_Absyn_Vardecl*_tmp3F4;struct Cyc_Absyn_Vardecl*_tmp3F3;struct Cyc_Absyn_Pat*_tmp3F2;struct Cyc_Absyn_Vardecl*_tmp3F1;struct _dyneither_ptr _tmp3F0;char _tmp3EF;enum Cyc_Absyn_Sign _tmp3EE;int _tmp3ED;switch(*((int*)_tmp3AF)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3B0="_";_tag_dyneither(_tmp3B0,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp3B1="NULL";_tag_dyneither(_tmp3B1,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp3EE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_tmp3ED=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_LL6:
# 1605
 if((int)_tmp3EE != (int)Cyc_Absyn_Unsigned)
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B4=({struct Cyc_Int_pa_PrintArg_struct _tmp546;_tmp546.tag=1U,_tmp546.f1=(unsigned long)_tmp3ED;_tmp546;});void*_tmp3B2[1U];_tmp3B2[0]=& _tmp3B4;({struct _dyneither_ptr _tmp760=({const char*_tmp3B3="%d";_tag_dyneither(_tmp3B3,sizeof(char),3U);});Cyc_aprintf(_tmp760,_tag_dyneither(_tmp3B2,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B7=({struct Cyc_Int_pa_PrintArg_struct _tmp547;_tmp547.tag=1U,_tmp547.f1=(unsigned int)_tmp3ED;_tmp547;});void*_tmp3B5[1U];_tmp3B5[0]=& _tmp3B7;({struct _dyneither_ptr _tmp761=({const char*_tmp3B6="%u";_tag_dyneither(_tmp3B6,sizeof(char),3U);});Cyc_aprintf(_tmp761,_tag_dyneither(_tmp3B5,sizeof(void*),1U));});}));}
goto _LL0;case 11U: _LL7: _tmp3EF=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_LL8:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3BA=({struct Cyc_String_pa_PrintArg_struct _tmp548;_tmp548.tag=0U,({struct _dyneither_ptr _tmp762=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp3EF));_tmp548.f1=_tmp762;});_tmp548;});void*_tmp3B8[1U];_tmp3B8[0]=& _tmp3BA;({struct _dyneither_ptr _tmp763=({const char*_tmp3B9="'%s'";_tag_dyneither(_tmp3B9,sizeof(char),5U);});Cyc_aprintf(_tmp763,_tag_dyneither(_tmp3B8,sizeof(void*),1U));});}));goto _LL0;case 12U: _LL9: _tmp3F0=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_LLA:
 s=Cyc_PP_text(_tmp3F0);goto _LL0;case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2)->r)->tag == 0U){_LLB: _tmp3F1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_LLC:
# 1612
 s=Cyc_Absynpp_qvar2doc(_tmp3F1->name);goto _LL0;}else{_LLD: _tmp3F3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_tmp3F2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_LLE:
# 1614
 s=({struct Cyc_PP_Doc*_tmp3BB[3U];({struct Cyc_PP_Doc*_tmp766=Cyc_Absynpp_qvar2doc(_tmp3F3->name);_tmp3BB[0]=_tmp766;}),({struct Cyc_PP_Doc*_tmp765=Cyc_PP_text(({const char*_tmp3BC=" as ";_tag_dyneither(_tmp3BC,sizeof(char),5U);}));_tmp3BB[1]=_tmp765;}),({struct Cyc_PP_Doc*_tmp764=Cyc_Absynpp_pat2doc(_tmp3F2);_tmp3BB[2]=_tmp764;});Cyc_PP_cat(_tag_dyneither(_tmp3BB,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 2U: _LLF: _tmp3F5=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_tmp3F4=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_LL10:
# 1616
 s=({struct Cyc_PP_Doc*_tmp3BD[5U];({struct Cyc_PP_Doc*_tmp76B=Cyc_PP_text(({const char*_tmp3BE="alias";_tag_dyneither(_tmp3BE,sizeof(char),6U);}));_tmp3BD[0]=_tmp76B;}),({struct Cyc_PP_Doc*_tmp76A=Cyc_PP_text(({const char*_tmp3BF=" <";_tag_dyneither(_tmp3BF,sizeof(char),3U);}));_tmp3BD[1]=_tmp76A;}),({struct Cyc_PP_Doc*_tmp769=Cyc_Absynpp_tvar2doc(_tmp3F5);_tmp3BD[2]=_tmp769;}),({struct Cyc_PP_Doc*_tmp768=Cyc_PP_text(({const char*_tmp3C0="> ";_tag_dyneither(_tmp3C0,sizeof(char),3U);}));_tmp3BD[3]=_tmp768;}),({
struct Cyc_PP_Doc*_tmp767=Cyc_Absynpp_vardecl2doc(_tmp3F4);_tmp3BD[4]=_tmp767;});Cyc_PP_cat(_tag_dyneither(_tmp3BD,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 4U: _LL11: _tmp3F7=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_tmp3F6=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_LL12:
# 1620
 s=({struct Cyc_PP_Doc*_tmp3C1[4U];({struct Cyc_PP_Doc*_tmp76F=Cyc_Absynpp_qvar2doc(_tmp3F6->name);_tmp3C1[0]=_tmp76F;}),({struct Cyc_PP_Doc*_tmp76E=Cyc_PP_text(({const char*_tmp3C2="<";_tag_dyneither(_tmp3C2,sizeof(char),2U);}));_tmp3C1[1]=_tmp76E;}),({struct Cyc_PP_Doc*_tmp76D=Cyc_Absynpp_tvar2doc(_tmp3F7);_tmp3C1[2]=_tmp76D;}),({struct Cyc_PP_Doc*_tmp76C=Cyc_PP_text(({const char*_tmp3C3=">";_tag_dyneither(_tmp3C3,sizeof(char),2U);}));_tmp3C1[3]=_tmp76C;});Cyc_PP_cat(_tag_dyneither(_tmp3C1,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 5U: _LL13: _tmp3F9=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_tmp3F8=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_LL14:
# 1623
 s=({struct Cyc_PP_Doc*_tmp3C4[4U];({struct Cyc_PP_Doc*_tmp775=Cyc_Absynpp_dollar();_tmp3C4[0]=_tmp775;}),({struct Cyc_PP_Doc*_tmp774=Cyc_PP_text(({const char*_tmp3C5="(";_tag_dyneither(_tmp3C5,sizeof(char),2U);}));_tmp3C4[1]=_tmp774;}),({struct Cyc_PP_Doc*_tmp773=({struct _dyneither_ptr _tmp772=({const char*_tmp3C6=",";_tag_dyneither(_tmp3C6,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp772,_tmp3F9);});_tmp3C4[2]=_tmp773;}),
_tmp3F8?({struct Cyc_PP_Doc*_tmp771=Cyc_PP_text(({const char*_tmp3C7=", ...)";_tag_dyneither(_tmp3C7,sizeof(char),7U);}));_tmp3C4[3]=_tmp771;}):({struct Cyc_PP_Doc*_tmp770=Cyc_PP_text(({const char*_tmp3C8=")";_tag_dyneither(_tmp3C8,sizeof(char),2U);}));_tmp3C4[3]=_tmp770;});Cyc_PP_cat(_tag_dyneither(_tmp3C4,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 6U: _LL15: _tmp3FA=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_LL16:
# 1627
 s=({struct Cyc_PP_Doc*_tmp3C9[2U];({struct Cyc_PP_Doc*_tmp777=Cyc_PP_text(({const char*_tmp3CA="&";_tag_dyneither(_tmp3CA,sizeof(char),2U);}));_tmp3C9[0]=_tmp777;}),({struct Cyc_PP_Doc*_tmp776=Cyc_Absynpp_pat2doc(_tmp3FA);_tmp3C9[1]=_tmp776;});Cyc_PP_cat(_tag_dyneither(_tmp3C9,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2)->r)->tag == 0U){_LL17: _tmp3FB=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_LL18:
# 1630
 s=({struct Cyc_PP_Doc*_tmp3CB[2U];({struct Cyc_PP_Doc*_tmp779=Cyc_PP_text(({const char*_tmp3CC="*";_tag_dyneither(_tmp3CC,sizeof(char),2U);}));_tmp3CB[0]=_tmp779;}),({struct Cyc_PP_Doc*_tmp778=Cyc_Absynpp_qvar2doc(_tmp3FB->name);_tmp3CB[1]=_tmp778;});Cyc_PP_cat(_tag_dyneither(_tmp3CB,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}else{_LL19: _tmp3FD=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_tmp3FC=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_LL1A:
# 1633
 s=({struct Cyc_PP_Doc*_tmp3CD[4U];({struct Cyc_PP_Doc*_tmp77D=Cyc_PP_text(({const char*_tmp3CE="*";_tag_dyneither(_tmp3CE,sizeof(char),2U);}));_tmp3CD[0]=_tmp77D;}),({struct Cyc_PP_Doc*_tmp77C=Cyc_Absynpp_qvar2doc(_tmp3FD->name);_tmp3CD[1]=_tmp77C;}),({struct Cyc_PP_Doc*_tmp77B=Cyc_PP_text(({const char*_tmp3CF=" as ";_tag_dyneither(_tmp3CF,sizeof(char),5U);}));_tmp3CD[2]=_tmp77B;}),({struct Cyc_PP_Doc*_tmp77A=Cyc_Absynpp_pat2doc(_tmp3FC);_tmp3CD[3]=_tmp77A;});Cyc_PP_cat(_tag_dyneither(_tmp3CD,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 15U: _LL1B: _tmp3FE=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_LL1C:
# 1636
 s=Cyc_Absynpp_qvar2doc(_tmp3FE);
goto _LL0;case 16U: _LL1D: _tmp401=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_tmp400=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_tmp3FF=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AF)->f3;_LL1E: {
# 1639
struct _dyneither_ptr term=_tmp3FF?({const char*_tmp3D3=", ...)";_tag_dyneither(_tmp3D3,sizeof(char),7U);}):({const char*_tmp3D4=")";_tag_dyneither(_tmp3D4,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3D0[2U];({struct Cyc_PP_Doc*_tmp782=Cyc_Absynpp_qvar2doc(_tmp401);_tmp3D0[0]=_tmp782;}),({struct Cyc_PP_Doc*_tmp781=({struct _dyneither_ptr _tmp780=({const char*_tmp3D1="(";_tag_dyneither(_tmp3D1,sizeof(char),2U);});struct _dyneither_ptr _tmp77F=term;struct _dyneither_ptr _tmp77E=({const char*_tmp3D2=",";_tag_dyneither(_tmp3D2,sizeof(char),2U);});Cyc_PP_group(_tmp780,_tmp77F,_tmp77E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp400));});_tmp3D0[1]=_tmp781;});Cyc_PP_cat(_tag_dyneither(_tmp3D0,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1 != 0){_LL1F: _tmp405=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_tmp404=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_tmp403=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AF)->f3;_tmp402=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AF)->f4;_LL20: {
# 1643
struct _dyneither_ptr term=_tmp402?({const char*_tmp3DE=", ...}";_tag_dyneither(_tmp3DE,sizeof(char),7U);}):({const char*_tmp3DF="}";_tag_dyneither(_tmp3DF,sizeof(char),2U);});
struct _tuple10 _tmp3D5=Cyc_Absyn_aggr_kinded_name(_tmp405);struct _tuple10 _tmp3D6=_tmp3D5;struct _tuple0*_tmp3DD;_LL2E: _tmp3DD=_tmp3D6.f2;_LL2F:;
s=({struct Cyc_PP_Doc*_tmp3D7[4U];({struct Cyc_PP_Doc*_tmp78C=Cyc_Absynpp_qvar2doc(_tmp3DD);_tmp3D7[0]=_tmp78C;}),({struct Cyc_PP_Doc*_tmp78B=Cyc_Absynpp_lb();_tmp3D7[1]=_tmp78B;}),({
struct Cyc_PP_Doc*_tmp78A=({struct _dyneither_ptr _tmp789=({const char*_tmp3D8="[";_tag_dyneither(_tmp3D8,sizeof(char),2U);});struct _dyneither_ptr _tmp788=({const char*_tmp3D9="]";_tag_dyneither(_tmp3D9,sizeof(char),2U);});struct _dyneither_ptr _tmp787=({const char*_tmp3DA=",";_tag_dyneither(_tmp3DA,sizeof(char),2U);});Cyc_PP_egroup(_tmp789,_tmp788,_tmp787,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp404));});_tmp3D7[2]=_tmp78A;}),({
struct Cyc_PP_Doc*_tmp786=({struct _dyneither_ptr _tmp785=({const char*_tmp3DB="";_tag_dyneither(_tmp3DB,sizeof(char),1U);});struct _dyneither_ptr _tmp784=term;struct _dyneither_ptr _tmp783=({const char*_tmp3DC=",";_tag_dyneither(_tmp3DC,sizeof(char),2U);});Cyc_PP_group(_tmp785,_tmp784,_tmp783,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp403));});_tmp3D7[3]=_tmp786;});Cyc_PP_cat(_tag_dyneither(_tmp3D7,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}else{_LL21: _tmp408=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_tmp407=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AF)->f3;_tmp406=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AF)->f4;_LL22: {
# 1650
struct _dyneither_ptr term=_tmp406?({const char*_tmp3E6=", ...}";_tag_dyneither(_tmp3E6,sizeof(char),7U);}):({const char*_tmp3E7="}";_tag_dyneither(_tmp3E7,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3E0[3U];({struct Cyc_PP_Doc*_tmp795=Cyc_Absynpp_lb();_tmp3E0[0]=_tmp795;}),({
struct Cyc_PP_Doc*_tmp794=({struct _dyneither_ptr _tmp793=({const char*_tmp3E1="[";_tag_dyneither(_tmp3E1,sizeof(char),2U);});struct _dyneither_ptr _tmp792=({const char*_tmp3E2="]";_tag_dyneither(_tmp3E2,sizeof(char),2U);});struct _dyneither_ptr _tmp791=({const char*_tmp3E3=",";_tag_dyneither(_tmp3E3,sizeof(char),2U);});Cyc_PP_egroup(_tmp793,_tmp792,_tmp791,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp408));});_tmp3E0[1]=_tmp794;}),({
struct Cyc_PP_Doc*_tmp790=({struct _dyneither_ptr _tmp78F=({const char*_tmp3E4="";_tag_dyneither(_tmp3E4,sizeof(char),1U);});struct _dyneither_ptr _tmp78E=term;struct _dyneither_ptr _tmp78D=({const char*_tmp3E5=",";_tag_dyneither(_tmp3E5,sizeof(char),2U);});Cyc_PP_group(_tmp78F,_tmp78E,_tmp78D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp407));});_tmp3E0[2]=_tmp790;});Cyc_PP_cat(_tag_dyneither(_tmp3E0,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp409=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_LL24:
 _tmp40A=_tmp409;goto _LL26;case 14U: _LL25: _tmp40A=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_LL26:
 s=Cyc_Absynpp_qvar2doc(_tmp40A->name);goto _LL0;case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AF)->f3 == 0){_LL27: _tmp40B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_LL28:
 s=Cyc_Absynpp_qvar2doc(_tmp40B->name);goto _LL0;}else{_LL29: _tmp40E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AF)->f2;_tmp40D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AF)->f3;_tmp40C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AF)->f4;_LL2A: {
# 1659
struct _dyneither_ptr term=_tmp40C?({const char*_tmp3EB=", ...)";_tag_dyneither(_tmp3EB,sizeof(char),7U);}):({const char*_tmp3EC=")";_tag_dyneither(_tmp3EC,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3E8[2U];({struct Cyc_PP_Doc*_tmp79A=Cyc_Absynpp_qvar2doc(_tmp40E->name);_tmp3E8[0]=_tmp79A;}),({struct Cyc_PP_Doc*_tmp799=({struct _dyneither_ptr _tmp798=({const char*_tmp3E9="(";_tag_dyneither(_tmp3E9,sizeof(char),2U);});struct _dyneither_ptr _tmp797=term;struct _dyneither_ptr _tmp796=({const char*_tmp3EA=",";_tag_dyneither(_tmp3EA,sizeof(char),2U);});Cyc_PP_egroup(_tmp798,_tmp797,_tmp796,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp40D));});_tmp3E8[1]=_tmp799;});Cyc_PP_cat(_tag_dyneither(_tmp3E8,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp40F=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3AF)->f1;_LL2C:
# 1663
 s=Cyc_Absynpp_exp2doc(_tmp40F);goto _LL0;}_LL0:;}
# 1665
return s;}
# 1668
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
return({struct Cyc_PP_Doc*_tmp410[2U];({struct Cyc_PP_Doc*_tmp79F=({struct _dyneither_ptr _tmp79E=({const char*_tmp411="";_tag_dyneither(_tmp411,sizeof(char),1U);});struct _dyneither_ptr _tmp79D=({const char*_tmp412="=";_tag_dyneither(_tmp412,sizeof(char),2U);});struct _dyneither_ptr _tmp79C=({const char*_tmp413="=";_tag_dyneither(_tmp413,sizeof(char),2U);});Cyc_PP_egroup(_tmp79E,_tmp79D,_tmp79C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp410[0]=_tmp79F;}),({
struct Cyc_PP_Doc*_tmp79B=Cyc_Absynpp_pat2doc((*dp).f2);_tmp410[1]=_tmp79B;});Cyc_PP_cat(_tag_dyneither(_tmp410,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1673
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp414[2U];({struct Cyc_PP_Doc*_tmp7A3=Cyc_PP_text(({const char*_tmp415="default: ";_tag_dyneither(_tmp415,sizeof(char),10U);}));_tmp414[0]=_tmp7A3;}),({
struct Cyc_PP_Doc*_tmp7A2=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp416[2U];({struct Cyc_PP_Doc*_tmp7A1=Cyc_PP_line_doc();_tmp416[0]=_tmp7A1;}),({struct Cyc_PP_Doc*_tmp7A0=Cyc_Absynpp_stmt2doc(c->body,0);_tmp416[1]=_tmp7A0;});Cyc_PP_cat(_tag_dyneither(_tmp416,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp414[1]=_tmp7A2;});Cyc_PP_cat(_tag_dyneither(_tmp414,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp417[4U];({struct Cyc_PP_Doc*_tmp7A9=Cyc_PP_text(({const char*_tmp418="case ";_tag_dyneither(_tmp418,sizeof(char),6U);}));_tmp417[0]=_tmp7A9;}),({
struct Cyc_PP_Doc*_tmp7A8=Cyc_Absynpp_pat2doc(c->pattern);_tmp417[1]=_tmp7A8;}),({
struct Cyc_PP_Doc*_tmp7A7=Cyc_PP_text(({const char*_tmp419=": ";_tag_dyneither(_tmp419,sizeof(char),3U);}));_tmp417[2]=_tmp7A7;}),({
struct Cyc_PP_Doc*_tmp7A6=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41A[2U];({struct Cyc_PP_Doc*_tmp7A5=Cyc_PP_line_doc();_tmp41A[0]=_tmp7A5;}),({struct Cyc_PP_Doc*_tmp7A4=Cyc_Absynpp_stmt2doc(c->body,0);_tmp41A[1]=_tmp7A4;});Cyc_PP_cat(_tag_dyneither(_tmp41A,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp417[3]=_tmp7A6;});Cyc_PP_cat(_tag_dyneither(_tmp417,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1683
return({struct Cyc_PP_Doc*_tmp41B[6U];({struct Cyc_PP_Doc*_tmp7B1=Cyc_PP_text(({const char*_tmp41C="case ";_tag_dyneither(_tmp41C,sizeof(char),6U);}));_tmp41B[0]=_tmp7B1;}),({
struct Cyc_PP_Doc*_tmp7B0=Cyc_Absynpp_pat2doc(c->pattern);_tmp41B[1]=_tmp7B0;}),({
struct Cyc_PP_Doc*_tmp7AF=Cyc_PP_text(({const char*_tmp41D=" && ";_tag_dyneither(_tmp41D,sizeof(char),5U);}));_tmp41B[2]=_tmp7AF;}),({
struct Cyc_PP_Doc*_tmp7AE=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp41B[3]=_tmp7AE;}),({
struct Cyc_PP_Doc*_tmp7AD=Cyc_PP_text(({const char*_tmp41E=": ";_tag_dyneither(_tmp41E,sizeof(char),3U);}));_tmp41B[4]=_tmp7AD;}),({
struct Cyc_PP_Doc*_tmp7AC=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41F[2U];({struct Cyc_PP_Doc*_tmp7AB=Cyc_PP_line_doc();_tmp41F[0]=_tmp7AB;}),({struct Cyc_PP_Doc*_tmp7AA=Cyc_Absynpp_stmt2doc(c->body,0);_tmp41F[1]=_tmp7AA;});Cyc_PP_cat(_tag_dyneither(_tmp41F,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp41B[5]=_tmp7AC;});Cyc_PP_cat(_tag_dyneither(_tmp41B,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1691
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _dyneither_ptr _tmp7B2=({const char*_tmp420="";_tag_dyneither(_tmp420,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp7B2,cs);});}
# 1695
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1699
return({struct Cyc_PP_Doc*_tmp421[3U];({struct Cyc_PP_Doc*_tmp7B5=Cyc_Absynpp_qvar2doc(f->name);_tmp421[0]=_tmp7B5;}),({struct Cyc_PP_Doc*_tmp7B4=Cyc_PP_text(({const char*_tmp422=" = ";_tag_dyneither(_tmp422,sizeof(char),4U);}));_tmp421[1]=_tmp7B4;}),({struct Cyc_PP_Doc*_tmp7B3=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp421[2]=_tmp7B3;});Cyc_PP_cat(_tag_dyneither(_tmp421,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1702
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _dyneither_ptr _tmp7B6=({const char*_tmp423=",";_tag_dyneither(_tmp423,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp7B6,fs);});}
# 1706
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1710
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _dyneither_ptr _tmp7B7=({const char*_tmp424=",";_tag_dyneither(_tmp424,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp7B7,vds);});}
# 1714
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp425=vd;enum Cyc_Absyn_Scope _tmp437;struct _tuple0*_tmp436;unsigned int _tmp435;struct Cyc_Absyn_Tqual _tmp434;void*_tmp433;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_List_List*_tmp431;_LL1: _tmp437=_tmp425->sc;_tmp436=_tmp425->name;_tmp435=_tmp425->varloc;_tmp434=_tmp425->tq;_tmp433=_tmp425->type;_tmp432=_tmp425->initializer;_tmp431=_tmp425->attributes;_LL2:;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp436);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp431);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp426=Cyc_Cyclone_c_compiler;if(_tmp426 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1723
{void*_tmp427=Cyc_Tcutil_compress(_tmp433);void*_tmp428=_tmp427;struct Cyc_List_List*_tmp429;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp428)->tag == 5U){_LL9: _tmp429=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp428)->f1).attributes;_LLA:
# 1725
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp429);
goto _LL8;}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1729
goto _LL3;}_LL3:;}{
# 1732
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp42A=Cyc_Cyclone_c_compiler;if(_tmp42A == Cyc_Cyclone_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1737
s=({struct Cyc_PP_Doc*_tmp42B[5U];_tmp42B[0]=tmp_doc,({
# 1739
struct Cyc_PP_Doc*_tmp7C1=Cyc_Absynpp_scope2doc(_tmp437);_tmp42B[1]=_tmp7C1;}),({
struct Cyc_PP_Doc*_tmp7C0=({struct Cyc_Absyn_Tqual _tmp7BF=_tmp434;void*_tmp7BE=_tmp433;Cyc_Absynpp_tqtd2doc(_tmp7BF,_tmp7BE,({struct Cyc_Core_Opt*_tmp42D=_cycalloc(sizeof(*_tmp42D));({struct Cyc_PP_Doc*_tmp7BD=({struct Cyc_PP_Doc*_tmp42C[2U];_tmp42C[0]=beforenamedoc,_tmp42C[1]=sn;Cyc_PP_cat(_tag_dyneither(_tmp42C,sizeof(struct Cyc_PP_Doc*),2U));});_tmp42D->v=_tmp7BD;});_tmp42D;}));});_tmp42B[2]=_tmp7C0;}),
_tmp432 == 0?({
struct Cyc_PP_Doc*_tmp7BC=Cyc_PP_nil_doc();_tmp42B[3]=_tmp7BC;}):({
struct Cyc_PP_Doc*_tmp7BB=({struct Cyc_PP_Doc*_tmp42E[2U];({struct Cyc_PP_Doc*_tmp7BA=Cyc_PP_text(({const char*_tmp42F=" = ";_tag_dyneither(_tmp42F,sizeof(char),4U);}));_tmp42E[0]=_tmp7BA;}),({struct Cyc_PP_Doc*_tmp7B9=Cyc_Absynpp_exp2doc(_tmp432);_tmp42E[1]=_tmp7B9;});Cyc_PP_cat(_tag_dyneither(_tmp42E,sizeof(struct Cyc_PP_Doc*),2U));});_tmp42B[3]=_tmp7BB;}),({
struct Cyc_PP_Doc*_tmp7B8=Cyc_PP_text(({const char*_tmp430=";";_tag_dyneither(_tmp430,sizeof(char),2U);}));_tmp42B[4]=_tmp7B8;});Cyc_PP_cat(_tag_dyneither(_tmp42B,sizeof(struct Cyc_PP_Doc*),5U));});
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1748
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp438=*x;struct _tuple17 _tmp439=_tmp438;struct _tuple0*_tmp43A;_LL1: _tmp43A=_tmp439.f2;_LL2:;
return Cyc_Absynpp_qvar2doc(_tmp43A);}
# 1753
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp43B[4U];({struct Cyc_PP_Doc*_tmp7C5=Cyc_Absynpp_scope2doc(ad->sc);_tmp43B[0]=_tmp7C5;}),({
struct Cyc_PP_Doc*_tmp7C4=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp43B[1]=_tmp7C4;}),({
struct Cyc_PP_Doc*_tmp7C3=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp43B[2]=_tmp7C3;}),({
struct Cyc_PP_Doc*_tmp7C2=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp43B[3]=_tmp7C2;});Cyc_PP_cat(_tag_dyneither(_tmp43B,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1760
return({struct Cyc_PP_Doc*_tmp43C[12U];({struct Cyc_PP_Doc*_tmp7D6=Cyc_Absynpp_scope2doc(ad->sc);_tmp43C[0]=_tmp7D6;}),({
struct Cyc_PP_Doc*_tmp7D5=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp43C[1]=_tmp7D5;}),({
struct Cyc_PP_Doc*_tmp7D4=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp43C[2]=_tmp7D4;}),({
struct Cyc_PP_Doc*_tmp7D3=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp43C[3]=_tmp7D3;}),({
struct Cyc_PP_Doc*_tmp7D2=Cyc_PP_blank_doc();_tmp43C[4]=_tmp7D2;}),({struct Cyc_PP_Doc*_tmp7D1=Cyc_Absynpp_lb();_tmp43C[5]=_tmp7D1;}),({
struct Cyc_PP_Doc*_tmp7D0=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp43C[6]=_tmp7D0;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp7CF=Cyc_PP_nil_doc();_tmp43C[7]=_tmp7CF;}):({
struct Cyc_PP_Doc*_tmp7CE=({struct Cyc_PP_Doc*_tmp43D[2U];({struct Cyc_PP_Doc*_tmp7CD=Cyc_PP_text(({const char*_tmp43E=":";_tag_dyneither(_tmp43E,sizeof(char),2U);}));_tmp43D[0]=_tmp7CD;}),({struct Cyc_PP_Doc*_tmp7CC=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp43D[1]=_tmp7CC;});Cyc_PP_cat(_tag_dyneither(_tmp43D,sizeof(struct Cyc_PP_Doc*),2U));});_tmp43C[7]=_tmp7CE;}),({
struct Cyc_PP_Doc*_tmp7CB=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43F[2U];({struct Cyc_PP_Doc*_tmp7CA=Cyc_PP_line_doc();_tmp43F[0]=_tmp7CA;}),({struct Cyc_PP_Doc*_tmp7C9=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp43F[1]=_tmp7C9;});Cyc_PP_cat(_tag_dyneither(_tmp43F,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp43C[8]=_tmp7CB;}),({
struct Cyc_PP_Doc*_tmp7C8=Cyc_PP_line_doc();_tmp43C[9]=_tmp7C8;}),({
struct Cyc_PP_Doc*_tmp7C7=Cyc_Absynpp_rb();_tmp43C[10]=_tmp7C7;}),({
struct Cyc_PP_Doc*_tmp7C6=Cyc_Absynpp_atts2doc(ad->attributes);_tmp43C[11]=_tmp7C6;});Cyc_PP_cat(_tag_dyneither(_tmp43C,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1774
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp440=dd;enum Cyc_Absyn_Scope _tmp44C;struct _tuple0*_tmp44B;struct Cyc_List_List*_tmp44A;struct Cyc_Core_Opt*_tmp449;int _tmp448;_LL1: _tmp44C=_tmp440->sc;_tmp44B=_tmp440->name;_tmp44A=_tmp440->tvs;_tmp449=_tmp440->fields;_tmp448=_tmp440->is_extensible;_LL2:;
if(_tmp449 == 0)
return({struct Cyc_PP_Doc*_tmp441[5U];({struct Cyc_PP_Doc*_tmp7DD=Cyc_Absynpp_scope2doc(_tmp44C);_tmp441[0]=_tmp7DD;}),
_tmp448?({struct Cyc_PP_Doc*_tmp7DC=Cyc_PP_text(({const char*_tmp442="@extensible ";_tag_dyneither(_tmp442,sizeof(char),13U);}));_tmp441[1]=_tmp7DC;}):({struct Cyc_PP_Doc*_tmp7DB=Cyc_PP_blank_doc();_tmp441[1]=_tmp7DB;}),({
struct Cyc_PP_Doc*_tmp7DA=Cyc_PP_text(({const char*_tmp443="datatype ";_tag_dyneither(_tmp443,sizeof(char),10U);}));_tmp441[2]=_tmp7DA;}),
_tmp448?({struct Cyc_PP_Doc*_tmp7D9=Cyc_Absynpp_qvar2bolddoc(_tmp44B);_tmp441[3]=_tmp7D9;}):({struct Cyc_PP_Doc*_tmp7D8=Cyc_Absynpp_typedef_name2bolddoc(_tmp44B);_tmp441[3]=_tmp7D8;}),({
struct Cyc_PP_Doc*_tmp7D7=Cyc_Absynpp_ktvars2doc(_tmp44A);_tmp441[4]=_tmp7D7;});Cyc_PP_cat(_tag_dyneither(_tmp441,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1783
return({struct Cyc_PP_Doc*_tmp444[10U];({struct Cyc_PP_Doc*_tmp7EB=Cyc_Absynpp_scope2doc(_tmp44C);_tmp444[0]=_tmp7EB;}),
_tmp448?({struct Cyc_PP_Doc*_tmp7EA=Cyc_PP_text(({const char*_tmp445="@extensible ";_tag_dyneither(_tmp445,sizeof(char),13U);}));_tmp444[1]=_tmp7EA;}):({struct Cyc_PP_Doc*_tmp7E9=Cyc_PP_blank_doc();_tmp444[1]=_tmp7E9;}),({
struct Cyc_PP_Doc*_tmp7E8=Cyc_PP_text(({const char*_tmp446="datatype ";_tag_dyneither(_tmp446,sizeof(char),10U);}));_tmp444[2]=_tmp7E8;}),
_tmp448?({struct Cyc_PP_Doc*_tmp7E7=Cyc_Absynpp_qvar2bolddoc(_tmp44B);_tmp444[3]=_tmp7E7;}):({struct Cyc_PP_Doc*_tmp7E6=Cyc_Absynpp_typedef_name2bolddoc(_tmp44B);_tmp444[3]=_tmp7E6;}),({
struct Cyc_PP_Doc*_tmp7E5=Cyc_Absynpp_ktvars2doc(_tmp44A);_tmp444[4]=_tmp7E5;}),({
struct Cyc_PP_Doc*_tmp7E4=Cyc_PP_blank_doc();_tmp444[5]=_tmp7E4;}),({struct Cyc_PP_Doc*_tmp7E3=Cyc_Absynpp_lb();_tmp444[6]=_tmp7E3;}),({
struct Cyc_PP_Doc*_tmp7E2=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp447[2U];({struct Cyc_PP_Doc*_tmp7E1=Cyc_PP_line_doc();_tmp447[0]=_tmp7E1;}),({struct Cyc_PP_Doc*_tmp7E0=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp449->v);_tmp447[1]=_tmp7E0;});Cyc_PP_cat(_tag_dyneither(_tmp447,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp444[7]=_tmp7E2;}),({
struct Cyc_PP_Doc*_tmp7DF=Cyc_PP_line_doc();_tmp444[8]=_tmp7DF;}),({
struct Cyc_PP_Doc*_tmp7DE=Cyc_Absynpp_rb();_tmp444[9]=_tmp7DE;});Cyc_PP_cat(_tag_dyneither(_tmp444,sizeof(struct Cyc_PP_Doc*),10U));});}}
# 1794
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp44D=ed;enum Cyc_Absyn_Scope _tmp455;struct _tuple0*_tmp454;struct Cyc_Core_Opt*_tmp453;_LL1: _tmp455=_tmp44D->sc;_tmp454=_tmp44D->name;_tmp453=_tmp44D->fields;_LL2:;
if(_tmp453 == 0)
return({struct Cyc_PP_Doc*_tmp44E[3U];({struct Cyc_PP_Doc*_tmp7EE=Cyc_Absynpp_scope2doc(_tmp455);_tmp44E[0]=_tmp7EE;}),({
struct Cyc_PP_Doc*_tmp7ED=Cyc_PP_text(({const char*_tmp44F="enum ";_tag_dyneither(_tmp44F,sizeof(char),6U);}));_tmp44E[1]=_tmp7ED;}),({
struct Cyc_PP_Doc*_tmp7EC=Cyc_Absynpp_typedef_name2bolddoc(_tmp454);_tmp44E[2]=_tmp7EC;});Cyc_PP_cat(_tag_dyneither(_tmp44E,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1802
return({struct Cyc_PP_Doc*_tmp450[8U];({struct Cyc_PP_Doc*_tmp7F8=Cyc_Absynpp_scope2doc(_tmp455);_tmp450[0]=_tmp7F8;}),({
struct Cyc_PP_Doc*_tmp7F7=Cyc_PP_text(({const char*_tmp451="enum ";_tag_dyneither(_tmp451,sizeof(char),6U);}));_tmp450[1]=_tmp7F7;}),({
struct Cyc_PP_Doc*_tmp7F6=Cyc_Absynpp_qvar2bolddoc(_tmp454);_tmp450[2]=_tmp7F6;}),({
struct Cyc_PP_Doc*_tmp7F5=Cyc_PP_blank_doc();_tmp450[3]=_tmp7F5;}),({struct Cyc_PP_Doc*_tmp7F4=Cyc_Absynpp_lb();_tmp450[4]=_tmp7F4;}),({
struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp452[2U];({struct Cyc_PP_Doc*_tmp7F2=Cyc_PP_line_doc();_tmp452[0]=_tmp7F2;}),({struct Cyc_PP_Doc*_tmp7F1=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp453->v);_tmp452[1]=_tmp7F1;});Cyc_PP_cat(_tag_dyneither(_tmp452,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp450[5]=_tmp7F3;}),({
struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_line_doc();_tmp450[6]=_tmp7F0;}),({
struct Cyc_PP_Doc*_tmp7EF=Cyc_Absynpp_rb();_tmp450[7]=_tmp7EF;});Cyc_PP_cat(_tag_dyneither(_tmp450,sizeof(struct Cyc_PP_Doc*),8U));});}}
# 1811
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp456=d->r;void*_tmp457=_tmp456;struct Cyc_List_List*_tmp4B9;struct Cyc_List_List*_tmp4B8;struct Cyc_List_List*_tmp4B7;struct _tuple0*_tmp4B6;struct Cyc_List_List*_tmp4B5;struct _dyneither_ptr*_tmp4B4;struct Cyc_List_List*_tmp4B3;struct Cyc_Absyn_Typedefdecl*_tmp4B2;struct Cyc_Absyn_Enumdecl*_tmp4B1;struct Cyc_List_List*_tmp4B0;struct Cyc_Absyn_Pat*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Datatypedecl*_tmp4AD;struct Cyc_Absyn_Tvar*_tmp4AC;struct Cyc_Absyn_Vardecl*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Vardecl*_tmp4A9;struct Cyc_Absyn_Aggrdecl*_tmp4A8;struct Cyc_Absyn_Fndecl*_tmp4A7;switch(*((int*)_tmp457)){case 1U: _LL1: _tmp4A7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_LL2: {
# 1815
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463->tag=5U,(_tmp463->f1).tvars=_tmp4A7->tvs,(_tmp463->f1).effect=_tmp4A7->effect,(_tmp463->f1).ret_tqual=_tmp4A7->ret_tqual,(_tmp463->f1).ret_type=_tmp4A7->ret_type,({
# 1819
struct Cyc_List_List*_tmp7F9=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4A7->args);(_tmp463->f1).args=_tmp7F9;}),(_tmp463->f1).c_varargs=_tmp4A7->c_varargs,(_tmp463->f1).cyc_varargs=_tmp4A7->cyc_varargs,(_tmp463->f1).rgn_po=_tmp4A7->rgn_po,(_tmp463->f1).attributes=0,(_tmp463->f1).requires_clause=_tmp4A7->requires_clause,(_tmp463->f1).requires_relns=_tmp4A7->requires_relns,(_tmp463->f1).ensures_clause=_tmp4A7->ensures_clause,(_tmp463->f1).ensures_relns=_tmp4A7->ensures_relns;_tmp463;});
# 1825
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4A7->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4A7->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp458=Cyc_Cyclone_c_compiler;if(_tmp458 == Cyc_Cyclone_Gcc_c){_LL24: _LL25:
 inlinedoc=Cyc_PP_text(({const char*_tmp459="inline ";_tag_dyneither(_tmp459,sizeof(char),8U);}));goto _LL23;}else{_LL26: _LL27:
 inlinedoc=Cyc_PP_text(({const char*_tmp45A="__inline ";_tag_dyneither(_tmp45A,sizeof(char),10U);}));goto _LL23;}_LL23:;}else{
# 1833
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4A7->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp45B=Cyc_Cyclone_c_compiler;if(_tmp45B == Cyc_Cyclone_Gcc_c){_LL29: _LL2A:
 beforenamedoc=attsdoc;goto _LL28;}else{_LL2B: _LL2C:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4A7->attributes);goto _LL28;}_LL28:;}{
# 1840
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4A7->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp7FC=Cyc_Absyn_empty_tqual(0U);void*_tmp7FB=t;Cyc_Absynpp_tqtd2doc(_tmp7FC,_tmp7FB,({struct Cyc_Core_Opt*_tmp462=_cycalloc(sizeof(*_tmp462));({
struct Cyc_PP_Doc*_tmp7FA=({struct Cyc_PP_Doc*_tmp461[2U];_tmp461[0]=beforenamedoc,_tmp461[1]=namedoc;Cyc_PP_cat(_tag_dyneither(_tmp461,sizeof(struct Cyc_PP_Doc*),2U));});_tmp462->v=_tmp7FA;});_tmp462;}));});
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp45F[5U];({struct Cyc_PP_Doc*_tmp803=Cyc_PP_blank_doc();_tmp45F[0]=_tmp803;}),({struct Cyc_PP_Doc*_tmp802=Cyc_Absynpp_lb();_tmp45F[1]=_tmp802;}),({
struct Cyc_PP_Doc*_tmp801=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp460[2U];({struct Cyc_PP_Doc*_tmp800=Cyc_PP_line_doc();_tmp460[0]=_tmp800;}),({struct Cyc_PP_Doc*_tmp7FF=Cyc_Absynpp_stmt2doc(_tmp4A7->body,0);_tmp460[1]=_tmp7FF;});Cyc_PP_cat(_tag_dyneither(_tmp460,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp45F[2]=_tmp801;}),({
struct Cyc_PP_Doc*_tmp7FE=Cyc_PP_line_doc();_tmp45F[3]=_tmp7FE;}),({
struct Cyc_PP_Doc*_tmp7FD=Cyc_Absynpp_rb();_tmp45F[4]=_tmp7FD;});Cyc_PP_cat(_tag_dyneither(_tmp45F,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp45C[4U];_tmp45C[0]=inlinedoc,_tmp45C[1]=scopedoc,_tmp45C[2]=tqtddoc,_tmp45C[3]=bodydoc;Cyc_PP_cat(_tag_dyneither(_tmp45C,sizeof(struct Cyc_PP_Doc*),4U));});
# 1849
{enum Cyc_Cyclone_C_Compilers _tmp45D=Cyc_Cyclone_c_compiler;if(_tmp45D == Cyc_Cyclone_Vc_c){_LL2E: _LL2F:
 s=({struct Cyc_PP_Doc*_tmp45E[2U];_tmp45E[0]=attsdoc,_tmp45E[1]=s;Cyc_PP_cat(_tag_dyneither(_tmp45E,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL2D;}else{_LL30: _LL31:
 goto _LL2D;}_LL2D:;}
# 1854
goto _LL0;};};}case 5U: _LL3: _tmp4A8=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_LL4:
# 1857
 s=({struct Cyc_PP_Doc*_tmp464[2U];({struct Cyc_PP_Doc*_tmp805=Cyc_Absynpp_aggrdecl2doc(_tmp4A8);_tmp464[0]=_tmp805;}),({struct Cyc_PP_Doc*_tmp804=Cyc_PP_text(({const char*_tmp465=";";_tag_dyneither(_tmp465,sizeof(char),2U);}));_tmp464[1]=_tmp804;});Cyc_PP_cat(_tag_dyneither(_tmp464,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 0U: _LL5: _tmp4A9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_LL6:
# 1860
 s=Cyc_Absynpp_vardecl2doc(_tmp4A9);
goto _LL0;case 4U: _LL7: _tmp4AC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_tmp4AB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp457)->f2;_tmp4AA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp457)->f3;_LL8:
# 1863
 s=({struct Cyc_PP_Doc*_tmp466[7U];({struct Cyc_PP_Doc*_tmp810=Cyc_PP_text(({const char*_tmp467="region";_tag_dyneither(_tmp467,sizeof(char),7U);}));_tmp466[0]=_tmp810;}),({
struct Cyc_PP_Doc*_tmp80F=Cyc_PP_text(({const char*_tmp468="<";_tag_dyneither(_tmp468,sizeof(char),2U);}));_tmp466[1]=_tmp80F;}),({
struct Cyc_PP_Doc*_tmp80E=Cyc_Absynpp_tvar2doc(_tmp4AC);_tmp466[2]=_tmp80E;}),({
struct Cyc_PP_Doc*_tmp80D=Cyc_PP_text(({const char*_tmp469=">";_tag_dyneither(_tmp469,sizeof(char),2U);}));_tmp466[3]=_tmp80D;}),({
struct Cyc_PP_Doc*_tmp80C=Cyc_Absynpp_qvar2doc(_tmp4AB->name);_tmp466[4]=_tmp80C;}),
(unsigned int)_tmp4AA?({struct Cyc_PP_Doc*_tmp80B=({struct Cyc_PP_Doc*_tmp46A[3U];({struct Cyc_PP_Doc*_tmp80A=Cyc_PP_text(({const char*_tmp46B=" = open(";_tag_dyneither(_tmp46B,sizeof(char),9U);}));_tmp46A[0]=_tmp80A;}),({struct Cyc_PP_Doc*_tmp809=Cyc_Absynpp_exp2doc(_tmp4AA);_tmp46A[1]=_tmp809;}),({
struct Cyc_PP_Doc*_tmp808=Cyc_PP_text(({const char*_tmp46C=")";_tag_dyneither(_tmp46C,sizeof(char),2U);}));_tmp46A[2]=_tmp808;});Cyc_PP_cat(_tag_dyneither(_tmp46A,sizeof(struct Cyc_PP_Doc*),3U));});
# 1868
_tmp466[5]=_tmp80B;}):({
struct Cyc_PP_Doc*_tmp807=Cyc_PP_nil_doc();_tmp466[5]=_tmp807;}),({
struct Cyc_PP_Doc*_tmp806=Cyc_PP_text(({const char*_tmp46D=";";_tag_dyneither(_tmp46D,sizeof(char),2U);}));_tmp466[6]=_tmp806;});Cyc_PP_cat(_tag_dyneither(_tmp466,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 6U: _LL9: _tmp4AD=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_LLA:
# 1873
 s=({struct Cyc_PP_Doc*_tmp46E[2U];({struct Cyc_PP_Doc*_tmp812=Cyc_Absynpp_datatypedecl2doc(_tmp4AD);_tmp46E[0]=_tmp812;}),({struct Cyc_PP_Doc*_tmp811=Cyc_PP_text(({const char*_tmp46F=";";_tag_dyneither(_tmp46F,sizeof(char),2U);}));_tmp46E[1]=_tmp811;});Cyc_PP_cat(_tag_dyneither(_tmp46E,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 2U: _LLB: _tmp4AF=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_tmp4AE=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp457)->f3;_LLC:
# 1876
 s=({struct Cyc_PP_Doc*_tmp470[5U];({struct Cyc_PP_Doc*_tmp817=Cyc_PP_text(({const char*_tmp471="let ";_tag_dyneither(_tmp471,sizeof(char),5U);}));_tmp470[0]=_tmp817;}),({
struct Cyc_PP_Doc*_tmp816=Cyc_Absynpp_pat2doc(_tmp4AF);_tmp470[1]=_tmp816;}),({
struct Cyc_PP_Doc*_tmp815=Cyc_PP_text(({const char*_tmp472=" = ";_tag_dyneither(_tmp472,sizeof(char),4U);}));_tmp470[2]=_tmp815;}),({
struct Cyc_PP_Doc*_tmp814=Cyc_Absynpp_exp2doc(_tmp4AE);_tmp470[3]=_tmp814;}),({
struct Cyc_PP_Doc*_tmp813=Cyc_PP_text(({const char*_tmp473=";";_tag_dyneither(_tmp473,sizeof(char),2U);}));_tmp470[4]=_tmp813;});Cyc_PP_cat(_tag_dyneither(_tmp470,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 3U: _LLD: _tmp4B0=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_LLE:
# 1883
 s=({struct Cyc_PP_Doc*_tmp474[3U];({struct Cyc_PP_Doc*_tmp81A=Cyc_PP_text(({const char*_tmp475="let ";_tag_dyneither(_tmp475,sizeof(char),5U);}));_tmp474[0]=_tmp81A;}),({struct Cyc_PP_Doc*_tmp819=Cyc_Absynpp_ids2doc(_tmp4B0);_tmp474[1]=_tmp819;}),({struct Cyc_PP_Doc*_tmp818=Cyc_PP_text(({const char*_tmp476=";";_tag_dyneither(_tmp476,sizeof(char),2U);}));_tmp474[2]=_tmp818;});Cyc_PP_cat(_tag_dyneither(_tmp474,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 7U: _LLF: _tmp4B1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_LL10:
# 1886
 s=({struct Cyc_PP_Doc*_tmp477[2U];({struct Cyc_PP_Doc*_tmp81C=Cyc_Absynpp_enumdecl2doc(_tmp4B1);_tmp477[0]=_tmp81C;}),({struct Cyc_PP_Doc*_tmp81B=Cyc_PP_text(({const char*_tmp478=";";_tag_dyneither(_tmp478,sizeof(char),2U);}));_tmp477[1]=_tmp81B;});Cyc_PP_cat(_tag_dyneither(_tmp477,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 8U: _LL11: _tmp4B2=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_LL12: {
# 1889
void*t;
if(_tmp4B2->defn != 0)
t=(void*)_check_null(_tmp4B2->defn);else{
# 1893
t=Cyc_Absyn_new_evar(_tmp4B2->kind,0);}
s=({struct Cyc_PP_Doc*_tmp479[4U];({struct Cyc_PP_Doc*_tmp825=Cyc_PP_text(({const char*_tmp47A="typedef ";_tag_dyneither(_tmp47A,sizeof(char),9U);}));_tmp479[0]=_tmp825;}),({
struct Cyc_PP_Doc*_tmp824=({struct Cyc_Absyn_Tqual _tmp823=_tmp4B2->tq;void*_tmp822=t;Cyc_Absynpp_tqtd2doc(_tmp823,_tmp822,({struct Cyc_Core_Opt*_tmp47C=_cycalloc(sizeof(*_tmp47C));({
# 1897
struct Cyc_PP_Doc*_tmp821=({struct Cyc_PP_Doc*_tmp47B[2U];({struct Cyc_PP_Doc*_tmp820=Cyc_Absynpp_typedef_name2bolddoc(_tmp4B2->name);_tmp47B[0]=_tmp820;}),({
struct Cyc_PP_Doc*_tmp81F=Cyc_Absynpp_tvars2doc(_tmp4B2->tvs);_tmp47B[1]=_tmp81F;});Cyc_PP_cat(_tag_dyneither(_tmp47B,sizeof(struct Cyc_PP_Doc*),2U));});
# 1897
_tmp47C->v=_tmp821;});_tmp47C;}));});
# 1895
_tmp479[1]=_tmp824;}),({
# 1900
struct Cyc_PP_Doc*_tmp81E=Cyc_Absynpp_atts2doc(_tmp4B2->atts);_tmp479[2]=_tmp81E;}),({
struct Cyc_PP_Doc*_tmp81D=Cyc_PP_text(({const char*_tmp47D=";";_tag_dyneither(_tmp47D,sizeof(char),2U);}));_tmp479[3]=_tmp81D;});Cyc_PP_cat(_tag_dyneither(_tmp479,sizeof(struct Cyc_PP_Doc*),4U));});
# 1903
goto _LL0;}case 9U: _LL13: _tmp4B4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_tmp4B3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp457)->f2;_LL14:
# 1905
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4B4);
s=({struct Cyc_PP_Doc*_tmp47E[8U];({struct Cyc_PP_Doc*_tmp82E=Cyc_PP_text(({const char*_tmp47F="namespace ";_tag_dyneither(_tmp47F,sizeof(char),11U);}));_tmp47E[0]=_tmp82E;}),({
struct Cyc_PP_Doc*_tmp82D=Cyc_PP_textptr(_tmp4B4);_tmp47E[1]=_tmp82D;}),({
struct Cyc_PP_Doc*_tmp82C=Cyc_PP_blank_doc();_tmp47E[2]=_tmp82C;}),({struct Cyc_PP_Doc*_tmp82B=Cyc_Absynpp_lb();_tmp47E[3]=_tmp82B;}),({
struct Cyc_PP_Doc*_tmp82A=Cyc_PP_line_doc();_tmp47E[4]=_tmp82A;}),({
struct Cyc_PP_Doc*_tmp829=({struct _dyneither_ptr _tmp828=({const char*_tmp480="";_tag_dyneither(_tmp480,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp828,_tmp4B3);});_tmp47E[5]=_tmp829;}),({
struct Cyc_PP_Doc*_tmp827=Cyc_PP_line_doc();_tmp47E[6]=_tmp827;}),({
struct Cyc_PP_Doc*_tmp826=Cyc_Absynpp_rb();_tmp47E[7]=_tmp826;});Cyc_PP_cat(_tag_dyneither(_tmp47E,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;case 10U: _LL15: _tmp4B6=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_tmp4B5=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp457)->f2;_LL16:
# 1916
 if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp481[8U];({struct Cyc_PP_Doc*_tmp837=Cyc_PP_text(({const char*_tmp482="using ";_tag_dyneither(_tmp482,sizeof(char),7U);}));_tmp481[0]=_tmp837;}),({
struct Cyc_PP_Doc*_tmp836=Cyc_Absynpp_qvar2doc(_tmp4B6);_tmp481[1]=_tmp836;}),({
struct Cyc_PP_Doc*_tmp835=Cyc_PP_blank_doc();_tmp481[2]=_tmp835;}),({struct Cyc_PP_Doc*_tmp834=Cyc_Absynpp_lb();_tmp481[3]=_tmp834;}),({
struct Cyc_PP_Doc*_tmp833=Cyc_PP_line_doc();_tmp481[4]=_tmp833;}),({
struct Cyc_PP_Doc*_tmp832=({struct _dyneither_ptr _tmp831=({const char*_tmp483="";_tag_dyneither(_tmp483,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp831,_tmp4B5);});_tmp481[5]=_tmp832;}),({
struct Cyc_PP_Doc*_tmp830=Cyc_PP_line_doc();_tmp481[6]=_tmp830;}),({
struct Cyc_PP_Doc*_tmp82F=Cyc_Absynpp_rb();_tmp481[7]=_tmp82F;});Cyc_PP_cat(_tag_dyneither(_tmp481,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1925
s=({struct Cyc_PP_Doc*_tmp484[11U];({struct Cyc_PP_Doc*_tmp843=Cyc_PP_text(({const char*_tmp485="/* using ";_tag_dyneither(_tmp485,sizeof(char),10U);}));_tmp484[0]=_tmp843;}),({
struct Cyc_PP_Doc*_tmp842=Cyc_Absynpp_qvar2doc(_tmp4B6);_tmp484[1]=_tmp842;}),({
struct Cyc_PP_Doc*_tmp841=Cyc_PP_blank_doc();_tmp484[2]=_tmp841;}),({
struct Cyc_PP_Doc*_tmp840=Cyc_Absynpp_lb();_tmp484[3]=_tmp840;}),({
struct Cyc_PP_Doc*_tmp83F=Cyc_PP_text(({const char*_tmp486=" */";_tag_dyneither(_tmp486,sizeof(char),4U);}));_tmp484[4]=_tmp83F;}),({
struct Cyc_PP_Doc*_tmp83E=Cyc_PP_line_doc();_tmp484[5]=_tmp83E;}),({
struct Cyc_PP_Doc*_tmp83D=({struct _dyneither_ptr _tmp83C=({const char*_tmp487="";_tag_dyneither(_tmp487,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp83C,_tmp4B5);});_tmp484[6]=_tmp83D;}),({
struct Cyc_PP_Doc*_tmp83B=Cyc_PP_line_doc();_tmp484[7]=_tmp83B;}),({
struct Cyc_PP_Doc*_tmp83A=Cyc_PP_text(({const char*_tmp488="/* ";_tag_dyneither(_tmp488,sizeof(char),4U);}));_tmp484[8]=_tmp83A;}),({
struct Cyc_PP_Doc*_tmp839=Cyc_Absynpp_rb();_tmp484[9]=_tmp839;}),({
struct Cyc_PP_Doc*_tmp838=Cyc_PP_text(({const char*_tmp489=" */";_tag_dyneither(_tmp489,sizeof(char),4U);}));_tmp484[10]=_tmp838;});Cyc_PP_cat(_tag_dyneither(_tmp484,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;case 11U: _LL17: _tmp4B7=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_LL18:
# 1938
 if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp48A[6U];({struct Cyc_PP_Doc*_tmp84A=Cyc_PP_text(({const char*_tmp48B="extern \"C\" ";_tag_dyneither(_tmp48B,sizeof(char),12U);}));_tmp48A[0]=_tmp84A;}),({
struct Cyc_PP_Doc*_tmp849=Cyc_Absynpp_lb();_tmp48A[1]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=Cyc_PP_line_doc();_tmp48A[2]=_tmp848;}),({
struct Cyc_PP_Doc*_tmp847=({struct _dyneither_ptr _tmp846=({const char*_tmp48C="";_tag_dyneither(_tmp48C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp846,_tmp4B7);});_tmp48A[3]=_tmp847;}),({
struct Cyc_PP_Doc*_tmp845=Cyc_PP_line_doc();_tmp48A[4]=_tmp845;}),({
struct Cyc_PP_Doc*_tmp844=Cyc_Absynpp_rb();_tmp48A[5]=_tmp844;});Cyc_PP_cat(_tag_dyneither(_tmp48A,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1946
s=({struct Cyc_PP_Doc*_tmp48D[9U];({struct Cyc_PP_Doc*_tmp854=Cyc_PP_text(({const char*_tmp48E="/* extern \"C\" ";_tag_dyneither(_tmp48E,sizeof(char),15U);}));_tmp48D[0]=_tmp854;}),({
struct Cyc_PP_Doc*_tmp853=Cyc_Absynpp_lb();_tmp48D[1]=_tmp853;}),({
struct Cyc_PP_Doc*_tmp852=Cyc_PP_text(({const char*_tmp48F=" */";_tag_dyneither(_tmp48F,sizeof(char),4U);}));_tmp48D[2]=_tmp852;}),({
struct Cyc_PP_Doc*_tmp851=Cyc_PP_line_doc();_tmp48D[3]=_tmp851;}),({
struct Cyc_PP_Doc*_tmp850=({struct _dyneither_ptr _tmp84F=({const char*_tmp490="";_tag_dyneither(_tmp490,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp84F,_tmp4B7);});_tmp48D[4]=_tmp850;}),({
struct Cyc_PP_Doc*_tmp84E=Cyc_PP_line_doc();_tmp48D[5]=_tmp84E;}),({
struct Cyc_PP_Doc*_tmp84D=Cyc_PP_text(({const char*_tmp491="/* ";_tag_dyneither(_tmp491,sizeof(char),4U);}));_tmp48D[6]=_tmp84D;}),({
struct Cyc_PP_Doc*_tmp84C=Cyc_Absynpp_rb();_tmp48D[7]=_tmp84C;}),({
struct Cyc_PP_Doc*_tmp84B=Cyc_PP_text(({const char*_tmp492=" */";_tag_dyneither(_tmp492,sizeof(char),4U);}));_tmp48D[8]=_tmp84B;});Cyc_PP_cat(_tag_dyneither(_tmp48D,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;case 12U: _LL19: _tmp4B9=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp457)->f1;_tmp4B8=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp457)->f2;_LL1A:
# 1957
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4B8 != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp493[7U];({struct Cyc_PP_Doc*_tmp85C=Cyc_Absynpp_rb();_tmp493[0]=_tmp85C;}),({struct Cyc_PP_Doc*_tmp85B=Cyc_PP_text(({const char*_tmp494=" export ";_tag_dyneither(_tmp494,sizeof(char),9U);}));_tmp493[1]=_tmp85B;}),({struct Cyc_PP_Doc*_tmp85A=Cyc_Absynpp_lb();_tmp493[2]=_tmp85A;}),({
struct Cyc_PP_Doc*_tmp859=Cyc_PP_line_doc();_tmp493[3]=_tmp859;}),({struct Cyc_PP_Doc*_tmp858=({struct _dyneither_ptr _tmp857=({const char*_tmp495=",";_tag_dyneither(_tmp495,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp857,_tmp4B8);});_tmp493[4]=_tmp858;}),({
struct Cyc_PP_Doc*_tmp856=Cyc_PP_line_doc();_tmp493[5]=_tmp856;}),({struct Cyc_PP_Doc*_tmp855=Cyc_Absynpp_rb();_tmp493[6]=_tmp855;});Cyc_PP_cat(_tag_dyneither(_tmp493,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1964
exs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp496[6U];({struct Cyc_PP_Doc*_tmp862=Cyc_PP_text(({const char*_tmp497="extern \"C include\" ";_tag_dyneither(_tmp497,sizeof(char),20U);}));_tmp496[0]=_tmp862;}),({
struct Cyc_PP_Doc*_tmp861=Cyc_Absynpp_lb();_tmp496[1]=_tmp861;}),({
struct Cyc_PP_Doc*_tmp860=Cyc_PP_line_doc();_tmp496[2]=_tmp860;}),({
struct Cyc_PP_Doc*_tmp85F=({struct _dyneither_ptr _tmp85E=({const char*_tmp498="";_tag_dyneither(_tmp498,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp85E,_tmp4B9);});_tmp496[3]=_tmp85F;}),({
struct Cyc_PP_Doc*_tmp85D=Cyc_PP_line_doc();_tmp496[4]=_tmp85D;}),_tmp496[5]=exs_doc;Cyc_PP_cat(_tag_dyneither(_tmp496,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1972
s=({struct Cyc_PP_Doc*_tmp499[9U];({struct Cyc_PP_Doc*_tmp86C=Cyc_PP_text(({const char*_tmp49A="/* extern \"C include\" ";_tag_dyneither(_tmp49A,sizeof(char),23U);}));_tmp499[0]=_tmp86C;}),({
struct Cyc_PP_Doc*_tmp86B=Cyc_Absynpp_lb();_tmp499[1]=_tmp86B;}),({
struct Cyc_PP_Doc*_tmp86A=Cyc_PP_text(({const char*_tmp49B=" */";_tag_dyneither(_tmp49B,sizeof(char),4U);}));_tmp499[2]=_tmp86A;}),({
struct Cyc_PP_Doc*_tmp869=Cyc_PP_line_doc();_tmp499[3]=_tmp869;}),({
struct Cyc_PP_Doc*_tmp868=({struct _dyneither_ptr _tmp867=({const char*_tmp49C="";_tag_dyneither(_tmp49C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp867,_tmp4B9);});_tmp499[4]=_tmp868;}),({
struct Cyc_PP_Doc*_tmp866=Cyc_PP_line_doc();_tmp499[5]=_tmp866;}),({
struct Cyc_PP_Doc*_tmp865=Cyc_PP_text(({const char*_tmp49D="/* ";_tag_dyneither(_tmp49D,sizeof(char),4U);}));_tmp499[6]=_tmp865;}),({
struct Cyc_PP_Doc*_tmp864=Cyc_Absynpp_rb();_tmp499[7]=_tmp864;}),({
struct Cyc_PP_Doc*_tmp863=Cyc_PP_text(({const char*_tmp49E=" */";_tag_dyneither(_tmp49E,sizeof(char),4U);}));_tmp499[8]=_tmp863;});Cyc_PP_cat(_tag_dyneither(_tmp499,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;case 13U: _LL1B: _LL1C:
# 1983
 s=({struct Cyc_PP_Doc*_tmp49F[2U];({struct Cyc_PP_Doc*_tmp86E=Cyc_PP_text(({const char*_tmp4A0="__cyclone_port_on__;";_tag_dyneither(_tmp4A0,sizeof(char),21U);}));_tmp49F[0]=_tmp86E;}),({struct Cyc_PP_Doc*_tmp86D=Cyc_Absynpp_lb();_tmp49F[1]=_tmp86D;});Cyc_PP_cat(_tag_dyneither(_tmp49F,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 14U: _LL1D: _LL1E:
# 1986
 s=({struct Cyc_PP_Doc*_tmp4A1[2U];({struct Cyc_PP_Doc*_tmp870=Cyc_PP_text(({const char*_tmp4A2="__cyclone_port_off__;";_tag_dyneither(_tmp4A2,sizeof(char),22U);}));_tmp4A1[0]=_tmp870;}),({struct Cyc_PP_Doc*_tmp86F=Cyc_Absynpp_lb();_tmp4A1[1]=_tmp86F;});Cyc_PP_cat(_tag_dyneither(_tmp4A1,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1989
 s=({struct Cyc_PP_Doc*_tmp4A3[2U];({struct Cyc_PP_Doc*_tmp872=Cyc_PP_text(({const char*_tmp4A4="__tempest_on__;";_tag_dyneither(_tmp4A4,sizeof(char),16U);}));_tmp4A3[0]=_tmp872;}),({struct Cyc_PP_Doc*_tmp871=Cyc_Absynpp_lb();_tmp4A3[1]=_tmp871;});Cyc_PP_cat(_tag_dyneither(_tmp4A3,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL21: _LL22:
# 1992
 s=({struct Cyc_PP_Doc*_tmp4A5[2U];({struct Cyc_PP_Doc*_tmp874=Cyc_PP_text(({const char*_tmp4A6="__tempest_off__;";_tag_dyneither(_tmp4A6,sizeof(char),17U);}));_tmp4A5[0]=_tmp874;}),({struct Cyc_PP_Doc*_tmp873=Cyc_Absynpp_lb();_tmp4A5[1]=_tmp873;});Cyc_PP_cat(_tag_dyneither(_tmp4A5,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 1996
return s;}
# 1999
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4BA=sc;switch(_tmp4BA){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp4BB="static ";_tag_dyneither(_tmp4BB,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp4BC="extern ";_tag_dyneither(_tmp4BC,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp4BD="extern \"C\" ";_tag_dyneither(_tmp4BD,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp4BE="abstract ";_tag_dyneither(_tmp4BE,sizeof(char),10U);}));default: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp4BF="register ";_tag_dyneither(_tmp4BF,sizeof(char),10U);}));}_LL0:;};}
# 2012
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4C0=Cyc_Tcutil_compress(t);void*_tmp4C1=_tmp4C0;struct Cyc_List_List*_tmp4C3;struct Cyc_Absyn_Tvar*_tmp4C2;switch(*((int*)_tmp4C1)){case 2U: _LL1: _tmp4C2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4C1)->f1;_LL2:
 return Cyc_Tcutil_is_temp_tvar(_tmp4C2);case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f1)->tag == 9U){_LL3: _tmp4C3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f2;_LL4:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4C3);}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2024
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4C4=t;struct Cyc_Absyn_Typedefdecl*_tmp4C6;void*_tmp4C5;switch(*((int*)_tmp4C4)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C4)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp4C6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C4)->f3;_tmp4C5=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C4)->f4;if(_tmp4C5 != 0){_LL6:
# 2031
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4C5);}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2038
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2041
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmp4C7=({struct _tuple16 _tmp549;_tmp549.f1=(void*)tms->hd,_tmp549.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp549;});struct _tuple16 _tmp4C8=_tmp4C7;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4C8.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4C8.f2)->tag == 3U){_LL1: _LL2:
# 2044
 return({struct Cyc_List_List*_tmp4CA=_region_malloc(r,sizeof(*_tmp4CA));_tmp4CA->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp876=({struct Cyc_List_List*_tmp4C9=_region_malloc(r,sizeof(*_tmp4C9));_tmp4C9->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp875=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4C9->tl=_tmp875;});_tmp4C9;});_tmp4CA->tl=_tmp876;});_tmp4CA;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp4CB=_region_malloc(r,sizeof(*_tmp4CB));_tmp4CB->hd=atts,_tmp4CB->tl=tms;_tmp4CB;});}_LL0:;}else{
# 2047
return({struct Cyc_List_List*_tmp4CC=_region_malloc(r,sizeof(*_tmp4CC));_tmp4CC->hd=atts,_tmp4CC->tl=tms;_tmp4CC;});}}
# 2052
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2054
void*_tmp4CD=t;struct _tuple0*_tmp50A;struct Cyc_List_List*_tmp509;struct Cyc_Absyn_Typedefdecl*_tmp508;void*_tmp507;struct Cyc_Core_Opt*_tmp506;void*_tmp505;int _tmp504;struct Cyc_List_List*_tmp503;void*_tmp502;struct Cyc_Absyn_Tqual _tmp501;void*_tmp500;struct Cyc_List_List*_tmp4FF;int _tmp4FE;struct Cyc_Absyn_VarargInfo*_tmp4FD;struct Cyc_List_List*_tmp4FC;struct Cyc_List_List*_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4F9;void*_tmp4F8;struct Cyc_Absyn_Tqual _tmp4F7;struct Cyc_Absyn_PtrAtts _tmp4F6;void*_tmp4F5;struct Cyc_Absyn_Tqual _tmp4F4;struct Cyc_Absyn_Exp*_tmp4F3;void*_tmp4F2;unsigned int _tmp4F1;switch(*((int*)_tmp4CD)){case 4U: _LL1: _tmp4F5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CD)->f1).elt_type;_tmp4F4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CD)->f1).tq;_tmp4F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CD)->f1).num_elts;_tmp4F2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CD)->f1).zero_term;_tmp4F1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CD)->f1).zt_loc;_LL2: {
# 2057
struct _tuple12 _tmp4CE=Cyc_Absynpp_to_tms(r,_tmp4F4,_tmp4F5);struct _tuple12 _tmp4CF=_tmp4CE;struct Cyc_Absyn_Tqual _tmp4D5;void*_tmp4D4;struct Cyc_List_List*_tmp4D3;_LLE: _tmp4D5=_tmp4CF.f1;_tmp4D4=_tmp4CF.f2;_tmp4D3=_tmp4CF.f3;_LLF:;{
void*tm;
if(_tmp4F3 == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0));_tmp4D0->tag=0U,_tmp4D0->f1=_tmp4F2,_tmp4D0->f2=_tmp4F1;_tmp4D0;});else{
# 2062
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1));_tmp4D1->tag=1U,_tmp4D1->f1=_tmp4F3,_tmp4D1->f2=_tmp4F2,_tmp4D1->f3=_tmp4F1;_tmp4D1;});}
return({struct _tuple12 _tmp54A;_tmp54A.f1=_tmp4D5,_tmp54A.f2=_tmp4D4,({struct Cyc_List_List*_tmp877=({struct Cyc_List_List*_tmp4D2=_region_malloc(r,sizeof(*_tmp4D2));_tmp4D2->hd=tm,_tmp4D2->tl=_tmp4D3;_tmp4D2;});_tmp54A.f3=_tmp877;});_tmp54A;});};}case 3U: _LL3: _tmp4F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).elt_type;_tmp4F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).elt_tq;_tmp4F6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts;_LL4: {
# 2066
struct _tuple12 _tmp4D6=Cyc_Absynpp_to_tms(r,_tmp4F7,_tmp4F8);struct _tuple12 _tmp4D7=_tmp4D6;struct Cyc_Absyn_Tqual _tmp4DC;void*_tmp4DB;struct Cyc_List_List*_tmp4DA;_LL11: _tmp4DC=_tmp4D7.f1;_tmp4DB=_tmp4D7.f2;_tmp4DA=_tmp4D7.f3;_LL12:;
_tmp4DA=({struct Cyc_List_List*_tmp4D9=_region_malloc(r,sizeof(*_tmp4D9));({void*_tmp878=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8));_tmp4D8->tag=2U,_tmp4D8->f1=_tmp4F6,_tmp4D8->f2=tq;_tmp4D8;});_tmp4D9->hd=_tmp878;}),_tmp4D9->tl=_tmp4DA;_tmp4D9;});
return({struct _tuple12 _tmp54B;_tmp54B.f1=_tmp4DC,_tmp54B.f2=_tmp4DB,_tmp54B.f3=_tmp4DA;_tmp54B;});}case 5U: _LL5: _tmp503=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).tvars;_tmp502=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).effect;_tmp501=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).ret_tqual;_tmp500=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).ret_type;_tmp4FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).args;_tmp4FE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).c_varargs;_tmp4FD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).cyc_varargs;_tmp4FC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).rgn_po;_tmp4FB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).attributes;_tmp4FA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).requires_clause;_tmp4F9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CD)->f1).ensures_clause;_LL6:
# 2072
 if(!Cyc_Absynpp_print_all_tvars){
# 2076
if(_tmp502 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp502)){
_tmp502=0;
_tmp503=0;}}else{
# 2081
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2084
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp503);}{
# 2089
struct _tuple12 _tmp4DD=Cyc_Absynpp_to_tms(r,_tmp501,_tmp500);struct _tuple12 _tmp4DE=_tmp4DD;struct Cyc_Absyn_Tqual _tmp4F0;void*_tmp4EF;struct Cyc_List_List*_tmp4EE;_LL14: _tmp4F0=_tmp4DE.f1;_tmp4EF=_tmp4DE.f2;_tmp4EE=_tmp4DE.f3;_LL15:;{
struct Cyc_List_List*tms=_tmp4EE;
# 2100 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp4DF=Cyc_Cyclone_c_compiler;if(_tmp4DF == Cyc_Cyclone_Gcc_c){_LL17: _LL18:
# 2102
 if(_tmp4FB != 0)
# 2105
tms=({struct _RegionHandle*_tmp87A=r;void*_tmp879=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E0=_region_malloc(r,sizeof(*_tmp4E0));_tmp4E0->tag=5U,_tmp4E0->f1=0U,_tmp4E0->f2=_tmp4FB;_tmp4E0;});Cyc_Absynpp_bubble_attributes(_tmp87A,_tmp879,tms);});
# 2107
tms=({struct Cyc_List_List*_tmp4E3=_region_malloc(r,sizeof(*_tmp4E3));({void*_tmp87C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E2=_region_malloc(r,sizeof(*_tmp4E2));
_tmp4E2->tag=3U,({void*_tmp87B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E1=_region_malloc(r,sizeof(*_tmp4E1));_tmp4E1->tag=1U,_tmp4E1->f1=_tmp4FF,_tmp4E1->f2=_tmp4FE,_tmp4E1->f3=_tmp4FD,_tmp4E1->f4=_tmp502,_tmp4E1->f5=_tmp4FC,_tmp4E1->f6=_tmp4FA,_tmp4E1->f7=_tmp4F9;_tmp4E1;});_tmp4E2->f1=_tmp87B;});_tmp4E2;});
# 2107
_tmp4E3->hd=_tmp87C;}),_tmp4E3->tl=tms;_tmp4E3;});
# 2111
goto _LL16;}else{_LL19: _LL1A:
# 2113
 tms=({struct Cyc_List_List*_tmp4E6=_region_malloc(r,sizeof(*_tmp4E6));({void*_tmp87E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E5=_region_malloc(r,sizeof(*_tmp4E5));
_tmp4E5->tag=3U,({void*_tmp87D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4));_tmp4E4->tag=1U,_tmp4E4->f1=_tmp4FF,_tmp4E4->f2=_tmp4FE,_tmp4E4->f3=_tmp4FD,_tmp4E4->f4=_tmp502,_tmp4E4->f5=_tmp4FC,_tmp4E4->f6=_tmp4FA,_tmp4E4->f7=_tmp4F9;_tmp4E4;});_tmp4E5->f1=_tmp87D;});_tmp4E5;});
# 2113
_tmp4E6->hd=_tmp87E;}),_tmp4E6->tl=tms;_tmp4E6;});
# 2117
for(0;_tmp4FB != 0;_tmp4FB=_tmp4FB->tl){
void*_tmp4E7=(void*)_tmp4FB->hd;void*_tmp4E8=_tmp4E7;switch(*((int*)_tmp4E8)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2122
 tms=({struct Cyc_List_List*_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB));({void*_tmp880=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4EA=_region_malloc(r,sizeof(*_tmp4EA));_tmp4EA->tag=5U,_tmp4EA->f1=0U,({struct Cyc_List_List*_tmp87F=({struct Cyc_List_List*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->hd=(void*)_tmp4FB->hd,_tmp4E9->tl=0;_tmp4E9;});_tmp4EA->f2=_tmp87F;});_tmp4EA;});_tmp4EB->hd=_tmp880;}),_tmp4EB->tl=tms;_tmp4EB;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2126
goto _LL16;}_LL16:;}
# 2129
AfterAtts:
 if(_tmp503 != 0)
tms=({struct Cyc_List_List*_tmp4ED=_region_malloc(r,sizeof(*_tmp4ED));({void*_tmp881=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4EC=_region_malloc(r,sizeof(*_tmp4EC));_tmp4EC->tag=4U,_tmp4EC->f1=_tmp503,_tmp4EC->f2=0U,_tmp4EC->f3=1;_tmp4EC;});_tmp4ED->hd=_tmp881;}),_tmp4ED->tl=tms;_tmp4ED;});
return({struct _tuple12 _tmp54C;_tmp54C.f1=_tmp4F0,_tmp54C.f2=_tmp4EF,_tmp54C.f3=tms;_tmp54C;});};};case 1U: _LL7: _tmp506=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4CD)->f1;_tmp505=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4CD)->f2;_tmp504=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4CD)->f3;_LL8:
# 2135
 if(_tmp505 == 0)
return({struct _tuple12 _tmp54D;_tmp54D.f1=tq,_tmp54D.f2=t,_tmp54D.f3=0;_tmp54D;});else{
# 2138
return Cyc_Absynpp_to_tms(r,tq,_tmp505);}case 8U: _LL9: _tmp50A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4CD)->f1;_tmp509=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4CD)->f2;_tmp508=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4CD)->f3;_tmp507=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4CD)->f4;_LLA:
# 2144
 if(_tmp507 == 0  || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple12 _tmp54E;_tmp54E.f1=tq,_tmp54E.f2=t,_tmp54E.f3=0;_tmp54E;});else{
# 2147
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp507);}default: _LLB: _LLC:
# 2152
 return({struct _tuple12 _tmp54F;_tmp54F.f1=tq,_tmp54F.f2=t,_tmp54F.f3=0;_tmp54F;});}_LL0:;}
# 2156
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2158
void*_tmp50B=t;void*_tmp510;void*_tmp50F;switch(*((int*)_tmp50B)){case 1U: _LL1: _tmp50F=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50B)->f2;if(_tmp50F != 0){_LL2:
 return Cyc_Absynpp_is_char_ptr(_tmp50F);}else{goto _LL5;}case 3U: _LL3: _tmp510=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50B)->f1).elt_type;_LL4:
# 2161
 L: {
void*_tmp50C=_tmp510;void*_tmp50E;void*_tmp50D;switch(*((int*)_tmp50C)){case 1U: _LL8: _tmp50D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50C)->f2;if(_tmp50D != 0){_LL9:
 _tmp510=_tmp50D;goto L;}else{goto _LLE;}case 8U: _LLA: _tmp50E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50C)->f4;if(_tmp50E != 0){_LLB:
 _tmp510=_tmp50E;goto L;}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50C)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50C)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 2168
 return 0;}_LL0:;}
# 2172
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp511=_new_region("temp");struct _RegionHandle*temp=& _tmp511;_push_region(temp);
{struct _tuple12 _tmp512=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _tmp513=_tmp512;struct Cyc_Absyn_Tqual _tmp51B;void*_tmp51A;struct Cyc_List_List*_tmp519;_LL1: _tmp51B=_tmp513.f1;_tmp51A=_tmp513.f2;_tmp519=_tmp513.f3;_LL2:;
_tmp519=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp519);
if(_tmp519 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmp515=({struct Cyc_PP_Doc*_tmp514[2U];({struct Cyc_PP_Doc*_tmp883=Cyc_Absynpp_tqual2doc(_tmp51B);_tmp514[0]=_tmp883;}),({struct Cyc_PP_Doc*_tmp882=Cyc_Absynpp_ntyp2doc(_tmp51A);_tmp514[1]=_tmp882;});Cyc_PP_cat(_tag_dyneither(_tmp514,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp515;}else{
# 2179
struct Cyc_PP_Doc*_tmp518=({struct Cyc_PP_Doc*_tmp516[4U];({
struct Cyc_PP_Doc*_tmp889=Cyc_Absynpp_tqual2doc(_tmp51B);_tmp516[0]=_tmp889;}),({
struct Cyc_PP_Doc*_tmp888=Cyc_Absynpp_ntyp2doc(_tmp51A);_tmp516[1]=_tmp888;}),({
struct Cyc_PP_Doc*_tmp887=Cyc_PP_text(({const char*_tmp517=" ";_tag_dyneither(_tmp517,sizeof(char),2U);}));_tmp516[2]=_tmp887;}),({
struct Cyc_PP_Doc*_tmp886=({int _tmp885=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp884=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp885,_tmp884,_tmp519);});_tmp516[3]=_tmp886;});Cyc_PP_cat(_tag_dyneither(_tmp516,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp518;}}
# 2174
;_pop_region(temp);}
# 2187
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp51C=f->requires_clause;
if((unsigned int)_tmp51C)
requires_doc=({struct Cyc_PP_Doc*_tmp51D[2U];({struct Cyc_PP_Doc*_tmp88B=Cyc_PP_text(({const char*_tmp51E="@requires ";_tag_dyneither(_tmp51E,sizeof(char),11U);}));_tmp51D[0]=_tmp88B;}),({struct Cyc_PP_Doc*_tmp88A=Cyc_Absynpp_exp2doc(_tmp51C);_tmp51D[1]=_tmp88A;});Cyc_PP_cat(_tag_dyneither(_tmp51D,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2193
requires_doc=Cyc_PP_nil_doc();}{
# 2195
enum Cyc_Cyclone_C_Compilers _tmp51F=Cyc_Cyclone_c_compiler;if(_tmp51F == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 2198
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp520[5U];({struct Cyc_PP_Doc*_tmp893=({struct Cyc_Absyn_Tqual _tmp892=f->tq;void*_tmp891=f->type;Cyc_Absynpp_tqtd2doc(_tmp892,_tmp891,({struct Cyc_Core_Opt*_tmp521=_cycalloc(sizeof(*_tmp521));({struct Cyc_PP_Doc*_tmp890=Cyc_PP_textptr(f->name);_tmp521->v=_tmp890;});_tmp521;}));});_tmp520[0]=_tmp893;}),({
struct Cyc_PP_Doc*_tmp88F=Cyc_PP_text(({const char*_tmp522=":";_tag_dyneither(_tmp522,sizeof(char),2U);}));_tmp520[1]=_tmp88F;}),({struct Cyc_PP_Doc*_tmp88E=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp520[2]=_tmp88E;}),({
struct Cyc_PP_Doc*_tmp88D=Cyc_Absynpp_atts2doc(f->attributes);_tmp520[3]=_tmp88D;}),({struct Cyc_PP_Doc*_tmp88C=Cyc_PP_text(({const char*_tmp523=";";_tag_dyneither(_tmp523,sizeof(char),2U);}));_tmp520[4]=_tmp88C;});Cyc_PP_cat(_tag_dyneither(_tmp520,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2203
return({struct Cyc_PP_Doc*_tmp524[4U];({struct Cyc_PP_Doc*_tmp899=({struct Cyc_Absyn_Tqual _tmp898=f->tq;void*_tmp897=f->type;Cyc_Absynpp_tqtd2doc(_tmp898,_tmp897,({struct Cyc_Core_Opt*_tmp525=_cycalloc(sizeof(*_tmp525));({struct Cyc_PP_Doc*_tmp896=Cyc_PP_textptr(f->name);_tmp525->v=_tmp896;});_tmp525;}));});_tmp524[0]=_tmp899;}),({
struct Cyc_PP_Doc*_tmp895=Cyc_Absynpp_atts2doc(f->attributes);_tmp524[1]=_tmp895;}),_tmp524[2]=requires_doc,({struct Cyc_PP_Doc*_tmp894=Cyc_PP_text(({const char*_tmp526=";";_tag_dyneither(_tmp526,sizeof(char),2U);}));_tmp524[3]=_tmp894;});Cyc_PP_cat(_tag_dyneither(_tmp524,sizeof(struct Cyc_PP_Doc*),4U));});}}else{_LL3: _LL4:
# 2206
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp527[5U];({struct Cyc_PP_Doc*_tmp8A1=Cyc_Absynpp_atts2doc(f->attributes);_tmp527[0]=_tmp8A1;}),({
struct Cyc_PP_Doc*_tmp8A0=({struct Cyc_Absyn_Tqual _tmp89F=f->tq;void*_tmp89E=f->type;Cyc_Absynpp_tqtd2doc(_tmp89F,_tmp89E,({struct Cyc_Core_Opt*_tmp528=_cycalloc(sizeof(*_tmp528));({struct Cyc_PP_Doc*_tmp89D=Cyc_PP_textptr(f->name);_tmp528->v=_tmp89D;});_tmp528;}));});_tmp527[1]=_tmp8A0;}),({
struct Cyc_PP_Doc*_tmp89C=Cyc_PP_text(({const char*_tmp529=":";_tag_dyneither(_tmp529,sizeof(char),2U);}));_tmp527[2]=_tmp89C;}),({struct Cyc_PP_Doc*_tmp89B=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp527[3]=_tmp89B;}),({struct Cyc_PP_Doc*_tmp89A=Cyc_PP_text(({const char*_tmp52A=";";_tag_dyneither(_tmp52A,sizeof(char),2U);}));_tmp527[4]=_tmp89A;});Cyc_PP_cat(_tag_dyneither(_tmp527,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2211
return({struct Cyc_PP_Doc*_tmp52B[4U];({struct Cyc_PP_Doc*_tmp8A7=Cyc_Absynpp_atts2doc(f->attributes);_tmp52B[0]=_tmp8A7;}),({
struct Cyc_PP_Doc*_tmp8A6=({struct Cyc_Absyn_Tqual _tmp8A5=f->tq;void*_tmp8A4=f->type;Cyc_Absynpp_tqtd2doc(_tmp8A5,_tmp8A4,({struct Cyc_Core_Opt*_tmp52C=_cycalloc(sizeof(*_tmp52C));({struct Cyc_PP_Doc*_tmp8A3=Cyc_PP_textptr(f->name);_tmp52C->v=_tmp8A3;});_tmp52C;}));});_tmp52B[1]=_tmp8A6;}),_tmp52B[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp8A2=Cyc_PP_text(({const char*_tmp52D=";";_tag_dyneither(_tmp52D,sizeof(char),2U);}));_tmp52B[3]=_tmp8A2;});Cyc_PP_cat(_tag_dyneither(_tmp52B,sizeof(struct Cyc_PP_Doc*),4U));});}}_LL0:;};}
# 2218
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp8A8=({const char*_tmp52E="";_tag_dyneither(_tmp52E,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp8A8,fields);});}
# 2222
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp52F[3U];({struct Cyc_PP_Doc*_tmp8AC=Cyc_Absynpp_scope2doc(f->sc);_tmp52F[0]=_tmp8AC;}),({struct Cyc_PP_Doc*_tmp8AB=Cyc_Absynpp_typedef_name2doc(f->name);_tmp52F[1]=_tmp8AB;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp8AA=Cyc_PP_nil_doc();_tmp52F[2]=_tmp8AA;}):({struct Cyc_PP_Doc*_tmp8A9=Cyc_Absynpp_args2doc(f->typs);_tmp52F[2]=_tmp8A9;});Cyc_PP_cat(_tag_dyneither(_tmp52F,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2227
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp8AD=({const char*_tmp530=",";_tag_dyneither(_tmp530,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp8AD,fields);});}
# 2236
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp8AE=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp8AE,72,f);});
({void*_tmp531=0U;({struct Cyc___cycFILE*_tmp8B0=f;struct _dyneither_ptr _tmp8AF=({const char*_tmp532="\n";_tag_dyneither(_tmp532,sizeof(char),2U);});Cyc_fprintf(_tmp8B0,_tmp8AF,_tag_dyneither(_tmp531,sizeof(void*),0U));});});}}
# 2243
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _dyneither_ptr _tmp8B1=({const char*_tmp533="";_tag_dyneither(_tmp533,sizeof(char),1U);});Cyc_PP_seql(_tmp8B1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2246
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2250
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_tvar2doc(t),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;};}
# 2261
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}

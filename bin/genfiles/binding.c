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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172 "list.h"
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;long double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U,Cyc_Absyn_BoolKind  = 6U,Cyc_Absyn_PtrBndKind  = 7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 301
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 314
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 323
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 705 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 888
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 925
extern void*Cyc_Absyn_heap_rgn_type;
# 929
extern void*Cyc_Absyn_false_type;
# 931
extern void*Cyc_Absyn_void_type;
# 947
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud();
# 978
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 1090
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1099
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 26 "warn.h"
void Cyc_Warn_vwarn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 28
void Cyc_Warn_warn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 30
void Cyc_Warn_flush_warnings();
# 32
void Cyc_Warn_verr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 39
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void*Cyc_Warn_vimpos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29 "binding.h"
extern int Cyc_Binding_warn_override;
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 36 "cifc.h"
void Cyc_Cifc_user_overrides(unsigned int loc,struct Cyc_List_List*ds,struct Cyc_List_List*ovrs);char Cyc_Binding_BindingError[13U]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 45 "binding.cyc"
int Cyc_Binding_warn_override=0;
# 59
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);
return({struct Cyc_Binding_NSCtxt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->curr_ns=0,({
struct Cyc_List_List*_tmp2B2=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));({void*_tmp2B1=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=0U,_tmp1->f1=0;_tmp1;});_tmp2->hd=_tmp2B1;}),_tmp2->tl=0;_tmp2;});_tmp3->availables=_tmp2B2;}),({
struct Cyc_Dict_Dict _tmp2B0=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,_tmp0);_tmp3->ns_data=_tmp2B0;});_tmp3;});}
# 65
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp4=abs_ns;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;switch((_tmp4.Abs_n).tag){case 3U: _LL1: _tmp7=(_tmp4.C_n).val;_LL2:
 _tmp8=_tmp7;goto _LL4;case 2U: _LL3: _tmp8=(_tmp4.Abs_n).val;_LL4:
 return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp8);default: _LL5: _LL6:
({void*_tmp5=0U;({struct _dyneither_ptr _tmp2B3=({const char*_tmp6="Binding:get_ns_data: relative ns";_tag_dyneither(_tmp6,sizeof(char),33U);});Cyc_Warn_impos(_tmp2B3,_tag_dyneither(_tmp5,sizeof(void*),0U));});});}_LL0:;}
# 83 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 86
struct Cyc_List_List*_tmp9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp9))
return _tmp9;
{struct Cyc_List_List*_tmpA=ctxt->availables;for(0;_tmpA != 0;_tmpA=_tmpA->tl){
void*_tmpB=(void*)_tmpA->hd;void*_tmpC=_tmpB;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmpE;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->tag == 1U){_LL1: _tmpE=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->f1;_LL2:
 _tmpF=_tmpE;goto _LL4;}else{_LL3: _tmpF=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpC)->f1;_LL4: {
# 93
struct Cyc_List_List*_tmpD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpF,rel_ns);
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmpD))
return _tmpD;
goto _LL0;}}_LL0:;}}
# 98
({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0U,({struct _dyneither_ptr _tmp2B5=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_List_List*_tmp2B4=rel_ns;Cyc_str_sepstr(_tmp2B4,({const char*_tmp13="::";_tag_dyneither(_tmp13,sizeof(char),3U);}));}));_tmp29E.f1=_tmp2B5;});_tmp29E;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({unsigned int _tmp2B7=loc;struct _dyneither_ptr _tmp2B6=({const char*_tmp11="namespace %s not found";_tag_dyneither(_tmp11,sizeof(char),23U);});Cyc_Warn_err(_tmp2B7,_tmp2B6,_tag_dyneither(_tmp10,sizeof(void*),1U));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=Cyc_Binding_BindingError;_tmp14;}));}
# 103
void*Cyc_Binding_resolve_lookup(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _dyneither_ptr*)){
struct _tuple0*_tmp15=qv;union Cyc_Absyn_Nmspace _tmp3E;struct _dyneither_ptr*_tmp3D;_LL1: _tmp3E=_tmp15->f1;_tmp3D=_tmp15->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp16=_tmp3E;struct Cyc_List_List*_tmp3C;struct Cyc_List_List*_tmp3B;struct Cyc_List_List*_tmp3A;switch((_tmp16.Abs_n).tag){case 1U: if((_tmp16.Rel_n).val == 0){_LL4: _LL5:
# 107
{struct _handler_cons _tmp17;_push_handler(& _tmp17);{int _tmp19=0;if(setjmp(_tmp17.handler))_tmp19=1;if(!_tmp19){{void*_tmp1A=({void*(*_tmp2B9)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2B8=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns);_tmp2B9(_tmp2B8,_tmp3D);});_npop_handler(0U);return _tmp1A;};_pop_handler();}else{void*_tmp18=(void*)_exn_thrown;void*_tmp1B=_tmp18;void*_tmp1C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B)->tag == Cyc_Dict_Absent){_LLF: _LL10:
 goto _LLE;}else{_LL11: _tmp1C=_tmp1B;_LL12:(int)_rethrow(_tmp1C);}_LLE:;}};}
{struct Cyc_List_List*_tmp1D=ctxt->availables;for(0;_tmp1D != 0;_tmp1D=_tmp1D->tl){
void*_tmp1E=(void*)_tmp1D->hd;void*_tmp1F=_tmp1E;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp26;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp1F)->tag == 1U){_LL14: _tmp26=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp1F)->f1;_LL15:
 _tmp27=_tmp26;goto _LL17;}else{_LL16: _tmp27=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp1F)->f1;_LL17:
# 113
{struct _handler_cons _tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(_tmp20.handler))_tmp22=1;if(!_tmp22){{void*_tmp23=({void*(*_tmp2BB)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2BA=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp27);_tmp2BB(_tmp2BA,_tmp3D);});_npop_handler(0U);return _tmp23;};_pop_handler();}else{void*_tmp21=(void*)_exn_thrown;void*_tmp24=_tmp21;void*_tmp25;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp24)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
 goto _LL18;}else{_LL1B: _tmp25=_tmp24;_LL1C:(int)_rethrow(_tmp25);}_LL18:;}};}
goto _LL13;}_LL13:;}}
# 117
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=Cyc_Binding_BindingError;_tmp28;}));}else{_LL6: _tmp3A=(_tmp16.Rel_n).val;_LL7: {
# 119
struct _handler_cons _tmp29;_push_handler(& _tmp29);{int _tmp2B=0;if(setjmp(_tmp29.handler))_tmp2B=1;if(!_tmp2B){
{struct Cyc_List_List*_tmp2C=Cyc_Binding_resolve_rel_ns(loc,ctxt,_tmp3A);
void*_tmp2D=({void*(*_tmp2BD)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2BC=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp2C);_tmp2BD(_tmp2BC,_tmp3D);});_npop_handler(0U);return _tmp2D;}
# 120
;_pop_handler();}else{void*_tmp2A=(void*)_exn_thrown;void*_tmp2E=_tmp2A;void*_tmp30;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2E)->tag == Cyc_Dict_Absent){_LL1E: _LL1F:
# 122
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=Cyc_Binding_BindingError;_tmp2F;}));}else{_LL20: _tmp30=_tmp2E;_LL21:(int)_rethrow(_tmp30);}_LL1D:;}};}}case 3U: _LL8: _tmp3B=(_tmp16.C_n).val;_LL9:
 _tmp3C=_tmp3B;goto _LLB;case 2U: _LLA: _tmp3C=(_tmp16.Abs_n).val;_LLB: {
# 125
struct _handler_cons _tmp31;_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))_tmp33=1;if(!_tmp33){{void*_tmp34=({void*(*_tmp2BF)(void*,struct _dyneither_ptr*)=lookup;void*_tmp2BE=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,_tmp3C);_tmp2BF(_tmp2BE,_tmp3D);});_npop_handler(0U);return _tmp34;};_pop_handler();}else{void*_tmp32=(void*)_exn_thrown;void*_tmp35=_tmp32;void*_tmp37;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp35)->tag == Cyc_Dict_Absent){_LL23: _LL24:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->tag=Cyc_Binding_BindingError;_tmp36;}));}else{_LL25: _tmp37=_tmp35;_LL26:(int)_rethrow(_tmp37);}_LL22:;}};}default: _LLC: _LLD:
({void*_tmp38=0U;({struct _dyneither_ptr _tmp2C0=({const char*_tmp39="lookup local in global";_tag_dyneither(_tmp39,sizeof(char),23U);});Cyc_Warn_impos(_tmp2C0,_tag_dyneither(_tmp38,sizeof(void*),0U));});});}_LL3:;};}
# 131
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp3F=ctxt->curr_ns;
struct Cyc_List_List*_tmp40=({struct Cyc_List_List*_tmp2C1=_tmp3F;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2C1,({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->hd=subname,_tmp43->tl=0;_tmp43;}));});
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,_tmp40))
({struct Cyc_Dict_Dict _tmp2C4=({struct Cyc_Dict_Dict _tmp2C3=ctxt->ns_data;struct Cyc_List_List*_tmp2C2=_tmp40;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(_tmp2C3,_tmp2C2,mkdata(env));});ctxt->ns_data=_tmp2C4;});
ctxt->curr_ns=_tmp40;
({struct Cyc_List_List*_tmp2C6=({struct Cyc_List_List*_tmp42=_cycalloc(sizeof(*_tmp42));({void*_tmp2C5=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->tag=0U,_tmp41->f1=_tmp40;_tmp41;});_tmp42->hd=_tmp2C5;}),_tmp42->tl=ctxt->availables;_tmp42;});ctxt->availables=_tmp2C6;});}
# 139
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp44=0U;({struct _dyneither_ptr _tmp2C7=({const char*_tmp45="leaving topmost namespace";_tag_dyneither(_tmp45,sizeof(char),26U);});Cyc_Warn_impos(_tmp2C7,_tag_dyneither(_tmp44,sizeof(void*),0U));});});{
void*_tmp46=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp47=_tmp46;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp47)->tag == 0U){_LL1: _LL2:
# 144
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
({struct Cyc_List_List*_tmp2C8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);ctxt->curr_ns=_tmp2C8;});
goto _LL0;}else{_LL3: _LL4:
# 148
({void*_tmp48=0U;({struct _dyneither_ptr _tmp2C9=({const char*_tmp49="leaving using as namespace";_tag_dyneither(_tmp49,sizeof(char),27U);});Cyc_Warn_impos(_tmp2C9,_tag_dyneither(_tmp48,sizeof(void*),0U));});});}_LL0:;};}
# 151
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp4A=usename;union Cyc_Absyn_Nmspace _tmp58;struct _dyneither_ptr*_tmp57;_LL1: _tmp58=_tmp4A->f1;_tmp57=_tmp4A->f2;_LL2:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp4B=_tmp58;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp53;switch((_tmp4B.Loc_n).tag){case 1U: _LL4: _tmp53=(_tmp4B.Rel_n).val;_LL5:
# 156
 ns=({unsigned int _tmp2CC=loc;struct Cyc_Binding_NSCtxt*_tmp2CB=ctxt;Cyc_Binding_resolve_rel_ns(_tmp2CC,_tmp2CB,({struct Cyc_List_List*_tmp2CA=_tmp53;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2CA,({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=_tmp57,_tmp4C->tl=0;_tmp4C;}));}));});{
struct Cyc_List_List*_tmp4D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);
({union Cyc_Absyn_Nmspace _tmp2CD=Cyc_Absyn_Abs_n(_tmp4D,0);(*usename).f1=_tmp2CD;});
goto _LL3;};case 2U: _LL6: _tmp54=(_tmp4B.Abs_n).val;_LL7:
# 161
 ns=({struct Cyc_List_List*_tmp2CE=_tmp54;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2CE,({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->hd=_tmp57,_tmp4E->tl=0;_tmp4E;}));});
goto _LL3;case 4U: _LL8: _LL9:
({void*_tmp4F=0U;({struct _dyneither_ptr _tmp2CF=({const char*_tmp50="enter_using, Loc_n";_tag_dyneither(_tmp50,sizeof(char),19U);});Cyc_Warn_impos(_tmp2CF,_tag_dyneither(_tmp4F,sizeof(void*),0U));});});default: _LLA: _LLB:
({void*_tmp51=0U;({struct _dyneither_ptr _tmp2D0=({const char*_tmp52="enter_using, C_n";_tag_dyneither(_tmp52,sizeof(char),17U);});Cyc_Warn_impos(_tmp2D0,_tag_dyneither(_tmp51,sizeof(void*),0U));});});}_LL3:;}
# 166
({struct Cyc_List_List*_tmp2D2=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));({void*_tmp2D1=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=1U,_tmp55->f1=ns;_tmp55;});_tmp56->hd=_tmp2D1;}),_tmp56->tl=ctxt->availables;_tmp56;});ctxt->availables=_tmp2D2;});
return ns;};}
# 169
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp59=0U;({struct _dyneither_ptr _tmp2D3=({const char*_tmp5A="leaving topmost namespace as a using";_tag_dyneither(_tmp5A,sizeof(char),37U);});Cyc_Warn_impos(_tmp2D3,_tag_dyneither(_tmp59,sizeof(void*),0U));});});{
void*_tmp5B=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_tmp5C=_tmp5B;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp5C)->tag == 1U){_LL1: _LL2:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL0;}else{_LL3: _LL4:
({void*_tmp5D=0U;({struct _dyneither_ptr _tmp2D4=({const char*_tmp5E="leaving namespace as using";_tag_dyneither(_tmp5E,sizeof(char),27U);});Cyc_Warn_impos(_tmp2D4,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});}_LL0:;};}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 213 "binding.cyc"
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 216
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 219
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp5F=_cycalloc(sizeof(*_tmp5F));({struct Cyc_Dict_Dict _tmp2D9=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->aggrdecls=_tmp2D9;}),({
struct Cyc_Dict_Dict _tmp2D8=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->datatypedecls=_tmp2D8;}),({
struct Cyc_Dict_Dict _tmp2D7=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->enumdecls=_tmp2D7;}),({
struct Cyc_Dict_Dict _tmp2D6=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->typedefs=_tmp2D6;}),({
struct Cyc_Dict_Dict _tmp2D5=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp5F->ordinaries=_tmp2D5;});_tmp5F;});}
# 227
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->aggrdecls,v);}
# 230
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->datatypedecls,v);}
# 233
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->enumdecls,v);}
# 236
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->typedefs,v);}
# 239
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _dyneither_ptr*v){
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(renv->ordinaries,v);}
# 242
static void*Cyc_Binding_lookup_ordinary(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){
struct _tuple0*_tmp60=qv;union Cyc_Absyn_Nmspace _tmp64;struct _dyneither_ptr*_tmp63;_LL1: _tmp64=_tmp60->f1;_tmp63=_tmp60->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp61=_tmp64;switch((_tmp61.Rel_n).tag){case 4U: _LL4: _LL5:
# 246
 if(Cyc_Binding_at_toplevel(env) || !((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=Cyc_Binding_BindingError;_tmp62;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63);case 1U: if((_tmp61.Rel_n).val == 0){_LL6: _LL7:
# 250
 if(!Cyc_Binding_at_toplevel(env) && ((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63))
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),_tmp63);
goto _LL9;}else{goto _LL8;}default: _LL8: _LL9:
# 254
 return((void*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL3:;};}
# 258
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*,struct Cyc_Absyn_Decl*);
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*);
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*,struct Cyc_Absyn_Pat*);
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);
void Cyc_Binding_resolve_type(unsigned int,struct Cyc_Binding_Env*,void*);
void Cyc_Binding_resolve_rgnpo(unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_aggrfields(unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_function_stuff(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo i);
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int,unsigned int,struct Cyc_Binding_Env*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*);
# 273
void Cyc_Binding_absolutize_decl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp65=(*qv).f1;union Cyc_Absyn_Nmspace _tmp66=_tmp65;switch((_tmp66.Abs_n).tag){case 1U: if((_tmp66.Rel_n).val == 0){_LL1: _LL2:
# 276
 if(Cyc_Binding_at_toplevel(env))
({union Cyc_Absyn_Nmspace _tmp2DB=({struct Cyc_List_List*_tmp2DA=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp2DA,Cyc_Binding_in_cinclude(env) || (int)sc == (int)Cyc_Absyn_ExternC);});(*qv).f1=_tmp2DB;});else{
# 279
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL0;}else{goto _LL7;}case 4U: _LL3: _LL4:
 goto _LL0;case 2U: _LL5: _LL6:
# 283
 if(!Cyc_Binding_at_toplevel(env))
goto _LL8;
goto _LL0;default: _LL7: _LL8:
# 287
({struct Cyc_String_pa_PrintArg_struct _tmp69=({struct Cyc_String_pa_PrintArg_struct _tmp29F;_tmp29F.tag=0U,({
struct _dyneither_ptr _tmp2DC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(qv));_tmp29F.f1=_tmp2DC;});_tmp29F;});void*_tmp67[1U];_tmp67[0]=& _tmp69;({unsigned int _tmp2DE=loc;struct _dyneither_ptr _tmp2DD=({const char*_tmp68="qualified names in declarations unimplemented (%s)";_tag_dyneither(_tmp68,sizeof(char),51U);});Cyc_Warn_err(_tmp2DE,_tmp2DD,_tag_dyneither(_tmp67,sizeof(void*),1U));});});}_LL0:;}
# 294
void Cyc_Binding_absolutize_topdecl(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp6A=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=_tmp6A;}
# 301
void Cyc_Binding_check_warn_override(unsigned int loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _dyneither_ptr hides_what;
struct _handler_cons _tmp6B;_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(_tmp6B.handler))_tmp6D=1;if(!_tmp6D){
{void*_tmp6E=Cyc_Binding_lookup_ordinary(loc,env,q);void*_tmp6F=_tmp6E;void*_tmp7B;switch(*((int*)_tmp6F)){case 0U: _LL1: _tmp7B=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp6F)->f1;_LL2:
# 306
 if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0U);return;}
{void*_tmp70=_tmp7B;switch(*((int*)_tmp70)){case 1U: _LLC: _LLD:
 hides_what=({const char*_tmp71="global variable";_tag_dyneither(_tmp71,sizeof(char),16U);});goto _LLB;case 4U: _LLE: _LLF:
 hides_what=({const char*_tmp72="local variable";_tag_dyneither(_tmp72,sizeof(char),15U);});goto _LLB;case 3U: _LL10: _LL11:
 hides_what=({const char*_tmp73="parameter";_tag_dyneither(_tmp73,sizeof(char),10U);});goto _LLB;case 5U: _LL12: _LL13:
 hides_what=({const char*_tmp74="pattern variable";_tag_dyneither(_tmp74,sizeof(char),17U);});goto _LLB;case 2U: _LL14: _LL15:
 hides_what=({const char*_tmp75="function";_tag_dyneither(_tmp75,sizeof(char),9U);});goto _LLB;default: _LL16: _LL17:
({void*_tmp76=0U;({struct _dyneither_ptr _tmp2DF=({const char*_tmp77="shadowing free variable!";_tag_dyneither(_tmp77,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2DF,_tag_dyneither(_tmp76,sizeof(void*),0U));});});}_LLB:;}
# 316
goto _LL0;case 1U: _LL3: _LL4:
 hides_what=({const char*_tmp78="struct constructor";_tag_dyneither(_tmp78,sizeof(char),19U);});goto _LL0;case 2U: _LL5: _LL6:
 hides_what=({const char*_tmp79="datatype constructor";_tag_dyneither(_tmp79,sizeof(char),21U);});goto _LL0;case 3U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
 hides_what=({const char*_tmp7A="enum tag";_tag_dyneither(_tmp7A,sizeof(char),9U);});goto _LL0;}_LL0:;}
# 322
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp2A0;_tmp2A0.tag=0U,_tmp2A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)hides_what);_tmp2A0;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp2E1=loc;struct _dyneither_ptr _tmp2E0=({const char*_tmp7D="declaration hides %s";_tag_dyneither(_tmp7D,sizeof(char),21U);});Cyc_Warn_warn(_tmp2E1,_tmp2E0,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});
_npop_handler(0U);return;
# 304
;_pop_handler();}else{void*_tmp6C=(void*)_exn_thrown;void*_tmp7F=_tmp6C;void*_tmp80;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp7F)->tag == Cyc_Binding_BindingError){_LL19: _LL1A:
# 324
 return;}else{_LL1B: _tmp80=_tmp7F;_LL1C:(int)_rethrow(_tmp80);}_LL18:;}};}
# 327
void Cyc_Binding_resolve_and_add_vardecl(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp81=vd->name;struct _tuple0*_tmp82=_tmp81;union Cyc_Absyn_Nmspace _tmp89;struct _dyneither_ptr*_tmp88;_LL1: _tmp89=_tmp82->f1;_tmp88=_tmp82->f2;_LL2:;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _tmp2E5=({struct Cyc_Dict_Dict _tmp2E4=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp2E3=_tmp88;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp2E4,_tmp2E3,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp84=_cycalloc(sizeof(*_tmp84));
_tmp84->tag=0U,({void*_tmp2E2=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=4U,_tmp83->f1=vd;_tmp83;});_tmp84->f1=_tmp2E2;});_tmp84;}));});
# 334
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp2E5;});else{
# 337
struct Cyc_Binding_ResolveNSEnv*_tmp85=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp89);
({struct Cyc_Dict_Dict _tmp2E9=({struct Cyc_Dict_Dict _tmp2E8=_tmp85->ordinaries;struct _dyneither_ptr*_tmp2E7=_tmp88;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp2E8,_tmp2E7,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp87=_cycalloc(sizeof(*_tmp87));
# 340
_tmp87->tag=0U,({void*_tmp2E6=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=1U,_tmp86->f1=vd;_tmp86;});_tmp87->f1=_tmp2E6;});_tmp87;}));});
# 338
_tmp85->ordinaries=_tmp2E9;});}};}
# 344
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp8A=s->r;void*_tmp8B=_tmp8A;struct Cyc_Absyn_Stmt*_tmpA3;struct Cyc_List_List*_tmpA2;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_List_List*_tmpA0;struct Cyc_Absyn_Decl*_tmp9F;struct Cyc_Absyn_Stmt*_tmp9E;struct Cyc_Absyn_Stmt*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Stmt*_tmp98;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Stmt*_tmp96;struct Cyc_Absyn_Stmt*_tmp95;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Stmt*_tmp92;struct Cyc_Absyn_Stmt*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Stmt*_tmp8F;struct Cyc_Absyn_Stmt*_tmp8E;struct Cyc_Absyn_Exp*_tmp8D;switch(*((int*)_tmp8B)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp8D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_LL4:
 Cyc_Binding_resolve_exp(env,_tmp8D);return;case 2U: _LL5: _tmp8F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp8E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL6:
 Cyc_Binding_resolve_stmt(env,_tmp8F);Cyc_Binding_resolve_stmt(env,_tmp8E);return;case 3U: _LL7: _tmp90=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_LL8:
 if((unsigned int)_tmp90)Cyc_Binding_resolve_exp(env,_tmp90);return;case 4U: _LL9: _tmp93=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp92=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_tmp91=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8B)->f3;_LLA:
# 351
 Cyc_Binding_resolve_exp(env,_tmp93);Cyc_Binding_resolve_stmt(env,_tmp92);Cyc_Binding_resolve_stmt(env,_tmp91);return;case 14U: _LLB: _tmp95=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp94=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2).f1;_LLC:
 _tmp97=_tmp94;_tmp96=_tmp95;goto _LLE;case 5U: _LLD: _tmp97=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1).f1;_tmp96=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LLE:
 Cyc_Binding_resolve_exp(env,_tmp97);Cyc_Binding_resolve_stmt(env,_tmp96);return;case 9U: _LLF: _tmp9B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp9A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2).f1;_tmp99=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f3).f1;_tmp98=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8B)->f4;_LL10:
# 355
 Cyc_Binding_resolve_exp(env,_tmp9B);Cyc_Binding_resolve_exp(env,_tmp9A);Cyc_Binding_resolve_exp(env,_tmp99);
Cyc_Binding_resolve_stmt(env,_tmp98);
return;case 6U: _LL11: _LL12:
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp9C=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_LL18:
# 362
 for(0;_tmp9C != 0;_tmp9C=_tmp9C->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp9C->hd);}
return;case 13U: _LL19: _tmp9D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL1A:
 Cyc_Binding_resolve_stmt(env,_tmp9D);return;case 12U: _LL1B: _tmp9F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmp9E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL1C: {
# 367
struct Cyc_Dict_Dict _tmp8C=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_decl(env,_tmp9F);
Cyc_Binding_resolve_stmt(env,_tmp9E);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp8C;
return;}case 10U: _LL1D: _tmpA1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmpA0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL1E:
 Cyc_Binding_resolve_exp(env,_tmpA1);Cyc_Binding_resolve_scs(env,_tmpA0);return;default: _LL1F: _tmpA3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f1;_tmpA2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp8B)->f2;_LL20:
 Cyc_Binding_resolve_stmt(env,_tmpA3);Cyc_Binding_resolve_scs(env,_tmpA2);return;}_LL0:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 376
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpA4=e->r;void*_tmpA5=_tmpA4;void*_tmp12E;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_Datatypedecl*_tmp12B;struct Cyc_Absyn_Datatypefield*_tmp12A;struct Cyc_Absyn_Enumdecl*_tmp129;struct Cyc_Absyn_Enumfield*_tmp128;void*_tmp127;struct Cyc_Absyn_Enumfield*_tmp126;struct Cyc_List_List*_tmp125;void*_tmp124;void*_tmp123;void*_tmp122;struct Cyc_Absyn_Stmt*_tmp121;void*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;void**_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Vardecl*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;struct _tuple0**_tmpF6;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Aggrdecl**_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_List_List*_tmpF1;int*_tmpF0;void**_tmpEF;switch(*((int*)_tmpA5)){case 1U: _LL1: _tmpEF=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL2: {
# 379
void*_tmpA6=*_tmpEF;void*_tmpA7=_tmpA6;struct _tuple0*_tmpC0;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA7)->tag == 0U){_LL56: _tmpC0=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpA7)->f1;_LL57: {
# 381
struct _handler_cons _tmpA8;_push_handler(& _tmpA8);{int _tmpAA=0;if(setjmp(_tmpA8.handler))_tmpAA=1;if(!_tmpAA){{void*_tmpAB=Cyc_Binding_lookup_ordinary(e->loc,env,_tmpC0);void*_tmpAC=_tmpAB;struct Cyc_Absyn_Aggrdecl*_tmpBA;void*_tmpB9;struct Cyc_Absyn_Enumfield*_tmpB8;struct Cyc_Absyn_Enumdecl*_tmpB7;struct Cyc_Absyn_Enumfield*_tmpB6;struct Cyc_Absyn_Datatypedecl*_tmpB5;struct Cyc_Absyn_Datatypefield*_tmpB4;void*_tmpB3;switch(*((int*)_tmpAC)){case 0U: _LL5B: _tmpB3=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpAC)->f1;_LL5C:
*_tmpEF=_tmpB3;_npop_handler(0U);return;case 2U: _LL5D: _tmpB5=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpAC)->f1;_tmpB4=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpAC)->f2;_LL5E:
({void*_tmp2EA=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=31U,_tmpAD->f1=0,_tmpAD->f2=_tmpB5,_tmpAD->f3=_tmpB4;_tmpAD;});e->r=_tmp2EA;});_npop_handler(0U);return;case 3U: _LL5F: _tmpB7=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpAC)->f1;_tmpB6=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpAC)->f2;_LL60:
({void*_tmp2EB=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=32U,_tmpAE->f1=_tmpB7,_tmpAE->f2=_tmpB6;_tmpAE;});e->r=_tmp2EB;});_npop_handler(0U);return;case 4U: _LL61: _tmpB9=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpAC)->f1;_tmpB8=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpAC)->f2;_LL62:
({void*_tmp2EC=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=33U,_tmpAF->f1=_tmpB9,_tmpAF->f2=_tmpB8;_tmpAF;});e->r=_tmp2EC;});_npop_handler(0U);return;default: _LL63: _tmpBA=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpAC)->f1;_LL64:
# 387
({struct Cyc_String_pa_PrintArg_struct _tmpB2=({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0U,({struct _dyneither_ptr _tmp2ED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC0));_tmp2A1.f1=_tmp2ED;});_tmp2A1;});void*_tmpB0[1U];_tmpB0[0]=& _tmpB2;({unsigned int _tmp2EF=e->loc;struct _dyneither_ptr _tmp2EE=({const char*_tmpB1="bad occurrence of type name %s";_tag_dyneither(_tmpB1,sizeof(char),31U);});Cyc_Warn_err(_tmp2EF,_tmp2EE,_tag_dyneither(_tmpB0,sizeof(void*),1U));});});
_npop_handler(0U);return;}_LL5A:;}
# 381
;_pop_handler();}else{void*_tmpA9=(void*)_exn_thrown;void*_tmpBB=_tmpA9;void*_tmpBF;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpBB)->tag == Cyc_Binding_BindingError){_LL66: _LL67:
# 391
({struct Cyc_String_pa_PrintArg_struct _tmpBE=({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0U,({struct _dyneither_ptr _tmp2F0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC0));_tmp2A2.f1=_tmp2F0;});_tmp2A2;});void*_tmpBC[1U];_tmpBC[0]=& _tmpBE;({unsigned int _tmp2F2=e->loc;struct _dyneither_ptr _tmp2F1=({const char*_tmpBD="undeclared identifier %s";_tag_dyneither(_tmpBD,sizeof(char),25U);});Cyc_Warn_err(_tmp2F2,_tmp2F1,_tag_dyneither(_tmpBC,sizeof(void*),1U));});});
return;}else{_LL68: _tmpBF=_tmpBB;_LL69:(int)_rethrow(_tmpBF);}_LL65:;}};}}else{_LL58: _LL59:
# 394
 return;}_LL55:;}case 10U: _LL3: _tmpF2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpF1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmpF0=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA5)->f4;_LL4:
# 397
*_tmpF0=1;
# 399
{struct Cyc_List_List*_tmpC1=_tmpF1;for(0;_tmpC1 != 0;_tmpC1=_tmpC1->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpC1->hd);}}{
void*_tmpC2=_tmpF2->r;void*_tmpC3=_tmpC2;void**_tmpE0;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC3)->tag == 1U){_LL6B: _tmpE0=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC3)->f1;_LL6C: {
# 403
void*_tmpC4=*_tmpE0;void*_tmpC5=_tmpC4;struct _tuple0*_tmpDF;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC5)->tag == 0U){_LL70: _tmpDF=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC5)->f1;_LL71: {
# 405
struct _handler_cons _tmpC6;_push_handler(& _tmpC6);{int _tmpC8=0;if(setjmp(_tmpC6.handler))_tmpC8=1;if(!_tmpC8){{void*_tmpC9=Cyc_Binding_lookup_ordinary(_tmpF2->loc,env,_tmpDF);void*_tmpCA=_tmpC9;struct Cyc_Absyn_Datatypedecl*_tmpD9;struct Cyc_Absyn_Datatypefield*_tmpD8;struct Cyc_Absyn_Aggrdecl*_tmpD7;void*_tmpD6;switch(*((int*)_tmpCA)){case 0U: _LL75: _tmpD6=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpCA)->f1;_LL76:
# 407
*_tmpE0=_tmpD6;_npop_handler(0U);return;case 1U: _LL77: _tmpD7=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpCA)->f1;_LL78: {
# 409
struct Cyc_List_List*_tmpCB=0;
for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
_tmpCB=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));({struct _tuple10*_tmp2F3=({struct _tuple10*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->f1=0,_tmpCC->f2=(struct Cyc_Absyn_Exp*)_tmpF1->hd;_tmpCC;});_tmpCD->hd=_tmp2F3;}),_tmpCD->tl=_tmpCB;_tmpCD;});}
({void*_tmp2F5=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->tag=29U,_tmpCE->f1=_tmpD7->name,_tmpCE->f2=0,({struct Cyc_List_List*_tmp2F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCB);_tmpCE->f3=_tmp2F4;}),_tmpCE->f4=_tmpD7;_tmpCE;});e->r=_tmp2F5;});
_npop_handler(0U);return;}case 2U: _LL79: _tmpD9=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpCA)->f1;_tmpD8=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpCA)->f2;_LL7A:
# 415
 if(_tmpD8->typs == 0)
# 417
({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=0U,({
struct _dyneither_ptr _tmp2F6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD8->name));_tmp2A3.f1=_tmp2F6;});_tmp2A3;});void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({unsigned int _tmp2F8=e->loc;struct _dyneither_ptr _tmp2F7=({const char*_tmpD0="%s is a constant, not a function";_tag_dyneither(_tmpD0,sizeof(char),33U);});Cyc_Warn_err(_tmp2F8,_tmp2F7,_tag_dyneither(_tmpCF,sizeof(void*),1U));});});
({void*_tmp2F9=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->tag=31U,_tmpD2->f1=_tmpF1,_tmpD2->f2=_tmpD9,_tmpD2->f3=_tmpD8;_tmpD2;});e->r=_tmp2F9;});
_npop_handler(0U);return;case 4U: _LL7B: _LL7C:
 goto _LL7E;default: _LL7D: _LL7E:
# 423
({struct Cyc_String_pa_PrintArg_struct _tmpD5=({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0U,({
struct _dyneither_ptr _tmp2FA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpDF));_tmp2A4.f1=_tmp2FA;});_tmp2A4;});void*_tmpD3[1U];_tmpD3[0]=& _tmpD5;({unsigned int _tmp2FC=e->loc;struct _dyneither_ptr _tmp2FB=({const char*_tmpD4="%s is an enum constructor, not a function";_tag_dyneither(_tmpD4,sizeof(char),42U);});Cyc_Warn_err(_tmp2FC,_tmp2FB,_tag_dyneither(_tmpD3,sizeof(void*),1U));});});
_npop_handler(0U);return;}_LL74:;}
# 405
;_pop_handler();}else{void*_tmpC7=(void*)_exn_thrown;void*_tmpDA=_tmpC7;void*_tmpDE;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpDA)->tag == Cyc_Binding_BindingError){_LL80: _LL81:
# 427
({struct Cyc_String_pa_PrintArg_struct _tmpDD=({struct Cyc_String_pa_PrintArg_struct _tmp2A5;_tmp2A5.tag=0U,({struct _dyneither_ptr _tmp2FD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpDF));_tmp2A5.f1=_tmp2FD;});_tmp2A5;});void*_tmpDB[1U];_tmpDB[0]=& _tmpDD;({unsigned int _tmp2FF=e->loc;struct _dyneither_ptr _tmp2FE=({const char*_tmpDC="undeclared identifier %s";_tag_dyneither(_tmpDC,sizeof(char),25U);});Cyc_Warn_err(_tmp2FF,_tmp2FE,_tag_dyneither(_tmpDB,sizeof(void*),1U));});});
return;}else{_LL82: _tmpDE=_tmpDA;_LL83:(int)_rethrow(_tmpDE);}_LL7F:;}};}}else{_LL72: _LL73:
# 430
 return;}_LL6F:;}}else{_LL6D: _LL6E:
# 432
 Cyc_Binding_resolve_exp(env,_tmpF2);return;}_LL6A:;};case 29U: _LL5: _tmpF6=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpF5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmpF4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_tmpF3=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA5)->f4;_LL6:
# 436
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpF4->hd)).f2);}{
struct _handler_cons _tmpE1;_push_handler(& _tmpE1);{int _tmpE3=0;if(setjmp(_tmpE1.handler))_tmpE3=1;if(!_tmpE3){
({struct Cyc_Absyn_Aggrdecl*_tmp300=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*_tmpF6,Cyc_Binding_lookup_aggrdecl);*_tmpF3=_tmp300;});
*_tmpF6=((struct Cyc_Absyn_Aggrdecl*)_check_null(*_tmpF3))->name;
_npop_handler(0U);return;
# 439
;_pop_handler();}else{void*_tmpE2=(void*)_exn_thrown;void*_tmpE4=_tmpE2;void*_tmpE8;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmpE4)->tag == Cyc_Binding_BindingError){_LL85: _LL86:
# 443
({struct Cyc_String_pa_PrintArg_struct _tmpE7=({struct Cyc_String_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=0U,({struct _dyneither_ptr _tmp301=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmpF6));_tmp2A6.f1=_tmp301;});_tmp2A6;});void*_tmpE5[1U];_tmpE5[0]=& _tmpE7;({unsigned int _tmp303=e->loc;struct _dyneither_ptr _tmp302=({const char*_tmpE6="unbound struct/union name %s";_tag_dyneither(_tmpE6,sizeof(char),29U);});Cyc_Warn_err(_tmp303,_tmp302,_tag_dyneither(_tmpE5,sizeof(void*),1U));});});
return;}else{_LL87: _tmpE8=_tmpE4;_LL88:(int)_rethrow(_tmpE8);}_LL84:;}};};case 27U: _LL7: _tmpF9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpF8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmpF7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL8:
# 448
 Cyc_Binding_resolve_exp(env,_tmpF8);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_tmp305=({struct Cyc_Dict_Dict*_tmpE9=_cycalloc(sizeof(*_tmpE9));({struct Cyc_Dict_Dict _tmp304=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmpE9=_tmp304;});_tmpE9;});env->local_vars=_tmp305;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmpF9);
Cyc_Binding_resolve_exp(env,_tmpF7);
env->local_vars=0;
return;}{
# 456
struct Cyc_Dict_Dict _tmpEA=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,_tmpF9);
Cyc_Binding_resolve_exp(env,_tmpF7);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmpEA;
return;};case 36U: _LL9: _tmpFA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LLA:
# 464
 for(0;_tmpFA != 0;_tmpFA=_tmpFA->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmpFA->hd)).f2);}
return;case 28U: _LLB: _tmpFC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmpFB=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LLC:
# 470
 Cyc_Binding_resolve_exp(env,_tmpFC);
Cyc_Binding_resolve_type(e->loc,env,_tmpFB);
return;case 2U: _LLD: _LLE:
# 474
 goto _LL10;case 0U: _LLF: _LL10:
 return;case 24U: _LL11: _tmpFD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL12:
# 477
 _tmpFE=_tmpFD;goto _LL14;case 3U: _LL13: _tmpFE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL14:
# 479
 for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmpFE->hd);}
return;case 41U: _LL15: _tmpFF=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL16:
# 483
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
# 494
 _tmp10C=_tmp10A;_tmp10B=_tmp109;goto _LL2C;case 9U: _LL2B: _tmp10C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp10B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL2C:
 _tmp10E=_tmp10C;_tmp10D=_tmp10B;goto _LL2E;case 4U: _LL2D: _tmp10E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp10D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL2E:
 _tmp110=_tmp10E;_tmp10F=_tmp10D;goto _LL30;case 23U: _LL2F: _tmp110=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp10F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL30:
 _tmp112=_tmp110;_tmp111=_tmp10F;goto _LL32;case 7U: _LL31: _tmp112=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp111=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL32:
 _tmp114=_tmp112;_tmp113=_tmp111;goto _LL34;case 8U: _LL33: _tmp114=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp113=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL34:
 Cyc_Binding_resolve_exp(env,_tmp114);Cyc_Binding_resolve_exp(env,_tmp113);return;case 34U: _LL35: _tmp117=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).rgn;_tmp116=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).elt_type;_tmp115=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).num_elts;_LL36:
# 502
 if(_tmp117 != 0)Cyc_Binding_resolve_exp(env,_tmp117);
if(_tmp116 != 0)Cyc_Binding_resolve_type(e->loc,env,*_tmp116);
Cyc_Binding_resolve_exp(env,_tmp115);
return;case 16U: _LL37: _tmp119=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp118=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL38:
# 508
 if(_tmp119 != 0)Cyc_Binding_resolve_exp(env,_tmp119);
Cyc_Binding_resolve_exp(env,_tmp118);
return;case 6U: _LL39: _tmp11C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp11B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmp11A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL3A:
# 513
 Cyc_Binding_resolve_exp(env,_tmp11C);Cyc_Binding_resolve_exp(env,_tmp11B);Cyc_Binding_resolve_exp(env,_tmp11A);return;case 13U: _LL3B: _tmp11E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp11D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL3C:
# 516
 Cyc_Binding_resolve_exp(env,_tmp11E);
for(0;_tmp11D != 0;_tmp11D=_tmp11D->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)_tmp11D->hd);}
return;case 14U: _LL3D: _tmp120=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp11F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL3E:
# 522
 Cyc_Binding_resolve_exp(env,_tmp11F);Cyc_Binding_resolve_type(e->loc,env,_tmp120);return;case 37U: _LL3F: _tmp121=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL40:
# 524
 Cyc_Binding_resolve_stmt(env,_tmp121);return;case 39U: _LL41: _tmp122=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL42:
# 526
 _tmp123=_tmp122;goto _LL44;case 19U: _LL43: _tmp123=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL44:
 _tmp124=_tmp123;goto _LL46;case 17U: _LL45: _tmp124=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL46:
 Cyc_Binding_resolve_type(e->loc,env,_tmp124);return;case 26U: _LL47: _tmp125=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL48:
# 531
 for(0;_tmp125 != 0;_tmp125=_tmp125->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp125->hd)).f2);}
return;case 25U: _LL49: _LL4A:
# 536
({void*_tmpEB=0U;({unsigned int _tmp307=e->loc;struct _dyneither_ptr _tmp306=({const char*_tmpEC="binding: compound literals unimplemented";_tag_dyneither(_tmpEC,sizeof(char),41U);});Cyc_Warn_err(_tmp307,_tmp306,_tag_dyneither(_tmpEB,sizeof(void*),0U));});});return;case 33U: _LL4B: _tmp127=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp126=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL4C:
# 539
 Cyc_Binding_resolve_type(e->loc,env,_tmp127);return;case 32U: _LL4D: _tmp129=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp128=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL4E:
 return;case 31U: _LL4F: _tmp12C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp12B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_tmp12A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA5)->f3;_LL50:
# 542
 for(0;_tmp12C != 0;_tmp12C=_tmp12C->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_tmp12C->hd);}
return;case 30U: _LL51: _tmp12E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_tmp12D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL52:
# 546
 Cyc_Binding_resolve_type(e->loc,env,_tmp12E);
for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple10*)_tmp12D->hd)).f2);}
return;default: _LL53: _LL54:
# 552
({void*_tmpED=0U;({unsigned int _tmp309=e->loc;struct _dyneither_ptr _tmp308=({const char*_tmpEE="asm expressions cannot occur within Cyclone code";_tag_dyneither(_tmpEE,sizeof(char),49U);});Cyc_Warn_err(_tmp309,_tmp308,_tag_dyneither(_tmpED,sizeof(void*),0U));});});}_LL0:;}
# 556
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp12F=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp130=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp131=_tmp130;struct Cyc_Absyn_Pat*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Stmt*_tmp132;_LL1: _tmp134=_tmp131->pattern;_tmp133=_tmp131->where_clause;_tmp132=_tmp131->body;_LL2:;
Cyc_Binding_resolve_pat(env,_tmp134);
if(_tmp133 != 0)
Cyc_Binding_resolve_exp(env,_tmp133);
Cyc_Binding_resolve_stmt(env,_tmp132);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp12F;}
# 566
return;}
# 568
void Cyc_Binding_resolve_aggrfields(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp135=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_tmp136=_tmp135;struct _dyneither_ptr*_tmp13C;struct Cyc_Absyn_Tqual _tmp13B;void*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Exp*_tmp137;_LL1: _tmp13C=_tmp136->name;_tmp13B=_tmp136->tq;_tmp13A=_tmp136->type;_tmp139=_tmp136->width;_tmp138=_tmp136->attributes;_tmp137=_tmp136->requires_clause;_LL2:;
Cyc_Binding_resolve_type(loc,env,_tmp13A);
if(_tmp139 != 0)
Cyc_Binding_resolve_exp(env,_tmp139);
if(_tmp137 != 0)
Cyc_Binding_resolve_exp(env,_tmp137);}
# 577
return;}
# 580
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int need_va_name,unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 586
struct Cyc_List_List*_tmp13D=0;
for(0;args != 0;args=args->tl){
struct _tuple8*_tmp13E=(struct _tuple8*)args->hd;struct _tuple8*_tmp13F=_tmp13E;struct _dyneither_ptr*_tmp145;struct Cyc_Absyn_Tqual _tmp144;void*_tmp143;_LL1: _tmp145=_tmp13F->f1;_tmp144=_tmp13F->f2;_tmp143=_tmp13F->f3;_LL2:;
if(_tmp145 == 0)
continue;{
struct Cyc_Absyn_Vardecl*_tmp140=({struct _tuple0*_tmp30A=({struct _tuple0*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->f1=Cyc_Absyn_Loc_n,_tmp142->f2=_tmp145;_tmp142;});Cyc_Absyn_new_vardecl(0U,_tmp30A,_tmp143,0);});
_tmp140->tq=_tmp144;
_tmp13D=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->hd=_tmp140,_tmp141->tl=_tmp13D;_tmp141;});};}
# 595
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp146=*vai;struct Cyc_Absyn_VarargInfo _tmp147=_tmp146;struct _dyneither_ptr*_tmp151;struct Cyc_Absyn_Tqual _tmp150;void*_tmp14F;int _tmp14E;_LL4: _tmp151=_tmp147.name;_tmp150=_tmp147.tq;_tmp14F=_tmp147.type;_tmp14E=_tmp147.inject;_LL5:;
if(_tmp151 == 0){
if(need_va_name)
({void*_tmp148=0U;({unsigned int _tmp30C=loc;struct _dyneither_ptr _tmp30B=({const char*_tmp149="missing name for vararg";_tag_dyneither(_tmp149,sizeof(char),24U);});Cyc_Warn_err(_tmp30C,_tmp30B,_tag_dyneither(_tmp148,sizeof(void*),0U));});});}else{
# 602
void*_tmp14A=Cyc_Absyn_fatptr_type(_tmp14F,Cyc_Absyn_heap_rgn_type,_tmp150,Cyc_Absyn_false_type);
struct Cyc_Absyn_Vardecl*_tmp14B=({struct _tuple0*_tmp30D=({struct _tuple0*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->f1=Cyc_Absyn_Loc_n,_tmp14D->f2=_tmp151;_tmp14D;});Cyc_Absyn_new_vardecl(0U,_tmp30D,_tmp14A,0);});
_tmp13D=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=_tmp14B,_tmp14C->tl=_tmp13D;_tmp14C;});}}
# 607
_tmp13D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13D);
return _tmp13D;}
# 611
void Cyc_Binding_resolve_function_stuff(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo i){
if(i.effect != 0)
Cyc_Binding_resolve_type(loc,env,i.effect);
Cyc_Binding_resolve_type(loc,env,i.ret_type);
{struct Cyc_List_List*_tmp152=i.args;for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple8*)_tmp152->hd)).f3);}}
if(i.cyc_varargs != 0)
Cyc_Binding_resolve_type(loc,env,(i.cyc_varargs)->type);
Cyc_Binding_resolve_rgnpo(loc,env,i.rgn_po);
if(i.requires_clause != 0)
Cyc_Binding_resolve_exp(env,i.requires_clause);
if(i.ensures_clause != 0){
# 624
struct Cyc_Dict_Dict _tmp153=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));
struct _dyneither_ptr*_tmp154=({struct _dyneither_ptr*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct _dyneither_ptr _tmp30E=({const char*_tmp159="return_value";_tag_dyneither(_tmp159,sizeof(char),13U);});*_tmp15A=_tmp30E;});_tmp15A;});
struct Cyc_Absyn_Vardecl*_tmp155=({struct _tuple0*_tmp30F=({struct _tuple0*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->f1=Cyc_Absyn_Loc_n,_tmp158->f2=_tmp154;_tmp158;});Cyc_Absyn_new_vardecl(0U,_tmp30F,i.ret_type,0);});
({struct Cyc_Dict_Dict _tmp313=({struct Cyc_Dict_Dict _tmp312=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp311=_tmp154;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp312,_tmp311,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp157=_cycalloc(sizeof(*_tmp157));
_tmp157->tag=0U,({void*_tmp310=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->tag=4U,_tmp156->f1=_tmp155;_tmp156;});_tmp157->f1=_tmp310;});_tmp157;}));});
# 627
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp313;});
# 629
Cyc_Binding_resolve_exp(env,i.ensures_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp153;}}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 639
void Cyc_Binding_resolve_type(unsigned int loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp15B=t;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_FnInfo _tmp1C9;void*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_List_List*_tmp1C6;void*_tmp1C5;void*_tmp1C4;void*_tmp1C3;void*_tmp1C2;struct Cyc_List_List*_tmp1C1;struct Cyc_Absyn_TypeDecl*_tmp1C0;void***_tmp1BF;struct _tuple0**_tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Typedefdecl**_tmp1BC;void*_tmp1BB;struct Cyc_List_List*_tmp1BA;struct Cyc_List_List*_tmp1B9;struct _tuple0**_tmp1B8;struct Cyc_Absyn_Enumdecl*_tmp1B7;union Cyc_Absyn_AggrInfo*_tmp1B6;struct Cyc_List_List*_tmp1B5;union Cyc_Absyn_DatatypeFieldInfo*_tmp1B4;struct Cyc_List_List*_tmp1B3;union Cyc_Absyn_DatatypeInfo*_tmp1B2;struct Cyc_List_List*_tmp1B1;switch(*((int*)_tmp15B)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1)){case 18U: _LL1: _tmp1B2=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1)->f1;_tmp1B1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f2;_LL2:
# 642
 for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B1->hd);}{
union Cyc_Absyn_DatatypeInfo _tmp15C=*_tmp1B2;union Cyc_Absyn_DatatypeInfo _tmp15D=_tmp15C;struct _tuple0*_tmp165;int _tmp164;if((_tmp15D.UnknownDatatype).tag == 1){_LL26: _tmp165=((_tmp15D.UnknownDatatype).val).name;_tmp164=((_tmp15D.UnknownDatatype).val).is_extensible;_LL27: {
# 646
struct _handler_cons _tmp15E;_push_handler(& _tmp15E);{int _tmp160=0;if(setjmp(_tmp15E.handler))_tmp160=1;if(!_tmp160){
{struct Cyc_Absyn_Datatypedecl*_tmp161=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp165,Cyc_Binding_lookup_datatypedecl))->hd;
({union Cyc_Absyn_DatatypeInfo _tmp314=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp2A7;_tmp2A7.name=_tmp161->name,_tmp2A7.is_extensible=_tmp164;_tmp2A7;}));*_tmp1B2=_tmp314;});
_npop_handler(0U);return;}
# 647
;_pop_handler();}else{void*_tmp15F=(void*)_exn_thrown;void*_tmp162=_tmp15F;void*_tmp163;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp162)->tag == Cyc_Binding_BindingError){_LL2B: _LL2C:
# 651
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp165,Cyc_Absyn_Public);return;}else{_LL2D: _tmp163=_tmp162;_LL2E:(int)_rethrow(_tmp163);}_LL2A:;}};}}else{_LL28: _LL29:
 return;}_LL25:;};case 19U: _LL3: _tmp1B4=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1)->f1;_tmp1B3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f2;_LL4:
# 655
 for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B3->hd);}{
union Cyc_Absyn_DatatypeFieldInfo _tmp166=*_tmp1B4;union Cyc_Absyn_DatatypeFieldInfo _tmp167=_tmp166;struct _tuple0*_tmp17C;struct _tuple0*_tmp17B;int _tmp17A;if((_tmp167.UnknownDatatypefield).tag == 1){_LL30: _tmp17C=((_tmp167.UnknownDatatypefield).val).datatype_name;_tmp17B=((_tmp167.UnknownDatatypefield).val).field_name;_tmp17A=((_tmp167.UnknownDatatypefield).val).is_extensible;_LL31:
# 661
{struct _handler_cons _tmp168;_push_handler(& _tmp168);{int _tmp16A=0;if(setjmp(_tmp168.handler))_tmp16A=1;if(!_tmp16A){
{void*_tmp16B=Cyc_Binding_lookup_ordinary(loc,env,_tmp17B);void*_tmp16C=_tmp16B;struct Cyc_Absyn_Datatypedecl*_tmp174;struct Cyc_Absyn_Datatypefield*_tmp173;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16C)->tag == 2U){_LL35: _tmp174=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16C)->f1;_tmp173=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp16C)->f2;_LL36: {
# 664
struct Cyc_Absyn_Datatypedecl*_tmp16D=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp17C,Cyc_Binding_lookup_datatypedecl))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp174->name,_tmp16D->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp170=({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0U,({
struct _dyneither_ptr _tmp315=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp173->name));_tmp2AA.f1=_tmp315;});_tmp2AA;});struct Cyc_String_pa_PrintArg_struct _tmp171=({struct Cyc_String_pa_PrintArg_struct _tmp2A9;_tmp2A9.tag=0U,({
struct _dyneither_ptr _tmp316=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp16D->name));_tmp2A9.f1=_tmp316;});_tmp2A9;});struct Cyc_String_pa_PrintArg_struct _tmp172=({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0U,({
struct _dyneither_ptr _tmp317=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp174->name));_tmp2A8.f1=_tmp317;});_tmp2A8;});void*_tmp16E[3U];_tmp16E[0]=& _tmp170,_tmp16E[1]=& _tmp171,_tmp16E[2]=& _tmp172;({unsigned int _tmp319=loc;struct _dyneither_ptr _tmp318=({const char*_tmp16F="%s is a variant of %s, not %s (shadowing not yet implemented properly)";_tag_dyneither(_tmp16F,sizeof(char),71U);});Cyc_Warn_err(_tmp319,_tmp318,_tag_dyneither(_tmp16E,sizeof(void*),3U));});});
_npop_handler(0U);return;}
# 672
({union Cyc_Absyn_DatatypeFieldInfo _tmp31A=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp2AB;_tmp2AB.datatype_name=_tmp174->name,_tmp2AB.field_name=_tmp173->name,_tmp2AB.is_extensible=_tmp17A;_tmp2AB;}));*_tmp1B4=_tmp31A;});
# 674
_npop_handler(0U);return;}}else{_LL37: _LL38:
 goto _LL34;}_LL34:;}
# 662
;_pop_handler();}else{void*_tmp169=(void*)_exn_thrown;void*_tmp175=_tmp169;void*_tmp176;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp175)->tag == Cyc_Binding_BindingError){_LL3A: _LL3B:
# 677
 goto _LL39;}else{_LL3C: _tmp176=_tmp175;_LL3D:(int)_rethrow(_tmp176);}_LL39:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0U,({struct _dyneither_ptr _tmp31B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp17B));_tmp2AC.f1=_tmp31B;});_tmp2AC;});void*_tmp177[1U];_tmp177[0]=& _tmp179;({unsigned int _tmp31D=loc;struct _dyneither_ptr _tmp31C=({const char*_tmp178="%s is not a datatype field";_tag_dyneither(_tmp178,sizeof(char),27U);});Cyc_Warn_err(_tmp31D,_tmp31C,_tag_dyneither(_tmp177,sizeof(void*),1U));});});
return;}else{_LL32: _LL33:
 return;}_LL2F:;};case 20U: _LL5: _tmp1B6=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1)->f1;_tmp1B5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f2;_LL6:
# 684
 for(0;_tmp1B5 != 0;_tmp1B5=_tmp1B5->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1B5->hd);}{
union Cyc_Absyn_AggrInfo _tmp17D=*_tmp1B6;union Cyc_Absyn_AggrInfo _tmp17E=_tmp17D;enum Cyc_Absyn_AggrKind _tmp18B;struct _tuple0*_tmp18A;struct Cyc_Core_Opt*_tmp189;if((_tmp17E.UnknownAggr).tag == 1){_LL3F: _tmp18B=((_tmp17E.UnknownAggr).val).f1;_tmp18A=((_tmp17E.UnknownAggr).val).f2;_tmp189=((_tmp17E.UnknownAggr).val).f3;_LL40: {
# 688
struct _handler_cons _tmp17F;_push_handler(& _tmp17F);{int _tmp181=0;if(setjmp(_tmp17F.handler))_tmp181=1;if(!_tmp181){
{struct Cyc_Absyn_Aggrdecl*_tmp182=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp18A,Cyc_Binding_lookup_aggrdecl);
if((int)_tmp182->kind != (int)_tmp18B)
({void*_tmp183=0U;({unsigned int _tmp31F=loc;struct _dyneither_ptr _tmp31E=({const char*_tmp184="struct vs. union mismatch with earlier declaration";_tag_dyneither(_tmp184,sizeof(char),51U);});Cyc_Warn_err(_tmp31F,_tmp31E,_tag_dyneither(_tmp183,sizeof(void*),0U));});});
if((((int)_tmp18B == (int)1U  && _tmp189 != 0) && _tmp182->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp182->impl))->tagged != (int)_tmp189->v)
# 694
({void*_tmp185=0U;({unsigned int _tmp321=loc;struct _dyneither_ptr _tmp320=({const char*_tmp186="@tagged mismatch with earlier declaration";_tag_dyneither(_tmp186,sizeof(char),42U);});Cyc_Warn_err(_tmp321,_tmp320,_tag_dyneither(_tmp185,sizeof(void*),0U));});});
({union Cyc_Absyn_AggrInfo _tmp322=Cyc_Absyn_UnknownAggr(_tmp18B,_tmp182->name,_tmp189);*_tmp1B6=_tmp322;});
_npop_handler(0U);return;}
# 689
;_pop_handler();}else{void*_tmp180=(void*)_exn_thrown;void*_tmp187=_tmp180;void*_tmp188;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp187)->tag == Cyc_Binding_BindingError){_LL44: _LL45:
# 698
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp18A,Cyc_Absyn_Public);return;}else{_LL46: _tmp188=_tmp187;_LL47:(int)_rethrow(_tmp188);}_LL43:;}};}}else{_LL41: _LL42:
 return;}_LL3E:;};case 15U: _LL7: _tmp1B8=(struct _tuple0**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1)->f1;_tmp1B7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1)->f2;_LL8:
# 702
 if(_tmp1B7 != 0)
return;{
struct _handler_cons _tmp18C;_push_handler(& _tmp18C);{int _tmp18E=0;if(setjmp(_tmp18C.handler))_tmp18E=1;if(!_tmp18E){
{struct Cyc_Absyn_Enumdecl*_tmp18F=((struct Cyc_Absyn_Enumdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1B8,Cyc_Binding_lookup_enumdecl);
*_tmp1B8=_tmp18F->name;
_npop_handler(0U);return;}
# 705
;_pop_handler();}else{void*_tmp18D=(void*)_exn_thrown;void*_tmp190=_tmp18D;void*_tmp191;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp190)->tag == Cyc_Binding_BindingError){_LL49: _LL4A:
# 709
 Cyc_Binding_absolutize_topdecl(loc,env,*_tmp1B8,Cyc_Absyn_Public);return;}else{_LL4B: _tmp191=_tmp190;_LL4C:(int)_rethrow(_tmp191);}_LL48:;}};};case 16U: _LLF: _tmp1B9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1)->f1;_LL10: {
# 747
struct Cyc_Binding_ResolveNSEnv*_tmp1A7=({struct Cyc_Binding_NSCtxt*_tmp324=env->ns;((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(_tmp324,({struct Cyc_List_List*_tmp323=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp323,
Cyc_Binding_in_cinclude(env));}));});
for(0;_tmp1B9 != 0;_tmp1B9=_tmp1B9->tl){
struct Cyc_Absyn_Enumfield*_tmp1A8=(struct Cyc_Absyn_Enumfield*)_tmp1B9->hd;
if(_tmp1A8->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp1A8->tag));
({struct Cyc_Dict_Dict _tmp327=({struct Cyc_Dict_Dict _tmp326=_tmp1A7->ordinaries;struct _dyneither_ptr*_tmp325=(*_tmp1A8->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp326,_tmp325,(void*)({struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->tag=4U,_tmp1A9->f1=t,_tmp1A9->f2=_tmp1A8;_tmp1A9;}));});_tmp1A7->ordinaries=_tmp327;});}
# 755
return;}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f2 == 0){_LL15: _LL16:
# 759
 return;}else{_LL17: _tmp1BB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1;_tmp1BA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f2;_LL18:
# 761
 for(0;_tmp1BA != 0;_tmp1BA=_tmp1BA->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1BA->hd);}
return;}}case 8U: _LL9: _tmp1BE=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15B)->f1;_tmp1BD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15B)->f2;_tmp1BC=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15B)->f3;_LLA:
# 712
 for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){
Cyc_Binding_resolve_type(loc,env,(void*)_tmp1BD->hd);}
{struct _handler_cons _tmp192;_push_handler(& _tmp192);{int _tmp194=0;if(setjmp(_tmp192.handler))_tmp194=1;if(!_tmp194){
{struct Cyc_Absyn_Typedefdecl*_tmp195=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*_tmp1BE,Cyc_Binding_lookup_typedefdecl);
# 717
*_tmp1BE=_tmp195->name;
_npop_handler(0U);return;}
# 715
;_pop_handler();}else{void*_tmp193=(void*)_exn_thrown;void*_tmp196=_tmp193;void*_tmp197;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp196)->tag == Cyc_Binding_BindingError){_LL4E: _LL4F:
# 719
 goto _LL4D;}else{_LL50: _tmp197=_tmp196;_LL51:(int)_rethrow(_tmp197);}_LL4D:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp19A=({struct Cyc_String_pa_PrintArg_struct _tmp2AD;_tmp2AD.tag=0U,({struct _dyneither_ptr _tmp328=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*_tmp1BE));_tmp2AD.f1=_tmp328;});_tmp2AD;});void*_tmp198[1U];_tmp198[0]=& _tmp19A;({unsigned int _tmp32A=loc;struct _dyneither_ptr _tmp329=({const char*_tmp199="unbound typdef name %s";_tag_dyneither(_tmp199,sizeof(char),23U);});Cyc_Warn_err(_tmp32A,_tmp329,_tag_dyneither(_tmp198,sizeof(void*),1U));});});
return;case 10U: _LLB: _tmp1C0=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15B)->f1;_tmp1BF=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15B)->f2;_LLC: {
# 725
struct Cyc_Dict_Dict*_tmp19B=env->local_vars;
env->local_vars=0;
{void*_tmp19C=_tmp1C0->r;void*_tmp19D=_tmp19C;struct Cyc_Absyn_Datatypedecl*_tmp1A6;struct Cyc_Absyn_Enumdecl*_tmp1A5;struct Cyc_Absyn_Aggrdecl*_tmp1A4;switch(*((int*)_tmp19D)){case 0U: _LL53: _tmp1A4=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp19D)->f1;_LL54:
# 729
({struct Cyc_Binding_Env*_tmp32C=env;Cyc_Binding_resolve_decl(_tmp32C,({struct Cyc_Absyn_Decl*_tmp19F=_cycalloc(sizeof(*_tmp19F));({void*_tmp32B=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=5U,_tmp19E->f1=_tmp1A4;_tmp19E;});_tmp19F->r=_tmp32B;}),_tmp19F->loc=_tmp1C0->loc;_tmp19F;}));});goto _LL52;case 1U: _LL55: _tmp1A5=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp19D)->f1;_LL56:
# 731
({struct Cyc_Binding_Env*_tmp32E=env;Cyc_Binding_resolve_decl(_tmp32E,({struct Cyc_Absyn_Decl*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));({void*_tmp32D=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->tag=7U,_tmp1A0->f1=_tmp1A5;_tmp1A0;});_tmp1A1->r=_tmp32D;}),_tmp1A1->loc=_tmp1C0->loc;_tmp1A1;}));});goto _LL52;default: _LL57: _tmp1A6=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp19D)->f1;_LL58:
# 733
({struct Cyc_Binding_Env*_tmp330=env;Cyc_Binding_resolve_decl(_tmp330,({struct Cyc_Absyn_Decl*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));({void*_tmp32F=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->tag=6U,_tmp1A2->f1=_tmp1A6;_tmp1A2;});_tmp1A3->r=_tmp32F;}),_tmp1A3->loc=_tmp1C0->loc;_tmp1A3;}));});goto _LL52;}_LL52:;}
# 735
env->local_vars=_tmp19B;
return;}case 7U: _LLD: _tmp1C1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp15B)->f2;_LLE:
# 742
 Cyc_Binding_resolve_aggrfields(loc,env,_tmp1C1);
return;case 1U: _LL11: _tmp1C2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15B)->f2;_LL12:
# 756
 if(_tmp1C2 != 0)Cyc_Binding_resolve_type(loc,env,_tmp1C2);return;case 2U: _LL13: _LL14:
# 758
 goto _LL16;case 3U: _LL19: _tmp1C5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15B)->f1).elt_type;_tmp1C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15B)->f1).ptr_atts).rgn;_tmp1C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15B)->f1).ptr_atts).bounds;_LL1A:
# 766
 Cyc_Binding_resolve_type(loc,env,_tmp1C5);
Cyc_Binding_resolve_type(loc,env,_tmp1C4);
Cyc_Binding_resolve_type(loc,env,_tmp1C3);
return;case 6U: _LL1B: _tmp1C6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp15B)->f1;_LL1C:
# 772
 for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)_tmp1C6->hd)).f2);}
return;case 4U: _LL1D: _tmp1C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15B)->f1).elt_type;_tmp1C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15B)->f1).num_elts;_LL1E:
# 776
 Cyc_Binding_resolve_type(loc,env,_tmp1C8);
if(_tmp1C7 != 0)Cyc_Binding_resolve_exp(env,_tmp1C7);
return;case 5U: _LL1F: _tmp1C9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15B)->f1;_LL20: {
# 783
struct Cyc_List_List*_tmp1AA=Cyc_Binding_get_fun_vardecls(0,loc,env,_tmp1C9.args,_tmp1C9.cyc_varargs);
# 785
struct Cyc_Dict_Dict*_tmp1AB=env->local_vars;
if(_tmp1AB != 0)
({struct Cyc_Dict_Dict*_tmp331=({struct Cyc_Dict_Dict*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));*_tmp1AC=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1AC;});env->local_vars=_tmp331;});else{
# 789
({struct Cyc_Dict_Dict*_tmp333=({struct Cyc_Dict_Dict*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));({struct Cyc_Dict_Dict _tmp332=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp1AD=_tmp332;});_tmp1AD;});env->local_vars=_tmp333;});}
{struct Cyc_List_List*_tmp1AE=_tmp1AA;for(0;_tmp1AE != 0;_tmp1AE=_tmp1AE->tl){
({struct Cyc_Dict_Dict _tmp337=({struct Cyc_Dict_Dict _tmp336=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp335=(*((struct Cyc_Absyn_Vardecl*)_tmp1AE->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp336,_tmp335,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));
_tmp1B0->tag=0U,({void*_tmp334=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->tag=3U,_tmp1AF->f1=(struct Cyc_Absyn_Vardecl*)_tmp1AE->hd;_tmp1AF;});_tmp1B0->f1=_tmp334;});_tmp1B0;}));});
# 791
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp337;});}}
# 794
Cyc_Binding_resolve_function_stuff(loc,env,_tmp1C9);
env->local_vars=_tmp1AB;
return;}case 9U: _LL21: _tmp1CA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp15B)->f1;_LL22:
# 798
 _tmp1CB=_tmp1CA;goto _LL24;default: _LL23: _tmp1CB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp15B)->f1;_LL24:
 Cyc_Binding_resolve_exp(env,_tmp1CB);return;}_LL0:;}struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 803
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1CC=p;void*_tmp21A;unsigned int _tmp219;_LL1: _tmp21A=_tmp1CC->r;_tmp219=_tmp1CC->loc;_LL2:;{
void*_tmp1CD=_tmp21A;struct Cyc_List_List*_tmp218;struct Cyc_Absyn_Exp*_tmp217;struct Cyc_List_List*_tmp216;struct Cyc_Absyn_Pat*_tmp215;struct Cyc_Absyn_Vardecl*_tmp214;struct Cyc_Absyn_Vardecl*_tmp213;struct Cyc_Absyn_Vardecl*_tmp212;struct Cyc_Absyn_Pat*_tmp211;struct Cyc_Absyn_Vardecl*_tmp210;struct Cyc_Absyn_Pat*_tmp20F;struct Cyc_List_List*_tmp20E;struct Cyc_List_List*_tmp20D;struct _tuple0*_tmp20C;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20A;int _tmp209;struct _tuple0*_tmp208;struct Cyc_List_List*_tmp207;int _tmp206;struct _tuple0*_tmp205;switch(*((int*)_tmp1CD)){case 15U: _LL4: _tmp205=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1;_LL5:
# 807
{struct _handler_cons _tmp1CE;_push_handler(& _tmp1CE);{int _tmp1D0=0;if(setjmp(_tmp1CE.handler))_tmp1D0=1;if(!_tmp1D0){{void*_tmp1D1=Cyc_Binding_lookup_ordinary(_tmp219,env,_tmp205);void*_tmp1D2=_tmp1D1;void*_tmp1DD;struct Cyc_Absyn_Enumfield*_tmp1DC;struct Cyc_Absyn_Enumdecl*_tmp1DB;struct Cyc_Absyn_Enumfield*_tmp1DA;struct Cyc_Absyn_Datatypedecl*_tmp1D9;struct Cyc_Absyn_Datatypefield*_tmp1D8;switch(*((int*)_tmp1D2)){case 0U: _LL2D: _LL2E:
 goto _LL2C;case 1U: _LL2F: _LL30:
# 810
({void*_tmp1D3=0U;({unsigned int _tmp339=_tmp219;struct _dyneither_ptr _tmp338=({const char*_tmp1D4="struct tag used without arguments in pattern";_tag_dyneither(_tmp1D4,sizeof(char),45U);});Cyc_Warn_err(_tmp339,_tmp338,_tag_dyneither(_tmp1D3,sizeof(void*),0U));});});
_npop_handler(0U);return;case 2U: _LL31: _tmp1D9=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1D2)->f1;_tmp1D8=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1D2)->f2;_LL32:
# 813
({void*_tmp33A=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->tag=8U,_tmp1D5->f1=_tmp1D9,_tmp1D5->f2=_tmp1D8,_tmp1D5->f3=0,_tmp1D5->f4=0;_tmp1D5;});p->r=_tmp33A;});_npop_handler(0U);return;case 3U: _LL33: _tmp1DB=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1D2)->f1;_tmp1DA=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp1D2)->f2;_LL34:
# 815
({void*_tmp33B=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->tag=13U,_tmp1D6->f1=_tmp1DB,_tmp1D6->f2=_tmp1DA;_tmp1D6;});p->r=_tmp33B;});_npop_handler(0U);return;default: _LL35: _tmp1DD=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1D2)->f1;_tmp1DC=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp1D2)->f2;_LL36:
# 817
({void*_tmp33C=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->tag=14U,_tmp1D7->f1=_tmp1DD,_tmp1D7->f2=_tmp1DC;_tmp1D7;});p->r=_tmp33C;});_npop_handler(0U);return;}_LL2C:;}
# 807
;_pop_handler();}else{void*_tmp1CF=(void*)_exn_thrown;void*_tmp1DE=_tmp1CF;void*_tmp1DF;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1DE)->tag == Cyc_Binding_BindingError){_LL38: _LL39:
# 818
 goto _LL37;}else{_LL3A: _tmp1DF=_tmp1DE;_LL3B:(int)_rethrow(_tmp1DF);}_LL37:;}};}{
struct Cyc_Absyn_Vardecl*_tmp1E0=Cyc_Absyn_new_vardecl(0U,_tmp205,Cyc_Absyn_void_type,0);
Cyc_Binding_resolve_and_add_vardecl(_tmp219,env,_tmp1E0);
({void*_tmp33E=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->tag=1U,_tmp1E1->f1=_tmp1E0,({struct Cyc_Absyn_Pat*_tmp33D=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp1E1->f2=_tmp33D;});_tmp1E1;});p->r=_tmp33E;});
return;};case 16U: _LL6: _tmp208=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1;_tmp207=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1CD)->f2;_tmp206=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1CD)->f3;_LL7:
# 825
{struct Cyc_List_List*_tmp1E2=_tmp207;for(0;_tmp1E2 != 0;_tmp1E2=_tmp1E2->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp1E2->hd);}}
{struct _handler_cons _tmp1E3;_push_handler(& _tmp1E3);{int _tmp1E5=0;if(setjmp(_tmp1E3.handler))_tmp1E5=1;if(!_tmp1E5){{void*_tmp1E6=Cyc_Binding_lookup_ordinary(_tmp219,env,_tmp208);void*_tmp1E7=_tmp1E6;struct Cyc_Absyn_Datatypedecl*_tmp1F3;struct Cyc_Absyn_Datatypefield*_tmp1F2;struct Cyc_Absyn_Aggrdecl*_tmp1F1;switch(*((int*)_tmp1E7)){case 0U: _LL3D: _LL3E:
 goto _LL3C;case 1U: _LL3F: _tmp1F1=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp1E7)->f1;_LL40: {
# 830
struct Cyc_List_List*_tmp1E8=0;
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
_tmp1E8=({struct Cyc_List_List*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));({struct _tuple12*_tmp33F=({struct _tuple12*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->f1=0,_tmp1E9->f2=(struct Cyc_Absyn_Pat*)_tmp207->hd;_tmp1E9;});_tmp1EA->hd=_tmp33F;}),_tmp1EA->tl=_tmp1E8;_tmp1EA;});}
({void*_tmp343=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp342=({union Cyc_Absyn_AggrInfo*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));({union Cyc_Absyn_AggrInfo _tmp341=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp1EB=_cycalloc(sizeof(*_tmp1EB));*_tmp1EB=_tmp1F1;_tmp1EB;}));*_tmp1EC=_tmp341;});_tmp1EC;});_tmp1ED->f1=_tmp342;}),_tmp1ED->f2=0,({struct Cyc_List_List*_tmp340=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1E8);_tmp1ED->f3=_tmp340;}),_tmp1ED->f4=_tmp206;_tmp1ED;});p->r=_tmp343;});
_npop_handler(0U);return;}case 2U: _LL41: _tmp1F3=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1E7)->f1;_tmp1F2=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp1E7)->f2;_LL42:
# 836
({void*_tmp344=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->tag=8U,_tmp1EE->f1=_tmp1F3,_tmp1EE->f2=_tmp1F2,_tmp1EE->f3=_tmp207,_tmp1EE->f4=_tmp206;_tmp1EE;});p->r=_tmp344;});_npop_handler(0U);return;case 3U: _LL43: _LL44:
 goto _LL46;default: _LL45: _LL46:
# 839
({void*_tmp1EF=0U;({unsigned int _tmp346=_tmp219;struct _dyneither_ptr _tmp345=({const char*_tmp1F0="enum tag used with arguments in pattern";_tag_dyneither(_tmp1F0,sizeof(char),40U);});Cyc_Warn_err(_tmp346,_tmp345,_tag_dyneither(_tmp1EF,sizeof(void*),0U));});});_npop_handler(0U);return;}_LL3C:;}
# 827
;_pop_handler();}else{void*_tmp1E4=(void*)_exn_thrown;void*_tmp1F4=_tmp1E4;void*_tmp1F5;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1F4)->tag == Cyc_Binding_BindingError){_LL48: _LL49:
# 840
 goto _LL47;}else{_LL4A: _tmp1F5=_tmp1F4;_LL4B:(int)_rethrow(_tmp1F5);}_LL47:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp1F8=({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0U,({struct _dyneither_ptr _tmp347=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp208));_tmp2AE.f1=_tmp347;});_tmp2AE;});void*_tmp1F6[1U];_tmp1F6[0]=& _tmp1F8;({unsigned int _tmp349=_tmp219;struct _dyneither_ptr _tmp348=({const char*_tmp1F7="%s is not a constructor in pattern";_tag_dyneither(_tmp1F7,sizeof(char),35U);});Cyc_Warn_err(_tmp349,_tmp348,_tag_dyneither(_tmp1F6,sizeof(void*),1U));});});
return;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1)->UnknownAggr).tag == 1){_LL8: _tmp20C=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1)->UnknownAggr).val).f2;_tmp20B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CD)->f2;_tmp20A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CD)->f3;_tmp209=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CD)->f4;_LL9:
# 845
{struct Cyc_List_List*_tmp1F9=_tmp20A;for(0;_tmp1F9 != 0;_tmp1F9=_tmp1F9->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple12*)_tmp1F9->hd)).f2);}}
{struct _handler_cons _tmp1FA;_push_handler(& _tmp1FA);{int _tmp1FC=0;if(setjmp(_tmp1FA.handler))_tmp1FC=1;if(!_tmp1FC){
{struct Cyc_Absyn_Aggrdecl*_tmp1FD=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(_tmp219,env->ns,_tmp20C,Cyc_Binding_lookup_aggrdecl);
({void*_tmp34C=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp34B=({union Cyc_Absyn_AggrInfo*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));({union Cyc_Absyn_AggrInfo _tmp34A=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp1FE=_cycalloc(sizeof(*_tmp1FE));*_tmp1FE=_tmp1FD;_tmp1FE;}));*_tmp1FF=_tmp34A;});_tmp1FF;});_tmp200->f1=_tmp34B;}),_tmp200->f2=_tmp20B,_tmp200->f3=_tmp20A,_tmp200->f4=_tmp209;_tmp200;});p->r=_tmp34C;});}
# 848
;_pop_handler();}else{void*_tmp1FB=(void*)_exn_thrown;void*_tmp201=_tmp1FB;void*_tmp204;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp201)->tag == Cyc_Binding_BindingError){_LL4D: _LL4E:
# 851
({void*_tmp202=0U;({unsigned int _tmp34E=_tmp219;struct _dyneither_ptr _tmp34D=({const char*_tmp203="non-aggregate name has designator patterns";_tag_dyneither(_tmp203,sizeof(char),43U);});Cyc_Warn_err(_tmp34E,_tmp34D,_tag_dyneither(_tmp202,sizeof(void*),0U));});});
return;}else{_LL4F: _tmp204=_tmp201;_LL50:(int)_rethrow(_tmp204);}_LL4C:;}};}
# 854
return;}else{_LL24: _tmp20D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CD)->f3;_LL25:
# 883
 for(0;_tmp20D != 0;_tmp20D=_tmp20D->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple12*)_tmp20D->hd)).f2);}
return;}}else{_LL22: _tmp20E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CD)->f3;_LL23:
# 881
 _tmp20D=_tmp20E;goto _LL25;}case 0U: _LLA: _LLB:
# 856
 return;case 3U: _LLC: _tmp210=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1;_tmp20F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1CD)->f2;_LLD:
# 858
 _tmp212=_tmp210;_tmp211=_tmp20F;goto _LLF;case 1U: _LLE: _tmp212=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1;_tmp211=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1CD)->f2;_LLF:
# 860
 Cyc_Binding_resolve_pat(env,_tmp211);
_tmp213=_tmp212;goto _LL11;case 4U: _LL10: _tmp213=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1CD)->f2;_LL11:
 _tmp214=_tmp213;goto _LL13;case 2U: _LL12: _tmp214=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1CD)->f2;_LL13:
# 864
 Cyc_Binding_resolve_and_add_vardecl(_tmp219,env,_tmp214);goto _LL3;case 6U: _LL14: _tmp215=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1;_LL15:
# 866
 Cyc_Binding_resolve_pat(env,_tmp215);return;case 5U: _LL16: _tmp216=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1;_LL17:
# 868
 for(0;_tmp216 != 0;_tmp216=_tmp216->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp216->hd);}
goto _LL3;case 17U: _LL18: _tmp217=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1CD)->f1;_LL19:
# 872
 Cyc_Binding_resolve_exp(env,_tmp217);return;case 9U: _LL1A: _LL1B:
# 874
 goto _LL1D;case 10U: _LL1C: _LL1D:
 goto _LL1F;case 11U: _LL1E: _LL1F:
 goto _LL21;case 12U: _LL20: _LL21:
 return;case 8U: _LL26: _tmp218=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1CD)->f3;_LL27:
# 887
 for(0;_tmp218 != 0;_tmp218=_tmp218->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)_tmp218->hd);}
return;case 13U: _LL28: _LL29:
 goto _LL2B;default: _LL2A: _LL2B:
 return;}_LL3:;};}struct _tuple13{void*f1;void*f2;};
# 895
void Cyc_Binding_resolve_rgnpo(unsigned int loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)po->hd)).f2);}}struct _tuple14{unsigned int f1;struct _tuple0*f2;int f3;};
# 909 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 912
unsigned int loc=d->loc;
void*_tmp21B=d->r;void*_tmp21C=_tmp21B;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp298;struct Cyc_List_List*_tmp297;struct Cyc_List_List*_tmp296;struct _tuple0*_tmp295;struct Cyc_List_List*_tmp294;struct _dyneither_ptr*_tmp293;struct Cyc_List_List*_tmp292;struct Cyc_Absyn_Datatypedecl*_tmp291;struct Cyc_Absyn_Enumdecl*_tmp290;struct Cyc_Absyn_Aggrdecl*_tmp28F;struct Cyc_Absyn_Typedefdecl*_tmp28E;struct Cyc_Absyn_Tvar*_tmp28D;struct Cyc_Absyn_Vardecl*_tmp28C;struct Cyc_Absyn_Exp*_tmp28B;struct Cyc_List_List*_tmp28A;struct Cyc_Absyn_Pat*_tmp289;struct Cyc_Absyn_Exp*_tmp288;struct Cyc_Absyn_Fndecl*_tmp287;struct Cyc_Absyn_Vardecl*_tmp286;switch(*((int*)_tmp21C)){case 0U: _LL1: _tmp286=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_LL2:
# 916
 Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp286);
# 921
if(_tmp286->initializer != 0  && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp286->initializer));
goto _LL0;case 1U: _LL3: _tmp287=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_LL4:
# 926
 Cyc_Binding_absolutize_decl(loc,env,_tmp287->name,_tmp287->sc);{
struct _tuple0*_tmp21D=_tmp287->name;struct _tuple0*_tmp21E=_tmp21D;union Cyc_Absyn_Nmspace _tmp22F;struct _dyneither_ptr*_tmp22E;_LL24: _tmp22F=_tmp21E->f1;_tmp22E=_tmp21E->f2;_LL25:;{
# 929
struct Cyc_List_List*_tmp21F=Cyc_Binding_get_fun_vardecls(1,loc,env,(_tmp287->i).args,(_tmp287->i).cyc_varargs);
({struct Cyc_Core_Opt*_tmp34F=({struct Cyc_Core_Opt*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220->v=_tmp21F;_tmp220;});_tmp287->param_vardecls=_tmp34F;});
# 932
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,_tmp287->name);{
# 935
struct Cyc_Dict_Dict*_tmp221=env->local_vars;
if(_tmp221 != 0)
({struct Cyc_Dict_Dict*_tmp350=({struct Cyc_Dict_Dict*_tmp222=_cycalloc(sizeof(*_tmp222));*_tmp222=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp222;});env->local_vars=_tmp350;});else{
# 939
({struct Cyc_Dict_Dict*_tmp352=({struct Cyc_Dict_Dict*_tmp223=_cycalloc(sizeof(*_tmp223));({struct Cyc_Dict_Dict _tmp351=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp223=_tmp351;});_tmp223;});env->local_vars=_tmp352;});}
{struct Cyc_List_List*_tmp224=_tmp21F;for(0;_tmp224 != 0;_tmp224=_tmp224->tl){
({struct Cyc_Dict_Dict _tmp356=({struct Cyc_Dict_Dict _tmp355=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp354=(*((struct Cyc_Absyn_Vardecl*)_tmp224->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp355,_tmp354,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp226=_cycalloc(sizeof(*_tmp226));
_tmp226->tag=0U,({void*_tmp353=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->tag=3U,_tmp225->f1=(struct Cyc_Absyn_Vardecl*)_tmp224->hd;_tmp225;});_tmp226->f1=_tmp353;});_tmp226;}));});
# 941
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp356;});}}
# 944
Cyc_Binding_resolve_function_stuff(loc,env,_tmp287->i);
# 946
if(_tmp221 != 0){
# 948
({struct Cyc_Dict_Dict _tmp35A=({struct Cyc_Dict_Dict _tmp359=*_tmp221;struct _dyneither_ptr*_tmp358=_tmp22E;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp359,_tmp358,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp228=_cycalloc(sizeof(*_tmp228));
_tmp228->tag=0U,({void*_tmp357=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->tag=2U,_tmp227->f1=_tmp287;_tmp227;});_tmp228->f1=_tmp357;});_tmp228;}));});
# 948
*_tmp221=_tmp35A;});
# 950
({struct Cyc_Dict_Dict _tmp35E=({struct Cyc_Dict_Dict _tmp35D=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _dyneither_ptr*_tmp35C=_tmp22E;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp35D,_tmp35C,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp22A=_cycalloc(sizeof(*_tmp22A));
_tmp22A->tag=0U,({void*_tmp35B=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->tag=2U,_tmp229->f1=_tmp287;_tmp229;});_tmp22A->f1=_tmp35B;});_tmp22A;}));});
# 950
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp35E;});}else{
# 953
struct Cyc_Binding_ResolveNSEnv*_tmp22B=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp22F);
({struct Cyc_Dict_Dict _tmp362=({struct Cyc_Dict_Dict _tmp361=_tmp22B->ordinaries;struct _dyneither_ptr*_tmp360=_tmp22E;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp361,_tmp360,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp22D=_cycalloc(sizeof(*_tmp22D));
# 956
_tmp22D->tag=0U,({void*_tmp35F=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->tag=2U,_tmp22C->f1=_tmp287;_tmp22C;});_tmp22D->f1=_tmp35F;});_tmp22D;}));});
# 954
_tmp22B->ordinaries=_tmp362;});}
# 959
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,_tmp287->body);
# 962
env->local_vars=_tmp221;
goto _LL0;};};};case 2U: _LL5: _tmp289=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_tmp288=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp21C)->f3;_LL6:
# 966
 if(Cyc_Binding_at_toplevel(env)){
({void*_tmp230=0U;({unsigned int _tmp364=loc;struct _dyneither_ptr _tmp363=({const char*_tmp231="let not allowed at top-level";_tag_dyneither(_tmp231,sizeof(char),29U);});Cyc_Warn_err(_tmp364,_tmp363,_tag_dyneither(_tmp230,sizeof(void*),0U));});});
goto _LL0;}
# 970
Cyc_Binding_resolve_exp(env,_tmp288);
Cyc_Binding_resolve_pat(env,_tmp289);
goto _LL0;case 3U: _LL7: _tmp28A=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_LL8:
# 975
 for(0;_tmp28A != 0;_tmp28A=_tmp28A->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)_tmp28A->hd);}
goto _LL0;case 4U: _LL9: _tmp28D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_tmp28C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp21C)->f2;_tmp28B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp21C)->f3;_LLA:
# 980
 if(_tmp28B != 0)
Cyc_Binding_resolve_exp(env,_tmp28B);
Cyc_Binding_resolve_and_add_vardecl(loc,env,_tmp28C);
goto _LL0;case 8U: _LLB: _tmp28E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_LLC:
# 986
 if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp232=0U;({unsigned int _tmp366=loc;struct _dyneither_ptr _tmp365=({const char*_tmp233="nested type definitions are not yet supported";_tag_dyneither(_tmp233,sizeof(char),46U);});Cyc_Warn_err(_tmp366,_tmp365,_tag_dyneither(_tmp232,sizeof(void*),0U));});});
goto _LL0;}
# 990
Cyc_Binding_absolutize_decl(loc,env,_tmp28E->name,_tmp28E->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp234=_tmp28E->name;struct _tuple0*_tmp235=_tmp234;union Cyc_Absyn_Nmspace _tmp238;struct _dyneither_ptr*_tmp237;_LL27: _tmp238=_tmp235->f1;_tmp237=_tmp235->f2;_LL28:;{
# 993
struct Cyc_Binding_ResolveNSEnv*_tmp236=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp238);
if(_tmp28E->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(_tmp28E->defn));
# 997
({struct Cyc_Dict_Dict _tmp367=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp236->typedefs,_tmp237,_tmp28E);_tmp236->typedefs=_tmp367;});
# 999
goto _LL0;};};case 5U: _LLD: _tmp28F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_LLE:
# 1002
 if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp239=0U;({unsigned int _tmp369=loc;struct _dyneither_ptr _tmp368=({const char*_tmp23A="nested type definitions are not yet supported";_tag_dyneither(_tmp23A,sizeof(char),46U);});Cyc_Warn_err(_tmp369,_tmp368,_tag_dyneither(_tmp239,sizeof(void*),0U));});});
goto _LL0;}
# 1006
Cyc_Binding_absolutize_decl(loc,env,_tmp28F->name,_tmp28F->sc);{
struct _tuple0*_tmp23B=_tmp28F->name;struct _tuple0*_tmp23C=_tmp23B;union Cyc_Absyn_Nmspace _tmp244;struct _dyneither_ptr*_tmp243;_LL2A: _tmp244=_tmp23C->f1;_tmp243=_tmp23C->f2;_LL2B:;{
# 1009
struct Cyc_Binding_ResolveNSEnv*_tmp23D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp244);
# 1011
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp23D->aggrdecls,_tmp243) && _tmp28F->impl == 0)
goto _LL0;
# 1014
({struct Cyc_Dict_Dict _tmp36A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp23D->aggrdecls,_tmp243,_tmp28F);_tmp23D->aggrdecls=_tmp36A;});
# 1016
({struct Cyc_Dict_Dict _tmp36D=({struct Cyc_Dict_Dict _tmp36C=_tmp23D->ordinaries;struct _dyneither_ptr*_tmp36B=_tmp243;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp36C,_tmp36B,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));
_tmp23E->tag=1U,_tmp23E->f1=_tmp28F;_tmp23E;}));});
# 1016
_tmp23D->ordinaries=_tmp36D;});
# 1018
if(_tmp28F->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp23F=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp28F->impl);struct Cyc_Absyn_AggrdeclImpl*_tmp240=_tmp23F;struct Cyc_List_List*_tmp242;struct Cyc_List_List*_tmp241;_LL2D: _tmp242=_tmp240->rgn_po;_tmp241=_tmp240->fields;_LL2E:;
Cyc_Binding_resolve_rgnpo(loc,env,_tmp242);
Cyc_Binding_resolve_aggrfields(loc,env,_tmp241);}
# 1023
goto _LL0;};};case 7U: _LLF: _tmp290=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_LL10:
# 1026
 if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp245=0U;({unsigned int _tmp36F=loc;struct _dyneither_ptr _tmp36E=({const char*_tmp246="nested type definitions are not yet supported";_tag_dyneither(_tmp246,sizeof(char),46U);});Cyc_Warn_err(_tmp36F,_tmp36E,_tag_dyneither(_tmp245,sizeof(void*),0U));});});
goto _LL0;}
# 1030
Cyc_Binding_absolutize_decl(loc,env,_tmp290->name,_tmp290->sc);{
struct _tuple0*_tmp247=_tmp290->name;struct _tuple0*_tmp248=_tmp247;union Cyc_Absyn_Nmspace _tmp24D;struct _dyneither_ptr*_tmp24C;_LL30: _tmp24D=_tmp248->f1;_tmp24C=_tmp248->f2;_LL31:;{
# 1033
struct Cyc_Binding_ResolveNSEnv*_tmp249=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp24D);
# 1035
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp249->enumdecls,_tmp24C) && _tmp290->fields == 0)
goto _LL0;
# 1039
({struct Cyc_Dict_Dict _tmp370=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp249->enumdecls,_tmp24C,_tmp290);_tmp249->enumdecls=_tmp370;});
# 1041
if(_tmp290->fields != 0){
# 1043
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp290->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp24A=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(_tmp24A->loc,env,_tmp24A->name,_tmp290->sc);
if(_tmp24A->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp24A->tag));
({struct Cyc_Dict_Dict _tmp373=({struct Cyc_Dict_Dict _tmp372=_tmp249->ordinaries;struct _dyneither_ptr*_tmp371=(*_tmp24A->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp372,_tmp371,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp24B=_cycalloc(sizeof(*_tmp24B));
# 1050
_tmp24B->tag=3U,_tmp24B->f1=_tmp290,_tmp24B->f2=_tmp24A;_tmp24B;}));});
# 1048
_tmp249->ordinaries=_tmp373;});}}
# 1052
goto _LL0;};};case 6U: _LL11: _tmp291=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_LL12:
# 1056
{struct _handler_cons _tmp24E;_push_handler(& _tmp24E);{int _tmp250=0;if(setjmp(_tmp24E.handler))_tmp250=1;if(!_tmp250){
{struct Cyc_List_List*_tmp251=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _dyneither_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,_tmp291->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*_tmp252=(struct Cyc_Absyn_Datatypedecl*)_tmp251->hd;
if(!_tmp252->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->tag=Cyc_Binding_BindingError;_tmp253;}));
_tmp291->name=_tmp252->name;
_tmp291->is_extensible=1;{
struct _tuple0*_tmp254=_tmp291->name;struct _tuple0*_tmp255=_tmp254;union Cyc_Absyn_Nmspace _tmp259;struct _dyneither_ptr*_tmp258;_LL33: _tmp259=_tmp255->f1;_tmp258=_tmp255->f2;_LL34:;{
struct Cyc_Binding_ResolveNSEnv*_tmp256=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp259);
({struct Cyc_Dict_Dict _tmp376=({struct Cyc_Dict_Dict _tmp375=_tmp256->datatypedecls;struct _dyneither_ptr*_tmp374=_tmp258;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp375,_tmp374,({struct Cyc_List_List*_tmp257=_cycalloc(sizeof(*_tmp257));
_tmp257->hd=_tmp291,_tmp257->tl=_tmp251;_tmp257;}));});
# 1065
_tmp256->datatypedecls=_tmp376;});};};}
# 1057
;_pop_handler();}else{void*_tmp24F=(void*)_exn_thrown;void*_tmp25A=_tmp24F;void*_tmp261;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp25A)->tag == Cyc_Binding_BindingError){_LL36: _LL37:
# 1069
 Cyc_Binding_absolutize_topdecl(loc,env,_tmp291->name,_tmp291->sc);{
struct _tuple0*_tmp25B=_tmp291->name;struct _tuple0*_tmp25C=_tmp25B;union Cyc_Absyn_Nmspace _tmp260;struct _dyneither_ptr*_tmp25F;_LL3B: _tmp260=_tmp25C->f1;_tmp25F=_tmp25C->f2;_LL3C:;{
struct Cyc_Binding_ResolveNSEnv*_tmp25D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,_tmp260);
# 1073
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp25D->datatypedecls,_tmp25F) && _tmp291->fields == 0)
# 1075
goto _LL35;
({struct Cyc_Dict_Dict _tmp379=({struct Cyc_Dict_Dict _tmp378=_tmp25D->datatypedecls;struct _dyneither_ptr*_tmp377=_tmp25F;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp378,_tmp377,({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));
_tmp25E->hd=_tmp291,_tmp25E->tl=0;_tmp25E;}));});
# 1076
_tmp25D->datatypedecls=_tmp379;});
# 1078
goto _LL35;};};}else{_LL38: _tmp261=_tmp25A;_LL39:(int)_rethrow(_tmp261);}_LL35:;}};}{
# 1081
struct _tuple0*_tmp262=_tmp291->name;struct _tuple0*_tmp263=_tmp262;union Cyc_Absyn_Nmspace _tmp270;struct _dyneither_ptr*_tmp26F;_LL3E: _tmp270=_tmp263->f1;_tmp26F=_tmp263->f2;_LL3F:;
if(_tmp291->fields != 0){
struct Cyc_List_List*_tmp264=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp291->fields))->v;for(0;_tmp264 != 0;_tmp264=_tmp264->tl){
struct Cyc_Absyn_Datatypefield*_tmp265=(struct Cyc_Absyn_Datatypefield*)_tmp264->hd;
{struct Cyc_List_List*_tmp266=_tmp265->typs;for(0;_tmp266 != 0;_tmp266=_tmp266->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple11*)_tmp266->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp267=(*_tmp265->name).f1;union Cyc_Absyn_Nmspace _tmp268=_tmp267;switch((_tmp268.Abs_n).tag){case 1U: if((_tmp268.Rel_n).val == 0){_LL41: _LL42:
# 1089
 if(_tmp291->is_extensible)
({union Cyc_Absyn_Nmspace _tmp37A=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);(*_tmp265->name).f1=_tmp37A;});else{
# 1092
(*_tmp265->name).f1=(*_tmp291->name).f1;}
goto _LL40;}else{_LL43: _LL44:
# 1095
({void*_tmp269=0U;({unsigned int _tmp37C=_tmp265->loc;struct _dyneither_ptr _tmp37B=({const char*_tmp26A="qualified datatype field declaratations are not allowed";_tag_dyneither(_tmp26A,sizeof(char),56U);});Cyc_Warn_err(_tmp37C,_tmp37B,_tag_dyneither(_tmp269,sizeof(void*),0U));});});
return;}case 2U: _LL45: _LL46:
 goto _LL40;default: _LL47: _LL48:
({void*_tmp26B=0U;({struct _dyneither_ptr _tmp37D=({const char*_tmp26C="datatype field Loc_n or C_n";_tag_dyneither(_tmp26C,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp37D,_tag_dyneither(_tmp26B,sizeof(void*),0U));});});}_LL40:;}{
# 1100
struct Cyc_Binding_ResolveNSEnv*_tmp26D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*_tmp265->name).f1);
({struct Cyc_Dict_Dict _tmp380=({struct Cyc_Dict_Dict _tmp37F=_tmp26D->ordinaries;struct _dyneither_ptr*_tmp37E=(*_tmp265->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp37F,_tmp37E,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp26E=_cycalloc(sizeof(*_tmp26E));
# 1103
_tmp26E->tag=2U,_tmp26E->f1=_tmp291,_tmp26E->f2=_tmp265;_tmp26E;}));});
# 1101
_tmp26D->ordinaries=_tmp380;});};}}
# 1105
goto _LL0;};case 9U: _LL13: _tmp293=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_tmp292=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp21C)->f2;_LL14:
# 1108
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _dyneither_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,_tmp293,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,_tmp292);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL0;case 10U: _LL15: _tmp295=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_tmp294=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp21C)->f2;_LL16:
# 1113
((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,_tmp295);
Cyc_Binding_resolve_decls(env,_tmp294);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL0;case 11U: _LL17: _tmp296=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_LL18: {
# 1119
int _tmp271=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp296);
env->in_cinclude=_tmp271;
goto _LL0;}case 12U: _LL19: _tmp299=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp21C)->f1;_tmp298=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp21C)->f2;_tmp297=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp21C)->f3;_LL1A:
# 1130
{struct Cyc_List_List*_tmp272=_tmp297;for(0;_tmp272 != 0;_tmp272=_tmp272->tl){
struct _tuple14*_tmp273=(struct _tuple14*)_tmp272->hd;struct _tuple14*_tmp274=_tmp273;unsigned int _tmp276;struct _tuple0*_tmp275;_LL4A: _tmp276=_tmp274->f1;_tmp275=_tmp274->f2;_LL4B:;
Cyc_Binding_absolutize_decl(_tmp276,env,_tmp275,Cyc_Absyn_ExternC);}}
# 1138
if(!Cyc_Binding_at_toplevel(env)){
({void*_tmp277=0U;({unsigned int _tmp382=loc;struct _dyneither_ptr _tmp381=({const char*_tmp278="extern \"C include\" not at toplevel";_tag_dyneither(_tmp278,sizeof(char),35U);});Cyc_Warn_err(_tmp382,_tmp381,_tag_dyneither(_tmp277,sizeof(void*),0U));});});
goto _LL0;}{
# 1142
struct Cyc_Binding_ResolveNSEnv*_tmp279=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);
struct Cyc_Dict_Dict _tmp27A=_tmp279->ordinaries;
int _tmp27B=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,_tmp299);
Cyc_Binding_resolve_decls(env,_tmp298);{
struct Cyc_Dict_Dict _tmp27C=_tmp279->ordinaries;
struct Cyc_Dict_Dict _tmp27D=_tmp27A;
{struct Cyc_List_List*_tmp27E=_tmp297;for(0;_tmp27E != 0;_tmp27E=_tmp27E->tl){
struct _tuple14*_tmp27F=(struct _tuple14*)_tmp27E->hd;struct _tuple14*_tmp280=_tmp27F;unsigned int _tmp285;struct _dyneither_ptr*_tmp284;_LL4D: _tmp285=_tmp280->f1;_tmp284=(_tmp280->f2)->f2;_LL4E:;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp27C,_tmp284) || 
((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp27A,_tmp284) && ({
void*_tmp383=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27A,_tmp284);_tmp383 == ((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27C,_tmp284);}))
({struct Cyc_String_pa_PrintArg_struct _tmp283=({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0U,_tmp2AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp284);_tmp2AF;});void*_tmp281[1U];_tmp281[0]=& _tmp283;({unsigned int _tmp385=_tmp285;struct _dyneither_ptr _tmp384=({const char*_tmp282="%s is exported but not defined";_tag_dyneither(_tmp282,sizeof(char),31U);});Cyc_Warn_err(_tmp385,_tmp384,_tag_dyneither(_tmp281,sizeof(void*),1U));});});
_tmp27D=({struct Cyc_Dict_Dict _tmp387=_tmp27D;struct _dyneither_ptr*_tmp386=_tmp284;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp387,_tmp386,((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27C,_tmp284));});}}
# 1158
_tmp279->ordinaries=_tmp27D;
env->in_cinclude=_tmp27B;
# 1162
Cyc_Cifc_user_overrides(loc,_tmp299,_tmp298);
goto _LL0;};};case 13U: _LL1B: _LL1C:
# 1165
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 1169
 goto _LL0;}_LL0:;}
# 1173
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1178
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp29A=({struct Cyc_Binding_Env*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->in_cinclude=0,({struct Cyc_Binding_NSCtxt*_tmp388=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp29D->ns=_tmp388;}),_tmp29D->local_vars=0;_tmp29D;});
({struct Cyc_Binding_Env*_tmp38B=_tmp29A;Cyc_Binding_resolve_decl(_tmp38B,({struct Cyc_Absyn_Decl*_tmp29C=_cycalloc(sizeof(*_tmp29C));({void*_tmp38A=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp389=Cyc_Absyn_exn_tud();_tmp29B->f1=_tmp389;});_tmp29B;});_tmp29C->r=_tmp38A;}),_tmp29C->loc=0U;_tmp29C;}));});
Cyc_Binding_resolve_decls(_tmp29A,tds);}

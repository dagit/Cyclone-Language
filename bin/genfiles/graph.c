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
# 119 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100 "cycboot.h"
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 75
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 82
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 85
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 235
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d);
# 274
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,void*);
# 46 "graph.h"
void Cyc_Graph_print(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict g,void(*nodeprint)(struct Cyc___cycFILE*,void*));
# 50
struct Cyc_Dict_Dict Cyc_Graph_empty(int(*cmp)(void*,void*));
# 53
struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict g,void*s);
# 56
struct Cyc_Dict_Dict Cyc_Graph_add_edge(struct Cyc_Dict_Dict g,void*s,void*t);
# 59
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict g,void*s,struct Cyc_Set_Set*T);
# 63
struct Cyc_Dict_Dict Cyc_Graph_remove_edge(struct Cyc_Dict_Dict g,void*s,void*t);
# 66
struct Cyc_Dict_Dict Cyc_Graph_remove_edges(struct Cyc_Dict_Dict g,void*s,struct Cyc_Set_Set*T);
# 69
int Cyc_Graph_is_edge(struct Cyc_Dict_Dict g,void*s,void*t);
# 72
struct Cyc_Set_Set*Cyc_Graph_get_targets(struct Cyc_Dict_Dict,void*s);
# 74
struct Cyc_Dict_Dict Cyc_Graph_tc(struct Cyc_Dict_Dict g);
# 80
struct Cyc_List_List*Cyc_Graph_tsort(struct Cyc_Dict_Dict g);
# 87
struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict g);
# 27 "graph.cyc"
struct Cyc_Dict_Dict Cyc_Graph_empty(int(*cmp)(void*,void*)){
return((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(cmp);}
# 34
struct Cyc_Set_Set*Cyc_Graph_get_targets(struct Cyc_Dict_Dict g,void*source){
struct Cyc_Set_Set**_tmp0=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_tmp1=_tmp0;struct Cyc_Set_Set*_tmp2;if(_tmp1 == 0){_LL1: _LL2:
 return Cyc_Set_empty(g.rel);}else{_LL3: _tmp2=*_tmp1;_LL4:
 return _tmp2;}_LL0:;}
# 45
struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict g,void*source){
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(g,source))return g;else{
return({struct Cyc_Dict_Dict _tmp8E=g;void*_tmp8D=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp8E,_tmp8D,Cyc_Set_empty(g.rel));});}}
# 53
struct Cyc_Dict_Dict Cyc_Graph_add_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp3=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp90=g;void*_tmp8F=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp90,_tmp8F,Cyc_Set_insert(_tmp3,target));});}
# 61
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*_tmp4=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp92=g;void*_tmp91=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp92,_tmp91,Cyc_Set_union_two(_tmp4,targets));});}
# 69
struct Cyc_Dict_Dict Cyc_Graph_remove_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp5=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp94=g;void*_tmp93=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp94,_tmp93,Cyc_Set_delete(_tmp5,target));});}
# 77
struct Cyc_Dict_Dict Cyc_Graph_remove_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*_tmp6=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp96=g;void*_tmp95=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp96,_tmp95,Cyc_Set_diff(_tmp6,targets));});}
# 86
static struct Cyc_List_List*Cyc_Graph_to_list(struct Cyc_Set_Set*ts){
# 88
struct Cyc_List_List*_tmp7=0;
while(!Cyc_Set_is_empty(ts)){
void*_tmp8=Cyc_Set_choose(ts);
ts=Cyc_Set_delete(ts,_tmp8);
_tmp7=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->hd=_tmp8,_tmp9->tl=_tmp7;_tmp9;});}
# 94
return _tmp7;}
# 100
int Cyc_Graph_is_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set**_tmpA=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_tmpB=_tmpA;struct Cyc_Set_Set*_tmpC;if(_tmpB == 0){_LL1: _LL2:
# 103
 return 0;}else{_LL3: _tmpC=*_tmpB;_LL4:
# 105
 return Cyc_Set_member(_tmpC,target);}_LL0:;}struct _tuple0{void*f1;struct Cyc_Set_Set*f2;};
# 112
void Cyc_Graph_print(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict g,void(*nodeprint)(struct Cyc___cycFILE*,void*)){
({void*_tmpD=0U;({struct Cyc___cycFILE*_tmp98=f;struct _dyneither_ptr _tmp97=({const char*_tmpE="digraph {\n";_tag_dyneither(_tmpE,sizeof(char),11U);});Cyc_fprintf(_tmp98,_tmp97,_tag_dyneither(_tmpD,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);
for(0;(unsigned int)_tmpF;_tmpF=_tmpF->tl){
struct _tuple0*_tmp10=(struct _tuple0*)_tmpF->hd;struct _tuple0*_tmp11=_tmp10;void*_tmp1C;struct Cyc_Set_Set*_tmp1B;_LL1: _tmp1C=_tmp11->f1;_tmp1B=_tmp11->f2;_LL2:;{
struct Cyc_List_List*_tmp12=Cyc_Graph_to_list(_tmp1B);
if((unsigned int)_tmp12)
for(0;(unsigned int)_tmp12;_tmp12=_tmp12->tl){
nodeprint(f,_tmp1C);
({void*_tmp13=0U;({struct Cyc___cycFILE*_tmp9A=f;struct _dyneither_ptr _tmp99=({const char*_tmp14=" -> ";_tag_dyneither(_tmp14,sizeof(char),5U);});Cyc_fprintf(_tmp9A,_tmp99,_tag_dyneither(_tmp13,sizeof(void*),0U));});});
nodeprint(f,_tmp12->hd);
({void*_tmp15=0U;({struct Cyc___cycFILE*_tmp9C=f;struct _dyneither_ptr _tmp9B=({const char*_tmp16=";\n";_tag_dyneither(_tmp16,sizeof(char),3U);});Cyc_fprintf(_tmp9C,_tmp9B,_tag_dyneither(_tmp15,sizeof(void*),0U));});});}else{
# 126
({void*_tmp17=0U;({struct Cyc___cycFILE*_tmp9E=f;struct _dyneither_ptr _tmp9D=({const char*_tmp18="node ";_tag_dyneither(_tmp18,sizeof(char),6U);});Cyc_fprintf(_tmp9E,_tmp9D,_tag_dyneither(_tmp17,sizeof(void*),0U));});});
nodeprint(f,_tmp1C);
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmpA0=f;struct _dyneither_ptr _tmp9F=({const char*_tmp1A="; // no targets\n";_tag_dyneither(_tmp1A,sizeof(char),17U);});Cyc_fprintf(_tmpA0,_tmp9F,_tag_dyneither(_tmp19,sizeof(void*),0U));});});}};}
# 131
({void*_tmp1D=0U;({struct Cyc___cycFILE*_tmpA2=f;struct _dyneither_ptr _tmpA1=({const char*_tmp1E="}\n";_tag_dyneither(_tmp1E,sizeof(char),3U);});Cyc_fprintf(_tmpA2,_tmpA1,_tag_dyneither(_tmp1D,sizeof(void*),0U));});});};}
# 141 "graph.cyc"
static struct Cyc_List_List*Cyc_Graph_sourcesOf(struct Cyc_Dict_Dict g,void*node){
struct Cyc_List_List*_tmp1F=0;
# 144
{struct Cyc_List_List*_tmp20=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);for(0;(unsigned int)_tmp20;_tmp20=_tmp20->tl){
struct _tuple0*_tmp21=(struct _tuple0*)_tmp20->hd;struct _tuple0*_tmp22=_tmp21;void*_tmp25;struct Cyc_Set_Set*_tmp24;_LL1: _tmp25=_tmp22->f1;_tmp24=_tmp22->f2;_LL2:;
if(Cyc_Set_member(_tmp24,node))_tmp1F=({struct Cyc_List_List*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->hd=_tmp25,_tmp23->tl=_tmp1F;_tmp23;});}}
# 148
return _tmp1F;}struct _tuple1{struct Cyc_Set_Set*f1;struct Cyc_Dict_Dict f2;};
# 157
static struct _tuple1 Cyc_Graph_divideGraph(struct Cyc_Dict_Dict g,void*source){
# 160
struct Cyc_Set_Set**_tmp26=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_tmp27=_tmp26;struct Cyc_Set_Set*_tmp28;if(_tmp27 == 0){_LL1: _LL2:
 return({struct _tuple1 _tmp8B;({struct Cyc_Set_Set*_tmpA3=Cyc_Set_empty(g.rel);_tmp8B.f1=_tmpA3;}),_tmp8B.f2=g;_tmp8B;});}else{_LL3: _tmp28=*_tmp27;_LL4:
 return({struct _tuple1 _tmp8C;_tmp8C.f1=_tmp28,({struct Cyc_Dict_Dict _tmpA4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_delete)(g,source);_tmp8C.f2=_tmpA4;});_tmp8C;});}_LL0:;}
# 169
static struct Cyc_Dict_Dict Cyc_Graph_add_edgeTc(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp29=Cyc_Graph_get_targets(g,target);
struct _tuple1 _tmp2A=Cyc_Graph_divideGraph(g,source);struct _tuple1 _tmp2B=_tmp2A;struct Cyc_Set_Set*_tmp31;struct Cyc_Dict_Dict _tmp30;_LL1: _tmp31=_tmp2B.f1;_tmp30=_tmp2B.f2;_LL2:;{
struct Cyc_List_List*_tmp2C=Cyc_Graph_sourcesOf(_tmp30,source);
struct Cyc_Set_Set*_tmp2D=({struct Cyc_Set_Set*_tmpA5=Cyc_Set_union_two(_tmp31,_tmp29);Cyc_Set_insert(_tmpA5,target);});
struct Cyc_Dict_Dict _tmp2E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp30,source,_tmp2D);
{struct Cyc_List_List*_tmp2F=_tmp2C;for(0;(unsigned int)_tmp2F;_tmp2F=_tmp2F->tl){
_tmp2E=Cyc_Graph_add_edges(_tmp2E,_tmp2F->hd,_tmp2D);}}
# 178
return _tmp2E;};}
# 180
struct Cyc_Dict_Dict Cyc_Graph_tc_slow(struct Cyc_Dict_Dict g){
struct Cyc_Dict_Dict _tmp32=Cyc_Graph_empty(g.rel);
struct Cyc_List_List*_tmp33=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);
for(0;(unsigned int)_tmp33;_tmp33=_tmp33->tl){
struct _tuple0*_tmp34=(struct _tuple0*)_tmp33->hd;struct _tuple0*_tmp35=_tmp34;void*_tmp39;struct Cyc_Set_Set*_tmp38;_LL1: _tmp39=_tmp35->f1;_tmp38=_tmp35->f2;_LL2:;{
struct Cyc_List_List*_tmp36=Cyc_Graph_to_list(_tmp38);
if((unsigned int)_tmp36){
struct Cyc_List_List*_tmp37=_tmp36;for(0;(unsigned int)_tmp37;_tmp37=_tmp37->tl){
_tmp32=Cyc_Graph_add_edgeTc(_tmp32,_tmp39,_tmp37->hd);}}else{
_tmp32=Cyc_Graph_add_node(_tmp32,_tmp39);}};}
# 191
return _tmp32;}struct Cyc_Graph_nodestate{void*root;int C;int visitindex;};struct Cyc_Graph_componentstate{struct Cyc_Set_Set*Succ;struct Cyc_Set_Set*nodes;};struct Cyc_Graph_tcstate{struct Cyc_Set_Set*visited;int visitindex;struct Cyc_Dict_Dict ns;struct Cyc_Dict_Dict cs;int Cindex;struct Cyc_List_List*nstack;struct Cyc_List_List*cstack;};
# 217
static void Cyc_Graph_comp_tc(struct Cyc_Graph_tcstate*ts,struct Cyc_Dict_Dict g,void*v){
# 219
int(*_tmp3A)(void*,void*)=g.rel;
({struct Cyc_Set_Set*_tmpA6=Cyc_Set_insert(ts->visited,v);ts->visited=_tmpA6;});{
struct Cyc_Graph_nodestate*_tmp3B=({struct Cyc_Graph_nodestate*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->root=v,_tmp4F->C=0,_tmp4F->visitindex=ts->visitindex ++;_tmp4F;});
({struct Cyc_Dict_Dict _tmpA7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Graph_nodestate*v))Cyc_Dict_insert)(ts->ns,v,_tmp3B);ts->ns=_tmpA7;});
({struct Cyc_List_List*_tmpA8=({struct Cyc_List_List*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->hd=v,_tmp3C->tl=ts->nstack;_tmp3C;});ts->nstack=_tmpA8;});{
int _tmp3D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts->cstack);
struct Cyc_Set_Set*_tmp3E=Cyc_Graph_get_targets(g,v);
while(!Cyc_Set_is_empty(_tmp3E)){
void*_tmp3F=Cyc_Set_choose(_tmp3E);
_tmp3E=Cyc_Set_delete(_tmp3E,_tmp3F);
if(_tmp3A(v,_tmp3F)== 0)continue;{
int is_forward_edge;
struct Cyc_Graph_nodestate*nsw;
if(Cyc_Set_member(ts->visited,_tmp3F)){
nsw=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp3F);
is_forward_edge=nsw->visitindex < _tmp3B->visitindex?0: 1;}else{
# 237
is_forward_edge=0;
Cyc_Graph_comp_tc(ts,g,_tmp3F);
nsw=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp3F);}{
# 241
int _tmp40=nsw->C;
if(_tmp40 == 0){
struct Cyc_Graph_nodestate*_tmp41=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp3B->root);
struct Cyc_Graph_nodestate*_tmp42=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,nsw->root);
# 246
if(_tmp41->visitindex > _tmp42->visitindex)
_tmp3B->root=nsw->root;}else{
# 250
if(!is_forward_edge)
({struct Cyc_List_List*_tmpA9=({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->hd=(void*)_tmp40,_tmp43->tl=ts->cstack;_tmp43;});ts->cstack=_tmpA9;});}};};}
# 253
if(_tmp3A(_tmp3B->root,v)!= 0)
return;{
# 256
int _tmp44=ts->Cindex ++;
struct Cyc_Graph_componentstate*_tmp45=({struct Cyc_Graph_componentstate*_tmp4E=_cycalloc(sizeof(*_tmp4E));({struct Cyc_Set_Set*_tmpAB=((struct Cyc_Set_Set*(*)(int(*cmp)(int,int)))Cyc_Set_empty)(Cyc_Core_intcmp);_tmp4E->Succ=_tmpAB;}),({
struct Cyc_Set_Set*_tmpAA=Cyc_Set_empty(_tmp3A);_tmp4E->nodes=_tmpAA;});_tmp4E;});
({struct Cyc_Dict_Dict _tmpAC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,int k,struct Cyc_Graph_componentstate*v))Cyc_Dict_insert)(ts->cs,_tmp44,_tmp45);ts->cs=_tmpAC;});
{struct Cyc_List_List*_tmp46=ts->nstack;struct Cyc_List_List*_tmp47=_tmp46;void*_tmp48;if(_tmp47 == 0){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp48=(void*)_tmp47->hd;_LL4:
# 263
 if(_tmp3A(_tmp48,v)!= 0  || 
Cyc_Graph_is_edge(g,v,v))
({struct Cyc_Set_Set*_tmpAD=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_insert)(_tmp45->Succ,_tmp44);_tmp45->Succ=_tmpAD;});
# 267
goto _LL0;}_LL0:;}
# 269
{int _tmp49=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts->cstack);for(0;_tmp49 > _tmp3D;-- _tmp49){
int _tmp4A=(int)((struct Cyc_List_List*)_check_null(ts->cstack))->hd;
ts->cstack=((struct Cyc_List_List*)_check_null(ts->cstack))->tl;
if(!((int(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_member)(_tmp45->Succ,_tmp4A)){
struct Cyc_Set_Set*_tmp4B=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_insert)(_tmp45->Succ,_tmp4A);
_tmp4B=({struct Cyc_Set_Set*_tmpAE=_tmp4B;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmpAE,(((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict d,int k))Cyc_Dict_lookup)(ts->cs,_tmp4A))->Succ);});
_tmp45->Succ=_tmp4B;}}}
# 278
while((unsigned int)ts->nstack){
void*_tmp4C=((struct Cyc_List_List*)_check_null(ts->nstack))->hd;
ts->nstack=((struct Cyc_List_List*)_check_null(ts->nstack))->tl;{
struct Cyc_Graph_nodestate*_tmp4D=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp4C);
_tmp4D->C=_tmp44;
({struct Cyc_Set_Set*_tmpAF=Cyc_Set_insert(_tmp45->nodes,_tmp4C);_tmp45->nodes=_tmpAF;});
if(_tmp3A(_tmp4C,v)== 0)break;};}};};};}
# 287
struct Cyc_Dict_Dict Cyc_Graph_tc(struct Cyc_Dict_Dict g){
int(*_tmp50)(void*,void*)=g.rel;
struct Cyc_Graph_tcstate*_tmp51=({struct Cyc_Graph_tcstate*_tmp5F=_cycalloc(sizeof(*_tmp5F));
({
struct Cyc_Set_Set*_tmpB2=Cyc_Set_empty(_tmp50);_tmp5F->visited=_tmpB2;}),_tmp5F->visitindex=1,({
# 293
struct Cyc_Dict_Dict _tmpB1=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(_tmp50);_tmp5F->ns=_tmpB1;}),({
struct Cyc_Dict_Dict _tmpB0=((struct Cyc_Dict_Dict(*)(int(*cmp)(int,int)))Cyc_Dict_empty)(Cyc_Core_intcmp);_tmp5F->cs=_tmpB0;}),_tmp5F->Cindex=1,_tmp5F->nstack=0,_tmp5F->cstack=0;_tmp5F;});
# 299
{struct Cyc_List_List*_tmp52=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);for(0;(unsigned int)_tmp52;_tmp52=_tmp52->tl){
struct _tuple0*_tmp53=(struct _tuple0*)_tmp52->hd;struct _tuple0*_tmp54=_tmp53;void*_tmp56;struct Cyc_Set_Set*_tmp55;_LL1: _tmp56=_tmp54->f1;_tmp55=_tmp54->f2;_LL2:;
if(!Cyc_Set_member(_tmp51->visited,_tmp56))Cyc_Graph_comp_tc(_tmp51,g,_tmp56);}}{
# 304
struct Cyc_Dict_Dict _tmp57=Cyc_Graph_empty(_tmp50);
{int _tmp58=1;for(0;_tmp58 < _tmp51->Cindex;++ _tmp58){
struct Cyc_Graph_componentstate*_tmp59=((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict d,int k))Cyc_Dict_lookup)(_tmp51->cs,_tmp58);
struct Cyc_Set_Set*_tmp5A=Cyc_Set_empty(_tmp50);
struct Cyc_Set_Set*_tmp5B=_tmp59->Succ;
while(!((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp5B)){
int _tmp5C=((int(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp5B);
_tmp5B=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_delete)(_tmp5B,_tmp5C);
_tmp5A=({struct Cyc_Set_Set*_tmpB3=_tmp5A;Cyc_Set_union_two(_tmpB3,(
((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict d,int k))Cyc_Dict_lookup)(_tmp51->cs,_tmp5C))->nodes);});}{
# 315
struct Cyc_Set_Set*_tmp5D=_tmp59->nodes;
while(!Cyc_Set_is_empty(_tmp5D)){
void*_tmp5E=Cyc_Set_choose(_tmp5D);
_tmp5D=Cyc_Set_delete(_tmp5D,_tmp5E);
_tmp57=Cyc_Graph_add_edges(_tmp57,_tmp5E,_tmp5A);}};}}
# 322
return _tmp57;};}
# 335 "graph.cyc"
static void Cyc_Graph_traverse(struct Cyc_Dict_Dict input,struct Cyc_Dict_Dict*output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned int k){
# 339
({struct Cyc_List_List*_tmpB4=({struct Cyc_List_List*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->hd=x,_tmp60->tl=*S;_tmp60;});*S=_tmpB4;});
({struct Cyc_Dict_Dict _tmpB5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,k);*N=_tmpB5;});
{struct Cyc_Set_Set*_tmp61=Cyc_Graph_get_targets(input,x);for(0;!Cyc_Set_is_empty(_tmp61);0){
void*_tmp62=Cyc_Set_choose(_tmp61);
_tmp61=Cyc_Set_delete(_tmp61,_tmp62);
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp62)== (unsigned int)0)
Cyc_Graph_traverse(input,output,N,S,_tmp62,k + (unsigned int)1);{
unsigned int _tmp63=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp62);
if(({unsigned int _tmpB6=_tmp63;_tmpB6 < ((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);}))
({struct Cyc_Dict_Dict _tmpB7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,_tmp63);*N=_tmpB7;});};}}
# 350
if(({unsigned int _tmpB8=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);_tmpB8 == k;})){
int(*_tmp64)(void*,void*)=(*output).rel;
struct Cyc_Set_Set*_tmp65=Cyc_Set_empty(_tmp64);
{struct Cyc_List_List*_tmp66=*S;for(0;(unsigned int)_tmp66;_tmp66=_tmp66->tl){
void*_tmp67=_tmp66->hd;
_tmp65=Cyc_Set_insert(_tmp65,_tmp67);
if(_tmp64(_tmp67,x)== 0)break;}}
# 358
for(0;(unsigned int)*S;*S=((struct Cyc_List_List*)_check_null(*S))->tl){
void*_tmp68=((struct Cyc_List_List*)_check_null(*S))->hd;
({struct Cyc_Dict_Dict _tmpB9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,_tmp68,- 1U);*N=_tmpB9;});
({struct Cyc_Dict_Dict _tmpBA=Cyc_Graph_add_node(*output,_tmp68);*output=_tmpBA;});
({struct Cyc_Dict_Dict _tmpBB=Cyc_Graph_add_edges(*output,_tmp68,_tmp65);*output=_tmpBB;});
if(_tmp64(_tmp68,x)== 0){
*S=((struct Cyc_List_List*)_check_null(*S))->tl;
break;}}}}
# 370
struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict input){
int(*_tmp69)(void*,void*)=input.rel;
struct Cyc_List_List*_tmp6A=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(input);
struct Cyc_Dict_Dict _tmp6B=Cyc_Graph_empty(_tmp69);
struct Cyc_List_List*_tmp6C=0;
struct Cyc_Dict_Dict N=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(_tmp69);
{struct Cyc_List_List*_tmp6D=_tmp6A;for(0;(unsigned int)_tmp6D;_tmp6D=_tmp6D->tl){
struct _tuple0*_tmp6E=(struct _tuple0*)_tmp6D->hd;struct _tuple0*_tmp6F=_tmp6E;void*_tmp70;_LL1: _tmp70=_tmp6F->f1;_LL2:;
N=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(N,_tmp70,0U);}}
# 380
{struct Cyc_List_List*_tmp71=_tmp6A;for(0;(unsigned int)_tmp71;_tmp71=_tmp71->tl){
struct _tuple0*_tmp72=(struct _tuple0*)_tmp71->hd;struct _tuple0*_tmp73=_tmp72;void*_tmp74;_LL4: _tmp74=_tmp73->f1;_LL5:;
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(N,_tmp74)== (unsigned int)0)
Cyc_Graph_traverse(input,& _tmp6B,& N,& _tmp6C,_tmp74,1U);}}
# 385
return _tmp6B;}
# 389
static void Cyc_Graph_tsort0(struct Cyc_Dict_Dict input,struct Cyc_List_List**output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned int k){
# 393
({struct Cyc_List_List*_tmpBC=({struct Cyc_List_List*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->hd=x,_tmp75->tl=*S;_tmp75;});*S=_tmpBC;});
({struct Cyc_Dict_Dict _tmpBD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,k);*N=_tmpBD;});
{struct Cyc_Set_Set*_tmp76=Cyc_Graph_get_targets(input,x);for(0;!Cyc_Set_is_empty(_tmp76);0){
void*_tmp77=Cyc_Set_choose(_tmp76);
_tmp76=Cyc_Set_delete(_tmp76,_tmp77);
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp77)== (unsigned int)0)
Cyc_Graph_tsort0(input,output,N,S,_tmp77,k + (unsigned int)1);{
unsigned int _tmp78=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp77);
if(({unsigned int _tmpBE=_tmp78;_tmpBE < ((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);}))
({struct Cyc_Dict_Dict _tmpBF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,_tmp78);*N=_tmpBF;});};}}
# 404
if(({unsigned int _tmpC0=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);_tmpC0 == k;})){
int(*_tmp79)(void*,void*)=input.rel;
struct Cyc_Set_Set*_tmp7A=Cyc_Set_empty(_tmp79);
{struct Cyc_List_List*_tmp7B=*S;for(0;(unsigned int)_tmp7B;_tmp7B=_tmp7B->tl){
void*_tmp7C=_tmp7B->hd;
_tmp7A=Cyc_Set_insert(_tmp7A,_tmp7C);
if(_tmp79(_tmp7C,x)== 0)break;}}
# 412
for(0;(unsigned int)*S;*S=((struct Cyc_List_List*)_check_null(*S))->tl){
void*_tmp7D=((struct Cyc_List_List*)_check_null(*S))->hd;
({struct Cyc_Dict_Dict _tmpC1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,_tmp7D,- 1U);*N=_tmpC1;});
({struct Cyc_List_List*_tmpC2=({struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->hd=_tmp7D,_tmp7E->tl=*output;_tmp7E;});*output=_tmpC2;});
if(_tmp79(_tmp7D,x)== 0){
*S=((struct Cyc_List_List*)_check_null(*S))->tl;
break;}}}}
# 423
struct Cyc_List_List*Cyc_Graph_tsort(struct Cyc_Dict_Dict input){
int(*_tmp7F)(void*,void*)=input.rel;
struct Cyc_List_List*_tmp80=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(input);
struct Cyc_List_List*_tmp81=0;
struct Cyc_List_List*_tmp82=0;
struct Cyc_Dict_Dict N=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(_tmp7F);
{struct Cyc_List_List*_tmp83=_tmp80;for(0;(unsigned int)_tmp83;_tmp83=_tmp83->tl){
struct _tuple0*_tmp84=(struct _tuple0*)_tmp83->hd;struct _tuple0*_tmp85=_tmp84;void*_tmp86;_LL1: _tmp86=_tmp85->f1;_LL2:;
N=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(N,_tmp86,0U);}}
# 433
{struct Cyc_List_List*_tmp87=_tmp80;for(0;(unsigned int)_tmp87;_tmp87=_tmp87->tl){
struct _tuple0*_tmp88=(struct _tuple0*)_tmp87->hd;struct _tuple0*_tmp89=_tmp88;void*_tmp8A;_LL4: _tmp8A=_tmp89->f1;_LL5:;
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(N,_tmp8A)== (unsigned int)0)
Cyc_Graph_tsort0(input,& _tmp81,& N,& _tmp82,_tmp8A,1U);}}
# 438
return _tmp81;}

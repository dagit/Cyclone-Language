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

//extern struct _xtunion_struct *_exn_thrown;
extern struct _xtunion_struct* Cyc_Core_get_exn_thrown();
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
# 165 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 960 "absyn.h"
void*Cyc_Absyn_bounds_one();
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple10{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple10 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_RgnOrder_RgnPO;
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 35 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr,struct _dyneither_ptr);
# 54
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
# 76
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 79
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 89
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 112
void*Cyc_Tcutil_compress(void*t);
# 346 "tcutil.h"
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 360
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 32 "evexp.h"
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 42 "evexp.h"
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
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
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 34 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
extern struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullOnly_val;
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;
extern struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_FatBound_val;
# 39
extern int Cyc_InsertChecks_warn_bounds_checks;
extern int Cyc_InsertChecks_warn_all_null_deref;
# 42
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*);char Cyc_InsertChecks_FatBound[9U]="FatBound";char Cyc_InsertChecks_NoCheck[8U]="NoCheck";char Cyc_InsertChecks_NullAndFatBound[16U]="NullAndFatBound";char Cyc_InsertChecks_NullAndThinBound[17U]="NullAndThinBound";char Cyc_InsertChecks_NullOnly[9U]="NullOnly";char Cyc_InsertChecks_ThinBound[10U]="ThinBound";
# 61 "insert_checks.cyc"
struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val={Cyc_InsertChecks_NoCheck};
struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullOnly_val={Cyc_InsertChecks_NullOnly};
struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val={Cyc_InsertChecks_NullAndFatBound};
struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_FatBound_val={Cyc_InsertChecks_FatBound};
# 67
int Cyc_InsertChecks_warn_bounds_checks=0;
int Cyc_InsertChecks_warn_all_null_deref=0;
unsigned int Cyc_InsertChecks_total_bounds_checks=0U;
unsigned int Cyc_InsertChecks_bounds_checks_eliminated=0U;
# 72
static struct Cyc_Absyn_Exp*Cyc_InsertChecks_shared_zero_exp(){
static struct Cyc_Absyn_Exp*ans=0;
if(!((unsigned int)ans))
ans=Cyc_Absyn_uint_exp(0U,0U);
return(struct Cyc_Absyn_Exp*)_check_null(ans);}struct Cyc_InsertChecks_Env{int in_sizeof: 1;int at_toplevel: 1;};
# 84
static struct Cyc_InsertChecks_Env Cyc_InsertChecks_toplevel_env(){return({struct Cyc_InsertChecks_Env _tmpA1;_tmpA1.in_sizeof=0,_tmpA1.at_toplevel=1;_tmpA1;});}
static struct Cyc_InsertChecks_Env Cyc_InsertChecks_fn_body_env(){return({struct Cyc_InsertChecks_Env _tmpA2;_tmpA2.in_sizeof=0,_tmpA2.at_toplevel=0;_tmpA2;});}
static struct Cyc_InsertChecks_Env Cyc_InsertChecks_enter_sizeof(struct Cyc_InsertChecks_Env nv){return({struct Cyc_InsertChecks_Env _tmpA3;_tmpA3.in_sizeof=1,_tmpA3.at_toplevel=nv.at_toplevel;_tmpA3;});}
# 90
static int Cyc_InsertChecks_need_null_check(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*e){
if(nv.in_sizeof)
return 0;
# 94
{void*_tmp0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp1=_tmp0;void*_tmp4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1)->tag == 3U){_LL1: _tmp4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1)->f1).ptr_atts).nullable;_LL2:
# 96
 if(!Cyc_Tcutil_force_type2bool(0,_tmp4))
return 0;
goto _LL0;}else{_LL3: _LL4:
({void*_tmp2=0U;({unsigned int _tmpA7=e->loc;struct _dyneither_ptr _tmpA6=({const char*_tmp3="need_null_check: non-pointer type";_tag_dyneither(_tmp3,sizeof(char),34U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpA7,_tmpA6,_tag_dyneither(_tmp2,sizeof(void*),0U));});});}_LL0:;}
# 101
{void*_tmp5=e->annot;void*_tmp6=_tmp5;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp6)->tag == Cyc_CfFlowInfo_UnknownZ){_LL6: _LL7:
 goto _LL5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp6)->tag == Cyc_CfFlowInfo_NotZero){_LL8: _LL9:
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp6)->tag == Cyc_CfFlowInfo_IsZero){_LLA: _LLB:
# 105
({void*_tmp7=0U;({unsigned int _tmpA9=e->loc;struct _dyneither_ptr _tmpA8=({const char*_tmp8="NULL pointer check will definitely fail";_tag_dyneither(_tmp8,sizeof(char),40U);});Cyc_Warn_err(_tmpA9,_tmpA8,_tag_dyneither(_tmp7,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp6)->tag == Cyc_Absyn_EmptyAnnot){_LLC: _LLD:
# 108
 return 0;}else{_LLE: _LLF:
({void*_tmp9=0U;({unsigned int _tmpAB=e->loc;struct _dyneither_ptr _tmpAA=({const char*_tmpA="need_null_check: unexpected annotation";_tag_dyneither(_tmpA,sizeof(char),39U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpAB,_tmpAA,_tag_dyneither(_tmp9,sizeof(void*),0U));});});}}}}_LL5:;}
# 111
if(nv.at_toplevel){
({void*_tmpB=0U;({unsigned int _tmpAD=e->loc;struct _dyneither_ptr _tmpAC=({const char*_tmpC="cannot perform NULL-pointer check at toplevel";_tag_dyneither(_tmpC,sizeof(char),46U);});Cyc_Warn_err(_tmpAD,_tmpAC,_tag_dyneither(_tmpB,sizeof(void*),0U));});});
return 0;}
# 115
if(Cyc_InsertChecks_warn_all_null_deref)
({void*_tmpD=0U;({unsigned int _tmpAF=e->loc;struct _dyneither_ptr _tmpAE=({const char*_tmpE="inserted null check";_tag_dyneither(_tmpE,sizeof(char),20U);});Cyc_Tcutil_warn(_tmpAF,_tmpAE,_tag_dyneither(_tmpD,sizeof(void*),0U));});});
return 1;}
# 120
static struct Cyc_List_List*Cyc_InsertChecks_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->annot;void*_tmp10=_tmpF;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp13;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp10)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp13=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp10)->f1;_LL2:
 return _tmp13;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp10)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp14=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp10)->f1;_LL4:
 return _tmp14;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp10)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
 return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp10)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp11=0U;({unsigned int _tmpB1=e->loc;struct _dyneither_ptr _tmpB0=({const char*_tmp12="get_relns: unexpected annotation";_tag_dyneither(_tmp12,sizeof(char),33U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpB1,_tmpB0,_tag_dyneither(_tmp11,sizeof(void*),0U));});});}}}}_LL0:;}
# 133
static int Cyc_InsertChecks_need_bounds_check(struct Cyc_InsertChecks_Env nv,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
if(nv.in_sizeof)
return 0;
if(nv.at_toplevel)
({void*_tmp15=0U;({unsigned int _tmpB3=a->loc;struct _dyneither_ptr _tmpB2=({const char*_tmp16="InsertChecks: memory read at top-level";_tag_dyneither(_tmp16,sizeof(char),39U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpB3,_tmpB2,_tag_dyneither(_tmp15,sizeof(void*),0U));});});
++ Cyc_InsertChecks_total_bounds_checks;{
# 145
int bd_valid=0;union Cyc_Relations_RelnOp rop_bd=Cyc_Relations_RConst(0U);
int a_valid=0;union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RConst(0U);
int i_valid=0;union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int z_valid=0;union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
# 151
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(a->topt));
if(bound != 0)
bd_valid=Cyc_Relations_exp2relnop(bound,& rop_bd);
# 155
{void*_tmp17=a->r;void*_tmp18=_tmp17;void*_tmp1A;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp18)->tag == 1U){_LL1: _tmp1A=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp18)->f1;_LL2: {
# 157
struct Cyc_Absyn_Vardecl*_tmp19=Cyc_Tcutil_nonesc_vardecl(_tmp1A);
if(_tmp19 != 0){
a_valid=1;
rop_a=Cyc_Relations_RNumelts(_tmp19);}
# 162
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 166
i_valid=Cyc_Relations_exp2relnop(i,& rop_i);
# 168
if(!i_valid){
inner_loop: {
void*_tmp1B=i->r;void*_tmp1C=_tmp1B;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;void*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;switch(*((int*)_tmp1C)){case 14U: _LL6: _tmp1E=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1C)->f1;_tmp1D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1C)->f2;_LL7:
 i=_tmp1D;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1C)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1C)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1C)->f2)->tl != 0){_LL8: _tmp20=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1C)->f2)->hd;_tmp1F=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1C)->f2)->tl)->hd;_LL9:
# 173
 z_valid=Cyc_Relations_exp2relnop(_tmp1F,& rop_z);goto _LL5;}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}}{
# 179
union Cyc_Relations_RelnOp rop_left;
if(!bd_valid  && !a_valid)
return 1;
if(bd_valid  && a_valid){
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bd,Cyc_Relations_Rlte,rop_a,relns);
rop_left=rop_a;}else{
if(a_valid)
rop_left=rop_a;else{
# 188
rop_left=rop_bd;}}
# 190
if(i_valid)relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_left,Cyc_Relations_Rlte,rop_i,relns);else{
if(z_valid)relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_left,Cyc_Relations_Rlt,rop_z,relns);else{
return 1;}}{
# 198
int ans=Cyc_Relations_consistent_relations(relns);
if(!ans)
++ Cyc_InsertChecks_bounds_checks_eliminated;
if(Cyc_InsertChecks_warn_bounds_checks  && ans)
({void*_tmp21=0U;({unsigned int _tmpB5=a->loc;struct _dyneither_ptr _tmpB4=({const char*_tmp22="inserted bounds check (location is pointer expression)";_tag_dyneither(_tmp22,sizeof(char),55U);});Cyc_Tcutil_warn(_tmpB5,_tmpB4,_tag_dyneither(_tmp21,sizeof(void*),0U));});});
return ans;};};};}
# 206
static void Cyc_InsertChecks_do_stmt(struct Cyc_InsertChecks_Env,struct Cyc_Absyn_Stmt*);
static void Cyc_InsertChecks_do_exp(struct Cyc_InsertChecks_Env,struct Cyc_Absyn_Exp*);
# 209
static void Cyc_InsertChecks_do_exp_opt(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*eo){
if(eo != 0)
Cyc_InsertChecks_do_exp(nv,eo);}struct _tuple11{void*f1;void*f2;};struct _tuple12{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 214
static void Cyc_InsertChecks_do_exp(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*e){
void*_tmp23=e->r;void*_tmp24=_tmp23;struct Cyc_Absyn_Stmt*_tmp7D;struct Cyc_List_List*_tmp7C;struct Cyc_List_List*_tmp7B;struct Cyc_List_List*_tmp7A;struct Cyc_Absyn_Exp*_tmp79;struct Cyc_Absyn_Exp*_tmp78;struct Cyc_Absyn_Exp*_tmp77;struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_Absyn_Exp*_tmp73;struct Cyc_Absyn_Exp*_tmp72;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_Absyn_Exp*_tmp6D;struct Cyc_Absyn_Exp*_tmp6C;struct Cyc_Absyn_Exp*_tmp6B;struct Cyc_Absyn_Exp*_tmp6A;struct Cyc_Absyn_Exp*_tmp69;struct Cyc_Absyn_Exp*_tmp68;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_Absyn_Exp*_tmp66;struct Cyc_Absyn_Exp*_tmp65;void*_tmp64;struct Cyc_Absyn_Exp*_tmp63;int _tmp62;enum Cyc_Absyn_Coercion _tmp61;struct Cyc_Absyn_Exp*_tmp60;struct Cyc_List_List*_tmp5F;struct Cyc_Absyn_Exp*_tmp5E;enum Cyc_Absyn_Incrementor _tmp5D;struct Cyc_Absyn_Exp*_tmp5C;struct Cyc_Absyn_Exp*_tmp5B;struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_Absyn_Exp*_tmp59;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp4F;switch(*((int*)_tmp24)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL0;case 31U: _LL7: _tmp4F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL8:
# 220
 _tmp50=_tmp4F;goto _LLA;case 24U: _LL9: _tmp50=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LLA:
 _tmp51=_tmp50;goto _LLC;case 3U: _LLB: _tmp51=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LLC:
# 223
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
for(0;_tmp51 != 0;_tmp51=_tmp51->tl){
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)_tmp51->hd);}
goto _LL0;case 4U: _LLD: _tmp53=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp52=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp24)->f3;_LLE:
# 230
 _tmp55=_tmp53;_tmp54=_tmp52;goto _LL10;case 9U: _LLF: _tmp55=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp54=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LL10:
 _tmp57=_tmp55;_tmp56=_tmp54;goto _LL12;case 7U: _LL11: _tmp57=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp56=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LL12:
 _tmp59=_tmp57;_tmp58=_tmp56;goto _LL14;case 8U: _LL13: _tmp59=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp58=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LL14:
# 234
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,_tmp59);
Cyc_InsertChecks_do_exp(nv,_tmp58);
goto _LL0;case 6U: _LL15: _tmp5C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp5B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_tmp5A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp24)->f3;_LL16:
# 239
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,_tmp5C);
Cyc_InsertChecks_do_exp(nv,_tmp5B);
Cyc_InsertChecks_do_exp(nv,_tmp5A);
goto _LL0;case 5U: _LL17: _tmp5E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp5D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LL18:
# 250
 if((Cyc_Tcutil_get_type_bound((void*)_check_null(_tmp5E->topt))!= 0  && 
Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(_tmp5E->topt))) && 
Cyc_InsertChecks_need_null_check(nv,_tmp5E))
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;else{
# 255
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
Cyc_InsertChecks_do_exp(nv,_tmp5E);
goto _LL0;case 10U: _LL19: _tmp60=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp5F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LL1A:
# 260
 if(Cyc_InsertChecks_need_null_check(nv,_tmp60))
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;else{
# 263
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
Cyc_InsertChecks_do_exp(nv,_tmp60);
for(0;_tmp5F != 0;_tmp5F=_tmp5F->tl){
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)_tmp5F->hd);}
goto _LL0;case 14U: _LL1B: _tmp64=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp63=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_tmp62=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp24)->f3;_tmp61=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp24)->f4;_LL1C:
# 272
{struct _tuple11 _tmp25=({struct _tuple11 _tmpA5;({void*_tmpB7=Cyc_Tcutil_compress((void*)_check_null(_tmp63->topt));_tmpA5.f1=_tmpB7;}),({void*_tmpB6=Cyc_Tcutil_compress(_tmp64);_tmpA5.f2=_tmpB6;});_tmpA5;});struct _tuple11 _tmp26=_tmp25;struct Cyc_Absyn_PtrInfo _tmp3E;struct Cyc_Absyn_PtrInfo _tmp3D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26.f2)->tag == 3U){_LL56: _tmp3E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26.f1)->f1;_tmp3D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26.f2)->f1;_LL57: {
# 274
int _tmp27=Cyc_Tcutil_force_type2bool(0,(_tmp3E.ptr_atts).nullable);
int _tmp28=Cyc_Tcutil_force_type2bool(0,(_tmp3D.ptr_atts).nullable);
struct Cyc_Absyn_Exp*_tmp29=({void*_tmpB8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB8,(_tmp3E.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp2A=({void*_tmpB9=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB9,(_tmp3D.ptr_atts).bounds);});
if((_tmp27  && !_tmp28) && (int)_tmp61 != (int)2U)
({void*_tmp2B=0U;({unsigned int _tmpBB=e->loc;struct _dyneither_ptr _tmpBA=({const char*_tmp2C="null-check conversion mis-classified";_tag_dyneither(_tmp2C,sizeof(char),37U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpBB,_tmpBA,_tag_dyneither(_tmp2B,sizeof(void*),0U));});});else{
if(((unsigned int)_tmp29  && !((unsigned int)_tmp2A)) && (int)_tmp61 == (int)2U)
({void*_tmp2D=0U;({unsigned int _tmpBD=e->loc;struct _dyneither_ptr _tmpBC=({const char*_tmp2E="conversion mis-classified as null-check";_tag_dyneither(_tmp2E,sizeof(char),40U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpBD,_tmpBC,_tag_dyneither(_tmp2D,sizeof(void*),0U));});});}
{struct _tuple12 _tmp2F=({struct _tuple12 _tmpA4;_tmpA4.f1=_tmp29,_tmpA4.f2=_tmp2A;_tmpA4;});struct _tuple12 _tmp30=_tmp2F;if(_tmp30.f1 != 0){if(_tmp30.f2 != 0){_LL5B: _LL5C:
# 284
 if(!({struct Cyc_Absyn_Exp*_tmpBE=(struct Cyc_Absyn_Exp*)_check_null(_tmp2A);Cyc_Evexp_lte_const_exp(_tmpBE,(struct Cyc_Absyn_Exp*)_check_null(_tmp29));}))
({void*_tmp31=0U;({unsigned int _tmpC0=e->loc;struct _dyneither_ptr _tmpBF=({const char*_tmp32="InsertChecks: cast to maybe-shorter pointer type";_tag_dyneither(_tmp32,sizeof(char),49U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpC0,_tmpBF,_tag_dyneither(_tmp31,sizeof(void*),0U));});});
if((_tmp27  && !_tmp28) && Cyc_InsertChecks_need_null_check(nv,_tmp63)){
if(!_tmp62)
({void*_tmp33=0U;({unsigned int _tmpC2=e->loc;struct _dyneither_ptr _tmpC1=({const char*_tmp34="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp34,sizeof(char),58U);});Cyc_Tcutil_warn(_tmpC2,_tmpC1,_tag_dyneither(_tmp33,sizeof(void*),0U));});});
# 290
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;}else{
# 292
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
goto _LL5A;}else{_LL5D: _LL5E:
# 296
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp29)))
({void*_tmp35=0U;({unsigned int _tmpC4=e->loc;struct _dyneither_ptr _tmpC3=({const char*_tmp36="cannot perform coercion: numelts is statically unknown";_tag_dyneither(_tmp36,sizeof(char),55U);});Cyc_Warn_err(_tmpC4,_tmpC3,_tag_dyneither(_tmp35,sizeof(void*),0U));});});
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
goto _LL5A;}}else{if(_tmp30.f2 != 0){_LL5F: _LL60:
# 301
 if(nv.at_toplevel)
({void*_tmp37=0U;({unsigned int _tmpC6=e->loc;struct _dyneither_ptr _tmpC5=({const char*_tmp38="cannot coerce fat-pointer to thin-pointer at toplevel";_tag_dyneither(_tmp38,sizeof(char),54U);});Cyc_Warn_err(_tmpC6,_tmpC5,_tag_dyneither(_tmp37,sizeof(void*),0U));});});
# 304
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp2A)))
({void*_tmp39=0U;({unsigned int _tmpC8=e->loc;struct _dyneither_ptr _tmpC7=({const char*_tmp3A="cannot perform coercion: numelts is statically unknown";_tag_dyneither(_tmp3A,sizeof(char),55U);});Cyc_Warn_err(_tmpC8,_tmpC7,_tag_dyneither(_tmp39,sizeof(void*),0U));});});
# 308
++ Cyc_InsertChecks_total_bounds_checks;{
int bds_chk=0;
if(!_tmp28  && Cyc_InsertChecks_need_null_check(nv,_tmp63)){
if(!_tmp62)
({void*_tmp3B=0U;({unsigned int _tmpCA=e->loc;struct _dyneither_ptr _tmpC9=({const char*_tmp3C="inserted null check due to implicit cast from *@fat (?) to @ type";_tag_dyneither(_tmp3C,sizeof(char),66U);});Cyc_Tcutil_warn(_tmpCA,_tmpC9,_tag_dyneither(_tmp3B,sizeof(void*),0U));});});
# 314
e->annot=bds_chk?(void*)& Cyc_InsertChecks_NullAndFatBound_val:(void*)& Cyc_InsertChecks_NullOnly_val;}else{
# 316
e->annot=bds_chk?(void*)& Cyc_InsertChecks_FatBound_val:(void*)& Cyc_InsertChecks_NoCheck_val;}
goto _LL5A;};}else{_LL61: _LL62:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL5A;}}_LL5A:;}
# 320
goto _LL55;}}else{goto _LL58;}}else{_LL58: _LL59:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL55;}_LL55:;}
# 323
Cyc_InsertChecks_do_exp(nv,_tmp63);
goto _LL0;case 22U: _LL1D: _tmp65=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL1E:
# 326
 _tmp66=_tmp65;goto _LL20;case 20U: _LL1F: _tmp66=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL20:
 _tmp68=_tmp66;_tmp67=Cyc_InsertChecks_shared_zero_exp();goto _LL22;case 23U: _LL21: _tmp68=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp67=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LL22:
# 329
{void*_tmp3F=Cyc_Tcutil_compress((void*)_check_null(_tmp68->topt));void*_tmp40=_tmp3F;void*_tmp4C;struct Cyc_Absyn_Tqual _tmp4B;void*_tmp4A;void*_tmp49;void*_tmp48;void*_tmp47;switch(*((int*)_tmp40)){case 6U: _LL64: _LL65:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL63;case 3U: _LL66: _tmp4C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40)->f1).elt_type;_tmp4B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40)->f1).elt_tq;_tmp4A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40)->f1).ptr_atts).rgn;_tmp49=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40)->f1).ptr_atts).nullable;_tmp48=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40)->f1).ptr_atts).bounds;_tmp47=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40)->f1).ptr_atts).zero_term;_LL67: {
# 332
int null_ck=Cyc_InsertChecks_need_null_check(nv,_tmp68);
int bds_ck=({struct Cyc_InsertChecks_Env _tmpCD=nv;struct Cyc_List_List*_tmpCC=Cyc_InsertChecks_get_relns(_tmp68);struct Cyc_Absyn_Exp*_tmpCB=_tmp68;Cyc_InsertChecks_need_bounds_check(_tmpCD,_tmpCC,_tmpCB,_tmp67);});
struct Cyc_Absyn_Exp*bd=({void*_tmpCE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCE,_tmp48);});
if(bds_ck){
if((unsigned int)bd){
if(!Cyc_Evexp_c_can_eval(bd))
({void*_tmp41=0U;({unsigned int _tmpD0=e->loc;struct _dyneither_ptr _tmpCF=({const char*_tmp42="cannot determine subscript is in bounds";_tag_dyneither(_tmp42,sizeof(char),40U);});Cyc_Warn_err(_tmpD0,_tmpCF,_tag_dyneither(_tmp41,sizeof(void*),0U));});});
({void*_tmpD1=
null_ck?(void*)({struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->tag=Cyc_InsertChecks_NullAndThinBound,_tmp43->f1=bd;_tmp43;}):(void*)({struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=Cyc_InsertChecks_ThinBound,_tmp44->f1=bd;_tmp44;});
# 339
e->annot=_tmpD1;});}else{
# 342
e->annot=null_ck?(void*)& Cyc_InsertChecks_NullAndFatBound_val:(void*)& Cyc_InsertChecks_FatBound_val;}}else{
if(null_ck)
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;else{
# 346
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}}
goto _LL63;}default: _LL68: _LL69:
({void*_tmp45=0U;({unsigned int _tmpD3=e->loc;struct _dyneither_ptr _tmpD2=({const char*_tmp46="InsertChecks: non-pointer-type on dereference";_tag_dyneither(_tmp46,sizeof(char),46U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpD3,_tmpD2,_tag_dyneither(_tmp45,sizeof(void*),0U));});});}_LL63:;}
# 350
Cyc_InsertChecks_do_exp(nv,_tmp68);
Cyc_InsertChecks_do_exp(nv,_tmp67);
goto _LL0;case 18U: _LL23: _tmp69=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL24:
# 355
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
({struct Cyc_InsertChecks_Env _tmpD4=Cyc_InsertChecks_enter_sizeof(nv);Cyc_InsertChecks_do_exp(_tmpD4,_tmp69);});
goto _LL0;case 16U: _LL25: _tmp6B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp6A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LL26:
# 360
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp_opt(nv,_tmp6B);
Cyc_InsertChecks_do_exp(nv,_tmp6A);
goto _LL0;case 34U: _LL27: _tmp6D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp24)->f1).rgn;_tmp6C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp24)->f1).num_elts;_LL28:
# 367
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp_opt(nv,_tmp6D);
Cyc_InsertChecks_do_exp(nv,_tmp6C);
goto _LL0;case 41U: _LL29: _tmp6E=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL2A:
# 372
 _tmp6F=_tmp6E;goto _LL2C;case 11U: _LL2B: _tmp6F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL2C:
 _tmp70=_tmp6F;goto _LL2E;case 12U: _LL2D: _tmp70=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL2E:
 _tmp71=_tmp70;goto _LL30;case 13U: _LL2F: _tmp71=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL30:
 _tmp72=_tmp71;goto _LL32;case 15U: _LL31: _tmp72=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL32:
# 377
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,_tmp72);
goto _LL0;case 32U: _LL33: _LL34:
 goto _LL36;case 33U: _LL35: _LL36:
 goto _LL38;case 17U: _LL37: _LL38:
 goto _LL3A;case 19U: _LL39: _LL3A:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL0;case 21U: _LL3B: _tmp73=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL3C:
 _tmp74=_tmp73;goto _LL3E;case 38U: _LL3D: _tmp74=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL3E:
 _tmp75=_tmp74;goto _LL40;case 28U: _LL3F: _tmp75=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL40:
# 387
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,_tmp75);
goto _LL0;case 27U: _LL41: _tmp77=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_tmp76=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp24)->f3;_LL42:
 _tmp79=_tmp77;_tmp78=_tmp76;goto _LL44;case 35U: _LL43: _tmp79=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_tmp78=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LL44:
# 392
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,_tmp79);
Cyc_InsertChecks_do_exp(nv,_tmp78);
goto _LL0;case 26U: _LL45: _tmp7A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL46:
 _tmp7B=_tmp7A;goto _LL48;case 29U: _LL47: _tmp7B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24)->f3;_LL48:
 _tmp7C=_tmp7B;goto _LL4A;case 30U: _LL49: _tmp7C=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp24)->f2;_LL4A:
# 399
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
for(0;_tmp7C != 0;_tmp7C=_tmp7C->tl){
Cyc_InsertChecks_do_exp(nv,(*((struct _tuple13*)_tmp7C->hd)).f2);}
goto _LL0;case 37U: _LL4B: _tmp7D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp24)->f1;_LL4C:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;Cyc_InsertChecks_do_stmt(nv,_tmp7D);goto _LL0;case 36U: _LL4D: _LL4E:
# 405
 goto _LL50;case 25U: _LL4F: _LL50:
 goto _LL52;case 39U: _LL51: _LL52:
 goto _LL54;default: _LL53: _LL54:
({void*_tmp4D=0U;({unsigned int _tmpD6=e->loc;struct _dyneither_ptr _tmpD5=({const char*_tmp4E="InsertChecks, unexpected exp form";_tag_dyneither(_tmp4E,sizeof(char),34U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpD6,_tmpD5,_tag_dyneither(_tmp4D,sizeof(void*),0U));});});}_LL0:;}
# 412
static void Cyc_InsertChecks_do_decl(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Decl*d){
void*_tmp7E=d->r;void*_tmp7F=_tmp7E;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp85;struct Cyc_List_List*_tmp84;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_Absyn_Fndecl*_tmp81;struct Cyc_Absyn_Vardecl*_tmp80;switch(*((int*)_tmp7F)){case 0U: _LL1: _tmp80=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7F)->f1;_LL2:
 Cyc_InsertChecks_do_exp_opt(nv,_tmp80->initializer);goto _LL0;case 1U: _LL3: _tmp81=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7F)->f1;_LL4:
({struct Cyc_InsertChecks_Env _tmpD7=Cyc_InsertChecks_fn_body_env();Cyc_InsertChecks_do_stmt(_tmpD7,_tmp81->body);});goto _LL0;case 2U: _LL5: _tmp82=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7F)->f3;_LL6:
 Cyc_InsertChecks_do_exp(nv,_tmp82);goto _LL0;case 3U: _LL7: _LL8:
 goto _LL0;case 4U: _LL9: _tmp83=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7F)->f3;_LLA:
 Cyc_InsertChecks_do_exp_opt(nv,_tmp83);goto _LL0;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 8U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
 goto _LL1C;case 12U: _LL1B: _LL1C:
 goto _LL0;case 11U: _LL1D: _tmp84=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp7F)->f1;_LL1E:
 _tmp85=_tmp84;goto _LL20;case 10U: _LL1F: _tmp85=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp7F)->f2;_LL20:
 _tmp86=_tmp85;goto _LL22;default: _LL21: _tmp86=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp7F)->f2;_LL22:
# 431
 for(0;_tmp86 != 0;_tmp86=_tmp86->tl){
Cyc_InsertChecks_do_decl(nv,(struct Cyc_Absyn_Decl*)_tmp86->hd);}}_LL0:;}
# 436
static void Cyc_InsertChecks_do_scs(struct Cyc_InsertChecks_Env nv,struct Cyc_List_List*scs){
# 439
for(0;scs != 0;scs=scs->tl){
if(((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause != 0)
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause));
Cyc_InsertChecks_do_stmt(nv,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}}
# 446
static void Cyc_InsertChecks_do_stmt(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Stmt*s){
void*_tmp87=s->r;void*_tmp88=_tmp87;struct Cyc_Absyn_Stmt*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Decl*_tmp9B;struct Cyc_Absyn_Stmt*_tmp9A;struct Cyc_Absyn_Stmt*_tmp99;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Stmt*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Stmt*_tmp92;struct Cyc_Absyn_Stmt*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Stmt*_tmp8E;struct Cyc_Absyn_Stmt*_tmp8D;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Stmt*_tmp8B;struct Cyc_Absyn_Stmt*_tmp8A;struct Cyc_Absyn_Exp*_tmp89;switch(*((int*)_tmp88)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp89=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_LL4:
 Cyc_InsertChecks_do_exp(nv,_tmp89);goto _LL0;case 2U: _LL5: _tmp8B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_tmp8A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp88)->f2;_LL6:
 Cyc_InsertChecks_do_stmt(nv,_tmp8B);Cyc_InsertChecks_do_stmt(nv,_tmp8A);goto _LL0;case 3U: _LL7: _tmp8C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_LL8:
 if((unsigned int)_tmp8C)Cyc_InsertChecks_do_exp(nv,_tmp8C);goto _LL0;case 4U: _LL9: _tmp8F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_tmp8E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp88)->f2;_tmp8D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp88)->f3;_LLA:
# 453
 Cyc_InsertChecks_do_exp(nv,_tmp8F);Cyc_InsertChecks_do_stmt(nv,_tmp8E);Cyc_InsertChecks_do_stmt(nv,_tmp8D);goto _LL0;case 14U: _LLB: _tmp91=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_tmp90=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp88)->f2).f1;_LLC:
 _tmp93=_tmp90;_tmp92=_tmp91;goto _LLE;case 5U: _LLD: _tmp93=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp88)->f1).f1;_tmp92=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp88)->f2;_LLE:
 Cyc_InsertChecks_do_exp(nv,_tmp93);Cyc_InsertChecks_do_stmt(nv,_tmp92);goto _LL0;case 9U: _LLF: _tmp97=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_tmp96=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp88)->f2).f1;_tmp95=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp88)->f3).f1;_tmp94=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp88)->f4;_LL10:
# 457
 Cyc_InsertChecks_do_exp(nv,_tmp97);Cyc_InsertChecks_do_exp(nv,_tmp96);Cyc_InsertChecks_do_exp(nv,_tmp95);Cyc_InsertChecks_do_stmt(nv,_tmp94);goto _LL0;case 6U: _LL11: _LL12:
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp98=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_LL18:
# 462
 for(0;_tmp98 != 0;_tmp98=_tmp98->tl){
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)_tmp98->hd);}
goto _LL0;case 13U: _LL19: _tmp99=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp88)->f2;_LL1A:
 Cyc_InsertChecks_do_stmt(nv,_tmp99);goto _LL0;case 12U: _LL1B: _tmp9B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_tmp9A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp88)->f2;_LL1C:
 Cyc_InsertChecks_do_decl(nv,_tmp9B);Cyc_InsertChecks_do_stmt(nv,_tmp9A);goto _LL0;case 10U: _LL1D: _tmp9D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_tmp9C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp88)->f2;_LL1E:
 Cyc_InsertChecks_do_exp(nv,_tmp9D);Cyc_InsertChecks_do_scs(nv,_tmp9C);goto _LL0;default: _LL1F: _tmp9F=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp88)->f1;_tmp9E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp88)->f2;_LL20:
 Cyc_InsertChecks_do_stmt(nv,_tmp9F);Cyc_InsertChecks_do_scs(nv,_tmp9E);goto _LL0;}_LL0:;}
# 472
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*tds){
struct Cyc_InsertChecks_Env _tmpA0=Cyc_InsertChecks_toplevel_env();
for(0;tds != 0;tds=tds->tl){
Cyc_InsertChecks_do_decl(_tmpA0,(struct Cyc_Absyn_Decl*)tds->hd);}}

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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 108 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 119
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 37 "position.h"
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 47
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 302
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 317
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 890 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 892
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 900
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 903
void*Cyc_Absyn_compress_kb(void*);
# 909
int Cyc_Absyn_type2bool(int def,void*);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 919
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 921
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 928
extern void*Cyc_Absyn_empty_effect;
# 930
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 932
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 951
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 957
extern void*Cyc_Absyn_fat_bound_type;
# 959
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 961
void*Cyc_Absyn_bounds_one();
# 963
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 968
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 990
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 992
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1100
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1147
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1151
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1153
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1155
int Cyc_Absyn_fntype_att(void*);
# 1161
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 74
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
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
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 72 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args);
# 77
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 80
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 146
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr,struct _dyneither_ptr);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr,struct _dyneither_ptr);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr,struct _dyneither_ptr);
# 40
int Cyc_Tcutil_is_void_type(void*);
int Cyc_Tcutil_is_char_type(void*);
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
int Cyc_Tcutil_is_integral_type(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
int Cyc_Tcutil_is_signed_type(void*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 53
int Cyc_Tcutil_is_dyneither_ptr(void*t);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
int Cyc_Tcutil_is_bound_one(void*b);
# 58
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 61
int Cyc_Tcutil_is_bits_only_type(void*t);
# 63
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 68
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 70
void*Cyc_Tcutil_pointer_region(void*t);
# 73
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 76
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 79
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 81
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 83
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 85
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 89
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 92
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
# 99
void*Cyc_Tcutil_copy_type(void*t);
# 102
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 105
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 109
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 122
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 124
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 126
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple14{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 130
struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 132
extern int Cyc_Tcutil_warn_alias_coerce;
# 135
extern int Cyc_Tcutil_warn_region_coerce;
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 148
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 153
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 158
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_boolko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ptrbko;
# 167
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 172
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 177
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 181
struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 186
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 188
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 192
void Cyc_Tcutil_explain_failure();
# 194
int Cyc_Tcutil_unify(void*,void*);
# 196
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 199
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 201
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 207
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 210
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 214
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 216
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 220
struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple16{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*,struct Cyc_Absyn_Tvar*);
# 226
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 253 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 255
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 263
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 266
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 273
void Cyc_Tcutil_check_nonzero_bound(unsigned int,void*);
# 275
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,void*,int do_warn);
# 277
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 279
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 287
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 292
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 295
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 300
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 305
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple17{int f1;void*f2;};
# 309
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 312
void*Cyc_Tcutil_normalize_effect(void*e);
# 315
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 317
int Cyc_Tcutil_new_tvar_id();
# 319
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 322
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 324
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 327
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 330
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 333
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 337
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 341
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 344
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 348
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 351
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 355
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 358
int Cyc_Tcutil_zeroable_type(void*t);
# 362
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 365
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 367
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**te);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 40 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 110
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 42 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_tmp1=_tmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 48
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 54
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_tmp5=_tmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 60
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_tmp7=_tmp6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 67
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_tmp9=_tmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 74
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 81
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_tmpD=_tmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 88
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_tmpF=_tmpE;void*_tmp11;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp11=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {
# 91
void*_tmp10=_tmp11;switch(*((int*)_tmp10)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 98
 return 0;}_LL0:;}
# 101
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Tcutil_compress(t);void*_tmp13=_tmp12;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
 return 1;}else{goto _LL5;}case 2U: _LL3: _LL4:
 return 1;default: goto _LL5;}else{_LL5: _LL6:
 return 0;}_LL0:;}
# 108
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t) || Cyc_Tcutil_is_any_float_type(t);}
# 111
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t) || Cyc_Tcutil_is_any_float_type(t);}
# 114
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp14=Cyc_Tcutil_compress(t);void*_tmp15=_tmp14;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 121
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 129
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp16=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp17=_tmp16;void*_tmp19;switch(*((int*)_tmp17)){case 0U: _LL1: _tmp19=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17)->f1;_LL2: {
# 132
void*_tmp18=_tmp19;switch(*((int*)_tmp18)){case 1U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 goto _LLF;case 4U: _LLE: _LLF:
 return 1;default: _LL10: _LL11:
 return 0;}_LL7:;}case 1U: _LL3: _LL4:
# 139
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);default: _LL5: _LL6:
 return 0;}_LL0:;}
# 145
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1A=Cyc_Tcutil_compress(t);void*_tmp1B=_tmp1A;void*_tmp1C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B)->tag == 3U){_LL1: _tmp1C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B)->f1).ptr_atts).zero_term;_LL2:
# 148
 return Cyc_Tcutil_force_type2bool(0,_tmp1C);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 154
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp1D=Cyc_Tcutil_compress(t);void*_tmp1E=_tmp1D;void*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->tag == 3U){_LL1: _tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->f1).ptr_atts).nullable;_LL2:
# 157
 return Cyc_Tcutil_force_type2bool(0,_tmp1F);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 163
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp20=Cyc_Tcutil_compress(t);void*_tmp21=_tmp20;void*_tmp22;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->tag == 3U){_LL1: _tmp22=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->f1).ptr_atts).bounds;_LL2:
# 166
 return Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp22);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 173
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp23=Cyc_Tcutil_compress(t);void*_tmp24=_tmp23;void*_tmp26;void*_tmp25;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->tag == 3U){_LL1: _tmp26=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).elt_type;_tmp25=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).bounds;_LL2:
# 176
 if(Cyc_Tcutil_unify(_tmp25,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=_tmp26;
return 1;}else{
# 180
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 187
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp27=Cyc_Tcutil_compress(t);void*_tmp28=_tmp27;void*_tmp2A;void*_tmp29;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28)->tag == 3U){_LL1: _tmp2A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28)->f1).elt_type;_tmp29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28)->f1).ptr_atts).zero_term;_LL2:
# 190
*elt_type_dest=_tmp2A;
return Cyc_Absyn_type2bool(0,_tmp29);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 199
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 201
void*_tmp2B=Cyc_Tcutil_compress(t);void*_tmp2C=_tmp2B;void*_tmp35;struct Cyc_Absyn_Tqual _tmp34;struct Cyc_Absyn_Exp*_tmp33;void*_tmp32;void*_tmp31;void*_tmp30;void*_tmp2F;switch(*((int*)_tmp2C)){case 3U: _LL1: _tmp31=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C)->f1).elt_type;_tmp30=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C)->f1).ptr_atts).bounds;_tmp2F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C)->f1).ptr_atts).zero_term;_LL2:
# 203
 if(Cyc_Absyn_type2bool(0,_tmp2F)){
*ptr_type=t;
*elt_type=_tmp31;
{void*_tmp2D=Cyc_Tcutil_compress(_tmp30);void*_tmp2E=_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->f1)->tag == 14U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 210
return 1;}else{
return 0;}case 4U: _LL3: _tmp35=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).elt_type;_tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).tq;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).num_elts;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).zero_term;_LL4:
# 213
 if(Cyc_Absyn_type2bool(0,_tmp32)){
*elt_type=_tmp35;
*is_dyneither=0;
({void*_tmpA13=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmpA13;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 226
int Cyc_Tcutil_is_tagged_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_tagged_pointer_type_elt(t,& ignore);}
# 232
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp36=({void*_tmpA14=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpA14,b);});
if(_tmp36 == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp37=_tmp36;
struct _tuple13 _tmp38=Cyc_Evexp_eval_const_uint_exp(_tmp37);struct _tuple13 _tmp39=_tmp38;unsigned int _tmp3B;int _tmp3A;_LL1: _tmp3B=_tmp39.f1;_tmp3A=_tmp39.f2;_LL2:;
return _tmp3A  && _tmp3B == (unsigned int)1;};}
# 241
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3C=Cyc_Tcutil_compress(t);void*_tmp3D=_tmp3C;struct Cyc_List_List*_tmp49;struct Cyc_List_List*_tmp48;void*_tmp47;void*_tmp46;void*_tmp45;struct Cyc_List_List*_tmp44;switch(*((int*)_tmp3D)){case 0U: _LL1: _tmp45=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f1;_tmp44=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f2;_LL2: {
# 244
void*_tmp3E=_tmp45;struct Cyc_Absyn_Aggrdecl*_tmp43;switch(*((int*)_tmp3E)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return 1;case 15U: _LL12: _LL13:
 goto _LL15;case 16U: _LL14: _LL15:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3E)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
 return 0;}else{_LL18: _tmp43=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3E)->f1).KnownAggr).val;_LL19:
# 252
 if(_tmp43->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp3F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3F;_push_region(rgn);
{struct Cyc_List_List*_tmp40=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp43->tvs,_tmp44);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,_tmp40,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp41=0;_npop_handler(0U);return _tmp41;}}}{
int _tmp42=1;_npop_handler(0U);return _tmp42;};}
# 259
;_pop_region(rgn);};};}default: _LL1A: _LL1B:
# 263
 return 0;}_LLB:;}case 4U: _LL3: _tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).elt_type;_tmp46=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).zero_term;_LL4:
# 268
 return !Cyc_Absyn_type2bool(0,_tmp46) && Cyc_Tcutil_is_bits_only_type(_tmp47);case 6U: _LL5: _tmp48=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3D)->f1;_LL6:
# 270
 for(0;_tmp48 != 0;_tmp48=_tmp48->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)_tmp48->hd)).f2))return 0;}
return 1;case 7U: _LL7: _tmp49=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D)->f2;_LL8:
# 274
 for(0;_tmp49 != 0;_tmp49=_tmp49->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)_tmp49->hd)->type))return 0;}
return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 283
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
{void*_tmp4A=Cyc_Tcutil_compress(t);void*_tmp4B=_tmp4A;struct Cyc_List_List*_tmp4F;void*_tmp4E;switch(*((int*)_tmp4B)){case 3U: _LL1: _tmp4E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_noreturn_fn_type(_tmp4E);case 5U: _LL3: _tmp4F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4B)->f1).attributes;_LL4:
# 287
 for(0;_tmp4F != 0;_tmp4F=_tmp4F->tl){
void*_tmp4C=(void*)_tmp4F->hd;void*_tmp4D=_tmp4C;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp4D)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 292
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 295
return 0;}char Cyc_Tcutil_Unify[6U]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 299
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 301
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 305
int Cyc_Tcutil_warn_region_coerce=0;
# 308
static void*Cyc_Tcutil_t1_failure=0;
static int Cyc_Tcutil_tq1_const=0;
static void*Cyc_Tcutil_t2_failure=0;
static int Cyc_Tcutil_tq2_const=0;
# 313
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 317
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 322
if(({struct _dyneither_ptr _tmpA15=({const char*_tmp50="(qualifiers don't match)";_tag_dyneither(_tmp50,sizeof(char),25U);});Cyc_strcmp(_tmpA15,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp53=({struct Cyc_String_pa_PrintArg_struct _tmp97F;_tmp97F.tag=0U,_tmp97F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp97F;});void*_tmp51[1U];_tmp51[0]=& _tmp53;({struct Cyc___cycFILE*_tmpA17=Cyc_stderr;struct _dyneither_ptr _tmpA16=({const char*_tmp52="  %s\n";_tag_dyneither(_tmp52,sizeof(char),6U);});Cyc_fprintf(_tmpA17,_tmpA16,_tag_dyneither(_tmp51,sizeof(void*),1U));});});
return;}
# 327
if(({struct _dyneither_ptr _tmpA18=({const char*_tmp54="(function effects do not match)";_tag_dyneither(_tmp54,sizeof(char),32U);});Cyc_strcmp(_tmpA18,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp55=Cyc_Absynpp_tc_params_r;
_tmp55.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp55);}{
# 332
void*_tmp56=Cyc_Tcutil_t1_failure;
void*_tmp57=Cyc_Tcutil_t2_failure;
struct _dyneither_ptr s1=(unsigned int)_tmp56?Cyc_Absynpp_typ2string(_tmp56):({const char*_tmp72="<?>";_tag_dyneither(_tmp72,sizeof(char),4U);});
struct _dyneither_ptr s2=(unsigned int)_tmp57?Cyc_Absynpp_typ2string(_tmp57):({const char*_tmp71="<?>";_tag_dyneither(_tmp71,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmp980;_tmp980.tag=0U,_tmp980.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp980;});void*_tmp58[1U];_tmp58[0]=& _tmp5A;({struct Cyc___cycFILE*_tmpA1A=Cyc_stderr;struct _dyneither_ptr _tmpA19=({const char*_tmp59="  %s";_tag_dyneither(_tmp59,sizeof(char),5U);});Cyc_fprintf(_tmpA1A,_tmpA19,_tag_dyneither(_tmp58,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmp5B=0U;({struct Cyc___cycFILE*_tmpA1C=Cyc_stderr;struct _dyneither_ptr _tmpA1B=({const char*_tmp5C="\n\t";_tag_dyneither(_tmp5C,sizeof(char),3U);});Cyc_fprintf(_tmpA1C,_tmpA1B,_tag_dyneither(_tmp5B,sizeof(void*),0U));});});
pos=8;}else{
# 343
({void*_tmp5D=0U;({struct Cyc___cycFILE*_tmpA1E=Cyc_stderr;struct _dyneither_ptr _tmpA1D=({const char*_tmp5E=" ";_tag_dyneither(_tmp5E,sizeof(char),2U);});Cyc_fprintf(_tmpA1E,_tmpA1D,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});
++ pos;}
# 346
({void*_tmp5F=0U;({struct Cyc___cycFILE*_tmpA20=Cyc_stderr;struct _dyneither_ptr _tmpA1F=({const char*_tmp60="and ";_tag_dyneither(_tmp60,sizeof(char),5U);});Cyc_fprintf(_tmpA20,_tmpA1F,_tag_dyneither(_tmp5F,sizeof(void*),0U));});});
pos +=4;
if((unsigned int)pos + _get_dyneither_size(s2,sizeof(char))>= (unsigned int)80){
({void*_tmp61=0U;({struct Cyc___cycFILE*_tmpA22=Cyc_stderr;struct _dyneither_ptr _tmpA21=({const char*_tmp62="\n\t";_tag_dyneither(_tmp62,sizeof(char),3U);});Cyc_fprintf(_tmpA22,_tmpA21,_tag_dyneither(_tmp61,sizeof(void*),0U));});});
pos=8;}
# 352
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp981;_tmp981.tag=0U,_tmp981.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp981;});void*_tmp63[1U];_tmp63[0]=& _tmp65;({struct Cyc___cycFILE*_tmpA24=Cyc_stderr;struct _dyneither_ptr _tmpA23=({const char*_tmp64="%s ";_tag_dyneither(_tmp64,sizeof(char),4U);});Cyc_fprintf(_tmpA24,_tmpA23,_tag_dyneither(_tmp63,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ (unsigned int)1;
if(pos + 17 >= 80){
({void*_tmp66=0U;({struct Cyc___cycFILE*_tmpA26=Cyc_stderr;struct _dyneither_ptr _tmpA25=({const char*_tmp67="\n\t";_tag_dyneither(_tmp67,sizeof(char),3U);});Cyc_fprintf(_tmpA26,_tmpA25,_tag_dyneither(_tmp66,sizeof(void*),0U));});});
pos=8;}
# 358
({void*_tmp68=0U;({struct Cyc___cycFILE*_tmpA28=Cyc_stderr;struct _dyneither_ptr _tmpA27=({const char*_tmp69="are not compatible. ";_tag_dyneither(_tmp69,sizeof(char),21U);});Cyc_fprintf(_tmpA28,_tmpA27,_tag_dyneither(_tmp68,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpA29=(char*)Cyc_Tcutil_failure_reason.curr;_tmpA29 != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({unsigned long _tmpA2A=(unsigned long)pos;_tmpA2A + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= (unsigned long)80)
({void*_tmp6A=0U;({struct Cyc___cycFILE*_tmpA2C=Cyc_stderr;struct _dyneither_ptr _tmpA2B=({const char*_tmp6B="\n\t";_tag_dyneither(_tmp6B,sizeof(char),3U);});Cyc_fprintf(_tmpA2C,_tmpA2B,_tag_dyneither(_tmp6A,sizeof(void*),0U));});});
# 364
({struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp982;_tmp982.tag=0U,_tmp982.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp982;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6E;({struct Cyc___cycFILE*_tmpA2E=Cyc_stderr;struct _dyneither_ptr _tmpA2D=({const char*_tmp6D="%s";_tag_dyneither(_tmp6D,sizeof(char),3U);});Cyc_fprintf(_tmpA2E,_tmpA2D,_tag_dyneither(_tmp6C,sizeof(void*),1U));});});}
# 366
({void*_tmp6F=0U;({struct Cyc___cycFILE*_tmpA30=Cyc_stderr;struct _dyneither_ptr _tmpA2F=({const char*_tmp70="\n";_tag_dyneither(_tmp70,sizeof(char),2U);});Cyc_fprintf(_tmpA30,_tmpA2F,_tag_dyneither(_tmp6F,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 370
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 372
Cyc_Warn_verr(loc,fmt,ap);}
# 374
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 377
Cyc_Warn_vimpos(fmt,ap);}
# 379
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 381
Cyc_Warn_vwarn(sg,fmt,ap);}
# 384
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp73=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp74=_tmp73;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 394
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 399
void*Cyc_Tcutil_compress(void*t){
void*_tmp75=t;void*_tmp7E;struct Cyc_Absyn_Exp*_tmp7D;struct Cyc_Absyn_Exp*_tmp7C;void**_tmp7B;void**_tmp7A;switch(*((int*)_tmp75)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp7A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp75)->f2;_LL8: {
# 410
void*ta=(void*)_check_null(*_tmp7A);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp7A=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75)->f4 == 0){_LL3: _LL4:
# 402
 return t;}else{_LL5: _tmp7B=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75)->f4;_LL6: {
# 404
void*ta=(void*)_check_null(*_tmp7B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp7B=t2;
return t2;}}case 9U: _LL9: _tmp7C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp75)->f1;_LLA:
# 416
 Cyc_Evexp_eval_const_uint_exp(_tmp7C);{
void*_tmp76=_tmp7C->r;void*_tmp77=_tmp76;void*_tmp78;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp77)->tag == 39U){_LL12: _tmp78=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp77)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp78);}else{_LL14: _LL15:
 return t;}_LL11:;};case 11U: _LLB: _tmp7D=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp75)->f1;_LLC: {
# 422
void*_tmp79=_tmp7D->topt;
if(_tmp79 != 0)return _tmp79;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f2 != 0){_LLD: _tmp7E=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75)->f2;_LLE:
# 426
 return Cyc_Tcutil_compress(_tmp7E);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 435
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 439
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp7F=Cyc_Absyn_compress_kb(kb);void*_tmp80=_tmp7F;struct Cyc_Absyn_Kind*_tmp83;switch(*((int*)_tmp80)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=1U,_tmp81->f1=0;_tmp81;});case 2U: _LL3: _tmp83=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp80)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=2U,_tmp82->f1=0,_tmp82->f2=_tmp83;_tmp82;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 446
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 448
return({struct Cyc_Absyn_Tvar*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->name=tv->name,_tmp84->identity=- 1,({void*_tmpA31=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp84->kind=_tmpA31;});_tmp84;});}
# 450
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 452
struct _tuple10*_tmp85=arg;struct _dyneither_ptr*_tmp89;struct Cyc_Absyn_Tqual _tmp88;void*_tmp87;_LL1: _tmp89=_tmp85->f1;_tmp88=_tmp85->f2;_tmp87=_tmp85->f3;_LL2:;
return({struct _tuple10*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->f1=_tmp89,_tmp86->f2=_tmp88,({void*_tmpA32=Cyc_Tcutil_copy_type(_tmp87);_tmp86->f3=_tmpA32;});_tmp86;});}
# 455
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp8A=arg;struct Cyc_Absyn_Tqual _tmp8D;void*_tmp8C;_LL1: _tmp8D=_tmp8A->f1;_tmp8C=_tmp8A->f2;_LL2:;
return({struct _tuple12*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->f1=_tmp8D,({void*_tmpA33=Cyc_Tcutil_copy_type(_tmp8C);_tmp8B->f2=_tmpA33;});_tmp8B;});}
# 459
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 461
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->name=f->name,_tmp8E->tq=f->tq,({void*_tmpA35=Cyc_Tcutil_copy_type(f->type);_tmp8E->type=_tmpA35;}),_tmp8E->width=f->width,_tmp8E->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmpA34=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp8E->requires_clause=_tmpA34;});_tmp8E;});}
# 465
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp8F=x;void*_tmp92;void*_tmp91;_LL1: _tmp92=_tmp8F->f1;_tmp91=_tmp8F->f2;_LL2:;
return({struct _tuple0*_tmp90=_cycalloc(sizeof(*_tmp90));({void*_tmpA37=Cyc_Tcutil_copy_type(_tmp92);_tmp90->f1=_tmpA37;}),({void*_tmpA36=Cyc_Tcutil_copy_type(_tmp91);_tmp90->f2=_tmpA36;});_tmp90;});}
# 469
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->name=f->name,_tmp93->tag=f->tag,_tmp93->loc=f->loc;_tmp93;});}
# 472
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 476
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp94=Cyc_Tcutil_compress(t);void*_tmp95=_tmp94;struct Cyc_Absyn_Datatypedecl*_tmpD9;struct Cyc_Absyn_Enumdecl*_tmpD8;struct Cyc_Absyn_Aggrdecl*_tmpD7;struct _tuple2*_tmpD6;struct Cyc_List_List*_tmpD5;struct Cyc_Absyn_Typedefdecl*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;enum Cyc_Absyn_AggrKind _tmpD1;struct Cyc_List_List*_tmpD0;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpCE;void*_tmpCD;struct Cyc_Absyn_Tqual _tmpCC;void*_tmpCB;struct Cyc_List_List*_tmpCA;int _tmpC9;struct Cyc_Absyn_VarargInfo*_tmpC8;struct Cyc_List_List*_tmpC7;struct Cyc_List_List*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_List_List*_tmpC4;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_List_List*_tmpC2;void*_tmpC1;struct Cyc_Absyn_Tqual _tmpC0;struct Cyc_Absyn_Exp*_tmpBF;void*_tmpBE;unsigned int _tmpBD;void*_tmpBC;struct Cyc_Absyn_Tqual _tmpBB;void*_tmpBA;void*_tmpB9;void*_tmpB8;void*_tmpB7;struct Cyc_Absyn_PtrLoc*_tmpB6;struct Cyc_Absyn_Tvar*_tmpB5;void*_tmpB4;struct Cyc_List_List*_tmpB3;void*_tmpB2;switch(*((int*)_tmp95)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95)->f2 == 0){_LL1: _tmpB2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95)->f1;_LL2:
 return t;}else{_LL3: _tmpB4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95)->f1;_tmpB3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp95)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=0U,_tmp96->f1=_tmpB4,({struct Cyc_List_List*_tmpA38=Cyc_Tcutil_copy_types(_tmpB3);_tmp96->f2=_tmpA38;});_tmp96;});}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmpB5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp95)->f1;_LL8:
 return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(_tmpB5));case 3U: _LL9: _tmpBC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95)->f1).elt_type;_tmpBB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95)->f1).elt_tq;_tmpBA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95)->f1).ptr_atts).rgn;_tmpB9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95)->f1).ptr_atts).nullable;_tmpB8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95)->f1).ptr_atts).bounds;_tmpB7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95)->f1).ptr_atts).zero_term;_tmpB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95)->f1).ptr_atts).ptrloc;_LLA: {
# 483
void*_tmp97=Cyc_Tcutil_copy_type(_tmpBC);
void*_tmp98=Cyc_Tcutil_copy_type(_tmpBA);
void*_tmp99=Cyc_Tcutil_copy_type(_tmpB9);
struct Cyc_Absyn_Tqual _tmp9A=_tmpBB;
# 488
void*_tmp9B=Cyc_Tcutil_copy_type(_tmpB8);
void*_tmp9C=Cyc_Tcutil_copy_type(_tmpB7);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=3U,(_tmp9D->f1).elt_type=_tmp97,(_tmp9D->f1).elt_tq=_tmp9A,((_tmp9D->f1).ptr_atts).rgn=_tmp98,((_tmp9D->f1).ptr_atts).nullable=_tmp99,((_tmp9D->f1).ptr_atts).bounds=_tmp9B,((_tmp9D->f1).ptr_atts).zero_term=_tmp9C,((_tmp9D->f1).ptr_atts).ptrloc=_tmpB6;_tmp9D;});}case 4U: _LLB: _tmpC1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95)->f1).elt_type;_tmpC0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95)->f1).tq;_tmpBF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95)->f1).num_elts;_tmpBE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95)->f1).zero_term;_tmpBD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95)->f1).zt_loc;_LLC: {
# 492
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpBF);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=4U,({void*_tmpA3A=Cyc_Tcutil_copy_type(_tmpC1);(_tmp9E->f1).elt_type=_tmpA3A;}),(_tmp9E->f1).tq=_tmpC0,(_tmp9E->f1).num_elts=eopt2,({
void*_tmpA39=Cyc_Tcutil_copy_type(_tmpBE);(_tmp9E->f1).zero_term=_tmpA39;}),(_tmp9E->f1).zt_loc=_tmpBD;_tmp9E;});}case 5U: _LLD: _tmpCE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).tvars;_tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).effect;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).ret_tqual;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).ret_type;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).args;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).c_varargs;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).cyc_varargs;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).rgn_po;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).attributes;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).requires_clause;_tmpC4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).requires_relns;_tmpC3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).ensures_clause;_tmpC2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1).ensures_relns;_LLE: {
# 496
struct Cyc_List_List*_tmp9F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpCE);
void*effopt2=_tmpCD == 0?0: Cyc_Tcutil_copy_type(_tmpCD);
void*_tmpA0=Cyc_Tcutil_copy_type(_tmpCB);
struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpCA);
int _tmpA2=_tmpC9;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpC8 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpC8;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->name=cv->name,_tmpA3->tq=cv->tq,({void*_tmpA3B=Cyc_Tcutil_copy_type(cv->type);_tmpA3->type=_tmpA3B;}),_tmpA3->inject=cv->inject;_tmpA3;});}{
# 507
struct Cyc_List_List*_tmpA4=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpC7);
struct Cyc_List_List*_tmpA5=_tmpC6;
struct Cyc_Absyn_Exp*_tmpA6=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC5);
struct Cyc_List_List*_tmpA7=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC4);
struct Cyc_Absyn_Exp*_tmpA8=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC3);
struct Cyc_List_List*_tmpA9=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=5U,(_tmpAA->f1).tvars=_tmp9F,(_tmpAA->f1).effect=effopt2,(_tmpAA->f1).ret_tqual=_tmpCC,(_tmpAA->f1).ret_type=_tmpA0,(_tmpAA->f1).args=_tmpA1,(_tmpAA->f1).c_varargs=_tmpA2,(_tmpAA->f1).cyc_varargs=cyc_varargs2,(_tmpAA->f1).rgn_po=_tmpA4,(_tmpAA->f1).attributes=_tmpA5,(_tmpAA->f1).requires_clause=_tmpA6,(_tmpAA->f1).requires_relns=_tmpA7,(_tmpAA->f1).ensures_clause=_tmpA8,(_tmpAA->f1).ensures_relns=_tmpA9;_tmpAA;});};}case 6U: _LLF: _tmpCF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp95)->f1;_LL10:
# 516
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=6U,({struct Cyc_List_List*_tmpA3C=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpCF);_tmpAB->f1=_tmpA3C;});_tmpAB;});case 7U: _LL11: _tmpD1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp95)->f1;_tmpD0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp95)->f2;_LL12:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=7U,_tmpAC->f1=_tmpD1,({struct Cyc_List_List*_tmpA3D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpD0);_tmpAC->f2=_tmpA3D;});_tmpAC;});case 9U: _LL13: _tmpD2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp95)->f1;_LL14:
# 520
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=9U,_tmpAD->f1=_tmpD2;_tmpAD;});case 11U: _LL15: _tmpD3=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp95)->f1;_LL16:
# 523
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=11U,_tmpAE->f1=_tmpD3;_tmpAE;});case 8U: _LL17: _tmpD6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp95)->f1;_tmpD5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp95)->f2;_tmpD4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp95)->f3;_LL18:
# 525
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=8U,_tmpAF->f1=_tmpD6,({struct Cyc_List_List*_tmpA3E=Cyc_Tcutil_copy_types(_tmpD5);_tmpAF->f2=_tmpA3E;}),_tmpAF->f3=_tmpD4,_tmpAF->f4=0;_tmpAF;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp95)->f1)->r)){case 0U: _LL19: _tmpD7=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp95)->f1)->r)->f1;_LL1A: {
# 528
struct Cyc_List_List*_tmpB0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD7->tvs);
return({union Cyc_Absyn_AggrInfo _tmpA3F=Cyc_Absyn_UnknownAggr(_tmpD7->kind,_tmpD7->name,0);Cyc_Absyn_aggr_type(_tmpA3F,_tmpB0);});}case 1U: _LL1B: _tmpD8=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp95)->f1)->r)->f1;_LL1C:
# 531
 return Cyc_Absyn_enum_type(_tmpD8->name,0);default: _LL1D: _tmpD9=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp95)->f1)->r)->f1;_LL1E: {
# 533
struct Cyc_List_List*_tmpB1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD9->tvs);
return({union Cyc_Absyn_DatatypeInfo _tmpA40=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp983;_tmp983.name=_tmpD9->name,_tmp983.is_extensible=0;_tmp983;}));Cyc_Absyn_datatype_type(_tmpA40,_tmpB1);});}}}_LL0:;}
# 548 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpDA=d;struct _dyneither_ptr*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDA)->tag == 0U){_LL1: _tmpDC=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDA)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->tag=0U,({struct Cyc_Absyn_Exp*_tmpA41=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpDC);_tmpDB->f1=_tmpA41;});_tmpDB;});}else{_LL3: _tmpDD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpDA)->f1;_LL4:
 return d;}_LL0:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 554
static struct _tuple18*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple18*e){
# 556
return({struct _tuple18*_tmpDE=_cycalloc(sizeof(*_tmpDE));({struct Cyc_List_List*_tmpA43=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpDE->f1=_tmpA43;}),({struct Cyc_Absyn_Exp*_tmpA42=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpDE->f2=_tmpA42;});_tmpDE;});}
# 559
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpDF=preserve_types;
{void*_tmpE0=e->r;void*_tmpE1=_tmpE0;int _tmp15E;struct _dyneither_ptr _tmp15D;void*_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;struct _dyneither_ptr*_tmp15A;struct Cyc_Core_Opt*_tmp159;struct Cyc_List_List*_tmp158;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp156;int _tmp155;struct Cyc_Absyn_Exp*_tmp154;void**_tmp153;struct Cyc_Absyn_Exp*_tmp152;int _tmp151;int _tmp150;void*_tmp14F;struct Cyc_Absyn_Enumfield*_tmp14E;struct Cyc_Absyn_Enumdecl*_tmp14D;struct Cyc_Absyn_Enumfield*_tmp14C;struct Cyc_List_List*_tmp14B;struct Cyc_Absyn_Datatypedecl*_tmp14A;struct Cyc_Absyn_Datatypefield*_tmp149;void*_tmp148;struct Cyc_List_List*_tmp147;struct _tuple2*_tmp146;struct Cyc_List_List*_tmp145;struct Cyc_List_List*_tmp144;struct Cyc_Absyn_Aggrdecl*_tmp143;struct Cyc_Absyn_Exp*_tmp142;void*_tmp141;int _tmp140;struct Cyc_Absyn_Vardecl*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;int _tmp13C;struct Cyc_List_List*_tmp13B;struct _dyneither_ptr*_tmp13A;struct Cyc_Absyn_Tqual _tmp139;void*_tmp138;struct Cyc_List_List*_tmp137;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct _dyneither_ptr*_tmp132;int _tmp131;int _tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct _dyneither_ptr*_tmp12E;int _tmp12D;int _tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;void*_tmp129;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_Exp*_tmp127;void*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;void*_tmp122;struct Cyc_Absyn_Exp*_tmp121;int _tmp120;enum Cyc_Absyn_Coercion _tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;int _tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_List_List*_tmp118;struct Cyc_Absyn_VarargCallInfo*_tmp117;int _tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;enum Cyc_Absyn_Incrementor _tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Core_Opt*_tmp109;struct Cyc_Absyn_Exp*_tmp108;enum Cyc_Absyn_Primop _tmp107;struct Cyc_List_List*_tmp106;struct _dyneither_ptr _tmp105;void*_tmp104;union Cyc_Absyn_Cnst _tmp103;switch(*((int*)_tmpE1)){case 0U: _LL1: _tmp103=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL2:
 new_e=Cyc_Absyn_const_exp(_tmp103,e->loc);goto _LL0;case 1U: _LL3: _tmp104=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL4:
 new_e=Cyc_Absyn_varb_exp(_tmp104,e->loc);goto _LL0;case 2U: _LL5: _tmp105=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL6:
 new_e=Cyc_Absyn_pragma_exp(_tmp105,e->loc);goto _LL0;case 3U: _LL7: _tmp107=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp106=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL8:
 new_e=({enum Cyc_Absyn_Primop _tmpA45=_tmp107;struct Cyc_List_List*_tmpA44=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp106);Cyc_Absyn_primop_exp(_tmpA45,_tmpA44,e->loc);});goto _LL0;case 4U: _LL9: _tmp10A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp109=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp108=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_LLA:
# 568
 new_e=({struct Cyc_Absyn_Exp*_tmpA48=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp10A);struct Cyc_Core_Opt*_tmpA47=(unsigned int)_tmp109?({struct Cyc_Core_Opt*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->v=(void*)_tmp109->v;_tmpE2;}): 0;struct Cyc_Absyn_Exp*_tmpA46=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp108);Cyc_Absyn_assignop_exp(_tmpA48,_tmpA47,_tmpA46,e->loc);});
goto _LL0;case 5U: _LLB: _tmp10C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp10B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLC:
 new_e=({struct Cyc_Absyn_Exp*_tmpA4A=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp10C);enum Cyc_Absyn_Incrementor _tmpA49=_tmp10B;Cyc_Absyn_increment_exp(_tmpA4A,_tmpA49,e->loc);});goto _LL0;case 6U: _LLD: _tmp10F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp10E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp10D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_LLE:
# 572
 new_e=({struct Cyc_Absyn_Exp*_tmpA4D=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp10F);struct Cyc_Absyn_Exp*_tmpA4C=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp10E);struct Cyc_Absyn_Exp*_tmpA4B=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp10D);Cyc_Absyn_conditional_exp(_tmpA4D,_tmpA4C,_tmpA4B,e->loc);});goto _LL0;case 7U: _LLF: _tmp111=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp110=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL10:
 new_e=({struct Cyc_Absyn_Exp*_tmpA4F=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp111);struct Cyc_Absyn_Exp*_tmpA4E=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp110);Cyc_Absyn_and_exp(_tmpA4F,_tmpA4E,e->loc);});goto _LL0;case 8U: _LL11: _tmp113=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp112=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL12:
 new_e=({struct Cyc_Absyn_Exp*_tmpA51=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp113);struct Cyc_Absyn_Exp*_tmpA50=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp112);Cyc_Absyn_or_exp(_tmpA51,_tmpA50,e->loc);});goto _LL0;case 9U: _LL13: _tmp115=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp114=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL14:
 new_e=({struct Cyc_Absyn_Exp*_tmpA53=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp115);struct Cyc_Absyn_Exp*_tmpA52=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp114);Cyc_Absyn_seq_exp(_tmpA53,_tmpA52,e->loc);});goto _LL0;case 10U: _LL15: _tmp119=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp118=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp117=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp116=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL16:
# 577
{struct Cyc_Absyn_VarargCallInfo*_tmpE3=_tmp117;int _tmpEF;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_VarargInfo*_tmpED;if(_tmpE3 != 0){_LL56: _tmpEF=_tmpE3->num_varargs;_tmpEE=_tmpE3->injectors;_tmpED=_tmpE3->vai;_LL57: {
# 579
struct Cyc_Absyn_VarargInfo*_tmpE4=_tmpED;struct _dyneither_ptr*_tmpEB;struct Cyc_Absyn_Tqual _tmpEA;void*_tmpE9;int _tmpE8;_LL5B: _tmpEB=_tmpE4->name;_tmpEA=_tmpE4->tq;_tmpE9=_tmpE4->type;_tmpE8=_tmpE4->inject;_LL5C:;
new_e=({void*_tmpA59=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->tag=10U,({
struct Cyc_Absyn_Exp*_tmpA58=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp119);_tmpE7->f1=_tmpA58;}),({struct Cyc_List_List*_tmpA57=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp118);_tmpE7->f2=_tmpA57;}),({
struct Cyc_Absyn_VarargCallInfo*_tmpA56=({struct Cyc_Absyn_VarargCallInfo*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->num_varargs=_tmpEF,_tmpE6->injectors=_tmpEE,({
struct Cyc_Absyn_VarargInfo*_tmpA55=({struct Cyc_Absyn_VarargInfo*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->name=_tmpEB,_tmpE5->tq=_tmpEA,({void*_tmpA54=Cyc_Tcutil_copy_type(_tmpE9);_tmpE5->type=_tmpA54;}),_tmpE5->inject=_tmpE8;_tmpE5;});_tmpE6->vai=_tmpA55;});_tmpE6;});
# 582
_tmpE7->f3=_tmpA56;}),_tmpE7->f4=_tmp116;_tmpE7;});
# 580
Cyc_Absyn_new_exp(_tmpA59,e->loc);});
# 585
goto _LL55;}}else{_LL58: _LL59:
# 587
 new_e=({void*_tmpA5C=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->tag=10U,({struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp119);_tmpEC->f1=_tmpA5B;}),({struct Cyc_List_List*_tmpA5A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp118);_tmpEC->f2=_tmpA5A;}),_tmpEC->f3=_tmp117,_tmpEC->f4=_tmp116;_tmpEC;});Cyc_Absyn_new_exp(_tmpA5C,e->loc);});
goto _LL55;}_LL55:;}
# 590
goto _LL0;case 11U: _LL17: _tmp11B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp11A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL18:
# 592
 new_e=_tmp11A?({struct Cyc_Absyn_Exp*_tmpA5E=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp11B);Cyc_Absyn_rethrow_exp(_tmpA5E,e->loc);}):({struct Cyc_Absyn_Exp*_tmpA5D=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp11B);Cyc_Absyn_throw_exp(_tmpA5D,e->loc);});
goto _LL0;case 12U: _LL19: _tmp11C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL1A:
 new_e=({struct Cyc_Absyn_Exp*_tmpA5F=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp11C);Cyc_Absyn_noinstantiate_exp(_tmpA5F,e->loc);});
goto _LL0;case 13U: _LL1B: _tmp11E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp11D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL1C:
# 597
 new_e=({struct Cyc_Absyn_Exp*_tmpA61=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp11E);struct Cyc_List_List*_tmpA60=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp11D);Cyc_Absyn_instantiate_exp(_tmpA61,_tmpA60,e->loc);});
goto _LL0;case 14U: _LL1D: _tmp122=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp121=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp120=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp11F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL1E:
# 600
 new_e=({void*_tmpA65=Cyc_Tcutil_copy_type(_tmp122);struct Cyc_Absyn_Exp*_tmpA64=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp121);int _tmpA63=_tmp120;enum Cyc_Absyn_Coercion _tmpA62=_tmp11F;Cyc_Absyn_cast_exp(_tmpA65,_tmpA64,_tmpA63,_tmpA62,e->loc);});goto _LL0;case 15U: _LL1F: _tmp123=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL20:
 new_e=({struct Cyc_Absyn_Exp*_tmpA66=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp123);Cyc_Absyn_address_exp(_tmpA66,e->loc);});goto _LL0;case 16U: _LL21: _tmp125=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp124=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL22: {
# 603
struct Cyc_Absyn_Exp*eo1=_tmp125;if(_tmp125 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp125);
new_e=({struct Cyc_Absyn_Exp*_tmpA68=eo1;struct Cyc_Absyn_Exp*_tmpA67=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp124);Cyc_Absyn_New_exp(_tmpA68,_tmpA67,e->loc);});
goto _LL0;}case 17U: _LL23: _tmp126=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL24:
 new_e=({void*_tmpA69=Cyc_Tcutil_copy_type(_tmp126);Cyc_Absyn_sizeoftype_exp(_tmpA69,e->loc);});
goto _LL0;case 18U: _LL25: _tmp127=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL26:
 new_e=({struct Cyc_Absyn_Exp*_tmpA6A=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp127);Cyc_Absyn_sizeofexp_exp(_tmpA6A,e->loc);});goto _LL0;case 19U: _LL27: _tmp129=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp128=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL28:
# 610
 new_e=({void*_tmpA6C=Cyc_Tcutil_copy_type(_tmp129);struct Cyc_List_List*_tmpA6B=_tmp128;Cyc_Absyn_offsetof_exp(_tmpA6C,_tmpA6B,e->loc);});goto _LL0;case 20U: _LL29: _tmp12A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL2A:
 new_e=({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp12A);Cyc_Absyn_deref_exp(_tmpA6D,e->loc);});goto _LL0;case 41U: _LL2B: _tmp12B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL2C:
 new_e=({struct Cyc_Absyn_Exp*_tmpA6E=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp12B);Cyc_Absyn_extension_exp(_tmpA6E,e->loc);});goto _LL0;case 21U: _LL2D: _tmp12F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp12E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp12D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp12C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL2E:
# 614
 new_e=({void*_tmpA70=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->tag=21U,({struct Cyc_Absyn_Exp*_tmpA6F=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp12F);_tmpF0->f1=_tmpA6F;}),_tmpF0->f2=_tmp12E,_tmpF0->f3=_tmp12D,_tmpF0->f4=_tmp12C;_tmpF0;});Cyc_Absyn_new_exp(_tmpA70,e->loc);});goto _LL0;case 22U: _LL2F: _tmp133=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp132=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp131=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp130=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL30:
# 616
 new_e=({void*_tmpA72=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->tag=22U,({struct Cyc_Absyn_Exp*_tmpA71=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp133);_tmpF1->f1=_tmpA71;}),_tmpF1->f2=_tmp132,_tmpF1->f3=_tmp131,_tmpF1->f4=_tmp130;_tmpF1;});Cyc_Absyn_new_exp(_tmpA72,e->loc);});goto _LL0;case 23U: _LL31: _tmp135=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp134=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL32:
 new_e=({struct Cyc_Absyn_Exp*_tmpA74=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp135);struct Cyc_Absyn_Exp*_tmpA73=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp134);Cyc_Absyn_subscript_exp(_tmpA74,_tmpA73,e->loc);});
goto _LL0;case 24U: _LL33: _tmp136=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL34:
 new_e=({struct Cyc_List_List*_tmpA75=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp136);Cyc_Absyn_tuple_exp(_tmpA75,e->loc);});goto _LL0;case 25U: _LL35: _tmp13A=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE1)->f1)->f1;_tmp139=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE1)->f1)->f2;_tmp138=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE1)->f1)->f3;_tmp137=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL36: {
# 621
struct _dyneither_ptr*vopt1=_tmp13A;
if(_tmp13A != 0)vopt1=_tmp13A;
new_e=({void*_tmpA79=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=25U,({struct _tuple10*_tmpA78=({struct _tuple10*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->f1=vopt1,_tmpF2->f2=_tmp139,({void*_tmpA77=Cyc_Tcutil_copy_type(_tmp138);_tmpF2->f3=_tmpA77;});_tmpF2;});_tmpF3->f1=_tmpA78;}),({
struct Cyc_List_List*_tmpA76=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp137);_tmpF3->f2=_tmpA76;});_tmpF3;});
# 623
Cyc_Absyn_new_exp(_tmpA79,e->loc);});
# 625
goto _LL0;}case 26U: _LL37: _tmp13B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL38:
 new_e=({void*_tmpA7B=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->tag=26U,({struct Cyc_List_List*_tmpA7A=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp13B);_tmpF4->f1=_tmpA7A;});_tmpF4;});Cyc_Absyn_new_exp(_tmpA7B,e->loc);});
goto _LL0;case 27U: _LL39: _tmp13F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp13E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp13D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp13C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL3A:
# 629
 new_e=({void*_tmpA7E=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=27U,_tmpF5->f1=_tmp13F,({struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp13E);_tmpF5->f2=_tmpA7D;}),({struct Cyc_Absyn_Exp*_tmpA7C=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp13D);_tmpF5->f3=_tmpA7C;}),_tmpF5->f4=_tmp13C;_tmpF5;});Cyc_Absyn_new_exp(_tmpA7E,e->loc);});
goto _LL0;case 28U: _LL3B: _tmp142=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp141=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp140=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_LL3C:
# 632
 new_e=({void*_tmpA81=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->tag=28U,({struct Cyc_Absyn_Exp*_tmpA80=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp142);_tmpF6->f1=_tmpA80;}),({void*_tmpA7F=Cyc_Tcutil_copy_type(_tmp141);_tmpF6->f2=_tmpA7F;}),_tmpF6->f3=_tmp140;_tmpF6;});Cyc_Absyn_new_exp(_tmpA81,_tmp142->loc);});
# 634
goto _LL0;case 29U: _LL3D: _tmp146=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp145=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp144=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp143=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL3E:
# 636
 new_e=({void*_tmpA84=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->tag=29U,_tmpF7->f1=_tmp146,({struct Cyc_List_List*_tmpA83=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp145);_tmpF7->f2=_tmpA83;}),({struct Cyc_List_List*_tmpA82=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp144);_tmpF7->f3=_tmpA82;}),_tmpF7->f4=_tmp143;_tmpF7;});Cyc_Absyn_new_exp(_tmpA84,e->loc);});
# 638
goto _LL0;case 30U: _LL3F: _tmp148=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp147=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL40:
# 640
 new_e=({void*_tmpA87=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->tag=30U,({void*_tmpA86=Cyc_Tcutil_copy_type(_tmp148);_tmpF8->f1=_tmpA86;}),({struct Cyc_List_List*_tmpA85=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp147);_tmpF8->f2=_tmpA85;});_tmpF8;});Cyc_Absyn_new_exp(_tmpA87,e->loc);});
goto _LL0;case 31U: _LL41: _tmp14B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp14A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp149=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_LL42:
# 643
 new_e=({void*_tmpA89=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->tag=31U,({struct Cyc_List_List*_tmpA88=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp14B);_tmpF9->f1=_tmpA88;}),_tmpF9->f2=_tmp14A,_tmpF9->f3=_tmp149;_tmpF9;});Cyc_Absyn_new_exp(_tmpA89,e->loc);});
goto _LL0;case 32U: _LL43: _tmp14D=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp14C=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL44:
 new_e=e;goto _LL0;case 33U: _LL45: _tmp14F=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp14E=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL46:
# 647
 new_e=({void*_tmpA8B=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=33U,({void*_tmpA8A=Cyc_Tcutil_copy_type(_tmp14F);_tmpFA->f1=_tmpA8A;}),_tmpFA->f2=_tmp14E;_tmpFA;});Cyc_Absyn_new_exp(_tmpA8B,e->loc);});
goto _LL0;case 34U: _LL47: _tmp155=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).is_calloc;_tmp154=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).rgn;_tmp153=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).elt_type;_tmp152=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).num_elts;_tmp151=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).fat_result;_tmp150=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).inline_call;_LL48: {
# 650
struct Cyc_Absyn_Exp*_tmpFB=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp154;if(_tmp154 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp154);{
void**t1=_tmp153;if(_tmp153 != 0)t1=({void**_tmpFC=_cycalloc(sizeof(*_tmpFC));({void*_tmpA8C=Cyc_Tcutil_copy_type(*_tmp153);*_tmpFC=_tmpA8C;});_tmpFC;});
({void*_tmpA8D=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->tag=34U,(_tmpFD->f1).is_calloc=_tmp155,(_tmpFD->f1).rgn=r1,(_tmpFD->f1).elt_type=t1,(_tmpFD->f1).num_elts=_tmp152,(_tmpFD->f1).fat_result=_tmp151,(_tmpFD->f1).inline_call=_tmp150;_tmpFD;});_tmpFB->r=_tmpA8D;});
new_e=_tmpFB;
goto _LL0;};}case 35U: _LL49: _tmp157=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp156=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL4A:
 new_e=({struct Cyc_Absyn_Exp*_tmpA8F=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp157);struct Cyc_Absyn_Exp*_tmpA8E=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp156);Cyc_Absyn_swap_exp(_tmpA8F,_tmpA8E,e->loc);});goto _LL0;case 36U: _LL4B: _tmp159=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp158=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL4C: {
# 658
struct Cyc_Core_Opt*nopt1=_tmp159;
if(_tmp159 != 0)nopt1=({struct Cyc_Core_Opt*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->v=(struct _tuple2*)_tmp159->v;_tmpFE;});
new_e=({void*_tmpA91=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->tag=36U,_tmpFF->f1=nopt1,({struct Cyc_List_List*_tmpA90=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp158);_tmpFF->f2=_tmpA90;});_tmpFF;});Cyc_Absyn_new_exp(_tmpA91,e->loc);});
goto _LL0;}case 37U: _LL4D: _LL4E:
# 663
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpA92=({const char*_tmp100="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp100,sizeof(char),45U);});_tmp101->f1=_tmpA92;});_tmp101;}));case 38U: _LL4F: _tmp15B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp15A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL50:
 new_e=({void*_tmpA94=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->tag=38U,({struct Cyc_Absyn_Exp*_tmpA93=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp15B);_tmp102->f1=_tmpA93;}),_tmp102->f2=_tmp15A;_tmp102;});Cyc_Absyn_new_exp(_tmpA94,e->loc);});
goto _LL0;case 39U: _LL51: _tmp15C=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL52:
 new_e=({void*_tmpA95=Cyc_Tcutil_copy_type(_tmp15C);Cyc_Absyn_valueof_exp(_tmpA95,e->loc);});
goto _LL0;default: _LL53: _tmp15E=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp15D=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL54:
 new_e=Cyc_Absyn_asm_exp(_tmp15E,_tmp15D,e->loc);goto _LL0;}_LL0:;}
# 671
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 675
return new_e;}
# 678
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple19{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 691 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp15F=ka1;enum Cyc_Absyn_KindQual _tmp168;enum Cyc_Absyn_AliasQual _tmp167;_LL1: _tmp168=_tmp15F->kind;_tmp167=_tmp15F->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp160=ka2;enum Cyc_Absyn_KindQual _tmp166;enum Cyc_Absyn_AliasQual _tmp165;_LL4: _tmp166=_tmp160->kind;_tmp165=_tmp160->aliasqual;_LL5:;
# 695
if((int)_tmp168 != (int)_tmp166){
struct _tuple19 _tmp161=({struct _tuple19 _tmp984;_tmp984.f1=_tmp168,_tmp984.f2=_tmp166;_tmp984;});struct _tuple19 _tmp162=_tmp161;switch(_tmp162.f1){case Cyc_Absyn_BoxKind: switch(_tmp162.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp162.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 704
if((int)_tmp167 != (int)_tmp165){
struct _tuple20 _tmp163=({struct _tuple20 _tmp985;_tmp985.f1=_tmp167,_tmp985.f2=_tmp165;_tmp985;});struct _tuple20 _tmp164=_tmp163;switch(_tmp164.f1){case Cyc_Absyn_Aliasable: if(_tmp164.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp164.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 711
return 1;};}
# 714
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp169=Cyc_Absyn_compress_kb(tv->kind);void*_tmp16A=_tmp169;struct Cyc_Absyn_Kind*_tmp16D;struct Cyc_Absyn_Kind*_tmp16C;switch(*((int*)_tmp16A)){case 0U: _LL1: _tmp16C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16A)->f1;_LL2:
 return _tmp16C;case 2U: _LL3: _tmp16D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16A)->f2;_LL4:
 return _tmp16D;default: _LL5: _LL6:
# 719
({void*_tmpA96=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=2U,_tmp16B->f1=0,_tmp16B->f2=def;_tmp16B;});tv->kind=_tmpA96;});
return def;}_LL0:;}
# 724
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp16E=({struct _tuple0 _tmp986;({void*_tmpA98=Cyc_Absyn_compress_kb(kb1);_tmp986.f1=_tmpA98;}),({void*_tmpA97=Cyc_Absyn_compress_kb(kb2);_tmp986.f2=_tmpA97;});_tmp986;});struct _tuple0 _tmp16F=_tmp16E;struct Cyc_Core_Opt**_tmp184;void*_tmp183;void*_tmp182;struct Cyc_Core_Opt**_tmp181;struct Cyc_Core_Opt**_tmp180;struct Cyc_Absyn_Kind*_tmp17F;struct Cyc_Core_Opt**_tmp17E;struct Cyc_Absyn_Kind*_tmp17D;struct Cyc_Core_Opt**_tmp17C;struct Cyc_Absyn_Kind*_tmp17B;struct Cyc_Absyn_Kind*_tmp17A;struct Cyc_Absyn_Kind*_tmp179;struct Cyc_Core_Opt**_tmp178;struct Cyc_Absyn_Kind*_tmp177;struct Cyc_Absyn_Kind*_tmp176;struct Cyc_Absyn_Kind*_tmp175;switch(*((int*)_tmp16F.f1)){case 0U: switch(*((int*)_tmp16F.f2)){case 0U: _LL1: _tmp176=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16F.f1)->f1;_tmp175=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16F.f2)->f1;_LL2:
 return _tmp176 == _tmp175;case 2U: _LL5: _tmp179=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16F.f1)->f1;_tmp178=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16F.f2)->f1;_tmp177=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16F.f2)->f2;_LL6:
# 733
 if(Cyc_Tcutil_kind_leq(_tmp179,_tmp177)){
({struct Cyc_Core_Opt*_tmpA99=({struct Cyc_Core_Opt*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->v=kb1;_tmp171;});*_tmp178=_tmpA99;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp16F.f2)){case 0U: _LL3: _tmp17C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16F.f1)->f1;_tmp17B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16F.f1)->f2;_tmp17A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16F.f2)->f1;_LL4:
# 728
 if(Cyc_Tcutil_kind_leq(_tmp17A,_tmp17B)){
({struct Cyc_Core_Opt*_tmpA9A=({struct Cyc_Core_Opt*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->v=kb2;_tmp170;});*_tmp17C=_tmpA9A;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp180=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16F.f1)->f1;_tmp17F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16F.f1)->f2;_tmp17E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16F.f2)->f1;_tmp17D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16F.f2)->f2;_LL8:
# 738
 if(Cyc_Tcutil_kind_leq(_tmp17F,_tmp17D)){
({struct Cyc_Core_Opt*_tmpA9B=({struct Cyc_Core_Opt*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->v=kb1;_tmp172;});*_tmp17E=_tmpA9B;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp17D,_tmp17F)){
({struct Cyc_Core_Opt*_tmpA9C=({struct Cyc_Core_Opt*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->v=kb2;_tmp173;});*_tmp180=_tmpA9C;});
return 1;}else{
return 0;}}default: _LLB: _tmp182=_tmp16F.f1;_tmp181=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp16F.f2)->f1;_LLC:
# 747
({struct Cyc_Core_Opt*_tmpA9D=({struct Cyc_Core_Opt*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->v=_tmp182;_tmp174;});*_tmp181=_tmpA9D;});
return 1;}default: _LL9: _tmp184=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp16F.f1)->f1;_tmp183=_tmp16F.f2;_LLA:
# 745
 _tmp182=_tmp183;_tmp181=_tmp184;goto _LLC;}_LL0:;}
# 752
struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp185=Cyc_Tcutil_compress(t);void*_tmp186=_tmp185;struct Cyc_Absyn_Tvar*_tmp18B;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp186)->tag == 2U){_LL1: _tmp18B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp186)->f1;_LL2: {
# 755
void*_tmp187=_tmp18B->kind;
_tmp18B->kind=kb;
return({struct _tuple15 _tmp987;_tmp987.f1=_tmp18B,_tmp987.f2=_tmp187;_tmp987;});}}else{_LL3: _LL4:
# 759
({struct Cyc_String_pa_PrintArg_struct _tmp18A=({struct Cyc_String_pa_PrintArg_struct _tmp988;_tmp988.tag=0U,({struct _dyneither_ptr _tmpA9E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp988.f1=_tmpA9E;});_tmp988;});void*_tmp188[1U];_tmp188[0]=& _tmp18A;({struct _dyneither_ptr _tmpA9F=({const char*_tmp189="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp189,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA9F,_tag_dyneither(_tmp188,sizeof(void*),1U));});});}_LL0:;}
# 765
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 767
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 771
struct _RegionHandle _tmp18C=_new_region("temp");struct _RegionHandle*temp=& _tmp18C;_push_region(temp);
{struct Cyc_List_List*_tmp18D=0;
# 774
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp18E=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp18F=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp190=Cyc_Tcutil_tvar_kind(_tmp18E,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp191=_tmp190;switch(((struct Cyc_Absyn_Kind*)_tmp191)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 780
 _tmp18D=({struct Cyc_List_List*_tmp193=_region_malloc(temp,sizeof(*_tmp193));({struct _tuple15*_tmpAA0=({struct _tuple15*_tmp192=_region_malloc(temp,sizeof(*_tmp192));_tmp192->f1=_tmp18E,_tmp192->f2=_tmp18F;_tmp192;});_tmp193->hd=_tmpAA0;}),_tmp193->tl=_tmp18D;_tmp193;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 784
if(_tmp18D != 0){
field_type=({struct _RegionHandle*_tmpAA2=temp;struct Cyc_List_List*_tmpAA1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp18D);Cyc_Tcutil_rsubstitute(_tmpAA2,_tmpAA1,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 772
;_pop_region(temp);}
# 789
return k;}
# 796
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 798
void*_tmp194=Cyc_Tcutil_compress(t);void*_tmp195=_tmp194;struct Cyc_Absyn_Typedefdecl*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_PtrInfo _tmp1B1;void*_tmp1B0;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Tvar*_tmp1AE;struct Cyc_Core_Opt*_tmp1AD;switch(*((int*)_tmp195)){case 1U: _LL1: _tmp1AD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp195)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1AD))->v;case 2U: _LL3: _tmp1AE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp195)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp1AE,& Cyc_Tcutil_bk);case 0U: _LL5: _tmp1B0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1;_tmp1AF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2;_LL6: {
# 802
void*_tmp196=_tmp1B0;enum Cyc_Absyn_AggrKind _tmp1A3;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_AggrdeclImpl*_tmp1A1;int _tmp1A0;struct Cyc_Absyn_Kind*_tmp19F;enum Cyc_Absyn_Size_of _tmp19E;switch(*((int*)_tmp196)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp19E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp196)->f2;_LL21:
 return((int)_tmp19E == (int)2U  || (int)_tmp19E == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp19F=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp196)->f2;_LL31:
 return _tmp19F;case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp196)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 823
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 825
({void*_tmp197=0U;({struct _dyneither_ptr _tmpAA3=({const char*_tmp198="type_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp198,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA3,_tag_dyneither(_tmp197,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp196)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 829
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp1A3=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp196)->f1).KnownAggr).val)->kind;_tmp1A2=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp196)->f1).KnownAggr).val)->tvs;_tmp1A1=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp196)->f1).KnownAggr).val)->impl;_tmp1A0=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp196)->f1).KnownAggr).val)->expected_mem_kind;_LL4B:
# 831
 if(_tmp1A1 == 0){
if(_tmp1A0)
return& Cyc_Tcutil_mk;else{
# 835
return& Cyc_Tcutil_ak;}}{
# 837
struct Cyc_List_List*_tmp199=_tmp1A1->fields;
if(_tmp199 == 0)return& Cyc_Tcutil_mk;
# 840
if((int)_tmp1A3 == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(_tmp199))->tl != 0;_tmp199=_tmp199->tl){;}{
void*_tmp19A=((struct Cyc_Absyn_Aggrfield*)_tmp199->hd)->type;
struct Cyc_Absyn_Kind*_tmp19B=Cyc_Tcutil_field_kind(_tmp19A,_tmp1AF,_tmp1A2);
if(_tmp19B == & Cyc_Tcutil_ak  || _tmp19B == & Cyc_Tcutil_tak)return _tmp19B;};}else{
# 848
for(0;_tmp199 != 0;_tmp199=_tmp199->tl){
void*_tmp19C=((struct Cyc_Absyn_Aggrfield*)_tmp199->hd)->type;
struct Cyc_Absyn_Kind*_tmp19D=Cyc_Tcutil_field_kind(_tmp19C,_tmp1AF,_tmp1A2);
if(_tmp19D == & Cyc_Tcutil_ak  || _tmp19D == & Cyc_Tcutil_tak)return _tmp19D;}}
# 854
return& Cyc_Tcutil_mk;};}}_LL1D:;}case 5U: _LL7: _LL8:
# 856
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp1B1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195)->f1;_LLC: {
# 859
void*_tmp1A4=Cyc_Tcutil_compress((_tmp1B1.ptr_atts).bounds);void*_tmp1A5=_tmp1A4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->f1)){case 13U: _LL4D: _LL4E: {
# 861
enum Cyc_Absyn_AliasQual _tmp1A6=(Cyc_Tcutil_type_kind((_tmp1B1.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1A7=_tmp1A6;switch(_tmp1A7){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 867
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 869
enum Cyc_Absyn_AliasQual _tmp1A8=(Cyc_Tcutil_type_kind((_tmp1B1.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1A9=_tmp1A8;switch(_tmp1A9){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 875
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 879
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp1B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).num_elts;_LL12:
# 881
 if(_tmp1B2 == 0  || Cyc_Tcutil_is_const_exp(_tmp1B2))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp1B3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp195)->f3;_LL16:
# 885
 if(_tmp1B3 == 0  || _tmp1B3->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1AC=({struct Cyc_String_pa_PrintArg_struct _tmp989;_tmp989.tag=0U,({struct _dyneither_ptr _tmpAA4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp989.f1=_tmpAA4;});_tmp989;});void*_tmp1AA[1U];_tmp1AA[0]=& _tmp1AC;({struct _dyneither_ptr _tmpAA5=({const char*_tmp1AB="type_kind: typedef found: %s";_tag_dyneither(_tmp1AB,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA5,_tag_dyneither(_tmp1AA,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B3->kind))->v;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp195)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 894
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || (int)k1->kind == (int)k2->kind  && (int)k1->aliasqual == (int)k2->aliasqual;}
# 899
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1B4;_push_handler(& _tmp1B4);{int _tmp1B6=0;if(setjmp(_tmp1B4.handler))_tmp1B6=1;if(!_tmp1B6){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1B7=1;_npop_handler(0U);return _tmp1B7;};
# 901
;_pop_handler();}else{void*_tmp1B5=(void*)_exn_thrown;void*_tmp1B8=_tmp1B5;void*_tmp1B9;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1B8)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 903
 return 0;}else{_LL3: _tmp1B9=_tmp1B8;_LL4:(int)_rethrow(_tmp1B9);}_LL0:;}};}
# 908
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1BA=t;struct Cyc_List_List*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_List_List*_tmp1D8;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List*_tmp1D6;void*_tmp1D5;struct Cyc_Absyn_Tqual _tmp1D4;void*_tmp1D3;struct Cyc_List_List*_tmp1D2;int _tmp1D1;struct Cyc_Absyn_VarargInfo*_tmp1D0;struct Cyc_List_List*_tmp1CF;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_List_List*_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_List_List*_tmp1CA;void*_tmp1C9;void*_tmp1C8;struct Cyc_Absyn_PtrInfo _tmp1C7;void*_tmp1C6;struct Cyc_Core_Opt**_tmp1C5;struct Cyc_Absyn_Tvar*_tmp1C4;switch(*((int*)_tmp1BA)){case 2U: _LL1: _tmp1C4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1BA)->f1;_LL2:
# 913
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1C4)){
Cyc_Tcutil_failure_reason=({const char*_tmp1BB="(type variable would escape scope)";_tag_dyneither(_tmp1BB,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 917
goto _LL0;case 1U: _LL3: _tmp1C6=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BA)->f2;_tmp1C5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BA)->f4;_LL4:
# 919
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1BC="(occurs check)";_tag_dyneither(_tmp1BC,sizeof(char),15U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 923
if(_tmp1C6 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1C6);else{
# 926
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C5))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 933
if(problem){
struct Cyc_List_List*_tmp1BD=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C5))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1BD=({struct Cyc_List_List*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp1BE->tl=_tmp1BD;_tmp1BE;});}}
# 939
({struct Cyc_Core_Opt*_tmpAA6=({struct Cyc_Core_Opt*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->v=_tmp1BD;_tmp1BF;});*_tmp1C5=_tmpAA6;});}}}
# 942
goto _LL0;case 3U: _LL5: _tmp1C7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->f1;_LL6:
# 944
 Cyc_Tcutil_occurs(evar,r,env,_tmp1C7.elt_type);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C7.ptr_atts).rgn);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C7.ptr_atts).nullable);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C7.ptr_atts).bounds);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C7.ptr_atts).zero_term);
goto _LL0;case 4U: _LL7: _tmp1C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BA)->f1).elt_type;_tmp1C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BA)->f1).zero_term;_LL8:
# 952
 Cyc_Tcutil_occurs(evar,r,env,_tmp1C9);
Cyc_Tcutil_occurs(evar,r,env,_tmp1C8);
goto _LL0;case 5U: _LL9: _tmp1D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).tvars;_tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).effect;_tmp1D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).ret_tqual;_tmp1D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).ret_type;_tmp1D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).args;_tmp1D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).c_varargs;_tmp1D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).cyc_varargs;_tmp1CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).rgn_po;_tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).attributes;_tmp1CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).requires_clause;_tmp1CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).requires_relns;_tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).ensures_clause;_tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BA)->f1).ensures_relns;_LLA:
# 957
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1D6,env);
if(_tmp1D5 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1D5);
Cyc_Tcutil_occurs(evar,r,env,_tmp1D3);
for(0;_tmp1D2 != 0;_tmp1D2=_tmp1D2->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1D2->hd)).f3);}
if(_tmp1D0 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1D0->type);
for(0;_tmp1CF != 0;_tmp1CF=_tmp1CF->tl){
struct _tuple0*_tmp1C0=(struct _tuple0*)_tmp1CF->hd;struct _tuple0*_tmp1C1=_tmp1C0;void*_tmp1C3;void*_tmp1C2;_LL16: _tmp1C3=_tmp1C1->f1;_tmp1C2=_tmp1C1->f2;_LL17:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1C3);
Cyc_Tcutil_occurs(evar,r,env,_tmp1C2);}
# 969
goto _LL0;case 6U: _LLB: _tmp1D7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1BA)->f1;_LLC:
# 971
 for(0;_tmp1D7 != 0;_tmp1D7=_tmp1D7->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1D7->hd)).f2);}
goto _LL0;case 7U: _LLD: _tmp1D8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1BA)->f2;_LLE:
# 976
 for(0;_tmp1D8 != 0;_tmp1D8=_tmp1D8->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1D8->hd)->type);}
goto _LL0;case 8U: _LLF: _tmp1D9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1BA)->f2;_LL10:
 _tmp1DA=_tmp1D9;goto _LL12;case 0U: _LL11: _tmp1DA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BA)->f2;_LL12:
# 981
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1DA);goto _LL0;default: _LL13: _LL14:
# 984
 goto _LL0;}_LL0:;};}
# 987
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 989
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 994
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1002
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1DB=0U;({struct _dyneither_ptr _tmpAA7=({const char*_tmp1DC="tq1 real_const not set.";_tag_dyneither(_tmp1DC,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA7,_tag_dyneither(_tmp1DB,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1DD=0U;({struct _dyneither_ptr _tmpAA8=({const char*_tmp1DE="tq2 real_const not set.";_tag_dyneither(_tmp1DE,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA8,_tag_dyneither(_tmp1DD,sizeof(void*),0U));});});
# 1008
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 1011
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp1DF="(qualifiers don't match)";_tag_dyneither(_tmp1DF,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1019
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 1023
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 1029
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp1E0=({struct _tuple0 _tmp98A;_tmp98A.f1=e1->r,_tmp98A.f2=e2->r;_tmp98A;});struct _tuple0 _tmp1E1=_tmp1E0;void*_tmp1EB;void*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1E8;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E1.f1)->tag == 14U){_LL1: _tmp1E8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E1.f1)->f2;_LL2:
# 1034
 Cyc_Tcutil_unify_cmp_exp(_tmp1E8,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E1.f2)->tag == 14U){_LL3: _tmp1E9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E1.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp1E9);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E1.f1)->tag == 39U){_LL5: _tmp1EA=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E1.f1)->f1;_LL6: {
# 1037
void*_tmp1E2=Cyc_Tcutil_compress(_tmp1EA);void*_tmp1E3=_tmp1E2;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E3)->tag == 1U){_LLC: _LLD:
({void*_tmpAA9=_tmp1EA;Cyc_Tcutil_unify_it(_tmpAA9,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4->tag=9U,_tmp1E4->f1=e2;_tmp1E4;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E1.f2)->tag == 39U){_LL7: _tmp1EB=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E1.f2)->f1;_LL8: {
# 1042
void*_tmp1E5=Cyc_Tcutil_compress(_tmp1EB);void*_tmp1E6=_tmp1E5;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E6)->tag == 1U){_LL11: _LL12:
({void*_tmpAAA=_tmp1EB;Cyc_Tcutil_unify_it(_tmpAAA,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->tag=9U,_tmp1E7->f1=e1;_tmp1E7;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 1046
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 1050
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp1EC=att;switch(*((int*)_tmp1EC)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _LL4:
 return 1;case 2U: _LL5: _LL6:
 return 2;case 3U: _LL7: _LL8:
 return 3;case 4U: _LL9: _LLA:
 return 4;case 5U: _LLB: _LLC:
 return 5;case 6U: _LLD: _LLE:
 return 6;case 7U: _LLF: _LL10:
 return 7;case 8U: _LL11: _LL12:
 return 8;case 9U: _LL13: _LL14:
 return 9;case 10U: _LL15: _LL16:
 return 10;case 11U: _LL17: _LL18:
 return 11;case 12U: _LL19: _LL1A:
 return 12;case 13U: _LL1B: _LL1C:
 return 13;case 14U: _LL1D: _LL1E:
 return 14;case 15U: _LL1F: _LL20:
 return 15;case 16U: _LL21: _LL22:
 return 16;case 17U: _LL23: _LL24:
 return 17;case 18U: _LL25: _LL26:
 return 18;case 19U: _LL27: _LL28:
 return 19;case 20U: _LL29: _LL2A:
 return 20;default: _LL2B: _LL2C:
 return 21;}_LL0:;}
# 1077
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp1ED=({struct _tuple0 _tmp98B;_tmp98B.f1=att1,_tmp98B.f2=att2;_tmp98B;});struct _tuple0 _tmp1EE=_tmp1ED;enum Cyc_Absyn_Format_Type _tmp1FE;int _tmp1FD;int _tmp1FC;enum Cyc_Absyn_Format_Type _tmp1FB;int _tmp1FA;int _tmp1F9;struct _dyneither_ptr _tmp1F8;struct _dyneither_ptr _tmp1F7;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F5;int _tmp1F4;int _tmp1F3;int _tmp1F2;int _tmp1F1;switch(*((int*)_tmp1EE.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1EE.f2)->tag == 0U){_LL1: _tmp1F2=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1EE.f1)->f1;_tmp1F1=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1EE.f2)->f1;_LL2:
 _tmp1F4=_tmp1F2;_tmp1F3=_tmp1F1;goto _LL4;}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1EE.f2)->tag == 20U){_LL3: _tmp1F4=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1EE.f1)->f1;_tmp1F3=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1EE.f2)->f1;_LL4:
# 1081
 return Cyc_Core_intcmp(_tmp1F4,_tmp1F3);}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1EE.f2)->tag == 6U){_LL5: _tmp1F6=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1EE.f1)->f1;_tmp1F5=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1EE.f2)->f1;_LL6:
# 1083
 if(_tmp1F6 == _tmp1F5)return 0;
if(_tmp1F6 == 0)return - 1;
if(_tmp1F5 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp1F6,_tmp1F5);}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1EE.f2)->tag == 8U){_LL7: _tmp1F8=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1EE.f1)->f1;_tmp1F7=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1EE.f2)->f1;_LL8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1F8,(struct _dyneither_ptr)_tmp1F7);}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1EE.f2)->tag == 19U){_LL9: _tmp1FE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1EE.f1)->f1;_tmp1FD=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1EE.f1)->f2;_tmp1FC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1EE.f1)->f3;_tmp1FB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1EE.f2)->f1;_tmp1FA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1EE.f2)->f2;_tmp1F9=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1EE.f2)->f3;_LLA: {
# 1089
int _tmp1EF=Cyc_Core_intcmp((int)((unsigned int)_tmp1FE),(int)((unsigned int)_tmp1FB));
if(_tmp1EF != 0)return _tmp1EF;{
int _tmp1F0=Cyc_Core_intcmp(_tmp1FD,_tmp1FA);
if(_tmp1F0 != 0)return _tmp1F0;
return Cyc_Core_intcmp(_tmp1FC,_tmp1F9);};}}else{goto _LLB;}default: _LLB: _LLC:
# 1095
 return({int _tmpAAB=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpAAB,Cyc_Tcutil_attribute_case_number(att2));});}_LL0:;}
# 1099
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 1103
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 1112
static void*Cyc_Tcutil_rgns_of(void*t);
# 1114
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 1118
static struct _tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp1FF=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp200=_tmp1FF;switch(((struct Cyc_Absyn_Kind*)_tmp200)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp200)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->tag=9U,({struct Cyc_Absyn_Exp*_tmpAAC=Cyc_Absyn_uint_exp(0U,0U);_tmp201->f1=_tmpAAC;});_tmp201;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 1129
return({struct _tuple15*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->f1=tv,_tmp202->f2=t;_tmp202;});}
# 1136
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp203=Cyc_Tcutil_compress(t);void*_tmp204=_tmp203;struct Cyc_List_List*_tmp21A;struct Cyc_List_List*_tmp219;struct Cyc_List_List*_tmp218;void*_tmp217;struct Cyc_Absyn_Tqual _tmp216;void*_tmp215;struct Cyc_List_List*_tmp214;struct Cyc_Absyn_VarargInfo*_tmp213;struct Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp211;void*_tmp210;void*_tmp20F;void*_tmp20E;struct Cyc_List_List*_tmp20D;switch(*((int*)_tmp204)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp204)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp204)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp20D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp204)->f2;_LL6: {
# 1141
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp20D);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 1145
struct Cyc_Absyn_Kind*_tmp205=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp206=_tmp205;switch(((struct Cyc_Absyn_Kind*)_tmp206)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp20F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp204)->f1).elt_type;_tmp20E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp204)->f1).ptr_atts).rgn;_LLC:
# 1153
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp207[2U];({void*_tmpAAE=Cyc_Absyn_access_eff(_tmp20E);_tmp207[0]=_tmpAAE;}),({void*_tmpAAD=Cyc_Tcutil_rgns_of(_tmp20F);_tmp207[1]=_tmpAAD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp207,sizeof(void*),2U));})));case 4U: _LLD: _tmp210=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp204)->f1).elt_type;_LLE:
# 1155
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp210));case 7U: _LLF: _tmp211=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp204)->f2;_LL10:
# 1157
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp211)));case 5U: _LL11: _tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp204)->f1).tvars;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp204)->f1).effect;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp204)->f1).ret_tqual;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp204)->f1).ret_type;_tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp204)->f1).args;_tmp213=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp204)->f1).cyc_varargs;_tmp212=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp204)->f1).rgn_po;_LL12: {
# 1166
void*_tmp208=({struct Cyc_List_List*_tmpAAF=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp218);Cyc_Tcutil_substitute(_tmpAAF,(void*)_check_null(_tmp217));});
return Cyc_Tcutil_normalize_effect(_tmp208);}case 6U: _LL13: _tmp219=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp204)->f1;_LL14: {
# 1169
struct Cyc_List_List*_tmp209=0;
for(0;_tmp219 != 0;_tmp219=_tmp219->tl){
_tmp209=({struct Cyc_List_List*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->hd=(*((struct _tuple12*)_tmp219->hd)).f2,_tmp20A->tl=_tmp209;_tmp20A;});}
_tmp21A=_tmp209;goto _LL16;}case 8U: _LL15: _tmp21A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp204)->f2;_LL16:
# 1174
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp21A)));case 10U: _LL17: _LL18:
({void*_tmp20B=0U;({struct _dyneither_ptr _tmpAB0=({const char*_tmp20C="typedecl in rgns_of";_tag_dyneither(_tmp20C,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAB0,_tag_dyneither(_tmp20B,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 1184
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp21B=e;void*_tmp227;struct Cyc_List_List**_tmp226;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21B)->f1)){case 9U: _LL1: _tmp226=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21B)->f2;_LL2: {
# 1188
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp226;for(0;effs != 0;effs=effs->tl){
void*_tmp21C=(void*)effs->hd;
({void*_tmpAB1=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp21C));effs->hd=_tmpAB1;});{
void*_tmp21D=(void*)effs->hd;void*_tmp21E=_tmp21D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21E)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21E)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21E)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21E)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21E)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21E)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21E)->f2)->tl == 0){_LLE: _LLF:
# 1197
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 1201
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp226;for(0;effs != 0;effs=effs->tl){
void*_tmp21F=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp220=_tmp21F;void*_tmp223;struct Cyc_List_List*_tmp222;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f1)){case 9U: _LL13: _tmp222=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2;_LL14:
# 1206
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp222,effects);
goto _LL12;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp223=_tmp220;_LL1C:
 effects=({struct Cyc_List_List*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221->hd=_tmp223,_tmp221->tl=effects;_tmp221;});goto _LL12;}_LL12:;}}
# 1214
({struct Cyc_List_List*_tmpAB2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp226=_tmpAB2;});
return e;};}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21B)->f2 != 0){_LL3: _tmp227=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21B)->f2)->hd;_LL4: {
# 1217
void*_tmp224=Cyc_Tcutil_compress(_tmp227);void*_tmp225=_tmp224;switch(*((int*)_tmp225)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp227);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 1222
 return e;}_LL0:;};}
# 1227
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp228=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->tag=5U,(_tmp229->f1).tvars=0,(_tmp229->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmpAB3=Cyc_Absyn_empty_tqual(0U);(_tmp229->f1).ret_tqual=_tmpAB3;}),(_tmp229->f1).ret_type=Cyc_Absyn_void_type,(_tmp229->f1).args=0,(_tmp229->f1).c_varargs=0,(_tmp229->f1).cyc_varargs=0,(_tmp229->f1).rgn_po=0,(_tmp229->f1).attributes=0,(_tmp229->f1).requires_clause=0,(_tmp229->f1).requires_relns=0,(_tmp229->f1).ensures_clause=0,(_tmp229->f1).ensures_relns=0;_tmp229;});
# 1237
return({void*_tmpAB7=(void*)_tmp228;void*_tmpAB6=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmpAB5=Cyc_Absyn_empty_tqual(0U);void*_tmpAB4=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmpAB7,_tmpAB6,_tmpAB5,_tmpAB4,Cyc_Absyn_false_type);});}
# 1244
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type  || r == Cyc_Absyn_unique_rgn_type) || r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp22A=Cyc_Tcutil_compress(e);void*_tmp22B=_tmp22A;struct Cyc_Core_Opt*_tmp246;void**_tmp245;struct Cyc_Core_Opt*_tmp244;void*_tmp243;struct Cyc_List_List*_tmp242;void*_tmp241;switch(*((int*)_tmp22B)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f2 != 0){_LL1: _tmp241=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f2)->hd;_LL2:
# 1251
 if(constrain)return Cyc_Tcutil_unify(r,_tmp241);
_tmp241=Cyc_Tcutil_compress(_tmp241);
if(r == _tmp241)return 1;{
struct _tuple0 _tmp22C=({struct _tuple0 _tmp98C;_tmp98C.f1=r,_tmp98C.f2=_tmp241;_tmp98C;});struct _tuple0 _tmp22D=_tmp22C;struct Cyc_Absyn_Tvar*_tmp22F;struct Cyc_Absyn_Tvar*_tmp22E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22D.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22D.f2)->tag == 2U){_LLC: _tmp22F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22D.f1)->f1;_tmp22E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22D.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp22F,_tmp22E)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};}else{goto _LL9;}case 9U: _LL3: _tmp242=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f2;_LL4:
# 1259
 for(0;_tmp242 != 0;_tmp242=_tmp242->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp242->hd))return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f2 != 0){_LL5: _tmp243=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f2)->hd;_LL6: {
# 1263
void*_tmp230=Cyc_Tcutil_rgns_of(_tmp243);void*_tmp231=_tmp230;void*_tmp23B;void*_tmp23A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp231)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp231)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp231)->f2 != 0){_LL11: _tmp23A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp231)->f2)->hd;_LL12:
# 1265
 if(!constrain)return 0;{
void*_tmp232=Cyc_Tcutil_compress(_tmp23A);void*_tmp233=_tmp232;struct Cyc_Core_Opt*_tmp239;void**_tmp238;struct Cyc_Core_Opt*_tmp237;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp233)->tag == 1U){_LL16: _tmp239=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp233)->f1;_tmp238=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp233)->f2;_tmp237=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp233)->f4;_LL17: {
# 1270
void*_tmp234=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp237);
# 1273
Cyc_Tcutil_occurs(_tmp234,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp237))->v,r);{
void*_tmp235=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp236[2U];_tmp236[0]=_tmp234,({void*_tmpAB8=Cyc_Absyn_access_eff(r);_tmp236[1]=_tmpAB8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp236,sizeof(void*),2U));})));
*_tmp238=_tmp235;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp23B=_tmp231;_LL14:
# 1279
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp23B);}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp246=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22B)->f1;_tmp245=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22B)->f2;_tmp244=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22B)->f4;_LL8:
# 1282
 if(_tmp246 == 0  || (int)((struct Cyc_Absyn_Kind*)_tmp246->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp23C=0U;({struct _dyneither_ptr _tmpAB9=({const char*_tmp23D="effect evar has wrong kind";_tag_dyneither(_tmp23D,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAB9,_tag_dyneither(_tmp23C,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1287
void*_tmp23E=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp244);
# 1290
Cyc_Tcutil_occurs(_tmp23E,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp244))->v,r);{
void*_tmp23F=Cyc_Absyn_join_eff(({void*_tmp240[2U];_tmp240[0]=_tmp23E,({void*_tmpABA=Cyc_Absyn_access_eff(r);_tmp240[1]=_tmpABA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp240,sizeof(void*),2U));}));
*_tmp245=_tmp23F;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1301
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp247=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp248=_tmp247;struct Cyc_Core_Opt*_tmp25A;void**_tmp259;struct Cyc_Core_Opt*_tmp258;void*_tmp257;struct Cyc_List_List*_tmp256;switch(*((int*)_tmp248)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp256=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f2;_LL4:
# 1306
 for(0;_tmp256 != 0;_tmp256=_tmp256->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp256->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f2 != 0){_LL5: _tmp257=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f2)->hd;_LL6:
# 1311
 _tmp257=Cyc_Tcutil_compress(_tmp257);
if(t == _tmp257)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp257);{
void*_tmp249=Cyc_Tcutil_rgns_of(t);void*_tmp24A=_tmp249;void*_tmp250;void*_tmp24F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f2 != 0){_LLC: _tmp24F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f2)->hd;_LLD: {
# 1316
struct _tuple0 _tmp24B=({struct _tuple0 _tmp98D;({void*_tmpABB=Cyc_Tcutil_compress(_tmp24F);_tmp98D.f1=_tmpABB;}),_tmp98D.f2=_tmp257;_tmp98D;});struct _tuple0 _tmp24C=_tmp24B;struct Cyc_Absyn_Tvar*_tmp24E;struct Cyc_Absyn_Tvar*_tmp24D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24C.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24C.f2)->tag == 2U){_LL11: _tmp24E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24C.f1)->f1;_tmp24D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24C.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp257);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp24F == _tmp257;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp250=_tmp24A;_LLF:
# 1320
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp250);}_LLB:;};}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp25A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp248)->f1;_tmp259=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp248)->f2;_tmp258=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp248)->f4;_LL8:
# 1323
 if(_tmp25A == 0  || (int)((struct Cyc_Absyn_Kind*)_tmp25A->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp251=0U;({struct _dyneither_ptr _tmpABC=({const char*_tmp252="effect evar has wrong kind";_tag_dyneither(_tmp252,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpABC,_tag_dyneither(_tmp251,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1328
void*_tmp253=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp258);
# 1331
Cyc_Tcutil_occurs(_tmp253,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp258))->v,t);{
void*_tmp254=Cyc_Absyn_join_eff(({void*_tmp255[2U];_tmp255[0]=_tmp253,({void*_tmpABD=Cyc_Absyn_regionsof_eff(t);_tmp255[1]=_tmpABD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp255,sizeof(void*),2U));}));
*_tmp259=_tmp254;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1342
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp25B=e;struct Cyc_Core_Opt*_tmp272;void**_tmp271;struct Cyc_Core_Opt*_tmp270;void*_tmp26F;struct Cyc_List_List*_tmp26E;struct Cyc_Absyn_Tvar*_tmp26D;switch(*((int*)_tmp25B)){case 2U: _LL1: _tmp26D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp25B)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp26D)== 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B)->f1)){case 9U: _LL3: _tmp26E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B)->f2;_LL4:
# 1347
 for(0;_tmp26E != 0;_tmp26E=_tmp26E->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp26E->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B)->f2 != 0){_LL5: _tmp26F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B)->f2)->hd;_LL6: {
# 1352
void*_tmp25C=Cyc_Tcutil_rgns_of(_tmp26F);void*_tmp25D=_tmp25C;void*_tmp267;void*_tmp266;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f2 != 0){_LLC: _tmp266=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f2)->hd;_LLD:
# 1354
 if(!may_constrain_evars)return 0;{
void*_tmp25E=Cyc_Tcutil_compress(_tmp266);void*_tmp25F=_tmp25E;struct Cyc_Core_Opt*_tmp265;void**_tmp264;struct Cyc_Core_Opt*_tmp263;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25F)->tag == 1U){_LL11: _tmp265=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25F)->f1;_tmp264=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25F)->f2;_tmp263=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25F)->f4;_LL12: {
# 1360
void*_tmp260=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp263);
# 1362
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp263))->v,v))return 0;{
void*_tmp261=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp262[2U];_tmp262[0]=_tmp260,({void*_tmpABE=Cyc_Absyn_var_type(v);_tmp262[1]=_tmpABE;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp262,sizeof(void*),2U));})));
*_tmp264=_tmp261;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp267=_tmp25D;_LLF:
# 1368
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp267);}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp272=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25B)->f1;_tmp271=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25B)->f2;_tmp270=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25B)->f4;_LL8:
# 1371
 if(_tmp272 == 0  || (int)((struct Cyc_Absyn_Kind*)_tmp272->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp268=0U;({struct _dyneither_ptr _tmpABF=({const char*_tmp269="effect evar has wrong kind";_tag_dyneither(_tmp269,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpABF,_tag_dyneither(_tmp268,sizeof(void*),0U));});});{
# 1375
void*_tmp26A=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp270);
# 1377
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp270))->v,v))
return 0;{
void*_tmp26B=Cyc_Absyn_join_eff(({void*_tmp26C[2U];_tmp26C[0]=_tmp26A,({void*_tmpAC0=Cyc_Absyn_var_type(v);_tmp26C[1]=_tmpAC0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26C,sizeof(void*),2U));}));
*_tmp271=_tmp26B;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1387
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp273=e;void*_tmp279;struct Cyc_List_List*_tmp278;switch(*((int*)_tmp273)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp273)->f1)){case 9U: _LL1: _tmp278=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp273)->f2;_LL2:
# 1391
 for(0;_tmp278 != 0;_tmp278=_tmp278->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp278->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp273)->f2 != 0){_LL3: _tmp279=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp273)->f2)->hd;_LL4: {
# 1396
void*_tmp274=Cyc_Tcutil_rgns_of(_tmp279);void*_tmp275=_tmp274;void*_tmp277;void*_tmp276;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f2 != 0){_LLA: _tmp276=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f2)->hd;_LLB:
 return 0;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp277=_tmp275;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp277);}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1400
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1414 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1419
void*_tmp27A=Cyc_Tcutil_compress(e1);void*_tmp27B=_tmp27A;void**_tmp288;struct Cyc_Core_Opt*_tmp287;struct Cyc_Absyn_Tvar*_tmp286;void*_tmp285;void*_tmp284;struct Cyc_List_List*_tmp283;switch(*((int*)_tmp27B)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f1)){case 9U: _LL1: _tmp283=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f2;_LL2:
# 1421
 for(0;_tmp283 != 0;_tmp283=_tmp283->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp283->hd,e2))
return 0;}
return 1;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f2 != 0){_LL3: _tmp284=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f2)->hd;_LL4:
# 1432
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp284,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp284,Cyc_Absyn_heap_rgn_type);}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f2 != 0){_LL7: _tmp285=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f2)->hd;_LL8: {
# 1436
void*_tmp27C=Cyc_Tcutil_rgns_of(_tmp285);void*_tmp27D=_tmp27C;void*_tmp27F;void*_tmp27E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f2 != 0){_LLE: _tmp27E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f2)->hd;_LLF:
# 1441
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp27E,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp27E,Cyc_Absyn_sint_type);}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp27F=_tmp27D;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp27F,e2);}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp286=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp27B)->f1;_LL6:
# 1434
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp286,e2);case 1U: _LL9: _tmp288=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27B)->f2;_tmp287=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27B)->f4;_LLA:
# 1446
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1450
*_tmp288=Cyc_Absyn_empty_effect;
# 1453
return 1;}else{
# 1455
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp98E;_tmp98E.tag=0U,({struct _dyneither_ptr _tmpAC1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmp98E.f1=_tmpAC1;});_tmp98E;});void*_tmp280[1U];_tmp280[0]=& _tmp282;({struct _dyneither_ptr _tmpAC2=({const char*_tmp281="subset_effect: bad effect: %s";_tag_dyneither(_tmp281,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAC2,_tag_dyneither(_tmp280,sizeof(void*),1U));});});}_LL0:;}
# 1471 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1487
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1489
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp289=(struct _tuple0*)r1->hd;struct _tuple0*_tmp28A=_tmp289;void*_tmp290;void*_tmp28F;_LL1: _tmp290=_tmp28A->f1;_tmp28F=_tmp28A->f2;_LL2:;{
int found=_tmp290 == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp28B=(struct _tuple0*)r2->hd;struct _tuple0*_tmp28C=_tmp28B;void*_tmp28E;void*_tmp28D;_LL4: _tmp28E=_tmp28C->f1;_tmp28D=_tmp28C->f2;_LL5:;
if(Cyc_Tcutil_unify(_tmp290,_tmp28E) && Cyc_Tcutil_unify(_tmp28F,_tmp28D)){
found=1;
break;}}}
# 1499
if(!found)return 0;};}}
# 1501
return 1;}
# 1508
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp291=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp292=({struct Cyc_List_List*_tmp293=_cycalloc(sizeof(*_tmp293));_tmp293->hd=_tmp291,_tmp293->tl=r1;_tmp293;});
if(Cyc_Relations_consistent_relations(_tmp292))return 0;}
# 1514
return 1;}
# 1519
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1525
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1527
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1530
int Cyc_Tcutil_tycon2int(void*t){
void*_tmp294=t;switch(*((int*)_tmp294)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp294)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp294)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1536
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1539
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1542
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1545
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp294)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1534
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1537
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1540
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1543
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1546
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp294)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1535
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1538
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1541
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1544
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1547
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1569
({void*_tmp295=0U;({struct _dyneither_ptr _tmpAC3=({const char*_tmp296="bad con";_tag_dyneither(_tmp296,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAC3,_tag_dyneither(_tmp295,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp294)->f1){case 0U: _LL21: _LL22:
# 1548
 return 16;case 1U: _LL23: _LL24:
 return 17;default: _LL25: _LL26:
 return 18;}case 3U: _LL27: _LL28:
 return 19;case 4U: _LL29: _LL2A:
 return 20;case 5U: _LL2B: _LL2C:
 return 21;case 6U: _LL2D: _LL2E:
 return 22;case 7U: _LL2F: _LL30:
 return 23;case 8U: _LL31: _LL32:
 return 24;case 9U: _LL33: _LL34:
 return 25;case 10U: _LL35: _LL36:
 return 26;case 11U: _LL37: _LL38:
 return 27;case 12U: _LL39: _LL3A:
 return 28;case 14U: _LL3B: _LL3C:
 return 29;case 13U: _LL3D: _LL3E:
 return 30;case 15U: _LL3F: _LL40:
 return 31;case 16U: _LL41: _LL42:
 return 32;case 17U: _LL43: _LL44:
 return 33;case 18U: _LL45: _LL46:
 return 34;case 19U: _LL47: _LL48:
 return 35;default: _LL49: _LL4A:
 return 36;}_LL0:;}
# 1573
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);
# 1575
struct _tuple2*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp297=i;struct _tuple2*_tmp299;struct Cyc_Absyn_Datatypedecl*_tmp298;if((_tmp297.KnownDatatype).tag == 2){_LL1: _tmp298=*(_tmp297.KnownDatatype).val;_LL2:
 return _tmp298->name;}else{_LL3: _tmp299=((_tmp297.UnknownDatatype).val).name;_LL4:
 return _tmp299;}_LL0:;}struct _tuple21{struct _tuple2*f1;struct _tuple2*f2;};
# 1582
struct _tuple21 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp29A=i;struct _tuple2*_tmp29E;struct _tuple2*_tmp29D;struct Cyc_Absyn_Datatypedecl*_tmp29C;struct Cyc_Absyn_Datatypefield*_tmp29B;if((_tmp29A.KnownDatatypefield).tag == 2){_LL1: _tmp29C=((_tmp29A.KnownDatatypefield).val).f1;_tmp29B=((_tmp29A.KnownDatatypefield).val).f2;_LL2:
# 1585
 return({struct _tuple21 _tmp98F;_tmp98F.f1=_tmp29C->name,_tmp98F.f2=_tmp29B->name;_tmp98F;});}else{_LL3: _tmp29E=((_tmp29A.UnknownDatatypefield).val).datatype_name;_tmp29D=((_tmp29A.UnknownDatatypefield).val).field_name;_LL4:
# 1587
 return({struct _tuple21 _tmp990;_tmp990.f1=_tmp29E,_tmp990.f2=_tmp29D;_tmp990;});}_LL0:;}struct _tuple22{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1591
struct _tuple22 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp29F=i;struct Cyc_Absyn_Aggrdecl*_tmp2A2;enum Cyc_Absyn_AggrKind _tmp2A1;struct _tuple2*_tmp2A0;if((_tmp29F.UnknownAggr).tag == 1){_LL1: _tmp2A1=((_tmp29F.UnknownAggr).val).f1;_tmp2A0=((_tmp29F.UnknownAggr).val).f2;_LL2:
 return({struct _tuple22 _tmp991;_tmp991.f1=_tmp2A1,_tmp991.f2=_tmp2A0;_tmp991;});}else{_LL3: _tmp2A2=*(_tmp29F.KnownAggr).val;_LL4:
 return({struct _tuple22 _tmp992;_tmp992.f1=_tmp2A2->kind,_tmp992.f2=_tmp2A2->name;_tmp992;});}_LL0:;}
# 1598
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1604
struct _tuple0 _tmp2A3=({struct _tuple0 _tmp993;_tmp993.f1=t1,_tmp993.f2=t2;_tmp993;});struct _tuple0 _tmp2A4=_tmp2A3;union Cyc_Absyn_AggrInfo _tmp2C2;union Cyc_Absyn_AggrInfo _tmp2C1;union Cyc_Absyn_DatatypeFieldInfo _tmp2C0;union Cyc_Absyn_DatatypeFieldInfo _tmp2BF;union Cyc_Absyn_DatatypeInfo _tmp2BE;union Cyc_Absyn_DatatypeInfo _tmp2BD;struct Cyc_List_List*_tmp2BC;struct Cyc_List_List*_tmp2BB;struct _dyneither_ptr _tmp2BA;struct _dyneither_ptr _tmp2B9;struct _tuple2*_tmp2B8;struct _tuple2*_tmp2B7;switch(*((int*)_tmp2A4.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 15U){_LL1: _tmp2B8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2B7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LL2:
 return Cyc_Absyn_qvar_cmp(_tmp2B8,_tmp2B7);}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 17U){_LL3: _tmp2BA=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2B9=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LL4:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2BA,(struct _dyneither_ptr)_tmp2B9);}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 16U){_LL5: _tmp2BC=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2BB=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LL6:
# 1608
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp2BC,_tmp2BB);}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 18U){_LL7: _tmp2BE=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2BD=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LL8: {
# 1610
struct _tuple2*q1=Cyc_Tcutil_get_datatype_qvar(_tmp2BE);
struct _tuple2*q2=Cyc_Tcutil_get_datatype_qvar(_tmp2BD);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 19U){_LL9: _tmp2C0=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2BF=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LLA: {
# 1614
struct _tuple21 _tmp2A5=Cyc_Tcutil_get_datatype_field_qvars(_tmp2C0);struct _tuple21 _tmp2A6=_tmp2A5;struct _tuple2*_tmp2AD;struct _tuple2*_tmp2AC;_LL10: _tmp2AD=_tmp2A6.f1;_tmp2AC=_tmp2A6.f2;_LL11:;{
struct _tuple21 _tmp2A7=Cyc_Tcutil_get_datatype_field_qvars(_tmp2BF);struct _tuple21 _tmp2A8=_tmp2A7;struct _tuple2*_tmp2AB;struct _tuple2*_tmp2AA;_LL13: _tmp2AB=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;_LL14:;{
int _tmp2A9=Cyc_Absyn_qvar_cmp(_tmp2AD,_tmp2AB);
if(_tmp2A9 != 0)return _tmp2A9;
return Cyc_Absyn_qvar_cmp(_tmp2AC,_tmp2AA);};};}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 20U){_LLB: _tmp2C2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2C1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LLC: {
# 1620
struct _tuple22 _tmp2AE=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2C2);struct _tuple22 _tmp2AF=_tmp2AE;enum Cyc_Absyn_AggrKind _tmp2B6;struct _tuple2*_tmp2B5;_LL16: _tmp2B6=_tmp2AF.f1;_tmp2B5=_tmp2AF.f2;_LL17:;{
struct _tuple22 _tmp2B0=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2C1);struct _tuple22 _tmp2B1=_tmp2B0;enum Cyc_Absyn_AggrKind _tmp2B4;struct _tuple2*_tmp2B3;_LL19: _tmp2B4=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;_LL1A:;{
int _tmp2B2=Cyc_Absyn_qvar_cmp(_tmp2B5,_tmp2B3);
if(_tmp2B2 != 0)return _tmp2B2;
return(int)_tmp2B6 - (int)_tmp2B4;};};}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;};};}struct _tuple23{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1630
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp2C3=t1;struct Cyc_Core_Opt*_tmp2D1;void**_tmp2D0;struct Cyc_Core_Opt*_tmp2CF;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->tag == 1U){_LL1: _tmp2D1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f1;_tmp2D0=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f2;_tmp2CF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f4;_LL2:
# 1641
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CF))->v,t2);{
struct Cyc_Absyn_Kind*_tmp2C4=Cyc_Tcutil_type_kind(t2);
# 1646
if(Cyc_Tcutil_kind_leq(_tmp2C4,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp2D1))->v)){
*_tmp2D0=t2;
return;}else{
# 1650
{void*_tmp2C5=t2;struct Cyc_Absyn_PtrInfo _tmp2CD;void**_tmp2CC;struct Cyc_Core_Opt*_tmp2CB;switch(*((int*)_tmp2C5)){case 1U: _LL6: _tmp2CC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f2;_tmp2CB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f4;_LL7: {
# 1653
struct Cyc_List_List*_tmp2C6=(struct Cyc_List_List*)_tmp2CF->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CB))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp2C6,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp2C7="(type variable would escape scope)";_tag_dyneither(_tmp2C7,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1660
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp2D1->v,_tmp2C4)){
*_tmp2CC=t1;return;}
# 1663
Cyc_Tcutil_failure_reason=({const char*_tmp2C8="(kinds are incompatible)";_tag_dyneither(_tmp2C8,sizeof(char),25U);});
goto _LL5;}case 3U: _LL8: _tmp2CD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5)->f1;if((int)((struct Cyc_Absyn_Kind*)_tmp2D1->v)->kind == (int)Cyc_Absyn_BoxKind){_LL9: {
# 1669
void*_tmp2C9=Cyc_Tcutil_compress((_tmp2CD.ptr_atts).bounds);
{void*_tmp2CA=_tmp2C9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2CA)->tag == 1U){_LLD: _LLE:
# 1672
({void*_tmpAC4=_tmp2C9;Cyc_Tcutil_unify(_tmpAC4,Cyc_Absyn_bounds_one());});
*_tmp2D0=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 1677
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1680
Cyc_Tcutil_failure_reason=({const char*_tmp2CE="(kinds are incompatible)";_tag_dyneither(_tmp2CE,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1683
 goto _LL0;}_LL0:;}
# 1688
{struct _tuple0 _tmp2D2=({struct _tuple0 _tmp999;_tmp999.f1=t2,_tmp999.f2=t1;_tmp999;});struct _tuple0 _tmp2D3=_tmp2D2;struct Cyc_List_List*_tmp361;struct Cyc_Absyn_Typedefdecl*_tmp360;struct Cyc_List_List*_tmp35F;struct Cyc_Absyn_Typedefdecl*_tmp35E;enum Cyc_Absyn_AggrKind _tmp35D;struct Cyc_List_List*_tmp35C;enum Cyc_Absyn_AggrKind _tmp35B;struct Cyc_List_List*_tmp35A;struct Cyc_List_List*_tmp359;struct Cyc_List_List*_tmp358;struct Cyc_List_List*_tmp357;void*_tmp356;struct Cyc_Absyn_Tqual _tmp355;void*_tmp354;struct Cyc_List_List*_tmp353;int _tmp352;struct Cyc_Absyn_VarargInfo*_tmp351;struct Cyc_List_List*_tmp350;struct Cyc_List_List*_tmp34F;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_List_List*_tmp34B;struct Cyc_List_List*_tmp34A;void*_tmp349;struct Cyc_Absyn_Tqual _tmp348;void*_tmp347;struct Cyc_List_List*_tmp346;int _tmp345;struct Cyc_Absyn_VarargInfo*_tmp344;struct Cyc_List_List*_tmp343;struct Cyc_List_List*_tmp342;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_List_List*_tmp340;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_List_List*_tmp33E;void*_tmp33D;struct Cyc_Absyn_Tqual _tmp33C;struct Cyc_Absyn_Exp*_tmp33B;void*_tmp33A;void*_tmp339;struct Cyc_Absyn_Tqual _tmp338;struct Cyc_Absyn_Exp*_tmp337;void*_tmp336;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_Absyn_Exp*_tmp334;void*_tmp333;struct Cyc_Absyn_Tqual _tmp332;void*_tmp331;void*_tmp330;void*_tmp32F;void*_tmp32E;void*_tmp32D;struct Cyc_Absyn_Tqual _tmp32C;void*_tmp32B;void*_tmp32A;void*_tmp329;void*_tmp328;struct Cyc_Absyn_Tvar*_tmp327;struct Cyc_Absyn_Tvar*_tmp326;void*_tmp325;struct Cyc_List_List*_tmp324;void*_tmp323;struct Cyc_List_List*_tmp322;switch(*((int*)_tmp2D3.f1)){case 1U: _LL12: _LL13:
# 1691
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1)->tag == 9U){_LL14: _LL15:
# 1694
 goto _LL17;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1)->tag == 9U)goto _LL16;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1)->tag == 8U)goto _LL18;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1)->tag == 8U)goto _LL1A;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1)->tag == 10U)goto _LL1C;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1)->tag == 10U)goto _LL1E;else{_LL20: _tmp325=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp324=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f2;_tmp323=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_tmp322=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f2;_LL21:
# 1705
 if(Cyc_Tcutil_tycon_cmp(_tmp325,_tmp323)== 0){
Cyc_Tcutil_unify_list(_tmp324,_tmp322);
return;}else{
# 1709
Cyc_Tcutil_failure_reason=({const char*_tmp2D5="(different type constructors)";_tag_dyneither(_tmp2D5,sizeof(char),30U);});}
goto _LL11;}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1)){case 8U: _LL18: _LL19:
# 1696
 goto _LL1B;case 10U: _LL1C: _LL1D:
# 1698
 goto _LL1F;default: goto _LL32;}}}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1)){case 9U: _LL16: _LL17:
# 1695
 goto _LL19;case 8U: _LL1A: _LL1B:
# 1697
 goto _LL1D;case 10U: _LL1E: _LL1F:
# 1700
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp2D4="(effects don't unify)";_tag_dyneither(_tmp2D4,sizeof(char),22U);});
goto _LL11;default: switch(*((int*)_tmp2D3.f1)){case 2U: goto _LL32;case 3U: goto _LL32;case 9U: goto _LL32;case 4U: goto _LL32;case 5U: goto _LL32;case 6U: goto _LL32;case 7U: goto _LL32;case 8U: goto _LL32;default: goto _LL32;}}else{switch(*((int*)_tmp2D3.f1)){case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 2U){_LL22: _tmp327=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp326=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_LL23: {
# 1713
struct _dyneither_ptr*_tmp2D6=_tmp327->name;
struct _dyneither_ptr*_tmp2D7=_tmp326->name;
# 1716
int _tmp2D8=_tmp327->identity;
int _tmp2D9=_tmp326->identity;
if(_tmp2D9 == _tmp2D8)return;
Cyc_Tcutil_failure_reason=({const char*_tmp2DA="(variable types are not the same)";_tag_dyneither(_tmp2DA,sizeof(char),34U);});
goto _LL11;}}else{goto _LL32;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 3U){_LL24: _tmp333=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).elt_type;_tmp332=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).elt_tq;_tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ptr_atts).rgn;_tmp330=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ptr_atts).nullable;_tmp32F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ptr_atts).bounds;_tmp32E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ptr_atts).zero_term;_tmp32D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).elt_type;_tmp32C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).elt_tq;_tmp32B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ptr_atts).rgn;_tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ptr_atts).nullable;_tmp329=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ptr_atts).bounds;_tmp328=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ptr_atts).zero_term;_LL25:
# 1724
 Cyc_Tcutil_unify_it(_tmp32D,_tmp333);
Cyc_Tcutil_unify_it(_tmp331,_tmp32B);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
struct _dyneither_ptr _tmp2DB=Cyc_Tcutil_failure_reason;
Cyc_Tcutil_failure_reason=({const char*_tmp2DC="(not both zero terminated)";_tag_dyneither(_tmp2DC,sizeof(char),27U);});
Cyc_Tcutil_unify_it(_tmp328,_tmp32E);
Cyc_Tcutil_unify_tqual(_tmp32C,_tmp32D,_tmp332,_tmp333);
Cyc_Tcutil_failure_reason=({const char*_tmp2DD="(different pointer bounds)";_tag_dyneither(_tmp2DD,sizeof(char),27U);});
Cyc_Tcutil_unify_it(_tmp329,_tmp32F);{
# 1735
void*_tmp2DE=Cyc_Tcutil_compress(_tmp329);void*_tmp2DF=_tmp2DE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DF)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DF)->f1)->tag == 14U){_LL35: _LL36:
# 1737
 Cyc_Tcutil_failure_reason=_tmp2DB;
return;}else{goto _LL37;}}else{_LL37: _LL38:
# 1740
 Cyc_Tcutil_failure_reason=({const char*_tmp2E0="(incompatible pointer types)";_tag_dyneither(_tmp2E0,sizeof(char),29U);});
Cyc_Tcutil_unify_it(_tmp32A,_tmp330);
return;}_LL34:;};};}else{goto _LL32;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 9U){_LL26: _tmp335=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp334=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_LL27:
# 1746
 if(!Cyc_Evexp_same_const_exp(_tmp335,_tmp334)){
Cyc_Tcutil_failure_reason=({const char*_tmp2E1="(cannot prove expressions are the same)";_tag_dyneither(_tmp2E1,sizeof(char),40U);});
goto _LL11;}
# 1750
return;}else{goto _LL32;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 4U){_LL28: _tmp33D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f1)->f1).elt_type;_tmp33C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f1)->f1).tq;_tmp33B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f1)->f1).num_elts;_tmp33A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f1)->f1).zero_term;_tmp339=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->f1).elt_type;_tmp338=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->f1).tq;_tmp337=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->f1).num_elts;_tmp336=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->f1).zero_term;_LL29:
# 1754
 Cyc_Tcutil_unify_it(_tmp339,_tmp33D);
Cyc_Tcutil_unify_tqual(_tmp338,_tmp339,_tmp33C,_tmp33D);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp2E2="(not both zero terminated)";_tag_dyneither(_tmp2E2,sizeof(char),27U);});
Cyc_Tcutil_unify_it(_tmp33A,_tmp336);
if(_tmp33B == _tmp337)return;
if(_tmp33B == 0  || _tmp337 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp33B,_tmp337))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp2E3="(different array sizes)";_tag_dyneither(_tmp2E3,sizeof(char),24U);});
goto _LL11;}else{goto _LL32;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 5U){_LL2A: _tmp357=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).tvars;_tmp356=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).effect;_tmp355=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ret_tqual;_tmp354=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ret_type;_tmp353=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).args;_tmp352=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).c_varargs;_tmp351=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).cyc_varargs;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).rgn_po;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).attributes;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).requires_clause;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).requires_relns;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ensures_clause;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ensures_relns;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).tvars;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).effect;_tmp348=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ret_tqual;_tmp347=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ret_type;_tmp346=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).args;_tmp345=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).c_varargs;_tmp344=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).cyc_varargs;_tmp343=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).rgn_po;_tmp342=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).attributes;_tmp341=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).requires_clause;_tmp340=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).requires_relns;_tmp33F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ensures_clause;_tmp33E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ensures_relns;_LL2B: {
# 1769
int done=0;
{struct _RegionHandle _tmp2E4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2E4;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp34A != 0){
if(_tmp357 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2E5="(second function type has too few type variables)";_tag_dyneither(_tmp2E5,sizeof(char),50U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1777
void*_tmp2E6=((struct Cyc_Absyn_Tvar*)_tmp34A->hd)->kind;
void*_tmp2E7=((struct Cyc_Absyn_Tvar*)_tmp357->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp2E6,_tmp2E7)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EA=({struct Cyc_String_pa_PrintArg_struct _tmp996;_tmp996.tag=0U,({
struct _dyneither_ptr _tmpAC5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp34A->hd));_tmp996.f1=_tmpAC5;});_tmp996;});struct Cyc_String_pa_PrintArg_struct _tmp2EB=({struct Cyc_String_pa_PrintArg_struct _tmp995;_tmp995.tag=0U,({
struct _dyneither_ptr _tmpAC6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp34A->hd,& Cyc_Tcutil_bk)));_tmp995.f1=_tmpAC6;});_tmp995;});struct Cyc_String_pa_PrintArg_struct _tmp2EC=({struct Cyc_String_pa_PrintArg_struct _tmp994;_tmp994.tag=0U,({
struct _dyneither_ptr _tmpAC7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp357->hd,& Cyc_Tcutil_bk)));_tmp994.f1=_tmpAC7;});_tmp994;});void*_tmp2E8[3U];_tmp2E8[0]=& _tmp2EA,_tmp2E8[1]=& _tmp2EB,_tmp2E8[2]=& _tmp2EC;({struct _dyneither_ptr _tmpAC8=({const char*_tmp2E9="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp2E9,sizeof(char),44U);});Cyc_aprintf(_tmpAC8,_tag_dyneither(_tmp2E8,sizeof(void*),3U));});});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1786
inst=({struct Cyc_List_List*_tmp2EE=_region_malloc(rgn,sizeof(*_tmp2EE));({struct _tuple15*_tmpACA=({struct _tuple15*_tmp2ED=_region_malloc(rgn,sizeof(*_tmp2ED));_tmp2ED->f1=(struct Cyc_Absyn_Tvar*)_tmp357->hd,({void*_tmpAC9=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp34A->hd);_tmp2ED->f2=_tmpAC9;});_tmp2ED;});_tmp2EE->hd=_tmpACA;}),_tmp2EE->tl=inst;_tmp2EE;});
_tmp34A=_tmp34A->tl;
_tmp357=_tmp357->tl;};}
# 1790
if(_tmp357 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2EF="(second function type has too many type variables)";_tag_dyneither(_tmp2EF,sizeof(char),51U);});
_npop_handler(0U);goto _LL11;}
# 1794
if(inst != 0){
({void*_tmpACD=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0->tag=5U,(_tmp2F0->f1).tvars=0,(_tmp2F0->f1).effect=_tmp349,(_tmp2F0->f1).ret_tqual=_tmp348,(_tmp2F0->f1).ret_type=_tmp347,(_tmp2F0->f1).args=_tmp346,(_tmp2F0->f1).c_varargs=_tmp345,(_tmp2F0->f1).cyc_varargs=_tmp344,(_tmp2F0->f1).rgn_po=_tmp343,(_tmp2F0->f1).attributes=_tmp342,(_tmp2F0->f1).requires_clause=_tmp34E,(_tmp2F0->f1).requires_relns=_tmp34D,(_tmp2F0->f1).ensures_clause=_tmp34C,(_tmp2F0->f1).ensures_relns=_tmp34B;_tmp2F0;});Cyc_Tcutil_unify_it(_tmpACD,({
# 1798
struct _RegionHandle*_tmpACC=rgn;struct Cyc_List_List*_tmpACB=inst;Cyc_Tcutil_rsubstitute(_tmpACC,_tmpACB,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));
_tmp2F1->tag=5U,(_tmp2F1->f1).tvars=0,(_tmp2F1->f1).effect=_tmp356,(_tmp2F1->f1).ret_tqual=_tmp355,(_tmp2F1->f1).ret_type=_tmp354,(_tmp2F1->f1).args=_tmp353,(_tmp2F1->f1).c_varargs=_tmp352,(_tmp2F1->f1).cyc_varargs=_tmp351,(_tmp2F1->f1).rgn_po=_tmp350,(_tmp2F1->f1).attributes=_tmp34F,(_tmp2F1->f1).requires_clause=_tmp341,(_tmp2F1->f1).requires_relns=_tmp340,(_tmp2F1->f1).ensures_clause=_tmp33F,(_tmp2F1->f1).ensures_relns=_tmp33E;_tmp2F1;}));}));});
# 1803
done=1;}}
# 1771
;_pop_region(rgn);}
# 1806
if(done)
return;
Cyc_Tcutil_unify_it(_tmp347,_tmp354);
Cyc_Tcutil_unify_tqual(_tmp348,_tmp347,_tmp355,_tmp354);
for(0;_tmp346 != 0  && _tmp353 != 0;(_tmp346=_tmp346->tl,_tmp353=_tmp353->tl)){
struct _tuple10 _tmp2F2=*((struct _tuple10*)_tmp346->hd);struct _tuple10 _tmp2F3=_tmp2F2;struct Cyc_Absyn_Tqual _tmp2F9;void*_tmp2F8;_LL3A: _tmp2F9=_tmp2F3.f2;_tmp2F8=_tmp2F3.f3;_LL3B:;{
struct _tuple10 _tmp2F4=*((struct _tuple10*)_tmp353->hd);struct _tuple10 _tmp2F5=_tmp2F4;struct Cyc_Absyn_Tqual _tmp2F7;void*_tmp2F6;_LL3D: _tmp2F7=_tmp2F5.f2;_tmp2F6=_tmp2F5.f3;_LL3E:;
Cyc_Tcutil_unify_it(_tmp2F8,_tmp2F6);
Cyc_Tcutil_unify_tqual(_tmp2F9,_tmp2F8,_tmp2F7,_tmp2F6);};}
# 1816
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp346 != 0  || _tmp353 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2FA="(function types have different number of arguments)";_tag_dyneither(_tmp2FA,sizeof(char),52U);});
goto _LL11;}
# 1822
if(_tmp345 != _tmp352){
Cyc_Tcutil_failure_reason=({const char*_tmp2FB="(only one function type takes C varargs)";_tag_dyneither(_tmp2FB,sizeof(char),41U);});
goto _LL11;}{
# 1827
int bad_cyc_vararg=0;
{struct _tuple23 _tmp2FC=({struct _tuple23 _tmp997;_tmp997.f1=_tmp344,_tmp997.f2=_tmp351;_tmp997;});struct _tuple23 _tmp2FD=_tmp2FC;struct _dyneither_ptr*_tmp307;struct Cyc_Absyn_Tqual _tmp306;void*_tmp305;int _tmp304;struct _dyneither_ptr*_tmp303;struct Cyc_Absyn_Tqual _tmp302;void*_tmp301;int _tmp300;if(_tmp2FD.f1 == 0){if(_tmp2FD.f2 == 0){_LL40: _LL41:
 goto _LL3F;}else{_LL42: _LL43:
 goto _LL45;}}else{if(_tmp2FD.f2 == 0){_LL44: _LL45:
# 1832
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp2FE="(only one function type takes varargs)";_tag_dyneither(_tmp2FE,sizeof(char),39U);});
goto _LL3F;}else{_LL46: _tmp307=(_tmp2FD.f1)->name;_tmp306=(_tmp2FD.f1)->tq;_tmp305=(_tmp2FD.f1)->type;_tmp304=(_tmp2FD.f1)->inject;_tmp303=(_tmp2FD.f2)->name;_tmp302=(_tmp2FD.f2)->tq;_tmp301=(_tmp2FD.f2)->type;_tmp300=(_tmp2FD.f2)->inject;_LL47:
# 1836
 Cyc_Tcutil_unify_it(_tmp305,_tmp301);
Cyc_Tcutil_unify_tqual(_tmp306,_tmp305,_tmp302,_tmp301);
if(_tmp304 != _tmp300){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp2FF="(only one function type injects varargs)";_tag_dyneither(_tmp2FF,sizeof(char),41U);});}
# 1842
goto _LL3F;}}_LL3F:;}
# 1844
if(bad_cyc_vararg)goto _LL11;{
# 1847
int bad_effect=0;
{struct _tuple0 _tmp308=({struct _tuple0 _tmp998;_tmp998.f1=_tmp349,_tmp998.f2=_tmp356;_tmp998;});struct _tuple0 _tmp309=_tmp308;if(_tmp309.f1 == 0){if(_tmp309.f2 == 0){_LL49: _LL4A:
 goto _LL48;}else{_LL4B: _LL4C:
 goto _LL4E;}}else{if(_tmp309.f2 == 0){_LL4D: _LL4E:
 bad_effect=1;goto _LL48;}else{_LL4F: _LL50:
 bad_effect=!({void*_tmpACE=(void*)_check_null(_tmp349);Cyc_Tcutil_unify_effect(_tmpACE,(void*)_check_null(_tmp356));});goto _LL48;}}_LL48:;}
# 1854
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp30A="(function effects do not match)";_tag_dyneither(_tmp30A,sizeof(char),32U);});
goto _LL11;}
# 1860
if(!Cyc_Tcutil_same_atts(_tmp34F,_tmp342)){
Cyc_Tcutil_failure_reason=({const char*_tmp30B="(function types have different attributes)";_tag_dyneither(_tmp30B,sizeof(char),43U);});
goto _LL11;}
# 1864
if(!Cyc_Tcutil_same_rgn_po(_tmp350,_tmp343)){
Cyc_Tcutil_failure_reason=({const char*_tmp30C="(function types have different region lifetime orderings)";_tag_dyneither(_tmp30C,sizeof(char),58U);});
goto _LL11;}
# 1868
if(!Cyc_Tcutil_check_logical_equivalence(_tmp34D,_tmp340)){
Cyc_Tcutil_failure_reason=({const char*_tmp30D="(@requires clauses not equivalent)";_tag_dyneither(_tmp30D,sizeof(char),35U);});
goto _LL11;}
# 1872
if(!Cyc_Tcutil_check_logical_equivalence(_tmp34B,_tmp33E)){
Cyc_Tcutil_failure_reason=({const char*_tmp30E="(@ensures clauses not equivalent)";_tag_dyneither(_tmp30E,sizeof(char),34U);});
goto _LL11;}
# 1876
return;};};}}else{goto _LL32;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 6U){_LL2C: _tmp359=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp358=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_LL2D:
# 1879
 for(0;_tmp358 != 0  && _tmp359 != 0;(_tmp358=_tmp358->tl,_tmp359=_tmp359->tl)){
struct _tuple12 _tmp30F=*((struct _tuple12*)_tmp358->hd);struct _tuple12 _tmp310=_tmp30F;struct Cyc_Absyn_Tqual _tmp316;void*_tmp315;_LL52: _tmp316=_tmp310.f1;_tmp315=_tmp310.f2;_LL53:;{
struct _tuple12 _tmp311=*((struct _tuple12*)_tmp359->hd);struct _tuple12 _tmp312=_tmp311;struct Cyc_Absyn_Tqual _tmp314;void*_tmp313;_LL55: _tmp314=_tmp312.f1;_tmp313=_tmp312.f2;_LL56:;
Cyc_Tcutil_unify_it(_tmp315,_tmp313);
Cyc_Tcutil_unify_tqual(_tmp316,_tmp315,_tmp314,_tmp313);};}
# 1885
if(_tmp358 == 0  && _tmp359 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp317="(tuple types have different numbers of components)";_tag_dyneither(_tmp317,sizeof(char),51U);});
goto _LL11;}else{goto _LL32;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 7U){_LL2E: _tmp35D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp35C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f1)->f2;_tmp35B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_tmp35A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f2)->f2;_LL2F:
# 1892
 if((int)_tmp35B != (int)_tmp35D){Cyc_Tcutil_failure_reason=({const char*_tmp318="(struct and union type)";_tag_dyneither(_tmp318,sizeof(char),24U);});goto _LL11;}
for(0;_tmp35A != 0  && _tmp35C != 0;(_tmp35A=_tmp35A->tl,_tmp35C=_tmp35C->tl)){
struct Cyc_Absyn_Aggrfield*_tmp319=(struct Cyc_Absyn_Aggrfield*)_tmp35A->hd;
struct Cyc_Absyn_Aggrfield*_tmp31A=(struct Cyc_Absyn_Aggrfield*)_tmp35C->hd;
if(Cyc_strptrcmp(_tmp319->name,_tmp31A->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp31B="(different member names)";_tag_dyneither(_tmp31B,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1900
Cyc_Tcutil_unify_it(_tmp319->type,_tmp31A->type);
Cyc_Tcutil_unify_tqual(_tmp319->tq,_tmp319->type,_tmp31A->tq,_tmp31A->type);
if(!Cyc_Tcutil_same_atts(_tmp319->attributes,_tmp31A->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp31C="(different attributes on member)";_tag_dyneither(_tmp31C,sizeof(char),33U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1908
if((_tmp319->width != 0  && _tmp31A->width == 0  || 
_tmp31A->width != 0  && _tmp319->width == 0) || 
(_tmp319->width != 0  && _tmp31A->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpACF=(struct Cyc_Absyn_Exp*)_check_null(_tmp319->width);Cyc_Evexp_same_const_exp(_tmpACF,(struct Cyc_Absyn_Exp*)_check_null(_tmp31A->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp31D="(different bitfield widths on member)";_tag_dyneither(_tmp31D,sizeof(char),38U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1917
if((_tmp319->requires_clause != 0  && _tmp31A->requires_clause == 0  || 
_tmp319->requires_clause == 0  && _tmp31A->requires_clause != 0) || 
(_tmp319->requires_clause == 0  && _tmp31A->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpAD0=(struct Cyc_Absyn_Exp*)_check_null(_tmp319->requires_clause);Cyc_Evexp_same_const_exp(_tmpAD0,(struct Cyc_Absyn_Exp*)_check_null(_tmp31A->requires_clause));})){
# 1922
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp31E="(different @requires clauses on member)";_tag_dyneither(_tmp31E,sizeof(char),40U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1928
if(_tmp35A == 0  && _tmp35C == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp31F="(different number of members)";_tag_dyneither(_tmp31F,sizeof(char),30U);});
goto _LL11;}else{goto _LL32;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 8U){_LL30: _tmp361=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f1)->f2;_tmp360=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f1)->f3;_tmp35F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f2)->f2;_tmp35E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f2)->f3;_LL31:
# 1934
 if(_tmp360 != _tmp35E){
Cyc_Tcutil_failure_reason=({const char*_tmp320="(different abstract typedefs)";_tag_dyneither(_tmp320,sizeof(char),30U);});
goto _LL11;}
# 1938
Cyc_Tcutil_failure_reason=({const char*_tmp321="(type parameters to typedef differ)";_tag_dyneither(_tmp321,sizeof(char),36U);});
Cyc_Tcutil_unify_list(_tmp361,_tmp35F);
return;}else{goto _LL32;}default: _LL32: _LL33:
 goto _LL11;}}}_LL11:;}
# 1943
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1946
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpAD2)(void*,void*)=cmp;void*_tmpAD1=(void*)_check_null(a1);_tmpAD2(_tmpAD1,(void*)_check_null(a2));});}
# 1953
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp362=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp363=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp362,_tmp363);}
# 1959
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp364=tqt1;struct Cyc_Absyn_Tqual _tmp36A;void*_tmp369;_LL1: _tmp36A=_tmp364->f1;_tmp369=_tmp364->f2;_LL2:;{
struct _tuple12*_tmp365=tqt2;struct Cyc_Absyn_Tqual _tmp368;void*_tmp367;_LL4: _tmp368=_tmp365->f1;_tmp367=_tmp365->f2;_LL5:;{
int _tmp366=Cyc_Tcutil_tqual_cmp(_tmp36A,_tmp368);
if(_tmp366 != 0)return _tmp366;
return Cyc_Tcutil_typecmp(_tmp369,_tmp367);};};}
# 1967
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp36B=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp36B != 0)return _tmp36B;{
int _tmp36C=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp36C != 0)return _tmp36C;{
int _tmp36D=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp36D != 0)return _tmp36D;{
int _tmp36E=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp36E != 0)return _tmp36E;
_tmp36E=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp36E != 0)return _tmp36E;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1981
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp36F=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp36F != 0)return _tmp36F;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1987
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp370=t;void*_tmp371;switch(*((int*)_tmp370)){case 1U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 2;case 3U: _LL5: _LL6:
 return 3;case 4U: _LL7: _LL8:
 return 4;case 5U: _LL9: _LLA:
 return 5;case 6U: _LLB: _LLC:
 return 6;case 7U: _LLD: _LLE:
 return 7;case 8U: _LLF: _LL10:
 return 8;case 9U: _LL11: _LL12:
 return 9;case 10U: _LL13: _LL14:
 return 10;case 11U: _LL15: _LL16:
 return 11;default: _LL17: _tmp371=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp370)->f1;_LL18:
 return 12 + Cyc_Tcutil_tycon2int(_tmp371);}_LL0:;}
# 2006
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp372=({int _tmpAD3=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpAD3,Cyc_Tcutil_type_case_number(t2));});
if(_tmp372 != 0)
return _tmp372;{
# 2015
struct _tuple0 _tmp373=({struct _tuple0 _tmp99A;_tmp99A.f1=t2,_tmp99A.f2=t1;_tmp99A;});struct _tuple0 _tmp374=_tmp373;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BD;enum Cyc_Absyn_AggrKind _tmp3BC;struct Cyc_List_List*_tmp3BB;enum Cyc_Absyn_AggrKind _tmp3BA;struct Cyc_List_List*_tmp3B9;struct Cyc_List_List*_tmp3B8;struct Cyc_List_List*_tmp3B7;struct Cyc_Absyn_FnInfo _tmp3B6;struct Cyc_Absyn_FnInfo _tmp3B5;void*_tmp3B4;struct Cyc_Absyn_Tqual _tmp3B3;struct Cyc_Absyn_Exp*_tmp3B2;void*_tmp3B1;void*_tmp3B0;struct Cyc_Absyn_Tqual _tmp3AF;struct Cyc_Absyn_Exp*_tmp3AE;void*_tmp3AD;void*_tmp3AC;struct Cyc_Absyn_Tqual _tmp3AB;void*_tmp3AA;void*_tmp3A9;void*_tmp3A8;void*_tmp3A7;void*_tmp3A6;struct Cyc_Absyn_Tqual _tmp3A5;void*_tmp3A4;void*_tmp3A3;void*_tmp3A2;void*_tmp3A1;struct Cyc_Absyn_Tvar*_tmp3A0;struct Cyc_Absyn_Tvar*_tmp39F;void*_tmp39E;struct Cyc_List_List*_tmp39D;void*_tmp39C;struct Cyc_List_List*_tmp39B;switch(*((int*)_tmp374.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f2)->tag == 0U){_LL1: _tmp39E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp39D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f1)->f2;_tmp39C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f2)->f1;_tmp39B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f2)->f2;_LL2: {
# 2017
int _tmp375=Cyc_Tcutil_tycon_cmp(_tmp39E,_tmp39C);
if(_tmp375 != 0)return _tmp375;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp39D,_tmp39B);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp374.f2)->tag == 1U){_LL3: _LL4:
# 2021
({void*_tmp376=0U;({struct _dyneither_ptr _tmpAD4=({const char*_tmp377="typecmp: can only compare closed types";_tag_dyneither(_tmp377,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAD4,_tag_dyneither(_tmp376,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp374.f2)->tag == 2U){_LL5: _tmp3A0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp39F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp374.f2)->f1;_LL6:
# 2025
 return Cyc_Core_intcmp(_tmp39F->identity,_tmp3A0->identity);}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->tag == 3U){_LL7: _tmp3AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).elt_type;_tmp3AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).elt_tq;_tmp3AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).ptr_atts).rgn;_tmp3A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).ptr_atts).nullable;_tmp3A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).ptr_atts).bounds;_tmp3A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).ptr_atts).zero_term;_tmp3A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).elt_type;_tmp3A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).elt_tq;_tmp3A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).ptr_atts).rgn;_tmp3A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).ptr_atts).nullable;_tmp3A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).ptr_atts).bounds;_tmp3A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).ptr_atts).zero_term;_LL8: {
# 2029
int _tmp378=Cyc_Tcutil_typecmp(_tmp3A6,_tmp3AC);
if(_tmp378 != 0)return _tmp378;{
int _tmp379=Cyc_Tcutil_typecmp(_tmp3A4,_tmp3AA);
if(_tmp379 != 0)return _tmp379;{
int _tmp37A=Cyc_Tcutil_tqual_cmp(_tmp3A5,_tmp3AB);
if(_tmp37A != 0)return _tmp37A;{
int _tmp37B=Cyc_Tcutil_typecmp(_tmp3A2,_tmp3A8);
if(_tmp37B != 0)return _tmp37B;{
int _tmp37C=Cyc_Tcutil_typecmp(_tmp3A1,_tmp3A7);
if(_tmp37C != 0)return _tmp37C;{
int _tmp37D=Cyc_Tcutil_typecmp(_tmp3A2,_tmp3A8);
if(_tmp37D != 0)return _tmp37D;
return Cyc_Tcutil_typecmp(_tmp3A3,_tmp3A9);};};};};};}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->tag == 4U){_LL9: _tmp3B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f1)->f1).elt_type;_tmp3B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f1)->f1).tq;_tmp3B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f1)->f1).num_elts;_tmp3B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f1)->f1).zero_term;_tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->f1).elt_type;_tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->f1).tq;_tmp3AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->f1).num_elts;_tmp3AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->f1).zero_term;_LLA: {
# 2045
int _tmp37E=Cyc_Tcutil_tqual_cmp(_tmp3AF,_tmp3B3);
if(_tmp37E != 0)return _tmp37E;{
int _tmp37F=Cyc_Tcutil_typecmp(_tmp3B0,_tmp3B4);
if(_tmp37F != 0)return _tmp37F;{
int _tmp380=Cyc_Tcutil_typecmp(_tmp3B1,_tmp3AD);
if(_tmp380 != 0)return _tmp380;
if(_tmp3B2 == _tmp3AE)return 0;
if(_tmp3B2 == 0  || _tmp3AE == 0)
({void*_tmp381=0U;({struct _dyneither_ptr _tmpAD5=({const char*_tmp382="missing expression in array index";_tag_dyneither(_tmp382,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAD5,_tag_dyneither(_tmp381,sizeof(void*),0U));});});
# 2055
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B2,_tmp3AE);};};}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp374.f2)->tag == 5U){_LLB: _tmp3B6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3B5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp374.f2)->f1;_LLC:
# 2058
 if(Cyc_Tcutil_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp3B6.ret_type,_tmp3B5.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(_tmp3B6.ret_tqual,_tmp3B5.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp383=_tmp3B6.args;
struct Cyc_List_List*_tmp384=_tmp3B5.args;
for(0;_tmp383 != 0  && _tmp384 != 0;(_tmp383=_tmp383->tl,_tmp384=_tmp384->tl)){
struct _tuple10 _tmp385=*((struct _tuple10*)_tmp383->hd);struct _tuple10 _tmp386=_tmp385;struct Cyc_Absyn_Tqual _tmp38C;void*_tmp38B;_LL18: _tmp38C=_tmp386.f2;_tmp38B=_tmp386.f3;_LL19:;{
struct _tuple10 _tmp387=*((struct _tuple10*)_tmp384->hd);struct _tuple10 _tmp388=_tmp387;struct Cyc_Absyn_Tqual _tmp38A;void*_tmp389;_LL1B: _tmp38A=_tmp388.f2;_tmp389=_tmp388.f3;_LL1C:;
r=Cyc_Tcutil_tqual_cmp(_tmp38C,_tmp38A);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp38B,_tmp389);
if(r != 0)return r;};}
# 2073
if(_tmp383 != 0)return 1;
if(_tmp384 != 0)return - 1;
if(_tmp3B6.c_varargs  && !_tmp3B5.c_varargs)return 1;
if(!_tmp3B6.c_varargs  && _tmp3B5.c_varargs)return - 1;
if(_tmp3B6.cyc_varargs != 0 & _tmp3B5.cyc_varargs == 0)return 1;
if(_tmp3B6.cyc_varargs == 0 & _tmp3B5.cyc_varargs != 0)return - 1;
if(_tmp3B6.cyc_varargs != 0 & _tmp3B5.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmpAD6=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3B6.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmpAD6,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3B5.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((_tmp3B6.cyc_varargs)->type,(_tmp3B5.cyc_varargs)->type);
if(r != 0)return r;
if((_tmp3B6.cyc_varargs)->inject  && !(_tmp3B5.cyc_varargs)->inject)return 1;
if(!(_tmp3B6.cyc_varargs)->inject  && (_tmp3B5.cyc_varargs)->inject)return - 1;}
# 2087
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp3B6.effect,_tmp3B5.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp38D=_tmp3B6.rgn_po;
struct Cyc_List_List*_tmp38E=_tmp3B5.rgn_po;
for(0;_tmp38D != 0  && _tmp38E != 0;(_tmp38D=_tmp38D->tl,_tmp38E=_tmp38E->tl)){
struct _tuple0 _tmp38F=*((struct _tuple0*)_tmp38D->hd);struct _tuple0 _tmp390=_tmp38F;void*_tmp396;void*_tmp395;_LL1E: _tmp396=_tmp390.f1;_tmp395=_tmp390.f2;_LL1F:;{
struct _tuple0 _tmp391=*((struct _tuple0*)_tmp38E->hd);struct _tuple0 _tmp392=_tmp391;void*_tmp394;void*_tmp393;_LL21: _tmp394=_tmp392.f1;_tmp393=_tmp392.f2;_LL22:;
r=Cyc_Tcutil_typecmp(_tmp396,_tmp394);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp395,_tmp393);if(r != 0)return r;};}
# 2097
if(_tmp38D != 0)return 1;
if(_tmp38E != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B6.requires_clause,_tmp3B5.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B6.ensures_clause,_tmp3B5.ensures_clause);
if(r != 0)return r;
# 2105
({void*_tmp397=0U;({struct _dyneither_ptr _tmpAD7=({const char*_tmp398="typecmp: function type comparison should never get here!";_tag_dyneither(_tmp398,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAD7,_tag_dyneither(_tmp397,sizeof(void*),0U));});});};};};}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp374.f2)->tag == 6U){_LLD: _tmp3B8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3B7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp374.f2)->f1;_LLE:
# 2108
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp3B7,_tmp3B8);}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f2)->tag == 7U){_LLF: _tmp3BC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3BB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f1)->f2;_tmp3BA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f2)->f1;_tmp3B9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f2)->f2;_LL10:
# 2111
 if((int)_tmp3BA != (int)_tmp3BC){
if((int)_tmp3BA == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp3B9,_tmp3BB);}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp374.f2)->tag == 9U){_LL11: _tmp3BE=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3BD=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp374.f2)->f1;_LL12:
# 2116
 _tmp3C0=_tmp3BE;_tmp3BF=_tmp3BD;goto _LL14;}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp374.f2)->tag == 11U){_LL13: _tmp3C0=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3BF=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp374.f2)->f1;_LL14:
# 2118
 return Cyc_Evexp_const_exp_cmp(_tmp3C0,_tmp3BF);}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp399=0U;({struct _dyneither_ptr _tmpAD8=({const char*_tmp39A="Unmatched case in typecmp";_tag_dyneither(_tmp39A,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAD8,_tag_dyneither(_tmp399,sizeof(void*),0U));});});}_LL0:;};};}
# 2125
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp3C1=({struct _tuple0 _tmp99C;_tmp99C.f1=t1,_tmp99C.f2=t2;_tmp99C;});struct _tuple0 _tmp3C2=_tmp3C1;void*_tmp3C8;void*_tmp3C7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2.f2)->tag == 0U){_LL1: _tmp3C8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2.f1)->f1;_tmp3C7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2.f2)->f1;_LL2: {
# 2130
struct _tuple0 _tmp3C3=({struct _tuple0 _tmp99B;_tmp99B.f1=_tmp3C8,_tmp99B.f2=_tmp3C7;_tmp99B;});struct _tuple0 _tmp3C4=_tmp3C3;int _tmp3C6;int _tmp3C5;switch(*((int*)_tmp3C4.f1)){case 2U: switch(*((int*)_tmp3C4.f2)){case 2U: _LL6: _tmp3C6=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C4.f1)->f1;_tmp3C5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f1;_LL7:
# 2132
 return _tmp3C5 < _tmp3C6;case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp3C4.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 2139
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 2144
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 2147
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f1 == 0){_LL14: _LL15:
# 2142
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp3C4.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 2140
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 2145
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 2148
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f1 == 0){_LL16: _LL17:
# 2143
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 2149
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 2146
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 2150
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 2152
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2154
 return 0;}_LL0:;};}
# 2160
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2163
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->v=t1;_tmp3C9;});}}}
# 2172
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2176
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp3CC=({struct Cyc_String_pa_PrintArg_struct _tmp99E;_tmp99E.tag=0U,({
struct _dyneither_ptr _tmpAD9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp99E.f1=_tmpAD9;});_tmp99E;});struct Cyc_String_pa_PrintArg_struct _tmp3CD=({struct Cyc_String_pa_PrintArg_struct _tmp99D;_tmp99D.tag=0U,({struct _dyneither_ptr _tmpADA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp99D.f1=_tmpADA;});_tmp99D;});void*_tmp3CA[2U];_tmp3CA[0]=& _tmp3CC,_tmp3CA[1]=& _tmp3CD;({unsigned int _tmpADC=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpADB=({const char*_tmp3CB="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp3CB,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpADC,_tmpADB,_tag_dyneither(_tmp3CA,sizeof(void*),2U));});});
return 0;}}}
# 2182
return 1;}
# 2187
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(te,e)){
void*_tmp3CE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3CF=_tmp3CE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CF)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2193
return 1;}
# 2197
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 2201
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp3D0=0U;({unsigned int _tmpADE=e->loc;struct _dyneither_ptr _tmpADD=({const char*_tmp3D1="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3D1,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpADE,_tmpADD,_tag_dyneither(_tmp3D0,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2207
return 0;}
# 2211
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 2215
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp3D2=0U;({unsigned int _tmpAE0=e->loc;struct _dyneither_ptr _tmpADF=({const char*_tmp3D3="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3D3,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpAE0,_tmpADF,_tag_dyneither(_tmp3D2,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2221
return 0;}
# 2226
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Tcutil_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 2232
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmpAE1=t;Cyc_Tcutil_unify(_tmpAE1,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 2237
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp3D4=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);
void*_tmp3D5=_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D7;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D5)->tag == 9U){_LL1: _tmp3D7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D5)->f1;_LL2:
 return _tmp3D7;}else{_LL3: _LL4: {
# 2242
struct Cyc_Absyn_Exp*_tmp3D6=Cyc_Absyn_valueof_exp(_tmp3D4,0U);
_tmp3D6->topt=Cyc_Absyn_uint_type;
return _tmp3D6;}}_LL0:;}
# 2251
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Tcutil_unify(def,b);{
void*_tmp3D8=Cyc_Tcutil_compress(b);void*_tmp3D9=_tmp3D8;struct Cyc_List_List*_tmp3DD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp3DD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->f2;_LL4:
 return Cyc_Tcutil_get_thin_bound(_tmp3DD);default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp3DC=({struct Cyc_String_pa_PrintArg_struct _tmp99F;_tmp99F.tag=0U,({struct _dyneither_ptr _tmpAE2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(b));_tmp99F.f1=_tmpAE2;});_tmp99F;});void*_tmp3DA[1U];_tmp3DA[0]=& _tmp3DC;({struct _dyneither_ptr _tmpAE3=({const char*_tmp3DB="get_bounds_exp: %s";_tag_dyneither(_tmp3DB,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAE3,_tag_dyneither(_tmp3DA,sizeof(void*),1U));});});}_LL0:;};}
# 2260
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp3DE=Cyc_Tcutil_compress(t);void*_tmp3DF=_tmp3DE;void*_tmp3E3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DF)->tag == 3U){_LL1: _tmp3E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DF)->f1).ptr_atts).bounds;_LL2:
# 2263
 return Cyc_Tcutil_get_bounds_exp(def,_tmp3E3);}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp3E2=({struct Cyc_String_pa_PrintArg_struct _tmp9A0;_tmp9A0.tag=0U,({struct _dyneither_ptr _tmpAE4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9A0.f1=_tmpAE4;});_tmp9A0;});void*_tmp3E0[1U];_tmp3E0[0]=& _tmp3E2;({struct _dyneither_ptr _tmpAE5=({const char*_tmp3E1="get_ptr_bounds_exp not pointer: %s";_tag_dyneither(_tmp3E1,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAE5,_tag_dyneither(_tmp3E0,sizeof(void*),1U));});});}_LL0:;}
# 2269
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));({struct Cyc_List_List*_tmpAE6=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3E4->v=_tmpAE6;});_tmp3E4;}));else{
# 2273
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->v=0;_tmp3E5;}));}}
# 2276
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));({struct Cyc_List_List*_tmpAE7=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3E6->v=_tmpAE7;});_tmp3E6;}));else{
# 2280
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->v=0;_tmp3E7;}));}}
# 2284
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple24{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 2292
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2294
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp3E8=({struct _tuple0 _tmp9A4;_tmp9A4.f1=t1,_tmp9A4.f2=t2;_tmp9A4;});struct _tuple0 _tmp3E9=_tmp3E8;void*_tmp3FE;struct Cyc_Absyn_Tqual _tmp3FD;struct Cyc_Absyn_Exp*_tmp3FC;void*_tmp3FB;void*_tmp3FA;struct Cyc_Absyn_Tqual _tmp3F9;struct Cyc_Absyn_Exp*_tmp3F8;void*_tmp3F7;struct Cyc_Absyn_PtrInfo _tmp3F6;struct Cyc_Absyn_PtrInfo _tmp3F5;switch(*((int*)_tmp3E9.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9.f2)->tag == 3U){_LL1: _tmp3F6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9.f1)->f1;_tmp3F5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9.f2)->f1;_LL2: {
# 2298
int okay=1;
# 2300
if(!Cyc_Tcutil_unify((_tmp3F6.ptr_atts).nullable,(_tmp3F5.ptr_atts).nullable))
# 2302
okay=!Cyc_Tcutil_force_type2bool(0,(_tmp3F6.ptr_atts).nullable);
# 2304
if(!Cyc_Tcutil_unify((_tmp3F6.ptr_atts).bounds,(_tmp3F5.ptr_atts).bounds)){
struct _tuple24 _tmp3EA=({struct _tuple24 _tmp9A1;({struct Cyc_Absyn_Exp*_tmpAEB=({void*_tmpAEA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpAEA,(_tmp3F6.ptr_atts).bounds);});_tmp9A1.f1=_tmpAEB;}),({
struct Cyc_Absyn_Exp*_tmpAE9=({void*_tmpAE8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpAE8,(_tmp3F5.ptr_atts).bounds);});_tmp9A1.f2=_tmpAE9;});_tmp9A1;});
# 2305
struct _tuple24 _tmp3EB=_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3EE;if(_tmp3EB.f2 == 0){_LLA: _LLB:
# 2308
 okay=1;goto _LL9;}else{if(_tmp3EB.f1 == 0){_LLC: _LLD:
# 2311
 if(Cyc_Tcutil_force_type2bool(0,(_tmp3F6.ptr_atts).zero_term) && ({
void*_tmpAEC=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpAEC,(_tmp3F5.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp3EF=_tmp3EB.f1;_tmp3EE=_tmp3EB.f2;_LLF:
# 2317
 okay=okay  && ({struct Cyc_Absyn_Exp*_tmpAED=(struct Cyc_Absyn_Exp*)_check_null(_tmp3EE);Cyc_Evexp_lte_const_exp(_tmpAED,(struct Cyc_Absyn_Exp*)_check_null(_tmp3EF));});
# 2321
if(!Cyc_Tcutil_force_type2bool(0,(_tmp3F5.ptr_atts).zero_term))
({void*_tmp3EC=0U;({unsigned int _tmpAEF=loc;struct _dyneither_ptr _tmpAEE=({const char*_tmp3ED="implicit cast to shorter array";_tag_dyneither(_tmp3ED,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpAEF,_tmpAEE,_tag_dyneither(_tmp3EC,sizeof(void*),0U));});});
goto _LL9;}}_LL9:;}
# 2328
okay=okay  && (!(_tmp3F6.elt_tq).real_const  || (_tmp3F5.elt_tq).real_const);
# 2331
if(!Cyc_Tcutil_unify((_tmp3F6.ptr_atts).rgn,(_tmp3F5.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp3F6.ptr_atts).rgn,(_tmp3F5.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp3F2=({struct Cyc_String_pa_PrintArg_struct _tmp9A3;_tmp9A3.tag=0U,({
struct _dyneither_ptr _tmpAF0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3F6.ptr_atts).rgn));_tmp9A3.f1=_tmpAF0;});_tmp9A3;});struct Cyc_String_pa_PrintArg_struct _tmp3F3=({struct Cyc_String_pa_PrintArg_struct _tmp9A2;_tmp9A2.tag=0U,({
struct _dyneither_ptr _tmpAF1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3F5.ptr_atts).rgn));_tmp9A2.f1=_tmpAF1;});_tmp9A2;});void*_tmp3F0[2U];_tmp3F0[0]=& _tmp3F2,_tmp3F0[1]=& _tmp3F3;({unsigned int _tmpAF3=loc;struct _dyneither_ptr _tmpAF2=({const char*_tmp3F1="implicit cast from region %s to region %s";_tag_dyneither(_tmp3F1,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpAF3,_tmpAF2,_tag_dyneither(_tmp3F0,sizeof(void*),2U));});});}else{
okay=0;}}
# 2341
okay=okay  && (Cyc_Tcutil_unify((_tmp3F6.ptr_atts).zero_term,(_tmp3F5.ptr_atts).zero_term) || 
# 2343
Cyc_Tcutil_force_type2bool(1,(_tmp3F6.ptr_atts).zero_term) && (_tmp3F5.elt_tq).real_const);{
# 2351
int _tmp3F4=
({void*_tmpAF4=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpAF4,(_tmp3F5.ptr_atts).bounds);}) && !
Cyc_Tcutil_force_type2bool(0,(_tmp3F5.ptr_atts).zero_term);
# 2357
okay=okay  && (Cyc_Tcutil_unify(_tmp3F6.elt_type,_tmp3F5.elt_type) || 
(_tmp3F4  && ((_tmp3F5.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3F5.elt_type)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp3F6.elt_type,_tmp3F5.elt_type));
# 2360
return okay;};}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->tag == 4U){_LL3: _tmp3FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f1)->f1).elt_type;_tmp3FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f1)->f1).tq;_tmp3FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f1)->f1).num_elts;_tmp3FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f1)->f1).zero_term;_tmp3FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->f1).elt_type;_tmp3F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->f1).tq;_tmp3F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->f1).num_elts;_tmp3F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->f1).zero_term;_LL4: {
# 2364
int okay;
# 2367
okay=Cyc_Tcutil_unify(_tmp3FB,_tmp3F7) && (
(_tmp3FC != 0  && _tmp3F8 != 0) && Cyc_Evexp_same_const_exp(_tmp3FC,_tmp3F8));
# 2370
return(okay  && Cyc_Tcutil_unify(_tmp3FE,_tmp3FA)) && (!_tmp3FD.real_const  || _tmp3F9.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E9.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E9.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E9.f2)->f1)->tag == 1U){_LL5: _LL6:
# 2372
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 2374
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2378
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp3FF=Cyc_Tcutil_compress(t);void*_tmp400=_tmp3FF;void*_tmp403;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp400)->tag == 3U){_LL1: _tmp403=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp400)->f1).elt_type;_LL2:
 return _tmp403;}else{_LL3: _LL4:
({void*_tmp401=0U;({struct _dyneither_ptr _tmpAF5=({const char*_tmp402="pointer_elt_type";_tag_dyneither(_tmp402,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF5,_tag_dyneither(_tmp401,sizeof(void*),0U));});});}_LL0:;}
# 2384
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp404=Cyc_Tcutil_compress(t);void*_tmp405=_tmp404;struct Cyc_Absyn_PtrAtts*_tmp408;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405)->tag == 3U){_LL1: _tmp408=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405)->f1).ptr_atts;_LL2:
 return _tmp408->rgn;}else{_LL3: _LL4:
({void*_tmp406=0U;({struct _dyneither_ptr _tmpAF6=({const char*_tmp407="pointer_elt_type";_tag_dyneither(_tmp407,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF6,_tag_dyneither(_tmp406,sizeof(void*),0U));});});}_LL0:;}
# 2391
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp409=Cyc_Tcutil_compress(t);void*_tmp40A=_tmp409;void*_tmp40B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40A)->tag == 3U){_LL1: _tmp40B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40A)->f1).ptr_atts).rgn;_LL2:
# 2394
*rgn=_tmp40B;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2403
static int Cyc_Tcutil_admits_zero(void*t){
void*_tmp40C=Cyc_Tcutil_compress(t);void*_tmp40D=_tmp40C;void*_tmp40F;void*_tmp40E;switch(*((int*)_tmp40D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp40F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40D)->f1).ptr_atts).nullable;_tmp40E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40D)->f1).ptr_atts).bounds;_LL6:
# 2411
 return !Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp40E) && Cyc_Tcutil_force_type2bool(0,_tmp40F);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2417
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp410=e->r;void*_tmp411=_tmp410;void*_tmp415;struct Cyc_Absyn_Exp*_tmp414;struct _dyneither_ptr _tmp413;switch(*((int*)_tmp411)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp411)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp411)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp411)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp411)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp411)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp413=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp411)->f1).Wchar_c).val;_LLC: {
# 2426
unsigned long _tmp412=Cyc_strlen((struct _dyneither_ptr)_tmp413);
int i=0;
if(_tmp412 >= (unsigned long)2  && (int)*((const char*)_check_dyneither_subscript(_tmp413,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_dyneither_subscript(_tmp413,sizeof(char),1))== (int)'0')i=2;else{
if(((int)*((const char*)_check_dyneither_subscript(_tmp413,sizeof(char),1))== (int)'x'  && _tmp412 >= (unsigned long)3) && (int)*((const char*)_check_dyneither_subscript(_tmp413,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp412;++ i){
if((int)*((const char*)_check_dyneither_subscript(_tmp413,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 2436
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2424
 return 1;case 14U: _LLD: _tmp415=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp411)->f1;_tmp414=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp411)->f2;_LLE:
# 2437
 return Cyc_Tcutil_is_zero(_tmp414) && Cyc_Tcutil_admits_zero(_tmp415);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2442
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 2451
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2456
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2461
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 2470
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2475
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2480
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp416=ka;enum Cyc_Absyn_KindQual _tmp41F;enum Cyc_Absyn_AliasQual _tmp41E;_LL1: _tmp41F=_tmp416->kind;_tmp41E=_tmp416->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp417=_tmp41E;switch(_tmp417){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2484
enum Cyc_Absyn_KindQual _tmp418=_tmp41F;switch(_tmp418){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2495
{enum Cyc_Absyn_KindQual _tmp419=_tmp41F;switch(_tmp419){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 2502
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 2504
{enum Cyc_Absyn_KindQual _tmp41A=_tmp41F;switch(_tmp41A){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 2511
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 2514
({struct Cyc_String_pa_PrintArg_struct _tmp41D=({struct Cyc_String_pa_PrintArg_struct _tmp9A5;_tmp9A5.tag=0U,({struct _dyneither_ptr _tmpAF7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp9A5.f1=_tmpAF7;});_tmp9A5;});void*_tmp41B[1U];_tmp41B[0]=& _tmp41D;({struct _dyneither_ptr _tmpAF8=({const char*_tmp41C="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp41C,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF8,_tag_dyneither(_tmp41B,sizeof(void*),1U));});});}
# 2517
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->tag=0U,_tmp420->f1=k;_tmp420;});}
# 2520
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp421=_cycalloc(sizeof(*_tmp421));({void*_tmpAF9=Cyc_Tcutil_kind_to_bound(k);_tmp421->v=_tmpAF9;});_tmp421;});}
# 2526
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpAFA=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->tag=0U,_tmp422->f1=Cyc_Absyn_Null_c;_tmp422;});e1->r=_tmpAFA;});{
struct Cyc_Core_Opt*_tmp423=Cyc_Tcenv_lookup_opt_type_vars(te);
void*_tmp424=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp9A8;({void*_tmpAFF=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp423);_tmp9A8.elt_type=_tmpAFF;}),({
struct Cyc_Absyn_Tqual _tmpAFE=Cyc_Absyn_empty_tqual(0U);_tmp9A8.elt_tq=_tmpAFE;}),
({void*_tmpAFD=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp423);(_tmp9A8.ptr_atts).rgn=_tmpAFD;}),(_tmp9A8.ptr_atts).nullable=Cyc_Absyn_true_type,({
# 2534
void*_tmpAFC=Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,_tmp423);(_tmp9A8.ptr_atts).bounds=_tmpAFC;}),({
void*_tmpAFB=Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,_tmp423);(_tmp9A8.ptr_atts).zero_term=_tmpAFB;}),(_tmp9A8.ptr_atts).ptrloc=0;_tmp9A8;}));
e1->topt=_tmp424;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp427=({struct Cyc_String_pa_PrintArg_struct _tmp9A7;_tmp9A7.tag=0U,({
struct _dyneither_ptr _tmpB00=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9A7.f1=_tmpB00;});_tmp9A7;});struct Cyc_String_pa_PrintArg_struct _tmp428=({struct Cyc_String_pa_PrintArg_struct _tmp9A6;_tmp9A6.tag=0U,({struct _dyneither_ptr _tmpB01=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc));_tmp9A6.f1=_tmpB01;});_tmp9A6;});void*_tmp425[2U];_tmp425[0]=& _tmp427,_tmp425[1]=& _tmp428;({struct _dyneither_ptr _tmpB02=({const char*_tmp426="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp426,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB02,_tag_dyneither(_tmp425,sizeof(void*),2U));});});
return retv;};};}
# 2544
return 0;}
# 2547
int Cyc_Tcutil_warn_alias_coerce=0;
# 2553
struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2557
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB05=({struct _dyneither_ptr*_tmp43B=_cycalloc(sizeof(*_tmp43B));({struct _dyneither_ptr _tmpB04=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp43A=({struct Cyc_Int_pa_PrintArg_struct _tmp9AA;_tmp9AA.tag=1U,_tmp9AA.f1=(unsigned long)counter ++;_tmp9AA;});void*_tmp438[1U];_tmp438[0]=& _tmp43A;({struct _dyneither_ptr _tmpB03=({const char*_tmp439="__aliasvar%d";_tag_dyneither(_tmp439,sizeof(char),13U);});Cyc_aprintf(_tmpB03,_tag_dyneither(_tmp438,sizeof(void*),1U));});});*_tmp43B=_tmpB04;});_tmp43B;});_tmp43C->f2=_tmpB05;});_tmp43C;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpB06=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->tag=4U,_tmp437->f1=vd;_tmp437;});Cyc_Absyn_varb_exp(_tmpB06,e->loc);});
# 2566
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2568
{void*_tmp429=Cyc_Tcutil_compress(e_type);void*_tmp42A=_tmp429;void*_tmp436;struct Cyc_Absyn_Tqual _tmp435;void*_tmp434;void*_tmp433;void*_tmp432;void*_tmp431;struct Cyc_Absyn_PtrLoc*_tmp430;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->tag == 3U){_LL1: _tmp436=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->f1).elt_type;_tmp435=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->f1).elt_tq;_tmp434=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->f1).ptr_atts).rgn;_tmp433=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->f1).ptr_atts).nullable;_tmp432=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->f1).ptr_atts).bounds;_tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->f1).ptr_atts).zero_term;_tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->f1).ptr_atts).ptrloc;_LL2:
# 2570
{void*_tmp42B=Cyc_Tcutil_compress(_tmp434);void*_tmp42C=_tmp42B;void**_tmp42F;struct Cyc_Core_Opt*_tmp42E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42C)->tag == 1U){_LL6: _tmp42F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42C)->f2;_tmp42E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42C)->f4;_LL7: {
# 2572
void*_tmp42D=Cyc_Absyn_var_type(tv);
*_tmp42F=_tmp42D;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2577
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2581
e->topt=0;
vd->initializer=0;{
# 2585
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2587
return({struct _tuple14 _tmp9A9;_tmp9A9.f1=d,_tmp9A9.f2=ve;_tmp9A9;});};}
# 2592
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned int loc){
# 2595
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_type,0)) && 
Cyc_Tcutil_is_pointer_type(e_type)){
# 2600
void*_tmp43D=Cyc_Tcutil_compress(wants_type);void*_tmp43E=_tmp43D;void*_tmp440;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43E)->tag == 3U){_LL1: _tmp440=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43E)->f1).ptr_atts).rgn;_LL2:
# 2602
 if(Cyc_Tcutil_is_heap_rgn_type(_tmp440))return 0;{
struct Cyc_Absyn_Kind*_tmp43F=Cyc_Tcutil_type_kind(_tmp440);
return(int)_tmp43F->kind == (int)Cyc_Absyn_RgnKind  && (int)_tmp43F->aliasqual == (int)Cyc_Absyn_Aliasable;};}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2608
return 0;}
# 2612
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2617
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2619
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2621
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp443=({struct Cyc_String_pa_PrintArg_struct _tmp9AC;_tmp9AC.tag=0U,({
struct _dyneither_ptr _tmpB07=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9AC.f1=_tmpB07;});_tmp9AC;});struct Cyc_String_pa_PrintArg_struct _tmp444=({struct Cyc_String_pa_PrintArg_struct _tmp9AB;_tmp9AB.tag=0U,({struct _dyneither_ptr _tmpB08=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9AB.f1=_tmpB08;});_tmp9AB;});void*_tmp441[2U];_tmp441[0]=& _tmp443,_tmp441[1]=& _tmp444;({unsigned int _tmpB0A=e->loc;struct _dyneither_ptr _tmpB09=({const char*_tmp442="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp442,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpB0A,_tmpB09,_tag_dyneither(_tmp441,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2629
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp447=({struct Cyc_String_pa_PrintArg_struct _tmp9AF;_tmp9AF.tag=0U,({
struct _dyneither_ptr _tmpB0B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp9AF.f1=_tmpB0B;});_tmp9AF;});struct Cyc_String_pa_PrintArg_struct _tmp448=({struct Cyc_String_pa_PrintArg_struct _tmp9AE;_tmp9AE.tag=0U,({struct _dyneither_ptr _tmpB0C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9AE.f1=_tmpB0C;});_tmp9AE;});struct Cyc_String_pa_PrintArg_struct _tmp449=({struct Cyc_String_pa_PrintArg_struct _tmp9AD;_tmp9AD.tag=0U,({struct _dyneither_ptr _tmpB0D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9AD.f1=_tmpB0D;});_tmp9AD;});void*_tmp445[3U];_tmp445[0]=& _tmp447,_tmp445[1]=& _tmp448,_tmp445[2]=& _tmp449;({unsigned int _tmpB0F=e->loc;struct _dyneither_ptr _tmpB0E=({const char*_tmp446="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp446,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpB0F,_tmpB0E,_tag_dyneither(_tmp445,sizeof(void*),3U));});});
*alias_coercion=1;}
# 2636
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((int)(c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 2643
if((int)c != (int)1U)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp44C=({struct Cyc_String_pa_PrintArg_struct _tmp9B1;_tmp9B1.tag=0U,({
struct _dyneither_ptr _tmpB10=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9B1.f1=_tmpB10;});_tmp9B1;});struct Cyc_String_pa_PrintArg_struct _tmp44D=({struct Cyc_String_pa_PrintArg_struct _tmp9B0;_tmp9B0.tag=0U,({struct _dyneither_ptr _tmpB11=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9B0.f1=_tmpB11;});_tmp9B0;});void*_tmp44A[2U];_tmp44A[0]=& _tmp44C,_tmp44A[1]=& _tmp44D;({unsigned int _tmpB13=e->loc;struct _dyneither_ptr _tmpB12=({const char*_tmp44B="implicit cast from %s to %s";_tag_dyneither(_tmp44B,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpB13,_tmpB12,_tag_dyneither(_tmp44A,sizeof(void*),2U));});});
return 1;}else{
# 2649
return 0;}}}}}
# 2656
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2659
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2673 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple25{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2677
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple25*env,struct Cyc_Absyn_Aggrfield*x){
# 2680
struct _tuple25 _tmp44E=*env;struct _tuple25 _tmp44F=_tmp44E;struct Cyc_List_List*_tmp457;struct _RegionHandle*_tmp456;struct Cyc_Tcenv_Tenv*_tmp455;int _tmp454;_LL1: _tmp457=_tmp44F.f1;_tmp456=_tmp44F.f2;_tmp455=_tmp44F.f3;_tmp454=_tmp44F.f4;_LL2:;{
# 2682
void*_tmp450=_tmp457 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp456,_tmp457,x->type);
struct Cyc_List_List*_tmp451=Cyc_Tcutil_flatten_type(_tmp456,_tmp454,_tmp455,_tmp450);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp451)== 1)
return({struct Cyc_List_List*_tmp453=_region_malloc(_tmp456,sizeof(*_tmp453));({struct _tuple12*_tmpB14=({struct _tuple12*_tmp452=_region_malloc(_tmp456,sizeof(*_tmp452));_tmp452->f1=x->tq,_tmp452->f2=_tmp450;_tmp452;});_tmp453->hd=_tmpB14;}),_tmp453->tl=0;_tmp453;});else{
return _tmp451;}};}struct _tuple26{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2688
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple26*env,struct _tuple12*x){
# 2690
struct _tuple26 _tmp458=*env;struct _tuple26 _tmp459=_tmp458;struct _RegionHandle*_tmp463;struct Cyc_Tcenv_Tenv*_tmp462;int _tmp461;_LL1: _tmp463=_tmp459.f1;_tmp462=_tmp459.f2;_tmp461=_tmp459.f3;_LL2:;{
struct _tuple12 _tmp45A=*x;struct _tuple12 _tmp45B=_tmp45A;struct Cyc_Absyn_Tqual _tmp460;void*_tmp45F;_LL4: _tmp460=_tmp45B.f1;_tmp45F=_tmp45B.f2;_LL5:;{
struct Cyc_List_List*_tmp45C=Cyc_Tcutil_flatten_type(_tmp463,_tmp461,_tmp462,_tmp45F);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp45C)== 1)
return({struct Cyc_List_List*_tmp45E=_region_malloc(_tmp463,sizeof(*_tmp45E));({struct _tuple12*_tmpB15=({struct _tuple12*_tmp45D=_region_malloc(_tmp463,sizeof(*_tmp45D));_tmp45D->f1=_tmp460,_tmp45D->f2=_tmp45F;_tmp45D;});_tmp45E->hd=_tmpB15;}),_tmp45E->tl=0;_tmp45E;});else{
return _tmp45C;}};};}
# 2697
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2701
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp464=t1;struct Cyc_List_List*_tmp483;struct Cyc_List_List*_tmp482;struct Cyc_Absyn_Aggrdecl*_tmp481;struct Cyc_List_List*_tmp480;switch(*((int*)_tmp464)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp464)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp464)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp481=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp464)->f1)->f1).KnownAggr).val;_tmp480=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp464)->f2;_LL6:
# 2719
 if((((int)_tmp481->kind == (int)Cyc_Absyn_UnionA  || _tmp481->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp481->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp481->impl))->rgn_po != 0)
# 2721
return({struct Cyc_List_List*_tmp46F=_region_malloc(r,sizeof(*_tmp46F));({struct _tuple12*_tmpB17=({struct _tuple12*_tmp46E=_region_malloc(r,sizeof(*_tmp46E));({struct Cyc_Absyn_Tqual _tmpB16=Cyc_Absyn_empty_tqual(0U);_tmp46E->f1=_tmpB16;}),_tmp46E->f2=t1;_tmp46E;});_tmp46F->hd=_tmpB17;}),_tmp46F->tl=0;_tmp46F;});{
struct Cyc_List_List*_tmp470=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp481->tvs,_tmp480);
struct _tuple25 env=({struct _tuple25 _tmp9B2;_tmp9B2.f1=_tmp470,_tmp9B2.f2=r,_tmp9B2.f3=te,_tmp9B2.f4=flatten;_tmp9B2;});
struct Cyc_List_List*_tmp471=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp481->impl))->fields;struct Cyc_List_List*_tmp472=_tmp471;struct Cyc_Absyn_Aggrfield*_tmp478;struct Cyc_List_List*_tmp477;if(_tmp472 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp478=(struct Cyc_Absyn_Aggrfield*)_tmp472->hd;_tmp477=_tmp472->tl;_LL14: {
# 2727
struct Cyc_List_List*_tmp473=Cyc_Tcutil_flatten_type_f(& env,_tmp478);
env.f4=0;{
struct Cyc_List_List*_tmp474=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp477);
struct Cyc_List_List*_tmp475=({struct Cyc_List_List*_tmp476=_region_malloc(r,sizeof(*_tmp476));_tmp476->hd=_tmp473,_tmp476->tl=_tmp474;_tmp476;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp475);};}}_LL10:;};}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp482=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp464)->f1;_LL4: {
# 2706
struct _tuple26 _tmp465=({struct _tuple26 _tmp9B3;_tmp9B3.f1=r,_tmp9B3.f2=te,_tmp9B3.f3=flatten;_tmp9B3;});
# 2708
struct Cyc_List_List*_tmp466=_tmp482;struct _tuple12*_tmp46D;struct Cyc_List_List*_tmp46C;if(_tmp466 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp46D=(struct _tuple12*)_tmp466->hd;_tmp46C=_tmp466->tl;_LLF: {
# 2711
struct Cyc_List_List*_tmp467=Cyc_Tcutil_rcopy_tqt(& _tmp465,_tmp46D);
_tmp465.f3=0;{
struct Cyc_List_List*_tmp468=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple26*,struct _tuple12*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp465,_tmp482);
struct Cyc_List_List*_tmp469=({struct Cyc_List_List*_tmp46B=_region_malloc(r,sizeof(*_tmp46B));_tmp46B->hd=_tmp467,_tmp46B->tl=_tmp468;_tmp46B;});
return({struct _RegionHandle*_tmpB18=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpB18,({struct Cyc_List_List*_tmp46A=_region_malloc(r,sizeof(*_tmp46A));_tmp46A->hd=_tmp467,_tmp46A->tl=_tmp468;_tmp46A;}));});};}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp464)->f1 == Cyc_Absyn_StructA){_LL7: _tmp483=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp464)->f2;_LL8: {
# 2734
struct _tuple25 env=({struct _tuple25 _tmp9B4;_tmp9B4.f1=0,_tmp9B4.f2=r,_tmp9B4.f3=te,_tmp9B4.f4=flatten;_tmp9B4;});
struct Cyc_List_List*_tmp479=_tmp483;struct Cyc_Absyn_Aggrfield*_tmp47F;struct Cyc_List_List*_tmp47E;if(_tmp479 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp47F=(struct Cyc_Absyn_Aggrfield*)_tmp479->hd;_tmp47E=_tmp479->tl;_LL19: {
# 2738
struct Cyc_List_List*_tmp47A=Cyc_Tcutil_flatten_type_f(& env,_tmp47F);
env.f4=0;{
struct Cyc_List_List*_tmp47B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp47E);
struct Cyc_List_List*_tmp47C=({struct Cyc_List_List*_tmp47D=_region_malloc(r,sizeof(*_tmp47D));_tmp47D->hd=_tmp47A,_tmp47D->tl=_tmp47B;_tmp47D;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp47C);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2744
 goto _LL0;}_LL0:;};}
# 2747
return({struct Cyc_List_List*_tmp485=_region_malloc(r,sizeof(*_tmp485));({struct _tuple12*_tmpB1A=({struct _tuple12*_tmp484=_region_malloc(r,sizeof(*_tmp484));({struct Cyc_Absyn_Tqual _tmpB19=Cyc_Absyn_empty_tqual(0U);_tmp484->f1=_tmpB19;}),_tmp484->f2=t1;_tmp484;});_tmp485->hd=_tmpB1A;}),_tmp485->tl=0;_tmp485;});}
# 2751
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp486=(void*)t->hd;void*_tmp487=_tmp486;switch(*((int*)_tmp487)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2757
 continue;default: _LL7: _LL8:
# 2759
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2762
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2765
return 1;}
# 2768
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp488=({struct _tuple0 _tmp9B5;({void*_tmpB1C=Cyc_Tcutil_compress(t1);_tmp9B5.f1=_tmpB1C;}),({void*_tmpB1B=Cyc_Tcutil_compress(t2);_tmp9B5.f2=_tmpB1B;});_tmp9B5;});struct _tuple0 _tmp489=_tmp488;enum Cyc_Absyn_Size_of _tmp48B;enum Cyc_Absyn_Size_of _tmp48A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp489.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp489.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp489.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp489.f2)->f1)->tag == 1U){_LL1: _tmp48B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp489.f1)->f1)->f2;_tmp48A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp489.f2)->f1)->f2;_LL2:
# 2771
 return((int)_tmp48B == (int)_tmp48A  || (int)_tmp48B == (int)2U  && (int)_tmp48A == (int)3U) || 
(int)_tmp48B == (int)3U  && (int)_tmp48A == (int)Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2779
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2782
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp48C=({struct _tuple0 _tmp9B6;_tmp9B6.f1=t1,_tmp9B6.f2=t2;_tmp9B6;});struct _tuple0 _tmp48D=_tmp48C;struct Cyc_Absyn_FnInfo _tmp4B7;struct Cyc_Absyn_FnInfo _tmp4B6;struct Cyc_Absyn_Datatypedecl*_tmp4B5;struct Cyc_Absyn_Datatypefield*_tmp4B4;struct Cyc_List_List*_tmp4B3;struct Cyc_Absyn_Datatypedecl*_tmp4B2;struct Cyc_List_List*_tmp4B1;void*_tmp4B0;struct Cyc_Absyn_Tqual _tmp4AF;void*_tmp4AE;void*_tmp4AD;void*_tmp4AC;void*_tmp4AB;void*_tmp4AA;struct Cyc_Absyn_Tqual _tmp4A9;void*_tmp4A8;void*_tmp4A7;void*_tmp4A6;void*_tmp4A5;switch(*((int*)_tmp48D.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f2)->tag == 3U){_LL1: _tmp4B0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f1)->f1).elt_type;_tmp4AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f1)->f1).elt_tq;_tmp4AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f1)->f1).ptr_atts).rgn;_tmp4AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f1)->f1).ptr_atts).nullable;_tmp4AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f1)->f1).ptr_atts).bounds;_tmp4AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f1)->f1).ptr_atts).zero_term;_tmp4AA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f2)->f1).elt_type;_tmp4A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f2)->f1).elt_tq;_tmp4A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f2)->f1).ptr_atts).rgn;_tmp4A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f2)->f1).ptr_atts).nullable;_tmp4A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f2)->f1).ptr_atts).bounds;_tmp4A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D.f2)->f1).ptr_atts).zero_term;_LL2:
# 2794
 if(_tmp4AF.real_const  && !_tmp4A9.real_const)
return 0;
# 2797
if((!Cyc_Tcutil_unify(_tmp4AD,_tmp4A7) && 
Cyc_Absyn_type2bool(0,_tmp4AD)) && !Cyc_Absyn_type2bool(0,_tmp4A7))
return 0;
# 2801
if((Cyc_Tcutil_unify(_tmp4AB,_tmp4A5) && !
Cyc_Absyn_type2bool(0,_tmp4AB)) && Cyc_Absyn_type2bool(0,_tmp4A5))
return 0;
# 2805
if((!Cyc_Tcutil_unify(_tmp4AE,_tmp4A8) && !Cyc_Tcenv_region_outlives(te,_tmp4AE,_tmp4A8)) && !
Cyc_Tcutil_subtype(te,assume,_tmp4AE,_tmp4A8))
return 0;
# 2809
if(!Cyc_Tcutil_unify(_tmp4AC,_tmp4A6)){
struct Cyc_Absyn_Exp*_tmp48E=({void*_tmpB1D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB1D,_tmp4AC);});
struct Cyc_Absyn_Exp*_tmp48F=({void*_tmpB1E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB1E,_tmp4A6);});
if(_tmp48E != _tmp48F){
if((_tmp48E == 0  || _tmp48F == 0) || !Cyc_Evexp_lte_const_exp(_tmp48F,_tmp48F))
return 0;}}
# 2819
if(!_tmp4A9.real_const  && _tmp4AF.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4AA)))
return 0;}{
# 2825
int _tmp490=
({void*_tmpB1F=_tmp4A6;Cyc_Tcutil_unify(_tmpB1F,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp4A5);
# 2830
return(_tmp490  && ({struct Cyc_Tcenv_Tenv*_tmpB23=te;struct Cyc_List_List*_tmpB22=({struct Cyc_List_List*_tmp492=_cycalloc(sizeof(*_tmp492));({struct _tuple0*_tmpB20=({struct _tuple0*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->f1=t1,_tmp491->f2=t2;_tmp491;});_tmp492->hd=_tmpB20;}),_tmp492->tl=assume;_tmp492;});void*_tmpB21=_tmp4B0;Cyc_Tcutil_ptrsubtype(_tmpB23,_tmpB22,_tmpB21,_tmp4AA);}) || Cyc_Tcutil_unify(_tmp4B0,_tmp4AA)) || Cyc_Tcutil_isomorphic(_tmp4B0,_tmp4AA);};}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp4B5=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp4B4=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp4B3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f1)->f2;_tmp4B2=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f2)->f1)->f1).KnownDatatype).val;_tmp4B1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48D.f2)->f2;_LL4:
# 2837
 if(_tmp4B5 != _tmp4B2  && Cyc_Absyn_qvar_cmp(_tmp4B5->name,_tmp4B2->name)!= 0)return 0;
# 2839
if(({int _tmpB24=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B3);_tmpB24 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B1);}))return 0;
for(0;_tmp4B3 != 0;(_tmp4B3=_tmp4B3->tl,_tmp4B1=_tmp4B1->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp4B3->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp4B1))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48D.f2)->tag == 5U){_LL5: _tmp4B7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48D.f1)->f1;_tmp4B6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48D.f2)->f1;_LL6:
# 2846
 if(_tmp4B7.tvars != 0  || _tmp4B6.tvars != 0){
struct Cyc_List_List*_tmp493=_tmp4B7.tvars;
struct Cyc_List_List*_tmp494=_tmp4B6.tvars;
if(({int _tmpB25=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp493);_tmpB25 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp494);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp493 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp493->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp494))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp496=_cycalloc(sizeof(*_tmp496));({struct _tuple15*_tmpB27=({struct _tuple15*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495->f1=(struct Cyc_Absyn_Tvar*)_tmp494->hd,({void*_tmpB26=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp493->hd);_tmp495->f2=_tmpB26;});_tmp495;});_tmp496->hd=_tmpB27;}),_tmp496->tl=inst;_tmp496;});
_tmp493=_tmp493->tl;
_tmp494=_tmp494->tl;}
# 2857
if(inst != 0){
_tmp4B7.tvars=0;
_tmp4B6.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpB2A=te;struct Cyc_List_List*_tmpB29=assume;void*_tmpB28=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->tag=5U,_tmp497->f1=_tmp4B7;_tmp497;});Cyc_Tcutil_subtype(_tmpB2A,_tmpB29,_tmpB28,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->tag=5U,_tmp498->f1=_tmp4B6;_tmp498;}));});}};}
# 2864
if(!Cyc_Tcutil_subtype(te,assume,_tmp4B7.ret_type,_tmp4B6.ret_type))return 0;{
struct Cyc_List_List*_tmp499=_tmp4B7.args;
struct Cyc_List_List*_tmp49A=_tmp4B6.args;
# 2869
if(({int _tmpB2B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp499);_tmpB2B != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49A);}))return 0;
# 2871
for(0;_tmp499 != 0;(_tmp499=_tmp499->tl,_tmp49A=_tmp49A->tl)){
struct _tuple10 _tmp49B=*((struct _tuple10*)_tmp499->hd);struct _tuple10 _tmp49C=_tmp49B;struct Cyc_Absyn_Tqual _tmp4A2;void*_tmp4A1;_LLA: _tmp4A2=_tmp49C.f2;_tmp4A1=_tmp49C.f3;_LLB:;{
struct _tuple10 _tmp49D=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp49A))->hd);struct _tuple10 _tmp49E=_tmp49D;struct Cyc_Absyn_Tqual _tmp4A0;void*_tmp49F;_LLD: _tmp4A0=_tmp49E.f2;_tmp49F=_tmp49E.f3;_LLE:;
# 2875
if(_tmp4A0.real_const  && !_tmp4A2.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp49F,_tmp4A1))
return 0;};}
# 2879
if(_tmp4B7.c_varargs != _tmp4B6.c_varargs)return 0;
if(_tmp4B7.cyc_varargs != 0  && _tmp4B6.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp4A3=*_tmp4B7.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp4A4=*_tmp4B6.cyc_varargs;
# 2884
if((_tmp4A4.tq).real_const  && !(_tmp4A3.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp4A4.type,_tmp4A3.type))
return 0;}else{
if(_tmp4B7.cyc_varargs != 0  || _tmp4B6.cyc_varargs != 0)return 0;}
# 2889
if(!({void*_tmpB2C=(void*)_check_null(_tmp4B7.effect);Cyc_Tcutil_subset_effect(1,_tmpB2C,(void*)_check_null(_tmp4B6.effect));}))return 0;
# 2891
if(!Cyc_Tcutil_sub_rgnpo(_tmp4B7.rgn_po,_tmp4B6.rgn_po))return 0;
# 2893
if(!Cyc_Tcutil_sub_attributes(_tmp4B7.attributes,_tmp4B6.attributes))return 0;
# 2895
if(!Cyc_Tcutil_check_logical_implication(_tmp4B6.requires_relns,_tmp4B7.requires_relns))
return 0;
# 2898
if(!Cyc_Tcutil_check_logical_implication(_tmp4B7.ensures_relns,_tmp4B6.ensures_relns))
return 0;
# 2901
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2913 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2915
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp4B8=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp4B9=_tmp4B8;struct Cyc_Absyn_Tqual _tmp4BF;void*_tmp4BE;_LL1: _tmp4BF=_tmp4B9->f1;_tmp4BE=_tmp4B9->f2;_LL2:;{
struct _tuple12*_tmp4BA=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp4BB=_tmp4BA;struct Cyc_Absyn_Tqual _tmp4BD;void*_tmp4BC;_LL4: _tmp4BD=_tmp4BB->f1;_tmp4BC=_tmp4BB->f2;_LL5:;
# 2922
if(_tmp4BF.real_const  && !_tmp4BD.real_const)return 0;
# 2924
if((_tmp4BD.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4BC))) && 
Cyc_Tcutil_subtype(te,assume,_tmp4BE,_tmp4BC))
# 2927
continue;
# 2929
if(Cyc_Tcutil_unify(_tmp4BE,_tmp4BC))
# 2931
continue;
# 2933
if(Cyc_Tcutil_isomorphic(_tmp4BE,_tmp4BC))
# 2935
continue;
# 2938
return 0;};};}
# 2940
return 1;}
# 2945
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2951
{void*_tmp4C0=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C0)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C0)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C0)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2954
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2956
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2960
void*_tmp4C1=t1;void*_tmp4E6;struct Cyc_Absyn_Enumdecl*_tmp4E5;void*_tmp4E4;struct Cyc_Absyn_Tqual _tmp4E3;struct Cyc_Absyn_Exp*_tmp4E2;void*_tmp4E1;void*_tmp4E0;struct Cyc_Absyn_Tqual _tmp4DF;void*_tmp4DE;void*_tmp4DD;void*_tmp4DC;void*_tmp4DB;switch(*((int*)_tmp4C1)){case 3U: _LLA: _tmp4E0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).elt_type;_tmp4DF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).elt_tq;_tmp4DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).ptr_atts).rgn;_tmp4DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).ptr_atts).nullable;_tmp4DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).ptr_atts).bounds;_tmp4DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).ptr_atts).zero_term;_LLB:
# 2969
{void*_tmp4C2=t2;void*_tmp4D1;struct Cyc_Absyn_Tqual _tmp4D0;void*_tmp4CF;void*_tmp4CE;void*_tmp4CD;void*_tmp4CC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->tag == 3U){_LL19: _tmp4D1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).elt_type;_tmp4D0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).elt_tq;_tmp4CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).ptr_atts).rgn;_tmp4CE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).ptr_atts).nullable;_tmp4CD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).ptr_atts).bounds;_tmp4CC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).ptr_atts).zero_term;_LL1A: {
# 2973
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp4C3=({struct Cyc_List_List*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));({struct _tuple0*_tmpB2D=({struct _tuple0*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->f1=t1,_tmp4CA->f2=t2;_tmp4CA;});_tmp4CB->hd=_tmpB2D;}),_tmp4CB->tl=0;_tmp4CB;});
int _tmp4C4=_tmp4D0.real_const  || !_tmp4DF.real_const;
# 2987 "tcutil.cyc"
int _tmp4C5=
({void*_tmpB2E=_tmp4CD;Cyc_Tcutil_unify(_tmpB2E,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp4CC);
# 2990
int _tmp4C6=_tmp4C4  && (
((_tmp4C5  && Cyc_Tcutil_ptrsubtype(te,_tmp4C3,_tmp4E0,_tmp4D1) || 
Cyc_Tcutil_unify(_tmp4E0,_tmp4D1)) || Cyc_Tcutil_isomorphic(_tmp4E0,_tmp4D1)) || Cyc_Tcutil_unify(_tmp4D1,Cyc_Absyn_void_type));
# 2994
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=Cyc_Tcutil_unify(_tmp4DB,_tmp4CC) || !Cyc_Absyn_type2bool(0,_tmp4CC);
# 2998
int _tmp4C7=_tmp4C6?0:((Cyc_Tcutil_is_bits_only_type(_tmp4E0) && Cyc_Tcutil_is_char_type(_tmp4D1)) && !
Cyc_Tcutil_force_type2bool(0,_tmp4CC)) && (
_tmp4D0.real_const  || !_tmp4DF.real_const);
int bounds_ok=Cyc_Tcutil_unify(_tmp4DC,_tmp4CD);
if(!bounds_ok  && !_tmp4C7){
struct Cyc_Absyn_Exp*_tmp4C8=({void*_tmpB2F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB2F,_tmp4DC);});
struct Cyc_Absyn_Exp*_tmp4C9=({void*_tmpB30=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB30,_tmp4CD);});
if((_tmp4C8 != 0  && _tmp4C9 != 0) && Cyc_Evexp_lte_const_exp(_tmp4C9,_tmp4C8))
bounds_ok=1;else{
if(_tmp4C8 == 0  || _tmp4C9 == 0)
bounds_ok=1;}}{
# 3010
int t1_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4DD);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4CE);
if(t1_nullable  && !t2_nullable)
coercion=2U;
# 3017
if(((bounds_ok  && zeroterm_ok) && (_tmp4C6  || _tmp4C7)) && (
Cyc_Tcutil_unify(_tmp4DE,_tmp4CF) || Cyc_Tcenv_region_outlives(te,_tmp4DE,_tmp4CF)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 3023
return Cyc_Absyn_Unknown_coercion;case 4U: _LLC: _tmp4E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C1)->f1).elt_type;_tmp4E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C1)->f1).tq;_tmp4E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C1)->f1).num_elts;_tmp4E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C1)->f1).zero_term;_LLD:
# 3025
{void*_tmp4D2=t2;void*_tmp4D6;struct Cyc_Absyn_Tqual _tmp4D5;struct Cyc_Absyn_Exp*_tmp4D4;void*_tmp4D3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->tag == 4U){_LL1E: _tmp4D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->f1).elt_type;_tmp4D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->f1).tq;_tmp4D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->f1).num_elts;_tmp4D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->f1).zero_term;_LL1F: {
# 3027
int okay;
okay=
(((_tmp4E2 != 0  && _tmp4D4 != 0) && Cyc_Tcutil_unify(_tmp4E1,_tmp4D3)) && 
Cyc_Evexp_lte_const_exp(_tmp4D4,_tmp4E2)) && 
Cyc_Evexp_lte_const_exp(_tmp4E2,_tmp4D4);
return
# 3034
(okay  && Cyc_Tcutil_unify(_tmp4E4,_tmp4D6)) && (!_tmp4E3.real_const  || _tmp4D5.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 3036
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 3038
return Cyc_Absyn_Unknown_coercion;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f1)){case 15U: _LLE: _tmp4E5=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f1)->f2;_LLF:
# 3042
{void*_tmp4D7=t2;struct Cyc_Absyn_Enumdecl*_tmp4D8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D7)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D7)->f1)->tag == 15U){_LL23: _tmp4D8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D7)->f1)->f2;_LL24:
# 3044
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4E5))->fields != 0  && ((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4D8))->fields != 0) && ({
int _tmpB31=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4E5->fields))->v);_tmpB31 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4D8->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 3050
goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 3053
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f2 != 0){_LL14: _tmp4E6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f2)->hd;_LL15:
# 3056
{void*_tmp4D9=t2;void*_tmp4DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D9)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D9)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D9)->f2 != 0){_LL28: _tmp4DA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D9)->f2)->hd;_LL29:
# 3058
 if(Cyc_Tcenv_region_outlives(te,_tmp4E6,_tmp4DA))return Cyc_Absyn_No_coercion;
goto _LL27;}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 3062
return Cyc_Absyn_Unknown_coercion;}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;};}
# 3068
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp4E7=Cyc_Absyn_copy_exp(e);
({void*_tmpB32=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8->tag=14U,_tmp4E8->f1=t,_tmp4E8->f2=_tmp4E7,_tmp4E8->f3=0,_tmp4E8->f4=c;_tmp4E8;});e->r=_tmpB32;});
e->topt=t;}}
# 3076
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple0 _tmp4E9=({struct _tuple0 _tmp9B8;({void*_tmpB34=Cyc_Tcutil_compress(t1);_tmp9B8.f1=_tmpB34;}),({void*_tmpB33=Cyc_Tcutil_compress(t2);_tmp9B8.f2=_tmpB33;});_tmp9B8;});struct _tuple0 _tmp4EA=_tmp4E9;void*_tmp4F0;void*_tmp4EF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EA.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EA.f2)->tag == 0U){_LL1: _tmp4F0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EA.f1)->f1;_tmp4EF=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EA.f2)->f1;_LL2:
# 3079
{struct _tuple0 _tmp4EB=({struct _tuple0 _tmp9B7;_tmp9B7.f1=_tmp4F0,_tmp9B7.f2=_tmp4EF;_tmp9B7;});struct _tuple0 _tmp4EC=_tmp4EB;int _tmp4EE;int _tmp4ED;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EC.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EC.f2)->tag == 2U){_LL6: _tmp4EE=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f1;_tmp4ED=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f1;_LL7:
# 3081
 if(_tmp4EE != 0  && _tmp4EE != 1)return t1;else{
if(_tmp4ED != 0  && _tmp4ED != 1)return t2;else{
if(_tmp4EE >= _tmp4ED)return t1;else{
return t2;}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EC.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 3091
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EC.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 3096
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 3089
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EC.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 3098
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 3088
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 3092
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EC.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 3097
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 3090
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EC.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EC.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 3099
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EC.f1)->tag == 4U){_LL18: _LL19:
# 3094
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EC.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 3100
 goto _LL5;}}}}}}_LL5:;}
# 3102
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3105
return Cyc_Absyn_sint_type;}
# 3115 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp4F1=({struct _tuple0 _tmp9B9;_tmp9B9.f1=c1,_tmp9B9.f2=c2;_tmp9B9;});struct _tuple0 _tmp4F2=_tmp4F1;struct Cyc_Core_Opt**_tmp506;struct Cyc_Absyn_Kind*_tmp505;struct Cyc_Core_Opt**_tmp504;struct Cyc_Absyn_Kind*_tmp503;struct Cyc_Core_Opt**_tmp502;struct Cyc_Absyn_Kind*_tmp501;struct Cyc_Absyn_Kind*_tmp500;struct Cyc_Core_Opt**_tmp4FF;struct Cyc_Core_Opt**_tmp4FE;struct Cyc_Absyn_Kind*_tmp4FD;struct Cyc_Core_Opt**_tmp4FC;struct Cyc_Absyn_Kind*_tmp4FB;struct Cyc_Absyn_Kind*_tmp4FA;struct Cyc_Absyn_Kind*_tmp4F9;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F2.f1)->tag == 0U)switch(*((int*)_tmp4F2.f2)){case 0U: _LL1: _tmp4FA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F2.f1)->f1;_tmp4F9=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F2.f2)->f1;_LL2:
 return _tmp4FA == _tmp4F9;case 1U: goto _LL3;default: _LL9: _tmp4FD=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F2.f1)->f1;_tmp4FC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F2.f2)->f1;_tmp4FB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F2.f2)->f2;_LLA:
# 3128
 if(Cyc_Tcutil_kind_leq(_tmp4FD,_tmp4FB)){
({struct Cyc_Core_Opt*_tmpB35=({struct Cyc_Core_Opt*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->v=c1;_tmp4F6;});*_tmp4FC=_tmpB35;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F2.f2)->tag == 1U){_LL3: _tmp4FE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F2.f2)->f1;_LL4:
# 3121
({struct Cyc_Core_Opt*_tmpB36=({struct Cyc_Core_Opt*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->v=c1;_tmp4F3;});*_tmp4FE=_tmpB36;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F2.f1)->tag == 1U){_LL5: _tmp4FF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F2.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpB37=({struct Cyc_Core_Opt*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->v=c2;_tmp4F4;});*_tmp4FF=_tmpB37;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F2.f2)->tag == 0U){_LL7: _tmp502=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F2.f1)->f1;_tmp501=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F2.f1)->f2;_tmp500=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F2.f2)->f1;_LL8:
# 3124
 if(Cyc_Tcutil_kind_leq(_tmp500,_tmp501)){
({struct Cyc_Core_Opt*_tmpB38=({struct Cyc_Core_Opt*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->v=c2;_tmp4F5;});*_tmp502=_tmpB38;});return 1;}else{
return 0;}}else{_LLB: _tmp506=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F2.f1)->f1;_tmp505=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F2.f1)->f2;_tmp504=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F2.f2)->f1;_tmp503=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F2.f2)->f2;_LLC:
# 3132
 if(Cyc_Tcutil_kind_leq(_tmp505,_tmp503)){
({struct Cyc_Core_Opt*_tmpB39=({struct Cyc_Core_Opt*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->v=c1;_tmp4F7;});*_tmp504=_tmpB39;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp503,_tmp505)){
({struct Cyc_Core_Opt*_tmpB3A=({struct Cyc_Core_Opt*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->v=c2;_tmp4F8;});*_tmp506=_tmpB3A;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3141
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3146
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp50C=({struct Cyc_Int_pa_PrintArg_struct _tmp9BA;_tmp9BA.tag=1U,_tmp9BA.f1=(unsigned long)i;_tmp9BA;});void*_tmp50A[1U];_tmp50A[0]=& _tmp50C;({struct _dyneither_ptr _tmpB3B=({const char*_tmp50B="#%d";_tag_dyneither(_tmp50B,sizeof(char),4U);});Cyc_aprintf(_tmpB3B,_tag_dyneither(_tmp50A,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp509=_cycalloc(sizeof(*_tmp509));({struct _dyneither_ptr*_tmpB3C=({unsigned int _tmp508=1;struct _dyneither_ptr*_tmp507=_cycalloc(_check_times(_tmp508,sizeof(struct _dyneither_ptr)));_tmp507[0]=s;_tmp507;});_tmp509->name=_tmpB3C;}),_tmp509->identity=- 1,_tmp509->kind=k;_tmp509;});}
# 3153
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp50D=*t->name;
return(int)*((const char*)_check_dyneither_subscript(_tmp50D,sizeof(char),0))== (int)'#';}
# 3158
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp510=({struct Cyc_String_pa_PrintArg_struct _tmp9BB;_tmp9BB.tag=0U,_tmp9BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);_tmp9BB;});void*_tmp50E[1U];_tmp50E[0]=& _tmp510;({struct _dyneither_ptr _tmpB3D=({const char*_tmp50F="%s";_tag_dyneither(_tmp50F,sizeof(char),3U);});Cyc_printf(_tmpB3D,_tag_dyneither(_tmp50E,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp511=({struct _dyneither_ptr _tmpB3E=({const char*_tmp517="`";_tag_dyneither(_tmp517,sizeof(char),2U);});Cyc_strconcat(_tmpB3E,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp512=_dyneither_ptr_plus(_tmp511,sizeof(char),1);char _tmp513=*((char*)_check_dyneither_subscript(_tmp512,sizeof(char),0U));char _tmp514='t';if(_get_dyneither_size(_tmp512,sizeof(char))== 1U  && (_tmp513 == 0  && _tmp514 != 0))_throw_arraybounds();*((char*)_tmp512.curr)=_tmp514;});
({struct _dyneither_ptr*_tmpB3F=({unsigned int _tmp516=1;struct _dyneither_ptr*_tmp515=_cycalloc(_check_times(_tmp516,sizeof(struct _dyneither_ptr)));_tmp515[0]=(struct _dyneither_ptr)_tmp511;_tmp515;});t->name=_tmpB3F;});};}
# 3167
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 3174
struct Cyc_List_List*_tmp518=0;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp518=({struct Cyc_List_List*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->hd=(void*)atts->hd,_tmp519->tl=_tmp518;_tmp519;});}}{
struct Cyc_Absyn_FnInfo _tmp51A=fd->i;
_tmp51A.attributes=_tmp518;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->tag=5U,_tmp51B->f1=_tmp51A;_tmp51B;});};}
# 3182
return(void*)_check_null(fd->cached_type);}
# 3188
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3190
union Cyc_Relations_RelnOp _tmp51C=*rop;union Cyc_Relations_RelnOp _tmp51D=_tmp51C;struct Cyc_Absyn_Vardecl*_tmp530;struct Cyc_Absyn_Vardecl*_tmp52F;switch((_tmp51D.RNumelts).tag){case 2U: _LL1: _tmp52F=(_tmp51D.RVar).val;_LL2: {
# 3192
struct _tuple2 _tmp51E=*_tmp52F->name;struct _tuple2 _tmp51F=_tmp51E;union Cyc_Absyn_Nmspace _tmp526;struct _dyneither_ptr*_tmp525;_LL8: _tmp526=_tmp51F.f1;_tmp525=_tmp51F.f2;_LL9:;
if(!((int)((_tmp526.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpB40=(struct _dyneither_ptr)*_tmp525;Cyc_strcmp(_tmpB40,({const char*_tmp520="return_value";_tag_dyneither(_tmp520,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpB41=Cyc_Relations_RReturn();*rop=_tmpB41;});
goto _LL0;}{
# 3198
unsigned int c=0U;
{struct Cyc_List_List*_tmp521=args;for(0;_tmp521 != 0;(_tmp521=_tmp521->tl,c ++)){
struct _tuple10*_tmp522=(struct _tuple10*)_tmp521->hd;struct _tuple10*_tmp523=_tmp522;struct _dyneither_ptr*_tmp524;_LLB: _tmp524=_tmp523->f1;_LLC:;
if(_tmp524 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp525,(struct _dyneither_ptr)*_tmp524)== 0){
({union Cyc_Relations_RelnOp _tmpB42=Cyc_Relations_RParam(c);*rop=_tmpB42;});
break;}}}}
# 3208
goto _LL0;};}case 3U: _LL3: _tmp530=(_tmp51D.RNumelts).val;_LL4: {
# 3210
struct _tuple2 _tmp527=*_tmp530->name;struct _tuple2 _tmp528=_tmp527;union Cyc_Absyn_Nmspace _tmp52E;struct _dyneither_ptr*_tmp52D;_LLE: _tmp52E=_tmp528.f1;_tmp52D=_tmp528.f2;_LLF:;
if(!((int)((_tmp52E.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp529=args;for(0;_tmp529 != 0;(_tmp529=_tmp529->tl,c ++)){
struct _tuple10*_tmp52A=(struct _tuple10*)_tmp529->hd;struct _tuple10*_tmp52B=_tmp52A;struct _dyneither_ptr*_tmp52C;_LL11: _tmp52C=_tmp52B->f1;_LL12:;
if(_tmp52C != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp52D,(struct _dyneither_ptr)*_tmp52C)== 0){
({union Cyc_Relations_RelnOp _tmpB43=Cyc_Relations_RParamNumelts(c);*rop=_tmpB43;});
break;}}}}
# 3222
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3227
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3229
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3233
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3235
if(e == 0)return 0;{
struct Cyc_List_List*_tmp531=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp531);
return _tmp531;};}
# 3242
static struct _dyneither_ptr*Cyc_Tcutil_fst_fdarg(struct _tuple10*t){return(struct _dyneither_ptr*)_check_null((*t).f1);}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp532=pr;struct Cyc_Absyn_Tqual _tmp535;void*_tmp534;_LL1: _tmp535=_tmp532->f1;_tmp534=_tmp532->f2;_LL2:;
if(_tmp534 == t)return pr;else{
return({struct _tuple12*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533->f1=_tmp535,_tmp533->f2=t;_tmp533;});}}struct _tuple27{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple28{struct _tuple27*f1;void*f2;};
# 3249
static struct _tuple28*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3251
return({struct _tuple28*_tmp537=_region_malloc(rgn,sizeof(*_tmp537));({struct _tuple27*_tmpB44=({struct _tuple27*_tmp536=_region_malloc(rgn,sizeof(*_tmp536));_tmp536->f1=(*y).f1,_tmp536->f2=(*y).f2;_tmp536;});_tmp537->f1=_tmpB44;}),_tmp537->f2=(*y).f3;_tmp537;});}
# 3253
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3255
struct _tuple10 _tmp538=*orig_arg;struct _tuple10 _tmp539=_tmp538;struct _dyneither_ptr*_tmp53D;struct Cyc_Absyn_Tqual _tmp53C;void*_tmp53B;_LL1: _tmp53D=_tmp539.f1;_tmp53C=_tmp539.f2;_tmp53B=_tmp539.f3;_LL2:;
if(t == _tmp53B)return orig_arg;
return({struct _tuple10*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A->f1=_tmp53D,_tmp53A->f2=_tmp53C,_tmp53A->f3=t;_tmp53A;});}
# 3259
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3262
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3267
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3269
return({struct Cyc_Absyn_Exp*_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E->topt=old->topt,_tmp53E->r=r,_tmp53E->loc=old->loc,_tmp53E->annot=old->annot;_tmp53E;});}
# 3274
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp53F=e->r;void*_tmp540=_tmp53F;void*_tmp580;void*_tmp57F;struct Cyc_List_List*_tmp57E;struct Cyc_Absyn_Exp*_tmp57D;struct Cyc_Absyn_Exp*_tmp57C;void*_tmp57B;void*_tmp57A;struct Cyc_Absyn_Exp*_tmp579;int _tmp578;enum Cyc_Absyn_Coercion _tmp577;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Exp*_tmp574;struct Cyc_Absyn_Exp*_tmp573;struct Cyc_Absyn_Exp*_tmp572;struct Cyc_Absyn_Exp*_tmp571;struct Cyc_Absyn_Exp*_tmp570;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Exp*_tmp56E;enum Cyc_Absyn_Primop _tmp56D;struct Cyc_List_List*_tmp56C;switch(*((int*)_tmp540)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp56D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_tmp56C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp540)->f2;_LLC:
# 3283
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56C)== 1){
struct Cyc_Absyn_Exp*_tmp541=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56C))->hd;
struct Cyc_Absyn_Exp*_tmp542=Cyc_Tcutil_rsubsexp(r,inst,_tmp541);
if(_tmp542 == _tmp541)return e;
return({struct Cyc_Absyn_Exp*_tmpB46=e;Cyc_Tcutil_copye(_tmpB46,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544->tag=3U,_tmp544->f1=_tmp56D,({struct Cyc_List_List*_tmpB45=({struct Cyc_Absyn_Exp*_tmp543[1U];_tmp543[0]=_tmp542;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp543,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp544->f2=_tmpB45;});_tmp544;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56C)== 2){
struct Cyc_Absyn_Exp*_tmp545=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56C))->hd;
struct Cyc_Absyn_Exp*_tmp546=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56C->tl))->hd;
struct Cyc_Absyn_Exp*_tmp547=Cyc_Tcutil_rsubsexp(r,inst,_tmp545);
struct Cyc_Absyn_Exp*_tmp548=Cyc_Tcutil_rsubsexp(r,inst,_tmp546);
if(_tmp547 == _tmp545  && _tmp548 == _tmp546)return e;
return({struct Cyc_Absyn_Exp*_tmpB48=e;Cyc_Tcutil_copye(_tmpB48,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->tag=3U,_tmp54A->f1=_tmp56D,({struct Cyc_List_List*_tmpB47=({struct Cyc_Absyn_Exp*_tmp549[2U];_tmp549[0]=_tmp547,_tmp549[1]=_tmp548;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp549,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp54A->f2=_tmpB47;});_tmp54A;}));});}else{
return({void*_tmp54B=0U;({struct _dyneither_ptr _tmpB49=({const char*_tmp54C="primop does not have 1 or 2 args!";_tag_dyneither(_tmp54C,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB49,_tag_dyneither(_tmp54B,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp570=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_tmp56F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp540)->f2;_tmp56E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp540)->f3;_LLE: {
# 3297
struct Cyc_Absyn_Exp*_tmp54D=Cyc_Tcutil_rsubsexp(r,inst,_tmp570);
struct Cyc_Absyn_Exp*_tmp54E=Cyc_Tcutil_rsubsexp(r,inst,_tmp56F);
struct Cyc_Absyn_Exp*_tmp54F=Cyc_Tcutil_rsubsexp(r,inst,_tmp56E);
if((_tmp54D == _tmp570  && _tmp54E == _tmp56F) && _tmp54F == _tmp56E)return e;
return({struct Cyc_Absyn_Exp*_tmpB4A=e;Cyc_Tcutil_copye(_tmpB4A,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->tag=6U,_tmp550->f1=_tmp54D,_tmp550->f2=_tmp54E,_tmp550->f3=_tmp54F;_tmp550;}));});}case 7U: _LLF: _tmp572=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_tmp571=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp540)->f2;_LL10: {
# 3303
struct Cyc_Absyn_Exp*_tmp551=Cyc_Tcutil_rsubsexp(r,inst,_tmp572);
struct Cyc_Absyn_Exp*_tmp552=Cyc_Tcutil_rsubsexp(r,inst,_tmp571);
if(_tmp551 == _tmp572  && _tmp552 == _tmp571)return e;
return({struct Cyc_Absyn_Exp*_tmpB4B=e;Cyc_Tcutil_copye(_tmpB4B,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->tag=7U,_tmp553->f1=_tmp551,_tmp553->f2=_tmp552;_tmp553;}));});}case 8U: _LL11: _tmp574=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_tmp573=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp540)->f2;_LL12: {
# 3308
struct Cyc_Absyn_Exp*_tmp554=Cyc_Tcutil_rsubsexp(r,inst,_tmp574);
struct Cyc_Absyn_Exp*_tmp555=Cyc_Tcutil_rsubsexp(r,inst,_tmp573);
if(_tmp554 == _tmp574  && _tmp555 == _tmp573)return e;
return({struct Cyc_Absyn_Exp*_tmpB4C=e;Cyc_Tcutil_copye(_tmpB4C,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->tag=8U,_tmp556->f1=_tmp554,_tmp556->f2=_tmp555;_tmp556;}));});}case 9U: _LL13: _tmp576=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_tmp575=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp540)->f2;_LL14: {
# 3313
struct Cyc_Absyn_Exp*_tmp557=Cyc_Tcutil_rsubsexp(r,inst,_tmp576);
struct Cyc_Absyn_Exp*_tmp558=Cyc_Tcutil_rsubsexp(r,inst,_tmp575);
if(_tmp557 == _tmp576  && _tmp558 == _tmp575)return e;
return({struct Cyc_Absyn_Exp*_tmpB4D=e;Cyc_Tcutil_copye(_tmpB4D,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->tag=9U,_tmp559->f1=_tmp557,_tmp559->f2=_tmp558;_tmp559;}));});}case 14U: _LL15: _tmp57A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_tmp579=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp540)->f2;_tmp578=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp540)->f3;_tmp577=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp540)->f4;_LL16: {
# 3318
struct Cyc_Absyn_Exp*_tmp55A=Cyc_Tcutil_rsubsexp(r,inst,_tmp579);
void*_tmp55B=Cyc_Tcutil_rsubstitute(r,inst,_tmp57A);
if(_tmp55A == _tmp579  && _tmp55B == _tmp57A)return e;
return({struct Cyc_Absyn_Exp*_tmpB4E=e;Cyc_Tcutil_copye(_tmpB4E,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->tag=14U,_tmp55C->f1=_tmp55B,_tmp55C->f2=_tmp55A,_tmp55C->f3=_tmp578,_tmp55C->f4=_tmp577;_tmp55C;}));});}case 17U: _LL17: _tmp57B=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_LL18: {
# 3323
void*_tmp55D=Cyc_Tcutil_rsubstitute(r,inst,_tmp57B);
if(_tmp55D == _tmp57B)return e;
return({struct Cyc_Absyn_Exp*_tmpB4F=e;Cyc_Tcutil_copye(_tmpB4F,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->tag=17U,_tmp55E->f1=_tmp55D;_tmp55E;}));});}case 18U: _LL19: _tmp57C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_LL1A: {
# 3327
struct Cyc_Absyn_Exp*_tmp55F=Cyc_Tcutil_rsubsexp(r,inst,_tmp57C);
if(_tmp55F == _tmp57C)return e;
return({struct Cyc_Absyn_Exp*_tmpB50=e;Cyc_Tcutil_copye(_tmpB50,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->tag=18U,_tmp560->f1=_tmp55F;_tmp560;}));});}case 41U: _LL1B: _tmp57D=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_LL1C: {
# 3331
struct Cyc_Absyn_Exp*_tmp561=Cyc_Tcutil_rsubsexp(r,inst,_tmp57D);
if(_tmp561 == _tmp57D)return e;
return({struct Cyc_Absyn_Exp*_tmpB51=e;Cyc_Tcutil_copye(_tmpB51,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->tag=41U,_tmp562->f1=_tmp561;_tmp562;}));});}case 19U: _LL1D: _tmp57F=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_tmp57E=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp540)->f2;_LL1E: {
# 3335
void*_tmp563=Cyc_Tcutil_rsubstitute(r,inst,_tmp57F);
if(_tmp563 == _tmp57F)return e;
return({struct Cyc_Absyn_Exp*_tmpB52=e;Cyc_Tcutil_copye(_tmpB52,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->tag=19U,_tmp564->f1=_tmp563,_tmp564->f2=_tmp57E;_tmp564;}));});}case 39U: _LL1F: _tmp580=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp540)->f1;_LL20: {
# 3339
void*_tmp565=Cyc_Tcutil_rsubstitute(r,inst,_tmp580);
if(_tmp565 == _tmp580)return e;{
# 3342
void*_tmp566=Cyc_Tcutil_compress(_tmp565);void*_tmp567=_tmp566;struct Cyc_Absyn_Exp*_tmp569;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp567)->tag == 9U){_LL24: _tmp569=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp567)->f1;_LL25:
 return _tmp569;}else{_LL26: _LL27:
# 3345
 return({struct Cyc_Absyn_Exp*_tmpB53=e;Cyc_Tcutil_copye(_tmpB53,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->tag=39U,_tmp568->f1=_tmp565;_tmp568;}));});}_LL23:;};}default: _LL21: _LL22:
# 3348
 return({void*_tmp56A=0U;({struct _dyneither_ptr _tmpB54=({const char*_tmp56B="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp56B,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB54,_tag_dyneither(_tmp56A,sizeof(void*),0U));});});}_LL0:;}
# 3352
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3355
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3359
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3363
void*_tmp581=f->type;
struct Cyc_Absyn_Exp*_tmp582=f->requires_clause;
void*_tmp583=Cyc_Tcutil_rsubstitute(r,inst,_tmp581);
struct Cyc_Absyn_Exp*_tmp584=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp582);
if(_tmp581 == _tmp583  && _tmp582 == _tmp584)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->name=f->name,_tmp585->tq=f->tq,_tmp585->type=_tmp583,_tmp585->width=f->width,_tmp585->attributes=f->attributes,_tmp585->requires_clause=_tmp584;_tmp585;});}}
# 3373
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3376
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp586=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp587=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp588=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp586);
struct Cyc_List_List*_tmp589=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp587);
if(_tmp588 == _tmp586  && _tmp589 == _tmp587)return fs;
return({struct Cyc_List_List*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->hd=_tmp588,_tmp58A->tl=_tmp589;_tmp58A;});};}
# 3385
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3388
struct _tuple1 _tmp58B=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp58C=_tmp58B;struct Cyc_List_List*_tmp590;struct Cyc_List_List*_tmp58F;_LL1: _tmp590=_tmp58C.f1;_tmp58F=_tmp58C.f2;_LL2:;{
struct Cyc_List_List*_tmp58D=Cyc_Tcutil_substs(rgn,inst,_tmp590);
struct Cyc_List_List*_tmp58E=Cyc_Tcutil_substs(rgn,inst,_tmp58F);
if(_tmp58D == _tmp590  && _tmp58E == _tmp58F)
return rgn_po;else{
# 3394
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp58D,_tmp58E);}};}
# 3397
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3400
void*_tmp591=Cyc_Tcutil_compress(t);void*_tmp592=_tmp591;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5E9;void*_tmp5E8;struct Cyc_List_List*_tmp5E7;void*_tmp5E6;void*_tmp5E5;enum Cyc_Absyn_AggrKind _tmp5E4;struct Cyc_List_List*_tmp5E3;struct Cyc_List_List*_tmp5E2;struct Cyc_List_List*_tmp5E1;void*_tmp5E0;struct Cyc_Absyn_Tqual _tmp5DF;void*_tmp5DE;struct Cyc_List_List*_tmp5DD;int _tmp5DC;struct Cyc_Absyn_VarargInfo*_tmp5DB;struct Cyc_List_List*_tmp5DA;struct Cyc_List_List*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D7;void*_tmp5D6;struct Cyc_Absyn_Tqual _tmp5D5;void*_tmp5D4;void*_tmp5D3;void*_tmp5D2;void*_tmp5D1;void*_tmp5D0;struct Cyc_Absyn_Tqual _tmp5CF;struct Cyc_Absyn_Exp*_tmp5CE;void*_tmp5CD;unsigned int _tmp5CC;struct _tuple2*_tmp5CB;struct Cyc_List_List*_tmp5CA;struct Cyc_Absyn_Typedefdecl*_tmp5C9;void*_tmp5C8;struct Cyc_Absyn_Tvar*_tmp5C7;switch(*((int*)_tmp592)){case 2U: _LL1: _tmp5C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp592)->f1;_LL2: {
# 3403
struct _handler_cons _tmp593;_push_handler(& _tmp593);{int _tmp595=0;if(setjmp(_tmp593.handler))_tmp595=1;if(!_tmp595){{void*_tmp596=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp5C7);_npop_handler(0U);return _tmp596;};_pop_handler();}else{void*_tmp594=(void*)_exn_thrown;void*_tmp597=_tmp594;void*_tmp598;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp597)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp598=_tmp597;_LL1F:(int)_rethrow(_tmp598);}_LL1B:;}};}case 8U: _LL3: _tmp5CB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp592)->f1;_tmp5CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp592)->f2;_tmp5C9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp592)->f3;_tmp5C8=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp592)->f4;_LL4: {
# 3406
struct Cyc_List_List*_tmp599=Cyc_Tcutil_substs(rgn,inst,_tmp5CA);
return _tmp599 == _tmp5CA?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->tag=8U,_tmp59A->f1=_tmp5CB,_tmp59A->f2=_tmp599,_tmp59A->f3=_tmp5C9,_tmp59A->f4=_tmp5C8;_tmp59A;});}case 4U: _LL5: _tmp5D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp592)->f1).elt_type;_tmp5CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp592)->f1).tq;_tmp5CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp592)->f1).num_elts;_tmp5CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp592)->f1).zero_term;_tmp5CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp592)->f1).zt_loc;_LL6: {
# 3409
void*_tmp59B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D0);
struct Cyc_Absyn_Exp*_tmp59C=_tmp5CE == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5CE);
void*_tmp59D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5CD);
return(_tmp59B == _tmp5D0  && _tmp59C == _tmp5CE) && _tmp59D == _tmp5CD?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp59E=_cycalloc(sizeof(*_tmp59E));
_tmp59E->tag=4U,(_tmp59E->f1).elt_type=_tmp59B,(_tmp59E->f1).tq=_tmp5CF,(_tmp59E->f1).num_elts=_tmp59C,(_tmp59E->f1).zero_term=_tmp59D,(_tmp59E->f1).zt_loc=_tmp5CC;_tmp59E;});}case 3U: _LL7: _tmp5D6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp592)->f1).elt_type;_tmp5D5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp592)->f1).elt_tq;_tmp5D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp592)->f1).ptr_atts).rgn;_tmp5D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp592)->f1).ptr_atts).nullable;_tmp5D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp592)->f1).ptr_atts).bounds;_tmp5D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp592)->f1).ptr_atts).zero_term;_LL8: {
# 3415
void*_tmp59F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D6);
void*_tmp5A0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D4);
void*_tmp5A1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D2);
void*_tmp5A2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D1);
if(((_tmp59F == _tmp5D6  && _tmp5A0 == _tmp5D4) && _tmp5A1 == _tmp5D2) && _tmp5A2 == _tmp5D1)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp9BC;_tmp9BC.elt_type=_tmp59F,_tmp9BC.elt_tq=_tmp5D5,(_tmp9BC.ptr_atts).rgn=_tmp5A0,(_tmp9BC.ptr_atts).nullable=_tmp5D3,(_tmp9BC.ptr_atts).bounds=_tmp5A1,(_tmp9BC.ptr_atts).zero_term=_tmp5A2,(_tmp9BC.ptr_atts).ptrloc=0;_tmp9BC;}));}case 5U: _LL9: _tmp5E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).tvars;_tmp5E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).effect;_tmp5DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).ret_tqual;_tmp5DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).ret_type;_tmp5DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).args;_tmp5DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).c_varargs;_tmp5DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).cyc_varargs;_tmp5DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).rgn_po;_tmp5D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).attributes;_tmp5D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).requires_clause;_tmp5D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592)->f1).ensures_clause;_LLA:
# 3425
{struct Cyc_List_List*_tmp5A3=_tmp5E1;for(0;_tmp5A3 != 0;_tmp5A3=_tmp5A3->tl){
inst=({struct Cyc_List_List*_tmp5A5=_region_malloc(rgn,sizeof(*_tmp5A5));({struct _tuple15*_tmpB56=({struct _tuple15*_tmp5A4=_region_malloc(rgn,sizeof(*_tmp5A4));_tmp5A4->f1=(struct Cyc_Absyn_Tvar*)_tmp5A3->hd,({void*_tmpB55=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp5A3->hd);_tmp5A4->f2=_tmpB55;});_tmp5A4;});_tmp5A5->hd=_tmpB56;}),_tmp5A5->tl=inst;_tmp5A5;});}}{
struct _tuple1 _tmp5A6=({struct _RegionHandle*_tmpB58=rgn;struct _RegionHandle*_tmpB57=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpB58,_tmpB57,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple28*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp5DD));});
# 3427
struct _tuple1 _tmp5A7=_tmp5A6;struct Cyc_List_List*_tmp5B6;struct Cyc_List_List*_tmp5B5;_LL21: _tmp5B6=_tmp5A7.f1;_tmp5B5=_tmp5A7.f2;_LL22:;{
# 3429
struct Cyc_List_List*_tmp5A8=_tmp5DD;
struct Cyc_List_List*_tmp5A9=Cyc_Tcutil_substs(rgn,inst,_tmp5B5);
if(_tmp5A9 != _tmp5B5)
_tmp5A8=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp5DD,_tmp5A9);{
void*eff2;
if(_tmp5E0 == 0)
eff2=0;else{
# 3437
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E0);
if(new_eff == _tmp5E0)
eff2=_tmp5E0;else{
# 3441
eff2=new_eff;}}{
# 3443
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp5DB == 0)
cyc_varargs2=0;else{
# 3447
struct Cyc_Absyn_VarargInfo _tmp5AA=*_tmp5DB;struct Cyc_Absyn_VarargInfo _tmp5AB=_tmp5AA;struct _dyneither_ptr*_tmp5B1;struct Cyc_Absyn_Tqual _tmp5B0;void*_tmp5AF;int _tmp5AE;_LL24: _tmp5B1=_tmp5AB.name;_tmp5B0=_tmp5AB.tq;_tmp5AF=_tmp5AB.type;_tmp5AE=_tmp5AB.inject;_LL25:;{
void*_tmp5AC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5AF);
if(_tmp5AC == _tmp5AF)cyc_varargs2=_tmp5DB;else{
# 3451
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->name=_tmp5B1,_tmp5AD->tq=_tmp5B0,_tmp5AD->type=_tmp5AC,_tmp5AD->inject=_tmp5AE;_tmp5AD;});}};}{
# 3453
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5DA);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5D8);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5D7);
struct Cyc_List_List*_tmp5B2=Cyc_Tcutil_extract_relns(_tmp5A8,req2);
struct Cyc_List_List*_tmp5B3=Cyc_Tcutil_extract_relns(_tmp5A8,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));
_tmp5B4->tag=5U,(_tmp5B4->f1).tvars=_tmp5E1,(_tmp5B4->f1).effect=eff2,(_tmp5B4->f1).ret_tqual=_tmp5DF,({void*_tmpB59=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5DE);(_tmp5B4->f1).ret_type=_tmpB59;}),(_tmp5B4->f1).args=_tmp5A8,(_tmp5B4->f1).c_varargs=_tmp5DC,(_tmp5B4->f1).cyc_varargs=cyc_varargs2,(_tmp5B4->f1).rgn_po=rgn_po2,(_tmp5B4->f1).attributes=_tmp5D9,(_tmp5B4->f1).requires_clause=req2,(_tmp5B4->f1).requires_relns=_tmp5B2,(_tmp5B4->f1).ensures_clause=ens2,(_tmp5B4->f1).ensures_relns=_tmp5B3;_tmp5B4;});};};};};};case 6U: _LLB: _tmp5E2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp592)->f1;_LLC: {
# 3463
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp5B7=_tmp5E2;for(0;_tmp5B7 != 0;_tmp5B7=_tmp5B7->tl){
void*_tmp5B8=(*((struct _tuple12*)_tmp5B7->hd)).f2;
void*_tmp5B9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5B8);
if(_tmp5B8 != _tmp5B9)
change=1;
# 3471
ts2=({struct Cyc_List_List*_tmp5BA=_region_malloc(rgn,sizeof(*_tmp5BA));_tmp5BA->hd=_tmp5B9,_tmp5BA->tl=ts2;_tmp5BA;});}}
# 3473
if(!change)
return t;{
struct Cyc_List_List*_tmp5BB=({struct Cyc_List_List*_tmpB5A=_tmp5E2;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpB5A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->tag=6U,_tmp5BC->f1=_tmp5BB;_tmp5BC;});};}case 7U: _LLD: _tmp5E4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp592)->f1;_tmp5E3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp592)->f2;_LLE: {
# 3478
struct Cyc_List_List*_tmp5BD=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp5E3);
if(_tmp5E3 == _tmp5BD)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->tag=7U,_tmp5BE->f1=_tmp5E4,_tmp5BE->f2=_tmp5BD;_tmp5BE;});}case 1U: _LLF: _tmp5E5=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp592)->f2;_LL10:
# 3482
 if(_tmp5E5 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E5);else{
return t;}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp592)->f2 == 0){_LL11: _tmp5E6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp592)->f1;_LL12:
 return t;}else{_LL13: _tmp5E8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp592)->f1;_tmp5E7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp592)->f2;_LL14: {
# 3486
struct Cyc_List_List*_tmp5BF=Cyc_Tcutil_substs(rgn,inst,_tmp5E7);
if(_tmp5E7 == _tmp5BF)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->tag=0U,_tmp5C0->f1=_tmp5E8,_tmp5C0->f2=_tmp5BF;_tmp5C0;});}}}case 9U: _LL15: _tmp5E9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp592)->f1;_LL16: {
# 3490
struct Cyc_Absyn_Exp*_tmp5C1=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5E9);
return _tmp5C1 == _tmp5E9?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->tag=9U,_tmp5C2->f1=_tmp5C1;_tmp5C2;});}case 11U: _LL17: _tmp5EA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp592)->f1;_LL18: {
# 3493
struct Cyc_Absyn_Exp*_tmp5C3=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5EA);
return _tmp5C3 == _tmp5EA?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4->tag=11U,_tmp5C4->f1=_tmp5C3;_tmp5C4;});}default: _LL19: _LL1A:
({void*_tmp5C5=0U;({struct _dyneither_ptr _tmpB5B=({const char*_tmp5C6="found typedecltype in rsubs";_tag_dyneither(_tmp5C6,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB5B,_tag_dyneither(_tmp5C5,sizeof(void*),0U));});});}_LL0:;}
# 3499
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3502
if(ts == 0)
return 0;{
void*_tmp5EB=(void*)ts->hd;
struct Cyc_List_List*_tmp5EC=ts->tl;
void*_tmp5ED=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5EB);
struct Cyc_List_List*_tmp5EE=Cyc_Tcutil_substs(rgn,inst,_tmp5EC);
if(_tmp5EB == _tmp5ED  && _tmp5EC == _tmp5EE)
return ts;
return({struct Cyc_List_List*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->hd=_tmp5ED,_tmp5EF->tl=_tmp5EE;_tmp5EF;});};}
# 3513
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3520
struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp5F0=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->f1=tv,({void*_tmpB5D=({struct Cyc_Core_Opt*_tmpB5C=_tmp5F0;Cyc_Absyn_new_evar(_tmpB5C,({struct Cyc_Core_Opt*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->v=s;_tmp5F1;}));});_tmp5F2->f2=_tmpB5D;});_tmp5F2;});}
# 3525
struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,struct Cyc_Absyn_Tvar*tv){
# 3527
struct _tuple16*_tmp5F3=env;struct Cyc_List_List*_tmp5F8;struct _RegionHandle*_tmp5F7;_LL1: _tmp5F8=_tmp5F3->f1;_tmp5F7=_tmp5F3->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp5F4=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5F6=_region_malloc(_tmp5F7,sizeof(*_tmp5F6));_tmp5F6->f1=tv,({void*_tmpB5F=({struct Cyc_Core_Opt*_tmpB5E=_tmp5F4;Cyc_Absyn_new_evar(_tmpB5E,({struct Cyc_Core_Opt*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->v=_tmp5F8;_tmp5F5;}));});_tmp5F6->f2=_tmpB5F;});_tmp5F6;});};}
# 3537
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3541
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp5FB=({struct Cyc_String_pa_PrintArg_struct _tmp9BF;_tmp9BF.tag=0U,_tmp9BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmp9BF;});struct Cyc_String_pa_PrintArg_struct _tmp5FC=({struct Cyc_String_pa_PrintArg_struct _tmp9BE;_tmp9BE.tag=0U,({
struct _dyneither_ptr _tmpB60=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmp9BE.f1=_tmpB60;});_tmp9BE;});struct Cyc_String_pa_PrintArg_struct _tmp5FD=({struct Cyc_String_pa_PrintArg_struct _tmp9BD;_tmp9BD.tag=0U,({struct _dyneither_ptr _tmpB61=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));_tmp9BD.f1=_tmpB61;});_tmp9BD;});void*_tmp5F9[3U];_tmp5F9[0]=& _tmp5FB,_tmp5F9[1]=& _tmp5FC,_tmp5F9[2]=& _tmp5FD;({unsigned int _tmpB63=loc;struct _dyneither_ptr _tmpB62=({const char*_tmp5FA="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp5FA,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpB63,_tmpB62,_tag_dyneither(_tmp5F9,sizeof(void*),3U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp5FE=0U;({struct _dyneither_ptr _tmpB64=({const char*_tmp5FF="same type variable has different identity!";_tag_dyneither(_tmp5FF,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB64,_tag_dyneither(_tmp5FE,sizeof(void*),0U));});});}
return tvs;}}}
# 3555
({int _tmpB65=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpB65;});
return({struct Cyc_List_List*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->hd=tv,_tmp600->tl=tvs;_tmp600;});}
# 3561
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3563
if(tv->identity == - 1)
({void*_tmp601=0U;({struct _dyneither_ptr _tmpB66=({const char*_tmp602="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp602,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB66,_tag_dyneither(_tmp601,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3567
struct Cyc_Absyn_Tvar*_tmp603=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp603->identity == - 1)
({void*_tmp604=0U;({struct _dyneither_ptr _tmpB67=({const char*_tmp605="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp605,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB67,_tag_dyneither(_tmp604,sizeof(void*),0U));});});
if(_tmp603->identity == tv->identity)
return tvs;}}
# 3574
return({struct Cyc_List_List*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->hd=tv,_tmp606->tl=tvs;_tmp606;});}struct _tuple29{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3580
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3585
if(tv->identity == - 1)
({void*_tmp607=0U;({struct _dyneither_ptr _tmpB68=({const char*_tmp608="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp608,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB68,_tag_dyneither(_tmp607,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3589
struct _tuple29*_tmp609=(struct _tuple29*)tvs2->hd;struct _tuple29*_tmp60A=_tmp609;struct Cyc_Absyn_Tvar*_tmp60E;int*_tmp60D;_LL1: _tmp60E=_tmp60A->f1;_tmp60D=(int*)& _tmp60A->f2;_LL2:;
if(_tmp60E->identity == - 1)
({void*_tmp60B=0U;({struct _dyneither_ptr _tmpB69=({const char*_tmp60C="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp60C,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB69,_tag_dyneither(_tmp60B,sizeof(void*),0U));});});
if(_tmp60E->identity == tv->identity){
*_tmp60D=*_tmp60D  || b;
return tvs;}}}
# 3597
return({struct Cyc_List_List*_tmp610=_region_malloc(r,sizeof(*_tmp610));({struct _tuple29*_tmpB6A=({struct _tuple29*_tmp60F=_region_malloc(r,sizeof(*_tmp60F));_tmp60F->f1=tv,_tmp60F->f2=b;_tmp60F;});_tmp610->hd=_tmpB6A;}),_tmp610->tl=tvs;_tmp610;});}
# 3601
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3603
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp613=({struct Cyc_String_pa_PrintArg_struct _tmp9C0;_tmp9C0.tag=0U,({struct _dyneither_ptr _tmpB6B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmp9C0.f1=_tmpB6B;});_tmp9C0;});void*_tmp611[1U];_tmp611[0]=& _tmp613;({struct _dyneither_ptr _tmpB6C=({const char*_tmp612="bound tvar id for %s is NULL";_tag_dyneither(_tmp612,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB6C,_tag_dyneither(_tmp611,sizeof(void*),1U));});});
return({struct Cyc_List_List*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->hd=tv,_tmp614->tl=tvs;_tmp614;});}struct _tuple30{void*f1;int f2;};
# 3612
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3615
void*_tmp615=Cyc_Tcutil_compress(e);void*_tmp616=_tmp615;int _tmp620;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp616)->tag == 1U){_LL1: _tmp620=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp616)->f3;_LL2:
# 3617
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple30*_tmp617=(struct _tuple30*)es2->hd;struct _tuple30*_tmp618=_tmp617;void*_tmp61D;int*_tmp61C;_LL6: _tmp61D=_tmp618->f1;_tmp61C=(int*)& _tmp618->f2;_LL7:;{
void*_tmp619=Cyc_Tcutil_compress(_tmp61D);void*_tmp61A=_tmp619;int _tmp61B;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61A)->tag == 1U){_LL9: _tmp61B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61A)->f3;_LLA:
# 3621
 if(_tmp620 == _tmp61B){
if(b != *_tmp61C)*_tmp61C=1;
return es;}
# 3625
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3629
return({struct Cyc_List_List*_tmp61F=_region_malloc(r,sizeof(*_tmp61F));({struct _tuple30*_tmpB6D=({struct _tuple30*_tmp61E=_region_malloc(r,sizeof(*_tmp61E));_tmp61E->f1=e,_tmp61E->f2=b;_tmp61E;});_tmp61F->hd=_tmpB6D;}),_tmp61F->tl=es;_tmp61F;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3634
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3637
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3646
if(!present)r=({struct Cyc_List_List*_tmp621=_region_malloc(rgn,sizeof(*_tmp621));_tmp621->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp621->tl=r;_tmp621;});}
# 3648
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3653
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3657
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple29 _tmp622=*((struct _tuple29*)tvs->hd);struct _tuple29 _tmp623=_tmp622;struct Cyc_Absyn_Tvar*_tmp626;int _tmp625;_LL1: _tmp626=_tmp623.f1;_tmp625=_tmp623.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp626->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3667
if(!present)res=({struct Cyc_List_List*_tmp624=_region_malloc(r,sizeof(*_tmp624));_tmp624->hd=(struct _tuple29*)tvs->hd,_tmp624->tl=res;_tmp624;});};}
# 3669
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3673
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_type,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3675
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp629=({struct Cyc_String_pa_PrintArg_struct _tmp9C1;_tmp9C1.tag=0U,_tmp9C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9C1;});void*_tmp627[1U];_tmp627[0]=& _tmp629;({unsigned int _tmpB6F=loc;struct _dyneither_ptr _tmpB6E=({const char*_tmp628="bitfield %s does not have constant width";_tag_dyneither(_tmp628,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpB6F,_tmpB6E,_tag_dyneither(_tmp627,sizeof(void*),1U));});});else{
# 3680
struct _tuple13 _tmp62A=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _tmp62B=_tmp62A;unsigned int _tmp631;int _tmp630;_LL1: _tmp631=_tmp62B.f1;_tmp630=_tmp62B.f2;_LL2:;
if(!_tmp630)
({void*_tmp62C=0U;({unsigned int _tmpB71=loc;struct _dyneither_ptr _tmpB70=({const char*_tmp62D="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp62D,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpB71,_tmpB70,_tag_dyneither(_tmp62C,sizeof(void*),0U));});});
if((int)_tmp631 < 0)
({void*_tmp62E=0U;({unsigned int _tmpB73=loc;struct _dyneither_ptr _tmpB72=({const char*_tmp62F="bitfield has negative width";_tag_dyneither(_tmp62F,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpB73,_tmpB72,_tag_dyneither(_tmp62E,sizeof(void*),0U));});});
w=_tmp631;}{
# 3687
void*_tmp632=Cyc_Tcutil_compress(field_type);void*_tmp633=_tmp632;enum Cyc_Absyn_Size_of _tmp641;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp633)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp633)->f1)->tag == 1U){_LL4: _tmp641=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp633)->f1)->f2;_LL5:
# 3690
{enum Cyc_Absyn_Size_of _tmp634=_tmp641;switch(_tmp634){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned int)8)({void*_tmp635=0U;({unsigned int _tmpB75=loc;struct _dyneither_ptr _tmpB74=({const char*_tmp636="bitfield larger than type";_tag_dyneither(_tmp636,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB75,_tmpB74,_tag_dyneither(_tmp635,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned int)16)({void*_tmp637=0U;({unsigned int _tmpB77=loc;struct _dyneither_ptr _tmpB76=({const char*_tmp638="bitfield larger than type";_tag_dyneither(_tmp638,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB77,_tmpB76,_tag_dyneither(_tmp637,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 3695
 if(w > (unsigned int)32)({void*_tmp639=0U;({unsigned int _tmpB79=loc;struct _dyneither_ptr _tmpB78=({const char*_tmp63A="bitfield larger than type";_tag_dyneither(_tmp63A,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB79,_tmpB78,_tag_dyneither(_tmp639,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
# 3698
 if(w > (unsigned int)64)({void*_tmp63B=0U;({unsigned int _tmpB7B=loc;struct _dyneither_ptr _tmpB7A=({const char*_tmp63C="bitfield larger than type";_tag_dyneither(_tmp63C,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB7B,_tmpB7A,_tag_dyneither(_tmp63B,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3700
goto _LL3;}else{goto _LL6;}}else{_LL6: _LL7:
# 3702
({struct Cyc_String_pa_PrintArg_struct _tmp63F=({struct Cyc_String_pa_PrintArg_struct _tmp9C3;_tmp9C3.tag=0U,_tmp9C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9C3;});struct Cyc_String_pa_PrintArg_struct _tmp640=({struct Cyc_String_pa_PrintArg_struct _tmp9C2;_tmp9C2.tag=0U,({
struct _dyneither_ptr _tmpB7C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_type));_tmp9C2.f1=_tmpB7C;});_tmp9C2;});void*_tmp63D[2U];_tmp63D[0]=& _tmp63F,_tmp63D[1]=& _tmp640;({unsigned int _tmpB7E=loc;struct _dyneither_ptr _tmpB7D=({const char*_tmp63E="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp63E,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpB7E,_tmpB7D,_tag_dyneither(_tmp63D,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 3709
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp642=(void*)atts->hd;void*_tmp643=_tmp642;switch(*((int*)_tmp643)){case 7U: _LL1: _LL2:
 continue;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp646=({struct Cyc_String_pa_PrintArg_struct _tmp9C5;_tmp9C5.tag=0U,({
struct _dyneither_ptr _tmpB7F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp9C5.f1=_tmpB7F;});_tmp9C5;});struct Cyc_String_pa_PrintArg_struct _tmp647=({struct Cyc_String_pa_PrintArg_struct _tmp9C4;_tmp9C4.tag=0U,_tmp9C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9C4;});void*_tmp644[2U];_tmp644[0]=& _tmp646,_tmp644[1]=& _tmp647;({unsigned int _tmpB81=loc;struct _dyneither_ptr _tmpB80=({const char*_tmp645="bad attribute %s on member %s";_tag_dyneither(_tmp645,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpB81,_tmpB80,_tag_dyneither(_tmp644,sizeof(void*),2U));});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3737
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp648=t;struct Cyc_Absyn_Typedefdecl*_tmp64C;void*_tmp64B;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp648)->tag == 8U){_LL1: _tmp64C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp648)->f3;_tmp64B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp648)->f4;_LL2:
# 3740
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp64C))->tq).real_const  || (_tmp64C->tq).print_const){
if(declared_const)({void*_tmp649=0U;({unsigned int _tmpB83=loc;struct _dyneither_ptr _tmpB82=({const char*_tmp64A="extra const";_tag_dyneither(_tmp64A,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpB83,_tmpB82,_tag_dyneither(_tmp649,sizeof(void*),0U));});});
return 1;}
# 3745
if((unsigned int)_tmp64B)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp64B);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3752
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp64D=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp64E=_tmp64D;void*_tmp652;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64E)->tag == 3U){_LL1: _tmp652=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64E)->f1).ptr_atts).rgn;_LL2:
# 3757
{void*_tmp64F=Cyc_Tcutil_compress(_tmp652);void*_tmp650=_tmp64F;struct Cyc_Absyn_Tvar*_tmp651;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp650)->tag == 2U){_LL6: _tmp651=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp650)->f1;_LL7:
# 3759
 return Cyc_Absyn_tvar_cmp(tvar,_tmp651)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3762
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3768
return 1;}
return 0;}
# 3772
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3775
void*_tmp653=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp654=_tmp653;switch(*((int*)_tmp654)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp654)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp654)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp654)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp654)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3784
 if((((int)expected_kind->kind == (int)Cyc_Absyn_BoxKind  || (int)expected_kind->kind == (int)Cyc_Absyn_MemKind) || (int)expected_kind->kind == (int)Cyc_Absyn_AnyKind) && 
# 3787
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3793
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3799
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple31{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3803
static struct _tuple31 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3806
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp657=({struct Cyc_String_pa_PrintArg_struct _tmp9C7;_tmp9C7.tag=0U,_tmp9C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9C7;});struct Cyc_String_pa_PrintArg_struct _tmp658=({struct Cyc_String_pa_PrintArg_struct _tmp9C6;_tmp9C6.tag=0U,({
struct _dyneither_ptr _tmpB84=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmp9C6.f1=_tmpB84;});_tmp9C6;});void*_tmp655[2U];_tmp655[0]=& _tmp657,_tmp655[1]=& _tmp658;({unsigned int _tmpB86=loc;struct _dyneither_ptr _tmpB85=({const char*_tmp656="%s clause has type %s instead of integral type";_tag_dyneither(_tmp656,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpB86,_tmpB85,_tag_dyneither(_tmp655,sizeof(void*),2U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp65B=({struct Cyc_String_pa_PrintArg_struct _tmp9C9;_tmp9C9.tag=0U,_tmp9C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9C9;});struct Cyc_String_pa_PrintArg_struct _tmp65C=({struct Cyc_String_pa_PrintArg_struct _tmp9C8;_tmp9C8.tag=0U,({
struct _dyneither_ptr _tmpB87=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(clause));_tmp9C8.f1=_tmpB87;});_tmp9C8;});void*_tmp659[2U];_tmp659[0]=& _tmp65B,_tmp659[1]=& _tmp65C;({unsigned int _tmpB89=clause->loc;struct _dyneither_ptr _tmpB88=({const char*_tmp65A="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp65A,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpB89,_tmpB88,_tag_dyneither(_tmp659,sizeof(void*),2U));});});}
# 3818
return({struct _tuple31 _tmp9CA;_tmp9CA.f1=cvtenv,_tmp9CA.f2=relns;_tmp9CA;});}
# 3848 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr);
# 3855
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_aggr(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 3861
{union Cyc_Absyn_AggrInfo _tmp65D=*info;union Cyc_Absyn_AggrInfo _tmp65E=_tmp65D;struct Cyc_Absyn_Aggrdecl*_tmp681;enum Cyc_Absyn_AggrKind _tmp680;struct _tuple2*_tmp67F;struct Cyc_Core_Opt*_tmp67E;if((_tmp65E.UnknownAggr).tag == 1){_LL1: _tmp680=((_tmp65E.UnknownAggr).val).f1;_tmp67F=((_tmp65E.UnknownAggr).val).f2;_tmp67E=((_tmp65E.UnknownAggr).val).f3;_LL2: {
# 3863
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp65F;_push_handler(& _tmp65F);{int _tmp661=0;if(setjmp(_tmp65F.handler))_tmp661=1;if(!_tmp661){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp67F);{
struct Cyc_Absyn_Aggrdecl*_tmp662=*adp;
if((int)_tmp662->kind != (int)_tmp680){
if((int)_tmp662->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp665=({struct Cyc_String_pa_PrintArg_struct _tmp9CC;_tmp9CC.tag=0U,({
struct _dyneither_ptr _tmpB8A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67F));_tmp9CC.f1=_tmpB8A;});_tmp9CC;});struct Cyc_String_pa_PrintArg_struct _tmp666=({struct Cyc_String_pa_PrintArg_struct _tmp9CB;_tmp9CB.tag=0U,({struct _dyneither_ptr _tmpB8B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67F));_tmp9CB.f1=_tmpB8B;});_tmp9CB;});void*_tmp663[2U];_tmp663[0]=& _tmp665,_tmp663[1]=& _tmp666;({unsigned int _tmpB8D=loc;struct _dyneither_ptr _tmpB8C=({const char*_tmp664="expecting struct %s instead of union %s";_tag_dyneither(_tmp664,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpB8D,_tmpB8C,_tag_dyneither(_tmp663,sizeof(void*),2U));});});else{
# 3872
({struct Cyc_String_pa_PrintArg_struct _tmp669=({struct Cyc_String_pa_PrintArg_struct _tmp9CE;_tmp9CE.tag=0U,({
struct _dyneither_ptr _tmpB8E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67F));_tmp9CE.f1=_tmpB8E;});_tmp9CE;});struct Cyc_String_pa_PrintArg_struct _tmp66A=({struct Cyc_String_pa_PrintArg_struct _tmp9CD;_tmp9CD.tag=0U,({struct _dyneither_ptr _tmpB8F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67F));_tmp9CD.f1=_tmpB8F;});_tmp9CD;});void*_tmp667[2U];_tmp667[0]=& _tmp669,_tmp667[1]=& _tmp66A;({unsigned int _tmpB91=loc;struct _dyneither_ptr _tmpB90=({const char*_tmp668="expecting union %s instead of struct %s";_tag_dyneither(_tmp668,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpB91,_tmpB90,_tag_dyneither(_tmp667,sizeof(void*),2U));});});}}
# 3875
if((unsigned int)_tmp67E  && (int)_tmp67E->v){
if(!((unsigned int)_tmp662->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp662->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp66D=({struct Cyc_String_pa_PrintArg_struct _tmp9CF;_tmp9CF.tag=0U,({
struct _dyneither_ptr _tmpB92=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67F));_tmp9CF.f1=_tmpB92;});_tmp9CF;});void*_tmp66B[1U];_tmp66B[0]=& _tmp66D;({unsigned int _tmpB94=loc;struct _dyneither_ptr _tmpB93=({const char*_tmp66C="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp66C,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpB94,_tmpB93,_tag_dyneither(_tmp66B,sizeof(void*),1U));});});}
# 3881
({union Cyc_Absyn_AggrInfo _tmpB95=Cyc_Absyn_KnownAggr(adp);*info=_tmpB95;});};
# 3865
;_pop_handler();}else{void*_tmp660=(void*)_exn_thrown;void*_tmp66E=_tmp660;void*_tmp674;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp66E)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3885
struct Cyc_Absyn_Aggrdecl*_tmp66F=({struct Cyc_Absyn_Aggrdecl*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->kind=_tmp680,_tmp673->sc=Cyc_Absyn_Extern,_tmp673->name=_tmp67F,_tmp673->tvs=0,_tmp673->impl=0,_tmp673->attributes=0,_tmp673->expected_mem_kind=0;_tmp673;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp66F);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp67F);
({union Cyc_Absyn_AggrInfo _tmpB96=Cyc_Absyn_KnownAggr(adp);*info=_tmpB96;});
# 3890
if(*targs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp672=({struct Cyc_String_pa_PrintArg_struct _tmp9D0;_tmp9D0.tag=0U,({struct _dyneither_ptr _tmpB97=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67F));_tmp9D0.f1=_tmpB97;});_tmp9D0;});void*_tmp670[1U];_tmp670[0]=& _tmp672;({unsigned int _tmpB99=loc;struct _dyneither_ptr _tmpB98=({const char*_tmp671="declare parameterized type %s before using";_tag_dyneither(_tmp671,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpB99,_tmpB98,_tag_dyneither(_tmp670,sizeof(void*),1U));});});
return cvtenv;}
# 3894
goto _LL5;}}else{_LL8: _tmp674=_tmp66E;_LL9:(int)_rethrow(_tmp674);}_LL5:;}};}
# 3896
_tmp681=*adp;goto _LL4;}}else{_LL3: _tmp681=*(_tmp65E.KnownAggr).val;_LL4: {
# 3898
struct Cyc_List_List*tvs=_tmp681->tvs;
struct Cyc_List_List*ts=*targs;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp675=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp676=(void*)ts->hd;
# 3906
{struct _tuple0 _tmp677=({struct _tuple0 _tmp9D1;({void*_tmpB9A=Cyc_Absyn_compress_kb(_tmp675->kind);_tmp9D1.f1=_tmpB9A;}),_tmp9D1.f2=_tmp676;_tmp9D1;});struct _tuple0 _tmp678=_tmp677;struct Cyc_Absyn_Tvar*_tmp679;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp678.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp678.f2)->tag == 2U){_LLB: _tmp679=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp678.f2)->f1;_LLC:
# 3908
({struct Cyc_List_List*_tmpB9B=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp679);cvtenv.kind_env=_tmpB9B;});
({struct Cyc_List_List*_tmpB9C=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp679,1);cvtenv.free_vars=_tmpB9C;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 3913
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 3917
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp67C=({struct Cyc_String_pa_PrintArg_struct _tmp9D2;_tmp9D2.tag=0U,({struct _dyneither_ptr _tmpB9D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp681->name));_tmp9D2.f1=_tmpB9D;});_tmp9D2;});void*_tmp67A[1U];_tmp67A[0]=& _tmp67C;({unsigned int _tmpB9F=loc;struct _dyneither_ptr _tmpB9E=({const char*_tmp67B="too many parameters for type %s";_tag_dyneither(_tmp67B,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpB9F,_tmpB9E,_tag_dyneither(_tmp67A,sizeof(void*),1U));});});
if(tvs != 0){
# 3921
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->hd=e,_tmp67D->tl=hidden_ts;_tmp67D;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 3928
({struct Cyc_List_List*_tmpBA1=({struct Cyc_List_List*_tmpBA0=*targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpBA0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targs=_tmpBA1;});}
# 3930
if((allow_abs_aggr  && _tmp681->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 3935
_tmp681->expected_mem_kind=1;}}_LL0:;}
# 3938
return cvtenv;}
# 3942
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 3948
struct Cyc_List_List*_tmp682=*targsp;
{union Cyc_Absyn_DatatypeInfo _tmp683=*info;union Cyc_Absyn_DatatypeInfo _tmp684=_tmp683;struct Cyc_Absyn_Datatypedecl*_tmp69B;struct _tuple2*_tmp69A;int _tmp699;if((_tmp684.UnknownDatatype).tag == 1){_LL1: _tmp69A=((_tmp684.UnknownDatatype).val).name;_tmp699=((_tmp684.UnknownDatatype).val).is_extensible;_LL2: {
# 3951
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp685;_push_handler(& _tmp685);{int _tmp687=0;if(setjmp(_tmp685.handler))_tmp687=1;if(!_tmp687){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp69A);;_pop_handler();}else{void*_tmp686=(void*)_exn_thrown;void*_tmp688=_tmp686;void*_tmp68E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp688)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3955
struct Cyc_Absyn_Datatypedecl*_tmp689=({struct Cyc_Absyn_Datatypedecl*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D->sc=Cyc_Absyn_Extern,_tmp68D->name=_tmp69A,_tmp68D->tvs=0,_tmp68D->fields=0,_tmp68D->is_extensible=_tmp699;_tmp68D;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp689);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp69A);
# 3959
if(_tmp682 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp68C=({struct Cyc_String_pa_PrintArg_struct _tmp9D3;_tmp9D3.tag=0U,({
struct _dyneither_ptr _tmpBA2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69A));_tmp9D3.f1=_tmpBA2;});_tmp9D3;});void*_tmp68A[1U];_tmp68A[0]=& _tmp68C;({unsigned int _tmpBA4=loc;struct _dyneither_ptr _tmpBA3=({const char*_tmp68B="declare parameterized datatype %s before using";_tag_dyneither(_tmp68B,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBA4,_tmpBA3,_tag_dyneither(_tmp68A,sizeof(void*),1U));});});
return cvtenv;}
# 3964
goto _LL5;}}else{_LL8: _tmp68E=_tmp688;_LL9:(int)_rethrow(_tmp68E);}_LL5:;}};}
# 3968
if(_tmp699  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp691=({struct Cyc_String_pa_PrintArg_struct _tmp9D4;_tmp9D4.tag=0U,({struct _dyneither_ptr _tmpBA5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69A));_tmp9D4.f1=_tmpBA5;});_tmp9D4;});void*_tmp68F[1U];_tmp68F[0]=& _tmp691;({unsigned int _tmpBA7=loc;struct _dyneither_ptr _tmpBA6=({const char*_tmp690="datatype %s was not declared @extensible";_tag_dyneither(_tmp690,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpBA7,_tmpBA6,_tag_dyneither(_tmp68F,sizeof(void*),1U));});});
({union Cyc_Absyn_DatatypeInfo _tmpBA8=Cyc_Absyn_KnownDatatype(tudp);*info=_tmpBA8;});
_tmp69B=*tudp;goto _LL4;}}else{_LL3: _tmp69B=*(_tmp684.KnownDatatype).val;_LL4: {
# 3974
struct Cyc_List_List*tvs=_tmp69B->tvs;
for(0;_tmp682 != 0  && tvs != 0;(_tmp682=_tmp682->tl,tvs=tvs->tl)){
void*t=(void*)_tmp682->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3980
{struct _tuple0 _tmp692=({struct _tuple0 _tmp9D5;({void*_tmpBA9=Cyc_Absyn_compress_kb(tv->kind);_tmp9D5.f1=_tmpBA9;}),_tmp9D5.f2=t;_tmp9D5;});struct _tuple0 _tmp693=_tmp692;struct Cyc_Absyn_Tvar*_tmp694;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp693.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp693.f2)->tag == 2U){_LLB: _tmp694=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp693.f2)->f1;_LLC:
# 3982
({struct Cyc_List_List*_tmpBAA=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp694);cvtenv.kind_env=_tmpBAA;});
({struct Cyc_List_List*_tmpBAB=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp694,1);cvtenv.free_vars=_tmpBAB;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 3987
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3991
if(_tmp682 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp697=({struct Cyc_String_pa_PrintArg_struct _tmp9D6;_tmp9D6.tag=0U,({
struct _dyneither_ptr _tmpBAC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69B->name));_tmp9D6.f1=_tmpBAC;});_tmp9D6;});void*_tmp695[1U];_tmp695[0]=& _tmp697;({unsigned int _tmpBAE=loc;struct _dyneither_ptr _tmpBAD=({const char*_tmp696="too many type arguments for datatype %s";_tag_dyneither(_tmp696,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpBAE,_tmpBAD,_tag_dyneither(_tmp695,sizeof(void*),1U));});});
if(tvs != 0){
# 3996
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->hd=e,_tmp698->tl=hidden_ts;_tmp698;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4003
({struct Cyc_List_List*_tmpBB0=({struct Cyc_List_List*_tmpBAF=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpBAF,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmpBB0;});}
# 4005
goto _LL0;}}_LL0:;}
# 4007
return cvtenv;}
# 4011
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype_field(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List*targs,int allow_abs_aggr){
# 4017
{union Cyc_Absyn_DatatypeFieldInfo _tmp69C=*info;union Cyc_Absyn_DatatypeFieldInfo _tmp69D=_tmp69C;struct Cyc_Absyn_Datatypedecl*_tmp6B0;struct Cyc_Absyn_Datatypefield*_tmp6AF;struct _tuple2*_tmp6AE;struct _tuple2*_tmp6AD;int _tmp6AC;if((_tmp69D.UnknownDatatypefield).tag == 1){_LL1: _tmp6AE=((_tmp69D.UnknownDatatypefield).val).datatype_name;_tmp6AD=((_tmp69D.UnknownDatatypefield).val).field_name;_tmp6AC=((_tmp69D.UnknownDatatypefield).val).is_extensible;_LL2: {
# 4020
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp6AE);
struct Cyc_Absyn_Datatypefield*tuf;
# 4025
{struct Cyc_List_List*_tmp69E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp69E=_tmp69E->tl){
if(_tmp69E == 0)({void*_tmp69F=0U;({struct _dyneither_ptr _tmpBB1=({const char*_tmp6A0="Tcutil found a bad datatypefield";_tag_dyneither(_tmp6A0,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBB1,_tag_dyneither(_tmp69F,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp69E->hd)->name,_tmp6AD)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp69E->hd;
break;}}}
# 4032
({union Cyc_Absyn_DatatypeFieldInfo _tmpBB2=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmpBB2;});
_tmp6B0=tud;_tmp6AF=tuf;goto _LL4;}}else{_LL3: _tmp6B0=((_tmp69D.KnownDatatypefield).val).f1;_tmp6AF=((_tmp69D.KnownDatatypefield).val).f2;_LL4: {
# 4036
struct Cyc_List_List*tvs=_tmp6B0->tvs;
for(0;targs != 0  && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4042
{struct _tuple0 _tmp6A1=({struct _tuple0 _tmp9D7;({void*_tmpBB3=Cyc_Absyn_compress_kb(tv->kind);_tmp9D7.f1=_tmpBB3;}),_tmp9D7.f2=t;_tmp9D7;});struct _tuple0 _tmp6A2=_tmp6A1;struct Cyc_Absyn_Tvar*_tmp6A3;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6A2.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A2.f2)->tag == 2U){_LL6: _tmp6A3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A2.f2)->f1;_LL7:
# 4044
({struct Cyc_List_List*_tmpBB4=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp6A3);cvtenv.kind_env=_tmpBB4;});
({struct Cyc_List_List*_tmpBB5=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp6A3,1);cvtenv.free_vars=_tmpBB5;});
continue;}else{goto _LL8;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}{
# 4049
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4053
if(targs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6A6=({struct Cyc_String_pa_PrintArg_struct _tmp9D9;_tmp9D9.tag=0U,({
struct _dyneither_ptr _tmpBB6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B0->name));_tmp9D9.f1=_tmpBB6;});_tmp9D9;});struct Cyc_String_pa_PrintArg_struct _tmp6A7=({struct Cyc_String_pa_PrintArg_struct _tmp9D8;_tmp9D8.tag=0U,({struct _dyneither_ptr _tmpBB7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6AF->name));_tmp9D8.f1=_tmpBB7;});_tmp9D8;});void*_tmp6A4[2U];_tmp6A4[0]=& _tmp6A6,_tmp6A4[1]=& _tmp6A7;({unsigned int _tmpBB9=loc;struct _dyneither_ptr _tmpBB8=({const char*_tmp6A5="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp6A5,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpBB9,_tmpBB8,_tag_dyneither(_tmp6A4,sizeof(void*),2U));});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6AA=({struct Cyc_String_pa_PrintArg_struct _tmp9DB;_tmp9DB.tag=0U,({
struct _dyneither_ptr _tmpBBA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B0->name));_tmp9DB.f1=_tmpBBA;});_tmp9DB;});struct Cyc_String_pa_PrintArg_struct _tmp6AB=({struct Cyc_String_pa_PrintArg_struct _tmp9DA;_tmp9DA.tag=0U,({struct _dyneither_ptr _tmpBBB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6AF->name));_tmp9DA.f1=_tmpBBB;});_tmp9DA;});void*_tmp6A8[2U];_tmp6A8[0]=& _tmp6AA,_tmp6A8[1]=& _tmp6AB;({unsigned int _tmpBBD=loc;struct _dyneither_ptr _tmpBBC=({const char*_tmp6A9="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp6A9,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpBBD,_tmpBBC,_tag_dyneither(_tmp6A8,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 4061
return cvtenv;}
# 4064
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_app(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 4069
struct Cyc_List_List*_tmp6B1=*targsp;
{void*_tmp6B2=c;union Cyc_Absyn_DatatypeFieldInfo*_tmp6D9;union Cyc_Absyn_DatatypeInfo*_tmp6D8;union Cyc_Absyn_AggrInfo*_tmp6D7;struct Cyc_List_List*_tmp6D6;struct _tuple2*_tmp6D5;struct Cyc_Absyn_Enumdecl**_tmp6D4;switch(*((int*)_tmp6B2)){case 1U: _LL1: _LL2:
# 4072
 goto _LL4;case 2U: _LL3: _LL4: goto _LL6;case 0U: _LL5: _LL6: goto _LL8;case 7U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA: goto _LLC;case 5U: _LLB: _LLC: goto _LLE;case 12U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10: goto _LL12;case 14U: _LL11: _LL12: goto _LL14;case 17U: _LL13: _LL14:
# 4076
 if(_tmp6B1 != 0)({struct Cyc_String_pa_PrintArg_struct _tmp6B5=({struct Cyc_String_pa_PrintArg_struct _tmp9DC;_tmp9DC.tag=0U,({
struct _dyneither_ptr _tmpBBE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6->tag=0U,_tmp6B6->f1=c,_tmp6B6->f2=_tmp6B1;_tmp6B6;})));_tmp9DC.f1=_tmpBBE;});_tmp9DC;});void*_tmp6B3[1U];_tmp6B3[0]=& _tmp6B5;({struct _dyneither_ptr _tmpBBF=({const char*_tmp6B4="%s applied to argument(s)";_tag_dyneither(_tmp6B4,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBBF,_tag_dyneither(_tmp6B3,sizeof(void*),1U));});});
goto _LL0;case 9U: _LL15: _LL16:
# 4080
 for(0;_tmp6B1 != 0;_tmp6B1=_tmp6B1->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp6B1->hd,1,1);}
goto _LL0;case 4U: _LL17: _LL18:
# 4084
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B1)!= 1)({void*_tmp6B7=0U;({struct _dyneither_ptr _tmpBC0=({const char*_tmp6B8="tag_t applied to wrong number of arguments";_tag_dyneither(_tmp6B8,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC0,_tag_dyneither(_tmp6B7,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp6B1))->hd,0,1);goto _LL0;case 15U: _LL19: _tmp6D5=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp6B2)->f1;_tmp6D4=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp6B2)->f2;_LL1A:
# 4087
 if(_tmp6B1 != 0)({void*_tmp6B9=0U;({struct _dyneither_ptr _tmpBC1=({const char*_tmp6BA="enum applied to argument(s)";_tag_dyneither(_tmp6BA,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC1,_tag_dyneither(_tmp6B9,sizeof(void*),0U));});});
if(*_tmp6D4 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp6D4))->fields == 0){
struct _handler_cons _tmp6BB;_push_handler(& _tmp6BB);{int _tmp6BD=0;if(setjmp(_tmp6BB.handler))_tmp6BD=1;if(!_tmp6BD){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6D5);
*_tmp6D4=*ed;}
# 4090
;_pop_handler();}else{void*_tmp6BC=(void*)_exn_thrown;void*_tmp6BE=_tmp6BC;void*_tmp6C1;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6BE)->tag == Cyc_Dict_Absent){_LL2C: _LL2D: {
# 4094
struct Cyc_Absyn_Enumdecl*_tmp6BF=({struct Cyc_Absyn_Enumdecl*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0->sc=Cyc_Absyn_Extern,_tmp6C0->name=_tmp6D5,_tmp6C0->fields=0;_tmp6C0;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp6BF);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6D5);
*_tmp6D4=*ed;
goto _LL2B;};}}else{_LL2E: _tmp6C1=_tmp6BE;_LL2F:(int)_rethrow(_tmp6C1);}_LL2B:;}};}
# 4100
goto _LL0;case 16U: _LL1B: _tmp6D6=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp6B2)->f1;_LL1C:
# 4102
 if(_tmp6B1 != 0)({void*_tmp6C2=0U;({struct _dyneither_ptr _tmpBC2=({const char*_tmp6C3="enum applied to argument(s)";_tag_dyneither(_tmp6C3,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC2,_tag_dyneither(_tmp6C2,sizeof(void*),0U));});});{
# 4104
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp6D6 != 0;_tmp6D6=_tmp6D6->tl){
struct Cyc_Absyn_Enumfield*_tmp6C4=(struct Cyc_Absyn_Enumfield*)_tmp6D6->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp6C4->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp6C7=({struct Cyc_String_pa_PrintArg_struct _tmp9DD;_tmp9DD.tag=0U,_tmp9DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6C4->name).f2);_tmp9DD;});void*_tmp6C5[1U];_tmp6C5[0]=& _tmp6C7;({unsigned int _tmpBC4=_tmp6C4->loc;struct _dyneither_ptr _tmpBC3=({const char*_tmp6C6="duplicate enum field name %s";_tag_dyneither(_tmp6C6,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpBC4,_tmpBC3,_tag_dyneither(_tmp6C5,sizeof(void*),1U));});});else{
# 4111
prev_fields=({struct Cyc_List_List*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->hd=(*_tmp6C4->name).f2,_tmp6C8->tl=prev_fields;_tmp6C8;});}
if(_tmp6C4->tag == 0)
({struct Cyc_Absyn_Exp*_tmpBC5=Cyc_Absyn_uint_exp(tag_count,_tmp6C4->loc);_tmp6C4->tag=_tmpBC5;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6C4->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp6CB=({struct Cyc_String_pa_PrintArg_struct _tmp9DE;_tmp9DE.tag=0U,_tmp9DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6C4->name).f2);_tmp9DE;});void*_tmp6C9[1U];_tmp6C9[0]=& _tmp6CB;({unsigned int _tmpBC7=loc;struct _dyneither_ptr _tmpBC6=({const char*_tmp6CA="enum field %s: expression is not constant";_tag_dyneither(_tmp6CA,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpBC7,_tmpBC6,_tag_dyneither(_tmp6C9,sizeof(void*),1U));});});}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6C4->tag))).f1;
tag_count=t1 + (unsigned int)1;};}
# 4119
goto _LL0;};case 10U: _LL1D: _LL1E:
# 4121
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B1)!= 1)({void*_tmp6CC=0U;({struct _dyneither_ptr _tmpBC8=({const char*_tmp6CD="regions has wrong number of arguments";_tag_dyneither(_tmp6CD,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC8,_tag_dyneither(_tmp6CC,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,(void*)((struct Cyc_List_List*)_check_null(_tmp6B1))->hd,1,1);goto _LL0;case 3U: _LL1F: _LL20:
# 4124
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B1)!= 1)({void*_tmp6CE=0U;({struct _dyneither_ptr _tmpBC9=({const char*_tmp6CF="region_t has wrong number of arguments";_tag_dyneither(_tmp6CF,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC9,_tag_dyneither(_tmp6CE,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp6B1))->hd,1,1);
goto _LL0;case 13U: _LL21: _LL22:
# 4128
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B1)!= 1)({void*_tmp6D0=0U;({struct _dyneither_ptr _tmpBCA=({const char*_tmp6D1="@thin has wrong number of arguments";_tag_dyneither(_tmp6D1,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBCA,_tag_dyneither(_tmp6D0,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp6B1))->hd,0,1);
goto _LL0;case 8U: _LL23: _LL24:
# 4132
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B1)!= 1)({void*_tmp6D2=0U;({struct _dyneither_ptr _tmpBCB=({const char*_tmp6D3="access has wrong number of arguments";_tag_dyneither(_tmp6D3,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBCB,_tag_dyneither(_tmp6D2,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp6B1))->hd,1,1);goto _LL0;case 20U: _LL25: _tmp6D7=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp6B2)->f1;_LL26:
# 4135
 cvtenv=Cyc_Tcutil_i_check_valid_aggr(loc,te,cvtenv,expected_kind,_tmp6D7,targsp,allow_abs_aggr);
# 4137
goto _LL0;case 18U: _LL27: _tmp6D8=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp6B2)->f1;_LL28:
# 4139
 cvtenv=Cyc_Tcutil_i_check_valid_datatype(loc,te,cvtenv,expected_kind,_tmp6D8,targsp,allow_abs_aggr);
# 4141
goto _LL0;default: _LL29: _tmp6D9=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp6B2)->f1;_LL2A:
# 4143
 cvtenv=Cyc_Tcutil_i_check_valid_datatype_field(loc,te,cvtenv,expected_kind,_tmp6D9,_tmp6B1,allow_abs_aggr);
# 4145
goto _LL0;}_LL0:;}
# 4147
return cvtenv;}struct _tuple32{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 4151
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 4159
{void*_tmp6DA=Cyc_Tcutil_compress(t);void*_tmp6DB=_tmp6DA;struct _tuple2*_tmp7DF;struct Cyc_List_List**_tmp7DE;struct Cyc_Absyn_Typedefdecl**_tmp7DD;void**_tmp7DC;enum Cyc_Absyn_AggrKind _tmp7DB;struct Cyc_List_List*_tmp7DA;struct Cyc_List_List*_tmp7D9;struct Cyc_List_List**_tmp7D8;void**_tmp7D7;struct Cyc_Absyn_Tqual*_tmp7D6;void*_tmp7D5;struct Cyc_List_List*_tmp7D4;int _tmp7D3;struct Cyc_Absyn_VarargInfo*_tmp7D2;struct Cyc_List_List*_tmp7D1;struct Cyc_List_List*_tmp7D0;struct Cyc_Absyn_Exp*_tmp7CF;struct Cyc_List_List**_tmp7CE;struct Cyc_Absyn_Exp*_tmp7CD;struct Cyc_List_List**_tmp7CC;void*_tmp7CB;struct Cyc_Absyn_Tqual*_tmp7CA;struct Cyc_Absyn_Exp**_tmp7C9;void*_tmp7C8;unsigned int _tmp7C7;struct Cyc_Absyn_Exp*_tmp7C6;struct Cyc_Absyn_Exp*_tmp7C5;void*_tmp7C4;struct Cyc_Absyn_Tqual*_tmp7C3;void*_tmp7C2;void*_tmp7C1;void*_tmp7C0;void*_tmp7BF;void*_tmp7BE;void***_tmp7BD;struct Cyc_Absyn_Tvar*_tmp7BC;struct Cyc_Core_Opt**_tmp7BB;void**_tmp7BA;void*_tmp7B9;struct Cyc_List_List**_tmp7B8;switch(*((int*)_tmp6DB)){case 0U: _LL1: _tmp7B9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6DB)->f1;_tmp7B8=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6DB)->f2;_LL2:
# 4161
 cvtenv=Cyc_Tcutil_i_check_valid_type_app(loc,te,cvtenv,expected_kind,_tmp7B9,_tmp7B8,put_in_effect,allow_abs_aggr);
# 4163
goto _LL0;case 1U: _LL3: _tmp7BB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6DB)->f1;_tmp7BA=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6DB)->f2;_LL4:
# 4166
 if(*_tmp7BB == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BB))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BB))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpBCC=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp7BB=_tmpBCC;});
if(((cvtenv.fn_result  && cvtenv.generalize_evars) && (int)expected_kind->kind == (int)Cyc_Absyn_RgnKind) && !te->tempest_generalize){
# 4171
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
*_tmp7BA=Cyc_Absyn_unique_rgn_type;else{
# 4174
*_tmp7BA=Cyc_Absyn_heap_rgn_type;}}else{
if((cvtenv.generalize_evars  && (int)expected_kind->kind != (int)Cyc_Absyn_BoolKind) && (int)expected_kind->kind != (int)Cyc_Absyn_PtrBndKind){
# 4178
struct Cyc_Absyn_Tvar*_tmp6DC=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD->tag=2U,_tmp6DD->f1=0,_tmp6DD->f2=expected_kind;_tmp6DD;}));
({void*_tmpBCD=Cyc_Absyn_var_type(_tmp6DC);*_tmp7BA=_tmpBCD;});
_tmp7BC=_tmp6DC;goto _LL6;}else{
# 4182
({struct Cyc_List_List*_tmpBCE=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpBCE;});}}
# 4184
goto _LL0;case 2U: _LL5: _tmp7BC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6DB)->f1;_LL6:
# 4186
{void*_tmp6DE=Cyc_Absyn_compress_kb(_tmp7BC->kind);void*_tmp6DF=_tmp6DE;struct Cyc_Core_Opt**_tmp6E2;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6DF)->tag == 1U){_LL1A: _tmp6E2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6DF)->f1;_LL1B:
# 4188
({struct Cyc_Core_Opt*_tmpBD0=({struct Cyc_Core_Opt*_tmp6E1=_cycalloc(sizeof(*_tmp6E1));({void*_tmpBCF=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0->tag=2U,_tmp6E0->f1=0,_tmp6E0->f2=expected_kind;_tmp6E0;});_tmp6E1->v=_tmpBCF;});_tmp6E1;});*_tmp6E2=_tmpBD0;});goto _LL19;}else{_LL1C: _LL1D:
 goto _LL19;}_LL19:;}
# 4193
({struct Cyc_List_List*_tmpBD1=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7BC);cvtenv.kind_env=_tmpBD1;});
# 4196
({struct Cyc_List_List*_tmpBD2=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7BC,put_in_effect);cvtenv.free_vars=_tmpBD2;});
# 4198
{void*_tmp6E3=Cyc_Absyn_compress_kb(_tmp7BC->kind);void*_tmp6E4=_tmp6E3;struct Cyc_Core_Opt**_tmp6E8;struct Cyc_Absyn_Kind*_tmp6E7;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E4)->tag == 2U){_LL1F: _tmp6E8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E4)->f1;_tmp6E7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E4)->f2;_LL20:
# 4200
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp6E7))
({struct Cyc_Core_Opt*_tmpBD4=({struct Cyc_Core_Opt*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));({void*_tmpBD3=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5->tag=2U,_tmp6E5->f1=0,_tmp6E5->f2=expected_kind;_tmp6E5;});_tmp6E6->v=_tmpBD3;});_tmp6E6;});*_tmp6E8=_tmpBD4;});
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 4205
goto _LL0;case 10U: _LL7: _tmp7BE=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6DB)->f1)->r;_tmp7BD=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6DB)->f2;_LL8: {
# 4211
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp6E9=_tmp7BE;struct Cyc_Absyn_Datatypedecl*_tmp6EC;struct Cyc_Absyn_Enumdecl*_tmp6EB;struct Cyc_Absyn_Aggrdecl*_tmp6EA;switch(*((int*)_tmp6E9)){case 0U: _LL24: _tmp6EA=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp6E9)->f1;_LL25:
# 4214
 if(te->in_extern_c_include)
_tmp6EA->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp6EA);goto _LL23;case 1U: _LL26: _tmp6EB=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp6E9)->f1;_LL27:
# 4218
 if(te->in_extern_c_include)
_tmp6EB->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp6EB);goto _LL23;default: _LL28: _tmp6EC=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp6E9)->f1;_LL29:
# 4222
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp6EC);goto _LL23;}_LL23:;}
# 4224
({void**_tmpBD5=({void**_tmp6ED=_cycalloc(sizeof(*_tmp6ED));*_tmp6ED=new_t;_tmp6ED;});*_tmp7BD=_tmpBD5;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 3U: _LL9: _tmp7C4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DB)->f1).elt_type;_tmp7C3=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DB)->f1).elt_tq;_tmp7C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DB)->f1).ptr_atts).rgn;_tmp7C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DB)->f1).ptr_atts).nullable;_tmp7C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DB)->f1).ptr_atts).bounds;_tmp7BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DB)->f1).ptr_atts).zero_term;_LLA: {
# 4230
int is_zero_terminated;
# 4232
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7C4,1,1);
({int _tmpBD6=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7C3->print_const,_tmp7C4);_tmp7C3->real_const=_tmpBD6;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp6EE=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp6EF=_tmp6EE;switch(_tmp6EF){case Cyc_Absyn_Aliasable: _LL2B: _LL2C:
 k=& Cyc_Tcutil_rk;goto _LL2A;case Cyc_Absyn_Unique: _LL2D: _LL2E:
 k=& Cyc_Tcutil_urk;goto _LL2A;case Cyc_Absyn_Top: _LL2F: _LL30:
 goto _LL32;default: _LL31: _LL32:
 k=& Cyc_Tcutil_trk;goto _LL2A;}_LL2A:;}
# 4241
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp7C2,1,1);
# 4244
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7BF,0,1);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7C1,0,1);
({void*_tmpBD7=_tmp7BF;Cyc_Tcutil_unify(_tmpBD7,Cyc_Tcutil_is_char_type(_tmp7C4)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});
is_zero_terminated=Cyc_Absyn_type2bool(0,_tmp7BF);
if(is_zero_terminated){
# 4250
if(!Cyc_Tcutil_admits_zero(_tmp7C4))
({struct Cyc_String_pa_PrintArg_struct _tmp6F2=({struct Cyc_String_pa_PrintArg_struct _tmp9DF;_tmp9DF.tag=0U,({
struct _dyneither_ptr _tmpBD8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7C4));_tmp9DF.f1=_tmpBD8;});_tmp9DF;});void*_tmp6F0[1U];_tmp6F0[0]=& _tmp6F2;({unsigned int _tmpBDA=loc;struct _dyneither_ptr _tmpBD9=({const char*_tmp6F1="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp6F1,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpBDA,_tmpBD9,_tag_dyneither(_tmp6F0,sizeof(void*),1U));});});}
# 4255
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ptrbk,_tmp7C0,0,allow_abs_aggr);{
struct Cyc_Absyn_Exp*_tmp6F3=({void*_tmpBDB=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBDB,_tmp7C0);});
if(_tmp6F3 != 0){
struct _tuple13 _tmp6F4=Cyc_Evexp_eval_const_uint_exp(_tmp6F3);struct _tuple13 _tmp6F5=_tmp6F4;unsigned int _tmp6F9;int _tmp6F8;_LL34: _tmp6F9=_tmp6F5.f1;_tmp6F8=_tmp6F5.f2;_LL35:;
if(is_zero_terminated  && (!_tmp6F8  || _tmp6F9 < (unsigned int)1))
({void*_tmp6F6=0U;({unsigned int _tmpBDD=loc;struct _dyneither_ptr _tmpBDC=({const char*_tmp6F7="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp6F7,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpBDD,_tmpBDC,_tag_dyneither(_tmp6F6,sizeof(void*),0U));});});}
# 4262
goto _LL0;};};}case 9U: _LLB: _tmp7C5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6DB)->f1;_LLC:
# 4267
({struct Cyc_Tcenv_Tenv*_tmpBDE=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpBDE,0,_tmp7C5);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp7C5))
({void*_tmp6FA=0U;({unsigned int _tmpBE0=loc;struct _dyneither_ptr _tmpBDF=({const char*_tmp6FB="valueof_t requires an int expression";_tag_dyneither(_tmp6FB,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpBE0,_tmpBDF,_tag_dyneither(_tmp6FA,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7C5,te,cvtenv);
goto _LL0;case 11U: _LLD: _tmp7C6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6DB)->f1;_LLE:
# 4276
({struct Cyc_Tcenv_Tenv*_tmpBE1=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpBE1,0,_tmp7C6);});
goto _LL0;case 4U: _LLF: _tmp7CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DB)->f1).elt_type;_tmp7CA=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DB)->f1).tq;_tmp7C9=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DB)->f1).num_elts;_tmp7C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DB)->f1).zero_term;_tmp7C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DB)->f1).zt_loc;_LL10: {
# 4281
struct Cyc_Absyn_Exp*_tmp6FC=*_tmp7C9;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7CB,1,allow_abs_aggr);
({int _tmpBE2=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7CA->print_const,_tmp7CB);_tmp7CA->real_const=_tmpBE2;});{
# 4285
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,_tmp7C8);
if(is_zero_terminated){
# 4288
if(!Cyc_Tcutil_admits_zero(_tmp7CB))
({struct Cyc_String_pa_PrintArg_struct _tmp6FF=({struct Cyc_String_pa_PrintArg_struct _tmp9E0;_tmp9E0.tag=0U,({
struct _dyneither_ptr _tmpBE3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7CB));_tmp9E0.f1=_tmpBE3;});_tmp9E0;});void*_tmp6FD[1U];_tmp6FD[0]=& _tmp6FF;({unsigned int _tmpBE5=loc;struct _dyneither_ptr _tmpBE4=({const char*_tmp6FE="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp6FE,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpBE5,_tmpBE4,_tag_dyneither(_tmp6FD,sizeof(void*),1U));});});}
# 4294
if(_tmp6FC == 0){
# 4296
if(is_zero_terminated)
# 4298
({struct Cyc_Absyn_Exp*_tmpBE6=_tmp6FC=Cyc_Absyn_uint_exp(1U,0U);*_tmp7C9=_tmpBE6;});else{
# 4301
({void*_tmp700=0U;({unsigned int _tmpBE8=loc;struct _dyneither_ptr _tmpBE7=({const char*_tmp701="array bound defaults to 1 here";_tag_dyneither(_tmp701,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpBE8,_tmpBE7,_tag_dyneither(_tmp700,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpBE9=_tmp6FC=Cyc_Absyn_uint_exp(1U,0U);*_tmp7C9=_tmpBE9;});}}
# 4305
({struct Cyc_Tcenv_Tenv*_tmpBEA=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpBEA,0,_tmp6FC);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp6FC))
({void*_tmp702=0U;({unsigned int _tmpBEC=loc;struct _dyneither_ptr _tmpBEB=({const char*_tmp703="array bounds expression is not an unsigned int";_tag_dyneither(_tmp703,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBEC,_tmpBEB,_tag_dyneither(_tmp702,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp6FC,te,cvtenv);{
# 4313
struct _tuple13 _tmp704=Cyc_Evexp_eval_const_uint_exp(_tmp6FC);struct _tuple13 _tmp705=_tmp704;unsigned int _tmp70B;int _tmp70A;_LL37: _tmp70B=_tmp705.f1;_tmp70A=_tmp705.f2;_LL38:;
# 4315
if((is_zero_terminated  && _tmp70A) && _tmp70B < (unsigned int)1)
({void*_tmp706=0U;({unsigned int _tmpBEE=loc;struct _dyneither_ptr _tmpBED=({const char*_tmp707="zero terminated array cannot have zero elements";_tag_dyneither(_tmp707,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpBEE,_tmpBED,_tag_dyneither(_tmp706,sizeof(void*),0U));});});
# 4318
if((_tmp70A  && _tmp70B < (unsigned int)1) && Cyc_Cyclone_tovc_r){
({void*_tmp708=0U;({unsigned int _tmpBF0=loc;struct _dyneither_ptr _tmpBEF=({const char*_tmp709="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp709,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpBF0,_tmpBEF,_tag_dyneither(_tmp708,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpBF1=Cyc_Absyn_uint_exp(1U,0U);*_tmp7C9=_tmpBF1;});}
# 4322
goto _LL0;};};}case 5U: _LL11: _tmp7D8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).tvars;_tmp7D7=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).effect;_tmp7D6=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).ret_tqual;_tmp7D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).ret_type;_tmp7D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).args;_tmp7D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).c_varargs;_tmp7D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).cyc_varargs;_tmp7D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).rgn_po;_tmp7D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).attributes;_tmp7CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).requires_clause;_tmp7CE=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).requires_relns;_tmp7CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).ensures_clause;_tmp7CC=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DB)->f1).ensures_relns;_LL12: {
# 4329
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp7D0 != 0;_tmp7D0=_tmp7D0->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7D0->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp70E=({struct Cyc_String_pa_PrintArg_struct _tmp9E1;_tmp9E1.tag=0U,({struct _dyneither_ptr _tmpBF2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7D0->hd));_tmp9E1.f1=_tmpBF2;});_tmp9E1;});void*_tmp70C[1U];_tmp70C[0]=& _tmp70E;({unsigned int _tmpBF4=loc;struct _dyneither_ptr _tmpBF3=({const char*_tmp70D="bad function type attribute %s";_tag_dyneither(_tmp70D,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpBF4,_tmpBF3,_tag_dyneither(_tmp70C,sizeof(void*),1U));});});{
void*_tmp70F=(void*)_tmp7D0->hd;void*_tmp710=_tmp70F;enum Cyc_Absyn_Format_Type _tmp715;int _tmp714;int _tmp713;switch(*((int*)_tmp710)){case 1U: _LL3A: _LL3B:
# 4342
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL39;case 2U: _LL3C: _LL3D:
# 4344
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL39;case 3U: _LL3E: _LL3F:
# 4346
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL39;case 19U: _LL40: _tmp715=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp710)->f1;_tmp714=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp710)->f2;_tmp713=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp710)->f3;_LL41:
# 4348
 if(!seen_format){
seen_format=1;
ft=_tmp715;
fmt_desc_arg=_tmp714;
fmt_arg_start=_tmp713;}else{
# 4354
({void*_tmp711=0U;({unsigned int _tmpBF6=loc;struct _dyneither_ptr _tmpBF5=({const char*_tmp712="function can't have multiple format attributes";_tag_dyneither(_tmp712,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBF6,_tmpBF5,_tag_dyneither(_tmp711,sizeof(void*),0U));});});}
goto _LL39;default: _LL42: _LL43:
 goto _LL39;}_LL39:;};}
# 4359
if(num_convs > 1)
({void*_tmp716=0U;({unsigned int _tmpBF8=loc;struct _dyneither_ptr _tmpBF7=({const char*_tmp717="function can't have multiple calling conventions";_tag_dyneither(_tmp717,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpBF8,_tmpBF7,_tag_dyneither(_tmp716,sizeof(void*),0U));});});
# 4364
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7D8);
{struct Cyc_List_List*b=*_tmp7D8;for(0;b != 0;b=b->tl){
({int _tmpBF9=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpBF9;});
({struct Cyc_List_List*_tmpBFA=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpBFA;});{
void*_tmp718=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp719=_tmp718;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp719)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp719)->f1)->kind == Cyc_Absyn_MemKind){_LL45: _LL46:
# 4370
({struct Cyc_String_pa_PrintArg_struct _tmp71C=({struct Cyc_String_pa_PrintArg_struct _tmp9E2;_tmp9E2.tag=0U,_tmp9E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);_tmp9E2;});void*_tmp71A[1U];_tmp71A[0]=& _tmp71C;({unsigned int _tmpBFC=loc;struct _dyneither_ptr _tmpBFB=({const char*_tmp71B="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp71B,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpBFC,_tmpBFB,_tag_dyneither(_tmp71A,sizeof(void*),1U));});});
goto _LL44;}else{goto _LL47;}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;};}}{
# 4378
struct Cyc_Tcutil_CVTEnv _tmp71D=({struct Cyc_Tcutil_CVTEnv _tmp9E5;_tmp9E5.r=Cyc_Core_heap_region,_tmp9E5.kind_env=cvtenv.kind_env,_tmp9E5.free_vars=0,_tmp9E5.free_evars=0,_tmp9E5.generalize_evars=cvtenv.generalize_evars,_tmp9E5.fn_result=1;_tmp9E5;});
# 4380
_tmp71D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71D,& Cyc_Tcutil_tmk,_tmp7D5,1,1);
({int _tmpBFD=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7D6->print_const,_tmp7D5);_tmp7D6->real_const=_tmpBFD;});
_tmp71D.fn_result=0;
# 4386
{struct Cyc_List_List*a=_tmp7D4;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp71E=(struct _tuple10*)a->hd;
void*_tmp71F=(*_tmp71E).f3;
_tmp71D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71D,& Cyc_Tcutil_tmk,_tmp71F,1,1);{
int _tmp720=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp71E).f2).print_const,_tmp71F);
((*_tmp71E).f2).real_const=_tmp720;
# 4394
if(Cyc_Tcutil_is_array_type(_tmp71F)){
# 4396
void*_tmp721=Cyc_Absyn_new_evar(0,0);
_tmp71D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71D,& Cyc_Tcutil_rk,_tmp721,1,1);
({void*_tmpBFE=Cyc_Tcutil_promote_array(_tmp71F,_tmp721,0);(*_tmp71E).f3=_tmpBFE;});}};}}
# 4403
if(_tmp7D2 != 0){
if(_tmp7D3)({void*_tmp722=0U;({struct _dyneither_ptr _tmpBFF=({const char*_tmp723="both c_vararg and cyc_vararg";_tag_dyneither(_tmp723,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBFF,_tag_dyneither(_tmp722,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp724=*_tmp7D2;struct Cyc_Absyn_VarargInfo _tmp725=_tmp724;void*_tmp736;int _tmp735;_LL4A: _tmp736=_tmp725.type;_tmp735=_tmp725.inject;_LL4B:;
_tmp71D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71D,& Cyc_Tcutil_tmk,_tmp736,1,1);
({int _tmpC00=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp7D2->tq).print_const,_tmp736);(_tmp7D2->tq).real_const=_tmpC00;});
# 4409
if(_tmp735){
void*_tmp726=Cyc_Tcutil_compress(_tmp736);void*_tmp727=_tmp726;void*_tmp734;void*_tmp733;void*_tmp732;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp727)->tag == 3U){_LL4D: _tmp734=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp727)->f1).elt_type;_tmp733=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp727)->f1).ptr_atts).bounds;_tmp732=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp727)->f1).ptr_atts).zero_term;_LL4E:
# 4412
{void*_tmp728=Cyc_Tcutil_compress(_tmp734);void*_tmp729=_tmp728;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp729)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp729)->f1)->tag == 18U){_LL52: _LL53:
# 4414
 if(Cyc_Tcutil_force_type2bool(0,_tmp732))
({void*_tmp72A=0U;({unsigned int _tmpC02=loc;struct _dyneither_ptr _tmpC01=({const char*_tmp72B="can't inject into a zeroterm pointer";_tag_dyneither(_tmp72B,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpC02,_tmpC01,_tag_dyneither(_tmp72A,sizeof(void*),0U));});});
if(!({void*_tmpC03=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpC03,_tmp733);}))
({void*_tmp72C=0U;({unsigned int _tmpC05=loc;struct _dyneither_ptr _tmpC04=({const char*_tmp72D="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp72D,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpC05,_tmpC04,_tag_dyneither(_tmp72C,sizeof(void*),0U));});});
goto _LL51;}else{goto _LL54;}}else{_LL54: _LL55:
({void*_tmp72E=0U;({unsigned int _tmpC07=loc;struct _dyneither_ptr _tmpC06=({const char*_tmp72F="can't inject a non-datatype type";_tag_dyneither(_tmp72F,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpC07,_tmpC06,_tag_dyneither(_tmp72E,sizeof(void*),0U));});});goto _LL51;}_LL51:;}
# 4421
goto _LL4C;}else{_LL4F: _LL50:
({void*_tmp730=0U;({unsigned int _tmpC09=loc;struct _dyneither_ptr _tmpC08=({const char*_tmp731="expecting a datatype pointer type";_tag_dyneither(_tmp731,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpC09,_tmpC08,_tag_dyneither(_tmp730,sizeof(void*),0U));});});goto _LL4C;}_LL4C:;}};}
# 4427
if(seen_format){
int _tmp737=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7D4);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp737) || fmt_arg_start < 0) || 
# 4431
(_tmp7D2 == 0  && !_tmp7D3) && fmt_arg_start != 0) || 
(_tmp7D2 != 0  || _tmp7D3) && fmt_arg_start != _tmp737 + 1)
# 4434
({void*_tmp738=0U;({unsigned int _tmpC0B=loc;struct _dyneither_ptr _tmpC0A=({const char*_tmp739="bad format descriptor";_tag_dyneither(_tmp739,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpC0B,_tmpC0A,_tag_dyneither(_tmp738,sizeof(void*),0U));});});else{
# 4437
struct _tuple10 _tmp73A=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7D4,fmt_desc_arg - 1);struct _tuple10 _tmp73B=_tmp73A;void*_tmp750;_LL57: _tmp750=_tmp73B.f3;_LL58:;
# 4439
{void*_tmp73C=Cyc_Tcutil_compress(_tmp750);void*_tmp73D=_tmp73C;void*_tmp749;void*_tmp748;void*_tmp747;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73D)->tag == 3U){_LL5A: _tmp749=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73D)->f1).elt_type;_tmp748=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73D)->f1).ptr_atts).bounds;_tmp747=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73D)->f1).ptr_atts).zero_term;_LL5B:
# 4442
 if(!Cyc_Tcutil_is_char_type(_tmp749))
({void*_tmp73E=0U;({unsigned int _tmpC0D=loc;struct _dyneither_ptr _tmpC0C=({const char*_tmp73F="format descriptor is not a string";_tag_dyneither(_tmp73F,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpC0D,_tmpC0C,_tag_dyneither(_tmp73E,sizeof(void*),0U));});});else{
# 4445
struct Cyc_Absyn_Exp*_tmp740=({void*_tmpC0E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpC0E,_tmp748);});
if(_tmp740 == 0  && _tmp7D3)
({void*_tmp741=0U;({unsigned int _tmpC10=loc;struct _dyneither_ptr _tmpC0F=({const char*_tmp742="format descriptor is not a char * type";_tag_dyneither(_tmp742,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpC10,_tmpC0F,_tag_dyneither(_tmp741,sizeof(void*),0U));});});else{
if(_tmp740 != 0  && !_tmp7D3)
({void*_tmp743=0U;({unsigned int _tmpC12=loc;struct _dyneither_ptr _tmpC11=({const char*_tmp744="format descriptor is not a char ? type";_tag_dyneither(_tmp744,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpC12,_tmpC11,_tag_dyneither(_tmp743,sizeof(void*),0U));});});}}
# 4451
goto _LL59;}else{_LL5C: _LL5D:
({void*_tmp745=0U;({unsigned int _tmpC14=loc;struct _dyneither_ptr _tmpC13=({const char*_tmp746="format descriptor is not a string type";_tag_dyneither(_tmp746,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpC14,_tmpC13,_tag_dyneither(_tmp745,sizeof(void*),0U));});});goto _LL59;}_LL59:;}
# 4454
if(fmt_arg_start != 0  && !_tmp7D3){
# 4458
int problem;
{struct _tuple32 _tmp74A=({struct _tuple32 _tmp9E3;_tmp9E3.f1=ft,({void*_tmpC15=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7D2))->type));_tmp9E3.f2=_tmpC15;});_tmp9E3;});struct _tuple32 _tmp74B=_tmp74A;struct Cyc_Absyn_Datatypedecl*_tmp74D;struct Cyc_Absyn_Datatypedecl*_tmp74C;switch(_tmp74B.f1){case Cyc_Absyn_Printf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74B.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74B.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74B.f2)->f1)->f1).KnownDatatype).tag == 2){_LL5F: _tmp74C=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74B.f2)->f1)->f1).KnownDatatype).val;_LL60:
# 4461
 problem=Cyc_Absyn_qvar_cmp(_tmp74C->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL5E;}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}case Cyc_Absyn_Scanf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74B.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74B.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74B.f2)->f1)->f1).KnownDatatype).tag == 2){_LL61: _tmp74D=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74B.f2)->f1)->f1).KnownDatatype).val;_LL62:
# 4463
 problem=Cyc_Absyn_qvar_cmp(_tmp74D->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL5E;}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}default: _LL63: _LL64:
# 4465
 problem=1;goto _LL5E;}_LL5E:;}
# 4467
if(problem)
({void*_tmp74E=0U;({unsigned int _tmpC17=loc;struct _dyneither_ptr _tmpC16=({const char*_tmp74F="format attribute and vararg types don't match";_tag_dyneither(_tmp74F,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpC17,_tmpC16,_tag_dyneither(_tmp74E,sizeof(void*),0U));});});}}}
# 4475
{struct Cyc_List_List*rpo=_tmp7D1;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp751=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp752=_tmp751;void*_tmp754;void*_tmp753;_LL66: _tmp754=_tmp752->f1;_tmp753=_tmp752->f2;_LL67:;
_tmp71D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71D,& Cyc_Tcutil_ek,_tmp754,1,1);
_tmp71D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71D,& Cyc_Tcutil_trk,_tmp753,1,1);}}{
# 4483
struct Cyc_Tcenv_Fenv*_tmp755=Cyc_Tcenv_bogus_fenv(_tmp7D5,_tmp7D4);
struct Cyc_Tcenv_Tenv*_tmp756=({struct Cyc_Tcenv_Tenv*_tmp794=_cycalloc(sizeof(*_tmp794));_tmp794->ns=te->ns,_tmp794->ae=te->ae,_tmp794->le=_tmp755,_tmp794->allow_valueof=1,_tmp794->in_extern_c_include=te->in_extern_c_include,_tmp794->in_tempest=te->in_tempest,_tmp794->tempest_generalize=te->tempest_generalize;_tmp794;});
struct _tuple31 _tmp757=({unsigned int _tmpC1B=loc;struct Cyc_Tcenv_Tenv*_tmpC1A=_tmp756;struct Cyc_Tcutil_CVTEnv _tmpC19=_tmp71D;struct _dyneither_ptr _tmpC18=({const char*_tmp793="@requires";_tag_dyneither(_tmp793,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpC1B,_tmpC1A,_tmpC19,_tmpC18,_tmp7CF);});struct _tuple31 _tmp758=_tmp757;struct Cyc_Tcutil_CVTEnv _tmp792;struct Cyc_List_List*_tmp791;_LL69: _tmp792=_tmp758.f1;_tmp791=_tmp758.f2;_LL6A:;
_tmp71D=_tmp792;
*_tmp7CE=_tmp791;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7D4,_tmp791);{
# 4496
struct _tuple31 _tmp759=({unsigned int _tmpC1F=loc;struct Cyc_Tcenv_Tenv*_tmpC1E=_tmp756;struct Cyc_Tcutil_CVTEnv _tmpC1D=_tmp71D;struct _dyneither_ptr _tmpC1C=({const char*_tmp790="@ensures";_tag_dyneither(_tmp790,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpC1F,_tmpC1E,_tmpC1D,_tmpC1C,_tmp7CD);});struct _tuple31 _tmp75A=_tmp759;struct Cyc_Tcutil_CVTEnv _tmp78F;struct Cyc_List_List*_tmp78E;_LL6C: _tmp78F=_tmp75A.f1;_tmp78E=_tmp75A.f2;_LL6D:;
_tmp71D=_tmp78F;
*_tmp7CC=_tmp78E;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7D4,_tmp78E);
# 4501
if(*_tmp7D7 != 0)
_tmp71D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71D,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp7D7),1,1);else{
# 4504
struct Cyc_List_List*effect=0;
# 4509
{struct Cyc_List_List*tvs=_tmp71D.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple29 _tmp75B=*((struct _tuple29*)tvs->hd);struct _tuple29 _tmp75C=_tmp75B;struct Cyc_Absyn_Tvar*_tmp76A;int _tmp769;_LL6F: _tmp76A=_tmp75C.f1;_tmp769=_tmp75C.f2;_LL70:;
if(!_tmp769)continue;{
void*_tmp75D=Cyc_Absyn_compress_kb(_tmp76A->kind);void*_tmp75E=_tmp75D;struct Cyc_Core_Opt**_tmp768;struct Cyc_Absyn_Kind*_tmp767;struct Cyc_Core_Opt**_tmp766;struct Cyc_Core_Opt**_tmp765;struct Cyc_Absyn_Kind*_tmp764;switch(*((int*)_tmp75E)){case 2U: _LL72: _tmp765=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75E)->f1;_tmp764=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75E)->f2;if((int)_tmp764->kind == (int)Cyc_Absyn_RgnKind){_LL73:
# 4515
 if((int)_tmp764->aliasqual == (int)Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpC20=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp765=_tmpC20;});_tmp767=_tmp764;goto _LL75;}
# 4518
({struct Cyc_Core_Opt*_tmpC21=Cyc_Tcutil_kind_to_bound_opt(_tmp764);*_tmp765=_tmpC21;});_tmp767=_tmp764;goto _LL75;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75E)->f2)->kind){case Cyc_Absyn_BoolKind: _LL76: _LL77:
# 4521
 goto _LL79;case Cyc_Absyn_PtrBndKind: _LL78: _LL79:
 goto _LL7B;case Cyc_Absyn_IntKind: _LL7A: _LL7B:
 goto _LL7D;case Cyc_Absyn_EffKind: _LL82: _tmp766=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75E)->f1;_LL83:
# 4528
({struct Cyc_Core_Opt*_tmpC22=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp766=_tmpC22;});goto _LL85;default: goto _LL88;}}case 0U: _LL74: _tmp767=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75E)->f1;if((int)_tmp767->kind == (int)Cyc_Absyn_RgnKind){_LL75:
# 4520
 effect=({struct Cyc_List_List*_tmp75F=_cycalloc(sizeof(*_tmp75F));({void*_tmpC23=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(_tmp76A));_tmp75F->hd=_tmpC23;}),_tmp75F->tl=effect;_tmp75F;});goto _LL71;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75E)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7C: _LL7D:
# 4524
 goto _LL7F;case Cyc_Absyn_PtrBndKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_IntKind: _LL80: _LL81:
 goto _LL71;case Cyc_Absyn_EffKind: _LL84: _LL85:
# 4530
 effect=({struct Cyc_List_List*_tmp760=_cycalloc(sizeof(*_tmp760));({void*_tmpC24=Cyc_Absyn_var_type(_tmp76A);_tmp760->hd=_tmpC24;}),_tmp760->tl=effect;_tmp760;});goto _LL71;default: _LL88: _LL89:
# 4535
 effect=({struct Cyc_List_List*_tmp763=_cycalloc(sizeof(*_tmp763));({void*_tmpC25=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(_tmp76A));_tmp763->hd=_tmpC25;}),_tmp763->tl=effect;_tmp763;});goto _LL71;}}default: _LL86: _tmp768=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp75E)->f1;_LL87:
# 4532
({struct Cyc_Core_Opt*_tmpC27=({struct Cyc_Core_Opt*_tmp762=_cycalloc(sizeof(*_tmp762));({void*_tmpC26=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761->tag=2U,_tmp761->f1=0,_tmp761->f2=& Cyc_Tcutil_ak;_tmp761;});_tmp762->v=_tmpC26;});_tmp762;});*_tmp768=_tmpC27;});goto _LL89;}_LL71:;};}}
# 4539
{struct Cyc_List_List*ts=_tmp71D.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple30 _tmp76B=*((struct _tuple30*)ts->hd);struct _tuple30 _tmp76C=_tmp76B;void*_tmp773;int _tmp772;_LL8B: _tmp773=_tmp76C.f1;_tmp772=_tmp76C.f2;_LL8C:;
if(!_tmp772)continue;{
struct Cyc_Absyn_Kind*_tmp76D=Cyc_Tcutil_type_kind(_tmp773);struct Cyc_Absyn_Kind*_tmp76E=_tmp76D;switch(((struct Cyc_Absyn_Kind*)_tmp76E)->kind){case Cyc_Absyn_RgnKind: _LL8E: _LL8F:
# 4544
 effect=({struct Cyc_List_List*_tmp76F=_cycalloc(sizeof(*_tmp76F));({void*_tmpC28=Cyc_Absyn_access_eff(_tmp773);_tmp76F->hd=_tmpC28;}),_tmp76F->tl=effect;_tmp76F;});goto _LL8D;case Cyc_Absyn_EffKind: _LL90: _LL91:
# 4546
 effect=({struct Cyc_List_List*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770->hd=_tmp773,_tmp770->tl=effect;_tmp770;});goto _LL8D;case Cyc_Absyn_IntKind: _LL92: _LL93:
 goto _LL8D;default: _LL94: _LL95:
# 4549
 effect=({struct Cyc_List_List*_tmp771=_cycalloc(sizeof(*_tmp771));({void*_tmpC29=Cyc_Absyn_regionsof_eff(_tmp773);_tmp771->hd=_tmpC29;}),_tmp771->tl=effect;_tmp771;});goto _LL8D;}_LL8D:;};}}
# 4552
({void*_tmpC2A=Cyc_Absyn_join_eff(effect);*_tmp7D7=_tmpC2A;});}
# 4558
if(*_tmp7D8 != 0){
struct Cyc_List_List*bs=*_tmp7D8;for(0;bs != 0;bs=bs->tl){
void*_tmp774=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp775=_tmp774;struct Cyc_Core_Opt**_tmp785;struct Cyc_Absyn_Kind*_tmp784;struct Cyc_Core_Opt**_tmp783;struct Cyc_Core_Opt**_tmp782;struct Cyc_Core_Opt**_tmp781;struct Cyc_Core_Opt**_tmp780;struct Cyc_Core_Opt**_tmp77F;struct Cyc_Core_Opt**_tmp77E;struct Cyc_Core_Opt**_tmp77D;struct Cyc_Core_Opt**_tmp77C;struct Cyc_Core_Opt**_tmp77B;switch(*((int*)_tmp775)){case 1U: _LL97: _tmp77B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp775)->f1;_LL98:
# 4562
({struct Cyc_String_pa_PrintArg_struct _tmp778=({struct Cyc_String_pa_PrintArg_struct _tmp9E4;_tmp9E4.tag=0U,_tmp9E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);_tmp9E4;});void*_tmp776[1U];_tmp776[0]=& _tmp778;({unsigned int _tmpC2C=loc;struct _dyneither_ptr _tmpC2B=({const char*_tmp777="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp777,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpC2C,_tmpC2B,_tag_dyneither(_tmp776,sizeof(void*),1U));});});
# 4564
_tmp77C=_tmp77B;goto _LL9A;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f2)->aliasqual == Cyc_Absyn_Top){_LL99: _tmp77C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f1;_LL9A:
 _tmp77D=_tmp77C;goto _LL9C;}else{goto _LLA9;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9B: _tmp77D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f1;_LL9C:
 _tmp77E=_tmp77D;goto _LL9E;case Cyc_Absyn_Aliasable: _LL9D: _tmp77E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f1;_LL9E:
 _tmp780=_tmp77E;goto _LLA0;case Cyc_Absyn_Unique: _LLA3: _tmp77F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f1;_LLA4:
# 4571
 _tmp782=_tmp77F;goto _LLA6;default: goto _LLA9;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9F: _tmp780=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f1;_LLA0:
# 4568
 _tmp781=_tmp780;goto _LLA2;case Cyc_Absyn_Aliasable: _LLA1: _tmp781=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f1;_LLA2:
# 4570
({struct Cyc_Core_Opt*_tmpC2D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp781=_tmpC2D;});goto _LL96;case Cyc_Absyn_Unique: _LLA5: _tmp782=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f1;_LLA6:
# 4573
({struct Cyc_Core_Opt*_tmpC2E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp782=_tmpC2E;});goto _LL96;default: goto _LLA9;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f2)->aliasqual == Cyc_Absyn_Top){_LLA7: _tmp783=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f1;_LLA8:
# 4575
({struct Cyc_Core_Opt*_tmpC2F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp783=_tmpC2F;});goto _LL96;}else{goto _LLA9;}default: _LLA9: _tmp785=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f1;_tmp784=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp775)->f2;_LLAA:
# 4577
({struct Cyc_Core_Opt*_tmpC30=Cyc_Tcutil_kind_to_bound_opt(_tmp784);*_tmp785=_tmpC30;});goto _LL96;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp775)->f1)->kind == Cyc_Absyn_MemKind){_LLAB: _LLAC:
# 4579
({void*_tmp779=0U;({unsigned int _tmpC32=loc;struct _dyneither_ptr _tmpC31=({const char*_tmp77A="functions can't abstract M types";_tag_dyneither(_tmp77A,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpC32,_tmpC31,_tag_dyneither(_tmp779,sizeof(void*),0U));});});goto _LL96;}else{_LLAD: _LLAE:
 goto _LL96;}}_LL96:;}}
# 4584
({struct Cyc_List_List*_tmpC33=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp71D.kind_env,*_tmp7D8);cvtenv.kind_env=_tmpC33;});
({struct Cyc_List_List*_tmpC34=Cyc_Tcutil_remove_bound_tvars_bool(_tmp71D.r,_tmp71D.free_vars,*_tmp7D8);_tmp71D.free_vars=_tmpC34;});
# 4587
{struct Cyc_List_List*tvs=_tmp71D.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple29 _tmp786=*((struct _tuple29*)tvs->hd);struct _tuple29 _tmp787=_tmp786;struct Cyc_Absyn_Tvar*_tmp789;int _tmp788;_LLB0: _tmp789=_tmp787.f1;_tmp788=_tmp787.f2;_LLB1:;
({struct Cyc_List_List*_tmpC35=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp789,_tmp788);cvtenv.free_vars=_tmpC35;});}}
# 4592
{struct Cyc_List_List*evs=_tmp71D.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple30 _tmp78A=*((struct _tuple30*)evs->hd);struct _tuple30 _tmp78B=_tmp78A;void*_tmp78D;int _tmp78C;_LLB3: _tmp78D=_tmp78B.f1;_tmp78C=_tmp78B.f2;_LLB4:;
({struct Cyc_List_List*_tmpC36=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp78D,_tmp78C);cvtenv.free_evars=_tmpC36;});}}
# 4596
goto _LL0;};};};}case 6U: _LL13: _tmp7D9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6DB)->f1;_LL14:
# 4599
 for(0;_tmp7D9 != 0;_tmp7D9=_tmp7D9->tl){
struct _tuple12*_tmp795=(struct _tuple12*)_tmp7D9->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp795).f2,1,0);
({int _tmpC37=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp795).f1).print_const,(*_tmp795).f2);
# 4602
((*_tmp795).f1).real_const=_tmpC37;});}
# 4605
goto _LL0;case 7U: _LL15: _tmp7DB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6DB)->f1;_tmp7DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6DB)->f2;_LL16: {
# 4609
struct Cyc_List_List*prev_fields=0;
for(0;_tmp7DA != 0;_tmp7DA=_tmp7DA->tl){
struct Cyc_Absyn_Aggrfield*_tmp796=(struct Cyc_Absyn_Aggrfield*)_tmp7DA->hd;struct Cyc_Absyn_Aggrfield*_tmp797=_tmp796;struct _dyneither_ptr*_tmp7A7;struct Cyc_Absyn_Tqual*_tmp7A6;void*_tmp7A5;struct Cyc_Absyn_Exp*_tmp7A4;struct Cyc_List_List*_tmp7A3;struct Cyc_Absyn_Exp*_tmp7A2;_LLB6: _tmp7A7=_tmp797->name;_tmp7A6=(struct Cyc_Absyn_Tqual*)& _tmp797->tq;_tmp7A5=_tmp797->type;_tmp7A4=_tmp797->width;_tmp7A3=_tmp797->attributes;_tmp7A2=_tmp797->requires_clause;_LLB7:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp7A7))
({struct Cyc_String_pa_PrintArg_struct _tmp79A=({struct Cyc_String_pa_PrintArg_struct _tmp9E6;_tmp9E6.tag=0U,_tmp9E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7A7);_tmp9E6;});void*_tmp798[1U];_tmp798[0]=& _tmp79A;({unsigned int _tmpC39=loc;struct _dyneither_ptr _tmpC38=({const char*_tmp799="duplicate field %s";_tag_dyneither(_tmp799,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpC39,_tmpC38,_tag_dyneither(_tmp798,sizeof(void*),1U));});});
if(({struct _dyneither_ptr _tmpC3A=(struct _dyneither_ptr)*_tmp7A7;Cyc_strcmp(_tmpC3A,({const char*_tmp79B="";_tag_dyneither(_tmp79B,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp79C=_cycalloc(sizeof(*_tmp79C));_tmp79C->hd=_tmp7A7,_tmp79C->tl=prev_fields;_tmp79C;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7A5,1,0);
({int _tmpC3B=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7A6->print_const,_tmp7A5);_tmp7A6->real_const=_tmpC3B;});
Cyc_Tcutil_check_bitfield(loc,te,_tmp7A5,_tmp7A4,_tmp7A7);
Cyc_Tcutil_check_field_atts(loc,_tmp7A7,_tmp7A3);
if(_tmp7A2 != 0){
# 4622
if((int)_tmp7DB != (int)1U)
({void*_tmp79D=0U;({unsigned int _tmpC3D=loc;struct _dyneither_ptr _tmpC3C=({const char*_tmp79E="@requires clause is only allowed on union members";_tag_dyneither(_tmp79E,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpC3D,_tmpC3C,_tag_dyneither(_tmp79D,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpC3E=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpC3E,0,_tmp7A2);});
if(!Cyc_Tcutil_is_integral(_tmp7A2))
({struct Cyc_String_pa_PrintArg_struct _tmp7A1=({struct Cyc_String_pa_PrintArg_struct _tmp9E7;_tmp9E7.tag=0U,({
struct _dyneither_ptr _tmpC3F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp7A2->topt)));_tmp9E7.f1=_tmpC3F;});_tmp9E7;});void*_tmp79F[1U];_tmp79F[0]=& _tmp7A1;({unsigned int _tmpC41=loc;struct _dyneither_ptr _tmpC40=({const char*_tmp7A0="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp7A0,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpC41,_tmpC40,_tag_dyneither(_tmp79F,sizeof(void*),1U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7A2,te,cvtenv);}}
# 4631
goto _LL0;}default: _LL17: _tmp7DF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DB)->f1;_tmp7DE=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DB)->f2;_tmp7DD=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DB)->f3;_tmp7DC=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DB)->f4;_LL18: {
# 4634
struct Cyc_List_List*targs=*_tmp7DE;
# 4636
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp7A8;_push_handler(& _tmp7A8);{int _tmp7AA=0;if(setjmp(_tmp7A8.handler))_tmp7AA=1;if(!_tmp7AA){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7DF);;_pop_handler();}else{void*_tmp7A9=(void*)_exn_thrown;void*_tmp7AB=_tmp7A9;void*_tmp7AF;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7AB)->tag == Cyc_Dict_Absent){_LLB9: _LLBA:
# 4639
({struct Cyc_String_pa_PrintArg_struct _tmp7AE=({struct Cyc_String_pa_PrintArg_struct _tmp9E8;_tmp9E8.tag=0U,({struct _dyneither_ptr _tmpC42=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7DF));_tmp9E8.f1=_tmpC42;});_tmp9E8;});void*_tmp7AC[1U];_tmp7AC[0]=& _tmp7AE;({unsigned int _tmpC44=loc;struct _dyneither_ptr _tmpC43=({const char*_tmp7AD="unbound typedef name %s";_tag_dyneither(_tmp7AD,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpC44,_tmpC43,_tag_dyneither(_tmp7AC,sizeof(void*),1U));});});
return cvtenv;}else{_LLBB: _tmp7AF=_tmp7AB;_LLBC:(int)_rethrow(_tmp7AF);}_LLB8:;}};}
# 4642
*_tmp7DD=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4647
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4652
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));({struct _tuple15*_tmpC45=({struct _tuple15*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));_tmp7B0->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp7B0->f2=(void*)ts->hd;_tmp7B0;});_tmp7B1->hd=_tmpC45;}),_tmp7B1->tl=inst;_tmp7B1;});}
# 4656
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7B4=({struct Cyc_String_pa_PrintArg_struct _tmp9E9;_tmp9E9.tag=0U,({struct _dyneither_ptr _tmpC46=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7DF));_tmp9E9.f1=_tmpC46;});_tmp9E9;});void*_tmp7B2[1U];_tmp7B2[0]=& _tmp7B4;({unsigned int _tmpC48=loc;struct _dyneither_ptr _tmpC47=({const char*_tmp7B3="too many parameters for typedef %s";_tag_dyneither(_tmp7B3,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpC48,_tmpC47,_tag_dyneither(_tmp7B2,sizeof(void*),1U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4661
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5->hd=e,_tmp7B5->tl=hidden_ts;_tmp7B5;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));({struct _tuple15*_tmpC49=({struct _tuple15*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp7B6->f2=e;_tmp7B6;});_tmp7B7->hd=_tmpC49;}),_tmp7B7->tl=inst;_tmp7B7;});}
# 4669
({struct Cyc_List_List*_tmpC4B=({struct Cyc_List_List*_tmpC4A=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC4A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp7DE=_tmpC4B;});}
# 4671
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp7DC=new_typ;}
# 4677
goto _LL0;};}}_LL0:;}
# 4679
if(!({struct Cyc_Absyn_Kind*_tmpC4C=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmpC4C,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp7E2=({struct Cyc_String_pa_PrintArg_struct _tmp9EC;_tmp9EC.tag=0U,({
struct _dyneither_ptr _tmpC4D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9EC.f1=_tmpC4D;});_tmp9EC;});struct Cyc_String_pa_PrintArg_struct _tmp7E3=({struct Cyc_String_pa_PrintArg_struct _tmp9EB;_tmp9EB.tag=0U,({struct _dyneither_ptr _tmpC4E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_type_kind(t)));_tmp9EB.f1=_tmpC4E;});_tmp9EB;});struct Cyc_String_pa_PrintArg_struct _tmp7E4=({struct Cyc_String_pa_PrintArg_struct _tmp9EA;_tmp9EA.tag=0U,({struct _dyneither_ptr _tmpC4F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(expected_kind));_tmp9EA.f1=_tmpC4F;});_tmp9EA;});void*_tmp7E0[3U];_tmp7E0[0]=& _tmp7E2,_tmp7E0[1]=& _tmp7E3,_tmp7E0[2]=& _tmp7E4;({unsigned int _tmpC51=loc;struct _dyneither_ptr _tmpC50=({const char*_tmp7E1="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp7E1,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpC51,_tmpC50,_tag_dyneither(_tmp7E0,sizeof(void*),3U));});});
# 4683
return cvtenv;}
# 4691
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4693
{void*_tmp7E5=e->r;void*_tmp7E6=_tmp7E5;struct Cyc_Absyn_Exp*_tmp7F9;struct Cyc_Absyn_Exp*_tmp7F8;void*_tmp7F7;void*_tmp7F6;void*_tmp7F5;void*_tmp7F4;struct Cyc_Absyn_Exp*_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F2;struct Cyc_Absyn_Exp*_tmp7F1;struct Cyc_Absyn_Exp*_tmp7F0;struct Cyc_Absyn_Exp*_tmp7EF;struct Cyc_Absyn_Exp*_tmp7EE;struct Cyc_Absyn_Exp*_tmp7ED;struct Cyc_Absyn_Exp*_tmp7EC;struct Cyc_Absyn_Exp*_tmp7EB;struct Cyc_Absyn_Exp*_tmp7EA;struct Cyc_List_List*_tmp7E9;switch(*((int*)_tmp7E6)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp7E9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7E6)->f2;_LLC:
# 4700
 for(0;_tmp7E9 != 0;_tmp7E9=_tmp7E9->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp7E9->hd,te,cvtenv);}
goto _LL0;case 6U: _LLD: _tmp7EC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_tmp7EB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E6)->f2;_tmp7EA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E6)->f3;_LLE:
# 4704
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EC,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EB,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EA,te,cvtenv);
goto _LL0;case 7U: _LLF: _tmp7EE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_tmp7ED=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E6)->f2;_LL10:
 _tmp7F0=_tmp7EE;_tmp7EF=_tmp7ED;goto _LL12;case 8U: _LL11: _tmp7F0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_tmp7EF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E6)->f2;_LL12:
 _tmp7F2=_tmp7F0;_tmp7F1=_tmp7EF;goto _LL14;case 9U: _LL13: _tmp7F2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_tmp7F1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E6)->f2;_LL14:
# 4711
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F2,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F1,te,cvtenv);
goto _LL0;case 14U: _LL15: _tmp7F4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_tmp7F3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7E6)->f2;_LL16:
# 4715
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F3,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7F4,1,0);
goto _LL0;case 19U: _LL17: _tmp7F5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_LL18:
 _tmp7F6=_tmp7F5;goto _LL1A;case 17U: _LL19: _tmp7F6=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_LL1A:
# 4720
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7F6,1,0);
goto _LL0;case 39U: _LL1B: _tmp7F7=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_LL1C:
# 4723
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp7F7,1,0);
goto _LL0;case 18U: _LL1D: _tmp7F8=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_LL1E:
# 4726
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F8,te,cvtenv);
goto _LL0;case 41U: _LL1F: _tmp7F9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp7E6)->f1;_LL20:
# 4729
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F9,te,cvtenv);
goto _LL0;default: _LL21: _LL22:
# 4732
({void*_tmp7E7=0U;({struct _dyneither_ptr _tmpC52=({const char*_tmp7E8="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp7E8,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC52,_tag_dyneither(_tmp7E7,sizeof(void*),0U));});});}_LL0:;}
# 4734
return cvtenv;}
# 4737
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4742
struct Cyc_List_List*_tmp7FA=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4745
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple29 _tmp7FB=*((struct _tuple29*)vs->hd);struct _tuple29 _tmp7FC=_tmp7FB;struct Cyc_Absyn_Tvar*_tmp7FD;_LL1: _tmp7FD=_tmp7FC.f1;_LL2:;
({struct Cyc_List_List*_tmpC53=Cyc_Tcutil_fast_add_free_tvar(_tmp7FA,_tmp7FD);cvt.kind_env=_tmpC53;});}}
# 4753
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple30 _tmp7FE=*((struct _tuple30*)evs->hd);struct _tuple30 _tmp7FF=_tmp7FE;void*_tmp808;_LL4: _tmp808=_tmp7FF.f1;_LL5:;{
void*_tmp800=Cyc_Tcutil_compress(_tmp808);void*_tmp801=_tmp800;struct Cyc_Core_Opt**_tmp807;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp801)->tag == 1U){_LL7: _tmp807=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp801)->f4;_LL8:
# 4757
 if(*_tmp807 == 0)
({struct Cyc_Core_Opt*_tmpC54=({struct Cyc_Core_Opt*_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802->v=_tmp7FA;_tmp802;});*_tmp807=_tmpC54;});else{
# 4761
struct Cyc_List_List*_tmp803=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp807))->v;
struct Cyc_List_List*_tmp804=0;
for(0;_tmp803 != 0;_tmp803=_tmp803->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp7FA,(struct Cyc_Absyn_Tvar*)_tmp803->hd))
_tmp804=({struct Cyc_List_List*_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805->hd=(struct Cyc_Absyn_Tvar*)_tmp803->hd,_tmp805->tl=_tmp804;_tmp805;});}
({struct Cyc_Core_Opt*_tmpC55=({struct Cyc_Core_Opt*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->v=_tmp804;_tmp806;});*_tmp807=_tmpC55;});}
# 4768
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4772
return cvt;}
# 4779
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
{void*_tmp809=Cyc_Tcutil_compress(e);void*_tmp80A=_tmp809;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp80A)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
# 4785
 continue;}_LL0:;}{
# 4787
struct Cyc_Absyn_Kind*_tmp80B=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_tmp80C=_tmp80B;switch(((struct Cyc_Absyn_Kind*)_tmp80C)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp80C)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
# 4789
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_unique_rgn_type))
({void*_tmp80D=0U;({struct _dyneither_ptr _tmpC56=({const char*_tmp80E="can't unify evar with unique region!";_tag_dyneither(_tmp80E,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC56,_tag_dyneither(_tmp80D,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Top: _LLA: _LLB:
# 4794
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_heap_rgn_type))({void*_tmp80F=0U;({struct _dyneither_ptr _tmpC57=({const char*_tmp810="can't unify evar with heap!";_tag_dyneither(_tmp810,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC57,_tag_dyneither(_tmp80F,sizeof(void*),0U));});});
goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 4797
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp811=0U;({struct _dyneither_ptr _tmpC58=({const char*_tmp812="can't unify evar with {}!";_tag_dyneither(_tmp812,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC58,_tag_dyneither(_tmp811,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
# 4800
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_false_type))({struct Cyc_String_pa_PrintArg_struct _tmp815=({struct Cyc_String_pa_PrintArg_struct _tmp9ED;_tmp9ED.tag=0U,({
struct _dyneither_ptr _tmpC59=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmp9ED.f1=_tmpC59;});_tmp9ED;});void*_tmp813[1U];_tmp813[0]=& _tmp815;({struct _dyneither_ptr _tmpC5A=({const char*_tmp814="can't unify evar %s with @false!";_tag_dyneither(_tmp814,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5A,_tag_dyneither(_tmp813,sizeof(void*),1U));});});
goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
# 4804
 if(!({void*_tmpC5B=e;Cyc_Tcutil_unify(_tmpC5B,Cyc_Absyn_bounds_one());}))({void*_tmp816=0U;({struct _dyneither_ptr _tmpC5C=({const char*_tmp817="can't unify evar with bounds_one!";_tag_dyneither(_tmp817,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5C,_tag_dyneither(_tmp816,sizeof(void*),0U));});});
goto _LL5;default: _LL12: _LL13:
# 4807
({struct Cyc_String_pa_PrintArg_struct _tmp81A=({struct Cyc_String_pa_PrintArg_struct _tmp9EF;_tmp9EF.tag=0U,({
struct _dyneither_ptr _tmpC5D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmp9EF.f1=_tmpC5D;});_tmp9EF;});struct Cyc_String_pa_PrintArg_struct _tmp81B=({struct Cyc_String_pa_PrintArg_struct _tmp9EE;_tmp9EE.tag=0U,({struct _dyneither_ptr _tmpC5E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(in_t));_tmp9EE.f1=_tmpC5E;});_tmp9EE;});void*_tmp818[2U];_tmp818[0]=& _tmp81A,_tmp818[1]=& _tmp81B;({unsigned int _tmpC60=loc;struct _dyneither_ptr _tmpC5F=({const char*_tmp819="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp819,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpC60,_tmpC5F,_tag_dyneither(_tmp818,sizeof(void*),2U));});});
goto _LL5;}_LL5:;};}}
# 4819
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
if(te->in_tempest  && !te->tempest_generalize)generalize_evars=0;{
struct Cyc_List_List*_tmp81C=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp81D=({unsigned int _tmpC64=loc;struct Cyc_Tcenv_Tenv*_tmpC63=te;struct Cyc_Tcutil_CVTEnv _tmpC62=({struct Cyc_Tcutil_CVTEnv _tmp9F3;_tmp9F3.r=Cyc_Core_heap_region,_tmp9F3.kind_env=_tmp81C,_tmp9F3.free_vars=0,_tmp9F3.free_evars=0,_tmp9F3.generalize_evars=generalize_evars,_tmp9F3.fn_result=0;_tmp9F3;});struct Cyc_Absyn_Kind*_tmpC61=expected_kind;Cyc_Tcutil_check_valid_type(_tmpC64,_tmpC63,_tmpC62,_tmpC61,1,t);});
# 4828
struct Cyc_List_List*_tmp81E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,_tmp81D.free_vars);
struct Cyc_List_List*_tmp81F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple30*))Cyc_Core_fst,_tmp81D.free_evars);
# 4832
if(_tmp81C != 0){
struct Cyc_List_List*_tmp820=0;
{struct Cyc_List_List*_tmp821=_tmp81E;for(0;(unsigned int)_tmp821;_tmp821=_tmp821->tl){
struct Cyc_Absyn_Tvar*_tmp822=(struct Cyc_Absyn_Tvar*)_tmp821->hd;
int found=0;
{struct Cyc_List_List*_tmp823=_tmp81C;for(0;(unsigned int)_tmp823;_tmp823=_tmp823->tl){
if(Cyc_Absyn_tvar_cmp(_tmp822,(struct Cyc_Absyn_Tvar*)_tmp823->hd)== 0){found=1;break;}}}
if(!found)
_tmp820=({struct Cyc_List_List*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824->hd=(struct Cyc_Absyn_Tvar*)_tmp821->hd,_tmp824->tl=_tmp820;_tmp824;});}}
# 4842
_tmp81E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp820);}
# 4848
{struct Cyc_List_List*x=_tmp81E;for(0;x != 0;x=x->tl){
void*_tmp825=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp826=_tmp825;enum Cyc_Absyn_AliasQual _tmp834;struct Cyc_Core_Opt**_tmp833;struct Cyc_Absyn_Kind*_tmp832;struct Cyc_Core_Opt**_tmp831;struct Cyc_Core_Opt**_tmp830;struct Cyc_Core_Opt**_tmp82F;struct Cyc_Core_Opt**_tmp82E;struct Cyc_Core_Opt**_tmp82D;struct Cyc_Core_Opt**_tmp82C;switch(*((int*)_tmp826)){case 1U: _LL1: _tmp82C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp826)->f1;_LL2:
 _tmp82D=_tmp82C;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp82D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f1;_LL4:
 _tmp82E=_tmp82D;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp82E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f1;_LL6:
 _tmp82F=_tmp82E;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp82F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f1;_LL8:
# 4854
({struct Cyc_Core_Opt*_tmpC65=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp82F=_tmpC65;});goto _LL0;case Cyc_Absyn_Unique: _LL9: _tmp830=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f1;_LLA:
# 4856
({struct Cyc_Core_Opt*_tmpC66=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp830=_tmpC66;});goto _LL0;default: goto _LLD;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp831=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f1;_LLC:
# 4858
({struct Cyc_Core_Opt*_tmpC67=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp831=_tmpC67;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp833=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f1;_tmp832=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp826)->f2;_LLE:
# 4860
({struct Cyc_Core_Opt*_tmpC68=Cyc_Tcutil_kind_to_bound_opt(_tmp832);*_tmp833=_tmpC68;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp826)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp834=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp826)->f1)->aliasqual;_LL10:
# 4862
({struct Cyc_String_pa_PrintArg_struct _tmp829=({struct Cyc_String_pa_PrintArg_struct _tmp9F1;_tmp9F1.tag=0U,({
struct _dyneither_ptr _tmpC69=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmp9F1.f1=_tmpC69;});_tmp9F1;});struct Cyc_String_pa_PrintArg_struct _tmp82A=({struct Cyc_String_pa_PrintArg_struct _tmp9F0;_tmp9F0.tag=0U,({struct _dyneither_ptr _tmpC6A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B->kind=Cyc_Absyn_MemKind,_tmp82B->aliasqual=_tmp834;_tmp82B;})));_tmp9F0.f1=_tmpC6A;});_tmp9F0;});void*_tmp827[2U];_tmp827[0]=& _tmp829,_tmp827[1]=& _tmp82A;({unsigned int _tmpC6C=loc;struct _dyneither_ptr _tmpC6B=({const char*_tmp828="type variable %s cannot have kind %s";_tag_dyneither(_tmp828,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpC6C,_tmpC6B,_tag_dyneither(_tmp827,sizeof(void*),2U));});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4869
if(_tmp81E != 0  || _tmp81F != 0){
{void*_tmp835=Cyc_Tcutil_compress(t);void*_tmp836=_tmp835;struct Cyc_List_List**_tmp837;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp836)->tag == 5U){_LL14: _tmp837=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp836)->f1).tvars;_LL15:
# 4872
 if(*_tmp837 == 0){
# 4874
({struct Cyc_List_List*_tmpC6D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp81E);*_tmp837=_tmpC6D;});
_tmp81E=0;}
# 4877
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4880
if(_tmp81E != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp83A=({struct Cyc_String_pa_PrintArg_struct _tmp9F2;_tmp9F2.tag=0U,_tmp9F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp81E->hd)->name);_tmp9F2;});void*_tmp838[1U];_tmp838[0]=& _tmp83A;({unsigned int _tmpC6F=loc;struct _dyneither_ptr _tmpC6E=({const char*_tmp839="unbound type variable %s";_tag_dyneither(_tmp839,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpC6F,_tmpC6E,_tag_dyneither(_tmp838,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_function_type(t) || !te->in_tempest)
Cyc_Tcutil_check_free_evars(_tmp81F,t,loc);}};}
# 4891
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 4894
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp83B=Cyc_Tcutil_compress(t);void*_tmp83C=_tmp83B;struct Cyc_Absyn_FnInfo _tmp83F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83C)->tag == 5U){_LL1: _tmp83F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83C)->f1;_LL2:
# 4897
 fd->i=_tmp83F;
({int _tmpC70=
Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp83F.ret_tqual).print_const,_tmp83F.ret_type);
# 4898
((fd->i).ret_tqual).real_const=_tmpC70;});
# 4900
goto _LL0;}else{_LL3: _LL4:
({void*_tmp83D=0U;({struct _dyneither_ptr _tmpC71=({const char*_tmp83E="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp83E,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC71,_tag_dyneither(_tmp83D,sizeof(void*),0U));});});}_LL0:;}{
# 4903
struct _RegionHandle _tmp840=_new_region("r");struct _RegionHandle*r=& _tmp840;_push_region(r);
({struct Cyc_List_List*_tmpC73=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,(fd->i).args);unsigned int _tmpC72=loc;Cyc_Tcutil_check_unique_vars(_tmpC73,_tmpC72,({const char*_tmp841="function declaration has repeated parameter";_tag_dyneither(_tmp841,sizeof(char),44U);}));});
# 4907
fd->cached_type=t;
# 4904
;_pop_region(r);};}
# 4912
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4918
struct Cyc_Tcutil_CVTEnv _tmp842=({unsigned int _tmpC78=loc;struct Cyc_Tcenv_Tenv*_tmpC77=te;struct Cyc_Tcutil_CVTEnv _tmpC76=({struct Cyc_Tcutil_CVTEnv _tmp9F6;_tmp9F6.r=Cyc_Core_heap_region,_tmp9F6.kind_env=bound_tvars,_tmp9F6.free_vars=0,_tmp9F6.free_evars=0,_tmp9F6.generalize_evars=0,_tmp9F6.fn_result=0;_tmp9F6;});struct Cyc_Absyn_Kind*_tmpC75=expected_kind;int _tmpC74=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpC78,_tmpC77,_tmpC76,_tmpC75,_tmpC74,t);});
# 4922
struct Cyc_List_List*_tmp843=({struct _RegionHandle*_tmpC7A=Cyc_Core_heap_region;struct Cyc_List_List*_tmpC79=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,_tmp842.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpC7A,_tmpC79,bound_tvars);});
# 4924
struct Cyc_List_List*_tmp844=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple30*))Cyc_Core_fst,_tmp842.free_evars);
{struct Cyc_List_List*fs=_tmp843;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp845=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp848=({struct Cyc_String_pa_PrintArg_struct _tmp9F5;_tmp9F5.tag=0U,_tmp9F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp845);_tmp9F5;});struct Cyc_String_pa_PrintArg_struct _tmp849=({struct Cyc_String_pa_PrintArg_struct _tmp9F4;_tmp9F4.tag=0U,({struct _dyneither_ptr _tmpC7B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9F4.f1=_tmpC7B;});_tmp9F4;});void*_tmp846[2U];_tmp846[0]=& _tmp848,_tmp846[1]=& _tmp849;({unsigned int _tmpC7D=loc;struct _dyneither_ptr _tmpC7C=({const char*_tmp847="unbound type variable %s in type %s";_tag_dyneither(_tmp847,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpC7D,_tmpC7C,_tag_dyneither(_tmp846,sizeof(void*),2U));});});}}
# 4929
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp844,t,loc);}
# 4933
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpC7E=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpC7E;});}
# 4938
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4944
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4947
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp84C=({struct Cyc_String_pa_PrintArg_struct _tmp9F8;_tmp9F8.tag=0U,_tmp9F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmp9F8;});struct Cyc_String_pa_PrintArg_struct _tmp84D=({struct Cyc_String_pa_PrintArg_struct _tmp9F7;_tmp9F7.tag=0U,({struct _dyneither_ptr _tmpC7F=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmp9F7.f1=_tmpC7F;});_tmp9F7;});void*_tmp84A[2U];_tmp84A[0]=& _tmp84C,_tmp84A[1]=& _tmp84D;({unsigned int _tmpC81=loc;struct _dyneither_ptr _tmpC80=({const char*_tmp84B="%s: %s";_tag_dyneither(_tmp84B,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpC81,_tmpC80,_tag_dyneither(_tmp84A,sizeof(void*),2U));});});}}}
# 4953
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4957
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4961
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpC83=tvs;unsigned int _tmpC82=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpC83,_tmpC82,Cyc_Absynpp_tvar2string,({const char*_tmp84E="duplicate type variable";_tag_dyneither(_tmp84E,sizeof(char),24U);}));});}struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple34{struct Cyc_List_List*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4975 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4980
struct _RegionHandle _tmp84F=_new_region("temp");struct _RegionHandle*temp=& _tmp84F;_push_region(temp);
# 4984
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpC84=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpC84,({const char*_tmp850="";_tag_dyneither(_tmp850,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp852=_region_malloc(temp,sizeof(*_tmp852));({struct _tuple33*_tmpC85=({struct _tuple33*_tmp851=_region_malloc(temp,sizeof(*_tmp851));_tmp851->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp851->f2=0;_tmp851;});_tmp852->hd=_tmpC85;}),_tmp852->tl=fields;_tmp852;});}}
# 4989
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4991
struct _dyneither_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp877="struct";_tag_dyneither(_tmp877,sizeof(char),7U);}):({const char*_tmp878="union";_tag_dyneither(_tmp878,sizeof(char),6U);});
# 4994
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple34*_tmp853=(struct _tuple34*)des->hd;struct _tuple34*_tmp854=_tmp853;struct Cyc_List_List*_tmp86F;void*_tmp86E;_LL1: _tmp86F=_tmp854->f1;_tmp86E=_tmp854->f2;_LL2:;
if(_tmp86F == 0){
# 4999
struct Cyc_List_List*_tmp855=fields;
for(0;_tmp855 != 0;_tmp855=_tmp855->tl){
if(!(*((struct _tuple33*)_tmp855->hd)).f2){
(*((struct _tuple33*)_tmp855->hd)).f2=1;
({struct Cyc_List_List*_tmpC87=({struct Cyc_List_List*_tmp857=_cycalloc(sizeof(*_tmp857));({void*_tmpC86=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp856=_cycalloc(sizeof(*_tmp856));_tmp856->tag=1U,_tmp856->f1=((*((struct _tuple33*)_tmp855->hd)).f1)->name;_tmp856;});_tmp857->hd=_tmpC86;}),_tmp857->tl=0;_tmp857;});(*((struct _tuple34*)des->hd)).f1=_tmpC87;});
ans=({struct Cyc_List_List*_tmp859=_region_malloc(rgn,sizeof(*_tmp859));({struct _tuple35*_tmpC88=({struct _tuple35*_tmp858=_region_malloc(rgn,sizeof(*_tmp858));_tmp858->f1=(*((struct _tuple33*)_tmp855->hd)).f1,_tmp858->f2=_tmp86E;_tmp858;});_tmp859->hd=_tmpC88;}),_tmp859->tl=ans;_tmp859;});
break;}}
# 5007
if(_tmp855 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp85C=({struct Cyc_String_pa_PrintArg_struct _tmp9F9;_tmp9F9.tag=0U,_tmp9F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp9F9;});void*_tmp85A[1U];_tmp85A[0]=& _tmp85C;({unsigned int _tmpC8A=loc;struct _dyneither_ptr _tmpC89=({const char*_tmp85B="too many arguments to %s";_tag_dyneither(_tmp85B,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpC8A,_tmpC89,_tag_dyneither(_tmp85A,sizeof(void*),1U));});});}else{
if(_tmp86F->tl != 0)
# 5011
({void*_tmp85D=0U;({unsigned int _tmpC8C=loc;struct _dyneither_ptr _tmpC8B=({const char*_tmp85E="multiple designators are not yet supported";_tag_dyneither(_tmp85E,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpC8C,_tmpC8B,_tag_dyneither(_tmp85D,sizeof(void*),0U));});});else{
# 5014
void*_tmp85F=(void*)_tmp86F->hd;void*_tmp860=_tmp85F;struct _dyneither_ptr*_tmp86D;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp860)->tag == 0U){_LL4: _LL5:
# 5016
({struct Cyc_String_pa_PrintArg_struct _tmp863=({struct Cyc_String_pa_PrintArg_struct _tmp9FA;_tmp9FA.tag=0U,_tmp9FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp9FA;});void*_tmp861[1U];_tmp861[0]=& _tmp863;({unsigned int _tmpC8E=loc;struct _dyneither_ptr _tmpC8D=({const char*_tmp862="array designator used in argument to %s";_tag_dyneither(_tmp862,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC8E,_tmpC8D,_tag_dyneither(_tmp861,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp86D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp860)->f1;_LL7: {
# 5019
struct Cyc_List_List*_tmp864=fields;
for(0;_tmp864 != 0;_tmp864=_tmp864->tl){
if(Cyc_strptrcmp(_tmp86D,((*((struct _tuple33*)_tmp864->hd)).f1)->name)== 0){
if((*((struct _tuple33*)_tmp864->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp867=({struct Cyc_String_pa_PrintArg_struct _tmp9FB;_tmp9FB.tag=0U,_tmp9FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp86D);_tmp9FB;});void*_tmp865[1U];_tmp865[0]=& _tmp867;({unsigned int _tmpC90=loc;struct _dyneither_ptr _tmpC8F=({const char*_tmp866="member %s has already been used as an argument";_tag_dyneither(_tmp866,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpC90,_tmpC8F,_tag_dyneither(_tmp865,sizeof(void*),1U));});});
(*((struct _tuple33*)_tmp864->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp869=_region_malloc(rgn,sizeof(*_tmp869));({struct _tuple35*_tmpC91=({struct _tuple35*_tmp868=_region_malloc(rgn,sizeof(*_tmp868));_tmp868->f1=(*((struct _tuple33*)_tmp864->hd)).f1,_tmp868->f2=_tmp86E;_tmp868;});_tmp869->hd=_tmpC91;}),_tmp869->tl=ans;_tmp869;});
break;}}
# 5028
if(_tmp864 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp86C=({struct Cyc_String_pa_PrintArg_struct _tmp9FC;_tmp9FC.tag=0U,_tmp9FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp86D);_tmp9FC;});void*_tmp86A[1U];_tmp86A[0]=& _tmp86C;({unsigned int _tmpC93=loc;struct _dyneither_ptr _tmpC92=({const char*_tmp86B="bad field designator %s";_tag_dyneither(_tmp86B,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpC93,_tmpC92,_tag_dyneither(_tmp86A,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 5033
if((int)aggr_kind == (int)0U)
# 5035
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple33*)fields->hd)).f2){
({void*_tmp870=0U;({unsigned int _tmpC95=loc;struct _dyneither_ptr _tmpC94=({const char*_tmp871="too few arguments to struct";_tag_dyneither(_tmp871,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpC95,_tmpC94,_tag_dyneither(_tmp870,sizeof(void*),0U));});});
break;}}else{
# 5042
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple33*)fields->hd)).f2){
if(found)({void*_tmp872=0U;({unsigned int _tmpC97=loc;struct _dyneither_ptr _tmpC96=({const char*_tmp873="only one member of a union is allowed";_tag_dyneither(_tmp873,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpC97,_tmpC96,_tag_dyneither(_tmp872,sizeof(void*),0U));});});
found=1;}}
# 5049
if(!found)({void*_tmp874=0U;({unsigned int _tmpC99=loc;struct _dyneither_ptr _tmpC98=({const char*_tmp875="missing member for union";_tag_dyneither(_tmp875,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpC99,_tmpC98,_tag_dyneither(_tmp874,sizeof(void*),0U));});});}{
# 5052
struct Cyc_List_List*_tmp876=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp876;};};}
# 4984
;_pop_region(temp);}
# 5058
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5060
void*_tmp879=e1->r;void*_tmp87A=_tmp879;struct Cyc_Absyn_Exp*_tmp88C;struct Cyc_Absyn_Exp*_tmp88B;struct Cyc_Absyn_Exp*_tmp88A;struct Cyc_Absyn_Exp*_tmp889;struct Cyc_Absyn_Exp*_tmp888;struct Cyc_Absyn_Exp*_tmp887;switch(*((int*)_tmp87A)){case 14U: _LL1: _LL2:
# 5062
({struct Cyc_String_pa_PrintArg_struct _tmp87D=({struct Cyc_String_pa_PrintArg_struct _tmp9FD;_tmp9FD.tag=0U,({struct _dyneither_ptr _tmpC9A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9FD.f1=_tmpC9A;});_tmp9FD;});void*_tmp87B[1U];_tmp87B[0]=& _tmp87D;({struct _dyneither_ptr _tmpC9B=({const char*_tmp87C="we have a cast in a lhs:  %s";_tag_dyneither(_tmp87C,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC9B,_tag_dyneither(_tmp87B,sizeof(void*),1U));});});case 20U: _LL3: _tmp887=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp87A)->f1;_LL4:
 _tmp888=_tmp887;goto _LL6;case 23U: _LL5: _tmp888=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp87A)->f1;_LL6:
# 5065
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp888->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp889=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp87A)->f1;_LL8:
 _tmp88A=_tmp889;goto _LLA;case 21U: _LL9: _tmp88A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp87A)->f1;_LLA:
# 5069
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp88A->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp880=({struct Cyc_String_pa_PrintArg_struct _tmp9FE;_tmp9FE.tag=0U,({
struct _dyneither_ptr _tmpC9C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9FE.f1=_tmpC9C;});_tmp9FE;});void*_tmp87E[1U];_tmp87E[0]=& _tmp880;({struct _dyneither_ptr _tmpC9D=({const char*_tmp87F="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp87F,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC9D,_tag_dyneither(_tmp87E,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp88B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp87A)->f1;_LLC:
 _tmp88C=_tmp88B;goto _LLE;case 12U: _LLD: _tmp88C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp87A)->f1;_LLE:
# 5075
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp88C->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp883=({struct Cyc_String_pa_PrintArg_struct _tmp9FF;_tmp9FF.tag=0U,({
struct _dyneither_ptr _tmpC9E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9FF.f1=_tmpC9E;});_tmp9FF;});void*_tmp881[1U];_tmp881[0]=& _tmp883;({struct _dyneither_ptr _tmpC9F=({const char*_tmp882="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp882,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC9F,_tag_dyneither(_tmp881,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp886=({struct Cyc_String_pa_PrintArg_struct _tmpA00;_tmpA00.tag=0U,({struct _dyneither_ptr _tmpCA0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA00.f1=_tmpCA0;});_tmpA00;});void*_tmp884[1U];_tmp884[0]=& _tmp886;({struct _dyneither_ptr _tmpCA1=({const char*_tmp885="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp885,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCA1,_tag_dyneither(_tmp884,sizeof(void*),1U));});});}_LL0:;}
# 5090
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5093
void*_tmp88D=Cyc_Tcutil_compress(r);void*_tmp88E=_tmp88D;struct Cyc_Absyn_Tvar*_tmp89A;enum Cyc_Absyn_KindQual _tmp899;enum Cyc_Absyn_AliasQual _tmp898;switch(*((int*)_tmp88E)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88E)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88E)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88E)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88E)->f4 == 0){_LL5: _tmp899=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88E)->f3)->kind)->v)->kind;_tmp898=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88E)->f3)->kind)->v)->aliasqual;_LL6:
# 5097
 return(int)_tmp899 == (int)3U  && ((int)_tmp898 == (int)1U  || (int)_tmp898 == (int)2U  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp89A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp88E)->f1;_LL8: {
# 5101
struct Cyc_Absyn_Kind*_tmp88F=Cyc_Tcutil_tvar_kind(_tmp89A,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp890=_tmp88F;enum Cyc_Absyn_KindQual _tmp897;enum Cyc_Absyn_AliasQual _tmp896;_LLC: _tmp897=_tmp890->kind;_tmp896=_tmp890->aliasqual;_LLD:;
if((int)_tmp897 == (int)3U  && ((int)_tmp896 == (int)1U  || (int)_tmp896 == (int)2U  && !must_be_unique)){
void*_tmp891=Cyc_Absyn_compress_kb(_tmp89A->kind);void*_tmp892=_tmp891;struct Cyc_Core_Opt**_tmp895;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp892)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp892)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp892)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp895=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp892)->f1;_LL10:
# 5105
({struct Cyc_Core_Opt*_tmpCA3=({struct Cyc_Core_Opt*_tmp894=_cycalloc(sizeof(*_tmp894));({void*_tmpCA2=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893->tag=2U,_tmp893->f1=0,_tmp893->f2=& Cyc_Tcutil_rk;_tmp893;});_tmp894->v=_tmpCA2;});_tmp894;});*_tmp895=_tmpCA3;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5110
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5117
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp89B=Cyc_Tcutil_compress(t);void*_tmp89C=_tmp89B;struct Cyc_Absyn_Tvar*_tmp8AA;void*_tmp8A9;switch(*((int*)_tmp89C)){case 3U: _LL1: _tmp8A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp89C)->f1).ptr_atts).rgn;_LL2:
# 5120
 return Cyc_Tcutil_is_noalias_region(_tmp8A9,must_be_unique);case 2U: _LL3: _tmp8AA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp89C)->f1;_LL4: {
# 5122
struct Cyc_Absyn_Kind*_tmp89D=Cyc_Tcutil_tvar_kind(_tmp8AA,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp89E=_tmp89D;enum Cyc_Absyn_KindQual _tmp8A8;enum Cyc_Absyn_AliasQual _tmp8A7;_LL8: _tmp8A8=_tmp89E->kind;_tmp8A7=_tmp89E->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp89F=_tmp8A8;switch(_tmp89F){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)_tmp8A7 == (int)1U  || (int)_tmp8A7 == (int)2U  && !must_be_unique){
void*_tmp8A0=Cyc_Absyn_compress_kb(_tmp8AA->kind);void*_tmp8A1=_tmp8A0;struct Cyc_Core_Opt**_tmp8A6;enum Cyc_Absyn_KindQual _tmp8A5;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A1)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A1)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp8A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A1)->f1;_tmp8A5=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A1)->f2)->kind;_LL15:
# 5128
({struct Cyc_Core_Opt*_tmpCA6=({struct Cyc_Core_Opt*_tmp8A4=_cycalloc(sizeof(*_tmp8A4));({void*_tmpCA5=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8A3=_cycalloc(sizeof(*_tmp8A3));_tmp8A3->tag=2U,_tmp8A3->f1=0,({struct Cyc_Absyn_Kind*_tmpCA4=({struct Cyc_Absyn_Kind*_tmp8A2=_cycalloc(sizeof(*_tmp8A2));_tmp8A2->kind=_tmp8A5,_tmp8A2->aliasqual=Cyc_Absyn_Aliasable;_tmp8A2;});_tmp8A3->f2=_tmpCA4;});_tmp8A3;});_tmp8A4->v=_tmpCA5;});_tmp8A4;});*_tmp8A6=_tmpCA6;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5132
 return 1;}_LL13:;}
# 5135
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5138
 return 0;}_LL0:;}
# 5141
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp8AB=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp8AB,0))return 1;{
void*_tmp8AC=_tmp8AB;struct Cyc_List_List*_tmp8C0;union Cyc_Absyn_DatatypeFieldInfo _tmp8BF;struct Cyc_List_List*_tmp8BE;union Cyc_Absyn_DatatypeInfo _tmp8BD;struct Cyc_List_List*_tmp8BC;struct Cyc_Absyn_Aggrdecl**_tmp8BB;struct Cyc_List_List*_tmp8BA;struct Cyc_List_List*_tmp8B9;switch(*((int*)_tmp8AC)){case 6U: _LL1: _tmp8B9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8AC)->f1;_LL2:
# 5146
 while(_tmp8B9 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp8B9->hd)).f2))return 1;
_tmp8B9=_tmp8B9->tl;}
# 5150
return 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AC)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AC)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp8BB=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AC)->f1)->f1).KnownAggr).val;_tmp8BA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AC)->f2;_LL4:
# 5152
 if((*_tmp8BB)->impl == 0)return 0;else{
# 5154
struct Cyc_List_List*_tmp8AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp8BB)->tvs,_tmp8BA);
struct Cyc_List_List*_tmp8AE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp8BB)->impl))->fields;
void*t;
while(_tmp8AE != 0){
t=_tmp8AD == 0?((struct Cyc_Absyn_Aggrfield*)_tmp8AE->hd)->type: Cyc_Tcutil_substitute(_tmp8AD,((struct Cyc_Absyn_Aggrfield*)_tmp8AE->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp8AE=_tmp8AE->tl;}
# 5162
return 0;}}else{_LL7: _LL8:
# 5172
 return 0;}case 18U: _LL9: _tmp8BD=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AC)->f1)->f1;_tmp8BC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AC)->f2;_LLA: {
# 5174
union Cyc_Absyn_DatatypeInfo _tmp8AF=_tmp8BD;struct Cyc_List_List*_tmp8B3;struct Cyc_Core_Opt*_tmp8B2;struct _tuple2*_tmp8B1;int _tmp8B0;if((_tmp8AF.UnknownDatatype).tag == 1){_LL10: _tmp8B1=((_tmp8AF.UnknownDatatype).val).name;_tmp8B0=((_tmp8AF.UnknownDatatype).val).is_extensible;_LL11:
# 5177
 return 0;}else{_LL12: _tmp8B3=(*(_tmp8AF.KnownDatatype).val)->tvs;_tmp8B2=(*(_tmp8AF.KnownDatatype).val)->fields;_LL13:
# 5180
 return 0;}_LLF:;}case 19U: _LLB: _tmp8BF=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AC)->f1)->f1;_tmp8BE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AC)->f2;_LLC: {
# 5183
union Cyc_Absyn_DatatypeFieldInfo _tmp8B4=_tmp8BF;struct Cyc_Absyn_Datatypedecl*_tmp8B8;struct Cyc_Absyn_Datatypefield*_tmp8B7;if((_tmp8B4.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5186
 return 0;}else{_LL17: _tmp8B8=((_tmp8B4.KnownDatatypefield).val).f1;_tmp8B7=((_tmp8B4.KnownDatatypefield).val).f2;_LL18: {
# 5188
struct Cyc_List_List*_tmp8B5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp8B8->tvs,_tmp8BE);
struct Cyc_List_List*_tmp8B6=_tmp8B7->typs;
while(_tmp8B6 != 0){
_tmp8AB=_tmp8B5 == 0?(*((struct _tuple12*)_tmp8B6->hd)).f2: Cyc_Tcutil_substitute(_tmp8B5,(*((struct _tuple12*)_tmp8B6->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp8AB))return 1;
_tmp8B6=_tmp8B6->tl;}
# 5195
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp8C0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8AC)->f2;_LL6:
# 5165
 while(_tmp8C0 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp8C0->hd)->type))return 1;
_tmp8C0=_tmp8C0->tl;}
# 5169
return 0;default: _LLD: _LLE:
# 5197
 return 0;}_LL0:;};}
# 5204
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp8C1=e->r;void*_tmp8C2=_tmp8C1;struct Cyc_Absyn_Stmt*_tmp8D8;struct Cyc_Absyn_Exp*_tmp8D7;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Exp*_tmp8D5;struct Cyc_Absyn_Exp*_tmp8D4;struct Cyc_Absyn_Exp*_tmp8D3;struct Cyc_Absyn_Exp*_tmp8D2;struct Cyc_Absyn_Exp*_tmp8D1;struct _dyneither_ptr*_tmp8D0;struct Cyc_Absyn_Exp*_tmp8CF;struct Cyc_Absyn_Exp*_tmp8CE;switch(*((int*)_tmp8C2)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8C2)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp8CE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8C2)->f1;_LL4:
 _tmp8CF=_tmp8CE;goto _LL6;case 20U: _LL5: _tmp8CF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8C2)->f1;_LL6:
# 5209
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp8CF->topt),1) && Cyc_Tcutil_is_noalias_path(_tmp8CF);case 21U: _LL7: _tmp8D1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8C2)->f1;_tmp8D0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8C2)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp8D1);case 23U: _LL9: _tmp8D3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8C2)->f1;_tmp8D2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8C2)->f2;_LLA: {
# 5212
void*_tmp8C3=Cyc_Tcutil_compress((void*)_check_null(_tmp8D3->topt));void*_tmp8C4=_tmp8C3;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8C4)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp8D3);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp8D5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8C2)->f2;_tmp8D4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8C2)->f3;_LLC:
# 5217
 return Cyc_Tcutil_is_noalias_path(_tmp8D5) && Cyc_Tcutil_is_noalias_path(_tmp8D4);case 9U: _LLD: _tmp8D6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8C2)->f2;_LLE:
 _tmp8D7=_tmp8D6;goto _LL10;case 14U: _LLF: _tmp8D7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8C2)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp8D7);case 37U: _LL11: _tmp8D8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8C2)->f1;_LL12:
# 5221
 while(1){
void*_tmp8C5=_tmp8D8->r;void*_tmp8C6=_tmp8C5;struct Cyc_Absyn_Exp*_tmp8CD;struct Cyc_Absyn_Decl*_tmp8CC;struct Cyc_Absyn_Stmt*_tmp8CB;struct Cyc_Absyn_Stmt*_tmp8CA;struct Cyc_Absyn_Stmt*_tmp8C9;switch(*((int*)_tmp8C6)){case 2U: _LL1B: _tmp8CA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8C6)->f1;_tmp8C9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8C6)->f2;_LL1C:
 _tmp8D8=_tmp8C9;goto _LL1A;case 12U: _LL1D: _tmp8CC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8C6)->f1;_tmp8CB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8C6)->f2;_LL1E:
 _tmp8D8=_tmp8CB;goto _LL1A;case 1U: _LL1F: _tmp8CD=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8C6)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp8CD);default: _LL21: _LL22:
({void*_tmp8C7=0U;({struct _dyneither_ptr _tmpCA7=({const char*_tmp8C8="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmp8C8,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCA7,_tag_dyneither(_tmp8C7,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5229
 return 1;}_LL0:;}
# 5246 "tcutil.cyc"
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5248
struct _tuple17 bogus_ans=({struct _tuple17 _tmpA0D;_tmpA0D.f1=0,_tmpA0D.f2=Cyc_Absyn_heap_rgn_type;_tmpA0D;});
void*_tmp8D9=e->r;void*_tmp8DA=_tmp8D9;struct Cyc_Absyn_Exp*_tmp913;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_Absyn_Exp*_tmp911;struct Cyc_Absyn_Exp*_tmp910;struct _dyneither_ptr*_tmp90F;int _tmp90E;struct Cyc_Absyn_Exp*_tmp90D;struct _dyneither_ptr*_tmp90C;int _tmp90B;void*_tmp90A;switch(*((int*)_tmp8DA)){case 1U: _LL1: _tmp90A=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL2: {
# 5252
void*_tmp8DB=_tmp90A;struct Cyc_Absyn_Vardecl*_tmp8E3;struct Cyc_Absyn_Vardecl*_tmp8E2;struct Cyc_Absyn_Vardecl*_tmp8E1;struct Cyc_Absyn_Vardecl*_tmp8E0;switch(*((int*)_tmp8DB)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp8E0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp8DB)->f1;_LL13: {
# 5256
void*_tmp8DC=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8DD=_tmp8DC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DD)->tag == 4U){_LL1B: _LL1C:
# 5258
 return({struct _tuple17 _tmpA01;_tmpA01.f1=1,_tmpA01.f2=Cyc_Absyn_heap_rgn_type;_tmpA01;});}else{_LL1D: _LL1E:
 return({struct _tuple17 _tmpA02;_tmpA02.f1=(_tmp8E0->tq).real_const,_tmpA02.f2=Cyc_Absyn_heap_rgn_type;_tmpA02;});}_LL1A:;}case 4U: _LL14: _tmp8E1=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp8DB)->f1;_LL15: {
# 5262
void*_tmp8DE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8DF=_tmp8DE;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DF)->tag == 4U){_LL20: _LL21:
 return({struct _tuple17 _tmpA03;_tmpA03.f1=1,_tmpA03.f2=(void*)_check_null(_tmp8E1->rgn);_tmpA03;});}else{_LL22: _LL23:
# 5265
 _tmp8E1->escapes=1;
return({struct _tuple17 _tmpA04;_tmpA04.f1=(_tmp8E1->tq).real_const,_tmpA04.f2=(void*)_check_null(_tmp8E1->rgn);_tmpA04;});}_LL1F:;}case 5U: _LL16: _tmp8E2=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp8DB)->f1;_LL17:
# 5268
 _tmp8E3=_tmp8E2;goto _LL19;default: _LL18: _tmp8E3=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp8DB)->f1;_LL19:
# 5270
 _tmp8E3->escapes=1;
return({struct _tuple17 _tmpA05;_tmpA05.f1=(_tmp8E3->tq).real_const,_tmpA05.f2=(void*)_check_null(_tmp8E3->rgn);_tmpA05;});}_LLD:;}case 21U: _LL3: _tmp90D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp90C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_tmp90B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8DA)->f3;_LL4:
# 5275
 if(_tmp90B)return bogus_ans;{
# 5278
void*_tmp8E4=Cyc_Tcutil_compress((void*)_check_null(_tmp90D->topt));void*_tmp8E5=_tmp8E4;struct Cyc_Absyn_Aggrdecl*_tmp8F1;struct Cyc_List_List*_tmp8F0;switch(*((int*)_tmp8E5)){case 7U: _LL25: _tmp8F0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8E5)->f2;_LL26: {
# 5280
struct Cyc_Absyn_Aggrfield*_tmp8E6=Cyc_Absyn_lookup_field(_tmp8F0,_tmp90C);
if(_tmp8E6 != 0  && _tmp8E6->width == 0){
struct _tuple17 _tmp8E7=Cyc_Tcutil_addressof_props(te,_tmp90D);struct _tuple17 _tmp8E8=_tmp8E7;int _tmp8EA;void*_tmp8E9;_LL2C: _tmp8EA=_tmp8E8.f1;_tmp8E9=_tmp8E8.f2;_LL2D:;
return({struct _tuple17 _tmpA06;_tmpA06.f1=(_tmp8E6->tq).real_const  || _tmp8EA,_tmpA06.f2=_tmp8E9;_tmpA06;});}
# 5285
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E5)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E5)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp8F1=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E5)->f1)->f1).KnownAggr).val;_LL28: {
# 5287
struct Cyc_Absyn_Aggrfield*_tmp8EB=Cyc_Absyn_lookup_decl_field(_tmp8F1,_tmp90C);
if(_tmp8EB != 0  && _tmp8EB->width == 0){
struct _tuple17 _tmp8EC=Cyc_Tcutil_addressof_props(te,_tmp90D);struct _tuple17 _tmp8ED=_tmp8EC;int _tmp8EF;void*_tmp8EE;_LL2F: _tmp8EF=_tmp8ED.f1;_tmp8EE=_tmp8ED.f2;_LL30:;
return({struct _tuple17 _tmpA07;_tmpA07.f1=(_tmp8EB->tq).real_const  || _tmp8EF,_tmpA07.f2=_tmp8EE;_tmpA07;});}
# 5292
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp910=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp90F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_tmp90E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8DA)->f3;_LL6:
# 5297
 if(_tmp90E)return bogus_ans;{
# 5300
void*_tmp8F2=Cyc_Tcutil_compress((void*)_check_null(_tmp910->topt));void*_tmp8F3=_tmp8F2;void*_tmp8F9;void*_tmp8F8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F3)->tag == 3U){_LL32: _tmp8F9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F3)->f1).elt_type;_tmp8F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F3)->f1).ptr_atts).rgn;_LL33: {
# 5302
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp8F4=Cyc_Tcutil_compress(_tmp8F9);void*_tmp8F5=_tmp8F4;struct Cyc_Absyn_Aggrdecl*_tmp8F7;struct Cyc_List_List*_tmp8F6;switch(*((int*)_tmp8F5)){case 7U: _LL37: _tmp8F6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8F5)->f2;_LL38:
# 5305
 finfo=Cyc_Absyn_lookup_field(_tmp8F6,_tmp90F);goto _LL36;case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F5)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F5)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp8F7=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F5)->f1)->f1).KnownAggr).val;_LL3A:
# 5307
 finfo=Cyc_Absyn_lookup_decl_field(_tmp8F7,_tmp90F);goto _LL36;}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5310
if(finfo != 0  && finfo->width == 0)
return({struct _tuple17 _tmpA08;_tmpA08.f1=(finfo->tq).real_const,_tmpA08.f2=_tmp8F8;_tmpA08;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp911=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL8: {
# 5317
void*_tmp8FA=Cyc_Tcutil_compress((void*)_check_null(_tmp911->topt));void*_tmp8FB=_tmp8FA;struct Cyc_Absyn_Tqual _tmp8FD;void*_tmp8FC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FB)->tag == 3U){_LL3E: _tmp8FD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FB)->f1).elt_tq;_tmp8FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FB)->f1).ptr_atts).rgn;_LL3F:
# 5319
 return({struct _tuple17 _tmpA09;_tmpA09.f1=_tmp8FD.real_const,_tmpA09.f2=_tmp8FC;_tmpA09;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp913=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp912=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LLA: {
# 5325
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp913->topt));
void*_tmp8FE=t;struct Cyc_Absyn_Tqual _tmp907;struct Cyc_Absyn_Tqual _tmp906;void*_tmp905;struct Cyc_List_List*_tmp904;switch(*((int*)_tmp8FE)){case 6U: _LL43: _tmp904=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8FE)->f1;_LL44: {
# 5329
struct _tuple13 _tmp8FF=Cyc_Evexp_eval_const_uint_exp(_tmp912);struct _tuple13 _tmp900=_tmp8FF;unsigned int _tmp903;int _tmp902;_LL4C: _tmp903=_tmp900.f1;_tmp902=_tmp900.f2;_LL4D:;
if(!_tmp902)
return bogus_ans;{
struct _tuple12*_tmp901=Cyc_Absyn_lookup_tuple_field(_tmp904,(int)_tmp903);
if(_tmp901 != 0)
return({struct _tuple17 _tmpA0A;_tmpA0A.f1=((*_tmp901).f1).real_const,({void*_tmpCA8=(Cyc_Tcutil_addressof_props(te,_tmp913)).f2;_tmpA0A.f2=_tmpCA8;});_tmpA0A;});
return bogus_ans;};}case 3U: _LL45: _tmp906=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FE)->f1).elt_tq;_tmp905=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FE)->f1).ptr_atts).rgn;_LL46:
# 5337
 return({struct _tuple17 _tmpA0B;_tmpA0B.f1=_tmp906.real_const,_tmpA0B.f2=_tmp905;_tmpA0B;});case 4U: _LL47: _tmp907=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8FE)->f1).tq;_LL48:
# 5343
 return({struct _tuple17 _tmpA0C;_tmpA0C.f1=_tmp907.real_const,({void*_tmpCA9=(Cyc_Tcutil_addressof_props(te,_tmp913)).f2;_tmpA0C.f2=_tmpCA9;});_tmpA0C;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5347
({void*_tmp908=0U;({unsigned int _tmpCAB=e->loc;struct _dyneither_ptr _tmpCAA=({const char*_tmp909="unary & applied to non-lvalue";_tag_dyneither(_tmp909,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpCAB,_tmpCAA,_tag_dyneither(_tmp908,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5354
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp914=({void*_tmpCAC=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCAC,b);});
if(_tmp914 == 0)return;{
struct Cyc_Absyn_Exp*_tmp915=_tmp914;
struct _tuple13 _tmp916=Cyc_Evexp_eval_const_uint_exp(_tmp915);struct _tuple13 _tmp917=_tmp916;unsigned int _tmp921;int _tmp920;_LL1: _tmp921=_tmp917.f1;_tmp920=_tmp917.f2;_LL2:;
if(_tmp920  && _tmp921 <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp91A=({struct Cyc_Int_pa_PrintArg_struct _tmpA0F;_tmpA0F.tag=1U,_tmpA0F.f1=(unsigned long)((int)_tmp921);_tmpA0F;});struct Cyc_Int_pa_PrintArg_struct _tmp91B=({struct Cyc_Int_pa_PrintArg_struct _tmpA0E;_tmpA0E.tag=1U,_tmpA0E.f1=(unsigned long)((int)i);_tmpA0E;});void*_tmp918[2U];_tmp918[0]=& _tmp91A,_tmp918[1]=& _tmp91B;({unsigned int _tmpCAE=loc;struct _dyneither_ptr _tmpCAD=({const char*_tmp919="a dereference will be out of bounds: %d <= %d";_tag_dyneither(_tmp919,sizeof(char),46U);});Cyc_Tcutil_warn(_tmpCAE,_tmpCAD,_tag_dyneither(_tmp918,sizeof(void*),2U));});});else{
# 5363
({struct Cyc_Int_pa_PrintArg_struct _tmp91E=({struct Cyc_Int_pa_PrintArg_struct _tmpA11;_tmpA11.tag=1U,_tmpA11.f1=(unsigned long)((int)_tmp921);_tmpA11;});struct Cyc_Int_pa_PrintArg_struct _tmp91F=({struct Cyc_Int_pa_PrintArg_struct _tmpA10;_tmpA10.tag=1U,_tmpA10.f1=(unsigned long)((int)i);_tmpA10;});void*_tmp91C[2U];_tmp91C[0]=& _tmp91E,_tmp91C[1]=& _tmp91F;({unsigned int _tmpCB0=loc;struct _dyneither_ptr _tmpCAF=({const char*_tmp91D="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmp91D,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpCB0,_tmpCAF,_tag_dyneither(_tmp91C,sizeof(void*),2U));});});}}
return;};}
# 5367
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 5376
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp922=e->r;void*_tmp923=_tmp922;struct Cyc_List_List*_tmp942;struct Cyc_List_List*_tmp941;enum Cyc_Absyn_Primop _tmp940;struct Cyc_List_List*_tmp93F;struct Cyc_List_List*_tmp93E;struct Cyc_List_List*_tmp93D;struct Cyc_List_List*_tmp93C;struct Cyc_Absyn_Exp*_tmp93B;struct Cyc_Absyn_Exp*_tmp93A;struct Cyc_Absyn_Exp*_tmp939;struct Cyc_Absyn_Exp*_tmp938;void*_tmp937;struct Cyc_Absyn_Exp*_tmp936;void*_tmp935;struct Cyc_Absyn_Exp*_tmp934;struct Cyc_Absyn_Exp*_tmp933;struct Cyc_Absyn_Exp*_tmp932;struct Cyc_Absyn_Exp*_tmp931;struct Cyc_Absyn_Exp*_tmp930;struct Cyc_Absyn_Exp*_tmp92F;struct Cyc_Absyn_Exp*_tmp92E;struct Cyc_Absyn_Exp*_tmp92D;struct Cyc_Absyn_Exp*_tmp92C;void*_tmp92B;switch(*((int*)_tmp923)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp92B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_LL10: {
# 5388
void*_tmp924=_tmp92B;struct Cyc_Absyn_Vardecl*_tmp92A;struct Cyc_Absyn_Vardecl*_tmp929;switch(*((int*)_tmp924)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp929=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp924)->f1;_LL37: {
# 5391
void*_tmp925=Cyc_Tcutil_compress(_tmp929->type);void*_tmp926=_tmp925;switch(*((int*)_tmp926)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp92A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp924)->f1;_LL39:
# 5398
 if((int)_tmp92A->sc == (int)Cyc_Absyn_Static){
void*_tmp927=Cyc_Tcutil_compress(_tmp92A->type);void*_tmp928=_tmp927;switch(*((int*)_tmp928)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 5405
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp92E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_tmp92D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp923)->f2;_tmp92C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp923)->f3;_LL12:
# 5411
 return(Cyc_Tcutil_cnst_exp(0,_tmp92E) && 
Cyc_Tcutil_cnst_exp(0,_tmp92D)) && 
Cyc_Tcutil_cnst_exp(0,_tmp92C);case 9U: _LL13: _tmp930=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_tmp92F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp923)->f2;_LL14:
# 5415
 return Cyc_Tcutil_cnst_exp(0,_tmp930) && Cyc_Tcutil_cnst_exp(0,_tmp92F);case 41U: _LL15: _tmp931=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_LL16:
 _tmp932=_tmp931;goto _LL18;case 12U: _LL17: _tmp932=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_LL18:
 _tmp933=_tmp932;goto _LL1A;case 13U: _LL19: _tmp933=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_LL1A:
# 5419
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp933);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp923)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp935=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_tmp934=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp923)->f2;_LL1C:
# 5421
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp934);}else{_LL1D: _tmp937=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_tmp936=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp923)->f2;_LL1E:
# 5424
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp936);}case 15U: _LL1F: _tmp938=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_LL20:
# 5426
 return Cyc_Tcutil_cnst_exp(1,_tmp938);case 27U: _LL21: _tmp93A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp923)->f2;_tmp939=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp923)->f3;_LL22:
# 5428
 return Cyc_Tcutil_cnst_exp(0,_tmp93A) && Cyc_Tcutil_cnst_exp(0,_tmp939);case 28U: _LL23: _tmp93B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_LL24:
# 5430
 return Cyc_Tcutil_cnst_exp(0,_tmp93B);case 26U: _LL25: _tmp93C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_LL26:
 _tmp93D=_tmp93C;goto _LL28;case 30U: _LL27: _tmp93D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp923)->f2;_LL28:
 _tmp93E=_tmp93D;goto _LL2A;case 29U: _LL29: _tmp93E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp923)->f3;_LL2A:
# 5434
 for(0;_tmp93E != 0;_tmp93E=_tmp93E->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple18*)_tmp93E->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp940=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_tmp93F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp923)->f2;_LL2C:
# 5439
 _tmp941=_tmp93F;goto _LL2E;case 24U: _LL2D: _tmp941=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_LL2E:
 _tmp942=_tmp941;goto _LL30;case 31U: _LL2F: _tmp942=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp923)->f1;_LL30:
# 5442
 for(0;_tmp942 != 0;_tmp942=_tmp942->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp942->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 5450
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5454
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5456
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp943=Cyc_Tcutil_compress(t);void*_tmp944=_tmp943;struct Cyc_List_List*_tmp955;struct Cyc_List_List*_tmp954;void*_tmp953;void*_tmp952;void*_tmp951;void*_tmp950;struct Cyc_List_List*_tmp94F;switch(*((int*)_tmp944)){case 0U: _LL1: _tmp950=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp944)->f1;_tmp94F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp944)->f2;_LL2: {
# 5459
void*_tmp945=_tmp950;union Cyc_Absyn_AggrInfo _tmp94E;struct Cyc_List_List*_tmp94D;struct Cyc_Absyn_Enumdecl*_tmp94C;switch(*((int*)_tmp945)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp94C=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp945)->f2;_LL15:
# 5465
 if(_tmp94C == 0  || _tmp94C->fields == 0)
return 0;
_tmp94D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp94C->fields))->v;goto _LL17;case 16U: _LL16: _tmp94D=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp945)->f1;_LL17:
# 5469
{struct Cyc_List_List*_tmp946=_tmp94D;for(0;_tmp946 != 0;_tmp946=_tmp946->tl){
if(((struct Cyc_Absyn_Enumfield*)_tmp946->hd)->tag == 0)
return _tmp946 == _tmp94D;{
struct _tuple13 _tmp947=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_tmp946->hd)->tag));struct _tuple13 _tmp948=_tmp947;unsigned int _tmp94A;int _tmp949;_LL1D: _tmp94A=_tmp948.f1;_tmp949=_tmp948.f2;_LL1E:;
if(_tmp949  && _tmp94A == (unsigned int)0)
return 1;};}}
# 5476
return 0;case 20U: _LL18: _tmp94E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp945)->f1;_LL19: {
# 5480
struct Cyc_Absyn_Aggrdecl*_tmp94B=Cyc_Absyn_get_known_aggrdecl(_tmp94E);
if(_tmp94B->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp94B->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_zeroable(_tmp94B->tvs,_tmp94F,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp94B->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp952=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp944)->f1).ptr_atts).nullable;_tmp951=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp944)->f1).ptr_atts).bounds;_LL4:
# 5487
 return Cyc_Tcutil_unify(_tmp951,Cyc_Absyn_fat_bound_type) || Cyc_Tcutil_force_type2bool(1,_tmp952);case 4U: _LL5: _tmp953=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp944)->f1).elt_type;_LL6:
 return Cyc_Tcutil_zeroable_type(_tmp953);case 6U: _LL7: _tmp954=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp944)->f1;_LL8:
# 5490
 for(0;_tmp954 != 0;_tmp954=_tmp954->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmp954->hd)).f2))return 0;}
return 1;case 7U: _LL9: _tmp955=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp944)->f2;_LLA:
 return Cyc_Tcutil_fields_zeroable(0,0,_tmp955);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 5497
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5499
struct _RegionHandle _tmp956=_new_region("rgn");struct _RegionHandle*rgn=& _tmp956;_push_region(rgn);
{struct Cyc_List_List*_tmp957=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,_tmp957,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp958=0;_npop_handler(0U);return _tmp958;}}{
# 5507
int _tmp959=1;_npop_handler(0U);return _tmp959;};}
# 5500
;_pop_region(rgn);}
# 5511
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmp95A=t;struct Cyc_Absyn_Typedefdecl*_tmp95F;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp95A)->tag == 8U){_LL1: _tmp95F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp95A)->f3;_LL2:
# 5514
 if(_tmp95F != 0){
struct Cyc_Absyn_Tqual _tmp95B=_tmp95F->tq;
if(((_tmp95B.print_const  || _tmp95B.q_volatile) || _tmp95B.q_restrict) || _tmp95B.real_const)
# 5519
({struct Cyc_String_pa_PrintArg_struct _tmp95E=({struct Cyc_String_pa_PrintArg_struct _tmpA12;_tmpA12.tag=0U,({struct _dyneither_ptr _tmpCB1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA12.f1=_tmpCB1;});_tmpA12;});void*_tmp95C[1U];_tmp95C[0]=& _tmp95E;({unsigned int _tmpCB3=loc;struct _dyneither_ptr _tmpCB2=({const char*_tmp95D="qualifier within typedef type %s is ignored";_tag_dyneither(_tmp95D,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpCB3,_tmpCB2,_tag_dyneither(_tmp95C,sizeof(void*),1U));});});}
# 5522
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5530
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp960=Cyc_Tcutil_compress(t);void*_tmp961=_tmp960;struct Cyc_List_List**_tmp967;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp961)->tag == 5U){_LL1: _tmp967=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp961)->f1).attributes;_LL2: {
# 5533
struct Cyc_List_List*_tmp962=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmp967,(void*)atts->hd))
({struct Cyc_List_List*_tmpCB4=({struct Cyc_List_List*_tmp963=_cycalloc(sizeof(*_tmp963));_tmp963->hd=(void*)atts->hd,_tmp963->tl=*_tmp967;_tmp963;});*_tmp967=_tmpCB4;});}else{
# 5540
_tmp962=({struct Cyc_List_List*_tmp964=_cycalloc(sizeof(*_tmp964));_tmp964->hd=(void*)atts->hd,_tmp964->tl=_tmp962;_tmp964;});}}
return _tmp962;}}else{_LL3: _LL4:
({void*_tmp965=0U;({struct _dyneither_ptr _tmpCB5=({const char*_tmp966="transfer_fn_type_atts";_tag_dyneither(_tmp966,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB5,_tag_dyneither(_tmp965,sizeof(void*),0U));});});}_LL0:;}
# 5547
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp968=Cyc_Tcutil_compress(t);void*_tmp969=_tmp968;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_PtrInfo*_tmp96A;switch(*((int*)_tmp969)){case 3U: _LL1: _tmp96A=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp969)->f1;_LL2:
# 5550
 return({void*_tmpCB6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCB6,(_tmp96A->ptr_atts).bounds);});case 4U: _LL3: _tmp96B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp969)->f1).num_elts;_LL4:
# 5552
 return _tmp96B;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5559
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmp96C=b;struct Cyc_Absyn_Vardecl*_tmp970;struct Cyc_Absyn_Vardecl*_tmp96F;struct Cyc_Absyn_Vardecl*_tmp96E;struct Cyc_Absyn_Vardecl*_tmp96D;switch(*((int*)_tmp96C)){case 5U: _LL1: _tmp96D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp96C)->f1;_LL2:
 _tmp96E=_tmp96D;goto _LL4;case 4U: _LL3: _tmp96E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp96C)->f1;_LL4:
 _tmp96F=_tmp96E;goto _LL6;case 3U: _LL5: _tmp96F=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp96C)->f1;_LL6:
 _tmp970=_tmp96F;goto _LL8;case 1U: _LL7: _tmp970=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp96C)->f1;_LL8:
# 5565
 if(!_tmp970->escapes)return _tmp970;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5569
return 0;}
# 5573
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp971=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmp971=({struct Cyc_List_List*_tmp972=_cycalloc(sizeof(*_tmp972));_tmp972->hd=*((void**)_check_null((void**)x->hd)),_tmp972->tl=_tmp971;_tmp972;});}}
return _tmp971;}
# 5580
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp973=Cyc_Tcutil_compress(t);void*_tmp974=_tmp973;void*_tmp97E;struct Cyc_Absyn_Tqual _tmp97D;struct Cyc_Absyn_Exp*_tmp97C;void*_tmp97B;unsigned int _tmp97A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp974)->tag == 4U){_LL1: _tmp97E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp974)->f1).elt_type;_tmp97D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp974)->f1).tq;_tmp97C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp974)->f1).num_elts;_tmp97B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp974)->f1).zero_term;_tmp97A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp974)->f1).zt_loc;_LL2: {
# 5583
void*b;
if(_tmp97C == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 5587
struct Cyc_Absyn_Exp*bnd=_tmp97C;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmp975=0U;({struct _dyneither_ptr _tmpCB7=({const char*_tmp976="cannot convert tag without type!";_tag_dyneither(_tmp976,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB7,_tag_dyneither(_tmp975,sizeof(void*),0U));});});{
void*_tmp977=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp978=_tmp977;void*_tmp979;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp978)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp978)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp978)->f2 != 0){_LL6: _tmp979=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp978)->f2)->hd;_LL7:
# 5593
 b=Cyc_Absyn_thin_bounds_exp(({void*_tmpCB8=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmpCB8,Cyc_Absyn_valueof_exp(_tmp979,0U),0,Cyc_Absyn_No_coercion,0U);}));
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 5596
 if(Cyc_Tcutil_is_const_exp(bnd))
b=Cyc_Absyn_thin_bounds_exp(bnd);else{
# 5599
b=Cyc_Absyn_fat_bound_type;}}_LL5:;};}else{
# 5603
b=Cyc_Absyn_thin_bounds_exp(bnd);}}
# 5605
return Cyc_Absyn_atb_type(_tmp97E,rgn,_tmp97D,b,_tmp97B);}}else{_LL3: _LL4:
 return t;}_LL0:;}

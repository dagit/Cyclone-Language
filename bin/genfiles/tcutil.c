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
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 301
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 316
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 323
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 888 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 890
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 897
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 902
void*Cyc_Absyn_compress_kb(void*);
# 908
int Cyc_Absyn_type2bool(int def,void*);
# 913
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 918
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 920
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 925
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 927
extern void*Cyc_Absyn_empty_effect;
# 929
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 931
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 950
struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar();
struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar();
# 956
extern void*Cyc_Absyn_fat_bound_type;
# 958
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 960
void*Cyc_Absyn_bounds_one();
# 962
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 967
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 989
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 991
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
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
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1098
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1145
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1147
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1149
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1157
int Cyc_Absyn_fntype_att(void*);
# 1159
int Cyc_Absyn_equal_att(void*a1,void*a2);
# 1161
int Cyc_Absyn_attribute_cmp(void*att1,void*att2);
# 1167
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
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
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 285
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 290
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 293
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 298
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 303
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple17{int f1;void*f2;};
# 307
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 310
void*Cyc_Tcutil_normalize_effect(void*e);
# 313
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 315
int Cyc_Tcutil_new_tvar_id();
# 317
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 320
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 322
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 325
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 331
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 335
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 339
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 346
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 349
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 353
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 356
int Cyc_Tcutil_zeroable_type(void*t);
# 360
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 363
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 365
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
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
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
void*_tmp16=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp17=_tmp16;void*_tmp18;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17)->tag == 1U){_LL1: _LL2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp18=_tmp17;_LL4:
 return Cyc_Tcutil_is_integral_type(_tmp18);}_LL0:;}
# 137
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp19=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp1A=_tmp19;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 147
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1B=Cyc_Tcutil_compress(t);void*_tmp1C=_tmp1B;void*_tmp1D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->tag == 3U){_LL1: _tmp1D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->f1).ptr_atts).zero_term;_LL2:
# 150
 return Cyc_Tcutil_force_type2bool(0,_tmp1D);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 156
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp1E=Cyc_Tcutil_compress(t);void*_tmp1F=_tmp1E;void*_tmp20;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->tag == 3U){_LL1: _tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1).ptr_atts).nullable;_LL2:
# 159
 return Cyc_Tcutil_force_type2bool(0,_tmp20);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 165
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp21=Cyc_Tcutil_compress(t);void*_tmp22=_tmp21;void*_tmp23;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->tag == 3U){_LL1: _tmp23=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->f1).ptr_atts).bounds;_LL2:
# 168
 return Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp23);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 175
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp24=Cyc_Tcutil_compress(t);void*_tmp25=_tmp24;void*_tmp27;void*_tmp26;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->tag == 3U){_LL1: _tmp27=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).elt_type;_tmp26=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).ptr_atts).bounds;_LL2:
# 178
 if(Cyc_Tcutil_unify(_tmp26,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=_tmp27;
return 1;}else{
# 182
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 189
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp28=Cyc_Tcutil_compress(t);void*_tmp29=_tmp28;void*_tmp2B;void*_tmp2A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->tag == 3U){_LL1: _tmp2B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).elt_type;_tmp2A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).ptr_atts).zero_term;_LL2:
# 192
*elt_type_dest=_tmp2B;
return Cyc_Absyn_type2bool(0,_tmp2A);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 201
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 203
void*_tmp2C=Cyc_Tcutil_compress(t);void*_tmp2D=_tmp2C;void*_tmp36;struct Cyc_Absyn_Tqual _tmp35;struct Cyc_Absyn_Exp*_tmp34;void*_tmp33;void*_tmp32;void*_tmp31;void*_tmp30;switch(*((int*)_tmp2D)){case 3U: _LL1: _tmp32=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp31=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).bounds;_tmp30=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).zero_term;_LL2:
# 205
 if(Cyc_Absyn_type2bool(0,_tmp30)){
*ptr_type=t;
*elt_type=_tmp32;
{void*_tmp2E=Cyc_Tcutil_compress(_tmp31);void*_tmp2F=_tmp2E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f1)->tag == 14U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 212
return 1;}else{
return 0;}case 4U: _LL3: _tmp36=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp35=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).tq;_tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).zero_term;_LL4:
# 215
 if(Cyc_Absyn_type2bool(0,_tmp33)){
*elt_type=_tmp36;
*is_dyneither=0;
({void*_tmp9FD=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp9FD;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 228
int Cyc_Tcutil_is_tagged_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_tagged_pointer_type_elt(t,& ignore);}
# 234
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmp9FE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp9FE,b);});
if(_tmp37 == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp38=_tmp37;
struct _tuple13 _tmp39=Cyc_Evexp_eval_const_uint_exp(_tmp38);struct _tuple13 _tmp3A=_tmp39;unsigned int _tmp3C;int _tmp3B;_LL1: _tmp3C=_tmp3A.f1;_tmp3B=_tmp3A.f2;_LL2:;
return _tmp3B  && _tmp3C == (unsigned int)1;};}
# 243
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3D=Cyc_Tcutil_compress(t);void*_tmp3E=_tmp3D;struct Cyc_List_List*_tmp4A;struct Cyc_List_List*_tmp49;void*_tmp48;void*_tmp47;void*_tmp46;struct Cyc_List_List*_tmp45;switch(*((int*)_tmp3E)){case 0U: _LL1: _tmp46=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f2;_LL2: {
# 246
void*_tmp3F=_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp44;switch(*((int*)_tmp3F)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return 1;case 15U: _LL12: _LL13:
 goto _LL15;case 16U: _LL14: _LL15:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
 return 0;}else{_LL18: _tmp44=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).KnownAggr).val;_LL19:
# 254
 if(_tmp44->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp40=_new_region("rgn");struct _RegionHandle*rgn=& _tmp40;_push_region(rgn);
{struct Cyc_List_List*_tmp41=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp44->tvs,_tmp45);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,_tmp41,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp42=0;_npop_handler(0U);return _tmp42;}}}{
int _tmp43=1;_npop_handler(0U);return _tmp43;};}
# 261
;_pop_region(rgn);};};}default: _LL1A: _LL1B:
# 265
 return 0;}_LLB:;}case 4U: _LL3: _tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).elt_type;_tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).zero_term;_LL4:
# 270
 return !Cyc_Absyn_type2bool(0,_tmp47) && Cyc_Tcutil_is_bits_only_type(_tmp48);case 6U: _LL5: _tmp49=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E)->f1;_LL6:
# 272
 for(0;_tmp49 != 0;_tmp49=_tmp49->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)_tmp49->hd)).f2))return 0;}
return 1;case 7U: _LL7: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E)->f2;_LL8:
# 276
 for(0;_tmp4A != 0;_tmp4A=_tmp4A->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)_tmp4A->hd)->type))return 0;}
return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 285
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_List_List*_tmp50;void*_tmp4F;switch(*((int*)_tmp4C)){case 3U: _LL1: _tmp4F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_noreturn_fn_type(_tmp4F);case 5U: _LL3: _tmp50=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_LL4:
# 289
 for(0;_tmp50 != 0;_tmp50=_tmp50->tl){
void*_tmp4D=(void*)_tmp50->hd;void*_tmp4E=_tmp4D;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp4E)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 294
return 0;default: _LL5: _LL6:
 return 0;}_LL0:;}char Cyc_Tcutil_Unify[6U]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 300
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 302
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 306
int Cyc_Tcutil_warn_region_coerce=0;
# 309
static void*Cyc_Tcutil_t1_failure=0;
static int Cyc_Tcutil_tq1_const=0;
static void*Cyc_Tcutil_t2_failure=0;
static int Cyc_Tcutil_tq2_const=0;
# 314
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 318
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 323
if(({struct _dyneither_ptr _tmp9FF=({const char*_tmp51="(qualifiers don't match)";_tag_dyneither(_tmp51,sizeof(char),25U);});Cyc_strcmp(_tmp9FF,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp54=({struct Cyc_String_pa_PrintArg_struct _tmp96A;_tmp96A.tag=0U,_tmp96A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp96A;});void*_tmp52[1U];_tmp52[0]=& _tmp54;({struct Cyc___cycFILE*_tmpA01=Cyc_stderr;struct _dyneither_ptr _tmpA00=({const char*_tmp53="  %s\n";_tag_dyneither(_tmp53,sizeof(char),6U);});Cyc_fprintf(_tmpA01,_tmpA00,_tag_dyneither(_tmp52,sizeof(void*),1U));});});
return;}
# 328
if(({struct _dyneither_ptr _tmpA02=({const char*_tmp55="(function effects do not match)";_tag_dyneither(_tmp55,sizeof(char),32U);});Cyc_strcmp(_tmpA02,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp56=Cyc_Absynpp_tc_params_r;
_tmp56.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp56);}{
# 333
void*_tmp57=Cyc_Tcutil_t1_failure;
void*_tmp58=Cyc_Tcutil_t2_failure;
struct _dyneither_ptr s1=(unsigned int)_tmp57?Cyc_Absynpp_typ2string(_tmp57):({const char*_tmp73="<?>";_tag_dyneither(_tmp73,sizeof(char),4U);});
struct _dyneither_ptr s2=(unsigned int)_tmp58?Cyc_Absynpp_typ2string(_tmp58):({const char*_tmp72="<?>";_tag_dyneither(_tmp72,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp5B=({struct Cyc_String_pa_PrintArg_struct _tmp96B;_tmp96B.tag=0U,_tmp96B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp96B;});void*_tmp59[1U];_tmp59[0]=& _tmp5B;({struct Cyc___cycFILE*_tmpA04=Cyc_stderr;struct _dyneither_ptr _tmpA03=({const char*_tmp5A="  %s";_tag_dyneither(_tmp5A,sizeof(char),5U);});Cyc_fprintf(_tmpA04,_tmpA03,_tag_dyneither(_tmp59,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmp5C=0U;({struct Cyc___cycFILE*_tmpA06=Cyc_stderr;struct _dyneither_ptr _tmpA05=({const char*_tmp5D="\n\t";_tag_dyneither(_tmp5D,sizeof(char),3U);});Cyc_fprintf(_tmpA06,_tmpA05,_tag_dyneither(_tmp5C,sizeof(void*),0U));});});
pos=8;}else{
# 344
({void*_tmp5E=0U;({struct Cyc___cycFILE*_tmpA08=Cyc_stderr;struct _dyneither_ptr _tmpA07=({const char*_tmp5F=" ";_tag_dyneither(_tmp5F,sizeof(char),2U);});Cyc_fprintf(_tmpA08,_tmpA07,_tag_dyneither(_tmp5E,sizeof(void*),0U));});});
++ pos;}
# 347
({void*_tmp60=0U;({struct Cyc___cycFILE*_tmpA0A=Cyc_stderr;struct _dyneither_ptr _tmpA09=({const char*_tmp61="and ";_tag_dyneither(_tmp61,sizeof(char),5U);});Cyc_fprintf(_tmpA0A,_tmpA09,_tag_dyneither(_tmp60,sizeof(void*),0U));});});
pos +=4;
if((unsigned int)pos + _get_dyneither_size(s2,sizeof(char))>= (unsigned int)80){
({void*_tmp62=0U;({struct Cyc___cycFILE*_tmpA0C=Cyc_stderr;struct _dyneither_ptr _tmpA0B=({const char*_tmp63="\n\t";_tag_dyneither(_tmp63,sizeof(char),3U);});Cyc_fprintf(_tmpA0C,_tmpA0B,_tag_dyneither(_tmp62,sizeof(void*),0U));});});
pos=8;}
# 353
({struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmp96C;_tmp96C.tag=0U,_tmp96C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp96C;});void*_tmp64[1U];_tmp64[0]=& _tmp66;({struct Cyc___cycFILE*_tmpA0E=Cyc_stderr;struct _dyneither_ptr _tmpA0D=({const char*_tmp65="%s ";_tag_dyneither(_tmp65,sizeof(char),4U);});Cyc_fprintf(_tmpA0E,_tmpA0D,_tag_dyneither(_tmp64,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ (unsigned int)1;
if(pos + 17 >= 80){
({void*_tmp67=0U;({struct Cyc___cycFILE*_tmpA10=Cyc_stderr;struct _dyneither_ptr _tmpA0F=({const char*_tmp68="\n\t";_tag_dyneither(_tmp68,sizeof(char),3U);});Cyc_fprintf(_tmpA10,_tmpA0F,_tag_dyneither(_tmp67,sizeof(void*),0U));});});
pos=8;}
# 359
({void*_tmp69=0U;({struct Cyc___cycFILE*_tmpA12=Cyc_stderr;struct _dyneither_ptr _tmpA11=({const char*_tmp6A="are not compatible. ";_tag_dyneither(_tmp6A,sizeof(char),21U);});Cyc_fprintf(_tmpA12,_tmpA11,_tag_dyneither(_tmp69,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpA13=(char*)Cyc_Tcutil_failure_reason.curr;_tmpA13 != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({unsigned long _tmpA14=(unsigned long)pos;_tmpA14 + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= (unsigned long)80)
({void*_tmp6B=0U;({struct Cyc___cycFILE*_tmpA16=Cyc_stderr;struct _dyneither_ptr _tmpA15=({const char*_tmp6C="\n\t";_tag_dyneither(_tmp6C,sizeof(char),3U);});Cyc_fprintf(_tmpA16,_tmpA15,_tag_dyneither(_tmp6B,sizeof(void*),0U));});});
# 365
({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp96D;_tmp96D.tag=0U,_tmp96D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp96D;});void*_tmp6D[1U];_tmp6D[0]=& _tmp6F;({struct Cyc___cycFILE*_tmpA18=Cyc_stderr;struct _dyneither_ptr _tmpA17=({const char*_tmp6E="%s";_tag_dyneither(_tmp6E,sizeof(char),3U);});Cyc_fprintf(_tmpA18,_tmpA17,_tag_dyneither(_tmp6D,sizeof(void*),1U));});});}
# 367
({void*_tmp70=0U;({struct Cyc___cycFILE*_tmpA1A=Cyc_stderr;struct _dyneither_ptr _tmpA19=({const char*_tmp71="\n";_tag_dyneither(_tmp71,sizeof(char),2U);});Cyc_fprintf(_tmpA1A,_tmpA19,_tag_dyneither(_tmp70,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 371
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 373
Cyc_Warn_verr(loc,fmt,ap);}
# 375
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 378
Cyc_Warn_vimpos(fmt,ap);}
# 380
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 382
Cyc_Warn_vwarn(sg,fmt,ap);}
# 386
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 391
void*Cyc_Tcutil_compress(void*t){
void*_tmp74=t;void*_tmp7D;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Exp*_tmp7B;void**_tmp7A;void**_tmp79;switch(*((int*)_tmp74)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp79=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74)->f2;_LL8: {
# 402
void*ta=(void*)_check_null(*_tmp79);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp79=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74)->f4 == 0){_LL3: _LL4:
# 394
 return t;}else{_LL5: _tmp7A=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74)->f4;_LL6: {
# 396
void*ta=(void*)_check_null(*_tmp7A);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp7A=t2;
return t2;}}case 9U: _LL9: _tmp7B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp74)->f1;_LLA:
# 408
 Cyc_Evexp_eval_const_uint_exp(_tmp7B);{
void*_tmp75=_tmp7B->r;void*_tmp76=_tmp75;void*_tmp77;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp76)->tag == 39U){_LL12: _tmp77=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp76)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp77);}else{_LL14: _LL15:
 return t;}_LL11:;};case 11U: _LLB: _tmp7C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp74)->f1;_LLC: {
# 414
void*_tmp78=_tmp7C->topt;
if(_tmp78 != 0)return _tmp78;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74)->f2 != 0){_LLD: _tmp7D=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74)->f2;_LLE:
# 418
 return Cyc_Tcutil_compress(_tmp7D);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 427
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 431
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp7E=Cyc_Absyn_compress_kb(kb);void*_tmp7F=_tmp7E;struct Cyc_Absyn_Kind*_tmp82;switch(*((int*)_tmp7F)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=1U,_tmp80->f1=0;_tmp80;});case 2U: _LL3: _tmp82=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7F)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=2U,_tmp81->f1=0,_tmp81->f2=_tmp82;_tmp81;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 438
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 440
return({struct Cyc_Absyn_Tvar*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->name=tv->name,_tmp83->identity=- 1,({void*_tmpA1B=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp83->kind=_tmpA1B;});_tmp83;});}
# 442
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 444
struct _tuple10*_tmp84=arg;struct _dyneither_ptr*_tmp88;struct Cyc_Absyn_Tqual _tmp87;void*_tmp86;_LL1: _tmp88=_tmp84->f1;_tmp87=_tmp84->f2;_tmp86=_tmp84->f3;_LL2:;
return({struct _tuple10*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->f1=_tmp88,_tmp85->f2=_tmp87,({void*_tmpA1C=Cyc_Tcutil_copy_type(_tmp86);_tmp85->f3=_tmpA1C;});_tmp85;});}
# 447
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp89=arg;struct Cyc_Absyn_Tqual _tmp8C;void*_tmp8B;_LL1: _tmp8C=_tmp89->f1;_tmp8B=_tmp89->f2;_LL2:;
return({struct _tuple12*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=_tmp8C,({void*_tmpA1D=Cyc_Tcutil_copy_type(_tmp8B);_tmp8A->f2=_tmpA1D;});_tmp8A;});}
# 451
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 453
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->name=f->name,_tmp8D->tq=f->tq,({void*_tmpA1F=Cyc_Tcutil_copy_type(f->type);_tmp8D->type=_tmpA1F;}),_tmp8D->width=f->width,_tmp8D->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmpA1E=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp8D->requires_clause=_tmpA1E;});_tmp8D;});}
# 457
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp8E=x;void*_tmp91;void*_tmp90;_LL1: _tmp91=_tmp8E->f1;_tmp90=_tmp8E->f2;_LL2:;
return({struct _tuple0*_tmp8F=_cycalloc(sizeof(*_tmp8F));({void*_tmpA21=Cyc_Tcutil_copy_type(_tmp91);_tmp8F->f1=_tmpA21;}),({void*_tmpA20=Cyc_Tcutil_copy_type(_tmp90);_tmp8F->f2=_tmpA20;});_tmp8F;});}
# 461
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->name=f->name,_tmp92->tag=f->tag,_tmp92->loc=f->loc;_tmp92;});}
# 464
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 468
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp93=Cyc_Tcutil_compress(t);void*_tmp94=_tmp93;struct Cyc_Absyn_Datatypedecl*_tmpD8;struct Cyc_Absyn_Enumdecl*_tmpD7;struct Cyc_Absyn_Aggrdecl*_tmpD6;struct _tuple2*_tmpD5;struct Cyc_List_List*_tmpD4;struct Cyc_Absyn_Typedefdecl*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD1;enum Cyc_Absyn_AggrKind _tmpD0;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpCE;struct Cyc_List_List*_tmpCD;void*_tmpCC;struct Cyc_Absyn_Tqual _tmpCB;void*_tmpCA;struct Cyc_List_List*_tmpC9;int _tmpC8;struct Cyc_Absyn_VarargInfo*_tmpC7;struct Cyc_List_List*_tmpC6;struct Cyc_List_List*_tmpC5;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_List_List*_tmpC3;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_List_List*_tmpC1;void*_tmpC0;struct Cyc_Absyn_Tqual _tmpBF;struct Cyc_Absyn_Exp*_tmpBE;void*_tmpBD;unsigned int _tmpBC;void*_tmpBB;struct Cyc_Absyn_Tqual _tmpBA;void*_tmpB9;void*_tmpB8;void*_tmpB7;void*_tmpB6;struct Cyc_Absyn_PtrLoc*_tmpB5;struct Cyc_Absyn_Tvar*_tmpB4;void*_tmpB3;struct Cyc_List_List*_tmpB2;void*_tmpB1;switch(*((int*)_tmp94)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp94)->f2 == 0){_LL1: _tmpB1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp94)->f1;_LL2:
 return t;}else{_LL3: _tmpB3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp94)->f1;_tmpB2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp94)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=0U,_tmp95->f1=_tmpB3,({struct Cyc_List_List*_tmpA22=Cyc_Tcutil_copy_types(_tmpB2);_tmp95->f2=_tmpA22;});_tmp95;});}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmpB4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp94)->f1;_LL8:
 return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(_tmpB4));case 3U: _LL9: _tmpBB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).elt_type;_tmpBA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).elt_tq;_tmpB9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).rgn;_tmpB8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).nullable;_tmpB7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).bounds;_tmpB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).zero_term;_tmpB5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).ptrloc;_LLA: {
# 475
void*_tmp96=Cyc_Tcutil_copy_type(_tmpBB);
void*_tmp97=Cyc_Tcutil_copy_type(_tmpB9);
void*_tmp98=Cyc_Tcutil_copy_type(_tmpB8);
struct Cyc_Absyn_Tqual _tmp99=_tmpBA;
# 480
void*_tmp9A=Cyc_Tcutil_copy_type(_tmpB7);
void*_tmp9B=Cyc_Tcutil_copy_type(_tmpB6);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=3U,(_tmp9C->f1).elt_type=_tmp96,(_tmp9C->f1).elt_tq=_tmp99,((_tmp9C->f1).ptr_atts).rgn=_tmp97,((_tmp9C->f1).ptr_atts).nullable=_tmp98,((_tmp9C->f1).ptr_atts).bounds=_tmp9A,((_tmp9C->f1).ptr_atts).zero_term=_tmp9B,((_tmp9C->f1).ptr_atts).ptrloc=_tmpB5;_tmp9C;});}case 4U: _LLB: _tmpC0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).elt_type;_tmpBF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).tq;_tmpBE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).num_elts;_tmpBD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).zero_term;_tmpBC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).zt_loc;_LLC: {
# 484
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpBE);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=4U,({void*_tmpA24=Cyc_Tcutil_copy_type(_tmpC0);(_tmp9D->f1).elt_type=_tmpA24;}),(_tmp9D->f1).tq=_tmpBF,(_tmp9D->f1).num_elts=eopt2,({
void*_tmpA23=Cyc_Tcutil_copy_type(_tmpBD);(_tmp9D->f1).zero_term=_tmpA23;}),(_tmp9D->f1).zt_loc=_tmpBC;_tmp9D;});}case 5U: _LLD: _tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).tvars;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).effect;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).ret_tqual;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).ret_type;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).args;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).c_varargs;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).cyc_varargs;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).rgn_po;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).attributes;_tmpC4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).requires_clause;_tmpC3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).requires_relns;_tmpC2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).ensures_clause;_tmpC1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).ensures_relns;_LLE: {
# 488
struct Cyc_List_List*_tmp9E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpCD);
void*effopt2=_tmpCC == 0?0: Cyc_Tcutil_copy_type(_tmpCC);
void*_tmp9F=Cyc_Tcutil_copy_type(_tmpCA);
struct Cyc_List_List*_tmpA0=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpC9);
int _tmpA1=_tmpC8;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpC7 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpC7;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->name=cv->name,_tmpA2->tq=cv->tq,({void*_tmpA25=Cyc_Tcutil_copy_type(cv->type);_tmpA2->type=_tmpA25;}),_tmpA2->inject=cv->inject;_tmpA2;});}{
# 499
struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpC6);
struct Cyc_List_List*_tmpA4=_tmpC5;
struct Cyc_Absyn_Exp*_tmpA5=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC4);
struct Cyc_List_List*_tmpA6=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3);
struct Cyc_Absyn_Exp*_tmpA7=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC2);
struct Cyc_List_List*_tmpA8=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->tag=5U,(_tmpA9->f1).tvars=_tmp9E,(_tmpA9->f1).effect=effopt2,(_tmpA9->f1).ret_tqual=_tmpCB,(_tmpA9->f1).ret_type=_tmp9F,(_tmpA9->f1).args=_tmpA0,(_tmpA9->f1).c_varargs=_tmpA1,(_tmpA9->f1).cyc_varargs=cyc_varargs2,(_tmpA9->f1).rgn_po=_tmpA3,(_tmpA9->f1).attributes=_tmpA4,(_tmpA9->f1).requires_clause=_tmpA5,(_tmpA9->f1).requires_relns=_tmpA6,(_tmpA9->f1).ensures_clause=_tmpA7,(_tmpA9->f1).ensures_relns=_tmpA8;_tmpA9;});};}case 6U: _LLF: _tmpCE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp94)->f1;_LL10:
# 508
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=6U,({struct Cyc_List_List*_tmpA26=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpCE);_tmpAA->f1=_tmpA26;});_tmpAA;});case 7U: _LL11: _tmpD0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp94)->f1;_tmpCF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp94)->f2;_LL12:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=7U,_tmpAB->f1=_tmpD0,({struct Cyc_List_List*_tmpA27=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpCF);_tmpAB->f2=_tmpA27;});_tmpAB;});case 9U: _LL13: _tmpD1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp94)->f1;_LL14:
# 512
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=9U,_tmpAC->f1=_tmpD1;_tmpAC;});case 11U: _LL15: _tmpD2=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp94)->f1;_LL16:
# 515
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=11U,_tmpAD->f1=_tmpD2;_tmpAD;});case 8U: _LL17: _tmpD5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp94)->f1;_tmpD4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp94)->f2;_tmpD3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp94)->f3;_LL18:
# 517
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=8U,_tmpAE->f1=_tmpD5,({struct Cyc_List_List*_tmpA28=Cyc_Tcutil_copy_types(_tmpD4);_tmpAE->f2=_tmpA28;}),_tmpAE->f3=_tmpD3,_tmpAE->f4=0;_tmpAE;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp94)->f1)->r)){case 0U: _LL19: _tmpD6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp94)->f1)->r)->f1;_LL1A: {
# 520
struct Cyc_List_List*_tmpAF=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD6->tvs);
return({union Cyc_Absyn_AggrInfo _tmpA29=Cyc_Absyn_UnknownAggr(_tmpD6->kind,_tmpD6->name,0);Cyc_Absyn_aggr_type(_tmpA29,_tmpAF);});}case 1U: _LL1B: _tmpD7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp94)->f1)->r)->f1;_LL1C:
# 523
 return Cyc_Absyn_enum_type(_tmpD7->name,0);default: _LL1D: _tmpD8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp94)->f1)->r)->f1;_LL1E: {
# 525
struct Cyc_List_List*_tmpB0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD8->tvs);
return({union Cyc_Absyn_DatatypeInfo _tmpA2A=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp96E;_tmp96E.name=_tmpD8->name,_tmp96E.is_extensible=0;_tmp96E;}));Cyc_Absyn_datatype_type(_tmpA2A,_tmpB0);});}}}_LL0:;}
# 540 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpD9=d;struct _dyneither_ptr*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD9)->tag == 0U){_LL1: _tmpDB=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD9)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->tag=0U,({struct Cyc_Absyn_Exp*_tmpA2B=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpDB);_tmpDA->f1=_tmpA2B;});_tmpDA;});}else{_LL3: _tmpDC=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpD9)->f1;_LL4:
 return d;}_LL0:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 546
static struct _tuple18*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple18*e){
# 548
return({struct _tuple18*_tmpDD=_cycalloc(sizeof(*_tmpDD));({struct Cyc_List_List*_tmpA2D=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpDD->f1=_tmpA2D;}),({struct Cyc_Absyn_Exp*_tmpA2C=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpDD->f2=_tmpA2C;});_tmpDD;});}
# 551
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpDE=preserve_types;
{void*_tmpDF=e->r;void*_tmpE0=_tmpDF;int _tmp15D;struct _dyneither_ptr _tmp15C;void*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;struct _dyneither_ptr*_tmp159;struct Cyc_Core_Opt*_tmp158;struct Cyc_List_List*_tmp157;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp155;int _tmp154;struct Cyc_Absyn_Exp*_tmp153;void**_tmp152;struct Cyc_Absyn_Exp*_tmp151;int _tmp150;int _tmp14F;void*_tmp14E;struct Cyc_Absyn_Enumfield*_tmp14D;struct Cyc_Absyn_Enumdecl*_tmp14C;struct Cyc_Absyn_Enumfield*_tmp14B;struct Cyc_List_List*_tmp14A;struct Cyc_Absyn_Datatypedecl*_tmp149;struct Cyc_Absyn_Datatypefield*_tmp148;void*_tmp147;struct Cyc_List_List*_tmp146;struct _tuple2*_tmp145;struct Cyc_List_List*_tmp144;struct Cyc_List_List*_tmp143;struct Cyc_Absyn_Aggrdecl*_tmp142;struct Cyc_Absyn_Exp*_tmp141;void*_tmp140;int _tmp13F;struct Cyc_Absyn_Vardecl*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;int _tmp13B;struct Cyc_List_List*_tmp13A;struct _dyneither_ptr*_tmp139;struct Cyc_Absyn_Tqual _tmp138;void*_tmp137;struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct _dyneither_ptr*_tmp131;int _tmp130;int _tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct _dyneither_ptr*_tmp12D;int _tmp12C;int _tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;void*_tmp128;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Exp*_tmp126;void*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp122;void*_tmp121;struct Cyc_Absyn_Exp*_tmp120;int _tmp11F;enum Cyc_Absyn_Coercion _tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;int _tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_VarargCallInfo*_tmp116;int _tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;enum Cyc_Absyn_Incrementor _tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Core_Opt*_tmp108;struct Cyc_Absyn_Exp*_tmp107;enum Cyc_Absyn_Primop _tmp106;struct Cyc_List_List*_tmp105;struct _dyneither_ptr _tmp104;void*_tmp103;union Cyc_Absyn_Cnst _tmp102;switch(*((int*)_tmpE0)){case 0U: _LL1: _tmp102=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL2:
 new_e=Cyc_Absyn_const_exp(_tmp102,e->loc);goto _LL0;case 1U: _LL3: _tmp103=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL4:
 new_e=Cyc_Absyn_varb_exp(_tmp103,e->loc);goto _LL0;case 2U: _LL5: _tmp104=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL6:
 new_e=Cyc_Absyn_pragma_exp(_tmp104,e->loc);goto _LL0;case 3U: _LL7: _tmp106=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp105=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL8:
 new_e=({enum Cyc_Absyn_Primop _tmpA2F=_tmp106;struct Cyc_List_List*_tmpA2E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp105);Cyc_Absyn_primop_exp(_tmpA2F,_tmpA2E,e->loc);});goto _LL0;case 4U: _LL9: _tmp109=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp108=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp107=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_LLA:
# 560
 new_e=({struct Cyc_Absyn_Exp*_tmpA32=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp109);struct Cyc_Core_Opt*_tmpA31=(unsigned int)_tmp108?({struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=(void*)_tmp108->v;_tmpE1;}): 0;struct Cyc_Absyn_Exp*_tmpA30=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp107);Cyc_Absyn_assignop_exp(_tmpA32,_tmpA31,_tmpA30,e->loc);});
goto _LL0;case 5U: _LLB: _tmp10B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp10A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LLC:
 new_e=({struct Cyc_Absyn_Exp*_tmpA34=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10B);enum Cyc_Absyn_Incrementor _tmpA33=_tmp10A;Cyc_Absyn_increment_exp(_tmpA34,_tmpA33,e->loc);});goto _LL0;case 6U: _LLD: _tmp10E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp10D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp10C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_LLE:
# 564
 new_e=({struct Cyc_Absyn_Exp*_tmpA37=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10E);struct Cyc_Absyn_Exp*_tmpA36=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10D);struct Cyc_Absyn_Exp*_tmpA35=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10C);Cyc_Absyn_conditional_exp(_tmpA37,_tmpA36,_tmpA35,e->loc);});goto _LL0;case 7U: _LLF: _tmp110=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp10F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL10:
 new_e=({struct Cyc_Absyn_Exp*_tmpA39=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp110);struct Cyc_Absyn_Exp*_tmpA38=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10F);Cyc_Absyn_and_exp(_tmpA39,_tmpA38,e->loc);});goto _LL0;case 8U: _LL11: _tmp112=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp111=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL12:
 new_e=({struct Cyc_Absyn_Exp*_tmpA3B=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp112);struct Cyc_Absyn_Exp*_tmpA3A=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp111);Cyc_Absyn_or_exp(_tmpA3B,_tmpA3A,e->loc);});goto _LL0;case 9U: _LL13: _tmp114=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp113=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL14:
 new_e=({struct Cyc_Absyn_Exp*_tmpA3D=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp114);struct Cyc_Absyn_Exp*_tmpA3C=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp113);Cyc_Absyn_seq_exp(_tmpA3D,_tmpA3C,e->loc);});goto _LL0;case 10U: _LL15: _tmp118=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp117=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp116=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp115=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL16:
# 569
{struct Cyc_Absyn_VarargCallInfo*_tmpE2=_tmp116;int _tmpEE;struct Cyc_List_List*_tmpED;struct Cyc_Absyn_VarargInfo*_tmpEC;if(_tmpE2 != 0){_LL56: _tmpEE=_tmpE2->num_varargs;_tmpED=_tmpE2->injectors;_tmpEC=_tmpE2->vai;_LL57: {
# 571
struct Cyc_Absyn_VarargInfo*_tmpE3=_tmpEC;struct _dyneither_ptr*_tmpEA;struct Cyc_Absyn_Tqual _tmpE9;void*_tmpE8;int _tmpE7;_LL5B: _tmpEA=_tmpE3->name;_tmpE9=_tmpE3->tq;_tmpE8=_tmpE3->type;_tmpE7=_tmpE3->inject;_LL5C:;
new_e=({void*_tmpA43=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->tag=10U,({
struct Cyc_Absyn_Exp*_tmpA42=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp118);_tmpE6->f1=_tmpA42;}),({struct Cyc_List_List*_tmpA41=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp117);_tmpE6->f2=_tmpA41;}),({
struct Cyc_Absyn_VarargCallInfo*_tmpA40=({struct Cyc_Absyn_VarargCallInfo*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->num_varargs=_tmpEE,_tmpE5->injectors=_tmpED,({
struct Cyc_Absyn_VarargInfo*_tmpA3F=({struct Cyc_Absyn_VarargInfo*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->name=_tmpEA,_tmpE4->tq=_tmpE9,({void*_tmpA3E=Cyc_Tcutil_copy_type(_tmpE8);_tmpE4->type=_tmpA3E;}),_tmpE4->inject=_tmpE7;_tmpE4;});_tmpE5->vai=_tmpA3F;});_tmpE5;});
# 574
_tmpE6->f3=_tmpA40;}),_tmpE6->f4=_tmp115;_tmpE6;});
# 572
Cyc_Absyn_new_exp(_tmpA43,e->loc);});
# 577
goto _LL55;}}else{_LL58: _LL59:
# 579
 new_e=({void*_tmpA46=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->tag=10U,({struct Cyc_Absyn_Exp*_tmpA45=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp118);_tmpEB->f1=_tmpA45;}),({struct Cyc_List_List*_tmpA44=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp117);_tmpEB->f2=_tmpA44;}),_tmpEB->f3=_tmp116,_tmpEB->f4=_tmp115;_tmpEB;});Cyc_Absyn_new_exp(_tmpA46,e->loc);});
goto _LL55;}_LL55:;}
# 582
goto _LL0;case 11U: _LL17: _tmp11A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp119=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL18:
# 584
 new_e=_tmp119?({struct Cyc_Absyn_Exp*_tmpA48=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp11A);Cyc_Absyn_rethrow_exp(_tmpA48,e->loc);}):({struct Cyc_Absyn_Exp*_tmpA47=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp11A);Cyc_Absyn_throw_exp(_tmpA47,e->loc);});
goto _LL0;case 12U: _LL19: _tmp11B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL1A:
 new_e=({struct Cyc_Absyn_Exp*_tmpA49=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp11B);Cyc_Absyn_noinstantiate_exp(_tmpA49,e->loc);});
goto _LL0;case 13U: _LL1B: _tmp11D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp11C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL1C:
# 589
 new_e=({struct Cyc_Absyn_Exp*_tmpA4B=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp11D);struct Cyc_List_List*_tmpA4A=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp11C);Cyc_Absyn_instantiate_exp(_tmpA4B,_tmpA4A,e->loc);});
goto _LL0;case 14U: _LL1D: _tmp121=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp120=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp11F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp11E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL1E:
# 592
 new_e=({void*_tmpA4F=Cyc_Tcutil_copy_type(_tmp121);struct Cyc_Absyn_Exp*_tmpA4E=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp120);int _tmpA4D=_tmp11F;enum Cyc_Absyn_Coercion _tmpA4C=_tmp11E;Cyc_Absyn_cast_exp(_tmpA4F,_tmpA4E,_tmpA4D,_tmpA4C,e->loc);});goto _LL0;case 15U: _LL1F: _tmp122=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL20:
 new_e=({struct Cyc_Absyn_Exp*_tmpA50=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp122);Cyc_Absyn_address_exp(_tmpA50,e->loc);});goto _LL0;case 16U: _LL21: _tmp124=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp123=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL22: {
# 595
struct Cyc_Absyn_Exp*eo1=_tmp124;if(_tmp124 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp124);
new_e=({struct Cyc_Absyn_Exp*_tmpA52=eo1;struct Cyc_Absyn_Exp*_tmpA51=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp123);Cyc_Absyn_New_exp(_tmpA52,_tmpA51,e->loc);});
goto _LL0;}case 17U: _LL23: _tmp125=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL24:
 new_e=({void*_tmpA53=Cyc_Tcutil_copy_type(_tmp125);Cyc_Absyn_sizeoftype_exp(_tmpA53,e->loc);});
goto _LL0;case 18U: _LL25: _tmp126=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL26:
 new_e=({struct Cyc_Absyn_Exp*_tmpA54=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp126);Cyc_Absyn_sizeofexp_exp(_tmpA54,e->loc);});goto _LL0;case 19U: _LL27: _tmp128=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp127=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL28:
# 602
 new_e=({void*_tmpA56=Cyc_Tcutil_copy_type(_tmp128);struct Cyc_List_List*_tmpA55=_tmp127;Cyc_Absyn_offsetof_exp(_tmpA56,_tmpA55,e->loc);});goto _LL0;case 20U: _LL29: _tmp129=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL2A:
 new_e=({struct Cyc_Absyn_Exp*_tmpA57=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp129);Cyc_Absyn_deref_exp(_tmpA57,e->loc);});goto _LL0;case 41U: _LL2B: _tmp12A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL2C:
 new_e=({struct Cyc_Absyn_Exp*_tmpA58=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp12A);Cyc_Absyn_extension_exp(_tmpA58,e->loc);});goto _LL0;case 21U: _LL2D: _tmp12E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp12D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp12C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp12B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL2E:
# 606
 new_e=({void*_tmpA5A=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->tag=21U,({struct Cyc_Absyn_Exp*_tmpA59=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp12E);_tmpEF->f1=_tmpA59;}),_tmpEF->f2=_tmp12D,_tmpEF->f3=_tmp12C,_tmpEF->f4=_tmp12B;_tmpEF;});Cyc_Absyn_new_exp(_tmpA5A,e->loc);});goto _LL0;case 22U: _LL2F: _tmp132=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp131=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp130=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp12F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL30:
# 608
 new_e=({void*_tmpA5C=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->tag=22U,({struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp132);_tmpF0->f1=_tmpA5B;}),_tmpF0->f2=_tmp131,_tmpF0->f3=_tmp130,_tmpF0->f4=_tmp12F;_tmpF0;});Cyc_Absyn_new_exp(_tmpA5C,e->loc);});goto _LL0;case 23U: _LL31: _tmp134=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp133=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL32:
 new_e=({struct Cyc_Absyn_Exp*_tmpA5E=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp134);struct Cyc_Absyn_Exp*_tmpA5D=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp133);Cyc_Absyn_subscript_exp(_tmpA5E,_tmpA5D,e->loc);});
goto _LL0;case 24U: _LL33: _tmp135=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL34:
 new_e=({struct Cyc_List_List*_tmpA5F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp135);Cyc_Absyn_tuple_exp(_tmpA5F,e->loc);});goto _LL0;case 25U: _LL35: _tmp139=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE0)->f1)->f1;_tmp138=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE0)->f1)->f2;_tmp137=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE0)->f1)->f3;_tmp136=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL36: {
# 613
struct _dyneither_ptr*vopt1=_tmp139;
if(_tmp139 != 0)vopt1=_tmp139;
new_e=({void*_tmpA63=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->tag=25U,({struct _tuple10*_tmpA62=({struct _tuple10*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->f1=vopt1,_tmpF1->f2=_tmp138,({void*_tmpA61=Cyc_Tcutil_copy_type(_tmp137);_tmpF1->f3=_tmpA61;});_tmpF1;});_tmpF2->f1=_tmpA62;}),({
struct Cyc_List_List*_tmpA60=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp136);_tmpF2->f2=_tmpA60;});_tmpF2;});
# 615
Cyc_Absyn_new_exp(_tmpA63,e->loc);});
# 617
goto _LL0;}case 26U: _LL37: _tmp13A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL38:
 new_e=({void*_tmpA65=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=26U,({struct Cyc_List_List*_tmpA64=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp13A);_tmpF3->f1=_tmpA64;});_tmpF3;});Cyc_Absyn_new_exp(_tmpA65,e->loc);});
goto _LL0;case 27U: _LL39: _tmp13E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp13D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp13C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp13B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL3A:
# 621
 new_e=({void*_tmpA68=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->tag=27U,_tmpF4->f1=_tmp13E,({struct Cyc_Absyn_Exp*_tmpA67=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp13D);_tmpF4->f2=_tmpA67;}),({struct Cyc_Absyn_Exp*_tmpA66=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp13C);_tmpF4->f3=_tmpA66;}),_tmpF4->f4=_tmp13B;_tmpF4;});Cyc_Absyn_new_exp(_tmpA68,e->loc);});
goto _LL0;case 28U: _LL3B: _tmp141=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp140=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp13F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_LL3C:
# 624
 new_e=({void*_tmpA6B=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=28U,({struct Cyc_Absyn_Exp*_tmpA6A=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp141);_tmpF5->f1=_tmpA6A;}),({void*_tmpA69=Cyc_Tcutil_copy_type(_tmp140);_tmpF5->f2=_tmpA69;}),_tmpF5->f3=_tmp13F;_tmpF5;});Cyc_Absyn_new_exp(_tmpA6B,_tmp141->loc);});
# 626
goto _LL0;case 29U: _LL3D: _tmp145=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp144=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp143=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp142=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL3E:
# 628
 new_e=({void*_tmpA6E=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->tag=29U,_tmpF6->f1=_tmp145,({struct Cyc_List_List*_tmpA6D=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp144);_tmpF6->f2=_tmpA6D;}),({struct Cyc_List_List*_tmpA6C=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp143);_tmpF6->f3=_tmpA6C;}),_tmpF6->f4=_tmp142;_tmpF6;});Cyc_Absyn_new_exp(_tmpA6E,e->loc);});
# 630
goto _LL0;case 30U: _LL3F: _tmp147=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp146=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL40:
# 632
 new_e=({void*_tmpA71=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->tag=30U,({void*_tmpA70=Cyc_Tcutil_copy_type(_tmp147);_tmpF7->f1=_tmpA70;}),({struct Cyc_List_List*_tmpA6F=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp146);_tmpF7->f2=_tmpA6F;});_tmpF7;});Cyc_Absyn_new_exp(_tmpA71,e->loc);});
goto _LL0;case 31U: _LL41: _tmp14A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp149=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp148=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_LL42:
# 635
 new_e=({void*_tmpA73=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->tag=31U,({struct Cyc_List_List*_tmpA72=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp14A);_tmpF8->f1=_tmpA72;}),_tmpF8->f2=_tmp149,_tmpF8->f3=_tmp148;_tmpF8;});Cyc_Absyn_new_exp(_tmpA73,e->loc);});
goto _LL0;case 32U: _LL43: _tmp14C=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp14B=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL44:
 new_e=e;goto _LL0;case 33U: _LL45: _tmp14E=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp14D=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL46:
# 639
 new_e=({void*_tmpA75=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->tag=33U,({void*_tmpA74=Cyc_Tcutil_copy_type(_tmp14E);_tmpF9->f1=_tmpA74;}),_tmpF9->f2=_tmp14D;_tmpF9;});Cyc_Absyn_new_exp(_tmpA75,e->loc);});
goto _LL0;case 34U: _LL47: _tmp154=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).is_calloc;_tmp153=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).rgn;_tmp152=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).elt_type;_tmp151=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).num_elts;_tmp150=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).fat_result;_tmp14F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).inline_call;_LL48: {
# 642
struct Cyc_Absyn_Exp*_tmpFA=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp153;if(_tmp153 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp153);{
void**t1=_tmp152;if(_tmp152 != 0)t1=({void**_tmpFB=_cycalloc(sizeof(*_tmpFB));({void*_tmpA76=Cyc_Tcutil_copy_type(*_tmp152);*_tmpFB=_tmpA76;});_tmpFB;});
({void*_tmpA77=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=34U,(_tmpFC->f1).is_calloc=_tmp154,(_tmpFC->f1).rgn=r1,(_tmpFC->f1).elt_type=t1,(_tmpFC->f1).num_elts=_tmp151,(_tmpFC->f1).fat_result=_tmp150,(_tmpFC->f1).inline_call=_tmp14F;_tmpFC;});_tmpFA->r=_tmpA77;});
new_e=_tmpFA;
goto _LL0;};}case 35U: _LL49: _tmp156=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp155=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL4A:
 new_e=({struct Cyc_Absyn_Exp*_tmpA79=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp156);struct Cyc_Absyn_Exp*_tmpA78=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp155);Cyc_Absyn_swap_exp(_tmpA79,_tmpA78,e->loc);});goto _LL0;case 36U: _LL4B: _tmp158=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp157=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL4C: {
# 650
struct Cyc_Core_Opt*nopt1=_tmp158;
if(_tmp158 != 0)nopt1=({struct Cyc_Core_Opt*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->v=(struct _tuple2*)_tmp158->v;_tmpFD;});
new_e=({void*_tmpA7B=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->tag=36U,_tmpFE->f1=nopt1,({struct Cyc_List_List*_tmpA7A=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp157);_tmpFE->f2=_tmpA7A;});_tmpFE;});Cyc_Absyn_new_exp(_tmpA7B,e->loc);});
goto _LL0;}case 37U: _LL4D: _LL4E:
# 655
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpA7C=({const char*_tmpFF="deep_copy: statement expressions unsupported";_tag_dyneither(_tmpFF,sizeof(char),45U);});_tmp100->f1=_tmpA7C;});_tmp100;}));case 38U: _LL4F: _tmp15A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp159=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL50:
 new_e=({void*_tmpA7E=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=38U,({struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp15A);_tmp101->f1=_tmpA7D;}),_tmp101->f2=_tmp159;_tmp101;});Cyc_Absyn_new_exp(_tmpA7E,e->loc);});
goto _LL0;case 39U: _LL51: _tmp15B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL52:
 new_e=({void*_tmpA7F=Cyc_Tcutil_copy_type(_tmp15B);Cyc_Absyn_valueof_exp(_tmpA7F,e->loc);});
goto _LL0;default: _LL53: _tmp15D=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp15C=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL54:
 new_e=Cyc_Absyn_asm_exp(_tmp15D,_tmp15C,e->loc);goto _LL0;}_LL0:;}
# 663
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 667
return new_e;}
# 670
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple19{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 683 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp15E=ka1;enum Cyc_Absyn_KindQual _tmp167;enum Cyc_Absyn_AliasQual _tmp166;_LL1: _tmp167=_tmp15E->kind;_tmp166=_tmp15E->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp15F=ka2;enum Cyc_Absyn_KindQual _tmp165;enum Cyc_Absyn_AliasQual _tmp164;_LL4: _tmp165=_tmp15F->kind;_tmp164=_tmp15F->aliasqual;_LL5:;
# 687
if((int)_tmp167 != (int)_tmp165){
struct _tuple19 _tmp160=({struct _tuple19 _tmp96F;_tmp96F.f1=_tmp167,_tmp96F.f2=_tmp165;_tmp96F;});struct _tuple19 _tmp161=_tmp160;switch(_tmp161.f1){case Cyc_Absyn_BoxKind: switch(_tmp161.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp161.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 696
if((int)_tmp166 != (int)_tmp164){
struct _tuple20 _tmp162=({struct _tuple20 _tmp970;_tmp970.f1=_tmp166,_tmp970.f2=_tmp164;_tmp970;});struct _tuple20 _tmp163=_tmp162;switch(_tmp163.f1){case Cyc_Absyn_Aliasable: if(_tmp163.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp163.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 703
return 1;};}
# 706
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp168=Cyc_Absyn_compress_kb(tv->kind);void*_tmp169=_tmp168;struct Cyc_Absyn_Kind*_tmp16C;struct Cyc_Absyn_Kind*_tmp16B;switch(*((int*)_tmp169)){case 0U: _LL1: _tmp16B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp169)->f1;_LL2:
 return _tmp16B;case 2U: _LL3: _tmp16C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169)->f2;_LL4:
 return _tmp16C;default: _LL5: _LL6:
({void*_tmpA80=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=2U,_tmp16A->f1=0,_tmp16A->f2=def;_tmp16A;});tv->kind=_tmpA80;});return def;}_LL0:;}
# 714
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp16D=({struct _tuple0 _tmp971;({void*_tmpA82=Cyc_Absyn_compress_kb(kb1);_tmp971.f1=_tmpA82;}),({void*_tmpA81=Cyc_Absyn_compress_kb(kb2);_tmp971.f2=_tmpA81;});_tmp971;});struct _tuple0 _tmp16E=_tmp16D;struct Cyc_Core_Opt**_tmp183;void*_tmp182;void*_tmp181;struct Cyc_Core_Opt**_tmp180;struct Cyc_Core_Opt**_tmp17F;struct Cyc_Absyn_Kind*_tmp17E;struct Cyc_Core_Opt**_tmp17D;struct Cyc_Absyn_Kind*_tmp17C;struct Cyc_Core_Opt**_tmp17B;struct Cyc_Absyn_Kind*_tmp17A;struct Cyc_Absyn_Kind*_tmp179;struct Cyc_Absyn_Kind*_tmp178;struct Cyc_Core_Opt**_tmp177;struct Cyc_Absyn_Kind*_tmp176;struct Cyc_Absyn_Kind*_tmp175;struct Cyc_Absyn_Kind*_tmp174;switch(*((int*)_tmp16E.f1)){case 0U: switch(*((int*)_tmp16E.f2)){case 0U: _LL1: _tmp175=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp174=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_LL2:
 return _tmp175 == _tmp174;case 2U: _LL5: _tmp178=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp177=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_tmp176=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f2;_LL6:
# 723
 if(Cyc_Tcutil_kind_leq(_tmp178,_tmp176)){
({struct Cyc_Core_Opt*_tmpA83=({struct Cyc_Core_Opt*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->v=kb1;_tmp170;});*_tmp177=_tmpA83;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp16E.f2)){case 0U: _LL3: _tmp17B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp17A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f2;_tmp179=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_LL4:
# 718
 if(Cyc_Tcutil_kind_leq(_tmp179,_tmp17A)){
({struct Cyc_Core_Opt*_tmpA84=({struct Cyc_Core_Opt*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->v=kb2;_tmp16F;});*_tmp17B=_tmpA84;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp17F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp17E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f2;_tmp17D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_tmp17C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f2;_LL8:
# 728
 if(Cyc_Tcutil_kind_leq(_tmp17E,_tmp17C)){
({struct Cyc_Core_Opt*_tmpA85=({struct Cyc_Core_Opt*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->v=kb1;_tmp171;});*_tmp17D=_tmpA85;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp17C,_tmp17E)){
({struct Cyc_Core_Opt*_tmpA86=({struct Cyc_Core_Opt*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->v=kb2;_tmp172;});*_tmp17F=_tmpA86;});
return 1;}else{
return 0;}}default: _LLB: _tmp181=_tmp16E.f1;_tmp180=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_LLC:
# 737
({struct Cyc_Core_Opt*_tmpA87=({struct Cyc_Core_Opt*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->v=_tmp181;_tmp173;});*_tmp180=_tmpA87;});
return 1;}default: _LL9: _tmp183=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp182=_tmp16E.f2;_LLA:
# 735
 _tmp181=_tmp182;_tmp180=_tmp183;goto _LLC;}_LL0:;}
# 742
struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp184=Cyc_Tcutil_compress(t);void*_tmp185=_tmp184;struct Cyc_Absyn_Tvar*_tmp18A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp185)->tag == 2U){_LL1: _tmp18A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp185)->f1;_LL2: {
# 745
void*_tmp186=_tmp18A->kind;
_tmp18A->kind=kb;
return({struct _tuple15 _tmp972;_tmp972.f1=_tmp18A,_tmp972.f2=_tmp186;_tmp972;});}}else{_LL3: _LL4:
# 749
({struct Cyc_String_pa_PrintArg_struct _tmp189=({struct Cyc_String_pa_PrintArg_struct _tmp973;_tmp973.tag=0U,({struct _dyneither_ptr _tmpA88=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp973.f1=_tmpA88;});_tmp973;});void*_tmp187[1U];_tmp187[0]=& _tmp189;({struct _dyneither_ptr _tmpA89=({const char*_tmp188="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp188,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA89,_tag_dyneither(_tmp187,sizeof(void*),1U));});});}_LL0:;}
# 755
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 757
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 761
struct _RegionHandle _tmp18B=_new_region("temp");struct _RegionHandle*temp=& _tmp18B;_push_region(temp);
{struct Cyc_List_List*_tmp18C=0;
# 764
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp18D=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp18E=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp18F=Cyc_Tcutil_tvar_kind(_tmp18D,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp190=_tmp18F;switch(((struct Cyc_Absyn_Kind*)_tmp190)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 770
 _tmp18C=({struct Cyc_List_List*_tmp192=_region_malloc(temp,sizeof(*_tmp192));({struct _tuple15*_tmpA8A=({struct _tuple15*_tmp191=_region_malloc(temp,sizeof(*_tmp191));_tmp191->f1=_tmp18D,_tmp191->f2=_tmp18E;_tmp191;});_tmp192->hd=_tmpA8A;}),_tmp192->tl=_tmp18C;_tmp192;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 774
if(_tmp18C != 0){
field_type=({struct _RegionHandle*_tmpA8C=temp;struct Cyc_List_List*_tmpA8B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp18C);Cyc_Tcutil_rsubstitute(_tmpA8C,_tmpA8B,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 762
;_pop_region(temp);}
# 779
return k;}
# 786
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 788
void*_tmp193=Cyc_Tcutil_compress(t);void*_tmp194=_tmp193;struct Cyc_Absyn_Typedefdecl*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_PtrInfo _tmp1B0;void*_tmp1AF;struct Cyc_List_List*_tmp1AE;struct Cyc_Absyn_Tvar*_tmp1AD;struct Cyc_Core_Opt*_tmp1AC;switch(*((int*)_tmp194)){case 1U: _LL1: _tmp1AC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp194)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1AC))->v;case 2U: _LL3: _tmp1AD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp194)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp1AD,& Cyc_Tcutil_bk);case 0U: _LL5: _tmp1AF=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1;_tmp1AE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LL6: {
# 792
void*_tmp195=_tmp1AF;enum Cyc_Absyn_AggrKind _tmp1A2;struct Cyc_List_List*_tmp1A1;struct Cyc_Absyn_AggrdeclImpl*_tmp1A0;int _tmp19F;struct Cyc_Absyn_Kind*_tmp19E;enum Cyc_Absyn_Size_of _tmp19D;switch(*((int*)_tmp195)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp19D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp195)->f2;_LL21:
 return((int)_tmp19D == (int)2U  || (int)_tmp19D == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp19E=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp195)->f2;_LL31:
 return _tmp19E;case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 813
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 815
({void*_tmp196=0U;({struct _dyneither_ptr _tmpA8D=({const char*_tmp197="type_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp197,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA8D,_tag_dyneither(_tmp196,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 819
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp1A2=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownAggr).val)->kind;_tmp1A1=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownAggr).val)->tvs;_tmp1A0=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownAggr).val)->impl;_tmp19F=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownAggr).val)->expected_mem_kind;_LL4B:
# 821
 if(_tmp1A0 == 0){
if(_tmp19F)
return& Cyc_Tcutil_mk;else{
# 825
return& Cyc_Tcutil_ak;}}{
# 827
struct Cyc_List_List*_tmp198=_tmp1A0->fields;
if(_tmp198 == 0)return& Cyc_Tcutil_mk;
# 830
if((int)_tmp1A2 == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(_tmp198))->tl != 0;_tmp198=_tmp198->tl){;}{
void*_tmp199=((struct Cyc_Absyn_Aggrfield*)_tmp198->hd)->type;
struct Cyc_Absyn_Kind*_tmp19A=Cyc_Tcutil_field_kind(_tmp199,_tmp1AE,_tmp1A1);
if(_tmp19A == & Cyc_Tcutil_ak  || _tmp19A == & Cyc_Tcutil_tak)return _tmp19A;};}else{
# 838
for(0;_tmp198 != 0;_tmp198=_tmp198->tl){
void*_tmp19B=((struct Cyc_Absyn_Aggrfield*)_tmp198->hd)->type;
struct Cyc_Absyn_Kind*_tmp19C=Cyc_Tcutil_field_kind(_tmp19B,_tmp1AE,_tmp1A1);
if(_tmp19C == & Cyc_Tcutil_ak  || _tmp19C == & Cyc_Tcutil_tak)return _tmp19C;}}
# 844
return& Cyc_Tcutil_mk;};}}_LL1D:;}case 5U: _LL7: _LL8:
# 846
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp1B0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp194)->f1;_LLC: {
# 849
void*_tmp1A3=Cyc_Tcutil_compress((_tmp1B0.ptr_atts).bounds);void*_tmp1A4=_tmp1A3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)){case 13U: _LL4D: _LL4E: {
# 851
enum Cyc_Absyn_AliasQual _tmp1A5=(Cyc_Tcutil_type_kind((_tmp1B0.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1A6=_tmp1A5;switch(_tmp1A6){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 857
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 859
enum Cyc_Absyn_AliasQual _tmp1A7=(Cyc_Tcutil_type_kind((_tmp1B0.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1A8=_tmp1A7;switch(_tmp1A8){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 865
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 869
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp1B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp194)->f1).num_elts;_LL12:
# 871
 if(_tmp1B1 == 0  || Cyc_Tcutil_is_const_exp(_tmp1B1))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp1B2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp194)->f3;_LL16:
# 875
 if(_tmp1B2 == 0  || _tmp1B2->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1AB=({struct Cyc_String_pa_PrintArg_struct _tmp974;_tmp974.tag=0U,({struct _dyneither_ptr _tmpA8E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp974.f1=_tmpA8E;});_tmp974;});void*_tmp1A9[1U];_tmp1A9[0]=& _tmp1AB;({struct _dyneither_ptr _tmpA8F=({const char*_tmp1AA="type_kind: typedef found: %s";_tag_dyneither(_tmp1AA,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA8F,_tag_dyneither(_tmp1A9,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B2->kind))->v;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp194)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 884
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || (int)k1->kind == (int)k2->kind  && (int)k1->aliasqual == (int)k2->aliasqual;}
# 889
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1B3;_push_handler(& _tmp1B3);{int _tmp1B5=0;if(setjmp(_tmp1B3.handler))_tmp1B5=1;if(!_tmp1B5){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1B6=1;_npop_handler(0U);return _tmp1B6;};
# 891
;_pop_handler();}else{void*_tmp1B4=(void*)Cyc_Core_get_exn_thrown();void*_tmp1B7=_tmp1B4;void*_tmp1B8;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1B7)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 893
 return 0;}else{_LL3: _tmp1B8=_tmp1B7;_LL4:(int)_rethrow(_tmp1B8);}_LL0:;}};}
# 898
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1B9=t;struct Cyc_List_List*_tmp1D9;struct Cyc_List_List*_tmp1D8;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List*_tmp1D6;struct Cyc_List_List*_tmp1D5;void*_tmp1D4;struct Cyc_Absyn_Tqual _tmp1D3;void*_tmp1D2;struct Cyc_List_List*_tmp1D1;int _tmp1D0;struct Cyc_Absyn_VarargInfo*_tmp1CF;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_List_List*_tmp1C9;void*_tmp1C8;void*_tmp1C7;struct Cyc_Absyn_PtrInfo _tmp1C6;void*_tmp1C5;struct Cyc_Core_Opt**_tmp1C4;struct Cyc_Absyn_Tvar*_tmp1C3;switch(*((int*)_tmp1B9)){case 2U: _LL1: _tmp1C3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B9)->f1;_LL2:
# 903
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1C3)){
Cyc_Tcutil_failure_reason=({const char*_tmp1BA="(type variable would escape scope)";_tag_dyneither(_tmp1BA,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 907
goto _LL0;case 1U: _LL3: _tmp1C5=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B9)->f2;_tmp1C4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B9)->f4;_LL4:
# 909
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1BB="(occurs check)";_tag_dyneither(_tmp1BB,sizeof(char),15U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 913
if(_tmp1C5 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1C5);else{
# 916
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C4))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 923
if(problem){
struct Cyc_List_List*_tmp1BC=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C4))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1BC=({struct Cyc_List_List*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp1BD->tl=_tmp1BC;_tmp1BD;});}}
# 929
({struct Cyc_Core_Opt*_tmpA90=({struct Cyc_Core_Opt*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->v=_tmp1BC;_tmp1BE;});*_tmp1C4=_tmpA90;});}}}
# 932
goto _LL0;case 3U: _LL5: _tmp1C6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9)->f1;_LL6:
# 934
 Cyc_Tcutil_occurs(evar,r,env,_tmp1C6.elt_type);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C6.ptr_atts).rgn);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C6.ptr_atts).nullable);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C6.ptr_atts).bounds);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C6.ptr_atts).zero_term);
goto _LL0;case 4U: _LL7: _tmp1C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B9)->f1).elt_type;_tmp1C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B9)->f1).zero_term;_LL8:
# 942
 Cyc_Tcutil_occurs(evar,r,env,_tmp1C8);
Cyc_Tcutil_occurs(evar,r,env,_tmp1C7);
goto _LL0;case 5U: _LL9: _tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).tvars;_tmp1D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).effect;_tmp1D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).ret_tqual;_tmp1D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).ret_type;_tmp1D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).args;_tmp1D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).c_varargs;_tmp1CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).cyc_varargs;_tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).rgn_po;_tmp1CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).attributes;_tmp1CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).requires_clause;_tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).requires_relns;_tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).ensures_clause;_tmp1C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).ensures_relns;_LLA:
# 947
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1D5,env);
if(_tmp1D4 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1D4);
Cyc_Tcutil_occurs(evar,r,env,_tmp1D2);
for(0;_tmp1D1 != 0;_tmp1D1=_tmp1D1->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1D1->hd)).f3);}
if(_tmp1CF != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1CF->type);
for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){
struct _tuple0*_tmp1BF=(struct _tuple0*)_tmp1CE->hd;struct _tuple0*_tmp1C0=_tmp1BF;void*_tmp1C2;void*_tmp1C1;_LL16: _tmp1C2=_tmp1C0->f1;_tmp1C1=_tmp1C0->f2;_LL17:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1C2);
Cyc_Tcutil_occurs(evar,r,env,_tmp1C1);}
# 959
goto _LL0;case 6U: _LLB: _tmp1D6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B9)->f1;_LLC:
# 961
 for(0;_tmp1D6 != 0;_tmp1D6=_tmp1D6->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1D6->hd)).f2);}
goto _LL0;case 7U: _LLD: _tmp1D7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B9)->f2;_LLE:
# 966
 for(0;_tmp1D7 != 0;_tmp1D7=_tmp1D7->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1D7->hd)->type);}
goto _LL0;case 8U: _LLF: _tmp1D8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B9)->f2;_LL10:
 _tmp1D9=_tmp1D8;goto _LL12;case 0U: _LL11: _tmp1D9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B9)->f2;_LL12:
# 971
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1D9);goto _LL0;default: _LL13: _LL14:
# 974
 goto _LL0;}_LL0:;};}
# 977
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 979
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 984
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 992
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1DA=0U;({struct _dyneither_ptr _tmpA91=({const char*_tmp1DB="tq1 real_const not set.";_tag_dyneither(_tmp1DB,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA91,_tag_dyneither(_tmp1DA,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1DC=0U;({struct _dyneither_ptr _tmpA92=({const char*_tmp1DD="tq2 real_const not set.";_tag_dyneither(_tmp1DD,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA92,_tag_dyneither(_tmp1DC,sizeof(void*),0U));});});
# 998
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 1001
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp1DE="(qualifiers don't match)";_tag_dyneither(_tmp1DE,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1009
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 1013
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp1DF=({struct _tuple0 _tmp975;_tmp975.f1=e1->r,_tmp975.f2=e2->r;_tmp975;});struct _tuple0 _tmp1E0=_tmp1DF;void*_tmp1EA;void*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E7;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E0.f1)->tag == 14U){_LL1: _tmp1E7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E0.f1)->f2;_LL2:
# 1018
 Cyc_Tcutil_unify_cmp_exp(_tmp1E7,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E0.f2)->tag == 14U){_LL3: _tmp1E8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E0.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp1E8);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E0.f1)->tag == 39U){_LL5: _tmp1E9=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E0.f1)->f1;_LL6: {
# 1021
void*_tmp1E1=Cyc_Tcutil_compress(_tmp1E9);void*_tmp1E2=_tmp1E1;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->tag == 1U){_LLC: _LLD:
({void*_tmpA93=_tmp1E9;Cyc_Tcutil_unify_it(_tmpA93,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->tag=9U,_tmp1E3->f1=e2;_tmp1E3;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E0.f2)->tag == 39U){_LL7: _tmp1EA=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E0.f2)->f1;_LL8: {
# 1026
void*_tmp1E4=Cyc_Tcutil_compress(_tmp1EA);void*_tmp1E5=_tmp1E4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E5)->tag == 1U){_LL11: _LL12:
({void*_tmpA94=_tmp1EA;Cyc_Tcutil_unify_it(_tmpA94,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->tag=9U,_tmp1E6->f1=e1;_tmp1E6;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 1030
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 1034
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 1043
static void*Cyc_Tcutil_rgns_of(void*t);
# 1045
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 1049
static struct _tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp1EB=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1EC=_tmp1EB;switch(((struct Cyc_Absyn_Kind*)_tmp1EC)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp1EC)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->tag=9U,({struct Cyc_Absyn_Exp*_tmpA95=Cyc_Absyn_uint_exp(0U,0U);_tmp1ED->f1=_tmpA95;});_tmp1ED;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 1060
return({struct _tuple15*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->f1=tv,_tmp1EE->f2=t;_tmp1EE;});}
# 1067
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1EF=Cyc_Tcutil_compress(t);void*_tmp1F0=_tmp1EF;struct Cyc_List_List*_tmp206;struct Cyc_List_List*_tmp205;struct Cyc_List_List*_tmp204;void*_tmp203;struct Cyc_Absyn_Tqual _tmp202;void*_tmp201;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_VarargInfo*_tmp1FF;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp1FD;void*_tmp1FC;void*_tmp1FB;void*_tmp1FA;struct Cyc_List_List*_tmp1F9;switch(*((int*)_tmp1F0)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp1F9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->f2;_LL6: {
# 1072
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp1F9);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 1076
struct Cyc_Absyn_Kind*_tmp1F1=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp1F2=_tmp1F1;switch(((struct Cyc_Absyn_Kind*)_tmp1F2)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp1FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F0)->f1).elt_type;_tmp1FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F0)->f1).ptr_atts).rgn;_LLC:
# 1084
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp1F3[2U];({void*_tmpA97=Cyc_Absyn_access_eff(_tmp1FA);_tmp1F3[0]=_tmpA97;}),({void*_tmpA96=Cyc_Tcutil_rgns_of(_tmp1FB);_tmp1F3[1]=_tmpA96;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1F3,sizeof(void*),2U));})));case 4U: _LLD: _tmp1FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F0)->f1).elt_type;_LLE:
# 1086
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp1FC));case 7U: _LLF: _tmp1FD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F0)->f2;_LL10:
# 1088
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1FD)));case 5U: _LL11: _tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).tvars;_tmp203=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).effect;_tmp202=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).ret_tqual;_tmp201=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).ret_type;_tmp200=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).args;_tmp1FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).cyc_varargs;_tmp1FE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).rgn_po;_LL12: {
# 1097
void*_tmp1F4=({struct Cyc_List_List*_tmpA98=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp204);Cyc_Tcutil_substitute(_tmpA98,(void*)_check_null(_tmp203));});
return Cyc_Tcutil_normalize_effect(_tmp1F4);}case 6U: _LL13: _tmp205=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F0)->f1;_LL14: {
# 1100
struct Cyc_List_List*_tmp1F5=0;
for(0;_tmp205 != 0;_tmp205=_tmp205->tl){
_tmp1F5=({struct Cyc_List_List*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6->hd=(*((struct _tuple12*)_tmp205->hd)).f2,_tmp1F6->tl=_tmp1F5;_tmp1F6;});}
_tmp206=_tmp1F5;goto _LL16;}case 8U: _LL15: _tmp206=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F0)->f2;_LL16:
# 1105
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp206)));case 10U: _LL17: _LL18:
({void*_tmp1F7=0U;({struct _dyneither_ptr _tmpA99=({const char*_tmp1F8="typedecl in rgns_of";_tag_dyneither(_tmp1F8,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA99,_tag_dyneither(_tmp1F7,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 1115
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp207=e;void*_tmp213;struct Cyc_List_List**_tmp212;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f1)){case 9U: _LL1: _tmp212=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2;_LL2: {
# 1119
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp212;for(0;effs != 0;effs=effs->tl){
void*_tmp208=(void*)effs->hd;
({void*_tmpA9A=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp208));effs->hd=_tmpA9A;});{
void*_tmp209=(void*)effs->hd;void*_tmp20A=_tmp209;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20A)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20A)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20A)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20A)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20A)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20A)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20A)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20A)->f2)->tl == 0){_LLE: _LLF:
# 1128
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 1132
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp212;for(0;effs != 0;effs=effs->tl){
void*_tmp20B=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp20C=_tmp20B;void*_tmp20F;struct Cyc_List_List*_tmp20E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->f1)){case 9U: _LL13: _tmp20E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->f2;_LL14:
# 1137
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp20E,effects);
goto _LL12;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp20F=_tmp20C;_LL1C:
 effects=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->hd=_tmp20F,_tmp20D->tl=effects;_tmp20D;});goto _LL12;}_LL12:;}}
# 1145
({struct Cyc_List_List*_tmpA9B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp212=_tmpA9B;});
return e;};}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2 != 0){_LL3: _tmp213=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2)->hd;_LL4: {
# 1148
void*_tmp210=Cyc_Tcutil_compress(_tmp213);void*_tmp211=_tmp210;switch(*((int*)_tmp211)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp213);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 1153
 return e;}_LL0:;};}
# 1158
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp214=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->tag=5U,(_tmp215->f1).tvars=0,(_tmp215->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmpA9C=Cyc_Absyn_empty_tqual(0U);(_tmp215->f1).ret_tqual=_tmpA9C;}),(_tmp215->f1).ret_type=Cyc_Absyn_void_type,(_tmp215->f1).args=0,(_tmp215->f1).c_varargs=0,(_tmp215->f1).cyc_varargs=0,(_tmp215->f1).rgn_po=0,(_tmp215->f1).attributes=0,(_tmp215->f1).requires_clause=0,(_tmp215->f1).requires_relns=0,(_tmp215->f1).ensures_clause=0,(_tmp215->f1).ensures_relns=0;_tmp215;});
# 1168
return({void*_tmpAA0=(void*)_tmp214;void*_tmpA9F=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmpA9E=Cyc_Absyn_empty_tqual(0U);void*_tmpA9D=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmpAA0,_tmpA9F,_tmpA9E,_tmpA9D,Cyc_Absyn_false_type);});}
# 1175
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type  || r == Cyc_Absyn_unique_rgn_type) || r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp216=Cyc_Tcutil_compress(e);void*_tmp217=_tmp216;struct Cyc_Core_Opt*_tmp232;void**_tmp231;struct Cyc_Core_Opt*_tmp230;void*_tmp22F;struct Cyc_List_List*_tmp22E;void*_tmp22D;switch(*((int*)_tmp217)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp217)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp217)->f2 != 0){_LL1: _tmp22D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp217)->f2)->hd;_LL2:
# 1182
 if(constrain)return Cyc_Tcutil_unify(r,_tmp22D);
_tmp22D=Cyc_Tcutil_compress(_tmp22D);
if(r == _tmp22D)return 1;{
struct _tuple0 _tmp218=({struct _tuple0 _tmp976;_tmp976.f1=r,_tmp976.f2=_tmp22D;_tmp976;});struct _tuple0 _tmp219=_tmp218;struct Cyc_Absyn_Tvar*_tmp21B;struct Cyc_Absyn_Tvar*_tmp21A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp219.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp219.f2)->tag == 2U){_LLC: _tmp21B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp219.f1)->f1;_tmp21A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp219.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp21B,_tmp21A)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};}else{goto _LL9;}case 9U: _LL3: _tmp22E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp217)->f2;_LL4:
# 1190
 for(0;_tmp22E != 0;_tmp22E=_tmp22E->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp22E->hd))return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp217)->f2 != 0){_LL5: _tmp22F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp217)->f2)->hd;_LL6: {
# 1194
void*_tmp21C=Cyc_Tcutil_rgns_of(_tmp22F);void*_tmp21D=_tmp21C;void*_tmp227;void*_tmp226;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2 != 0){_LL11: _tmp226=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2)->hd;_LL12:
# 1196
 if(!constrain)return 0;{
void*_tmp21E=Cyc_Tcutil_compress(_tmp226);void*_tmp21F=_tmp21E;struct Cyc_Core_Opt*_tmp225;void**_tmp224;struct Cyc_Core_Opt*_tmp223;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21F)->tag == 1U){_LL16: _tmp225=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21F)->f1;_tmp224=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21F)->f2;_tmp223=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21F)->f4;_LL17: {
# 1201
void*_tmp220=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp223);
# 1204
Cyc_Tcutil_occurs(_tmp220,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp223))->v,r);{
void*_tmp221=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp222[2U];_tmp222[0]=_tmp220,({void*_tmpAA1=Cyc_Absyn_access_eff(r);_tmp222[1]=_tmpAA1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp222,sizeof(void*),2U));})));
*_tmp224=_tmp221;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp227=_tmp21D;_LL14:
# 1210
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp227);}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp232=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp217)->f1;_tmp231=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp217)->f2;_tmp230=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp217)->f4;_LL8:
# 1213
 if(_tmp232 == 0  || (int)((struct Cyc_Absyn_Kind*)_tmp232->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp228=0U;({struct _dyneither_ptr _tmpAA2=({const char*_tmp229="effect evar has wrong kind";_tag_dyneither(_tmp229,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA2,_tag_dyneither(_tmp228,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1218
void*_tmp22A=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp230);
# 1221
Cyc_Tcutil_occurs(_tmp22A,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp230))->v,r);{
void*_tmp22B=Cyc_Absyn_join_eff(({void*_tmp22C[2U];_tmp22C[0]=_tmp22A,({void*_tmpAA3=Cyc_Absyn_access_eff(r);_tmp22C[1]=_tmpAA3;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp22C,sizeof(void*),2U));}));
*_tmp231=_tmp22B;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1232
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp233=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp234=_tmp233;struct Cyc_Core_Opt*_tmp246;void**_tmp245;struct Cyc_Core_Opt*_tmp244;void*_tmp243;struct Cyc_List_List*_tmp242;switch(*((int*)_tmp234)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp234)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp242=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp234)->f2;_LL4:
# 1237
 for(0;_tmp242 != 0;_tmp242=_tmp242->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp242->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp234)->f2 != 0){_LL5: _tmp243=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp234)->f2)->hd;_LL6:
# 1242
 _tmp243=Cyc_Tcutil_compress(_tmp243);
if(t == _tmp243)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp243);{
void*_tmp235=Cyc_Tcutil_rgns_of(t);void*_tmp236=_tmp235;void*_tmp23C;void*_tmp23B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236)->f2 != 0){_LLC: _tmp23B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp236)->f2)->hd;_LLD: {
# 1247
struct _tuple0 _tmp237=({struct _tuple0 _tmp977;({void*_tmpAA4=Cyc_Tcutil_compress(_tmp23B);_tmp977.f1=_tmpAA4;}),_tmp977.f2=_tmp243;_tmp977;});struct _tuple0 _tmp238=_tmp237;struct Cyc_Absyn_Tvar*_tmp23A;struct Cyc_Absyn_Tvar*_tmp239;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp238.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp238.f2)->tag == 2U){_LL11: _tmp23A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp238.f1)->f1;_tmp239=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp238.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp243);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp23B == _tmp243;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp23C=_tmp236;_LLF:
# 1251
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp23C);}_LLB:;};}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp246=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp234)->f1;_tmp245=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp234)->f2;_tmp244=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp234)->f4;_LL8:
# 1254
 if(_tmp246 == 0  || (int)((struct Cyc_Absyn_Kind*)_tmp246->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp23D=0U;({struct _dyneither_ptr _tmpAA5=({const char*_tmp23E="effect evar has wrong kind";_tag_dyneither(_tmp23E,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA5,_tag_dyneither(_tmp23D,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1259
void*_tmp23F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp244);
# 1262
Cyc_Tcutil_occurs(_tmp23F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp244))->v,t);{
void*_tmp240=Cyc_Absyn_join_eff(({void*_tmp241[2U];_tmp241[0]=_tmp23F,({void*_tmpAA6=Cyc_Absyn_regionsof_eff(t);_tmp241[1]=_tmpAA6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp241,sizeof(void*),2U));}));
*_tmp245=_tmp240;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1273
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp247=e;struct Cyc_Core_Opt*_tmp25E;void**_tmp25D;struct Cyc_Core_Opt*_tmp25C;void*_tmp25B;struct Cyc_List_List*_tmp25A;struct Cyc_Absyn_Tvar*_tmp259;switch(*((int*)_tmp247)){case 2U: _LL1: _tmp259=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp247)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp259)== 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f1)){case 9U: _LL3: _tmp25A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f2;_LL4:
# 1278
 for(0;_tmp25A != 0;_tmp25A=_tmp25A->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp25A->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f2 != 0){_LL5: _tmp25B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f2)->hd;_LL6: {
# 1283
void*_tmp248=Cyc_Tcutil_rgns_of(_tmp25B);void*_tmp249=_tmp248;void*_tmp253;void*_tmp252;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->f2 != 0){_LLC: _tmp252=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->f2)->hd;_LLD:
# 1285
 if(!may_constrain_evars)return 0;{
void*_tmp24A=Cyc_Tcutil_compress(_tmp252);void*_tmp24B=_tmp24A;struct Cyc_Core_Opt*_tmp251;void**_tmp250;struct Cyc_Core_Opt*_tmp24F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24B)->tag == 1U){_LL11: _tmp251=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24B)->f1;_tmp250=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24B)->f2;_tmp24F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24B)->f4;_LL12: {
# 1291
void*_tmp24C=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp24F);
# 1293
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp24F))->v,v))return 0;{
void*_tmp24D=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp24E[2U];_tmp24E[0]=_tmp24C,({void*_tmpAA7=Cyc_Absyn_var_type(v);_tmp24E[1]=_tmpAA7;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp24E,sizeof(void*),2U));})));
*_tmp250=_tmp24D;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp253=_tmp249;_LLF:
# 1299
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp253);}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp25E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp247)->f1;_tmp25D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp247)->f2;_tmp25C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp247)->f4;_LL8:
# 1302
 if(_tmp25E == 0  || (int)((struct Cyc_Absyn_Kind*)_tmp25E->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp254=0U;({struct _dyneither_ptr _tmpAA8=({const char*_tmp255="effect evar has wrong kind";_tag_dyneither(_tmp255,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA8,_tag_dyneither(_tmp254,sizeof(void*),0U));});});{
# 1306
void*_tmp256=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp25C);
# 1308
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp25C))->v,v))
return 0;{
void*_tmp257=Cyc_Absyn_join_eff(({void*_tmp258[2U];_tmp258[0]=_tmp256,({void*_tmpAA9=Cyc_Absyn_var_type(v);_tmp258[1]=_tmpAA9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp258,sizeof(void*),2U));}));
*_tmp25D=_tmp257;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1318
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp25F=e;void*_tmp265;struct Cyc_List_List*_tmp264;switch(*((int*)_tmp25F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f1)){case 9U: _LL1: _tmp264=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f2;_LL2:
# 1322
 for(0;_tmp264 != 0;_tmp264=_tmp264->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp264->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f2 != 0){_LL3: _tmp265=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f2)->hd;_LL4: {
# 1327
void*_tmp260=Cyc_Tcutil_rgns_of(_tmp265);void*_tmp261=_tmp260;void*_tmp263;void*_tmp262;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp261)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp261)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp261)->f2 != 0){_LLA: _tmp262=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp261)->f2)->hd;_LLB:
 return 0;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp263=_tmp261;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp263);}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1331
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1345 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1350
void*_tmp266=Cyc_Tcutil_compress(e1);void*_tmp267=_tmp266;void**_tmp274;struct Cyc_Core_Opt*_tmp273;struct Cyc_Absyn_Tvar*_tmp272;void*_tmp271;void*_tmp270;struct Cyc_List_List*_tmp26F;switch(*((int*)_tmp267)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->f1)){case 9U: _LL1: _tmp26F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->f2;_LL2:
# 1352
 for(0;_tmp26F != 0;_tmp26F=_tmp26F->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp26F->hd,e2))
return 0;}
return 1;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->f2 != 0){_LL3: _tmp270=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->f2)->hd;_LL4:
# 1363
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp270,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp270,Cyc_Absyn_heap_rgn_type);}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->f2 != 0){_LL7: _tmp271=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp267)->f2)->hd;_LL8: {
# 1367
void*_tmp268=Cyc_Tcutil_rgns_of(_tmp271);void*_tmp269=_tmp268;void*_tmp26B;void*_tmp26A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp269)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp269)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp269)->f2 != 0){_LLE: _tmp26A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp269)->f2)->hd;_LLF:
# 1372
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp26A,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp26A,Cyc_Absyn_sint_type);}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp26B=_tmp269;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp26B,e2);}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp272=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp267)->f1;_LL6:
# 1365
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp272,e2);case 1U: _LL9: _tmp274=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp267)->f2;_tmp273=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp267)->f4;_LLA:
# 1377
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1381
*_tmp274=Cyc_Absyn_empty_effect;
# 1384
return 1;}else{
# 1386
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp26E=({struct Cyc_String_pa_PrintArg_struct _tmp978;_tmp978.tag=0U,({struct _dyneither_ptr _tmpAAA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmp978.f1=_tmpAAA;});_tmp978;});void*_tmp26C[1U];_tmp26C[0]=& _tmp26E;({struct _dyneither_ptr _tmpAAB=({const char*_tmp26D="subset_effect: bad effect: %s";_tag_dyneither(_tmp26D,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAAB,_tag_dyneither(_tmp26C,sizeof(void*),1U));});});}_LL0:;}
# 1402 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1418
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1420
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp275=(struct _tuple0*)r1->hd;struct _tuple0*_tmp276=_tmp275;void*_tmp27C;void*_tmp27B;_LL1: _tmp27C=_tmp276->f1;_tmp27B=_tmp276->f2;_LL2:;{
int found=_tmp27C == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp277=(struct _tuple0*)r2->hd;struct _tuple0*_tmp278=_tmp277;void*_tmp27A;void*_tmp279;_LL4: _tmp27A=_tmp278->f1;_tmp279=_tmp278->f2;_LL5:;
if(Cyc_Tcutil_unify(_tmp27C,_tmp27A) && Cyc_Tcutil_unify(_tmp27B,_tmp279)){
found=1;
break;}}}
# 1430
if(!found)return 0;};}}
# 1432
return 1;}
# 1439
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp27D=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp27E=({struct Cyc_List_List*_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->hd=_tmp27D,_tmp27F->tl=r1;_tmp27F;});
if(Cyc_Relations_consistent_relations(_tmp27E))return 0;}
# 1445
return 1;}
# 1450
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1456
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1458
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1461
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp280=t;switch(*((int*)_tmp280)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp280)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp280)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1467
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1470
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1473
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1476
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp280)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1465
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1468
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1471
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1474
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1477
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp280)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1466
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1469
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1472
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1475
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1478
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1500
({void*_tmp281=0U;({struct _dyneither_ptr _tmpAAC=({const char*_tmp282="bad con";_tag_dyneither(_tmp282,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAAC,_tag_dyneither(_tmp281,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp280)->f1){case 0U: _LL21: _LL22:
# 1479
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
# 1503
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp283=t;void*_tmp284;switch(*((int*)_tmp283)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp284=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp283)->f1;_LL18:
 return 12 + Cyc_Tcutil_tycon2int(_tmp284);}_LL0:;}
# 1520
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);
# 1522
struct _tuple2*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp285=i;struct _tuple2*_tmp287;struct Cyc_Absyn_Datatypedecl*_tmp286;if((_tmp285.KnownDatatype).tag == 2){_LL1: _tmp286=*(_tmp285.KnownDatatype).val;_LL2:
 return _tmp286->name;}else{_LL3: _tmp287=((_tmp285.UnknownDatatype).val).name;_LL4:
 return _tmp287;}_LL0:;}struct _tuple21{struct _tuple2*f1;struct _tuple2*f2;};
# 1529
struct _tuple21 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp288=i;struct _tuple2*_tmp28C;struct _tuple2*_tmp28B;struct Cyc_Absyn_Datatypedecl*_tmp28A;struct Cyc_Absyn_Datatypefield*_tmp289;if((_tmp288.KnownDatatypefield).tag == 2){_LL1: _tmp28A=((_tmp288.KnownDatatypefield).val).f1;_tmp289=((_tmp288.KnownDatatypefield).val).f2;_LL2:
# 1532
 return({struct _tuple21 _tmp979;_tmp979.f1=_tmp28A->name,_tmp979.f2=_tmp289->name;_tmp979;});}else{_LL3: _tmp28C=((_tmp288.UnknownDatatypefield).val).datatype_name;_tmp28B=((_tmp288.UnknownDatatypefield).val).field_name;_LL4:
# 1534
 return({struct _tuple21 _tmp97A;_tmp97A.f1=_tmp28C,_tmp97A.f2=_tmp28B;_tmp97A;});}_LL0:;}struct _tuple22{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1538
struct _tuple22 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp28D=i;struct Cyc_Absyn_Aggrdecl*_tmp290;enum Cyc_Absyn_AggrKind _tmp28F;struct _tuple2*_tmp28E;if((_tmp28D.UnknownAggr).tag == 1){_LL1: _tmp28F=((_tmp28D.UnknownAggr).val).f1;_tmp28E=((_tmp28D.UnknownAggr).val).f2;_LL2:
 return({struct _tuple22 _tmp97B;_tmp97B.f1=_tmp28F,_tmp97B.f2=_tmp28E;_tmp97B;});}else{_LL3: _tmp290=*(_tmp28D.KnownAggr).val;_LL4:
 return({struct _tuple22 _tmp97C;_tmp97C.f1=_tmp290->kind,_tmp97C.f2=_tmp290->name;_tmp97C;});}_LL0:;}
# 1545
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1551
struct _tuple0 _tmp291=({struct _tuple0 _tmp97D;_tmp97D.f1=t1,_tmp97D.f2=t2;_tmp97D;});struct _tuple0 _tmp292=_tmp291;union Cyc_Absyn_AggrInfo _tmp2B0;union Cyc_Absyn_AggrInfo _tmp2AF;union Cyc_Absyn_DatatypeFieldInfo _tmp2AE;union Cyc_Absyn_DatatypeFieldInfo _tmp2AD;union Cyc_Absyn_DatatypeInfo _tmp2AC;union Cyc_Absyn_DatatypeInfo _tmp2AB;struct Cyc_List_List*_tmp2AA;struct Cyc_List_List*_tmp2A9;struct _dyneither_ptr _tmp2A8;struct _dyneither_ptr _tmp2A7;struct _tuple2*_tmp2A6;struct _tuple2*_tmp2A5;switch(*((int*)_tmp292.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp292.f2)->tag == 15U){_LL1: _tmp2A6=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp292.f1)->f1;_tmp2A5=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp292.f2)->f1;_LL2:
 return Cyc_Absyn_qvar_cmp(_tmp2A6,_tmp2A5);}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp292.f2)->tag == 17U){_LL3: _tmp2A8=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp292.f1)->f1;_tmp2A7=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp292.f2)->f1;_LL4:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2A8,(struct _dyneither_ptr)_tmp2A7);}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp292.f2)->tag == 16U){_LL5: _tmp2AA=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp292.f1)->f1;_tmp2A9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp292.f2)->f1;_LL6:
# 1555
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp2AA,_tmp2A9);}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp292.f2)->tag == 18U){_LL7: _tmp2AC=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp292.f1)->f1;_tmp2AB=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp292.f2)->f1;_LL8: {
# 1557
struct _tuple2*q1=Cyc_Tcutil_get_datatype_qvar(_tmp2AC);
struct _tuple2*q2=Cyc_Tcutil_get_datatype_qvar(_tmp2AB);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp292.f2)->tag == 19U){_LL9: _tmp2AE=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp292.f1)->f1;_tmp2AD=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp292.f2)->f1;_LLA: {
# 1561
struct _tuple21 _tmp293=Cyc_Tcutil_get_datatype_field_qvars(_tmp2AE);struct _tuple21 _tmp294=_tmp293;struct _tuple2*_tmp29B;struct _tuple2*_tmp29A;_LL10: _tmp29B=_tmp294.f1;_tmp29A=_tmp294.f2;_LL11:;{
struct _tuple21 _tmp295=Cyc_Tcutil_get_datatype_field_qvars(_tmp2AD);struct _tuple21 _tmp296=_tmp295;struct _tuple2*_tmp299;struct _tuple2*_tmp298;_LL13: _tmp299=_tmp296.f1;_tmp298=_tmp296.f2;_LL14:;{
int _tmp297=Cyc_Absyn_qvar_cmp(_tmp29B,_tmp299);
if(_tmp297 != 0)return _tmp297;
return Cyc_Absyn_qvar_cmp(_tmp29A,_tmp298);};};}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp292.f2)->tag == 20U){_LLB: _tmp2B0=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp292.f1)->f1;_tmp2AF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp292.f2)->f1;_LLC: {
# 1567
struct _tuple22 _tmp29C=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2B0);struct _tuple22 _tmp29D=_tmp29C;enum Cyc_Absyn_AggrKind _tmp2A4;struct _tuple2*_tmp2A3;_LL16: _tmp2A4=_tmp29D.f1;_tmp2A3=_tmp29D.f2;_LL17:;{
struct _tuple22 _tmp29E=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2AF);struct _tuple22 _tmp29F=_tmp29E;enum Cyc_Absyn_AggrKind _tmp2A2;struct _tuple2*_tmp2A1;_LL19: _tmp2A2=_tmp29F.f1;_tmp2A1=_tmp29F.f2;_LL1A:;{
int _tmp2A0=Cyc_Absyn_qvar_cmp(_tmp2A3,_tmp2A1);
if(_tmp2A0 != 0)return _tmp2A0;
return(int)_tmp2A4 - (int)_tmp2A2;};};}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;};};}struct _tuple23{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1577
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp2B1=t1;struct Cyc_Core_Opt*_tmp2BF;void**_tmp2BE;struct Cyc_Core_Opt*_tmp2BD;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B1)->tag == 1U){_LL1: _tmp2BF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B1)->f1;_tmp2BE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B1)->f2;_tmp2BD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B1)->f4;_LL2:
# 1588
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BD))->v,t2);{
struct Cyc_Absyn_Kind*_tmp2B2=Cyc_Tcutil_type_kind(t2);
# 1593
if(Cyc_Tcutil_kind_leq(_tmp2B2,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp2BF))->v)){
*_tmp2BE=t2;
return;}else{
# 1597
{void*_tmp2B3=t2;struct Cyc_Absyn_PtrInfo _tmp2BB;void**_tmp2BA;struct Cyc_Core_Opt*_tmp2B9;switch(*((int*)_tmp2B3)){case 1U: _LL6: _tmp2BA=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B3)->f2;_tmp2B9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B3)->f4;_LL7: {
# 1600
struct Cyc_List_List*_tmp2B4=(struct Cyc_List_List*)_tmp2BD->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B9))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp2B4,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp2B5="(type variable would escape scope)";_tag_dyneither(_tmp2B5,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1607
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp2BF->v,_tmp2B2)){
*_tmp2BA=t1;return;}
# 1610
Cyc_Tcutil_failure_reason=({const char*_tmp2B6="(kinds are incompatible)";_tag_dyneither(_tmp2B6,sizeof(char),25U);});
goto _LL5;}case 3U: _LL8: _tmp2BB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B3)->f1;if((int)((struct Cyc_Absyn_Kind*)_tmp2BF->v)->kind == (int)Cyc_Absyn_BoxKind){_LL9: {
# 1616
void*_tmp2B7=Cyc_Tcutil_compress((_tmp2BB.ptr_atts).bounds);
{void*_tmp2B8=_tmp2B7;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B8)->tag == 1U){_LLD: _LLE:
# 1619
({void*_tmpAAD=_tmp2B7;Cyc_Tcutil_unify(_tmpAAD,Cyc_Absyn_bounds_one());});
*_tmp2BE=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 1624
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1627
Cyc_Tcutil_failure_reason=({const char*_tmp2BC="(kinds are incompatible)";_tag_dyneither(_tmp2BC,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1630
 goto _LL0;}_LL0:;}
# 1635
{struct _tuple0 _tmp2C0=({struct _tuple0 _tmp983;_tmp983.f1=t2,_tmp983.f2=t1;_tmp983;});struct _tuple0 _tmp2C1=_tmp2C0;struct Cyc_List_List*_tmp34F;struct Cyc_Absyn_Typedefdecl*_tmp34E;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_Typedefdecl*_tmp34C;enum Cyc_Absyn_AggrKind _tmp34B;struct Cyc_List_List*_tmp34A;enum Cyc_Absyn_AggrKind _tmp349;struct Cyc_List_List*_tmp348;struct Cyc_List_List*_tmp347;struct Cyc_List_List*_tmp346;struct Cyc_List_List*_tmp345;void*_tmp344;struct Cyc_Absyn_Tqual _tmp343;void*_tmp342;struct Cyc_List_List*_tmp341;int _tmp340;struct Cyc_Absyn_VarargInfo*_tmp33F;struct Cyc_List_List*_tmp33E;struct Cyc_List_List*_tmp33D;struct Cyc_Absyn_Exp*_tmp33C;struct Cyc_List_List*_tmp33B;struct Cyc_Absyn_Exp*_tmp33A;struct Cyc_List_List*_tmp339;struct Cyc_List_List*_tmp338;void*_tmp337;struct Cyc_Absyn_Tqual _tmp336;void*_tmp335;struct Cyc_List_List*_tmp334;int _tmp333;struct Cyc_Absyn_VarargInfo*_tmp332;struct Cyc_List_List*_tmp331;struct Cyc_List_List*_tmp330;struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_List_List*_tmp32E;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_List_List*_tmp32C;void*_tmp32B;struct Cyc_Absyn_Tqual _tmp32A;struct Cyc_Absyn_Exp*_tmp329;void*_tmp328;void*_tmp327;struct Cyc_Absyn_Tqual _tmp326;struct Cyc_Absyn_Exp*_tmp325;void*_tmp324;struct Cyc_Absyn_Exp*_tmp323;struct Cyc_Absyn_Exp*_tmp322;void*_tmp321;struct Cyc_Absyn_Tqual _tmp320;void*_tmp31F;void*_tmp31E;void*_tmp31D;void*_tmp31C;void*_tmp31B;struct Cyc_Absyn_Tqual _tmp31A;void*_tmp319;void*_tmp318;void*_tmp317;void*_tmp316;struct Cyc_Absyn_Tvar*_tmp315;struct Cyc_Absyn_Tvar*_tmp314;void*_tmp313;struct Cyc_List_List*_tmp312;void*_tmp311;struct Cyc_List_List*_tmp310;switch(*((int*)_tmp2C1.f1)){case 1U: _LL12: _LL13:
# 1638
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f1)->f1)->tag == 9U){_LL14: _LL15:
# 1641
 goto _LL17;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f2)->f1)->tag == 9U)goto _LL16;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f1)->f1)->tag == 8U)goto _LL18;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f2)->f1)->tag == 8U)goto _LL1A;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f1)->f1)->tag == 10U)goto _LL1C;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f2)->f1)->tag == 10U)goto _LL1E;else{_LL20: _tmp313=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f1)->f1;_tmp312=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f1)->f2;_tmp311=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f2)->f1;_tmp310=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f2)->f2;_LL21:
# 1652
 if(Cyc_Tcutil_tycon_cmp(_tmp313,_tmp311)== 0){
Cyc_Tcutil_unify_list(_tmp312,_tmp310);
return;}else{
# 1656
Cyc_Tcutil_failure_reason=({const char*_tmp2C3="(different type constructors)";_tag_dyneither(_tmp2C3,sizeof(char),30U);});}
goto _LL11;}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f1)->f1)){case 8U: _LL18: _LL19:
# 1643
 goto _LL1B;case 10U: _LL1C: _LL1D:
# 1645
 goto _LL1F;default: goto _LL32;}}}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1.f2)->f1)){case 9U: _LL16: _LL17:
# 1642
 goto _LL19;case 8U: _LL1A: _LL1B:
# 1644
 goto _LL1D;case 10U: _LL1E: _LL1F:
# 1647
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp2C2="(effects don't unify)";_tag_dyneither(_tmp2C2,sizeof(char),22U);});
goto _LL11;default: switch(*((int*)_tmp2C1.f1)){case 2U: goto _LL32;case 3U: goto _LL32;case 9U: goto _LL32;case 4U: goto _LL32;case 5U: goto _LL32;case 6U: goto _LL32;case 7U: goto _LL32;case 8U: goto _LL32;default: goto _LL32;}}else{switch(*((int*)_tmp2C1.f1)){case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 2U){_LL22: _tmp315=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C1.f1)->f1;_tmp314=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C1.f2)->f1;_LL23: {
# 1660
struct _dyneither_ptr*_tmp2C4=_tmp315->name;
struct _dyneither_ptr*_tmp2C5=_tmp314->name;
# 1663
int _tmp2C6=_tmp315->identity;
int _tmp2C7=_tmp314->identity;
if(_tmp2C7 == _tmp2C6)return;
Cyc_Tcutil_failure_reason=({const char*_tmp2C8="(variable types are not the same)";_tag_dyneither(_tmp2C8,sizeof(char),34U);});
goto _LL11;}}else{goto _LL32;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 3U){_LL24: _tmp321=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f1)->f1).elt_type;_tmp320=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f1)->f1).elt_tq;_tmp31F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f1)->f1).ptr_atts).rgn;_tmp31E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f1)->f1).ptr_atts).nullable;_tmp31D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f1)->f1).ptr_atts).bounds;_tmp31C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f1)->f1).ptr_atts).zero_term;_tmp31B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f2)->f1).elt_type;_tmp31A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f2)->f1).elt_tq;_tmp319=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f2)->f1).ptr_atts).rgn;_tmp318=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f2)->f1).ptr_atts).nullable;_tmp317=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f2)->f1).ptr_atts).bounds;_tmp316=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1.f2)->f1).ptr_atts).zero_term;_LL25:
# 1671
 Cyc_Tcutil_unify_it(_tmp31B,_tmp321);
Cyc_Tcutil_unify_it(_tmp31F,_tmp319);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
struct _dyneither_ptr _tmp2C9=Cyc_Tcutil_failure_reason;
if(!Cyc_Tcutil_unify(_tmp316,_tmp31C)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp2CA="(not both zero terminated)";_tag_dyneither(_tmp2CA,sizeof(char),27U);});
goto _LL11;}
# 1681
Cyc_Tcutil_unify_tqual(_tmp31A,_tmp31B,_tmp320,_tmp321);
if(!Cyc_Tcutil_unify(_tmp317,_tmp31D)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp2CB="(different pointer bounds)";_tag_dyneither(_tmp2CB,sizeof(char),27U);});
goto _LL11;}{
# 1688
void*_tmp2CC=Cyc_Tcutil_compress(_tmp317);void*_tmp2CD=_tmp2CC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2CD)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2CD)->f1)->tag == 14U){_LL35: _LL36:
# 1690
 Cyc_Tcutil_failure_reason=_tmp2C9;
return;}else{goto _LL37;}}else{_LL37: _LL38:
# 1693
 Cyc_Tcutil_failure_reason=({const char*_tmp2CE="(incompatible pointer types)";_tag_dyneither(_tmp2CE,sizeof(char),29U);});
Cyc_Tcutil_unify_it(_tmp318,_tmp31E);
return;}_LL34:;};};}else{goto _LL32;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 9U){_LL26: _tmp323=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2C1.f1)->f1;_tmp322=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2C1.f2)->f1;_LL27:
# 1699
 if(!Cyc_Evexp_same_const_exp(_tmp323,_tmp322)){
Cyc_Tcutil_failure_reason=({const char*_tmp2CF="(cannot prove expressions are the same)";_tag_dyneither(_tmp2CF,sizeof(char),40U);});
goto _LL11;}
# 1703
return;}else{goto _LL32;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 4U){_LL28: _tmp32B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C1.f1)->f1).elt_type;_tmp32A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C1.f1)->f1).tq;_tmp329=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C1.f1)->f1).num_elts;_tmp328=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C1.f1)->f1).zero_term;_tmp327=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C1.f2)->f1).elt_type;_tmp326=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C1.f2)->f1).tq;_tmp325=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C1.f2)->f1).num_elts;_tmp324=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C1.f2)->f1).zero_term;_LL29:
# 1707
 Cyc_Tcutil_unify_it(_tmp327,_tmp32B);
Cyc_Tcutil_unify_tqual(_tmp326,_tmp327,_tmp32A,_tmp32B);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp2D0="(not both zero terminated)";_tag_dyneither(_tmp2D0,sizeof(char),27U);});
Cyc_Tcutil_unify_it(_tmp328,_tmp324);
if(_tmp329 == _tmp325)return;
if(_tmp329 == 0  || _tmp325 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp329,_tmp325))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp2D1="(different array sizes)";_tag_dyneither(_tmp2D1,sizeof(char),24U);});
goto _LL11;}else{goto _LL32;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 5U){_LL2A: _tmp345=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).tvars;_tmp344=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).effect;_tmp343=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).ret_tqual;_tmp342=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).ret_type;_tmp341=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).args;_tmp340=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).c_varargs;_tmp33F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).cyc_varargs;_tmp33E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).rgn_po;_tmp33D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).attributes;_tmp33C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).requires_clause;_tmp33B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).requires_relns;_tmp33A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).ensures_clause;_tmp339=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f1)->f1).ensures_relns;_tmp338=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).tvars;_tmp337=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).effect;_tmp336=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).ret_tqual;_tmp335=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).ret_type;_tmp334=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).args;_tmp333=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).c_varargs;_tmp332=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).cyc_varargs;_tmp331=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).rgn_po;_tmp330=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).attributes;_tmp32F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).requires_clause;_tmp32E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).requires_relns;_tmp32D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).ensures_clause;_tmp32C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C1.f2)->f1).ensures_relns;_LL2B: {
# 1722
int done=0;
{struct _RegionHandle _tmp2D2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2D2;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp338 != 0){
if(_tmp345 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2D3="(second function type has too few type variables)";_tag_dyneither(_tmp2D3,sizeof(char),50U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1730
void*_tmp2D4=((struct Cyc_Absyn_Tvar*)_tmp338->hd)->kind;
void*_tmp2D5=((struct Cyc_Absyn_Tvar*)_tmp345->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp2D4,_tmp2D5)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D8=({struct Cyc_String_pa_PrintArg_struct _tmp980;_tmp980.tag=0U,({
struct _dyneither_ptr _tmpAAE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp338->hd));_tmp980.f1=_tmpAAE;});_tmp980;});struct Cyc_String_pa_PrintArg_struct _tmp2D9=({struct Cyc_String_pa_PrintArg_struct _tmp97F;_tmp97F.tag=0U,({
struct _dyneither_ptr _tmpAAF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp338->hd,& Cyc_Tcutil_bk)));_tmp97F.f1=_tmpAAF;});_tmp97F;});struct Cyc_String_pa_PrintArg_struct _tmp2DA=({struct Cyc_String_pa_PrintArg_struct _tmp97E;_tmp97E.tag=0U,({
struct _dyneither_ptr _tmpAB0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp345->hd,& Cyc_Tcutil_bk)));_tmp97E.f1=_tmpAB0;});_tmp97E;});void*_tmp2D6[3U];_tmp2D6[0]=& _tmp2D8,_tmp2D6[1]=& _tmp2D9,_tmp2D6[2]=& _tmp2DA;({struct _dyneither_ptr _tmpAB1=({const char*_tmp2D7="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp2D7,sizeof(char),44U);});Cyc_aprintf(_tmpAB1,_tag_dyneither(_tmp2D6,sizeof(void*),3U));});});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1739
inst=({struct Cyc_List_List*_tmp2DC=_region_malloc(rgn,sizeof(*_tmp2DC));({struct _tuple15*_tmpAB3=({struct _tuple15*_tmp2DB=_region_malloc(rgn,sizeof(*_tmp2DB));_tmp2DB->f1=(struct Cyc_Absyn_Tvar*)_tmp345->hd,({void*_tmpAB2=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp338->hd);_tmp2DB->f2=_tmpAB2;});_tmp2DB;});_tmp2DC->hd=_tmpAB3;}),_tmp2DC->tl=inst;_tmp2DC;});
_tmp338=_tmp338->tl;
_tmp345=_tmp345->tl;};}
# 1743
if(_tmp345 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2DD="(second function type has too many type variables)";_tag_dyneither(_tmp2DD,sizeof(char),51U);});
_npop_handler(0U);goto _LL11;}
# 1747
if(inst != 0){
({void*_tmpAB6=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2DE=_cycalloc(sizeof(*_tmp2DE));_tmp2DE->tag=5U,(_tmp2DE->f1).tvars=0,(_tmp2DE->f1).effect=_tmp337,(_tmp2DE->f1).ret_tqual=_tmp336,(_tmp2DE->f1).ret_type=_tmp335,(_tmp2DE->f1).args=_tmp334,(_tmp2DE->f1).c_varargs=_tmp333,(_tmp2DE->f1).cyc_varargs=_tmp332,(_tmp2DE->f1).rgn_po=_tmp331,(_tmp2DE->f1).attributes=_tmp330,(_tmp2DE->f1).requires_clause=_tmp33C,(_tmp2DE->f1).requires_relns=_tmp33B,(_tmp2DE->f1).ensures_clause=_tmp33A,(_tmp2DE->f1).ensures_relns=_tmp339;_tmp2DE;});Cyc_Tcutil_unify_it(_tmpAB6,({
# 1751
struct _RegionHandle*_tmpAB5=rgn;struct Cyc_List_List*_tmpAB4=inst;Cyc_Tcutil_rsubstitute(_tmpAB5,_tmpAB4,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));
_tmp2DF->tag=5U,(_tmp2DF->f1).tvars=0,(_tmp2DF->f1).effect=_tmp344,(_tmp2DF->f1).ret_tqual=_tmp343,(_tmp2DF->f1).ret_type=_tmp342,(_tmp2DF->f1).args=_tmp341,(_tmp2DF->f1).c_varargs=_tmp340,(_tmp2DF->f1).cyc_varargs=_tmp33F,(_tmp2DF->f1).rgn_po=_tmp33E,(_tmp2DF->f1).attributes=_tmp33D,(_tmp2DF->f1).requires_clause=_tmp32F,(_tmp2DF->f1).requires_relns=_tmp32E,(_tmp2DF->f1).ensures_clause=_tmp32D,(_tmp2DF->f1).ensures_relns=_tmp32C;_tmp2DF;}));}));});
# 1756
done=1;}}
# 1724
;_pop_region(rgn);}
# 1759
if(done)
return;
Cyc_Tcutil_unify_it(_tmp335,_tmp342);
Cyc_Tcutil_unify_tqual(_tmp336,_tmp335,_tmp343,_tmp342);
for(0;_tmp334 != 0  && _tmp341 != 0;(_tmp334=_tmp334->tl,_tmp341=_tmp341->tl)){
struct _tuple10 _tmp2E0=*((struct _tuple10*)_tmp334->hd);struct _tuple10 _tmp2E1=_tmp2E0;struct Cyc_Absyn_Tqual _tmp2E7;void*_tmp2E6;_LL3A: _tmp2E7=_tmp2E1.f2;_tmp2E6=_tmp2E1.f3;_LL3B:;{
struct _tuple10 _tmp2E2=*((struct _tuple10*)_tmp341->hd);struct _tuple10 _tmp2E3=_tmp2E2;struct Cyc_Absyn_Tqual _tmp2E5;void*_tmp2E4;_LL3D: _tmp2E5=_tmp2E3.f2;_tmp2E4=_tmp2E3.f3;_LL3E:;
Cyc_Tcutil_unify_it(_tmp2E6,_tmp2E4);
Cyc_Tcutil_unify_tqual(_tmp2E7,_tmp2E6,_tmp2E5,_tmp2E4);};}
# 1769
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp334 != 0  || _tmp341 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2E8="(function types have different number of arguments)";_tag_dyneither(_tmp2E8,sizeof(char),52U);});
goto _LL11;}
# 1775
if(_tmp333 != _tmp340){
Cyc_Tcutil_failure_reason=({const char*_tmp2E9="(only one function type takes C varargs)";_tag_dyneither(_tmp2E9,sizeof(char),41U);});
goto _LL11;}{
# 1780
int bad_cyc_vararg=0;
{struct _tuple23 _tmp2EA=({struct _tuple23 _tmp981;_tmp981.f1=_tmp332,_tmp981.f2=_tmp33F;_tmp981;});struct _tuple23 _tmp2EB=_tmp2EA;struct _dyneither_ptr*_tmp2F5;struct Cyc_Absyn_Tqual _tmp2F4;void*_tmp2F3;int _tmp2F2;struct _dyneither_ptr*_tmp2F1;struct Cyc_Absyn_Tqual _tmp2F0;void*_tmp2EF;int _tmp2EE;if(_tmp2EB.f1 == 0){if(_tmp2EB.f2 == 0){_LL40: _LL41:
 goto _LL3F;}else{_LL42: _LL43:
 goto _LL45;}}else{if(_tmp2EB.f2 == 0){_LL44: _LL45:
# 1785
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp2EC="(only one function type takes varargs)";_tag_dyneither(_tmp2EC,sizeof(char),39U);});
goto _LL3F;}else{_LL46: _tmp2F5=(_tmp2EB.f1)->name;_tmp2F4=(_tmp2EB.f1)->tq;_tmp2F3=(_tmp2EB.f1)->type;_tmp2F2=(_tmp2EB.f1)->inject;_tmp2F1=(_tmp2EB.f2)->name;_tmp2F0=(_tmp2EB.f2)->tq;_tmp2EF=(_tmp2EB.f2)->type;_tmp2EE=(_tmp2EB.f2)->inject;_LL47:
# 1789
 Cyc_Tcutil_unify_it(_tmp2F3,_tmp2EF);
Cyc_Tcutil_unify_tqual(_tmp2F4,_tmp2F3,_tmp2F0,_tmp2EF);
if(_tmp2F2 != _tmp2EE){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp2ED="(only one function type injects varargs)";_tag_dyneither(_tmp2ED,sizeof(char),41U);});}
# 1795
goto _LL3F;}}_LL3F:;}
# 1797
if(bad_cyc_vararg)goto _LL11;{
# 1800
int bad_effect=0;
{struct _tuple0 _tmp2F6=({struct _tuple0 _tmp982;_tmp982.f1=_tmp337,_tmp982.f2=_tmp344;_tmp982;});struct _tuple0 _tmp2F7=_tmp2F6;if(_tmp2F7.f1 == 0){if(_tmp2F7.f2 == 0){_LL49: _LL4A:
 goto _LL48;}else{_LL4B: _LL4C:
 goto _LL4E;}}else{if(_tmp2F7.f2 == 0){_LL4D: _LL4E:
 bad_effect=1;goto _LL48;}else{_LL4F: _LL50:
 bad_effect=!({void*_tmpAB7=(void*)_check_null(_tmp337);Cyc_Tcutil_unify_effect(_tmpAB7,(void*)_check_null(_tmp344));});goto _LL48;}}_LL48:;}
# 1807
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp2F8="(function effects do not match)";_tag_dyneither(_tmp2F8,sizeof(char),32U);});
goto _LL11;}
# 1813
if(!Cyc_Tcutil_same_atts(_tmp33D,_tmp330)){
Cyc_Tcutil_failure_reason=({const char*_tmp2F9="(function types have different attributes)";_tag_dyneither(_tmp2F9,sizeof(char),43U);});
goto _LL11;}
# 1817
if(!Cyc_Tcutil_same_rgn_po(_tmp33E,_tmp331)){
Cyc_Tcutil_failure_reason=({const char*_tmp2FA="(function types have different region lifetime orderings)";_tag_dyneither(_tmp2FA,sizeof(char),58U);});
goto _LL11;}
# 1821
if(!Cyc_Tcutil_check_logical_equivalence(_tmp33B,_tmp32E)){
Cyc_Tcutil_failure_reason=({const char*_tmp2FB="(@requires clauses not equivalent)";_tag_dyneither(_tmp2FB,sizeof(char),35U);});
goto _LL11;}
# 1825
if(!Cyc_Tcutil_check_logical_equivalence(_tmp339,_tmp32C)){
Cyc_Tcutil_failure_reason=({const char*_tmp2FC="(@ensures clauses not equivalent)";_tag_dyneither(_tmp2FC,sizeof(char),34U);});
goto _LL11;}
# 1829
return;};};}}else{goto _LL32;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 6U){_LL2C: _tmp347=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2C1.f1)->f1;_tmp346=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2C1.f2)->f1;_LL2D:
# 1832
 for(0;_tmp346 != 0  && _tmp347 != 0;(_tmp346=_tmp346->tl,_tmp347=_tmp347->tl)){
struct _tuple12 _tmp2FD=*((struct _tuple12*)_tmp346->hd);struct _tuple12 _tmp2FE=_tmp2FD;struct Cyc_Absyn_Tqual _tmp304;void*_tmp303;_LL52: _tmp304=_tmp2FE.f1;_tmp303=_tmp2FE.f2;_LL53:;{
struct _tuple12 _tmp2FF=*((struct _tuple12*)_tmp347->hd);struct _tuple12 _tmp300=_tmp2FF;struct Cyc_Absyn_Tqual _tmp302;void*_tmp301;_LL55: _tmp302=_tmp300.f1;_tmp301=_tmp300.f2;_LL56:;
Cyc_Tcutil_unify_it(_tmp303,_tmp301);
Cyc_Tcutil_unify_tqual(_tmp304,_tmp303,_tmp302,_tmp301);};}
# 1838
if(_tmp346 == 0  && _tmp347 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp305="(tuple types have different numbers of components)";_tag_dyneither(_tmp305,sizeof(char),51U);});
goto _LL11;}else{goto _LL32;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 7U){_LL2E: _tmp34B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C1.f1)->f1;_tmp34A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C1.f1)->f2;_tmp349=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C1.f2)->f1;_tmp348=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C1.f2)->f2;_LL2F:
# 1845
 if((int)_tmp349 != (int)_tmp34B){Cyc_Tcutil_failure_reason=({const char*_tmp306="(struct and union type)";_tag_dyneither(_tmp306,sizeof(char),24U);});goto _LL11;}
for(0;_tmp348 != 0  && _tmp34A != 0;(_tmp348=_tmp348->tl,_tmp34A=_tmp34A->tl)){
struct Cyc_Absyn_Aggrfield*_tmp307=(struct Cyc_Absyn_Aggrfield*)_tmp348->hd;
struct Cyc_Absyn_Aggrfield*_tmp308=(struct Cyc_Absyn_Aggrfield*)_tmp34A->hd;
if(Cyc_strptrcmp(_tmp307->name,_tmp308->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp309="(different member names)";_tag_dyneither(_tmp309,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1853
Cyc_Tcutil_unify_it(_tmp307->type,_tmp308->type);
Cyc_Tcutil_unify_tqual(_tmp307->tq,_tmp307->type,_tmp308->tq,_tmp308->type);
if(!Cyc_Tcutil_same_atts(_tmp307->attributes,_tmp308->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp30A="(different attributes on member)";_tag_dyneither(_tmp30A,sizeof(char),33U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1861
if((_tmp307->width != 0  && _tmp308->width == 0  || 
_tmp308->width != 0  && _tmp307->width == 0) || 
(_tmp307->width != 0  && _tmp308->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpAB8=(struct Cyc_Absyn_Exp*)_check_null(_tmp307->width);Cyc_Evexp_same_const_exp(_tmpAB8,(struct Cyc_Absyn_Exp*)_check_null(_tmp308->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp30B="(different bitfield widths on member)";_tag_dyneither(_tmp30B,sizeof(char),38U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1870
if((_tmp307->requires_clause != 0  && _tmp308->requires_clause == 0  || 
_tmp307->requires_clause == 0  && _tmp308->requires_clause != 0) || 
(_tmp307->requires_clause == 0  && _tmp308->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpAB9=(struct Cyc_Absyn_Exp*)_check_null(_tmp307->requires_clause);Cyc_Evexp_same_const_exp(_tmpAB9,(struct Cyc_Absyn_Exp*)_check_null(_tmp308->requires_clause));})){
# 1875
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp30C="(different @requires clauses on member)";_tag_dyneither(_tmp30C,sizeof(char),40U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1881
if(_tmp348 == 0  && _tmp34A == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp30D="(different number of members)";_tag_dyneither(_tmp30D,sizeof(char),30U);});
goto _LL11;}else{goto _LL32;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2C1.f2)->tag == 8U){_LL30: _tmp34F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2C1.f1)->f2;_tmp34E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2C1.f1)->f3;_tmp34D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2C1.f2)->f2;_tmp34C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2C1.f2)->f3;_LL31:
# 1887
 if(_tmp34E != _tmp34C){
Cyc_Tcutil_failure_reason=({const char*_tmp30E="(different abstract typedefs)";_tag_dyneither(_tmp30E,sizeof(char),30U);});
goto _LL11;}
# 1891
Cyc_Tcutil_failure_reason=({const char*_tmp30F="(type parameters to typedef differ)";_tag_dyneither(_tmp30F,sizeof(char),36U);});
Cyc_Tcutil_unify_list(_tmp34F,_tmp34D);
return;}else{goto _LL32;}default: _LL32: _LL33:
 goto _LL11;}}}_LL11:;}
# 1896
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1899
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpABB)(void*,void*)=cmp;void*_tmpABA=(void*)_check_null(a1);_tmpABB(_tmpABA,(void*)_check_null(a2));});}
# 1906
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp350=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp351=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp350,_tmp351);}
# 1912
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp352=tqt1;struct Cyc_Absyn_Tqual _tmp358;void*_tmp357;_LL1: _tmp358=_tmp352->f1;_tmp357=_tmp352->f2;_LL2:;{
struct _tuple12*_tmp353=tqt2;struct Cyc_Absyn_Tqual _tmp356;void*_tmp355;_LL4: _tmp356=_tmp353->f1;_tmp355=_tmp353->f2;_LL5:;{
int _tmp354=Cyc_Tcutil_tqual_cmp(_tmp358,_tmp356);
if(_tmp354 != 0)return _tmp354;
return Cyc_Tcutil_typecmp(_tmp357,_tmp355);};};}
# 1920
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp359=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp359 != 0)return _tmp359;{
int _tmp35A=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp35A != 0)return _tmp35A;{
int _tmp35B=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp35B != 0)return _tmp35B;{
int _tmp35C=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp35C != 0)return _tmp35C;
_tmp35C=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp35C != 0)return _tmp35C;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1934
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp35D=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp35D != 0)return _tmp35D;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1943
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp35E=({int _tmpABC=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpABC,Cyc_Tcutil_type_case_number(t2));});
if(_tmp35E != 0)
return _tmp35E;{
# 1952
struct _tuple0 _tmp35F=({struct _tuple0 _tmp984;_tmp984.f1=t2,_tmp984.f2=t1;_tmp984;});struct _tuple0 _tmp360=_tmp35F;struct Cyc_Absyn_Exp*_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3A9;enum Cyc_Absyn_AggrKind _tmp3A8;struct Cyc_List_List*_tmp3A7;enum Cyc_Absyn_AggrKind _tmp3A6;struct Cyc_List_List*_tmp3A5;struct Cyc_List_List*_tmp3A4;struct Cyc_List_List*_tmp3A3;struct Cyc_Absyn_FnInfo _tmp3A2;struct Cyc_Absyn_FnInfo _tmp3A1;void*_tmp3A0;struct Cyc_Absyn_Tqual _tmp39F;struct Cyc_Absyn_Exp*_tmp39E;void*_tmp39D;void*_tmp39C;struct Cyc_Absyn_Tqual _tmp39B;struct Cyc_Absyn_Exp*_tmp39A;void*_tmp399;void*_tmp398;struct Cyc_Absyn_Tqual _tmp397;void*_tmp396;void*_tmp395;void*_tmp394;void*_tmp393;void*_tmp392;struct Cyc_Absyn_Tqual _tmp391;void*_tmp390;void*_tmp38F;void*_tmp38E;void*_tmp38D;struct Cyc_Absyn_Tvar*_tmp38C;struct Cyc_Absyn_Tvar*_tmp38B;void*_tmp38A;struct Cyc_List_List*_tmp389;void*_tmp388;struct Cyc_List_List*_tmp387;switch(*((int*)_tmp360.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp360.f2)->tag == 0U){_LL1: _tmp38A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp360.f1)->f1;_tmp389=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp360.f1)->f2;_tmp388=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp360.f2)->f1;_tmp387=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp360.f2)->f2;_LL2: {
# 1954
int _tmp361=Cyc_Tcutil_tycon_cmp(_tmp38A,_tmp388);
if(_tmp361 != 0)return _tmp361;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp389,_tmp387);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp360.f2)->tag == 1U){_LL3: _LL4:
# 1958
({void*_tmp362=0U;({struct _dyneither_ptr _tmpABD=({const char*_tmp363="typecmp: can only compare closed types";_tag_dyneither(_tmp363,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpABD,_tag_dyneither(_tmp362,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp360.f2)->tag == 2U){_LL5: _tmp38C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp360.f1)->f1;_tmp38B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp360.f2)->f1;_LL6:
# 1962
 return Cyc_Core_intcmp(_tmp38B->identity,_tmp38C->identity);}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f2)->tag == 3U){_LL7: _tmp398=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f1)->f1).elt_type;_tmp397=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f1)->f1).elt_tq;_tmp396=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f1)->f1).ptr_atts).rgn;_tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f1)->f1).ptr_atts).nullable;_tmp394=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f1)->f1).ptr_atts).bounds;_tmp393=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f1)->f1).ptr_atts).zero_term;_tmp392=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f2)->f1).elt_type;_tmp391=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f2)->f1).elt_tq;_tmp390=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f2)->f1).ptr_atts).rgn;_tmp38F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f2)->f1).ptr_atts).nullable;_tmp38E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f2)->f1).ptr_atts).bounds;_tmp38D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360.f2)->f1).ptr_atts).zero_term;_LL8: {
# 1966
int _tmp364=Cyc_Tcutil_typecmp(_tmp392,_tmp398);
if(_tmp364 != 0)return _tmp364;{
int _tmp365=Cyc_Tcutil_typecmp(_tmp390,_tmp396);
if(_tmp365 != 0)return _tmp365;{
int _tmp366=Cyc_Tcutil_tqual_cmp(_tmp391,_tmp397);
if(_tmp366 != 0)return _tmp366;{
int _tmp367=Cyc_Tcutil_typecmp(_tmp38E,_tmp394);
if(_tmp367 != 0)return _tmp367;{
int _tmp368=Cyc_Tcutil_typecmp(_tmp38D,_tmp393);
if(_tmp368 != 0)return _tmp368;{
int _tmp369=Cyc_Tcutil_typecmp(_tmp38E,_tmp394);
if(_tmp369 != 0)return _tmp369;
return Cyc_Tcutil_typecmp(_tmp38F,_tmp395);};};};};};}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp360.f2)->tag == 4U){_LL9: _tmp3A0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp360.f1)->f1).elt_type;_tmp39F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp360.f1)->f1).tq;_tmp39E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp360.f1)->f1).num_elts;_tmp39D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp360.f1)->f1).zero_term;_tmp39C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp360.f2)->f1).elt_type;_tmp39B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp360.f2)->f1).tq;_tmp39A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp360.f2)->f1).num_elts;_tmp399=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp360.f2)->f1).zero_term;_LLA: {
# 1982
int _tmp36A=Cyc_Tcutil_tqual_cmp(_tmp39B,_tmp39F);
if(_tmp36A != 0)return _tmp36A;{
int _tmp36B=Cyc_Tcutil_typecmp(_tmp39C,_tmp3A0);
if(_tmp36B != 0)return _tmp36B;{
int _tmp36C=Cyc_Tcutil_typecmp(_tmp39D,_tmp399);
if(_tmp36C != 0)return _tmp36C;
if(_tmp39E == _tmp39A)return 0;
if(_tmp39E == 0  || _tmp39A == 0)
({void*_tmp36D=0U;({struct _dyneither_ptr _tmpABE=({const char*_tmp36E="missing expression in array index";_tag_dyneither(_tmp36E,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpABE,_tag_dyneither(_tmp36D,sizeof(void*),0U));});});
# 1992
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp39E,_tmp39A);};};}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp360.f2)->tag == 5U){_LLB: _tmp3A2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp360.f1)->f1;_tmp3A1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp360.f2)->f1;_LLC:
# 1995
 if(Cyc_Tcutil_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp3A2.ret_type,_tmp3A1.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(_tmp3A2.ret_tqual,_tmp3A1.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp36F=_tmp3A2.args;
struct Cyc_List_List*_tmp370=_tmp3A1.args;
for(0;_tmp36F != 0  && _tmp370 != 0;(_tmp36F=_tmp36F->tl,_tmp370=_tmp370->tl)){
struct _tuple10 _tmp371=*((struct _tuple10*)_tmp36F->hd);struct _tuple10 _tmp372=_tmp371;struct Cyc_Absyn_Tqual _tmp378;void*_tmp377;_LL18: _tmp378=_tmp372.f2;_tmp377=_tmp372.f3;_LL19:;{
struct _tuple10 _tmp373=*((struct _tuple10*)_tmp370->hd);struct _tuple10 _tmp374=_tmp373;struct Cyc_Absyn_Tqual _tmp376;void*_tmp375;_LL1B: _tmp376=_tmp374.f2;_tmp375=_tmp374.f3;_LL1C:;
r=Cyc_Tcutil_tqual_cmp(_tmp378,_tmp376);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp377,_tmp375);
if(r != 0)return r;};}
# 2010
if(_tmp36F != 0)return 1;
if(_tmp370 != 0)return - 1;
if(_tmp3A2.c_varargs  && !_tmp3A1.c_varargs)return 1;
if(!_tmp3A2.c_varargs  && _tmp3A1.c_varargs)return - 1;
if(_tmp3A2.cyc_varargs != 0 & _tmp3A1.cyc_varargs == 0)return 1;
if(_tmp3A2.cyc_varargs == 0 & _tmp3A1.cyc_varargs != 0)return - 1;
if(_tmp3A2.cyc_varargs != 0 & _tmp3A1.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmpABF=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3A2.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmpABF,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3A1.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((_tmp3A2.cyc_varargs)->type,(_tmp3A1.cyc_varargs)->type);
if(r != 0)return r;
if((_tmp3A2.cyc_varargs)->inject  && !(_tmp3A1.cyc_varargs)->inject)return 1;
if(!(_tmp3A2.cyc_varargs)->inject  && (_tmp3A1.cyc_varargs)->inject)return - 1;}
# 2024
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp3A2.effect,_tmp3A1.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp379=_tmp3A2.rgn_po;
struct Cyc_List_List*_tmp37A=_tmp3A1.rgn_po;
for(0;_tmp379 != 0  && _tmp37A != 0;(_tmp379=_tmp379->tl,_tmp37A=_tmp37A->tl)){
struct _tuple0 _tmp37B=*((struct _tuple0*)_tmp379->hd);struct _tuple0 _tmp37C=_tmp37B;void*_tmp382;void*_tmp381;_LL1E: _tmp382=_tmp37C.f1;_tmp381=_tmp37C.f2;_LL1F:;{
struct _tuple0 _tmp37D=*((struct _tuple0*)_tmp37A->hd);struct _tuple0 _tmp37E=_tmp37D;void*_tmp380;void*_tmp37F;_LL21: _tmp380=_tmp37E.f1;_tmp37F=_tmp37E.f2;_LL22:;
r=Cyc_Tcutil_typecmp(_tmp382,_tmp380);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp381,_tmp37F);if(r != 0)return r;};}
# 2034
if(_tmp379 != 0)return 1;
if(_tmp37A != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3A2.requires_clause,_tmp3A1.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3A2.ensures_clause,_tmp3A1.ensures_clause);
if(r != 0)return r;
# 2042
({void*_tmp383=0U;({struct _dyneither_ptr _tmpAC0=({const char*_tmp384="typecmp: function type comparison should never get here!";_tag_dyneither(_tmp384,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAC0,_tag_dyneither(_tmp383,sizeof(void*),0U));});});};};};}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp360.f2)->tag == 6U){_LLD: _tmp3A4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp360.f1)->f1;_tmp3A3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp360.f2)->f1;_LLE:
# 2045
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp3A3,_tmp3A4);}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp360.f2)->tag == 7U){_LLF: _tmp3A8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp360.f1)->f1;_tmp3A7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp360.f1)->f2;_tmp3A6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp360.f2)->f1;_tmp3A5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp360.f2)->f2;_LL10:
# 2048
 if((int)_tmp3A6 != (int)_tmp3A8){
if((int)_tmp3A6 == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp3A5,_tmp3A7);}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp360.f2)->tag == 9U){_LL11: _tmp3AA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp360.f1)->f1;_tmp3A9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp360.f2)->f1;_LL12:
# 2053
 _tmp3AC=_tmp3AA;_tmp3AB=_tmp3A9;goto _LL14;}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp360.f2)->tag == 11U){_LL13: _tmp3AC=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp360.f1)->f1;_tmp3AB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp360.f2)->f1;_LL14:
# 2055
 return Cyc_Evexp_const_exp_cmp(_tmp3AC,_tmp3AB);}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp385=0U;({struct _dyneither_ptr _tmpAC1=({const char*_tmp386="Unmatched case in typecmp";_tag_dyneither(_tmp386,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAC1,_tag_dyneither(_tmp385,sizeof(void*),0U));});});}_LL0:;};};}
# 2062
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp3AD=({struct _tuple0 _tmp986;_tmp986.f1=t1,_tmp986.f2=t2;_tmp986;});struct _tuple0 _tmp3AE=_tmp3AD;void*_tmp3B4;void*_tmp3B3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE.f2)->tag == 0U){_LL1: _tmp3B4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE.f1)->f1;_tmp3B3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE.f2)->f1;_LL2: {
# 2067
struct _tuple0 _tmp3AF=({struct _tuple0 _tmp985;_tmp985.f1=_tmp3B4,_tmp985.f2=_tmp3B3;_tmp985;});struct _tuple0 _tmp3B0=_tmp3AF;int _tmp3B2;int _tmp3B1;switch(*((int*)_tmp3B0.f1)){case 2U: switch(*((int*)_tmp3B0.f2)){case 2U: _LL6: _tmp3B2=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3B0.f1)->f1;_tmp3B1=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3B0.f2)->f1;_LL7:
# 2069
 return _tmp3B1 < _tmp3B2;case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B0.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B0.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B0.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp3B0.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B0.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 2076
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 2081
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 2084
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3B0.f2)->f1 == 0){_LL14: _LL15:
# 2079
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp3B0.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B0.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 2077
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 2082
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 2085
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3B0.f2)->f1 == 0){_LL16: _LL17:
# 2080
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B0.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B0.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 2086
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B0.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B0.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 2083
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 2087
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 2089
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2091
 return 0;}_LL0:;};}
# 2097
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2100
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->v=t1;_tmp3B5;});}}}
# 2109
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2113
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp3B8=({struct Cyc_String_pa_PrintArg_struct _tmp988;_tmp988.tag=0U,({
struct _dyneither_ptr _tmpAC2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp988.f1=_tmpAC2;});_tmp988;});struct Cyc_String_pa_PrintArg_struct _tmp3B9=({struct Cyc_String_pa_PrintArg_struct _tmp987;_tmp987.tag=0U,({struct _dyneither_ptr _tmpAC3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp987.f1=_tmpAC3;});_tmp987;});void*_tmp3B6[2U];_tmp3B6[0]=& _tmp3B8,_tmp3B6[1]=& _tmp3B9;({unsigned int _tmpAC5=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpAC4=({const char*_tmp3B7="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp3B7,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpAC5,_tmpAC4,_tag_dyneither(_tmp3B6,sizeof(void*),2U));});});
return 0;}}}
# 2119
return 1;}
# 2124
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(te,e)){
void*_tmp3BA=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3BB=_tmp3BA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BB)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2130
return 1;}
# 2134
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 2138
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp3BC=0U;({unsigned int _tmpAC7=e->loc;struct _dyneither_ptr _tmpAC6=({const char*_tmp3BD="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3BD,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpAC7,_tmpAC6,_tag_dyneither(_tmp3BC,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2144
return 0;}
# 2148
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 2152
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp3BE=0U;({unsigned int _tmpAC9=e->loc;struct _dyneither_ptr _tmpAC8=({const char*_tmp3BF="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3BF,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpAC9,_tmpAC8,_tag_dyneither(_tmp3BE,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2158
return 0;}
# 2163
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Tcutil_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 2169
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmpACA=t;Cyc_Tcutil_unify(_tmpACA,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 2174
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp3C0=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);
void*_tmp3C1=_tmp3C0;struct Cyc_Absyn_Exp*_tmp3C3;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3C1)->tag == 9U){_LL1: _tmp3C3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3C1)->f1;_LL2:
 return _tmp3C3;}else{_LL3: _LL4: {
# 2179
struct Cyc_Absyn_Exp*_tmp3C2=Cyc_Absyn_valueof_exp(_tmp3C0,0U);
_tmp3C2->topt=Cyc_Absyn_uint_type;
return _tmp3C2;}}_LL0:;}
# 2188
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Tcutil_unify(def,b);{
void*_tmp3C4=Cyc_Tcutil_compress(b);void*_tmp3C5=_tmp3C4;struct Cyc_List_List*_tmp3C9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C5)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C5)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp3C9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C5)->f2;_LL4:
 return Cyc_Tcutil_get_thin_bound(_tmp3C9);default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp3C8=({struct Cyc_String_pa_PrintArg_struct _tmp989;_tmp989.tag=0U,({struct _dyneither_ptr _tmpACB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(b));_tmp989.f1=_tmpACB;});_tmp989;});void*_tmp3C6[1U];_tmp3C6[0]=& _tmp3C8;({struct _dyneither_ptr _tmpACC=({const char*_tmp3C7="get_bounds_exp: %s";_tag_dyneither(_tmp3C7,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpACC,_tag_dyneither(_tmp3C6,sizeof(void*),1U));});});}_LL0:;};}
# 2197
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp3CA=Cyc_Tcutil_compress(t);void*_tmp3CB=_tmp3CA;void*_tmp3CF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->tag == 3U){_LL1: _tmp3CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->f1).ptr_atts).bounds;_LL2:
# 2200
 return Cyc_Tcutil_get_bounds_exp(def,_tmp3CF);}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp3CE=({struct Cyc_String_pa_PrintArg_struct _tmp98A;_tmp98A.tag=0U,({struct _dyneither_ptr _tmpACD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp98A.f1=_tmpACD;});_tmp98A;});void*_tmp3CC[1U];_tmp3CC[0]=& _tmp3CE;({struct _dyneither_ptr _tmpACE=({const char*_tmp3CD="get_ptr_bounds_exp not pointer: %s";_tag_dyneither(_tmp3CD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpACE,_tag_dyneither(_tmp3CC,sizeof(void*),1U));});});}_LL0:;}
# 2206
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({struct Cyc_List_List*_tmpACF=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3D0->v=_tmpACF;});_tmp3D0;}));else{
# 2210
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->v=0;_tmp3D1;}));}}
# 2213
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));({struct Cyc_List_List*_tmpAD0=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3D2->v=_tmpAD0;});_tmp3D2;}));else{
# 2217
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->v=0;_tmp3D3;}));}}
# 2221
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple24{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 2229
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2231
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp3D4=({struct _tuple0 _tmp98E;_tmp98E.f1=t1,_tmp98E.f2=t2;_tmp98E;});struct _tuple0 _tmp3D5=_tmp3D4;void*_tmp3EA;struct Cyc_Absyn_Tqual _tmp3E9;struct Cyc_Absyn_Exp*_tmp3E8;void*_tmp3E7;void*_tmp3E6;struct Cyc_Absyn_Tqual _tmp3E5;struct Cyc_Absyn_Exp*_tmp3E4;void*_tmp3E3;struct Cyc_Absyn_PtrInfo _tmp3E2;struct Cyc_Absyn_PtrInfo _tmp3E1;switch(*((int*)_tmp3D5.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D5.f2)->tag == 3U){_LL1: _tmp3E2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D5.f1)->f1;_tmp3E1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D5.f2)->f1;_LL2: {
# 2235
int okay=1;
# 2237
if(!Cyc_Tcutil_unify((_tmp3E2.ptr_atts).nullable,(_tmp3E1.ptr_atts).nullable))
# 2239
okay=!Cyc_Tcutil_force_type2bool(0,(_tmp3E2.ptr_atts).nullable);
# 2241
if(!Cyc_Tcutil_unify((_tmp3E2.ptr_atts).bounds,(_tmp3E1.ptr_atts).bounds)){
struct _tuple24 _tmp3D6=({struct _tuple24 _tmp98B;({struct Cyc_Absyn_Exp*_tmpAD4=({void*_tmpAD3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpAD3,(_tmp3E2.ptr_atts).bounds);});_tmp98B.f1=_tmpAD4;}),({
struct Cyc_Absyn_Exp*_tmpAD2=({void*_tmpAD1=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpAD1,(_tmp3E1.ptr_atts).bounds);});_tmp98B.f2=_tmpAD2;});_tmp98B;});
# 2242
struct _tuple24 _tmp3D7=_tmp3D6;struct Cyc_Absyn_Exp*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DA;if(_tmp3D7.f2 == 0){_LLA: _LLB:
# 2245
 okay=1;goto _LL9;}else{if(_tmp3D7.f1 == 0){_LLC: _LLD:
# 2248
 if(Cyc_Tcutil_force_type2bool(0,(_tmp3E2.ptr_atts).zero_term) && ({
void*_tmpAD5=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpAD5,(_tmp3E1.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp3DB=_tmp3D7.f1;_tmp3DA=_tmp3D7.f2;_LLF:
# 2254
 okay=okay  && ({struct Cyc_Absyn_Exp*_tmpAD6=(struct Cyc_Absyn_Exp*)_check_null(_tmp3DA);Cyc_Evexp_lte_const_exp(_tmpAD6,(struct Cyc_Absyn_Exp*)_check_null(_tmp3DB));});
# 2258
if(!Cyc_Tcutil_force_type2bool(0,(_tmp3E1.ptr_atts).zero_term))
({void*_tmp3D8=0U;({unsigned int _tmpAD8=loc;struct _dyneither_ptr _tmpAD7=({const char*_tmp3D9="implicit cast to shorter array";_tag_dyneither(_tmp3D9,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpAD8,_tmpAD7,_tag_dyneither(_tmp3D8,sizeof(void*),0U));});});
goto _LL9;}}_LL9:;}
# 2265
okay=okay  && (!(_tmp3E2.elt_tq).real_const  || (_tmp3E1.elt_tq).real_const);
# 2268
if(!Cyc_Tcutil_unify((_tmp3E2.ptr_atts).rgn,(_tmp3E1.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp3E2.ptr_atts).rgn,(_tmp3E1.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp3DE=({struct Cyc_String_pa_PrintArg_struct _tmp98D;_tmp98D.tag=0U,({
struct _dyneither_ptr _tmpAD9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3E2.ptr_atts).rgn));_tmp98D.f1=_tmpAD9;});_tmp98D;});struct Cyc_String_pa_PrintArg_struct _tmp3DF=({struct Cyc_String_pa_PrintArg_struct _tmp98C;_tmp98C.tag=0U,({
struct _dyneither_ptr _tmpADA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3E1.ptr_atts).rgn));_tmp98C.f1=_tmpADA;});_tmp98C;});void*_tmp3DC[2U];_tmp3DC[0]=& _tmp3DE,_tmp3DC[1]=& _tmp3DF;({unsigned int _tmpADC=loc;struct _dyneither_ptr _tmpADB=({const char*_tmp3DD="implicit cast from region %s to region %s";_tag_dyneither(_tmp3DD,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpADC,_tmpADB,_tag_dyneither(_tmp3DC,sizeof(void*),2U));});});}else{
okay=0;}}
# 2278
okay=okay  && (Cyc_Tcutil_unify((_tmp3E2.ptr_atts).zero_term,(_tmp3E1.ptr_atts).zero_term) || 
# 2280
Cyc_Tcutil_force_type2bool(1,(_tmp3E2.ptr_atts).zero_term) && (_tmp3E1.elt_tq).real_const);{
# 2288
int _tmp3E0=
({void*_tmpADD=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpADD,(_tmp3E1.ptr_atts).bounds);}) && !
Cyc_Tcutil_force_type2bool(0,(_tmp3E1.ptr_atts).zero_term);
# 2294
okay=okay  && (Cyc_Tcutil_unify(_tmp3E2.elt_type,_tmp3E1.elt_type) || 
(_tmp3E0  && ((_tmp3E1.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3E1.elt_type)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp3E2.elt_type,_tmp3E1.elt_type));
# 2297
return okay;};}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D5.f2)->tag == 4U){_LL3: _tmp3EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D5.f1)->f1).elt_type;_tmp3E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D5.f1)->f1).tq;_tmp3E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D5.f1)->f1).num_elts;_tmp3E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D5.f1)->f1).zero_term;_tmp3E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D5.f2)->f1).elt_type;_tmp3E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D5.f2)->f1).tq;_tmp3E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D5.f2)->f1).num_elts;_tmp3E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D5.f2)->f1).zero_term;_LL4: {
# 2301
int okay;
# 2304
okay=Cyc_Tcutil_unify(_tmp3E7,_tmp3E3) && (
(_tmp3E8 != 0  && _tmp3E4 != 0) && Cyc_Evexp_same_const_exp(_tmp3E8,_tmp3E4));
# 2307
return(okay  && Cyc_Tcutil_unify(_tmp3EA,_tmp3E6)) && (!_tmp3E9.real_const  || _tmp3E5.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D5.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D5.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D5.f2)->f1)->tag == 1U){_LL5: _LL6:
# 2309
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 2311
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2315
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp3EB=Cyc_Tcutil_compress(t);void*_tmp3EC=_tmp3EB;void*_tmp3EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC)->tag == 3U){_LL1: _tmp3EF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC)->f1).elt_type;_LL2:
 return _tmp3EF;}else{_LL3: _LL4:
({void*_tmp3ED=0U;({struct _dyneither_ptr _tmpADE=({const char*_tmp3EE="pointer_elt_type";_tag_dyneither(_tmp3EE,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpADE,_tag_dyneither(_tmp3ED,sizeof(void*),0U));});});}_LL0:;}
# 2321
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp3F0=Cyc_Tcutil_compress(t);void*_tmp3F1=_tmp3F0;struct Cyc_Absyn_PtrAtts*_tmp3F4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1)->tag == 3U){_LL1: _tmp3F4=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1)->f1).ptr_atts;_LL2:
 return _tmp3F4->rgn;}else{_LL3: _LL4:
({void*_tmp3F2=0U;({struct _dyneither_ptr _tmpADF=({const char*_tmp3F3="pointer_elt_type";_tag_dyneither(_tmp3F3,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpADF,_tag_dyneither(_tmp3F2,sizeof(void*),0U));});});}_LL0:;}
# 2328
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp3F5=Cyc_Tcutil_compress(t);void*_tmp3F6=_tmp3F5;void*_tmp3F7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6)->tag == 3U){_LL1: _tmp3F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6)->f1).ptr_atts).rgn;_LL2:
# 2331
*rgn=_tmp3F7;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2340
static int Cyc_Tcutil_admits_zero(void*t){
void*_tmp3F8=Cyc_Tcutil_compress(t);void*_tmp3F9=_tmp3F8;void*_tmp3FB;void*_tmp3FA;switch(*((int*)_tmp3F9)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F9)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp3FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F9)->f1).ptr_atts).nullable;_tmp3FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F9)->f1).ptr_atts).bounds;_LL6:
# 2348
 return !Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp3FA) && Cyc_Tcutil_force_type2bool(0,_tmp3FB);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2354
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp3FC=e->r;void*_tmp3FD=_tmp3FC;void*_tmp401;struct Cyc_Absyn_Exp*_tmp400;struct _dyneither_ptr _tmp3FF;switch(*((int*)_tmp3FD)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp3FF=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1).Wchar_c).val;_LLC: {
# 2363
unsigned long _tmp3FE=Cyc_strlen((struct _dyneither_ptr)_tmp3FF);
int i=0;
if(_tmp3FE >= (unsigned long)2  && (int)*((const char*)_check_dyneither_subscript(_tmp3FF,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_dyneither_subscript(_tmp3FF,sizeof(char),1))== (int)'0')i=2;else{
if(((int)*((const char*)_check_dyneither_subscript(_tmp3FF,sizeof(char),1))== (int)'x'  && _tmp3FE >= (unsigned long)3) && (int)*((const char*)_check_dyneither_subscript(_tmp3FF,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp3FE;++ i){
if((int)*((const char*)_check_dyneither_subscript(_tmp3FF,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 2373
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2361
 return 1;case 14U: _LLD: _tmp401=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_tmp400=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2;_LLE:
# 2374
 return Cyc_Tcutil_is_zero(_tmp400) && Cyc_Tcutil_admits_zero(_tmp401);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2379
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 2388
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2393
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2398
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 2407
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2412
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2417
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp402=ka;enum Cyc_Absyn_KindQual _tmp40B;enum Cyc_Absyn_AliasQual _tmp40A;_LL1: _tmp40B=_tmp402->kind;_tmp40A=_tmp402->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp403=_tmp40A;switch(_tmp403){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2421
enum Cyc_Absyn_KindQual _tmp404=_tmp40B;switch(_tmp404){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2432
{enum Cyc_Absyn_KindQual _tmp405=_tmp40B;switch(_tmp405){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 2439
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 2441
{enum Cyc_Absyn_KindQual _tmp406=_tmp40B;switch(_tmp406){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 2448
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 2451
({struct Cyc_String_pa_PrintArg_struct _tmp409=({struct Cyc_String_pa_PrintArg_struct _tmp98F;_tmp98F.tag=0U,({struct _dyneither_ptr _tmpAE0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp98F.f1=_tmpAE0;});_tmp98F;});void*_tmp407[1U];_tmp407[0]=& _tmp409;({struct _dyneither_ptr _tmpAE1=({const char*_tmp408="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp408,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAE1,_tag_dyneither(_tmp407,sizeof(void*),1U));});});}
# 2454
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->tag=0U,_tmp40C->f1=k;_tmp40C;});}
# 2457
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp40D=_cycalloc(sizeof(*_tmp40D));({void*_tmpAE2=Cyc_Tcutil_kind_to_bound(k);_tmp40D->v=_tmpAE2;});_tmp40D;});}
# 2463
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpAE3=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmpAE3;});{
struct Cyc_Core_Opt*_tmp40E=Cyc_Tcenv_lookup_opt_type_vars(te);
void*_tmp40F=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp992;({void*_tmpAE8=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp40E);_tmp992.elt_type=_tmpAE8;}),({
struct Cyc_Absyn_Tqual _tmpAE7=Cyc_Absyn_empty_tqual(0U);_tmp992.elt_tq=_tmpAE7;}),
({void*_tmpAE6=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp40E);(_tmp992.ptr_atts).rgn=_tmpAE6;}),(_tmp992.ptr_atts).nullable=Cyc_Absyn_true_type,({
# 2471
void*_tmpAE5=Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,_tmp40E);(_tmp992.ptr_atts).bounds=_tmpAE5;}),({
void*_tmpAE4=Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,_tmp40E);(_tmp992.ptr_atts).zero_term=_tmpAE4;}),(_tmp992.ptr_atts).ptrloc=0;_tmp992;}));
e1->topt=_tmp40F;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp412=({struct Cyc_String_pa_PrintArg_struct _tmp991;_tmp991.tag=0U,({
struct _dyneither_ptr _tmpAE9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp991.f1=_tmpAE9;});_tmp991;});struct Cyc_String_pa_PrintArg_struct _tmp413=({struct Cyc_String_pa_PrintArg_struct _tmp990;_tmp990.tag=0U,({struct _dyneither_ptr _tmpAEA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc));_tmp990.f1=_tmpAEA;});_tmp990;});void*_tmp410[2U];_tmp410[0]=& _tmp412,_tmp410[1]=& _tmp413;({struct _dyneither_ptr _tmpAEB=({const char*_tmp411="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp411,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAEB,_tag_dyneither(_tmp410,sizeof(void*),2U));});});
return retv;};};}
# 2481
return 0;}
# 2484
int Cyc_Tcutil_warn_alias_coerce=0;
# 2490
struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2494
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpAEE=({struct _dyneither_ptr*_tmp426=_cycalloc(sizeof(*_tmp426));({struct _dyneither_ptr _tmpAED=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp425=({struct Cyc_Int_pa_PrintArg_struct _tmp994;_tmp994.tag=1U,_tmp994.f1=(unsigned long)counter ++;_tmp994;});void*_tmp423[1U];_tmp423[0]=& _tmp425;({struct _dyneither_ptr _tmpAEC=({const char*_tmp424="__aliasvar%d";_tag_dyneither(_tmp424,sizeof(char),13U);});Cyc_aprintf(_tmpAEC,_tag_dyneither(_tmp423,sizeof(void*),1U));});});*_tmp426=_tmpAED;});_tmp426;});_tmp427->f2=_tmpAEE;});_tmp427;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpAEF=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->tag=4U,_tmp422->f1=vd;_tmp422;});Cyc_Absyn_varb_exp(_tmpAEF,e->loc);});
# 2503
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2505
{void*_tmp414=Cyc_Tcutil_compress(e_type);void*_tmp415=_tmp414;void*_tmp421;struct Cyc_Absyn_Tqual _tmp420;void*_tmp41F;void*_tmp41E;void*_tmp41D;void*_tmp41C;struct Cyc_Absyn_PtrLoc*_tmp41B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->tag == 3U){_LL1: _tmp421=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).elt_type;_tmp420=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).elt_tq;_tmp41F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).ptr_atts).rgn;_tmp41E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).ptr_atts).nullable;_tmp41D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).ptr_atts).bounds;_tmp41C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).ptr_atts).zero_term;_tmp41B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415)->f1).ptr_atts).ptrloc;_LL2:
# 2507
{void*_tmp416=Cyc_Tcutil_compress(_tmp41F);void*_tmp417=_tmp416;void**_tmp41A;struct Cyc_Core_Opt*_tmp419;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp417)->tag == 1U){_LL6: _tmp41A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp417)->f2;_tmp419=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp417)->f4;_LL7: {
# 2509
void*_tmp418=Cyc_Absyn_var_type(tv);
*_tmp41A=_tmp418;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2514
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2518
e->topt=0;
vd->initializer=0;{
# 2522
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2524
return({struct _tuple14 _tmp993;_tmp993.f1=d,_tmp993.f2=ve;_tmp993;});};}
# 2529
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned int loc){
# 2532
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_type,0)) && 
Cyc_Tcutil_is_pointer_type(e_type)){
# 2537
void*_tmp428=Cyc_Tcutil_compress(wants_type);void*_tmp429=_tmp428;void*_tmp42B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->tag == 3U){_LL1: _tmp42B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).ptr_atts).rgn;_LL2:
# 2539
 if(Cyc_Tcutil_is_heap_rgn_type(_tmp42B))return 0;{
struct Cyc_Absyn_Kind*_tmp42A=Cyc_Tcutil_type_kind(_tmp42B);
return(int)_tmp42A->kind == (int)Cyc_Absyn_RgnKind  && (int)_tmp42A->aliasqual == (int)Cyc_Absyn_Aliasable;};}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2545
return 0;}
# 2549
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2554
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2556
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2558
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp42E=({struct Cyc_String_pa_PrintArg_struct _tmp996;_tmp996.tag=0U,({
struct _dyneither_ptr _tmpAF0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp996.f1=_tmpAF0;});_tmp996;});struct Cyc_String_pa_PrintArg_struct _tmp42F=({struct Cyc_String_pa_PrintArg_struct _tmp995;_tmp995.tag=0U,({struct _dyneither_ptr _tmpAF1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp995.f1=_tmpAF1;});_tmp995;});void*_tmp42C[2U];_tmp42C[0]=& _tmp42E,_tmp42C[1]=& _tmp42F;({unsigned int _tmpAF3=e->loc;struct _dyneither_ptr _tmpAF2=({const char*_tmp42D="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp42D,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpAF3,_tmpAF2,_tag_dyneither(_tmp42C,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2566
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp432=({struct Cyc_String_pa_PrintArg_struct _tmp999;_tmp999.tag=0U,({
struct _dyneither_ptr _tmpAF4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp999.f1=_tmpAF4;});_tmp999;});struct Cyc_String_pa_PrintArg_struct _tmp433=({struct Cyc_String_pa_PrintArg_struct _tmp998;_tmp998.tag=0U,({struct _dyneither_ptr _tmpAF5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp998.f1=_tmpAF5;});_tmp998;});struct Cyc_String_pa_PrintArg_struct _tmp434=({struct Cyc_String_pa_PrintArg_struct _tmp997;_tmp997.tag=0U,({struct _dyneither_ptr _tmpAF6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp997.f1=_tmpAF6;});_tmp997;});void*_tmp430[3U];_tmp430[0]=& _tmp432,_tmp430[1]=& _tmp433,_tmp430[2]=& _tmp434;({unsigned int _tmpAF8=e->loc;struct _dyneither_ptr _tmpAF7=({const char*_tmp431="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp431,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpAF8,_tmpAF7,_tag_dyneither(_tmp430,sizeof(void*),3U));});});
*alias_coercion=1;}
# 2573
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((int)(c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 2580
if((int)c != (int)1U)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp437=({struct Cyc_String_pa_PrintArg_struct _tmp99B;_tmp99B.tag=0U,({
struct _dyneither_ptr _tmpAF9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp99B.f1=_tmpAF9;});_tmp99B;});struct Cyc_String_pa_PrintArg_struct _tmp438=({struct Cyc_String_pa_PrintArg_struct _tmp99A;_tmp99A.tag=0U,({struct _dyneither_ptr _tmpAFA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp99A.f1=_tmpAFA;});_tmp99A;});void*_tmp435[2U];_tmp435[0]=& _tmp437,_tmp435[1]=& _tmp438;({unsigned int _tmpAFC=e->loc;struct _dyneither_ptr _tmpAFB=({const char*_tmp436="implicit cast from %s to %s";_tag_dyneither(_tmp436,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpAFC,_tmpAFB,_tag_dyneither(_tmp435,sizeof(void*),2U));});});
return 1;}else{
# 2586
return 0;}}}}}
# 2593
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2596
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2610 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple25{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2614
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple25*env,struct Cyc_Absyn_Aggrfield*x){
# 2617
struct _tuple25 _tmp439=*env;struct _tuple25 _tmp43A=_tmp439;struct Cyc_List_List*_tmp442;struct _RegionHandle*_tmp441;struct Cyc_Tcenv_Tenv*_tmp440;int _tmp43F;_LL1: _tmp442=_tmp43A.f1;_tmp441=_tmp43A.f2;_tmp440=_tmp43A.f3;_tmp43F=_tmp43A.f4;_LL2:;{
# 2619
void*_tmp43B=_tmp442 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp441,_tmp442,x->type);
struct Cyc_List_List*_tmp43C=Cyc_Tcutil_flatten_type(_tmp441,_tmp43F,_tmp440,_tmp43B);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp43C)== 1)
return({struct Cyc_List_List*_tmp43E=_region_malloc(_tmp441,sizeof(*_tmp43E));({struct _tuple12*_tmpAFD=({struct _tuple12*_tmp43D=_region_malloc(_tmp441,sizeof(*_tmp43D));_tmp43D->f1=x->tq,_tmp43D->f2=_tmp43B;_tmp43D;});_tmp43E->hd=_tmpAFD;}),_tmp43E->tl=0;_tmp43E;});else{
return _tmp43C;}};}struct _tuple26{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2625
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple26*env,struct _tuple12*x){
# 2627
struct _tuple26 _tmp443=*env;struct _tuple26 _tmp444=_tmp443;struct _RegionHandle*_tmp44E;struct Cyc_Tcenv_Tenv*_tmp44D;int _tmp44C;_LL1: _tmp44E=_tmp444.f1;_tmp44D=_tmp444.f2;_tmp44C=_tmp444.f3;_LL2:;{
struct _tuple12 _tmp445=*x;struct _tuple12 _tmp446=_tmp445;struct Cyc_Absyn_Tqual _tmp44B;void*_tmp44A;_LL4: _tmp44B=_tmp446.f1;_tmp44A=_tmp446.f2;_LL5:;{
struct Cyc_List_List*_tmp447=Cyc_Tcutil_flatten_type(_tmp44E,_tmp44C,_tmp44D,_tmp44A);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp447)== 1)
return({struct Cyc_List_List*_tmp449=_region_malloc(_tmp44E,sizeof(*_tmp449));({struct _tuple12*_tmpAFE=({struct _tuple12*_tmp448=_region_malloc(_tmp44E,sizeof(*_tmp448));_tmp448->f1=_tmp44B,_tmp448->f2=_tmp44A;_tmp448;});_tmp449->hd=_tmpAFE;}),_tmp449->tl=0;_tmp449;});else{
return _tmp447;}};};}
# 2634
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2638
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp44F=t1;struct Cyc_List_List*_tmp46E;struct Cyc_List_List*_tmp46D;struct Cyc_Absyn_Aggrdecl*_tmp46C;struct Cyc_List_List*_tmp46B;switch(*((int*)_tmp44F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp46C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f1)->f1).KnownAggr).val;_tmp46B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f2;_LL6:
# 2656
 if((((int)_tmp46C->kind == (int)Cyc_Absyn_UnionA  || _tmp46C->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46C->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46C->impl))->rgn_po != 0)
# 2658
return({struct Cyc_List_List*_tmp45A=_region_malloc(r,sizeof(*_tmp45A));({struct _tuple12*_tmpB00=({struct _tuple12*_tmp459=_region_malloc(r,sizeof(*_tmp459));({struct Cyc_Absyn_Tqual _tmpAFF=Cyc_Absyn_empty_tqual(0U);_tmp459->f1=_tmpAFF;}),_tmp459->f2=t1;_tmp459;});_tmp45A->hd=_tmpB00;}),_tmp45A->tl=0;_tmp45A;});{
struct Cyc_List_List*_tmp45B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp46C->tvs,_tmp46B);
struct _tuple25 env=({struct _tuple25 _tmp99C;_tmp99C.f1=_tmp45B,_tmp99C.f2=r,_tmp99C.f3=te,_tmp99C.f4=flatten;_tmp99C;});
struct Cyc_List_List*_tmp45C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46C->impl))->fields;struct Cyc_List_List*_tmp45D=_tmp45C;struct Cyc_Absyn_Aggrfield*_tmp463;struct Cyc_List_List*_tmp462;if(_tmp45D == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp463=(struct Cyc_Absyn_Aggrfield*)_tmp45D->hd;_tmp462=_tmp45D->tl;_LL14: {
# 2664
struct Cyc_List_List*_tmp45E=Cyc_Tcutil_flatten_type_f(& env,_tmp463);
env.f4=0;{
struct Cyc_List_List*_tmp45F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp462);
struct Cyc_List_List*_tmp460=({struct Cyc_List_List*_tmp461=_region_malloc(r,sizeof(*_tmp461));_tmp461->hd=_tmp45E,_tmp461->tl=_tmp45F;_tmp461;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp460);};}}_LL10:;};}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp46D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp44F)->f1;_LL4: {
# 2643
struct _tuple26 _tmp450=({struct _tuple26 _tmp99D;_tmp99D.f1=r,_tmp99D.f2=te,_tmp99D.f3=flatten;_tmp99D;});
# 2645
struct Cyc_List_List*_tmp451=_tmp46D;struct _tuple12*_tmp458;struct Cyc_List_List*_tmp457;if(_tmp451 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp458=(struct _tuple12*)_tmp451->hd;_tmp457=_tmp451->tl;_LLF: {
# 2648
struct Cyc_List_List*_tmp452=Cyc_Tcutil_rcopy_tqt(& _tmp450,_tmp458);
_tmp450.f3=0;{
struct Cyc_List_List*_tmp453=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple26*,struct _tuple12*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp450,_tmp46D);
struct Cyc_List_List*_tmp454=({struct Cyc_List_List*_tmp456=_region_malloc(r,sizeof(*_tmp456));_tmp456->hd=_tmp452,_tmp456->tl=_tmp453;_tmp456;});
return({struct _RegionHandle*_tmpB01=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpB01,({struct Cyc_List_List*_tmp455=_region_malloc(r,sizeof(*_tmp455));_tmp455->hd=_tmp452,_tmp455->tl=_tmp453;_tmp455;}));});};}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp44F)->f1 == Cyc_Absyn_StructA){_LL7: _tmp46E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp44F)->f2;_LL8: {
# 2671
struct _tuple25 env=({struct _tuple25 _tmp99E;_tmp99E.f1=0,_tmp99E.f2=r,_tmp99E.f3=te,_tmp99E.f4=flatten;_tmp99E;});
struct Cyc_List_List*_tmp464=_tmp46E;struct Cyc_Absyn_Aggrfield*_tmp46A;struct Cyc_List_List*_tmp469;if(_tmp464 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp46A=(struct Cyc_Absyn_Aggrfield*)_tmp464->hd;_tmp469=_tmp464->tl;_LL19: {
# 2675
struct Cyc_List_List*_tmp465=Cyc_Tcutil_flatten_type_f(& env,_tmp46A);
env.f4=0;{
struct Cyc_List_List*_tmp466=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp469);
struct Cyc_List_List*_tmp467=({struct Cyc_List_List*_tmp468=_region_malloc(r,sizeof(*_tmp468));_tmp468->hd=_tmp465,_tmp468->tl=_tmp466;_tmp468;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp467);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2681
 goto _LL0;}_LL0:;};}
# 2684
return({struct Cyc_List_List*_tmp470=_region_malloc(r,sizeof(*_tmp470));({struct _tuple12*_tmpB03=({struct _tuple12*_tmp46F=_region_malloc(r,sizeof(*_tmp46F));({struct Cyc_Absyn_Tqual _tmpB02=Cyc_Absyn_empty_tqual(0U);_tmp46F->f1=_tmpB02;}),_tmp46F->f2=t1;_tmp46F;});_tmp470->hd=_tmpB03;}),_tmp470->tl=0;_tmp470;});}
# 2688
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp471=(void*)t->hd;void*_tmp472=_tmp471;switch(*((int*)_tmp472)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2694
 continue;default: _LL7: _LL8:
# 2696
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2699
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))return 0;}
# 2702
return 1;}
# 2705
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp473=({struct _tuple0 _tmp99F;({void*_tmpB05=Cyc_Tcutil_compress(t1);_tmp99F.f1=_tmpB05;}),({void*_tmpB04=Cyc_Tcutil_compress(t2);_tmp99F.f2=_tmpB04;});_tmp99F;});struct _tuple0 _tmp474=_tmp473;enum Cyc_Absyn_Size_of _tmp476;enum Cyc_Absyn_Size_of _tmp475;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474.f2)->f1)->tag == 1U){_LL1: _tmp476=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474.f1)->f1)->f2;_tmp475=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp474.f2)->f1)->f2;_LL2:
# 2708
 return((int)_tmp476 == (int)_tmp475  || (int)_tmp476 == (int)2U  && (int)_tmp475 == (int)3U) || 
(int)_tmp476 == (int)3U  && (int)_tmp475 == (int)Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2716
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2719
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp477=({struct _tuple0 _tmp9A0;_tmp9A0.f1=t1,_tmp9A0.f2=t2;_tmp9A0;});struct _tuple0 _tmp478=_tmp477;struct Cyc_Absyn_FnInfo _tmp4A2;struct Cyc_Absyn_FnInfo _tmp4A1;struct Cyc_Absyn_Datatypedecl*_tmp4A0;struct Cyc_Absyn_Datatypefield*_tmp49F;struct Cyc_List_List*_tmp49E;struct Cyc_Absyn_Datatypedecl*_tmp49D;struct Cyc_List_List*_tmp49C;void*_tmp49B;struct Cyc_Absyn_Tqual _tmp49A;void*_tmp499;void*_tmp498;void*_tmp497;void*_tmp496;void*_tmp495;struct Cyc_Absyn_Tqual _tmp494;void*_tmp493;void*_tmp492;void*_tmp491;void*_tmp490;switch(*((int*)_tmp478.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f2)->tag == 3U){_LL1: _tmp49B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f1)->f1).elt_type;_tmp49A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f1)->f1).elt_tq;_tmp499=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f1)->f1).ptr_atts).rgn;_tmp498=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f1)->f1).ptr_atts).nullable;_tmp497=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f1)->f1).ptr_atts).bounds;_tmp496=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f1)->f1).ptr_atts).zero_term;_tmp495=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f2)->f1).elt_type;_tmp494=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f2)->f1).elt_tq;_tmp493=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f2)->f1).ptr_atts).rgn;_tmp492=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f2)->f1).ptr_atts).nullable;_tmp491=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f2)->f1).ptr_atts).bounds;_tmp490=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478.f2)->f1).ptr_atts).zero_term;_LL2:
# 2731
 if(_tmp49A.real_const  && !_tmp494.real_const)
return 0;
# 2734
if((!Cyc_Tcutil_unify(_tmp498,_tmp492) && 
Cyc_Absyn_type2bool(0,_tmp498)) && !Cyc_Absyn_type2bool(0,_tmp492))
return 0;
# 2738
if((Cyc_Tcutil_unify(_tmp496,_tmp490) && !
Cyc_Absyn_type2bool(0,_tmp496)) && Cyc_Absyn_type2bool(0,_tmp490))
return 0;
# 2742
if((!Cyc_Tcutil_unify(_tmp499,_tmp493) && !Cyc_Tcenv_region_outlives(te,_tmp499,_tmp493)) && !
Cyc_Tcutil_subtype(te,assume,_tmp499,_tmp493))
return 0;
# 2746
if(!Cyc_Tcutil_unify(_tmp497,_tmp491)){
struct Cyc_Absyn_Exp*_tmp479=({void*_tmpB06=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB06,_tmp497);});
struct Cyc_Absyn_Exp*_tmp47A=({void*_tmpB07=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB07,_tmp491);});
if(_tmp479 != _tmp47A){
if((_tmp479 == 0  || _tmp47A == 0) || !Cyc_Evexp_lte_const_exp(_tmp47A,_tmp47A))
return 0;}}
# 2756
if(!_tmp494.real_const  && _tmp49A.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp495)))
return 0;}{
# 2762
int _tmp47B=
({void*_tmpB08=_tmp491;Cyc_Tcutil_unify(_tmpB08,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp490);
# 2767
return(_tmp47B  && ({struct Cyc_Tcenv_Tenv*_tmpB0C=te;struct Cyc_List_List*_tmpB0B=({struct Cyc_List_List*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct _tuple0*_tmpB09=({struct _tuple0*_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C->f1=t1,_tmp47C->f2=t2;_tmp47C;});_tmp47D->hd=_tmpB09;}),_tmp47D->tl=assume;_tmp47D;});void*_tmpB0A=_tmp49B;Cyc_Tcutil_ptrsubtype(_tmpB0C,_tmpB0B,_tmpB0A,_tmp495);}) || Cyc_Tcutil_unify(_tmp49B,_tmp495)) || Cyc_Tcutil_isomorphic(_tmp49B,_tmp495);};}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp4A0=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp49F=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp49E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f1)->f2;_tmp49D=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f2)->f1)->f1).KnownDatatype).val;_tmp49C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp478.f2)->f2;_LL4:
# 2774
 if(_tmp4A0 != _tmp49D  && Cyc_Absyn_qvar_cmp(_tmp4A0->name,_tmp49D->name)!= 0)return 0;
# 2776
if(({int _tmpB0D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49E);_tmpB0D != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49C);}))return 0;
for(0;_tmp49E != 0;(_tmp49E=_tmp49E->tl,_tmp49C=_tmp49C->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp49E->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp49C))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp478.f2)->tag == 5U){_LL5: _tmp4A2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp478.f1)->f1;_tmp4A1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp478.f2)->f1;_LL6:
# 2783
 if(_tmp4A2.tvars != 0  || _tmp4A1.tvars != 0){
struct Cyc_List_List*_tmp47E=_tmp4A2.tvars;
struct Cyc_List_List*_tmp47F=_tmp4A1.tvars;
if(({int _tmpB0E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp47E);_tmpB0E != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp47F);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp47E != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp47E->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp47F))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp481=_cycalloc(sizeof(*_tmp481));({struct _tuple15*_tmpB10=({struct _tuple15*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->f1=(struct Cyc_Absyn_Tvar*)_tmp47F->hd,({void*_tmpB0F=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp47E->hd);_tmp480->f2=_tmpB0F;});_tmp480;});_tmp481->hd=_tmpB10;}),_tmp481->tl=inst;_tmp481;});
_tmp47E=_tmp47E->tl;
_tmp47F=_tmp47F->tl;}
# 2794
if(inst != 0){
_tmp4A2.tvars=0;
_tmp4A1.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpB13=te;struct Cyc_List_List*_tmpB12=assume;void*_tmpB11=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482->tag=5U,_tmp482->f1=_tmp4A2;_tmp482;});Cyc_Tcutil_subtype(_tmpB13,_tmpB12,_tmpB11,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->tag=5U,_tmp483->f1=_tmp4A1;_tmp483;}));});}};}
# 2801
if(!Cyc_Tcutil_subtype(te,assume,_tmp4A2.ret_type,_tmp4A1.ret_type))return 0;{
struct Cyc_List_List*_tmp484=_tmp4A2.args;
struct Cyc_List_List*_tmp485=_tmp4A1.args;
# 2806
if(({int _tmpB14=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp484);_tmpB14 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp485);}))return 0;
# 2808
for(0;_tmp484 != 0;(_tmp484=_tmp484->tl,_tmp485=_tmp485->tl)){
struct _tuple10 _tmp486=*((struct _tuple10*)_tmp484->hd);struct _tuple10 _tmp487=_tmp486;struct Cyc_Absyn_Tqual _tmp48D;void*_tmp48C;_LLA: _tmp48D=_tmp487.f2;_tmp48C=_tmp487.f3;_LLB:;{
struct _tuple10 _tmp488=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp485))->hd);struct _tuple10 _tmp489=_tmp488;struct Cyc_Absyn_Tqual _tmp48B;void*_tmp48A;_LLD: _tmp48B=_tmp489.f2;_tmp48A=_tmp489.f3;_LLE:;
# 2812
if(_tmp48B.real_const  && !_tmp48D.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp48A,_tmp48C))
return 0;};}
# 2816
if(_tmp4A2.c_varargs != _tmp4A1.c_varargs)return 0;
if(_tmp4A2.cyc_varargs != 0  && _tmp4A1.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp48E=*_tmp4A2.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp48F=*_tmp4A1.cyc_varargs;
# 2821
if((_tmp48F.tq).real_const  && !(_tmp48E.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp48F.type,_tmp48E.type))
return 0;}else{
if(_tmp4A2.cyc_varargs != 0  || _tmp4A1.cyc_varargs != 0)return 0;}
# 2826
if(!({void*_tmpB15=(void*)_check_null(_tmp4A2.effect);Cyc_Tcutil_subset_effect(1,_tmpB15,(void*)_check_null(_tmp4A1.effect));}))return 0;
# 2828
if(!Cyc_Tcutil_sub_rgnpo(_tmp4A2.rgn_po,_tmp4A1.rgn_po))return 0;
# 2830
if(!Cyc_Tcutil_sub_attributes(_tmp4A2.attributes,_tmp4A1.attributes))return 0;
# 2832
if(!Cyc_Tcutil_check_logical_implication(_tmp4A1.requires_relns,_tmp4A2.requires_relns))
return 0;
# 2835
if(!Cyc_Tcutil_check_logical_implication(_tmp4A2.ensures_relns,_tmp4A1.ensures_relns))
return 0;
# 2838
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2850 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2852
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp4A3=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp4A4=_tmp4A3;struct Cyc_Absyn_Tqual _tmp4AA;void*_tmp4A9;_LL1: _tmp4AA=_tmp4A4->f1;_tmp4A9=_tmp4A4->f2;_LL2:;{
struct _tuple12*_tmp4A5=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp4A6=_tmp4A5;struct Cyc_Absyn_Tqual _tmp4A8;void*_tmp4A7;_LL4: _tmp4A8=_tmp4A6->f1;_tmp4A7=_tmp4A6->f2;_LL5:;
# 2859
if(_tmp4AA.real_const  && !_tmp4A8.real_const)return 0;
# 2861
if((_tmp4A8.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4A7))) && 
Cyc_Tcutil_subtype(te,assume,_tmp4A9,_tmp4A7))
# 2864
continue;
# 2866
if(Cyc_Tcutil_unify(_tmp4A9,_tmp4A7))
# 2868
continue;
# 2870
if(Cyc_Tcutil_isomorphic(_tmp4A9,_tmp4A7))
# 2872
continue;
# 2875
return 0;};};}
# 2877
return 1;}
# 2882
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2888
{void*_tmp4AB=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AB)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AB)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AB)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2891
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2893
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2897
void*_tmp4AC=t1;void*_tmp4D1;struct Cyc_Absyn_Enumdecl*_tmp4D0;void*_tmp4CF;struct Cyc_Absyn_Tqual _tmp4CE;struct Cyc_Absyn_Exp*_tmp4CD;void*_tmp4CC;void*_tmp4CB;struct Cyc_Absyn_Tqual _tmp4CA;void*_tmp4C9;void*_tmp4C8;void*_tmp4C7;void*_tmp4C6;switch(*((int*)_tmp4AC)){case 3U: _LLA: _tmp4CB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AC)->f1).elt_type;_tmp4CA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AC)->f1).elt_tq;_tmp4C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AC)->f1).ptr_atts).rgn;_tmp4C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AC)->f1).ptr_atts).nullable;_tmp4C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AC)->f1).ptr_atts).bounds;_tmp4C6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AC)->f1).ptr_atts).zero_term;_LLB:
# 2906
{void*_tmp4AD=t2;void*_tmp4BC;struct Cyc_Absyn_Tqual _tmp4BB;void*_tmp4BA;void*_tmp4B9;void*_tmp4B8;void*_tmp4B7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->tag == 3U){_LL19: _tmp4BC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).elt_type;_tmp4BB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).elt_tq;_tmp4BA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).ptr_atts).rgn;_tmp4B9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).ptr_atts).nullable;_tmp4B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).ptr_atts).bounds;_tmp4B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).ptr_atts).zero_term;_LL1A: {
# 2910
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp4AE=({struct Cyc_List_List*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));({struct _tuple0*_tmpB16=({struct _tuple0*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->f1=t1,_tmp4B5->f2=t2;_tmp4B5;});_tmp4B6->hd=_tmpB16;}),_tmp4B6->tl=0;_tmp4B6;});
int _tmp4AF=_tmp4BB.real_const  || !_tmp4CA.real_const;
# 2924 "tcutil.cyc"
int _tmp4B0=
({void*_tmpB17=_tmp4B8;Cyc_Tcutil_unify(_tmpB17,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp4B7);
# 2927
int _tmp4B1=_tmp4AF  && (
((_tmp4B0  && Cyc_Tcutil_ptrsubtype(te,_tmp4AE,_tmp4CB,_tmp4BC) || 
Cyc_Tcutil_unify(_tmp4CB,_tmp4BC)) || Cyc_Tcutil_isomorphic(_tmp4CB,_tmp4BC)) || Cyc_Tcutil_unify(_tmp4BC,Cyc_Absyn_void_type));
# 2931
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=Cyc_Tcutil_unify(_tmp4C6,_tmp4B7) || !Cyc_Absyn_type2bool(0,_tmp4B7);
# 2935
int _tmp4B2=_tmp4B1?0:((Cyc_Tcutil_is_bits_only_type(_tmp4CB) && Cyc_Tcutil_is_char_type(_tmp4BC)) && !
Cyc_Tcutil_force_type2bool(0,_tmp4B7)) && (
_tmp4BB.real_const  || !_tmp4CA.real_const);
int bounds_ok=Cyc_Tcutil_unify(_tmp4C7,_tmp4B8);
if(!bounds_ok  && !_tmp4B2){
struct Cyc_Absyn_Exp*_tmp4B3=({void*_tmpB18=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB18,_tmp4C7);});
struct Cyc_Absyn_Exp*_tmp4B4=({void*_tmpB19=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB19,_tmp4B8);});
if((_tmp4B3 != 0  && _tmp4B4 != 0) && Cyc_Evexp_lte_const_exp(_tmp4B4,_tmp4B3))
bounds_ok=1;else{
if(_tmp4B3 == 0  || _tmp4B4 == 0)
bounds_ok=1;}}{
# 2947
int t1_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4C8);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4B9);
if(t1_nullable  && !t2_nullable)
coercion=2U;
# 2954
if(((bounds_ok  && zeroterm_ok) && (_tmp4B1  || _tmp4B2)) && (
Cyc_Tcutil_unify(_tmp4C9,_tmp4BA) || Cyc_Tcenv_region_outlives(te,_tmp4C9,_tmp4BA)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2960
return Cyc_Absyn_Unknown_coercion;case 4U: _LLC: _tmp4CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AC)->f1).elt_type;_tmp4CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AC)->f1).tq;_tmp4CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AC)->f1).num_elts;_tmp4CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AC)->f1).zero_term;_LLD:
# 2962
{void*_tmp4BD=t2;void*_tmp4C1;struct Cyc_Absyn_Tqual _tmp4C0;struct Cyc_Absyn_Exp*_tmp4BF;void*_tmp4BE;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BD)->tag == 4U){_LL1E: _tmp4C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BD)->f1).elt_type;_tmp4C0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BD)->f1).tq;_tmp4BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BD)->f1).num_elts;_tmp4BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BD)->f1).zero_term;_LL1F: {
# 2964
int okay;
okay=
(((_tmp4CD != 0  && _tmp4BF != 0) && Cyc_Tcutil_unify(_tmp4CC,_tmp4BE)) && 
Cyc_Evexp_lte_const_exp(_tmp4BF,_tmp4CD)) && 
Cyc_Evexp_lte_const_exp(_tmp4CD,_tmp4BF);
return
# 2971
(okay  && Cyc_Tcutil_unify(_tmp4CF,_tmp4C1)) && (!_tmp4CE.real_const  || _tmp4C0.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2973
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2975
return Cyc_Absyn_Unknown_coercion;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AC)->f1)){case 15U: _LLE: _tmp4D0=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AC)->f1)->f2;_LLF:
# 2979
{void*_tmp4C2=t2;struct Cyc_Absyn_Enumdecl*_tmp4C3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C2)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C2)->f1)->tag == 15U){_LL23: _tmp4C3=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C2)->f1)->f2;_LL24:
# 2981
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4D0))->fields != 0  && ((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4C3))->fields != 0) && ({
int _tmpB1A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4D0->fields))->v);_tmpB1A >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4C3->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2987
goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2990
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AC)->f2 != 0){_LL14: _tmp4D1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AC)->f2)->hd;_LL15:
# 2993
{void*_tmp4C4=t2;void*_tmp4C5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C4)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C4)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C4)->f2 != 0){_LL28: _tmp4C5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C4)->f2)->hd;_LL29:
# 2995
 if(Cyc_Tcenv_region_outlives(te,_tmp4D1,_tmp4C5))return Cyc_Absyn_No_coercion;
goto _LL27;}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2999
return Cyc_Absyn_Unknown_coercion;}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;};}
# 3005
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_Absyn_copy_exp(e);
({void*_tmpB1B=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3->tag=14U,_tmp4D3->f1=t,_tmp4D3->f2=_tmp4D2,_tmp4D3->f3=0,_tmp4D3->f4=c;_tmp4D3;});e->r=_tmpB1B;});
e->topt=t;}}
# 3013
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple0 _tmp4D4=({struct _tuple0 _tmp9A2;({void*_tmpB1D=Cyc_Tcutil_compress(t1);_tmp9A2.f1=_tmpB1D;}),({void*_tmpB1C=Cyc_Tcutil_compress(t2);_tmp9A2.f2=_tmpB1C;});_tmp9A2;});struct _tuple0 _tmp4D5=_tmp4D4;void*_tmp4DB;void*_tmp4DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D5.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D5.f2)->tag == 0U){_LL1: _tmp4DB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D5.f1)->f1;_tmp4DA=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D5.f2)->f1;_LL2:
# 3016
{struct _tuple0 _tmp4D6=({struct _tuple0 _tmp9A1;_tmp9A1.f1=_tmp4DB,_tmp9A1.f2=_tmp4DA;_tmp9A1;});struct _tuple0 _tmp4D7=_tmp4D6;int _tmp4D9;int _tmp4D8;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4D7.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4D7.f2)->tag == 2U){_LL6: _tmp4D9=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f1;_tmp4D8=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f1;_LL7:
# 3018
 if(_tmp4D9 != 0  && _tmp4D9 != 1)return t1;else{
if(_tmp4D8 != 0  && _tmp4D8 != 1)return t2;else{
if(_tmp4D9 >= _tmp4D8)return t1;else{
return t2;}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4D7.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 3028
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4D7.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 3033
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 3026
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4D7.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 3035
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 3025
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 3029
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4D7.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 3034
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 3027
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4D7.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4D7.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 3036
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4D7.f1)->tag == 4U){_LL18: _LL19:
# 3031
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4D7.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 3037
 goto _LL5;}}}}}}_LL5:;}
# 3039
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3042
return Cyc_Absyn_sint_type;}
# 3052 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp4DC=({struct _tuple0 _tmp9A3;_tmp9A3.f1=c1,_tmp9A3.f2=c2;_tmp9A3;});struct _tuple0 _tmp4DD=_tmp4DC;struct Cyc_Core_Opt**_tmp4F1;struct Cyc_Absyn_Kind*_tmp4F0;struct Cyc_Core_Opt**_tmp4EF;struct Cyc_Absyn_Kind*_tmp4EE;struct Cyc_Core_Opt**_tmp4ED;struct Cyc_Absyn_Kind*_tmp4EC;struct Cyc_Absyn_Kind*_tmp4EB;struct Cyc_Core_Opt**_tmp4EA;struct Cyc_Core_Opt**_tmp4E9;struct Cyc_Absyn_Kind*_tmp4E8;struct Cyc_Core_Opt**_tmp4E7;struct Cyc_Absyn_Kind*_tmp4E6;struct Cyc_Absyn_Kind*_tmp4E5;struct Cyc_Absyn_Kind*_tmp4E4;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4DD.f1)->tag == 0U)switch(*((int*)_tmp4DD.f2)){case 0U: _LL1: _tmp4E5=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4DD.f1)->f1;_tmp4E4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4DD.f2)->f1;_LL2:
 return _tmp4E5 == _tmp4E4;case 1U: goto _LL3;default: _LL9: _tmp4E8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4DD.f1)->f1;_tmp4E7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4DD.f2)->f1;_tmp4E6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4DD.f2)->f2;_LLA:
# 3065
 if(Cyc_Tcutil_kind_leq(_tmp4E8,_tmp4E6)){
({struct Cyc_Core_Opt*_tmpB1E=({struct Cyc_Core_Opt*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->v=c1;_tmp4E1;});*_tmp4E7=_tmpB1E;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4DD.f2)->tag == 1U){_LL3: _tmp4E9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4DD.f2)->f1;_LL4:
# 3058
({struct Cyc_Core_Opt*_tmpB1F=({struct Cyc_Core_Opt*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE->v=c1;_tmp4DE;});*_tmp4E9=_tmpB1F;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4DD.f1)->tag == 1U){_LL5: _tmp4EA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4DD.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpB20=({struct Cyc_Core_Opt*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->v=c2;_tmp4DF;});*_tmp4EA=_tmpB20;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4DD.f2)->tag == 0U){_LL7: _tmp4ED=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4DD.f1)->f1;_tmp4EC=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4DD.f1)->f2;_tmp4EB=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4DD.f2)->f1;_LL8:
# 3061
 if(Cyc_Tcutil_kind_leq(_tmp4EB,_tmp4EC)){
({struct Cyc_Core_Opt*_tmpB21=({struct Cyc_Core_Opt*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->v=c2;_tmp4E0;});*_tmp4ED=_tmpB21;});return 1;}else{
return 0;}}else{_LLB: _tmp4F1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4DD.f1)->f1;_tmp4F0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4DD.f1)->f2;_tmp4EF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4DD.f2)->f1;_tmp4EE=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4DD.f2)->f2;_LLC:
# 3069
 if(Cyc_Tcutil_kind_leq(_tmp4F0,_tmp4EE)){
({struct Cyc_Core_Opt*_tmpB22=({struct Cyc_Core_Opt*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->v=c1;_tmp4E2;});*_tmp4EF=_tmpB22;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp4EE,_tmp4F0)){
({struct Cyc_Core_Opt*_tmpB23=({struct Cyc_Core_Opt*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->v=c2;_tmp4E3;});*_tmp4F1=_tmpB23;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3078
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3083
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4F7=({struct Cyc_Int_pa_PrintArg_struct _tmp9A4;_tmp9A4.tag=1U,_tmp9A4.f1=(unsigned long)i;_tmp9A4;});void*_tmp4F5[1U];_tmp4F5[0]=& _tmp4F7;({struct _dyneither_ptr _tmpB24=({const char*_tmp4F6="#%d";_tag_dyneither(_tmp4F6,sizeof(char),4U);});Cyc_aprintf(_tmpB24,_tag_dyneither(_tmp4F5,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));({struct _dyneither_ptr*_tmpB25=({unsigned int _tmp4F3=1;struct _dyneither_ptr*_tmp4F2=_cycalloc(_check_times(_tmp4F3,sizeof(struct _dyneither_ptr)));_tmp4F2[0]=s;_tmp4F2;});_tmp4F4->name=_tmpB25;}),_tmp4F4->identity=- 1,_tmp4F4->kind=k;_tmp4F4;});}
# 3090
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp4F8=*t->name;
return(int)*((const char*)_check_dyneither_subscript(_tmp4F8,sizeof(char),0))== (int)'#';}
# 3095
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp4FB=({struct Cyc_String_pa_PrintArg_struct _tmp9A5;_tmp9A5.tag=0U,_tmp9A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);_tmp9A5;});void*_tmp4F9[1U];_tmp4F9[0]=& _tmp4FB;({struct _dyneither_ptr _tmpB26=({const char*_tmp4FA="%s";_tag_dyneither(_tmp4FA,sizeof(char),3U);});Cyc_printf(_tmpB26,_tag_dyneither(_tmp4F9,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp4FC=({struct _dyneither_ptr _tmpB27=({const char*_tmp502="`";_tag_dyneither(_tmp502,sizeof(char),2U);});Cyc_strconcat(_tmpB27,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp4FD=_dyneither_ptr_plus(_tmp4FC,sizeof(char),1);char _tmp4FE=*((char*)_check_dyneither_subscript(_tmp4FD,sizeof(char),0U));char _tmp4FF='t';if(_get_dyneither_size(_tmp4FD,sizeof(char))== 1U  && (_tmp4FE == 0  && _tmp4FF != 0))_throw_arraybounds();*((char*)_tmp4FD.curr)=_tmp4FF;});
({struct _dyneither_ptr*_tmpB28=({unsigned int _tmp501=1;struct _dyneither_ptr*_tmp500=_cycalloc(_check_times(_tmp501,sizeof(struct _dyneither_ptr)));_tmp500[0]=(struct _dyneither_ptr)_tmp4FC;_tmp500;});t->name=_tmpB28;});};}
# 3104
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 3111
struct Cyc_List_List*_tmp503=0;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp503=({struct Cyc_List_List*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->hd=(void*)atts->hd,_tmp504->tl=_tmp503;_tmp504;});}}{
struct Cyc_Absyn_FnInfo _tmp505=fd->i;
_tmp505.attributes=_tmp503;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->tag=5U,_tmp506->f1=_tmp505;_tmp506;});};}
# 3119
return(void*)_check_null(fd->cached_type);}
# 3125
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3127
union Cyc_Relations_RelnOp _tmp507=*rop;union Cyc_Relations_RelnOp _tmp508=_tmp507;struct Cyc_Absyn_Vardecl*_tmp51B;struct Cyc_Absyn_Vardecl*_tmp51A;switch((_tmp508.RNumelts).tag){case 2U: _LL1: _tmp51A=(_tmp508.RVar).val;_LL2: {
# 3129
struct _tuple2 _tmp509=*_tmp51A->name;struct _tuple2 _tmp50A=_tmp509;union Cyc_Absyn_Nmspace _tmp511;struct _dyneither_ptr*_tmp510;_LL8: _tmp511=_tmp50A.f1;_tmp510=_tmp50A.f2;_LL9:;
if(!((int)((_tmp511.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpB29=(struct _dyneither_ptr)*_tmp510;Cyc_strcmp(_tmpB29,({const char*_tmp50B="return_value";_tag_dyneither(_tmp50B,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpB2A=Cyc_Relations_RReturn();*rop=_tmpB2A;});
goto _LL0;}{
# 3135
unsigned int c=0U;
{struct Cyc_List_List*_tmp50C=args;for(0;_tmp50C != 0;(_tmp50C=_tmp50C->tl,c ++)){
struct _tuple10*_tmp50D=(struct _tuple10*)_tmp50C->hd;struct _tuple10*_tmp50E=_tmp50D;struct _dyneither_ptr*_tmp50F;_LLB: _tmp50F=_tmp50E->f1;_LLC:;
if(_tmp50F != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp510,(struct _dyneither_ptr)*_tmp50F)== 0){
({union Cyc_Relations_RelnOp _tmpB2B=Cyc_Relations_RParam(c);*rop=_tmpB2B;});
break;}}}}
# 3145
goto _LL0;};}case 3U: _LL3: _tmp51B=(_tmp508.RNumelts).val;_LL4: {
# 3147
struct _tuple2 _tmp512=*_tmp51B->name;struct _tuple2 _tmp513=_tmp512;union Cyc_Absyn_Nmspace _tmp519;struct _dyneither_ptr*_tmp518;_LLE: _tmp519=_tmp513.f1;_tmp518=_tmp513.f2;_LLF:;
if(!((int)((_tmp519.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp514=args;for(0;_tmp514 != 0;(_tmp514=_tmp514->tl,c ++)){
struct _tuple10*_tmp515=(struct _tuple10*)_tmp514->hd;struct _tuple10*_tmp516=_tmp515;struct _dyneither_ptr*_tmp517;_LL11: _tmp517=_tmp516->f1;_LL12:;
if(_tmp517 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp518,(struct _dyneither_ptr)*_tmp517)== 0){
({union Cyc_Relations_RelnOp _tmpB2C=Cyc_Relations_RParamNumelts(c);*rop=_tmpB2C;});
break;}}}}
# 3159
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3164
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3166
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3170
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3172
if(e == 0)return 0;{
struct Cyc_List_List*_tmp51C=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp51C);
return _tmp51C;};}
# 3179
static struct _dyneither_ptr*Cyc_Tcutil_fst_fdarg(struct _tuple10*t){return(struct _dyneither_ptr*)_check_null((*t).f1);}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp51D=pr;struct Cyc_Absyn_Tqual _tmp520;void*_tmp51F;_LL1: _tmp520=_tmp51D->f1;_tmp51F=_tmp51D->f2;_LL2:;
if(_tmp51F == t)return pr;else{
return({struct _tuple12*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->f1=_tmp520,_tmp51E->f2=t;_tmp51E;});}}struct _tuple27{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple28{struct _tuple27*f1;void*f2;};
# 3186
static struct _tuple28*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3188
return({struct _tuple28*_tmp522=_region_malloc(rgn,sizeof(*_tmp522));({struct _tuple27*_tmpB2D=({struct _tuple27*_tmp521=_region_malloc(rgn,sizeof(*_tmp521));_tmp521->f1=(*y).f1,_tmp521->f2=(*y).f2;_tmp521;});_tmp522->f1=_tmpB2D;}),_tmp522->f2=(*y).f3;_tmp522;});}
# 3190
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3192
struct _tuple10 _tmp523=*orig_arg;struct _tuple10 _tmp524=_tmp523;struct _dyneither_ptr*_tmp528;struct Cyc_Absyn_Tqual _tmp527;void*_tmp526;_LL1: _tmp528=_tmp524.f1;_tmp527=_tmp524.f2;_tmp526=_tmp524.f3;_LL2:;
if(t == _tmp526)return orig_arg;
return({struct _tuple10*_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525->f1=_tmp528,_tmp525->f2=_tmp527,_tmp525->f3=t;_tmp525;});}
# 3196
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3199
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3204
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3206
return({struct Cyc_Absyn_Exp*_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529->topt=old->topt,_tmp529->r=r,_tmp529->loc=old->loc,_tmp529->annot=old->annot;_tmp529;});}
# 3211
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp52A=e->r;void*_tmp52B=_tmp52A;void*_tmp56B;void*_tmp56A;struct Cyc_List_List*_tmp569;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Exp*_tmp567;void*_tmp566;void*_tmp565;struct Cyc_Absyn_Exp*_tmp564;int _tmp563;enum Cyc_Absyn_Coercion _tmp562;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Exp*_tmp55B;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*_tmp559;enum Cyc_Absyn_Primop _tmp558;struct Cyc_List_List*_tmp557;switch(*((int*)_tmp52B)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp558=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_tmp557=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp52B)->f2;_LLC:
# 3220
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp557)== 1){
struct Cyc_Absyn_Exp*_tmp52C=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp557))->hd;
struct Cyc_Absyn_Exp*_tmp52D=Cyc_Tcutil_rsubsexp(r,inst,_tmp52C);
if(_tmp52D == _tmp52C)return e;
return({struct Cyc_Absyn_Exp*_tmpB2F=e;Cyc_Tcutil_copye(_tmpB2F,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->tag=3U,_tmp52F->f1=_tmp558,({struct Cyc_List_List*_tmpB2E=({struct Cyc_Absyn_Exp*_tmp52E[1U];_tmp52E[0]=_tmp52D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp52E,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp52F->f2=_tmpB2E;});_tmp52F;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp557)== 2){
struct Cyc_Absyn_Exp*_tmp530=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp557))->hd;
struct Cyc_Absyn_Exp*_tmp531=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp557->tl))->hd;
struct Cyc_Absyn_Exp*_tmp532=Cyc_Tcutil_rsubsexp(r,inst,_tmp530);
struct Cyc_Absyn_Exp*_tmp533=Cyc_Tcutil_rsubsexp(r,inst,_tmp531);
if(_tmp532 == _tmp530  && _tmp533 == _tmp531)return e;
return({struct Cyc_Absyn_Exp*_tmpB31=e;Cyc_Tcutil_copye(_tmpB31,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->tag=3U,_tmp535->f1=_tmp558,({struct Cyc_List_List*_tmpB30=({struct Cyc_Absyn_Exp*_tmp534[2U];_tmp534[0]=_tmp532,_tmp534[1]=_tmp533;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp534,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp535->f2=_tmpB30;});_tmp535;}));});}else{
return({void*_tmp536=0U;({struct _dyneither_ptr _tmpB32=({const char*_tmp537="primop does not have 1 or 2 args!";_tag_dyneither(_tmp537,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB32,_tag_dyneither(_tmp536,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp55B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_tmp55A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp52B)->f2;_tmp559=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp52B)->f3;_LLE: {
# 3234
struct Cyc_Absyn_Exp*_tmp538=Cyc_Tcutil_rsubsexp(r,inst,_tmp55B);
struct Cyc_Absyn_Exp*_tmp539=Cyc_Tcutil_rsubsexp(r,inst,_tmp55A);
struct Cyc_Absyn_Exp*_tmp53A=Cyc_Tcutil_rsubsexp(r,inst,_tmp559);
if((_tmp538 == _tmp55B  && _tmp539 == _tmp55A) && _tmp53A == _tmp559)return e;
return({struct Cyc_Absyn_Exp*_tmpB33=e;Cyc_Tcutil_copye(_tmpB33,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B->tag=6U,_tmp53B->f1=_tmp538,_tmp53B->f2=_tmp539,_tmp53B->f3=_tmp53A;_tmp53B;}));});}case 7U: _LLF: _tmp55D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_tmp55C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp52B)->f2;_LL10: {
# 3240
struct Cyc_Absyn_Exp*_tmp53C=Cyc_Tcutil_rsubsexp(r,inst,_tmp55D);
struct Cyc_Absyn_Exp*_tmp53D=Cyc_Tcutil_rsubsexp(r,inst,_tmp55C);
if(_tmp53C == _tmp55D  && _tmp53D == _tmp55C)return e;
return({struct Cyc_Absyn_Exp*_tmpB34=e;Cyc_Tcutil_copye(_tmpB34,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E->tag=7U,_tmp53E->f1=_tmp53C,_tmp53E->f2=_tmp53D;_tmp53E;}));});}case 8U: _LL11: _tmp55F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_tmp55E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp52B)->f2;_LL12: {
# 3245
struct Cyc_Absyn_Exp*_tmp53F=Cyc_Tcutil_rsubsexp(r,inst,_tmp55F);
struct Cyc_Absyn_Exp*_tmp540=Cyc_Tcutil_rsubsexp(r,inst,_tmp55E);
if(_tmp53F == _tmp55F  && _tmp540 == _tmp55E)return e;
return({struct Cyc_Absyn_Exp*_tmpB35=e;Cyc_Tcutil_copye(_tmpB35,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->tag=8U,_tmp541->f1=_tmp53F,_tmp541->f2=_tmp540;_tmp541;}));});}case 9U: _LL13: _tmp561=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_tmp560=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp52B)->f2;_LL14: {
# 3250
struct Cyc_Absyn_Exp*_tmp542=Cyc_Tcutil_rsubsexp(r,inst,_tmp561);
struct Cyc_Absyn_Exp*_tmp543=Cyc_Tcutil_rsubsexp(r,inst,_tmp560);
if(_tmp542 == _tmp561  && _tmp543 == _tmp560)return e;
return({struct Cyc_Absyn_Exp*_tmpB36=e;Cyc_Tcutil_copye(_tmpB36,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544->tag=9U,_tmp544->f1=_tmp542,_tmp544->f2=_tmp543;_tmp544;}));});}case 14U: _LL15: _tmp565=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_tmp564=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp52B)->f2;_tmp563=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp52B)->f3;_tmp562=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp52B)->f4;_LL16: {
# 3255
struct Cyc_Absyn_Exp*_tmp545=Cyc_Tcutil_rsubsexp(r,inst,_tmp564);
void*_tmp546=Cyc_Tcutil_rsubstitute(r,inst,_tmp565);
if(_tmp545 == _tmp564  && _tmp546 == _tmp565)return e;
return({struct Cyc_Absyn_Exp*_tmpB37=e;Cyc_Tcutil_copye(_tmpB37,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->tag=14U,_tmp547->f1=_tmp546,_tmp547->f2=_tmp545,_tmp547->f3=_tmp563,_tmp547->f4=_tmp562;_tmp547;}));});}case 17U: _LL17: _tmp566=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LL18: {
# 3260
void*_tmp548=Cyc_Tcutil_rsubstitute(r,inst,_tmp566);
if(_tmp548 == _tmp566)return e;
return({struct Cyc_Absyn_Exp*_tmpB38=e;Cyc_Tcutil_copye(_tmpB38,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->tag=17U,_tmp549->f1=_tmp548;_tmp549;}));});}case 18U: _LL19: _tmp567=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LL1A: {
# 3264
struct Cyc_Absyn_Exp*_tmp54A=Cyc_Tcutil_rsubsexp(r,inst,_tmp567);
if(_tmp54A == _tmp567)return e;
return({struct Cyc_Absyn_Exp*_tmpB39=e;Cyc_Tcutil_copye(_tmpB39,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->tag=18U,_tmp54B->f1=_tmp54A;_tmp54B;}));});}case 41U: _LL1B: _tmp568=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LL1C: {
# 3268
struct Cyc_Absyn_Exp*_tmp54C=Cyc_Tcutil_rsubsexp(r,inst,_tmp568);
if(_tmp54C == _tmp568)return e;
return({struct Cyc_Absyn_Exp*_tmpB3A=e;Cyc_Tcutil_copye(_tmpB3A,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->tag=41U,_tmp54D->f1=_tmp54C;_tmp54D;}));});}case 19U: _LL1D: _tmp56A=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_tmp569=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp52B)->f2;_LL1E: {
# 3272
void*_tmp54E=Cyc_Tcutil_rsubstitute(r,inst,_tmp56A);
if(_tmp54E == _tmp56A)return e;
return({struct Cyc_Absyn_Exp*_tmpB3B=e;Cyc_Tcutil_copye(_tmpB3B,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->tag=19U,_tmp54F->f1=_tmp54E,_tmp54F->f2=_tmp569;_tmp54F;}));});}case 39U: _LL1F: _tmp56B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LL20: {
# 3276
void*_tmp550=Cyc_Tcutil_rsubstitute(r,inst,_tmp56B);
if(_tmp550 == _tmp56B)return e;{
# 3279
void*_tmp551=Cyc_Tcutil_compress(_tmp550);void*_tmp552=_tmp551;struct Cyc_Absyn_Exp*_tmp554;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp552)->tag == 9U){_LL24: _tmp554=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp552)->f1;_LL25:
 return _tmp554;}else{_LL26: _LL27:
# 3282
 return({struct Cyc_Absyn_Exp*_tmpB3C=e;Cyc_Tcutil_copye(_tmpB3C,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->tag=39U,_tmp553->f1=_tmp550;_tmp553;}));});}_LL23:;};}default: _LL21: _LL22:
# 3285
 return({void*_tmp555=0U;({struct _dyneither_ptr _tmpB3D=({const char*_tmp556="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp556,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB3D,_tag_dyneither(_tmp555,sizeof(void*),0U));});});}_LL0:;}
# 3289
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3292
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3296
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3300
void*_tmp56C=f->type;
struct Cyc_Absyn_Exp*_tmp56D=f->requires_clause;
void*_tmp56E=Cyc_Tcutil_rsubstitute(r,inst,_tmp56C);
struct Cyc_Absyn_Exp*_tmp56F=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp56D);
if(_tmp56C == _tmp56E  && _tmp56D == _tmp56F)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->name=f->name,_tmp570->tq=f->tq,_tmp570->type=_tmp56E,_tmp570->width=f->width,_tmp570->attributes=f->attributes,_tmp570->requires_clause=_tmp56F;_tmp570;});}}
# 3310
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3313
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp571=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp572=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp573=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp571);
struct Cyc_List_List*_tmp574=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp572);
if(_tmp573 == _tmp571  && _tmp574 == _tmp572)return fs;
return({struct Cyc_List_List*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->hd=_tmp573,_tmp575->tl=_tmp574;_tmp575;});};}
# 3322
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3325
struct _tuple1 _tmp576=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp577=_tmp576;struct Cyc_List_List*_tmp57B;struct Cyc_List_List*_tmp57A;_LL1: _tmp57B=_tmp577.f1;_tmp57A=_tmp577.f2;_LL2:;{
struct Cyc_List_List*_tmp578=Cyc_Tcutil_substs(rgn,inst,_tmp57B);
struct Cyc_List_List*_tmp579=Cyc_Tcutil_substs(rgn,inst,_tmp57A);
if(_tmp578 == _tmp57B  && _tmp579 == _tmp57A)
return rgn_po;else{
# 3331
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp578,_tmp579);}};}
# 3334
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3337
void*_tmp57C=Cyc_Tcutil_compress(t);void*_tmp57D=_tmp57C;struct Cyc_Absyn_Exp*_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D4;void*_tmp5D3;struct Cyc_List_List*_tmp5D2;void*_tmp5D1;void*_tmp5D0;enum Cyc_Absyn_AggrKind _tmp5CF;struct Cyc_List_List*_tmp5CE;struct Cyc_List_List*_tmp5CD;struct Cyc_List_List*_tmp5CC;void*_tmp5CB;struct Cyc_Absyn_Tqual _tmp5CA;void*_tmp5C9;struct Cyc_List_List*_tmp5C8;int _tmp5C7;struct Cyc_Absyn_VarargInfo*_tmp5C6;struct Cyc_List_List*_tmp5C5;struct Cyc_List_List*_tmp5C4;struct Cyc_Absyn_Exp*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;void*_tmp5C1;struct Cyc_Absyn_Tqual _tmp5C0;void*_tmp5BF;void*_tmp5BE;void*_tmp5BD;void*_tmp5BC;void*_tmp5BB;struct Cyc_Absyn_Tqual _tmp5BA;struct Cyc_Absyn_Exp*_tmp5B9;void*_tmp5B8;unsigned int _tmp5B7;struct _tuple2*_tmp5B6;struct Cyc_List_List*_tmp5B5;struct Cyc_Absyn_Typedefdecl*_tmp5B4;void*_tmp5B3;struct Cyc_Absyn_Tvar*_tmp5B2;switch(*((int*)_tmp57D)){case 2U: _LL1: _tmp5B2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp57D)->f1;_LL2: {
# 3340
struct _handler_cons _tmp57E;_push_handler(& _tmp57E);{int _tmp580=0;if(setjmp(_tmp57E.handler))_tmp580=1;if(!_tmp580){{void*_tmp581=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp5B2);_npop_handler(0U);return _tmp581;};_pop_handler();}else{void*_tmp57F=(void*)Cyc_Core_get_exn_thrown();void*_tmp582=_tmp57F;void*_tmp583;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp582)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp583=_tmp582;_LL1F:(int)_rethrow(_tmp583);}_LL1B:;}};}case 8U: _LL3: _tmp5B6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp57D)->f1;_tmp5B5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp57D)->f2;_tmp5B4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp57D)->f3;_tmp5B3=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp57D)->f4;_LL4: {
# 3343
struct Cyc_List_List*_tmp584=Cyc_Tcutil_substs(rgn,inst,_tmp5B5);
return _tmp584 == _tmp5B5?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->tag=8U,_tmp585->f1=_tmp5B6,_tmp585->f2=_tmp584,_tmp585->f3=_tmp5B4,_tmp585->f4=_tmp5B3;_tmp585;});}case 4U: _LL5: _tmp5BB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57D)->f1).elt_type;_tmp5BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57D)->f1).tq;_tmp5B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57D)->f1).num_elts;_tmp5B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57D)->f1).zero_term;_tmp5B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57D)->f1).zt_loc;_LL6: {
# 3346
void*_tmp586=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5BB);
struct Cyc_Absyn_Exp*_tmp587=_tmp5B9 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5B9);
void*_tmp588=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5B8);
return(_tmp586 == _tmp5BB  && _tmp587 == _tmp5B9) && _tmp588 == _tmp5B8?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp589=_cycalloc(sizeof(*_tmp589));
_tmp589->tag=4U,(_tmp589->f1).elt_type=_tmp586,(_tmp589->f1).tq=_tmp5BA,(_tmp589->f1).num_elts=_tmp587,(_tmp589->f1).zero_term=_tmp588,(_tmp589->f1).zt_loc=_tmp5B7;_tmp589;});}case 3U: _LL7: _tmp5C1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).elt_type;_tmp5C0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).elt_tq;_tmp5BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).ptr_atts).rgn;_tmp5BE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).ptr_atts).nullable;_tmp5BD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).ptr_atts).bounds;_tmp5BC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).ptr_atts).zero_term;_LL8: {
# 3352
void*_tmp58A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5C1);
void*_tmp58B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5BF);
void*_tmp58C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5BD);
void*_tmp58D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5BC);
if(((_tmp58A == _tmp5C1  && _tmp58B == _tmp5BF) && _tmp58C == _tmp5BD) && _tmp58D == _tmp5BC)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp9A6;_tmp9A6.elt_type=_tmp58A,_tmp9A6.elt_tq=_tmp5C0,(_tmp9A6.ptr_atts).rgn=_tmp58B,(_tmp9A6.ptr_atts).nullable=_tmp5BE,(_tmp9A6.ptr_atts).bounds=_tmp58C,(_tmp9A6.ptr_atts).zero_term=_tmp58D,(_tmp9A6.ptr_atts).ptrloc=0;_tmp9A6;}));}case 5U: _LL9: _tmp5CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).tvars;_tmp5CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).effect;_tmp5CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).ret_tqual;_tmp5C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).ret_type;_tmp5C8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).args;_tmp5C7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).c_varargs;_tmp5C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).cyc_varargs;_tmp5C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).rgn_po;_tmp5C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).attributes;_tmp5C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).requires_clause;_tmp5C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp57D)->f1).ensures_clause;_LLA:
# 3362
{struct Cyc_List_List*_tmp58E=_tmp5CC;for(0;_tmp58E != 0;_tmp58E=_tmp58E->tl){
inst=({struct Cyc_List_List*_tmp590=_region_malloc(rgn,sizeof(*_tmp590));({struct _tuple15*_tmpB3F=({struct _tuple15*_tmp58F=_region_malloc(rgn,sizeof(*_tmp58F));_tmp58F->f1=(struct Cyc_Absyn_Tvar*)_tmp58E->hd,({void*_tmpB3E=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp58E->hd);_tmp58F->f2=_tmpB3E;});_tmp58F;});_tmp590->hd=_tmpB3F;}),_tmp590->tl=inst;_tmp590;});}}{
struct _tuple1 _tmp591=({struct _RegionHandle*_tmpB41=rgn;struct _RegionHandle*_tmpB40=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpB41,_tmpB40,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple28*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp5C8));});
# 3364
struct _tuple1 _tmp592=_tmp591;struct Cyc_List_List*_tmp5A1;struct Cyc_List_List*_tmp5A0;_LL21: _tmp5A1=_tmp592.f1;_tmp5A0=_tmp592.f2;_LL22:;{
# 3366
struct Cyc_List_List*_tmp593=_tmp5C8;
struct Cyc_List_List*_tmp594=Cyc_Tcutil_substs(rgn,inst,_tmp5A0);
if(_tmp594 != _tmp5A0)
_tmp593=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp5C8,_tmp594);{
void*eff2;
if(_tmp5CB == 0)
eff2=0;else{
# 3374
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5CB);
if(new_eff == _tmp5CB)
eff2=_tmp5CB;else{
# 3378
eff2=new_eff;}}{
# 3380
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp5C6 == 0)
cyc_varargs2=0;else{
# 3384
struct Cyc_Absyn_VarargInfo _tmp595=*_tmp5C6;struct Cyc_Absyn_VarargInfo _tmp596=_tmp595;struct _dyneither_ptr*_tmp59C;struct Cyc_Absyn_Tqual _tmp59B;void*_tmp59A;int _tmp599;_LL24: _tmp59C=_tmp596.name;_tmp59B=_tmp596.tq;_tmp59A=_tmp596.type;_tmp599=_tmp596.inject;_LL25:;{
void*_tmp597=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp59A);
if(_tmp597 == _tmp59A)cyc_varargs2=_tmp5C6;else{
# 3388
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->name=_tmp59C,_tmp598->tq=_tmp59B,_tmp598->type=_tmp597,_tmp598->inject=_tmp599;_tmp598;});}};}{
# 3390
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5C5);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5C3);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5C2);
struct Cyc_List_List*_tmp59D=Cyc_Tcutil_extract_relns(_tmp593,req2);
struct Cyc_List_List*_tmp59E=Cyc_Tcutil_extract_relns(_tmp593,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp59F=_cycalloc(sizeof(*_tmp59F));
_tmp59F->tag=5U,(_tmp59F->f1).tvars=_tmp5CC,(_tmp59F->f1).effect=eff2,(_tmp59F->f1).ret_tqual=_tmp5CA,({void*_tmpB42=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5C9);(_tmp59F->f1).ret_type=_tmpB42;}),(_tmp59F->f1).args=_tmp593,(_tmp59F->f1).c_varargs=_tmp5C7,(_tmp59F->f1).cyc_varargs=cyc_varargs2,(_tmp59F->f1).rgn_po=rgn_po2,(_tmp59F->f1).attributes=_tmp5C4,(_tmp59F->f1).requires_clause=req2,(_tmp59F->f1).requires_relns=_tmp59D,(_tmp59F->f1).ensures_clause=ens2,(_tmp59F->f1).ensures_relns=_tmp59E;_tmp59F;});};};};};};case 6U: _LLB: _tmp5CD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57D)->f1;_LLC: {
# 3400
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp5A2=_tmp5CD;for(0;_tmp5A2 != 0;_tmp5A2=_tmp5A2->tl){
void*_tmp5A3=(*((struct _tuple12*)_tmp5A2->hd)).f2;
void*_tmp5A4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5A3);
if(_tmp5A3 != _tmp5A4)
change=1;
# 3408
ts2=({struct Cyc_List_List*_tmp5A5=_region_malloc(rgn,sizeof(*_tmp5A5));_tmp5A5->hd=_tmp5A4,_tmp5A5->tl=ts2;_tmp5A5;});}}
# 3410
if(!change)
return t;{
struct Cyc_List_List*_tmp5A6=({struct Cyc_List_List*_tmpB43=_tmp5CD;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpB43,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->tag=6U,_tmp5A7->f1=_tmp5A6;_tmp5A7;});};}case 7U: _LLD: _tmp5CF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57D)->f1;_tmp5CE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57D)->f2;_LLE: {
# 3415
struct Cyc_List_List*_tmp5A8=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp5CE);
if(_tmp5CE == _tmp5A8)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));_tmp5A9->tag=7U,_tmp5A9->f1=_tmp5CF,_tmp5A9->f2=_tmp5A8;_tmp5A9;});}case 1U: _LLF: _tmp5D0=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp57D)->f2;_LL10:
# 3419
 if(_tmp5D0 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D0);else{
return t;}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57D)->f2 == 0){_LL11: _tmp5D1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57D)->f1;_LL12:
 return t;}else{_LL13: _tmp5D3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57D)->f1;_tmp5D2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57D)->f2;_LL14: {
# 3423
struct Cyc_List_List*_tmp5AA=Cyc_Tcutil_substs(rgn,inst,_tmp5D2);
if(_tmp5D2 == _tmp5AA)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->tag=0U,_tmp5AB->f1=_tmp5D3,_tmp5AB->f2=_tmp5AA;_tmp5AB;});}}}case 9U: _LL15: _tmp5D4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp57D)->f1;_LL16: {
# 3427
struct Cyc_Absyn_Exp*_tmp5AC=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5D4);
return _tmp5AC == _tmp5D4?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->tag=9U,_tmp5AD->f1=_tmp5AC;_tmp5AD;});}case 11U: _LL17: _tmp5D5=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp57D)->f1;_LL18: {
# 3430
struct Cyc_Absyn_Exp*_tmp5AE=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5D5);
return _tmp5AE == _tmp5D5?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->tag=11U,_tmp5AF->f1=_tmp5AE;_tmp5AF;});}default: _LL19: _LL1A:
({void*_tmp5B0=0U;({struct _dyneither_ptr _tmpB44=({const char*_tmp5B1="found typedecltype in rsubs";_tag_dyneither(_tmp5B1,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB44,_tag_dyneither(_tmp5B0,sizeof(void*),0U));});});}_LL0:;}
# 3436
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3439
if(ts == 0)
return 0;{
void*_tmp5D6=(void*)ts->hd;
struct Cyc_List_List*_tmp5D7=ts->tl;
void*_tmp5D8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D6);
struct Cyc_List_List*_tmp5D9=Cyc_Tcutil_substs(rgn,inst,_tmp5D7);
if(_tmp5D6 == _tmp5D8  && _tmp5D7 == _tmp5D9)
return ts;
return({struct Cyc_List_List*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->hd=_tmp5D8,_tmp5DA->tl=_tmp5D9;_tmp5DA;});};}
# 3450
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3457
struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp5DB=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->f1=tv,({void*_tmpB46=({struct Cyc_Core_Opt*_tmpB45=_tmp5DB;Cyc_Absyn_new_evar(_tmpB45,({struct Cyc_Core_Opt*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->v=s;_tmp5DC;}));});_tmp5DD->f2=_tmpB46;});_tmp5DD;});}
# 3462
struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,struct Cyc_Absyn_Tvar*tv){
# 3464
struct _tuple16*_tmp5DE=env;struct Cyc_List_List*_tmp5E3;struct _RegionHandle*_tmp5E2;_LL1: _tmp5E3=_tmp5DE->f1;_tmp5E2=_tmp5DE->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp5DF=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5E1=_region_malloc(_tmp5E2,sizeof(*_tmp5E1));_tmp5E1->f1=tv,({void*_tmpB48=({struct Cyc_Core_Opt*_tmpB47=_tmp5DF;Cyc_Absyn_new_evar(_tmpB47,({struct Cyc_Core_Opt*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));_tmp5E0->v=_tmp5E3;_tmp5E0;}));});_tmp5E1->f2=_tmpB48;});_tmp5E1;});};}
# 3474
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3478
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp5E6=({struct Cyc_String_pa_PrintArg_struct _tmp9A9;_tmp9A9.tag=0U,_tmp9A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmp9A9;});struct Cyc_String_pa_PrintArg_struct _tmp5E7=({struct Cyc_String_pa_PrintArg_struct _tmp9A8;_tmp9A8.tag=0U,({
struct _dyneither_ptr _tmpB49=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmp9A8.f1=_tmpB49;});_tmp9A8;});struct Cyc_String_pa_PrintArg_struct _tmp5E8=({struct Cyc_String_pa_PrintArg_struct _tmp9A7;_tmp9A7.tag=0U,({struct _dyneither_ptr _tmpB4A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));_tmp9A7.f1=_tmpB4A;});_tmp9A7;});void*_tmp5E4[3U];_tmp5E4[0]=& _tmp5E6,_tmp5E4[1]=& _tmp5E7,_tmp5E4[2]=& _tmp5E8;({unsigned int _tmpB4C=loc;struct _dyneither_ptr _tmpB4B=({const char*_tmp5E5="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp5E5,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpB4C,_tmpB4B,_tag_dyneither(_tmp5E4,sizeof(void*),3U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp5E9=0U;({struct _dyneither_ptr _tmpB4D=({const char*_tmp5EA="same type variable has different identity!";_tag_dyneither(_tmp5EA,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB4D,_tag_dyneither(_tmp5E9,sizeof(void*),0U));});});}
return tvs;}}}
# 3492
({int _tmpB4E=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpB4E;});
return({struct Cyc_List_List*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB->hd=tv,_tmp5EB->tl=tvs;_tmp5EB;});}
# 3498
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3500
if(tv->identity == - 1)
({void*_tmp5EC=0U;({struct _dyneither_ptr _tmpB4F=({const char*_tmp5ED="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp5ED,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB4F,_tag_dyneither(_tmp5EC,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3504
struct Cyc_Absyn_Tvar*_tmp5EE=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp5EE->identity == - 1)
({void*_tmp5EF=0U;({struct _dyneither_ptr _tmpB50=({const char*_tmp5F0="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp5F0,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB50,_tag_dyneither(_tmp5EF,sizeof(void*),0U));});});
if(_tmp5EE->identity == tv->identity)
return tvs;}}
# 3511
return({struct Cyc_List_List*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->hd=tv,_tmp5F1->tl=tvs;_tmp5F1;});}struct _tuple29{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3517
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3522
if(tv->identity == - 1)
({void*_tmp5F2=0U;({struct _dyneither_ptr _tmpB51=({const char*_tmp5F3="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp5F3,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB51,_tag_dyneither(_tmp5F2,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3526
struct _tuple29*_tmp5F4=(struct _tuple29*)tvs2->hd;struct _tuple29*_tmp5F5=_tmp5F4;struct Cyc_Absyn_Tvar*_tmp5F9;int*_tmp5F8;_LL1: _tmp5F9=_tmp5F5->f1;_tmp5F8=(int*)& _tmp5F5->f2;_LL2:;
if(_tmp5F9->identity == - 1)
({void*_tmp5F6=0U;({struct _dyneither_ptr _tmpB52=({const char*_tmp5F7="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp5F7,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB52,_tag_dyneither(_tmp5F6,sizeof(void*),0U));});});
if(_tmp5F9->identity == tv->identity){
*_tmp5F8=*_tmp5F8  || b;
return tvs;}}}
# 3534
return({struct Cyc_List_List*_tmp5FB=_region_malloc(r,sizeof(*_tmp5FB));({struct _tuple29*_tmpB53=({struct _tuple29*_tmp5FA=_region_malloc(r,sizeof(*_tmp5FA));_tmp5FA->f1=tv,_tmp5FA->f2=b;_tmp5FA;});_tmp5FB->hd=_tmpB53;}),_tmp5FB->tl=tvs;_tmp5FB;});}
# 3538
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3540
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp5FE=({struct Cyc_String_pa_PrintArg_struct _tmp9AA;_tmp9AA.tag=0U,({struct _dyneither_ptr _tmpB54=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmp9AA.f1=_tmpB54;});_tmp9AA;});void*_tmp5FC[1U];_tmp5FC[0]=& _tmp5FE;({struct _dyneither_ptr _tmpB55=({const char*_tmp5FD="bound tvar id for %s is NULL";_tag_dyneither(_tmp5FD,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB55,_tag_dyneither(_tmp5FC,sizeof(void*),1U));});});
return({struct Cyc_List_List*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->hd=tv,_tmp5FF->tl=tvs;_tmp5FF;});}struct _tuple30{void*f1;int f2;};
# 3549
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3552
void*_tmp600=Cyc_Tcutil_compress(e);void*_tmp601=_tmp600;int _tmp60B;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp601)->tag == 1U){_LL1: _tmp60B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp601)->f3;_LL2:
# 3554
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple30*_tmp602=(struct _tuple30*)es2->hd;struct _tuple30*_tmp603=_tmp602;void*_tmp608;int*_tmp607;_LL6: _tmp608=_tmp603->f1;_tmp607=(int*)& _tmp603->f2;_LL7:;{
void*_tmp604=Cyc_Tcutil_compress(_tmp608);void*_tmp605=_tmp604;int _tmp606;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp605)->tag == 1U){_LL9: _tmp606=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp605)->f3;_LLA:
# 3558
 if(_tmp60B == _tmp606){
if(b != *_tmp607)*_tmp607=1;
return es;}
# 3562
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3566
return({struct Cyc_List_List*_tmp60A=_region_malloc(r,sizeof(*_tmp60A));({struct _tuple30*_tmpB56=({struct _tuple30*_tmp609=_region_malloc(r,sizeof(*_tmp609));_tmp609->f1=e,_tmp609->f2=b;_tmp609;});_tmp60A->hd=_tmpB56;}),_tmp60A->tl=es;_tmp60A;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3571
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3574
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3583
if(!present)r=({struct Cyc_List_List*_tmp60C=_region_malloc(rgn,sizeof(*_tmp60C));_tmp60C->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp60C->tl=r;_tmp60C;});}
# 3585
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3590
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3594
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple29 _tmp60D=*((struct _tuple29*)tvs->hd);struct _tuple29 _tmp60E=_tmp60D;struct Cyc_Absyn_Tvar*_tmp611;int _tmp610;_LL1: _tmp611=_tmp60E.f1;_tmp610=_tmp60E.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp611->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3604
if(!present)res=({struct Cyc_List_List*_tmp60F=_region_malloc(r,sizeof(*_tmp60F));_tmp60F->hd=(struct _tuple29*)tvs->hd,_tmp60F->tl=res;_tmp60F;});};}
# 3606
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3610
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_type,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3612
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp614=({struct Cyc_String_pa_PrintArg_struct _tmp9AB;_tmp9AB.tag=0U,_tmp9AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9AB;});void*_tmp612[1U];_tmp612[0]=& _tmp614;({unsigned int _tmpB58=loc;struct _dyneither_ptr _tmpB57=({const char*_tmp613="bitfield %s does not have constant width";_tag_dyneither(_tmp613,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpB58,_tmpB57,_tag_dyneither(_tmp612,sizeof(void*),1U));});});else{
# 3617
struct _tuple13 _tmp615=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _tmp616=_tmp615;unsigned int _tmp61C;int _tmp61B;_LL1: _tmp61C=_tmp616.f1;_tmp61B=_tmp616.f2;_LL2:;
if(!_tmp61B)
({void*_tmp617=0U;({unsigned int _tmpB5A=loc;struct _dyneither_ptr _tmpB59=({const char*_tmp618="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp618,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpB5A,_tmpB59,_tag_dyneither(_tmp617,sizeof(void*),0U));});});
if((int)_tmp61C < 0)
({void*_tmp619=0U;({unsigned int _tmpB5C=loc;struct _dyneither_ptr _tmpB5B=({const char*_tmp61A="bitfield has negative width";_tag_dyneither(_tmp61A,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpB5C,_tmpB5B,_tag_dyneither(_tmp619,sizeof(void*),0U));});});
w=_tmp61C;}{
# 3624
void*_tmp61D=Cyc_Tcutil_compress(field_type);void*_tmp61E=_tmp61D;enum Cyc_Absyn_Size_of _tmp62C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp61E)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp61E)->f1)->tag == 1U){_LL4: _tmp62C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp61E)->f1)->f2;_LL5:
# 3627
{enum Cyc_Absyn_Size_of _tmp61F=_tmp62C;switch(_tmp61F){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned int)8)({void*_tmp620=0U;({unsigned int _tmpB5E=loc;struct _dyneither_ptr _tmpB5D=({const char*_tmp621="bitfield larger than type";_tag_dyneither(_tmp621,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB5E,_tmpB5D,_tag_dyneither(_tmp620,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned int)16)({void*_tmp622=0U;({unsigned int _tmpB60=loc;struct _dyneither_ptr _tmpB5F=({const char*_tmp623="bitfield larger than type";_tag_dyneither(_tmp623,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB60,_tmpB5F,_tag_dyneither(_tmp622,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned int)32)({void*_tmp624=0U;({unsigned int _tmpB62=loc;struct _dyneither_ptr _tmpB61=({const char*_tmp625="bitfield larger than type";_tag_dyneither(_tmp625,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB62,_tmpB61,_tag_dyneither(_tmp624,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned int)64)({void*_tmp626=0U;({unsigned int _tmpB64=loc;struct _dyneither_ptr _tmpB63=({const char*_tmp627="bitfield larger than type";_tag_dyneither(_tmp627,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB64,_tmpB63,_tag_dyneither(_tmp626,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3635
goto _LL3;}else{goto _LL6;}}else{_LL6: _LL7:
# 3637
({struct Cyc_String_pa_PrintArg_struct _tmp62A=({struct Cyc_String_pa_PrintArg_struct _tmp9AD;_tmp9AD.tag=0U,_tmp9AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9AD;});struct Cyc_String_pa_PrintArg_struct _tmp62B=({struct Cyc_String_pa_PrintArg_struct _tmp9AC;_tmp9AC.tag=0U,({
struct _dyneither_ptr _tmpB65=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_type));_tmp9AC.f1=_tmpB65;});_tmp9AC;});void*_tmp628[2U];_tmp628[0]=& _tmp62A,_tmp628[1]=& _tmp62B;({unsigned int _tmpB67=loc;struct _dyneither_ptr _tmpB66=({const char*_tmp629="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp629,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpB67,_tmpB66,_tag_dyneither(_tmp628,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 3644
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp62D=(void*)atts->hd;void*_tmp62E=_tmp62D;switch(*((int*)_tmp62E)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp631=({struct Cyc_String_pa_PrintArg_struct _tmp9AF;_tmp9AF.tag=0U,({
struct _dyneither_ptr _tmpB68=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp9AF.f1=_tmpB68;});_tmp9AF;});struct Cyc_String_pa_PrintArg_struct _tmp632=({struct Cyc_String_pa_PrintArg_struct _tmp9AE;_tmp9AE.tag=0U,_tmp9AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9AE;});void*_tmp62F[2U];_tmp62F[0]=& _tmp631,_tmp62F[1]=& _tmp632;({unsigned int _tmpB6A=loc;struct _dyneither_ptr _tmpB69=({const char*_tmp630="bad attribute %s on member %s";_tag_dyneither(_tmp630,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpB6A,_tmpB69,_tag_dyneither(_tmp62F,sizeof(void*),2U));});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3672
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp633=t;struct Cyc_Absyn_Typedefdecl*_tmp637;void*_tmp636;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp633)->tag == 8U){_LL1: _tmp637=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp633)->f3;_tmp636=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp633)->f4;_LL2:
# 3675
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp637))->tq).real_const  || (_tmp637->tq).print_const){
if(declared_const)({void*_tmp634=0U;({unsigned int _tmpB6C=loc;struct _dyneither_ptr _tmpB6B=({const char*_tmp635="extra const";_tag_dyneither(_tmp635,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpB6C,_tmpB6B,_tag_dyneither(_tmp634,sizeof(void*),0U));});});
return 1;}
# 3680
if((unsigned int)_tmp636)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp636);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3687
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp638=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp639=_tmp638;void*_tmp63D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp639)->tag == 3U){_LL1: _tmp63D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp639)->f1).ptr_atts).rgn;_LL2:
# 3692
{void*_tmp63A=Cyc_Tcutil_compress(_tmp63D);void*_tmp63B=_tmp63A;struct Cyc_Absyn_Tvar*_tmp63C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp63B)->tag == 2U){_LL6: _tmp63C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp63B)->f1;_LL7:
 return Cyc_Absyn_tvar_cmp(tvar,_tmp63C)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3696
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3702
return 1;}
return 0;}
# 3706
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3709
void*_tmp63E=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp63F=_tmp63E;switch(*((int*)_tmp63F)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp63F)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp63F)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp63F)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp63F)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3718
 if((((int)expected_kind->kind == (int)Cyc_Absyn_BoxKind  || (int)expected_kind->kind == (int)Cyc_Absyn_MemKind) || (int)expected_kind->kind == (int)Cyc_Absyn_AnyKind) && 
# 3721
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3727
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3733
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*,struct Cyc_Tcenv_Tenv*,struct Cyc_Tcutil_CVTEnv);struct _tuple31{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3736
static struct _tuple31 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3739
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp642=({struct Cyc_String_pa_PrintArg_struct _tmp9B1;_tmp9B1.tag=0U,_tmp9B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9B1;});struct Cyc_String_pa_PrintArg_struct _tmp643=({struct Cyc_String_pa_PrintArg_struct _tmp9B0;_tmp9B0.tag=0U,({
struct _dyneither_ptr _tmpB6D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmp9B0.f1=_tmpB6D;});_tmp9B0;});void*_tmp640[2U];_tmp640[0]=& _tmp642,_tmp640[1]=& _tmp643;({unsigned int _tmpB6F=loc;struct _dyneither_ptr _tmpB6E=({const char*_tmp641="%s clause has type %s instead of integral type";_tag_dyneither(_tmp641,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpB6F,_tmpB6E,_tag_dyneither(_tmp640,sizeof(void*),2U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp646=({struct Cyc_String_pa_PrintArg_struct _tmp9B3;_tmp9B3.tag=0U,_tmp9B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9B3;});struct Cyc_String_pa_PrintArg_struct _tmp647=({struct Cyc_String_pa_PrintArg_struct _tmp9B2;_tmp9B2.tag=0U,({
struct _dyneither_ptr _tmpB70=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(clause));_tmp9B2.f1=_tmpB70;});_tmp9B2;});void*_tmp644[2U];_tmp644[0]=& _tmp646,_tmp644[1]=& _tmp647;({unsigned int _tmpB72=clause->loc;struct _dyneither_ptr _tmpB71=({const char*_tmp645="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp645,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpB72,_tmpB71,_tag_dyneither(_tmp644,sizeof(void*),2U));});});}
# 3751
return({struct _tuple31 _tmp9B4;_tmp9B4.f1=cvtenv,_tmp9B4.f2=relns;_tmp9B4;});}
# 3781 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Tcutil_CVTEnv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr);
# 3786
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_aggr(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 3792
{union Cyc_Absyn_AggrInfo _tmp648=*info;union Cyc_Absyn_AggrInfo _tmp649=_tmp648;struct Cyc_Absyn_Aggrdecl*_tmp66C;enum Cyc_Absyn_AggrKind _tmp66B;struct _tuple2*_tmp66A;struct Cyc_Core_Opt*_tmp669;if((_tmp649.UnknownAggr).tag == 1){_LL1: _tmp66B=((_tmp649.UnknownAggr).val).f1;_tmp66A=((_tmp649.UnknownAggr).val).f2;_tmp669=((_tmp649.UnknownAggr).val).f3;_LL2: {
# 3794
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp64A;_push_handler(& _tmp64A);{int _tmp64C=0;if(setjmp(_tmp64A.handler))_tmp64C=1;if(!_tmp64C){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp66A);{
struct Cyc_Absyn_Aggrdecl*_tmp64D=*adp;
if((int)_tmp64D->kind != (int)_tmp66B){
if((int)_tmp64D->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp650=({struct Cyc_String_pa_PrintArg_struct _tmp9B6;_tmp9B6.tag=0U,({
struct _dyneither_ptr _tmpB73=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp66A));_tmp9B6.f1=_tmpB73;});_tmp9B6;});struct Cyc_String_pa_PrintArg_struct _tmp651=({struct Cyc_String_pa_PrintArg_struct _tmp9B5;_tmp9B5.tag=0U,({struct _dyneither_ptr _tmpB74=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp66A));_tmp9B5.f1=_tmpB74;});_tmp9B5;});void*_tmp64E[2U];_tmp64E[0]=& _tmp650,_tmp64E[1]=& _tmp651;({unsigned int _tmpB76=loc;struct _dyneither_ptr _tmpB75=({const char*_tmp64F="expecting struct %s instead of union %s";_tag_dyneither(_tmp64F,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpB76,_tmpB75,_tag_dyneither(_tmp64E,sizeof(void*),2U));});});else{
# 3803
({struct Cyc_String_pa_PrintArg_struct _tmp654=({struct Cyc_String_pa_PrintArg_struct _tmp9B8;_tmp9B8.tag=0U,({
struct _dyneither_ptr _tmpB77=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp66A));_tmp9B8.f1=_tmpB77;});_tmp9B8;});struct Cyc_String_pa_PrintArg_struct _tmp655=({struct Cyc_String_pa_PrintArg_struct _tmp9B7;_tmp9B7.tag=0U,({struct _dyneither_ptr _tmpB78=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp66A));_tmp9B7.f1=_tmpB78;});_tmp9B7;});void*_tmp652[2U];_tmp652[0]=& _tmp654,_tmp652[1]=& _tmp655;({unsigned int _tmpB7A=loc;struct _dyneither_ptr _tmpB79=({const char*_tmp653="expecting union %s instead of struct %s";_tag_dyneither(_tmp653,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpB7A,_tmpB79,_tag_dyneither(_tmp652,sizeof(void*),2U));});});}}
# 3806
if((unsigned int)_tmp669  && (int)_tmp669->v){
if(!((unsigned int)_tmp64D->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp64D->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp658=({struct Cyc_String_pa_PrintArg_struct _tmp9B9;_tmp9B9.tag=0U,({
struct _dyneither_ptr _tmpB7B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp66A));_tmp9B9.f1=_tmpB7B;});_tmp9B9;});void*_tmp656[1U];_tmp656[0]=& _tmp658;({unsigned int _tmpB7D=loc;struct _dyneither_ptr _tmpB7C=({const char*_tmp657="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp657,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpB7D,_tmpB7C,_tag_dyneither(_tmp656,sizeof(void*),1U));});});}
# 3812
({union Cyc_Absyn_AggrInfo _tmpB7E=Cyc_Absyn_KnownAggr(adp);*info=_tmpB7E;});};
# 3796
;_pop_handler();}else{void*_tmp64B=(void*)Cyc_Core_get_exn_thrown();void*_tmp659=_tmp64B;void*_tmp65F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp659)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3816
struct Cyc_Absyn_Aggrdecl*_tmp65A=({struct Cyc_Absyn_Aggrdecl*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->kind=_tmp66B,_tmp65E->sc=Cyc_Absyn_Extern,_tmp65E->name=_tmp66A,_tmp65E->tvs=0,_tmp65E->impl=0,_tmp65E->attributes=0,_tmp65E->expected_mem_kind=0;_tmp65E;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp65A);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp66A);
({union Cyc_Absyn_AggrInfo _tmpB7F=Cyc_Absyn_KnownAggr(adp);*info=_tmpB7F;});
# 3821
if(*targs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp65D=({struct Cyc_String_pa_PrintArg_struct _tmp9BA;_tmp9BA.tag=0U,({struct _dyneither_ptr _tmpB80=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp66A));_tmp9BA.f1=_tmpB80;});_tmp9BA;});void*_tmp65B[1U];_tmp65B[0]=& _tmp65D;({unsigned int _tmpB82=loc;struct _dyneither_ptr _tmpB81=({const char*_tmp65C="declare parameterized type %s before using";_tag_dyneither(_tmp65C,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpB82,_tmpB81,_tag_dyneither(_tmp65B,sizeof(void*),1U));});});
return cvtenv;}
# 3825
goto _LL5;}}else{_LL8: _tmp65F=_tmp659;_LL9:(int)_rethrow(_tmp65F);}_LL5:;}};}
# 3827
_tmp66C=*adp;goto _LL4;}}else{_LL3: _tmp66C=*(_tmp649.KnownAggr).val;_LL4: {
# 3829
struct Cyc_List_List*tvs=_tmp66C->tvs;
struct Cyc_List_List*ts=*targs;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp660=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp661=(void*)ts->hd;
# 3837
{struct _tuple0 _tmp662=({struct _tuple0 _tmp9BB;({void*_tmpB83=Cyc_Absyn_compress_kb(_tmp660->kind);_tmp9BB.f1=_tmpB83;}),_tmp9BB.f2=_tmp661;_tmp9BB;});struct _tuple0 _tmp663=_tmp662;struct Cyc_Absyn_Tvar*_tmp664;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp663.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp663.f2)->tag == 2U){_LLB: _tmp664=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp663.f2)->f1;_LLC:
# 3839
({struct Cyc_List_List*_tmpB84=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp664);cvtenv.kind_env=_tmpB84;});
({struct Cyc_List_List*_tmpB85=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp664,1);cvtenv.free_vars=_tmpB85;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 3844
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 3848
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp667=({struct Cyc_String_pa_PrintArg_struct _tmp9BC;_tmp9BC.tag=0U,({struct _dyneither_ptr _tmpB86=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp66C->name));_tmp9BC.f1=_tmpB86;});_tmp9BC;});void*_tmp665[1U];_tmp665[0]=& _tmp667;({unsigned int _tmpB88=loc;struct _dyneither_ptr _tmpB87=({const char*_tmp666="too many parameters for type %s";_tag_dyneither(_tmp666,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpB88,_tmpB87,_tag_dyneither(_tmp665,sizeof(void*),1U));});});
if(tvs != 0){
# 3852
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->hd=e,_tmp668->tl=hidden_ts;_tmp668;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 3859
({struct Cyc_List_List*_tmpB8A=({struct Cyc_List_List*_tmpB89=*targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB89,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targs=_tmpB8A;});}
# 3861
if((allow_abs_aggr  && _tmp66C->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 3866
_tmp66C->expected_mem_kind=1;}}_LL0:;}
# 3869
return cvtenv;}
# 3873
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 3879
struct Cyc_List_List*_tmp66D=*targsp;
{union Cyc_Absyn_DatatypeInfo _tmp66E=*info;union Cyc_Absyn_DatatypeInfo _tmp66F=_tmp66E;struct Cyc_Absyn_Datatypedecl*_tmp686;struct _tuple2*_tmp685;int _tmp684;if((_tmp66F.UnknownDatatype).tag == 1){_LL1: _tmp685=((_tmp66F.UnknownDatatype).val).name;_tmp684=((_tmp66F.UnknownDatatype).val).is_extensible;_LL2: {
# 3882
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp670;_push_handler(& _tmp670);{int _tmp672=0;if(setjmp(_tmp670.handler))_tmp672=1;if(!_tmp672){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp685);;_pop_handler();}else{void*_tmp671=(void*)Cyc_Core_get_exn_thrown();void*_tmp673=_tmp671;void*_tmp679;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp673)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3886
struct Cyc_Absyn_Datatypedecl*_tmp674=({struct Cyc_Absyn_Datatypedecl*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->sc=Cyc_Absyn_Extern,_tmp678->name=_tmp685,_tmp678->tvs=0,_tmp678->fields=0,_tmp678->is_extensible=_tmp684;_tmp678;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp674);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp685);
# 3890
if(_tmp66D != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp677=({struct Cyc_String_pa_PrintArg_struct _tmp9BD;_tmp9BD.tag=0U,({
struct _dyneither_ptr _tmpB8B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp685));_tmp9BD.f1=_tmpB8B;});_tmp9BD;});void*_tmp675[1U];_tmp675[0]=& _tmp677;({unsigned int _tmpB8D=loc;struct _dyneither_ptr _tmpB8C=({const char*_tmp676="declare parameterized datatype %s before using";_tag_dyneither(_tmp676,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpB8D,_tmpB8C,_tag_dyneither(_tmp675,sizeof(void*),1U));});});
return cvtenv;}
# 3895
goto _LL5;}}else{_LL8: _tmp679=_tmp673;_LL9:(int)_rethrow(_tmp679);}_LL5:;}};}
# 3899
if(_tmp684  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp67C=({struct Cyc_String_pa_PrintArg_struct _tmp9BE;_tmp9BE.tag=0U,({struct _dyneither_ptr _tmpB8E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp685));_tmp9BE.f1=_tmpB8E;});_tmp9BE;});void*_tmp67A[1U];_tmp67A[0]=& _tmp67C;({unsigned int _tmpB90=loc;struct _dyneither_ptr _tmpB8F=({const char*_tmp67B="datatype %s was not declared @extensible";_tag_dyneither(_tmp67B,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpB90,_tmpB8F,_tag_dyneither(_tmp67A,sizeof(void*),1U));});});
({union Cyc_Absyn_DatatypeInfo _tmpB91=Cyc_Absyn_KnownDatatype(tudp);*info=_tmpB91;});
_tmp686=*tudp;goto _LL4;}}else{_LL3: _tmp686=*(_tmp66F.KnownDatatype).val;_LL4: {
# 3905
struct Cyc_List_List*tvs=_tmp686->tvs;
for(0;_tmp66D != 0  && tvs != 0;(_tmp66D=_tmp66D->tl,tvs=tvs->tl)){
void*t=(void*)_tmp66D->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3911
{struct _tuple0 _tmp67D=({struct _tuple0 _tmp9BF;({void*_tmpB92=Cyc_Absyn_compress_kb(tv->kind);_tmp9BF.f1=_tmpB92;}),_tmp9BF.f2=t;_tmp9BF;});struct _tuple0 _tmp67E=_tmp67D;struct Cyc_Absyn_Tvar*_tmp67F;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp67E.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp67E.f2)->tag == 2U){_LLB: _tmp67F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp67E.f2)->f1;_LLC:
# 3913
({struct Cyc_List_List*_tmpB93=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp67F);cvtenv.kind_env=_tmpB93;});
({struct Cyc_List_List*_tmpB94=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp67F,1);cvtenv.free_vars=_tmpB94;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 3918
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3922
if(_tmp66D != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp682=({struct Cyc_String_pa_PrintArg_struct _tmp9C0;_tmp9C0.tag=0U,({
struct _dyneither_ptr _tmpB95=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp686->name));_tmp9C0.f1=_tmpB95;});_tmp9C0;});void*_tmp680[1U];_tmp680[0]=& _tmp682;({unsigned int _tmpB97=loc;struct _dyneither_ptr _tmpB96=({const char*_tmp681="too many type arguments for datatype %s";_tag_dyneither(_tmp681,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpB97,_tmpB96,_tag_dyneither(_tmp680,sizeof(void*),1U));});});
if(tvs != 0){
# 3927
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->hd=e,_tmp683->tl=hidden_ts;_tmp683;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 3934
({struct Cyc_List_List*_tmpB99=({struct Cyc_List_List*_tmpB98=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB98,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmpB99;});}
# 3936
goto _LL0;}}_LL0:;}
# 3938
return cvtenv;}
# 3942
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype_field(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List*targs,int allow_abs_aggr){
# 3948
{union Cyc_Absyn_DatatypeFieldInfo _tmp687=*info;union Cyc_Absyn_DatatypeFieldInfo _tmp688=_tmp687;struct Cyc_Absyn_Datatypedecl*_tmp69B;struct Cyc_Absyn_Datatypefield*_tmp69A;struct _tuple2*_tmp699;struct _tuple2*_tmp698;int _tmp697;if((_tmp688.UnknownDatatypefield).tag == 1){_LL1: _tmp699=((_tmp688.UnknownDatatypefield).val).datatype_name;_tmp698=((_tmp688.UnknownDatatypefield).val).field_name;_tmp697=((_tmp688.UnknownDatatypefield).val).is_extensible;_LL2: {
# 3951
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp699);
struct Cyc_Absyn_Datatypefield*tuf;
# 3956
{struct Cyc_List_List*_tmp689=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp689=_tmp689->tl){
if(_tmp689 == 0)({void*_tmp68A=0U;({struct _dyneither_ptr _tmpB9A=({const char*_tmp68B="Tcutil found a bad datatypefield";_tag_dyneither(_tmp68B,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB9A,_tag_dyneither(_tmp68A,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp689->hd)->name,_tmp698)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp689->hd;
break;}}}
# 3963
({union Cyc_Absyn_DatatypeFieldInfo _tmpB9B=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmpB9B;});
_tmp69B=tud;_tmp69A=tuf;goto _LL4;}}else{_LL3: _tmp69B=((_tmp688.KnownDatatypefield).val).f1;_tmp69A=((_tmp688.KnownDatatypefield).val).f2;_LL4: {
# 3967
struct Cyc_List_List*tvs=_tmp69B->tvs;
for(0;targs != 0  && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3973
{struct _tuple0 _tmp68C=({struct _tuple0 _tmp9C1;({void*_tmpB9C=Cyc_Absyn_compress_kb(tv->kind);_tmp9C1.f1=_tmpB9C;}),_tmp9C1.f2=t;_tmp9C1;});struct _tuple0 _tmp68D=_tmp68C;struct Cyc_Absyn_Tvar*_tmp68E;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp68D.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68D.f2)->tag == 2U){_LL6: _tmp68E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68D.f2)->f1;_LL7:
# 3975
({struct Cyc_List_List*_tmpB9D=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp68E);cvtenv.kind_env=_tmpB9D;});
({struct Cyc_List_List*_tmpB9E=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp68E,1);cvtenv.free_vars=_tmpB9E;});
continue;}else{goto _LL8;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}{
# 3980
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3984
if(targs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp691=({struct Cyc_String_pa_PrintArg_struct _tmp9C3;_tmp9C3.tag=0U,({
struct _dyneither_ptr _tmpB9F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69B->name));_tmp9C3.f1=_tmpB9F;});_tmp9C3;});struct Cyc_String_pa_PrintArg_struct _tmp692=({struct Cyc_String_pa_PrintArg_struct _tmp9C2;_tmp9C2.tag=0U,({struct _dyneither_ptr _tmpBA0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69A->name));_tmp9C2.f1=_tmpBA0;});_tmp9C2;});void*_tmp68F[2U];_tmp68F[0]=& _tmp691,_tmp68F[1]=& _tmp692;({unsigned int _tmpBA2=loc;struct _dyneither_ptr _tmpBA1=({const char*_tmp690="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp690,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpBA2,_tmpBA1,_tag_dyneither(_tmp68F,sizeof(void*),2U));});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp695=({struct Cyc_String_pa_PrintArg_struct _tmp9C5;_tmp9C5.tag=0U,({
struct _dyneither_ptr _tmpBA3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69B->name));_tmp9C5.f1=_tmpBA3;});_tmp9C5;});struct Cyc_String_pa_PrintArg_struct _tmp696=({struct Cyc_String_pa_PrintArg_struct _tmp9C4;_tmp9C4.tag=0U,({struct _dyneither_ptr _tmpBA4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69A->name));_tmp9C4.f1=_tmpBA4;});_tmp9C4;});void*_tmp693[2U];_tmp693[0]=& _tmp695,_tmp693[1]=& _tmp696;({unsigned int _tmpBA6=loc;struct _dyneither_ptr _tmpBA5=({const char*_tmp694="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp694,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpBA6,_tmpBA5,_tag_dyneither(_tmp693,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 3992
return cvtenv;}
# 3995
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_app(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 4000
struct Cyc_List_List*_tmp69C=*targsp;
{void*_tmp69D=c;union Cyc_Absyn_DatatypeFieldInfo*_tmp6C4;union Cyc_Absyn_DatatypeInfo*_tmp6C3;union Cyc_Absyn_AggrInfo*_tmp6C2;struct Cyc_List_List*_tmp6C1;struct _tuple2*_tmp6C0;struct Cyc_Absyn_Enumdecl**_tmp6BF;switch(*((int*)_tmp69D)){case 1U: _LL1: _LL2:
# 4003
 goto _LL4;case 2U: _LL3: _LL4: goto _LL6;case 0U: _LL5: _LL6: goto _LL8;case 7U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA: goto _LLC;case 5U: _LLB: _LLC: goto _LLE;case 12U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10: goto _LL12;case 14U: _LL11: _LL12: goto _LL14;case 17U: _LL13: _LL14:
# 4007
 if(_tmp69C != 0)({struct Cyc_String_pa_PrintArg_struct _tmp6A0=({struct Cyc_String_pa_PrintArg_struct _tmp9C6;_tmp9C6.tag=0U,({
struct _dyneither_ptr _tmpBA7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->tag=0U,_tmp6A1->f1=c,_tmp6A1->f2=_tmp69C;_tmp6A1;})));_tmp9C6.f1=_tmpBA7;});_tmp9C6;});void*_tmp69E[1U];_tmp69E[0]=& _tmp6A0;({struct _dyneither_ptr _tmpBA8=({const char*_tmp69F="%s applied to argument(s)";_tag_dyneither(_tmp69F,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBA8,_tag_dyneither(_tmp69E,sizeof(void*),1U));});});
goto _LL0;case 9U: _LL15: _LL16:
# 4011
 for(0;_tmp69C != 0;_tmp69C=_tmp69C->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp69C->hd,1,1);}
goto _LL0;case 4U: _LL17: _LL18:
# 4015
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp69C)!= 1)({void*_tmp6A2=0U;({struct _dyneither_ptr _tmpBA9=({const char*_tmp6A3="tag_t applied to wrong number of arguments";_tag_dyneither(_tmp6A3,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBA9,_tag_dyneither(_tmp6A2,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp69C))->hd,0,1);goto _LL0;case 15U: _LL19: _tmp6C0=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp69D)->f1;_tmp6BF=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp69D)->f2;_LL1A:
# 4018
 if(_tmp69C != 0)({void*_tmp6A4=0U;({struct _dyneither_ptr _tmpBAA=({const char*_tmp6A5="enum applied to argument(s)";_tag_dyneither(_tmp6A5,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBAA,_tag_dyneither(_tmp6A4,sizeof(void*),0U));});});
if(*_tmp6BF == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp6BF))->fields == 0){
struct _handler_cons _tmp6A6;_push_handler(& _tmp6A6);{int _tmp6A8=0;if(setjmp(_tmp6A6.handler))_tmp6A8=1;if(!_tmp6A8){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6C0);
*_tmp6BF=*ed;}
# 4021
;_pop_handler();}else{void*_tmp6A7=(void*)Cyc_Core_get_exn_thrown();void*_tmp6A9=_tmp6A7;void*_tmp6AC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6A9)->tag == Cyc_Dict_Absent){_LL2C: _LL2D: {
# 4025
struct Cyc_Absyn_Enumdecl*_tmp6AA=({struct Cyc_Absyn_Enumdecl*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB->sc=Cyc_Absyn_Extern,_tmp6AB->name=_tmp6C0,_tmp6AB->fields=0;_tmp6AB;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp6AA);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6C0);
*_tmp6BF=*ed;
goto _LL2B;};}}else{_LL2E: _tmp6AC=_tmp6A9;_LL2F:(int)_rethrow(_tmp6AC);}_LL2B:;}};}
# 4031
goto _LL0;case 16U: _LL1B: _tmp6C1=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp69D)->f1;_LL1C:
# 4033
 if(_tmp69C != 0)({void*_tmp6AD=0U;({struct _dyneither_ptr _tmpBAB=({const char*_tmp6AE="enum applied to argument(s)";_tag_dyneither(_tmp6AE,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBAB,_tag_dyneither(_tmp6AD,sizeof(void*),0U));});});{
# 4035
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp6C1 != 0;_tmp6C1=_tmp6C1->tl){
struct Cyc_Absyn_Enumfield*_tmp6AF=(struct Cyc_Absyn_Enumfield*)_tmp6C1->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp6AF->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp6B2=({struct Cyc_String_pa_PrintArg_struct _tmp9C7;_tmp9C7.tag=0U,_tmp9C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6AF->name).f2);_tmp9C7;});void*_tmp6B0[1U];_tmp6B0[0]=& _tmp6B2;({unsigned int _tmpBAD=_tmp6AF->loc;struct _dyneither_ptr _tmpBAC=({const char*_tmp6B1="duplicate enum field name %s";_tag_dyneither(_tmp6B1,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpBAD,_tmpBAC,_tag_dyneither(_tmp6B0,sizeof(void*),1U));});});else{
# 4042
prev_fields=({struct Cyc_List_List*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3->hd=(*_tmp6AF->name).f2,_tmp6B3->tl=prev_fields;_tmp6B3;});}
if(_tmp6AF->tag == 0)
({struct Cyc_Absyn_Exp*_tmpBAE=Cyc_Absyn_uint_exp(tag_count,_tmp6AF->loc);_tmp6AF->tag=_tmpBAE;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6AF->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp6B6=({struct Cyc_String_pa_PrintArg_struct _tmp9C8;_tmp9C8.tag=0U,_tmp9C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6AF->name).f2);_tmp9C8;});void*_tmp6B4[1U];_tmp6B4[0]=& _tmp6B6;({unsigned int _tmpBB0=loc;struct _dyneither_ptr _tmpBAF=({const char*_tmp6B5="enum field %s: expression is not constant";_tag_dyneither(_tmp6B5,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpBB0,_tmpBAF,_tag_dyneither(_tmp6B4,sizeof(void*),1U));});});}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6AF->tag))).f1;
tag_count=t1 + (unsigned int)1;};}
# 4050
goto _LL0;};case 10U: _LL1D: _LL1E:
# 4052
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp69C)!= 1)({void*_tmp6B7=0U;({struct _dyneither_ptr _tmpBB1=({const char*_tmp6B8="regions has wrong number of arguments";_tag_dyneither(_tmp6B8,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBB1,_tag_dyneither(_tmp6B7,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,(void*)((struct Cyc_List_List*)_check_null(_tmp69C))->hd,1,1);goto _LL0;case 3U: _LL1F: _LL20:
# 4055
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp69C)!= 1)({void*_tmp6B9=0U;({struct _dyneither_ptr _tmpBB2=({const char*_tmp6BA="region_t has wrong number of arguments";_tag_dyneither(_tmp6BA,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBB2,_tag_dyneither(_tmp6B9,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp69C))->hd,1,1);
goto _LL0;case 13U: _LL21: _LL22:
# 4059
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp69C)!= 1)({void*_tmp6BB=0U;({struct _dyneither_ptr _tmpBB3=({const char*_tmp6BC="@thin has wrong number of arguments";_tag_dyneither(_tmp6BC,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBB3,_tag_dyneither(_tmp6BB,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp69C))->hd,0,1);
goto _LL0;case 8U: _LL23: _LL24:
# 4063
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp69C)!= 1)({void*_tmp6BD=0U;({struct _dyneither_ptr _tmpBB4=({const char*_tmp6BE="access has wrong number of arguments";_tag_dyneither(_tmp6BE,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBB4,_tag_dyneither(_tmp6BD,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp69C))->hd,1,1);goto _LL0;case 20U: _LL25: _tmp6C2=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp69D)->f1;_LL26:
# 4066
 cvtenv=Cyc_Tcutil_i_check_valid_aggr(loc,te,cvtenv,expected_kind,_tmp6C2,targsp,allow_abs_aggr);
# 4068
goto _LL0;case 18U: _LL27: _tmp6C3=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp69D)->f1;_LL28:
# 4070
 cvtenv=Cyc_Tcutil_i_check_valid_datatype(loc,te,cvtenv,expected_kind,_tmp6C3,targsp,allow_abs_aggr);
# 4072
goto _LL0;default: _LL29: _tmp6C4=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp69D)->f1;_LL2A:
# 4074
 cvtenv=Cyc_Tcutil_i_check_valid_datatype_field(loc,te,cvtenv,expected_kind,_tmp6C4,_tmp69C,allow_abs_aggr);
# 4076
goto _LL0;}_LL0:;}
# 4078
return cvtenv;}struct _tuple32{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 4082
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 4090
{void*_tmp6C5=Cyc_Tcutil_compress(t);void*_tmp6C6=_tmp6C5;struct _tuple2*_tmp7CA;struct Cyc_List_List**_tmp7C9;struct Cyc_Absyn_Typedefdecl**_tmp7C8;void**_tmp7C7;enum Cyc_Absyn_AggrKind _tmp7C6;struct Cyc_List_List*_tmp7C5;struct Cyc_List_List*_tmp7C4;struct Cyc_List_List**_tmp7C3;void**_tmp7C2;struct Cyc_Absyn_Tqual*_tmp7C1;void*_tmp7C0;struct Cyc_List_List*_tmp7BF;int _tmp7BE;struct Cyc_Absyn_VarargInfo*_tmp7BD;struct Cyc_List_List*_tmp7BC;struct Cyc_List_List*_tmp7BB;struct Cyc_Absyn_Exp*_tmp7BA;struct Cyc_List_List**_tmp7B9;struct Cyc_Absyn_Exp*_tmp7B8;struct Cyc_List_List**_tmp7B7;void*_tmp7B6;struct Cyc_Absyn_Tqual*_tmp7B5;struct Cyc_Absyn_Exp**_tmp7B4;void*_tmp7B3;unsigned int _tmp7B2;struct Cyc_Absyn_Exp*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B0;void*_tmp7AF;struct Cyc_Absyn_Tqual*_tmp7AE;void*_tmp7AD;void*_tmp7AC;void*_tmp7AB;void*_tmp7AA;void*_tmp7A9;void***_tmp7A8;struct Cyc_Absyn_Tvar*_tmp7A7;struct Cyc_Core_Opt**_tmp7A6;void**_tmp7A5;void*_tmp7A4;struct Cyc_List_List**_tmp7A3;switch(*((int*)_tmp6C6)){case 0U: _LL1: _tmp7A4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6C6)->f1;_tmp7A3=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6C6)->f2;_LL2:
# 4092
 cvtenv=Cyc_Tcutil_i_check_valid_type_app(loc,te,cvtenv,expected_kind,_tmp7A4,_tmp7A3,put_in_effect,allow_abs_aggr);
# 4094
goto _LL0;case 1U: _LL3: _tmp7A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6C6)->f1;_tmp7A5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6C6)->f2;_LL4:
# 4097
 if(*_tmp7A6 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7A6))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7A6))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpBB5=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp7A6=_tmpBB5;});
if(((cvtenv.fn_result  && cvtenv.generalize_evars) && (int)expected_kind->kind == (int)Cyc_Absyn_RgnKind) && !te->tempest_generalize){
# 4102
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
*_tmp7A5=Cyc_Absyn_unique_rgn_type;else{
# 4105
*_tmp7A5=Cyc_Absyn_heap_rgn_type;}}else{
if((cvtenv.generalize_evars  && (int)expected_kind->kind != (int)Cyc_Absyn_BoolKind) && (int)expected_kind->kind != (int)Cyc_Absyn_PtrBndKind){
# 4109
struct Cyc_Absyn_Tvar*_tmp6C7=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->tag=2U,_tmp6C8->f1=0,_tmp6C8->f2=expected_kind;_tmp6C8;}));
({void*_tmpBB6=Cyc_Absyn_var_type(_tmp6C7);*_tmp7A5=_tmpBB6;});
_tmp7A7=_tmp6C7;goto _LL6;}else{
# 4113
({struct Cyc_List_List*_tmpBB7=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpBB7;});}}
# 4115
goto _LL0;case 2U: _LL5: _tmp7A7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6C6)->f1;_LL6:
# 4117
{void*_tmp6C9=Cyc_Absyn_compress_kb(_tmp7A7->kind);void*_tmp6CA=_tmp6C9;struct Cyc_Core_Opt**_tmp6CD;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6CA)->tag == 1U){_LL1A: _tmp6CD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6CA)->f1;_LL1B:
({struct Cyc_Core_Opt*_tmpBB9=({struct Cyc_Core_Opt*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));({void*_tmpBB8=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->tag=2U,_tmp6CB->f1=0,_tmp6CB->f2=expected_kind;_tmp6CB;});_tmp6CC->v=_tmpBB8;});_tmp6CC;});*_tmp6CD=_tmpBB9;});goto _LL19;}else{_LL1C: _LL1D:
 goto _LL19;}_LL19:;}
# 4123
({struct Cyc_List_List*_tmpBBA=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7A7);cvtenv.kind_env=_tmpBBA;});
# 4126
({struct Cyc_List_List*_tmpBBB=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7A7,put_in_effect);cvtenv.free_vars=_tmpBBB;});
# 4128
{void*_tmp6CE=Cyc_Absyn_compress_kb(_tmp7A7->kind);void*_tmp6CF=_tmp6CE;struct Cyc_Core_Opt**_tmp6D3;struct Cyc_Absyn_Kind*_tmp6D2;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6CF)->tag == 2U){_LL1F: _tmp6D3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6CF)->f1;_tmp6D2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6CF)->f2;_LL20:
# 4130
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp6D2))
({struct Cyc_Core_Opt*_tmpBBD=({struct Cyc_Core_Opt*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));({void*_tmpBBC=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->tag=2U,_tmp6D0->f1=0,_tmp6D0->f2=expected_kind;_tmp6D0;});_tmp6D1->v=_tmpBBC;});_tmp6D1;});*_tmp6D3=_tmpBBD;});
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 4135
goto _LL0;case 10U: _LL7: _tmp7A9=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6C6)->f1)->r;_tmp7A8=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6C6)->f2;_LL8: {
# 4141
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp6D4=_tmp7A9;struct Cyc_Absyn_Datatypedecl*_tmp6D7;struct Cyc_Absyn_Enumdecl*_tmp6D6;struct Cyc_Absyn_Aggrdecl*_tmp6D5;switch(*((int*)_tmp6D4)){case 0U: _LL24: _tmp6D5=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp6D4)->f1;_LL25:
# 4144
 if(te->in_extern_c_include)
_tmp6D5->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp6D5);goto _LL23;case 1U: _LL26: _tmp6D6=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp6D4)->f1;_LL27:
# 4148
 if(te->in_extern_c_include)
_tmp6D6->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp6D6);goto _LL23;default: _LL28: _tmp6D7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp6D4)->f1;_LL29:
# 4152
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp6D7);goto _LL23;}_LL23:;}
# 4154
({void**_tmpBBE=({void**_tmp6D8=_cycalloc(sizeof(*_tmp6D8));*_tmp6D8=new_t;_tmp6D8;});*_tmp7A8=_tmpBBE;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 3U: _LL9: _tmp7AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C6)->f1).elt_type;_tmp7AE=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C6)->f1).elt_tq;_tmp7AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C6)->f1).ptr_atts).rgn;_tmp7AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C6)->f1).ptr_atts).nullable;_tmp7AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C6)->f1).ptr_atts).bounds;_tmp7AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C6)->f1).ptr_atts).zero_term;_LLA: {
# 4160
int is_zero_terminated;
# 4162
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7AF,1,1);
({int _tmpBBF=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7AE->print_const,_tmp7AF);_tmp7AE->real_const=_tmpBBF;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp6D9=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp6DA=_tmp6D9;switch(_tmp6DA){case Cyc_Absyn_Aliasable: _LL2B: _LL2C:
 k=& Cyc_Tcutil_rk;goto _LL2A;case Cyc_Absyn_Unique: _LL2D: _LL2E:
 k=& Cyc_Tcutil_urk;goto _LL2A;case Cyc_Absyn_Top: _LL2F: _LL30:
 goto _LL32;default: _LL31: _LL32:
 k=& Cyc_Tcutil_trk;goto _LL2A;}_LL2A:;}
# 4171
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp7AD,1,1);
# 4174
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7AA,0,1);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7AC,0,1);
({void*_tmpBC0=_tmp7AA;Cyc_Tcutil_unify(_tmpBC0,Cyc_Tcutil_is_char_type(_tmp7AF)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});
is_zero_terminated=Cyc_Absyn_type2bool(0,_tmp7AA);
if(is_zero_terminated){
# 4180
if(!Cyc_Tcutil_admits_zero(_tmp7AF))
({struct Cyc_String_pa_PrintArg_struct _tmp6DD=({struct Cyc_String_pa_PrintArg_struct _tmp9C9;_tmp9C9.tag=0U,({
struct _dyneither_ptr _tmpBC1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7AF));_tmp9C9.f1=_tmpBC1;});_tmp9C9;});void*_tmp6DB[1U];_tmp6DB[0]=& _tmp6DD;({unsigned int _tmpBC3=loc;struct _dyneither_ptr _tmpBC2=({const char*_tmp6DC="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp6DC,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpBC3,_tmpBC2,_tag_dyneither(_tmp6DB,sizeof(void*),1U));});});}
# 4185
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ptrbk,_tmp7AB,0,allow_abs_aggr);{
struct Cyc_Absyn_Exp*_tmp6DE=({void*_tmpBC4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBC4,_tmp7AB);});
if(_tmp6DE != 0){
struct _tuple13 _tmp6DF=Cyc_Evexp_eval_const_uint_exp(_tmp6DE);struct _tuple13 _tmp6E0=_tmp6DF;unsigned int _tmp6E4;int _tmp6E3;_LL34: _tmp6E4=_tmp6E0.f1;_tmp6E3=_tmp6E0.f2;_LL35:;
if(is_zero_terminated  && (!_tmp6E3  || _tmp6E4 < (unsigned int)1))
({void*_tmp6E1=0U;({unsigned int _tmpBC6=loc;struct _dyneither_ptr _tmpBC5=({const char*_tmp6E2="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp6E2,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpBC6,_tmpBC5,_tag_dyneither(_tmp6E1,sizeof(void*),0U));});});}
# 4192
goto _LL0;};};}case 9U: _LLB: _tmp7B0=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6C6)->f1;_LLC:
# 4197
({struct Cyc_Tcenv_Tenv*_tmpBC7=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpBC7,0,_tmp7B0);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp7B0))
({void*_tmp6E5=0U;({unsigned int _tmpBC9=loc;struct _dyneither_ptr _tmpBC8=({const char*_tmp6E6="valueof_t requires an int expression";_tag_dyneither(_tmp6E6,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpBC9,_tmpBC8,_tag_dyneither(_tmp6E5,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7B0,te,cvtenv);
goto _LL0;case 11U: _LLD: _tmp7B1=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6C6)->f1;_LLE:
# 4206
({struct Cyc_Tcenv_Tenv*_tmpBCA=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpBCA,0,_tmp7B1);});
goto _LL0;case 4U: _LLF: _tmp7B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C6)->f1).elt_type;_tmp7B5=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C6)->f1).tq;_tmp7B4=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C6)->f1).num_elts;_tmp7B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C6)->f1).zero_term;_tmp7B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C6)->f1).zt_loc;_LL10: {
# 4211
struct Cyc_Absyn_Exp*_tmp6E7=*_tmp7B4;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7B6,1,allow_abs_aggr);
({int _tmpBCB=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7B5->print_const,_tmp7B6);_tmp7B5->real_const=_tmpBCB;});{
# 4215
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,_tmp7B3);
if(is_zero_terminated){
# 4218
if(!Cyc_Tcutil_admits_zero(_tmp7B6))
({struct Cyc_String_pa_PrintArg_struct _tmp6EA=({struct Cyc_String_pa_PrintArg_struct _tmp9CA;_tmp9CA.tag=0U,({
struct _dyneither_ptr _tmpBCC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7B6));_tmp9CA.f1=_tmpBCC;});_tmp9CA;});void*_tmp6E8[1U];_tmp6E8[0]=& _tmp6EA;({unsigned int _tmpBCE=loc;struct _dyneither_ptr _tmpBCD=({const char*_tmp6E9="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp6E9,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpBCE,_tmpBCD,_tag_dyneither(_tmp6E8,sizeof(void*),1U));});});}
# 4224
if(_tmp6E7 == 0){
# 4226
if(is_zero_terminated)
# 4228
({struct Cyc_Absyn_Exp*_tmpBCF=_tmp6E7=Cyc_Absyn_uint_exp(1U,0U);*_tmp7B4=_tmpBCF;});else{
# 4231
({void*_tmp6EB=0U;({unsigned int _tmpBD1=loc;struct _dyneither_ptr _tmpBD0=({const char*_tmp6EC="array bound defaults to 1 here";_tag_dyneither(_tmp6EC,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpBD1,_tmpBD0,_tag_dyneither(_tmp6EB,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpBD2=_tmp6E7=Cyc_Absyn_uint_exp(1U,0U);*_tmp7B4=_tmpBD2;});}}
# 4235
({struct Cyc_Tcenv_Tenv*_tmpBD3=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpBD3,0,_tmp6E7);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp6E7))
({void*_tmp6ED=0U;({unsigned int _tmpBD5=loc;struct _dyneither_ptr _tmpBD4=({const char*_tmp6EE="array bounds expression is not an unsigned int";_tag_dyneither(_tmp6EE,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBD5,_tmpBD4,_tag_dyneither(_tmp6ED,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp6E7,te,cvtenv);{
# 4243
struct _tuple13 _tmp6EF=Cyc_Evexp_eval_const_uint_exp(_tmp6E7);struct _tuple13 _tmp6F0=_tmp6EF;unsigned int _tmp6F6;int _tmp6F5;_LL37: _tmp6F6=_tmp6F0.f1;_tmp6F5=_tmp6F0.f2;_LL38:;
# 4245
if((is_zero_terminated  && _tmp6F5) && _tmp6F6 < (unsigned int)1)
({void*_tmp6F1=0U;({unsigned int _tmpBD7=loc;struct _dyneither_ptr _tmpBD6=({const char*_tmp6F2="zero terminated array cannot have zero elements";_tag_dyneither(_tmp6F2,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpBD7,_tmpBD6,_tag_dyneither(_tmp6F1,sizeof(void*),0U));});});
# 4248
if((_tmp6F5  && _tmp6F6 < (unsigned int)1) && Cyc_Cyclone_tovc_r){
({void*_tmp6F3=0U;({unsigned int _tmpBD9=loc;struct _dyneither_ptr _tmpBD8=({const char*_tmp6F4="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp6F4,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpBD9,_tmpBD8,_tag_dyneither(_tmp6F3,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpBDA=Cyc_Absyn_uint_exp(1U,0U);*_tmp7B4=_tmpBDA;});}
# 4252
goto _LL0;};};}case 5U: _LL11: _tmp7C3=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).tvars;_tmp7C2=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).effect;_tmp7C1=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).ret_tqual;_tmp7C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).ret_type;_tmp7BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).args;_tmp7BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).c_varargs;_tmp7BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).cyc_varargs;_tmp7BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).rgn_po;_tmp7BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).attributes;_tmp7BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).requires_clause;_tmp7B9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).requires_relns;_tmp7B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).ensures_clause;_tmp7B7=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C6)->f1).ensures_relns;_LL12: {
# 4259
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp7BB != 0;_tmp7BB=_tmp7BB->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7BB->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp6F9=({struct Cyc_String_pa_PrintArg_struct _tmp9CB;_tmp9CB.tag=0U,({struct _dyneither_ptr _tmpBDB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7BB->hd));_tmp9CB.f1=_tmpBDB;});_tmp9CB;});void*_tmp6F7[1U];_tmp6F7[0]=& _tmp6F9;({unsigned int _tmpBDD=loc;struct _dyneither_ptr _tmpBDC=({const char*_tmp6F8="bad function type attribute %s";_tag_dyneither(_tmp6F8,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpBDD,_tmpBDC,_tag_dyneither(_tmp6F7,sizeof(void*),1U));});});{
void*_tmp6FA=(void*)_tmp7BB->hd;void*_tmp6FB=_tmp6FA;enum Cyc_Absyn_Format_Type _tmp700;int _tmp6FF;int _tmp6FE;switch(*((int*)_tmp6FB)){case 1U: _LL3A: _LL3B:
# 4272
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL39;case 2U: _LL3C: _LL3D:
# 4274
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL39;case 3U: _LL3E: _LL3F:
# 4276
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL39;case 19U: _LL40: _tmp700=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp6FB)->f1;_tmp6FF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp6FB)->f2;_tmp6FE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp6FB)->f3;_LL41:
# 4278
 if(!seen_format){
seen_format=1;
ft=_tmp700;
fmt_desc_arg=_tmp6FF;
fmt_arg_start=_tmp6FE;}else{
# 4284
({void*_tmp6FC=0U;({unsigned int _tmpBDF=loc;struct _dyneither_ptr _tmpBDE=({const char*_tmp6FD="function can't have multiple format attributes";_tag_dyneither(_tmp6FD,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBDF,_tmpBDE,_tag_dyneither(_tmp6FC,sizeof(void*),0U));});});}
goto _LL39;default: _LL42: _LL43:
 goto _LL39;}_LL39:;};}
# 4289
if(num_convs > 1)
({void*_tmp701=0U;({unsigned int _tmpBE1=loc;struct _dyneither_ptr _tmpBE0=({const char*_tmp702="function can't have multiple calling conventions";_tag_dyneither(_tmp702,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpBE1,_tmpBE0,_tag_dyneither(_tmp701,sizeof(void*),0U));});});
# 4294
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7C3);
{struct Cyc_List_List*b=*_tmp7C3;for(0;b != 0;b=b->tl){
({int _tmpBE2=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpBE2;});
({struct Cyc_List_List*_tmpBE3=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpBE3;});{
void*_tmp703=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp704=_tmp703;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp704)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp704)->f1)->kind == Cyc_Absyn_MemKind){_LL45: _LL46:
# 4300
({struct Cyc_String_pa_PrintArg_struct _tmp707=({struct Cyc_String_pa_PrintArg_struct _tmp9CC;_tmp9CC.tag=0U,_tmp9CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);_tmp9CC;});void*_tmp705[1U];_tmp705[0]=& _tmp707;({unsigned int _tmpBE5=loc;struct _dyneither_ptr _tmpBE4=({const char*_tmp706="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp706,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpBE5,_tmpBE4,_tag_dyneither(_tmp705,sizeof(void*),1U));});});
goto _LL44;}else{goto _LL47;}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;};}}{
# 4308
struct Cyc_Tcutil_CVTEnv _tmp708=({struct Cyc_Tcutil_CVTEnv _tmp9CF;_tmp9CF.r=Cyc_Core_heap_region,_tmp9CF.kind_env=cvtenv.kind_env,_tmp9CF.free_vars=0,_tmp9CF.free_evars=0,_tmp9CF.generalize_evars=cvtenv.generalize_evars,_tmp9CF.fn_result=1;_tmp9CF;});
# 4310
_tmp708=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp708,& Cyc_Tcutil_tmk,_tmp7C0,1,1);
({int _tmpBE6=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7C1->print_const,_tmp7C0);_tmp7C1->real_const=_tmpBE6;});
_tmp708.fn_result=0;
# 4316
{struct Cyc_List_List*a=_tmp7BF;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp709=(struct _tuple10*)a->hd;
void*_tmp70A=(*_tmp709).f3;
_tmp708=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp708,& Cyc_Tcutil_tmk,_tmp70A,1,1);{
int _tmp70B=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp709).f2).print_const,_tmp70A);
((*_tmp709).f2).real_const=_tmp70B;
# 4324
if(Cyc_Tcutil_is_array_type(_tmp70A)){
# 4326
void*_tmp70C=Cyc_Absyn_new_evar(0,0);
_tmp708=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp708,& Cyc_Tcutil_rk,_tmp70C,1,1);
({void*_tmpBE7=Cyc_Tcutil_promote_array(_tmp70A,_tmp70C,0);(*_tmp709).f3=_tmpBE7;});}};}}
# 4333
if(_tmp7BD != 0){
if(_tmp7BE)({void*_tmp70D=0U;({struct _dyneither_ptr _tmpBE8=({const char*_tmp70E="both c_vararg and cyc_vararg";_tag_dyneither(_tmp70E,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBE8,_tag_dyneither(_tmp70D,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp70F=*_tmp7BD;struct Cyc_Absyn_VarargInfo _tmp710=_tmp70F;void*_tmp721;int _tmp720;_LL4A: _tmp721=_tmp710.type;_tmp720=_tmp710.inject;_LL4B:;
_tmp708=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp708,& Cyc_Tcutil_tmk,_tmp721,1,1);
({int _tmpBE9=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp7BD->tq).print_const,_tmp721);(_tmp7BD->tq).real_const=_tmpBE9;});
# 4339
if(_tmp720){
void*_tmp711=Cyc_Tcutil_compress(_tmp721);void*_tmp712=_tmp711;void*_tmp71F;void*_tmp71E;void*_tmp71D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp712)->tag == 3U){_LL4D: _tmp71F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp712)->f1).elt_type;_tmp71E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp712)->f1).ptr_atts).bounds;_tmp71D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp712)->f1).ptr_atts).zero_term;_LL4E:
# 4342
{void*_tmp713=Cyc_Tcutil_compress(_tmp71F);void*_tmp714=_tmp713;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp714)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp714)->f1)->tag == 18U){_LL52: _LL53:
# 4344
 if(Cyc_Tcutil_force_type2bool(0,_tmp71D))
({void*_tmp715=0U;({unsigned int _tmpBEB=loc;struct _dyneither_ptr _tmpBEA=({const char*_tmp716="can't inject into a zeroterm pointer";_tag_dyneither(_tmp716,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpBEB,_tmpBEA,_tag_dyneither(_tmp715,sizeof(void*),0U));});});
if(!({void*_tmpBEC=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpBEC,_tmp71E);}))
({void*_tmp717=0U;({unsigned int _tmpBEE=loc;struct _dyneither_ptr _tmpBED=({const char*_tmp718="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp718,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpBEE,_tmpBED,_tag_dyneither(_tmp717,sizeof(void*),0U));});});
goto _LL51;}else{goto _LL54;}}else{_LL54: _LL55:
({void*_tmp719=0U;({unsigned int _tmpBF0=loc;struct _dyneither_ptr _tmpBEF=({const char*_tmp71A="can't inject a non-datatype type";_tag_dyneither(_tmp71A,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpBF0,_tmpBEF,_tag_dyneither(_tmp719,sizeof(void*),0U));});});goto _LL51;}_LL51:;}
# 4351
goto _LL4C;}else{_LL4F: _LL50:
({void*_tmp71B=0U;({unsigned int _tmpBF2=loc;struct _dyneither_ptr _tmpBF1=({const char*_tmp71C="expecting a datatype pointer type";_tag_dyneither(_tmp71C,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpBF2,_tmpBF1,_tag_dyneither(_tmp71B,sizeof(void*),0U));});});goto _LL4C;}_LL4C:;}};}
# 4357
if(seen_format){
int _tmp722=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7BF);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp722) || fmt_arg_start < 0) || 
# 4361
(_tmp7BD == 0  && !_tmp7BE) && fmt_arg_start != 0) || 
(_tmp7BD != 0  || _tmp7BE) && fmt_arg_start != _tmp722 + 1)
# 4364
({void*_tmp723=0U;({unsigned int _tmpBF4=loc;struct _dyneither_ptr _tmpBF3=({const char*_tmp724="bad format descriptor";_tag_dyneither(_tmp724,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpBF4,_tmpBF3,_tag_dyneither(_tmp723,sizeof(void*),0U));});});else{
# 4367
struct _tuple10 _tmp725=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7BF,fmt_desc_arg - 1);struct _tuple10 _tmp726=_tmp725;void*_tmp73B;_LL57: _tmp73B=_tmp726.f3;_LL58:;
# 4369
{void*_tmp727=Cyc_Tcutil_compress(_tmp73B);void*_tmp728=_tmp727;void*_tmp734;void*_tmp733;void*_tmp732;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp728)->tag == 3U){_LL5A: _tmp734=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp728)->f1).elt_type;_tmp733=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp728)->f1).ptr_atts).bounds;_tmp732=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp728)->f1).ptr_atts).zero_term;_LL5B:
# 4372
 if(!Cyc_Tcutil_is_char_type(_tmp734))
({void*_tmp729=0U;({unsigned int _tmpBF6=loc;struct _dyneither_ptr _tmpBF5=({const char*_tmp72A="format descriptor is not a string";_tag_dyneither(_tmp72A,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpBF6,_tmpBF5,_tag_dyneither(_tmp729,sizeof(void*),0U));});});else{
# 4375
struct Cyc_Absyn_Exp*_tmp72B=({void*_tmpBF7=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBF7,_tmp733);});
if(_tmp72B == 0  && _tmp7BE)
({void*_tmp72C=0U;({unsigned int _tmpBF9=loc;struct _dyneither_ptr _tmpBF8=({const char*_tmp72D="format descriptor is not a char * type";_tag_dyneither(_tmp72D,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpBF9,_tmpBF8,_tag_dyneither(_tmp72C,sizeof(void*),0U));});});else{
if(_tmp72B != 0  && !_tmp7BE)
({void*_tmp72E=0U;({unsigned int _tmpBFB=loc;struct _dyneither_ptr _tmpBFA=({const char*_tmp72F="format descriptor is not a char ? type";_tag_dyneither(_tmp72F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpBFB,_tmpBFA,_tag_dyneither(_tmp72E,sizeof(void*),0U));});});}}
# 4381
goto _LL59;}else{_LL5C: _LL5D:
({void*_tmp730=0U;({unsigned int _tmpBFD=loc;struct _dyneither_ptr _tmpBFC=({const char*_tmp731="format descriptor is not a string type";_tag_dyneither(_tmp731,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpBFD,_tmpBFC,_tag_dyneither(_tmp730,sizeof(void*),0U));});});goto _LL59;}_LL59:;}
# 4384
if(fmt_arg_start != 0  && !_tmp7BE){
# 4388
int problem;
{struct _tuple32 _tmp735=({struct _tuple32 _tmp9CD;_tmp9CD.f1=ft,({void*_tmpBFE=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7BD))->type));_tmp9CD.f2=_tmpBFE;});_tmp9CD;});struct _tuple32 _tmp736=_tmp735;struct Cyc_Absyn_Datatypedecl*_tmp738;struct Cyc_Absyn_Datatypedecl*_tmp737;switch(_tmp736.f1){case Cyc_Absyn_Printf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp736.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp736.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp736.f2)->f1)->f1).KnownDatatype).tag == 2){_LL5F: _tmp737=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp736.f2)->f1)->f1).KnownDatatype).val;_LL60:
# 4391
 problem=({struct _tuple2*_tmpBFF=_tmp737->name;Cyc_Absyn_qvar_cmp(_tmpBFF,Cyc_Absyn_datatype_print_arg_qvar());})!= 0;
goto _LL5E;}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}case Cyc_Absyn_Scanf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp736.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp736.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp736.f2)->f1)->f1).KnownDatatype).tag == 2){_LL61: _tmp738=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp736.f2)->f1)->f1).KnownDatatype).val;_LL62:
# 4394
 problem=({struct _tuple2*_tmpC00=_tmp738->name;Cyc_Absyn_qvar_cmp(_tmpC00,Cyc_Absyn_datatype_scanf_arg_qvar());})!= 0;
goto _LL5E;}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}default: _LL63: _LL64:
# 4397
 problem=1;
goto _LL5E;}_LL5E:;}
# 4400
if(problem)
({void*_tmp739=0U;({unsigned int _tmpC02=loc;struct _dyneither_ptr _tmpC01=({const char*_tmp73A="format attribute and vararg types don't match";_tag_dyneither(_tmp73A,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpC02,_tmpC01,_tag_dyneither(_tmp739,sizeof(void*),0U));});});}}}
# 4408
{struct Cyc_List_List*rpo=_tmp7BC;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp73C=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp73D=_tmp73C;void*_tmp73F;void*_tmp73E;_LL66: _tmp73F=_tmp73D->f1;_tmp73E=_tmp73D->f2;_LL67:;
_tmp708=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp708,& Cyc_Tcutil_ek,_tmp73F,1,1);
_tmp708=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp708,& Cyc_Tcutil_trk,_tmp73E,1,1);}}{
# 4416
struct Cyc_Tcenv_Fenv*_tmp740=Cyc_Tcenv_bogus_fenv(_tmp7C0,_tmp7BF);
struct Cyc_Tcenv_Tenv*_tmp741=({struct Cyc_Tcenv_Tenv*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F->ns=te->ns,_tmp77F->ae=te->ae,_tmp77F->le=_tmp740,_tmp77F->allow_valueof=1,_tmp77F->in_extern_c_include=te->in_extern_c_include,_tmp77F->in_tempest=te->in_tempest,_tmp77F->tempest_generalize=te->tempest_generalize;_tmp77F;});
struct _tuple31 _tmp742=({unsigned int _tmpC06=loc;struct Cyc_Tcenv_Tenv*_tmpC05=_tmp741;struct Cyc_Tcutil_CVTEnv _tmpC04=_tmp708;struct _dyneither_ptr _tmpC03=({const char*_tmp77E="@requires";_tag_dyneither(_tmp77E,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpC06,_tmpC05,_tmpC04,_tmpC03,_tmp7BA);});struct _tuple31 _tmp743=_tmp742;struct Cyc_Tcutil_CVTEnv _tmp77D;struct Cyc_List_List*_tmp77C;_LL69: _tmp77D=_tmp743.f1;_tmp77C=_tmp743.f2;_LL6A:;
_tmp708=_tmp77D;
*_tmp7B9=_tmp77C;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7BF,_tmp77C);{
# 4429
struct _tuple31 _tmp744=({unsigned int _tmpC0A=loc;struct Cyc_Tcenv_Tenv*_tmpC09=_tmp741;struct Cyc_Tcutil_CVTEnv _tmpC08=_tmp708;struct _dyneither_ptr _tmpC07=({const char*_tmp77B="@ensures";_tag_dyneither(_tmp77B,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpC0A,_tmpC09,_tmpC08,_tmpC07,_tmp7B8);});struct _tuple31 _tmp745=_tmp744;struct Cyc_Tcutil_CVTEnv _tmp77A;struct Cyc_List_List*_tmp779;_LL6C: _tmp77A=_tmp745.f1;_tmp779=_tmp745.f2;_LL6D:;
_tmp708=_tmp77A;
*_tmp7B7=_tmp779;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7BF,_tmp779);
# 4434
if(*_tmp7C2 != 0)
_tmp708=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp708,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp7C2),1,1);else{
# 4437
struct Cyc_List_List*effect=0;
# 4442
{struct Cyc_List_List*tvs=_tmp708.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple29 _tmp746=*((struct _tuple29*)tvs->hd);struct _tuple29 _tmp747=_tmp746;struct Cyc_Absyn_Tvar*_tmp755;int _tmp754;_LL6F: _tmp755=_tmp747.f1;_tmp754=_tmp747.f2;_LL70:;
if(!_tmp754)continue;{
void*_tmp748=Cyc_Absyn_compress_kb(_tmp755->kind);void*_tmp749=_tmp748;struct Cyc_Core_Opt**_tmp753;struct Cyc_Absyn_Kind*_tmp752;struct Cyc_Core_Opt**_tmp751;struct Cyc_Core_Opt**_tmp750;struct Cyc_Absyn_Kind*_tmp74F;switch(*((int*)_tmp749)){case 2U: _LL72: _tmp750=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp749)->f1;_tmp74F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp749)->f2;if((int)_tmp74F->kind == (int)Cyc_Absyn_RgnKind){_LL73:
# 4448
 if((int)_tmp74F->aliasqual == (int)Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpC0B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp750=_tmpC0B;});_tmp752=_tmp74F;goto _LL75;}
# 4451
({struct Cyc_Core_Opt*_tmpC0C=Cyc_Tcutil_kind_to_bound_opt(_tmp74F);*_tmp750=_tmpC0C;});_tmp752=_tmp74F;goto _LL75;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp749)->f2)->kind){case Cyc_Absyn_BoolKind: _LL76: _LL77:
# 4454
 goto _LL79;case Cyc_Absyn_PtrBndKind: _LL78: _LL79:
 goto _LL7B;case Cyc_Absyn_IntKind: _LL7A: _LL7B:
 goto _LL7D;case Cyc_Absyn_EffKind: _LL82: _tmp751=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp749)->f1;_LL83:
# 4461
({struct Cyc_Core_Opt*_tmpC0D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp751=_tmpC0D;});goto _LL85;default: goto _LL88;}}case 0U: _LL74: _tmp752=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp749)->f1;if((int)_tmp752->kind == (int)Cyc_Absyn_RgnKind){_LL75:
# 4453
 effect=({struct Cyc_List_List*_tmp74A=_cycalloc(sizeof(*_tmp74A));({void*_tmpC0E=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(_tmp755));_tmp74A->hd=_tmpC0E;}),_tmp74A->tl=effect;_tmp74A;});goto _LL71;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp749)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7C: _LL7D:
# 4457
 goto _LL7F;case Cyc_Absyn_PtrBndKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_IntKind: _LL80: _LL81:
 goto _LL71;case Cyc_Absyn_EffKind: _LL84: _LL85:
# 4463
 effect=({struct Cyc_List_List*_tmp74B=_cycalloc(sizeof(*_tmp74B));({void*_tmpC0F=Cyc_Absyn_var_type(_tmp755);_tmp74B->hd=_tmpC0F;}),_tmp74B->tl=effect;_tmp74B;});goto _LL71;default: _LL88: _LL89:
# 4468
 effect=({struct Cyc_List_List*_tmp74E=_cycalloc(sizeof(*_tmp74E));({void*_tmpC10=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(_tmp755));_tmp74E->hd=_tmpC10;}),_tmp74E->tl=effect;_tmp74E;});goto _LL71;}}default: _LL86: _tmp753=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp749)->f1;_LL87:
# 4465
({struct Cyc_Core_Opt*_tmpC12=({struct Cyc_Core_Opt*_tmp74D=_cycalloc(sizeof(*_tmp74D));({void*_tmpC11=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C->tag=2U,_tmp74C->f1=0,_tmp74C->f2=& Cyc_Tcutil_ak;_tmp74C;});_tmp74D->v=_tmpC11;});_tmp74D;});*_tmp753=_tmpC12;});goto _LL89;}_LL71:;};}}
# 4472
{struct Cyc_List_List*ts=_tmp708.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple30 _tmp756=*((struct _tuple30*)ts->hd);struct _tuple30 _tmp757=_tmp756;void*_tmp75E;int _tmp75D;_LL8B: _tmp75E=_tmp757.f1;_tmp75D=_tmp757.f2;_LL8C:;
if(!_tmp75D)continue;{
struct Cyc_Absyn_Kind*_tmp758=Cyc_Tcutil_type_kind(_tmp75E);struct Cyc_Absyn_Kind*_tmp759=_tmp758;switch(((struct Cyc_Absyn_Kind*)_tmp759)->kind){case Cyc_Absyn_RgnKind: _LL8E: _LL8F:
# 4477
 effect=({struct Cyc_List_List*_tmp75A=_cycalloc(sizeof(*_tmp75A));({void*_tmpC13=Cyc_Absyn_access_eff(_tmp75E);_tmp75A->hd=_tmpC13;}),_tmp75A->tl=effect;_tmp75A;});goto _LL8D;case Cyc_Absyn_EffKind: _LL90: _LL91:
# 4479
 effect=({struct Cyc_List_List*_tmp75B=_cycalloc(sizeof(*_tmp75B));_tmp75B->hd=_tmp75E,_tmp75B->tl=effect;_tmp75B;});goto _LL8D;case Cyc_Absyn_IntKind: _LL92: _LL93:
 goto _LL8D;default: _LL94: _LL95:
# 4482
 effect=({struct Cyc_List_List*_tmp75C=_cycalloc(sizeof(*_tmp75C));({void*_tmpC14=Cyc_Absyn_regionsof_eff(_tmp75E);_tmp75C->hd=_tmpC14;}),_tmp75C->tl=effect;_tmp75C;});goto _LL8D;}_LL8D:;};}}
# 4485
({void*_tmpC15=Cyc_Absyn_join_eff(effect);*_tmp7C2=_tmpC15;});}
# 4491
if(*_tmp7C3 != 0){
struct Cyc_List_List*bs=*_tmp7C3;for(0;bs != 0;bs=bs->tl){
void*_tmp75F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp760=_tmp75F;struct Cyc_Core_Opt**_tmp770;struct Cyc_Absyn_Kind*_tmp76F;struct Cyc_Core_Opt**_tmp76E;struct Cyc_Core_Opt**_tmp76D;struct Cyc_Core_Opt**_tmp76C;struct Cyc_Core_Opt**_tmp76B;struct Cyc_Core_Opt**_tmp76A;struct Cyc_Core_Opt**_tmp769;struct Cyc_Core_Opt**_tmp768;struct Cyc_Core_Opt**_tmp767;struct Cyc_Core_Opt**_tmp766;switch(*((int*)_tmp760)){case 1U: _LL97: _tmp766=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp760)->f1;_LL98:
# 4495
({struct Cyc_String_pa_PrintArg_struct _tmp763=({struct Cyc_String_pa_PrintArg_struct _tmp9CE;_tmp9CE.tag=0U,_tmp9CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);_tmp9CE;});void*_tmp761[1U];_tmp761[0]=& _tmp763;({unsigned int _tmpC17=loc;struct _dyneither_ptr _tmpC16=({const char*_tmp762="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp762,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpC17,_tmpC16,_tag_dyneither(_tmp761,sizeof(void*),1U));});});
# 4497
_tmp767=_tmp766;goto _LL9A;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f2)->aliasqual == Cyc_Absyn_Top){_LL99: _tmp767=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f1;_LL9A:
 _tmp768=_tmp767;goto _LL9C;}else{goto _LLA9;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9B: _tmp768=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f1;_LL9C:
 _tmp769=_tmp768;goto _LL9E;case Cyc_Absyn_Aliasable: _LL9D: _tmp769=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f1;_LL9E:
 _tmp76B=_tmp769;goto _LLA0;case Cyc_Absyn_Unique: _LLA3: _tmp76A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f1;_LLA4:
# 4504
 _tmp76D=_tmp76A;goto _LLA6;default: goto _LLA9;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9F: _tmp76B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f1;_LLA0:
# 4501
 _tmp76C=_tmp76B;goto _LLA2;case Cyc_Absyn_Aliasable: _LLA1: _tmp76C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f1;_LLA2:
# 4503
({struct Cyc_Core_Opt*_tmpC18=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp76C=_tmpC18;});goto _LL96;case Cyc_Absyn_Unique: _LLA5: _tmp76D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f1;_LLA6:
# 4506
({struct Cyc_Core_Opt*_tmpC19=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp76D=_tmpC19;});goto _LL96;default: goto _LLA9;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f2)->aliasqual == Cyc_Absyn_Top){_LLA7: _tmp76E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f1;_LLA8:
# 4508
({struct Cyc_Core_Opt*_tmpC1A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp76E=_tmpC1A;});goto _LL96;}else{goto _LLA9;}default: _LLA9: _tmp770=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f1;_tmp76F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760)->f2;_LLAA:
# 4510
({struct Cyc_Core_Opt*_tmpC1B=Cyc_Tcutil_kind_to_bound_opt(_tmp76F);*_tmp770=_tmpC1B;});goto _LL96;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760)->f1)->kind == Cyc_Absyn_MemKind){_LLAB: _LLAC:
# 4512
({void*_tmp764=0U;({unsigned int _tmpC1D=loc;struct _dyneither_ptr _tmpC1C=({const char*_tmp765="functions can't abstract M types";_tag_dyneither(_tmp765,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpC1D,_tmpC1C,_tag_dyneither(_tmp764,sizeof(void*),0U));});});goto _LL96;}else{_LLAD: _LLAE:
 goto _LL96;}}_LL96:;}}
# 4517
({struct Cyc_List_List*_tmpC1E=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp708.kind_env,*_tmp7C3);cvtenv.kind_env=_tmpC1E;});
({struct Cyc_List_List*_tmpC1F=Cyc_Tcutil_remove_bound_tvars_bool(_tmp708.r,_tmp708.free_vars,*_tmp7C3);_tmp708.free_vars=_tmpC1F;});
# 4520
{struct Cyc_List_List*tvs=_tmp708.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple29 _tmp771=*((struct _tuple29*)tvs->hd);struct _tuple29 _tmp772=_tmp771;struct Cyc_Absyn_Tvar*_tmp774;int _tmp773;_LLB0: _tmp774=_tmp772.f1;_tmp773=_tmp772.f2;_LLB1:;
({struct Cyc_List_List*_tmpC20=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp774,_tmp773);cvtenv.free_vars=_tmpC20;});}}
# 4525
{struct Cyc_List_List*evs=_tmp708.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple30 _tmp775=*((struct _tuple30*)evs->hd);struct _tuple30 _tmp776=_tmp775;void*_tmp778;int _tmp777;_LLB3: _tmp778=_tmp776.f1;_tmp777=_tmp776.f2;_LLB4:;
({struct Cyc_List_List*_tmpC21=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp778,_tmp777);cvtenv.free_evars=_tmpC21;});}}
# 4529
goto _LL0;};};};}case 6U: _LL13: _tmp7C4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6C6)->f1;_LL14:
# 4532
 for(0;_tmp7C4 != 0;_tmp7C4=_tmp7C4->tl){
struct _tuple12*_tmp780=(struct _tuple12*)_tmp7C4->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp780).f2,1,0);
({int _tmpC22=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp780).f1).print_const,(*_tmp780).f2);
# 4535
((*_tmp780).f1).real_const=_tmpC22;});}
# 4538
goto _LL0;case 7U: _LL15: _tmp7C6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6C6)->f1;_tmp7C5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6C6)->f2;_LL16: {
# 4542
struct Cyc_List_List*prev_fields=0;
for(0;_tmp7C5 != 0;_tmp7C5=_tmp7C5->tl){
struct Cyc_Absyn_Aggrfield*_tmp781=(struct Cyc_Absyn_Aggrfield*)_tmp7C5->hd;struct Cyc_Absyn_Aggrfield*_tmp782=_tmp781;struct _dyneither_ptr*_tmp792;struct Cyc_Absyn_Tqual*_tmp791;void*_tmp790;struct Cyc_Absyn_Exp*_tmp78F;struct Cyc_List_List*_tmp78E;struct Cyc_Absyn_Exp*_tmp78D;_LLB6: _tmp792=_tmp782->name;_tmp791=(struct Cyc_Absyn_Tqual*)& _tmp782->tq;_tmp790=_tmp782->type;_tmp78F=_tmp782->width;_tmp78E=_tmp782->attributes;_tmp78D=_tmp782->requires_clause;_LLB7:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp792))
({struct Cyc_String_pa_PrintArg_struct _tmp785=({struct Cyc_String_pa_PrintArg_struct _tmp9D0;_tmp9D0.tag=0U,_tmp9D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp792);_tmp9D0;});void*_tmp783[1U];_tmp783[0]=& _tmp785;({unsigned int _tmpC24=loc;struct _dyneither_ptr _tmpC23=({const char*_tmp784="duplicate field %s";_tag_dyneither(_tmp784,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpC24,_tmpC23,_tag_dyneither(_tmp783,sizeof(void*),1U));});});
if(({struct _dyneither_ptr _tmpC25=(struct _dyneither_ptr)*_tmp792;Cyc_strcmp(_tmpC25,({const char*_tmp786="";_tag_dyneither(_tmp786,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp787=_cycalloc(sizeof(*_tmp787));_tmp787->hd=_tmp792,_tmp787->tl=prev_fields;_tmp787;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp790,1,0);
({int _tmpC26=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp791->print_const,_tmp790);_tmp791->real_const=_tmpC26;});
Cyc_Tcutil_check_bitfield(loc,te,_tmp790,_tmp78F,_tmp792);
Cyc_Tcutil_check_field_atts(loc,_tmp792,_tmp78E);
if(_tmp78D != 0){
# 4555
if((int)_tmp7C6 != (int)1U)
({void*_tmp788=0U;({unsigned int _tmpC28=loc;struct _dyneither_ptr _tmpC27=({const char*_tmp789="@requires clause is only allowed on union members";_tag_dyneither(_tmp789,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpC28,_tmpC27,_tag_dyneither(_tmp788,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpC29=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpC29,0,_tmp78D);});
if(!Cyc_Tcutil_is_integral(_tmp78D))
({struct Cyc_String_pa_PrintArg_struct _tmp78C=({struct Cyc_String_pa_PrintArg_struct _tmp9D1;_tmp9D1.tag=0U,({
struct _dyneither_ptr _tmpC2A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp78D->topt)));_tmp9D1.f1=_tmpC2A;});_tmp9D1;});void*_tmp78A[1U];_tmp78A[0]=& _tmp78C;({unsigned int _tmpC2C=loc;struct _dyneither_ptr _tmpC2B=({const char*_tmp78B="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp78B,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpC2C,_tmpC2B,_tag_dyneither(_tmp78A,sizeof(void*),1U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp78D,te,cvtenv);}}
# 4564
goto _LL0;}default: _LL17: _tmp7CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6C6)->f1;_tmp7C9=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6C6)->f2;_tmp7C8=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6C6)->f3;_tmp7C7=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6C6)->f4;_LL18: {
# 4567
struct Cyc_List_List*targs=*_tmp7C9;
# 4569
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp793;_push_handler(& _tmp793);{int _tmp795=0;if(setjmp(_tmp793.handler))_tmp795=1;if(!_tmp795){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7CA);;_pop_handler();}else{void*_tmp794=(void*)Cyc_Core_get_exn_thrown();void*_tmp796=_tmp794;void*_tmp79A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp796)->tag == Cyc_Dict_Absent){_LLB9: _LLBA:
# 4572
({struct Cyc_String_pa_PrintArg_struct _tmp799=({struct Cyc_String_pa_PrintArg_struct _tmp9D2;_tmp9D2.tag=0U,({struct _dyneither_ptr _tmpC2D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7CA));_tmp9D2.f1=_tmpC2D;});_tmp9D2;});void*_tmp797[1U];_tmp797[0]=& _tmp799;({unsigned int _tmpC2F=loc;struct _dyneither_ptr _tmpC2E=({const char*_tmp798="unbound typedef name %s";_tag_dyneither(_tmp798,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpC2F,_tmpC2E,_tag_dyneither(_tmp797,sizeof(void*),1U));});});
return cvtenv;}else{_LLBB: _tmp79A=_tmp796;_LLBC:(int)_rethrow(_tmp79A);}_LLB8:;}};}
# 4575
*_tmp7C8=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4580
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4585
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp79C=_cycalloc(sizeof(*_tmp79C));({struct _tuple15*_tmpC30=({struct _tuple15*_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp79B->f2=(void*)ts->hd;_tmp79B;});_tmp79C->hd=_tmpC30;}),_tmp79C->tl=inst;_tmp79C;});}
# 4589
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp79F=({struct Cyc_String_pa_PrintArg_struct _tmp9D3;_tmp9D3.tag=0U,({struct _dyneither_ptr _tmpC31=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7CA));_tmp9D3.f1=_tmpC31;});_tmp9D3;});void*_tmp79D[1U];_tmp79D[0]=& _tmp79F;({unsigned int _tmpC33=loc;struct _dyneither_ptr _tmpC32=({const char*_tmp79E="too many parameters for typedef %s";_tag_dyneither(_tmp79E,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpC33,_tmpC32,_tag_dyneither(_tmp79D,sizeof(void*),1U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4594
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0->hd=e,_tmp7A0->tl=hidden_ts;_tmp7A0;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));({struct _tuple15*_tmpC34=({struct _tuple15*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));_tmp7A1->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp7A1->f2=e;_tmp7A1;});_tmp7A2->hd=_tmpC34;}),_tmp7A2->tl=inst;_tmp7A2;});}
# 4602
({struct Cyc_List_List*_tmpC36=({struct Cyc_List_List*_tmpC35=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC35,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp7C9=_tmpC36;});}
# 4604
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp7C7=new_typ;}
# 4610
goto _LL0;};}}_LL0:;}
# 4612
if(!({struct Cyc_Absyn_Kind*_tmpC37=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmpC37,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp7CD=({struct Cyc_String_pa_PrintArg_struct _tmp9D6;_tmp9D6.tag=0U,({
struct _dyneither_ptr _tmpC38=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9D6.f1=_tmpC38;});_tmp9D6;});struct Cyc_String_pa_PrintArg_struct _tmp7CE=({struct Cyc_String_pa_PrintArg_struct _tmp9D5;_tmp9D5.tag=0U,({struct _dyneither_ptr _tmpC39=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_type_kind(t)));_tmp9D5.f1=_tmpC39;});_tmp9D5;});struct Cyc_String_pa_PrintArg_struct _tmp7CF=({struct Cyc_String_pa_PrintArg_struct _tmp9D4;_tmp9D4.tag=0U,({struct _dyneither_ptr _tmpC3A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(expected_kind));_tmp9D4.f1=_tmpC3A;});_tmp9D4;});void*_tmp7CB[3U];_tmp7CB[0]=& _tmp7CD,_tmp7CB[1]=& _tmp7CE,_tmp7CB[2]=& _tmp7CF;({unsigned int _tmpC3C=loc;struct _dyneither_ptr _tmpC3B=({const char*_tmp7CC="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp7CC,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpC3C,_tmpC3B,_tag_dyneither(_tmp7CB,sizeof(void*),3U));});});
# 4616
return cvtenv;}
# 4624
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4626
{void*_tmp7D0=e->r;void*_tmp7D1=_tmp7D0;struct Cyc_Absyn_Exp*_tmp7E4;struct Cyc_Absyn_Exp*_tmp7E3;void*_tmp7E2;void*_tmp7E1;void*_tmp7E0;void*_tmp7DF;struct Cyc_Absyn_Exp*_tmp7DE;struct Cyc_Absyn_Exp*_tmp7DD;struct Cyc_Absyn_Exp*_tmp7DC;struct Cyc_Absyn_Exp*_tmp7DB;struct Cyc_Absyn_Exp*_tmp7DA;struct Cyc_Absyn_Exp*_tmp7D9;struct Cyc_Absyn_Exp*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7D7;struct Cyc_Absyn_Exp*_tmp7D6;struct Cyc_Absyn_Exp*_tmp7D5;struct Cyc_List_List*_tmp7D4;switch(*((int*)_tmp7D1)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp7D4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7D1)->f2;_LLC:
# 4633
 for(0;_tmp7D4 != 0;_tmp7D4=_tmp7D4->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp7D4->hd,te,cvtenv);}
goto _LL0;case 6U: _LLD: _tmp7D7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_tmp7D6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7D1)->f2;_tmp7D5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7D1)->f3;_LLE:
# 4637
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7D7,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7D6,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7D5,te,cvtenv);
goto _LL0;case 7U: _LLF: _tmp7D9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_tmp7D8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7D1)->f2;_LL10:
 _tmp7DB=_tmp7D9;_tmp7DA=_tmp7D8;goto _LL12;case 8U: _LL11: _tmp7DB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_tmp7DA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7D1)->f2;_LL12:
 _tmp7DD=_tmp7DB;_tmp7DC=_tmp7DA;goto _LL14;case 9U: _LL13: _tmp7DD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_tmp7DC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7D1)->f2;_LL14:
# 4644
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7DD,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7DC,te,cvtenv);
goto _LL0;case 14U: _LL15: _tmp7DF=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_tmp7DE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7D1)->f2;_LL16:
# 4648
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7DE,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7DF,1,0);
goto _LL0;case 19U: _LL17: _tmp7E0=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_LL18:
 _tmp7E1=_tmp7E0;goto _LL1A;case 17U: _LL19: _tmp7E1=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_LL1A:
# 4653
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7E1,1,0);
goto _LL0;case 39U: _LL1B: _tmp7E2=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_LL1C:
# 4656
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp7E2,1,0);
goto _LL0;case 18U: _LL1D: _tmp7E3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_LL1E:
# 4659
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7E3,te,cvtenv);
goto _LL0;case 41U: _LL1F: _tmp7E4=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp7D1)->f1;_LL20:
# 4662
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7E4,te,cvtenv);
goto _LL0;default: _LL21: _LL22:
# 4665
({void*_tmp7D2=0U;({struct _dyneither_ptr _tmpC3D=({const char*_tmp7D3="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp7D3,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC3D,_tag_dyneither(_tmp7D2,sizeof(void*),0U));});});}_LL0:;}
# 4667
return cvtenv;}
# 4670
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4675
struct Cyc_List_List*_tmp7E5=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4678
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple29 _tmp7E6=*((struct _tuple29*)vs->hd);struct _tuple29 _tmp7E7=_tmp7E6;struct Cyc_Absyn_Tvar*_tmp7E8;_LL1: _tmp7E8=_tmp7E7.f1;_LL2:;
({struct Cyc_List_List*_tmpC3E=Cyc_Tcutil_fast_add_free_tvar(_tmp7E5,_tmp7E8);cvt.kind_env=_tmpC3E;});}}
# 4686
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple30 _tmp7E9=*((struct _tuple30*)evs->hd);struct _tuple30 _tmp7EA=_tmp7E9;void*_tmp7F3;_LL4: _tmp7F3=_tmp7EA.f1;_LL5:;{
void*_tmp7EB=Cyc_Tcutil_compress(_tmp7F3);void*_tmp7EC=_tmp7EB;struct Cyc_Core_Opt**_tmp7F2;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7EC)->tag == 1U){_LL7: _tmp7F2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7EC)->f4;_LL8:
# 4690
 if(*_tmp7F2 == 0)
({struct Cyc_Core_Opt*_tmpC3F=({struct Cyc_Core_Opt*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED->v=_tmp7E5;_tmp7ED;});*_tmp7F2=_tmpC3F;});else{
# 4694
struct Cyc_List_List*_tmp7EE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp7F2))->v;
struct Cyc_List_List*_tmp7EF=0;
for(0;_tmp7EE != 0;_tmp7EE=_tmp7EE->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp7E5,(struct Cyc_Absyn_Tvar*)_tmp7EE->hd))
_tmp7EF=({struct Cyc_List_List*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0->hd=(struct Cyc_Absyn_Tvar*)_tmp7EE->hd,_tmp7F0->tl=_tmp7EF;_tmp7F0;});}
({struct Cyc_Core_Opt*_tmpC40=({struct Cyc_Core_Opt*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1->v=_tmp7EF;_tmp7F1;});*_tmp7F2=_tmpC40;});}
# 4701
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4705
return cvt;}
# 4712
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
{void*_tmp7F4=Cyc_Tcutil_compress(e);void*_tmp7F5=_tmp7F4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7F5)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
# 4718
 continue;}_LL0:;}{
# 4720
struct Cyc_Absyn_Kind*_tmp7F6=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_tmp7F7=_tmp7F6;switch(((struct Cyc_Absyn_Kind*)_tmp7F7)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp7F7)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
# 4722
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_unique_rgn_type))
({void*_tmp7F8=0U;({struct _dyneither_ptr _tmpC41=({const char*_tmp7F9="can't unify evar with unique region!";_tag_dyneither(_tmp7F9,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC41,_tag_dyneither(_tmp7F8,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Top: _LLA: _LLB:
# 4727
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_heap_rgn_type))({void*_tmp7FA=0U;({struct _dyneither_ptr _tmpC42=({const char*_tmp7FB="can't unify evar with heap!";_tag_dyneither(_tmp7FB,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC42,_tag_dyneither(_tmp7FA,sizeof(void*),0U));});});
goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 4730
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp7FC=0U;({struct _dyneither_ptr _tmpC43=({const char*_tmp7FD="can't unify evar with {}!";_tag_dyneither(_tmp7FD,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC43,_tag_dyneither(_tmp7FC,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
# 4733
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_false_type))({struct Cyc_String_pa_PrintArg_struct _tmp800=({struct Cyc_String_pa_PrintArg_struct _tmp9D7;_tmp9D7.tag=0U,({
struct _dyneither_ptr _tmpC44=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmp9D7.f1=_tmpC44;});_tmp9D7;});void*_tmp7FE[1U];_tmp7FE[0]=& _tmp800;({struct _dyneither_ptr _tmpC45=({const char*_tmp7FF="can't unify evar %s with @false!";_tag_dyneither(_tmp7FF,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC45,_tag_dyneither(_tmp7FE,sizeof(void*),1U));});});
goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
# 4737
 if(!({void*_tmpC46=e;Cyc_Tcutil_unify(_tmpC46,Cyc_Absyn_bounds_one());}))({void*_tmp801=0U;({struct _dyneither_ptr _tmpC47=({const char*_tmp802="can't unify evar with bounds_one!";_tag_dyneither(_tmp802,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC47,_tag_dyneither(_tmp801,sizeof(void*),0U));});});
goto _LL5;default: _LL12: _LL13:
# 4740
({struct Cyc_String_pa_PrintArg_struct _tmp805=({struct Cyc_String_pa_PrintArg_struct _tmp9D9;_tmp9D9.tag=0U,({
struct _dyneither_ptr _tmpC48=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmp9D9.f1=_tmpC48;});_tmp9D9;});struct Cyc_String_pa_PrintArg_struct _tmp806=({struct Cyc_String_pa_PrintArg_struct _tmp9D8;_tmp9D8.tag=0U,({struct _dyneither_ptr _tmpC49=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(in_t));_tmp9D8.f1=_tmpC49;});_tmp9D8;});void*_tmp803[2U];_tmp803[0]=& _tmp805,_tmp803[1]=& _tmp806;({unsigned int _tmpC4B=loc;struct _dyneither_ptr _tmpC4A=({const char*_tmp804="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp804,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpC4B,_tmpC4A,_tag_dyneither(_tmp803,sizeof(void*),2U));});});
goto _LL5;}_LL5:;};}}
# 4752
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
if(te->in_tempest  && !te->tempest_generalize)generalize_evars=0;{
struct Cyc_List_List*_tmp807=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp808=({unsigned int _tmpC4F=loc;struct Cyc_Tcenv_Tenv*_tmpC4E=te;struct Cyc_Tcutil_CVTEnv _tmpC4D=({struct Cyc_Tcutil_CVTEnv _tmp9DD;_tmp9DD.r=Cyc_Core_heap_region,_tmp9DD.kind_env=_tmp807,_tmp9DD.free_vars=0,_tmp9DD.free_evars=0,_tmp9DD.generalize_evars=generalize_evars,_tmp9DD.fn_result=0;_tmp9DD;});struct Cyc_Absyn_Kind*_tmpC4C=expected_kind;Cyc_Tcutil_check_valid_type(_tmpC4F,_tmpC4E,_tmpC4D,_tmpC4C,1,t);});
# 4761
struct Cyc_List_List*_tmp809=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,_tmp808.free_vars);
struct Cyc_List_List*_tmp80A=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple30*))Cyc_Core_fst,_tmp808.free_evars);
# 4765
if(_tmp807 != 0){
struct Cyc_List_List*_tmp80B=0;
{struct Cyc_List_List*_tmp80C=_tmp809;for(0;(unsigned int)_tmp80C;_tmp80C=_tmp80C->tl){
struct Cyc_Absyn_Tvar*_tmp80D=(struct Cyc_Absyn_Tvar*)_tmp80C->hd;
int found=0;
{struct Cyc_List_List*_tmp80E=_tmp807;for(0;(unsigned int)_tmp80E;_tmp80E=_tmp80E->tl){
if(Cyc_Absyn_tvar_cmp(_tmp80D,(struct Cyc_Absyn_Tvar*)_tmp80E->hd)== 0){found=1;break;}}}
if(!found)
_tmp80B=({struct Cyc_List_List*_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F->hd=(struct Cyc_Absyn_Tvar*)_tmp80C->hd,_tmp80F->tl=_tmp80B;_tmp80F;});}}
# 4775
_tmp809=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp80B);}
# 4781
{struct Cyc_List_List*x=_tmp809;for(0;x != 0;x=x->tl){
void*_tmp810=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp811=_tmp810;enum Cyc_Absyn_AliasQual _tmp81F;struct Cyc_Core_Opt**_tmp81E;struct Cyc_Absyn_Kind*_tmp81D;struct Cyc_Core_Opt**_tmp81C;struct Cyc_Core_Opt**_tmp81B;struct Cyc_Core_Opt**_tmp81A;struct Cyc_Core_Opt**_tmp819;struct Cyc_Core_Opt**_tmp818;struct Cyc_Core_Opt**_tmp817;switch(*((int*)_tmp811)){case 1U: _LL1: _tmp817=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp811)->f1;_LL2:
 _tmp818=_tmp817;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp818=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f1;_LL4:
 _tmp819=_tmp818;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp819=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f1;_LL6:
 _tmp81A=_tmp819;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp81A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f1;_LL8:
# 4787
({struct Cyc_Core_Opt*_tmpC50=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp81A=_tmpC50;});goto _LL0;case Cyc_Absyn_Unique: _LL9: _tmp81B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f1;_LLA:
# 4789
({struct Cyc_Core_Opt*_tmpC51=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp81B=_tmpC51;});goto _LL0;default: goto _LLD;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp81C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f1;_LLC:
# 4791
({struct Cyc_Core_Opt*_tmpC52=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp81C=_tmpC52;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp81E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f1;_tmp81D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp811)->f2;_LLE:
# 4793
({struct Cyc_Core_Opt*_tmpC53=Cyc_Tcutil_kind_to_bound_opt(_tmp81D);*_tmp81E=_tmpC53;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp811)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp81F=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp811)->f1)->aliasqual;_LL10:
# 4795
({struct Cyc_String_pa_PrintArg_struct _tmp814=({struct Cyc_String_pa_PrintArg_struct _tmp9DB;_tmp9DB.tag=0U,({
struct _dyneither_ptr _tmpC54=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmp9DB.f1=_tmpC54;});_tmp9DB;});struct Cyc_String_pa_PrintArg_struct _tmp815=({struct Cyc_String_pa_PrintArg_struct _tmp9DA;_tmp9DA.tag=0U,({struct _dyneither_ptr _tmpC55=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816->kind=Cyc_Absyn_MemKind,_tmp816->aliasqual=_tmp81F;_tmp816;})));_tmp9DA.f1=_tmpC55;});_tmp9DA;});void*_tmp812[2U];_tmp812[0]=& _tmp814,_tmp812[1]=& _tmp815;({unsigned int _tmpC57=loc;struct _dyneither_ptr _tmpC56=({const char*_tmp813="type variable %s cannot have kind %s";_tag_dyneither(_tmp813,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpC57,_tmpC56,_tag_dyneither(_tmp812,sizeof(void*),2U));});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4802
if(_tmp809 != 0  || _tmp80A != 0){
{void*_tmp820=Cyc_Tcutil_compress(t);void*_tmp821=_tmp820;struct Cyc_List_List**_tmp822;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp821)->tag == 5U){_LL14: _tmp822=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp821)->f1).tvars;_LL15:
# 4805
 if(*_tmp822 == 0){
# 4807
({struct Cyc_List_List*_tmpC58=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp809);*_tmp822=_tmpC58;});
_tmp809=0;}
# 4810
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4813
if(_tmp809 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp825=({struct Cyc_String_pa_PrintArg_struct _tmp9DC;_tmp9DC.tag=0U,_tmp9DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp809->hd)->name);_tmp9DC;});void*_tmp823[1U];_tmp823[0]=& _tmp825;({unsigned int _tmpC5A=loc;struct _dyneither_ptr _tmpC59=({const char*_tmp824="unbound type variable %s";_tag_dyneither(_tmp824,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpC5A,_tmpC59,_tag_dyneither(_tmp823,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_function_type(t) || !te->in_tempest)
Cyc_Tcutil_check_free_evars(_tmp80A,t,loc);}};}
# 4824
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 4827
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp826=Cyc_Tcutil_compress(t);void*_tmp827=_tmp826;struct Cyc_Absyn_FnInfo _tmp82B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp827)->tag == 5U){_LL1: _tmp82B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp827)->f1;_LL2: {
# 4830
struct Cyc_List_List*_tmp828=(fd->i).attributes;
fd->i=_tmp82B;
(fd->i).attributes=_tmp828;
({int _tmpC5B=
Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp82B.ret_tqual).print_const,_tmp82B.ret_type);
# 4833
((fd->i).ret_tqual).real_const=_tmpC5B;});
# 4835
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp829=0U;({struct _dyneither_ptr _tmpC5C=({const char*_tmp82A="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp82A,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5C,_tag_dyneither(_tmp829,sizeof(void*),0U));});});}_LL0:;}{
# 4838
struct _RegionHandle _tmp82C=_new_region("r");struct _RegionHandle*r=& _tmp82C;_push_region(r);
({struct Cyc_List_List*_tmpC5E=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,(fd->i).args);unsigned int _tmpC5D=loc;Cyc_Tcutil_check_unique_vars(_tmpC5E,_tmpC5D,({const char*_tmp82D="function declaration has repeated parameter";_tag_dyneither(_tmp82D,sizeof(char),44U);}));});
# 4842
fd->cached_type=t;
# 4839
;_pop_region(r);};}
# 4847
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4853
struct Cyc_Tcutil_CVTEnv _tmp82E=({unsigned int _tmpC63=loc;struct Cyc_Tcenv_Tenv*_tmpC62=te;struct Cyc_Tcutil_CVTEnv _tmpC61=({struct Cyc_Tcutil_CVTEnv _tmp9E0;_tmp9E0.r=Cyc_Core_heap_region,_tmp9E0.kind_env=bound_tvars,_tmp9E0.free_vars=0,_tmp9E0.free_evars=0,_tmp9E0.generalize_evars=0,_tmp9E0.fn_result=0;_tmp9E0;});struct Cyc_Absyn_Kind*_tmpC60=expected_kind;int _tmpC5F=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpC63,_tmpC62,_tmpC61,_tmpC60,_tmpC5F,t);});
# 4857
struct Cyc_List_List*_tmp82F=({struct _RegionHandle*_tmpC65=Cyc_Core_heap_region;struct Cyc_List_List*_tmpC64=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,_tmp82E.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpC65,_tmpC64,bound_tvars);});
# 4859
struct Cyc_List_List*_tmp830=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple30*))Cyc_Core_fst,_tmp82E.free_evars);
{struct Cyc_List_List*fs=_tmp82F;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp831=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp834=({struct Cyc_String_pa_PrintArg_struct _tmp9DF;_tmp9DF.tag=0U,_tmp9DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp831);_tmp9DF;});struct Cyc_String_pa_PrintArg_struct _tmp835=({struct Cyc_String_pa_PrintArg_struct _tmp9DE;_tmp9DE.tag=0U,({struct _dyneither_ptr _tmpC66=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9DE.f1=_tmpC66;});_tmp9DE;});void*_tmp832[2U];_tmp832[0]=& _tmp834,_tmp832[1]=& _tmp835;({unsigned int _tmpC68=loc;struct _dyneither_ptr _tmpC67=({const char*_tmp833="unbound type variable %s in type %s";_tag_dyneither(_tmp833,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpC68,_tmpC67,_tag_dyneither(_tmp832,sizeof(void*),2U));});});}}
# 4864
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp830,t,loc);}
# 4868
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpC69=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpC69;});}
# 4872
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4878
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4881
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp838=({struct Cyc_String_pa_PrintArg_struct _tmp9E2;_tmp9E2.tag=0U,_tmp9E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmp9E2;});struct Cyc_String_pa_PrintArg_struct _tmp839=({struct Cyc_String_pa_PrintArg_struct _tmp9E1;_tmp9E1.tag=0U,({struct _dyneither_ptr _tmpC6A=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmp9E1.f1=_tmpC6A;});_tmp9E1;});void*_tmp836[2U];_tmp836[0]=& _tmp838,_tmp836[1]=& _tmp839;({unsigned int _tmpC6C=loc;struct _dyneither_ptr _tmpC6B=({const char*_tmp837="%s: %s";_tag_dyneither(_tmp837,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpC6C,_tmpC6B,_tag_dyneither(_tmp836,sizeof(void*),2U));});});}}}
# 4887
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4891
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4895
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpC6E=tvs;unsigned int _tmpC6D=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpC6E,_tmpC6D,Cyc_Absynpp_tvar2string,({const char*_tmp83A="duplicate type variable";_tag_dyneither(_tmp83A,sizeof(char),24U);}));});}struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple34{struct Cyc_List_List*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4909 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4914
struct _RegionHandle _tmp83B=_new_region("temp");struct _RegionHandle*temp=& _tmp83B;_push_region(temp);
# 4918
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpC6F=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpC6F,({const char*_tmp83C="";_tag_dyneither(_tmp83C,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp83E=_region_malloc(temp,sizeof(*_tmp83E));({struct _tuple33*_tmpC70=({struct _tuple33*_tmp83D=_region_malloc(temp,sizeof(*_tmp83D));_tmp83D->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp83D->f2=0;_tmp83D;});_tmp83E->hd=_tmpC70;}),_tmp83E->tl=fields;_tmp83E;});}}
# 4923
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4925
struct _dyneither_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp863="struct";_tag_dyneither(_tmp863,sizeof(char),7U);}):({const char*_tmp864="union";_tag_dyneither(_tmp864,sizeof(char),6U);});
# 4928
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple34*_tmp83F=(struct _tuple34*)des->hd;struct _tuple34*_tmp840=_tmp83F;struct Cyc_List_List*_tmp85B;void*_tmp85A;_LL1: _tmp85B=_tmp840->f1;_tmp85A=_tmp840->f2;_LL2:;
if(_tmp85B == 0){
# 4933
struct Cyc_List_List*_tmp841=fields;
for(0;_tmp841 != 0;_tmp841=_tmp841->tl){
if(!(*((struct _tuple33*)_tmp841->hd)).f2){
(*((struct _tuple33*)_tmp841->hd)).f2=1;
({struct Cyc_List_List*_tmpC72=({struct Cyc_List_List*_tmp843=_cycalloc(sizeof(*_tmp843));({void*_tmpC71=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842->tag=1U,_tmp842->f1=((*((struct _tuple33*)_tmp841->hd)).f1)->name;_tmp842;});_tmp843->hd=_tmpC71;}),_tmp843->tl=0;_tmp843;});(*((struct _tuple34*)des->hd)).f1=_tmpC72;});
ans=({struct Cyc_List_List*_tmp845=_region_malloc(rgn,sizeof(*_tmp845));({struct _tuple35*_tmpC73=({struct _tuple35*_tmp844=_region_malloc(rgn,sizeof(*_tmp844));_tmp844->f1=(*((struct _tuple33*)_tmp841->hd)).f1,_tmp844->f2=_tmp85A;_tmp844;});_tmp845->hd=_tmpC73;}),_tmp845->tl=ans;_tmp845;});
break;}}
# 4941
if(_tmp841 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp848=({struct Cyc_String_pa_PrintArg_struct _tmp9E3;_tmp9E3.tag=0U,_tmp9E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp9E3;});void*_tmp846[1U];_tmp846[0]=& _tmp848;({unsigned int _tmpC75=loc;struct _dyneither_ptr _tmpC74=({const char*_tmp847="too many arguments to %s";_tag_dyneither(_tmp847,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpC75,_tmpC74,_tag_dyneither(_tmp846,sizeof(void*),1U));});});}else{
if(_tmp85B->tl != 0)
# 4945
({void*_tmp849=0U;({unsigned int _tmpC77=loc;struct _dyneither_ptr _tmpC76=({const char*_tmp84A="multiple designators are not yet supported";_tag_dyneither(_tmp84A,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpC77,_tmpC76,_tag_dyneither(_tmp849,sizeof(void*),0U));});});else{
# 4948
void*_tmp84B=(void*)_tmp85B->hd;void*_tmp84C=_tmp84B;struct _dyneither_ptr*_tmp859;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp84C)->tag == 0U){_LL4: _LL5:
# 4950
({struct Cyc_String_pa_PrintArg_struct _tmp84F=({struct Cyc_String_pa_PrintArg_struct _tmp9E4;_tmp9E4.tag=0U,_tmp9E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp9E4;});void*_tmp84D[1U];_tmp84D[0]=& _tmp84F;({unsigned int _tmpC79=loc;struct _dyneither_ptr _tmpC78=({const char*_tmp84E="array designator used in argument to %s";_tag_dyneither(_tmp84E,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC79,_tmpC78,_tag_dyneither(_tmp84D,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp859=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp84C)->f1;_LL7: {
# 4953
struct Cyc_List_List*_tmp850=fields;
for(0;_tmp850 != 0;_tmp850=_tmp850->tl){
if(Cyc_strptrcmp(_tmp859,((*((struct _tuple33*)_tmp850->hd)).f1)->name)== 0){
if((*((struct _tuple33*)_tmp850->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp853=({struct Cyc_String_pa_PrintArg_struct _tmp9E5;_tmp9E5.tag=0U,_tmp9E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp859);_tmp9E5;});void*_tmp851[1U];_tmp851[0]=& _tmp853;({unsigned int _tmpC7B=loc;struct _dyneither_ptr _tmpC7A=({const char*_tmp852="member %s has already been used as an argument";_tag_dyneither(_tmp852,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpC7B,_tmpC7A,_tag_dyneither(_tmp851,sizeof(void*),1U));});});
(*((struct _tuple33*)_tmp850->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp855=_region_malloc(rgn,sizeof(*_tmp855));({struct _tuple35*_tmpC7C=({struct _tuple35*_tmp854=_region_malloc(rgn,sizeof(*_tmp854));_tmp854->f1=(*((struct _tuple33*)_tmp850->hd)).f1,_tmp854->f2=_tmp85A;_tmp854;});_tmp855->hd=_tmpC7C;}),_tmp855->tl=ans;_tmp855;});
break;}}
# 4962
if(_tmp850 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp858=({struct Cyc_String_pa_PrintArg_struct _tmp9E6;_tmp9E6.tag=0U,_tmp9E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp859);_tmp9E6;});void*_tmp856[1U];_tmp856[0]=& _tmp858;({unsigned int _tmpC7E=loc;struct _dyneither_ptr _tmpC7D=({const char*_tmp857="bad field designator %s";_tag_dyneither(_tmp857,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpC7E,_tmpC7D,_tag_dyneither(_tmp856,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 4967
if((int)aggr_kind == (int)0U)
# 4969
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple33*)fields->hd)).f2){
({void*_tmp85C=0U;({unsigned int _tmpC80=loc;struct _dyneither_ptr _tmpC7F=({const char*_tmp85D="too few arguments to struct";_tag_dyneither(_tmp85D,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpC80,_tmpC7F,_tag_dyneither(_tmp85C,sizeof(void*),0U));});});
break;}}else{
# 4976
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple33*)fields->hd)).f2){
if(found)({void*_tmp85E=0U;({unsigned int _tmpC82=loc;struct _dyneither_ptr _tmpC81=({const char*_tmp85F="only one member of a union is allowed";_tag_dyneither(_tmp85F,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpC82,_tmpC81,_tag_dyneither(_tmp85E,sizeof(void*),0U));});});
found=1;}}
# 4983
if(!found)({void*_tmp860=0U;({unsigned int _tmpC84=loc;struct _dyneither_ptr _tmpC83=({const char*_tmp861="missing member for union";_tag_dyneither(_tmp861,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpC84,_tmpC83,_tag_dyneither(_tmp860,sizeof(void*),0U));});});}{
# 4986
struct Cyc_List_List*_tmp862=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp862;};};}
# 4918
;_pop_region(temp);}
# 4992
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4994
void*_tmp865=e1->r;void*_tmp866=_tmp865;struct Cyc_Absyn_Exp*_tmp878;struct Cyc_Absyn_Exp*_tmp877;struct Cyc_Absyn_Exp*_tmp876;struct Cyc_Absyn_Exp*_tmp875;struct Cyc_Absyn_Exp*_tmp874;struct Cyc_Absyn_Exp*_tmp873;switch(*((int*)_tmp866)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp869=({struct Cyc_String_pa_PrintArg_struct _tmp9E7;_tmp9E7.tag=0U,({struct _dyneither_ptr _tmpC85=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9E7.f1=_tmpC85;});_tmp9E7;});void*_tmp867[1U];_tmp867[0]=& _tmp869;({struct _dyneither_ptr _tmpC86=({const char*_tmp868="we have a cast in a lhs:  %s";_tag_dyneither(_tmp868,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC86,_tag_dyneither(_tmp867,sizeof(void*),1U));});});case 20U: _LL3: _tmp873=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp866)->f1;_LL4:
 _tmp874=_tmp873;goto _LL6;case 23U: _LL5: _tmp874=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp866)->f1;_LL6:
# 4998
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp874->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp875=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp866)->f1;_LL8:
 _tmp876=_tmp875;goto _LLA;case 21U: _LL9: _tmp876=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp866)->f1;_LLA:
# 5002
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp876->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp86C=({struct Cyc_String_pa_PrintArg_struct _tmp9E8;_tmp9E8.tag=0U,({
struct _dyneither_ptr _tmpC87=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9E8.f1=_tmpC87;});_tmp9E8;});void*_tmp86A[1U];_tmp86A[0]=& _tmp86C;({struct _dyneither_ptr _tmpC88=({const char*_tmp86B="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp86B,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC88,_tag_dyneither(_tmp86A,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp877=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp866)->f1;_LLC:
 _tmp878=_tmp877;goto _LLE;case 12U: _LLD: _tmp878=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp866)->f1;_LLE:
# 5008
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp878->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp86F=({struct Cyc_String_pa_PrintArg_struct _tmp9E9;_tmp9E9.tag=0U,({
struct _dyneither_ptr _tmpC89=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9E9.f1=_tmpC89;});_tmp9E9;});void*_tmp86D[1U];_tmp86D[0]=& _tmp86F;({struct _dyneither_ptr _tmpC8A=({const char*_tmp86E="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp86E,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC8A,_tag_dyneither(_tmp86D,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp872=({struct Cyc_String_pa_PrintArg_struct _tmp9EA;_tmp9EA.tag=0U,({struct _dyneither_ptr _tmpC8B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9EA.f1=_tmpC8B;});_tmp9EA;});void*_tmp870[1U];_tmp870[0]=& _tmp872;({struct _dyneither_ptr _tmpC8C=({const char*_tmp871="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp871,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC8C,_tag_dyneither(_tmp870,sizeof(void*),1U));});});}_LL0:;}
# 5023
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5026
void*_tmp879=Cyc_Tcutil_compress(r);void*_tmp87A=_tmp879;struct Cyc_Absyn_Tvar*_tmp886;enum Cyc_Absyn_KindQual _tmp885;enum Cyc_Absyn_AliasQual _tmp884;switch(*((int*)_tmp87A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp87A)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp87A)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp87A)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp87A)->f4 == 0){_LL5: _tmp885=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp87A)->f3)->kind)->v)->kind;_tmp884=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp87A)->f3)->kind)->v)->aliasqual;_LL6:
# 5030
 return(int)_tmp885 == (int)3U  && ((int)_tmp884 == (int)1U  || (int)_tmp884 == (int)2U  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp886=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp87A)->f1;_LL8: {
# 5034
struct Cyc_Absyn_Kind*_tmp87B=Cyc_Tcutil_tvar_kind(_tmp886,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp87C=_tmp87B;enum Cyc_Absyn_KindQual _tmp883;enum Cyc_Absyn_AliasQual _tmp882;_LLC: _tmp883=_tmp87C->kind;_tmp882=_tmp87C->aliasqual;_LLD:;
if((int)_tmp883 == (int)3U  && ((int)_tmp882 == (int)1U  || (int)_tmp882 == (int)2U  && !must_be_unique)){
void*_tmp87D=Cyc_Absyn_compress_kb(_tmp886->kind);void*_tmp87E=_tmp87D;struct Cyc_Core_Opt**_tmp881;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp87E)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp87E)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp87E)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp881=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp87E)->f1;_LL10:
# 5038
({struct Cyc_Core_Opt*_tmpC8E=({struct Cyc_Core_Opt*_tmp880=_cycalloc(sizeof(*_tmp880));({void*_tmpC8D=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F->tag=2U,_tmp87F->f1=0,_tmp87F->f2=& Cyc_Tcutil_rk;_tmp87F;});_tmp880->v=_tmpC8D;});_tmp880;});*_tmp881=_tmpC8E;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5043
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5050
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp887=Cyc_Tcutil_compress(t);void*_tmp888=_tmp887;struct Cyc_Absyn_Tvar*_tmp896;void*_tmp895;switch(*((int*)_tmp888)){case 3U: _LL1: _tmp895=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp888)->f1).ptr_atts).rgn;_LL2:
# 5053
 return Cyc_Tcutil_is_noalias_region(_tmp895,must_be_unique);case 2U: _LL3: _tmp896=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp888)->f1;_LL4: {
# 5055
struct Cyc_Absyn_Kind*_tmp889=Cyc_Tcutil_tvar_kind(_tmp896,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp88A=_tmp889;enum Cyc_Absyn_KindQual _tmp894;enum Cyc_Absyn_AliasQual _tmp893;_LL8: _tmp894=_tmp88A->kind;_tmp893=_tmp88A->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp88B=_tmp894;switch(_tmp88B){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)_tmp893 == (int)1U  || (int)_tmp893 == (int)2U  && !must_be_unique){
void*_tmp88C=Cyc_Absyn_compress_kb(_tmp896->kind);void*_tmp88D=_tmp88C;struct Cyc_Core_Opt**_tmp892;enum Cyc_Absyn_KindQual _tmp891;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp88D)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp88D)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp892=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp88D)->f1;_tmp891=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp88D)->f2)->kind;_LL15:
# 5061
({struct Cyc_Core_Opt*_tmpC91=({struct Cyc_Core_Opt*_tmp890=_cycalloc(sizeof(*_tmp890));({void*_tmpC90=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp88F=_cycalloc(sizeof(*_tmp88F));_tmp88F->tag=2U,_tmp88F->f1=0,({struct Cyc_Absyn_Kind*_tmpC8F=({struct Cyc_Absyn_Kind*_tmp88E=_cycalloc(sizeof(*_tmp88E));_tmp88E->kind=_tmp891,_tmp88E->aliasqual=Cyc_Absyn_Aliasable;_tmp88E;});_tmp88F->f2=_tmpC8F;});_tmp88F;});_tmp890->v=_tmpC90;});_tmp890;});*_tmp892=_tmpC91;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5065
 return 1;}_LL13:;}
# 5068
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5071
 return 0;}_LL0:;}
# 5074
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp897=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp897,0))return 1;{
void*_tmp898=_tmp897;struct Cyc_List_List*_tmp8AC;union Cyc_Absyn_DatatypeFieldInfo _tmp8AB;struct Cyc_List_List*_tmp8AA;union Cyc_Absyn_DatatypeInfo _tmp8A9;struct Cyc_List_List*_tmp8A8;struct Cyc_Absyn_Aggrdecl**_tmp8A7;struct Cyc_List_List*_tmp8A6;struct Cyc_List_List*_tmp8A5;switch(*((int*)_tmp898)){case 6U: _LL1: _tmp8A5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp898)->f1;_LL2:
# 5079
 while(_tmp8A5 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp8A5->hd)).f2))return 1;
_tmp8A5=_tmp8A5->tl;}
# 5083
return 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp898)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp898)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp8A7=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp898)->f1)->f1).KnownAggr).val;_tmp8A6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp898)->f2;_LL4:
# 5085
 if((*_tmp8A7)->impl == 0)return 0;else{
# 5087
struct Cyc_List_List*_tmp899=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp8A7)->tvs,_tmp8A6);
struct Cyc_List_List*_tmp89A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp8A7)->impl))->fields;
void*t;
while(_tmp89A != 0){
t=_tmp899 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp89A->hd)->type: Cyc_Tcutil_substitute(_tmp899,((struct Cyc_Absyn_Aggrfield*)_tmp89A->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp89A=_tmp89A->tl;}
# 5095
return 0;}}else{_LL7: _LL8:
# 5105
 return 0;}case 18U: _LL9: _tmp8A9=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp898)->f1)->f1;_tmp8A8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp898)->f2;_LLA: {
# 5107
union Cyc_Absyn_DatatypeInfo _tmp89B=_tmp8A9;struct Cyc_List_List*_tmp89F;struct Cyc_Core_Opt*_tmp89E;struct _tuple2*_tmp89D;int _tmp89C;if((_tmp89B.UnknownDatatype).tag == 1){_LL10: _tmp89D=((_tmp89B.UnknownDatatype).val).name;_tmp89C=((_tmp89B.UnknownDatatype).val).is_extensible;_LL11:
# 5110
 return 0;}else{_LL12: _tmp89F=(*(_tmp89B.KnownDatatype).val)->tvs;_tmp89E=(*(_tmp89B.KnownDatatype).val)->fields;_LL13:
# 5113
 return 0;}_LLF:;}case 19U: _LLB: _tmp8AB=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp898)->f1)->f1;_tmp8AA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp898)->f2;_LLC: {
# 5116
union Cyc_Absyn_DatatypeFieldInfo _tmp8A0=_tmp8AB;struct Cyc_Absyn_Datatypedecl*_tmp8A4;struct Cyc_Absyn_Datatypefield*_tmp8A3;if((_tmp8A0.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5119
 return 0;}else{_LL17: _tmp8A4=((_tmp8A0.KnownDatatypefield).val).f1;_tmp8A3=((_tmp8A0.KnownDatatypefield).val).f2;_LL18: {
# 5121
struct Cyc_List_List*_tmp8A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp8A4->tvs,_tmp8AA);
struct Cyc_List_List*_tmp8A2=_tmp8A3->typs;
while(_tmp8A2 != 0){
_tmp897=_tmp8A1 == 0?(*((struct _tuple12*)_tmp8A2->hd)).f2: Cyc_Tcutil_substitute(_tmp8A1,(*((struct _tuple12*)_tmp8A2->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp897))return 1;
_tmp8A2=_tmp8A2->tl;}
# 5128
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp8AC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp898)->f2;_LL6:
# 5098
 while(_tmp8AC != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp8AC->hd)->type))return 1;
_tmp8AC=_tmp8AC->tl;}
# 5102
return 0;default: _LLD: _LLE:
# 5130
 return 0;}_LL0:;};}
# 5137
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp8AD=e->r;void*_tmp8AE=_tmp8AD;struct Cyc_Absyn_Stmt*_tmp8C4;struct Cyc_Absyn_Exp*_tmp8C3;struct Cyc_Absyn_Exp*_tmp8C2;struct Cyc_Absyn_Exp*_tmp8C1;struct Cyc_Absyn_Exp*_tmp8C0;struct Cyc_Absyn_Exp*_tmp8BF;struct Cyc_Absyn_Exp*_tmp8BE;struct Cyc_Absyn_Exp*_tmp8BD;struct _dyneither_ptr*_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BB;struct Cyc_Absyn_Exp*_tmp8BA;switch(*((int*)_tmp8AE)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8AE)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp8BA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8AE)->f1;_LL4:
 _tmp8BB=_tmp8BA;goto _LL6;case 20U: _LL5: _tmp8BB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8AE)->f1;_LL6:
# 5142
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp8BB->topt),1) && Cyc_Tcutil_is_noalias_path(_tmp8BB);case 21U: _LL7: _tmp8BD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8AE)->f1;_tmp8BC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8AE)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp8BD);case 23U: _LL9: _tmp8BF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8AE)->f1;_tmp8BE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8AE)->f2;_LLA: {
# 5145
void*_tmp8AF=Cyc_Tcutil_compress((void*)_check_null(_tmp8BF->topt));void*_tmp8B0=_tmp8AF;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8B0)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp8BF);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp8C1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8AE)->f2;_tmp8C0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8AE)->f3;_LLC:
# 5150
 return Cyc_Tcutil_is_noalias_path(_tmp8C1) && Cyc_Tcutil_is_noalias_path(_tmp8C0);case 9U: _LLD: _tmp8C2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8AE)->f2;_LLE:
 _tmp8C3=_tmp8C2;goto _LL10;case 14U: _LLF: _tmp8C3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8AE)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp8C3);case 37U: _LL11: _tmp8C4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8AE)->f1;_LL12:
# 5154
 while(1){
void*_tmp8B1=_tmp8C4->r;void*_tmp8B2=_tmp8B1;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_Absyn_Decl*_tmp8B8;struct Cyc_Absyn_Stmt*_tmp8B7;struct Cyc_Absyn_Stmt*_tmp8B6;struct Cyc_Absyn_Stmt*_tmp8B5;switch(*((int*)_tmp8B2)){case 2U: _LL1B: _tmp8B6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8B2)->f1;_tmp8B5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8B2)->f2;_LL1C:
 _tmp8C4=_tmp8B5;goto _LL1A;case 12U: _LL1D: _tmp8B8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8B2)->f1;_tmp8B7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8B2)->f2;_LL1E:
 _tmp8C4=_tmp8B7;goto _LL1A;case 1U: _LL1F: _tmp8B9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8B2)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp8B9);default: _LL21: _LL22:
({void*_tmp8B3=0U;({struct _dyneither_ptr _tmpC92=({const char*_tmp8B4="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmp8B4,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC92,_tag_dyneither(_tmp8B3,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5162
 return 1;}_LL0:;}
# 5179 "tcutil.cyc"
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5181
struct _tuple17 bogus_ans=({struct _tuple17 _tmp9F7;_tmp9F7.f1=0,_tmp9F7.f2=Cyc_Absyn_heap_rgn_type;_tmp9F7;});
void*_tmp8C5=e->r;void*_tmp8C6=_tmp8C5;struct Cyc_Absyn_Exp*_tmp8FF;struct Cyc_Absyn_Exp*_tmp8FE;struct Cyc_Absyn_Exp*_tmp8FD;struct Cyc_Absyn_Exp*_tmp8FC;struct _dyneither_ptr*_tmp8FB;int _tmp8FA;struct Cyc_Absyn_Exp*_tmp8F9;struct _dyneither_ptr*_tmp8F8;int _tmp8F7;void*_tmp8F6;switch(*((int*)_tmp8C6)){case 1U: _LL1: _tmp8F6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8C6)->f1;_LL2: {
# 5185
void*_tmp8C7=_tmp8F6;struct Cyc_Absyn_Vardecl*_tmp8CF;struct Cyc_Absyn_Vardecl*_tmp8CE;struct Cyc_Absyn_Vardecl*_tmp8CD;struct Cyc_Absyn_Vardecl*_tmp8CC;switch(*((int*)_tmp8C7)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp8CC=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp8C7)->f1;_LL13: {
# 5189
void*_tmp8C8=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8C9=_tmp8C8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8C9)->tag == 4U){_LL1B: _LL1C:
# 5191
 return({struct _tuple17 _tmp9EB;_tmp9EB.f1=1,_tmp9EB.f2=Cyc_Absyn_heap_rgn_type;_tmp9EB;});}else{_LL1D: _LL1E:
 return({struct _tuple17 _tmp9EC;_tmp9EC.f1=(_tmp8CC->tq).real_const,_tmp9EC.f2=Cyc_Absyn_heap_rgn_type;_tmp9EC;});}_LL1A:;}case 4U: _LL14: _tmp8CD=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp8C7)->f1;_LL15: {
# 5195
void*_tmp8CA=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8CB=_tmp8CA;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8CB)->tag == 4U){_LL20: _LL21:
 return({struct _tuple17 _tmp9ED;_tmp9ED.f1=1,_tmp9ED.f2=(void*)_check_null(_tmp8CD->rgn);_tmp9ED;});}else{_LL22: _LL23:
# 5198
 _tmp8CD->escapes=1;
return({struct _tuple17 _tmp9EE;_tmp9EE.f1=(_tmp8CD->tq).real_const,_tmp9EE.f2=(void*)_check_null(_tmp8CD->rgn);_tmp9EE;});}_LL1F:;}case 5U: _LL16: _tmp8CE=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp8C7)->f1;_LL17:
# 5201
 _tmp8CF=_tmp8CE;goto _LL19;default: _LL18: _tmp8CF=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp8C7)->f1;_LL19:
# 5203
 _tmp8CF->escapes=1;
return({struct _tuple17 _tmp9EF;_tmp9EF.f1=(_tmp8CF->tq).real_const,_tmp9EF.f2=(void*)_check_null(_tmp8CF->rgn);_tmp9EF;});}_LLD:;}case 21U: _LL3: _tmp8F9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8C6)->f1;_tmp8F8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8C6)->f2;_tmp8F7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8C6)->f3;_LL4:
# 5208
 if(_tmp8F7)return bogus_ans;{
# 5211
void*_tmp8D0=Cyc_Tcutil_compress((void*)_check_null(_tmp8F9->topt));void*_tmp8D1=_tmp8D0;struct Cyc_Absyn_Aggrdecl*_tmp8DD;struct Cyc_List_List*_tmp8DC;switch(*((int*)_tmp8D1)){case 7U: _LL25: _tmp8DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8D1)->f2;_LL26: {
# 5213
struct Cyc_Absyn_Aggrfield*_tmp8D2=Cyc_Absyn_lookup_field(_tmp8DC,_tmp8F8);
if(_tmp8D2 != 0  && _tmp8D2->width == 0){
struct _tuple17 _tmp8D3=Cyc_Tcutil_addressof_props(te,_tmp8F9);struct _tuple17 _tmp8D4=_tmp8D3;int _tmp8D6;void*_tmp8D5;_LL2C: _tmp8D6=_tmp8D4.f1;_tmp8D5=_tmp8D4.f2;_LL2D:;
return({struct _tuple17 _tmp9F0;_tmp9F0.f1=(_tmp8D2->tq).real_const  || _tmp8D6,_tmp9F0.f2=_tmp8D5;_tmp9F0;});}
# 5218
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8D1)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8D1)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp8DD=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8D1)->f1)->f1).KnownAggr).val;_LL28: {
# 5220
struct Cyc_Absyn_Aggrfield*_tmp8D7=Cyc_Absyn_lookup_decl_field(_tmp8DD,_tmp8F8);
if(_tmp8D7 != 0  && _tmp8D7->width == 0){
struct _tuple17 _tmp8D8=Cyc_Tcutil_addressof_props(te,_tmp8F9);struct _tuple17 _tmp8D9=_tmp8D8;int _tmp8DB;void*_tmp8DA;_LL2F: _tmp8DB=_tmp8D9.f1;_tmp8DA=_tmp8D9.f2;_LL30:;
return({struct _tuple17 _tmp9F1;_tmp9F1.f1=(_tmp8D7->tq).real_const  || _tmp8DB,_tmp9F1.f2=_tmp8DA;_tmp9F1;});}
# 5225
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp8FC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8C6)->f1;_tmp8FB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8C6)->f2;_tmp8FA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8C6)->f3;_LL6:
# 5230
 if(_tmp8FA)return bogus_ans;{
# 5233
void*_tmp8DE=Cyc_Tcutil_compress((void*)_check_null(_tmp8FC->topt));void*_tmp8DF=_tmp8DE;void*_tmp8E5;void*_tmp8E4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8DF)->tag == 3U){_LL32: _tmp8E5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8DF)->f1).elt_type;_tmp8E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8DF)->f1).ptr_atts).rgn;_LL33: {
# 5235
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp8E0=Cyc_Tcutil_compress(_tmp8E5);void*_tmp8E1=_tmp8E0;struct Cyc_Absyn_Aggrdecl*_tmp8E3;struct Cyc_List_List*_tmp8E2;switch(*((int*)_tmp8E1)){case 7U: _LL37: _tmp8E2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8E1)->f2;_LL38:
# 5238
 finfo=Cyc_Absyn_lookup_field(_tmp8E2,_tmp8FB);goto _LL36;case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E1)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E1)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp8E3=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E1)->f1)->f1).KnownAggr).val;_LL3A:
# 5240
 finfo=Cyc_Absyn_lookup_decl_field(_tmp8E3,_tmp8FB);goto _LL36;}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5243
if(finfo != 0  && finfo->width == 0)
return({struct _tuple17 _tmp9F2;_tmp9F2.f1=(finfo->tq).real_const,_tmp9F2.f2=_tmp8E4;_tmp9F2;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp8FD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8C6)->f1;_LL8: {
# 5250
void*_tmp8E6=Cyc_Tcutil_compress((void*)_check_null(_tmp8FD->topt));void*_tmp8E7=_tmp8E6;struct Cyc_Absyn_Tqual _tmp8E9;void*_tmp8E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E7)->tag == 3U){_LL3E: _tmp8E9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E7)->f1).elt_tq;_tmp8E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E7)->f1).ptr_atts).rgn;_LL3F:
# 5252
 return({struct _tuple17 _tmp9F3;_tmp9F3.f1=_tmp8E9.real_const,_tmp9F3.f2=_tmp8E8;_tmp9F3;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp8FF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8C6)->f1;_tmp8FE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8C6)->f2;_LLA: {
# 5258
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp8FF->topt));
void*_tmp8EA=t;struct Cyc_Absyn_Tqual _tmp8F3;struct Cyc_Absyn_Tqual _tmp8F2;void*_tmp8F1;struct Cyc_List_List*_tmp8F0;switch(*((int*)_tmp8EA)){case 6U: _LL43: _tmp8F0=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8EA)->f1;_LL44: {
# 5262
struct _tuple13 _tmp8EB=Cyc_Evexp_eval_const_uint_exp(_tmp8FE);struct _tuple13 _tmp8EC=_tmp8EB;unsigned int _tmp8EF;int _tmp8EE;_LL4C: _tmp8EF=_tmp8EC.f1;_tmp8EE=_tmp8EC.f2;_LL4D:;
if(!_tmp8EE)
return bogus_ans;{
struct _tuple12*_tmp8ED=Cyc_Absyn_lookup_tuple_field(_tmp8F0,(int)_tmp8EF);
if(_tmp8ED != 0)
return({struct _tuple17 _tmp9F4;_tmp9F4.f1=((*_tmp8ED).f1).real_const,({void*_tmpC93=(Cyc_Tcutil_addressof_props(te,_tmp8FF)).f2;_tmp9F4.f2=_tmpC93;});_tmp9F4;});
return bogus_ans;};}case 3U: _LL45: _tmp8F2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8EA)->f1).elt_tq;_tmp8F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8EA)->f1).ptr_atts).rgn;_LL46:
# 5270
 return({struct _tuple17 _tmp9F5;_tmp9F5.f1=_tmp8F2.real_const,_tmp9F5.f2=_tmp8F1;_tmp9F5;});case 4U: _LL47: _tmp8F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8EA)->f1).tq;_LL48:
# 5276
 return({struct _tuple17 _tmp9F6;_tmp9F6.f1=_tmp8F3.real_const,({void*_tmpC94=(Cyc_Tcutil_addressof_props(te,_tmp8FF)).f2;_tmp9F6.f2=_tmpC94;});_tmp9F6;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5280
({void*_tmp8F4=0U;({unsigned int _tmpC96=e->loc;struct _dyneither_ptr _tmpC95=({const char*_tmp8F5="unary & applied to non-lvalue";_tag_dyneither(_tmp8F5,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpC96,_tmpC95,_tag_dyneither(_tmp8F4,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5287
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp900=({void*_tmpC97=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpC97,b);});
if(_tmp900 == 0)return;{
struct Cyc_Absyn_Exp*_tmp901=_tmp900;
struct _tuple13 _tmp902=Cyc_Evexp_eval_const_uint_exp(_tmp901);struct _tuple13 _tmp903=_tmp902;unsigned int _tmp90D;int _tmp90C;_LL1: _tmp90D=_tmp903.f1;_tmp90C=_tmp903.f2;_LL2:;
if(_tmp90C  && _tmp90D <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp906=({struct Cyc_Int_pa_PrintArg_struct _tmp9F9;_tmp9F9.tag=1U,_tmp9F9.f1=(unsigned long)((int)_tmp90D);_tmp9F9;});struct Cyc_Int_pa_PrintArg_struct _tmp907=({struct Cyc_Int_pa_PrintArg_struct _tmp9F8;_tmp9F8.tag=1U,_tmp9F8.f1=(unsigned long)((int)i);_tmp9F8;});void*_tmp904[2U];_tmp904[0]=& _tmp906,_tmp904[1]=& _tmp907;({unsigned int _tmpC99=loc;struct _dyneither_ptr _tmpC98=({const char*_tmp905="a dereference will be out of bounds: %d <= %d";_tag_dyneither(_tmp905,sizeof(char),46U);});Cyc_Tcutil_warn(_tmpC99,_tmpC98,_tag_dyneither(_tmp904,sizeof(void*),2U));});});else{
# 5296
({struct Cyc_Int_pa_PrintArg_struct _tmp90A=({struct Cyc_Int_pa_PrintArg_struct _tmp9FB;_tmp9FB.tag=1U,_tmp9FB.f1=(unsigned long)((int)_tmp90D);_tmp9FB;});struct Cyc_Int_pa_PrintArg_struct _tmp90B=({struct Cyc_Int_pa_PrintArg_struct _tmp9FA;_tmp9FA.tag=1U,_tmp9FA.f1=(unsigned long)((int)i);_tmp9FA;});void*_tmp908[2U];_tmp908[0]=& _tmp90A,_tmp908[1]=& _tmp90B;({unsigned int _tmpC9B=loc;struct _dyneither_ptr _tmpC9A=({const char*_tmp909="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmp909,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpC9B,_tmpC9A,_tag_dyneither(_tmp908,sizeof(void*),2U));});});}}
return;};}
# 5300
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 5309
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp90E=e->r;void*_tmp90F=_tmp90E;struct Cyc_List_List*_tmp92E;struct Cyc_List_List*_tmp92D;enum Cyc_Absyn_Primop _tmp92C;struct Cyc_List_List*_tmp92B;struct Cyc_List_List*_tmp92A;struct Cyc_List_List*_tmp929;struct Cyc_List_List*_tmp928;struct Cyc_Absyn_Exp*_tmp927;struct Cyc_Absyn_Exp*_tmp926;struct Cyc_Absyn_Exp*_tmp925;struct Cyc_Absyn_Exp*_tmp924;void*_tmp923;struct Cyc_Absyn_Exp*_tmp922;void*_tmp921;struct Cyc_Absyn_Exp*_tmp920;struct Cyc_Absyn_Exp*_tmp91F;struct Cyc_Absyn_Exp*_tmp91E;struct Cyc_Absyn_Exp*_tmp91D;struct Cyc_Absyn_Exp*_tmp91C;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_Absyn_Exp*_tmp91A;struct Cyc_Absyn_Exp*_tmp919;struct Cyc_Absyn_Exp*_tmp918;void*_tmp917;switch(*((int*)_tmp90F)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp917=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_LL10: {
# 5321
void*_tmp910=_tmp917;struct Cyc_Absyn_Vardecl*_tmp916;struct Cyc_Absyn_Vardecl*_tmp915;switch(*((int*)_tmp910)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp915=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp910)->f1;_LL37: {
# 5324
void*_tmp911=Cyc_Tcutil_compress(_tmp915->type);void*_tmp912=_tmp911;switch(*((int*)_tmp912)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp916=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp910)->f1;_LL39:
# 5331
 if((int)_tmp916->sc == (int)Cyc_Absyn_Static){
void*_tmp913=Cyc_Tcutil_compress(_tmp916->type);void*_tmp914=_tmp913;switch(*((int*)_tmp914)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 5338
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp91A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_tmp919=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp90F)->f2;_tmp918=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp90F)->f3;_LL12:
# 5344
 return(Cyc_Tcutil_cnst_exp(0,_tmp91A) && 
Cyc_Tcutil_cnst_exp(0,_tmp919)) && 
Cyc_Tcutil_cnst_exp(0,_tmp918);case 9U: _LL13: _tmp91C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_tmp91B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp90F)->f2;_LL14:
# 5348
 return Cyc_Tcutil_cnst_exp(0,_tmp91C) && Cyc_Tcutil_cnst_exp(0,_tmp91B);case 41U: _LL15: _tmp91D=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_LL16:
 _tmp91E=_tmp91D;goto _LL18;case 12U: _LL17: _tmp91E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_LL18:
 _tmp91F=_tmp91E;goto _LL1A;case 13U: _LL19: _tmp91F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_LL1A:
# 5352
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp91F);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp90F)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp921=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_tmp920=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp90F)->f2;_LL1C:
# 5354
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp920);}else{_LL1D: _tmp923=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_tmp922=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp90F)->f2;_LL1E:
# 5357
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp922);}case 15U: _LL1F: _tmp924=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_LL20:
# 5359
 return Cyc_Tcutil_cnst_exp(1,_tmp924);case 27U: _LL21: _tmp926=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp90F)->f2;_tmp925=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp90F)->f3;_LL22:
# 5361
 return Cyc_Tcutil_cnst_exp(0,_tmp926) && Cyc_Tcutil_cnst_exp(0,_tmp925);case 28U: _LL23: _tmp927=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_LL24:
# 5363
 return Cyc_Tcutil_cnst_exp(0,_tmp927);case 26U: _LL25: _tmp928=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_LL26:
 _tmp929=_tmp928;goto _LL28;case 30U: _LL27: _tmp929=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp90F)->f2;_LL28:
 _tmp92A=_tmp929;goto _LL2A;case 29U: _LL29: _tmp92A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp90F)->f3;_LL2A:
# 5367
 for(0;_tmp92A != 0;_tmp92A=_tmp92A->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple18*)_tmp92A->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp92C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_tmp92B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp90F)->f2;_LL2C:
# 5372
 _tmp92D=_tmp92B;goto _LL2E;case 24U: _LL2D: _tmp92D=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_LL2E:
 _tmp92E=_tmp92D;goto _LL30;case 31U: _LL2F: _tmp92E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp90F)->f1;_LL30:
# 5375
 for(0;_tmp92E != 0;_tmp92E=_tmp92E->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp92E->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 5382
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5386
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp92F=Cyc_Tcutil_compress(t);void*_tmp930=_tmp92F;struct Cyc_List_List*_tmp940;struct Cyc_List_List*_tmp93F;void*_tmp93E;void*_tmp93D;void*_tmp93C;struct Cyc_List_List*_tmp93B;switch(*((int*)_tmp930)){case 0U: _LL1: _tmp93C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp930)->f1;_tmp93B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp930)->f2;_LL2: {
# 5390
void*_tmp931=_tmp93C;union Cyc_Absyn_AggrInfo _tmp93A;struct Cyc_List_List*_tmp939;struct Cyc_Absyn_Enumdecl*_tmp938;switch(*((int*)_tmp931)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp938=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp931)->f2;_LL15:
# 5395
 if(_tmp938 == 0  || _tmp938->fields == 0)
return 0;
_tmp939=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp938->fields))->v;goto _LL17;case 16U: _LL16: _tmp939=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp931)->f1;_LL17:
# 5399
{struct Cyc_List_List*_tmp932=_tmp939;for(0;_tmp932 != 0;_tmp932=_tmp932->tl){
if(((struct Cyc_Absyn_Enumfield*)_tmp932->hd)->tag == 0)
return _tmp932 == _tmp939;{
struct _tuple13 _tmp933=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_tmp932->hd)->tag));struct _tuple13 _tmp934=_tmp933;unsigned int _tmp936;int _tmp935;_LL1D: _tmp936=_tmp934.f1;_tmp935=_tmp934.f2;_LL1E:;
if(_tmp935  && _tmp936 == (unsigned int)0)
return 1;};}}
# 5406
return 0;case 20U: _LL18: _tmp93A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp931)->f1;_LL19: {
# 5409
struct Cyc_Absyn_Aggrdecl*_tmp937=Cyc_Absyn_get_known_aggrdecl(_tmp93A);
if(_tmp937->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp937->impl))->exist_vars != 0)return 0;
if((int)_tmp937->kind == (int)Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp937->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(_tmp937->tvs,_tmp93B,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp937->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp93D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp930)->f1).ptr_atts).nullable;_LL4:
# 5417
 return Cyc_Tcutil_force_type2bool(1,_tmp93D);case 4U: _LL5: _tmp93E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp930)->f1).elt_type;_LL6:
 return Cyc_Tcutil_zeroable_type(_tmp93E);case 6U: _LL7: _tmp93F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp930)->f1;_LL8:
# 5420
 for(0;_tmp93F != 0;_tmp93F=_tmp93F->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmp93F->hd)).f2))return 0;}
return 1;case 7U: _LL9: _tmp940=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp930)->f2;_LLA:
 return Cyc_Tcutil_fields_zeroable(0,0,_tmp940);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 5427
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5429
struct _RegionHandle _tmp941=_new_region("rgn");struct _RegionHandle*rgn=& _tmp941;_push_region(rgn);
{struct Cyc_List_List*_tmp942=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,_tmp942,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp943=0;_npop_handler(0U);return _tmp943;}}{
# 5437
int _tmp944=1;_npop_handler(0U);return _tmp944;};}
# 5430
;_pop_region(rgn);}
# 5441
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmp945=t;struct Cyc_Absyn_Typedefdecl*_tmp94A;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp945)->tag == 8U){_LL1: _tmp94A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp945)->f3;_LL2:
# 5444
 if(_tmp94A != 0){
struct Cyc_Absyn_Tqual _tmp946=_tmp94A->tq;
if(((_tmp946.print_const  || _tmp946.q_volatile) || _tmp946.q_restrict) || _tmp946.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp949=({struct Cyc_String_pa_PrintArg_struct _tmp9FC;_tmp9FC.tag=0U,({struct _dyneither_ptr _tmpC9C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9FC.f1=_tmpC9C;});_tmp9FC;});void*_tmp947[1U];_tmp947[0]=& _tmp949;({unsigned int _tmpC9E=loc;struct _dyneither_ptr _tmpC9D=({const char*_tmp948="qualifier within typedef type %s is ignored";_tag_dyneither(_tmp948,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpC9E,_tmpC9D,_tag_dyneither(_tmp947,sizeof(void*),1U));});});}
# 5449
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5456
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp94B=Cyc_Tcutil_compress(t);void*_tmp94C=_tmp94B;struct Cyc_List_List**_tmp952;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94C)->tag == 5U){_LL1: _tmp952=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94C)->f1).attributes;_LL2: {
# 5459
struct Cyc_List_List*_tmp94D=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*_tmp952,(void*)atts->hd))
({struct Cyc_List_List*_tmpC9F=({struct Cyc_List_List*_tmp94E=_cycalloc(sizeof(*_tmp94E));_tmp94E->hd=(void*)atts->hd,_tmp94E->tl=*_tmp952;_tmp94E;});*_tmp952=_tmpC9F;});}else{
# 5466
_tmp94D=({struct Cyc_List_List*_tmp94F=_cycalloc(sizeof(*_tmp94F));_tmp94F->hd=(void*)atts->hd,_tmp94F->tl=_tmp94D;_tmp94F;});}}
return _tmp94D;}}else{_LL3: _LL4:
({void*_tmp950=0U;({struct _dyneither_ptr _tmpCA0=({const char*_tmp951="transfer_fn_type_atts";_tag_dyneither(_tmp951,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCA0,_tag_dyneither(_tmp950,sizeof(void*),0U));});});}_LL0:;}
# 5473
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp953=Cyc_Tcutil_compress(t);void*_tmp954=_tmp953;struct Cyc_Absyn_Exp*_tmp956;struct Cyc_Absyn_PtrInfo _tmp955;switch(*((int*)_tmp954)){case 3U: _LL1: _tmp955=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp954)->f1;_LL2:
 return({void*_tmpCA1=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCA1,(_tmp955.ptr_atts).bounds);});case 4U: _LL3: _tmp956=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp954)->f1).num_elts;_LL4:
 return _tmp956;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5483
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmp957=b;struct Cyc_Absyn_Vardecl*_tmp95B;struct Cyc_Absyn_Vardecl*_tmp95A;struct Cyc_Absyn_Vardecl*_tmp959;struct Cyc_Absyn_Vardecl*_tmp958;switch(*((int*)_tmp957)){case 5U: _LL1: _tmp958=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp957)->f1;_LL2:
 _tmp959=_tmp958;goto _LL4;case 4U: _LL3: _tmp959=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp957)->f1;_LL4:
 _tmp95A=_tmp959;goto _LL6;case 3U: _LL5: _tmp95A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp957)->f1;_LL6:
 _tmp95B=_tmp95A;goto _LL8;case 1U: _LL7: _tmp95B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp957)->f1;_LL8:
# 5489
 if(!_tmp95B->escapes)return _tmp95B;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5493
return 0;}
# 5497
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp95C=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmp95C=({struct Cyc_List_List*_tmp95D=_cycalloc(sizeof(*_tmp95D));_tmp95D->hd=*((void**)_check_null((void**)x->hd)),_tmp95D->tl=_tmp95C;_tmp95D;});}}
return _tmp95C;}
# 5504
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp95E=Cyc_Tcutil_compress(t);void*_tmp95F=_tmp95E;void*_tmp969;struct Cyc_Absyn_Tqual _tmp968;struct Cyc_Absyn_Exp*_tmp967;void*_tmp966;unsigned int _tmp965;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95F)->tag == 4U){_LL1: _tmp969=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95F)->f1).elt_type;_tmp968=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95F)->f1).tq;_tmp967=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95F)->f1).num_elts;_tmp966=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95F)->f1).zero_term;_tmp965=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95F)->f1).zt_loc;_LL2: {
# 5507
void*b;
if(_tmp967 == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 5511
struct Cyc_Absyn_Exp*bnd=_tmp967;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmp960=0U;({struct _dyneither_ptr _tmpCA2=({const char*_tmp961="cannot convert tag without type!";_tag_dyneither(_tmp961,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCA2,_tag_dyneither(_tmp960,sizeof(void*),0U));});});{
void*_tmp962=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp963=_tmp962;void*_tmp964;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp963)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp963)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp963)->f2 != 0){_LL6: _tmp964=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp963)->f2)->hd;_LL7:
# 5517
 b=Cyc_Absyn_thin_bounds_exp(({void*_tmpCA3=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmpCA3,Cyc_Absyn_valueof_exp(_tmp964,0U),0,Cyc_Absyn_No_coercion,0U);}));
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 5520
 if(Cyc_Tcutil_is_const_exp(bnd))
b=Cyc_Absyn_thin_bounds_exp(bnd);else{
# 5523
b=Cyc_Absyn_fat_bound_type;}}_LL5:;};}else{
# 5527
b=Cyc_Absyn_thin_bounds_exp(bnd);}}
# 5529
return Cyc_Absyn_atb_type(_tmp969,rgn,_tmp968,b,_tmp966);}}else{_LL3: _LL4:
 return t;}_LL0:;}

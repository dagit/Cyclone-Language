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
# 34 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
# 36
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
# 58 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 73 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 75
({struct _dyneither_ptr _tmp722=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp721=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp721,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp722,ap);});}
# 77
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 79
({struct _dyneither_ptr _tmp724=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp723=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp723,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp724,ap);});}
# 82
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 85
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 95
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 97
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 123
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp2=*x;struct _tuple14 _tmp3=_tmp2;struct _tuple1*_tmp9;struct _tuple1*_tmp8;_LL1: _tmp9=_tmp3.f1;_tmp8=_tmp3.f2;_LL2:;{
struct _tuple14 _tmp4=*y;struct _tuple14 _tmp5=_tmp4;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL4: _tmp7=_tmp5.f1;_tmp6=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp9,_tmp7);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp8,_tmp6);};};}
# 132
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp11=_region_malloc(d,sizeof(*_tmp11));
({struct Cyc_List_List**_tmp730=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp730;}),({
struct Cyc_List_List**_tmp72F=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp72F;}),({
struct Cyc_Dict_Dict*_tmp72E=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp72D=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp72D;});_tmpC;});_tmp11->aggrs_so_far=_tmp72E;}),({
struct Cyc_List_List**_tmp72C=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp72C;}),({
struct Cyc_Set_Set**_tmp72B=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp72A=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp72A;});_tmpE;});_tmp11->datatypes_so_far=_tmp72B;}),({
struct Cyc_Dict_Dict*_tmp729=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp728=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp728;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp729;}),({
struct Cyc_Dict_Dict*_tmp727=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp726=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp726;});_tmp10;});_tmp11->qvar_tags=_tmp727;}),({
struct Cyc_Xarray_Xarray*_tmp725=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp725;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 151
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 157
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 160
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp731=({struct Cyc_Toc_TocStateWrap _tmp6EF;_tmp6EF.dyn=_tmp1C,_tmp6EF.state=_tmp1B;_tmp6EF;});*ts=_tmp731;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp1A=({struct _tuple15 _tmp6F0;_tmp6F0.f1=_tmp1B,_tmp6F0.f2=arg;_tmp6F0;});
({void*_tmp732=f(h,& _tmp1A);res=_tmp732;});}
# 168
;}
# 170
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)));
return res;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 174
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 177
struct _tuple17 _tmp1D=*env;struct _tuple17 _tmp1E=_tmp1D;struct Cyc_Toc_TocState*_tmp24;struct _tuple1*_tmp23;void*(*_tmp22)(struct _tuple1*);_LL1: _tmp24=_tmp1E.f1;_tmp23=(_tmp1E.f2)->f1;_tmp22=(_tmp1E.f2)->f2;_LL2:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp24->aggrs_so_far,_tmp23);
if(v == 0)
return _tmp22(_tmp23);else{
# 182
struct _tuple18*_tmp1F=*v;struct _tuple18*_tmp20=_tmp1F;void*_tmp21;_LL4: _tmp21=_tmp20->f2;_LL5:;
return _tmp21;}};}
# 187
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp6F1;_tmp6F1.f1=q,_tmp6F1.f2=type_maker;_tmp6F1;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp25[5U]="curr";
# 200 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp25,_tmp25,_tmp25 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp26[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp26,_tmp26,_tmp26 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp27[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp27,_tmp27,_tmp27 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp28[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp28,_tmp28,_tmp28 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp29[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp29,_tmp29,_tmp29 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2A[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2B[17U]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2B,_tmp2B,_tmp2B + 17U};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp2C[16U]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp2C,_tmp2C,_tmp2C + 16U};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;static char _tmp2D[7U]="_throw";
# 221 "toc.cyc"
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
# 289
void*Cyc_Toc_dyneither_ptr_type(){
static void*dpt=0;
void*_tmp6D=dpt;
if(_tmp6D == 0){
({void*_tmp733=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0),0);_tmp6D=_tmp733;});
dpt=_tmp6D;}
# 296
return _tmp6D;}
# 299
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 301
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 305
static struct _tuple19*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp70=_cycalloc(sizeof(*_tmp70));({struct Cyc_List_List*_tmp735=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));({void*_tmp734=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=1U,_tmp6E->f1=name;_tmp6E;});_tmp6F->hd=_tmp734;}),_tmp6F->tl=0;_tmp6F;});_tmp70->f1=_tmp735;}),_tmp70->f2=e;_tmp70;});}
# 311
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp71=e->r;void*_tmp72=_tmp71;struct Cyc_Absyn_Exp*_tmp73;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp73=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp73);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 317
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=14U,_tmp74->f1=t,_tmp74->f2=e,_tmp74->f3=0,_tmp74->f4=Cyc_Absyn_No_coercion;_tmp74;});}
# 320
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=20U,_tmp75->f1=e;_tmp75;});}
# 323
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=23U,_tmp76->f1=e1,_tmp76->f2=e2;_tmp76;});}
# 326
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=37U,_tmp77->f1=s;_tmp77;});}
# 329
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=17U,_tmp78->f1=t;_tmp78;});}
# 332
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=10U,_tmp79->f1=e,_tmp79->f2=es,_tmp79->f3=0,_tmp79->f4=1;_tmp79;});}
# 335
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=1U,_tmp7A->f1=e;_tmp7A;});}
# 338
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=2U,_tmp7B->f1=s1,_tmp7B->f2=s2;_tmp7B;});}
# 341
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=6U,_tmp7C->f1=e1,_tmp7C->f2=e2,_tmp7C->f3=e3;_tmp7C;});}
# 344
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=21U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});}
# 347
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=22U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});}
# 350
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 353
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=36U,_tmp7F->f1=tdopt,_tmp7F->f2=ds;_tmp7F;});}
# 355
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=8U,_tmp80->f1=v;_tmp80;});}
# 358
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 362
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp81=e->r;void*_tmp82=_tmp81;struct Cyc_Absyn_Exp*_tmp83;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->tag == 20U){_LL1: _tmp83=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp83,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 381
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 390
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 399
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 408
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 419
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp84=Cyc_Tcutil_compress(t);void*_tmp85=_tmp84;enum Cyc_Absyn_Size_of _tmp8C;switch(*((int*)_tmp85)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)){case 1U: _LL1: _tmp8C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f2;_LL2:
# 423
{enum Cyc_Absyn_Size_of _tmp86=_tmp8C;switch(_tmp86){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp87=0U;({struct _dyneither_ptr _tmp736=({const char*_tmp88="impossible IntCon (not char, short or int)";_tag_dyneither(_tmp88,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp736,_tag_dyneither(_tmp87,sizeof(void*),0U));});});}_LLD:;}
# 429
goto _LL0;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}default: goto _LLB;}case 3U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp6F2;_tmp6F2.tag=0U,({struct _dyneither_ptr _tmp737=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp6F2.f1=_tmp737;});_tmp6F2;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({struct _dyneither_ptr _tmp738=({const char*_tmp8A="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8A,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp738,_tag_dyneither(_tmp89,sizeof(void*),1U));});});}_LL0:;}
# 436
return function;}
# 438
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8D=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8E=_tmp8D;void*_tmp91;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->tag == 3U){_LL1: _tmp91=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp91);}else{_LL3: _LL4:
({void*_tmp8F=0U;({struct _dyneither_ptr _tmp739=({const char*_tmp90="impossible type (not pointer)";_tag_dyneither(_tmp90,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp739,_tag_dyneither(_tmp8F,sizeof(void*),0U));});});}_LL0:;}
# 448
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp92=e->r;void*_tmp93=_tmp92;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;long long _tmp99;int _tmp98;short _tmp97;struct _dyneither_ptr _tmp96;char _tmp95;switch(*((int*)_tmp93)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Null_c).tag){case 2U: _LL1: _tmp95=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Char_c).val).f2;_LL2:
 return _tmp95 == '\000';case 3U: _LL3: _tmp96=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Wchar_c).val;_LL4: {
# 452
unsigned long _tmp94=Cyc_strlen((struct _dyneither_ptr)_tmp96);
int i=0;
if(_tmp94 >= 2  && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),1))== 'x'  && _tmp94 >= 3) && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp94;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 462
return 0;}}case 4U: _LL5: _tmp97=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Short_c).val).f2;_LL6:
 return _tmp97 == 0;case 5U: _LL7: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Int_c).val).f2;_LL8:
 return _tmp98 == 0;case 6U: _LL9: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).LongLong_c).val).f2;_LLA:
 return _tmp99 == 0;case 1U: _LLD: _LLE:
# 467
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 466
 goto _LLE;case 14U: _LLF: _tmp9A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL10:
# 468
 return Cyc_Toc_is_zero(_tmp9A);case 24U: _LL11: _tmp9B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9B);case 26U: _LL13: _tmp9C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL14:
 _tmp9D=_tmp9C;goto _LL16;case 29U: _LL15: _tmp9D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_LL16:
 _tmp9E=_tmp9D;goto _LL18;case 25U: _LL17: _tmp9E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL18:
 _tmp9F=_tmp9E;goto _LL1A;case 36U: _LL19: _tmp9F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL1A:
# 474
 for(0;_tmp9F != 0;_tmp9F=_tmp9F->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp9F->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 482
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA0=x;union Cyc_Absyn_Nmspace _tmpB2;struct _dyneither_ptr*_tmpB1;_LL1: _tmpB2=_tmpA0->f1;_tmpB1=_tmpA0->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA1=_tmpB2;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpAE;switch((_tmpA1.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAE=0;goto _LL7;case 1U: _LL6: _tmpAE=(_tmpA1.Rel_n).val;_LL7:
 _tmpAF=_tmpAE;goto _LL9;case 2U: _LL8: _tmpAF=(_tmpA1.Abs_n).val;_LL9:
 _tmpB0=_tmpAF;goto _LLB;default: _LLA: _tmpB0=(_tmpA1.C_n).val;_LLB:
# 491
 if(_tmpB0 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp6F4;_tmp6F4.tag=0U,_tmp6F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp6F4;});struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp6F3;_tmp6F3.tag=0U,_tmp6F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp6F3;});void*_tmpA2[2U];_tmpA2[0]=& _tmpA4,_tmpA2[1]=& _tmpA5;({struct _dyneither_ptr _tmp73A=({const char*_tmpA3="%s_%s_struct";_tag_dyneither(_tmpA3,sizeof(char),13U);});Cyc_aprintf(_tmp73A,_tag_dyneither(_tmpA2,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA6=_new_region("r");struct _RegionHandle*r=& _tmpA6;_push_region(r);
{struct _dyneither_ptr _tmpAD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp6F7;_tmp6F7.tag=0U,_tmp6F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp6F7;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp6F6;_tmp6F6.tag=0U,({struct _dyneither_ptr _tmp73D=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp73C=r;struct Cyc_List_List*_tmp73B=_tmpB0;Cyc_rstr_sepstr(_tmp73C,_tmp73B,({const char*_tmpAC="_";_tag_dyneither(_tmpAC,sizeof(char),2U);}));}));_tmp6F6.f1=_tmp73D;});_tmp6F6;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp6F5;_tmp6F5.tag=0U,_tmp6F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp6F5;});void*_tmpA7[3U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA,_tmpA7[2]=& _tmpAB;({struct _dyneither_ptr _tmp73E=({const char*_tmpA8="%s_%s_%s_struct";_tag_dyneither(_tmpA8,sizeof(char),16U);});Cyc_aprintf(_tmp73E,_tag_dyneither(_tmpA7,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAD;};_pop_region(r);};}_LL3:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 504
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 507
struct _tuple20 _tmpB3=*env;struct _tuple20 _tmpB4=_tmpB3;struct Cyc_Dict_Dict*_tmpC7;struct _tuple14*_tmpC6;_LL1: _tmpC7=(_tmpB4.f1)->qvar_tags;_tmpC6=_tmpB4.f2;_LL2:;{
struct _tuple14 _tmpB5=*_tmpC6;struct _tuple14 _tmpB6=_tmpB5;struct _tuple1*_tmpC5;struct _tuple1*_tmpC4;_LL4: _tmpC5=_tmpB6.f1;_tmpC4=_tmpB6.f2;_LL5:;{
struct _handler_cons _tmpB7;_push_handler(& _tmpB7);{int _tmpB9=0;if(setjmp(_tmpB7.handler))_tmpB9=1;if(!_tmpB9){{struct _tuple1*_tmpBA=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmpC7,Cyc_Toc_qvar_tag_cmp,_tmpC6);_npop_handler(0U);return _tmpBA;};_pop_handler();}else{void*_tmpB8=(void*)_exn_thrown;void*_tmpBB=_tmpB8;void*_tmpC3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBB)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 511
struct _tuple14*_tmpBC=({struct _tuple14*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->f1=_tmpC5,_tmpC2->f2=_tmpC4;_tmpC2;});
struct _tuple1*_tmpBD=_tmpC5;union Cyc_Absyn_Nmspace _tmpC1;struct _dyneither_ptr*_tmpC0;_LLC: _tmpC1=_tmpBD->f1;_tmpC0=_tmpBD->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC0,_tmpC4);
struct _tuple1*res=({struct _tuple1*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->f1=_tmpC1,({struct _dyneither_ptr*_tmp73F=({struct _dyneither_ptr*_tmpBE=_cycalloc(sizeof(*_tmpBE));*_tmpBE=newvar;_tmpBE;});_tmpBF->f2=_tmp73F;});_tmpBF;});
({struct Cyc_Dict_Dict _tmp740=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC7,_tmpBC,res);*_tmpC7=_tmp740;});
return res;};}}else{_LL9: _tmpC3=_tmpBB;_LLA:(int)_rethrow(_tmpC3);}_LL6:;}};};};}
# 519
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp6F8;_tmp6F8.f1=fieldname,_tmp6F8.f2=dtname;_tmp6F8;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 526
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->kind=Cyc_Absyn_StructA,_tmpCA->sc=Cyc_Absyn_Public,_tmpCA->tvs=0,_tmpCA->attributes=0,_tmpCA->expected_mem_kind=0,({
# 529
struct _tuple1*_tmp743=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));({union Cyc_Absyn_Nmspace _tmp742=Cyc_Absyn_Rel_n(0);_tmpC8->f1=_tmp742;}),_tmpC8->f2=name;_tmpC8;});_tmpCA->name=_tmp743;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp741=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->exist_vars=0,_tmpC9->rgn_po=0,_tmpC9->tagged=0,_tmpC9->fields=fs;_tmpC9;});_tmpCA->impl=_tmp741;});_tmpCA;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 538
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 541
struct _tuple21 _tmpCB=*env;struct _tuple21 _tmpCC=_tmpCB;struct Cyc_List_List**_tmpE2;struct Cyc_List_List*_tmpE1;_LL1: _tmpE2=(_tmpCC.f1)->tuple_types;_tmpE1=_tmpCC.f2;_LL2:;
# 543
{struct Cyc_List_List*_tmpCD=*_tmpE2;for(0;_tmpCD != 0;_tmpCD=_tmpCD->tl){
struct _tuple22*_tmpCE=(struct _tuple22*)_tmpCD->hd;struct _tuple22*_tmpCF=_tmpCE;void*_tmpD2;struct Cyc_List_List*_tmpD1;_LL4: _tmpD2=_tmpCF->f1;_tmpD1=_tmpCF->f2;_LL5:;{
struct Cyc_List_List*_tmpD0=_tmpE1;
for(0;_tmpD0 != 0  && _tmpD1 != 0;(_tmpD0=_tmpD0->tl,_tmpD1=_tmpD1->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD0->hd)).f2,(void*)_tmpD1->hd))
break;}
if(_tmpD0 == 0  && _tmpD1 == 0)
return _tmpD2;};}}{
# 554
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct _dyneither_ptr _tmp745=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDF=({struct Cyc_Int_pa_PrintArg_struct _tmp6F9;_tmp6F9.tag=1U,_tmp6F9.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp6F9;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({struct _dyneither_ptr _tmp744=({const char*_tmpDE="_tuple%d";_tag_dyneither(_tmpDE,sizeof(char),9U);});Cyc_aprintf(_tmp744,_tag_dyneither(_tmpDD,sizeof(void*),1U));});});*_tmpE0=_tmp745;});_tmpE0;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE1);
struct Cyc_List_List*_tmpD3=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp748=({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct Cyc_Absyn_Aggrfield*_tmp747=({struct Cyc_Absyn_Aggrfield*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct _dyneither_ptr*_tmp746=Cyc_Absyn_fieldname(i);_tmpD4->name=_tmp746;}),_tmpD4->tq=Cyc_Toc_mt_tq,_tmpD4->type=(void*)ts2->hd,_tmpD4->width=0,_tmpD4->attributes=0,_tmpD4->requires_clause=0;_tmpD4;});_tmpD5->hd=_tmp747;}),_tmpD5->tl=_tmpD3;_tmpD5;});_tmpD3=_tmp748;});}}
({struct Cyc_List_List*_tmp749=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD3);_tmpD3=_tmp749;});{
struct Cyc_Absyn_Aggrdecl*_tmpD6=Cyc_Toc_make_c_struct_defn(xname,_tmpD3);
void*_tmpD7=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDC=_cycalloc(sizeof(*_tmpDC));*_tmpDC=_tmpD6;_tmpDC;})),0);
({struct Cyc_List_List*_tmp74B=({struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct Cyc_Absyn_Decl*_tmp74A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=5U,_tmpD8->f1=_tmpD6;_tmpD8;}),0U);_tmpD9->hd=_tmp74A;}),_tmpD9->tl=Cyc_Toc_result_decls;_tmpD9;});Cyc_Toc_result_decls=_tmp74B;});
({struct Cyc_List_List*_tmp74D=({struct Cyc_List_List*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));({struct _tuple22*_tmp74C=({struct _tuple22*_tmpDA=_region_malloc(d,sizeof(*_tmpDA));_tmpDA->f1=_tmpD7,_tmpDA->f2=ts;_tmpDA;});_tmpDB->hd=_tmp74C;}),_tmpDB->tl=*_tmpE2;_tmpDB;});*_tmpE2=_tmp74D;});
return _tmpD7;};};}
# 567
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 572
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 575
struct _tuple24*_tmpE3=env;struct Cyc_List_List**_tmpF7;enum Cyc_Absyn_AggrKind _tmpF6;struct Cyc_List_List*_tmpF5;_LL1: _tmpF7=(_tmpE3->f1)->anon_aggr_types;_tmpF6=(_tmpE3->f2)->f1;_tmpF5=(_tmpE3->f2)->f2;_LL2:;
# 577
{struct Cyc_List_List*_tmpE4=*_tmpF7;for(0;_tmpE4 != 0;_tmpE4=_tmpE4->tl){
struct _tuple25*_tmpE5=(struct _tuple25*)_tmpE4->hd;struct _tuple25*_tmpE6=_tmpE5;void*_tmpE9;enum Cyc_Absyn_AggrKind _tmpE8;struct Cyc_List_List*_tmpE7;_LL4: _tmpE9=_tmpE6->f1;_tmpE8=_tmpE6->f2;_tmpE7=_tmpE6->f3;_LL5:;
if(_tmpF6 != _tmpE8)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE7,_tmpF5))
return _tmpE9;}}{
# 586
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF4=_cycalloc(sizeof(*_tmpF4));({struct _dyneither_ptr _tmp74F=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF3=({struct Cyc_Int_pa_PrintArg_struct _tmp6FA;_tmp6FA.tag=1U,_tmp6FA.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp6FA;});void*_tmpF1[1U];_tmpF1[0]=& _tmpF3;({struct _dyneither_ptr _tmp74E=({const char*_tmpF2="_tuple%d";_tag_dyneither(_tmpF2,sizeof(char),9U);});Cyc_aprintf(_tmp74E,_tag_dyneither(_tmpF1,sizeof(void*),1U));});});*_tmpF4=_tmp74F;});_tmpF4;});
struct Cyc_Absyn_Aggrdecl*_tmpEA=Cyc_Toc_make_c_struct_defn(xname,_tmpF5);
_tmpEA->kind=_tmpF6;{
void*_tmpEB=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF0=_cycalloc(sizeof(*_tmpF0));*_tmpF0=_tmpEA;_tmpF0;})),0);
({struct Cyc_List_List*_tmp751=({struct Cyc_List_List*_tmpED=_cycalloc(sizeof(*_tmpED));({struct Cyc_Absyn_Decl*_tmp750=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->tag=5U,_tmpEC->f1=_tmpEA;_tmpEC;}),0U);_tmpED->hd=_tmp750;}),_tmpED->tl=Cyc_Toc_result_decls;_tmpED;});Cyc_Toc_result_decls=_tmp751;});
({struct Cyc_List_List*_tmp753=({struct Cyc_List_List*_tmpEF=_region_malloc(d,sizeof(*_tmpEF));({struct _tuple25*_tmp752=({struct _tuple25*_tmpEE=_region_malloc(d,sizeof(*_tmpEE));_tmpEE->f1=_tmpEB,_tmpEE->f2=_tmpF6,_tmpEE->f3=_tmpF5;_tmpEE;});_tmpEF->hd=_tmp752;}),_tmpEF->tl=*_tmpF7;_tmpEF;});*_tmpF7=_tmp753;});
return _tmpEB;};};}
# 594
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(({struct _tuple23*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->f1=ak,_tmpF8->f2=fs;_tmpF8;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple26{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple27{struct Cyc_Toc_TocState*f1;struct _tuple26*f2;};struct _tuple28{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 603
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple27*env){
# 612
struct _tuple27 _tmpF9=*env;struct _tuple27 _tmpFA=_tmpF9;struct Cyc_List_List**_tmp11D;struct _tuple1*_tmp11C;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_List_List*_tmp119;_LL1: _tmp11D=(_tmpFA.f1)->abs_struct_types;_tmp11C=(_tmpFA.f2)->f1;_tmp11B=(_tmpFA.f2)->f2;_tmp11A=(_tmpFA.f2)->f3;_tmp119=(_tmpFA.f2)->f4;_LL2:;
# 616
{struct Cyc_List_List*_tmpFB=*_tmp11D;for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){
struct _tuple28*_tmpFC=(struct _tuple28*)_tmpFB->hd;struct _tuple28*_tmpFD=_tmpFC;struct _tuple1*_tmp105;struct Cyc_List_List*_tmp104;void*_tmp103;_LL4: _tmp105=_tmpFD->f1;_tmp104=_tmpFD->f2;_tmp103=_tmpFD->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp105,_tmp11C)== 0  && ({
int _tmp754=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11A);_tmp754 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp104);})){
int okay=1;
{struct Cyc_List_List*_tmpFE=_tmp11A;for(0;_tmpFE != 0;(_tmpFE=_tmpFE->tl,_tmp104=_tmp104->tl)){
void*_tmpFF=(void*)_tmpFE->hd;
void*_tmp100=(void*)((struct Cyc_List_List*)_check_null(_tmp104))->hd;
{struct Cyc_Absyn_Kind*_tmp101=Cyc_Tcutil_type_kind(_tmpFF);struct Cyc_Absyn_Kind*_tmp102=_tmp101;switch(((struct Cyc_Absyn_Kind*)_tmp102)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 627
 continue;default: _LLB: _LLC:
# 630
 if(Cyc_Tcutil_unify(_tmpFF,_tmp100) || ({void*_tmp755=Cyc_Toc_typ_to_c(_tmpFF);Cyc_Tcutil_unify(_tmp755,Cyc_Toc_typ_to_c(_tmp100));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 635
break;}}
# 637
if(okay)
return _tmp103;}}}{
# 644
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp118=_cycalloc(sizeof(*_tmp118));({struct _dyneither_ptr _tmp757=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp117=({struct Cyc_Int_pa_PrintArg_struct _tmp6FB;_tmp6FB.tag=1U,_tmp6FB.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp6FB;});void*_tmp115[1U];_tmp115[0]=& _tmp117;({struct _dyneither_ptr _tmp756=({const char*_tmp116="_tuple%d";_tag_dyneither(_tmp116,sizeof(char),9U);});Cyc_aprintf(_tmp756,_tag_dyneither(_tmp115,sizeof(void*),1U));});});*_tmp118=_tmp757;});_tmp118;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp106=0;
# 648
({struct Cyc_List_List*_tmp759=({struct Cyc_List_List*_tmp108=_region_malloc(d,sizeof(*_tmp108));({struct _tuple28*_tmp758=({struct _tuple28*_tmp107=_region_malloc(d,sizeof(*_tmp107));_tmp107->f1=_tmp11C,_tmp107->f2=_tmp11A,_tmp107->f3=x;_tmp107;});_tmp108->hd=_tmp758;}),_tmp108->tl=*_tmp11D;_tmp108;});*_tmp11D=_tmp759;});{
# 651
struct _RegionHandle _tmp109=_new_region("r");struct _RegionHandle*r=& _tmp109;_push_region(r);
{struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11B,_tmp11A);
for(0;_tmp119 != 0;_tmp119=_tmp119->tl){
struct Cyc_Absyn_Aggrfield*_tmp10B=(struct Cyc_Absyn_Aggrfield*)_tmp119->hd;
void*t=_tmp10B->type;
struct Cyc_List_List*atts=_tmp10B->attributes;
# 661
if((_tmp119->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t))) && !
Cyc_Tcutil_is_array_type(t))
({struct Cyc_List_List*_tmp75B=({struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*_tmp10D));({void*_tmp75A=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->tag=6U,_tmp10C->f1=0;_tmp10C;});_tmp10D->hd=_tmp75A;}),_tmp10D->tl=atts;_tmp10D;});atts=_tmp75B;});
({void*_tmp75C=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10A,t));t=_tmp75C;});
# 668
if(Cyc_Tcutil_unify(t,Cyc_Absyn_void_type))
({void*_tmp760=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=4U,({void*_tmp75F=Cyc_Absyn_void_star_type();(_tmp10E->f1).elt_type=_tmp75F;}),({struct Cyc_Absyn_Tqual _tmp75E=Cyc_Absyn_empty_tqual(0U);(_tmp10E->f1).tq=_tmp75E;}),({
struct Cyc_Absyn_Exp*_tmp75D=Cyc_Absyn_uint_exp(0U,0U);(_tmp10E->f1).num_elts=_tmp75D;}),(_tmp10E->f1).zero_term=Cyc_Absyn_false_type,(_tmp10E->f1).zt_loc=0U;_tmp10E;});
# 669
t=_tmp760;});
# 672
({struct Cyc_List_List*_tmp762=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Absyn_Aggrfield*_tmp761=({struct Cyc_Absyn_Aggrfield*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->name=_tmp10B->name,_tmp10F->tq=Cyc_Toc_mt_tq,_tmp10F->type=t,_tmp10F->width=_tmp10B->width,_tmp10F->attributes=atts,_tmp10F->requires_clause=0;_tmp10F;});_tmp110->hd=_tmp761;}),_tmp110->tl=_tmp106;_tmp110;});_tmp106=_tmp762;});}
# 674
({struct Cyc_List_List*_tmp763=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp106);_tmp106=_tmp763;});{
struct Cyc_Absyn_Aggrdecl*_tmp111=Cyc_Toc_make_c_struct_defn(xname,_tmp106);
({struct Cyc_List_List*_tmp765=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Decl*_tmp764=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=5U,_tmp112->f1=_tmp111;_tmp112;}),0U);_tmp113->hd=_tmp764;}),_tmp113->tl=Cyc_Toc_result_decls;_tmp113;});Cyc_Toc_result_decls=_tmp765;});{
void*_tmp114=x;_npop_handler(0U);return _tmp114;};};}
# 652
;_pop_region(r);};};}
# 680
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 684
struct _tuple26 env=({struct _tuple26 _tmp6FC;_tmp6FC.f1=struct_name,_tmp6FC.f2=type_vars,_tmp6FC.f3=type_args,_tmp6FC.f4=fields;_tmp6FC;});
return((void*(*)(struct _tuple26*arg,void*(*f)(struct _RegionHandle*,struct _tuple27*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 692
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp768=({struct _dyneither_ptr*_tmp121=_cycalloc(sizeof(*_tmp121));({struct _dyneither_ptr _tmp767=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp120=({struct Cyc_Int_pa_PrintArg_struct _tmp6FD;_tmp6FD.tag=1U,_tmp6FD.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp6FD;});void*_tmp11E[1U];_tmp11E[0]=& _tmp120;({struct _dyneither_ptr _tmp766=({const char*_tmp11F="_tmp%X";_tag_dyneither(_tmp11F,sizeof(char),7U);});Cyc_aprintf(_tmp766,_tag_dyneither(_tmp11E,sizeof(void*),1U));});});*_tmp121=_tmp767;});_tmp121;});_tmp122->f2=_tmp768;});_tmp122;});}struct _tuple29{struct Cyc_Toc_TocState*f1;int f2;};
# 698
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple29*env){
struct _tuple29 _tmp123=*env;struct _tuple29 _tmp124=_tmp123;struct Cyc_Xarray_Xarray*_tmp12C;_LL1: _tmp12C=(_tmp124.f1)->temp_labels;_LL2:;{
int _tmp125=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp769=_tmp125;_tmp769 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12C);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12C,_tmp125);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12B=_cycalloc(sizeof(*_tmp12B));({struct _dyneither_ptr _tmp76B=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12A=({struct Cyc_Int_pa_PrintArg_struct _tmp6FE;_tmp6FE.tag=1U,_tmp6FE.f1=(unsigned int)_tmp125;_tmp6FE;});void*_tmp128[1U];_tmp128[0]=& _tmp12A;({struct _dyneither_ptr _tmp76A=({const char*_tmp129="_LL%X";_tag_dyneither(_tmp129,sizeof(char),6U);});Cyc_aprintf(_tmp76A,_tag_dyneither(_tmp128,sizeof(void*),1U));});});*_tmp12B=_tmp76B;});_tmp12B;});
if(({int _tmp76C=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12C,res);_tmp76C != _tmp125;}))
({void*_tmp126=0U;({struct _dyneither_ptr _tmp76D=({const char*_tmp127="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp127,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp76D,_tag_dyneither(_tmp126,sizeof(void*),0U));});});
return res;};};}
# 709
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple29*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 715
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp12D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp12D))->hd)->name)!= 0){
++ ans;
_tmp12D=_tmp12D->tl;}
# 722
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 728
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp12E=*a;struct _tuple9 _tmp12F=_tmp12E;struct _dyneither_ptr*_tmp133;struct Cyc_Absyn_Tqual _tmp132;void*_tmp131;_LL1: _tmp133=_tmp12F.f1;_tmp132=_tmp12F.f2;_tmp131=_tmp12F.f3;_LL2:;
return({struct _tuple9*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->f1=_tmp133,_tmp130->f2=_tmp132,({void*_tmp76E=Cyc_Toc_typ_to_c(_tmp131);_tmp130->f3=_tmp76E;});_tmp130;});}
# 751 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp134=Cyc_Tcutil_compress(t);void*_tmp135=_tmp134;void*_tmp13B;void*_tmp13A;struct Cyc_Absyn_Tqual _tmp139;struct Cyc_Absyn_Exp*_tmp138;void*_tmp137;unsigned int _tmp136;switch(*((int*)_tmp135)){case 4U: _LL1: _tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).elt_type;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).tq;_tmp138=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).num_elts;_tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zero_term;_tmp136=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zt_loc;_LL2:
# 754
 return({void*_tmp76F=Cyc_Toc_typ_to_c_array(_tmp13A);Cyc_Absyn_cstar_type(_tmp76F,_tmp139);});case 1U: _LL3: _tmp13B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp135)->f2;if(_tmp13B != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13B);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 760
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 762
struct Cyc_Absyn_Aggrfield*_tmp13C=({struct Cyc_Absyn_Aggrfield*_tmp13D=_cycalloc(sizeof(*_tmp13D));*_tmp13D=*f;_tmp13D;});
_tmp13C->type=new_type;
_tmp13C->requires_clause=0;
_tmp13C->tq=Cyc_Toc_mt_tq;
return _tmp13C;}
# 769
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 771
return;}
# 774
static void*Cyc_Toc_char_star_type(){
static void**cs=0;
if(cs == 0)
({void**_tmp771=({void**_tmp13E=_cycalloc(sizeof(*_tmp13E));({void*_tmp770=Cyc_Absyn_star_type(Cyc_Absyn_char_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type);*_tmp13E=_tmp770;});_tmp13E;});cs=_tmp771;});
return*cs;}
# 780
static void*Cyc_Toc_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp774=({void**_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp773=({void*_tmp772=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp772,Cyc_Toc_mt_tq);});*_tmp13F=_tmp773;});_tmp13F;});r=_tmp774;});
return*r;}
# 786
static void*Cyc_Toc_dyn_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp777=({void**_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp776=({void*_tmp775=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_type(_tmp775,Cyc_Toc_mt_tq);});*_tmp140=_tmp776;});_tmp140;});r=_tmp777;});
return*r;}
# 792
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp141=Cyc_Tcutil_compress(t);void*_tmp142=_tmp141;struct Cyc_Absyn_Tvar*_tmp143;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->tag == 2U){_LL1: _tmp143=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 798
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp144=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp145=_tmp144;if(((struct Cyc_Absyn_Kind*)_tmp145)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 805
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp146=t;struct Cyc_Absyn_Datatypedecl*_tmp18D;void**_tmp18C;struct Cyc_Absyn_Enumdecl*_tmp18B;struct Cyc_Absyn_Aggrdecl*_tmp18A;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp188;struct _tuple1*_tmp187;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Typedefdecl*_tmp185;void*_tmp184;enum Cyc_Absyn_AggrKind _tmp183;struct Cyc_List_List*_tmp182;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Tqual _tmp180;void*_tmp17F;struct Cyc_List_List*_tmp17E;int _tmp17D;struct Cyc_Absyn_VarargInfo*_tmp17C;struct Cyc_List_List*_tmp17B;void*_tmp17A;struct Cyc_Absyn_Tqual _tmp179;struct Cyc_Absyn_Exp*_tmp178;unsigned int _tmp177;void*_tmp176;struct Cyc_Absyn_Tqual _tmp175;void*_tmp174;struct Cyc_Absyn_Tvar*_tmp173;void**_tmp172;struct Cyc_List_List*_tmp171;struct _tuple1*_tmp170;union Cyc_Absyn_AggrInfo _tmp16F;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Datatypedecl*_tmp16D;struct Cyc_Absyn_Datatypefield*_tmp16C;switch(*((int*)_tmp146)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 820
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp16D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).val).f1;_tmp16C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).val).f2;_LLA:
# 822
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp16C->name,_tmp16D->name));}else{_LLB: _LLC:
# 824
({void*_tmp147=0U;({struct _dyneither_ptr _tmp778=({const char*_tmp148="unresolved DatatypeFieldType";_tag_dyneither(_tmp148,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp778,_tag_dyneither(_tmp147,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 833
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp16F=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_tmp16E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f2;_LL1C:
# 880
{union Cyc_Absyn_AggrInfo _tmp158=_tmp16F;if((_tmp158.UnknownAggr).tag == 1){_LL45: _LL46:
 return t;}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 884
struct Cyc_Absyn_Aggrdecl*_tmp159=Cyc_Absyn_get_known_aggrdecl(_tmp16F);
if(_tmp159->expected_mem_kind){
# 887
if(_tmp159->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp700;_tmp700.tag=0U,({
struct _dyneither_ptr _tmp779=(struct _dyneither_ptr)(_tmp159->kind == Cyc_Absyn_UnionA?({const char*_tmp15E="union";_tag_dyneither(_tmp15E,sizeof(char),6U);}):({const char*_tmp15F="struct";_tag_dyneither(_tmp15F,sizeof(char),7U);}));_tmp700.f1=_tmp779;});_tmp700;});struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp6FF;_tmp6FF.tag=0U,({
struct _dyneither_ptr _tmp77A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp6FF.f1=_tmp77A;});_tmp6FF;});void*_tmp15A[2U];_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D;({struct _dyneither_ptr _tmp77B=({const char*_tmp15B="%s %s was never defined.";_tag_dyneither(_tmp15B,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp77B,_tag_dyneither(_tmp15A,sizeof(void*),2U));});});}
# 895
if(_tmp159->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp160=_tmp159->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp159->impl))->fields;
if(_tmp160 == 0)return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp160))->tl != 0;_tmp160=_tmp160->tl){;}{
void*_tmp161=((struct Cyc_Absyn_Aggrfield*)_tmp160->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp161))){
if(_tmp159->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp701;_tmp701.tag=0U,({struct _dyneither_ptr _tmp77C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp701.f1=_tmp77C;});_tmp701;});void*_tmp162[1U];_tmp162[0]=& _tmp164;({struct _dyneither_ptr _tmp77D=({const char*_tmp163="struct %s ended up being abstract.";_tag_dyneither(_tmp163,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp77D,_tag_dyneither(_tmp162,sizeof(void*),1U));});});{
# 908
struct _RegionHandle _tmp165=_new_region("r");struct _RegionHandle*r=& _tmp165;_push_region(r);
{struct Cyc_List_List*_tmp166=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp159->tvs,_tmp16E);
void*_tmp167=Cyc_Tcutil_rsubstitute(r,_tmp166,_tmp161);
if(Cyc_Toc_is_abstract_type(_tmp167)){void*_tmp168=Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp168;}{
void*_tmp169=Cyc_Toc_add_struct_type(_tmp159->name,_tmp159->tvs,_tmp16E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp159->impl))->fields);_npop_handler(0U);return _tmp169;};}
# 909
;_pop_region(r);};}
# 914
return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);};};};case 15U: _LL1D: _tmp170=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_LL1E:
 return t;case 16U: _LL1F: _tmp171=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp171);return t;case 4U: _LL23: _LL24:
# 926
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 931
 return Cyc_Absyn_void_star_type();}case 1U: _LL3: _tmp172=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp146)->f2;_LL4:
# 809
 if(*_tmp172 == 0){
*_tmp172=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 813
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp172));case 2U: _LL5: _tmp173=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->f1;_LL6:
# 815
 if((Cyc_Tcutil_tvar_kind(_tmp173,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 817
return Cyc_Absyn_void_type;else{
# 819
return Cyc_Absyn_void_star_type();}case 3U: _LLD: _tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp175=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_tq;_tmp174=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).ptr_atts).bounds;_LLE:
# 828
({void*_tmp77E=Cyc_Toc_typ_to_c(_tmp176);_tmp176=_tmp77E;});
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp174)== 0)
return Cyc_Toc_dyneither_ptr_type();else{
# 832
return Cyc_Absyn_star_type(_tmp176,Cyc_Absyn_heap_rgn_type,_tmp175,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).tq;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).num_elts;_tmp177=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).zt_loc;_LL14:
# 836
 return({void*_tmp782=Cyc_Toc_typ_to_c(_tmp17A);struct Cyc_Absyn_Tqual _tmp781=_tmp179;struct Cyc_Absyn_Exp*_tmp780=_tmp178;void*_tmp77F=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp782,_tmp781,_tmp780,_tmp77F,_tmp177);});case 5U: _LL15: _tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_tqual;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_type;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).args;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).c_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).cyc_varargs;_tmp17B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).attributes;_LL16: {
# 842
struct Cyc_List_List*_tmp149=0;
for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
void*_tmp14A=(void*)_tmp17B->hd;void*_tmp14B=_tmp14A;switch(*((int*)_tmp14B)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
({struct Cyc_List_List*_tmp783=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=(void*)_tmp17B->hd,_tmp14C->tl=_tmp149;_tmp14C;});_tmp149=_tmp783;});goto _LL35;}_LL35:;}{
# 853
struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17E);
if(_tmp17C != 0){
# 856
void*_tmp14E=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp17C->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp14F=({struct _tuple9*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->f1=_tmp17C->name,_tmp151->f2=_tmp17C->tq,_tmp151->f3=_tmp14E;_tmp151;});
({struct Cyc_List_List*_tmp785=({struct Cyc_List_List*_tmp784=_tmp14D;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp784,({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->hd=_tmp14F,_tmp150->tl=0;_tmp150;}));});_tmp14D=_tmp785;});}
# 860
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->tag=5U,(_tmp152->f1).tvars=0,(_tmp152->f1).effect=0,(_tmp152->f1).ret_tqual=_tmp180,({void*_tmp786=Cyc_Toc_typ_to_c(_tmp17F);(_tmp152->f1).ret_type=_tmp786;}),(_tmp152->f1).args=_tmp14D,(_tmp152->f1).c_varargs=_tmp17D,(_tmp152->f1).cyc_varargs=0,(_tmp152->f1).rgn_po=0,(_tmp152->f1).attributes=_tmp149,(_tmp152->f1).requires_clause=0,(_tmp152->f1).requires_relns=0,(_tmp152->f1).ensures_clause=0,(_tmp152->f1).ensures_relns=0;_tmp152;});};}case 6U: _LL17: _tmp181=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp146)->f1;_LL18: {
# 865
struct Cyc_List_List*_tmp153=0;
for(0;_tmp181 != 0;_tmp181=_tmp181->tl){
({struct Cyc_List_List*_tmp789=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));({struct _tuple12*_tmp788=({struct _tuple12*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->f1=(*((struct _tuple12*)_tmp181->hd)).f1,({void*_tmp787=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp181->hd)).f2);_tmp154->f2=_tmp787;});_tmp154;});_tmp155->hd=_tmp788;}),_tmp155->tl=_tmp153;_tmp155;});_tmp153=_tmp789;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp153));}case 7U: _LL19: _tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f1;_tmp182=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f2;_LL1A: {
# 874
struct Cyc_List_List*_tmp156=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp78C=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*_tmp157));({struct Cyc_Absyn_Aggrfield*_tmp78B=({struct Cyc_Absyn_Aggrfield*_tmp78A=(struct Cyc_Absyn_Aggrfield*)_tmp182->hd;Cyc_Toc_aggrfield_to_c(_tmp78A,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp182->hd)->type));});_tmp157->hd=_tmp78B;}),_tmp157->tl=_tmp156;_tmp157;});_tmp156=_tmp78C;});}
return({enum Cyc_Absyn_AggrKind _tmp78D=_tmp183;Cyc_Toc_add_anon_aggr_type(_tmp78D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp156));});}case 8U: _LL21: _tmp187=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f1;_tmp186=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f2;_tmp185=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f3;_tmp184=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f4;_LL22:
# 920
 if(_tmp184 == 0){
if(_tmp186 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=8U,_tmp16A->f1=_tmp187,_tmp16A->f2=0,_tmp16A->f3=_tmp185,_tmp16A->f4=0;_tmp16A;});else{
return t;}}else{
# 925
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=8U,_tmp16B->f1=_tmp187,_tmp16B->f2=0,_tmp16B->f3=_tmp185,({void*_tmp78E=Cyc_Toc_typ_to_c(_tmp184);_tmp16B->f4=_tmp78E;});_tmp16B;});}case 9U: _LL2B: _tmp188=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp146)->f1;_LL2C:
# 936
 return t;case 11U: _LL2D: _tmp189=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp146)->f1;_LL2E:
# 941
 if(_tmp189->topt != 0)return Cyc_Toc_typ_to_c((void*)_check_null(_tmp189->topt));else{
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)){case 0U: _LL2F: _tmp18A=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_LL30:
# 944
 Cyc_Toc_aggrdecl_to_c(_tmp18A);
if(_tmp18A->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18A->name,Cyc_Absyn_unionq_type);else{
return Cyc_Toc_aggrdecl_type(_tmp18A->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp18B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_LL32:
# 949
 Cyc_Toc_enumdecl_to_c(_tmp18B);
return t;default: _LL33: _tmp18D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_tmp18C=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f2;_LL34:
# 952
 Cyc_Toc_datatypedecl_to_c(_tmp18D);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp18C)));}}_LL0:;}
# 957
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp18E=t;void*_tmp190;struct Cyc_Absyn_Tqual _tmp18F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->tag == 4U){_LL1: _tmp190=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->f1).elt_type;_tmp18F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->f1).tq;_LL2:
# 960
 return({void*_tmp78F=Cyc_Absyn_star_type(_tmp190,Cyc_Absyn_heap_rgn_type,_tmp18F,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp78F,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 967
static int Cyc_Toc_atomic_type(void*t){
void*_tmp191=Cyc_Tcutil_compress(t);void*_tmp192=_tmp191;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A4;void*_tmp1A3;void*_tmp1A2;struct Cyc_List_List*_tmp1A1;switch(*((int*)_tmp192)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp1A2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1;_tmp1A1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL4: {
# 971
void*_tmp193=_tmp1A2;struct Cyc_Absyn_Datatypedecl*_tmp19D;struct Cyc_Absyn_Datatypefield*_tmp19C;union Cyc_Absyn_AggrInfo _tmp19B;switch(*((int*)_tmp193)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D:
 return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp19B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp193)->f1;_LL25:
# 979
{union Cyc_Absyn_AggrInfo _tmp194=_tmp19B;if((_tmp194.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 983
struct Cyc_Absyn_Aggrdecl*_tmp195=Cyc_Absyn_get_known_aggrdecl(_tmp19B);
if(_tmp195->impl == 0)
return 0;
{struct Cyc_List_List*_tmp196=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp195->impl))->fields;for(0;_tmp196 != 0;_tmp196=_tmp196->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp196->hd)->type))return 0;}}
return 1;};case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp19D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f1;_tmp19C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f2;_LL27:
# 990
{struct Cyc_List_List*_tmp197=_tmp19C->typs;for(0;_tmp197 != 0;_tmp197=_tmp197->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp197->hd)).f2))return 0;}}
return 1;}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp19A=({struct Cyc_String_pa_PrintArg_struct _tmp702;_tmp702.tag=0U,({struct _dyneither_ptr _tmp790=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp702.f1=_tmp790;});_tmp702;});void*_tmp198[1U];_tmp198[0]=& _tmp19A;({struct _dyneither_ptr _tmp791=({const char*_tmp199="atomic_typ:  bad type %s";_tag_dyneither(_tmp199,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp791,_tag_dyneither(_tmp198,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 995
 return 1;case 4U: _LL7: _tmp1A3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192)->f1).elt_type;_LL8:
 return Cyc_Toc_atomic_type(_tmp1A3);case 7U: _LL9: _tmp1A4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp192)->f2;_LLA:
# 1003
 for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp1A4->hd)->type))return 0;}
return 1;case 6U: _LLB: _tmp1A5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp192)->f1;_LLC:
# 1007
 for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp1A5->hd)).f2))return 0;}
return 1;case 3U: _LLD: _LLE:
# 1012
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp703;_tmp703.tag=0U,({struct _dyneither_ptr _tmp792=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp703.f1=_tmp792;});_tmp703;});void*_tmp19E[1U];_tmp19E[0]=& _tmp1A0;({struct _dyneither_ptr _tmp793=({const char*_tmp19F="atomic_typ:  bad type %s";_tag_dyneither(_tmp19F,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp793,_tag_dyneither(_tmp19E,sizeof(void*),1U));});});}_LL0:;}
# 1017
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A6=Cyc_Tcutil_compress(t);void*_tmp1A7=_tmp1A6;void*_tmp1A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A7)->tag == 3U){_LL1: _tmp1A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A7)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_void_type(_tmp1A8);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1024
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1028
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1033
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A9=Cyc_Tcutil_compress(t);void*_tmp1AA=_tmp1A9;struct Cyc_List_List*_tmp1B6;union Cyc_Absyn_AggrInfo _tmp1B5;switch(*((int*)_tmp1AA)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->tag == 20U){_LL1: _tmp1B5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->f1;_LL2: {
# 1036
struct Cyc_Absyn_Aggrdecl*_tmp1AB=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);
if(_tmp1AB->impl == 0)
({void*_tmp1AC=0U;({struct _dyneither_ptr _tmp794=({const char*_tmp1AD="is_poly_field: type missing fields";_tag_dyneither(_tmp1AD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp794,_tag_dyneither(_tmp1AC,sizeof(void*),0U));});});
_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AB->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AA)->f2;_LL4: {
# 1041
struct Cyc_Absyn_Aggrfield*_tmp1AE=Cyc_Absyn_lookup_field(_tmp1B6,f);
if(_tmp1AE == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp704;_tmp704.tag=0U,_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp704;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _dyneither_ptr _tmp795=({const char*_tmp1B0="is_poly_field: bad field %s";_tag_dyneither(_tmp1B0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp795,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AE->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp705;_tmp705.tag=0U,({struct _dyneither_ptr _tmp796=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp705.f1=_tmp796;});_tmp705;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp797=({const char*_tmp1B3="is_poly_field: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp797,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL0:;}
# 1052
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B7=e->r;void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct _dyneither_ptr*_tmp1BF;switch(*((int*)_tmp1B8)){case 21U: _LL1: _tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL2:
# 1055
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C0->topt),_tmp1BF) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL4: {
# 1058
void*_tmp1B9=Cyc_Tcutil_compress((void*)_check_null(_tmp1C2->topt));void*_tmp1BA=_tmp1B9;void*_tmp1BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->tag == 3U){_LL8: _tmp1BE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->f1).elt_type;_LL9:
# 1060
 return Cyc_Toc_is_poly_field(_tmp1BE,_tmp1C1) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp706;_tmp706.tag=0U,({struct _dyneither_ptr _tmp798=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C2->topt)));_tmp706.f1=_tmp798;});_tmp706;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct _dyneither_ptr _tmp799=({const char*_tmp1BC="is_poly_project: bad type %s";_tag_dyneither(_tmp1BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp799,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1063
 return 0;}_LL0:;}
# 1068
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp79A=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp79A,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=s,_tmp1C3->tl=0;_tmp1C3;}),0U);});}
# 1072
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp79B=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp79B,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1076
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
return Cyc_Toc_atomic_type(t)?Cyc_Toc_malloc_atomic(s): Cyc_Toc_malloc_ptr(s);}
# 1080
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp79C=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp79C,({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1084
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp79D=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp79D,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1088
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*allocator=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp79E=allocator;Cyc_Absyn_fncall_exp(_tmp79E,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=s,_tmp1C7[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1093
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp79F=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp79F,({struct Cyc_Absyn_Exp*_tmp1C8[3U];_tmp1C8[0]=rgn,_tmp1C8[1]=s,_tmp1C8[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1097
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp7A0=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp7A0,({struct Cyc_List_List*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->hd=e,_tmp1C9->tl=0;_tmp1C9;}),0U);});}
# 1100
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp7A1=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp7A1,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=e,_tmp1CA->tl=0;_tmp1CA;}),0U);});}
# 1104
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp7A2=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp7A2,({void*_tmp1CB=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CB,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1109
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1117
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1CC=e->r;void*_tmp1CD=_tmp1CC;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CD)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CD)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1122
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp7A4=({struct Cyc_List_List*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));({struct Cyc_Absyn_Decl*_tmp7A3=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->tag=0U,_tmp1CE->f1=vd;_tmp1CE;}),0U);_tmp1CF->hd=_tmp7A3;}),_tmp1CF->tl=Cyc_Toc_result_decls;_tmp1CF;});Cyc_Toc_result_decls=_tmp7A4;});
({struct Cyc_Absyn_Exp*_tmp7A5=Cyc_Absyn_var_exp(x,0U);xexp=_tmp7A5;});
({struct Cyc_Absyn_Exp*_tmp7A6=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp7A6;});
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1130
({struct Cyc_Absyn_Exp*_tmp7A8=({void*_tmp7A7=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp7A7,e);});xexp=_tmp7A8;});
# 1132
({struct Cyc_Absyn_Exp*_tmp7AA=({void*_tmp7A9=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp7A9,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp7AA;});
goto _LL0;}_LL0:;}
# 1135
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1D0[3U];({struct _tuple19*_tmp7AD=({struct _tuple19*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->f1=0,_tmp1D1->f2=xexp;_tmp1D1;});_tmp1D0[0]=_tmp7AD;}),({
struct _tuple19*_tmp7AC=({struct _tuple19*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xexp;_tmp1D2;});_tmp1D0[1]=_tmp7AC;}),({
struct _tuple19*_tmp7AB=({struct _tuple19*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xplussz;_tmp1D3;});_tmp1D0[2]=_tmp7AB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D0,sizeof(struct _tuple19*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1170 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1D5=_region_malloc(r,sizeof(*_tmp1D5));_tmp1D5->break_lab=0,_tmp1D5->continue_lab=0,_tmp1D5->fallthru_info=0,_tmp1D5->toplevel=1,({int*_tmp7AE=({int*_tmp1D4=_region_malloc(r,sizeof(*_tmp1D4));*_tmp1D4=0;_tmp1D4;});_tmp1D5->in_lhs=_tmp7AE;}),_tmp1D5->rgn=r;_tmp1D5;});}
# 1174
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D6=nv;int _tmp1D7;_LL1: _tmp1D7=_tmp1D6->toplevel;_LL2:;
return _tmp1D7;}
# 1178
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D8=e;struct _dyneither_ptr**_tmp1DE;struct _dyneither_ptr**_tmp1DD;struct Cyc_Toc_FallthruInfo*_tmp1DC;int _tmp1DB;int*_tmp1DA;_LL1: _tmp1DE=_tmp1D8->break_lab;_tmp1DD=_tmp1D8->continue_lab;_tmp1DC=_tmp1D8->fallthru_info;_tmp1DB=_tmp1D8->toplevel;_tmp1DA=_tmp1D8->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));_tmp1D9->break_lab=_tmp1DE,_tmp1D9->continue_lab=_tmp1DD,_tmp1D9->fallthru_info=_tmp1DC,_tmp1D9->toplevel=0,_tmp1D9->in_lhs=_tmp1DA,_tmp1D9->rgn=r;_tmp1D9;});}
# 1182
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DF=e;struct _dyneither_ptr**_tmp1E5;struct _dyneither_ptr**_tmp1E4;struct Cyc_Toc_FallthruInfo*_tmp1E3;int _tmp1E2;int*_tmp1E1;_LL1: _tmp1E5=_tmp1DF->break_lab;_tmp1E4=_tmp1DF->continue_lab;_tmp1E3=_tmp1DF->fallthru_info;_tmp1E2=_tmp1DF->toplevel;_tmp1E1=_tmp1DF->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));_tmp1E0->break_lab=_tmp1E5,_tmp1E0->continue_lab=_tmp1E4,_tmp1E0->fallthru_info=_tmp1E3,_tmp1E0->toplevel=1,_tmp1E0->in_lhs=_tmp1E1,_tmp1E0->rgn=r;_tmp1E0;});}
# 1186
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1E6=nv;int*_tmp1E7;_LL1: _tmp1E7=_tmp1E6->in_lhs;_LL2:;
return*_tmp1E7;}
# 1190
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E8=e;int*_tmp1E9;_LL1: _tmp1E9=_tmp1E8->in_lhs;_LL2:;
*_tmp1E9=b;}
# 1195
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EA=e;struct _dyneither_ptr**_tmp1F0;struct _dyneither_ptr**_tmp1EF;struct Cyc_Toc_FallthruInfo*_tmp1EE;int _tmp1ED;int*_tmp1EC;_LL1: _tmp1F0=_tmp1EA->break_lab;_tmp1EF=_tmp1EA->continue_lab;_tmp1EE=_tmp1EA->fallthru_info;_tmp1ED=_tmp1EA->toplevel;_tmp1EC=_tmp1EA->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1EB=_region_malloc(r,sizeof(*_tmp1EB));_tmp1EB->break_lab=_tmp1F0,_tmp1EB->continue_lab=_tmp1EF,_tmp1EB->fallthru_info=_tmp1EE,_tmp1EB->toplevel=_tmp1ED,_tmp1EB->in_lhs=_tmp1EC,_tmp1EB->rgn=r;_tmp1EB;});}
# 1202
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F1=e;struct _dyneither_ptr**_tmp1F7;struct _dyneither_ptr**_tmp1F6;struct Cyc_Toc_FallthruInfo*_tmp1F5;int _tmp1F4;int*_tmp1F3;_LL1: _tmp1F7=_tmp1F1->break_lab;_tmp1F6=_tmp1F1->continue_lab;_tmp1F5=_tmp1F1->fallthru_info;_tmp1F4=_tmp1F1->toplevel;_tmp1F3=_tmp1F1->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1F2=_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2->break_lab=0,_tmp1F2->continue_lab=0,_tmp1F2->fallthru_info=_tmp1F5,_tmp1F2->toplevel=_tmp1F4,_tmp1F2->in_lhs=_tmp1F3,_tmp1F2->rgn=r;_tmp1F2;});}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1212
struct Cyc_Toc_Env*_tmp1F8=e;struct _dyneither_ptr**_tmp200;struct _dyneither_ptr**_tmp1FF;struct Cyc_Toc_FallthruInfo*_tmp1FE;int _tmp1FD;int*_tmp1FC;_LL1: _tmp200=_tmp1F8->break_lab;_tmp1FF=_tmp1F8->continue_lab;_tmp1FE=_tmp1F8->fallthru_info;_tmp1FD=_tmp1F8->toplevel;_tmp1FC=_tmp1F8->in_lhs;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));
_tmp1FB->label=fallthru_l,_tmp1FB->binders=fallthru_binders;_tmp1FB;});
return({struct Cyc_Toc_Env*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));({struct _dyneither_ptr**_tmp7AF=({struct _dyneither_ptr**_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));*_tmp1F9=break_l;_tmp1F9;});_tmp1FA->break_lab=_tmp7AF;}),_tmp1FA->continue_lab=_tmp1FF,_tmp1FA->fallthru_info=fi,_tmp1FA->toplevel=_tmp1FD,_tmp1FA->in_lhs=_tmp1FC,_tmp1FA->rgn=r;_tmp1FA;});};}
# 1217
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1219
struct Cyc_Toc_Env*_tmp201=e;struct _dyneither_ptr**_tmp208;struct _dyneither_ptr**_tmp207;struct Cyc_Toc_FallthruInfo*_tmp206;int _tmp205;int*_tmp204;_LL1: _tmp208=_tmp201->break_lab;_tmp207=_tmp201->continue_lab;_tmp206=_tmp201->fallthru_info;_tmp205=_tmp201->toplevel;_tmp204=_tmp201->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp203=_region_malloc(r,sizeof(*_tmp203));({struct _dyneither_ptr**_tmp7B0=({struct _dyneither_ptr**_tmp202=_region_malloc(r,sizeof(*_tmp202));*_tmp202=break_l;_tmp202;});_tmp203->break_lab=_tmp7B0;}),_tmp203->continue_lab=_tmp207,_tmp203->fallthru_info=0,_tmp203->toplevel=_tmp205,_tmp203->in_lhs=_tmp204,_tmp203->rgn=r;_tmp203;});}
# 1226
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1229
struct Cyc_Toc_Env*_tmp209=e;struct _dyneither_ptr**_tmp210;struct _dyneither_ptr**_tmp20F;struct Cyc_Toc_FallthruInfo*_tmp20E;int _tmp20D;int*_tmp20C;_LL1: _tmp210=_tmp209->break_lab;_tmp20F=_tmp209->continue_lab;_tmp20E=_tmp209->fallthru_info;_tmp20D=_tmp209->toplevel;_tmp20C=_tmp209->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));_tmp20B->break_lab=0,_tmp20B->continue_lab=_tmp20F,({struct Cyc_Toc_FallthruInfo*_tmp7B1=({struct Cyc_Toc_FallthruInfo*_tmp20A=_region_malloc(r,sizeof(*_tmp20A));_tmp20A->label=next_l,_tmp20A->binders=0;_tmp20A;});_tmp20B->fallthru_info=_tmp7B1;}),_tmp20B->toplevel=_tmp20D,_tmp20B->in_lhs=_tmp20C,_tmp20B->rgn=r;_tmp20B;});}
# 1242 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1245
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp211=e->annot;void*_tmp212=_tmp211;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({void*_tmp213=0U;({unsigned int _tmp7B3=e->loc;struct _dyneither_ptr _tmp7B2=({const char*_tmp214="Toc: do_null_check";_tag_dyneither(_tmp214,sizeof(char),19U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp7B3,_tmp7B2,_tag_dyneither(_tmp213,sizeof(void*),0U));});});}}}}}}_LL0:;}
# 1261
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1263
int ans;
int _tmp215=Cyc_Toc_in_lhs(nv);
void*_tmp216=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));
void*_tmp217=Cyc_Toc_typ_to_c(_tmp216);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp218=_tmp216;void*_tmp22C;struct Cyc_Absyn_Tqual _tmp22B;void*_tmp22A;void*_tmp229;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->tag == 3U){_LL1: _tmp22C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->f1).elt_type;_tmp22B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->f1).elt_tq;_tmp22A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->f1).ptr_atts).bounds;_tmp229=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->f1).ptr_atts).zero_term;_LL2:
# 1274
{void*_tmp219=annot;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Exp*_tmp225;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1276
 if(!((unsigned int)({void*_tmp7B4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7B4,_tmp22A);})))
({void*_tmp7B8=({void*_tmp7B7=({void*_tmp7B5=Cyc_Toc_typ_to_c(_tmp22C);Cyc_Absyn_cstar_type(_tmp7B5,_tmp22B);});Cyc_Toc_cast_it_r(_tmp7B7,({
struct Cyc_Absyn_Exp*_tmp7B6=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7B6,Cyc_Toc_curr_sp,0U);}));});
# 1277
ptr->r=_tmp7B8;});
# 1279
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1282
 if(!((unsigned int)({void*_tmp7B9=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7B9,_tmp22A);})))
goto _LLB;
# 1285
({void*_tmp7BD=({void*_tmp7BC=_tmp217;Cyc_Toc_cast_it_r(_tmp7BC,({
struct Cyc_Absyn_Exp*_tmp7BB=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp7BB,({struct Cyc_Absyn_Exp*_tmp21A[1U];({struct Cyc_Absyn_Exp*_tmp7BA=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21A[0]=_tmp7BA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21A,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 1285
ptr->r=_tmp7BD;});
# 1287
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1292
void*ta1=Cyc_Toc_typ_to_c(_tmp22C);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp7C2=({void*_tmp7C1=Cyc_Absyn_cstar_type(ta1,_tmp22B);Cyc_Toc_cast_it_r(_tmp7C1,({
struct Cyc_Absyn_Exp*_tmp7C0=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp7C0,({struct Cyc_Absyn_Exp*_tmp21B[3U];({
struct Cyc_Absyn_Exp*_tmp7BF=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21B[0]=_tmp7BF;}),({
struct Cyc_Absyn_Exp*_tmp7BE=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp21B[1]=_tmp7BE;}),_tmp21B[2]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21B,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 1294
ptr->r=_tmp7C2;});
# 1299
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp225=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->f1;_LLF:
# 1302
 fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp226=_tmp225;goto _LL11;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp226=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->f1;_LL11: {
# 1305
void*ta1=Cyc_Toc_typ_to_c(_tmp22C);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1310
struct _tuple13 _tmp21C=Cyc_Evexp_eval_const_uint_exp(_tmp226);struct _tuple13 _tmp21D=_tmp21C;unsigned int _tmp222;int _tmp221;_LL15: _tmp222=_tmp21D.f1;_tmp221=_tmp21D.f2;_LL16:;
if((!_tmp221  || _tmp222 != 1) || !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1313
({void*_tmp7C7=({void*_tmp7C6=Cyc_Absyn_cstar_type(ta1,_tmp22B);Cyc_Toc_cast_it_r(_tmp7C6,({
struct Cyc_Absyn_Exp*_tmp7C5=fn_e;Cyc_Absyn_fncall_exp(_tmp7C5,({struct Cyc_Absyn_Exp*_tmp21E[4U];({struct Cyc_Absyn_Exp*_tmp7C4=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21E[0]=_tmp7C4;}),_tmp21E[1]=_tmp226,({
struct Cyc_Absyn_Exp*_tmp7C3=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp21E[2]=_tmp7C3;}),_tmp21E[3]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21E,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 1313
ptr->r=_tmp7C7;});
# 1317
ans=1;}else{
# 1320
if(Cyc_Toc_is_zero(_tmp226)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1323
({void*_tmp7CB=({void*_tmp7CA=_tmp217;Cyc_Toc_cast_it_r(_tmp7CA,({
struct Cyc_Absyn_Exp*_tmp7C9=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp7C9,({struct Cyc_Absyn_Exp*_tmp21F[1U];({struct Cyc_Absyn_Exp*_tmp7C8=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp21F[0]=_tmp7C8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21F,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 1323
ptr->r=_tmp7CB;});
# 1326
ans=0;}else{
# 1330
({struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);fn_e=_tmp7CC;});
({void*_tmp7D0=({void*_tmp7CF=Cyc_Absyn_cstar_type(ta1,_tmp22B);Cyc_Toc_cast_it_r(_tmp7CF,({
struct Cyc_Absyn_Exp*_tmp7CE=fn_e;Cyc_Absyn_fncall_exp(_tmp7CE,({struct Cyc_Absyn_Exp*_tmp220[3U];({struct Cyc_Absyn_Exp*_tmp7CD=Cyc_Absyn_new_exp(ptr->r,0U);_tmp220[0]=_tmp7CD;}),_tmp220[1]=_tmp226,_tmp220[2]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp220,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 1331
ptr->r=_tmp7D0;});
# 1334
ans=1;}}
# 1337
goto _LL5;}}else{_LL12: _LL13:
({void*_tmp223=0U;({struct _dyneither_ptr _tmp7D1=({const char*_tmp224="FIX: ptr_use_to_c, bad annotation";_tag_dyneither(_tmp224,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7D1,_tag_dyneither(_tmp223,sizeof(void*),0U));});});}}}}}}_LL5:;}
# 1340
Cyc_Toc_set_lhs(nv,_tmp215);
return ans;}else{_LL3: _LL4:
({void*_tmp227=0U;({struct _dyneither_ptr _tmp7D2=({const char*_tmp228="ptr_use_to_c: non-pointer-type";_tag_dyneither(_tmp228,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7D2,_tag_dyneither(_tmp227,sizeof(void*),0U));});});}_LL0:;};}
# 1346
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp22D=0U;({struct _dyneither_ptr _tmp7D3=({const char*_tmp22E="Missing type in primop ";_tag_dyneither(_tmp22E,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7D3,_tag_dyneither(_tmp22D,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1350
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F->f1=Cyc_Toc_mt_tq,({void*_tmp7D4=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp22F->f2=_tmp7D4;});_tmp22F;});}
# 1355
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp230=e->r;void*_tmp231=_tmp230;struct Cyc_Absyn_Exp*_tmp239;int _tmp238;struct Cyc_Absyn_Exp*_tmp237;int _tmp236;struct Cyc_List_List*_tmp235;switch(*((int*)_tmp231)){case 26U: _LL1: _tmp235=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp231)->f1;_LL2:
# 1358
{struct Cyc_List_List*_tmp232=_tmp235;for(0;_tmp232 != 0;_tmp232=_tmp232->tl){
if((*((struct _tuple19*)_tmp232->hd)).f1 != 0)
({void*_tmp233=0U;({struct _dyneither_ptr _tmp7D5=({const char*_tmp234="array designators for abstract-field initialization";_tag_dyneither(_tmp234,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp7D5,_tag_dyneither(_tmp233,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp7D6=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp235),0U);_tmp237=_tmp7D6;});_tmp236=0;goto _LL4;case 27U: _LL3: _tmp237=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp231)->f2;_tmp236=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp231)->f4;_LL4:
 _tmp239=_tmp237;_tmp238=_tmp236;goto _LL6;case 28U: _LL5: _tmp239=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp231)->f1;_tmp238=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp231)->f3;_LL6:
# 1364
({struct Cyc_Absyn_Exp*_tmp7D7=Cyc_Absyn_copy_exp(_tmp239);_tmp239=_tmp7D7;});
return _tmp238?({struct Cyc_Absyn_Exp*_tmp7D8=_tmp239;Cyc_Absyn_add_exp(_tmp7D8,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp239;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1373
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1381
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp23A=e->r;void*_tmp23B=_tmp23A;struct Cyc_List_List*_tmp23C;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23B)->tag == 29U){_LL1: _tmp23C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23B)->f3;_LL2:
 dles=_tmp23C;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1387
{void*_tmp23D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp23E=_tmp23D;struct Cyc_Absyn_Aggrdecl*_tmp23F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23E)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23E)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23E)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp23F=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23E)->f1)->f1).KnownAggr).val;_LL7:
# 1389
 if(_tmp23F->impl == 0)
return 0;
if(_tmp23F->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23F->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1397
 return 0;}_LL5:;}
# 1399
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp240=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp241=(struct _tuple19*)dles->hd;struct _tuple19*_tmp242=_tmp241;struct Cyc_List_List*_tmp24B;struct Cyc_Absyn_Exp*_tmp24A;_LLB: _tmp24B=_tmp242->f1;_tmp24A=_tmp242->f2;_LLC:;{
struct _dyneither_ptr*_tmp243=Cyc_Absyn_designatorlist_to_fieldname(_tmp24B);
if(!Cyc_strptrcmp(_tmp243,_tmp240->name)){
struct Cyc_Absyn_Exp*_tmp244=Cyc_Toc_get_varsizeexp(_tmp24A);
if(_tmp244 != 0)
return _tmp244;{
void*_tmp245=Cyc_Tcutil_compress(_tmp240->type);void*_tmp246=_tmp245;void*_tmp249;struct Cyc_Absyn_Exp*_tmp248;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp246)->tag == 4U){_LLE: _tmp249=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp246)->f1).elt_type;_tmp248=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp246)->f1).num_elts;_LLF:
# 1414
 if(_tmp248 == 0  || !Cyc_Toc_is_zero(_tmp248))
return 0;
# 1420
return({struct Cyc_Absyn_Exp*_tmp7DB=({struct Cyc_Absyn_Exp*_tmp7DA=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp7DA,({struct Cyc_Absyn_Exp*_tmp247[2U];_tmp247[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp24A)),({
# 1422
struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Absyn_sizeoftype_exp(_tmp249,0U);_tmp247[1]=_tmp7D9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp247,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1420
Cyc_Absyn_add_exp(_tmp7DB,
# 1423
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1428
({void*_tmp24C=0U;({struct _dyneither_ptr _tmp7DC=({const char*_tmp24D="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp24D,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7DC,_tag_dyneither(_tmp24C,sizeof(void*),0U));});});};}
# 1431
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp24E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp24E != 0;_tmp24E=_tmp24E->tl){
struct Cyc_Absyn_Aggrfield*_tmp24F=(struct Cyc_Absyn_Aggrfield*)_tmp24E->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp24F->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1438
({void*_tmp250=0U;({struct _dyneither_ptr _tmp7DE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp253=({struct Cyc_String_pa_PrintArg_struct _tmp707;_tmp707.tag=0U,_tmp707.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp707;});void*_tmp251[1U];_tmp251[0]=& _tmp253;({struct _dyneither_ptr _tmp7DD=({const char*_tmp252="get_member_offset %s failed";_tag_dyneither(_tmp252,sizeof(char),28U);});Cyc_aprintf(_tmp7DD,_tag_dyneither(_tmp251,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7DE,_tag_dyneither(_tmp250,sizeof(void*),0U));});});}struct _tuple30{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1442
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple30*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1445
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1448
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254->tag=5U,_tmp254->f1=e1,_tmp254->f2=incr;_tmp254;}),0U);}
# 1452
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1461
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1463
void*_tmp255=e1->r;void*_tmp256=_tmp255;struct Cyc_Absyn_Exp*_tmp25E;struct _dyneither_ptr*_tmp25D;int _tmp25C;int _tmp25B;void*_tmp25A;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_Absyn_Stmt*_tmp258;switch(*((int*)_tmp256)){case 37U: _LL1: _tmp258=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp258,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp25A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp259=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp259,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp25E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp25D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_tmp25C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f3;_tmp25B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f4;_LL6:
# 1468
 e1->r=_tmp25E->r;
({struct Cyc_Absyn_Exp*_tmp7E1=e1;struct Cyc_List_List*_tmp7E0=({struct Cyc_List_List*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->hd=_tmp25D,_tmp257->tl=fs;_tmp257;});struct Cyc_Absyn_Exp*(*_tmp7DF)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp7E1,_tmp7E0,_tmp7DF,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1476
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1478
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp7E2=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp7E2;});}
({void*_tmp7E3=(f(e1_copy,f_env))->r;e1->r=_tmp7E3;});
goto _LL0;}}_LL0:;}
# 1484
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1486
void*_tmp25F=s->r;void*_tmp260=_tmp25F;struct Cyc_Absyn_Stmt*_tmp267;struct Cyc_Absyn_Decl*_tmp266;struct Cyc_Absyn_Stmt*_tmp265;struct Cyc_Absyn_Exp*_tmp264;switch(*((int*)_tmp260)){case 1U: _LL1: _tmp264=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp260)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp264,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp266=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp260)->f1;_tmp265=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp260)->f2;_LL4:
# 1489
 Cyc_Toc_lvalue_assign_stmt(_tmp265,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp267=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp260)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp267,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp263=({struct Cyc_String_pa_PrintArg_struct _tmp708;_tmp708.tag=0U,({struct _dyneither_ptr _tmp7E4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp708.f1=_tmp7E4;});_tmp708;});void*_tmp261[1U];_tmp261[0]=& _tmp263;({struct _dyneither_ptr _tmp7E5=({const char*_tmp262="lvalue_assign_stmt: %s";_tag_dyneither(_tmp262,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp7E5,_tag_dyneither(_tmp261,sizeof(void*),1U));});});}_LL0:;}
# 1495
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1499
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp268=e->r;void*_tmp269=_tmp268;struct Cyc_Absyn_Stmt*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;void**_tmp26E;struct Cyc_Absyn_Exp**_tmp26D;switch(*((int*)_tmp269)){case 14U: _LL1: _tmp26E=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_tmp26D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp269)->f2;_LL2:
# 1502
({struct Cyc_Absyn_Exp*_tmp7E6=Cyc_Toc_push_address_exp(*_tmp26D);*_tmp26D=_tmp7E6;});
({void*_tmp7E7=Cyc_Absyn_cstar_type(*_tmp26E,Cyc_Toc_mt_tq);*_tmp26E=_tmp7E7;});
return e;case 20U: _LL3: _tmp26F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_LL4:
# 1506
 return _tmp26F;case 37U: _LL5: _tmp270=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_LL6:
# 1510
 Cyc_Toc_push_address_stmt(_tmp270);
return e;default: _LL7: _LL8:
# 1513
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp26C=({struct Cyc_String_pa_PrintArg_struct _tmp709;_tmp709.tag=0U,({struct _dyneither_ptr _tmp7E8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp709.f1=_tmp7E8;});_tmp709;});void*_tmp26A[1U];_tmp26A[0]=& _tmp26C;({struct _dyneither_ptr _tmp7E9=({const char*_tmp26B="can't take & of exp %s";_tag_dyneither(_tmp26B,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7E9,_tag_dyneither(_tmp26A,sizeof(void*),1U));});});}_LL0:;}
# 1517
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp271=s->r;void*_tmp272=_tmp271;struct Cyc_Absyn_Exp**_tmp278;struct Cyc_Absyn_Stmt*_tmp277;struct Cyc_Absyn_Stmt*_tmp276;switch(*((int*)_tmp272)){case 2U: _LL1: _tmp276=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp272)->f2;_LL2:
 _tmp277=_tmp276;goto _LL4;case 12U: _LL3: _tmp277=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp272)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp277);goto _LL0;case 1U: _LL5: _tmp278=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp272)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp7EA=Cyc_Toc_push_address_exp(*_tmp278);*_tmp278=_tmp7EA;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp275=({struct Cyc_String_pa_PrintArg_struct _tmp70A;_tmp70A.tag=0U,({struct _dyneither_ptr _tmp7EB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp70A.f1=_tmp7EB;});_tmp70A;});void*_tmp273[1U];_tmp273[0]=& _tmp275;({struct _dyneither_ptr _tmp7EC=({const char*_tmp274="can't take & of stmt %s";_tag_dyneither(_tmp274,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7EC,_tag_dyneither(_tmp273,sizeof(void*),1U));});});}_LL0:;}
# 1529
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp279=Cyc_Tcutil_compress(t);void*_tmp27A=_tmp279;void*_tmp28B;switch(*((int*)_tmp27A)){case 3U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp7ED=Cyc_Absyn_null_exp(0U);res=_tmp7ED;});goto _LL0;case 0U: _LL3: _tmp28B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f1;_LL4:
# 1534
{void*_tmp27B=_tmp28B;int _tmp287;enum Cyc_Absyn_Sign _tmp286;enum Cyc_Absyn_Sign _tmp285;enum Cyc_Absyn_Sign _tmp284;enum Cyc_Absyn_Sign _tmp283;enum Cyc_Absyn_Sign _tmp282;switch(*((int*)_tmp27B)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f2){case Cyc_Absyn_Char_sz: _LL8: _tmp282=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL9:
({struct Cyc_Absyn_Exp*_tmp7EE=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp282,'\000'),0U);res=_tmp7EE;});goto _LL7;case Cyc_Absyn_Short_sz: _LLA: _tmp283=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LLB:
({struct Cyc_Absyn_Exp*_tmp7EF=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp283,0),0U);res=_tmp7EF;});goto _LL7;case Cyc_Absyn_Int_sz: _LL10: _tmp284=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL11:
# 1539
 _tmp285=_tmp284;goto _LL13;case Cyc_Absyn_Long_sz: _LL12: _tmp285=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL13:
({struct Cyc_Absyn_Exp*_tmp7F0=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp285,0),0U);res=_tmp7F0;});goto _LL7;default: _LL14: _tmp286=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL15:
({struct Cyc_Absyn_Exp*_tmp7F1=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp286,(long long)0),0U);res=_tmp7F1;});goto _LL7;}case 15U: _LLC: _LLD:
# 1537
 goto _LLF;case 16U: _LLE: _LLF:
 _tmp284=1U;goto _LL11;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27B)->f1){case 0U: _LL16: _LL17:
# 1542
({struct Cyc_Absyn_Exp*_tmp7F2=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp27C="0.0F";_tag_dyneither(_tmp27C,sizeof(char),5U);}),0),0U);res=_tmp7F2;});goto _LL7;case 1U: _LL18: _LL19:
({struct Cyc_Absyn_Exp*_tmp7F3=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp27D="0.0";_tag_dyneither(_tmp27D,sizeof(char),4U);}),1),0U);res=_tmp7F3;});goto _LL7;default: _LL1A: _tmp287=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL1B:
({struct Cyc_Absyn_Exp*_tmp7F5=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp7F4=({const char*_tmp27E="0.0L";_tag_dyneither(_tmp27E,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp7F4,_tmp287);}),0U);res=_tmp7F5;});goto _LL7;}default: _LL1C: _LL1D:
({struct Cyc_String_pa_PrintArg_struct _tmp281=({struct Cyc_String_pa_PrintArg_struct _tmp70B;_tmp70B.tag=0U,({struct _dyneither_ptr _tmp7F6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp70B.f1=_tmp7F6;});_tmp70B;});void*_tmp27F[1U];_tmp27F[0]=& _tmp281;({struct _dyneither_ptr _tmp7F7=({const char*_tmp280="found non-zero type %s in generate_zero";_tag_dyneither(_tmp280,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F7,_tag_dyneither(_tmp27F,sizeof(void*),1U));});});}_LL7:;}
# 1547
goto _LL0;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp28A=({struct Cyc_String_pa_PrintArg_struct _tmp70C;_tmp70C.tag=0U,({struct _dyneither_ptr _tmp7F8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp70C.f1=_tmp7F8;});_tmp70C;});void*_tmp288[1U];_tmp288[0]=& _tmp28A;({struct _dyneither_ptr _tmp7F9=({const char*_tmp289="found non-zero type %s in generate_zero";_tag_dyneither(_tmp289,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F9,_tag_dyneither(_tmp288,sizeof(void*),1U));});});}_LL0:;}
# 1550
res->topt=t;
return res;}
# 1557
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1569
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp28C=Cyc_Toc_typ_to_c(elt_type);
void*_tmp28D=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp28E=Cyc_Absyn_cstar_type(_tmp28C,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp28F=({struct Cyc_Core_Opt*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->v=_tmp28E;_tmp2C0;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp290=e1->r;void*_tmp291=_tmp290;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_Absyn_Exp*_tmp295;switch(*((int*)_tmp291)){case 20U: _LL1: _tmp295=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp291)->f1;_LL2:
# 1577
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp7FA=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp295,0,Cyc_Absyn_Other_coercion,0U);_tmp295=_tmp7FA;});
_tmp295->topt=fat_ptr_type;
_tmp295->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1582
Cyc_Toc_exp_to_c(nv,_tmp295);xinit=_tmp295;goto _LL0;case 23U: _LL3: _tmp297=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp291)->f1;_tmp296=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp291)->f2;_LL4:
# 1584
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp7FB=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp297,0,Cyc_Absyn_Other_coercion,0U);_tmp297=_tmp7FB;});
_tmp297->topt=fat_ptr_type;
_tmp297->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1589
Cyc_Toc_exp_to_c(nv,_tmp297);Cyc_Toc_exp_to_c(nv,_tmp296);
({struct Cyc_Absyn_Exp*_tmp7FE=({struct Cyc_Absyn_Exp*_tmp7FD=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp7FD,({struct Cyc_Absyn_Exp*_tmp292[3U];_tmp292[0]=_tmp297,({
struct Cyc_Absyn_Exp*_tmp7FC=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp292[1]=_tmp7FC;}),_tmp292[2]=_tmp296;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp292,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1590
xinit=_tmp7FE;});
# 1592
goto _LL0;default: _LL5: _LL6:
({void*_tmp293=0U;({struct _dyneither_ptr _tmp7FF=({const char*_tmp294="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp294,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7FF,_tag_dyneither(_tmp293,sizeof(void*),0U));});});}_LL0:;}{
# 1595
struct _tuple1*_tmp298=Cyc_Toc_temp_var();
struct _RegionHandle _tmp299=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp299;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp29A=({struct Cyc_Absyn_Vardecl*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->sc=Cyc_Absyn_Public,_tmp2BF->name=_tmp298,_tmp2BF->varloc=0U,_tmp2BF->tq=Cyc_Toc_mt_tq,_tmp2BF->type=_tmp28D,_tmp2BF->initializer=xinit,_tmp2BF->rgn=0,_tmp2BF->attributes=0,_tmp2BF->escapes=0;_tmp2BF;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29B=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->tag=4U,_tmp2BE->f1=_tmp29A;_tmp2BE;});
struct Cyc_Absyn_Exp*_tmp29C=Cyc_Absyn_varb_exp((void*)_tmp29B,0U);
_tmp29C->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp29D=Cyc_Absyn_deref_exp(_tmp29C,0U);
_tmp29D->topt=elt_type;
_tmp29D->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,_tmp29D);{
struct _tuple1*_tmp29E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp29F=({struct Cyc_Absyn_Vardecl*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD->sc=Cyc_Absyn_Public,_tmp2BD->name=_tmp29E,_tmp2BD->varloc=0U,_tmp2BD->tq=Cyc_Toc_mt_tq,_tmp2BD->type=_tmp28C,_tmp2BD->initializer=_tmp29D,_tmp2BD->rgn=0,_tmp2BD->attributes=0,_tmp2BD->escapes=0;_tmp2BD;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A0=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC->tag=4U,_tmp2BC->f1=_tmp29F;_tmp2BC;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2A1=Cyc_Absyn_varb_exp((void*)_tmp2A0,0U);
_tmp2A1->topt=_tmp29D->topt;
({struct Cyc_Absyn_Exp*_tmp802=({enum Cyc_Absyn_Primop _tmp801=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp800=_tmp2A1;Cyc_Absyn_prim2_exp(_tmp801,_tmp800,Cyc_Absyn_copy_exp(e2),0U);});z_init=_tmp802;});
z_init->topt=_tmp2A1->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1616
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2A2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2A3=({struct Cyc_Absyn_Vardecl*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->sc=Cyc_Absyn_Public,_tmp2BB->name=_tmp2A2,_tmp2BB->varloc=0U,_tmp2BB->tq=Cyc_Toc_mt_tq,_tmp2BB->type=_tmp28C,_tmp2BB->initializer=z_init,_tmp2BB->rgn=0,_tmp2BB->attributes=0,_tmp2BB->escapes=0;_tmp2BB;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A4=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA->tag=4U,_tmp2BA->f1=_tmp2A3;_tmp2BA;});
# 1622
struct Cyc_Absyn_Exp*_tmp2A5=Cyc_Absyn_varb_exp((void*)_tmp2A0,0U);_tmp2A5->topt=_tmp29D->topt;{
struct Cyc_Absyn_Exp*_tmp2A6=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2A5,_tmp2A6,0U);
_tmp2A7->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2A7);{
# 1628
struct Cyc_Absyn_Exp*_tmp2A8=Cyc_Absyn_varb_exp((void*)_tmp2A4,0U);_tmp2A8->topt=_tmp29D->topt;{
struct Cyc_Absyn_Exp*_tmp2A9=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2AA=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2A8,_tmp2A9,0U);
_tmp2AA->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2AA);{
# 1634
struct Cyc_List_List*_tmp2AB=({struct Cyc_Absyn_Exp*_tmp2B9[2U];({struct Cyc_Absyn_Exp*_tmp804=Cyc_Absyn_varb_exp((void*)_tmp29B,0U);_tmp2B9[0]=_tmp804;}),({
struct Cyc_Absyn_Exp*_tmp803=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2B9[1]=_tmp803;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2B9,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2AC=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp806=({struct Cyc_Absyn_Exp*_tmp805=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2AB,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp805,_tmp2AC,0U);});xsize=_tmp806;});{
# 1641
struct Cyc_Absyn_Exp*_tmp2AD=({struct Cyc_Absyn_Exp*_tmp807=xsize;Cyc_Absyn_and_exp(_tmp807,Cyc_Absyn_and_exp(_tmp2A7,_tmp2AA,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2AE=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2AF=({struct Cyc_Absyn_Exp*_tmp808=Cyc_Absyn_varb_exp((void*)_tmp29B,0U);Cyc_Toc_member_exp(_tmp808,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp809=Cyc_Toc_cast_it(_tmp28E,_tmp2AF);_tmp2AF=_tmp809;});{
struct Cyc_Absyn_Exp*_tmp2B0=Cyc_Absyn_deref_exp(_tmp2AF,0U);
struct Cyc_Absyn_Exp*_tmp2B1=({struct Cyc_Absyn_Exp*_tmp80A=_tmp2B0;Cyc_Absyn_assign_exp(_tmp80A,Cyc_Absyn_var_exp(_tmp2A2,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2B2=Cyc_Absyn_exp_stmt(_tmp2B1,0U);
({struct Cyc_Absyn_Stmt*_tmp80E=({struct Cyc_Absyn_Stmt*_tmp80D=({struct Cyc_Absyn_Exp*_tmp80C=_tmp2AD;struct Cyc_Absyn_Stmt*_tmp80B=_tmp2AE;Cyc_Absyn_ifthenelse_stmt(_tmp80C,_tmp80B,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp80D,_tmp2B2,0U);});_tmp2B2=_tmp80E;});
({struct Cyc_Absyn_Stmt*_tmp811=({struct Cyc_Absyn_Decl*_tmp810=({struct Cyc_Absyn_Decl*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));({void*_tmp80F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3->tag=0U,_tmp2B3->f1=_tmp2A3;_tmp2B3;});_tmp2B4->r=_tmp80F;}),_tmp2B4->loc=0U;_tmp2B4;});Cyc_Absyn_decl_stmt(_tmp810,_tmp2B2,0U);});_tmp2B2=_tmp811;});
({struct Cyc_Absyn_Stmt*_tmp814=({struct Cyc_Absyn_Decl*_tmp813=({struct Cyc_Absyn_Decl*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));({void*_tmp812=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->tag=0U,_tmp2B5->f1=_tmp29F;_tmp2B5;});_tmp2B6->r=_tmp812;}),_tmp2B6->loc=0U;_tmp2B6;});Cyc_Absyn_decl_stmt(_tmp813,_tmp2B2,0U);});_tmp2B2=_tmp814;});
({struct Cyc_Absyn_Stmt*_tmp817=({struct Cyc_Absyn_Decl*_tmp816=({struct Cyc_Absyn_Decl*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));({void*_tmp815=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->tag=0U,_tmp2B7->f1=_tmp29A;_tmp2B7;});_tmp2B8->r=_tmp815;}),_tmp2B8->loc=0U;_tmp2B8;});Cyc_Absyn_decl_stmt(_tmp816,_tmp2B2,0U);});_tmp2B2=_tmp817;});
({void*_tmp818=Cyc_Toc_stmt_exp_r(_tmp2B2);e->r=_tmp818;});};};};};};};};};};}
# 1597
;_pop_region(rgn2);};}
# 1667 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1671
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2C1=Cyc_Tcutil_compress(aggrtype);void*_tmp2C2=_tmp2C1;union Cyc_Absyn_AggrInfo _tmp2C7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)->tag == 20U){_LL1: _tmp2C7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)->f1;_LL2:
({struct Cyc_Absyn_Aggrdecl*_tmp819=Cyc_Absyn_get_known_aggrdecl(_tmp2C7);ad=_tmp819;});goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp2C3=0U;({struct _dyneither_ptr _tmp81C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C6=({struct Cyc_String_pa_PrintArg_struct _tmp70D;_tmp70D.tag=0U,({
struct _dyneither_ptr _tmp81A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp70D.f1=_tmp81A;});_tmp70D;});void*_tmp2C4[1U];_tmp2C4[0]=& _tmp2C6;({struct _dyneither_ptr _tmp81B=({const char*_tmp2C5="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2C5,sizeof(char),51U);});Cyc_aprintf(_tmp81B,_tag_dyneither(_tmp2C4,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp81C,_tag_dyneither(_tmp2C3,sizeof(void*),0U));});});}_LL0:;}{
# 1677
struct _tuple1*_tmp2C8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2C9=Cyc_Absyn_var_exp(_tmp2C8,0U);
struct Cyc_Absyn_Exp*_tmp2CA=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2CB=Cyc_Absyn_aggrarrow_exp(_tmp2C9,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2CC=Cyc_Absyn_neq_exp(_tmp2CB,_tmp2CA,0U);
struct Cyc_Absyn_Exp*_tmp2CD=Cyc_Absyn_aggrarrow_exp(_tmp2C9,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2CE=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2CD,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2CF=({struct Cyc_Absyn_Exp*_tmp81E=_tmp2CC;struct Cyc_Absyn_Stmt*_tmp81D=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp81E,_tmp81D,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2D0=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2D1=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2D2=({struct _tuple1*_tmp821=_tmp2C8;void*_tmp820=_tmp2D0;struct Cyc_Absyn_Exp*_tmp81F=_tmp2D1;Cyc_Absyn_declare_stmt(_tmp821,_tmp820,_tmp81F,Cyc_Absyn_seq_stmt(_tmp2CF,_tmp2CE,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2D2);}else{
# 1691
struct Cyc_Absyn_Exp*_tmp2D3=({struct Cyc_Absyn_Exp*_tmp822=aggrproj(_tmp2C9,f,0U);Cyc_Toc_member_exp(_tmp822,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2D4=Cyc_Absyn_neq_exp(_tmp2D3,_tmp2CA,0U);
struct Cyc_Absyn_Exp*_tmp2D5=({struct Cyc_Absyn_Exp*_tmp823=aggrproj(_tmp2C9,f,0U);Cyc_Toc_member_exp(_tmp823,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2D6=Cyc_Absyn_exp_stmt(_tmp2D5,0U);
struct Cyc_Absyn_Stmt*_tmp2D7=({struct Cyc_Absyn_Exp*_tmp825=_tmp2D4;struct Cyc_Absyn_Stmt*_tmp824=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp825,_tmp824,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2D8=({struct _tuple1*_tmp828=_tmp2C8;void*_tmp827=e1_c_type;struct Cyc_Absyn_Exp*_tmp826=e1;Cyc_Absyn_declare_stmt(_tmp828,_tmp827,_tmp826,Cyc_Absyn_seq_stmt(_tmp2D7,_tmp2D6,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2D8);}};}
# 1701
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1704
void*_tmp2D9=Cyc_Tcutil_compress(t);void*_tmp2DA=_tmp2D9;union Cyc_Absyn_AggrInfo _tmp2E1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->f1)->tag == 20U){_LL1: _tmp2E1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->f1)->f1;_LL2: {
# 1706
struct Cyc_Absyn_Aggrdecl*_tmp2DB=Cyc_Absyn_get_known_aggrdecl(_tmp2E1);
({int _tmp829=Cyc_Toc_get_member_offset(_tmp2DB,f);*f_tag=_tmp829;});{
# 1709
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DF=({struct Cyc_String_pa_PrintArg_struct _tmp70F;_tmp70F.tag=0U,_tmp70F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2DB->name).f2);_tmp70F;});struct Cyc_String_pa_PrintArg_struct _tmp2E0=({struct Cyc_String_pa_PrintArg_struct _tmp70E;_tmp70E.tag=0U,_tmp70E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp70E;});void*_tmp2DD[2U];_tmp2DD[0]=& _tmp2DF,_tmp2DD[1]=& _tmp2E0;({struct _dyneither_ptr _tmp82A=({const char*_tmp2DE="_union_%s_%s";_tag_dyneither(_tmp2DE,sizeof(char),13U);});Cyc_aprintf(_tmp82A,_tag_dyneither(_tmp2DD,sizeof(void*),2U));});});
({void*_tmp82B=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));*_tmp2DC=str;_tmp2DC;}));*tagged_member_type=_tmp82B;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DB->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1720
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1723
void*_tmp2E2=e->r;void*_tmp2E3=_tmp2E2;struct Cyc_Absyn_Exp*_tmp2EF;struct _dyneither_ptr*_tmp2EE;int*_tmp2ED;struct Cyc_Absyn_Exp*_tmp2EC;struct _dyneither_ptr*_tmp2EB;int*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2E9;switch(*((int*)_tmp2E3)){case 14U: _LL1: _tmp2E9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E3)->f2;_LL2:
({void*_tmp2E4=0U;({struct _dyneither_ptr _tmp82C=({const char*_tmp2E5="cast on lhs!";_tag_dyneither(_tmp2E5,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp82C,_tag_dyneither(_tmp2E4,sizeof(void*),0U));});});case 21U: _LL3: _tmp2EC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E3)->f1;_tmp2EB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E3)->f2;_tmp2EA=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E3)->f4;_LL4:
# 1726
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp2EC->topt),_tmp2EB,f_tag,tagged_member_type,clear_read,_tmp2EA);case 22U: _LL5: _tmp2EF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E3)->f1;_tmp2EE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E3)->f2;_tmp2ED=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E3)->f4;_LL6: {
# 1729
void*_tmp2E6=Cyc_Tcutil_compress((void*)_check_null(_tmp2EF->topt));void*_tmp2E7=_tmp2E6;void*_tmp2E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E7)->tag == 3U){_LLA: _tmp2E8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E7)->f1).elt_type;_LLB:
# 1731
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2E8,_tmp2EE,f_tag,tagged_member_type,clear_read,_tmp2ED);}else{_LLC: _LLD:
# 1733
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1735
 return 0;}_LL0:;}
# 1748 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1752
struct _tuple1*_tmp2F0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp2F0,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp82D=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp82D;});else{
# 1762
struct Cyc_Absyn_Exp*_tmp2F1=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp830=({struct Cyc_Absyn_Exp*_tmp82F=_tmp2F1;struct Cyc_Absyn_Stmt*_tmp82E=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp82F,_tmp82E,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp830;});}{
# 1765
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp833=_tmp2F0;void*_tmp832=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp831=
Cyc_Toc_push_address_exp(e1);
# 1765
Cyc_Absyn_declare_stmt(_tmp833,_tmp832,_tmp831,
# 1767
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple31{void*f1;void*f2;};struct _tuple32{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1771
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp2F4=({struct Cyc_String_pa_PrintArg_struct _tmp710;_tmp710.tag=0U,({struct _dyneither_ptr _tmp834=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp710.f1=_tmp834;});_tmp710;});void*_tmp2F2[1U];_tmp2F2[0]=& _tmp2F4;({unsigned int _tmp836=e->loc;struct _dyneither_ptr _tmp835=({const char*_tmp2F3="exp_to_c: no type for %s";_tag_dyneither(_tmp2F3,sizeof(char),25U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp836,_tmp835,_tag_dyneither(_tmp2F2,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1776
int did_inserted_checks=0;
{void*_tmp2F5=e->annot;void*_tmp2F6=_tmp2F5;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2F6)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2F6)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1782
{void*_tmp2F7=e->r;void*_tmp2F8=_tmp2F7;struct Cyc_Absyn_Stmt*_tmp495;struct Cyc_Absyn_Exp*_tmp494;struct _dyneither_ptr*_tmp493;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp491;int _tmp490;struct Cyc_Absyn_Exp*_tmp48F;void**_tmp48E;struct Cyc_Absyn_Exp*_tmp48D;int _tmp48C;int _tmp48B;struct Cyc_List_List*_tmp48A;struct Cyc_Absyn_Datatypedecl*_tmp489;struct Cyc_Absyn_Datatypefield*_tmp488;struct _tuple1**_tmp487;struct Cyc_List_List*_tmp486;struct Cyc_List_List*_tmp485;struct Cyc_Absyn_Aggrdecl*_tmp484;void*_tmp483;struct Cyc_List_List*_tmp482;struct Cyc_Absyn_Exp*_tmp481;void*_tmp480;int _tmp47F;struct Cyc_Absyn_Vardecl*_tmp47E;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Exp*_tmp47C;int _tmp47B;struct Cyc_List_List*_tmp47A;struct Cyc_List_List*_tmp479;struct Cyc_Absyn_Exp*_tmp478;struct Cyc_Absyn_Exp*_tmp477;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_Absyn_Exp*_tmp475;struct _dyneither_ptr*_tmp474;int _tmp473;int _tmp472;struct Cyc_Absyn_Exp*_tmp471;struct _dyneither_ptr*_tmp470;int _tmp46F;int _tmp46E;void*_tmp46D;struct Cyc_List_List*_tmp46C;void*_tmp46B;struct Cyc_Absyn_Exp*_tmp46A;struct Cyc_Absyn_Exp*_tmp469;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*_tmp467;void**_tmp466;struct Cyc_Absyn_Exp*_tmp465;int _tmp464;enum Cyc_Absyn_Coercion _tmp463;struct Cyc_Absyn_Exp*_tmp462;struct Cyc_List_List*_tmp461;struct Cyc_Absyn_Exp*_tmp460;struct Cyc_Absyn_Exp*_tmp45F;int _tmp45E;struct Cyc_Absyn_Exp*_tmp45D;struct Cyc_List_List*_tmp45C;int _tmp45B;struct Cyc_List_List*_tmp45A;struct Cyc_Absyn_VarargInfo*_tmp459;struct Cyc_Absyn_Exp*_tmp458;struct Cyc_List_List*_tmp457;struct Cyc_Absyn_Exp*_tmp456;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Exp*_tmp454;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_Absyn_Exp*_tmp452;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Core_Opt*_tmp44C;struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_Absyn_Exp*_tmp44A;enum Cyc_Absyn_Incrementor _tmp449;enum Cyc_Absyn_Primop _tmp448;struct Cyc_List_List*_tmp447;struct Cyc_Absyn_Exp*_tmp446;switch(*((int*)_tmp2F8)){case 2U: _LL8: _LL9:
# 1784
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1791
struct Cyc_Absyn_Exp*_tmp2F9=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp837=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp2F9,_tmp2F9))->r;e->r=_tmp837;});else{
# 1796
({void*_tmp839=({struct Cyc_Absyn_Exp*_tmp838=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp838,({struct Cyc_Absyn_Exp*_tmp2FA[3U];_tmp2FA[0]=_tmp2F9,_tmp2FA[1]=_tmp2F9,_tmp2FA[2]=_tmp2F9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2FA,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp839;});}}else{
# 1798
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1800
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 41U: _LL10: _tmp446=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL11:
 Cyc_Toc_exp_to_c(nv,_tmp446);goto _LL7;case 3U: _LL12: _tmp448=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp447=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL13: {
# 1806
struct Cyc_List_List*_tmp2FB=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp447);
# 1808
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp447);
{enum Cyc_Absyn_Primop _tmp2FC=_tmp448;switch(_tmp2FC){case Cyc_Absyn_Numelts: _LL61: _LL62: {
# 1811
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447))->hd;
{void*_tmp2FD=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp2FE=_tmp2FD;void*_tmp30C;void*_tmp30B;void*_tmp30A;void*_tmp309;struct Cyc_Absyn_Exp*_tmp308;switch(*((int*)_tmp2FE)){case 4U: _LL76: _tmp308=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2FE)->f1).num_elts;_LL77:
# 1816
 e->r=((struct Cyc_Absyn_Exp*)_check_null(_tmp308))->r;
goto _LL75;case 3U: _LL78: _tmp30C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).elt_type;_tmp30B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).nullable;_tmp30A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).bounds;_tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).zero_term;_LL79: {
# 1819
struct Cyc_Absyn_Exp*_tmp2FF=({void*_tmp83A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp83A,_tmp30A);});
if(_tmp2FF == 0)
# 1822
({void*_tmp83D=({struct Cyc_Absyn_Exp*_tmp83C=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp83C,({struct Cyc_Absyn_Exp*_tmp300[2U];_tmp300[0]=(struct Cyc_Absyn_Exp*)_tmp447->hd,({
# 1824
struct Cyc_Absyn_Exp*_tmp83B=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp30C),0U);_tmp300[1]=_tmp83B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp300,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1822
e->r=_tmp83D;});else{
# 1826
struct Cyc_Absyn_Exp*_tmp301=_tmp2FF;
# 1828
if(Cyc_Tcutil_force_type2bool(0,_tmp309)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp447->hd);
# 1832
({void*_tmp83F=({struct Cyc_Absyn_Exp*_tmp83E=function_e;Cyc_Toc_fncall_exp_r(_tmp83E,({struct Cyc_Absyn_Exp*_tmp302[2U];_tmp302[0]=(struct Cyc_Absyn_Exp*)_tmp447->hd,_tmp302[1]=_tmp301;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp302,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp83F;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp30B)){
if(!Cyc_Evexp_c_can_eval(_tmp301))
({void*_tmp303=0U;({unsigned int _tmp841=e->loc;struct _dyneither_ptr _tmp840=({const char*_tmp304="can't calculate numelts";_tag_dyneither(_tmp304,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp841,_tmp840,_tag_dyneither(_tmp303,sizeof(void*),0U));});});
# 1837
({void*_tmp844=({struct Cyc_Absyn_Exp*_tmp843=arg;struct Cyc_Absyn_Exp*_tmp842=_tmp301;Cyc_Toc_conditional_exp_r(_tmp843,_tmp842,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp844;});}else{
# 1839
e->r=_tmp301->r;goto _LL75;}}}
# 1842
goto _LL75;}default: _LL7A: _LL7B:
({struct Cyc_String_pa_PrintArg_struct _tmp307=({struct Cyc_String_pa_PrintArg_struct _tmp711;_tmp711.tag=0U,({
struct _dyneither_ptr _tmp845=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp711.f1=_tmp845;});_tmp711;});void*_tmp305[1U];_tmp305[0]=& _tmp307;({struct _dyneither_ptr _tmp846=({const char*_tmp306="numelts primop applied to non-array %s";_tag_dyneither(_tmp306,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp846,_tag_dyneither(_tmp305,sizeof(void*),1U));});});}_LL75:;}
# 1846
goto _LL60;}case Cyc_Absyn_Plus: _LL63: _LL64:
# 1851
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp30D=0U;({struct _dyneither_ptr _tmp847=({const char*_tmp30E="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp30E,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp847,_tag_dyneither(_tmp30D,sizeof(void*),0U));});});
{void*_tmp30F=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd);void*_tmp310=_tmp30F;void*_tmp316;void*_tmp315;void*_tmp314;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->tag == 3U){_LL7D: _tmp316=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).elt_type;_tmp315=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).ptr_atts).bounds;_tmp314=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).ptr_atts).zero_term;_LL7E: {
# 1855
struct Cyc_Absyn_Exp*_tmp311=({void*_tmp848=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp848,_tmp315);});
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447->tl))->hd;
if(_tmp311 == 0)
({void*_tmp84B=({struct Cyc_Absyn_Exp*_tmp84A=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp84A,({struct Cyc_Absyn_Exp*_tmp312[3U];_tmp312[0]=e1,({
struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp316),0U);_tmp312[1]=_tmp849;}),_tmp312[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp312,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1859
e->r=_tmp84B;});else{
# 1861
if(Cyc_Tcutil_force_type2bool(0,_tmp314))
({void*_tmp84D=({struct Cyc_Absyn_Exp*_tmp84C=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp84C,({struct Cyc_Absyn_Exp*_tmp313[3U];_tmp313[0]=e1,_tmp313[1]=_tmp311,_tmp313[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp313,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp84D;});}
goto _LL7C;}}else{_LL7F: _LL80:
 goto _LL7C;}_LL7C:;}
# 1866
goto _LL60;case Cyc_Absyn_Minus: _LL65: _LL66: {
# 1871
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp2FB->tl))->hd)){
({void*_tmp84F=({struct Cyc_Absyn_Exp*_tmp84E=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp84E,Cyc_Toc_curr_sp);});e1->r=_tmp84F;});
({void*_tmp851=({struct Cyc_Absyn_Exp*_tmp850=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp850,Cyc_Toc_curr_sp);});e2->r=_tmp851;});
({void*_tmp853=({void*_tmp852=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp852;});e1->topt=_tmp853;});
({void*_tmp855=({struct Cyc_Absyn_Exp*_tmp854=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp854,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1879
e->r=_tmp855;});}else{
# 1882
({void*_tmp859=({struct Cyc_Absyn_Exp*_tmp858=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp858,({struct Cyc_Absyn_Exp*_tmp317[3U];_tmp317[0]=e1,({
struct Cyc_Absyn_Exp*_tmp857=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp317[1]=_tmp857;}),({
struct Cyc_Absyn_Exp*_tmp856=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp317[2]=_tmp856;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp317,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1882
e->r=_tmp859;});}}
# 1886
goto _LL60;}case Cyc_Absyn_Eq: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Neq: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Gt: _LL6B: _LL6C:
 goto _LL6E;case Cyc_Absyn_Gte: _LL6D: _LL6E:
 goto _LL70;case Cyc_Absyn_Lt: _LL6F: _LL70:
 goto _LL72;case Cyc_Absyn_Lte: _LL71: _LL72: {
# 1894
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp2FB->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& elt_type)){
void*_tmp318=({void*_tmp85A=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp85A,Cyc_Toc_mt_tq);});
({void*_tmp85D=({void*_tmp85C=_tmp318;Cyc_Toc_cast_it_r(_tmp85C,({struct Cyc_Absyn_Exp*_tmp85B=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp85B,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp85D;});
e1->topt=_tmp318;}
# 1904
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
void*_tmp319=({void*_tmp85E=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp85E,Cyc_Toc_mt_tq);});
({void*_tmp861=({void*_tmp860=_tmp319;Cyc_Toc_cast_it_r(_tmp860,({struct Cyc_Absyn_Exp*_tmp85F=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp85F,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp861;});
e2->topt=_tmp319;}
# 1909
goto _LL60;}default: _LL73: _LL74:
 goto _LL60;}_LL60:;}
# 1912
goto _LL7;}case 5U: _LL14: _tmp44A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp449=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL15: {
# 1914
void*e2_cyc_typ=(void*)_check_null(_tmp44A->topt);
# 1923 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _dyneither_ptr incr_str=({const char*_tmp32C="increment";_tag_dyneither(_tmp32C,sizeof(char),10U);});
if(_tmp449 == Cyc_Absyn_PreDec  || _tmp449 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp862=({const char*_tmp31A="decrement";_tag_dyneither(_tmp31A,sizeof(char),10U);});incr_str=_tmp862;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp44A,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp31D=({struct Cyc_String_pa_PrintArg_struct _tmp712;_tmp712.tag=0U,_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp712;});void*_tmp31B[1U];_tmp31B[0]=& _tmp31D;({unsigned int _tmp864=e->loc;struct _dyneither_ptr _tmp863=({const char*_tmp31C="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp31C,sizeof(char),74U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp864,_tmp863,_tag_dyneither(_tmp31B,sizeof(void*),1U));});});
# 1931
if(Cyc_Toc_is_tagged_union_project(_tmp44A,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp31E=_tmp449;switch(_tmp31E){case Cyc_Absyn_PreInc: _LL82: _LL83:
 op=0U;goto _LL81;case Cyc_Absyn_PreDec: _LL84: _LL85:
 op=2U;goto _LL81;default: _LL86: _LL87:
({struct Cyc_String_pa_PrintArg_struct _tmp321=({struct Cyc_String_pa_PrintArg_struct _tmp713;_tmp713.tag=0U,_tmp713.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp713;});void*_tmp31F[1U];_tmp31F[0]=& _tmp321;({unsigned int _tmp866=e->loc;struct _dyneither_ptr _tmp865=({const char*_tmp320="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp320,sizeof(char),58U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp866,_tmp865,_tag_dyneither(_tmp31F,sizeof(void*),1U));});});}_LL81:;}
# 1941
({void*_tmp868=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323->tag=4U,_tmp323->f1=_tmp44A,({struct Cyc_Core_Opt*_tmp867=({struct Cyc_Core_Opt*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->v=(void*)op;_tmp322;});_tmp323->f2=_tmp867;}),_tmp323->f3=one;_tmp323;});e->r=_tmp868;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1945
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp44A);
Cyc_Toc_set_lhs(nv,0);{
# 1950
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp449 == Cyc_Absyn_PostInc  || _tmp449 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 1956
if(_tmp449 == Cyc_Absyn_PreDec  || _tmp449 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp86D=({struct Cyc_Absyn_Exp*_tmp86C=fn_e;Cyc_Toc_fncall_exp_r(_tmp86C,({struct Cyc_Absyn_Exp*_tmp324[3U];({struct Cyc_Absyn_Exp*_tmp86B=Cyc_Toc_push_address_exp(_tmp44A);_tmp324[0]=_tmp86B;}),({
struct Cyc_Absyn_Exp*_tmp86A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp324[1]=_tmp86A;}),({
struct Cyc_Absyn_Exp*_tmp869=Cyc_Absyn_signed_int_exp(change,0U);_tmp324[2]=_tmp869;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp324,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1958
e->r=_tmp86D;});}else{
# 1961
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1967
did_inserted_checks=1;
if(_tmp449 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp325=Cyc_Toc_temp_var();
void*_tmp326=({void*_tmp86E=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp86E,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp327=Cyc_Toc_push_address_exp(_tmp44A);
struct Cyc_Absyn_Exp*_tmp328=({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp325,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp86F,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
({struct Cyc_Absyn_Exp*_tmp872=({struct Cyc_Absyn_Exp*_tmp871=({struct Cyc_Absyn_Exp*_tmp870=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp325,0U),0U);Cyc_Absyn_neq_exp(_tmp870,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1975
Cyc_Absyn_and_exp(_tmp871,_tmp328,0U);});_tmp328=_tmp872;});{
# 1977
struct Cyc_Absyn_Stmt*_tmp329=({struct Cyc_Absyn_Exp*_tmp874=_tmp328;struct Cyc_Absyn_Stmt*_tmp873=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp325,0U),0U),0U),0U);
# 1977
Cyc_Absyn_ifthenelse_stmt(_tmp874,_tmp873,
# 1979
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp876=({struct Cyc_Absyn_Stmt*_tmp875=_tmp329;Cyc_Absyn_seq_stmt(_tmp875,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp325,0U),0U),0U),0U);});_tmp329=_tmp876;});
({void*_tmp877=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp325,_tmp326,_tmp327,_tmp329,0U),0U))->r;e->r=_tmp877;});};}else{
# 1984
struct Cyc_Toc_functionSet*_tmp32A=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp32A,_tmp44A);
({void*_tmp87B=({struct Cyc_Absyn_Exp*_tmp87A=fn_e;Cyc_Toc_fncall_exp_r(_tmp87A,({struct Cyc_Absyn_Exp*_tmp32B[2U];({struct Cyc_Absyn_Exp*_tmp879=Cyc_Toc_push_address_exp(_tmp44A);_tmp32B[0]=_tmp879;}),({
struct Cyc_Absyn_Exp*_tmp878=Cyc_Absyn_signed_int_exp(1,0U);_tmp32B[1]=_tmp878;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32B,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1986
e->r=_tmp87B;});}}else{
# 1989
if(elt_typ == Cyc_Absyn_void_type  && !Cyc_Absyn_is_lvalue(_tmp44A)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp44A,0,Cyc_Toc_incr_lvalue,_tmp449);
e->r=_tmp44A->r;}}}
# 1993
goto _LL7;};}case 4U: _LL16: _tmp44D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp44C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp44B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_LL17: {
# 2012 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp44D->topt);
void*e2_old_typ=(void*)_check_null(_tmp44B->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp44D,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp44D);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp44B);
({void*_tmp87C=Cyc_Toc_tagged_union_assignop(_tmp44D,e1_old_typ,_tmp44C,_tmp44B,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp87C;});
# 2023
goto _LL7;}{
# 2025
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp44D,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp44D,_tmp44C,_tmp44B,ptr_type,is_dyneither,elt_type);
# 2031
return;}{
# 2035
int e1_poly=Cyc_Toc_is_poly_project(_tmp44D);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp44D);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp44B);{
# 2041
int done=0;
if(_tmp44C != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp32D=(enum Cyc_Absyn_Primop)_tmp44C->v;enum Cyc_Absyn_Primop _tmp32E=_tmp32D;switch(_tmp32E){case Cyc_Absyn_Plus: _LL89: _LL8A:
 change=_tmp44B;goto _LL88;case Cyc_Absyn_Minus: _LL8B: _LL8C:
({struct Cyc_Absyn_Exp*_tmp87D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp44B,0U);change=_tmp87D;});goto _LL88;default: _LL8D: _LL8E:
({void*_tmp32F=0U;({struct _dyneither_ptr _tmp87E=({const char*_tmp330="bad t ? pointer arithmetic";_tag_dyneither(_tmp330,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp87E,_tag_dyneither(_tmp32F,sizeof(void*),0U));});});}_LL88:;}
# 2051
done=1;{
# 2053
struct Cyc_Absyn_Exp*_tmp331=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp882=({struct Cyc_Absyn_Exp*_tmp881=_tmp331;Cyc_Toc_fncall_exp_r(_tmp881,({struct Cyc_Absyn_Exp*_tmp332[3U];({struct Cyc_Absyn_Exp*_tmp880=Cyc_Toc_push_address_exp(_tmp44D);_tmp332[0]=_tmp880;}),({
struct Cyc_Absyn_Exp*_tmp87F=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp332[1]=_tmp87F;}),_tmp332[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp332,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2054
e->r=_tmp882;});};}else{
# 2057
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2060
enum Cyc_Absyn_Primop _tmp333=(enum Cyc_Absyn_Primop)_tmp44C->v;enum Cyc_Absyn_Primop _tmp334=_tmp333;if(_tmp334 == Cyc_Absyn_Plus){_LL90: _LL91:
# 2062
 done=1;
({void*_tmp884=({struct Cyc_Absyn_Exp*_tmp883=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp44D);Cyc_Toc_fncall_exp_r(_tmp883,({struct Cyc_Absyn_Exp*_tmp335[2U];_tmp335[0]=_tmp44D,_tmp335[1]=_tmp44B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp335,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp884;});
goto _LL8F;}else{_LL92: _LL93:
({void*_tmp336=0U;({struct _dyneither_ptr _tmp885=({const char*_tmp337="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp337,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp885,_tag_dyneither(_tmp336,sizeof(void*),0U));});});}_LL8F:;}}}
# 2069
if(!done){
# 2071
if(e1_poly)
({void*_tmp887=({void*_tmp886=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp886,Cyc_Absyn_new_exp(_tmp44B->r,0U));});_tmp44B->r=_tmp887;});
# 2077
if(!Cyc_Absyn_is_lvalue(_tmp44D)){
({struct Cyc_Absyn_Exp*_tmp888=_tmp44D;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple30*),struct _tuple30*f_env))Cyc_Toc_lvalue_assign)(_tmp888,0,Cyc_Toc_assignop_lvalue,({struct _tuple30*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338->f1=_tmp44C,_tmp338->f2=_tmp44B;_tmp338;}));});
e->r=_tmp44D->r;}}
# 2082
goto _LL7;};};};}case 6U: _LL18: _tmp450=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp44F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp44E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_LL19:
# 2084
 Cyc_Toc_exp_to_c(nv,_tmp450);
Cyc_Toc_exp_to_c(nv,_tmp44F);
Cyc_Toc_exp_to_c(nv,_tmp44E);
goto _LL7;case 7U: _LL1A: _tmp452=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp451=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL1B:
 _tmp454=_tmp452;_tmp453=_tmp451;goto _LL1D;case 8U: _LL1C: _tmp454=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp453=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL1D:
 _tmp456=_tmp454;_tmp455=_tmp453;goto _LL1F;case 9U: _LL1E: _tmp456=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp455=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL1F:
# 2091
 Cyc_Toc_exp_to_c(nv,_tmp456);
Cyc_Toc_exp_to_c(nv,_tmp455);
goto _LL7;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3 == 0){_LL20: _tmp458=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp457=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL21: {
# 2095
void*e1_typ=(void*)_check_null(_tmp458->topt);
Cyc_Toc_exp_to_c(nv,_tmp458);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2100
({void*_tmp88C=({void*_tmp88B=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp88B,({
struct Cyc_Absyn_Exp*_tmp88A=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp88A,({struct Cyc_List_List*_tmp339=_cycalloc(sizeof(*_tmp339));
({struct Cyc_Absyn_Exp*_tmp889=Cyc_Absyn_copy_exp(_tmp458);_tmp339->hd=_tmp889;}),_tmp339->tl=0;_tmp339;}),0U);}));});
# 2100
_tmp458->r=_tmp88C;});
# 2103
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp457);
goto _LL7;}}else{_LL22: _tmp45D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp45C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp45B=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3)->num_varargs;_tmp45A=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3)->injectors;_tmp459=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3)->vai;_LL23: {
# 2114 "toc.cyc"
struct _RegionHandle _tmp33A=_new_region("r");struct _RegionHandle*r=& _tmp33A;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp45B,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp459->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2122
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp45C);
int num_normargs=num_args - _tmp45B;
# 2126
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp45C=_tmp45C->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45C))->hd);
({struct Cyc_List_List*_tmp88D=({struct Cyc_List_List*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B->hd=(struct Cyc_Absyn_Exp*)_tmp45C->hd,_tmp33B->tl=new_args;_tmp33B;});new_args=_tmp88D;});}}
# 2131
({struct Cyc_List_List*_tmp891=({struct Cyc_List_List*_tmp33D=_cycalloc(sizeof(*_tmp33D));({struct Cyc_Absyn_Exp*_tmp890=({struct Cyc_Absyn_Exp*_tmp88F=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp88F,({struct Cyc_Absyn_Exp*_tmp33C[3U];_tmp33C[0]=argvexp,({
# 2133
struct Cyc_Absyn_Exp*_tmp88E=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp33C[1]=_tmp88E;}),_tmp33C[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp33C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2131
_tmp33D->hd=_tmp890;}),_tmp33D->tl=new_args;_tmp33D;});new_args=_tmp891;});
# 2136
({struct Cyc_List_List*_tmp892=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp892;});{
# 2138
void*e1_typ=(void*)_check_null(_tmp45D->topt);
Cyc_Toc_exp_to_c(nv,_tmp45D);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2143
({void*_tmp896=({void*_tmp895=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp895,({
struct Cyc_Absyn_Exp*_tmp894=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp894,({struct Cyc_List_List*_tmp33E=_cycalloc(sizeof(*_tmp33E));
({struct Cyc_Absyn_Exp*_tmp893=Cyc_Absyn_copy_exp(_tmp45D);_tmp33E->hd=_tmp893;}),_tmp33E->tl=0;_tmp33E;}),0U);}));});
# 2143
_tmp45D->r=_tmp896;});{
# 2146
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp45D,new_args,0U),0U);
# 2149
if(_tmp459->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp33F=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp459->type));void*_tmp340=_tmp33F;struct Cyc_Absyn_Datatypedecl*_tmp343;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp340)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp340)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp340)->f1)->f1).KnownDatatype).tag == 2){_LL95: _tmp343=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp340)->f1)->f1).KnownDatatype).val;_LL96:
 tud=_tmp343;goto _LL94;}else{goto _LL97;}}else{goto _LL97;}}else{_LL97: _LL98:
({void*_tmp341=0U;({struct _dyneither_ptr _tmp897=({const char*_tmp342="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp342,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp897,_tag_dyneither(_tmp341,sizeof(void*),0U));});});}_LL94:;}{
# 2155
struct _dyneither_ptr vs=({unsigned int _tmp34F=(unsigned int)_tmp45B;struct _tuple1**_tmp34E=({struct _RegionHandle*_tmp898=r;_region_malloc(_tmp898,_check_times(_tmp34F,sizeof(struct _tuple1*)));});({{unsigned int _tmp714=(unsigned int)_tmp45B;unsigned int i;for(i=0;i < _tmp714;++ i){({struct _tuple1*_tmp899=Cyc_Toc_temp_var();_tmp34E[i]=_tmp899;});}}0;});_tag_dyneither(_tmp34E,sizeof(struct _tuple1*),_tmp34F);});
# 2157
if(_tmp45B != 0){
# 2161
struct Cyc_List_List*_tmp344=0;
{int i=_tmp45B - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp89B=({struct Cyc_List_List*_tmp345=_cycalloc(sizeof(*_tmp345));({struct Cyc_Absyn_Exp*_tmp89A=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp345->hd=_tmp89A;}),_tmp345->tl=_tmp344;_tmp345;});_tmp344=_tmp89B;});}}
({struct Cyc_Absyn_Stmt*_tmp89F=({struct _tuple1*_tmp89E=argv;void*_tmp89D=arr_type;struct Cyc_Absyn_Exp*_tmp89C=Cyc_Absyn_array_exp(_tmp344,0U);Cyc_Absyn_declare_stmt(_tmp89E,_tmp89D,_tmp89C,s,0U);});s=_tmp89F;});
# 2166
({struct Cyc_List_List*_tmp8A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp45C);_tmp45C=_tmp8A0;});
({struct Cyc_List_List*_tmp8A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp45A);_tmp45A=_tmp8A1;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp45C)- 1;
for(0;_tmp45C != 0;(_tmp45C=_tmp45C->tl,_tmp45A=_tmp45A->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp45C->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp346=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp45A))->hd;struct Cyc_Absyn_Datatypefield*_tmp347=_tmp346;struct _tuple1*_tmp34D;struct Cyc_List_List*_tmp34C;_LL9A: _tmp34D=_tmp347->name;_tmp34C=_tmp347->typs;_LL9B:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp34C))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp8A2;});{
# 2180
struct _tuple1*_tmp348=Cyc_Toc_collapse_qvars(_tmp34D,tud->name);
struct Cyc_List_List*_tmp349=({struct _tuple19*_tmp34B[2U];({struct _tuple19*_tmp8A6=({struct _dyneither_ptr*_tmp8A5=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp8A5,Cyc_Toc_datatype_tag(tud,_tmp34D));});_tmp34B[0]=_tmp8A6;}),({
struct _tuple19*_tmp8A4=({struct _dyneither_ptr*_tmp8A3=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp8A3,arg);});_tmp34B[1]=_tmp8A4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34B,sizeof(struct _tuple19*),2U));});
({struct Cyc_Absyn_Stmt*_tmp8AA=({struct _tuple1*_tmp8A9=var;void*_tmp8A8=Cyc_Absyn_strctq(_tmp348);struct Cyc_Absyn_Exp*_tmp8A7=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->tag=29U,_tmp34A->f1=_tmp348,_tmp34A->f2=0,_tmp34A->f3=_tmp349,_tmp34A->f4=0;_tmp34A;}),0U);
# 2183
Cyc_Absyn_declare_stmt(_tmp8A9,_tmp8A8,_tmp8A7,s,0U);});s=_tmp8AA;});};};}};}else{
# 2191
({struct Cyc_Absyn_Stmt*_tmp8AE=({struct _tuple1*_tmp8AD=argv;void*_tmp8AC=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8AD,_tmp8AC,_tmp8AB,s,0U);});s=_tmp8AE;});}};}else{
# 2196
if(_tmp45B != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp45C != 0;_tmp45C=_tmp45C->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp45C->hd);
({struct Cyc_List_List*_tmp8AF=({struct Cyc_List_List*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350->hd=(struct Cyc_Absyn_Exp*)_tmp45C->hd,_tmp350->tl=array_args;_tmp350;});array_args=_tmp8AF;});}{
# 2202
struct Cyc_Absyn_Exp*_tmp351=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp8B0=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp351,s,0U);s=_tmp8B0;});};}else{
# 2205
({struct Cyc_Absyn_Stmt*_tmp8B4=({struct _tuple1*_tmp8B3=argv;void*_tmp8B2=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp8B1=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8B3,_tmp8B2,_tmp8B1,s,0U);});s=_tmp8B4;});}}
# 2208
({void*_tmp8B5=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8B5;});};};}
# 2210
_npop_handler(0U);goto _LL7;
# 2114
;_pop_region(r);}}case 11U: _LL24: _tmp45F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp45E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL25:
# 2213
 Cyc_Toc_exp_to_c(nv,_tmp45F);{
struct Cyc_Absyn_Exp*_tmp352=_tmp45E?Cyc_Toc_newrethrow_exp(_tmp45F): Cyc_Toc_newthrow_exp(_tmp45F);
({void*_tmp8B7=({void*_tmp8B6=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp8B6,_tmp352,0U);})->r;e->r=_tmp8B7;});
goto _LL7;};case 12U: _LL26: _tmp460=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL27:
 Cyc_Toc_exp_to_c(nv,_tmp460);goto _LL7;case 13U: _LL28: _tmp462=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp461=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL29:
# 2219
 Cyc_Toc_exp_to_c(nv,_tmp462);
# 2228 "toc.cyc"
for(0;_tmp461 != 0;_tmp461=_tmp461->tl){
enum Cyc_Absyn_KindQual _tmp353=(Cyc_Tcutil_type_kind((void*)_tmp461->hd))->kind;
if(_tmp353 != Cyc_Absyn_EffKind  && _tmp353 != Cyc_Absyn_RgnKind){
{void*_tmp354=Cyc_Tcutil_compress((void*)_tmp461->hd);void*_tmp355=_tmp354;switch(*((int*)_tmp355)){case 2U: _LL9D: _LL9E:
 goto _LLA0;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355)->f1)->tag == 18U){_LL9F: _LLA0:
 continue;}else{goto _LLA1;}default: _LLA1: _LLA2:
# 2235
({void*_tmp8B9=({void*_tmp8B8=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8B8,_tmp462,0U);})->r;e->r=_tmp8B9;});
goto _LL9C;}_LL9C:;}
# 2238
break;}}
# 2241
goto _LL7;case 14U: _LL2A: _tmp466=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp465=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp464=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp463=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL2B: {
# 2243
void*old_t2=(void*)_check_null(_tmp465->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp466;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp466=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp465);
# 2250
{struct _tuple31 _tmp356=({struct _tuple31 _tmp716;({void*_tmp8BB=Cyc_Tcutil_compress(old_t2);_tmp716.f1=_tmp8BB;}),({void*_tmp8BA=Cyc_Tcutil_compress(new_typ);_tmp716.f2=_tmp8BA;});_tmp716;});struct _tuple31 _tmp357=_tmp356;struct Cyc_Absyn_PtrInfo _tmp37B;struct Cyc_Absyn_PtrInfo _tmp37A;struct Cyc_Absyn_PtrInfo _tmp379;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357.f1)->tag == 3U)switch(*((int*)_tmp357.f2)){case 3U: _LLA4: _tmp37A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357.f1)->f1;_tmp379=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357.f2)->f1;_LLA5: {
# 2252
struct Cyc_Absyn_Exp*_tmp358=({void*_tmp8BC=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8BC,(_tmp37A.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp359=({void*_tmp8BD=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8BD,(_tmp379.ptr_atts).bounds);});
int _tmp35A=Cyc_Tcutil_force_type2bool(0,(_tmp37A.ptr_atts).zero_term);
int _tmp35B=Cyc_Tcutil_force_type2bool(0,(_tmp379.ptr_atts).zero_term);
{struct _tuple32 _tmp35C=({struct _tuple32 _tmp715;_tmp715.f1=_tmp358,_tmp715.f2=_tmp359;_tmp715;});struct _tuple32 _tmp35D=_tmp35C;if(_tmp35D.f1 != 0){if(_tmp35D.f2 != 0){_LLAB: _LLAC:
# 2259
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8C0=({void*_tmp8BF=*_tmp466;Cyc_Toc_cast_it_r(_tmp8BF,({struct Cyc_Absyn_Exp*_tmp8BE=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp8BE,({struct Cyc_List_List*_tmp35E=_cycalloc(sizeof(*_tmp35E));
_tmp35E->hd=_tmp465,_tmp35E->tl=0;_tmp35E;}),0U);}));});
# 2261
e->r=_tmp8C0;});else{
# 2263
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp465->r;}
# 2269
goto _LLAA;}else{_LLAD: _LLAE: {
# 2271
struct Cyc_Absyn_Exp*_tmp35F=(struct Cyc_Absyn_Exp*)_check_null(_tmp358);
struct _tuple13 _tmp360=Cyc_Evexp_eval_const_uint_exp(_tmp35F);struct _tuple13 _tmp361=_tmp360;unsigned int _tmp36F;int _tmp36E;_LLB4: _tmp36F=_tmp361.f1;_tmp36E=_tmp361.f2;_LLB5:;
if(Cyc_Toc_is_toplevel(nv)){
# 2277
if((_tmp35A  && !(_tmp379.elt_tq).real_const) && !_tmp35B)
({struct Cyc_Absyn_Exp*_tmp8C2=({struct Cyc_Absyn_Exp*_tmp8C1=_tmp35F;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8C1,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp35F=_tmp8C2;});
({void*_tmp8C3=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp35F,_tmp465))->r;e->r=_tmp8C3;});}else{
# 2282
if(_tmp35A){
# 2287
struct _tuple1*_tmp362=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp363=Cyc_Absyn_var_exp(_tmp362,0U);
struct Cyc_Absyn_Exp*arg3;
# 2292
{void*_tmp364=_tmp465->r;void*_tmp365=_tmp364;struct Cyc_Absyn_Vardecl*_tmp368;struct Cyc_Absyn_Vardecl*_tmp367;switch(*((int*)_tmp365)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp365)->f1).Wstring_c).tag){case 8U: _LLB7: _LLB8:
 arg3=_tmp35F;goto _LLB6;case 9U: _LLB9: _LLBA:
 arg3=_tmp35F;goto _LLB6;default: goto _LLBF;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp365)->f1)){case 1U: _LLBB: _tmp367=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp365)->f1)->f1;_LLBC:
 _tmp368=_tmp367;goto _LLBE;case 4U: _LLBD: _tmp368=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp365)->f1)->f1;_LLBE:
# 2297
 if(!Cyc_Tcutil_is_array_type(_tmp368->type))
goto _LLC0;
arg3=_tmp35F;
goto _LLB6;default: goto _LLBF;}default: _LLBF: _LLC0:
# 2304
 if(_tmp36E  && _tmp36F != 1)
arg3=_tmp35F;else{
# 2307
({struct Cyc_Absyn_Exp*_tmp8C7=({struct Cyc_Absyn_Exp*_tmp8C6=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp465);Cyc_Absyn_fncall_exp(_tmp8C6,({struct Cyc_Absyn_Exp*_tmp366[2U];({
struct Cyc_Absyn_Exp*_tmp8C5=({void*_tmp8C4=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp8C4,_tmp363);});_tmp366[0]=_tmp8C5;}),_tmp366[1]=_tmp35F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp366,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2307
arg3=_tmp8C7;});}
# 2309
goto _LLB6;}_LLB6:;}
# 2311
if(!_tmp35B  && !(_tmp379.elt_tq).real_const)
# 2314
({struct Cyc_Absyn_Exp*_tmp8C9=({struct Cyc_Absyn_Exp*_tmp8C8=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8C8,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp8C9;});{
# 2316
struct Cyc_Absyn_Exp*_tmp369=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp379.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp36A=({struct Cyc_Absyn_Exp*_tmp8CA=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8CA,({struct Cyc_Absyn_Exp*_tmp36C[3U];_tmp36C[0]=_tmp363,_tmp36C[1]=_tmp369,_tmp36C[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp36B=Cyc_Absyn_exp_stmt(_tmp36A,0U);
({struct Cyc_Absyn_Stmt*_tmp8CE=({struct _tuple1*_tmp8CD=_tmp362;void*_tmp8CC=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp8CB=_tmp465;Cyc_Absyn_declare_stmt(_tmp8CD,_tmp8CC,_tmp8CB,_tmp36B,0U);});_tmp36B=_tmp8CE;});
({void*_tmp8CF=Cyc_Toc_stmt_exp_r(_tmp36B);e->r=_tmp8CF;});};}else{
# 2323
({void*_tmp8D2=({struct Cyc_Absyn_Exp*_tmp8D1=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp8D1,({struct Cyc_Absyn_Exp*_tmp36D[3U];_tmp36D[0]=_tmp465,({
# 2325
struct Cyc_Absyn_Exp*_tmp8D0=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp379.elt_type),0U);_tmp36D[1]=_tmp8D0;}),_tmp36D[2]=_tmp35F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36D,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2323
e->r=_tmp8D2;});}}
# 2328
goto _LLAA;}}}else{if(_tmp35D.f2 != 0){_LLAF: _LLB0: {
# 2338 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp370=(struct Cyc_Absyn_Exp*)_check_null(_tmp359);
if(_tmp35A  && !_tmp35B)
({struct Cyc_Absyn_Exp*_tmp8D4=({struct Cyc_Absyn_Exp*_tmp8D3=_tmp359;Cyc_Absyn_add_exp(_tmp8D3,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp370=_tmp8D4;});{
# 2345
struct Cyc_Absyn_Exp*_tmp371=({struct Cyc_Absyn_Exp*_tmp8D6=Cyc_Toc__untag_dyneither_ptr_e;Cyc_Absyn_fncall_exp(_tmp8D6,({struct Cyc_Absyn_Exp*_tmp373[3U];_tmp373[0]=_tmp465,({
# 2347
struct Cyc_Absyn_Exp*_tmp8D5=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp37A.elt_type),0U);_tmp373[1]=_tmp8D5;}),_tmp373[2]=_tmp370;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp373,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2349
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8D9=({struct Cyc_Absyn_Exp*_tmp8D8=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp8D8,({struct Cyc_List_List*_tmp372=_cycalloc(sizeof(*_tmp372));
({struct Cyc_Absyn_Exp*_tmp8D7=Cyc_Absyn_copy_exp(_tmp371);_tmp372->hd=_tmp8D7;}),_tmp372->tl=0;_tmp372;}));});
# 2351
_tmp371->r=_tmp8D9;});
# 2353
({void*_tmp8DA=Cyc_Toc_cast_it_r(*_tmp466,_tmp371);e->r=_tmp8DA;});
goto _LLAA;};}}else{_LLB1: _LLB2:
# 2358
 if((_tmp35A  && !_tmp35B) && !(_tmp379.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp374=0U;({struct _dyneither_ptr _tmp8DB=({const char*_tmp375="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp375,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp8DB,_tag_dyneither(_tmp374,sizeof(void*),0U));});});
({void*_tmp8DF=({struct Cyc_Absyn_Exp*_tmp8DE=Cyc_Toc__dyneither_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp8DE,({struct Cyc_Absyn_Exp*_tmp376[3U];_tmp376[0]=_tmp465,({
struct Cyc_Absyn_Exp*_tmp8DD=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp37A.elt_type),0U);_tmp376[1]=_tmp8DD;}),({
struct Cyc_Absyn_Exp*_tmp8DC=Cyc_Absyn_uint_exp(1U,0U);_tmp376[2]=_tmp8DC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp376,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2361
e->r=_tmp8DF;});}
# 2365
goto _LLAA;}}_LLAA:;}
# 2367
goto _LLA3;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp357.f2)->f1)->tag == 1U){_LLA6: _tmp37B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357.f1)->f1;_LLA7:
# 2369
{struct Cyc_Absyn_Exp*_tmp377=({void*_tmp8E0=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8E0,(_tmp37B.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp378=_tmp377;if(_tmp378 == 0){_LLC2: _LLC3:
# 2371
({void*_tmp8E2=({struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Absyn_new_exp(_tmp465->r,_tmp465->loc);Cyc_Toc_aggrmember_exp_r(_tmp8E1,Cyc_Toc_curr_sp);});_tmp465->r=_tmp8E2;});
_tmp465->topt=new_typ_c;
goto _LLC1;}else{_LLC4: _LLC5:
 goto _LLC1;}_LLC1:;}
# 2376
goto _LLA3;}else{goto _LLA8;}default: goto _LLA8;}else{_LLA8: _LLA9:
# 2378
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp465->r;
goto _LLA3;}_LLA3:;}
# 2382
goto _LL7;}case 15U: _LL2C: _tmp467=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL2D:
# 2385
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp467);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp467)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp467,0,Cyc_Toc_address_lvalue,1);
# 2391
({void*_tmp8E4=({void*_tmp8E3=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8E3,_tmp467);});e->r=_tmp8E4;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp467->topt))))
# 2395
({void*_tmp8E6=({void*_tmp8E5=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8E5,_tmp467);});e->r=_tmp8E6;});}
# 2397
goto _LL7;case 16U: _LL2E: _tmp469=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp468=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL2F:
# 2404
 Cyc_Toc_exp_to_c(nv,_tmp468);{
# 2406
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp468->topt));
struct _tuple1*_tmp37C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp37D=Cyc_Absyn_var_exp(_tmp37C,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp37E=Cyc_Tcutil_compress(elt_typ);void*_tmp37F=_tmp37E;void*_tmp380;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37F)->tag == 4U){_LLC7: _tmp380=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37F)->f1).elt_type;_LLC8:
# 2412
({void*_tmp8E7=Cyc_Toc_typ_to_c(_tmp380);elt_typ=_tmp8E7;});
({struct Cyc_Absyn_Exp*_tmp8E8=Cyc_Absyn_copy_exp(_tmp37D);lhs=_tmp8E8;});
goto _LLC6;}else{_LLC9: _LLCA:
# 2416
({struct Cyc_Absyn_Exp*_tmp8E9=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp37D),0U);lhs=_tmp8E9;});
goto _LLC6;}_LLC6:;}{
# 2419
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp468);
struct _tuple1*_tmp381=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp382=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp381),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp8EC=({struct Cyc_Absyn_Exp*_tmp8EB=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp8EB,({struct Cyc_Absyn_Exp*_tmp383[2U];_tmp383[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp382),({
struct Cyc_Absyn_Exp*_tmp8EA=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp383[1]=_tmp8EA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp383,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2424
mexp=_tmp8EC;});else{
# 2427
({struct Cyc_Absyn_Exp*_tmp8ED=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp37D,0U),0U);mexp=_tmp8ED;});}{
# 2430
struct Cyc_Absyn_Exp*_tmp384=Cyc_Toc_get_varsizeexp(_tmp468);
if(_tmp384 != 0)
({struct Cyc_Absyn_Exp*_tmp8EE=Cyc_Absyn_add_exp(mexp,_tmp384,0U);mexp=_tmp8EE;});
# 2434
if(_tmp469 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp8EF=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp8EF;});else{
# 2437
struct Cyc_Absyn_Exp*r=_tmp469;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp8F0=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp8F0;});}{
# 2441
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp8F5=({struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8F4,({struct Cyc_Absyn_Exp*_tmp385[3U];({
struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Absyn_copy_exp(_tmp37D);_tmp385[0]=_tmp8F3;}),({
struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp385[1]=_tmp8F2;}),
(unsigned int)_tmp382?_tmp385[2]=_tmp382:({struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Absyn_uint_exp(1U,0U);_tmp385[2]=_tmp8F1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp385,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2443
answer=_tmp8F5;});else{
# 2448
({struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Absyn_copy_exp(_tmp37D);answer=_tmp8F6;});}
({void*_tmp8F7=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386->tag=Cyc_Toc_Dest,_tmp386->f1=_tmp37D;_tmp386;});e->annot=_tmp8F7;});{
struct Cyc_Absyn_Stmt*_tmp387=({struct _tuple1*_tmp8FB=_tmp37C;void*_tmp8FA=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8F9=mexp;Cyc_Absyn_declare_stmt(_tmp8FB,_tmp8FA,_tmp8F9,({
struct Cyc_Absyn_Stmt*_tmp8F8=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp8F8,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2454
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp8FC=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp381),Cyc_Absyn_uint_type,array_len,_tmp387,0U);_tmp387=_tmp8FC;});
({void*_tmp8FD=Cyc_Toc_stmt_exp_r(_tmp387);e->r=_tmp8FD;});
if(_tmp384 != 0)
({void*_tmp8FF=({void*_tmp8FE=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp8FE,Cyc_Absyn_copy_exp(e));});e->r=_tmp8FF;});
goto _LL7;};};};};};case 18U: _LL30: _tmp46A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL31:
# 2462
 Cyc_Toc_exp_to_c(nv,_tmp46A);goto _LL7;case 17U: _LL32: _tmp46B=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL33:
({void*_tmp901=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388->tag=17U,({void*_tmp900=Cyc_Toc_typ_to_c(_tmp46B);_tmp388->f1=_tmp900;});_tmp388;});e->r=_tmp901;});goto _LL7;case 19U: _LL34: _tmp46D=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp46C=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL35: {
# 2465
void*_tmp389=_tmp46D;
struct Cyc_List_List*_tmp38A=_tmp46C;
for(0;_tmp38A != 0;_tmp38A=_tmp38A->tl){
void*_tmp38B=(void*)_tmp38A->hd;void*_tmp38C=_tmp38B;unsigned int _tmp39E;struct _dyneither_ptr*_tmp39D;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp38C)->tag == 0U){_LLCC: _tmp39D=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp38C)->f1;_LLCD:
 goto _LLCB;}else{_LLCE: _tmp39E=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp38C)->f1;_LLCF:
# 2471
{void*_tmp38D=Cyc_Tcutil_compress(_tmp389);void*_tmp38E=_tmp38D;struct Cyc_List_List*_tmp39C;struct Cyc_List_List*_tmp39B;struct Cyc_Absyn_Datatypefield*_tmp39A;union Cyc_Absyn_AggrInfo _tmp399;switch(*((int*)_tmp38E)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38E)->f1)){case 20U: _LLD1: _tmp399=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38E)->f1)->f1;_LLD2: {
# 2473
struct Cyc_Absyn_Aggrdecl*_tmp38F=Cyc_Absyn_get_known_aggrdecl(_tmp399);
if(_tmp38F->impl == 0)
({void*_tmp390=0U;({struct _dyneither_ptr _tmp902=({const char*_tmp391="struct fields must be known";_tag_dyneither(_tmp391,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp902,_tag_dyneither(_tmp390,sizeof(void*),0U));});});
_tmp39B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp38F->impl))->fields;goto _LLD4;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38E)->f1)->f1).KnownDatatypefield).tag == 2){_LLD7: _tmp39A=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38E)->f1)->f1).KnownDatatypefield).val).f2;_LLD8:
# 2487
 if(_tmp39E == 0)
({void*_tmp903=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395->tag=0U,_tmp395->f1=Cyc_Toc_tag_sp;_tmp395;}));_tmp38A->hd=_tmp903;});else{
# 2490
({void*_tmp904=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp39A->typs,(int)(_tmp39E - 1))).f2;_tmp389=_tmp904;});
({void*_tmp906=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->tag=0U,({struct _dyneither_ptr*_tmp905=Cyc_Absyn_fieldname((int)_tmp39E);_tmp396->f1=_tmp905;});_tmp396;}));_tmp38A->hd=_tmp906;});}
# 2493
goto _LLD0;}else{goto _LLD9;}default: goto _LLD9;}case 7U: _LLD3: _tmp39B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38E)->f2;_LLD4: {
# 2478
struct Cyc_Absyn_Aggrfield*_tmp392=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp39B,(int)_tmp39E);
({void*_tmp907=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->tag=0U,_tmp393->f1=_tmp392->name;_tmp393;}));_tmp38A->hd=_tmp907;});
_tmp389=_tmp392->type;
goto _LLD0;}case 6U: _LLD5: _tmp39C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38E)->f1;_LLD6:
# 2483
({void*_tmp909=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->tag=0U,({struct _dyneither_ptr*_tmp908=Cyc_Absyn_fieldname((int)(_tmp39E + 1));_tmp394->f1=_tmp908;});_tmp394;}));_tmp38A->hd=_tmp909;});
({void*_tmp90A=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp39C,(int)_tmp39E)).f2;_tmp389=_tmp90A;});
goto _LLD0;default: _LLD9: _LLDA:
# 2494
({void*_tmp397=0U;({struct _dyneither_ptr _tmp90B=({const char*_tmp398="impossible type for offsetof tuple index";_tag_dyneither(_tmp398,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp90B,_tag_dyneither(_tmp397,sizeof(void*),0U));});});
goto _LLD0;}_LLD0:;}
# 2497
goto _LLCB;}_LLCB:;}
# 2500
({void*_tmp90D=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->tag=19U,({void*_tmp90C=Cyc_Toc_typ_to_c(_tmp46D);_tmp39F->f1=_tmp90C;}),_tmp39F->f2=_tmp46C;_tmp39F;});e->r=_tmp90D;});
goto _LL7;}case 21U: _LL36: _tmp471=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp470=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp46F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp46E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL37: {
# 2503
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp471->topt);
Cyc_Toc_exp_to_c(nv,_tmp471);
if(_tmp46F  && _tmp46E)
({void*_tmp912=({struct Cyc_Absyn_Exp*_tmp911=_tmp471;void*_tmp910=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp90F=e1_cyc_type;struct _dyneither_ptr*_tmp90E=_tmp470;Cyc_Toc_check_tagged_union(_tmp911,_tmp910,_tmp90F,_tmp90E,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2507
e->r=_tmp912;});
# 2509
if(is_poly)
({void*_tmp914=({void*_tmp913=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp913,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp914;});
goto _LL7;}case 22U: _LL38: _tmp475=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp474=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp473=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp472=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL39: {
# 2513
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(_tmp475->topt);
void*ta;
{void*_tmp3A0=Cyc_Tcutil_compress(e1typ);void*_tmp3A1=_tmp3A0;struct Cyc_Absyn_PtrInfo _tmp3A4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A1)->tag == 3U){_LLDC: _tmp3A4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A1)->f1;_LLDD:
 ta=_tmp3A4.elt_type;goto _LLDB;}else{_LLDE: _LLDF:
({void*_tmp3A2=0U;({struct _dyneither_ptr _tmp915=({const char*_tmp3A3="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3A3,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp915,_tag_dyneither(_tmp3A2,sizeof(void*),0U));});});}_LLDB:;}
# 2520
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp475,e->annot,0);
if(_tmp473  && _tmp472)
({void*_tmp91A=({struct Cyc_Absyn_Exp*_tmp919=_tmp475;void*_tmp918=Cyc_Toc_typ_to_c(e1typ);void*_tmp917=ta;struct _dyneither_ptr*_tmp916=_tmp474;Cyc_Toc_check_tagged_union(_tmp919,_tmp918,_tmp917,_tmp916,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp91A;});
if(is_poly  && _tmp472)
({void*_tmp91C=({void*_tmp91B=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp91B,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp91C;});
goto _LL7;}case 20U: _LL3A: _tmp476=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL3B:
# 2528
 did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp476,e->annot,0);
goto _LL7;case 23U: _LL3C: _tmp478=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp477=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL3D:
# 2532
{void*_tmp3A5=Cyc_Tcutil_compress((void*)_check_null(_tmp478->topt));void*_tmp3A6=_tmp3A5;struct Cyc_List_List*_tmp3AE;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A6)->tag == 6U){_LLE1: _tmp3AE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A6)->f1;_LLE2:
# 2534
 Cyc_Toc_exp_to_c(nv,_tmp478);{
int _tmp3A7=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp477);{
struct _tuple13 _tmp3A8=Cyc_Evexp_eval_const_uint_exp(_tmp477);struct _tuple13 _tmp3A9=_tmp3A8;unsigned int _tmp3AD;int _tmp3AC;_LLE6: _tmp3AD=_tmp3A9.f1;_tmp3AC=_tmp3A9.f2;_LLE7:;
if(!_tmp3AC)
({void*_tmp3AA=0U;({struct _dyneither_ptr _tmp91D=({const char*_tmp3AB="unknown tuple subscript in translation to C";_tag_dyneither(_tmp3AB,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp91D,_tag_dyneither(_tmp3AA,sizeof(void*),0U));});});
({void*_tmp91F=({struct Cyc_Absyn_Exp*_tmp91E=_tmp478;Cyc_Toc_aggrmember_exp_r(_tmp91E,Cyc_Absyn_fieldname((int)(_tmp3AD + 1)));});e->r=_tmp91F;});
goto _LLE0;};};}else{_LLE3: _LLE4:
# 2544
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,_tmp478,e->annot,_tmp477);
if(index_used)
({void*_tmp920=Cyc_Toc_deref_exp_r(_tmp478);e->r=_tmp920;});
goto _LLE0;};}_LLE0:;}
# 2550
goto _LL7;case 24U: _LL3E: _tmp479=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL3F:
# 2552
 if(!Cyc_Toc_is_toplevel(nv)){
# 2554
void*_tmp3AF=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp479));
{void*_tmp3B0=_tmp3AF;union Cyc_Absyn_AggrInfo _tmp3B5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f2 == 0){_LLE9: _tmp3B5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f1)->f1;_LLEA: {
# 2557
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3B5);
{int i=1;for(0;_tmp479 != 0;(_tmp479=_tmp479->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp479->hd);
({struct Cyc_List_List*_tmp923=({struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));({struct _tuple19*_tmp922=({struct _dyneither_ptr*_tmp921=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp921,(struct Cyc_Absyn_Exp*)_tmp479->hd);});_tmp3B1->hd=_tmp922;}),_tmp3B1->tl=dles;_tmp3B1;});dles=_tmp923;});}}
# 2563
({void*_tmp925=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->tag=29U,_tmp3B2->f1=sd->name,_tmp3B2->f2=0,({struct Cyc_List_List*_tmp924=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp3B2->f3=_tmp924;}),_tmp3B2->f4=sd;_tmp3B2;});e->r=_tmp925;});
e->topt=_tmp3AF;
goto _LLE8;}}else{goto _LLEB;}}else{goto _LLEB;}}else{_LLEB: _LLEC:
({void*_tmp3B3=0U;({struct _dyneither_ptr _tmp926=({const char*_tmp3B4="tuple type not an aggregate";_tag_dyneither(_tmp3B4,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp926,_tag_dyneither(_tmp3B3,sizeof(void*),0U));});});}_LLE8:;}
# 2568
goto _LL7;}else{
# 2572
struct Cyc_List_List*dles=0;
for(0;_tmp479 != 0;_tmp479=_tmp479->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp479->hd);
({struct Cyc_List_List*_tmp928=({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({struct _tuple19*_tmp927=({struct _tuple19*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->f1=0,_tmp3B6->f2=(struct Cyc_Absyn_Exp*)_tmp479->hd;_tmp3B6;});_tmp3B7->hd=_tmp927;}),_tmp3B7->tl=dles;_tmp3B7;});dles=_tmp928;});}
# 2577
({void*_tmp929=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp929;});}
# 2579
goto _LL7;case 26U: _LL40: _tmp47A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL41:
# 2585
{struct Cyc_List_List*_tmp3B8=_tmp47A;for(0;_tmp3B8 != 0;_tmp3B8=_tmp3B8->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3B8->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp92A=Cyc_Toc_unresolvedmem_exp_r(0,_tmp47A);e->r=_tmp92A;});
goto _LL7;case 27U: _LL42: _tmp47E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp47D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp47C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp47B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL43:
# 2593
 Cyc_Toc_exp_to_c(nv,_tmp47D);
Cyc_Toc_exp_to_c(nv,_tmp47C);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp3B9=Cyc_Evexp_eval_const_uint_exp(_tmp47D);struct _tuple13 _tmp3BA=_tmp3B9;unsigned int _tmp3C4;int _tmp3C3;_LLEE: _tmp3C4=_tmp3BA.f1;_tmp3C3=_tmp3BA.f2;_LLEF:;{
void*_tmp3BB=Cyc_Toc_typ_to_c((void*)_check_null(_tmp47C->topt));
struct Cyc_List_List*es=0;
# 2600
if(!Cyc_Toc_is_zero(_tmp47C)){
if(!_tmp3C3)
({void*_tmp3BC=0U;({unsigned int _tmp92C=_tmp47D->loc;struct _dyneither_ptr _tmp92B=({const char*_tmp3BD="cannot determine value of constant";_tag_dyneither(_tmp3BD,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp92C,_tmp92B,_tag_dyneither(_tmp3BC,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp3C4;++ i){
({struct Cyc_List_List*_tmp92E=({struct Cyc_List_List*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({struct _tuple19*_tmp92D=({struct _tuple19*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->f1=0,_tmp3BE->f2=_tmp47C;_tmp3BE;});_tmp3BF->hd=_tmp92D;}),_tmp3BF->tl=es;_tmp3BF;});es=_tmp92E;});}}
# 2606
if(_tmp47B){
struct Cyc_Absyn_Exp*_tmp3C0=({void*_tmp92F=_tmp3BB;Cyc_Toc_cast_it(_tmp92F,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp932=({struct Cyc_List_List*_tmp931=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp931,({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct _tuple19*_tmp930=({struct _tuple19*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->f1=0,_tmp3C1->f2=_tmp3C0;_tmp3C1;});_tmp3C2->hd=_tmp930;}),_tmp3C2->tl=0;_tmp3C2;}));});es=_tmp932;});}}
# 2611
({void*_tmp933=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp933;});};}
# 2613
goto _LL7;case 28U: _LL44: _tmp481=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp480=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp47F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_LL45:
# 2620
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp934=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp934;});else{
# 2623
Cyc_Toc_exp_to_c(nv,_tmp481);}
goto _LL7;case 30U: _LL46: _tmp483=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp482=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL47:
# 2627
{struct Cyc_List_List*_tmp3C5=_tmp482;for(0;_tmp3C5 != 0;_tmp3C5=_tmp3C5->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3C5->hd)).f2);}}{
void*_tmp3C6=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp3C6;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3C7=Cyc_Tcutil_compress(_tmp3C6);void*_tmp3C8=_tmp3C7;union Cyc_Absyn_AggrInfo _tmp3CC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->f2 == 0){_LLF1: _tmp3CC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->f1)->f1;_LLF2: {
# 2634
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3CC);
({void*_tmp935=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->tag=29U,_tmp3C9->f1=sd->name,_tmp3C9->f2=0,_tmp3C9->f3=_tmp482,_tmp3C9->f4=sd;_tmp3C9;});e->r=_tmp935;});
e->topt=_tmp3C6;
goto _LLF0;}}else{goto _LLF3;}}else{goto _LLF3;}}else{_LLF3: _LLF4:
({void*_tmp3CA=0U;({struct _dyneither_ptr _tmp936=({const char*_tmp3CB="anonStruct type not an aggregate";_tag_dyneither(_tmp3CB,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp936,_tag_dyneither(_tmp3CA,sizeof(void*),0U));});});}_LLF0:;}else{
# 2641
({void*_tmp937=Cyc_Toc_unresolvedmem_exp_r(0,_tmp482);e->r=_tmp937;});}
goto _LL7;};case 29U: _LL48: _tmp487=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp486=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp485=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp484=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL49:
# 2647
 if(_tmp484 == 0  || _tmp484->impl == 0)
({void*_tmp3CD=0U;({struct _dyneither_ptr _tmp938=({const char*_tmp3CE="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp3CE,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp938,_tag_dyneither(_tmp3CD,sizeof(void*),0U));});});{
void*_tmp3CF=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp3D0=Cyc_Tcutil_compress(_tmp3CF);void*_tmp3D1=_tmp3D0;union Cyc_Absyn_AggrInfo _tmp3D4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->f1)->tag == 20U){_LLF6: _tmp3D4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->f1)->f1;_LLF7:
({struct _tuple1*_tmp939=(Cyc_Absyn_aggr_kinded_name(_tmp3D4)).f2;*_tmp487=_tmp939;});goto _LLF5;}else{goto _LLF8;}}else{_LLF8: _LLF9:
({void*_tmp3D2=0U;({struct _dyneither_ptr _tmp93A=({const char*_tmp3D3="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp3D3,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp93A,_tag_dyneither(_tmp3D2,sizeof(void*),0U));});});}_LLF5:;}{
# 2658
struct Cyc_List_List*_tmp3D5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->fields;
if(_tmp3D5 == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(_tmp3D5))->tl != 0;_tmp3D5=_tmp3D5->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp3D6=(struct Cyc_Absyn_Aggrfield*)_tmp3D5->hd;
struct Cyc_List_List*_tmp3D7=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp485,_tmp484->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->fields);
# 2665
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3D8=Cyc_Tcutil_compress(old_typ);void*_tmp3D9=_tmp3D8;struct Cyc_List_List*_tmp405;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->f1)->tag == 20U){_LLFB: _tmp405=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->f2;_LLFC:
# 2669
{struct Cyc_List_List*_tmp3DA=_tmp3D7;for(0;_tmp3DA != 0;_tmp3DA=_tmp3DA->tl){
struct _tuple33*_tmp3DB=(struct _tuple33*)_tmp3DA->hd;struct _tuple33*_tmp3DC=_tmp3DB;struct Cyc_Absyn_Aggrfield*_tmp402;struct Cyc_Absyn_Exp*_tmp401;_LL100: _tmp402=_tmp3DC->f1;_tmp401=_tmp3DC->f2;_LL101:;{
void*_tmp3DD=_tmp401->topt;
Cyc_Toc_exp_to_c(nv,_tmp401);
# 2674
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp402->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp401->topt)))
({void*_tmp93C=({void*_tmp93B=Cyc_Toc_typ_to_c(_tmp402->type);Cyc_Toc_cast_it(_tmp93B,
Cyc_Absyn_copy_exp(_tmp401));})->r;
# 2676
_tmp401->r=_tmp93C;});
# 2679
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->tagged){
struct _dyneither_ptr*_tmp3DE=_tmp402->name;
struct Cyc_Absyn_Exp*_tmp3DF=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp484,_tmp3DE),0U);
struct _tuple19*_tmp3E0=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp3DF);
struct _tuple19*_tmp3E1=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp401);
struct _tuple1*s=({struct _tuple1*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({union Cyc_Absyn_Nmspace _tmp940=Cyc_Absyn_Abs_n(0,1);_tmp3EC->f1=_tmp940;}),({
struct _dyneither_ptr*_tmp93F=({struct _dyneither_ptr*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));({struct _dyneither_ptr _tmp93E=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3E9=({struct Cyc_String_pa_PrintArg_struct _tmp718;_tmp718.tag=0U,_tmp718.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp484->name).f2);_tmp718;});struct Cyc_String_pa_PrintArg_struct _tmp3EA=({struct Cyc_String_pa_PrintArg_struct _tmp717;_tmp717.tag=0U,_tmp717.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3DE);_tmp717;});void*_tmp3E7[2U];_tmp3E7[0]=& _tmp3E9,_tmp3E7[1]=& _tmp3EA;({struct _dyneither_ptr _tmp93D=({const char*_tmp3E8="_union_%s_%s";_tag_dyneither(_tmp3E8,sizeof(char),13U);});Cyc_aprintf(_tmp93D,_tag_dyneither(_tmp3E7,sizeof(void*),2U));});});*_tmp3EB=_tmp93E;});_tmp3EB;});_tmp3EC->f2=_tmp93F;});_tmp3EC;});
# 2687
struct _tuple19*_tmp3E2=({
struct _dyneither_ptr*_tmp942=_tmp3DE;Cyc_Toc_make_field(_tmp942,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->tag=29U,_tmp3E6->f1=s,_tmp3E6->f2=0,({struct Cyc_List_List*_tmp941=({struct _tuple19*_tmp3E5[2U];_tmp3E5[0]=_tmp3E0,_tmp3E5[1]=_tmp3E1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3E5,sizeof(struct _tuple19*),2U));});_tmp3E6->f3=_tmp941;}),_tmp3E6->f4=0;_tmp3E6;}),0U));});
# 2690
({void*_tmp944=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->tag=29U,_tmp3E4->f1=*_tmp487,_tmp3E4->f2=0,({struct Cyc_List_List*_tmp943=({struct _tuple19*_tmp3E3[1U];_tmp3E3[0]=_tmp3E2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3E3,sizeof(struct _tuple19*),1U));});_tmp3E4->f3=_tmp943;}),_tmp3E4->f4=_tmp484;_tmp3E4;});e->r=_tmp944;});}
# 2696
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp3D6 == _tmp402){
struct Cyc_List_List*_tmp3ED=({struct Cyc_List_List*_tmp945=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->exist_vars,_tmp486);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp945,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp484->tvs,_tmp405));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp3EE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->fields;for(0;_tmp3EE != 0;_tmp3EE=_tmp3EE->tl){
struct Cyc_Absyn_Aggrfield*_tmp3EF=(struct Cyc_Absyn_Aggrfield*)_tmp3EE->hd;
void*_tmp3F0=Cyc_Tcutil_substitute(_tmp3ED,_tmp3EF->type);
struct Cyc_Absyn_Aggrfield*_tmp3F1=({struct Cyc_Absyn_Aggrfield*_tmp946=_tmp3EF;Cyc_Toc_aggrfield_to_c(_tmp946,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp3ED,_tmp3F0)));});
({struct Cyc_List_List*_tmp947=({struct Cyc_List_List*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2->hd=_tmp3F1,_tmp3F2->tl=new_fields;_tmp3F2;});new_fields=_tmp947;});
# 2711
if(_tmp3EE->tl == 0){
{void*_tmp3F3=Cyc_Tcutil_compress(_tmp3F1->type);void*_tmp3F4=_tmp3F3;struct Cyc_Absyn_ArrayInfo _tmp3F9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F4)->tag == 4U){_LL103: _tmp3F9=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F4)->f1;_LL104:
# 2714
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3F9.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp3F5=_tmp3F9;
({struct Cyc_Absyn_Exp*_tmp948=Cyc_Absyn_uint_exp(0U,0U);_tmp3F5.num_elts=_tmp948;});
({void*_tmp949=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));_tmp3F6->tag=4U,_tmp3F6->f1=_tmp3F5;_tmp3F6;});_tmp3F1->type=_tmp949;});}
# 2719
goto _LL102;}else{_LL105: _LL106:
# 2723
 if(_tmp401->topt == 0)
goto _LL102;
{void*_tmp3F7=Cyc_Tcutil_compress((void*)_check_null(_tmp401->topt));void*_tmp3F8=_tmp3F7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F8)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F8)->f1)->tag == 20U){_LL108: _LL109:
# 2727
 _tmp3F1->type=(void*)_check_null(_tmp401->topt);goto _LL107;}else{goto _LL10A;}}else{_LL10A: _LL10B:
 goto _LL107;}_LL107:;}
# 2730
goto _LL102;}_LL102:;}
# 2734
if(!Cyc_Tcutil_is_array_type(_tmp3EF->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3EF->type)))
({struct Cyc_List_List*_tmp94B=({struct Cyc_List_List*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));({void*_tmp94A=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->tag=6U,_tmp3FA->f1=0;_tmp3FA;});_tmp3FB->hd=_tmp94A;}),_tmp3FB->tl=_tmp3F1->attributes;_tmp3FB;});_tmp3F1->attributes=_tmp94B;});}}}
# 2740
({struct Cyc_Absyn_Aggrdecl*_tmp94F=({struct _dyneither_ptr*_tmp94E=({struct _dyneither_ptr*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));({struct _dyneither_ptr _tmp94D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3FE=({struct Cyc_Int_pa_PrintArg_struct _tmp719;_tmp719.tag=1U,_tmp719.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp719;});void*_tmp3FC[1U];_tmp3FC[0]=& _tmp3FE;({struct _dyneither_ptr _tmp94C=({const char*_tmp3FD="_genStruct%d";_tag_dyneither(_tmp3FD,sizeof(char),13U);});Cyc_aprintf(_tmp94C,_tag_dyneither(_tmp3FC,sizeof(void*),1U));});});*_tmp3FF=_tmp94D;});_tmp3FF;});Cyc_Toc_make_c_struct_defn(_tmp94E,
# 2742
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 2740
_tmp484=_tmp94F;});
# 2743
*_tmp487=_tmp484->name;
Cyc_Toc_aggrdecl_to_c(_tmp484);
# 2746
({void*_tmp950=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp400=_cycalloc(sizeof(*_tmp400));*_tmp400=_tmp484;_tmp400;})),0);e->topt=_tmp950;});}};}}
# 2749
goto _LLFA;}else{goto _LLFD;}}else{_LLFD: _LLFE:
({void*_tmp403=0U;({struct _dyneither_ptr _tmp951=({const char*_tmp404="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp404,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp951,_tag_dyneither(_tmp403,sizeof(void*),0U));});});}_LLFA:;}else{
# 2758
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->tagged){
# 2760
struct _tuple33*_tmp406=(struct _tuple33*)((struct Cyc_List_List*)_check_null(_tmp3D7))->hd;struct _tuple33*_tmp407=_tmp406;struct Cyc_Absyn_Aggrfield*_tmp410;struct Cyc_Absyn_Exp*_tmp40F;_LL10D: _tmp410=_tmp407->f1;_tmp40F=_tmp407->f2;_LL10E:;{
void*_tmp408=(void*)_check_null(_tmp40F->topt);
void*_tmp409=_tmp410->type;
Cyc_Toc_exp_to_c(nv,_tmp40F);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp409) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp408))
({void*_tmp953=({
void*_tmp952=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp952,Cyc_Absyn_new_exp(_tmp40F->r,0U));});
# 2766
_tmp40F->r=_tmp953;});{
# 2769
int i=Cyc_Toc_get_member_offset(_tmp484,_tmp410->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp40A=({struct _tuple19*_tmp40C[2U];({struct _tuple19*_tmp955=({struct _tuple19*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->f1=0,_tmp40D->f2=field_tag_exp;_tmp40D;});_tmp40C[0]=_tmp955;}),({struct _tuple19*_tmp954=({struct _tuple19*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->f1=0,_tmp40E->f2=_tmp40F;_tmp40E;});_tmp40C[1]=_tmp954;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp40C,sizeof(struct _tuple19*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp40A,0U);
({void*_tmp957=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp40B[1U];({struct _tuple19*_tmp956=Cyc_Toc_make_field(_tmp410->name,umem);_tmp40B[0]=_tmp956;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp40B,sizeof(struct _tuple19*),1U));}));e->r=_tmp957;});};};}else{
# 2776
struct Cyc_List_List*_tmp411=0;
struct Cyc_List_List*_tmp412=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->fields;
for(0;_tmp412 != 0;_tmp412=_tmp412->tl){
struct Cyc_List_List*_tmp413=_tmp3D7;for(0;_tmp413 != 0;_tmp413=_tmp413->tl){
if((*((struct _tuple33*)_tmp413->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp412->hd){
struct _tuple33*_tmp414=(struct _tuple33*)_tmp413->hd;struct _tuple33*_tmp415=_tmp414;struct Cyc_Absyn_Aggrfield*_tmp41B;struct Cyc_Absyn_Exp*_tmp41A;_LL110: _tmp41B=_tmp415->f1;_tmp41A=_tmp415->f2;_LL111:;{
void*_tmp416=Cyc_Toc_typ_to_c(_tmp41B->type);
void*_tmp417=Cyc_Toc_typ_to_c((void*)_check_null(_tmp41A->topt));
Cyc_Toc_exp_to_c(nv,_tmp41A);
# 2786
if(!Cyc_Tcutil_unify(_tmp416,_tmp417)){
# 2788
if(!Cyc_Tcutil_is_arithmetic_type(_tmp416) || !
Cyc_Tcutil_is_arithmetic_type(_tmp417))
({struct Cyc_Absyn_Exp*_tmp959=({void*_tmp958=_tmp416;Cyc_Toc_cast_it(_tmp958,Cyc_Absyn_copy_exp(_tmp41A));});_tmp41A=_tmp959;});}
({struct Cyc_List_List*_tmp95B=({struct Cyc_List_List*_tmp419=_cycalloc(sizeof(*_tmp419));({struct _tuple19*_tmp95A=({struct _tuple19*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->f1=0,_tmp418->f2=_tmp41A;_tmp418;});_tmp419->hd=_tmp95A;}),_tmp419->tl=_tmp411;_tmp419;});_tmp411=_tmp95B;});
break;};}}}
# 2795
({void*_tmp95C=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp411));e->r=_tmp95C;});}}
# 2798
goto _LL7;};};};case 31U: _LL4A: _tmp48A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp489=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp488=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_LL4B: {
# 2802
struct Cyc_List_List*_tmp41C=_tmp488->typs;
{struct Cyc_List_List*_tmp41D=_tmp48A;for(0;_tmp41D != 0;(_tmp41D=_tmp41D->tl,_tmp41C=_tmp41C->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp41D->hd;
void*_tmp41E=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp41C))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp41E))
({void*_tmp95D=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp95D;});}}{
# 2813
struct Cyc_Absyn_Exp*_tmp41F=
_tmp489->is_extensible?Cyc_Absyn_var_exp(_tmp488->name,0U): Cyc_Toc_datatype_tag(_tmp489,_tmp488->name);
# 2816
if(!Cyc_Toc_is_toplevel(nv)){
# 2818
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp48A != 0;(_tmp48A=_tmp48A->tl,++ i)){
({struct Cyc_List_List*_tmp960=({struct Cyc_List_List*_tmp420=_cycalloc(sizeof(*_tmp420));({struct _tuple19*_tmp95F=({struct _dyneither_ptr*_tmp95E=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp95E,(struct Cyc_Absyn_Exp*)_tmp48A->hd);});_tmp420->hd=_tmp95F;}),_tmp420->tl=dles;_tmp420;});dles=_tmp960;});}}{
# 2824
struct _tuple19*_tmp421=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp41F);
({void*_tmp964=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->tag=29U,({struct _tuple1*_tmp963=Cyc_Toc_collapse_qvars(_tmp488->name,_tmp489->name);_tmp423->f1=_tmp963;}),_tmp423->f2=0,({
struct Cyc_List_List*_tmp962=({struct Cyc_List_List*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->hd=_tmp421,({struct Cyc_List_List*_tmp961=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp422->tl=_tmp961;});_tmp422;});_tmp423->f3=_tmp962;}),_tmp423->f4=0;_tmp423;});
# 2825
e->r=_tmp964;});};}else{
# 2830
struct Cyc_List_List*_tmp424=0;
for(0;_tmp48A != 0;_tmp48A=_tmp48A->tl){
({struct Cyc_List_List*_tmp966=({struct Cyc_List_List*_tmp426=_cycalloc(sizeof(*_tmp426));({struct _tuple19*_tmp965=({struct _tuple19*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->f1=0,_tmp425->f2=(struct Cyc_Absyn_Exp*)_tmp48A->hd;_tmp425;});_tmp426->hd=_tmp965;}),_tmp426->tl=_tmp424;_tmp426;});_tmp424=_tmp966;});}
({void*_tmp969=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp428=_cycalloc(sizeof(*_tmp428));({struct _tuple19*_tmp968=({struct _tuple19*_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427->f1=0,_tmp427->f2=_tmp41F;_tmp427;});_tmp428->hd=_tmp968;}),({
struct Cyc_List_List*_tmp967=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp424);_tmp428->tl=_tmp967;});_tmp428;}));
# 2833
e->r=_tmp969;});}
# 2836
goto _LL7;};}case 32U: _LL4C: _LL4D:
# 2838
 goto _LL4F;case 33U: _LL4E: _LL4F:
 goto _LL7;case 34U: _LL50: _tmp490=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).is_calloc;_tmp48F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).rgn;_tmp48E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).elt_type;_tmp48D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).num_elts;_tmp48C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).fat_result;_tmp48B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).inline_call;_LL51: {
# 2842
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp48E)));
Cyc_Toc_exp_to_c(nv,_tmp48D);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=_tmp48D;
if(_tmp48C){
({struct _tuple1*_tmp96A=Cyc_Toc_temp_var();x=_tmp96A;});
({struct Cyc_Absyn_Exp*_tmp96B=Cyc_Absyn_var_exp(x,0U);num_elts2=_tmp96B;});}
# 2851
if(_tmp490){
if(_tmp48F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp48F;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmp96E=({struct Cyc_Absyn_Exp*_tmp96D=rgn;struct Cyc_Absyn_Exp*_tmp96C=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmp96D,_tmp96C,num_elts2);});pexp=_tmp96E;});}else{
# 2857
({struct Cyc_Absyn_Exp*_tmp971=({void*_tmp970=*_tmp48E;struct Cyc_Absyn_Exp*_tmp96F=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp970,_tmp96F,num_elts2);});pexp=_tmp971;});}}else{
# 2859
if(_tmp48F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp48F;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp48B)
({struct Cyc_Absyn_Exp*_tmp972=Cyc_Toc_rmalloc_inline_exp(rgn,num_elts2);pexp=_tmp972;});else{
# 2865
({struct Cyc_Absyn_Exp*_tmp973=Cyc_Toc_rmalloc_exp(rgn,num_elts2);pexp=_tmp973;});}}else{
# 2867
({struct Cyc_Absyn_Exp*_tmp974=Cyc_Toc_malloc_exp(*_tmp48E,num_elts2);pexp=_tmp974;});}}
# 2869
if(_tmp48C){
struct Cyc_Absyn_Exp*elt_sz=_tmp490?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp975=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp975,({struct Cyc_Absyn_Exp*_tmp429[3U];_tmp429[0]=pexp,_tmp429[1]=elt_sz,_tmp429[2]=num_elts2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp429,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp978=(struct _tuple1*)_check_null(x);void*_tmp977=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp976=_tmp48D;Cyc_Absyn_declare_stmt(_tmp978,_tmp977,_tmp976,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp979=Cyc_Toc_stmt_exp_r(s);e->r=_tmp979;});}else{
# 2876
e->r=pexp->r;}
goto _LL7;};}case 35U: _LL52: _tmp492=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp491=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL53: {
# 2886
void*e1_old_typ=(void*)_check_null(_tmp492->topt);
void*e2_old_typ=(void*)_check_null(_tmp491->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp42A=0U;({struct _dyneither_ptr _tmp97A=({const char*_tmp42B="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp42B,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp97A,_tag_dyneither(_tmp42A,sizeof(void*),0U));});});{
# 2894
unsigned int _tmp42C=e->loc;
struct _tuple1*_tmp42D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_var_exp(_tmp42D,_tmp42C);_tmp42E->topt=e1_old_typ;{
struct _tuple1*_tmp42F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp430=Cyc_Absyn_var_exp(_tmp42F,_tmp42C);_tmp430->topt=e2_old_typ;{
# 2900
struct Cyc_Absyn_Exp*_tmp431=({struct Cyc_Absyn_Exp*_tmp97C=Cyc_Tcutil_deep_copy_exp(1,_tmp492);struct Cyc_Absyn_Exp*_tmp97B=_tmp430;Cyc_Absyn_assign_exp(_tmp97C,_tmp97B,_tmp42C);});_tmp431->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp432=Cyc_Absyn_exp_stmt(_tmp431,_tmp42C);
struct Cyc_Absyn_Exp*_tmp433=({struct Cyc_Absyn_Exp*_tmp97E=Cyc_Tcutil_deep_copy_exp(1,_tmp491);struct Cyc_Absyn_Exp*_tmp97D=_tmp42E;Cyc_Absyn_assign_exp(_tmp97E,_tmp97D,_tmp42C);});_tmp433->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp434=Cyc_Absyn_exp_stmt(_tmp433,_tmp42C);
# 2905
struct Cyc_Absyn_Stmt*_tmp435=({struct _tuple1*_tmp986=_tmp42D;void*_tmp985=e1_old_typ;struct Cyc_Absyn_Exp*_tmp984=_tmp492;struct Cyc_Absyn_Stmt*_tmp983=({
struct _tuple1*_tmp982=_tmp42F;void*_tmp981=e2_old_typ;struct Cyc_Absyn_Exp*_tmp980=_tmp491;struct Cyc_Absyn_Stmt*_tmp97F=
Cyc_Absyn_seq_stmt(_tmp432,_tmp434,_tmp42C);
# 2906
Cyc_Absyn_declare_stmt(_tmp982,_tmp981,_tmp980,_tmp97F,_tmp42C);});
# 2905
Cyc_Absyn_declare_stmt(_tmp986,_tmp985,_tmp984,_tmp983,_tmp42C);});
# 2908
Cyc_Toc_stmt_to_c(nv,_tmp435);
({void*_tmp987=Cyc_Toc_stmt_exp_r(_tmp435);e->r=_tmp987;});
goto _LL7;};};};};};}case 38U: _LL54: _tmp494=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp493=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL55: {
# 2913
void*_tmp436=Cyc_Tcutil_compress((void*)_check_null(_tmp494->topt));
Cyc_Toc_exp_to_c(nv,_tmp494);
{void*_tmp437=_tmp436;struct Cyc_Absyn_Aggrdecl*_tmp43D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->f1)->f1).KnownAggr).tag == 2){_LL113: _tmp43D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp437)->f1)->f1).KnownAggr).val;_LL114: {
# 2917
struct Cyc_Absyn_Exp*_tmp438=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp43D,_tmp493),0U);
struct Cyc_Absyn_Exp*_tmp439=Cyc_Toc_member_exp(_tmp494,_tmp493,0U);
struct Cyc_Absyn_Exp*_tmp43A=Cyc_Toc_member_exp(_tmp439,Cyc_Toc_tag_sp,0U);
({void*_tmp988=(Cyc_Absyn_eq_exp(_tmp43A,_tmp438,0U))->r;e->r=_tmp988;});
goto _LL112;}}else{goto _LL115;}}else{goto _LL115;}}else{_LL115: _LL116:
({void*_tmp43B=0U;({struct _dyneither_ptr _tmp989=({const char*_tmp43C="non-aggregate type in tagcheck";_tag_dyneither(_tmp43C,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp989,_tag_dyneither(_tmp43B,sizeof(void*),0U));});});}_LL112:;}
# 2924
goto _LL7;}case 37U: _LL56: _tmp495=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL57:
 Cyc_Toc_stmt_to_c(nv,_tmp495);goto _LL7;case 36U: _LL58: _LL59:
# 2927
({void*_tmp43E=0U;({struct _dyneither_ptr _tmp98A=({const char*_tmp43F="UnresolvedMem";_tag_dyneither(_tmp43F,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98A,_tag_dyneither(_tmp43E,sizeof(void*),0U));});});case 25U: _LL5A: _LL5B:
({void*_tmp440=0U;({struct _dyneither_ptr _tmp98B=({const char*_tmp441="compoundlit";_tag_dyneither(_tmp441,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp98B,_tag_dyneither(_tmp440,sizeof(void*),0U));});});case 39U: _LL5C: _LL5D:
({void*_tmp442=0U;({struct _dyneither_ptr _tmp98C=({const char*_tmp443="valueof(-)";_tag_dyneither(_tmp443,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98C,_tag_dyneither(_tmp442,sizeof(void*),0U));});});default: _LL5E: _LL5F:
({void*_tmp444=0U;({struct _dyneither_ptr _tmp98D=({const char*_tmp445="__asm__";_tag_dyneither(_tmp445,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98D,_tag_dyneither(_tmp444,sizeof(void*),0U));});});}_LL7:;}
# 2932
if(!did_inserted_checks)
({void*_tmp496=0U;({unsigned int _tmp991=e->loc;struct _dyneither_ptr _tmp990=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp499=({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0U,({
struct _dyneither_ptr _tmp98E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp71A.f1=_tmp98E;});_tmp71A;});void*_tmp497[1U];_tmp497[0]=& _tmp499;({struct _dyneither_ptr _tmp98F=({const char*_tmp498="Toc did not examine an inserted check: %s";_tag_dyneither(_tmp498,sizeof(char),42U);});Cyc_aprintf(_tmp98F,_tag_dyneither(_tmp497,sizeof(void*),1U));});});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp991,_tmp990,_tag_dyneither(_tmp496,sizeof(void*),0U));});});};}struct _tuple34{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2961 "toc.cyc"
static struct _tuple34*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2963
return({struct _tuple34*_tmp49A=_region_malloc(r,sizeof(*_tmp49A));_tmp49A->f1=0,({struct _dyneither_ptr*_tmp993=Cyc_Toc_fresh_label();_tmp49A->f2=_tmp993;}),({struct _dyneither_ptr*_tmp992=Cyc_Toc_fresh_label();_tmp49A->f3=_tmp992;}),_tmp49A->f4=sc;_tmp49A;});}
# 2968
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp49B=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 2974
if((int)(((_tmp49B->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp49E=_tmp49B->orig_pat;if((_tmp49E.pattern).tag != 1)_throw_match();(_tmp49E.pattern).val;})->topt);
void*_tmp49C=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp49D=_tmp49C;switch(*((int*)_tmp49D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49D)->f1)){case 0U: _LL1: _LL2:
# 2979
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmp995=({void*_tmp994=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp994,v);});v=_tmp995;});goto _LL0;}_LL0:;}{
# 2985
void*_tmp49F=_tmp49B->access;void*_tmp4A0=_tmp49F;struct Cyc_Absyn_Datatypedecl*_tmp4AB;struct Cyc_Absyn_Datatypefield*_tmp4AA;unsigned int _tmp4A9;int _tmp4A8;struct _dyneither_ptr*_tmp4A7;unsigned int _tmp4A6;switch(*((int*)_tmp4A0)){case 0U: _LLA: _LLB:
# 2990
 goto _LL9;case 1U: _LLC: _LLD:
# 2995
 if(ps->tl != 0){
void*_tmp4A1=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp4A2=_tmp4A1;struct Cyc_Absyn_Datatypedecl*_tmp4A5;struct Cyc_Absyn_Datatypefield*_tmp4A4;unsigned int _tmp4A3;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A2)->tag == 3U){_LL15: _tmp4A5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A2)->f1;_tmp4A4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A2)->f2;_tmp4A3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A2)->f3;_LL16:
# 2998
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmp998=({void*_tmp997=({void*_tmp996=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4A4->name,_tmp4A5->name));Cyc_Absyn_cstar_type(_tmp996,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp997,v);});v=_tmp998;});
({struct Cyc_Absyn_Exp*_tmp99A=({struct Cyc_Absyn_Exp*_tmp999=v;Cyc_Absyn_aggrarrow_exp(_tmp999,Cyc_Absyn_fieldname((int)(_tmp4A3 + 1)),0U);});v=_tmp99A;});
continue;}else{_LL17: _LL18:
# 3003
 goto _LL14;}_LL14:;}
# 3006
({struct Cyc_Absyn_Exp*_tmp99B=Cyc_Absyn_deref_exp(v,0U);v=_tmp99B;});
goto _LL9;case 2U: _LLE: _tmp4A6=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4A0)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmp99D=({struct Cyc_Absyn_Exp*_tmp99C=v;Cyc_Toc_member_exp(_tmp99C,Cyc_Absyn_fieldname((int)(_tmp4A6 + 1)),0U);});v=_tmp99D;});goto _LL9;case 4U: _LL10: _tmp4A8=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4A0)->f1;_tmp4A7=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4A0)->f2;_LL11:
# 3010
({struct Cyc_Absyn_Exp*_tmp99E=Cyc_Toc_member_exp(v,_tmp4A7,0U);v=_tmp99E;});
if(_tmp4A8)
({struct Cyc_Absyn_Exp*_tmp99F=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmp99F;});
goto _LL9;default: _LL12: _tmp4AB=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A0)->f1;_tmp4AA=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A0)->f2;_tmp4A9=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A0)->f3;_LL13:
# 3015
({struct Cyc_Absyn_Exp*_tmp9A1=({struct Cyc_Absyn_Exp*_tmp9A0=v;Cyc_Toc_member_exp(_tmp9A0,Cyc_Absyn_fieldname((int)(_tmp4A9 + 1)),0U);});v=_tmp9A1;});
goto _LL9;}_LL9:;};}
# 3019
return v;}
# 3024
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4AC=t;struct Cyc_Absyn_Datatypedecl*_tmp4C5;struct Cyc_Absyn_Datatypefield*_tmp4C4;struct _dyneither_ptr*_tmp4C3;int _tmp4C2;int _tmp4C1;struct Cyc_Absyn_Datatypedecl*_tmp4C0;struct Cyc_Absyn_Datatypefield*_tmp4BF;unsigned int _tmp4BE;struct _dyneither_ptr _tmp4BD;int _tmp4BC;void*_tmp4BB;struct Cyc_Absyn_Enumfield*_tmp4BA;struct Cyc_Absyn_Enumdecl*_tmp4B9;struct Cyc_Absyn_Enumfield*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;switch(*((int*)_tmp4AC)){case 0U: _LL1: _tmp4B7=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4AC)->f1;_LL2:
# 3028
 if(_tmp4B7 == 0)return v;else{return _tmp4B7;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmp9A2=v;Cyc_Absyn_eq_exp(_tmp9A2,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmp9A3=v;Cyc_Absyn_neq_exp(_tmp9A3,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4B9=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4AC)->f1;_tmp4B8=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4AC)->f2;_LL8:
# 3032
 return({struct Cyc_Absyn_Exp*_tmp9A4=v;Cyc_Absyn_eq_exp(_tmp9A4,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->tag=32U,_tmp4AD->f1=_tmp4B9,_tmp4AD->f2=_tmp4B8;_tmp4AD;}),0U),0U);});case 4U: _LL9: _tmp4BB=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4AC)->f1;_tmp4BA=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4AC)->f2;_LLA:
# 3034
 return({struct Cyc_Absyn_Exp*_tmp9A5=v;Cyc_Absyn_eq_exp(_tmp9A5,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE->tag=33U,_tmp4AE->f1=_tmp4BB,_tmp4AE->f2=_tmp4BA;_tmp4AE;}),0U),0U);});case 5U: _LLB: _tmp4BD=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4AC)->f1;_tmp4BC=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4AC)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmp9A6=v;Cyc_Absyn_eq_exp(_tmp9A6,Cyc_Absyn_float_exp(_tmp4BD,_tmp4BC,0U),0U);});case 6U: _LLD: _tmp4BE=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4AC)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmp9A7=v;Cyc_Absyn_eq_exp(_tmp9A7,Cyc_Absyn_signed_int_exp((int)_tmp4BE,0U),0U);});case 7U: _LLF: _tmp4C1=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4AC)->f1;_tmp4C0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4AC)->f2;_tmp4BF=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4AC)->f3;_LL10:
# 3040
 LOOP1: {
void*_tmp4AF=v->r;void*_tmp4B0=_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B1;switch(*((int*)_tmp4B0)){case 14U: _LL16: _tmp4B1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL17:
 v=_tmp4B1;goto LOOP1;case 20U: _LL18: _tmp4B2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL19:
 v=_tmp4B2;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3047
({struct Cyc_Absyn_Exp*_tmp9AA=({void*_tmp9A9=({void*_tmp9A8=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4BF->name,_tmp4C0->name));Cyc_Absyn_cstar_type(_tmp9A8,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9A9,v);});v=_tmp9AA;});
return({struct Cyc_Absyn_Exp*_tmp9AB=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp9AB,Cyc_Absyn_uint_exp((unsigned int)_tmp4C1,0U),0U);});case 8U: _LL11: _tmp4C3=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4AC)->f1;_tmp4C2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4AC)->f2;_LL12:
# 3052
({struct Cyc_Absyn_Exp*_tmp9AD=({struct Cyc_Absyn_Exp*_tmp9AC=Cyc_Toc_member_exp(v,_tmp4C3,0U);Cyc_Toc_member_exp(_tmp9AC,Cyc_Toc_tag_sp,0U);});v=_tmp9AD;});
return({struct Cyc_Absyn_Exp*_tmp9AE=v;Cyc_Absyn_eq_exp(_tmp9AE,Cyc_Absyn_signed_int_exp(_tmp4C2,0U),0U);});default: _LL13: _tmp4C5=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4AC)->f1;_tmp4C4=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4AC)->f2;_LL14:
# 3056
 LOOP2: {
void*_tmp4B3=v->r;void*_tmp4B4=_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B5;switch(*((int*)_tmp4B4)){case 14U: _LL1D: _tmp4B5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B4)->f2;_LL1E:
 v=_tmp4B5;goto LOOP2;case 20U: _LL1F: _tmp4B6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B4)->f1;_LL20:
 v=_tmp4B6;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3063
({struct Cyc_Absyn_Exp*_tmp9B1=({void*_tmp9B0=({void*_tmp9AF=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4C4->name,_tmp4C5->name));Cyc_Absyn_cstar_type(_tmp9AF,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9B0,v);});v=_tmp9B1;});
return({struct Cyc_Absyn_Exp*_tmp9B2=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp9B2,Cyc_Absyn_var_exp(_tmp4C4->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3077
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple35{int f1;void*f2;};
# 3084
static struct _tuple35 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3086
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple31 _tmp4C6=*((struct _tuple31*)ss->hd);struct _tuple31 _tmp4C7=_tmp4C6;void*_tmp4CD;_LL1: _tmp4CD=_tmp4C7.f1;_LL2:;{
void*_tmp4C8=_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CC;struct _dyneither_ptr*_tmp4CB;switch(*((int*)_tmp4C8)){case 3U: _LL4: _LL5:
# 3092
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3095
 continue;case 8U: _LLA: _tmp4CB=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4C8)->f1;_LLB:
# 3097
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmp9B3=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->tag=3U,_tmp4C9->f1=_tmp4CB;_tmp4C9;});k=_tmp9B3;});
continue;case 7U: _LLC: _LLD:
# 3101
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp4CC=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4C8)->f1;if(_tmp4CC != 0){_LLF:
# 3105
({void*_tmp9B4=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->tag=2U,_tmp4CA->f1=_tmp4CC;_tmp4CA;});k=_tmp9B4;});
return({struct _tuple35 _tmp71B;_tmp71B.f1=0,_tmp71B.f2=k;_tmp71B;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple35 _tmp71C;_tmp71C.f1=0,_tmp71C.f2=k;_tmp71C;});}_LL3:;};}
# 3114
return({struct _tuple35 _tmp71D;_tmp71D.f1=c,_tmp71D.f2=k;_tmp71D;});}
# 3119
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4CE=p;int _tmp4D9;int _tmp4D8;unsigned int _tmp4D7;void*_tmp4D6;struct Cyc_Absyn_Enumfield*_tmp4D5;struct Cyc_Absyn_Enumdecl*_tmp4D4;struct Cyc_Absyn_Enumfield*_tmp4D3;switch(*((int*)_tmp4CE)){case 3U: _LL1: _tmp4D4=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4CE)->f1;_tmp4D3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4CE)->f2;_LL2:
({struct Cyc_Absyn_Exp*_tmp9B5=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF->tag=32U,_tmp4CF->f1=_tmp4D4,_tmp4CF->f2=_tmp4D3;_tmp4CF;}),0U);e=_tmp9B5;});goto _LL0;case 4U: _LL3: _tmp4D6=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4CE)->f1;_tmp4D5=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4CE)->f2;_LL4:
({struct Cyc_Absyn_Exp*_tmp9B6=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->tag=33U,_tmp4D0->f1=_tmp4D6,_tmp4D0->f2=_tmp4D5;_tmp4D0;}),0U);e=_tmp9B6;});goto _LL0;case 6U: _LL5: _tmp4D7=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4CE)->f1;_LL6:
 _tmp4D8=(int)_tmp4D7;goto _LL8;case 7U: _LL7: _tmp4D8=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4CE)->f1;_LL8:
 _tmp4D9=_tmp4D8;goto _LLA;case 8U: _LL9: _tmp4D9=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4CE)->f2;_LLA:
({struct Cyc_Absyn_Exp*_tmp9B7=Cyc_Absyn_uint_exp((unsigned int)_tmp4D9,0U);e=_tmp9B7;});goto _LL0;default: _LLB: _LLC:
({void*_tmp4D1=0U;({struct _dyneither_ptr _tmp9B8=({const char*_tmp4D2="compile_pat_test_as_case!";_tag_dyneither(_tmp4D2,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9B8,_tag_dyneither(_tmp4D1,sizeof(void*),0U));});});}_LL0:;}
# 3129
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));_tmp4DA->tag=17U,_tmp4DA->f1=e;_tmp4DA;}),0U);}
# 3133
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple36{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3142
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3145
void*t=(void*)_check_null(p->topt);
void*_tmp4DB=p->r;void*_tmp4DC=_tmp4DB;union Cyc_Absyn_AggrInfo _tmp50D;struct Cyc_List_List*_tmp50C;struct Cyc_List_List*_tmp50B;struct Cyc_List_List*_tmp50A;struct Cyc_Absyn_Pat*_tmp509;struct Cyc_Absyn_Datatypedecl*_tmp508;struct Cyc_Absyn_Datatypefield*_tmp507;struct Cyc_List_List*_tmp506;struct Cyc_Absyn_Vardecl*_tmp505;struct Cyc_Absyn_Pat*_tmp504;struct Cyc_Absyn_Vardecl*_tmp503;struct Cyc_Absyn_Vardecl*_tmp502;struct Cyc_Absyn_Pat*_tmp501;struct Cyc_Absyn_Vardecl*_tmp500;switch(*((int*)_tmp4DC)){case 2U: _LL1: _tmp500=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4DC)->f2;_LL2: {
# 3148
struct Cyc_Absyn_Pat*_tmp4DD=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp4DD->topt=p->topt;
_tmp502=_tmp500;_tmp501=_tmp4DD;goto _LL4;}case 1U: _LL3: _tmp502=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1;_tmp501=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4DC)->f2;_LL4:
# 3153
({struct Cyc_List_List*_tmp9B9=({struct Cyc_List_List*_tmp4DE=_region_malloc(rgn,sizeof(*_tmp4DE));_tmp4DE->hd=_tmp502,_tmp4DE->tl=*decls;_tmp4DE;});*decls=_tmp9B9;});{
struct Cyc_Absyn_Stmt*_tmp4DF=({struct Cyc_Absyn_Exp*_tmp9BA=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->tag=4U,_tmp4E0->f1=_tmp502;_tmp4E0;}),0U);Cyc_Absyn_assign_stmt(_tmp9BA,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmp9BB=_tmp4DF;Cyc_Toc_seq_stmt_opt(_tmp9BB,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp501));});};case 4U: _LL5: _tmp503=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4DC)->f2;_LL6:
# 3158
({struct Cyc_List_List*_tmp9BC=({struct Cyc_List_List*_tmp4E1=_region_malloc(rgn,sizeof(*_tmp4E1));_tmp4E1->hd=_tmp503,_tmp4E1->tl=*decls;_tmp4E1;});*decls=_tmp9BC;});
return({struct Cyc_Absyn_Exp*_tmp9BD=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->tag=4U,_tmp4E2->f1=_tmp503;_tmp4E2;}),0U);Cyc_Absyn_assign_stmt(_tmp9BD,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp505=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1;_tmp504=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4DC)->f2;_LLA:
# 3163
({struct Cyc_List_List*_tmp9BE=({struct Cyc_List_List*_tmp4E3=_region_malloc(rgn,sizeof(*_tmp4E3));_tmp4E3->hd=_tmp505,_tmp4E3->tl=*decls;_tmp4E3;});*decls=_tmp9BE;});
({void*_tmp9BF=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp505->type=_tmp9BF;});{
# 3166
struct Cyc_Absyn_Stmt*_tmp4E4=({struct Cyc_Absyn_Exp*_tmp9C2=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));_tmp4E5->tag=4U,_tmp4E5->f1=_tmp505;_tmp4E5;}),0U);Cyc_Absyn_assign_stmt(_tmp9C2,({
void*_tmp9C1=({void*_tmp9C0=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp9C0,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9C1,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmp9C3=_tmp4E4;Cyc_Toc_seq_stmt_opt(_tmp9C3,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp504));});};case 9U: _LLB: _LLC:
# 3171
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1)->r)->tag == 8U){_LL17: _tmp508=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1)->r)->f1;_tmp507=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1)->r)->f2;_tmp506=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1)->r)->f3;_LL18:
# 3180
 if(_tmp506 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp507->name,_tmp508->name);
void*_tmp4E6=({void*_tmp9C4=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmp9C4,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmp9C5=Cyc_Toc_cast_it(_tmp4E6,path);path=_tmp9C5;});{
int cnt=1;
struct Cyc_List_List*_tmp4E7=_tmp507->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp506 != 0;(_tmp506=_tmp506->tl,_tmp4E7=((struct Cyc_List_List*)_check_null(_tmp4E7))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4E8=(struct Cyc_Absyn_Pat*)_tmp506->hd;
if(_tmp4E8->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp4E9=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4E7))->hd)).f2;
void*_tmp4EA=(void*)_check_null(_tmp4E8->topt);
void*_tmp4EB=Cyc_Toc_typ_to_c_array(_tmp4EA);
struct Cyc_Absyn_Exp*_tmp4EC=({struct Cyc_Absyn_Exp*_tmp9C6=path;Cyc_Absyn_aggrarrow_exp(_tmp9C6,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp4E9)))
({struct Cyc_Absyn_Exp*_tmp9C7=Cyc_Toc_cast_it(_tmp4EB,_tmp4EC);_tmp4EC=_tmp9C7;});
({struct Cyc_Absyn_Stmt*_tmp9C9=({struct Cyc_Absyn_Stmt*_tmp9C8=s;Cyc_Toc_seq_stmt_opt(_tmp9C8,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4EC,_tmp4E8));});s=_tmp9C9;});};}
# 3198
return s;};};}else{_LL21: _tmp509=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1;_LL22:
# 3243
 return({struct _RegionHandle*_tmp9CD=rgn;struct Cyc_Toc_Env**_tmp9CC=nv;struct Cyc_List_List**_tmp9CB=decls;struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmp9CD,_tmp9CC,_tmp9CB,_tmp9CA,_tmp509);});}case 8U: _LL19: _tmp50A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4DC)->f3;_LL1A:
# 3200
 _tmp50B=_tmp50A;goto _LL1C;case 5U: _LL1B: _tmp50B=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1;_LL1C: {
# 3202
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp50B != 0;(_tmp50B=_tmp50B->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4ED=(struct Cyc_Absyn_Pat*)_tmp50B->hd;
if(_tmp4ED->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp4EE=(void*)_check_null(_tmp4ED->topt);
struct _dyneither_ptr*_tmp4EF=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmp9D3=({struct Cyc_Absyn_Stmt*_tmp9D2=s;Cyc_Toc_seq_stmt_opt(_tmp9D2,({struct _RegionHandle*_tmp9D1=rgn;struct Cyc_Toc_Env**_tmp9D0=nv;struct Cyc_List_List**_tmp9CF=decls;struct Cyc_Absyn_Exp*_tmp9CE=Cyc_Toc_member_exp(path,_tmp4EF,0U);Cyc_Toc_extract_pattern_vars(_tmp9D1,_tmp9D0,_tmp9CF,_tmp9CE,_tmp4ED);}));});s=_tmp9D3;});};}
# 3212
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1 == 0){_LL1D: _LL1E:
({void*_tmp4F0=0U;({struct _dyneither_ptr _tmp9D4=({const char*_tmp4F1="unresolved aggregate pattern!";_tag_dyneither(_tmp4F1,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D4,_tag_dyneither(_tmp4F0,sizeof(void*),0U));});});}else{_LL1F: _tmp50D=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4DC)->f1;_tmp50C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4DC)->f3;_LL20: {
# 3216
struct Cyc_Absyn_Aggrdecl*_tmp4F2=Cyc_Absyn_get_known_aggrdecl(_tmp50D);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp50C != 0;_tmp50C=_tmp50C->tl){
struct _tuple36*_tmp4F3=(struct _tuple36*)_tmp50C->hd;
struct Cyc_Absyn_Pat*_tmp4F4=(*_tmp4F3).f2;
if(_tmp4F4->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp4F5=Cyc_Absyn_designatorlist_to_fieldname((*_tmp4F3).f1);
void*_tmp4F6=(void*)_check_null(_tmp4F4->topt);
void*_tmp4F7=Cyc_Toc_typ_to_c_array(_tmp4F6);
struct Cyc_Absyn_Exp*_tmp4F8=Cyc_Toc_member_exp(path,_tmp4F5,0U);
# 3228
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F2->impl))->tagged)({struct Cyc_Absyn_Exp*_tmp9D5=Cyc_Toc_member_exp(_tmp4F8,Cyc_Toc_val_sp,0U);_tmp4F8=_tmp9D5;});{
void*_tmp4F9=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F2->impl))->fields,_tmp4F5)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp4F9))
({struct Cyc_Absyn_Exp*_tmp9D6=Cyc_Toc_cast_it(_tmp4F7,_tmp4F8);_tmp4F8=_tmp9D6;});else{
if(!Cyc_Tcutil_is_array_type(_tmp4F9) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4F9)))
# 3235
({struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Absyn_deref_exp(({void*_tmp9D7=Cyc_Absyn_cstar_type(_tmp4F7,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp9D7,
Cyc_Absyn_address_exp(_tmp4F8,0U));}),0U);
# 3235
_tmp4F8=_tmp9D8;});}
# 3238
({struct Cyc_Absyn_Stmt*_tmp9DA=({struct Cyc_Absyn_Stmt*_tmp9D9=s;Cyc_Toc_seq_stmt_opt(_tmp9D9,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4F8,_tmp4F4));});s=_tmp9DA;});};};}
# 3240
return s;}}case 15U: _LL23: _LL24:
# 3245
({void*_tmp4FA=0U;({struct _dyneither_ptr _tmp9DB=({const char*_tmp4FB="unknownid pat";_tag_dyneither(_tmp4FB,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9DB,_tag_dyneither(_tmp4FA,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp4FC=0U;({struct _dyneither_ptr _tmp9DC=({const char*_tmp4FD="unknowncall pat";_tag_dyneither(_tmp4FD,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9DC,_tag_dyneither(_tmp4FC,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp4FE=0U;({struct _dyneither_ptr _tmp9DD=({const char*_tmp4FF="exp pat";_tag_dyneither(_tmp4FF,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9DD,_tag_dyneither(_tmp4FE,sizeof(void*),0U));});});}_LL0:;}struct _tuple37{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3256
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3264
void*_tmp50E=dopt;struct Cyc_List_List*_tmp54C;struct Cyc_List_List*_tmp54B;void*_tmp54A;struct Cyc_Tcpat_Rhs*_tmp549;if(_tmp50E == 0){_LL1: _LL2:
# 3266
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp50E)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp549=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp50E)->f1;_LL6:
# 3271
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple34*_tmp50F=(struct _tuple34*)lscs->hd;struct _tuple34*_tmp510=_tmp50F;int*_tmp517;struct _dyneither_ptr*_tmp516;struct _dyneither_ptr*_tmp515;struct Cyc_Absyn_Switch_clause*_tmp514;_LLA: _tmp517=(int*)& _tmp510->f1;_tmp516=_tmp510->f2;_tmp515=_tmp510->f3;_tmp514=_tmp510->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp511=_tmp514->body;
if(_tmp511 == _tmp549->rhs){
# 3276
if(*_tmp517)
return Cyc_Absyn_goto_stmt(_tmp516,0U);
*_tmp517=1;{
# 3280
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp9E0=rgn;struct Cyc_List_List**_tmp9DF=decls;struct Cyc_Absyn_Exp*_tmp9DE=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp9E0,& nv,_tmp9DF,_tmp9DE,_tmp514->pattern);});
# 3283
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp515,_tmp514->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmp9E1=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmp9E1;});
({struct Cyc_Absyn_Stmt*_tmp9E2=Cyc_Absyn_label_stmt(_tmp516,res,0U);res=_tmp9E2;});
({struct Cyc_List_List*_tmp9E4=({struct Cyc_List_List*_tmp513=_region_malloc(rgn,sizeof(*_tmp513));({struct _tuple37*_tmp9E3=({struct _tuple37*_tmp512=_region_malloc(rgn,sizeof(*_tmp512));_tmp512->f1=nv,_tmp512->f2=_tmp515,_tmp512->f3=_tmp511;_tmp512;});_tmp513->hd=_tmp9E3;}),_tmp513->tl=*bodies;_tmp513;});*bodies=_tmp9E4;});
return res;};}};}
# 3291
({void*_tmp518=0U;({struct _dyneither_ptr _tmp9E5=({const char*_tmp519="couldn't find rhs!";_tag_dyneither(_tmp519,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E5,_tag_dyneither(_tmp518,sizeof(void*),0U));});});default: _LL7: _tmp54C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp50E)->f1;_tmp54B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp50E)->f2;_tmp54A=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp50E)->f3;_LL8: {
# 3294
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp54A,lscs,v);
# 3296
struct Cyc_Absyn_Exp*_tmp51A=({struct Cyc_List_List*_tmp9E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp54C);Cyc_Toc_compile_path(_tmp9E6,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp51B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp54B);
# 3299
struct _tuple35 _tmp51C=Cyc_Toc_admits_switch(_tmp51B);struct _tuple35 _tmp51D=_tmp51C;int _tmp548;void*_tmp547;_LLD: _tmp548=_tmp51D.f1;_tmp547=_tmp51D.f2;_LLE:;
if(_tmp548 > 1){
# 3303
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp52E=_cycalloc(sizeof(*_tmp52E));
({struct Cyc_Absyn_Switch_clause*_tmp9E8=({struct Cyc_Absyn_Switch_clause*_tmp52D=_cycalloc(sizeof(*_tmp52D));({struct Cyc_Absyn_Pat*_tmp9E7=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp52D->pattern=_tmp9E7;}),_tmp52D->pat_vars=0,_tmp52D->where_clause=0,_tmp52D->body=res,_tmp52D->loc=0U;_tmp52D;});_tmp52E->hd=_tmp9E8;}),_tmp52E->tl=0;_tmp52E;});
# 3306
for(0;_tmp51B != 0;_tmp51B=_tmp51B->tl){
struct _tuple31 _tmp51E=*((struct _tuple31*)_tmp51B->hd);struct _tuple31 _tmp51F=_tmp51E;void*_tmp525;void*_tmp524;_LL10: _tmp525=_tmp51F.f1;_tmp524=_tmp51F.f2;_LL11:;{
# 3309
struct Cyc_Absyn_Pat*_tmp520=Cyc_Toc_compile_pat_test_as_case(_tmp525);
# 3311
struct Cyc_Absyn_Stmt*_tmp521=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp524,lscs,v);
# 3313
({struct Cyc_List_List*_tmp9EA=({struct Cyc_List_List*_tmp523=_cycalloc(sizeof(*_tmp523));({struct Cyc_Absyn_Switch_clause*_tmp9E9=({struct Cyc_Absyn_Switch_clause*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->pattern=_tmp520,_tmp522->pat_vars=0,_tmp522->where_clause=0,_tmp522->body=_tmp521,_tmp522->loc=0U;_tmp522;});_tmp523->hd=_tmp9E9;}),_tmp523->tl=new_lscs;_tmp523;});new_lscs=_tmp9EA;});};}
# 3315
{void*_tmp526=_tmp547;struct _dyneither_ptr*_tmp52B;switch(*((int*)_tmp526)){case 1U: _LL13: _LL14:
# 3318
 LOOP1: {
void*_tmp527=_tmp51A->r;void*_tmp528=_tmp527;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;switch(*((int*)_tmp528)){case 14U: _LL1C: _tmp529=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp528)->f2;_LL1D:
 _tmp51A=_tmp529;goto LOOP1;case 20U: _LL1E: _tmp52A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp528)->f1;_LL1F:
 _tmp51A=_tmp52A;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3324
({struct Cyc_Absyn_Exp*_tmp9EC=Cyc_Absyn_deref_exp(({void*_tmp9EB=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp9EB,_tmp51A);}),0U);_tmp51A=_tmp9EC;});goto _LL12;case 3U: _LL15: _tmp52B=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp526)->f1;_LL16:
# 3327
({struct Cyc_Absyn_Exp*_tmp9EE=({struct Cyc_Absyn_Exp*_tmp9ED=Cyc_Toc_member_exp(_tmp51A,_tmp52B,0U);Cyc_Toc_member_exp(_tmp9ED,Cyc_Toc_tag_sp,0U);});_tmp51A=_tmp9EE;});goto _LL12;case 2U: _LL17: _LL18:
# 3329
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3332
({struct Cyc_Absyn_Stmt*_tmp9EF=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->tag=10U,_tmp52C->f1=_tmp51A,_tmp52C->f2=new_lscs,_tmp52C->f3=0;_tmp52C;}),0U);res=_tmp9EF;});}else{
# 3336
void*_tmp52F=_tmp547;struct Cyc_Absyn_Exp*_tmp546;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp52F)->tag == 2U){_LL23: _tmp546=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp52F)->f1;_LL24: {
# 3338
struct Cyc_List_List*_tmp530=_tmp51B;void*_tmp53F;struct Cyc_Tcpat_Rhs*_tmp53E;if(_tmp530 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple31*)((struct Cyc_List_List*)_tmp530)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp530)->tl == 0){_LL28: _tmp53F=((struct _tuple31*)_tmp530->hd)->f1;_tmp53E=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple31*)_tmp530->hd)->f2)->f1;_LL29:
# 3344
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple34*_tmp531=(struct _tuple34*)lscs->hd;struct _tuple34*_tmp532=_tmp531;int*_tmp539;struct _dyneither_ptr*_tmp538;struct _dyneither_ptr*_tmp537;struct Cyc_Absyn_Switch_clause*_tmp536;_LL2D: _tmp539=(int*)& _tmp532->f1;_tmp538=_tmp532->f2;_tmp537=_tmp532->f3;_tmp536=_tmp532->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp533=_tmp536->body;
if(_tmp533 == _tmp53E->rhs){
# 3349
if(*_tmp539)
return Cyc_Absyn_goto_stmt(_tmp538,0U);
*_tmp539=1;{
# 3353
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp9F2=rgn;struct Cyc_List_List**_tmp9F1=decls;struct Cyc_Absyn_Exp*_tmp9F0=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp9F2,& nv,_tmp9F1,_tmp9F0,_tmp536->pattern);});
# 3356
Cyc_Toc_exp_to_c(nv,_tmp546);{
# 3359
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp537,_tmp536->body,0U);
({struct Cyc_Absyn_Stmt*_tmp9F3=Cyc_Absyn_ifthenelse_stmt(_tmp546,r,res,0U);r=_tmp9F3;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmp9F4=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmp9F4;});
({struct Cyc_Absyn_Stmt*_tmp9F5=Cyc_Absyn_label_stmt(_tmp538,r,0U);r=_tmp9F5;});
({struct Cyc_List_List*_tmp9F7=({struct Cyc_List_List*_tmp535=_region_malloc(rgn,sizeof(*_tmp535));({struct _tuple37*_tmp9F6=({struct _tuple37*_tmp534=_region_malloc(rgn,sizeof(*_tmp534));_tmp534->f1=nv,_tmp534->f2=_tmp537,_tmp534->f3=_tmp533;_tmp534;});_tmp535->hd=_tmp9F6;}),_tmp535->tl=*bodies;_tmp535;});*bodies=_tmp9F7;});
return r;};};}};}
# 3368
({void*_tmp53A=0U;({struct _dyneither_ptr _tmp9F8=({const char*_tmp53B="couldn't find rhs!";_tag_dyneither(_tmp53B,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F8,_tag_dyneither(_tmp53A,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp53C=0U;({struct _dyneither_ptr _tmp9F9=({const char*_tmp53D="bad where clause in match compiler";_tag_dyneither(_tmp53D,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F9,_tag_dyneither(_tmp53C,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3373
 for(0;_tmp51B != 0;_tmp51B=_tmp51B->tl){
struct _tuple31 _tmp540=*((struct _tuple31*)_tmp51B->hd);struct _tuple31 _tmp541=_tmp540;void*_tmp545;void*_tmp544;_LL30: _tmp545=_tmp541.f1;_tmp544=_tmp541.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp542=Cyc_Toc_compile_pat_test(_tmp51A,_tmp545);
struct Cyc_Absyn_Stmt*_tmp543=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp544,lscs,v);
({struct Cyc_Absyn_Stmt*_tmp9FA=Cyc_Absyn_ifthenelse_stmt(_tmp542,_tmp543,res,0U);res=_tmp9FA;});};}}_LL22:;}
# 3381
return res;}}}_LL0:;}
# 3391
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3393
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple37*_tmp54D=(struct _tuple37*)bodies->hd;struct _tuple37*_tmp54E=_tmp54D;struct Cyc_Toc_Env**_tmp550;struct Cyc_Absyn_Stmt*_tmp54F;_LL1: _tmp550=(struct Cyc_Toc_Env**)& _tmp54E->f1;_tmp54F=_tmp54E->f3;_LL2:;
if(_tmp54F == s)return _tmp550;}
# 3399
return 0;}
# 3403
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3406
void*_tmp551=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3409
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp552=_new_region("rgn");struct _RegionHandle*rgn=& _tmp552;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp553=Cyc_Toc_share_env(rgn,nv);
# 3416
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple34*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3421
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp553,& mydecls,& mybodies,dopt,lscs,v);
# 3429
{struct Cyc_List_List*_tmp554=lscs;for(0;_tmp554 != 0;_tmp554=_tmp554->tl){
struct _tuple34*_tmp555=(struct _tuple34*)_tmp554->hd;struct _tuple34*_tmp556=_tmp555;struct _dyneither_ptr*_tmp55E;struct Cyc_Absyn_Switch_clause*_tmp55D;_LL1: _tmp55E=_tmp556->f3;_tmp55D=_tmp556->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp55D->body;
# 3433
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3436
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp557=*envp;
# 3439
if(_tmp554->tl != 0){
struct _tuple34*_tmp558=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp554->tl))->hd;struct _tuple34*_tmp559=_tmp558;struct _dyneither_ptr*_tmp55C;struct Cyc_Absyn_Switch_clause*_tmp55B;_LL4: _tmp55C=_tmp559->f3;_tmp55B=_tmp559->f4;_LL5:;{
# 3443
struct Cyc_Toc_Env**_tmp55A=Cyc_Toc_find_case_env(mybodies,_tmp55B->body);
# 3448
if(_tmp55A == 0)
({struct Cyc_Toc_Env*_tmp9FB=Cyc_Toc_last_switchclause_env(rgn,_tmp557,end_l);Cyc_Toc_stmt_to_c(_tmp9FB,s);});else{
# 3452
struct Cyc_List_List*vs=0;
if(_tmp55B->pat_vars != 0){
({struct Cyc_List_List*_tmp9FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp55B->pat_vars))->v)).f1);vs=_tmp9FC;});
({struct Cyc_List_List*_tmp9FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmp9FD;});}
# 3457
({struct Cyc_Toc_Env*_tmp9FE=Cyc_Toc_non_last_switchclause_env(rgn,_tmp557,end_l,_tmp55C,vs);Cyc_Toc_stmt_to_c(_tmp9FE,s);});}};}else{
# 3461
({struct Cyc_Toc_Env*_tmp9FF=Cyc_Toc_last_switchclause_env(rgn,_tmp557,end_l);Cyc_Toc_stmt_to_c(_tmp9FF,s);});}};};}}{
# 3465
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA01=test_tree;Cyc_Absyn_seq_stmt(_tmpA01,({struct _dyneither_ptr*_tmpA00=end_l;Cyc_Absyn_label_stmt(_tmpA00,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3467
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp55F=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpA02=Cyc_Toc_temp_var();_tmp55F->name=_tmpA02;});
({void*_tmpA03=Cyc_Toc_typ_to_c_array(_tmp55F->type);_tmp55F->type=_tmpA03;});
({struct Cyc_Absyn_Stmt*_tmpA05=({struct Cyc_Absyn_Decl*_tmpA04=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->tag=0U,_tmp560->f1=_tmp55F;_tmp560;}),0U);Cyc_Absyn_decl_stmt(_tmpA04,res,0U);});res=_tmpA05;});}
# 3474
({void*_tmpA09=({struct _tuple1*_tmpA08=v;void*_tmpA07=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA06=e;Cyc_Absyn_declare_stmt(_tmpA08,_tmpA07,_tmpA06,res,0U);})->r;whole_s->r=_tmpA09;});
_npop_handler(0U);return;};}
# 3413
;_pop_region(rgn);};}
# 3480
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3482
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3487
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3489
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA0B=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpA0B,({struct Cyc_List_List*_tmp561=_cycalloc(sizeof(*_tmp561));
({struct Cyc_Absyn_Exp*_tmpA0A=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp561->hd=_tmpA0A;}),_tmp561->tl=0;_tmp561;}),0U);}),0U);}
# 3493
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA0D=({struct Cyc_Absyn_Stmt*_tmpA0C=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA0C,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA0D;});}
# 3498
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3500
while(1){
void*_tmp562=s->r;void*_tmp563=_tmp562;struct Cyc_Absyn_Stmt*_tmp5B9;struct Cyc_List_List*_tmp5B8;void*_tmp5B7;struct Cyc_Absyn_Stmt*_tmp5B6;struct Cyc_Absyn_Exp*_tmp5B5;struct _dyneither_ptr*_tmp5B4;struct Cyc_Absyn_Stmt*_tmp5B3;struct Cyc_Absyn_Decl*_tmp5B2;struct Cyc_Absyn_Stmt*_tmp5B1;struct Cyc_List_List*_tmp5B0;struct Cyc_Absyn_Switch_clause**_tmp5AF;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_List_List*_tmp5AD;void*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_Exp*_tmp5A9;struct Cyc_Absyn_Stmt*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A7;struct Cyc_Absyn_Stmt*_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Stmt*_tmp5A4;struct Cyc_Absyn_Stmt*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A2;struct Cyc_Absyn_Stmt*_tmp5A1;struct Cyc_Absyn_Stmt*_tmp5A0;struct Cyc_Absyn_Exp*_tmp59F;switch(*((int*)_tmp563)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp59F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_LL4:
 Cyc_Toc_exp_to_c(nv,_tmp59F);return;case 2U: _LL5: _tmp5A1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_tmp5A0=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp563)->f2;_LL6:
# 3505
 Cyc_Toc_stmt_to_c(nv,_tmp5A1);
s=_tmp5A0;
continue;case 3U: _LL7: _tmp5A2=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_LL8:
# 3509
 if(_tmp5A2 != 0){
void*t=Cyc_Toc_typ_to_c((void*)_check_null(_tmp5A2->topt));
Cyc_Toc_exp_to_c(nv,_tmp5A2);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
# 3515
struct _tuple1*_tmp564=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp565=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp564,0U),0U);
Cyc_Toc_do_npop_before(npop,_tmp565);
({void*_tmpA0E=(Cyc_Absyn_declare_stmt(_tmp564,t,_tmp5A2,_tmp565,0U))->r;s->r=_tmpA0E;});}};}else{
# 3521
({int _tmpA0F=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA0F,s);});}
return;case 4U: _LL9: _tmp5A5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_tmp5A4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp563)->f2;_tmp5A3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp563)->f3;_LLA:
# 3524
 Cyc_Toc_exp_to_c(nv,_tmp5A5);
Cyc_Toc_stmt_to_c(nv,_tmp5A4);
s=_tmp5A3;
continue;case 5U: _LLB: _tmp5A7=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp563)->f1).f1;_tmp5A6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp563)->f2;_LLC:
# 3529
 Cyc_Toc_exp_to_c(nv,_tmp5A7);{
struct _RegionHandle _tmp566=_new_region("temp");struct _RegionHandle*temp=& _tmp566;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA10=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA10,_tmp5A6);});
# 3533
_npop_handler(0U);return;
# 3530
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3535
struct Cyc_Toc_Env*_tmp567=nv;struct _dyneither_ptr**_tmp568;_LL22: _tmp568=_tmp567->break_lab;_LL23:;
if(_tmp568 != 0)
({void*_tmpA11=Cyc_Toc_goto_stmt_r(*_tmp568);s->r=_tmpA11;});
# 3539
({int _tmpA12=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA12,s);});
return;}case 7U: _LLF: _LL10: {
# 3542
struct Cyc_Toc_Env*_tmp569=nv;struct _dyneither_ptr**_tmp56A;_LL25: _tmp56A=_tmp569->continue_lab;_LL26:;
if(_tmp56A != 0)
({void*_tmpA13=Cyc_Toc_goto_stmt_r(*_tmp56A);s->r=_tmpA13;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3548
({int _tmpA14=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA14,s);});
return;case 9U: _LL13: _tmp5AB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_tmp5AA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp563)->f2).f1;_tmp5A9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp563)->f3).f1;_tmp5A8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp563)->f4;_LL14:
# 3552
 Cyc_Toc_exp_to_c(nv,_tmp5AB);Cyc_Toc_exp_to_c(nv,_tmp5AA);Cyc_Toc_exp_to_c(nv,_tmp5A9);{
struct _RegionHandle _tmp56B=_new_region("temp");struct _RegionHandle*temp=& _tmp56B;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA15=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA15,_tmp5A8);});
# 3556
_npop_handler(0U);return;
# 3553
;_pop_region(temp);};case 10U: _LL15: _tmp5AE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_tmp5AD=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp563)->f2;_tmp5AC=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp563)->f3;_LL16:
# 3558
 Cyc_Toc_xlate_switch(nv,s,_tmp5AE,_tmp5AD,_tmp5AC);
return;case 11U: _LL17: _tmp5B0=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_tmp5AF=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp563)->f2;_LL18: {
# 3561
struct Cyc_Toc_Env*_tmp56C=nv;struct Cyc_Toc_FallthruInfo*_tmp576;_LL28: _tmp576=_tmp56C->fallthru_info;_LL29:;
if(_tmp576 == 0)
({void*_tmp56D=0U;({struct _dyneither_ptr _tmpA16=({const char*_tmp56E="fallthru in unexpected place";_tag_dyneither(_tmp56E,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA16,_tag_dyneither(_tmp56D,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp56F=*_tmp576;struct Cyc_Toc_FallthruInfo _tmp570=_tmp56F;struct _dyneither_ptr*_tmp575;struct Cyc_List_List*_tmp574;_LL2B: _tmp575=_tmp570.label;_tmp574=_tmp570.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp575,0U);
# 3567
({int _tmpA17=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA17,s2);});{
struct Cyc_List_List*_tmp571=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp574);
struct Cyc_List_List*_tmp572=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5B0);
for(0;_tmp571 != 0;(_tmp571=_tmp571->tl,_tmp572=_tmp572->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp572))->hd);
({struct Cyc_Absyn_Stmt*_tmpA1A=({struct Cyc_Absyn_Stmt*_tmpA19=({struct Cyc_Absyn_Exp*_tmpA18=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=5U,_tmp573->f1=(struct Cyc_Absyn_Vardecl*)_tmp571->hd;_tmp573;}),0U);Cyc_Absyn_assign_stmt(_tmpA18,(struct Cyc_Absyn_Exp*)_tmp572->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA19,s2,0U);});s2=_tmpA1A;});}
# 3575
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp5B2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_tmp5B1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp563)->f2;_LL1A:
# 3581
{void*_tmp577=_tmp5B2->r;void*_tmp578=_tmp577;struct Cyc_Absyn_Tvar*_tmp593;struct Cyc_Absyn_Vardecl*_tmp592;struct Cyc_Absyn_Exp*_tmp591;struct Cyc_Absyn_Fndecl*_tmp590;struct Cyc_List_List*_tmp58F;struct Cyc_Absyn_Pat*_tmp58E;struct Cyc_Absyn_Exp*_tmp58D;void*_tmp58C;struct Cyc_Absyn_Vardecl*_tmp58B;switch(*((int*)_tmp578)){case 0U: _LL2E: _tmp58B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp578)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp58B,_tmp5B1);goto _LL2D;case 2U: _LL30: _tmp58E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp578)->f1;_tmp58D=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp578)->f3;_tmp58C=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp578)->f4;_LL31:
# 3589
{void*_tmp579=_tmp58E->r;void*_tmp57A=_tmp579;struct Cyc_Absyn_Vardecl*_tmp57C;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp57A)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp57A)->f2)->r)->tag == 0U){_LL3B: _tmp57C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp57A)->f1;_LL3C:
# 3591
({struct _tuple1*_tmpA1B=Cyc_Toc_temp_var();_tmp57C->name=_tmpA1B;});
_tmp57C->initializer=_tmp58D;
({void*_tmpA1C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->tag=0U,_tmp57B->f1=_tmp57C;_tmp57B;});_tmp5B2->r=_tmpA1C;});
Cyc_Toc_local_decl_to_c(nv,_tmp57C,_tmp5B1);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3600
({void*_tmpA1D=(Cyc_Toc_letdecl_to_c(nv,_tmp58E,_tmp58C,(void*)_check_null(_tmp58D->topt),_tmp58D,_tmp5B1))->r;s->r=_tmpA1D;});
goto _LL3A;}_LL3A:;}
# 3603
goto _LL2D;case 3U: _LL32: _tmp58F=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp578)->f1;_LL33: {
# 3607
struct Cyc_List_List*_tmp57D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp58F);
if(_tmp57D == 0)
({void*_tmp57E=0U;({struct _dyneither_ptr _tmpA1E=({const char*_tmp57F="empty Letv_d";_tag_dyneither(_tmp57F,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA1E,_tag_dyneither(_tmp57E,sizeof(void*),0U));});});
({void*_tmpA1F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->tag=0U,_tmp580->f1=(struct Cyc_Absyn_Vardecl*)_tmp57D->hd;_tmp580;});_tmp5B2->r=_tmpA1F;});
_tmp57D=_tmp57D->tl;
for(0;_tmp57D != 0;_tmp57D=_tmp57D->tl){
struct Cyc_Absyn_Decl*_tmp581=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582->tag=0U,_tmp582->f1=(struct Cyc_Absyn_Vardecl*)_tmp57D->hd;_tmp582;}),0U);
({void*_tmpA21=({struct Cyc_Absyn_Decl*_tmpA20=_tmp581;Cyc_Absyn_decl_stmt(_tmpA20,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA21;});}
# 3616
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp590=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp578)->f1;_LL35:
# 3619
 Cyc_Toc_fndecl_to_c(nv,_tmp590,0);
Cyc_Toc_stmt_to_c(nv,_tmp5B1);
goto _LL2D;case 4U: _LL36: _tmp593=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp578)->f1;_tmp592=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp578)->f2;_tmp591=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp578)->f3;_LL37: {
# 3623
struct Cyc_Absyn_Stmt*_tmp583=_tmp5B1;
# 3625
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp592->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3632
Cyc_Toc_stmt_to_c(nv,_tmp583);
if(Cyc_Absyn_no_regions)
({void*_tmpA25=({struct _tuple1*_tmpA24=x_var;void*_tmpA23=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA22=
Cyc_Absyn_uint_exp(0U,0U);
# 3634
Cyc_Absyn_declare_stmt(_tmpA24,_tmpA23,_tmpA22,_tmp583,0U);})->r;s->r=_tmpA25;});else{
# 3636
if((unsigned int)_tmp591){
# 3638
Cyc_Toc_exp_to_c(nv,_tmp591);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA27=_tmp591;Cyc_Absyn_aggrarrow_exp(_tmpA27,({struct _dyneither_ptr*_tmp585=_cycalloc(sizeof(*_tmp585));({struct _dyneither_ptr _tmpA26=({const char*_tmp584="h";_tag_dyneither(_tmp584,sizeof(char),2U);});*_tmp585=_tmpA26;});_tmp585;}),0U);}),0U);
({void*_tmpA28=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp583,0U))->r;s->r=_tmpA28;});};}else{
# 3648
({void*_tmpA35=({
struct _tuple1*_tmpA34=rh_var;void*_tmpA33=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA32=({
struct Cyc_Absyn_Exp*_tmpA2A=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpA2A,({struct Cyc_List_List*_tmp586=_cycalloc(sizeof(*_tmp586));
({struct Cyc_Absyn_Exp*_tmpA29=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp586->hd=_tmpA29;}),_tmp586->tl=0;_tmp586;}),0U);});
# 3649
Cyc_Absyn_declare_stmt(_tmpA34,_tmpA33,_tmpA32,({
# 3653
struct _tuple1*_tmpA31=x_var;void*_tmpA30=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA2F=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA31,_tmpA30,_tmpA2F,({
struct Cyc_Absyn_Stmt*_tmpA2E=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA2B=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpA2B,({struct Cyc_Absyn_Exp*_tmp587[1U];_tmp587[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp587,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpA2E,({
struct Cyc_Absyn_Stmt*_tmpA2D=_tmp583;Cyc_Absyn_seq_stmt(_tmpA2D,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA2C=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpA2C,({struct Cyc_Absyn_Exp*_tmp588[1U];_tmp588[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp588,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3648
s->r=_tmpA35;});}}
# 3659
return;}default: _LL38: _LL39:
({void*_tmp589=0U;({struct _dyneither_ptr _tmpA36=({const char*_tmp58A="bad nested declaration within function";_tag_dyneither(_tmp58A,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA36,_tag_dyneither(_tmp589,sizeof(void*),0U));});});}_LL2D:;}
# 3662
return;case 13U: _LL1B: _tmp5B4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_tmp5B3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp563)->f2;_LL1C:
 s=_tmp5B3;continue;case 14U: _LL1D: _tmp5B6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_tmp5B5=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp563)->f2).f1;_LL1E: {
# 3665
struct _RegionHandle _tmp594=_new_region("temp");struct _RegionHandle*temp=& _tmp594;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA37=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA37,_tmp5B6);});
Cyc_Toc_exp_to_c(nv,_tmp5B5);
# 3669
_npop_handler(0U);return;
# 3665
;_pop_region(temp);}default: _LL1F: _tmp5B9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp563)->f1;_tmp5B8=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp563)->f2;_tmp5B7=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp563)->f3;_LL20: {
# 3683 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3693
e_exp->topt=e_typ;{
struct _RegionHandle _tmp595=_new_region("temp");struct _RegionHandle*temp=& _tmp595;_push_region(temp);
# 3696
Cyc_Toc_stmt_to_c(nv,_tmp5B9);{
struct Cyc_Absyn_Stmt*_tmp596=({struct Cyc_Absyn_Stmt*_tmpA38=_tmp5B9;Cyc_Absyn_seq_stmt(_tmpA38,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 3701
struct Cyc_Absyn_Stmt*_tmp597=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E->tag=10U,_tmp59E->f1=e_exp,_tmp59E->f2=_tmp5B8,_tmp59E->f3=_tmp5B7;_tmp59E;}),0U);
# 3703
Cyc_Toc_stmt_to_c(nv,_tmp597);{
# 3706
struct Cyc_Absyn_Exp*_tmp598=({
struct Cyc_Absyn_Exp*_tmpA3A=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpA3A,({struct Cyc_List_List*_tmp59D=_cycalloc(sizeof(*_tmp59D));
({struct Cyc_Absyn_Exp*_tmpA39=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp59D->hd=_tmpA39;}),_tmp59D->tl=0;_tmp59D;}),0U);});
# 3710
struct Cyc_Absyn_Stmt*_tmp599=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA3C=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpA3C,({struct Cyc_List_List*_tmp59C=_cycalloc(sizeof(*_tmp59C));
({struct Cyc_Absyn_Exp*_tmpA3B=Cyc_Absyn_address_exp(h_exp,0U);_tmp59C->hd=_tmpA3B;}),_tmp59C->tl=0;_tmp59C;}),0U);}),0U);
# 3714
struct Cyc_Absyn_Exp*_tmp59A=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp59B=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpA4B=({
struct _tuple1*_tmpA4A=h_var;void*_tmpA49=h_typ;Cyc_Absyn_declare_stmt(_tmpA4A,_tmpA49,0,({
struct Cyc_Absyn_Stmt*_tmpA48=_tmp599;Cyc_Absyn_seq_stmt(_tmpA48,({
struct _tuple1*_tmpA47=was_thrown_var;void*_tmpA46=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpA45=_tmp59A;Cyc_Absyn_declare_stmt(_tmpA47,_tmpA46,_tmpA45,({
struct Cyc_Absyn_Stmt*_tmpA44=({struct Cyc_Absyn_Exp*_tmpA3E=_tmp598;struct Cyc_Absyn_Stmt*_tmpA3D=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp59B,0U);
# 3720
Cyc_Absyn_ifthenelse_stmt(_tmpA3E,_tmpA3D,
# 3722
Cyc_Toc_skip_stmt_dl(),0U);});
# 3720
Cyc_Absyn_seq_stmt(_tmpA44,({
# 3723
struct Cyc_Absyn_Exp*_tmpA43=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpA42=_tmp596;Cyc_Absyn_ifthenelse_stmt(_tmpA43,_tmpA42,({
# 3725
struct _tuple1*_tmpA41=e_var;void*_tmpA40=e_typ;struct Cyc_Absyn_Exp*_tmpA3F=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 3725
Cyc_Absyn_declare_stmt(_tmpA41,_tmpA40,_tmpA3F,_tmp597,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3716
s->r=_tmpA4B;});};};
# 3729
_npop_handler(0U);return;
# 3694
;_pop_region(temp);};}}_LL0:;}}
# 3738
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpA4C=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpA4C;});{
struct _RegionHandle _tmp5BA=_new_region("frgn");struct _RegionHandle*frgn=& _tmp5BA;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp5BB=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp5BC=f->args;for(0;_tmp5BC != 0;_tmp5BC=_tmp5BC->tl){
struct _tuple1*_tmp5BD=({struct _tuple1*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->f1=Cyc_Absyn_Loc_n,_tmp5BE->f2=(*((struct _tuple9*)_tmp5BC->hd)).f1;_tmp5BE;});
({void*_tmpA4D=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp5BC->hd)).f3);(*((struct _tuple9*)_tmp5BC->hd)).f3=_tmpA4D;});}}
# 3754
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3758
({struct Cyc_Hashtable_Table**_tmpA4F=({struct Cyc_Hashtable_Table**_tmp5BF=_cycalloc(sizeof(*_tmp5BF));({struct Cyc_Hashtable_Table*_tmpA4E=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp5BF=_tmpA4E;});_tmp5BF;});Cyc_Toc_fn_pop_table=_tmpA4F;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp5C0=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp5C1=_tmp5C0;struct _dyneither_ptr*_tmp5CA;struct Cyc_Absyn_Tqual _tmp5C9;void*_tmp5C8;int _tmp5C7;_LL1: _tmp5CA=_tmp5C1.name;_tmp5C9=_tmp5C1.tq;_tmp5C8=_tmp5C1.type;_tmp5C7=_tmp5C1.inject;_LL2:;{
void*_tmp5C2=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp5C8,Cyc_Absyn_heap_rgn_type,_tmp5C9,Cyc_Absyn_false_type));
struct _tuple1*_tmp5C3=({struct _tuple1*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6->f1=Cyc_Absyn_Loc_n,_tmp5C6->f2=(struct _dyneither_ptr*)_check_null(_tmp5CA);_tmp5C6;});
({struct Cyc_List_List*_tmpA52=({struct Cyc_List_List*_tmpA51=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA51,({struct Cyc_List_List*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));({struct _tuple9*_tmpA50=({struct _tuple9*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4->f1=_tmp5CA,_tmp5C4->f2=_tmp5C9,_tmp5C4->f3=_tmp5C2;_tmp5C4;});_tmp5C5->hd=_tmpA50;}),_tmp5C5->tl=0;_tmp5C5;}));});f->args=_tmpA52;});
f->cyc_varargs=0;};}
# 3767
{struct Cyc_List_List*_tmp5CB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp5CB != 0;_tmp5CB=_tmp5CB->tl){
({void*_tmpA53=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp5CB->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp5CB->hd)->type=_tmpA53;});}}
# 3770
({struct Cyc_Toc_Env*_tmpA54=Cyc_Toc_clear_toplevel(frgn,_tmp5BB);Cyc_Toc_stmt_to_c(_tmpA54,f->body);});}
# 3747
;_pop_region(frgn);};}
# 3774
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp5CC=s;switch(_tmp5CC){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3791 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 3793
struct _tuple38 _tmp5CD=*env;struct _tuple38 _tmp5CE=_tmp5CD;struct Cyc_Toc_TocState*_tmp602;struct Cyc_Absyn_Aggrdecl*_tmp601;_LL1: _tmp602=_tmp5CE.f1;_tmp601=*_tmp5CE.f2;_LL2:;{
struct _tuple1*_tmp5CF=_tmp601->name;
struct Cyc_Toc_TocState _tmp5D0=*_tmp602;struct Cyc_Toc_TocState _tmp5D1=_tmp5D0;struct Cyc_Dict_Dict*_tmp600;_LL4: _tmp600=_tmp5D1.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple18**_tmp5D2=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp600,_tmp5CF);
if(_tmp5D2 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp601->kind == Cyc_Absyn_StructA)
({struct _tuple18*_tmpA56=({struct _tuple18*_tmp5D3=_region_malloc(d,sizeof(*_tmp5D3));_tmp5D3->f1=_tmp601,({void*_tmpA55=Cyc_Absyn_strctq(_tmp5CF);_tmp5D3->f2=_tmpA55;});_tmp5D3;});v=_tmpA56;});else{
# 3804
({struct _tuple18*_tmpA58=({struct _tuple18*_tmp5D4=_region_malloc(d,sizeof(*_tmp5D4));_tmp5D4->f1=_tmp601,({void*_tmpA57=Cyc_Absyn_unionq_type(_tmp5CF);_tmp5D4->f2=_tmpA57;});_tmp5D4;});v=_tmpA58;});}
({struct Cyc_Dict_Dict _tmpA59=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp600,_tmp5CF,v);*_tmp600=_tmpA59;});};}else{
# 3807
struct _tuple18*_tmp5D5=*_tmp5D2;struct _tuple18*_tmp5D6=_tmp5D5;struct Cyc_Absyn_Aggrdecl*_tmp5D9;void*_tmp5D8;_LL7: _tmp5D9=_tmp5D6->f1;_tmp5D8=_tmp5D6->f2;_LL8:;
if(_tmp5D9->impl == 0){
({struct Cyc_Dict_Dict _tmpA5C=({struct Cyc_Dict_Dict _tmpA5B=*_tmp600;struct _tuple1*_tmpA5A=_tmp5CF;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpA5B,_tmpA5A,({struct _tuple18*_tmp5D7=_region_malloc(d,sizeof(*_tmp5D7));_tmp5D7->f1=_tmp601,_tmp5D7->f2=_tmp5D8;_tmp5D7;}));});*_tmp600=_tmpA5C;});
seen_defn_before=0;}else{
# 3812
seen_defn_before=1;}}{
# 3814
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->kind=_tmp601->kind,_tmp5FF->sc=Cyc_Absyn_Public,_tmp5FF->name=_tmp601->name,_tmp5FF->tvs=0,_tmp5FF->impl=0,_tmp5FF->expected_mem_kind=0,_tmp5FF->attributes=_tmp601->attributes;_tmp5FF;});
# 3821
if(_tmp601->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpA5D=({struct Cyc_Absyn_AggrdeclImpl*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->exist_vars=0,_tmp5DA->rgn_po=0,_tmp5DA->fields=0,_tmp5DA->tagged=0;_tmp5DA;});new_ad->impl=_tmpA5D;});{
# 3826
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp5DB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp601->impl))->fields;for(0;_tmp5DB != 0;_tmp5DB=_tmp5DB->tl){
# 3830
struct Cyc_Absyn_Aggrfield*_tmp5DC=(struct Cyc_Absyn_Aggrfield*)_tmp5DB->hd;
void*_tmp5DD=_tmp5DC->type;
struct Cyc_List_List*_tmp5DE=_tmp5DC->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp5DD)) && (
_tmp601->kind == Cyc_Absyn_StructA  && _tmp5DB->tl == 0  || _tmp601->kind == Cyc_Absyn_UnionA)){
# 3844 "toc.cyc"
void*_tmp5DF=Cyc_Tcutil_compress(_tmp5DD);void*_tmp5E0=_tmp5DF;void*_tmp5E8;struct Cyc_Absyn_Tqual _tmp5E7;void*_tmp5E6;unsigned int _tmp5E5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->tag == 4U){_LLA: _tmp5E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->f1).elt_type;_tmp5E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->f1).tq;_tmp5E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->f1).zero_term;_tmp5E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->f1).zt_loc;_LLB:
# 3847
({void*_tmpA5F=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->tag=4U,(_tmp5E1->f1).elt_type=_tmp5E8,(_tmp5E1->f1).tq=_tmp5E7,({struct Cyc_Absyn_Exp*_tmpA5E=Cyc_Absyn_uint_exp(0U,0U);(_tmp5E1->f1).num_elts=_tmpA5E;}),(_tmp5E1->f1).zero_term=_tmp5E6,(_tmp5E1->f1).zt_loc=_tmp5E5;_tmp5E1;});_tmp5DD=_tmpA5F;});
goto _LL9;}else{_LLC: _LLD:
# 3850
({struct Cyc_List_List*_tmpA61=({struct Cyc_List_List*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));({void*_tmpA60=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->tag=6U,_tmp5E2->f1=0;_tmp5E2;});_tmp5E3->hd=_tmpA60;}),_tmp5E3->tl=_tmp5DE;_tmp5E3;});_tmp5DE=_tmpA61;});
({void*_tmpA65=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->tag=4U,({void*_tmpA64=Cyc_Absyn_void_star_type();(_tmp5E4->f1).elt_type=_tmpA64;}),({
struct Cyc_Absyn_Tqual _tmpA63=Cyc_Absyn_empty_tqual(0U);(_tmp5E4->f1).tq=_tmpA63;}),({
struct Cyc_Absyn_Exp*_tmpA62=Cyc_Absyn_uint_exp(0U,0U);(_tmp5E4->f1).num_elts=_tmpA62;}),(_tmp5E4->f1).zero_term=Cyc_Absyn_false_type,(_tmp5E4->f1).zt_loc=0U;_tmp5E4;});
# 3851
_tmp5DD=_tmpA65;});}_LL9:;}{
# 3857
struct Cyc_Absyn_Aggrfield*_tmp5E9=({struct Cyc_Absyn_Aggrfield*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->name=_tmp5DC->name,_tmp5FB->tq=Cyc_Toc_mt_tq,({
# 3859
void*_tmpA66=Cyc_Toc_typ_to_c(_tmp5DD);_tmp5FB->type=_tmpA66;}),_tmp5FB->width=_tmp5DC->width,_tmp5FB->attributes=_tmp5DE,_tmp5FB->requires_clause=0;_tmp5FB;});
# 3867
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp601->impl))->tagged){
void*_tmp5EA=_tmp5E9->type;
struct _dyneither_ptr*_tmp5EB=_tmp5E9->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5F8=({struct Cyc_String_pa_PrintArg_struct _tmp71F;_tmp71F.tag=0U,_tmp71F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp601->name).f2);_tmp71F;});struct Cyc_String_pa_PrintArg_struct _tmp5F9=({struct Cyc_String_pa_PrintArg_struct _tmp71E;_tmp71E.tag=0U,_tmp71E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5EB);_tmp71E;});void*_tmp5F6[2U];_tmp5F6[0]=& _tmp5F8,_tmp5F6[1]=& _tmp5F9;({struct _dyneither_ptr _tmpA67=({const char*_tmp5F7="_union_%s_%s";_tag_dyneither(_tmp5F7,sizeof(char),13U);});Cyc_aprintf(_tmpA67,_tag_dyneither(_tmp5F6,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));*_tmp5F5=s;_tmp5F5;});
struct Cyc_Absyn_Aggrfield*_tmp5EC=({struct Cyc_Absyn_Aggrfield*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->name=Cyc_Toc_val_sp,_tmp5F4->tq=Cyc_Toc_mt_tq,_tmp5F4->type=_tmp5EA,_tmp5F4->width=0,_tmp5F4->attributes=0,_tmp5F4->requires_clause=0;_tmp5F4;});
struct Cyc_Absyn_Aggrfield*_tmp5ED=({struct Cyc_Absyn_Aggrfield*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->name=Cyc_Toc_tag_sp,_tmp5F3->tq=Cyc_Toc_mt_tq,_tmp5F3->type=Cyc_Absyn_sint_type,_tmp5F3->width=0,_tmp5F3->attributes=0,_tmp5F3->requires_clause=0;_tmp5F3;});
struct Cyc_List_List*_tmp5EE=({struct Cyc_Absyn_Aggrfield*_tmp5F2[2U];_tmp5F2[0]=_tmp5ED,_tmp5F2[1]=_tmp5EC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5F2,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp5EF=Cyc_Toc_make_c_struct_defn(str,_tmp5EE);
({struct Cyc_List_List*_tmpA69=({struct Cyc_List_List*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));({struct Cyc_Absyn_Decl*_tmpA68=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->tag=5U,_tmp5F0->f1=_tmp5EF;_tmp5F0;}),0U);_tmp5F1->hd=_tmpA68;}),_tmp5F1->tl=Cyc_Toc_result_decls;_tmp5F1;});Cyc_Toc_result_decls=_tmpA69;});
({void*_tmpA6A=Cyc_Absyn_strct(str);_tmp5E9->type=_tmpA6A;});}
# 3879
({struct Cyc_List_List*_tmpA6B=({struct Cyc_List_List*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->hd=_tmp5E9,_tmp5FA->tl=new_fields;_tmp5FA;});new_fields=_tmpA6B;});};}}
# 3881
({struct Cyc_List_List*_tmpA6C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpA6C;});};}
# 3884
if(!seen_defn_before)
({struct Cyc_List_List*_tmpA6F=({struct Cyc_List_List*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));({struct Cyc_Absyn_Decl*_tmpA6E=({struct Cyc_Absyn_Decl*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));({void*_tmpA6D=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->tag=5U,_tmp5FC->f1=new_ad;_tmp5FC;});_tmp5FD->r=_tmpA6D;}),_tmp5FD->loc=0U;_tmp5FD;});_tmp5FE->hd=_tmpA6E;}),_tmp5FE->tl=Cyc_Toc_result_decls;_tmp5FE;});Cyc_Toc_result_decls=_tmpA6F;});
return 0;};};};}
# 3888
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple39{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3916 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 3919
struct _tuple39 _tmp603=*env;struct _tuple39 _tmp604=_tmp603;struct Cyc_Set_Set**_tmp615;struct Cyc_Absyn_Datatypedecl*_tmp614;_LL1: _tmp615=(_tmp604.f1)->datatypes_so_far;_tmp614=_tmp604.f2;_LL2:;{
struct _tuple1*_tmp605=_tmp614->name;
if(_tmp614->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp615,_tmp605))
return 0;
({struct Cyc_Set_Set*_tmpA70=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp615,_tmp605);*_tmp615=_tmpA70;});
{struct Cyc_List_List*_tmp606=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp614->fields))->v;for(0;_tmp606 != 0;_tmp606=_tmp606->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp606->hd;
# 3927
struct Cyc_List_List*_tmp607=0;
int i=1;
{struct Cyc_List_List*_tmp608=f->typs;for(0;_tmp608 != 0;(_tmp608=_tmp608->tl,i ++)){
struct _dyneither_ptr*_tmp609=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp60A=({struct Cyc_Absyn_Aggrfield*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->name=_tmp609,_tmp60C->tq=(*((struct _tuple12*)_tmp608->hd)).f1,({
void*_tmpA71=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp608->hd)).f2);_tmp60C->type=_tmpA71;}),_tmp60C->width=0,_tmp60C->attributes=0,_tmp60C->requires_clause=0;_tmp60C;});
({struct Cyc_List_List*_tmpA72=({struct Cyc_List_List*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->hd=_tmp60A,_tmp60B->tl=_tmp607;_tmp60B;});_tmp607=_tmpA72;});}}
# 3935
({struct Cyc_List_List*_tmpA75=({struct Cyc_List_List*_tmp60E=_cycalloc(sizeof(*_tmp60E));({struct Cyc_Absyn_Aggrfield*_tmpA74=({struct Cyc_Absyn_Aggrfield*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->name=Cyc_Toc_tag_sp,_tmp60D->tq=Cyc_Toc_mt_tq,_tmp60D->type=Cyc_Absyn_sint_type,_tmp60D->width=0,_tmp60D->attributes=0,_tmp60D->requires_clause=0;_tmp60D;});_tmp60E->hd=_tmpA74;}),({
struct Cyc_List_List*_tmpA73=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp607);_tmp60E->tl=_tmpA73;});_tmp60E;});
# 3935
_tmp607=_tmpA75;});{
# 3937
struct Cyc_Absyn_Aggrdecl*_tmp60F=({struct _dyneither_ptr*_tmpA77=({struct _dyneither_ptr*_tmp613=_cycalloc(sizeof(*_tmp613));({struct _dyneither_ptr _tmpA76=({const char*_tmp612="";_tag_dyneither(_tmp612,sizeof(char),1U);});*_tmp613=_tmpA76;});_tmp613;});Cyc_Toc_make_c_struct_defn(_tmpA77,_tmp607);});
({struct _tuple1*_tmpA78=Cyc_Toc_collapse_qvars(f->name,_tmp614->name);_tmp60F->name=_tmpA78;});
({struct Cyc_List_List*_tmpA7A=({struct Cyc_List_List*_tmp611=_cycalloc(sizeof(*_tmp611));({struct Cyc_Absyn_Decl*_tmpA79=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=5U,_tmp610->f1=_tmp60F;_tmp610;}),0U);_tmp611->hd=_tmpA79;}),_tmp611->tl=Cyc_Toc_result_decls;_tmp611;});Cyc_Toc_result_decls=_tmpA7A;});};}}
# 3941
return 0;};}
# 3944
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3963 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 3966
struct _tuple39 _tmp616=*env;struct _tuple39 _tmp617=_tmp616;struct Cyc_Toc_TocState*_tmp639;struct Cyc_Absyn_Datatypedecl*_tmp638;_LL1: _tmp639=_tmp617.f1;_tmp638=_tmp617.f2;_LL2:;
if(_tmp638->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp618=*_tmp639;struct Cyc_Toc_TocState _tmp619=_tmp618;struct Cyc_Dict_Dict*_tmp637;_LL4: _tmp637=_tmp619.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp61A=_tmp638->name;
{struct Cyc_List_List*_tmp61B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp638->fields))->v;for(0;_tmp61B != 0;_tmp61B=_tmp61B->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp61B->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp61C=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp61D=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp61C,Cyc_Absyn_false_type,0U);
# 3977
int*_tmp61E=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp637,f->name);int*_tmp61F=_tmp61E;if(_tmp61F == 0){_LL7: _LL8: {
# 3979
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpA7B=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpA7B;});{
# 3983
struct Cyc_Absyn_Vardecl*_tmp620=Cyc_Absyn_new_vardecl(0U,f->name,_tmp61D,initopt);
_tmp620->sc=f->sc;
({struct Cyc_List_List*_tmpA7D=({struct Cyc_List_List*_tmp622=_cycalloc(sizeof(*_tmp622));({struct Cyc_Absyn_Decl*_tmpA7C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->tag=0U,_tmp621->f1=_tmp620;_tmp621;}),0U);_tmp622->hd=_tmpA7C;}),_tmp622->tl=Cyc_Toc_result_decls;_tmp622;});Cyc_Toc_result_decls=_tmpA7D;});
({struct Cyc_Dict_Dict _tmpA7E=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp637,f->name,f->sc != Cyc_Absyn_Extern);
# 3986
*_tmp637=_tmpA7E;});{
# 3988
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp623=f->typs;for(0;_tmp623 != 0;(_tmp623=_tmp623->tl,i ++)){
struct _dyneither_ptr*_tmp624=({struct _dyneither_ptr*_tmp62B=_cycalloc(sizeof(*_tmp62B));({struct _dyneither_ptr _tmpA80=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp62A=({struct Cyc_Int_pa_PrintArg_struct _tmp720;_tmp720.tag=1U,_tmp720.f1=(unsigned long)i;_tmp720;});void*_tmp628[1U];_tmp628[0]=& _tmp62A;({struct _dyneither_ptr _tmpA7F=({const char*_tmp629="f%d";_tag_dyneither(_tmp629,sizeof(char),4U);});Cyc_aprintf(_tmpA7F,_tag_dyneither(_tmp628,sizeof(void*),1U));});});*_tmp62B=_tmpA80;});_tmp62B;});
struct Cyc_Absyn_Aggrfield*_tmp625=({struct Cyc_Absyn_Aggrfield*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->name=_tmp624,_tmp627->tq=(*((struct _tuple12*)_tmp623->hd)).f1,({
void*_tmpA81=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp623->hd)).f2);_tmp627->type=_tmpA81;}),_tmp627->width=0,_tmp627->attributes=0,_tmp627->requires_clause=0;_tmp627;});
({struct Cyc_List_List*_tmpA82=({struct Cyc_List_List*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->hd=_tmp625,_tmp626->tl=fields;_tmp626;});fields=_tmpA82;});}}
# 3996
({struct Cyc_List_List*_tmpA86=({struct Cyc_List_List*_tmp62D=_cycalloc(sizeof(*_tmp62D));({struct Cyc_Absyn_Aggrfield*_tmpA85=({struct Cyc_Absyn_Aggrfield*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->name=Cyc_Toc_tag_sp,_tmp62C->tq=Cyc_Toc_mt_tq,({
void*_tmpA84=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp62C->type=_tmpA84;}),_tmp62C->width=0,_tmp62C->attributes=0,_tmp62C->requires_clause=0;_tmp62C;});
# 3996
_tmp62D->hd=_tmpA85;}),({
# 3998
struct Cyc_List_List*_tmpA83=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp62D->tl=_tmpA83;});_tmp62D;});
# 3996
fields=_tmpA86;});{
# 3999
struct Cyc_Absyn_Aggrdecl*_tmp62E=({struct _dyneither_ptr*_tmpA88=({struct _dyneither_ptr*_tmp632=_cycalloc(sizeof(*_tmp632));({struct _dyneither_ptr _tmpA87=({const char*_tmp631="";_tag_dyneither(_tmp631,sizeof(char),1U);});*_tmp632=_tmpA87;});_tmp632;});Cyc_Toc_make_c_struct_defn(_tmpA88,fields);});
({struct _tuple1*_tmpA89=Cyc_Toc_collapse_qvars(f->name,_tmp638->name);_tmp62E->name=_tmpA89;});
({struct Cyc_List_List*_tmpA8B=({struct Cyc_List_List*_tmp630=_cycalloc(sizeof(*_tmp630));({struct Cyc_Absyn_Decl*_tmpA8A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->tag=5U,_tmp62F->f1=_tmp62E;_tmp62F;}),0U);_tmp630->hd=_tmpA8A;}),_tmp630->tl=Cyc_Toc_result_decls;_tmp630;});Cyc_Toc_result_decls=_tmpA8B;});
goto _LL6;};};};}}else{if(*((int*)_tmp61F)== 0){_LL9: _LLA:
# 4004
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp633=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp634=Cyc_Absyn_new_vardecl(0U,f->name,_tmp61D,_tmp633);
_tmp634->sc=f->sc;
({struct Cyc_List_List*_tmpA8D=({struct Cyc_List_List*_tmp636=_cycalloc(sizeof(*_tmp636));({struct Cyc_Absyn_Decl*_tmpA8C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->tag=0U,_tmp635->f1=_tmp634;_tmp635;}),0U);_tmp636->hd=_tmpA8C;}),_tmp636->tl=Cyc_Toc_result_decls;_tmp636;});Cyc_Toc_result_decls=_tmpA8D;});
({struct Cyc_Dict_Dict _tmpA8E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp637,f->name,1);*_tmp637=_tmpA8E;});}
# 4011
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4015
return 0;};};}
# 4018
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4022
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4028
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpA8F=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpA8F;});
# 4032
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4041
struct _RegionHandle _tmp63A=_new_region("temp");struct _RegionHandle*temp=& _tmp63A;_push_region(temp);
{struct Cyc_Toc_Env*_tmp63B=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp63B,init);}
# 4042
;_pop_region(temp);}else{
# 4046
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4049
void*_tmp63C=Cyc_Tcutil_compress(old_typ);void*_tmp63D=_tmp63C;void*_tmp644;struct Cyc_Absyn_Exp*_tmp643;void*_tmp642;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp63D)->tag == 4U){_LL1: _tmp644=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp63D)->f1).elt_type;_tmp643=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp63D)->f1).num_elts;_tmp642=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp63D)->f1).zero_term;_LL2:
# 4051
 if(Cyc_Tcutil_force_type2bool(0,_tmp642)){
if(_tmp643 == 0)
({void*_tmp63E=0U;({struct _dyneither_ptr _tmpA90=({const char*_tmp63F="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp63F,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA90,_tag_dyneither(_tmp63E,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp643;
struct Cyc_Absyn_Exp*_tmp640=({struct Cyc_Absyn_Exp*_tmpA92=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpA92,({
struct Cyc_Absyn_Exp*_tmpA91=num_elts;Cyc_Absyn_add_exp(_tmpA91,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4058
struct Cyc_Absyn_Exp*_tmp641=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpA94=({struct Cyc_Absyn_Stmt*_tmpA93=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp640,_tmp641,0U),0U);Cyc_Toc_seq_stmt_r(_tmpA93,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4059
s->r=_tmpA94;});};}
# 4062
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4070
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp645=d;struct Cyc_List_List*_tmp64C;struct Cyc_List_List*_tmp64B;void**_tmp64A;struct Cyc_Tcpat_Rhs*_tmp649;switch(*((int*)_tmp645)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp649=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp645)->f1;_LL4:
 _tmp649->rhs=success;return d;default: _LL5: _tmp64C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp645)->f1;_tmp64B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp645)->f2;_tmp64A=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp645)->f3;_LL6:
# 4075
({void*_tmpA95=Cyc_Toc_rewrite_decision(*_tmp64A,success);*_tmp64A=_tmpA95;});
for(0;_tmp64B != 0;_tmp64B=_tmp64B->tl){
struct _tuple31*_tmp646=(struct _tuple31*)_tmp64B->hd;struct _tuple31*_tmp647=_tmp646;void**_tmp648;_LL8: _tmp648=(void**)& _tmp647->f2;_LL9:;
({void*_tmpA96=Cyc_Toc_rewrite_decision(*_tmp648,success);*_tmp648=_tmpA96;});}
# 4080
return d;}_LL0:;}
# 4091 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4093
struct _RegionHandle _tmp64D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp64D;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp64E=Cyc_Toc_share_env(rgn,nv);
void*_tmp64F=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp64E,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4103
struct Cyc_Absyn_Stmt*_tmp650=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4107
({void*_tmpA97=Cyc_Toc_rewrite_decision(dopt,_tmp650);dopt=_tmpA97;});{
# 4109
struct Cyc_Absyn_Switch_clause*_tmp651=({struct Cyc_Absyn_Switch_clause*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->pattern=p,_tmp65D->pat_vars=0,_tmp65D->where_clause=0,_tmp65D->body=_tmp650,_tmp65D->loc=0U;_tmp65D;});
struct Cyc_List_List*_tmp652=({struct _RegionHandle*_tmpA9A=rgn;struct _RegionHandle*_tmpA99=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple34*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpA9A,Cyc_Toc_gen_labels,_tmpA99,({struct Cyc_Absyn_Switch_clause*_tmp65C[1U];_tmp65C[0]=_tmp651;({struct _RegionHandle*_tmpA98=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpA98,_tag_dyneither(_tmp65C,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4112
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4116
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp64E,& mydecls,& mybodies,dopt,_tmp652,v);
# 4119
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple37*_tmp653=(struct _tuple37*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple37*_tmp654=_tmp653;struct Cyc_Toc_Env*_tmp656;struct Cyc_Absyn_Stmt*_tmp655;_LL1: _tmp656=_tmp654->f1;_tmp655=_tmp654->f3;_LL2:;
if(_tmp655 == _tmp650){senv=_tmp656;goto FOUND_ENV;}}
# 4124
({void*_tmp657=0U;({struct _dyneither_ptr _tmpA9B=({const char*_tmp658="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp658,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA9B,_tag_dyneither(_tmp657,sizeof(void*),0U));});});
FOUND_ENV:
# 4128
 Cyc_Toc_stmt_to_c(senv,s);{
# 4130
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4132
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp659=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpA9C=Cyc_Toc_temp_var();_tmp659->name=_tmpA9C;});
({void*_tmpA9D=Cyc_Toc_typ_to_c_array(_tmp659->type);_tmp659->type=_tmpA9D;});
({struct Cyc_Absyn_Stmt*_tmpA9F=({struct Cyc_Absyn_Decl*_tmpA9E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->tag=0U,_tmp65A->f1=_tmp659;_tmp65A;}),0U);Cyc_Absyn_decl_stmt(_tmpA9E,res,0U);});res=_tmpA9F;});}
# 4139
({struct Cyc_Absyn_Stmt*_tmpAA3=({struct _tuple1*_tmpAA2=v;void*_tmpAA1=Cyc_Toc_typ_to_c(_tmp64F);struct Cyc_Absyn_Exp*_tmpAA0=e;Cyc_Absyn_declare_stmt(_tmpAA2,_tmpAA1,_tmpAA0,res,0U);});res=_tmpAA3;});{
struct Cyc_Absyn_Stmt*_tmp65B=res;_npop_handler(0U);return _tmp65B;};};};};}
# 4094
;_pop_region(rgn);}
# 4147
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp65E=e->r;void*_tmp65F=_tmp65E;struct Cyc_Absyn_MallocInfo*_tmp68A;struct Cyc_Absyn_Stmt*_tmp689;void**_tmp688;void**_tmp687;struct Cyc_List_List*_tmp686;struct Cyc_List_List*_tmp685;void**_tmp684;struct Cyc_List_List*_tmp683;void**_tmp682;struct Cyc_Absyn_Exp*_tmp681;struct Cyc_Absyn_Exp*_tmp680;struct Cyc_List_List*_tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_Absyn_Exp*_tmp679;struct Cyc_Absyn_Exp*_tmp678;struct Cyc_Absyn_Exp*_tmp677;struct Cyc_Absyn_Exp*_tmp676;struct Cyc_Absyn_Exp*_tmp675;struct Cyc_Absyn_Exp*_tmp674;struct Cyc_Absyn_Exp*_tmp673;struct Cyc_Absyn_Exp*_tmp672;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_Absyn_Exp*_tmp670;struct Cyc_List_List*_tmp66F;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_Absyn_Exp*_tmp66D;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66B;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_Absyn_Exp*_tmp669;struct Cyc_Absyn_Exp*_tmp668;struct Cyc_Absyn_Exp*_tmp667;struct Cyc_Absyn_Exp*_tmp666;switch(*((int*)_tmp65F)){case 41U: _LL1: _tmp666=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL2:
 _tmp667=_tmp666;goto _LL4;case 20U: _LL3: _tmp667=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL4:
 _tmp668=_tmp667;goto _LL6;case 21U: _LL5: _tmp668=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL6:
 _tmp669=_tmp668;goto _LL8;case 22U: _LL7: _tmp669=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL8:
 _tmp66A=_tmp669;goto _LLA;case 15U: _LL9: _tmp66A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LLA:
 _tmp66B=_tmp66A;goto _LLC;case 11U: _LLB: _tmp66B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LLC:
 _tmp66C=_tmp66B;goto _LLE;case 12U: _LLD: _tmp66C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LLE:
 _tmp66D=_tmp66C;goto _LL10;case 18U: _LLF: _tmp66D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL10:
 _tmp66E=_tmp66D;goto _LL12;case 5U: _LL11: _tmp66E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp66E);goto _LL0;case 3U: _LL13: _tmp66F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp66F);goto _LL0;case 7U: _LL15: _tmp671=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_tmp670=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL16:
 _tmp673=_tmp671;_tmp672=_tmp670;goto _LL18;case 8U: _LL17: _tmp673=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_tmp672=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL18:
 _tmp675=_tmp673;_tmp674=_tmp672;goto _LL1A;case 9U: _LL19: _tmp675=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_tmp674=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL1A:
 _tmp677=_tmp675;_tmp676=_tmp674;goto _LL1C;case 23U: _LL1B: _tmp677=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_tmp676=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL1C:
 _tmp679=_tmp677;_tmp678=_tmp676;goto _LL1E;case 35U: _LL1D: _tmp679=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_tmp678=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL1E:
 _tmp67B=_tmp679;_tmp67A=_tmp678;goto _LL20;case 4U: _LL1F: _tmp67B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_tmp67A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp65F)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp67B);Cyc_Toc_exptypes_to_c(_tmp67A);goto _LL0;case 6U: _LL21: _tmp67E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_tmp67D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_tmp67C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp65F)->f3;_LL22:
# 4166
 Cyc_Toc_exptypes_to_c(_tmp67E);Cyc_Toc_exptypes_to_c(_tmp67D);Cyc_Toc_exptypes_to_c(_tmp67C);goto _LL0;case 10U: _LL23: _tmp680=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_tmp67F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL24:
# 4168
 Cyc_Toc_exptypes_to_c(_tmp680);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp67F);goto _LL0;case 14U: _LL25: _tmp682=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_tmp681=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL26:
({void*_tmpAA4=Cyc_Toc_typ_to_c(*_tmp682);*_tmp682=_tmpAA4;});Cyc_Toc_exptypes_to_c(_tmp681);goto _LL0;case 25U: _LL27: _tmp684=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp65F)->f1)->f3;_tmp683=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL28:
# 4171
({void*_tmpAA5=Cyc_Toc_typ_to_c(*_tmp684);*_tmp684=_tmpAA5;});
_tmp685=_tmp683;goto _LL2A;case 36U: _LL29: _tmp685=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp65F)->f2;_LL2A:
 _tmp686=_tmp685;goto _LL2C;case 26U: _LL2B: _tmp686=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL2C:
# 4175
 for(0;_tmp686 != 0;_tmp686=_tmp686->tl){
struct _tuple19 _tmp660=*((struct _tuple19*)_tmp686->hd);struct _tuple19 _tmp661=_tmp660;struct Cyc_Absyn_Exp*_tmp662;_LL56: _tmp662=_tmp661.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp662);}
# 4179
goto _LL0;case 19U: _LL2D: _tmp687=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL2E:
 _tmp688=_tmp687;goto _LL30;case 17U: _LL2F: _tmp688=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL30:
({void*_tmpAA6=Cyc_Toc_typ_to_c(*_tmp688);*_tmp688=_tmpAA6;});goto _LL0;case 37U: _LL31: _tmp689=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp689);goto _LL0;case 34U: _LL33: _tmp68A=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp65F)->f1;_LL34:
# 4184
 if(_tmp68A->elt_type != 0)
({void**_tmpAA8=({void**_tmp663=_cycalloc(sizeof(*_tmp663));({void*_tmpAA7=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp68A->elt_type)));*_tmp663=_tmpAA7;});_tmp663;});_tmp68A->elt_type=_tmpAA8;});
Cyc_Toc_exptypes_to_c(_tmp68A->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4194
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
# 4205
({void*_tmp664=0U;({unsigned int _tmpAAA=e->loc;struct _dyneither_ptr _tmpAA9=({const char*_tmp665="Cyclone expression within C code";_tag_dyneither(_tmp665,sizeof(char),33U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpAAA,_tmpAA9,_tag_dyneither(_tmp664,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4209
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp68B=d->r;void*_tmp68C=_tmp68B;struct Cyc_Absyn_Typedefdecl*_tmp696;struct Cyc_Absyn_Enumdecl*_tmp695;struct Cyc_Absyn_Aggrdecl*_tmp694;struct Cyc_Absyn_Fndecl*_tmp693;struct Cyc_Absyn_Vardecl*_tmp692;switch(*((int*)_tmp68C)){case 0U: _LL1: _tmp692=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp68C)->f1;_LL2:
# 4212
({void*_tmpAAB=Cyc_Toc_typ_to_c(_tmp692->type);_tmp692->type=_tmpAAB;});
if(_tmp692->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp692->initializer));
goto _LL0;case 1U: _LL3: _tmp693=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp68C)->f1;_LL4:
# 4216
({void*_tmpAAC=Cyc_Toc_typ_to_c(_tmp693->ret_type);_tmp693->ret_type=_tmpAAC;});
{struct Cyc_List_List*_tmp68D=_tmp693->args;for(0;_tmp68D != 0;_tmp68D=_tmp68D->tl){
({void*_tmpAAD=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp68D->hd)).f3);(*((struct _tuple9*)_tmp68D->hd)).f3=_tmpAAD;});}}
# 4220
goto _LL0;case 5U: _LL5: _tmp694=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp68C)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp694);goto _LL0;case 7U: _LL7: _tmp695=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp68C)->f1;_LL8:
# 4223
 if(_tmp695->fields != 0){
struct Cyc_List_List*_tmp68E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp695->fields))->v;for(0;_tmp68E != 0;_tmp68E=_tmp68E->tl){
struct Cyc_Absyn_Enumfield*_tmp68F=(struct Cyc_Absyn_Enumfield*)_tmp68E->hd;
if(_tmp68F->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp68F->tag));}}
# 4228
goto _LL0;case 8U: _LL9: _tmp696=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp68C)->f1;_LLA:
({void*_tmpAAE=Cyc_Toc_typ_to_c((void*)_check_null(_tmp696->defn));_tmp696->defn=_tmpAAE;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4238
({void*_tmp690=0U;({unsigned int _tmpAB0=d->loc;struct _dyneither_ptr _tmpAAF=({const char*_tmp691="Cyclone declaration within C code";_tag_dyneither(_tmp691,sizeof(char),34U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpAB0,_tmpAAF,_tag_dyneither(_tmp690,sizeof(void*),0U));});});goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
 goto _LL0;}_LL0:;}
# 4244
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp697=s->r;void*_tmp698=_tmp697;struct Cyc_Absyn_Stmt*_tmp6B0;struct Cyc_Absyn_Stmt*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6AE;struct Cyc_Absyn_Decl*_tmp6AD;struct Cyc_Absyn_Stmt*_tmp6AC;struct Cyc_Absyn_Exp*_tmp6AB;struct Cyc_List_List*_tmp6AA;void*_tmp6A9;struct Cyc_Absyn_Exp*_tmp6A8;struct Cyc_Absyn_Exp*_tmp6A7;struct Cyc_Absyn_Exp*_tmp6A6;struct Cyc_Absyn_Stmt*_tmp6A5;struct Cyc_Absyn_Exp*_tmp6A4;struct Cyc_Absyn_Stmt*_tmp6A3;struct Cyc_Absyn_Exp*_tmp6A2;struct Cyc_Absyn_Stmt*_tmp6A1;struct Cyc_Absyn_Stmt*_tmp6A0;struct Cyc_Absyn_Exp*_tmp69F;struct Cyc_Absyn_Stmt*_tmp69E;struct Cyc_Absyn_Stmt*_tmp69D;struct Cyc_Absyn_Exp*_tmp69C;switch(*((int*)_tmp698)){case 1U: _LL1: _tmp69C=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp698)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp69C);goto _LL0;case 2U: _LL3: _tmp69E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp698)->f1;_tmp69D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp698)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp69E);Cyc_Toc_stmttypes_to_c(_tmp69D);goto _LL0;case 3U: _LL5: _tmp69F=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp698)->f1;_LL6:
 if(_tmp69F != 0)Cyc_Toc_exptypes_to_c(_tmp69F);goto _LL0;case 4U: _LL7: _tmp6A2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp698)->f1;_tmp6A1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp698)->f2;_tmp6A0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp698)->f3;_LL8:
# 4250
 Cyc_Toc_exptypes_to_c(_tmp6A2);Cyc_Toc_stmttypes_to_c(_tmp6A1);Cyc_Toc_stmttypes_to_c(_tmp6A0);goto _LL0;case 5U: _LL9: _tmp6A4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp698)->f1).f1;_tmp6A3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp698)->f2;_LLA:
# 4252
 Cyc_Toc_exptypes_to_c(_tmp6A4);Cyc_Toc_stmttypes_to_c(_tmp6A3);goto _LL0;case 9U: _LLB: _tmp6A8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp698)->f1;_tmp6A7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp698)->f2).f1;_tmp6A6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp698)->f3).f1;_tmp6A5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp698)->f4;_LLC:
# 4254
 Cyc_Toc_exptypes_to_c(_tmp6A8);Cyc_Toc_exptypes_to_c(_tmp6A7);Cyc_Toc_exptypes_to_c(_tmp6A6);
Cyc_Toc_stmttypes_to_c(_tmp6A5);goto _LL0;case 10U: _LLD: _tmp6AB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp698)->f1;_tmp6AA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp698)->f2;_tmp6A9=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp698)->f3;_LLE:
# 4257
 Cyc_Toc_exptypes_to_c(_tmp6AB);
for(0;_tmp6AA != 0;_tmp6AA=_tmp6AA->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp6AA->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp6AD=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp698)->f1;_tmp6AC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp698)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp6AD);Cyc_Toc_stmttypes_to_c(_tmp6AC);goto _LL0;case 14U: _LL11: _tmp6AF=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp698)->f1;_tmp6AE=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp698)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp6AF);Cyc_Toc_exptypes_to_c(_tmp6AE);goto _LL0;case 13U: _LL13: _tmp6B0=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp698)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp6B0);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4267
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4270
({void*_tmpAB1=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->tag=0U;_tmp699;});s->r=_tmpAB1;});
goto _LL0;default: _LL1F: _LL20:
# 4273
({void*_tmp69A=0U;({unsigned int _tmpAB3=s->loc;struct _dyneither_ptr _tmpAB2=({const char*_tmp69B="Cyclone statement in C code";_tag_dyneither(_tmp69B,sizeof(char),28U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpAB3,_tmpAB2,_tag_dyneither(_tmp69A,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4277
static void*Cyc_Toc_decl_to_enumtype(struct Cyc_Absyn_Decl*d){
void*_tmp6B1=d->r;void*_tmp6B2=_tmp6B1;struct Cyc_Absyn_Enumdecl*_tmp6B7;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6B2)->tag == 7U){_LL1: _tmp6B7=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6B2)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4->tag=0U,({void*_tmpAB4=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3->tag=15U,_tmp6B3->f1=_tmp6B7->name,_tmp6B3->f2=_tmp6B7;_tmp6B3;});_tmp6B4->f1=_tmpAB4;}),_tmp6B4->f2=0;_tmp6B4;});}else{_LL3: _LL4:
({void*_tmp6B5=0U;({struct _dyneither_ptr _tmpAB5=({const char*_tmp6B6="Toc::get_enumdecl_name";_tag_dyneither(_tmp6B6,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAB5,_tag_dyneither(_tmp6B5,sizeof(void*),0U));});});}_LL0:;}
# 4284
static struct Cyc_Absyn_Decl*Cyc_Toc_type2enumdecl(void*t){
void*_tmp6B8=t;struct Cyc_Absyn_Enumdecl*_tmp6BC;unsigned int _tmp6BB;if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6B8)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6B8)->f1)->r)->tag == 1U){_LL1: _tmp6BC=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6B8)->f1)->r)->f1;_tmp6BB=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6B8)->f1)->loc;_LL2:
# 4287
 return({struct Cyc_Absyn_Decl*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));({void*_tmpAB6=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->tag=7U,_tmp6B9->f1=_tmp6BC;_tmp6B9;});_tmp6BA->r=_tmpAB6;}),_tmp6BA->loc=_tmp6BB;_tmp6BA;});}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 4295
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp6BD=0U;({struct _dyneither_ptr _tmpAB7=({const char*_tmp6BE="decls_to_c: not at toplevel!";_tag_dyneither(_tmp6BE,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAB7,_tag_dyneither(_tmp6BD,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp6BF=d->r;void*_tmp6C0=_tmp6BF;struct Cyc_List_List*_tmp6DB;struct Cyc_List_List*_tmp6DA;struct Cyc_List_List*_tmp6D9;struct Cyc_List_List*_tmp6D8;struct Cyc_Absyn_Typedefdecl*_tmp6D7;struct Cyc_Absyn_Enumdecl*_tmp6D6;struct Cyc_Absyn_Datatypedecl*_tmp6D5;struct Cyc_Absyn_Aggrdecl*_tmp6D4;struct Cyc_Absyn_Fndecl*_tmp6D3;struct Cyc_Absyn_Vardecl*_tmp6D2;switch(*((int*)_tmp6C0)){case 0U: _LL1: _tmp6D2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6C0)->f1;_LL2: {
# 4303
struct _tuple1*_tmp6C1=_tmp6D2->name;
# 4305
if(_tmp6D2->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpAB9=({struct _tuple1*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));({union Cyc_Absyn_Nmspace _tmpAB8=Cyc_Absyn_Abs_n(0,1);_tmp6C2->f1=_tmpAB8;}),_tmp6C2->f2=(*_tmp6C1).f2;_tmp6C2;});_tmp6C1=_tmpAB9;});
if(_tmp6D2->initializer != 0){
if(_tmp6D2->sc == Cyc_Absyn_ExternC)_tmp6D2->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6D2->initializer));else{
# 4312
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp6D2->initializer));}}
# 4314
_tmp6D2->name=_tmp6C1;
({enum Cyc_Absyn_Scope _tmpABA=Cyc_Toc_scope_to_c(_tmp6D2->sc);_tmp6D2->sc=_tmpABA;});
({void*_tmpABB=Cyc_Toc_typ_to_c(_tmp6D2->type);_tmp6D2->type=_tmpABB;});
({struct Cyc_List_List*_tmpABC=({struct Cyc_List_List*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->hd=d,_tmp6C3->tl=Cyc_Toc_result_decls;_tmp6C3;});Cyc_Toc_result_decls=_tmpABC;});
goto _LL0;}case 1U: _LL3: _tmp6D3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6C0)->f1;_LL4: {
# 4320
struct _tuple1*_tmp6C4=_tmp6D3->name;
# 4322
if(_tmp6D3->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpABE=({struct _tuple1*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));({union Cyc_Absyn_Nmspace _tmpABD=Cyc_Absyn_Abs_n(0,1);_tmp6C5->f1=_tmpABD;}),_tmp6C5->f2=(*_tmp6C4).f2;_tmp6C5;});_tmp6C4=_tmpABE;});
_tmp6D3->sc=Cyc_Absyn_Public;}
# 4326
_tmp6D3->name=_tmp6C4;
Cyc_Toc_fndecl_to_c(nv,_tmp6D3,cinclude);
({struct Cyc_List_List*_tmpABF=({struct Cyc_List_List*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6->hd=d,_tmp6C6->tl=Cyc_Toc_result_decls;_tmp6C6;});Cyc_Toc_result_decls=_tmpABF;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp6C7=0U;({struct _dyneither_ptr _tmpAC0=({const char*_tmp6C8="letdecl at toplevel";_tag_dyneither(_tmp6C8,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAC0,_tag_dyneither(_tmp6C7,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp6C9=0U;({struct _dyneither_ptr _tmpAC1=({const char*_tmp6CA="region decl at toplevel";_tag_dyneither(_tmp6CA,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAC1,_tag_dyneither(_tmp6C9,sizeof(void*),0U));});});case 5U: _LLB: _tmp6D4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6C0)->f1;_LLC:
# 4334
 Cyc_Toc_aggrdecl_to_c(_tmp6D4);
goto _LL0;case 6U: _LLD: _tmp6D5=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp6C0)->f1;_LLE:
# 4337
 if(_tmp6D5->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp6D5);else{
# 4340
Cyc_Toc_datatypedecl_to_c(_tmp6D5);}
goto _LL0;case 7U: _LLF: _tmp6D6=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C0)->f1;_LL10:
# 4343
 Cyc_Toc_enumdecl_to_c(_tmp6D6);
({struct Cyc_List_List*_tmpAC2=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->hd=d,_tmp6CB->tl=Cyc_Toc_result_decls;_tmp6CB;});Cyc_Toc_result_decls=_tmpAC2;});
goto _LL0;case 8U: _LL11: _tmp6D7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6C0)->f1;_LL12:
# 4347
 _tmp6D7->tvs=0;
if(_tmp6D7->defn != 0){
void*_tmp6CC=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6D7->defn));
_tmp6D7->defn=_tmp6CC;{
# 4353
struct Cyc_Absyn_Decl*_tmp6CD=Cyc_Toc_type2enumdecl(_tmp6CC);
if(_tmp6CD != 0){
({struct Cyc_List_List*_tmpAC3=({struct Cyc_List_List*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->hd=_tmp6CD,_tmp6CE->tl=Cyc_Toc_result_decls;_tmp6CE;});Cyc_Toc_result_decls=_tmpAC3;});
({void*_tmpAC4=Cyc_Toc_decl_to_enumtype(_tmp6CD);_tmp6D7->defn=_tmpAC4;});}};}else{
# 4359
enum Cyc_Absyn_KindQual _tmp6CF=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp6D7->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp6D0=_tmp6CF;if(_tmp6D0 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpAC5=Cyc_Absyn_void_star_type();_tmp6D7->defn=_tmpAC5;});goto _LL1F;}else{_LL22: _LL23:
 _tmp6D7->defn=Cyc_Absyn_void_type;goto _LL1F;}_LL1F:;}
# 4367
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpAC6=({struct Cyc_List_List*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->hd=d,_tmp6D1->tl=Cyc_Toc_result_decls;_tmp6D1;});Cyc_Toc_result_decls=_tmpAC6;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp6D8=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6C0)->f2;_LL18:
 _tmp6D9=_tmp6D8;goto _LL1A;case 10U: _LL19: _tmp6D9=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6C0)->f2;_LL1A:
 _tmp6DA=_tmp6D9;goto _LL1C;case 11U: _LL1B: _tmp6DA=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6C0)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpAC7=Cyc_Toc_decls_to_c(r,nv,_tmp6DA,top,cinclude);nv=_tmpAC7;});goto _LL0;default: _LL1D: _tmp6DB=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6C0)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpAC8=Cyc_Toc_decls_to_c(r,nv,_tmp6DB,top,1);nv=_tmpAC8;});goto _LL0;}_LL0:;};}
# 4378
return nv;}
# 4382
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp6DC=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp6DD=_tmp6DC;struct Cyc_Core_DynamicRegion*_tmp6E2;_LL1: _tmp6E2=_tmp6DD.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp6E2->h;
({struct Cyc_Toc_TocState*_tmpAC9=Cyc_Toc_empty_toc_state(h);ts=_tmpAC9;});;}
({struct Cyc_Toc_TocStateWrap*_tmpACA=({struct Cyc_Toc_TocStateWrap*_tmp6DF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6DF));_tmp6DF->dyn=_tmp6E2,_tmp6DF->state=ts;_tmp6DF;});Cyc_Toc_toc_state=_tmpACA;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
({struct _dyneither_ptr _tmpACB=_tag_dyneither(({unsigned int _tmp6E1=35;struct _dyneither_ptr**_tmp6E0=_cycalloc(_check_times(_tmp6E1,sizeof(struct _dyneither_ptr*)));_tmp6E0[0]=& Cyc_Toc__throw_str,_tmp6E0[1]=& Cyc_Toc_setjmp_str,_tmp6E0[2]=& Cyc_Toc__push_handler_str,_tmp6E0[3]=& Cyc_Toc__pop_handler_str,_tmp6E0[4]=& Cyc_Toc__exn_thrown_str,_tmp6E0[5]=& Cyc_Toc__npop_handler_str,_tmp6E0[6]=& Cyc_Toc__check_null_str,_tmp6E0[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp6E0[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp6E0[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp6E0[10]=& Cyc_Toc__dyneither_ptr_str,_tmp6E0[11]=& Cyc_Toc__tag_dyneither_str,_tmp6E0[12]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp6E0[13]=& Cyc_Toc__get_dyneither_size_str,_tmp6E0[14]=& Cyc_Toc__get_zero_arr_size_str,_tmp6E0[15]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp6E0[16]=& Cyc_Toc__zero_arr_plus_str,_tmp6E0[17]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp6E0[18]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp6E0[19]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp6E0[20]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp6E0[21]=& Cyc_Toc__cycalloc_str,_tmp6E0[22]=& Cyc_Toc__cyccalloc_str,_tmp6E0[23]=& Cyc_Toc__cycalloc_atomic_str,_tmp6E0[24]=& Cyc_Toc__cyccalloc_atomic_str,_tmp6E0[25]=& Cyc_Toc__region_malloc_str,_tmp6E0[26]=& Cyc_Toc__region_calloc_str,_tmp6E0[27]=& Cyc_Toc__check_times_str,_tmp6E0[28]=& Cyc_Toc__new_region_str,_tmp6E0[29]=& Cyc_Toc__push_region_str,_tmp6E0[30]=& Cyc_Toc__pop_region_str,_tmp6E0[31]=& Cyc_Toc__throw_arraybounds_str,_tmp6E0[32]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp6E0[33]=& Cyc_Toc__throw_match_str,_tmp6E0[34]=& Cyc_Toc__fast_region_malloc_str;_tmp6E0;}),sizeof(struct _dyneither_ptr*),35U);Cyc_Toc_globals=_tmpACB;});};}
# 4431
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp6E3=ts;struct Cyc_Toc_TocStateWrap*_tmp6E4=Cyc_Toc_toc_state;ts=_tmp6E4;Cyc_Toc_toc_state=_tmp6E3;});{
struct Cyc_Toc_TocStateWrap _tmp6E5=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp6E6=_tmp6E5;struct Cyc_Core_DynamicRegion*_tmp6EC;struct Cyc_Toc_TocState*_tmp6EB;_LL1: _tmp6EC=_tmp6E6.dyn;_tmp6EB=_tmp6E6.state;_LL2:;
# 4436
{struct _RegionHandle*h=& _tmp6EC->h;
{struct Cyc_Toc_TocState _tmp6E8=*_tmp6EB;struct Cyc_Toc_TocState _tmp6E9=_tmp6E8;struct Cyc_Xarray_Xarray*_tmp6EA;_LL4: _tmp6EA=_tmp6E9.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp6EA);}
# 4437
;}
# 4440
Cyc_Core_free_rckey(_tmp6EC);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4443
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4449
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4451
({struct Cyc_Hashtable_Table**_tmpACC=({struct Cyc_Hashtable_Table**_tmp6ED=_cycalloc(sizeof(*_tmp6ED));*_tmp6ED=pop_tables;_tmp6ED;});Cyc_Toc_gpop_tables=_tmpACC;});
Cyc_Toc_init();
{struct _RegionHandle _tmp6EE=_new_region("start");struct _RegionHandle*start=& _tmp6EE;_push_region(start);
({struct _RegionHandle*_tmpACE=start;struct Cyc_Toc_Env*_tmpACD=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpACE,_tmpACD,ds,1,0);});;_pop_region(start);}
# 4456
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}

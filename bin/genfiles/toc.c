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
({struct _dyneither_ptr _tmp724=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp723=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp723,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp724,ap);});}
# 79
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 81
({struct _dyneither_ptr _tmp726=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp725=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp725,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp726,ap);});}
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
({struct Cyc_List_List**_tmp732=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp732;}),({
struct Cyc_List_List**_tmp731=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp731;}),({
struct Cyc_Dict_Dict*_tmp730=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp72F=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp72F;});_tmpC;});_tmp11->aggrs_so_far=_tmp730;}),({
struct Cyc_List_List**_tmp72E=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp72E;}),({
struct Cyc_Set_Set**_tmp72D=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp72C=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp72C;});_tmpE;});_tmp11->datatypes_so_far=_tmp72D;}),({
struct Cyc_Dict_Dict*_tmp72B=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp72A=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp72A;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp72B;}),({
struct Cyc_Dict_Dict*_tmp729=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp728=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp728;});_tmp10;});_tmp11->qvar_tags=_tmp729;}),({
struct Cyc_Xarray_Xarray*_tmp727=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp727;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 153
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 159
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 162
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp733=({struct Cyc_Toc_TocStateWrap _tmp6F2;_tmp6F2.dyn=_tmp1C,_tmp6F2.state=_tmp1B;_tmp6F2;});*ts=_tmp733;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp1A=({struct _tuple15 _tmp6F3;_tmp6F3.f1=_tmp1B,_tmp6F3.f2=arg;_tmp6F3;});
({void*_tmp734=f(h,& _tmp1A);res=_tmp734;});}
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
struct _tuple16 env=({struct _tuple16 _tmp6F4;_tmp6F4.f1=q,_tmp6F4.f2=type_maker;_tmp6F4;});
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
# 291
void*Cyc_Toc_dyneither_ptr_type(){
static void*dpt=0;
void*_tmp6D=dpt;
if(_tmp6D == 0){
({void*_tmp735=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0),0);_tmp6D=_tmp735;});
dpt=_tmp6D;}
# 298
return _tmp6D;}
# 301
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 303
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 307
static struct _tuple19*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp70=_cycalloc(sizeof(*_tmp70));({struct Cyc_List_List*_tmp737=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));({void*_tmp736=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=1U,_tmp6E->f1=name;_tmp6E;});_tmp6F->hd=_tmp736;}),_tmp6F->tl=0;_tmp6F;});_tmp70->f1=_tmp737;}),_tmp70->f2=e;_tmp70;});}
# 313
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp71=e->r;void*_tmp72=_tmp71;struct Cyc_Absyn_Exp*_tmp73;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp73=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp73);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 319
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=14U,_tmp74->f1=t,_tmp74->f2=e,_tmp74->f3=0,_tmp74->f4=Cyc_Absyn_No_coercion;_tmp74;});}
# 322
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=20U,_tmp75->f1=e;_tmp75;});}
# 325
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=23U,_tmp76->f1=e1,_tmp76->f2=e2;_tmp76;});}
# 328
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=37U,_tmp77->f1=s;_tmp77;});}
# 331
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=17U,_tmp78->f1=t;_tmp78;});}
# 334
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=10U,_tmp79->f1=e,_tmp79->f2=es,_tmp79->f3=0,_tmp79->f4=1;_tmp79;});}
# 337
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=1U,_tmp7A->f1=e;_tmp7A;});}
# 340
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=2U,_tmp7B->f1=s1,_tmp7B->f2=s2;_tmp7B;});}
# 343
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=6U,_tmp7C->f1=e1,_tmp7C->f2=e2,_tmp7C->f3=e3;_tmp7C;});}
# 346
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=21U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});}
# 349
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=22U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});}
# 352
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 355
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=36U,_tmp7F->f1=tdopt,_tmp7F->f2=ds;_tmp7F;});}
# 357
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=8U,_tmp80->f1=v;_tmp80;});}
# 360
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 364
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp81=e->r;void*_tmp82=_tmp81;struct Cyc_Absyn_Exp*_tmp83;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->tag == 20U){_LL1: _tmp83=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp83,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 383
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 392
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 401
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 410
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 421
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp84=Cyc_Tcutil_compress(t);void*_tmp85=_tmp84;enum Cyc_Absyn_Size_of _tmp8C;switch(*((int*)_tmp85)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)){case 1U: _LL1: _tmp8C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f2;_LL2:
# 425
{enum Cyc_Absyn_Size_of _tmp86=_tmp8C;switch(_tmp86){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp87=0U;({struct _dyneither_ptr _tmp738=({const char*_tmp88="impossible IntCon (not char, short or int)";_tag_dyneither(_tmp88,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp738,_tag_dyneither(_tmp87,sizeof(void*),0U));});});}_LLD:;}
# 431
goto _LL0;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}default: goto _LLB;}case 3U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp6F5;_tmp6F5.tag=0U,({struct _dyneither_ptr _tmp739=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp6F5.f1=_tmp739;});_tmp6F5;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({struct _dyneither_ptr _tmp73A=({const char*_tmp8A="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8A,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp73A,_tag_dyneither(_tmp89,sizeof(void*),1U));});});}_LL0:;}
# 438
return function;}
# 440
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8D=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8E=_tmp8D;void*_tmp91;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->tag == 3U){_LL1: _tmp91=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp91);}else{_LL3: _LL4:
({void*_tmp8F=0U;({struct _dyneither_ptr _tmp73B=({const char*_tmp90="impossible type (not pointer)";_tag_dyneither(_tmp90,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp73B,_tag_dyneither(_tmp8F,sizeof(void*),0U));});});}_LL0:;}
# 450
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp92=e->r;void*_tmp93=_tmp92;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;long long _tmp99;int _tmp98;short _tmp97;struct _dyneither_ptr _tmp96;char _tmp95;switch(*((int*)_tmp93)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Null_c).tag){case 2U: _LL1: _tmp95=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Char_c).val).f2;_LL2:
 return _tmp95 == '\000';case 3U: _LL3: _tmp96=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Wchar_c).val;_LL4: {
# 454
unsigned long _tmp94=Cyc_strlen((struct _dyneither_ptr)_tmp96);
int i=0;
if(_tmp94 >= 2  && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),1))== 'x'  && _tmp94 >= 3) && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp94;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 464
return 0;}}case 4U: _LL5: _tmp97=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Short_c).val).f2;_LL6:
 return _tmp97 == 0;case 5U: _LL7: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Int_c).val).f2;_LL8:
 return _tmp98 == 0;case 6U: _LL9: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).LongLong_c).val).f2;_LLA:
 return _tmp99 == 0;case 1U: _LLD: _LLE:
# 469
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 468
 goto _LLE;case 14U: _LLF: _tmp9A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL10:
# 470
 return Cyc_Toc_is_zero(_tmp9A);case 24U: _LL11: _tmp9B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9B);case 26U: _LL13: _tmp9C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL14:
 _tmp9D=_tmp9C;goto _LL16;case 29U: _LL15: _tmp9D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_LL16:
 _tmp9E=_tmp9D;goto _LL18;case 25U: _LL17: _tmp9E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL18:
 _tmp9F=_tmp9E;goto _LL1A;case 36U: _LL19: _tmp9F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL1A:
# 476
 for(0;_tmp9F != 0;_tmp9F=_tmp9F->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp9F->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 484
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA0=x;union Cyc_Absyn_Nmspace _tmpB2;struct _dyneither_ptr*_tmpB1;_LL1: _tmpB2=_tmpA0->f1;_tmpB1=_tmpA0->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA1=_tmpB2;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpAE;switch((_tmpA1.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAE=0;goto _LL7;case 1U: _LL6: _tmpAE=(_tmpA1.Rel_n).val;_LL7:
 _tmpAF=_tmpAE;goto _LL9;case 2U: _LL8: _tmpAF=(_tmpA1.Abs_n).val;_LL9:
 _tmpB0=_tmpAF;goto _LLB;default: _LLA: _tmpB0=(_tmpA1.C_n).val;_LLB:
# 493
 if(_tmpB0 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp6F7;_tmp6F7.tag=0U,_tmp6F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp6F7;});struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp6F6;_tmp6F6.tag=0U,_tmp6F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp6F6;});void*_tmpA2[2U];_tmpA2[0]=& _tmpA4,_tmpA2[1]=& _tmpA5;({struct _dyneither_ptr _tmp73C=({const char*_tmpA3="%s_%s_struct";_tag_dyneither(_tmpA3,sizeof(char),13U);});Cyc_aprintf(_tmp73C,_tag_dyneither(_tmpA2,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA6=_new_region("r");struct _RegionHandle*r=& _tmpA6;_push_region(r);
{struct _dyneither_ptr _tmpAD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp6FA;_tmp6FA.tag=0U,_tmp6FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp6FA;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp6F9;_tmp6F9.tag=0U,({struct _dyneither_ptr _tmp73F=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp73E=r;struct Cyc_List_List*_tmp73D=_tmpB0;Cyc_rstr_sepstr(_tmp73E,_tmp73D,({const char*_tmpAC="_";_tag_dyneither(_tmpAC,sizeof(char),2U);}));}));_tmp6F9.f1=_tmp73F;});_tmp6F9;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp6F8;_tmp6F8.tag=0U,_tmp6F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp6F8;});void*_tmpA7[3U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA,_tmpA7[2]=& _tmpAB;({struct _dyneither_ptr _tmp740=({const char*_tmpA8="%s_%s_%s_struct";_tag_dyneither(_tmpA8,sizeof(char),16U);});Cyc_aprintf(_tmp740,_tag_dyneither(_tmpA7,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAD;};_pop_region(r);};}_LL3:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 506
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 509
struct _tuple20 _tmpB3=*env;struct _tuple20 _tmpB4=_tmpB3;struct Cyc_Dict_Dict*_tmpC7;struct _tuple14*_tmpC6;_LL1: _tmpC7=(_tmpB4.f1)->qvar_tags;_tmpC6=_tmpB4.f2;_LL2:;{
struct _tuple14 _tmpB5=*_tmpC6;struct _tuple14 _tmpB6=_tmpB5;struct _tuple1*_tmpC5;struct _tuple1*_tmpC4;_LL4: _tmpC5=_tmpB6.f1;_tmpC4=_tmpB6.f2;_LL5:;{
struct _handler_cons _tmpB7;_push_handler(& _tmpB7);{int _tmpB9=0;if(setjmp(_tmpB7.handler))_tmpB9=1;if(!_tmpB9){{struct _tuple1*_tmpBA=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmpC7,Cyc_Toc_qvar_tag_cmp,_tmpC6);_npop_handler(0U);return _tmpBA;};_pop_handler();}else{void*_tmpB8=(void*)_exn_thrown;void*_tmpBB=_tmpB8;void*_tmpC3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBB)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 513
struct _tuple14*_tmpBC=({struct _tuple14*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->f1=_tmpC5,_tmpC2->f2=_tmpC4;_tmpC2;});
struct _tuple1*_tmpBD=_tmpC5;union Cyc_Absyn_Nmspace _tmpC1;struct _dyneither_ptr*_tmpC0;_LLC: _tmpC1=_tmpBD->f1;_tmpC0=_tmpBD->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC0,_tmpC4);
struct _tuple1*res=({struct _tuple1*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->f1=_tmpC1,({struct _dyneither_ptr*_tmp741=({struct _dyneither_ptr*_tmpBE=_cycalloc(sizeof(*_tmpBE));*_tmpBE=newvar;_tmpBE;});_tmpBF->f2=_tmp741;});_tmpBF;});
({struct Cyc_Dict_Dict _tmp742=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC7,_tmpBC,res);*_tmpC7=_tmp742;});
return res;};}}else{_LL9: _tmpC3=_tmpBB;_LLA:(int)_rethrow(_tmpC3);}_LL6:;}};};};}
# 521
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp6FB;_tmp6FB.f1=fieldname,_tmp6FB.f2=dtname;_tmp6FB;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 528
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->kind=Cyc_Absyn_StructA,_tmpCA->sc=Cyc_Absyn_Public,_tmpCA->tvs=0,_tmpCA->attributes=0,_tmpCA->expected_mem_kind=0,({
# 531
struct _tuple1*_tmp745=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));({union Cyc_Absyn_Nmspace _tmp744=Cyc_Absyn_Rel_n(0);_tmpC8->f1=_tmp744;}),_tmpC8->f2=name;_tmpC8;});_tmpCA->name=_tmp745;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp743=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->exist_vars=0,_tmpC9->rgn_po=0,_tmpC9->tagged=0,_tmpC9->fields=fs;_tmpC9;});_tmpCA->impl=_tmp743;});_tmpCA;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 540
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 543
struct _tuple21 _tmpCB=*env;struct _tuple21 _tmpCC=_tmpCB;struct Cyc_List_List**_tmpE2;struct Cyc_List_List*_tmpE1;_LL1: _tmpE2=(_tmpCC.f1)->tuple_types;_tmpE1=_tmpCC.f2;_LL2:;
# 545
{struct Cyc_List_List*_tmpCD=*_tmpE2;for(0;_tmpCD != 0;_tmpCD=_tmpCD->tl){
struct _tuple22*_tmpCE=(struct _tuple22*)_tmpCD->hd;struct _tuple22*_tmpCF=_tmpCE;void*_tmpD2;struct Cyc_List_List*_tmpD1;_LL4: _tmpD2=_tmpCF->f1;_tmpD1=_tmpCF->f2;_LL5:;{
struct Cyc_List_List*_tmpD0=_tmpE1;
for(0;_tmpD0 != 0  && _tmpD1 != 0;(_tmpD0=_tmpD0->tl,_tmpD1=_tmpD1->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD0->hd)).f2,(void*)_tmpD1->hd))
break;}
if(_tmpD0 == 0  && _tmpD1 == 0)
return _tmpD2;};}}{
# 556
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct _dyneither_ptr _tmp747=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDF=({struct Cyc_Int_pa_PrintArg_struct _tmp6FC;_tmp6FC.tag=1U,_tmp6FC.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp6FC;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({struct _dyneither_ptr _tmp746=({const char*_tmpDE="_tuple%d";_tag_dyneither(_tmpDE,sizeof(char),9U);});Cyc_aprintf(_tmp746,_tag_dyneither(_tmpDD,sizeof(void*),1U));});});*_tmpE0=_tmp747;});_tmpE0;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE1);
struct Cyc_List_List*_tmpD3=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp74A=({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct Cyc_Absyn_Aggrfield*_tmp749=({struct Cyc_Absyn_Aggrfield*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct _dyneither_ptr*_tmp748=Cyc_Absyn_fieldname(i);_tmpD4->name=_tmp748;}),_tmpD4->tq=Cyc_Toc_mt_tq,_tmpD4->type=(void*)ts2->hd,_tmpD4->width=0,_tmpD4->attributes=0,_tmpD4->requires_clause=0;_tmpD4;});_tmpD5->hd=_tmp749;}),_tmpD5->tl=_tmpD3;_tmpD5;});_tmpD3=_tmp74A;});}}
({struct Cyc_List_List*_tmp74B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD3);_tmpD3=_tmp74B;});{
struct Cyc_Absyn_Aggrdecl*_tmpD6=Cyc_Toc_make_c_struct_defn(xname,_tmpD3);
void*_tmpD7=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDC=_cycalloc(sizeof(*_tmpDC));*_tmpDC=_tmpD6;_tmpDC;})),0);
({struct Cyc_List_List*_tmp74D=({struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct Cyc_Absyn_Decl*_tmp74C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=5U,_tmpD8->f1=_tmpD6;_tmpD8;}),0U);_tmpD9->hd=_tmp74C;}),_tmpD9->tl=Cyc_Toc_result_decls;_tmpD9;});Cyc_Toc_result_decls=_tmp74D;});
({struct Cyc_List_List*_tmp74F=({struct Cyc_List_List*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));({struct _tuple22*_tmp74E=({struct _tuple22*_tmpDA=_region_malloc(d,sizeof(*_tmpDA));_tmpDA->f1=_tmpD7,_tmpDA->f2=ts;_tmpDA;});_tmpDB->hd=_tmp74E;}),_tmpDB->tl=*_tmpE2;_tmpDB;});*_tmpE2=_tmp74F;});
return _tmpD7;};};}
# 569
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 574
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 577
struct _tuple24*_tmpE3=env;struct Cyc_List_List**_tmpF7;enum Cyc_Absyn_AggrKind _tmpF6;struct Cyc_List_List*_tmpF5;_LL1: _tmpF7=(_tmpE3->f1)->anon_aggr_types;_tmpF6=(_tmpE3->f2)->f1;_tmpF5=(_tmpE3->f2)->f2;_LL2:;
# 579
{struct Cyc_List_List*_tmpE4=*_tmpF7;for(0;_tmpE4 != 0;_tmpE4=_tmpE4->tl){
struct _tuple25*_tmpE5=(struct _tuple25*)_tmpE4->hd;struct _tuple25*_tmpE6=_tmpE5;void*_tmpE9;enum Cyc_Absyn_AggrKind _tmpE8;struct Cyc_List_List*_tmpE7;_LL4: _tmpE9=_tmpE6->f1;_tmpE8=_tmpE6->f2;_tmpE7=_tmpE6->f3;_LL5:;
if(_tmpF6 != _tmpE8)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE7,_tmpF5))
return _tmpE9;}}{
# 588
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF4=_cycalloc(sizeof(*_tmpF4));({struct _dyneither_ptr _tmp751=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF3=({struct Cyc_Int_pa_PrintArg_struct _tmp6FD;_tmp6FD.tag=1U,_tmp6FD.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp6FD;});void*_tmpF1[1U];_tmpF1[0]=& _tmpF3;({struct _dyneither_ptr _tmp750=({const char*_tmpF2="_tuple%d";_tag_dyneither(_tmpF2,sizeof(char),9U);});Cyc_aprintf(_tmp750,_tag_dyneither(_tmpF1,sizeof(void*),1U));});});*_tmpF4=_tmp751;});_tmpF4;});
struct Cyc_Absyn_Aggrdecl*_tmpEA=Cyc_Toc_make_c_struct_defn(xname,_tmpF5);
_tmpEA->kind=_tmpF6;{
void*_tmpEB=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF0=_cycalloc(sizeof(*_tmpF0));*_tmpF0=_tmpEA;_tmpF0;})),0);
({struct Cyc_List_List*_tmp753=({struct Cyc_List_List*_tmpED=_cycalloc(sizeof(*_tmpED));({struct Cyc_Absyn_Decl*_tmp752=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->tag=5U,_tmpEC->f1=_tmpEA;_tmpEC;}),0U);_tmpED->hd=_tmp752;}),_tmpED->tl=Cyc_Toc_result_decls;_tmpED;});Cyc_Toc_result_decls=_tmp753;});
({struct Cyc_List_List*_tmp755=({struct Cyc_List_List*_tmpEF=_region_malloc(d,sizeof(*_tmpEF));({struct _tuple25*_tmp754=({struct _tuple25*_tmpEE=_region_malloc(d,sizeof(*_tmpEE));_tmpEE->f1=_tmpEB,_tmpEE->f2=_tmpF6,_tmpEE->f3=_tmpF5;_tmpEE;});_tmpEF->hd=_tmp754;}),_tmpEF->tl=*_tmpF7;_tmpEF;});*_tmpF7=_tmp755;});
return _tmpEB;};};}
# 596
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(({struct _tuple23*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->f1=ak,_tmpF8->f2=fs;_tmpF8;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple26{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple27{struct Cyc_Toc_TocState*f1;struct _tuple26*f2;};struct _tuple28{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 605
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple27*env){
# 614
struct _tuple27 _tmpF9=*env;struct _tuple27 _tmpFA=_tmpF9;struct Cyc_List_List**_tmp11D;struct _tuple1*_tmp11C;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_List_List*_tmp119;_LL1: _tmp11D=(_tmpFA.f1)->abs_struct_types;_tmp11C=(_tmpFA.f2)->f1;_tmp11B=(_tmpFA.f2)->f2;_tmp11A=(_tmpFA.f2)->f3;_tmp119=(_tmpFA.f2)->f4;_LL2:;
# 618
{struct Cyc_List_List*_tmpFB=*_tmp11D;for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){
struct _tuple28*_tmpFC=(struct _tuple28*)_tmpFB->hd;struct _tuple28*_tmpFD=_tmpFC;struct _tuple1*_tmp105;struct Cyc_List_List*_tmp104;void*_tmp103;_LL4: _tmp105=_tmpFD->f1;_tmp104=_tmpFD->f2;_tmp103=_tmpFD->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp105,_tmp11C)== 0  && ({
int _tmp756=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11A);_tmp756 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp104);})){
int okay=1;
{struct Cyc_List_List*_tmpFE=_tmp11A;for(0;_tmpFE != 0;(_tmpFE=_tmpFE->tl,_tmp104=_tmp104->tl)){
void*_tmpFF=(void*)_tmpFE->hd;
void*_tmp100=(void*)((struct Cyc_List_List*)_check_null(_tmp104))->hd;
{struct Cyc_Absyn_Kind*_tmp101=Cyc_Tcutil_type_kind(_tmpFF);struct Cyc_Absyn_Kind*_tmp102=_tmp101;switch(((struct Cyc_Absyn_Kind*)_tmp102)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 629
 continue;default: _LLB: _LLC:
# 632
 if(Cyc_Tcutil_unify(_tmpFF,_tmp100) || ({void*_tmp757=Cyc_Toc_typ_to_c(_tmpFF);Cyc_Tcutil_unify(_tmp757,Cyc_Toc_typ_to_c(_tmp100));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 637
break;}}
# 639
if(okay)
return _tmp103;}}}{
# 646
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp118=_cycalloc(sizeof(*_tmp118));({struct _dyneither_ptr _tmp759=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp117=({struct Cyc_Int_pa_PrintArg_struct _tmp6FE;_tmp6FE.tag=1U,_tmp6FE.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp6FE;});void*_tmp115[1U];_tmp115[0]=& _tmp117;({struct _dyneither_ptr _tmp758=({const char*_tmp116="_tuple%d";_tag_dyneither(_tmp116,sizeof(char),9U);});Cyc_aprintf(_tmp758,_tag_dyneither(_tmp115,sizeof(void*),1U));});});*_tmp118=_tmp759;});_tmp118;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp106=0;
# 650
({struct Cyc_List_List*_tmp75B=({struct Cyc_List_List*_tmp108=_region_malloc(d,sizeof(*_tmp108));({struct _tuple28*_tmp75A=({struct _tuple28*_tmp107=_region_malloc(d,sizeof(*_tmp107));_tmp107->f1=_tmp11C,_tmp107->f2=_tmp11A,_tmp107->f3=x;_tmp107;});_tmp108->hd=_tmp75A;}),_tmp108->tl=*_tmp11D;_tmp108;});*_tmp11D=_tmp75B;});{
# 653
struct _RegionHandle _tmp109=_new_region("r");struct _RegionHandle*r=& _tmp109;_push_region(r);
{struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11B,_tmp11A);
for(0;_tmp119 != 0;_tmp119=_tmp119->tl){
struct Cyc_Absyn_Aggrfield*_tmp10B=(struct Cyc_Absyn_Aggrfield*)_tmp119->hd;
void*t=_tmp10B->type;
struct Cyc_List_List*atts=_tmp10B->attributes;
# 663
if((_tmp119->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t))) && !
Cyc_Tcutil_is_array_type(t))
({struct Cyc_List_List*_tmp75D=({struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*_tmp10D));({void*_tmp75C=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->tag=6U,_tmp10C->f1=0;_tmp10C;});_tmp10D->hd=_tmp75C;}),_tmp10D->tl=atts;_tmp10D;});atts=_tmp75D;});
({void*_tmp75E=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10A,t));t=_tmp75E;});
# 670
if(Cyc_Tcutil_unify(t,Cyc_Absyn_void_type))
({void*_tmp762=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=4U,({void*_tmp761=Cyc_Absyn_void_star_type();(_tmp10E->f1).elt_type=_tmp761;}),({struct Cyc_Absyn_Tqual _tmp760=Cyc_Absyn_empty_tqual(0U);(_tmp10E->f1).tq=_tmp760;}),({
struct Cyc_Absyn_Exp*_tmp75F=Cyc_Absyn_uint_exp(0U,0U);(_tmp10E->f1).num_elts=_tmp75F;}),(_tmp10E->f1).zero_term=Cyc_Absyn_false_type,(_tmp10E->f1).zt_loc=0U;_tmp10E;});
# 671
t=_tmp762;});
# 674
({struct Cyc_List_List*_tmp764=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Absyn_Aggrfield*_tmp763=({struct Cyc_Absyn_Aggrfield*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->name=_tmp10B->name,_tmp10F->tq=Cyc_Toc_mt_tq,_tmp10F->type=t,_tmp10F->width=_tmp10B->width,_tmp10F->attributes=atts,_tmp10F->requires_clause=0;_tmp10F;});_tmp110->hd=_tmp763;}),_tmp110->tl=_tmp106;_tmp110;});_tmp106=_tmp764;});}
# 676
({struct Cyc_List_List*_tmp765=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp106);_tmp106=_tmp765;});{
struct Cyc_Absyn_Aggrdecl*_tmp111=Cyc_Toc_make_c_struct_defn(xname,_tmp106);
({struct Cyc_List_List*_tmp767=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Decl*_tmp766=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=5U,_tmp112->f1=_tmp111;_tmp112;}),0U);_tmp113->hd=_tmp766;}),_tmp113->tl=Cyc_Toc_result_decls;_tmp113;});Cyc_Toc_result_decls=_tmp767;});{
void*_tmp114=x;_npop_handler(0U);return _tmp114;};};}
# 654
;_pop_region(r);};};}
# 682
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 686
struct _tuple26 env=({struct _tuple26 _tmp6FF;_tmp6FF.f1=struct_name,_tmp6FF.f2=type_vars,_tmp6FF.f3=type_args,_tmp6FF.f4=fields;_tmp6FF;});
return((void*(*)(struct _tuple26*arg,void*(*f)(struct _RegionHandle*,struct _tuple27*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 694
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp76A=({struct _dyneither_ptr*_tmp121=_cycalloc(sizeof(*_tmp121));({struct _dyneither_ptr _tmp769=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp120=({struct Cyc_Int_pa_PrintArg_struct _tmp700;_tmp700.tag=1U,_tmp700.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp700;});void*_tmp11E[1U];_tmp11E[0]=& _tmp120;({struct _dyneither_ptr _tmp768=({const char*_tmp11F="_tmp%X";_tag_dyneither(_tmp11F,sizeof(char),7U);});Cyc_aprintf(_tmp768,_tag_dyneither(_tmp11E,sizeof(void*),1U));});});*_tmp121=_tmp769;});_tmp121;});_tmp122->f2=_tmp76A;});_tmp122;});}struct _tuple29{struct Cyc_Toc_TocState*f1;int f2;};
# 700
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple29*env){
struct _tuple29 _tmp123=*env;struct _tuple29 _tmp124=_tmp123;struct Cyc_Xarray_Xarray*_tmp12C;_LL1: _tmp12C=(_tmp124.f1)->temp_labels;_LL2:;{
int _tmp125=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp76B=_tmp125;_tmp76B < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12C);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12C,_tmp125);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12B=_cycalloc(sizeof(*_tmp12B));({struct _dyneither_ptr _tmp76D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12A=({struct Cyc_Int_pa_PrintArg_struct _tmp701;_tmp701.tag=1U,_tmp701.f1=(unsigned int)_tmp125;_tmp701;});void*_tmp128[1U];_tmp128[0]=& _tmp12A;({struct _dyneither_ptr _tmp76C=({const char*_tmp129="_LL%X";_tag_dyneither(_tmp129,sizeof(char),6U);});Cyc_aprintf(_tmp76C,_tag_dyneither(_tmp128,sizeof(void*),1U));});});*_tmp12B=_tmp76D;});_tmp12B;});
if(({int _tmp76E=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12C,res);_tmp76E != _tmp125;}))
({void*_tmp126=0U;({struct _dyneither_ptr _tmp76F=({const char*_tmp127="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp127,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp76F,_tag_dyneither(_tmp126,sizeof(void*),0U));});});
return res;};};}
# 711
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple29*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 717
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp12D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp12D))->hd)->name)!= 0){
++ ans;
_tmp12D=_tmp12D->tl;}
# 724
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 730
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp12E=*a;struct _tuple9 _tmp12F=_tmp12E;struct _dyneither_ptr*_tmp133;struct Cyc_Absyn_Tqual _tmp132;void*_tmp131;_LL1: _tmp133=_tmp12F.f1;_tmp132=_tmp12F.f2;_tmp131=_tmp12F.f3;_LL2:;
return({struct _tuple9*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->f1=_tmp133,_tmp130->f2=_tmp132,({void*_tmp770=Cyc_Toc_typ_to_c(_tmp131);_tmp130->f3=_tmp770;});_tmp130;});}
# 753 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp134=Cyc_Tcutil_compress(t);void*_tmp135=_tmp134;void*_tmp13B;void*_tmp13A;struct Cyc_Absyn_Tqual _tmp139;struct Cyc_Absyn_Exp*_tmp138;void*_tmp137;unsigned int _tmp136;switch(*((int*)_tmp135)){case 4U: _LL1: _tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).elt_type;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).tq;_tmp138=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).num_elts;_tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zero_term;_tmp136=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zt_loc;_LL2:
# 756
 return({void*_tmp771=Cyc_Toc_typ_to_c_array(_tmp13A);Cyc_Absyn_cstar_type(_tmp771,_tmp139);});case 1U: _LL3: _tmp13B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp135)->f2;if(_tmp13B != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13B);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 762
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 764
struct Cyc_Absyn_Aggrfield*_tmp13C=({struct Cyc_Absyn_Aggrfield*_tmp13D=_cycalloc(sizeof(*_tmp13D));*_tmp13D=*f;_tmp13D;});
_tmp13C->type=new_type;
_tmp13C->requires_clause=0;
_tmp13C->tq=Cyc_Toc_mt_tq;
return _tmp13C;}
# 771
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 773
return;}
# 776
static void*Cyc_Toc_char_star_type(){
static void**cs=0;
if(cs == 0)
({void**_tmp773=({void**_tmp13E=_cycalloc(sizeof(*_tmp13E));({void*_tmp772=Cyc_Absyn_star_type(Cyc_Absyn_char_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type);*_tmp13E=_tmp772;});_tmp13E;});cs=_tmp773;});
return*cs;}
# 782
static void*Cyc_Toc_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp776=({void**_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp775=({void*_tmp774=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp774,Cyc_Toc_mt_tq);});*_tmp13F=_tmp775;});_tmp13F;});r=_tmp776;});
return*r;}
# 788
static void*Cyc_Toc_dyn_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp779=({void**_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp778=({void*_tmp777=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_type(_tmp777,Cyc_Toc_mt_tq);});*_tmp140=_tmp778;});_tmp140;});r=_tmp779;});
return*r;}
# 794
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp141=Cyc_Tcutil_compress(t);void*_tmp142=_tmp141;struct Cyc_Absyn_Tvar*_tmp143;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->tag == 2U){_LL1: _tmp143=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 800
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp144=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp145=_tmp144;if(((struct Cyc_Absyn_Kind*)_tmp145)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 807
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp146=t;struct Cyc_Absyn_Datatypedecl*_tmp18D;void**_tmp18C;struct Cyc_Absyn_Enumdecl*_tmp18B;struct Cyc_Absyn_Aggrdecl*_tmp18A;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp188;struct _tuple1*_tmp187;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Typedefdecl*_tmp185;void*_tmp184;enum Cyc_Absyn_AggrKind _tmp183;struct Cyc_List_List*_tmp182;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Tqual _tmp180;void*_tmp17F;struct Cyc_List_List*_tmp17E;int _tmp17D;struct Cyc_Absyn_VarargInfo*_tmp17C;struct Cyc_List_List*_tmp17B;void*_tmp17A;struct Cyc_Absyn_Tqual _tmp179;struct Cyc_Absyn_Exp*_tmp178;unsigned int _tmp177;void*_tmp176;struct Cyc_Absyn_Tqual _tmp175;void*_tmp174;struct Cyc_Absyn_Tvar*_tmp173;void**_tmp172;struct Cyc_List_List*_tmp171;struct _tuple1*_tmp170;union Cyc_Absyn_AggrInfo _tmp16F;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Datatypedecl*_tmp16D;struct Cyc_Absyn_Datatypefield*_tmp16C;switch(*((int*)_tmp146)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 822
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp16D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).val).f1;_tmp16C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).val).f2;_LLA:
# 824
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp16C->name,_tmp16D->name));}else{_LLB: _LLC:
# 826
({void*_tmp147=0U;({struct _dyneither_ptr _tmp77A=({const char*_tmp148="unresolved DatatypeFieldType";_tag_dyneither(_tmp148,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp77A,_tag_dyneither(_tmp147,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 835
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp16F=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_tmp16E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f2;_LL1C:
# 882
{union Cyc_Absyn_AggrInfo _tmp158=_tmp16F;if((_tmp158.UnknownAggr).tag == 1){_LL45: _LL46:
 return t;}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 886
struct Cyc_Absyn_Aggrdecl*_tmp159=Cyc_Absyn_get_known_aggrdecl(_tmp16F);
if(_tmp159->expected_mem_kind){
# 889
if(_tmp159->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp703;_tmp703.tag=0U,({
struct _dyneither_ptr _tmp77B=(struct _dyneither_ptr)(_tmp159->kind == Cyc_Absyn_UnionA?({const char*_tmp15E="union";_tag_dyneither(_tmp15E,sizeof(char),6U);}):({const char*_tmp15F="struct";_tag_dyneither(_tmp15F,sizeof(char),7U);}));_tmp703.f1=_tmp77B;});_tmp703;});struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp702;_tmp702.tag=0U,({
struct _dyneither_ptr _tmp77C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp702.f1=_tmp77C;});_tmp702;});void*_tmp15A[2U];_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D;({struct _dyneither_ptr _tmp77D=({const char*_tmp15B="%s %s was never defined.";_tag_dyneither(_tmp15B,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp77D,_tag_dyneither(_tmp15A,sizeof(void*),2U));});});}
# 897
if(_tmp159->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp160=_tmp159->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp159->impl))->fields;
if(_tmp160 == 0)return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp160))->tl != 0;_tmp160=_tmp160->tl){;}{
void*_tmp161=((struct Cyc_Absyn_Aggrfield*)_tmp160->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp161))){
if(_tmp159->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp704;_tmp704.tag=0U,({struct _dyneither_ptr _tmp77E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp704.f1=_tmp77E;});_tmp704;});void*_tmp162[1U];_tmp162[0]=& _tmp164;({struct _dyneither_ptr _tmp77F=({const char*_tmp163="struct %s ended up being abstract.";_tag_dyneither(_tmp163,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp77F,_tag_dyneither(_tmp162,sizeof(void*),1U));});});{
# 910
struct _RegionHandle _tmp165=_new_region("r");struct _RegionHandle*r=& _tmp165;_push_region(r);
{struct Cyc_List_List*_tmp166=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp159->tvs,_tmp16E);
void*_tmp167=Cyc_Tcutil_rsubstitute(r,_tmp166,_tmp161);
if(Cyc_Toc_is_abstract_type(_tmp167)){void*_tmp168=Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp168;}{
void*_tmp169=Cyc_Toc_add_struct_type(_tmp159->name,_tmp159->tvs,_tmp16E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp159->impl))->fields);_npop_handler(0U);return _tmp169;};}
# 911
;_pop_region(r);};}
# 916
return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);};};};case 15U: _LL1D: _tmp170=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_LL1E:
 return t;case 16U: _LL1F: _tmp171=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp171);return t;case 4U: _LL23: _LL24:
# 928
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 933
 return Cyc_Absyn_void_star_type();}case 1U: _LL3: _tmp172=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp146)->f2;_LL4:
# 811
 if(*_tmp172 == 0){
*_tmp172=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 815
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp172));case 2U: _LL5: _tmp173=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->f1;_LL6:
# 817
 if((Cyc_Tcutil_tvar_kind(_tmp173,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 819
return Cyc_Absyn_void_type;else{
# 821
return Cyc_Absyn_void_star_type();}case 3U: _LLD: _tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp175=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_tq;_tmp174=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).ptr_atts).bounds;_LLE:
# 830
({void*_tmp780=Cyc_Toc_typ_to_c(_tmp176);_tmp176=_tmp780;});
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp174)== 0)
return Cyc_Toc_dyneither_ptr_type();else{
# 834
return Cyc_Absyn_star_type(_tmp176,Cyc_Absyn_heap_rgn_type,_tmp175,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).tq;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).num_elts;_tmp177=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).zt_loc;_LL14:
# 838
 return({void*_tmp784=Cyc_Toc_typ_to_c(_tmp17A);struct Cyc_Absyn_Tqual _tmp783=_tmp179;struct Cyc_Absyn_Exp*_tmp782=_tmp178;void*_tmp781=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp784,_tmp783,_tmp782,_tmp781,_tmp177);});case 5U: _LL15: _tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_tqual;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_type;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).args;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).c_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).cyc_varargs;_tmp17B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).attributes;_LL16: {
# 844
struct Cyc_List_List*_tmp149=0;
for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
void*_tmp14A=(void*)_tmp17B->hd;void*_tmp14B=_tmp14A;switch(*((int*)_tmp14B)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
({struct Cyc_List_List*_tmp785=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=(void*)_tmp17B->hd,_tmp14C->tl=_tmp149;_tmp14C;});_tmp149=_tmp785;});goto _LL35;}_LL35:;}{
# 855
struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17E);
if(_tmp17C != 0){
# 858
void*_tmp14E=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp17C->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp14F=({struct _tuple9*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->f1=_tmp17C->name,_tmp151->f2=_tmp17C->tq,_tmp151->f3=_tmp14E;_tmp151;});
({struct Cyc_List_List*_tmp787=({struct Cyc_List_List*_tmp786=_tmp14D;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp786,({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->hd=_tmp14F,_tmp150->tl=0;_tmp150;}));});_tmp14D=_tmp787;});}
# 862
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->tag=5U,(_tmp152->f1).tvars=0,(_tmp152->f1).effect=0,(_tmp152->f1).ret_tqual=_tmp180,({void*_tmp788=Cyc_Toc_typ_to_c(_tmp17F);(_tmp152->f1).ret_type=_tmp788;}),(_tmp152->f1).args=_tmp14D,(_tmp152->f1).c_varargs=_tmp17D,(_tmp152->f1).cyc_varargs=0,(_tmp152->f1).rgn_po=0,(_tmp152->f1).attributes=_tmp149,(_tmp152->f1).requires_clause=0,(_tmp152->f1).requires_relns=0,(_tmp152->f1).ensures_clause=0,(_tmp152->f1).ensures_relns=0;_tmp152;});};}case 6U: _LL17: _tmp181=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp146)->f1;_LL18: {
# 867
struct Cyc_List_List*_tmp153=0;
for(0;_tmp181 != 0;_tmp181=_tmp181->tl){
({struct Cyc_List_List*_tmp78B=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));({struct _tuple12*_tmp78A=({struct _tuple12*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->f1=(*((struct _tuple12*)_tmp181->hd)).f1,({void*_tmp789=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp181->hd)).f2);_tmp154->f2=_tmp789;});_tmp154;});_tmp155->hd=_tmp78A;}),_tmp155->tl=_tmp153;_tmp155;});_tmp153=_tmp78B;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp153));}case 7U: _LL19: _tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f1;_tmp182=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f2;_LL1A: {
# 876
struct Cyc_List_List*_tmp156=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp78E=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*_tmp157));({struct Cyc_Absyn_Aggrfield*_tmp78D=({struct Cyc_Absyn_Aggrfield*_tmp78C=(struct Cyc_Absyn_Aggrfield*)_tmp182->hd;Cyc_Toc_aggrfield_to_c(_tmp78C,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp182->hd)->type));});_tmp157->hd=_tmp78D;}),_tmp157->tl=_tmp156;_tmp157;});_tmp156=_tmp78E;});}
return({enum Cyc_Absyn_AggrKind _tmp78F=_tmp183;Cyc_Toc_add_anon_aggr_type(_tmp78F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp156));});}case 8U: _LL21: _tmp187=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f1;_tmp186=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f2;_tmp185=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f3;_tmp184=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f4;_LL22:
# 922
 if(_tmp184 == 0){
if(_tmp186 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=8U,_tmp16A->f1=_tmp187,_tmp16A->f2=0,_tmp16A->f3=_tmp185,_tmp16A->f4=0;_tmp16A;});else{
return t;}}else{
# 927
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=8U,_tmp16B->f1=_tmp187,_tmp16B->f2=0,_tmp16B->f3=_tmp185,({void*_tmp790=Cyc_Toc_typ_to_c(_tmp184);_tmp16B->f4=_tmp790;});_tmp16B;});}case 9U: _LL2B: _tmp188=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp146)->f1;_LL2C:
# 938
 return t;case 11U: _LL2D: _tmp189=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp146)->f1;_LL2E:
# 943
 if(_tmp189->topt != 0)return Cyc_Toc_typ_to_c((void*)_check_null(_tmp189->topt));else{
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)){case 0U: _LL2F: _tmp18A=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_LL30:
# 946
 Cyc_Toc_aggrdecl_to_c(_tmp18A);
if(_tmp18A->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18A->name,Cyc_Absyn_unionq_type);else{
return Cyc_Toc_aggrdecl_type(_tmp18A->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp18B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_LL32:
# 951
 Cyc_Toc_enumdecl_to_c(_tmp18B);
return t;default: _LL33: _tmp18D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_tmp18C=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f2;_LL34:
# 954
 Cyc_Toc_datatypedecl_to_c(_tmp18D);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp18C)));}}_LL0:;}
# 959
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp18E=t;void*_tmp190;struct Cyc_Absyn_Tqual _tmp18F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->tag == 4U){_LL1: _tmp190=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->f1).elt_type;_tmp18F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->f1).tq;_LL2:
# 962
 return({void*_tmp791=Cyc_Absyn_star_type(_tmp190,Cyc_Absyn_heap_rgn_type,_tmp18F,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp791,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 969
static int Cyc_Toc_atomic_type(void*t){
void*_tmp191=Cyc_Tcutil_compress(t);void*_tmp192=_tmp191;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A4;void*_tmp1A3;void*_tmp1A2;struct Cyc_List_List*_tmp1A1;switch(*((int*)_tmp192)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp1A2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1;_tmp1A1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL4: {
# 973
void*_tmp193=_tmp1A2;struct Cyc_Absyn_Datatypedecl*_tmp19D;struct Cyc_Absyn_Datatypefield*_tmp19C;union Cyc_Absyn_AggrInfo _tmp19B;switch(*((int*)_tmp193)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D:
 return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp19B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp193)->f1;_LL25:
# 981
{union Cyc_Absyn_AggrInfo _tmp194=_tmp19B;if((_tmp194.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 985
struct Cyc_Absyn_Aggrdecl*_tmp195=Cyc_Absyn_get_known_aggrdecl(_tmp19B);
if(_tmp195->impl == 0)
return 0;
{struct Cyc_List_List*_tmp196=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp195->impl))->fields;for(0;_tmp196 != 0;_tmp196=_tmp196->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp196->hd)->type))return 0;}}
return 1;};case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp19D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f1;_tmp19C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f2;_LL27:
# 992
{struct Cyc_List_List*_tmp197=_tmp19C->typs;for(0;_tmp197 != 0;_tmp197=_tmp197->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp197->hd)).f2))return 0;}}
return 1;}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp19A=({struct Cyc_String_pa_PrintArg_struct _tmp705;_tmp705.tag=0U,({struct _dyneither_ptr _tmp792=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp705.f1=_tmp792;});_tmp705;});void*_tmp198[1U];_tmp198[0]=& _tmp19A;({struct _dyneither_ptr _tmp793=({const char*_tmp199="atomic_typ:  bad type %s";_tag_dyneither(_tmp199,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp793,_tag_dyneither(_tmp198,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 997
 return 1;case 4U: _LL7: _tmp1A3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192)->f1).elt_type;_LL8:
 return Cyc_Toc_atomic_type(_tmp1A3);case 7U: _LL9: _tmp1A4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp192)->f2;_LLA:
# 1005
 for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp1A4->hd)->type))return 0;}
return 1;case 6U: _LLB: _tmp1A5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp192)->f1;_LLC:
# 1009
 for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp1A5->hd)).f2))return 0;}
return 1;case 3U: _LLD: _LLE:
# 1014
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp706;_tmp706.tag=0U,({struct _dyneither_ptr _tmp794=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp706.f1=_tmp794;});_tmp706;});void*_tmp19E[1U];_tmp19E[0]=& _tmp1A0;({struct _dyneither_ptr _tmp795=({const char*_tmp19F="atomic_typ:  bad type %s";_tag_dyneither(_tmp19F,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp795,_tag_dyneither(_tmp19E,sizeof(void*),1U));});});}_LL0:;}
# 1019
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A6=Cyc_Tcutil_compress(t);void*_tmp1A7=_tmp1A6;void*_tmp1A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A7)->tag == 3U){_LL1: _tmp1A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A7)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_void_type(_tmp1A8);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1026
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1030
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1035
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A9=Cyc_Tcutil_compress(t);void*_tmp1AA=_tmp1A9;struct Cyc_List_List*_tmp1B6;union Cyc_Absyn_AggrInfo _tmp1B5;switch(*((int*)_tmp1AA)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->tag == 20U){_LL1: _tmp1B5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->f1;_LL2: {
# 1038
struct Cyc_Absyn_Aggrdecl*_tmp1AB=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);
if(_tmp1AB->impl == 0)
({void*_tmp1AC=0U;({struct _dyneither_ptr _tmp796=({const char*_tmp1AD="is_poly_field: type missing fields";_tag_dyneither(_tmp1AD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp796,_tag_dyneither(_tmp1AC,sizeof(void*),0U));});});
_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AB->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AA)->f2;_LL4: {
# 1043
struct Cyc_Absyn_Aggrfield*_tmp1AE=Cyc_Absyn_lookup_field(_tmp1B6,f);
if(_tmp1AE == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp707;_tmp707.tag=0U,_tmp707.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp707;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _dyneither_ptr _tmp797=({const char*_tmp1B0="is_poly_field: bad field %s";_tag_dyneither(_tmp1B0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp797,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AE->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp708;_tmp708.tag=0U,({struct _dyneither_ptr _tmp798=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp708.f1=_tmp798;});_tmp708;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp799=({const char*_tmp1B3="is_poly_field: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp799,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL0:;}
# 1054
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B7=e->r;void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct _dyneither_ptr*_tmp1BF;switch(*((int*)_tmp1B8)){case 21U: _LL1: _tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL2:
# 1057
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C0->topt),_tmp1BF) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL4: {
# 1060
void*_tmp1B9=Cyc_Tcutil_compress((void*)_check_null(_tmp1C2->topt));void*_tmp1BA=_tmp1B9;void*_tmp1BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->tag == 3U){_LL8: _tmp1BE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->f1).elt_type;_LL9:
# 1062
 return Cyc_Toc_is_poly_field(_tmp1BE,_tmp1C1) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp709;_tmp709.tag=0U,({struct _dyneither_ptr _tmp79A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C2->topt)));_tmp709.f1=_tmp79A;});_tmp709;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct _dyneither_ptr _tmp79B=({const char*_tmp1BC="is_poly_project: bad type %s";_tag_dyneither(_tmp1BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79B,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1065
 return 0;}_LL0:;}
# 1070
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp79C=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp79C,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=s,_tmp1C3->tl=0;_tmp1C3;}),0U);});}
# 1074
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp79D=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp79D,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1078
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
return Cyc_Toc_atomic_type(t)?Cyc_Toc_malloc_atomic(s): Cyc_Toc_malloc_ptr(s);}
# 1082
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp79E=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp79E,({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1086
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp79F=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp79F,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1090
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*allocator=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp7A0=allocator;Cyc_Absyn_fncall_exp(_tmp7A0,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=s,_tmp1C7[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1095
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp7A1=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp7A1,({struct Cyc_Absyn_Exp*_tmp1C8[3U];_tmp1C8[0]=rgn,_tmp1C8[1]=s,_tmp1C8[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1099
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp7A2=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp7A2,({struct Cyc_List_List*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->hd=e,_tmp1C9->tl=0;_tmp1C9;}),0U);});}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp7A3=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp7A3,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=e,_tmp1CA->tl=0;_tmp1CA;}),0U);});}
# 1106
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp7A4=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp7A4,({void*_tmp1CB=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CB,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1111
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1119
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1CC=e->r;void*_tmp1CD=_tmp1CC;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CD)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CD)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1124
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp7A6=({struct Cyc_List_List*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));({struct Cyc_Absyn_Decl*_tmp7A5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->tag=0U,_tmp1CE->f1=vd;_tmp1CE;}),0U);_tmp1CF->hd=_tmp7A5;}),_tmp1CF->tl=Cyc_Toc_result_decls;_tmp1CF;});Cyc_Toc_result_decls=_tmp7A6;});
({struct Cyc_Absyn_Exp*_tmp7A7=Cyc_Absyn_var_exp(x,0U);xexp=_tmp7A7;});
({struct Cyc_Absyn_Exp*_tmp7A8=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp7A8;});
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1132
({struct Cyc_Absyn_Exp*_tmp7AA=({void*_tmp7A9=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp7A9,e);});xexp=_tmp7AA;});
# 1134
({struct Cyc_Absyn_Exp*_tmp7AC=({void*_tmp7AB=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp7AB,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp7AC;});
goto _LL0;}_LL0:;}
# 1137
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1D0[3U];({struct _tuple19*_tmp7AF=({struct _tuple19*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->f1=0,_tmp1D1->f2=xexp;_tmp1D1;});_tmp1D0[0]=_tmp7AF;}),({
struct _tuple19*_tmp7AE=({struct _tuple19*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xexp;_tmp1D2;});_tmp1D0[1]=_tmp7AE;}),({
struct _tuple19*_tmp7AD=({struct _tuple19*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xplussz;_tmp1D3;});_tmp1D0[2]=_tmp7AD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D0,sizeof(struct _tuple19*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1172 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1D5=_region_malloc(r,sizeof(*_tmp1D5));_tmp1D5->break_lab=0,_tmp1D5->continue_lab=0,_tmp1D5->fallthru_info=0,_tmp1D5->toplevel=1,({int*_tmp7B0=({int*_tmp1D4=_region_malloc(r,sizeof(*_tmp1D4));*_tmp1D4=0;_tmp1D4;});_tmp1D5->in_lhs=_tmp7B0;}),_tmp1D5->rgn=r;_tmp1D5;});}
# 1176
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D6=nv;int _tmp1D7;_LL1: _tmp1D7=_tmp1D6->toplevel;_LL2:;
return _tmp1D7;}
# 1180
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D8=e;struct _dyneither_ptr**_tmp1DE;struct _dyneither_ptr**_tmp1DD;struct Cyc_Toc_FallthruInfo*_tmp1DC;int _tmp1DB;int*_tmp1DA;_LL1: _tmp1DE=_tmp1D8->break_lab;_tmp1DD=_tmp1D8->continue_lab;_tmp1DC=_tmp1D8->fallthru_info;_tmp1DB=_tmp1D8->toplevel;_tmp1DA=_tmp1D8->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));_tmp1D9->break_lab=_tmp1DE,_tmp1D9->continue_lab=_tmp1DD,_tmp1D9->fallthru_info=_tmp1DC,_tmp1D9->toplevel=0,_tmp1D9->in_lhs=_tmp1DA,_tmp1D9->rgn=r;_tmp1D9;});}
# 1184
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DF=e;struct _dyneither_ptr**_tmp1E5;struct _dyneither_ptr**_tmp1E4;struct Cyc_Toc_FallthruInfo*_tmp1E3;int _tmp1E2;int*_tmp1E1;_LL1: _tmp1E5=_tmp1DF->break_lab;_tmp1E4=_tmp1DF->continue_lab;_tmp1E3=_tmp1DF->fallthru_info;_tmp1E2=_tmp1DF->toplevel;_tmp1E1=_tmp1DF->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));_tmp1E0->break_lab=_tmp1E5,_tmp1E0->continue_lab=_tmp1E4,_tmp1E0->fallthru_info=_tmp1E3,_tmp1E0->toplevel=1,_tmp1E0->in_lhs=_tmp1E1,_tmp1E0->rgn=r;_tmp1E0;});}
# 1188
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1E6=nv;int*_tmp1E7;_LL1: _tmp1E7=_tmp1E6->in_lhs;_LL2:;
return*_tmp1E7;}
# 1192
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E8=e;int*_tmp1E9;_LL1: _tmp1E9=_tmp1E8->in_lhs;_LL2:;
*_tmp1E9=b;}
# 1197
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EA=e;struct _dyneither_ptr**_tmp1F0;struct _dyneither_ptr**_tmp1EF;struct Cyc_Toc_FallthruInfo*_tmp1EE;int _tmp1ED;int*_tmp1EC;_LL1: _tmp1F0=_tmp1EA->break_lab;_tmp1EF=_tmp1EA->continue_lab;_tmp1EE=_tmp1EA->fallthru_info;_tmp1ED=_tmp1EA->toplevel;_tmp1EC=_tmp1EA->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1EB=_region_malloc(r,sizeof(*_tmp1EB));_tmp1EB->break_lab=_tmp1F0,_tmp1EB->continue_lab=_tmp1EF,_tmp1EB->fallthru_info=_tmp1EE,_tmp1EB->toplevel=_tmp1ED,_tmp1EB->in_lhs=_tmp1EC,_tmp1EB->rgn=r;_tmp1EB;});}
# 1204
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F1=e;struct _dyneither_ptr**_tmp1F7;struct _dyneither_ptr**_tmp1F6;struct Cyc_Toc_FallthruInfo*_tmp1F5;int _tmp1F4;int*_tmp1F3;_LL1: _tmp1F7=_tmp1F1->break_lab;_tmp1F6=_tmp1F1->continue_lab;_tmp1F5=_tmp1F1->fallthru_info;_tmp1F4=_tmp1F1->toplevel;_tmp1F3=_tmp1F1->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1F2=_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2->break_lab=0,_tmp1F2->continue_lab=0,_tmp1F2->fallthru_info=_tmp1F5,_tmp1F2->toplevel=_tmp1F4,_tmp1F2->in_lhs=_tmp1F3,_tmp1F2->rgn=r;_tmp1F2;});}
# 1210
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1214
struct Cyc_Toc_Env*_tmp1F8=e;struct _dyneither_ptr**_tmp200;struct _dyneither_ptr**_tmp1FF;struct Cyc_Toc_FallthruInfo*_tmp1FE;int _tmp1FD;int*_tmp1FC;_LL1: _tmp200=_tmp1F8->break_lab;_tmp1FF=_tmp1F8->continue_lab;_tmp1FE=_tmp1F8->fallthru_info;_tmp1FD=_tmp1F8->toplevel;_tmp1FC=_tmp1F8->in_lhs;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));
_tmp1FB->label=fallthru_l,_tmp1FB->binders=fallthru_binders;_tmp1FB;});
return({struct Cyc_Toc_Env*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));({struct _dyneither_ptr**_tmp7B1=({struct _dyneither_ptr**_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));*_tmp1F9=break_l;_tmp1F9;});_tmp1FA->break_lab=_tmp7B1;}),_tmp1FA->continue_lab=_tmp1FF,_tmp1FA->fallthru_info=fi,_tmp1FA->toplevel=_tmp1FD,_tmp1FA->in_lhs=_tmp1FC,_tmp1FA->rgn=r;_tmp1FA;});};}
# 1219
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1221
struct Cyc_Toc_Env*_tmp201=e;struct _dyneither_ptr**_tmp208;struct _dyneither_ptr**_tmp207;struct Cyc_Toc_FallthruInfo*_tmp206;int _tmp205;int*_tmp204;_LL1: _tmp208=_tmp201->break_lab;_tmp207=_tmp201->continue_lab;_tmp206=_tmp201->fallthru_info;_tmp205=_tmp201->toplevel;_tmp204=_tmp201->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp203=_region_malloc(r,sizeof(*_tmp203));({struct _dyneither_ptr**_tmp7B2=({struct _dyneither_ptr**_tmp202=_region_malloc(r,sizeof(*_tmp202));*_tmp202=break_l;_tmp202;});_tmp203->break_lab=_tmp7B2;}),_tmp203->continue_lab=_tmp207,_tmp203->fallthru_info=0,_tmp203->toplevel=_tmp205,_tmp203->in_lhs=_tmp204,_tmp203->rgn=r;_tmp203;});}
# 1228
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1231
struct Cyc_Toc_Env*_tmp209=e;struct _dyneither_ptr**_tmp210;struct _dyneither_ptr**_tmp20F;struct Cyc_Toc_FallthruInfo*_tmp20E;int _tmp20D;int*_tmp20C;_LL1: _tmp210=_tmp209->break_lab;_tmp20F=_tmp209->continue_lab;_tmp20E=_tmp209->fallthru_info;_tmp20D=_tmp209->toplevel;_tmp20C=_tmp209->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));_tmp20B->break_lab=0,_tmp20B->continue_lab=_tmp20F,({struct Cyc_Toc_FallthruInfo*_tmp7B3=({struct Cyc_Toc_FallthruInfo*_tmp20A=_region_malloc(r,sizeof(*_tmp20A));_tmp20A->label=next_l,_tmp20A->binders=0;_tmp20A;});_tmp20B->fallthru_info=_tmp7B3;}),_tmp20B->toplevel=_tmp20D,_tmp20B->in_lhs=_tmp20C,_tmp20B->rgn=r;_tmp20B;});}
# 1244 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1247
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp211=e->annot;void*_tmp212=_tmp211;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp212)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({void*_tmp213=0U;({unsigned int _tmp7B5=e->loc;struct _dyneither_ptr _tmp7B4=({const char*_tmp214="Toc: do_null_check";_tag_dyneither(_tmp214,sizeof(char),19U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp7B5,_tmp7B4,_tag_dyneither(_tmp213,sizeof(void*),0U));});});}}}}}}_LL0:;}
# 1263
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1265
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
# 1276
{void*_tmp219=annot;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Exp*_tmp225;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1278
 if(!((unsigned int)({void*_tmp7B6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7B6,_tmp22A);})))
({void*_tmp7BA=({void*_tmp7B9=({void*_tmp7B7=Cyc_Toc_typ_to_c(_tmp22C);Cyc_Absyn_cstar_type(_tmp7B7,_tmp22B);});Cyc_Toc_cast_it_r(_tmp7B9,({
struct Cyc_Absyn_Exp*_tmp7B8=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7B8,Cyc_Toc_curr_sp,0U);}));});
# 1279
ptr->r=_tmp7BA;});
# 1281
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1284
 if(!((unsigned int)({void*_tmp7BB=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7BB,_tmp22A);})))
goto _LLB;
# 1287
({void*_tmp7BF=({void*_tmp7BE=_tmp217;Cyc_Toc_cast_it_r(_tmp7BE,({
struct Cyc_Absyn_Exp*_tmp7BD=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp7BD,({struct Cyc_Absyn_Exp*_tmp21A[1U];({struct Cyc_Absyn_Exp*_tmp7BC=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21A[0]=_tmp7BC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21A,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 1287
ptr->r=_tmp7BF;});
# 1289
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1294
void*ta1=Cyc_Toc_typ_to_c(_tmp22C);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp7C4=({void*_tmp7C3=Cyc_Absyn_cstar_type(ta1,_tmp22B);Cyc_Toc_cast_it_r(_tmp7C3,({
struct Cyc_Absyn_Exp*_tmp7C2=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp7C2,({struct Cyc_Absyn_Exp*_tmp21B[3U];({
struct Cyc_Absyn_Exp*_tmp7C1=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21B[0]=_tmp7C1;}),({
struct Cyc_Absyn_Exp*_tmp7C0=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp21B[1]=_tmp7C0;}),_tmp21B[2]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21B,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 1296
ptr->r=_tmp7C4;});
# 1301
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp225=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->f1;_LLF:
# 1304
 fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp226=_tmp225;goto _LL11;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp226=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->f1;_LL11: {
# 1307
void*ta1=Cyc_Toc_typ_to_c(_tmp22C);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1312
struct _tuple13 _tmp21C=Cyc_Evexp_eval_const_uint_exp(_tmp226);struct _tuple13 _tmp21D=_tmp21C;unsigned int _tmp222;int _tmp221;_LL15: _tmp222=_tmp21D.f1;_tmp221=_tmp21D.f2;_LL16:;
if((!_tmp221  || _tmp222 != 1) || !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1315
({void*_tmp7C9=({void*_tmp7C8=Cyc_Absyn_cstar_type(ta1,_tmp22B);Cyc_Toc_cast_it_r(_tmp7C8,({
struct Cyc_Absyn_Exp*_tmp7C7=fn_e;Cyc_Absyn_fncall_exp(_tmp7C7,({struct Cyc_Absyn_Exp*_tmp21E[4U];({struct Cyc_Absyn_Exp*_tmp7C6=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21E[0]=_tmp7C6;}),_tmp21E[1]=_tmp226,({
struct Cyc_Absyn_Exp*_tmp7C5=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp21E[2]=_tmp7C5;}),_tmp21E[3]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21E,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 1315
ptr->r=_tmp7C9;});
# 1319
ans=1;}else{
# 1322
if(Cyc_Toc_is_zero(_tmp226)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1325
({void*_tmp7CD=({void*_tmp7CC=_tmp217;Cyc_Toc_cast_it_r(_tmp7CC,({
struct Cyc_Absyn_Exp*_tmp7CB=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp7CB,({struct Cyc_Absyn_Exp*_tmp21F[1U];({struct Cyc_Absyn_Exp*_tmp7CA=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp21F[0]=_tmp7CA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21F,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 1325
ptr->r=_tmp7CD;});
# 1328
ans=0;}else{
# 1332
({struct Cyc_Absyn_Exp*_tmp7CE=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);fn_e=_tmp7CE;});
({void*_tmp7D2=({void*_tmp7D1=Cyc_Absyn_cstar_type(ta1,_tmp22B);Cyc_Toc_cast_it_r(_tmp7D1,({
struct Cyc_Absyn_Exp*_tmp7D0=fn_e;Cyc_Absyn_fncall_exp(_tmp7D0,({struct Cyc_Absyn_Exp*_tmp220[3U];({struct Cyc_Absyn_Exp*_tmp7CF=Cyc_Absyn_new_exp(ptr->r,0U);_tmp220[0]=_tmp7CF;}),_tmp220[1]=_tmp226,_tmp220[2]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp220,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 1333
ptr->r=_tmp7D2;});
# 1336
ans=1;}}
# 1339
goto _LL5;}}else{_LL12: _LL13:
({void*_tmp223=0U;({struct _dyneither_ptr _tmp7D3=({const char*_tmp224="FIX: ptr_use_to_c, bad annotation";_tag_dyneither(_tmp224,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7D3,_tag_dyneither(_tmp223,sizeof(void*),0U));});});}}}}}}_LL5:;}
# 1342
Cyc_Toc_set_lhs(nv,_tmp215);
return ans;}else{_LL3: _LL4:
({void*_tmp227=0U;({struct _dyneither_ptr _tmp7D4=({const char*_tmp228="ptr_use_to_c: non-pointer-type";_tag_dyneither(_tmp228,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7D4,_tag_dyneither(_tmp227,sizeof(void*),0U));});});}_LL0:;};}
# 1348
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp22D=0U;({struct _dyneither_ptr _tmp7D5=({const char*_tmp22E="Missing type in primop ";_tag_dyneither(_tmp22E,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7D5,_tag_dyneither(_tmp22D,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1352
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F->f1=Cyc_Toc_mt_tq,({void*_tmp7D6=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp22F->f2=_tmp7D6;});_tmp22F;});}
# 1357
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp230=e->r;void*_tmp231=_tmp230;struct Cyc_Absyn_Exp*_tmp239;int _tmp238;struct Cyc_Absyn_Exp*_tmp237;int _tmp236;struct Cyc_List_List*_tmp235;switch(*((int*)_tmp231)){case 26U: _LL1: _tmp235=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp231)->f1;_LL2:
# 1360
{struct Cyc_List_List*_tmp232=_tmp235;for(0;_tmp232 != 0;_tmp232=_tmp232->tl){
if((*((struct _tuple19*)_tmp232->hd)).f1 != 0)
({void*_tmp233=0U;({struct _dyneither_ptr _tmp7D7=({const char*_tmp234="array designators for abstract-field initialization";_tag_dyneither(_tmp234,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp7D7,_tag_dyneither(_tmp233,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp7D8=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp235),0U);_tmp237=_tmp7D8;});_tmp236=0;goto _LL4;case 27U: _LL3: _tmp237=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp231)->f2;_tmp236=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp231)->f4;_LL4:
 _tmp239=_tmp237;_tmp238=_tmp236;goto _LL6;case 28U: _LL5: _tmp239=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp231)->f1;_tmp238=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp231)->f3;_LL6:
# 1366
({struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Absyn_copy_exp(_tmp239);_tmp239=_tmp7D9;});
return _tmp238?({struct Cyc_Absyn_Exp*_tmp7DA=_tmp239;Cyc_Absyn_add_exp(_tmp7DA,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp239;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1375
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1383
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp23A=e->r;void*_tmp23B=_tmp23A;struct Cyc_List_List*_tmp23C;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23B)->tag == 29U){_LL1: _tmp23C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23B)->f3;_LL2:
 dles=_tmp23C;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1389
{void*_tmp23D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp23E=_tmp23D;struct Cyc_Absyn_Aggrdecl*_tmp23F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23E)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23E)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23E)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp23F=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23E)->f1)->f1).KnownAggr).val;_LL7:
# 1391
 if(_tmp23F->impl == 0)
return 0;
if(_tmp23F->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23F->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1399
 return 0;}_LL5:;}
# 1401
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
# 1416
 if(_tmp248 == 0  || !Cyc_Toc_is_zero(_tmp248))
return 0;
# 1422
return({struct Cyc_Absyn_Exp*_tmp7DD=({struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp7DC,({struct Cyc_Absyn_Exp*_tmp247[2U];_tmp247[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp24A)),({
# 1424
struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Absyn_sizeoftype_exp(_tmp249,0U);_tmp247[1]=_tmp7DB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp247,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1422
Cyc_Absyn_add_exp(_tmp7DD,
# 1425
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1430
({void*_tmp24C=0U;({struct _dyneither_ptr _tmp7DE=({const char*_tmp24D="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp24D,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7DE,_tag_dyneither(_tmp24C,sizeof(void*),0U));});});};}
# 1433
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp24E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp24E != 0;_tmp24E=_tmp24E->tl){
struct Cyc_Absyn_Aggrfield*_tmp24F=(struct Cyc_Absyn_Aggrfield*)_tmp24E->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp24F->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1440
({void*_tmp250=0U;({struct _dyneither_ptr _tmp7E0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp253=({struct Cyc_String_pa_PrintArg_struct _tmp70A;_tmp70A.tag=0U,_tmp70A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp70A;});void*_tmp251[1U];_tmp251[0]=& _tmp253;({struct _dyneither_ptr _tmp7DF=({const char*_tmp252="get_member_offset %s failed";_tag_dyneither(_tmp252,sizeof(char),28U);});Cyc_aprintf(_tmp7DF,_tag_dyneither(_tmp251,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7E0,_tag_dyneither(_tmp250,sizeof(void*),0U));});});}struct _tuple30{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1444
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple30*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1447
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1450
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254->tag=5U,_tmp254->f1=e1,_tmp254->f2=incr;_tmp254;}),0U);}
# 1454
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1463
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1465
void*_tmp255=e1->r;void*_tmp256=_tmp255;struct Cyc_Absyn_Exp*_tmp25E;struct _dyneither_ptr*_tmp25D;int _tmp25C;int _tmp25B;void*_tmp25A;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_Absyn_Stmt*_tmp258;switch(*((int*)_tmp256)){case 37U: _LL1: _tmp258=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp258,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp25A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp259=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp259,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp25E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp25D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_tmp25C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f3;_tmp25B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256)->f4;_LL6:
# 1470
 e1->r=_tmp25E->r;
({struct Cyc_Absyn_Exp*_tmp7E3=e1;struct Cyc_List_List*_tmp7E2=({struct Cyc_List_List*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->hd=_tmp25D,_tmp257->tl=fs;_tmp257;});struct Cyc_Absyn_Exp*(*_tmp7E1)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp7E3,_tmp7E2,_tmp7E1,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1478
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1480
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp7E4;});}
({void*_tmp7E5=(f(e1_copy,f_env))->r;e1->r=_tmp7E5;});
goto _LL0;}}_LL0:;}
# 1486
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1488
void*_tmp25F=s->r;void*_tmp260=_tmp25F;struct Cyc_Absyn_Stmt*_tmp267;struct Cyc_Absyn_Decl*_tmp266;struct Cyc_Absyn_Stmt*_tmp265;struct Cyc_Absyn_Exp*_tmp264;switch(*((int*)_tmp260)){case 1U: _LL1: _tmp264=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp260)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp264,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp266=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp260)->f1;_tmp265=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp260)->f2;_LL4:
# 1491
 Cyc_Toc_lvalue_assign_stmt(_tmp265,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp267=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp260)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp267,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp263=({struct Cyc_String_pa_PrintArg_struct _tmp70B;_tmp70B.tag=0U,({struct _dyneither_ptr _tmp7E6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp70B.f1=_tmp7E6;});_tmp70B;});void*_tmp261[1U];_tmp261[0]=& _tmp263;({struct _dyneither_ptr _tmp7E7=({const char*_tmp262="lvalue_assign_stmt: %s";_tag_dyneither(_tmp262,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp7E7,_tag_dyneither(_tmp261,sizeof(void*),1U));});});}_LL0:;}
# 1497
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1501
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp268=e->r;void*_tmp269=_tmp268;struct Cyc_Absyn_Stmt*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;void**_tmp26E;struct Cyc_Absyn_Exp**_tmp26D;switch(*((int*)_tmp269)){case 14U: _LL1: _tmp26E=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_tmp26D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp269)->f2;_LL2:
# 1504
({struct Cyc_Absyn_Exp*_tmp7E8=Cyc_Toc_push_address_exp(*_tmp26D);*_tmp26D=_tmp7E8;});
({void*_tmp7E9=Cyc_Absyn_cstar_type(*_tmp26E,Cyc_Toc_mt_tq);*_tmp26E=_tmp7E9;});
return e;case 20U: _LL3: _tmp26F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_LL4:
# 1508
 return _tmp26F;case 37U: _LL5: _tmp270=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_LL6:
# 1512
 Cyc_Toc_push_address_stmt(_tmp270);
return e;default: _LL7: _LL8:
# 1515
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp26C=({struct Cyc_String_pa_PrintArg_struct _tmp70C;_tmp70C.tag=0U,({struct _dyneither_ptr _tmp7EA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp70C.f1=_tmp7EA;});_tmp70C;});void*_tmp26A[1U];_tmp26A[0]=& _tmp26C;({struct _dyneither_ptr _tmp7EB=({const char*_tmp26B="can't take & of exp %s";_tag_dyneither(_tmp26B,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7EB,_tag_dyneither(_tmp26A,sizeof(void*),1U));});});}_LL0:;}
# 1519
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp271=s->r;void*_tmp272=_tmp271;struct Cyc_Absyn_Exp**_tmp278;struct Cyc_Absyn_Stmt*_tmp277;struct Cyc_Absyn_Stmt*_tmp276;switch(*((int*)_tmp272)){case 2U: _LL1: _tmp276=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp272)->f2;_LL2:
 _tmp277=_tmp276;goto _LL4;case 12U: _LL3: _tmp277=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp272)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp277);goto _LL0;case 1U: _LL5: _tmp278=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp272)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp7EC=Cyc_Toc_push_address_exp(*_tmp278);*_tmp278=_tmp7EC;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp275=({struct Cyc_String_pa_PrintArg_struct _tmp70D;_tmp70D.tag=0U,({struct _dyneither_ptr _tmp7ED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp70D.f1=_tmp7ED;});_tmp70D;});void*_tmp273[1U];_tmp273[0]=& _tmp275;({struct _dyneither_ptr _tmp7EE=({const char*_tmp274="can't take & of stmt %s";_tag_dyneither(_tmp274,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7EE,_tag_dyneither(_tmp273,sizeof(void*),1U));});});}_LL0:;}
# 1531
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp279=Cyc_Tcutil_compress(t);void*_tmp27A=_tmp279;void*_tmp28B;switch(*((int*)_tmp27A)){case 3U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp7EF=Cyc_Absyn_null_exp(0U);res=_tmp7EF;});goto _LL0;case 0U: _LL3: _tmp28B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f1;_LL4:
# 1536
{void*_tmp27B=_tmp28B;int _tmp287;enum Cyc_Absyn_Sign _tmp286;enum Cyc_Absyn_Sign _tmp285;enum Cyc_Absyn_Sign _tmp284;enum Cyc_Absyn_Sign _tmp283;enum Cyc_Absyn_Sign _tmp282;switch(*((int*)_tmp27B)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f2){case Cyc_Absyn_Char_sz: _LL8: _tmp282=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL9:
({struct Cyc_Absyn_Exp*_tmp7F0=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp282,'\000'),0U);res=_tmp7F0;});goto _LL7;case Cyc_Absyn_Short_sz: _LLA: _tmp283=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LLB:
({struct Cyc_Absyn_Exp*_tmp7F1=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp283,0),0U);res=_tmp7F1;});goto _LL7;case Cyc_Absyn_Int_sz: _LL10: _tmp284=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL11:
# 1541
 _tmp285=_tmp284;goto _LL13;case Cyc_Absyn_Long_sz: _LL12: _tmp285=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL13:
({struct Cyc_Absyn_Exp*_tmp7F2=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp285,0),0U);res=_tmp7F2;});goto _LL7;default: _LL14: _tmp286=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL15:
({struct Cyc_Absyn_Exp*_tmp7F3=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp286,(long long)0),0U);res=_tmp7F3;});goto _LL7;}case 15U: _LLC: _LLD:
# 1539
 goto _LLF;case 16U: _LLE: _LLF:
 _tmp284=1U;goto _LL11;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27B)->f1){case 0U: _LL16: _LL17:
# 1544
({struct Cyc_Absyn_Exp*_tmp7F4=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp27C="0.0F";_tag_dyneither(_tmp27C,sizeof(char),5U);}),0),0U);res=_tmp7F4;});goto _LL7;case 1U: _LL18: _LL19:
({struct Cyc_Absyn_Exp*_tmp7F5=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp27D="0.0";_tag_dyneither(_tmp27D,sizeof(char),4U);}),1),0U);res=_tmp7F5;});goto _LL7;default: _LL1A: _tmp287=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27B)->f1;_LL1B:
({struct Cyc_Absyn_Exp*_tmp7F7=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp7F6=({const char*_tmp27E="0.0L";_tag_dyneither(_tmp27E,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp7F6,_tmp287);}),0U);res=_tmp7F7;});goto _LL7;}default: _LL1C: _LL1D:
({struct Cyc_String_pa_PrintArg_struct _tmp281=({struct Cyc_String_pa_PrintArg_struct _tmp70E;_tmp70E.tag=0U,({struct _dyneither_ptr _tmp7F8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp70E.f1=_tmp7F8;});_tmp70E;});void*_tmp27F[1U];_tmp27F[0]=& _tmp281;({struct _dyneither_ptr _tmp7F9=({const char*_tmp280="found non-zero type %s in generate_zero";_tag_dyneither(_tmp280,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F9,_tag_dyneither(_tmp27F,sizeof(void*),1U));});});}_LL7:;}
# 1549
goto _LL0;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp28A=({struct Cyc_String_pa_PrintArg_struct _tmp70F;_tmp70F.tag=0U,({struct _dyneither_ptr _tmp7FA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp70F.f1=_tmp7FA;});_tmp70F;});void*_tmp288[1U];_tmp288[0]=& _tmp28A;({struct _dyneither_ptr _tmp7FB=({const char*_tmp289="found non-zero type %s in generate_zero";_tag_dyneither(_tmp289,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7FB,_tag_dyneither(_tmp288,sizeof(void*),1U));});});}_LL0:;}
# 1552
res->topt=t;
return res;}
# 1559
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1571
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp28C=Cyc_Toc_typ_to_c(elt_type);
void*_tmp28D=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp28E=Cyc_Absyn_cstar_type(_tmp28C,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp28F=({struct Cyc_Core_Opt*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->v=_tmp28E;_tmp2C0;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp290=e1->r;void*_tmp291=_tmp290;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_Absyn_Exp*_tmp295;switch(*((int*)_tmp291)){case 20U: _LL1: _tmp295=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp291)->f1;_LL2:
# 1579
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp7FC=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp295,0,Cyc_Absyn_Other_coercion,0U);_tmp295=_tmp7FC;});
_tmp295->topt=fat_ptr_type;
_tmp295->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;}
# 1584
Cyc_Toc_exp_to_c(nv,_tmp295);xinit=_tmp295;goto _LL0;case 23U: _LL3: _tmp297=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp291)->f1;_tmp296=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp291)->f2;_LL4:
# 1586
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp7FD=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp297,0,Cyc_Absyn_Other_coercion,0U);_tmp297=_tmp7FD;});
_tmp297->topt=fat_ptr_type;
_tmp297->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;}
# 1591
Cyc_Toc_exp_to_c(nv,_tmp297);Cyc_Toc_exp_to_c(nv,_tmp296);
({struct Cyc_Absyn_Exp*_tmp800=({struct Cyc_Absyn_Exp*_tmp7FF=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp7FF,({struct Cyc_Absyn_Exp*_tmp292[3U];_tmp292[0]=_tmp297,({
struct Cyc_Absyn_Exp*_tmp7FE=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp292[1]=_tmp7FE;}),_tmp292[2]=_tmp296;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp292,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1592
xinit=_tmp800;});
# 1594
goto _LL0;default: _LL5: _LL6:
({void*_tmp293=0U;({struct _dyneither_ptr _tmp801=({const char*_tmp294="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp294,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp801,_tag_dyneither(_tmp293,sizeof(void*),0U));});});}_LL0:;}{
# 1597
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
({struct Cyc_Absyn_Exp*_tmp802=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2A1,e2,0U);z_init=_tmp802;});
z_init->topt=_tmp2A1->topt;}
# 1617
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2A2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2A3=({struct Cyc_Absyn_Vardecl*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->sc=Cyc_Absyn_Public,_tmp2BB->name=_tmp2A2,_tmp2BB->varloc=0U,_tmp2BB->tq=Cyc_Toc_mt_tq,_tmp2BB->type=_tmp28C,_tmp2BB->initializer=z_init,_tmp2BB->rgn=0,_tmp2BB->attributes=0,_tmp2BB->escapes=0;_tmp2BB;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A4=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA->tag=4U,_tmp2BA->f1=_tmp2A3;_tmp2BA;});
# 1623
struct Cyc_Absyn_Exp*_tmp2A5=Cyc_Absyn_varb_exp((void*)_tmp2A0,0U);_tmp2A5->topt=_tmp29D->topt;{
struct Cyc_Absyn_Exp*_tmp2A6=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2A5,_tmp2A6,0U);
_tmp2A7->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2A7);{
# 1629
struct Cyc_Absyn_Exp*_tmp2A8=Cyc_Absyn_varb_exp((void*)_tmp2A4,0U);_tmp2A8->topt=_tmp29D->topt;{
struct Cyc_Absyn_Exp*_tmp2A9=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2AA=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2A8,_tmp2A9,0U);
_tmp2AA->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2AA);{
# 1635
struct Cyc_List_List*_tmp2AB=({struct Cyc_Absyn_Exp*_tmp2B9[2U];({struct Cyc_Absyn_Exp*_tmp804=Cyc_Absyn_varb_exp((void*)_tmp29B,0U);_tmp2B9[0]=_tmp804;}),({
struct Cyc_Absyn_Exp*_tmp803=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2B9[1]=_tmp803;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2B9,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2AC=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp806=({struct Cyc_Absyn_Exp*_tmp805=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2AB,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp805,_tmp2AC,0U);});xsize=_tmp806;});{
# 1642
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
# 1599
;_pop_region(rgn2);};}
# 1668 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1672
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2C1=Cyc_Tcutil_compress(aggrtype);void*_tmp2C2=_tmp2C1;union Cyc_Absyn_AggrInfo _tmp2C7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)->tag == 20U){_LL1: _tmp2C7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)->f1;_LL2:
({struct Cyc_Absyn_Aggrdecl*_tmp819=Cyc_Absyn_get_known_aggrdecl(_tmp2C7);ad=_tmp819;});goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp2C3=0U;({struct _dyneither_ptr _tmp81C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C6=({struct Cyc_String_pa_PrintArg_struct _tmp710;_tmp710.tag=0U,({
struct _dyneither_ptr _tmp81A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp710.f1=_tmp81A;});_tmp710;});void*_tmp2C4[1U];_tmp2C4[0]=& _tmp2C6;({struct _dyneither_ptr _tmp81B=({const char*_tmp2C5="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2C5,sizeof(char),51U);});Cyc_aprintf(_tmp81B,_tag_dyneither(_tmp2C4,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp81C,_tag_dyneither(_tmp2C3,sizeof(void*),0U));});});}_LL0:;}{
# 1678
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
# 1692
struct Cyc_Absyn_Exp*_tmp2D3=({struct Cyc_Absyn_Exp*_tmp822=aggrproj(_tmp2C9,f,0U);Cyc_Toc_member_exp(_tmp822,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2D4=Cyc_Absyn_neq_exp(_tmp2D3,_tmp2CA,0U);
struct Cyc_Absyn_Exp*_tmp2D5=({struct Cyc_Absyn_Exp*_tmp823=aggrproj(_tmp2C9,f,0U);Cyc_Toc_member_exp(_tmp823,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2D6=Cyc_Absyn_exp_stmt(_tmp2D5,0U);
struct Cyc_Absyn_Stmt*_tmp2D7=({struct Cyc_Absyn_Exp*_tmp825=_tmp2D4;struct Cyc_Absyn_Stmt*_tmp824=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp825,_tmp824,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2D8=({struct _tuple1*_tmp828=_tmp2C8;void*_tmp827=e1_c_type;struct Cyc_Absyn_Exp*_tmp826=e1;Cyc_Absyn_declare_stmt(_tmp828,_tmp827,_tmp826,Cyc_Absyn_seq_stmt(_tmp2D7,_tmp2D6,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2D8);}};}
# 1702
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1705
void*_tmp2D9=Cyc_Tcutil_compress(t);void*_tmp2DA=_tmp2D9;union Cyc_Absyn_AggrInfo _tmp2E1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->f1)->tag == 20U){_LL1: _tmp2E1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->f1)->f1;_LL2: {
# 1707
struct Cyc_Absyn_Aggrdecl*_tmp2DB=Cyc_Absyn_get_known_aggrdecl(_tmp2E1);
({int _tmp829=Cyc_Toc_get_member_offset(_tmp2DB,f);*f_tag=_tmp829;});{
# 1710
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DF=({struct Cyc_String_pa_PrintArg_struct _tmp712;_tmp712.tag=0U,_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2DB->name).f2);_tmp712;});struct Cyc_String_pa_PrintArg_struct _tmp2E0=({struct Cyc_String_pa_PrintArg_struct _tmp711;_tmp711.tag=0U,_tmp711.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp711;});void*_tmp2DD[2U];_tmp2DD[0]=& _tmp2DF,_tmp2DD[1]=& _tmp2E0;({struct _dyneither_ptr _tmp82A=({const char*_tmp2DE="_union_%s_%s";_tag_dyneither(_tmp2DE,sizeof(char),13U);});Cyc_aprintf(_tmp82A,_tag_dyneither(_tmp2DD,sizeof(void*),2U));});});
({void*_tmp82B=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));*_tmp2DC=str;_tmp2DC;}));*tagged_member_type=_tmp82B;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DB->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1721
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1724
void*_tmp2E2=e->r;void*_tmp2E3=_tmp2E2;struct Cyc_Absyn_Exp*_tmp2EF;struct _dyneither_ptr*_tmp2EE;int*_tmp2ED;struct Cyc_Absyn_Exp*_tmp2EC;struct _dyneither_ptr*_tmp2EB;int*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2E9;switch(*((int*)_tmp2E3)){case 14U: _LL1: _tmp2E9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E3)->f2;_LL2:
({void*_tmp2E4=0U;({struct _dyneither_ptr _tmp82C=({const char*_tmp2E5="cast on lhs!";_tag_dyneither(_tmp2E5,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp82C,_tag_dyneither(_tmp2E4,sizeof(void*),0U));});});case 21U: _LL3: _tmp2EC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E3)->f1;_tmp2EB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E3)->f2;_tmp2EA=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E3)->f4;_LL4:
# 1727
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp2EC->topt),_tmp2EB,f_tag,tagged_member_type,clear_read,_tmp2EA);case 22U: _LL5: _tmp2EF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E3)->f1;_tmp2EE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E3)->f2;_tmp2ED=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E3)->f4;_LL6: {
# 1730
void*_tmp2E6=Cyc_Tcutil_compress((void*)_check_null(_tmp2EF->topt));void*_tmp2E7=_tmp2E6;void*_tmp2E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E7)->tag == 3U){_LLA: _tmp2E8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E7)->f1).elt_type;_LLB:
# 1732
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2E8,_tmp2EE,f_tag,tagged_member_type,clear_read,_tmp2ED);}else{_LLC: _LLD:
# 1734
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1736
 return 0;}_LL0:;}
# 1749 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1753
struct _tuple1*_tmp2F0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp2F0,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp82D=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp82D;});else{
# 1763
struct Cyc_Absyn_Exp*_tmp2F1=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp830=({struct Cyc_Absyn_Exp*_tmp82F=_tmp2F1;struct Cyc_Absyn_Stmt*_tmp82E=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp82F,_tmp82E,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp830;});}{
# 1766
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp833=_tmp2F0;void*_tmp832=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp831=
Cyc_Toc_push_address_exp(e1);
# 1766
Cyc_Absyn_declare_stmt(_tmp833,_tmp832,_tmp831,
# 1768
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple31{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple32{void*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1772
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp2F4=({struct Cyc_String_pa_PrintArg_struct _tmp713;_tmp713.tag=0U,({struct _dyneither_ptr _tmp834=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp713.f1=_tmp834;});_tmp713;});void*_tmp2F2[1U];_tmp2F2[0]=& _tmp2F4;({unsigned int _tmp836=e->loc;struct _dyneither_ptr _tmp835=({const char*_tmp2F3="exp_to_c: no type for %s";_tag_dyneither(_tmp2F3,sizeof(char),25U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp836,_tmp835,_tag_dyneither(_tmp2F2,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1777
int did_inserted_checks=0;
{void*_tmp2F5=e->annot;void*_tmp2F6=_tmp2F5;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2F6)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 did_inserted_checks=1;goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1782
void*_tmp2F7=e->r;void*_tmp2F8=_tmp2F7;struct Cyc_Absyn_Stmt*_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;struct _dyneither_ptr*_tmp499;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp497;int _tmp496;struct Cyc_Absyn_Exp*_tmp495;void**_tmp494;struct Cyc_Absyn_Exp*_tmp493;int _tmp492;int _tmp491;struct Cyc_List_List*_tmp490;struct Cyc_Absyn_Datatypedecl*_tmp48F;struct Cyc_Absyn_Datatypefield*_tmp48E;struct _tuple1**_tmp48D;struct Cyc_List_List*_tmp48C;struct Cyc_List_List*_tmp48B;struct Cyc_Absyn_Aggrdecl*_tmp48A;void*_tmp489;struct Cyc_List_List*_tmp488;struct Cyc_Absyn_Exp*_tmp487;void*_tmp486;int _tmp485;struct Cyc_Absyn_Vardecl*_tmp484;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Exp*_tmp482;int _tmp481;struct Cyc_List_List*_tmp480;struct Cyc_List_List*_tmp47F;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Exp*_tmp47B;struct _dyneither_ptr*_tmp47A;int _tmp479;int _tmp478;struct Cyc_Absyn_Exp*_tmp477;struct _dyneither_ptr*_tmp476;int _tmp475;int _tmp474;void*_tmp473;struct Cyc_List_List*_tmp472;void*_tmp471;struct Cyc_Absyn_Exp*_tmp470;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_Absyn_Exp*_tmp46E;struct Cyc_Absyn_Exp*_tmp46D;void**_tmp46C;struct Cyc_Absyn_Exp*_tmp46B;int _tmp46A;enum Cyc_Absyn_Coercion _tmp469;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_List_List*_tmp467;struct Cyc_Absyn_Exp*_tmp466;struct Cyc_Absyn_Exp*_tmp465;int _tmp464;struct Cyc_Absyn_Exp*_tmp463;struct Cyc_List_List*_tmp462;int _tmp461;struct Cyc_List_List*_tmp460;struct Cyc_Absyn_VarargInfo*_tmp45F;struct Cyc_Absyn_Exp*_tmp45E;struct Cyc_List_List*_tmp45D;struct Cyc_Absyn_Exp*_tmp45C;struct Cyc_Absyn_Exp*_tmp45B;struct Cyc_Absyn_Exp*_tmp45A;struct Cyc_Absyn_Exp*_tmp459;struct Cyc_Absyn_Exp*_tmp458;struct Cyc_Absyn_Exp*_tmp457;struct Cyc_Absyn_Exp*_tmp456;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Exp*_tmp454;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_Core_Opt*_tmp452;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*_tmp450;enum Cyc_Absyn_Incrementor _tmp44F;enum Cyc_Absyn_Primop _tmp44E;struct Cyc_List_List*_tmp44D;struct Cyc_Absyn_Exp*_tmp44C;switch(*((int*)_tmp2F8)){case 2U: _LL6: _LL7:
# 1784
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL5;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).Null_c).tag == 1){_LL8: _LL9: {
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
goto _LL5;}}else{_LLA: _LLB:
 goto _LL5;}case 1U: _LLC: _LLD:
 goto _LL5;case 41U: _LLE: _tmp44C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LLF:
 Cyc_Toc_exp_to_c(nv,_tmp44C);goto _LL5;case 3U: _LL10: _tmp44E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp44D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL11: {
# 1806
struct Cyc_List_List*_tmp2FB=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp44D);
# 1808
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp44D);
{enum Cyc_Absyn_Primop _tmp2FC=_tmp44E;switch(_tmp2FC){case Cyc_Absyn_Numelts: _LL5F: _LL60: {
# 1811
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp44D))->hd;
{void*_tmp2FD=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp2FE=_tmp2FD;void*_tmp30C;void*_tmp30B;void*_tmp30A;void*_tmp309;struct Cyc_Absyn_Exp*_tmp308;switch(*((int*)_tmp2FE)){case 4U: _LL74: _tmp308=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2FE)->f1).num_elts;_LL75:
# 1816
 e->r=((struct Cyc_Absyn_Exp*)_check_null(_tmp308))->r;
goto _LL73;case 3U: _LL76: _tmp30C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).elt_type;_tmp30B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).nullable;_tmp30A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).bounds;_tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).zero_term;_LL77: {
# 1819
struct Cyc_Absyn_Exp*_tmp2FF=({void*_tmp83A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp83A,_tmp30A);});
if(_tmp2FF == 0)
# 1822
({void*_tmp83D=({struct Cyc_Absyn_Exp*_tmp83C=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp83C,({struct Cyc_Absyn_Exp*_tmp300[2U];_tmp300[0]=(struct Cyc_Absyn_Exp*)_tmp44D->hd,({
# 1824
struct Cyc_Absyn_Exp*_tmp83B=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp30C),0U);_tmp300[1]=_tmp83B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp300,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1822
e->r=_tmp83D;});else{
# 1826
struct Cyc_Absyn_Exp*_tmp301=_tmp2FF;
# 1828
if(Cyc_Tcutil_force_type2bool(0,_tmp309)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp44D->hd);
# 1832
({void*_tmp83F=({struct Cyc_Absyn_Exp*_tmp83E=function_e;Cyc_Toc_fncall_exp_r(_tmp83E,({struct Cyc_Absyn_Exp*_tmp302[2U];_tmp302[0]=(struct Cyc_Absyn_Exp*)_tmp44D->hd,_tmp302[1]=_tmp301;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp302,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp83F;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp30B)){
if(!Cyc_Evexp_c_can_eval(_tmp301))
({void*_tmp303=0U;({unsigned int _tmp841=e->loc;struct _dyneither_ptr _tmp840=({const char*_tmp304="can't calculate numelts";_tag_dyneither(_tmp304,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp841,_tmp840,_tag_dyneither(_tmp303,sizeof(void*),0U));});});
# 1837
({void*_tmp844=({struct Cyc_Absyn_Exp*_tmp843=arg;struct Cyc_Absyn_Exp*_tmp842=_tmp301;Cyc_Toc_conditional_exp_r(_tmp843,_tmp842,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp844;});}else{
# 1839
e->r=_tmp301->r;goto _LL73;}}}
# 1842
goto _LL73;}default: _LL78: _LL79:
# 1844
({struct Cyc_String_pa_PrintArg_struct _tmp307=({struct Cyc_String_pa_PrintArg_struct _tmp714;_tmp714.tag=0U,({
struct _dyneither_ptr _tmp845=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp714.f1=_tmp845;});_tmp714;});void*_tmp305[1U];_tmp305[0]=& _tmp307;({struct _dyneither_ptr _tmp846=({const char*_tmp306="numelts primop applied to non-array %s";_tag_dyneither(_tmp306,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp846,_tag_dyneither(_tmp305,sizeof(void*),1U));});});}_LL73:;}
# 1847
goto _LL5E;}case Cyc_Absyn_Plus: _LL61: _LL62:
# 1852
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp30D=0U;({struct _dyneither_ptr _tmp847=({const char*_tmp30E="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp30E,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp847,_tag_dyneither(_tmp30D,sizeof(void*),0U));});});
{void*_tmp30F=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd);void*_tmp310=_tmp30F;void*_tmp316;void*_tmp315;void*_tmp314;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->tag == 3U){_LL7B: _tmp316=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).elt_type;_tmp315=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).ptr_atts).bounds;_tmp314=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).ptr_atts).zero_term;_LL7C: {
# 1856
struct Cyc_Absyn_Exp*_tmp311=({void*_tmp848=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp848,_tmp315);});
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp44D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp44D->tl))->hd;
if(_tmp311 == 0)
({void*_tmp84B=({struct Cyc_Absyn_Exp*_tmp84A=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp84A,({struct Cyc_Absyn_Exp*_tmp312[3U];_tmp312[0]=e1,({
struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp316),0U);_tmp312[1]=_tmp849;}),_tmp312[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp312,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1860
e->r=_tmp84B;});else{
# 1862
if(Cyc_Tcutil_force_type2bool(0,_tmp314))
({void*_tmp84D=({struct Cyc_Absyn_Exp*_tmp84C=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp84C,({struct Cyc_Absyn_Exp*_tmp313[3U];_tmp313[0]=e1,_tmp313[1]=_tmp311,_tmp313[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp313,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp84D;});}
goto _LL7A;}}else{_LL7D: _LL7E:
 goto _LL7A;}_LL7A:;}
# 1867
goto _LL5E;case Cyc_Absyn_Minus: _LL63: _LL64: {
# 1872
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp44D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp44D->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp2FB->tl))->hd)){
({void*_tmp84F=({struct Cyc_Absyn_Exp*_tmp84E=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp84E,Cyc_Toc_curr_sp);});e1->r=_tmp84F;});
({void*_tmp851=({struct Cyc_Absyn_Exp*_tmp850=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp850,Cyc_Toc_curr_sp);});e2->r=_tmp851;});
({void*_tmp853=({void*_tmp852=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp852;});e1->topt=_tmp853;});
({void*_tmp855=({struct Cyc_Absyn_Exp*_tmp854=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp854,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1880
e->r=_tmp855;});}else{
# 1883
({void*_tmp859=({struct Cyc_Absyn_Exp*_tmp858=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp858,({struct Cyc_Absyn_Exp*_tmp317[3U];_tmp317[0]=e1,({
struct Cyc_Absyn_Exp*_tmp857=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp317[1]=_tmp857;}),({
struct Cyc_Absyn_Exp*_tmp856=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp317[2]=_tmp856;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp317,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1883
e->r=_tmp859;});}}
# 1887
goto _LL5E;}case Cyc_Absyn_Eq: _LL65: _LL66:
 goto _LL68;case Cyc_Absyn_Neq: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Gt: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Gte: _LL6B: _LL6C:
 goto _LL6E;case Cyc_Absyn_Lt: _LL6D: _LL6E:
 goto _LL70;case Cyc_Absyn_Lte: _LL6F: _LL70: {
# 1895
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp44D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp44D->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp2FB->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& elt_type)){
void*_tmp318=({void*_tmp85A=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp85A,Cyc_Toc_mt_tq);});
({void*_tmp85D=({void*_tmp85C=_tmp318;Cyc_Toc_cast_it_r(_tmp85C,({struct Cyc_Absyn_Exp*_tmp85B=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp85B,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp85D;});
e1->topt=_tmp318;}
# 1905
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
void*_tmp319=({void*_tmp85E=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp85E,Cyc_Toc_mt_tq);});
({void*_tmp861=({void*_tmp860=_tmp319;Cyc_Toc_cast_it_r(_tmp860,({struct Cyc_Absyn_Exp*_tmp85F=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp85F,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp861;});
e2->topt=_tmp319;}
# 1910
goto _LL5E;}default: _LL71: _LL72:
 goto _LL5E;}_LL5E:;}
# 1913
goto _LL5;}case 5U: _LL12: _tmp450=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp44F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL13: {
# 1915
void*e2_cyc_typ=(void*)_check_null(_tmp450->topt);
# 1924 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _dyneither_ptr incr_str=({const char*_tmp32C="increment";_tag_dyneither(_tmp32C,sizeof(char),10U);});
if(_tmp44F == Cyc_Absyn_PreDec  || _tmp44F == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp862=({const char*_tmp31A="decrement";_tag_dyneither(_tmp31A,sizeof(char),10U);});incr_str=_tmp862;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp450,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp31D=({struct Cyc_String_pa_PrintArg_struct _tmp715;_tmp715.tag=0U,_tmp715.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp715;});void*_tmp31B[1U];_tmp31B[0]=& _tmp31D;({unsigned int _tmp864=e->loc;struct _dyneither_ptr _tmp863=({const char*_tmp31C="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp31C,sizeof(char),74U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp864,_tmp863,_tag_dyneither(_tmp31B,sizeof(void*),1U));});});
# 1932
if(Cyc_Toc_is_tagged_union_project(_tmp450,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp31E=_tmp44F;switch(_tmp31E){case Cyc_Absyn_PreInc: _LL80: _LL81:
 op=0U;goto _LL7F;case Cyc_Absyn_PreDec: _LL82: _LL83:
 op=2U;goto _LL7F;default: _LL84: _LL85:
({struct Cyc_String_pa_PrintArg_struct _tmp321=({struct Cyc_String_pa_PrintArg_struct _tmp716;_tmp716.tag=0U,_tmp716.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp716;});void*_tmp31F[1U];_tmp31F[0]=& _tmp321;({unsigned int _tmp866=e->loc;struct _dyneither_ptr _tmp865=({const char*_tmp320="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp320,sizeof(char),58U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp866,_tmp865,_tag_dyneither(_tmp31F,sizeof(void*),1U));});});}_LL7F:;}
# 1942
({void*_tmp868=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323->tag=4U,_tmp323->f1=_tmp450,({struct Cyc_Core_Opt*_tmp867=({struct Cyc_Core_Opt*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->v=(void*)op;_tmp322;});_tmp323->f2=_tmp867;}),_tmp323->f3=one;_tmp323;});e->r=_tmp868;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1946
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp450);
Cyc_Toc_set_lhs(nv,0);{
# 1951
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp44F == Cyc_Absyn_PostInc  || _tmp44F == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 1957
if(_tmp44F == Cyc_Absyn_PreDec  || _tmp44F == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp86D=({struct Cyc_Absyn_Exp*_tmp86C=fn_e;Cyc_Toc_fncall_exp_r(_tmp86C,({struct Cyc_Absyn_Exp*_tmp324[3U];({struct Cyc_Absyn_Exp*_tmp86B=Cyc_Toc_push_address_exp(_tmp450);_tmp324[0]=_tmp86B;}),({
struct Cyc_Absyn_Exp*_tmp86A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp324[1]=_tmp86A;}),({
struct Cyc_Absyn_Exp*_tmp869=Cyc_Absyn_signed_int_exp(change,0U);_tmp324[2]=_tmp869;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp324,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1959
e->r=_tmp86D;});}else{
# 1962
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1968
did_inserted_checks=1;
if(_tmp44F != Cyc_Absyn_PostInc){
struct _tuple1*_tmp325=Cyc_Toc_temp_var();
void*_tmp326=({void*_tmp86E=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp86E,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp327=Cyc_Toc_push_address_exp(_tmp450);
struct Cyc_Absyn_Exp*_tmp328=({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp325,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp86F,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
({struct Cyc_Absyn_Exp*_tmp872=({struct Cyc_Absyn_Exp*_tmp871=({struct Cyc_Absyn_Exp*_tmp870=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp325,0U),0U);Cyc_Absyn_neq_exp(_tmp870,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1976
Cyc_Absyn_and_exp(_tmp871,_tmp328,0U);});_tmp328=_tmp872;});{
# 1978
struct Cyc_Absyn_Stmt*_tmp329=({struct Cyc_Absyn_Exp*_tmp874=_tmp328;struct Cyc_Absyn_Stmt*_tmp873=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp325,0U),0U),0U),0U);
# 1978
Cyc_Absyn_ifthenelse_stmt(_tmp874,_tmp873,
# 1980
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp876=({struct Cyc_Absyn_Stmt*_tmp875=_tmp329;Cyc_Absyn_seq_stmt(_tmp875,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp325,0U),0U),0U),0U);});_tmp329=_tmp876;});
({void*_tmp877=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp325,_tmp326,_tmp327,_tmp329,0U),0U))->r;e->r=_tmp877;});};}else{
# 1985
struct Cyc_Toc_functionSet*_tmp32A=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp32A,_tmp450);
({void*_tmp87B=({struct Cyc_Absyn_Exp*_tmp87A=fn_e;Cyc_Toc_fncall_exp_r(_tmp87A,({struct Cyc_Absyn_Exp*_tmp32B[2U];({struct Cyc_Absyn_Exp*_tmp879=Cyc_Toc_push_address_exp(_tmp450);_tmp32B[0]=_tmp879;}),({
struct Cyc_Absyn_Exp*_tmp878=Cyc_Absyn_signed_int_exp(1,0U);_tmp32B[1]=_tmp878;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32B,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1987
e->r=_tmp87B;});}}else{
# 1990
if(elt_typ == Cyc_Absyn_void_type  && !Cyc_Absyn_is_lvalue(_tmp450)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp450,0,Cyc_Toc_incr_lvalue,_tmp44F);
e->r=_tmp450->r;}}}
# 1994
goto _LL5;};}case 4U: _LL14: _tmp453=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp452=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp451=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_LL15: {
# 2013 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp453->topt);
void*e2_old_typ=(void*)_check_null(_tmp451->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp453,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp453);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp451);
({void*_tmp87C=Cyc_Toc_tagged_union_assignop(_tmp453,e1_old_typ,_tmp452,_tmp451,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp87C;});
# 2024
goto _LL5;}{
# 2026
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp453,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp453,_tmp452,_tmp451,ptr_type,is_dyneither,elt_type);
# 2032
return;}{
# 2036
int e1_poly=Cyc_Toc_is_poly_project(_tmp453);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp453);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp451);{
# 2042
int done=0;
if(_tmp452 != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp32D=(enum Cyc_Absyn_Primop)_tmp452->v;enum Cyc_Absyn_Primop _tmp32E=_tmp32D;switch(_tmp32E){case Cyc_Absyn_Plus: _LL87: _LL88:
 change=_tmp451;goto _LL86;case Cyc_Absyn_Minus: _LL89: _LL8A:
# 2050
({struct Cyc_Absyn_Exp*_tmp87D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp451,0U);change=_tmp87D;});goto _LL86;default: _LL8B: _LL8C:
({void*_tmp32F=0U;({struct _dyneither_ptr _tmp87E=({const char*_tmp330="bad t ? pointer arithmetic";_tag_dyneither(_tmp330,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp87E,_tag_dyneither(_tmp32F,sizeof(void*),0U));});});}_LL86:;}
# 2053
done=1;{
# 2055
struct Cyc_Absyn_Exp*_tmp331=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp882=({struct Cyc_Absyn_Exp*_tmp881=_tmp331;Cyc_Toc_fncall_exp_r(_tmp881,({struct Cyc_Absyn_Exp*_tmp332[3U];({struct Cyc_Absyn_Exp*_tmp880=Cyc_Toc_push_address_exp(_tmp453);_tmp332[0]=_tmp880;}),({
struct Cyc_Absyn_Exp*_tmp87F=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp332[1]=_tmp87F;}),_tmp332[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp332,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2056
e->r=_tmp882;});};}else{
# 2059
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2062
enum Cyc_Absyn_Primop _tmp333=(enum Cyc_Absyn_Primop)_tmp452->v;enum Cyc_Absyn_Primop _tmp334=_tmp333;if(_tmp334 == Cyc_Absyn_Plus){_LL8E: _LL8F:
# 2064
 done=1;
({void*_tmp884=({struct Cyc_Absyn_Exp*_tmp883=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp453);Cyc_Toc_fncall_exp_r(_tmp883,({struct Cyc_Absyn_Exp*_tmp335[2U];_tmp335[0]=_tmp453,_tmp335[1]=_tmp451;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp335,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp884;});
goto _LL8D;}else{_LL90: _LL91:
({void*_tmp336=0U;({struct _dyneither_ptr _tmp885=({const char*_tmp337="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp337,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp885,_tag_dyneither(_tmp336,sizeof(void*),0U));});});}_LL8D:;}}}
# 2071
if(!done){
# 2073
if(e1_poly)
({void*_tmp887=({void*_tmp886=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp886,Cyc_Absyn_new_exp(_tmp451->r,0U));});_tmp451->r=_tmp887;});
# 2079
if(!Cyc_Absyn_is_lvalue(_tmp453)){
({struct Cyc_Absyn_Exp*_tmp888=_tmp453;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple30*),struct _tuple30*f_env))Cyc_Toc_lvalue_assign)(_tmp888,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp339=1;struct _tuple30*_tmp338=_cycalloc(_check_times(_tmp339,sizeof(struct _tuple30)));(_tmp338[0]).f1=_tmp452,(_tmp338[0]).f2=_tmp451;_tmp338;}));});
e->r=_tmp453->r;}}
# 2084
goto _LL5;};};};}case 6U: _LL16: _tmp456=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp455=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp454=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_LL17:
# 2086
 Cyc_Toc_exp_to_c(nv,_tmp456);
Cyc_Toc_exp_to_c(nv,_tmp455);
Cyc_Toc_exp_to_c(nv,_tmp454);
goto _LL5;case 7U: _LL18: _tmp458=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp457=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL19:
 _tmp45A=_tmp458;_tmp459=_tmp457;goto _LL1B;case 8U: _LL1A: _tmp45A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp459=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL1B:
 _tmp45C=_tmp45A;_tmp45B=_tmp459;goto _LL1D;case 9U: _LL1C: _tmp45C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp45B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL1D:
# 2093
 Cyc_Toc_exp_to_c(nv,_tmp45C);
Cyc_Toc_exp_to_c(nv,_tmp45B);
goto _LL5;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3 == 0){_LL1E: _tmp45E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp45D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL1F: {
# 2097
void*e1_typ=(void*)_check_null(_tmp45E->topt);
Cyc_Toc_exp_to_c(nv,_tmp45E);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2102
({void*_tmp88C=({void*_tmp88B=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp88B,({
struct Cyc_Absyn_Exp*_tmp88A=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp88A,({struct Cyc_List_List*_tmp33A=_cycalloc(sizeof(*_tmp33A));
({struct Cyc_Absyn_Exp*_tmp889=Cyc_Absyn_copy_exp(_tmp45E);_tmp33A->hd=_tmp889;}),_tmp33A->tl=0;_tmp33A;}),0U);}));});
# 2102
_tmp45E->r=_tmp88C;});
# 2105
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp45D);
goto _LL5;}}else{_LL20: _tmp463=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp462=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp461=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3)->num_varargs;_tmp460=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3)->injectors;_tmp45F=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3)->vai;_LL21: {
# 2116 "toc.cyc"
struct _RegionHandle _tmp33B=_new_region("r");struct _RegionHandle*r=& _tmp33B;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp461,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp45F->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2124
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp462);
int num_normargs=num_args - _tmp461;
# 2128
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp462=_tmp462->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp462))->hd);
({struct Cyc_List_List*_tmp88D=({struct Cyc_List_List*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->hd=(struct Cyc_Absyn_Exp*)_tmp462->hd,_tmp33C->tl=new_args;_tmp33C;});new_args=_tmp88D;});}}
# 2133
({struct Cyc_List_List*_tmp891=({struct Cyc_List_List*_tmp33E=_cycalloc(sizeof(*_tmp33E));({struct Cyc_Absyn_Exp*_tmp890=({struct Cyc_Absyn_Exp*_tmp88F=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp88F,({struct Cyc_Absyn_Exp*_tmp33D[3U];_tmp33D[0]=argvexp,({
# 2135
struct Cyc_Absyn_Exp*_tmp88E=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp33D[1]=_tmp88E;}),_tmp33D[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp33D,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2133
_tmp33E->hd=_tmp890;}),_tmp33E->tl=new_args;_tmp33E;});new_args=_tmp891;});
# 2138
({struct Cyc_List_List*_tmp892=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp892;});{
# 2140
void*e1_typ=(void*)_check_null(_tmp463->topt);
Cyc_Toc_exp_to_c(nv,_tmp463);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2145
({void*_tmp896=({void*_tmp895=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp895,({
struct Cyc_Absyn_Exp*_tmp894=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp894,({struct Cyc_List_List*_tmp33F=_cycalloc(sizeof(*_tmp33F));
({struct Cyc_Absyn_Exp*_tmp893=Cyc_Absyn_copy_exp(_tmp463);_tmp33F->hd=_tmp893;}),_tmp33F->tl=0;_tmp33F;}),0U);}));});
# 2145
_tmp463->r=_tmp896;});{
# 2148
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp463,new_args,0U),0U);
# 2151
if(_tmp45F->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp340=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp45F->type));void*_tmp341=_tmp340;struct Cyc_Absyn_Datatypedecl*_tmp344;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp341)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp341)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp341)->f1)->f1).KnownDatatype).tag == 2){_LL93: _tmp344=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp341)->f1)->f1).KnownDatatype).val;_LL94:
 tud=_tmp344;goto _LL92;}else{goto _LL95;}}else{goto _LL95;}}else{_LL95: _LL96:
({void*_tmp342=0U;({struct _dyneither_ptr _tmp897=({const char*_tmp343="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp343,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp897,_tag_dyneither(_tmp342,sizeof(void*),0U));});});}_LL92:;}{
# 2157
struct _dyneither_ptr vs=({unsigned int _tmp355=(unsigned int)_tmp461;struct _tuple1**_tmp354=({struct _RegionHandle*_tmp898=r;_region_malloc(_tmp898,_check_times(_tmp355,sizeof(struct _tuple1*)));});({{unsigned int _tmp717=(unsigned int)_tmp461;unsigned int i;for(i=0;i < _tmp717;++ i){({struct _tuple1*_tmp899=Cyc_Toc_temp_var();_tmp354[i]=_tmp899;});}}0;});_tag_dyneither(_tmp354,sizeof(struct _tuple1*),_tmp355);});
# 2159
if(_tmp461 != 0){
# 2163
struct Cyc_List_List*_tmp345=0;
{int i=_tmp461 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp89B=({struct Cyc_List_List*_tmp346=_cycalloc(sizeof(*_tmp346));({struct Cyc_Absyn_Exp*_tmp89A=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp346->hd=_tmp89A;}),_tmp346->tl=_tmp345;_tmp346;});_tmp345=_tmp89B;});}}
({struct Cyc_Absyn_Stmt*_tmp89F=({struct _tuple1*_tmp89E=argv;void*_tmp89D=arr_type;struct Cyc_Absyn_Exp*_tmp89C=Cyc_Absyn_array_exp(_tmp345,0U);Cyc_Absyn_declare_stmt(_tmp89E,_tmp89D,_tmp89C,s,0U);});s=_tmp89F;});
# 2168
({struct Cyc_List_List*_tmp8A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp462);_tmp462=_tmp8A0;});
({struct Cyc_List_List*_tmp8A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp460);_tmp460=_tmp8A1;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp462)- 1;
for(0;_tmp462 != 0;(_tmp462=_tmp462->tl,_tmp460=_tmp460->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp462->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp347=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp460))->hd;struct Cyc_Absyn_Datatypefield*_tmp348=_tmp347;struct _tuple1*_tmp34E;struct Cyc_List_List*_tmp34D;_LL98: _tmp34E=_tmp348->name;_tmp34D=_tmp348->typs;_LL99:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp34D))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp8A2;});{
# 2182
struct _tuple1*_tmp349=Cyc_Toc_collapse_qvars(_tmp34E,tud->name);
struct Cyc_List_List*_tmp34A=({struct _tuple19*_tmp34C[2U];({struct _tuple19*_tmp8A6=({struct _dyneither_ptr*_tmp8A5=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp8A5,Cyc_Toc_datatype_tag(tud,_tmp34E));});_tmp34C[0]=_tmp8A6;}),({
struct _tuple19*_tmp8A4=({struct _dyneither_ptr*_tmp8A3=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp8A3,arg);});_tmp34C[1]=_tmp8A4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34C,sizeof(struct _tuple19*),2U));});
({struct Cyc_Absyn_Stmt*_tmp8AA=({struct _tuple1*_tmp8A9=var;void*_tmp8A8=Cyc_Absyn_strctq(_tmp349);struct Cyc_Absyn_Exp*_tmp8A7=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->tag=29U,_tmp34B->f1=_tmp349,_tmp34B->f2=0,_tmp34B->f3=_tmp34A,_tmp34B->f4=0;_tmp34B;}),0U);
# 2185
Cyc_Absyn_declare_stmt(_tmp8A9,_tmp8A8,_tmp8A7,s,0U);});s=_tmp8AA;});};};}};}else{
# 2193
struct Cyc_List_List*_tmp34F=({struct _tuple31*_tmp350[3U];({struct _tuple31*_tmp8B3=({struct _tuple31*_tmp351=_cycalloc(sizeof(*_tmp351));({struct _dyneither_ptr _tmp8B2=_tag_dyneither(0,0,0);_tmp351->f1=_tmp8B2;}),({struct Cyc_Absyn_Exp*_tmp8B1=Cyc_Absyn_uint_exp(0U,0U);_tmp351->f2=_tmp8B1;});_tmp351;});_tmp350[0]=_tmp8B3;}),({
struct _tuple31*_tmp8B0=({struct _tuple31*_tmp352=_cycalloc(sizeof(*_tmp352));({struct _dyneither_ptr _tmp8AF=_tag_dyneither(0,0,0);_tmp352->f1=_tmp8AF;}),({struct Cyc_Absyn_Exp*_tmp8AE=Cyc_Absyn_uint_exp(0U,0U);_tmp352->f2=_tmp8AE;});_tmp352;});_tmp350[1]=_tmp8B0;}),({
struct _tuple31*_tmp8AD=({struct _tuple31*_tmp353=_cycalloc(sizeof(*_tmp353));({struct _dyneither_ptr _tmp8AC=_tag_dyneither(0,0,0);_tmp353->f1=_tmp8AC;}),({struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_uint_exp(0U,0U);_tmp353->f2=_tmp8AB;});_tmp353;});_tmp350[2]=_tmp8AD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp350,sizeof(struct _tuple31*),3U));});
({struct Cyc_Absyn_Stmt*_tmp8B7=({struct _tuple1*_tmp8B6=argv;void*_tmp8B5=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8B6,_tmp8B5,_tmp8B4,s,0U);});s=_tmp8B7;});}};}else{
# 2201
if(_tmp461 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp462 != 0;_tmp462=_tmp462->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp462->hd);
({struct Cyc_List_List*_tmp8B8=({struct Cyc_List_List*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->hd=(struct Cyc_Absyn_Exp*)_tmp462->hd,_tmp356->tl=array_args;_tmp356;});array_args=_tmp8B8;});}{
# 2207
struct Cyc_Absyn_Exp*_tmp357=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp8B9=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp357,s,0U);s=_tmp8B9;});};}else{
# 2210
({struct Cyc_Absyn_Stmt*_tmp8BD=({struct _tuple1*_tmp8BC=argv;void*_tmp8BB=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8BC,_tmp8BB,_tmp8BA,s,0U);});s=_tmp8BD;});}}
# 2213
({void*_tmp8BE=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8BE;});};};}
# 2215
_npop_handler(0U);goto _LL5;
# 2116
;_pop_region(r);}}case 11U: _LL22: _tmp465=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp464=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL23:
# 2218
 Cyc_Toc_exp_to_c(nv,_tmp465);{
struct Cyc_Absyn_Exp*_tmp358=_tmp464?Cyc_Toc_newrethrow_exp(_tmp465): Cyc_Toc_newthrow_exp(_tmp465);
({void*_tmp8C0=({void*_tmp8BF=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp8BF,_tmp358,0U);})->r;e->r=_tmp8C0;});
goto _LL5;};case 12U: _LL24: _tmp466=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL25:
 Cyc_Toc_exp_to_c(nv,_tmp466);goto _LL5;case 13U: _LL26: _tmp468=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp467=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL27:
# 2224
 Cyc_Toc_exp_to_c(nv,_tmp468);
# 2233 "toc.cyc"
for(0;_tmp467 != 0;_tmp467=_tmp467->tl){
enum Cyc_Absyn_KindQual _tmp359=(Cyc_Tcutil_type_kind((void*)_tmp467->hd))->kind;
if(_tmp359 != Cyc_Absyn_EffKind  && _tmp359 != Cyc_Absyn_RgnKind){
{void*_tmp35A=Cyc_Tcutil_compress((void*)_tmp467->hd);void*_tmp35B=_tmp35A;switch(*((int*)_tmp35B)){case 2U: _LL9B: _LL9C:
 goto _LL9E;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35B)->f1)->tag == 18U){_LL9D: _LL9E:
 continue;}else{goto _LL9F;}default: _LL9F: _LLA0:
# 2240
({void*_tmp8C2=({void*_tmp8C1=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8C1,_tmp468,0U);})->r;e->r=_tmp8C2;});
goto _LL9A;}_LL9A:;}
# 2243
break;}}
# 2246
goto _LL5;case 14U: _LL28: _tmp46C=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp46B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp46A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp469=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL29: {
# 2248
void*old_t2=(void*)_check_null(_tmp46B->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp46C;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp46C=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp46B);
# 2255
{struct _tuple32 _tmp35C=({struct _tuple32 _tmp719;({void*_tmp8C4=Cyc_Tcutil_compress(old_t2);_tmp719.f1=_tmp8C4;}),({void*_tmp8C3=Cyc_Tcutil_compress(new_typ);_tmp719.f2=_tmp8C3;});_tmp719;});struct _tuple32 _tmp35D=_tmp35C;struct Cyc_Absyn_PtrInfo _tmp381;struct Cyc_Absyn_PtrInfo _tmp380;struct Cyc_Absyn_PtrInfo _tmp37F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35D.f1)->tag == 3U)switch(*((int*)_tmp35D.f2)){case 3U: _LLA2: _tmp380=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35D.f1)->f1;_tmp37F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35D.f2)->f1;_LLA3: {
# 2257
struct Cyc_Absyn_Exp*_tmp35E=({void*_tmp8C5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8C5,(_tmp380.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp35F=({void*_tmp8C6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8C6,(_tmp37F.ptr_atts).bounds);});
int _tmp360=Cyc_Tcutil_force_type2bool(0,(_tmp380.ptr_atts).zero_term);
int _tmp361=Cyc_Tcutil_force_type2bool(0,(_tmp37F.ptr_atts).zero_term);
{struct _tuple33 _tmp362=({struct _tuple33 _tmp718;_tmp718.f1=_tmp35E,_tmp718.f2=_tmp35F;_tmp718;});struct _tuple33 _tmp363=_tmp362;if(_tmp363.f1 != 0){if(_tmp363.f2 != 0){_LLA9: _LLAA:
# 2264
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8C9=({void*_tmp8C8=*_tmp46C;Cyc_Toc_cast_it_r(_tmp8C8,({struct Cyc_Absyn_Exp*_tmp8C7=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp8C7,({struct Cyc_List_List*_tmp364=_cycalloc(sizeof(*_tmp364));
_tmp364->hd=_tmp46B,_tmp364->tl=0;_tmp364;}),0U);}));});
# 2266
e->r=_tmp8C9;});else{
# 2268
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp46B->r;}
# 2274
goto _LLA8;}else{_LLAB: _LLAC: {
# 2276
struct Cyc_Absyn_Exp*_tmp365=(struct Cyc_Absyn_Exp*)_check_null(_tmp35E);
struct _tuple13 _tmp366=Cyc_Evexp_eval_const_uint_exp(_tmp365);struct _tuple13 _tmp367=_tmp366;unsigned int _tmp375;int _tmp374;_LLB2: _tmp375=_tmp367.f1;_tmp374=_tmp367.f2;_LLB3:;
if(Cyc_Toc_is_toplevel(nv)){
# 2282
if((_tmp360  && !(_tmp37F.elt_tq).real_const) && !_tmp361)
({struct Cyc_Absyn_Exp*_tmp8CB=({struct Cyc_Absyn_Exp*_tmp8CA=_tmp365;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8CA,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp365=_tmp8CB;});
({void*_tmp8CC=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp365,_tmp46B))->r;e->r=_tmp8CC;});}else{
# 2287
if(_tmp360){
# 2292
struct _tuple1*_tmp368=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp369=Cyc_Absyn_var_exp(_tmp368,0U);
struct Cyc_Absyn_Exp*arg3;
# 2297
{void*_tmp36A=_tmp46B->r;void*_tmp36B=_tmp36A;struct Cyc_Absyn_Vardecl*_tmp36E;struct Cyc_Absyn_Vardecl*_tmp36D;switch(*((int*)_tmp36B)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp36B)->f1).Wstring_c).tag){case 8U: _LLB5: _LLB6:
 arg3=_tmp365;goto _LLB4;case 9U: _LLB7: _LLB8:
 arg3=_tmp365;goto _LLB4;default: goto _LLBD;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp36B)->f1)){case 1U: _LLB9: _tmp36D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp36B)->f1)->f1;_LLBA:
 _tmp36E=_tmp36D;goto _LLBC;case 4U: _LLBB: _tmp36E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp36B)->f1)->f1;_LLBC:
# 2302
 if(!Cyc_Tcutil_is_array_type(_tmp36E->type))
goto _LLBE;
arg3=_tmp365;
goto _LLB4;default: goto _LLBD;}default: _LLBD: _LLBE:
# 2309
 if(_tmp374  && _tmp375 != 1)
arg3=_tmp365;else{
# 2312
({struct Cyc_Absyn_Exp*_tmp8D0=({struct Cyc_Absyn_Exp*_tmp8CF=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp46B);Cyc_Absyn_fncall_exp(_tmp8CF,({struct Cyc_Absyn_Exp*_tmp36C[2U];({
struct Cyc_Absyn_Exp*_tmp8CE=({void*_tmp8CD=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp8CD,_tmp369);});_tmp36C[0]=_tmp8CE;}),_tmp36C[1]=_tmp365;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36C,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2312
arg3=_tmp8D0;});}
# 2314
goto _LLB4;}_LLB4:;}
# 2316
if(!_tmp361  && !(_tmp37F.elt_tq).real_const)
# 2319
({struct Cyc_Absyn_Exp*_tmp8D2=({struct Cyc_Absyn_Exp*_tmp8D1=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8D1,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp8D2;});{
# 2321
struct Cyc_Absyn_Exp*_tmp36F=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp37F.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp370=({struct Cyc_Absyn_Exp*_tmp8D3=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8D3,({struct Cyc_Absyn_Exp*_tmp372[3U];_tmp372[0]=_tmp369,_tmp372[1]=_tmp36F,_tmp372[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp372,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp371=Cyc_Absyn_exp_stmt(_tmp370,0U);
({struct Cyc_Absyn_Stmt*_tmp8D7=({struct _tuple1*_tmp8D6=_tmp368;void*_tmp8D5=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp8D4=_tmp46B;Cyc_Absyn_declare_stmt(_tmp8D6,_tmp8D5,_tmp8D4,_tmp371,0U);});_tmp371=_tmp8D7;});
({void*_tmp8D8=Cyc_Toc_stmt_exp_r(_tmp371);e->r=_tmp8D8;});};}else{
# 2328
({void*_tmp8DB=({struct Cyc_Absyn_Exp*_tmp8DA=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp8DA,({struct Cyc_Absyn_Exp*_tmp373[3U];_tmp373[0]=_tmp46B,({
# 2330
struct Cyc_Absyn_Exp*_tmp8D9=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp37F.elt_type),0U);_tmp373[1]=_tmp8D9;}),_tmp373[2]=_tmp365;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp373,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2328
e->r=_tmp8DB;});}}
# 2333
goto _LLA8;}}}else{if(_tmp363.f2 != 0){_LLAD: _LLAE: {
# 2343 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp376=(struct Cyc_Absyn_Exp*)_check_null(_tmp35F);
if(_tmp360  && !_tmp361)
({struct Cyc_Absyn_Exp*_tmp8DD=({struct Cyc_Absyn_Exp*_tmp8DC=_tmp35F;Cyc_Absyn_add_exp(_tmp8DC,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp376=_tmp8DD;});{
# 2350
struct Cyc_Absyn_Exp*_tmp377=({struct Cyc_Absyn_Exp*_tmp8DF=Cyc_Toc__untag_dyneither_ptr_e;Cyc_Absyn_fncall_exp(_tmp8DF,({struct Cyc_Absyn_Exp*_tmp379[3U];_tmp379[0]=_tmp46B,({
# 2352
struct Cyc_Absyn_Exp*_tmp8DE=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp380.elt_type),0U);_tmp379[1]=_tmp8DE;}),_tmp379[2]=_tmp376;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp379,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2354
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8E2=({struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp8E1,({struct Cyc_List_List*_tmp378=_cycalloc(sizeof(*_tmp378));
({struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_copy_exp(_tmp377);_tmp378->hd=_tmp8E0;}),_tmp378->tl=0;_tmp378;}));});
# 2356
_tmp377->r=_tmp8E2;});
# 2358
({void*_tmp8E3=Cyc_Toc_cast_it_r(*_tmp46C,_tmp377);e->r=_tmp8E3;});
goto _LLA8;};}}else{_LLAF: _LLB0:
# 2363
 if((_tmp360  && !_tmp361) && !(_tmp37F.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp37A=0U;({struct _dyneither_ptr _tmp8E4=({const char*_tmp37B="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp37B,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp8E4,_tag_dyneither(_tmp37A,sizeof(void*),0U));});});
({void*_tmp8E8=({struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Toc__dyneither_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp8E7,({struct Cyc_Absyn_Exp*_tmp37C[3U];_tmp37C[0]=_tmp46B,({
struct Cyc_Absyn_Exp*_tmp8E6=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp380.elt_type),0U);_tmp37C[1]=_tmp8E6;}),({
struct Cyc_Absyn_Exp*_tmp8E5=Cyc_Absyn_uint_exp(1U,0U);_tmp37C[2]=_tmp8E5;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp37C,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2366
e->r=_tmp8E8;});}
# 2370
goto _LLA8;}}_LLA8:;}
# 2372
goto _LLA1;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35D.f2)->f1)->tag == 1U){_LLA4: _tmp381=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35D.f1)->f1;_LLA5:
# 2374
{struct Cyc_Absyn_Exp*_tmp37D=({void*_tmp8E9=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8E9,(_tmp381.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp37E=_tmp37D;if(_tmp37E == 0){_LLC0: _LLC1:
# 2376
({void*_tmp8EB=({struct Cyc_Absyn_Exp*_tmp8EA=Cyc_Absyn_new_exp(_tmp46B->r,_tmp46B->loc);Cyc_Toc_aggrmember_exp_r(_tmp8EA,Cyc_Toc_curr_sp);});_tmp46B->r=_tmp8EB;});
_tmp46B->topt=new_typ_c;
goto _LLBF;}else{_LLC2: _LLC3:
 goto _LLBF;}_LLBF:;}
# 2381
goto _LLA1;}else{goto _LLA6;}default: goto _LLA6;}else{_LLA6: _LLA7:
# 2383
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp46B->r;
goto _LLA1;}_LLA1:;}
# 2387
goto _LL5;}case 15U: _LL2A: _tmp46D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL2B:
# 2390
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp46D);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp46D)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp46D,0,Cyc_Toc_address_lvalue,1);
# 2396
({void*_tmp8ED=({void*_tmp8EC=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8EC,_tmp46D);});e->r=_tmp8ED;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp46D->topt))))
# 2400
({void*_tmp8EF=({void*_tmp8EE=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8EE,_tmp46D);});e->r=_tmp8EF;});}
# 2402
goto _LL5;case 16U: _LL2C: _tmp46F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp46E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL2D:
# 2409
 Cyc_Toc_exp_to_c(nv,_tmp46E);{
# 2411
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp46E->topt));
struct _tuple1*_tmp382=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp383=Cyc_Absyn_var_exp(_tmp382,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp384=Cyc_Tcutil_compress(elt_typ);void*_tmp385=_tmp384;void*_tmp386;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp385)->tag == 4U){_LLC5: _tmp386=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp385)->f1).elt_type;_LLC6:
# 2417
({void*_tmp8F0=Cyc_Toc_typ_to_c(_tmp386);elt_typ=_tmp8F0;});
({struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Absyn_copy_exp(_tmp383);lhs=_tmp8F1;});
goto _LLC4;}else{_LLC7: _LLC8:
# 2421
({struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp383),0U);lhs=_tmp8F2;});
goto _LLC4;}_LLC4:;}{
# 2424
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp46E);
struct _tuple1*_tmp387=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp388=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp387),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp8F5=({struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp8F4,({struct Cyc_Absyn_Exp*_tmp389[2U];_tmp389[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp388),({
struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp389[1]=_tmp8F3;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp389,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2429
mexp=_tmp8F5;});else{
# 2432
({struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp383,0U),0U);mexp=_tmp8F6;});}{
# 2435
struct Cyc_Absyn_Exp*_tmp38A=Cyc_Toc_get_varsizeexp(_tmp46E);
if(_tmp38A != 0)
({struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Absyn_add_exp(mexp,_tmp38A,0U);mexp=_tmp8F7;});
# 2439
if(_tmp46F == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp8F8=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp8F8;});else{
# 2442
struct Cyc_Absyn_Exp*r=_tmp46F;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp8F9;});}{
# 2446
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp8FE=({struct Cyc_Absyn_Exp*_tmp8FD=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8FD,({struct Cyc_Absyn_Exp*_tmp38B[3U];({
struct Cyc_Absyn_Exp*_tmp8FC=Cyc_Absyn_copy_exp(_tmp383);_tmp38B[0]=_tmp8FC;}),({
struct Cyc_Absyn_Exp*_tmp8FB=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp38B[1]=_tmp8FB;}),
(unsigned int)_tmp388?_tmp38B[2]=_tmp388:({struct Cyc_Absyn_Exp*_tmp8FA=Cyc_Absyn_uint_exp(1U,0U);_tmp38B[2]=_tmp8FA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp38B,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2448
answer=_tmp8FE;});else{
# 2453
({struct Cyc_Absyn_Exp*_tmp8FF=Cyc_Absyn_copy_exp(_tmp383);answer=_tmp8FF;});}
({void*_tmp900=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C->tag=Cyc_Toc_Dest,_tmp38C->f1=_tmp383;_tmp38C;});e->annot=_tmp900;});{
struct Cyc_Absyn_Stmt*_tmp38D=({struct _tuple1*_tmp904=_tmp382;void*_tmp903=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp902=mexp;Cyc_Absyn_declare_stmt(_tmp904,_tmp903,_tmp902,({
struct Cyc_Absyn_Stmt*_tmp901=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp901,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2459
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp905=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp387),Cyc_Absyn_uint_type,array_len,_tmp38D,0U);_tmp38D=_tmp905;});
({void*_tmp906=Cyc_Toc_stmt_exp_r(_tmp38D);e->r=_tmp906;});
if(_tmp38A != 0)
({void*_tmp908=({void*_tmp907=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp907,Cyc_Absyn_copy_exp(e));});e->r=_tmp908;});
goto _LL5;};};};};};case 18U: _LL2E: _tmp470=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL2F:
# 2467
 Cyc_Toc_exp_to_c(nv,_tmp470);goto _LL5;case 17U: _LL30: _tmp471=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL31:
({void*_tmp90A=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E->tag=17U,({void*_tmp909=Cyc_Toc_typ_to_c(_tmp471);_tmp38E->f1=_tmp909;});_tmp38E;});e->r=_tmp90A;});goto _LL5;case 19U: _LL32: _tmp473=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp472=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL33: {
# 2470
void*_tmp38F=_tmp473;
struct Cyc_List_List*_tmp390=_tmp472;
for(0;_tmp390 != 0;_tmp390=_tmp390->tl){
void*_tmp391=(void*)_tmp390->hd;void*_tmp392=_tmp391;unsigned int _tmp3A4;struct _dyneither_ptr*_tmp3A3;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp392)->tag == 0U){_LLCA: _tmp3A3=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp392)->f1;_LLCB:
 goto _LLC9;}else{_LLCC: _tmp3A4=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp392)->f1;_LLCD:
# 2476
{void*_tmp393=Cyc_Tcutil_compress(_tmp38F);void*_tmp394=_tmp393;struct Cyc_List_List*_tmp3A2;struct Cyc_List_List*_tmp3A1;struct Cyc_Absyn_Datatypefield*_tmp3A0;union Cyc_Absyn_AggrInfo _tmp39F;switch(*((int*)_tmp394)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp394)->f1)){case 20U: _LLCF: _tmp39F=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp394)->f1)->f1;_LLD0: {
# 2478
struct Cyc_Absyn_Aggrdecl*_tmp395=Cyc_Absyn_get_known_aggrdecl(_tmp39F);
if(_tmp395->impl == 0)
({void*_tmp396=0U;({struct _dyneither_ptr _tmp90B=({const char*_tmp397="struct fields must be known";_tag_dyneither(_tmp397,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp90B,_tag_dyneither(_tmp396,sizeof(void*),0U));});});
_tmp3A1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp395->impl))->fields;goto _LLD2;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp394)->f1)->f1).KnownDatatypefield).tag == 2){_LLD5: _tmp3A0=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp394)->f1)->f1).KnownDatatypefield).val).f2;_LLD6:
# 2492
 if(_tmp3A4 == 0)
({void*_tmp90C=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->tag=0U,_tmp39B->f1=Cyc_Toc_tag_sp;_tmp39B;}));_tmp390->hd=_tmp90C;});else{
# 2495
({void*_tmp90D=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3A0->typs,(int)(_tmp3A4 - 1))).f2;_tmp38F=_tmp90D;});
({void*_tmp90F=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->tag=0U,({struct _dyneither_ptr*_tmp90E=Cyc_Absyn_fieldname((int)_tmp3A4);_tmp39C->f1=_tmp90E;});_tmp39C;}));_tmp390->hd=_tmp90F;});}
# 2498
goto _LLCE;}else{goto _LLD7;}default: goto _LLD7;}case 7U: _LLD1: _tmp3A1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp394)->f2;_LLD2: {
# 2483
struct Cyc_Absyn_Aggrfield*_tmp398=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3A1,(int)_tmp3A4);
({void*_tmp910=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399->tag=0U,_tmp399->f1=_tmp398->name;_tmp399;}));_tmp390->hd=_tmp910;});
_tmp38F=_tmp398->type;
goto _LLCE;}case 6U: _LLD3: _tmp3A2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp394)->f1;_LLD4:
# 2488
({void*_tmp912=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->tag=0U,({struct _dyneither_ptr*_tmp911=Cyc_Absyn_fieldname((int)(_tmp3A4 + 1));_tmp39A->f1=_tmp911;});_tmp39A;}));_tmp390->hd=_tmp912;});
({void*_tmp913=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3A2,(int)_tmp3A4)).f2;_tmp38F=_tmp913;});
goto _LLCE;default: _LLD7: _LLD8:
# 2499
({void*_tmp39D=0U;({struct _dyneither_ptr _tmp914=({const char*_tmp39E="impossible type for offsetof tuple index";_tag_dyneither(_tmp39E,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp914,_tag_dyneither(_tmp39D,sizeof(void*),0U));});});
goto _LLCE;}_LLCE:;}
# 2502
goto _LLC9;}_LLC9:;}
# 2505
({void*_tmp916=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->tag=19U,({void*_tmp915=Cyc_Toc_typ_to_c(_tmp473);_tmp3A5->f1=_tmp915;}),_tmp3A5->f2=_tmp472;_tmp3A5;});e->r=_tmp916;});
goto _LL5;}case 21U: _LL34: _tmp477=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp476=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp475=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp474=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL35: {
# 2508
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp477->topt);
Cyc_Toc_exp_to_c(nv,_tmp477);
if(_tmp475  && _tmp474)
({void*_tmp91B=({struct Cyc_Absyn_Exp*_tmp91A=_tmp477;void*_tmp919=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp918=e1_cyc_type;struct _dyneither_ptr*_tmp917=_tmp476;Cyc_Toc_check_tagged_union(_tmp91A,_tmp919,_tmp918,_tmp917,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2512
e->r=_tmp91B;});
# 2514
if(is_poly)
({void*_tmp91D=({void*_tmp91C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp91C,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp91D;});
goto _LL5;}case 22U: _LL36: _tmp47B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp47A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp479=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp478=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL37: {
# 2518
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(_tmp47B->topt);
void*ta;
{void*_tmp3A6=Cyc_Tcutil_compress(e1typ);void*_tmp3A7=_tmp3A6;struct Cyc_Absyn_PtrInfo _tmp3AA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A7)->tag == 3U){_LLDA: _tmp3AA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A7)->f1;_LLDB:
 ta=_tmp3AA.elt_type;goto _LLD9;}else{_LLDC: _LLDD:
({void*_tmp3A8=0U;({struct _dyneither_ptr _tmp91E=({const char*_tmp3A9="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3A9,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp91E,_tag_dyneither(_tmp3A8,sizeof(void*),0U));});});}_LLD9:;}
# 2525
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp47B,e->annot,0);
if(_tmp479  && _tmp478)
({void*_tmp923=({struct Cyc_Absyn_Exp*_tmp922=_tmp47B;void*_tmp921=Cyc_Toc_typ_to_c(e1typ);void*_tmp920=ta;struct _dyneither_ptr*_tmp91F=_tmp47A;Cyc_Toc_check_tagged_union(_tmp922,_tmp921,_tmp920,_tmp91F,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp923;});
if(is_poly  && _tmp478)
({void*_tmp925=({void*_tmp924=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp924,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp925;});
goto _LL5;}case 20U: _LL38: _tmp47C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL39:
# 2533
 did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp47C,e->annot,0);
goto _LL5;case 23U: _LL3A: _tmp47E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp47D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL3B:
# 2537
{void*_tmp3AB=Cyc_Tcutil_compress((void*)_check_null(_tmp47E->topt));void*_tmp3AC=_tmp3AB;struct Cyc_List_List*_tmp3B4;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3AC)->tag == 6U){_LLDF: _tmp3B4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3AC)->f1;_LLE0:
# 2539
 Cyc_Toc_exp_to_c(nv,_tmp47E);{
int _tmp3AD=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp47D);{
struct _tuple13 _tmp3AE=Cyc_Evexp_eval_const_uint_exp(_tmp47D);struct _tuple13 _tmp3AF=_tmp3AE;unsigned int _tmp3B3;int _tmp3B2;_LLE4: _tmp3B3=_tmp3AF.f1;_tmp3B2=_tmp3AF.f2;_LLE5:;
if(!_tmp3B2)
({void*_tmp3B0=0U;({struct _dyneither_ptr _tmp926=({const char*_tmp3B1="unknown tuple subscript in translation to C";_tag_dyneither(_tmp3B1,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp926,_tag_dyneither(_tmp3B0,sizeof(void*),0U));});});
({void*_tmp928=({struct Cyc_Absyn_Exp*_tmp927=_tmp47E;Cyc_Toc_aggrmember_exp_r(_tmp927,Cyc_Absyn_fieldname((int)(_tmp3B3 + 1)));});e->r=_tmp928;});
goto _LLDE;};};}else{_LLE1: _LLE2:
# 2549
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,_tmp47E,e->annot,_tmp47D);
if(index_used)
({void*_tmp929=Cyc_Toc_deref_exp_r(_tmp47E);e->r=_tmp929;});
goto _LLDE;};}_LLDE:;}
# 2555
goto _LL5;case 24U: _LL3C: _tmp47F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL3D:
# 2557
 if(!Cyc_Toc_is_toplevel(nv)){
# 2559
void*_tmp3B5=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp47F));
{void*_tmp3B6=_tmp3B5;union Cyc_Absyn_AggrInfo _tmp3BB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B6)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B6)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B6)->f2 == 0){_LLE7: _tmp3BB=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B6)->f1)->f1;_LLE8: {
# 2562
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3BB);
{int i=1;for(0;_tmp47F != 0;(_tmp47F=_tmp47F->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp47F->hd);
({struct Cyc_List_List*_tmp92C=({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({struct _tuple19*_tmp92B=({struct _dyneither_ptr*_tmp92A=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp92A,(struct Cyc_Absyn_Exp*)_tmp47F->hd);});_tmp3B7->hd=_tmp92B;}),_tmp3B7->tl=dles;_tmp3B7;});dles=_tmp92C;});}}
# 2568
({void*_tmp92E=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->tag=29U,_tmp3B8->f1=sd->name,_tmp3B8->f2=0,({struct Cyc_List_List*_tmp92D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp3B8->f3=_tmp92D;}),_tmp3B8->f4=sd;_tmp3B8;});e->r=_tmp92E;});
e->topt=_tmp3B5;
goto _LLE6;}}else{goto _LLE9;}}else{goto _LLE9;}}else{_LLE9: _LLEA:
({void*_tmp3B9=0U;({struct _dyneither_ptr _tmp92F=({const char*_tmp3BA="tuple type not an aggregate";_tag_dyneither(_tmp3BA,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp92F,_tag_dyneither(_tmp3B9,sizeof(void*),0U));});});}_LLE6:;}
# 2573
goto _LL5;}else{
# 2577
struct Cyc_List_List*dles=0;
for(0;_tmp47F != 0;_tmp47F=_tmp47F->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp47F->hd);
({struct Cyc_List_List*_tmp931=({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({struct _tuple19*_tmp930=({struct _tuple19*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->f1=0,_tmp3BC->f2=(struct Cyc_Absyn_Exp*)_tmp47F->hd;_tmp3BC;});_tmp3BD->hd=_tmp930;}),_tmp3BD->tl=dles;_tmp3BD;});dles=_tmp931;});}
# 2582
({void*_tmp932=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp932;});}
# 2584
goto _LL5;case 26U: _LL3E: _tmp480=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL3F:
# 2590
{struct Cyc_List_List*_tmp3BE=_tmp480;for(0;_tmp3BE != 0;_tmp3BE=_tmp3BE->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3BE->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp933=Cyc_Toc_unresolvedmem_exp_r(0,_tmp480);e->r=_tmp933;});
goto _LL5;case 27U: _LL40: _tmp484=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp483=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp482=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp481=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL41:
# 2598
 Cyc_Toc_exp_to_c(nv,_tmp483);
Cyc_Toc_exp_to_c(nv,_tmp482);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp3BF=Cyc_Evexp_eval_const_uint_exp(_tmp483);struct _tuple13 _tmp3C0=_tmp3BF;unsigned int _tmp3CA;int _tmp3C9;_LLEC: _tmp3CA=_tmp3C0.f1;_tmp3C9=_tmp3C0.f2;_LLED:;{
void*_tmp3C1=Cyc_Toc_typ_to_c((void*)_check_null(_tmp482->topt));
struct Cyc_List_List*es=0;
# 2605
if(!Cyc_Toc_is_zero(_tmp482)){
if(!_tmp3C9)
({void*_tmp3C2=0U;({unsigned int _tmp935=_tmp483->loc;struct _dyneither_ptr _tmp934=({const char*_tmp3C3="cannot determine value of constant";_tag_dyneither(_tmp3C3,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp935,_tmp934,_tag_dyneither(_tmp3C2,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp3CA;++ i){
({struct Cyc_List_List*_tmp937=({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct _tuple19*_tmp936=({struct _tuple19*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->f1=0,_tmp3C4->f2=_tmp482;_tmp3C4;});_tmp3C5->hd=_tmp936;}),_tmp3C5->tl=es;_tmp3C5;});es=_tmp937;});}}
# 2611
if(_tmp481){
struct Cyc_Absyn_Exp*_tmp3C6=({void*_tmp938=_tmp3C1;Cyc_Toc_cast_it(_tmp938,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp93B=({struct Cyc_List_List*_tmp93A=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp93A,({struct Cyc_List_List*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));({struct _tuple19*_tmp939=({struct _tuple19*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->f1=0,_tmp3C7->f2=_tmp3C6;_tmp3C7;});_tmp3C8->hd=_tmp939;}),_tmp3C8->tl=0;_tmp3C8;}));});es=_tmp93B;});}}
# 2616
({void*_tmp93C=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp93C;});};}
# 2618
goto _LL5;case 28U: _LL42: _tmp487=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp486=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp485=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_LL43:
# 2625
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp93D=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp93D;});else{
# 2628
Cyc_Toc_exp_to_c(nv,_tmp487);}
goto _LL5;case 30U: _LL44: _tmp489=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp488=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL45:
# 2632
{struct Cyc_List_List*_tmp3CB=_tmp488;for(0;_tmp3CB != 0;_tmp3CB=_tmp3CB->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3CB->hd)).f2);}}{
void*_tmp3CC=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp3CC;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3CD=Cyc_Tcutil_compress(_tmp3CC);void*_tmp3CE=_tmp3CD;union Cyc_Absyn_AggrInfo _tmp3D2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f2 == 0){_LLEF: _tmp3D2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f1)->f1;_LLF0: {
# 2639
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3D2);
({void*_tmp93E=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->tag=29U,_tmp3CF->f1=sd->name,_tmp3CF->f2=0,_tmp3CF->f3=_tmp488,_tmp3CF->f4=sd;_tmp3CF;});e->r=_tmp93E;});
e->topt=_tmp3CC;
goto _LLEE;}}else{goto _LLF1;}}else{goto _LLF1;}}else{_LLF1: _LLF2:
({void*_tmp3D0=0U;({struct _dyneither_ptr _tmp93F=({const char*_tmp3D1="anonStruct type not an aggregate";_tag_dyneither(_tmp3D1,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp93F,_tag_dyneither(_tmp3D0,sizeof(void*),0U));});});}_LLEE:;}else{
# 2646
({void*_tmp940=Cyc_Toc_unresolvedmem_exp_r(0,_tmp488);e->r=_tmp940;});}
goto _LL5;};case 29U: _LL46: _tmp48D=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp48C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp48B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp48A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;_LL47:
# 2652
 if(_tmp48A == 0  || _tmp48A->impl == 0)
({void*_tmp3D3=0U;({struct _dyneither_ptr _tmp941=({const char*_tmp3D4="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp3D4,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp941,_tag_dyneither(_tmp3D3,sizeof(void*),0U));});});{
void*_tmp3D5=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp3D6=Cyc_Tcutil_compress(_tmp3D5);void*_tmp3D7=_tmp3D6;union Cyc_Absyn_AggrInfo _tmp3DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->tag == 20U){_LLF4: _tmp3DA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->f1;_LLF5:
({struct _tuple1*_tmp942=(Cyc_Absyn_aggr_kinded_name(_tmp3DA)).f2;*_tmp48D=_tmp942;});goto _LLF3;}else{goto _LLF6;}}else{_LLF6: _LLF7:
({void*_tmp3D8=0U;({struct _dyneither_ptr _tmp943=({const char*_tmp3D9="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp3D9,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp943,_tag_dyneither(_tmp3D8,sizeof(void*),0U));});});}_LLF3:;}{
# 2663
struct Cyc_List_List*_tmp3DB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48A->impl))->fields;
if(_tmp3DB == 0)goto _LL5;
for(0;((struct Cyc_List_List*)_check_null(_tmp3DB))->tl != 0;_tmp3DB=_tmp3DB->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp3DC=(struct Cyc_Absyn_Aggrfield*)_tmp3DB->hd;
struct Cyc_List_List*_tmp3DD=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp48B,_tmp48A->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48A->impl))->fields);
# 2670
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3DE=Cyc_Tcutil_compress(old_typ);void*_tmp3DF=_tmp3DE;struct Cyc_List_List*_tmp40B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DF)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DF)->f1)->tag == 20U){_LLF9: _tmp40B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DF)->f2;_LLFA:
# 2674
{struct Cyc_List_List*_tmp3E0=_tmp3DD;for(0;_tmp3E0 != 0;_tmp3E0=_tmp3E0->tl){
struct _tuple34*_tmp3E1=(struct _tuple34*)_tmp3E0->hd;struct _tuple34*_tmp3E2=_tmp3E1;struct Cyc_Absyn_Aggrfield*_tmp408;struct Cyc_Absyn_Exp*_tmp407;_LLFE: _tmp408=_tmp3E2->f1;_tmp407=_tmp3E2->f2;_LLFF:;{
void*_tmp3E3=_tmp407->topt;
Cyc_Toc_exp_to_c(nv,_tmp407);
# 2679
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp408->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp407->topt)))
({void*_tmp945=({void*_tmp944=Cyc_Toc_typ_to_c(_tmp408->type);Cyc_Toc_cast_it(_tmp944,
Cyc_Absyn_copy_exp(_tmp407));})->r;
# 2681
_tmp407->r=_tmp945;});
# 2684
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48A->impl))->tagged){
struct _dyneither_ptr*_tmp3E4=_tmp408->name;
struct Cyc_Absyn_Exp*_tmp3E5=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp48A,_tmp3E4),0U);
struct _tuple19*_tmp3E6=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp3E5);
struct _tuple19*_tmp3E7=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp407);
struct _tuple1*s=({struct _tuple1*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));({union Cyc_Absyn_Nmspace _tmp949=Cyc_Absyn_Abs_n(0,1);_tmp3F2->f1=_tmp949;}),({
struct _dyneither_ptr*_tmp948=({struct _dyneither_ptr*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));({struct _dyneither_ptr _tmp947=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3EF=({struct Cyc_String_pa_PrintArg_struct _tmp71B;_tmp71B.tag=0U,_tmp71B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp48A->name).f2);_tmp71B;});struct Cyc_String_pa_PrintArg_struct _tmp3F0=({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0U,_tmp71A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3E4);_tmp71A;});void*_tmp3ED[2U];_tmp3ED[0]=& _tmp3EF,_tmp3ED[1]=& _tmp3F0;({struct _dyneither_ptr _tmp946=({const char*_tmp3EE="_union_%s_%s";_tag_dyneither(_tmp3EE,sizeof(char),13U);});Cyc_aprintf(_tmp946,_tag_dyneither(_tmp3ED,sizeof(void*),2U));});});*_tmp3F1=_tmp947;});_tmp3F1;});_tmp3F2->f2=_tmp948;});_tmp3F2;});
# 2692
struct _tuple19*_tmp3E8=({
struct _dyneither_ptr*_tmp94B=_tmp3E4;Cyc_Toc_make_field(_tmp94B,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC->tag=29U,_tmp3EC->f1=s,_tmp3EC->f2=0,({struct Cyc_List_List*_tmp94A=({struct _tuple19*_tmp3EB[2U];_tmp3EB[0]=_tmp3E6,_tmp3EB[1]=_tmp3E7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3EB,sizeof(struct _tuple19*),2U));});_tmp3EC->f3=_tmp94A;}),_tmp3EC->f4=0;_tmp3EC;}),0U));});
# 2695
({void*_tmp94D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->tag=29U,_tmp3EA->f1=*_tmp48D,_tmp3EA->f2=0,({struct Cyc_List_List*_tmp94C=({struct _tuple19*_tmp3E9[1U];_tmp3E9[0]=_tmp3E8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3E9,sizeof(struct _tuple19*),1U));});_tmp3EA->f3=_tmp94C;}),_tmp3EA->f4=_tmp48A;_tmp3EA;});e->r=_tmp94D;});}
# 2701
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp3DC == _tmp408){
struct Cyc_List_List*_tmp3F3=({struct Cyc_List_List*_tmp94E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48A->impl))->exist_vars,_tmp48C);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp94E,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp48A->tvs,_tmp40B));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp3F4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48A->impl))->fields;for(0;_tmp3F4 != 0;_tmp3F4=_tmp3F4->tl){
struct Cyc_Absyn_Aggrfield*_tmp3F5=(struct Cyc_Absyn_Aggrfield*)_tmp3F4->hd;
void*_tmp3F6=Cyc_Tcutil_substitute(_tmp3F3,_tmp3F5->type);
struct Cyc_Absyn_Aggrfield*_tmp3F7=({struct Cyc_Absyn_Aggrfield*_tmp94F=_tmp3F5;Cyc_Toc_aggrfield_to_c(_tmp94F,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp3F3,_tmp3F6)));});
({struct Cyc_List_List*_tmp950=({struct Cyc_List_List*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8->hd=_tmp3F7,_tmp3F8->tl=new_fields;_tmp3F8;});new_fields=_tmp950;});
# 2716
if(_tmp3F4->tl == 0){
{void*_tmp3F9=Cyc_Tcutil_compress(_tmp3F7->type);void*_tmp3FA=_tmp3F9;struct Cyc_Absyn_ArrayInfo _tmp3FF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3FA)->tag == 4U){_LL101: _tmp3FF=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3FA)->f1;_LL102:
# 2719
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3FF.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp3FB=_tmp3FF;
({struct Cyc_Absyn_Exp*_tmp951=Cyc_Absyn_uint_exp(0U,0U);_tmp3FB.num_elts=_tmp951;});
({void*_tmp952=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->tag=4U,_tmp3FC->f1=_tmp3FB;_tmp3FC;});_tmp3F7->type=_tmp952;});}
# 2724
goto _LL100;}else{_LL103: _LL104:
# 2728
 if(_tmp407->topt == 0)
goto _LL100;
{void*_tmp3FD=Cyc_Tcutil_compress((void*)_check_null(_tmp407->topt));void*_tmp3FE=_tmp3FD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FE)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FE)->f1)->tag == 20U){_LL106: _LL107:
# 2732
 _tmp3F7->type=(void*)_check_null(_tmp407->topt);goto _LL105;}else{goto _LL108;}}else{_LL108: _LL109:
 goto _LL105;}_LL105:;}
# 2735
goto _LL100;}_LL100:;}
# 2739
if(!Cyc_Tcutil_is_array_type(_tmp3F5->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3F5->type)))
({struct Cyc_List_List*_tmp954=({struct Cyc_List_List*_tmp401=_cycalloc(sizeof(*_tmp401));({void*_tmp953=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->tag=6U,_tmp400->f1=0;_tmp400;});_tmp401->hd=_tmp953;}),_tmp401->tl=_tmp3F7->attributes;_tmp401;});_tmp3F7->attributes=_tmp954;});}}}
# 2745
({struct Cyc_Absyn_Aggrdecl*_tmp958=({struct _dyneither_ptr*_tmp957=({struct _dyneither_ptr*_tmp405=_cycalloc(sizeof(*_tmp405));({struct _dyneither_ptr _tmp956=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp404=({struct Cyc_Int_pa_PrintArg_struct _tmp71C;_tmp71C.tag=1U,_tmp71C.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp71C;});void*_tmp402[1U];_tmp402[0]=& _tmp404;({struct _dyneither_ptr _tmp955=({const char*_tmp403="_genStruct%d";_tag_dyneither(_tmp403,sizeof(char),13U);});Cyc_aprintf(_tmp955,_tag_dyneither(_tmp402,sizeof(void*),1U));});});*_tmp405=_tmp956;});_tmp405;});Cyc_Toc_make_c_struct_defn(_tmp957,
# 2747
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 2745
_tmp48A=_tmp958;});
# 2748
*_tmp48D=_tmp48A->name;
Cyc_Toc_aggrdecl_to_c(_tmp48A);
# 2751
({void*_tmp959=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp406=_cycalloc(sizeof(*_tmp406));*_tmp406=_tmp48A;_tmp406;})),0);e->topt=_tmp959;});}};}}
# 2754
goto _LLF8;}else{goto _LLFB;}}else{_LLFB: _LLFC:
({void*_tmp409=0U;({struct _dyneither_ptr _tmp95A=({const char*_tmp40A="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp40A,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp95A,_tag_dyneither(_tmp409,sizeof(void*),0U));});});}_LLF8:;}else{
# 2763
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48A->impl))->tagged){
# 2765
struct _tuple34*_tmp40C=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp3DD))->hd;struct _tuple34*_tmp40D=_tmp40C;struct Cyc_Absyn_Aggrfield*_tmp416;struct Cyc_Absyn_Exp*_tmp415;_LL10B: _tmp416=_tmp40D->f1;_tmp415=_tmp40D->f2;_LL10C:;{
void*_tmp40E=(void*)_check_null(_tmp415->topt);
void*_tmp40F=_tmp416->type;
Cyc_Toc_exp_to_c(nv,_tmp415);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp40F) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp40E))
({void*_tmp95C=({
void*_tmp95B=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp95B,Cyc_Absyn_new_exp(_tmp415->r,0U));});
# 2771
_tmp415->r=_tmp95C;});{
# 2774
int i=Cyc_Toc_get_member_offset(_tmp48A,_tmp416->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp410=({struct _tuple19*_tmp412[2U];({struct _tuple19*_tmp95E=({struct _tuple19*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413->f1=0,_tmp413->f2=field_tag_exp;_tmp413;});_tmp412[0]=_tmp95E;}),({struct _tuple19*_tmp95D=({struct _tuple19*_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->f1=0,_tmp414->f2=_tmp415;_tmp414;});_tmp412[1]=_tmp95D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp412,sizeof(struct _tuple19*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp410,0U);
({void*_tmp960=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp411[1U];({struct _tuple19*_tmp95F=Cyc_Toc_make_field(_tmp416->name,umem);_tmp411[0]=_tmp95F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp411,sizeof(struct _tuple19*),1U));}));e->r=_tmp960;});};};}else{
# 2781
struct Cyc_List_List*_tmp417=0;
struct Cyc_List_List*_tmp418=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48A->impl))->fields;
for(0;_tmp418 != 0;_tmp418=_tmp418->tl){
struct Cyc_List_List*_tmp419=_tmp3DD;for(0;_tmp419 != 0;_tmp419=_tmp419->tl){
if((*((struct _tuple34*)_tmp419->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp418->hd){
struct _tuple34*_tmp41A=(struct _tuple34*)_tmp419->hd;struct _tuple34*_tmp41B=_tmp41A;struct Cyc_Absyn_Aggrfield*_tmp421;struct Cyc_Absyn_Exp*_tmp420;_LL10E: _tmp421=_tmp41B->f1;_tmp420=_tmp41B->f2;_LL10F:;{
void*_tmp41C=Cyc_Toc_typ_to_c(_tmp421->type);
void*_tmp41D=Cyc_Toc_typ_to_c((void*)_check_null(_tmp420->topt));
Cyc_Toc_exp_to_c(nv,_tmp420);
# 2791
if(!Cyc_Tcutil_unify(_tmp41C,_tmp41D)){
# 2793
if(!Cyc_Tcutil_is_arithmetic_type(_tmp41C) || !
Cyc_Tcutil_is_arithmetic_type(_tmp41D))
({struct Cyc_Absyn_Exp*_tmp962=({void*_tmp961=_tmp41C;Cyc_Toc_cast_it(_tmp961,Cyc_Absyn_copy_exp(_tmp420));});_tmp420=_tmp962;});}
({struct Cyc_List_List*_tmp964=({struct Cyc_List_List*_tmp41F=_cycalloc(sizeof(*_tmp41F));({struct _tuple19*_tmp963=({struct _tuple19*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->f1=0,_tmp41E->f2=_tmp420;_tmp41E;});_tmp41F->hd=_tmp963;}),_tmp41F->tl=_tmp417;_tmp41F;});_tmp417=_tmp964;});
break;};}}}
# 2800
({void*_tmp965=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp417));e->r=_tmp965;});}}
# 2803
goto _LL5;};};};case 31U: _LL48: _tmp490=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp48F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp48E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_LL49: {
# 2807
struct Cyc_List_List*_tmp422=_tmp48E->typs;
{struct Cyc_List_List*_tmp423=_tmp490;for(0;_tmp423 != 0;(_tmp423=_tmp423->tl,_tmp422=_tmp422->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp423->hd;
void*_tmp424=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp422))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp424))
({void*_tmp966=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp966;});}}{
# 2818
struct Cyc_Absyn_Exp*_tmp425=
_tmp48F->is_extensible?Cyc_Absyn_var_exp(_tmp48E->name,0U): Cyc_Toc_datatype_tag(_tmp48F,_tmp48E->name);
# 2821
if(!Cyc_Toc_is_toplevel(nv)){
# 2823
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp490 != 0;(_tmp490=_tmp490->tl,++ i)){
({struct Cyc_List_List*_tmp969=({struct Cyc_List_List*_tmp426=_cycalloc(sizeof(*_tmp426));({struct _tuple19*_tmp968=({struct _dyneither_ptr*_tmp967=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp967,(struct Cyc_Absyn_Exp*)_tmp490->hd);});_tmp426->hd=_tmp968;}),_tmp426->tl=dles;_tmp426;});dles=_tmp969;});}}{
# 2829
struct _tuple19*_tmp427=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp425);
({void*_tmp96D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->tag=29U,({struct _tuple1*_tmp96C=Cyc_Toc_collapse_qvars(_tmp48E->name,_tmp48F->name);_tmp429->f1=_tmp96C;}),_tmp429->f2=0,({
struct Cyc_List_List*_tmp96B=({struct Cyc_List_List*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->hd=_tmp427,({struct Cyc_List_List*_tmp96A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp428->tl=_tmp96A;});_tmp428;});_tmp429->f3=_tmp96B;}),_tmp429->f4=0;_tmp429;});
# 2830
e->r=_tmp96D;});};}else{
# 2835
struct Cyc_List_List*_tmp42A=0;
for(0;_tmp490 != 0;_tmp490=_tmp490->tl){
({struct Cyc_List_List*_tmp96F=({struct Cyc_List_List*_tmp42C=_cycalloc(sizeof(*_tmp42C));({struct _tuple19*_tmp96E=({struct _tuple19*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->f1=0,_tmp42B->f2=(struct Cyc_Absyn_Exp*)_tmp490->hd;_tmp42B;});_tmp42C->hd=_tmp96E;}),_tmp42C->tl=_tmp42A;_tmp42C;});_tmp42A=_tmp96F;});}
({void*_tmp972=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp42E=_cycalloc(sizeof(*_tmp42E));({struct _tuple19*_tmp971=({struct _tuple19*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D->f1=0,_tmp42D->f2=_tmp425;_tmp42D;});_tmp42E->hd=_tmp971;}),({
struct Cyc_List_List*_tmp970=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp42A);_tmp42E->tl=_tmp970;});_tmp42E;}));
# 2838
e->r=_tmp972;});}
# 2841
goto _LL5;};}case 32U: _LL4A: _LL4B:
# 2843
 goto _LL4D;case 33U: _LL4C: _LL4D:
 goto _LL5;case 34U: _LL4E: _tmp496=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).is_calloc;_tmp495=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).rgn;_tmp494=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).elt_type;_tmp493=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).num_elts;_tmp492=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).fat_result;_tmp491=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).inline_call;_LL4F: {
# 2847
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp494)));
Cyc_Toc_exp_to_c(nv,_tmp493);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=_tmp493;
if(_tmp492){
({struct _tuple1*_tmp973=Cyc_Toc_temp_var();x=_tmp973;});
({struct Cyc_Absyn_Exp*_tmp974=Cyc_Absyn_var_exp(x,0U);num_elts2=_tmp974;});}
# 2856
if(_tmp496){
if(_tmp495 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp495;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmp977=({struct Cyc_Absyn_Exp*_tmp976=rgn;struct Cyc_Absyn_Exp*_tmp975=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmp976,_tmp975,num_elts2);});pexp=_tmp977;});}else{
# 2862
({struct Cyc_Absyn_Exp*_tmp97A=({void*_tmp979=*_tmp494;struct Cyc_Absyn_Exp*_tmp978=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp979,_tmp978,num_elts2);});pexp=_tmp97A;});}}else{
# 2864
if(_tmp495 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp495;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp491)
({struct Cyc_Absyn_Exp*_tmp97B=Cyc_Toc_rmalloc_inline_exp(rgn,num_elts2);pexp=_tmp97B;});else{
# 2870
({struct Cyc_Absyn_Exp*_tmp97C=Cyc_Toc_rmalloc_exp(rgn,num_elts2);pexp=_tmp97C;});}}else{
# 2872
({struct Cyc_Absyn_Exp*_tmp97D=Cyc_Toc_malloc_exp(*_tmp494,num_elts2);pexp=_tmp97D;});}}
# 2874
if(_tmp492){
struct Cyc_Absyn_Exp*elt_sz=_tmp496?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp97E=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp97E,({struct Cyc_Absyn_Exp*_tmp42F[3U];_tmp42F[0]=pexp,_tmp42F[1]=elt_sz,_tmp42F[2]=num_elts2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42F,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp981=(struct _tuple1*)_check_null(x);void*_tmp980=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp97F=_tmp493;Cyc_Absyn_declare_stmt(_tmp981,_tmp980,_tmp97F,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp982=Cyc_Toc_stmt_exp_r(s);e->r=_tmp982;});}else{
# 2881
e->r=pexp->r;}
goto _LL5;};}case 35U: _LL50: _tmp498=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp497=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL51: {
# 2891
void*e1_old_typ=(void*)_check_null(_tmp498->topt);
void*e2_old_typ=(void*)_check_null(_tmp497->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp430=0U;({struct _dyneither_ptr _tmp983=({const char*_tmp431="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp431,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp983,_tag_dyneither(_tmp430,sizeof(void*),0U));});});{
# 2899
unsigned int _tmp432=e->loc;
struct _tuple1*_tmp433=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp434=Cyc_Absyn_var_exp(_tmp433,_tmp432);_tmp434->topt=e1_old_typ;{
struct _tuple1*_tmp435=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp436=Cyc_Absyn_var_exp(_tmp435,_tmp432);_tmp436->topt=e2_old_typ;{
# 2905
struct Cyc_Absyn_Exp*_tmp437=({struct Cyc_Absyn_Exp*_tmp985=Cyc_Tcutil_deep_copy_exp(1,_tmp498);struct Cyc_Absyn_Exp*_tmp984=_tmp436;Cyc_Absyn_assign_exp(_tmp985,_tmp984,_tmp432);});_tmp437->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp438=Cyc_Absyn_exp_stmt(_tmp437,_tmp432);
struct Cyc_Absyn_Exp*_tmp439=({struct Cyc_Absyn_Exp*_tmp987=Cyc_Tcutil_deep_copy_exp(1,_tmp497);struct Cyc_Absyn_Exp*_tmp986=_tmp434;Cyc_Absyn_assign_exp(_tmp987,_tmp986,_tmp432);});_tmp439->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp43A=Cyc_Absyn_exp_stmt(_tmp439,_tmp432);
# 2910
struct Cyc_Absyn_Stmt*_tmp43B=({struct _tuple1*_tmp98F=_tmp433;void*_tmp98E=e1_old_typ;struct Cyc_Absyn_Exp*_tmp98D=_tmp498;struct Cyc_Absyn_Stmt*_tmp98C=({
struct _tuple1*_tmp98B=_tmp435;void*_tmp98A=e2_old_typ;struct Cyc_Absyn_Exp*_tmp989=_tmp497;struct Cyc_Absyn_Stmt*_tmp988=
Cyc_Absyn_seq_stmt(_tmp438,_tmp43A,_tmp432);
# 2911
Cyc_Absyn_declare_stmt(_tmp98B,_tmp98A,_tmp989,_tmp988,_tmp432);});
# 2910
Cyc_Absyn_declare_stmt(_tmp98F,_tmp98E,_tmp98D,_tmp98C,_tmp432);});
# 2913
Cyc_Toc_stmt_to_c(nv,_tmp43B);
({void*_tmp990=Cyc_Toc_stmt_exp_r(_tmp43B);e->r=_tmp990;});
goto _LL5;};};};};};}case 38U: _LL52: _tmp49A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp499=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL53: {
# 2918
void*_tmp43C=Cyc_Tcutil_compress((void*)_check_null(_tmp49A->topt));
Cyc_Toc_exp_to_c(nv,_tmp49A);
{void*_tmp43D=_tmp43C;struct Cyc_Absyn_Aggrdecl*_tmp443;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43D)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43D)->f1)->f1).KnownAggr).tag == 2){_LL111: _tmp443=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43D)->f1)->f1).KnownAggr).val;_LL112: {
# 2922
struct Cyc_Absyn_Exp*_tmp43E=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp443,_tmp499),0U);
struct Cyc_Absyn_Exp*_tmp43F=Cyc_Toc_member_exp(_tmp49A,_tmp499,0U);
struct Cyc_Absyn_Exp*_tmp440=Cyc_Toc_member_exp(_tmp43F,Cyc_Toc_tag_sp,0U);
({void*_tmp991=(Cyc_Absyn_eq_exp(_tmp440,_tmp43E,0U))->r;e->r=_tmp991;});
goto _LL110;}}else{goto _LL113;}}else{goto _LL113;}}else{_LL113: _LL114:
({void*_tmp441=0U;({struct _dyneither_ptr _tmp992=({const char*_tmp442="non-aggregate type in tagcheck";_tag_dyneither(_tmp442,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp992,_tag_dyneither(_tmp441,sizeof(void*),0U));});});}_LL110:;}
# 2929
goto _LL5;}case 37U: _LL54: _tmp49B=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_LL55:
 Cyc_Toc_stmt_to_c(nv,_tmp49B);goto _LL5;case 36U: _LL56: _LL57:
# 2932
({void*_tmp444=0U;({struct _dyneither_ptr _tmp993=({const char*_tmp445="UnresolvedMem";_tag_dyneither(_tmp445,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp993,_tag_dyneither(_tmp444,sizeof(void*),0U));});});case 25U: _LL58: _LL59:
({void*_tmp446=0U;({struct _dyneither_ptr _tmp994=({const char*_tmp447="compoundlit";_tag_dyneither(_tmp447,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp994,_tag_dyneither(_tmp446,sizeof(void*),0U));});});case 39U: _LL5A: _LL5B:
({void*_tmp448=0U;({struct _dyneither_ptr _tmp995=({const char*_tmp449="valueof(-)";_tag_dyneither(_tmp449,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp995,_tag_dyneither(_tmp448,sizeof(void*),0U));});});default: _LL5C: _LL5D:
({void*_tmp44A=0U;({struct _dyneither_ptr _tmp996=({const char*_tmp44B="__asm__";_tag_dyneither(_tmp44B,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp996,_tag_dyneither(_tmp44A,sizeof(void*),0U));});});}_LL5:;};};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2966 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2968
return({struct _tuple35*_tmp49C=_region_malloc(r,sizeof(*_tmp49C));_tmp49C->f1=0,({struct _dyneither_ptr*_tmp998=Cyc_Toc_fresh_label();_tmp49C->f2=_tmp998;}),({struct _dyneither_ptr*_tmp997=Cyc_Toc_fresh_label();_tmp49C->f3=_tmp997;}),_tmp49C->f4=sc;_tmp49C;});}
# 2973
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp49D=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 2979
if((int)(((_tmp49D->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp4A0=_tmp49D->orig_pat;if((_tmp4A0.pattern).tag != 1)_throw_match();(_tmp4A0.pattern).val;})->topt);
void*_tmp49E=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp49F=_tmp49E;switch(*((int*)_tmp49F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49F)->f1)){case 0U: _LL1: _LL2:
# 2984
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmp99A=({void*_tmp999=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp999,v);});v=_tmp99A;});goto _LL0;}_LL0:;}{
# 2990
void*_tmp4A1=_tmp49D->access;void*_tmp4A2=_tmp4A1;struct Cyc_Absyn_Datatypedecl*_tmp4AD;struct Cyc_Absyn_Datatypefield*_tmp4AC;unsigned int _tmp4AB;int _tmp4AA;struct _dyneither_ptr*_tmp4A9;unsigned int _tmp4A8;switch(*((int*)_tmp4A2)){case 0U: _LLA: _LLB:
# 2995
 goto _LL9;case 1U: _LLC: _LLD:
# 3000
 if(ps->tl != 0){
void*_tmp4A3=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp4A4=_tmp4A3;struct Cyc_Absyn_Datatypedecl*_tmp4A7;struct Cyc_Absyn_Datatypefield*_tmp4A6;unsigned int _tmp4A5;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A4)->tag == 3U){_LL15: _tmp4A7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A4)->f1;_tmp4A6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A4)->f2;_tmp4A5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A4)->f3;_LL16:
# 3003
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmp99D=({void*_tmp99C=({void*_tmp99B=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4A6->name,_tmp4A7->name));Cyc_Absyn_cstar_type(_tmp99B,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp99C,v);});v=_tmp99D;});
({struct Cyc_Absyn_Exp*_tmp99F=({struct Cyc_Absyn_Exp*_tmp99E=v;Cyc_Absyn_aggrarrow_exp(_tmp99E,Cyc_Absyn_fieldname((int)(_tmp4A5 + 1)),0U);});v=_tmp99F;});
continue;}else{_LL17: _LL18:
# 3008
 goto _LL14;}_LL14:;}
# 3011
({struct Cyc_Absyn_Exp*_tmp9A0=Cyc_Absyn_deref_exp(v,0U);v=_tmp9A0;});
goto _LL9;case 2U: _LLE: _tmp4A8=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4A2)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmp9A2=({struct Cyc_Absyn_Exp*_tmp9A1=v;Cyc_Toc_member_exp(_tmp9A1,Cyc_Absyn_fieldname((int)(_tmp4A8 + 1)),0U);});v=_tmp9A2;});goto _LL9;case 4U: _LL10: _tmp4AA=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4A2)->f1;_tmp4A9=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4A2)->f2;_LL11:
# 3015
({struct Cyc_Absyn_Exp*_tmp9A3=Cyc_Toc_member_exp(v,_tmp4A9,0U);v=_tmp9A3;});
if(_tmp4AA)
({struct Cyc_Absyn_Exp*_tmp9A4=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmp9A4;});
goto _LL9;default: _LL12: _tmp4AD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A2)->f1;_tmp4AC=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A2)->f2;_tmp4AB=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4A2)->f3;_LL13:
# 3020
({struct Cyc_Absyn_Exp*_tmp9A6=({struct Cyc_Absyn_Exp*_tmp9A5=v;Cyc_Toc_member_exp(_tmp9A5,Cyc_Absyn_fieldname((int)(_tmp4AB + 1)),0U);});v=_tmp9A6;});
goto _LL9;}_LL9:;};}
# 3024
return v;}
# 3029
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4AE=t;struct Cyc_Absyn_Datatypedecl*_tmp4C7;struct Cyc_Absyn_Datatypefield*_tmp4C6;struct _dyneither_ptr*_tmp4C5;int _tmp4C4;int _tmp4C3;struct Cyc_Absyn_Datatypedecl*_tmp4C2;struct Cyc_Absyn_Datatypefield*_tmp4C1;unsigned int _tmp4C0;struct _dyneither_ptr _tmp4BF;int _tmp4BE;void*_tmp4BD;struct Cyc_Absyn_Enumfield*_tmp4BC;struct Cyc_Absyn_Enumdecl*_tmp4BB;struct Cyc_Absyn_Enumfield*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4B9;switch(*((int*)_tmp4AE)){case 0U: _LL1: _tmp4B9=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4AE)->f1;_LL2:
# 3033
 if(_tmp4B9 == 0)return v;else{return _tmp4B9;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmp9A7=v;Cyc_Absyn_eq_exp(_tmp9A7,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmp9A8=v;Cyc_Absyn_neq_exp(_tmp9A8,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4BB=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4AE)->f1;_tmp4BA=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4AE)->f2;_LL8:
# 3037
 return({struct Cyc_Absyn_Exp*_tmp9A9=v;Cyc_Absyn_eq_exp(_tmp9A9,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF->tag=32U,_tmp4AF->f1=_tmp4BB,_tmp4AF->f2=_tmp4BA;_tmp4AF;}),0U),0U);});case 4U: _LL9: _tmp4BD=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4AE)->f1;_tmp4BC=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4AE)->f2;_LLA:
# 3039
 return({struct Cyc_Absyn_Exp*_tmp9AA=v;Cyc_Absyn_eq_exp(_tmp9AA,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0->tag=33U,_tmp4B0->f1=_tmp4BD,_tmp4B0->f2=_tmp4BC;_tmp4B0;}),0U),0U);});case 5U: _LLB: _tmp4BF=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4AE)->f1;_tmp4BE=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4AE)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmp9AB=v;Cyc_Absyn_eq_exp(_tmp9AB,Cyc_Absyn_float_exp(_tmp4BF,_tmp4BE,0U),0U);});case 6U: _LLD: _tmp4C0=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4AE)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmp9AC=v;Cyc_Absyn_eq_exp(_tmp9AC,Cyc_Absyn_signed_int_exp((int)_tmp4C0,0U),0U);});case 7U: _LLF: _tmp4C3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4AE)->f1;_tmp4C2=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4AE)->f2;_tmp4C1=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4AE)->f3;_LL10:
# 3045
 LOOP1: {
void*_tmp4B1=v->r;void*_tmp4B2=_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B3;switch(*((int*)_tmp4B2)){case 14U: _LL16: _tmp4B3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B2)->f2;_LL17:
 v=_tmp4B3;goto LOOP1;case 20U: _LL18: _tmp4B4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B2)->f1;_LL19:
 v=_tmp4B4;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3052
({struct Cyc_Absyn_Exp*_tmp9AF=({void*_tmp9AE=({void*_tmp9AD=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4C1->name,_tmp4C2->name));Cyc_Absyn_cstar_type(_tmp9AD,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9AE,v);});v=_tmp9AF;});
return({struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp9B0,Cyc_Absyn_uint_exp((unsigned int)_tmp4C3,0U),0U);});case 8U: _LL11: _tmp4C5=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4AE)->f1;_tmp4C4=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4AE)->f2;_LL12:
# 3057
({struct Cyc_Absyn_Exp*_tmp9B2=({struct Cyc_Absyn_Exp*_tmp9B1=Cyc_Toc_member_exp(v,_tmp4C5,0U);Cyc_Toc_member_exp(_tmp9B1,Cyc_Toc_tag_sp,0U);});v=_tmp9B2;});
return({struct Cyc_Absyn_Exp*_tmp9B3=v;Cyc_Absyn_eq_exp(_tmp9B3,Cyc_Absyn_signed_int_exp(_tmp4C4,0U),0U);});default: _LL13: _tmp4C7=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4AE)->f1;_tmp4C6=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4AE)->f2;_LL14:
# 3061
 LOOP2: {
void*_tmp4B5=v->r;void*_tmp4B6=_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;switch(*((int*)_tmp4B6)){case 14U: _LL1D: _tmp4B7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B6)->f2;_LL1E:
 v=_tmp4B7;goto LOOP2;case 20U: _LL1F: _tmp4B8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B6)->f1;_LL20:
 v=_tmp4B8;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3068
({struct Cyc_Absyn_Exp*_tmp9B6=({void*_tmp9B5=({void*_tmp9B4=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4C6->name,_tmp4C7->name));Cyc_Absyn_cstar_type(_tmp9B4,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9B5,v);});v=_tmp9B6;});
return({struct Cyc_Absyn_Exp*_tmp9B7=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp9B7,Cyc_Absyn_var_exp(_tmp4C6->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3082
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3090
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3092
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple32 _tmp4C8=*((struct _tuple32*)ss->hd);struct _tuple32 _tmp4C9=_tmp4C8;void*_tmp4CF;_LL1: _tmp4CF=_tmp4C9.f1;_LL2:;{
void*_tmp4CA=_tmp4CF;struct Cyc_Absyn_Exp*_tmp4CE;struct _dyneither_ptr*_tmp4CD;switch(*((int*)_tmp4CA)){case 3U: _LL4: _LL5:
# 3098
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3101
 continue;case 8U: _LLA: _tmp4CD=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4CA)->f1;_LLB:
# 3103
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmp9B8=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->tag=3U,_tmp4CB->f1=_tmp4CD;_tmp4CB;});k=_tmp9B8;});
continue;case 7U: _LLC: _LLD:
# 3107
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp4CE=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4CA)->f1;if(_tmp4CE != 0){_LLF:
# 3111
({void*_tmp9B9=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC->tag=2U,_tmp4CC->f1=_tmp4CE;_tmp4CC;});k=_tmp9B9;});
return({struct _tuple36 _tmp71D;_tmp71D.f1=0,_tmp71D.f2=k;_tmp71D;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp71E;_tmp71E.f1=0,_tmp71E.f2=k;_tmp71E;});}_LL3:;};}
# 3120
return({struct _tuple36 _tmp71F;_tmp71F.f1=c,_tmp71F.f2=k;_tmp71F;});}
# 3125
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4D0=p;int _tmp4DB;int _tmp4DA;unsigned int _tmp4D9;void*_tmp4D8;struct Cyc_Absyn_Enumfield*_tmp4D7;struct Cyc_Absyn_Enumdecl*_tmp4D6;struct Cyc_Absyn_Enumfield*_tmp4D5;switch(*((int*)_tmp4D0)){case 3U: _LL1: _tmp4D6=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4D0)->f1;_tmp4D5=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4D0)->f2;_LL2:
# 3129
({struct Cyc_Absyn_Exp*_tmp9BA=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1->tag=32U,_tmp4D1->f1=_tmp4D6,_tmp4D1->f2=_tmp4D5;_tmp4D1;}),0U);e=_tmp9BA;});goto _LL0;case 4U: _LL3: _tmp4D8=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4D0)->f1;_tmp4D7=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4D0)->f2;_LL4:
# 3131
({struct Cyc_Absyn_Exp*_tmp9BB=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2->tag=33U,_tmp4D2->f1=_tmp4D8,_tmp4D2->f2=_tmp4D7;_tmp4D2;}),0U);e=_tmp9BB;});goto _LL0;case 6U: _LL5: _tmp4D9=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4D0)->f1;_LL6:
 _tmp4DA=(int)_tmp4D9;goto _LL8;case 7U: _LL7: _tmp4DA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D0)->f1;_LL8:
 _tmp4DB=_tmp4DA;goto _LLA;case 8U: _LL9: _tmp4DB=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4D0)->f2;_LLA:
# 3135
({struct Cyc_Absyn_Exp*_tmp9BC=Cyc_Absyn_uint_exp((unsigned int)_tmp4DB,0U);e=_tmp9BC;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp4D3=0U;({struct _dyneither_ptr _tmp9BD=({const char*_tmp4D4="compile_pat_test_as_case!";_tag_dyneither(_tmp4D4,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9BD,_tag_dyneither(_tmp4D3,sizeof(void*),0U));});});}_LL0:;}
# 3139
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->tag=17U,_tmp4DC->f1=e;_tmp4DC;}),0U);}
# 3143
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3152
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3155
void*t=(void*)_check_null(p->topt);
void*_tmp4DD=p->r;void*_tmp4DE=_tmp4DD;union Cyc_Absyn_AggrInfo _tmp50F;struct Cyc_List_List*_tmp50E;struct Cyc_List_List*_tmp50D;struct Cyc_List_List*_tmp50C;struct Cyc_Absyn_Pat*_tmp50B;struct Cyc_Absyn_Datatypedecl*_tmp50A;struct Cyc_Absyn_Datatypefield*_tmp509;struct Cyc_List_List*_tmp508;struct Cyc_Absyn_Vardecl*_tmp507;struct Cyc_Absyn_Pat*_tmp506;struct Cyc_Absyn_Vardecl*_tmp505;struct Cyc_Absyn_Vardecl*_tmp504;struct Cyc_Absyn_Pat*_tmp503;struct Cyc_Absyn_Vardecl*_tmp502;switch(*((int*)_tmp4DE)){case 2U: _LL1: _tmp502=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4DE)->f2;_LL2: {
# 3158
struct Cyc_Absyn_Pat*_tmp4DF=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp4DF->topt=p->topt;
_tmp504=_tmp502;_tmp503=_tmp4DF;goto _LL4;}case 1U: _LL3: _tmp504=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1;_tmp503=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4DE)->f2;_LL4:
# 3163
({struct Cyc_List_List*_tmp9BE=({struct Cyc_List_List*_tmp4E0=_region_malloc(rgn,sizeof(*_tmp4E0));_tmp4E0->hd=_tmp504,_tmp4E0->tl=*decls;_tmp4E0;});*decls=_tmp9BE;});{
struct Cyc_Absyn_Stmt*_tmp4E1=({struct Cyc_Absyn_Exp*_tmp9BF=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->tag=4U,_tmp4E2->f1=_tmp504;_tmp4E2;}),0U);Cyc_Absyn_assign_stmt(_tmp9BF,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmp9C0=_tmp4E1;Cyc_Toc_seq_stmt_opt(_tmp9C0,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp503));});};case 4U: _LL5: _tmp505=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4DE)->f2;_LL6:
# 3168
({struct Cyc_List_List*_tmp9C1=({struct Cyc_List_List*_tmp4E3=_region_malloc(rgn,sizeof(*_tmp4E3));_tmp4E3->hd=_tmp505,_tmp4E3->tl=*decls;_tmp4E3;});*decls=_tmp9C1;});
return({struct Cyc_Absyn_Exp*_tmp9C2=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->tag=4U,_tmp4E4->f1=_tmp505;_tmp4E4;}),0U);Cyc_Absyn_assign_stmt(_tmp9C2,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp507=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1;_tmp506=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4DE)->f2;_LLA:
# 3173
({struct Cyc_List_List*_tmp9C3=({struct Cyc_List_List*_tmp4E5=_region_malloc(rgn,sizeof(*_tmp4E5));_tmp4E5->hd=_tmp507,_tmp4E5->tl=*decls;_tmp4E5;});*decls=_tmp9C3;});
({void*_tmp9C4=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp507->type=_tmp9C4;});{
# 3176
struct Cyc_Absyn_Stmt*_tmp4E6=({struct Cyc_Absyn_Exp*_tmp9C7=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->tag=4U,_tmp4E7->f1=_tmp507;_tmp4E7;}),0U);Cyc_Absyn_assign_stmt(_tmp9C7,({
void*_tmp9C6=({void*_tmp9C5=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp9C5,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9C6,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmp9C8=_tmp4E6;Cyc_Toc_seq_stmt_opt(_tmp9C8,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp506));});};case 9U: _LLB: _LLC:
# 3181
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1)->r)->tag == 8U){_LL17: _tmp50A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1)->r)->f1;_tmp509=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1)->r)->f2;_tmp508=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1)->r)->f3;_LL18:
# 3190
 if(_tmp508 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp509->name,_tmp50A->name);
void*_tmp4E8=({void*_tmp9C9=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmp9C9,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Toc_cast_it(_tmp4E8,path);path=_tmp9CA;});{
int cnt=1;
struct Cyc_List_List*_tmp4E9=_tmp509->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp508 != 0;(_tmp508=_tmp508->tl,_tmp4E9=((struct Cyc_List_List*)_check_null(_tmp4E9))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4EA=(struct Cyc_Absyn_Pat*)_tmp508->hd;
if(_tmp4EA->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp4EB=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4E9))->hd)).f2;
void*_tmp4EC=(void*)_check_null(_tmp4EA->topt);
void*_tmp4ED=Cyc_Toc_typ_to_c_array(_tmp4EC);
struct Cyc_Absyn_Exp*_tmp4EE=({struct Cyc_Absyn_Exp*_tmp9CB=path;Cyc_Absyn_aggrarrow_exp(_tmp9CB,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp4EB)))
({struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Toc_cast_it(_tmp4ED,_tmp4EE);_tmp4EE=_tmp9CC;});
({struct Cyc_Absyn_Stmt*_tmp9CE=({struct Cyc_Absyn_Stmt*_tmp9CD=s;Cyc_Toc_seq_stmt_opt(_tmp9CD,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4EE,_tmp4EA));});s=_tmp9CE;});};}
# 3208
return s;};};}else{_LL21: _tmp50B=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1;_LL22:
# 3253
 return({struct _RegionHandle*_tmp9D2=rgn;struct Cyc_Toc_Env**_tmp9D1=nv;struct Cyc_List_List**_tmp9D0=decls;struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmp9D2,_tmp9D1,_tmp9D0,_tmp9CF,_tmp50B);});}case 8U: _LL19: _tmp50C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4DE)->f3;_LL1A:
# 3210
 _tmp50D=_tmp50C;goto _LL1C;case 5U: _LL1B: _tmp50D=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1;_LL1C: {
# 3212
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp50D != 0;(_tmp50D=_tmp50D->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4EF=(struct Cyc_Absyn_Pat*)_tmp50D->hd;
if(_tmp4EF->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp4F0=(void*)_check_null(_tmp4EF->topt);
struct _dyneither_ptr*_tmp4F1=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmp9D8=({struct Cyc_Absyn_Stmt*_tmp9D7=s;Cyc_Toc_seq_stmt_opt(_tmp9D7,({struct _RegionHandle*_tmp9D6=rgn;struct Cyc_Toc_Env**_tmp9D5=nv;struct Cyc_List_List**_tmp9D4=decls;struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Toc_member_exp(path,_tmp4F1,0U);Cyc_Toc_extract_pattern_vars(_tmp9D6,_tmp9D5,_tmp9D4,_tmp9D3,_tmp4EF);}));});s=_tmp9D8;});};}
# 3222
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1 == 0){_LL1D: _LL1E:
({void*_tmp4F2=0U;({struct _dyneither_ptr _tmp9D9=({const char*_tmp4F3="unresolved aggregate pattern!";_tag_dyneither(_tmp4F3,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D9,_tag_dyneither(_tmp4F2,sizeof(void*),0U));});});}else{_LL1F: _tmp50F=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4DE)->f1;_tmp50E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4DE)->f3;_LL20: {
# 3226
struct Cyc_Absyn_Aggrdecl*_tmp4F4=Cyc_Absyn_get_known_aggrdecl(_tmp50F);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp50E != 0;_tmp50E=_tmp50E->tl){
struct _tuple37*_tmp4F5=(struct _tuple37*)_tmp50E->hd;
struct Cyc_Absyn_Pat*_tmp4F6=(*_tmp4F5).f2;
if(_tmp4F6->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp4F7=Cyc_Absyn_designatorlist_to_fieldname((*_tmp4F5).f1);
void*_tmp4F8=(void*)_check_null(_tmp4F6->topt);
void*_tmp4F9=Cyc_Toc_typ_to_c_array(_tmp4F8);
struct Cyc_Absyn_Exp*_tmp4FA=Cyc_Toc_member_exp(path,_tmp4F7,0U);
# 3238
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F4->impl))->tagged)({struct Cyc_Absyn_Exp*_tmp9DA=Cyc_Toc_member_exp(_tmp4FA,Cyc_Toc_val_sp,0U);_tmp4FA=_tmp9DA;});{
void*_tmp4FB=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F4->impl))->fields,_tmp4F7)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp4FB))
({struct Cyc_Absyn_Exp*_tmp9DB=Cyc_Toc_cast_it(_tmp4F9,_tmp4FA);_tmp4FA=_tmp9DB;});else{
if(!Cyc_Tcutil_is_array_type(_tmp4FB) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4FB)))
# 3245
({struct Cyc_Absyn_Exp*_tmp9DD=Cyc_Absyn_deref_exp(({void*_tmp9DC=Cyc_Absyn_cstar_type(_tmp4F9,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp9DC,
Cyc_Absyn_address_exp(_tmp4FA,0U));}),0U);
# 3245
_tmp4FA=_tmp9DD;});}
# 3248
({struct Cyc_Absyn_Stmt*_tmp9DF=({struct Cyc_Absyn_Stmt*_tmp9DE=s;Cyc_Toc_seq_stmt_opt(_tmp9DE,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4FA,_tmp4F6));});s=_tmp9DF;});};};}
# 3250
return s;}}case 15U: _LL23: _LL24:
# 3255
({void*_tmp4FC=0U;({struct _dyneither_ptr _tmp9E0=({const char*_tmp4FD="unknownid pat";_tag_dyneither(_tmp4FD,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E0,_tag_dyneither(_tmp4FC,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp4FE=0U;({struct _dyneither_ptr _tmp9E1=({const char*_tmp4FF="unknowncall pat";_tag_dyneither(_tmp4FF,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E1,_tag_dyneither(_tmp4FE,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp500=0U;({struct _dyneither_ptr _tmp9E2=({const char*_tmp501="exp pat";_tag_dyneither(_tmp501,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E2,_tag_dyneither(_tmp500,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3266
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3274
void*_tmp510=dopt;struct Cyc_List_List*_tmp54E;struct Cyc_List_List*_tmp54D;void*_tmp54C;struct Cyc_Tcpat_Rhs*_tmp54B;if(_tmp510 == 0){_LL1: _LL2:
# 3276
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp510)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp54B=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp510)->f1;_LL6:
# 3281
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp511=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp512=_tmp511;int*_tmp519;struct _dyneither_ptr*_tmp518;struct _dyneither_ptr*_tmp517;struct Cyc_Absyn_Switch_clause*_tmp516;_LLA: _tmp519=(int*)& _tmp512->f1;_tmp518=_tmp512->f2;_tmp517=_tmp512->f3;_tmp516=_tmp512->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp513=_tmp516->body;
if(_tmp513 == _tmp54B->rhs){
# 3286
if(*_tmp519)
return Cyc_Absyn_goto_stmt(_tmp518,0U);
*_tmp519=1;{
# 3290
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp9E5=rgn;struct Cyc_List_List**_tmp9E4=decls;struct Cyc_Absyn_Exp*_tmp9E3=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp9E5,& nv,_tmp9E4,_tmp9E3,_tmp516->pattern);});
# 3293
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp517,_tmp516->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmp9E6=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmp9E6;});
({struct Cyc_Absyn_Stmt*_tmp9E7=Cyc_Absyn_label_stmt(_tmp518,res,0U);res=_tmp9E7;});
({struct Cyc_List_List*_tmp9E9=({struct Cyc_List_List*_tmp515=_region_malloc(rgn,sizeof(*_tmp515));({struct _tuple38*_tmp9E8=({struct _tuple38*_tmp514=_region_malloc(rgn,sizeof(*_tmp514));_tmp514->f1=nv,_tmp514->f2=_tmp517,_tmp514->f3=_tmp513;_tmp514;});_tmp515->hd=_tmp9E8;}),_tmp515->tl=*bodies;_tmp515;});*bodies=_tmp9E9;});
return res;};}};}
# 3301
({void*_tmp51A=0U;({struct _dyneither_ptr _tmp9EA=({const char*_tmp51B="couldn't find rhs!";_tag_dyneither(_tmp51B,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9EA,_tag_dyneither(_tmp51A,sizeof(void*),0U));});});default: _LL7: _tmp54E=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp510)->f1;_tmp54D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp510)->f2;_tmp54C=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp510)->f3;_LL8: {
# 3304
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp54C,lscs,v);
# 3306
struct Cyc_Absyn_Exp*_tmp51C=({struct Cyc_List_List*_tmp9EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp54E);Cyc_Toc_compile_path(_tmp9EB,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp51D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp54D);
# 3309
struct _tuple36 _tmp51E=Cyc_Toc_admits_switch(_tmp51D);struct _tuple36 _tmp51F=_tmp51E;int _tmp54A;void*_tmp549;_LLD: _tmp54A=_tmp51F.f1;_tmp549=_tmp51F.f2;_LLE:;
if(_tmp54A > 1){
# 3313
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp530=_cycalloc(sizeof(*_tmp530));
({struct Cyc_Absyn_Switch_clause*_tmp9ED=({struct Cyc_Absyn_Switch_clause*_tmp52F=_cycalloc(sizeof(*_tmp52F));({struct Cyc_Absyn_Pat*_tmp9EC=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp52F->pattern=_tmp9EC;}),_tmp52F->pat_vars=0,_tmp52F->where_clause=0,_tmp52F->body=res,_tmp52F->loc=0U;_tmp52F;});_tmp530->hd=_tmp9ED;}),_tmp530->tl=0;_tmp530;});
# 3316
for(0;_tmp51D != 0;_tmp51D=_tmp51D->tl){
struct _tuple32 _tmp520=*((struct _tuple32*)_tmp51D->hd);struct _tuple32 _tmp521=_tmp520;void*_tmp527;void*_tmp526;_LL10: _tmp527=_tmp521.f1;_tmp526=_tmp521.f2;_LL11:;{
# 3319
struct Cyc_Absyn_Pat*_tmp522=Cyc_Toc_compile_pat_test_as_case(_tmp527);
# 3321
struct Cyc_Absyn_Stmt*_tmp523=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp526,lscs,v);
# 3323
({struct Cyc_List_List*_tmp9EF=({struct Cyc_List_List*_tmp525=_cycalloc(sizeof(*_tmp525));({struct Cyc_Absyn_Switch_clause*_tmp9EE=({struct Cyc_Absyn_Switch_clause*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->pattern=_tmp522,_tmp524->pat_vars=0,_tmp524->where_clause=0,_tmp524->body=_tmp523,_tmp524->loc=0U;_tmp524;});_tmp525->hd=_tmp9EE;}),_tmp525->tl=new_lscs;_tmp525;});new_lscs=_tmp9EF;});};}
# 3325
{void*_tmp528=_tmp549;struct _dyneither_ptr*_tmp52D;switch(*((int*)_tmp528)){case 1U: _LL13: _LL14:
# 3328
 LOOP1: {
void*_tmp529=_tmp51C->r;void*_tmp52A=_tmp529;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp52B;switch(*((int*)_tmp52A)){case 14U: _LL1C: _tmp52B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp52A)->f2;_LL1D:
 _tmp51C=_tmp52B;goto LOOP1;case 20U: _LL1E: _tmp52C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp52A)->f1;_LL1F:
 _tmp51C=_tmp52C;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3334
({struct Cyc_Absyn_Exp*_tmp9F1=Cyc_Absyn_deref_exp(({void*_tmp9F0=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp9F0,_tmp51C);}),0U);_tmp51C=_tmp9F1;});goto _LL12;case 3U: _LL15: _tmp52D=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp528)->f1;_LL16:
# 3337
({struct Cyc_Absyn_Exp*_tmp9F3=({struct Cyc_Absyn_Exp*_tmp9F2=Cyc_Toc_member_exp(_tmp51C,_tmp52D,0U);Cyc_Toc_member_exp(_tmp9F2,Cyc_Toc_tag_sp,0U);});_tmp51C=_tmp9F3;});goto _LL12;case 2U: _LL17: _LL18:
# 3339
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3342
({struct Cyc_Absyn_Stmt*_tmp9F4=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->tag=10U,_tmp52E->f1=_tmp51C,_tmp52E->f2=new_lscs,_tmp52E->f3=0;_tmp52E;}),0U);res=_tmp9F4;});}else{
# 3346
void*_tmp531=_tmp549;struct Cyc_Absyn_Exp*_tmp548;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp531)->tag == 2U){_LL23: _tmp548=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp531)->f1;_LL24: {
# 3348
struct Cyc_List_List*_tmp532=_tmp51D;void*_tmp541;struct Cyc_Tcpat_Rhs*_tmp540;if(_tmp532 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)((struct Cyc_List_List*)_tmp532)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp532)->tl == 0){_LL28: _tmp541=((struct _tuple32*)_tmp532->hd)->f1;_tmp540=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)_tmp532->hd)->f2)->f1;_LL29:
# 3354
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp533=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp534=_tmp533;int*_tmp53B;struct _dyneither_ptr*_tmp53A;struct _dyneither_ptr*_tmp539;struct Cyc_Absyn_Switch_clause*_tmp538;_LL2D: _tmp53B=(int*)& _tmp534->f1;_tmp53A=_tmp534->f2;_tmp539=_tmp534->f3;_tmp538=_tmp534->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp535=_tmp538->body;
if(_tmp535 == _tmp540->rhs){
# 3359
if(*_tmp53B)
return Cyc_Absyn_goto_stmt(_tmp53A,0U);
*_tmp53B=1;{
# 3363
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp9F7=rgn;struct Cyc_List_List**_tmp9F6=decls;struct Cyc_Absyn_Exp*_tmp9F5=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp9F7,& nv,_tmp9F6,_tmp9F5,_tmp538->pattern);});
# 3366
Cyc_Toc_exp_to_c(nv,_tmp548);{
# 3369
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp539,_tmp538->body,0U);
({struct Cyc_Absyn_Stmt*_tmp9F8=Cyc_Absyn_ifthenelse_stmt(_tmp548,r,res,0U);r=_tmp9F8;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmp9F9=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmp9F9;});
({struct Cyc_Absyn_Stmt*_tmp9FA=Cyc_Absyn_label_stmt(_tmp53A,r,0U);r=_tmp9FA;});
({struct Cyc_List_List*_tmp9FC=({struct Cyc_List_List*_tmp537=_region_malloc(rgn,sizeof(*_tmp537));({struct _tuple38*_tmp9FB=({struct _tuple38*_tmp536=_region_malloc(rgn,sizeof(*_tmp536));_tmp536->f1=nv,_tmp536->f2=_tmp539,_tmp536->f3=_tmp535;_tmp536;});_tmp537->hd=_tmp9FB;}),_tmp537->tl=*bodies;_tmp537;});*bodies=_tmp9FC;});
return r;};};}};}
# 3378
({void*_tmp53C=0U;({struct _dyneither_ptr _tmp9FD=({const char*_tmp53D="couldn't find rhs!";_tag_dyneither(_tmp53D,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FD,_tag_dyneither(_tmp53C,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp53E=0U;({struct _dyneither_ptr _tmp9FE=({const char*_tmp53F="bad where clause in match compiler";_tag_dyneither(_tmp53F,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FE,_tag_dyneither(_tmp53E,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3383
 for(0;_tmp51D != 0;_tmp51D=_tmp51D->tl){
struct _tuple32 _tmp542=*((struct _tuple32*)_tmp51D->hd);struct _tuple32 _tmp543=_tmp542;void*_tmp547;void*_tmp546;_LL30: _tmp547=_tmp543.f1;_tmp546=_tmp543.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp544=Cyc_Toc_compile_pat_test(_tmp51C,_tmp547);
struct Cyc_Absyn_Stmt*_tmp545=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp546,lscs,v);
({struct Cyc_Absyn_Stmt*_tmp9FF=Cyc_Absyn_ifthenelse_stmt(_tmp544,_tmp545,res,0U);res=_tmp9FF;});};}}_LL22:;}
# 3391
return res;}}}_LL0:;}
# 3401
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3403
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp54F=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp550=_tmp54F;struct Cyc_Toc_Env**_tmp552;struct Cyc_Absyn_Stmt*_tmp551;_LL1: _tmp552=(struct Cyc_Toc_Env**)& _tmp550->f1;_tmp551=_tmp550->f3;_LL2:;
if(_tmp551 == s)return _tmp552;}
# 3409
return 0;}
# 3413
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3416
void*_tmp553=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3419
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp554=_new_region("rgn");struct _RegionHandle*rgn=& _tmp554;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp555=Cyc_Toc_share_env(rgn,nv);
# 3426
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3431
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp555,& mydecls,& mybodies,dopt,lscs,v);
# 3439
{struct Cyc_List_List*_tmp556=lscs;for(0;_tmp556 != 0;_tmp556=_tmp556->tl){
struct _tuple35*_tmp557=(struct _tuple35*)_tmp556->hd;struct _tuple35*_tmp558=_tmp557;struct _dyneither_ptr*_tmp560;struct Cyc_Absyn_Switch_clause*_tmp55F;_LL1: _tmp560=_tmp558->f3;_tmp55F=_tmp558->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp55F->body;
# 3443
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3446
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp559=*envp;
# 3449
if(_tmp556->tl != 0){
struct _tuple35*_tmp55A=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp556->tl))->hd;struct _tuple35*_tmp55B=_tmp55A;struct _dyneither_ptr*_tmp55E;struct Cyc_Absyn_Switch_clause*_tmp55D;_LL4: _tmp55E=_tmp55B->f3;_tmp55D=_tmp55B->f4;_LL5:;{
# 3453
struct Cyc_Toc_Env**_tmp55C=Cyc_Toc_find_case_env(mybodies,_tmp55D->body);
# 3458
if(_tmp55C == 0)
({struct Cyc_Toc_Env*_tmpA00=Cyc_Toc_last_switchclause_env(rgn,_tmp559,end_l);Cyc_Toc_stmt_to_c(_tmpA00,s);});else{
# 3462
struct Cyc_List_List*vs=0;
if(_tmp55D->pat_vars != 0){
({struct Cyc_List_List*_tmpA01=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp55D->pat_vars))->v)).f1);vs=_tmpA01;});
({struct Cyc_List_List*_tmpA02=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpA02;});}
# 3467
({struct Cyc_Toc_Env*_tmpA03=Cyc_Toc_non_last_switchclause_env(rgn,_tmp559,end_l,_tmp55E,vs);Cyc_Toc_stmt_to_c(_tmpA03,s);});}};}else{
# 3471
({struct Cyc_Toc_Env*_tmpA04=Cyc_Toc_last_switchclause_env(rgn,_tmp559,end_l);Cyc_Toc_stmt_to_c(_tmpA04,s);});}};};}}{
# 3475
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA06=test_tree;Cyc_Absyn_seq_stmt(_tmpA06,({struct _dyneither_ptr*_tmpA05=end_l;Cyc_Absyn_label_stmt(_tmpA05,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3477
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp561=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpA07=Cyc_Toc_temp_var();_tmp561->name=_tmpA07;});
({void*_tmpA08=Cyc_Toc_typ_to_c_array(_tmp561->type);_tmp561->type=_tmpA08;});
({struct Cyc_Absyn_Stmt*_tmpA0A=({struct Cyc_Absyn_Decl*_tmpA09=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->tag=0U,_tmp562->f1=_tmp561;_tmp562;}),0U);Cyc_Absyn_decl_stmt(_tmpA09,res,0U);});res=_tmpA0A;});}
# 3484
({void*_tmpA0E=({struct _tuple1*_tmpA0D=v;void*_tmpA0C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA0B=e;Cyc_Absyn_declare_stmt(_tmpA0D,_tmpA0C,_tmpA0B,res,0U);})->r;whole_s->r=_tmpA0E;});
_npop_handler(0U);return;};}
# 3423
;_pop_region(rgn);};}
# 3490
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3492
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3497
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3499
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA10=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpA10,({struct Cyc_List_List*_tmp563=_cycalloc(sizeof(*_tmp563));
({struct Cyc_Absyn_Exp*_tmpA0F=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp563->hd=_tmpA0F;}),_tmp563->tl=0;_tmp563;}),0U);}),0U);}
# 3503
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA12=({struct Cyc_Absyn_Stmt*_tmpA11=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA11,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA12;});}
# 3508
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3510
while(1){
void*_tmp564=s->r;void*_tmp565=_tmp564;struct Cyc_Absyn_Stmt*_tmp5BC;struct Cyc_List_List*_tmp5BB;void*_tmp5BA;struct Cyc_Absyn_Stmt*_tmp5B9;struct Cyc_Absyn_Exp*_tmp5B8;struct _dyneither_ptr*_tmp5B7;struct Cyc_Absyn_Stmt*_tmp5B6;struct Cyc_Absyn_Decl*_tmp5B5;struct Cyc_Absyn_Stmt*_tmp5B4;struct Cyc_List_List*_tmp5B3;struct Cyc_Absyn_Switch_clause**_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_List_List*_tmp5B0;void*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Stmt*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_Stmt*_tmp5A9;struct Cyc_Absyn_Exp*_tmp5A8;struct Cyc_Absyn_Stmt*_tmp5A7;struct Cyc_Absyn_Stmt*_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Stmt*_tmp5A4;struct Cyc_Absyn_Stmt*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A2;switch(*((int*)_tmp565)){case 0U: _LL1: _LL2:
# 3513
 return;case 1U: _LL3: _tmp5A2=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_LL4:
# 3515
 Cyc_Toc_exp_to_c(nv,_tmp5A2);
return;case 2U: _LL5: _tmp5A4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5A3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL6:
# 3518
 Cyc_Toc_stmt_to_c(nv,_tmp5A4);
s=_tmp5A3;
continue;case 3U: _LL7: _tmp5A5=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_LL8: {
# 3522
void*topt=0;
if(_tmp5A5 != 0){
({void*_tmpA13=Cyc_Toc_typ_to_c((void*)_check_null(_tmp5A5->topt));topt=_tmpA13;});
Cyc_Toc_exp_to_c(nv,_tmp5A5);}{
# 3528
int _tmp566=Cyc_Toc_get_npop(s);
if(_tmp566 > 0){
if(topt != 0){
struct _tuple1*_tmp567=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp568=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp567,0U),0U);
({void*_tmpA18=({struct _tuple1*_tmpA17=_tmp567;void*_tmpA16=topt;struct Cyc_Absyn_Exp*_tmpA15=_tmp5A5;Cyc_Absyn_declare_stmt(_tmpA17,_tmpA16,_tmpA15,({
struct Cyc_Absyn_Stmt*_tmpA14=Cyc_Toc_make_npop_handler(_tmp566);Cyc_Absyn_seq_stmt(_tmpA14,_tmp568,0U);}),0U);})->r;
# 3533
s->r=_tmpA18;});}else{
# 3537
Cyc_Toc_do_npop_before(_tmp566,s);}}
# 3539
return;};}case 4U: _LL9: _tmp5A8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5A7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_tmp5A6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp565)->f3;_LLA:
# 3541
 Cyc_Toc_exp_to_c(nv,_tmp5A8);
Cyc_Toc_stmt_to_c(nv,_tmp5A7);
s=_tmp5A6;
continue;case 5U: _LLB: _tmp5AA=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp565)->f1).f1;_tmp5A9=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LLC:
# 3546
 Cyc_Toc_exp_to_c(nv,_tmp5AA);{
struct _RegionHandle _tmp569=_new_region("temp");struct _RegionHandle*temp=& _tmp569;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA19=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA19,_tmp5A9);});
# 3550
_npop_handler(0U);return;
# 3547
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3552
struct Cyc_Toc_Env*_tmp56A=nv;struct _dyneither_ptr**_tmp56B;_LL22: _tmp56B=_tmp56A->break_lab;_LL23:;
if(_tmp56B != 0)
({void*_tmpA1A=Cyc_Toc_goto_stmt_r(*_tmp56B);s->r=_tmpA1A;});
# 3556
({int _tmpA1B=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA1B,s);});
return;}case 7U: _LLF: _LL10: {
# 3559
struct Cyc_Toc_Env*_tmp56C=nv;struct _dyneither_ptr**_tmp56D;_LL25: _tmp56D=_tmp56C->continue_lab;_LL26:;
if(_tmp56D != 0)
({void*_tmpA1C=Cyc_Toc_goto_stmt_r(*_tmp56D);s->r=_tmpA1C;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3565
({int _tmpA1D=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA1D,s);});
return;case 9U: _LL13: _tmp5AE=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5AD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f2).f1;_tmp5AC=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f3).f1;_tmp5AB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f4;_LL14:
# 3569
 Cyc_Toc_exp_to_c(nv,_tmp5AE);Cyc_Toc_exp_to_c(nv,_tmp5AD);Cyc_Toc_exp_to_c(nv,_tmp5AC);{
struct _RegionHandle _tmp56E=_new_region("temp");struct _RegionHandle*temp=& _tmp56E;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA1E=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA1E,_tmp5AB);});
# 3573
_npop_handler(0U);return;
# 3570
;_pop_region(temp);};case 10U: _LL15: _tmp5B1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5B0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_tmp5AF=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp565)->f3;_LL16:
# 3575
 Cyc_Toc_xlate_switch(nv,s,_tmp5B1,_tmp5B0,_tmp5AF);
return;case 11U: _LL17: _tmp5B3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5B2=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL18: {
# 3578
struct Cyc_Toc_Env*_tmp56F=nv;struct Cyc_Toc_FallthruInfo*_tmp579;_LL28: _tmp579=_tmp56F->fallthru_info;_LL29:;
if(_tmp579 == 0)
({void*_tmp570=0U;({struct _dyneither_ptr _tmpA1F=({const char*_tmp571="fallthru in unexpected place";_tag_dyneither(_tmp571,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA1F,_tag_dyneither(_tmp570,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp572=*_tmp579;struct Cyc_Toc_FallthruInfo _tmp573=_tmp572;struct _dyneither_ptr*_tmp578;struct Cyc_List_List*_tmp577;_LL2B: _tmp578=_tmp573.label;_tmp577=_tmp573.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp578,0U);
# 3584
({int _tmpA20=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA20,s2);});{
struct Cyc_List_List*_tmp574=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp577);
struct Cyc_List_List*_tmp575=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5B3);
for(0;_tmp574 != 0;(_tmp574=_tmp574->tl,_tmp575=_tmp575->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp575))->hd);
({struct Cyc_Absyn_Stmt*_tmpA23=({struct Cyc_Absyn_Stmt*_tmpA22=({struct Cyc_Absyn_Exp*_tmpA21=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->tag=5U,_tmp576->f1=(struct Cyc_Absyn_Vardecl*)_tmp574->hd;_tmp576;}),0U);Cyc_Absyn_assign_stmt(_tmpA21,(struct Cyc_Absyn_Exp*)_tmp575->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA22,s2,0U);});s2=_tmpA23;});}
# 3592
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp5B5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5B4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL1A:
# 3598
{void*_tmp57A=_tmp5B5->r;void*_tmp57B=_tmp57A;struct Cyc_Absyn_Tvar*_tmp596;struct Cyc_Absyn_Vardecl*_tmp595;struct Cyc_Absyn_Exp*_tmp594;struct Cyc_Absyn_Fndecl*_tmp593;struct Cyc_List_List*_tmp592;struct Cyc_Absyn_Pat*_tmp591;struct Cyc_Absyn_Exp*_tmp590;void*_tmp58F;struct Cyc_Absyn_Vardecl*_tmp58E;switch(*((int*)_tmp57B)){case 0U: _LL2E: _tmp58E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp58E,_tmp5B4);goto _LL2D;case 2U: _LL30: _tmp591=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_tmp590=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp57B)->f3;_tmp58F=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp57B)->f4;_LL31:
# 3606
{void*_tmp57C=_tmp591->r;void*_tmp57D=_tmp57C;struct Cyc_Absyn_Vardecl*_tmp57F;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp57D)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp57D)->f2)->r)->tag == 0U){_LL3B: _tmp57F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp57D)->f1;_LL3C:
# 3608
({struct _tuple1*_tmpA24=Cyc_Toc_temp_var();_tmp57F->name=_tmpA24;});
_tmp57F->initializer=_tmp590;
({void*_tmpA25=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->tag=0U,_tmp57E->f1=_tmp57F;_tmp57E;});_tmp5B5->r=_tmpA25;});
Cyc_Toc_local_decl_to_c(nv,_tmp57F,_tmp5B4);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3617
({void*_tmpA26=(Cyc_Toc_letdecl_to_c(nv,_tmp591,_tmp58F,(void*)_check_null(_tmp590->topt),_tmp590,_tmp5B4))->r;s->r=_tmpA26;});
goto _LL3A;}_LL3A:;}
# 3620
goto _LL2D;case 3U: _LL32: _tmp592=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_LL33: {
# 3624
struct Cyc_List_List*_tmp580=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp592);
if(_tmp580 == 0)
({void*_tmp581=0U;({struct _dyneither_ptr _tmpA27=({const char*_tmp582="empty Letv_d";_tag_dyneither(_tmp582,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA27,_tag_dyneither(_tmp581,sizeof(void*),0U));});});
({void*_tmpA28=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->tag=0U,_tmp583->f1=(struct Cyc_Absyn_Vardecl*)_tmp580->hd;_tmp583;});_tmp5B5->r=_tmpA28;});
_tmp580=_tmp580->tl;
for(0;_tmp580 != 0;_tmp580=_tmp580->tl){
struct Cyc_Absyn_Decl*_tmp584=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->tag=0U,_tmp585->f1=(struct Cyc_Absyn_Vardecl*)_tmp580->hd;_tmp585;}),0U);
({void*_tmpA2A=({struct Cyc_Absyn_Decl*_tmpA29=_tmp584;Cyc_Absyn_decl_stmt(_tmpA29,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA2A;});}
# 3633
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp593=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_LL35:
# 3636
 Cyc_Toc_fndecl_to_c(nv,_tmp593,0);
Cyc_Toc_stmt_to_c(nv,_tmp5B4);
goto _LL2D;case 4U: _LL36: _tmp596=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp57B)->f1;_tmp595=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp57B)->f2;_tmp594=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp57B)->f3;_LL37: {
# 3640
struct Cyc_Absyn_Stmt*_tmp586=_tmp5B4;
# 3642
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp595->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3649
Cyc_Toc_stmt_to_c(nv,_tmp586);
if(Cyc_Absyn_no_regions)
({void*_tmpA2E=({struct _tuple1*_tmpA2D=x_var;void*_tmpA2C=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA2B=
Cyc_Absyn_uint_exp(0U,0U);
# 3651
Cyc_Absyn_declare_stmt(_tmpA2D,_tmpA2C,_tmpA2B,_tmp586,0U);})->r;s->r=_tmpA2E;});else{
# 3653
if((unsigned int)_tmp594){
# 3655
Cyc_Toc_exp_to_c(nv,_tmp594);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA30=_tmp594;Cyc_Absyn_aggrarrow_exp(_tmpA30,({struct _dyneither_ptr*_tmp588=_cycalloc(sizeof(*_tmp588));({struct _dyneither_ptr _tmpA2F=({const char*_tmp587="h";_tag_dyneither(_tmp587,sizeof(char),2U);});*_tmp588=_tmpA2F;});_tmp588;}),0U);}),0U);
({void*_tmpA31=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp586,0U))->r;s->r=_tmpA31;});};}else{
# 3665
({void*_tmpA3E=({
struct _tuple1*_tmpA3D=rh_var;void*_tmpA3C=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA3B=({
struct Cyc_Absyn_Exp*_tmpA33=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpA33,({struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));
({struct Cyc_Absyn_Exp*_tmpA32=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp589->hd=_tmpA32;}),_tmp589->tl=0;_tmp589;}),0U);});
# 3666
Cyc_Absyn_declare_stmt(_tmpA3D,_tmpA3C,_tmpA3B,({
# 3670
struct _tuple1*_tmpA3A=x_var;void*_tmpA39=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA38=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA3A,_tmpA39,_tmpA38,({
struct Cyc_Absyn_Stmt*_tmpA37=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA34=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpA34,({struct Cyc_Absyn_Exp*_tmp58A[1U];_tmp58A[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp58A,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpA37,({
struct Cyc_Absyn_Stmt*_tmpA36=_tmp586;Cyc_Absyn_seq_stmt(_tmpA36,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA35=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpA35,({struct Cyc_Absyn_Exp*_tmp58B[1U];_tmp58B[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp58B,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3665
s->r=_tmpA3E;});}}
# 3676
return;}default: _LL38: _LL39:
({void*_tmp58C=0U;({struct _dyneither_ptr _tmpA3F=({const char*_tmp58D="bad nested declaration within function";_tag_dyneither(_tmp58D,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA3F,_tag_dyneither(_tmp58C,sizeof(void*),0U));});});}_LL2D:;}
# 3679
return;case 13U: _LL1B: _tmp5B7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5B6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL1C:
 s=_tmp5B6;continue;case 14U: _LL1D: _tmp5B9=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5B8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp565)->f2).f1;_LL1E: {
# 3682
struct _RegionHandle _tmp597=_new_region("temp");struct _RegionHandle*temp=& _tmp597;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA40=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA40,_tmp5B9);});
Cyc_Toc_exp_to_c(nv,_tmp5B8);
# 3686
_npop_handler(0U);return;
# 3682
;_pop_region(temp);}default: _LL1F: _tmp5BC=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5BB=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_tmp5BA=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp565)->f3;_LL20: {
# 3700 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3710
e_exp->topt=e_typ;{
struct _RegionHandle _tmp598=_new_region("temp");struct _RegionHandle*temp=& _tmp598;_push_region(temp);
# 3713
Cyc_Toc_stmt_to_c(nv,_tmp5BC);{
struct Cyc_Absyn_Stmt*_tmp599=({struct Cyc_Absyn_Stmt*_tmpA41=_tmp5BC;Cyc_Absyn_seq_stmt(_tmpA41,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 3718
struct Cyc_Absyn_Stmt*_tmp59A=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->tag=10U,_tmp5A1->f1=e_exp,_tmp5A1->f2=_tmp5BB,_tmp5A1->f3=_tmp5BA;_tmp5A1;}),0U);
# 3720
Cyc_Toc_stmt_to_c(nv,_tmp59A);{
# 3723
struct Cyc_Absyn_Exp*_tmp59B=({
struct Cyc_Absyn_Exp*_tmpA43=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpA43,({struct Cyc_List_List*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));
({struct Cyc_Absyn_Exp*_tmpA42=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp5A0->hd=_tmpA42;}),_tmp5A0->tl=0;_tmp5A0;}),0U);});
# 3727
struct Cyc_Absyn_Stmt*_tmp59C=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA45=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpA45,({struct Cyc_List_List*_tmp59F=_cycalloc(sizeof(*_tmp59F));
({struct Cyc_Absyn_Exp*_tmpA44=Cyc_Absyn_address_exp(h_exp,0U);_tmp59F->hd=_tmpA44;}),_tmp59F->tl=0;_tmp59F;}),0U);}),0U);
# 3731
struct Cyc_Absyn_Exp*_tmp59D=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp59E=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpA54=({
struct _tuple1*_tmpA53=h_var;void*_tmpA52=h_typ;Cyc_Absyn_declare_stmt(_tmpA53,_tmpA52,0,({
struct Cyc_Absyn_Stmt*_tmpA51=_tmp59C;Cyc_Absyn_seq_stmt(_tmpA51,({
struct _tuple1*_tmpA50=was_thrown_var;void*_tmpA4F=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpA4E=_tmp59D;Cyc_Absyn_declare_stmt(_tmpA50,_tmpA4F,_tmpA4E,({
struct Cyc_Absyn_Stmt*_tmpA4D=({struct Cyc_Absyn_Exp*_tmpA47=_tmp59B;struct Cyc_Absyn_Stmt*_tmpA46=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp59E,0U);
# 3737
Cyc_Absyn_ifthenelse_stmt(_tmpA47,_tmpA46,
# 3739
Cyc_Toc_skip_stmt_dl(),0U);});
# 3737
Cyc_Absyn_seq_stmt(_tmpA4D,({
# 3740
struct Cyc_Absyn_Exp*_tmpA4C=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpA4B=_tmp599;Cyc_Absyn_ifthenelse_stmt(_tmpA4C,_tmpA4B,({
# 3742
struct _tuple1*_tmpA4A=e_var;void*_tmpA49=e_typ;struct Cyc_Absyn_Exp*_tmpA48=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 3742
Cyc_Absyn_declare_stmt(_tmpA4A,_tmpA49,_tmpA48,_tmp59A,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3733
s->r=_tmpA54;});};};
# 3746
_npop_handler(0U);return;
# 3711
;_pop_region(temp);};}}_LL0:;}}
# 3755
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpA55=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpA55;});{
struct _RegionHandle _tmp5BD=_new_region("frgn");struct _RegionHandle*frgn=& _tmp5BD;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp5BE=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp5BF=f->args;for(0;_tmp5BF != 0;_tmp5BF=_tmp5BF->tl){
struct _tuple1*_tmp5C0=({struct _tuple1*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->f1=Cyc_Absyn_Loc_n,_tmp5C1->f2=(*((struct _tuple9*)_tmp5BF->hd)).f1;_tmp5C1;});
({void*_tmpA56=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp5BF->hd)).f3);(*((struct _tuple9*)_tmp5BF->hd)).f3=_tmpA56;});}}
# 3771
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3775
({struct Cyc_Hashtable_Table**_tmpA58=({struct Cyc_Hashtable_Table**_tmp5C2=_cycalloc(sizeof(*_tmp5C2));({struct Cyc_Hashtable_Table*_tmpA57=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp5C2=_tmpA57;});_tmp5C2;});Cyc_Toc_fn_pop_table=_tmpA58;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp5C3=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp5C4=_tmp5C3;struct _dyneither_ptr*_tmp5CD;struct Cyc_Absyn_Tqual _tmp5CC;void*_tmp5CB;int _tmp5CA;_LL1: _tmp5CD=_tmp5C4.name;_tmp5CC=_tmp5C4.tq;_tmp5CB=_tmp5C4.type;_tmp5CA=_tmp5C4.inject;_LL2:;{
void*_tmp5C5=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp5CB,Cyc_Absyn_heap_rgn_type,_tmp5CC,Cyc_Absyn_false_type));
struct _tuple1*_tmp5C6=({struct _tuple1*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->f1=Cyc_Absyn_Loc_n,_tmp5C9->f2=(struct _dyneither_ptr*)_check_null(_tmp5CD);_tmp5C9;});
({struct Cyc_List_List*_tmpA5B=({struct Cyc_List_List*_tmpA5A=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA5A,({struct Cyc_List_List*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));({struct _tuple9*_tmpA59=({struct _tuple9*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7->f1=_tmp5CD,_tmp5C7->f2=_tmp5CC,_tmp5C7->f3=_tmp5C5;_tmp5C7;});_tmp5C8->hd=_tmpA59;}),_tmp5C8->tl=0;_tmp5C8;}));});f->args=_tmpA5B;});
f->cyc_varargs=0;};}
# 3784
{struct Cyc_List_List*_tmp5CE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp5CE != 0;_tmp5CE=_tmp5CE->tl){
({void*_tmpA5C=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp5CE->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp5CE->hd)->type=_tmpA5C;});}}
# 3787
({struct Cyc_Toc_Env*_tmpA5D=Cyc_Toc_clear_toplevel(frgn,_tmp5BE);Cyc_Toc_stmt_to_c(_tmpA5D,f->body);});}
# 3764
;_pop_region(frgn);};}
# 3791
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp5CF=s;switch(_tmp5CF){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3808 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 3810
struct _tuple39 _tmp5D0=*env;struct _tuple39 _tmp5D1=_tmp5D0;struct Cyc_Toc_TocState*_tmp605;struct Cyc_Absyn_Aggrdecl*_tmp604;_LL1: _tmp605=_tmp5D1.f1;_tmp604=*_tmp5D1.f2;_LL2:;{
struct _tuple1*_tmp5D2=_tmp604->name;
struct Cyc_Toc_TocState _tmp5D3=*_tmp605;struct Cyc_Toc_TocState _tmp5D4=_tmp5D3;struct Cyc_Dict_Dict*_tmp603;_LL4: _tmp603=_tmp5D4.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple18**_tmp5D5=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp603,_tmp5D2);
if(_tmp5D5 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp604->kind == Cyc_Absyn_StructA)
({struct _tuple18*_tmpA5F=({struct _tuple18*_tmp5D6=_region_malloc(d,sizeof(*_tmp5D6));_tmp5D6->f1=_tmp604,({void*_tmpA5E=Cyc_Absyn_strctq(_tmp5D2);_tmp5D6->f2=_tmpA5E;});_tmp5D6;});v=_tmpA5F;});else{
# 3821
({struct _tuple18*_tmpA61=({struct _tuple18*_tmp5D7=_region_malloc(d,sizeof(*_tmp5D7));_tmp5D7->f1=_tmp604,({void*_tmpA60=Cyc_Absyn_unionq_type(_tmp5D2);_tmp5D7->f2=_tmpA60;});_tmp5D7;});v=_tmpA61;});}
({struct Cyc_Dict_Dict _tmpA62=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp603,_tmp5D2,v);*_tmp603=_tmpA62;});};}else{
# 3824
struct _tuple18*_tmp5D8=*_tmp5D5;struct _tuple18*_tmp5D9=_tmp5D8;struct Cyc_Absyn_Aggrdecl*_tmp5DC;void*_tmp5DB;_LL7: _tmp5DC=_tmp5D9->f1;_tmp5DB=_tmp5D9->f2;_LL8:;
if(_tmp5DC->impl == 0){
({struct Cyc_Dict_Dict _tmpA65=({struct Cyc_Dict_Dict _tmpA64=*_tmp603;struct _tuple1*_tmpA63=_tmp5D2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpA64,_tmpA63,({struct _tuple18*_tmp5DA=_region_malloc(d,sizeof(*_tmp5DA));_tmp5DA->f1=_tmp604,_tmp5DA->f2=_tmp5DB;_tmp5DA;}));});*_tmp603=_tmpA65;});
seen_defn_before=0;}else{
# 3829
seen_defn_before=1;}}{
# 3831
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->kind=_tmp604->kind,_tmp602->sc=Cyc_Absyn_Public,_tmp602->name=_tmp604->name,_tmp602->tvs=0,_tmp602->impl=0,_tmp602->expected_mem_kind=0,_tmp602->attributes=_tmp604->attributes;_tmp602;});
# 3838
if(_tmp604->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpA66=({struct Cyc_Absyn_AggrdeclImpl*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->exist_vars=0,_tmp5DD->rgn_po=0,_tmp5DD->fields=0,_tmp5DD->tagged=0;_tmp5DD;});new_ad->impl=_tmpA66;});{
# 3843
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp5DE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp604->impl))->fields;for(0;_tmp5DE != 0;_tmp5DE=_tmp5DE->tl){
# 3847
struct Cyc_Absyn_Aggrfield*_tmp5DF=(struct Cyc_Absyn_Aggrfield*)_tmp5DE->hd;
void*_tmp5E0=_tmp5DF->type;
struct Cyc_List_List*_tmp5E1=_tmp5DF->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp5E0)) && (
_tmp604->kind == Cyc_Absyn_StructA  && _tmp5DE->tl == 0  || _tmp604->kind == Cyc_Absyn_UnionA)){
# 3861 "toc.cyc"
void*_tmp5E2=Cyc_Tcutil_compress(_tmp5E0);void*_tmp5E3=_tmp5E2;void*_tmp5EB;struct Cyc_Absyn_Tqual _tmp5EA;void*_tmp5E9;unsigned int _tmp5E8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->tag == 4U){_LLA: _tmp5EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).elt_type;_tmp5EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).tq;_tmp5E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).zero_term;_tmp5E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).zt_loc;_LLB:
# 3864
({void*_tmpA68=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->tag=4U,(_tmp5E4->f1).elt_type=_tmp5EB,(_tmp5E4->f1).tq=_tmp5EA,({struct Cyc_Absyn_Exp*_tmpA67=Cyc_Absyn_uint_exp(0U,0U);(_tmp5E4->f1).num_elts=_tmpA67;}),(_tmp5E4->f1).zero_term=_tmp5E9,(_tmp5E4->f1).zt_loc=_tmp5E8;_tmp5E4;});_tmp5E0=_tmpA68;});
goto _LL9;}else{_LLC: _LLD:
# 3867
({struct Cyc_List_List*_tmpA6A=({struct Cyc_List_List*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));({void*_tmpA69=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5->tag=6U,_tmp5E5->f1=0;_tmp5E5;});_tmp5E6->hd=_tmpA69;}),_tmp5E6->tl=_tmp5E1;_tmp5E6;});_tmp5E1=_tmpA6A;});
({void*_tmpA6E=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->tag=4U,({void*_tmpA6D=Cyc_Absyn_void_star_type();(_tmp5E7->f1).elt_type=_tmpA6D;}),({
struct Cyc_Absyn_Tqual _tmpA6C=Cyc_Absyn_empty_tqual(0U);(_tmp5E7->f1).tq=_tmpA6C;}),({
struct Cyc_Absyn_Exp*_tmpA6B=Cyc_Absyn_uint_exp(0U,0U);(_tmp5E7->f1).num_elts=_tmpA6B;}),(_tmp5E7->f1).zero_term=Cyc_Absyn_false_type,(_tmp5E7->f1).zt_loc=0U;_tmp5E7;});
# 3868
_tmp5E0=_tmpA6E;});}_LL9:;}{
# 3874
struct Cyc_Absyn_Aggrfield*_tmp5EC=({struct Cyc_Absyn_Aggrfield*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->name=_tmp5DF->name,_tmp5FE->tq=Cyc_Toc_mt_tq,({
# 3876
void*_tmpA6F=Cyc_Toc_typ_to_c(_tmp5E0);_tmp5FE->type=_tmpA6F;}),_tmp5FE->width=_tmp5DF->width,_tmp5FE->attributes=_tmp5E1,_tmp5FE->requires_clause=0;_tmp5FE;});
# 3884
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp604->impl))->tagged){
void*_tmp5ED=_tmp5EC->type;
struct _dyneither_ptr*_tmp5EE=_tmp5EC->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5FB=({struct Cyc_String_pa_PrintArg_struct _tmp721;_tmp721.tag=0U,_tmp721.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp604->name).f2);_tmp721;});struct Cyc_String_pa_PrintArg_struct _tmp5FC=({struct Cyc_String_pa_PrintArg_struct _tmp720;_tmp720.tag=0U,_tmp720.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5EE);_tmp720;});void*_tmp5F9[2U];_tmp5F9[0]=& _tmp5FB,_tmp5F9[1]=& _tmp5FC;({struct _dyneither_ptr _tmpA70=({const char*_tmp5FA="_union_%s_%s";_tag_dyneither(_tmp5FA,sizeof(char),13U);});Cyc_aprintf(_tmpA70,_tag_dyneither(_tmp5F9,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));*_tmp5F8=s;_tmp5F8;});
struct Cyc_Absyn_Aggrfield*_tmp5EF=({struct Cyc_Absyn_Aggrfield*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->name=Cyc_Toc_val_sp,_tmp5F7->tq=Cyc_Toc_mt_tq,_tmp5F7->type=_tmp5ED,_tmp5F7->width=0,_tmp5F7->attributes=0,_tmp5F7->requires_clause=0;_tmp5F7;});
struct Cyc_Absyn_Aggrfield*_tmp5F0=({struct Cyc_Absyn_Aggrfield*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->name=Cyc_Toc_tag_sp,_tmp5F6->tq=Cyc_Toc_mt_tq,_tmp5F6->type=Cyc_Absyn_sint_type,_tmp5F6->width=0,_tmp5F6->attributes=0,_tmp5F6->requires_clause=0;_tmp5F6;});
struct Cyc_List_List*_tmp5F1=({struct Cyc_Absyn_Aggrfield*_tmp5F5[2U];_tmp5F5[0]=_tmp5F0,_tmp5F5[1]=_tmp5EF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5F5,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp5F2=Cyc_Toc_make_c_struct_defn(str,_tmp5F1);
({struct Cyc_List_List*_tmpA72=({struct Cyc_List_List*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));({struct Cyc_Absyn_Decl*_tmpA71=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->tag=5U,_tmp5F3->f1=_tmp5F2;_tmp5F3;}),0U);_tmp5F4->hd=_tmpA71;}),_tmp5F4->tl=Cyc_Toc_result_decls;_tmp5F4;});Cyc_Toc_result_decls=_tmpA72;});
({void*_tmpA73=Cyc_Absyn_strct(str);_tmp5EC->type=_tmpA73;});}
# 3896
({struct Cyc_List_List*_tmpA74=({struct Cyc_List_List*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->hd=_tmp5EC,_tmp5FD->tl=new_fields;_tmp5FD;});new_fields=_tmpA74;});};}}
# 3898
({struct Cyc_List_List*_tmpA75=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpA75;});};}
# 3901
if(!seen_defn_before)
({struct Cyc_List_List*_tmpA78=({struct Cyc_List_List*_tmp601=_cycalloc(sizeof(*_tmp601));({struct Cyc_Absyn_Decl*_tmpA77=({struct Cyc_Absyn_Decl*_tmp600=_cycalloc(sizeof(*_tmp600));({void*_tmpA76=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->tag=5U,_tmp5FF->f1=new_ad;_tmp5FF;});_tmp600->r=_tmpA76;}),_tmp600->loc=0U;_tmp600;});_tmp601->hd=_tmpA77;}),_tmp601->tl=Cyc_Toc_result_decls;_tmp601;});Cyc_Toc_result_decls=_tmpA78;});
return 0;};};};}
# 3905
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple40{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3933 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3936
struct _tuple40 _tmp606=*env;struct _tuple40 _tmp607=_tmp606;struct Cyc_Set_Set**_tmp618;struct Cyc_Absyn_Datatypedecl*_tmp617;_LL1: _tmp618=(_tmp607.f1)->datatypes_so_far;_tmp617=_tmp607.f2;_LL2:;{
struct _tuple1*_tmp608=_tmp617->name;
if(_tmp617->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp618,_tmp608))
return 0;
({struct Cyc_Set_Set*_tmpA79=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp618,_tmp608);*_tmp618=_tmpA79;});
{struct Cyc_List_List*_tmp609=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp617->fields))->v;for(0;_tmp609 != 0;_tmp609=_tmp609->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp609->hd;
# 3944
struct Cyc_List_List*_tmp60A=0;
int i=1;
{struct Cyc_List_List*_tmp60B=f->typs;for(0;_tmp60B != 0;(_tmp60B=_tmp60B->tl,i ++)){
struct _dyneither_ptr*_tmp60C=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp60D=({struct Cyc_Absyn_Aggrfield*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->name=_tmp60C,_tmp60F->tq=(*((struct _tuple12*)_tmp60B->hd)).f1,({
void*_tmpA7A=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp60B->hd)).f2);_tmp60F->type=_tmpA7A;}),_tmp60F->width=0,_tmp60F->attributes=0,_tmp60F->requires_clause=0;_tmp60F;});
({struct Cyc_List_List*_tmpA7B=({struct Cyc_List_List*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->hd=_tmp60D,_tmp60E->tl=_tmp60A;_tmp60E;});_tmp60A=_tmpA7B;});}}
# 3952
({struct Cyc_List_List*_tmpA7E=({struct Cyc_List_List*_tmp611=_cycalloc(sizeof(*_tmp611));({struct Cyc_Absyn_Aggrfield*_tmpA7D=({struct Cyc_Absyn_Aggrfield*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->name=Cyc_Toc_tag_sp,_tmp610->tq=Cyc_Toc_mt_tq,_tmp610->type=Cyc_Absyn_sint_type,_tmp610->width=0,_tmp610->attributes=0,_tmp610->requires_clause=0;_tmp610;});_tmp611->hd=_tmpA7D;}),({
struct Cyc_List_List*_tmpA7C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp60A);_tmp611->tl=_tmpA7C;});_tmp611;});
# 3952
_tmp60A=_tmpA7E;});{
# 3954
struct Cyc_Absyn_Aggrdecl*_tmp612=({struct _dyneither_ptr*_tmpA80=({struct _dyneither_ptr*_tmp616=_cycalloc(sizeof(*_tmp616));({struct _dyneither_ptr _tmpA7F=({const char*_tmp615="";_tag_dyneither(_tmp615,sizeof(char),1U);});*_tmp616=_tmpA7F;});_tmp616;});Cyc_Toc_make_c_struct_defn(_tmpA80,_tmp60A);});
({struct _tuple1*_tmpA81=Cyc_Toc_collapse_qvars(f->name,_tmp617->name);_tmp612->name=_tmpA81;});
({struct Cyc_List_List*_tmpA83=({struct Cyc_List_List*_tmp614=_cycalloc(sizeof(*_tmp614));({struct Cyc_Absyn_Decl*_tmpA82=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->tag=5U,_tmp613->f1=_tmp612;_tmp613;}),0U);_tmp614->hd=_tmpA82;}),_tmp614->tl=Cyc_Toc_result_decls;_tmp614;});Cyc_Toc_result_decls=_tmpA83;});};}}
# 3958
return 0;};}
# 3961
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3980 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3983
struct _tuple40 _tmp619=*env;struct _tuple40 _tmp61A=_tmp619;struct Cyc_Toc_TocState*_tmp63C;struct Cyc_Absyn_Datatypedecl*_tmp63B;_LL1: _tmp63C=_tmp61A.f1;_tmp63B=_tmp61A.f2;_LL2:;
if(_tmp63B->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp61B=*_tmp63C;struct Cyc_Toc_TocState _tmp61C=_tmp61B;struct Cyc_Dict_Dict*_tmp63A;_LL4: _tmp63A=_tmp61C.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp61D=_tmp63B->name;
{struct Cyc_List_List*_tmp61E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp63B->fields))->v;for(0;_tmp61E != 0;_tmp61E=_tmp61E->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp61E->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp620=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp61F,Cyc_Absyn_false_type,0U);
# 3994
int*_tmp621=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp63A,f->name);int*_tmp622=_tmp621;if(_tmp622 == 0){_LL7: _LL8: {
# 3996
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpA84=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpA84;});{
# 4000
struct Cyc_Absyn_Vardecl*_tmp623=Cyc_Absyn_new_vardecl(0U,f->name,_tmp620,initopt);
_tmp623->sc=f->sc;
({struct Cyc_List_List*_tmpA86=({struct Cyc_List_List*_tmp625=_cycalloc(sizeof(*_tmp625));({struct Cyc_Absyn_Decl*_tmpA85=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->tag=0U,_tmp624->f1=_tmp623;_tmp624;}),0U);_tmp625->hd=_tmpA85;}),_tmp625->tl=Cyc_Toc_result_decls;_tmp625;});Cyc_Toc_result_decls=_tmpA86;});
({struct Cyc_Dict_Dict _tmpA87=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp63A,f->name,f->sc != Cyc_Absyn_Extern);
# 4003
*_tmp63A=_tmpA87;});{
# 4005
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp626=f->typs;for(0;_tmp626 != 0;(_tmp626=_tmp626->tl,i ++)){
struct _dyneither_ptr*_tmp627=({struct _dyneither_ptr*_tmp62E=_cycalloc(sizeof(*_tmp62E));({struct _dyneither_ptr _tmpA89=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp62D=({struct Cyc_Int_pa_PrintArg_struct _tmp722;_tmp722.tag=1U,_tmp722.f1=(unsigned long)i;_tmp722;});void*_tmp62B[1U];_tmp62B[0]=& _tmp62D;({struct _dyneither_ptr _tmpA88=({const char*_tmp62C="f%d";_tag_dyneither(_tmp62C,sizeof(char),4U);});Cyc_aprintf(_tmpA88,_tag_dyneither(_tmp62B,sizeof(void*),1U));});});*_tmp62E=_tmpA89;});_tmp62E;});
struct Cyc_Absyn_Aggrfield*_tmp628=({struct Cyc_Absyn_Aggrfield*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->name=_tmp627,_tmp62A->tq=(*((struct _tuple12*)_tmp626->hd)).f1,({
void*_tmpA8A=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp626->hd)).f2);_tmp62A->type=_tmpA8A;}),_tmp62A->width=0,_tmp62A->attributes=0,_tmp62A->requires_clause=0;_tmp62A;});
({struct Cyc_List_List*_tmpA8B=({struct Cyc_List_List*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->hd=_tmp628,_tmp629->tl=fields;_tmp629;});fields=_tmpA8B;});}}
# 4013
({struct Cyc_List_List*_tmpA8F=({struct Cyc_List_List*_tmp630=_cycalloc(sizeof(*_tmp630));({struct Cyc_Absyn_Aggrfield*_tmpA8E=({struct Cyc_Absyn_Aggrfield*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->name=Cyc_Toc_tag_sp,_tmp62F->tq=Cyc_Toc_mt_tq,({
void*_tmpA8D=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp62F->type=_tmpA8D;}),_tmp62F->width=0,_tmp62F->attributes=0,_tmp62F->requires_clause=0;_tmp62F;});
# 4013
_tmp630->hd=_tmpA8E;}),({
# 4015
struct Cyc_List_List*_tmpA8C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp630->tl=_tmpA8C;});_tmp630;});
# 4013
fields=_tmpA8F;});{
# 4016
struct Cyc_Absyn_Aggrdecl*_tmp631=({struct _dyneither_ptr*_tmpA91=({struct _dyneither_ptr*_tmp635=_cycalloc(sizeof(*_tmp635));({struct _dyneither_ptr _tmpA90=({const char*_tmp634="";_tag_dyneither(_tmp634,sizeof(char),1U);});*_tmp635=_tmpA90;});_tmp635;});Cyc_Toc_make_c_struct_defn(_tmpA91,fields);});
({struct _tuple1*_tmpA92=Cyc_Toc_collapse_qvars(f->name,_tmp63B->name);_tmp631->name=_tmpA92;});
({struct Cyc_List_List*_tmpA94=({struct Cyc_List_List*_tmp633=_cycalloc(sizeof(*_tmp633));({struct Cyc_Absyn_Decl*_tmpA93=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->tag=5U,_tmp632->f1=_tmp631;_tmp632;}),0U);_tmp633->hd=_tmpA93;}),_tmp633->tl=Cyc_Toc_result_decls;_tmp633;});Cyc_Toc_result_decls=_tmpA94;});
goto _LL6;};};};}}else{if(*((int*)_tmp622)== 0){_LL9: _LLA:
# 4021
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp636=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp637=Cyc_Absyn_new_vardecl(0U,f->name,_tmp620,_tmp636);
_tmp637->sc=f->sc;
({struct Cyc_List_List*_tmpA96=({struct Cyc_List_List*_tmp639=_cycalloc(sizeof(*_tmp639));({struct Cyc_Absyn_Decl*_tmpA95=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->tag=0U,_tmp638->f1=_tmp637;_tmp638;}),0U);_tmp639->hd=_tmpA95;}),_tmp639->tl=Cyc_Toc_result_decls;_tmp639;});Cyc_Toc_result_decls=_tmpA96;});
({struct Cyc_Dict_Dict _tmpA97=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp63A,f->name,1);*_tmp63A=_tmpA97;});}
# 4028
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4032
return 0;};};}
# 4035
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4039
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4045
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpA98=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpA98;});
# 4049
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4058
struct _RegionHandle _tmp63D=_new_region("temp");struct _RegionHandle*temp=& _tmp63D;_push_region(temp);
{struct Cyc_Toc_Env*_tmp63E=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp63E,init);}
# 4059
;_pop_region(temp);}else{
# 4063
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4066
void*_tmp63F=Cyc_Tcutil_compress(old_typ);void*_tmp640=_tmp63F;void*_tmp647;struct Cyc_Absyn_Exp*_tmp646;void*_tmp645;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp640)->tag == 4U){_LL1: _tmp647=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp640)->f1).elt_type;_tmp646=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp640)->f1).num_elts;_tmp645=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp640)->f1).zero_term;_LL2:
# 4068
 if(Cyc_Tcutil_force_type2bool(0,_tmp645)){
if(_tmp646 == 0)
({void*_tmp641=0U;({struct _dyneither_ptr _tmpA99=({const char*_tmp642="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp642,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA99,_tag_dyneither(_tmp641,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp646;
struct Cyc_Absyn_Exp*_tmp643=({struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpA9B,({
struct Cyc_Absyn_Exp*_tmpA9A=num_elts;Cyc_Absyn_add_exp(_tmpA9A,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4075
struct Cyc_Absyn_Exp*_tmp644=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpA9D=({struct Cyc_Absyn_Stmt*_tmpA9C=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp643,_tmp644,0U),0U);Cyc_Toc_seq_stmt_r(_tmpA9C,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4076
s->r=_tmpA9D;});};}
# 4079
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4087
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp648=d;struct Cyc_List_List*_tmp64F;struct Cyc_List_List*_tmp64E;void**_tmp64D;struct Cyc_Tcpat_Rhs*_tmp64C;switch(*((int*)_tmp648)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp64C=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp648)->f1;_LL4:
 _tmp64C->rhs=success;return d;default: _LL5: _tmp64F=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp648)->f1;_tmp64E=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp648)->f2;_tmp64D=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp648)->f3;_LL6:
# 4092
({void*_tmpA9E=Cyc_Toc_rewrite_decision(*_tmp64D,success);*_tmp64D=_tmpA9E;});
for(0;_tmp64E != 0;_tmp64E=_tmp64E->tl){
struct _tuple32*_tmp649=(struct _tuple32*)_tmp64E->hd;struct _tuple32*_tmp64A=_tmp649;void**_tmp64B;_LL8: _tmp64B=(void**)& _tmp64A->f2;_LL9:;
({void*_tmpA9F=Cyc_Toc_rewrite_decision(*_tmp64B,success);*_tmp64B=_tmpA9F;});}
# 4097
return d;}_LL0:;}
# 4108 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4110
struct _RegionHandle _tmp650=_new_region("rgn");struct _RegionHandle*rgn=& _tmp650;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp651=Cyc_Toc_share_env(rgn,nv);
void*_tmp652=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp651,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4120
struct Cyc_Absyn_Stmt*_tmp653=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4124
({void*_tmpAA0=Cyc_Toc_rewrite_decision(dopt,_tmp653);dopt=_tmpAA0;});{
# 4126
struct Cyc_Absyn_Switch_clause*_tmp654=({struct Cyc_Absyn_Switch_clause*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->pattern=p,_tmp660->pat_vars=0,_tmp660->where_clause=0,_tmp660->body=_tmp653,_tmp660->loc=0U;_tmp660;});
struct Cyc_List_List*_tmp655=({struct _RegionHandle*_tmpAA3=rgn;struct _RegionHandle*_tmpAA2=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpAA3,Cyc_Toc_gen_labels,_tmpAA2,({struct Cyc_Absyn_Switch_clause*_tmp65F[1U];_tmp65F[0]=_tmp654;({struct _RegionHandle*_tmpAA1=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpAA1,_tag_dyneither(_tmp65F,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4129
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4133
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp651,& mydecls,& mybodies,dopt,_tmp655,v);
# 4136
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp656=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp657=_tmp656;struct Cyc_Toc_Env*_tmp659;struct Cyc_Absyn_Stmt*_tmp658;_LL1: _tmp659=_tmp657->f1;_tmp658=_tmp657->f3;_LL2:;
if(_tmp658 == _tmp653){senv=_tmp659;goto FOUND_ENV;}}
# 4141
({void*_tmp65A=0U;({struct _dyneither_ptr _tmpAA4=({const char*_tmp65B="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp65B,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA4,_tag_dyneither(_tmp65A,sizeof(void*),0U));});});
FOUND_ENV:
# 4145
 Cyc_Toc_stmt_to_c(senv,s);{
# 4147
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4149
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp65C=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpAA5=Cyc_Toc_temp_var();_tmp65C->name=_tmpAA5;});
({void*_tmpAA6=Cyc_Toc_typ_to_c_array(_tmp65C->type);_tmp65C->type=_tmpAA6;});
({struct Cyc_Absyn_Stmt*_tmpAA8=({struct Cyc_Absyn_Decl*_tmpAA7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->tag=0U,_tmp65D->f1=_tmp65C;_tmp65D;}),0U);Cyc_Absyn_decl_stmt(_tmpAA7,res,0U);});res=_tmpAA8;});}
# 4156
({struct Cyc_Absyn_Stmt*_tmpAAC=({struct _tuple1*_tmpAAB=v;void*_tmpAAA=Cyc_Toc_typ_to_c(_tmp652);struct Cyc_Absyn_Exp*_tmpAA9=e;Cyc_Absyn_declare_stmt(_tmpAAB,_tmpAAA,_tmpAA9,res,0U);});res=_tmpAAC;});{
struct Cyc_Absyn_Stmt*_tmp65E=res;_npop_handler(0U);return _tmp65E;};};};};}
# 4111
;_pop_region(rgn);}
# 4164
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp661=e->r;void*_tmp662=_tmp661;struct Cyc_Absyn_MallocInfo*_tmp68D;struct Cyc_Absyn_Stmt*_tmp68C;void**_tmp68B;void**_tmp68A;struct Cyc_List_List*_tmp689;struct Cyc_List_List*_tmp688;void**_tmp687;struct Cyc_List_List*_tmp686;void**_tmp685;struct Cyc_Absyn_Exp*_tmp684;struct Cyc_Absyn_Exp*_tmp683;struct Cyc_List_List*_tmp682;struct Cyc_Absyn_Exp*_tmp681;struct Cyc_Absyn_Exp*_tmp680;struct Cyc_Absyn_Exp*_tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_Absyn_Exp*_tmp679;struct Cyc_Absyn_Exp*_tmp678;struct Cyc_Absyn_Exp*_tmp677;struct Cyc_Absyn_Exp*_tmp676;struct Cyc_Absyn_Exp*_tmp675;struct Cyc_Absyn_Exp*_tmp674;struct Cyc_Absyn_Exp*_tmp673;struct Cyc_List_List*_tmp672;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_Absyn_Exp*_tmp670;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_Absyn_Exp*_tmp66D;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66B;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_Absyn_Exp*_tmp669;switch(*((int*)_tmp662)){case 41U: _LL1: _tmp669=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL2:
 _tmp66A=_tmp669;goto _LL4;case 20U: _LL3: _tmp66A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL4:
 _tmp66B=_tmp66A;goto _LL6;case 21U: _LL5: _tmp66B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL6:
 _tmp66C=_tmp66B;goto _LL8;case 22U: _LL7: _tmp66C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL8:
 _tmp66D=_tmp66C;goto _LLA;case 15U: _LL9: _tmp66D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LLA:
 _tmp66E=_tmp66D;goto _LLC;case 11U: _LLB: _tmp66E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LLC:
 _tmp66F=_tmp66E;goto _LLE;case 12U: _LLD: _tmp66F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LLE:
 _tmp670=_tmp66F;goto _LL10;case 18U: _LLF: _tmp670=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL10:
 _tmp671=_tmp670;goto _LL12;case 5U: _LL11: _tmp671=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp671);goto _LL0;case 3U: _LL13: _tmp672=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp672);goto _LL0;case 7U: _LL15: _tmp674=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_tmp673=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL16:
 _tmp676=_tmp674;_tmp675=_tmp673;goto _LL18;case 8U: _LL17: _tmp676=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_tmp675=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL18:
 _tmp678=_tmp676;_tmp677=_tmp675;goto _LL1A;case 9U: _LL19: _tmp678=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_tmp677=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL1A:
 _tmp67A=_tmp678;_tmp679=_tmp677;goto _LL1C;case 23U: _LL1B: _tmp67A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_tmp679=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL1C:
 _tmp67C=_tmp67A;_tmp67B=_tmp679;goto _LL1E;case 35U: _LL1D: _tmp67C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_tmp67B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL1E:
 _tmp67E=_tmp67C;_tmp67D=_tmp67B;goto _LL20;case 4U: _LL1F: _tmp67E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_tmp67D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp662)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp67E);Cyc_Toc_exptypes_to_c(_tmp67D);goto _LL0;case 6U: _LL21: _tmp681=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_tmp680=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_tmp67F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp662)->f3;_LL22:
# 4183
 Cyc_Toc_exptypes_to_c(_tmp681);Cyc_Toc_exptypes_to_c(_tmp680);Cyc_Toc_exptypes_to_c(_tmp67F);goto _LL0;case 10U: _LL23: _tmp683=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_tmp682=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL24:
# 4185
 Cyc_Toc_exptypes_to_c(_tmp683);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp682);goto _LL0;case 14U: _LL25: _tmp685=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_tmp684=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL26:
({void*_tmpAAD=Cyc_Toc_typ_to_c(*_tmp685);*_tmp685=_tmpAAD;});Cyc_Toc_exptypes_to_c(_tmp684);goto _LL0;case 25U: _LL27: _tmp687=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp662)->f1)->f3;_tmp686=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL28:
# 4188
({void*_tmpAAE=Cyc_Toc_typ_to_c(*_tmp687);*_tmp687=_tmpAAE;});
_tmp688=_tmp686;goto _LL2A;case 36U: _LL29: _tmp688=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp662)->f2;_LL2A:
 _tmp689=_tmp688;goto _LL2C;case 26U: _LL2B: _tmp689=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL2C:
# 4192
 for(0;_tmp689 != 0;_tmp689=_tmp689->tl){
struct _tuple19 _tmp663=*((struct _tuple19*)_tmp689->hd);struct _tuple19 _tmp664=_tmp663;struct Cyc_Absyn_Exp*_tmp665;_LL56: _tmp665=_tmp664.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp665);}
# 4196
goto _LL0;case 19U: _LL2D: _tmp68A=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL2E:
 _tmp68B=_tmp68A;goto _LL30;case 17U: _LL2F: _tmp68B=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL30:
({void*_tmpAAF=Cyc_Toc_typ_to_c(*_tmp68B);*_tmp68B=_tmpAAF;});goto _LL0;case 37U: _LL31: _tmp68C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp68C);goto _LL0;case 34U: _LL33: _tmp68D=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp662)->f1;_LL34:
# 4201
 if(_tmp68D->elt_type != 0)
({void**_tmpAB1=({void**_tmp666=_cycalloc(sizeof(*_tmp666));({void*_tmpAB0=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp68D->elt_type)));*_tmp666=_tmpAB0;});_tmp666;});_tmp68D->elt_type=_tmpAB1;});
Cyc_Toc_exptypes_to_c(_tmp68D->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4211
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
# 4222
({void*_tmp667=0U;({unsigned int _tmpAB3=e->loc;struct _dyneither_ptr _tmpAB2=({const char*_tmp668="Cyclone expression within C code";_tag_dyneither(_tmp668,sizeof(char),33U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpAB3,_tmpAB2,_tag_dyneither(_tmp667,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4226
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp68E=d->r;void*_tmp68F=_tmp68E;struct Cyc_Absyn_Typedefdecl*_tmp699;struct Cyc_Absyn_Enumdecl*_tmp698;struct Cyc_Absyn_Aggrdecl*_tmp697;struct Cyc_Absyn_Fndecl*_tmp696;struct Cyc_Absyn_Vardecl*_tmp695;switch(*((int*)_tmp68F)){case 0U: _LL1: _tmp695=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp68F)->f1;_LL2:
# 4229
({void*_tmpAB4=Cyc_Toc_typ_to_c(_tmp695->type);_tmp695->type=_tmpAB4;});
if(_tmp695->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp695->initializer));
goto _LL0;case 1U: _LL3: _tmp696=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp68F)->f1;_LL4:
# 4233
({void*_tmpAB5=Cyc_Toc_typ_to_c(_tmp696->ret_type);_tmp696->ret_type=_tmpAB5;});
{struct Cyc_List_List*_tmp690=_tmp696->args;for(0;_tmp690 != 0;_tmp690=_tmp690->tl){
({void*_tmpAB6=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp690->hd)).f3);(*((struct _tuple9*)_tmp690->hd)).f3=_tmpAB6;});}}
# 4237
goto _LL0;case 5U: _LL5: _tmp697=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp68F)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp697);goto _LL0;case 7U: _LL7: _tmp698=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp68F)->f1;_LL8:
# 4240
 if(_tmp698->fields != 0){
struct Cyc_List_List*_tmp691=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp698->fields))->v;for(0;_tmp691 != 0;_tmp691=_tmp691->tl){
struct Cyc_Absyn_Enumfield*_tmp692=(struct Cyc_Absyn_Enumfield*)_tmp691->hd;
if(_tmp692->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp692->tag));}}
# 4245
goto _LL0;case 8U: _LL9: _tmp699=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp68F)->f1;_LLA:
({void*_tmpAB7=Cyc_Toc_typ_to_c((void*)_check_null(_tmp699->defn));_tmp699->defn=_tmpAB7;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4255
({void*_tmp693=0U;({unsigned int _tmpAB9=d->loc;struct _dyneither_ptr _tmpAB8=({const char*_tmp694="Cyclone declaration within C code";_tag_dyneither(_tmp694,sizeof(char),34U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpAB9,_tmpAB8,_tag_dyneither(_tmp693,sizeof(void*),0U));});});goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
 goto _LL0;}_LL0:;}
# 4261
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp69A=s->r;void*_tmp69B=_tmp69A;struct Cyc_Absyn_Stmt*_tmp6B3;struct Cyc_Absyn_Stmt*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B1;struct Cyc_Absyn_Decl*_tmp6B0;struct Cyc_Absyn_Stmt*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6AE;struct Cyc_List_List*_tmp6AD;void*_tmp6AC;struct Cyc_Absyn_Exp*_tmp6AB;struct Cyc_Absyn_Exp*_tmp6AA;struct Cyc_Absyn_Exp*_tmp6A9;struct Cyc_Absyn_Stmt*_tmp6A8;struct Cyc_Absyn_Exp*_tmp6A7;struct Cyc_Absyn_Stmt*_tmp6A6;struct Cyc_Absyn_Exp*_tmp6A5;struct Cyc_Absyn_Stmt*_tmp6A4;struct Cyc_Absyn_Stmt*_tmp6A3;struct Cyc_Absyn_Exp*_tmp6A2;struct Cyc_Absyn_Stmt*_tmp6A1;struct Cyc_Absyn_Stmt*_tmp6A0;struct Cyc_Absyn_Exp*_tmp69F;switch(*((int*)_tmp69B)){case 1U: _LL1: _tmp69F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp69B)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp69F);goto _LL0;case 2U: _LL3: _tmp6A1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp69B)->f1;_tmp6A0=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp69B)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp6A1);Cyc_Toc_stmttypes_to_c(_tmp6A0);goto _LL0;case 3U: _LL5: _tmp6A2=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp69B)->f1;_LL6:
 if(_tmp6A2 != 0)Cyc_Toc_exptypes_to_c(_tmp6A2);goto _LL0;case 4U: _LL7: _tmp6A5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp69B)->f1;_tmp6A4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp69B)->f2;_tmp6A3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp69B)->f3;_LL8:
# 4267
 Cyc_Toc_exptypes_to_c(_tmp6A5);Cyc_Toc_stmttypes_to_c(_tmp6A4);Cyc_Toc_stmttypes_to_c(_tmp6A3);goto _LL0;case 5U: _LL9: _tmp6A7=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp69B)->f1).f1;_tmp6A6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp69B)->f2;_LLA:
# 4269
 Cyc_Toc_exptypes_to_c(_tmp6A7);Cyc_Toc_stmttypes_to_c(_tmp6A6);goto _LL0;case 9U: _LLB: _tmp6AB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp69B)->f1;_tmp6AA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp69B)->f2).f1;_tmp6A9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp69B)->f3).f1;_tmp6A8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp69B)->f4;_LLC:
# 4271
 Cyc_Toc_exptypes_to_c(_tmp6AB);Cyc_Toc_exptypes_to_c(_tmp6AA);Cyc_Toc_exptypes_to_c(_tmp6A9);
Cyc_Toc_stmttypes_to_c(_tmp6A8);goto _LL0;case 10U: _LLD: _tmp6AE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp69B)->f1;_tmp6AD=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp69B)->f2;_tmp6AC=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp69B)->f3;_LLE:
# 4274
 Cyc_Toc_exptypes_to_c(_tmp6AE);
for(0;_tmp6AD != 0;_tmp6AD=_tmp6AD->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp6AD->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp6B0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp69B)->f1;_tmp6AF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp69B)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp6B0);Cyc_Toc_stmttypes_to_c(_tmp6AF);goto _LL0;case 14U: _LL11: _tmp6B2=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp69B)->f1;_tmp6B1=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp69B)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp6B2);Cyc_Toc_exptypes_to_c(_tmp6B1);goto _LL0;case 13U: _LL13: _tmp6B3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp69B)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp6B3);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4284
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4287
({void*_tmpABA=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->tag=0U;_tmp69C;});s->r=_tmpABA;});
goto _LL0;default: _LL1F: _LL20:
# 4290
({void*_tmp69D=0U;({unsigned int _tmpABC=s->loc;struct _dyneither_ptr _tmpABB=({const char*_tmp69E="Cyclone statement in C code";_tag_dyneither(_tmp69E,sizeof(char),28U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpABC,_tmpABB,_tag_dyneither(_tmp69D,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4294
static void*Cyc_Toc_decl_to_enumtype(struct Cyc_Absyn_Decl*d){
void*_tmp6B4=d->r;void*_tmp6B5=_tmp6B4;struct Cyc_Absyn_Enumdecl*_tmp6BA;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6B5)->tag == 7U){_LL1: _tmp6BA=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6B5)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->tag=0U,({void*_tmpABD=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6->tag=15U,_tmp6B6->f1=_tmp6BA->name,_tmp6B6->f2=_tmp6BA;_tmp6B6;});_tmp6B7->f1=_tmpABD;}),_tmp6B7->f2=0;_tmp6B7;});}else{_LL3: _LL4:
({void*_tmp6B8=0U;({struct _dyneither_ptr _tmpABE=({const char*_tmp6B9="Toc::get_enumdecl_name";_tag_dyneither(_tmp6B9,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpABE,_tag_dyneither(_tmp6B8,sizeof(void*),0U));});});}_LL0:;}
# 4301
static struct Cyc_Absyn_Decl*Cyc_Toc_type2enumdecl(void*t){
void*_tmp6BB=t;struct Cyc_Absyn_Enumdecl*_tmp6BF;unsigned int _tmp6BE;if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6BB)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6BB)->f1)->r)->tag == 1U){_LL1: _tmp6BF=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6BB)->f1)->r)->f1;_tmp6BE=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6BB)->f1)->loc;_LL2:
# 4304
 return({struct Cyc_Absyn_Decl*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));({void*_tmpABF=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->tag=7U,_tmp6BC->f1=_tmp6BF;_tmp6BC;});_tmp6BD->r=_tmpABF;}),_tmp6BD->loc=_tmp6BE;_tmp6BD;});}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 4312
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp6C0=0U;({struct _dyneither_ptr _tmpAC0=({const char*_tmp6C1="decls_to_c: not at toplevel!";_tag_dyneither(_tmp6C1,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAC0,_tag_dyneither(_tmp6C0,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp6C2=d->r;void*_tmp6C3=_tmp6C2;struct Cyc_List_List*_tmp6DE;struct Cyc_List_List*_tmp6DD;struct Cyc_List_List*_tmp6DC;struct Cyc_List_List*_tmp6DB;struct Cyc_Absyn_Typedefdecl*_tmp6DA;struct Cyc_Absyn_Enumdecl*_tmp6D9;struct Cyc_Absyn_Datatypedecl*_tmp6D8;struct Cyc_Absyn_Aggrdecl*_tmp6D7;struct Cyc_Absyn_Fndecl*_tmp6D6;struct Cyc_Absyn_Vardecl*_tmp6D5;switch(*((int*)_tmp6C3)){case 0U: _LL1: _tmp6D5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6C3)->f1;_LL2: {
# 4320
struct _tuple1*_tmp6C4=_tmp6D5->name;
# 4322
if(_tmp6D5->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpAC2=({struct _tuple1*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));({union Cyc_Absyn_Nmspace _tmpAC1=Cyc_Absyn_Abs_n(0,1);_tmp6C5->f1=_tmpAC1;}),_tmp6C5->f2=(*_tmp6C4).f2;_tmp6C5;});_tmp6C4=_tmpAC2;});
if(_tmp6D5->initializer != 0){
if(_tmp6D5->sc == Cyc_Absyn_ExternC)_tmp6D5->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6D5->initializer));else{
# 4329
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp6D5->initializer));}}
# 4331
_tmp6D5->name=_tmp6C4;
({enum Cyc_Absyn_Scope _tmpAC3=Cyc_Toc_scope_to_c(_tmp6D5->sc);_tmp6D5->sc=_tmpAC3;});
({void*_tmpAC4=Cyc_Toc_typ_to_c(_tmp6D5->type);_tmp6D5->type=_tmpAC4;});
({struct Cyc_List_List*_tmpAC5=({struct Cyc_List_List*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6->hd=d,_tmp6C6->tl=Cyc_Toc_result_decls;_tmp6C6;});Cyc_Toc_result_decls=_tmpAC5;});
goto _LL0;}case 1U: _LL3: _tmp6D6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6C3)->f1;_LL4: {
# 4337
struct _tuple1*_tmp6C7=_tmp6D6->name;
# 4339
if(_tmp6D6->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpAC7=({struct _tuple1*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));({union Cyc_Absyn_Nmspace _tmpAC6=Cyc_Absyn_Abs_n(0,1);_tmp6C8->f1=_tmpAC6;}),_tmp6C8->f2=(*_tmp6C7).f2;_tmp6C8;});_tmp6C7=_tmpAC7;});
_tmp6D6->sc=Cyc_Absyn_Public;}
# 4343
_tmp6D6->name=_tmp6C7;
Cyc_Toc_fndecl_to_c(nv,_tmp6D6,cinclude);
({struct Cyc_List_List*_tmpAC8=({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9->hd=d,_tmp6C9->tl=Cyc_Toc_result_decls;_tmp6C9;});Cyc_Toc_result_decls=_tmpAC8;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp6CA=0U;({struct _dyneither_ptr _tmpAC9=({const char*_tmp6CB="letdecl at toplevel";_tag_dyneither(_tmp6CB,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAC9,_tag_dyneither(_tmp6CA,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp6CC=0U;({struct _dyneither_ptr _tmpACA=({const char*_tmp6CD="region decl at toplevel";_tag_dyneither(_tmp6CD,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACA,_tag_dyneither(_tmp6CC,sizeof(void*),0U));});});case 5U: _LLB: _tmp6D7=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6C3)->f1;_LLC:
# 4351
 Cyc_Toc_aggrdecl_to_c(_tmp6D7);
goto _LL0;case 6U: _LLD: _tmp6D8=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp6C3)->f1;_LLE:
# 4354
 if(_tmp6D8->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp6D8);else{
# 4357
Cyc_Toc_datatypedecl_to_c(_tmp6D8);}
goto _LL0;case 7U: _LLF: _tmp6D9=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C3)->f1;_LL10:
# 4360
 Cyc_Toc_enumdecl_to_c(_tmp6D9);
({struct Cyc_List_List*_tmpACB=({struct Cyc_List_List*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->hd=d,_tmp6CE->tl=Cyc_Toc_result_decls;_tmp6CE;});Cyc_Toc_result_decls=_tmpACB;});
goto _LL0;case 8U: _LL11: _tmp6DA=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6C3)->f1;_LL12:
# 4364
 _tmp6DA->tvs=0;
if(_tmp6DA->defn != 0){
void*_tmp6CF=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6DA->defn));
_tmp6DA->defn=_tmp6CF;{
# 4370
struct Cyc_Absyn_Decl*_tmp6D0=Cyc_Toc_type2enumdecl(_tmp6CF);
if(_tmp6D0 != 0){
({struct Cyc_List_List*_tmpACC=({struct Cyc_List_List*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->hd=_tmp6D0,_tmp6D1->tl=Cyc_Toc_result_decls;_tmp6D1;});Cyc_Toc_result_decls=_tmpACC;});
({void*_tmpACD=Cyc_Toc_decl_to_enumtype(_tmp6D0);_tmp6DA->defn=_tmpACD;});}};}else{
# 4376
enum Cyc_Absyn_KindQual _tmp6D2=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp6DA->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp6D3=_tmp6D2;if(_tmp6D3 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpACE=Cyc_Absyn_void_star_type();_tmp6DA->defn=_tmpACE;});goto _LL1F;}else{_LL22: _LL23:
 _tmp6DA->defn=Cyc_Absyn_void_type;goto _LL1F;}_LL1F:;}
# 4384
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpACF=({struct Cyc_List_List*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->hd=d,_tmp6D4->tl=Cyc_Toc_result_decls;_tmp6D4;});Cyc_Toc_result_decls=_tmpACF;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp6DB=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6C3)->f2;_LL18:
 _tmp6DC=_tmp6DB;goto _LL1A;case 10U: _LL19: _tmp6DC=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6C3)->f2;_LL1A:
 _tmp6DD=_tmp6DC;goto _LL1C;case 11U: _LL1B: _tmp6DD=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6C3)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpAD0=Cyc_Toc_decls_to_c(r,nv,_tmp6DD,top,cinclude);nv=_tmpAD0;});goto _LL0;default: _LL1D: _tmp6DE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6C3)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpAD1=Cyc_Toc_decls_to_c(r,nv,_tmp6DE,top,1);nv=_tmpAD1;});goto _LL0;}_LL0:;};}
# 4395
return nv;}
# 4399
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp6DF=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp6E0=_tmp6DF;struct Cyc_Core_DynamicRegion*_tmp6E5;_LL1: _tmp6E5=_tmp6E0.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp6E5->h;
({struct Cyc_Toc_TocState*_tmpAD2=Cyc_Toc_empty_toc_state(h);ts=_tmpAD2;});;}
({struct Cyc_Toc_TocStateWrap*_tmpAD3=({struct Cyc_Toc_TocStateWrap*_tmp6E2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6E2));_tmp6E2->dyn=_tmp6E5,_tmp6E2->state=ts;_tmp6E2;});Cyc_Toc_toc_state=_tmpAD3;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
({struct _dyneither_ptr _tmpAD4=_tag_dyneither(({unsigned int _tmp6E4=35;struct _dyneither_ptr**_tmp6E3=_cycalloc(_check_times(_tmp6E4,sizeof(struct _dyneither_ptr*)));_tmp6E3[0]=& Cyc_Toc__throw_str,_tmp6E3[1]=& Cyc_Toc_setjmp_str,_tmp6E3[2]=& Cyc_Toc__push_handler_str,_tmp6E3[3]=& Cyc_Toc__pop_handler_str,_tmp6E3[4]=& Cyc_Toc__exn_thrown_str,_tmp6E3[5]=& Cyc_Toc__npop_handler_str,_tmp6E3[6]=& Cyc_Toc__check_null_str,_tmp6E3[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp6E3[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp6E3[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp6E3[10]=& Cyc_Toc__dyneither_ptr_str,_tmp6E3[11]=& Cyc_Toc__tag_dyneither_str,_tmp6E3[12]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp6E3[13]=& Cyc_Toc__get_dyneither_size_str,_tmp6E3[14]=& Cyc_Toc__get_zero_arr_size_str,_tmp6E3[15]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp6E3[16]=& Cyc_Toc__zero_arr_plus_str,_tmp6E3[17]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp6E3[18]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp6E3[19]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp6E3[20]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp6E3[21]=& Cyc_Toc__cycalloc_str,_tmp6E3[22]=& Cyc_Toc__cyccalloc_str,_tmp6E3[23]=& Cyc_Toc__cycalloc_atomic_str,_tmp6E3[24]=& Cyc_Toc__cyccalloc_atomic_str,_tmp6E3[25]=& Cyc_Toc__region_malloc_str,_tmp6E3[26]=& Cyc_Toc__region_calloc_str,_tmp6E3[27]=& Cyc_Toc__check_times_str,_tmp6E3[28]=& Cyc_Toc__new_region_str,_tmp6E3[29]=& Cyc_Toc__push_region_str,_tmp6E3[30]=& Cyc_Toc__pop_region_str,_tmp6E3[31]=& Cyc_Toc__throw_arraybounds_str,_tmp6E3[32]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp6E3[33]=& Cyc_Toc__throw_match_str,_tmp6E3[34]=& Cyc_Toc__fast_region_malloc_str;_tmp6E3;}),sizeof(struct _dyneither_ptr*),35U);Cyc_Toc_globals=_tmpAD4;});};}
# 4448
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp6E6=ts;struct Cyc_Toc_TocStateWrap*_tmp6E7=Cyc_Toc_toc_state;ts=_tmp6E7;Cyc_Toc_toc_state=_tmp6E6;});{
struct Cyc_Toc_TocStateWrap _tmp6E8=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp6E9=_tmp6E8;struct Cyc_Core_DynamicRegion*_tmp6EF;struct Cyc_Toc_TocState*_tmp6EE;_LL1: _tmp6EF=_tmp6E9.dyn;_tmp6EE=_tmp6E9.state;_LL2:;
# 4453
{struct _RegionHandle*h=& _tmp6EF->h;
{struct Cyc_Toc_TocState _tmp6EB=*_tmp6EE;struct Cyc_Toc_TocState _tmp6EC=_tmp6EB;struct Cyc_Xarray_Xarray*_tmp6ED;_LL4: _tmp6ED=_tmp6EC.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp6ED);}
# 4454
;}
# 4457
Cyc_Core_free_rckey(_tmp6EF);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4460
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4466
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4468
({struct Cyc_Hashtable_Table**_tmpAD5=({struct Cyc_Hashtable_Table**_tmp6F0=_cycalloc(sizeof(*_tmp6F0));*_tmp6F0=pop_tables;_tmp6F0;});Cyc_Toc_gpop_tables=_tmpAD5;});
Cyc_Toc_init();
{struct _RegionHandle _tmp6F1=_new_region("start");struct _RegionHandle*start=& _tmp6F1;_push_region(start);
({struct _RegionHandle*_tmpAD7=start;struct Cyc_Toc_Env*_tmpAD6=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpAD7,_tmpAD6,ds,1,0);});;_pop_region(start);}
# 4473
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
